//---------------------------------------------------------------
// NAME : TcapAcuConstDef.h 
//
// COPYRIGHT
// TSS : - Copyright (C) 2010 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
// Originator  : Paresh                         Date: 30/03/10
//----------------------------------------------------------------

#ifndef TCAP_ACU_CONST_DEF_H
#define TCAP_ACU_CONST_DEF_H
#include <stdio.h>
#include <stdlib.h>
#include "Ss7ConstDef.h"
#include "SS7LogCodes.h"
#include "TcapUtil.h"
#include "TcapAculabConstDef.h"
#include "TcapMsgDisplayApi.h"
//#include "OamLogCodes.h" CMS

#ifdef __cplusplus
extern "C"
{
      #include "tcap_api.h"
      #include "tcap_synch.h"
}
#endif


#define RED "\33[31m"
#define GREEN "\33[32m"
#define BLUE "\33[34m"
#define UNDO  "\33[0m"

#define DLG_TIMEOUT_MIN 1

#define DLG_TIMEOUT_MAX 5000

#define NUM_OF_SEM      1

#define ACU_TCAP_MAX_PROCESS_NAME 50

#define ACU_TCAP_MAX_COMPONENT    5

#define ACU_TCAP_ERR_TEXT_LEN     100

#define MAX_ACU_TCAP_INSTANCES    50
//#define MAX_INSTANCE_PER_PC        7
#define MAX_INSTANCE_PER_PC        10

#define TRACE_ACU_TCAP_HDLR_ENV "TRACE_ACULAB_TCAP_HDLR"

#define TRACE_ACU_TCAP_TRAFFIC_ENV "TRACE_ACULAB_TCAP_HDLR_TRAFFIC"

#define TRACE_ACU_TCAP_DLG_CLEAN_ENV "TRACE_ACULAB_TCAP_DLG_CLEANER"

#define TRACE_SSFC_DLG_CLEANER_ENV "TRACE_TCAP_ACU_DLG_CLEANER"

#define ACU_TCAP_PASS              "T@Y@N@TCAP@ACU"

#define TCAP_ACU_MAX_LIC_LEN       20 

#define SEM_MIN  TSS_MIN_IPC //1
#define SEM_MAX  TSS_MAX_IPC //10000

#define SHM_MIN  TSS_MIN_IPC //1
#define SHM_MAX  TSS_MAX_IPC //10000

#define MIN_DIS_PARAM_VAL 0
#define MAX_DIS_PARAM_VAL 255

#define MIN_ACU_TCAP_DLG  1
#define MAX_ACU_TCAP_DLG  500000

#define MIN_ACU_TCAP_SHIFT_DLG  0

#define SS7_PEG_CFG "Peg.cfg"



enum SubSemaphoreIds
{
   SEM_FOR_IN_DLG_POOL = 0,
   SEM_FOR_OUT_DLG_POOL,
   SEM_FOR_DLG_MGMT_QUEUE
};

struct DlgRecord
{
   unsigned int      dlgId ;
   acu_tcap_trans_t  *trans;
   int               ssn;
   time_t            insertTime;
   int               transValidationKey; //Randomly generated

   int             	origTransIdLen;
   int             	destTransIdLen;
   unsigned int      origTransId;
   unsigned int      destTransId;
   int               invokeId;
   acu_sccp_addr_t   callingAddr;
   acu_sccp_addr_t   calledAddr;
	UINT8  				opCode;
	EnumTcapDlg			dlgType;
   TcapAcnPdu        applicationContext;
	BOOLEAN				restarted;
   int               ssapInstance;
   #ifdef KAFKA_BRIDGE
   KafkaRoutingInfo   routing;
   #endif

   DlgRecord()
   {
      dlgId = 0;
      ssn = 0;
      memset(&insertTime,0,sizeof(time_t));
      transValidationKey = 0;
      invokeId = 0;
      origTransId = 0;
      destTransId = 0;
      memset(&callingAddr,0,sizeof(callingAddr));
      memset(&calledAddr,0,sizeof(calledAddr));
      memset(&applicationContext,0,sizeof(applicationContext));
		dlgType = TCAP_CONTROL;
		opCode	=	0;
		restarted	=	false;
      ssapInstance   =  0;
      //callingAddr = NULL;
      //calledAddr = NULL;
   }
};
// Dlg Management Queue
struct DlgMgmtQueueHeader
{
   UINT32             nextFreeDlgIdx;
   UINT32             lastFreeDlgIdx;
   UINT32             recordCount;
};

struct DlgMgmtQueue
{
   DlgMgmtQueueHeader  *HeaderPtr;
   UINT32              *dlgIdx;   // will point to array[gMaxMsgSize] of MSG_IDX_DATA_TYPE
};


typedef DlgRecord* DlgRecordPool;

struct TcapTransIds
{
   unsigned int locId;
   unsigned int remId;
   unsigned int remIdLen;
};


enum EnumAddrFlag
{
   LOCAL_ADDR = 0,
   REMOTE_ADDR = 1
};

enum EnumDelFlag
{
   NO_DELETE_TRANS = 0,
   DELTE_TRANS     = 1
};
enum EnumTcapAcuEvents
{
   PEG_DROP_RCVD_FROM_NWK = 59,
   PEG_DROP_SEND_TO_NWK   = 60,
   PEG_RCVD_FROM_APP      = 81,
   PEG_SEND_TO_NWK        = 82,
   PEG_RCVD_FROM_NWK      = 83,
   PEG_SEND_TO_APPL       = 84
};



#endif
