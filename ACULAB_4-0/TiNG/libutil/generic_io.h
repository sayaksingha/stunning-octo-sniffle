/* generic_io.h - operating system independent file operations */

#ifndef INCLUDED_GENERIC_IO_H
#define INCLUDED_GENERIC_IO_H

#ifdef TiNGTYPE_LINUX

typedef int OPENFILE;

#endif

#ifdef TiNGTYPE_QNX

typedef int OPENFILE;

#endif


#ifdef TiNGTYPE_WINNT

#include <windows.h>

typedef HANDLE OPENFILE;

#endif

struct device {
	struct device *next;
	char name[1];
};

typedef struct {
	struct device *root, *curdev;
} DEVLIST;

#define INKEY_NONE 256
#define INKEY_EOF 257

OPENFILE io_invalidfile(void);
int io_isinvalid(OPENFILE dp);
int io_tingdev_open(DEVLIST *dp);
char *io_tingdev_next(DEVLIST *dp);
void io_tingdev_close(DEVLIST *dp);
int io_openfile(char *name, int flags, int mode, OPENFILE *fp);
int io_readfile(OPENFILE f, void *buf, unsigned nc, unsigned *nr);
int io_writefile(OPENFILE f, void *buf, unsigned nc, unsigned *nw);
int io_ioctlfile(OPENFILE f, int cmd, void *ap, unsigned asiz);
int io_closefile(OPENFILE f);
int io_stdinfile(OPENFILE *fp);
int io_inkey(OPENFILE f);

int io_opendev_ting(OPENFILE *dp, char *cardname);
void io_formaterr(char *fn, long err);
void io_lasterr(char *fn);

#ifdef TiNGTYPE_LINUX
void Sleep(int ms);
#endif

#ifdef TiNGTYPE_QNX
void Sleep(int ms);
#endif


#endif
