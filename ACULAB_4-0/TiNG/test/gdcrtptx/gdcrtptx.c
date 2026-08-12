/* gdcrtptx.c - generic data communication tx tester */

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <smbesp.h>
#include <smdrvr.h>
#include <sys/timeb.h>
#include <pthread.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../Testlib/rtptx_codec.h"
#include "smrtp.h"
#include "smdc.h"

	// encodings
#include "smdc_async.h"
#include "smdc_hdlc.h"
#include "smdc_none.h"
#include "smdc_sync.h"

	// protocols
#include "smdc_raw.h"
#include "smdc_cw.h"
#include "smdc_fsk.h"
#include "smdc_v17.h"
#include "smdc_v27.h"
#include "smdc_v29.h"

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#define USE_POLL
#endif
#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#define USE_POLL
#endif
#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#endif

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#include "../../libutil/v18a.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef struct {
	SMDC_CHANNEL_CONFIG_PARMS smdc;
	tSMEventId ev;
	int tep;
	int shorttrain;
	struct sockaddr* local_ip;
	tSMCollectorId collector;
} CHAN;

static int fn_smdc_line_status(tSMChannelId chan, int *lnk, int *tx, int *rx, int *cts)
{
 SMDC_LINE_STATUS_PARMS ls;
 int r;
 memset(&ls, 0, sizeof(ls));
 ls.channel = chan;
 r = smdc_line_status(&ls);
 *lnk = ls.link_status;
 *rx = ls.rx_status;
 *tx = ls.tx_status;
 *cts = ls.tx_cts;
 if (r) return printerr_sm("smdc_line_status", r);
 return 0;
}

static int fn_smdc_tx_status(tSMChannelId chan, int *cap, int *sts, int *flo)
{
 int r;
 SMDC_TX_STATUS_PARMS txs;
 memset(&txs, 0, sizeof(txs));
 txs.channel = chan;
 r = smdc_tx_status(&txs);
 *cap = txs.capacity;
 *sts = txs.status;
 *flo = txs.flow;
 if (r) return printerr_sm("smdc_tx_status", r);
 return 0;
}

static int fn_smdc_tx_control(tSMChannelId chan, int cmd, int cap)
{
 int r;
 SMDC_TX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.capacity = cap;
 dp.blocking = 1;
 r = smdc_tx_control(&dp);
 if (r) return printerr_sm("smdc_tx_control", r);
 return 0;
}

static int fn_smdc_line_control(tSMChannelId chan, int cmd, int mask, int toggle)
{
 SMDC_LINE_CONTROL_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.channel = chan;
 cp.cmd = cmd;
 cp.aux_mask = mask;
 cp.aux_toggle = toggle;
 r = smdc_line_control(&cp);
 if (r) return printerr_sm("smdc_line_control", r);
 return 0;
}

static void showstatus(CHAN *chanp)
{
 int r, p0, p1, p2, p3;
 r = fn_smdc_tx_status(chanp->smdc.channel, &p0, &p1, &p2);
 if (r) {
	printf("smdc_tx_status returned %d\n", r);
	return;
 }
 printf("tx: status=");
 switch (p1) {
 case kSMDCTxStatusEmpty: printf("Empty"); break;
 case kSMDCTxStatusHasCapacity: printf("Capacity"); break;
 case kSMDCTxStatusNoCapacity: printf("Full"); break;
 case kSMDCTxStatusUnderrun: printf("Underrun"); break;
 case kSMDCTxStatusFinishing: printf("Finishing"); break;
 case kSMDCTxStatusFinished: printf("Finished"); break;
 default: printf("?(%d)", p1);
 }
 printf(" capacity=%d, flow=%d, ", p0, p2);
#ifdef USE_POLL
 {
  struct pollfd pfd;
  pfd.fd = chanp->ev.fd;
  pfd.events = chanp->ev.mode;
  switch (poll(&pfd, 1, 0)) {
  case 1: printf("event:set"); break;
  case 0: printf("event:clear"); break;
  default: perror("poll() failed");
  }
 }
#endif
#ifdef TiNGTYPE_WINNT
 {
  DWORD d = WaitForSingleObject(chanp->ev, 0);
  switch (d) {
  case WAIT_OBJECT_0: printf("event:set"); break;
  case WAIT_TIMEOUT: printf("event:clear"); break;
  case WAIT_FAILED: lasterr("WaitForSingleObject failed"); break;
  default:
  	printf("Unknown status (%d = 0x%x) from WaitForSingleObject\n", d, d);
  }
 }
#endif
 printf("\n");
 r = fn_smdc_line_status(chanp->smdc.channel, &p0, &p1, &p2, &p3);
 if (r) {
	printf("smdc_line_status returned %d\n", r);
	return;
 }
 printf("line: link=");
 switch (p0) {
 case kSMDCLinkStatusNotConnected: printf("NotConnected"); break;
 case kSMDCLinkStatusConnecting: printf("Connecting"); break;
 case kSMDCLinkStatusConnected: printf("Connected"); break;
 case kSMDCLinkStatusDisconnecting: printf("Disconnecting"); break;
 default: printf("?(%d)", p0);
 }
 printf(" rx=");
 switch (p2) {
 case kSMDCRxStatusNoCarrier: printf("NoCarrier"); break;
 case kSMDCRxStatusCarrierPresent: printf("Carrier"); break;
 case kSMDCRxStatusReceivingData: printf("Data"); break;
 default: printf("?(%d)", p1);
 }
 printf(" tx=");
 switch (p1) {
 case kSMDCTxStatusSilent: printf("Silent"); break;
 case kSMDCTxStatusSendingCarrier: printf("Carrier"); break;
 case kSMDCTxStatusSendingData: printf("Data"); break;
 default: printf("?(%d)", p2);
 }
 printf(" txcts=%d\n", p3);
}

static int sksp(FILE *f)
{
 for (;;) {
	int c = getc(f);
	if (c != ' ' && c != '\t') return c;
 }
}

static int getesc(FILE *f)
{
 int c = getc(f);
 if (c == '\\') {
	c = getc(f);
	if ((unsigned) (c - '0') <= 9) {
		int d = getc(f);
		c -= '0';
		if ((unsigned) (d - '0') <= 9) {
			c = (c<<3) | (d-'0');
			d = getc(f);
			if ((unsigned) (d - '0') <= 9) {
				c = (c << 3) | (d - '0');
			} else ungetc(d, f);
		} else ungetc(d, f);
	} else if (c == 'r') c = '\r';
	else if (c == 'n') c = '\n';
	c |= 0x100;		// ensure caller can distinguish real newline
 }
 return c;
}

