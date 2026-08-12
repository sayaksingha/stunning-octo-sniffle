// @(#) $Id: Ss7Structs.h,v 1.1.4.4 2014/06/27 20:15:29 mram Exp $
//----------------------------------------------------------------------
// NAME 
// Ss7Structs.h
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
// Originated: Joel Baby Jose                                28-AUG-2008
//----------------------------------------------------------------------
#ifndef INCL_SS7_STRUCTS_DEF_H
#define INCL_SS7_STRUCTS_DEF_H

#include "Ss7ConstDef.h"

#ifndef TDAPI_H
// Defining TDArray structures to remove dependency on Telesoft structure

typedef unsigned char   Byte;
typedef unsigned short  Word;
typedef unsigned int Uint32;


#define MAX_TDADDRESS_DIGITS   20
typedef struct {
	Byte  typeOfNumber;
	Byte  numberingPlan;
	Byte  numberOfDigits;
	Byte  digits[MAX_TDADDRESS_DIGITS];
} TDAddress;


#define MAX_TDARRAY_BYTES      255
#define MAX_TD4KARRAY_BYTES      4096

typedef struct {
	int      numberOfBytes;
   UINT8  array[MAX_TDARRAY_BYTES];
} TDArray;

typedef struct {
   int      numberOfBytes;
   UINT8  array[MAX_TD4KARRAY_BYTES];
} TD4kArray;

#define MAX_GLOBAL_TITLE_DIGITS  20


// Data types 
typedef struct {
   UINT8  addressIndicator;
   UINT8  subsystemNumber;
   int      pointCode;
   UINT8  natureOfAddress;
   UINT8  translationType;
   UINT8  numberingPlan;
   UINT8  encodingScheme;
   UINT8  numberOfDigits;
   UINT8  digits[ MAX_GLOBAL_TITLE_DIGITS ];
} SCCPAddress;

typedef SCCPAddress TCAPAddress;

typedef struct {
   UINT8 member;
   UINT8 cluster;
   UINT8 network;
} AnsiPointCode;

typedef struct {
   UINT8  linkedId;
} TCAPLinkedId;

typedef struct {
   UINT8  problemCodeType;
   UINT8  problemCode;
} TCAPProblem;

typedef struct {
   UINT8  errorType;
   TDArray  errorCodeData;
} TCAPErrorCode;

#define TCAP_OPERATION_MAX_OID   64
typedef struct {
   UINT8  operationFamily;
   int    operationCode;
   int      globalValueLength;
   UINT8  globalValue[TCAP_OPERATION_MAX_OID];
} TCAPOperation;

typedef struct {
   UINT8  returnOption;
   UINT8  sequenceControlOption;
   UINT8  sequenceControlValue;
} TCAPQualityOfService;

typedef struct {
   Uint32   dialogueId;
   Uint32   owner;
   long  creationTime;
   UINT8  tslState;
   UINT8  cslState;
} TCAPDialogueInfo;
#endif
//#include "Ss7BerEncDec.h"

////////////////////////////////////////////////////////////////////////
//---------------------------- Ss7 Peg Details -----------------------//
////////////////////////////////////////////////////////////////////////
enum EnumSs7PegEvents
{
   // Tcap Handler
   PEG_TCAP_BEGIN_TX             = 61,
   PEG_TCAP_CONTINUE_TX          = 62,
   PEG_TCAP_END_TX               = 63,
   PEG_TCAP_USER_ABORT_TX        = 64,

   PEG_TCAP_INVOKE_COMP_TX       = 65,
   PEG_TCAP_RET_RESULT_COMP_TX   = 66,
   PEG_TCAP_RET_ERROR_COMP_TX    = 67,
   PEG_TCAP_REJECT_COMP_TX       = 68,

   PEG_TCAP_RSP_TIMEOUT_RX       = 69,

   PEG_TCAP_BEGIN_RX             = 70,
   PEG_TCAP_CONTINUE_RX          = 71,
   PEG_TCAP_END_RX               = 72,
   PEG_TCAP_USER_ABORT_RX        = 73,
   PEG_TCAP_PROTOCOL_ABORT_RX    = 74,
   PEG_TCAP_NULL_ABORT_RX        = 75,
   PEG_TCAP_UNIDIRECTIONAL_RX    = 76,

   PEG_TCAP_INVOKE_COMP_RX       = 77,
   PEG_TCAP_RET_RESULT_COMP_RX   = 78,
   PEG_TCAP_RET_ERROR_COMP_RX    = 79,
   PEG_TCAP_REJECT_COMP_RX       = 80
};

////////////////////////////////////////////////////////////////////////
//---------------------------- MTP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumMtpNetworkIndicator
{
   NI_INTERNATIONAL_NETWORK = 0,
   NI_SPARE = 2,
   NI_NATIONAL_NETWORK = 3,
   NI_RESERVED_FOR_NATIONAL = 4
};

