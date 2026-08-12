/* assptest.c - test ASSP */

#include <stdlib.h>
#include <stdio.h>
#include "assp.h"
#include "timefn.h"

#ifdef USE_FTIME
#include <sys/timeb.h>

static uint_fast64_t now_u64(void)
{
 struct timeb ft;
 uint_fast64_t now;
 ftime(&ft);
 now = ft.time;
 now *= 10000000;		// convert to uS (in precision of ABSTIME)
 now += ft.millitm * 1000;
 return a_now;
}

int_fast64_t time_tai_uS(void)
{
 return now_u64();
}
#endif

#ifdef USE_GETTIMEOFDAY
#include <sys/time.h>
#include <unistd.h>

static uint_fast64_t now_u64(void)
{
 struct timeval tod;
 uint_fast64_t now;
 gettimeofday(&tod, 0);
#ifdef __GNUC__
#ifdef __i386__
	// major hotspot, according to gprof
 	// this saves about 10% of total running time for simple replay
 __asm__ ("mul %2" : "=A" (now) :  "%a" (tod.tv_sec), "q" (1000*1000));
 now += (unsigned) tod.tv_usec;	// why is this field signed????
#else
 now = tod.tv_sec;
 now *= 1000 * 1000;		// convert to uS (in precision of ABSTIME)
 now += tod.tv_usec;
#endif
#else
 now = tod.tv_sec;
 now *= 1000 * 1000;		// convert to uS (in precision of ABSTIME)
 now += tod.tv_usec;
#endif
 return now;
}

int_fast64_t time_tai_uS(void)
{
 return now_u64();
}
#endif

#ifdef USE_GETHRTIME
#include <sys/time.h>
#include <unistd.h>

static uint_fast64_t now_u64(void)
{
 return gethrtime() / 1000;
}

int_fast64_t time_tai_uS(void)
{
 uint_fast64_t now = now_u64();
 struct timeval tod;
 gettimeofday(&tod, 0);
 now = tod.tv_sec;
 now *= 1000 * 1000;		// convert to uS (in precision of ABSTIME)
 now += tod.tv_usec;
 return now;
}
#endif

#ifdef USE_GETSYSTEMTIMEASFILETIME
#include <windows.h>

static uint_fast64_t now_u64(void)
{
 LARGE_INTEGER now_li;
 FILETIME ft;
 uint_fast64_t now;
 GetSystemTimeAsFileTime(&ft);
 now_li.LowPart = ft.dwLowDateTime;
 now_li.HighPart = ft.dwHighDateTime;
 now = now_li.QuadPart / 10;	// convert to uS from 100 nS
 return now;
}

int_fast64_t time_tai_uS(void)
{
 uint_fast64_t v;
 v = 134774;			// days between 1601-01-01 and 1970-01-01
 v *= 60 * 60 * 24;		// seconds
 v *= 1000 * 1000;		// uS
 return now_u64() - v;
}
#endif

#ifdef USE_RDTSC
#ifdef __GNUC__
static uint_fast64_t ticker(void)
{
 unsigned long long val;
#ifdef TiNGCPUTYPE_AMD64
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
#ifdef _MSC_VER
#ifdef TiNGCPUTYPE_AMD64
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#define ticker() __rdtsc()
#else
#pragma warning(disable : 4035)
static __declspec (naked) uint_fast64_t ticker(void)
{
 __asm {
	_emit 0x0f __asm _emit 0x31
	ret
 }
}
#endif
#endif

#ifdef TiNGTYPE_LINUX
#include <sys/time.h>
#include <unistd.h>

static uint_fast64_t tpus;

static uint_fast64_t now_u64(void)
{
 if (!tpus) {
	uint_fast64_t s = ticker();
	sleep(1);
	tpus = (ticker() - s) / 1000 / 1000;
	//fprintf(stderr, "tpus = %d\n", (long) tpus);
 }
 return ticker() / tpus;
}

int_fast64_t time_tai_uS(void)
{
 uint_fast64_t now = now_u64();
 struct timeval tod;
 gettimeofday(&tod, 0);
 now = tod.tv_sec;
 now *= 1000 * 1000;		// convert to uS (in precision of ABSTIME)
 now += tod.tv_usec;
 return now;
}
#endif

#ifdef TiNGTYPE_WINNT
#include <windows.h>

static uint_fast64_t tpus;

static uint_fast64_t now_u64(void)
{
 if (!tpus) {
	uint_fast64_t s = ticker();
	Sleep(1000);
	tpus = (ticker() - s) / 1000 / 1000;
	//fprintf(stderr, "tpus = %d\n", (long) tpus);
 }
 return ticker() / tpus;
}

int_fast64_t time_tai_uS(void)
{
 LARGE_INTEGER now_li;
 FILETIME ft;
 uint_fast64_t v;
 GetSystemTimeAsFileTime(&ft);
 now_li.LowPart = ft.dwLowDateTime;
 now_li.HighPart = ft.dwHighDateTime;
 v = 134774;			// days between 1601-01-01 and 1970-01-01
 v *= 60 * 60 * 24;		// seconds
 v *= 1000 * 1000;		// uS
 return now_li.QuadPart / 10 - v;
}
#endif
#endif

#ifdef USE_GLOBALTIME

static uint_fast64_t now;

void systick(void)
{
  now = now_u64();
}

#define now_u64() now
#endif

ABSTIME time_when(RELTIME until)
{
 ABSTIME a_now;
#ifdef INLINE_TIMEFUNC
 a_now = now_u64();
#else
 a_now.t = now_u64();
#endif
 return time_add(a_now, until);
}

RELTIME time_until(ABSTIME when)
{
 ABSTIME a_now;
#ifdef INLINE_TIMEFUNC
 a_now = now_u64();
#else
 a_now.t = now_u64();
#endif
 return time_sub(when, a_now);
}

#ifdef USE_GETHRTIME
uint_fast64_t ticker(void)
{
 return gethrtime();
}
#endif
