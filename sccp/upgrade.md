# Release Notes — ANSI SS7 Handlers
## `sccp_ansi` · `tcap_ansi` · `include/Ss7Structs.h`
### Release Date: June 13–14, 2026

---

## Summary

This release delivers **five production-hardening changes** across the ANSI
SCCP and TCAP handler stack, plus one **critical bug fix** for a
silent-corruption defect in the ANSI Private opcode serialization path.
All changes are guarded by configuration flags where appropriate, and are
**backward-compatible** — no API changes, no IPC key changes, no struct
size changes that break existing binaries consuming `AnsiTcapMsg` or
`_SccpInfo` without recompile.

> **`Ss7Structs.h` consumers must recompile.** `TCAPOperation` gained a
> 2-byte field (`wireOpCode[2]`). Any object file linking against a struct
> that embeds `TCAPOperation` must be rebuilt.

---

## Files Modified

| File | Component | Type |
|---|---|---|
| `Aculab/sccp_ansi/src/SccpAculabHandler.cc` | sccp_ansi | Bug fix + Feature |
| `Aculab/sccp_ansi/src/SccpAculabApi.cc` | sccp_ansi | Bug fix |
| `Aculab/sccp_ansi/include/SccpAculabConstDef.h` | sccp_ansi | Feature |
| `Aculab/sccp_ansi/include/SccpAculabApi.h` | sccp_ansi | Feature |
| `Aculab/sccp_ansi/src/SccpAculabHandlerMain.cc` | sccp_ansi | Feature |
| `Aculab/tcap_ansi/src/TcapAculabHandlerMain.cc` | tcap_ansi | Feature |
| `Aculab/tcap_ansi/include/TcapAculabHandler.h` | tcap_ansi | Feature |
| `Aculab/tcap_ansi/src/TcapAculabHandler.cc` | tcap_ansi | Feature |
| `Aculab/tcap_ansi/src/TcapAculabApi.cc` | tcap_ansi | Critical bug fix |
| `include/Ss7Structs.h` | Shared | Critical bug fix |

---

## Changes — `sccp_ansi`

---

### [FIX-1] `ACU_SCCP_MSG_NOTICE` — Silent UDTS Drop Eliminated

**Files:** `SccpAculabHandler.cc`, `SccpAculabConstDef.h`

**Problem:**
The `ACU_SCCP_MSG_NOTICE` case (SCCP UDTS — network-returned message) was
handled by the `default` branch, which only logged "Freed Default Message"
(ACUSCCP37) and silently discarded the packet. There was no visibility into
why messages were being returned by the network, no cause-code logging, and
no pegging.

**Root cause:** Network route failures, GTT translation misses, and dead
point codes all return UDTS to the sender. Without logging the return cause
code, diagnosing routing faults in production was impossible.

**Fix:**
- Added an explicit `ACU_SCCP_MSG_NOTICE` case in `ProcessRxMsgFromStack()`
- Logs the SCCP return cause code from `acu_sccp_msg_get_notice_cause()`
- Pegs a new dedicated counter `PEG_NOTICE_RCVD` (value `95`)

**Config:** Always active. No config flag required.

**New peg counter added to `SccpAculabConstDef.h`:**
```cpp
PEG_NOTICE_RCVD = 95   // Increments on every ACU_SCCP_MSG_NOTICE received
```

**Log emitted on NOTICE:**
```
ACUSCCP_N: SCCP NOTICE (UDTS): ReturnCause=0x<N> (<N>) DataLen:<N>
           [Packet dropped by network — GTT/route failure]
```

---

### [FIX-2] `ACU_SCCP_MSG_USER_STATUS` — SSN-Level Outage Detection

**File:** `SccpAculabHandler.cc`

**Problem:**
`ACU_SCCP_MSG_USER_STATUS` was commented out in the message handler switch.
Only `ACU_SCCP_MSG_SP_STATUS` (whole Signalling Point outage) was active.
This meant that a **subsystem-level** restart (SSN comes back in service
without a full node restart) did not update `mDestAStatus`/`mDestBStatus`,
leaving the handler permanently blocking outbound traffic until a full
SP_STATUS event fired.

**Fix:**
- Uncommented and merged `ACU_SCCP_MSG_USER_STATUS` into the same handler
  block as `ACU_SCCP_MSG_SP_STATUS`
- Both now call `GetAcuSccpConState()` and update destination status
- SSN-only restarts now correctly re-enable outbound routing

**Impact:** Eliminates the manual workaround of restarting `SccpAnsiHandler`
after a remote SSN restart.

---

### [FIX-3] `GetSsapStatus()` — False Reconnect on Quiet Periods

**File:** `SccpAculabApi.cc`

**Problem:**
The original silence watchdog triggered a full SSAP teardown and reconnect
after 10 seconds of no inbound messages. On idle off-peak traffic periods
(e.g. 03:00–06:00) this caused unnecessary reconnect storms, brief
outage windows, and alarm noise.

**Root cause:** The watchdog checked only elapsed time (`lDiffTime >= 10.0`)
without validating whether the link was actually healthy. A perfectly
`IN_SERVICE` SSAP would be torn down purely due to silence.

