static const char *id = "@(#) $Id: SccpAculabHandlerMain.cc,v 1.1.4.1.4.1 2021/12/01 12:43:25 jamesjac Exp $";
//----------------------------------------------------------------------
// NAME :SccpAculabHandlerMain.cc
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
// Originated By:Keerthini AB                         Date:25-Aug-2014
//----------------------------------------------------------------------
#include "SccpAculabHandler.h"

//----------------------------------------------------------------------
Log      gLog;
PegApi   gPeg;
CTrace   gTrace (TRACE_ACU_SCCP_HDLR_ENV);
TEXT     gProcessName[ACU_SCCP_MAX_PROCESS_NAME];

pthread_t gRxThreadId[MAX_ACU_SCCP_INSTANCES +1];
pthread_t gTxThreadId[MAX_ACU_SCCP_INSTANCES +1];
void *RxThread (void *);
void *TxThread (void *);

TEXT gCfgFile[MAX_LOG_TEXT_LEN + 1] = "";
//----------------------------------------------------------------------
// METHOD      : RxThread
// DESCRIPTION : Thread execution C function
// PARAMETER   : void *
// RETURN      : void
//----------------------------------------------------------------------
void * RxThread (void *lPtr)
{
   SccpAculabHandler *lSccpAculabHandler = (SccpAculabHandler*) lPtr;

   while (AculabUtil::KeepRunning ())
   {
      acu_sccp_msg_t *lMsg;

      if (true == lSccpAculabHandler->RxMsgFromStack (&lMsg))
      {
         //printf("Msg From stack processed and sent to application successfully...");
      }
   }
   return NULL;
}

//----------------------------------------------------------------------
// METHOD      : TxThread
// DESCRIPTION : Thread execution C function
// PARAMETER   : void *
// RETURN      : void
//----------------------------------------------------------------------
void * TxThread (void *lPtr)
{
   SccpAculabHandler *lSccpAculabHandler = (SccpAculabHandler*) lPtr;

   while (AculabUtil::KeepRunning ())
   {
      _SccpInfo lSccpMsg;

      if (true == lSccpAculabHandler->RxMsgFromApplication (lSccpMsg))
      {
         // Block until Aculab SSAP is fully connected before sending to stack
         while (true != lSccpAculabHandler->GetSsapStatus() && AculabUtil::KeepRunning())
         {
            sleep(1);
         }
         
         lSccpAculabHandler->ProcessTxMsgToStack (lSccpMsg);
      }
   }
   return NULL;
}

//----------------------------------------------------------------------
// METHOD      : HandleThreads
// DESCRIPTION : function creates threads
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HandleThreads(SccpAculabHandler *sccpAculabHandlerObj)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BOOLEAN     lRetVal;
   pthread_t   lThreadId;

      lRetVal =
         pthread_create (&lThreadId, NULL, &TxThread, sccpAculabHandlerObj);
      if (0 == lRetVal)
      {
         T(gTrace, printf ("%s: Tid:%lu Tx Thread Creation success\n",
               gProcessName, lThreadId););

         pthread_detach (lThreadId);
         gTxThreadId[0] = lThreadId;
      }
      else
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Error Tx Thread Creation failed. ErrCode:%d)", lRetVal);
         TERR(gTrace, printf ("%s:\33[31m %s Exiting...\33[0m\n", gProcessName, lLogText););

         return false;
      }

      sleep (1);

      lRetVal =
         pthread_create (&lThreadId, NULL, &RxThread, sccpAculabHandlerObj);
      if (0 == lRetVal)
      {
         T(gTrace, printf ("%s: Tid:%lu Rx Thread Creation success\n",
               gProcessName, lThreadId););

         pthread_detach (lThreadId);
         gRxThreadId[0] = lThreadId;
      }
      else
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "ErrCode:%d Rx Thread Creation failed", lRetVal);
         TERR(gTrace, printf ("%s:\33[31m %s. Exiting...\33[0m\n", gProcessName, lLogText););

         return false;
      }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION : Main function
