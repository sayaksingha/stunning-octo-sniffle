/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/monitor/ss7monitor.h,v 1.71 2023/08/03 10:09:51 davidla Exp $ $Name: SS7_6_16_1 $" */

#ifndef SS7MONITOR_H
#define SS7MONITOR_H

/* SS7 monitor - external interfaces */


/*
 * Define an appropiate prefix for library function definitions,
 * in order to exclude internal functions from the library symbol table
 * and to ensure that the library calls its own public functions.
 *
 * The library itself is compiled with -fvisibility=hidden and
 * -DACU_MONITOR_LIB_API=__attribute__((visibility("protected")))
 */

#ifndef ACU_MONITOR_LIB_API
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) >= 30400
#define ACU_MONITOR_LIB_API __attribute__((visibility("default")))
#else
#define ACU_MONITOR_LIB_API
#endif
#endif

#include <stdio.h>
#include <stdarg.h>

#include <res_lib.h>
#include <sw_lib.h>

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#define SS7MON_PRINTF_ATTR(F, L)
#else
#include <sys/socket.h>
typedef int socket_t;
#define SS7MON_PRINTF_ATTR(F, L) __attribute__((format(printf, (F), (L))))
#endif

/* Errors all negative - NB Add new codes at the top */
#define ACU_SS7MON_ERRORS(x) \
x( NO_PT_MONITOR,          "Can't start pt_monitor" ) \
x( NO_CARD_INFO,           "acu_get_card_info() failed" ) \
x( BAD_STATE_REQ,          "Invalid state req" ) \
x( BAD_INTERFACE_ID,       "Invalid interface id" ) \
x( BAD_DATA,               "Invalid data ptr or length" ) \
x( BAD_ACTION,             "Invalid action parameter" ) \
x( INCORRECT_CARD_KEY,     "Incorrect card key" ) \
x( AUTH_FAILURE,           "Authentication failed" ) \
x( NO_CARD_KEY,            "Card key required, but not configured" ) \
x( AUTH_TIMEOUT,           "Authentication timed out" ) \
x( BAD_HOSTNAME,           "Unable to resolve address" ) \
x( BAD_LIB_REF,            "invalid ISUP library reference" ) \
x( BAD_USER_REF,           "invalid ISUP user reference" ) \
x( PRM_NOT_FOUND,          "parameter not found in message" ) \
x( UNKNOWN_CONFIG,         "unknown configuration parameter" ) \
x( BAD_PC_SIZE,            "invalid pointcode size" ) \
x( BAD_SI,                 "invalid si value" ) \
x( BAD_NI,                 "invalid ni value" ) \
x( MSG_VERSION,            "message corrupt (bad version)" ) \
x( MSG_OVERLONG,           "message corrupt (overlong)" ) \
x( SEND_FAILED,            "send() failed" ) \
x( RECV_FAILED,            "recv() failed" ) \
x( DISCONNECTED,           "remote end disconnected" ) \
x( ALREADY_CONNECTED,      "endpoint already connected" ) \
x( CONNECT_FAILED,         "connect() failed" ) \
x( NO_SOCKET,              "socket() failed" ) \
x( MALLOC_FAIL,            "malloc() failed" ) \
x( SUCCESS,                "Success" ) \

/* Warnings */
#define ACU_SS7MON_WRN_TX_FLOW_OFF 1

#define ACU_SS7MON_ERR_ENUM(name, text) ACU_SS7MON_ERROR_##name,
#define ACU_SS7MON_ERR_COUNT(name, text) -1
typedef enum {
    ACU_SS7MON_ERROR_HIGHEST = ACU_SS7MON_ERRORS(ACU_SS7MON_ERR_COUNT),
    ACU_SS7MON_ERRORS(ACU_SS7MON_ERR_ENUM)
    ACU_SS7MON_ERROR_NUM_ERRMSG = -ACU_SS7MON_ERROR_HIGHEST
} acu_ss7mon_errors_t;
#undef ACU_SS7MON_ERR_ENUM
#undef ACU_SS7MON_ERR_COUNT

/* Macros for deriving stream & timeslot from interface_id */
#define acu_ss7mon_interface_id_to_stream(id) (((id) >> 5) + 48)
#define acu_ss7mon_interface_id_to_ts(id)     ((id) & 0x1f)

