/* gdcrtprx.c - generic data communication rx tester */

#include <errno.h>
#include <pthread.h>
#include <malloc.h>
#include <stdio.h>
#include <smbesp.h>
#include <smdrvr.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "../Testlib/rtprx_codec.h"
#include "smrtp.h"
#include "smdc.h"

	// encodings
#include "smdc_async.h"
#include "smdc_hdlc.h"
#include "smdc_none.h"
#include "smdc_sync.h"

	// protocols
#include "smdc_bulk.h"
#include "smdc_cpu.h"
#include "smdc_cw.h"
#include "smdc_fsk.h"
#include "smdc_raw.h"
#include "smdc_ttyrx.h"
#include "smdc_v17.h"
#include "smdc_v27.h"
#include "smdc_v29.h"
#include "smdc_v32.h"

#include "../libutil/v18a.h"

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
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

int u = 0;
int uc = 0;
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
 case 'u':
	u |= c << (8 * uc);
	if (++uc == 4) {
		if (u == 0xFEFF) {
			fprintf(of->of, "\nBOM\n");
			of->col = 0;
		} else if (u == 0x2028) {
			fprintf(of->of, "\n");
			of->col = 0;
		} else if (u & 0xffffff80) {
			fprintf(of->of, " %08x ", u);
			of->col += 10;
		} else {
			putc(u, of->of);
		}
		uc = 0;
		u = 0;
	}
	break;
 case 'r':
 	putc(c, of->of);
 	return 0;
 case 'v':
 	v18a_show((void (*)(void *, char))cput, of, vs, c);
 }
 if (of->col > 64) {
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
			case 0xf:
				fprintf(of->of, "\\[final=%lx,idle]\n", hb->final);
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

static int mainloop(SMDC_CHANNEL_CONFIG_PARMS *smdcp, OUTF *of, char *format, tSMEventId *evp)
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
	if (evp) {
		r = smd_ev_wait(*evp);
		if (r) return printerr_sm("smd_ev_wait", r);
	}
	memset(&dp, 0, sizeof(dp));
	dp.channel = smdcp->channel;
	dp.data = (char*)buff;
	dp.max_length = sizeof(buff);
	r = smdc_rx_data(&dp);
	if (r) return printerr_sm("smdc_rx_data", r);
	if (!dp.done_length) {
		flushof(of);
		showstatus(smdcp->channel, &hist);
	} else {
		if (smdcp->encoding == kSMDCConfigEncodingHDLC) {
			if (handlehdlc(of, &vs, &hdlcbuf, buff,
				dp.done_length, format))
				return 1;
		} else {
			unsigned char *bp;
			int nc = dp.done_length;
			for (bp = buff; nc--; bp++) {
				if (showchar(of, &vs, *bp, format)) return 1;
			}
		}
		fflush(of->of);
	}
 }
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

