/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/sccp_lib/sccp_api.h,v 1.26 2024/11/15 09:18:39 davidla Exp $ $Name: SS7_6_17_0 $" */

#ifndef SCCP_API_H
#define SCCP_API_H

/* SCCP API - external interfaces */

#include <stdarg.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

#ifndef ACU_SCCP_LIB_API
#ifdef _WIN32
#define ACU_SCCP_LIB_API __declspec(dllimport)
#else
#define ACU_SCCP_LIB_API __attribute__((visibility("default")))
#endif
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* Errors: negative integers - Most negative first - Add new codes at the top */

#define ACU_SCCP_ERRORS(x) \
\
x( ACU_SCCP_ERROR_CONNECTION_IDLE,                       \
        "Connection (or timer) is idle")                 \
x( ACU_SCCP_ERROR_CONNECTION_OUTSTATE,                   \
        "Connection (or timer) in incorrect state")      \
x( ACU_SCCP_ERROR_BAD_CONNECTION_STATE,                  \
        "Connection state incorrect for this request")   \
\
/* User errors */ \
x( ACU_SCCP_ERROR_CANNOT_OPEN_CONFIG_FILE,               \
        "Configuration file cannot be found")            \
x( ACU_SCCP_ERROR_INVALID_CONFIG_VALUE,                  \
        "Configuration parameter value invalid")         \
x( ACU_SCCP_ERROR_UNKNOWN_CONFIG_PARAM,                  \
        "Configuration parameter not known")             \
x( ACU_SCCP_ERROR_NO_LOCAL_POINTCODE,                    \
        "Local pointcode not configured")                \
x( ACU_SCCP_ERROR_NO_LOCAL_SSN,                          \
        "Local SSN not configured")                      \
x( ACU_SCCP_ERROR_ALREADY_CONNECTED,                     \
        "Already connected to SCCP")                     \
x( ACU_SCCP_ERROR_WRONG_MSG_TYPE,                        \
        "Message type incorrect for this function")      \
x( ACU_SCCP_ERROR_BAD_OS_EVENT,                          \
        "Invalid OS event structure")                    \
x( ACU_SCCP_ERROR_BAD_EVENT,                             \
        "Invalid event address")                         \
x( ACU_SCCP_ERROR_BAD_MESSAGE,                           \
        "Invalid message address")                       \
x( ACU_SCCP_ERROR_BAD_SSAP,                              \
        "Invalid sccp sap address")                      \
x( ACU_SCCP_ERROR_BAD_CONNECTION,                        \
        "Invalid sccp connection address")               \
x( ACU_SCCP_ERROR_NO_THREADS,                            \
        "Thread creation failed")                        \
x( ACU_SCCP_ERROR_MALLOC_FAIL,                           \
        "malloc() failed")                               \
x( ACU_SCCP_ERROR_not_used_2,                            \
        "error code not used")                           \
\
/* Non-error errors */ \
x( ACU_SCCP_ERROR_NO_INFORMATION_AVAILABLE,              \
        "Requested information not avaialble")           \
x( ACU_SCCP_ERROR_NO_MESSAGE,                            \
        "No message on queue")                           \
x( ACU_SCCP_ERROR_TIMEDOUT,                              \
        "Timeout expired")                               \
x( ACU_SCCP_ERROR_not_used_1,                            \
        "error code not used")                           \
\
x( ACU_SCCP_ERROR_SUCCESS,                               \
        "Success") /* Guaranteed to be zero */

#define ACU_SCCP_ERR_ENUM(name, text) name,
#define ACU_SCCP_ERR_COUNT(name, text) -1
typedef enum {
    ACU_SCCP_ERROR_HIGHEST = ACU_SCCP_ERRORS(ACU_SCCP_ERR_COUNT),
    ACU_SCCP_ERRORS(ACU_SCCP_ERR_ENUM)
    ACU_SCCP_ERROR_NUM_ERRMSG = -ACU_SCCP_ERROR_HIGHEST
} acu_sccp_errors_t;
#undef ACU_SCCP_ERR_ENUM
#undef ACU_SCCP_ERR_COUNT

