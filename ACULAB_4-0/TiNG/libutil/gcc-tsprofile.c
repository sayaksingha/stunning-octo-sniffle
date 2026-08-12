#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct tsp {
	unsigned space;
	struct tsrec {
		uint64_t ts;
		void *fn;
		uint32_t code;
	} *bp, buff[1024];
	int fd, savefd;
} TSP;

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static
#ifdef _REENTRANT
	__thread
#endif
		TSP *mytsp;

static void gcctsp_flush()
{
 unsigned nw = sizeof(mytsp->buff) - mytsp->space * sizeof(mytsp->buff[0]);
 	// suppress warnings without a cast using 'nc = nw'
 int nc = mytsp->fd == -1 ? nc = nw : write(mytsp->fd, mytsp->buff, nw);
 if (nc + 0u == nw) {
	mytsp->space = arlen(mytsp->buff);
	mytsp->bp = mytsp->buff;
	return;
 }
 if (nc < 0) {
	perror("write() failed");
	fprintf(stderr, "Cannot write to profile trace file\n");
	exit(1);
 }
 fprintf(stderr, "Wrote %d of %d to profile trace file\n", nc, nw);
 exit(1);
}

struct thread {
	void *(*fn)(void *arg);
	void *arg;
};

extern char *gcctsp_filename;

void gcctsp_init()
{
 char fname[80];
 mytsp = malloc(sizeof(*mytsp));
 if (!mytsp) {
 	perror("malloc() failed");
 	exit(1);
 }
 mytsp->space = arlen(mytsp->buff);
 mytsp->bp = mytsp->buff;
 if (gcctsp_filename) {
 	sprintf(fname, gcctsp_filename, pthread_self());
 } else {
 	sprintf(fname, "gcc-thread-profile-%ld", pthread_self());
 }
 mytsp->fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0666);
 if (mytsp->fd == -1) {
 	perror("open() failed");
 	fprintf(stderr, "Cannot create profile trace file: %s\n", fname);
 	exit(1);
 }
}

void gcctsp_suspend()
{
 if (mytsp->fd != -1) {
	if (mytsp->space != arlen(mytsp->buff)) gcctsp_flush();
	mytsp->savefd = mytsp->fd;
	mytsp->fd = -1;
 }
}

void gcctsp_resume()
{
 if (mytsp->fd == -1) {
	if (mytsp->space != arlen(mytsp->buff)) gcctsp_flush();
	mytsp->fd = mytsp->savefd;
 }
}

void gcctsp_fini()
{
 if (mytsp->space != arlen(mytsp->buff)) gcctsp_flush();
 close(mytsp->fd);
}

#ifdef _REENTRANT
static void *gcctsp_run(void *arg)
{
 struct thread *tp = arg;
 struct thread thr = *tp;
 void *r;
 free(arg);
 gcctsp_init();
 r = thr.fn(thr.arg);
 gcctsp_fini();
 return r;
}

int gcctsp_pthread_create(pthread_t *tid, pthread_attr_t *attr, void *(*fn)(void *), void *arg)
{
 struct thread *tp = malloc(sizeof(*tp));
 int e;
 if (!tp) {
 	perror("malloc() failed");
 	fprintf(stderr, "Cannot allocate space for thread descriptor\n");
 	return EAGAIN;
 }
 tp->fn = fn;
 tp->arg = arg;
 e = pthread_create(tid, attr, gcctsp_run, tp);
 if (e) free(tp);
 return e;
}
#endif

static uint_fast64_t ticker(void)
{
 uint64_t val;
 __asm__ __volatile__ (
 	".byte 0xf; .byte 0x31"
 	:  "=A" (val) );
 return val;
}

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
 mytsp->bp->ts = ticker();
 mytsp->bp->fn = this_fn;
 mytsp->bp->code = 'N';
 mytsp->bp++;
 if (--mytsp->space) return;
 gcctsp_flush();
 mytsp->bp->ts = ticker();
 mytsp->bp->fn = gcctsp_flush;
 mytsp->bp->code = 'P';
 mytsp->bp++;
 --mytsp->space;
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
 mytsp->bp->ts = ticker();
 mytsp->bp->fn = this_fn;
 mytsp->bp->code = 'X';
 mytsp->bp++;
 if (--mytsp->space) return;
 gcctsp_flush();
 mytsp->bp->ts = ticker();
 mytsp->bp->fn = gcctsp_flush;
 mytsp->bp->code = 'P';
 mytsp->bp++;
 --mytsp->space;
}
