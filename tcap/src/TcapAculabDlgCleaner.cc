static const char *id = "@(#) $Id: TcapAculabDlgCleaner.cc,v 1.1.4.5.2.1 2021/12/01 12:43:26 jamesjac Exp $";
//---------------------------------------------------------------
// NAME : TcapAcuDlgCleaner.cc 
//
// COPYRIGHT
// TSS : - Copyright (C) 2010 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
//
// Originator  : Paresh                     Date: 30/03/10
//----------------------------------------------------------------

#include "TcapAculabDlgCleaner.h"

extern TEXT gProcessName[ACU_TCAP_MAX_PROCESS_NAME];
extern CTrace gTrace;
extern Log gLog;

//-------------------------------------------------------------
// METHOD        : DlgCleaner
// DESCRIPTION   : Constructor
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
DlgCleaner::DlgCleaner()
{
   //DlgMgr lDlgMgr(gProcessName,TRACE_ACU_TCAP_DLG_CLEAN_ENV);
   //mDlgManager = lDlgMgr;
}

//-------------------------------------------------------------
// METHOD        : ~DlgCleaner
// DESCRIPTION   : Destructor
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
DlgCleaner::~DlgCleaner()
{
}

//-------------------------------------------------------------
// METHOD        : Init 
// DESCRIPTION   : Initialization
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::Init()
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Ab Change: Pass TCAP_ANSI_CFG down to DlgManager
   if(false == mDlgManager.Init((TEXT*)TCAP_ANSI_CFG))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Dialogue Manager Initialization failed. Exiting...");
      T(gTrace, 
            printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04,lLogText);
      return false;
   }

   if(false == ReadConfig())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ReadConfig failed. Exiting...");
      T(gTrace,
            printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04,lLogText);
      return false;
   }

   if(false == CreateMsgQ())
   {
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD        : DeInit
// DESCRIPTION   : DeInitialization
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::DeInit()
{
   //mDlgManager.DeInit();
   return true;
}

//-------------------------------------------------------------
// METHOD        : ReadConfig
// DESCRIPTION   : Read Configuration parameters from file
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::ReadConfig()
{
   TEXT      lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   CfgRead   lCfgRead(gProcessName);

   if (false == lCfgRead.CfgInit(TCAP_ANSI_CFG))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to open the file %s", TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS09,lLogText);
      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_TIMEOUT",
            mDlgTimeout, DLG_TIMEOUT_MIN, DLG_TIMEOUT_MAX ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_TIMEOUT Parameter not found in %s",
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      lCfgRead.CfgDeInit();
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
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_TIMEOUT_CAP = %d\n",
               gProcessName, mCapDlgTimeout););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_CLEANER_SSN",
            mSpecialSsn, 1, 255 ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_CLEANER_SSN Parameter not found in %s",
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_CLEANER_SSN = %d\n",
               gProcessName, mSpecialSsn););
   }

   if (CFG_OK != lCfgRead.GetConfigNum("MAX_ACU_TCAP_DLG_SIZE", 
            mMaxDlgSize, 
            1, 500000))
   {
      sprintf(lLogText, "Configuration Error for MAX_ACU_TCAP_DLG_SIZE in file %s, exiting ...", TCAP_ANSI_CFG);
      T(gTrace, printf("%s\n", lLogText););
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MAX_ACU_TCAP_DLG_SIZE = %d\n", gProcessName, mMaxDlgSize););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("MSG_TCAP_HDLR_Q_RCV",mTcapMsgQKey ,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "MSG_TCAP_HDLR_Q_RCV Parameter not found in %s",TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_TCAP_HDLR_Q_RCV = %d\n",gProcessName,mTcapMsgQKey););
   }

   lCfgRead.CfgDeInit();

   return true;
}

//-------------------------------------------------------------
// METHOD        : ReloadConfig
// DESCRIPTION   : Reloads the configuration from the file
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
void DlgCleaner::ReloadConfig()
{
   TEXT      lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   CfgRead   lCfgRead(gProcessName);

   if(false == lCfgRead.CfgInit(TCAP_ANSI_CFG))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to open the file %s", TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS09,lLogText);
      return;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_TIMEOUT",
            mDlgTimeout, DLG_TIMEOUT_MIN, DLG_TIMEOUT_MAX ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_TIMEOUT Parameter not found in %s",
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
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
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_TIMEOUT_CAP = %d\n",
               gProcessName, mCapDlgTimeout););
   }

   if(CFG_OK != lCfgRead.GetConfigNum("ACU_TCAP_DLG_CLEANER_SSN",
            mSpecialSsn, 1, 255 ))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "ACU_TCAP_DLG_CLEANER_SSN Parameter not found in %s",
            TCAP_ANSI_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11,lLogText);
   }
   else
   {
      T(gTrace, printf("%s: ACU_TCAP_DLG_CLEANER_SSN = %d\n",
               gProcessName, mSpecialSsn););
   }

   lCfgRead.CfgDeInit();
}


