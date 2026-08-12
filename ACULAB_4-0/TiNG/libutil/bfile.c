#include "bfile.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#if 1
#define showerror(s) perror(s)
#else
#define showerror(s)
#endif

struct BFILE {
	unsigned char *buf;		// buffer
	unsigned char *bufpos;		// position in buffer
	unsigned char *bufend;		// end of buffer
	unsigned char *rlimit;		// end of input data (only for read)
	off_t bstart;			// lseek position of buf[0]
	void *p;			// open file descriptor or whatever
	int s_errno;			// had error
	int eof;			// reached EOF (only for read)
	struct {
		int (*read)(size_t *nr, void *buf, size_t bsize, void *p);
		int (*write)(size_t *nw, void *buf, size_t bsize, void *p);
		int (*seek)(off_t *pos, void *p, off_t offset, int whence);
		int (*close)(void *p);
	} op;
};

ACUAPI int bferror(BFILE *bfp)
{
 int e = bfp->s_errno;
 bfp->s_errno = 0;
 return e;
}

ACUAPI int bfflush(BFILE *bfp)
{
 if (!bfp->s_errno) {
	unsigned char *s = bfp->buf;
	if (bfp->rlimit) {
		bfp->bstart += bfp->rlimit - bfp->buf;
		bfp->rlimit = 0;
		bfp->bufpos = bfp->buf;
		bfp->eof = 0;
	}
	for (;;) {
		int rem = bfp->bufpos - s;
		size_t nw;
		if (!rem) {
			bfp->bufpos = bfp->buf;
			break;
		}
		bfp->s_errno = bfp->op.write(&nw, s, rem, bfp->p);
		if (!bfp->s_errno) {
			s += nw;
		} else {
			memmove(bfp->buf, s, rem);
			bfp->bufpos = bfp->buf + rem;
			break;
		}
 	}
	bfp->bstart += s - bfp->buf;
 }
 return bfp->s_errno;
}

static int bffill(BFILE *bfp)
{
 size_t r;
 if (bfp->eof || bfflush(bfp)) return bfp->s_errno;
 bfp->s_errno = bfp->op.read(&r, bfp->buf, bfp->bufend - bfp->buf, bfp->p);
 if (!bfp->s_errno) {
	bfp->rlimit = bfp->buf + r;
	if (!r) bfp->eof = 1;
 } else {
	bfp->rlimit = bfp->buf;
 }
 return bfp->s_errno;
}

ACUAPI int bfread(BFILE *bfp, size_t *nread, void *ptr, size_t size)
{
 *nread = 0;
 if (!bfp->rlimit && bffill(bfp)) return bfp->s_errno;
 for (; size;) {
 	size_t nr = bfp->rlimit - bfp->bufpos;
 	if (!nr) {
 		if (bffill(bfp)) break;
 		nr = bfp->rlimit - bfp->bufpos;
 		if (!nr) break;
	}
 	if (nr > size) nr = size;
 	memcpy(ptr, bfp->bufpos, nr);
 	bfp->bufpos += nr;
 	ptr = (void *) (nr + (char *) ptr);
 	size -= nr;
 	*nread += nr;
 }
 if (!*nread) bfp->eof = 0;
 return bfp->s_errno;
}

ACUAPI int bfwrite(BFILE *bfp, size_t *left, void *ptr, size_t size)
{
 if (bfp->rlimit) bfflush(bfp);
 if (!bfp->s_errno) {
	for (; size;) {
		size_t nw = bfp->bufend - bfp->bufpos;
		if (nw > size) nw = size;
		memcpy(bfp->bufpos, ptr, nw);
		bfp->bufpos += nw;
		ptr = (void *) ((char *) ptr + nw);
		size -= nw;
		if (bfp->bufpos == bfp->bufend && bfflush(bfp)) break;
	}
 }
 *left = size;
 return bfp->s_errno;
}

ACUAPI int bfseek(BFILE *bfp, off_t *pos, off_t offset, int whence)
{
 if (bfp->s_errno) return bfp->s_errno;
 if (whence == SEEK_CUR) {			// it's relative
	offset += bfp->bstart;			// make absolute
	offset += bfp->bufpos - bfp->buf;	// adjust for data read/written
	whence = SEEK_SET;
 }
 if (whence == SEEK_SET && bfp->rlimit) {
 		// see if we can just skip back or forward in the current buffer
	const off_t bufoff = offset - bfp->bstart;
	if (offset >= bfp->bstart && bufoff <= bfp->rlimit - bfp->buf) {
		bfp->bufpos = bfp->buf + bufoff;
		*pos = offset;
		return 0;
	}
 }
 if (!bfflush(bfp)) {
	bfp->s_errno = bfp->op.seek(pos, bfp->p, offset, whence);
	if (!bfp->s_errno) bfp->bstart = *pos;
 }
 return bfp->s_errno;
}

ACUAPI int bfsetup(BFILE *bfp,
		int (*readfn)(size_t *nr, void *buf, size_t bsize, void *p),
		int (*writefn)(size_t *nw, void *buf, size_t bsize, void *p),
		int (*seekfn)(off_t *pos, void *p, off_t offset, int whence),
		int (*closefn)(void *p),
		void *p)
{
 bfp->op.read = readfn;
 bfp->op.write = writefn;
 bfp->op.seek = seekfn;
 bfp->op.close = closefn;
 bfp->p = p;
 return 0;
}

static int null_close(void *p)
{
 (void) p;
 return 0;
}

ACUAPI int bfile(BFILE **bfilepp)
{
 BFILE *bfp = malloc(sizeof(*bfp));
 *bfilepp = bfp;
 if (!bfp) showerror("malloc failed");
 if (bfp) {
	const int bufsize = 1024;
	if (! (bfp->buf = malloc(bufsize)) ) {
		free(bfp);
		*bfilepp = 0;	// for safety
		showerror("malloc failed");
		return errno;
	}
	bfp->eof = bfp->s_errno = 0;
	bfp->bufpos = bfp->buf;
	bfp->bstart = 0;
	bfp->rlimit = 0;
	bfp->bufend = bfp->buf + bufsize;
	bfp->op.close = null_close;
 } else return errno;
 return 0;
}

ACUAPI int bfile_dtor(BFILE *bfp)
{
 int e;
 if (!bfp) return 0;
 if (!bfp->rlimit) bfflush(bfp);
 e = bfp->op.close(bfp->p);
 if (!e) e = bfp->s_errno;
 free(bfp->buf);
 free(bfp);
 return e;
}
