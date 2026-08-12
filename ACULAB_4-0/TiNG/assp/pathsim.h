/* pathsim.h - network path simulation */

#ifndef INCLUDED_PATHSIM_H
#define INCLUDED_PATHSIM_H

#include "assp.h"
#include "gasdev.h"
#include "simevent.h"

typedef struct netpacket {
	struct netpacket *nextpkt;
	ABSTIME deltime;	// time when this packet has finished arriving
	unsigned dlen;
	uint8_t *data;
} NETPACKET;

typedef struct {
	char *name;
	NETPACKET *pktq;	// queue of packets
	NETPACKET **qnextp;	// where to put next packet
	ABSTIME linkfree;	// when next packet can start being transmitted
	ABSTIME prevdel;
	RAN2 randgen;
	GASDEV delay_dist;
		// config
	void (*deliver)(void *p, NETPACKET *np);
	void *deliver_par;
		// parameters
	double bps;		// bps of deliveries
	double ploss;
	double delay_mean, delay_dev;
	unsigned latency_uS;
	double probdup;
		// statistics
	unsigned nlost;
	unsigned nsent;
	unsigned maxtx;
		// these should be unsigned, but Microsoft compiler
		// cannot handle uint_fast64_t -> double conversion!
	int_fast64_t busy_uS;
	int_fast64_t blocked_uS;
	int_fast64_t idle_uS;
	unsigned long rxbits;
	int pktno;
} HOP;

struct tcbpath_impl {
	HOP *firsthop;
};

void hop_init(HOP *hop, char *name, HOP *nexthop, TCB *tcb, double bps, double ploss, double probdup, double delay_mean, double delay_dev, double latency);

void tcbpath_sim_init(TCBPATH *tp, TCBPATH_IMPL *path, HOP *firsthop);

void closepath(TCBPATH_IMPL *path, char *name);

#endif