**Fix:** Reconnect is now triggered **only** if one or both of these
conditions are true:
1. A connection that reports `IN_SERVICE` simultaneously reports
   `RX_BLOCKED` or `TX_BLOCKED` (genuine flow control failure)
2. Elapsed silence exceeds **30 seconds** AND **neither** host connection
   is `IN_SERVICE` (genuine link loss)

A quiet but healthy link now generates a warning log only — no reconnect.

**Thresholds:**
| Condition | Old Behaviour | New Behaviour |
|---|---|---|
| 10 s silence on healthy link | Reconnect | No action |
| 30 s silence on healthy link | Reconnect | Warning log only |
| `RX_BLOCKED` / `TX_BLOCKED` while `IN_SERVICE` | Reconnect after timeout | Immediate reconnect |
| Neither host `IN_SERVICE` for 30 s | (same) | Reconnect |

---

### [FEATURE] Custom SCCP Routing Mode (IPC Loopback to `tcap_ansi`)

**Files:** `SccpAculabHandler.cc`, `SccpAculabApi.cc`, `SccpAculabApi.h`,
`SccpAculabHandlerMain.cc`

**Purpose:**
Enables `sccp_ansi` to bypass the native Aculab TCAP stack and forward
decoded `_SccpInfo` structs directly to `tcap_ansi` via an IPC loopback
queue. This supports deployment topologies where SCCP routing must be
managed at the application layer while TCAP processing is handled by
`tcap_ansi`.

**How it works:**

- **Inbound (Network → Application):**
  `SccpAnsiHandler` decodes the UDT, populates `_SccpInfo`, and writes
  it to `MSG_SCCP_TO_TCAP_Q` (IPC loopback) instead of `MSG_SCCP_DEC_Q_RCV`.
  `TcapAculabHandler`'s `CustomSccpRxThread` reads from this queue and
  processes the message as if it arrived from the native TCAP SSAP.

- **Outbound (Application → Network):**
  `TcapAculabHandler` builds a `_SccpInfo`, writes it to
  `MSG_SCCP_HDLR_Q_RCV`. `SccpAnsiHandler`'s `TxThread` reads it and
  calls `acu_sccp_unitdata_request()` to send via the Aculab driver.

**Activation (both processes must match):**
```ini
# Sccp_<SSN>.cfg
ENABLE_CUSTOM_SCCP_ROUTING = 1   # 0 = standard, 1 = IPC loopback

# Tcap_<SSN>.cfg
ENABLE_CUSTOM_SCCP_ROUTING = 1   # must match sccp_ansi config
```

**Key constraint:** `SccpAnsiHandler` must be started **before**
`AcuTcapHandler` when custom routing is enabled, so the IPC queue exists
before TCAP tries to attach to it.

**Guard:** This feature is completely inactive when
`ENABLE_CUSTOM_SCCP_ROUTING = 0`. Standard Aculab TCAP stack path is
unchanged.

---

## Changes — `tcap_ansi`

---

### [CRITICAL BUG FIX] Lazy Serialization Dangling Pointer — ANSI Private Opcode Corruption

**Files:** `TcapAculabApi.cc`, `include/Ss7Structs.h`

**Severity:** **Critical / Silent Data Corruption**

**Problem:**
In `AddAcuTcapComponet()` (`TcapAculabApi.cc`), the ANSI Private opcode
path declared a 2-byte array on the local stack and passed its address to
`acu_tcap_msg_add_comp_invoke()`:

```cpp
// DANGEROUS — was in production
UINT8 ansi_priv_op[2];
ansi_priv_op[0] = (operationCode >> 8) & 0xFF;  // Family
ansi_priv_op[1] = operationCode & 0xFF;           // Specifier
pOpCode = ansi_priv_op;  // raw pointer to stack memory

acu_tcap_msg_add_comp_invoke(lMsg, ..., pOpCode, 2, ...);
// AddAcuTcapComponet() returns here
// ansi_priv_op[] is now DESTROYED — stack frame gone
// ...
// LATER in caller:
acu_tcap_msg_send(lMsg);  // reads the dead pointer → GARBAGE on wire
```

**Root cause:**
The Aculab library's `acu_tcap_msg_add_comp_invoke()` is **lazy** — it
stores the `op_code` **pointer** internally without copying the bytes. The
actual BER encoding only happens when `acu_tcap_msg_send()` is called from
the calling function's context. By that point, `ansi_priv_op[]` has left
scope and its stack memory has been recycled.

**Consequence:**
Every TCAP INVOKE sent with a Private opcode (`isPrivate == true`) had
corrupted Family and/or Specifier bytes on the wire. The remote node
received a malformed component and either silently dropped it or responded
with a REJECT. The corruption was intermittent — under light load the
compiler might not recycle the stack bytes immediately, making it hard to
reproduce consistently.

