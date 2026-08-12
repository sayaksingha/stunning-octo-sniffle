static const char* id="@(#) $Id: TcapNewWriteMsg.cc,v 1.1.4.2.2.1 2019/04/17 02:33:59 mram Exp $";

//#include "TelesoftStructs.h"
#include "TcapStructs.h"
#include "TcapMsgDisplayApi.h"
#include <iomanip>
TEXT gProcessName[SS7_MAX_PROCESS_NAME + 1] = "";
CTrace gTrace ("TRACE_TCAP");
Log gLog;

   template <class T>
void PrintHex(const T& hex)
{
   if (hex.numberOfBytes > 0)
   {
      printf("  *-------- Data --------*\n    ");
      for (int i = 0; i < hex.numberOfBytes; i++)
         printf(" %02X", hex.array[i]);
      printf("\n  *----------------------*\n");
   }
}

void PrintHex(const TDArray& hex)
{
   if (hex.numberOfBytes > 0)
   {
      printf("  *-------- Data --------*\n    ");
      for (int i = 0; i < hex.numberOfBytes; i++)
         printf(" %02X", hex.array[i]);
      printf("\n  *----------------------*\n");
   }
}

   template <class T>
void FormatHexData(TEXT* lParamData, T& param)
{
   int lParamDataLen = strlen(lParamData);
   int lParamLen     = 0;

   param.numberOfBytes = 0;

   for (int i = 0; i < lParamDataLen && lParamLen < MAX_TDARRAY_BYTES; i++)
   {
      if (lParamData[i] != 0x20 && lParamData[i] != 0x0a)
      {
         if (lParamData[i] >= 'a' && lParamData[i] <=  'f')
         {
            lParamData[i] = lParamData[i] - 'a' + 10 + '0';
         }
         else if (lParamData[i] >= 'A' && lParamData[i] <= 'F')
         {
            lParamData[i] = lParamData[i] - 'A' + 10 + '0';
         }
         param.array[lParamLen] = (lParamData[i] -'0') << 4;

         i++;

         if (lParamData[i] >= 'a' && lParamData[i] <=  'f')
         {
            lParamData[i] = lParamData[i] - 'a' + 10 + '0';
         }
         else if (lParamData[i] >= 'A' && lParamData[i] <= 'F')
         {
            lParamData[i] = lParamData[i] - 'A' + 10 + '0';
         }
         param.array[lParamLen] =
            param.array[lParamLen] | (0x0F & (lParamData[i] -'0') );

         lParamLen++;
      }
   }

   param.numberOfBytes = lParamLen;

   PrintHex(param);

   return;
}

void FormatHexData(TEXT* lParamData, TDArray& param)
{
   int lParamDataLen = strlen(lParamData);
   int lParamLen     = 0;

   param.numberOfBytes = 0;

   for (int i = 0; i < lParamDataLen && lParamLen < MAX_TDARRAY_BYTES; i++)
   {
      if (lParamData[i] != 0x20 && lParamData[i] != 0x0a)
      {
         if (lParamData[i] >= 'a' && lParamData[i] <=  'f')
         {
            lParamData[i] = lParamData[i] - 'a' + 10 + '0';
         }
         else if (lParamData[i] >= 'A' && lParamData[i] <= 'F')
         {
            lParamData[i] = lParamData[i] - 'A' + 10 + '0';
         }
         param.array[lParamLen] = (lParamData[i] -'0') << 4;

         i++;

         if (lParamData[i] >= 'a' && lParamData[i] <=  'f')
         {
            lParamData[i] = lParamData[i] - 'a' + 10 + '0';
         }
         else if (lParamData[i] >= 'A' && lParamData[i] <= 'F')
         {
            lParamData[i] = lParamData[i] - 'A' + 10 + '0';
         }
         param.array[lParamLen] =
            param.array[lParamLen] | (0x0F & (lParamData[i] -'0') );

         lParamLen++;
      }
   }

   param.numberOfBytes = lParamLen;

   PrintHex(param);

   return;
}