/* Overall types of SCCP messages */
#define ACU_SCCP_MSG_INTERNAL(msg_type) ((msg_type) << 8)
typedef enum {
  /* Overall message types - set by acu_sccp_xxx_msg_get() */
    ACU_SCCP_MSG_UNITDATA       = ACU_SCCP_MSG_INTERNAL(1),
    ACU_SCCP_MSG_NOTICE         = ACU_SCCP_MSG_INTERNAL(2),
    ACU_SCCP_MSG_TIMEOUT        = ACU_SCCP_MSG_INTERNAL(4),
    ACU_SCCP_MSG_CON_STATE      = ACU_SCCP_MSG_INTERNAL(5),
    ACU_SCCP_MSG_USER_STATUS    = ACU_SCCP_MSG_INTERNAL(6),
    ACU_SCCP_MSG_SP_STATUS      = ACU_SCCP_MSG_INTERNAL(7),
    ACU_SCCP_MSG_DATA_IND       = ACU_SCCP_MSG_INTERNAL(8),
    ACU_SCCP_MSG_CONNECT_IND    = ACU_SCCP_MSG_INTERNAL(9),
    ACU_SCCP_MSG_CONNECT_CONF   = ACU_SCCP_MSG_INTERNAL(10),
    ACU_SCCP_MSG_CONREF_CONF    = ACU_SCCP_MSG_INTERNAL(11),
    ACU_SCCP_MSG_DISCON_IND     = ACU_SCCP_MSG_INTERNAL(12),
    ACU_SCCP_MSG_DISCON_CONF    = ACU_SCCP_MSG_INTERNAL(13),
    ACU_SCCP_MSG_ERROR_IND      = ACU_SCCP_MSG_INTERNAL(14),
    ACU_SCCP_MSG_INACTIVITY_IND = ACU_SCCP_MSG_INTERNAL(15)
} acu_sccp_msg_type_t;

#undef ACU_SCCP_MSG_INTERNAL


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
#define ACU_SCCP_CFG_SCCP_ADDR(NAME, name, val, max, type) \
        ACU_SCCP_CFG_LOCAL_##NAME = 0x40 + (val), \
        ACU_SCCP_CFG_CLEAR_LOCAL_##NAME = 0x50 + (val), \
        ACU_SCCP_CFG_REMOTE_##NAME = 0x60 + (val), \
        ACU_SCCP_CFG_CLEAR_REMOTE_##NAME = 0x70 + (val), \

