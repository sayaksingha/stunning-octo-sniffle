# sccp_ansi — Test & Verification Guide
# By Abhishek chhabra

> **Scope**: End-to-end test procedures for `SccpAnsiHandler` covering two
> operating modes:
> - **Mode A** — Standard Aculab stack path (no custom routing)
> - **Mode B** — Custom SCCP routing path (ENABLE_CUSTOM_SCCP_ROUTING=1,
>   forwarded to `tcap_ansi` via IPC loopback)
>
> All steps below reference the production hardening changes made during the
> June 2026 audit. Tests are designed to be run sequentially — do not skip
> the prerequisite sections.

---

## 1. Architecture Reference

```
                  ┌────────────────────────────────────────────────────────┐
                  │                      SS7 Network                       │
                  │     (Remote ANSI Node — Server 70 or 71)               │
                  └──────────────────────┬─────────────────────────────────┘
                                         │  UDT / UDTS (wire)
                                         ▼
                           ┌─────────────────────────┐
                           │   Aculab SS7 Driver      │
                           │  (tcpss7d / ss7daemon)  │
                           └────────────┬────────────┘
                                        │  acu_sccp_ssap_msg_get()
                                        ▼
                         ┌──────────────────────────────┐
                         │  SccpAnsiHandler (ACUSCCP_N) │◄─── Sccp_N.cfg
                         │  RxThread: RxMsgFromStack()  │
                         │  TxThread: ProcessTxMsgToStack│
                         └────────┬─────────────────────┘
                                  │
             ┌────────────────────┼────────────────────────┐
             │                                             │
    ENABLE_CUSTOM_SCCP_ROUTING=0              ENABLE_CUSTOM_SCCP_ROUTING=1
             │                                             │
             ▼                                             ▼
  MSG_SCCP_DEC_Q_RCV (WrQ)              MSG_SCCP_TO_TCAP_Q (IPC loopback)
  ────────────────────────►              ──────────────────────────────►
  Downstream SCCP consumer              TcapAculabHandler CustomSccpRxThread
  (e.g. raw SCCP app)                   → ProcessRxSccpMsgFromStack()
                                        → TxMsgToApplication()
```

---

## 2. Prerequisites

### 2.1 Build Verification

```bash
# Build sccp_ansi clean
make -C /path/to/ss7/src/Aculab/sccp_ansi clean
make -C /path/to/ss7/src/Aculab/sccp_ansi

# Binary location after build
ls -lh /path/to/ss7/src/Aculab/sccp_ansi/obj/SccpAnsiHandler

# Build tcap_ansi (required for Mode B testing)
make -C /path/to/ss7/src/Aculab/tcap_ansi clean
make -C /path/to/ss7/src/Aculab/tcap_ansi
```

**Expected**: Zero errors. Warnings about unused variables in dead `#if 0` blocks are acceptable.

### 2.2 IPC Cleanup

Before every test run, flush stale IPC queues to avoid reading leftover
messages from previous sessions:

```bash
# List all IPC queues
ipcs -q

# Remove any queues with the keys from ss7.cfg:
# MSG_SCCP_HDLR_Q_RCV, MSG_SCCP_DEC_Q_RCV, MSG_SCCP_HEART_BEAT_Q_RCV
# MSG_SCCP_TO_TCAP_Q (if used for custom routing)
ipcrm -Q <key_in_hex>   # repeat for each key

# Quick nuke of all user IPC queues (use carefully on shared servers):
ipcs -q | awk 'NR>2 {print $2}' | xargs -I{} ipcrm -q {}
```

### 2.3 Configuration Files

#### `Sccp_<SSN>.cfg` — required keys

