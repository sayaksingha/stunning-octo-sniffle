static const char *id = "@(#) $Id: TcapAculabHandler.cc,v 1.1.4.9.2.1.2.4 2023/01/31 13:51:09 mram Exp $";
//----------------------------------------------------------------------------
// NAME :TcapAculabHandler.cc 
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
// Originated : 23-JAN-2010                                           Paresh
//----------------------------------------------------------------------------

#include "TcapAculabHandler.h"

extern Log    gLog;
extern CTrace gTrace;
extern TEXT   gProcessName[ACU_TCAP_MAX_PROCESS_NAME];
extern UINT32 gMsgCounter;
extern BOOLEAN gRestoreStatus;

pthread_mutex_t  gPthreadMutexLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t   gPthreadCondLock = PTHREAD_COND_INITIALIZER;

extern SsapDetailsStruct gSsapDetails;

//-------------------------------------------------------------------------------
// METHOD      : TcapAculabHandler
// DESCRIPTION : Default Constructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
TcapAculabHandler::TcapAculabHandler()
{
   mNumofOPCs = 0;
   gMsgCounter = 1;
   mNoOfInstance = 0;
   gettimeofday(&mStartTime, NULL);
   mEndTime.tv_sec = mStartTime.tv_sec + 5;
   mEndTime.tv_usec = mStartTime.tv_usec;
}

//-------------------------------------------------------------------------------
// METHOD      : TcapAculabHandler 
// DESCRIPTION : Parameterised Constructor
// PARAMETER   : TEXT *, TEXT *
// RETURN      : None
//-------------------------------------------------------------------------------
TcapAculabHandler::TcapAculabHandler(TEXT *lProcName,
      TEXT *lTraceEnv)
{
   gMsgCounter = 1;
   mNumofOPCs = 0;
   mNoOfInstance = 0;
   gettimeofday(&mStartTime, NULL);
   mEndTime.tv_sec = mStartTime.tv_sec + 5;
   mEndTime.tv_usec = mStartTime.tv_usec;
}
//-------------------------------------------------------------------------------
// METHOD      : ~TcapAculabHandler 
// DESCRIPTION : Destructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
TcapAculabHandler::~TcapAculabHandler()
{

}
//-------------------------------------------------------------------------------
// METHOD      : Init 
// DESCRIPTION : Initializes all library and configuration
// PARAMETER   : TEXT *, int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::Init(TEXT *lCfgFile, 
      int lMsgType,
      int& lNoOfInstance)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";

   mMsgType = lMsgType;

   //if(false == gLog.Init())
   //{
   //   return false;
   // }
   if(false == ReadConfig(lNoOfInstance))
   {
      return false;
   }
   if(false == DecryptLicKey())
   {
      return false; // TBR remove after fixing license issue
   }   
   mTransValidationKey = GetTransValidationKey();

   if(false == mTransDlgMap.Init(mTransValidationKey, mMsgType))
   {
      return false;
   }
   if(false ==  mAculabApi.Init(lCfgFile,mPegFlag,mNoOfInstance,mTransValidationKey,mMsgType))
   {
      return false;
   }
   if(false == CreateMsgQ())
   {
      return false;
   }

   if(false == mDlgManager.Init())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "MN: Dialogue Manager Initialization failed. Exiting...");
      T(gTrace,
            printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04,lLogText);
      return false;
   }

   gRestoreStatus = true;

   if(mRestorationFlag)
   {
      gRestoreStatus = false;
      sleep(1);
      this->GetDlgInfoAndRestore();
   }
   else
   {
      int         lSsn=0;
      time_t      lInsertTime;
      for(unsigned int i = 1; i <= mMaxDlgSize; i++)
      {
         mDlgManager.GetInsertTimeAndSsn(i, lInsertTime, lSsn);
         
         if(lSsn == mMsgType)
         {
            mDlgManager.FreeDlgInfo(i); 

            snprintf(lLogText, MAX_LOG_TEXT_LEN, "dlgId : %u and Ssn (%d), Shared memory clered \n", i, lSsn);
            T(gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(GSYS04,lLogText);
         }

      }
   }   
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetDlgInfoAndRestore 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::GetDlgInfoAndRestore()
{
   time_t      lCurrTime;
   time_t      lInsertTime;
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";

   acu_tcap_trans_t  *lTransPtr=NULL;
   DlgRecord         lDlgRcd;
   int         lInstanceNo = 0;
   int         lInvokeId = 0;
   acu_sccp_addr_t   callingAddr;	
   acu_sccp_addr_t   calledAddr;	
   EnumTcapDlg			dlgType;
   UINT8					opCode;
   UINT32				lDlgTimeout;

   sprintf(lLogText,"MN: DlgId->TxnId Restoration Started. RestoreStatus:%d", gRestoreStatus);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP155, 1, lLogText);

   if(146 != mMsgType)
   {
      lDlgTimeout = mDlgTimeout;
   }
   else
   {
      lDlgTimeout = mCapDlgTimeout; 
   }

   for(unsigned int i = 1; i <= mMaxDlgSize; i++)
   {
      lInsertTime = 0;
      lCurrTime = 0;

      lDlgRcd.dlgId = i;
      mDlgManager.GetAllDlgInfo(lDlgRcd); 

      lCurrTime = time(NULL) - lDlgRcd.insertTime;

      if((lCurrTime  < lDlgTimeout) && (lDlgRcd.insertTime != 0))
      {

         sprintf(lLogText,"MN: DlgId:%d lTid:%08X rTid:%08X lTidLen:%d rTidLen:%d InsertTime:%ld TimeDiff:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Restore Details", lDlgRcd.dlgId, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lCurrTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP53, 1, lLogText);

         sprintf(lLogText,"MN: DlgId:%d Calling Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d",lDlgRcd.dlgId, lDlgRcd.callingAddr.sa_gt.sag_num, lDlgRcd.callingAddr.sa_tt, lDlgRcd.callingAddr.sa_np, lDlgRcd.callingAddr.sa_es, lDlgRcd.callingAddr.sa_nai);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP54, 1, lLogText);

         sprintf(lLogText,"MN: DlgId:%d Called Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d", lDlgRcd.dlgId, lDlgRcd.calledAddr.sa_gt.sag_num, lDlgRcd.calledAddr.sa_tt, lDlgRcd.calledAddr.sa_np, lDlgRcd.calledAddr.sa_es, lDlgRcd.calledAddr.sa_nai);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP55, 1, lLogText);

         if(true == mAculabApi.CreateAcuTcapOngoingTrans(&lTransPtr, lInstanceNo, lDlgRcd.origTransIdLen, lDlgRcd.origTransId, lDlgRcd.destTransIdLen, lDlgRcd.destTransId, lDlgRcd.ssn, lDlgRcd.opCode, lDlgRcd.invokeId, lCurrTime, lDlgRcd.ssapInstance))
         {
            *acu_tcap_trans_get_locaddr(lTransPtr) = lDlgRcd.callingAddr;
            *acu_tcap_trans_get_remaddr(lTransPtr) = lDlgRcd.calledAddr;

            lDlgRcd.trans = lTransPtr; 
            lDlgRcd.restarted	=	true;

            if(false == mTransDlgMap.MapDlgIdtoTransId(lDlgRcd))
            {
               sprintf(lLogText,"MN: DlgId:%d TransPtr:%08X Mapping dialog id to Transaction failed", i, lTransPtr);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP56, 1, lLogText);
            }
            sprintf(lLogText,"MN: DlgId:%d TransPtr:%08X Transaction Restore success", i, lTransPtr);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP57, 1, lLogText);

            if(lDlgRcd.dlgType == TCAP_BEGIN)
            {
               int lRetVal = acu_tcap_transaction_restore_app_ctx(lTransPtr,
                     lDlgRcd.applicationContext.array, 
                     lDlgRcd.applicationContext.numberOfBytes);

               //for (int i = 0; i < lDlgRcd.applicationContext.numberOfBytes;)  // TBR
               //{
               //printf("%02X ", lDlgRcd.applicationContext.array[i]);
               //i++;
               //}
               //printf("\n");

               if(0 != lRetVal)
               {
                  TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
                  mAcuTcapUtil.ReturnAculabErrStr(lRetVal,lErrText);
                  sprintf(lLogText,"%s",lErrText);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               }
               else
               {
                  sprintf(lLogText,"MN: NumberOfBytes:%d ApplicationContext:%s Setting Application context Success", 
                        lDlgRcd.applicationContext.numberOfBytes, lDlgRcd.applicationContext.array);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP58, 1, lLogText);
               }
            }
         }
         else
         {
            sprintf(lLogText,"MN: DlgId:%d Transaction Restore failed", i);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP59, 1, lLogText);
         }
      }

   }

   sleep(1);	
   gRestoreStatus = true;

   sprintf(lLogText,"MN: RestoreStatus:%d Transaction Restoration Completed.", gRestoreStatus);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP106, lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTotalInstanceNo 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : UINT16 
//-------------------------------------------------------------------------------
int TcapAculabHandler::GetTransValidationKey()
{
   struct timeval lTimeValue;
   gettimeofday(&lTimeValue, NULL);
   unsigned int lSeedVal = lTimeValue.tv_sec * 100000 + lTimeValue.tv_usec;
   return rand_r(&lSeedVal);
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateSsapStatus 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::UpdateSsapStatus(const int lInstanceNo)
{
   return mAculabApi.UpdateSsapStatus(lInstanceNo);
}

//-------------------------------------------------------------------------------
// METHOD      : GetSsapStatus 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : UINT16 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::GetSsapStatus(const int lInstanceNo)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";
   int lLoop = 0;

   if(false ==  mAculabApi.GetSsapStatus(lInstanceNo))
   {
      sprintf(lLogText,"MN: InstanceNo:%d RestoreStatus:%d GetSsapStatus Error Trying for 2 sec...", lInstanceNo, gRestoreStatus);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP96, lLogText);
      while(true)
      {
         if( 2 != lLoop )
         {
            lLoop++;
            sleep(1);

            mAculabApi.UpdateSsapStatus(lInstanceNo);

            if( false != mAculabApi.GetSsapStatus(lInstanceNo))
            {
               sprintf(lLogText,"MN: InstanceNo:%d RestoreStatus:%d GetSsapStatus Success After Wait", 
                     lInstanceNo, gRestoreStatus);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP97, lLogText);
               break;
            }
         }
         else
         {
            sprintf(lLogText,"MN: GetSsapStatus Error After 2 sec wait... lInstanceNo:%d RestoreStatus:%d", 
                  lInstanceNo, gRestoreStatus);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP98, lLogText);
            return false;
         }
      }
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTotalInstanceNo 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : UINT16 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ReconnectSsap(int &lInstanceNo)
{
   return mAculabApi.SsapReConnect(lInstanceNo, mMsgType, mDlgTimeout, mCapDlgTimeout, mMaxDlgSize);
}

