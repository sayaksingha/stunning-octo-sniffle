/* ticker.h - fast ticking timer for performance measuring */

#ifndef INCLUDED_TICKER_H
#define INCLUDED_TICKER_H

#ifdef TiNGTYPE_LINUX
#include <stdint.h>

static uint_fast64_t ticker(void)
{
 uint_fast64_t val;
#if defined(TiNGTYPE_PXPPC) || defined(TiNGCPUTYPE_PPC)
#ifdef TiNG_PPC_USE_MFTB
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
 val = h0;
 val <<= 32;
 val += low;
#else
 unsigned long h0, low, h1;
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
 val = h0;
 val <<= 32;
 val += low;
#endif
#elif defined(TiNGCPUTYPE_AMD64)
 unsigned long a, d;
 __asm__ volatile (".byte 0x0f, 0x31" : "=a" (a), "=d" (d));
 val = d;
 val <<= 32;
 val += a;
#else
 __asm__ __volatile__ (
 	".byte 0xf; .byte 0x31"
 	:  "=A" (val) );
#endif
 return val;
}
#endif

#ifdef TiNGTYPE_QNX
#include <stdint.h>

#ifdef __PPC__
static uint_fast64_t ticker(void)
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
static uint_fast64_t ticker(void)
{
 uint_fast64_t val;
 __asm__ __volatile__ (
 	".byte 0xf; .byte 0x31"
 	:  "=A" (val) );
 return val;
}
#endif

#endif


#ifdef TiNGTYPE_WINNT
#include "WINNT/stdint.h"

#ifdef __DMC__
static __declspec (naked) __int64 ticker(void)
{
 __asm {
	_emit 0x0f __asm _emit 0x31
	ret
 }
}
#endif
#ifdef __GNUC__
static uint_fast64_t ticker(void)
{
 unsigned long long val;
#ifdef TiNGCPUTYPE_AMD64
 unsigned long a, d;
 __asm__ __volatile__ (".byte 0x0f, 0x31" : "=a" (a), "=d" (d));
 val = d;
 val <<= 32;
 val += a;
#else
 __asm__ __volatile__ (
 	".byte 0xf; .byte 0x31"
 	:  "=A" (val) );
#endif //TiNGCPUTYPE_AMD64
 return val;
}
#endif
#ifdef _MSC_VER
#ifdef TiNGCPUTYPE_AMD64

#include <intrin.h>

#pragma intrinsic(__rdtsc)
#define ticker() __rdtsc()

#else
	// disable warning that it has no return value
#pragma warning(disable : 4035)
static __declspec (naked) uint_fast64_t ticker(void)
{
 __asm {
	_emit 0x0f __asm _emit 0x31
	ret
 }
}
#endif //TiNGCPUTYPE_AMD64
#endif //_MSC_VER
#endif

#endif
