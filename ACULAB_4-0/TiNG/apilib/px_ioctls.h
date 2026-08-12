/* px_ioctls.h - ioctl implementation for Prosody X */

#ifndef INCLUDED_PX_IOCTLS_H
#define INCLUDED_PX_IOCTLS_H

#include "px_conn.h"

LOCALDEC int px_ioctls(PX_CARDCONN_VTBL_DATA *pvp, unsigned long cmd, void *ap, unsigned asiz);

#endif
