/* errcode_sw.h - convert switch driver error codes to Englsh text */

#ifndef INCLUDED_ERRCODE_SW_H
#define INCLUDED_ERRCODE_SW_H

const char *errcode_sw(int err);
int printerr_sw(const char *name, int r);

#endif
