// @(#) $Id: MapStructs.h,v 1.1.1.2.6.8 2016/07/27 23:04:59 jamesjac Exp $
//----------------------------------------------------------------------
// NAME
// MapStructs.h
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
// Originated: Joel Baby Jose                                
// Modified  : Lakshmi prasanna.G                            08-SEP-2008
//----------------------------------------------------------------------
#ifndef INCL_SS7_MAP_STRUCT_DEF_H
#define INCL_SS7_MAP_STRUCT_DEF_H
#include "Ss7ConstDef.h"
#include "Ss7Structs.h"
#include "TcapStructs.h"
#include "MapUtil.h"
#include "Ss7BerEncDec.h"
struct ExtensionContainer
{
   TDArray data;
};

//====================================================================//
//----------------------SMS RELATED STRUCTURES -----------------------//
//====================================================================//
#define MAP_TIME_LEN                            14
#define SS7_MAX_IMEI_LEN                        17

enum EnumOperationRsp
{
   OPER_REP_FAILURE = 0,
   OPER_REP_ACK = 1,
   OPER_REP_NACK = 2
};

enum EnumDelIntended
{
  REQUESTED_ONLY_IMSI    = 0,
  REQUESTED_ONLY_MCC_MNC = 1
};

/*
   The TP-Message-Type-Indicator is a 2-bit field, located within bits 
   no 0 and 1 of the first octet of all PDUs which can be given the following values:
   bit1 bit0 Message type
   0 0 SMS-DELIVER (in the direction SC to MS)
   0 0 SMS-DELIVER REPORT (in the direction MS to SC)
   1 0 SMS-STATUS-REPORT (in the direction SC to MS)
   1 0 SMS-COMMAND (in the direction MS to SC)
   0 1 SMS-SUBMIT (in the direction MS to SC)
   0 1 SMS-SUBMIT-REPORT (in the direction SC to MS)
   1 1 Reserved
   If an MS receives a TPDU with a "Reserved" value in the TP-MTI it shall 
   process the message as if it were an "SMS-DELIVER" but store the message 
   exactly as received
 */
// Message Type indicators
#define SMS_DELIVER_MTI                     0x00
#define SMS_DELIVER_REPORT_MTI              0x00
#define SMS_STATUS_REPORT_MTI               0x02
#define SMS_COMMAND_MTI                     0x02
#define SMS_SUBMIT_MTI                      0x01
#define SMS_SUBMIT_REPORT_MTI               0x01


struct smSubmitRPUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;  //Parameter describing the message type
         UINT8 rd:1;   //Parameter indicates whether SC should reject duplicate message or not
         UINT8 vpf:2;  //Parameter indicating whether or not the TP-VP field is present..
         UINT8 srr:1;  //Parameter indicating if the SME has requested a status report.
         UINT8 udhi:1; //Parameter indicating that the TP-UD field contains a Header
         UINT8 rp:1;   //Parameter indicating the request for Reply Path.
      };
   };
   UINT8 messageRef;             //Parameter identifying the SMS-SUBMIT.
   ISDNAddressString destAddr;   //Address of the originating SME.**O
   UINT8 protoId;                //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;         //Parameter identifying the coding scheme within the TP-User-Data.**O
   //Parameter identifying time when the SC received the message. **O
   TEXT valPeriod[MAP_TIME_LEN + 1];
   //Parameter indicating number of bytes in TP-User-Data field to follow. INTIGER.
   UINT8 userDataLen;
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

struct smDeliveryRepAckUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;     //Parameter describing the message type
         UINT8 spare1:4;  //Parameter indicates whether SC should reject duplicate message or not
         UINT8 udhi:1;    //Parameter indicating that the TP-UD field contains a Header
         UINT8 spare2:1;  //Parameter indicating the request for Reply Path.
      };
   };
   UINT8 paramInd;        // Parameter indicating the presence of any of the optional parameters which follow
   UINT8 protoId;         //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;             //Parameter identifying the coding scheme within the TP-User-Data.**O
   UINT8 userDataLen;     //Parameter indicating the length of the TP-User-Data field to follow
   UINT8 userData[MAP_MAX_USER_DATA_LEN];  // User data
};

struct smDeliveryRepFailureUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;      //Parameter describing the message type
         UINT8 spare1:4;   //Parameter indicates whether SC should reject duplicate message or not
         UINT8 udhi:1;     //Parameter indicating that the TP-UD field contains a Header
         UINT8 spare2:1;   //Parameter indicating the request for Reply Path.
      };
   };
   UINT8 failureCause;     // Parameter indicating the reason for SMS-SUBMIT failure
   UINT8 paramInd;         // Parameter indicating the presence of any of the optional parameters which follow
   UINT8 protoId;          //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;              //Parameter identifying the coding scheme within the TP-User-Data.**O
   UINT8 userDataLen;      //Parameter indicating the length of the TP-User-Data field to follow
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

struct smSubmitRepAckUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;      //Parameter describing the message type
         UINT8 spare1:4;   //Parameter indicates whether SC should reject duplicate message or not
         UINT8 udhi:1;     //Parameter indicating that the TP-UD field contains a Header
         UINT8 spare2:1;   //Parameter indicating the request for Reply Path.
      };
   };
   UINT8 paramInd;         // Parameter indicating the presence of any of the optional parameters which follow
   TEXT scts[MAP_TIME_LEN + 1]; // Parameter identifying the time when the SC received the SMS-SUBMIT
   UINT8 protoId;          //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;              //Parameter identifying the coding scheme within the TP-User-Data.**O
   UINT8 userDataLen;      //Parameter indicating the length of the TP-User-Data field to follow
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

struct smSubmitRepFailureUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;           //Parameter describing the message type
         UINT8 spare1:4;        //Parameter indicates whether SC should reject duplicate message or not
         UINT8 udhi:1;          //Parameter indicating that the TP-UD field contains a Header
         UINT8 spare2:1;        //Parameter indicating the request for Reply Path.
      };
   };
   UINT8 failureCause;          //Parameter indicating the reason for SMS-SUBMIT failure
   UINT8 paramInd;              //Parameter indicating the presence of any of the optional parameters which follow
   TEXT scts[MAP_TIME_LEN + 1]; //Parameter identifying the time when the SC received the SMS-SUBMIT
   UINT8 protoId;               //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;                   //Parameter identifying the coding scheme within the TP-User-Data.**O
   UINT8 userDataLen;           //Parameter indicating the number of bytes in TP-User-Data field to follow
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

struct smDeliverRPUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;           //Parameter describing the message type
         UINT8 mms:1;           //Parameter indicates whether or not there are more messages to send
         UINT8 resv:2;
         UINT8 sri:1;           //Parameter indicating if the SME has requested a status report.
         UINT8 udhi:1;          //Parameter indicating that the TP-UD field contains a Header
         UINT8 repPath:1;       //Parameter indicating that Reply Path exists.
      };
   };
   ISDNAddressString origAddr;  //Address of the originating SME.**O
   UINT8 protoId;               //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;                   //Parameter identifying the coding scheme within the TP-User-Data.**O
   TEXT scts[MAP_TIME_LEN + 1]; //Parameter identifying time when the SC received the message. **O
   UINT8 userDataLen;           //Parameter indicating the number of bytes in TP-User-Data field to follow. INTIGER.
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

struct smStatusReportRPUI
{
   union
   {
      UINT8 uiFlags;
      struct
      {
         UINT8 mti:2;           //Parameter describing the message type
         UINT8 mms:1;           //Parameter indicates whether or not there are more messages to send
         UINT8 resv:2;
         UINT8 srq:1;       //Parameter indicating that Reply Path exists.
         UINT8 udhi:1;          //Parameter indicating that the TP-UD field contains a Header
         UINT8 resv1:1;
      };
   };
   UINT8 messageRef;             //Parameter identifying the SMS-SUBMIT.
   ISDNAddressString recipientAddr;  //Address of the originating SME.**O
   TEXT scts[MAP_TIME_LEN + 1]; //Parameter identifying time when the SC received the message. **O
   TEXT dt[MAP_TIME_LEN + 1];   //Parameter identifying the time associated with a particular TP-ST outcome
   UINT8 status;                //Parameter identifying the status of the previously sent mobile originated short message
   UINT8 paramInd;              //Parameter indicating the presence of any of the optional parameters which follow
   UINT8 protoId;               //Parameter identifying the above layer protocol, if any.**O
   UINT8 dcs;                   //Parameter identifying the coding scheme within the TP-User-Data.**O
   UINT8 userDataLen;           //Parameter indicating the number of bytes in TP-User-Data field to follow. INTIGER.
   UINT8 userData[MAP_MAX_USER_DATA_LEN]; // User data
};

enum Map_EnumSmRpDa
{
   MAP_IMSI = 0,
   MAP_LMSI = 1,
   MAP_DEST_SC_ADDR = 2,
   MAP_NO_SM_RP_DA = 3
};

struct smRPDA
{
   Map_EnumSmRpDa enumSmRpDa;
   union
   {
      TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
      TEXT lmsi[SS7_MAX_ADDRESS_LEN + 1];
      ISDNAddressString serviceCentreAddressDA;
      BOOLEAN noSMRPDA;
   };
};

enum Map_EnumSmRpOa
{
   MAP_MSISDN = 0,
   MAP_ORIG_SC_ADDR = 1,
   MAP_NO_SM_RP_OA = 2
};

struct smRPOA
{
   Map_EnumSmRpOa enumSmRpOa;
   union
   {
      ISDNAddressString msisdn;
      ISDNAddressString serviceCentreAddressOA;
      BOOLEAN noSMRPOA;
   };
};

