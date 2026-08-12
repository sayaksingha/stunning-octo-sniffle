/*------------------------------------------------------------*/
/* (C)2002-2003 ACULAB plc                                    */
/*------------------------------------------------------------*/
/*                                                            */
/*                                                            */
/* Program File Name : acu_type.h                             */
/*                                                            */
/*           Purpose : V6 Common Declarations                 */
/*                                                            */
/*       Create Date : 23th March 2004                        */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/
/*                                                            */
/*                                                            */
/* Change History                                             */
/*                                                            */
/* rev: 6.1.0      23/04/2003 Initial Version                 */
/*                                                            */
/*                                                            */
/*------------------------------------------------------------*/


#ifndef ACU_TYPE_H_INCLUDED
#define ACU_TYPE_H_INCLUDED

#ifndef __KERNEL__
#	ifndef ACU_QNX
#		include <string.h>
#	endif
#endif

#ifdef __linux__
#	ifndef ACU_LINUX
#		define ACU_LINUX
#	endif
#	ifdef _LP64
#		ifndef ACU_LINUX_64
#			define ACU_LINUX_64
#		endif
#	endif
#endif
#ifdef __QNX__
#	ifndef ACU_QNX
#		define ACU_QNX
#	endif
#endif

#ifndef TiNG_USE_V6
    #define TiNG_USE_V6
#endif

/* Determine appropriate method to mark functions as deprecated */
/* Deprecation only supported in MSVC compiler v13.00 (aka Visual C++ .NET) and above */
#if defined(_MSC_VER) && _MSC_VER > 1200
	#define ACU_DEPRECATED __declspec(deprecated) 
#elif  defined(__GNUC__)
	#define ACU_DEPRECATED __attribute__((deprecated))
#else
	#define ACU_DEPRECATED
#endif

/* For compatibility with 6.7 stripe headers */
#define ACU_DEPRECATE_PRE ACU_DEPRECATED
#define ACU_DEPRECATE_POST

#ifdef _WIN32
	#ifndef ACU_DONT_USE_TING_DLL
		#ifndef TiNG_USE_DLL
	        #define TiNG_USE_DLL
		#endif
	#endif

    #ifndef TiNGTYPE_WINNT
        #define TiNGTYPE_WINNT
    #endif

    #ifndef DRIVER
		/* Streamline what we get from windows.h */
		#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
		#define ACU_TYPE_WIN32_LEAN_AND_MEAN
		#endif

        #include <windows.h>

		#ifdef ACU_TYPE_WIN32_LEAN_AND_MEAN
		/* If we turned WIN32_LEAN_AND_MEAN on, turn it off again */
		#undef WIN32_LEAN_AND_MEAN
		#endif

        typedef HANDLE ACU_WAIT_OBJECT;
    #else
	typedef struct { int sdjkdjdj; } ACU_WAIT_OBJECT;  // Dummy for userspace structs
    #endif /* DRIVER */
    typedef unsigned __int64  ACU_64BIT_WAIT_OBJECT;  // Contains the value of a HANDLE
#else /* not Windows */

        #ifdef __linux__
            #ifndef TiNGTYPE_LINUX
            #define TiNGTYPE_LINUX
	    #endif
        #endif

	#ifdef __powerpc__
	    #ifndef TiNGTYPE_PXPPC
	    #define TiNGTYPE_PXPPC
	    #endif
	#endif
       
	#ifndef __KERNEL__
		#include <sys/poll.h>
		#include <pthread.h>

	    typedef struct _ACU_WAIT_OBJECT
	    {
	        int              event_fds[2];
	        struct pollfd    event_pfd;
	        int              active;
	        int              resettable;
	        int              last_event;
	        int              last_fired;
			void			*cs_lock;
	    } ACU_WAIT_OBJECT;
	#else
		typedef struct { int sdjkdjdj; } ACU_WAIT_OBJECT;  // Dummy for userspace structs
	#endif

#endif

