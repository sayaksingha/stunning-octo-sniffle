/* errcode_cl.h - convert call control error codes to Englsh text */

#ifndef INCLUDED_ERRCODE_CL_H
#define INCLUDED_ERRCODE_CL_H

const char *errcode_cl(int err);
int printerr_cl(const char *name, int r);

#endif