////////////////////////////////////////////////////////////////////////
//--------------------- Mt Forward SM SM -----------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_MT_FSM_REQ_MMS                BIT_01
#define OPTIONAL_T_MT_FSM_REQ_SM_DEL_TIME        BIT_02
#define OPTIONAL_T_MT_FSM_REQ_SM_DEL_START_TIME  BIT_03
struct mtforwardSMArg
{
   smRPDA smRpDa;
   smRPOA smRpOa;
   //smDeliverRPUI smRPUIData;
   TDArray smRpUi;
   UINT32 optional_params;
   //BOOLEAN moreMsgToSend;
};

struct mtforwardSM
{
   smRPDA smRpDa;
   smRPOA smRpOa;
   UINT8  mti;
   union
   {
      smDeliverRPUI  smDELUIData;
      smStatusReportRPUI smSRUIData;
   };
   UINT32     optional_params;
   BOOLEAN    moreMsgToSend;
   INT16      smDeliveryTimer;
   UINT8      smDeliveryStartTime[4+1];
};

#define OPTIONAL_T_MT_FSM_RSP_SM_RP_UI         BIT_01
struct mtforwardSMRes
{
   UINT32 optional_params;
   smDeliveryRepAckUI smRepAck;
};

////////////////////////////////////////////////////////////////////////
//--------------------- Mo Forward SM SM -----------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_MO_FSM_SM_REQ_RP_DA_IMSI            BIT_01

struct moforwardSMArg
{
   smRPDA smRpDa;
   smRPOA smRpOa;
   //smSubmitRPUI smRPUIData;
   TDArray smRpUi;

   UINT32 optional_params;
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
};

struct moforwardSM
{
   smRPDA smRpDa;
   smRPOA smRpOa;
   UINT8 mti;
   union
   {
      smSubmitRPUI smSubmitUIData;
   };

   UINT32 optional_params;
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
};

#define OPTIONAL_T_MO_FSM_RSP_SM_RP_UI                 BIT_01

struct moforwardSMRes
{
   UINT32 optional_params;
   EnumOperationRsp rspType;
   union
   {
      smSubmitRepFailureUI smRepFailure;
      smSubmitRepAckUI smRepAck;
   };
};

////////////////////////////////////////////////////////////////////////
//--------------------- Send Routing Info For SM ---------------------//
////////////////////////////////////////////////////////////////////////
#define MAX_SM_RP_MTTI_LEN   10
#define OPTIONAL_T_SRI_SM_REQ_GPRS_SUPP_INDI      BIT_01
#define OPTIONAL_T_SRI_SM_REQ_SM_RP_MTI           BIT_02
#define OPTIONAL_T_SRI_SM_REQ_SM_RP_SMEA          BIT_03
#define OPTIONAL_T_SRI_SM_REQ_SM_RP_SMDEL         BIT_04
#define OPTIONAL_T_SRI_SM_REQ_SM_RP_IPSMSGW       BIT_05
#define OPTIONAL_T_SRI_SM_REQ_SM_RP_IMSI          BIT_06

struct routingInfoForSMArg
{
   ISDNAddressString msisdn;
   BOOLEAN           smRPPRI;
   ISDNAddressString serviceCentreAddress;

   UINT32            optional_params;
   BOOLEAN           gprsSupportIndicator;
   //INT16             smRPMTI[MAX_SM_RP_MTTI_LEN + 1];
   INT16             smRPMTI;
   TEXT              smRPSMEA[SS7_MAX_ADDRESS_LEN + 1];
   EnumDelIntended   smDeliveryNotIntended;
   BOOLEAN           ipSmGwGuidanceIndicator;
   TEXT              imsi[SS7_LMSI_LEN + 1];
};

typedef routingInfoForSMArg routingInfoForSMArgV3;

#define OPTIONAL_T_SRI_SM_RSP_LMSI                    BIT_01
#define OPTIONAL_T_SRI_SM_RSP_GPRS_NODE_INDI          BIT_02
#define OPTIONAL_T_SRI_SM_RSP_ADDNL_NUMBER            BIT_03

#define OPTIONAL_T_SRI_SM_RSP_NW_NODE_ADDR            BIT_04
#define OPTIONAL_T_SRI_SM_RSP_ADDNL_NW_NODE_ADDR      BIT_05
#define OPTIONAL_T_SRI_SM_RSP_IP_SM_GW_GUIDE          BIT_06

enum Map_EnumAdditionalNumber
{
   MAP_MSC_NUM = 0,
   MAP_SGSN_NUM = 1
};

struct additionalNumber
{
   Map_EnumAdditionalNumber enumAdditionalNumber;

   union
   {
      ISDNAddressString mscNumber;
      ISDNAddressString sgsnNumber;
   };
};

struct NwNodeDiameterAddr
{
   TEXT   diameterName[SS7_MAX_DIAMETER_NAME_LEN  +1];
   TEXT   diameterRealm[SS7_MAX_DIAMETER_NAME_LEN +1];
};

typedef NwNodeDiameterAddr AdditionalNwNodeDiameterAddr;

struct IpSmGwGuidance
{
   INT16  minDeliveryTimeValue;         // 30 - 600
   INT16  recommendedDeliveryTimeValue; // 30 - 600
};

struct routingInfoForSMRes
{
   TEXT                          imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString             networkNodeNumber;

   UINT32                        optional_params;
   TEXT                          lmsi[SS7_MAX_ADDRESS_LEN + 1];
   BOOLEAN                       gprsNodeIndicator;
   additionalNumber              additionalNumberDetails;
   NwNodeDiameterAddr            nwNodeDiameterAddrDetails;
   AdditionalNwNodeDiameterAddr  additionalNwNodeDiameterAddrDetails;
   IpSmGwGuidance                ipSmGwGuidanceDetails;
};
typedef routingInfoForSMRes routingInfoForSMResV3;

#define OPTIONAL_T_SRI_SM_V1_REQ_CUG_INTER_LOCK      BIT_01
#define OPTIONAL_T_SRI_SM_V1_REQ_TELESERVICE_CODE    BIT_02
struct routingInfoForSMArgV1
{
   ISDNAddressString msisdn;
   BOOLEAN smRPPRI;
   ISDNAddressString serviceCentreAddress;

   UINT32 optional_params;
   TEXT   cugInterLock[4];
   UINT8  teleServiceCode;
};

#define OPTIONAL_T_SRI_SM_V1_RSP_ROAMING_NUMBER      BIT_01
#define OPTIONAL_T_SRI_SM_V1_RSP_MSC_NUMBER          BIT_02
#define OPTIONAL_T_SRI_SM_V1_RSP_LMSI                BIT_03
#define OPTIONAL_T_SRI_SM_V1_RSP_FORWARDED_TO_NUMBER BIT_04
#define OPTIONAL_T_SRI_SM_V1_RSP_FORWARDING_OPTIONS  BIT_05
#define OPTIONAL_T_SRI_SM_V1_RSP_MWD_SET             BIT_06
struct routingInfoForSMResV1
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];

   UINT32 optional_params;
   ISDNAddressString roamingNumber;
   ISDNAddressString mscNumber;
   TEXT lmsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString forwardedToNumber;
   UINT8 forwardingOptions;
   BOOLEAN mwdSet;
};

#define OPTIONAL_T_SRI_SM_V2_REQ_TELESERVICE_CODE    BIT_01
struct routingInfoForSMArgV2
{
   ISDNAddressString msisdn;
   BOOLEAN smRPPRI;
   ISDNAddressString serviceCentreAddress;

   UINT32 optional_params;
   UINT8  teleServiceCode;
};

#define OPTIONAL_T_SRI_SM_V2_RSP_ROAMING_NUMBER      BIT_01
#define OPTIONAL_T_SRI_SM_V2_RSP_MSC_NUMBER          BIT_02
#define OPTIONAL_T_SRI_SM_V2_RSP_LMSI                BIT_03
#define OPTIONAL_T_SRI_SM_V2_RSP_MWD_SET             BIT_04
struct routingInfoForSMResV2
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];

   UINT32 optional_params;
   ISDNAddressString roamingNumber;
   ISDNAddressString mscNumber;
   TEXT lmsi[SS7_MAX_ADDRESS_LEN + 1];
   BOOLEAN mwdSet;
};

struct sendRoutingInfoForSmArg
{
   UINT8 version;
   union
   {
   routingInfoForSMArgV1     ver1Info;
   routingInfoForSMArgV2     ver2Info;
   routingInfoForSMArgV3     ver3Info;
   };
};

struct sendRoutingInfoForSmRes
{
   UINT8 version;
   union
   {
   routingInfoForSMResV1     ver1Info;
   routingInfoForSMResV2     ver2Info;
   routingInfoForSMResV3     ver3Info;
   };
};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//----------------------- Inform Service Centre ----------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_INFORM_SERVICE_CENTRE_REQ_MSISDN           BIT_01
#define OPTIONAL_T_INFORM_SERVICE_CENTRE_MW_STATUS            BIT_02
#define OPTIONAL_T_INFORM_SERVICE_CENTRE_ABS_SUB_DIAG         BIT_03
#define OPTIONAL_T_INFORM_SERVICE_CENTRE_ADDNL_ABS_SUB_DIAG   BIT_04

#define SC_ADDRESS_NOT_INCLUDED 0
#define MNRF_SET                1
#define MCEF_SET                2
#define MNRG_SET                3