```ini
# IPC Queue keys (must match ss7.cfg)
MSG_SCCP_HDLR_Q_RCV      = 4100   # RdQ  — inbound from TCAP app
MSG_SCCP_DEC_Q_RCV       = 4101   # WrQ  — outbound to SCCP consumer / TCAP
MSG_SCCP_HEART_BEAT_Q_RCV = 4102  # HbQ  — heartbeat

# Routing config
SCCP_DESTINATION_1       = <PC_of_remote_node_A>    # e.g. 221
SCCP_DESTINATION_2       = <PC_of_remote_node_B>    # 0 = disabled
SCCP_PEG_REQUIRED        = 1      # 1 = enable pegging
SCCP_MSG_DIPLAY_PARAM    = 1      # 1 = enable detailed display in trace

# Custom routing (Mode B only — set 0 for Mode A)
ENABLE_CUSTOM_SCCP_ROUTING = 0    # 0=standard, 1=IPC loopback to tcap_ansi
```

> **Note**: `SCCP_DESTINATION_2` is optional. If the config read fails
> for it, the handler resets it to 0 and continues (single-destination mode).

#### `ss7.cfg` — shared IPC keys must be consistent

Verify that the key values in `Sccp_<SSN>.cfg` match what all other
processes (TCAP handler, application) read from `ss7.cfg`.

### 2.4 Environment Variables

```bash
# Enable full trace output — required for all test steps below
export TRACE_ACULAB_SCCP_HDLR=1

# Optional: full trace for tcap_ansi in Mode B testing
export TRACE_ACULAB_TCAP_HDLR=1
```

---

## 3. Mode A — Standard Aculab Stack Path

In this mode `ENABLE_CUSTOM_SCCP_ROUTING=0`. SCCP messages flow:

```
Remote Node → Aculab Driver → SccpAnsiHandler → MSG_SCCP_DEC_Q_RCV → App
App         → MSG_SCCP_HDLR_Q_RCV → SccpAnsiHandler → Aculab Driver → Remote Node
```

### Test A-1 — Startup and SSAP Connection

**Steps:**
```bash
# Start SCCP handler (SSN = 149 in this example)
./SccpAnsiHandler 149
```

**Expected log sequence (with trace enabled):**
```
ACUSCCP_149: Starting...
ACUSCCP_149: PegApi: ShmKey:<key> ... PegCount Shm Created
ACUSCCP_149: MSG_SCCP_HDLR_Q_RCV = 4100 -> 1004
ACUSCCP_149: MSG_SCCP_DEC_Q_RCV  = 4101 -> 1005
ACUSCCP_149: MSG_SCCP_HEART_BEAT_Q_RCV = 4102 -> 1006
ACUSCCP_149: SCCP_DESTINATION_1 = <PC>
ACUSCCP_149: Initialization Okay...
ACUSCCP_149: SSAP Connection status ... ACU_SCCP_CON_STATE_IN_SERVICE
```

**Failure indicators:**
- `CfgRead object initialization failed` → config file not found or wrong path
- `Error creating MSG_SCCP_HANDLER_Q_RCV queue` → IPC key conflict; run IPC cleanup
- Process exits immediately → check `SCCP_DESTINATION_1` is a valid non-zero PC value

---

### Test A-2 — Inbound UDT Reception

Send a UDT from the remote node (Server 70/71) addressed to the local SSN.

**Expected logs:**
```
ACUSCCP_149: Tcap Msg received Success with msgType: MSG_UNITDATA
ACUSCCP_149: RX: UDT received DataLen:<N> TransId:<hex>
ACUSCCP_149: Sccp Message wrote to the Decoder Queue Key:4101
```

**Verify with ipcs:**
```bash
# Message should appear in WrQ (MSG_SCCP_DEC_Q_RCV)
ipcs -q | grep <WrMsgQKey_in_hex>
# Column 'messages' should increment per received UDT
```

