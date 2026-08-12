static const char *id = "@(#) $Id: TcapAculabApi.cc,v 1.1.4.11.2.2 2021/12/01 12:43:26 jamesjac Exp $";
//----------------------------------------------------------------------------
// NAME: TcapAculabApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
// Originated : 23-JAN-2010                                           Paresh
//----------------------------------------------------------------------------

#include "TcapAculabApi.h"

extern SsapDetailsStruct gSsapDetails;
extern BOOLEAN gRestoreStatus;
pthread_mutex_t  gMutexLock = PTHREAD_MUTEX_INITIALIZER;

//-------------------------------------------------------------------------------
// METHOD      : TcapAculab 
// DESCRIPTION : Constructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
TcapAculab::TcapAculab()
{
   mDialigueComponent.clear();   
   memset(&mSaapStatus, 0, sizeof(mSaapStatus));
   for(UINT8 lTemp = 0; lTemp < MAX_ACU_TCAP_INSTANCES; lTemp++)
   {
      mSsapPtr[lTemp] = NULL;
   }

   mHostAIP[0] = '\0';
   mHostBIP[0] = '\0';
   mHostAUP = false;
   mHostBUP = false;
}

//-------------------------------------------------------------------------------
// METHOD      : ~TcapAculab
// DESCRIPTION : Destructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
TcapAculab::~TcapAculab()
{

}
//-------------------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializing Api
// PARAMETER   : TEXT *
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::Init(TEXT *lCfgFile, BOOLEAN lPegFlag, int lNoOfInstance, int lTransValidationKey, INT16 lMsgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   mPegFlag = lPegFlag;

   mNoOfInstance = lNoOfInstance;

   if(false == this->ReadConfig())
   {
      return false;
   }

   if(false == SsapCreate())
   {
      return false;
   }

   if(false == SsapCreateThreadPool())
   {
      return false;
   }

   if(false == mDlgManager.Init())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "AI: Dialogue Manager Initialization failed. Exiting...");
      T(gTrace,
            printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04,lLogText);
      return false;
   }

   if(false == mTransDlgMap.Init(lTransValidationKey, lMsgType))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "AI: Trans Dialogue Manager Initialization failed. Exiting...");
      T(gTrace,
            printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04,lLogText);
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
BOOLEAN TcapAculab::ReloadConfig()
{
   if(false == SsapCreate(true))
      return false;
}

