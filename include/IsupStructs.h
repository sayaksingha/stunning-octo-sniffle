// @(#) $Id: IsupStructs.h,v 1.1.6.2 2012/12/03 14:24:28 shijuc Exp $
//----------------------------------------------------------------------------
// NAME IsupStructs.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
// This defines the various structures that are to be populated with regard
// to the Isup Messages.
//
// Originated : 29-FEB-2008                                        Narandra R.
//----------------------------------------------------------------------------

#ifndef INCL_ISUP_STRUCTS_H
#define INCL_ISUP_STRUCTS_H

//#include "TelesoftConstDef.h"

#ifndef TDAPI_H

/////////ISUP HASH DEFINES///////////
#define TDAPI_MAX_BYTE_OPT_LEN   256

#define TDAPI_MIN_CIRCUITS       0
#define TDAPI_MAX_CIRCUITS       4096

#define TDAPI_MIN_CIRCUIT_ID     1
#define TDAPI_MAX_CIRCUIT_ID     4096

#define TDAPI_MIN_CIRCUIT_ID_CODE  1
#define TDAPI_MAX_CIRCUIT_ID_CODE  4096

#define TDAPI_MIN_CKT_NI  1
#define TDAPI_MAX_CKT_NI  16383

#define TDAPI_MIN_CKT_CE_LEN  15
#define TDAPI_MAX_CKT_CE_LEN  20 

/* End of Definitions for ISUP Handler */
#endif

#define REDIRECTION_MASK     	0xF0
#define MAX_UPGRD_PARAM			25   // Maximum upgraded parametes - parameter copatibility info

//#include "SS7Common.h"

enum CktCmd
{
   CKT_CMD_BLOCK,
   CKT_CMD_UNBLOCK,
   CKT_CMD_BLOCK_ACK,
   CKT_CMD_UNBLOCK_ACK,
   CKT_CMD_GROUP_BLOCK,
   CKT_CMD_GROUP_BLOCK_ACK,
   CKT_CMD_GROUP_UNBLOCK,
   CKT_CMD_GROUP_UNBLOCK_ACK,
   CKT_CMD_STATUS,
   CKT_CMD_REL
};

enum CktCmdStatus
{
   CKT_CMD_BLOCKED,
   CKT_CMD_UN_BLOCKED,
   CKT_CMD_GRP_BLOCKED,
   CKT_CMD_GRP_UN_BLOCKED,
   CKT_CMD_RELEASED
};

struct CktCmdMsg
{
   UINT16   minCktId;
   UINT16   maxCktId;
   CktCmd   cktCmd;
};

struct CktCmdRspMsg
{
   UINT16         minCktId;
   UINT16         maxCktId;
   CktCmdStatus   status;
};