// PARAMETER   : int , char **
// RETURN      : int
//----------------------------------------------------------------------
int main (int argc, char *argv[])
{
   ProcessLock lProcessLock;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lCfgFile[MAX_LOG_TEXT_LEN + 1] = "";
/*
   if (argc != 2)
   {
      printf ("Usage: %s <ssn>\n",
            basename (argv[0]));
      printf ("export %s=1\n", TRACE_ACU_SCCP_HDLR_ENV);

      return 1;
   }
*/

      if (argc != 2)
   {
      // Modern ASCII Art Banner (SCCP)
      printf("\n");
      printf("  ███████╗ ██████╗ ██████╗ ██████╗ \n");
      printf("  ██╔════╝██╔════╝██╔════╝ ██╔══██╗\n");
      printf("  ███████╗██║     ██║      ██████╔╝\n");
      printf("  ╚════██║██║     ██║      ██╔═══╝ \n");
      printf("  ███████║╚██████╗╚██████╗ ██║     \n");
      printf("  ╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     \n");
      printf("  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
      
      // Product Name & Version
      printf("  \033[1;36mProduct     :\033[0m Aculab ANSI SCCP Handler\n");
      printf("  \033[1;36mVersion     :\033[0m %s \n",ANSI_PRODUCT_VER);
      
      // Description (What it does)
      printf("  \033[1;36mDescription :\033[0m Facilitates Signalling Connection Control Part\n");
      printf("                (SCCP) routing over SS7 networks. It provides\n");
      printf("                extended routing, flow control,\n");
      printf("                and Global Title Translation (GTT) capabilities.\n\n");
      
      // ANSI Standard Info
      printf("  \033[1;36mCompliance  :\033[0m Implements the ANSI (American National Standards\n");
      printf("                Institute) SS7 specification for SCCP. It ensures\n");
      printf("                reliable connectionless and connection-oriented\n");
      printf("                network services for North American telecom nodes.\n");
      printf("  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");

      // Usage Instructions
      printf("  \033[1;33mUsage:\033[0m %s <ssn>\n\n", basename(argv[0]));
      
      // Environment variables (Updated for SCCP as an example)
      printf("  \033[1;31mEnvironment Requirement:\033[0m\n");
      printf("  export %s=1\n\n", "TRACE_ACU_SCCP_HDLR_ENV"); // Replace with your actual SCCP ENV macro

      return 1;
   }

   int lSsn = atoi (argv[1]);

   if (lSsn <= 0 || lSsn >= 255)
   {
      printf ("Please enter a valid SSN for SCCP Handler\n");
      printf ("Usage: %s <ssn>\n",
            basename (argv[0]));
      printf ("export %s=1\n", TRACE_ACU_SCCP_HDLR_ENV);

      return 1;
   }

   sprintf (gProcessName, "%s_%d", "ACUSCCP", lSsn);

   gLog.SetProcessName (gProcessName);


   SccpAculabHandler lSccpAculabHandler;

   sprintf (lCfgFile, "Sccp_%d.cfg", lSsn);
   strcpy(gCfgFile, lCfgFile);

   gLog.SetProcessName (gProcessName);
   T(gTrace, printf ("%s:\33[32m Starting...\33[0m\n", gProcessName););

   // Initialize log object
   if (true != gLog.Init())
   {
      TERR(gTrace, printf ("%s:\33[31m Log object Init Failed. Exiting...\33[0m\n",
            gProcessName););

      return 1;
   }

   AculabUtil::LoadSignals();
   AculabUtil::SetTransmitFlag();

   lProcessLock.SetProdProcName (SCCP_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      TERR(gTrace, printf ("%s:\33[31m Process already running. Exiting...\33[0m\n",
            gProcessName););

      return 1;
   }

   // Startup log
   gLog.GenerateLog (GSYS01);

   if (true != gPeg.Init ("SHM_SCCP_PEG_KEY"))
   {
      gLog.GenerateLog (GSYS04, "PegApi object Init Failed");
      TERR(gTrace, printf ("%s:\33[31m PegApi object Init Failed. Exiting...\33[0m\n",
            gProcessName););

      return 1;
   }

   if (true != lSccpAculabHandler.Init (lCfgFile, lSsn))
   {
      gLog.GenerateLog (GSYS04, "Handler object Init Failed");
      TERR(gTrace, printf ("%s:\33[31m Handler object Init Failed. Exiting...\33[0m\n",
            gProcessName););

      return 1;
   }

   //usleep(1000);

   if(false == HandleThreads(&lSccpAculabHandler))
   {
      gLog.GenerateLog (GSYS04, "Failed Creating Threads");
      TERR(gTrace, printf ("%s:\33[31m Failed Created Threads. Exiting...\33[0m\n",
            gProcessName););

      return 1;
   }

   gLog.GenerateLog (GSYS03);
   T(gTrace, printf ("%s:\33[32m Initialization Okay...\33[0m\n", gProcessName););

   AculabUtil::ResetConfigFlag();

   while (AculabUtil::KeepRunning())
   {
      if (true == AculabUtil::ReloadConfig())
      {
         T(gTrace, printf("%s: Reading ConfigData on SIGCFG\n", gProcessName););
         AculabUtil::HandleSignal(AculabUtil::GetSignalNo());

         lSccpAculabHandler.ReloadConfig();
         AculabUtil::ResetConfigFlag();
         //HandleThreads(&lSccpAculabHandler);
      }

      sleep (3);

      lSccpAculabHandler.UpdateSsapStatus();
      if (true != lSccpAculabHandler.GetSsapStatus())
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Error Ssap instance running status false");
         TERR(gTrace, printf ("%s:\33[31m %s Reconnecting SSAP\33[0m\n", gProcessName, lLogText););

         //pthread_kill (gRxThreadId[0], 30);
         //pthread_kill (gTxThreadId[0], 30);

         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(ACUSCCP13, lLogText);

         if(false == lSccpAculabHandler.ReconnectSsap())
         {
            sprintf(lLogText, "Failed in recreating faulty SSAP instance");
            TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
               gLog.GenerateLog(GSYS04, lLogText);
            continue;
         }

         HandleThreads(&lSccpAculabHandler);

         usleep (1000);
      }
   }

   gLog.GenerateLog (GSYS02);
   T(gTrace, printf ("%s:\33[32m Shutting down...\33[0m\n", gProcessName););

   return 0;
}