/* Message types for mm_msg_type field */
#define ACU_SS7MON_MSG_NO_DATA         0x00
#define ACU_SS7MON_MSG_UNKNOWN         0x01
#define ACU_SS7MON_MSG_TRACE           0x02
#define ACU_SS7MON_MSG_MONITOR_ACK     0x03
#define ACU_SS7MON_MSG_MONITOR_FAIL    0x04
#define ACU_SS7MON_MSG_UNMONITOR_ACK   0x05
#define ACU_SS7MON_MSG_L1DATA          0x06
#define ACU_SS7MON_MSG_L2DATA_BAD      0x07
#define ACU_SS7MON_MSG_L3DATA          0x08
#define ACU_SS7MON_MSG_ISUPDATA        0x09    /* Non call-related ISUP msgs */
#define ACU_SS7MON_MSG_AUTHENTICATED   0x0a
#define ACU_SS7MON_MSG_CFG_LINK_ACK    0x0b
#define ACU_SS7MON_MSG_CFG_LINK_FAIL   0x0c
#define ACU_SS7MON_MSG_AUDIODATA       0x0d

#define ACU_SS7MON_MSG_IOC_RESPONSE(code)    (((code) << 16) | 0x10)

#define ACU_SS7MTP2_MSG_EST_CONF       0x100
#define ACU_SS7MTP2_MSG_REL_IND        0x101
#define ACU_SS7MTP2_MSG_REL_CONF       0x102
#define ACU_SS7MTP2_MSG_TX_FLOW_ON     0x103
#define ACU_SS7MTP2_MSG_RTRV_BSNT      0x104
#define ACU_SS7MTP2_MSG_CANT_RTRV      0x105
#define ACU_SS7MTP2_MSG_RTRV_DATA      0x106
#define ACU_SS7MTP2_MSG_RTRV_COMP      0x107
/* State change indications */
#define ACU_SS7MTP2_MSG_RPO_ENTER      0x108
#define ACU_SS7MTP2_MSG_RPO_EXIT       0x109
#define ACU_SS7MTP2_MSG_L2DATA         0x10a

/* Decode flags */
#define ACU_SS7MON_MEF_DECODE_MTP3       0x0001
#define ACU_SS7MON_MEF_DECODE_USERPART   0x0002
#define ACU_SS7MON_MEF_RAW_TRACE         0x0010   /* Set to leave header on trace */
#define ACU_SS7MON_MEF_EXTENDED_SEQ      0x0020   /* Extended sequence number for HSL */
#define ACU_SS7MON_MEF_ISDN_LAPD         0x0040   /* Monitoring ISDN/LAPD not SS7 */

#define ACU_SS7MON_MEF_IOCTL_SENT        0x8000   /* Internal use only */

/* 'Action' parameters for retrieval interface */
#define ACU_SS7MTP2_REQ_RTRV_BSNT         0x01
#define ACU_SS7MTP2_REQ_RTRV_MSGS         0x02

/* 'State' parameters for state request */
#define ACU_SS7MTP2_REQ_LPO_SET           0x0
#define ACU_SS7MTP2_REQ_LPO_CLEAR         0x1
#define ACU_SS7MTP2_REQ_EMER_SET          0x2
#define ACU_SS7MTP2_REQ_EMER_CLEAR        0x3

/* ACU_SS7MON_CFG_L2_MODE parameter values */
#define ACU_SS7MON_L2_MODE_MTP2           0x0   /* MTP2 with E1 timer values */
#define ACU_SS7MON_L2_MODE_T1             0x1   /* MTP2 with T1 timer values */
#define ACU_SS7MON_L2_MODE_PCR            0x2
#define ACU_SS7MON_L2_MODE_RAW            0x4   /* Monitor mode (default) */
#define ACU_SS7MON_L2_MODE_APP_EST_REQ    0x8   /* APP calls est_req() */

/* Library internal structure that describes an endpoint */
typedef struct acu_ss7mon_ep acu_ss7mon_ep_t;

typedef void *acu_ss7mon_isup_lib_ref_t;
typedef void *acu_ss7mon_isup_user_ref_t;

