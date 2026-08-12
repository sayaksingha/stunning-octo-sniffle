/* TiNG.h - user/kernel interface */
#ifndef INCLUDED_TiNG_H
#define INCLUDED_TiNG_H

#include "cmndev.h"

#ifdef TiNGTYPE_LINUX
#define KNOWN_TiNGTYPE

#define IOCTL_TiNG(x) ('P'+(x))

#endif

#ifdef TiNGTYPE_QNX
#define KNOWN_TiNGTYPE

#define IOCTL_TiNG(x) ('P'+(x))

#endif

#ifdef TiNGTYPE_WINNT
#define KNOWN_TiNGTYPE

#define DEVICE_TYPE_TiNG 0xcb00
#define TiNG_IOCTL_BASE 0xcb0

#define IOCTL_TiNG(x) CTL_CODE(					\
				DEVICE_TYPE_TiNG,		\
				TiNG_IOCTL_BASE+(x),		\
				METHOD_BUFFERED,		\
				FILE_ANY_ACCESS)
#endif

#ifndef KNOWN_TiNGTYPE
#error no TiNGTYPE variant defined
#endif

#define TiNG_VARIABLE_LENGTH 1

/* Even numbered IOCTLs read an object, the next IOCTL writes it.
 * ioctls all have ioctl_length as the first field and when this is
 * different to the expected value, the structure is either assumed
 * to have extra zeros appended to bring it up to the correct length,
 * or the extra is ignored and ioctl_length is set to reflect the
 * amount actually used.
 */

typedef U32 SHARCADDR;
typedef U32 SHARCDATA;
typedef struct {
	U8 byte[6];
} SHARCINST;

typedef struct {
	U32 type;
	U32 timeslot;	/* order is: tx0, tx1, rx0, rx1: 32 timeslots each */
} TiNG_TIMESLOT;
#define TiNG_TIMESLOT_FLAG_INPUT 0x80000000

enum TiNG_COMPAND_TYPE {
	TiNG_TSTYPE_NONE,
	TiNG_TSTYPE_COMPAND_ALAW,
	TiNG_TSTYPE_COMPAND_uLAW
};

/* a read or write channel is ready when the minimum number of bits has been
 * transferred OR the timeout has expired. The timeout is started when the any
 * data is transferred. A negative number of bits refers to empty space (i.e.
 * buflen - bits == space).
 *
 * Note that the maximum timeout is about 49 days.
 */

/* Prosody S : name of env. var. which holds port no. of Prosody S server
 */
#define TiNG_PROSODYS_PORTNO_ENVVARNAME "ACULAB_PROSODYS_PORT"
 
/* Prosody S : messaging constants/structs.
 */
enum prosodys_major {
	TiNG_PROSODYS_ERROR=0,
	TiNG_PROSODYS_READ=1,
	TiNG_PROSODYS_WRITE=2,
	TiNG_PROSODYS_IOCTL=3,
#ifndef TiNGTYPE_WINNT
	TiNG_PROSODYS_EVENT=4,
#endif
	TiNG_PROSODYS_WRSTRING=5,
};

struct ting_prosodysread {
	U32 length;
};

struct ting_prosodys_header {
	U32 major;
	U32 minor;
	U32 length;
};

struct ting_prosodysread_req {
	struct ting_prosodys_header hdr;
	U32 rqsize;
};

#include "doc/gen/TiNG.h"

#endif
