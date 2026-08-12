// @(#) $Id: SccpAculabConstDef.h,v 1.1.4.1 2016/07/27 23:04:49 jamesjac Exp $
//---------------------------------------------------------------
// NAME : SccpAcuConstDef.h 
//
// COPYRIGHT
// TSS : - Copyright (C) 2010 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
// Originator  : Keerthini AB                    Date: 25 AUG 2014
//----------------------------------------------------------------

#ifndef SCCP_ACU_CONST_DEF_H
#define SCCP_ACU_CONST_DEF_H
#include <stdio.h>
#include <stdlib.h>
#include "Ss7ConstDef.h"
#include "SS7LogCodes.h"
#include "Ss7Structs.h"

#ifdef __cplusplus
extern "C"
{
         #include "sccp_api.h"
         #include "sccp_synch.h"
}
#endif

#define RED "\33[31m"
#define GREEN "\33[32m"
#define BLUE "\33[34m"
#define UNDO  "\33[0m"


#define ACU_SCCP_MAX_PROCESS_NAME 50

#define ACU_SCCP_MAX_COMPONENT    5

#define ACU_SCCP_ERR_TEXT_LEN     100

#define MAX_ACU_SCCP_INSTANCES    1

#define MAX_INSTANCE_PER_PC        7

#define TRACE_ACU_SCCP_HDLR_ENV "TRACE_ACULAB_SCCP_HDLR"

#define ACU_SCCP_PASS              "T@Y@N@SCCP@ACU"

#define SCCP_ACU_MAX_LIC_LEN       20 

#define SCCP_ACU_MAX_UDT_LENGTH    300

#define SEM_MIN  TSS_MIN_IPC 
#define SEM_MAX  TSS_MAX_IPC

#define SHM_MIN  TSS_MIN_IPC 
#define SHM_MAX  TSS_MAX_IPC 

#define MIN_DIS_PARAM_VAL 0
#define MAX_DIS_PARAM_VAL 255

#define SS7_PEG_CFG "Peg.cfg"

enum EnumSccpAcuEvents
{
   PEG_UDT_RCVD_FROM_STACK = 91,
   PEG_UDT_RCVD_FROM_APPL  = 92,
   PEG_UDT_SENT_TO_STACK   = 93,
   PEG_UDT_SENT_TO_APPL    = 94,
   /* Ab Comment
    * Counts every SCCP NOTICE (UDTS) bounce-back received from the network.
    * A non-zero value here indicates routing failures (dead PC, GTT miss,
    * subsystem congestion) that were previously invisible. */
   PEG_NOTICE_RCVD          = 95
};

#endif
