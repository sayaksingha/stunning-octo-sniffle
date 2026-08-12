/*------------------------------------------------------------*/
/* Copyright ACULAB plc. (c) 1996-1997                        */
/*------------------------------------------------------------*/
/*                                                            */
/*                                                            */
/* Program File Name : smhlib.c                               */
/*                                                            */
/*           Purpose : High level library for SM driver       */
/*                                                            */
/*            Author : Peter Dain                             */
/*                                                            */
/*       Create Date : 6th July 1996                          */
/*                                                            */
/*             Tools : CC compiler                            */
/*                                                            */
/*                                                            */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/

/*
 * By default uses standard C RTL fread/fwrite calls.
 * Make sure you use multithreaded C RTL if required. 
 * or compile with define __SMWIN32HLIB__ in order to use WIN32 calls.
 */

#ifdef __STDC__
#define __NEWC__
#endif


#include "smdrvr.h"
#include "smhlib.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "../apilib/trace.h"
#include "../apilib/once.h"

#include <pthread.h>

#ifndef ACUAPI
#define ACUAPI
#endif

#ifndef __SMWIN32HLIB__
#ifdef TiNGTYPE_WINNT
#define ssize_t size_t

efopen_fn fopen_aculab_TiNG_highapi_ptr = fopen;
efread_fn fread_aculab_TiNG_highapi_ptr = fread;
efwrite_fn fwrite_aculab_TiNG_highapi_ptr = fwrite;
efseek_fn fseek_aculab_TiNG_highapi_ptr = fseek;
eftell_fn ftell_aculab_TiNG_highapi_ptr = ftell;
eferror_fn ferror_aculab_TiNG_highapi_ptr = ferror;
efclose_fn fclose_aculab_TiNG_highapi_ptr = fclose;
eerrno_ptr_fn errno_aculab_TiNG_highapi_ptr = _errno;

ACUAPI void ACUTiNG_WINAPI highapi_override_fileio(efopen_fn open, efread_fn read, efwrite_fn write, efseek_fn seek, eftell_fn tell, eferror_fn error, efclose_fn close, eerrno_ptr_fn enp)
{
	fopen_aculab_TiNG_highapi_ptr = open;
	fread_aculab_TiNG_highapi_ptr = read;
	fwrite_aculab_TiNG_highapi_ptr = write;
	fseek_aculab_TiNG_highapi_ptr = seek;
	ftell_aculab_TiNG_highapi_ptr = tell;
	ferror_aculab_TiNG_highapi_ptr = error;
	fclose_aculab_TiNG_highapi_ptr = close;
	errno_aculab_TiNG_highapi_ptr = enp;
}


#define CALL_FUNC(x) (*(x ## _aculab_TiNG_highapi_ptr))
#define GET_ERRNO (*(*errno_aculab_TiNG_highapi_ptr)())
#endif
#endif

#include "efile.i"

static pthread_mutex_t replay_mx;
static pthread_cond_t replay_complete;
static pthread_once_t replay_once_init = PTHREAD_ONCE_INIT;

static pthread_mutex_t record_mx;
static pthread_cond_t record_complete;
static pthread_once_t record_once_init = PTHREAD_ONCE_INIT;

static void replay_init(void)
{
 int r = pthread_mutex_init(&replay_mx, 0);
 if (r) {
 	fprintf(stderr, "ptrhead_mutex_init failed in replay_init: %s\n",
 		strerror(r));
	abort();
 }
 r = pthread_cond_init(&replay_complete, 0);
 if (r) {
 	fprintf(stderr, "ptrhead_cond_init failed in replay_init: %s\n",
 		strerror(r));
	abort();
 }
}

static void record_init(void)
{
 int r = pthread_mutex_init(&record_mx, 0);
 if (r) {
 	fprintf(stderr, "ptrhead_mutex_init failed in record_init: %s\n",
 		strerror(r));
	abort();
 }
 r = pthread_cond_init(&record_complete, 0);
 if (r) {
 	fprintf(stderr, "ptrhead_cond_init failed in record_init: %s\n",
 		strerror(r));
	abort();
 }
}

