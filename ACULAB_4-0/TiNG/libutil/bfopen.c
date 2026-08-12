#include "bfile.h"
#include "bfopen.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#ifdef TiNGTYPE_WINNT
#include <io.h>
#ifndef HAS_SSIZE_T
typedef long ssize_t;
#endif
#else
#include <unistd.h>
#endif

#if 1
#define showerror(s) perror(s)
#else
#define showerror(s)
#endif

static int std_read(size_t *nr, void *buf, size_t bsize, void *p)
{
 ssize_t n = read((int) p, buf, bsize);
 *nr = n;
 if (n >= 0) return 0;
 *nr = 0;
 showerror("read failed");
 return errno;
}

static int std_write(size_t *nw, void *buf, size_t bsize, void *p)
{
 ssize_t n = write((int) p, buf, bsize);
 *nw = n;
 if (n >= 0) return 0;
 *nw = 0;
 showerror("write failed");
 return errno;
}

static int std_seek(off_t *pos, void *p, off_t offset, int whence)
{
 if ((*pos = lseek((int) p, offset, whence)) != (off_t) -1) return 0;
 showerror("lseek failed");
 return errno;
}

static int std_close(void *p)
{
 if (!close((int) p)) return 0;
 showerror("close failed");
 return errno;
}

static int openfile(int *fd, char *filename, char *mode)
{
 int flags = 0;
 int otype = O_RDONLY;
 for (; *mode; mode++) {
	switch (*mode) {
		// ignore unknown codes for forward compatibility
	case 'r': otype = O_RDONLY; break;
	case 'w': otype = O_WRONLY; break;
	case 'u': otype = O_RDWR; break;
	case 't': flags |= O_TRUNC; break;
	case 'a': flags |= O_APPEND; break;
	case 'c': flags |= O_CREAT; break;
	case 'b':
	// select binary mode if operating system needs it
#ifdef O_BINARY
		flags |= O_BINARY;
#endif
			break;
	case ',':
		goto done;
	}
 }
done:
 if ((*fd = open(filename, otype | flags, 0666)) != -1) return 0;
 showerror("open failed");
 return errno;
}

ACUAPI int bfopen(BFILE *bfp, char *filename, char *mode)
{
 int fd;
 int e = openfile(&fd, filename, mode);
 if (!e) e = bfsetup(bfp, std_read, std_write, std_seek, std_close, (void *) fd);
 return e;
}