enum EnumParameterTag
{
#ifdef TELESOFT
   USER_TO_USER_INFORMATION                 = 0x01,
   ORIGINAL_CALLED_NUMBER                   = 0x02,
   REDIRECTING_NUMBER                       = 0x03,
   REDIRECTION_INFORMATION                  = 0x05,
   CALL_REFERENCE                           = 0x09,
   USER_SERVICE_INFORMATION                 = 0x0b,
   ACCESS_TRANSPORT_PARAMETER               = 0x0c,
   NATURE_OF_CONNECTION_INDICATOR           = 0x20,
   CALLED_PARTY_NUMBER                      = 0x21,
   CALLING_PARTY_NUMBER                     = 0x22,
   CALLING_PARTY_CATEGORY                   = 0x23,
   BUSINESS_GROUP                           = 0x24,
   CARRIER_IDENTIFICATION                   = 0x26,
   CARRIER_SELECTION                        = 0x27,
   CHARGE_NUMBER                            = 0x28,
   CONNECTION_REQUEST                       = 0x2A,
   EGRESS_SERVICE                           = 0x2B,
   GENERIC_ADDRESS                          = 0x2C,
   GENERIC_DIGITS                           = 0x2D,
   GENERIC_NAME                             = 0x2F,
   HOP_COUNTER                              = 0x30,
   INFORMATION_REQUEST_INDICATORS           = 0x32,
   JURISDICTION                             = 0x33,
   NETWORK_TRANSPORT                        = 0x34,
   OPERATOR_SERVICES_INFORMATION            = 0x35,
   ORIGINATING_LINE_INFORMATION             = 0x36,
   BACKWARD_CALL_INDICATORS                 = 0x37,
   PRECEDENCE                               = 0x38,
   REMOTE_OPERATIONS                        = 0x39,
   SERVICE_ACTIVATION                       = 0x3A,
   SERVICE_CODE_INDICATOR                   = 0x3B,
   SPECIAL_PROCESSING_REQUEST               = 0x3C,
   TRANSACTION_REQUEST                      = 0x3D,
   TRANSIT_NETWORK_SELECTION                = 0x3E,
   USER_SERVICE_INFORMATION_PRIME           = 0x3F,
   CAUSE_INDICATORS                         = 0x40,
   INFORMATION_INDICATORS                   = 0x41,
   NOTIFICATION_INDICATOR                   = 0x42,
   TRANSMISSION_MEDIUM_USED                 = 0x43,
   USER_TO_USER_INDICATORS                  = 0x44,
   EVENT_INFORMATION                        = 0x45,
   AUTOMATIC_CONGESTION_LEVEL               = 0x46,
   SUSPEND_INDICATOR                        = 0x48,
   RESUME_INDICATOR                         = 0x49,
   CONTINUITY_INDICATOR                     = 0x4A,
   FORWARD_CALL_INDICATORS                  = 0x50,
   OPTIONAL_BACKWARD_CALL_INDICATORS        = 0x5C,
   ACCESS_DELIVERY_INFORMATION              = 0x60,
   CALL_DIVERSION_INFORMATION               = 0x61,
   CALL_HISTORY                             = 0x62,
   CUG_INTERLOCK_CODE                       = 0x63,
   CONNECTED_NUMBER                         = 0x64,
   ECHO_CONTROL_INFORMATION                 = 0x66,
   GENERIC_NOTIFICATION_INDICATOR           = 0x67,
   GENERIC_NUMBER                           = 0x68,
   GENERIC_REFERENCE                        = 0x69,
   LOCATION_NUMBER                          = 0x6A,
   NETWORK_SPECIFIC_FACILITIES              = 0x6B,
   OPTIONAL_FORWARD_CALL_INDICATORS         = 0x6C,
   ORIGINATING_ISC_POINT_CODE               = 0x6D,
   PARAMETER_COMPATIBILITY_INFORMATION      = 0x6E,
   PROPAGATION_DELAY_COUNTER                = 0x70,
   REDIRECTION_NUMBER_RESTRICTION           = 0x71,
   SIGNALLING_POINT_CODE                    = 0x72,
   TRANSMISSION_MEDIUM_REQUIREMENT          = 0x73,
   TMR_PRIME                                = 0x74,
   UNKNOWN_IE                               = 0x75,
   REDIRECTION_NUMBER                       = 0x76,
   APPLICATION_TRANSPORT                    = 0x7b
#else

   //////////////////////////////////////////////////////

