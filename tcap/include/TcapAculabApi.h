// @(#) $Id: TcapAculabApi.h,v 1.1.4.8.2.1 2021/12/01 12:43:26 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME:TcapAculabApi.h 
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
#ifndef TCAP_ACULAB_H
#define TCAP_ACULAB_H

#include "PegApi.h"
#include "Ss7ConstDef.h"
#include "Ss7Structs.h"
#include "TcapStructs.h"
#include "TcapAculabUtil.h"
#include "map"
#include "TcapAculabDlgMgr.h"
#include "TcapAculabTransDlgMap.h"

extern PegApi gPeg;

using namespace std;

typedef struct _DialogueComp
{
   UINT8          numberOfComponent;
   UINT8          totComponent;
   BOOLEAN        lastComponentFlag;
   AnsiTcapComponent  tcapComponent[ACU_TCAP_MAX_COMPONENT];
}DialogueComp;

typedef enum _SsapState
{
   CONNECTING,
   IN_SERVICE,
   EXITING
}SsapState;

typedef struct _SsapStatus 
{
   SsapState            state;
   UINT16               transIdRange;
   time_t               lastActTime;
   acu_tcap_cs_state_t  host_a_con_state;
   acu_tcap_cs_state_t  host_b_con_state;
}SsapStatus;

typedef enum _EnumSsapInstanceStatus
{
   ADDED_NEW=1,
   CREATE,
   CONNECT,
   ACTIVE,
   DELETE
}EnumSsapInstanceStatus;

typedef struct _SsapInstance
{
   UINT8       instanceId;
   EnumSsapInstanceStatus status;
   pthread_t   rxThreadId;
   pthread_t   txThreadId;
}SsapInstance;

typedef struct _SsapDetails
{
   TEXT           configFile[50];
   UINT16         transId;
   UINT8          numOfInstances;
   SsapInstance   instance[MAX_INSTANCE_PER_PC];
   UINT8          lastUsedInstance;

   _SsapDetails()
   {
      lastUsedInstance = 0;
   }
}SsapDetails;

//mapping pointcode and it owns details
typedef map<UINT32, SsapDetails>  SsapDetailsStruct;
typedef map<UINT32, SsapDetails>::iterator  SsapDetailsStructIterator;

class TcapAculab
{
   acu_tcap_ssap_t   *mSsapPtr[MAX_ACU_TCAP_INSTANCES];
   
   AculabUtil        mAcuTcapUtil;

   unsigned int      mApplTimeOutVal;
   unsigned int      mMinThread;
   unsigned int      mMaxThread;
   unsigned int      mMaxQueuedJobs;

   acu_sccp_addr_t  *mRemAddr;
   acu_sccp_addr_t  *mLocAddr;

   const acu_tcap_con_state_t *pConA_State;
   const acu_tcap_con_state_t *pConB_State;

   TEXT           mCfgFile[100 + 1];
   TEXT           mHostAIP[20 + 1];
   TEXT           mHostBIP[20 + 1];
   int            mNoOfInstance;

   BOOLEAN        mPegFlag;
   BOOLEAN        mHostAUP;
   BOOLEAN        mHostBUP;

   SsapStatus     mSaapStatus[MAX_ACU_TCAP_INSTANCES];  

   map<UINT32, DialogueComp>  mDialigueComponent;
   DlgMgr       mDlgManager;
   TransDlgMap  mTransDlgMap;

   public:

   TcapAculab();
     
     ~TcapAculab();
     
     BOOLEAN Init(TEXT *pCfgFile,BOOLEAN lPegFlag, int lInstanceNo, int mTransValidationKey, INT16 mMsgType);
  
     BOOLEAN SetTcapCinfigFilePath(TEXT *lCfgFile);

     // Ab Change: Pass lCfgFile down to ReadConfig
   BOOLEAN ReadConfig(TEXT* lCfgFile);

     BOOLEAN ReloadConfig();

     BOOLEAN ReadIpcConfig();

     // Ab Change: Pass lCfgFile down to ReadTcapConfig
     BOOLEAN ReadTcapConfig(TEXT* lCfgFile);

     BOOLEAN SsapCreate(BOOLEAN reload = false); 

     BOOLEAN SsapConnect(UINT8 instance);
   
     //BOOLEAN SsapReConnect(int &lInstanceNo);
     BOOLEAN SsapReConnect(int &lInstanceNo, INT16 &rMsgType, UINT32 &rDlgTimeout, UINT32 &rCapDlgTimeout, UINT32 &rMaxDlgSize);

     BOOLEAN UpdateSsapStatus(const int lInstanceNo);

     BOOLEAN GetSsapStatus(const int lInstanceNo);

     BOOLEAN GetAcuTcapConState(acu_tcap_msg_t *lMsg,int &lInstanceNo); 
      
     BOOLEAN GetAcuSccpConState(acu_tcap_msg_t *lMsg);

     BOOLEAN GetAcuTcapEvent(acu_tcap_msg_t **lAcuTcapMsgPtr,int &lInstanceNo);