#ifdef __GNUC__
// Libraries can be build with -fvisibility=hidden.
// ACU_DLL_IMPORT then needed to access functions in a different library.
#define ACU_DLL_EXPORT __attribute__((visibility("protected")))	// Shared library build
#define ACU_DLL_IMPORT __attribute__((visibility("default")))	// Released header
#define ACU_EXPORT
#else
#define ACU_DLL_EXPORT __declspec(dllexport)	// Shared library build, expose symbol
#define ACU_DLL_IMPORT __declspec(dllimport)	// Released header, optimised call.
#define ACU_EXPORT WINAPI                      	// Calling convention (32bit only, 'symbol@n')
#endif

#ifndef ACU_LINUX
   #ifndef ACU_QNX
       #pragma pack ( push, 1 )
       #define ACU_PACK_DIRECTIVE
   #else
       #define ACU_PACK_DIRECTIVE __attribute__ ((packed))
   #endif
#else
   #define ACU_PACK_DIRECTIVE __attribute__ ((packed))
#endif /* ACU_LINUX */



/*------------ data type definitions ---------------*/
/*                                                  */
/* The device driver makes the following assumptions*/
/* about the data types char, int and long          */
/*                                                  */
/*             char:  8 bit, signed                 */
/*             int:  32 bit, signed                 */
/*             long: 32 bit, signed                 */
/*                                                  */
/* The libraries have been modified to use the      */
/* typedef's provided below. If you wish to change  */
/* your environment, then modify the typedef's to   */
/* suit.                                            */
/*                                                  */


/*-------------------------------------------------------*/
/*                                                       */
/* for Windows NT/2000/XP                                */
/* bit compilers use the following typedefs:             */
/*                                                       */
/*-------------------------------------------------------*/

#if defined( _WIN32 )

	#ifndef _BUFP_
	    typedef char  * BUFP;                 /* obsolete (pre-V5) */
	    #define _BUFP_
	#endif


	#ifndef _ACU_CHAR_
	    typedef char  ACU_CHAR;               /* signed 8 bit    */
	    #define _ACU_CHAR_
	#endif

	#ifndef _ACU_UCHAR_
	    typedef unsigned char ACU_UCHAR;      /* unsigned 8 bit  */
	    #define _ACU_UCHAR_
	#endif

	#ifndef _ACU_SHORT_
	    typedef short  ACU_SHORT;             /* unsigned 16 bit */
	    #define _ACU_SHORT_
	#endif

	#ifndef _ACU_USHORT_
	    typedef unsigned short  ACU_USHORT;   /* unsigned 16 bit */
	    #define _ACU_USHORT_
	#endif

	#ifndef _ACU_INT_
	    typedef int   ACU_INT;                /* signed 32 bit   */
	    #define _ACU_INT_
	#endif

	#ifndef _ACU_LONG_
	    typedef long  ACU_LONG;               /* signed 32 bit   */
	    #define _ACU_LONG_
	#endif

	#ifndef _ACU_UINT_
	    typedef unsigned int ACU_UINT;        /* unsigned 32 bit */
	    #define _ACU_UINT_
	#endif

	#ifndef _ACU_ULONG_
	    typedef unsigned long  ACU_ULONG;     /* unsigned 32 bit */
	    #define _ACU_ULONG_
	#endif

	#ifndef _ACU_ULONGLONG_
		#ifdef _MSC_VER
			typedef unsigned __int64 ACU_ULONGLONG;              /* unsigned 64 bit */
		#else
			typedef unsigned long long ACU_ULONGLONG;            /* unsigned 64 bit */
		#endif
		#define _ACU_ULONGLONG_
	#endif

	#ifndef _ACU_POINTER_
		typedef ACU_ULONGLONG ACU_POINTER;		/* 64-bit pointer */
		#define _ACU_POINTER_
	#endif

	#ifndef _ACU_LONGLONG_
           #ifdef _MSC_VER
	      typedef __int64 ACU_LONGLONG;              /* signed 64 bit */
           #else
              typedef long long ACU_LONGLONG;            /* signed 64 bit */
           #endif
	   #define _ACU_LONGLONG_
	#endif

	#ifndef _ACU_TOKEN_
	    typedef unsigned long ACU_TOKEN;        /* unsigned 32 bit */
	    #define _ACU_TOKEN_
	#endif

	#ifndef _ACU_ACT_
	    typedef ACU_POINTER ACU_ACT;        /* unsigned 64 bit */
	    #define _ACU_ACT_
	#endif
	
	#ifndef _ACU_COMPAT_ACT_
	    typedef unsigned long ACU_COMPAT_ACT;   /* unsigned 32 bit */
	    #define _ACU_COMPAT_ACT_
	#endif
	
	/* New types for V6 */

	#ifndef _ACU_CARD_ID_
	    typedef ACU_UINT ACU_CARD_ID;
	    #define _ACU_CARD_ID_
	#endif

	#ifndef _ACU_PORT_ID_
	    typedef ACU_ULONG ACU_PORT_ID; /* port id */
	    #define _ACU_PORT_ID_
	#endif

	#ifndef _ACU_CALL_HANDLE_
	    typedef ACU_ULONG ACU_CALL_HANDLE;  /* handle */
	    #define _ACU_CALL_HANDLE_
	#endif

	#ifndef _ACU_ERR_
	    typedef ACU_INT ACU_ERR; /* error code type */
	    #define _ACU_ERR_
	#endif

	#ifndef _ACU_MODULE_ID_
	    typedef ACU_UINT ACU_MODULE_ID;
	    #define _ACU_MODULE_ID_
	#endif

	#ifndef _ACU_RESOURCE_ID_
	    typedef ACU_UINT ACU_RESOURCE_ID;
	    #define _ACU_RESOURCE_ID_
	#endif

	#ifndef _ACU_EVENT_QUEUE_
	    typedef ACU_UINT ACU_EVENT_QUEUE;
	    #define _ACU_EVENT_QUEUE_
	#endif
