static const char *id = "@(#) $Id: TcapAculabHandlerMain.cc,v 1.1.4.8.4.1.2.1 2022/04/28 09:45:28 mram Exp $";
//----------------------------------------------------------------------
// NAME :TcapAculabHandlerMain.cc
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
// Originated : 23-JAN-2010                                       Paresh
//----------------------------------------------------------------------
#include "TcapAculabHandler.h"

//----------------------------------------------------------------------
Log      gLog;
PegApi   gPeg;
CTrace   gTrace (TRACE_ACU_TCAP_HDLR_ENV);
TEXT     gProcessName[ACU_TCAP_MAX_PROCESS_NAME];
UINT32   gMsgCounter;
BOOLEAN  gRestoreStatus;

pthread_t gRxThreadId[MAX_ACU_TCAP_INSTANCES +1];
pthread_t gTxThreadId[MAX_ACU_TCAP_INSTANCES +1];
void *RxThread (void *);
void *TxThread (void *);

SsapDetailsStruct gSsapDetails;

//----------------------------------------------------------------------
// METHOD      : RxThread 
// DESCRIPTION : Thread execution C function
// PARAMETER   : void *
// RETURN      : void
//----------------------------------------------------------------------
void * RxThread (void *lPtr)
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   AcuThreadStruct *lTempPtr = (AcuThreadStruct *) lPtr;
   TcapAculabHandler *lTcapAculabHandler =
      (TcapAculabHandler *) lTempPtr->pTcapAculabHandler;

   int lInstanceNo = (*lTempPtr).instanceNo;
   timeval lTimeValue;
   double lStartTime = 0;

   while (AculabUtil::KeepRunning ())
   {
      acu_tcap_msg_t *lMsg;

      //gettimeofday (&lTimeValue, NULL);
      //lStartTime =
      //   lTimeValue.tv_sec + (lTimeValue.tv_usec / 1000000.00);

      if (true == lTcapAculabHandler->RxMsgFromStack (&lMsg, lInstanceNo))
      {
         //lTempPtr->pMsg = lMsg; Not required

         TcapMsg lTcapMsg;
         memset (&lTcapMsg, 0, sizeof (TcapMsg));

         lTcapAculabHandler->ProcessRxMsgFromStack (lMsg, lTcapMsg, lInstanceNo);
      }

      //gettimeofday (&lTimeValue, NULL);
      //sprintf(lLogText, " RX Processing Time:%f\n",
      //      (lTimeValue.tv_sec +
      //      (lTimeValue.tv_usec / 1000000.00)) - lStartTime);
      //T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      //gLog.GenerateLog(ACUTCAP180, 3, lLogText);
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
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   AcuThreadStruct *lTempPtr = (AcuThreadStruct *) lPtr;
   TcapAculabHandler *lTcapAculabHandler =
      (TcapAculabHandler *) lTempPtr->pTcapAculabHandler;

   int lInstanceNo = (*lTempPtr).instanceNo;

   timeval lTimeValue;
   double lStartTime = 0;

   while (AculabUtil::KeepRunning ())
   {
      if( true == gRestoreStatus)
      {
         TcapMsg lTcapMsg;
         acu_tcap_msg_t lMsgPtr;

         //gettimeofday (&lTimeValue, NULL);
         //lStartTime =
         //lTimeValue.tv_sec + (lTimeValue.tv_usec / 1000000.00);
         if (true == lTcapAculabHandler->RxMsgFromApplication (lTcapMsg))
         {
            lTcapAculabHandler->ProcessTxMsgToStack (lTcapMsg, &lMsgPtr);
         }

         //gettimeofday (&lTimeValue, NULL);
         //      sprintf(lLogText, " TX Processing Time:%f\n", 
         //      (lTimeValue.tv_sec +
         //          (lTimeValue.tv_usec / 1000000.00)) - lStartTime);
         //T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         //gLog.GenerateLog(ACUTCAP179, 3, lLogText);
      }
      else
      {
         sleep(1);
         sprintf(lLogText,"RX Msg From App Waiting for Restoration:%d", gRestoreStatus);
         T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
         gLog.GenerateLog(ACUTCAP105, 1, lLogText);
      }
   }

   return NULL;
}

