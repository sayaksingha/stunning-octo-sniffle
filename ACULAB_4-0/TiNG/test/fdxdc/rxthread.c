#include "fdxdc.h"
#include <stdio.h>
#include <string.h>
#include "../Testlib/errcode_sm.h"

#include "../libutil/v18a.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	FILE *of;
	unsigned col;
} OUTF;

static void cput(OUTF *of, char c)
{
 putc(c, of->of);
 if (c == '\n') of->col = 0;
 else of->col++;
}

static int flushof(OUTF *of)
{
 if (of->col) {
 	putc('\n', of->of);
 	of->col = 0;
 }
 return 0;
}

typedef struct {
	SMDC_LINE_STATUS_PARMS ls;
	SMDC_RX_STATUS_PARMS rxs;
} STATUS_HIST;

static void showstatus(tSMChannelId chan, STATUS_HIST *hp)
{
 SMDC_LINE_STATUS_PARMS ls;
 SMDC_RX_STATUS_PARMS rxs;
 int r;
 memset(&rxs, 0, sizeof(rxs));
 memset(&ls, 0, sizeof(ls));
 rxs.channel = chan;
 ls.channel = chan;
 r = smdc_rx_status(&rxs);
 if (r) {
	printerr_sm("smdc_rx_status", r);
	return;
 }
 r = smdc_line_status(&ls);
 if (r) {
	printerr_sm("smdc_line_status", r);
	return;
 }
 printf("sts:");
 if (ls.link_status != hp->ls.link_status) {
	printf(" line:link->");
	switch (ls.link_status) {
	case kSMDCLinkStatusNotConnected: printf("NotConnected"); break;
	case kSMDCLinkStatusConnecting: printf("Connecting"); break;
	case kSMDCLinkStatusConnected: printf("Connected"); break;
	case kSMDCLinkStatusDisconnecting: printf("Disconnecting"); break;
	default: printf("?(%d)", ls.link_status);
	}
	hp->ls.link_status = ls.link_status;
 }
 if (ls.rx_status != hp->ls.rx_status) {
	printf(" line:rx->");
	switch (ls.rx_status) {
	case kSMDCRxStatusNoCarrier: printf("NoCarrier"); break;
	case kSMDCRxStatusCarrierPresent: printf("Carrier"); break;
	case kSMDCRxStatusReceivingData: printf("Data"); break;
	default: printf("?(%d)", ls.rx_status);
	}
	hp->ls.rx_status = ls.rx_status;
 }
 if (ls.tx_status != hp->ls.tx_status) {
	printf(" line:tx->");
	switch (ls.tx_status) {
	case kSMDCTxStatusSilent: printf("Silent"); break;
	case kSMDCTxStatusSendingCarrier: printf("Carrier"); break;
	case kSMDCTxStatusSendingData: printf("Data"); break;
	default: printf("?(%d)", ls.tx_status);
	}
	hp->ls.tx_status = ls.tx_status;
 }
 if (ls.tx_cts != hp->ls.tx_cts) {
	printf(" line:tx_cts->%d", ls.tx_cts);
	hp->ls.tx_cts = ls.tx_cts;
 }
 if (rxs.status != hp->rxs.status) {
	printf(" rx:status->");
	switch (rxs.status) {
	case kSMDCRxStatusEmpty: printf("Empty"); break;
	case kSMDCRxStatusData: printf("Data"); break;
	case kSMDCRxStatusOverrun: printf("Overrun"); break;
	case kSMDCRxStatusCarrier: printf("Carrier"); break;
	case kSMDCRxStatusFinished: printf("Finished"); break;
	default: printf("?(%d)", rxs.status);
	}
	hp->rxs.status = rxs.status;
 }
 if (rxs.available_octets != hp->rxs.available_octets) {
	printf(" rx:avail->%d", rxs.available_octets);
	hp->rxs.available_octets = rxs.available_octets;
 }
 if (rxs.flow != hp->rxs.flow) {
	printf(" rx:flow->%d", rxs.flow);
	hp->rxs.flow = rxs.flow;
 }
 if (rxs.status == kSMDCRxStatusCarrier && rxs.u.carrier.carrier != hp->rxs.u.carrier.carrier) {
	 printf(" rx:carrier->%d", rxs.u.carrier.carrier);
	 hp->rxs.u.carrier.carrier = rxs.u.carrier.carrier;
 }
 printf("\n");
}

