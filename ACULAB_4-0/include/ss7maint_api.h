/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/maintapi/ss7maint_api.h,v 1.61 2024/11/15 09:18:39 davidla Exp $ $Name: SS7_6_17_0 $" */

#ifndef SS7MAINT_API_H
#define SS7MAINT_API_H

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

#ifndef ACU_MAINTAPI_LIB_API
#ifdef _WIN32
#define ACU_MAINTAPI_LIB_API __declspec(dllimport)
#else
#define ACU_MAINTAPI_LIB_API __attribute__((visibility("default")))
#endif
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* Errors: negative integers - Most negative first - Add new codes at the top */

#define ACU_SS7MAINTAPI_ERRORS(x) \
x( ACU_SS7MAINTAPI_ERROR_TOO_LONG,         "Too much data"                   ) \
x( ACU_SS7MAINTAPI_ERROR_UNSPECIFIED,      "Unspecified error"               ) \
x( ACU_SS7MAINTAPI_ERROR_TIMEDOUT,         "Operation timed out"             ) \
x( ACU_SS7MAINTAPI_ERROR_BLOCK_INPROG,     "Un/blocking already in progress" ) \
x( ACU_SS7MAINTAPI_ERROR_RESET_INPROG,     "Reset already in progress"       ) \
x( ACU_SS7MAINTAPI_ERROR_BAD_STATE,        "Incorrect state for request"     ) \
x( ACU_SS7MAINTAPI_ERROR_NOTFOUND,         "Object not found"                ) \
x( ACU_SS7MAINTAPI_ERROR_CONGESTION,       "Failed due to congestion"        ) \
x( ACU_SS7MAINTAPI_ERROR_NOTSUPPORTED,     "Not supported"                   ) \
x( ACU_SS7MAINTAPI_ERROR_TOO_MANY,         "Too many objects for request"    ) \
\
x( ACU_SS7MAINTAPI_ERROR_CONTEXT_INUSE,    "Context already in use"          ) \
x( ACU_SS7MAINTAPI_ERROR_ARGUMENT_RANGE,   "Argument out of range"           ) \
x( ACU_SS7MAINTAPI_ERROR_INVALID_EVENT,    "Invalid event address"           ) \
x( ACU_SS7MAINTAPI_ERROR_INVALID_SEARCH,   "Invalid search address"          ) \
x( ACU_SS7MAINTAPI_ERROR_INVALID_RESULT,   "Invalid result address"          ) \
x( ACU_SS7MAINTAPI_ERROR_INVALID_CONTEXT,  "Invalid context address"         ) \
x( ACU_SS7MAINTAPI_ERROR_NO_DRIVER,        "Unable to open ss7 driver"       ) \
x( ACU_SS7MAINTAPI_ERROR_MALLOC_FAIL,      "malloc() failed"                 ) \
\
/* Non-error errors */ \
x( ACU_SS7MAINTAPI_ERROR_SUCCESS,          "Success") /* Guaranteed to be zero */

#define ACU_SS7MAINTAPI_ERR_ENUM(name, text) name,
#define ACU_SS7MAINTAPI_ERR_COUNT(name, text) -1
typedef enum {
    ACU_SS7MAINTAPI_ERROR_HIGHEST = ACU_SS7MAINTAPI_ERRORS(ACU_SS7MAINTAPI_ERR_COUNT),
    ACU_SS7MAINTAPI_ERRORS(ACU_SS7MAINTAPI_ERR_ENUM)
    ACU_SS7MAINTAPI_ERROR_NUM_ERRMSG = -ACU_SS7MAINTAPI_ERROR_HIGHEST
} acu_ss7maintapi_errors_t;
#undef ACU_SS7MAINTAPI_ERR_ENUM
#undef ACU_SS7MAINTAPI_ERR_COUNT

typedef enum {
    ACU_SS7MAINTAPI_STATS,     /* amr_stats present */
    ACU_SS7MAINTAPI_IPv6       /* ams/amr_ip_addr present */
} acu_ss7maintapi_feature_t;


#define ACU_SS7_STATUS_VAL(tag, text) tag,
#define ACU_SS7_STATUS_V_VAL(tag, val, text) tag = val,

#define ACU_SS7_MTP_LINK_STATUS_LIST(x) \
x(ACU_SS7_MTP_LINK_L3L2_AVAILABLE,    "MTP3 and MTP2 Available")  \
x(ACU_SS7_MTP_LINK_L3_UNAVAILABLE,    "MTP3 Unavailable")         \
x(ACU_SS7_MTP_LINK_L2_UNAVAILABLE,    "MTP2 Unavailable")         \
x(ACU_SS7_MTP_LINK_L3L2_UNAVAILABLE,  "MTP3 and MTP2 Unavailable")

enum acu_ss7_mtp_link_svals {
    ACU_SS7_MTP_LINK_STATUS_LIST(ACU_SS7_STATUS_VAL)
    ACU_SS7_MTP_NUM_LINKSTATUS
};

