// @(#) $Id: Ss7ConstDef.h,v 1.1.4.9 2019/04/16 20:58:03 jamesjac Exp $
//----------------------------------------------------------------------
// NAME 
// Ss7ConstDef.h
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
#ifndef INCL_SS7_CONST_DEF_H
#define INCL_SS7_CONST_DEF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <libgen.h>
#include <math.h>
#include <iostream>

//#include "tdapi.h"

#include "Types.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "StrTokenizer.h"
#include "LogApi.h"
#include "PegApi.h"
//#include "OamLogCodes.h"
#include "CfgRead.h"
#include "SS7LogCodes.h"

//#include "Ss7Limits.h"

#define ANSI_PRODUCT_VER "3.0_RC2"

#define BIT_01    1
#define BIT_02    2
#define BIT_03    4
#define BIT_04    8
#define BIT_05    16
#define BIT_06    32
#define BIT_07    64
#define BIT_08    128
#define BIT_09    256
#define BIT_10    512
#define BIT_11    1024
#define BIT_12    2048
#define BIT_13    4096
#define BIT_14    8192
#define BIT_15    16384
#define BIT_16    32768
#define BIT_17    65536
#define BIT_18    131072
#define BIT_19    262144
#define BIT_20    524288
#define BIT_21    1048576
#define BIT_22    2097152
#define BIT_23    4194304
#define BIT_24    8388608
#define BIT_25    16777216
#define BIT_26    33554432
#define BIT_27    67108864
#define BIT_28    134217728

#define TRACE_SS7_ENV                 "TRACE_SS7"
#define TRACE_MAP_HANDLER_ENV         "TRACE_MAP_HANDLER"
#define TRACE_MAP_ENCODER_ENV         "TRACE_MAP_ENCODER"
#define TRACE_MAP_DECODER_ENV         "TRACE_MAP_DECODER"
#define TRACE_TCAP_HANDLER_ENV        "TRACE_TCAP_HANDLER"
#define TRACE_TCAP_DECODER_ENV        "TRACE_TCAP_DECODER"
#define TRACE_TCAP_ENCCODER_ENV       "TRACE_TCAP_ENCODER"
#define TRACE_SS7_STATE_SYNC_ENV      "TRACE_SS7_STATE_SYNC"
#define TRACE_ISUP_HANDLER_ENV        "TRACE_ISUP_HANDLER"
#define TRACE_MTP_HANDLER_ENV         "TRACE_MTP_HANDLER"
#define TRACE_SS7_CARD_DEBUG          "TRACE_SS7_CARD_DEBUG"
#define TRACE_MEDIA_HANDLER_ENV       "TRACE_MEDIA_HANDLER"
#define TRACE_DIA_TCAP_DLG_CLEAN_ENV  "TRACE_DIA_TCAP_DLG_CLEAN_ENV"

#define TRACE_MSU_DECODER  "TRACE_MSU_DECODER"
#define TRACE_MSU_ENCODER  "TRACE_MSU_ENCODER"
#define TRACE_MSU_ENCDEC   "TRACE_MSU_ENCDEC"

#define TRACE_TCAP_MSG_TRANSLATOR_ENV "TRACE_TCAP_MSG_TRANSLATOR"
#define TRACE_SCCP_TRANSLATION_ENV    "TRACE_SCCP_TRANSLATION"
// Dk Encoder and Decoder
#define TRACE_DK_MAP_ENCODER_ENV    "TRACE_DK_MAP_ENCODER"
#define TRACE_DK_MAP_DECODER_ENV    "TRACE_DK_MAP_DECODER"
#define TRACE_LINK_STATUS           "TRACE_LINK_STATUS"
#define MAX_LOG_TEXT_LEN            TSS_MAX_LOG_TEXT_LEN

#define MAP_NAME                   "MAP"
#define CAP_NAME                   "CAP" // added - Cap
#define TCAP_NAME                  "TCAP"
#define ISUP_NAME                  "ISUP"
#define MTP_NAME                   "MTP"
#define MEDIA_NAME                 "MEDIA"
#define SCCP_NAME                  "SCCP"

#define SIGTRC                      12
#define SIGCFG                      10
#define SS7_IPC_PERM                0666

#define SS7_PROD_CFG                "Product.cfg"
#define SS7_IPC_CFG                 "ipc.cfg"
#define SS7_KER_CFG                 "kernel.cfg"
#define SS7_CFG                     "ss7.cfg"
#define SS7_MAP_ROUTING_CFG         "mapRouting.cfg"
#define TCAP_ANSI_CFG		    "TcapAnsiHandler.cfg"
#define SCCP_ANSI_CFG		    "SccpAnsiHandler.cfg"

#define SSM_CFG                     "SyncManager.cfg"

#define ASCII_TO_GSM_CFG            "AsciiToGsm.cfg"

#define CCURE_CFG                   "ccure.cfg"

#define SS7_MAX_PROCESS_NAME         50 

#define SS7_MAX_ADDRESS_LEN          20
#define SS7_LMSI_LEN                 8
#define SS7_MAX_USSD_STRING_LEN      160
#define SS7_MAX_8BIT_USSD_STRING_LEN 182

#define SS7_MAX_NUM_OF_OPERATORS            1000
#define SS7_MAX_OPERATOR_NAME_LEN           50
#define SS7_MAX_COUNTRY_NAME_LEN            50