#elif defined( ACU_LINUX )

	#ifndef _BUFP_
	    typedef char  * BUFP;                 /* obsolete (pre-V5) */
	    #define _BUFP_
	#endif


	#ifndef _ACU_CHAR_
	    typedef char  ACU_CHAR;               /* signed 8 bit    */
	    #define _ACU_CHAR_
	#endif

	#ifndef _ACU_UCHAR_
	    typedef unsigned char ACU_UCHAR;      /* unsigned 8 bit  */
	    #define _ACU_UCHAR_
	#endif
#ifdef __KERNEL__
	#include <linux/types.h>

	#ifndef _ACU_SHORT_
	    typedef __s16  ACU_SHORT;             /* unsigned 16 bit */
	    #define _ACU_SHORT_
	#endif

	#ifndef _ACU_USHORT_
	    typedef __u16  ACU_USHORT;   /* unsigned 16 bit */
	    #define _ACU_USHORT_
	#endif

	#ifndef _ACU_INT_
	    typedef __s32   ACU_INT;                /* signed 32 bit   */
	    #define _ACU_INT_
	#endif

	#ifndef _ACU_LONG_
	    typedef __u32  ACU_LONG;               /* signed 32 bit   */
	    #define _ACU_LONG_
	#endif

	#ifndef _ACU_UINT_
	    typedef __u32 ACU_UINT;        /* unsigned 32 bit */
	    #define _ACU_UINT_
	#endif

	#ifndef _ACU_ULONG_
	    typedef __u32  ACU_ULONG;     /* unsigned 32 bit */
	    #define _ACU_ULONG_
	#endif

	#ifndef _ACU_ULONGLONG_
		typedef __u64 ACU_ULONGLONG;   /* unsigned 64 bit */
	    #define _ACU_ULONGLONG_
    	#endif

	#ifndef _ACU_LONGLONG_
		typedef __s64 ACU_LONGLONG;     /* signed 64 bit */
	    #define _ACU_LONGLONG_
	#endif
