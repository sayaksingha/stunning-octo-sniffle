static const char *id = "@(#) $Id: SccpAculabApi.cc,v 1.1.4.1.4.1 2021/12/01 12:43:25 jamesjac Exp $";
//----------------------------------------------------------------------------
// NAME: SccpAculabApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// Originator : Keerthini AB                               Date : 25 AUG 2014
//----------------------------------------------------------------------------

#include "SccpAculabApi.h"

//extern SsapDetailsStruct gSsapDetails;
extern TEXT gCfgFile[MAX_LOG_TEXT_LEN + 1];
//-------------------------------------------------------------------------------
// METHOD      : SccpAculab
// DESCRIPTION : Constructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
SccpAculab::SccpAculab()
{
   memset(&mSaapStatus, 0, sizeof(mSaapStatus));
   mSsapPtr = NULL;
   // Defensive initialization: Prevents arbitrary segmentation faults if any Aculab 
   // connection-setter functions are accidentally invoked before mConnection is properly assigned.
   mConnection = NULL;
}

//-------------------------------------------------------------------------------
// METHOD      : ~SccpAculab
// DESCRIPTION : Destructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
SccpAculab::~SccpAculab()
{

}
//-------------------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializing Api
// PARAMETER   : TEXT *
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::Init(TEXT *lCfgFile, BOOLEAN lPegFlag)
{
   mPegFlag = lPegFlag;

   if(false == ReadSccpConfig())
   {
      return false;
   }

   if(false == SsapCreate())
   {
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION : Reload Configurations
// PARAMETER   :
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::ReloadConfig()
{
   if(false == ReadSccpConfig())
   {
      return false;
   }

   if(false == SsapCreate(true))
   {
      return false;
   }
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetSccpConfigFilePath
// DESCRIPTION : Sets SCCP Application config file path
// PARAMETER   : TEXT *lCfgFile
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::SetSccpConfigFilePath(TEXT *lCfgFile)
{
   TEXT lProductHome[50 + 1] = "";
   TEXT lCfgPath[50 + 1] = "";

   strcpy(lProductHome ,getenv("PRODUCT_HOME"));
   if(NULL == lProductHome)
   {
      gLog.GenerateLog(ACUSCCP13, "SCCP config PRODUCT_HOME fail");
      return false;
   }

   strcpy(lCfgPath, getenv("PRODUCT_CFG_PATH"));
   if(NULL == lCfgPath)
   {
      gLog.GenerateLog(ACUSCCP13, "SCCP config PRODUCT_CFG_PATH fail");
      return false;
   }

   snprintf(mCfgFile, sizeof(mCfgFile), "%s%s/%s", lProductHome, lCfgPath, lCfgFile);
   T(gTrace,printf("%s: SCCP config file:%s\n",gProcessName,mCfgFile););
      gLog.GenerateLog(ACUSCCP13, mCfgFile);

   return true;

}
//-------------------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : Reads config parameters
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::ReadConfig()
{
   if(false == ReadSccpConfig())
   {
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReadSccpConfig
// DESCRIPTION : Reads config params related to SCCP
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::ReadSccpConfig()
{
   CfgRead        lCfgRead("ACULAB_SCCP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   mLocalPc       = 0;

   if (false == lCfgRead.CfgInit(gCfgFile))
   {
      sprintf(lLogText, "CfgRead object(%s) intialization failed...No such File",gCfgFile);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   if( CFG_OK !=  lCfgRead.GetConfigNum("LocalPC", mLocalPc,
            1, 35000))
   {
      sprintf(lLogText, "Config read Err for LocalPC in %s",gCfgFile);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }

   lCfgRead.CfgDeInit();

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SsapCreate
// DESCRIPTION : Creates aculab SSAP for application
// PARAMETER   : NONE
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::SsapCreate(BOOLEAN reload)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lTraceTag[20] = "";

   if(false == SetSccpConfigFilePath(gCfgFile))
   {
      return false;
   }
   mSsapPtr = acu_sccp_ssap_create(mCfgFile, acu_sccp_ssap_flags_t(ACU_SCCP_LOG_STDERR));

   if(NULL == mSsapPtr)
   {
      sprintf(lLogText,"SSAP creation failed, config file %s",mCfgFile);
      TERR(gTrace, printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP01,lLogText);
      return false;
   }

   mLocAddr = acu_sccp_ssap_get_locaddr(mSsapPtr);
   sprintf(lTraceTag, "sccp_%d_%d", mLocAddr->sa_pc, 0);
   if((unsigned int)mLocalPc != mLocAddr->sa_pc)
   {
      sprintf(lLogText,"SSAP creation failed, LocalPC:%d OPC in file %s is %d",
            mLocalPc, mCfgFile, mLocAddr->sa_pc);
      TERR(gTrace, printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP01,lLogText);
      acu_sccp_ssap_delete(mSsapPtr);
      return false;
   }

   acu_sccp_ssap_set_cfg_str(mSsapPtr, ACU_SCCP_CFG_TRACE_TAG,lTraceTag);

   //Getting remote node adress details
   mRemAddr = acu_sccp_ssap_get_remaddr(mSsapPtr);

   if (mRemAddr->sa_pc != 0)
   {
      acu_sccp_enable_sp_status(mSsapPtr, mRemAddr->sa_pc);
      sprintf(lLogText,"SSAP SP status enabled  remote PC: %d",
            mRemAddr->sa_pc);
      T(gTrace, printf("%s:%s\n",gProcessName, lLogText););

   }
   else
   {
      T(gTrace, printf("%s:%s SSAP SP status disabled%s\n",gProcessName,RED,UNDO););
   }

   //This function enables the receipt of ACU_SCCP_MSG_USER_STATUS
   //messages for the specified pointcode and ssn.
   acu_sccp_enable_user_status(mSsapPtr, mRemAddr->sa_pc ? mRemAddr->sa_pc : ~0u,
         mRemAddr->sa_ssn ? mRemAddr->sa_ssn : ~0u);
   sprintf(lLogText,"SSAP user status recieve enabled remote(PC:%d SSN:%d)",
         mRemAddr->sa_pc, mRemAddr->sa_ssn);
   T(gTrace,printf("%s:%s\n",gProcessName, lLogText););

   mSaapStatus.lastActTime = time(NULL);

   if(false == SsapConnect())
   {
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SsapConnect
// DESCRIPTION : Application uses this api to connect to SSAP
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::SsapConnect()
{
   int   lRetVal = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //This function causes the SCCP library to try to establish a TCP/IP
   //connection between the ssap and the SCCP driver code.

   if(mSsapPtr == NULL)
   {
      sprintf(lLogText,"SSAP mSsapPtr NULL");
      TERR(gTrace, printf("%s:%s %s%s\n",gProcessName,RED,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP02,lLogText);
      return false;
   }

   lRetVal = acu_sccp_ssap_connect_sccp(mSsapPtr);

   if(0 != lRetVal)
   {
      sprintf(lLogText,"SSAP connected to SCCP driver code failed");
      TERR(gTrace, printf("%s:%s %s%s\n",gProcessName,RED,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP02,lLogText);
      return false;
   }

   sprintf(lLogText,"SSAP connected to SCCP driver code success");
   T(gTrace, printf("%s:%s %s%s\n",gProcessName,BLUE,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP12,lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SsapReConnect
// DESCRIPTION : Reconnects the coonection with SSAP
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::SsapReConnect()
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lTraceTag[10] = "";
   int lTempInstance = 0;

   mLocAddr = acu_sccp_ssap_get_locaddr(mSsapPtr);

   acu_sccp_ssap_delete(mSsapPtr);
   //setting transmitflag as false;
   AculabUtil::mTransmitFlag = false;
   mSsapPtr = NULL;

   if(false == SetSccpConfigFilePath(gCfgFile))
   {
      return false;
   }
   mSsapPtr = acu_sccp_ssap_create(mCfgFile,
         acu_sccp_ssap_flags_t(ACU_SCCP_LOG_STDERR));

   if(NULL == mSsapPtr)
   {
      sprintf(lLogText,"SSAP creation failed, config file %s",mCfgFile);
      //printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO);
      T(gTrace, printf("%s:%s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP01,lLogText);
      return false;
   }
   sprintf(lTraceTag, "sccp_%d_%d", mLocAddr->sa_pc, lTempInstance);
   acu_sccp_ssap_set_cfg_str(mSsapPtr, ACU_SCCP_CFG_TRACE_TAG,lTraceTag);

   mRemAddr = acu_sccp_ssap_get_remaddr(mSsapPtr);

   if (mRemAddr->sa_pc != 0)
   {
      acu_sccp_enable_sp_status(mSsapPtr, mRemAddr->sa_pc);
      sprintf(lLogText,"SSAP SP status enabled  remote PC: %d",
            mRemAddr->sa_pc);
      T(gTrace, printf("%s:%s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP01,lLogText);
   }
   else
   {
      T(gTrace, printf("%s: SSAP SP status disabled\n",gProcessName););
      gLog.GenerateLog(ACUSCCP01,"SSAP SP status disabled");
   }

   //This function enables the receipt of ACU_SCCP_MSG_USER_STATUS
   //messages for the specified pointcode and ssn.

   acu_sccp_enable_user_status(mSsapPtr, mRemAddr->sa_pc ? mRemAddr->sa_pc : ~0u,
         mRemAddr->sa_ssn ? mRemAddr->sa_ssn : ~0u);
   sprintf(lLogText,"SSAP user status recieve enabled remote(PC:%d SSN:%d)",
         mRemAddr->sa_pc, mRemAddr->sa_ssn);
   T(gTrace, printf("%s:%s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP12,lLogText);

   mSaapStatus.lastActTime = time(NULL);

   if(false == SsapConnect())
   {
      return false;
   }
   else
   {
      //mSaapStatus.status = ACTIVE; //CONNECT;
      mSaapStatus.state = IN_SERVICE;
      //setting transmitflag as true;
      AculabUtil::mTransmitFlag = true;
   }

      gLog.GenerateLog(ACUSCCP12, "SSAP Reconnect success");
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SendAcuSccpMsg
// DESCRIPTION : Sends ACULAB Sccp Message to stack
// PARAMETER   : acu_sccp_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::SendAcuSccpMsg(const void *data, unsigned int dataLen, UINT32 transId)
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_SCCP_ERR_TEXT_LEN + 1] = "";

   if (NULL == mConnection)
   {
      sprintf(lLogText,"TransactionId:%08x Transmitting UDT msg failed: Connection not ready (mConnection is NULL)", transId);
      TERR(gTrace,printf("%s:%s %s%s\n",gProcessName,RED,lLogText,UNDO););
      gLog.GenerateLog(ACUSCCP12,lLogText);
      return false;
   }

   int lRetVal = acu_sccp_unitdata_request(mConnection, data, dataLen);
   if(0 != lRetVal)
   {

      mAcuSccpUtil.ReturnAculabErrStr(lRetVal,lErrText);
      sprintf(lLogText,"TransactionId:%08x Transmitting UDT msg to stack Failed:%s", transId, lErrText);
      TERR(gTrace,printf("%s: %s",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP12,lLogText);
      return false;
   }
   else
   {
      sprintf(lLogText,"TransactionId:%08x Transmitting SCCP UDT msg to stack Sucess", transId);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP43,lLogText);
      return true;
   }
}



//-------------------------------------------------------------------------------
// METHOD      : GetAcuSccpEvent
// DESCRIPTION : Retrives SCCP messge events from aculab library
// PARAMETER   : acu_sccp_msg_t **(Double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::GetAcuSccpEvent(acu_sccp_msg_t **lAcuSccpMsgPtr)
{
   int   lRetVal = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Blocing read has to be modified for 1 second
   if(EXITING == mSaapStatus.state)
   {
      sprintf(lLogText,"Instance(0)Sccp Msg recieved from with msgType: %s But SSAP is in EXIT state",
            mAcuSccpUtil.GetAcuSccpMsgTypeStr((*lAcuSccpMsgPtr)->tm_msg_type));
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP29, lLogText);
      return false;
   }

   lRetVal = acu_sccp_ssap_msg_get(mSsapPtr, 500, lAcuSccpMsgPtr);

   //Update in shared meory the status of this ssap coonection
   mSaapStatus.lastActTime = time(NULL);
   if(0 == lRetVal)
   {
      sprintf(lLogText,"Instance(0)Sccp Msg recieved from with msgType: %d-%s",
            (*lAcuSccpMsgPtr)->tm_msg_type,
            mAcuSccpUtil.GetAcuSccpMsgTypeStr((*lAcuSccpMsgPtr)->tm_msg_type));
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP11, lLogText);
      return true;
   }
   else
   {
      return false;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateSsapStatus
// DESCRIPTION : Updates Ssap Status
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::UpdateSsapStatus()
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if(mSsapPtr == NULL)
      return true;

   acu_sccp_get_con_state(mSsapPtr, 0, &pConA_State);
   mSaapStatus.host_a_con_state = pConA_State->cs_state;
   acu_sccp_get_con_state(mSsapPtr, 1, &pConB_State);
   mSaapStatus.host_b_con_state = pConB_State->cs_state;

   sprintf(lLogText,"SSAP status for instance:0 is with host_a %d and with host_b %d",
         pConA_State->cs_state, pConB_State->cs_state);
   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUSCCP13, 2,  lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetSsapStatus
// DESCRIPTION : Retrieves Ssap Status
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::GetSsapStatus()
{
   time_t lTime     = time(NULL);
   time_t lDiffTime = lTime - mSaapStatus.lastActTime;
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(mSsapPtr == NULL)
      return true;

   // -----------------------------------------------------------------------
   // Ab Comment
   // Priority 1: Genuine TCP/hardware fault — trigger reconnect immediately.
   //
   // ACU_SCCP_CON_STATE_RX_BLOCKED or TX_BLOCKED while IN_SERVICE means the
   // Aculab driver's ring buffer is full or the underlying TCP link is stalled.
   // This is a real fault that will freeze all SS7 traffic if not cleared.
   // -----------------------------------------------------------------------
   bool lConABlocked =
      (mSaapStatus.host_a_con_state & ACU_SCCP_CON_STATE_IN_SERVICE) &&
      (mSaapStatus.host_a_con_state & ACU_SCCP_CON_STATE_RX_BLOCKED ||
       mSaapStatus.host_a_con_state & ACU_SCCP_CON_STATE_TX_BLOCKED);

   bool lConBBlocked =
      (mSaapStatus.host_b_con_state & ACU_SCCP_CON_STATE_IN_SERVICE) &&
      (mSaapStatus.host_b_con_state & ACU_SCCP_CON_STATE_RX_BLOCKED ||
       mSaapStatus.host_b_con_state & ACU_SCCP_CON_STATE_TX_BLOCKED);

   if(lConABlocked || lConBBlocked)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SSAP Connection BLOCKED: host_a=0x%X host_b=0x%X Triggering reconnect",
            mSaapStatus.host_a_con_state, mSaapStatus.host_b_con_state);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP13, lLogText);
      mSaapStatus.state = EXITING;
      return false;
   }

   // -----------------------------------------------------------------------
   // Ab Comment
   // Priority 2: Extended silence watchdog (30s).
   //
   // No message received for 30 seconds is unusual but NOT necessarily a fault
   // (low-traffic off-peak periods are common). Only trigger reconnect if the
   // SSAP connection is also NOT in-service — i.e. the driver has gone away.
   // Previously this was 10s which caused false reconnects on quiet periods.
   // -----------------------------------------------------------------------
   if(lDiffTime >= 30)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SSAP silence for %ld s: host_a=0x%X host_b=0x%X",
            (long)lDiffTime,
            mSaapStatus.host_a_con_state, mSaapStatus.host_b_con_state);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP13, 2, lLogText);

      // Only reconnect if neither connection is in-service.
      // If at least one is in-service, the link is alive — just quiet.
      bool lConAInService = (mSaapStatus.host_a_con_state & ACU_SCCP_CON_STATE_IN_SERVICE) != 0;
      bool lConBInService = (mSaapStatus.host_b_con_state & ACU_SCCP_CON_STATE_IN_SERVICE) != 0;

      if(!lConAInService && !lConBInService)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "SSAP not in-service after %ld s silence. Triggering reconnect",
               (long)lDiffTime);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP13, lLogText);
         mSaapStatus.state = EXITING;
         return false;
      }
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : HandleEventAcuSccpMsg
// DESCRIPTION : Returns the ACU SCCP MESSAGE type recieved from stack
// PARAMETER   : acu_sccp_msg_t *
// RETURN      : acu_sccp_msg_type_t
//-------------------------------------------------------------------------------
acu_sccp_msg_type_t SccpAculab::HandleEventAcuSccpMsg(acu_sccp_msg_t *lAcuSccpMsgPtr)
{

   switch(lAcuSccpMsgPtr->tm_msg_type)
   {
      case ACU_SCCP_MSG_UNITDATA:
         {
            return ACU_SCCP_MSG_UNITDATA;
         }
      case ACU_SCCP_MSG_NOTICE:
         {
            return ACU_SCCP_MSG_NOTICE;
         }
      case ACU_SCCP_MSG_TIMEOUT:
         {
            return ACU_SCCP_MSG_TIMEOUT;
         }
      case ACU_SCCP_MSG_CON_STATE:
         {
            return ACU_SCCP_MSG_CON_STATE;
         }
      case ACU_SCCP_MSG_USER_STATUS:
         {
            return ACU_SCCP_MSG_USER_STATUS;
         }
      case ACU_SCCP_MSG_SP_STATUS:
         {
            return ACU_SCCP_MSG_SP_STATUS;
         }
      case ACU_SCCP_MSG_DATA_IND:
         {
            return ACU_SCCP_MSG_DATA_IND;
         }
      case ACU_SCCP_MSG_CONNECT_IND:
         {
            return ACU_SCCP_MSG_CONNECT_IND;
         }
      case ACU_SCCP_MSG_CONNECT_CONF:
         {
            return ACU_SCCP_MSG_CONNECT_CONF;
         }
      case ACU_SCCP_MSG_CONREF_CONF:
         {
            return ACU_SCCP_MSG_CONREF_CONF;
         }
      case ACU_SCCP_MSG_DISCON_IND:
         {
            return ACU_SCCP_MSG_DISCON_IND;
         }
      case ACU_SCCP_MSG_DISCON_CONF:
         {
            return ACU_SCCP_MSG_DISCON_CONF;
         }
      case ACU_SCCP_MSG_ERROR_IND:
         {
            return ACU_SCCP_MSG_ERROR_IND;
         }
      case ACU_SCCP_MSG_INACTIVITY_IND:
         {
            return ACU_SCCP_MSG_INACTIVITY_IND;
         }

      default:
         TERR(gTrace, printf("%s: %s Unexpected Message Type:0x%x %s\n",
               gProcessName,RED,lAcuSccpMsgPtr->tm_msg_type,UNDO););
         return lAcuSccpMsgPtr->tm_msg_type;
         break;
   }

}

//-------------------------------------------------------------------------------
// METHOD      : GetAcuSccpConState
// DESCRIPTION :
// PARAMETER   : SccpMsg, acu_sccp_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculab::GetAcuSccpConState(acu_sccp_msg_t *lMsg)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   acu_sccp_msg_get_con_state(lMsg, &pConA_State, &pConB_State);

   TEXT lConStr[200] = "";

   if(true == mAcuSccpUtil.PrintConState(lConStr,pConA_State))
   {
      acu_sccp_get_con_state(mSsapPtr,0,&pConA_State);
      mAcuSccpUtil.PrintConState(lConStr,pConA_State);
      gTrace.PrintTraceTimeStamp("Y");
      T(gTrace, printf("%s: Instance(0) current state:%s \n",
            gProcessName,lConStr););

   }
   if(true == mAcuSccpUtil.PrintConState(lConStr,pConB_State))
   {
      acu_sccp_get_con_state(mSsapPtr,1,&pConB_State);

      mAcuSccpUtil.PrintConState(lConStr,pConB_State);
      gTrace.PrintTraceTimeStamp("Y");
      T(gTrace, printf("%s: Instance(0) current state:%s \n",
            gProcessName,lConStr););
   }

   if (NULL == mConnection) {
      mConnection = acu_sccp_ssap_get_unitdata_con(mSsapPtr);
      if(NULL != mConnection)
      {
         // Ensure default QoS Priorities for ANSI SCCP
         acu_sccp_con_set_cfg_int(mConnection, ACU_SCCP_CFG_QOS_PRIORITY, 0);
         acu_sccp_con_set_cfg_int(mConnection, ACU_SCCP_CFG_QOS_RESPONSE_PRI, 1);
      }
      else
      {
         T(gTrace, printf("%s:%s %s%s\n",gProcessName,RED,"SCCP connection area on Ssap creation Failed...",UNDO););
         gLog.GenerateLog(ACUSCCP02,"SCCP connection area on Ssap creation Failed...");
         return false;
      }
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetRemoteLocalAddress
// DESCRIPTION : Sets the Received Lcoal and remote Address
// PARAMETER   : acu_sccp_addr_t
// RETURN      : BOOLEAN
//-----------------------------------------------------------------------------
BOOLEAN SccpAculab::SetRemoteLocalAddress(acu_sccp_addr_t lLocAddr, acu_sccp_addr_t lRemAddr)
{
   if (NULL == mConnection)
   {
      T(gTrace, printf("%s:%s SetRemoteLocalAddress failed: mConnection is NULL%s\n",gProcessName,RED,UNDO););
      return false;
   }
   memcpy(acu_sccp_con_get_locaddr(mConnection), &lLocAddr, sizeof(acu_sccp_addr_t));
   memcpy(acu_sccp_con_get_remaddr(mConnection), &lRemAddr, sizeof(acu_sccp_addr_t));

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetReturnOption
// DESCRIPTION : Sets the Return Option QoS parameter on the connection
// PARAMETER   : bool enable
// RETURN      : BOOLEAN
//-----------------------------------------------------------------------------
BOOLEAN SccpAculab::SetReturnOption(bool enable)
{
   if (NULL == mConnection)
   {
      T(gTrace, printf("%s:%s SetReturnOption failed: mConnection is NULL%s\n",gProcessName,RED,UNDO););
      return false;
   }
   acu_sccp_con_set_cfg_int(mConnection, ACU_SCCP_CFG_QOS_RET_OPT, enable ? 1 : 0);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : UnblockConnection
// DESCRIPTION : Unblocks Connection
// PARAMETER   : acu_sccp_con_t*
// RETURN      : BOOLEAN
//-----------------------------------------------------------------------------
BOOLEAN SccpAculab::UnblockConnection(acu_sccp_con_t *con)
{
   acu_sccp_con_unblock(con);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetSccpStatus
// DESCRIPTION : Get Destination SP status
// PARAMETER   :
// RETURN      : BOOLEAN
//-----------------------------------------------------------------------------
BOOLEAN SccpAculab::GetSccpStatus(INT16 dest, BOOLEAN &status)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   const acu_sccp_sccp_status_t *lSccp_status;

   if(0 == acu_sccp_get_sccp_status(mSsapPtr, dest, 0, &lSccp_status))
   {
      sprintf(lLogText,"SCCP Destination acu_sccp_get_sccp_status Success %d", dest);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP25, lLogText);

      // Ab Comment : Safely check status ONLY if API call succeeded
      if(ACU_SCCP_REM_SCCP_AVAIL == lSccp_status->tsp_sccp_status)
      {
         sprintf(lLogText,"SCCP Destination %d Available", dest);
         T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP27, lLogText);
         status = true;
      }
      else
      {
         sprintf(lLogText,"SCCP Destination %d Not Available", dest);
         T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP28, lLogText);
         status = false;
      }
   }
   else
   {
      sprintf(lLogText,"SCCP Destination acu_sccp_get_sccp_status Failed %d", dest);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP26, lLogText);
      status = false;
   }

   return true;
}

