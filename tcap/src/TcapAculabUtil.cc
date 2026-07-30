// @(#) $Id: TcapAculabUtil.cc,v 1.1.4.4.4.1 2021/12/01 12:43:27 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME: TcapAculabUtil.cc 
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

#include "TcapAculabUtil.h"

BOOLEAN    AculabUtil::mSigCfg = false;;
BOOLEAN    AculabUtil::mSigTrc = false;
BOOLEAN    AculabUtil::mKeepRunning = true;
BOOLEAN    AculabUtil::mAbortFlag = false;
INT32      AculabUtil::mSignal = 0;
BOOLEAN    AculabUtil::mTransmitFlag[MAX_ACU_TCAP_INSTANCES];
extern CTrace     gTrace;
extern Log        gLog;
extern TEXT       gProcessName[ACU_TCAP_MAX_PROCESS_NAME];

//-------------------------------------------------------------------------------
// METHOD      : PrintConState 
// DESCRIPTION : Prints Conection staus of application with stack 
// PARAMETER   : const char *, const acu_tcap_con_state_t *
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN AculabUtil::PrintConState(TEXT  *lText,  
      const acu_tcap_con_state_t *con_state,
      int &lInstanceNo, TEXT *lHost)
{
   unsigned int ipAddr = con_state->cs_ipaddr;

   if(ipAddr == 0)
   {
      return false;
   }

   TEXT lIpAddr[50] = "";

   sprintf(lIpAddr,"(%d.%d.%d.%d:%d)", 
         (ipAddr >> 24) & 0xff, (ipAddr >> 16) & 0xff,
         (ipAddr >> 8) & 0xff, ipAddr & 0xff, con_state->cs_tcpport);

   if(con_state->cs_state & ACU_TCAP_CON_STATE_IDLE)
   {   
      sprintf(lText,RED "%s %s Disconnected, %s" UNDO, lHost, lIpAddr,con_state->cs_fail_text);
      gLog.GenerateLog(ACUTCAP03,2,lText);
      //return true;
   }

   if(con_state->cs_state & ACU_TCAP_CON_STATE_CONNECTING)
   {   
      sprintf(lText,BLUE "%s %s Connecting" UNDO, lHost, lIpAddr);
      //return true;
   }

   if(con_state->cs_state & ACU_TCAP_CON_STATE_CONNECTED)
   {   
      sprintf(lText,BLUE "%s %s Connected" UNDO, lHost, lIpAddr);
      gLog.GenerateLog(ACUTCAP02,2,lText);
      //return true;
   }

   if(con_state->cs_state & ACU_TCAP_CON_STATE_IN_SERVICE)
   {    
      sprintf(lText,BLUE "%s %s In Service " UNDO, lHost, lIpAddr);
      if(con_state->cs_state & ACU_TCAP_CON_STATE_RX_BLOCKED)
      {   
         sprintf(lText,RED "%s %s In Service receive blocked" UNDO, lHost, lIpAddr);
         gLog.GenerateLog(ACUTCAP17,2,lText);
         return true;
      }

      if(con_state->cs_state & ACU_TCAP_CON_STATE_RX_FLOW)
      {
         sprintf(lText,RED "%s %s In Service receive flow control" UNDO, lHost, lIpAddr);
         gLog.GenerateLog(ACUTCAP18,2,lText);
         return true;
      }

      if(con_state->cs_state &  ACU_TCAP_CON_STATE_TX_BLOCKED)
      {   
         AculabUtil::mTransmitFlag[lInstanceNo] = false; 
         sprintf(lText,RED "%s %s In Service transmit blocked" UNDO,lHost,lIpAddr);
         gLog.GenerateLog(ACUTCAP19,2,lText);
         return true; 
      }
      if(con_state->cs_state &  ACU_TCAP_CON_STATE_TX_FLOW)
      {  
         AculabUtil::mTransmitFlag[lInstanceNo] = false; 
         sprintf(lText,RED "%s %s In Service transmit flow control" UNDO,lHost,lIpAddr);
         gLog.GenerateLog(ACUTCAP20,2,lText);
         return true;
      }
      gLog.GenerateLog(ACUTCAP04,lText);
      AculabUtil::mTransmitFlag[lInstanceNo] = true;
      return true;
   }

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetTransmitFlag 
// DESCRIPTION : returns Transmit flag for transmitting message to stack 
// PARAMETER   : none 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN AculabUtil::GetTransmitFlag(int & lInstanceNo)
{
   return AculabUtil::mTransmitFlag[lInstanceNo];
}

//-------------------------------------------------------------------------------
// METHOD      : GetFreeInstanceForTransmit 
// DESCRIPTION : returns Free Instance to send message to stack 
//               (Instances which are not transmit blocked)
// PARAMETER   : none 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
int AculabUtil::GetFreeInstanceForTransmit(int & lInstances)
{
   int lInstanceCount = 0;

   for(; lInstanceCount < lInstances ; lInstanceCount++)
   {
      if(AculabUtil::mTransmitFlag[lInstanceCount])
         return lInstanceCount;     
   }
   return -1;
}

//-------------------------------------------------------------------------------
// METHOD      : SetTransmitFlag 
// DESCRIPTION : Sets Transmit flags for all threads to true 
// PARAMETER   : UINT16 
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::SetTransmitFlag(int lInstances)
{
   for(int i = 0; i < lInstances ; i ++)
   {
      AculabUtil::mTransmitFlag[i] = true;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : GetAcuTcapMsgTypeStr 
// DESCRIPTION : Converts Acu TCAP Message type to str 
// PARAMETER   : acu_tcap_msg_type_t 
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
TEXT *  AculabUtil::GetAcuTcapMsgTypeStr(acu_tcap_msg_type_t &lMsgType)
{
   switch(lMsgType)
   {
      case ACU_TCAP_MSG_DATA:
         return "DATA";

      case ACU_TCAP_MSG_NOTICE:
         return "NOTICE";

      case ACU_TCAP_MSG_USER_STATUS:
         return "USER_STATUS";

      case ACU_TCAP_MSG_SP_STATUS:
         return "SP_STATUS";

      case ACU_TCAP_MSG_TIMEOUT:
         return "MSG_TIMEOUT";

      case ACU_TCAP_MSG_CON_STATE:
         return "CON_STATE";

      case ACU_TCAP_MSG_P_ABORT:
         return "P_ABORT";

      case ACU_TCAP_MSG_LOCAL_ABORT:
         return "LOCAL_ABORT";

      case ACU_TCAP_MSG_ITU_UNI:
         return "ITU_UNI";

      case ACU_TCAP_MSG_ITU_BEGIN:
         return "ITU_BEGIN";

      case ACU_TCAP_MSG_ITU_END:
         return "ITU_END";

      case ACU_TCAP_MSG_ITU_CONTINUE:
         return "ITU_CONTINUE";

      case ACU_TCAP_MSG_ITU_ABORT:
         return "ITU_ABORT";

      default:
         return "UNHANDLED";
         break;

   }
}
//-------------------------------------------------------------------------------
// METHOD      : ShowAddressIndicator 
// DESCRIPTION : showa address indicator bye in readable format
// PARAMETER   : UINT8
// RETURN      : void 
//-------------------------------------------------------------------------------
void  AculabUtil::ShowAddressIndicator( UINT8 & lAddrInd)
{
   int lReserve = (lAddrInd >> 7) & 1;
   int lRtInd = (lAddrInd >> 6) & 1;
   int lSsnInd = (lAddrInd >> 1) & 1;
   int lPcInd  = (lAddrInd >> 0) & 1;

   int lGtInd4 = (lAddrInd >> 5) & 1;
   int lGtInd3 = (lAddrInd >> 4) & 1;
   int lGtInd2 = (lAddrInd >> 3) & 1;
   int lGtInd1 = (lAddrInd >> 2) & 1;

   int lTemp = (lAddrInd >> 2) & 15;
   TEXT lText[30 + 1] = "";
   switch(lTemp)
   {
      case 0:
         sprintf(lText,"GT not included");
         break;
      case 1:
         sprintf(lText,"GT includes NAI");
         break;
      case 2:
         sprintf(lText,"GT includes TT");
         break;
      case 3:
         sprintf(lText,"GT includes TT NP ES");
         break;
      case 4:
         sprintf(lText,"GT includes TT NP ES NAI");
         break;
      default:
         break;

   }

   T(gTrace,printf("*---------------------------------------------------------------*\n"););
   T(gTrace,printf("|   Bit-7|           Bit-6| Bit-5-4-3-2|       Bit-1|      Bit-0|\n"););
   T(gTrace,printf("|Reserved|RoutingIndicator|GtInidicator|SsnIndicator|PcIndicator|\n"););
   T(gTrace,printf("|%8d|%16d|%9d%d%d%d|%12d|%11d|\n",
            lReserve,lRtInd,lGtInd4,lGtInd3,lGtInd2,lGtInd1,lSsnInd,lPcInd);); 
   if(lRtInd == 0)
   {   
      T(gTrace,printf("|---------------------------------------------------------------|\n"););
      T(gTrace,printf("|%-63s|\n",lText););
   }
   T(gTrace,printf("*---------------------------------------------------------------*\n"););


}


//-------------------------------------------------------------------------------
// METHOD      : PrintSccpStatus 
// DESCRIPTION : Prints SCCP Connection Status .
// PARAMETER   : acu_tcap_msg_t *msg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN  AculabUtil::ErrReportSccp(acu_tcap_msg_t  *msg)
{
   gTrace.PrintTraceTimeStamp("Y");
   printf("%s:%s ACU_TCAP_MSG_NOTICE ret_opt:%d %s\n",gProcessName,RED,msg->tm_ret_opt,UNDO);
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : PrintSccpStatus 
// DESCRIPTION : Prints SCCP Connection Status .
// PARAMETER   : acu_tcap_msg_t *msg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN AculabUtil::PrintSccpStatus(acu_tcap_msg_t *msg)
{
   const acu_tcap_sccp_status_t *sccp_status;

   if (acu_tcap_msg_get_sccp_status(msg, &sccp_status) != 0) {
      /* This is the only reason the above can fail */
      //Error
      return false;
   }


   gTrace.PrintTraceTimeStamp("Y");
   printf("%s:%s %s host %c:PC:%d",
         gProcessName,BLUE,msg->tm_msg_type == ACU_TCAP_MSG_USER_STATUS ? "USER" : "SP",
         sccp_status->tsp_host, sccp_status->tsp_pc);


   if (msg->tm_msg_type == ACU_TCAP_MSG_USER_STATUS) 
   {
      printf(" SSN:%d", sccp_status->tsp_ssn);
      switch (sccp_status->tsp_user_status) {
         case ACU_SCCP_UOS:
            printf(", User out of service");
            break;
         case ACU_SCCP_UIS:
            printf(", User in service");
            break;
         default:
            printf(", User state %d", sccp_status->tsp_user_status);
            break;
      }
   }

   switch (sccp_status->tsp_sp_status) 
   {
      case ACU_SCCP_SP_PROHIBIT:
         printf(RED ", Signalling point prohibited" UNDO);
         break;
      case ACU_SCCP_SP_ACCESS:
         printf(", Signalling point accessible");
         break;
      default:
         printf(", Signalling point state %d", sccp_status->tsp_sp_status);
         break;
   }

   switch (sccp_status->tsp_sccp_status) {
      case ACU_SCCP_REM_SCCP_PROHIBIT:
         printf(RED ", Remote SCCP prohibited" UNDO);
         break;
      case ACU_SCCP_REM_SCCP_UNAVAIL:
         printf(RED ", Remote SCCP unavailable" UNDO);
         break;
      case ACU_SCCP_REM_SCCP_UNEQUIP:
         printf(RED ", Remote SCCP unequipped" UNDO);
         break;
      case ACU_SCCP_REM_SCCP_INACCESS:
         printf(RED ", Remote SCCP inaccessible" UNDO);
         break;
      case ACU_SCCP_REM_SCCP_CONGEST:
         printf(RED ", Remote SCCP congested" UNDO);
         break;
      case ACU_SCCP_REM_SCCP_AVAIL:
         printf(", Remote SCCP available");
         break;
      default:
         printf(", Remote SCCP state %d", sccp_status->tsp_sccp_status);
         break;
   }
   printf("%s\n",UNDO);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : DispOctetString 
// DESCRIPTION : Displays Octect data
// PARAMETER   : char * 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN AculabUtil::DispOctetString(char *string)
{
   int i = 0;
   int len = strlen(string);
   for (; i < len; i++)
      printf(" %2.2x", string[i]);
   putchar('\n');

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ReturnAculabErrStr
// DESCRIPTION : Returns Error string with respect to error no
// PARAMETER   : int lErrCode,TEXT *lErrText
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::ReturnAculabErrStr(int lErrCode,TEXT *lErrText)
{
   if (lErrCode <= 0)
   {
      sprintf(lErrText,"[%d](%s)",lErrCode,acu_tcap_strerror(lErrCode, 1));
   }
}


//-------------------------------------------------------------------------------
// METHOD      : ReturnAculabErrStr
// DESCRIPTION : Returns Error string with respect to error no
// PARAMETER   : int lErrCode,TEXT *lErrText
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::DisplayAddress (const TCAPAddress * address,TEXT *lText)
{
   if (NULL == address)
   {
      printf("%s:%sAddress: Absent\n",gProcessName,lText);
      return;
   }

   printf("%s: %sAddress: SSN:%d PC:%d NOA:%d TT:%d NP:%d ES:%d\n",
         gProcessName,lText,
         address->subsystemNumber,address->pointCode,
         address->natureOfAddress,address->translationType,
         address->numberingPlan,address->encodingScheme);
   if(address->numberOfDigits != 0)
   {   
      printf("%s: %sAddress Digits:",gProcessName,lText);
      for (int i = 0; i <  address->numberOfDigits; i++)
         printf("%X", address->digits[i]);
      printf("\n");
   }    
}

//-------------------------------------------------------------------------------
// METHOD      : ReturnAculabErrStr
// DESCRIPTION : Returns Error string with respect to error no
// PARAMETER   : int lErrCode,TEXT *lErrText
// RETURN      : void
//-------------------------------------------------------------------------------
void  AculabUtil::DisplayUserInformation (const TcapUserInfoPdu * param)
{
   if (NULL == param)
   {
      printf("%s: UserInformation:<Absent>\n",gProcessName);
      return;
   }

   printf("%s: UserInformation:",gProcessName);
   PrintHex (param->numberOfBytes, param->array,
         SS7_MAX_USER_INFO_PDU_LEN);
}

//-------------------------------------------------------------------------------
// METHOD      : ReturnAculabErrStr
// DESCRIPTION : Returns Error string with respect to error no
// PARAMETER   : int lErrCode,TEXT *lErrText
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::PrintHex (int numberOfBytes, const UINT8 * data,
      int arrayLen)
{
   printf("%s: Number Of Bytes = %d\n", gProcessName, numberOfBytes);

   for (int i = 0; i < numberOfBytes && i < arrayLen;)
   {
      printf("      ");
      for (int j = 0; j < 16 && i < numberOfBytes && i < MAX_TDARRAY_BYTES; j++, i++)
         printf("%02X ", data[i]);
      printf("\n");
   }
}

//-------------------------------------------------------------------------------
// METHOD      : ReturnAculabErrStr
// DESCRIPTION : Returns Error string with respect to error no
// PARAMETER   : int lErrCode,TEXT *lErrText
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::DisplayApplicationContext (const TDArray * param)
{
   if (NULL == param)
   {
      printf("%s: ApplicationContext:<Absent>\n",gProcessName);
      return;
   }

   printf("%s: ApplicationContext: ",gProcessName);
   PrintHex (param->numberOfBytes, param->array);
}


//-------------------------------------------------------------------------------
// METHOD      : LoadSignals 
// DESCRIPTION : Registers signals to be handled
// PARAMETER   : none
// RETURN      : void
//-------------------------------------------------------------------------------
void AculabUtil::LoadSignals()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGABRT, SIG_IGN);
   //signal (SIGSEGV, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, HandleSignal);
   signal (SIGTERM,HandleSignal);
   signal (SIGQUIT,HandleSignal);
   signal (SIGTSTP,HandleSignal);
   signal (SIGPIPE,HandleSignal);
   signal (SIGTRACE, HandleSignal);
   signal (SIGCFG, HandleSignal);
   //signal (SIGSEGV,HandleSignal);
   signal (SIGABRT,HandleSignal);
   signal(30, HandleSignal);
}

//-------------------------------------------------------------------------------
// METHOD      : GetSignalNo 
// DESCRIPTION : Returns sinal no
// PARAMETER   : none
// RETURN      : INT32
//-------------------------------------------------------------------------------
INT32 AculabUtil::GetSignalNo ()
{
   return mSignal;
}

//-------------------------------------------------------------------------------
// METHOD      : HandleSignal 
// DESCRIPTION : Signal Handler Function
// PARAMETER   : int
// RETURN      : void
//-------------------------------------------------------------------------------

void AculabUtil::HandleSignal (int sig)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //INT16 lLogSeverity = MAJOR;

   if (false == mKeepRunning)
      _exit (sig);

   mSignal = sig;
   switch (sig)
   {
      case SIGABRT:
         {  
            //sleep(2);
            mAbortFlag = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGABRT  <%d>  - ignoring", mSignal);
            break;
         }
      case SIGSEGV:
         {
            //sleep(2);
            mAbortFlag = true;
            mKeepRunning = false;
            //mKeepRunning = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGSEGV  <%d>  - ignoring",mSignal);
            break;
         }
      case SIGTRACE:
         {
            mSigTrc = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTRC <%d> - Changing Trace", mSignal);
            break;
         }

      case SIGCFG:
         {
            mSigCfg = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGCFG <%d> - Reloading Configuration",
                  mSignal);
            break;
         }

      case SIGTERM:
         {
            mKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTERM <%d> - Exiting...", mSignal);
            break;
         }

      case SIGINT:
         {
            mKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGINT <%d> - Exiting...", mSignal);
            break;
         }

      case SIGQUIT:
         {
            mKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGQUIT <%d> - Exiting...", mSignal);
            break;
         }

      case SIGPIPE:
         {
            mKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGPIPE <%d> - Exiting...", mSignal);
            break;
         }

         /*case SIGSEGV:
           {
           mKeepRunning = false;
           snprintf (lLogText, MAX_LOG_TEXT_LEN,
           "Received signal SIGSEGV <%d> - Exiting...", mSignal);
           break;
           }*/

      case SIGTSTP:
         {
            mKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTSTP <%d> - Exiting...", mSignal);
            break;
         }

      case 30:
         {
            printf("Received signal to thread :%lu\n", pthread_self());
            pthread_exit(NULL);
         }
         break;

      default:
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIG_UNKNOWN <%d> - Signal Ignored...",
                  mSignal);
         }
   }
   printf ("%s:\33[31m %s\33[0m\n", gProcessName, lLogText);
   gLog.GenerateLog (GSYS13, lLogText, 99);
   signal (sig, HandleSignal);

   return;
}