#else
	#include <stdint.h>

	#ifndef _ACU_SHORT_
	    typedef int16_t  ACU_SHORT;             /* unsigned 16 bit */
	    #define _ACU_SHORT_
	#endif

	#ifndef _ACU_USHORT_
	    typedef uint16_t  ACU_USHORT;   /* unsigned 16 bit */
	    #define _ACU_USHORT_
	#endif

	#ifndef _ACU_INT_
	    typedef int32_t   ACU_INT;                /* signed 32 bit   */
	    #define _ACU_INT_
	#endif

	#ifndef _ACU_LONG_
	    typedef int32_t  ACU_LONG;               /* signed 32 bit   */
	    #define _ACU_LONG_
	#endif

	#ifndef _ACU_UINT_
	    typedef uint32_t ACU_UINT;        /* unsigned 32 bit */
	    #define _ACU_UINT_
	#endif

	#ifndef _ACU_ULONG_
	    typedef uint32_t  ACU_ULONG;     /* unsigned 32 bit */
	    #define _ACU_ULONG_
	#endif

	#ifndef _ACU_ULONGLONG_
		typedef uint64_t ACU_ULONGLONG;   /* unsigned 64 bit */
	    #define _ACU_ULONGLONG_
    	#endif

	#ifndef _ACU_LONGLONG_
		typedef int64_t ACU_LONGLONG;     /* signed 64 bit */
	    #define _ACU_LONGLONG_
	#endif

#endif

	#ifndef _ACU_POINTER_
		typedef ACU_ULONGLONG ACU_POINTER;		/* 64-bit pointer */
		#define _ACU_POINTER_
	#endif

	#ifndef _ACU_TOKEN_
	    typedef ACU_ULONG ACU_TOKEN;        /* unsigned 32 bit */
	    #define _ACU_TOKEN_
	#endif

	#ifndef _ACU_ACT_
	    typedef ACU_ULONGLONG ACU_ACT;        /* unsigned 64 bit */
	    #define _ACU_ACT_
	#endif

	#ifndef _ACU_COMPAT_ACT_
	    typedef ACU_ULONG ACU_COMPAT_ACT;        /* unsigned 32 bit */
	    #define _ACU_COMPAT_ACT_
	#endif

	/* New types for V6 */

	#ifndef _ACU_CARD_ID_
	    typedef ACU_UINT ACU_CARD_ID;
	    #define _ACU_CARD_ID_
	#endif

	#ifndef _ACU_PORT_ID_
	    typedef ACU_ULONG ACU_PORT_ID; /* port id */
	    #define _ACU_PORT_ID_
	#endif

	#ifndef _ACU_CALL_HANDLE_
	    typedef ACU_ULONG ACU_CALL_HANDLE;  /* handle */
	    #define _ACU_CALL_HANDLE_
	#endif

	#ifndef _ACU_ERR_
	    typedef ACU_INT ACU_ERR; /* error code type */
	    #define _ACU_ERR_
	#endif

	#ifndef _ACU_MODULE_ID_
	    typedef ACU_UINT ACU_MODULE_ID;
	    #define _ACU_MODULE_ID_
	#endif

	#ifndef _ACU_RESOURCE_ID_
	    typedef ACU_UINT ACU_RESOURCE_ID;
	    #define _ACU_RESOURCE_ID_
	#endif

	#ifndef _ACU_EVENT_QUEUE_
	    typedef ACU_UINT ACU_EVENT_QUEUE;
	    #define _ACU_EVENT_QUEUE_
	#endif