//-------------------------------------------------------------------------------
// METHOD      : SsapCreateThreadPool 
// DESCRIPTION : Creates Threadpool 
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SsapCreateThreadPool()
{
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetTcapCinfigFilePath 
// DESCRIPTION : Sets TCAP Application config file path
// PARAMETER   : TEXT *lCfgFile
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetTcapCinfigFilePath(TEXT *lCfgFile)
{
   TEXT lProductHome[50 + 1] = "";
   TEXT lCfgPath[50 + 1] = "";

   strcpy(lProductHome ,getenv("PRODUCT_HOME"));
   if(NULL == lProductHome)
   {
      return false;
   }

   strcpy(lCfgPath, getenv("PRODUCT_CFG_PATH"));
   if(NULL == lCfgPath)
   {
      return false;
   }

   sprintf(mCfgFile,"%s%s/%s",lProductHome,lCfgPath,lCfgFile);
   T(gTrace,printf("%s: TCAP config file:%s\n",gProcessName,mCfgFile););

   return true;

}
//-------------------------------------------------------------------------------
// METHOD      : ReadConfig 
// DESCRIPTION : Reads config parameters
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::ReadConfig()
{
   if(false == this->ReadTcapConfig())
   {
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReadTcapConfig
// DESCRIPTION : Reads config params related to TCAP
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::ReadTcapConfig()
{
   TEXT lLogText  [MAX_LOG_TEXT_LEN + 1] = "";
   CfgRead        lCfgRead("ACULAB_TCAP_API");

   if (false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      T(gTrace, printf("%s: CfgRead object(%s) intialization failed...\n",
               gProcessName, SS7_KER_CFG););
      gLog.GenerateLog(GSYS09,
            "%s CfgRead object initializaton failed", SS7_KER_CFG);

      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigStr("TCAP_HOST_A_ALT_IP",
            mHostAIP,
            SS7_MIN_IP_LEN, SS7_MAX_IP_LEN))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_HOST_A_ALT_IP in %s",
            SS7_KER_CFG);
      T(gTrace,printf("%s: %s\n", lLogText,gProcessName););
   }
   else
   {
      T(gTrace,printf("%s: TCAP_HOST_A_ALT_IP = %s\n", gProcessName, mHostAIP););
   }

   if(CFG_OK != lCfgRead.GetConfigStr("TCAP_HOST_B_ALT_IP",
            mHostBIP,
            SS7_MIN_IP_LEN, SS7_MAX_IP_LEN))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_HOST_B_ALT_IP in %s",
            SS7_KER_CFG);
      T(gTrace,printf("%s: %s\n", lLogText,gProcessName););
   }
   else
   {
      T(gTrace,printf("%s: TCAP_HOST_B_ALT_IP = %s\n", gProcessName, mHostBIP););
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
BOOLEAN TcapAculab::SsapCreate(BOOLEAN reload)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lTraceTag[20] = "";
   SsapDetails lSsapDetails;
   map<UINT32, SsapDetails>::iterator lmSsapDetails = gSsapDetails.begin();
   static UINT8 lTemp;
   UINT32   lLocalPC;


   if(false == reload)
   {
      lTemp = 0;
   }
   else
   {
      lTemp = mNoOfInstance;
   }
   for(; lmSsapDetails != gSsapDetails.end(); lmSsapDetails++)
   {
      lSsapDetails = lmSsapDetails->second;
      lLocalPC = lmSsapDetails->first;
      if(false == SetTcapCinfigFilePath(lSsapDetails.configFile))
      {
         return false;
      }

      for(UINT8 i = 0; i < lSsapDetails.numOfInstances; i++, lTemp++)
      {
         if(ADDED_NEW != lSsapDetails.instance[i].status)
            continue;
	 
	 //ANSI - dynamic flag to support ansi | ITU
        // mSsapPtr[lTemp] = acu_tcap_ssap_create(mCfgFile, acu_tcap_ssap_flags_t(tcapFlag | ACU_TCAP_LOG_STDERR));
         
            mSsapPtr[lTemp] = acu_tcap_ssap_create(mCfgFile,
               acu_tcap_ssap_flags_t(SS7_STANDARD_ANSI | ACU_TCAP_LOG_STDERR));
         // ---> ADD THIS ENTIRE BLOCK HERE <---
             acu_tcap_ssap_set_cfg_int(mSsapPtr[lTemp], ACU_TCAP_CFG_ENC_DEF_LEN, 1);
             acu_tcap_ssap_set_cfg_int(mSsapPtr[lTemp], ACU_TCAP_CFG_QOS_RET_OPT, 1);

         //UPTO ANSI - 
         
         if(NULL == mSsapPtr[lTemp])
         {
            sprintf(lLogText,"AI: Config File %s SSAP creation failed",mCfgFile);
            printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO);
            gSsapDetails.erase(lLocalPC);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP01,lLogText);
            return false;
         }

         sprintf(lLogText,"AI: InstanceIdx:%d  SsapPtrIdx:%d NumOfInstances:%d ConfigFile:%s SSAP Creation Success,",
               i, lTemp, lSsapDetails.numOfInstances,  mCfgFile);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP81,lLogText);

         lSsapDetails.instance[i].instanceId = lTemp;

         mLocAddr = acu_tcap_ssap_get_locaddr(mSsapPtr[lTemp]);
         sprintf(lTraceTag, "tcap_%d_%d", mLocAddr->sa_pc, i);

         if(lLocalPC != mLocAddr->sa_pc)
         {
            sprintf(lLogText,"AI: SSAP creation failed, OPC in kernel.cfg is %d and OPC in file %s is %d",
                  lLocalPC, mCfgFile, mLocAddr->sa_pc);
            printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP01,lLogText);
            acu_tcap_ssap_delete(mSsapPtr[lTemp]);
            return false;
         }

         acu_tcap_ssap_set_cfg_str(mSsapPtr[lTemp], ACU_TCAP_CFG_TRACE_TAG,lTraceTag);

         if(lSsapDetails.transId > 0 && lSsapDetails.transId <= 4094)
         {
            mSaapStatus[lTemp].transIdRange = lSsapDetails.transId;
            sprintf(lLogText,"AI: SsapDetTransId:%d lTemp:%d SSAP Set Cfg Init", lSsapDetails.transId, lTemp);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP82,lLogText);
            acu_tcap_ssap_set_cfg_int(mSsapPtr[lTemp], ACU_TCAP_CFG_TRANID_RANGE, lSsapDetails.transId++);
         }
         else if(lSsapDetails.transId > 4094)
         {
            //for(int lInstanceNo = 0; lInstanceNo < lTemp; lInstanceNo++)
            acu_tcap_ssap_delete(mSsapPtr[lTemp]);

            return false;
         }

         //Getting remote node adress details
         mRemAddr = acu_tcap_ssap_get_remaddr(mSsapPtr[lTemp]);

         if (mRemAddr->sa_pc != 0)
         {
            acu_tcap_enable_sp_status(mSsapPtr[lTemp], mRemAddr->sa_pc);
            sprintf(lLogText,"AI: SSAP SP status enabled Instance:%d  RemotePC: %d", lTemp, mRemAddr->sa_pc);
            printf("%s:%s %s %s\n",gProcessName,BLUE,lLogText,UNDO);
            gLog.GenerateLog(ACUTCAP139,lLogText);
         }
         else
         {
            printf("%s:%s AI: SSAP SP status Instance:%d disabled%s\n",gProcessName,RED,lTemp,UNDO);
            sprintf(lLogText,"AI: SSAP SP status disabled Instance:%d  RemotePC: %d", lTemp, mRemAddr->sa_pc);
            gLog.GenerateLog(ACUTCAP140,lLogText);
         }

         //This function enables the receipt of ACU_TCAP_MSG_USER_STATUS 
         //messages for the specified pointcode and ssn.

         acu_tcap_enable_user_status(mSsapPtr[lTemp], mRemAddr->sa_pc ? mRemAddr->sa_pc : ~0u,
               mRemAddr->sa_ssn ? mRemAddr->sa_ssn : ~0u);
         sprintf(lLogText,"AI: SSAP user status recieve enabled remote(PC:%d SSN:%d)",
               mRemAddr->sa_pc, mRemAddr->sa_ssn);
         printf("%s:%s %s%s\n",gProcessName,BLUE,lLogText,UNDO);

         mSaapStatus[lTemp].lastActTime = time(NULL);

         if(false == SsapConnect(lTemp))
         {
            return false;
         }
         else
         {
            lSsapDetails.instance[i].status = CONNECT;

            // Storing TRANID_RANGE for Restoration process 
            // -------------------------------------------
            // -------------------------------------------
            if(lSsapDetails.transId == 0)
            {
               acu_tcap_trans_t  *lTransPtr=NULL;
               acu_tcap_msg_t lMsg; 
               acu_tcap_msg_t *lMsgPtr = &lMsg;
               unsigned int lLocId;
               unsigned int lRemId;
               unsigned int lRemIdLen;
               unsigned int lTempLocId;
               int lInstance = lTemp;


               sleep(1);
               if(false == this->CreateAcuTcapTrans(&lTransPtr,lInstance))
               {
                  sprintf(lLogText,"AI: TransPtr:%08X Instance:%d Transaction creation failed", lTransPtr, lInstance);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP85, lLogText);
                  // retuen ?????
               }
               else
               {
                  if (false == this->AllocAcuTcapMsg(&lMsgPtr, lTransPtr))
                  {
                     sprintf(lLogText,"AI: TrsnPtr:%08X Message allocation failed for Restoration Purpose", lTransPtr);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP141,lLogText);
                     // retuen ?????
                  }
                  sprintf(lLogText,"AI: TrsnPtr:%08X Instance:%d Message allocation success for Restoration Purpose", 
                        lTransPtr, lInstance);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP86,lLogText);

                  this->GetTransIds(lMsgPtr, &lLocId, &lRemId, &lRemIdLen);

                  lTempLocId = lLocId >> 20;
                  mSaapStatus[lTemp].transIdRange = lTempLocId;

                  sprintf(lLogText,"AI: lTId:%08X rTId:%08X DestTidLen:%d lTemp:%d TransIdRange:%d GetTransIds", 
                        lLocId, lRemId, lRemIdLen, lTemp, lTempLocId);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP87,lLogText);

                  acu_tcap_msg_free(lMsgPtr);
                  this->DeleteAcuTcapTrans(lTransPtr);
               }
            }
            // -------------------------------------------
            // -------------------------------------------
            // -------------------------------------------


         }
      }        
      (*lmSsapDetails).second = lSsapDetails;
   }

   mNoOfInstance = lTemp;
   sprintf(lLogText,"AI: SSAP Create Successful mNoOfInstance:%d", mNoOfInstance);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP83,lLogText);
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : SsapConnect 
// DESCRIPTION : Application uses this api to connect to SSAP
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SsapConnect(UINT8 instance)
{
   int   lRetVal = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //This function causes the TCAP library to try to establish a TCP/IP 
   //connection between the ssap and the SCCP driver code.

   if(mSsapPtr[instance] == NULL)
      return false;

   lRetVal = acu_tcap_ssap_connect_sccp(mSsapPtr[instance]);

   if(0 != lRetVal)
   {
      sprintf(lLogText,"AI: Instance:%d SSAP connected to SCCP driver code failed", instance);
      printf("%s:%s %s%s\n",gProcessName,RED,lLogText,UNDO);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP02,lLogText);
      return false;
   }

   sprintf(lLogText,"AI: Instance:%d SSAP connected to SCCP driver code success", instance);
   printf("%s:%s %s%s\n",gProcessName,BLUE,lLogText,UNDO);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP84,lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SsapReConnect( 
// DESCRIPTION : Disconnects the coonection with SSAP
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SsapReConnect(int &lInstanceNo, INT16 &rMsgType, UINT32 &rDlgTimeout, UINT32 &rCapDlgTimeout, UINT32 &rMaxDlgSize)
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lTraceTag[10] = "";
   SsapDetails lSsapDetails;
   SsapDetailsStructIterator lmSsapDetails;
   UINT8  lTempInstance = 0;
   //This function deletes a SCCP access point, and any 
   //TCAP transactions created on it.
   
   //
   sprintf(lLogText,"AI: MsgType:%d DlgTimeout:%d CapDlgTimeout:%d MaxDlgSize:%d RestoreStatus:%d SSAP Reconnect", 
         rMsgType, rDlgTimeout, rCapDlgTimeout, rMaxDlgSize, gRestoreStatus);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP142,lLogText);

   mLocAddr = acu_tcap_ssap_get_locaddr(mSsapPtr[lInstanceNo]);
   sprintf(lLogText,"AI: SSAP Get Local Address mLocAddr:%d", mLocAddr->sa_pc);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP143,lLogText);

   if(gSsapDetails.end() != (lmSsapDetails = gSsapDetails.find(mLocAddr->sa_pc)))
   {
      lSsapDetails = lmSsapDetails->second;

      for(UINT8 i = 0; i < lSsapDetails.numOfInstances; i++)
      {
         if(lInstanceNo == lSsapDetails.instance[i].instanceId)
         {
            lSsapDetails.instance[i].status = DELETE;
            lTempInstance = i;
         }
      }
   }
   sprintf(lLogText,"AI: Instance:%d SsapTransId:%d SSAP Before Ssap Delete", lInstanceNo, lSsapDetails.transId);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP144,lLogText);

   //setting transmitflag as false;
   AculabUtil::mTransmitFlag[lInstanceNo] = false;
   acu_tcap_ssap_delete(mSsapPtr[lInstanceNo]);
   sprintf(lLogText,"AI: Instance:%d SsapTransId:%d SSAP After ssap delete", lInstanceNo, lSsapDetails.transId);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP145,lLogText);

   mSsapPtr[lInstanceNo] = NULL;

   if(false == SetTcapCinfigFilePath(lSsapDetails.configFile))
   {
      return false;
   }
   // ANSI - support dynamic flag to support ansi | ITU
   mSsapPtr[lInstanceNo] = acu_tcap_ssap_create(mCfgFile, 
         acu_tcap_ssap_flags_t(SS7_STANDARD_ANSI | ACU_TCAP_LOG_STDERR));
   // ---> ADD THIS ENTIRE BLOCK HERE <---
       acu_tcap_ssap_set_cfg_int(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_ENC_DEF_LEN, 1);
       acu_tcap_ssap_set_cfg_int(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_QOS_RET_OPT, 1);
   //UPTO ANSI -
   if(NULL == mSsapPtr[lInstanceNo])
   {
      sprintf(lLogText,"AI: SSAP creation failed, config file %s",mCfgFile);
      printf("%s:%s %s %s\n",gProcessName,RED,lLogText,UNDO);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP146,lLogText);
      return false;
   }

   sprintf(lTraceTag, "tcap_%d_%d", mLocAddr->sa_pc, lTempInstance);
   acu_tcap_ssap_set_cfg_str(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_TRACE_TAG,lTraceTag);

   /* if(lSsapDetails.transId > 0 && lSsapDetails.transId <= 4094)
      {
      mSaapStatus[lInstanceNo].transIdRange = lSsapDetails.transId;
      acu_tcap_ssap_set_cfg_int(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_TRANID_RANGE, lSsapDetails.transId);
      sprintf(lLogText,"TRANSID_RANGE set from Tcap cfg Instance:%d SsapTransId:%d", lInstanceNo, lSsapDetails.transId);
      gLog.GenerateLog(ACUTCAP88,lLogText);
      }
      else*/ if(mSaapStatus[lInstanceNo].transIdRange > 0 && mSaapStatus[lInstanceNo].transIdRange <= 4094)
   {
      acu_tcap_ssap_set_cfg_int(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_TRANID_RANGE, mSaapStatus[lInstanceNo].transIdRange);
      sprintf(lLogText,"AI: TRANSID_RANGE set for Restoration Instance:%d SsapTransId:%d", lInstanceNo, mSaapStatus[lInstanceNo].transIdRange);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP89,lLogText);
   }
   else if(lSsapDetails.transId > 4094)
   {
      //for(int lInstanceNo = 0; lInstanceNo < lTemp; lInstanceNo++)
      acu_tcap_ssap_delete(mSsapPtr[lInstanceNo]);

      return false;
   }
   mRemAddr = acu_tcap_ssap_get_remaddr(mSsapPtr[lInstanceNo]);

   if (mRemAddr->sa_pc != 0)
   {
      acu_tcap_enable_sp_status(mSsapPtr[lInstanceNo], mRemAddr->sa_pc);
      sprintf(lLogText,"AI: SSAP SP status enabled  remote PC: %d",
            mRemAddr->sa_pc);
      printf("%s:%s %s %s\n",gProcessName,BLUE,lLogText,UNDO);

   }
   else
   {
      printf("%s:%s AI: SSAP SP status disabled%s\n",gProcessName,RED,UNDO);
   }

   //This function enables the receipt of ACU_TCAP_MSG_USER_STATUS 
   //messages for the specified pointcode and ssn.

   acu_tcap_enable_user_status(mSsapPtr[lInstanceNo], mRemAddr->sa_pc ? mRemAddr->sa_pc : ~0u,
         mRemAddr->sa_ssn ? mRemAddr->sa_ssn : ~0u);
   sprintf(lLogText,"AI: SSAP user status recieve enabled remote(PC:%d SSN:%d)",
         mRemAddr->sa_pc, mRemAddr->sa_ssn);
   printf("%s:%s %s%s\n",gProcessName,BLUE,lLogText,UNDO);

   mSaapStatus[lInstanceNo].lastActTime = time(NULL);

   if(false == SsapConnect(lInstanceNo))
   {
      return false;
   }
   else
   {
      // SSAP Reconnect Restoration Added
      // ------------------------------------------------------------------------ 
      // ------------------------------------------------------------------------ 
      // ------------------------------------------------------------------------
      sleep(1); // Wait for SSAP to come to Service
      sprintf(lLogText,"AI: Instance:%d SsapTransId:%d RestoreStatus:%d SSAP Reconnect Restoration Started", 
            lInstanceNo, mSaapStatus[lInstanceNo].transIdRange, gRestoreStatus);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP147,lLogText);

      DlgRecord   lDlgRcd;
      UINT32      lDlgTimeout;
      acu_tcap_trans_t  *lTransPtr=NULL;
      time_t lInsertTime = 0;
      time_t lCurrTime = 0;

      if(146 != rMsgType)
      {
         lDlgTimeout = rDlgTimeout;
      }
      else
      {
         lDlgTimeout = rCapDlgTimeout;
      }

      for(unsigned int i = 1; i <= rMaxDlgSize; i++)
      {
         lDlgRcd.dlgId = i;
         mDlgManager.GetAllDlgInfo(lDlgRcd);

         lCurrTime = time(NULL) - lDlgRcd.insertTime;

         if(( lCurrTime  < lDlgTimeout) && (lDlgRcd.insertTime != 0) && (lInstanceNo == lDlgRcd.ssapInstance) )
         {
            //sprintf(lLogText,"Transaction Details DlgId:%d InvokeId:%d CurrTime:%ld InsertTime:%ld Ssn:%d LocalTid:%08X LocalTidLen:%d DestTid:%08X DestTidLen:%d DlgType:%d Restarted:%d Opcode:%d SsapInstance:%d", lDlgRcd.dlgId, lDlgRcd.invokeId, lCurrTime, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.origTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransId, lDlgRcd.destTransIdLen, lDlgRcd.dlgType, lDlgRcd.restarted, lDlgRcd.opCode, lDlgRcd.ssapInstance);
            sprintf(lLogText,"AI: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Restore Details", lDlgRcd.dlgId, lDlgRcd.trans, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP53, 1, lLogText);

            sprintf(lLogText,"AI: DlgId:%d Calling Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d",
                  lDlgRcd.dlgId, lDlgRcd.callingAddr.sa_gt.sag_num, lDlgRcd.callingAddr.sa_tt, lDlgRcd.callingAddr.sa_np, lDlgRcd.callingAddr.sa_es, lDlgRcd.callingAddr.sa_nai);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP54, 1, lLogText);

            sprintf(lLogText,"AI: DlgId:%d Called Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d",
                  lDlgRcd.dlgId, lDlgRcd.calledAddr.sa_gt.sag_num, lDlgRcd.calledAddr.sa_tt, lDlgRcd.calledAddr.sa_np, lDlgRcd.calledAddr.sa_es, lDlgRcd.calledAddr.sa_nai);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP55, 1, lLogText);

            if(true == this->CreateAcuTcapOngoingTrans(&lTransPtr, lInstanceNo, lDlgRcd.origTransIdLen, lDlgRcd.origTransId, lDlgRcd.destTransIdLen, lDlgRcd.destTransId, lDlgRcd.ssn, lDlgRcd.opCode, lDlgRcd.invokeId, lCurrTime, lDlgRcd.ssapInstance))
            {
               *acu_tcap_trans_get_locaddr(lTransPtr) = lDlgRcd.callingAddr;
               *acu_tcap_trans_get_remaddr(lTransPtr) = lDlgRcd.calledAddr;

               lDlgRcd.trans = lTransPtr;
               lDlgRcd.restarted =  true;

               if(false == mTransDlgMap.MapDlgIdtoTransId(lDlgRcd))
               {
                  sprintf(lLogText,"AI: DlgId:%d TransPtr:%08X Mapping DialogId to Transaction Map Failed", i, lTransPtr);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP56, 1, lLogText);
               }
               sprintf(lLogText,"AI: DlgId:%d TransPtr:%08X Transaction Restore Success", i, lTransPtr);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP57, 1, lLogText);

               if(lDlgRcd.dlgType == TCAP_BEGIN)
               {
                  int lRetVal = acu_tcap_transaction_restore_app_ctx(lTransPtr,
                        lDlgRcd.applicationContext.array,
                        lDlgRcd.applicationContext.numberOfBytes);

                  for (int i = 0; i < lDlgRcd.applicationContext.numberOfBytes;)  // TBR
                  {
                     printf("%02X ", lDlgRcd.applicationContext.array[i]);
                     i++;
                  }
                  printf("\n");

                  if(0 != lRetVal)
                  {
                     TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
                     mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                     sprintf(lLogText,"%s",lErrText);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  }
                  else
                  {
                     sprintf(lLogText,"AI: NumberOfBytes:%d ApplicationContext:%s Setting Application context Success", 
                           lDlgRcd.applicationContext.numberOfBytes, lDlgRcd.applicationContext.array);
                     T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP58, 1, lLogText);
                  }
               }
            }
            else
            {
               sprintf(lLogText,"AI: DlgId:%d RestoreStatus:%d Transaction Restore Failed", i, gRestoreStatus);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP59, 1, lLogText);
               //return false;
            }
         }
      }


      // ------------------------------------------------------------------------ 
      // ------------------------------------------------------------------------ 
      // ------------------------------------------------------------------------ 

      gRestoreStatus = true;
      sleep(1);
      lSsapDetails.instance[lTempInstance].status = ACTIVE; //CONNECT;
      mSaapStatus[lInstanceNo].state = IN_SERVICE;
      //setting transmitflag as true;
      AculabUtil::mTransmitFlag[lInstanceNo] = true;
   }
   (*lmSsapDetails).second = lSsapDetails;

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SendAcuTcapMsg 
// DESCRIPTION : Sends ACULAB Tcap Message to stack
// PARAMETER   : acu_tcap_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SendAcuTcapMsg(acu_tcap_msg_t *lAcuTcapMsgPtr)
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";

   int lRetVal = acu_tcap_msg_send(lAcuTcapMsgPtr);
   if(0 != lRetVal) 
   {
      mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);

      sprintf(lLogText,"AI: Transmitting msg to stack failed:%s",lErrText);
      TERR(gTrace,printf("%s: %s",gProcessName,lLogText);); 
      gLog.GenerateLog(ACUTCAP30,lLogText);
      return false; 
   }
   else
   {
      sprintf(lLogText,"AI: Transmitting msg to stack Success:%s",lErrText);
      T(gTrace,printf("%s: %s",gProcessName,lLogText);); 
      gLog.GenerateLog(ACUTCAP135, 3, lLogText);
      return true;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : GetAcuTcapEvent 
// DESCRIPTION : Retrives TCAP messge events from aculab library
// PARAMETER   : acu_tcap_msg_t **(Double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetAcuTcapEvent(acu_tcap_msg_t **lAcuTcapMsgPtr, int &lInstanceNo)
{
   int   lRetVal = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Blocing read has to be modified for 1 second
   if(EXITING == mSaapStatus[lInstanceNo].state)
      return false;

   lRetVal = acu_tcap_ssap_msg_get(mSsapPtr[lInstanceNo], 500, lAcuTcapMsgPtr);

   //Update in shared meory the status of this ssap coonection
   mSaapStatus[lInstanceNo].lastActTime = time(NULL);
   if(0 == lRetVal)
   {
      sprintf(lLogText,"AI: Instance(%d)Tcap Msg recieved from with msgType: %s", 
            lInstanceNo, mAcuTcapUtil.GetAcuTcapMsgTypeStr((*lAcuTcapMsgPtr)->tm_msg_type));
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP99, lLogText);

      return true;
   }
   else
   {
      return false;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateSsapStatus 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::UpdateSsapStatus(const int lInstanceNo)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if(lInstanceNo >= MAX_ACU_TCAP_INSTANCES || mSsapPtr[lInstanceNo] == NULL)
      return true;;

   acu_tcap_get_con_state(mSsapPtr[lInstanceNo], 0, &pConA_State);
   mSaapStatus[lInstanceNo].host_a_con_state = pConA_State->cs_state;
   acu_tcap_get_con_state(mSsapPtr[lInstanceNo], 1, &pConB_State);
   mSaapStatus[lInstanceNo].host_b_con_state = pConB_State->cs_state;

   sprintf(lLogText,"AI: SSAP status for instance:%d is with host_a %d and with host_b %d", lInstanceNo, 
         pConA_State->cs_state, pConB_State->cs_state);
   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
   gLog.GenerateLog(ACUTCAP148, 2,  lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetSsapStatus 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetSsapStatus(const int lInstanceNo)
{
   time_t lTime = time(NULL);
   double lDiffTime = difftime(lTime, mSaapStatus[lInstanceNo].lastActTime);  
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   sprintf(lLogText,"AI: Get SsapStatus CurrentTime:%ld LastActiveTime:%ld DiffTime:%.2f lInstanceNo:%d",
         lTime, mSaapStatus[lInstanceNo].lastActTime, lDiffTime, lInstanceNo);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
   gLog.GenerateLog(ACUTCAP32, 2, lLogText);

   if(lInstanceNo > MAX_ACU_TCAP_INSTANCES || mSsapPtr[lInstanceNo] == NULL)
      return true;

   if('\0' == mHostAIP[0] && '\0' == mHostBIP[0])
   {
      if((lDiffTime >= 15.0) || 
            (mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_IN_SERVICE && 
             (mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_RX_BLOCKED ||
              mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_TX_BLOCKED)) ||
            (mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_IN_SERVICE &&
             (mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_RX_BLOCKED ||
              mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_TX_BLOCKED)))
      {
         sprintf(lLogText,"AI: SSAP Connection status for instance:%d is with host_a %d cs_failure:%d cs_fail_text:%s lDiffTime:%.2f", lInstanceNo, mSaapStatus[lInstanceNo].host_a_con_state, pConA_State->cs_failure, pConA_State->cs_fail_text, lDiffTime);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
         gLog.GenerateLog(ACUTCAP149, lLogText);

         sprintf(lLogText,"AI: SSAP Connection status for instance:%d is with host_b %d cs_failure:%d cs_fail_text:%s lDiffTime:%.2f", lInstanceNo, mSaapStatus[lInstanceNo].host_b_con_state, pConB_State->cs_failure, pConB_State->cs_fail_text, lDiffTime);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
         gLog.GenerateLog(ACUTCAP149, lLogText);

         mSaapStatus[lInstanceNo].state = EXITING;
         gRestoreStatus = false;
         return false;
      }
   }
   else
   {
      if((lDiffTime >= 10.0) ||
            (mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_IN_SERVICE  &&
             (mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_RX_BLOCKED  ||
              mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_STATE_TX_BLOCKED)) ||
            mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_FAIL_CON_TIMEOUT  ||
            mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_FAIL_CON_REJECTED ||
            mSaapStatus[lInstanceNo].host_a_con_state & ACU_TCAP_CON_FAIL_LOGIN_REJECTED)
      {
         sprintf(lLogText,"AI: SSAP Connection status for instance:%d is with host_a %d cs_failure:%d cs_fail_text:%s", 
               lInstanceNo, mSaapStatus[lInstanceNo].host_a_con_state, pConA_State->cs_failure, pConA_State->cs_fail_text);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
         gLog.GenerateLog(ACUTCAP149, lLogText);

         if('\0' != mHostAIP[0])
         {
            if (0 != acu_tcap_ssap_set_cfg_str(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_HOST_A_NAME, mHostAIP))
            {
               sprintf(lLogText,"AI: Host ACU_TCAP_CFG_HOST_A_NAME Configuration Setting Failed:%s", mHostAIP);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP38, lLogText);

               if (false == mHostBUP)
               {
                  mSaapStatus[lInstanceNo].state = EXITING;

                  sprintf(lLogText,"AI: Shutting down TCAP, as HOST B also down :%d", mHostBUP);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP39, lLogText);
                  return false;
               }
            }
            sprintf(lLogText,"AI: Host ACU_TCAP_CFG_HOST_A_NAME Configuration Setting Success:%s", mHostAIP);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP40, lLogText);
         }
         else
         {
            if (false == mHostBUP)
            {
               mSaapStatus[lInstanceNo].state = EXITING;

               sprintf(lLogText,"AI: Shutting down TCAP, as HOST B also down and alternative IP not configured:%d", mHostBUP);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP41, lLogText);
               return false;
            }
         }
      }
      else
      {
         mHostAUP = true;
      }

      if((lDiffTime >= 10.0) ||
            (mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_IN_SERVICE  &&
             (mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_RX_BLOCKED  ||
              mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_STATE_TX_BLOCKED)) ||
            mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_FAIL_CON_TIMEOUT  ||
            mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_FAIL_CON_REJECTED ||
            mSaapStatus[lInstanceNo].host_b_con_state & ACU_TCAP_CON_FAIL_LOGIN_REJECTED)
      {
         sprintf(lLogText,"AI: SSAP Connection status for instance:%d is with host_b %d cs_failure:%d cs_fail_text:%s", 
               lInstanceNo, mSaapStatus[lInstanceNo].host_b_con_state, pConB_State->cs_failure, pConB_State->cs_fail_text);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText);); 
         gLog.GenerateLog(ACUTCAP149, lLogText);

         if('\0' != mHostBIP[0])
         {
            if (0 != acu_tcap_ssap_set_cfg_str(mSsapPtr[lInstanceNo], ACU_TCAP_CFG_HOST_B_NAME, mHostBIP))
            {
               sprintf(lLogText,"AI: Host ACU_TCAP_CFG_HOST_B_NAME Configuration Setting Failed:%s", mHostBIP);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP38, lLogText);

               if (false == mHostAUP)
               {
                  mSaapStatus[lInstanceNo].state = EXITING;

                  sprintf(lLogText,"AI: Shutting down TCAP, as HOST A also down :%d", mHostAUP);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP39, lLogText);
                  return false;
               }
            }
            sprintf(lLogText,"AI: Host ACU_TCAP_CFG_HOST_B_NAME Configuration Setting Success:%s", mHostBIP);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP40, lLogText);
         }
         else
         {
            if (false == mHostAUP)
            {
               mSaapStatus[lInstanceNo].state = EXITING;

               sprintf(lLogText,"AI: Shutting down TCAP, as HOST A also down and alternative IP not configured:%d", mHostAUP);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP41, lLogText);
               return false;
            }
         }
      }
      else
      {
         mHostBUP = true;
      }
   }
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : HandleEventAcuTcapMsg 
// DESCRIPTION : Returns the ACU TCAP MESSAGE type recieved from stack 
// PARAMETER   : acu_tcap_msg_t *
// RETURN      : acu_tcap_msg_type_t
//-------------------------------------------------------------------------------
acu_tcap_msg_type_t TcapAculab::HandleEventAcuTcapMsg(acu_tcap_msg_t *lAcuTcapMsgPtr)
{

   switch(lAcuTcapMsgPtr->tm_msg_type)
   {
      case ACU_TCAP_MSG_DATA:
         {
            return ACU_TCAP_MSG_DATA;
         }
      case ACU_TCAP_MSG_NOTICE:
         {
            return ACU_TCAP_MSG_NOTICE;
         }
      case ACU_TCAP_MSG_USER_STATUS:   
         {
            return ACU_TCAP_MSG_USER_STATUS;
         }
      case ACU_TCAP_MSG_SP_STATUS:   
         {
            return ACU_TCAP_MSG_SP_STATUS;
         }
      case ACU_TCAP_MSG_TIMEOUT:   
         {
            return ACU_TCAP_MSG_TIMEOUT;
         }
      case ACU_TCAP_MSG_CON_STATE:
         {
            return ACU_TCAP_MSG_CON_STATE;
         }
      default:
         printf("%s: %s Unexpected Message Type:0x%x %s\n",
               gProcessName,RED,lAcuTcapMsgPtr->tm_msg_type,UNDO);
         return lAcuTcapMsgPtr->tm_msg_type;
         break;
   }

}

