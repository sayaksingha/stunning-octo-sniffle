/* rawtest.i - test code for raw data */

#ifdef TEST_STRUCTS
#ifdef TEST_STRUCTS_TX
	int dummy;
#else
	unsigned long toterrs;
#endif
#else

#include "../Testlib/errcode_sm.h"
#include "smdc_raw.h"
#include "smdc_sync.h"

static int raw_conf(struct txparam *txp, struct rxparam *rxp, struct
chanconf *confp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa)
{
 int r;
 sa->type = kSMChannelTypeOutput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (tx)", r);
 txp->chan = sa->channel;
 sa->type = kSMChannelTypeInput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (rx)", r);
 rxp->chan = sa->channel;
 r =  fn_smdc_channel_config(txp->chan, kSMDCProtocolRawTx, 0, 0,
 	kSMDCConfigEncodingSync, 0, 0);
 if (r) return printerr_sm("smdc_channel_config(tx)", r);
 r =  fn_smdc_channel_config(rxp->chan, kSMDCProtocolRawRx, 0, 0,
 	kSMDCConfigEncodingSync, 0, 0);
 if (r) return printerr_sm("smdc_channel_config(rx)", r);
 confp->speed = 64000;
 rxp->maxrx = confp->maxrx * 8000;
 return 0;
}


static const char *rawtx_start(struct txparam *txp)
{
 (void) txp;
 return 0;
}

static int rawtx_sts(struct txparam *txp)
{
 return simple_txsts(txp);
}

static int rawtx_upd(struct txparam *txp)
{
 unsigned char buff[512];
 patfill(buff, arlen(buff), txp);
 txp->txlen += arlen(buff);
 if (fn_smdc_tx_data(txp->chan, buff, arlen(buff))) return 1;
 return 0;
}

static void rawtx_fini(struct txparam *txp)
{
 int r = sm_channel_release(txp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

static const char *rawrx_start(struct rxparam *rxp)
{
 if (fn_smdc_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 512, 50)) {
	return "control failed";
 }
 return 0;
}

static int rawrx_sts(struct rxparam *rxp)
{
 return simple_rxsts(rxp);
}

static int rawrx_upd(struct rxparam *rxp, unsigned char *buff, unsigned nc)
{
 rxp->u.raw.toterrs += patcheck(rxp, buff, nc, 0);
 return 0;
}

static void rawrx_fini(struct rxparam *rxp)
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
 if (rxp->u.raw.toterrs) sprintf(endstr(rxp->endmsg), "\nError rate: 1/%g", rxp->rxcnt / (double) rxp->u.raw.toterrs);
 r = sm_channel_release(rxp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

#endif
