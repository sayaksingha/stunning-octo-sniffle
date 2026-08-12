/* fdxdc.c - generic data communication full duplex tester */

#include "fdxdc.h"

#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"

#ifdef TiNGTYPE_LINUX
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#include <sys/socket.h>
#endif
#ifdef TiNGTYPE_QNX
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/poll.h>
#endif
#ifdef TiNGTYPE_WINNT
#include "../../libutil/WINNT/lasterr.h"
#endif

#ifdef _MSC_VER

/* disable stupid warnings about doubles being converted to floats.
 */

#pragma warning(disable:4244)
#pragma warning(disable:4305)
/* and about argument conversions (does anyone still use a
   non-prototype compiler?) */
#pragma warning(disable:4761)

#endif

#define arlen(x) (sizeof(x)/sizeof(*(x)))

static int closecoll(CHAN *chan)
{
 if (chan->coll != kSMNullCollectorId) {
	int r = sm_collector_destroy(chan->coll);
	if (r) return printerr_sm("sm_collector_destroy", r);
 }
 chan->coll = kSMNullCollectorId;
 return 0;
}

static int runthreads(CHAN *chan, char *inifile)
{
 pthread_t tid;
 void *sts;
 int r;
 if (chan->cfg->rx.smdc.protocol != kSMDCProtocolNone) {
	int e = pthread_create(&tid, 0, rxmain, chan);
	if (e) {
		fprintf(stderr, "pthread_create() failed: %s\n", strerror(e));
		return 1;
	}
 }
 r = runtx(chan, inifile);
 if (chan->cfg->rx.smdc.protocol != kSMDCProtocolNone) {
		// FIXME: kill rx thread here
	int e = pthread_join(tid, &sts);
	if (e) {
		fprintf(stderr, "pthread_join() failed: %s\n", strerror(e));
		r = 1;
	} else if (sts) r = 1;
 }
 return r;
}

static int runrxtx(CHAN_CONFIG *cfg, tSMModuleId mod, char *inifile)
{
 CHAN chan;
 int e;
 int r;
 chan.cfg = cfg;
 r = setup_rxtx(&chan, mod);
 if (r) return 1;
 r = runthreads(&chan, inifile);
 if (closecoll(&chan)) r = 1;
 e = sm_channel_release(chan.chan);
 if (e) {
 	printerr_sm("sm_channel_release", e);
 	r = 1;
 }
 return r;
}

static unsigned auto_crc_size(unsigned crc)
{
 unsigned i;
 if (!crc) return 0;
 for (i=0; crc; crc >>= 1) i++;
 return i - 1;
}

static int lookup_fsk(SMDC_FSK_CONFIG_PARMS *fskp, char *name)
{
 static struct {
	char *name;
	float markfreq, spacefreq;
	unsigned speed;
 } std[] = {
	{ "v21o", 980, 1180, 300, },
	{ "v21a", 1650, 1850, 300, },
	{ "v23o", 390, 450, 75, },
	{ "v23a", 1300, 2100, 1200, },
	{ "v23a600", 1300, 1700, 600, },
	{ "bell103a", 1270, 1070, 300, },
	{ "bell103o", 2225, 2025, 300, },
 };
 unsigned i;
 for (i=0; ; i++) {
	if (i >= arlen(std)) {
		fprintf(stderr, "Unknown standard: %s\n", name);
		fprintf(stderr, "Valid standards:");
		for (i=0; i < arlen(std); i++) {
			fprintf(stderr, " %s", std[i].name);
		}
		fprintf(stderr, "\n");
		return 1;
	}
	if (!strcmp(name, std[i].name)) {
		fskp->mark_frequency = std[i].markfreq;
		fskp->space_frequency = std[i].spacefreq;
		fskp->speed = std[i].speed;
		break;
	}
 }
 return 0;
}