static unsigned getnum(FILE *f)
{
 int v = 0;
 for (;;) {
	int c = getc(f);
	if ((unsigned) (c - '0') <= 9) {
		v = v * 10 + c - '0';
	} else {
		ungetc(c, f);
		return v;
	}
 }
}

static int getint(FILE *f)
{
 int c = getc(f);
 if ((unsigned) (c - '0') <= 9) {
 	ungetc(c, f);
 	return getnum(f);
 }
 if (c == '-') return -(int)(getnum(f));
 ungetc(c, f);
 return 0;
}

static int set_tep(CHAN *chanp, FILE *f)
{
 chanp->tep = getnum(f);
 printf("TEP %sabled\n", chanp->tep ? "en" : "dis");
 return 0;
}

static void set_txcap(CHAN *chanp, FILE *f)
{
 int x = getint(f);
 fn_smdc_tx_control(chanp->smdc.channel, kSMDCTxCtlNotifyOnCapacity, x);
}

static int readbits(tSM_UT32 **bits, tSM_UT32 *bitlen, FILE *f)
{
 unsigned wlen = 0;	// length written (words)
 unsigned mlen = 0;	// length malloc'ed (words)
 unsigned mask = 1;	// next bit to set
 *bits = 0;
 *bitlen = 0;
 for (;;) {
 	int c = getc(f);
 	if (c != '0' && c != '1') {
 		ungetc(c, f);
 		return 0;
	}
	if (mask == 1) {	// first bit of new word
		if (wlen >= mlen) {
			tSM_UT32 *n;
			mlen = (mlen+1) * 2;
			n = realloc(*bits, mlen * sizeof(*n));
			if (!n) {
				perror("realloc() failed");
				fprintf(stderr, "Cannot allocate space for bitstring\n");
				free(*bits);
				return 1;
			}
			*bits = n;
		}
		(*bits)[wlen] = 0;
	}
	if (c == '1') (*bits)[wlen] |= mask;
	++*bitlen;
	if (mask & 0x80000000) {
		mask = 1;
		wlen++;
	} else mask <<= 1;
 }
}

static int set_prefsuf(CHAN *chanp, FILE *f)
{
 SMDC_LINE_CONTROL_PARMS cp;
 int c;
 memset(&cp, 0, sizeof(cp));
 if (readbits(&cp.prefix_data, &cp.prefix_bitlength, f)) {
	fprintf(stderr, "Cannot read prefix\n");
	return 1;
 }
 c = getc(f);
 if (c == ':') {
	if (readbits(&cp.suffix_data, &cp.suffix_bitlength, f)) {
		fprintf(stderr, "Cannot read suffix\n");
		free(cp.prefix_data);
		return 1;
	}
 } else ungetc(c, f);
 cp.channel = chanp->smdc.channel;
 cp.cmd = kSMDCLineCtlCmdSetPrefixSuffix;
 c = smdc_line_control(&cp);
 free(cp.prefix_data);
 free(cp.suffix_data);
 if (c) return printerr_sm("smdc_line_control", c);
 return 0;
}

static int set_train(CHAN *chanp, FILE *f)
{
 chanp->shorttrain = getnum(f);
 printf("short training %sabled\n", chanp->shorttrain ? "en" : "dis");
 return 0;
}

static int fn_smdc_tx_data(CHAN *chanp, void *buf, int len)
{
 for (;;) {
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chanp->smdc.channel;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_tx_data(&dp);
	if (r) return printerr_sm("smdc_tx_data", r);
	if (! (len -= dp.done_length)) break;
	buf = (char *) buf + dp.done_length;
	showstatus(chanp);
 }
 return 0;
}

static void sendflags(CHAN *chanp, FILE *f)
{
 int nf = getnum(f);
 unsigned blen;
 char buff[4];
 if (chanp->smdc.encoding != kSMDCConfigEncodingHDLC) {
 	fprintf(stderr, "can only send flags or idle over HDLC\n");
 	return;
 }
 nf = -nf;
 buff[0] = nf & 0xff;
 buff[1] = (nf >> 8) & 0xff;
 buff[2] = (nf >> 16) & 0xff;
 buff[3] = (nf >> 24) & 0xff;
 for (blen=0; ; ) {
	unsigned i = 4 - blen;
	if (!i) break;
	// if (i > 2048) i = 2048;
	fn_smdc_tx_data(chanp, buff+blen, i);
	blen += i;
 }
}

static void sendblocks(CHAN *chanp, FILE *f)
{
 int nb = getnum(f);
 unsigned bpos = 0;
 unsigned v = 0;
 char buff[2048];
 for (;;) {
 	if (!v) {
 		if (!nb) {
			if (bpos) fn_smdc_tx_data(chanp, buff, bpos);
 			return;
		}
 		buff[bpos++] = --nb;
 		if (bpos == arlen(buff)) {
			if (fn_smdc_tx_data(chanp, buff, bpos)) return;
			bpos = 0;
		}
	}
	buff[bpos++] = v++;
	v &= 0xff;
	if (bpos == arlen(buff)) {
		if (fn_smdc_tx_data(chanp, buff, bpos)) return;
		bpos = 0;
	}
 }
}

static void sendnumber(CHAN *chanp, FILE *f)
{
 int nb = getnum(f);
 char buff[4];
 buff[0] = nb;
 buff[1] = nb >> 8;
 buff[2] = nb >> 16;
 buff[3] = nb >> 24;
 fn_smdc_tx_data(chanp, buff, 4);
}

