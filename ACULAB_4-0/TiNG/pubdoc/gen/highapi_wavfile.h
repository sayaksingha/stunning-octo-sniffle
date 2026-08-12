/* highapi_wavfile.h - Prosody WAV FILE API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_HIGHAPI_WAVFILE_H
#define INCLUDED_PUBDOC_HIGHAPI_WAVFILE_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif



#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_start sm_replay_wav_start_iPcPFPPt
#define SM_REPLAY_WAV_START_NAME "sm_replay_wav_start_iPcPFPPt"
#endif
typedef int SM_REPLAY_WAV_START_FN(char *filename, SM_FILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_start(char *filename, SM_FILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_close sm_replay_wav_close_iPFPPt
#define SM_REPLAY_WAV_CLOSE_NAME "sm_replay_wav_close_iPFPPt"
#endif
typedef int SM_REPLAY_WAV_CLOSE_FN(SM_FILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_close(SM_FILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_start sm_record_wav_start_iPcPFCPt
#define SM_RECORD_WAV_START_NAME "sm_record_wav_start_iPcPFCPt"
#endif
typedef int SM_RECORD_WAV_START_FN(char *filename, SM_FILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_start(char *filename, SM_FILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_close sm_record_wav_close_iPFCPt
#define SM_RECORD_WAV_CLOSE_NAME "sm_record_wav_close_iPFCPt"
#endif
typedef int SM_RECORD_WAV_CLOSE_FN(SM_FILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_close(SM_FILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_trim_close sm_record_wav_trim_close_iPFCPtUi
#define SM_RECORD_WAV_TRIM_CLOSE_NAME "sm_record_wav_trim_close_iPFCPtUi"
#endif
typedef int SM_RECORD_WAV_TRIM_CLOSE_FN(SM_FILE_RECORD_PARMS *file_parms, tSM_UT32 trimLength);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_trim_close(SM_FILE_RECORD_PARMS *file_parms, tSM_UT32 trimLength);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_get_type sm_replay_wav_get_type_iPcPi
#define SM_REPLAY_WAV_GET_TYPE_NAME "sm_replay_wav_get_type_iPcPi"
#endif
typedef int SM_REPLAY_WAV_GET_TYPE_FN(char *filename, int *replay_type);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_get_type(char *filename, int *replay_type);


typedef struct sm_replay_wav_prepare_parms {
	char *filename;
	SM_REPLAY_PARMS *replay_parms;
} SM_REPLAY_WAV_PREPARE_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_prepare sm_replay_wav_prepare_iPsPcPRPt_
#define SM_REPLAY_WAV_PREPARE_NAME "sm_replay_wav_prepare_iPsPcPRPt_"
#endif
typedef int SM_REPLAY_WAV_PREPARE_FN(struct sm_replay_wav_prepare_parms *replay_wav_prep_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_prepare(struct sm_replay_wav_prepare_parms *replay_wav_prep_parms);



#endif
