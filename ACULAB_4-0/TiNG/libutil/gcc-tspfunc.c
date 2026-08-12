#include <pthread.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void gcctsp_free(void *p)
{
 free(p);
}

void *gcctsp_malloc(size_t size)
{
 return malloc(size);
}

int gcctsp_poll(struct pollfd *pfds, unsigned nfds, int timeout)
{
 return poll(pfds, nfds, timeout);
}

int gcctsp_pthread_cond_broadcast(pthread_cond_t *cv)
{
 return pthread_cond_broadcast(cv);
}

int gcctsp_pthread_cond_signal(pthread_cond_t *cv)
{
 return pthread_cond_signal(cv);
}

int gcctsp_pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mx)
{
 return pthread_cond_wait(cv, mx);
}

int gcctsp_pthread_mutex_lock(pthread_mutex_t *mx)
{
 return pthread_mutex_lock(mx);
}

int gcctsp_pthread_mutex_unlock(pthread_mutex_t *mx)
{
 return pthread_mutex_unlock(mx);
}

ssize_t gcctsp_read(int fd, void *buf, size_t s)
{
 return read(fd, buf, s);
}

ssize_t gcctsp_recv(int s, void *buf, size_t len, int flags)
{
 return recv(s, buf, len, flags);
}

int gcctsp_recvfrom(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen)
{
 return recvfrom(s, buf, len, flags, from, fromlen);
}

int gcctsp_select(int n, fd_set *rfd, fd_set *wfd, fd_set *xfd, struct timeval *tmo)
{
 return select(n, rfd, wfd, xfd, tmo);
}

ssize_t gcctsp_send(int s, const void *buf, size_t len, int flags)
{
 return send(s, buf, len, flags);
}

ssize_t gcctsp_sendto(int s, const void *buf, size_t len, int flags, struct sockaddr *to, socklen_t tolen)
{
 return sendto(s, buf, len, flags, to, tolen);
}

ssize_t gcctsp_write(int fd, void *buf, size_t s)
{
 return write(fd, buf, s);
}
