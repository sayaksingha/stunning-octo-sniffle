/* smclib.h - high level conferencing library */
#ifndef INCLUDED_SMCLIB_H
#define INCLUDED_SMCLIB_H

#ifdef TiNGTARGET_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#include "smbesp.h"

typedef struct sm_conference *tSMConference;

#ifdef __cplusplus
extern "C" {
#endif

#include "../libutil/header_prolog.h"
#include "../pubdoc/gen/highapi_conf.h"
#include "../libutil/header_epilog.h"

extern ACUAPI char *smConfError;
extern ACUAPI int smConfErrno;

#define CONF_MAX_PARTY      60

#ifdef __cplusplus
}
#endif

#endif
