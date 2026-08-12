#ifndef INCLUDED_SMGROOVE_H
#define INCLUDED_SMGROOVE_H

#include <stdint.h>
#include "TiNGo_datafeed.h"
#include "smdrvr.h"

typedef struct tSMGroove_struct tSMGroove;

struct module;

#include "smtypes.h"
#include "cardconn.h"

struct tSMGroove_struct {
	int (*starttask)(tSMGroove *groove, uint32_t typecode, uint32_t ver);
	int (*read)(tSMGroove *groove, void *buf, unsigned rqlen, unsigned *donelen);
	int (*write)(tSMGroove *groove, void *buf, unsigned rqlen, unsigned *donelen);
	int (*sendmsg)(tSMGroove *groove, uint32_t msg, ...);
	int (*sendmsgstring)(tSMGroove *groove, uint32_t id, uint32_t slen, char *s);
	int (*rdmsg)(tSMGroove *groove, uint32_t msg[16], int flags);
	int (*rdmsg_ex)(tSMGroove *gp, uint32_t msg[16], int flags, uint32_t* cap, unsigned* has_msg);
	int (*rdmsgstring)(tSMGroove *groove, uint32_t id, uint32_t id_mask, uint32_t maxlen, char *s, uint32_t *len);
	int (*getev)(tSMGroove *groove, tSMEventId *ep);
	int (*get_df)(tSMGroove *groove, DATAFEED df, struct module *module);
	int (*unbind_df)(tSMGroove *groove, DATAFEED df);
	int (*engage_df)(tSMGroove *groove, DATAFEED df, enum dfconntyp type);
	int (*connect_df)(tSMGroove *groove, DATAFEED *targ, DATAFEED df);
	CARDCONN *(*cardconn_cx)(tSMGroove *groove);
	int (*setfriend)(tSMGroove *groove, tSMGroove *friend, unsigned type);
	int (*setfriendcx)(tSMGroove *groove, CARDCONN *cx, unsigned type);
	void (*dtor)(tSMGroove *groove);
	struct tSMGroove_data *gdp;
};

#define SMGROOVE_RDMSG_FLAG_NOWAIT 1
#define SMGROOVE_RDMSG_FLAG_KEEPSTRINGS 2

#define SMGroove aculab_TiNG_SMGroove
#define SMGroove_module aculab_TiNG_SMGroove_module

ACUAPI int ACUTiNG_WINAPI SMGroove(tSMGroove *gp, tSMModuleId mod);
int SMGroove_module(tSMGroove *gp, struct module *module);
#endif