   USER_TO_USER_INFORMATION            = 0x20,
   ORIGINAL_CALLED_NUMBER              = 0x28,
   REDIRECTING_NUMBER                  = 0x0B,
   REDIRECTION_INFORMATION             = 0x13,
   CALL_REFERENCE                      = 0x01,
   USER_SERVICE_INFORMATION            = 0x1D,
   ACCESS_TRANSPORT_PARAMETER          = 0x03,
   NATURE_OF_CONNECTION_INDICATOR      = 0x06,
   CALLED_PARTY_NUMBER                 = 0x70,//anm,cpg,acm,rel
   CALLING_PARTY_NUMBER                = 0x0A,
   CALLING_PARTY_CATEGORY              = 0x09,
   //BUISNESS_GROUP                      = 0x24,//****
   //CARRIER_IDENTIFICATION              = 0x26,//****
   //CARRIER_SELECTION                   = 0x27,//****
   //CHARGE_NUMBER                       = 0x28,//****
   CONNECTION_REQUEST                  = 0x0D,
   //EGRESS_SERVICE                      = 0x2B,//****
   //GENERIC_ADDRESS                     = 0x2C,//****
   GENERIC_DIGITS                      = 0xC1,
   //GENERIC_NAME                        = 0x2F,//***
   HOP_COUNTER                         = 0x3D,
   INFORMATION_REQUEST_INDICATORS      = 0x0E,
   //JURISDICTION                        = 0x33,//****
   //NETWORK_TRANSPORT                   = 0x34,//***
   //OPERATOR_SERVICE_INFORMATION        = 0x35,//***
   //ORIGINATING_LINE_INFORMATION        = 0x36,//****
   BACKWARD_CALL_INDICATORS            = 0x11,
   PRECEDENCE                          = 0x3A,//MLPP precedence
   REMOTE_OPERATION                    = 0x32,
   SERVICE_ACTIVATION                  = 0x33,
   //SERVICE_CODE_INDICATOR              = 0x3B,//****
   //SPECIAL_PROCESSING_REQUEST          = 0x3C,//****
   //TRANSACTION_REQUEST                 = 0x3D,//****
   TRANSMIT_NETWORK_SELECTION          = 0x23,
   USER_SERVICE_INFORMATION_PRIME      = 0x30,
   CAUSE_INDICATOR                     = 0x12,
   INFORMATION_INDICATOR               = 0x0F,
   NOTIFICATION_INDICATOR              = 0x27,
   TRANSMISSION_MEDIUM_USED            = 0x35,
   USER_TOUSER_INDICATORS              = 0x2A,
   EVENT_INFORMATION                   = 0x24,
   AUTOMATIC_CONGESTION_LEVEL          = 0x27,
   SUSPEND_INDICATOR                   = 0x22,
   RESUME_INDICATOR                    = 0x22,
   CONTINUITY_INDICATOR                = 0x10,
   FORWARD_CALL_INDICATORS             = 0x07,
   OPTIONAL_BACKWARD_CALL_INDICATORS   = 0x29,
   ACCESS_DELIVERY_INFORMATION         = 0x2E,
   CALL_DIVERSION_INFORMATION          = 0x36,
   CALL_HISTORY                        = 0x2D,
   CUG_INTERLOCK_CODE                  = 0x1A,
   CONNECTED_NUMBER                    = 0x21,
   ECHO_CONTROL_INFORMATION            = 0x37,
   GENERIC_NOTIFICATION_INDICATOR      = 0x2C,
   GENERIC_NUMBER                      = 0xC0,
   //GENERIC_REFERENCE                   = 0x69,//
   LOCATION_NUMBER                     = 0x3F,
   NETWORK_SPECIFIC_FACILITIES         = 0x2F,
   OPTIONAL_FORWARD_CALL_INDICATORS    = 0x08,
   ORIGINATING_ISC_POINT_CODE          = 0x2B,
   PARAMETER_COMPABILITY_INFORMATION   = 0x39,
   PROPOGATION_DELEAY_COUNTER          = 0x31,
   REDIRECTION_NUMBER_REESTRICTION     = 0x40,
   SIGNALLING_POINT_CODE               = 0x1E,
   TRANSMISSION_MEDIUM_REQUIRENMENT    = 0x02,
   TMR_PRIME                           = 0x3E,
   UNKNOWN_IE                          = 0x75,//
   REDIRECTION_NUMBER                  = 0x0C,
   APPLICATION_TRANSPORT_PARAMETER     = 0x78,
	HIGH_LAYER_COMPATIBILITY				= 0X7D
#endif
};