#define ACU_SS7_MTP_LINK_PROBLEM_LIST(x) \
x(ACU_SS7_MTP_LINK_P_L3_NO_CONFIG,     0x00000001, "MTP3 Link not configured") \
x(ACU_SS7_MTP_LINK_P_L3_UNKNOWN,       0x00000002, "MTP3 State unknown")       \
x(ACU_SS7_MTP_LINK_P_L3_INACTIVE,      0x00000004, "MTP3 Inactive")            \
x(ACU_SS7_MTP_LINK_P_L3_DEACTIVATED,   0x00000008, "MTP3 Manually deactivated")\
x(ACU_SS7_MTP_LINK_P_L3_MTP2_FAIL,     0x00000010, "MTP3 Reports MTP2 failed") \
x(ACU_SS7_MTP_LINK_P_L3_SLT_FAIL,      0x00000020, "MTP3 SLTM-SLTA failure")   \
x(ACU_SS7_MTP_LINK_P_L3_ACTIVATING,    0x00000040, "MTP3 Activating")          \
x(ACU_SS7_MTP_LINK_P_L3_LOC_INHIB,     0x00000080, "MTP3 Locally inhibited")   \
x(ACU_SS7_MTP_LINK_P_L3_REM_INHIB,     0x00000100, "MTP3 Remotely inhibited")  \
x(ACU_SS7_MTP_LINK_P_L3_REM_BLOCKED,   0x00000200, "MTP3 Remote blocked (RPO)")\
x(ACU_SS7_MTP_LINK_P_L2_UNKNOWN,       0x00010000, "MTP2 State unknown")       \
x(ACU_SS7_MTP_LINK_P_L2_PWR_OFF,       0x00020000, "MTP2 Power off")           \
x(ACU_SS7_MTP_LINK_P_L2_OUT_OF_SRV,    0x00040000, "MTP2 Out of service")      \
x(ACU_SS7_MTP_LINK_P_L2_ALIGNED_RDY,   0x00080000, "MTP2 Aligned ready")       \
x(ACU_SS7_MTP_LINK_P_L2_ALIGNED_N_RDY, 0x00100000, "MTP2 Aligned not ready")   \
x(ACU_SS7_MTP_LINK_P_L2_NOT_ALIGNED,   0x00200000, "MTP2 Not aligned")         \
x(ACU_SS7_MTP_LINK_P_L2_ALIGNED,       0x00400000, "MTP2 Aligned")             \
x(ACU_SS7_MTP_LINK_P_L2_PROVING,       0x00800000, "MTP2 Proving")             \
x(ACU_SS7_MTP_LINK_P_L2_LOC_BLOCKED,   0x01000000, "MTP2 Local blocked (LPO)") \
x(ACU_SS7_MTP_LINK_P_L2_REM_BLOCKED,   0x02000000, "MTP2 Remote blocked (RPO)")

enum acu_ss7_mtp_link_pvals {
    ACU_SS7_MTP_LINK_PROBLEM_LIST(ACU_SS7_STATUS_V_VAL)
    ACU_SS7_MTP_LINK_P_NONE = 0
};

#define ACU_SS7_MTP_ROUTE_STATUS_LIST(x) \
x(ACU_SS7_MTP_ROUTE_WORKING,       "Working")                \
x(ACU_SS7_MTP_ROUTE_STANDBY,       "Standby")                \
x(ACU_SS7_MTP_ROUTE_WORKINGRES,    "Working/Restricted")     \
x(ACU_SS7_MTP_ROUTE_STANDBYRES,    "Standby/Restricted")     \
x(ACU_SS7_MTP_ROUTE_PROHIBITED,    "Prohibited")             \
x(ACU_SS7_MTP_ROUTE_UNAVAILABLE,   "Unavailable")            \
x(ACU_SS7_MTP_ROUTE_UNAVAILRES,    "Unavailable/Restricted") \
x(ACU_SS7_MTP_ROUTE_UNAVAILPRO,    "Unavailable/Prohibited") \
x(ACU_SS7_MTP_ROUTE_UNKNOWN,       "Unknown")

enum acu_ss7_mtp_route_svals {
    ACU_SS7_MTP_ROUTE_STATUS_LIST(ACU_SS7_STATUS_VAL)
    ACU_SS7_MTP_NUM_ROUTESTATUS
};

#define ACU_SS7_MTP_ROUTE_PROBLEM_LIST(x) \
x(ACU_SS7_MTP_ROUTE_P_TFR_RECEIVED,  0x0001,    "TFR message received") \
x(ACU_SS7_MTP_ROUTE_P_TFP_RECEIVED,  0x0002,    "TFP message received") \
x(ACU_SS7_MTP_ROUTE_P_LINKSET_FAIL,  0x0004,    "Link set failure")     \
x(ACU_SS7_MTP_ROUTE_P_MTP_RESTART,   0x0008,    "MTP restart")

enum acu_ss7_mtp_route_pvals {
    ACU_SS7_MTP_ROUTE_PROBLEM_LIST(ACU_SS7_STATUS_V_VAL)
    ACU_SS7_MTP_ROUTE_P_NONE = 0
};

