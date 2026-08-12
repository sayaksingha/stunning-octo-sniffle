// @(#) $Id: MsuStructs.h,v 1.1.4.2.14.1 2021/12/01 12:43:28 jamesjac Exp $
//----------------------------------------------------------------------
// NAME
// MsuStructs.h
// 
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2001
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Joel Baby Jose                               20-AUG-2007
//----------------------------------------------------------------------
#ifndef INCL_SS7_MSU_STRUCTS_H
#define INCL_SS7_MSU_STRUCTS_H

#include "Ss7Structs.h"
#include "Ss7BerEncDec.h"
#include "TcapStructs.h"     // Ab Comment: Required for KafkaRoutingInfo (KAFKA_BRIDGE)

#define SS7_ORIG_TRANS_ID_TAG  0x48
#define SS7_DEST_TRANS_ID_TAG  0x49

#define SS7_DLG_PORTION_TAG    0x6B
#define SS7_COMP_PORTION_TAG   0x6C

#define SS7_INVOKE_ID_TAG        0x02
#define SS7_LINKED_ID_TAG        0x80

#define SS7_LOCAL_OP_CODE_TAG    0x02
#define SS7_GLOBAL_OP_CODE_TAG   0x06

#define SS7_LOCAL_ERR_CODE_TAG   0x02
#define SS7_GLOBAL_ERR_CODE_TAG  0x06

//For Dialouge Portion
#define SS7_DLG_TAG                         0x6B
#define SS7_DLG_EXTERNAL_TAG                0x28
#define SS7_DLG_OBJECT_ID                   0x06
#define SS7_DLG_SINGLE_ASN_TAG              0xA0
#define SS7_DLG_REQUEST_TAG                 0x60
#define SS7_DLG_RESPONSE_TAG                0x61
#define SS7_DLG_ABORT_TAG                   0x64
#define SS7_DLG_ABORT_SOURCE_TAG            0x80
#define SS7_DLG_PROTOCOL_VERSION_TAG        0x80
#define SS7_DLG_ACN_NAME_TAG                0xA1
#define SS7_DLG_ACN_OBJECT_ID               0x06
#define SS7_DLG_RESULT_TYPE_TAG             0xA2
#define SS7_DLG_ASSOCIATE_RESULT_TAG        0x02
#define SS7_DLG_RES_SRC_DIAGNOSTIC_TAG      0xA3
#define SS7_DLG_SERVICE_USER_TAG            0xA1
#define SS7_DLG_SERVICE_USER_VALUE_TAG      0x02
#define SS7_DLG_SERVICE_PROVIDER_TAG        0xA2
#define SS7_USER_INFO_TAG                   0xBE
#define SS7_USER_INFO_EXTERNAL_TAG          0x28
#define SS7_USER_INFO_OBJECT_ID             0x06
#define SS7_USER_INFO_SINGLE_ASN_TAG        0xA0
#define SS7_MAP_USER_ABORT_TAG              0xA4
#define SS7_APP_PROC_CAN_TAG                0x83
#define SS7_MAP_PROV_ABORT_TAG              0xA5
#define SS7_DLG_SERVICE_PROV_TAG            0xA2
#define SS7_REJECT_PERM                     0x01
#define SS7_MAP_USER_ABORT_TAG              0xA4
#define SS7_MAP_PROVIDER_ABORT_TAG          0xA5

enum EnumSccpMsgType
{
   SCCP_MSG_CR    = 1,
   SCCP_MSG_CC    = 2,
   SCCP_MSG_CREF  = 3,
   SCCP_MSG_RLSD  = 4,
   SCCP_MSG_RLC   = 5,
   SCCP_MSG_DT1   = 6,
   SCCP_MSG_DT2   = 7,
   SCCP_MSG_AK    = 8,
   SCCP_MSG_UDT   = 9,
   SCCP_MSG_UDTS  = 10,
   SCCP_MSG_ED    = 11,
   SCCP_MSG_EA    = 12,
   SCCP_MSG_RSR   = 13,
   SCCP_MSG_RSC   = 14,
   SCCP_MSG_ERR   = 15,
   SCCP_MSG_IT    = 16,
   SCCP_MSG_XUDT  = 17,
   SCCP_MSG_XUDS  = 18
};

struct _AddressIndicator
{
   union
   {
      UINT8 addrIndicator;
      struct
      {
         UINT8 pc:1;
         UINT8 ssn:1;
         UINT8 gt:4;
         UINT8 ri:1;
         UINT8 fnu:1;
      };
   };
};

enum EnumTcapPblmType
{
   TCAP_PBLM_TYPE_GENERAL = 0x80,
   TCAP_PBLM_TYPE_INVOKE = 0x81,
   TCAP_PBLM_TYPE_RET_RESULT = 0x82,
   TCAP_PBLM_TYPE_RET_ERROR = 0x83
};

enum EnumTcapPblmCodeGeneral
{
   TCAP_PBLM_GEN_UNRECOGNIZED_COMPONENT = 0,
   TCAP_PBLM_GEN_MISTYPED_COMPONENT = 1,
   TCAP_PBLM_GEN_BADLY_STRUCTURED_COMPONENT = 2
};

