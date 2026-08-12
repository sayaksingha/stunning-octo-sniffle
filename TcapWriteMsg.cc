static const char* id="@(#) $Id: TcapWriteMsg.cc,v 1.1.1.3.6.3.16.1 2019/04/17 02:34:00 mram Exp $";

//#include "TelesoftStructs.h"
#include "TcapStructs.h"
#include "TcapMsgDisplayApi.h"

TEXT gProcessName[SS7_MAX_PROCESS_NAME + 1] = "";
CTrace gTrace ("TRACE_TCAP");
Log gLog;

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

//----------------------------------------------------------------------
// METHOD      : 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
void GetTcapAddress (TCAPAddress & rAddress)
{
   TEXT lAddress[SS7_MAX_ADDRESS_LEN +1] = "";
   int lTemp = 0;

   cout << "--------------------------------------" << endl;
   cout << " Enter Address Indicator    : ";
   cin  >> lTemp;
   rAddress.addressIndicator = lTemp;

   cout << " Enter Subsystem Number     : ";
   cin  >> lTemp;
   rAddress.subsystemNumber = lTemp;

   cout << " Enter PointCode            : ";
   cin  >> lTemp;
   rAddress.pointCode = lTemp;

   cout << " Enter NatureOfAddress      : ";
   cin  >> lTemp;
   rAddress.natureOfAddress = lTemp;

   rAddress.translationType = 0;

   cout << " Enter NumberingPlan        : ";
   cin  >> lTemp;
   rAddress.numberingPlan = lTemp;

   cout << " Enter Address              : ";
   cin  >> lAddress;
   rAddress.numberOfDigits = strlen (lAddress);

   if ((rAddress.numberOfDigits % 2) == 0)
      rAddress.encodingScheme = 2;
   else
      rAddress.encodingScheme = 1;

   for(UINT8 i = 0; i < rAddress.numberOfDigits; i++)
   {
      rAddress.digits[i] = lAddress[i] - '0';
   }
   cout << "--------------------------------------" << endl;
}

