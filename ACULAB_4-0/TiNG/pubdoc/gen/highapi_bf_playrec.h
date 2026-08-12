/* highapi_bf_playrec.h - Prosody high level BFILE play/record API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_HIGHAPI_BF_PLAYREC_H
#define INCLUDED_PUBDOC_HIGHAPI_BF_PLAYREC_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

#define kSMMaxHBRecordDataBufferSize 2048
#define kSMMaxHBReplayDataBufferSize 2048


typedef struct sm_bfile_replay_parms {
	BFILE *fd;
	tSM_UT32 offset;
	SM_REPLAY_PARMS replay_parms;
	int status;
	tSM_UT32 private_length;
	int completing;
	int data_length_in_buffer;
	char buffer[kSMMaxHBReplayDataBufferSize];
} SM_BFILE_REPLAY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_bfile_start sm_replay_bfile_start_iPsPBtUiRPtiUiiicr2048_
#define SM_REPLAY_BFILE_START_NAME "sm_replay_bfile_start_iPsPBtUiRPtiUiiicr2048_"
#endif
typedef int SM_REPLAY_BFILE_START_FN(struct sm_bfile_replay_parms *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_bfile_start(struct sm_bfile_replay_parms *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_bfile_progress_istatus sm_replay_bfile_progress_istatus_iPBPPti
#define SM_REPLAY_BFILE_PROGRESS_ISTATUS_NAME "sm_replay_bfile_progress_istatus_iPBPPti"
#endif
typedef int SM_REPLAY_BFILE_PROGRESS_ISTATUS_FN(SM_BFILE_REPLAY_PARMS *file_parms, int initial_status);

ACUAPI int ACUTiNG_WINAPI sm_replay_bfile_progress_istatus(SM_BFILE_REPLAY_PARMS *file_parms, int initial_status);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_bfile_progress sm_replay_bfile_progress_iPBPPt
#define SM_REPLAY_BFILE_PROGRESS_NAME "sm_replay_bfile_progress_iPBPPt"
#endif
typedef int SM_REPLAY_BFILE_PROGRESS_FN(SM_BFILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_bfile_progress(SM_BFILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_bfile_complete sm_replay_bfile_complete_iPBPPt
#define SM_REPLAY_BFILE_COMPLETE_NAME "sm_replay_bfile_complete_iPBPPt"
#endif
typedef int SM_REPLAY_BFILE_COMPLETE_FN(SM_BFILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_bfile_complete(SM_BFILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_bfile_stop sm_replay_bfile_stop_iPBPPti
#define SM_REPLAY_BFILE_STOP_NAME "sm_replay_bfile_stop_iPBPPti"
#endif
typedef int SM_REPLAY_BFILE_STOP_FN(SM_BFILE_REPLAY_PARMS *file_parms, int nowait);

ACUAPI int ACUTiNG_WINAPI sm_replay_bfile_stop(SM_BFILE_REPLAY_PARMS *file_parms, int nowait);


typedef struct sm_bfile_record_parms {
	BFILE *fd;
	SM_RECORD_PARMS record_parms;
	int status;
	tSM_UT32 private_length;
	int completing;
	char buffer[kSMMaxHBRecordDataBufferSize];
} SM_BFILE_RECORD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_start sm_record_bfile_start_iPsPBtRCtiUiicr2048_
#define SM_RECORD_BFILE_START_NAME "sm_record_bfile_start_iPsPBtRCtiUiicr2048_"
#endif
typedef int SM_RECORD_BFILE_START_FN(struct sm_bfile_record_parms *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_start(struct sm_bfile_record_parms *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_progress_istatus sm_record_bfile_progress_istatus_iPBCPti
#define SM_RECORD_BFILE_PROGRESS_ISTATUS_NAME "sm_record_bfile_progress_istatus_iPBCPti"
#endif
typedef int SM_RECORD_BFILE_PROGRESS_ISTATUS_FN(SM_BFILE_RECORD_PARMS *file_parms, int initial_status);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_progress_istatus(SM_BFILE_RECORD_PARMS *file_parms, int initial_status);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_progress_ostatus sm_record_bfile_progress_ostatus_iPBCPtPRSPt
#define SM_RECORD_BFILE_PROGRESS_OSTATUS_NAME "sm_record_bfile_progress_ostatus_iPBCPtPRSPt"
#endif
typedef int SM_RECORD_BFILE_PROGRESS_OSTATUS_FN(SM_BFILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_progress_ostatus(SM_BFILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_progress sm_record_bfile_progress_iPBCPt
#define SM_RECORD_BFILE_PROGRESS_NAME "sm_record_bfile_progress_iPBCPt"
#endif
typedef int SM_RECORD_BFILE_PROGRESS_FN(SM_BFILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_progress(SM_BFILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_complete sm_record_bfile_complete_iPBCPt
#define SM_RECORD_BFILE_COMPLETE_NAME "sm_record_bfile_complete_iPBCPt"
#endif
typedef int SM_RECORD_BFILE_COMPLETE_FN(SM_BFILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_complete(SM_BFILE_RECORD_PARMS *file_parms);

#ifdef PROSODY_TiNG

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_complete_tstatus sm_record_bfile_complete_tstatus_iPBCPtPRSPt
#define SM_RECORD_BFILE_COMPLETE_TSTATUS_NAME "sm_record_bfile_complete_tstatus_iPBCPtPRSPt"
#endif
typedef int SM_RECORD_BFILE_COMPLETE_TSTATUS_FN(SM_BFILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_complete_tstatus(SM_BFILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

#endif

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_bfile_stop sm_record_bfile_stop_iPBCPti
#define SM_RECORD_BFILE_STOP_NAME "sm_record_bfile_stop_iPBCPti"
#endif
typedef int SM_RECORD_BFILE_STOP_FN(SM_BFILE_RECORD_PARMS *file_parms, int nowait);

ACUAPI int ACUTiNG_WINAPI sm_record_bfile_stop(SM_BFILE_RECORD_PARMS *file_parms, int nowait);



#endif