struct informServiceCentreArg
{
   UINT32              optional_params;
   ISDNAddressString   storedMSISDN;
   UINT16              mwStatus;
   // In mwStatus if 1 bit is set means sc-AddressNotIncluded
   //                2 bit is set means mnrf-Set
   //                3 bit is set means mcef-Set
   //                4 bit is set means mnrg-Set
   INT16               absentSubscriberDiagnosticSM;
   INT16               additionalAbsentSubscriberDiagnosticSM;
};

////////////////////////////////////////////////////////////////////////
//------------------------- Send Routing Info ------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_SEND_ROUTING_INFO_CUG_CHECK_INFO_CUG_OUT_GOING_ACCESS     BIT_01
#define OPTIONAL_T_SEND_ROUTING_INFO_CUG_CHECK_INFO_EXTENSION_CONTAINER      BIT_02
struct cugCheckInfo
{
   TEXT cugInterLock[4];

   UINT32 optional_params;
   BOOLEAN cugOutGoingAccess;
   ExtensionContainer extensionContainer;
};

enum interrogationType
{
   MAP_BASIC_CALL = 0,
   MAP_FORWARDING = 1
};

enum forwardingReason
{
   MAP_NOT_REACHABLE = 0,
   MAP_BUSY = 1,
   MAP_NO_REPLY = 2
};

enum EnumBasicSerGroup
{
   MAP_EXT_BEARER_SERVICE = 0,
   MAP_EXT_TELE_SERVICE = 1
};

struct basicServiceGroup
{
   EnumBasicSerGroup enumBasicSerGroup;

   union
   {
      TEXT extBearerService[SS7_MAX_ADDRESS_LEN + 1];
      TEXT extTeleService[SS7_MAX_ADDRESS_LEN + 1];
   };
};

//enum ProtocolId
enum protocolId
{
   MAP_GSM_0408 = 1,
   MAP_GSM_0806 = 2,
   MAP_GSM_BSSMAP = 3,
   MAP_ETS_300102_1 = 4
};

#define OPTIONAL_NETWORK_SIGNAL_INFO_EXT_CONTAINER     BIT_01
struct networkSignalInfo
{
   UINT32               optional_params;
   protocolId           protocolIdDetails;
   TEXT                 sigInfo[200];
   ExtensionContainer   extensionContainer;
};

#define OPTIONAL_T_SUPPRESS_T_CSI     BIT_01
struct camelInfo
{
   UINT16 supportedCamelPhase; // In this if 1 bit set means phase1
   //            2 bit set means phase2
   UINT32 optional_params;
   BOOLEAN suppressTCSI;
};

enum extProtocolId
{
   MAP_EST_300356 = 1
};

#define OPTIONAL_T_ADDITIONAL_SIGNAL_INFO_EXT_CONTAINER      BIT_15
struct additionalSignalInfo
{
   extProtocolId extProtocolIdDetails;
   TEXT sigInfo[200];
   ExtensionContainer   extensionContainer;
};


#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_CUG_CHECK_INFO       BIT_01
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_NUM_OF_FORWARDING    BIT_02
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_OR_INTERROGATION     BIT_03
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_OR_CAPABILITY        BIT_04
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_CALL_REF_NUM         BIT_05
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_FORW_REASONS         BIT_06
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_BASIC_SER_GROUP      BIT_07
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_NW_SIG_INFO          BIT_08
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_CAMEL_DETAILS        BIT_09
//#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_SUPPRESS_TCSI        BIT_09
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_SUPRESSION_OF_ANNO   BIT_10
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_ALERTING_PATTERN     BIT_11
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_CCBS_CALL            BIT_12
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_SUPP_CCBS_PHASE      BIT_13
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_ADD_SIG_INGO         BIT_14
#define OPTIONAL_T_SEND_ROUTING_INFO_REQ_EXT_CONTAINER        BIT_15

struct sendRoutingInfoArg
{
   ISDNAddressString    msisdn;
   interrogationType    interrogationTypeDetails;
   ISDNAddressString    gmscAddr;

   UINT32               optional_params;
   cugCheckInfo         cugCheckInfoDetails;
   int                  numOfForwardingDetails;
   BOOLEAN              orInterrogationDetails;
   int                  orCapability;
   TEXT                 callRefNum[SS7_MAX_ADDRESS_LEN + 1];
   forwardingReason     forwardingReasonDetails;
   basicServiceGroup    basicServiceGroupDetails;
   networkSignalInfo    networkSignalInfoDetails;
   camelInfo            camelInfoDetails;
   BOOLEAN              supressionOfAnnouncement;
   ExtensionContainer   extensionContainer;
   TEXT                 alertingPattern;
   BOOLEAN              ccbsCall;
   UINT8                suppCcbsPhase;
   additionalSignalInfo additionalSignalInfoDetails;
};

// SendRoutingInfo Response
#define OPTIONAL_T_FORWARDING_DATA_FORWARDED_TO_NUM         BIT_01
#define OPTIONAL_T_FORWARDING_DATA_FORWARDED_TO_SUBADDRESS  BIT_02
#define OPTIONAL_T_FORWARDING_DATA_FORWARDING_OPTIONS       BIT_03
#define OPTIONAL_T_FORWARDING_DATA_LONG_FORWARDED_TO_NUMBER BIT_04

struct forwardingData
{
   UINT32 optional_params;
   ISDNAddressString forwardedToNum;
   ISDNAddressString forwardedToSubaddress;
   TEXT forwardingOptions;
   ISDNAddressString longForwardedToNumber;
};

enum EnumRoutingInfo
{
   MAP_ROAMING_NUM = 0,
   MAP_FORWARDING_DATA = 1
};

struct routingInfo
{
   EnumRoutingInfo enumRoutingInfo;

   union
   {
      ISDNAddressString roamingNumber;
      forwardingData forwardingDataDetails;
   };
};

enum tBcsmTrigDetPoint
{
   MAP_TERM_ATTEMPT_AUTHORIZED = 12
};

enum defaultCallHandling
{
   MAP_CONTINUE_CALL = 0,
   MAP_RELEASE_CALL = 1
};

struct tBcsmTriggerDetectionPoint
{
   tBcsmTrigDetPoint tBcsmTrigDetPointDetails;
   int serviceKey;
   ISDNAddressString gsmScfAddr;
   defaultCallHandling defaultCallHandlingDetails;
};

#define OPTIONAL_T_T_CSI_CAMEL_CAP_HAND  BIT_01

struct tCsi
{
   tBcsmTriggerDetectionPoint tBcsmTriggerDetectionPointDetails;

   UINT32 optional_params;
   UINT8 camelCapaHand;
};

enum oBcsmTriggerDetectionPoint
{
   MAP_COLLECTED_INFO = 2
};

struct oBcsmCamelTDPDataList
{
   oBcsmTriggerDetectionPoint oBcsmTriggerDetectionPointDetails;
   int serviceKey;
   //TEXT gsmSCFAddress[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString gsmSCFAddress;
   defaultCallHandling defaultCallHandlingDetails;
};

#define OPTIONAL_T_O_CSI_CAMEL_CAP_HAND  BIT_01
struct oCsi
{
   oBcsmCamelTDPDataList oBcsmCamelTDPDataListDetails;

   UINT32 optional_params;
   UINT8 camelCapHand;
};

enum matchType
{
   MAP_INHIBITING = 0,
   MAP_ENABLING = 1
};

#define OPTIONAL_T_DESTINATION_NUMBER_CRITERIA_DEST_NUM_LIST      BIT_01
#define OPTIONAL_T_DESTINATION_NUMBER_CRITERIA_DEST_NUM_LEN_LIST  BIT_02

struct destinationNumberCriteria
{
   matchType matchTypeDetails;

   UINT32 optional_params;
   TEXT destNumList[10][SS7_MAX_ADDRESS_LEN + 1];
   int destNumLenList[3];
};

enum EnumBasicServiceCriteria
{
   MAP_EXT_BEARERsERVICE = 0,
   MAP_EXT_TELESERVICE = 1
};

struct basicServiceCriteria
{
   EnumBasicServiceCriteria enumBasicServiceCriteria;

   union
   {
      TEXT extBearerService[5];
      TEXT extTeleService[5];
   };
};

enum callTypeCriteria
{
   MAP_FORWARDED = 0,
   MAP_NOT_FORWARDED = 1
};

#define OPTIONAL_T_O_BCSM_CAMEL_TDP_CRITERIA_LIST_DESTINATION_NUMBER_CRITERIA  BIT_01
#define OPTIONAL_T_O_BCSM_CAMEL_TDP_CRITERIA_LIST_BASIC_SERVICE_CRITERIA       BIT_02
#define OPTIONAL_T_O_BCSM_CAMEL_TDP_CRITERIA_LIST_CALL_TYPE_CRITERIA           BIT_03

struct oBcsmCamelTDPCriteriaList
{
   oBcsmTriggerDetectionPoint oBcsmTriggerDetectionPointDetails;

   UINT32 optional_params;
   destinationNumberCriteria destinationNumberCriteriaDetails;
   basicServiceCriteria basicServiceCriteriaDetails;
   callTypeCriteria callTypeCriteriaDetails;
};

#define OPTIONAL_T_GMSC_CAMEL_SUBSCRIPTION_INFO_T_CSI                           BIT_01
#define OPTIONAL_T_GMSC_CAMEL_SUBSCRIPTION_INFO_O_CSI                           BIT_02
#define OPTIONAL_T_GMSC_CAMEL_SUBSCRIPTION_INFO_O_BCSM_CAMEL_TDP_CRITERIA_LIST  BIT_03

struct GmscCamelSubscriptionInfo
{
   UINT32 optional_params;
   tCsi tCsiDetails;
   oCsi oCsiDetails;
   oBcsmCamelTDPCriteriaList oBcsmCamelTDPCriteriaListDetails[5];
};