#define SS7_MIN_COUNTRY_CODE_LEN            2
#define SS7_MAX_COUNTRY_CODE_LEN            5

#define SS7_MAX_NDD_LEN                     3
#define SS7_MAX_IDD_LEN                     3

#define SS7_MAX_MCC_MNC_LEN                 9
#define SS7_MAX_CC_NDC_LEN                  9

#define SS7_MAX_NUM_OF_MCC_MNC_DETAILS             2000
#define SS7_MAX_NUM_OF_CC_NDC_DETAILS              2000

#define SS7_MAX_NUM_OF_MGT_TRANSLATIONS            1000
#define SS7_MAX_NUM_OF_GT_PC_TRANSLATIONS          255
#define SS7_MAX_NUM_OF_OPCODE_GT_PC_TRANSLATIONS   25


#define SS7_MAX_NUM_OF_OPCODE_DETAILS      25
#define SS7_MAX_OPCODE_NAME_LEN     50
#define SS7_MAX_SHORT_CODE_LEN      6
//Added by paresh
#define SS7_MAX_GENERIC_NUM_LEN    11
#define SS7_MAX_CARRIER_ID_LEN     3
#define SS7_MAX_CHARGE_NUM_LEN     7

#define ASCII_OFFSET                0x30
#define BYTE_RESET_VAL              0x00
#define ONE_NIBBLE_SHIFT            0x04
#define ONE_BYTE_SHIFT              0x08
#define LOWER_NIBBLE_MASK           0x0F
#define UPPER_NIBBLE_MASK           0xF0
#define NIBBLE_FILLER               0xF0

#define USSD_SEVEN_BIT_ENC_DCS_VALUES "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15"
#define SS7_MIN_DCS_VALUE           0
#define SS7_MAX_DCS_VALUE           255

#define SS7_MAX_ASCII_GSM_ARRAY_SIZE 256
#define GSM_ESC_CHARACTER            0x1B
#define EXTND_OFFSET                 128

#define SS7_TCAP_QUERY_TIMEOUT       45

/////////////////////////////////////////////////////
#define SS7_MIN_IPC_Q_KEY   TSS_MIN_IPC //1000
#define SS7_MAX_IPC_Q_KEY   TSS_MAX_IPC //9999

#define SS7_MIN_NO_OF_PC 0
#define SS7_MAX_NO_OF_PC 128

#define SS7_MIN_POINTCODE   1
#define SS7_MAX_POINTCODE   16383

#define SS7_MIN_BOOLEAN  0
#define SS7_MAX_BOOLEAN  1

#define SS7_MIN_CKT_UNBLK_INTERVAL  0
#define SS7_MAX_CKT_UNBLK_INTERVAL  9999


#define SS7_MIN_SHM_KEY          1000
#define SS7_MAX_SHM_KEY          9999

#define SS7_MAX_CARDS  2

//#define SS7_MAX_COUNTRY_CODE_LEN  5


#define SS7_MIN_NI  0
#define SS7_MAX_NI  3
#define SS7_MIN_RI  0
#define SS7_MAX_RI  255

////////////////////////////////////////////////////
#define SS7_MAX_CELL_ID_FIXED_LEN       14
#define SS7_MAX_LAI_FIXED_LEN           5
#define SS7_MAX_GEOGRAPHICAL_INFO_LEN   8
#define SS7_MAX_LOCATION_NUMBER_LEN     10
#define SS7_MAX_NUMBER_OF_BCSM_EVENTS   10
#define SS7_MAX_LSA_IDENTITY_LEN        3
#define SS7_MAX_GEODETIC_INFO_LEN       10

#define SS7_MAX_BUFFER_LEN              500
#define SS7_MAX_CONFIG_BUF_LEN          100

#define SS7_MIN_REL_LOCATION_VAL     0x00
#define SS7_MAX_REL_LOCATION_VAL     0x0a

#define SS7_MIN_REL_CAUSE_VAL    0x00
#define SS7_MAX_REL_CAUSE_VAL    0x7F

#define SS7_MIN_REL_CODING_STD_VAL    0
#define SS7_MAX_REL_CODING_STD_VAL    3

#define SS7_EXT_BASIC_SERVICE_CODE_LEN  5

#define SS7_MAX_TCAP_QUERY_TIMEOUT      255

#define SS7_MAX_DIAMETER_NAME_LEN        55

#define MAX_ACU_REASON_LEN          20

#define SS7_MIN_IP_LEN              7
#define SS7_MAX_IP_LEN              20

#define SS7_PARAM_LEN               12

enum EnumMapAcn
{
   UPDATE_LOC_ACN    = 1,
   CANCEL_LOC_ACN    = 2,
   PRN_ACN           = 3,
   SRI_ACN           = 5,
   NET_FUNC_SS_ACN   = 18,
   NET_USS_ACN       = 19,
   SRI_SM_ACN        = 20,
   MO_FSM_ACN        = 21,
   ALERT_SC_ACN      = 23,
   READY_FOR_SM_ACN  = 24,
   MT_FSM_ACN        = 25,
   PURGE_ACN         = 27,
   PSI_ACN           = 28,
   ATI_ACN           = 29,
   SM_WAIT_DATA_ACN  = 20
};

extern Log     gLog;
extern CTrace  gTrace;
extern TEXT    gProcessName[];
#endif