//-------------------------------------------------------------------------------
// METHOD      : TrnslateMsgToAculabFormat
// DESCRIPTION : Translates Application Tcap Message to ACULAB Tcap
//               Message format
// PARAMETER   : TcapMsg, acu_tcap_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetAcuTcapConState(acu_tcap_msg_t *lMsg,int &lInstanceNo)
{

   acu_tcap_msg_get_con_state(lMsg, &pConA_State, &pConB_State);

   TEXT lConStr[200] = "";

   if(true == mAcuTcapUtil.PrintConState(lConStr,pConA_State,lInstanceNo, "ConA"))
   {
      acu_tcap_get_con_state(mSsapPtr[lInstanceNo],0,&pConA_State);
      mAcuTcapUtil.PrintConState(lConStr,pConA_State,lInstanceNo, "ConA");
      gTrace.PrintTraceTimeStamp("Y");
      printf("%s: Instance(%d) current state:%s \n",
            gProcessName,lInstanceNo,lConStr);

   }
   if(true == mAcuTcapUtil.PrintConState(lConStr,pConB_State,lInstanceNo, "ConB"))
   {
      acu_tcap_get_con_state(mSsapPtr[lInstanceNo],1,&pConB_State);

      mAcuTcapUtil.PrintConState(lConStr,pConB_State,lInstanceNo, "ConB");
      gTrace.PrintTraceTimeStamp("Y");
      printf("%s: Instance(%d) current state:%s \n",
            gProcessName,lInstanceNo,lConStr);
   }

   return true;
}   

//-------------------------------------------------------------------------------
// METHOD      : TrnslateMsgToAculabFormat
// DESCRIPTION : Translates Application Tcap Message to ACULAB Tcap
//               Message format
// PARAMETER   : TcapMsg, acu_tcap_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetAcuSccpConState(acu_tcap_msg_t *lMsg)
{
   return mAcuTcapUtil.PrintSccpStatus(lMsg);
}

//-------------------------------------------------------------------------------
// METHOD      : TrnslateMsgToAculabFormat
// DESCRIPTION : Translates Application Tcap Message to ACULAB Tcap
//               Message format
// PARAMETER   : TcapMsg, acu_tcap_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetErrReportSccp(acu_tcap_msg_t *lMsg)
{
   return  mAcuTcapUtil.ErrReportSccp(lMsg);
}
//-------------------------------------------------------------------------------
// METHOD      : CreateAcuTcapTrans
// DESCRIPTION : Creates ACULAB Tcap Messgae Transaction
// PARAMETER   : acu_tcap_trans_t **(Double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::CreateAcuTcapTrans(acu_tcap_trans_t **lTrsnPtr,int &lInstanceNo)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(NULL == mSsapPtr[lInstanceNo])
   {
      mSaapStatus[lInstanceNo].state = EXITING;
      sprintf(lLogText,"AI: Acu SSAP Poiter found NULL (instance %d)\n",
            lInstanceNo);
      gTrace.PrintTraceTimeStamp("Y");
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP31,lLogText);
      return false;
   }

   *lTrsnPtr = acu_tcap_transaction_create(mSsapPtr[lInstanceNo]);
   if(0 == *lTrsnPtr)
   {
      //transaction creation failure
      sprintf(lLogText,"AI: Acu transaction(instance %d) creation failed\n",
            lInstanceNo);
      gTrace.PrintTraceTimeStamp("Y");
      printf("%s: %s %s %s\n",gProcessName,RED,lLogText,UNDO);
      gLog.GenerateLog(ACUTCAP05,lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : CreateAcuTcapOngoingTrans
// DESCRIPTION : Creates ACULAB Tcap Messgae Transaction
// PARAMETER   : acu_tcap_trans_t **(Double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::CreateAcuTcapOngoingTrans(acu_tcap_trans_t **lTrsnPtr,int &lInstanceNo, UINT8 origTransIdLen, UINT32 origTransId, UINT8 destTransIdLen, UINT32 destTransId, int ssn, UINT8 opCode, int invokeId, time_t timeOut, int ssapInstance)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   int lRetVal;

   if(NULL == mSsapPtr[ssapInstance])
   {
      mSaapStatus[ssapInstance].state = EXITING;
      sprintf(lLogText,"AI: Ongoing Acu SSAP Poiter found NULL (instance %d)\n",
            ssapInstance);
      gTrace.PrintTraceTimeStamp("Y");
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP150,lLogText);
      return false;
   }

   sprintf(lLogText,"AI: LocalTid:%08X DestTid:%08X origTransIdLen:%d DestTidLen:%d InvokeId:%d Ssn:%d Opcode:%d timeOut:%d InstanceNo:%d SsapInstance:%d Create Tcap Ongoing Transaction Details", origTransId, destTransId, origTransIdLen, destTransIdLen, invokeId, ssn, opCode, timeOut, lInstanceNo, ssapInstance);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP111,lLogText);


   //acu_tcap_trans_t *acu_tcap_transaction_restore(acu_tcap_ssap_t *ssap,
   //            unsigned int loc_id, unsigned int rem_id, unsigned int rem_id_len);
   *lTrsnPtr = acu_tcap_transaction_restore(mSsapPtr[ssapInstance], origTransId, destTransId, destTransIdLen);

   if(0 == *lTrsnPtr)
   {
      //transaction creation failure
      sprintf(lLogText,"AI: Acu Transaction(instance %d) Restore failed\n",
            ssapInstance);
      gTrace.PrintTraceTimeStamp("Y");
      printf("%s: %s %s %s\n",gProcessName,RED,lLogText,UNDO);
      gLog.GenerateLog(ACUTCAP151,lLogText);
      return false;
   }
   sprintf(lLogText,"AI: Acu transaction(instance %d) creation success LocalTid:%08X\n", ssapInstance, origTransId);

   //int acu_tcap_operation_restore(acu_tcap_trans_t *trans, int invoke_id,
   //            unsigned int class, unsigned int tmo_sec)
   //if(ssn != 146)
   if((ssn == 146 && opCode == CAP_ACTIVITY_TEST_MSG) || (ssn != 146))
   {
      lRetVal = acu_tcap_operation_restore(*lTrsnPtr, invokeId, 1, timeOut);

      if(0 != lRetVal)
      {
         sprintf(lLogText,"AI: LocalTid:%08X DestTid:%08X invokeId:%d Operation Restore failed", 
               origTransId, destTransId, invokeId);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP110,lLogText);
      }

      sprintf(lLogText,"AI: LocalTid:%08X DestTid:%08X invokeId:%d Operation Restore Success", 
            origTransId, destTransId, invokeId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP111,lLogText);
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : AcuTcapOperationRestore 
// DESCRIPTION : Add aculab tcap message component from application tcap message 
//               component
// PARAMETER   : AnsiTcapMsg lTcapMsg,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
#if 0
BOOLEAN TcapAculab::AcuTcapOperationRestore(AnsiTcapMsg lTcapMsg, acu_tcap_msg_t *lMsg)
{
   int lRetVal;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
   DialogueComp      lDialogueComp;

   //last_class The TCAP operation class 0 or 1 to 4, bitwise 'or' in ACU_TCAP_LAST to
   //   generate an ANSI 'INVOKE_LAST' component. The operation classes are:
   //   0 Operation state engine disabled, all message sequences are valid
   //   1 Report success or failure (all responses valid)
   //   2 Report failure only (return-result not valid)
   //   3 Report success only (return-error not valid)
   //   4 Outcome not reported (neither return-result nor return-error valid)


   //acu_tcap_operation_restore
   lRetVal = acu_tcap_msg_add_comp_invoke(lMsg,
         lTcapMsg.tcapComponent.invokeId,
         ACU_TCAP_NO_INVOKE_ID, //linked_id
         //2,//last_class
         1,
         lTcapMsg.tcapComponent.tcapInvokeComp.timeout,
         //5,
         NULL,
         lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode,
         lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.array,
         lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.numberOfBytes
         );

   return true;
}
#endif
//-------------------------------------------------------------------------------
// METHOD      : AllocAcuTcapMsg 
// DESCRIPTION : Allocaes ACULAB Tcap Message
// PARAMETER   : acu_tcap_msg_t **(Double pointer), acu_tcap_trans_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::AllocAcuTcapMsg(acu_tcap_msg_t **lMsg,acu_tcap_trans_t *lTrsnPtr)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   *lMsg = acu_tcap_msg_alloc(lTrsnPtr);
   if(NULL == *lMsg)
   {
      //Messge allocation error
      sprintf(lLogText,"AI: TrsnPtr:%08X Acu message alloc failed", lTrsnPtr);
      gTrace.PrintTraceTimeStamp("Y");
      printf("%s: %s %s %s\n",gProcessName,RED,lLogText,UNDO);
      gLog.GenerateLog(ACUTCAP14,lLogText);
      return false;
   }

   int lRetVal = 0;
   unsigned int lLocId;
   unsigned int lRemId;
   unsigned int lRemIdLen;

   //acu_tcap_trans_t *lTrans = &lMsg->tm_trans;  

   lRetVal = GetTransIds(*lMsg, &lLocId, &lRemId, &lRemIdLen);
   //   lRetVal =  acu_tcap_trans_get_ids(lTrans, lLocId, lRemId, lRemIdLen);

   if(false != lRetVal)
   {
      //Error
      //   printf("Error Code : %d:%s\n",lRetVal,acu_tcap_strerror(lRetVal, 1));
      //return false;
      sprintf(lLogText,"AI: TrsnPtr:%08X lTId:%08X rTId:%08X Acu message alloc Success",
            lTrsnPtr, lLocId, lRemId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP153, 3, lLogText);
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : DeleteAcuTcapTrans 
// DESCRIPTION : DeAllocaes ACULAB Tcap Message transaction
// PARAMETER   : acu_tcap_trans_t *
// RETURN      : void
//-------------------------------------------------------------------------------

void TcapAculab::DeleteAcuTcapTrans(acu_tcap_trans_t *pTrans)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if(NULL != pTrans)
   { 
      acu_tcap_transaction_delete(pTrans);
      sprintf(lLogText,"AI: TrsnPtr:%08X Acu message Deallocate Success", pTrans);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP154, 3, lLogText);
   }   

}

//-------------------------------------------------------------------------------
// METHOD      : FreeAcuTcapMsg 
// DESCRIPTION : Frees ACULAB Tcap Message
// PARAMETER   : acu_tcap_msg_t *
// RETURN      : void
//-------------------------------------------------------------------------------
void TcapAculab::FreeAcuTcapMsg(acu_tcap_msg_t *pMsg)
{   
   acu_tcap_msg_free(pMsg);
} 

//-------------------------------------------------------------------------------
// METHOD      : InitAcuTcapMsg 
// DESCRIPTION : acu_tcap_msg_t *, acu_tcap_msg_type_t
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::InitAcuTcapMsg(acu_tcap_msg_t *lMsg, acu_tcap_msg_type_t lType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   int lRetVal = acu_tcap_msg_init(lMsg, lType);
   if(0 != lRetVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "AI: Acu message init failed for MsgType:%s-RetVal:%s",
            AculabUtil::GetAcuTcapMsgTypeStr(lType), acu_tcap_strerror(lRetVal, 0));
      gTrace.PrintTraceTimeStamp("Y");
      //printf("%s: %s %s %s\n",gProcessName,RED,lLogText,UNDO);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP15,lLogText);

      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTcapDialogue 
// DESCRIPTION : Gets Application TCAP Message dialogue portion
//               from aculab tcap message dialogue
// PARAMETER   : const acu_tcap_dialogue_t *, AnsiTcapMsg &(Refrence)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetTcapDialogue(const acu_tcap_dialogue_t *lDlg, AnsiTcapMsg &lTcapMsg)
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
/* IN new AnsiTcapMsg userInformation is not supported
   if(lDlg->td_flags & ACU_TCAP_DF_HAS_UI)
   {
      if(lDlg->td_ui_len > 11)
      {
         lTcapMsg.userInformation.numberOfBytes = lDlg->td_ui_len - 11;

         memcpy(&lTcapMsg.userInformation.array,lDlg->td_ui+11,
               lTcapMsg.userInformation.numberOfBytes);
         sprintf(lLogText,"AI: DlgId:%d lTId:%08X rTId:%08X Poulating Dlg User Info > 11",
               lTcapMsg.dialogueId,lTcapMsg.origTransId, lTcapMsg.destTransId);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP112, 2, lLogText);
      }
      else
      {
         lTcapMsg.userInformation.numberOfBytes = lDlg->td_ui_len;
         memcpy(&lTcapMsg.userInformation.array,lDlg->td_ui,
               lTcapMsg.userInformation.numberOfBytes);
         sprintf(lLogText,"AI: DlgId:%d lTId:%08X rTId:%08X Poulating Dlg User Info",
               lTcapMsg.dialogueId,lTcapMsg.origTransId, lTcapMsg.destTransId);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP113, 2, lLogText);
      }
   }
*/
 /* IN new AnsiTcapMsg applicationContext is not supported
   if(lDlg->td_flags & ACU_TCAP_DF_HAS_HEX_APP_CTX)
   {  
      lTcapMsg.applicationContext.numberOfBytes = lDlg->td_app_ctx_len;

      memcpy(&lTcapMsg.applicationContext.array,lDlg->td_app_ctx,
            lDlg->td_app_ctx_len);
      sprintf(lLogText, "AI: DlgId:%d lTId:%08X rTId:%08X Poulating Application Context",
            lTcapMsg.dialogueId,lTcapMsg.origTransId, lTcapMsg.destTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP114, 2, lLogText);
   }
  */
   return true;

}

