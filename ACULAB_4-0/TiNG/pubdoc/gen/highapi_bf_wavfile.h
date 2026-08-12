/* highapi_bf_wavfile.h - Prosody WAV BFILE API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_HIGHAPI_BF_WAVFILE_H
#define INCLUDED_PUBDOC_HIGHAPI_BF_WAVFILE_H

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
#define sm_replay_wav_bf_start sm_replay_wav_bf_start_iPcPBPPt
#define SM_REPLAY_WAV_BF_START_NAME "sm_replay_wav_bf_start_iPcPBPPt"
#endif
typedef int SM_REPLAY_WAV_BF_START_FN(char *filename, SM_BFILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_start(char *filename, SM_BFILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_bf_close sm_replay_wav_bf_close_iPBPPt
#define SM_REPLAY_WAV_BF_CLOSE_NAME "sm_replay_wav_bf_close_iPBPPt"
#endif
typedef int SM_REPLAY_WAV_BF_CLOSE_FN(SM_BFILE_REPLAY_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_close(SM_BFILE_REPLAY_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_bf_start sm_record_wav_bf_start_iPcPBCPt
#define SM_RECORD_WAV_BF_START_NAME "sm_record_wav_bf_start_iPcPBCPt"
#endif
typedef int SM_RECORD_WAV_BF_START_FN(char *filename, SM_BFILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_start(char *filename, SM_BFILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_bf_close sm_record_wav_bf_close_iPBCPt
#define SM_RECORD_WAV_BF_CLOSE_NAME "sm_record_wav_bf_close_iPBCPt"
#endif
typedef int SM_RECORD_WAV_BF_CLOSE_FN(SM_BFILE_RECORD_PARMS *file_parms);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_close(SM_BFILE_RECORD_PARMS *file_parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_record_wav_bf_trim_close sm_record_wav_bf_trim_close_iPBCPtUi
#define SM_RECORD_WAV_BF_TRIM_CLOSE_NAME "sm_record_wav_bf_trim_close_iPBCPtUi"
#endif
typedef int SM_RECORD_WAV_BF_TRIM_CLOSE_FN(SM_BFILE_RECORD_PARMS *file_parms, tSM_UT32 trimLength);

ACUAPI int ACUTiNG_WINAPI sm_record_wav_bf_trim_close(SM_BFILE_RECORD_PARMS *file_parms, tSM_UT32 trimLength);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_replay_wav_bf_get_type sm_replay_wav_bf_get_type_iPcPi
#define SM_REPLAY_WAV_BF_GET_TYPE_NAME "sm_replay_wav_bf_get_type_iPcPi"
#endif
typedef int SM_REPLAY_WAV_BF_GET_TYPE_FN(char *filename, int *replay_type);

ACUAPI int ACUTiNG_WINAPI sm_replay_wav_bf_get_type(char *filename, int *replay_type);



#endif
