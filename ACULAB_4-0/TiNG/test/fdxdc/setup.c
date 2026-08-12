#include "fdxdc.h"
#include <stdio.h>
#include <string.h>
#include "../Testlib/errcode_sm.h"

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


static tSMChannelId alloc_chan(tSMModuleId mod)
{
 SM_CHANNEL_ALLOC_PLACED_PARMS pp;
 int r;
 memset(&pp, 0, sizeof(pp));
 pp.type = kSMChannelTypeFullDuplex;
 pp.module = mod;
 r = sm_channel_alloc_placed(&pp);
 if (r) {
	printerr_sm("sm_channel_alloc_placed", r);
	return kSMNullChannelId;
 }
 return pp.channel;
}

static int createcoll(CHAN *chan, tSMModuleId mod)
{
 SM_UDP_COLLECTOR_CREATE_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.module = mod;
 if (chan->cfg->tx_local_ip->sa_family == AF_INET) {
	cp.address = ((struct sockaddr_in*)chan->cfg->rx_dest_ip)->sin_addr;
 } else {
	memcpy(cp.ipv6_address.s6_addr, ((struct sockaddr_in6*)chan->cfg->tx_local_ip)->sin6_addr.s6_addr, 16);
	cp.type = kSMCollectorTypeIPv6;
 }
 r = sm_udp_collector_create(&cp);
 if (r) return printerr_sm("sm_udp_collector_create", r);
 chan->coll = cp.collector;
 return 0;
}

static int coll_getevent(CHAN *chan, tSMEventId *evp)
{
 SM_COLLECTOR_EVENT_PARMS ep;
 int r;
 memset(&ep, 0, sizeof(ep));
 ep.collector = chan->coll;
 r = sm_collector_get_event(&ep);
 if (r) return printerr_sm("sm_collector_get_event", r);
 *evp = ep.event;
 return 0;
}

static int makecoll(CHAN *chan)
{
 tSMEventId ev;
 if (coll_getevent(chan, &ev)) return 1;
 for (;;) {
 	SM_COLLECTOR_STATUS_PARMS cp;
 	int r = smd_ev_wait(ev);
	if (r) return printerr_sm("smd_ev_wait", r);
	memset(&cp, 0, sizeof(cp));
	cp.collector = chan->coll;
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
		ccp.channel = chan->chan;
		ccp.source = chan->coll;
		r = sm_channel_collector_connect(&ccp);
		if (r) return printerr_sm("sm_channel_collector_connect", r);
	}
		return 0;
	case kSMCollectorStatusNewPeer: break;
	}
 }
}

static int setup_coll(CHAN *chan, tSMModuleId mod)
{
 chan->coll = kSMNullCollectorId;
 if (chan->cfg->tx_local_ip != NULL) {
	int r = createcoll(chan, mod);
	if (!r) r = makecoll(chan);
	if (r) return 1;
 }
 return 0;
}

static int setup_timeslots(CHAN *chan)
{
 SM_SWITCH_CHANNEL_PARMS sp;
 int r;
 memset(&sp, 0, sizeof(sp));
 sp.channel = chan->chan;
 sp.st = chan->cfg->tx.ts.stream;
 sp.ts = chan->cfg->tx.ts.timeslot;
 sp.type = chan->cfg->tx.ts.type;
 if (++chan->cfg->tx.ts.timeslot >= 32) {
  	chan->cfg->tx.ts.timeslot -= 32;
  	chan->cfg->tx.ts.stream++;
 }
 r = sm_switch_channel_output(&sp);
 if (r) return printerr_sm("sm_switch_channel_output", r);
 memset(&sp, 0, sizeof(sp));
 sp.channel = chan->chan;
 sp.st = chan->cfg->rx.ts.stream;
 sp.ts = chan->cfg->rx.ts.timeslot;
 sp.type = chan->cfg->rx.ts.type;
 if (++chan->cfg->rx.ts.timeslot >= 32) {
  	chan->cfg->rx.ts.timeslot -= 32;
  	chan->cfg->rx.ts.stream++;
 }
 r = sm_switch_channel_input(&sp);
 if (r) return printerr_sm("sm_switch_channel_input", r);
 return 0;
}

static int setup_event(tSMEventId *evp, CHAN *chan, int evtype)
{
 SM_CHANNEL_SET_EVENT_PARMS ep;
 int r;
 r = smd_ev_create(evp, chan->chan, evtype, kSMChannelSpecificEvent);
 if (r) return printerr_sm("smd_ev_create", r);
 memset(&ep, 0, sizeof(ep));
 ep.channel = chan->chan;
 ep.event_type = evtype;
 ep.issue_events = kSMChannelSpecificEvent;
 ep.event = *evp;
 r = sm_channel_set_event(&ep);
 if (r) return printerr_sm("sm_channel_set_event", r);
 return 0;
}