enum GenIsupMsgType
{
   GEN_ISUP_UNKNOWN,
   GEN_ISUP_IAM,
   GEN_ISUP_ACM,
   GEN_ISUP_ANM,
   GEN_ISUP_CPG,
   GEN_ISUP_REL,
   GEN_ISUP_RLC,
   GEN_ISUP_RSC
};


enum GenEnumRedirectReason
{
   GEN_RI_UNKNOWN                           = 0x00,
   GEN_RI_USER_BUSY                         = 0x01,
   GEN_RI_NO_REPLY                          = 0x02,
   GEN_RI_UNCONDITIONAL                     = 0x03,
   GEN_RI_DEFLECTION_DURING_ALERTING        = 0x04,
   GEN_RI_DEFLECTION_IMMEDIATE_REPLY        = 0x05,
   GEN_RI_SUBSCRIBER_UNREACHABLE            = 0x06
};


enum GenEnumNumberFormat
{
   GEN_NATIONAL_FMT,
   GEN_INTERNATIONAL_FMT,
   GEN_SUBSCRIBER_NUM_FMT,
   GEN_UNKNOWN_FMT
};

/*enum EnumNumberingPlan
{
	PLAN_ISDN 		= 0x01,
	PLAN_DATA 		= 0x03,              // Data numbering plan
	PLAN_TELEX		= 0x04,             // Telex numbering plan
	PLAN_NATIONAL	= 0x05,          // National numbering plan
	PLAN_RESERVED = 0x06,
	PLAN_PRIVATE,           // Private numbering plan
	PLAN_ERMES             // ERMES numbering plan
};*/

enum GenEnumAddrPrsnt
{
	PRESN_ALLOWED,		
	PRESN_RESTRICTED,	
	PRESN_ADDR_NOT_AVAILABLE, 
	PRESN_RESERVED
};

enum GenEnumEventInfo
{
   EI_UNKNOWN,
   EI_ALERT,
   EI_PROGRESS,
   EI_INFORMATION_AVAILABLE,
   EI_CALL_FORWARDED_ON_BUSY,
   EI_CALL_FORWARDED_ON_NO_REPLY,
   EI_CALL_FORWARDED_UNCONDITIONALLY
};

enum GenEnumCpc
{
   CPC_NO_INDICATION,
   ISUP_CPC_ORDINARY_SUBSCRIBER,
   CPC_PAY_PHONE,
   CPC_SPARE
};

enum GenEnumCauseLocation
{
	USER, 			
	LPN,				//private network serving the local user (LPN)
	LN,				//public network serving the local user (LN)
	TN,				//transit network (TN)
	RLN,				//public network serving the remote user (RLN)
	RPN,				//private network serving the remote user (RPN)
	INTL = 7,		//international network (INTL)
	BI 	= 10		//network beyond interworking point (BI)

};

enum GenEnumTypeOfAns
{
   TOA_NO_INDICATION                   = 0x00,
   TOA_NO_CHARGE                       = 0x01,
   TOA_CHARGE                          = 0x02,
   TOA_SPARE                           = 0x03
};

enum GenEnumNotiInd
{
	 USER_SUSPENDED 				= 0x00,
	 USER_RESUMED  				= 0x01, 
	 BEARER_SERVICE_CHANGED 	= 0x02, 
	 //Discr. ext. ASN.1 enc comp 0x3 
	 CALL_COMPLETION_DELAY 		= 0x04, 
	 CONFERENCE_ESTABLISHED 	= 0x42, 
	 CONFERENCE_DISCONNECTED 	= 0x43, 
	 OTHER_PARTY_ADDED 			= 0x44, 
	 ISOLATED 						= 0x45, 
	 REATTACHED 					= 0x46, 
	 OTHER_PARTY_ISOLATED 		= 0x47, 
	 OTHER_PARTY_REATTACHED 	= 0x48,
	 OTHER_PARTY_SPLIT 			= 0x49,
	 OTHER_PARTY_DISCONNECTED 	= 0x4a, 
	 CONFERENCE_FLOATING 		= 0x4b,
	 WAITING_CALL 					= 0x60,
	 DIVERSION_ACTIVATED 		= 0x68,
	 CALL_TRANSFER_ALERT 		= 0x69,
	 CALL_TRANSFER_ACTIVE 		= 0x6a,
	 REMOTE_HOLD 					= 0x79,
	 REMOTE_RETRIEVAL 			= 0x7a,
	 CALL_DIVERTING 				= 0x7b 
};

