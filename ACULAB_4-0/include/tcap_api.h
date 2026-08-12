/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/tcap/api/tcap_api.h,v 1.183 2023/02/22 10:39:00 davidla Exp $ $Name: SS7_6_16_1 $" */

#ifndef TCAP_API_H
#define TCAP_API_H

/* TCAP API - external interfaces */

#include <stdarg.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif


/*
 * Define an appropiate prefix for library function definitions,
 * in order to exclude internal functions from the library symbol table
 * and to ensure that the library calls its own public functions.
 *
 * The library itself is compiled with -fvisibility=hidden and
 * -DACU_TCAP_LIB_API=__attribute__((visibility("protected")))
 */

#ifndef ACU_TCAP_LIB_API
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) >= 30400
#define ACU_TCAP_LIB_API __attribute__((visibility("default")))
#else
#define ACU_TCAP_LIB_API
#endif
#endif

#include <tcap_asn1_codec.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Errors: negative integers - Most negative first - Add new codes at the top */

#define ACU_TCAP_ERRORS(x) \
x( ACU_TCAP_ERROR_BAD_THREAD_POOL,                       \
        "Invalid thread pool address")                   \
x( ACU_TCAP_ERROR_THREAD_POOL_NO_FREE_THREADS,           \
        "No free threads available")                     \
\
/* Message decode errors */ \
x( ACU_TCAP_ERROR_UNKNOWN_MESSAGE_TYPE,                  \
        "Invalid TCAP message type")                     \
x( ACU_TCAP_ERROR_BAD_REMOTE_TID,                        \
        "Remote transaction-id incorrect")               \
x( ACU_TCAP_ERROR_BAD_LOCAL_TID,                         \
        "Local transaction-id incorrect")                \
x( ACU_TCAP_ERROR_BAD_TCAP_MSG,                          \
        "Incorrectly formatted TCAP message")            \
x( ACU_TCAP_ERROR_BAD_PROTOCOL_VERSION,                  \
        "Dialogue pdu not version1")                     \
x( ACU_TCAP_ERROR_BAD_ASN1,                              \
        "Badly constructed ASN.1")                       \
x( ACU_TCAP_ERROR_INVALID_DIALOGUE_USER_SYNTAX,          \
        "Invalid user-defined dialogue abstract syntax") \
\
/* Message encoder errors */ \
x( ACU_TCAP_ERROR_BUILD_SEQUENCE,                        \
        "Message being built in the wrong order")        \
x( ACU_TCAP_ERROR_INVALID_MSG_TYPE,                      \
        "Specified message type is invalid")             \
x( ACU_TCAP_ERROR_NO_REMOTE_TRANS_ID,                    \
        "No remote transaction id")                      \
x( ACU_TCAP_ERROR_DIALOGUE_NOT_ALLOWED,                  \
        "Dialogue portion not allowed in this message")  \
x( ACU_TCAP_ERROR_INVALID_DIALOGUE_USERINFO,             \
        "Dialogue userinfo must be ASN.1 EXTERNAL")      \
x( ACU_TCAP_ERROR_INVALID_PROBLEM_TYPE,                  \
        "Problem type for Reject invalid")               \
x( ACU_TCAP_ERROR_ASN1_ENCODING,                         \
        "Error from ASN.1 encoding functions")           \
\
/* Encode/decoder general errors */ \
x( ACU_TCAP_ERROR_INVOKE_ID_IDLE,                        \
        "Operation on idle invoke-id")                   \
x( ACU_TCAP_ERROR_INVOKE_ID_OUTSTATE,                    \
        "Operation on this invoke-id in wrong state")    \
x( ACU_TCAP_ERROR_BAD_TRANSACTION_STATE,                 \
        "Transaction state incorrect for this request")  \
\
/* User errors */ \
x( ACU_TCAP_ERROR_CANNOT_OPEN_CONFIG_FILE,               \
        "Configuration file cannot be found")            \
x( ACU_TCAP_ERROR_INVALID_CONFIG_VALUE,                  \
        "Configuration parameter value invalid")         \
x( ACU_TCAP_ERROR_UNKNOWN_CONFIG_PARAM,                  \
        "Configuration parameter not known")             \
x( ACU_TCAP_ERROR_NO_LOCAL_POINTCODE,                    \
        "Local pointcode not configured")                \
x( ACU_TCAP_ERROR_NO_LOCAL_SSN,                          \
        "Local SSN not configured")                      \
x( ACU_TCAP_ERROR_ALREADY_CONNECTED,                     \
        "Already connected to SCCP")                     \
x( ACU_TCAP_ERROR_WRONG_MSG_TYPE,                        \
        "Message type incorrect for this function")      \
x( ACU_TCAP_ERROR_BAD_OS_EVENT,                          \
        "Invalid OS event structure")                    \
x( ACU_TCAP_ERROR_BAD_EVENT,                             \
        "Invalid event address")                         \
x( ACU_TCAP_ERROR_BAD_MESSAGE,                           \
        "Invalid message address")                       \
x( ACU_TCAP_ERROR_BAD_SSAP,                              \
        "Invalid sccp sap address")                      \
x( ACU_TCAP_ERROR_BAD_TRANSACTION,                       \
        "Invalid tcap transaction address")              \
x( ACU_TCAP_ERROR_NO_THREADS,                            \
        "Thread creation failed")                        \
x( ACU_TCAP_ERROR_MALLOC_FAIL,                           \
        "malloc() failed")                               \
x( ACU_TCAP_ERROR_not_used_1,                            \
        "error code not used")                           \
\
/* Non-error errors */ \
x( ACU_TCAP_ERROR_NO_INFORMATION_AVAILABLE,              \
        "Requested information not avaialble")           \
x( ACU_TCAP_ERROR_NO_MESSAGE,                            \
        "No message on queue")                           \
x( ACU_TCAP_ERROR_TIMEDOUT,                              \
        "Timeout expired")                               \
x( ACU_TCAP_ERROR_NO_COMPONENT,                          \
        "No (more) components in message")               \
\
x( ACU_TCAP_ERROR_SUCCESS,                               \
        "Success") /* Guaranteed to be zero */

#define ACU_TCAP_ERR_ENUM(name, text) name,
#define ACU_TCAP_ERR_COUNT(name, text) -1
typedef enum {
    ACU_TCAP_ERROR_HIGHEST = ACU_TCAP_ERRORS(ACU_TCAP_ERR_COUNT),
    ACU_TCAP_ERRORS(ACU_TCAP_ERR_ENUM)
    ACU_TCAP_ERROR_NUM_ERRMSG = -ACU_TCAP_ERROR_HIGHEST
} acu_tcap_errors_t;
#undef ACU_TCAP_ERR_ENUM
#undef ACU_TCAP_ERR_COUNT

