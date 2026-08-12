/* pros_x.h - Prosody X API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_PROS_X_H
#define INCLUDED_PUBDOC_PROS_X_H

#ifdef ACULAB_TiNG_VERSION_NUM
#if ACULAB_TiNG_VERSION_NUM != 1504
#error ACULAB_TiNG_VERSION_NUM does not match 1504
#endif
#else
#define ACULAB_TiNG_VERSION_NUM 1504
#define ACULAB_TiNG_VERSION_CHANGED 
#define ACULAB_TiNG_VERSION_STRING "m1504."
#endif



typedef struct sm_open_prosody_s_v3_parms {
	char *ps_locator;
	tSMCardId card_id;
} SM_OPEN_PROSODY_S_V3_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_open_prosody_s_v3 sm_open_prosody_s_v3_iPsPcCAt_
#define SM_OPEN_PROSODY_S_V3_NAME "sm_open_prosody_s_v3_iPsPcCAt_"
#endif
typedef int SM_OPEN_PROSODY_S_V3_FN(struct sm_open_prosody_s_v3_parms *openp);

ACUAPI int ACUTiNG_WINAPI sm_open_prosody_s_v3(struct sm_open_prosody_s_v3_parms *openp);


typedef struct sm_open_prosody_x_parms {
	char *px_locator;
	tSMCardId card_id;
} SM_OPEN_PROSODY_X_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_open_prosody_x sm_open_prosody_x_iPsPcCAt_
#define SM_OPEN_PROSODY_X_NAME "sm_open_prosody_x_iPsPcCAt_"
#endif
typedef int SM_OPEN_PROSODY_X_FN(struct sm_open_prosody_x_parms *openp);

ACUAPI int ACUTiNG_WINAPI sm_open_prosody_x(struct sm_open_prosody_x_parms *openp);



#endif
