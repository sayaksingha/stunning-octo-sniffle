/* This is an unpublished API - do not use except for debugging */

/*------------------------------------------------------------*/
/* Copyright ACULAB plc. (c) 1996-1998                        */
/*------------------------------------------------------------*/
/*                                                            */
/*                                                            */
/* Program File Name : smdiag.h 	                      	  */
/*                                                            */
/*           Purpose : SHARC Module driver header file        */
/*                     specific to diagnostic entry points    */
/*                                                            */
/*            Author : Peter Dain                             */
/*                                                            */
/*       Create Date : 21st February 1997                     */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/

#ifndef __SMDIAG__
#define __SMDIAG__

#ifndef __SMDRVR__
#include "smdrvr.h"
#endif

#define kSMDSPRegCmdRead 	0
#define kSMDSPRegCmdWrite 	1

typedef struct sm_dspreg_parms {
	tSMModuleId module;
	tSM_INT		regId;
	tSM_INT		cmd;
	tSM_INT		winDiag;
	tSM_UT32	value;
	tSM_UT32	irqs0;
	tSM_UT32	irqs1;
	tSM_UT32	lastMessage[2];
} SM_DSPREG_PARMS;

#define kSMMemCmdReadIntMem 		0
#define kSMMemCmdWriteIntMem 		1
#define kSMMemCmdReadExtMem 		2
#define kSMMemCmdWriteExtMem 		3

typedef struct sm_mem_parms {
	tSMModuleId module;
	tSM_INT			word_count;		/* Count of 32 bit words to set/retrive. */
	tSM_UT32		addr;
	tSM_INT			cmd;
	tSM_UT32		data[8];
} SM_MEM_PARMS;

typedef struct sm_diag_action_parms {
	tSM_INT			card;
	tSM_INT			p0;
	tSM_INT			p1;
	tSM_INT			p2;
	tSM_INT			p3;
} SM_DIAG_ACTION_PARMS;

typedef struct sm_diag_os_parms {
	tSM_INT			last_status;
	tSM_INT			last_location;
	tSM_INT			p0;
	tSM_INT			p1;
	tSM_INT			p2;
	tSM_INT			p3;
} SM_DIAG_OS_PARMS;


#define kSMDiagJobMsgTypeMsgFromModule 	1
#define kSMDiagJobMsgTypeJobToModule 	2
#define kSMDiagJobMsgTypeMsgFromDriver  3
#define kSMDiagJobMsgTypeAppAPICall		4

#define kSMDDiagTrackCmdTrackingOff		0
#define kSMDDiagTrackCmdTrackJobMsg		1
#define kSMDDiagTrackCmdTrackAPI		2

typedef struct sm_diag_jobmsg_parms {
	tSM_INT			tracking_on;
	tSM_INT			is_jobmsg;
	tSM_INT			card;
	tSMModuleId module;
	tSM_INT			tracked_type;
	tSM_UT32		timestamp;
	tSM_UT32		index;
	tSM_UT32		addr;
	tSM_UT32		words[6];
} SM_DIAG_JOBMSG_PARMS;

typedef struct sm_dspregall_parms {
	tSMModuleId module;
	tSM_INT 	regId;
	tSM_UT32	msgRegs[8];
	tSM_UT32	gp9;
	tSM_UT32	value;
	tSM_UT32	lastMessage[2];
} SM_DSPREGALL_PARMS;


typedef struct sm_drvstats_parms {
	tSM_UT32	writes_initiated;
	tSM_UT32	writes_completed;
	tSM_UT32	reads_initiated;
	tSM_UT32	reads_completed;
	tSM_UT32	dma_timeouts;
	tSM_INT		last_module;
	tSM_UT32	last_addr;
} SM_DRVSTATS_PARMS;


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_dspreg(
	struct sm_dspreg_parms*  
);

/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_mem(
	struct sm_mem_parms*  
);

/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_diag_action(
	struct sm_diag_action_parms*  
);

/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_diag_os(
	struct sm_diag_os_parms*  
);

/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_diag_jobmsg(
	struct sm_diag_jobmsg_parms*  
);


/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_dspregall(
	struct sm_dspregall_parms*  
);


/*
 * Diagnostic entry point - not for use in applications.
 */
ACUAPI int ACUTiNG_WINAPI sm_drvstats(
	struct sm_drvstats_parms*  
);

#ifdef __cplusplus
}
#endif


#endif