#define OPTIONAL_T_CAMEL_ROUTING_INFO_FORWARDING_DATA  BIT_01

struct camelRoutingInfo
{
   GmscCamelSubscriptionInfo gmscCamelSubscriptionInfoDetails;

   UINT32 optional_params;
   forwardingData forwardingDataDetails;
};

enum EnumExtRoutingInfo
{
   MAP_ROUTING_INFO = 0,
   MAP_CAMEL_ROUTING_INFO = 1
};

struct extendedRoutingInfo
{
   EnumExtRoutingInfo enumExtRoutingInfo;

   union
   {
      routingInfo routingInfoDetails;
      camelRoutingInfo camelRoutingInfoDetails;
   };
};

enum EnumSubscriberState
{
   MAP_ASSUMED_IDLE = 0,
   MAP_CAMEL_BUSY = 1,
   MAP_NET_DET_NOT_REACHABLE = 2,
   MAP_NOT_PROVIDED_FROM_VLR = 3
};

enum netDetNotReachable // should be matching with spec
{
   MAP_MS_PURGED = 0,
   MAP_IMSI_DETACHED = 1,
   MAP_RESTRICTED_AREA = 2,
   MAP_NOT_REGISTERED = 3
};
struct subscriberState
{
   EnumSubscriberState enumSubscriberState;
   netDetNotReachable netDetNotReachableDetails;
};

#define OPTIONAL_T_SUBSCRIBER_INFO_LOCATION_INFORMATION   BIT_01
#define OPTIONAL_T_SUBSCRIBER_INFO_SUBSCRIBER_STATE       BIT_02
struct subscriberInfo
{
   UINT32 optional_params;
   LocationInfo    locationInformationDetails;
   subscriberState subscriberStateDetails;
};

struct naeaPreferredCI
{
   TEXT naeaPreferredCIC[3];
};

#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_CCBS_INDICATORS_CCBS_POSSIBLE              BIT_01
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_CCBS_INDICATORS_KEEP_CCBS_CALL_INDICATOR   BIT_02
struct ccbsIndicators
{
   UINT32 optional_params;
   BOOLEAN ccbsPossible;
   BOOLEAN keepCCBSCallIndicator;
};

enum numPortabilityStatus
{
   MAP_NOT_KNOWN_TO_BE_PORTED = 0,
   MAP_OWN_NUMBER_PORTED_OUT = 1,
   MAP_FOREIGN_NUMBER_PORTED_TO_FOREIGN_NETWORK = 2
};

#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_IMSI                         BIT_01
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_EXTENDED_ROUTING_INFO        BIT_02
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_CUG_CHECK_INFO               BIT_03
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_CUG_SUBSCRIPTION_FLAG        BIT_04
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_SUBSCRIBER_INFO              BIT_05
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_SS_LIST                      BIT_06
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_BASIC_SERVICE                BIT_07
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_FORW_INTER_REQUIRED          BIT_08
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_VMSC_ADDRESS                 BIT_09
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_NAEA_PREFERRED_CI            BIT_10
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_CCBS_INDICATORS              BIT_11
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_MSISDN                       BIT_12
#define OPTIONAL_T_SEND_ROUTING_INFO_RSP_NUMBER_PORTABILITY_STATUS    BIT_13

struct sendRoutingInfoRsp2
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   routingInfo routingInfoDetails;

   UINT32 optional_params;
   cugCheckInfo cugCheckInfoDetails;
};

struct sendRoutingInfoRsp
{
   UINT32 optional_params;
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   extendedRoutingInfo extendedRoutingInfoDetails;
   cugCheckInfo cugCheckInfoDetails;
   BOOLEAN cugSubscriptionFlag;
   subscriberInfo subscriberInfoDetails;
   TEXT ssList[30];
   basicServiceCriteria basicServiceDetails;
   BOOLEAN forwInterRequired;
   ISDNAddressString vmscAddress;
   naeaPreferredCI naeaPreferredCIDetails;
   ccbsIndicators ccbsIndicatorsDetails;
   ISDNAddressString msisdn;
   numPortabilityStatus numberPortabilityStatusDetails;
};

////////////////////////////////////////////////////////////////////////
//----------------------- Provide Roaming Number ---------------------//
////////////////////////////////////////////////////////////////////////
struct gsmBearerCapability
{
   protocolId protocolIdDetails;
   TEXT signalInfo[200];
};

#define OPTIONAL_T_PROVIDE_ROAMING_NUM_ARG_MSISDN                BIT_01
#define OPTIONAL_T_PROVIDE_ROAMING_NUM_ARG_LMSI                  BIT_02
#define OPTIONAL_T_PROVIDE_ROAMING_NUM_ARG_GSM_BEARER_CAPABILITY BIT_03
#define OPTIONAL_T_PROVIDE_ROAMING_NUM_ARG_NETWORK_SIGNAL_INFO   BIT_04
#define OPTIONAL_T_PROVIDE_ROAMING_NUM_ARG_GMSC_ADDR             BIT_05

struct provideRoamingNumberArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString mscNumber;

   UINT32 optional_params;
   ISDNAddressString msisdn;
   TEXT lmsi[SS7_MAX_ADDRESS_LEN + 1];
   gsmBearerCapability gsmBearerCapabilityDetails;
   networkSignalInfo networkSignalInfoDetails;
   ISDNAddressString gmscAddr;
};

struct provideRoamingNumberRsp
{
   ISDNAddressString roamingNumber;
   //TEXT roamingNumber[SS7_MAX_ADDRESS_LEN + 1];
};

////////////////////////////////////////////////////////////////////////
//----------------------- Provide Subscriber Info --------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_REQUSETED_INFO_LOCATION_INFORMATION     BIT_01
#define OPTIONAL_T_REQUSETED_INFO_SUBSCRIBER_STATE         BIT_02
#define OPTIONAL_T_REQUSETED_INFO_EXTENSION_CONTAINER      BIT_03
#define OPTIONAL_T_REQUSETED_INFO_IMEI                     BIT_04
#define OPTIONAL_T_REQUSETED_INFO_CURRENT_LOCATION         BIT_05
#define OPTIONAL_T_REQUSETED_INFO_REQUESTED_DOMAIN         BIT_06
#define OPTIONAL_T_REQUSETED_INFO_MS_CLASSMARK             BIT_07
#define OPTIONAL_T_REQUSETED_INFO_MNP_REQUESTED_INFO       BIT_08
struct requestedInfo
{
   UINT32 optional_params;
   UINT8  requestedDomain;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_T_PSI_LMSI               BIT_01
#define OPTIONAL_T_PSI_REQUSETED_INFO     BIT_02
struct provideSubscriberInfoArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];

   UINT32 optional_params;
   TEXT lmsi[SS7_MAX_ADDRESS_LEN + 1];
   requestedInfo requestedInfoDetails;
};

//Psi Response
struct provideSubscriberInfoRes
{
   subscriberInfo subscriberInfoDetails;
   TEXT           imei[SS7_MAX_IMEI_LEN+1];
};

////////////////////////////////////////////////////////////////////////
//----------------------- Any Time Interrogation  --------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_T_ATI_REQUSETED_INFO     BIT_01
enum EnumRequestedDomain
{
   EnumCS_Domain = 0,
   EnumPS_Domain = 1
};
struct requested_atiInfo
{
   UINT32              optional_params;
   subscriberInfo      subscriberAtiDetails;
   ExtensionContainer  extensionContainer;
   BOOLEAN             imei;
   BOOLEAN             currentLocation;
   EnumRequestedDomain domainDetails;
   BOOLEAN             ms_classmark;
   BOOLEAN             mnpRequestedInfo;
};
struct anyTimeInterrogationArg
{
   TEXT                imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString   msisdn;
   ISDNAddressString   gsmSCFAddress;
   UINT32              optional_params;
   requested_atiInfo   requestedDetails;
};


//ATI Response
struct anyTimeInterrogationRes
{
   subscriberInfo subscriberInfoDetails;
   TEXT           imei[SS7_MAX_IMEI_LEN+1];
};
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_VLR_CAPABILITY_SUPPORTED_CAMEL_PHASES            BIT_01
#define OPTIONAL_VLR_CAPABILITY_SOLSA_SUPPORT                     BIT_02
#define OPTIONAL_VLR_CAPABILITY_IST_SUPPORT                       BIT_03
#define OPTIONAL_VLR_CAPABILITY_SUPER_CHARGER_SUPPORT             BIT_04
#define OPTIONAL_VLR_CAPABILITY_LONG_FTN_SUPPORT                  BIT_05
#define OPTIONAL_VLR_CAPABILITY_SUPPORTED_LCS_CAPABILITY          BIT_06

struct VlrCapability
{
   UINT32 optional_params;
   UINT8 supportedCamelPhases;
   UINT8 supportedLcsCapabilitySets;
};

////////////////////////////////////////////////////////////////////////
//----------------------- BeginSubscriberActivity --------------------//
////////////////////////////////////////////////////////////////////////
struct beginSubscriberActivityArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString originatingEntityNumber;
};

////////////////////////////////////////////////////////////////////////
//---------------------------- CancelLocation ------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumClIdentity
{
   CL_IDENTITY_IMSI,
   CL_IDENTITY_IMSI_WITH_LMSI
};

enum EnumClCancellationType // Value should not be changed
{
   CL_CANCEL_TYPE_UPDATE_PROCEDURE = 0,
   CL_CANCEL_TYPE_SUBSCRIPTION_WITHDRAW = 1
};