static int showchar(OUTF *of, V18ASTATE *vs, int c, char *format)
{
 switch (*format) {
 default:
 	printf("[Format='%s' - no output]\n", format);
	break;
 case 0:
 case 'c':
	if (c >= 128) {
		cput(of, '~');
		c &= 0x7f;
	}
	if (c < ' ') {
		cput(of, '^');
		c += 'A'-1;
	}
	if (c == '~' || c == '^' || c == '\\') {
		cput(of, '\\');
	}
	cput(of, c);
	break;
 case 'x':
	fprintf(of->of, "%02x ", c);
	of->col += 3;
	break;
 case 'r':
 	putc(c, of->of);
 	return 0;
 case 'v':
 	v18a_show((void (*)(void *, char))cput, of, vs, c);
 }
 if (of->col >= 64) {
 	flushof(of);
 }
 return 0;
}

static int showbits(OUTF *of, V18ASTATE *vs, unsigned long v, unsigned bits, char *format)
{
 for (; bits >= 8; bits -= 8, v >>= 8) {
	showchar(of, vs, v & 0xff, format);
 }
 if (bits) {
	flushof(of);
	fprintf(of->of, "\\[bits:");
	of->col += 7;
	for (; bits; bits--) {
		putc('0' + (v & 1), of->of);
		of->col++;
		v >>= 1;
	}
	putc(']', of->of);
	of->col++;
 }
 return 0;
}

typedef struct {
	unsigned hpos;
	unsigned long final;
	unsigned long cur;
	int lastc;
	enum { HS_DATA, HS_FF, HS_HADFINAL, } hstate;
} HDLCBUF;

static int handlehdlc(OUTF *of, V18ASTATE *vs, HDLCBUF *hb, unsigned char *bp, int nc, char *format)
{
 for (; nc; nc--) {
	hb->cur |= (unsigned long /* stupid 'value preserving rules' */) *bp++ << hb->hpos;
	hb->hpos += 8;
	if (hb->hpos == 32) {
		switch (hb->hstate) {
			char *sts;
		case HS_DATA:
			if (hb->cur == 0xffffffff) {
				hb->hstate = HS_FF;
				break;
			}
			if (hb->lastc != EOF) showchar(of, vs, hb->lastc, format);
			showchar(of, vs, hb->cur & 0xff, format);
			showchar(of, vs, (hb->cur >> 8) & 0xff, format);
			showchar(of, vs, (hb->cur >> 16) & 0xff, format);
			hb->lastc = (hb->cur >> 24) & 0xff;
			break;
		case HS_FF:
			if (hb->cur == 0xffffffff) {
					// an escape in data
				if (hb->lastc != EOF) showchar(of, vs, hb->lastc, format);
				showchar(of, vs, 0xff, format);
				showchar(of, vs, 0xff, format);
				showchar(of, vs, 0xff, format);
				hb->lastc =  0xff;
				hb->hstate = HS_DATA;
				break;
			}
			hb->hstate = HS_HADFINAL;
			hb->final = hb->cur;
			break;
		case HS_HADFINAL:
			switch (hb->cur) {
			case 0x0:
				sts = "ok";
				break;
			case 0x2:
				sts = "-crc";
				break;
			case 0x1:
				sts = "abort";
				break;
			case 0x3:
				sts = "Abort";
				break;
			default:
				sts = 0;
			}
			if (sts) {
				unsigned mask = 0xfc000000;
				unsigned pat = 0xf8000000;
				unsigned bits;
				if (hb->lastc != EOF) {
					if (hb->final & 0xff000000) {
						showchar(of, vs, hb->lastc, format);
					} else {
						hb->final <<= 8;
						hb->final |= hb->lastc;
					}
				}
				for (bits=26; ; bits--) {
					if ((hb->final & mask) == pat) {
						showbits(of, vs, hb->final, bits, format);
						break;
					}
					if (mask & 1) {
						flushof(of);
						fprintf(of->of, "\\[?final:%08lx]\n",
							hb->final);
						break;
					}
					mask >>= 1;
					pat >>= 1;
				}
				fprintf(of->of, "\\[%s]\n", sts);
			} else switch (hb->cur) {
			case 0x5:
				fprintf(of->of, "\\[final=%lx,flags]\n", hb->final);
				break;
			case 0x9:
				fprintf(of->of, "\\[final=%lx,junk]\n", hb->final);
				break;
			default:
				fprintf(of->of, "\\[final=%lx,sts=%08lx]\n",
					hb->final, hb->cur);
			}
			of->col = 0;
			hb->hstate = HS_DATA;
			hb->lastc = EOF;
			break;
		}
		hb->cur = 0;
		hb->hpos = 0;
	}
 }
 return 0;
}