/* Names for configurable parameters */
typedef enum {
    ACU_SS7MON_CFG_L2_MODE,             /* Set to 4 (default) for raw hdlc */
    ACU_SS7MON_CFG_RXBUF_SIZE,          /* Set before connect */
    ACU_SS7MON_CFG_DECODE_FLAGS,        /* Decodes to perform */
    ACU_SS7MON_CFG_PMXC_PORT,           /* TCP port to connect to */
    ACU_SS7MON_CFG_HIGH_SPEED,          /* Number of timeslots for high speed */
    ACU_SS7MON_CFG_HSL_TS_IGNORE,       /* Timeslots to ignore on HSL */
    ACU_SS7MON_CFG_H100_STREAM,         /* H.100 stream for retiming port 0 HSL */
    ACU_SS7MON_CFG_TRACE_FILE_MAX_SIZE, /* Max size of trace file (bytes) */
    ACU_SS7MON_CFG_TRACE_FILE_KEPT,     /* Number of trace files kept */
    ACU_SS7MON_CFG_TRACE_LEVEL,         /* Trace level filter, default 5 */
    ACU_SS7MON_CFG_AUTH_TMO_MS,         /* Timeout for connect authenticate */
    ACU_SS7MON_CFG_INT_COUNT,
    ACU_SS7MON_CFG_STRING = 0x1000,
    ACU_SS7MON_CFG_L2_CONFIG,           /* xxx=nnn list for card */
    ACU_SS7MON_CFG_CARD_KEY,            /* Security key for card */
    ACU_SS7MON_CFG_CARD_SERIAL,         /* Card serial number */
    ACU_SS7MON_CFG_TRACE_FILENAME,      /* Filename for library log/trace */
    ACU_SS7MON_CFG_PT_MONITOR,          /* Use alternate monitor on PT cards */
    ACU_SS7MON_CFG_STRING_COUNT
} acu_ss7mon_cfg_param_t;

/* Information about a received message */
typedef struct acu_ss7mon_msg {
    acu_ss7mon_ep_t *mm_ep;            /* Endpoint message came from */
    unsigned int    mm_msg_type;
    unsigned int    mm_link_id;        /* User specified token for link */
    unsigned int    mm_interface_id;   /* System token, 0..255 = timeslot */
    int             mm_status;         /* Error status for some message types */
    unsigned char   *mm_buffer;        /* Pointer to userdata */
    unsigned int    mm_buflen;         /* Length of userdata */

    /* From MTP2 & MTP3 headers - set by acu_ss7mon_decode_mtp3,
     * when mm_buffer is advanced past the MTP3 routing label.  */
    unsigned char   mm_bsn_bib;        /* Byte 0 of raw frame */
    unsigned char   mm_fsn_fib;        /* Byte 1 of raw frame */
    unsigned char   mm_len_pri;        /* Byte 2 of raw frame */
    unsigned char   mm_sio;            /* Byte 3 of raw frame */

    unsigned int    mm_dpc;            /* From MTP3 routing label */
    unsigned int    mm_opc;            /* From MTP3 routing label */
    unsigned char   mm_ni;             /* From mm_sio */
    unsigned char   mm_si;             /* From mm_sio */
    unsigned char   mm_sls;            /* From MTP3 routing label */
    unsigned char   mm_h1h0;           /* For si <= 2 */

    void            *mm_upart_info;    /* MTP3 Userpart specific data */
} acu_ss7mon_msg_t;

/* Info passed to userpart decode functions.
 * The function will (probably) want to allocate a state structure and
 * assign it to ui_state - it will then be available for decoding further
 * messages between the same pair of pointcodes.
 * The decode function is called with a NULL message parameter in order
 * to free this area when an endpoint is deleted.
 * The ui_config pointer is that passed to set_upart_decode() by the
 * application, the same pointer will be passed for multiple pointcode pairs.
 */
typedef struct acu_ss7mon_ui {
    void          *ui_state;           /* Per pointcode-pair state */
    void          *ui_config;          /* User-supplied config */
} acu_ss7mon_ui_t;

/***** Definitions for ISUP user part decoder *******/

