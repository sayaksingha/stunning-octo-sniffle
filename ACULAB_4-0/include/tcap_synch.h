/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/tcap/api/tcap_synch.h,v 1.33 2020/10/12 09:23:03 davidla Exp $ $Name: SS7_6_16_1 $" */

#ifndef ACU_TCAP_SYNCH_H
#define ACU_TCAP_SYNCH_H


/*
 * Define an appropiate prefix for library function definitions,
 * in order to exclude internal functions from the library symbol table
 * and to ensure that the library calls its own public functions.
 *
 * The library itself is compiled with -fvisibility=hidden and
 * -DACU_TCAP_LIB_API=__attribute__((visibility("protected")))
 */

#ifndef ACU_TCAP_LIB_API
#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100) >= 30400
#define ACU_TCAP_LIB_API __attribute__((visibility("default")))
#else
#define ACU_TCAP_LIB_API
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
} acu_tcap_mutex_t;

typedef struct {
    HANDLE         atc_event[2];      /* Manual reset events */
    unsigned int   atc_count[2];      /* Number of waiters on each event */
    unsigned int   atc_active;        /* Event to wait on */
    unsigned int   atc_pending;       /* Broadcast while still draining */
} acu_tcap_cond_t;

typedef struct {
    HANDLE        att_handle;
    unsigned int  att_thrd_id;
} acu_tcap_thrd_id_t;

ACU_TCAP_LIB_API int acu_tcap_mutex_recurse(acu_tcap_mutex_t *, const char *,
        int);
#define acu_tcap_mutex_create(mutex) \
        ((mutex)->atm_line = 0, \
            InitializeCriticalSection(&(mutex)->atm_lock), 0)
#define acu_tcap_mutex_delete(mutex) DeleteCriticalSection(&(mutex)->atm_lock)
#define acu_tcap_mutex_check_recurse(mutex, file, line) \
          ((mutex)->atm_line && acu_tcap_mutex_recurse(mutex, file, line), \
          (mutex)->atm_file = file, (mutex)->atm_line = line, 0)
#define acu_tcap_mutex_lock(mutex)    \
        (EnterCriticalSection(&(mutex)->atm_lock), \
          acu_tcap_mutex_check_recurse(mutex, __FILE__, __LINE__))
#define acu_tcap_mutex_trylock(mutex) \
        (!TryEnterCriticalSection&((mutex)->atm_lock) ? -1 : \
          acu_tcap_mutex_check_recurse(mutex, __FILE__, __LINE__))
#define acu_tcap_mutex_unlock(mutex) \
        ((mutex)->atm_line = 0, LeaveCriticalSection(&(mutex)->atm_lock))

ACU_TCAP_LIB_API int acu_tcap_condvar_create(acu_tcap_cond_t *);
ACU_TCAP_LIB_API void acu_tcap_condvar_delete(acu_tcap_cond_t *);
#define acu_tcap_condvar_wait(cv, mutex) \
        acu_tcap_condvar_wait_tmo(cv, mutex, INFINITE)
ACU_TCAP_LIB_API int acu_tcap_condvar_wait_tmo(acu_tcap_cond_t *,
        acu_tcap_mutex_t *, int);
ACU_TCAP_LIB_API void acu_tcap_condvar_broadcast(acu_tcap_cond_t *);

#define ACU_TCAP_THREAD_FN(fn, arg) unsigned int (__stdcall fn)(void *arg)
#define acu_tcap_thread_create(id, fn, arg) \
        (-!((id)->att_handle = (HANDLE)_beginthreadex(NULL, 0, fn, arg, 0, \
            &(id)->att_thrd_id)))
#define acu_tcap_thread_exit(detach, rval) \
        if (detach) _endthread(); else _endthreadex(rval)
#define acu_tcap_thread_join(id, rvalp) \
        (WaitForSingleObject((id)->att_handle, INFINITE) && ( \
        (rvalp) && GetExitCodeThread((id)->att_handle, rvalp), \
        CloseHandle((id)->att_handle)))
