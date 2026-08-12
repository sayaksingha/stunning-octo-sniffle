/* TiNGo_chan.h - channel objects */

#ifndef INCLUDED_TINGO_CHAN_H
#define INCLUDED_TINGO_CHAN_H

#include "../TiNG.h"
#include "confmask.h"
#include "TiNGo_coll.h"
#include "TiNGo_datafeed.h"
#include "TiNGo_disp.h"
#include "event.h"
#include "paranoia.h"
#include "rawchan.h"
#include "smtypes.h"
#include "stream.h"

#define CONFIN_ID_NONE ~0u

enum handles { 
	// output handles first
	HANDLE_WRITE,
	// then inputs (some loops use echo as first input)
	HANDLE_ECHO, 
	HANDLE_READ,
	// then detection
	HANDLE_DET,
	HANDLE_NUM
};

#define HANDLE_ALL HANDLE_NUM

enum event_types { EVENT_READ, EVENT_WRITE, EVENT_NOTIFY, EVENT_NUM };

struct tSMChannelId_struct {
	PARAMUTX pm;
	void *vtbl_data;
	RAWCHAN chan[HANDLE_NUM];
		// the events associated with this channel
		// mostly needed so that nonidle works as that needs
		// to dynamically disassociate and re-associate events
		// with channels
	struct {
		int nonidle;
	} event[EVENT_NUM];
#ifndef OMIT_ANYCHAN
	struct chanev {
			// this is part of the channel structure but these
			// fields are protected by the anychan mutex - not
			// the channel's mutex
		enum event_types etyp;
		struct chanev **prevp, *next;
		enum evstate {
			ANYEV_NOTANY,	// not involved with anychan
			ANYEV_IDLE,	// no operation in progress
			ANYEV_WAIT,	// waiting for readiness
			ANYEV_READY,	// possibly ready
		} evstate;
		tSMEventId intev;	// 'real' event during anychan
#ifdef TiNGTYPE_LINUX
			// position in newest waitspec (0=none)
		unsigned waitpos;
#endif
#ifdef TiNGTYPE_QNX
			// position in newest waitspec (0=none)
		unsigned waitpos;
#endif
#ifdef TiNGTYPE_WINNT
		struct waiter *slave;	// null if not on any slave's list
		unsigned waitpos;	// position on slave's list (0=none)
#endif
		struct tSMChannelId_struct *chan;// points to our channel
	} anyev[EVENT_NUM];
#endif
	int type;			// kSMChannelType*
	struct {
		CONFMASK outmask;
		unsigned id;		// CONFIN_ID_NONE if not enabled
		int attached;
		int volume;
		int agc;
		double min_lmin;
		double speech_thresh;
	} confin;
	struct {
		CONFMASK outmask;
		unsigned id;		// CONFIN_ID_NONE if not enabled
	} civin;
	struct module *mod;
	int caps_mask;
	tSM_INT channel_ix;
		// detection stuff
	struct {
		unsigned detmask;			// detections running
		unsigned tone_detect_map_digits;	// map flag
		unsigned tone_detect_end;		// end flag
		unsigned tone_detect_band1len;		// length when tone started
		unsigned tone_detect_len;		// len flag
		struct hist {				// history for cptone
			unsigned id;
			unsigned dur;
		} *hist;
		unsigned histlen;		// length of hist[]
		unsigned hist_cont;		// last tone was continuous
		unsigned numrecog;		// length of recog_id
		unsigned *recog_id;
	} recog[2];				// 0=normal, 1=record
	unsigned catsig;			// non zero if catsig is running
		// rx and tx stuff - first the part which appears in both
	DATAFEED df[2];	// datafeed output and input
	DATAFEED ecref;
	DATAFEED ecaltdest;
	unsigned ecloopback; // non zero if echo canceller dest is same as input
	COLLECTOR *collector;
	DISPATCHER *dispatcher;
	DATAFEED redirect_rx;
	DATAFEED redirect_tx;
	int redirect_tx_threshold;
	int redirect_tx_timeout;
	unsigned out_sample_rate;
	struct {
		// reserved value for 'pos' field
#define SERTS_NONE ~0u
		enum rxtxf {
			RXTXF_NONE,
			RXTXF_PLAYREC,		// playing or recording
			RXTXF_GENSTONE,		// generating simple tone
			RXTXF_GENCPTONE,	// generating CP tone
			RXTXF_GENDTMF,		// generating DTMF tone
			RXTXF_GENTONELIST,	// generating a tone list
			RXTXF_CONFOUT,		// a std conference output
			RXTXF_CIVOUT,		// a CIV conference output
			RXTXF_DATA,		// smdc_config has been used
			RXTXF_INVALID=0x7fffffff, // can't have function e.g. simplex channel
		} func;
			// the state is only valid when func != NONE
		enum {
			RXTXS_RUN,	// configured for a function
			RXTXS_LAST,	// terminating: e.g. awaiting DEAD msg
					// did (play) put_last
					// or (record) abort+discard
					// or (data) finish
					// or (tone) got MARK, killed
			RXTXS_DONE,	// task is DEAD but not yet reported
		} state;		// [0]=Output/tx, [1]=Input/rx
		int underrun;		// overrun for rx
		struct protable *protocol;
		int minimum_bits;
		unsigned timeout_milliseconds;
		unsigned dataready;	// set from last DATA message
		void (*dtor)(struct tSMChannelId_struct *);
		int gotready;	// backward comapatibity kludge
		DATAFEED auxdf;
	} rxtx[2];		// 0=Output/tx, 1=Input/rx
		// rx and tx stuff - tx only items
	union {			// selected by 'rxtx[0].func'
		struct {
			struct {
				CONFMASK inmask;
				int id;
				int volume;
				int agc;
			} confout;
			struct {
				float factor[64];	// for each input
				CONFMASK inmask;
				int id;
			} civout;
			struct rxtx {
				unsigned length;	// amount sent
				unsigned maxlength;	// limit
				unsigned laststop;
				unsigned capacity;
			} rxtx;
				// for playing cptones
				// 'rxtx.length' * 2 + mark = buf size
				// 'rxtx.maxlength' is main bytes to send
			struct {
				char *tonebuf;
				int repeat;
				enum {
					TGEN_NONE, // not running or aborted
					TGEN_TASK, // started as task
					TGEN_SUBTASK, // started as subtask
				} started_task;
				unsigned phase_reversal_period;
			} tone;
			unsigned speed;			// percent
			int gainbg;			// gainbg has started
		} besp;
		struct {
			struct enctbl *encoding;
			union {
				struct {
					int carrier;
					float power;
				} fsk;
				struct {
					int carrier;
				} ifp;
				struct {
					int carrier;
				} v17;
				struct {
					int carrier;
				} v27;
				struct {
					int carrier;
				} v29;
				struct {
					int carrier;
				} v32;
				struct {
					int status;
					int changing_rate;
					int user_capacity;
					int user_blocking;
					enum  {
						eV34HDPrimaryRx,
						eV34HDPrimaryTx,
						eV34FD,
					} dir;
					int tx_data_disabled; // non zero when data can not be transmitted (e.g. during switch from control to primary)
				} v34;
			} u;
		} dc;
	} ut;
		// rx and tx stuff - rx only items
	union {			// selected by 'rxtx[1].func'
		struct {
			struct rxtx rxtx;
			unsigned sample_rate;
			unsigned rate_factor; // 8000 means no adjustment, used for 6k and 11k
			enum {
				RECEND_RUNNING,
				RECEND_ERROR,
				RECEND_ABORT,
				RECEND_LENGTH,
				RECEND_SILENCE,
				RECEND_MAXTIME,
			} reason;
		} besp;
		struct {
			struct enctbl *encoding;
			union {
				struct {
					unsigned speed;
				} cw;
				struct {
					unsigned error;
					int carrier;
				} ifp;
				struct {
					int synced;
				} rlp;
				struct {
					int synced;
						// 48K: ... S1 X S3 S4
						// others: ... S1 X S3  S4 E1 E2 E3  E4 E5 E6 E7  S6 X S8 S9
					unsigned long last_sxe;
				} v110;
				struct {
					unsigned speed;
					unsigned filtthresh;
					unsigned rx_carrier_off_mS;
					unsigned rx_carrier_on_mS;
					int carrier;
					int gensigstats;
					int sigstats;
				} fsk;
				struct {
					int carrier;
				} tty;
				struct {
					int carrier;
				} v17;
				struct {
					int carrier;
				} v27;
				struct {
					int carrier;
				} v29;
				struct {
					int carrier;
				} v32;
				struct {
					int carrier;
					int link_status;
					int status;
					int snr_status;
					int a_bitrate; // hdx = control, fdx = rx
					int b_bitrate; // hdx = primary, fdx = tx
				} v34;
				struct {
					int protocol;
				} t38;
			} u;
		} dc;
	} ur;
};

#include "cardconn.h"

LOCALDEC void chan_dtor(struct tSMChannelId_struct *cp);
LOCALDEC struct tSMChannelId_struct *init_chan(struct module *mod);

#include "TiNGo_mod.h"

#endif
