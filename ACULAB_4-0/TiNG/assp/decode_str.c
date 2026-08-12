/* decode_str.h - decode a rx packet into a string for assp */

#include "assp.h"
#include "decode_str.h"
#include "vseprintf.h"

static uint8_t *decd32(uint32_t *vp, uint8_t *sp)
{
 *vp = 0;
 *vp += *sp++ << 24;
 *vp += *sp++ << 16;
 *vp += *sp++ << 8;
 *vp += *sp++;
 return sp;
}

static uint8_t *decd16(uint32_t *vp, uint8_t *sp)
{
 *vp = 0;
 *vp += *sp++ << 8;
 *vp += *sp++;
 return sp;
}

char *decode_str(char *op, char *oep, uint8_t *sp, uint8_t *iep)
{
 uint32_t v;
 sp = decd32(&v, sp);
 op = seprintf(op, oep, "[C=%x", v);
 sp = decd32(&v, sp);
 op = seprintf(op, oep, ", S=%u", v);
 sp = decd32(&v, sp);
 op = seprintf(op, oep, ", A=%u", v);
 sp = decd32(&v, sp);
 op = seprintf(op, oep, ", T=%u", v);
 sp = decd32(&v, sp);
 op = seprintf(op, oep, ", R=%u ", v);
 sp += 2;				// reserved field
 if (sp >= iep) {
	op = seprintf(op, oep, "??short by %d]", iep - sp);
	return op;
 }
 	// unsequenced options
 for (;;) {
	uint8_t *nextopt;
	uint32_t opv;
	if (sp >= iep) break;
		// sp < iep
	sp = decd16(&opv, sp);
	if (!opv) break;
		// sp - 2 < iep, so iep - sp is very big if sp > iep
	if ((unsigned) (iep - sp) < opv * 2) {	// option overflows packet
		op = seprintf(op, oep, "??malformed ");
		break;
	}
	nextopt = sp + opv * 2;
	sp = decd16(&opv, sp);
	switch (opv) {
	case ASSP_OPT_RST: op = seprintf(op, oep, "RST "); break;
	case ASSP_OPT_FASTACK: op = seprintf(op, oep, "FAST "); break;
	case ASSP_OPT_ACKLESS: op = seprintf(op, oep, "ACKL "); break;
	case ASSP_OPT_WTRDNS:
		op = seprintf(op, oep, "WTRDNS(");
		for (; sp + 1 < nextopt; ) {
			op = seprintf(op, oep, "%d:", *sp++);
			op = seprintf(op, oep, "%d,", *sp++);
		}
		if (sp != nextopt) op = seprintf(op, oep, "%d!", *sp++);
		op = seprintf(op - 1, oep, ") ");
		break;
	case ASSP_OPT_GETCOOKIE: op = seprintf(op, oep, "GETCOOKIE "); break;
	case ASSP_OPT_USECOOKIE:
		op = seprintf(op, oep, "USECOOKIE(");
		for (; sp < nextopt; ) {
			op = seprintf(op, oep, "%02x", *sp++);
		}
		op = seprintf(op, oep, ") ");
		break;
	case ASSP_OPT_COOKIE:
		op = seprintf(op, oep, "COOKIE(");
		for (; sp < nextopt; ) {
			op = seprintf(op, oep, "%02x", *sp++);
		}
		op = seprintf(op, oep, ") ");
		break;
	case ASSP_OPT_KEYCODE:
		op = seprintf(op, oep, "KEYCODE(");
		for (; sp < nextopt; ) {
			op = seprintf(op, oep, "%02x", *sp++);
		}
		op = seprintf(op, oep, ") ");
		break;
	case ASSP_OPT_SYN: op = seprintf(op, oep, "SYN "); break;
	case ASSP_OPT_FIN: op = seprintf(op, oep, "FIN "); break;
	default:
		op = seprintf(op, oep, "OPT(%x:", opv);
		for (; sp < nextopt; ) {
			op = seprintf(op, oep, "%02x", *sp++);
		}
		op = seprintf(op, oep, ") ");
		break;
	}
	sp = nextopt;
 }
 if (sp < iep) op = seprintf(op, oep, "+DATA(%d) ", iep-sp);
 op[-1] = ']';	// overwrite last space
 return op;
}