/* Overall types of TCAP messages */
/* Internal messages (an invalid encoding for ASN.1 message types) */
#define ACU_TCAP_MSG_INTERNAL(msg_type) ((msg_type) << 8)
typedef enum {
  /* Overall message types - set by acu_tcap_xxx_msg_get() */
    ACU_TCAP_MSG_DATA           = ACU_TCAP_MSG_INTERNAL(1),
    ACU_TCAP_MSG_NOTICE         = ACU_TCAP_MSG_INTERNAL(2),
    ACU_TCAP_MSG_TIMEOUT        = ACU_TCAP_MSG_INTERNAL(4),
    ACU_TCAP_MSG_CON_STATE      = ACU_TCAP_MSG_INTERNAL(5),
    ACU_TCAP_MSG_USER_STATUS    = ACU_TCAP_MSG_INTERNAL(6),
    ACU_TCAP_MSG_SP_STATUS      = ACU_TCAP_MSG_INTERNAL(7),
  /* Message types generated by acu_tcap_msg_decode() */
    ACU_TCAP_MSG_P_ABORT        = ACU_TCAP_MSG_INTERNAL(16),
    ACU_TCAP_MSG_LOCAL_ABORT    = ACU_TCAP_MSG_INTERNAL(17),
  /* ITU TCAP - for acu_tcap_msg_init() and set by acu_tcap_msg_decode() */
    ACU_TCAP_MSG_ITU_UNI        = ACU_ASN1_APP_CONS(1),
    ACU_TCAP_MSG_ITU_BEGIN      = ACU_ASN1_APP_CONS(2),
    ACU_TCAP_MSG_ITU_END        = ACU_ASN1_APP_CONS(4),
    ACU_TCAP_MSG_ITU_CONTINUE   = ACU_ASN1_APP_CONS(5),
    ACU_TCAP_MSG_ITU_ABORT      = ACU_ASN1_APP_CONS(7),
  /* ANSI TCAP - for acu_tcap_msg_init() and set by acu_tcap_msg_decode() */
    ACU_TCAP_MSG_ANSI_UNI       = ACU_ASN1_PRV_CONS(1),
    ACU_TCAP_MSG_ANSI_QUERY     = ACU_ASN1_PRV_CONS(2),
    ACU_TCAP_MSG_ANSI_QUERY_WO  = ACU_ASN1_PRV_CONS(3),
    ACU_TCAP_MSG_ANSI_RESPONSE  = ACU_ASN1_PRV_CONS(4),
    ACU_TCAP_MSG_ANSI_CONV      = ACU_ASN1_PRV_CONS(5),
    ACU_TCAP_MSG_ANSI_CONV_WO   = ACU_ASN1_PRV_CONS(6),
    ACU_TCAP_MSG_ANSI_ABORT     = ACU_ASN1_PRV_CONS(0x16)
} acu_tcap_msg_type_t;

/* Component types - set by acu_tcap_msg_get_component() */
typedef enum {
  /* Internal pseduo-component types */
    ACU_TCAP_COMP_LOCAL_REJECT        = ACU_TCAP_MSG_INTERNAL(1),
    ACU_TCAP_COMP_OP_TIMEOUT          = ACU_TCAP_MSG_INTERNAL(2),
    ACU_TCAP_COMP_ANSI_ABORT          = ACU_TCAP_MSG_INTERNAL(3),
  /* ITU TCAP components */
    ACU_TCAP_COMP_ITU_INVOKE          = ACU_ASN1_CTX_CONS(1),
    ACU_TCAP_COMP_ITU_RESULT_LAST     = ACU_ASN1_CTX_CONS(2),
    ACU_TCAP_COMP_ITU_ERROR           = ACU_ASN1_CTX_CONS(3),
    ACU_TCAP_COMP_ITU_REJECT          = ACU_ASN1_CTX_CONS(4),
    ACU_TCAP_COMP_ITU_RESULT_NOTLAST  = ACU_ASN1_CTX_CONS(7),
  /* ANSI TCAP components */
    ACU_TCAP_COMP_ANSI_INVOKE_LAST    = ACU_ASN1_PRV_CONS(9),
    ACU_TCAP_COMP_ANSI_RESULT_LAST    = ACU_ASN1_PRV_CONS(10),
    ACU_TCAP_COMP_ANSI_ERROR          = ACU_ASN1_PRV_CONS(11),
    ACU_TCAP_COMP_ANSI_REJECT         = ACU_ASN1_PRV_CONS(12),
    ACU_TCAP_COMP_ANSI_INVOKE_NOTLAST = ACU_ASN1_PRV_CONS(13),
    ACU_TCAP_COMP_ANSI_RESULT_NOTLAST = ACU_ASN1_PRV_CONS(14)
} acu_tcap_comp_type_t;

/* Dialogue portion types (for inward messages) */
typedef enum {
  /* ITU TCAP dialogue portions */
    ACU_TCAP_DLG_ITU_USER_SYNTAX      = ACU_TCAP_MSG_INTERNAL(1),
    ACU_TCAP_DLG_ITU_AARQ_AUDT        = ACU_ASN1_APP_CONS(0),
    ACU_TCAP_DLG_ITU_AARE             = ACU_ASN1_APP_CONS(1),
    ACU_TCAP_DLG_ITU_ABRT             = ACU_ASN1_APP_CONS(4),
  /* ANSI TCAP dialogue */
    ACU_TCAP_DLG_ANSI                 = ACU_ASN1_PRV_CONS(25)
} acu_tcap_dlg_type_t;

#undef ACU_TCAP_MSG_INTERNAL

/* P-Abort cause values */
typedef enum {
  /* ITU values from Q.773 */
    ACU_TCAP_P_ABORT_ITU_UNRECOGNIZED_MESSAGE_TYPE = 0,
    ACU_TCAP_P_ABORT_ITU_UNRECOGNIZED_TRANSACTION_ID,
    ACU_TCAP_P_ABORT_ITU_BADLY_FORMATTED_TRANSACTION_PORTION,
    ACU_TCAP_P_ABORT_ITU_INCORRECT_TRANSACTION_PORTION,
    ACU_TCAP_P_ABORT_ITU_RESOURCE_LIMITATION,
    /* For Q.774 3.2.2.1 (no value assigned) */
    ACU_TCAP_P_ABORT_ITU_ABNORMAL_DIALOGUE = 0x7e,
  /* ANSI values from T1.114.3 */
    ACU_TCAP_P_ABORT_ANSI_UNRECOGNIZED_PACKAGE_TYPE = 1,
    ACU_TCAP_P_ABORT_ANSI_INCORRECT_TRANSACTION_PORTION,
    ACU_TCAP_P_ABORT_ANSI_BADLY_STRUCTURED_TRANSACTION_PORTION,
    ACU_TCAP_P_ABORT_ANSI_UNASSIGNED_RESPONDING_TRANSACTION_ID,
    ACU_TCAP_P_ABORT_ANSI_PERMISSION_TO_RELEASE_PROBLEM,
    ACU_TCAP_P_ABORT_ANSI_RESOURCE_UNAVAILABLE,
    ACU_TCAP_P_ABORT_ANSI_UNRECOGNIZED_DIALOGUE_PORTION_ID,
    ACU_TCAP_P_ABORT_ANSI_BADLY_STRUCTURED_DIALOGUE_PORTION,
    ACU_TCAP_P_ABORT_ANSI_MISSING_DIALOGUE_PORTION,
    ACU_TCAP_P_ABORT_ANSI_INCONSISTENT_DIALOGUE_PORTION
} acu_tcap_p_abort_cause_t;

