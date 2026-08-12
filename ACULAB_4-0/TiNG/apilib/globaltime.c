#include "globaltime.h"
#include <inttypes.h>

#ifdef TiNGTYPE_LINUX
#define CALIBRATE_FTIME
#endif
#ifdef TiNGTYPE_QNX
#define CALIBRATE_FTIME
#endif
#ifdef TiNGTYPE_WINNT
#define CALIBRATE_QUERYPERFORMANCEFREQUENCY
#endif

#ifdef CALIBRATE_FTIME
#include "ticker.h"

#define FTIME_MIN_CAL_PERIOD 100

 // If there is a system time jump (e.g. due to ntpdate) we will get the wrong result
 // We assume this will only happen once, but can be either forward or backwards
 // So calculate the ticks_per_us at least three times and use the modal value
#define FTIME_CAL_TRIES 3

#ifdef TiNGTYPE_LINUX
#include <sys/poll.h>
#ifndef Sleep
#define Sleep(x) poll(0, 0, (x))
#endif
#include <sys/timeb.h>
#endif

#ifdef TiNGTYPE_QNX
#include <sys/poll.h>
#ifndef Sleep
#define Sleep(x) poll(0, 0, (x))
#endif
#include <sys/timeb.h>
#endif


#ifdef TiNGTYPE_WINNT
#include <sys/timeb.h>
#include <sys/types.h>
#define ftime _ftime
#endif

struct timecal_t {
	struct timeb st;
	uint_fast64_t start;
	struct timeb en;
	uint_fast64_t end;
} timecal[FTIME_CAL_TRIES];

static uint_fast64_t ticks_per_us;

#ifndef TiNG_USE_MONOTONIC_CLOCK
#	ifdef _POSIX_TIMERS
#		ifdef CLOCK_MONOTONIC
#	define		TiNG_USE_MONOTONIC_CLOCK
#		endif
#	endif
#endif

#ifdef TiNG_USE_MONOTONIC_CLOCK
extern int TiNG_can_use_monotonic_clock;

typedef uint_fast64_t (*now_u64_func_ptr)(void);

static now_u64_func_ptr now_u64_func;


static uint_fast64_t now_u64_ticker(void)
{
 return ticker() / ticks_per_us;
}

static uint_fast64_t now_u64_gettime(void)
{
 struct timespec t;
 uint_fast64_t now;
 clock_gettime(CLOCK_MONOTONIC, &t);
 now = t.tv_sec * UINT64_C(1000000);
 now += t.tv_nsec/1000;
 return now;
}

uint_fast64_t now_u64(void)
{
 return (*now_u64_func)();
}
#else
uint_fast64_t now_u64(void)
{
 return ticker() / ticks_per_us;
}
#endif

LOCALDEF void timecal_start(void)
{
#ifdef TiNG_USE_MONOTONIC_CLOCK
 struct timespec ts;
 if (TiNG_can_use_monotonic_clock && !clock_getres(CLOCK_MONOTONIC, &ts)) { // should we check the res?
	 now_u64_func = &now_u64_gettime;
	 ticks_per_us = ts.tv_nsec;
 } else {
	 now_u64_func = &now_u64_ticker;
#endif
// NB all the time calibration is done in the 'update' function
#ifdef TiNG_USE_MONOTONIC_CLOCK
 }
#endif
}

