/* cmndev.h - user/kernel interface common to all devices */
#ifndef INCLUDED_CMNDEV_H
#define INCLUDED_CMNDEV_H

#if defined(TiNGTYPE_WINNT)

#ifdef TiNG_DRIVER
#include "driver/WINNT/ntddk-wdm.h"
#else
#include <windows.h>
#include <winioctl.h>
#endif

#ifdef __BORLANDC__
typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned long U32;
typedef signed long S32;
typedef unsigned __int64 U64;
typedef signed __int64 S64;
#define HAVE_DEFINED_64BIT
#endif

#ifdef __DMC__
#include <stdint.h>
typedef uint8_t U8;
typedef int8_t S8;
typedef uint16_t U16;
typedef int16_t S16;
typedef uint32_t U32;
typedef int32_t S32;
typedef uint64_t U64;
typedef int64_t S64;
#define HAVE_DEFINED_64BIT
#endif

#ifdef _MSC_VER
typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned long U32;
typedef signed long S32;
typedef unsigned __int64 U64;
typedef signed __int64 S64;
#define HAVE_DEFINED_64BIT
#endif

#endif

#ifdef TiNGTYPE_LINUX

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

typedef char S8;
typedef short S16;
typedef int S32;

#endif

#ifdef TiNGTYPE_QNX

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

typedef char S8;
typedef short S16;
typedef int S32;

#endif

#ifndef HAVE_DEFINED_64BIT
typedef unsigned long long U64;
typedef signed long long S64;
#endif

#endif