enum EnumMtpServiceIndicator
{
   SI_SNM = 0,
   SI_MTN = 1,
   SI_MTNS = 3,
   SI_SCCP = 4,
   SI_TUP = 5,
   SI_ISUP = 6,
};

////////////////////////////////////////////////////////////////////////
//--------------------------- SCCP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumSccpGlobalTitleIndicator
{
   SCCP_GTI_NO_GLOBAL_TITLE = 0,
   SCCP_GTI_NAI = 1,
   SCCP_GTI_TTYPE = 2,
   SCCP_GTI_TTYPE_NP_ENCODING_SCHEME = 3,
   SCCP_GTI_TTYPE_NP_ENCODING_SCHEME_NAI = 4,
};

enum EnumSccpRoutingIndicator
{
   SCCP_RI_ROUTE_ON_GT = 0,
   SCCP_RI_ROUTE_ON_SSN = 1
};

//------------------------- Sccp Address Indicator -------------------//
#define AI_GT        16		//0x10 // Route on GT. PC/SSN Absent
#define AI_GT_PC     17		//0x11 // Route on GT. PC Present
#define AI_GT_SSN    18		//0x12 // Route on GT. SSN present
#define AI_GT_PC_SSN 19		//0x13 // Route on GT. PC/SSN present

#define AI_SSN       66		//0x42 // Route on SSN. GT/PC Absent
#define AI_SSN_PC    67		//0x43 // Route on SSN. PC Present, GT Absent
#define AI_SSN_GT_PC 83		//0x53 // Route on SSN. GT/PC present

enum EnumSccpSubsystemNumber
{
   SCCP_SSN_UNKNOWN = 0,
   SCCP_SSN_MANAGEMENT = 1,
   SCCP_SSN_ISUP = 3,
   SCCP_SSN_OMAP = 4,
   SCCP_SSN_MAP = 5,
   SCCP_SSN_HLR = 6,
   SCCP_SSN_VLR = 7,
   SCCP_SSN_MSC = 8,
   SCCP_SSN_EIC = 9,
   SCCP_SSN_AUC = 10,
   SCCP_SSN_ISDN_SS = 11,
   SCCP_SSN_TC_TEST_REPONDER = 16,
   SCCP_SSN_CAP = 146,
   SCCP_SSN_GSM_SCF = 147,
   SCCP_SSN_SGSN = 149
};

//enum EnumSccpOddEvenIndicator
//{
//   SCCP_ODD_NUMBER_OF_ADDRESS_SIGNALS = 1,
//   SCCP_EVEN_NUMBER_OF_ADDRESS_SIGNALS = 0
//};

enum EnumIsdnNoaIndicator
{
   ISDN_NAI_UNKNOWN = 0,
   ISDN_NAI_INTERNATIONAL_NUMBER = 1,
   ISDN_NAI_NATIONAL_SIGNIFICANT_NUMBER = 2,
   ISDN_NAI_NATIONAL_USE = 3,
   ISDN_NAI_SUBSCRIBER_NUMBER = 4,
   ISDN_NAI_ALPHANUMERIC = 5,
   ISDN_NAI_ABBREVIATED_NUMBER = 6
};

// Nature of Address Indicator
enum EnumSs7NoaIndicator
{
   SCCP_NAI_UNKNOWN = 0,
   SCCP_NAI_SUBSCRIBER_NUMBER = 1,
   SCCP_NAI_NATIONAL_USE = 2,
   SCCP_NAI_NATIONAL_SIGNIFICANT_NUMBER = 3,
   SCCP_NAI_INTERNATIONAL_NUMBER = 4,
   SCCP_NAI_ALPHANUMERIC = 5
};

// Numbering Plan
enum EnumSs7NumberingPlan
{
   SCCP_NP_UNKNOWN = 0,
   SCCP_NP_ISDN_TELEPHONY = 1,	//E.164
   SCCP_NP_GENERIC = 2,
   SCCP_NP_DATA = 3,		//X.121
   SCCP_NP_TELEX = 4,		//F.69
   SCCP_NP_MARITIME_MOBILE = 5,	//E.211
   SCCP_NP_LAND_MOBILE = 6,	//E.212
   SCCP_NP_ISDN_MOBILE = 7	//E.214
};

enum EnumSccpEncodingScheme
{
   SCCP_ES_UNKNOWN = 0,
   SCCP_ES_BCD_ODD = 1,
   SCCP_ES_BCD_EVEN = 2
};

