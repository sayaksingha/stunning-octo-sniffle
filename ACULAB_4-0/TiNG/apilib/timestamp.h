#ifndef INCLUDED_TIMESTAMP_H
#define INCLUDED_TIMESTAMP_H

// a fine-grained timestamp with very low overhead, matching:
//	unsigned long long timestamp_now(void);
// but implemented inline where possible
//
// The macro TIME_FMT is a constant string which is the appropriate
// printf() format for a timestamp

#ifdef TiNGTYPE_LINUX

__inline__ STATIC unsigned long long timestamp_now(void)
{
 // __asm__ __volatile__ (".byte 0x0f; .byte 0x31" : "=A" (t));
#if defined(TiNGTYPE_PXPPC) || defined(TiNGCPUTYPE_PPC)
 unsigned long long t;
 unsigned long h0, low, h1;
#ifdef TiNG_PPC_USE_MFTB
 __asm__ __volatile__ (
 	"9:\n"
 	"	mftbu %0\n"
 	"	mftb %1\n"
 	"	mftbu %2\n"
 	"	cmpw %0,%2\n"
 	"	bne 9b\n"
 	: "=r" (h0), "=r" (low), "=r" (h1)
 	: 
 	: "cc");
#else
#ifndef TiNG_STR_SPRN_TBRL
#define TiNG_STR_SPRN_TBRL   "0x10C"   /* Time Base Read Lower Register (user, R/O) */
#endif
#ifndef TiNG_STR_SPRN_TBRU
#define TiNG_STR_SPRN_TBRU   "0x10D"   /* Time Base Read Upper Register (user, R/O) */
#endif
 __asm__ __volatile__ (
    "9:\n"
    "       mfspr %0," TiNG_STR_SPRN_TBRU "\n"
    "       mfspr %1," TiNG_STR_SPRN_TBRL "\n"
    "       mfspr %2," TiNG_STR_SPRN_TBRU "\n"
    "       cmpw %0,%2\n"
    "       bne 9b\n"
    : "=r" (h0), "=r" (low), "=r" (h1)
    :
    : "cc");
#endif
 t = h0;
 t <<= 32;
 t += low;
 return t;
#elif defined(TiNGCPUTYPE_AMD64)
 unsigned long a, d;
 unsigned long long val;
 __asm__ volatile (".byte 0x0f, 0x31" : "=a" (a), "=d" (d));
 val = d;
 val <<= 32;
 val += a;
 return val;
#else
 unsigned long long val;
 __asm__ __volatile__ (
 	".byte 0xf; .byte 0x31"
 	:  "=A" (val) );
 return val;
#endif
}

#endif

#ifdef TiNGTYPE_QNX

#ifdef __PPC__

__inline__ STATIC unsigned long long timestamp_now(void)
{
 unsigned long long t;
 unsigned long h0, low, h1;
 __asm__ __volatile__ (
 	"9:\n"
 	"	mftbu %0\n"
 	"	mftb %1\n"
 	"	mftbu %2\n"
 	"	cmpw %0,%2\n"
 	"	bne 9b\n"
 	: "=r" (h0), "=r" (low), "=r" (h1)
 	: 
 	: "cc");
 t = h0;
 t <<= 32;
 t += low;
 return t;
}

#endif

#ifdef __X86__
__inline__ STATIC unsigned long long timestamp_now(void)
{
 // __asm__ __volatile__ (".byte 0x0f; .byte 0x31" : "=A" (t));
 unsigned long long t;
 __asm__ __volatile__ ("rdtsc" : "=A" (t));
 return t;
}

#endif

#endif


#ifdef TiNGTYPE_WINNT
#ifdef __DMC__
STATIC __declspec (naked) __int64 timestamp_now(void)
{
 __asm {
	_emit 0x0f __asm _emit 0x31
	ret
 }
}
#endif
#ifdef _MSC_VER
#ifdef TiNGCPUTYPE_AMD64

#include <intrin.h>

#pragma intrinsic(__rdtsc)
#define timestamp_now() __rdtsc()

#else
	// disable warning that it has no return value
#pragma warning(disable : 4035)
STATIC __declspec (naked) __int64 timestamp_now(void)
{
 __asm {
	_emit 0x0f __asm _emit 0x31
	ret
 }
}
#endif
#define TIME_FMT "@%I64u:"

#endif

#ifdef __GNUC__

__inline__ STATIC unsigned long long timestamp_now(void)
{
 unsigned long long t;
 // __asm__ __volatile__ (".byte 0x0f; .byte 0x31" : "=A" (t));
 __asm__ __volatile__ ("rdtsc" : "=A" (t));
 return t;
}
#endif
#endif

#ifndef TIME_FMT
#define TIME_FMT "@%lld:"
#endif

#endif
