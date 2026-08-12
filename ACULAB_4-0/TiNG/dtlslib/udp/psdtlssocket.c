// Use TiNG/test/Testlib event sets to service ready events for DTLS status functions.
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include <pthread.h>
#ifndef _WIN32
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include "smrtp.h"
#include "smdtls.h"
#include "psdtlssocket.h"
#include "eventset.h"

// Windows can wait on up to 64 or 31 pairs plus extra for event set
#define kMaxPairsPerEVSet 31

static EVENTSET*		sPSDTLSSocketEVSetWorker;
static pthread_t*		sPSDTLSSocketEVSetWorkerThread;
static uint32_t*		sPSDTLSSocketEVSetPairUseCount;
static int				sPSDTLSSocketMaxEvSets;
static pthread_mutex_t	sPSDTLSSocketEVSetMux;

#define DTLS_MAX_PACKET_LEN 1500

typedef struct ttPacketData {
	struct ttPacketData*	fNext;
	char 					fRest[1];
} tPacketData;

typedef struct {
	pthread_cond_t			fQCondVar;
	pthread_mutex_t			fQMux;
	tPacketData*			fQHead;
	tPacketData**			fQTail;
	int						fInPeekMode;
	int						fAbortWait;
} tPacketQueue;

typedef struct ttDTLSPrimPacket {
	struct ttDTLSPrimPacket* 	fNext;
	int							fDataLen;
	char						fData[2048];
} tDTLSPrimPacket;

typedef struct ttSocketContext {
	int						fIsIPV6;
	tSMVMPrxId				fVMPRx;
	uint16_t				fOurUDPPort;
	uint16_t				fDestPort;
	tSMDTLSrxId				fDTLSRx;
	tSMDTLStxId				fDTLSTx;
	tPacketQueue			fRxQ;
	tSMEventId				fRxEventId;
	tPacketQueue			fTxQ;
	tSMEventId				fTxEventId;
	int						fLimitSet;
	struct timespec			fLimitTime;
	int						fEVSetIx;
	ACTIVE_JOB*				fTxEVSetJob;
	ACTIVE_JOB*				fRxEVSetJob;
	int						fReadyToTx;
	pthread_mutex_t			fTxReadyMux;
	pthread_cond_t			fTxCleanUpCondVar;
	pthread_mutex_t			fTxCleanUpMux;
	int						fTxFinished;
	pthread_cond_t			fRxCleanUpCondVar;
	pthread_mutex_t			fRxCleanUpMux;
	int						fRxFinished;
} tSocketContext;


static int psDTLSSocketAssignEventSet( void )
{
	int 		result = -1;
	int			i;
	uint32_t*	pCount;

	pthread_mutex_lock(&sPSDTLSSocketEVSetMux);

	pCount = sPSDTLSSocketEVSetPairUseCount;

	for (i = 0; (result == -1) && (i < sPSDTLSSocketMaxEvSets); i++)
	{
		uint32_t n = *pCount;

		if (n < kMaxPairsPerEVSet)
		{
			*pCount += 1;
			result = i;
		}

		pCount += 1;
	}

	pthread_mutex_unlock(&sPSDTLSSocketEVSetMux);

	return result;
}

static void psDTLSSocketFinishedWithEventSet( int evSetIx )
{
	uint32_t*	pCount;

	pthread_mutex_lock(&sPSDTLSSocketEVSetMux);

	pCount = sPSDTLSSocketEVSetPairUseCount+evSetIx;

	*pCount -= 1;

	pthread_mutex_unlock(&sPSDTLSSocketEVSetMux);
}

static void initPacketQueue( tPacketQueue* q )
{
	memset(q,0,sizeof(tPacketQueue));

	q->fQTail = &(q->fQHead);

	pthread_mutex_init(&q->fQMux,0);
	pthread_cond_init(&q->fQCondVar,0);
}

static void destroyPacketQueue( tPacketQueue* q )
{
	tPacketData* pkt;

	while (q->fQHead != 0)
	{
		pkt = q->fQHead;

		q->fQHead = pkt->fNext;

		free(pkt);
	}

	pthread_mutex_destroy(&q->fQMux);
	pthread_cond_destroy(&q->fQCondVar);
}

