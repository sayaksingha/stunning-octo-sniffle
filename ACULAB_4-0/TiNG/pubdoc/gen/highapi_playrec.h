/* highapi_playrec.h - Prosody high level FILE play/record API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_HIGHAPI_PLAYREC_H
#define INCLUDED_PUBDOC_HIGHAPI_PLAYREC_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif

#define kSMMaxHiRecordDataBufferSize 2048
#define kSMMaxHiReplayDataBufferSize 2048


typedef struct sm_file_replay_parms {
	tSM_OPEN_FILE fd;
	tSM_UT32 offset;
	SM_REPLAY_PARMS replay_parms;
	int status;
	tSM_UT32 private_length;
	int completing;
	int data_length_in_buffer;
	char buffer[kSMMaxHiReplayDataBufferSize];
} SM_FILE_REPLAY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_file_start sm_replay_file_start_iPsOFtUiRPtiUiiicr2048_
#define SM_REPLAY_FILE_START_NAME "sm_replay_file_start_iPsOFtUiRPtiUiiicr2048_"
#endif
typedef int SM_REPLAY_FILE_START_FN(struct sm_file_replay_parms *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_file_start(struct sm_file_replay_parms *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_file_progress_istatus sm_replay_file_progress_istatus_iPFPPti
#define SM_REPLAY_FILE_PROGRESS_ISTATUS_NAME "sm_replay_file_progress_istatus_iPFPPti"
#endif
typedef int SM_REPLAY_FILE_PROGRESS_ISTATUS_FN(SM_FILE_REPLAY_PARMS *file_parms, int initial_status);

ACUAPI int ACUTiNG_WINAPI sm_replay_file_progress_istatus(SM_FILE_REPLAY_PARMS *file_parms, int initial_status);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_file_progress sm_replay_file_progress_iPFPPt
#define SM_REPLAY_FILE_PROGRESS_NAME "sm_replay_file_progress_iPFPPt"
#endif
typedef int SM_REPLAY_FILE_PROGRESS_FN(SM_FILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_file_progress(SM_FILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_file_complete sm_replay_file_complete_iPFPPt
#define SM_REPLAY_FILE_COMPLETE_NAME "sm_replay_file_complete_iPFPPt"
#endif
typedef int SM_REPLAY_FILE_COMPLETE_FN(SM_FILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_file_complete(SM_FILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_file_stop sm_replay_file_stop_iPFPPti
#define SM_REPLAY_FILE_STOP_NAME "sm_replay_file_stop_iPFPPti"
#endif
typedef int SM_REPLAY_FILE_STOP_FN(SM_FILE_REPLAY_PARMS *file_parms, int nowait);

ACUAPI int ACUTiNG_WINAPI sm_replay_file_stop(SM_FILE_REPLAY_PARMS *file_parms, int nowait);


typedef struct sm_file_record_parms {
	tSM_OPEN_FILE fd;
	SM_RECORD_PARMS record_parms;
	int status;
	tSM_UT32 private_length;
	int completing;
	char buffer[kSMMaxHiRecordDataBufferSize];
} SM_FILE_RECORD_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_start sm_record_file_start_iPsOFtRCtiUiicr2048_
#define SM_RECORD_FILE_START_NAME "sm_record_file_start_iPsOFtRCtiUiicr2048_"
#endif
typedef int SM_RECORD_FILE_START_FN(struct sm_file_record_parms *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_file_start(struct sm_file_record_parms *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_progress_istatus sm_record_file_progress_istatus_iPFCPti
#define SM_RECORD_FILE_PROGRESS_ISTATUS_NAME "sm_record_file_progress_istatus_iPFCPti"
#endif
typedef int SM_RECORD_FILE_PROGRESS_ISTATUS_FN(SM_FILE_RECORD_PARMS *file_parms, int initial_status);

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress_istatus(SM_FILE_RECORD_PARMS *file_parms, int initial_status);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_progress_ostatus sm_record_file_progress_ostatus_iPFCPtPRSPt
#define SM_RECORD_FILE_PROGRESS_OSTATUS_NAME "sm_record_file_progress_ostatus_iPFCPtPRSPt"
#endif
typedef int SM_RECORD_FILE_PROGRESS_OSTATUS_FN(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress_ostatus(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_progress sm_record_file_progress_iPFCPt
#define SM_RECORD_FILE_PROGRESS_NAME "sm_record_file_progress_iPFCPt"
#endif
typedef int SM_RECORD_FILE_PROGRESS_FN(SM_FILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_file_progress(SM_FILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_complete sm_record_file_complete_iPFCPt
#define SM_RECORD_FILE_COMPLETE_NAME "sm_record_file_complete_iPFCPt"
#endif
typedef int SM_RECORD_FILE_COMPLETE_FN(SM_FILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_file_complete(SM_FILE_RECORD_PARMS *file_parms);

#ifdef PROSODY_TiNG

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_complete_tstatus sm_record_file_complete_tstatus_iPFCPtPRSPt
#define SM_RECORD_FILE_COMPLETE_TSTATUS_NAME "sm_record_file_complete_tstatus_iPFCPtPRSPt"
#endif
typedef int SM_RECORD_FILE_COMPLETE_TSTATUS_FN(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_file_complete_tstatus(SM_FILE_RECORD_PARMS *file_parms, SM_RECORD_STATUS_PARMS *status_parms);

#endif

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_file_stop sm_record_file_stop_iPFCPti
#define SM_RECORD_FILE_STOP_NAME "sm_record_file_stop_iPFCPti"
#endif
typedef int SM_RECORD_FILE_STOP_FN(SM_FILE_RECORD_PARMS *file_parms, int nowait);

ACUAPI int ACUTiNG_WINAPI sm_record_file_stop(SM_FILE_RECORD_PARMS *file_parms, int nowait);



#endif