//-------------------------------------------------------------------------------
// METHOD      : GetTcapComponets 
// DESCRIPTION : Gets Application TCAP Components from aculab tcap message
//               componet(s)
// PARAMETER   : acu_tcap_msg_t * lMsg, AnsiTcapMsg &lTcapMsg, const acu_tcap_dialogue_t *lDlg, TcapComponent *lComp, int &lNoOfComp
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
#if 0
BOOLEAN  TcapAculab::GetTcapComponets(acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg,
      const acu_tcap_dialogue_t *lDlg, TcapComponent *lComp,int &lNoOfComp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   int NoOfComp = acu_tcap_msg_has_components(lMsg);

   if(0 == NoOfComp)
   {
      lNoOfComp = 0;
      sprintf(lLogText, "AI: DlgId:%d Components not Present", lTcapMsg.dialogueId);
      T(gTrace,printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUTCAP115, 2, lLogText);

      switch(lMsg->tm_msg_type)
      {
         case ACU_TCAP_MSG_LOCAL_ABORT:
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_USER_ABORT;
               lComp->tcapAbortComp.abortReason.numberOfBytes = 1;
               memcpy(&lComp->tcapAbortComp.abortReason.array,&lMsg->tm_p_abort_cause,1);

	       sprintf(lLogText,
                     "AI: DlgId:%d lTId:%08X Populating Abort Msg(LOCAL_ABORT)",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId );

               //removing received txn id sprintf(lLogText, "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(LOCAL_ABORT)", lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               
	       T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP116, 2, lLogText);
               return true;
            }

         case ACU_TCAP_MSG_P_ABORT:   
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_PROTOCOL_ABORT;
               lComp->tcapAbortComp.abortReason.numberOfBytes = 1;
               memcpy(&lComp->tcapAbortComp.abortReason.array,&lMsg->tm_p_abort_cause,1);
               sprintf(lLogText,
                     "AI: DlgId:%d lTId:%08X Populating Abort Msg(LOCAL_ABORT)",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId );
               //removing received txn id sprintf(lLogText, "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(P_ABORT)", lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););   
               gLog.GenerateLog(ACUTCAP117, 2, lLogText);
               return true;

            }
         case ACU_TCAP_MSG_ITU_ABORT:
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_USER_ABORT;

               if(NULL != lDlg)
               {
                  if(lDlg->td_flags & ACU_TCAP_DF_HAS_ABRT_SOURCE || lDlg->td_flags & ACU_TCAP_DF_HAS_AARE_DIAG)
                  {
                     lComp->tcapAbortComp.abortReason.numberOfBytes = 1;

                     switch(lDlg->td_result)
                     {
                        case ACU_TCAP_AARE_ACCEPTED_USER:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_ACCEPTED_USER;
                           break;

                        case ACU_TCAP_AARE_ACCEPTED_PROVIDER:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_ACCEPTED_PROVIDER;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_NULL:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_NULL;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_NO_REASON:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_NO_REASON;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NULL:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NULL;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NO_REASON:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NO_REASON;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION;
                           break;

                        default:
                           //Add default reason TBD
                           break;
                     }
                  }
		  /* application context not present in current AnsiTcapMsg
                  if(lDlg->td_flags & ACU_TCAP_DF_HAS_HEX_APP_CTX)
                  {
                     memcpy(lTcapMsg.applicationContext.array, lDlg->td_app_ctx, lDlg->td_app_ctx_len);
                     lTcapMsg.applicationContext.numberOfBytes = lDlg->td_app_ctx_len;
                  }
		  */
               }
               
	      sprintf(lLogText, "AI: DlgId:%d lTId:%08X Populating Abort Msg(ITU_ABORT)", lTcapMsg.dialogueId, lTcapMsg.origTransId );
              //Ansi dont support mutiple txn id so removing sprintf(lLogText, "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(ITU_ABORT)", lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP118, 2, lLogText);
               return true;
            }
         default:
            {   
               sprintf(lLogText, "AI: DlgId:%d lTId:%08X Msg:%d", lTcapMsg.dialogueId, lTcapMsg.origTransId ,  lMsg->tm_msg_type);
               //Ansi dont support mutiple txn id so removing  sprintf(lLogText, "AI: DlgId:%d lTId:%08X rTId:%08X Msg:%d", lTcapMsg.dialogueId, lTcapMsg.origTransId , lTcapMsg.destTransId, lMsg->tm_msg_type);
               TERR(gTrace,printf("%s: %s No Component Received\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP36,lLogText);

               //return false;
            }
      }

      lTcapMsg.componentPresent = false;
      lTcapMsg.tcapComponent.lastComponent = TCAP_NO_COMPONENT;
   }
   else
   {
      sprintf(lLogText,"AI: DlgId:%d Components Present", lTcapMsg.dialogueId);
      T(gTrace,printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUTCAP119, 2, lLogText);
      const acu_tcap_component_t *Comp;
      //if(0 != NoOfComp)
      {
         lTcapMsg.componentPresent = true; 

         lNoOfComp = 0; 

         while( ACU_TCAP_ERROR_NO_COMPONENT != acu_tcap_msg_get_component(lMsg , &Comp))
         {   
            //Sending multiple components
            lComp->invokeId = Comp->tc_invoke_id;

            lComp->tcapComp = GetTcapCompType(Comp->tc_type);

            switch(lComp->tcapComp)
            {
               case TCAP_INVOKE_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_INVOKE_COMP_RX);

                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        // INT opcode = National (no pointer was present)
                        lComp->tcapInvokeComp.operation.operationCode = Comp->tc_op_code_val;
                        lComp->tcapInvokeComp.operation.isPrivate = false;
                     
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(NATIONAL OPCODE:%d)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId, 
                              lComp->tcapInvokeComp.operation.operationCode);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP120, 2, lLogText);

                     }
                     else if (Comp->tc_flags & ACU_TCAP_CF_HAS_HEX_OPCODE)
                     {
                        // HEX opcode = Private (pointer was present)
                        lComp->tcapInvokeComp.operation.isPrivate = true;
                        if (Comp->tc_op_code_len >= 2) 
                        {
                           // Pack Family and Specifier into UINT32
                           lComp->tcapInvokeComp.operation.operationCode = 
                              (Comp->tc_op_code[0] << 8) | Comp->tc_op_code[1];
                        }
                        else if (Comp->tc_op_code_len == 1)
                        {
                           lComp->tcapInvokeComp.operation.operationCode = Comp->tc_op_code[0];
                        }
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(PRIVATE OPCODE:%04X)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId, 
                              lComp->tcapInvokeComp.operation.operationCode);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP120, 2, lLogText);
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapInvokeComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;
                        memcpy(&(lComp->tcapInvokeComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(PARAM DATA)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP121, 2, lLogText);
                     }
                  }
                  break;
               case TCAP_RET_RESULT_COMP:
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_RET_RESULT_COMP_RX);

                     TEXT lText[150 + 1] = "";
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        lComp->tcapRetResultComp.operation.operationCode = Comp->tc_op_code_val;
                        lComp->tcapRetResultComp.operation.isPrivate = false;
                        sprintf(lText,"(OPCODE:%d)", lComp->tcapRetResultComp.operation.operationCode);
                     }
                     else
                     {
                        lComp->tcapRetResultComp.operation.operationCode = 0xFF;
                        lComp->tcapRetResultComp.operation.isPrivate = false;
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapRetResultComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;
                        memcpy(&(lComp->tcapRetResultComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                        sprintf(lText,"%s","(PARAM DATA)");
                     }
                     if(Comp->tc_type == ACU_TCAP_COMP_ANSI_RESULT_NOTLAST)
                     {   
                        lComp->tcapRetResultComp.lastIndicator = 
                           TCAP_NOT_LAST_INDICATOR; 
                        sprintf(lLogText,
                              "AI: DlgId:%d Populating Ret_Result Msg %s NOT_LAST",
                              lTcapMsg.dialogueId, lText); 
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP122, 2, lLogText);
                     }
                     else
                     {
                        lComp->tcapRetResultComp.lastIndicator = 
                           TCAP_LAST_INDICATOR;
                        sprintf(lLogText,"AI: DlgId:%d Populating Ret_Result Msg %s LAST ",
                              lTcapMsg.dialogueId, lText);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP123, 2, lLogText);
                     }
                  }
                  break;
               case TCAP_RET_ERROR_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_RET_ERROR_COMP_RX);

                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        // INT error code = National (no pointer was present)
                        lComp->tcapRetErrorComp.errorCode.errorCode = Comp->tc_op_code_val;
                        lComp->tcapRetErrorComp.errorCode.isPrivate = false;
                     }
                     else if(Comp->tc_flags & ACU_TCAP_CF_HAS_HEX_OPCODE)
                     {
                        // HEX error code = Private (pointer was present)
                        lComp->tcapRetErrorComp.errorCode.isPrivate = true;
                        if (Comp->tc_op_code_len >= 2)
                        {
                           lComp->tcapRetErrorComp.errorCode.errorCode =
                              (Comp->tc_op_code[0] << 8) | Comp->tc_op_code[1];
                        }
                        else if (Comp->tc_op_code_len == 1)
                        {
                           lComp->tcapRetErrorComp.errorCode.errorCode = Comp->tc_op_code[0];
                        }
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapRetErrorComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;

                        memcpy(&(lComp->tcapRetErrorComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                     }      
                  }
                  break;
               case TCAP_REJECT_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_REJECT_COMP_RX);

                     lComp->tcapRejectComp.rejectType = 
                        (EnumRejectType)Comp->tc_rejected_type;

                     SetTcapProblem(lComp->tcapRejectComp.problem,
                           Comp->tc_op_code_val);

                  }
                  break;
               case TCAP_ABORT_COMP:
                  {
                  }
                  break;
               default:  
                  {
                     sprintf(lLogText,
                           "AI: DlgId:%d lTId:%08X rTId:%08X Comp:%d Unknown Msg Component",
                           lTcapMsg.dialogueId, lTcapMsg.origTransId,
                           lTcapMsg.destTransId, lComp->tcapComp);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP37,lLogText);

                     return false;
                  }
            }
            lComp->lastComponent = TCAP_NOT_LAST_COMPONENT;
            lNoOfComp++; 
            lComp++;
         }
         lComp--;
         lComp->lastComponent = TCAP_LAST_COMPONENT;

      }
      //else
      //{
      //}
   }

   return true;
}
#endif
//-------------------------------------------------------------------------------
// METHOD      : GetTcapComponets
// DESCRIPTION : Parses the Aculab Msg structure to Application TcapMsg
// PARAMETER   : acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg,
//               const acu_tcap_dialogue_t *lDialogInfo,
//               TcapComponent *lComp, int &lNoOfComp
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------

BOOLEAN  TcapAculab::GetTcapComponets(acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg,
      const acu_tcap_dialogue_t *lDlg, TcapComponent *lComp,int &lNoOfComp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   int NoOfComp = acu_tcap_msg_has_components(lMsg);

   if(0 == NoOfComp)
   {
      lNoOfComp = 0;
      sprintf(lLogText, "AI: DlgId:%d Components not Present", lTcapMsg.dialogueId);
      T(gTrace,printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUTCAP115, 2, lLogText);

      switch(lMsg->tm_msg_type)
      {
         case ACU_TCAP_MSG_LOCAL_ABORT:
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_USER_ABORT;
               lComp->tcapAbortComp.abortReason.numberOfBytes = 1;
               memcpy(&lComp->tcapAbortComp.abortReason.array,&lMsg->tm_p_abort_cause,1);

               sprintf(lLogText,
                     "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(LOCAL_ABORT)",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP116, 2, lLogText);
               return true;
            }

         case ACU_TCAP_MSG_P_ABORT:   
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_PROTOCOL_ABORT;
               lComp->tcapAbortComp.abortReason.numberOfBytes = 1;
               memcpy(&lComp->tcapAbortComp.abortReason.array,&lMsg->tm_p_abort_cause,1);

               sprintf(lLogText,
                     "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(P_ABORT)",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););   
               gLog.GenerateLog(ACUTCAP117, 2, lLogText);
               return true;

            }
         case ACU_TCAP_MSG_ITU_ABORT:
            {
               lNoOfComp = 1;
               lTcapMsg.componentPresent = true;
               lComp->invokeId = 0;
               lComp->lastComponent = TCAP_LAST_COMPONENT;
               lComp->tcapComp = TCAP_ABORT_COMP;
               lComp->tcapAbortComp.abortType = TCAP_USER_ABORT;

               if(NULL != lDlg)
               {
                  if(lDlg->td_flags & ACU_TCAP_DF_HAS_ABRT_SOURCE || lDlg->td_flags & ACU_TCAP_DF_HAS_AARE_DIAG)
                  {
                     lComp->tcapAbortComp.abortReason.numberOfBytes = 1;

                     switch(lDlg->td_result)
                     {
                        case ACU_TCAP_AARE_ACCEPTED_USER:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_ACCEPTED_USER;
                           break;

                        case ACU_TCAP_AARE_ACCEPTED_PROVIDER:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_ACCEPTED_PROVIDER;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_NULL:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_NULL;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_NO_REASON:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_NO_REASON;
                           break;

                        case ACU_TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NULL:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NULL;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NO_REASON:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NO_REASON;
                           break;

                        case ACU_TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION:
                           lComp->tcapAbortComp.abortReason.array[0] = TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION;
                           break;

                        default:
                           //Add default reason TBD
                           break;
                     }
                  }
                  if(lDlg->td_flags & ACU_TCAP_DF_HAS_HEX_APP_CTX)
                  {
                     memcpy(lTcapMsg.applicationContext.array, lDlg->td_app_ctx, lDlg->td_app_ctx_len);
                     lTcapMsg.applicationContext.numberOfBytes = lDlg->td_app_ctx_len;
                  }
               }

               sprintf(lLogText, 
                     "AI: DlgId:%d lTId:%08X rTId:%08X Populating Abort Msg(ITU_ABORT)",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP118, 2, lLogText);
               return true;
            }
         default:
            {
               sprintf(lLogText,
                     "AI: DlgId:%d lTId:%08X rTId:%08X Msg:%d",
                     lTcapMsg.dialogueId, lTcapMsg.origTransId ,
                     lTcapMsg.destTransId, lMsg->tm_msg_type);
               TERR(gTrace,printf("%s: %s No Component Received\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP36,lLogText);

               //return false;
            }
      }

      lTcapMsg.componentPresent = false;
      lTcapMsg.tcapComponent.lastComponent = TCAP_NO_COMPONENT;
   }
   else
   {
      sprintf(lLogText,"AI: DlgId:%d Components Present", lTcapMsg.dialogueId);
      T(gTrace,printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACUTCAP119, 2, lLogText);
      const acu_tcap_component_t *Comp;
      //if(0 != NoOfComp)
      {
         lTcapMsg.componentPresent = true; 

         lNoOfComp = 0; 

         while( ACU_TCAP_ERROR_NO_COMPONENT != acu_tcap_msg_get_component(lMsg , &Comp))
         {   
            //Sending multiple components
            lComp->invokeId = Comp->tc_invoke_id;

            lComp->tcapComp = GetTcapCompType(Comp->tc_type);

            switch(lComp->tcapComp)
            {
               case TCAP_INVOKE_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_INVOKE_COMP_RX);

                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        // INT opcode = National (no pointer was present)
                        lComp->tcapInvokeComp.operation.operationCode = Comp->tc_op_code_val;
                        lComp->tcapInvokeComp.operation.isPrivate = false;
                     
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(NATIONAL OPCODE:%d)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId, 
                              lComp->tcapInvokeComp.operation.operationCode);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP120, 2, lLogText);

                     }
                     else if (Comp->tc_flags & ACU_TCAP_CF_HAS_HEX_OPCODE)
                     {
                        // HEX opcode = Private (pointer was present)
                        lComp->tcapInvokeComp.operation.isPrivate = true;
                        if (Comp->tc_op_code_len >= 2) 
                        {
                           // Pack Family and Specifier into UINT32
                           lComp->tcapInvokeComp.operation.operationCode = 
                              (Comp->tc_op_code[0] << 8) | Comp->tc_op_code[1];
                        }
                        else if (Comp->tc_op_code_len == 1)
                        {
                           lComp->tcapInvokeComp.operation.operationCode = Comp->tc_op_code[0];
                        }
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(PRIVATE OPCODE:%04X)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId, 
                              lComp->tcapInvokeComp.operation.operationCode);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP120, 2, lLogText);
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapInvokeComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;
                        memcpy(&(lComp->tcapInvokeComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                        sprintf(lLogText,
                              "AI: DlgId:%d lTId:%08X rTId:%08X Populating Invoke Msg(PARAM DATA)",
                              lTcapMsg.dialogueId, lTcapMsg.origTransId ,lTcapMsg.destTransId);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP121, 2, lLogText);
                     }
                  }
                  break;
               case TCAP_RET_RESULT_COMP:
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_RET_RESULT_COMP_RX);

                     TEXT lText[150 + 1] = "";
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        lComp->tcapRetResultComp.operation.operationCode = Comp->tc_op_code_val;
                        lComp->tcapRetResultComp.operation.isPrivate = false;
                        sprintf(lText,"(OPCODE:%d)", lComp->tcapRetResultComp.operation.operationCode);
                     }
                     else
                     {
                        lComp->tcapRetResultComp.operation.operationCode = 0xFF;
                        lComp->tcapRetResultComp.operation.isPrivate = false;
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapRetResultComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;
                        memcpy(&(lComp->tcapRetResultComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                        sprintf(lText,"%s","(PARAM DATA)");
                     }
                     if(Comp->tc_type == ACU_TCAP_COMP_ANSI_RESULT_NOTLAST)
                     {   
                        lComp->tcapRetResultComp.lastIndicator = 
                           TCAP_NOT_LAST_INDICATOR; 
                        sprintf(lLogText,
                              "AI: DlgId:%d Populating Ret_Result Msg %s NOT_LAST",
                              lTcapMsg.dialogueId, lText); 
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP122, 2, lLogText);
                     }
                     else
                     {
                        lComp->tcapRetResultComp.lastIndicator = 
                           TCAP_LAST_INDICATOR;
                        sprintf(lLogText,"AI: DlgId:%d Populating Ret_Result Msg %s LAST ",
                              lTcapMsg.dialogueId, lText);
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP123, 2, lLogText);
                     }
                  }
                  break;
               case TCAP_RET_ERROR_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_RET_ERROR_COMP_RX);

                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_INT_OPCODE)
                     {
                        // INT error code = National (no pointer was present)
                        lComp->tcapRetErrorComp.errorCode.errorCode = Comp->tc_op_code_val;
                        lComp->tcapRetErrorComp.errorCode.isPrivate = false;
                     }
                     else if(Comp->tc_flags & ACU_TCAP_CF_HAS_HEX_OPCODE)
                     {
                        // HEX error code = Private (pointer was present)
                        lComp->tcapRetErrorComp.errorCode.isPrivate = true;
                        if (Comp->tc_op_code_len >= 2)
                        {
                           lComp->tcapRetErrorComp.errorCode.errorCode =
                              (Comp->tc_op_code[0] << 8) | Comp->tc_op_code[1];
                        }
                        else if (Comp->tc_op_code_len == 1)
                        {
                           lComp->tcapRetErrorComp.errorCode.errorCode = Comp->tc_op_code[0];
                        }
                     }
                     if(Comp->tc_flags & ACU_TCAP_CF_HAS_PARAMETER)
                     {
                        lComp->tcapRetErrorComp.parameterData.numberOfBytes =
                           Comp->tc_param_len;

                        memcpy(&(lComp->tcapRetErrorComp.parameterData.array),
                              Comp->tc_param,Comp->tc_param_len);
                     }      
                  }
                  break;
               case TCAP_REJECT_COMP: 
                  {
                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_REJECT_COMP_RX);

                     lComp->tcapRejectComp.rejectType = 
                        (EnumRejectType)Comp->tc_rejected_type;

                     SetTcapProblem(lComp->tcapRejectComp.problem,
                           Comp->tc_op_code_val);

                  }
                  break;
               case TCAP_ABORT_COMP:
                  {
                  }
                  break;
               default:  
                  {
                     sprintf(lLogText,
                           "AI: DlgId:%d lTId:%08X rTId:%08X Comp:%d Unknown Msg Component",
                           lTcapMsg.dialogueId, lTcapMsg.origTransId,
                           lTcapMsg.destTransId, lComp->tcapComp);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP37,lLogText);

                     return false;
                  }
            }
            lComp->lastComponent = TCAP_NOT_LAST_COMPONENT;
            lNoOfComp++; 
            lComp++;
         }
         lComp--;
         lComp->lastComponent = TCAP_LAST_COMPONENT;

      }
      //else
      //{
      //}
   }

   return true;
}



//-------------------------------------------------------------------------------
// METHOD      : GetTcapTimeOutComponet 
// DESCRIPTION : Form TCAP Time out message
// PARAMETER   : acu_tcap_msg_t * lMsg, AnsiTcapMsg & lTcapMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------