LOCALDEF uint_fast64_t timecal_update(void)
{
 if (ticks_per_us) return ticks_per_us;
 uint_fast64_t minDur = FTIME_MIN_CAL_PERIOD;
 unsigned num = FTIME_CAL_TRIES;
 unsigned i, j;
 uint_fast64_t ticks[FTIME_CAL_TRIES];

 for (i = 0; i < num; i++) {
 	ticks[i] = 0;
 }

 for (i = 0; i < num; i++) {
 	uint_fast64_t dt;
 	uint_fast64_t tick;
 	struct timecal_t *ptimecal = &timecal[i];
 	struct timeb t;
 	for (;;) {
 		ftime(&t);
 		ptimecal->start = (uint_fast64_t)ticker();
 		ftime(&ptimecal->st);
 		if (ptimecal->st.millitm == (t.millitm + 1) && ptimecal->st.time == t.time) break; // doesn't handle 1.000 vs 0.999 but 1ms won't matter
 	}
 	for (;;) {
 		ftime(&t);
 		ptimecal->end = (uint_fast64_t)ticker();
 		ftime(&ptimecal->en);
 		if (ptimecal->en.millitm == (t.millitm + 1) && ptimecal->en.time == t.time) {
 			dt = ptimecal->en.time - ptimecal->st.time;
 			dt *= 1000;
 			dt += ptimecal->en.millitm;
 			dt -= ptimecal->st.millitm;
 			if (dt >= minDur) break;
 			Sleep(minDur - dt);
 		}
 	}
 	dt *= 1000;
 	tick = (ptimecal->end - ptimecal->start) / dt;

 	j = 0;
 	while (ticks[j] > tick) j++;
 	if (j < i) memmove(&ticks[j+1], &ticks[j], (i - j) * sizeof(tick));
 	ticks[j] = tick;
 }
 ticks_per_us = ticks[num/2]; // the modal value
 if (ticks_per_us == 0) abort(); // we'll divide by this later, so fail now to aid debugging
 return ticks_per_us;
}

#endif

#ifdef CALIBRATE_GETTIMEOFDAY
#include "ticker.h"
#ifdef TiNGTYPE_LINUX
#include <sys/time.h>
#endif
#ifdef TiNGTYPE_QNX
#include <sys/time.h>
#endif
struct {
	struct timeval st;
	uint_fast64_t start;
} timecal;

static uint_fast64_t ticks_per_us;

LOCALDEF void timecal_start(void)
{
 if (!timecal.st.tv_sec) {
	for (;;) {
		struct timeval t;
		gettimeofday(&t, 0);
		timecal.start = ticker();
		gettimeofday(&timecal.st, 0);
		if (timecal.st.tv_usec != t.tv_usec) break;
	}
 }
}

LOCALDEF uint_fast64_t timecal_update(void)
{
 uint_fast64_t end;
 struct timeval en;
 uint_fast64_t dt;
 if (ticks_per_us) return ticks_per_us;
 for (;;) {
	struct timeval t;
	gettimeofday(&t, 0);
  	end = ticker();
	gettimeofday(&en, 0);
  	if (en.tv_usec != t.tv_usec) break;
 }
 dt = en.tv_sec - timecal.st.tv_sec;
 dt *= 1000 * 1000;
 dt += en.tv_usec - timecal.st.tv_usec;
 ticks_per_us = (end - timecal.start) / dt;
 return ticks_per_us;
}

uint_fast64_t now_u64(void)
{
 return ticker() / ticks_per_us;
}
#endif

#ifdef CALIBRATE_GETHRTIME
#include <sys/time.h>

LOCALDEF void timecal_start(void)
{
}

LOCALDEF uint_fast64_t timecal_update(void)
{
 return 1000;
}

uint_fast64_t now_u64(void)
{
 return gethrtime() / 1000;
}
#endif

#ifdef CALIBRATE_QUERYPERFORMANCEFREQUENCY
static uint_fast64_t ticks_per_us;
#include "../libutil/WINNT/wind.h"

LOCALDEF void timecal_start(void)
{
}

// Some systems use the ACPI (or PM) timer which runs at 3579545Hz
// - A shift of 0 gives error of 19%   with ticks_per_us == 3
// - A shift of 1 gives error of 2%    with ticks_per_us == 7
// - A shift of 6 gives error of 0.04% with ticks_per_us == 229
// Other systems use GHz clocks
#define PERFORMANCE_COUNTER_SHIFT 6
static uint_fast32_t qpc_shift = 64; //initial value so all times are 0 until timecal_update() call

LOCALDEF uint_fast64_t timecal_update(void)
{
  LARGE_INTEGER v;
  QueryPerformanceFrequency(&v);
  if (qpc_shift == 64) {
     if (v.QuadPart < 250000000) { // if less than 250MHz then use a shift to gain some accuracy
           qpc_shift = PERFORMANCE_COUNTER_SHIFT;
     } else {
           qpc_shift = 0;
     }
  }
  ticks_per_us = (v.QuadPart << qpc_shift) / 1000000;
  return v.QuadPart;
}

uint_fast64_t now_u64(void)
{
  LARGE_INTEGER now;
  QueryPerformanceCounter(&now);
  return (now.QuadPart << qpc_shift) / ticks_per_us;
}

#endif