static void addPacketToQueue( tPacketQueue* q, tPacketData* pkt )
{
	pthread_mutex_lock(&q->fQMux);

	*(q->fQTail) = pkt;

	pkt->fNext = 0;

	q->fQTail = &(pkt->fNext);

	if (q->fQHead == 0)
	{
		q->fQHead = pkt;
	}

	pthread_cond_signal(&q->fQCondVar);

	pthread_mutex_unlock(&q->fQMux);
}

static void setPeekModeQueue( tPacketQueue* q, int peek )
{
	pthread_mutex_lock(&q->fQMux);

	q->fInPeekMode = peek;

	pthread_mutex_unlock(&q->fQMux);
}

static tPacketData* getPacketFromQueue( tPacketQueue* q, int doWait )
{
	tPacketData* pkt = 0;

	pthread_mutex_lock(&q->fQMux);

	if (doWait)
	{
		while ((q->fQHead == 0) && (!q->fAbortWait))
		{
			pthread_cond_wait(&(q->fQCondVar),&(q->fQMux));
		}
	}

	if ((q->fQHead != 0) && (!q->fAbortWait))
	{
		pkt = q->fQHead;

		q->fQHead = pkt->fNext;

		if (q->fQHead == 0)
		{
			q->fQTail = &(q->fQHead);
		}
	}

	pthread_mutex_unlock(&q->fQMux);

	return pkt;
}


static int psDTLSSocketEVSetTxHandler( struct active_job* job )
{
	int								rc;
	SM_DTLSTX_STATUS_PARMS			statusParms;
	tSocketContext* 				sc = (tSocketContext*) (job->mf);
	tDTLSPrimPacket*				pkt;
	SM_DTLSTX_SEND_PACKET_PARMS		dtlsSendParms;
	SM_DTLSTX_CONFIG_NOTIFY_PARMS	dtlsNotifyParms;

	memset(&statusParms,0,sizeof(statusParms));

	statusParms.dtlstx = sc->fDTLSTx;

	rc = sm_dtlstx_status(&statusParms);

 	if (rc != 0)
	{
		// error action - will remove from event set
		return -1;
	}

	if (statusParms.status == kSMDTLStxStatusHasCapacity)
	{
		// do non-blocking wait
		pkt = (tDTLSPrimPacket*) getPacketFromQueue(&sc->fTxQ,0);

		if (pkt != 0)
		{
			// we have something to send
			memset(&dtlsSendParms,0,sizeof(dtlsSendParms));

			dtlsSendParms.dtlstx        = sc->fDTLSTx;
			dtlsSendParms.data          = &(pkt->fData[0]);
			dtlsSendParms.packet_length = pkt->fDataLen;

			rc = sm_dtlstx_send_packet(&dtlsSendParms);

			free(pkt);

			if (rc != 0)
			{
				// error action
				return -1;
			}

			pthread_mutex_lock(&sc->fTxReadyMux);

			sc->fReadyToTx = 0;

			pthread_mutex_unlock(&sc->fTxReadyMux);
		}
		else
		{
			pthread_mutex_lock(&sc->fTxReadyMux);

			memset(&dtlsNotifyParms,0,sizeof(dtlsNotifyParms));

			dtlsNotifyParms.dtlstx            = sc->fDTLSTx;
			dtlsNotifyParms.packet_length     = DTLS_MAX_PACKET_LEN;
			dtlsNotifyParms.no_capacity_event = 1;

			rc = sm_dtlstx_config_notify(&dtlsNotifyParms); 

			if (rc == 0)
			{
				sc->fReadyToTx = 1;
			}

			pthread_mutex_unlock(&sc->fTxReadyMux);

			if (rc != 0)
			{
				return -1;
			}
		}
	}

	return 0;
}

static void psDTLSSocketEVSetTxCleanUp( struct active_job* job )
{
	tSocketContext* sc = (tSocketContext*) (job->mf);

	pthread_mutex_lock(&sc->fTxCleanUpMux);

	sc->fTxFinished = 1;

	pthread_cond_signal(&sc->fTxCleanUpCondVar);

	pthread_mutex_unlock(&sc->fTxCleanUpMux);
}

