// @(#) $Id: SccpAculabUtil.h,v 1.1.4.1 2016/07/27 23:04:49 jamesjac Exp $
//----------------------------------------------------------------------------
// NAME:SccpAculabUtil.h
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
// Originated By : Keerthini AB                            Date : 25 Aug 2014
//----------------------------------------------------------------------------
#ifndef INCL_ACULAB_UTIL_H
#define INCL_ACULAB_UTIL_H
#include "SccpAculabConstDef.h"

void HandleSignal (int sig);

class AculabUtil 
{
   public:
      static BOOLEAN    mSigCfg;
      static BOOLEAN    mSigTrc;
      static BOOLEAN    mKeepRunning;
      static INT32      mSignal;
      static BOOLEAN    mAbortFlag;
      static BOOLEAN    mTransmitFlag;   

      static BOOLEAN PrintConState(TEXT *id,
            const acu_sccp_con_state_t *con_state); 

      static void LoadSignals();
      static INT32 GetSignalNo();
      static void HandleSignal(int sig);
      static BOOLEAN KeepRunning();
      static void ResetKeepRunning();
      static void ProcessToExit();
      static BOOLEAN ReloadConfig();
      // Reset config reload status
      static  void ResetConfigFlag();
      // Returns the config error string
      static  TEXT* GetConfigError(const ConfigReturn lCfgRet);
      // Returns status whether process should check trace variable
      static  BOOLEAN CheckTrace();
      // Reset config trace variable
      static  void ResetTraceFlag();
      
      static BOOLEAN GetTransmitFlag();
      static void SetTransmitFlag();
      //static int GetFreeInstanceForTransmit(int & lInstances); 
      // Recreate the Queue on Error
      static  BOOLEAN ReCreateMsgQueue(MsgQueue& queue, const key_t key);
      //Handle Queue Create Error
      static  BOOLEAN HandleQueueCreateError(msgQueueError err,
            MsgQueue&     queue);
      // Handle Queue Read Error
      static  BOOLEAN HandleQueueReadError(msgQueueError err, MsgQueue& queue);
      // Handle Queue Write Error
      static  BOOLEAN HandleQueueWriteError(msgQueueError err,
            MsgQueue&     queue,
            void*         ptr,
            INT16&        size,
            int           mtype,
            modeOption    mode = BLOCKING);

      static TEXT* ConvertQueErrToStr(const msgQueueError queErr);
      static TEXT* GetAcuSccpMsgTypeStr(acu_sccp_msg_type_t &lMsgType);
      static TEXT* GetAcuUserStatusMsgTypeStr(const acu_sccp_u_svals &lStatus);
      static TEXT* GetAcuSpStatusTypeStr(const acu_sccp_sp_svals &lStatus);
      static TEXT* GetAcuSccpStatusTypeStr(const acu_sccp_sccp_svals &lStatus);

      //**********************************
      static void PrintHex (int numberOfBytes, const UINT8 * data, int arrayLen =  MAX_TDARRAY_BYTES);

      static BOOLEAN PrintSccpStatus(acu_sccp_msg_t *msg);  
      static void ReturnAculabErrStr(int lErrCode,TEXT *lErrText);
};
#endif

