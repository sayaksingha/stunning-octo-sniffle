// @(#) $Id: SccpAculabApi.h,v 1.1.4.1 2016/07/27 23:04:49 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME:SccpAculabApi.h 
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated By : Keerthini AB                            Date : 25 Aug 2014
//----------------------------------------------------------------------------
#ifndef SCCP_ACULAB_H
#define SCCP_ACULAB_H

#include "PegApi.h"
#include "Ss7ConstDef.h"
#include "Ss7Structs.h"
#include "MsuStructs.h"
#include "SccpAculabUtil.h"
#include "map"

extern PegApi gPeg;

using namespace std;

typedef enum _SsapState
{
   CONNECTING,
   IN_SERVICE,
   EXITING
}SsapState;

typedef struct _SsapStatus 
{
   SsapState            state;
   UINT16               transIdRange;
   time_t               lastActTime;
   acu_sccp_cs_state_t  host_a_con_state;
   acu_sccp_cs_state_t  host_b_con_state;
}SsapStatus;

class SccpAculab
{
   acu_sccp_ssap_t   *mSsapPtr;
   
   AculabUtil        mAcuSccpUtil;

   const acu_sccp_con_state_t *pConA_State;
   const acu_sccp_con_state_t *pConB_State;
   acu_sccp_addr_t            *mLocAddr;
   acu_sccp_addr_t            *mRemAddr;

   TEXT           mCfgFile[100 + 1];

   BOOLEAN        mPegFlag;

   SsapStatus     mSaapStatus; 

   acu_sccp_con_t *mConnection;

   int            mLocalPc;

   public:

     SccpAculab();
     
     ~SccpAculab();
     
     BOOLEAN Init(TEXT *pCfgFile,BOOLEAN lPegFlag);
  
     BOOLEAN SetSccpConfigFilePath(TEXT *lCfgFile);

     BOOLEAN ReadConfig();

     BOOLEAN ReadSccpConfig();

     BOOLEAN ReloadConfig();

     BOOLEAN ReadIpcConfig();

     BOOLEAN SsapCreate(BOOLEAN reload = false);

     BOOLEAN SsapConnect();
   
     BOOLEAN SsapReConnect();
   
     BOOLEAN UpdateSsapStatus();

     BOOLEAN GetSsapStatus();

     BOOLEAN GetAcuSccpConState(acu_sccp_msg_t *lMsg); 
      
     BOOLEAN GetAcuSccpEvent(acu_sccp_msg_t **lAcuSccpMsgPtr);

     acu_sccp_msg_type_t HandleEventAcuSccpMsg(acu_sccp_msg_t *lAcuSccpMsgPtr);

     BOOLEAN SendAcuSccpMsg(const void *data, unsigned int dataLen, UINT32 transId); 
     
     BOOLEAN SetRemoteLocalAddress(acu_sccp_addr_t lLocAddr, acu_sccp_addr_t lRemAddr);

     BOOLEAN UnblockConnection(acu_sccp_con_t *con);

     BOOLEAN GetSccpStatus(INT16 dest, BOOLEAN &status);
};

#endif