/* Reject problem values */
typedef enum {
  /* ITU values from Q.773 */
    ACU_TCAP_REJECT_ITU_GENERAL_UNRECOGNIZED_COMPONENT       = 0 << 8 | 0,
    ACU_TCAP_REJECT_ITU_GENERAL_MISTYPED_COMPONENT,
    ACU_TCAP_REJECT_ITU_GENERAL_BADLY_STRUCTURED_COMPONENT,
    ACU_TCAP_REJECT_ITU_INVOKE_DUPLICATE_INVOKE_ID           = 1 << 8 | 0,
    ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_OPERATION,
    ACU_TCAP_REJECT_ITU_INVOKE_MISTYPED_PARAMETER,
    ACU_TCAP_REJECT_ITU_INVOKE_RESOURCE_LIMITATION,
    ACU_TCAP_REJECT_ITU_INVOKE_INITIATING_RELEASE,
    ACU_TCAP_REJECT_ITU_INVOKE_UNRECOGNIZED_LINKED_ID,
    ACU_TCAP_REJECT_ITU_INVOKE_LINKED_RESPONSE_UNEXPECTED,
    ACU_TCAP_REJECT_ITU_INVOKE_UNEXPECTED_LINKED_OPERATION,
    ACU_TCAP_REJECT_ITU_RESULT_UNRECOGNIZED_INVOKE_ID        = 2 << 8 | 0,
    ACU_TCAP_REJECT_ITU_RESULT_RETURN_RESULT_UNEXPECTED,
    ACU_TCAP_REJECT_ITU_RESULT_MISTYPED_PARAMETER,
    ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_INVOKE_ID         = 3 << 8 | 0,
    ACU_TCAP_REJECT_ITU_ERROR_RETURN_ERROR_UNEXPECTED,
    ACU_TCAP_REJECT_ITU_ERROR_UNRECOGNIZED_ERROR,
    ACU_TCAP_REJECT_ITU_ERROR_UNEXPECTED_ERROR,
    ACU_TCAP_REJECT_ITU_ERROR_MISTYPED_PARAMETER,
  /* ANSI values from T1.114.3 */
    ACU_TCAP_REJECT_ANSI_GENERAL_UNRECOGNIZED_COMPONENT_TYPE = 1 << 8 | 1,
    ACU_TCAP_REJECT_ANSI_GENERAL_INCORRECT_COMPONENT_PORTION,
    ACU_TCAP_REJECT_ANSI_GENERAL_BADLY_STRUCTURED_COMPONENT_PORTION,
    ACU_TCAP_REJECT_ANSI_GENERAL_INCORRECT_COMPONENT_CODING,
    ACU_TCAP_REJECT_ANSI_INVOKE_DUPLICATE_INVOKE_ID          = 2 << 8 | 1,
    ACU_TCAP_REJECT_ANSI_INVOKE_UNRECOGNIZED_OPERATION_CODE,
    ACU_TCAP_REJECT_ANSI_INVOKE_INCORRECT_PARAMETER,
    ACU_TCAP_REJECT_ANSI_INVOKE_UNRECOGNIZED_CORRELATION_ID,
    ACU_TCAP_REJECT_ANSI_RESULT_UNASSIGNED_CORRELATION_ID    = 3 << 8 | 1,
    ACU_TCAP_REJECT_ANSI_RESULT_UNEXPECTED_RETURN_RESULT,
    ACU_TCAP_REJECT_ANSI_RESULT_INCORRECT_PARAMETER,
    ACU_TCAP_REJECT_ANSI_ERROR_UNASSIGNED_CORRELATION_ID     = 4 << 8 | 1,
    ACU_TCAP_REJECT_ANSI_ERROR_UNEXPECTED_RETURN_ERROR,
    ACU_TCAP_REJECT_ANSI_ERROR_UNRECOGNIZED_ERROR,
    ACU_TCAP_REJECT_ANSI_ERROR_UNEXPECTED_ERROR,
    ACU_TCAP_REJECT_ANSI_ERROR_INCORRECT_PARAMETER,
    ACU_TCAP_REJECT_ANSI_TRANS_UNRECOGNIZED_PACKAGE_TYPE     = 5 << 8 | 1,
    ACU_TCAP_REJECT_ANSI_TRANS_INCORRECT_TRANSACTION_PORTION,
    ACU_TCAP_REJECT_ANSI_TRANS_BADLY_STRUCTURED_TRANSACTION_PORTION,
    ACU_TCAP_REJECT_ANSI_TRANS_UNASSIGNED_RESPONDING_TRANSACTION_ID,
    ACU_TCAP_REJECT_ANSI_TRANS_PERMISSION_TO_RELEASE,
    ACU_TCAP_REJECT_ANSI_TRANS_RESOURCE_UNAVAILABLE
} acu_tcap_reject_problem_t;

/* Status code for ITU AARE dialogue pdu - first backwards message.
 * ETSI constrains the individual values to the range 0..127,
 * so it is hopefully safe to pack them into a 32bit value. */
#define ACU_TCAP_AARE_RESULT(rslt, src, diag) \
        ((rslt) << 16 | (src) << 8 | (diag))
typedef enum {
    ACU_TCAP_AARE_ACCEPTED_USER             = ACU_TCAP_AARE_RESULT(0,1,0),
    ACU_TCAP_AARE_ACCEPTED_PROVIDER         = ACU_TCAP_AARE_RESULT(0,2,0),
    ACU_TCAP_AARE_REJECT_USER_NULL          = ACU_TCAP_AARE_RESULT(1,1,0),
    ACU_TCAP_AARE_REJECT_USER_NO_REASON     = ACU_TCAP_AARE_RESULT(1,1,1),
    ACU_TCAP_AARE_REJECT_USER_APPLICATION_CONTEXT_NOT_SUPPORTED
                                            = ACU_TCAP_AARE_RESULT(1,1,2),
    ACU_TCAP_AARE_REJECT_PROVIDER_NULL      = ACU_TCAP_AARE_RESULT(1,2,0),
    ACU_TCAP_AARE_REJECT_PROVIDER_NO_REASON = ACU_TCAP_AARE_RESULT(1,2,1),
    ACU_TCAP_AARE_REJECT_PROVIDER_NO_COMMON_DIALOGUE_PORTION
                                            = ACU_TCAP_AARE_RESULT(1,2,2)
} acu_tcap_aare_result_t;

/* A few miscellaneous constants */
#define ACU_TCAP_LAST         0x80    /* For result_last and invoke_last */
#define ACU_TCAP_NO_INVOKE_ID 256     /* To suppress addition of invoke id */


/* Definitions for parts of an SCCP address buffer */
#define ACU_SCCP_ADDR_FIELD(x) \
x( GTI,    gti,    0,      255,  char)  /* For RAW_GT only */ \
x( SSN,    ssn,    1,      255,  char) \
x( PC,     pc,     2, 0xffffff,  int) \
x( RL_PC,  rl_pc,  3, 0xffffff,  int)   /* From (receive) routing label */ \
x( TT,     tt,     4,      255,  char)  /* Translation type */ \
x( NP,     np,     5,       15,  char)  /* Numbering plan */ \
x( ES,     es,     6,       15,  char)  /* Encoding scheme */ \
x( NAI,    nai,    7,      127,  char)  /* Nature of address indicator */

/* Bit-pattern constants ACU_SCCP_SA_VALID_xxx for sa_valid */
#define ACU_SCCP_SA_VALID(NAME, name, val, max, type) \
    ACU_SCCP_SA_VALID_##NAME = 1 << (val),
enum { ACU_SCCP_ADDR_FIELD(ACU_SCCP_SA_VALID) ACU_SCCP_SA_VALID_0 = 0 };
#undef ACU_SCCP_SA_VALID