static err_t connect_rtp_chan(tSMVMPrxId vmprx, tSMChannelId chan);
static tSMChannelId makechan(SMDC_CHANNEL_CONFIG_PARMS *smdcp, tSMModuleId module, tSMVMPrxId vmprx, int xferbits, unsigned xfertimeout, char *dest_ip, unsigned dest_port, char *source_ip, unsigned source_port)
{
 int r;
 {
  SM_CHANNEL_ALLOC_PLACED_PARMS pp;
  memset(&pp, 0, sizeof(pp));
  pp.type = kSMChannelTypeInput;
  pp.module = module;
  r = sm_channel_alloc_placed(&pp);
  if (r) {
	printerr_sm("sm_channel_alloc_placed", r);
	return kSMNullChannelId;
  }
  smdcp->channel = pp.channel;
 }
 {
	err_t err = connect_rtp_chan(vmprx, smdcp->channel);
	if (err) {
		error_log(stderr, error(err, "connect_rtp_chan() failed"));
		sm_channel_release(smdcp->channel);
		return kSMNullChannelId;
	}
 }
 if (dest_ip) {
	SM_CHANNEL_DISPATCHER_CONNECT_PARMS dcp;
	SM_UDP_DISPATCHER_CREATE_PARMS udcp;
	struct sockaddr* sap;
	memset(&udcp, 0, sizeof(udcp));
	udcp.module = module;
	if (lookup_ip(&sap, dest_ip, dest_port) != 0) {
		sm_channel_release(smdcp->channel);
		return kSMNullChannelId;
	}
	if (sap->sa_family == AF_INET) {
		memcpy(&udcp.destination, sap, sizeof(struct sockaddr_in));
	} else {
		memcpy(&udcp.destination_ipv6, sap, sizeof(struct sockaddr_in6));
		udcp.type = kSMDispatcherTypeIPv6;
	}
	free(sap);
	if (source_ip) {
		if (lookup_ip(&sap, source_ip, source_port) != 0) {
			sm_channel_release(smdcp->channel);
			return kSMNullChannelId;
		}
		if (sap->sa_family == AF_INET && udcp.type == kSMDispatcherTypeIPv4) {
			memcpy(&udcp.source, sap, sizeof(struct sockaddr_in));
		}
		if (sap->sa_family == AF_INET6 && udcp.type == kSMDispatcherTypeIPv6) {
			memcpy(&udcp.source_ipv6, sap, sizeof(struct sockaddr_in6));
		}
		free(sap);
	}
	r = sm_udp_dispatcher_create(&udcp);
	if (r) {
		printerr_sm("sm_udp_dispatcher_create", r);
		sm_channel_release(smdcp->channel);
		return kSMNullChannelId;
	}
	memset(&dcp, 0, sizeof(dcp));
	dcp.channel = smdcp->channel;
	dcp.dest = udcp.dispatcher;
	r = sm_channel_dispatcher_connect(&dcp);
	if (r) {
		printerr_sm("sm_channel_dispatcher_connect", r);
		sm_channel_release(smdcp->channel);
		return kSMNullChannelId;
	}
 }
 r = smdc_channel_config(smdcp);
 if (r) {
 	printerr_sm("smdc_channel_config", r);
	sm_channel_release(smdcp->channel);
	return kSMNullChannelId;
 }
 if (xferbits || xfertimeout) {
	SMDC_RX_CONTROL_PARMS dp;
	memset(&dp, 0, sizeof(dp));
	dp.channel = smdcp->channel;
	dp.cmd = kSMDCRxCtlNotifyOnData;
	dp.min_to_collect = xferbits;
	dp.min_idle = xfertimeout;
	r = smdc_rx_control(&dp);
	if (r) {
		printerr_sm("smdc_rx_control", r);
		sm_channel_release(smdcp->channel);
		return kSMNullChannelId;
	}
 }
 return smdcp->channel;
}

static int closechan(tSMChannelId chan)
{
 int r = sm_channel_release(chan);
 if (r) printerr_sm("sm_channel_release", r);
 return r;
}

