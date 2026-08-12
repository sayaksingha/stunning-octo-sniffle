/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/tools/lib_decode/ss7_lib_decode.h,v 1.4 2024/11/15 09:18:41 davidla Exp $ $Name: SS7_6_17_0 $" */

#ifndef SS7_LIB_DECODE_H
#define SS7_LIB_DECODE_H

#ifndef ACU_SS7_DECODE_LIB_API
#ifdef _WIN32
#define ACU_SS7_DECODE_LIB_API __declspec(dllimport)
#else
#define ACU_SS7_DECODE_LIB_API __attribute__((visibility("default")))
#endif
#endif

/* Trace id values used for trace filtering and decode.
 * These appear (in hex) in the 'raw' trace file following the date/time */

/* The low 8 bits identify the trace source (code module) */
/* NB the TCAP library uses trace ids 0xc0 to 0xff */
#define SS7_TRACE_SRC_MASK      0x00ffu
#define SS7_TRACE_FLAGS(x) \
x(SS7_TRACE_F_ANON,     anon,       0) /* Trace source anonymous */ \
x(SS7_TRACE_F_MTP2,     mtp2,       1) /* Trace came from MTP2 */ \
x(SS7_TRACE_F_MTP3,     mtp3,       2) /* Trace came from MTP3 */ \
x(SS7_TRACE_F_ISUP,     isup,       3) /* Trace came from ISUP */ \
x(SS7_TRACE_F_SCCP,     sccp,       4) /* Trace came from SCCP */ \
x(SS7_TRACE_F_SSAP,     ssap,       5) /* ... and etc...       */ \
x(SS7_TRACE_F_ATTMAN,   attman,     6) /*                      */ \
x(SS7_TRACE_F_LINKMAN,  linkman,    7) /*                      */ \
x(SS7_TRACE_F_CICMAN,   cicman,     8) /*                      */ \
x(SS7_TRACE_F_CALLCTL,  callctl,    9) /*                      */ \
x(SS7_TRACE_F_C7OS,     c7os,      10) /*                      */ \
x(SS7_TRACE_F_REAL_OS,  real_os,   11) /*                      */ \
x(SS7_TRACE_F_CLOCK,    clock,     12) /*                      */ \
x(SS7_TRACE_F_TLS_HW,   tls_hw,    13) /*                      */ \
x(SS7_TRACE_F_TX_DATA,  tx_data,   14) /*                      */ \
x(SS7_TRACE_F_RX_DATA,  rx_data,   15) /*                      */ \
x(SS7_TRACE_F_M2UA,     pmxc,      16) /* PMX MTP2 interface   */ \
x(SS7_TRACE_F_TCP,      tcp,       17) /* TCP/IP trace         */ \
x(SS7_TRACE_F_ISUPTCP,  isuptcp,   18) /* Remote ISUP          */ \
x(SS7_TRACE_F_TCPLINK,  tcplink,   19) /* TCP signalling links */ \
x(SS7_TRACE_F_M3UA,     m3ua,      20) /* M3UA                 */ \
x(SS7_TRACE_F_NIF,      nif,       21) /* Nodal Interworking Function  */ \
x(SS7_TRACE_F_DUAL_LK,  dual_link, 22) /* Dual link            */ \
x(SS7_TRACE_F_IP,       ip,        23) /* Low level IP         */ \
x(SS7_TRACE_F_SCTP,     sctp,      24) /* Windows SCTP         */ \
x(SS7_TRACE_F_USER,     user,      25) /* Trace from user-land */ \
x(SS7_TRACE_F_DEVIF,    devif,     26) /* Device interface     */ \
x(SS7_TRACE_F_FBSDSCTP, fbsdsctp,  27) /* Win Freebsd sctp     */ \
x(SS7_TRACE_F_DAEMON,   daemon,    28) /* ss7 daemon/ss7sxc    */ \
x(SS7_TRACE_F_M2PA,     m2pa,      29) /* M2PA links           */ \

#define T_F(tag, name, value) tag = value,
enum { SS7_TRACE_FLAGS(T_F) SS7_TRACE_F_XXXX };
#undef T_F