enum EnumTcapPblmCodeInvoke
{
   TCAP_PBLM_INV_DUPLICATE_INVOKE_ID = 0,
   TCAP_PBLM_INV_UNRECOGNIZED_OPERATION = 1,
   TCAP_PBLM_INV_MISTYPED_PARAMETER = 2,
   TCAP_PBLM_INV_RESOURCE_LIMITATION = 3,
   TCAP_PBLM_INV_INITIATING_RELEASE = 4,
   TCAP_PBLM_INV_UNRECOGNIZED_LINKED_ID = 5,
   TCAP_PBLM_INV_UNEXPECTED_LINKED_RESPONSE = 6,
   TCAP_PBLM_INV_UNEXPECTED_LINKED_OPERATION = 7
};

enum EnumTcapPblmCodeReturnResult
{
   TCAP_PBLM_RET_RES_UNASSIGNED_INVOKE_ID = 0,
   TCAP_PBLM_RET_RES_UNEXPECTED_RETURN_RESULT = 1,
   TCAP_PBLM_RET_RES_MISTYPED_PARAMETER = 2
};

enum EnumTcapPblmCodeReturnError
{
   TCAP_PBLM_RET_ERR_UNASSIGNED_INVOKE_ID = 0,
   TCAP_PBLM_RET_ERR_UNEXPECTED_RETURN_ERROR = 1,
   TCAP_PBLM_RET_ERR_UNRECOGNIZED_ERROR = 2,
   TCAP_PBLM_RET_ERR_UNEXPECTED_ERROR = 3,
   TCAP_PBLM_RET_ERR_MISTYPED_PARAMETER = 4
};

struct _TcapRejectComp
{
   //EnumTcapRejectType  rejectType;
   TCAPProblem problem;
};

struct _TcapRetErrorComp
{
   TCAPErrorCode errorCode;
   TDArray paramData;
};

struct _TcapRetResultComp
{
   TCAPOperation operation;
   TDArray paramData;
};

struct _TcapInvokeComp
{
   TCAPOperation operation;
   TCAPLinkedId linkedId;
   TDArray paramData;
};

enum EnumTcapCompType
{
   TCAP_COMP_INVOKE = 0xA1,
   TCAP_COMP_RET_REULT_LAST = 0xA2,
   TCAP_COMP_RET_ERR = 0xA3,
   TCAP_COMP_REJECT = 0xA4,
   TCAP_COMP_RET_RESULT = 0xA7
};

enum EnumAbortCompType
{
   TCAP_U_ABRT = 0x6B,
   TCAP_P_ABRT = 0x4A,
   TCAP_NULL_ABRT = 0
};

enum EnumPAbortReason
{
   TCAP_UNRECOGNIZED_TYPE = 0,
   TCAP_UNRECOGNIZED_TID = 1,
   TCAP_BADLY_FORMATED_TRANS_PORTION = 2,
   TCAP_INCORRECT_TRANS_PORTION = 3,
   TCAP_RESRC_LIMITATION = 4
};

struct _TcapAbortComp
{
   EnumAbortCompType abortCompType;
   TDArray abortCompReason;
};

struct _TcapCompInfo
{
   int invokeId;
   //EnumTcapLastComp       lastComponent;
   EnumTcapCompType tcapComp;
   union
   {
      _TcapInvokeComp invoke;
      _TcapRetResultComp retResult;
      _TcapRetErrorComp retErr;
      _TcapRejectComp reject;
      _TcapAbortComp abort;
   };
};

struct _TcapDlgInfo
{
   TDArray dlgPdu;
};

enum EnumTcapTransType
{
   TCAP_TRANS_UNIDIRECTIONAL = 0x61,
   TCAP_TRANS_BEGIN = 0x62,
   TCAP_TRANS_END = 0x64,
   TCAP_TRANS_CONTINUE = 0x65,
   TCAP_TRANS_ABORT = 0x67
};

struct _TransactionInfo
{
   EnumTcapTransType pkgType;
   UINT8 origTransIdLen;
   UINT8 destTransIdLen;
   UINT32 origTransId;
   UINT32 destTransId;
};

struct _SccpUdt
{
   union
   {
      UINT8 pcMsgHdlg;
      struct
      {
         UINT8 protoClass:4;
         UINT8 msgHdlg:4;
      };
   };

   TCAPAddress cldPartyAddress;
   TCAPAddress clgPartyAddress;
   _TransactionInfo transInfo;
   _TcapDlgInfo dlgInfo;
   _TcapCompInfo compInfo;
};

struct _SccpInfo
{
   UINT8 msgType;
   union
   {
      _SccpUdt udt;
   };
#ifdef SS7_TIMESTAMP
   struct timespec         timeStamp;
#endif
   // Ab Comment: ALWAYS include routing when KAFKA_BRIDGE is defined to avoid
   // IPC sizing mismatches between binaries (sccp_ansi and KafkaBridge).
   // Both must be compiled with the same flag set.
#ifdef KAFKA_BRIDGE
   KafkaRoutingInfo         routing;
#endif
};

struct _RoutingLabel
{
   UINT16 dpc:14;
   UINT16 opc:14;
   UINT8 sls:4;
};

struct _Sio
{
   union
   {
      UINT8 sio;
      struct
      {
         UINT8 si:4;
         UINT8 priority:2;
         UINT8 ni:2;
      };
   };
};

struct _MtpInfo
{
   _Sio sio;
   _RoutingLabel routingLabel;
};

struct _MsuInfo
{
   int channel;
   _MtpInfo mtpInfo;
   union
   {
      _SccpInfo sccpInfo;
   };
};

#define SS7_MAX_MSU_LEN        300

struct _L2Pdu
{
   int channel;
   int dpc;
   int opc;
   int length;
   UINT8 msu[SS7_MAX_MSU_LEN + 1];
};

#endif
