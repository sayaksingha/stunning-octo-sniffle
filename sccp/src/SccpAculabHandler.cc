static const char *id = "@(#) $Id: SccpAculabHandler.cc,v 1.1.4.1.4.1 2021/12/01 12:43:25 jamesjac Exp $";
//----------------------------------------------------------------------------
// NAME :SccpAculabHandler.cc
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
// Originator : Keerthini AB                                 Date:25 AUG 2014
//----------------------------------------------------------------------------
#include "SccpAculabHandler.h"
extern Log    gLog;
extern CTrace gTrace;
extern TEXT   gProcessName[ACU_SCCP_MAX_PROCESS_NAME];
extern TEXT gCfgFile[MAX_LOG_TEXT_LEN + 1];
//-------------------------------------------------------------------------------
// METHOD      : SccpAculabHandler
// DESCRIPTION : Default Constructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
SccpAculabHandler::SccpAculabHandler()
{
   mDestinationA = 0;
   mDestinationB = 0;
   mPcFlag       = false;
}
//-------------------------------------------------------------------------------
// METHOD      : SccpAculabHandler
// DESCRIPTION : Parameterised Constructor
// PARAMETER   : TEXT *, TEXT *
// RETURN      : None
//-------------------------------------------------------------------------------
SccpAculabHandler::SccpAculabHandler(TEXT *lProcName,
      TEXT *lTraceEnv)
{
}
//-------------------------------------------------------------------------------
// METHOD      : ~SccpAculabHandler
// DESCRIPTION : Destructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
SccpAculabHandler::~SccpAculabHandler()
{
}
//-------------------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes all library and configuration
// PARAMETER   : TEXT *, int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::Init(TEXT *lCfgFile, int lMsgType)
{
   mMsgType = lMsgType;
   if(false == ReadConfig())
   {
      return false;
   }
   if(false ==  mAculabApi.Init(lCfgFile,mPegFlag))
   {
      return false;
   }
   if(false == CreateMsgQ())
   {
      return false;
   }
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : UpdateSsapStatus
// DESCRIPTION : Updates Ssap Status
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::UpdateSsapStatus()
{
   return mAculabApi.UpdateSsapStatus();
}
//-------------------------------------------------------------------------------
// METHOD      : GetSsapStatus
// DESCRIPTION : Retrieves Ssap Status
// PARAMETER   : none
// RETURN      : UINT16
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::GetSsapStatus()
{
   return mAculabApi.GetSsapStatus();
}
//-------------------------------------------------------------------------------
// METHOD      : ReconnectSsap
// DESCRIPTION : Reconnects Ssap
// PARAMETER   : none
// RETURN      : UINT16
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::ReconnectSsap()
{
   return mAculabApi.SsapReConnect();
}
//-------------------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : Calls diffrent apis for getting configuration
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::ReadConfig()
{
   if(false == ReadIpcConfig())
   {
      return false;
   }
   if(false == ReadKernelConfig())
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
BOOLEAN SccpAculabHandler::ReadIpcConfig()
{
   CfgRead        lCfgRead("ACULAB_SCCP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if (false == lCfgRead.CfgInit(SCCP_ANSI_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed...\n",
            gProcessName,SCCP_ANSI_CFG);
      gLog.GenerateLog(GSYS09, "ss7.cfg CfgRead object initializaton failed");
      return false;
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_SCCP_HDLR_Q_RCV",mMsgQ.RdMsgQKey ,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_SCCP_HDLR_Q_RCV in %s",
            SCCP_ANSI_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_SCCP_HDLR_Q_RCV = %d -> %x\n", gProcessName,
               mMsgQ.RdMsgQKey,mMsgQ.RdMsgQKey););
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_SCCP_DEC_Q_RCV",mMsgQ.WrMsgQKey,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_SCCP_DEC_Q_RCV in %s",
            SCCP_ANSI_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_SCCP_DEC_Q_RCV = %d -> %x\n",gProcessName,
               mMsgQ.WrMsgQKey,mMsgQ.WrMsgQKey););
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_SCCP_HEART_BEAT_Q_RCV",mMsgQ.RdHbQKey,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_SCCP_HEART_BEAT_Q_RCV in %s",
            SCCP_ANSI_CFG);
      printf(":%s:%s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: MSG_SCCP_HEART_BEAT_Q_RCV = %d -> %x\n",gProcessName,
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
BOOLEAN SccpAculabHandler::ReadKernelConfig()
{
   CfgRead        lCfgRead("ACULAB_SCCP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if (false == lCfgRead.CfgInit(SCCP_ANSI_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SCCP_ANSI_CFG);
      gLog.GenerateLog(GSYS09,
            "kerlnel.cfg CfgRead object initializaton failed");
      return false;
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("SCCP_PEG_REQUIRED",
            mPegFlag ,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_PEG_REQUIRED in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_PEG_REQUIRED = %d \n", gProcessName,
               mPegFlag););
   }
   if(CFG_OK != lCfgRead.GetConfigNum("SCCP_MSG_DIPLAY_PARAM",
            mDisplayParam,
            MIN_DIS_PARAM_VAL,
            MAX_DIS_PARAM_VAL))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_MSG_DIPLAY_PARAM in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_MSG_DIPLAY_PARAM = %d\n", gProcessName, mDisplayParam););
   }
   if(CFG_OK != lCfgRead.GetConfigNum("SCCP_DESTINATION_1",
            mDestinationA,
            1,
            16777215))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_DESTINATION_1 in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_DESTINATION_1 = %d\n", gProcessName, mDestinationA););
   }
   if(CFG_OK != lCfgRead.GetConfigNum("SCCP_DESTINATION_2",
            mDestinationB,
            1,
            16777215))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_DESTINATION_2 in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      mDestinationB = 0;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_DESTINATION_2 = %d\n", gProcessName, mDestinationB););
   }
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::ReloadConfig()
{
   CfgRead        lCfgRead("ACULAB_SCCP_API");
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   if (false == lCfgRead.CfgInit(SCCP_ANSI_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SCCP_ANSI_CFG);
      gLog.GenerateLog(GSYS09,
            "kerlnel.cfg CfgRead object initializaton failed");
      return false;
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("SCCP_PEG_REQUIRED",
            mPegFlag ,
            SS7_MIN_BOOLEAN,
            SS7_MAX_BOOLEAN))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_PEG_REQUIRED in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_PEG_REQUIRED = %d \n", gProcessName,
               mPegFlag););
   }
   if(CFG_OK != lCfgRead.GetConfigNum("SCCP_MSG_DIPLAY_PARAM",
            mDisplayParam,
            MIN_DIS_PARAM_VAL,
            MAX_DIS_PARAM_VAL))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_MSG_DIPLAY_PARAM in %s",
            SCCP_ANSI_CFG);
      printf("%s: %s\n", lLogText,gProcessName);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace,printf("%s: SCCP_MSG_DIPLAY_PARAM = %d\n", gProcessName, mDisplayParam););
   }
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : CreateMsgQ
// DESCRIPTION : Creates Message queues
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::CreateMsgQ()
{
   msgQueueError lQueErr;
   lQueErr = mMsgQ.RdQ.Create (mMsgQ.RdMsgQKey, IPC_CREAT | SS7_IPC_PERM);
   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,mMsgQ.RdQ);
      printf("%s: Error creating MSG_SCCP_HANDLER_Q_RCV queue key:%d\n",
            gProcessName,mMsgQ.RdMsgQKey);
      return false;
   }
   lQueErr = mMsgQ.WrQ.Create (mMsgQ.WrMsgQKey, IPC_CREAT | SS7_IPC_PERM);
   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,  mMsgQ.WrQ);
      printf("%s: Error creating MSG_SCCP_DECODER_Q_RCV queue key:%d\n",
            gProcessName, mMsgQ.WrMsgQKey);
      return false;
   }
   lQueErr = mMsgQ.HbQ.Create (mMsgQ.RdHbQKey, IPC_CREAT | SS7_IPC_PERM);
   if(Q_SUCCESS != lQueErr)
   {
      AculabUtil::HandleQueueCreateError (lQueErr,  mMsgQ.HbQ);
      printf("%s: Error creating MSG_SCCP_HEART_BEAT_Q_RCV queue key:%d\n",
            gProcessName, mMsgQ.RdHbQKey);
      return false;
   }
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : TxMsgToApplication
// DESCRIPTION : Sends Message to application queue
// PARAMETER   : SccpMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::TxMsgToApplication(_SccpInfo &lTxSccpMsg)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   msgQueueError  lQueErr;
   // Ab Comment: _SccpInfo automatically expands to include KafkaRoutingInfo when compiled
   // with KAFKA_BRIDGE. Both KafkaBridge and sccp_ansi must be compiled with the same flag
   // to ensure this size calculation matches across IPC queue bounds.
   INT16          lMsgLen = sizeof (_SccpInfo);
   lTxSccpMsg.msgType = SCCP_MSG_UDT;
   PrintApplSccpStruct(lTxSccpMsg, "TX");
   lQueErr = mMsgQ.WrQ.WriteMsg ((void *) &lTxSccpMsg, lMsgLen,
         mMsgType, NON_BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      if (false ==
            AculabUtil::HandleQueueWriteError (lQueErr, mMsgQ.WrQ, &lTxSccpMsg,
               lMsgLen, lTxSccpMsg.msgType, NON_BLOCKING))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "MsgType:%d MsgLen:%d QueueKey:%d err:%d Sccp Message write to the Decoder Queue Failed...",
               mMsgType, lMsgLen, mMsgQ.WrMsgQKey, lQueErr);
         TERR (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP14,1,lLogText);
         return false;
      }
   }
   if(mPegFlag)
      gPeg.PegEvent(PEG_UDT_SENT_TO_APPL);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "TransactionId:%08x IndefiniteLenFlag:%d MsgType:%d MsgLen:%d QueueKey:%d Sccp Message wrote to the Decoder Queue",
         lTxSccpMsg.udt.transInfo.origTransId, mIndLenFlag, mMsgType, lMsgLen, mMsgQ.WrMsgQKey);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACUSCCP10,lLogText);
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : TxMsgToStack
// DESCRIPTION : Transmitts message to stack
// PARAMETER   : acu_sccp_msg_t * (aculab message structure pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::TxMsgToStack(UINT8 data[SCCP_ACU_MAX_UDT_LENGTH], UINT16 dataLen, UINT32 transId)
{
   if(true == mAculabApi.SendAcuSccpMsg(data, dataLen, transId))
   {
      if(mPegFlag)
         gPeg.PegEvent(PEG_UDT_SENT_TO_STACK);
      return true;
   }
   else
      return false;
}
//-------------------------------------------------------------------------------
// METHOD      : ProcessTxMsgToStack
// DESCRIPTION : Processes Appliaction Sccp Message before transmitting
//               to stack
// PARAMETER   : SccpMsg (Application sccp message),
//               acu_sccp_msg_t *(aculab message structure pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::ProcessTxMsgToStack(_SccpInfo &lSccpMsg)
{
   UINT8             lData[SCCP_ACU_MAX_UDT_LENGTH];
   UINT16            lDataLen = 0;
   acu_sccp_addr_t   lLocAddr;
   acu_sccp_addr_t   lRemAddr;
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   memset(&lData, '\0', SCCP_ACU_MAX_UDT_LENGTH);
   switch (lSccpMsg.msgType)
   {
      case SCCP_MSG_UDT:
         {
            //int lTemcount = 0;
            if(lSccpMsg.udt.clgPartyAddress.digits[0] > 0x30)
            {
               for(int i = 0; i < lSccpMsg.udt.clgPartyAddress.numberOfDigits; i++)
               {
                  {
                     lSccpMsg.udt.clgPartyAddress.digits[i] = lSccpMsg.udt.clgPartyAddress.digits[i] - 0x30;
                  }
               }
            }
            if(lSccpMsg.udt.cldPartyAddress.digits[0] > 0x30)
            {
               for(int i = 0; i < lSccpMsg.udt.cldPartyAddress.numberOfDigits; i++)
               {
                  {
                     lSccpMsg.udt.cldPartyAddress.digits[i] = lSccpMsg.udt.cldPartyAddress.digits[i] - 0x30;
                  }
               }
            }
            PrintApplSccpStruct(lSccpMsg, "RX");
            if(mPegFlag)
                  gPeg.PegEvent(PEG_UDT_RCVD_FROM_APPL);
            memset (&lLocAddr, '\0', sizeof(lLocAddr));
            memset (&lRemAddr, '\0', sizeof(lRemAddr));
            // Ab Comment: Debug - print raw AI values immediately before EncodeAddress to detect any struct corruption
            /*printf("%s: [DEBUG] ClgParty AI=0x%02X PC=%d SSN=%d numDig=%d\n",
                   gProcessName,
                   (unsigned)lSccpMsg.udt.clgPartyAddress.addressIndicator,
                   (int)lSccpMsg.udt.clgPartyAddress.pointCode,
                   (int)lSccpMsg.udt.clgPartyAddress.subsystemNumber,
                   (int)lSccpMsg.udt.clgPartyAddress.numberOfDigits);
            printf("%s: [DEBUG] CldParty AI=0x%02X PC=%d SSN=%d numDig=%d\n",
                   gProcessName,
                   (unsigned)lSccpMsg.udt.cldPartyAddress.addressIndicator,
                   (int)lSccpMsg.udt.cldPartyAddress.pointCode,
                   (int)lSccpMsg.udt.cldPartyAddress.subsystemNumber,
                   (int)lSccpMsg.udt.cldPartyAddress.numberOfDigits);
	    */
            if(false == EncodeAddress(lLocAddr, lSccpMsg.udt.clgPartyAddress))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Encode Remote Address Failed...");
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP17,lLogText);
               return false;
            }
            if(true == EncodeAddress(lRemAddr, lSccpMsg.udt.cldPartyAddress))
            {
               if(!mDestinationB)
               {
                  if(mDestAStatus)
                  {
                     lRemAddr.sa_valid = lRemAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
                     lRemAddr.sa_pc = mDestinationA;
                  }
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN, "Destination 1 is not UP status 1:%d", mDestAStatus);
                     TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(ACUSCCP24, lLogText);
                     return false;
                  }
               }
               else
               {
                  if(!mPcFlag)
                  {
                     mPcFlag = true;
                     if(mDestAStatus)
                     {
                        lRemAddr.sa_valid = lRemAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
                        lRemAddr.sa_pc = mDestinationA;
                     }
                     else
                     {
                        if(mDestBStatus)
                        {
                           lRemAddr.sa_valid = lRemAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
                           lRemAddr.sa_pc =  mDestinationB;
                        }
                        else
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "Destination 1 Selection Failed Status 1:%d Status 2:%d", mDestAStatus, mDestBStatus);
                           TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACUSCCP24, lLogText);
                           return false;
                        }
                     }
                  }
                  else
                  {
                     mPcFlag = false;
                     if(mDestBStatus)
                     {
                        lRemAddr.sa_valid = lRemAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
                        lRemAddr.sa_pc = mDestinationB;
                     }
                     else
                     {
                        if(mDestAStatus)
                        {
                           lRemAddr.sa_valid = lRemAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
                           lRemAddr.sa_pc =  mDestinationA;
                        }
                        else
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "Destination 2 Selection Failed Status 1:%d Status 2:%d", mDestAStatus, mDestBStatus);
                           TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACUSCCP24, lLogText);
                           return false;
                        }
                     }
                  }
               }
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Encode Local Address Failed...");
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP18,lLogText);
               return false;
            }
            if(false == mAculabApi.SetRemoteLocalAddress(lLocAddr, lRemAddr))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Set Local & Remote Address Failed...");
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP20,lLogText);
               return false;
            }
            // Ab Comment: Map pcMsgHdlg Return Option bit (0x80) to Aculab connection QoS
            // This MUST be called after SetRemoteLocalAddress so that mConnection is initialized
            if (lSccpMsg.udt.pcMsgHdlg & 0x80)
            {
               mAculabApi.SetReturnOption(true);
            }
            else
            {
               mAculabApi.SetReturnOption(false);
            }
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Set Local & Remote Address Success RemPc:%d",lRemAddr.sa_pc);
            T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACUSCCP21,2,lLogText);
            if(false == EncodeSccpUnitData(lSccpMsg, lData, lDataLen))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Encode Sccp UDT Failed...");
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP22,lLogText);
               return false;
            }
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Encode Sccp UDT Sucess");
            T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACUSCCP23,2,lLogText);
            if(false == TxMsgToStack(lData, lDataLen, lSccpMsg.udt.transInfo.origTransId))
               return false;
            return true;
            break;
         }
      default:
         TERR (gTrace, printf ("%s: Not a SCCP UDT. Discarding...\n",
                  gProcessName););
         return false;
   }
}
//-------------------------------------------------------------------------------
// METHOD      : RxMsgFromStack
// DESCRIPTION : Recieves aculab message struture from stack
// PARAMETER   : acu_sccp_msg_t **(aculab message structure double pointer)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::RxMsgFromStack(acu_sccp_msg_t **lMsgPtr)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lHeartBeatText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof(lHeartBeatText);
   // Read Messages from Heart-Beat Queue
   (void)mMsgQ.HbQ.ReadMsg((void *)&lHeartBeatText, lMsgLen, mMsgType, NON_BLOCKING);
   if(false ==  mAculabApi.GetAcuSccpEvent(lMsgPtr))
   {
      return false;
   }
   else
   {
      acu_sccp_msg_type_t lMsgType;
      lMsgType = mAculabApi.HandleEventAcuSccpMsg(*lMsgPtr);
      switch(lMsgType)
      {
         case ACU_SCCP_MSG_CON_STATE:
            {
               mAculabApi.GetAcuSccpConState(*lMsgPtr);
               acu_sccp_msg_free(*lMsgPtr);
               return false;
            }
         //case ACU_SCCP_MSG_DATA_IND:
           // {
             //  return true;
               //break;
            //}
         case ACU_SCCP_MSG_UNITDATA:
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "SCCP Unit Data received from stack");
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP34, lLogText);
               BOOLEAN lRes;
               //BOOLEAN lFlag = false;
               if(mPegFlag)
                  gPeg.PegEvent(PEG_UDT_RCVD_FROM_STACK);
               lRes = DecodeUnitData(lMsgPtr);
               if(false == lRes)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Decode Sccp Unit data Failed...");
                  TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACUSCCP30, lLogText);
                  mAculabApi.UnblockConnection((*lMsgPtr)->tm_con);
                  acu_sccp_msg_free(*lMsgPtr);
                  return false;
               }
               // Standard behavior: send the decoded SccpInfo to the IPC queue
               TxMsgToApplication(mSccpInfo);
               
               mAculabApi.UnblockConnection((*lMsgPtr)->tm_con);
               acu_sccp_msg_free(*lMsgPtr);
               break;
            }
         case ACU_SCCP_MSG_NOTICE:
            {
               // ---------------------------------------------------------------
	       // Ab Comment
               // SCCP NOTICE (UDTS/XUDTS) — network bounce-back.
               // This fires when acu_sccp_unitdata_request() succeeded locally
               // but the network rejected the packet (GTT failure, dead PC,
               // congestion, etc.). Without this handler, every dropped packet
               // is silently swallowed in the default: case and completely
               // invisible in logs or pegs.
               //
               // tm_cause holds the SCCP Return Cause value (ITU-T Q.711 §3.12 /
               // ANSI T1.112.3). Common values:
               //   0x00 = No translation for address of such nature
               //   0x01 = No translation for specific address
               //   0x02 = Subsystem congestion
               //   0x03 = Subsystem failure
               //   0x04 = Unequipped user
               //   0x05 = Network failure
               //   0x0D = Transport unavailable
               //   0x0F = Unqualified
               // tm_data/tm_data_length hold the original TCAP PDU if needed
               // for diagnostic logging.
               // ---------------------------------------------------------------
               UINT8 lNoticeCause = (*lMsgPtr)->tm_cause;
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "SCCP NOTICE (UDTS): ReturnCause=0x%02X (%d) DataLen:%d "
                     "[Packet dropped by network — GTT/route failure]",
                     lNoticeCause, lNoticeCause, (*lMsgPtr)->tm_data_length);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP36, lLogText);
               if(mPegFlag)
                  gPeg.PegEvent(PEG_NOTICE_RCVD);
               
               // Ab Comment: MUST unblock the connection/SSAP after receiving a NOTICE.
               // Failing to do this causes a one-shot defect where the Aculab driver's 
               // flow control permanently suspends delivering future NOTICE messages 
               // on this context to protect the ring buffer.
               mAculabApi.UnblockConnection((*lMsgPtr)->tm_con);
               acu_sccp_msg_free(*lMsgPtr);
               break;
            }
         case ACU_SCCP_MSG_SP_STATUS:
         case ACU_SCCP_MSG_USER_STATUS:
            {
               // ---------------------------------------------------------------
	       // Ab Comment:
               // SP_STATUS  : Entire remote Signalling Point went prohibited/
               //              accessible. Updates route availability for that PC.
               // USER_STATUS: Individual remote subsystem (SSN) went OOS/IS
               //              while the SP itself stayed up. Without this case,
               //              SSN-level outages are invisible and sccp_ansi
               //              would keep routing to a dead subsystem.
               // Both cases refresh mDestAStatus and mDestBStatus via
               // acu_sccp_get_sccp_status(), which gates outbound traffic in
               // ProcessTxMsgToStack(). No separate per-SSN state needed here
               // because GetSccpStatus() checks ACU_SCCP_REM_SCCP_AVAIL which
               // reflects the combined SP+SSN reachability.
               // ---------------------------------------------------------------
               mAculabApi.GetSccpStatus(mDestinationA, mDestAStatus);
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Got SCCP Status Dest-1-Status:%d", mDestAStatus);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP25, lLogText);
               if(mDestinationB)
               {
                  mAculabApi.GetSccpStatus(mDestinationB, mDestBStatus);
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Got SCCP Status Dest-2-Status:%d", mDestBStatus);
                  T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACUSCCP25, lLogText);
               }
               acu_sccp_msg_free(*lMsgPtr);
               break;
            }
         default:
            {
               if(NULL != (*lMsgPtr)->tm_con)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Received Default Message Connection pointer active");
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACUSCCP36, lLogText);
                  mAculabApi.UnblockConnection((*lMsgPtr)->tm_con);
               }
               acu_sccp_msg_free(*lMsgPtr);
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Freed Default Message");
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP37, lLogText);
               return true;
            }
      }
      return true;
   }
   return false;
}
//-------------------------------------------------------------------------------
// METHOD      : RxMsgFromApplication
// DESCRIPTION : Recieves application sccp message from application
//               message queue
// PARAMETER   : SccpMsg (application sccp message structure)
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN SccpAculabHandler::RxMsgFromApplication(_SccpInfo &lRxSccpMsg)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   // Ab Comment: _SccpInfo automatically expands to include KafkaRoutingInfo when compiled
   // with KAFKA_BRIDGE. Both KafkaBridge and sccp_ansi must be compiled with the same flag
   // to ensure this size calculation matches across IPC queue bounds.
   INT16          lMsgLen = sizeof (_SccpInfo);
   msgQueueError  lQueErr;
   memset (&lRxSccpMsg, 0, lMsgLen);
   // [BUGFIX]: Pass 0 instead of mMsgType to msgrcv. This allows sccp_ansi 
   // to read messages from tcap_ansi, which tags its IPC messages with its 
   // own SSN (200), rather than sccp_ansi's SSN (149).
   lQueErr = mMsgQ.RdQ.ReadMsg((void *)&lRxSccpMsg, lMsgLen, 0, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return AculabUtil::HandleQueueReadError (lQueErr, mMsgQ.RdQ);
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "QueueKey:%d Sccp Message Received from Application",
         mMsgQ.RdMsgQKey);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACUSCCP09,lLogText);
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : PrintApplSccpStruct
// DESCRIPTION : Prints Application sccp structure
// PARAMETER   : SccpMsg (application sccp message structure)
// RETURN      : void
//-------------------------------------------------------------------------------
void SccpAculabHandler::PrintApplSccpStruct(_SccpInfo SccpMsg, TEXT *lText)
{
   if(0 != mDisplayParam )
   {
      T (gTrace, {
         printf("-----------------%s APPLICATION --------------------\n",lText);
         printf("\tSccpMsgType    :%d\n", SccpMsg.msgType);
         printf("==================UDT===================\n");
         printf("\tpcMsgHdlg      :%d \n", SccpMsg.udt.pcMsgHdlg);
      });
      DisplayAddress(&SccpMsg.udt.clgPartyAddress, "Orig");
      DisplayAddress(&SccpMsg.udt.cldPartyAddress, "Dest");
      T(gTrace, {
         printf("----------Transaction Portion-----------\n");
         printf("\tPkgType        :%d\n", (EnumTcapTransType)SccpMsg.udt.transInfo.pkgType);
         printf("\tOrigTranIdLen  :%d\n",SccpMsg.udt.transInfo.origTransIdLen);
         printf("\tOrigTranId     :%u\n", SccpMsg.udt.transInfo.origTransId);
         printf("\tDestTranIdLen  :%d\n",SccpMsg.udt.transInfo.destTransIdLen);
         printf("\tDestTranId     :%u\n", SccpMsg.udt.transInfo.destTransId);
         printf("----------------DlgInfo-----------------\n");
         printf("\tNumberOfBytes  :%d\n", SccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes);
         printf("\tData           :");
         for (int i = 0; i < SccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes && i < SS7_MAX_ACN_PDU_LEN;)
         {
            for (int j = 0; j < SccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes && i < SccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes && i < MAX_TDARRAY_BYTES; j++, i++)
               printf("%02X ", SccpMsg.udt.dlgInfo.dlgPdu.array[i]);
         }
         printf("\n---------------CmpInfo-------------------\n");
         printf("\tCompType       :%02X\n", SccpMsg.udt.compInfo.tcapComp);
         printf("\tInvokeId       :%d\n", SccpMsg.udt.compInfo.invokeId);
         // Ab Comment : Fixed C-union memory alignment bug causing segfaults. 
         // Do not unconditionally access 'invoke' memory layout for all component types.
         if (SccpMsg.udt.compInfo.tcapComp == TCAP_COMP_INVOKE)
         {
            printf("\tLinkedId       :%d\n", SccpMsg.udt.compInfo.invoke.linkedId.linkedId);
            printf("\tOpCode         :%d\n", SccpMsg.udt.compInfo.invoke.operation.operationCode);
            printf("\tParamLength    :%d\n", SccpMsg.udt.compInfo.invoke.paramData.numberOfBytes);
            printf("\tComponentData  :");
            for (int i = 0; i < SccpMsg.udt.compInfo.invoke.paramData.numberOfBytes && i < 2000; i++)
               printf ("%02X ", SccpMsg.udt.compInfo.invoke.paramData.array[i]);
            printf("\n");
         }
         else if (SccpMsg.udt.compInfo.tcapComp == TCAP_COMP_RET_RESULT || SccpMsg.udt.compInfo.tcapComp == TCAP_COMP_RET_REULT_LAST)
         {
            printf("\tOpCode         :%d\n", SccpMsg.udt.compInfo.retResult.operation.operationCode);
            printf("\tParamLength    :%d\n", SccpMsg.udt.compInfo.retResult.paramData.numberOfBytes);
            printf("\tComponentData  :");
            for (int i = 0; i < SccpMsg.udt.compInfo.retResult.paramData.numberOfBytes && i < 2000; i++)
               printf ("%02X ", SccpMsg.udt.compInfo.retResult.paramData.array[i]);
            printf("\n");
         }
         else if (SccpMsg.udt.compInfo.tcapComp == TCAP_COMP_RET_ERR)
         {
            printf("\tParamLength    :%d\n", SccpMsg.udt.compInfo.retErr.paramData.numberOfBytes);
            printf("\tComponentData  :");
            for (int i = 0; i < SccpMsg.udt.compInfo.retErr.paramData.numberOfBytes && i < 2000; i++)
               printf ("%02X ", SccpMsg.udt.compInfo.retErr.paramData.array[i]);
            printf("\n");
         }
         else if (SccpMsg.udt.compInfo.tcapComp == TCAP_COMP_REJECT)
         {
            printf("\tProblemType    :%d\n", SccpMsg.udt.compInfo.reject.problem.problemCodeType);
            printf("\tProblemCode    :%d\n", SccpMsg.udt.compInfo.reject.problem.problemCode);
         }
         printf("-----------------------------------------------------\n");
      });
   }
}
//----------------------------------------------------------------------
// METHOD NAME : SccpDecodeComponent
// DESCRIPTION : Decodes the Address
// PARAMETERS  : Address, PDU
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN SccpAculabHandler::DecodeComponent (UINT8 * pdu, UINT8 pduLen)
{
   UINT8 lOffset = 0;
   UINT8 lCompLen = 0;
   UINT8 lCompLenOffset = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN+1] = "";
   lCompLen = pdu[lOffset++];
   if (0x81 == lCompLen)
   {
      lCompLen = pdu[lOffset++];
   }
   else if (0x80 == lCompLen)
   {
      //T (gTrace, printf ("%s: Indefinite Length. CompPortion\n", gProcessName););
      mIndLenFlag = true;
      //sprintf(lLogText,"TransactionId:%08x, Indefinite Length. CompPortion", origTransId);
      sprintf(lLogText,"TransactionId:%08x, Indefinite Length. CompPortion", mSccpInfo.udt.transInfo.origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP32, 1, lLogText);
      pduLen -= 2;
   }
   UINT8 compTag = pdu[lOffset++];
   switch (compTag)
   {
      case SS7_ANSI_COMP_INVOKE_LAST:
      case SS7_ANSI_COMP_INVOKE_NOT_LAST:
         mSccpInfo.udt.compInfo.tcapComp = TCAP_COMP_INVOKE;
         break;
      case SS7_ANSI_COMP_RET_RESULT_LAST:
      case SS7_ANSI_COMP_RET_RESULT_NOT_LAST:
         mSccpInfo.udt.compInfo.tcapComp = TCAP_COMP_RET_REULT_LAST;
         break;
      case SS7_ANSI_COMP_RET_ERROR:
         mSccpInfo.udt.compInfo.tcapComp = TCAP_COMP_RET_ERR;
         break;
      case SS7_ANSI_COMP_REJECT:
         mSccpInfo.udt.compInfo.tcapComp = TCAP_COMP_REJECT;
         break;
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Unknown ANSI Tcap Component Tag: 0x%02X. Discarding... MsgType:%d", compTag, mSccpInfo.msgType);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP39, lLogText);
         return false;
   }
   lCompLen = pdu[lOffset++]; // skipping component len
   if (0x81 == lCompLen)
      lCompLen = pdu[lOffset++];
   else if (0x80 == lCompLen)
   {
      mIndLenFlag = true;
      sprintf(lLogText,"TransactionId:%08x, Indefinite Length. CompType", mSccpInfo.udt.transInfo.origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP33, 1, lLogText);
      pduLen -= 2;
      lCompLen = pduLen - lOffset;
   }
   lCompLenOffset = lOffset - 1;
   // Invoke id
   if (SS7_ANSI_INVOKE_ID_TAG == pdu[lOffset])
   {
      lOffset += 2;     // skipping Tag and Length
      mSccpInfo.udt.compInfo.invokeId = pdu[lOffset++];
   }
   // LinkedId (Correlation ID in ANSI)
   if (SS7_ANSI_LINKED_ID_TAG == pdu[lOffset])
   {
      lOffset += 2;     // skipping tag and Length
      mSccpInfo.udt.compInfo.invoke.linkedId.linkedId =
         pdu[lOffset++];
   }
   switch (mSccpInfo.udt.compInfo.tcapComp)
   {
      case TCAP_COMP_INVOKE:
         if (SS7_ANSI_NATIONAL_OP_CODE_TAG == pdu[lOffset]
               || SS7_ANSI_PRIVATE_OP_CODE_TAG == pdu[lOffset])
         {
            mSccpInfo.udt.compInfo.invoke.operation.isPrivate = (SS7_ANSI_PRIVATE_OP_CODE_TAG == pdu[lOffset]);
            int opLen = pdu[lOffset + 1];
            lOffset += 2;     // skipping tag and Length
            
            // Ab Comment: Dynamically decode OpCode based on length (1-4 bytes) for production-grade robustness.
            mSccpInfo.udt.compInfo.invoke.operation.operationCode = 0;
            for (int i = 0; i < opLen && i < 4; i++) {
               mSccpInfo.udt.compInfo.invoke.operation.operationCode = 
                  (mSccpInfo.udt.compInfo.invoke.operation.operationCode << 8) | pdu[lOffset++];
            }
            if (opLen > 4) {
               lOffset += (opLen - 4); // Skip any extra bytes to prevent buffer corruption
            }
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Operation Code Missing. Discarding... MsgType:%d", mSccpInfo.msgType);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACUSCCP44, lLogText);
            return false;
         }
         mSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes =
            lCompLen - lOffset + lCompLenOffset + 1;
         memcpy (mSccpInfo.udt.compInfo.invoke.paramData.array,
               &pdu[lOffset],
               mSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes);
         break;
      case TCAP_COMP_RET_RESULT:
      case TCAP_COMP_RET_REULT_LAST:
         if (SS7_ANSI_PARAM_SEQUENCE_TAG == pdu[lOffset]
               || SS7_ANSI_PARAM_SET_TAG == pdu[lOffset])
         {
            lOffset++; // skip tag
            if (0x81 == pdu[lOffset])
               lOffset += 2; // skip long length indicator
            else
               lOffset++; // skip length
            if (SS7_ANSI_NATIONAL_OP_CODE_TAG == pdu[lOffset]
                  || SS7_ANSI_PRIVATE_OP_CODE_TAG == pdu[lOffset])
            {
               mSccpInfo.udt.compInfo.retResult.operation.isPrivate = (SS7_ANSI_PRIVATE_OP_CODE_TAG == pdu[lOffset]);
               int opLen = pdu[lOffset + 1];
               lOffset += 2;  // skipping tag and Length
               
               // Ab Comment: Dynamically decode OpCode based on length (1-4 bytes) for production-grade robustness.
               mSccpInfo.udt.compInfo.retResult.operation.operationCode = 0;
               for (int i = 0; i < opLen && i < 4; i++) {
                  mSccpInfo.udt.compInfo.retResult.operation.operationCode = 
                     (mSccpInfo.udt.compInfo.retResult.operation.operationCode << 8) | pdu[lOffset++];
               }
               if (opLen > 4) {
                  lOffset += (opLen - 4); // Skip any extra bytes to prevent buffer corruption
               }
            }
            mSccpInfo.udt.compInfo.retResult.paramData.numberOfBytes =
               lCompLen - lOffset + lCompLenOffset + 1;
            memcpy (mSccpInfo.udt.compInfo.retResult.paramData.array,
                  &pdu[lOffset],
                  mSccpInfo.udt.compInfo.retResult.
                  paramData.numberOfBytes);
         }
         break;
      case TCAP_COMP_REJECT:
         mSccpInfo.udt.compInfo.reject.problem.problemCodeType =
            pdu[lOffset++];
         lOffset++;     // skipping Length
         mSccpInfo.udt.compInfo.reject.problem.problemCode =
            pdu[lOffset++];
         T (gTrace, printf ("\n      Problem Type   :%d",
                  mSccpInfo.udt.compInfo.reject.problem.problemCodeType);
               printf ("      Problem Code   :%d",
                  mSccpInfo.udt.compInfo.reject.problem.problemCode););
         break;
      case TCAP_COMP_RET_ERR:
         if (SS7_ANSI_NATIONAL_ERR_CODE_TAG == pdu[lOffset]
               || SS7_ANSI_PRIVATE_ERR_CODE_TAG == pdu[lOffset])
         {
            mSccpInfo.udt.compInfo.retErr.errorCode.errorType =
               pdu[lOffset++];
            mSccpInfo.udt.compInfo.retErr.
               errorCode.errorCodeData.numberOfBytes = pdu[lOffset++];
            for (int i = 0;
                  i < mSccpInfo.udt.compInfo.retErr.
                  errorCode.errorCodeData.numberOfBytes;
                  i++)
               mSccpInfo.udt.compInfo.retErr.
                  errorCode.errorCodeData.array[i] = pdu[lOffset++];
            T (gTrace, printf ("\n      ErrType        :%d",
                     mSccpInfo.udt.compInfo.retErr.errorCode.errorType);
                  printf ("      ErrCode        :%d\n",
                     mSccpInfo.udt.compInfo.retErr.
                     errorCode.errorCodeData.array[0]););
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error Code Missing. Discarding... MsgType:%d", mSccpInfo.msgType);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACUSCCP38, lLogText);
            return false;
         }
         if (lOffset < lCompLenOffset + lCompLen + 1)
         {
            mSccpInfo.udt.compInfo.retErr.paramData.numberOfBytes =
               lCompLen - lOffset + lCompLenOffset + 1;
            memcpy (mSccpInfo.udt.compInfo.retErr.paramData.array,
                  &pdu[lOffset],
                  mSccpInfo.udt.compInfo.retErr.paramData.numberOfBytes);
         }
         else
         {
            mSccpInfo.udt.compInfo.retErr.paramData.numberOfBytes = 0;
         }
         break;
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Unknown Tcap Component. Discarding... MsgType:%d", mSccpInfo.msgType);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP39, lLogText);
         return false;
   }
   return true;
}
//-----------------------------------------------------------------------
// METHOD NAME : DecodeUnitData
// DESCRIPTION : Decodes UnitData
// PARAMETERS  : Address, PDU
//  RETURN     : None
// ----------------------------------------------------------------------
BOOLEAN SccpAculabHandler::DecodeUnitData (acu_sccp_msg_t **lMsgPtr)
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   UINT8      lData[SCCP_ACU_MAX_UDT_LENGTH];
   UINT16     lOffset = 0;
   UINT8      lDataLen = 0;
   UINT16     lPkgLen;
   UINT8      lOrig[100];
   UINT8      lDest[100];
   mIndLenFlag = false;
   memset(&mSccpInfo, '\0', sizeof(_SccpInfo));
