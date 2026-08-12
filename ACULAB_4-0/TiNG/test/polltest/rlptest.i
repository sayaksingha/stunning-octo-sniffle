/* rlptest.i - test code for V.110 RLP protocol */

#ifdef TEST_STRUCTS
#ifdef TEST_STRUCTS_TX
	int dummy;
#else
		// fcnt[had data error][crc said bad]
	unsigned long zcnt;		// all-zero frames received
	unsigned long lostcnt;		// number of gaps in sequencing
	unsigned long fcnt[2][2];	// frames received
	unsigned nc;		// number of characters in curfram
	unsigned char curfram[32];
#endif
#else

#include <time.h>

#include "../Testlib/errcode_sm.h"
#include "smdc_none.h"
#include "smdc_rlp.h"

static int rlp_conf(struct txparam *txp, struct rxparam *rxp, struct chanconf *confp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa)
{
 SMDC_V110RLP_CONFIG_PARMS v110rlp;
 int r;
 sa->type = kSMChannelTypeOutput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (tx)", r);
 txp->chan = sa->channel;
 sa->type = kSMChannelTypeInput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (rx)", r);
 rxp->chan = sa->channel;
 if (!confp->speed) confp->speed = 48000;
 memset(&v110rlp, 0, sizeof(v110rlp));
 v110rlp.speed = confp->speed;
 r = fn_smdc_channel_config(txp->chan,
	kSMDCProtocolV110RLPtx, &v110rlp, sizeof(v110rlp),
 	kSMDCConfigEncodingNone, 0, 0);
 if (r) return printerr_sm("smdc_channel_config(tx)", r);
 r = fn_smdc_channel_config(rxp->chan,
	kSMDCProtocolV110RLPrx, &v110rlp, sizeof(v110rlp),
 	kSMDCConfigEncodingNone, 0, 0);
 if (r) return printerr_sm("smdc_channel_config(rx)", r);
 rxp->maxrx = confp->maxrx * confp->speed / 8;
 return 0;
}

static const char *rlptx_start(struct txparam *txp)
{
 (void) txp;
 return 0;
}

static void rlptx_fini(struct txparam *txp)
{
 int r = sm_channel_release(txp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

static int rlptx_sts(struct txparam *txp)
{
 return simple_txsts(txp);
}

static int rlptx_upd(struct txparam *txp)
{
 unsigned char buff[32];
 unsigned i;
 patfill(buff, 27, txp);
 for (i=27; i < 32; i++) buff[i] = '\377';
 if (time(0) & 1) buff[30] = '\376';
 txp->txlen += 27;
 if (fn_smdc_tx_data(txp->chan, buff, i)) return 1;
 return 0;
}

static const char *rlprx_start(struct rxparam *rxp)
{
 unsigned i;
 for (i=0; i<arlen(rxp->u.rlp.fcnt); i++) {
	unsigned j;
	for (j=0; j<arlen(rxp->u.rlp.fcnt[i]); j++) {
		rxp->u.rlp.fcnt[i][j]=0;
	}
 }
 rxp->u.rlp.zcnt=0;
 rxp->u.rlp.lostcnt=1;
 rxp->rxpat = -1;
 if (fn_smdc_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 32, 50)) {
	return "control failed";
 }
 return 0;
}

static int rlprx_sts(struct rxparam *rxp)
{
 return simple_rxsts(rxp);
}

static int rlprx_upd(struct rxparam *rxp, unsigned char *buff, unsigned nc)
{
 int ofs = 0;
 for (; nc; ) {
	unsigned int i = arlen(rxp->u.rlp.curfram) - rxp->u.rlp.nc;
	if (i > nc) i = nc;
	memcpy(rxp->u.rlp.curfram + rxp->u.rlp.nc, buff, i);
	buff += i;
	ofs += i;
	nc -= i;
	rxp->u.rlp.nc += i;
	if (rxp->u.rlp.nc < arlen(rxp->u.rlp.curfram)) break;
	for (i=0; ; i++) {
		if (i == 30) {
			// zero
			rxp->u.rlp.zcnt++;
			if (rxp->rxbusy == 'R') rxp->rxbusy = 'z';
			break;
		}
		if (rxp->u.rlp.curfram[i]) {
			unsigned errs = patcheck(rxp, rxp->u.rlp.curfram, 27, ofs - arlen(rxp->u.rlp.curfram));
			unsigned badcrc = !!(rxp->u.rlp.curfram[31] & 0x80);
			// FIXME: lost frame if it's ok but not in sequence
			rxp->u.rlp.fcnt[!!errs][badcrc]++;
			break;
		}
	}
	rxp->u.rlp.nc = 0;
	rxp->rxcnt -= 2;
 }
 return 0;
}

static void rlprx_fini(struct rxparam *rxp)
{
 int r;
 if (rxp->rxcnt) {
	double rxtim;
	const double fcnt = rxp->rxcnt / 30.0;
	rxtim = rxp->rxlast.millitm - rxp->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += rxp->rxlast.time - rxp->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %g (+%ld zero) frames in %g S",
		fcnt, rxp->u.rlp.zcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g frames/sec (%g bps)",
		fcnt / rxtim, 320 * fcnt / rxtim);
	if (rxp->u.rlp.lostcnt) sprintf(endstr(rxp->endmsg), "\nLost: %ld ", rxp->u.rlp.lostcnt);
	if (rxp->u.rlp.fcnt[0][1]
		|| rxp->u.rlp.fcnt[1][0]
		|| rxp->u.rlp.fcnt[1][1]) {
		sprintf(endstr(rxp->endmsg), "%sErrors: crc:%ld (%g) data:%ld (%g) crc&data: %ld (%g)",
			rxp->u.rlp.lostcnt ? "" : "\n",
			rxp->u.rlp.fcnt[0][1], rxp->u.rlp.fcnt[0][1] / fcnt,
			rxp->u.rlp.fcnt[1][0], rxp->u.rlp.fcnt[1][0] / fcnt,
			rxp->u.rlp.fcnt[1][1], rxp->u.rlp.fcnt[1][1] / fcnt);
	}
 } else sprintf(endstr(rxp->endmsg), "Data not counted");
 r = sm_channel_release(rxp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

#endif
