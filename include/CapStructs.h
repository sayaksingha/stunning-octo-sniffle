// @(#) $Id: CapStructs.h,v 1.1.6.1.6.3 2012/12/03 14:24:28 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// CapStructs.h
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
// Originated:  Joel Baby Jose                               15-MAR-2005
//----------------------------------------------------------------------
#ifndef INCL_SS7_CAP_STRUCT_DEF_H
#define INCL_SS7_CAP_STRUCT_DEF_H

#include "Ss7ConstDef.h"
#include "Ss7Structs.h"
#include "Ss7BerEncDec.h"

#define CAP_MAX_NO_OF_CAP_ACN  7

const UINT8 CapAcn[CAP_MAX_NO_OF_CAP_ACN][7] = {
   {0, 0, 0, 0, 0, 0x00, 0},	// Not present
   {4, 0, 0, 1, 0, 0x32, 1},	//CS1_SSF_TO_SCF
   {4, 0, 0, 1, 0, 0x33, 1},	//CS1_ASSIST_SSF_TO_SCF
   {4, 0, 0, 1, 0, 0x34, 1},	//CS1_SRF_TO_SCF
   {4, 0, 0, 1, 0, 0x32, 2},	//CS2_SSF_TO_SCF
   {4, 0, 0, 1, 0, 0x33, 2},	//CS2_ASSIST_SSF_TO_SCF
   {4, 0, 0, 1, 0, 0x34, 2}	//CS1_SRF_TO_SCF
};

enum EnumCapAcn
{
   CAP_ACN_NOT_PRESENT = 0,
   CS1_SSF_TO_SCF = 1,
   CS1_ASSIST_SSF_TO_SCF = 2,
   CS1_SRF_TO_SCF = 3,
   CS2_SSF_TO_SCF = 4,
   CS2_ASSIST_SSF_TO_SCF = 5,
   CS2_SRF_TO_SCF = 6
};

enum EnumCapSubsState
{
   ASSUMED_IDLE = 1,
   CAMEL_BUSY,
   NETDET_NOT_REACHABLE,
   NOT_PROVIDED_FROM_VLR
};

enum EnumCapEventTypeBCSM
{
   COLLECTED_INFO = 2,
   ANALYZED_INFORMATION = 3,
   ROUTE_SELECT_FAILURE = 4,
   O_CALLED_PARTY_BUSY = 5,
   O_NO_ANSWER = 6,
   O_ANSWER = 7,
   O_MID_CALL = 8,
   O_DISCONNECT = 9,
   O_ABANDON = 10,
   TERM_ATTEMPT_AUTHORIZED = 12,
   T_BUSY = 13,
   T_NO_ANSWER = 14,
   T_ANSWER = 15,
   T_MID_CALL = 16,
   T_DISCONNECT = 17,
   T_ABANDON = 18,
   O_TERM_SEIZED = 19,
   CALL_ACCEPTED = 27,
   O_CHANGE_OF_POSITION = 50,
   T_CHANGE_OF_POSITION = 51,
   O_SERVICE_CHANGE = 52,
   T_SERVICE_CHANGE = 53
};

enum EnumCapRequestedInfoType
{
   CALL_ATTEMPT_ELAPSED_TIME = 0,
   CALL_STOP_TIME = 1,
   CALL_CONNECTED_ELAPSED_TIME = 2,
   RELEASE_CAUSE = 30
};

enum EnumCapMonitorMode
{
   INTERRUPTED  = 0,
   NOTIFY_AND_CONTINUE = 1,
   TRANSPARENT = 2
};

enum EnumCapMiscCallInfo
{
   REQUEST = 0,
   NOTIFICATION,
};

enum EnumCapLegType
{
   UNKNOWN_LEG_ID = 0,
   LEG_1 = 1,
   LEG_2 = 2
};

struct TimeAndTimeZone
{
   tm time;
   UINT8 timeZone;
};

#define OPTIONAL_T_NO_ANS_SPECIFIC_INFO_CALL_FWD      BIT_01
#define OPTIONAL_T_NO_ANS_SPECIFIC_INFO_FWD_DEST_NUM  BIT_02
struct tNoAnswerSpecificInfo
{
   UINT32 optional_params;
   BOOLEAN callFwd;
   TEXT fwdDestinationNumber;
};

