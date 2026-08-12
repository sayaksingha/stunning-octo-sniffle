/* event.c - implement an event mechanism */

#include "../event.h"
#include "../trace.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

STATIC int setblocking(int fd, int b)
{
 int i = fcntl(fd, F_GETFL, 0);
 if (i < 0) {
	if (TiNGtrace) print_errno(errno, "fcntl(F_GETFL) failed");
	return 1;
 }
 i |= O_NONBLOCK;
 if (b) i &= ~O_NONBLOCK;
 if (fcntl(fd, F_SETFL, i) < 0) {
	if (TiNGtrace) print_errno(errno, "fcntl(F_SETFL) failed");
	return 1;
 }
 return 0;
}

STATIC int closeonexec(int fd)
{
 int flg = fcntl(fd, F_GETFD, 0);
 if (flg < 0) {
	if (TiNGtrace) print_errno(errno, "fcntl(F_GETFD) on device failed");
	return 1;
 }
 if (fcntl(fd, F_SETFD, flg | FD_CLOEXEC) == -1) {
	if (TiNGtrace) print_errno(errno, "fcntl(F_SETFD) on device failed");
	return 1;
 }
 return 0;
}

void evnt(EVNT *ev)
{
 ev->fd[0] = ev->fd[1] = -1;
}

int signal_event(SIG_EVNT ev)
{
 int nc;
 if (ev == -1) return 0;
 nc = write(ev, "*", 1);
 if (nc == -1 && errno == EAGAIN) return 0;	// full => already set
 if (nc != 1) {
	if (TiNGtrace) print_errno(errno, "write on pipe failed");
	abort();
	return err(ERR_SM_OS_OTHER_PROBLEM);
 }
 return 0;
}

int evnt_clear(EVNT ev)
{
 char buf[64];
 int nc;
 if (evnt_to_listen(ev) == -1) return 0;
 do {
	nc = read(evnt_to_listen(ev), buf, sizeof(buf));
 } while (nc == sizeof(buf));
 if (nc == -1 && errno == EAGAIN) return 0;	// empty => clear
 if (nc < 0) {
	if (TiNGtrace) print_errno(errno, "read on pipe failed");
	return err(ERR_SM_OS_OTHER_PROBLEM);
 }
 return 0;
}

int evnt_create(EVNT *ev)
{
 if (pipe(ev->fd)) {
	if (TiNGtrace) print_errno(errno, "pipe failed");
	return err(ERR_SM_OS_RESOURCE_PROBLEM);
 }
 if (setblocking(ev->fd[0], 0)
 	|| setblocking(ev->fd[1], 0)
	|| closeonexec(ev->fd[0])
	|| closeonexec(ev->fd[1])) {
	evnt_destroy(ev);
	return err(ERR_SM_OS_OTHER_PROBLEM);
 }
 return 0;
}

int evnt_from_listen(EVNT *ev, int fd)
{
 ev->fd[0] = fd;
 return 0;
}

int evnt_valid(EVNT ev)
{
 return ev.fd[0] != -1 || ev.fd[1] != -1;
}

int evnt_assign(EVNT *ev, EVNT rhs)
{
 evnt_destroy(ev);
 if (!evnt_valid(rhs)) return 0;
 ev->fd[0] = dup(rhs.fd[0]);
 if (ev->fd[0] == -1) {
	if (TiNGtrace) print_errno(errno, "dup() failed");
	return err(ERR_SM_OS_RESOURCE_PROBLEM);
 }
 ev->fd[1] = dup(rhs.fd[1]);
 if (ev->fd[1] == -1) {
	if (TiNGtrace) print_errno(errno, "dup() failed");
	close(ev->fd[0]);
	ev->fd[0] = -1;
	return err(ERR_SM_OS_RESOURCE_PROBLEM);
 }
 if (closeonexec(ev->fd[0])
	|| closeonexec(ev->fd[1])) {
	evnt_destroy(ev);
	return err(ERR_SM_OS_OTHER_PROBLEM);
 }
 return 0;
}

void evnt_destroy(EVNT *ev)
{
 if (ev->fd[0] != -1 && close(ev->fd[0])) {
	if (TiNGtrace) print_errno(errno, "close failed");
	abort();
 }
 if (ev->fd[1] != -1 && close(ev->fd[1])) {
	if (TiNGtrace) print_errno(errno, "close failed");
	abort();
 }
 ev->fd[0] = -1;
 ev->fd[1] = -1;
}