#define OPTIONAL_CL_CANCEL_TYPE                                   BIT_01

struct cancelLocationArg
{
   EnumClIdentity identity;
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   TEXT lmsi[SS7_LMSI_LEN + 1];
   UINT32 optional_params;
   EnumClCancellationType clType;
};

//----------------------------------CL RSP START -----------------------
struct cancelLocationRes
{
   ;
};

////////////////////////////////////////////////////////////////////////
//------------------------------ PurgeMS -----------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_PURGE_VLR                                        BIT_01
#define OPTIONAL_PURGE_SGSN                                       BIT_02

struct purgeMSArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   UINT32 optional_params;
   ISDNAddressString vlrNumber;
   ISDNAddressString sgsnNumber;
};

//----------------------------------PurgeMS RSP START ------------------
#define OPTIONAL_PURGE_FREEZE_TIMSI                               BIT_01
#define OPTIONAL_PURGE_FREEZEP_TIMSI                              BIT_02

struct purgeMSRes
{
   UINT32 optional_params;
};

////////////////////////////////////////////////////////////////////////
//----------------------------- ReadyForSM ---------------------------//
////////////////////////////////////////////////////////////////////////
enum EnumReadySmAlertReason // Value should not be changed
{
   READY_SM_ALERT_MS_PRESENT = 0,
   READY_SM_ALERT_MEMORY_AVALABLE = 1
};

#define OPTIONAL_READY_SM_ALERT_REASON_IND                        BIT_01
struct readyForSMArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   EnumReadySmAlertReason alertReason;
   UINT32 optional_params;
};

//----------------------------------RFS RSP START ----------------------
struct readyForSMRes
{
   ;
};

////////////////////////////////////////////////////////////////////////
//--------------------------- AlertServiceCentre ---------------------//
////////////////////////////////////////////////////////////////////////

#define OPTIONAL_ALERT_SC_IMSI                                    BIT_01

struct alertServiceCentreArg
{
   ISDNAddressString  msisdn;
   ISDNAddressString  serviceCentreAddress;

   UINT32             optional_params;
   TEXT               imsi[SS7_LMSI_LEN + 1];
};

//----------------------------------AlertSc RSP START ------------------
struct alertServiceCentreRes
{
   ;
};

////////////////////////////////////////////////////////////////////////
//---------------------------- RestoreData ---------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_RESTORE_DATA_LMSI                                BIT_01
#define OPTIONAL_RESTORE_DATA_VLR_CAPABILITY                      BIT_02

struct restoreDataArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   UINT32 optional_params;
   TEXT lmsi[SS7_LMSI_LEN + 1];
   VlrCapability vlrCapability;
};

#define OPTIONAL_RESTORE_DATA_MS_NOT_REACHABLE                    BIT_01

//----------------------------------RestoreData RSP START --------------
struct restoreDataRes
{
   ISDNAddressString hlrNumber;
   UINT32 optional_params;
};

////////////////////////////////////////////////////////////////////////
//--------------------------- UpdateLocation -------------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_UL_LMSI                                          BIT_01
#define OPTIONAL_UL_VLR_CAPABILITY                                BIT_02

struct updateLocationArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString mscNumber;
   ISDNAddressString vlrNumber;
   UINT32 optional_params;
   TEXT lmsi[SS7_LMSI_LEN + 1];
   VlrCapability vlrCapability;
};

#define OPTIONAL_UL_ADD_CAPABILITY                                BIT_01

//----------------------------------UL RSP START -----------------------
struct updateLocationRes
{
   ISDNAddressString hlrNumber;
   UINT32 optional_params;
};

////////////////////////////////////////////////////////////////////////
//----------------------- ReportSMDeliveryStatus ---------------------//
////////////////////////////////////////////////////////////////////////
#define OPTIONAL_REPSM_DEL_ABSCENT_SUB_DIAG           BIT_01
#define OPTIONAL_REPSM_DEL_GPRS_SUPP_INDI             BIT_02
#define OPTIONAL_REPSM_DEL_DEL_OUTCOME_INDI           BIT_03
#define OPTIONAL_REPSM_DEL_ADDL_DEL_OUTCOME           BIT_04
#define OPTIONAL_REPSM_DEL_ADDL_ABSCENT_SUB_DIAG      BIT_05
#define OPTIONAL_REPSM_DEL_IPSM_GW_INDI               BIT_06
#define OPTIONAL_REPSM_DEL_IPSM_GW_DEL_OUTCOME        BIT_07
#define OPTIONAL_REPSM_DEL_IPSM_GW_ABSCENT_SUB_DIAG   BIT_08
#define OPTIONAL_REPSM_DEL_IMSI                       BIT_09

enum EnumSmDeliveryOutcome
{
   SM_DELIVERY_MEMORY_CAPACITY_EXCEEDED = 0,
   SM_DELIVERY_ABSENT_SUBSCRIBER = 1,
   SM_DELIVERY_SUCCESSFUL_TRANSFER = 2
};

struct reportSMDeliveryStatusArg
{
   ISDNAddressString       msisdn;
   ISDNAddressString       serviceCentreAddress;
   EnumSmDeliveryOutcome   smDeliveryOutcome;

   UINT32                  optional_params;
   INT16                   absentSubsDiagnosticSM;
   BOOLEAN                 gprsSupportIndicator;
   BOOLEAN                 deliveryOutcomeIndicator;
   EnumSmDeliveryOutcome   additionalSMDeliveryOutcome;
   INT16                   additionalAbsentSubsDiagnosticSM;
   BOOLEAN                 ipSmGwIndicator;
   EnumSmDeliveryOutcome   ipSmGwSmDeliveryOutcome;
   INT16                   ipSmGwAbsentSubsDiagnosticSM;
   TEXT                    imsi[SS7_LMSI_LEN + 1];
}; 

//----------------------------------RSDS RSP START ---------------------
struct reportSMDeliveryStatusRes
{
   ISDNAddressString storedMSISDN;
}; // optional params needs to add

////////////////////////////////////////////////////////////////////////
//------------------------------- USSD -------------------------------//
////////////////////////////////////////////////////////////////////////
struct UssdString
{
   UINT8 ussdDataCodingScheme;
   UINT8 length;
   UINT8 data[SS7_MAX_8BIT_USSD_STRING_LEN + 1];
};

#define OPTIONAL_USSD_ALERTING_PATTERN                            BIT_01
#define OPTIONAL_USSD_MSISDN                                      BIT_02

struct ussdArg
{
   UssdString ussdData;
   UINT32 optional_params;
   UINT8 alertingPattern;
   ISDNAddressString msisdn;
};

//----------------------------------USSD RSP START ---------------------
struct ussdRes
{
   UssdString ussdData;
};

////////////////////////////////////////////////////////////////////////
//------------------------------- PUSSD ------------------------------//
////////////////////////////////////////////////////////////////////////
struct ssUserData
{
   TEXT ussdData[SS7_MAX_USSD_STRING_LEN + 1];
};

////////////////////////////////////////////////////////////////////////
//------------------------- InsertSubscriberData ---------------------//
////////////////////////////////////////////////////////////////////////
#define SS7_MAX_NUM_OF_BEARER_SERVICES              50
#define SS7_MAX_NUM_OF_TELE_SERVICES                50

#define SS7_MAX_BEARER_SERVICE_CODE_LEN             5
#define SS7_MAX_TELE_SERVICE_CODE_LEN               5

#define SS7_MAX_SS_STATUS                           5
#if 0
#define SS7_MAX_NUM_OF_SS                           30
#define SS7_MAX_NUM_OF_EXT_BASIC_SERVICE_GROUPS     32
#endif

#define SS7_MAX_NUM_OF_SS                           3
#define SS7_MAX_NUM_OF_EXT_BASIC_SERVICE_GROUPS     4

#define SS7_MAX_NUM_OF_CAMEL_TDP_DATA               10
#define SS7_MAX_NUM_OF_CAMEL_SS_EVENTS              10
#define SS7_MAX_NUM_OF_CAMEL_BASIC_SERVICE_CRITERIA 5
#define SS7_MAX_NUM_OF_CAMEL_DESTINATION_NUMBERS    10
#define SS7_MAX_NUM_OF_CAMEL_DESTINATION_NUMBER_LEN 3

//--------------------------Generic Start-------------------------------
struct ExtBearerServiceCode
{
   UINT8 extBearerServiceCode[SS7_EXT_BASIC_SERVICE_CODE_LEN + 1];
};

struct ExtTeleServiceCode
{
   UINT8 extTeleServiceCode[SS7_EXT_BASIC_SERVICE_CODE_LEN + 1];
};

enum EnumExtBasicServiceCode
{
   EXT_BASIC_SRV_CODE_BEARER_SRV_CODE,
   EXT_BASIC_SRV_CODE_TELE_SRV_CODE
};

union ExtBasicServiceCode
{
   ExtBearerServiceCode extBearerService;
   ExtTeleServiceCode extTeleService;
};

struct SSCode
{
   ;
};

enum O_BcsmTriggerDetectionPoint
{
   ISD_TDP_COLLECTED_INFO = 2
};

//--------------------------Generic End---------------------------------

struct LCSInformation
{
   ;
};

struct LSAInformation
{
   ;
};

struct NetworkAccessMode
{
   ;
};

struct GPRSSubscriptionData
{
   ;
};

struct NAEAPreferredCI
{
   ;
};

//--------------------------VLR CAMEL SUBS INFO START-------------------
enum CallTypeCriteria
{
   ISD_CALL_TYPE_FORWARDED = 0,
   ISD_CALL_TYPE_NOTFORWARDED = 1
};