static void sendline(CHAN *chanp, FILE *f, int eol, int iswide, int odd)
{
 unsigned blen = 2048;
 char *buff = malloc(blen);
 unsigned bp = 0;
 if (!buff) {
	perror("Cannot malloc buffer");
	return;
 }
 if (chanp->smdc.encoding == kSMDCConfigEncodingHDLC) bp = 4;
 for (;;) {
	int c = getesc(f);
	if (c == '\n') {
		ungetc(c, f);
		break;
	}
	if (c == EOF) break;
	buff[bp++] = c;
	if (iswide) buff[bp++] = 0;
	if (iswide > 1) buff[bp++] = 0;
	if (iswide > 2) buff[bp++] = 0;
	if (bp+4 >= blen) {
		char *t = realloc(buff, blen *= 2);
		if (!t) {
			perror("realloc failed");
			free(buff);
			return;
		}
		buff = t;
	}
 }
 if (eol) {
	buff[bp++] = '\r';
	if (iswide) buff[bp++] = 0;
	if (iswide > 1) buff[bp++] = 0;
	if (iswide > 2) buff[bp++] = 0;
	buff[bp++] = '\n';
	if (iswide) buff[bp++] = 0;
	if (iswide > 1) buff[bp++] = 0;
	if (iswide > 2) buff[bp++] = 0;
 }
 if (chanp->smdc.encoding == kSMDCConfigEncodingHDLC) {
	unsigned i = (bp - 4) * 8 - odd;
	buff[0] = i;
	buff[1] = i >> 8;
	buff[2] = i >> 16;
	buff[3] = i >> 24;
	i = i / 8 + 4;
	for (; bp & 3; ) buff[bp++] = 0;	// pad to word
 }
 for (blen=0; ; ) {
	unsigned i = bp - blen;
	if (!i) break;
	// if (i > 2048) i = 2048;
	fn_smdc_tx_data(chanp, buff+blen, i);
	blen += i;
 }
 free(buff);
}

static void sendv18a(CHAN *chanp, V18ASTATE *vs, FILE *f)
{
 unsigned blen = 2048;
 char *buff = malloc(blen);
 unsigned char v18ain[8];	// left over input chars
 unsigned nv18ain = 0;	// number of left over input chars
 unsigned bp = 0;
 if (!buff) {
	perror("Cannot malloc buffer");
	return;
 }
 if (chanp->smdc.encoding == kSMDCConfigEncodingHDLC) bp = 4;
 for (;;) {
	int c = getesc(f);
	unsigned char *inp;
	if (c == '\n') {
		ungetc(c, f);
		break;
	}
	if (c == EOF) break;
	v18ain[nv18ain++] = c;
	inp = v18ain;
	bp = v18a_fromT50(&buff[bp], buff+blen, vs, &inp, &v18ain[nv18ain]) - buff;
	if (inp == v18ain) {
		fprintf(stderr, "Ignored char %02x\n", v18ain[0]);
		inp++;
	}
		// move unused chars to start of buffer
	c = &v18ain[nv18ain] - inp;
	if (c) memmove(v18ain, inp, c);
	nv18ain = c;
	if (bp+4 >= blen) {
		char *t = realloc(buff, blen *= 2);
		if (!t) {
			perror("realloc failed");
			free(buff);
			return;
		}
		buff = t;
	}
 }
 if (chanp->smdc.encoding == kSMDCConfigEncodingHDLC) {
	unsigned i = (bp - 4) * 8;
	buff[0] = i;
	buff[1] = i >> 8;
	buff[2] = i >> 16;
	buff[3] = i >> 24;
	i = i / 8 + 4;
	for (; bp & 3; ) buff[bp++] = 0;	// pad to word
 }
 for (blen=0; ; ) {
	unsigned i = bp - blen;
	if (!i) break;
	// if (i > 2048) i = 2048;
	fn_smdc_tx_data(chanp, buff+blen, i);
	blen += i;
 }
 free(buff);
}

static void sendodd(CHAN *chanp, FILE *f, int iswide)
{
 int odd = getc(f);
 if (odd < '0' || odd > '8') {
 	fprintf(stderr, "Error: expected number of bits, got '%c'\n", odd);
	for (;;) {
		if (odd == EOF) return;
		if (odd == '\n') {
			ungetc(odd, f);
			return;
		}
		odd = getc(f);
	}
 }
 odd -= '0';
 sendline(chanp, f, 0, iswide, 8 - odd);
}

static void fn_smdc_stop(CHAN* chanp)
{
	SMDC_STOP_PARMS stop_parms;
	int r;
	memset(&stop_parms,0,sizeof(stop_parms));
	stop_parms.channel=chanp->smdc.channel;
	r=smdc_stop(&stop_parms);
	if (r) {
		printf("smdc_stop returned %d\n",r);
	}
}

static void fn_smdc_channel_config(CHAN* chanp)
{
	int r = smdc_channel_config(&chanp->smdc);
	if (r) {
 		printerr_sm("smdc_channel_config", r);
	}
}

static unsigned auto_crc_size(unsigned crc)
{
 unsigned i;
 if (!crc) return 0;
 for (i=0; crc; crc >>= 1) i++;
 return i - 1;
}

