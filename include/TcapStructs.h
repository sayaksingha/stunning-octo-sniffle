// @(#) $Id: TcapStructs.h,v 1.1.1.2.6.6.4.1 2021/12/01 12:43:28 jamesjac Exp $
//----------------------------------------------------------------------
// NAME 
// TcapStructs.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2001
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
// This defines the various structures that are to be populated with regard
// to the TCAP messages.
//
// Originated : Joel Baby Jose                               28-AUG-2008
//----------------------------------------------------------------------
#ifndef INCL_SS7_TCAP_STRUCT_DEF_H
#define INCL_SS7_TCAP_STRUCT_DEF_H

#include "Ss7ConstDef.h"
#include "Ss7Structs.h"

#include "Ss7BerEncDec.h"

#define TCAP_STACK_ID_LEN    16
#define TCAP_APP_ID_LEN      32
#define TCAP_REPLY_TOPIC_LEN 128

//Added by Abhishek
enum EnumSs7Standard
{
   SS7_STANDARD_ITU = 0,
   SS7_STANDARD_ANSI = 1
};

enum EnumTcapDlg
{
   // ITU TCAP Message Types
   TCAP_BEGIN                                = 0,
   TCAP_PRE_ARRANGED_END                     = 1,	// Pre arranged value should be 1 as per tdapi
   TCAP_END                                  = 2,			// basic end should have value as 2 
   TCAP_CONTINUE                             = 3,
   TCAP_ABORT                                = 4,
   TCAP_RSP_TIMEOUT                          = 5,
   TCAP_CONTROL                              = 6,
   TCAP_UNIDIRECTIONAL                       = 7,
   TCAP_BEGIN_WITH_ADDRESS                   = 8,
   TCAP_END_WITH_ADDRESS                     = 9,
   TCAP_BEGIN_CONTINUE                       = 10,                      
   // ANSI TCAP Message Types
   TCAP_ANSI_QUERY_WITH_PERMISSION           = 11,
   TCAP_ANSI_QUERY_WITHOUT_PERMISSION        = 12,
   TCAP_ANSI_RESPONSE                        = 13,
   TCAP_ANSI_CONVERSATION_WITH_PERMISSION    = 14,
   TCAP_ANSI_CONVERSATION_WITHOUT_PERMISSION = 15,
   TCAP_ANSI_ABORT                           = 16,
   TCAP_ANSI_UNI                             = 17  
};

enum EnumTcapComp
{
   TCAP_INVOKE_COMP = 0,
   TCAP_RET_RESULT_COMP = 1,
   TCAP_RET_ERROR_COMP = 2,
   TCAP_REJECT_COMP = 3,
   TCAP_ABORT_COMP = 4,
   TCAP_RSP_TIMEOUT_COMP
};

enum EnumTcapLastComp
{
   TCAP_NO_COMPONENT = 0,
   TCAP_LAST_COMPONENT = 1,
   TCAP_NOT_LAST_COMPONENT = 2
};

enum EnumRetResLastIndicator
{
   TCAP_LAST_INDICATOR = 1,
   TCAP_NOT_LAST_INDICATOR = 2
};

enum EnumAbortType
{
   TCAP_NULL_ABORT = 0,
   TCAP_USER_ABORT = 1,
   TCAP_PROTOCOL_ABORT = 2
};

enum EnumRejectType
{
   REJECT_UNKNOWN = 0,
   LOCAL_TCAP = 1,
   REMOTE_TCAP = 2,
   REMOTE_TC_USER = 3
};

enum EnumTcapRetErrCode
{
   TCAP_RET_ERR_UNKNOWN_SUBS = 1,
   TCAP_RET_ERR_ABSENT_SUBS = 27,
   TCAP_RET_ERR_FACILITY_NOT_SUPPORTED = 21,
   TCAP_RET_ERR_SYS_FAILURE = 34,
   TCAP_RET_ERR_DATA_MISSING = 35,
   TCAP_RET_ERR_UNEXPECTED_DATA = 36
};

