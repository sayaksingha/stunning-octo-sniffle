/* aculog.h - ACULAB standard logging */

#ifndef INCLUDED_ACULOG_H
#define INCLUDED_ACULOG_H

#ifdef ___GNUC__
void aculog(char *fmt, ...) __attribute__((format (printf, 1, 2)));
#else
void aculog(char *fmt, ...);
#endif

int aculog_init_fd(int fd, char *tag);
int aculog_init(char *addr, char *port, char *tag);
void aculog_close(void);

#endif