/* SCCP address structure
 * notes:
 * - The fields are invalid unless the corresponding sa_valid_xxx bit is set.
 * - The sa_gti field is ignored unless ACU_SCCP_SA_FLAGS_RAW_GT is set,
 *   it's value on inbound messages depends on the national variant in use.
 *   The gti field of outbound messages depends on which of the other
 *   fields have been set.
 * - The value supplied by the application for the sa_pointcode field of
 *   local addresses is ignored. The actual local pointcode is used.
 * - The sa_rl_pc field is only relevant for incoming messages.
 * - The first address digit is in the least significant 4 bits of
 *   sa_gt.sag_digits[0].  ie reversed from a normal hexdump of the array.
*/
typedef struct acu_sccp_addr {
    unsigned char     sa_flags;
#define ACU_SCCP_SA_FLAGS_ROUTE_SSN   (1 << 0)
#define ACU_SCCP_SA_FLAGS_RAW_GT      (1 << 1)
#define ACU_SCCP_SA_FLAGS_RESERVED    (1 << 7)     /* Used internally */
    unsigned char     sa_valid;
#define ACU_SCCP_SA_FIELD(NAME, name, val, max, type) \
        unsigned type     sa_##name;
    ACU_SCCP_ADDR_FIELD(ACU_SCCP_SA_FIELD)
#undef ACU_SCCP_SA_FIELD
    struct acu_sccp_gt {
        unsigned short    sag_num;           /* Number of digits */
        unsigned char     sag_digits[62];    /* BCD digits (2 per byte) */
    } sa_gt;                                 /* Global title */
} acu_sccp_addr_t;

/* Number of bytes needed when copying an address buffer */
#define ACU_SCCP_ADDR_LEN(sa) \
    (offsetof(acu_sccp_addr_t, sa_gt.sag_digits) + ((sa)->sa_gt.sag_num + 1)/2)


/* Tags for configuring local and remote SCCP addresses */
#define ACU_TCAP_CFG_SCCP_ADDR(NAME, name, val, max, type) \
        ACU_TCAP_CFG_LOCAL_##NAME = 0x40 + (val), \
        ACU_TCAP_CFG_CLEAR_LOCAL_##NAME = 0x50 + (val), \
        ACU_TCAP_CFG_REMOTE_##NAME = 0x60 + (val), \
        ACU_TCAP_CFG_CLEAR_REMOTE_##NAME = 0x70 + (val), \

/* Configurable parameters for acu_tcap_ssap/tran_set_cfg() */
typedef enum {
  /* SSAP parameters */
    ACU_TCAP_CFG_LOGFILE,             /* Name of logfile */
    ACU_TCAP_CFG_TRACE_TAG,           /* Name for trace entries for this ssap */
    ACU_TCAP_CFG_LOGFILE_MAX_SIZE,    /* Maximum size of logfile */
    ACU_TCAP_CFG_LOGFILE_OLD_KEPT,    /* Number of old logfiles kept */
    ACU_TCAP_CFG_LOGFILE_APPEND,      /* Append to logfile */
    ACU_TCAP_CFG_TRACE_MODE,          /* 0 => line, 1 => block, 2 => cyclic */
    ACU_TCAP_CFG_TRACE_BUFFER_SIZE,   /* Buffer size for block/cyclic trace */
    ACU_TCAP_CFG_LOGFILE_FLOCK_INDEX, /* lockf() logfile.index */
    ACU_TCAP_CFG_SERVER       = 0x10, /* Set to receive BEGIN messages */
    ACU_TCAP_CFG_UNI_SERVER,          /* Set to receive UNIDIRECTIONAL msgs */
    ACU_TCAP_CFG_TRANID_RANGE = 0x18, /* Top 12 bits of transaction IDs */
    ACU_TCAP_CFG_TRAN_RESTORE,        /* Restore transaction on rx continue */
    ACU_TCAP_CFG_REATTACH_TIMEOUT,    /* Timeout for app restart and reattach */
    ACU_TCAP_CFG_REATTACH_DISCARD,    /* Set to discard (not queue) */
    ACU_TCAP_CFG_RX_MSG_WINDOW,       /* For flow control from driver */
    ACU_TCAP_CFG_RX_BYTE_WINDOW,      /* For flow control from driver */
    ACU_TCAP_CFG_RX_MAX_QLEN,         /* For flow control from driver */
    ACU_TCAP_CFG_UNKNOWN_TO_APP,      /* Pass messages with unknown transactions */
    ACU_TCAP_CFG_SINGLE_THREADED = 0x20, /* Do not block transactions */
    ACU_TCAP_CFG_HOST_A_NAME  = 0x30, /* IPv4 address (string) */
    ACU_TCAP_CFG_HOST_A_PORT,         /* TCP port number */
    ACU_TCAP_CFG_HOST_A_PASSWORD,     /* Password for TCP connection */
    ACU_TCAP_CFG_HOST_B_NAME  = 0x34, /* IPv4 address (string) */
    ACU_TCAP_CFG_HOST_B_PORT,         /* TCP port number */
    ACU_TCAP_CFG_HOST_B_PASSWORD,     /* Password for TCP connection */
    ACU_TCAP_CFG_RX_BUFLEN    = 0x38, /* TCP rx buffer size */
    ACU_TCAP_CFG_KEEPALIVE_TIMEOUT,   /* Interval between keepalives */
    ACU_TCAP_CFG_TX_QUEUE_LEN,        /* Buffers in TX queue before flow ctl */
    ACU_TCAP_CFG_CONNECT_TIMEOUT,     /* Connection (to SCCP) timeout */
    ACU_TCAP_CFG_TX_BYTE_WINDOW,      /* Byte limit for ack from driver */
  /* 0x40-0x5f for local address and 0x60-0x7f for remote address */
    ACU_SCCP_ADDR_FIELD(ACU_TCAP_CFG_SCCP_ADDR)
    ACU_TCAP_CFG_LOCAL_FLAGS  = 0x48,
    ACU_TCAP_CFG_LOCAL_GT_DIGITS,
    ACU_TCAP_CFG_NI           = 0x4f, /* Network Indicator */
    ACU_TCAP_CFG_REMOTE_FLAGS= 0x68,
    ACU_TCAP_CFG_REMOTE_GT_DIGITS,
  /* These values set on the SSAP will be inherited by all transactions */
    ACU_TCAP_CFG_QOS_RET_OPT  = 0x80, /* Request SCCP set 'return on error' */
    ACU_TCAP_CFG_QOS_SEQ_CTRL,        /* Send all messages on same SLS */
    ACU_TCAP_CFG_QOS_PRIORITY,        /* Message priority 0..7 (or ~0u) */
    ACU_TCAP_CFG_OPERATION_TIMEOUT,   /* Default Operation (Invoke) timeout */
    ACU_TCAP_CFG_ADD_TCAP_VERSION,    /* Non-zero to include tcap version */
    ACU_TCAP_CFG_QOS_RESPONSE_PRI,    /* Message priority for responses */
    ACU_TCAP_CFG_REVERSE_ROUTE,       /* Use reverse route for responses */
    ACU_TCAP_CFG_ENC_DEF_LEN,         /* Encode with definite length */
    ACU_TCAP_CFG_PREFERRED_MAXLEN,    /* Preferred max length for TCAP messge */
    ACU_TCAP_CFG_RESPOND_RX_LOC_GT,   /* Respond from received local GT */
    ACU_TCAP_CFG_STICKY_CON,          /* Non-zero: Send all data one host */
  /* Trace levels for each source - currently there are 64 sources */
    ACU_TCAP_CFG_TRACE_LEVEL_ALL = 0xff,
    ACU_TCAP_CFG_TRACE_LEVEL_0 = 0x100
#define ACU_TCAP_CFG_TRACE_LEVEL(source) (ACU_TCAP_CFG_TRACE_LEVEL_0 + (source))
} acu_tcap_cfg_param_t;
#undef ACU_TCAP_CFG_SCCP_ADDR