enum EnumTcapApiError
{
   API_ERR_INCORRECT_MESSAGE_LENGTH = 0x01,
   API_ERR_MISSING_MANDATORY_PARAMETER = 0x02,
   API_ERR_INVALID_IE_LENGTH = 0x03,
   API_ERR_BAD_IE_PARAMETER_VALUE = 0x04,
   API_ERR_NO_FREE_DIALOGUES = 0x05,
   API_ERR_INVALID_DIALOGUE_ID = 0x06,
   API_ERR_MAX_COMPONENT_LENGTH_EXCEEDED = 0x07,
   API_ERR_BAD_MESSAGE_PARAMETER = 0x08,
   API_ERR_WRONG_TRANSACTION = 0x09,
   API_ERR_DIALOGUE_UNAVAILABLE = 0x0a,
   API_ERR_INVOKE_IN_USE = 0x0b
};

enum EnumTcapPegEvents
{
   PEG_TCAP_MSU_SENT = 1,
   PEG_TCAP_MSU_RECV
};

struct TcapAbortComp
{
   EnumAbortType           abortType;
   TDArray                 abortReason;
};

struct TcapRejectComp
{
   EnumRejectType          rejectType;
   TCAPProblem             problem;
};

#define SS7_MAX_TCAP_ERROR_CODE_DATA_LEN  150
struct TcapErrorCodeData
{
   int                     numberOfBytes;
   UINT8                   array[SS7_MAX_TCAP_ERROR_CODE_DATA_LEN +1];
};

struct TcapErrorCode
{
   UINT8                   errorType;
   TcapErrorCodeData       errorCodeData;
};

//Updated it to support private or other opcode.
struct AnsiTcapErrorCode
{
   UINT32                  errorCode;
   TcapErrorCodeData       errorCodeData;
   bool                    isPrivate;
};

struct TcapRetErrorParamPdu
{
   int                     numberOfBytes;
   UINT8                   array[SS7_MAX_TCAP_ERROR_CODE_DATA_LEN +1];
};

struct TcapRetErrorComp
{
   TcapErrorCode           errorCode;
   TcapRetErrorParamPdu    parameterData;
};

struct AnsiTcapRetErrorComp
{
   AnsiTcapErrorCode           errorCode;
   TcapRetErrorParamPdu    parameterData;
};

struct TcapRetResultComp
{
   EnumRetResLastIndicator lastIndicator;
   TCAPOperation           operation;
   TDArray                 parameterData;
};

struct TcapInvokeComp
{
   int timeout;
   TCAPOperation           operation;
   TCAPLinkedId            linkedId;
   TDArray                 parameterData;
};

struct TcapComponent
{
   int invokeId;
   EnumTcapLastComp        lastComponent;
   EnumTcapComp            tcapComp;
   union
   {
      TcapInvokeComp       tcapInvokeComp;
      TcapRetResultComp    tcapRetResultComp;
      TcapRetErrorComp     tcapRetErrorComp;
      TcapRejectComp       tcapRejectComp;
      TcapAbortComp        tcapAbortComp;
   };
};

struct AnsiTcapComponent
{
   int invokeId;
   EnumTcapLastComp        lastComponent;
   EnumTcapComp            tcapComp;
   union
   {
      TcapInvokeComp       tcapInvokeComp;
      TcapRetResultComp    tcapRetResultComp;
      AnsiTcapRetErrorComp     tcapRetErrorComp;
      TcapRejectComp       tcapRejectComp;
      TcapAbortComp        tcapAbortComp;
   };
};

#define SS7_MAX_USER_INFO_PDU_LEN 50
struct TcapUserInfoPdu
{
   int                     numberOfBytes;
   UINT8                   array[SS7_MAX_USER_INFO_PDU_LEN + 1];
};

#define SS7_MAX_ACN_PDU_LEN 10
struct TcapAcnPdu
{
   int                     numberOfBytes;
   UINT8                   array[SS7_MAX_ACN_PDU_LEN + 1];
};

//Added By Abhishek - Generic routing info for Kafka bridging (shared by TCAP and SCCP).
struct KafkaRoutingInfo
{
   char stackId[TCAP_STACK_ID_LEN];
   char appId[TCAP_APP_ID_LEN];
   char replyTopic[TCAP_REPLY_TOPIC_LEN];
   int  partition;