BOOLEAN  TcapAculab::GetTcapTimeOutComponet(acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg)
{
   const acu_tcap_component_t *Comp;

   int lRetErr = acu_tcap_msg_get_component(lMsg , &Comp);

   lTcapMsg.tcapDlg = TCAP_RSP_TIMEOUT;
   lTcapMsg.componentPresent = true;
   lTcapMsg.tcapComponent.tcapComp = TCAP_RSP_TIMEOUT_COMP;
   lTcapMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
   lTcapMsg.tcapComponent.invokeId =  Comp->tc_invoke_id;

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : FormTcapAbortMsg 
// DESCRIPTION : Form TCAP abort message  
// PARAMETER   : acu_tcap_msg_t * AnsiTcapMsg & ,TcapComponent *, int &lNoOfComp
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN  TcapAculab::FormTcapAbortMsg(AnsiTcapMsg &lTcapMsg,EnumAbortType lType,
      acu_tcap_p_abort_cause_t lCause)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lTcapMsg.tcapDlg = TCAP_ANSI_ABORT;
   lTcapMsg.componentPresent = true;
   lTcapMsg.tcapComponent.invokeId = 0;

   lTcapMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
   lTcapMsg.tcapComponent.tcapComp = TCAP_ABORT_COMP;

   if(lType == TCAP_PROTOCOL_ABORT)
      lTcapMsg.tcapComponent.tcapAbortComp.abortType = TCAP_PROTOCOL_ABORT;
   else
      lTcapMsg.tcapComponent.tcapAbortComp.abortType = TCAP_USER_ABORT;
   lTcapMsg.tcapComponent.tcapAbortComp.abortReason.numberOfBytes = 1;
   memcpy(&lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array,&lCause,1);

   sprintf(lLogText,
         "AI: DlgId:%d lTId:%08X Populating Abort Msg",
         lTcapMsg.dialogueId, lTcapMsg.origTransId );

   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP124, 2, lLogText);
   return true;

}

//-------------------------------------------------------------------------------
// METHOD      : SetTcapProblem 
// DESCRIPTION : Sets TCAP Problem type from aculab tcap message  
// PARAMETER   : TCAPProblem &lProblem, unsigned int lOpCodeVal 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetTcapProblem(TCAPProblem &lProblem, unsigned int lOpCodeVal)
{

   switch(lOpCodeVal)
   {
      case ACU_TCAP_REJECT_ITU_GENERAL_UNRECOGNIZED_COMPONENT:
         {
            lProblem.problemCodeType = 1;
            lProblem.problemCode = 0;
         }
         break;
      case ACU_TCAP_REJECT_ITU_GENERAL_MISTYPED_COMPONENT:
         {
            lProblem.problemCodeType = 1;
            lProblem.problemCode = 1;
         }
         break;
      case ACU_TCAP_REJECT_ITU_GENERAL_BADLY_STRUCTURED_COMPONENT:
         {
            lProblem.problemCodeType = 1;
            lProblem.problemCode = 2;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_DUPLICATE_INVOKE_ID:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 0;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_OPERATION:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 1;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_MISTYPED_PARAMETER:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 2;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_RESOURCE_LIMITATION:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 3;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_INITIATING_RELEASE:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 4;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_LINKED_ID:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 5;
         }
         break;
      case ACU_TCAP_REJECT_ITU_INVOKE_LINKED_RESPONSE_UNEXPECTED:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 6;
         }
         break;
         //case ACU_TCAP_REJECT_ITU_INVOKE_LINKED_RESPONSE_EXPECTED:
         //   {
         //      lProblem.problemCodeType = 2;
         //      lProblem.problemCode = 6;
         //   }
         //   break;
      case ACU_TCAP_REJECT_ITU_INVOKE_UNEXPECTED_LINKED_OPERATION:
         {
            lProblem.problemCodeType = 2;
            lProblem.problemCode = 7;
         }
         break;
      case ACU_TCAP_REJECT_ITU_RESULT_UNRECOGNIZED_INVOKE_ID:
         {
            lProblem.problemCodeType = 3;
            lProblem.problemCode = 0;
         }
         break;
      case ACU_TCAP_REJECT_ITU_RESULT_RETURN_RESULT_UNEXPECTED:
         {
            lProblem.problemCodeType = 3;
            lProblem.problemCode = 1;
         }
         break;
      case ACU_TCAP_REJECT_ITU_RESULT_MISTYPED_PARAMETER:
         {
            lProblem.problemCodeType = 3;
            lProblem.problemCode = 2;
         }
         break;
      case ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_INVOKE_ID:
         {
            lProblem.problemCodeType = 4;
            lProblem.problemCode = 0;
         }
         break;
      case ACU_TCAP_REJECT_ITU_ERROR_RETURN_ERROR_UNEXPECTED:
         {
            lProblem.problemCodeType = 4;
            lProblem.problemCode = 1;
         }
         break;
      case ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_ERROR:
         {
            lProblem.problemCodeType = 4;
            lProblem.problemCode = 2;
         }
         break;
      case ACU_TCAP_REJECT_ITU_ERROR_UNEXPECTED_ERROR:
         {
            lProblem.problemCodeType = 4;
            lProblem.problemCode = 3;
         }
         break;
      case ACU_TCAP_REJECT_ITU_ERROR_MISTYPED_PARAMETER:
         {
            lProblem.problemCodeType = 4;
            lProblem.problemCode = 4;
         }
         break;
      default:
         lProblem.problemCodeType = 0xff;
         return false;
   }

   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : GetTcapCompType 
// DESCRIPTION : Gets application tcap meesage component type from 
//               aculab tcap message component
// PARAMETER   : acu_tcap_comp_type_t 
// RETURN      : EnumTcapComp
//-------------------------------------------------------------------------------
EnumTcapComp TcapAculab::GetTcapCompType(const acu_tcap_comp_type_t &lComp)
{
   switch(lComp)
   {
      case ACU_TCAP_COMP_ITU_INVOKE:
      //ANSI
      case ACU_TCAP_COMP_ANSI_INVOKE_LAST:
      case ACU_TCAP_COMP_ANSI_INVOKE_NOTLAST:      
	      return TCAP_INVOKE_COMP;

      case ACU_TCAP_COMP_ITU_RESULT_LAST:
      //ANSIacu_tcap_msg_add_dialogue
      case ACU_TCAP_COMP_ANSI_RESULT_LAST:   
         return TCAP_RET_RESULT_COMP;

      case ACU_TCAP_COMP_ITU_ERROR:
      //ANSI
      case ACU_TCAP_COMP_ANSI_ERROR:
	      return TCAP_RET_ERROR_COMP;

      case ACU_TCAP_COMP_ITU_REJECT:
      //ANSI
      case ACU_TCAP_COMP_ANSI_REJECT:
         return TCAP_REJECT_COMP;

      case ACU_TCAP_COMP_ITU_RESULT_NOTLAST:
      //ANSI
      case ACU_TCAP_COMP_ANSI_RESULT_NOTLAST:
	      return TCAP_RET_RESULT_COMP;

      case ACU_TCAP_COMP_LOCAL_REJECT:
      case ACU_TCAP_COMP_OP_TIMEOUT:
      default:
         return TCAP_ABORT_COMP;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : AddAcuTcapDialogue 
// DESCRIPTION : Adds dialogue to aculab tcap message from applicaton
//               TCAP Message
// PARAMETER   : acu_tcap_msg_t *lMsg,AnsiTcapMsg lTcapMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
/* This not required as per ansi
BOOLEAN TcapAculab::AddAcuTcapDialogue(acu_tcap_msg_t *lMsg,AnsiTcapMsg lTcapMsg)
{

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
   UINT32 lAbortReason;
   
   uint16_t ctx_len = 0;
   // Only extract the value if the application actually provided it
   if (lTcapMsg.applicationContext.numberOfBytes >= 2) 
   {
      ctx_len = (lTcapMsg.applicationContext.array[0] << 8) | lTcapMsg.applicationContext.array[1];
   }

   if((TCAP_ABORT_COMP == lTcapMsg.tcapComponent.tcapComp))
   {
      if(lTcapMsg.tcapComponent.tcapAbortComp.abortType == TCAP_USER_ABORT)
      {
         if(lTcapMsg.tcapComponent.tcapAbortComp.abortReason.numberOfBytes > 0)
         {
            //if(lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array[0] == 2)
            switch(lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array[0])
            {
               case TCAP_AARE_ACCEPTED_USER: 
                  lAbortReason = ACU_TCAP_AARE_ACCEPTED_USER;
                  break;

               case TCAP_AARE_ACCEPTED_PROVIDER :
                  lAbortReason = ACU_TCAP_AARE_ACCEPTED_PROVIDER;
                  break;

               case TCAP_AARE_REJECT_USER_NULL :
                  lAbortReason = ACU_TCAP_AARE_REJECT_USER_NULL;
                  break;

               case TCAP_AARE_REJECT_USER_NO_REASON :
                  lAbortReason = ACU_TCAP_AARE_REJECT_USER_NO_REASON;
                  break;

               case TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED :
                  lAbortReason = ACU_TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED;
                  break;

               case TCAP_AARE_REJECT_PROVIDER_NULL :
                  lAbortReason = ACU_TCAP_AARE_REJECT_PROVIDER_NULL;
                  break;

               case TCAP_AARE_REJECT_PROVIDER_NO_REASON :
                  lAbortReason = ACU_TCAP_AARE_REJECT_PROVIDER_NO_REASON;
                  break;

               case TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION:
                  lAbortReason = ACU_TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION;
                  break;
                  //default:
            }
           
            // int lRetVal = acu_tcap_msg_add_dialogue(lMsg, 
            //       lAbortReason,
            //       lTcapMsg.applicationContext.array,
            //       lTcapMsg.applicationContext.numberOfBytes);
           

	        //ANSI -  Pass NULL for array, pass value in length parameter
         //   int lRetVal = acu_tcap_msg_add_dialogue(lMsg, lAbortReason, NULL, ctx_len);
         //   //UPTO ANSI         
	      //   if(0 != lRetVal)
         //       {
         //          mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
         //          sprintf(lLogText,"Tcap Add Msg Dialog Error %s",lErrText);
         //          TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         //          gLog.GenerateLog(ACUTCAP07,lLogText);
         //          return false;
         //       }

            if (ctx_len > 0 || lAbortReason != 0) 
           {
              int lRetVal = acu_tcap_msg_add_dialogue(lMsg, lAbortReason, NULL, ctx_len);
              //UPTO ANSI         
              if(0 != lRetVal)
              {
                 mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                 sprintf(lLogText,"Tcap Add Msg Dialog Error %s",lErrText);
                 TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                 gLog.GenerateLog(ACUTCAP07,lLogText);
                 return false;
              }
           }


            return true;
         }
      }
   }


   // //ITU Abort (ABRT) abort source: 0 => user, 1 => provider
     
   //ANSI
   // int lRetVal = acu_tcap_msg_add_dialogue(lMsg, 0, NULL, ctx_len);
      
   // //UPTO ANSI
   // if(0 != lRetVal)
   // {
   //    mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
   //    sprintf(lLogText,"Tcap Add Msg Dialog App Context Error %s",lErrText);
   //    TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   //    gLog.GenerateLog(ACUTCAP07,lLogText);
   //    return false;
   // }

   if (ctx_len > 0)
   {
      int lRetVal = acu_tcap_msg_add_dialogue(lMsg, 0, NULL, ctx_len);   
         if(0 != lRetVal)
      {
         mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
         sprintf(lLogText,"Tcap Add Msg Dialog App Context Error %s",lErrText);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP07,lLogText);
         return false;
      }
   }   
   // UPTO ANSI -
  
   // else if(lTcapMsg.applicationContext.numberOfBytes != 0)
   // {
   // int lRetVal = acu_tcap_msg_add_dialogue(lMsg, 0,
   //          lTcapMsg.applicationContext.array, 
   //          lTcapMsg.applicationContext.numberOfBytes);
   //    if(0 != lRetVal)
   //    {
   //       mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
   //       sprintf(lLogText,"Tcap Add Msg Dialog App Context Error %s",lErrText);
   //       TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   //       gLog.GenerateLog(ACUTCAP07,lLogText);
   //       return false;
   //    }

   // }
   
   // if(lTcapMsg.userInformation.numberOfBytes != 0)
   // {
   //    UINT8                   lTempArray[SS7_MAX_USER_INFO_PDU_LEN + 1];
   //    lTempArray[0] = 0x28;
   //    lTempArray[1] = lTcapMsg.userInformation.numberOfBytes;
   //    memcpy(&lTempArray[2],lTcapMsg.userInformation.array,lTcapMsg.userInformation.numberOfBytes);

   //    int lRetVal = acu_tcap_msg_add_dlg_userinfo(lMsg,lTempArray,
   //          lTcapMsg.userInformation.numberOfBytes + 2);

   //    //int lRetVal = acu_tcap_msg_add_dlg_userinfo(lMsg,lTcapMsg.userInformation.array,
   //    //      lTcapMsg.userInformation.numberOfBytes);
   //    if(0 != lRetVal)
   //    {
   //       mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
   //       sprintf(lLogText,"Tcap Add Msg Dialog UserInfo Error %s",lErrText);
   //       TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   //       gLog.GenerateLog(ACUTCAP07,lLogText);
   //       return false;
   //    }

   // }

   return true;
}
*/
//-------------------------------------------------------------------------------
// METHOD      : AddAcuTcapComponet 
// DESCRIPTION : Add aculab tcap message component from application tcap message 
//               component
// PARAMETER   : AnsiTcapMsg lTcapMsg,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
#if 0
BOOLEAN TcapAculab::AddAcuTcapComponet(AnsiTcapMsg lTcapMsg, acu_tcap_msg_t *lMsg)
{
   int lRetVal;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
   DialogueComp      lDialogueComp;
   map<UINT32, DialogueComp>::iterator lDiaComp;
   //ANSI 
   int mStandard = SS7_STANDARD_ANSI ; //temp - ansi test
   memset(&lDialogueComp, 0, sizeof(DialogueComp));

   //sleep(10); // TBR

   lDiaComp = mDialigueComponent.find(lTcapMsg.dialogueId);

   if(lDiaComp != mDialigueComponent.end())
   {
      //lDialogueComp.numberOfComponent = (*lDiaComp).second.numberOfComponent;
      memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
      mDialigueComponent.erase(lTcapMsg.dialogueId);

      //TBR
      /* while(lDialogueComp.numberOfComponent != 4)
         {
         memset(&lDialogueComp, 0, sizeof(DialogueComp));
         lDiaComp = mDialigueComponent.find(lTcapMsg.dialogueId);
         memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
         mDialigueComponent.erase(lTcapMsg.dialogueId);
         }
      //TBR*/
      //sleep(10);

      sprintf(lLogText,
            "AI: DlgId:%d Number of Components Received %d",
            lTcapMsg.dialogueId, lDialogueComp.numberOfComponent);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(ACUTCAP125, 2, lLogText);
   }
   else
   {
      lDialogueComp.numberOfComponent = 1;
      memcpy(&lDialogueComp.tcapComponent[0], &lTcapMsg.tcapComponent, sizeof(TcapComponent));
   }

   for(UINT8 lTemp = 0; lTemp < lDialogueComp.numberOfComponent; lTemp++)
   {
      memcpy(&lTcapMsg.tcapComponent, &lDialogueComp.tcapComponent[lTemp], 
            sizeof(TcapComponent));

      //Replace opcode with null if it is 0xFF/255
      if(lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode == 0xFF)
         lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode = 0x00;

      switch(lTcapMsg.tcapComponent.tcapComp)
      {   
         case TCAP_INVOKE_COMP:
            {
               if(mPegFlag)
                  gPeg.PegEvent(PEG_TCAP_INVOKE_COMP_TX);

               //last_class The TCAP operation class 0 or 1 to 4, bitwise 'or' in ACU_TCAP_LAST to
               //   generate an ANSI 'INVOKE_LAST' component. The operation classes are:
               //   0 Operation state engine disabled, all message sequences are valid
               //   1 Report success or failure (all responses valid)
               //   2 Report failure only (return-result not valid)
               //   3 Report success only (return-error not valid)
               //   4 Outcome not reported (neither return-result nor return-error valid)
            
	       //ANSI -
	            int lastClass = 1;
               if (mStandard == SS7_STANDARD_ANSI && lTcapMsg.tcapComponent.lastComponent == TCAP_LAST_COMPONENT) {
                   lastClass |= ACU_TCAP_LAST;
               }


               int invokeId = lTcapMsg.tcapComponent.invokeId;
               if (invokeId == 0) {
                  invokeId = ACU_TCAP_NO_INVOKE_ID;
               }
              // std::cout<< " TBR : [lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode] --> "<<lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode<<std::endl;

               UINT8 ansi_priv_op[2];
               if (mStandard == SS7_STANDARD_ANSI) {
                  ansi_priv_op[0] = lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationFamily;
                  ansi_priv_op[1] = lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode;
               }

               lRetVal = acu_tcap_msg_add_comp_invoke(lMsg,
                     invokeId,
                     ACU_TCAP_NO_INVOKE_ID, // linked_id
                     lastClass,
                     lTcapMsg.tcapComponent.tcapInvokeComp.timeout,
                     (void*)ansi_priv_op ,
                      2 ,
                      lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.array,
                     lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.numberOfBytes
                  );
                  
 		         /*lRetVal = acu_tcap_msg_add_comp_invoke(lMsg,
               invokeId,
               ACU_TCAP_NO_INVOKE_ID, //linked_id
               1,// lastClass,
               lTcapMsg.tcapComponent.tcapInvokeComp.timeout,
               //5,
               NULL,
               (mStandard == SS7_STANDARD_ANSI) ?
                 ((lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationFamily << 8) |
                  lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode) :
                 lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode,
               lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.array,
               lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.numberOfBytes
               );    */
	        //UPTO ANSI - 
		/*
	       lRetVal = acu_tcap_msg_add_comp_invoke(lMsg,
                     lTcapMsg.tcapComponent.invokeId,
                     ACU_TCAP_NO_INVOKE_ID, //linked_id
                     //2,//last_class
                     1,
                     lTcapMsg.tcapComponent.tcapInvokeComp.timeout,
                     //5,
                     NULL,
                     lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode,
                     lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.array,
                     lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.numberOfBytes
                     );
		*/


               if(0 != lRetVal)
               {
                  mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                  mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                  sprintf(lLogText,"Add Invoke Componet Error %s",lErrText);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP07,lLogText);
                  return false;
               }
               break;
            }
         case TCAP_RET_RESULT_COMP:
            {
               if(mPegFlag)
                  gPeg.PegEvent(PEG_TCAP_RET_RESULT_COMP_TX);

               int lLast = ACU_TCAP_LAST;
               if(lTcapMsg.tcapComponent.tcapRetResultComp.lastIndicator == TCAP_NOT_LAST_INDICATOR)
               {
                  lLast = 0;
               }

                lRetVal = acu_tcap_msg_add_comp_result(lMsg,
                      lTcapMsg.tcapComponent.invokeId,
                      lLast,
                      NULL,
                      (mStandard == SS7_STANDARD_ANSI) ?
                        ((lTcapMsg.tcapComponent.tcapRetResultComp.operation.operationFamily << 8) |
                         lTcapMsg.tcapComponent.tcapRetResultComp.operation.operationCode) :
                        lTcapMsg.tcapComponent.tcapRetResultComp.operation.operationCode,
                      lTcapMsg.tcapComponent.tcapRetResultComp.parameterData.array,
                      lTcapMsg.tcapComponent.tcapRetResultComp.parameterData.numberOfBytes);
               if(0 != lRetVal)
               {
                  mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                  mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                  sprintf(lLogText,"Tcap Add Msg Component Result Error %s",lErrText);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP07,lLogText);
                  return false;

               }

               break;
            }
         case TCAP_RET_ERROR_COMP:
            {
               if(mPegFlag)
                  gPeg.PegEvent(PEG_TCAP_RET_ERROR_COMP_TX);

                     //             lRetVal = acu_tcap_msg_add_comp_error(lMsg,
                     // lTcapMsg.tcapComponent.invokeId,
                     // NULL, //Local error
                     // lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCodeData.array[0],
                     // lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.array,
                     // lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.numberOfBytes);

                // ANSI error code = 16-bit: upper byte = errorType (family), lower byte = array[0] (specifier)
               int ansiErrorCode = (lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorType << 8) |
                                    lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCodeData.array[0];
   


               lRetVal = acu_tcap_msg_add_comp_error(lMsg,
                      lTcapMsg.tcapComponent.invokeId,
                      NULL, // ANSI: pass NULL for op pointer, numeric code below
                      ansiErrorCode,
                      lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.array,
                      lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.numberOfBytes);
                      
               if(0 != lRetVal)
               {
                  mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                  mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                  sprintf(lLogText,"Tcap Add Msg ComponentError Err %s",lErrText);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP07,lLogText);
                  return false;
               } 
               break;
            }
         case TCAP_REJECT_COMP:
            {
               if(mPegFlag)
                  gPeg.PegEvent(PEG_TCAP_REJECT_COMP_TX);
               acu_tcap_reject_problem_t lProb;

               GetAcuRejectProblem(lProb,
                     lTcapMsg.tcapComponent.tcapRejectComp.problem);

               lRetVal = acu_tcap_msg_add_comp_reject(lMsg,
                     lTcapMsg.tcapComponent.invokeId,
                     lProb,
                     NULL,
                     0);
               if(0 != lRetVal)
               {
                  mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                  mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                  sprintf(lLogText,"Tcap Add Msg Component Reject Error %s",lErrText);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP07,lLogText);
                  return false;
               }
               break;
            }
         case TCAP_ABORT_COMP: 
            {
	       // ANSI -
               if (mStandard == SS7_STANDARD_ANSI) 
               {
                  acu_tcap_msg_add_ansi_abort_userinfo(
                        lMsg,
                        lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array,
                        lTcapMsg.tcapComponent.tcapAbortComp.abortReason.numberOfBytes
                  );
               } 	           
               //acu_tcap_msg_add_ans_abort_userinfo(); 
               break;
            }
         default:
            break;
      }   
   }

   return true;
}