//-------------------------------------------------------------
// METHOD        : Init
// DESCRIPTION   : Initialization
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::CreateMsgQ()
{
   msgQueueError lQueErr;
   lQueErr = mTcapMsgQ.Create (mTcapMsgQKey, IPC_CREAT | SS7_IPC_PERM);
   if(Q_SUCCESS != lQueErr)
   {
      printf("%s: Error creating MSG_TCAP_HANDLER_Q_RCV queue key:%d\n",
            gProcessName,mTcapMsgQKey);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD        : Init
// DESCRIPTION   : Initialization
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::SendPreArrangedEnd(unsigned int lDlgId, int lSsn)
{
   //TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (TcapMsg);


   AnsiTcapMsg lTcapMsg;
   memset(&lTcapMsg,0,sizeof(lTcapMsg));

   lTcapMsg.ssn = lSsn;
   lTcapMsg.dialogueId = lDlgId;
   lTcapMsg.tcapDlg = TCAP_PRE_ARRANGED_END;

   msgQueueError lQueErr = mTcapMsgQ.WriteMsg((void *)&lTcapMsg, lMsgLen,
         (LONG) lTcapMsg.ssn, NON_BLOCKING);

   if (Q_SUCCESS != lQueErr)
   {
      if (false ==
            AculabUtil::HandleQueueWriteError(lQueErr,
               mTcapMsgQ, &lTcapMsg,lMsgLen,(int)(lTcapMsg.ssn),NON_BLOCKING))
      {
         TERR(gTrace, printf ("%s: dlgId:%d Write to Queue Failed\n",
                  gProcessName, lTcapMsg.dialogueId););
         return false;
      }
   }
   T(gTrace, printf ("%s: dlgId:%d TCAP_PRE_ARRANGED_END wrote to TcapHdlr Rcv queue\n",
            gProcessName, lTcapMsg.dialogueId););


   return true;

}

//-------------------------------------------------------------
// METHOD        : CleanTimedoutDlg
// DESCRIPTION   : Clears Dlg after certain time period 
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN DlgCleaner::CleanTimedoutDlg()
{
   time_t      lCurrTime;
   time_t      lInsertTime;
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";
   int         lSsn = 0;
   // or MAX_OUT_DLG_ID
   for(unsigned int i = 1; i <= mMaxDlgSize; i++)
   {
      lInsertTime = 0;
      lCurrTime = 0;
      mDlgManager.GetInsertTimeAndSsn(i, lInsertTime, lSsn);
      lCurrTime = time(NULL);

      //Call can last for more than actual MAP dialogues
      //Currently call can last upto maximum of 7200 seconds 
      if(lSsn == mSpecialSsn)
      {
         if(((lCurrTime - lInsertTime) > mCapDlgTimeout) && lInsertTime !=0 )
         {
            if(true == SendPreArrangedEnd(i, lSsn))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "Sending PreArrangedEnd TimedoutDlgId:%u ssn(%d) TimeDiff %ld ",
                     i,lSsn,lCurrTime - lInsertTime);
               T(gTrace, printf("%s: %s\n",gProcessName ,lLogText););
               gLog.GenerateLog(GSYS11,lLogText);

            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "Sending PreArrangedEnd failed TimedoutDlgId:%u ssn(%d) TimeDiff %ld ",
                     i, lSsn,lCurrTime - lInsertTime);
               TERR(gTrace, printf("%s: %s\n", gProcessName,lLogText););
               gLog.GenerateLog(GSYS11,lLogText);
            }

         }
      }

      else if(((lCurrTime - lInsertTime) > mDlgTimeout) && lInsertTime !=0 )
      {
         if(true == SendPreArrangedEnd(i, lSsn))
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Sending PreArrangedEnd TimedoutDlgId:%u ssn(%d) TimeDiff %ld ",
                  i,lSsn,lCurrTime - lInsertTime);
            T(gTrace, printf("%s: %s\n",gProcessName ,lLogText););
            gLog.GenerateLog(GSYS11,lLogText);

         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Sending PreArrangedEnd failed TimedoutDlgId:%u ssn(%d) TimeDiff %ld ",
                  i, lSsn,lCurrTime - lInsertTime);
            TERR(gTrace, printf("%s: %s\n", gProcessName,lLogText););
            gLog.GenerateLog(GSYS11,lLogText);
         }

      }
      if(0 == (i % 2000))
      {
         usleep(1000);
      }

   }
   return true;
}
