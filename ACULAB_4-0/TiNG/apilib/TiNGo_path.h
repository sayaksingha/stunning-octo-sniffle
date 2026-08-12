/* TiNGo_path.h - TiNG path objects */

#ifndef INCLUDED_TINGO_PATH_H
#define INCLUDED_TINGO_PATH_H

#include "paranoia.h"
#include "TiNGo_mod.h"
#include "smgroove.h"

struct tSMPathId_struct {
	PARAMUTX pm;
	struct module *mod;
	tSMGroove groove;
	enum {PATH_STATE_IDLE, PATH_STATE_RUNNING, PATH_STATE_STOPPING} state;
	DATAFEED dataf; //equivalent to tSMDatafeedId
	DATAFEED dfin;
	struct {
		int running;
		DATAFEED refdf;
	} ec;
	struct {
		int running;
	} agc;
	struct {
		int running;
	} td;
	struct {
		int running;
		DATAFEED bgdf;
	} mix;
	struct {
		int running;
	} pitchshift;
	struct {
		int running;
	} resample;
	struct {
		int running;
	} emphasis;
	struct {
		int running;
	} delay;
	struct {
		enum {
			kPathCodecNone,
			kPathCodecAMRNBDec,
			kPathCodecAMRNBEnc,
		} running;
	} codec;
	struct {
		int running;
	} ttyeliminate;
};

LOCALDEC void path_dtor(struct tSMPathId_struct *vtp);
LOCALDEC struct tSMPathId_struct *init_path(struct module *mod);

#endif