/* Configurable parameters for acu_sccp_ssap/tran_set_cfg() */
typedef enum {
  /* SSAP parameters */
    ACU_SCCP_CFG_LOGFILE,             /* Name of logfile */
    ACU_SCCP_CFG_TRACE_TAG,           /* Name for trace entries for this ssap */
    ACU_SCCP_CFG_LOGFILE_MAX_SIZE,    /* Maximum size of logfile */
    ACU_SCCP_CFG_LOGFILE_OLD_KEPT,    /* Number of old logfiles kept */
    ACU_SCCP_CFG_LOGFILE_APPEND,      /* Append to logfile */
    ACU_SCCP_CFG_TRACE_MODE,          /* 0 => line, 1 => block, 2 => cyclic */
    ACU_SCCP_CFG_TRACE_BUFFER_SIZE,   /* Buffer size for block/cyclic trace */
    ACU_SCCP_CFG_LOGFILE_FLOCK_INDEX, /* lockf() logfile.index */
    ACU_SCCP_CFG_CO_SERVER    = 0x10, /* Set to receive CO messages */
    ACU_SCCP_CFG_CL_SERVER,           /* Set to receive CL msgs */
    ACU_SCCP_CFG_CONID_RANGE = 0x18,  /* Top 12 bits of connection IDs */
    ACU_SCCP_CFG_SINGLE_THREADED = 0x20, /* Do not block connections */
    ACU_SCCP_CFG_HOST_A_NAME  = 0x30, /* IPv4 address (string) */
    ACU_SCCP_CFG_HOST_A_PORT,         /* TCP port number */
    ACU_SCCP_CFG_HOST_A_PASSWORD,     /* Password for TCP connection */
    ACU_SCCP_CFG_HOST_B_NAME  = 0x34, /* IPv4 address (string) */
    ACU_SCCP_CFG_HOST_B_PORT,         /* TCP port number */
    ACU_SCCP_CFG_HOST_B_PASSWORD,     /* Password for TCP connection */
    ACU_SCCP_CFG_RX_BUFLEN    = 0x38, /* TCP rx buffer size */
    ACU_SCCP_CFG_KEEPALIVE_TIMEOUT,   /* Interval between keepalives */
    ACU_SCCP_CFG_TX_QUEUE_LEN,        /* Buffers in TX queue before flow ctl */
    ACU_SCCP_CFG_CONNECT_TIMEOUT,     /* Connection (to SCCP) timeout */
    ACU_SCCP_CFG_TX_BYTE_WINDOW,      /* Byte limit for ack from driver */
  /* 0x40-0x5f for local address and 0x60-0x7f for remote address */
    ACU_SCCP_ADDR_FIELD(ACU_SCCP_CFG_SCCP_ADDR)
    ACU_SCCP_CFG_LOCAL_FLAGS  = 0x48,
    ACU_SCCP_CFG_LOCAL_GT_DIGITS,
    ACU_SCCP_CFG_NI           = 0x4f, /* Network Indicator */
    ACU_SCCP_CFG_REMOTE_FLAGS= 0x68,
    ACU_SCCP_CFG_REMOTE_GT_DIGITS,
  /* These values set on the SSAP will be inherited by all connections */
    ACU_SCCP_CFG_QOS_RET_OPT  = 0x80, /* Request SCCP set 'return on error' */
    ACU_SCCP_CFG_QOS_SEQ_CTRL,        /* Send all messages on same SLS */
    ACU_SCCP_CFG_QOS_PRIORITY,        /* Message priority 0..7 (or ~0u) */
    ACU_SCCP_CFG_NU_83,
    ACU_SCCP_CFG_NU_84,
    ACU_SCCP_CFG_QOS_RESPONSE_PRI,    /* Message priority for responses */
    ACU_SCCP_CFG_NU_86,
    ACU_SCCP_CFG_NU_87,
    ACU_SCCP_CFG_NU_88,
    ACU_SCCP_CFG_NU_89,
    ACU_SCCP_CFG_STICKY_CON,          /* Non-zero: Send all data one host */
  /* Protocol timers, SSAP gets defaults from the driver, overwritable
   * on a per-connection basis. */
    ACU_SCCP_CFG_T_CONN_EST   = 0x90, /* T(Conn est) */
    ACU_SCCP_CFG_T_IAS,               /* T(ias) - delay before send IT */
    ACU_SCCP_CFG_T_IAR,               /* T(iar) - rx idle before disconnect */
    ACU_SCCP_CFG_T_REL,               /* T(rel) - release msg timeout */
    ACU_SCCP_CFG_T_REPEAT_REL,        /* T(repeat rel) - release retx tmo */
    ACU_SCCP_CFG_T_INT,               /* T(int) - release retransmit limit */
  /* Trace levels for each source - currently there are 64 sources */
    ACU_SCCP_CFG_TRACE_LEVEL_ALL = 0xff,
    ACU_SCCP_CFG_TRACE_LEVEL_0 = 0x100
#define ACU_SCCP_CFG_TRACE_LEVEL(source) (ACU_SCCP_CFG_TRACE_LEVEL_0 + (source))
} acu_sccp_cfg_param_t;
#undef ACU_SCCP_CFG_SCCP_ADDR

/* ssap flags */
typedef enum {
    ACU_SCCP_CO_SERVER  = 0x00010,   /* Set to receive CR messages */
    ACU_SCCP_CL_SERVER  = 0x00020,   /* Set to receive connectionless msgs */
    ACU_SCCP_LOG_APPEND = 0x00040,   /* Append to log file */
    ACU_SCCP_LOG_NOLOCK = 0x00080,   /* Rotate logfile without lockf(),
                                      * inverted from CFG_LOGFILE_FLOCK_INDEX */
    ACU_SCCP_LOG_STDERR = 0x00100,   /* Write early errors to stderr */
    ACU_SCCP_STATUS_IND = 0x01000    /* Set to receive raw SCCP status msgs */
} acu_sccp_ssap_flags_t;