#endif
//-------------------------------------------------------------------------------
// METHOD      : AddAcuTcapComponet
// DESCRIPTION : Adds the Aculab Msg Component type
// PARAMETER   : AnsiTcapMsg lTcapMsg , acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------

BOOLEAN TcapAculab::AddAcuTcapComponet(AnsiTcapMsg lTcapMsg, acu_tcap_msg_t *lMsg)
{
   int lRetVal;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
   DialogueComp      lDialogueComp;
   map<UINT32, DialogueComp>::iterator lDiaComp;
   //ANSI 
   int mStandard = SS7_STANDARD_ANSI ; //temp - ansi test
   memset(&lDialogueComp, 0, sizeof(DialogueComp));

   //sleep(10); // TBR

   lDiaComp = mDialigueComponent.find(lTcapMsg.dialogueId);

   if(lDiaComp != mDialigueComponent.end())
   {
      //lDialogueComp.numberOfComponent = (*lDiaComp).second.numberOfComponent;
      memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
      mDialigueComponent.erase(lTcapMsg.dialogueId);

      //TBR
      /* while(lDialogueComp.numberOfComponent != 4)
         {
         memset(&lDialogueComp, 0, sizeof(DialogueComp));
         lDiaComp = mDialigueComponent.find(lTcapMsg.dialogueId);
         memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
         mDialigueComponent.erase(lTcapMsg.dialogueId);
         }
      //TBR*/
      //sleep(10);

      sprintf(lLogText,
            "AI: DlgId:%d Number of Components Received %d",
            lTcapMsg.dialogueId, lDialogueComp.numberOfComponent);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(ACUTCAP125, 2, lLogText);
   }
   else
   {
      lDialogueComp.numberOfComponent = 1;
      memcpy(&lDialogueComp.tcapComponent[0], &lTcapMsg.tcapComponent, sizeof(TcapComponent));
   }

   for(UINT8 lTemp = 0; lTemp < lDialogueComp.numberOfComponent; lTemp++)
   {
      memcpy(&lTcapMsg.tcapComponent, &lDialogueComp.tcapComponent[lTemp], 
            sizeof(TcapComponent));

       //Replace opcode with null if it is 0xFF/255
       if(lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode == 0xFF)
          lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode = 0x00;

       switch(lTcapMsg.tcapComponent.tcapComp)
       {   
          case TCAP_INVOKE_COMP:
             {
                if(mPegFlag)
                   gPeg.PegEvent(PEG_TCAP_INVOKE_COMP_TX);

                // ANSI TCAP operation class (0-4), bitwise OR with ACU_TCAP_LAST
                // for INVOKE_LAST component
                int lastClass = 1;
                if (lTcapMsg.tcapComponent.lastComponent == TCAP_LAST_COMPONENT) {
                    lastClass |= ACU_TCAP_LAST;
                }

                int invokeId = lTcapMsg.tcapComponent.invokeId;
                if (invokeId == 0) {
                   invokeId = ACU_TCAP_NO_INVOKE_ID;
                }

                // Determine Private vs National operation code encoding
                // Private: pass 2-byte array pointer (Family + Specifier)
                // National: pass NULL and integer code
                UINT8 ansi_priv_op[2];
                UINT8* pOpCode = NULL;
                int lOpCodeOrLen = lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode;

                if (lTcapMsg.tcapComponent.tcapInvokeComp.operation.isPrivate) {
                   ansi_priv_op[0] = (lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode >> 8) & 0xFF;  // Family
                   ansi_priv_op[1] = lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode & 0xFF;         // Specifier
                   pOpCode = ansi_priv_op;
                   lOpCodeOrLen = 2;
                }

                lRetVal = acu_tcap_msg_add_comp_invoke(lMsg,
                      invokeId,
                      ACU_TCAP_NO_INVOKE_ID, // linked_id
                      lastClass,
                      lTcapMsg.tcapComponent.tcapInvokeComp.timeout,
                      pOpCode,       // NULL = National, array pointer = Private
                      lOpCodeOrLen,  // integer code for National, length 2 for Private
                      lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.array,
                      lTcapMsg.tcapComponent.tcapInvokeComp.parameterData.numberOfBytes
                   );

                if(0 != lRetVal)
                {
                   mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                   mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                   sprintf(lLogText,"Add Invoke Componet Error %s",lErrText);
                   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                   gLog.GenerateLog(ACUTCAP07,lLogText);
                   return false;
                }
                break;
             }
          case TCAP_RET_RESULT_COMP:
             {
                if(mPegFlag)
                   gPeg.PegEvent(PEG_TCAP_RET_RESULT_COMP_TX);

                int lLast = ACU_TCAP_LAST;
                if(lTcapMsg.tcapComponent.tcapRetResultComp.lastIndicator == TCAP_NOT_LAST_INDICATOR)
                {
                   lLast = 0;
                }

                // ANSI Return Result does not carry operation code on the wire.
                // Aculab ignores op_code and op_code_len for ANSI Result.
                lRetVal = acu_tcap_msg_add_comp_result(lMsg,
                      lTcapMsg.tcapComponent.invokeId,
                      lLast,
                      NULL,
                      0,
                      lTcapMsg.tcapComponent.tcapRetResultComp.parameterData.array,
                      lTcapMsg.tcapComponent.tcapRetResultComp.parameterData.numberOfBytes);
                if(0 != lRetVal)
                {
                   mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                   mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                   sprintf(lLogText,"Tcap Add Msg Component Result Error %s",lErrText);
                   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                   gLog.GenerateLog(ACUTCAP07,lLogText);
                   return false;
                }

                break;
             }
          case TCAP_RET_ERROR_COMP:
             {
                if(mPegFlag)
                   gPeg.PegEvent(PEG_TCAP_RET_ERROR_COMP_TX);

                // Determine Private vs National error code encoding
                // Same pattern as Invoke: pointer for Private, NULL for National
                UINT8 ansi_priv_err[2];
                UINT8* pErrCode = NULL;
                int lErrCodeOrLen = lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCode;

                if (lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.isPrivate) {
                   ansi_priv_err[0] = (lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCode >> 8) & 0xFF;  // Family
                   ansi_priv_err[1] = lTcapMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCode & 0xFF;         // Specifier
                   pErrCode = ansi_priv_err;
                   lErrCodeOrLen = 2;
                }

                lRetVal = acu_tcap_msg_add_comp_error(lMsg,
                      lTcapMsg.tcapComponent.invokeId,
                      pErrCode,       // NULL = National, array pointer = Private
                      lErrCodeOrLen,  // integer code for National, length 2 for Private
                      lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.array,
                      lTcapMsg.tcapComponent.tcapRetErrorComp.parameterData.numberOfBytes);

                if(0 != lRetVal)
                {
                   mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                   mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                   sprintf(lLogText,"Tcap Add Msg ComponentError Err %s",lErrText);
                   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                   gLog.GenerateLog(ACUTCAP07,lLogText);
                   return false;
                } 
                break;
             }
          case TCAP_REJECT_COMP:
             {
                if(mPegFlag)
                   gPeg.PegEvent(PEG_TCAP_REJECT_COMP_TX);
                acu_tcap_reject_problem_t lProb;

                GetAcuRejectProblem(lProb,
                      lTcapMsg.tcapComponent.tcapRejectComp.problem);

                lRetVal = acu_tcap_msg_add_comp_reject(lMsg,
                      lTcapMsg.tcapComponent.invokeId,
                      lProb,
                      NULL,
                      0);
                if(0 != lRetVal)
                {
                   mDialigueComponent.erase(lTcapMsg.dialogueId); //TBC Mahesh
                   mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                   sprintf(lLogText,"Tcap Add Msg Component Reject Error %s",lErrText);
                   TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                   gLog.GenerateLog(ACUTCAP07,lLogText);
                   return false;
                }
                break;
             }
          case TCAP_ABORT_COMP: 
             {
                acu_tcap_msg_add_ansi_abort_userinfo(
                      lMsg,
                      lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array,
                      lTcapMsg.tcapComponent.tcapAbortComp.abortReason.numberOfBytes
                );
                break;
             }
          default:
             break;
      }   
   }

   return true;
}


//-------------------------------------------------------------------------------
// METHOD      : GetAcuRejectProblem 
// DESCRIPTION : Converts appliaction problem type to aculab tcap problem type
// PARAMETER   : acu_tcap_reject_problem_t &lProb, TCAPProblem &lProblem
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetAcuRejectProblem(acu_tcap_reject_problem_t &lProb,
      TCAPProblem &lProblem)
{
   switch(lProblem.problemCodeType)
   {
      case 1:
         {
            switch(lProblem.problemCode)
            {
               case 0:
                  lProb = ACU_TCAP_REJECT_ITU_GENERAL_UNRECOGNIZED_COMPONENT;
                  break;
               case 1:
                  lProb = ACU_TCAP_REJECT_ITU_GENERAL_MISTYPED_COMPONENT;
                  break;
               case 2:
                  lProb = ACU_TCAP_REJECT_ITU_GENERAL_BADLY_STRUCTURED_COMPONENT;
                  break;
               default:
                  return false;

            }
         }
         break;
      case 2:
         switch(lProblem.problemCode)
         {
            case 0:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_DUPLICATE_INVOKE_ID;
               break;
            case 1:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_OPERATION;
               break;
            case 2:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_MISTYPED_PARAMETER;
               break;
            case 3:
               lProb =ACU_TCAP_REJECT_ITU_INVOKE_RESOURCE_LIMITATION;
               break;
            case 4:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_INITIATING_RELEASE;
               break;
            case 5:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_LINKED_ID;
               break;
            case 6:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_LINKED_RESPONSE_UNEXPECTED;
               break;
               //case 6:
               //   lProb = ACU_TCAP_REJECT_ITU_INVOKE_LINKED_RESPONSE_EXPECTED;
               //   break;
            case 7:
               lProb = ACU_TCAP_REJECT_ITU_INVOKE_UNEXPECTED_LINKED_OPERATION;
               break;
            default: 
               return false;
         }
      case 3:
         switch(lProblem.problemCode)
         {
            case 0:
               lProb = ACU_TCAP_REJECT_ITU_RESULT_UNRECOGNIZED_INVOKE_ID;
               break;
            case 1:
               lProb = ACU_TCAP_REJECT_ITU_RESULT_RETURN_RESULT_UNEXPECTED;
               break;
            case 2:
               lProb = ACU_TCAP_REJECT_ITU_RESULT_MISTYPED_PARAMETER;
               break;
            default:   
               return false;
         }
      case 4:
         switch(lProblem.problemCode)
         {
            case 0:
               lProb = ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_INVOKE_ID;
               break;
            case 1:
               lProb = ACU_TCAP_REJECT_ITU_ERROR_RETURN_ERROR_UNEXPECTED;
               break;
            case 2:
               lProb = ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_ERROR;
               break;
            case 3:
               lProb = ACU_TCAP_REJECT_ITU_ERROR_UNEXPECTED_ERROR;
               break;
            case 4:
               lProb = ACU_TCAP_REJECT_ITU_ERROR_MISTYPED_PARAMETER;
               break;
            default:
               return false;

         }
      default:
         return false;
   }

   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : FillTcapStruct
// DESCRIPTION : Fills application tcap message from aculab tcap 
//               messaga structure
// PARAMETER   : acu_tcap_msg_t *lMsg,TcapMsg &lTcapMsg,  
//               const acu_tcap_dialogue_t *lDlg,
//               TcapComponent *lComp,int &lNoOfComp     
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------

BOOLEAN TcapAculab::FillTcapStruct(acu_tcap_msg_t *lMsg,AnsiTcapMsg &lTcapMsg,
      const acu_tcap_dialogue_t *lDlg,
      TcapComponent *lComp,int &lNoOfComp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = ""; 

   //lTcapMsg.dialogueId = lDlgId;
   lTcapMsg.tcUserId = 0;
   //lTcapMsg.origTransIdLen = 0; 
   //lTcapMsg.destTransIdLen = 0;

   if(false == GetTcapAddress(lTcapMsg.destAddress, LOCAL_ADDR,lMsg))
   {
      sprintf(lLogText,"AI: DlgId:%d lTId:%08X Populating Dest Tcap Address Failed",
            lTcapMsg.dialogueId,lTcapMsg.origTransId);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP126, 2, lLogText);
   }
   else
   {

      sprintf(lLogText,"AI: DlgId:%d lTId:%08X Populating Dest Tcap Address Success",
            lTcapMsg.dialogueId,lTcapMsg.origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP127, 2, lLogText);
   }

   if(false == GetTcapAddress(lTcapMsg.origAddress, REMOTE_ADDR,lMsg))
   {
      sprintf(lLogText,"AI: DlgId:%d lTId:%08X Populating Orig Tcap Address Failed",
            lTcapMsg.dialogueId,lTcapMsg.origTransId);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP128, 2, lLogText);
   }
   else
   {
      sprintf(lLogText,"AI: DlgId:%d lTId:%08X Populating Orig Tcap Address Success",
            lTcapMsg.dialogueId,lTcapMsg.origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP129, 2, lLogText);
   }

   if(lDlg != NULL)
   {
      GetTcapDialogue(lDlg, lTcapMsg);
   }   
   else
   {
      sprintf(lLogText,"AI: DlgId:%d lTId:%08X Dialog Portion Missing",
            lTcapMsg.dialogueId,lTcapMsg.origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP35,lLogText);
   }

   lTcapMsg.tcapDlg = GetTcapDlgType(lMsg);

   if(false == GetTcapComponets(lMsg, lTcapMsg, lDlg, lComp, lNoOfComp))
   {
      return false;
   }

   return true; 
}