static int psDTLSSocketEVSetRxHandler( struct active_job* job )
{
	int							rc;
	SM_DTLSRX_GET_PACKET_PARMS	getPacketParms;
	SM_DTLSRX_STATUS_PARMS		statusParms;
	tSocketContext* 			sc = (tSocketContext*) (job->mf);
	tDTLSPrimPacket*			pkt;
	int							stopped;

	memset(&statusParms,0,sizeof(statusParms));

	statusParms.dtlsrx = sc->fDTLSRx;

	rc = sm_dtlsrx_status(&statusParms);

 	if (rc != 0)
	{
		// error return, will remove job
		return -1;
	}

	if (	(statusParms.status == kSMDTLSrxStatusHasPacket)
		|| 	(statusParms.status == kSMDTLSrxStatusOverrun)   )
	{
		pkt = malloc(sizeof(tDTLSPrimPacket));

		memset(&getPacketParms,0,sizeof(getPacketParms));

		getPacketParms.dtlsrx 		= sc->fDTLSRx;
		getPacketParms.data   		= &(pkt->fData[0]);
		getPacketParms.max_length 	= sizeof(pkt->fData);

		rc = sm_dtlsrx_get_packet(&getPacketParms);

		if (rc != 0)
		{
			//error return
			free(pkt);
			return -1;
		}

		pkt->fDataLen = getPacketParms. packet_length;

		addPacketToQueue(&sc->fRxQ,(tPacketData*)pkt);
	}

	return 0;
}

static void psDTLSSocketEVSetRxCleanUp( struct active_job* job )
{
	tSocketContext* sc = (tSocketContext*) (job->mf);

	pthread_mutex_lock(&sc->fRxCleanUpMux);

	sc->fRxFinished = 1;

	pthread_cond_signal(&sc->fRxCleanUpCondVar);

	pthread_mutex_unlock(&sc->fRxCleanUpMux);
}


static void setFutureTime( int msFromNow, struct timespec* futureTime )
{
	uint64_t t;
#ifdef TiNGTYPE_WINNT
	struct _timeb			now;
#else
	struct timeval			now;
#endif

#ifdef TiNGTYPE_WINNT
	_ftime(&now);
	t = ((uint64_t)now.millitm) + ((uint64_t)now.time*1000L) + (uint64_t)msFromNow;

	futureTime->tv_sec  = (unsigned long) (t/1000);
	futureTime->tv_nsec = (long) (t-futureTime->tv_sec*1000)*1000*1000;
#else
	gettimeofday(&now, 0);
	t = ((uint64_t)now.tv_usec) + ((uint64_t)now.tv_sec*1000*1000L) + ((uint64_t)msFromNow*1000);

	futureTime->tv_sec  = (unsigned long) (t/(1000*1000));
	futureTime->tv_nsec = (t-futureTime->tv_sec*1000*1000)*1000;
#endif
}

static int limitFutureTime( struct timespec* limitTime, struct timespec* futureTime )
{
	int isLimit = 0;

	if (futureTime->tv_sec > limitTime->tv_sec)
	{
		isLimit = 1;

		*futureTime = *limitTime;
	}
	else if ((futureTime->tv_sec == limitTime->tv_sec) && (futureTime->tv_nsec > limitTime->tv_nsec))
	{
		isLimit = 1;

		*futureTime = *limitTime;
	}

	return isLimit;
}

// Return 1 if packet in queue on exit (and not aborted)
// Return 0 if no packet within timeout
// Return -1 if total max (limit time) reached
// Return -2 if aborted
static int awaitNonEmptyQueue( tPacketQueue* q, int timeoutInMilliSecs, struct timespec* limitTime )
{
	int						result;
	int 					isPacket;
	struct timespec			ts;
	int						rc;
	int						timedOut;
	int						isLimit;

	if (timeoutInMilliSecs <= 0)
	{
		ts = *limitTime;
		isLimit = 1;
	}
	else
	{
		setFutureTime(timeoutInMilliSecs,&ts);
		isLimit = limitFutureTime(limitTime,&ts);
	}

	pthread_mutex_lock(&q->fQMux);

	rc = 0;
	timedOut = 0;
	
	while ((q->fQHead == 0) && (!q->fAbortWait) && (rc == 0) && !timedOut)
	{
		rc = pthread_cond_timedwait(&(q->fQCondVar),&(q->fQMux),&ts);
		
		if (rc == ETIMEDOUT)
		{
			rc = 0;
			timedOut = 1;
		}
	}

	isPacket = ((q->fQHead != 0) && (!q->fAbortWait)) ? 1 : 0;

	result = (q->fAbortWait) ? -2 : 0;

	pthread_mutex_unlock(&q->fQMux);

	if (result == 0)
	{
		if (isPacket)
		{
			result = 1;
		}
		else if (isLimit)
		{
			result = -1;
		}
	}
	
	return result;
}