//----------------------------------------------------------------------
// METHOD      : 
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
   MsgQueue       lHandlerQue;
   key_t          lHandlerKey;
   msgQueueError  lQueError;
   TcapMsg        lTxMsg;
   INT16          lTxMsgLen    = 0;
   UINT16         lTemp        = 0;
   UINT16         lMsgType     = 0;
   TEXT           lParamData[500];

   if(argc < 4)
   {
      printf("Usage: <Queue Key> <DlgId> <HdlrSsn> <Optional Flags>\n");
      printf ("Optional Flags(1/0): OrigAddr? DestAddr? Acn? UserInfo? Comp?\n");
      return 1;
   }

   //Setting the Queue Keys
   lHandlerKey = atoi(argv[1]);

   //Create the Decoder Queue
   if ((lHandlerQue.Create(lHandlerKey, 0664| IPC_CREAT)) != Q_SUCCESS)
   {
      printf("Error Creating the Queue\n");
      return 1;
   }

   //memsetting the structure
   lTxMsgLen = sizeof(TcapMsg);
   memset(&lTxMsg, 0, lTxMsgLen);

   lTxMsg.dialogueId = atoi (argv[2]);
   lTxMsg.ssn = atoi (argv[3]);

   if (argc > 4)
      lTemp = atoi(argv[4]);
   else
   {
      cout << "OrigAddr Required?(1/0)     : ";
      cin  >> lTemp;
   }
   if (lTemp == 1)
   {
      GetTcapAddress (lTxMsg.origAddress);
   }

   if (argc > 5)
      lTemp = atoi(argv[5]);
   else
   {
      cout << "DestAddr Required?(1/0)     : ";
      cin  >> lTemp;
   }
   if (lTemp == 1)
   {
      GetTcapAddress (lTxMsg.destAddress);
   }

   if (argc > 6)
      lTemp = atoi(argv[6]);
   else
   {
      cout << "Acn Required?(1/0)          : ";
      cin  >> lTemp;
   }
   if (lTemp == 1)
   {
      //Populating the Application Context
      lTxMsg.applicationContext.numberOfBytes = 7;
      lTxMsg.applicationContext.array[0] = 0x04;
      lTxMsg.applicationContext.array[1] = 0x00;
      lTxMsg.applicationContext.array[2] = 0x00; 
      lTxMsg.applicationContext.array[3] = 0x01;
      cout << "--------------------------------------" << endl;
      if (SCCP_SSN_CAP == lTxMsg.destAddress.subsystemNumber ||
            SCCP_SSN_CAP == lTxMsg.origAddress.subsystemNumber ||
            SCCP_SSN_CAP == lTxMsg.ssn)
      {
         cout << " Enter Acn[4]               : ";
         cin  >> lTemp;
         lTxMsg.applicationContext.array[4] = lTemp;
         cout << " Enter Acn[5]               : ";
         cin  >> lTemp;
         lTxMsg.applicationContext.array[5] = lTemp;
         cout << " Enter Acn[6]               : ";
         cin  >> lTemp;
         lTxMsg.applicationContext.array[6] = lTemp;
      }
      else
      {
         lTxMsg.applicationContext.array[4] = 0x00;
         cout << " Enter Acn                  : ";
         cin  >> lTemp;
         lTxMsg.applicationContext.array[5] = lTemp;
         cout << " Enter Version              : ";
         cin  >> lTemp;
         lTxMsg.applicationContext.array[6] = lTemp;
      }
      cout << "--------------------------------------" << endl;
   }

   //Populating the UserInfo
   if (argc > 7)
      lTemp = atoi(argv[7]);
   else
   {
      cout << "UserInfo Required?(1/0)     : ";
      cin  >> lTemp;
   }
   if (lTemp == 1)
   {
      getc(stdin);
      cout << " Enter UserInfo PDU         : ";
      //gets(lParamData);
      fgets(lParamData, 500, stdin);

      FormatHexData(lParamData, (TDArray&)lTxMsg.userInformation);
   }
   else
   {
      lTxMsg.userInformation.numberOfBytes = 0;
   }

   //Dialogue Type
   cout << "--------------------------------------" << endl;
   cout << " 1 - Begin      2 - Continue" << endl;
   cout << " 3 - End(Basic) 4 - Abort"    << endl;
   cout << " 5 - End(PreArranged)" << endl;
   cout << "--------------------------------------" << endl;
   cout << "Enter the Dlg Type          : ";
   cin  >> lTemp;

   //Identifying the Dialogue
   switch (lTemp)
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

      default:
         break;
   }

   if (argc > 8)
      lTemp = atoi(argv[8]);
   else
   {
      cout << "Component Present?(1/0)     : ";
      cin  >> lTemp;
   }
   lTxMsg.componentPresent = lTemp ? true : false;

   if (lTxMsg.componentPresent)
   {
      cout << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
      //InvokeId
      cout << " Enter the InvokeId         : ";
      cin  >> lTemp;
      lTxMsg.tcapComponent.invokeId = lTemp;

      cout << " Last Component?(1/0)       : ";
      cin  >> lTemp;
      lTxMsg.tcapComponent.lastComponent =
         lTemp ? TCAP_LAST_COMPONENT : TCAP_NOT_LAST_COMPONENT;

      //Component Type
      cout << " -------------------------------------" << endl;
      cout << "  1 - Invoke       2 - ReturnResult" << endl;
      cout << "  3 - ReturnError  4 - Reject " << endl;
      cout << "  5 - Abort "  << endl;
      cout << " -------------------------------------" << endl;
      cout << " Enter Component Type       : ";
      cin  >> lTemp;

      switch (lTemp)
      {
         case 1:
            {
               lTxMsg.tcapComponent.tcapComp = TCAP_INVOKE_COMP;

               cout << " Enter OpCode               : ";
               cin  >> lTemp;
               lTxMsg.tcapComponent.tcapInvokeComp.operation.operationCode = lTemp;

               //cout << "  Enter LinkedId            : ";
               //cin  >> (char)lTxMsg.tcapComponent[lIndex].tcapInvokeComp.linkedId.linkedId;
               //scanf("%d", &lTxMsg.tcapComponent.tcapInvokeComp.linkedId.linkedId);

               getc(stdin);
               cout << " Enter Invoke PDU(0-Null)   : ";
               fgets(lParamData, 500, stdin);

               if (lParamData[0] == '0' && lParamData[1] == 0x0a)
               {
                  cout << "  Null ParameterData \n";
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

               cout << " LastIndicator?(1/0)        : ";
               cin  >> lTemp;

               if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapRetResultComp.lastIndicator = TCAP_LAST_INDICATOR;
               else
                  lTxMsg.tcapComponent.tcapRetResultComp.lastIndicator = TCAP_NOT_LAST_INDICATOR;

               cout << " Enter OpCode(255->NULL)    : ";
               cin  >> lTemp;

               if(lTemp == 255)
                  //lTxMsg.tcapComponent.tcapRetResultComp.operation.operationCode = TCAP_OP_CODE_NOT_PRESENT;
                  lTxMsg.tcapComponent.tcapRetResultComp.operation.operationCode = 0xFF;
               else
                  lTxMsg.tcapComponent.tcapRetResultComp.operation.operationCode = lTemp;

               getc(stdin);
               cout << " Enter Result PDU(0-Null)   : ";
               fgets(lParamData, 500, stdin);

               if (lParamData[0] == '0' && lParamData[1] == 0x0a)
               {
                  cout << "  Null ParameterData \n";
               }
               else
               {
                  FormatHexData(lParamData, lTxMsg.tcapComponent.tcapRetResultComp.parameterData);
               }
               break;
            }

         case 3:
            {
               lTxMsg.tcapComponent.tcapComp = TCAP_RET_ERROR_COMP;

               cout << " Enter ErrCode Type         : ";
               cin  >> lTemp;
               lTxMsg.tcapComponent.tcapRetErrorComp.errorCode.errorType = lTemp;

               getc(stdin);
               cout << " Enter Err Data(0-Null)     : ";
               fgets(lParamData, 500, stdin);

               if (lParamData[0] != '0' && lParamData[1] != 0x0a)
               {
                  FormatHexData(lParamData, (TDArray&)lTxMsg.tcapComponent.tcapRetErrorComp.errorCode.errorCodeData);
               }

               //cout << "  ParameterData (0-Null): ";
               //fgets(lParamData, 500, stdin);

               //if (lParamData[0] != '0' && lParamData[1] != 0x0a)
               //{
               //FormatHexData(lParamData, lTxMsg.tcapComponent.tcapRetErrorComp.parameterData);
               //}
               break;
            }

         case 4:
            {
               lTxMsg.tcapComponent.tcapComp = TCAP_REJECT_COMP;

               cout << " -------------------------------------" << endl;
               cout << "  0 - Unknown       1 - Local Tcap" << endl;
               cout << "  2 - Remote Tcap   3 - Remote TcUser" << endl;
               cout << " -------------------------------------" << endl;
               cout << " Reject Type                : ";
               cin>>lTemp;

               if (lTemp == 0)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REJECT_UNKNOWN;
               else if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = LOCAL_TCAP;
               else if (lTemp == 2)
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REMOTE_TCAP;
               else 
                  lTxMsg.tcapComponent.tcapRejectComp.rejectType = REMOTE_TC_USER;

               cout << " Problem Type               : ";
               cin  >>lTemp;
               lTxMsg.tcapComponent.tcapRejectComp.problem.problemCodeType = lTemp;

               cout << " Problem Code               : ";
               cin  >> lTemp;
               lTxMsg.tcapComponent.tcapRejectComp.problem.problemCode = lTemp;

               break;
            }

         case 5:
            {
               lTxMsg.tcapComponent.tcapComp = TCAP_ABORT_COMP;

               cout << " -------------------------------------" << endl;
               cout << "  0 - Null Abort    1 - User Abort" << endl;
               cout << "  2 - Protocol Abort" << endl;
               cout << " -------------------------------------" << endl;
               cout << " Abort Type                : ";
               cin>>lTemp;
               if (lTemp == 0)
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_NULL_ABORT;
               else if (lTemp == 1)
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_USER_ABORT;
               else 
                  lTxMsg.tcapComponent.tcapAbortComp.abortType = TCAP_PROTOCOL_ABORT;

               //getc(stdin);
               //cout << "  Abort Reason (0-Null) : ";
               //fgets(lParamData, 500, stdin);

               //if (lParamData[0] != '0' && lParamData[1] != 0x0a)
               //{
               //   FormatHexData(lParamData, lTxMsg.tcapComponent.tcapAbortComp.abortReason);
               //}
               break;
            }

         default:
            {
               break;
            }
      }

      lTxMsg.tcapComponent.lastComponent = TCAP_LAST_COMPONENT; 
      cout << " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
   }

   //cout << "Enter DlgId             : ";
   //cin  >> lTemp;
   //lTxMsg.dialogueId = lTemp;

   //cout << "Enter Handler SSN       : ";
   //cin  >> lTemp;
   //lTxMsg.ssn = lTemp;

   cout << "Enter MsgType               : ";
   cin  >> lTemp;
   lMsgType = lTemp;

   TcapMsgDisplayApi lDispalyApi;
   lDispalyApi.GenerateTcapMsgTrace (lTxMsg, "Transmit", "stdout");

   //Writing the structure to the Queue
   lQueError = lHandlerQue.WriteMsg((void*)&lTxMsg, lTxMsgLen, lMsgType, NON_BLOCKING);

   if (Q_SUCCESS != lQueError)
   {
      printf("Error(%d) in Writing to Queue\n", lQueError);
      return 1;
   }
   printf("Writing to Queue Success\n");

   return 0;
}
