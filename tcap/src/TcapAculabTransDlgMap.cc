// @(#) $Id: TcapAculabTransDlgMap.cc,v 1.1.4.1.16.1.2.1 2022/04/28 09:46:16 mram Exp $
//----------------------------------------------------------------------------
// NAME: TcapAculabTransDlgMap.cc 
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
#include "TcapAculabTransDlgMap.h"
TransDlgMap::TransDlgMap()
{
   //DlgMgr tempDlgMgr(gProcessName, TRACE_ACU_TCAP_HDLR_ENV);
   //mDlgMgr = tempDlgMgr;
}
//-------------------------------------------------------------------------------
// METHOD      : ~TransDlgMap 
// DESCRIPTION : Destructor 
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------

TransDlgMap::~TransDlgMap()
{
}
//-------------------------------------------------------------------------------
// METHOD      : Init 
// DESCRIPTION : Initialization  
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
// Ab Change: Pass lCfgFile so it can be passed down to DlgMgr
BOOLEAN TransDlgMap::Init(int lTransValidationKey, int ssn, TEXT* lCfgFile)
{
   mTransValidationKey = lTransValidationKey;
   mSsn = ssn;
   pthread_mutex_init(&mDlgLock, NULL);
   pthread_mutex_init(&mTransLock, NULL);

   // Ab Change: Pass lCfgFile to DlgMgr
   return mDlgMgr.Init(lCfgFile);

}
//-------------------------------------------------------------------------------
// METHOD      : ReadConfig 
// DESCRIPTION : Reading configuration 
// PARAMETER   : none
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::ReadConfig()
{
   return true;
}
//-------------------------------------------------------------------------------
// METHOD      : MapTransIdToDlgId 
// DESCRIPTION : Stores aculab transation pointer address
//               for corresponding allocated dialogue id
// PARAMETER   : acu_tcap_trans_t *, int 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::MapTransIdToDlgId(acu_tcap_trans_t *lTrans,unsigned int &lDlgId)
{
   //Function to allocate dialogue ID for RX Message from network
   //Get free dialogue Id from Dlg Manager
   //map it with transaction id
   DlgRecord lDlgRecord;
   lDlgRecord.trans = lTrans;
   lDlgRecord.transValidationKey = mTransValidationKey;
   lDlgRecord.ssn = mSsn;
   DlgRecord *pDlgRecord;

   pDlgRecord = mDlgMgr.Allocate_DlgId(lDlgRecord);
   if(NULL == pDlgRecord)
   {
      TERR(gTrace,printf("Allocate_DlgId failed ...\n"););
      return false;
   }
   else
   {
      lDlgId = pDlgRecord->dlgId;
      acu_tcap_trans_set_userptr(lDlgRecord.trans,pDlgRecord);
      return true;
   }

}

//-------------------------------------------------------------------------------
// METHOD      : MapTransIdToDlgId 
// DESCRIPTION : Stores aculab transation pointer address
//               for corresponding allocated dialogue id
// PARAMETER   : acu_tcap_trans_t *, int 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::MapTransIdToDlgId(acu_tcap_trans_t *lTrans, DlgRecord &lDlgRecord)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //Function to allocate dialogue ID for RX Message from network
   //Get free dialogue Id from Dlg Manager
   //map it with transaction id
   //DlgRecord lDlgRecord;
   lDlgRecord.trans = lTrans;
   lDlgRecord.transValidationKey = mTransValidationKey;
   lDlgRecord.ssn = mSsn;
   DlgRecord *pDlgRecord;

   pDlgRecord = mDlgMgr.Allocate_DlgId(lDlgRecord);
   if(NULL == pDlgRecord)
   {
      //TERR(gTrace,printf("Allocate_DlgId failed ...\n"););
      sprintf(lLogText,"TR: DlgId:%d MapTransIdToDlgId Failed", lDlgRecord.dlgId);
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP92, lLogText);
      return false;
   }
   else
   {
      lDlgRecord.dlgId = pDlgRecord->dlgId;
      acu_tcap_trans_set_userptr(lDlgRecord.trans,pDlgRecord);

      sprintf(lLogText,"TR: DlgId:%d TrandPtr:%08X LocalTid:%08X DestTid:%08X MapTransIdToDlgId Success", 
            pDlgRecord->dlgId, lTrans, pDlgRecord->origTransId, pDlgRecord->destTransId);
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP93,2, lLogText);
      return true;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : MapDlgIdtoTransId 