#define OPTIONAL_T_BUSY_SPECIFIC_INFO_BUSY_CAUSE      BIT_01
#define OPTIONAL_T_BUSY_SPECIFIC_INFO_CALL_FWD        BIT_02
#define OPTIONAL_T_BUSY_SPECIFIC_INFO_ROUTE_NOT_PER   BIT_03
#define OPTIONAL_T_BUSY_SPECIFIC_INFO_FWD_DEST_NUM    BIT_04
struct tBusySpecificInfo
{
   UINT32 optional_params;
   UINT8 busyCause;
   BOOLEAN callFwd;
   BOOLEAN routeNotPermitted;
   TEXT fwdDestinationNumber;
};

struct DisconnectSpecificInfo
{
   UINT8 releaseCause;
};

#define OPTIONAL_ANS_SPECIFIC_INFO_DEST_ADDR   BIT_01
#define OPTIONAL_ANS_SPECIFIC_INFO_ORCALL      BIT_02
#define OPTIONAL_ANS_SPECIFIC_INFO_FWD_CALL    BIT_03
struct AnswerSpecificInfo
{
   UINT32 optional_params;
   TEXT destinationAddress[SS7_MAX_ADDRESS_LEN + 1];
   BOOLEAN orCall;
   BOOLEAN fwdCall;
};

struct oCalledPartyBusySpecificInfo
{
   UINT8 busyCause;
};

struct EventSpecificInformationBCSM
{
   union
   {
      //RouteSelectFailureSpecificInfo  routeSelectFailure;
      oCalledPartyBusySpecificInfo oCalledPartyBusy;
      AnswerSpecificInfo oAnswer;
      //oMidCallSpecificInfo            oMidCall;
      DisconnectSpecificInfo oDisconnect;
      tBusySpecificInfo tBusy;
      tNoAnswerSpecificInfo tNoAnswer;
      AnswerSpecificInfo tAnswer;
      //tMidCallSpecificInfo            tMidCall;
      DisconnectSpecificInfo tDisconnect;
      //oTermSeizedSpecificInfo         oTermSeized;
      //callAcceptedSpecificInfo        callAccepted;
      //oAbandonSpecificInfo            oAbandon;
      //oChangeOfPositionSpecificInfo   oChangeOfPosition;
      //tChangeOfPositionSpecificInfo   tChangeOfPosition;
   };
};

struct HighLayerCompat
{
   UINT8 interpretPresentMethod;
   UINT8 higherLayerCharID;
};

struct BearerService
{
   UINT8 infoTransCapab;
   UINT8 transferMode;
   UINT8 uiLayer1ProtID;
};

struct BasicServiceCode
{
   UINT8 basicServiceCodeType;	// 1 - BearerService, 2 - Teleservice 
   UINT8 basicServiceCodeValue;
};

#define OPTIONAL_BCSM_EVENT_LEG_TYPE    BIT_01
#define OPTIONAL_BCSM_EVENT_APP_TIMER   BIT_02
struct BCSMEvent
{
   EnumCapEventTypeBCSM eventTypeBCSM;
   EnumCapMonitorMode monitorMode;
   UINT32 optional_params;
   EnumCapLegType legType;
   UINT8 apptimervalue;
};

struct TimeIfTariffSwitch
{
   INT32 timeSinceTariffSwitch;
   INT32 tariffSwitchInterval;
};

#define OPTIONAL_APPLY_CHRG_REP_TARIFF_SWITCH_INTERVAL       BIT_01
struct TimeInformation
{
   UINT32 optional_params;
   BOOLEAN tariffSwitch;
   INT32 timeIfNoTariffSwitch;
   TimeIfTariffSwitch timeIfTariffSwitch;
};

struct RequestedInfoValue
{
   UINT16 callAttemptElapsedTimeValue;
   tm callStopTimeValue;
   UINT16 callConnectedElapsedTimeValue;
   UINT8 releaseCauseValue;
};

struct RequestedInformation
{
   EnumCapRequestedInfoType requestedInfoType;
   RequestedInfoValue requestedInfoValue;
};

struct CapDefaultMsg
{
   TDArray param;
};

struct ActivityTestMsg
{
   ;
};

#define CAP_MAX_NUM_OF_INFO_ITEMS  4
struct CallInformationRequestMsg
{
   INT16 numberOfReqInfo;
   EnumCapRequestedInfoType requestedInfoType[CAP_MAX_NUM_OF_INFO_ITEMS];
   EnumCapLegType legId;
};