struct NoaNp
{
   union
   {
      UINT8 noaNp;
      struct
      {
         EnumSs7NumberingPlan numberingPlan:4;
         EnumIsdnNoaIndicator natureOfAddress:3;
         UINT8 extension:1;
      };      
   };
};

struct ISDNAddressString
{
   NoaNp noaNp;
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

enum EnumScreenInd
{
  CP_SCREENING_USER_PROVIDED_NOT_VERIFIED = 0,
  CP_SCREENING_USER_PROVIDED_VERIFIED     = 1,
  CP_SCREENING_USER_PROVIDED_FAILED       = 2,
  CP_SCREENING_NETWORK_PROVIDED           = 3
};

enum EnumPresentation
{
  CP_PRESENTATION_ALLOWED                = 0,
  CP_PRESENTATION_RESTRICTED             = 1,
  CP_PRESENTATION_ADDRESS_NOT_AVAILABLE  = 2,
  CP_PRESENTATION_SPARE                  = 3
};

enum EnumNumPlanInd
{
  CP_NUMBERING_PLAN_SPARE_1    = 0,
  CP_NUMBERING_PLAN_ISDN       = 1,
  CP_NUMBERING_PLAN_SPARE_2    = 2,
  CP_NUMBERING_PLAN_DATA       = 3,
  CP_NUMBERING_PLAN_TELEX      = 4,
  CP_NUMBERING_PLAN_NATIONAL_1 = 5,
  CP_NUMBERING_PLAN_NATIONAL_2 = 6,
  CP_NUMBERING_PLAN_SPARE_3    = 7,

};

enum EnumNumIncInd
{
  CP_NUMBER_COMPLETE   = 0,
  CP_NUMBER_INCOMPLETE = 1
};

struct ScreenClip
{
   union 
   {
      UINT8 screenClip;
      struct
      {
         EnumScreenInd    screenInd:2;
         EnumPresentation presentationInd:2;
         EnumNumPlanInd   numPlanInd:3;
         EnumNumIncInd    numIncInd:1;
      };
   };
};

////////////////////////////////////////////////////////////////////////
//--------------------------- TCAP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum TcapClassOfService
{
   TCAP_CLASS_OF_SERVICE_1 = 1,
   TCAP_CLASS_OF_SERVICE_2 = 2,
   TCAP_CLASS_OF_SERVICE_3 = 3,
   TCAP_CLASS_OF_SERVICE_4 = 4
};

enum EnumTcapProblemCodeType
{
   TCAP_PROB_TYP_GENERAL = 1,
   TCAP_PROB_TYP_INVOKE = 2,
   TCAP_PROB_TYP_RETURN_RESULT = 3,
   TCAP_PROB_TYP_RETURN_ERROR = 4,
   TCAP_PROB_TYP_NA = 0xFF
};

enum EnumTcapProblemCodeGeneral
{
   TCAP_PROB_GEN_UNRECOGNIZED_COMPONENT = 0,
   TCAP_PROB_GEN_MISTYPED_COMPONENT = 1,
   TCAP_PROB_GEN_BADLY_STRUCTURED_COMPONENT = 2
};

enum EnumTcapProblemCodeInvoke
{
   TCAP_PROB_INV_DUPLICATE_INVOKE_ID = 0,
   TCAP_PROB_INV_UNRECOGNIZED_OPERATION = 1,
   TCAP_PROB_INV_MISTYPED_PARAMETER = 2,
   TCAP_PROB_INV_RESOURCE_LIMITATION = 3,
   TCAP_PROB_INV_INITIATING_RELEASE = 4,
   TCAP_PROB_INV_UNRECOGNIZED_LINKED_ID = 5,
   TCAP_PROB_INV_UNEXPECTED_LINKED_RESPONSE = 6,
   TCAP_PROB_INV_UNEXPECTED_LINKED_OPERATION = 7
};

enum EnumTcapProblemCodeReturnResult
{
   TCAP_PROB_RET_RES_UNASSIGNED_INVOKE_ID = 0,
   TCAP_PROB_RET_RES_UNEXPECTED_RETURN_RESULT = 1,
   TCAP_PROB_RET_RES_MISTYPED_PARAMETER = 2
};

enum EnumTcapProblemCodeReturnError
{
   TCAP_PROB_RET_ERR_UNASSIGNED_INVOKE_ID = 0,
   TCAP_PROB_RET_ERR_UNEXPECTED_RETURN_ERROR = 1,
   TCAP_PROB_RET_ERR_UNRECOGNIZED_ERROR = 2,
   TCAP_PROB_RET_ERR_UNEXPECTED_ERROR = 3,
   TCAP_PROB_RET_ERR_MISTYPED_PARAMETER = 4
};

////////////////////////////////////////////////////////////////////////
//--------------------------- ISUP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumRelCallCause
{
   RC_NORMAL = 0x00,
   RC_UNALLOCATED_UNASSIGNED_NUMBER = 0x01,
   RC_NO_ROUTE_TO_SPECIFIED_TRANSIT_NW = 0x02,
   RC_NO_ROUTE_TO_DESTINATION = 0x03,
   RC_SEND_SPECIAL_INFO_TONE = 0x04,
   RC_MISDIALLED_TRUNK_PREFIX = 0x05,
   RC_CHANNEL_UNACCEPTABLE = 0x06,
   RC_CALL_AWARDED_AND_BEING_DELIVERED_IN_EST_CHANNEL = 0x07,
   RC_PREEMPTION = 0x08,
   RC_PREEMPTION_CIRCUIT_RESERVED_FOR_REUSE = 0x09,
   RC_NORMAL_CALL_CLEARING = 0x10,
   RC_USER_BUSY = 0x11,
   RC_NO_USER_RESPONDING = 0x12,
   RC_NO_ANSWER_FROM_USER = 0x13,
   RC_SUBSCRIBER_ABSENT = 0x14,
   RC_CALL_REJECTED = 0x15,
   RC_NUMBER_CHANGED = 0x16,
   RC_NOON_SELECETD_USER_CLEARING = 0x1A,
   RC_DESTINATION_OUT_OF_ORDER = 0x1B,
   RC_ADDRESS_INCOMPLETE = 0x1C,
   RC_FACILITY_REJECTED = 0x1D,
   RC_RESPONSE_TO_STATUS_ENQUIRY = 0x1E,
   RC_NORMAL_UNSPECIFIED = 0x1F,
   RC_NO_CIRCUIT_CHANNEL_AVAILABLE = 0x22,
   RC_NETWORK_OUT_OF_ORDER = 0x26,
   RC_PERMANENET_FRAME_MODE_CONNECTION_OUT_OF_SERVICE = 0x27,
   RC_PERMANENET_FRAME_MODE_CONNECTION_OPERATIONAL = 0x28,
   RC_TEMPORARY_FAILURE = 0x29,
   RC_SWITCHING_EQUIPMENT_CONGESTION = 0x2A,
   RC_ACCESS_INFORMATION_DISCARDED = 0x2B,
   RC_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE = 0x2C,
   RC_PRECEDENCE_CALL_BLOCKED = 0x2E,
   RC_RESOURCE_UNAVAILABLE_UNSPECIFIED = 0x2F,
   RC_QUALITY_OF_SERVICE_UNAVAILABLE = 0x31,
   RC_REQUESTED_FACILITY_NOT_SUBSCRIBED = 0x32,
   RC_OUTGOING_CALLS_BARRED_WITHIN_CUG = 0x35,
   RC_INCOMING_CALLS_BARRED_WITHIN_CUG = 0x37,
   RC_BEARER_CAPABILITY_NOT_AUTHORIZED = 0x39,
   RC_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE = 0x3A,
   RC_INCONSISTENCY_IN_DESIGNATED_OUTGOING_ACCESS_INFO_AND_SUBS_CLASS = 0x3E,
   RC_SERVICE_OR_OPTION_NOT_AVAILABLE_UNSPECIFIED = 0x3F,
   RC_BEARER_CAPABILITY_NOT_IMPLEMENTED = 0x41,
   RC_CHANNEL_TYPE_NOT_IMPLEMENTED = 0x42,
   RC_REQUESTED_FACILITY_NOT_IMPLEMENTED = 0x45,
   RC_ONLY_RESTRICTED_DIGITAL_INFO_BEARER_CAPABILITY_IS_AVBLE = 0x46,
   RC_SERVICE_OR_OPTION_NOT_IMPLEMENTED_UNSPECIFIED = 0x4F,
   RC_INVALID_CALL_REFERENCE_VALUE = 0x51,
   RC_DENFIED_CHANNEL_DOES_NOT_EXIST = 0x52,
   RC_A_SUSPENDED_CALL_EXISTS_BUT_THIS_CALL_IDENTITY_DOES_NOT = 0x53,
   RC_CALL_IDENTITY_IN_USE = 0x54,
   RC_NO_CALL_SUSPENDED = 0x55,
   RC_CALL_HAVING_THE_REQUESTED_CALL_IDENTITY_HAS_BEN_CLEARED = 0x56,
   RC_USER_NOT_MEMBER_OF_CUG = 0x57,
   RC_INCOMPATIBLE_DESTINATION = 0x58,
   RC_NON_EXISTENT_CUG = 0x5A,
   RC_INVALID_TRANSIT_NETWORK_SELECTION = 0x5B,
   RC_INVALID_MESSAGE_UNSPECIFIED = 0x5F,
   RC_MANDATORY_INFORMATION_ELEMENT_IS_MISSING = 0x60,
   RC_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED = 0x61,
   RC_MSg_NOT_COMPATIBLE_WITH_CALL_STATE_MSG_TYPE_NON_EXISTENT_NOT_IMPL = 0x62,
   RC_INVALID_INFO_ELEMENT_CONTENTS = 0x64,
   RC_MESSAGE_NOT_COMPATIBLE_WITH_CALL_STATE = 0x65,
   RC_RECOVERY_ON_TIMER_EXPIRY = 0x66,
   RC_PARAMETER_NON_EXISTENT_OR_NOT_IMPLEMENTED_PASSED = 0x67,
   RC_MESSAGE_WITH_UNRECOGNIZED_PARAMETER_DISCARDED = 0x6E,
   RC_PROTOCOL_ERROR_UNSPECIFIED = 0x6F,
   RC_INTERWORKING_UNSPECIFIED = 0x7F
};

enum EnumIsupOddEvenIndicator
{
   ISUP_OE_EVEN = 0,
   ISUP_OE_ODD = 1
};

enum EnumIsupScreeningIndicator
{
   ISUP_SI_USER_PROVIDED = 1,
   ISUP_SI_NW_PROVIDED = 3
};

enum EnumIsupAddrPresentationRestrictInd
{
   ISUP_APRI_ALLOWED = 0,
   ISUP_APRI_RESTRICTED = 1,
   ISUP_APRI_ADDRESS_NOT_AVAILABLE = 2
};

enum EnumIsupNumberIncompleteInd
{
   ISUP_NI_COMPLETE = 0,
   ISUP_NI_INCOMPLETE = 1
};

enum EnumIsupInnIndicator
{
   ISUP_INN_ALLOWED = 0,
   ISUP_INN_NOT_ALLOWED = 1
};

struct ISUPCallingPartyNumber // Q.763
{
   union
   {
      UINT8 oddEvenAndNoap;
      struct
      {
         EnumSs7NoaIndicator natureOfAddress:7;
         UINT8 oddEvenIndicator:1;
      };
   };
   union
   {
      UINT8 SsIndAndNonp;
      struct
      {
         EnumIsupScreeningIndicator screeningIndicator:2;
         EnumIsupAddrPresentationRestrictInd restrictInd:2;
         EnumSs7NumberingPlan numberingPlan:3;
         EnumIsupNumberIncompleteInd numberIncompleteInd:1;
      };
   };
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

struct ISUPCalledPartyNumber //EN 300 356-1
{
   union
   {
      UINT8 oddEvenAndNoA;
      struct
      {
         EnumSs7NoaIndicator natureOfAddress:7;
         UINT8 oddEvenIndicator:1;
      };
   };