static void abortQueueWait( tPacketQueue* q )
{
	pthread_mutex_lock(&q->fQMux);

	q->fAbortWait = 1;

	pthread_cond_signal(&q->fQCondVar);

	pthread_mutex_unlock(&q->fQMux);
}


static int psDTLSSocketEVSetSetUp( tSocketContext* sc )
{
	int 				result;
	int 				evSetIx = psDTLSSocketAssignEventSet();
	err_t				err;
	struct active_job* 	job;

	result = 0;

	if (evSetIx < 0)
	{
		result = -1;
	}
	else
	{
		sc->fEVSetIx = evSetIx;

		job = malloc(sizeof(ACTIVE_JOB));

		memset(job,0,sizeof(ACTIVE_JOB));

		job->event		= sc->fTxEventId;
		job->handler	= psDTLSSocketEVSetTxHandler;
		job->cleanup_fn = psDTLSSocketEVSetTxCleanUp;
		job->mf			= (struct appactive *) sc;

		pthread_mutex_init(&sc->fTxCleanUpMux,0);
		pthread_cond_init(&sc->fTxCleanUpCondVar,0);

		pthread_mutex_init(&sc->fRxCleanUpMux,0);
		pthread_cond_init(&sc->fRxCleanUpCondVar,0);

		sc->fTxEVSetJob = job;

		if ((err = eventset_insert(&sPSDTLSSocketEVSetWorker[evSetIx],job)) != 0)
		{
			free(job);
			sc->fTxEVSetJob = 0;
		
			result = -1;
		}
		else
		{
			job = malloc(sizeof(ACTIVE_JOB));

			memset(job,0,sizeof(ACTIVE_JOB));

			job->event		= sc->fRxEventId;
			job->handler	= psDTLSSocketEVSetRxHandler;
			job->cleanup_fn = psDTLSSocketEVSetRxCleanUp;
			job->mf			= (struct appactive *) sc;

			sc->fRxEVSetJob = job;

			if ((err = eventset_insert(&sPSDTLSSocketEVSetWorker[evSetIx],job)) != 0)
			{
				free(job);
				sc->fRxEVSetJob = 0;

				eventset_delete(&sPSDTLSSocketEVSetWorker[sc->fEVSetIx],sc->fTxEVSetJob);

				// await tx clean up signal
				pthread_mutex_lock(&sc->fTxCleanUpMux);

				while (sc->fTxFinished == 0)
				{
					pthread_cond_wait(&(sc->fTxCleanUpCondVar),&(sc->fTxCleanUpMux));
				}

				pthread_mutex_unlock(&sc->fTxCleanUpMux);
				
				free(sc->fTxEVSetJob);

				sc->fTxEVSetJob= 0;
			
				result = -1;
			}
		}
	
		if (result != 0)
		{
			pthread_mutex_destroy(&sc->fTxCleanUpMux);
			pthread_cond_destroy(&sc->fTxCleanUpCondVar);
			pthread_mutex_destroy(&sc->fRxCleanUpMux);
			pthread_cond_destroy(&sc->fRxCleanUpCondVar);

			psDTLSSocketFinishedWithEventSet(evSetIx);
		}
	}

	return result;
}


