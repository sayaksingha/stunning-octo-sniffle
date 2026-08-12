/* decode.h - decode a rx packet for assp */

#include "assp.h"

static char *dec32(uint_fast32_t *vp, char *sp)
{
 *vp = 0;
 *vp += *sp++ << 24;
 *vp += *sp++ << 16;
 *vp += *sp++ << 8;
 *vp += *sp++;
 return sp;
}

static char *dec16(uint_fast32_t *vp, char *sp)
{
 *vp = 0;
 *vp += *sp++ << 8;
 *vp += *sp++;
 return sp;
}

int decode(PACKET *rxp, char *sp, char *iep)
{
 sp = dec32(&rxp->pd.cid, sp);
 sp = dec32(&rxp->pd.seq, sp);
 sp = dec32(&rxp->pd.ack, sp);
 sp = dec32(&rxp->pd.txts, sp);
 sp = dec32(&rxp->pd.refts, sp);
 sp += 2;				// reserved field
 	// unsequenced options
 for (;;) {
	char *optstart = sp;
	uint_fast32_t optcode;
	uint_fast32_t optlen;
	if (sp >= iep) {
		ill-formed packet
	}
	sp = dec16(&optlen, sp);
	if (!optlen) break;
	optstart = sp;
	sp = dec16(&optcode, sp);
	switch (optcode) {
	case ASSP_OPT_RST: rxp->pd.isrst = 1; break;
	case ASSP_OPT_FASTACK: rxp->pd.isfastack = 1; break;
	case ASSP_OPT_ACKLESS: rxp->pd.isackless = 1; break;
	case ASSP_OPT_WTRDNS:
		rxp->pd.wtr_dns = sp;
		rxp->pd.wtr_dns_len = optlen - 1;
		break;
	case ASSP_OPT_GETCOOKIE: rxp->pd.isgetcookie = 1; break;
	case ASSP_OPT_COOKIE:
		if (optlen < (COOKIE_LEN + 1) / 2) {
			// FIXME: bad cookie
		}
		rxp->pd.cookie = sp;
		break;
	case ASSP_OPT_SYN: rxp->pd.issyn = 1; break;
	case ASSP_OPT_FIN: rxp->pd.isfin = 1; break;
	default:
		check for critical flag
	}
	sp = optstart + 2 * optlen;
 }
 rxp->pd.data = sp;
 rxp->pd.dsize = iep - sp;
 return 0;
}
