/* hdlctest.i - test code for HDLC protocol */

#ifdef TEST_STRUCTS
#ifdef TEST_STRUCTS_TX
	int dummy;
#else
		// fcnt[had data error][crc said bad]
	unsigned long fcnt[2][2];	// frames received
	unsigned char rxpat;	// next data byte expected
	unsigned nextcheck;	// next character to check in curfram
	unsigned nc;		// number of characters in curfram
	unsigned char curfram[1024];
#endif
#else

#include "../Testlib/errcode_sm.h"
#include "smdc_hdlc.h"
#include "smdc_raw.h"

static int hdlc_conf(struct txparam *txp, struct rxparam *rxp, struct chanconf *confp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa)
{
 SMDC_HDLC_FORMAT_PARMS hdlc;
 int r;
 sa->type = kSMChannelTypeOutput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (tx)", r);
 txp->chan = sa->channel;
 sa->type = kSMChannelTypeInput;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed (rx)", r);
 rxp->chan = sa->channel;
 memset(&hdlc, 0, sizeof(hdlc));
 hdlc.crc = confp->crc;
 hdlc.crcsize = confp->crcsize;
 r =  fn_smdc_channel_config(txp->chan, kSMDCProtocolRawTx, 0, 0,
 	kSMDCConfigEncodingHDLC, &hdlc, sizeof(hdlc));
 if (r) return printerr_sm("smdc_channel_config(tx)", r);
 r =  fn_smdc_channel_config(rxp->chan, kSMDCProtocolRawRx, 0, 0,
 	kSMDCConfigEncodingHDLC, &hdlc, sizeof(hdlc));
 if (r) return printerr_sm("smdc_channel_config(rx)", r);
 confp->speed = 64000;
 rxp->maxrx = confp->maxrx * 8000;
 return 0;
}

static const char *hdlctx_start(struct txparam *txp)
{
 (void) txp;
 return 0;
}

static int hdlctx_sts(struct txparam *txp)
{
 return simple_txsts(txp);
}

static int hdlctx_upd(struct txparam *txp)
{
 unsigned char buff[512];
 unsigned j;
 for (j=0; j<arlen(buff); j += 32) {
	unsigned i=0;
	buff[j + i++] = 28*8;
	buff[j + i++] = 0;
	buff[j + i++] = 0;
	buff[j + i++] = 0;
	patfill(buff+j+i, 32-i, txp);
 }
 txp->txlen += j/32*28;
 if (fn_smdc_tx_data(txp->chan, buff, j)) return 1;
 return 0;
}