int psDTLSSocketCreate( tSMModuleId moduleId, tSMVMPrxId vmprx, tPSDTLSSocketAccess* pSA )
{
	int								result;
	int								rc;
	SM_DTLSRX_CREATE_PARMS			dtlsRxCreateParms;
	SM_DTLSTX_CREATE_PARMS			dtlsTxCreateParms;
	SM_VMPRX_CONFIG_DTLS_PARMS		dtlsRxConfig;
	SM_DTLSRX_EVENT_PARMS			rxEvParms;
	SM_DTLSTX_EVENT_PARMS			txEvParms;
	SM_DTLSTX_CONFIG_NOTIFY_PARMS 	dtlsTxNotifyParms;
	int								dtlsPairCreated;

	tSocketContext* sc = calloc(1,sizeof(tSocketContext));

	*pSA = NULL;

	if (sc == NULL)
	{
		return -1;
	}

	result = 0;

	sc->fVMPRx = vmprx;

	dtlsPairCreated = 0;
	
	memset(&dtlsRxCreateParms,0,sizeof(SM_DTLSRX_CREATE_PARMS));

	dtlsRxCreateParms.module = moduleId;

	rc = sm_dtlsrx_create(&dtlsRxCreateParms);

	if (rc != 0)
	{
		result = -1;
	}
	else
	{
		sc->fDTLSRx = dtlsRxCreateParms.dtlsrx;

		memset(&dtlsTxCreateParms,0,sizeof(SM_DTLSTX_CREATE_PARMS));

		dtlsTxCreateParms.module = moduleId;

		rc = sm_dtlstx_create(&dtlsTxCreateParms);

		if (rc != 0)
		{
			sm_dtlsrx_destroy(sc->fDTLSRx);
	
			result = -1;
		}
		else
		{
			sc->fDTLSTx = dtlsTxCreateParms.dtlstx;
			dtlsPairCreated = 1;
		}
	}

	if (dtlsPairCreated)
	{
		memset(&dtlsTxNotifyParms,0,sizeof(dtlsTxNotifyParms));

		dtlsTxNotifyParms.dtlstx 			= sc->fDTLSTx;
		dtlsTxNotifyParms.packet_length 	= DTLS_MAX_PACKET_LEN;
		dtlsTxNotifyParms.no_capacity_event = 0;

		rc = sm_dtlstx_config_notify(&dtlsTxNotifyParms);

		if (rc != 0)
		{
			result = -1;
		}

		memset(&dtlsRxConfig,0,sizeof(dtlsRxConfig));

		dtlsRxConfig.vmprx 			= vmprx;
		dtlsRxConfig.dtlsrx 		= sc->fDTLSRx;
		dtlsRxConfig.use_RTCP_port 	= 0;

		rc = sm_vmprx_config_dtls(&dtlsRxConfig);

		if (rc != 0)
		{
			result = -1;
		}

		memset(&rxEvParms,0,sizeof(rxEvParms));

		rxEvParms.dtlsrx = sc->fDTLSRx;

		rc = sm_dtlsrx_get_event(&rxEvParms);

		if (rc != 0)
		{
			result = -1;
		}

		memset(&txEvParms,0,sizeof(txEvParms));

		txEvParms.dtlstx = sc->fDTLSTx;

		rc = sm_dtlstx_get_event(&txEvParms);

		if (rc != 0)
		{
			result = -1;
		}

		sc->fTxEventId = txEvParms.event;
		sc->fRxEventId = rxEvParms.event;
	}

	if (result == 0)
	{
		initPacketQueue(&sc->fTxQ);
		initPacketQueue(&sc->fRxQ);
	
		result = psDTLSSocketEVSetSetUp(sc);

		if (result != 0)
		{
			destroyPacketQueue(&sc->fTxQ);
			destroyPacketQueue(&sc->fRxQ);
		}
		else
		{
			*pSA = sc;
		}
	}

	if (result != 0)
	{
		if (dtlsPairCreated)
		{
			sm_dtlsrx_destroy(sc->fDTLSRx);
			sm_dtlstx_destroy(sc->fDTLSTx);
		}
		
		free(sc);
	}

	return result;
}


int psDTLSSocketConfigIPv6( tPSDTLSSocketAccess sa, SOCKADDR_IN6* src, SOCKADDR_IN6* dst )
{
	int							rc;
	int							result = 0;
	tSocketContext* 			sc = (tSocketContext*) sa;
	SM_DTLSTX_CONFIG_IPV6_PARMS	configIPV6;

	memset(&configIPV6,0,sizeof(configIPV6));

	configIPV6.dtlstx = sc->fDTLSTx;

	memcpy(&configIPV6.destination,dst,sizeof(SOCKADDR_IN6));

	if (src)
	{
		memcpy(&configIPV6.source,src,sizeof(SOCKADDR_IN6));
	}

	rc = sm_dtlstx_config_ipv6(&configIPV6); 

	if (rc != 0)
	{
		result = -1;
	}

	return result;
}