static int runrx(SMDC_CHANNEL_CONFIG_PARMS *smdcp, tSMModuleId module, tSMVMPrxId vmprx, FILE *of, char *format, int useevent, int xferbits, unsigned xfertimeout, char *dest_ip, unsigned dest_port, char *source_ip, unsigned source_port)
{
 tSMEventId ev;
 OUTF outf;
 int i = 0;
 smdcp->channel = makechan(smdcp, module, vmprx, xferbits, xfertimeout, dest_ip, dest_port, source_ip, source_port);
 if (smdcp->channel == kSMNullChannelId) return 1;
 outf.of = of;
 outf.col = 0;
 if (useevent) {
	SM_CHANNEL_SET_EVENT_PARMS evp;
	i = smd_ev_create(&ev, smdcp->channel,
		kSMEventTypeReadData, kSMChannelSpecificEvent);
	if (i) return printerr_sm("smd_ev_create", i);
	memset(&evp, 0, sizeof(evp));
	evp.channel = smdcp->channel;
	evp.event_type = kSMEventTypeReadData;
	evp.issue_events = kSMChannelSpecificEvent;
	evp.event = ev;
	i = sm_channel_set_event(&evp);
	if (i) {
		smd_ev_free(ev);
		return printerr_sm("sm_channel_set_event", i);
	}
 }
 if (!i) i = mainloop(smdcp, &outf, format, useevent ? &ev : 0);
 if (useevent) {
 	int r = smd_ev_free(ev);
	if (r) {
		printerr_sm("smd_ev_free", r);
		if (!i) i = r;
	}
 }
 if (closechan(smdcp->channel) && !i) i = 1;
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

static unsigned auto_crc_size(unsigned crc)
{
 unsigned i;
 if (!crc) return 0;
 for (i=0; crc; crc >>= 1) i++;
 return i - 1;
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

static err_t stop_vmprx(tSMVMPrxId vmprx)
{
 SM_VMPRX_STOP_PARMS vsp;
 int e;
 memset(&vsp, 0, sizeof(vsp));
 vsp.vmprx = vmprx;
 e = sm_vmprx_stop(&vsp);
 if (e) return prosody_error(e, "sm_vmprx_stop() failed");
 return 0;
}

#define FORMAT_IN_ADDR "%d.%d.%d.%d"
#define ARGS_IN_ADDR(x) \
	((ntohl((x).s_addr) >> 24) & 0xff), \
	((ntohl((x).s_addr) >> 16) & 0xff), \
	((ntohl((x).s_addr) >> 8) & 0xff), \
	(ntohl((x).s_addr) & 0xff)


#define FORMAT_IN6_ADDR "%x:%x:%x:%x:%x:%x:%x:%x"
#define ARGS_IN6_ADDR(x) \
	((((x).s6_addr[0])<<8)+((x).s6_addr[1])), \
	((((x).s6_addr[2])<<8)+((x).s6_addr[3])), \
	((((x).s6_addr[4])<<8)+((x).s6_addr[5])), \
	((((x).s6_addr[6])<<8)+((x).s6_addr[7])), \
	((((x).s6_addr[8])<<8)+((x).s6_addr[9])), \
	((((x).s6_addr[10])<<8)+((x).s6_addr[11])), \
	((((x).s6_addr[12])<<8)+((x).s6_addr[13])), \
	((((x).s6_addr[14])<<8)+((x).s6_addr[15]))

static int handle_vmprx(tSMVMPrxId vmprx)
{
	SM_VMPRX_STATUS_PARMS statusp;
	SM_VMPRX_PORT_PARMS portp;
	int e;
	err_t err = 0;
	memset(&statusp,0,sizeof(statusp));
	statusp.vmprx = vmprx;
	e = sm_vmprx_status(&statusp);
	if (e) err = prosody_error(e, "sm_vmprx_status() failed");
	else if (statusp.status == kSMVMPrxStatusStopped) {
		printf("Status: STOPPED\n");
		return 1;
	} else if (statusp.status == kSMVMPrxStatusGotPorts) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) {
			err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		} else {
			printf("RTP port = " FORMAT_IN_ADDR ":%u, RTCP port = %u\n", ARGS_IN_ADDR(portp.address), portp.RTP_port, portp.RTCP_port);
		}
	} else if (statusp.status == kSMVMPrxStatusGotPortsIPv6) {
		memset(&portp, 0, sizeof(portp));
		portp.vmprx = vmprx;
		portp.nowait = 1;
		e = sm_vmprx_get_ports(&portp);
		if (e) {
			err =  prosody_error(e, "sm_vmprx_get_ports() failed");
		} else {
			printf("RTP port = [" FORMAT_IN6_ADDR "]:%u, RTCP port = %u\n", ARGS_IN6_ADDR(portp.ipv6_address), portp.RTP_port, portp.RTCP_port);
		}
	} else if(statusp.status == kSMVMPrxStatusDetectTone) {
		printf("Tone - %d %f\n",statusp.u.tone.id, statusp.u.tone.volume);
	} else if(statusp.status == kSMVMPrxStatusEndTone) {
		printf("Tone end - %d %f %d\n",statusp.u.tone.id, statusp.u.tone.volume, statusp.u.tone.duration);
	} else if(statusp.status == kSMVMPrxStatusNewSSRC) {
		printf("SSRC - %d from " FORMAT_IN_ADDR ":%u\n",statusp.u.ssrc.ssrc, ARGS_IN_ADDR(statusp.u.ssrc.address), statusp.u.ssrc.port);
	} else if(statusp.status == kSMVMPrxStatusNewSSRCIPv6) {
		printf("SSRC - %d from [" FORMAT_IN6_ADDR "]:%u\n",statusp.u.ssrc_ipv6.ssrc, ARGS_IN6_ADDR(statusp.u.ssrc_ipv6.address), statusp.u.ssrc_ipv6.port);
	} else if(statusp.status == kSMVMPrxStatusUnhandledPayload) {
		printf("Unhandled payload type - %d\n",statusp.u.payload.type);
	} else if (statusp.status == kSMVMPrxStatusCodecSpecific) {
		printf("Codec specific payload type - %d\n",statusp.u.codec_specific.payload_type);
		e = sm_vmprx_status_discard_codec_specific(vmprx);
		if (e) err = prosody_error(e, "sm_vmprx_status_discard_codec_specific() failed");
	}
	if (err) {
		error_log(stderr, error(err, "handle_vmprx() failed"));
		stop_vmprx(vmprx);
		return 1;
	}
	return 0;	
}