/* ssap flags */
typedef enum {
    ACU_TCAP_ITU        = 0x00000,
    ACU_TCAP_ANSI       = 0x00001,
    ACU_TCAP_SERVER     = 0x00010,   /* Set to receive BEGIN mssages */
    ACU_TCAP_UNI_SERVER = 0x00020,   /* Set to receive UNIDIRECTIONAL msgs */
    ACU_TCAP_LOG_APPEND = 0x00040,   /* Append to log file */
    ACU_TCAP_LOG_NOLOCK = 0x00080,   /* Rotate logfile without lockf(),
                                      * inverted from CFG_LOGFILE_FLOCK_INDEX */
    ACU_TCAP_LOG_STDERR = 0x00100,   /* Write early errors to stderr */
    ACU_TCAP_STATUS_IND = 0x01000    /* Set to receive raw SCCP status msgs */
} acu_tcap_ssap_flags_t;

/* Trace flags */
enum {
  /* Low 8 bits identify trace source */
    ACU_TCAP_TRACE_APPLICATION_0,           /* 0..9 for application use */
#define ACU_TCAP_TRACE_APPLICATION(n) ((n) & ACU_TCAP_TRACE_MASK)
    ACU_TCAP_TRACE_API_ENTRY = 0x10,        /* Entry to API routine */
    ACU_TCAP_TRACE_API_EXIT,                /* Normal exit from same */
    ACU_TCAP_TRACE_API_ERROR,               /* Error exit */
    ACU_TCAP_TRACE_API_EVENT,               /* API event (etc) */
    ACU_TCAP_TRACE_API_INFO,                /* Additional information */
    ACU_TCAP_TRACE_API_CONFIG,              /* Config change */
    ACU_TCAP_TRACE_API_OP_TIMER,            /* Operation timers */
    ACU_TCAP_TRACE_ENCODER = 0x20,          /* TCAP message being sent */
    ACU_TCAP_TRACE_DECODER,                 /* Received TCAP message */
    ACU_TCAP_TRACE_TCAP_MSG,                /* General message processing */
    ACU_TCAP_TRACE_STATUS,                  /* Pointcode and user status */
    ACU_TCAP_TRACE_QUEUE_ERROR = 0x28,      /* Message queue errors */
    ACU_TCAP_TRACE_MUTEX_ERROR,             /* Mutex lock/unlock erors */
    ACU_TCAP_TRACE_TCP       = 0x30,        /* TCP control */
    ACU_TCAP_TRACE_TCP_SEND,                /* TCP message being sent */
    ACU_TCAP_TRACE_TCP_RECV,                /* Received TCP message */
    ACU_TCAP_TRACE_MASK = 0x3f,             /* Last entry we keep level for */

  /* Bits 8-11 tell 'ss7maint decode' how to format the data */
    ACU_TCAP_TRACE_FMT_SIGTRAN = 0x100,     /* Trace data is TCP SIGTRAN */
    ACU_TCAP_TRACE_FMT_TCAP    = 0x600,     /* Trace data is TCAP ASN.1 */
    ACU_TCAP_TRACE_FMT_ASN1    = 0x700,     /* Trace data is ASN.1 */

  /* Bits 12-15 are the trace level/priority, lower being higher priority */
  /* These are used to filter the written trace, and by 'ss7maint decode' */
    ACU_TCAP_TRACE_LEVEL_1 = 0x1000,        /* 16 trace levels per source */
#define ACU_TCAP_TRACE_LEVEL(n)      (ACU_TCAP_TRACE_LEVEL_1 * ((n) & 15))

    ACU_TCAP_TRACE_ALWAYS    = 1u << 30,    /* => always trace */
    ACU_TCAP_TRACE_FIRST     = 1u << 29,    /* => acquire lock and add header */
    ACU_TCAP_TRACE_LAST      = 1u << 28,    /* => release lock */
    ACU_TCAP_TRACE_FLUSH     = 1u << 27,    /* => write buffered trace */
    ACU_TCAP_TRACE_MIDDLE    = 0,
    ACU_TCAP_TRACE_ONLY      = ACU_TCAP_TRACE_FIRST | ACU_TCAP_TRACE_LAST
};