#elif defined ( ACU_QNX )
	#include <stdint.h>

	#ifndef _BUFP_
	    typedef char  * BUFP;                 /* obsolete (pre-V5) */
	    #define _BUFP_
	#endif


	#ifndef _ACU_CHAR_
	    typedef char  ACU_CHAR;               /* signed 8 bit    */
	    #define _ACU_CHAR_
	#endif

	#ifndef _ACU_UCHAR_
	    typedef unsigned char ACU_UCHAR;      /* unsigned 8 bit  */
	    #define _ACU_UCHAR_
	#endif

	#ifndef _ACU_SHORT_
	    typedef short  ACU_SHORT;             /* unsigned 16 bit */
	    #define _ACU_SHORT_
	#endif

	#ifndef _ACU_USHORT_
	    typedef unsigned short  ACU_USHORT;   /* unsigned 16 bit */
	    #define _ACU_USHORT_
	#endif

	#ifndef _ACU_INT_
	    typedef int   ACU_INT;                /* signed 32 bit   */
	    #define _ACU_INT_
	#endif

	#ifndef _ACU_LONG_
	    typedef long  ACU_LONG;               /* signed 32 bit   */
	    #define _ACU_LONG_
	#endif

	#ifndef _ACU_UINT_
	    typedef unsigned int ACU_UINT;        /* unsigned 32 bit */
	    #define _ACU_UINT_
	#endif

	#ifndef _ACU_ULONG_
	    typedef unsigned long  ACU_ULONG;     /* unsigned 32 bit */
	    #define _ACU_ULONG_
	#endif

	#ifndef _ACU_ULONGLONG_
		typedef uint64_t ACU_ULONGLONG;   /* unsigned 64 bit */
	    #define _ACU_ULONGLONG_
    	#endif

	#ifndef _ACU_LONGLONG_
		typedef int64_t ACU_LONGLONG;     /* signed 64 bit */
	    #define _ACU_LONGLONG_
	#endif

	#ifndef _ACU_POINTER_
		typedef uint64_t ACU_POINTER;		/* 64-bit pointer */
		#define _ACU_POINTER_
	#endif

	#ifndef _ACU_TOKEN_
	    typedef unsigned long ACU_TOKEN;        /* unsigned 32 bit */
	    #define _ACU_TOKEN_
	#endif

	#ifndef _ACU_ACT_
	    typedef ACU_POINTER ACU_ACT;            /* unsigned 64 bit */
	    #define _ACU_ACT_
	#endif
	
	#ifndef _ACU_COMPAT_ACT_
	    typedef ACU_POINTER ACU_COMPAT_ACT;     /* unsigned 64 bit */
	    #define _ACU_COMPAT_ACT_
	#endif
	/* New types for V6 */

	#ifndef _ACU_CARD_ID_
	    typedef ACU_UINT ACU_CARD_ID;
	    #define _ACU_CARD_ID_
	#endif

	#ifndef _ACU_PORT_ID_
	    typedef ACU_ULONG ACU_PORT_ID; /* port id */
	    #define _ACU_PORT_ID_
	#endif

	#ifndef _ACU_CALL_HANDLE_
	    typedef ACU_ULONG ACU_CALL_HANDLE;  /* handle */
	    #define _ACU_CALL_HANDLE_
	#endif

	#ifndef _ACU_ERR_
	    typedef ACU_INT ACU_ERR; /* error code type */
	    #define _ACU_ERR_
	#endif

	#ifndef _ACU_MODULE_ID_
	    typedef ACU_UINT ACU_MODULE_ID;
	    #define _ACU_MODULE_ID_
	#endif

	#ifndef _ACU_RESOURCE_ID_
	    typedef ACU_UINT ACU_RESOURCE_ID;
	    #define _ACU_RESOURCE_ID_
	#endif

	#ifndef _ACU_EVENT_QUEUE_
	    typedef ACU_UINT ACU_EVENT_QUEUE;
	    #define _ACU_EVENT_QUEUE_
	#endif
#else
#	if defined (__linux__)
#		error: You must define ACU_LINUX for compilation
#	elif defined (__QNX__)
#		error: You must define ACU_QNX for compilation
#	endif
#endif

/*
 * Macros to simplify checking for error conditions
 */
#define ACU_SUCCESS(x)(x >= 0)
#define ACU_FAILURE(x)(x < 0)

/*
 *	Macros for self-documenting code
 */

#define ACU_UNREFERENCED_PARM(x) {(void)x;}
#define ACU_ARRSIZE(x) (sizeof(x)/sizeof(x[0]))

enum 
{
	/*
	 *	Aculab error codes
	 */
	ERR_NO_ERROR					= 0,	/* There was no error */
	ERR_PARM						= -5,	/* Illegal parameter */
	ERR_INVALID_PARM				= -5,	/* Illegal parameter DUPLICATE */
	ERR_NO_SYS_RES					= -31,	/* Out of OS resources                    */