#define ACU_SS7_MTP_DEST_STATUS_LIST(x) \
x(ACU_SS7_MTP_DEST_ACCESSIBLE,     "Accessible")      \
x(ACU_SS7_MTP_DEST_RESTRICTED,     "Restricted")      \
x(ACU_SS7_MTP_DEST_INACCESSIBLE,   "Inaccessible")    \
x(ACU_SS7_MTP_DEST_UNKNOWN,        "Unknown")

enum acu_ss7_mtp_dest_svals {
    ACU_SS7_MTP_DEST_STATUS_LIST(ACU_SS7_STATUS_VAL)
    ACU_SS7_MTP_NUM_DESTSTATUS
};

#define ACU_SS7_MTP_DEST_PROBLEM_LIST(x) \
x(ACU_SS7_MTP_DEST_P_OWNSP_RESTART,  0x0001,    "Own SP restart")      \
x(ACU_SS7_MTP_DEST_P_ADJ_RESTART,    0x0002,    "Adjacent SP restart") \
x(ACU_SS7_MTP_DEST_P_ROUTE_RESTR,    0x0004,    "Routes restricted")   \
x(ACU_SS7_MTP_DEST_P_ROUTE_FAILURE,  0x0008,    "Route failure")

enum acu_ss7_mtp_dest_pvals {
    ACU_SS7_MTP_DEST_PROBLEM_LIST(ACU_SS7_STATUS_V_VAL)
    ACU_SS7_MTP_DEST_p_NONE = 0
};

#define ACU_SS7_ISUP_CIC_STATUS_LIST(x) \
x(ACU_SS7_ISUP_CIC_IDLE,            "Idle")            \
x(ACU_SS7_ISUP_CIC_INCOMING,        "Incoming")        \
x(ACU_SS7_ISUP_CIC_OUTGOING,        "Outgoing")        \
x(ACU_SS7_ISUP_CIC_CONNECTED,       "Connected")       \
x(ACU_SS7_ISUP_CIC_RELEASED,        "Released")        \
x(ACU_SS7_ISUP_CIC_FREE,            "Free")            \
x(ACU_SS7_ISUP_CIC_STATE_6,         "Unknown")         \
x(ACU_SS7_ISUP_CIC_UNKNOWN,         "Unknown")         \
x(ACU_SS7_ISUP_CIC_FAIL_IDLE,       "Fail/Idle")       \
x(ACU_SS7_ISUP_CIC_FAIL_INCOMING,   "Fail/Incoming")   \
x(ACU_SS7_ISUP_CIC_FAIL_OUTGOING,   "Fail/Outgoing")   \
x(ACU_SS7_ISUP_CIC_FAIL_CONNECTED,  "Fail/Connected")  \
x(ACU_SS7_ISUP_CIC_FAIL_RELEASED,   "Fail/Released")   \
x(ACU_SS7_ISUP_CIC_FAIL_FREE,       "Fail/Free")       \
x(ACU_SS7_ISUP_CIC_FAIL_STATE_6,    "Fail/Unknown")    \
x(ACU_SS7_ISUP_CIC_FAIL_UNKNOWN,    "Fail/Unknown")    \

enum acu_ss7_isup_cic_svals {
    ACU_SS7_ISUP_CIC_STATUS_LIST(ACU_SS7_STATUS_VAL)
    ACU_SS7_ISUP_NUM_CICSTATUS,
    ACU_SS7_ISUP_CIC_FAIL = ACU_SS7_ISUP_CIC_FAIL_IDLE
};

#define ACU_SS7_ISUP_CIC_PROBLEM_LIST(x) \
x(ACU_SS7_ISUP_CIC_P_LOC_MAINTENANCE, 0x000001, "Local maintenance blocked")  \
x(ACU_SS7_ISUP_CIC_P_LOC_HARDWARE,    0x000002, "Local hardware blocked")     \
x(ACU_SS7_ISUP_CIC_P_REM_MAINTENANCE, 0x000004, "Remote maintenance blocked") \
x(ACU_SS7_ISUP_CIC_P_REM_HARDWARE,    0x000008, "Remote hardware blocked")    \
x(ACU_SS7_ISUP_CIC_P_UCIC,            0x000010, "Unequipped cic (UCIC)")      \
x(ACU_SS7_ISUP_CIC_P_UNBLOCK_BY_RESET,0x000020, "Being unblocked by reset")   \
x(ACU_SS7_ISUP_CIC_P_UNBLOCK_PENDING, 0x000040, "Unblock in progress")        \
x(ACU_SS7_ISUP_CIC_P_BLOCK_PENDING,   0x000080, "Block in progress")          \
x(ACU_SS7_ISUP_CIC_P_INWARD_RESET,    0x004000, "Inward reset in progress")   \
x(ACU_SS7_ISUP_CIC_P_RESET_PENDING,   0x008000, "Reset in progress")          \
x(ACU_SS7_ISUP_CIC_P_ISUP_RESTART,    0x010000, "ISUP Restarting")            \
x(ACU_SS7_ISUP_CIC_P_UP_INACCESSIBLE, 0x020000, "User Part inaccessible")     \
x(ACU_SS7_ISUP_CIC_P_UP_UNEQUIPPED,   0x040000, "User Part unequipped")       \
x(ACU_SS7_ISUP_CIC_P_UP_UNAVAILABLE,  0x080000, "User Part unavailable")      \
x(ACU_SS7_ISUP_CIC_P_MTP_PAUSED,      0x100000, "MTP paused")                 \
x(ACU_SS7_ISUP_CIC_P_MTP_UNAVAILABLE, 0x200000, "Local MTP unavailable")      \

