// @(#) $Id: SccpAculabHandler.h,v 1.1.4.1 2016/07/27 23:04:49 jamesjac Exp $
//--------------------------------------------------------------------------
// NAME : SccpAculabHandler.h 
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
// Originator : Keerthini AB                                      25-Aug-2014                                     
//----------------------------------------------------------------------------

#ifndef SCCP_ACULAB_HDLR_H
#define SCCP_ACULAB_HDLR_H
#include "SccpAculabApi.h"
#include "MsuStructs.h"

struct SccpMsgQ
{
   key_t    RdMsgQKey;  //Sccp Handler Q key
   key_t    WrMsgQKey;  //Sccp Decoder Q key
   key_t    RdHbQKey;   //Sccp Heart-beat Q key
   MsgQueue RdQ;        //Sccp Handler Q
   MsgQueue WrQ;        //Sccp decoder Q
   MsgQueue HbQ;        //Sccp Heart-beat Q
};

class SccpAculabHandler
{
   _SccpInfo           mSccpInfo;

   SccpAculab          mAculabApi;

   INT16               mMsgType;
   int                 mNoOfInstance;
   int                 mTransValidationKey;

   SccpMsgQ            mMsgQ;
   AculabUtil          mAcuSccpUtil; 
   BOOLEAN             mPegFlag;
   INT16               mDisplayParam;

   long                mMsgLimitCount;
   int                 mLicPassAscii;
   TEXT                mLicKey[SCCP_ACU_MAX_LIC_LEN + 1];
   UINT8               mNumofOPCs;
   
   INT16               mDestinationA;
   INT16               mDestinationB;

   BOOLEAN             mDestAStatus;
   BOOLEAN             mDestBStatus;
   BOOLEAN             mPcFlag;
   BOOLEAN             mIndLenFlag;

   public:
   SccpAculabHandler();

   SccpAculabHandler(TEXT *lProcName, TEXT *lTraceEnv);

   ~SccpAculabHandler();

   BOOLEAN CreateMsgQ(); 

   BOOLEAN Init(TEXT *lCfgFile, int lSsn);

   BOOLEAN ReadConfig();

   BOOLEAN ReloadConfig();

   BOOLEAN ReadIpcConfig();

   BOOLEAN ReadKernelConfig();

   BOOLEAN DecryptLicKey();

   BOOLEAN GetSsapStatus();

   BOOLEAN UpdateSsapStatus();

   BOOLEAN ReconnectSsap();

   BOOLEAN TxMsgToApplication(_SccpInfo &lTxSccpMsg);

   BOOLEAN TxMsgToStack(UINT8 data[300], UINT16 dataLen, UINT32 transId);

   BOOLEAN RxMsgFromStack(acu_sccp_msg_t **lMsgPtr);

   BOOLEAN RxMsgFromApplication(_SccpInfo &lRxSccpMsg);

   BOOLEAN ProcessTxMsgToStack(_SccpInfo &lSccpMsg);

   BOOLEAN DecodeComponent (UINT8 * pdu, UINT8 pduLen);

   BOOLEAN DecodeUnitData (acu_sccp_msg_t **lMsgPtr);

   BOOLEAN EncodeSccpUnitData(_SccpInfo &lSccpMsg, UINT8 lData[], UINT16& lDataLen);

   BOOLEAN EncodeAddress(acu_sccp_addr_t &lAddr, TCAPAddress &lAddress);

   UINT8 EncodeComponent (UINT8 * pdu, _SccpInfo &lSccpInfo);

   UINT8 EncodeTransId (UINT8 transIdLen, UINT32 transId, UINT8 * pdu);

   UINT16  ReadSccpConfig(TEXT*);

   int    GetTransValidationKey();

   void PrintApplSccpStruct(_SccpInfo SccpMsg, TEXT *lText);

   void DisplayAddress (const TCAPAddress * address, TEXT *lText);
};

#endif