static int setup_tx_event(CHAN *chan)
{
 if (!chan->cfg->tx.useevent) return 0;
 return setup_event(&chan->txev, chan, kSMEventTypeWriteData);
}

static int setup_tx(CHAN *chan, tSMModuleId mod)
{
 return setup_coll(chan, mod)
 	|| setup_tx_event(chan);
}

static int setup_rx_disp(CHAN *chan, tSMModuleId mod)
{
 if (chan->cfg->rx_dest_ip) {
	SM_CHANNEL_DISPATCHER_CONNECT_PARMS dcp;
	SM_UDP_DISPATCHER_CREATE_PARMS udcp;
	int r;
	memset(&udcp, 0, sizeof(udcp));
	udcp.module = mod;
	if (chan->cfg->rx_dest_ip->sa_family == AF_INET) {
		udcp.destination.sin_addr = ((struct sockaddr_in*)chan->cfg->rx_dest_ip)->sin_addr;
		udcp.destination.sin_port = htons(((struct sockaddr_in*)chan->cfg->rx_dest_ip)->sin_port);
		udcp.source.sin_addr = ((struct sockaddr_in*)chan->cfg->rx_source_ip)->sin_addr;
		udcp.source.sin_port = htons(((struct sockaddr_in*)chan->cfg->rx_source_ip)->sin_port);
	} else {
		memcpy(udcp.destination_ipv6.sin6_addr.s6_addr, ((struct sockaddr_in6*)chan->cfg->rx_dest_ip)->sin6_addr.s6_addr, 16);
		udcp.destination_ipv6.sin6_port = htons(((struct sockaddr_in6*)chan->cfg->rx_dest_ip)->sin6_port);
		memcpy(udcp.source_ipv6.sin6_addr.s6_addr, ((struct sockaddr_in6*)chan->cfg->rx_source_ip)->sin6_addr.s6_addr, 16);
		udcp.source_ipv6.sin6_port = htons(((struct sockaddr_in6*)chan->cfg->rx_source_ip)->sin6_port);
		udcp.type = kSMDispatcherTypeIPv6;
	}
	r = sm_udp_dispatcher_create(&udcp);
	if (r) return printerr_sm("sm_udp_dispatcher_create", r);
	memset(&dcp, 0, sizeof(dcp));
	dcp.channel = chan->chan;
	dcp.dest = udcp.dispatcher;
	r = sm_channel_dispatcher_connect(&dcp);
	if (r) return printerr_sm("sm_channel_dispatcher_connect", r);
 }
 return 0;
}

static int setup_rx_event(CHAN *chan)
{
 if (!chan->cfg->rx.useevent) return 0;
 return setup_event(&chan->rxev, chan, kSMEventTypeReadData);
}

static int setup_rx_xfer(CHAN *chan)
{
 if (chan->cfg->rx_xferbits || chan->cfg->rx_xfertimeout) {
	SMDC_RX_CONTROL_PARMS dp;
	int r;
	memset(&dp, 0, sizeof(dp));
	dp.channel = chan->chan;
	dp.cmd = kSMDCRxCtlNotifyOnData;
	dp.min_to_collect = chan->cfg->rx_xferbits;
	dp.min_idle = chan->cfg->rx_xfertimeout;
	r = smdc_rx_control(&dp);
	if (r) return printerr_sm("smdc_rx_control", r);
 }
 return 0;
}

static int setup_rx(CHAN *chan, tSMModuleId mod)
{
 return setup_rx_event(chan)
	|| setup_rx_disp(chan, mod)
	|| setup_rx_xfer(chan);
}

static int setup_config(CHAN *chan)
{
 if (chan->cfg->rx.smdc.protocol != kSMDCProtocolNone) {
	SMDC_CHANNEL_CONFIG_PARMS ccp;
	int r;
	ccp = chan->cfg->rx.smdc;
	ccp.channel = chan->chan;
	r = smdc_channel_config(&ccp);
	if (r) return printerr_sm("smdc_channel_config(rx)", r);
 }
 if (chan->cfg->rx.smdc.protocol == kSMDCProtocolV110) {
	SMDC_LINE_CONTROL_PARMS cp;
	int r;
	memset(&cp, 0, sizeof(cp));
	cp.channel = chan->chan;
 	cp.cmd = chan->cfg->rx.isanswer
 		? kSMDCLineCtlCmdResponderConnect
 		: kSMDCLineCtlCmdInitiatorConnect;
	r = smdc_line_control(&cp);
	if (r) return printerr_sm("smdc_line_control", r);
 }
 return 0;
}

int setup_rxtx(CHAN *chan, tSMModuleId mod)
{
 chan->chan = alloc_chan(mod);
 if (chan->chan == kSMNullChannelId) return 1;
 if (setup_tx(chan, mod)
 	|| setup_rx(chan, mod)
 	|| setup_timeslots(chan)
 	|| setup_config(chan)) {
	sm_channel_release(chan->chan);
	return 1;
 }
 return 0;
}
