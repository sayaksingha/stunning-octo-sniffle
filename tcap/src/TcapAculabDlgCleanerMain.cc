static const char* id="@(#) $Id: TcapAculabDlgCleanerMain.cc,v 1.1.4.2.14.1 2021/12/01 12:43:26 jamesjac Exp $";
//---------------------------------------------------------------
// NAME : TcapAcuDlgCleanerMain.cc 
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
// Originator  : Paresh                      Date: 30/03/10
//----------------------------------------------------------------

#include "TcapAculabDlgCleaner.h"

Log             gLog;
CTrace          gTrace(TRACE_ACU_TCAP_DLG_CLEAN_ENV);
TEXT            gProcessName[SS7_MAX_PROCESS_NAME + 1] = "";

//HealthCheckApi  gHealthCheckApi;

//-------------------------------------------------------------
// METHOD        : main
// DESCRIPTION   : main function
// PARAMETER     : int argc, char *argv[]
// RETURN        : int 
//-------------------------------------------------------------
int main(int argc,char *argv[])
{
   ProcessLock   lProcessLock;
   TEXT          lLogText[MAX_LOG_TEXT_LEN+ 1] = "";

   if(argc != 1)
   {
      printf("Usage: Exec\n");
      printf("TRACE: export %s=1 \n",TRACE_ACU_TCAP_DLG_CLEAN_ENV);
      exit(1);
   }

   strcpy(gProcessName, basename(argv[0]));

   //Load Signals
   AculabUtil::LoadSignals();

   //Set Process Name
   gLog.SetProcessName(gProcessName);
   //Set Trace Key
   gTrace.SetTraceKey(TRACE_ACU_TCAP_DLG_CLEAN_ENV);

   printf("%s: Process Starting....\n", gProcessName);

   //Initialise Log object
   if (false == gLog.Init())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Failed to initialize Log Obj. Exiting...");
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText);)   
         gLog.GenerateLog(GSYS04, lLogText);
      exit(1);
   }


   // Set ProcessLock
   lProcessLock.SetProdProcName(TCAP_NAME, gProcessName);

   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Process already running. Exiting...");
      TERR(gTrace, printf("%s: \33[35m%s\33[0m\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS16, lLogText);
      exit(1);
   }

   gLog.GenerateLog(GSYS01);

   DlgCleaner   lDlgCleaner;
   if(false == lDlgCleaner.Init())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to initialise Dlg Cleaner object. Exiting...");
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText);)   
         gLog.GenerateLog(GSYS04, lLogText);
      exit(1);
   }
   sprintf(lLogText,"Process initialization okay...");
   printf("%s: \33[32m%s\33[0m\n", gProcessName, lLogText);
   gLog.GenerateLog(GSYS03);

   while(true == AculabUtil::KeepRunning())
   {
      if (true == AculabUtil::ReloadConfig())
      {
         T(gTrace, 
               printf("%s: Reloading Configurations...\n", 
                  gProcessName););
         lDlgCleaner.ReloadConfig();
         AculabUtil::ResetConfigFlag();
      }

      lDlgCleaner.CleanTimedoutDlg();
      sleep(3);
   }

   lDlgCleaner.DeInit();
   printf("%s: Proces Shutting Down....\n", 
         gProcessName);
   return 0;
}