STATIC int real_sm_replay_file_start(struct sm_file_replay_parms *file_parms)
{
 int rc = pthread_once(&replay_once_init, replay_init);
 if (rc) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 file_parms->completing = 0;
 if (file_parms->replay_parms.data_length == 0) {
	file_parms->private_length = ~0u;
 } else file_parms->private_length = file_parms->replay_parms.data_length;
 file_parms->data_length_in_buffer = 0;
 if (efseek(file_parms->fd, file_parms->offset, SEEK_SET)) {
	return file_parms->status = err(ERR_SM_FILE_ACCESS);
 } else if (! (rc = sm_replay_start(&file_parms->replay_parms)) ) {
	file_parms->status = ERR_SM_PENDING;
	file_parms->completing = 1;
	return 0;
 } else {
	return file_parms->status = rc;
 }
}

/* Handle a status fetched from the channel. Returns ERR_SM_PENDING if
 * data was successfully written. In that case the status should be
 * re-checked and this function called again. Any other return value
 * indicates that either the replay has finished (if file_parms->status
 * is no longer ERR_SM_PENDING), an error has occurred, or the channel
 * will generate another event when the next status change occurs.
 */
static int unsafe_sm_replay_file_progress_one(struct sm_file_replay_parms *file_parms, int status)
{
 int rc = 0;
 switch (status) {
	SM_TS_DATA_PARMS dataParms;
	int readOctets;
 case kSMReplayStatusComplete:
	return file_parms->status = 0;
 case kSMReplayStatusCompleteData:
 case kSMReplayStatusNoCapacity:
 case kSMReplayStatusUnderrun:
		/* status remains as pending. */
	return 0;
 case kSMReplayStatusHasCapacity:
	memset(&dataParms, 0, sizeof(dataParms));
	dataParms.channel = file_parms->replay_parms.channel;
	dataParms.data = &file_parms->buffer[0];
	dataParms.length = file_parms->private_length;
	if (file_parms->private_length == ~0u
		|| file_parms->private_length > kSMMaxReplayDataBufferSize)
		dataParms.length = kSMMaxReplayDataBufferSize;
	else dataParms.length = file_parms->private_length ;
	readOctets = efread(&file_parms->buffer[0], 1, dataParms.length, file_parms->fd);
	if (readOctets < 0) return file_parms->status = err(ERR_SM_FILE_ACCESS);
	dataParms.length = readOctets;
	if (!readOctets) {
		return sm_put_last_replay_data(&dataParms);
	}
	rc = sm_put_replay_data(&dataParms);
	if (rc) return rc;
	if (file_parms->private_length != ~0u)
		file_parms->private_length -= readOctets;
	return ERR_SM_PENDING;		/* and still pending */
 default:
	return file_parms->status = err(ERR_SM_DEVERR);
 }
}

/*
 * Progress a replay previously initiated by a call to sm_replay_file until
 * replay complete, replay blocked or an error occurs.
 *
 * Returns:
 *	0              if replay completed.
 *	ERR_SM_PENDING if replay blocked pending more buffering capacity on DSP.
 *	other	       if error during replay
 */
