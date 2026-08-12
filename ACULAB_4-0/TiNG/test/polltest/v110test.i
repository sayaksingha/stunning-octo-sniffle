/* v110test.i - test code for V.110 protocol */

#ifdef TEST_STRUCTS
#ifdef TEST_STRUCTS_TX
	int dummy;
#else
	unsigned long toterrs;
#endif
#else

#include "../Testlib/errcode_sm.h"
#include "smdc_none.h"
#include "smdc_v110.h"

static int v110_conf(struct txparam *txp, struct rxparam *rxp, struct chanconf *confp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa)
{
 SMDC_V110_CONFIG_PARMS v110;
 int r;
 sa->type = kSMChannelTypeFullDuplex;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (tx)", r);
 rxp->chan = txp->chan = sa->channel;
 if (!confp->speed) confp->speed = 38400;
 memset(&v110, 0, sizeof(v110));
 v110.speed = confp->speed;
 v110.format = confp->sync ? kSMDCFormatSync : kSMDCFormatAsync;
 r = fn_smdc_channel_config(txp->chan,
	kSMDCProtocolV110, &v110, sizeof(v110),
 	kSMDCConfigEncodingNone, 0, 0);
 if (r) return printerr_sm("smdc_channel_config", r);
 rxp->maxrx = confp->maxrx * confp->speed / (confp->sync ? 8 : 10);
 return 0;
}

static const char *v110tx_start(struct txparam *txp)
{
 if (fn_smdc_line_control(txp->chan, kSMDCLineCtlCmdInitiatorConnect, 0, 0)) {
	return "control failed";
 }
 return 0;
}

static int v110tx_sts(struct txparam *txp)
{
 return simple_txsts(txp);
}

static int v110tx_upd(struct txparam *txp)
{
 unsigned char buff[256];
 patfill(buff, arlen(buff), txp);
#if 0
	// kludge for testing S/X/E bit override
 if ((txp->txlen ^ (txp->u.v110.txlen+i)) & 0x1000) {
 	const bits = kSMDCV110Aux_S1
		| kSMDCV110Aux_S3
		| kSMDCV110Aux_S4
		| kSMDCV110Aux_E4
		| kSMDCV110Aux_E5
		| kSMDCV110Aux_E6
		| kSMDCV110Aux_E7;
	fn_smdc_control(txp->chan, kSMDCLineCtlCmdSetAux, ~bits,
		txp->txlen & 0x1000 ? bits : 0);
 }
#endif
 txp->txlen += arlen(buff);
 if (fn_smdc_tx_data(txp->chan, buff, arlen(buff))) return 1;
 return 0;
}

static void v110tx_fini(struct txparam *txp)
{
 (void) txp;
}

static const char *v110rx_start(struct rxparam *rxp)
{
 if (fn_smdc_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 256, 100)) {
	return "control failed";
 }
 return 0;
}

static int v110rx_sts(struct rxparam *rxp)
{
 return simple_rxsts(rxp);
}

static int v110rx_upd(struct rxparam *rxp, unsigned char *buff, unsigned nc)
{
 rxp->u.v110.toterrs += patcheck(rxp, buff, nc, 0);
 return 0;
}

static void v110rx_fini(struct rxparam *rxp)
{
 int r;
 if (rxp->rxcnt) {
	double rxtim;
	rxtim = rxp->rxlast.millitm - rxp->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += rxp->rxlast.time - rxp->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S",
		rxp->rxcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g bytes/sec (%g bps)",
		rxp->rxcnt / rxtim, 8.0 * rxp->rxcnt / rxtim);
 } else sprintf(endstr(rxp->endmsg), "Got 0 bytes");
 if (rxp->u.v110.toterrs) sprintf(endstr(rxp->endmsg), "\nError rate: 1/%g", rxp->rxcnt / (double) rxp->u.v110.toterrs);
 r = sm_channel_release(rxp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

#endif
