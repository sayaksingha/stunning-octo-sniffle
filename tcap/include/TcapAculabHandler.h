// @(#) $Id: TcapAculabHandler.h,v 1.1.4.8.2.1 2021/12/01 12:43:26 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME : TcapAculabHandler.h 
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

#ifndef TCAP_ACULAB_HDLR_H
#define TCAP_ACULAB_HDLR_H
#include "TcapAculabDlgMgr.h"
#include "TcapAculabTransDlgMap.h"
#include "TcapAculabApi.h"

struct TcapMsgQ
{
   key_t    RdMsgQKey;  //Tcap Handler Q key
   key_t    WrMsgQKey;  //Tcap Decoder Q key
   key_t    RdHbQKey;   //Tcap Heart-beat Q key
   MsgQueue RdQ;        //Tcap Handler Q
   MsgQueue WrQ;        //Tcap decoder Q
   MsgQueue HbQ;        //Tcap Heart-beat Q
};

   
class TcapAculabHandler
{
   TcapMsg  mRxTcapMsg;
   TcapMsg  mTxTcapMsg;

   struct timeval mStartTime;
   struct timeval mEndTime;

   acu_tcap_msg_t    *mTxAcuTcapMsg;
   acu_tcap_msg_t    *mRxAcuTcapMsg;
   
   acu_tcap_thrd_pool_t *mThrdPoolPtr;

   TcapAculab        mAculabApi;
   TransDlgMap       mTransDlgMap;

   INT16             mMsgType;
   INT16             mTransRange;
   int             mNoOfInstance;
   int             mTransValidationKey;
   int               mSetRcvLocalAddress;

   //Log               mLog;
   //CTrace            mTrace;
   //TEXT              mProcessName[50 + 1];
   TcapMsgQ         mMsgQ;
   AculabUtil       mAcuTcapUtil; 
   BOOLEAN          mPegFlag;
   TcapMsgDisplayApi mDispApi;
   INT16        mDisplayParam;
   BOOLEAN      mSetLocAddrFlag;
   BOOLEAN      mAppGtRelayFlag;
   BOOLEAN      mRestorationFlag;

   long         mMsgLimitCount;
   int          mLicPassAscii;
   TEXT         mLicKey[TCAP_ACU_MAX_LIC_LEN + 1];
   BOOLEAN      mSndTimeOutOnPreEnd;
   UINT8        mNumofOPCs;
   UINT32       mMaxDlgSize;
   DlgMgr       mDlgManager;
   UINT32       mDlgTimeout;
   UINT32       mCapDlgTimeout;
   public:

   TcapAculabHandler();
   TcapAculabHandler(TEXT *lProcName, TEXT *lTraceEnv);

   ~TcapAculabHandler();

   BOOLEAN CreateMsgQ(); 

   void PrintApplTcapStruct(TcapMsg &lTcapMsg,TEXT *lText);

   BOOLEAN Init(TEXT *lCfgFile, int lSsn,int& lNoOfInstance);

   BOOLEAN ReadConfig(int&);

   BOOLEAN ReloadConfig(int&);

   BOOLEAN ReadIpcConfig();
   BOOLEAN ReadKernelConfig(int &);
   UINT16  ReadTcapConfig(TEXT*);

   BOOLEAN DecryptLicKey();

   BOOLEAN GetSsapStatus(const int lInstanceNo);
   BOOLEAN UpdateSsapStatus(const int lInstanceNo);
   BOOLEAN ReconnectSsap(int &lInstanceNo);

   BOOLEAN TxMsgToApplication(TcapMsg &lTcapMsg);

   //BOOLEAN TxMsgToStack(acu_tcap_msg_t *lMsgPtr, EnumDelFlag lDelFlag);
   BOOLEAN TxMsgToStack(acu_tcap_msg_t *lMsgPtr);

   BOOLEAN RxMsgFromStack(acu_tcap_msg_t **lMsgPtr, int &lInstanceNo);

   BOOLEAN RxMsgFromApplication(TcapMsg &lRxTcapMsg);
   BOOLEAN WriteBackToTcapHdlrQueue(TcapMsg &lRxTcapMsg); //Added for transmit block/flow

   BOOLEAN ProcessTxMsgToStack(TcapMsg &lTcapMsg,acu_tcap_msg_t *lMsgPtr);

   BOOLEAN ProcessRxMsgFromStack(acu_tcap_msg_t *lMsgPtr,TcapMsg &lTcapMsg, int &lInstanceId);

   UINT16 GetTotalInstanceNo();
   int    GetTransValidationKey();
   BOOLEAN GetDlgInfoAndRestore();

};

struct  AcuThreadStruct
{
    TcapAculabHandler    *pTcapAculabHandler;
    acu_tcap_msg_t       *pMsg;
    acu_tcap_thrd_pool_t *p_tpool;
    int                  instanceNo;
};

#endif