struct CallInformationReportMsg
{
   UINT8 numberOfReqInfo;
   RequestedInformation requestedInfo[CAP_MAX_NUM_OF_INFO_ITEMS + 1];
   EnumCapLegType legId;
};

struct ApplyChargingReportMsg
{
   EnumCapLegType partyToCharge;
   TimeInformation timeInformation;
   BOOLEAN callActive;
};

#define OPTIONAL_APPLY_CHRG_RELEASE_IF_DURATION_EXCEED   BIT_01
#define OPTIONAL_APPLY_CHRG_TARIFF_SWITCH_INTERVAL       BIT_02
#define OPTIONAL_APPLY_CHRG_AUDIABLE_INDICATOR           BIT_03
struct ApplyChargingMsg
{
   UINT32 optional_params;
   INT32 maxCallPeriodDuration;
   BOOLEAN releaseIfDurationExceeded;
   INT32 tariffSwitchInterval;
   BOOLEAN audiableIndicator;
   EnumCapLegType partyToCharge;
};

struct ContinueMsg
{
   ;
};

#define OPTIONAL_EVENT_REPORT_BCSM_EVENT_SPECIFIC_INFO   BIT_01
#define OPTIONAL_EVENT_REPORT_BCSM_LEG_ID                BIT_02
#define OPTIONAL_EVENT_REPORT_BCSM_MISC_CALL_INFO        BIT_03
struct EventReportBCSMMsg
{
   EnumCapEventTypeBCSM eventTypeBCSM;
   UINT32 optional_params;
   EventSpecificInformationBCSM eventSpecificInfo;
   EnumCapLegType legId;
   EnumCapMiscCallInfo miscCallInfo;
};

struct RequestReportBCSMMsg
{
   INT16 numberOfBCSMEvents;
   BCSMEvent BSCMEvents[SS7_MAX_NUMBER_OF_BCSM_EVENTS];
};

struct ReleaseCallMsg
{
   UINT8 relLocCodingStd;
   EnumRelCallCause relCallCause;
};

#define OPTIONAL_CONNECT_ALERT_PATTERN        BIT_01
#define OPTIONAL_CONNECT_ORIG_CLD_PARTY       BIT_02
#define OPTIONAL_CONNECT_CLG_PARTY_CATEGORY   BIT_03
#define OPTIONAL_CONNECT_DISPLAY_NUMBER       BIT_04
#define OPTIONAL_CONNECT_REDIRECT_PARTY       BIT_05
#define OPTIONAL_CONNECT_REDIRECT_INFO        BIT_06
#define OPTIONAL_CONNECT_GENERIC_NUMS         BIT_07
#define OPTIONAL_CONNECT_SUPRESS_ANNOUNCEMENT BIT_08
#define OPTIONAL_CONNECT_OCSI_APPLICABLE      BIT_09
#define OPTIONAL_CONNECT_NA_CARRIER_INFO      BIT_10
#define OPTIONAL_CONNECT_NA_OLI_INFO          BIT_11
#define OPTIONAL_CONNECT_NA_CHARGE_NUMBER     BIT_12

struct ConnectMsg
{
   TEXT destRoutingAddress[SS7_MAX_ADDRESS_LEN + 1];
   UINT32 optional_params;
   UINT8 alertingPattern;

   UINT8 callingPartysCategory;
   TEXT originalCalledPartyNumber[SS7_MAX_ADDRESS_LEN + 1];

   // The Following params are Added by Paresh
   ISUPCalledPartyNumber  redirectingPartyId;
   RedirectionInformation redirectInfo;
   //Set of five generic Numbers
   TEXT displayNumber[SS7_MAX_ADDRESS_LEN + 1];

   BOOLEAN supressionOfAnnouncement;
   BOOLEAN OCSIApplicable;
   NaCarrierInfo naCarrierInfo;
   //NAOliInfo ::= OCTET STRING (SIZE (1))
   //  -- NA Oli information takes the same value as defined in ANSI ISUP T1.113
   // -- e.g. '3D'H ? Decimal value 61 - Cellular Service (Type 1)
   // -- '3E'H ? Decimal value 62 - Cellular Service (Type 2)
   // -- '3F'H ? Decimal value 63 - Cellular Service (roaming)
   UINT8 naOliInfo;
   TEXT naChargeNumber[SS7_MAX_ADDRESS_LEN + 1];
};