struct BasicServiceCriteria
{
   EnumExtBasicServiceCode extBasicServiceCodeType;
   ExtBasicServiceCode
      extBasicServiceCode[SS7_MAX_NUM_OF_CAMEL_BASIC_SERVICE_CRITERIA + 1];
};

struct DestinationNumberList
{
   UINT8 numOfDestinationNumberList;
   TEXT destNumList[SS7_MAX_NUM_OF_CAMEL_DESTINATION_NUMBERS + 1][SS7_MAX_ADDRESS_LEN + 1];
};

struct DestinationNumberLengthList
{
   UINT8 numOfDestinationNumberLengthList;
   int destNumLenList[SS7_MAX_NUM_OF_CAMEL_DESTINATION_NUMBER_LEN + 1];
};

enum MatchType
{
   ISD_DEST_INHIBITING = 0,
   ISD_DEST_ENABLING = 1
};

#define OPTIONAL_DESTINATION_NUMBER_CRITERIA_NUMBER_LIST          BIT_01
#define OPTIONAL_DESTINATION_NUMBER_CRITERIA_NUMBER_LENGTH_LIST   BIT_02

struct DestinationNumberCriteria
{
   MatchType matchType;
   UINT32 optional_params;
   DestinationNumberList destinationNumberList;
   DestinationNumberLengthList destinationNumberLengthList;
};

#define OPTIONAL_O_BCSM_CAMEL_TDP_DESTINATION_NUMBER_CRITERIA     BIT_01
#define OPTIONAL_O_BCSM_CAMEL_TDP_BASIC_SERVICE_CRITERIA          BIT_02
#define OPTIONAL_O_BCSM_CAMEL_TDP_CALL_TYPE_CRITERIA              BIT_03

struct O_BcsmCamelTDP_Criteria
{
   O_BcsmTriggerDetectionPoint o_BcsmTriggerDetectionPoint;
   UINT32 optional_params;
   DestinationNumberCriteria destinationNumberCriteria;
   BasicServiceCriteria basicServiceCriteria;
   CallTypeCriteria callTypeCriteria;
};

struct O_BcsmCamelTDPCriteriaList
{
   UINT8 numOfO_BcsmCamelTDP_Criteria;
   O_BcsmCamelTDP_Criteria o_BcsmCamelTDPCriteria[SS7_MAX_NUM_OF_CAMEL_TDP_DATA + 1];
};

struct SS_EventList
{
   SSCode ssCode[SS7_MAX_NUM_OF_CAMEL_SS_EVENTS + 1];
};

#define OPTIONAL_SS_CAMEL_DATA_EXTENSION_CONTAINER                BIT_01

struct SS_CamelData
{
   SS_EventList ssEventList;
   TEXT gsmSCFAddress[SS7_MAX_ADDRESS_LEN + 1];
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_SS_CSI_EXTENSION_CONTAINER                       BIT_01

struct SS_CSI
{
   SS_CamelData ssCamelData;
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

enum DefaultCallHandling
{
   ISD_DEF_CALL_HDLG_CONTINUECALL = 0,
   ISD_DEF_CALL_HDLG_RELEASECALL = 1
};

#define OPTIONAL_O_BCSM_CAMEL_TDP_DATA_EXTENSION_CONTAINER         BIT_01

struct O_BcsmCamelTDPData
{
   O_BcsmTriggerDetectionPoint o_BcsmTriggerDetectionPoint;
   UINT32 serviceKey;
   ISDNAddressString gsmSCFAddress;
   DefaultCallHandling defaultCallHandling;
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

struct O_BcsmCamelTDPDataList
{
   UINT8 numOfO_BcsmCamelTDPData;
   O_BcsmCamelTDPData o_BcsmCamelTDPData[SS7_MAX_NUM_OF_CAMEL_TDP_DATA + 1];
};

#define OPTIONAL_O_CSI_EXTENSION_CONTAINER                        BIT_01
#define OPTIONAL_O_CSI_CAMEL_CAPABILITY_HANDLING                  BIT_02
#define OPTIONAL_O_CSI_NOTIFICATION_TO_CSE                        BIT_03
#define OPTIONAL_O_CSI_CSI_ACTIVE                                 BIT_04
struct O_CSI
{
   O_BcsmCamelTDPDataList o_BcsmCamelTDPDataList;
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
   UINT16 camelCapabilityHandling;
};

enum DefaultSmsHandling
{
   ISD_SMS_HANDLING_CONTINUE = 0,
   ISD_SMS_HANDLING_RELEASE = 1
};

enum Sms_TriggerDetectionPoint
{
   ISD_SMS_TDP_SMS_COLLECTED_INFO = 1,
   ISD_SMS_TDP_SMS_DELIVERY_REQUEST = 2
};

#define OPTIONAL_SMS_CAMEL_TDP_DATA_EXTENSION_CONTAINER           BIT_01

struct Sms_CAMEL_TDP_Data
{
   Sms_TriggerDetectionPoint sms_TriggerDetectionPoint;
   UINT32 serviceKey;
   ISDNAddressString gsmSCFAddress;
   DefaultSmsHandling defaultSmsHandling;
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

struct Sms_CAMEL_TDP_DataList
{
   UINT8 numOfSms_CAMEL_TDP_Data;
   Sms_CAMEL_TDP_Data sms_CAMEL_TDP_Data[SS7_MAX_NUM_OF_CAMEL_TDP_DATA + 1];
};

#define OPTIONAL_MO_SMS_CSI_SMS_CAMEL_TDP_DATA_LIST               BIT_01
#define OPTIONAL_MO_SMS_CSI_CAMEL_CAPABILITY_HANDLING             BIT_02
#define OPTIONAL_MO_SMS_CSI_EXTENSION_CONTAINER                   BIT_03
#define OPTIONAL_MO_SMS_CSI_NOTIFICATION_TO_CSE                   BIT_04
#define OPTIONAL_MO_SMS_CSI_ACTIVE                                BIT_05
struct Mo_sms_CSI
{
   UINT32 optional_params;
   Sms_CAMEL_TDP_DataList sms_CAMEL_TDP_DataList;
   UINT16 camelCapabilityHandling;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_VLR_CAMEL_SUBS_INFO_O_CSI                        BIT_01
#define OPTIONAL_VLR_CAMEL_SUBS_INFO_EXTENSION_CONTAINER          BIT_02
#define OPTIONAL_VLR_CAMEL_SUBS_INFO_SS_CSI                       BIT_03
#define OPTIONAL_VLR_CAMEL_SUBS_INFO_O_BCSM_TDP_CRITERIA_LIST     BIT_04
#define OPTIONAL_VLR_CAMEL_SUBS_INFO_TIF_CSI                      BIT_05
#define OPTIONAL_VLR_CAMEL_SUBS_INFO_MO_SMS_CSI                   BIT_06

struct VlrCamelSubscriptionInfo
{
   UINT32 optional_params;
   O_CSI o_CSI;
   ExtensionContainer extensionContainer;
   SS_CSI ss_CSI;
   O_BcsmCamelTDPCriteriaList o_BcsmCamelTDPCriteriaList;
   BOOLEAN tif_CSI;
   Mo_sms_CSI mo_sms_CSI;
};

//--------------------------VLR CAMEL SUBS INFO END-------------------

struct VGCSDataList
{
   ;
};

struct VBSDataList
{
   ;
};

struct ZoneCodeList
{
   ;
};

//----------------------------ODB DATA START----------------------------
struct ODB_HPLMNData
{
   UINT32 odbHplmnData;
   // BIT 0:   plmn-SpecificBarringType1 
   // BIT 1:   plmn-SpecificBarringType2 
   // BIT 2:   plmn-SpecificBarringType3 
   // BIT 3:   plmn-SpecificBarringType4 
   //-- exception handling: reception of unknown bit assignments in the
   //-- ODB-HPLMN-Data type shall be treated like unsupported ODB-HPLMN-Data
};

struct ODB_GeneralData
{
   UINT32 odbGenData;
   // Bit 0 :   allOG-CallsBarred
   // Bit 1 :   internationalOGCallsBarred
   // Bit 2 :   internationalOGCallsNotToHPLMN-CountryBarred
   // Bit 3 :   premiumRateInformationOGCallsBarre
   // Bit 4 :   premiumRateEntertainementOGCallsBarred
   // Bit 5 :   ss-AccessBarred,
   // Bit 6 :   interzonalOGCallsBarred
   // Bit 7 :   interzonalOGCallsNotToHPLMN-CountryBarred
   // Bit 8 :   interzonalOGCallsAndInternationalOGCallsNotToHPLMN-CountryBarred
   // Bit 9 :   allECT-Barred
   // Bit 10:   chargeableECT-Barred
   // Bit 11:   internationalECT-Barred
   // Bit 12:   interzonalECT-Barred
   // Bit 13:   doublyChargeableECT-Barred
   // Bit 14:   multipleECT-Barred
   //-- exception handling: reception of unknown bit assignments in the
   // -- ODB-GeneralData type shall be treated like unsupported ODB-GeneralData
};

#define OPTIONAL_ODB_HPLMN_DATA                                   BIT_01
#define OPTIONAL_ODB_EXTENSION_CONTAINER                          BIT_02

struct ODBData
{
   ODB_GeneralData odbGeneralData;
   UINT32 optional_params;
   ODB_HPLMNData odbHPLMNData;
   ExtensionContainer extensionContainer;
};
//--------------------------ODB DATA END--------------------------------

//--------------------------SS INFO START-------------------------------
struct ExtSSStatus
{
   TEXT ssStatus[5];
};

#define OPTIONAL_CALL_BARRING_FEATURE_BASIC_SERVICE               BIT_01
#define OPTIONAL_CALL_BARRING_FEATURE_EXTENSION_CONTAINER         BIT_02

struct ExtCallBarringFeature
{
   UINT32 optional_params;
   EnumExtBasicServiceCode extBasicServiceCodeType;
   ExtBasicServiceCode basicService;
   ExtSSStatus ssStatus;
   ExtensionContainer extensionContainer;
};

struct EMLPPInfo
{
   ;
};

struct ExtSSData
{
   ;
};

struct CUGInfo
{
   ;
};

//#define OPTIONAL_CALL_BARRING_FEATURE_EXTENSION_CONTAINER          BIT_01
struct ExtCallBarFeatureList
{
   UINT8 numOfExtCallBarringFeature;
   ExtCallBarringFeature
      extCallBarringFeature[SS7_MAX_NUM_OF_EXT_BASIC_SERVICE_GROUPS + 1];

   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_CALL_BARRING_EXTENSION_CONTAINER                 BIT_01
#define OPTIONAL_CALL_BARRING_SS_CODE                             BIT_02  // As for ACTIVE/DEACTIVATE SS
struct ExtCallBarInfo
{
   UINT8 ssCode;
   ExtCallBarFeatureList callBarringFeatureList;
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_FORW_FEATURE_LIST_BASIC_SERV                     BIT_01
#define OPTIONAL_FORW_FEATURE_LIST_FWD_NO                         BIT_02
#define OPTIONAL_FORW_FEATURE_LIST_FWD_SUB_ADDR                   BIT_03
#define OPTIONAL_FORW_FEATURE_LIST_FWD_OPTION                     BIT_04
#define OPTIONAL_FORW_FEATURE_LIST_NO_REPLY_TIME                  BIT_05
#define OPTIONAL_FORW_FEATURE_LIST_EXTENSION_CONTAINER            BIT_06
struct ExtForwFeatureList
{
   EnumExtBasicServiceCode extBasicServiceCodeType;
   ExtBasicServiceCode     basicService;

