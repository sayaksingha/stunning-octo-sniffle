/* errcode_sm.h - convert Prosody error codes to Englsh text */

#ifndef INCLUDED_ERRCODE_SM_H
#define INCLUDED_ERRCODE_SM_H

#ifdef __cplusplus
extern "C" {
#endif

const char *errcode_sm(int err);
int printerr_sm(const char *name, int r);

#ifdef __cplusplus
}
#endif
#endif