     BOOLEAN SsapCreateThreadPool();

     BOOLEAN WaitForCoonection();
      
     EnumTcapComp GetTcapCompType(const acu_tcap_comp_type_t &lComp);

     BOOLEAN FillTcapStruct(acu_tcap_msg_t *lMsg,AnsiTcapMsg &lTcapMsg,
                            const acu_tcap_dialogue_t *lDlg,
                            AnsiTcapComponent *lComp,int &lNoOfComp);


      acu_tcap_msg_type_t HandleEventAcuTcapMsg(acu_tcap_msg_t *lAcuTcapMsgPtr);
      BOOLEAN SendAcuTcapMsg(acu_tcap_msg_t*); 
     BOOLEAN TrnslateMsgToAculabFormat(AnsiTcapMsg lTcapMsg, acu_tcap_msg_t *lMsg);
     acu_tcap_msg_type_t GetAcuTcapMsgType(AnsiTcapMsg &lTcapMsg);

     BOOLEAN CreateAcuTcapTrans(acu_tcap_trans_t **lTrsnPtr,int &lInstanceNo);

     BOOLEAN CreateAcuTcapOngoingTrans(acu_tcap_trans_t **lTrsnPtr, int &lInstanceNo, UINT8 origTransIdLen, UINT32 origTransId, UINT8 destTransIdLen, UINT32 destTransId, int ssn, UINT8 opCode, int invokeId, time_t timeOut, int ssapInstance);

     BOOLEAN AcuTcapOperationRestore(AnsiTcapMsg lTcapMsg, acu_tcap_msg_t *lMsg);

     BOOLEAN AllocAcuTcapMsg(acu_tcap_msg_t **lMsg,acu_tcap_trans_t *lTrsnPtr);

     BOOLEAN InitAcuTcapMsg(acu_tcap_msg_t *lMsg,acu_tcap_msg_type_t lType);

     BOOLEAN AddAcuTcapComponet(AnsiTcapMsg lTcapMsg,acu_tcap_msg_t *lMsg);

      
     BOOLEAN  GetTcapComponets(acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg,
           const acu_tcap_dialogue_t *lDlg, AnsiTcapComponent *lcomp,int &lNoOfComp);
      
     BOOLEAN  GetTcapTimeOutComponet(acu_tcap_msg_t *lMsg, AnsiTcapMsg &lTcapMsg);

     BOOLEAN FormTcapAbortMsg(AnsiTcapMsg &lTcapMsg,EnumAbortType lType,
                                    acu_tcap_p_abort_cause_t lCause);


     BOOLEAN AddAcuTcapDialogue(acu_tcap_msg_t *lMsg,AnsiTcapMsg lTcapMsg);

     EnumTcapDlg GetTcapDlgType(acu_tcap_msg *lMsg);

     BOOLEAN GetTcapDialogue(const acu_tcap_dialogue_t *lDlg, AnsiTcapMsg &lTcapMsg);

     BOOLEAN FillTcapComponent(acu_tcap_component_t*, AnsiTcapMsg&);

     BOOLEAN GetTransIds(acu_tcap_msg_t *lMsg,unsigned int *lLocId,
                                              unsigned int *lRemId,
                                              unsigned int *lRemIdLen);

     BOOLEAN GetTcapAddress(TCAPAddress &lAddress, EnumAddrFlag lFlag, acu_tcap_msg_t *lMsg);

  
     BOOLEAN SetLocTcapAddress(acu_tcap_msg_t *lMsg,
     TCAPAddress &lAddress);

     BOOLEAN SetRemTcapAddress(acu_tcap_msg_t *lMsg,
            TCAPAddress &lAddress);
      BOOLEAN SetAddress(acu_sccp_addr_t *lAddr,
         TCAPAddress &lAddress);

   BOOLEAN SetLocSccpAddr(TCAPAddress &lTcapAddress,int &lInstanceNo);
   BOOLEAN SetRemSccpAddr(TCAPAddress &lTcapAddress,int &lInstanceNo);
	BOOLEAN SetLocTcapAddress(acu_tcap_msg_t *lMsg, acu_sccp_addr_t **lAddr);
	BOOLEAN SetRemTcapAddress(acu_tcap_msg_t *lMsg, acu_sccp_addr_t **lAddr);

   BOOLEAN ShowAddress(acu_tcap_msg_t *lMsg);
   BOOLEAN SetTcapProblem(TCAPProblem &lProblem, unsigned int lOpCodeVal); 
   
    BOOLEAN GetAcuRejectProblem(acu_tcap_reject_problem_t &lProb,
                                                   TCAPProblem &lProblem);
   
    void DeleteAcuTcapTrans(acu_tcap_trans_t *pTrans);
    
    void FreeAcuTcapMsg(acu_tcap_msg_t *pMsg);
   
    BOOLEAN GetErrReportSccp(acu_tcap_msg_t *lMsg);

    void SetRcvLocalAddress(acu_tcap_msg_t *lMsg);

    BOOLEAN HandlerMultipleComponents(AnsiTcapMsg  tcapMsg);

};

#endif