/* Generic indication types used by ISUP decoder (ii_generic) */
#define ACU_SS7MON_ISUP_BEGIN     0x01    /* IAM */
#define ACU_SS7MON_ISUP_CONTINUE  0x02    /* Other call-related messages */
#define ACU_SS7MON_ISUP_RELEASE_COMPLETE  0x03 /* RLC */
#define ACU_SS7MON_ISUP_ABORT     0x04    /* monitoring aborted by library */
#define ACU_SS7MON_ISUP_OTHER     0x05    /* Unknown, or not call-related */

/* Generic (ISUP) call states */
#define ACU_SS7MON_STATE_CALLING              0x00
#define ACU_SS7MON_STATE_CONNECTED            0x01
#define ACU_SS7MON_STATE_FORWARD_RELEASED     0x02
#define ACU_SS7MON_STATE_BACKWARD_RELEASED    0x03

/* ISUP Variant definitions, use as the 'cfg' field when enabling the
 * ISUP decoder with ...set_upart_decode */
#define ACU_SS7MON_ISUP_ITU_CFG    acu_ss7mon_isup_get_variant(0)
#define ACU_SS7MON_ISUP_ANSI_CFG   acu_ss7mon_isup_get_variant(1)
#define ACU_SS7MON_ISUP_CHINA_CFG  acu_ss7mon_isup_get_variant(2)

/* Change indicator bits that may appear in cd_altered */
#define ACU_SS7MON_UPDATE_CALLING_NUMBER  0x01
#define ACU_SS7MON_UPDATE_CALLED_NUMBER   0x02
#define ACU_SS7MON_UPDATE_STATE           0x04

/* ISUP Call details struct maintained by the library and visible to user
 */
typedef struct acu_ss7mon_isup_details {
    unsigned int  id_altered;
    int           id_called_ndigits;
    char          *id_called_number;
    int           id_calling_ndigits;
    char          *id_calling_number;
    unsigned int  id_state;
} acu_ss7mon_isup_details_t;

/* This is the mm_upart_info seen for ISUPDATA messages */
typedef struct acu_ss7mon_isup_info {
    unsigned int                ii_cic;
    unsigned int                ii_generic;
    unsigned int                ii_q763_type;
    const struct acu_ss7mon_isup_cfg_variant *ii_variant; /* For library use only */
    void                        *ii_lib_ref;
    void                        *ii_user_ref;
    acu_ss7mon_isup_details_t   *ii_details;
} acu_ss7mon_isup_info_t;

/**** Function definitions *****/

#define ACU_SS7MON_FUNCTIONS(x) \
x(const char *, lib_version, (void)) \
x(acu_ss7mon_ep_t *, create_endpoint, (void)) \
x(int, configure_endpoint, (acu_ss7mon_ep_t *, acu_ss7mon_cfg_param_t, ...)) \
x(void, delete_endpoint, (acu_ss7mon_ep_t *)) \
\
x(int, connect, (acu_ss7mon_ep_t *, const char *)) \
x(socket_t, get_socket, (acu_ss7mon_ep_t *)) \
x(int, get_socket_error, (acu_ss7mon_ep_t *)) \
\
x(int, monitor_link, (acu_ss7mon_ep_t *, unsigned int, unsigned int, \
        unsigned int)) \
x(int, configure_link, (acu_ss7mon_ep_t *, unsigned int, const char *)) \
x(int, unmonitor_link, (acu_ss7mon_ep_t *, unsigned int)) \
x(int, get_msg, (acu_ss7mon_ep_t *, acu_ss7mon_msg_t **, int)) \
x(void, undelete_msg, (acu_ss7mon_ep_t *)) \
x(acu_ss7mon_msg_t *, copy_msg, (acu_ss7mon_msg_t *)) \
x(void, free_msg, (acu_ss7mon_msg_t *)) \
x(const char *, get_dsp_version, (acu_ss7mon_ep_t *)) \
\
x(int, set_pointcode_size, (acu_ss7mon_ep_t *, unsigned int, unsigned int)) \
\
x(int, decode_mtp3, (acu_ss7mon_msg_t *)) \
x(int, decode_isup, (acu_ss7mon_msg_t *, acu_ss7mon_ui_t *)) \
x(int, decode_upart, (acu_ss7mon_msg_t *)) \
x(int, set_upart_decode, (acu_ss7mon_ep_t *, unsigned int, unsigned int, \
        unsigned int, unsigned int, \
        int (*)(acu_ss7mon_msg_t *, acu_ss7mon_ui_t *), const void *)) \
