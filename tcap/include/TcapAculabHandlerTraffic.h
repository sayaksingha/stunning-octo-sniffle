// @(#) $Id: TcapAculabHandlerTraffic.h,v 1.1.4.2 2014/04/21 10:47:37 sreenu Exp $
//---------------------------------------------------------------
// NAME :TcapAcuHandlerTraffic.h  
//
// COPYRIGHT
// TSS : - Copyright (C) 2010 Tayana Software Solution Pvt Ltd.,
//         All rights reserved. No part of this computer program
//         may be used or reproduced in any form by any
//         means without prior written permission of
//         Tayana Software Solution Pvt Ltd.
//
// DESCRIPTION
//
// Originated By: Paresh                     Date:29-MAR-2010
//----------------------------------------------------------------


#ifndef INCL_ACU_TCAP_TRAFFIC_H
#define INCL_ACU_TCAP_TRAFFIC_H

#include "TcapAculabConstDef.h"
#include "TcapAculabDlgMgr.h"
//#include "Peg.h"  //CMS
#define   DELAY_DEFAULT 1


class Traffic
{
   public:
      //Constructor
      Traffic();
      // Destructor
      ~Traffic();

      BOOLEAN Init();

      BOOLEAN ReadConfigData();

      void    ReloadConfig();

      BOOLEAN CreateSharedMemory();

      BOOLEAN GetSnapShot();

      BOOLEAN GetInitialSnapShot();

      BOOLEAN PrintTraffic();

      BOOLEAN CalculateCurrentTime();

      BOOLEAN ProcessTraffic();

      BOOLEAN DeInit();

      DlgMgr mDlgManager;
   
   private:
      //EventInfo    *mShmPtr;

      TEXT         mCurrentTime[50];

      key_t        mShmKey;

      INT32        mNumOfEvents;

      //EventInfo    mPrevEventInfo;

      //EventInfo    mCurrEventInfo;

      UINT32       mTotalSeconds;
      unsigned int mMaxDlgSize;

};

#endif
