static const char* id="@(#) $Id: TcapReadMsg.cc,v 1.1.1.3.6.4.12.1 2019/04/17 02:33:59 mram Exp $";

//#include "TelesoftStructs.h"
#include "TcapStructs.h"
#include "TcapMsgDisplayApi.h"

TEXT gProcessName[SS7_MAX_PROCESS_NAME + 1] = "";
CTrace gTrace ("TRACE_TCAP");
Log gLog;

int main(int argc, char* argv[])
{
   MsgQueue       lHandlerQue;
   key_t          lHandlerKey;
   msgQueueError  lQueError;
   TcapMsg        lTxMsg;
   INT16          lTxMsgLen    = 0;
   //UINT16         lCompCh      = 0;
   //UINT16         lDlgCh       = 0;
   UINT16         lTemp        = 0;
   UINT32         lMsgType     = 0;
   UINT32         lCount = 1;
   //TEXT           lDigits[MAX_TDADDRESS_DIGITS];
   //TEXT           lParamData[500];

   if(argc != 3)
   {
      printf("Usage: <Queue Key> <count>\n");
      return 1;
   }

   //Setting the Queue Keys
   lHandlerKey = atoi(argv[1]);

   lCount =  atoi(argv[2]);
   cout << "Queue Key               : " << lHandlerKey << endl;

   //Create the Decoder Queue
   if ((lHandlerQue.Create(lHandlerKey, 0664| IPC_CREAT)) != Q_SUCCESS)
   {
      printf("Error Creating the Queue\n");
      return 1;
   }

   //memsetting the structure
   lTxMsgLen = sizeof(TcapMsg);
   memset(&lTxMsg, 0, lTxMsgLen);

   cout << "Enter MsgType           : ";
   cin  >> lTemp;
   lMsgType = lTemp;
   
   LONG  lMsgType1 = 0;
   TcapMsgDisplayApi lDispalyApi;
   
   do
   {
      if (lMsgType)
         lQueError = lHandlerQue.ReadMsg((void*)&lTxMsg, lTxMsgLen, lMsgType, NON_BLOCKING);
      else
         lQueError = lHandlerQue.ReadMsgWithType((void*)&lTxMsg, lTxMsgLen, 
               lMsgType1, NON_BLOCKING);

      if (Q_SUCCESS == lQueError)
      {
         if (0 == lMsgType)
         {
            cout << " --------------------------------------" << endl;
            cout << " MsgType                : " << lMsgType1 << endl;
            cout << " --------------------------------------" << endl;
         }
         lDispalyApi.GenerateTcapMsgTrace (lTxMsg, "Receive", "stdout");
         lCount--;
      }
   }
   while ((Q_SUCCESS == lQueError) && (lCount > 0));

   return 0;
}
