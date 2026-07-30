//@(#) $Id: TcapAculabUtil.h,v 1.1.4.2 2016/07/27 23:04:52 jamesjac Exp $

#ifndef INCL_ACULAB_UTIL_H
#define INCL_ACULAB_UTIL_H
#include "TcapAculabConstDef.h"


void HandleSignal (int sig);

class AculabUtil 
{
   


   public:
      static BOOLEAN    mSigCfg;
      static BOOLEAN    mSigTrc;
      static BOOLEAN    mKeepRunning;
      static INT32      mSignal;
      static BOOLEAN    mAbortFlag;
      static BOOLEAN    mTransmitFlag[50];   

      static BOOLEAN PrintConState(TEXT *id,
            const acu_tcap_con_state_t *con_state, int &lInstanceNo, TEXT *lHost); 

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
      
      static BOOLEAN GetTransmitFlag(int &lInstanceNo);
      static void SetTransmitFlag(int lInstances);
      static int GetFreeInstanceForTransmit(int & lInstances); 
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
      static TEXT* GetAcuTcapMsgTypeStr(acu_tcap_msg_type_t &lMsgType);
      static void  ShowAddressIndicator( UINT8 & lAddrInd);
      static void GetDestAddress(TCAPAddress &lTcapAddr, TEXT *lStr);
      static  TEXT * ConvertTcapDlgTypeToStr (EnumTcapDlg & dlgType);
      //**********************************
      static void DisplayAddress (const TCAPAddress * address,TEXT * lText);
      static void DisplayUserInformation (const TcapUserInfoPdu * param);
      static void PrintHex (int numberOfBytes, const UINT8 * data, int arrayLen =  MAX_TDARRAY_BYTES);
      static void DisplayApplicationContext (const TDArray * param);
      static void DisplayParamData (const TDArray * param);

      static BOOLEAN PrintSccpStatus(acu_tcap_msg_t *msg);  
      static BOOLEAN DispOctetString(char *string); 
      static BOOLEAN ErrReportSccp(acu_tcap_msg_t  *msg);
      static void ReturnAculabErrStr(int lErrCode,TEXT *lErrText);
      static BOOLEAN PrintConfig ();
};
#endif