//-------------------------------------------------------------------------------
// METHOD      : ConvertTcapDlgTypeToStr 
// DESCRIPTION : Converts enum dlgtype to string
// PARAMETER   : int 
// RETURN      : TEXT *
//-------------------------------------------------------------------------------

TEXT * AculabUtil::ConvertTcapDlgTypeToStr (EnumTcapDlg & dlgType)
{
   switch (dlgType)
   {
      case TCAP_BEGIN:
         return "BEGIN";

      case TCAP_PRE_ARRANGED_END:
         return "PRE_ARRANGED_END";

      case TCAP_END:
         return "END";

      case TCAP_CONTINUE:
         return "CONTINUE";

      case TCAP_ABORT:
         return "ABORT";

      case TCAP_RSP_TIMEOUT:
         return "TIMEOUT";

      case TCAP_CONTROL:
         return "CONTROL";

      case TCAP_UNIDIRECTIONAL:
         return "UNIDIRECTIONAL";

      case TCAP_BEGIN_WITH_ADDRESS:
         return "BEGIN_WITH_ADDRESS";

      case TCAP_BEGIN_CONTINUE:
         return "BEGIN_CONTINUE";

      default:
         return "UNKNOWN";
   }
}

void AculabUtil::GetDestAddress(TCAPAddress &lTcapAddr, TEXT *lStr)
{
   TEXT lGt[SS7_MAX_ADDRESS_LEN + 1] = "";
   for (int i = 0; i <  lTcapAddr.numberOfDigits; i++)
   {
      lGt[i] = lTcapAddr.digits[i] + '0';
   }

   sprintf(lStr,"PC:%d AI:%d GT:%s",lTcapAddr.pointCode,lTcapAddr.addressIndicator,lGt);

}