// All These SMS related CAP Messages can be used CAP-3 version and Above.
// These structures are defined using CAP-3 version.
/*------------------------------------------------------------------------*/
/************************** Initial Dp Sms ********************************/
/*------------------------------------------------------------------------*/
#define OPTIONAL_INITDP_CLD_PARTY_NUM          BIT_01
#define OPTIONAL_INITDP_CLG_PARTY_NUM          BIT_02
#define OPTIONAL_INITDP_CLG_PARTY_CATEGORY     BIT_03
#define OPTIONAL_INITDP_LOC_NUM                BIT_04
#define OPTIONAL_INITDP_EVENT_TYPE_BCSM        BIT_05
#define OPTIONAL_INITDP_IMSI                   BIT_06
#define OPTIONAL_INITDP_SUBS_STATE             BIT_07
#define OPTIONAL_INITDP_LOC_INFO               BIT_08
#define OPTIONAL_INITDP_CALL_REF_NUM           BIT_09
#define OPTIONAL_INITDP_MSC_ADDR               BIT_10
#define OPTIONAL_INITDP_CLD_PARTY_BCD          BIT_11
#define OPTIONAL_INITDP_TIME_ZONE              BIT_12
#define OPTIONAL_INITDP_BEARER_CAP             BIT_13
#define OPTIONAL_INITDP_HIGH_LAYER_COMPAT      BIT_14
#define OPTIONAL_INITDP_BASIC_SERVICE_CODE     BIT_15
#define OPTIONAL_INITDP_REDIRECTION_INFO       BIT_16
#define OPTIONAL_INITDP_REDIRECTION_PARTY      BIT_17

struct InitialDPMsg
{
   INT32                   serviceKey;
   UINT32                  optional_params;
   EnumSs7NoaIndicator     calledPartyNOA;
   TEXT                    calledPartyNum[SS7_MAX_ADDRESS_LEN + 1];
   EnumSs7NoaIndicator     callingPartyNOA;
   ScreenClip              screenClip;
   TEXT                    callingPartyNum[SS7_MAX_ADDRESS_LEN + 1];
   UINT8                   callingPartysCategory;
   TEXT                    locationNumber[SS7_MAX_ADDRESS_LEN + 1];
   RedirectionInformation  redirectionInformation;
   ISUPCalledPartyNumber   redirectingPartyID;
   EnumCapEventTypeBCSM    eventTypeBCSM;
   HighLayerCompat         highLayerCompat;
   BearerService           bearerCapability;
   BasicServiceCode        basicServiceCode;
   TEXT                    imsi[SS7_MAX_ADDRESS_LEN + 1];
   EnumCapSubsState        subsState;
   LocationInfo            locationInformation;
   TEXT                    callReferenceNumber[SS7_MAX_ADDRESS_LEN + 1];
   TEXT                    mscAddress[SS7_MAX_ADDRESS_LEN + 1];
   NoaNp                   calledPartyBCDNumNoaNp;
   TEXT                    calledPartyBCDNum[SS7_MAX_ADDRESS_LEN + 1];
   TimeAndTimeZone         timeAndTimeZone;
};

#define SS7_MAX_TIME_LEN                 25
#define SS7_MAX_TP_VAL_PERIOD_LEN        7
//These are the Release SMS Message Possible Causes.
enum Cap_SmsRelCause
{
   CAP_SMS_MEMORY_CAPACITY_EXCEEDED             = 22,
   CAP_SMS_INVALID_SM_REFERENCE                 = 81,
   CAP_SMS_INVALID_SHORT_MSG                    = 95,
   CAP_SMS_MANDATORY_PARAM_INFO                 = 96,
   CAP_SMS_MSG_TYPE_NON_EXISTENT                = 97,
   CAP_SMS_MSG_NOT_COMPATIABLE_WITH_SM_PROTOCOL = 98
};
/*
   CAP MO-SMS RELEASE CAUSE
   -------------------------
   1        Unassigned (unallocated) number
   8        Operator determined barring
   10       Call barred
   11       Reserved
   21       Short message transfer rejected
   27       Destination out of order
   28       Unidentified subscriber
   29       Facility rejected
   30       Unknown subscriber
   38       Network out of order
   41       Temporary failure
   42       Congestion
   47       Resources unavailable, unspecified
   50       Requested facility not subscribed
   69       Requested facility not implemented
   81       Invalid short message transfer reference value
   95       Semantically incorrect message
   96       Invalid mandatory information
   97       Message type non-existent or not implemented
   98       Message not compatible with short message protocol state
   99       Information element non-existent or not implemented
   111      Protocol error, unspecified
   127      Interworking, unspecified

   CAP MT-SMS RELEASE CAUSE
   -------------------------
   22      Memory capacity exceeded
   81      Invalid short message transfer reference value
   95      Semantically incorrect message
   96      Invalid mandatory information
   97      Message type non-existent or not implemented
   98      Message not compatible with short message protocol state
   99      Information element non-existent or not implemented
   111     Protocol error, unspecified
 */