//-------------------------------------------------------------------------------
// METHOD      : DecryptLicKey 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : UINT16 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::DecryptLicKey()
{
   int lTcapPassAscii = 0;
   TEXT lTcapPassStr[20] = "";
   strcpy(lTcapPassStr, ACU_TCAP_PASS);

   TEXT  lRandomNo1[4] = "";
   TEXT  lRandomNo2[2] = "";
   TEXT  lLicSerNumChanCnt[7] = "";
   TEXT  lLicChanCntC2[7] = "";
   UINT8 lLicKeyIndex = 0;
   long int lConst1 = 56025, lConst2 = 2587;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lRandomNo2[0] = mLicKey[lLicKeyIndex++];
   for(int lCount = 0; lLicKeyIndex < 7; lCount++)
   {
      lLicSerNumChanCnt[lCount] = mLicKey[lLicKeyIndex++];
   }

   for(int lCount = 0; lLicKeyIndex < 10; lCount++)
   {
      lRandomNo1[lCount] = mLicKey[lLicKeyIndex++];
   }

   for(int lCount = 0; lLicKeyIndex < 15; lCount++)
   {
      lLicChanCntC2[lCount] = mLicKey[lLicKeyIndex++];
   }


   for(UINT16 lCount = 0; lCount < strlen(lTcapPassStr); lCount++)
   {
      lTcapPassAscii = lTcapPassAscii + (toascii(lTcapPassStr[lCount]) * (lCount + 1));
   }


   mMsgLimitCount = atol(lLicChanCntC2) - atol(lRandomNo2) - lConst2;
   mLicPassAscii = atol(lLicSerNumChanCnt) - ( (lConst1 - mMsgLimitCount ) * atol(lRandomNo1) );


   if(lTcapPassAscii == mLicPassAscii )
   {
      printf("%s:%s %s %ld %s\n",gProcessName,BLUE,"Valid Licence key,RX Message handling licence count: ",
            mMsgLimitCount,UNDO);
      sprintf(lLogText, "MN: Valid Licence key,RX Message handling licence Count:%ld", mMsgLimitCount);
      gLog.GenerateLog(ACUTCAP107, lLogText);
   }
   else
   {
      printf("%s:%s %s %s\n",gProcessName,RED,"InValid Licence, please reconfigure licence key",UNDO);
      sprintf(lLogText, "MN: InValid Licence, please reconfigure licence key");
      gLog.GenerateLog(ACUTCAP108, lLogText);
      return false;
   }

   //Licence is checking for an average of 5 secs
   mMsgLimitCount = mMsgLimitCount * 5;

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTotalInstanceNo 
// DESCRIPTION : 
// PARAMETER   : none
// RETURN      : UINT16 
//-------------------------------------------------------------------------------
UINT16 TcapAculabHandler::GetTotalInstanceNo()
{
   return mNoOfInstance;
}
//-------------------------------------------------------------------------------
// METHOD      : ReadConfig 
// DESCRIPTION : Calls diffrent apis for getting configuration
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ReadConfig(int &lNoOfInstance)
{
   if(false == ReadIpcConfig())
   {
      return false;
   }
   if(false == ReadKernelConfig(lNoOfInstance))
   {
      return false;
   }
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReadIpcConfig 
// DESCRIPTION : Reads IPC configuration parameters  
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ReadIpcConfig()
{
   CfgRead        lCfgRead("ACULAB_TCAP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit(SS7_IPC_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed...\n",
            gProcessName,SS7_IPC_CFG);
      gLog.GenerateLog(GSYS09,1,"ss7.cfg CfgRead object initializaton failed");
      return false;
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_TCAP_HDLR_Q_RCV",mMsgQ.RdMsgQKey , 
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_TCAP_HDLR_Q_RCV in %s",
            SS7_IPC_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_TCAP_HDLR_Q_RCV = %d -> %x\n", gProcessName,
               mMsgQ.RdMsgQKey,mMsgQ.RdMsgQKey););
   }

   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_TCAP_DEC_Q_RCV",mMsgQ.WrMsgQKey,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_TCAP_DEC_Q_RCV in %s",
            SS7_IPC_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_TCAP_DEC_Q_RCV = %d -> %x\n",gProcessName,
               mMsgQ.WrMsgQKey,mMsgQ.WrMsgQKey););
   }

   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_TCAP_HEART_BEAT_Q_RCV",mMsgQ.RdHbQKey,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_TCAP_HEART_BEAT_Q_RCV in %s",
            SS7_IPC_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_TCAP_HEART_BEAT_Q_RCV = %d -> %x\n",gProcessName,
               mMsgQ.RdHbQKey,mMsgQ.RdHbQKey););
   }
   lCfgRead.CfgDeInit();
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReadKernelConfig 
// DESCRIPTION : Reads Kernel configuration parameters  
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ReadKernelConfig(int &lNoOfInstance)
{
   CfgRead        lCfgRead("ACULAB_TCAP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   UINT8          lNumofOPCs = 0;

   if (false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SS7_KER_CFG);
      gLog.GenerateLog(GSYS09, 
            "kerlnel.cfg CfgRead object initializaton failed");
      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("RESTORATION_REQUIRED",
            mRestorationFlag,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for RESTORATION_REQUIRED  in %s Setting Default Value 0",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      mRestorationFlag = 0;
   }
   else
   {
      T(gTrace,printf("%s: RESTORATION_REQUIRED = %d\n", gProcessName, mRestorationFlag););
   }

   if (CFG_OK != lCfgRead.GetConfigNum("MAX_ACU_TCAP_DLG_SIZE",
            mMaxDlgSize,
            1, 500000))
   {
      sprintf(lLogText, "Configuration Error for MAX_ACU_TCAP_DLG_SIZE in file kernel.cfg, exiting ...");
      T(gTrace, printf("%s\n", lLogText););
      //gLog.GenerateLog(DLG23, lLogText);
      lCfgRead.CfgDeInit();
   }
   else
   {
      T(gTrace, printf("%s: MAX_ACU_TCAP_DLG_SIZE = %d\n", gProcessName, mMaxDlgSize););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_TIMEOUT",
            mDlgTimeout, DLG_TIMEOUT_MIN, DLG_TIMEOUT_MAX ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_TIMEOUT Parameter not found in %s",
            SS7_KER_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      return false;
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_TIMEOUT = %d\n",
               gProcessName, mDlgTimeout););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_TIMEOUT_CAP",
            mCapDlgTimeout, DLG_TIMEOUT_MIN, 8000 ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_TIMEOUT_CAP Parameter not found in %s",
            SS7_KER_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      return false;
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_TIMEOUT_CAP = %d\n",
               gProcessName, mCapDlgTimeout););
   }

   if( CFG_OK !=  lCfgRead.GetConfigNum("TCAP_DISABLE_RECV_LOCAL_ADDRESS",
            mSetRcvLocalAddress ,
            0,
            1))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_DISABLE_RECV_LOCAL_ADDRESS in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      mSetRcvLocalAddress = 0;
   }
   else
   {
      T(gTrace,printf("%s: TCAP_DISABLE_RECV_LOCAL_ADDRESS = %d \n", gProcessName,
               mSetRcvLocalAddress););
   }

   if( CFG_OK !=  lCfgRead.GetConfigNum("TCAP_PEG_REQUIRED",
            mPegFlag ,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_PEG_REQUIRED in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: TCAP_PEG_REQUIRED = %d \n", gProcessName,
               mPegFlag););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("TCAP_MSG_DISPLAY_PARAM",
            mDisplayParam,
            MIN_DIS_PARAM_VAL,
            MAX_DIS_PARAM_VAL))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_MSG_DISPLAY_PARAM in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: TCAP_MSG_DISPLAY_PARAM = %d\n", gProcessName, mDisplayParam););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("SET_LOCAL_ACU_TCAP_ADDR_FLAG",
            mSetLocAddrFlag,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for SET_LOCAL_ACU_TCAP_ADDR_FLAG  in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      return false;

   }
   else
   {
      T(gTrace,printf("%s: SET_LOCAL_ACU_TCAP_ADDR_FLAG = %d\n", gProcessName, mSetLocAddrFlag););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("SET_APP_GT_RELAY_FLAG",
            mAppGtRelayFlag,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for SET_APP_GT_RELAY_FLAG  in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      mAppGtRelayFlag = false;

   }
   else
   {
      T(gTrace,printf("%s: SET_APP_GT_RELAY_FLAG = %d\n", gProcessName, mAppGtRelayFlag););
   }


   if(CFG_OK != lCfgRead.GetConfigStr("TCAP_MSG_LICENCE_KEY",
            mLicKey,
            1,TCAP_ACU_MAX_LIC_LEN))
   {
      sprintf(lLogText,
            "Config read Err for TCAP_MSG_LICENCE_KEY in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      return false;
   }
   else
   {
      T(gTrace,printf("%s: TCAP_MSG_LICENCE_KEY = %s\n", gProcessName,mLicKey););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("SEND_RSP_TIMEOUT_ON_PRE_ARR_END",
            mSndTimeOutOnPreEnd,
            SS7_MIN_BOOLEAN, 
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for SEND_RSP_TIMEOUT_ON_PRE_ARR_END in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SEND_RSP_TIMEOUT_ON_PRE_ARR_END = %d\n", gProcessName, mSndTimeOutOnPreEnd););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("NUMBER_OF_OPC",
            lNumofOPCs,
            SS7_MIN_NO_OF_PC, 
            SS7_MAX_NO_OF_PC))
   {
      sprintf(lLogText,
            "Config read Err for NUMBER_OF_OPC in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      //return false;
      //Handling Default Condition, not more than single PC
   }
   else
   {
      T(gTrace,printf("%s: NUMBER_OF_OPC = %d\n", gProcessName, lNumofOPCs););
   }

   if(0 == lNumofOPCs)
   {
      SsapDetails lSsapDetails;
      memset(&lSsapDetails, 0, sizeof(SsapDetails));
      lSsapDetails.numOfInstances = lNoOfInstance;
      for(UINT8 i = 0; i < lSsapDetails.numOfInstances; i++)
      {
         lSsapDetails.instance[i].status = ADDED_NEW;
      }
      sprintf(lSsapDetails.configFile, "Tcap_%d.cfg", mMsgType);
      lSsapDetails.transId = ReadTcapConfig(lSsapDetails.configFile);
      mTransRange = lSsapDetails.transId;
      //gSsapDetails.insert(std::pair<UINT32, SsapDetails>(1, lSsapDetails));
      mNumofOPCs = 1;

      ////////////////////////////////////TBT
      int lPc;
      if (false == lCfgRead.CfgInit(lSsapDetails.configFile))
      {
         printf("%s: CfgRead object(%s) intialization failed.\n",
               gProcessName,lSsapDetails.configFile);
         gLog.GenerateLog(GSYS09, "Tcap_<ssn>.cfg CfgRead object initializaton failed");
         return false;
      }

      if( CFG_OK !=  lCfgRead.GetConfigNum("LocalPC",
               lPc,
               1,
               9999))
      {
         sprintf(lLogText,
               "Config read Err for lPc in %s",
               lSsapDetails.configFile);
         printf("%s: %s\n", lLogText,gProcessName);
         lCfgRead.CfgDeInit();
         return false;
      }
      else
      {
         T(gTrace,printf("%s: lPc = %d \n", gProcessName, lPc););
         gSsapDetails.insert(std::pair<UINT32, SsapDetails>(lPc, lSsapDetails));
         mNoOfInstance += lSsapDetails.numOfInstances;
      }

      ////////////////////////////////////TBT
   }

   for(UINT8 lTemp = 0; lTemp < lNumofOPCs; lTemp++)
   {
      TEXT     lOpcString[20] = "";
      TEXT     lTempPC[100];
      TEXT*    lTempText;
      map<UINT32, SsapDetails>::iterator lmSsapDetails;
      SsapDetails lSsapDetails;
      UINT32   lPointCode = 0;

      mNumofOPCs = lNumofOPCs;

      sprintf(lOpcString, "OPC_%d", lTemp);
      if(CFG_OK != lCfgRead.GetConfigStr(lOpcString,
               lTempPC,
               SS7_MIN_POINTCODE,100))
      {
         sprintf(lLogText,
               "Config read Err for %s in %s", lOpcString, SS7_KER_CFG);
         printf("%s: %s\n", lLogText,gProcessName);
         return false;
      }
      else
      {
         lTempText = (char*)strtok(lTempPC, ":");
         if(NULL != lTempText)
         {
            lPointCode = atoi(lTempText);

            lmSsapDetails = gSsapDetails.find(lPointCode);
            if(lmSsapDetails != gSsapDetails.end())
            {
               sprintf(lLogText, "PointCode %d is configured more than once", lPointCode);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(GSYS09, lLogText);
               return false;
            }

            lTempText = (char*)strtok(NULL, " #//");
            if(NULL != lTempText)
            {
               lSsapDetails.numOfInstances = atoi(lTempText);
               if(MAX_INSTANCE_PER_PC < lSsapDetails.numOfInstances)
               {
                  sprintf(lLogText, "Number of Instances for PC:%d is Configured more than Maximum limit:%d",
                        lPointCode, MAX_INSTANCE_PER_PC);
                  TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
                     gLog.GenerateLog(GSYS09, lLogText);
                  return false;
               }
               else
               {
                  for(UINT8 i = 0; i < lSsapDetails.numOfInstances; i++)
                  {
                     lSsapDetails.instance[i].status = ADDED_NEW;
                  }
               }
            }
            else
            {
               sprintf(lLogText, "Number of Instances is not configured for PC %d", lPointCode);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(GSYS09, lLogText);
               return false;
            }
            sprintf(lSsapDetails.configFile, "Tcap_%d_%d.cfg", lPointCode, mMsgType);
         }
         else
         {
            sprintf(lLogText, "PC is not Configured Properly for %s", lTempPC);
            TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(GSYS09, lLogText);
            return false;
         }

         lSsapDetails.transId = ReadTcapConfig(lSsapDetails.configFile);
         mTransRange = lSsapDetails.transId;
         mNoOfInstance += lSsapDetails.numOfInstances;
         gSsapDetails.insert(std::pair<UINT32, SsapDetails>(lPointCode, lSsapDetails));

         T(gTrace,printf("%s: %s = %s\n", gProcessName, lOpcString, lTempPC););
      }
      lNoOfInstance = mNoOfInstance;
   }

   lCfgRead.CfgDeInit(); 
   sprintf(lLogText, "Config Read Success NoOfInstance:%d TransRange:%d", mNoOfInstance, mTransRange);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACUTCAP80, lLogText);
   return true;

}

//-------------------------------------------------------------------------------
// METHOD      : ReloadConfig 
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ReloadConfig(int &lNoOfInstance)
{
   CfgRead        lCfgRead("ACULAB_TCAP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   UINT8          lNumofOPCs = 0;

   if (false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SS7_KER_CFG);
      gLog.GenerateLog(GSYS09, 
            "kerlnel.cfg CfgRead object initializaton failed");
      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("NUMBER_OF_OPC",
            lNumofOPCs,
            SS7_MIN_NO_OF_PC, 
            SS7_MAX_NO_OF_PC))
   {
      sprintf(lLogText,
            "Config read Err for NUMBER_OF_OPC in %s",
            SS7_KER_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      //return false;
      //Handling Default Condition, not more than single PC
   }
   else
   {
      T(gTrace,printf("%s: NUMBER_OF_OPC = %d\n", gProcessName, lNumofOPCs););
   }

   for(UINT8 lTemp = 0; lTemp < lNumofOPCs; lTemp++)
   {
      TEXT     lOpcString[20] = "";
      TEXT     lTempPC[100];
      TEXT*    lTempText;
      UINT32   lPointCode = 0;

      map<UINT32, SsapDetails>::iterator lmSsapDetails;
      SsapDetails lSsapDetails;

      mNumofOPCs = lNumofOPCs;

      sprintf(lOpcString, "OPC_%d", lTemp);
      if(CFG_OK != lCfgRead.GetConfigStr(lOpcString,
               lTempPC,
               SS7_MIN_POINTCODE,100))
      {
         sprintf(lLogText,
               "Config read Err for %s in %s", lOpcString, SS7_KER_CFG);
         printf("%s: %s\n", lLogText,gProcessName);
         return false;
      }
      else
      {
         sprintf(lLogText,
               "%s = %s", lOpcString, lTempPC);
         printf("%s: %s\n", lLogText, gProcessName);
         lTempText = (char*)strtok(lTempPC, ":");
         if(NULL != lTempText)
         {
            lPointCode = atoi(lTempText);

            lmSsapDetails = gSsapDetails.find(lPointCode);
            if(lmSsapDetails != gSsapDetails.end())
            {
               sprintf(lLogText, "PointCode %d is configured more than once", lPointCode);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(GSYS09, lLogText);
               continue;
            }

            lTempText = (char*)strtok(NULL, " #//");
            if(NULL != lTempText)
            {
               lSsapDetails.numOfInstances = atoi(lTempText);
               if(MAX_INSTANCE_PER_PC < lSsapDetails.numOfInstances)
               {
                  sprintf(lLogText, "Number of Instances for PC:%d is Configured more than Maximum limit:%d",
                        lPointCode, MAX_INSTANCE_PER_PC);
                  TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
                     gLog.GenerateLog(GSYS09, lLogText);
                  return false;
               }
               else
               {
                  for(UINT8 i = 0; i < lSsapDetails.numOfInstances; i++)
                     lSsapDetails.instance[i].status = ADDED_NEW;
               }
            }
            else
            {
               sprintf(lLogText, "Number of Instances is not configured for PC %d", lPointCode);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(GSYS09, lLogText);
               return false;
            }
            sprintf(lLogText, "Number of Instances %d configured for PC %d", lSsapDetails.numOfInstances, lPointCode);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

            sprintf(lSsapDetails.configFile, "Tcap_%d_%d.cfg", lPointCode, mMsgType);
         }
         else
         {
            sprintf(lLogText, "PC is not Configured Properly for %s", lTempPC);
            TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(GSYS09, lLogText);
            return false;
         }

         lSsapDetails.transId = ReadTcapConfig(lSsapDetails.configFile);
         mTransRange = lSsapDetails.transId;
         mNoOfInstance += lSsapDetails.numOfInstances;
         gSsapDetails.insert(std::pair<UINT32, SsapDetails>(lPointCode, lSsapDetails));

         T(gTrace,printf("%s: %s = %s\n", gProcessName, lOpcString, lTempPC););
      }
   }
   lNoOfInstance = mNoOfInstance;

   mAculabApi.ReloadConfig();

   lCfgRead.CfgDeInit(); 
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReadTcapConfig 
// DESCRIPTION : Reads Tcap configuration parameters  
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
UINT16 TcapAculabHandler::ReadTcapConfig(TEXT *cfgFile)
{
   CfgRead        lCfgRead("ACULAB_TCAP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   UINT16        lTransId = 0;

   if (false == lCfgRead.CfgInit(cfgFile))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,cfgFile);
      gLog.GenerateLog(GSYS09, 
            "Tcap CfgRead object initializaton failed");
      return 0;
   }

   //While testing the maximum value the driver is accepting is 4094
   if( CFG_OK !=  lCfgRead.GetConfigNum("TRANID_RANGE",
            lTransId ,
            0,
            4094))
   {
      sprintf(lLogText,
            "Config read Err for TRANSID_RANGE in %s Range should be 0 - 4094",
            cfgFile);
      printf("%s: %s\n", lLogText,gProcessName);
      lTransId = 0;
   }
   else
   {
      T(gTrace,printf("%s: TRANSID_RANGE = %d \n", gProcessName,
               lTransId););
   }

   lCfgRead.CfgDeInit(); 
   return lTransId;

}

//-------------------------------------------------------------------------------
// METHOD      : CreateMsgQ 
// DESCRIPTION : Creates Message queues 
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::CreateMsgQ()
{
   msgQueueError lQueErr;

   lQueErr = mMsgQ.RdQ.Create (mMsgQ.RdMsgQKey, IPC_CREAT | SS7_IPC_PERM);

   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,mMsgQ.RdQ);
      printf("%s: Error creating MSG_TCAP_HANDLER_Q_RCV queue key:%d\n",
            gProcessName,mMsgQ.RdMsgQKey);
      return false;
   }

   lQueErr = mMsgQ.WrQ.Create (mMsgQ.WrMsgQKey, IPC_CREAT | SS7_IPC_PERM);

   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,  mMsgQ.WrQ);
      printf("%s: Error creating MSG_TCAP_DECODER_Q_RCV queue key:%d\n",
            gProcessName, mMsgQ.WrMsgQKey);
      return false;
   }

   lQueErr = mMsgQ.HbQ.Create (mMsgQ.RdHbQKey, IPC_CREAT | SS7_IPC_PERM);

   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,  mMsgQ.HbQ);
      printf("%s: Error creating MSG_TCAP_HEART_BEAT_Q_RCV queue key:%d\n",
            gProcessName, mMsgQ.RdHbQKey);
      return false;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : WriteBackToTcapHdlrQueue 