/* Some bits to tell the decoder where to start */
#define SS7_TRACE_FMT_MASK      0x0f00u   /* 4 bits for decoder */
#define SS7_TRACE_FMT_NONE      0x0000u   /* Not protocol data */
#define SS7_TRACE_FMT_SIGTRAN   0x0100u   /* SIGTRAN frame (TCP data) */
#define SS7_TRACE_FMT_MTP2      0x0200u   /* Level 2 frame */
#define SS7_TRACE_FMT_MTP3      0x0300u   /* Level 3 frame */
#define SS7_TRACE_FMT_ISUP      0x0400u   /* ISUP message */
#define SS7_TRACE_FMT_SCCP      0x0500u   /* SCCP message */
#define SS7_TRACE_FMT_TCAP      0x0600u   /* ASN.1 encoded TCAP */
#define SS7_TRACE_FMT_ASN1      0x0700u   /* BER encoded ASN.1 */
#define SS7_TRACE_FMT_SCCP_ADDR 0x0800u   /* SCCP address buffer */
#define SS7_TRACE_FMT_TRACE     0x0900u   /* Trace from monitor */

/* Overall trace 'priorities' (lower numbers are higher priority)
 * LOG and below are more likely to be customer visible. */
#define SS7_TRACE_F_PRIORITIES  0xf000u   /* All priotity bits */
#define SS7_TRACE_F_FATAL       0x1000u   /* Likely fatal errors (to syslog) */
#define SS7_TRACE_F_ERROR       0x2000u   /* Serious problems (prot errors) */
#define SS7_TRACE_F_LOG         0x3000u   /* Worthy of logging (ie discards) */
#define SS7_TRACE_F_API_ERROR   0x4000u   /* API call parameter errors */
#define SS7_TRACE_F_NORM        0x5000u   /* Normal information */
#define SS7_TRACE_F_DIAG        0x7000u   /* Diagnostic information */
#define SS7_TRACE_F_FULL        0x8000u   /* Usually extra trace_printf calls */
#define SS7_TRACE_F_VERBOSE     0xc000u   /* Usually extra trace_mem calls */
#define SS7_TRACE_F_MAX         0xe000u   /* Highest trully valid level */
#define SS7_TRACE_F_PARANOID    0xf000u   /* NB: may cause mutex issues */

/* The userpart major variant (usually the mtp3 variant) is saved
 * in bits 16 and 17 (18 and 19 are reserved).
 * This is used to select between the ITU and ANSI ISUP decoders. */
#define SS7_TRACE_F_SET_VARIANT(variant) ((variant) << 16)
#define SS7_TRACE_F_GET_VARIANT(flags)   ((flags) >> 16 & 0x3)

/* Bits 20 to 22: Point code width settings (based on mtp3 variant) */
#define SS7_TRACE_F_PC_WIDTH_MASK  0x300000
#define SS7_TRACE_F_PC_WIDTH_14    0x000000   /* ITU International */
#define SS7_TRACE_F_PC_WIDTH_16    0x100000   /* Japan (not implemented) */
#define SS7_TRACE_F_PC_WIDTH_24    0x200000   /* ANSI, China */
#define SS7_TRACE_F_ISDN_LAPD      0x300000   /* For monitor trace */
#define SS7_TRACE_F_ANSI_888       0x400000   /* ansi pc in 8-8-8 format */
/* MTP2 extended sequence numbers (optional for high speed links) */
#define SS7_TRACE_F_MTP2_ESN       0x800000
#define SS7_TRACE_F_MTP3_IS_MTP2  0x1000000   /* For monitor trace */

/* Miscellany */
#define SS7_TRACE_F_DECODE_TRACE   (1u << 30)  /* Decode MTP2 from trace */
#define SS7_TRACE_F_NO_HEXDUMP     (1u << 31)  /* Disables decoder hexdump */

#ifdef __cplusplus
extern "C"{
#endif

struct su_info;

#define ACU_SS7_DECODE_FUNCTIONS(x) \
    x(void, ss7_decode_free_su_info, (struct su_info *)) \
    x(struct su_info *, ss7_decode_init_su_info, (struct su_info *)) \
    x(void, ss7_decode_set_verbose, (struct su_info *, int)) \
    x(void, ss7_decode_set_pedantic, (struct su_info *, int)) \
\
    x(int, ss7_decode_set_variant, (struct su_info *, const char *)) \
    x(int, ss7_decode_su, (struct su_info *, unsigned int, \
        const unsigned char *, unsigned int)) \
\
    x(const char *, ss7_decode_get_output, (struct su_info *)) \
    x(int, ss7_decode_get_warnings, (struct su_info *)) \

/* Define C external function definitions */
#ifndef ACU_SS7_DECODE_NO_FN_DEFS    /* Define to suppress these definitions */
#define ACU_SS7_DECODE_FN_DEFS(rslt, name, args) \
        ACU_SS7_DECODE_LIB_API rslt name args;
ACU_SS7_DECODE_FUNCTIONS(ACU_SS7_DECODE_FN_DEFS)
#undef ACU_SS7_DECODE_FN_DEFS
#endif



#ifdef __cplusplus
}
#endif

#endif