	ERR_MAX_APP_LIMIT				= -50,	/* Tried to start more than the supported number of applications */
	ERR_UNSUPPORTED					= -52,	/* Unsupported operation attempted  */
	ERR_NOT_IMPLEMENTED				= -53,	/* Function isn't implemented yet */
	ERR_FAIL						= -501, /* An error occurred for which there is no better error code */
	ERR_NO_MEMORY					= -502,	/* Operation failed because insufficient memory was allocated */
	ERR_CARD_NOT_FOUND				= -503,	/* Specified card could not be found */
	ERR_INVALID_RESOURCE			= -505,	/* An invalid resource was specified */
	ERR_ALREADY_OPEN				= -506,	/* Application attempted to open a resource that's already open */
	ERR_SERVER_NOT_RESPONDING		= -507,	/* The server is not responding */
	ERR_CARD_EJECT_PENDING			= -508,	/* The operation has failed because the card is waiting to be ejected */
	ERR_TIMEOUT						= -509, /* The operation timed out */
	ERR_STILL_IN_USE				= -510,	/* The resource could not be closed as it is still in use */
	ERR_INVALID_CARD				= -511,	/* An invalid card id was specified */
	ERR_INVALID_CONFIG				= -512, /* A configuration problem occurred */
	ERR_BUFF_SIZE					= -513,	/* The buffer provided is not big enough */
	ERR_RESOURCE_RELEASED			= -514,	/* The resource has been released */
	ERR_ALREADY_EXISTS				= -515,	/* This resource already exists */
	ERR_LIBRARY_NOT_LOADED			= -516, /* A required library is not loaded or doesn't contain the required function */
	ERR_ENVIRONMENT_NOT_SET			= -517, /* A required environment variable doesn't exist */
	ERR_FILE_ACCESS                         = -518, /* File I/O failed */
	ERR_BOARD_COMM_FAILURE                  = -519, /* Communication with board failed. */
	ERR_FILE_FORMAT                         = -520, /* Unable to parse file */
	ERR_BOOTLOADER_FAILED                   = -521, /* The firmware bootloader failed */
	ERR_INTERRUPTED							= -522, /* The operating system has interrupted a system function */
	ERR_FUNCTION_NOT_FOUND					= -523, /* The specified function is not available */
	ERR_SERVICE_DEPENDENCY_FAILED			= -524, /* A dependency of the specified service has failed */
	ERR_SERVICE_UNKNOWN						= -525, /* An unknown service was specified */
	ERR_SERVICE_RUNNING						= -526, /* The specified service is running */
	ERR_SERVICE_NOT_RUNNING					= -527, /* The specified service is not running */
	ERR_SERVICE_EXISTS						= -528, /* The specified service already exists */
	ERR_SERVICE_START_FAILED				= -529,	/* The specified service failed to start */
	ERR_SERVICE_STOP_FAILED					= -530,	/* The specified service failed to stop */
	ERR_FILE_NOT_FOUND						= -531, /* The specified file could not be found */
	ERR_BAD_KEY								= -532, /* The specified key is invalid */
	ERR_WRONG_SUBNET						= -533, /* The specified address is on the wrong subnet */
	ERR_NO_ACULAB_ROOT						= -534, /* The ACULAB_ROOT environment variable is not set */

	/*
	 *	General constants
	 */
	ACU_NO_EVENT				= 0,		/* No event waiting */
	ACU_MAX_SERIAL				= 20,		/* maximum length of a serial number */
	ACU_MAX_HWVER				= 20,		/* maximum length of a hardware version string */
	ACU_MAX_SNMP_COMMUNITY		= 20,		/* Maximum length of a SNMP community name */
	ACU_INFINITE_WAIT			= -1,		/* Use when an operation should wait indefinitely */
	ACU_MAX_IP_ADDRESS			= 60,
	MAX_TRACE_FILENAME			= 200,		/* Maximum size that the full path to a trace file can be */
	ACU_MAXADDR					= 512,		/* Maximum hostname address */