//-------------------------------------------------------------------------------
// METHOD      : GetTransIds 
// DESCRIPTION : Get transaction ids from aculab tcap message
// PARAMETER   : acu_tcap_msg_t *lMsg,unsigned int *lLocId,
//               unsigned int *lRemId, unsigned int *rem_id_len)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::GetTransIds(acu_tcap_msg_t *lMsg,unsigned int *lLocId,
      unsigned int *lRemId,
      unsigned int *rem_id_len)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   int   lRetVal = 0;

   acu_tcap_trans_t *lTrans = lMsg->tm_trans;  

   lRetVal =  acu_tcap_trans_get_ids(lTrans, lLocId, lRemId, rem_id_len);

   if(0 != lRetVal)
   {
      //Error
      //printf("Error Code : %d:%s\n",lRetVal,acu_tcap_strerror(lRetVal, 1));
      sprintf(lLogText,"AI: ErrorCode:%d-%s Tcap Get Transaction ID Failed",
            lRetVal, acu_tcap_strerror(lRetVal, 1));
      T(gTrace,printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUTCAP130, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTcapAddress 
// DESCRIPTION : GetTcapAddress from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
/*
BOOLEAN TcapAculab::GetTcapAddress(TCAPAddress &lAddress, 
      EnumAddrFlag lFlag,
      acu_tcap_msg_t *lMsg)
{
   acu_sccp_addr_t * lAddr;


   if(LOCAL_ADDR == lFlag)
   {   
      lAddr = acu_tcap_trans_get_locaddr(lMsg->tm_trans);
   }
   else
   {
      lAddr = acu_tcap_trans_get_remaddr(lMsg->tm_trans);
   }


   if(lAddr->sa_flags  == ACU_SCCP_SA_FLAGS_ROUTE_SSN)
   {
      lAddress.addressIndicator = lAddress.addressIndicator | 0x40;
   }
   else
   {
      lAddress.addressIndicator = lAddress.addressIndicator & 0x00;
      lAddress.numberOfDigits = lAddr->sa_gt.sag_num;

      int lTempCount = 0;
      if(0 == (lAddress.numberOfDigits % 2 ))
      {
         lTempCount = lAddress.numberOfDigits / 2;
      }
      else
      {
         lTempCount = lAddress.numberOfDigits / 2 + 1 ;
      }


      int j = 0;
      for(int i = 0; i < lTempCount; i++)
      {

         lAddress.digits[j++] = lAddr->sa_gt.sag_digits[i] & 0x0F;
         lAddress.digits[j++] = (lAddr->sa_gt.sag_digits[i] >> 4) & 0x0F;
      }
   }


   if(lAddr->sa_valid & ACU_SCCP_SA_VALID_PC)
   {
      lAddress.pointCode = lAddr->sa_pc;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x01;
   }

   if(lAddr->sa_valid & ACU_SCCP_SA_VALID_SSN)
   {
      lAddress.subsystemNumber = lAddr->sa_ssn;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x02;
   }

   if(lAddr->sa_valid & ( ACU_SCCP_SA_VALID_TT |
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES |
            ACU_SCCP_SA_VALID_NAI))
   {
      lAddress.addressIndicator = lAddress.addressIndicator | 0x10;
      lAddress.natureOfAddress = lAddr->sa_nai;
      lAddress.translationType = lAddr->sa_tt;
      lAddress.numberingPlan   = lAddr->sa_np;
      lAddress.encodingScheme  = lAddr->sa_es;
   }
   else if(lAddr->sa_valid & (ACU_SCCP_SA_VALID_TT | 
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES))
   {
      lAddress.addressIndicator = lAddress.addressIndicator | 0x0C;
      lAddress.translationType = lAddr->sa_tt;
      lAddress.numberingPlan   = lAddr->sa_np;
      lAddress.encodingScheme  = lAddr->sa_es;
   }
   else if(lAddr->sa_valid & ACU_SCCP_SA_VALID_NAI)
   {
      lAddress.natureOfAddress = lAddr->sa_nai;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x04;
   }
   else if(lAddr->sa_valid & ACU_SCCP_SA_VALID_TT)
   {
      lAddress.translationType = lAddr->sa_tt;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x08;

   }

   return true;
}
*/

BOOLEAN TcapAculab::GetTcapAddress(TCAPAddress &lAddress, 
      EnumAddrFlag lFlag,
      acu_tcap_msg_t *lMsg)
{
   acu_sccp_addr_t * lAddr;


   if(LOCAL_ADDR == lFlag)
   {   
      lAddr = acu_tcap_trans_get_locaddr(lMsg->tm_trans);
   }
   else
   {
      lAddr = acu_tcap_trans_get_remaddr(lMsg->tm_trans);
   }


   if(lAddr->sa_flags  == ACU_SCCP_SA_FLAGS_ROUTE_SSN)
   {
      lAddress.addressIndicator = lAddress.addressIndicator | 0x40;
   }
   else
   {
      lAddress.addressIndicator = lAddress.addressIndicator & 0x00;
      lAddress.numberOfDigits = lAddr->sa_gt.sag_num;

      int lTempCount = 0;
      if(0 == (lAddress.numberOfDigits % 2 ))
      {
         lTempCount = lAddress.numberOfDigits / 2;
      }
      else
      {
         lTempCount = lAddress.numberOfDigits / 2 + 1 ;
      }


      int j = 0;
      for(int i = 0; i < lTempCount; i++)
      {

         lAddress.digits[j++] = lAddr->sa_gt.sag_digits[i] & 0x0F;
         lAddress.digits[j++] = (lAddr->sa_gt.sag_digits[i] >> 4) & 0x0F;
      }
   }


   // ANSI: Bit 2 (0x02) is PC Indicator, Bit 1 (0x01) is SSN Indicator
   if(lAddr->sa_valid & ACU_SCCP_SA_VALID_PC)
   {
      lAddress.pointCode = lAddr->sa_pc;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x02;
   }

   if(lAddr->sa_valid & ACU_SCCP_SA_VALID_SSN)
   {
      lAddress.subsystemNumber = lAddr->sa_ssn;
      lAddress.addressIndicator = lAddress.addressIndicator | 0x01;
   }

   // In ANSI SCCP, NAI is not part of the SCCP address header, so we omit
   // decoding ACU_SCCP_SA_VALID_NAI.
   
   if(lAddr->sa_valid & (ACU_SCCP_SA_VALID_TT | 
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES))
   {
      // If TT, NP, ES are present under ANSI, set 0x10 or 0x0c (default to 0x10)
      lAddress.addressIndicator = lAddress.addressIndicator | 0x10;
      lAddress.translationType = lAddr->sa_tt;
      lAddress.numberingPlan   = lAddr->sa_np;
      lAddress.encodingScheme  = lAddr->sa_es;
   }
   else if(lAddr->sa_valid & ACU_SCCP_SA_VALID_TT)
   {
      // If only TT is present under ANSI, set 0x04 (GTI = 1)
      lAddress.addressIndicator = lAddress.addressIndicator | 0x04;
      lAddress.translationType = lAddr->sa_tt;
   }

   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : SetLocTcapAddress 
// DESCRIPTION : SetLocTcapAddress from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetLocTcapAddress(acu_tcap_msg_t *lMsg,
      TCAPAddress &lTcapAddress)
{

   acu_sccp_addr_t * lAddr = acu_tcap_trans_get_locaddr(lMsg->tm_trans);
   if(NULL != lAddr)
      return SetAddress(lAddr,lTcapAddress);

   return false;
}

//-------------------------------------------------------------------------------
// METHOD      : SetLocTcapAddress 
// DESCRIPTION : SetLocTcapAddress from aculab tcap adress
// PARAMETER   : acu_tcap_msg_t, acu_sccp_addr_t 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetLocTcapAddress(acu_tcap_msg_t *lMsg, acu_sccp_addr_t **lAddr)
{
   *lAddr = acu_tcap_trans_get_locaddr(lMsg->tm_trans);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetRemTcapAddress 
// DESCRIPTION : SetRemTcapAddress from aculab tcap adress
// PARAMETER   : acu_tcap_msg_t, acu_sccp_addr_t
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetRemTcapAddress(acu_tcap_msg_t *lMsg, acu_sccp_addr_t **lAddr)
{
   *lAddr = acu_tcap_trans_get_remaddr(lMsg->tm_trans); 
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : SetLocSccpAddr 
// DESCRIPTION : SetLocSccpAddr from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetLocSccpAddr(TCAPAddress &lTcapAddress,int &lInstanceNo)
{
   acu_sccp_addr_t *lAddr = acu_tcap_ssap_get_locaddr(mSsapPtr[lInstanceNo]);
   if(lAddr != NULL)
   {
      return SetAddress(lAddr,lTcapAddress);
   }
   else
   {
      return false;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : SetRemSccpAddr 
// DESCRIPTION : SetRemSccpAddr from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::SetRemSccpAddr(TCAPAddress &lTcapAddress,int &lInstanceNo)
{

   acu_sccp_addr_t *lAddr = acu_tcap_ssap_get_remaddr(mSsapPtr[lInstanceNo]);
   if(lAddr != NULL)
      return SetAddress(lAddr,lTcapAddress);
   else
      return false;
}

//-------------------------------------------------------------------------------
// METHOD      : SetRemTcapAddress 
// DESCRIPTION : SetRemTcapAddress from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------

BOOLEAN TcapAculab::SetRemTcapAddress(acu_tcap_msg_t *lMsg,
      TCAPAddress &lAddress)
{
   acu_sccp_addr_t *lAddr = acu_tcap_trans_get_remaddr(lMsg->tm_trans); 
   if(lAddr != NULL)
      return SetAddress(lAddr,lAddress);
   else
      return false;
}

/*
BOOLEAN TcapAculab::SetAddress(acu_sccp_addr_t *lAddr,
      TCAPAddress &lAddress)
{
   // printf("Address Indicator : %02X\n", lAddress.addressIndicator);

   if((lAddress.addressIndicator & 0x40) == 0)
   {
      // EXPLICITLY clear ACU_SCCP_SA_FLAGS_RAW_GT for ANSI.
      lAddr->sa_flags &= ~ACU_SCCP_SA_FLAGS_RAW_GT;
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_GTI;
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_NAI;

      lAddr->sa_gt.sag_num = lAddress.numberOfDigits;
      int i = 0;

      int lTemcount = 0;
      if(0 == (lAddress.numberOfDigits % 2 ))
      {
         lTemcount = lAddress.numberOfDigits / 2;
      }
      else
      {
         lTemcount = lAddress.numberOfDigits / 2 + 1;
      }

      for(int j = 0; j < lTemcount ;)
      {
         lAddr->sa_gt.sag_digits[j] = 0x0F & lAddress.digits[i++];
         lAddr->sa_gt.sag_digits[j] |= (0xF0 & (lAddress.digits[i++] << 4));
         j++;
      }

      if(SCCP_ES_BCD_ODD == lAddress.encodingScheme)
      {
         lAddr->sa_gt.sag_digits[lTemcount - 1] = lAddr->sa_gt.sag_digits[lTemcount - 1] & 0x0F;
      }

   }
   else
   {
      lAddr->sa_flags = ACU_SCCP_SA_FLAGS_ROUTE_SSN;
   }

   // ANSI: Bit 2 (0x02) is PC Indicator, Bit 1 (0x01) is SSN Indicator
   if(lAddress.addressIndicator & 0x02 || lAddress.pointCode != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_PC;
      lAddr->sa_pc = lAddress.pointCode;
   }
   if(lAddress.addressIndicator & 0x01 || lAddress.subsystemNumber != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_SSN;
      lAddr->sa_ssn = lAddress.subsystemNumber;
   }

   // In ANSI SCCP, NAI is not part of the SCCP address header, so we omit
   // setting ACU_SCCP_SA_VALID_NAI even if natureOfAddress is provided.
   
   // GTI = 1 (0x04): Translation Type only
   if(lAddress.addressIndicator & 0x04 )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT;
      lAddr->sa_tt = lAddress.translationType;
   }
   // GTI = 2 (0x08): Translation Type only
   if(lAddress.addressIndicator & 0x08 )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT;
      lAddr->sa_tt = lAddress.translationType;
   }
   // GTI = 3 (0x0c): Translation Type, Numbering Plan, Encoding Scheme
   if(lAddress.addressIndicator & 0x0c )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT 
         | ACU_SCCP_SA_VALID_NP
         | ACU_SCCP_SA_VALID_ES;
      lAddr->sa_tt = lAddress.translationType;
      lAddr->sa_np = lAddress.numberingPlan;
      lAddr->sa_es = lAddress.encodingScheme;
   }
   // GTI = 4 (0x10): Translation Type, Numbering Plan, Encoding Scheme
   if(lAddress.addressIndicator & 0x10 )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT
         | ACU_SCCP_SA_VALID_NP
         | ACU_SCCP_SA_VALID_ES;
      lAddr->sa_tt = lAddress.translationType;
      lAddr->sa_np = lAddress.numberingPlan;
      lAddr->sa_es = lAddress.encodingScheme;
   }

   return true;
}
*/
//-------------------------------------------------------------------------------
// METHOD      : SetAddress 
// DESCRIPTION : SetAddress from aculab tcap adress
// PARAMETER   : TCAPAddress &lAddress, EnumAddrFlag lFlag,acu_tcap_msg_t *lMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
/*
BOOLEAN TcapAculab::SetAddress(acu_sccp_addr_t *lAddr,
      TCAPAddress &lAddress)
{
   // printf("Address Indicator : %02X\n", lAddress.addressIndicator);

   if((lAddress.addressIndicator & 0x40) == 0)
   {
      // EXPLICITLY clear ACU_SCCP_SA_FLAGS_RAW_GT.
      // The Aculab SSAP may pre-set this flag from its config file.
      // If left set, the stack treats sag_digits[] as a pre-formatted raw byte
      // stream and will NOT prepend TT/NP/ES/NAI header bytes on the wire.
      // Clearing it tells the stack to build the GT header bytes from sa_tt,
      // sa_np, sa_es, sa_nai automatically when sending the SCCP UDT.
      lAddr->sa_flags &= ~ACU_SCCP_SA_FLAGS_RAW_GT;

      // EXPLICITLY clear ACU_SCCP_SA_VALID_GTI from sa_valid.
      // The Aculab SSAP pre-initializes sa_valid = 0xf7 (all bits set) from
      // its Tcap_1000_8.cfg config file, including VALID_GTI (bit 0).
      // VALID_GTI tells the encoder "use the raw sa_gti field for GTI".
      // Having VALID_GTI set *alongside* VALID_TT/NP/ES/NAI is ambiguous;
      // the SCCP encoder rejects it with "cannot convert supplied sa_valid to
      // a gti".  Clearing it leaves TT+NP+ES+NAI as the sole source of truth
      // so the encoder auto-selects GTI-4 (ANSI) and builds the GT header.
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_GTI;
      
      // EXPLICITLY clear ACU_SCCP_SA_VALID_NAI for ANSI.
      // ANSI SCCP (unlike ITU) does not support NAI in the GT header.
      // Aculab's default sa_valid=0xf7 includes VALID_NAI. If left set,
      // the encoder will throw "cannot convert supplied sa_valid to a gti"
      // because there is no ANSI GTI that includes NAI.
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_NAI;

      lAddr->sa_gt.sag_num = lAddress.numberOfDigits;
      int i = 0;

      int lTemcount = 0;
      if(0 == (lAddress.numberOfDigits % 2 ))
      {
         lTemcount = lAddress.numberOfDigits / 2;
      }
      else
      {
         lTemcount = lAddress.numberOfDigits / 2 + 1;
      }

      for(int j = 0; j < lTemcount ;)
      {
         lAddr->sa_gt.sag_digits[j] = 0x0F & lAddress.digits[i++];
         lAddr->sa_gt.sag_digits[j] |= (0xF0 & (lAddress.digits[i++] << 4));
         j++;
      }

      if(SCCP_ES_BCD_ODD == lAddress.encodingScheme)
      {
         lAddr->sa_gt.sag_digits[lTemcount - 1] = lAddr->sa_gt.sag_digits[lTemcount - 1] & 0x0F;
      }

   }
   else
   {
      lAddr->sa_flags = ACU_SCCP_SA_FLAGS_ROUTE_SSN;
   }

   if(lAddress.addressIndicator & 0x01 || lAddress.pointCode != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_PC;
      lAddr->sa_pc = lAddress.pointCode;
   
   }

   if(lAddress.addressIndicator & 0x02 || lAddress.subsystemNumber != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_SSN;
      lAddr->sa_ssn = lAddress.subsystemNumber;
   }

   // In ANSI SCCP, NAI is not part of the SCCP address header, so we omit
   // setting ACU_SCCP_SA_VALID_NAI even if natureOfAddress is provided.
   if(lAddress.addressIndicator & 0x08 )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT;
      lAddr->sa_tt = lAddress.translationType;
   }
   if(lAddress.addressIndicator & 0x0c )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT 
         |ACU_SCCP_SA_VALID_NP
         |ACU_SCCP_SA_VALID_ES;
      lAddr->sa_tt = lAddress.translationType;
      lAddr->sa_np = lAddress.numberingPlan;
      lAddr->sa_es = lAddress.encodingScheme;
   }
   if(lAddress.addressIndicator & 0x10 )
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT
         | ACU_SCCP_SA_VALID_NP
         | ACU_SCCP_SA_VALID_ES;
      lAddr->sa_tt = lAddress.translationType;
      lAddr->sa_np = lAddress.numberingPlan;
      lAddr->sa_es = lAddress.encodingScheme;
      // sa_nai is intentionally omitted for ANSI.
   }

   // No NAI fallback logic for ANSI.

   return true;
}
*/



BOOLEAN TcapAculab::SetAddress(acu_sccp_addr_t *lAddr,
      TCAPAddress &lAddress)
{
   // printf("Address Indicator : %02X\n", lAddress.addressIndicator);

   if((lAddress.addressIndicator & 0x40) == 0)
   {
      // EXPLICITLY clear ACU_SCCP_SA_FLAGS_RAW_GT.
      // The Aculab SSAP may pre-set this flag from its config file.
      // If left set, the stack treats sag_digits[] as a pre-formatted raw byte
      // stream and will NOT prepend TT/NP/ES/NAI header bytes on the wire.
      // Clearing it tells the stack to build the GT header bytes from sa_tt,
      // sa_np, sa_es, sa_nai automatically when sending the SCCP UDT.
      lAddr->sa_flags &= ~ACU_SCCP_SA_FLAGS_RAW_GT;

      // EXPLICITLY clear ACU_SCCP_SA_VALID_GTI from sa_valid.
      // The Aculab SSAP pre-initializes sa_valid = 0xf7 (all bits set) from
      // its Tcap_1000_8.cfg config file, including VALID_GTI (bit 0).
      // VALID_GTI tells the encoder "use the raw sa_gti field for GTI".
      // Having VALID_GTI set *alongside* VALID_TT/NP/ES/NAI is ambiguous;
      // the SCCP encoder rejects it with "cannot convert supplied sa_valid to
      // a gti".  Clearing it leaves TT+NP+ES+NAI as the sole source of truth
      // so the encoder auto-selects GTI-4 (ANSI) and builds the GT header.
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_GTI;

      // EXPLICITLY clear ACU_SCCP_SA_VALID_NAI for ANSI.
      // ANSI SCCP (unlike ITU) does not support NAI in the GT header.
      // Aculab's default sa_valid=0xf7 includes VALID_NAI. If left set,
      // the encoder will throw "cannot convert supplied sa_valid to a gti"
      // because there is no ANSI GTI that includes NAI.
      lAddr->sa_valid &= ~ACU_SCCP_SA_VALID_NAI;

      lAddr->sa_gt.sag_num = lAddress.numberOfDigits;
      int i = 0;

      int lTemcount = 0;
      if(0 == (lAddress.numberOfDigits % 2 ))
      {
         lTemcount = lAddress.numberOfDigits / 2;
      }
      else
      {
         lTemcount = lAddress.numberOfDigits / 2 + 1;
      }

      for(int j = 0; j < lTemcount ;)
      {
         lAddr->sa_gt.sag_digits[j] = 0x0F & lAddress.digits[i++];
         lAddr->sa_gt.sag_digits[j] |= (0xF0 & (lAddress.digits[i++] << 4));
         j++;
      }

      if(SCCP_ES_BCD_ODD == lAddress.encodingScheme)
      {
         lAddr->sa_gt.sag_digits[lTemcount - 1] = lAddr->sa_gt.sag_digits[lTemcount - 1] & 0x0F;
      }

   }
   else
   {
      lAddr->sa_flags = ACU_SCCP_SA_FLAGS_ROUTE_SSN;
   }

   // ANSI: Bit 2 (0x02) is PC Indicator, Bit 1 (0x01) is SSN Indicator
   if(lAddress.addressIndicator & 0x02 || lAddress.pointCode != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_PC;
      lAddr->sa_pc = lAddress.pointCode;
   }

   if(lAddress.addressIndicator & 0x01 || lAddress.subsystemNumber != 0)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_SSN;
      lAddr->sa_ssn = lAddress.subsystemNumber;
   }

   // Extract ANSI GTI value (bits 3-6)
   int gti = (lAddress.addressIndicator >> 2) & 0x0f;

   if(gti == 1) // GTI = 1 (Translation Type only)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT;
      lAddr->sa_tt = lAddress.translationType;
   }
   else if(gti == 2 || gti == 3) // GTI = 2 or 3 (Translation Type, Numbering Plan, Encoding Scheme)
   {
      lAddr->sa_valid = lAddr->sa_valid | ACU_SCCP_SA_VALID_TT
         | ACU_SCCP_SA_VALID_NP
         | ACU_SCCP_SA_VALID_ES;
      lAddr->sa_tt = lAddress.translationType;
      lAddr->sa_np = lAddress.numberingPlan;
      lAddr->sa_es = lAddress.encodingScheme;
   }

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "ANSI SetAddress Debug: addressIndicator=0x%02X, sa_flags=0x%04X, sa_valid=0x%02X, sa_pc=%d, sa_ssn=%d, sa_tt=%d, sa_np=%d, sa_es=%d, gti=%d",
         lAddress.addressIndicator, lAddr->sa_flags, lAddr->sa_valid, lAddr->sa_pc, lAddr->sa_ssn, lAddr->sa_tt, lAddr->sa_np, lAddr->sa_es, gti);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACUTCAP127, 2, lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetAcuTcapMsgType 
// DESCRIPTION : Get Aculab Tcap message type from application
//               TCAP Message
// PARAMETER   : AnsiTcapMsg lTcapMsg
// RETURN      : acu_tcap_msg_type_t
//-------------------------------------------------------------------------------

// ACU_TCAP_MSG_ANSI_QUERY  ➔  ACU_TCAP_MSG_ITU_BEGIN
// ACU_TCAP_MSG_ANSI_QUERY_WO  ➔  ACU_TCAP_MSG_ITU_BEGIN
// ACU_TCAP_MSG_ANSI_CONV  ➔  ACU_TCAP_MSG_ITU_CONTINUE
// ACU_TCAP_MSG_ANSI_CONV_WO  ➔  ACU_TCAP_MSG_ITU_CONTINUE
// ACU_TCAP_MSG_ANSI_RESPONSE  ➔  ACU_TCAP_MSG_ITU_END
// ACU_TCAP_MSG_ANSI_ABORT  ➔  ACU_TCAP_MSG_ITU_ABORT
// ACU_TCAP_MSG_ANSI_UNI  ➔  ACU_TCAP_MSG_ITU_UNI

acu_tcap_msg_type_t TcapAculab::GetAcuTcapMsgType(AnsiTcapMsg &lTcapMsg) 
{
   switch(lTcapMsg.tcapDlg)
   {   

      //ANSI -  
      case TCAP_ANSI_QUERY_WITH_PERMISSION:
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_BEGIN_TX);
            return ACU_TCAP_MSG_ANSI_QUERY;
         }
      case TCAP_ANSI_QUERY_WITHOUT_PERMISSION:
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_BEGIN_TX);
            return ACU_TCAP_MSG_ANSI_QUERY_WO;
         }
      case TCAP_ANSI_RESPONSE :
      case TCAP_PRE_ARRANGED_END:   // = 1, // Pre arranged value should be 1 as per tdapi
      case TCAP_END_WITH_ADDRESS:   // = 9,
      case TCAP_END:                // = 2,        // basic end should have value as 2
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_END_TX);
            return ACU_TCAP_MSG_ANSI_RESPONSE; 
         }
      case TCAP_ANSI_CONVERSATION_WITH_PERMISSION:           // = 3,
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_CONTINUE_TX);
            return ACU_TCAP_MSG_ANSI_CONV;

         }
      
            case TCAP_ANSI_CONVERSATION_WITHOUT_PERMISSION:           // = 3,
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_CONTINUE_TX);
            return ACU_TCAP_MSG_ANSI_CONV_WO;

         }   
      case TCAP_ANSI_ABORT:              // = 4,
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_USER_ABORT_TX);
            return ACU_TCAP_MSG_ANSI_ABORT;

         }
      case TCAP_RSP_TIMEOUT:        // = 5,
         {
            return ACU_TCAP_MSG_TIMEOUT;
         }
      case TCAP_ANSI_UNI:     // = 7,
         {

            return ACU_TCAP_MSG_ANSI_UNI;
         }
      case TCAP_CONTROL:            // = 6,
      default:
         {
            return ACU_TCAP_MSG_TIMEOUT;
         }

   }

}

