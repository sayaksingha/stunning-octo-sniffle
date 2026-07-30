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

   if (false == lCfgRead.CfgInit(SS7_IPC_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed...\n",
            gProcessName,SS7_IPC_CFG);
      gLog.GenerateLog(GSYS09, "ss7.cfg CfgRead object initializaton failed");
      return false;
   }
   if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_SCCP_HDLR_Q_RCV",mMsgQ.RdMsgQKey ,
            SS7_MIN_IPC_Q_KEY, SS7_MAX_IPC_Q_KEY))
   {
      sprintf(lLogText,
            "Config read Err for MSG_SCCP_HDLR_Q_RCV in %s",
            SS7_IPC_CFG);
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
            SS7_IPC_CFG);
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
            SS7_IPC_CFG);
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

   if (false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SS7_KER_CFG);
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
            SS7_KER_CFG);
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
            SS7_KER_CFG);
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
            35000))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_DESTINATION_1 in %s",
            SS7_KER_CFG);
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
            35000))
   {
      sprintf(lLogText,
            "Config read Err for SCCP_DESTINATION_2 in %s",
            SS7_KER_CFG);
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

   if (false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      printf("%s: CfgRead object(%s) intialization failed.\n",
            gProcessName,SS7_KER_CFG);
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
            SS7_KER_CFG);
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
            SS7_KER_CFG);
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
   msgQueueError  lQueErr;

   // Read Messages from Heart-Beat Queue
   lQueErr = mMsgQ.HbQ.ReadMsg((void *)&lHeartBeatText, lMsgLen, mMsgType, NON_BLOCKING);

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
               //if(false == TxMsgToApplication(mSccpInfo))
                  //return false;
                  TxMsgToApplication(mSccpInfo);
                  mAculabApi.UnblockConnection((*lMsgPtr)->tm_con);
                  acu_sccp_msg_free(*lMsgPtr);
               break;
            }
         case ACU_SCCP_MSG_SP_STATUS:
            //case ACU_SCCP_MSG_USER_STATUS:
            {
               mAculabApi.GetSccpStatus(mDestinationA, mDestAStatus);
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Got SCCP Status Dest-1-Status:%d", mDestAStatus);
               T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACUSCCP25,lLogText);

               if(mDestinationB)
               {
                  mAculabApi.GetSccpStatus(mDestinationB, mDestBStatus);
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Got SCCP Status Dest-2-Status:%d", mDestBStatus);
                  T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACUSCCP25,lLogText);
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
   INT16          lMsgLen = sizeof (_SccpInfo);
   msgQueueError  lQueErr;

   memset (&lRxSccpMsg, 0, lMsgLen);

   lQueErr = mMsgQ.RdQ.ReadMsg((void *)&lRxSccpMsg, lMsgLen, mMsgType, BLOCKING);

   if (Q_SUCCESS != lQueErr)
   {
      return AculabUtil::HandleQueueReadError (lQueErr, mMsgQ.RdQ);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "MsgType:%d QueueKey:%d Sccp Message Received from Application",
         mMsgType, mMsgQ.WrMsgQKey);
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
      T (gTrace, printf("-----------------%s APPLICATION --------------------\n",lText););
      printf("\tSccpMsgType    :%d\n", SccpMsg.msgType);
      printf("==================UDT===================\n");
      printf("\tpcMsgHdlg      :%d \n", SccpMsg.udt.pcMsgHdlg);
      DisplayAddress(&SccpMsg.udt.clgPartyAddress, "Orig");
      DisplayAddress(&SccpMsg.udt.cldPartyAddress, "Dest");
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
      printf("\tLinkedId       :%d\n", SccpMsg.udt.compInfo.invoke.linkedId.linkedId);
      printf("\tOpCode         :%d\n", SccpMsg.udt.compInfo.invoke.operation.operationCode);
      printf("\tParamLength    :%d\n", SccpMsg.udt.compInfo.invoke.paramData.numberOfBytes);
      printf("\tComponentData  :");
      for (int i = 0; i < SccpMsg.udt.compInfo.invoke.paramData.numberOfBytes; i++)
         printf ("%02X ", SccpMsg.udt.compInfo.invoke.paramData.array[i]);
      printf("\n");
      T (gTrace, printf("-----------------------------------------------------\n"););

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

   mSccpInfo.udt.compInfo.tcapComp = (EnumTcapCompType) pdu[lOffset++];

   lCompLen = pdu[lOffset++]; // skipping component len
   if (0x81 == lCompLen)
      lCompLen = pdu[lOffset++];
   else if (0x80 == lCompLen)
   {
      //T (gTrace, printf ("%s: Indefinite Length. CompType\n", gProcessName););
      mIndLenFlag = true;
      sprintf(lLogText,"TransactionId:%08x, Indefinite Length. CompType", mSccpInfo.udt.transInfo.origTransId);
      //sprintf(lLogText,"TransactionId:%08x, Indefinite Length. CompType", origTransId);
      T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUSCCP33, 1, lLogText);
      pduLen -= 2;
      lCompLen = pduLen - lOffset;
   }

   lCompLenOffset = lOffset - 1;

   // Invoke id
   if (SS7_INVOKE_ID_TAG == pdu[lOffset])
   {
      lOffset += 2;     // skippng Length
      mSccpInfo.udt.compInfo.invokeId = pdu[lOffset++];
   }

   // LinkedId
   if (SS7_LINKED_ID_TAG == pdu[lOffset])
   {
      lOffset += 2;     // skipping tag and Length
      mSccpInfo.udt.compInfo.invoke.linkedId.linkedId =
         pdu[lOffset++];
   }

   switch (mSccpInfo.udt.compInfo.tcapComp)
   {
      case TCAP_COMP_INVOKE:
         if (SS7_LOCAL_OP_CODE_TAG == pdu[lOffset])
         {
            lOffset += 2;     // skipping tag and Length
            mSccpInfo.udt.compInfo.invoke.operation.operationCode =
               pdu[lOffset++];
         }
         else if (SS7_GLOBAL_OP_CODE_TAG == pdu[lOffset])
         {
            lOffset += (pdu[lOffset + 1] + 2);
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Operation Code Missing. Discarding... MsgType:%d", mSccpInfo.msgType);
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACUSCCP44, lLogText);
            //if(mDumpPdu == 1)
            //   DumpPduToFile(lLogText);
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
         if (0x30 == pdu[lOffset++])
         {
            if (0x81 == pdu[lOffset++])
               lOffset++;

            if (SS7_LOCAL_OP_CODE_TAG == pdu[lOffset])
            {
               lOffset += 2;  // skipping tag and Length

               mSccpInfo.udt.compInfo.retResult.
                  operation.operationCode = pdu[lOffset++];
            }
            else if (SS7_GLOBAL_OP_CODE_TAG == pdu[lOffset])
            {
               lOffset += (pdu[lOffset + 1] + 2);
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
         //mSccpInfo.udt.compInfo.reject.rejectType =
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
         if (SS7_LOCAL_ERR_CODE_TAG == pdu[lOffset]
               || SS7_GLOBAL_ERR_CODE_TAG == pdu[lOffset])
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
            //if(mDumpPdu == 1)
            //   DumpPduToFile(lLogText);
            return false;
         }

         break;

      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Unknown Tcap Component. Discarding... MsgType:%d", mSccpInfo.msgType);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP39, lLogText);
         //if(mDumpPdu == 1)
         //   DumpPduToFile(lLogText);
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
   lDataLen = (*lMsgPtr)->tm_data_length;
   memcpy(lData, (*lMsgPtr)->tm_data, lDataLen);
   //T (gTrace,printf("%s: Sccp Unit DataLength:%d\n", gProcessName, lDataLen););
   sprintf(lLogText,"Decoding Unit Data... Sccp Unit DataLength:%d", lDataLen);
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

   mSccpInfo.udt.transInfo.pkgType = (EnumTcapTransType)lData[lOffset++];
   lPkgLen = lData[lOffset++];
   if (0x81 == lPkgLen)
   {
      lPkgLen = lData[lOffset++];
   }
   else if (0x80 == lPkgLen)  // indefinite Length
   {
      //printf ("Indefinite Length\n");
      lPkgLen = lDataLen - 4; // PkgType + Length + delimiter(0x00 0x00)
      lDataLen -= 2;    // delimiter(0x00 0x00)
   }

   switch (lData[lOffset])
   {
      case SS7_ORIG_TRANS_ID_TAG:
         lOffset++;
         mSccpInfo.udt.transInfo.origTransIdLen = lData[lOffset];
         for (int i = 1; i <= lData[lOffset]; i++)
            mSccpInfo.udt.transInfo.origTransId =
               ((mSccpInfo.udt.transInfo.origTransId << 8) | lData[lOffset + i]);

         lOffset += (lData[lOffset] + 1);

         if (SS7_DEST_TRANS_ID_TAG != lData[lOffset])
            break;

      case SS7_DEST_TRANS_ID_TAG:
         lOffset++;
         mSccpInfo.udt.transInfo.destTransIdLen = lData[lOffset];
         for (int i = 1; i <= lData[lOffset]; i++)
            mSccpInfo.udt.transInfo.destTransId =
               ((mSccpInfo.udt.transInfo.destTransId << 8) | lData[lOffset + i]);

         lOffset += (lData[lOffset] + 1);

         break;

      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "TransactionId Missing. Discarding... MsgType:%d", mSccpInfo.msgType);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACUSCCP15, lLogText);
         //if(mDumpPdu == 1)
         //   DumpPduToFile(lLogText);
         return false;

   }

   if (TCAP_TRANS_ABORT == mSccpInfo.udt.transInfo.pkgType)
   {
      switch (lData[lOffset++])
      {
         case TCAP_P_ABRT:
            mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_P_ABRT;

            lOffset++;

            mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes =
               1;
            mSccpInfo.udt.compInfo.abort.abortCompReason.array[0] =
               lData[lOffset];
            break;

         case TCAP_U_ABRT:
            mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_U_ABRT;

            mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes =
               lData[lOffset++];
            memcpy (mSccpInfo.udt.compInfo.abort.abortCompReason.array,
                  &lData[lOffset],
                  mSccpInfo.udt.compInfo.abort.
                  abortCompReason.numberOfBytes);
            break;

         default:
            mSccpInfo.udt.compInfo.abort.abortCompType = TCAP_NULL_ABRT;

            mSccpInfo.udt.compInfo.abort.abortCompReason.numberOfBytes = 0;
            break;
      }
      return true;
   }
   switch (lData[lOffset])
   {
      case SS7_DLG_PORTION_TAG:

         if (0x80 == lData[lOffset + 1]) //Indefinite Length
         {
            //T (gTrace, printf ("%s: Indefinite Length. DlgPortion\n", gProcessName););
            mIndLenFlag = true;
            sprintf(lLogText,"TransactionId:%08x, Indefinite Length DlgPortion", mSccpInfo.udt.transInfo.origTransId);
            T(gTrace,printf("%s: %s\n",gProcessName,lLogText););
            gLog.GenerateLog(ACUSCCP31, 1, lLogText);
            int i = 0;

            for (i = lOffset + 2; i < lPkgLen; i++)
            {
               if (SS7_COMP_PORTION_TAG == lData[i])
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

         if (SS7_COMP_PORTION_TAG != lData[lOffset])
            break;

      case SS7_COMP_PORTION_TAG:
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

   lData[lOffset++] = lSccpMsg.udt.transInfo.pkgType;
   lPkgLenIdx = lOffset;
   lOffset++; //Package Length

   if(0 != lSccpMsg.udt.transInfo.origTransIdLen)
   {
      lData[lOffset++] = SS7_ORIG_TRANS_ID_TAG;

      lOffset += EncodeTransId (lSccpMsg.udt.transInfo.origTransIdLen,
            lSccpMsg.udt.transInfo.origTransId, &lData[lOffset]);
   }

   if(0 != lSccpMsg.udt.transInfo.destTransIdLen)
   {
      lData[lOffset++] = SS7_DEST_TRANS_ID_TAG;

      lOffset += EncodeTransId (lSccpMsg.udt.transInfo.destTransIdLen,
            lSccpMsg.udt.transInfo.destTransId, &lData[lOffset]);
   }

   lData[lOffset] = SS7_DLG_PORTION_TAG;
   memcpy (&lData[lOffset], lSccpMsg.udt.dlgInfo.dlgPdu.array,
         lSccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes);

   lOffset += lSccpMsg.udt.dlgInfo.dlgPdu.numberOfBytes;

   // ----------- Component Portion ---------------------
   lOffset += this->EncodeComponent (&lData[lOffset], lSccpMsg);
#if 0
   lData[lPkgLenIdx] = lOffset - 2;
   lDataLen = lOffset;
#endif

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

#if 0
   printf("---------------------------------PDU------------------------------------------\n");
   for(UINT32 i = 0; i < lDataLen; i++)
   {
      printf("%02X|", (lData[i]));
   }
   printf("\n------------------------------------------------------------------------------\n");
#endif

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

   lOffset++;        // skipping Length

   if (transIdLen > 3 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 24) & 0xFF;

   if (transIdLen > 2 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 16) & 0xFF;

   if (transIdLen > 1 || transIdLen == 0)
      pdu[lOffset++] = (transId >> 8) & 0xFF;

   pdu[lOffset++] = (transId) & 0xFF;

   pdu[0] = lOffset - 1;

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
   //TEXT       lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if((lAddress.addressIndicator & 0x40) == 0)
   {
      //lAddr.sa_flags = ACU_SCCP_SA_FLAGS_RAW_GT;
      lAddr.sa_valid = (lAddr.sa_valid | ACU_SCCP_SA_VALID_GTI);
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
   else
   {
      lAddr.sa_flags = ACU_SCCP_SA_FLAGS_ROUTE_SSN;
   }

   if(lAddress.addressIndicator & 0x02 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_SSN;
      lAddr.sa_ssn = lAddress.subsystemNumber;
   }

   if(lAddress.addressIndicator & 0x04 )
   {
      lAddr.sa_valid = lAddr.sa_valid | ACU_SCCP_SA_VALID_NAI;
      lAddr.sa_nai = lAddress.natureOfAddress;
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
         | ACU_SCCP_SA_VALID_ES
         | ACU_SCCP_SA_VALID_NAI;

      lAddr.sa_tt = lAddress.translationType;
      lAddr.sa_np = lAddress.numberingPlan;
      lAddr.sa_es = lAddress.encodingScheme;
      lAddr.sa_nai = lAddress.natureOfAddress;
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

   pdu[lOffset++] = SS7_COMP_PORTION_TAG;

   lCompSeqLenIdx = lOffset++;   // skipping Component length

   pdu[lOffset++] = lSccpInfo.udt.compInfo.tcapComp;

   lCompLenIdx = lOffset++;   // skipping Component length

   pdu[lOffset++] = SS7_INVOKE_ID_TAG;
   pdu[lOffset++] = 1;
   pdu[lOffset++] = lSccpInfo.udt.compInfo.invokeId;

   switch (lSccpInfo.udt.compInfo.tcapComp)
   {
      case TCAP_COMP_INVOKE:

         pdu[lOffset++] = SS7_LOCAL_OP_CODE_TAG;
         pdu[lOffset++] = 1;
         pdu[lOffset++] =
            lSccpInfo.udt.compInfo.invoke.operation.operationCode;

         memcpy (&pdu[lOffset],
               lSccpInfo.udt.compInfo.invoke.paramData.array,
               lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes);

         lOffset +=
            lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes;
         break;

      case TCAP_COMP_RET_REULT_LAST:
      case TCAP_COMP_RET_RESULT:
         if (0 != lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes)
         {
            pdu[lOffset++] = 0x30;
            pdu[lOffset++] =
               lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes + 3;

            pdu[lOffset++] = SS7_LOCAL_OP_CODE_TAG;
            pdu[lOffset++] = 1;
            pdu[lOffset++] =
               lSccpInfo.udt.compInfo.retResult.operation.operationCode;

            memcpy (&pdu[lOffset],
                  lSccpInfo.udt.compInfo.invoke.paramData.array,
                  lSccpInfo.udt.compInfo.invoke.paramData.
                  numberOfBytes);

            lOffset +=
               lSccpInfo.udt.compInfo.invoke.paramData.numberOfBytes;
         }
         break;
      case TCAP_COMP_RET_ERR:
         T (gTrace, printf ("\n      ErrCode        :%d\n",
                  lSccpInfo.udt.compInfo.retErr.errorCode.
                  errorCodeData.array[0]););

         pdu[lOffset++] = SS7_LOCAL_ERR_CODE_TAG;
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
   if (NULL == address)
   {
      printf(" %sAddress-----------:Absent\n",lText);
      return;
   }

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
}