enum acu_ss7_isup_cic_pvals {
    ACU_SS7_ISUP_CIC_PROBLEM_LIST(ACU_SS7_STATUS_V_VAL)
    ACU_SS7_ISUP_CIC_P_NONE = 0
};

/* Standard statistics (indexes into amr_stats[]).
 * Not all these are counted for all entities.
 * If the 'time' is zero it is likely that no statistics counts are kept.
 * 'Failures' are more serious than 'errors'.
 */
#define ACU_SS7_STD_COUNTS_LIST(x) \
x(ACU_SS7_AMRC_TIME,             "Time since all zero (seconds)") \
x(ACU_SS7_AMRC_TX_MSGS,          "Transmit messages") \
x(ACU_SS7_AMRC_TX_BYTES,         "Transmit bytes") \
x(ACU_SS7_AMRC_RETX_MSGS,        "Re-transmit messages") \
x(ACU_SS7_AMRC_RETX_BYTES,       "Re-transmit bytes") \
x(ACU_SS7_AMRC_TX_DISCARDS,      "Transmit discards") \
x(ACU_SS7_AMRC_RX_MSGS,          "Receive messages") \
x(ACU_SS7_AMRC_RX_BYTES,         "Receive bytes") \
x(ACU_SS7_AMRC_RX_DISCARDS,      "Receive discards") \
x(ACU_SS7_AMRC_RX_DISCARD_BYTES, "Receive discard bytes") \
x(ACU_SS7_AMRC_ERRORS,           "Total errors") \
x(ACU_SS7_AMRC_LAST_ERROR,       "Type of last error") /* Zero if unknown */ \
x(ACU_SS7_AMRC_ERR_TIME,         "Time since last error") \
x(ACU_SS7_AMRC_FAILURES,         "Total failures") \
x(ACU_SS7_AMRC_LAST_FAIL,        "Type of last failure") /* Zero if unknown */ \
x(ACU_SS7_AMRC_FAIL_TIME,        "Time since last failure") \

enum acu_ss7_amr_statistics {
    ACU_SS7_STD_COUNTS_LIST(ACU_SS7_STATUS_VAL)
    ACU_SS7_AMRC_COUNT = 32
};

#undef ACU_SS7_STATUS_VAL
#undef ACU_SS7_STATUS_V_VAL

typedef enum {
    ACU_SS7_VARIANT_ITU   = 0,
    ACU_SS7_VARIANT_ANSI  = 1,
    ACU_SS7_VARIANT_CHINA = 2,
    ACU_SS7_VARIANT_JAPAN = 3
} acu_ss7_variant_t;


typedef enum {
    ACU_SS7_MTP_LINK_INHIBIT    = 0x0100,
    ACU_SS7_MTP_LINK_UNINHIBIT,
    ACU_SS7_MTP_LINK_ACTIVATE,
    ACU_SS7_MTP_LINK_DEACTIVATE
} acu_ss7maint_link_action_t;

typedef enum {
    ACU_SS7_ISUP_CIC_BLOCK      = 0x0200,
    ACU_SS7_ISUP_CIC_UNBLOCK,
    ACU_SS7_ISUP_CIC_RESET,
    ACU_SS7_ISUP_CIC_HW_BLOCK,
    ACU_SS7_ISUP_CIC_HW_UNBLOCK
} acu_ss7maint_cic_action_t;

typedef enum {
    ACU_SS7_M3UA_CONNECT        = 0x0300,  /* Connect to remote system */
    ACU_SS7_M3UA_DISCONNECT,               /* Disconnect from remote system */
    ACU_SS7_M3UA_RK_REGISTER,              /* Register route key */
    ACU_SS7_M3UA_RK_DEREGISTER,            /* Deregister route key */
    ACU_SS7_M3UA_RK_ACTIVATE,              /* Activate route key */
    ACU_SS7_M3UA_RK_INACTIVATE             /* Inactivate route key */
} acu_ss7maint_m3ua_action_t;

