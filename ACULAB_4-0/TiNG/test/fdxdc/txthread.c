#include "fdxdc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Testlib/errcode_sm.h"

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#define USE_POLL
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#define USE_POLL
#endif


#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#endif

#include "../libutil/v18a.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

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
 r = fn_smdc_tx_status(chanp->chan, &p0, &p1, &p2);
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
 if (chanp->cfg->tx.useevent) {
#ifdef USE_POLL
	struct pollfd pfd;
	pfd.fd = chanp->txev.fd;
	pfd.events = chanp->txev.mode;
	switch (poll(&pfd, 1, 0)) {
	case 1: printf("event:set"); break;
	case 0: printf("event:clear"); break;
	default: perror("poll() failed");
	}
#endif
#ifdef TiNGTYPE_WINNT
	DWORD d = WaitForSingleObject(chanp->txev, 0);
	switch (d) {
	case WAIT_OBJECT_0: printf("event:set"); break;
	case WAIT_TIMEOUT: printf("event:clear"); break;
	case WAIT_FAILED: lasterr("WaitForSingleObject failed"); break;
	default:
		printf("Unknown status (%d = 0x%x) from WaitForSingleObject\n", d, d);
	}
#endif
 }
 printf("\n");
 r = fn_smdc_line_status(chanp->chan, &p0, &p1, &p2, &p3);
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
 chanp->cfg->tx_tep = getnum(f);
 printf("TEP %sabled\n", chanp->cfg->tx_tep ? "en" : "dis");
 return 0;
}

static void set_txcap(CHAN *chanp, FILE *f)
{
 int x = getint(f);
 fn_smdc_tx_control(chanp->chan, kSMDCTxCtlNotifyOnCapacity, x);
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
 cp.channel = chanp->chan;
 cp.cmd = kSMDCLineCtlCmdSetPrefixSuffix;
 c = smdc_line_control(&cp);
 free(cp.prefix_data);
 free(cp.suffix_data);
 if (c) return printerr_sm("smdc_line_control", c);
 return 0;
}

static int set_train(CHAN *chanp, FILE *f)
{
 chanp->cfg->tx_shorttrain = getnum(f);
 printf("short training %sabled\n", chanp->cfg->tx_shorttrain ? "en" : "dis");
 return 0;
}

static int fn_smdc_tx_data(CHAN *chanp, void *buf, int len)
{
 for (;;) {
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chanp->chan;
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
 if (chanp->cfg->tx.smdc.encoding != kSMDCConfigEncodingHDLC) {
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

static void sendline(CHAN *chanp, FILE *f, int eol, int iswide, int odd)
{
 unsigned blen = 2048;
 char *buff = malloc(blen);
 unsigned bp = 0;
 if (!buff) {
	perror("Cannot malloc buffer");
	return;
 }
 if (chanp->cfg->tx.smdc.encoding == kSMDCConfigEncodingHDLC) bp = 4;
 for (;;) {
	int c = getesc(f);
	if (c == '\n') {
		ungetc(c, f);
		break;
	}
	if (c == EOF) break;
	buff[bp++] = c;
	if (iswide) buff[bp++] = 0;
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
	buff[bp++] = '\n';
	if (iswide) buff[bp++] = 0;
 }
 if (chanp->cfg->tx.smdc.encoding == kSMDCConfigEncodingHDLC) {
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
 if (chanp->cfg->tx.smdc.encoding == kSMDCConfigEncodingHDLC) bp = 4;
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
 if (chanp->cfg->tx.smdc.encoding == kSMDCConfigEncodingHDLC) {
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
 memset(&stop_parms, 0, sizeof(stop_parms));
 stop_parms.channel=chanp->chan;
 r = smdc_stop(&stop_parms);
 if (r) {
	printf("smdc_stop returned %d\n", r);
 }
}

static int fn_smdc_channel_config(CHAN* chan)
{
 SMDC_CHANNEL_CONFIG_PARMS ccp;
 int r;
 ccp = chan->cfg->tx.smdc;
 ccp.channel = chan->chan;
 r = smdc_channel_config(&ccp);
 if (r) return printerr_sm("smdc_channel_config(tx)", r);
 return 0;
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
 const int iswide = chanp->cfg->tx.smdc.encoding == kSMDCConfigEncodingAsync
 	&& ((SMDC_ASYNC_FORMAT_PARMS *) chanp->cfg->tx.smdc.encoding_config_data)
 		->databits > 8;
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
	ep.channel = chanp->chan;
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
	cp.channel = chanp->chan;
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
	fn_smdc_tx_control(chanp->chan, kSMDCTxCtlFinish, 2048);
 	break;
 case 'p':
 	set_prefsuf(chanp, f);
 	break;
 case 'R': {
	SMDC_LINE_CONTROL_PARMS cp;
	int r;
	memset(&cp, 0, sizeof(cp));
	cp.channel = chanp->chan;
	cp.cmd = kSMDCLineCtlCmdAssertRTS;
	cp.tep = chanp->cfg->tx_tep;
	cp.shorttrain = chanp->cfg->tx_shorttrain;
	r = smdc_line_control(&cp);
	if (r) return printerr_sm("smdc_line_control", r);
 }
	break;
 case 'r':
	fn_smdc_line_control(chanp->chan, kSMDCLineCtlCmdUnassertRTS, 0, 0);
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
 case 'T': {
	SMDC_LINE_CONTROL_PARMS cp;
	int r;
	int speed = getnum(f);
	memset(&cp, 0, sizeof(cp));
	cp.channel = chanp->chan;
	cp.cmd = kSMDCLineCtlCmdRetrain;
	cp.retrain_speed = speed;
	r = smdc_line_control(&cp);
	if (r) return printerr_sm("smdc_line_control", r);
 }
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

int runtx(CHAN *chanp, char *inifile)
{
 V18ASTATE vs;
 FILE *f;
 if (chanp->cfg->rx.smdc.protocol != kSMDCProtocolV32
	&& chanp->cfg->rx.smdc.protocol != kSMDCProtocolV110) {
	if (fn_smdc_channel_config(chanp)) return 1;
 }
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