enum Cap_SmsEventType
{
   SMS_UNKNOWN = 0,
   SMS_COLLECT_INFO = 1,
   SMS_O_FAILURE = 2,
   SMS_O_SUBMITION = 3,
   SMS_DELIVARY_REQUESTED = 11, 
   SMS_T_SMS_FAILURE = 12, 
   SMS_T_DELIVARY = 13 
};

struct O_SmsFailureSpecificInfo
{
   UINT8  MO_SMSCause;
};

struct O_SmsSubmissionSpecificInfo
{
};

struct T_SmsFailureSpecificInfo 
{
   UINT8  MT_SMSCause;
};

struct T_SmsDeliverSpecificInfo 
{
};

enum EnumEvtSpecificInfo
{
   EventSpecificInfoNotPresent = 0,
   O_SmsFailure     = 1,
   O_SmsSubmission  = 2,
   T_SmsFailure     = 3,
   T_SmsDeliver     = 4
};
struct EventSpecificInformationSMS
{
   EnumEvtSpecificInfo enumEvtSpecificInfo;
   union
   {
      O_SmsFailureSpecificInfo      oSmsCause;
      O_SmsSubmissionSpecificInfo   oSmsSumbitInfo;
      T_SmsFailureSpecificInfo      tSmsCause;
      T_SmsDeliverSpecificInfo      tSmsDeliverInfo;
   };
};

struct Cap_SmsEvents
{
   Cap_SmsEventType     event;
   EnumCapMonitorMode   mode;
};

//Not decoded, need to create structure for LocationInformationGprs
#define LOCATION_INFORMATION_GPRS_LEN    60 
struct LocationInfoGprs
{
   UINT8               length;
   UINT8               locationInformationGPRS[LOCATION_INFORMATION_GPRS_LEN + 1];
};

//Not decoded, need to create structure for extensions
#define SS7_EXTENSIONS_LEN    60 
struct Extensions
{
   UINT8               length;
   UINT8               extensions[SS7_EXTENSIONS_LEN + 1];
};

//Not decoded, need to create structure for need need MS-Classmark2
#define MS_CLASS_MARK_2_LEN    60 
struct MsClassMark2
{
   UINT8               length;
   UINT8               msClassMark2[MS_CLASS_MARK_2_LEN + 1];
};

//Not decoded, need to create structure for need need GPRSMSClass
#define GPRS_MS_CLASS_LEN    60 
struct GPRSMSClass
{
   UINT8               length;
   UINT8               gprsMsClass[GPRS_MS_CLASS_LEN + 1];
};

struct TpValidityPeriod
{
   UINT8               length;
   UINT8               tPValidityPeriod[SS7_MAX_TP_VAL_PERIOD_LEN +1]; 
};

#define OPT_INIT_DP_SMS_DEST_SUBS_NO           BIT_01
#define OPT_INIT_DP_SMS_CALLING_NO             BIT_02
#define OPT_INIT_DP_SMS_EVENTTYPESMS           BIT_03
#define OPT_INIT_DP_SMS_IMSI                   BIT_04
#define OPT_INIT_DP_SMS_LOCATIONINFOMSC        BIT_05
#define OPT_INIT_DP_SMS_LOCATIONINFOGPRS       BIT_06
#define OPT_INIT_DP_SMS_SMSC_ADDRESS           BIT_07
#define OPT_INIT_DP_SMS_TIMEANDTIMEZONE        BIT_08
#define OPT_INIT_DP_SMS_TPSMSSPECIFICINFO      BIT_09
#define OPT_INIT_DP_SMS_TPPROTOCOLIDENTFY      BIT_10
#define OPT_INIT_DP_SMS_TPDATACODINGSCHEME     BIT_11
#define OPT_INIT_DP_SMS_TPVALIDITYPERIOD       BIT_12
#define OPT_INIT_DP_SMS_EXT_CONTAINER          BIT_13
#define OPT_INIT_DP_SMS_SMSREFERENCENO         BIT_14
#define OPT_INIT_DP_SMS_MSCADDRESS             BIT_15
#define OPT_INIT_DP_SMS_SGSNNUMBER             BIT_16
#define OPT_INIT_DP_SMS_MS_CLASS_MARK          BIT_17
#define OPT_INIT_DP_SMS_GPRS_MS_CLASS          BIT_18
#define OPT_INIT_DP_SMS_IMEI                   BIT_19
#define OPT_INIT_DP_SMS_CALLED_PARTY_NO        BIT_20

