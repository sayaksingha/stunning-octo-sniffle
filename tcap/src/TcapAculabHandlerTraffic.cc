static const char *id="@(#) $Id: TcapAculabHandlerTraffic.cc,v 1.1.4.2.6.1 2021/12/01 12:43:27 jamesjac Exp $";

//---------------------------------------------------------------
// NAME : TcapAcuHandlerTraffic.cc
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
// Originated: Paresh                  Date:29-Mar-2010
//----------------------------------------------------------------
#include "TcapAculabHandlerTraffic.h"


extern INT32   gDelaySeconds;
extern TEXT    gProcessName[SS7_MAX_PROCESS_NAME];
//----------------------------------------------------------------
// METHOD      : Traffic
// DESCRIPTION : Constructor of class Traffic
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------
Traffic::Traffic()
{
   mTotalSeconds = 0;
   //DlgMgr  lDlgMgr(gProcessName,TRACE_ACU_TCAP_TRAFFIC_ENV);
   //mDlgManager = lDlgMgr;
}

//----------------------------------------------------------------
// METHOD      : ~Traffic
// DESCRIPTION : destructor of class Traffic
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------
Traffic::~Traffic()
{

}

//----------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes the CreateSharedMemory and  
//               ReadConfigData functions
// PARAMETER   : none
// RETURN      : Boolean
//               true - If Initialization is successful
//               false- If Initialization fails
//-----------------------------------------------------------------
BOOLEAN Traffic::Init()
{
   //Read Configuration
   if(true != this->ReadConfigData())
   {
      printf("%s: Reading configuration failed \n",gProcessName);
      return false;
   }

   //Create shared Memory
   if(true != this->CreateSharedMemory())
   {
      printf("%s: Failed to create shared memory\n",gProcessName);
      return false;
   }

   if(false == mDlgManager.Init())
   {
      return false;
   }
   return true;
}