//----------------------------------------------------------------------
// METHOD      : KeepRunning
// DESCRIPTION : Returns status whether process should continue or not
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::KeepRunning ()
{
   if (CheckTrace())
   {
      gTrace.SetTrace();
      ResetTraceFlag();
   }

   return mKeepRunning;
}
//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION : Resets the keep running flag to false
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void AculabUtil::ResetKeepRunning ()
{
   mKeepRunning = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ProcessToExit
// DESCRIPTION : Sets Keep Running flag false
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void AculabUtil::ProcessToExit ()
{
   mKeepRunning = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION : Returns status whether process should  reload config
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::ReloadConfig ()
{
   return mSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : PrintConfig 
// DESCRIPTION : Print reload config flag
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::PrintConfig ()
{
   T (gTrace, printf ("%s: Reload Config Flag(mSigCfg):%d\n", 
            gProcessName, mSigCfg););
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void AculabUtil::ResetConfigFlag ()
{
   mSigCfg = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : GetConfigError
// DESCRIPTION :
// PARAMETER   : ConfigReturn  Enum
// RETURN      : Config Error String
//----------------------------------------------------------------------
TEXT * AculabUtil::GetConfigError (const ConfigReturn lCfgRet)
{
   switch (lCfgRet)
   {
      case CFG_FILE_OPEN_ERR:
         return "CFG_FILE_OPEN_ERR";

      case CFG_KEYWORD_NOT_FOUND:
         return "CFG_KEYWORD_NOT_FOUND";

      case CFG_VALUE_NOT_CONFIGURED:
         return "CFG_VALUE_NOT_CONFIGURED";

      case CFG_BUFFER_OVERFLOW:
         return "CFG_BUFFER_OVERFLOW";

      case CFG_RANGE_EXCEEDED:
         return "CFG_RANGE_EXCEEDED";

      case CFG_UNKNOWN_ERR:
      default:
         return "CFG_UNKNOWN_ERR";
   }
}

//----------------------------------------------------------------------
// METHOD      : CheckTrace
// DESCRIPTION : Returns status whether process should check trace
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::CheckTrace ()
{
   return mSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void AculabUtil::ResetTraceFlag ()
{
   mSigTrc = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ReCreateMsgQueue
// DESCRIPTION : Recreate  the Msg Queues on error
// PARAMETER   : MsgQueue& msgQueue
//             : key_t     key
// RETURN      : success/failure
//----------------------------------------------------------------------
BOOLEAN AculabUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
{
   if (Q_SUCCESS != queue.Create (key, 0666 | IPC_CREAT))
   {
      TEXT
         lLogText[MAX_LOG_TEXT_LEN + 1] = "";

      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Could not recreate queue Key:%d. ", key);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS06, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleQueueCreateError
// DESCRIPTION : Handle Queue Create Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_CREATE_PERMISSION_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_CREATE_PERMISSION_ERR occured while creating queue %d",
               key);
         break;

      case Q_FLAG_ERROR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_FLAG_ERROR occured while creating queue %d", key);
         break;

      case Q_REMOVAL_MARKED:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_REMOVAL_MARKED occured while creating queue %d", key);
         break;

      case Q_NO_CREAT_FLAG:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_NO_CREAT_FLAG occured while creating queue %d", key);
         break;

      case Q_NOT_ENOUGH_MEM:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_NOT_ENOUGH_MEM occured while creating queue %d", key);
         break;

      case Q_LIMIT_EXCEED:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_LIMIT_EXCEED occured while creating queue %d", key);
         break;

      case Q_MSG_TYPE_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_MSG_TYPE_ERR occured while creating queue %d", key);
         break;

      default:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Error:%d occured while creating queue %d", err, key);
   }

   TERR (gTrace, printf ("%s:\33[31m %s\33[0m\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS07, lLogText);
   return false;
}
//----------------------------------------------------------------------
// METHOD      : HandleQueueReadError
// DESCRIPTION : Handle Queue Read Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_MSG_LEN_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_MSG_LEN_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_PERM_ERR:
         mKeepRunning = false;
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_READ_PERM_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_SIGNAL_ERR:
         return false;

      case Q_READ_BUF_ADDR_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_READ_BUF_ADDR_ERR occured while reading from queue:%d",
               key);
         break;

      case Q_REMOVED_ERR:
      case Q_READ_INVALID_ID:
         {
            if (false == ReCreateMsgQueue (queue, key))
               mKeepRunning = false;

            return false;
         }

      case Q_NO_MSG_WITH_TYPE:
         return false;

      case Q_READ_UNKNOWN_ERR:
      default:
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error:%d (%s) occured while reading from queue:%d",
                  err, ConvertQueErrToStr (err), key);
            break;
         }
   }

   TERR (gTrace, printf ("%s:\33[31m %s\33[0m\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS07, lLogText);

   return false;
}

//----------------------------------------------------------------------
// METHOD      : HandleQueueWriteError
// DESCRIPTION : Handle Queue Write Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabUtil::HandleQueueWriteError (msgQueueError err,
      MsgQueue & queue,
      void *ptr,
      INT16 & size, int mtype, modeOption mode)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_WRITE_INVALID_ID:
      case Q_WRITE_PERM_ERR:
      case Q_NOT_EXIST:
         {
            if (ReCreateMsgQueue (queue, key))
            {
               while (mKeepRunning)
               {
                  err = queue.WriteMsg (ptr, size, mtype, mode);
                  if (Q_SUCCESS == err)
                     return true;
                  else
                     HandleQueueWriteError (err, queue, ptr, size, mode);
               }
            }

            mKeepRunning = false;
            return false;
         }

      case Q_WRITE_BUF_ADDR_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_WRITE_BUF_ADDR_ERR occured while writing to queue:%d",
               key);
         break;

      case Q_WRITE_SIGNAL_ERR:
         {
            while (mKeepRunning)
            {
               err = queue.WriteMsg (ptr, size, mtype, mode);
               if (Q_SUCCESS == err)
                  return true;
               else
                  HandleQueueWriteError (err, queue, ptr, size, mode);
            }

            return false;
         }

      case Q_BYTES_LIMIT_ERR:
         err = queue.WriteMsg (ptr, size, mtype, mode);
         if (Q_SUCCESS == err)
            return true;
         else
            return false;

      case Q_WRITE_NO_MEM:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_WRITE_NO_MEM occured while writing to queue:%d", key);
         break;

      case Q_WRITE_UNKNOWN_ERR:
      default:
         if (BLOCKING == mode)
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error:%d (%s) occured while writing to queue:%d",
                  err, ConvertQueErrToStr (err), key);
         break;
   }

   TERR (gTrace, printf ("%s:\33[31m %s\33[0m\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS08, lLogText);
   return false;
}

//----------------------------------------------------------------------
// METHOD      : ConvertQueErrToStr
// DESCRIPTION : Converts Queue Error to str
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * AculabUtil::ConvertQueErrToStr (const msgQueueError queErr)
{
   switch (queErr)
   {
      case Q_SUCCESS:
         return "Q_SUCCESS";

      case Q_INVALID_COMMAND:
         return "Q_INVALID_COMMAND";

      case Q_MSG_TYPE_ERR:
         return "Q_MSG_TYPE_ERR";

      case Q_NOT_EXIST:
         return "Q_NOT_EXIST";

      case Q_CREATE_PERMISSION_ERR:
         return "Q_CREATE_PERMISSION_ERR";

      case Q_FLAG_ERROR:
         return "Q_FLAG_ERROR";

      case Q_REMOVAL_MARKED:
         return "Q_REMOVAL_MARKED";

      case Q_NO_CREAT_FLAG:
         return "Q_NO_CREAT_FLAG";

      case Q_NOT_ENOUGH_MEM:
         return "Q_NOT_ENOUGH_MEM";

      case Q_LIMIT_EXCEED:
         return "Q_LIMIT_EXCEED";

      case Q_DELETE_PERMISSION_ERR:
         return "Q_DELETE_PERMISSION_ERR";

      case Q_DELETE_INVALID_SECOND_ARG:
         return "Q_DELETE_INVALID_SECOND_ARG";
      case Q_MSG_LEN_ERR:
         return "Q_MSG_LEN_ERR";

      case Q_READ_PERM_ERR:
         return "Q_READ_PERM_ERR";

      case Q_READ_BUF_ADDR_ERR:
         return "Q_READ_BUF_ADDR_ERR";

      case Q_REMOVED_ERR:
         return "Q_REMOVED_ERR";

      case Q_READ_SIGNAL_ERR:
         return "Q_READ_SIGNAL_ERR";

      case Q_READ_INVALID_ID:
         return "Q_READ_INVALID_ID";

      case Q_READ_UNKNOWN_ERR:
         return "Q_READ_UNKNOWN_ERR";

      case Q_NO_MSG_WITH_TYPE:
         return "Q_NO_MSG_WITH_TYPE";

      case Q_BYTES_LIMIT_ERR:
         return "Q_BYTES_LIMIT_ERR";

      case Q_WRITE_PERM_ERR:
         return "Q_WRITE_PERM_ERR";

      case Q_WRITE_BUF_ADDR_ERR:
         return "Q_WRITE_BUF_ADDR_ERR";

      case Q_WRITE_SIGNAL_ERR:
         return "Q_WRITE_SIGNAL_ERR";

      case Q_WRITE_INVALID_ID:
         return "Q_WRITE_INVALID_ID";

      case Q_WRITE_NO_MEM:
         return "Q_WRITE_NO_MEM";

      case Q_WRITE_UNKNOWN_ERR:
         return "Q_WRITE_UNKNOWN_ERR";

      case Q_STATUS_PERMISSION_ERR:
         return "Q_STATUS_PERMISSION_ERR";

      case Q_STATUS_INVALID_ADDRESS_ERR:
         return "Q_STATUS_INVALID_ADDRESS_ERR";

      case Q_UNKNOWN_ERR:
      default:
         return "Q_UNKNOWN_ERR";
   }
}