static err_t create_vmprx(tSMVMPrxId *pvmprx, tSMModuleId module, unsigned ipv6)
{
 SM_VMPRX_CREATE_PARMS vcp;
 int e;
 memset(&vcp, 0, sizeof(vcp));
 vcp.module = module;
 if (ipv6) vcp.type = kSMVMPrxTypeIPv6;
 e = sm_vmprx_create(&vcp);
 if (e) return prosody_error(e, "sm_vmprx_create() failed");
 *pvmprx = vcp.vmprx;
 return 0;
}

static err_t get_event(tSMVMPrxId vmprx, tSMEventId* pev)
{
 SM_VMPRX_EVENT_PARMS evp;
 int e;
 memset(&evp, 0, sizeof(evp));
 evp.vmprx = vmprx;
 e = sm_vmprx_get_event(&evp);
 if (e) return prosody_error(e, "sm_vmprx_get_event() failed");
 *pev = evp.event;
 return 0;
}

static err_t config_vmprx(tSMVMPrxId vmprx, RTPRX_PAR* rtprx_par, char* srtp_key)
{
 err_t err = rtprx_config_vmprx(vmprx, rtprx_par);
 if (err) return err;
 if (srtp_key) {
	int e;
	SM_VMPRX_CONFIG_ENCRYPTION_AES_CM_PARMS ep;
	SM_VMPRX_CONFIG_AUTHENTICATION_HMAC_SHA1_PARMS ap;
	memset(&ap, 0, sizeof(ap));
	ap.vmprx = vmprx;
	ap.key = srtp_key;
	ap.keylen = 30;
	ap.taglen = 80;
	e = sm_vmprx_config_authentication_hmac_sha1(&ap);
	if (e) return prosody_error(e, "sm_vmprx_config_authentication_hmac_sha1() failed");

	memset(&ep, 0, sizeof(ep));
	ep.vmprx = vmprx;
	ep.key = srtp_key;
	ep.keylen = 30;
	e = sm_vmprx_config_encryption_aes_cm(&ep);
	if (e) return prosody_error(e, "sm_vmprx_config_encryption_aes_cm() failed");
 }
 return 0;
}