#ifdef SS7_TIMESTAMP
                  clock_gettime (CLOCK_REALTIME, &mSccpInfo.timeStamp);
#endif
   // Ab Comment: Populate pcMsgHdlg from Aculab's received message fields.
   // The Aculab library parses the SCCP UDT header internally and exposes
   // the Protocol Class in tm_class and the Return Option in tm_ret_opt.
   // Without this, pcMsgHdlg stays zero after memset, causing returnOnError
   // to always serialize as false in the KafkaBridge JSON output.
   {
      UINT8 lProtoClass = (UINT8)((*lMsgPtr)->tm_class & 0x0F);
      UINT8 lRetOpt     = (*lMsgPtr)->tm_ret_opt ? 0x80 : 0x00;
      mSccpInfo.udt.pcMsgHdlg = lProtoClass | lRetOpt;
   }
   lDataLen = (*lMsgPtr)->tm_data_length;
   memcpy(lData, (*lMsgPtr)->tm_data, lDataLen);
   //T (gTrace,printf("%s: Sccp Unit DataLength:%d\n", gProcessName, lDataLen););
   sprintf(lLogText,"Decoding Unit Data... Sccp Unit DataLength:%d pcMsgHdlg:0x%02X", lDataLen, mSccpInfo.udt.pcMsgHdlg);
   T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUSCCP35, 2 ,lLogText);
  /* printf("---------------------------------PDU------------------------------------------\n");
   for(UINT32 i = 0; i < lDataLen; i++)
   {
      printf("%02X|", lData[i]);
   }
   printf("\n------------------------------------------------------------------------------\n");
 */
   if(NULL == (*lMsgPtr)->tm_local_addr)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Local Address Not Found");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP40, lLogText);
   }
   if(NULL == (*lMsgPtr)->tm_remote_addr)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Remote Address Not Found");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP41, lLogText);
   }
   if(NULL == (*lMsgPtr)->tm_con)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Connection Not Found");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP42, lLogText);
      return false;
   }
   int lTempCount = 0;
   int lDestCnt = 0;
   if(0 == ((*lMsgPtr)->tm_remote_addr->sa_gt.sag_num % 2 ))
   {
      lTempCount = (*lMsgPtr)->tm_remote_addr->sa_gt.sag_num / 2;
   }
   else
   {
      lTempCount = (*lMsgPtr)->tm_remote_addr->sa_gt.sag_num / 2 + 1 ;
   }
   if(0 == ((*lMsgPtr)->tm_local_addr->sa_gt.sag_num % 2 ))
   {
      lDestCnt = (*lMsgPtr)->tm_local_addr->sa_gt.sag_num / 2;
   }
   else
   {
      lDestCnt = (*lMsgPtr)->tm_local_addr->sa_gt.sag_num / 2 + 1 ;
   }
   int j = 0;
   for(int i = 0; i < lTempCount; i++)
   {
      lOrig[j++] = (*lMsgPtr)->tm_remote_addr->sa_gt.sag_digits[i] & 0x0F;
      lOrig[j++] = ((*lMsgPtr)->tm_remote_addr->sa_gt.sag_digits[i] >> 4) & 0x0F;
   }
   int k = 0;
   for(int l = 0; l < lDestCnt; l++)
   {
      lDest[k++] = (*lMsgPtr)->tm_local_addr->sa_gt.sag_digits[l] & 0x0F;
      lDest[k++] = ((*lMsgPtr)->tm_local_addr->sa_gt.sag_digits[l] >> 4) & 0x0F;
   }
   mSccpInfo.udt.clgPartyAddress.numberOfDigits = (*lMsgPtr)->tm_remote_addr->sa_gt.sag_num;
   mSccpInfo.udt.cldPartyAddress.numberOfDigits = (*lMsgPtr)->tm_local_addr->sa_gt.sag_num;
   for (int i = 0; i <  (*lMsgPtr)->tm_remote_addr->sa_gt.sag_num; i++)
   {
      mSccpInfo.udt.clgPartyAddress.digits[i] = lOrig[i];
   }
   for (int i = 0; i < (*lMsgPtr)->tm_local_addr->sa_gt.sag_num; i++)
   {
      mSccpInfo.udt.cldPartyAddress.digits[i] = lDest[i];
   }
   //------------------------------------------------------------ AI
   if((*lMsgPtr)->tm_local_addr->sa_valid & ACU_SCCP_SA_VALID_PC)
   {
      mSccpInfo.udt.cldPartyAddress.pointCode = (*lMsgPtr)->tm_local_addr->sa_pc;
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x01;
   }
   if((*lMsgPtr)->tm_local_addr->sa_valid & ACU_SCCP_SA_VALID_SSN)
   {
      mSccpInfo.udt.cldPartyAddress.subsystemNumber = (*lMsgPtr)->tm_local_addr->sa_ssn;
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x02;
   }
   if((*lMsgPtr)->tm_local_addr->sa_valid & ( ACU_SCCP_SA_VALID_TT |
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES |
            ACU_SCCP_SA_VALID_NAI))
   {
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x10;
      mSccpInfo.udt.cldPartyAddress.natureOfAddress = (*lMsgPtr)->tm_local_addr->sa_nai;
      mSccpInfo.udt.cldPartyAddress.translationType = (*lMsgPtr)->tm_local_addr->sa_tt;
      mSccpInfo.udt.cldPartyAddress.numberingPlan   = (*lMsgPtr)->tm_local_addr->sa_np;
      mSccpInfo.udt.cldPartyAddress.encodingScheme  = (*lMsgPtr)->tm_local_addr->sa_es;
   }
   else if((*lMsgPtr)->tm_local_addr->sa_valid & (ACU_SCCP_SA_VALID_TT |
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES))
   {
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x0C;
      mSccpInfo.udt.cldPartyAddress.translationType = (*lMsgPtr)->tm_local_addr->sa_tt;
      mSccpInfo.udt.cldPartyAddress.numberingPlan   = (*lMsgPtr)->tm_local_addr->sa_np;
      mSccpInfo.udt.cldPartyAddress.encodingScheme  = (*lMsgPtr)->tm_local_addr->sa_es;
   }
   else if((*lMsgPtr)->tm_local_addr->sa_valid & ACU_SCCP_SA_VALID_NAI)
   {
      mSccpInfo.udt.cldPartyAddress.natureOfAddress = (*lMsgPtr)->tm_local_addr->sa_nai;
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x04;
   }
   else if((*lMsgPtr)->tm_local_addr->sa_valid & ACU_SCCP_SA_VALID_TT)
   {
      mSccpInfo.udt.cldPartyAddress.translationType = (*lMsgPtr)->tm_local_addr->sa_tt;
      mSccpInfo.udt.cldPartyAddress.addressIndicator = mSccpInfo.udt.cldPartyAddress.addressIndicator | 0x08;
   }
   //------------------------------------------------------------ AI  remote
   if((*lMsgPtr)->tm_remote_addr->sa_valid & ACU_SCCP_SA_VALID_PC)
   {
      mSccpInfo.udt.clgPartyAddress.pointCode = (*lMsgPtr)->tm_remote_addr->sa_pc;
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x01;
   }
   if((*lMsgPtr)->tm_remote_addr->sa_valid & ACU_SCCP_SA_VALID_SSN)
   {
      mSccpInfo.udt.clgPartyAddress.subsystemNumber = (*lMsgPtr)->tm_remote_addr->sa_ssn;
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x02;
   }
   if((*lMsgPtr)->tm_remote_addr->sa_valid & ( ACU_SCCP_SA_VALID_TT |
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES |
            ACU_SCCP_SA_VALID_NAI))
   {
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x10;
      mSccpInfo.udt.clgPartyAddress.natureOfAddress = (*lMsgPtr)->tm_remote_addr->sa_nai;
      mSccpInfo.udt.clgPartyAddress.translationType = (*lMsgPtr)->tm_remote_addr->sa_tt;
      mSccpInfo.udt.clgPartyAddress.numberingPlan   = (*lMsgPtr)->tm_remote_addr->sa_np;
      mSccpInfo.udt.clgPartyAddress.encodingScheme  = (*lMsgPtr)->tm_remote_addr->sa_es;
   }
   else if((*lMsgPtr)->tm_remote_addr->sa_valid & (ACU_SCCP_SA_VALID_TT |
            ACU_SCCP_SA_VALID_NP |
            ACU_SCCP_SA_VALID_ES))
   {
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x0C;
      mSccpInfo.udt.clgPartyAddress.translationType = (*lMsgPtr)->tm_remote_addr->sa_tt;
      mSccpInfo.udt.clgPartyAddress.numberingPlan   = (*lMsgPtr)->tm_remote_addr->sa_np;
      mSccpInfo.udt.clgPartyAddress.encodingScheme  = (*lMsgPtr)->tm_remote_addr->sa_es;
   }
   else if((*lMsgPtr)->tm_remote_addr->sa_valid & ACU_SCCP_SA_VALID_NAI)
   {
      mSccpInfo.udt.clgPartyAddress.natureOfAddress = (*lMsgPtr)->tm_remote_addr->sa_nai;
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x04;
   }
   else if((*lMsgPtr)->tm_remote_addr->sa_valid & ACU_SCCP_SA_VALID_TT)
   {
      mSccpInfo.udt.clgPartyAddress.translationType = (*lMsgPtr)->tm_remote_addr->sa_tt;
      mSccpInfo.udt.clgPartyAddress.addressIndicator = mSccpInfo.udt.clgPartyAddress.addressIndicator | 0x08;
   }
   //------------------------------------------------------------
