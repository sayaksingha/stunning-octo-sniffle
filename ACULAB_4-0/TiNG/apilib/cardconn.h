/* cardconn.h - connection to a Prosody card */

#ifndef INCLUDED_CARDCONN_H
#define INCLUDED_CARDCONN_H

#ifdef TiNGTYPE_LINUX
#include "POSIX/socket.h"
#endif
#ifdef TiNGTYPE_QNX
#include "POSIX/socket.h"
#endif
#ifdef TiNGTYPE_WINNT
#include "../libutil/WINNT/wind.h"
#endif

typedef struct cardconn CARDCONN;
struct tSMDatafeedId_struct;

#include "errcode.h"
#include "event.h"

struct cardconn {
	struct cardconnvtbl {
		ERRCODE (*clone)(CARDCONN *newc, CARDCONN *oldc);
		ERRCODE (*setblock)(CARDCONN *cx, int b);
		ERRCODE (*read)(CARDCONN *cx, void *buf, unsigned nc, unsigned *nr);
		ERRCODE (*write)(CARDCONN *cx, void *buf, unsigned nc, unsigned *nw);
		ERRCODE (*ioctl)(CARDCONN *cx, unsigned long cmd, void *ap, unsigned asiz);
		ERRCODE (*msgstring)(CARDCONN *cx, unsigned long id, void *buf, unsigned len);
		ERRCODE (*rdmsgstring)(CARDCONN *cx, unsigned long id, unsigned long id_mask, unsigned maxlen, void *s, unsigned *len);
		ERRCODE (*discardmsgs)(CARDCONN *cx, unsigned discard);
		ERRCODE (*event)(CARDCONN *cx, EVNT *ev);
		ERRCODE (*shutdown)(CARDCONN *cx);
		ERRCODE (*sec)(CARDCONN *cx, int, int, void*, tSMSecCallback encdec);
		ERRCODE (*close)(CARDCONN *cx);

	} *vtbl;
	void *vtbl_data;
};

#define CARDCONN_INIT {0, 0, }

/* these generic operations are the only ones permitted unless the object
 * has been made into a more specific cardconn type.
 */

/* check if object is valid */
#define cardconn_valid(cx) (!!(cx)->vtbl)

/* construct a non-valid object */
#define cardconn(cx) ((cx)->vtbl = 0)

/* these are only valid if the object is valid
 */
#define cx_clone(ncx, ocx) (ocx)->vtbl->clone((ncx), (ocx))
#define cx_setblock(cx, b) (cx)->vtbl->setblock((cx), (b))
#define cx_read(cx, buf, nc, nr) (cx)->vtbl->read((cx), (buf), (nc), (nr))
#define cx_write(cx, buf, nc, nw) (cx)->vtbl->write((cx), (buf), (nc), (nw))
#define cx_ioctl(cx, cmd, ap, asiz) (cx)->vtbl->ioctl((cx), (cmd), (ap), (asiz))
#define cx_msgstring(cx, id, buf, len) (cx)->vtbl->msgstring((cx), (id), (buf), (len))
#define cx_rdmsgstring(cx, id, id_mask, maxlen, s, len) (cx)->vtbl->rdmsgstring((cx), (id), (id_mask), (maxlen), (s), (len))
#define cx_discardmsgs(cx, discard) (cx)->vtbl->discardmsgs((cx), (discard))
#define cx_event(cx, pfd) (cx)->vtbl->event((cx), (pfd))
#define cx_shutdown(cx) (cx)->vtbl->shutdown((cx))
#define cx_sec(cx,level,keyid,context,encdec) (cx)->vtbl->sec((cx),(level),(keyid),(context),(encdec))
#define cx_close(cx) (cx)->vtbl->close((cx))
#endif
