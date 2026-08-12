/* smbfhlib.h - high level BFILE play/record library */
#ifndef INCLUDED_SMBFHLIB_H
#define INCLUDED_SMBFHLIB_H

#ifdef TiNGTARGET_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "../libutil/bfile.h"
#include "smbesp.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/highapi_bf_playrec.h"
#include "../libutil/header_epilog.h"

#ifdef __cplusplus
}
#endif

#endif
