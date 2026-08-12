#ifndef ACU_RESOLV_H
#define ACU_RESOLV_H

/* These are the only request types that are understood */
#define ACU_NS_T_A       1
#define ACU_NS_T_CNAME   5
#define ACU_NS_T_SOA     6
#define ACU_NS_T_NULL   10
#define ACU_NS_T_PTR    12
#define ACU_NS_T_MX     15
#define ACU_NS_T_AAAA   28
#define ACU_NS_T_SRV    33
#define ACU_NS_T_NAPTR  35

#ifndef ACU_RESOLV_LIB_API
#ifdef _WIN32
#define ACU_RESOLV_LIB_API __declspec(dllimport)
#else
#define ACU_RESOLV_LIB_API __attribute__((visibility("default")))
#endif
#endif

#ifndef _WIN32
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>


/* A copy of enough of the horrid camel-infested windows definitions for compatibility */
typedef enum _DnsSection {
    DnsSectionQuestion,
    DnsSectionAnswer,
    DnsSectionAuthority,
    DnsSectionAddtional
} DNS_SECTION;

typedef struct _DnsRecordFlags {
    unsigned int   Section:2;
    unsigned int   Delete:1;
    unsigned int   CharSet:2;   // 3 => Ascii
    unsigned int   Unused:3;

    unsigned int   Reserved:24;
} DNS_RECORD_FLAGS;

typedef union {
    uint64_t             IP6Qword[2];
    uint32_t             IP6Dword[4];
    uint16_t             IP6Word[8];
    uint8_t              IP6Byte[16];
} IP6_ADDRESS;

typedef struct {
    uint32_t             IpAddress;   // Network order
} DNS_A_DATA;

typedef struct {
    const char           *pNameHost;
} DNS_PTR_DATA;

typedef struct {
    const char           *pNamePrimaryServer;
    const char           *pNameAdministrator;
    unsigned int         dwSerialNo;
    unsigned int         dwRefresh;
    unsigned int         dwRetry;
    unsigned int         dwExpire;
    unsigned int         dwDefaultTtl;
} DNS_SOA_DATA;

typedef struct {
    unsigned int         dwByteCount;
    unsigned char        Data[];
} DNS_NULL_DATA;

typedef struct {
    const char           *pNameMailbox;
    const char           *pNameErrorsMailbox;
} DNS_MINFO_DATA;

typedef struct {
    const char           *pNameExchange;
    unsigned int         wPreference;
} DNS_MX_DATA;

typedef struct {
    unsigned int         dwStringCount;
    const char           *pStringArray[];
} DNS_TXT_DATA;

typedef struct {
    IP6_ADDRESS          Ip6Address;
} DNS_AAAA_DATA;

/* See RFC 2782 */
typedef struct {
    const char           *pNameTarget;
    unsigned int         wPriority;
    unsigned int         wWeight;
    unsigned int         wPort;
} DNS_SRV_DATA;

/* See RFC 2915 */
typedef struct {
    unsigned int         wOrder;
    unsigned int         wPreference;
    const char           *pFlags;
    const char           *pService;
    const char           *pRegularExpression;
    const char           *pReplacement;
} DNS_NAPTR_DATA;

typedef struct _DnsRecord {
    struct _DnsRecord    *pNext;
    const char           *pName;
    unsigned int         wType;
    unsigned int         wDataLength;
    union {
        unsigned int     DW;
        DNS_RECORD_FLAGS S;
    } Flags;
    unsigned int         dwTtl;

    union {
        unsigned char    raw_data[20];
        DNS_A_DATA       A;
        DNS_PTR_DATA     PTR, Ptr;
        DNS_PTR_DATA     CNAME, Cname;
        DNS_SOA_DATA     SOA, Soa;
        DNS_NULL_DATA    Null;
        DNS_MINFO_DATA   MINFO, Minfo;
        DNS_MX_DATA      MX, Mx;
        DNS_TXT_DATA     TXT, Txt;
        DNS_AAAA_DATA    AAAA;
        DNS_SRV_DATA     SRV, Srv;
        DNS_NAPTR_DATA   NAPTR, Naptr;
    } Data;
} DNS_RECORD;

#else
#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 4214)  // nonstandard extension used: bit field types other than int
#include <windns.h>
#pragma warning(pop)

#endif

/* Trace function could be (acu_dns_trace_fn)vfprintf.
 * Any '\n' are guaranteed to be at the end of format strings. */
typedef void (*acu_dns_trace_fn)(void *, const char *, va_list);

int ACU_RESOLV_LIB_API acu_dns_query(const char *, unsigned int, unsigned int,
        DNS_RECORD **, acu_dns_trace_fn, void *);

void ACU_RESOLV_LIB_API acu_dns_result_free(DNS_RECORD *p);

/* 'Pretty' print lookup results.
 * Format and args printed before first entry (if any present)
 */
void ACU_RESOLV_LIB_API acu_dns_dump_result(DNS_RECORD *, acu_dns_trace_fn,
        void *, const char *, ...);
void ACU_RESOLV_LIB_API acu_dns_dump_results(DNS_RECORD *, acu_dns_trace_fn,
        void *, const char *, ...);

#endif