struct Cap_InitialDPSmsArg

{
   INT32                   serviceKey;
   UINT32                  optional_params;
   ISDNAddressString       destinationSubsNo;
   ISDNAddressString       callingPartyNumber;
   Cap_SmsEventType        eventTypeSMS;
   TEXT                    imsi[SS7_MAX_ADDRESS_LEN + 1]; 
   LocationInfo            locationInformationMSC;
   LocationInfoGprs        locationInformationGPRS; // Not Decoded
   ISDNAddressString       smscAddress;
   TimeAndTimeZone         timeAndTimezone;
   UINT8                   tPShortMsgSpecificInfo;
   UINT8                   tPProtocolIdentifier;
   UINT8                   tPDataCodingScheme;
   TpValidityPeriod        tPValidityPeriod; 
   Extensions              extension; // Not Decoded
   TEXT                    smsReferenceNumber[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString       mscAddress;
   ISDNAddressString       sgsnNumber;
   MsClassMark2            msClassMark2; // Not Decoded
   GPRSMSClass             gprsClass; // Not Decoded
   TEXT                    imei[SS7_MAX_ADDRESS_LEN +1];
   ISDNAddressString       calledPartyNo;
};

/*------------------------------------------------------------------------*/
/***************************** Connect Sms ********************************/
/*------------------------------------------------------------------------*/
#define OPTIONAL_T_CONNECT_SMS_REQ_CALLING_PARTY_NUMBER               BIT_01
#define OPTIONAL_T_CONNECT_SMS_RSP_DESTINATION_SUBSCRIBER_NUMBER      BIT_02
#define OPTIONAL_T_CONNECT_SMS_REQ_SMSC_ADDRESS                       BIT_03

struct Cap_ConnectSmsArg
{
   UINT32            optional_params;
   ISDNAddressString callingPartysNumber;
   ISDNAddressString destinationSubsNo;
   ISDNAddressString smscAddress;
};

struct Cap_ConnectSmsRsp
{
};

/*------------------------------------------------------------------------*/
/***************************** Release Sms ********************************/
/*------------------------------------------------------------------------*/
//#define CAP_REL_SMS_CAUSE_LEN 1
struct Cap_ReleaseSmsArg
{
   //TEXT RpCause[CAP_REL_SMS_CAUSE_LEN + 1];
   Cap_SmsRelCause  RpCause;
};

/*------------------------------------------------------------------------*/
/***************************** Continue Sms *******************************/
/*------------------------------------------------------------------------*/
#define CAP_CONT_ALRT_LEN             3

struct Cap_ContinueSmsArg
{
};

/*------------------------------------------------------------------------*/
/***************************** Event Report Sms ***************************/
/*------------------------------------------------------------------------*/
#define OPTIONAL_EVT_REPORT_SMS_O_SMS_FAILURE_INFO                   BIT_01
#define OPTIONAL_EVT_REPORT_SMS_T_SMS_FAILURE_INFO                   BIT_02
#define OPTIONAL_EVT_REPORT_SMS_O_SMS_SUBMIT_INFO                    BIT_03
#define OPTIONAL_EVT_REPORT_SMS_T_SMS_DELIVER_INFO                   BIT_04
#define OPTIONAL_EVT_REPORT_SMS_EXT_CONTAINER                        BIT_05
struct Cap_EventReportSMSArg
{
   UINT32                          optional_params;
   Cap_SmsEventType                eventTypeSMS;
   EventSpecificInformationSMS     eventInfo;
   EnumCapMiscCallInfo             miscInfo;//???
   Extensions                      extension; // Not Decoded
};

/*------------------------------------------------------------------------*/
/***************************** Request Report Sms *************************/
/*------------------------------------------------------------------------*/
#define SS7_MAX_NUMBER_OF_SMS_EVENTS  10

struct Cap_RequestReportSMSEventArg
{
   INT16            numberOfSMSEvents;
   Cap_SmsEvents    events[SS7_MAX_NUMBER_OF_SMS_EVENTS];
};
#endif