//----------------------------------------------------------------------
// METHOD      : CreateThreads
// DESCRIPTION : function creates threads 
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
BOOLEAN HandleThreads(TcapAculabHandler *tcapAculabHandlerObj)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   BOOLEAN     lRetVal;
   pthread_t   lThreadId;

   map<UINT32, SsapDetails>::iterator lmSsapDetails = gSsapDetails.begin();
   SsapDetails lSsapDetails;

   for(; lmSsapDetails != gSsapDetails.end(); lmSsapDetails++)
   {
      lSsapDetails = lmSsapDetails->second;

      AcuThreadStruct lAcuThreadStruct;
      lAcuThreadStruct.pTcapAculabHandler = tcapAculabHandlerObj;

      for (int i = 0; i < lSsapDetails.numOfInstances; i++)
      {
         if(CONNECT != lSsapDetails.instance[i].status)
            continue;

         lAcuThreadStruct.instanceNo = lSsapDetails.instance[i].instanceId;

         lRetVal =
            pthread_create (&lThreadId, NULL, &RxThread, &lAcuThreadStruct);
         if (0 == lRetVal)
         {
            printf ("%s: Tid:%lu Rx Thread(%d) Creation success\n",
                  gProcessName, lThreadId, lAcuThreadStruct.instanceNo);

            pthread_detach (lThreadId);
            gRxThreadId[lAcuThreadStruct.instanceNo] = lThreadId;
         }
         else
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "ErrCode:%d Rx Thread Creation failed", lRetVal);
            printf ("%s:\33[31m %s. Exiting...\33[0m\n", gProcessName, lLogText);

            return false;
         }

         sleep (1);

         lRetVal =
            pthread_create (&lThreadId, NULL, &TxThread, &lAcuThreadStruct);
         if (0 == lRetVal)
         {
            printf ("%s: Tid:%lu Tx Thread(%d) Creation success\n",
                  gProcessName, lThreadId, lAcuThreadStruct.instanceNo);

            pthread_detach (lThreadId);
            gTxThreadId[lAcuThreadStruct.instanceNo] = lThreadId;
         }
         else
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error Tx Thread Creation failed. ErrCode:%d)", lRetVal);
            printf ("%s:\33[31m %s Exiting...\33[0m\n", gProcessName, lLogText);

            return false;
         }

         sleep (1);
         lSsapDetails.instance[i].status = ACTIVE;
      }
      lmSsapDetails->second = lSsapDetails;
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

   if (argc != 3)
   {
      printf ("Usage: %s <ssn> <no of thread instances>\n",
            basename (argv[0]));
      printf ("export %s=1\n", TRACE_ACU_TCAP_HDLR_ENV);

      return 1;
   }

   int lSsn = atoi (argv[1]);
   int lNoOfInstance = atoi (argv[2]);

   if (lNoOfInstance > MAX_ACU_TCAP_INSTANCES)
   {
      printf ("Maximum thread instances can't be more than %d\n",
            MAX_ACU_TCAP_INSTANCES);
      printf ("Usage: %s <ssn> <no of thread instances>\n",
            basename (argv[0]));
      printf ("export %s=1\n", TRACE_ACU_TCAP_HDLR_ENV);

      return 1;
   }


   if (lSsn <= 0 || lSsn >= 255)
   {
      printf ("Please enter a valid SSN for TCAP Handler\n");
      printf ("Usage: %s <ssn> <no of thread instances>\n",
            basename (argv[0]));
      printf ("export %s=1\n", TRACE_ACU_TCAP_HDLR_ENV);

      return 1;
   }

   //sprintf (gProcessName, "%s_%d", basename (argv[0]), lSsn);
   sprintf (gProcessName, "ACUTCAP_%d", lSsn);

   gLog.SetProcessName (gProcessName);


   TcapAculabHandler lTcapAculabHandler;

   sprintf (lCfgFile, "Tcap_%d.cfg", lSsn);

   gLog.SetProcessName (gProcessName);
   printf ("%s:\33[32m Starting...\33[0m\n", gProcessName);

   // Initialize log object
   if (true != gLog.Init())
   {
      printf ("%s:\33[31m Log object Init Failed. Exiting...\33[0m\n",
            gProcessName);

      return 1;
   }

   AculabUtil::LoadSignals();
   AculabUtil::SetTransmitFlag(MAX_ACU_TCAP_INSTANCES);

   lProcessLock.SetProdProcName (TCAP_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      printf ("%s:\33[31m Process already running. Exiting...\33[0m\n",
            gProcessName);

      return 1;
   }

   // Startup log
   gLog.GenerateLog (GSYS01);

   if (true != gPeg.Init ("SHM_TCAP_PEG_KEY"))
   {
      gLog.GenerateLog (GSYS04, "PegApi object Init Failed");
      printf ("%s:\33[31m PegApi object Init Failed. Exiting...\33[0m\n",
            gProcessName);

      return 1;
   }

   if (true != lTcapAculabHandler.Init (lCfgFile, lSsn, lNoOfInstance))
   {
      gLog.GenerateLog (GSYS04, "Handler object Init Failed");
      printf ("%s:\33[31m Handler object Init Failed. Exiting...\33[0m\n",
            gProcessName);

      return 1;
   }

   //usleep(1000);

   if(false == HandleThreads(&lTcapAculabHandler))
   {
      gLog.GenerateLog (GSYS04, "Failed Creating Threads");
      printf ("%s:\33[31m Failed Created Threads. Exiting...\33[0m\n",
            gProcessName);

      return 1;
   }

   gLog.GenerateLog (GSYS03);
   printf ("%s:\33[32m Initialization Okay...\33[0m\n", gProcessName);

   //AculabUtil::ResetConfigFlag();
   //AculabUtil::PrintConfig();

   while (AculabUtil::KeepRunning())
   {

      if (true == AculabUtil::ReloadConfig())
      {
         T(gTrace, printf("%s: Reading ConfigData on SIGCFG\n", gProcessName););
         AculabUtil::HandleSignal(AculabUtil::GetSignalNo());

         lTcapAculabHandler.ReloadConfig(lNoOfInstance);
         HandleThreads(&lTcapAculabHandler);
         AculabUtil::ResetConfigFlag();
      }

      sleep (3);

      for (int lCount = 0; lCount < lNoOfInstance; lCount++)
      {
         lTcapAculabHandler.UpdateSsapStatus((const int)lCount);
         if (true != lTcapAculabHandler.GetSsapStatus ((const int)lCount))
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error Ssap instance %d running status false", lCount);
            printf ("%s:\33[31m %s Reconnecting SSAP\33[0m\n",
                  gProcessName, lLogText);
            TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
               gLog.GenerateLog(ACUTCAP33, lLogText);

            //pthread_kill (gRxThreadId[lCount], 30);
            //pthread_kill (gTxThreadId[lCount], 30);

            if(false == lTcapAculabHandler.ReconnectSsap (lCount))
            {
               sprintf(lLogText, "Failed in recreating faulty SSAP instance : %d", lCount);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
                  gLog.GenerateLog(ACUTCAP34, lLogText);
               continue;
            }

            HandleThreads(&lTcapAculabHandler);
            usleep (1000);
         }
      }
   }

   gLog.GenerateLog (GSYS02);
   printf ("%s:\33[32m Shutting down...\33[0m\n", gProcessName);

   return 0;
}
