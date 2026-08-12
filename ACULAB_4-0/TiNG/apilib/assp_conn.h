#ifndef INCLUDED_ASSP_CONN_H
#define INCLUDED_ASSP_CONN_H

#include "../assp/assp_packet.h"
#include "../assp/assp.h"
#include "smtypes.h"
#include "condvar.h"
#include "event.h"
#include "timequeue_pq_evnt.h"
#include "pathipv4.h"

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

enum assp_conn_status {
	kAsspConnCreated,
	kAsspConnRunning,
	kAsspConnDisconnected,
	kAsspConnStopping,
	kAsspConnFinished,
};

#define kMaxASSPConnRecvFailsBurst 4

struct crypt_parms {
	/* 
     * If zero, all payloads encrypted, 
	 * If bit 0 set, bulk (eg. replay) DATA to module plaintext, 
	 * If bit 1 set, bulk (eg. record) DATA to host plaintext 
     */ 
	int level;	
	int keyId;
	int newKeyId;
	int firstKeyIdPending;
	int keyChangePending;
	tSMSecCallback encdec;
	void* context;
};

struct assp_conn {
	MUTX rx_mx;
	PACKET *rxq, **rxq_endptr;
	enum assp_conn_status status;
	enum assp_conn_status read_status;
	EVNT evnt; // externally visible event

	MUTX tx_mx; // if rx_mx is also needed, lock it first!
	PACKET *txq, *txq_last;
	EVNT command_event;
	int stop;
	enum { AST_CLOSED, AST_STOPPED, AST_RUN, AST_PERTURBED } protostate;
	int rcv_fail_count;
	int max_recv_fail_count;

	pthread_t tid;
	TCB tcb;
	TCBPATH_IMPL pathimpl;
	TCBPATH path;
	TIMEQUEUE_IMPL tqi;
	TIMEQUEUE tq;
	EVNT tq_event;
	struct crypt_parms* crypt;
	char buff[256];
};

LOCALDEC struct assp_conn *new_conn(void);
LOCALDEC EVNT *assp_conn_get_event(struct assp_conn *cnx);
LOCALDEC int assp_conn_ctor(struct assp_conn *cnx, char *remaddr, char *asspmonaddr, int asspmonport, int port, char *key);
LOCALDEC void assp_conn_start(struct assp_conn *cnx);
LOCALDEC void assp_conn_send(struct assp_conn *cnx, PACKET *pkt);
LOCALDEC PACKET *assp_conn_recv(struct assp_conn *cnx);
LOCALDEC enum assp_conn_status assp_conn_status(struct assp_conn *cnx);
LOCALDEC PACKET *assp_conn_new_packet(struct assp_conn *cnx, unsigned int size);
LOCALDEC void assp_conn_free_packet(struct assp_conn *cnx, PACKET *pkt);
LOCALDEC void assp_conn_shutdown(struct assp_conn *cnx);
LOCALDEC void assp_conn_dtor(struct assp_conn *cnx);

#endif