**Verify fix — ACU_SCCP_MSG_NOTICE handling (from this session's Fix 1):**

Kill the remote node's SCCP or route to a dead PC, then send a UDT. You
should now see:

```
ACUSCCP_149: SCCP NOTICE (UDTS): ReturnCause=0x03 (3) DataLen:N
             [Packet dropped by network — GTT/route failure]
```

**NOT the old silent "Freed Default Message" log (ACUSCCP37)**. If you
still see only ACUSCCP37, the fix did not compile in — rebuild.

---

### Test A-3 — SP_STATUS and USER_STATUS Handling

**Steps:**
1. Kill the remote SCCP node (bring down Server 71)
2. Wait for the SS7 driver to propagate the outage (typically 3–10 s)

**Expected logs (SP_STATUS — from original code, was working):**
```
ACUSCCP_149: Got SCCP Status Dest-1-Status:0   ← 0 = unreachable
```

**Expected logs (USER_STATUS — from this session's Fix 2):**

Restart *only* the remote SSN (not the whole node) to trigger a
subsystem-level change:

```
ACUSCCP_149: Got SCCP Status Dest-1-Status:1   ← SSN back in service
```

If `USER_STATUS` was previously commented out (which it was before Fix 2),
this log would never appear on a pure SSN restart. Confirm it now fires.

**Outbound blocking verification:**

While `mDestAStatus = 0`, have the application send a UDT via
`MSG_SCCP_HDLR_Q_RCV`. Expected:
```
ACUSCCP_149: Destination 1 is not UP status 1:0
```
And `ProcessTxMsgToStack()` returns false — **no packet sent to the
driver**. This confirms the route-gate is working correctly.

---

### Test A-4 — Load-Balancing with Two Destinations

Set `SCCP_DESTINATION_2 = <PC_B>` in `Sccp_149.cfg`.

Send 10 outbound UDTs from the application. Verify via trace logs that
the handler alternates between `mDestinationA` and `mDestinationB`
(the `mPcFlag` round-robin). Each log should show the destination PC
flipping:

```
ACUSCCP_149: sa_pc set to <PC_A>
ACUSCCP_149: sa_pc set to <PC_B>
ACUSCCP_149: sa_pc set to <PC_A>
...
```

Kill PC_B. All subsequent sends must route exclusively to PC_A without
any timeout or drop (fallback logic in `ProcessTxMsgToStack`).

---

### Test A-5 — SSAP Reconnect (Silence Watchdog — Fix 3)

This test validates the updated `GetSsapStatus()` behaviour from this
session's Fix 3: **quiet periods no longer cause false reconnects**.

**Old behaviour (before fix):** 10 seconds of no inbound messages →
triggers full SSAP teardown and reconnect even if the link is healthy.

**New behaviour (after fix):**
- Blocked state (`RX_BLOCKED` / `TX_BLOCKED`) while `IN_SERVICE` →
  **immediate** reconnect
- 30 seconds of silence → warns, but reconnects **only if** neither
  connection is `IN_SERVICE`

**Steps:**
1. Run the handler with no inbound traffic for 35 seconds
2. Verify the handler does NOT reconnect (connection stays `IN_SERVICE`)
3. Log expected:
   ```
   ACUSCCP_149: SSAP silence for 30 s: host_a=0x... host_b=0x...
   ```
   But **no** `SSAP Connection BLOCKED` log and **no** reconnect attempt.

4. Manually block the Aculab TCP connection (via iptables or by stopping
   the SS7 daemon). Verify the handler reconnects within ~5 seconds:
   ```
   ACUSCCP_149: SSAP Connection BLOCKED: host_a=0x1 host_b=0x... Triggering reconnect
   ACUSCCP_149: Reconnecting SSAP...
   ```

---

### Test A-6 — PEG_NOTICE_RCVD Counter

Verify the new peg counter (value = 95) increments on NOTICE events.

```bash
# Before triggering a NOTICE:
# Read peg shared memory tool or check via your peg display utility
<peg_display_tool> | grep 95

# Send a UDT to a dead point code to trigger UDTS/NOTICE
# Wait for the network to return the bounce-back

# After NOTICE received:
<peg_display_tool> | grep 95
# Counter should have incremented by 1
```

---

## 4. Mode B — Custom SCCP Routing Path (ENABLE_CUSTOM_SCCP_ROUTING=1)

In this mode SCCP bypasses the native Aculab TCAP stack. Messages flow:

```
Remote Node
  → Aculab Driver
  → SccpAnsiHandler RxThread (DecodeUnitData → _SccpInfo)
  → MSG_SCCP_TO_TCAP_Q (IPC loopback queue)
  → TcapAculabHandler CustomSccpRxThread
  → ProcessRxSccpMsgFromStack()
  → TxMsgToApplication()
  → Application
```

Outbound (Application → Network):
```
Application
  → MSG_TCAP_HDLR_Q_RCV (TCAP TxThread)
  → ProcessTxMsgToStack()
  → [custom path: BuildCustomSccpMsg → MSG_SCCP_HDLR_Q_RCV]
  → SccpAnsiHandler TxThread
  → Aculab Driver → Remote Node
```

### Test B-1 — Prerequisites for Mode B

Before starting, ensure:

```bash
# 1. Custom routing key exists in ss7.cfg
grep "MSG_SCCP_TO_TCAP_Q" ss7.cfg
# Should return a valid IPC key, e.g.:
# MSG_SCCP_TO_TCAP_Q = 5001

# 2. Both processes configured consistently
grep "ENABLE_CUSTOM_SCCP_ROUTING" Sccp_149.cfg   # must be 1
grep "ENABLE_CUSTOM_SCCP_ROUTING" Tcap_200.cfg   # must be 1

# 3. SSN in Tcap_200.cfg must match what sccp_ansi forwards to
grep "MSG_SCCP_TO_TCAP_Q" Tcap_200.cfg           # same key as sccp_ansi writes to
```

### Test B-2 — Startup Sequence (Order Matters)

Start processes in this order — **SCCP handler must be running before
TCAP handler** so the IPC queue exists when TCAP tries to attach:

```bash
# Terminal 1: Start sccp_ansi
export TRACE_ACULAB_SCCP_HDLR=1
./SccpAnsiHandler 149

# Wait for:
# ACUSCCP_149: Initialization Okay...
# ACUSCCP_149: SSAP ... IN_SERVICE

# Terminal 2: Start tcap_ansi
export TRACE_ACULAB_TCAP_HDLR=1
./AcuTcapHandler 200 1

# Wait for:
# ACUTCAP_200: CustomSccpRxThread: Starting custom SCCP Rx thread
# ACUTCAP_200: Initialization Okay...
```

**Failure indicators for Mode B:**
- `CustomSccpRxThread: Q_SUCCESS != ReadMsg` on first iteration →
  queue key mismatch; verify `MSG_SCCP_TO_TCAP_Q` is identical in both
  config files
- TCAP exits immediately → `ENABLE_CUSTOM_SCCP_ROUTING` is 0 in TCAP config
  while SCCP is writing to the loopback queue — queue fills up, SCCP logs
  write failures

---

### Test B-3 — Inbound UDT via Custom Path

Send a UDT from the remote node to the local SSN.

**Step 1 — Verify SCCP decodes and forwards:**
```
ACUSCCP_149: RX: UDT received DataLen:N TransId:<hex>
ACUSCCP_149: Custom routing: forwarding _SccpInfo to MSG_SCCP_TO_TCAP_Q
```

**Step 2 — Verify TCAP receives and processes:**
```
ACUTCAP_200: CustomSccpRxThread: ProcessRxSccpMsgFromStack success
ACUTCAP_200: RX: DlgId:<N> allocated for origTransId:<hex>
ACUTCAP_200: Sccp Message wrote to application queue
```

**Step 3 — Verify no double-free or stale timeout:**

After the message is processed, wait 10 seconds. You should NOT see:
```
ACUTCAP_200: ACU_TCAP_MSG_TIMEOUT for DlgId:<N>
```
This was the bug fixed earlier in this session: the custom path does not
use `acu_tcap_trans_t`, so no Aculab transaction is created and no
timeout fires. If you see a timeout, the explicit transaction cleanup
in the custom path (`acu_tcap_trans_block` + `DeleteAcuTcapTrans`) is
not working — check `ProcessTxMsgToStack` custom section.

---

### Test B-4 — Outbound UDT via Custom Path

Have the application send a response (TCAP RESPONSE/INVOKE type message)
via the standard TCAP TX queue.

**Expected SCCP logs:**
```
ACUSCCP_149: RX: Application message received from MSG_SCCP_HDLR_Q_RCV
ACUSCCP_149: ProcessTxMsgToStack: sa_pc=<destination_PC>
ACUSCCP_149: UDT sent to stack DataLen:N TransId:<hex>
```

**Expected TCAP logs:**
```
ACUTCAP_200: TX: Custom path — built _SccpInfo for SCCP queue
ACUTCAP_200: TX: Wrote to MSG_SCCP_HDLR_Q_RCV (custom routing)
```

Capture the wire on the remote node to confirm the UDT arrives with the
correct SCCP calling/called party addresses.

---

### Test B-5 — Mode B with SP_STATUS Outage (Failover)

1. Kill the remote node while in Mode B
2. Verify SCCP logs `Got SCCP Status Dest-1-Status:0`
3. Send an outbound message from the application
4. Expected: SCCP logs `Destination 1 is not UP status 1:0` and
   **does not** forward to Aculab driver
5. Restart the remote node, wait for `USER_STATUS` or `SP_STATUS` to
   fire (Fix 2 from this session — `ACU_SCCP_MSG_USER_STATUS` is now
   active)
6. Verify `mDestAStatus` is refreshed to 1 and outbound traffic resumes

---

### Test B-6 — Isolation: Mode B Does Not Affect Aculab TCAP Stack

In Mode B, the custom SCCP path and the native Aculab TCAP path must be
**completely independent**. Verify:

1. The TCAP RxThread (`RxMsgFromStack`) must still process its own
   `acu_tcap_ssap_msg_get()` events normally (CON_STATE, SP_STATUS,
   TIMEOUT) — these still come from the Aculab TCAP SSAP, not the SCCP
   IPC queue.

2. Inject a genuine TCAP message (from a different SSN/source that uses
   the native TCAP path). Verify it is processed by `RxThread` /
   `ProcessRxMsgFromStack` while `CustomSccpRxThread` is running
   simultaneously with no interference.

3. Verify that `acu_tcap_trans_unblock()` is called correctly on the
   native path regardless of what `CustomSccpRxThread` is doing. Both
   threads are independent — one must not block the other.

---

## 5. Edge Case Verification Checklist

The following table maps each production edge case from our audit to a
specific test and the expected evidence:

| # | Edge Case | Test | Evidence |
|---|---|---|---|
| 1 | Ring buffer — `acu_sccp_msg_copy_rx_buffer` not needed | A-2 | ACUSCCP10 log fires immediately after `msg_free`; no `RX_BLOCKED` under burst |
| 2 | Thread deadlock — `acu_sccp_con_unblock` | A-2 | Subsequent UDTs after first one are received without hanging |
| 3 | NOTICE/UDTS silent drop — **Fixed** | A-6 | `ReturnCause=0x03` log appears; PEG_NOTICE_RCVD (counter 95) increments |
| 4 | TCP disconnect — watchdog reconnect | A-5 | Blocked state → reconnect within 5s; 30s silence → no false reconnect |
| 5 | SP_STATUS OAM | A-3 | `Dest-1-Status:0` on PC outage; outbound blocked |
| 6 | USER_STATUS SSN — **Fixed** | A-3 | `Dest-1-Status:1` fires on SSN-only restart (was invisible before) |
| 7 | Address validity masking | A-4 | `ACU_SCCP_SA_VALID_PC` ORed correctly; verify via wire trace or Aculab debug log |
| 8 | Custom routing isolation | B-6 | Native TCAP events still processed while custom thread runs |
| 9 | Custom path no spurious timeout | B-3 | No `ACU_TCAP_MSG_TIMEOUT` after custom-path message delivery |
| 10 | Mode B failover on SP outage | B-5 | USER_STATUS refreshes mDestAStatus; outbound resumes automatically |

---

## 6. Log Codes Quick Reference

| Log Code | Meaning |
|---|---|
| `ACUSCCP10` | UDT written to decoder queue (normal Rx) |
| `ACUSCCP13` | SSAP connection failure / reconnect trigger |
| `ACUSCCP14` | Write to application queue failed |
| `ACUSCCP17` | Encode address failed |
| `ACUSCCP24` | Destination not UP — outbound blocked |
| `ACUSCCP25` | SP/SSN status update received |
| `ACUSCCP36` | **NEW** — SCCP NOTICE (UDTS) received with cause code |
| `ACUSCCP37` | Default message freed (should NOT appear for NOTICE after fix) |
| `PEG 91` | `PEG_UDT_RCVD_FROM_STACK` |
| `PEG 92` | `PEG_UDT_RCVD_FROM_APPL` |
| `PEG 93` | `PEG_UDT_SENT_TO_STACK` |
| `PEG 94` | `PEG_UDT_SENT_TO_APPL` |
| `PEG 95` | **NEW** — `PEG_NOTICE_RCVD` (UDTS bounce-back) |

---

## 7. Known Constraints and Non-Issues

| Item | Status | Reason |
|---|---|---|
| `acu_sccp_msg_copy_rx_buffer()` not called | ✅ Correct | `DecodeUnitData` deep-copies `tm_data` to stack array before IPC write. Ring buffer released before any async work. |
| Single connection (`mConnection`) per SSAP | ✅ By design | Architecture is single-connection per SSAP instance. `con_unblock` is called on every exit path. |
| 30s silence watchdog (was 10s) | ✅ Fixed | Raised threshold and added `IN_SERVICE` check to prevent false reconnects on quiet off-peak periods. |
| Timer precision for ANSI operations | ⚠️ Not applicable to sccp_ansi | `sccp_ansi` has no operation timers. This concern applies only to `tcap_ansi`. |
| `acu_tcap_transaction_restore()` | ✅ Not applicable | sccp_ansi is connectionless (UDT only). No transaction state to restore. |

---

## 8. Regression Test After Config Change

Any time `Sccp_<SSN>.cfg` is modified, restart the handler and verify
these 3 logs appear in order before any traffic is processed:

```
1. ACUSCCP_N: MSG_SCCP_HDLR_Q_RCV   = <key>  -> <hex>
2. ACUSCCP_N: SCCP_DESTINATION_1    = <PC>
3. ACUSCCP_N: Initialization Okay...
```

If any of these are missing, the config change is not reflected — the
handler may be using stale values from a previous run. Always do an
IPC cleanup (§2.2) before restarting after a config change.

---

## 9. Production Go-Live Checklist

- [ ] `SCCP_PEG_REQUIRED=1` confirmed in production config
- [ ] `SCCP_DESTINATION_1` and `SCCP_DESTINATION_2` verified against
      network provisioning
- [ ] `ENABLE_CUSTOM_SCCP_ROUTING` consistent between `Sccp_N.cfg`
      and `Tcap_M.cfg` (both 0 or both 1)
- [ ] IPC keys verified unique and not colliding with other processes
- [ ] PEG_NOTICE_RCVD (counter 95) baseline = 0 at startup confirmed
- [ ] SSAP `IN_SERVICE` confirmed before routing any live traffic
- [ ] `USER_STATUS` firing correctly verified via Test A-3
- [ ] `TRACE_ACULAB_SCCP_HDLR=0` in production (trace=1 doubles CPU overhead)
- [ ] No stale IPC queues from previous deployment (`ipcs -q` clean)

---

*Document maintained in `E:/cvs/ss7/src/Aculab/sccp_ansi/TESTING.md`.*
*Last updated: June 2026 — post production hardening audit.*
