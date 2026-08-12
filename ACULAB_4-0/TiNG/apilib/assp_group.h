/* assp_group.h - group of ASSP connections running over same socket */

#ifndef INCLUDED_ASSP_GROUP_H
#define INCLUDED_ASSP_GROUP_H

#include "../assp/assp_packet.h"
#include "assp_conn.h"
#include "event.h"
#include "mutx.h"
#include "paranoia.h"
#include <pthread.h>

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif


#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
typedef unsigned long in_addr_t;
#endif

typedef int (*check_chan_ready)(void* context);

typedef struct {
	enum { CHAN_CLOSED, CHAN_OPEN } chanstate;
	struct assp_group *grp;
	PACKET *rxq, **rxq_last;
	CV rxcv;	// used with group mutex
	EVNT cxevnt;		// Prosody event (either rxq non empty)
	uint32_t taskid;	// Raw task id - no generation number
	uint32_t taskgen;	// Generation number - could be a much smaller number
	uint64_t activegenmask; // mask of generations that has started tasks but not seen dead msg yet
	unsigned discardmsgs; // discard incoming messages
	check_chan_ready ready_fn;
	void *ready_fn_context;
		// Invariant: cnx->cxchan[taskid] == this
} ASSP_CHAN;

typedef struct assp_group {
	PARAMUTX pm;	// only lock held by assp thread
	pthread_t tid;
	enum {
		kASSPGroupPending = -1,
		kASSPGroupRunning,
		kASSPGroupStopping,
		kASSPGroupStopped,
	} stop;
	EVNT group_event; // signal to notify conn thread to exit
	EVNT stopped_event; // signalled when conn thread exits
	unsigned numcxchan;
	uint32_t tx_extrabits;	//The extra bits that are currently being prepended to sent task ids
	uint32_t rx_extrabits;	//The extra bits that are currently being prepended to received task ids
	ASSP_CHAN **cxchan;
	struct assp_conn *conn;
	EVNT *conn_evnt;
	enum assp_conn_status conn_status;
} ASSP_GROUP;

LOCALDEC int asspgroup_ctor(ASSP_GROUP *grp, char *remaddr, char *asspmonaddr, int asspmonport, int port, char *key);
LOCALDEC void asspgroup_shutdown(ASSP_GROUP *grp);
LOCALDEC void asspgroup_dtor(ASSP_GROUP *grp);
LOCALDEC ASSP_CHAN *alloc_chan(ASSP_GROUP *grp);
LOCALDEC void assp_chan_close(ASSP_CHAN *chancx);
LOCALDEC ASSP_CHAN *channomagic2chan(ASSP_GROUP *grp, unsigned channo, unsigned magic);
LOCALDEC void task_nextgen(ASSP_CHAN *cnx);
LOCALDEC int task_genisactive(ASSP_CHAN *cnx);
LOCALDEC void task_activegen(ASSP_CHAN *cnx);
LOCALDEC int asspgroup_isconnected(ASSP_GROUP *grp); // call with group lock held
LOCALDEC void kickoffgroup(ASSP_GROUP *grp);

#endif