int psDTLSSocketConfigIPv4( tPSDTLSSocketAccess sa, SOCKADDR_IN* src, SOCKADDR_IN* dst )
{
	int							rc;
	int							result = 0;
	tSocketContext* 			sc = (tSocketContext*) sa;
	SM_DTLSTX_CONFIG_PARMS		configIPV4;

	memset(&configIPV4,0,sizeof(configIPV4));

	configIPV4.dtlstx = sc->fDTLSTx;

	memcpy(&configIPV4.destination,dst,sizeof(SOCKADDR_IN));

	if (src)
	{
		memcpy(&configIPV4.source,src,sizeof(SOCKADDR_IN));
	}

	rc = sm_dtlstx_config(&configIPV4); 

	if (rc != 0)
	{
		result = -1;
	}

	return result;
}

void psDTLSSocketSetLimitTime( tPSDTLSSocketAccess sa, int secsFromNow )
{
	tSocketContext* sc = (tSocketContext*) sa;

	setFutureTime(1000*secsFromNow,&(sc->fLimitTime));

	sc->fLimitSet = 1;
}


int psDTLSSocketDestroy( tPSDTLSSocketAccess sa )
{
	tSocketContext* sc = (tSocketContext*) sa;

	eventset_delete(&sPSDTLSSocketEVSetWorker[sc->fEVSetIx],sc->fRxEVSetJob);
	eventset_delete(&sPSDTLSSocketEVSetWorker[sc->fEVSetIx],sc->fTxEVSetJob);

	// await tx clean up signal
	pthread_mutex_lock(&sc->fTxCleanUpMux);

	while (sc->fTxFinished == 0)
	{
		pthread_cond_wait(&(sc->fTxCleanUpCondVar),&(sc->fTxCleanUpMux));
	}

	pthread_mutex_unlock(&sc->fTxCleanUpMux);

	// await rx clean up signal

	pthread_mutex_lock(&sc->fRxCleanUpMux);

	while (sc->fRxFinished == 0)
	{
		pthread_cond_wait(&(sc->fRxCleanUpCondVar),&(sc->fRxCleanUpMux));
	}

	pthread_mutex_unlock(&sc->fRxCleanUpMux);

	free(sc->fRxEVSetJob);
	free(sc->fTxEVSetJob);
	
	pthread_mutex_destroy(&sc->fTxCleanUpMux);
	pthread_cond_destroy(&sc->fTxCleanUpCondVar);
	pthread_mutex_destroy(&sc->fRxCleanUpMux);
	pthread_cond_destroy(&sc->fRxCleanUpCondVar);

	destroyPacketQueue(&sc->fTxQ);
	destroyPacketQueue(&sc->fRxQ);

	sm_dtlsrx_destroy(sc->fDTLSRx);
	sm_dtlstx_destroy(sc->fDTLSTx);

	psDTLSSocketFinishedWithEventSet(sc->fEVSetIx);

	free(sc);

	return 0;
}

// return 0 if no packet arrived, 1 if packet is waiting, -1 if limit hit, -2 if no limit set (error) or abort
int psDTLSSocketPoll( tPSDTLSSocketAccess sa, int timeoutInMilliSecs )
{
	int 			result;
	tSocketContext* sc = (tSocketContext*) sa;

	if (sc->fLimitSet)
	{
		result = awaitNonEmptyQueue(&sc->fRxQ,timeoutInMilliSecs,&(sc->fLimitTime));
	}
	else
	{
		result = -2;
	}

	return result;
}

// interrupt above wait
void psDTLSSocketPollAbort( tPSDTLSSocketAccess sa )
{
	tSocketContext* sc = (tSocketContext*) sa;

	abortQueueWait(&sc->fRxQ);
}