x(void, dump_upart_decode, (acu_ss7mon_ep_t *)) \
\
x(int, isup_find_next_parameter, (acu_ss7mon_msg_t *, int, \
        unsigned char *, unsigned char **, int *)) \
x(int, isup_locate_parameter, (acu_ss7mon_msg_t *, int, \
        unsigned char **, int *)) \
x(int, isup_accept, (acu_ss7mon_isup_lib_ref_t, acu_ss7mon_isup_user_ref_t)) \
x(int, isup_reject, (acu_ss7mon_isup_lib_ref_t)) \
x(const char *, get_error_text, (int)) \
x(void *, isup_get_variant, (int)) \
\
x(int, ioctl_req, (acu_ss7mon_ep_t *, unsigned int, \
        unsigned int, const void *, unsigned int)) \
x(int, connect_card_info, (acu_ss7mon_ep_t *, const ACU_CARD_INFO_PARMS *)) \
x(int, _set_sw, (acu_ss7mon_msg_t *, unsigned int, unsigned int, \
        int (ACU_EXPORT *)(ACU_CARD_ID, OUTPUT_PARMS *))) \
x(void SS7MON_PRINTF_ATTR(5, 6), trace, (acu_ss7mon_ep_t *, unsigned int, \
        const void *, unsigned int, const char *, ...)) \

#define ACU_SS7MTP2_FUNCTIONS(x) \
x(int, est_req, (acu_ss7mon_ep_t *, unsigned int, const char *)) \
x(int, rel_req, (acu_ss7mon_ep_t *, unsigned int)) \
x(int, rtrv_req, (acu_ss7mon_ep_t *, unsigned int, int, int)) \
x(int, state_req, (acu_ss7mon_ep_t *, unsigned int, int)) \
x(int, data_req, (acu_ss7mon_ep_t *, unsigned int, int, const unsigned char *)) \

#ifndef ACU_SS7MON_NO_FN_DEFS
#define ACU_SS7MTP2_FN_DEFS(rslt, name, args) \
        ACU_MONITOR_LIB_API rslt acu_ss7mtp2_##name args;
#define ACU_SS7MON_FN_DEFS(rslt, name, args) \
        ACU_MONITOR_LIB_API rslt acu_ss7mon_##name args;
ACU_SS7MON_FUNCTIONS(ACU_SS7MON_FN_DEFS)
ACU_SS7MTP2_FUNCTIONS(ACU_SS7MTP2_FN_DEFS)
#undef ACU_SS7MON_FN_DEFS
#undef ACU_SS7MTP2_FN_DEFS
#endif

/* Inline wrapper functions.
 * If these are used the application needs to be linked with the
 * aculab resource and switch libraries (res_lib.lib and sw_lib.lib
 * or libacu_res.so and libacu_sw.so).
 * The monitor library is not linked with either library. */

#if !defined(ACU_SS7MON_NO_FN_DEFS) || defined(acu_ss7mon_connect_card_info)
static __inline int
acu_ss7mon_connect_card_id(acu_ss7mon_ep_t *ep, ACU_CARD_ID card_id)
{
    ACU_CARD_INFO_PARMS card_info_parms;

    INIT_ACU_STRUCT(&card_info_parms);
    card_info_parms.card_id = card_id;
    if (acu_get_card_info(&card_info_parms))
        return ACU_SS7MON_ERROR_NO_CARD_INFO;

    return acu_ss7mon_connect_card_info(ep, &card_info_parms);
}
#endif

#if !defined(ACU_SS7MON_NO_FN_DEFS) || defined(acu_ss7mon__set_sw)
/* Set switch matrix after receiving a ACU_SS7MON_MSG_MONITOR_ACK message.
 * NB: This uses the ACU_CARD_ID saved by acu_ss7mon_connect_card_id()
 *     acu_open_switch() must have be called.
 * 'stream' is the TDM switch stream number, TDM port 0 is stream 32.
 * Return value is from sw_set_output().  */
static __inline int
acu_ss7mon_set_sw(acu_ss7mon_msg_t *monitor_ack_msg,
        unsigned int stream, unsigned int timeslot)
{
    return acu_ss7mon__set_sw(monitor_ack_msg, stream, timeslot, sw_set_output);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