// DESCRIPTION : Writes Message back to request handler queue
//               for deferred processing 
// PARAMETER   : TcapMsg
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::WriteBackToTcapHdlrQueue(TcapMsg &lRxTcapMsg)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (TcapMsg);

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Msg:%s DlgId:%d MsgType:%d QueueKey:%d",
         AculabUtil::ConvertTcapDlgTypeToStr(lRxTcapMsg.tcapDlg),
         lRxTcapMsg.dialogueId,mMsgType,mMsgQ.RdMsgQKey);

   msgQueueError lQueErr =  mMsgQ.RdQ.WriteMsg((void *)&lRxTcapMsg, lMsgLen,
         mMsgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueErr)
   {
      if (false ==
            AculabUtil::HandleQueueWriteError (lQueErr, mMsgQ.RdQ,&lRxTcapMsg,
               lMsgLen,mMsgType,NON_BLOCKING))
      {
         TERR(gTrace, printf ("%s: %s Write to Queue Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   gLog.GenerateLog(ACUTCAP26,lLogText); 
   T(gTrace, printf ("%s: %s Message Wrote Back to TcapHdlr Rcv Queue\n",
            gProcessName, lLogText););
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : TxMsgToApplication 
// DESCRIPTION : Sends Message to application queue
// PARAMETER   : TcapMsg
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::TxMsgToApplication(TcapMsg &lTxTcapMsg)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   msgQueueError  lQueErr;
   INT16          lMsgLen = sizeof (TcapMsg);
   TEXT           lAddrText[100] = "";

   PrintApplTcapStruct(lTxTcapMsg,"RX");

   AculabUtil::GetDestAddress(lTxTcapMsg.destAddress, lAddrText);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RX: DlgId:%d Msg:%s %s MsgType:%d QueueKey:%d Message Wrote to Decoder Queue", 
         lTxTcapMsg.dialogueId, AculabUtil::ConvertTcapDlgTypeToStr(lTxTcapMsg.tcapDlg),lAddrText,
         mMsgType, mMsgQ.WrMsgQKey);


   lQueErr = mMsgQ.WrQ.WriteMsg ((void *) &lTxTcapMsg, lMsgLen,
         mMsgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueErr)
   {
      if (false ==
            AculabUtil::HandleQueueWriteError (lQueErr, mMsgQ.WrQ, &lTxTcapMsg,
               lMsgLen, mMsgType, NON_BLOCKING))
      {
         TERR(gTrace, printf ("%s: %s Write to Queue Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   if(mPegFlag)
      gPeg.PegEvent(PEG_SEND_TO_APPL);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

   gLog.GenerateLog(ACUTCAP28,1,lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : TxMsgToStack 
// DESCRIPTION : Transmitts message to stack
// PARAMETER   : acu_tcap_msg_t * (aculab message structure pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::TxMsgToStack(acu_tcap_msg_t *lAcuTcapMsgPtr)
{
   if (true == mAculabApi.SendAcuTcapMsg(lAcuTcapMsgPtr))
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_SEND_TO_NWK);
      return true;
   }
   else
      return false;
}

//-------------------------------------------------------------------------------
// METHOD      : ProcessTxMsgToStack 
// DESCRIPTION : Processes Appliaction Tcap Message before transmitting
//               to stack
// PARAMETER   : TcapMsg (Application tcap message),
//               acu_tcap_msg_t *(aculab message structure pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ProcessTxMsgToStack(TcapMsg &lTcapMsg,
      acu_tcap_msg_t *lMsgPtr)
{
   EnumDelFlag       lDelFlag = NO_DELETE_TRANS;
   DlgRecord         lDlgRcd;
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   acu_tcap_trans_t  *lTransPtr=NULL;
   lDlgRcd.dlgId     = lTcapMsg.dialogueId;
   acu_sccp_addr_t   *lCallingAddr=NULL;
   acu_sccp_addr_t   *lCalledAddr=NULL;

#ifdef SS7_TIMESTAMP
   clock_gettime (CLOCK_REALTIME, &lTcapMsg.timeStamp);
#endif

   if(true == mAculabApi.HandlerMultipleComponents(lTcapMsg))
   {
      sprintf(lLogText,"TX: DlgId:%d LastComp:%d TcUserId:%d Received Multiple Components, Not last Component", 
            lTcapMsg.dialogueId, lTcapMsg.tcapComponent.lastComponent, lTcapMsg.tcUserId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP43,lLogText);
      return true;
   }

   if(true == mTransDlgMap.GetTransInfo(lDlgRcd, &lTransPtr))
   {
      //This loop handles for response to stack initiated message
      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Retrived Tcap Transaction Info",
            lTcapMsg.dialogueId, lTransPtr);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP79, 3, lLogText);

      if(NULL == lTransPtr)
      {
         sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Transaction ptr NULL, so Dropping Message",
               lTcapMsg.dialogueId, lTransPtr);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP174, lLogText);
         return false;
      }

      if(TCAP_BEGIN == lTcapMsg.tcapDlg)
      {
         if(mPegFlag)
            gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

         sprintf(lLogText,"TX: DlgId:%d Rcv BEGIN duplicate !!!!!"
               ",sending ABORT msg to aapl",lTcapMsg.dialogueId);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP23,lLogText);

         //Sendiing ABORT meesge to Application as its duplicate dialogue ID - paresh
         lTcapMsg.tcapDlg = TCAP_ABORT;
         EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
         acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
         mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
         TCAPAddress lAddress = lTcapMsg.destAddress;
         lTcapMsg.destAddress = lTcapMsg.origAddress;
         lTcapMsg.origAddress = lAddress;

         TxMsgToApplication(lTcapMsg);

         return false;
      }
      else
      {
         //Added for dropping the response messages pending from application
         // if the process restartes as the transaction range and pointer 
         // aculab transaction becomes invalid after the process restartes

#if 0 
         // Below Commented to avoid trans validation for HA
         if(lDlgRcd.transValidationKey != mTransValidationKey)
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);
            sprintf(lLogText,"Rcv resp msg of old tcap handler dlgId:%d"
                  ",sending ABORT msg to aapl",lTcapMsg.dialogueId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            //gLog.GenerateLog(ACUTCAP22,lLogText);

            mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);
            lTcapMsg.tcapDlg = TCAP_ABORT;
            EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
            acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
            mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
            TCAPAddress lAddress = lTcapMsg.destAddress;
            lTcapMsg.destAddress = lTcapMsg.origAddress;
            lTcapMsg.origAddress = lAddress;
            TxMsgToApplication(lTcapMsg);
            gLog.GenerateLog(ACUTCAP23,lLogText);
            return false;

         }
#endif
      }

      //Handling of Pre arranged end
      if(lTcapMsg.tcapDlg == TCAP_PRE_ARRANGED_END)
      {
         unsigned int lHalfMaxDlgId = mTransDlgMap.GetHalfDlgSize();

         if(lTcapMsg.dialogueId > lHalfMaxDlgId)
         {  
            // No response from application
            //Delete transaction
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Rcv PRE_ARRANGED_END(No resp from appl)"
                  ",deleting transaction and dlg",lTcapMsg.dialogueId, lTransPtr);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP22,lLogText);

            if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
            {  
               if(0 == acu_tcap_trans_block(lTransPtr))
               {
                  mAculabApi.DeleteAcuTcapTrans(lTransPtr);
               }
               else
               {
                  sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Trans Already Blocked No resp from appl",
                        lTcapMsg.dialogueId, lTransPtr);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP176,lLogText);
               }
            }   

            //if(mPegFlag)
            //gPeg.PegEvent(PEG_TCAP_RSP_TIMEOUT_TX);
         }
         else
         {
            // No response from network
            //or the application wants to close the ongoing transaction
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Rcv PRE_ARRANGED_END(no resp from nw)"
                  ",deleting transaction and dlg",lTcapMsg.dialogueId, lTransPtr);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP22,lLogText);         

            if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
            {
               if(0 == acu_tcap_trans_block(lTransPtr))
               {
                  mAculabApi.DeleteAcuTcapTrans(lTransPtr);
               }
               else
               {
                  sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Trans Already Blocked No resp from nw",
                        lTcapMsg.dialogueId, lTransPtr);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP176,lLogText);
               }
            }

            if(mPegFlag)
               gPeg.PegEvent(PEG_TCAP_RSP_TIMEOUT_RX);
         }

         if(mSndTimeOutOnPreEnd == true)
         {
            lTcapMsg.ssn = mMsgType;
            lTcapMsg.tcapDlg = TCAP_RSP_TIMEOUT; 
            memset(&lTcapMsg.tcapComponent, 0, sizeof(lTcapMsg.tcapComponent));
            lTcapMsg.componentPresent = true;
            lTcapMsg.tcapComponent.invokeId = 1;
            lTcapMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
            lTcapMsg.tcapComponent.tcapComp = TCAP_RSP_TIMEOUT_COMP;

            TxMsgToApplication(lTcapMsg);
         }