	/*-------------------------*/
	/*       TRACE FLAGS       */
	/*-------------------------*/
	TRACE_FLAG_API				= 1,	/* Trace API calls */
	TRACE_FLAG_INTERNAL			= 2,	/* Trace internal operations */
	TRACE_FLAG_EVENT			= 4,	/* Trace event queuing and generation */
	TRACE_FLAG_BUFFERED			= 8,	/* Buffer trace */
	TRACE_FLAG_THREAD_ID		= 16,	/* Trace thread ids */
	TRACE_FLAG_DELETE_OLD		= 32,	/* Delete all but the last two trace files */
	TRACE_FLAG_LOCKS			= 64,	/* Trace internal lock operation */
	TRACE_FLAG_SOCKET			= 0x10000,	/* Traces in acu_socket.c */

	/*--------------------------------------*/
	/*       INTERNAL RESOURCE TYPES        */
	/*--------------------------------------*/
	ACU_RES_TYPE_CARD			= 1,
	ACU_RES_TYPE_SWITCH			= 2,
	ACU_RES_TYPE_CALL			= 3,
	ACU_RES_TYPE_PORT			= 4,
	ACU_RES_TYPE_HANDLE			= 5,
	ACU_RES_TYPE_SPEECH			= 6,
	ACU_RES_TYPE_MODULE			= 7,
	ACU_RES_TYPE_XBUS			= 8

};

#define V6_TRACE_BUFFER_SIZE 4096
typedef struct tV6_TRACE_BUFFER
{
	ACU_ULONG		size;
	ACU_PORT_ID		port_id;
	ACU_CARD_ID		card_id;	/* FOR FUTURE EXPANSION */
	ACU_UINT		bytes_returned;
	ACU_UINT		bytes_lost;
	ACU_UINT		buffer_size;
	ACU_CHAR		buffer[V6_TRACE_BUFFER_SIZE];
} ACU_PACK_DIRECTIVE V6_TRACE_BUFFER;

typedef struct tACU_APP_CONTEXT_TOKEN_PARMS_64
{
    ACU_ULONG         size;                /* IN */
    ACU_RESOURCE_ID   resource_id;         /* IN */
    ACU_COMPAT_ACT    app_context_token;   /* IN/OUT */
} ACU_APP_CONTEXT_TOKEN_PARMS_64;

typedef struct tACU_APP_CONTEXT_TOKEN_PARMS
{
    ACU_ULONG         size;                /* IN */
    ACU_RESOURCE_ID   resource_id;         /* IN */
    ACU_ACT           app_context_token;   /* IN/OUT */
} ACU_APP_CONTEXT_TOKEN_PARMS;
	
typedef struct tACU_QUEUE_PARMS
{
    ACU_ULONG         size;                /* IN */
    ACU_RESOURCE_ID   resource_id;         /* IN */
    ACU_EVENT_QUEUE   queue_id;            /* IN */
} ACU_QUEUE_PARMS;


#define TiNG_TO_ACU_POINTER(x) ((const char *)(x) - (const char *)0)
#define ACU_POINTER_TO_TiNG(x) ((void *)((char *)0 + x))

#define PTR_TO_ACU_POINTER(x) ((const char *)(x) - (const char *)0)
#define ACU_POINTER_TO_PTR(x) ((void *)((char *)0 + x))


/* Helpful macro to initialise Aculab structures */
#define INIT_ACU_STRUCT(struct_p) { memset(struct_p, 0, sizeof(*(struct_p))); (struct_p)->size = sizeof(*(struct_p)); }
#define INIT_ACU_CL_STRUCT(struct_p) { memset(struct_p, 0, sizeof(*(struct_p))); (struct_p)->size = sizeof(*(struct_p)); }
#define INIT_ACU_SW_STRUCT(struct_p) { memset(struct_p, 0, sizeof(*(struct_p))); (struct_p)->size = sizeof(*(struct_p)); }
#define INIT_ACU_SM_STRUCT(struct_p) { memset(struct_p, 0, sizeof(*(struct_p))); }

#ifndef ACU_LINUX
#ifndef ACU_QNX
#pragma pack ( pop )
#endif /* ACU_QNX */
#endif /* ACU_LINUX */

#endif /* ACU_TYPE_H_INCLUDED */