static int setup_mod(struct rxtxcfg *cfg, char *mod, struct rxtxcfg *rxcfg, unsigned bitrate, unsigned spacefreq, float power, unsigned markfreq, unsigned carrier_on, unsigned carrier_off, int answer, char *standard, unsigned xorval, unsigned i460_mask)
{
 memset(&cfg->m, 0, sizeof(cfg->m));
 cfg->isanswer = answer;
 if (!mod) return 0;
 if (!strcmp(mod, "raw")) {
 	cfg->smdc.protocol = kSMDCProtocolRawRx;
 	cfg->smdc.config_length = sizeof(cfg->m.raw);
 	cfg->smdc.config_data = &cfg->m.raw;
 	cfg->m.raw.xorval = xorval;
 	cfg->m.raw.i460_mask = i460_mask;
 } else if (!strcmp(mod, "cw")) {
 	cfg->smdc.protocol = kSMDCProtocolCWrx;
	cfg->smdc.config_length = sizeof(cfg->m.cw);
	cfg->smdc.config_data = &cfg->m.cw;
	if (spacefreq) cfg->m.cw.space_frequency = spacefreq;
	if (bitrate) cfg->m.cw.speed = bitrate;
 } else if (!strcmp(mod, "fsk")) {
 	cfg->smdc.protocol = kSMDCProtocolFSKrx;
	cfg->smdc.config_length = sizeof(cfg->m.fsk);
	cfg->smdc.config_data = &cfg->m.fsk;
	if (standard) {
		if (lookup_fsk(&cfg->m.fsk, standard)) return 1;
	}
	cfg->m.fsk.rx_carrier_on_mS = carrier_on;
	cfg->m.fsk.rx_carrier_off_mS = carrier_off;
	if (markfreq) cfg->m.fsk.mark_frequency = markfreq;
	if (spacefreq) cfg->m.fsk.space_frequency = spacefreq;
	if (bitrate) cfg->m.fsk.speed = bitrate;
	cfg->m.fsk.user_power = power;
	cfg->m.fsk.set_power = 1;
 } else if (!strcmp(mod, "v110")) {
 	cfg->smdc.protocol = kSMDCProtocolV110;
	cfg->smdc.config_length = sizeof(cfg->m.v110);
	cfg->smdc.config_data = &cfg->m.v110;
	if (bitrate) cfg->m.v110.speed = bitrate;
 } else if (!strcmp(mod, "v110rlp")) {
 	cfg->smdc.protocol = kSMDCProtocolV110RLPrx;
	cfg->smdc.config_length = sizeof(cfg->m.v110rlp);
	cfg->smdc.config_data = &cfg->m.v110rlp;
	if (bitrate) cfg->m.v110rlp.speed = bitrate;
 } else if (!strcmp(mod, "v17")) {
 	cfg->smdc.protocol = kSMDCProtocolV17rx;
	cfg->smdc.config_length = sizeof(cfg->m.v17);
	cfg->smdc.config_data = &cfg->m.v17;
	if (bitrate) cfg->m.v17.speed = bitrate;
 } else if (!strcmp(mod, "v27")) {
 	cfg->smdc.protocol = kSMDCProtocolV27rx;
	cfg->smdc.config_length = sizeof(cfg->m.v27);
	cfg->smdc.config_data = &cfg->m.v27;
	if (bitrate) cfg->m.v27.speed = bitrate;
 } else if (!strcmp(mod, "v29")) {
 	cfg->smdc.protocol = kSMDCProtocolV29rx;
	cfg->smdc.config_length = sizeof(cfg->m.v29);
	cfg->smdc.config_data = &cfg->m.v29;
	if (bitrate) cfg->m.v29.speed = bitrate;
 } else if (!strcmp(mod, "v32")) {
	cfg->smdc.protocol = kSMDCProtocolV32rx;
	cfg->smdc.config_length = sizeof(cfg->m.v32);
	cfg->smdc.config_data = &cfg->m.v32;
	cfg->m.v32.speed = bitrate;
	cfg->m.v32.isanswer = answer;
	cfg->m.v32.no_ans_tone = spacefreq ? 1 : 0;
	cfg->m.v32.retrain_can_change_speed = markfreq ? 1 : 0;
 } else if (*mod == '=' && !mod[1]) {
 	if (!rxcfg) {
		fprintf(stderr, "Modulation option '=' is only for tx\n");
		return 1;
 	}
 	*cfg = *rxcfg;
 } else {
	fprintf(stderr, "unknown modulation option: %s\n", mod);
	fprintf(stderr, "Valid options are: raw cw fsk v110 v110rlp v17 v27 v29 v32 =\n");
	return 1;
 }
 if (rxcfg) {
 	switch (cfg->smdc.protocol) {
 	case kSMDCProtocolCWrx: cfg->smdc.protocol = kSMDCProtocolCWtx; break;
 	case kSMDCProtocolFSKrx: cfg->smdc.protocol = kSMDCProtocolFSKtx; break;
 	case kSMDCProtocolRawRx: cfg->smdc.protocol = kSMDCProtocolRawTx; break;
 	case kSMDCProtocolV110RLPrx: cfg->smdc.protocol = kSMDCProtocolV110RLPtx; break;
 	case kSMDCProtocolV17rx: cfg->smdc.protocol = kSMDCProtocolV17tx; break;
 	case kSMDCProtocolV27rx: cfg->smdc.protocol = kSMDCProtocolV27tx; break;
 	case kSMDCProtocolV29rx: cfg->smdc.protocol = kSMDCProtocolV29tx; break;
 	case kSMDCProtocolV32rx:
		if (rxcfg->smdc.protocol != kSMDCProtocolV32rx) {
			fprintf(stderr, "Modulation 'v32' can only be used on tx if also used on rx\n");
			return 1;
		}
		rxcfg->smdc.protocol = cfg->smdc.protocol = kSMDCProtocolV32;
		if (rxcfg->m.v32.speed != bitrate) {
			fprintf(stderr, "Tx (%d) and Rx (%d) speeds must be equal\n",
				bitrate, rxcfg->m.v32.speed);
			return 1;
		}
		if (cfg->isanswer != rxcfg->isanswer) {
			fprintf(stderr, "Tx and Rx disagree whether to answer or originate\n");
			return 1;
		}
		if (rxcfg->m.v32.no_ans_tone != cfg->m.v32.no_ans_tone) {
			fprintf(stderr, "Tx and Rx disagree whether ANS tone is used (configured with spacefreq)\n");
			return 1;
		}
		if (rxcfg->m.v32.retrain_can_change_speed != cfg->m.v32.retrain_can_change_speed) {
			fprintf(stderr, "Tx and Rx disagree whether retrains can change speed (configured with markfreq)\n");
			return 1;
		}
		break;
 	case kSMDCProtocolV110:
		break;
 	}
 }
 return 0;
}