static int runcmd(CHAN *chanp, V18ASTATE *vs, FILE *f, int c)
{
 const int iswide = (chanp->smdc.encoding == kSMDCConfigEncodingAsync
 	&& ((SMDC_ASYNC_FORMAT_PARMS *) chanp->smdc.encoding_config_data)
	->databits > 8) ? 1 : 0;
 switch (c) {
 case '?':
	printf("Commands:\n"
		"		show current status\n"
		"=...		send line of data\n"
		"-...		send line of data with no newline\n"
		"v...		send data translating into V.18 Annex A format\n"
		"!N...		[hdlc] send data with last byte containing N bits\n"
		"AN		set tx capacity threshold to N\n"
		"bN		send N blocks (257 bytes each) of data\n"
		"ci		control: connect as initiator\n"
		"cr		control: connect as responder\n"
		"cd		control: disconnect \n"
		"f N		[hdlc] send N flags (0=idle)\n"
		"F		finish\n"
		"p...[:...]	set prefix [and suffix]\n"		
		"R		RTS on\n"
		"r		RTS off\n"
		"e N		set talker echo protoection\n"
		"t N		set short training\n"
		"s		stop\n"
		"C		config channel\n"
		"EaNnM		set encoding to async, N databits, M stopbits\n"
		"EhX[xY]	set encoding to HDLC, crc=X, crcsize=Y\n"
		"Es		set encoding to sync\n"
		"E		set no encoding\n"
		"u...		send line of data as 32bit values\n"
		"U...		send line of data with no newline as 32bit values\n"
		"#anything comment\n"
		"q		quit\n"
		);
	break;
 case 'E': {
	union {
		SMDC_ASYNC_FORMAT_PARMS async;
		SMDC_HDLC_FORMAT_PARMS hdlc;
	} e;
	SMDC_ENCODING_PARMS ep;
	int r;
	memset(&ep, 0, sizeof(ep));
	memset(&e, 0, sizeof(e));
	ep.channel = chanp->smdc.channel;
	c = getc(f);
	switch (c) {
	case 'a':
		ep.encoding = kSMDCConfigEncodingAsync;
		ep.encoding_config_data = &e.async;
		ep.encoding_config_length = sizeof(e.async);
		e.async.databits = getnum(f);
		c = getc(f);
		if (c == 'n') {
			e.async.stopbits = getnum(f);
		} else {
			ungetc(c, f);
			c = 0;
			printf("Expected 'n' after databits\n");
		}
		break;
	case 'h':
		ep.encoding = kSMDCConfigEncodingHDLC;
		ep.encoding_config_data = &e.hdlc;
		ep.encoding_config_length = sizeof(e.hdlc);
		e.hdlc.crc = getnum(f);
		c = getc(f);
		if (c == 'x') {
			 e.hdlc.crcsize = getnum(f);
		} else {
			ungetc(c, f);
			e.hdlc.crcsize = auto_crc_size(e.hdlc.crc);
		}
		break;
	case 's':
		ep.encoding = kSMDCConfigEncodingSync;
		break;
	case '\n':
		break;
	default:
		ungetc(c, f);
		c = 0;
		printf("Encoding ignored (no valid encoding code found)\n");
	}
	if (c) {
		r = smdc_tx_encoding(&ep);
		if (r) return printerr_sm("smdc_line_control", r);
	}
	break;
 }
 case 'A':
	set_txcap(chanp, f);
 	break;
 case 'b':
	sendblocks(chanp, f);
 	break;
 case 'c': {
	SMDC_LINE_CONTROL_PARMS cp;
	int r;
	memset(&cp, 0, sizeof(cp));
	cp.channel = chanp->smdc.channel;
 	c = getc(f);
 	switch (c) {
 	case 'i': cp.cmd = kSMDCLineCtlCmdInitiatorConnect; break;
 	case 'r': cp.cmd = kSMDCLineCtlCmdResponderConnect; break;
 	case 'd': cp.cmd = kSMDCLineCtlCmdDisconnect; break;
 	default:
		ungetc(c, f);
		c = 0;
		printf("Control ignored (no valid control code found)\n");
 	}
	if (c) {
		r = smdc_line_control(&cp);
		if (r) return printerr_sm("smdc_line_control", r);
	}
 }
 	break;
 case 'e':
	set_tep(chanp, f);
 	break;
 case 'f':
	sendflags(chanp, f);
 	break;
 case 'F':
	fn_smdc_tx_control(chanp->smdc.channel, kSMDCTxCtlFinish, 2048);
 	break;
 case 'p':
 	set_prefsuf(chanp, f);
 	break;
 case 'R': {
	SMDC_LINE_CONTROL_PARMS cp;
	int r;
	memset(&cp, 0, sizeof(cp));
	cp.channel = chanp->smdc.channel;
	cp.cmd = kSMDCLineCtlCmdAssertRTS;
	cp.tep = chanp->tep;
	cp.shorttrain = chanp->shorttrain;
	r = smdc_line_control(&cp);
	if (r) return printerr_sm("smdc_line_control", r);
 }
	break;
 case 'r':
	fn_smdc_line_control(chanp->smdc.channel, kSMDCLineCtlCmdUnassertRTS, 0, 0);
	break;
 case 's':
	fn_smdc_stop(chanp);
	break;
 case 'C':
	fn_smdc_channel_config(chanp);
	break;
 case 't':
	set_train(chanp, f);
 	break;
 case '!':
	sendodd(chanp, f, iswide);
 	break;
 case '=':
	sendline(chanp, f, 1, iswide, 0);
 	break;
 case '-':
	sendline(chanp, f, 0, iswide, 0);
	break;
 case 'v':
 	if (iswide) {
 		fprintf(stderr, "Cannot use >8 bit chars with V.18A\n");
	} else {
		sendv18a(chanp, vs, f);
	}
 	break;
 case 'u':
	sendline(chanp, f, 1, 3, 0);
 	break;
 case 'U':
	sendline(chanp, f, 0, 3, 0);
	break;
 case 'n':
	sendnumber(chanp, f);
 	break;
 case EOF:
 case 'q': return c;
 case '\n': showstatus(chanp); ungetc(c, f); break;
 default:
	fprintf(stderr, "Bad command: %c\n", c);
	break;
 case '#':
	for (;;) {
		c = getc(f);
		if (c == EOF) return c;
		putchar(c);
		if (c == '\n') return c;
	}
	break;
 }
 for (;;) {
	c = getc(f);
	if (c == EOF || c == '\n') return c;
 }
}

static int mainloop(CHAN *chanp, V18ASTATE *vs, FILE *f)
{
 for (;;) {
	int c = sksp(f);
	c = runcmd(chanp, vs, f, c);
	if (c != '\n') return c;
 }
}

static int procfiles(CHAN *chanp, char *inifile)
{
 V18ASTATE vs;
 FILE *f;
 v18astate(&vs);
 if (inifile) {
 	f = fopen(inifile, "r");
 	if (!f) {
 		perror("Cannot open file");
 		fprintf(stderr, "File: %s\n", inifile);
 		return 1;
	}
	switch (mainloop(chanp, &vs, f)) {
	case EOF: break;
	case 'q':
		fprintf(stderr, "quitting inifile\n");
		return 0;
	default:
		fprintf(stderr, "error while processing initfile: %s\n",
			inifile);
		return 1;
	}
 }
 switch (mainloop(chanp, &vs, stdin)) {
 case EOF: break;
 case 'q':
	fprintf(stderr, "quitting\n");
	break;
 default:
	fprintf(stderr, "error while processing stdin\n");
	return 1;
 }
 return 0;
}

static int createcoll(CHAN *chanp, tSMModuleId module)
{
 SM_UDP_COLLECTOR_CREATE_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.module = module;
 if (chanp->local_ip->sa_family == AF_INET) {
	memcpy(&cp.address, &((struct sockaddr_in*)chanp->local_ip)->sin_addr, sizeof(struct in_addr));
 } else {
	memcpy(&cp.ipv6_address, &((struct sockaddr_in6*)chanp->local_ip)->sin6_addr, sizeof(struct in6_addr));
	cp.type = kSMCollectorTypeIPv6;
 }
 r = sm_udp_collector_create(&cp);
 if (r) return printerr_sm("sm_udp_collector_create", r);
 chanp->collector = cp.collector;
 return 0;
}

