/* visdecl.h - Prosody declarations for Linux visible to the API */

#ifndef INCLUDED_VISDECL_H
#define INCLUDED_VISDECL_H

typedef int tSM_INT;
typedef unsigned long long tSM_UT64;
typedef unsigned long tSM_UT32;
typedef unsigned char tSM_UT8;     

typedef struct tSMEventId_internals *tSM_EVENT_INTERNALS;

typedef struct tSMEventId {
	tSM_INT fd;
	tSM_INT mode;
	tSM_EVENT_INTERNALS eip;
} tSMEventId;

typedef float tSMIEEE32Bit754854Float;

#endif
