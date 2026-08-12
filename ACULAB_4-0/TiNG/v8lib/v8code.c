#include "v8.h"

	// call function
const int preferred_call_function = 0x81;	// for CI/CM
const int is_multimedia = 0;
const int is_textphone = 0;
const int is_videotext = 0;
const int is_callertxfax = 0;
const int is_callerrxfax = 0;
const int is_data = 0;
	// modulation modes - as necessary
const int has_v90_or_v92_ana = 0;
const int has_v90_or_v92_dig = 0;
const int has_v91 = 0;
const int has_v34hdx = 0;
const int has_v34fdx = 0;
const int has_v32 = 0;
const int has_v22 = 0;
const int has_v17 = 1;
const int has_v29 = 1;
const int has_v27 = 1;
const int has_v26ter = 0;
const int has_v26bis = 0;
const int has_v23fdx = 0;
const int has_v23hdx = 0;
const int has_v21 = 0;
	// protocol - as necessary
const int has_v42 = 0;
	// PSTN access - as necessary
const int is_cell_caller = 0;
const int is_cell_answering = 0;
const int is_digital_network = 0;

int v8_build_code(unsigned char *buf)
{
 	// preamble
 const int is_ci = 1;
 const int is_cm = 0;
 unsigned pos = 0;
 	// preamble
 if (is_ci) buf[pos++] = 0x00;
 else if (is_cm) buf[pos++] = 0xe0;
 else /* V.92 */ buf[pos++] = 0x55;
 	// call function - must come first
 buf[pos++] = preferred_call_function;
 if (is_multimedia) buf[pos++] = 0x21;
 else if (is_textphone) buf[pos++] = 0x41;
 else if (is_videotext) buf[pos++] = 0x61;
 else if (is_callertxfax) buf[pos++] = 0x81;
 else if (is_callerrxfax) buf[pos++] = 0xa1;
 else if (is_data) buf[pos++] = 0xc1;
 	// modulation modes
 buf[pos++] = 0x05 | (has_v90_or_v92_ana | has_v90_or_v92_dig) << 5
 	| has_v91 << 5 | has_v34fdx << 6 | has_v34hdx << 7;
 buf[pos++] = 0x10 | has_v32 << 0 | has_v22 << 1 | has_v17 << 2
 	| has_v29 << 6 | has_v27 << 7;
 buf[pos++] = 0x10 | has_v26ter << 0 | has_v26bis << 1 | has_v23fdx << 3
 	| has_v23hdx << 6 | has_v21 << 7;
	// PCM modem availability
 if (has_v90_or_v92_ana || has_v90_or_v92_dig || has_v91) {
	buf[pos++] = 0x07 | has_v90_or_v92_ana << 5 | has_v90_or_v92_dig << 6
		| has_v91 << 7;
 }
 	// protocol
 if (has_v42) buf[pos++] = 0x2a;
 if (has_v90_or_v92_ana || has_v90_or_v92_dig || has_v91) {
	buf[pos++] = 0x0d | is_cell_caller << 5 | is_cell_answering << 6
		| is_digital_network << 7;
 }
 return pos;
}

int v8_check_code(unsigned char *out, unsigned char *in, unsigned char *inend)
{
 unsigned pcm_modem = 0;
 unsigned outlen = 0;
 if (in == inend) return 0;	// zero-length input - bogus
 if (*in != 0x00 && *in != 0xe0) return 0; // not CI, CM, or JM - cannot handle
 out[outlen++] = 0xe0;	// must be JM if we are responding
 if (++in == inend) return 0;	// no call function - bogus
 switch (*in) {	// must be call function - return if incompatible
 case 0x21: if (!is_multimedia) return 0; break;
 case 0x41: if (!is_textphone) return 0; break;
 case 0x61: if (!is_videotext) return 0; break;
 case 0x81: if (!is_callertxfax) return 0; break;
 case 0xa1: if (!is_callerrxfax) return 0; break;
 case 0xc1: if (!is_data) return 0; break;
 default: return 0;
 }
 out[outlen++] = *in++;	// probably need to record thie somewhere also
 if (in == inend) return outlen;
 	// should we copy unknown extension octets, clearing b0-2,b6,b7?
 while (in < inend && (*in & 0x38) == 0x10) {
 	in++;	// skip unknowns
	if (in == inend) return outlen;
 }
 for (;;) {
	switch (*in & 0x1f) {
		unsigned v;
	case 0x05:	// modulation
		v = 0x05 | (has_v90_or_v92_ana | has_v90_or_v92_dig) << 5
			| has_v91 << 5 | has_v34fdx << 6 | has_v34hdx << 7;
		pcm_modem = (out[outlen++] = v & *in++) & 0x20;
		if (in == inend) return outlen;
		v = 0x10 | has_v32 << 0 | has_v22 << 1 | has_v17 << 2
			| has_v29 << 6 | has_v27 << 7;
		if ((*in & 0x38) == 0x10) out[outlen++] = v, in++;
		if (in == inend) return outlen;
		v = 0x10 | has_v26ter << 0 | has_v26bis << 1 | has_v23fdx << 3
			| has_v23hdx << 6 | has_v21 << 7;
		if ((*in & 0x38) == 0x10) out[outlen++] = v, in++;
		if (in == inend) return outlen;
			// copy extra mode octets, clearing them
		while ((*in & 0x38) == 0x10) {
			out[outlen++] = 0x38 & *in++;
			if (in == inend) return outlen;
		}
		continue;	// already handled extension octets
	case 0x0a:	// protocols
		v = 0xa | has_v42 << 5;
		if (*in & (has_v42 << 5)) out[outlen++] = v;
		break;
	case 0x0d:	// PSTN access
		v = 0x0d | (*in & 0x20) | is_cell_answering << 6
			| is_digital_network << 7;
		out[outlen++] = v;
		break;
	case 0x07:	// PCM modem
		v = has_v90_or_v92_ana << 5 | has_v90_or_v92_dig << 6
			| has_v91 << 7;
		v &= *in;
		if (pcm_modem && v) {
			out[outlen++] = 0x07 | v;
		}
		break;
	default:	// unknown - ignore
		break;
	}
		// NB: not reached from 'modulation'
	do {	// skip extension octets
		in++;
		if (in == inend) return outlen;
	} while ((*in & 0x38) == 0x10);
 }
}