/*   //-------------------------------------------------------------
//   mSccpInfo.udt.cldPartyAddress.addressIndicator = 0x12;
   //   ((*lMsgPtr)->tm_local_addr->sa_np << 4) | (((*lMsgPtr)->tm_local_addr->sa_es));
   mSccpInfo.udt.cldPartyAddress.subsystemNumber  = (*lMsgPtr)->tm_local_addr->sa_ssn;
   mSccpInfo.udt.cldPartyAddress.pointCode        = (*lMsgPtr)->tm_local_addr->sa_pc;
   mSccpInfo.udt.cldPartyAddress.translationType  = (*lMsgPtr)->tm_local_addr->sa_tt;
   mSccpInfo.udt.cldPartyAddress.natureOfAddress  = (*lMsgPtr)->tm_local_addr->sa_nai;
   mSccpInfo.udt.cldPartyAddress.numberingPlan    = (*lMsgPtr)->tm_local_addr->sa_np;
   mSccpInfo.udt.cldPartyAddress.encodingScheme   = (*lMsgPtr)->tm_local_addr->sa_es;
//   mSccpInfo.udt.clgPartyAddress.addressIndicator = 0x12;
    //  ((*lMsgPtr)->tm_remote_addr->sa_np << 4) | (((*lMsgPtr)->tm_remote_addr->sa_es));
   mSccpInfo.udt.clgPartyAddress.subsystemNumber  = (*lMsgPtr)->tm_remote_addr->sa_ssn;
   mSccpInfo.udt.clgPartyAddress.pointCode        = (*lMsgPtr)->tm_remote_addr->sa_pc;
   mSccpInfo.udt.clgPartyAddress.translationType  = (*lMsgPtr)->tm_remote_addr->sa_tt;
   mSccpInfo.udt.clgPartyAddress.natureOfAddress  = (*lMsgPtr)->tm_remote_addr->sa_nai;
   mSccpInfo.udt.clgPartyAddress.numberingPlan    = (*lMsgPtr)->tm_remote_addr->sa_np;
   mSccpInfo.udt.clgPartyAddress.encodingScheme   = (*lMsgPtr)->tm_remote_addr->sa_es; */
   //-------------------------------------------------------------
   UINT8 lPkgTag = lData[lOffset++];
   switch (lPkgTag)
   {
      case SS7_ANSI_TRANS_QUERY_WO_PERM:
      case SS7_ANSI_TRANS_QUERY_W_PERM:
         mSccpInfo.udt.transInfo.pkgType = TCAP_TRANS_BEGIN;
         break;
      case SS7_ANSI_TRANS_RESP:
         mSccpInfo.udt.transInfo.pkgType = TCAP_TRANS_END;
         break;
      case SS7_ANSI_TRANS_CONV_W_PERM:
      case SS7_ANSI_TRANS_CONV_WO_PERM:
         mSccpInfo.udt.transInfo.pkgType = TCAP_TRANS_CONTINUE;
         break;
      case SS7_ANSI_TRANS_UNI:
         mSccpInfo.udt.transInfo.pkgType = TCAP_TRANS_UNIDIRECTIONAL;
         break;
      case SS7_ANSI_TRANS_ABORT:
         mSccpInfo.udt.transInfo.pkgType = TCAP_TRANS_ABORT;
         break;
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Unknown ANSI Package Tag: 0x%02X. Discarding...", lPkgTag);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP15, lLogText);
         return false;
   }
   lPkgLen = lData[lOffset++];
   if (0x81 == lPkgLen)
   {
      lPkgLen = lData[lOffset++];
   }
   else if (0x80 == lPkgLen)  // indefinite Length
   {
      lPkgLen = lDataLen - 4; // PkgType + Length + delimiter(0x00 0x00)
      lDataLen -= 2;    // delimiter(0x00 0x00)
   }
   if (lData[lOffset] == SS7_ANSI_TRANS_ID_TAG)
   {
      lOffset++; // skip tag
      UINT8 transIdLen = lData[lOffset++];
      if (transIdLen == 4)
      {
         if (lPkgTag == SS7_ANSI_TRANS_RESP)
         {
            mSccpInfo.udt.transInfo.destTransIdLen = 4;
            mSccpInfo.udt.transInfo.destTransId = 0;
            for (int i = 0; i < 4; i++)
            {
               mSccpInfo.udt.transInfo.destTransId = (mSccpInfo.udt.transInfo.destTransId << 8) | lData[lOffset++];
            }
            mSccpInfo.udt.transInfo.origTransIdLen = 0;
            mSccpInfo.udt.transInfo.origTransId = 0;
         }
         else
         {
            mSccpInfo.udt.transInfo.origTransIdLen = 4;
            mSccpInfo.udt.transInfo.origTransId = 0;
            for (int i = 0; i < 4; i++)
            {
               mSccpInfo.udt.transInfo.origTransId = (mSccpInfo.udt.transInfo.origTransId << 8) | lData[lOffset++];
            }
            mSccpInfo.udt.transInfo.destTransIdLen = 0;
            mSccpInfo.udt.transInfo.destTransId = 0;
         }
      }
      else if (transIdLen == 8)
      {
         mSccpInfo.udt.transInfo.origTransIdLen = 4;
         mSccpInfo.udt.transInfo.origTransId = 0;
         for (int i = 0; i < 4; i++)
         {
            mSccpInfo.udt.transInfo.origTransId = (mSccpInfo.udt.transInfo.origTransId << 8) | lData[lOffset++];
         }
         mSccpInfo.udt.transInfo.destTransIdLen = 4;
         mSccpInfo.udt.transInfo.destTransId = 0;
         for (int i = 0; i < 4; i++)
         {
            mSccpInfo.udt.transInfo.destTransId = (mSccpInfo.udt.transInfo.destTransId << 8) | lData[lOffset++];
         }
      }
      else
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Invalid ANSI Transaction ID Length: %d. Discarding...", transIdLen);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP15, lLogText);
         return false;
      }
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "ANSI Transaction ID Tag (0xC7) Missing (found 0x%02X). Discarding...", lData[lOffset]);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACUSCCP15, lLogText);
      return false;
   }
   if (TCAP_TRANS_ABORT == mSccpInfo.udt.transInfo.pkgType)
   {
      if (lOffset < lDataLen)
      {
         switch (lData[lOffset++])
         {
            case TCAP_P_ABRT:
               mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_P_ABRT;
               lOffset++;
               mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes = 1;
               mSccpInfo.udt.compInfo.abort.abortCompReason.array[0] = lData[lOffset];
               break;
            case TCAP_U_ABRT:
               mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_U_ABRT;
               mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes = lData[lOffset++];
               memcpy (mSccpInfo.udt.compInfo.abort.abortCompReason.array,
                     &lData[lOffset],
                     mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes);
               break;
            default:
               mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_NULL_ABRT;
               mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes = 0;
               break;
         }
      }
      return true;
   }
   switch (lData[lOffset])
   {
      case SS7_DLG_PORTION_TAG:
         if (0x80 == lData[lOffset + 1]) //Indefinite Length
         {
            mIndLenFlag = true;
            sprintf(lLogText,"TransactionId:%08x, Indefinite Length DlgPortion", mSccpInfo.udt.transInfo.origTransId);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUSCCP31, 1, lLogText);
            int i = 0;
            for (i = lOffset + 2; i < lPkgLen; i++)
            {
               if (SS7_ANSI_COMP_PORTION_TAG == lData[i])
               {
                  if (0x00 == lData[i - 1] && 0x00 == lData[i - 2])
                     break;
               }
            }
            mSccpInfo.udt.dlgInfo.dlgPdu.numberOfBytes = i - lOffset;
         }
         else if (0x81 == lData[lOffset + 1])
         {
            mSccpInfo.udt.dlgInfo.dlgPdu.numberOfBytes =
               lData[lOffset + 2] + 3;
         }
         else
         {
            mSccpInfo.udt.dlgInfo.dlgPdu.numberOfBytes =
               lData[lOffset + 1] + 2;
         }
         memcpy (mSccpInfo.udt.dlgInfo.dlgPdu.array,
               &lData[lOffset],
               mSccpInfo.udt.dlgInfo.dlgPdu.numberOfBytes);
         // skipping Dlg Portion
         lOffset += mSccpInfo.udt.dlgInfo.dlgPdu.numberOfBytes;
         if (SS7_ANSI_COMP_PORTION_TAG != lData[lOffset])
            break;
      case SS7_ANSI_COMP_PORTION_TAG:
         // skipping Tag
         lOffset++;
         return this->DecodeComponent (&lData[lOffset], lDataLen - lOffset);
         break;
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "No Component/Dialogue Tag found, MsgType:%d", mSccpInfo.msgType);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP16, lLogText);
         //if(mDumpPdu == 1)
         //   DumpPduToFile(lLogText);
         return false;
   }
   return true;
}
//-----------------------------------------------------------------------
// METHOD NAME : EncodeSccpUnitData
// DESCRIPTION : Encodes UnitData
// PARAMETERS  : Address, PDU
// RETURN      : None
// ----------------------------------------------------------------------
BOOLEAN SccpAculabHandler::EncodeSccpUnitData(_SccpInfo &lSccpMsg, UINT8 lData[], UINT16& lDataLen)
{
   UINT16     lOffset = 0;
   UINT16     lPkgLenIdx;
   UINT8 lPkgTag = 0;
   switch (lSccpMsg.udt.transInfo.pkgType)
   {
      case TCAP_TRANS_BEGIN:
         lPkgTag = SS7_ANSI_TRANS_QUERY_W_PERM;
         break;
      case TCAP_TRANS_END:
         lPkgTag = SS7_ANSI_TRANS_RESP;
         break;
      case TCAP_TRANS_CONTINUE:
         lPkgTag = SS7_ANSI_TRANS_CONV_W_PERM;
         break;
      case TCAP_TRANS_ABORT:
         lPkgTag = SS7_ANSI_TRANS_ABORT;
         break;
      case TCAP_TRANS_UNIDIRECTIONAL:
         lPkgTag = SS7_ANSI_TRANS_UNI;
         break;
      default:
         lPkgTag = SS7_ANSI_TRANS_QUERY_W_PERM;
         break;
   }
   lData[lOffset++] = lPkgTag;
   lPkgLenIdx = lOffset;
   lOffset++; // Package Length Placeholder
   lData[lOffset++] = SS7_ANSI_TRANS_ID_TAG;
   if (lSccpMsg.udt.transInfo.pkgType == TCAP_TRANS_CONTINUE)
   {
      lData[lOffset++] = 8;
      lOffset += EncodeTransId (4, lSccpMsg.udt.transInfo.origTransId, &lData[lOffset]);
      lOffset += EncodeTransId (4, lSccpMsg.udt.transInfo.destTransId, &lData[lOffset]);
   }
   else if (lSccpMsg.udt.transInfo.pkgType == TCAP_TRANS_END || lSccpMsg.udt.transInfo.pkgType == TCAP_TRANS_ABORT)
   {
      lData[lOffset++] = 4;
      UINT32 transId = (lSccpMsg.udt.transInfo.destTransId != 0) ? 
                        lSccpMsg.udt.transInfo.destTransId : 
                        lSccpMsg.udt.transInfo.origTransId;
      lOffset += EncodeTransId (4, transId, &lData[lOffset]);
   }
   else // Query / Begin / Unidirectional
   {
      lData[lOffset++] = 4;
      lOffset += EncodeTransId (4, lSccpMsg.udt.transInfo.origTransId, &lData[lOffset]);
   }
   if (lSccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes > 0)
   {
      lData[lOffset++] = SS7_DLG_PORTION_TAG;
      memcpy (&lData[lOffset], lSccpMsg.udt.dlgInfo.dlgPdu.array,
            lSccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes);
      lOffset += lSccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes;
   }
   // ----------- Component Portion ---------------------
   if (lSccpMsg.udt.compInfo.tcapComp != 0)
   {
      lOffset += this->EncodeComponent (&lData[lOffset], lSccpMsg);
   }
   UINT8 lLength = 0;
   lLength = lOffset - 2;
   if (lLength > 0x7F)
   {
      lData[lPkgLenIdx++] = 0x81; // long form 1 byte
      memmove (&lData[lPkgLenIdx + 1], &lData[lPkgLenIdx], lOffset);
      lOffset++;
   }
   lData[lPkgLenIdx] = lLength;
   lDataLen = lOffset;
   return true;
}
//----------------------------------------------------------------------
//// METHOD NAME : EncodeTransId
//// DESCRIPTION : Encodes the
//// PARAMETERS  : Address, pdu, offset
//// RETURN      : BOOLEAN
////----------------------------------------------------------------------
UINT8 SccpAculabHandler::EncodeTransId (UINT8 transIdLen, UINT32 transId, UINT8 * pdu)
{
   UINT8 lOffset = 0;
   // Ab Comment : Removed redundant pdu[lOffset++] = transIdLen which was illegally shifting the entire ASN.1 sequence
   if (transIdLen > 3 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 24) & 0xFF;
   if (transIdLen > 2 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 16) & 0xFF;
   if (transIdLen > 1 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 8) & 0xFF;
   pdu[lOffset++] = (transId) & 0xFF;
   return lOffset;
}
//----------------------------------------------------------------------
// METHOD NAME : EncodeAddress
// DESCRIPTION : Encodes the Address
// PARAMETERS  : Address, pdu, offset
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SccpAculabHandler::EncodeAddress(acu_sccp_addr_t& lAddr, TCAPAddress &lAddress)
{
   // Ab Comment: Debug - trace raw AI entering EncodeAddress and which branch is taken
   /*printf("[EncodeAddress] AI=0x%02X numDigits=%d PC=%d SSN=%d\n",
          (unsigned)lAddress.addressIndicator,
          (int)lAddress.numberOfDigits,
          (int)lAddress.pointCode,
          (int)lAddress.subsystemNumber);*/
   if((lAddress.addressIndicator & 0x40) != 0)
   {
      lAddr.sa_flags = ACU_SCCP_SA_FLAGS_ROUTE_SSN;
   }
   if(lAddress.numberOfDigits > 0)
   {
      // Ab Comment: Do NOT set ACU_SCCP_SA_VALID_GTI explicitly.
      // Setting VALID_GTI without sag_gti causes Aculab to default to GTI=1 (NAI-only format)
      // which is wrong. Without VALID_GTI, Aculab auto-derives the correct ANSI GTI-4
      // from the ACU_SCCP_SA_VALID_TT | VALID_NP | VALID_ES flags set below (via AI bit 0x10).
      //lAddr.sa_valid = (lAddr.sa_valid | ACU_SCCP_SA_VALID_GTI);
      lAddr.sa_gt.sag_num = lAddress.numberOfDigits;
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
         lAddr.sa_gt.sag_digits[j] = 0x0F & lAddress.digits[i++];
         lAddr.sa_gt.sag_digits[j] |= (0xF0 & (lAddress.digits[i++] << 4));
         j++;
      }
      if(SCCP_ES_BCD_ODD == lAddress.encodingScheme)
      {
         lAddr.sa_gt.sag_digits[lTemcount - 1] = lAddr.sa_gt.sag_digits[lTemcount - 1] & 0x0F;
      }
   }
   if(lAddress.addressIndicator & 0x01 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_PC;
      lAddr.sa_pc = lAddress.pointCode;
   }
   if(lAddress.addressIndicator & 0x02 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_SSN;
      lAddr.sa_ssn = lAddress.subsystemNumber;
   }
   if(lAddress.addressIndicator & 0x04 )
   {
      // ANSI SCCP: NAI is not supported in the Global Title.
      // lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_NAI;
      // lAddr.sa_nai = lAddress.natureOfAddress;
   }
   if(lAddress.addressIndicator & 0x08 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_TT;
      lAddr.sa_tt = lAddress.translationType;
   }
   if(lAddress.addressIndicator & 0x0c )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_TT
         |ACU_SCCP_SA_VALID_NP
         |ACU_SCCP_SA_VALID_ES;
      lAddr.sa_tt = lAddress.translationType;
      lAddr.sa_np = lAddress.numberingPlan;
      lAddr.sa_es = lAddress.encodingScheme;
   }
   if(lAddress.addressIndicator & 0x10 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_TT
         | ACU_SCCP_SA_VALID_NP
         | ACU_SCCP_SA_VALID_ES;
         // ANSI SCCP: Do NOT set VALID_NAI
         // | ACU_SCCP_SA_VALID_NAI;
      lAddr.sa_tt = lAddress.translationType;
      lAddr.sa_np = lAddress.numberingPlan;
      lAddr.sa_es = lAddress.encodingScheme;
      // lAddr.sa_nai = lAddress.natureOfAddress;
   }
   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : EncodeComponent
