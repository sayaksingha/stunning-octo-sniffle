#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timeb.h>
#include "smdrvr.h"
//#include <conio.h>
#include "smdc.h"
#include "smdc_raw.h"

#include <malloc.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../../libutil/generic_io.h"

#define arlen(s) (sizeof(s)/sizeof(*(s)))

#ifdef TiNGTYPE_LINUX
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#include <pthread.h>
#include <unistd.h>
typedef struct pollfd WAITABLE;
#endif


#ifdef TiNGTYPE_WINNT
#include "libutil/WINNT/lasterr.h"
typedef HANDLE WAITABLE;
#endif

#include "../apilib/smdc_rlp.h"

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

	// handy utilities
static char *endstr(char *s)
{
 while (*s) s++;
 return s;
}

	// convenient encapsulations of Prosody API functions
static int chan_config_raw(tSMChannelId chan)
{
 SMDC_CHANNEL_CONFIG_PARMS sc;
 int r;
 memset(&sc, 0, sizeof(sc));
 sc.channel = chan;
 sc.protocol = kSMDCProtocolRawRx;
 sc.config_length = 0;
 sc.config_data = 0;
 r = smdc_channel_config(&sc);
 if (r) return printerr_sm("smdc_channel_config", r);
 return 0;
}

static int chan_rx_control(tSMChannelId chan, int cmd, int mincol, int minidl)
{
 int r;
 SMDC_RX_CONTROL_PARMS dp;
 memset(&dp, 0, sizeof(dp));
 dp.channel = chan;
 dp.cmd = cmd;
 dp.min_to_collect = mincol;
 dp.min_idle = minidl;
 r = smdc_rx_control(&dp);
 if (r) return printerr_sm("smdc_rx_control", r);
 return 0;
}

static int chan_rx_data(int *rxlen, tSMChannelId chan, void *buf, int len)
{
 for (;;) {
	SMDC_RX_STATUS_PARMS sp;
	SMDC_DATA_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan;
	dp.data = buf;
	dp.max_length = len;
	r = smdc_rx_data(&dp);
	if (r) return printerr_sm("smdc_rx_data", r);
	if ((*rxlen =  dp.done_length)) break;
	memset(&sp, 0, sizeof(sp));
	sp.channel = chan;
	r = smdc_rx_status(&sp);
	if (r) return printerr_sm("smdc_rx_status", r);
 }
 return 0;
}

struct rxparam {
	enum cstate {CHAN_DEAD, CHAN_STOPPING, CHAN_RUNNING} cstate;
	int busy;
	FILE *rxfile;
	tSMChannelId chan;
	unsigned long maxrx;
	char endmsg[256];
	struct timeb rxstart, rxlast;
	unsigned long rxcnt;
	int sts_link, sts_linkrx, sts_linkcts;
	int sts_rxflow;
	unsigned char rxpat;
};

static const char *initrx(struct rxparam *rxp)
{
 rxp->rxcnt=0;
 rxp->sts_link=0xaaaa;
 rxp->sts_linkrx=0xaaaa;
 rxp->sts_linkcts=0xaaaa;
 rxp->sts_rxflow=0xaaaa;
 rxp->rxpat = 0;
 if (chan_rx_control(rxp->chan, kSMDCRxCtlNotifyOnData, 256, 50)) {
	return "control failed";
 }
 *rxp->endmsg = 0;
 return 0;
}

static int updaterx(struct rxparam *rxp)
{
 unsigned char buff[1024];
 int p0;
 //int p1, p2;
 if (rxp->cstate != CHAN_RUNNING) return 1;
 //if (chan_rx_status(&p0, &p1, &p2, rxp->chan)) return 1;
 //printf("Rxs: %d %d %d\n", p0, p1, p2);
 if (chan_rx_data(&p0, rxp->chan, buff, sizeof(buff))) return 1;
 //printf("got %d\n", p0);
 rxp->busy = 1;
 ftime(&rxp->rxlast);
 if (!rxp->rxcnt) rxp->rxstart = rxp->rxlast;
 if (0 && p0 != sizeof(buff)) {
	fprintf(stderr, "Partial read: %d of %ld\n",
		p0, (long) sizeof(buff));
	return 1;
 }
 if (rxp->rxfile) {
	if (fwrite(buff, 1, p0, rxp->rxfile) != (unsigned) p0) {
		perror("fwrite failed");
		return 1;
	}
	fflush(rxp->rxfile);
 }
 rxp->rxcnt += p0;
 if ((unsigned) p0 > rxp->maxrx) rxp->maxrx = 0;
 else rxp->maxrx -= p0;
 if (!rxp->maxrx) {
	sprintf(rxp->endmsg, "[limit] ");
	return 1;
 }
 return 0;
}

