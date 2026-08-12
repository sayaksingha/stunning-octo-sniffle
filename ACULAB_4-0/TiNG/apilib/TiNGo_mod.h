/* TiNGo_mod.h - module objects */

#ifndef INCLUDED_TINGO_MOD_H
#define INCLUDED_TINGO_MOD_H

struct module;

#include "../TiNG.h"
#include "paranoia.h"
#include "rawchan.h"
#include "smtypes.h"
#include "stream.h"

struct dettone {
	float minfreq, maxfreq;
};

#define TONEID_NONE ~0u

struct tonepair {
	unsigned tlow, thigh;
};

struct dettoneset {
	RAWCHAN chan;	// handle to task holding tone set
	const struct tonepair *tonelist;
	unsigned numtones;
	unsigned band1len;
	unsigned band2len;
	float req_third_peak;
	float req_signal_to_noise_ratio;
	float req_minimum_power;
	float req_twist_for_dual_tone;
	float start_freq;
	float stop_freq;
	unsigned mintime;
	unsigned mintimeoff;
	unsigned narrow;
};

#define MOD_DEF_DET_TONE_START 250
#define MOD_DEF_DET_TONE_STOP 3406

struct cptone {
	unsigned id, maxdur, mindur;
};

struct cplist {
	struct cptone *tonelist;
	unsigned listlen;
	unsigned id;
	char *dbgname;
};

struct fgentone {
	float freq;
	float ampl;
};

struct gentone {
	unsigned freq;
	unsigned ampl;
};

struct genpair {
	unsigned a, b;
};

#define MOD_DEF_INITIAL_CATSIG_LMAX 1.0e8f

struct catsigpar {
	float ln_nrg_coeff;
	float min_Lmin;
	float Lmin_decay;
	float speech_thresh;
	float debounce;
	float initial_Lmax;
	unsigned min_valid_period_count;
	unsigned min_valid_count;
	unsigned glitch_count;
	unsigned qualify_count;
	unsigned alter_duration;
	unsigned max_valid_tone_cnt;
	unsigned max_valid_speech_cnt;
	unsigned threshold_samp_cnt;
	unsigned delay_time;
	unsigned period_time;
	unsigned max_off_count;
	unsigned min_period_off;
};

struct module {
	PARAMUTX pm;
	void (*unlock)(struct module *mp);
	struct card *card;
	unsigned long modmask;
	CARDCONN cx;
	tSMEventId *ev;
	unsigned refs;
	tSM_INT fwid;
	STREAM in_st0, in_st1;
	STREAM out_st0, out_st1;
	enum TiNG_COMPAND_TYPE in_type;
	enum TiNG_COMPAND_TYPE out_type;
		// tone detection
	struct {
		MUTX mx;
		unsigned ntone;
		struct dettone *tonelist;	// all simple tones
		unsigned ntoneset;
		struct dettoneset **tonesetlist;
	} det;
	struct cplist *cplist;
	unsigned maxhist;
	unsigned mincont;
	unsigned ncptone;
	unsigned cptoneset;
		// signal categorisation
	struct catsigpar catsigpar;
		// tone generation
	unsigned ngentone;
	struct fgentone *gentone;
	unsigned nmodel;
	struct iwr_model {
		unsigned modtaskid;
		RAWCHAN rch;
	} *iwr;
	unsigned ngenpair;
	struct genpair *genpair;
	struct {
		RAWCHAN chan;
		unsigned maxconf;
			// translate conf id to channel
			// pointers to arrays of length 'maxconf'
		struct tSMChannelId_struct **id2in;
		struct tSMChannelId_struct **id2out;
	} confman;
	struct {
		RAWCHAN chan;
		unsigned maxconf;
			// translate conf id to channel
			// pointers to arrays of length 'maxconf'
		struct tSMChannelId_struct **id2in;
		struct tSMChannelId_struct **id2out;
	} civman;
};

LOCALDEC void close_tonesets(struct module *mp);
LOCALDEC void mod_dtor(struct module *mp);
	// utility: convert the freq/ampl pair to a gentone
LOCALDEC int mkfgentone(struct fgentone *gp, double freq, double ampl);
LOCALDEC int mkgentone(struct gentone *gp, double freq, double ampl, double rate);
LOCALDEC struct module *init_mod(struct card *card, unsigned long modmask);

#endif