static err_t connect_rtp_chan(tSMVMPrxId vmprx, tSMChannelId chan)
{
 SM_VMPRX_DATAFEED_PARMS vdgp;
 SM_CHANNEL_DATAFEED_CONNECT_PARMS cp;
 int e;
 memset(&vdgp, 0, sizeof(vdgp));
 vdgp.vmprx = vmprx;
 e = sm_vmprx_get_datafeed(&vdgp);
 if (e) return prosody_error(e, "sm_vmprx_get_datafeed() failed");
 memset(&cp, 0, sizeof(cp));
 cp.channel = chan;
 cp.data_source = vdgp.datafeed;
 e = sm_channel_datafeed_connect(&cp);
 if (e) return prosody_error(e, "sm_channel_datafeed_connect() failed");
 return 0;
}

	// the worker thread is this function
static void *worker_thread(void *p)
{
 tSMVMPrxId vmprx = p;
 tSMEventId ev;
 err_t err;
 err = get_event(vmprx, &ev);
 if (err) {
	error_log(stderr, error(err, "worker thread get_event() failed"));
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
	r = handle_vmprx(vmprx);
	if (r) break;
 }
 return 0;
}

	// start the worker thread
static err_t startworker(pthread_t *tidp, tSMVMPrxId vmprx)
{
 err_t err = 0;
 int e;
 e = pthread_create(tidp, 0, worker_thread, vmprx);
 if (e) {
 	err = error_errno(e, "pthread_create() failed");
 	err = error(err, "cannot start worker thread");
 }
 return err;
}

static int start_vmprx(tSMVMPrxId *pvmprx, tSMModuleId module, RTPRX_PAR* rtprx_par, unsigned use_ipv6, char* srtp_key)
{
	pthread_t tid;
	err_t err = create_vmprx(pvmprx, module, use_ipv6);
	if (err) {
		error_log(stderr, error(err, "create_vmprx() failed"));
		return 1;
	}
	err = config_vmprx(*pvmprx, rtprx_par, srtp_key);
	if (err) {
		error_log(stderr, error(err, "config_vmprx() failed"));
		return 1;
	}
	err = startworker(&tid, *pvmprx);
	if (err) {
		error_log(stderr, error(err, "startworker() failed"));
		return 1;
	}
	return 0;
}

static int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const unsigned char *data = (const unsigned char *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   unsigned int n = 0;
   size_t blocks = dataLength / 3;
   size_t padCount = dataLength % 3;
   unsigned char n0, n1, n2, n3;

   if (padCount) blocks++;
   if (resultSize < (blocks << 2)) return -1; // result not big enough

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = data[x] << 16;
 
      if((x+1) < dataLength)
         n += data[x+1] << 8;
 
      if((x+2) < dataLength)
         n += data[x+2];
 
      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (unsigned char)(n >> 18) & 63;
      n1 = (unsigned char)(n >> 12) & 63;
      n2 = (unsigned char)(n >> 6) & 63;
      n3 = (unsigned char)n & 63;
 
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      result[resultIndex++] = base64chars[n0];
      result[resultIndex++] = base64chars[n1];
 
      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         result[resultIndex++] = base64chars[n2];
      }
 
      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         result[resultIndex++] = base64chars[n3];
      }
   }  
 
   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         result[resultIndex++] = '=';
      } 
   }
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}


#include "gen/gdcrtprx.args.i"