enum GenEnumSccpMethod
{
	SCCP_NO_INDICATION,
	CONN_LESS_AVAILABLE,
	CONN_ORIENTED_AVAILABLE,
	CONN_LESS_AND_ORIENTED_AVAILABLE
};

enum GenEnumEndtoEndMethod
{
	NO_END_TO_END_AVAILABLE 	= 0x00,
	PASS_ALONG_AVAILABLE    	= 0x01,
	SCCP_AVAILABLE          	= 0x02,
	PASS_ALONG_SCCP_AVAILABLE  = 0x03
};

enum GenEnumNotiSubInd
{
   PRSN_UNKNOWN                 = 0X00,
   PRSN_NOT_ALLOWED             = 0X01,
   PRSN_ALLOW_WITH_REDIR_NUM    = 0X02,
   PRSN_ALLOW_WITHOUT_REDIR_NUM = 0X03,
   PRSN_SPARE
};

enum GenEnumCause
{
   UNALLOCATED_NUM               = 0x1, //Unallocated (unassignd) number 
   NO_ROUTE_TO_SPECD_TRANSIT_NET = 0x2, //No route to specd. transit net 
   O_ROUTE_TO_DST                = 0x3, //No route to destination 
   EDN_SPECIAL_INFO_TONE         = 0x4, //Send special information tone 
   ISDIALED_TRUNK_PREFIX         = 0x5, //Misdialled trunk prefix 
   RE_EMP                        = 0x8, //Preemption 
   RE_EMP_CKT_RESERVED           = 0x9, //Preemption - circuit reserved 
   ORMAL_CLR                     = 0x10, //Normal clearing 
   SER_BUSY                      = 0x11, //User busy 
   O_USER_RESP                   = 0x12, //No user responding 
   O_USER_ANS                    = 0x13, //No answer from user 
   UBSCRIBER_ABSENT              = 0x14, //Subsciber absent 
   ALL_REJECTED                  = 0x15, //Call rejected 
   UM_CHANGED                    = 0x16, //Number changed 
   DST_OUTOF_ORDER               = 0x1b, //Destination out of order 
   ADDR_COMPLETE                 = 0x1c, //Address incomplete 
   FACILITY_REJECTED             = 0x1d, //Facility rejected 
   NORMAL_UNSPECIFIED            = 0x1f, //Normal - unspecified 
   NO_CKT_AVAILABLE              = 0x22, //No circuit available 
   NETWORK_OUTOF_ORDER           = 0x26, //Network out of order 
   TEMP_FAILURE                  = 0x29, //Temporary failure 
   SWTCH_CONGESTION              = 0x2a, //Switching equipment congestion 
   ACCESS_INFO_DISCARDED         = 0x2b, //Access information discarded 
   REQ_CHANNEL_NOT_AVAILABLE     = 0x2c, //Requested channel not avail. 
   PRECEDENCE_CALL_BLOCKED       = 0x2e, //Precedence call blocked 
   RESOURCEA_UN_AVAILABLE        = 0x2f, //Resource unavailable - unspec. 
   REQ_FACILITY_NOT_SUBS         = 0x32, //Requested fac. not subscribed 
   OUTGOING_CALL_BARRED          = 0x35, //Outgoing calls barred within CUG 
   INCOMING_CALL_BARRED          = 0x37, //Incoming calls barred within CUG 
   BEARER_CAP_NOT_AUTHORIZED     = 0x39, //Bearer capabil. not authorized 
   BEARER_CAP_NOT_AVAILABLE      = 0x3a, //Bearer capabil. not available 
   INCONSIST_OUTGOING_ACCESS     = 0x3e, //Inconsistency in outgoing access 
   NO_SERVICE_OPTION             = 0x3f, //Service/option not available 
   BREARER_CAP_NOT_IMPL          = 0x41, //Bearer capabil not implemented 
   REQ_FACILITY_NOT_IMPL         = 0x45, //Requested fac. not implemented 
   RESTRICTED_CAP_AVAILABLE      = 0x46, //Only restricted capabil avail. 
   SERVICE_NOT_IMPL              = 0x4f, //Service/option not implemented 
   NOT_CUG_MEMBER                = 0x57, //User not member of CUG 
   NCOMPAT_DST                   = 0x58, //Incompatible destination 
   CUG_NOT_EXIST                 = 0x5a, //Non-existent CUG 
   NVALID_TRANS_NET_SEL          = 0x5b, //Invalid transit net selection 
   NVALID_MSG                    = 0x5f, //Invalid message, unspecified 
   SG_TYPE_NOT_EXIST             = 0x61, //Msg type non-existent/implem. 
   PARAM_NOT_EXIST                = 0x63, //Parm non-exist/implem.-discard 
   IMER_EXPIRY_RECOVER           = 0x66, //Recovery on timer expiry 
   ON_EXIST_PARAM_PASS           = 0x67, //Parm non-exist/implem.-passed 
   N_RECOGNISED_PARAM_MSG_DISCARD = 0x6e, //Msg with unrecogn. param. discarded
   ROTOCAOL_ERROR                = 0x6f, //Protocol error, unspecified 
   NTERWORKING                   = 0x7f //Interworking, unspecified 
};

