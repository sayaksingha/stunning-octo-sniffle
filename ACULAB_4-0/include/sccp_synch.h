/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/sccp_lib/sccp_synch.h,v 1.3 2024/11/15 09:18:39 davidla Exp $ $Name: SS7_6_17_0 $" */

#ifndef ACU_SCCP_SYNCH_H
#define ACU_SCCP_SYNCH_H

#ifndef ACU_SCCP_LIB_API
#ifdef _WIN32
#define ACU_SCCP_LIB_API __declspec(dllimport)
#else
#define ACU_SCCP_LIB_API __attribute__((visibility("default")))
#endif
#endif

/* Wrappers to allow common source between Windows and Linux */

#ifdef _WIN32
/* We require the caller to #include <windows.h> */
#include <process.h>

/* We don't allow recursive acquistion under pthreads, so detect it here */
typedef struct {
    CRITICAL_SECTION atm_lock;
    const char       *atm_file;
    unsigned int     atm_line;
} acu_sccp_mutex_t;

typedef struct {
    HANDLE         atc_event[2];      /* Manual reset events */
    unsigned int   atc_count[2];      /* Number of waiters on each event */
    unsigned int   atc_active;        /* Event to wait on */
    unsigned int   atc_pending;       /* Broadcast while still draining */
} acu_sccp_cond_t;

typedef struct {
    HANDLE        att_handle;
    unsigned int  att_thrd_id;
} acu_sccp_thrd_id_t;

ACU_SCCP_LIB_API int acu_sccp_mutex_recurse(acu_sccp_mutex_t *, const char *,
        int);
#define acu_sccp_mutex_create(mutex) \
        ((mutex)->atm_line = 0, \
            InitializeCriticalSection(&(mutex)->atm_lock), 0)
#define acu_sccp_mutex_delete(mutex) DeleteCriticalSection(&(mutex)->atm_lock)
#define acu_sccp_mutex_check_recurse(mutex, file, line) \
          ((mutex)->atm_line && acu_sccp_mutex_recurse(mutex, file, line), \
          (mutex)->atm_file = file, (mutex)->atm_line = line, 0)
#define acu_sccp_mutex_lock(mutex)    \
        (EnterCriticalSection(&(mutex)->atm_lock), \
          acu_sccp_mutex_check_recurse(mutex, __FILE__, __LINE__))
#define acu_sccp_mutex_trylock(mutex) \
        (!TryEnterCriticalSection&((mutex)->atm_lock) ? -1 : \
          acu_sccp_mutex_check_recurse(mutex, __FILE__, __LINE__))
#define acu_sccp_mutex_unlock(mutex) \
        ((mutex)->atm_line = 0, LeaveCriticalSection(&(mutex)->atm_lock))

ACU_SCCP_LIB_API int acu_sccp_condvar_create(acu_sccp_cond_t *);
ACU_SCCP_LIB_API void acu_sccp_condvar_delete(acu_sccp_cond_t *);
#define acu_sccp_condvar_wait(cv, mutex) \
        acu_sccp_condvar_wait_tmo(cv, mutex, INFINITE)
ACU_SCCP_LIB_API int acu_sccp_condvar_wait_tmo(acu_sccp_cond_t *,
        acu_sccp_mutex_t *, int);
ACU_SCCP_LIB_API void acu_sccp_condvar_broadcast(acu_sccp_cond_t *);

#define ACU_SCCP_THREAD_FN(fn, arg) unsigned int (__stdcall fn)(void *arg)
#define acu_sccp_thread_create(id, fn, arg) \
        (-!((id)->att_handle = (HANDLE)_beginthreadex(NULL, 0, fn, arg, 0, \
            &(id)->att_thrd_id)))
#define acu_sccp_thread_exit(detach, rval) \
        if (detach) _endthread(); else _endthreadex(rval)
#define acu_sccp_thread_join(id, rvalp) \
        (WaitForSingleObject((id)->att_handle, INFINITE) && ( \
        (rvalp) && GetExitCodeThread((id)->att_handle, rvalp), \
        CloseHandle((id)->att_handle)))
#define acu_sccp_thread_id() GetCurrentThreadId()

#else
/* We use pthreads on Linux */
#include <pthread.h>

typedef struct {
    pthread_mutex_t    atm_mutex;
} acu_sccp_mutex_t;

typedef struct {
    pthread_cond_t     atc_cond;
} acu_sccp_cond_t;


typedef struct {
    pthread_t  att_thrd_id;
    void       *att_thrd_rval;
} acu_sccp_thrd_id_t;

#define acu_sccp_mutex_create(mutex) pthread_mutex_init(&(mutex)->atm_mutex, 0)
#define acu_sccp_mutex_delete(mutex) pthread_mutex_destroy(&(mutex)->atm_mutex)
#define acu_sccp_mutex_lock(mutex) pthread_mutex_lock(&(mutex)->atm_mutex)
#define acu_sccp_mutex_trylock(mutex) pthread_mutex_trylock(&(mutex)->atm_mutex)
#define acu_sccp_mutex_unlock(mutex) pthread_mutex_unlock(&(mutex)->atm_mutex)

#define acu_sccp_condvar_create(cv) pthread_cond_init(&(cv)->atc_cond, NULL)
#define acu_sccp_condvar_delete(cv) pthread_cond_destroy(&(cv)->atc_cond)
#define acu_sccp_condvar_wait(cv, mutex) \
        pthread_cond_wait(&(cv)->atc_cond, &(mutex)->atm_mutex)
/* We have to convert the timeout interval to an absolute time before
 * calling pthread_cond_timedwait. */
ACU_SCCP_LIB_API int acu_sccp_condvar_wait_tmo(acu_sccp_cond_t *,
        acu_sccp_mutex_t *, int);
#define acu_sccp_condvar_broadcast(cv) pthread_cond_broadcast(&(cv)->atc_cond)

#define ACU_SCCP_THREAD_FN(fn, arg) void *(fn)(void *arg)
#define acu_sccp_thread_create(id, fn, arg) \
        pthread_create(&(id)->att_thrd_id, NULL, fn, arg)
#define acu_sccp_thread_exit(detach, rval) \
        ((void)((detach) && pthread_detach(pthread_self())), \
        pthread_exit((void *)(rval)))
#define acu_sccp_thread_join(id, rvalp) \
        (-(pthread_join((id)->att_thrd_id, &(id)->att_thrd_rval) || \
        (((rvalp) && (*(rvalp) = (unsigned int)(id)->att_thrd_rval)), 0)))
#define acu_sccp_thread_id() pthread_self()

#endif

#endif