int main(int argc, char **argv)
{
 SMDC_CHANNEL_CONFIG_PARMS smdc;
 tSMModuleId module;
 tSMCardId card;
 tSMVMPrxId vmprx = 0;
 int iErr;
 union {
	SMDC_CW_CONFIG_PARMS cw;
	SMDC_FSK_CONFIG_PARMS fsk;
	SMDC_V17_CONFIG_PARMS v17;
	SMDC_V27_CONFIG_PARMS v27;
	SMDC_V29_CONFIG_PARMS v29;
	SMDC_V32_CONFIG_PARMS v32;
 } mod;
 union {
	SMDC_ASYNC_FORMAT_PARMS async;
	SMDC_HDLC_FORMAT_PARMS hdlc;
 } enc;
 char *format;
 FILE *outf;
 RTPRX_PAR rtprx_par;
 unsigned use_ipv6;
 char* srtp_key = NULL;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 memset(&smdc, 0, sizeof(smdc));
 memset(&mod, 0, sizeof(mod));
 memset(&enc, 0, sizeof(enc));
 if (!arg.modulation) {
	fprintf(stderr, "%s: missing modulation option\n", progname);
	return 1;
 }
 if (!strcmp(arg.modulation, "raw")) {
 	smdc.protocol = kSMDCProtocolRawRx;
 } else if (!strcmp(arg.modulation, "bulk")) {
 	smdc.protocol = kSMDCProtocolBulkRx;
 } else if (!strcmp(arg.modulation, "cpu")) {
 	smdc.protocol = kSMDCProtocolCpu;
 } else if (!strcmp(arg.modulation, "tty")) {
 	smdc.protocol = kSMDCProtocolTTYrx;
 } else if (!strcmp(arg.modulation, "cw")) {
 	smdc.protocol = kSMDCProtocolCWrx;
	smdc.config_length = sizeof(mod.cw);
	smdc.config_data = &mod.cw;
	if (arg.spacefreq) mod.cw.space_frequency = arg.spacefreq;
	if (arg.bitrate) mod.cw.speed = arg.bitrate;
 } else if (!strcmp(arg.modulation, "fsk")) {
 	smdc.protocol = kSMDCProtocolFSKrx;
	smdc.config_length = sizeof(mod.fsk);
	smdc.config_data = &mod.fsk;
	if (arg.standard) {
		if (lookup_fsk(&mod.fsk, arg.standard)) {
			fprintf(stderr, "%s: bad option\n", progname);
			return 1;
		}
	}
	mod.fsk.rx_carrier_on_mS = arg.carrier_on;
	mod.fsk.rx_carrier_off_mS = arg.carrier_off;
	if (arg.markfreq) mod.fsk.mark_frequency = arg.markfreq;
	if (arg.spacefreq) mod.fsk.space_frequency = arg.spacefreq;
	if (arg.bitrate) mod.fsk.speed = arg.bitrate;
 } else if (!strcmp(arg.modulation, "v17")) {
 	smdc.protocol = kSMDCProtocolV17rx;
	if (arg.bitrate) mod.v17.speed = arg.bitrate;
	smdc.config_length = sizeof(mod.v17);
	smdc.config_data = &mod.v17;
 } else if (!strcmp(arg.modulation, "v27")) {
 	smdc.protocol = kSMDCProtocolV27rx;
	if (arg.bitrate) mod.v27.speed = arg.bitrate;
	smdc.config_length = sizeof(mod.v27);
	smdc.config_data = &mod.v27;
 } else if (!strcmp(arg.modulation, "v29")) {
 	smdc.protocol = kSMDCProtocolV29rx;
	if (arg.bitrate) mod.v29.speed = arg.bitrate;
	smdc.config_length = sizeof(mod.v29);
	smdc.config_data = &mod.v29;
 } else if (!strcmp(arg.modulation, "v32")) {
 	smdc.protocol = kSMDCProtocolV32rx;
	if (arg.bitrate) mod.v32.speed = arg.bitrate;
	mod.v32.isanswer = arg.answer;
	smdc.config_length = sizeof(mod.v32);
	smdc.config_data = &mod.v32;
 } else {
	fprintf(stderr, "%s: unknown modulation option: %s\n",
		progname, arg.modulation);
	fprintf(stderr, "Valid options are: bulk cpu fsk raw tty v17 v27 v29 v32\n");
	return 1;
 }
 if (arg.encoding) {
	if (!strcmp(arg.encoding, "sync")) {
		smdc.encoding = kSMDCConfigEncodingSync;
	} else if (!strcmp(arg.encoding, "async")) {
		smdc.encoding = kSMDCConfigEncodingAsync;
		smdc.encoding_config_data = &enc.async;
		smdc.encoding_config_length = sizeof(enc.async);
		enc.async.databits = arg.databits;
	} else if (!strcmp(arg.encoding, "hdlc")) {
		smdc.encoding = kSMDCConfigEncodingHDLC;
		smdc.encoding_config_data = &enc.hdlc;
		smdc.encoding_config_length = sizeof(enc.hdlc);
		if (arg.crc) enc.hdlc.crc = arg.crc;
		if (arg.crcsize) enc.hdlc.crcsize = arg.crcsize;
		else enc.hdlc.crcsize = auto_crc_size(enc.hdlc.crc);
		if (arg.min_flags) enc.hdlc.rx_min_initial_flags = arg.min_flags;
		if (arg.min_ones) enc.hdlc.rx_min_ones = arg.min_ones;
	} else {
		fprintf(stderr, "%s: unknown encoding option: %s\n",
			progname, arg.encoding);
		fprintf(stderr, "Valid options are: sync async hdlc\n");
		return 1;
	}
 }
 if (arg.outfile) {
	outf = fopen(arg.outfile, "wb");
	if (!outf) {
		perror("fopen");
		fprintf(stderr, "Cannot create file: %s\n", arg.outfile);
		return 1;
	}
 } else outf = stdout;
 if (arg.format) format = arg.format;
 else format = "c";
 if (arg.serialnumber) {
	err_t e = modopen(&card, &module, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 if (!arg.codec || rtprx_init_par(&rtprx_par, arg.codec)) {
	fprintf(stderr, "RTP codec required\nUsage: %s" ARGS_USAGE "\nFormats:", progname);
	rtprx_dump_codecs(stderr);
	return -1;
 }
 use_ipv6 = arg.use_ipv6;
 rtprx_par.regen_tones = arg.regen_tones;
 rtprx_par.enforce_tone_spacing = arg.enforce_tone_spacing;
 rtprx_par.frame_len = arg.frame_len;
 rtprx_par.detect_tones = arg.detect_tones;
 rtprx_par.tone_payload_mapping = arg.rfc2833tones;
 rtprx_par.init_ms = arg.jitter_initial_ms;
 rtprx_par.max_ms = arg.jitter_max_ms;
 rtprx_par.ad_target_delay = arg.ad_target_delay;
 rtprx_par.ad_freq_upper_tolerance = arg.ad_freq_upper_tolerance;
 rtprx_par.ad_freq_lower_tolerance = arg.ad_freq_lower_tolerance;
 rtprx_par.payloadmapping = arg.payloadmapping;
 rtprx_par.plc = arg.plc;
 rtprx_par.bitrate = arg.codec_bitrate;
 rtprx_par.variant = arg.variant;
 rtprx_par.handle_cn = !!arg.comfort_noise;
 rtprx_par.comfort_noise_payload_mapping = arg.comfort_noise_payload_mapping;

 if (arg.use_srtp) {
	char b64[41];
	int i;
	srtp_key = malloc(30);
	// fill with random data
	srand(time(NULL));
	for(i=0;i<30;i++) {
		srtp_key[i] = rand();
	}
	// print value as base64 encoded string for passing to transmitter
	base64encode(srtp_key, 30, b64, 40);
	b64[40] = 0;
	printf("SRTP key:%s\n", b64);
 }

 iErr = start_vmprx(&vmprx, module, &rtprx_par, use_ipv6, srtp_key);


 if (!iErr) iErr = runrx(&smdc, module, vmprx, outf, format, arg.useevent, arg.xferbits, arg.xfertimeout, arg.dest_ip, arg.dest_port, arg.source_ip, arg.source_port);
 if (arg.serialnumber) {
	modclose(module);
	cardclose(card);
 }
 free(srtp_key);
 return iErr;
}