/* Trace flags */
enum {
  /* Low bits identify trace source */
    ACU_SCCP_TRACE_APPLICATION_0,           /* 0..9 for application use */
#define ACU_SCCP_TRACE_APPLICATION(n) ((n) & ACU_SCCP_TRACE_MASK)
    ACU_SCCP_TRACE_API_ENTRY = 0x10,        /* Entry to API routine */
    ACU_SCCP_TRACE_API_EXIT,                /* Normal exit from same */
    ACU_SCCP_TRACE_API_ERROR,               /* Error exit */
    ACU_SCCP_TRACE_API_EVENT,               /* API event (etc) */
    ACU_SCCP_TRACE_API_INFO,                /* Additional information */
    ACU_SCCP_TRACE_API_CONFIG,              /* Config change */
    ACU_SCCP_TRACE_API_OP_TIMER,            /* Operation timers */
    ACU_SCCP_TRACE_ENCODER = 0x20,          /* SCCP message being sent */
    ACU_SCCP_TRACE_DECODER,                 /* Received SCCP message */
    ACU_SCCP_TRACE_SCCP_MSG,                /* General message procesing */
    ACU_SCCP_TRACE_STATUS,                  /* Pointcode and user status */
    ACU_SCCP_TRACE_QUEUE_ERROR = 0x28,      /* Message queue errors */
    ACU_SCCP_TRACE_MUTEX_ERROR,             /* Mutex lock/unlock erors */
    ACU_SCCP_TRACE_TCP       = 0x30,        /* TCP control */
    ACU_SCCP_TRACE_TCP_SEND,                /* TCP message being sent */
    ACU_SCCP_TRACE_TCP_RECV,                /* Received TCP message */
    ACU_SCCP_TRACE_MASK = 0x3f,             /* Last entry we keep level for */

    ACU_SCCP_TRACE_FMT_SIGTRAN = 0x100,     /* Trace data is TCP SIGTRAN */

    ACU_SCCP_TRACE_LEVEL_1 = 0x1000,        /* 16 trace levels per source */
#define ACU_SCCP_TRACE_LEVEL(n)      (ACU_SCCP_TRACE_LEVEL_1 * ((n) & 15))

    ACU_SCCP_TRACE_ALWAYS    = 1u << 30,    /* => always trace */
    ACU_SCCP_TRACE_FIRST     = 1u << 29,    /* => acquire lock and add header */
    ACU_SCCP_TRACE_LAST      = 1u << 28,    /* => release lock */
    ACU_SCCP_TRACE_FLUSH     = 1u << 27,    /* => write buffered trace */
    ACU_SCCP_TRACE_MIDDLE    = 0,
    ACU_SCCP_TRACE_ONLY      = ACU_SCCP_TRACE_FIRST | ACU_SCCP_TRACE_LAST
};