#if 0
         lTcapMsg.tcapDlg = TCAP_ABORT;
         lTcapMsg.componentPresent = true;
         lTcapMsg.tcapComponent.invokeId = 0;

         lTcapMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
         lTcapMsg.tcapComponent.tcapComp = TCAP_ABORT_COMP;

         acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_RESOURCE_LIMITATION;
         memcpy(&lTcapMsg.tcapComponent.tcapAbortComp.abortReason.array, &lCause, 1);

         sprintf(lLogText,"Sending ABORT to Network DialogId:%d",
               lTcapMsg.dialogueId);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP78,lLogText);
         // From here program flow is usual after else condition.
#endif
         return false;
      }

   }
   else
   {
      //New transaction has to be made for application initiated request
      if(TCAP_BEGIN == lTcapMsg.tcapDlg)
      {  

         lDlgRcd.dlgId = lTcapMsg.dialogueId;
         SsapDetailsStructIterator lSsapDetailsIterator = gSsapDetails.begin();
         SsapDetails lSsapDetails;
         int lInstanceId = 0;
         int lDefaultInstance = (gSsapDetails.begin()->second).numOfInstances;
         lSsapDetails = gSsapDetails.begin()->second;

         //Finding the instance based on the Orig PC, if nothing is selected then default 0 index 
         //will be considered
         if(mNumofOPCs > 1)
         {
            if((lSsapDetailsIterator = gSsapDetails.find(lTcapMsg.origAddress.pointCode)) != gSsapDetails.end())
            {
               lSsapDetails = lSsapDetailsIterator->second;
            }
            else
            {
               lSsapDetailsIterator = gSsapDetails.begin();
               lSsapDetails = lSsapDetailsIterator->second;
            }
         }

         int lTempInstances = 0;

         do
         {
            if(lSsapDetails.lastUsedInstance == lSsapDetails.numOfInstances - 1)
            {
               lInstanceId = lSsapDetails.instance[lSsapDetails.lastUsedInstance].instanceId;
               lSsapDetails.lastUsedInstance = 0;
            }
            else
            {
               lInstanceId = lSsapDetails.instance[lSsapDetails.lastUsedInstance].instanceId;
               lSsapDetails.lastUsedInstance++;
            }

            lSsapDetailsIterator->second = lSsapDetails;
            lTempInstances++;

         }while((false == AculabUtil::mTransmitFlag[lInstanceId]) && (lTempInstances != lSsapDetails.numOfInstances));

         if(false == AculabUtil::mTransmitFlag[lInstanceId])
         {
            usleep(100);
            sprintf(lLogText,"TX: DlgId:%d Transaction creation failed, None of the instance is IN_SERVICE",
                  lTcapMsg.dialogueId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP157,lLogText);

            lTcapMsg.tcapDlg = TCAP_ABORT;
            EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
            acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;

            mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);

            TCAPAddress lAddress = lTcapMsg.destAddress;
            lTcapMsg.destAddress = lTcapMsg.origAddress;
            lTcapMsg.origAddress = lAddress;

            TxMsgToApplication(lTcapMsg);

            mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);
            return false;
         }

         if(true == mAculabApi.CreateAcuTcapTrans(&lTransPtr,lInstanceId))
         {
            lDlgRcd.trans = lTransPtr;
            lDlgRcd.ssapInstance = lInstanceId;

            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Transaction creation success",
                  lTcapMsg.dialogueId, lTransPtr);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP88, 2, lLogText);
         }
         else
         {
            if(mPegFlag)
               gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

            sprintf(lLogText,"TX: DlgId:%d Transaction creation failed",
                  lTcapMsg.dialogueId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP90, 2, lLogText);

            lTcapMsg.tcapDlg = TCAP_ABORT;
            EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
            acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
            mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
            TCAPAddress lAddress = lTcapMsg.destAddress;
            lTcapMsg.destAddress = lTcapMsg.origAddress;
            lTcapMsg.origAddress = lAddress;

            TxMsgToApplication(lTcapMsg);
            mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);

            return false;
         }
      }
      else if(TCAP_BEGIN != lTcapMsg.tcapDlg) //To be checked
      {
         if(mPegFlag)
            gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

         sprintf(lLogText,
               "TX: DlgId:%d Rcv msg:%s from Appl with No Transaction Info",
               lTcapMsg.dialogueId, AculabUtil::ConvertTcapDlgTypeToStr(lTcapMsg.tcapDlg));
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP158,lLogText);

         if(lTcapMsg.tcapDlg == TCAP_PRE_ARRANGED_END)
         {
            sprintf(lLogText,
                  "TX: DlgId:%d Rcv msg:%s TCAP_PRE_ARRANGED_END from Appl with no Transaction Info",
                  lTcapMsg.dialogueId, AculabUtil::ConvertTcapDlgTypeToStr(lTcapMsg.tcapDlg));
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP168,lLogText);

            mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);
            return false;
         }

         lTcapMsg.tcapDlg = TCAP_ABORT;
         EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
         acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
         mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
         TCAPAddress lAddress = lTcapMsg.destAddress;
         lTcapMsg.destAddress = lTcapMsg.origAddress;
         lTcapMsg.origAddress = lAddress;

         TxMsgToApplication(lTcapMsg);
         mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);

         return false;
      }
   }


   if (false == mAculabApi.AllocAcuTcapMsg(&lMsgPtr, lTransPtr))
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Message Allocation Failed !!!!!",
            lTcapMsg.dialogueId, lTransPtr);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP159,lLogText);

      lTcapMsg.tcapDlg = TCAP_ABORT;
      EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
      acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
      mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
      TCAPAddress lAddress = lTcapMsg.destAddress;
      lTcapMsg.destAddress = lTcapMsg.origAddress;
      lTcapMsg.origAddress = lAddress;

      TxMsgToApplication(lTcapMsg);

      if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
      {
         if(0 == acu_tcap_trans_block(lTransPtr))
         {
            mAculabApi.DeleteAcuTcapTrans(lTransPtr);
         }
         else
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Trans Already Blocked, Message Allocation Failed",
                  lTcapMsg.dialogueId, lTransPtr);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP176,lLogText);
         }
      }   

      return false;
   }

   unsigned int lLocId;
   unsigned int lNewLocId;
   unsigned int lRemId;
   unsigned int lRemIdLen;
   mAculabApi.GetTransIds(lMsgPtr, &lLocId, &lRemId, &lRemIdLen);


   if(lTcapMsg.tcapDlg == TCAP_BEGIN)
   { 
      if(mSetLocAddrFlag)
      {   
         mAculabApi.SetLocTcapAddress(lMsgPtr, lTcapMsg.origAddress);
      }   
      mAculabApi.SetRemTcapAddress(lMsgPtr,lTcapMsg.destAddress);
   }
   else //Added by Pushpa, for relaying the GT received from application in case of non BEGIN messages.
   {
      if(lLocId == 0 || lDlgRcd.origTransId != lLocId)
      {
         acu_tcap_msg_free(lMsgPtr);

         sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X oTId:%08X Dialog Completed Dropping Message, after AllocAcuTcapMsg",
               lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lDlgRcd.origTransId);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP109,lLogText);

         return false;
      }

      if(mAppGtRelayFlag)
      {   
         if(lTcapMsg.destAddress.numberOfDigits != 0)
            mAculabApi.SetRemTcapAddress(lMsgPtr,lTcapMsg.destAddress);

         if(lTcapMsg.origAddress.numberOfDigits != 0)
            mAculabApi.SetLocTcapAddress(lMsgPtr, lTcapMsg.origAddress);
      }
   }


   acu_tcap_msg_type_t lType =  mAculabApi.GetAcuTcapMsgType(lTcapMsg);

   if (false == mAculabApi.InitAcuTcapMsg(lMsgPtr, lType))
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Message Init Failed !!!!!",
            lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP160,lLogText);

      lTcapMsg.tcapDlg = TCAP_ABORT;
      EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
      acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;

      mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);

      TCAPAddress lAddress = lTcapMsg.destAddress;
      lTcapMsg.destAddress = lTcapMsg.origAddress;
      lTcapMsg.origAddress = lAddress;

      TxMsgToApplication(lTcapMsg);


      if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
      {   
         mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

         //if(lLocId == 0)
         if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message, after InitAcuTcapMsg Fail",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP109,lLogText);

            acu_tcap_msg_free(lMsgPtr);

            //return false;
         }
         else
         {
            acu_tcap_msg_free(lMsgPtr);

            if(0 == acu_tcap_trans_block(lTransPtr))
            {
               mAculabApi.DeleteAcuTcapTrans(lTransPtr);
            }
            else
            {
               sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X oTId:%08X Trans Already Blocked InitAcuTcapMsg Fail",
                     lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId, lDlgRcd.origTransId);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP176,lLogText);
            }
         }
      }
      else
      {
         acu_tcap_msg_free(lMsgPtr);
      }

      return false;
   }

   if(false == mAculabApi.AddAcuTcapDialogue(lMsgPtr, lTcapMsg))
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Adding Dialouge Data Failed !!!!!",
            lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP161,lLogText);

      lTcapMsg.tcapDlg = TCAP_ABORT;
      EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
      acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
      mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
      TCAPAddress lAddress = lTcapMsg.destAddress;
      lTcapMsg.destAddress = lTcapMsg.origAddress;
      lTcapMsg.origAddress = lAddress;

      TxMsgToApplication(lTcapMsg);


      if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
      {   
         mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

         //if(lLocId == 0)
         if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message, after AddAcuTcapDialogue Fail",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP109,lLogText);

            acu_tcap_msg_free(lMsgPtr);

            //return false;
         }
         else
         {
            acu_tcap_msg_free(lMsgPtr);
            if(0 == acu_tcap_trans_block(lTransPtr))
            {
               mAculabApi.DeleteAcuTcapTrans(lTransPtr);
            }
            else
            {
               sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Trans Already Blocked, AddAcuTcapDialogue Fail",
                     lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP176,lLogText);
            }

         }
      }   
      else
      {
         acu_tcap_msg_free(lMsgPtr);
      }
      return false;
   }

   //If components are to be included, pass them to the API using 
   //acu_tcap_msg_add_comp_invoke()/_result()/_error()/_reject() 
   //may be called repeatedly to add components to the message.
   if(true == lTcapMsg.componentPresent)
   {
      if(false == mAculabApi.AddAcuTcapComponet(lTcapMsg, lMsgPtr))
      {
         if(mPegFlag)
            gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

         sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Adding Component Data Failed",
               lTcapMsg.dialogueId, lTransPtr);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP162,lLogText);

         lTcapMsg.tcapDlg = TCAP_ABORT;
         EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
         acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
         mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
         TCAPAddress lAddress = lTcapMsg.destAddress;
         lTcapMsg.destAddress = lTcapMsg.origAddress;
         lTcapMsg.origAddress = lAddress;

         TxMsgToApplication(lTcapMsg);

         if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
         {   
            mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

            //if(lLocId == 0)
            if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
            {
               sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message, after AddAcuTcapComponet Fail",
                     lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP109,lLogText);

               acu_tcap_msg_free(lMsgPtr);

               //return false;
            }
            else
            {
               acu_tcap_msg_free(lMsgPtr);
               if(0 == acu_tcap_trans_block(lTransPtr))
               {
                  mAculabApi.DeleteAcuTcapTrans(lTransPtr);
               }
               else
               {
                  sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Trans Already Blocked, AddAcuTcapComponet Fail",
                        lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP176,lLogText);
               }

            }
         }   
         else
         {
            acu_tcap_msg_free(lMsgPtr);
         }
         return false;

      }
   }  

   // HA TX modifications ------------ProcessTxMsgToStack-----------
   // !=Begin case copy latest invoke id (dest trans id will be over written each time)
   lDlgRcd.invokeId        =  lTcapMsg.tcapComponent.invokeId;

   if(TCAP_BEGIN != lTcapMsg.tcapDlg)
   {
      lDlgRcd.ssn = mMsgType;
      lDlgRcd.origTransId     =  lLocId;
      lDlgRcd.origTransIdLen  =  4;
      lDlgRcd.destTransId     =  lRemId;
      lDlgRcd.destTransIdLen  =  lRemIdLen;
      lDlgRcd.dlgType         =  lTcapMsg.tcapDlg;
      lDlgRcd.insertTime      = time(NULL);

      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Continue/End Details", lDlgRcd.dlgId, lDlgRcd.trans, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);

      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP60, 2, lLogText);
   }

   // ---------------------------------------------------------------------------

   switch(lTcapMsg.tcapDlg)
   {
      case TCAP_BEGIN:
         {
            // HA TX modifications ------------ProcessTxMsgToStack-----------
            // In Begin copy only once below mentioned info

            lDlgRcd.ssn             = mMsgType;
            lDlgRcd.origTransId     =  lLocId;
            lDlgRcd.origTransIdLen  =  4;
            lDlgRcd.dlgType         =  TCAP_BEGIN;
            lDlgRcd.opCode          =  lTcapMsg.tcapComponent.tcapInvokeComp.operation.operationCode;

            mAculabApi.SetLocTcapAddress(lMsgPtr, &lCallingAddr);
            lDlgRcd.callingAddr = *lCallingAddr;

            mAculabApi.SetRemTcapAddress(lMsgPtr, &lCalledAddr);
            lDlgRcd.calledAddr = *lCalledAddr;

            memcpy(&lDlgRcd.applicationContext, &lTcapMsg.applicationContext, sizeof(lTcapMsg.applicationContext));
		#ifdef KAFKA_BRIDGE
	            memcpy(&lDlgRcd.routing, &lTcapMsg.routing, sizeof(TcapRoutingInfo));
		#endif
            // Remove after test
            //mDispApi.DisplayApplicationContext(&lDlgRcd.applicationContext);

            sprintf(lLogText,"TX: DlgId:%d sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d Msg Calling Address Details", lDlgRcd.dlgId, lDlgRcd.callingAddr.sa_gt.sag_num, lDlgRcd.callingAddr.sa_tt, lDlgRcd.callingAddr.sa_np, lDlgRcd.callingAddr.sa_es, lDlgRcd.callingAddr.sa_nai);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP61, 2, lLogText);

            sprintf(lLogText,"TX: DlgId:%d sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d Msg Called Address Details", lDlgRcd.dlgId, lDlgRcd.calledAddr.sa_gt.sag_num, lDlgRcd.calledAddr.sa_tt, lDlgRcd.calledAddr.sa_np, lDlgRcd.calledAddr.sa_es, lDlgRcd.calledAddr.sa_nai);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP62, 2, lLogText);

            // Update dlag record in dlg record pool
            // Store Dlgrecord pinter in aculab library memory
            lDlgRcd.transValidationKey =  mTransValidationKey;  
            lDlgRcd.insertTime = time(NULL);
            lDlgRcd.ssn = mMsgType;

            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d, SsapInstance:%d Begin Details", lDlgRcd.dlgId, lDlgRcd.trans, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);

            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP63, 2, lLogText);

            if(false == mTransDlgMap.MapDlgIdtoTransId(lDlgRcd))
            {
               if(mPegFlag)
                  gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);
               sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X BEGIN message MapDlgIdtoTransId failed",
                     lDlgRcd.dlgId, lTransPtr, lLocId);
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP163,lLogText);

               //Send back Abort
               lTcapMsg.tcapDlg = TCAP_ABORT;
               EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
               acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
               mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
               TCAPAddress lAddress = lTcapMsg.destAddress;
               lTcapMsg.destAddress = lTcapMsg.origAddress;
               lTcapMsg.origAddress = lAddress;

               TxMsgToApplication(lTcapMsg);

               mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

               //if(lLocId == 0)
               if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
               {
                  sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message, Begin Failed",
                        lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP109,lLogText);

                  acu_tcap_msg_free(lMsgPtr);

                  //return false;
               }
               else
               {
                  acu_tcap_msg_free(lMsgPtr);
                  if(0 == acu_tcap_trans_block(lTransPtr))
                  {
                     mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                  }
                  else
                  {
                     sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Trans Already Blocked, Begin Failed",
                           lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP176,lLogText);
                  }

               }

               return false;
            }
         }
         break;
      case TCAP_END:
      case TCAP_ABORT:
      case TCAP_RSP_TIMEOUT:
      case TCAP_END_WITH_ADDRESS:
         {   
            lDelFlag = DELTE_TRANS;
         }
         break;
      default:
         {
            // Update dlag record in dlg record pool
            // Store Dlgrecord pinter in aculab library memory
            lDlgRcd.restarted = false;
            lDlgRcd.transValidationKey =  mTransValidationKey;
            lDlgRcd.insertTime = time(NULL);
            lDlgRcd.ssn = mMsgType;

            if(false == mTransDlgMap.MapDlgIdtoTransId(lDlgRcd))
            {
               sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Mapping dialog id to Transaction failed (default case)", 
                     lDlgRcd.dlgId, lTransPtr);
               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP91, lLogText);
            }

            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X Mapping dialog id to Transaction success (default case)", 
                  lDlgRcd.dlgId, lTransPtr);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP100, lLogText);
         }
         break;
   }

   if(DELTE_TRANS == lDelFlag) // lock
   {
      //timeval lTimeValue;
      //double lStartTime = 0;

      //gettimeofday (&lTimeValue, NULL);

      //lStartTime =
      //  lTimeValue.tv_sec + (lTimeValue.tv_usec / 1000000.00);

      if(1 == acu_tcap_trans_block(lTransPtr))
      {
         usleep(500);

         if(1 == acu_tcap_trans_block(lTransPtr))
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Trans Still Blocked, Dropping Msg",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP175,lLogText);
            return false;
         }
         else
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Got Trans Lock",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP176,lLogText);

            DlgRecord    lDlgRecord;
            lDlgRecord.dlgId = lTcapMsg.dialogueId;
            acu_tcap_trans_t  *lTransPtrLock=NULL;
            if(true == mTransDlgMap.GetTransInfo(lDlgRecord, &lTransPtrLock))
               //if(true == mTransDlgMap.GetDialogueInfo(lTransPtr, lDlgRecord))
            {
               if(lDlgRecord.origTransId == 0)
               {
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X oTId:%08X Dialog Completed Dropping Message on Lock",
                        lDlgRecord.dlgId, lTransPtrLock, lLocId, lRemId, lDlgRecord.origTransId);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP109,lLogText);
                  return false;
               }
            }

         }
      }
      else
      {
         sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Got Trans Lock at First Attempt",
               lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP177,lLogText);
      }

      //gettimeofday (&lTimeValue, NULL);
      //    sprintf(lLogText," Lock Processing Time:%f\n", 
      //  (lTimeValue.tv_sec +
      //    (lTimeValue.tv_usec / 1000000.00)) - lStartTime);
      //TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      //gLog.GenerateLog(ACUTCAP178,lLogText);
   }

   if(false ==  TxMsgToStack(lMsgPtr))
   {
      sprintf(lLogText, "TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg %s transmit to stack failed",
            lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, AculabUtil::ConvertTcapDlgTypeToStr(lTcapMsg.tcapDlg));
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP12,2,lLogText);

      if(mPegFlag)
         gPeg.PegEvent(PEG_DROP_SEND_TO_NWK);

      lTcapMsg.tcapDlg = TCAP_ABORT;
      EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
      acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE;
      mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);
      TCAPAddress lAddress = lTcapMsg.destAddress;
      lTcapMsg.destAddress = lTcapMsg.origAddress;
      lTcapMsg.origAddress = lAddress;

      TxMsgToApplication(lTcapMsg);

      mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

      //if(lLocId == 0)
      if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
      {
         sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X oTId:%08X Dialog Completed Dropping Message, TxMsgToStack Fail",
               lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId, lDlgRcd.origTransId);
         TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP109,lLogText);

         acu_tcap_msg_free(lMsgPtr);

         //return false;
      }
      else
      {
         acu_tcap_msg_free(lMsgPtr);
         if(0 == acu_tcap_trans_block(lTransPtr))
         {
            mAculabApi.DeleteAcuTcapTrans(lTransPtr);
         }
         else
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Trans Already Blocked, TxMsgToStack Fail",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP176,lLogText);
         }

      }

      return false;
   }

   sprintf(lLogText, "TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg %s Transmit to stack success",
         lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, AculabUtil::ConvertTcapDlgTypeToStr(lTcapMsg.tcapDlg));
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP169,1,lLogText);

   if(DELTE_TRANS == lDelFlag)
   {
      sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg %s Deleting Transaction ptr",
            lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, AculabUtil::ConvertTcapDlgTypeToStr(lTcapMsg.tcapDlg));
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP69, 2, lLogText);


      if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
      {   
         mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

         //if(lLocId == 0)
         if(lNewLocId == 0 || lDlgRcd.origTransId != lNewLocId)
         {
            sprintf(lLogText,"TX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X oTId:%08X Dialog Completed Dropping Message, in DELTE_TRANS",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lNewLocId, lDlgRcd.origTransId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP109,lLogText);

            acu_tcap_msg_free(lMsgPtr);

            //return false;
         }
         else
         {
            acu_tcap_msg_free(lMsgPtr);
            mAculabApi.DeleteAcuTcapTrans(lTransPtr);
         }
      }   
      else
      {
         acu_tcap_msg_free(lMsgPtr);
      }
   }
   else
   {
      acu_tcap_msg_free(lMsgPtr);
   }

   pthread_mutex_lock(&gPthreadMutexLock);
   //Checking Message handling limit as per configuration
   if(gMsgCounter <=  mMsgLimitCount)
   {
      gMsgCounter++;
      sprintf(lLogText,"TX: Message Counter is less than or equal to limit:%d",gMsgCounter);
      T(gTrace,printf("%s: %s",gProcessName,lLogText);); 
      gLog.GenerateLog(ACUTCAP136, 4, lLogText);
   }
   else
   {
      if(gMsgCounter > mMsgLimitCount)
      { 
         gettimeofday(&mStartTime, NULL);
   
         if(mEndTime.tv_sec >= mStartTime.tv_sec)
         {
            struct timeval tv;

            //checking mStartTime and mEtartTime for time delay 
            //reset end time
            
            INT32 diff = difftime(mStartTime.tv_sec, mEndTime.tv_sec);

            if(diff > 5)
            {
                  sprintf(lLogText,"Difference between end time and start time %d ", diff);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP25,lLogText);

                  mEndTime.tv_sec = mStartTime.tv_sec;
                  mEndTime.tv_usec = mStartTime.tv_usec;
            }

            tv.tv_sec = mEndTime.tv_sec - mStartTime.tv_sec;
            if(mEndTime.tv_usec >= mStartTime.tv_usec)
            {
               tv.tv_usec = (mEndTime.tv_usec - mStartTime.tv_usec) % 1000000;
            }
            else
            {
               if(tv.tv_sec > 0)
               {
                  tv.tv_sec -= 1;
                  tv.tv_usec = ((1000000 - mStartTime.tv_usec) + mEndTime.tv_usec) % 1000000;
               }
               else
               {
                  tv.tv_sec = 0;
                  tv.tv_usec = 0;
               }
            }
            sprintf(lLogText,"TX: License Exceeded, No of Msgs %d, waiting %d sec and %ld microsecs",
                  gMsgCounter, tv.tv_sec, tv.tv_usec);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP25,lLogText);

            select(1, NULL, NULL, NULL, &tv);
         }
         gettimeofday(&mStartTime, NULL);
         mEndTime.tv_sec = mStartTime.tv_sec + 5;
         mEndTime.tv_usec = mStartTime.tv_usec;
         gMsgCounter = 1;

         sprintf(lLogText,"TX: Message Counter Reset:%d", gMsgCounter);
         T(gTrace,printf("%s: %s",gProcessName,lLogText);); 
         gLog.GenerateLog(ACUTCAP137, 4, lLogText);
       }
   }
   pthread_mutex_unlock(&gPthreadMutexLock);
   sprintf(lLogText,"TX: Message Counter Success :%d", gMsgCounter);
   T(gTrace,printf("%s: %s",gProcessName,lLogText);); 
   gLog.GenerateLog(ACUTCAP138, 4, lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : RxMsgFromStack 
// DESCRIPTION : Recieves aculab message struture from stack 
// PARAMETER   : acu_tcap_msg_t **(aculab message structure double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::RxMsgFromStack(acu_tcap_msg_t **lMsgPtr,int &lInstanceId)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lHeartBeatText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof(lHeartBeatText);
   msgQueueError  lQueErr;
   acu_tcap_trans_t *lTransPtr = NULL; 

   // Read Messages from Heart-Beat Queue
   lQueErr = mMsgQ.HbQ.ReadMsg((void *)&lHeartBeatText, lMsgLen, mMsgType, NON_BLOCKING);

   if(false ==  mAculabApi.GetAcuTcapEvent(lMsgPtr,lInstanceId))
   {
      return false;
   }
   else
   {
      acu_tcap_msg_type_t lMsgType; 
      lMsgType = mAculabApi.HandleEventAcuTcapMsg(*lMsgPtr);
      lTransPtr = (*lMsgPtr)->tm_trans;

      switch(lMsgType)
      {
         case ACU_TCAP_MSG_DATA:
            {

               if(mPegFlag)   
                  gPeg.PegEvent(PEG_RCVD_FROM_NWK);  
               return true;
            } 
         case ACU_TCAP_MSG_TIMEOUT:
            {
               if(mMsgType != 146)
               {
                  unsigned int lLocId;
                  unsigned int lRemId;
                  unsigned int lRemIdLen;

                  if (true == mAculabApi.GetTransIds(*lMsgPtr, &lLocId, &lRemId, &lRemIdLen))
                  {
                     TcapMsg lTcapMsg;
                     memset(&lTcapMsg,0,sizeof(TcapMsg));
#ifdef SS7_TIMESTAMP
                     clock_gettime (CLOCK_REALTIME, &lTcapMsg.timeStamp);
#endif
                     lTcapMsg.origTransId = (UINT32)lLocId;
                     lTcapMsg.destTransId = (UINT32)lRemId;
                     lTcapMsg.destTransIdLen = (UINT8)lRemIdLen;
                     lTcapMsg.origTransIdLen = 4;
                     lTcapMsg.ssn = mMsgType;

                     DlgRecord lDlgRcd;

                     if(true == mTransDlgMap.GetDialogueInfo(lTransPtr, lDlgRcd))
                     {
                        lTcapMsg.dialogueId = lDlgRcd.dlgId;
                        sprintf(lLogText,
                              "RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg:%s Get Dialogue Info",
                              lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, 
                              AculabUtil::GetAcuTcapMsgTypeStr((*lMsgPtr)->tm_msg_type));
                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP16, 2, lLogText);

                        lTcapMsg.tcapDlg = TCAP_RSP_TIMEOUT; 

                        mAculabApi.GetTcapTimeOutComponet(*lMsgPtr,lTcapMsg);

                        TxMsgToApplication(lTcapMsg);

                        if(mPegFlag)
                           gPeg.PegEvent(PEG_TCAP_RSP_TIMEOUT_RX);

                     }
                     else
                     {
                        sprintf(lLogText,
                              "RX: DlgId:%d Msg:%s Rcv Get Dialog Info failed", 
                              lTcapMsg.dialogueId, AculabUtil::GetAcuTcapMsgTypeStr((*lMsgPtr)->tm_msg_type));

                        T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP11,lLogText);
                     }

                     acu_tcap_trans_unblock(lTransPtr);
                     acu_tcap_msg_free(*lMsgPtr);

                     if(mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
                     {   
                        mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                     }
                  }
                  else
                  {
                     TcapMsg lTcapMsg;
                     memset(&lTcapMsg,0,sizeof(TcapMsg));

#ifdef SS7_TIMESTAMP
                     clock_gettime (CLOCK_REALTIME, &lTcapMsg.timeStamp);
#endif
                     DlgRecord lDlgRcd;
                     if(true == mTransDlgMap.GetDialogueInfo(lTransPtr, lDlgRcd))
                     {
                        lTcapMsg.dialogueId = lDlgRcd.dlgId;

                        sprintf(lLogText,
                              "RX: DlgId:%d TransPtr:%08X Msg:%s Rcv (get transId failed) Get Dialog Success",
                              lTcapMsg.dialogueId, lTransPtr, 
                              AculabUtil::GetAcuTcapMsgTypeStr((*lMsgPtr)->tm_msg_type));              
                        T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP101, 2, lLogText);
                     }
                     else
                     {
                        sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X Msg:%s Rcv (get transId failed) Get Dialog Failed",
                              lTcapMsg.dialogueId, lTransPtr, AculabUtil::GetAcuTcapMsgTypeStr((*lMsgPtr)->tm_msg_type));
                        T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP102,lLogText);

                     }


                     acu_tcap_trans_unblock(lTransPtr);
                     acu_tcap_msg_free(*lMsgPtr);
                     mAculabApi.DeleteAcuTcapTrans(lTransPtr);

                     sprintf(lLogText,"RX: DlgId:%d Acu TCAP Transaction Deleted",lTcapMsg.dialogueId);
                     T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP06, 3, lLogText);

                     if(mPegFlag)
                        gPeg.PegEvent(PEG_TCAP_RSP_TIMEOUT_RX);


                     lTcapMsg.ssn = mMsgType;
                     lTcapMsg.tcapDlg = TCAP_RSP_TIMEOUT; 
                     lTcapMsg.componentPresent = true;
                     lTcapMsg.tcapComponent.invokeId = 1;
                     lTcapMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
                     lTcapMsg.tcapComponent.tcapComp = TCAP_RSP_TIMEOUT_COMP;

                     TxMsgToApplication(lTcapMsg);
                  }
               }
               else
               {
                  DlgRecord lDlgRcd;

                  mTransDlgMap.GetDialogueInfo(lTransPtr, lDlgRcd);

                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X ACU_TCAP_MSG_TIMEOUT Ignoring for SSN 146", 
                        lDlgRcd.dlgId, lTransPtr);
                  T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP77,2, lLogText);

                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(*lMsgPtr);
               }
               return false;
            }     

         case ACU_TCAP_MSG_CON_STATE:
            {
               mAculabApi.GetAcuTcapConState(*lMsgPtr, lInstanceId);
               acu_tcap_msg_free(*lMsgPtr);

               return false;
            }
         case ACU_TCAP_MSG_SP_STATUS:
         case ACU_TCAP_MSG_USER_STATUS:
            {
               mAculabApi.GetAcuSccpConState(*lMsgPtr);
               acu_tcap_msg_free(*lMsgPtr);
               return false;
            }
         case ACU_TCAP_MSG_NOTICE:
            {
               mAculabApi.GetErrReportSccp(*lMsgPtr);

               unsigned int lLocId;
               unsigned int lRemId;
               unsigned int lRemIdLen;
               DlgRecord    lDlgRcd;
               TcapMsg lTcapMsg;
               memset(&lTcapMsg,0,sizeof(TcapMsg));

               if(lTransPtr != NULL)
               {
                  if(true == mTransDlgMap.GetDialogueInfo(lTransPtr, lDlgRcd))
                  {
                     lTcapMsg.dialogueId = lDlgRcd.dlgId;
                     mAculabApi.GetTransIds(*lMsgPtr, &lLocId, &lRemId, &lRemIdLen);
                     sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X oTId:%08X Rcv:%s ACU_TCAP_MSG_NOTICE Get Dialogue Info  Resp Msg", lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lDlgRcd.origTransId,  AculabUtil::GetAcuTcapMsgTypeStr((*lMsgPtr)->tm_msg_type));
                     T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP42, 2, lLogText);
                  }
               }

               if(mPegFlag)
                  gPeg.PegEvent(PEG_RCVD_FROM_NWK);

               acu_tcap_msg_free(*lMsgPtr);
               if(mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
               {   
                  if(lLocId == 0 || lDlgRcd.origTransId == 0)
                  {
                     sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X oTId:%08X Dialog Completed Dropping Message",
                           lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lDlgRcd.origTransId);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP109,lLogText);
                     return false;
                  }
                  mAculabApi.DeleteAcuTcapTrans(lTransPtr);
               }
               return false;
            }
         default:
            acu_tcap_msg_free(*lMsgPtr);
            return false;
      }

   }
}
//-------------------------------------------------------------------------------
// METHOD      : ProcessRxMsgFromStack 
// DESCRIPTION : Processes aculab tcap meessage after recieving from stack 
// PARAMETER   : acu_tcap_msg_t *(aculab message structure pointer),
//               TcapMsg &(application Tcap Message refrence)    
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::ProcessRxMsgFromStack(acu_tcap_msg_t *lMsgPtr, 
      TcapMsg &lTcapMsg, int &lInstanceId)
{
   unsigned int lLocId;
   unsigned int lNewLocId;
   unsigned int lRemId;
   unsigned int lTempLocId;
   unsigned int lRemIdLen;
   DlgRecord    lDlgRcd;
   DlgRecord    lTempDlgRcd;
   TEXT         lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   acu_sccp_addr_t   *lCallingAddr=NULL;
   acu_sccp_addr_t   *lCalledAddr=NULL; 
   int lSsapInstance = lInstanceId;


#ifdef SS7_TIMESTAMP
   clock_gettime (CLOCK_REALTIME, &lTcapMsg.timeStamp);
#endif

   TcapComponent lTcapComponent[ACU_TCAP_MAX_COMPONENT];
   memset(&lTcapComponent[0], 0, sizeof(lTcapComponent));

   //Taking address of transaction pointer to a local pointer
   acu_tcap_trans_t *lTransPtr = NULL; 

   int   lComponentCount = 0;
   const acu_tcap_dialogue_t *lDlg;

   int lRetVal = acu_tcap_msg_decode(lMsgPtr, &lDlg);
   if(0 != lRetVal)
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_DROP_RCVD_FROM_NWK);

      TEXT lErrText[ACU_TCAP_ERR_TEXT_LEN + 1] = "";
      AculabUtil::ReturnAculabErrStr(lRetVal , lErrText);

      sprintf(lLogText,"RX: Decoding of recv message %s failed(%d):%s",
            AculabUtil::GetAcuTcapMsgTypeStr(lMsgPtr->tm_msg_type),lRetVal,lErrText);
      TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP10, lLogText);

      lTransPtr = lMsgPtr->tm_trans;
      if(lTransPtr != NULL)
      {
         if(true == mTransDlgMap.GetDialogueInfo(lMsgPtr->tm_trans, lDlgRcd))
         {
            mAculabApi.GetTransIds(lMsgPtr, &lLocId, &lRemId, &lRemIdLen);

            lTcapMsg.dialogueId = lDlgRcd.dlgId;

            sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X oTId:%08X Rcv:%s Decode failed Get Dialogue Info",
                  lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId, lDlgRcd.origTransId, AculabUtil::GetAcuTcapMsgTypeStr(lMsgPtr->tm_msg_type));
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP164, 2, lLogText);

            if(0 != lTcapMsg.dialogueId)
            {
               lTcapMsg.tcapDlg = TCAP_ABORT;
               EnumAbortType lAbortType = TCAP_PROTOCOL_ABORT;
               acu_tcap_p_abort_cause_t lCause = ACU_TCAP_P_ABORT_ITU_RESOURCE_LIMITATION; 

               mAculabApi.FormTcapAbortMsg(lTcapMsg,lAbortType, lCause);

               TCAPAddress lAddress = lTcapMsg.destAddress;
               lTcapMsg.destAddress = lTcapMsg.origAddress;
               lTcapMsg.origAddress = lAddress;
               lTcapMsg.ssn = mMsgType;

               mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId);  

               TxMsgToApplication(lTcapMsg);
            }
         }

         mAculabApi.GetTransIds(lMsgPtr, &lLocId, &lRemId, &lRemIdLen);
         acu_tcap_trans_unblock(lTransPtr);
         acu_tcap_msg_free(lMsgPtr);

         if(lLocId == 0 || lDlgRcd.origTransId == 0)
         {
            sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Dialog Completed Dropping Message",
                  lDlgRcd.dlgId, lTransPtr, lLocId, lRemId);
            TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUTCAP109,lLogText);
            return false;
         }

         mAculabApi.DeleteAcuTcapTrans(lTransPtr);
         return false;
      }

      acu_tcap_msg_free(lMsgPtr);
      return false;
   }
   else
   {
      lTransPtr = lMsgPtr->tm_trans;
      //in order to mecorde any further messages for that transaction available from the ssap
      //queue. The block is applied in order to stop an application having more than one thread
      //processing messages for a single transaction.


      //Getting transaction Ids for incoming message
      //If Fails delete transaction and free message 
      if (true ==  mAculabApi.GetTransIds(lMsgPtr, &lLocId, &lRemId, &lRemIdLen))
      {
         // Find Instance ID from transaction ID
         //lTempLocId = lLocId >> 20;  
         //lTempDlgRcd.ssapInstance = lTempLocId - mTransRange;

         //Fiiling up transaction ids
         lTcapMsg.origTransId = (UINT32)lLocId;
         lTcapMsg.destTransId = (UINT32)lRemId;
         lTcapMsg.destTransIdLen = (UINT8)lRemIdLen;
         lTcapMsg.origTransIdLen = 4;

         // HA RX modifications ------------ProcessRxMsgFromStack-----------
         lTempDlgRcd.invokeId        =  lTcapMsg.tcapComponent.invokeId;
         lTempDlgRcd.origTransId		=	lTcapMsg.origTransId;
         lTempDlgRcd.destTransId		=	lTcapMsg.destTransId;
         lTempDlgRcd.destTransIdLen	=	lTcapMsg.destTransIdLen;	
         lTempDlgRcd.origTransIdLen	=	lTcapMsg.origTransIdLen;

         //Getting dialogue info for incoming response if it fails
         //then its an incoming request
         if((ACU_TCAP_MSG_ITU_BEGIN != lMsgPtr->tm_msg_type) &&
               ( true == mTransDlgMap.GetDialogueInfo(lMsgPtr->tm_trans, lDlgRcd)))
         {
            //Incoming Request
            if(ACU_TCAP_MSG_ITU_CONTINUE == lMsgPtr->tm_msg_type)
            {
               // HA RX modifications ------------ProcessRxMsgFromStack-----------
               // !=Begin case copy latest invoke id (dest trans id will be over written each time)
               lDlgRcd.trans = lMsgPtr->tm_trans; 
               lDlgRcd.invokeId        =  lTempDlgRcd.invokeId;
               lDlgRcd.origTransId	=	lTempDlgRcd.origTransId;
               lDlgRcd.destTransId	=	lTempDlgRcd.destTransId;
               lDlgRcd.destTransIdLen	=	lTempDlgRcd.destTransIdLen;
               lDlgRcd.origTransIdLen	=	lTempDlgRcd.origTransIdLen;
               lDlgRcd.ssapInstance   =  lSsapInstance;

               // Update dlag record in dlg record pool
               // Store Dlgrecord pinter in aculab library memory
               lDlgRcd.transValidationKey =  mTransValidationKey;  
               lDlgRcd.insertTime = time(NULL);
               lDlgRcd.ssn = mMsgType;
               lDlgRcd.dlgType  =  TCAP_CONTINUE;

               sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Continue Details", lDlgRcd.dlgId, lDlgRcd.trans, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);

               T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
               gLog.GenerateLog(ACUTCAP64, 2, lLogText);

               mTransDlgMap.MapDlgIdtoTransId(lDlgRcd);
            }

            //Incoming response
            lTcapMsg.dialogueId = lDlgRcd.dlgId;
            sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Resp Transaction Rcv Msg:%s",
                  lTcapMsg.dialogueId, lMsgPtr->tm_trans, lLocId, lRemId, AculabUtil::GetAcuTcapMsgTypeStr(lMsgPtr->tm_msg_type));

            T(gTrace,printf("%s: %s\n",gProcessName, lLogText););
            gLog.GenerateLog(ACUTCAP29, 2, lLogText);
            //Fill Message type (Handler SSN)
            lTcapMsg.ssn = mMsgType;
            lRetVal = mAculabApi.FillTcapStruct(lMsgPtr, lTcapMsg, lDlg,
                  &lTcapComponent[0],lComponentCount);
	    #ifdef KAFKA_BRIDGE
	    memcpy(&lTcapMsg.routing, &lDlgRcd.routing, sizeof(TcapRoutingInfo));
	    #endif
         }
         else
         {
            //Incoming Request
            if(ACU_TCAP_MSG_ITU_BEGIN == lMsgPtr->tm_msg_type)
            {
               //If incoming begin then allocate transaction
               if(true == mTransDlgMap.MapTransIdToDlgId(lMsgPtr->tm_trans, lDlgRcd))
               {
                  sprintf(lLogText,
                        "RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg:%s New Transaction Rcv MapTransIdToDlgId Success",
                        lDlgRcd.dlgId, lMsgPtr->tm_trans, lLocId, lRemId, AculabUtil::GetAcuTcapMsgTypeStr(lMsgPtr->tm_msg_type));
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););        
                  gLog.GenerateLog(ACUTCAP13, 2, lLogText);

                  lTcapMsg.dialogueId = lDlgRcd.dlgId;

                  //if(0 == mSetRcvLocalAddress)  // Not Required as set below.....
                  //{
                  //Sets the received Local Address as Destination Address for incoming BEGIN Msg
                  //    mAculabApi.SetRcvLocalAddress(lMsgPtr);
                  // }

                  //Fill Message type (Handler SSN)
                  lTcapMsg.ssn = mMsgType;
                  lRetVal = mAculabApi.FillTcapStruct(lMsgPtr, lTcapMsg, lDlg, 
                        &lTcapComponent[0], lComponentCount);

                  // HA RX modifications ------------ProcessRxMsgFromStack-----------
                  // In Begin copy only once below mentioned info
                  lDlgRcd.origTransId     =  lLocId;
                  lDlgRcd.origTransIdLen  =  lTcapMsg.origTransIdLen;
                  lDlgRcd.destTransId     =  lRemId;
                  lDlgRcd.destTransIdLen  =  lRemIdLen;
                  lDlgRcd.invokeId        =  lTcapMsg.tcapComponent.invokeId;
                  lDlgRcd.ssn = mMsgType;
                  lDlgRcd.dlgType  =  TCAP_BEGIN;
                  lDlgRcd.insertTime = time(NULL);
                  lDlgRcd.ssapInstance   =  lSsapInstance;

                  // Local Address setting	 
                  if(0 == mSetRcvLocalAddress)  // Not Required as set below.....
                  {
                     //Sets the received Local Address as Destination Address for incoming BEGIN Msg
                     mAculabApi.SetRcvLocalAddress(lMsgPtr);
                  }
                  mAculabApi.SetLocTcapAddress(lMsgPtr, &lCallingAddr);
                  lDlgRcd.callingAddr = *lCallingAddr;

                  mAculabApi.SetRemTcapAddress(lMsgPtr, &lCalledAddr);
                  lDlgRcd.calledAddr = *lCalledAddr;
                  // ---------------------------------------------------------------------------
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Begin Details", lDlgRcd.dlgId, lDlgRcd.trans, lDlgRcd.origTransId, lDlgRcd.destTransId, lDlgRcd.origTransIdLen, lDlgRcd.destTransIdLen, lDlgRcd.insertTime, lDlgRcd.ssn, lDlgRcd.invokeId, lDlgRcd.dlgType, lDlgRcd.ssapInstance);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP65, 2, lLogText);

                  sprintf(lLogText,"RX: DlgId:%d Msg Calling Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d",lDlgRcd.dlgId, lDlgRcd.callingAddr.sa_gt.sag_num, lDlgRcd.callingAddr.sa_tt, lDlgRcd.callingAddr.sa_np, lDlgRcd.callingAddr.sa_es, lDlgRcd.callingAddr.sa_nai);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP66, 2, lLogText);

                  sprintf(lLogText,"RX: DlgId:%d Msg Called Address Details sa_gt.sag_num:%d sa_tt:%d sa_np:%d sa_es.sag_num:%d sa_nai:%d", lDlgRcd.dlgId, lDlgRcd.calledAddr.sa_gt.sag_num, lDlgRcd.calledAddr.sa_tt, lDlgRcd.calledAddr.sa_np, lDlgRcd.calledAddr.sa_es, lDlgRcd.calledAddr.sa_nai);
                  T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP67, 2, lLogText);

                  memcpy(&lDlgRcd.applicationContext, &lTcapMsg.applicationContext, sizeof(lTcapMsg.applicationContext));

                  if(false == mTransDlgMap.MapDlgIdtoTransId(lDlgRcd))
                  {
                     sprintf(lLogText,"RX: DlgId:%d Mapping dialog id to Transaction failed for storing dialog record", 
                           lDlgRcd.dlgId);
                     T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP68, 2, lLogText);
                  }
               }
               else
               {
                  if(mPegFlag)
                     gLog.GenerateLog(PEG_DROP_RCVD_FROM_NWK);
                  //Unblock the transaction
                  sprintf(lLogText, 
                        "RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X New transaction MapTransIdToDlgId(allocate dialogue) failed ",lDlgRcd.dlgId, lTransPtr, lLocId, lRemId);
                  T(gTrace,printf("%s :%s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP21,lLogText);

                  /////// new
                  mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(lMsgPtr);

                  //if(lLocId == 0)
                  if(lNewLocId == 0 || lLocId != lNewLocId)
                  {
                     sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                           lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                     TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                     gLog.GenerateLog(ACUTCAP109,lLogText);
                     return false;
                  }
                  mAculabApi.DeleteAcuTcapTrans(lTransPtr);

                  return false;
               }
            }
            else
            {
               //Incoming fresh request but not a begin type
               if(mPegFlag)
                  gLog.GenerateLog(PEG_DROP_RCVD_FROM_NWK);

               sprintf(lLogText,"RX: Msg rcv dropped %s(fresh request) from stack",
                     AculabUtil::GetAcuTcapMsgTypeStr(lMsgPtr->tm_msg_type));
               TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););

               gLog.GenerateLog(ACUTCAP170,lLogText);

               mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);
               //Unblock the transaction
               acu_tcap_trans_unblock(lTransPtr);
               acu_tcap_msg_free(lMsgPtr);

               //if(lLocId == 0)
               if(lNewLocId == 0 || lLocId != lNewLocId)
               {
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                        lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                  TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP109,lLogText);
                  return false;
               }

               mAculabApi.DeleteAcuTcapTrans(lTransPtr);

               return false;
            }
         }

         //Clearing transaction and message pointers after
         //transmitting message(END) to application
         switch(lMsgPtr->tm_msg_type)
         {
            case ACU_TCAP_MSG_P_ABORT:
               { 
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg rcv ACU_TCAP_MSG_P_ABORT from stack,"
                        "deleting transaction",lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                  T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP165,lLogText);

                  mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

                  //Unblock the transaction
                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(lMsgPtr);

                  if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
                  {
                     //if(lLocId == 0)
                     if(lNewLocId == 0 || lLocId != lNewLocId)
                     {
                        sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                              lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                        TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP109,lLogText);
                     }
                     else
                     {
                        mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                     }
                  }

                  break;
               } 
            case ACU_TCAP_MSG_LOCAL_ABORT:
               {
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg rcv ACU_TCAP_MSG_LOCAL_ABORT from stack,"
                        "deleting transaction",
                        lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                  T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP166,lLogText);

                  mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

                  //Unblock the transaction
                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(lMsgPtr);

                  if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
                  {
                     //if(lLocId == 0)
                     if(lNewLocId == 0 || lLocId != lNewLocId)
                     {
                        sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                              lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                        TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP109,lLogText);
                     }
                     else
                     {
                        mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                     }
                  }

                  break;
               }
            case ACU_TCAP_MSG_ITU_END:
               {  
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg rcv ACU_TCAP_MSG_ITU_END from stack,"
                        " deleting  transaction", lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                  T(gTrace,printf ("%s: %s\n",gProcessName,lLogText););
                  gLog.GenerateLog(ACUTCAP171,lLogText);

                  mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

                  //Unblock the transaction
                  acu_tcap_trans_unblock(lTransPtr);   
                  acu_tcap_msg_free(lMsgPtr);

                  if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
                  {
                     //if(lLocId == 0)
                     if(lNewLocId == 0 || lLocId != lNewLocId)
                     {
                        sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                              lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                        TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP109,lLogText);
                     }
                     else
                     {
                        mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                     }
                  }

                  break;
               }   
            case ACU_TCAP_MSG_ITU_ABORT:
               {
                  sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X Msg rcv ACU_TCAP_MSG_ITU_ABORT from stack,"
                        "deleting  transaction", lTcapMsg.dialogueId, lTransPtr, lLocId, lRemId);
                  T(gTrace,printf ("%s: %s\n",gProcessName, lLogText););
                  gLog.GenerateLog(ACUTCAP167, lLogText);

                  mAculabApi.GetTransIds(lMsgPtr, &lNewLocId, &lRemId, &lRemIdLen);

                  //Unblock the transaction
                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(lMsgPtr);

                  if(true == mTransDlgMap.DeleteDlgInfo(lTcapMsg.dialogueId))
                  {
                     //if(lLocId == 0)
                     if(lNewLocId == 0 || lLocId != lNewLocId)
                     {
                        sprintf(lLogText,"RX: DlgId:%d TransPtr:%08X lTId:%08X rTId:%08X nTId:%08X Dialog Completed Dropping Message",
                              lDlgRcd.dlgId, lTransPtr, lLocId, lRemId, lNewLocId);
                        TERR(gTrace,printf("%s: %s\n",gProcessName,lLogText););
                        gLog.GenerateLog(ACUTCAP109,lLogText);
                     }
                     else
                     {
                        mAculabApi.DeleteAcuTcapTrans(lTransPtr);
                     }
                  }
                  break;
               }   
            default:
               {
                  //In case of Begine and continue unblock the transaction
                  //and release the message buffer
                  acu_tcap_trans_unblock(lTransPtr);
                  acu_tcap_msg_free(lMsgPtr);
                  break;
               }   

         }

         if((lRetVal == true) && (lTcapMsg.componentPresent == false))
         {
            lRetVal = TxMsgToApplication(lTcapMsg);
         }

         if((lRetVal ==true) && (lTcapMsg.componentPresent == true))
         {
            for(int i = 0; i < lComponentCount; i++)
            {
               if((i == 1)&& (lTcapMsg.tcapDlg == TCAP_BEGIN))
               {
                  lTcapMsg.tcapDlg = TCAP_BEGIN_CONTINUE;
               }
               lTcapMsg.tcapComponent = lTcapComponent[i];
               lRetVal = TxMsgToApplication(lTcapMsg);
            } 
         }

      }
      else
      {
         if(mPegFlag)
            gPeg.PegEvent(PEG_DROP_RCVD_FROM_NWK);

         sprintf(lLogText,"RX: TransPtr:%08X Retriving transaction ids failed transaction deleted !!!!!", lTransPtr);
         TERR(gTrace,printf ("%s: %s\n",gProcessName,
                  "Retriving transaction ids failed transaction deleted"););
         gLog.GenerateLog(ACUTCAP152, lLogText);
         //Unblock the transaction
         acu_tcap_trans_unblock(lTransPtr);
         acu_tcap_msg_free(lMsgPtr);

         mAculabApi.DeleteAcuTcapTrans(lTransPtr);

         lRetVal =  false;
      }
   } 
   return lRetVal;
}