static int setup_enc(struct rxtxcfg *cfg, char *enc, struct rxtxcfg *rxcfg, unsigned crcsize, unsigned crc, unsigned databits, unsigned stopbits)
{
 memset(&cfg->e, 0, sizeof(cfg->e));
 if (enc) {
	if (!strcmp(enc, "sync")) {
		cfg->smdc.encoding = kSMDCConfigEncodingSync;
	} else if (!strcmp(enc, "async")) {
		cfg->smdc.encoding = kSMDCConfigEncodingAsync;
		cfg->smdc.encoding_config_data = &cfg->e.async;
		cfg->smdc.encoding_config_length = sizeof(cfg->e.async);
		cfg->e.async.stopbits = stopbits;
		cfg->e.async.databits = databits;
	} else if (!strcmp(enc, "hdlc")) {
		cfg->smdc.encoding = kSMDCConfigEncodingHDLC;
		cfg->smdc.encoding_config_data = &cfg->e.hdlc;
		cfg->smdc.encoding_config_length = sizeof(cfg->e.hdlc);
		if (crc) cfg->e.hdlc.crc = crc;
		if (crcsize) cfg->e.hdlc.crcsize = crcsize;
		else cfg->e.hdlc.crcsize = auto_crc_size(cfg->e.hdlc.crc);
	} else if (*enc == '=' && !enc[1]) {
		if (!rxcfg) {
			fprintf(stderr, "Encoding option '=' is only for tx\n");
			return 1;
		}
		*cfg = *rxcfg;
	} else {
		fprintf(stderr, "unknown encoding option: %s\n", enc);
		fprintf(stderr, "Valid options are: sync async hdlc\n");
		return 1;
	}
 }
 return 0;
}

static int check_v110(struct rxtxcfg *cfg)
{
 if (cfg->smdc.protocol == kSMDCProtocolV110) {
	if (cfg->smdc.encoding == kSMDCConfigEncodingAsync) {
		cfg->m.v110.format = kSMDCFormatAsync;
	} else if (cfg->smdc.encoding == kSMDCConfigEncodingSync) {
		cfg->m.v110.format = kSMDCFormatSync;
	} else {
		fprintf(stderr, "V110 must specify encoding as 'sync' or 'async'\n");
		return 1;
	}
 	cfg->smdc.encoding = kSMDCConfigEncodingNone;
	cfg->smdc.encoding_config_data = 0;
	cfg->smdc.encoding_config_length = 0;
 }
 return 0;
}

