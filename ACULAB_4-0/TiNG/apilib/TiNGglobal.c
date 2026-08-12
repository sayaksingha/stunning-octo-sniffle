#include "TiNGanyevent.h"
#include "TiNGcommon.h"
#include "globaltime.h"
#include "trace.h"
#include <stdlib.h>

#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif
#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define SYSTIME_GETTIMEOFDAY
#endif
#ifdef TiNGTYPE_QNX
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define SYSTIME_GETTIMEOFDAY
#endif
#include "timestamp.h"

#define STRINGISER(s) #s
#define STRING(s) STRINGISER(s)

LOCALDEF const char *TiNG_buildinfo = "TiNGglobal.o"
#ifdef _REENTRANT
	" (_REENTRANT defined)"
#endif
#ifndef OMIT_ANYCHAN
	" (OMIT_ANYCHAN undefined)"
#endif
#ifdef TiNG_PARANOIA
	" (TiNG_PARANOIA defined)"
#endif
#ifdef TiNG_TRACE
	" (TiNG_TRACE=" STRING(TiNG_TRACE) ")"
#endif
	;

#include <inttypes.h>

#ifdef TiNGTYPE_LINUX
STATIC void report_trace_tick_rate(void)
{
 // parse /proc/cpuinfo to get the first CPU clock speed
 // SMP machines should have all processors the same
 // speedstep/powernow machines may have varying clocks, 
 // but then the timestamps aren't very useful
 char buf[4096]; // should be plenty
 int ok = 0;
 FILE* f = fopen("/proc/cpuinfo","r");
 if (f) {
	int len = fread(buf,1,4095,f); // one less so we can always add the null
	fclose(f);
	if (len > 0) {
		char *a;
		buf[len] = 0;
		a = strstr(buf, "cpu MHz");
		if (a) { // find the number after cpu MHz
			while (*a && (*a < '0' || *a > '9')) {
				a++;
			}
			if (a) { // we're at the number
				int rate = 0;
				while (*a >= '0' && *a <= '9') {
					rate *= 10;
					rate += (*a++)-'0';
				}
				// got the integer part (ignoring the rest)
				olog("ticks_per_us = %d\n", rate);
				ok = 1;
			}
		}
	}
 }
 if (!ok) olog("Failed to get trace tick rate\n");
}
#endif

#ifdef TiNGTYPE_QNX
#include <sys/syspage.h>
STATIC void report_trace_tick_rate(void)
{
 uint64_t cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
 uint32_t tpus = cps/1000000;
 olog("ticks_per_us = %d\n", tpus);
}
#endif


#ifdef TiNGTYPE_WINNT
STATIC void report_trace_tick_rate(void)
{
 // read processor 0's clock rate from registry
 // SMP machines should have all processors the same
 // speedstep/powernow machines may have varying clocks, 
 // but then the timestamps aren't very useful
 HKEY key;
 DWORD val, type, len = 4;
 LONG res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
						0,
						KEY_QUERY_VALUE,
						&key);
 if (res == ERROR_SUCCESS) {
	res = RegQueryValueEx(key, "~MHz", 0, &type, (unsigned char*)&val, &len);
	RegCloseKey(key);
 }
 if (res == ERROR_SUCCESS && type == REG_DWORD) {
	olog("ticks_per_us = %d\n", val);
 } else {
	olog("Failed to get trace tick rate\n");
 }
}
#endif

#ifndef TiNG_USE_MONOTONIC_CLOCK
#	ifdef _POSIX_TIMERS
#		ifdef CLOCK_MONOTONIC
#	define		TiNG_USE_MONOTONIC_CLOCK
#		endif
#	endif
#endif

#ifdef TiNG_USE_MONOTONIC_CLOCK
STATIC int TiNG_can_use_monotonic_clock;
#endif

STATIC int init(void)
{
 uint_fast64_t ticks_per_us;
#if TiNG_TRACE > 0
 {
	char *trace = getenv("TINGTRACE");
	if (trace) {
		char *s;
		TiNGtrace = strtol(trace, &s, 0);
		if (*s) {
			olog("TINGTRACE: junk after number: '%s'\n", s);
		}
	}
	if (TiNGtrace > 1) olog("init %s %s\n", version, TiNG_buildinfo);
 }
#endif
#ifdef TiNG_USE_MONOTONIC_CLOCK
 TiNG_can_use_monotonic_clock = 0;
 {
	char *m = getenv("TiNG_CLOCK_MONOTONIC");
	if (m) {
		char *s;
		TiNG_can_use_monotonic_clock = strtol(m, &s, 0);
		if (*s) {
			olog("TiNG_CLOCK_MONOTONIC: junk after number: '%s'\n", s);
		}
	}
	if (TiNG_can_use_monotonic_clock) {
		struct timespec ts;
		if (clock_getres(CLOCK_MONOTONIC, &ts)) { // it is not supported if this returns non zero
			TiNG_can_use_monotonic_clock = 0;
			if (TiNGtrace > 1) olog("CLOCK_MONOTONIC not supported\n");
		}
	}
	if (TiNGtrace > 1 && TiNG_can_use_monotonic_clock) olog("Using CLOCK_MONOTONIC\n");
 }
#endif
 timecal_start();
#if TiNG_TRACE > 1
 mutx_init(&trace_mx);
 trace_hand = trace_vtbl->io_invalidfile();
#endif
 cardmap_ctor(&aculab_TiNG_cardmap);
#ifndef OMIT_ANYCHAN
 anychan_init();
#endif
 ticks_per_us = timecal_update();
 if (TiNGtrace > 1) olog("time calibration = %" PRIu64 "\n", ticks_per_us);
 if (TiNGtrace > 1) report_trace_tick_rate();
 if (TiNGtrace > 1) {
#ifdef TiNGTYPE_WINNT
	SYSTEMTIME syst;
	GetSystemTime(&syst);
	olog(TIME_FMT "system time UTC %d/%02d/%02d %d:%02d:%02d.%03d\n", timestamp_now(), syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds);
#endif
#ifdef SYSTIME_GETTIMEOFDAY
	struct timeval tv;
	struct tm syst;
	gettimeofday(&tv, 0);
	gmtime_r(&tv.tv_sec, &syst);
	olog(TIME_FMT "system time UTC %d/%02d/%02d %d:%02d:%02d.%06ld\n", timestamp_now(), syst.tm_year+1900, syst.tm_mon+1, syst.tm_mday, syst.tm_hour, syst.tm_min, syst.tm_sec, tv.tv_usec);
#endif
 }
 return 0;
}

RUN_ONCE run_once_init = RUN_ONCE_INITIALISER(init);