//-------------------------------------------------------------------------------
// METHOD      : RxMsgFromApplication 
// DESCRIPTION : Recieves application tcap message from application 
//               message queue
// PARAMETER   : TcapMsg (application tcap message structure)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TcapAculabHandler::RxMsgFromApplication(TcapMsg &lRxTcapMsg)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16          lMsgLen = sizeof (TcapMsg);
   TEXT           lAddrText[100] = "";
   INT16          msgType = mMsgType;
   msgQueueError  lQueErr;

   memset (&lRxTcapMsg, 0, lMsgLen);
   // Read Messages from the Handler Queue
   lQueErr = mMsgQ.RdQ.ReadMsg((void *)&lRxTcapMsg, lMsgLen, msgType, BLOCKING);

   if (Q_SUCCESS != lQueErr)
   {   
      return AculabUtil::HandleQueueReadError (lQueErr, mMsgQ.RdQ);
   }

   if (0 == lRxTcapMsg.dialogueId)
   {
      // Send HeartBeat Rsp
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "TX: Received invalid Dialog ID :%d", lRxTcapMsg.dialogueId);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if(mPegFlag)
      gPeg.PegEvent(PEG_RCVD_FROM_APP);

   AculabUtil::GetDestAddress(lRxTcapMsg.destAddress, lAddrText);

   snprintf (lLogText, MAX_LOG_TEXT_LEN, "TX: DlgId:%d Msg:%s %s QueueKey:%d Msg Rcv from TcapHdlr Queue",
         lRxTcapMsg.dialogueId, AculabUtil::ConvertTcapDlgTypeToStr(lRxTcapMsg.tcapDlg), lAddrText, mMsgQ.RdQ.GetQueueKey ());

   T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog(ACUTCAP27,lLogText);

   PrintApplTcapStruct(lRxTcapMsg,"TX");
   return true;

}