/* M3UA connection state definition */
#define ACU_M3UA_CON_STATES(x) \
    x(CS_IDLE,                  "idle",                   M3UA_TIMER_NONE) \
    x(CS_LISTEN,                "listen",                 M3UA_TIMER_NONE) \
    x(CS_NEED_CONNECT,          "need_connect",           M3UA_TIMER_CDLY) \
    x(CS_CONNECT_RETRY,         "connect_retry",          M3UA_TIMER_RTRY) \
    x(CS_DISCONNECTING,         "disconnecting",          M3UA_TIMER_NONE) \
    x(CS_CONNECTING,            "connecting",             M3UA_TIMER_CONN) \
    x(CS_DATA_XFER,             "data_xfer",              M3UA_TIMER_CONN) \
    x(CS_WAIT_ASPSM_UP_ACK,     "wait_aspsm_up_ack",      M3UA_TIMER_WACK) \
    x(CS_WAIT_ASPSM_DOWN_ACK,   "wait_aspsm_down_ack",    M3UA_TIMER_WACK) \
    x(CS_ACTIVE,                "active",                 M3UA_TIMER_NONE) \
    /* These are sub-states of CS_ACTIVE */      \
    x(CS_WAIT_MGMT_NOTIFY,      "wait_mgmt_notify",       M3UA_TIMER_WNFY) \
    x(CS_WAIT_RKM_REG_RSP,      "wait_rkm_reg_rsp",       M3UA_TIMER_WACK) \
    x(CS_WAIT_RKM_DEREG_RSP,    "wait_rkm_dereg_rsp",     M3UA_TIMER_WACK) \
    x(CS_WAIT_ASPTM_ACTIVATE_ACK,   "wait_asptm_activate_ack", \
            M3UA_TIMER_WACK) \
    x(CS_WAIT_ASPTM_INACTIVATE_ACK, "wait_asptm_inactivate_ack", \
            M3UA_TIMER_WACK) \


/* Definitions for management type events, generated by the protocol code
 * and queued for reception via the maintenance library.  */

/* Event timestamps are in UTC */
#ifdef _WIN32
/* Seconds between 1601 (windows FILETIME) and 1970 (unix times) */
#define SS7_WIN_TIME_OFFSET ((__int64)116444736 * 100)

#pragma pack (push, 4)
typedef struct acu_ss7maint_time {
    __int64       tv_100nsec;        /* 100 nanosecond units since 1601 */
} acu_ss7maint_time_t;
#pragma pack (pop)
#else
typedef struct acu_ss7maint_time {
    unsigned int  tv_sec;            /* Seconds since 1970 */
    unsigned int  tv_usec;           /* and microseconds */
} acu_ss7maint_time_t;
#endif

typedef enum {
    ACU_SS7MAINTAPI_SYSTEM_ANY     = -1,
    ACU_SS7MAINTAPI_SYSTEM_LOCAL   =  0,
    ACU_SS7MAINTAPI_SYSTEM_DUAL    =  1,
    ACU_SS7MAINTAPI_SYSTEM_DISTRIB =  2    /* Address in ame_system_ip */
    /* May also be IPv4 address in host order */
} acu_ss7maint_system_t;

/* Event queued to user */
struct acu_ss7maintapi_event {
  /* Event identification, only one event for each will be queued to a user */
    int                 ame_system;       /* System that sourced event */
    int                 ame_localpc;      /* Local pointcode */
    int                 ame_localpc_inst; /* Local pc instance (usually 0) */
    int                 ame_server_type;  /* MTP3/ISUP etc */
    unsigned int        ame_event_bit;    /* Event being queued */
    int                 ame_remotepc;     /* Destination or adjacent pc */
    int                 ame_item;         /* CIC, slc, pc etc */
    int                 ame_item_1;
  /* Per event information (last copy kept) */
    acu_ss7maint_time_t ame_epoch;        /* Time when stats initialised */
    acu_ss7maint_time_t ame_now;          /* Time when this record generated */
    unsigned int        ame_number;       /* Events generated since ame_epoch */
    unsigned int        ame_state;        /* Current state of monitored item */
    unsigned int        ame_info;
    unsigned int        ame_info_1;
    unsigned int        ame_pad[8];
  /* Additional id fields */
    struct sockaddr_in6 ame_system_ip;    /* Source if distrib isup */
    struct sockaddr_in6 ame_remote_ip;    /* Remote if M3UA */
};
#define ame_ip_addr ame_remotepc          /* Remote IPv4 host address (M3UA) */
#undef acu_ss7maintapi_event
typedef struct acu_ss7maintapi_event acu_ss7maintapi_event_t;

/* Server types */
#define ACU_SS7MAINTAPI_SERVER_ANY      (-1)
#define ACU_SS7MAINTAPI_SERVER_MTP3       1
#define ACU_SS7MAINTAPI_SERVER_ISUP       2
#define ACU_SS7MAINTAPI_SERVER_DUAL_LINK  3
#define ACU_SS7MAINTAPI_SERVER_SCCP       4
#define ACU_SS7MAINTAPI_SERVER_TCAP       5
#define ACU_SS7MAINTAPI_SERVER_M3UA       6
#define ACU_SS7MAINTAPI_SERVER_LAST       6

/* Global event (all servers) */
#define ACU_SS7_EV_SERVER_STATE       0x1
/* State values for ACU_SS7_EV_SERVER_STATE */
#define ACU_SS7_EVS_SERVER_DELETE     0
#define ACU_SS7_EVS_SERVER_CREATE     1
#define ACU_SS7_EVS_SERVER_CONFIG     2      /* Not generated by all servers */

