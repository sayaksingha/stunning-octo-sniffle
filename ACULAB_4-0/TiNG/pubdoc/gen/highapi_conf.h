/* highapi_conf.h - Prosody high level conferencing API declarations */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

#ifndef INCLUDED_PUBDOC_HIGHAPI_CONF_H
#define INCLUDED_PUBDOC_HIGHAPI_CONF_H

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
#define sm_conference_create sm_conference_create_Ft
#define SM_CONFERENCE_CREATE_NAME "sm_conference_create_Ft"
#endif
typedef tSMConference SM_CONFERENCE_CREATE_FN(void);

ACUAPI tSMConference ACUTiNG_WINAPI sm_conference_create(void);


typedef struct sm_conference_add_party_parms {
	tSMConference conf;
	tSMChannelId channelIn;
	tSMChannelId channelOut;
	int out_volume;
	int out_agc;
	int in_volume;
	int in_agc;
} SM_CONFERENCE_ADD_PARTY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conference_add_party sm_conference_add_party_iPsFtCtCtiiii_
#define SM_CONFERENCE_ADD_PARTY_NAME "sm_conference_add_party_iPsFtCtCtiiii_"
#endif
typedef int SM_CONFERENCE_ADD_PARTY_FN(struct sm_conference_add_party_parms *parms);

ACUAPI int ACUTiNG_WINAPI sm_conference_add_party(struct sm_conference_add_party_parms *parms);


typedef struct sm_conference_remove_party_parms {
	tSMConference conf;
	tSMChannelId channelIn;
	tSMChannelId channelOut;
} SM_CONFERENCE_REMOVE_PARTY_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conference_remove_party sm_conference_remove_party_iPsFtCtCt_
#define SM_CONFERENCE_REMOVE_PARTY_NAME "sm_conference_remove_party_iPsFtCtCt_"
#endif
typedef int SM_CONFERENCE_REMOVE_PARTY_FN(struct sm_conference_remove_party_parms *parms);

ACUAPI int ACUTiNG_WINAPI sm_conference_remove_party(struct sm_conference_remove_party_parms *parms);


#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conference_delete sm_conference_delete_vFt
#define SM_CONFERENCE_DELETE_NAME "sm_conference_delete_vFt"
#endif
typedef void SM_CONFERENCE_DELETE_FN(tSMConference conf);

ACUAPI void ACUTiNG_WINAPI sm_conference_delete(tSMConference conf);


typedef struct sm_conference_info_parms {
	tSMConference conf;
	int activeChannelCount;
	tSMChannelId *activeChannelList;
} SM_CONFERENCE_INFO_PARMS;

#ifndef TiNG_USE_UNDECORATED_NAMES
#define sm_conference_info sm_conference_info_iPsFtiPCt_
#define SM_CONFERENCE_INFO_NAME "sm_conference_info_iPsFtiPCt_"
#endif
typedef int SM_CONFERENCE_INFO_FN(struct sm_conference_info_parms *parms);

ACUAPI int ACUTiNG_WINAPI sm_conference_info(struct sm_conference_info_parms *parms);



#endif