   union
   {
      UINT8 npInnIndicator;
      struct
      {
         UINT8 spare:4;
         EnumSs7NumberingPlan numberingPlan:3;
         EnumIsupInnIndicator innIndicator:1;
      };
   };
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

struct ISUPConnectedNumber
{
   EnumSs7NoaIndicator natureOfAddress:7;
   UINT8 oddEvenIndicator:1;
   EnumIsupScreeningIndicator screeningIndicator:2;
   EnumIsupAddrPresentationRestrictInd restrictInd:2;
   EnumSs7NumberingPlan numberingPlan:3;
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

//struct ISUPGenericNumber
//{
//   
//};

struct ISUPLocationNumber
{
   EnumSs7NoaIndicator natureOfAddress:7;
   UINT8 oddEvenIndicator:1;
   EnumIsupScreeningIndicator screeningIndicator:2;
   EnumIsupAddrPresentationRestrictInd restrictInd:2;
   EnumSs7NumberingPlan numberingPlan:3;
   EnumIsupInnIndicator innIndicator:1;
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

struct ISUPOriginalCalledNumber
{
   EnumSs7NoaIndicator natureOfAddress:7;
   UINT8 oddEvenIndicator:1;
   EnumIsupAddrPresentationRestrictInd restrictInd:2;
   EnumSs7NumberingPlan numberingPlan:3;
   TEXT address[SS7_MAX_ADDRESS_LEN + 1];
};

struct RedirectionInformation
{
   UINT8 redirectingIndicator:3;
   UINT8 originalRedirectionReason:4;
   UINT8 redirectionCounter:3;
   UINT8 redirectingReason:4;
};

////////////////////////////////////////////////////////////////////////
//---------------------------- MAP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumMapOperation
{
   MAP_UPDATE_LOCATION = 2,
   MAP_CANCEL_LOCATION = 3,
   MAP_PROVIDE_ROAMING_NUMBER = 4,
   MAP_NOTE_SUBSCRIBER_DATA_MODIFIED = 5,
   MAP_RESUME_CALL_HANDLING = 6,
   MAP_INSERT_SUBSCRIBER_DATA = 7,
   MAP_DELETE_SUBSCRIBER_DATA = 8,
   MAP_SEND_PARAMETERS = 9,
   MAP_REGISTER_SS = 10,
   MAP_ERASE_SS = 11,
   MAP_ACTIVATE_SS = 12,
   MAP_DE_ACTIVATE_SS = 13,
   MAP_INTERROGATE_SS = 14,
   MAP_AUTH_FAILURE_REPORT =15,
   MAP_REGISTER_PASSWORD = 17,
   MAP_GET_PASSWORD = 18,
   MAP_PROCESS_USS_DATA = 19,
   MAP_RELEASE_RESOURCES = 20,
   MAP_MT_FORWARD_SM_VGCS = 21,      
   MAP_SEND_ROUTING_INFO = 22,
   MAP_UPDATE_GPRS_LOCATION = 23,
   MAP_SEND_ROUTING_INFO_FOR_GPRS = 24,
   MAP_FAILURE_REPORT = 25,
   MAP_NOTE_MS_PRESENT_FOR_GPRS = 26,
   MAP_PERFORM_HANDOVER = 28,
   MAP_SEND_END_SIGNAL = 29,
   MAP_PERFORM_SUBSEQUENT_HANDOVER = 30,
   MAP_PROVIDE_SIWFS_NUMBER = 31,
   MAP_SIWFS_SIGNALLING_MODIFY = 32,
   MAP_PROCESS_ACCESS_SIGNALLING = 33,
   MAP_FORWARD_ACCESS_SIGNALLING = 34,
   MAP_NOTE_INTERNAL_HANDOVER = 35,
   MAP_RESET = 37,
   MAP_FORWARD_CHECK_SS_INDICATION = 38,
   MAP_PREPARE_GROUP_CALL = 39,
   MAP_SEND_GROUP_CALL_END_SIGNAL = 40,
   MAP_PROCESS_GROUP_CALL_SIGNALLING = 41,
   MAP_FORWARD_GROUP_CALL_SIGNALLING = 42,
   MAP_CHECK_IMSI = 43,
   MAP_MTFORWARD_SM = 44,
   MAP_SEND_ROUTING_INFO_FOR_SM = 45,
   MAP_FORWARD_SM = 46,
   MAP_MOFORWARD_SM = 46,
   MAP_REPORT_SM_DELIVER_STATUS = 47,
   MAP_NOTE_MS_PRESENT = 48,
   MAP_ALERT_SC_WITHOUT_RESULT = 49,
   MAP_ACTIVATE_TRACE_MODE = 50,
   MAP_DEACTIVATE_TRACE_MODE = 51,
   MAP_TRACE_SUBSCRIBER_ACTIVITY = 52,
   MAP_BEGIN_SUBS_ACTIVITY = 54,
   MAP_SEND_AUTHENTICATION = 56,
   MAP_RESTORE_DATA = 57,
   MAP_SEND_IMSI = 58,
   MAP_PROCESS_USS_REQ = 59,
   MAP_USS_REQ = 60,
   MAP_USS_NOTIFY = 61,
   MAP_ANY_TIME_SUBS_INTERROGATION = 62,
   MAP_INFORM_SERVICE_CENTER = 63,
   MAP_ALERT_SERVICE_CENTER = 64,
   MAP_ANY_TIME_MODIFICATION = 65,
   MAP_READY_FOR_SM = 66,
   MAP_PURGE_MS = 67,
   MAP_PREPARE_HANDOVER = 68,
   MAP_PREPARE_SUBSEQUENT_HANDOVER = 69,
   MAP_PROVIDE_SUBSCRIBER_INFO = 70,
   MAP_ANY_TIME_INTEROGATION = 71,
   MAP_SS_INVOCATION_NOTIFICATION =72,
   MAP_SET_REPORTING_STATE = 73,
   MAP_STATUS_REPORT = 74,
   MAP_REMOTE_USER_FREE = 75,
   MAP_REGISTER_CC_ENTRY = 76,
   MAP_ERASE_CC_ENTRY = 77,
   MAP_PROVODE_SUBS_LOCATION = 83,
   MAP_SEND_GROUP_CALL_INFO = 84,
   MAP_SEND_ROUTING_INFO_FOR_LCS = 85,
   MAP_SUBS_LOCATION_REPORT = 86,
   MAP_IST_ALERT = 87,
   MAP_IST_COMMAND = 88,
   MAP_NOTE_MM_EVENT =89
};

////////////////////////////////////////////////////////////////////////
//---------------------------- CAP Parameters ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumCapOperation
{
   CAP_INTITAL_DP_MSG = 0,
   CAP_ESTABLISH_TEMP_CONNECTION_MSG = 17,
   CAP_DISCONNECT_FWD_CONNECTION_MSG = 18,
   CAP_CONNECT_TO_RESOURCE_MSG = 19,
   CAP_CONNECT_MSG = 20,
   CAP_RELEASE_CALL_MSG = 22,
   CAP_REQUEST_REPORT_BSCM_MSG = 23,
   CAP_EVENT_REPORT_BCSM_MSG = 24,
   CAP_COLLECT_INFO_MSG = 27,
   CAP_CONTINUE_MSG = 31,
   CAP_RESET_TIMER_MSG = 33,
   CAP_FURNISH_CHARGING_INFO_MSG = 34,
   CAP_APPLY_CHARGING_MSG = 35,
   CAP_APPLY_CHARGING_REPORT_MSG = 36,
   CAP_CALL_GAP_MSG = 41,
   CAP_CALL_INFORMATION_REPORT_MSG = 44,
   CAP_CALL_INFORAMTION_REQUEST_MSG = 45,
   CAP_SEND_CHARGING_INFO_MSG = 46,
   CAP_PLAY_ANNOUNCEMENT_MSG = 47,
   CAP_PROMPT_COLLECT_USER_INFO_MSG = 48,
   CAP_CANCEL_MSG = 53,
   CAP_ACTIVITY_TEST_MSG = 55,
   CAP_INTITAL_DP_SMS_MSG = 60,
   CAP_FURNISH_CHARGING_INFO_SMS_MSG = 61,
   CAP_CONNECT_SMS_MSG = 62,
   CAP_REQUEST_REPORT_SMS_EVENT_MSG = 63,
   CAP_EVENT_REPORT_SMS_MSG = 64,
   CAP_CONTINUE_SMS_MSG = 65,
   CAP_RELEASE_SMS_MSG = 66,
   CAP_RESET_TIMER_SMS_MSG = 67,
   CAP_APPLY_CHARGING_GPRS_MSG = 71,
   CAP_APPLY_CHARGING_REPORT_GPRS_MSG = 72,
   CAP_CANCEL_GPRS_MSG = 73,
   CAP_CONNECT_GPRS_MSG = 74,
   CAP_CONTINUE_GPRS_MSG = 75,
   CAP_ENTITY_RELEASED_GPRS_MSG = 76,
   CAP_FURNISH_CHARGING_INFO_GPRS_MSG = 77,
   CAP_INTITAL_DP_GPRS_MSG = 78,
   CAP_RELEASE_GPRS_MSG = 79,
   CAP_EVENT_REPORT_GPRS_MSG = 80,
   CAP_REQUEST_REPORT_GPRS_EVENT_MSG = 81,
   CAP_RESET_TIMER_GPRS_MSG = 82,
   CAP_SEND_CHARGING_INFO_GPRS_MSG = 83,
   CAP_CONTINUE_WITH_ARG_MSG = 88,
   CAP_DISCONNECT_LEG_MSG = 90,
   CAP_MOVE_LEG_MSG = 93,
   CAP_SPLIT_LEG_MSG = 95,
   CAP_PLAY_TONE_MSG = 97
};

struct NaCarrierInfo
{
   TEXT naCarrierId[SS7_MAX_CARRIER_ID_LEN + 1];
   // -- NA carrier selection information octet carries the same values as ANSI
   //   -- ISUP T1.113: '00'H ? not indicated or not explicitly provided
   //   -- '01'H ? subscribed not dialed
   //   -- '02'H ? subscribed and dialed
   //   -- '03'H ? subscribed with dialing undetermined
   //   -- '04'H ? dialed CIC not subscribed
   UINT8 naCICSelectionType;
};

struct Ss7GtRoutingInfo
{
   TEXT gt[SS7_MAX_ADDRESS_LEN + 1];
   UINT16 pointCode;
   UINT16 ssn;
   UINT8 ai;
   UINT8 np;
   UINT8 noa;
   UINT8 length;			// Length of gt
};

struct Ss7OpCodeDetails
{
   TEXT opCodeName[SS7_MAX_OPCODE_NAME_LEN + 1];
   UINT8 opCode;
   UINT8 numOfDest;
   Ss7GtRoutingInfo dests[SS7_MAX_NUM_OF_OPCODE_GT_PC_TRANSLATIONS + 1];
};

struct Ss7OpDetails
{
   UINT32 opId;
   UINT8 ccLength;
   UINT8 ndcSnLength;
   TEXT opName[SS7_MAX_OPERATOR_NAME_LEN +1];
   TEXT country[SS7_MAX_COUNTRY_NAME_LEN +1];
   TEXT cc[SS7_MAX_COUNTRY_CODE_LEN +1];
   TEXT ndd[SS7_MAX_NDD_LEN +1];
   TEXT idd[SS7_MAX_IDD_LEN +1];
   TEXT defaultGt[SS7_MAX_ADDRESS_LEN +1];
   UINT8 ulVersion;
   UINT8 camelPhase;
};

struct Ss7MgtDetails
{
   UINT16 opDetailsIndex;
   UINT8 mccMncLength;
   UINT8 ccNcLength;
   UINT8 deleteDigits;
   TEXT mccMnc[SS7_MAX_ADDRESS_LEN + 1];
   TEXT ccNc[SS7_MAX_ADDRESS_LEN + 1];
};

struct Ss7CcNdcDetails
{
   UINT16 opDetailsIndex;
   UINT8 ccNdcLength;
   TEXT ccNdc[SS7_MAX_CC_NDC_LEN +1];
};

struct Ss7MccMncDetails
{
   UINT16 opDetailsIndex;
   UINT8 mccMncLength;
   TEXT mccMnc[SS7_MAX_MCC_MNC_LEN +1];
};

// Location Informtaion Structure used for MAP & CAP Operations
enum EnumCapCellIdOrLai
{
   CELLID_OR_LAI_CELL_ID,
   CELLID_OR_LAI_LAI
};

struct CellIdFixedLength
{
   TEXT   mcc[4];
   TEXT   mnc[4];
   UINT16 lac;
   UINT16 cellId;
};

struct CellIdOrLaiInfo
{
   EnumCapCellIdOrLai cellIdOrLai;
   union
   {
      CellIdFixedLength cellIdFixedLength;
      CellIdFixedLength laiFixedLength;
      //TEXT cellIdFixedLength[SS7_MAX_CELL_ID_FIXED_LEN + 1];
      //TEXT laiFixedLength[SS7_MAX_LAI_FIXED_LEN + 1];
   };
};

#define SS7_GEO_DEGREES_LEN   3
struct GeographicalInfo
{
   UINT8 typeOfShape;
   UINT8 latitude[SS7_GEO_DEGREES_LEN];
   UINT8 longitude[SS7_GEO_DEGREES_LEN];
   UINT8 uncertaintyCode;
};

struct GeodeticInfo
{
   UINT8 spi;                          // Screening and presentation indicators
   UINT8 typeOfShape;
   UINT8 latitude[SS7_GEO_DEGREES_LEN];
   UINT8 longitude[SS7_GEO_DEGREES_LEN];
   UINT8 uncertaintyCode;
   UINT8 confidence;
};

#define OPTIONAL_INIT_DP_LOC_INFO_AGE_OF_LOC         BIT_01
#define OPTIONAL_INIT_DP_LOC_INFO_GEO_INFO           BIT_02
#define OPTIONAL_INIT_DP_LOC_INFO_VLR_NUM            BIT_03
#define OPTIONAL_INIT_DP_LOC_INFO_LOC_NUM            BIT_04
#define OPTIONAL_INIT_DP_LOC_INFO_CELLID_OR_LAI      BIT_05

#define OPTIONAL_INIT_DP_LOC_INFO_LSA_IDENTITY       BIT_06
#define OPTIONAL_INIT_DP_LOC_INFO_MSC_NUM            BIT_07
#define OPTIONAL_INIT_DP_LOC_INFO_GEODETIC_INFO      BIT_08
#define OPTIONAL_INIT_DP_LOC_INFO_CURR_LOC_RETRIEVED BIT_09
#define OPTIONAL_INIT_DP_LOC_INFO_SAI_PRESENT        BIT_10

struct LocationInfo
{
   UINT32 optional_params;
   UINT16 ageOfLocationInformation;
   GeographicalInfo  geographicalInformation;
   TEXT   vlrNumber[SS7_MAX_ADDRESS_LEN + 1];
   TEXT   locationNumber[SS7_MAX_LOCATION_NUMBER_LEN + 1];
   CellIdOrLaiInfo cellIdOrLai;

   UINT8  lsaIdentity[SS7_MAX_LSA_IDENTITY_LEN +1];
   TEXT   mscNumber[SS7_MAX_ADDRESS_LEN + 1];
   GeodeticInfo  geodeticInfo;
   BOOLEAN currLocationRetrieved;
   BOOLEAN saiPresent;
};

struct StatusMsu
{
   UINT32         routingContext;
   UINT32         opc;
   UINT32         dpc;
   UINT32         txMsu;
   UINT32         rxMsu;
   UINT32         discardedMsu;
   UINT32         totaltxMsu;
   UINT32         totalrxMsu;
   UINT32         totaldiscardedMsu;
   UINT32         status;
   TEXT           reason[MAX_ACU_REASON_LEN + 1];
   UINT32         seconds;
   struct tm      genTime;
};


#endif