static void hdlctx_fini(struct txparam *txp)
{
 int r = sm_channel_release(txp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

static const char *hdlcrx_start(struct rxparam *rxp)
{
 unsigned i;
 for (i=0; i<arlen(rxp->u.hdlc.fcnt); i++) {
	unsigned j;
	for (j=0; j<arlen(rxp->u.hdlc.fcnt[i]); j++) {
		rxp->u.hdlc.fcnt[i][j]=0;
	}
 }
 rxp->u.hdlc.nextcheck=0;
 rxp->u.hdlc.nc=0;
 rxp->u.hdlc.rxpat = 0;
 if (fn_smdc_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 512, 500)) {
	return "control failed";
 }
 return 0;
}

static int hdlcrx_sts(struct rxparam *rxp)
{
 return simple_rxsts(rxp);
}

static int hdlcrx_upd(struct rxparam *rxp, unsigned char *buff, unsigned nc)
{
 int ofs = 0;
 if (rxp->u.hdlc.nc >= sizeof(rxp->u.hdlc.curfram)) {
	fprintf(stderr, "rx frame too long\n");
	return 1;
 }
 if (!rxp->check) return 0;
 for (; nc; ) {
	unsigned int i, p0;
	i = sizeof(rxp->u.hdlc.curfram) - rxp->u.hdlc.nc;
	if (i > nc) i = nc;
	memcpy(rxp->u.hdlc.curfram + rxp->u.hdlc.nc, buff, i);
	buff += i;
	ofs += i;
	nc -= i;
	rxp->u.hdlc.nc += i;
	p0 = i;
	for (i = rxp->u.hdlc.nextcheck; i+12 <= rxp->u.hdlc.nc; i+=4) {
		if (rxp->u.hdlc.curfram[i] != 0xff) continue;
			// marker may start at [i-3..i]
		while (i>0 && rxp->u.hdlc.curfram[i-1] == 0xff) i--;
		if (rxp->u.hdlc.curfram[i+1] == 0xff
			&& rxp->u.hdlc.curfram[i+2] == 0xff
			&& rxp->u.hdlc.curfram[i+3] == 0xff) {
			if (rxp->u.hdlc.curfram[i+4] == 0xff
				&& rxp->u.hdlc.curfram[i+5] == 0xff
				&& rxp->u.hdlc.curfram[i+6] == 0xff
				&& rxp->u.hdlc.curfram[i+7] == 0xff) {	// escaped
				unsigned j;
				for (j=i+4; j < rxp->u.hdlc.nc; j++)
					rxp->u.hdlc.curfram[j-4] = rxp->u.hdlc.curfram[j];
				rxp->u.hdlc.nc -= 4;
				rxp->rxcnt -= 4; // not counted as received
			} else {	// it's end of frame
				static char eof[8] = "\076\0\0\0\0\0\0\0";
				int badcrc=0;
				unsigned j;
				unsigned errs = patcheck(rxp, rxp->u.hdlc.curfram, i, ofs - i);
				i += 4;	// skip 0xffffffff
				for (j=0; j<8; i++, j++) {
					if (rxp->u.hdlc.curfram[i] != eof[j]) {
						if (rxp->check > 1) {
							if (!errs) printf("Error: ");
							printf(" %02x!=%02x",
								rxp->u.hdlc.curfram[i], eof[j]);
						}
						if (j == 4 && rxp->u.hdlc.curfram[i] & 0x02) {
							badcrc=1;
						} else errs++;
					}
				}
				if (errs || badcrc) {
					if (rxp->check > 1) printf("\n");
				}
				rxp->u.hdlc.fcnt[!!errs][badcrc]++;
				if (errs) rxp->rxbusy = 'r';
				else if (badcrc) rxp->rxbusy = 'c';
				rxp->rxcnt -= 12;
				p0 -= 12;
				for (j=0; i < rxp->u.hdlc.nc; j++, i++)
					rxp->u.hdlc.curfram[j] = rxp->u.hdlc.curfram[i];
				rxp->u.hdlc.nc = j;
				i = -4;
			}
		}
	}
	rxp->u.hdlc.nextcheck = i;
 }
 return 0;
}

static void hdlcrx_fini(struct rxparam *rxp)
{
 int r;
 if (rxp->rxcnt) {
	double rxtim;
	const double rxfram = rxp->u.hdlc.fcnt[0][0]
		+ rxp->u.hdlc.fcnt[0][1]
		+ rxp->u.hdlc.fcnt[1][0]
		+ rxp->u.hdlc.fcnt[1][1];
	rxtim = rxp->rxlast.millitm - rxp->rxstart.millitm;
	rxtim /= 1000.0;
	rxtim += rxp->rxlast.time - rxp->rxstart.time;
	sprintf(endstr(rxp->endmsg), "Got %ld bytes in %g S",
		rxp->rxcnt,
		rxtim);
	if (rxtim) sprintf(endstr(rxp->endmsg), " = %g bytes/sec (%g bps, %gfps)",
		rxp->rxcnt / rxtim, 8.0 * rxp->rxcnt / rxtim, rxfram/rxtim);
	if (rxfram != rxp->u.hdlc.fcnt[0][0]) {
		sprintf(endstr(rxp->endmsg), "\nErrors: crc:%ld (%g) data:%ld (%g) crc&data: %ld (%g)",
			rxp->u.hdlc.fcnt[0][1], rxp->u.hdlc.fcnt[0][1] / rxfram,
			rxp->u.hdlc.fcnt[1][0], rxp->u.hdlc.fcnt[1][0] / rxfram,
			rxp->u.hdlc.fcnt[1][1], rxp->u.hdlc.fcnt[1][1] / rxfram);
	}
 } else sprintf(endstr(rxp->endmsg), "Got 0 bytes");
 r = sm_channel_release(rxp->chan);
 if (r) printerr_sm("sm_channel_release", r);
}

#endif
