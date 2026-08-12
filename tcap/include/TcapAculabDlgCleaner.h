// @(#) $Id: TcapAculabDlgCleaner.h,v 1.1.4.3 2019/04/16 20:58:02 jamesjac Exp $
//---------------------------------------------------------------
// NAME : TcapAcuDlgCleaner.h 
//
// COPYRIGHT
// TSS : - Copyright (C) 2010 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
// Originator  : Paresh                         Date: 30/03/10
//----------------------------------------------------------------
#ifndef TCAP_ACU_DLG_CLEANER_H
#define TCAP_ACU_DLG_CLEANER_H

#include "TcapAculabUtil.h"
#include "TcapAculabDlgMgr.h"

class DlgCleaner
{
   public:
      DlgCleaner();

      ~DlgCleaner();

      BOOLEAN Init();

      BOOLEAN DeInit();

      BOOLEAN ReadConfig();

      void    ReloadConfig();

      BOOLEAN CleanTimedoutDlg();

      DlgMgr   mDlgManager;
      
      BOOLEAN CreateMsgQ();
      
      BOOLEAN SendPreArrangedEnd(unsigned int lDlgId, int lSsn);

   private:
      
      UINT32       mDlgTimeout;
      UINT32       mCapDlgTimeout;
      UINT32       mSpecialSsn;
      UINT32       mMaxDlgSize;
      UINT16       mTcapMsgQKey;
      MsgQueue     mTcapMsgQ;
};

#endif