static void finishrx(struct rxparam *rxp)
{
 if (rxp->rxfile) fclose(rxp->rxfile);
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
}

	// initialisation

static int startchan(struct rxparam *rxp, SM_CHANNEL_ALLOC_PLACED_PARMS *sa, MVIP *ints, unsigned long maxrx, char *pfx, struct pollfd *fdp)
{
 SM_SWITCH_CHANNEL_PARMS swp;
 tSMEventId ev;
 int r;
 const char *s;
 r = sm_channel_alloc_placed(sa);
 if (r) return printerr_sm("sm_channel_alloc_placed", r);
 memset(&swp, 0, sizeof(swp));
 swp.channel = rxp->chan = sa->channel;
 if (pfx) {
	char *rxfname = malloc(strlen(pfx)+sizeof(".000"));
	if (!rxfname) {
		perror("malloc failed");
		return 1;
	}
	sprintf(rxfname, "%s-%d-%d", pfx, ints->stream, ints->timeslot);
	rxp->rxfile = fopen(rxfname, "wb");
	if (!rxp->rxfile) {
		perror("Cannot create file");
		fprintf(stderr, "File: %s\n", rxfname);
		free(rxfname);
		return 1;
	}
	free(rxfname);
 } else rxp->rxfile = 0;
 if (chan_config_raw(rxp->chan)) return 1;
 swp.st = ints->stream;
 swp.ts = ints->timeslot;
 swp.type = ints->type;
 if (++ints->timeslot >= 32) {
	ints->timeslot = 0;
	ints->stream++;
 }
 r = sm_switch_channel_input(&swp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 rxp->maxrx = maxrx;
 rxp->busy = 0;
 rxp->cstate = CHAN_RUNNING;
 if ( (s = initrx(rxp)) ) {
	fprintf(stderr, "cannot initialise rx: %s\n", s);
	return 1;
 }
 smd_ev_create(&ev, rxp->chan, kSMEventTypeReadData, kSMChannelSpecificEvent);
 fdp->fd = ev.fd;
 fdp->events = ev.mode;
 return 0;
}

	// periodic status display

static void showsts(struct rxparam *rxp, int numchan)
{
 static char twiddle[] = "-\\|/";
 static int twidpos;
 char *disp = malloc(numchan*3+sizeof(" \r"));
 char *cp = disp;
 int chan;
 if (!disp) return;
 *cp++ = '\r';
 for (chan=0; chan<numchan; chan++) {
	switch (rxp[chan].cstate) {
	case CHAN_DEAD: *cp++ = '_'; break;
	case CHAN_STOPPING: *cp++ = 'x'; break;
	case CHAN_RUNNING:
		if (rxp[chan].busy) *cp++ = 'R';
		else *cp++ = '.';
	}
	rxp[chan].busy = 0;
 }
 *cp++ = twiddle[twidpos];
 if (++twidpos == arlen(twiddle)-1) twidpos = 0;
 *cp = 0;
 fputs(disp, stdout);
 free(disp);
}

	// starting everything up and shutting it down

static int testraw(tSMModuleId modnum, unsigned long maxrx, MVIP *ints, char *pfx, unsigned numchan)
{
 unsigned nfds = numchan;	// N*rx+kbd
 unsigned minfd = 0;
 struct pollfd *fds = (struct pollfd *) malloc((nfds+1) * sizeof(*fds));
 SM_CHANNEL_ALLOC_PLACED_PARMS sa;
 struct timeb laststs;
 struct rxparam *rxp;
 unsigned chan;
 	// general initialisation
 // rxp[X] is fd X
 if (!fds) {
	fprintf(stderr, "Cannot allocate memory for file descriptors\n");
	return 1;
 }
 rxp = (struct rxparam *) malloc(numchan * sizeof(*rxp));
 if (!rxp) {
	fprintf(stderr, "Cannot allocate memory for rx channel params\n");
	return 1;
 }
 for (chan=0; chan<numchan; chan++) {
	rxp[chan].cstate = CHAN_DEAD;
 }
 	// initialisation needed by data transfer threads
 memset(&sa, 0, sizeof(sa));
 sa.type = kSMChannelTypeInput;
 sa.module = modnum;
 for (chan=0; chan < numchan; chan++) {
	int sts;
	sts = startchan(
		rxp+chan,
		&sa,
		ints,
		maxrx,
		pfx,
		fds+chan);
	if (sts) return 1;
 }
 fds[nfds].events = POLLIN | POLLHUP;
 fds[nfds].fd = 0;
 ftime(&laststs);
 for (;;) {
	struct timeb now;
	int i;
	ftime(&now);
	i = 1 + laststs.time - now.time;	// seconds until next status due
	i = 1000 * i;				// convert to mS
	i += laststs.millitm - now.millitm;	// include mS
	if (i <= 0) {
		showsts(rxp, numchan);
		i = 1000;
		laststs = now;
	}
	while (fds[minfd].fd == -1) minfd++;	// trim initial closed chans
	if (minfd >= nfds) break;	// only kbd left
	i = poll(fds, nfds+1, i);
	if (i < 0) {
		perror("poll() failed");
		break;
	}
	if (fds[nfds].revents & POLLIN) {	// kbd
		char c;
		fds[nfds].revents &= ~POLLIN;
		i = read(fds[nfds].fd, &c, 1);
		if (i < 0) {
			perror("read() failed");
			break;
		}
		if (!i) break;
		switch (c) {
		case '-':
			for (chan=0; chan < numchan; chan++) {
				if (rxp[chan].cstate == CHAN_RUNNING) {
					rxp[chan].cstate = CHAN_STOPPING;
					sm_channel_release(rxp[chan].chan);
					printf("Stopping %d \n", chan);
					break;
				}
			}
			break;
		case 'q':
			for (chan=0; chan < numchan; chan++) {
				if (rxp[chan].cstate != CHAN_DEAD) {
					rxp[chan].cstate = CHAN_STOPPING;
					sm_channel_release(rxp[chan].chan);
				}
			}
			break;
		}
	} else {
		struct pollfd *pfd;
		unsigned j;
		int perr;
		for (pfd=fds, j=0; j < nfds; pfd++, j++) {
			if (pfd->revents & (POLLIN|POLLHUP|POLLNVAL)) {	// rx
				if (updaterx(rxp+j)
					|| pfd->revents & (POLLHUP|POLLNVAL)) {
					int r;
					pfd->fd = -1;
					finishrx(rxp+j);
					printf("%dr: %s\n", j, rxp[j].endmsg);
					r = sm_channel_release(rxp[j].chan);
					if (r) return printerr_sm("sm_channel_release", r);
					rxp[j].cstate = CHAN_DEAD;
				}
				pfd->revents &= ~(POLLIN|POLLHUP|POLLNVAL);
			}
		}
		perr = 0;
		for (j=0; j < nfds; j++) {
			if (fds[j].revents) {
				printf("fd[%d].revents = 0x%x, fd=%d\n",
					j, fds[j].revents, fds[j].fd);
				perr = 1;
			}
		}
		//if (perr) break;
	}
 }
 free(rxp);
 free(fds);
 return 0;
}

#include "gen/polltestraw.args.i"

int main(int argc, char **argv)
{
 tSMModuleId module; 
 tSMCardId card;
 int sts;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.numchan) arg.numchan = 1;
 if (arg.maxrx) arg.maxrx *= 8000;
 else arg.maxrx = ~0;
 if (arg.serialnumber) {
	err_t e = modopen(&card, &module, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "Serial number is required\n Usage %s" ARGS_USAGE "\n",progname);
 }
 sts = testraw(module, arg.maxrx, &arg.intimeslot, arg.fileprefix, arg.numchan);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 return sts;
}