//--------------------------------------------------------------------
// METHOD      : ReadConfigData
// DESCRIPTION : Reads configuration files ipc.cfg and Peg.cfg
// PARAMETER   : none
// RETURN      : Boolean
//               true - Reading configuration is successful
//               false- Reading configuration failed
//---------------------------------------------------------------------
BOOLEAN Traffic::ReadConfigData()
{
   CfgRead     lCfgRead(gProcessName);

   //Reading form file ipc.cfg
   if (false == lCfgRead.CfgInit(SS7_IPC_CFG))
   {
      printf("Could not open the file %s\n", SS7_IPC_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum("SHM_MIS_PEG_KEY",mShmKey,SHM_MIN,SHM_MAX))
   {
      printf("SHM_MIS_PEG_KEY not found in the file %s\n", SS7_IPC_CFG);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      printf("%s: SHM_MIS_PEG_KEY = %d\n",gProcessName, mShmKey);
   }


   //Close ipc.cfg
   lCfgRead.CfgDeInit();

   //Reading from file Peg.cfg
   if (false == lCfgRead.CfgInit(SS7_PEG_CFG))
   {
      printf("Could not open the file %s\n", SS7_PEG_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum("NUM_OF_EVENTS",mNumOfEvents,1,
            150))
   {
      printf("NUM_OF_EVENTS Parameter not found in the file %s\n",SS7_PEG_CFG);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      printf("%s: NUM_OF_EVENTS = %ld\n",gProcessName, mNumOfEvents);
   }

   //Close peg.cfg
   lCfgRead.CfgDeInit();
   //Reading from file kernel.cfg
   if(false == lCfgRead.CfgInit(SS7_KER_CFG))
   {
      printf("Could not open the file %s\n", SS7_KER_CFG);
      return false;
   }
   if (CFG_OK != lCfgRead.GetConfigNum("MAX_ACU_TCAP_DLG_SIZE",mMaxDlgSize,
            1,256000))
   {
      printf("MAX_ACU_TCAP_DLG_SIZE  Parameter not found in the file %s\n",SS7_KER_CFG);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      printf("%s: MAX_ACU_TCAP_DLG_SIZE = %u\n",gProcessName,mMaxDlgSize);
   }
   lCfgRead.CfgDeInit();
   return true;
}

//-------------------------------------------------------------
// METHOD        : ReloadConfig
// DESCRIPTION   : Reloads the configuration from the file
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
void Traffic::ReloadConfig()
{
   CfgRead   lCfgRead(gProcessName);

   //Open Peg.cfg file
   if(false == lCfgRead.CfgInit(SS7_PEG_CFG))
   {
      printf("Failed to open the file %s\n", SS7_PEG_CFG);
   }

   if(CFG_OK != lCfgRead.GetConfigNum("NUM_OF_EVENTS",mNumOfEvents,1,
            150))
   {
      printf("NUM_OF_EVENTS Parameter not found in %s\n",SS7_PEG_CFG);
      lCfgRead.CfgDeInit();
   }

   else
   {
      printf("%s: NUM_OF_EVENTS = %ld\n",gProcessName, mNumOfEvents);
   }

   lCfgRead.CfgDeInit();
   //Close Peg.cfg file
}


//-------------------------------------------------------------------------
// METHOD      : CreateSharedMemory
// DESCRIPTION : Creates Shared memory for reading the events
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Created Successfully
//               false - Unable to create 
//--------------------------------------------------------------------------
BOOLEAN Traffic::CreateSharedMemory()
{
   /*
      INT32 lShmID = 0;

   //create shared memory
   lShmID = shmget ((key_t)mShmKey, sizeof(EventInfo), IPC_CREAT|IPC_EXCL| SS7_IPC_PERM);

   if (lShmID < 0)
   {
   lShmID = shmget ((key_t)mShmKey, sizeof(EventInfo), IPC_CREAT| SS7_IPC_PERM);
   if(lShmID < 0)
   {   
   printf("Unable to create shared memory with Key = %d\n", mShmKey);
   return false;
   }   
   }


   //Map a pointer to the shared memory created 
   mShmPtr = (EventInfo *) shmat (lShmID, (EventInfo *) 0, 0);

   //NULL pointer indicates unsuccessfull mapping
   if (NULL == mShmPtr) 
   {
   printf("Unable to map pointer to the shared memory\n");
   return false;
   }

   printf("%s: Shared memory created successfully with key-%d\n",gProcessName,mShmKey);
   */
   return true;
}

//------------------------------------------------------------------------
// METHOD      : GetSnapShot
// DESCRIPTION : Reads values from the shared memory
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Read Success
//------------------------------------------------------------------------
BOOLEAN Traffic::GetSnapShot()
{
   /*  for (int i = PEG_DROP_RCVD_FROM_NWK; i <= PEG_SEND_TO_APPL; i++)
       {
       mCurrEventInfo.OnlinePegCount[i][0] = mShmPtr->OnlinePegCount[i][0];
       }
       */
   return true;
}

//-----------------------------------------------------------------------
// METHOD      : GetInitialSnapShot
// DESCRIPTION : Reads values from the shared memory
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Read Success
//-----------------------------------------------------------------------
BOOLEAN Traffic::GetInitialSnapShot()
{
   /*
      for (int i = PEG_DROP_RCVD_FROM_NWK; i <= PEG_SEND_TO_APPL ; i++)
      {
      mPrevEventInfo.OnlinePegCount[i][0] = mShmPtr->OnlinePegCount[i][1];
      }   
      */
   return true;
}

//------------------------------------------------------------------------
// METHOD      : PrintTraffic
// DESCRIPTION : Prints the values read from shared memory
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Success
//               false - failure
//-------------------------------------------------------------------------
BOOLEAN Traffic::PrintTraffic()
{
   /*
      static unsigned long int  lPeakVal[PEG_SEND_TO_APPL+2];
      static unsigned long int  lTotalValue[PEG_SEND_TO_APPL+2];
      this->CalculateCurrentTime();
      printf("================================================================================\n");
      printf("                         ACULAB TCAP HANDLER PEG EVENTS\n");
      printf("                            %s\n",mCurrentTime);
      printf("================================================================================\n");
      cout << endl; 
      cout << "  ";
      cout.width(42);
      cout.fill(' ');
      cout.setf(ios::left, ios::adjustfield);
      cout<<"EVENTS";
      cout.width(5);
      cout.setf(ios::right, ios::adjustfield);
      cout<<gDelaySeconds;
      cout<<" s";
      cout.width(12);
      cout.setf(ios::left, ios::adjustfield);
      cout.setf(ios::right, ios::adjustfield);
      cout<<"Peak";
      cout.width(14);
      cout.setf(ios::left, ios::adjustfield);
      cout.setf(ios::right, ios::adjustfield);
      mTotalSeconds = mTotalSeconds + gDelaySeconds;
      cout<<mTotalSeconds<<" s"<<endl;
      printf("--------------------------------------------------------------------------------\n");

      for (int lEvent = PEG_DROP_RCVD_FROM_NWK ; lEvent <= PEG_SEND_TO_APPL + 1; lEvent++)
      {
   //lTotalValue[lEvent] = 0;
   switch(lEvent)
   {
   case PEG_DROP_RCVD_FROM_NWK:

   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX FROM NETWORK DROPPED" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
   {
   lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   }   
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_DROP_SEND_TO_NWK:   

   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX TO NETWORK DROPPED" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
   {
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   }   
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   printf("--------------------------------------------------------------------------------\n");
   break;
   case PEG_RCVD_FROM_APP:
   printf("--------------------------------------------------------------------------------\n");
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX FROM APPLICATION" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
   {
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   }   
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_SEND_TO_NWK:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_RCVD_FROM_NWK:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_SEND_TO_APPL:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX TO APPLICATON" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_BEGIN_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX BEGIN TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_CONTINUE_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX CONTINUE TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_END_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX END TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_USER_ABORT_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX USER_ABORT TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_INVOKE_COMP_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX INVOKE_COMP TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_RET_RESULT_COMP_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX RET_RESULT_COMP TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_RET_ERROR_COMP_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX RET_ERROR_COMP TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_REJECT_COMP_TX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "TX REJECT_COMP TO NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_RSP_TIMEOUT_RX:
   printf("--------------------------------------------------------------------------------\n");
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX RSP_TIMEOUT FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_BEGIN_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX BEGIN FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_CONTINUE_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX CONTINUE FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_END_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX END FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_USER_ABORT_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX USER_ABORT FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_PROTOCOL_ABORT_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX PROTOCOL_ABORT FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_INVOKE_COMP_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX INVOKE_COMP FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_RET_RESULT_COMP_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX RET_RESULT_COMP FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_RET_ERROR_COMP_RX:
   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX RET_ERROR_COMP FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;

   case PEG_TCAP_REJECT_COMP_RX:

   cout << "  ";
   cout.width(42);
   cout.fill('.');
   cout.setf(ios::left, ios::adjustfield);
   cout << "RX REJECT_COMP FROM NETWORK" ;
   cout.width(5);
   cout.setf(ios::right, ios::adjustfield);
   cout<< mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);
   if( mPrevEventInfo.OnlinePegCount[lEvent][0] > lPeakVal[lEvent])
      lPeakVal[lEvent] = mPrevEventInfo.OnlinePegCount[lEvent][0];
   cout<< lPeakVal[lEvent];
   cout.width(14);
   cout.setf(ios::right, ios::adjustfield);

   lTotalValue[lEvent] = (lTotalValue[lEvent] +  mPrevEventInfo.OnlinePegCount[lEvent][0]);
   cout<< lTotalValue[lEvent];
   cout<<endl;
   break;
   case PEG_TCAP_NULL_ABORT_RX:
   break;
   case PEG_TCAP_UNIDIRECTIONAL_RX:
   break;
   default:
   break;

}
}

printf("--------------------------------------------------------------------------------\n");
cout << "  ";
cout.width(46);
cout.fill('.');
cout.setf(ios::left, ios::adjustfield);
cout << "FREE DIALOGUE" << "("<<mMaxDlgSize/2<<")" ;
cout.width(5);
cout.setf(ios::right, ios::adjustfield);
cout<<mDlgManager.GetNumberOfFreeDlgRecords() ;
cout.width(14);
cout.setf(ios::right, ios::adjustfield);
cout<<mDlgManager.GetDlgPoolUsage()<<"%Used";
cout<<endl;

printf("\n");
return true; */
}

//----------------------------------------------------------------
// METHOD      : ProcessTraffic
// DESCRIPTION : Prints the current values and difference between 
//               the current and previous values
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Success 
//               false - failure
//-----------------------------------------------------------------
BOOLEAN Traffic::ProcessTraffic()
{
   /*
      if (true != this->GetSnapShot())
      {
      printf("Failed to Read from shared memory...\n");
      }

   //Difference between current and previous info
   for(int i = PEG_DROP_RCVD_FROM_NWK; i <= PEG_SEND_TO_APPL; i++)
   {
   mPrevEventInfo.OnlinePegCount[i][0] = 
   mCurrEventInfo.OnlinePegCount[i][0] - mPrevEventInfo.OnlinePegCount[i][0];
   }

   //Print the Traffic
   if( true !=this->PrintTraffic())
   {
   printf("Failed to display the traffic events...\n");
   }

   //Load the current info into previous info
   for(int i = PEG_DROP_RCVD_FROM_NWK; i <= PEG_SEND_TO_APPL;i++)
   {
   mPrevEventInfo.OnlinePegCount[i][0] = mCurrEventInfo.OnlinePegCount[i][0];
   }
   */
   return true;
}

//-------------------------------------------------------------------
// METHOD      : CalculateCurrentTime
// DESCRIPTION : Gives the current date and time
// PARAMETER   : none
// RETURN      : Boolean
//               true  - Success
//               false - failure
//-------------------------------------------------------------------
BOOLEAN Traffic::CalculateCurrentTime()
{
   unsigned int         lTimelen;
   timeval              lTimeval;
   struct tm*           lTmStruct;
   TEXT                 lTimeString[50];

   gettimeofday(&lTimeval, NULL);
   lTmStruct = localtime(&(lTimeval.tv_sec));

   lTimelen = strftime(lTimeString, 50 ,"%e %b %Y %T", lTmStruct);
   strcpy(mCurrentTime,lTimeString);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : De-Initialization
// DESCRIPTION :
// PARAMETER   :
// RETURN      : void
//----------------------------------------------------------------------
BOOLEAN Traffic::DeInit()
{
   return true;
}



