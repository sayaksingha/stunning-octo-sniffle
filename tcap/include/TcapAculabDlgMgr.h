// @(#) $Id: TcapAculabDlgMgr.h,v 1.1.4.2.4.1 2021/12/01 12:43:26 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME : TcapAculabDlgMgr.h 
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
#ifndef INCL_ACULAB_DLG_MGR_H
#define INCL_ACULAB_DLG_MGR_H

#include "TcapAculabUtil.h"

class DlgMgr
{
   TEXT                    mLogText[MAX_LOG_TEXT_LEN];

   DlgRecordPool           mInDlgRecordPool;
   DlgMgmtQueue*           mDlgMgmtQueue;

   DlgMgmtQueueHeader*     mDlgMgmtQueueHeader;
   
   unsigned int            mMaxDlgSize;
   unsigned int            mHalfMaxDlgSize;
   unsigned int            mMaxAllocateDlgSize;

   key_t                   mInDlgShmKey;

   key_t                   mInDlgSemKey;

   key_t                   mDlgMgmtQKey;

   pthread_mutex_t         mThreadLock;

   int                     mSemId;


   public:

     DlgMgr();
     DlgMgr(TEXT *,TEXT *);
     ~DlgMgr();


    BOOLEAN  Init(); 
    BOOLEAN  ReadConfig();
    
    // Returns Allocated DlgId for outgoing Map messages.
    DlgRecord*  Allocate_DlgId(DlgRecord &lDlgRecord);

    // Free Dialogue Id and the corresponding Info for both incoming & outgoing messages.
    BOOLEAN  FreeDlgInfo(const unsigned int &dlgId);

    // Get Dlg Information stored at that dialogue Id.
    BOOLEAN  GetDlgInfo(const unsigned int &dlgId, struct DlgRecord &);

    DlgRecord*  GetDlgInfo(const unsigned int dlgId);

    // Update Dlg Info for the givem dlg Id.
    DlgRecord*  UpdateDlgInfo(unsigned int & dlgId, DlgRecord &lDlgRecord);


    // Returns the percentage of Pool Usage utilized for outgoing Map Messages.
    UINT16   GetDlgPoolUsage();

    unsigned int GetHalfDlgSize();
    // Returns back the number of free dlg Ids.
    UINT32   GetNumberOfFreeDlgRecords();
   
    BOOLEAN  GetInsertTimeAndSsn(UINT32 index,time_t & insertTime, int &lSsn);

    BOOLEAN  GetInsertTime(UINT32 index,time_t & insertTime);
    
    BOOLEAN  GetFreeTime(UINT32 index,UINT32& insertTime);

   BOOLEAN Lock(int semNum);
   BOOLEAN UnLock(int semNum);
   BOOLEAN UpdateDlgMgmtQueueIdx(UINT32 *index);

   BOOLEAN GetAllDlgInfo(UINT32 index,time_t & insertTime, int &lSsn, int &localTid, int &localTidLen, int &destTid, int & destTidLen, int &invokeId, acu_sccp_addr_t &callingAddr, acu_sccp_addr_t &calledAddr, EnumTcapDlg&, UINT8& );

	BOOLEAN GetAllDlgInfo(DlgRecord &lDlgRcd);

};

#endif