   ExtSSStatus             ssStatus;
   
   ISDNAddressString forwardedToNum;
   ISDNAddressString forwardedToSubaddress;
   TEXT forwardingOptions[SS7_MAX_SS_STATUS + 1];
   UINT32 noReplyConditionTime;
   
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

#define OPTIONAL_FORW_INFO_EXTENSION_CONTAINER                    BIT_01

struct ExtForwInfo
{
   UINT8  ssCode;
   //ExtForwFeatureList forwardingFeatureList; 
   ExtForwFeatureList forwardingFeatureList[SS7_MAX_NUM_OF_EXT_BASIC_SERVICE_GROUPS + 1];;

   UINT8 numOfExtCallFwdFeature;
   
   UINT32 optional_params;
   ExtensionContainer extensionContainer;
};

union ExtSSInfo
{
   ExtForwInfo forwardingInfo;
   ExtCallBarInfo callBarringInfo;
   CUGInfo cugInfo;
   ExtSSData ssData;
   EMLPPInfo emlppInfo;
};

enum EnumExtSSInfo
{
   EXT_SS_EXT_FWD_INFO,
   EXT_SS_EXT_CALL_BARRING_INFO,
   EXT_SS_CUG_INFO,
   EXT_SS_EXT_DATA,
   EXT_SS_EMLPP_INFO
};

struct ExtSSInfoList
{
   UINT8 numOfExtSSInfoList;
   EnumExtSSInfo extSSInfoType[SS7_MAX_NUM_OF_SS + 1];
   ExtSSInfo extSSInfo[SS7_MAX_NUM_OF_SS + 1];
};
//--------------------------SS INFO END---------------------------------

//--------------------------BEARER & TELE SERVICE  START----------------
struct TeleServiceList
{
   ExtTeleServiceCode extTeleServiceCode[SS7_MAX_NUM_OF_TELE_SERVICES + 1];
};

struct BearerServiceList
{
   ExtBearerServiceCode extBearerServiceCode[SS7_MAX_NUM_OF_BEARER_SERVICES + 1];
};

//--------------------------BEARER & TELE SERVICE  END------------------
enum EnumSubscriberStatus
{
   ISD_SERVICE_GRANTED = 0,
   ISD_OPERATOR_DETERMINED_BARRING = 1
};

#define OPTIONAL_ISD_IMSI                                         BIT_01
#define OPTIONAL_ISD_MSISDN                                       BIT_02
#define OPTIONAL_ISD_CATEGORY                                     BIT_03

#define OPTIONAL_ISD_SUBSCRIBER_STATUS                            BIT_04
#define OPTIONAL_ISD_BEARER_SERVICE_LIST                          BIT_05
#define OPTIONAL_ISD_TELE_SERVICE_LIST                            BIT_06

#define OPTIONAL_ISD_PROVISIONED_SS                               BIT_07
#define OPTIONAL_ISD_ODB_DATA                                     BIT_08

#define OPTIONAL_ISD_ROAM_RESTRICT_DUE_TO_UNSUPPORTED_FEATURE     BIT_09

#define OPTIONAL_ISD_REGIONAL_SUBSCRIPTION_DATA                   BIT_10
#define OPTIONAL_ISD_VBS_SUBSCRIPTION_DATA                        BIT_11
#define OPTIONAL_ISD_VGCS_SUBSCRIPTION_DATA                       BIT_12
#define OPTIONAL_ISD_VLR_CAMEL_SUBSCRIPTION_INFO                  BIT_13
#define OPTIONAL_ISD_EXTENSION_CONTAINER                          BIT_14

#define OPTIONAL_ISD_NAEA_PREFERRED_CI                            BIT_15
#define OPTIONAL_ISD_GPRS_SUBSCRIPTION_DATA                       BIT_16
#define OPTIONAL_ISD_ROAM_RESTRICT_IN_SGSN_DUE_TO_UNSUPPORT_FEATURE BIT_17
#define OPTIONAL_ISD_NETWORK_ACCESS_MODE                          BIT_18
#define OPTIONAL_ISD_LSA_INFORMATION                              BIT_19
#define OPTIONAL_ISD_LMU_INDICATOR                                BIT_20
#define OPTIONAL_ISD_LCS_INFORMATION                              BIT_21

#define OPTIONAL_ISD_IST_TIMER                                    BIT_22
#define OPTIONAL_ISD_SUPER_CHARGER_SUPPORTED_IN_HLR               BIT_23
#define OPTIONAL_ISD_MC_SS_INFO                                   BIT_24
#define OPTIONAL_CS_ALLOCATION_RETENTION_PRIORITY                 BIT_25
#define OPTIONAL_SGSN_CAMEL_SUBSCRIPTION_INFO                     BIT_26
#define OPTIONAL_ISD_CHARGING_CHARACTERISTICS                     BIT_27
#define OPTIONAL_ISD_ACCESS_RESTRICTION_DATA                      BIT_28

struct insertSubscriberDataArg
{
   UINT32 optional_params;

   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];
   ISDNAddressString msisdn;
   UINT8 category;

   EnumSubscriberStatus subscriberStatus;
   UINT8 numOfBearerServices;
   BearerServiceList bearerServiceList;
   UINT8 numOfTeleServices;
   TeleServiceList teleServiceList;

   ExtSSInfoList provisionedSS;
   ODBData odbData;
   //ZoneCodeList regionalSubscriptionData;
   //VBSDataList vbsSubscriptionData;
   //VGCSDataList vgcsSubscriptionData;
   VlrCamelSubscriptionInfo vlrCamelSubscriptionInfo;

   //ExtensionContainer extensionContainer;
   //NAEAPreferredCI naeaPreferredCI;
   //GPRSSubscriptionData gprsSubscriptionData;
   //NetworkAccessMode networkAccessMode;
   //LSAInformation lsaInformation;
   //LCSInformation lcsInformation;
};
//----------------------------------ISD END-----------------------------

//----------------------------------ISD RSP START ----------------------
enum RegionalSubscriptionResponse
{

   ISD_REGIONAL_NETWORK_NODE_AREARESTRICTED = 0,
   ISD_REGIONAL_TOO_MANY_ZONE_CODES,
   ISD_REGIONAL_ZONE_CODES_CONFLICT,
   ISD_REGIONAL_REGIONAL_SUBSC_NOT_SUPPORTED
};

struct SS_List
{
   SSCode ssCode[SS7_MAX_NUM_OF_SS + 1];
};

#define OPTIONAL_ISD_RSP_TELE_SERVICE_LIST                        BIT_01
#define OPTIONAL_ISD_RSP_BEARER_SERVICE_LIST                      BIT_02
#define OPTIONAL_ISD_RSP_SS_LIST                                  BIT_03
#define OPTIONAL_ISD_RSP_ODB_GENERAL_DATA                         BIT_04
#define OPTIONAL_ISD_RSP_REGIONAL_SUBSCRIPTION_RESPONSE           BIT_05
#define OPTIONAL_ISD_RSP_SUPPORTED_CAMEL_PHASES                   BIT_06
#define OPTIONAL_ISD_RSP_EXTENSION_CONTAINER                      BIT_07

struct insertSubscriberDataRes
{
   UINT32 optional_params;

