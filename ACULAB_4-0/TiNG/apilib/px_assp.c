#include "px_assp.h"
#include "px_util.h"
#include "trace.h"
#include <string.h>

LOCALDEF void dump_assp_pkt(struct assp_conn *cnx, PACKET *pkt, char *txt)
{
 uint8_t *dp = pkt->datastart;
 for (; dp < pkt->dataend; ) {
	char buff[20 + 9*17];
	uint32_t m = getmsg(&dp);
	if (m & 0x80000000) {
		int mlen = (m >> 16) & 0xf;
		if (!mlen) mlen = 17;
		sprintf(buff, "%04x:MSG(%04x)", m & 0xffff, m >> 16);
		for (; --mlen; ) sprintf(buff + strlen(buff), " %08x", getmsg(&dp));
	} else if (m & 0xffff0000) {
		sprintf(buff, "%04x:SPACE(%x)", m & 0xffff, m >> 16);
	} else if (dp < pkt->dataend) {
		uint32_t dlen = getmsg(&dp);
		uint32_t type = (dlen & 0xffff0000) >> 16;
		uint32_t actlen = pkt->dataend - dp;
		char *type_str = "UNKNOWN";
		dlen &= 0xffff;
		if (type == 0) {
			sprintf(buff, "%04x:DATA+%x", m & 0xffff, dlen);
		} else if (type == 1) {
			uint8_t *tmp_dp = dp;
			sprintf(buff, "%04x:STRING(%x)+%x", m & 0xffff, getmsg(&tmp_dp), dlen - 4);
		} else {
			sprintf(buff, "%04x:%s+%x", m & 0xffff, type_str, dlen);
		}
		if (dlen > actlen) {
			olog("%s@%p:%s...\n", txt, pkt, buff);
			return;
		}
		dlen = (dlen + 3) & ~3;	// add any padding
		dp += dlen;
	} else {
		olog("%s@%p:?%08lx.\n", txt, pkt, m);
		return;
	}
	olog("%s@%p-%p:%s\n", txt, cnx, pkt, buff);
 }
 if (dp != pkt->dataend) olog("%s:error: ran off end by %d\n", txt, dp - pkt->dataend);
}

