/* randent.c - true random number generator */

#include "randent.h"
#include "sha.h"
#include <stdint.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifdef USE_GETTIMEOFDAY
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef TiNGTYPE_QNX
#include <unistd.h>
#endif


#ifdef TiNGTYPE_WINNT
#include <windows.h>
#endif

#ifndef UINTPTR_MAX
#ifndef _UINTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED
typedef unsigned long uintptr_t;
#endif
#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static struct {
	uint_fast32_t sha_digest[SHA_HASH_BUFFER_SIZE+SHA_HASH_EXTRA_SIZE];
	uint_fast32_t in[16];
	int ninp;
	uint_fast32_t out[4];
	int nready;
	int randomness;
} entpool;

#define MAX_RANDOMNESS 100

	// Note that Linux has /dev/random, so none of this is needed
void randent_init(void)
{
 entpool.randomness = 0;
 randent_addrand(0, 0);
#ifdef TiNGTYPE_QNX
 {
  int pid = getpid();
  randent_addrand(&pid, sizeof(pid));
 }
#endif
#ifdef TiNGTYPE_WINNT
 {
  DWORD pid = GetCurrentProcessId();
  randent_addrand(&pid, sizeof(pid));
 }
 {
  char buf[1024];
  randent_addrand(buf, GetCurrentDirectory(sizeof(buf), buf));
 }
 {
  DWORD pid = GetCurrentThreadId();
  randent_addrand(&pid, sizeof(pid));
 }
 {
  POINT pt;
  GetCursorPos(&pt);
  randent_addrand(&pt, sizeof(pt));
 }
 {
  struct {
  	DWORD spc, bps, nfc, nc;
  } df;
  GetDiskFreeSpace(".", &df.spc, &df.bps, &df.nfc, &df.nc);
  randent_addrand(&df, sizeof(df));
 }
 {
  struct {
  	ULARGE_INTEGER av, tot, fr;
  } df;
  GetDiskFreeSpaceEx(".", &df.av, &df.tot, &df.fr);
  randent_addrand(&df, sizeof(df));
 }
 {
  char *s = GetEnvironmentStrings();
  char *e;
  for (e=s; *e; ) {	// each string
  	for (; *++e; ) ;	// each char
  }
  randent_addrand(s, (unsigned)(e - s));
 }
 {
  char buf[1024];
  randent_addrand(buf, GetSystemDirectory(buf, sizeof(buf)));
 }
 {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  randent_addrand(&si, sizeof(si));
 }
 {
  struct {
  	FILETIME ct, et, kt, ut;
  } tt;
  GetThreadTimes(GetCurrentThread(), &tt.ct, &tt.et, &tt.kt, &tt.ut);
  randent_addrand(&tt, sizeof(tt));
 }
 {
  DWORD t = GetTickCount();
  randent_addrand(&t, sizeof(t));
 }
 {
  struct {
  	char name[1024], fsname[64];
	DWORD vsn, mcl, fsf;
  } vi;
  GetVolumeInformation(0, vi.name, sizeof(vi.name), &vi.vsn, &vi.mcl, &vi.fsf,
	  vi.fsname, sizeof(vi.fsname));
  randent_addrand(&vi, sizeof(vi));
 }
 {
  char buf[1024];
  randent_addrand(buf, GetWindowsDirectory(buf, sizeof(buf)));
 }
 {
  MEMORYSTATUS ms;
  ms.dwLength = sizeof(ms);
  GlobalMemoryStatus(&ms);
  randent_addrand(&ms, sizeof(ms));
 }
#endif
}

unsigned long randent32(void)
{
 if (entpool.randomness) entpool.randomness--;
 if (entpool.nready <= 0) {
	int i;
	entpool.in[0] ^= entpool.ninp;
	SHATransform(entpool.sha_digest, entpool.in);
	for (i=0; i < 4; i++) {
		entpool.in[i]
			^= entpool.out[i]
				^= entpool.sha_digest[i]
					^ entpool.sha_digest[4];
	}
	entpool.nready = 4;
 }
 return entpool.out[--entpool.nready & 3];
}

void randent_addrand(void *d, unsigned bytes)
{
 unsigned char *data = d;
 uint32_t dp = (uint32_t)((uintptr_t) d);
 if (entpool.randomness >= MAX_RANDOMNESS) return;	// save CPU time
 entpool.randomness++;
 for (; bytes; bytes--) {
	((unsigned char *) entpool.in)[entpool.ninp++ & 0xf] ^= *data++;
 }
#ifdef RAND_USE_GETTIMEOFDAY
 {
  struct timeval tod;
  gettimeofday(&tod, 0);
  data = &tod;
  bytes = sizeof(tod);
  for (; bytes; bytes--) {
	((unsigned char *) entpool.in)[entpool.ninp++ & 0xf] ^= *data++;
  }
 }
#endif
#ifdef TiNGTYPE_WINNT
 {
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  data = (unsigned char *) &ft;
  bytes = sizeof(ft);
  for (; bytes; bytes--) {
	((unsigned char *) entpool.in)[entpool.ninp++ & 0xf] ^= *data++;
  }
 }
#endif
 dp ^= dp >> 16;
 dp ^= dp >> 8;
 ((unsigned char *) entpool.in)[entpool.ninp++ & 0xf] ^= dp;
}