/* Define for building 'flags' parameter to trace functions */
#define ACU_SCCP_TRF(part, src, lvl) \
    (ACU_SCCP_TRACE_##part | ACU_SCCP_TRACE_##src | ACU_SCCP_TRACE_LEVEL(lvl))

/* Trace mode constants */
#define ACU_SCCP_TRACE_MODE_LINE    0       /* Write every entry */
#define ACU_SCCP_TRACE_MODE_BLOCK   1       /* Write when buffer full */
#define ACU_SCCP_TRACE_MODE_CYCLIC  2       /* Write on request only */

/* Private structures */
typedef struct acu_sccp_ssap acu_sccp_ssap_t;
typedef struct acu_sccp_connection acu_sccp_con_t;
typedef struct acu_sccp_event acu_sccp_event_t;

/* State of the TCP connection to SCCP */
typedef enum {
    ACU_SCCP_CON_STATE_IDLE,
    ACU_SCCP_CON_STATE_CONNECTING,
    ACU_SCCP_CON_STATE_CONNECTED,
    ACU_SCCP_CON_STATE_IN_SERVICE,
  /* If IN_SERVICE the following bits can be set */
    ACU_SCCP_CON_STATE_RX_BLOCKED = 0x010000,
    ACU_SCCP_CON_STATE_RX_FLOW    = 0x020000,
    ACU_SCCP_CON_STATE_TX_BLOCKED = 0x100000,
    ACU_SCCP_CON_STATE_TX_FLOW    = 0x200000
} acu_sccp_cs_state_t;

/* Overall reason why the connection to the SS7 driver failed */
#define ACU_SCCP_CON_FAIL_INFO(x) \
x(ACU_SCCP_CON_FAIL_SSAP_DELETED,    "ssap deleted") \
x(ACU_SCCP_CON_FAIL_CON_TIMEOUT,     "TCP/IP connect request timed out") \
x(ACU_SCCP_CON_FAIL_CON_REJECTED,    "TCP/IP connect rejected") \
x(ACU_SCCP_CON_FAIL_LOGIN_REJECTED,  "") /* Login sequence failed */ \
x(ACU_SCCP_CON_FAIL_INWARD,          "inward disconnected") \
x(ACU_SCCP_CON_FAIL_KEEPALIVE,       "keepalive timed out") \
x(ACU_SCCP_CON_FAIL_REGISTRATION,    "registration with SCCP failed") \
x(ACU_SCCP_CON_FAIL_BAD_MESSAGE,     "corrupt message received from SCCP") \

#define ACU_SCCP_CON_FAIL_INFO_ENUM(tag, text) tag,
typedef enum { ACU_SCCP_CON_FAIL_INFO(ACU_SCCP_CON_FAIL_INFO_ENUM)
    ACU_SCCP_CON_FAIL_NUM_ERRORS
} acu_sccp_cs_fail_t;
#undef ACU_SCCP_CON_FAIL_INFO_ENUM

/* Information about the status of a TCP/IP connection to SCCP */
typedef struct acu_sccp_con_state {
    unsigned int        cs_ipaddr;           /* IPv4 address of SCCP system */
    unsigned int        cs_tcpport;          /* TCP port number */
    acu_sccp_cs_state_t cs_state;            /* Current state */
    acu_sccp_cs_fail_t  cs_failure;          /* Reason for last connect fail */
    const char          *cs_fail_text;       /* NB: more descriptive */
    unsigned int        cs_tx_qlen;          /* Transmit queue_length */
    struct sockaddr_in6 cs_sockaddr;
} acu_sccp_con_state_t;


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

/* Structure returned by acu_sccp_msg_get_sccp_status_ind() with information
 * from ACU_SCCP_MSG_SP/USER_STATUS messages */
typedef struct acu_sccp_sccp_status {
    unsigned int             tsp_pc;        /* Affected pointcode */
    unsigned char            tsp_ssn;       /* Affected SSN (or zero) */
    char                     tsp_host;      /* Reporting host, 'a' or 'b' */
    char                     tsp_pad[2];
    enum acu_sccp_u_svals    tsp_user_status;
    enum acu_sccp_sp_svals   tsp_sp_status;
    enum acu_sccp_sccp_svals tsp_sccp_status;
    unsigned int             tsp_tx_cong_cost;
} acu_sccp_sccp_status_t;

/*
 * SCCP message structure.  Used for both inward and outbound messages.
 * Inbound messages are queued on both the ssap and connection data areas.
 * NB This is the first element of the much longer structure used internally.
 * Applications should not modify any fields of this structure.
 */
struct acu_sccp_msg {
    acu_sccp_msg_type_t    tm_msg_type;      /* Overall received message type */

    acu_sccp_ssap_t        *tm_ssap;         /* Associated ssap */
    acu_sccp_con_t         *tm_con;          /* and connection (if any) */

    /* Address fields for received messages (NULL if invalid) */
    const acu_sccp_addr_t  *tm_local_addr;   /* Called address from SCCP */
    const acu_sccp_addr_t  *tm_remote_addr;  /* Calling address from SCCP */

    /* Data from received message */
    const void             *tm_data;
    unsigned int           tm_data_length;

    unsigned char          tm_ret_opt;       /* UDT 'return on error' bit */
    unsigned char          tm_class;         /* SCCP class 0..3 */

    unsigned char          tm_cause;         /* Cause from (X)UDTS, RLSD etc */

    unsigned char          tm_priority;      /* Message importance/priority */

    unsigned char          tm_timer_id;      /* for MSG_TIMEOUT */
};
#undef acu_sccp_msg

typedef struct acu_sccp_msg acu_sccp_msg_t;

#ifdef _WIN32
typedef void *acu_sccp_os_event_t;
#else
typedef int acu_sccp_os_event_t;
#endif

#ifdef __GNUC__
#define ACU_SCCP_PRINTF_ATTR(F, L) __attribute__((format(printf, (F), (L))))
#else
#define ACU_SCCP_PRINTF_ATTR(F, L)
#endif

/*
 * The SCCP interfaces are defined using this pre-processor 'magic' in order to
 * make it possible to define other data structures using the same information.
 * In particular it can be used to define the data areas needed to dynamically
 * load the SCCP library.
 */

#define ACU_SCCP_FUNCTIONS(x) \
x(void, acu_sccp_trace_v, (acu_sccp_ssap_t *, unsigned int, const void *, int, \
                        const char *, va_list)) \
x(void, acu_sccp_trace, (acu_sccp_ssap_t *, unsigned int, const char *, ...) \
                        ACU_SCCP_PRINTF_ATTR(3, 4)) \
x(void, acu_sccp_trace_buf, (acu_sccp_ssap_t *, unsigned int, const void *, \
                        int, const char *, ...) ACU_SCCP_PRINTF_ATTR(5, 6)) \
x(int, acu_sccp_trace_error, (acu_sccp_ssap_t *, const char *, int, \
                        const char *, ...) ACU_SCCP_PRINTF_ATTR(4, 5)) \
x(const char *, acu_sccp_strerror, (int, unsigned int)) \
\
/* SSAP functions */ \
\
x(acu_sccp_ssap_t *, acu_sccp_ssap_create, (const char *, \
                        acu_sccp_ssap_flags_t)) \
x(void, acu_sccp_ssap_delete, (acu_sccp_ssap_t *)) \
x(int, acu_sccp_ssap_connect_sccp, (acu_sccp_ssap_t *)) \
x(int, acu_sccp_ssap_set_cfg_int, (acu_sccp_ssap_t *, acu_sccp_cfg_param_t, \
                        unsigned int)) \
x(int, acu_sccp_ssap_set_cfg_str, (acu_sccp_ssap_t *, acu_sccp_cfg_param_t, \
                        const char *)) \
x(acu_sccp_addr_t *, acu_sccp_ssap_get_locaddr, (acu_sccp_ssap_t *)) \
x(acu_sccp_addr_t *, acu_sccp_ssap_get_remaddr, (acu_sccp_ssap_t *)) \
\
/* Connection structure functions */ \
\
x(acu_sccp_con_t *, acu_sccp_con_create, (acu_sccp_ssap_t *)) \
x(void, acu_sccp_con_delete, (acu_sccp_con_t *)) \
x(acu_sccp_con_t *, acu_sccp_ssap_get_unitdata_con, (acu_sccp_ssap_t *)) \
x(void, acu_sccp_con_set_userptr, (acu_sccp_con_t *, void *)) \
x(void *, acu_sccp_con_get_userptr, (acu_sccp_con_t *)) \
x(int, acu_sccp_con_get_ids, (acu_sccp_con_t *, unsigned int *, \
                        unsigned int *, unsigned int *)) \
x(int, acu_sccp_con_set_cfg_int, (acu_sccp_con_t *, acu_sccp_cfg_param_t, \
                        unsigned int)) \
x(int, acu_sccp_con_set_cfg_str, (acu_sccp_con_t *, acu_sccp_cfg_param_t, \
                        const char *)) \
x(acu_sccp_addr_t *, acu_sccp_con_get_locaddr, (acu_sccp_con_t *)) \
x(acu_sccp_addr_t *, acu_sccp_con_get_remaddr, (acu_sccp_con_t *)) \
\
/* Message sending functions */ \
\
x(int, acu_sccp_connect_request, (acu_sccp_con_t *, const void *, \
                        unsigned int)) \
x(int, acu_sccp_connect_confirm, (acu_sccp_con_t *, const void *, \
                        unsigned int)) \
x(int, acu_sccp_connect_refused, (acu_sccp_con_t *, int, const void *, \
                        unsigned int)) \
x(int, acu_sccp_disconnect, (acu_sccp_con_t *, int, const void *, \
                        unsigned int)) \
x(int, acu_sccp_data_request, (acu_sccp_con_t *, const void *, \
                        unsigned int)) \
x(int, acu_sccp_unitdata_request, (acu_sccp_con_t *, const void *, \
                        unsigned int)) \
\
/* Message receiving functions */ \
\
x(int, acu_sccp_ssap_msg_get, (acu_sccp_ssap_t *, int, acu_sccp_msg_t **)) \
x(void, acu_sccp_ssap_wakeup_msg_get, (acu_sccp_ssap_t *)) \
x(int, acu_sccp_con_msg_get, (acu_sccp_con_t *, int, acu_sccp_msg_t **)) \
x(void, acu_sccp_con_wakeup_msg_get, (acu_sccp_con_t *)) \
x(int, acu_sccp_event_msg_get, (acu_sccp_event_t *, acu_sccp_msg_t **)) \
x(void, acu_sccp_msg_free, (acu_sccp_msg_t *)) \
x(void, acu_sccp_con_unblock, (acu_sccp_con_t *)) \
x(int, acu_sccp_con_block, (acu_sccp_con_t *)) \
x(int, acu_sccp_msg_copy_rx_buffer, (acu_sccp_msg_t *)) \
\
/* Connection timer functions */ \
\
x(int, acu_sccp_con_timer_start, (acu_sccp_con_t *, int, unsigned int)) \
x(int, acu_sccp_con_timer_restart, (acu_sccp_con_t *, int, unsigned int)) \
x(int, acu_sccp_con_timer_cancel, (acu_sccp_con_t *, int)) \
\
/* Connection status functions (connection to driver) */ \
\
x(int, acu_sccp_get_con_state, (acu_sccp_ssap_t *, int, \
                        const acu_sccp_con_state_t **)) \
x(int, acu_sccp_msg_get_con_state, (acu_sccp_msg_t *, \
                        const acu_sccp_con_state_t **, \
                        const acu_sccp_con_state_t **)) \
\
/* Remote SP and SSN status functions */ \
\
x(int, acu_sccp_get_sccp_status, (acu_sccp_ssap_t *, unsigned int, \
                        unsigned int, const acu_sccp_sccp_status_t **)) \
x(int, acu_sccp_msg_get_sccp_status, (acu_sccp_msg_t *, \
                        const acu_sccp_sccp_status_t **)) \
x(void, acu_sccp_enable_user_status, (acu_sccp_ssap_t *, unsigned int, \
                        unsigned int)) \
x(void, acu_sccp_enable_sp_status, (acu_sccp_ssap_t *, unsigned int)) \
\
/* Event functions */ \
\
x(acu_sccp_event_t *, acu_sccp_event_create, (acu_sccp_ssap_t *)) \
x(void, acu_sccp_event_delete, (acu_sccp_event_t *)) \
x(int, acu_sccp_event_wait, (acu_sccp_event_t *, int)) \
x(acu_sccp_os_event_t, acu_sccp_event_get_os_event, (acu_sccp_event_t *)) \
x(void, acu_sccp_event_clear, (acu_sccp_event_t *)) \
x(void, acu_sccp_event_signal, (acu_sccp_event_t *)) \
x(int, acu_sccp_event_ssap_attach, (acu_sccp_event_t *, acu_sccp_ssap_t *)) \
x(int, acu_sccp_event_ssap_detach, (acu_sccp_event_t *, acu_sccp_ssap_t *)) \
x(int, acu_sccp_event_ssap_detach_all, (acu_sccp_ssap_t *)) \
x(int, acu_sccp_event_con_attach, (acu_sccp_event_t *, acu_sccp_con_t *)) \
x(int, acu_sccp_event_con_detach, (acu_sccp_event_t *, acu_sccp_con_t *)) \
x(int, acu_sccp_event_con_detach_all, (acu_sccp_con_t *)) \
\
/* Undocumented diagnostic functions */ \
x(void, acu_sccp_dump_con_timers, (acu_sccp_ssap_t *, int)) \

/* Define C external function definitions */
#ifndef ACU_SCCP_NO_FN_DEFS      /* Define to suppress these definitions */
#define ACU_SCCP_FN_DEFS(rslt, name, args) ACU_SCCP_LIB_API rslt name args;
ACU_SCCP_FUNCTIONS(ACU_SCCP_FN_DEFS)
#undef ACU_SCCP_FN_DEFS
#endif

/* Define for C function variable externs */
#define ACU_SCCP_EXTERN_FN_DEFS_(rslt, name, args) extern rslt (*name) args;
#define ACU_SCCP_EXTERN_FN_DEFS ACU_SCCP_FUNCTIONS(ACU_SCCP_EXTERN_FN_DEFS_)

/* Define for C function variables */
#define ACU_SCCP_FN_DEFS_(rslt, name, args) rslt (*name) args = 0;
#define ACU_SCCP_FN_DEFS ACU_SCCP_FUNCTIONS(ACU_SCCP_FN_DEFS_)

/* Code fragment to initialise function pointers from library handle/file */
#define ACU_SCCP_FN_INIT(fd, err_fn) \
{ \
    typedef struct { const char *fn_name; void **fn_addr; } fni_t; \
    static const fni_t fn_info[] = {ACU_SCCP_FUNCTIONS(ACU_SCCP_FN_TABLE) {0}};\
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
#define ACU_SCCP_FN_TABLE(rval, name, arg) {#name, (void **)(void *)&name},
#ifdef _WIN32
#define dlsym(fd, fn) GetProcAddress(fd, fn)
#endif

#ifdef __cplusplus
}
#endif

#endif