#define acu_tcap_thread_id() GetCurrentThreadId()

#else
/* We use pthreads on and Linux */
#include <pthread.h>

typedef struct {
    pthread_mutex_t    atm_mutex;
} acu_tcap_mutex_t;

typedef struct {
    pthread_cond_t     atc_cond;
} acu_tcap_cond_t;


typedef struct {
    pthread_t  att_thrd_id;
    void       *att_thrd_rval;
} acu_tcap_thrd_id_t;

#define acu_tcap_mutex_create(mutex) pthread_mutex_init(&(mutex)->atm_mutex, 0)
#define acu_tcap_mutex_delete(mutex) pthread_mutex_destroy(&(mutex)->atm_mutex)
#define acu_tcap_mutex_lock(mutex) pthread_mutex_lock(&(mutex)->atm_mutex)
#define acu_tcap_mutex_trylock(mutex) pthread_mutex_trylock(&(mutex)->atm_mutex)
#define acu_tcap_mutex_unlock(mutex) pthread_mutex_unlock(&(mutex)->atm_mutex)

#define acu_tcap_condvar_create(cv) pthread_cond_init(&(cv)->atc_cond, NULL)
#define acu_tcap_condvar_delete(cv) pthread_cond_destroy(&(cv)->atc_cond)
#define acu_tcap_condvar_wait(cv, mutex) \
        pthread_cond_wait(&(cv)->atc_cond, &(mutex)->atm_mutex)
/* We have to convert the timeout interval to an absolute time before
 * calling pthread_cond_timedwait. */
ACU_TCAP_LIB_API int acu_tcap_condvar_wait_tmo(acu_tcap_cond_t *,
        acu_tcap_mutex_t *, int);
#define acu_tcap_condvar_broadcast(cv) pthread_cond_broadcast(&(cv)->atc_cond)

#define ACU_TCAP_THREAD_FN(fn, arg) void *(fn)(void *arg)
#define acu_tcap_thread_create(id, fn, arg) \
        pthread_create(&(id)->att_thrd_id, NULL, fn, arg)
#define acu_tcap_thread_exit(detach, rval) \
        ((void)((detach) && pthread_detach(pthread_self())), \
        pthread_exit((void *)(rval)))
#define acu_tcap_thread_join(id, rvalp) \
        (-(pthread_join((id)->att_thrd_id, &(id)->att_thrd_rval) || \
        (((rvalp) && (*(rvalp) = (unsigned int)(id)->att_thrd_rval)), 0)))
#define acu_tcap_thread_id() pthread_self()

#endif

/* Thread pool functions */

#define ACU_TCAP_THREAD_POOL_F_BLOCKING  (1u << 0)
#define ACU_TCAP_THREAD_POOL_F_NO_JOB    (1u << 1)

typedef struct acu_tcap_thrd_pool acu_tcap_thrd_pool_t;
ACU_TCAP_LIB_API acu_tcap_thrd_pool_t *acu_tcap_thread_pool_create(
        unsigned int, unsigned int, unsigned int);
ACU_TCAP_LIB_API int acu_tcap_thread_pool_destroy(acu_tcap_thrd_pool_t *);
ACU_TCAP_LIB_API unsigned int acu_tcap_thread_pool_num_active(
        acu_tcap_thrd_pool_t *);
ACU_TCAP_LIB_API unsigned int acu_tcap_thread_pool_num_idle(
        acu_tcap_thrd_pool_t *);
ACU_TCAP_LIB_API unsigned int acu_tcap_thread_pool_num_jobs(
        acu_tcap_thrd_pool_t *);
ACU_TCAP_LIB_API int acu_tcap_thread_pool_submit(acu_tcap_thrd_pool_t *,
        void (*)(void *), void *, unsigned int);

#endif