// DESCRIPTION : Stores dialogue record corresponding 
//               to aculab local transaction id
// PARAMETER   : DlgRecord, unsigned int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::MapDlgIdtoTransId(DlgRecord &lDlgRecord)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   DlgRecord *pDlgRecord;
   //printf("\n lDlgRecord.dlgId:%d\n", lDlgRecord.dlgId);
   pDlgRecord = mDlgMgr.UpdateDlgInfo(lDlgRecord.dlgId,lDlgRecord);

   if(pDlgRecord != NULL)
   { 
      acu_tcap_trans_set_userptr(lDlgRecord.trans,pDlgRecord);
      sprintf(lLogText,"TR: DlgId:%d TrandPtr:%08X LocalTid:%08X DestTid:%08X MapDlgIdtoTransId Success",
            pDlgRecord->dlgId, pDlgRecord->trans, pDlgRecord->origTransId, pDlgRecord->destTransId);
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP94,2, lLogText);
      return true;
   }
   else
   {
      sprintf(lLogText,"TR: DlgId:%d MapDlgIdtoTransId Failure", lDlgRecord.dlgId);
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP95, lLogText);
      return false;
   }

}
//-------------------------------------------------------------------------------
// METHOD      : GetTransInfo 
// DESCRIPTION : Retrive aculab transaction pointer info 
//               corresponding to dialouge id
// PARAMETER   : UINT32, acu_tcap_trans_t ** 
// RETURN      : BOOLEAN
// THROWS      : none
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::GetTransInfo(DlgRecord &lDlgRecord, acu_tcap_trans_t **lTrans)
{
   //DlgRecord lDlgRecord;
   if(false == mDlgMgr.GetDlgInfo(lDlgRecord.dlgId, lDlgRecord))
   {
      return false;
   }
   else
   {
      *lTrans = lDlgRecord.trans;
      return true;
   }
}

//-------------------------------------------------------------------------------
// METHOD      : GetDialogueInfo 
// DESCRIPTION : Retrive dialogue record corresponding to 
//               aculab local transaction id
// PARAMETER   : unsigned int, DlgRecord
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::GetDialogueInfo(acu_tcap_trans_t *lTrans, DlgRecord &lDlgRecord)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   DlgRecord *pDlgRecord = (DlgRecord *) acu_tcap_trans_get_userptr(lTrans);
   if(NULL != pDlgRecord)
   {
      memcpy(&lDlgRecord, pDlgRecord, sizeof(DlgRecord));

      sprintf(lLogText,"TR: DlgId:%d TrandPtr:%08X LocalTid:%08X DestTid:%08X GetDialogueInfo Success",
            pDlgRecord->dlgId, pDlgRecord->trans, pDlgRecord->origTransId, pDlgRecord->destTransId);
      T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
      gLog.GenerateLog(ACUTCAP103,2, lLogText);
      return true;
   }

   sprintf(lLogText,"GetDialogueInfo Failure");
   T(gTrace, printf("%s: %s\n",gProcessName,lLogText););
   gLog.GenerateLog(ACUTCAP104, lLogText);

   return false;
}

//-------------------------------------------------------------------------------
// METHOD      : DeleteTransInfo 
// DESCRIPTION : Delete dialogue record corresponding to 
//               aculab local transaction id
// PARAMETER   : unsigned int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::DeleteTransInfo(unsigned int lTransId)
{
   pthread_mutex_lock(&mDlgLock);
   mapDlgRecord.erase(lTransId);
   pthread_mutex_unlock(&mDlgLock);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : DeleteDlgInfo 
// DESCRIPTION : Delete aculab transation info corresponding to 
//               dialouge id
// PARAMETER   : int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN TransDlgMap::DeleteDlgInfo(unsigned int dlgId)
{
   return mDlgMgr.FreeDlgInfo(dlgId);
}
//-------------------------------------------------------------------------------
// METHOD      : GetHalfDlgSize 
// DESCRIPTION : Delete aculab transation info corresponding to
//               dialouge id
// PARAMETER   : int
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
unsigned int TransDlgMap::GetHalfDlgSize()
{
   return mDlgMgr.GetHalfDlgSize();
}