static int lookup_ip(struct sockaddr **ip, char *addr, unsigned port)
{
	// NB this ALWAYS returns the first answer
 if (addr) {
	struct addrinfo hints;
	struct addrinfo *result;
	char portstr[8];
	int s;

	sprintf(portstr, "%u", port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;	/* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#else
	hints.ai_flags = 0;
#endif

	s = getaddrinfo(addr, portstr, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo(%s, %s..) : %s\n", addr, portstr, gai_strerror(s));
		return -1;
	}
	*ip = malloc(result->ai_addrlen);
	if (!*ip) {
		fprintf(stderr, "malloc failed\n");
		return -1;
	}
	memcpy(*ip, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
 } else {
 	*ip = NULL;
 }
 return 0;
}

#include "gen/fdxdc.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 CHAN_CONFIG chan;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 memset(&chan.rx.smdc, 0, sizeof(chan.rx.smdc));
 memset(&chan.tx.smdc, 0, sizeof(chan.tx.smdc));
 chan.rx.smdc.protocol = kSMDCProtocolNone;
 chan.tx.smdc.protocol = kSMDCProtocolNone;
 chan.tx_tep = 0;
 chan.tx_shorttrain = 0;
 if (setup_mod(&chan.rx, arg.rx_modulation, 0, arg.rx_bitrate, arg.rx_spacefreq, arg.tx_power, arg.rx_markfreq, arg.rx_carrier_on, arg.rx_carrier_off, arg.rx_answer, arg.rx_standard, arg.rx_xorval, arg.rx_i460_mask)) goto fail;
 if (setup_mod(&chan.tx, arg.tx_modulation, &chan.rx, arg.tx_bitrate, arg.tx_spacefreq, arg.tx_power, arg.tx_markfreq, 0, 0, arg.tx_answer, arg.tx_standard, arg.tx_xorval, arg.tx_i460_mask)) goto fail;
 if (chan.tx.smdc.protocol == kSMDCProtocolNone
 	&& chan.rx.smdc.protocol == kSMDCProtocolNone) {
	fprintf(stderr, "Neither rx nor tx has modulation option - nothing to do\n");
	goto fail;
 }
 if (setup_enc(&chan.rx, arg.rx_encoding, 0, arg.rx_crcsize, arg.rx_crc, arg.rx_databits, 0)) goto fail;
 if (setup_enc(&chan.tx, arg.tx_encoding, &chan.tx, arg.tx_crcsize, arg.tx_crc, arg.tx_databits, arg.tx_stopbits)) goto fail;
 if (check_v110(&chan.rx)) goto fail;
 if (check_v110(&chan.tx)) goto fail;
 chan.rx_outfile = arg.outfile;
 if (arg.format) chan.rx_format = arg.format;
 else chan.rx_format = "c";
 // arg.tx_patlen
 // arg.rx_patlen
 chan.rx.ts = arg.rx_timeslot;
 chan.tx.ts = arg.tx_timeslot;
 chan.rx.useevent = arg.rx_useevent;
 chan.tx.useevent = arg.tx_useevent;
 chan.rx_xferbits = arg.rx_xferbits;
 chan.rx_xfertimeout = arg.rx_xfertimeout;
 if (lookup_ip(&chan.tx_local_ip, arg.tx_local_ip, 0)) goto fail;
 if (lookup_ip(&chan.rx_dest_ip, arg.rx_dest_ip, arg.rx_dest_port)) goto fail;
 if (lookup_ip(&chan.rx_source_ip, arg.rx_source_ip, arg.rx_source_port)) goto fail;
 if (arg.serialnumber) {
	err_t err = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (err) {
		error_log(stderr, err);
		goto fail;
	}
 } else {
	fprintf(stderr, "No Prosody card specified\n");
	goto fail;
 }
 iErr = runrxtx(&chan, mod, arg.inifile);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 if (!iErr) return 0;
fail:
 fprintf(stderr, "%s: failed\n", progname);
 return 1;
}