static int rxloop(CHAN *chan, OUTF *of)
{
 STATUS_HIST hist;
 HDLCBUF hdlcbuf;
 V18ASTATE vs;
 v18astate(&vs);
 memset(&hist, 0, sizeof(hist));
 hdlcbuf.hpos = 0;
 hdlcbuf.cur = 0;
 hdlcbuf.lastc = EOF;
 hdlcbuf.hstate = HS_DATA;
 for (;;) {
	unsigned char buff[8192];
	SMDC_DATA_PARMS dp;
	int r;
	if (chan->cfg->rx.useevent) {
		r = smd_ev_wait(chan->rxev);
		if (r) return printerr_sm("smd_ev_wait", r);
	}
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan->chan;
	dp.data = buff;
	dp.max_length = sizeof(buff);
	r = smdc_rx_data(&dp);
	if (r) return printerr_sm("smdc_rx_data", r);
	if (!dp.done_length) {
		flushof(of);
		showstatus(chan->chan, &hist);
		fflush(stdout);
	} else {
		if (chan->cfg->rx.smdc.encoding == kSMDCConfigEncodingHDLC) {
			if (handlehdlc(of, &vs, &hdlcbuf, buff,
				dp.done_length, chan->cfg->rx_format))
				return 1;
		} else {
			unsigned char *bp;
			int nc = dp.done_length;
			for (bp = buff; nc--; bp++) {
				if (showchar(of, &vs, *bp, chan->cfg->rx_format)) return 1;
			}
		}
		fflush(of->of);
	}
 }
}

void *rxmain(void *p)
{
 CHAN *chan = p;
 OUTF of;
 int r;
 of.col = 0;
 if (chan->cfg->rx_outfile) {
 	of.of = fopen(chan->cfg->rx_outfile, "w");
 	if (!of.of) {
 		perror("fopen() failed");
 		fprintf(stderr, "Cannot create file: %s\n", chan->cfg->rx_outfile);
 		r = 1;
 		goto fail;
	}
 } else of.of = stdout;
 r = rxloop(chan, &of);
 if (chan->cfg->rx_outfile) {
 	if (fclose(of.of)) {
 		perror("fclose() failed");
 		fprintf(stderr, "Error closing file: %s\n", chan->cfg->rx_outfile);
 		r = 1;
	}
 }
fail:
 if (chan->cfg->rx.useevent) {
 	int r2 = smd_ev_free(chan->rxev);
	if (r2) {
		printerr_sm("smd_ev_free", r2);
		r = r2;
	}
 }
	// maybe signal that we are exitting
 return (void *) r;
}