/* MTP3 events */
#define ACU_SS7_EV_MTP3_DEST_STATE    0x0002
#define ACU_SS7_EV_MTP3_ROUTE_STATE   0x0004
#define ACU_SS7_EV_MTP3_LINK_STATE    0x0008

/* ISUP events */
#define ACU_SS7_EV_ISUP_CIC_REGISTER  0x0002  /* CIC added or removed */
#define ACU_SS7_EV_ISUP_CIC_STATE     0x0004  /* Call state changes */
#define ACU_SS7_EV_ISUP_CIC_BLOCK     0x0008  /* Blocking status change */
#define ACU_SS7_EV_ISUP_CIC_RESET     0x0010  /* Reset state change */

/* State values for ACU_SS7_EV_ISUP_CIC_REGISTER */
#define ACU_SS7_EVS_CIC_REG_LOCAL     0x0001  /* Registered with local MTP3 */
#define ACU_SS7_EVS_CIC_REG_DUAL      0x0002  /* Reg. with dual system */
#define ACU_SS7_EVS_CIC_REG_HOST_A    0x0010  /* Distributed ISUP reg. host A */
#define ACU_SS7_EVS_CIC_REG_HOST_B    0x0020  /* .. and MTP3 host B */
#define ACU_SS7_EVS_CIC_REG_DISTRIB   0x0100  /* MTP3 system, CIC is distrbtd */
#define ACU_SS7_EVS_CIC_REG_AWOL      0x0200  /* TCP/IP connection closed */

/* TCAP events */
#define ACU_SS7_EV_TCAP_REGISTER      0x0002

/* State values for ACU_SS7_EV_TCAP_REGISTER (bit significant) */
#define ACU_SS7_EVS_TCAP_ANSI         0x0001  /* App is ANSI TCAP */
#define ACU_SS7_EVS_TCAP_REGISTERED   0x0002  /* Application is registered */
#define ACU_SS7_EVS_TCAP_ALT_REG      0x0008  /* Another app is registered */
#define ACU_SS7_EVS_TCAP_SERVER       0x0010  /* App set ACU_TCAP_SERVER */
#define ACU_SS7_EVS_TCAP_UNI_SERVER   0x0020  /* App set ACU_TCAP_UNI_SERVER */
#define ACU_SS7_EVS_TCAP_ALT_SVR      0x0040  /* Another app set TCAP_SERVER */
#define ACU_SS7_EVS_TCAP_ALT_UNI_SVR  0x0080  /* Ano app set TCAP_UNI_SERVER */
#define ACU_SS7_EVS_TCAP_STATUS_IND   0x1000  /* App requested status inds */
#define ACU_SS7_EVS_TCAP_RX_FLOW_OFF  0x2000  /* App not acking rx data */

/* M3UA events */
#define ACU_SS7_EV_M3UA_RK_STATE_CHANGE 0x0002
#define ACU_SS7_EV_M3UA_CON_ROUTE_CTX   0x0004

/* State values for ACU_SS7_EV_M3UA_RK_STATE_CHANGE (values match RFC4666) */
#define ACU_SS7_M3UA_AS_INACTIVE      0x00010002
#define ACU_SS7_M3UA_AS_ACTIVE        0x00010003
#define ACU_SS7_M3UA_AS_PENDING       0x00010004
#define ACU_SS7_M3UA_INSUFFICIENT_ASP 0x00020001
#define ACU_SS7_M3UA_ALT_ASP_ACTIVE   0x00020002
#define ACU_SS7_M3UA_ASP_FAILURE      0x00020003

/* State values for ACU_SS7_EV_M3UA_CON_ROUTE_CTX */
#define ACU_SS7_M3UA_CRK_DISCONNECTED      0
#define ACU_SS7_M3UA_CRK_IDLE              1  /* Connected but not registered */
#define ACU_SS7_M3UA_CRK_REGISTERING       2
#define ACU_SS7_M3UA_CRK_REGISTERED        3
#define ACU_SS7_M3UA_CRK_ACTIVATING        4
#define ACU_SS7_M3UA_CRK_ACTIVATED         5  /* But not receiving traffic */
#define ACU_SS7_M3UA_CRK_ACTIVE            6  /* NOTIFY(AS_ACTIVE) received */
#define ACU_SS7_M3UA_CRK_INACTIVATING      7
#define ACU_SS7_M3UA_CRK_DEREGISTERING     8
#define ACU_SS7_M3UA_CRK_UNAVAILABLE       9  /* Active but not available */


/* Private structures */
typedef struct acu_ss7maintapi_ctx acu_ss7maintapi_ctx_t;


#ifdef _WIN32
typedef void *acu_ss7maintapi_os_event_t;
#else
typedef int acu_ss7maintapi_os_event_t;
#endif

struct acu_ss7maintapi_search {
    int ams_next;
    int ams_system;
    int ams_localpc;
    int ams_localpc_inst;
    int ams_remotepc;
    int ams_adjacentpc;
    int ams_slc;
    int ams_cic;
    int ams_cic_lim;
    char ams_name[16];
    struct sockaddr_in6 ams_sockaddr;
/* TCAP and M3UA status requests rename some of the above */
#define ams_ssn ams_slc
#define ams_tran_id ams_cic
#define ams_ip_addr ams_adjacentpc
#define ams_route_ctx ams_slc
};
#undef  acu_ss7maintapi_search

