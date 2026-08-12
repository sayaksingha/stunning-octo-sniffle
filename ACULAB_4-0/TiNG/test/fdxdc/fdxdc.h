#ifndef INCLUDED_FDXDC_H
#define INCLUDED_FDXDC_H

#include "smdrvr.h"
#include "smdc.h"

	// encodings
#include "smdc_async.h"
#include "smdc_hdlc.h"
#include "smdc_none.h"
#include "smdc_sync.h"

	// protocols
#include "smdc_raw.h"
#include "smdc_cw.h"
#include "smdc_fsk.h"
#include "smdc_v110.h"
#include "smdc_rlp.h"
#include "smdc_v17.h"
#include "smdc_v27.h"
#include "smdc_v29.h"
#include "smdc_v32.h"

typedef struct {
	int stream;
	int timeslot;
	int type;
} MVIP;
#define MVIP_STREAM_NONE 0xff

typedef struct {
		// rx only settings
	struct sockaddr *rx_dest_ip;
	struct sockaddr *rx_source_ip;
	char *rx_format;
	char *rx_outfile;
	int rx_xferbits;
	unsigned rx_xfertimeout;
		// tx only settings
	int tx_tep;
	int tx_shorttrain;
	struct sockaddr *tx_local_ip;
	struct rxtxcfg {
		SMDC_CHANNEL_CONFIG_PARMS smdc;
		MVIP ts;
		int isanswer;
		int useevent;
		union {
			SMDC_ASYNC_FORMAT_PARMS async;
			SMDC_HDLC_FORMAT_PARMS hdlc;
		} e;
		union {
			SMDC_RAW_CONFIG_PARMS raw;
			SMDC_CW_CONFIG_PARMS cw;
			SMDC_FSK_CONFIG_PARMS fsk;
			SMDC_V17_CONFIG_PARMS v17;
			SMDC_V27_CONFIG_PARMS v27;
			SMDC_V29_CONFIG_PARMS v29;
			SMDC_V32_CONFIG_PARMS v32;
			SMDC_V110_CONFIG_PARMS v110;
			SMDC_V110RLP_CONFIG_PARMS v110rlp;
		} m;
	} rx, tx;
} CHAN_CONFIG;

typedef struct {
	tSMChannelId chan;
	tSMCollectorId coll;
	tSMEventId txev, rxev;
	CHAN_CONFIG *cfg;
} CHAN;

void *rxmain(void *p);	// actually takes CHAN *
int setup_rxtx(CHAN *chan, tSMModuleId mod);
int runtx(CHAN *chanp, char *inifile);

#endif