enum GenEnumCalledPartyStatus
{
   NO_INDICATION 		= 0x00,
   SUBS_FREE     		= 0x01,
	CONNECT_WHEN_FREE = 0x02,
   STATUS_SPARE      = 0x03
};

enum GenEnumPassNotPossible
{
   RELEASE_CALL   = 0X00,
   DISCARD_MSG    = 0X01,
   DISCARD_PARAM  = 0X02
};

/*struct callAddr
{
	UINT8							natureOfAddr;
	BOOLEAN						odd;
	BOOLEAN						inn;
	TEXT							msisdn[SS7_MAX_ADDRESS_LEN + 1];
	EnumNumberingPlan   	numPlan;
	
};*/

struct GenParamCompInd
{
	INT16			            UpgradedParam;
	UINT8 		            transIntermEx;
	UINT8		            	releaseCallInd; 
	UINT8		            	sendNotiInd;
	UINT8		            	discardMsgInd;  
	UINT8		            	discardParamInd;     
	GenEnumPassNotPossible		noPassonPossible; 
	UINT8		            	ExtnInd;
	UINT8                   BNinterwork;
};

struct GenBackwardCallInd
{
	//UINT8    						chargeInd;
	GenEnumCalledPartyStatus	calledPartyStatusInd;
	GenEnumCpc 						calledPartyCategory;
	GenEnumEndtoEndMethod    	methodType;
	UINT8  						interworkInd;
	UINT8      					endtoendInfo;
	UINT8  						isupInd;
	UINT8      					holdingReq;
	UINT8  						isupAccessInd;
	UINT8      					echoCntrlDeviceInd;
	GenEnumSccpMethod				sccpMethodInd;
};

struct GenUserServiceInfo
{
	UINT8    	infoTransCap;   //0,8,9,10,11,18 (HEX)
	UINT8			codingStandard; //0,1,2,3
	INT16    	infoTransRate;
	UINT8    	transMode;
	UINT8			userlayerProtocol1;
	UINT8			layerIdentification;
	UINT8			rateMul;
};

struct GenAccessTransport
{
	//prgInd param;
	//callingPartySubAddr param;
	//calledPartySubAddr param;
	//lowLayerComp param;
	UINT8		hlcPrsntMethod; //hlc - High Layer Compatibility Parameter
	UINT8 	hlcInterpretation;
	UINT8   	hlcCodeStd;
	UINT8 	hlc; //High Layer Charecteristics
	UINT8		ehlcIdnt;
	UINT8    eVideoTeleCharIdnt;
};