static int coll_getevent(CHAN *chanp, tSMEventId *evp)
{
 SM_COLLECTOR_EVENT_PARMS ep;
 int r;
 memset(&ep, 0, sizeof(ep));
 ep.collector = chanp->collector;
 r = sm_collector_get_event(&ep);
 if (r) return printerr_sm("sm_collector_get_event", r);
 *evp = ep.event;
 return 0;
}

static int makecoll(CHAN *chanp)
{
 tSMEventId ev;
 if (coll_getevent(chanp, &ev)) return 1;
 for (;;) {
 	SM_COLLECTOR_STATUS_PARMS cp;
 	int r = smd_ev_wait(ev);
	if (r) return printerr_sm("smd_ev_wait", r);
	memset(&cp, 0, sizeof(cp));
	cp.collector = chanp->collector;
 	r = sm_collector_status(&cp);
	if (r) return printerr_sm("sm_collector_status", r);
	switch (cp.status) {
		unsigned long addr;
	case kSMCollectorStatusRunning: break;
	case kSMCollectorStatusGotPorts: {
		SM_CHANNEL_COLLECTOR_CONNECT_PARMS ccp;
		addr = ntohl(cp.u.ports.address.s_addr);
		printf("Listening on port %d.%d.%d.%d:%d\n",
			(int) (addr >> 24) & 0xff,
			(int) (addr >> 16) & 0xff,
			(int) (addr >> 8) & 0xff,
			(int) (addr >> 0) & 0xff,
			cp.u.ports.port);
		memset(&ccp, 0, sizeof(ccp));
		ccp.channel = chanp->smdc.channel;
		ccp.source = chanp->collector;
		r = sm_channel_collector_connect(&ccp);
		if (r) return printerr_sm("sm_channel_collector_connect", r);
	}
		return 0;
	case kSMCollectorStatusNewPeer: break;
	}
 }
}

static int closecoll(CHAN *chanp)
{
 int r = sm_collector_destroy(chanp->collector);
 if (r) return printerr_sm("sm_collector_destroy", r);
 return 0;
}

static err_t connect_chan_rtp(tSMVMPtxId vmptx, tSMChannelId chan);
static tSMChannelId makechan(CHAN *chanp, tSMModuleId module, tSMVMPtxId vmptx)
{
 int r;
 {
  SM_CHANNEL_ALLOC_PLACED_PARMS pp;
  memset(&pp, 0, sizeof(pp));
  pp.type = kSMChannelTypeOutput;
  pp.module = module;
  r = sm_channel_alloc_placed(&pp);
  if (r) {
	printerr_sm("sm_channel_alloc_placed", r);
	return kSMNullChannelId;
  }
  chanp->smdc.channel = pp.channel;
 }
 {
  chanp->collector = kSMNullCollectorId;
  if (chanp->local_ip != NULL) {
	r = createcoll(chanp, module);
	if (!r) r = makecoll(chanp);
	if (r) {
		sm_channel_release(chanp->smdc.channel);
		return kSMNullChannelId;
	}
  }
 }
 {
	err_t err = connect_chan_rtp(vmptx, chanp->smdc.channel);
	if (err) {
		error_log(stderr, error(err, "connect_chan_rtp() failed"));
		sm_channel_release(chanp->smdc.channel);
		return kSMNullChannelId;
	}
 }
 r = smd_ev_create(&chanp->ev, chanp->smdc.channel, kSMEventTypeWriteData, kSMChannelSpecificEvent);
 if (r) {
 	printerr_sm("smd_ev_create", r);
	return kSMNullChannelId;
 }
 {
  SM_CHANNEL_SET_EVENT_PARMS ep;
  memset(&ep, 0, sizeof(ep));
  ep.channel = chanp->smdc.channel;
  ep.event_type = kSMEventTypeWriteData;
  ep.issue_events = kSMChannelSpecificEvent;
  ep.event = chanp->ev;
  r = sm_channel_set_event(&ep);
  if (r) {
  	printerr_sm("sm_channel_set_event", r);
	return kSMNullChannelId;
  }
 }
 r = smdc_channel_config(&chanp->smdc);
 if (r) {
 	printerr_sm("smdc_channel_config", r);
	sm_channel_release(chanp->smdc.channel);
	return kSMNullChannelId;
 }
 return chanp->smdc.channel;
}

static int closechan(CHAN *chanp)
{
 SM_CHANNEL_SET_EVENT_PARMS ep;
 int r;
 memset(&ep, 0, sizeof(ep));
 ep.channel = chanp->smdc.channel;
 ep.event_type = kSMEventTypeWriteData;
 ep.issue_events = kSMChannelNoEvent;
 ep.event = chanp->ev;
 r = sm_channel_set_event(&ep);
 if (r) return printerr_sm("sm_channel_set_event", r);
 r = smd_ev_free(chanp->ev);
 if (r) return printerr_sm("smd_ev_free", r);
 r = sm_channel_release(chanp->smdc.channel);
 if (r) printerr_sm("sm_channel_release", r);
 return r;
}

static int runtx(CHAN *chanp, tSMModuleId module, char *inifile, tSMVMPtxId vmptx)
{
 int i = 0;
 chanp->smdc.channel = makechan(chanp, module, vmptx);
 if (chanp->smdc.channel == kSMNullChannelId) return 1;
 if (!i) i = procfiles(chanp, inifile);
 if (closecoll(chanp) && !i) i = 1;
 if (closechan(chanp) && !i) return 1;
 return i;
}

static int lookup_fsk(SMDC_FSK_CONFIG_PARMS *fskp, char *name)
{
 static struct {
	char *name;
	float markfreq, spacefreq;
	unsigned speed;
 } std[] = {
	{ "v21o", 980, 1180, 300, },
	{ "v21a", 1650, 1850, 300, },
	{ "v23o", 390, 450, 75, },
	{ "v23a", 1300, 2100, 1200, },
	{ "v23a600", 1300, 1700, 600, },
	{ "bell103a", 1270, 1070, 300, },
	{ "bell103o", 2225, 2025, 300, },
 };
 unsigned i;
 for (i=0; ; i++) {
	if (i >= arlen(std)) {
		fprintf(stderr, "Unknown standard: %s\n", name);
		fprintf(stderr, "Valid standards:");
		for (i=0; i < arlen(std); i++) {
			fprintf(stderr, " %s", std[i].name);
		}
		fprintf(stderr, "\n");
		return 1;
	}
	if (!strcmp(name, std[i].name)) {
		fskp->mark_frequency = std[i].markfreq;
		fskp->space_frequency = std[i].spacefreq;
		fskp->speed = std[i].speed;
		break;
	}
 }
 return 0;
}

