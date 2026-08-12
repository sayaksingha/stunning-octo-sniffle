// @(#) $Id: TcapAculabTransDlgMap.h,v 1.1.4.1.16.1 2021/12/01 12:43:26 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME: TcapAculabTransDlgMap.h 
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
#include <string>
#include <sstream>
#include <vector>
#include <map>
//#include <hash_map>

using namespace std;
#ifndef INCL_ACULAB_TRANS_DLG_MAP_H
#define INCL_ACULAB_TRANS_DLG_MAP_H

typedef DlgRecord DlgRecordData;

class TransDlgMap
{
  
   //CTrace mTrace;
   //Log    mLog;
   //TEXT   mProcessName[50 + 1];
   DlgMgr mDlgMgr;
   
   int                 mTransValidationKey;
   int                 mSsn;
   map <unsigned int , DlgRecordData> mapDlgRecord;
   map <UINT32 , acu_tcap_trans_t *>        mapTransRecord; 
   
   pthread_mutex_t         mTransLock;
   pthread_mutex_t         mDlgLock;

   public:
      
   TransDlgMap();
   
   //TransDlgMap(TEXT *, TEXT *);
   
   ~TransDlgMap();
   
   // Ab Change: Pass lCfgFile so it can be passed to DlgMgr
   BOOLEAN Init(int, int, TEXT*);
   
   BOOLEAN ReadConfig();
   
   BOOLEAN MapTransIdToDlgId(acu_tcap_trans_t *lTrans,unsigned int &lDlgId);
	BOOLEAN MapTransIdToDlgId(acu_tcap_trans_t *lTrans, DlgRecord &lDlgRecord);
   
   BOOLEAN MapDlgIdtoTransId(DlgRecord &lDlgRecoed);
   
   BOOLEAN GetTransInfo(DlgRecord &lDlgRecord,acu_tcap_trans_t **lTrans);
   
   BOOLEAN FreeDialogueInfo(const UINT32 dlgIndex);
   
   BOOLEAN GetDialogueInfo(acu_tcap_trans_t *lTrans, DlgRecord &lDlgRecoed);
   
   BOOLEAN DeleteDlgInfo(unsigned int dlgId); 
   
   BOOLEAN DeleteTransInfo(unsigned int lTransId);
   
  unsigned int GetHalfDlgSize();

   void PrintMapSize();
};

#endif