typedef struct acu_ss7maintapi_search acu_ss7maintapi_search_t;

struct acu_ss7maintapi_result {
    int amr_system;
    int amr_localpc;
    int amr_localpc_inst;
    int amr_remotepc;
    int amr_adjacentpc;
    int amr_slc;
    int amr_cic;
    int amr_priority;
    int amr_trunk;
    int amr_ts;
    int amr_variant;
    int amr_state;
    unsigned int amr_reason;
    char amr_str_state[32];
    char amr_str_reason[16][32];
    char amr_serial[16];
    unsigned int amr_stats[ACU_SS7_AMRC_COUNT];   /* Statistics counts */
    struct sockaddr_in6 amr_sockaddr;
    int  amr_sockaddr_len;       /* Likely column width (excl port) */
/* Some of the above are renamed for some entities */
#define amr_ssn             amr_slc                /* TCAP results */
#define amr_tran_id         amr_cic                /* TCAP results */
#define amr_name            amr_serial             /* M3UA results */
#define amr_ip_addr         amr_adjacentpc         /* M3UA results */
#define amr_route_ctx       amr_slc                /* M3UA results */
#define amr_si_mask         amr_ts                 /* M3UA results */
#define amr_cic_registered  amr_adjacentpc         /* ISUP results */
};
#undef  acu_ss7maintapi_result

typedef struct acu_ss7maintapi_result acu_ss7maintapi_result_t;

/* Raw access to MTP3
 * (Uses the same interface to the ss7 driver as the maintenance API
 * and is included in the same library.) */
#define ACU_SS7MAINTAPI_NDELAY    (1<<0)
#define ACU_SS7MAINTAPI_TX_ONLY   (1<<5)
#define ACU_SS7MAINTAPI_TO_DUAL   (1<<16)
#define ACU_SS7MAINTAPI_FROM_DUAL (1<<17)
#define ACU_SS7MAINTAPI_ADOPT_NI  0x7d
#define ACU_SS7MAINTAPI_WILD_NI   0x7e

typedef struct acu_ss7maintapi_mtp3_attach_info {
    int  am3a_localpc;
    int  am3a_localpc_inst;
    int  am3a_ni;
    int  am3a_si;
    int  am3a_pad1[4];
    int  am3a_variant;
    int  am3a_maxsu;
    int  am3a_pad2[6];
} acu_ss7maintapi_mtp3_attach_info_t;

typedef struct acu_ss7maintapi_mtp3_send_info {
    int  am3s_remotepc;
    int  am3s_sls;
    int  am3s_msg_pri;
    int  am3s_pad[5];
} acu_ss7maintapi_mtp3_send_info_t;

typedef enum {
    ACU_SS7_MTP3_IND_DATA,
    ACU_SS7_MTP3_IND_PAUSE,
    ACU_SS7_MTP3_IND_RESUME,
    ACU_SS7_MTP3_IND_CONGESTION,
    ACU_SS7_MTP3_IND_STATUS
} acu_ss7maint_mtp3_ind_t;

typedef struct acu_ss7maintapi_mtp3_recv_info {
    acu_ss7maint_mtp3_ind_t  am3r_indication;
    int  am3r_remotepc;
    int  am3r_adjacentpc;   /* Zero if unavailable */
    int  am3r_ni;
    int  am3r_sls;
    int  am3r_msg_pri;
    int  am3r_msg_len;
    int  am3r_cong_lvl;     /* for ACU_SS7_MTP3_IND_CONGESTION */
    int  am3r_up_cause;     /* for ACU_SS7_MTP3_IND_STATUS */
    int  am3r_pad[8];
} acu_ss7maintapi_mtp3_recv_info_t;

#ifdef __GNUC__
#define ACU_SS7MAINTAPI_PRINTF_ATTR(F, L) __attribute__((format(printf, F, L)))
#else
#define ACU_SS7MAINTAPI_PRINTF_ATTR(F, L)
#endif

/*
 * The SS7MAINTAPI interfaces are defined using this pre-processor 'magic' in
 * order to make it possible to define other data structures using the same
 * information.
 * In particular it can be used to define the data areas needed to dynamically
 * load the SS7MAINTAPI library.
 */

#define ACU_SS7MAINTAPI_FUNCTIONS(x) \
    x(const char *, acu_ss7maintapi_lib_version,  (acu_ss7maintapi_ctx_t *)) \
    x(const char *, acu_ss7maintapi_drv_version,  (acu_ss7maintapi_ctx_t *)) \
    x(const char *, acu_ss7maintapi_strerror,  (int, unsigned int)) \
