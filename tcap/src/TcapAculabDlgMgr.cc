// @(#) $Id: TcapAculabDlgMgr.cc,v 1.1.4.4.2.2 2021/12/01 12:43:26 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME : TcapAculabDlgMgr.cc 
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
#include "TcapAculabDlgMgr.h"

//-------------------------------------------------------------------------------
// METHOD      : DlgMgr()
// DESCRIPTION : Default Constructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
DlgMgr::DlgMgr ()
{
   mDlgMgmtQueue = NULLPTR;
}

//-------------------------------------------------------------------------------
// METHOD      : ~DlgMgr()
// DESCRIPTION : Default Destructor
// PARAMETER   : none
// RETURN      : none
//-------------------------------------------------------------------------------
DlgMgr::~DlgMgr ()
{

}

//-------------------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : Reading configuration parameters
// PARAMETER   : none
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::ReadConfig ()
{
   CfgRead lCfgReadObj ("DLG_MANAGER");
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgReadObj.CfgInit (SS7_KER_CFG))
   {
      printf ("%s:\33[00;31m CfgRead object intialization failed.\33[0m\n",
            gProcessName);
      gLog.GenerateLog (CFG01, "DM: CfgRead object initializaton failed");
      return false;
   }

   UINT16 lInDlgStartIdx;
   if (CFG_OK != lCfgReadObj.GetConfigNum ("ACU_TCAP_IN_DLG_SHIFT_INDX",
            lInDlgStartIdx, MIN_ACU_TCAP_SHIFT_DLG, MAX_ACU_TCAP_DLG))
   {
      sprintf (lLogText,
            "DM: Configuration read Err for ACU_TCAP_IN_DLG_SHIFT_INDX in file kernel.cfg");
      printf ("%s:\33[00;31m %s \33[0m\n", gProcessName, lLogText);
      gLog.GenerateLog (CFG02, lLogText);
      lInDlgStartIdx = 0;
   }
   T (gTrace, printf ("%s: ACU_TCAP_IN_DLG_SHIFT_INDX = %d\n", gProcessName, lInDlgStartIdx););

   if (CFG_OK != lCfgReadObj.GetConfigNum ("MAX_ACU_TCAP_DLG_SIZE",
            mMaxDlgSize, MIN_ACU_TCAP_DLG, MAX_ACU_TCAP_DLG))
   {
      sprintf (lLogText,
            "DM: Configuration read Err for MAX_ACU_TCAP_DLG_SIZE in file kernel.cfg");
      printf ("%s:\33[00;31m %s \33[0m\n", gProcessName, lLogText);
      gLog.GenerateLog (CFG02, lLogText);
      lCfgReadObj.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: MAX_ACU_TCAP_DLG_SIZE = %d\n", gProcessName, mMaxDlgSize););

   lCfgReadObj.CfgDeInit ();

   mHalfMaxDlgSize = mMaxDlgSize / 2 + lInDlgStartIdx;
   mMaxAllocateDlgSize = mMaxDlgSize / 2 - lInDlgStartIdx;

   // Reading parameter fromn ipc.cfg
   if (false == lCfgReadObj.CfgInit (SS7_IPC_CFG))
   {
      printf ("%s: \33[00;31m CfgRead object intialization failed.\33[0m\n",
            gProcessName);
      gLog.GenerateLog (CFG01, "DM: CfgRead object initializaton failed");
      return false;
   }

   if (CFG_OK != lCfgReadObj.GetConfigNum ("SEM_IN_DLG_KEY",
            mInDlgSemKey, SEM_MIN, SEM_MAX))
   {
      sprintf (lLogText,
            "DM: Configuration read Err for SEM_IN_DLG_KEY in file ipc.cfg, exiting ...");
      printf ("%s: \33[00;31m %s \33[0m \n", gProcessName, lLogText);
      gLog.GenerateLog (CFG02, lLogText);
      lCfgReadObj.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: SEM_IN_DLG_KEY = %d\n", gProcessName, mInDlgSemKey););

   if (CFG_OK != lCfgReadObj.GetConfigNum ("SHM_IN_DLG_POOL_KEY",
            mInDlgShmKey, SHM_MIN, SHM_MAX))
   {
      sprintf (lLogText,
            "DM: Configuration read Err for SHM_IN_DLG_POOL_KEY in file ipc.cfg, exiting ...");
      printf ("%s:\33[00;31m %s \33[0m\n", gProcessName, lLogText);
      gLog.GenerateLog (CFG02, lLogText);
      lCfgReadObj.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: SHM_IN_DLG_POOL_KEY =  %d\n", gProcessName, mInDlgShmKey););

   if (CFG_OK != lCfgReadObj.GetConfigNum ("SHM_DLG_MGMT_QUEUE_KEY",
            mDlgMgmtQKey, SHM_MIN, SHM_MAX))
   {
      sprintf (lLogText,
            "DM: Configuration read Err for SHM_DLG_MGMT_QUEUE_KEY in file ipc.cfg, exiting ...");
      printf ("%s:\33[00;31m %s \33[0m\n", gProcessName, lLogText);
      gLog.GenerateLog (CFG02, lLogText);
      lCfgReadObj.CfgDeInit ();
      return false;
   }
   T (gTrace, printf ("%s: SHM_DLG_MGMT_QUEUE_KEY  =  %d\n", gProcessName, mDlgMgmtQKey););

   lCfgReadObj.CfgDeInit ();

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Init Dlg Manager library 
// PARAMETER   : none
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::Init ()
{
   UINT32 lMemSize;
   int lShmIdInDlgRecordPool;
   int lShmIdDlgMgmtQueue;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   pthread_mutex_init (&mThreadLock, NULL);

   if (gLog.Init () == false)
   {
      printf ("%s:\33[00;31m DM: In Init() Could not initialize Log Object DLG_INIT_ERR\33[0m\n", gProcessName);
      return false;
   }

   if (false == this->ReadConfig ())
   {
      sprintf (lLogText, "DM: DlgMgr ReadConfig Failed");
      printf ("%s:\33[00;31m %s \33[0m\n", gProcessName, lLogText);
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   mDlgMgmtQueue = new DlgMgmtQueue;
   if (mDlgMgmtQueue == NULLPTR)
   {
      printf ("%s: \33[00;31m DM: In Init() Could not create memory for mDlgMgmtQueue header\33[0m\n", gProcessName);
      return false;
   }

   lMemSize = sizeof (DlgRecord) * (mMaxDlgSize + 1);

   lShmIdInDlgRecordPool = shmget (mInDlgShmKey, lMemSize, IPC_CREAT | IPC_EXCL | SS7_IPC_PERM);
   if (lShmIdInDlgRecordPool < 0)
   {
      lShmIdInDlgRecordPool = shmget (mInDlgShmKey, lMemSize, IPC_CREAT | SS7_IPC_PERM);
      if (lShmIdInDlgRecordPool < 0)
      {
         sprintf (lLogText,
               "DM: DlgManager::Init failed, Could not access InDlgRecordPool Key:%d",
               mInDlgShmKey);
         gLog.GenerateLog (GSYS06, mLogText);
         printf ("%s:\33[00;31m %s\33[0m\n", gProcessName, lLogText);
         return false;
      }

      // initializing mDlgRecordPool pointers
      mInDlgRecordPool = (DlgRecord *) shmat (lShmIdInDlgRecordPool, 0, 0);
      T (gTrace,
            printf ("%s: DM: In Init() InDlgRecordPool pointers linked successfully\n",
               gProcessName););
   }
   else
   {
      // initializing mDlgRecordPool pointers
      mInDlgRecordPool = (DlgRecord *) shmat (lShmIdInDlgRecordPool, 0, 0);
      memset (mInDlgRecordPool, 0, sizeof (DlgRecord) * (mMaxDlgSize + 1));
      T (gTrace,
            printf ("%s: DM: In Init() InDlgRecordPool Created successfully\n",
               gProcessName););
   }

   // size of DlgMqmtQueue
   lMemSize = sizeof (UINT32) * (mMaxDlgSize + 1) + sizeof (DlgMgmtQueueHeader);
   lShmIdDlgMgmtQueue = shmget (mDlgMgmtQKey, lMemSize, IPC_CREAT | IPC_EXCL | SS7_IPC_PERM);
   if (lShmIdDlgMgmtQueue < 0)
   {
      lShmIdDlgMgmtQueue = shmget (mDlgMgmtQKey, lMemSize, IPC_CREAT | SS7_IPC_PERM);
      if (lShmIdDlgMgmtQueue < 0)
      {
         sprintf (lLogText,
               "DM: DlgManager::Init failed, Could not access DlgMgmtQueue Key:%d",
               mDlgMgmtQKey);
         gLog.GenerateLog (GSYS06, mLogText);
         printf ("%s:\33[00;31m %s\33[0m\n", gProcessName, lLogText);
         return false;
      }
      // initializing DlgMgmtQueue pointer
      mDlgMgmtQueueHeader = (DlgMgmtQueueHeader *) shmat (lShmIdDlgMgmtQueue, 0, 0);
      mDlgMgmtQueue->HeaderPtr = mDlgMgmtQueueHeader;
      mDlgMgmtQueue->dlgIdx = (UINT32 *) (mDlgMgmtQueueHeader + 1);   // check
      //mDlgMgmtQueue->dlgIdx = (UINT32 *) (mDlgMgmtQueueHeader + 2000);
      T (gTrace,
            printf ("%s: DM: In Init() DlgMgmtQueue pointers linked successfully\n",
               gProcessName););
   }
   else
   {
      // initializing DlgMgmtQueue pointer
      mDlgMgmtQueueHeader = (DlgMgmtQueueHeader *) shmat (lShmIdDlgMgmtQueue, 0, 0);

      mDlgMgmtQueueHeader->nextFreeDlgIdx = 1;
      mDlgMgmtQueueHeader->lastFreeDlgIdx = 1;
      mDlgMgmtQueueHeader->recordCount = 0;

      mDlgMgmtQueue->HeaderPtr = mDlgMgmtQueueHeader;
      mDlgMgmtQueue->dlgIdx = (UINT32 *) (mDlgMgmtQueueHeader + 1);
      //mDlgMgmtQueue->dlgIdx = (UINT32 *) (mDlgMgmtQueueHeader + 2000);

      for (UINT32 lDlgIterator = 0; lDlgIterator <= mMaxAllocateDlgSize; lDlgIterator++)
      {
         mDlgMgmtQueue->dlgIdx[lDlgIterator] = lDlgIterator;
      }
   }

   mSemId = semget (mInDlgSemKey, NUM_OF_SEM, SS7_IPC_PERM | IPC_CREAT);
   if (-1 == mSemId)
   {
      printf ("%s:\33[00;31m DM: Semaphore Creation (Key:%d) - Failed.\33[0m\n",
            gProcessName, mInDlgSemKey);
      return false;
   }
   T (gTrace, printf ("%s: DM: Semaphore Creation (Key:%d) - Success.\n", gProcessName, mInDlgSemKey););

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : Allocate_DlgId
// DESCRIPTION : Allocates dlgId from Free pool(mDlgMgmtQueue)  
// PARAMETER   : DlgRecord
// RETURN      : DlgRecord * 
//-------------------------------------------------------------------------------
DlgRecord * DlgMgr::Allocate_DlgId (DlgRecord & dlgRecord)
{
   UINT32 lDlgIdx;
   DlgRecord *lDlgRecord;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   Lock (SEM_FOR_IN_DLG_POOL);

   if (mDlgMgmtQueue->dlgIdx[(mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx] == 0)
   {
      UINT32 lTempIndx = (mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx;

      if ((mDlgMgmtQueue->HeaderPtr)->recordCount == mMaxAllocateDlgSize)
      {
         sprintf (lLogText, "DM: DlgRecordPool full");
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog (ACUTCAP24, lLogText);

         UnLock (SEM_FOR_IN_DLG_POOL);
         return NULL;
      }

      UpdateDlgMgmtQueueIdx (&((mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx));
      while (mDlgMgmtQueue->dlgIdx[(mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx]
            == 0)
      {
         if (lTempIndx == (mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx)
         {
            sprintf (lLogText, "DM: DlgRecordPool full Inside While");
            TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog (ACUTCAP24, lLogText);
            UnLock (SEM_FOR_IN_DLG_POOL);
            return NULL;
         }
         UpdateDlgMgmtQueueIdx (& ((mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx));
      }
   }

   lDlgIdx = mDlgMgmtQueue->dlgIdx[(mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx];
   //lDlgIdx = lDlgIdx + (mMaxDlgSize / 2);

   lDlgIdx = lDlgIdx + mHalfMaxDlgSize;

   mDlgMgmtQueue->dlgIdx[(mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx] = 0;

   UpdateDlgMgmtQueueIdx (&((mDlgMgmtQueue->HeaderPtr)->nextFreeDlgIdx));

   mDlgMgmtQueue->HeaderPtr->recordCount++;


   lDlgRecord = (DlgRecord *) & (mInDlgRecordPool[lDlgIdx]);
   memcpy (lDlgRecord, &dlgRecord, sizeof (struct DlgRecord));
   lDlgRecord->dlgId = lDlgIdx;
   lDlgRecord->insertTime = time (NULL);

   UnLock (SEM_FOR_IN_DLG_POOL);

   sprintf (lLogText, "DM: DlgId:%d In Allocate_DlgId() Success ",
         lDlgRecord->dlgId);
   T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog (ACUTCAP70, 3, lLogText);

   return lDlgRecord;
}

//-------------------------------------------------------------------------------
// METHOD      : FreeDlgInfo 
// DESCRIPTION : Marks the dlgId as free and add to free dlg pool 
// PARAMETER   : const UINT32
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::FreeDlgInfo (const unsigned int &dlgIndex)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   Lock (SEM_FOR_IN_DLG_POOL);

   if (dlgIndex > mHalfMaxDlgSize && dlgIndex <= mMaxDlgSize)
   {
      if (mInDlgRecordPool[dlgIndex].dlgId == 0)
      {
         sprintf (lLogText,
               "DM: DlgId:%d In Free Dlg Info :Already Freed DlgId Zero",
               dlgIndex);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog (ACUTCAP73, lLogText);
         UnLock (SEM_FOR_IN_DLG_POOL);
         return false;
      }

      memset (&mInDlgRecordPool[dlgIndex], 0, sizeof (DlgRecord));

      mDlgMgmtQueue->dlgIdx[(mDlgMgmtQueue->HeaderPtr)->lastFreeDlgIdx] =
         dlgIndex - mHalfMaxDlgSize;

      UpdateDlgMgmtQueueIdx (&((mDlgMgmtQueue->HeaderPtr)->lastFreeDlgIdx));

      mDlgMgmtQueue->HeaderPtr->recordCount--;

      sprintf (lLogText,
            "DM: DlgId:%d In FreeDlgInfo() Success Outgoing Range",
            dlgIndex);
      T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP71, 3, lLogText);

      UnLock (SEM_FOR_IN_DLG_POOL);
      return true;
   }
   if (dlgIndex >= 1 && dlgIndex <= mHalfMaxDlgSize)
   {
      if (mInDlgRecordPool[dlgIndex].dlgId == 0)
      {
         sprintf (lLogText, "DM: DlgId:%d In Free Dlg Info, Already Freed",
               dlgIndex);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog (ACUTCAP74, lLogText);

         UnLock (SEM_FOR_IN_DLG_POOL);

         return false;
      }

      memset (&mInDlgRecordPool[dlgIndex], 0, sizeof (DlgRecord));

      sprintf (lLogText,
            "DM: DlgId:%d In FreeDlgInfo() Success Incoming Range",
            dlgIndex);
      T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP72, 3, lLogText);
      UnLock (SEM_FOR_IN_DLG_POOL);
      return true;
   }

   if (dlgIndex < 1 || dlgIndex > mMaxDlgSize)
   {

      sprintf (lLogText, "DM: DlgId:%d In Free Dlg Info :Invalid", dlgIndex);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP75, lLogText);
      UnLock (SEM_FOR_IN_DLG_POOL);
      return false;
   }

   if (mInDlgRecordPool[dlgIndex].dlgId == 0)
   {
      sprintf (lLogText, "DM: DlgId:%d In Free Dlg Info :Already Freed",
            dlgIndex);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP76, lLogText);
      UnLock (SEM_FOR_IN_DLG_POOL);
      return false;
   }

   UnLock (SEM_FOR_IN_DLG_POOL);

   return true;
}


//-------------------------------------------------------------------------------
// METHOD      : GetDlgInfo 
// DESCRIPTION : Get Dlg Information stored at the dlgId index in dlgPool  
// PARAMETER   : const int
// RETURN      : DlgRecord*
//-------------------------------------------------------------------------------
DlgRecord * DlgMgr::GetDlgInfo (const unsigned int dlgId)
{
   if (dlgId > mMaxDlgSize || dlgId < 1 || 0 == dlgId)
   {
      TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

      sprintf (lLogText, "DM: DlgId:%d In GetDlgInfo() Invalid dlgId", dlgId);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP08, lLogText);

      return NULL;
   }

   return &(mInDlgRecordPool[dlgId]);
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateMsgStatus
// DESCRIPTION : Get Dlg Information stored at the dlgId index in dlgPool  
// PARAMETER   : const int , DlgRecord &
// RETURN      : BOOLEAN
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::GetDlgInfo (const unsigned int &dlgId, DlgRecord & dlgRecord)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DlgRecord * lDlgRecord;
   lDlgRecord = NULLPTR;
   unsigned int lDlgId = 0;

   if (dlgId > mMaxDlgSize || dlgId < 1)
   {
      TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

      sprintf (lLogText, "DM: DlgId:%d In GetDlgInfo() Invalid dlgRecord",
            dlgId);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP09, lLogText);

      return false;
   }

   lDlgId = dlgId;

   lDlgRecord = &(mInDlgRecordPool[lDlgId]);

   if (lDlgRecord->dlgId == 0)
   {
      sprintf (lLogText, "DM: DlgId:%d In GetDlgInfo() Failed", dlgId);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (ACUTCAP131, lLogText);

      return false;
   }

   memcpy (&dlgRecord, lDlgRecord, sizeof (DlgRecord));

   sprintf (lLogText, "DM: DlgId:%d In GetDlgInfo() Success", dlgId);
   T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (ACUTCAP132, lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateDlgInfo 
// DESCRIPTION : Update Dlg Info for the givem dlgId in dlgPool  
// PARAMETER   : const unsigned int, DlgRecord
// RETURN      : DlgRecord *
//-------------------------------------------------------------------------------
DlgRecord * DlgMgr::UpdateDlgInfo (unsigned int &dlgId, DlgRecord & dlgRecord)
{
   unsigned int lDlgId;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //   if ( dlgId > mHalfMaxDlgSize)
   {
      //      TERR(gTrace, printf("%s: In UpdateDlgInfo() :Invalid dlg Id - %d\n", gProcessName, dlgId);)
      //      return NULL;
   }
   Lock (SEM_FOR_IN_DLG_POOL);

   lDlgId = dlgId;

   // if(dlgId >= 1 && dlgId <= mHalfMaxDlgSize)
   {
      memcpy (&(mInDlgRecordPool[lDlgId]), &dlgRecord, sizeof (DlgRecord));

      UnLock (SEM_FOR_IN_DLG_POOL);
   }

   //snprintf(lLogText, MAX_LOG_TEXT_LEN, "In UpdateDlgInfo() - Got Info For DlgId:%d StoredDlgId:%d TransPtr:%08X InvokeId:%d InsertTime:%ld Ssn:%d LocalTid:%08X LocalTidLen:%d DestTid:%08X DestTidLen:%d DlgType:%d Restarted:%d Opcode:%d SsapInstance:%d", dlgId, dlgRecord.dlgId, dlgRecord.trans, dlgRecord.invokeId, dlgRecord.insertTime, dlgRecord.ssn, dlgRecord.origTransId, dlgRecord.origTransIdLen, dlgRecord.destTransId, dlgRecord.destTransIdLen, dlgRecord.dlgType, dlgRecord.restarted, dlgRecord.opCode, dlgRecord.ssapInstance);
   sprintf (lLogText,
         "DM: DlgId:%d TransPtr:%08X lTid:%08X rTid:%08X StoredDlgId:%d LocalTidLen:%d DestTidLen:%d InsertTime:%ld Ssn:%d InvokeId:%d DlgType:%d SsapInstance:%d Restore Details",
         dlgId, dlgRecord.trans, dlgRecord.origTransId,
         dlgRecord.destTransId, dlgRecord.dlgId, dlgRecord.origTransIdLen,
         dlgRecord.destTransIdLen, dlgRecord.insertTime, dlgRecord.ssn,
         dlgRecord.invokeId, dlgRecord.dlgType, dlgRecord.ssapInstance);
   T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog (ACUTCAP156, 3, lLogText);

   return &(mInDlgRecordPool[lDlgId]);
}

//-------------------------------------------------------------------------------
// METHOD      : GetDlgPoolUsage 
// DESCRIPTION : return dlg pool usage in percentage
// PARAMETER   : none
// RETURN      : UINT16
//-------------------------------------------------------------------------------
UINT16 DlgMgr::GetDlgPoolUsage ()
{
   UINT16 lPercentage;

   lPercentage = (UINT16) ((mDlgMgmtQueue->HeaderPtr)->recordCount * 100 / mMaxAllocateDlgSize);

   return lPercentage;
}

//-------------------------------------------------------------------------------
// METHOD      : GetHalfDlgSize 
// DESCRIPTION : return half of dlg pool size
// PARAMETER   : none
// RETURN      : unsigned int
//-------------------------------------------------------------------------------
unsigned int DlgMgr::GetHalfDlgSize ()
{
   return mHalfMaxDlgSize;
}

//-------------------------------------------------------------------------------
// METHOD      : GetNumberOfFreeDlgRecords 
// DESCRIPTION : Returns back the number of free dlg Ids.
// PARAMETER   : none
// RETURN      : UINT32 
//-------------------------------------------------------------------------------
UINT32 DlgMgr::GetNumberOfFreeDlgRecords ()
{
   return (mMaxAllocateDlgSize - (mDlgMgmtQueue->HeaderPtr)->recordCount);
}

//-------------------------------------------------------------------------------
// METHOD      : GetInsertTime 
// DESCRIPTION : Updates Insert time of particular dlgId
//               with the insertTime(UINT32) reference
// PARAMETER   : UINT32, UINT32& 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::GetInsertTime (UINT32 index, time_t & insertTime)
{
   insertTime = mInDlgRecordPool[index].insertTime;
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetInsertTime 
// DESCRIPTION : Updates Insert time of particular dlgId
//               with the insertTime(UINT32) reference
// PARAMETER   : UINT32, UINT32& 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::GetInsertTimeAndSsn (UINT32 index, time_t & insertTime, int &lSsn)
{
   insertTime = mInDlgRecordPool[index].insertTime;
   lSsn = mInDlgRecordPool[index].ssn;
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : GetAllDlgInfo 
// DESCRIPTION : 
// PARAMETER   : UINT32, UINT32& 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::GetAllDlgInfo (DlgRecord & lDlgRcd)
{
   lDlgRcd.insertTime = mInDlgRecordPool[lDlgRcd.dlgId].insertTime;
   lDlgRcd.ssn = mInDlgRecordPool[lDlgRcd.dlgId].ssn;
   lDlgRcd.origTransId = mInDlgRecordPool[lDlgRcd.dlgId].origTransId;
   lDlgRcd.origTransIdLen = mInDlgRecordPool[lDlgRcd.dlgId].origTransIdLen;
   lDlgRcd.destTransId = mInDlgRecordPool[lDlgRcd.dlgId].destTransId;
   lDlgRcd.destTransIdLen = mInDlgRecordPool[lDlgRcd.dlgId].destTransIdLen;
   lDlgRcd.invokeId = mInDlgRecordPool[lDlgRcd.dlgId].invokeId;
   lDlgRcd.callingAddr = mInDlgRecordPool[lDlgRcd.dlgId].callingAddr;
   lDlgRcd.calledAddr = mInDlgRecordPool[lDlgRcd.dlgId].calledAddr;
   lDlgRcd.dlgType = mInDlgRecordPool[lDlgRcd.dlgId].dlgType;
   lDlgRcd.opCode = mInDlgRecordPool[lDlgRcd.dlgId].opCode;
   lDlgRcd.restarted = mInDlgRecordPool[lDlgRcd.dlgId].restarted;
   lDlgRcd.ssapInstance = mInDlgRecordPool[lDlgRcd.dlgId].ssapInstance;
   memcpy (&lDlgRcd.applicationContext,
         &mInDlgRecordPool[lDlgRcd.dlgId].applicationContext,
         sizeof (lDlgRcd.applicationContext));

   //lTrsnPtr    = mInDlgRecordPool[index].trans;
#if 0
   if (mInDlgRecordPool[index].callingAddr != NULL)
   {
      printf ("\n\n TBR ###########GetAllDlgInfo##################  \n\n");
      printf (" TBR: lDlgRcd.callingAddr->sa_gt.sag_num:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_gt.sag_num);
      printf (" TBR: lDlgRcd.callingAddr->sa_tt:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_tt);
      printf (" TBR: lDlgRcd.callingAddr->sa_np:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_np);
      printf (" TBR: lDlgRcd.callingAddr->sa_es.sag_num:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_es);
      printf (" TBR: lDlgRcd.callingAddr->sa_nai:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_nai);
   }
#endif

   return true;
}


//-------------------------------------------------------------------------------
// METHOD      : GetAllDlgInfo 
// DESCRIPTION : 
// PARAMETER   : UINT32, UINT32& 
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::GetAllDlgInfo (UINT32 index, time_t & insertTime, int &lSsn,
      int &localTid, int &localTidLen, int &destTid, int &destTidLen, int &invokeId,
      acu_sccp_addr_t & callingAddr, acu_sccp_addr_t & calledAddr,
      EnumTcapDlg & dlgType, UINT8 & opCode)
{
   insertTime = mInDlgRecordPool[index].insertTime;
   lSsn = mInDlgRecordPool[index].ssn;
   localTid = mInDlgRecordPool[index].origTransId;
   localTidLen = mInDlgRecordPool[index].origTransIdLen;
   destTid = mInDlgRecordPool[index].destTransId;
   destTidLen = mInDlgRecordPool[index].destTransIdLen;
   invokeId = mInDlgRecordPool[index].invokeId;
   callingAddr = mInDlgRecordPool[index].callingAddr;
   calledAddr = mInDlgRecordPool[index].calledAddr;
   dlgType = mInDlgRecordPool[index].dlgType;
   opCode = mInDlgRecordPool[index].opCode;
   //lTrsnPtr    = mInDlgRecordPool[index].trans;
#if 0
   if (mInDlgRecordPool[index].callingAddr != NULL)
   {
      printf ("\n\n TBR ###########GetAllDlgInfo##################  \n\n");
      printf (" TBR: lDlgRcd.callingAddr->sa_gt.sag_num:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_gt.sag_num);
      printf (" TBR: lDlgRcd.callingAddr->sa_tt:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_tt);
      printf (" TBR: lDlgRcd.callingAddr->sa_np:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_np);
      printf (" TBR: lDlgRcd.callingAddr->sa_es.sag_num:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_es);
      printf (" TBR: lDlgRcd.callingAddr->sa_nai:%d \n",
            mInDlgRecordPool[index].callingAddr->sa_nai);
   }
#endif

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : Lock 
// DESCRIPTION : Locks sharedmemory with semaphore variable
// PARAMETER   : int
// RETURN      : UINT32 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::Lock (int semNum)
{
   struct sembuf lsemLocker[2];
   int lReturnVal;
   int lnsops;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lsemLocker[0].sem_num = semNum;
   lsemLocker[0].sem_op = 0;
   lsemLocker[0].sem_flg = SEM_UNDO;

   lsemLocker[1].sem_num = semNum;
   lsemLocker[1].sem_op = 1;
   lsemLocker[1].sem_flg = SEM_UNDO | IPC_NOWAIT;

   lnsops = 2;

   if (0 == mSemId)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Lock: SemId=0 ERROR");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);

      //this->SemInit();

      return false;
   }

   lReturnVal = semop (mSemId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Lock: SemLock failed");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);

      //this->SemInit();

      return false;
   }
   //T(gTrace, printf("%s : Lock (SemId:%d) - Success.\n", gProcessName, mSemId););
   sprintf (lLogText, "DM: SemId:%d In Lock() Success", mSemId);
   T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog (ACUTCAP133, 3, lLogText);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : UnLock 
// DESCRIPTION : UnLocks sharedmemory with semaphore variable
// PARAMETER   : int
// RETURN      : BOOLEAN 
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::UnLock (int semNum)
{
   struct sembuf lsemLocker[2];
   int lReturnVal;
   int lnsops;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lsemLocker[0].sem_num = semNum;
   lsemLocker[0].sem_op = -1;
   lsemLocker[0].sem_flg = SEM_UNDO | IPC_NOWAIT;

   lnsops = 1;

   if (0 == mSemId)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "UnLock: SemId=0 ERROR");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);

      //this->SemInit();

      return false;
   }

   lReturnVal = semop (mSemId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "UnLock: SemUnLock failed");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);

      //this->SemInit();

      return false;
   }
   //T(gTrace, printf("%s : UnLock (SemId:%d) - Success.\n", gProcessName, mSemId););
   sprintf (lLogText, "DM: SemId:%d In UnLock() Success", mSemId);
   T (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog (ACUTCAP134, 3, lLogText);

   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : UpdateDlgMgmtQueueIdx
// DESCRIPTION : The function increments the passed index of DlgMgmtQueue. If it
//               crosses the boundary then the passed index is set to first
//               position
// PARAMETER   : IdxPtr - Address of nextFreeIdx or lastFreeIdx
// RETURN      : none
//-------------------------------------------------------------------------------
BOOLEAN DlgMgr::UpdateDlgMgmtQueueIdx (UINT32 * IdxPtr)
{
   if ((*IdxPtr + 1) > mMaxAllocateDlgSize)
   {
      *IdxPtr = 1;
   }
   else
   {
      (*IdxPtr)++;
   }

   return true;
}
