/* assp_impl,h - declarations used internally in the ASSP implementation */
#ifndef INCLUDED_ASSP_IMPL_H
#define INCLUDED_ASSP_IMPL_H

#include <stdint.h>

typedef struct {
	uint32_t rxtim;
	char *error;		// if non-null, type of error
		// fixed header stuff
	uint32_t ack;
	uint32_t txts;
	uint32_t refts;
		// non-sequenced options
	int isrst;
	int fastack;
	int ackless;
	unsigned wtr_dns_len;		// number of items in wtr_dns
	uint8_t *wtr_dns;
		// sequenced options
	int hasseq;
	int issyn;
	int getcookie;
	uint8_t *usecookie;
	uint8_t *cookie;
	unsigned cookielen;
	uint8_t *keycode;
	unsigned keycodelen;
	PACKET *pkt;
} RPACKET;

#endif