   void reset()
   {
      partition = 0;
      memset(stackId, 0, TCAP_STACK_ID_LEN);
      memset(appId, 0, TCAP_APP_ID_LEN);
      memset(replyTopic, 0, TCAP_REPLY_TOPIC_LEN);
   }

   // Utility to check if routing is valid
   bool isPopulated() const 
   {
      return stackId[0] != '\0';
   }
};

struct TcapMsg
{
   //EnumSs7Standard         standard;
   int                     ssn;
   int                     dialogueId;
   int                     tcUserId;
   UINT8                   origTransIdLen;
   UINT8                   destTransIdLen;
   UINT32                  origTransId;
   UINT32                  destTransId;
   TCAPAddress             destAddress;
   TCAPAddress             origAddress;
   TcapAcnPdu              applicationContext;
   TcapUserInfoPdu         userInformation;
   EnumTcapDlg             tcapDlg;
   BOOLEAN                 componentPresent;
   TcapComponent           tcapComponent;

   // ANSI - SPECIFIC ADDITIONS
   //bool                    isAnsiNumericContext;
   //int                     ansiApplicationContext;
   // ---------------------------------
#ifdef SS7_TIMESTAMP
   struct timespec         timeStamp;
#endif
   // Extended TcapMsg including routing metadata for Kafka/Application layer interaction.
#ifdef KAFKA_BRIDGE 
   KafkaRoutingInfo         routing;
#endif

};

struct AnsiTcapMsg
{
   int                     ssn;
   int                     dialogueId;
   int                     tcUserId;
   UINT8                   origTransIdLen;
   //UINT8                 destTransIdLen;
   UINT32                  origTransId;
   //UINT32                destTransId;
   TCAPAddress             destAddress;
   TCAPAddress             origAddress;
   EnumTcapDlg             tcapDlg;
   BOOLEAN                 componentPresent;
   AnsiTcapComponent       tcapComponent;
#ifdef SS7_TIMESTAMP
   struct timespec         timeStamp;
#endif
   // Extended AnsiTcapMsg including routing metadata for Kafka/Application layer interaction.
#ifdef KAFKA_BRIDGE 
   KafkaRoutingInfo         routing;
#endif
};


//--------------------------------------------------------------------//
//--------------------- Old Tcap Structure ---------------------------//
//--------------------------------------------------------------------//
struct __TcapInvokeComp
{
#ifdef SS7_1_4
   int                     timeout;
#endif
   TCAPOperation           operation;
   TCAPLinkedId            linkedId;
   TDArray                 parameterData;
};

struct __TcapRetErrorComp
{
   TCAPErrorCode           errorCode;
   TDArray                 parameterData;
};

struct __TcapComponents
{
   int                     invokeId;
   EnumTcapLastComp        lastComponent;
   EnumTcapComp            tcapComp;
   union
   {
      __TcapInvokeComp     tcapInvokeComp;
      TcapRetResultComp    tcapRetResultComp;
      __TcapRetErrorComp   tcapRetErrorComp;
      TcapRejectComp       tcapRejectComp;
      TcapAbortComp        tcapAbortComp;
   };
};

#define MAX_TCAP_COMPONENTS         4
struct __TcapMsg
{
   int                     ssn;
   int                     dialogueId;
#ifdef SS7_1_4
   int                     tcUserId;
#endif
   TCAPAddress             address;
#ifdef SS7_1_4
   TCAPAddress             origAddr;
#endif
   TDArray                 applicationContext;
   TDArray                 userInformation;
   EnumTcapDlg             tcapDlg;
   int                     numOfComponents;
   __TcapComponents        tcapComponent[MAX_TCAP_COMPONENTS];
};

enum EnumAcuTcapAbortReason
{
    TCAP_AARE_ACCEPTED_USER             = 1,
    TCAP_AARE_ACCEPTED_PROVIDER         = 2,
    TCAP_AARE_REJECT_USER_NULL          = 3,
    TCAP_AARE_REJECT_USER_NO_REASON     = 4,
    TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED
                                        = 5,
    TCAP_AARE_REJECT_PROVIDER_NULL      = 6,
    TCAP_AARE_REJECT_PROVIDER_NO_REASON = 7,
    TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION
                                        = 8 
};

#endif