\
    x(int,  acu_ss7maintapi_open,             (acu_ss7maintapi_ctx_t **))    \
    x(void, acu_ss7maintapi_close,            (acu_ss7maintapi_ctx_t *))     \
    x(void, acu_ss7_maintapi_pc_width,        (acu_ss7maintapi_ctx_t *,  \
                int *, int *))     \
    x(int,  acu_ss7maintapi_search_alloc,     (acu_ss7maintapi_search_t **)) \
    x(void, acu_ss7maintapi_search_free,      (acu_ss7maintapi_search_t *))  \
    x(int,  acu_ss7maintapi_result_alloc,     (acu_ss7maintapi_result_t **)) \
    x(void, acu_ss7maintapi_result_free,      (acu_ss7maintapi_result_t *))  \
\
    x(int,  acu_ss7maintapi_status_mtp_link,  (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_mtp_route, (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_mtp_dest,  (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_isup_cic,  (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_m3ua_connection, (acu_ss7maintapi_ctx_t *, \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_m3ua_address, (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_tcap_application, (acu_ss7maintapi_ctx_t *, \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
\
    x(int,  acu_ss7maintapi_mtp_link,         (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maint_link_action_t)) \
    x(int,  acu_ss7maintapi_isup_cic,         (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maint_cic_action_t))  \
    x(int,  acu_ss7maintapi_m3ua,             (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maint_m3ua_action_t)) \
\
    x(int,  acu_ss7maintapi_event_register,   (acu_ss7maintapi_ctx_t *, \
                int, int, int, unsigned int)) \
    x(int,  acu_ss7maintapi_event_deregister, (acu_ss7maintapi_ctx_t *, \
                int, int, int, unsigned int)) \
    x(int,  acu_ss7maintapi_event_alloc,      (acu_ss7maintapi_event_t **)) \
    x(void, acu_ss7maintapi_event_free,       (acu_ss7maintapi_event_t *))  \
    x(int,  acu_ss7maintapi_event_get,        (acu_ss7maintapi_ctx_t *, \
                acu_ss7maintapi_event_t *, int))  \
    x(int,  acu_ss7maintapi_event_get_os_event,(acu_ss7maintapi_ctx_t *, \
                acu_ss7maintapi_os_event_t *)) \
\
    x(int, acu_ss7maintapi_mtp3_attach,       (acu_ss7maintapi_ctx_t *, \
                int, acu_ss7maintapi_mtp3_attach_info_t *)) \
    x(void, acu_ss7maintapi_mtp3_detach,      (acu_ss7maintapi_ctx_t *)) \
    x(int, acu_ss7maintapi_mtp3_send,         (acu_ss7maintapi_ctx_t *, \
                int, const acu_ss7maintapi_mtp3_send_info_t *, \
                const void *, int)) \
    x(int, acu_ss7maintapi_mtp3_recv,         (acu_ss7maintapi_ctx_t *, \
                int, acu_ss7maintapi_mtp3_recv_info_t *, void *, int)) \
\
    x(int,  acu_ss7maintapi_status_m3ua_route_key, (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(int,  acu_ss7maintapi_status_m3ua_con_stats, (acu_ss7maintapi_ctx_t *,  \
                acu_ss7maintapi_search_t *, acu_ss7maintapi_result_t *)) \
    x(unsigned int, acu_ss7maintapi_feature_check, \
                (acu_ss7maintapi_feature_t)) \

/* Define C external function definitions */
#ifndef ACU_SS7MAINTAPI_NO_FN_DEFS    /* Define to suppress these definitions */
#define ACU_SS7MAINTAPI_FN_DEFS(rslt, name, args) \
        ACU_MAINTAPI_LIB_API rslt name args;
ACU_SS7MAINTAPI_FUNCTIONS(ACU_SS7MAINTAPI_FN_DEFS)
#undef ACU_SS7MAINTAPI_FN_DEFS
#endif

/* Define for C function variable externs */
#define ACU_SS7MAINTAPI_EXTERN_FN_DEFS_(rslt, name, args) \
        extern rslt (*name) args;
#define ACU_SS7MAINTAPI_EXTERN_FN_DEFS \
        ACU_SS7MAINTAPI_FUNCTIONS(ACU_SS7MAINTAPI_EXTERN_FN_DEFS_)

/* Define for C function variables */
#define ACU_SS7MAINTAPI_FN_DEFS_(rslt, name, args) rslt (*name) args = 0;
#define ACU_SS7MAINTAPI_FN_DEFS \
        ACU_SS7MAINTAPI_FUNCTIONS(ACU_SS7MAINTAPI_FN_DEFS_)

/* Code fragment to initialise function pointers from library handle/file */
#define ACU_SS7MAINTAPI_FN_INIT(fd, err_fn) \
{ \
    typedef struct { const char *fn_name; void **fn_addr; } fni_t; \
    static const fni_t fn_info[] = \
            {ACU_SS7MAINTAPI_FUNCTIONS(ACU_SS7MAINTAPI_FN_TABLE) {0}};\
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
#define ACU_SS7MAINTAPI_FN_TABLE(rval, name, arg) {#name, (void **)(void *)&name},
#ifdef _WIN32
#define dlsym(fd, fn) GetProcAddress(fd, fn)
#endif

#ifdef __cplusplus
}
#endif

#endif
