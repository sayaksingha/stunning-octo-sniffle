/* ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/include/tools/common/isup_lib.h,v 1.15 2024/11/15 09:18:38 davidla Exp $ $Name: SS7_6_17_0 $" */

#ifndef ISUP_LIB_H
#define ISUP_LIB_H
/*****************************************************************************\
*
* ISUP_LIB.H : Flexible ISUP helper functions for parameter manipulation
*
* (C)2005-2006 Aculab Plc
*
\*****************************************************************************/

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude rarely-used stuff from Windows headers */
#endif

#include <windows.h>
#endif /* _WIN32 */

#include "cl_lib.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#ifndef ACU_ISUP_LIB_API
#ifdef _WIN32
#define ACU_ISUP_LIB_API __declspec(dllimport)
#else
#define ACU_ISUP_LIB_API __attribute__((visibility("default")))
#endif
#endif

#define ISUP_SUCCESS    (0)  /* Success */
#define ISUP_EBADPARAMS (-1) /* Bad parameters */
#define ISUP_EBOUNDSCHK (-2) /* Would require memory access beyond a buffer */
#define ISUP_ENOTFOUND  (-3) /* The requested object was not found */


/* Returns a version string */
ACU_ISUP_LIB_API const char *isup_lib_version(void);

/* This function is designed for use on RAW_MSG_XPARMS received from ISUP via
 * call_feature_details(), where the first byte of data[] is a message type.
 * The function returns a pointer to the next parameter type, or 0 if not found.
 * Arg2 specifies a starting point for the parameter search; use NULL to search
 * from the start of the message.  If arg2 is non-zero, searching begins at the
 * parameter after the one pointed to.  */
ACU_ISUP_LIB_API ACU_UCHAR *isup_get_next_parameter(const RAW_MSG_XPARMS *raw, const ACU_UCHAR *params);

/* This function is designed for use on RAW_MSG_XPARMS received from ISUP via
 * call_feature_details(), where the first byte of data[] is a message type.
 * The function returns a pointer to the next occurrence of the specified
 * parameter type, or 0 if not found.  Arg3 specifies a starting point for the
 * parameter search; use NULL to search from the start of the message.  If arg3
 * is non-zero, searching begins at the parameter after the one pointed to.  */
ACU_ISUP_LIB_API ACU_UCHAR *isup_get_parameter(const RAW_MSG_XPARMS *raw, ACU_UCHAR type, const ACU_UCHAR *params);

/* Places a parameter payload into a raw message in preparation for
 * transmission.  Returns a negative error code if the requested operation would
 * result in a write operation beyond the raw_msg_xparms. */
ACU_ISUP_LIB_API ACU_INT isup_set_parameter(RAW_MSG_XPARMS *raw, ACU_UCHAR type, ACU_UCHAR* param, ACU_UINT len);

/* Returns instruction indicators for the specified parameter type, or a
 * negative error code. */
ACU_ISUP_LIB_API ACU_INT isup_get_pcompat_info(const RAW_MSG_XPARMS *raw, ACU_UCHAR type, const ACU_UCHAR *info);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ISUP_LIB_H */