int main(int argc, char* argv[])
{
   MsgQueue       lHandlerQue;
   key_t          lHandlerKey;
   msgQueueError  lQueError;
   TcapMsg        lTxMsg;
   INT16          lTxMsgLen    = 0;
   UINT16         lCompCh      = 0;
   UINT16         lDlgCh       = 0;
   UINT16         lTemp        = 0;
   UINT32         lMsgType     = 0;
   TEXT           lDigits[MAX_TDADDRESS_DIGITS];
   TEXT           lParamData[500];

   //for(int i=0; i<argc; i++)
   //   cout << "argv[" <<i << "]:" << argv[i] << "\t";
   // cout << endl;

   //if(argc < 33)
   if(argc < 10)
   {
      printf("argv 1  = <QueKey>\n"
            "argv 2  = <Orig Addr Ind>\n"
            "argv 3  = <Orig SubSytem No>\n"
            "argv 4  = <Orig Pt>\n"
            "argv 5  = <Orig Nature Of Addr>  0-Spare 1-SubscriberNo 3-National 4-International\n"
            "argv 6  = <Orig Trans Type>\n"
            "argv 7  = <Orig Numbering Plan>  => 1 - CC-NDC   7 - MCC-MNC\n"
            "argv 8  = <Orig Gt>\n"
            "argv 9  = <Dst Addr Ind>\n"
            "argv 10 = <Dst SubSytem No>\n"
            "argv 11 = <Dst Pt>\n"
            "argv 12 = <Dst Nature Of Addr> =>0 - Spare 1-SubscriberNo 3-National 4-International\n"
            "argv 13 = <Dst Trans Type>\n"
            "argv 14 = <Dst Numbering Plan>  => 1 - CC-NDC   7 - MCC-MNC\n"
            "argv 15 = <Dst Gt>\n"
            "argv 16 = <Appl Cont Required?0/1>\n"
            "argv 17 = <User Info Required?0/1>\n"
            "argv 18 = <Dlg Type>=> 1-Begin  2-Continue 3-End(Basic) 4-Abort 5 - End(PreArranged)\n"
            "argv 19 = <Comp Present?0/1>\n"
            "argv 20 = <Invoke Id>\n"
            "argv 21 = <Component Type> 1-Invoke  2-ReturnResult 3-ReturnError  4-Reject 5-Abort\n"
            "argv 22 = < Invoke, ReturnResult => Opcode\n"
            "            ReturnError          => ErrType\n"
            "            Reject               => Problem Type\n"
            "            Abort                => Abort Type\n"
            "          >"
            "argv 23 = < ReturnResult         => Last indicator 0/1\n"
            "            ReturnError          => ErrCode\n"
            "            Reject               => Problem Code\n"
            "          >\n"
            "argv 24 = Reject Type  => 0-Unknown  1-Local Tcap 2-Remote Tcap 3-Remote TcUser\n"
            "argv 25 = <DlgId>\n"
            "argv 26 = <Hdlr ssn>\n"
            "argv 27 = <Msg Type>\n"
            "argv 28 = <origTransId>\n"
            "argv 29 = <tcUserId>\n"
            "argv 30 = <UserInfo>\n"
            "argv 31 = <Application Context Version>\n"
            "argv 32 = <Application Context>\n"
            "argv 33 = <Pdu>\n"
            "\n");
      return 1;
   }

   //Setting the Queue Keys
   lHandlerKey = atoi(argv[1]);

   cout << "Handler Queue Key       : " << lHandlerKey << endl;

   //Create the Decoder Queue
   if ((lHandlerQue.Create(lHandlerKey, 0664| IPC_CREAT)) != Q_SUCCESS)
   {
      printf("Error Creating the Queue\n");
      return 1;
   }

   //memsetting the structure
   lTxMsgLen = sizeof(TcapMsg);
   memset(&lTxMsg, '\0', sizeof(TcapMsg));

   lTxMsg.origTransId  = atoi(argv[28]);
   lTxMsg.tcUserId     = atoi(argv[29]);

   //Receiving the Orig Address parameters from the user
   lTxMsg.origAddress.addressIndicator   = atoi(argv[2]);
   lTxMsg.origAddress.subsystemNumber    = atoi(argv[3]);
   lTxMsg.origAddress.pointCode          = atoi(argv[4]);;
   lTxMsg.origAddress.natureOfAddress    = atoi(argv[5]);
   lTxMsg.origAddress.translationType    = atoi(argv[6]);
   lTxMsg.origAddress.numberingPlan      = atoi(argv[7]);
   lTxMsg.origAddress.numberOfDigits     = 0;

   if(atoi(argv[8])  != 0 && (strncasecmp("ZERO", argv[8], 4) && strncasecmp("SKIP", argv[8], 4)))
   {
      strcpy(lDigits, argv[8]);

      lTxMsg.origAddress.numberOfDigits = strlen(lDigits);
      for(UINT8 i=0; i < lTxMsg.origAddress.numberOfDigits; i++)
      {
         lTxMsg.origAddress.digits[i] = (lDigits[i] - '0');
      }

      if ((lTxMsg.origAddress.numberOfDigits % 2) == 0)
         lTxMsg.origAddress.encodingScheme = 2;
      else
         lTxMsg.origAddress.encodingScheme = 1;
   }
   else
      lTxMsg.origAddress.numberOfDigits = 0;

   //Receiving the Destination Address parameters from the user
   lTxMsg.destAddress.addressIndicator    = atoi(argv[9]);
   lTxMsg.destAddress.subsystemNumber     = atoi(argv[10]);
   lTxMsg.destAddress.pointCode           = atoi(argv[11]);
   lTxMsg.destAddress.natureOfAddress     = atoi(argv[12]);
   lTxMsg.destAddress.translationType     = atoi(argv[13]);
   lTxMsg.destAddress.numberingPlan       = atoi(argv[14]);

   lTxMsg.destAddress.numberOfDigits      = 0;
   //ANSI -
   //lTxMsg.ansiApplicationContext = 12;
   if(atoi(argv[15]) != 0 && (strncasecmp("ZERO", argv[8], 4) && strncasecmp("SKIP", argv[8], 4)))
   {
      strcpy(lDigits, argv[15]);

      lTxMsg.destAddress.numberOfDigits = strlen(lDigits);
      for(UINT8 i=0; i < lTxMsg.destAddress.numberOfDigits; i++)
      {
         lTxMsg.destAddress.digits[i] = lDigits[i] - '0';
      }

      if ((lTxMsg.destAddress.numberOfDigits % 2) == 0)
         lTxMsg.destAddress.encodingScheme = 2;
      else
         lTxMsg.destAddress.encodingScheme = 1;
   }
   else
      lTxMsg.destAddress.numberOfDigits = 0;

   //Populating the Application Context
   lTemp = atoi(argv[16]);
   if (lTemp == 1)
   {
      lTxMsg.applicationContext.numberOfBytes = 7;
      lTxMsg.applicationContext.array[0] = 0x04;
      lTxMsg.applicationContext.array[1] = 0x00;
      lTxMsg.applicationContext.array[2] = 0x00;
      lTxMsg.applicationContext.array[3] = 0x01;
      lTxMsg.applicationContext.array[4] = 0x00;

      lTemp = atoi(argv[31]);
      lTxMsg.applicationContext.array[5] = lTemp & 0xff;

      lTemp = atoi(argv[32]);
      lTxMsg.applicationContext.array[6] = lTemp & 0xff;
   }

   //Populating the User Info
   lTemp = atoi(argv[17]);
   if (lTemp == 1)
   {
      strcpy(lParamData, argv[30]);

      FormatHexData(lParamData, (TDArray&)lTxMsg.userInformation);
   }
   else
   {
      lTxMsg.userInformation.numberOfBytes = 0;
   }

   //Identifying the Dialogue
   lDlgCh = atoi(argv[18]);
   switch(lDlgCh)
   {
      case 1:
         lTxMsg.tcapDlg = TCAP_BEGIN;
         break;

      case 2:
         lTxMsg.tcapDlg = TCAP_CONTINUE;
         break;

      case 3:
         lTxMsg.tcapDlg = TCAP_END;
         break;

      case 4:
         lTxMsg.tcapDlg = TCAP_ABORT;
         break;

      case 5:
         lTxMsg.tcapDlg = TCAP_PRE_ARRANGED_END;
         break;
      case 11:
	 lTxMsg.tcapDlg=TCAP_ANSI_QUERY_WITH_PERMISSION;
      default:
         break;
   }

   lTxMsg.componentPresent = atoi(argv[19]);

   if (lTxMsg.componentPresent)
   {
      lTxMsg.tcapComponent.invokeId = atoi(argv[20]);

      lCompCh = atoi(argv[21]);

      printf("lCompCh: %d\n", lCompCh);

      switch(lCompCh)
      {
         case 1:
            {
               lTxMsg.tcapComponent.tcapComp                               = TCAP_INVOKE_COMP;
               lTxMsg.tcapComponent.tcapInvokeComp.operation.operationCode = atoi(argv[22]);
               strcpy(lParamData, argv[33]);
               if (lParamData[0] == '0' && lParamData[1] == 0x0a)
               {
                  cout << " Null ParameterData \n";
               }
               else
               {
                  FormatHexData(lParamData, lTxMsg.tcapComponent.tcapInvokeComp.parameterData);
               }
               break;
            }

         case 2:
            {
               lTxMsg.tcapComponent.tcapComp = TCAP_RET_RESULT_COMP;

               lTemp = atoi(argv[23]);

               if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapRetResultComp.lastIndicator = TCAP_LAST_INDICATOR;
               else
                  lTxMsg.tcapComponent.tcapRetResultComp.
                     lastIndicator = TCAP_NOT_LAST_INDICATOR;
               // Opcode
               lTemp = atoi(argv[22]);

               if(lTemp == 255)
                  lTxMsg.tcapComponent.tcapRetResultComp.
                     //operation.operationCode = TCAP_OP_CODE_NOT_PRESENT;
                     operation.operationCode = 0xFF;
               else
                  lTxMsg.tcapComponent.tcapRetResultComp.operation.operationCode = lTemp;

               strcpy(lParamData, argv[33]);
               if (lParamData[0] != '0' && lParamData[1] != 0x0a)
                  FormatHexData(lParamData,
                        lTxMsg.tcapComponent.tcapRetResultComp.parameterData);
               break;
            }
         case 3:
            {
               lTxMsg.tcapComponent.tcapComp                            = TCAP_RET_ERROR_COMP;
               lTxMsg.tcapComponent.tcapRetErrorComp.errorCode.errorType = atoi(argv[22]);

               strcpy(lParamData, argv[23]);
               if (lParamData[0] != '0' && lParamData[1] != 0x0a)
               {
                  FormatHexData(lParamData,
                        lTxMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCodeData);
               }
               else
                  lTxMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCodeData.numberOfBytes = 0x00;

               strcpy(lParamData, argv[33]);
               if (lParamData[0] != '0' && lParamData[1] != 0x0a)
               {
                  FormatHexData(lParamData,
                        lTxMsg.tcapComponent.tcapRetErrorComp.parameterData);
               }
               break;
            }

         case 4:
            {
               lTxMsg.tcapComponent.tcapComp                     = TCAP_REJECT_COMP;
               //Reject Type
               lTemp = atoi(argv[24]);
               if (lTemp == 0)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REJECT_UNKNOWN;
               else if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = LOCAL_TCAP;
               else if (lTemp == 2)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REMOTE_TCAP;
               else
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REMOTE_TC_USER;

               lTxMsg.tcapComponent.tcapRejectComp.problem.problemCodeType = atoi(argv[22]);
               lTxMsg.tcapComponent.tcapRejectComp.problem.problemCode     = atoi(argv[23]);
               break;
            }

         case 5:
            {
               lTxMsg.tcapComponent.tcapComp            = TCAP_ABORT_COMP;
               lTemp = atoi(argv[22]);
               if (lTemp == 0)
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_NULL_ABORT;
               else if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_USER_ABORT;
               else
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_PROTOCOL_ABORT;

               strcpy(lParamData, argv[33]);
               //if (lParamData[0] != '0')
               {
                  //lTxMsg.tcapComponent.tcapAbortComp.abortReason.numberOfBytes = 0x01;
                  printf("Abort Reason: %s\n", lParamData);
                  FormatHexData(lParamData,
                        lTxMsg.tcapComponent.tcapAbortComp.abortReason);
                  //lTxMsg.tcapComponent.tcapAbortComp.abortReason.array[0] = atoi(argv[33]);
               }
               break;
            }
         default:
            {
               break;
            }
      }

      if (1 == lCompCh || 2 == lCompCh)
      {
         if( 1 == atoi(argv[23]))
            lTxMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
         else
            lTxMsg.tcapComponent.lastComponent = TCAP_NOT_LAST_COMPONENT;
      }
      else
         lTxMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT;
   }

   //DlgId
   lTxMsg.dialogueId = atoi(argv[25]);
   lTxMsg.ssn        = atoi(argv[26]);
   lMsgType          = atoi(argv[27]);

   TcapMsgDisplayApi lDispalyApi;
   lDispalyApi.GenerateTcapMsgTrace (lTxMsg, "Transmit", "stdout");

   //Writing the structure to the Queue
   lQueError = lHandlerQue.WriteMsg((void*)&lTxMsg, lTxMsgLen, lMsgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueError)
   {
      printf("Error in Writing to Queue\n");
      return 1;
   }
   printf("Writing to Queue Success\n");

   return 0;
}