STATIC int real_sm_replay_file_progress(struct sm_file_replay_parms *file_parms)
{
 int rc = 0;
 if (pthread_mutex_lock(&replay_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	SM_REPLAY_STATUS_PARMS statusParms;
	memset(&statusParms, 0, sizeof(statusParms));
	statusParms.channel = file_parms->replay_parms.channel;
	rc = sm_replay_status(&statusParms);
	if (!rc) {
		rc = unsafe_sm_replay_file_progress_one(file_parms, statusParms.status);
	}
 }
 pthread_mutex_unlock(&replay_mx);
 if (rc) return rc;
 return file_parms->status;
}

/*
 * Progress a replay previously initiated by a call to sm_replay_file until
 * replay complete, replay blocked or an error occurs.
 *
 * Returns:
 *	0              if replay completed.
 *	ERR_SM_PENDING if replay blocked pending more buffering capacity on DSP.
 *	other	       if error during replay
 */
STATIC int real_sm_replay_file_progress_istatus(struct sm_file_replay_parms *file_parms, int status)
{
 int rc = 0;
 if (pthread_mutex_lock(&replay_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	rc = unsafe_sm_replay_file_progress_one(file_parms, status);
 }
 pthread_mutex_unlock(&replay_mx);
 if (rc) return rc;
 return file_parms->status;
}

STATIC int real_sm_replay_file_complete(struct sm_file_replay_parms *file_parms)
{
 SM_CHANNEL_SET_EVENT_PARMS eventParms;
 int rc;
 eventParms.channel = file_parms->replay_parms.channel;
 eventParms.issue_events = kSMChannelSpecificEvent;
 eventParms.event_type = kSMEventTypeWriteData;
 rc = smd_ev_create(&eventParms.event, eventParms.channel,
		kSMEventTypeWriteData, kSMChannelSpecificEvent);
 if (!rc) {
	int r;
	rc = sm_channel_set_event(&eventParms);
	if (!rc) {
		for (;;) {
			rc = sm_replay_file_progress(file_parms);
			if (rc != ERR_SM_PENDING) break;
			rc = smd_ev_wait(eventParms.event);
			if (rc) break;
		}
		eventParms.issue_events = kSMChannelNoEvent;
		r = sm_channel_set_event(&eventParms);
		if (!rc) rc = r;
	}
	r = smd_ev_free(eventParms.event);
	if (!rc) rc = r;
 }
 if (pthread_mutex_lock(&replay_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 file_parms->status = rc;
 file_parms->completing = 0;
 pthread_mutex_unlock(&replay_mx);
 pthread_cond_broadcast(&replay_complete);
 return rc;
}

STATIC int real_sm_replay_file_stop(struct sm_file_replay_parms *file_parms, int nowait)
{
 int rc = 0;
 if (pthread_mutex_lock(&replay_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	SM_REPLAY_ABORT_PARMS abortParms;
	memset(&abortParms, 0, sizeof(abortParms));
	abortParms.channel = file_parms->replay_parms.channel;
	rc = sm_replay_abort(&abortParms);
	if (!rc) {
		if (!nowait) {
			while (file_parms->status == ERR_SM_PENDING
				|| file_parms->completing) {
				pthread_cond_wait(&replay_complete, &replay_mx);
			}
			rc = file_parms->status;
		}
		file_parms->offset += abortParms.offset;
		if (file_parms->replay_parms.data_length) {
			file_parms->replay_parms.data_length -= abortParms.offset;
			if (!file_parms->replay_parms.data_length) {
				file_parms->replay_parms.data_length = 1;
			}
		}
	}
 }
 pthread_mutex_unlock(&replay_mx);
 return rc;
}

STATIC int real_sm_record_file_start(struct sm_file_record_parms *file_parms)
{
 int rc = pthread_once(&record_once_init, record_init);
 if (rc) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 rc = sm_record_start(&file_parms->record_parms);
 if (!rc) {
	file_parms->status = ERR_SM_PENDING;
	file_parms->completing = 1;
 } else {
	file_parms->status = rc;
	file_parms->completing = 0;
 }
 return rc;
}

static int unsafe_sm_record_file_progress_one(struct sm_file_record_parms *file_parms, int status)
{
 int rc = 0;
 switch (status) {
	SM_TS_DATA_PARMS dataParms;
 case kSMRecordStatusComplete:
	return file_parms->status = 0;
 case kSMRecordStatusOverrun:
 case kSMRecordStatusNoData:
 case kSMRecordStatusRecognition:
	return 0; 
 case kSMRecordStatusCompleteData:
 case kSMRecordStatusData:
	memset(&dataParms, 0, sizeof(dataParms));
	dataParms.channel = file_parms->record_parms.channel;
	dataParms.data = &file_parms->buffer[0];
	rc = sm_get_recorded_data(&dataParms);
	if (rc) return file_parms->status = rc;
	if (dataParms.length != (tSM_INT)(efwrite(&file_parms->buffer[0], sizeof(tSM_UT8), dataParms.length, file_parms->fd))) {
		return file_parms->status = err(ERR_SM_FILE_ACCESS);
	}
	return ERR_SM_PENDING;
 default:
	return file_parms->status = err(ERR_SM_DEVERR);
 }
}

STATIC int real_sm_record_file_progress_ostatus(struct sm_file_record_parms *file_parms, struct sm_record_status_parms *status_parms)
{
 int rc = 0;
 if (pthread_mutex_lock(&record_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	status_parms->channel = file_parms->record_parms.channel;
	rc = sm_record_status(status_parms);
	if (rc) {
		pthread_mutex_unlock(&record_mx);
		return rc;
	}
	file_parms->record_parms.sampling_rate = status_parms->sample_rate; // update sampling rate to the rate we're getting
	rc = unsafe_sm_record_file_progress_one(file_parms, status_parms->status);
 }
 pthread_mutex_unlock(&record_mx);
 if(rc) return rc;
 return file_parms->status;
}

STATIC int real_sm_record_file_progress(struct sm_file_record_parms *file_parms)
{
	SM_RECORD_STATUS_PARMS statusParms;
	memset(&statusParms, 0, sizeof(statusParms));
	return  sm_record_file_progress_ostatus(file_parms, &statusParms); 
}

STATIC int real_sm_record_file_progress_istatus(struct sm_file_record_parms *file_parms, int status)
{
 int rc = 0;
 if (pthread_mutex_lock(&record_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	rc = unsafe_sm_record_file_progress_one(file_parms, status);
 }
 pthread_mutex_unlock(&record_mx);
 if(rc) return rc;
 return file_parms->status;
}

STATIC int real_sm_record_file_complete_tstatus(struct sm_file_record_parms *file_parms, struct sm_record_status_parms *status_parms)
{
 SM_CHANNEL_SET_EVENT_PARMS eventParms;
 int rc;
 memset(&eventParms, 0, sizeof(eventParms));
 eventParms.channel = file_parms->record_parms.channel;
 eventParms.issue_events = kSMChannelSpecificEvent;
 eventParms.event_type = kSMEventTypeReadData;
 rc = smd_ev_create(&eventParms.event, eventParms.channel,
 		kSMEventTypeReadData, kSMChannelSpecificEvent);
 if (!rc) {
	int r;
	rc = sm_channel_set_event(&eventParms);
	if (!rc) {
		 for (;;) {
		 	memset(status_parms,0,sizeof(*status_parms));
			rc = sm_record_file_progress_ostatus(file_parms,status_parms);
			if (rc != ERR_SM_PENDING) break;
			rc = smd_ev_wait(eventParms.event);
			if (rc) break;
		}
		eventParms.issue_events = kSMChannelNoEvent;
		r = sm_channel_set_event(&eventParms);
		if (!rc) rc = r;
	 }
	r = smd_ev_free(eventParms.event);
	if (!rc) rc = r;
 }
 if (pthread_mutex_lock(&record_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 file_parms->status = rc;
 file_parms->completing = 0;
 pthread_cond_broadcast(&record_complete);
 pthread_mutex_unlock(&record_mx);
 return rc;
}

STATIC int real_sm_record_file_complete(struct sm_file_record_parms *file_parms)
{
	SM_RECORD_STATUS_PARMS status_parms;
	memset(&status_parms, 0, sizeof(status_parms));
	return sm_record_file_complete_tstatus(file_parms, &status_parms);
}

STATIC int real_sm_record_file_stop( struct sm_file_record_parms* file_parms ,int nowait)
{
 int rc = 0;
 if (pthread_mutex_lock(&record_mx)) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 if (file_parms->status == ERR_SM_PENDING) {
	SM_RECORD_ABORT_PARMS abortParms;
	memset(&abortParms, 0, sizeof(abortParms));
	abortParms.channel = file_parms->record_parms.channel;
	abortParms.discard = 0;
	rc = sm_record_abort(&abortParms);
	if (!rc && !nowait) {
		while (file_parms->status == ERR_SM_PENDING
			|| file_parms->completing) {
			pthread_cond_wait(&record_complete, &record_mx);
		}
		rc = file_parms->status;
	}
	if (rc == ERR_SM_NO_RECORD_IN_PROGRESS) rc = 0;
 }
 pthread_mutex_unlock(&record_mx);
 return rc;
}

#include "../apilib/timestamp.h"
#include "../pubdoc/gen/highapi_playrec.i"