// All these functions return either the amount of data successfully read or written 
// (if the return value is positive) or that no data was successfully read or written 
// if the result is 0 or -1. If the return value is -2 then the operation is not 
// implemented in the specific BIO type.
int psDTLSSocketSend(void* ctx, const char* data, unsigned len)
{
	tSocketContext* sc = (tSocketContext*) ctx;

	tDTLSPrimPacket* pkt = (tDTLSPrimPacket*) malloc(sizeof(tDTLSPrimPacket));

	pkt->fDataLen = len;

	memcpy(&(pkt->fData[0]),data,len);

	addPacketToQueue(&sc->fTxQ,(tPacketData*)pkt);

	pthread_mutex_lock(&sc->fTxReadyMux);

	if (sc->fReadyToTx)
	{
		int								rc;
		SM_DTLSTX_CONFIG_NOTIFY_PARMS	dtlsNotifyParms;

		// enabling the event, will stimulate event set action to send packet
		memset(&dtlsNotifyParms,0,sizeof(dtlsNotifyParms));

		dtlsNotifyParms.dtlstx            = sc->fDTLSTx;
		dtlsNotifyParms.packet_length     = DTLS_MAX_PACKET_LEN;
		dtlsNotifyParms.no_capacity_event = 0;

		rc = sm_dtlstx_config_notify(&dtlsNotifyParms); 

		if (rc != 0)
		{
			len = -1;
		}
	}

	sc->fReadyToTx = 0;

	pthread_mutex_unlock(&sc->fTxReadyMux);

	return len;
}

// Hack if data zero, use to control peek for OpenSSL 1.1
int psDTLSSocketRcv(void* ctx, char* data, unsigned max_len)
{
	tSocketContext* 		sc = (tSocketContext*) ctx;
	int						result;
	int						n;
	tDTLSPrimPacket*		p;

	result = 0;

	if (data == 0)
	{
		setPeekModeQueue(&sc->fRxQ,max_len);
	}
	else
	{
		p = (tDTLSPrimPacket*) getPacketFromQueue(&sc->fRxQ,0);

		if (p != 0)
		{
			n = p->fDataLen;

			if (n <= (int) max_len)
			{
				memcpy(data,&(p->fData[0]),n);

				result = n;
			}

			free(p);
		}
	}

	return result;
}


static void* evWorkerThread( void* p )
{
	EVENTSET* e = (EVENTSET*) p;

	while (!e->stop)
	{
		err_t err = eventset_wait(e);

		if (err != 0)
		{
			break;
		}
	}

	return 0;
}

static void commonCleanUpLib( int n )
{
	int i;
	
	for (i = 0; i < n; i++)
	{
		eventset_stop(sPSDTLSSocketEVSetWorker+i);
		pthread_join(*(sPSDTLSSocketEVSetWorkerThread+i),NULL);
		eventset_dtor(sPSDTLSSocketEVSetWorker+i);
	}

	free(sPSDTLSSocketEVSetWorker);
	free(sPSDTLSSocketEVSetWorkerThread);
	free(sPSDTLSSocketEVSetPairUseCount);

	pthread_mutex_destroy(&sPSDTLSSocketEVSetMux);
}

int psDTLSSocketInitLib( int maxSockets )
{
	int	result;
	int i;
	int maxPerEVSet            = 2*kMaxPairsPerEVSet;
	int	started = 0;
	
	result = 0;

	sPSDTLSSocketMaxEvSets = (maxSockets+(maxPerEVSet-1))/maxPerEVSet;
	
	sPSDTLSSocketEVSetWorker       = calloc(sPSDTLSSocketMaxEvSets,sizeof(EVENTSET));
	sPSDTLSSocketEVSetWorkerThread = calloc(sPSDTLSSocketMaxEvSets,sizeof(pthread_t));
	sPSDTLSSocketEVSetPairUseCount = calloc(sPSDTLSSocketMaxEvSets,sizeof(uint32_t));

	pthread_mutex_init(&sPSDTLSSocketEVSetMux,0);

	for (i = 0; (result == 0) && (i < sPSDTLSSocketMaxEvSets); i++)
	{
		if (eventset(sPSDTLSSocketEVSetWorker+i) == 0)
		{
			if (pthread_create(sPSDTLSSocketEVSetWorkerThread+i,NULL,evWorkerThread,sPSDTLSSocketEVSetWorker+i) != 0)
			{
				eventset_dtor(sPSDTLSSocketEVSetWorker+i);

				result = -1;
			}
			else
			{
				started += 1;
			}
		}
		else
		{
			result = -1;
		}
	}

	if (result != 0)
	{
		commonCleanUpLib(started);
	}
	
	
	return result;
}

void psDTLSSocketDtorLib( void )
{
	commonCleanUpLib(sPSDTLSSocketMaxEvSets);
}