   TeleServiceList teleserviceList;
   BearerServiceList bearerServiceList;
   SS_List ssList;
   ODB_GeneralData odbGeneralData;
   RegionalSubscriptionResponse regionalSubscriptionResponse;
   UINT8 supportedCamelPhases;
   ExtensionContainer extensionContainer;
};
//----------------------------------ISD RSP END ------------------------

////////////////////////////////////////////////////////////////////////
//------------------------- DeleteSubscriberData ---------------------//
////////////////////////////////////////////////////////////////////////
struct BasicServiceList
{
   //EnumExtBasicServiceCode extBasicServiceCodeType[SS7_MAX_NUM_OF_DSD_BASIC_SRV_CODE +1]; //70
   //ExtBasicServiceCode  extBasicServiceCode[SS7_MAX_NUM_OF_DSD_BASIC_SRV_CODE +1]; //70
};

struct SpecificCSI_Withdraw
{
   UINT32 csi;
};

#define OPTIONAL_DSD_BASIC_SERVICE_LIST                           BIT_01
#define OPTIONAL_DSD_SS_LIST                                      BIT_02
#define OPTIONAL_DSD_ROAM_RESTRICTION                             BIT_03
#define OPTIONAL_DSD_CAMEL_SUBSCRIPTION_WITHDRAW                  BIT_04
#define OPTIONAL_DSD_CSI_WITHDRAW                                 BIT_05

struct deleteSubscriberDataArg
{
   TEXT imsi[SS7_MAX_ADDRESS_LEN + 1];

   UINT32 optional_params;

   BasicServiceList basicServiceList;
   //UINT8 ss_List[SS7_MAX_NUM_OF_DSD_SS_LIST +1];
   SpecificCSI_Withdraw specificCSI_Withdraw;
};

//----------------------------------DSD RSP ----------------------------
#define OPTIONAL_DSD_RSP_REG_SUBS_RSP                             BIT_01

struct deleteSubscriberDataRes
{
   UINT32 optional_params;

   RegionalSubscriptionResponse regionalSubscriptionResponse;
};
//----------------------------------DSD RSP END ------------------------

//--------------Supplementary services related services ----------------
#define  MAX_BEARER_SERV_LEN        30
#define  MAX_SUB_ADDR_LEN           25
#define  MAX_FWD_LEN                20
//#define  MAX_NO_OF_CFWD             15
//#define  MAX_NO_OF_CBS              15
#define  MAX_NO_OF_CFWD             5  //Modified for Collectcall Vinay,to reduce struct size
#define  MAX_NO_OF_CBS              5  //Modified for Collectcall Vinay
#define  MAX_GRP_SERV               2
#define  MAX_SSDATA_BASIC_SERV_GRP  13

enum BasicServGroup
{
   BEARER_SERV =1,
   TELE_SERV
};

enum CliRestriction
{
   PERMANENT = 0,
   TEMPORARYDEFAULTRESTRICTED,
   TEMPORARYDEFAULTALLOWED,
   CLIR_UNKNOWN
};

enum OverrideCategory
{
   OVERRIDE_ENABLED = 0,
   OVERRIDE_DISABLED = 1
};

enum SsSubscription
{
   CLIR = 1,      // CliRestriction is Present
   OVERRIDE,    // OverrideCategory is present.
   SSSUBOTHER   // Unknown
};

//enum IntSSRspComp
//{
//   SS_STATUS = 0,
//   BASICSERVGROUPLIST,
//   FORWARDINGFEATURELIST,
//   GENINFOLIST
//};

enum SSStatus
{
   CFWDALL     = 33,
   CFWDUN      = 40,
   CFWDCON     = 41,
   CFWDBUSY    = 42,
   CFWDNOANS   = 43,
   CFWDNOREACH = 36,
   CFWDUNKNOW  = 0
};

//struct CcbsFeatureList
//{
//   UINT32           ccbsIndex;
//   TEXT             bSubsNumber[MAX_MSISDN_LEN+1];
//   TEXT             bSubsAddress[MAX_SUB_ADDR_LEN +1];
//   BasicServGroup   basicServiceGroup;
//};

#define OPTIONAL_SS_FWD_SSCODE                BIT_01
struct ForwardingInfo
{
   UINT8              ssCode;
   UINT8              noOfFwdFeatures;
   ExtForwFeatureList forwardingFeatureList[MAX_NO_OF_CFWD + 1];;
   UINT32             optional_params;
};

//struct GenericServiceInfo
//{
//   SSStatus         ssStatus;
//   CliRestriction   cliRestriction;
//   UINT32           maxEntitledPriority;
//   UINT32           defaultPriority;
//   CcbsFeatureList  ccbsList;
//};

#define OPTIONAL_SS_CALL_BAR_LIST_BASIC_SERV_GRP   BIT_01
#define OPTIONAL_SS_CALL_BAR_LIST_SS_STATUS        BIT_02
struct CallBarList
{
   BasicServGroup    basicServGroup;
   UINT8             basicServiceCode;
   SSStatus          ssStatus;
   UINT32            optional_params;
};

#define OPTIONAL_SS_CALL_BAR_SS_CODE            BIT_01
struct CallBaringInfo
{
   UINT8           ssCode;
   UINT8           noOfCallBars;
   CallBarList     callbarlist[MAX_NO_OF_CBS +1];
   UINT32          optional_params;
};

#define OPTIONAL_SS_DATA_SS_CODE              BIT_01
#define OPTIONAL_SS_DATA_SS_STATUS            BIT_02
#define OPTIONAL_SS_DATA_SS_SUBSCRIPTION      BIT_03
#define OPTIONAL_SS_DATA_BASIC_SERV_GRP       BIT_04
#define OPTIONAL_SS_DATA_DEFAULT_PRIORITY     BIT_05
struct SsData
{
   UINT8             ssCode;
   SSStatus          ssStatus;
   SsSubscription    ssSubcomp;
   union
   {
      CliRestriction      cli;
      OverrideCategory    overlay;
   };
   
   UINT8              noOfBasicServs;
   struct 
   {
      BasicServGroup    basicServGroup;
      UINT8             basicServiceCode;
   } basicServ[MAX_SSDATA_BASIC_SERV_GRP + 1];

   UINT8             defaultPriority;
   UINT32            optional_params;
};

//struct IntterogateSSRsp
//{
//   IntSSRspComp         IntSSComp;
//   union
//   {
//      SSStatus             ssStatus;
//      BasicServGroup       basicServGroup[MAX_GRP_SERV +1];
//      ExtForwFeatureList   forwardingFeatureList[MAX_NO_OF_CFWD + 1];;
//      GenericServiceInfo   genServInfo;
//   }
//};

//struct IntterogateSSReq
//{
//   UINT8            ssCode;
//   BasicServGroup   basicServiceGroup;
//};

#define OPTIONAL_ACTIVATE_SS_BASIC_SERV            BIT_01
#define OPTIONAL_ACTIVATE_SS_LONG_FTN_SUPPORTED    BIT_02
struct ActivateSSReq
{
   UINT8            ssCode;
   BasicServGroup   basicServiceGroup;
   UINT32           optional_params;
};

enum ActDeactSSComp
{
   FWD_COMP = 1,
   BAR_COMP = 2,
   SS_COMP  = 3
};

struct ActivateSSRsp
{
   ActDeactSSComp      actSSComp;
   union
   {
      ForwardingInfo      fwdInfo;
      CallBaringInfo      callBarInfo;
      SsData              ssData;
   };
};

#define OPTIONAL_DEACTIVATE_SS_BASIC_SERV            BIT_01
#define OPTIONAL_DEACTIVATE_SS_LONG_FTN_SUPPORTED    BIT_02
struct DeactivateSSReq
{
   UINT8            ssCode;
   BasicServGroup   basicServiceGroup;
   UINT32           optional_params;
};

struct DeactivateSSRsp
{
   ActDeactSSComp      actSSComp;
   union
   {
      ForwardingInfo      fwdInfo;
      CallBaringInfo      callBarInfo;
      SsData              ssData;
   };
};

#define OPTIONAL_REG_SS_BASIC_SERV            BIT_01
#define OPTIONAL_REG_SS_FWD_TO_NUM            BIT_02
#define OPTIONAL_REG_SS_FWD_TO_SUB_ADD        BIT_03
#define OPTIONAL_REG_SS_NO_REPLY_COND_TIME    BIT_04
#define OPTIONAL_REG_SS_DEFAULT_PRIORITY      BIT_05
#define OPTIONAL_REG_SS_NBR_USER              BIT_06
#define OPTIONAL_REG_SS_LONG_FTN_SUPPORT      BIT_07
struct RegisterSSReq
{
   UINT8            ssCode;
   UINT32           optional_params;
   BasicServGroup   basicService;
   UINT8            serviceCode;
   TEXT             forwardedToNumber[SS7_MAX_ADDRESS_LEN+1];
   TEXT             forwardedToSubaddress[SS7_MAX_ADDRESS_LEN+1];
   INT16            noReplyConditionTime;
   INT16            defaultPriority;
   INT16            nbrUser;
   BOOLEAN          longFTNSupported;
};

struct RegisterSSRsp
{
   union
   {
      ForwardingInfo   fwdInfo;
      CallBaringInfo   callBarInfo;
      SsData           ssData;
   };
};

#define OPTIONAL_EARASE_SS_BASIC_SERV         BIT_01
#define OPTIONAL_ERASE_SS_LONG_FTN_SUPPORT    BIT_02
struct EraseSSReq
{
   UINT8            ssCode;
   UINT32           optional_params;
   BasicServGroup   basicService;
   UINT8            serviceCode;
   BOOLEAN          longFTNSupported;
};
struct EraseSSRsp
{
   union
   {
      ForwardingInfo   fwdInfo;
      CallBaringInfo   callBarInfo;
      SsData           ssData;
   };
};


//--------------Supplementary services related services END ------------

#endif