//-------------------------------------------------------------------------------
// METHOD      : PrintApplTcapStruct 
// DESCRIPTION : Prints Application tcap structure
// PARAMETER   : TcapMsg (application tcap message structure)
// RETURN      : void  
//-------------------------------------------------------------------------------
void TcapAculabHandler::PrintApplTcapStruct(TcapMsg &lTcapMsg,TEXT *lText)
{
   if(0 != mDisplayParam )
   {   
      T (gTrace, printf("%s APPLIACTION -----DlgId:%d---------------------------\n",lText,lTcapMsg.dialogueId);
            if(mDisplayParam & 1)
            {
            mDispApi.DisplayTcapDLgAndTrans(lTcapMsg);
            }
            if(mDisplayParam & 2)
            {
            mDispApi.DisplayAddress(&lTcapMsg.origAddress,"Orig");
            }
            if(mDisplayParam & 4)
            {
            mDispApi.DisplayAddress(&lTcapMsg.destAddress,"Dest");
            }
            if(mDisplayParam & 8)
            {
            mDispApi.DisplayApplicationContext(&lTcapMsg.applicationContext);
            }
            if(mDisplayParam & 16)
            {
            mDispApi.DisplayUserInformation(&lTcapMsg.userInformation);
            }
            if(mDisplayParam & 32)
            {
               mDispApi.DisplayComponent(lTcapMsg);
            }
            printf("----------------------------------------------------\n"););
   }
}