// DESCRIPTION : Encodes the
// PARAMETERS  :
// RETURN      :
//----------------------------------------------------------------------
UINT8 SccpAculabHandler::EncodeComponent (UINT8 * pdu, _SccpInfo &lSccpInfo)
{
   UINT8 lOffset = 0;
   UINT8 lLength = 0;
   UINT8 lCompLenIdx = 0;
   UINT8 lCompSeqLenIdx = 0;
   pdu[lOffset++] = SS7_ANSI_COMP_PORTION_TAG;
   lCompSeqLenIdx = lOffset++;   // skipping Component length
   UINT8 lCompTag = 0;
   switch (lSccpInfo.udt.compInfo.tcapComp)
   {
      case TCAP_COMP_INVOKE:
         lCompTag = SS7_ANSI_COMP_INVOKE_LAST;
         break;
      case TCAP_COMP_RET_REULT_LAST:
      case TCAP_COMP_RET_RESULT:
         lCompTag = SS7_ANSI_COMP_RET_RESULT_LAST;
         break;
      case TCAP_COMP_RET_ERR:
         lCompTag = SS7_ANSI_COMP_RET_ERROR;
         break;
      case TCAP_COMP_REJECT:
         lCompTag = SS7_ANSI_COMP_REJECT;
         break;
      default:
         lCompTag = SS7_ANSI_COMP_INVOKE_LAST;
         break;
   }
   pdu[lOffset++] = lCompTag;
   lCompLenIdx = lOffset++;   // skipping Component length
   pdu[lOffset++] = SS7_ANSI_INVOKE_ID_TAG;
   pdu[lOffset++] = 1;
   pdu[lOffset++] = lSccpInfo.udt.compInfo.invokeId;
   if (lSccpInfo.udt.compInfo.tcapComp == TCAP_COMP_INVOKE &&
       lSccpInfo.udt.compInfo.invoke.linkedId.linkedId != 0)
   {
      pdu[lOffset++] = SS7_ANSI_LINKED_ID_TAG;
      pdu[lOffset++] = 1;
      pdu[lOffset++] = lSccpInfo.udt.compInfo.invoke.linkedId.linkedId;
   }
   switch (lSccpInfo.udt.compInfo.tcapComp)
   {
      case TCAP_COMP_INVOKE:
      {
         // Ab Comment : Respect isPrivate flag to dynamically map National (0xD0) vs Private (0xD1) operation code.
         if (lSccpInfo.udt.compInfo.invoke.operation.isPrivate)
         {
            pdu[lOffset++] = SS7_ANSI_PRIVATE_OP_CODE_TAG;
         }
         else
         {
            pdu[lOffset++] = SS7_ANSI_NATIONAL_OP_CODE_TAG;
         }
         // Ab Comment: Dynamically encode OpCode length based on value to support any ANSI message type.
         UINT32 opCode = lSccpInfo.udt.compInfo.invoke.operation.operationCode;
         if (opCode > 0xFFFFFF) {
             pdu[lOffset++] = 4;
             pdu[lOffset++] = (opCode >> 24) & 0xFF;
             pdu[lOffset++] = (opCode >> 16) & 0xFF;
             pdu[lOffset++] = (opCode >> 8) & 0xFF;
             pdu[lOffset++] = opCode & 0xFF;
         } else if (opCode > 0xFFFF) {
             pdu[lOffset++] = 3;
             pdu[lOffset++] = (opCode >> 16) & 0xFF;
             pdu[lOffset++] = (opCode >> 8) & 0xFF;
             pdu[lOffset++] = opCode & 0xFF;
         } else if (opCode > 0xFF) {
             pdu[lOffset++] = 2;
             pdu[lOffset++] = (opCode >> 8) & 0xFF;
             pdu[lOffset++] = opCode & 0xFF;
         } else {
             pdu[lOffset++] = 1;
             pdu[lOffset++] = opCode & 0xFF;
         }
         memcpy (&pdu[lOffset],
               lSccpInfo.udt.compInfo.invoke.paramData.array,
               lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes);
         lOffset +=
            lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes;
         break;
      }
      case TCAP_COMP_RET_REULT_LAST:
      case TCAP_COMP_RET_RESULT:
      {
         if (0 != lSccpInfo.udt.compInfo.retResult.paramData.numberOfBytes)
         {
            pdu[lOffset++] = SS7_ANSI_PARAM_SEQUENCE_TAG;
            pdu[lOffset++] =
               lSccpInfo.udt.compInfo.retResult.paramData.numberOfBytes + 4;
            // Ab Comment : Respect isPrivate flag to dynamically map National (0xD0) vs Private (0xD1) operation code.
            if (lSccpInfo.udt.compInfo.retResult.operation.isPrivate)
            {
               pdu[lOffset++] = SS7_ANSI_PRIVATE_OP_CODE_TAG;
            }
            else
            {
               pdu[lOffset++] = SS7_ANSI_NATIONAL_OP_CODE_TAG;
            }
            // Ab Comment: Dynamically encode OpCode length based on value to support any ANSI message type.
            UINT32 retOpCode = lSccpInfo.udt.compInfo.retResult.operation.operationCode;
            if (retOpCode > 0xFFFFFF) {
                pdu[lOffset++] = 4;
                pdu[lOffset++] = (retOpCode >> 24) & 0xFF;
                pdu[lOffset++] = (retOpCode >> 16) & 0xFF;
                pdu[lOffset++] = (retOpCode >> 8) & 0xFF;
                pdu[lOffset++] = retOpCode & 0xFF;
            } else if (retOpCode > 0xFFFF) {
                pdu[lOffset++] = 3;
                pdu[lOffset++] = (retOpCode >> 16) & 0xFF;
                pdu[lOffset++] = (retOpCode >> 8) & 0xFF;
                pdu[lOffset++] = retOpCode & 0xFF;
            } else if (retOpCode > 0xFF) {
                pdu[lOffset++] = 2;
                pdu[lOffset++] = (retOpCode >> 8) & 0xFF;
                pdu[lOffset++] = retOpCode & 0xFF;
            } else {
                pdu[lOffset++] = 1;
                pdu[lOffset++] = retOpCode & 0xFF;
            }
            memcpy (&pdu[lOffset],
                  lSccpInfo.udt.compInfo.retResult.paramData.array,
                  lSccpInfo.udt.compInfo.retResult.paramData.
                  numberOfBytes);
            lOffset +=
               lSccpInfo.udt.compInfo.retResult.paramData.numberOfBytes;
         }
         break;
      }
      case TCAP_COMP_RET_ERR:
         T (gTrace, printf ("\n      ErrCode        :%d\n",
                  lSccpInfo.udt.compInfo.retErr.errorCode.
                  errorCodeData.array[0]););
         pdu[lOffset++] = SS7_ANSI_NATIONAL_ERR_CODE_TAG;
         pdu[lOffset++] = 1;
         pdu[lOffset++] =
            lSccpInfo.udt.compInfo.retErr.errorCode.errorCodeData.
            array[0];
         if (lSccpInfo.udt.compInfo.retErr.paramData.numberOfBytes)
         {
            memcpy (&pdu[lOffset],
                  lSccpInfo.udt.compInfo.retErr.paramData.array,
                  lSccpInfo.udt.compInfo.retErr.paramData.
                  numberOfBytes);
            lOffset +=
               lSccpInfo.udt.compInfo.retErr.paramData.numberOfBytes;
         }
         break;
      case TCAP_COMP_REJECT:
         T (gTrace,
               printf ("\n      Problem Type   :%02X\n",
                  lSccpInfo.udt.compInfo.reject.problem.
                  problemCodeType);
               printf ("      Problem Code   :%d\n",
                  lSccpInfo.udt.compInfo.reject.problem.problemCode););
         pdu[lOffset++] =
            lSccpInfo.udt.compInfo.reject.problem.problemCodeType;
         pdu[lOffset++] = 1;
         pdu[lOffset++] =
            lSccpInfo.udt.compInfo.reject.problem.problemCode;
         break;
      default:
         break;
   }
   // Updating Comp Len
   lLength = lOffset - lCompLenIdx - 1;
   if (lLength > 0x7F)
   {
      pdu[lCompLenIdx++] = 0x81; // long form 1 byte
      memmove (&pdu[lCompLenIdx + 1], &pdu[lCompLenIdx], lLength);
      lOffset++;
   }
   pdu[lCompLenIdx] = lLength;
   // Updating Comp Seq Len
   lLength = lOffset - lCompSeqLenIdx - 1;
   if (lLength > 0x7F)
   {
      pdu[lCompSeqLenIdx++] = 0x81; // long form 1 byte
      memmove (&pdu[lCompSeqLenIdx + 1], &pdu[lCompSeqLenIdx], lLength);
      lOffset++;
   }
   pdu[lCompSeqLenIdx] = lLength;
   return lOffset;
}
//----------------------------------------------------------------------
// METHOD      : DisplayAddress
// DESCRIPTION : Display TCAPAddress structure
// PARAMETER   : TCAPAddress , TEXT
// RETURN      : None
//----------------------------------------------------------------------
void SccpAculabHandler::DisplayAddress (const TCAPAddress * address, TEXT *lText)
{
   if (NULL == address || address->addressIndicator == 0)
   {
      T(gTrace, printf(" %sAddress-----------:Absent\n",lText););
      return;
   }
   T(gTrace, {
      printf(" %sAddress-----------:Included\n",lText);
      printf("\t%-15s:%d\n","addressInd", address->addressIndicator);
      printf("\t%-15s:%d\n","subsystemNo", address->subsystemNumber);
      printf("\t%-15s:%d\n","pointCode", address->pointCode);
      printf("\t%-15s:%d\n","natureOfAddr", address->natureOfAddress);
      printf("\t%-15s:%d\n","translationType", address->translationType);
      printf("\t%-15s:%d\n","numberingPlan", address->numberingPlan);
      printf("\t%-15s:%d\n","encodingScheme", address->encodingScheme);
      printf("\t%-15s:","digits");
      for (int i = 0; i <  address->numberOfDigits; i++)
         printf("%X", address->digits[i]);
      printf("\n");
   });
}