**Fix:**
Added `UINT8 wireOpCode[2]` to `TCAPOperation` in `Ss7Structs.h`. This
field lives inside `AnsiTcapMsg` on the **caller's** (`ProcessTxMsgToStack`)
stack frame, which is alive for the full `AddAcuTcapComponet() → acu_tcap_msg_send()`
sequence.

```cpp
// In Ss7Structs.h — TCAPOperation struct
UINT8 wireOpCode[2];  // [0]=Family, [1]=Specifier — valid when isPrivate==true

// In TcapAculabApi.cc — AddAcuTcapComponet() SAFE path
TCAPOperation& lOp = lTcapMsg.tcapComponent.tcapInvokeComp.operation;
UINT8* pOpCode   = NULL;
int lOpCodeOrLen = lOp.operationCode;  // National: integer opcode

if (lOp.isPrivate) {
    lOp.wireOpCode[0] = static_cast<UINT8>((lOp.operationCode >> 8) & 0xFF);
    lOp.wireOpCode[1] = static_cast<UINT8>(lOp.operationCode & 0xFF);
    pOpCode      = lOp.wireOpCode;   // points into caller's AnsiTcapMsg — safe
    lOpCodeOrLen = 2;
}
acu_tcap_msg_add_comp_invoke(lMsg, ..., pOpCode, lOpCodeOrLen, ...);
```

The same fix was applied to the disabled `#if 0` block for consistency,
so re-enabling it in the future will not reintroduce the bug.

**Recompile requirement:**
`TCAPOperation` in `Ss7Structs.h` gained 2 bytes. Any translation unit
that includes `Ss7Structs.h` (directly or transitively via `TcapStructs.h`)
must be recompiled. The struct layout change affects `sizeof(TcapInvokeComp)`,
`sizeof(AnsiTcapComponent)`, and `sizeof(AnsiTcapMsg)`. A `DEBUG: sizeof(TcapMsg)`
print at startup confirms the new size.

---

### [FEATURE] Custom SCCP Rx Thread in `tcap_ansi`

**Files:** `TcapAculabHandlerMain.cc`, `TcapAculabHandler.h`,
`TcapAculabHandler.cc`

**Purpose:**
Counterpart to the `sccp_ansi` custom routing feature. When enabled,
`TcapAculabHandler` spawns a dedicated `CustomSccpRxThread` that reads
decoded `_SccpInfo` structs from the `MSG_SCCP_TO_TCAP_Q` IPC loopback
queue and processes them through `ProcessRxSccpMsgFromStack()`.

**Key design decisions:**
- The custom thread is **completely independent** of `RxThread`. Native
  Aculab TCAP SSAP events (`acu_tcap_ssap_msg_get`) continue processing
  in `RxThread` without any interference.
- No `acu_tcap_trans_t` is created for custom-path messages. The handler
  uses the `_SccpInfo.udt.transInfo` fields directly for dialogue mapping.
  This avoids spurious `ACU_TCAP_MSG_TIMEOUT` events on the Aculab transaction
  queue — which was the bug triggering the original timeout issues in the
  field.
- `acu_tcap_trans_unblock()` and `DeleteAcuTcapTrans()` are **not** called
  on the custom path (there is no transaction to unblock).

**Activation:** `ENABLE_CUSTOM_SCCP_ROUTING = 1` in `Tcap_<SSN>.cfg`.
When set to `0`, `CustomSccpRxThread` is not created and the code path is
entirely inactive.

---

## Backward Compatibility

| Consumer | Impact | Action Required |
|---|---|---|
| Standard `tcap_ansi` with `ENABLE_CUSTOM_SCCP_ROUTING=0` | None | Recompile only (due to `Ss7Structs.h`) |
| Standard `sccp_ansi` with `ENABLE_CUSTOM_SCCP_ROUTING=0` | None | Recompile only |
| Any binary linking `libAculabApi.a` or `libSccpAculabApi.a` | Possible ABI break | Recompile all |
| Application code using `TCAPOperation` by value or in structs | Size change | Recompile all |
| Application code using `_SccpInfo` | No change | None |
| Application code using `AnsiTcapMsg` | Size change via `TCAPOperation` | Recompile all |

---

## Verification

```bash
# Confirm struct size change (look for updated sizeof output at startup)
./AcuTcapHandler <ssn> <instance>
# Expect: DEBUG: sizeof(TcapMsg) in handler = <new_value>

# Build both components clean
make -C Aculab/sccp_ansi clean && make -C Aculab/sccp_ansi
make -C Aculab/tcap_ansi clean && make -C Aculab/tcap_ansi

# Full test procedures → see Aculab/sccp_ansi/TESTING.md
```

---

## Known Issues / Not Addressed

| Item | Decision |
|---|---|
| Timer drift precision in `tcap_ansi` (Aculab 1s resolution) | Not addressed — no sub-3s SLA requirement confirmed |
| `acu_tcap_msg_copy_rx_buffer()` | Not required — all RX data deep-copied before `msg_free()` |
| Connection-oriented SCCP (Class 2/3) | Out of scope — `sccp_ansi` is Class 0 UDT only |

---

*Maintained in `E:/cvs/ss7/src/Aculab/sccp_ansi/upgrade.md`*