static int lookup_ip(struct sockaddr **ip, char *addr, unsigned port)
{
	// NB this ALWAYS returns the first answer
 if (addr) {
	struct addrinfo hints;
	struct addrinfo *result;
	char portstr[8];
	int s;

	sprintf(portstr, "%u", port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#else
	hints.ai_flags = 0;
#endif

	s = getaddrinfo(addr, portstr, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo(%s, %s..) : %s\n", addr, portstr, gai_strerror(s));
		return -1;
	}
	*ip = malloc(result->ai_addrlen);
	if (!*ip) {
		fprintf(stderr, "malloc failed\n");
		return -1;
	}
	memcpy(*ip, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
 } else {
 	*ip = NULL;
 }
 return 0;
}


	// Convert a Prosody error code and an explanation into an err_t
static err_t prosody_error(int rc, char *text)
{
 const char *name = errcode_sm(rc);
 char buf[80];
 if (!name) {
 	sprintf(buf, "Prosody error %d\n", rc);
 	name = buf;
 }
 return error(error(0, name), text);
}

static err_t stop_vmptx(tSMVMPtxId vmptx)
{
 SM_VMPTX_STOP_PARMS vsp;
 int e;
 memset(&vsp, 0, sizeof(vsp));
 vsp.vmptx = vmptx;
 e = sm_vmptx_stop(&vsp);
 if (e) return prosody_error(e, "sm_vmptx_stop() failed");
 return 0;
}

static int handle_vmptx(tSMVMPtxId vmptx)
{
	SM_VMPTX_STATUS_PARMS statusp;
	int e;
	err_t err = 0;
	memset(&statusp,0,sizeof(statusp));
	statusp.vmptx = vmptx;
	e = sm_vmptx_status(&statusp);
	if (e) err = prosody_error(e, "sm_vmptx_status() failed");
	else if (statusp.status == kSMVMPrxStatusStopped) {
		return 1;
	}
	if (err) {
		error_log(stderr, error(err, "handle_vmptx() failed"));
		stop_vmptx(vmptx);
		return 1;
	}
	return 0;	
}

static err_t create_vmptx(tSMVMPtxId* pvmptx, tSMModuleId module)
{
 SM_VMPTX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 e = sm_vmptx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmptx_create() failed");
 *pvmptx = vcp.vmptx;
 return 0;
}

static err_t connect_chan_rtp(tSMVMPtxId vmptx, tSMChannelId chan)
{
 SM_CHANNEL_DATAFEED_PARMS tdgp;
 SM_VMPTX_DATAFEED_CONNECT_PARMS vdcp;
 int e;
 memset(&tdgp, 0, sizeof(tdgp));
 tdgp.channel = chan;
 e = sm_channel_get_datafeed(&tdgp);
 if (e) return prosody_error(e, "sm_channel_get_datafeed() failed");
 memset(&vdcp, 0, sizeof(vdcp));
 vdcp.vmptx = vmptx;
 vdcp.data_source = tdgp.datafeed;
 e = sm_vmptx_datafeed_connect(&vdcp);
 if (e) return prosody_error(e, "sm_vmptx_datafeed_connect() failed");
 return 0;
}

static err_t get_event(tSMVMPtxId vmptx, tSMEventId *pev)
{
 SM_VMPTX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmptx = vmptx;
 e = sm_vmptx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmptx_get_event() failed");
 *pev = evp.event;
 return 0;
}

static tSMVMPTxToneSetId G_VMPToneSet=0;

typedef struct {
	unsigned short source_port;
	unsigned short dest_port;
	char dest_ip[256];
	unsigned elim_tones;
	RTPTX_PAR rtptx_par;
} VMPPAR;

static err_t config_vmptx(tSMVMPtxId vmptx, VMPPAR vmppar, tSMModuleId module, char* srtp_key)
{
 int e;
 err_t err;
 struct addrinfo hints;
 struct addrinfo *result;
 int s;
 char *ipaddr;
 short port_num;
 ipaddr = vmppar.dest_ip;
 port_num = vmppar.dest_port;
 memset(&hints, 0, sizeof(struct addrinfo));
 hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
 hints.ai_socktype = SOCK_DGRAM;
 hints.ai_protocol = IPPROTO_UDP;
 hints.ai_flags = AI_NUMERICHOST;

 s = getaddrinfo(vmppar.dest_ip, NULL, &hints, &result);
 if (s != 0) {
	err = error(error(NULL, gai_strerror(s)), "getaddrinfo failed");
	return err;
 }

 if (result->ai_family == AF_INET) {
	SM_VMPTX_CONFIG_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
	configp.destination_rtp.sin_port = htons(port_num);
	configp.destination_rtcp.sin_addr.s_addr = ((struct sockaddr_in*)result->ai_addr)->sin_addr.s_addr;
	configp.destination_rtcp.sin_port = htons(port_num+1);
	configp.source_rtp.sin_port = htons(vmppar.source_port);
	e = sm_vmptx_config(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 } else {
	SM_VMPTX_CONFIG_IPV6_PARMS configp;
	memset(&configp, 0, sizeof(configp));
	configp.vmptx = vmptx;
	configp.destination_rtp.sin6_addr = ((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
	configp.destination_rtp.sin6_port = htons(port_num);
	configp.destination_rtcp.sin6_addr = ((struct sockaddr_in6*)result->ai_addr)->sin6_addr;
	configp.destination_rtcp.sin6_port = htons(port_num+1);
	configp.source_rtp.sin6_port = htons(vmppar.source_port);
	e = sm_vmptx_config_ipv6(&configp);
	if (e) return prosody_error(e, "sm_vmptx_config() failed");
 }
 freeaddrinfo(result);

 err = rtptx_config_vmptx(vmptx, &vmppar.rtptx_par);
 if (err) return err;
 if(vmppar.elim_tones) {
	struct sm_vmptx_tone_parms tparms;
	SM_VMPTX_CODEC_RFC2833_PARMS codecp;
	if(!G_VMPToneSet) {
		struct sm_vmptx_create_toneset_parms tsparms;
		tsparms.module = module;
		tsparms.toneset = kSMVMPTxDefaultToneSet;
		e = sm_vmptx_create_toneset(&tsparms);
 		if (e) return prosody_error(e, "sm_vmptx_config_toneset() failed");
		G_VMPToneSet = tsparms.tone_set_id;
	}
	//turn on tones;
	memset(&codecp,0,sizeof(codecp));
	codecp.vmptx = vmptx;
	codecp.payload_type = vmppar.elim_tones;
	e = sm_vmptx_config_codec_rfc2833(&codecp);
	if (e) return prosody_error(e,"sm_vmptx_config_code_rfc2833() failed");
	memset(&tparms, 0, sizeof(tparms));
	tparms.vmptx = vmptx;
	tparms.convert_tones = 1;
	tparms.tone_set_id = G_VMPToneSet;
	tparms.elim_tones = 1;
	e = sm_vmptx_config_tones(&tparms);
 	if (e) return prosody_error(e, "sm_vmprx_config_toneset() failed");
 }
 if (srtp_key) {
	SM_VMPTX_CONFIG_ENCRYPTION_AES_CM_PARMS ep;
	SM_VMPTX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS ap;
	memset(&ap, 0, sizeof(ap));
	ap.vmptx = vmptx;
	ap.key = srtp_key;
	ap.keylen = 30;
	ap.taglen = 80;
	e = sm_vmptx_config_authentication_hmac_sha1(&ap);
	if (e) return prosody_error(e, "sm_vmptx_config_authentication_hmac_sha1() failed");

	memset(&ep, 0, sizeof(ep));
	ep.vmptx = vmptx;
	ep.key = srtp_key;
	ep.keylen = 30;
	e = sm_vmptx_config_encryption_aes_cm(&ep);
	if (e) return prosody_error(e, "sm_vmptx_config_encryption_aes_cm() failed");
 }
 return 0;
}

	// the worker thread is this function
static void *worker_thread(void *p)
{
 tSMVMPtxId vmptx = p;
 tSMEventId ev;
 err_t err = get_event(vmptx, &ev);
 if (err) {
	error_log(stderr, error(err, "get_event() failed"));
	return (void *) 1;
 }
#ifdef TiNGTYPE_WINNT
 if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS)) {
	error_log(stderr, error_last("SetPriorityClass"));
	return (void *) 1;
 }
#endif
 for(;;) {
	int r;
	r = smd_ev_wait(ev);
	if (r) return (void *) printerr_sm("smd_ev_wait", r);
	r = handle_vmptx(vmptx);
	if (r) break;
 }
 return 0;
}


static int start_vmptx(tSMVMPtxId* pvmptx, pthread_t *ptid, tSMModuleId module, VMPPAR vmppar, char* srtp_key)
{
 err_t err = create_vmptx(pvmptx, module);
 int e;
 if (err) {
	error_log(stderr, error(err, "start_vmptx() failed"));
	return 1;
 }
 vmppar.rtptx_par.sample_rate = 8000;
 err = config_vmptx(*pvmptx, vmppar, module, srtp_key);
 if (err) {
	error_log(stderr, error(err, "start_vmptx() failed"));
	return 1;
 }
 e = pthread_create(ptid, 0, worker_thread, *pvmptx);
 if (e) {
 	err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
	error_log(stderr, error(err, "start_vmptx() failed"));
	return 1;
 }
 return 0;
}

static const unsigned char b64dectbl[] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 128, 255, 255,
255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};

static int base64decode(const unsigned char *in, size_t inlen, void *out, size_t outlen)
{
	unsigned int word;
	size_t pos;
	int pad = 0;
	unsigned char *outp = (unsigned char*)out;
	if (inlen % 4 || (inlen >> 2) * 3 > outlen) return -1; // outlen could be less if padding present!
	for(pos = 0; pos < inlen; ) {
		unsigned char code = b64dectbl[in[pos++]];
		if (code == 255 || code == 128) return -1; // not valid
		word = code << 18;
		code = b64dectbl[in[pos++]];
		if (code == 255 || code == 128) return -1; // not valid
		word |= code << 12;
		code = b64dectbl[in[pos++]];
		if (code == 255) return -1; // not valid
		if (code == 128) { // padding
			if (pos + 1 != inlen || in[pos] != '=') return -1;
			pad = 2;
			code = 0;
		}
		word |= code << 6;
		code = b64dectbl[in[pos++]];
		if (code == 255) return -1; // not valid
		if (code == 128) { // padding
			if (pos != inlen) return -1;
			if (!pad) pad = 1;
			code = 0;
		} else if (pad) {
			return -1; // expected more pad
		}
		word |= code;

		if (pad == 0) {
			*outp++ = (word & 0x00ff0000)>>16;
			*outp++ = (word & 0x0000ff00)>>8;
			*outp++ = (word & 0x000000ff);
		} else if (pad == 1) {
			*outp++ = (word & 0x00ff0000)>>16;
			*outp++ = (word & 0x0000ff00)>>8;
		} else { // pad == 2
			*outp++ = (word & 0x00ff0000)>>16;
		}
	}
	return (outp - (unsigned char*)out);
}


#include "gen/gdcrtptx.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 CHAN chan;
 int iErr;
 char* srtp_key = NULL;
 union {
	SMDC_ASYNC_FORMAT_PARMS async;
	SMDC_HDLC_FORMAT_PARMS hdlc;
 } e;
 union {
	SMDC_CW_CONFIG_PARMS cw;
	SMDC_FSK_CONFIG_PARMS fsk;
	SMDC_V17_CONFIG_PARMS v17;
	SMDC_V27_CONFIG_PARMS v27;
	SMDC_V29_CONFIG_PARMS v29;
 } m;
 tSMVMPtxId vmptx = 0;
 pthread_t tid;
 VMPPAR vmppar;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 memset(&chan.smdc, 0, sizeof(chan.smdc));
 chan.tep = 0;
 chan.shorttrain = 0;
 memset(&e, 0, sizeof(e));
 memset(&m, 0, sizeof(m));
 if (!arg.modulation) {
	fprintf(stderr, "%s: missing modulation option\n", progname);
	return 1;
 }
 if (!strcmp(arg.modulation, "raw")) {
 	chan.smdc.protocol = kSMDCProtocolRawTx;
 } else if (!strcmp(arg.modulation, "cw")) {
 	chan.smdc.protocol = kSMDCProtocolCWtx;
	chan.smdc.config_length = sizeof(m.cw);
	chan.smdc.config_data = &m.cw;
	if (arg.spacefreq) m.cw.space_frequency = arg.spacefreq;
	if (arg.bitrate) m.cw.speed = arg.bitrate;
 } else if (!strcmp(arg.modulation, "fsk")) {
 	chan.smdc.protocol = kSMDCProtocolFSKtx;
	chan.smdc.config_length = sizeof(m.fsk);
	chan.smdc.config_data = &m.fsk;
	if (arg.standard) {
		if (lookup_fsk(&m.fsk, arg.standard)) {
			fprintf(stderr, "%s: bad option\n", progname);
			return 1;
		}
	}
	if (arg.markfreq) m.fsk.mark_frequency = arg.markfreq;
	if (arg.spacefreq) m.fsk.space_frequency = arg.spacefreq;
	if (arg.bitrate) m.fsk.speed = arg.bitrate;
	m.fsk.user_power = arg.power;
	m.fsk.set_power = 1;
 } else if (!strcmp(arg.modulation, "v17")) {
 	chan.smdc.protocol = kSMDCProtocolV17tx;
	chan.smdc.config_length = sizeof(m.v17);
	chan.smdc.config_data = &m.v17;
	if (arg.bitrate) m.v17.speed = arg.bitrate;
 } else if (!strcmp(arg.modulation, "v27")) {
 	chan.smdc.protocol = kSMDCProtocolV27tx;
	chan.smdc.config_length = sizeof(m.v27);
	chan.smdc.config_data = &m.v27;
	if (arg.bitrate) m.v27.speed = arg.bitrate;
 } else if (!strcmp(arg.modulation, "v29")) {
 	chan.smdc.protocol = kSMDCProtocolV29tx;
	chan.smdc.config_length = sizeof(m.v29);
	chan.smdc.config_data = &m.v29;
	if (arg.bitrate) m.v29.speed = arg.bitrate;
 } else {
	fprintf(stderr, "%s: unknown modulation option: %s\n",
		progname, arg.modulation);
	fprintf(stderr, "Valid options are: raw fsk v27 v29\n");
	return 1;
 }
 if (arg.encoding) {
	if (!strcmp(arg.encoding, "sync")) {
		chan.smdc.encoding = kSMDCConfigEncodingSync;
	} else if (!strcmp(arg.encoding, "async")) {
		chan.smdc.encoding = kSMDCConfigEncodingAsync;
		chan.smdc.encoding_config_data = &e.async;
		chan.smdc.encoding_config_length = sizeof(e.async);
		e.async.stopbits = arg.stopbits;
		e.async.databits = arg.databits;
	} else if (!strcmp(arg.encoding, "hdlc")) {
		chan.smdc.encoding = kSMDCConfigEncodingHDLC;
		chan.smdc.encoding_config_data = &e.hdlc;
		chan.smdc.encoding_config_length = sizeof(e.hdlc);
		if (arg.crc) e.hdlc.crc = arg.crc;
		if (arg.crcsize) e.hdlc.crcsize = arg.crcsize;
		else e.hdlc.crcsize = auto_crc_size(e.hdlc.crc);
	} else {
		fprintf(stderr, "%s: unknown encoding option: %s\n",
			progname, arg.encoding);
		fprintf(stderr, "Valid options are: sync async hdlc\n");
		return 1;
	}
 }

 if (lookup_ip(&chan.local_ip, arg.local_ip, 0) != 0) {
	fprintf(stderr, "Cannot interpret address: %s\n", arg.local_ip);
 }
 if (arg.serialnumber) {
	err_t err = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (err) {
		error_log(stderr, err);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 
 memset(&vmppar, 0, sizeof(vmppar));
 if (!arg.codec || rtptx_init_par(&vmppar.rtptx_par, arg.codec)) {
	fprintf(stderr, "RTP codec required\nUsage: %s" ARGS_USAGE "\nFormats:", progname);
	rtptx_dump_codecs(stderr);
	return -1;
 }
 vmppar.source_port = arg.source_port;
 strcpy(vmppar.dest_ip, arg.dest_ip);
 vmppar.dest_port = arg.dest_port;
 vmppar.elim_tones = arg.rfc2833tones;
 if (arg.pticks == 0) vmppar.rtptx_par.pticks = 80;
 else vmppar.rtptx_par.pticks = arg.pticks;
 if (arg.frame_len == 0) vmppar.rtptx_par.frame_len = 20 * 8;
 else vmppar.rtptx_par.frame_len = arg.frame_len;
 vmppar.rtptx_par.vadmode = arg.vad;
 vmppar.rtptx_par.high_pass_filter = arg.high_pass_filter;
 vmppar.rtptx_par.bitrate = arg.codec_bitrate;
 vmppar.rtptx_par.variant = arg.variant;
 vmppar.rtptx_par.payloadmapping = arg.payloadmapping;
 vmppar.rtptx_par.comfort_noise_payload_mapping = arg.comfort_noise_payload_mapping;

 if (arg.srtp_key) {
	srtp_key = malloc(30);
	if (strlen(arg.srtp_key) != 40 || base64decode(arg.srtp_key, 40, srtp_key, 30) != 30) {
		fprintf(stderr, "SRTP key must be 30 octets encoded in 40 base64 characters\n");
		return -1;
	}
 }

 iErr = start_vmptx(&vmptx, &tid, mod, vmppar, srtp_key); 

 if(!iErr) iErr = runtx(&chan, mod, arg.inifile, vmptx);

 if (vmptx) {
	void* pv;
	stop_vmptx(vmptx);
	pthread_join(tid, &pv);
	sm_vmptx_destroy(vmptx);
 }

 if(G_VMPToneSet) {
	struct sm_vmptx_destroy_toneset_parms tsparms;
	int err;
	tsparms.tone_set_id = G_VMPToneSet;
	err = sm_vmptx_destroy_toneset(&tsparms);
	if (err) error_log(stderr, prosody_error(err, "sm_vmptx_destroy_toneset() failed"));
 }
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 free(srtp_key);
 return iErr;
}