struct GenIsupIam
{
	TEXT                   callingNumber[SS7_MAX_ADDRESS_LEN + 1]; 	 //Isup Handler
	TEXT                   calledNumber[SS7_MAX_ADDRESS_LEN + 1];      //Mandatory Variable Param/ Isup Hdlr
	TEXT                   origCalledNumber[SS7_MAX_ADDRESS_LEN + 1];  //C party number
	TEXT                   redirectingNumber[SS7_MAX_ADDRESS_LEN + 1]; //No. getting redirected - B num.
	GenEnumNumberFormat    callingNumberFormat;  // (req for wmca)
   GenEnumNumberFormat    calledNumberFormat;   // (req for wmca)
   GenEnumRedirectReason  redirectReason;       // Redirection Info. / isup hdlr
   BOOLEAN                prsntRestFlg;         //Calling No. presentation restricted/not
   BOOLEAN                ocldPrsntRestFlg;     //Orig Called No. presentation restricted/not
   BOOLEAN                redirectPrsntRestFlg; //Redirceting No. presentation restricted/not
   UINT8                  opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16                  optLen;
	GenUserServiceInfo     UserServiceInfo;
	GenAccessTransport     AccessTransport;
};

struct GenIsupAcm
{
   GenEnumCpc                 cpc; //frm isuphandler
   GenEnumEventInfo           eventInfo;
   UINT8                   cause;
   UINT8                   opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16                   optLen;
	GenEnumNotiSubInd		      notiSubOpt; //Call diversion Info - Notification Subscription Option
   GenEnumRedirectReason      callDivertReason;//Call diversion Info
	TEXT  				      redirectionNum[SS7_MAX_ADDRESS_LEN + 1];
//	TEXT  				      connectedNum[SS7_MAX_ADDRESS_LEN + 1];  // To be added for new release 
	INT16                   genNotiInd;
	GenParamCompInd	      ParamCompInd[MAX_UPGRD_PARAM];
};



struct GenIsupAnm
{
   GenEnumTypeOfAns        typeOfAns;  //From IsupHandler
	//INT16						genericNotiInd;
	//LONG						connectedNumber;
   UINT8                opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16                optLen;
	GenBackwardCallInd   backwardCallInd;
	INT16						callHisPDValue; //Call History Propogation Delay Value
	GenParamCompInd		ParamCompInd[MAX_UPGRD_PARAM];
};


struct GenIsupCpg
{
   GenEnumEventInfo     eventInfo;
   UINT8             opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16             optLen;
};


struct GenIsupRel
{
   UINT8                   cause;
   UINT8                   location;
	GenEnumRedirectReason   redirectReason;
   UINT8                   opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16                   optLen;
	INT16							sigPointCode; //Signalling Point Code
};


struct GenIsupRlc
{
   UINT8    cause;
   UINT8    location;
   UINT8    codeStandard;
   UINT8    opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16    optLen;
};


struct GenIsupReset
{
   UINT8   opt[TDAPI_MAX_BYTE_OPT_LEN];
   INT16   optLen;
};


struct GenIsupMsg
{
   GenIsupMsgType    isupMsgType; //Type of Msg = IAM,ACM,ANM,REL,RLC
   UINT16            circuitId;  // Allocated Ckt ID - Mandatory Fixed parameter
   int			      pointCode;  
   union
   {
      GenIsupIam       isupIam;
      GenIsupCpg       isupCpg;
      GenIsupAcm       isupAcm;
      GenIsupAnm       isupAnm;
      GenIsupRel       isupRel;
      GenIsupRlc       isupRlc;
      GenIsupReset     isupReset;
   };
};

struct GenCauseInd
{
	UINT8   		location;
	UINT8	  		codeStandard;
	BOOLEAN 		extInd1;
	UINT16		causevalue;
	BOOLEAN     extInd2;
};

#endif
