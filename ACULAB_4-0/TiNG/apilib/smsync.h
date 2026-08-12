#ifndef INCLUDED_SMSYNC_H
#define INCLUDED_SMSYNC_H

/* This is an unpublished API - do not use except for debugging */

#include "smdrvr.h"

typedef struct tSMSynchroniser_struct *tSMSynchroniser;

#include "../libutil/header_prolog.h"
typedef struct sm_sync_create_parms {
	tSMModuleId module;
	tSMSynchroniser synchroniser;
} SM_SYNC_CREATE_PARMS;

typedef struct sm_sync_add_parms {
	tSMSynchroniser synchroniser;
	tSMChannelId channel;
} SM_SYNC_ADD_PARMS;

typedef struct sm_sync_delete_parms {
	tSMSynchroniser synchroniser;
} SM_SYNC_DELETE_PARMS;
#include "../libutil/header_epilog.h"

ACUAPI int ACUTiNG_WINAPI sm_sync_create(struct sm_sync_create_parms *pp);
ACUAPI int ACUTiNG_WINAPI sm_sync_add(struct sm_sync_add_parms *pp);
ACUAPI int ACUTiNG_WINAPI sm_sync_delete(struct sm_sync_delete_parms *pp);

#endif