//-------------------------------------------------------------------------------
// METHOD      : GetTcapDlgType 
// DESCRIPTION : Get application TCAP dlg type from aculab tcap dlg type
// PARAMETER   : acu_tcap_msg *
// RETURN      : EnumTcapDlg
//-------------------------------------------------------------------------------
/*
EnumTcapDlg TcapAculab::GetTcapDlgType(acu_tcap_msg *lMsg)
{

   switch(lMsg->tm_msg_type)
   {

      case ACU_TCAP_MSG_TIMEOUT:
         return TCAP_RSP_TIMEOUT;
      case ACU_TCAP_MSG_P_ABORT:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_PROTOCOL_ABORT_RX);
         return TCAP_ABORT;

      case ACU_TCAP_MSG_LOCAL_ABORT:  
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_USER_ABORT_RX);
         return TCAP_ABORT;

      case ACU_TCAP_MSG_ITU_ABORT:
         return TCAP_ABORT;

      case ACU_TCAP_MSG_ITU_UNI:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_UNIDIRECTIONAL_RX);
         return TCAP_UNIDIRECTIONAL;

      //ANSI -
      case ACU_TCAP_MSG_ANSI_QUERY:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_BEGIN_RX);
         return TCAP_ANSI_QUERY_WITH_PERMISSION;
      case ACU_TCAP_MSG_ANSI_QUERY_WO:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_BEGIN_RX);
         return TCAP_ANSI_QUERY_WITHOUT_PERMISSION;
       //UPTO ANSI -

      case ACU_TCAP_MSG_ITU_BEGIN:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_BEGIN_RX);
         return TCAP_BEGIN;
      case ACU_TCAP_MSG_ITU_END: 
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_END_RX);
         return TCAP_END;
      case ACU_TCAP_MSG_ITU_CONTINUE:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_CONTINUE_RX);
         return TCAP_CONTINUE;
      default:
         return TCAP_END; // Not mapped
   }    
}
*/


EnumTcapDlg TcapAculab::GetTcapDlgType(acu_tcap_msg *lMsg)
{

   switch(lMsg->tm_msg_type)
   {

      case ACU_TCAP_MSG_TIMEOUT:
         return TCAP_RSP_TIMEOUT;
         /* Message types generated by acu_tcap_msg_decode() */
      case ACU_TCAP_MSG_P_ABORT:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_PROTOCOL_ABORT_RX);
         return TCAP_ANSI_ABORT;

      case ACU_TCAP_MSG_LOCAL_ABORT:  
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_USER_ABORT_RX);
         return TCAP_ANSI_ABORT;

      case ACU_TCAP_MSG_ANSI_ABORT:
         return TCAP_ANSI_ABORT;

      case ACU_TCAP_MSG_ANSI_UNI:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_UNIDIRECTIONAL_RX);
         return TCAP_UNIDIRECTIONAL;

      case ACU_TCAP_MSG_ANSI_QUERY:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_BEGIN_RX);
         return TCAP_ANSI_QUERY_WITH_PERMISSION;

      case ACU_TCAP_MSG_ANSI_QUERY_WO:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_BEGIN_RX);
         return TCAP_ANSI_QUERY_WITHOUT_PERMISSION;

      case ACU_TCAP_MSG_ANSI_RESPONSE:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_END_RX);
         return TCAP_ANSI_RESPONSE;

      case ACU_TCAP_MSG_ANSI_CONV:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_CONTINUE_RX);
         return TCAP_ANSI_CONVERSATION_WITH_PERMISSION;

      case ACU_TCAP_MSG_ANSI_CONV_WO:
         if(mPegFlag)
            gPeg.PegEvent(PEG_TCAP_CONTINUE_RX);
         return TCAP_ANSI_CONVERSATION_WITHOUT_PERMISSION;

      default:
         return TCAP_ANSI_RESPONSE; // default mapped
   }    
}
//-------------------------------------------------------------------------------
// METHOD      : SetRcvLocalAddress 
// DESCRIPTION : Sets the Received Lcoal Address for TCAP Structure
// PARAMETER   : acu_tcap_msg_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
void TcapAculab::SetRcvLocalAddress(acu_tcap_msg_t *rAcuTcapMsgPtr)
{
   // Added to populate Local Address (Destination Address) for incoming BEGIN Message
   // Komal : to be verified
   if(rAcuTcapMsgPtr != NULL)
      *acu_tcap_trans_get_locaddr(rAcuTcapMsgPtr->tm_trans) = *rAcuTcapMsgPtr->tm_local_addr;
}
//-------------------------------------------------------------------------------
// METHOD      : HandlerMultipleComponents 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculab::HandlerMultipleComponents(AnsiTcapMsg lTcapMsg)
{
   pthread_mutex_lock (&gMutexLock);

   DialogueComp      lDialogueComp;
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&lDialogueComp, 0, sizeof(DialogueComp));

   map<UINT32, DialogueComp>::iterator lDiaComp;

   lDiaComp = mDialigueComponent.find(lTcapMsg.dialogueId);

   if(lDiaComp != mDialigueComponent.end())
   {
      sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d Dialog Component Present in Map", 
            lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP49,lLogText);

      if (lTcapMsg.tcapComponent.lastComponent == TCAP_LAST_COMPONENT)
      {

         memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
         mDialigueComponent.erase(lTcapMsg.dialogueId);

         if(lDialogueComp.numberOfComponent == 0)
         {
            sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d TotComponent:%d NumberOfComponent:%d Wrong Info Present in Map (last)", 
                  lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, 
                  lTcapMsg.tcUserId, lDialogueComp.totComponent, lDialogueComp.numberOfComponent);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP172,lLogText);

            memset(&lDialogueComp, 0, sizeof(DialogueComp));
         }

         memcpy(&lDialogueComp.tcapComponent[lTcapMsg.tcUserId], &lTcapMsg.tcapComponent,
               sizeof(TcapComponent));
         lDialogueComp.numberOfComponent = lDialogueComp.numberOfComponent + 1;
         lDialogueComp.totComponent = lTcapMsg.tcUserId + 1;
         lDialogueComp.lastComponentFlag = 1;
         mDialigueComponent.insert(std::pair<UINT32, DialogueComp>(lTcapMsg.dialogueId, lDialogueComp));

         sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d TotComponent:%d NumberOfComponent:%d Received Multiple Components, Last Component", 
               lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, 
               lTcapMsg.tcUserId, lDialogueComp.totComponent, lDialogueComp.numberOfComponent);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP44,lLogText);


         if(lDialogueComp.totComponent != lDialogueComp.numberOfComponent)
         {
            sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d TotComponent:%d NumberOfComponent:%d Total Components not Received", 
                  lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, 
                  lTcapMsg.tcUserId, lDialogueComp.totComponent, lDialogueComp.numberOfComponent);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP45,lLogText);

            pthread_mutex_unlock (&gMutexLock);
            return true;
         }
      }
      else
      {

         memcpy(&lDialogueComp, &lDiaComp->second, sizeof(DialogueComp));
         mDialigueComponent.erase(lTcapMsg.dialogueId);

         if(lDialogueComp.numberOfComponent == 0)
         {
            sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d TotComponent:%d NumberOfComponent:%d Wrong Info Present in Map (not last)", 
                  lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, 
                  lTcapMsg.tcUserId, lDialogueComp.totComponent, lDialogueComp.numberOfComponent);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP173,lLogText);

            memset(&lDialogueComp, 0, sizeof(DialogueComp));
         }

         memcpy(&lDialogueComp.tcapComponent[lTcapMsg.tcUserId], &lTcapMsg.tcapComponent,
               sizeof(TcapComponent));

         lDialogueComp.numberOfComponent = lDialogueComp.numberOfComponent + 1;
         lDialogueComp.totComponent = lTcapMsg.tcUserId + 1;

         mDialigueComponent.insert(std::pair<UINT32, DialogueComp>(lTcapMsg.dialogueId, lDialogueComp));

         sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d TotComponent:%d NumberOfComponent:%d LastComponentFlag:%d  Received Multiple Components, not last Component", lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId, lDialogueComp.totComponent, lDialogueComp.numberOfComponent, lDialogueComp.lastComponentFlag);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP46,lLogText);


         if(lTcapMsg.tcapComponent.lastComponent == TCAP_NOT_LAST_COMPONENT)
         {
            if((lDialogueComp.lastComponentFlag == 1) && (lDialogueComp.numberOfComponent == lDialogueComp.totComponent))
            {
               sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d All Components Received", 
                     lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP47,lLogText);

               pthread_mutex_unlock (&gMutexLock);
               return false;
            }
            else
            {
               sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d All Components not Received", 
                     lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP48,lLogText);

               pthread_mutex_unlock (&gMutexLock);
               return true;
            }
         }
      }
   }
   else
   {
      sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d Dialog Component not Present in Map", 
            lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP50,lLogText);

      if(lTcapMsg.tcapComponent.lastComponent == TCAP_NOT_LAST_COMPONENT)
      {
         memcpy(&lDialogueComp.tcapComponent[lTcapMsg.tcUserId], &lTcapMsg.tcapComponent,
               sizeof(TcapComponent));
         lDialogueComp.numberOfComponent = lDialogueComp.numberOfComponent + 1;
         lDialogueComp.totComponent = lTcapMsg.tcUserId + 1;
         mDialigueComponent.insert(std::pair<UINT32, DialogueComp>(lTcapMsg.dialogueId, lDialogueComp));

         sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d NumberOfComponent:%d not Last Component", 
               lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId, lDialogueComp.numberOfComponent);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP51,lLogText);

         pthread_mutex_unlock (&gMutexLock);
         return true;
      }
      else if(lTcapMsg.tcapComponent.lastComponent == TCAP_LAST_COMPONENT)
      {
         sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d NumberOfComponent:%d Last Component, not Present in Map", 
               lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId, lDialogueComp.numberOfComponent);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP50, 2, lLogText);
      }
   }

   sprintf(lLogText,"AI: DlgId:%d LastComp:%d TcUserId:%d Returning components", 
         lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP52,lLogText);

   pthread_mutex_unlock (&gMutexLock);
   return false;
}