/* Defines for building 'flags' parameter to trace functions */
#define ACU_TCAP_TRF(part, src, lvl) \
    (ACU_TCAP_TRACE_##part | ACU_TCAP_TRACE_##src | ACU_TCAP_TRACE_LEVEL(lvl))
#define ACU_TCAP_TRFF(part, src, lvl, fmt) \
    (ACU_TCAP_TRF(part, src, lvl) | ACU_TCAP_TRACE_FMT_##fmt)

/* Trace mode constants */
#define ACU_TCAP_TRACE_MODE_LINE    0       /* Write every entry */
#define ACU_TCAP_TRACE_MODE_BLOCK   1       /* Write when buffer full */
#define ACU_TCAP_TRACE_MODE_CYCLIC  2       /* Write on request only */

/* Private structures */
typedef struct acu_tcap_ssap acu_tcap_ssap_t;
typedef struct acu_tcap_transaction acu_tcap_trans_t;
typedef struct acu_tcap_event acu_tcap_event_t;

/* State of the TCP connection to SCCP */
typedef enum {
    ACU_TCAP_CON_STATE_IDLE,
    ACU_TCAP_CON_STATE_CONNECTING,
    ACU_TCAP_CON_STATE_CONNECTED,
    ACU_TCAP_CON_STATE_IN_SERVICE,
  /* If IN_SERVICE the following bits can be set */
    ACU_TCAP_CON_STATE_RX_BLOCKED = 0x010000,
    ACU_TCAP_CON_STATE_RX_FLOW    = 0x020000,
    ACU_TCAP_CON_STATE_TX_BLOCKED = 0x100000,
    ACU_TCAP_CON_STATE_TX_FLOW    = 0x200000
} acu_tcap_cs_state_t;

/* Overall reason why the connection failed */
#define ACU_TCAP_CON_FAIL_INFO(x) \
x(ACU_TCAP_CON_FAIL_SSAP_DELETED,    "ssap deleted") \
x(ACU_TCAP_CON_FAIL_CON_TIMEOUT,     "TCP/IP connect request timed out") \
x(ACU_TCAP_CON_FAIL_CON_REJECTED,    "TCP/IP connect rejected") \
x(ACU_TCAP_CON_FAIL_LOGIN_REJECTED,  "") /* Login sequence failed */ \
x(ACU_TCAP_CON_FAIL_INWARD,          "inward disconnected") \
x(ACU_TCAP_CON_FAIL_KEEPALIVE,       "keepalive timed out") \
x(ACU_TCAP_CON_FAIL_REGISTRATION,    "registration with SCCP failed") \
x(ACU_TCAP_CON_FAIL_BAD_MESSAGE,     "corrupt message received from SCCP") \

#define ACU_TCAP_CON_FAIL_INFO_ENUM(tag, text) tag,
typedef enum { ACU_TCAP_CON_FAIL_INFO(ACU_TCAP_CON_FAIL_INFO_ENUM)
    ACU_TCAP_CON_FAIL_NUM_ERRORS
} acu_tcap_cs_fail_t;
#undef ACU_TCAP_CON_FAIL_INFO_ENUM

/* Information about the status of a TCP/IP connection to SCCP */
typedef struct acu_tcap_con_state {
    unsigned int        cs_ipaddr;           /* IPv4 address of SCCP system */
    unsigned int        cs_tcpport;          /* TCP port number */
    acu_tcap_cs_state_t cs_state;            /* Current state */
    acu_tcap_cs_fail_t  cs_failure;          /* Reason for last connect fail */
    const char          *cs_fail_text;       /* NB: more descriptive */
    unsigned int        cs_tx_qlen;          /* Transmit queue length */
    struct sockaddr_in6 cs_sockaddr;
} acu_tcap_con_state_t;

/* Information about a received TCAP message dialogue */
typedef struct acu_tcap_dialogue {
    acu_tcap_dlg_type_t  td_type;           /* ASN.1 type of dialogue portion */

    /* Bits that indicate which fields below are valid */
    unsigned int         td_flags;
#define ACU_TCAP_DF_HAS_UI            (1u << 2)    /* td_ui/_len */
#define ACU_TCAP_DF_HAS_HEX_APP_CTX   (1u << 3)    /* td_app_ctx/_len */
#define ACU_TCAP_DF_HAS_INT_APP_CTX   (1u << 4)    /* in td_app_ctx_len */
#define ACU_TCAP_DF_HAS_AARE_DIAG     (1u << 5)    /* in td_result */
#define ACU_TCAP_DF_HAS_ABRT_SOURCE   (1u << 6)    /* in td_result */
#define ACU_TCAP_DF_HAS_USER_SYNTAX   (1u << 8)    /* td_ui/_len */
#define ACU_TCAP_DF_HAS_INT_SEC_CTX   (1u << 9)    /* in td_sec_ctx_len */
#define ACU_TCAP_DF_HAS_OBJ_SEC_CTX   (1u << 10)   /* td_sec_ctx/_len */
#define ACU_TCAP_DF_HAS_INT_CFD_ALG   (1u << 11)   /* in td_cfd_alg_len */
#define ACU_TCAP_DF_HAS_OBJ_CFD_ALG   (1u << 12)   /* td_cfd_alg/_len */
#define ACU_TCAP_DF_HAS_CFD_VAL       (1u << 13)   /* td_cfd_val/_len */

    const unsigned char  *td_app_ctx;       /* Application context name */
    int                  td_app_ctx_len;
    int                  td_pad_1;

    unsigned int         td_result;         /* ITU AARE or ABRT */

    const unsigned char  *td_sec_ctx;       /* ANSI only */
    int                  td_sec_ctx_len;    /* Security context */

    const unsigned char  *td_ui;            /* Dialogue userinfo */
    unsigned int         td_ui_len;

    const unsigned char  *td_cfd_alg;       /* ANSI only */
    int                  td_cfd_alg_len;    /* Confidentiality algorithm */
    const unsigned char  *td_cfd_val;       /* ANSI only */
    unsigned int         td_cfd_val_len;    /* Confidentiality value */
} acu_tcap_dialogue_t;

/* Information about a received TCAP message component */
typedef struct acu_tcap_component {
    acu_tcap_comp_type_t tc_type;          /* ASN.1 type of component */

    /* Bits that indicate which fields below are valid */
    unsigned int         tc_flags;
#define ACU_TCAP_CF_HAS_INVOKE_ID     (1u << 0)    /* tc_invoke_id */
#define ACU_TCAP_CF_HAS_LINKED_ID     (1u << 1)    /* tc_linked_id */
#define ACU_TCAP_CF_HAS_PARAMETER     (1u << 2)    /* tc_param/_len */
#define ACU_TCAP_CF_HAS_HEX_OPCODE    (1u << 3)    /* tc_op_code/_len */
#define ACU_TCAP_CF_HAS_INT_OPCODE    (1u << 4)    /* tc_op_code_val */
#define ACU_TCAP_CF_INVOKE_ID_LOCAL   (1u << 5)    /* tc_invoke_id is local */
#define ACU_TCAP_CF_SEQ_TAG           (1u << 31)   /* Used internally */

    char                 tc_invoke_id;
    char                 tc_linked_id;     /* From received invoke component */

    char                 tc_pad3[2];

    /* Component operation/error/reject code */
    const unsigned char  *tc_op_code;      /* Address of TLV (ie OBJECT-ID) */
    unsigned int         tc_op_code_len;   /* and its length */
    unsigned int         tc_op_code_val;   /* Value of integer code */

    /* Component parameter */
    const unsigned char  *tc_param;        /* Address of TLV (ANY defd by op) */
    unsigned int         tc_param_len;     /* and its length */

    /* For locally generated component rejects */
    unsigned int         tc_rejected_type; /* Original component type */
    acu_tcap_errors_t    tc_reject_error;
} acu_tcap_component_t;


#ifndef ACU_SCCP_U_STATUS_LIST
#define ACU_SCCP_STATUSVAL(tag, text) tag,


/* Status of a remote user (pointcode + SSN) */
#define ACU_SCCP_U_STATUS_LIST(x) \
x(ACU_SCCP_UOS,               "UOS")   /* User out of service */ \
x(ACU_SCCP_UIS,               "UIS")   /* User in service */ \

enum acu_sccp_u_svals {
    ACU_SCCP_U_STATUS_LIST(ACU_SCCP_STATUSVAL)
    ACU_SCCP_NUM_USTATUS
};

/* Status of remote signalling point (from MTP3) */
#define ACU_SCCP_SP_STATUS_LIST(x) \
x(ACU_SCCP_SP_PROHIBIT,       "SP Prohibited")\
x(ACU_SCCP_SP_RES1,           "Reserved")\
x(ACU_SCCP_SP_RES2,           "Reserved")\
x(ACU_SCCP_SP_ACCESS,         "SP Accessible") \

enum acu_sccp_sp_svals {
    ACU_SCCP_SP_STATUS_LIST(ACU_SCCP_STATUSVAL)
    ACU_SCCP_NUM_SPSTATUS
};

/* Status of remote SCCP system */
#define ACU_SCCP_SCCP_STATUS_LIST(x) \
x(ACU_SCCP_REM_SCCP_PROHIBIT, "Remote SCCP Prohibited")\
x(ACU_SCCP_REM_SCCP_UNAVAIL,  "Remote SCCP Unavailable, reason unknown")\
x(ACU_SCCP_REM_SCCP_UNEQUIP,  "Remote SCCP Unequipped")\
x(ACU_SCCP_REM_SCCP_INACCESS, "Remote SCCP Inaccessible")\
x(ACU_SCCP_REM_SCCP_CONGEST,  "Remote SCCP Congested")\
x(ACU_SCCP_REM_SCCP_AVAIL,    "Remote SCCP Available") \

enum acu_sccp_sccp_svals {
    ACU_SCCP_SCCP_STATUS_LIST(ACU_SCCP_STATUSVAL)
    ACU_SCCP_NUM_SCCPSTATUS
};

#undef ACU_SCCP_STATUSVAL
#endif

/* Structure returned by acu_tcap_msg_get_sccp_status_ind() with information
 * from ACU_TCAP_MSG_SP/USER_STATUS messages */
typedef struct acu_tcap_sccp_status {
    unsigned int             tsp_pc;        /* Affected pointcode */
    unsigned char            tsp_ssn;       /* Affected SSN (or zero) */
    char                     tsp_host;      /* Reporting host, 'a' or 'b' */
    char                     tsp_pad[2];
    enum acu_sccp_u_svals    tsp_user_status;
    enum acu_sccp_sp_svals   tsp_sp_status;
    enum acu_sccp_sccp_svals tsp_sccp_status;
    unsigned int             tsp_tx_cong_cost;
} acu_tcap_sccp_status_t;

/*
 * TCAP message structure.  Used for both inward and outbound messages.
 * Inbound messages are queued on both the ssap and transaction data areas.
 * NB This is the first element of the much longer structure used internally.
 * Applications should not modify any fields of this structure.
 */
struct acu_tcap_msg {
    acu_tcap_msg_type_t    tm_msg_type;      /* Overall received message type */

    acu_tcap_ssap_t        *tm_ssap;         /* Associated ssap */
    acu_tcap_trans_t       *tm_trans;        /* and transaction (if any) */

    /* Address fields for received messages (NULL if invalid) */
    const acu_sccp_addr_t  *tm_local_addr;   /* Called address from SCCP */
    const acu_sccp_addr_t  *tm_remote_addr;  /* Calling address from SCCP */

    unsigned char          tm_ret_opt;       /* SCCP 'return on error' */
    unsigned char          tm_seq_ctrl;      /* effectively SCCP class 0,1 */

    unsigned char          tm_ret_cause;     /* Return cause from (X)UDTS */

    unsigned char          tm_priority;      /* ANSI message priority */

    unsigned int           tm_p_abort_cause;
};
#undef acu_tcap_msg

typedef struct acu_tcap_msg acu_tcap_msg_t;

#ifdef _WIN32
typedef void *acu_tcap_os_event_t;
#else
typedef int acu_tcap_os_event_t;
#endif

#ifdef __GNUC__
#define ACU_TCAP_PRINTF_ATTR(F, L) __attribute__((format(printf, (F), (L))))
#else
#define ACU_TCAP_PRINTF_ATTR(F, L)
#endif

/*
 * The TCAP interfaces are defined using this pre-processor 'magic' in order to
 * make it possible to define other data structures using the same information.
 * In particular it can be used to define the data areas needed to dynamically
 * load the TCAP library.
 */

#define ACU_TCAP_FUNCTIONS(x) \
ACU_ASN1_FUNCTIONS(x) \
x(void, acu_tcap_trace_v, (acu_tcap_ssap_t *, unsigned int, const void *, int, \
                        const char *, va_list)) \
x(void, acu_tcap_trace, (acu_tcap_ssap_t *, unsigned int, const char *, ...) \
                        ACU_TCAP_PRINTF_ATTR(3, 4)) \
x(void, acu_tcap_trace_buf, (acu_tcap_ssap_t *, unsigned int, const void *, \
                        int, const char *, ...) ACU_TCAP_PRINTF_ATTR(5, 6)) \
x(int, acu_tcap_trace_error, (acu_tcap_ssap_t *, const char *, int, \
                        const char *, ...) ACU_TCAP_PRINTF_ATTR(4, 5)) \
x(const char *, acu_tcap_strerror, (int, unsigned int)) \
\
/* SSAP functions */ \
\
x(acu_tcap_ssap_t *, acu_tcap_ssap_create, (const char *, \
                        acu_tcap_ssap_flags_t)) \
x(void, acu_tcap_ssap_delete, (acu_tcap_ssap_t *)) \
x(int, acu_tcap_ssap_connect_sccp, (acu_tcap_ssap_t *)) \
x(int, acu_tcap_ssap_set_cfg_int, (acu_tcap_ssap_t *, acu_tcap_cfg_param_t, \
                        unsigned int)) \
x(int, acu_tcap_ssap_set_cfg_str, (acu_tcap_ssap_t *, acu_tcap_cfg_param_t, \
                        const char *)) \
x(acu_sccp_addr_t *, acu_tcap_ssap_get_locaddr, (acu_tcap_ssap_t *)) \
x(acu_sccp_addr_t *, acu_tcap_ssap_get_remaddr, (acu_tcap_ssap_t *)) \
\
/* Transaction functions */ \
\
x(acu_tcap_trans_t *, acu_tcap_transaction_create, (acu_tcap_ssap_t *)) \
x(void, acu_tcap_transaction_delete, (acu_tcap_trans_t *)) \
x(acu_tcap_trans_t *, acu_tcap_ssap_get_uni_transaction, (acu_tcap_ssap_t *)) \
x(void, acu_tcap_trans_set_userptr, (acu_tcap_trans_t *, void *)) \
x(void *, acu_tcap_trans_get_userptr, (acu_tcap_trans_t *)) \
x(int, acu_tcap_trans_get_ids, (acu_tcap_trans_t *, unsigned int *, \
                        unsigned int *, unsigned int *)) \
x(int, acu_tcap_trans_set_cfg_int, (acu_tcap_trans_t *, acu_tcap_cfg_param_t, \
                        unsigned int)) \
x(int, acu_tcap_trans_set_cfg_str, (acu_tcap_trans_t *, acu_tcap_cfg_param_t, \
                        const char *)) \
x(acu_sccp_addr_t *, acu_tcap_trans_get_locaddr, (acu_tcap_trans_t *)) \
x(acu_sccp_addr_t *, acu_tcap_trans_get_remaddr, (acu_tcap_trans_t *)) \
\
/* Common message functions */ \
\
x(acu_tcap_msg_t *, acu_tcap_msg_alloc, (acu_tcap_trans_t *)) \
x(void, acu_tcap_msg_free, (acu_tcap_msg_t *)) \
x(acu_asn1_buf_t *, acu_tcap_msg_get_a1b, (acu_tcap_msg_t *)) \
x(int, acu_tcap_msg_get_ids, (acu_tcap_msg_t *, unsigned int *, \
                        unsigned int *, unsigned int *)) \
\
/* Message sending functions */ \
\
x(int, acu_tcap_msg_init, (acu_tcap_msg_t *, acu_tcap_msg_type_t)) \
x(int, acu_tcap_msg_add_dialogue, (acu_tcap_msg_t *, unsigned int, \
                        const void *, int)) \
x(int, acu_tcap_msg_add_dlg_userinfo, (acu_tcap_msg_t *, const void *, \
                        unsigned int)) \
x(int, acu_tcap_msg_add_dlg_security_context, (acu_tcap_msg_t *, \
                        const void *, int)) \
x(int, acu_tcap_msg_add_dlg_confidentiality, (acu_tcap_msg_t *, \
                        const void *, int, const void *, unsigned int)) \
x(int, acu_tcap_msg_add_comp_invoke, (acu_tcap_msg_t *, int, int, int, int, \
                        const void *, int, const void *, int)) \
x(int, acu_tcap_msg_add_comp_result, (acu_tcap_msg_t *, int, int, \
                        const void *, int, const void *, int)) \
x(int, acu_tcap_msg_add_comp_error, (acu_tcap_msg_t *, int, \
                        const void *, int, const void *, int)) \
x(int, acu_tcap_msg_add_comp_reject, (acu_tcap_msg_t *, int, \
                        acu_tcap_reject_problem_t, const void *, int)) \
x(int, acu_tcap_msg_add_ansi_abort_userinfo, (acu_tcap_msg_t *, const void *, \
                        unsigned int)) \
/* acu_tcap_msg_add_comp_param() is deprecated */ \
x(int, acu_tcap_msg_add_comp_param, (acu_tcap_msg_t *, const void *, int)) \
x(int, acu_tcap_msg_send, (acu_tcap_msg_t *)) \
x(int, acu_tcap_msg_reply_reject, (acu_tcap_msg_t *)) \
\
/* Message receiving functions */ \
\
x(int, acu_tcap_ssap_msg_get, (acu_tcap_ssap_t *, int, acu_tcap_msg_t **)) \
x(void, acu_tcap_ssap_wakeup_msg_get, (acu_tcap_ssap_t *)) \
x(int, acu_tcap_trans_msg_get, (acu_tcap_trans_t *, int, acu_tcap_msg_t **)) \
x(void, acu_tcap_trans_wakeup_msg_get, (acu_tcap_trans_t *)) \
x(int, acu_tcap_event_msg_get, (acu_tcap_event_t *, acu_tcap_msg_t **)) \
x(int, acu_tcap_msg_decode, (acu_tcap_msg_t *, const acu_tcap_dialogue_t **)) \
x(int, acu_tcap_msg_has_components, (acu_tcap_msg_t *)) \
x(int, acu_tcap_msg_get_component, (acu_tcap_msg_t *, \
                        const acu_tcap_component_t **))\
x(void, acu_tcap_trans_unblock, (acu_tcap_trans_t *)) \
x(int, acu_tcap_trans_block, (acu_tcap_trans_t *)) \
x(int, acu_tcap_msg_copy_rx_buffer, (acu_tcap_msg_t *)) \
\
/* Operation functions */ \
\
x(int, acu_tcap_operation_timer_start, (acu_tcap_trans_t *, int, unsigned int))\
x(int, acu_tcap_operation_timer_restart, (acu_tcap_trans_t *, int, \
                        unsigned int)) \
x(int, acu_tcap_operation_cancel, (acu_tcap_trans_t *, int)) \
\
/* Connection status functions */ \
\
x(int, acu_tcap_get_con_state, (acu_tcap_ssap_t *, int, \
                        const acu_tcap_con_state_t **)) \
x(int, acu_tcap_msg_get_con_state, (acu_tcap_msg_t *, \
                        const acu_tcap_con_state_t **, \
                        const acu_tcap_con_state_t **)) \
\
/* Remote SP and SSN status functions */ \
\
x(int, acu_tcap_get_sccp_status, (acu_tcap_ssap_t *, unsigned int, \
                        unsigned int, const acu_tcap_sccp_status_t **)) \
x(int, acu_tcap_msg_get_sccp_status, (acu_tcap_msg_t *, \
                        const acu_tcap_sccp_status_t **)) \
x(void, acu_tcap_enable_user_status, (acu_tcap_ssap_t *, unsigned int, \
                        unsigned int)) \
x(void, acu_tcap_enable_sp_status, (acu_tcap_ssap_t *, unsigned int)) \
\
/* Event functions */ \
\
x(acu_tcap_event_t *, acu_tcap_event_create, (acu_tcap_ssap_t *)) \
x(void, acu_tcap_event_delete, (acu_tcap_event_t *)) \
x(int, acu_tcap_event_wait, (acu_tcap_event_t *, int)) \
x(acu_tcap_os_event_t, acu_tcap_event_get_os_event, (acu_tcap_event_t *)) \
x(void, acu_tcap_event_clear, (acu_tcap_event_t *)) \
x(void, acu_tcap_event_signal, (acu_tcap_event_t *)) \
x(int, acu_tcap_event_ssap_attach, (acu_tcap_event_t *, acu_tcap_ssap_t *)) \
x(int, acu_tcap_event_ssap_detach, (acu_tcap_event_t *, acu_tcap_ssap_t *)) \
x(int, acu_tcap_event_ssap_detach_all, (acu_tcap_ssap_t *)) \
x(int, acu_tcap_event_trans_attach, (acu_tcap_event_t *, acu_tcap_trans_t *)) \
x(int, acu_tcap_event_trans_detach, (acu_tcap_event_t *, acu_tcap_trans_t *)) \
x(int, acu_tcap_event_trans_detach_all, (acu_tcap_trans_t *)) \
\
/* Transaction restore functions */ \
\
x(acu_tcap_trans_t *, acu_tcap_transaction_restore, (acu_tcap_ssap_t *, \
                        unsigned int, unsigned int, unsigned int)) \
x(int, acu_tcap_operation_restore, (acu_tcap_trans_t *, int, \
                        unsigned int, unsigned int)) \
x(int, acu_tcap_transaction_restore_app_ctx, (acu_tcap_trans_t *, \
                        const void *, unsigned int)) \
\
/* Undocumented diagnostic functions */ \
x(void, acu_tcap_dump_operation_timers, (acu_tcap_ssap_t *, int)) \

/* Define C external function definitions */
#ifndef ACU_TCAP_NO_FN_DEFS      /* Define to suppress these definitions */
#define ACU_TCAP_FN_DEFS(rslt, name, args) ACU_TCAP_LIB_API rslt name args;
ACU_TCAP_FUNCTIONS(ACU_TCAP_FN_DEFS)
#undef ACU_TCAP_FN_DEFS
#endif

/* Define for C function variable externs */
#define ACU_TCAP_EXTERN_FN_DEFS_(rslt, name, args) extern rslt (*name) args;
#define ACU_TCAP_EXTERN_FN_DEFS ACU_TCAP_FUNCTIONS(ACU_TCAP_EXTERN_FN_DEFS_)

/* Define for C function variables */
#define ACU_TCAP_FN_DEFS_(rslt, name, args) rslt (*name) args = 0;
#define ACU_TCAP_FN_DEFS ACU_TCAP_FUNCTIONS(ACU_TCAP_FN_DEFS_)

/* Code fragment to initialise function pointers from library handle/file */
#define ACU_TCAP_FN_INIT(fd, err_fn) \
{ \
    typedef struct { const char *fn_name; void **fn_addr; } fni_t; \
    static const fni_t fn_info[] = {ACU_TCAP_FUNCTIONS(ACU_TCAP_FN_TABLE) {0}};\
    const fni_t *fni; \
    for (fni = fn_info; fni->fn_name; fni++) { \
        *fni->fn_addr = dlsym((fd), fni->fn_name); \
        if (!*fni->fn_addr) { \
            err_fn(fni->fn_name); \
            for (fni = fn_info; fni->fn_name; fni++) \
                *fni->fn_addr = 0; \
            break; \
        } \
    } \
}
#define ACU_TCAP_FN_TABLE(rval, name, arg) {#name, (void **)(void *)&name},
#ifdef _WIN32
#define dlsym(fd, fn) GetProcAddress(fd, fn)
#endif

#ifdef __cplusplus
}
#endif

#endif
