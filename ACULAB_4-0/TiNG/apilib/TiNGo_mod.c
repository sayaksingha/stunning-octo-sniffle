/* TiNGo_mod.c - module objects */

#include "TiNGo_mod.h"
#include "TiNGcommon.h"
#include "TiNGevent.h"
#include "smtypes.h"
#include "cardconn.h"
#include <math.h>
#include <stdlib.h>

#define arlen(x) (sizeof(x)/sizeof(*(x)))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const struct dettone defdettones[] = {
	{ 679.6875, 710.9375, },	// 0	DTMF low
	{ 742.1875, 789.0625, },	// 1
	{ 835.9375, 867.1875, },	// 2
	{ 914.0625, 960.9375, },	// 3
	{ 1179.6875, 1242.1875 },	// 4	DTMF high
	{ 1304.6875, 1367.1875 },	// 5
	{ 1445.3125, 1507.8125 },	// 6
	{ 1601.5625, 1664.0625 },	// 7
	{ 304.6875, 554.6875, },	// 8	CP 1
	{ 898.4375, 1007.8125, },	// 9	CP 2
	{ 1335.9375, 1460.9375, },	// 10	CP 3
	{ 1742.1875, 1851.5625, },	// 11	CP 4
	{ 589.0, 651.0, },		// 12	CP 5
	{ 1062.0, 1138.0, },		// 13	Fax CNG
	{ 2085.0, 2115.0, },		// 14	Fax CED
};

static const struct tonepair dtmf_tonelist[] = {
	{ 0, 4, },
	{ 1, 4, },
	{ 2, 4, },
	{ 3, 4, },
	{ 0, 5, },
	{ 1, 5, },
	{ 2, 5, },
	{ 3, 5, },
	{ 0, 6, },
	{ 1, 6, },
	{ 2, 6, },
	{ 3, 6, },
	{ 0, 7, },
	{ 1, 7, },
	{ 2, 7, },
	{ 3, 7, },
}, cp_tonelist[] = {
	{ TONEID_NONE, 8, },		// CP 1: 350, 400, 425, 440, 450, 480
	{ TONEID_NONE, 9, },		// CP 2: 913.8, 950, 985.2
	{ TONEID_NONE, 10, },		// CP 3: 1370.6, 1400, 1428.5
	{ TONEID_NONE, 11, },		// CP 4: 1776.7, 1800
	{ TONEID_NONE, 12, },		// CP 5: 620
	{ 8, 12, },			// CP 6: 480 + 620
	{ 8, 8, },			// CP 7: CP1 + CP1
	{ TONEID_NONE, 13, },		// Fax CNG
	{ TONEID_NONE, 14, },		// Fax CED
};

LOCALDEF const struct dettoneset defdettoneset[] = {
	{
		RAWCHAN_INIT,			// handle - not used here
		dtmf_tonelist,
		arlen(dtmf_tonelist),
		4,
		4,
		0.079432823,
		0.755573455,
		-36,		// dBm0
		10,
		MOD_DEF_DET_TONE_START,
		MOD_DEF_DET_TONE_STOP,
		0,
		0,
		0,
	}, {
		RAWCHAN_INIT,			// handle - not used here
		cp_tonelist,
		arlen(cp_tonelist),
		1,
		0,
		0.1,
		0.5,
		-36,		// dBm0
		10,
		MOD_DEF_DET_TONE_START,
		MOD_DEF_DET_TONE_STOP,
		0,
		0,
		1,
	},
};

static struct cptone unob_uk[] = {
	{ 1, ~0U, 5000, },	// 400 Hz
}, dial_uk[] = {
	{ 7, ~0U, 3000, },	// 350 + 440 Hz
}, busy_uk[] = {
	{ 1, 450, 300, },	// 400 Hz
	{ 0, 450, 300, },
	{ 1, 450, 300, },	// 400 Hz
	{ 0, 450, 300, },
}, busy_ec[] = {
	{ 1, 608, 352, },	// 425 Hz
	{ 0, 608, 352, },
	{ 1, 608, 352, },	// 425 Hz
	{ 0, 608, 352, },
}, path_eng_uk[] = {
	{ 1, 480, 320, },	// 400 Hz
	{ 0, 630, 420, },
	{ 1, 270, 180, },	// 400 Hz
	{ 0, 420, 280, },
}, ring_uk_1[] = {
	{ 1, 480, 320, },	// 400 or 450
	{ 0, 240, 160, },
	{ 1, 480, 320, },	// 400 or 450
}, ring_uk_2[] = {
	{ 1, 480, 320, },	// 400 or 450
	{ 0, 2400, 1600, },
	{ 1, 480, 320, },	// 400 or 450
}, ring_uk_3[] = {
	{ 1, 480, 300, },	// 400 + 450	(too close for dual)
	{ 7, 32, 0, },		// may get dual at transition
	{ 0, 240, 160, },
	{ 7, 32, 0, },		// may get dual at transition
	{ 1, 480, 300, },	// 400 + 450	(too close for dual)
}, ring_uk_4[] = {
	{ 1, 480, 300, },	// 400 + 450	(too close for dual)
	{ 7, 32, 0, },		// may get dual at transition
	{ 0, 2400, 1600, },
	{ 7, 32, 0, },		// may get dual at transition
	{ 1, 480, 300, },	// 400 + 450	(too close for dual)
}, ring_ecus[] = {
	{1, 1200, 800, },	// 440 + 480	(too close for dual)
	{0, ~0U, 1600, },	// (or 450 modulated by 50)
}, sit[] = {
	{ 4, 400, 260, },	// 1800
	{ 0, 32, 0, },
	{ 3, 400, 260, },	// 1400
	{ 0, 32, 0, },
	{ 2, 400, 260, },	// 950
}, a_fax_cng[] = {
	{ 8, 576, 416, },	// 1100
	{ 0, 3456, 2240, },
}, a_fax_ced[] = {
	{ 9, 4000, 2592, },
}, busy_us[] = {
	{6, 600, 400, },	// 480 + 620
	{0, 600, 400, },
	{6, 600, 400, },	// 480 + 620
	{0, 600, 400, },
}, ring_us_long[] = {
	{1, 2400, 1600, },	// 440 + 480	(too close for dual)
	{0, 4800, 3200, },
}, dial_us[] = {
	{7, ~0U, 3000, },	// 350 + 440
}, reorder_us[] = {
	{6, 300, 200, },	// 480 + 620
	{0, 300, 200, },
	{6, 300, 200, },	// 480 + 620
	{0, 300, 200, },
}, intercept_us[] = {
	{ 1, 276, 184, },	// 440
	{ 0, 32, 0, }, // allow for transition
	{ 5, 276, 184, },	// 620
}, sit_us_nc[] = {
	{ 4, 456, 304, },	// 1776.7
	{ 0, 32, 0, }, // allow for transition
	{ 3, 456, 304, },	// 1428.5
	{ 0, 32, 0, }, // allow for transition
	{ 2, 456, 304, },	// 985.2
}, sit_us_ic[] = {
	{ 4, 456, 304, },	// 1776.7
	{ 0, 32, 0, }, // allow for transition
	{ 3, 329, 219, },	// 1370.6
	{ 0, 32, 0, }, // allow for transition
	{ 2, 329, 219, },	// 913.8
}, sit_us_vc[] = {
	{ 4, 456, 304, },	// 1776.7
	{ 0, 32, 0, }, // allow for transition
	{ 3, 329, 219, },	// 1370.6
	{ 0, 32, 0, }, // allow for transition
	{ 2, 456, 304, },	// 985.2
}, sit_us_ro[] = {
	{ 4, 456, 304, },	// 1776.7
	{ 0, 32, 0, }, // allow for transition
	{ 3, 456, 304, },	// 1428.5
	{ 0, 32, 0, }, // allow for transition
	{ 2, 329, 219, },	// 913.8
};

static const struct cplist cplist[] = {
	{ unob_uk, arlen(unob_uk), 1, "unob_uk", },
	{ busy_uk, arlen(busy_uk), 2, "busy_uk", },
	{ busy_ec, arlen(busy_ec), 2, "busy_ec", },
	{ busy_us, arlen(busy_us), 2, "busy_us", },
	{ path_eng_uk, arlen(path_eng_uk), 3, "path_eng_uk", },
	{ ring_uk_1, arlen(ring_uk_1), 4, "ring_uk_1", },
	{ ring_uk_2, arlen(ring_uk_2), 4, "ring_uk_2", },
	{ ring_uk_3, arlen(ring_uk_3), 4, "ring_uk_3", },
	{ ring_uk_4, arlen(ring_uk_4), 4, "ring_uk_4", },
	{ ring_ecus, arlen(ring_ecus), 4, "ring_ecus", },
	{ ring_us_long, arlen(ring_us_long), 4, "ring_us_long", },
	{ sit, arlen(sit), 5, "sit", },
	{ a_fax_cng, arlen(a_fax_cng), 6, "a_fax_cng", },
	{ a_fax_ced, arlen(a_fax_ced), 7, "a_fax_ced", },
	{ dial_us, arlen(dial_us), 8, "dial_us", },
	{ dial_uk, arlen(dial_uk), 8, "dial_uk", },
	{ reorder_us, arlen(reorder_us), 9, "reorder_us", },
	{ intercept_us, arlen(intercept_us), 11, "intercept_us", },
	{ sit_us_nc, arlen(sit_us_nc), 5, "sit_us_nc", },
	{ sit_us_ic, arlen(sit_us_ic), 5, "sit_us_ic", },
	{ sit_us_vc, arlen(sit_us_vc), 5, "sit_us_vc", },
	{ sit_us_ro, arlen(sit_us_ro), 5, "sit_us_ro", },
};

static const struct fgentone defgentone[] = {
	{0, 0, },
	{697, -12, },
	{770, -12, },
	{852, -12, },
	{941, -12, },
	{1209, -10, },
	{1336, -10, },
	{1477, -10, },
	{1633, -10, },
	{400, -10, },
	{450, -10, },
	{425, -4.5, },
	{950, -13, },
	{1400, -13, },
	{1800, -13, },
	{350, -10, },
	{440, -10, },
	{1100, -11.5, },
	{2100, -11, },
	{350, -13, },
	{440, -13, },
	{440, -19, },
	{480, -17, },
	{480, -19, },
	{480, -24, },
	{620, -24, },
};

static const struct genpair defgenpair[] = {
	{4, 6},		// 0
	{1, 5},		// 1
	{1, 6},		// 2
	{1, 7},		// 3
	{2, 5},		// 4
	{2, 6},		// 5
	{2, 7},		// 6
	{3, 5},		// 7
	{3, 6},		// 8
	{3, 7},		// 9
	{1, 8},		// A
	{2, 8},		// B
	{3, 8},		// C
	{4, 8},		// D
	{4, 5},		// *
	{4, 7},		// #
	{9, 0},		// busy/path
	{9, 10},	// Ring
	{11, 0},	// E.C. dial/busy/unobtainable
	{12, 0},	// SIT
	{13, 0},	// SIT
	{14, 0},	// SIT
	{15, 16},	// U.K. dial tone
	{17, 0},	// fax CNG
	{18, 0},	// fax CED
	{19, 20},	// U.S. Dial
	{24, 25}, 	// U.S. busy/reorder
	{21, 23},	// U.S. Ring tone
	{22, 0},	// U.S. Alarm/Intercept
};

static const struct catsigpar defcatsigpar = {
	0.8,				// ln_nrg_coeff
	10108.0,				// min_Lmin
	1.0008,				// Lmin_decay
	0.01,				// speech_thresh
	8.0,				// debounce
	MOD_DEF_INITIAL_CATSIG_LMAX,	// initial Lmax
	152,				// min_valid_period_count
	112,				// min_valid_count
	48,				// glitch_count
	24,				// qualify_count
	160,				// alter_duration
	48,				// max_valid_tone_cnt
	64,				// min_valid_speech_cnt
	360,				// threshold_samp_cnt
	112,				// delay_time
	1480,				// period_time
	16,				// max_off_count
	600,				// min_period_off
};

LOCALDEF void close_tonesets(struct module *mp)
{
 unsigned i;
 for (i=0; i < mp->det.ntoneset; i++) {
	struct dettoneset *sp = mp->det.tonesetlist[i];
	if (sp) {
		CARDCONN *cx = &sp->chan.cx;
		if (cardconn_valid(cx)) {
			ERRCODE e = cx_close(cx);
			if (ERR(e) && TiNGtrace) printerr(e);
		}
		free((void *) sp->tonelist);
		free(sp);
	}
	mp->det.tonesetlist[i] = 0;
 }
}

LOCALDEF void mod_dtor(struct module *mp)
{
 struct cplist *cpl;
 unsigned i;
 close_tonesets(mp);
 free(mp->det.tonesetlist);
 free(mp->det.tonelist);
 free(mp->confman.id2in);
 free(mp->confman.id2out);
 free(mp->civman.id2in);
 free(mp->civman.id2out);
 	// must close module handle only when no channels open
 	// (as this waits for all ASSP connections to finish)
 	// this includes internal channels such as the toneset ones
 if (cardconn_valid(&mp->cx)) cx_close(&mp->cx);
 free(mp->genpair);
 free(mp->gentone);
 cpl = mp->cplist;
 for (i=0; i < mp->ncptone; i++, cpl++) {
	free(cpl->tonelist);
 }
 free(mp->cplist);
 free(mp->iwr);
 if (mp->ev) {
	tSMEventId *ep = mp->ev;
#ifdef TiNGTYPE_LINUX
	ep->eip->smd_ev_free(*ep);
	free(ep->eip);
#endif
#ifdef TiNGTYPE_QNX
	ep->eip->smd_ev_free(*ep);
	free(ep->eip);
#endif
#ifdef TiNGTYPE_WINNT
	if (!CloseHandle(*ep)) {
		ERRCODE e;
		e.text = "CloseHandle failed";
		e.errnum = GetLastError();
		if (TiNGtrace) printerr(e);
	}
#endif
	free(mp->ev);
 }
 mutx_dtor(&mp->pm.mx);
 mutx_dtor(&mp->det.mx);
 memset(mp, 0xa5, sizeof(*mp));		// for safety
 free(mp);
}

LOCALDEF int mkfgentone(struct fgentone *gp, double freq, double ampl)
{
 if (!freq) {
	gp->freq = 0;
	gp->ampl = 0;
 }
 if (ampl < -42.0 || ampl > 20.0 || freq < 1.0 || freq > 3600.0) return 1;
 gp->freq = freq;
 gp->ampl = ampl;
 return 0;
}

LOCALDEF int mkgentone(struct gentone *gp, double freq, double ampl, double rate)
{
 gp->freq = 0;
 gp->ampl = 0;
 if (ampl < -42.0 || ampl > 20.0 || freq < 1.0 || freq > 3600.0) return 1;
 freq *= 2 * M_PI / rate;
 // fix neg conversion for 32bit TiNG lib with CL 19.27.29111 for x86
 // eg 2400 -> -1.03689e+07 -> ff61c887 (before outcome was 0xffffffff)
 gp->freq = (U32)((S32)(cos(freq)*2 * (1<<24)));
 gp->ampl = (U32)((S32)(sin(freq) * pow(10, ampl / 20.0) * 5706.8 * (1<<16)));
 return 0;
}

STATIC int init_mod_cptone(struct module *mp)
{
 struct cplist *cpl = alloc(0, sizeof(cplist));
 if (!cpl) return 1;
 mp->cplist = cpl;
 mp->maxhist = 0;
 mp->mincont = 60000;
 for (mp->ncptone=0; mp->ncptone < arlen(cplist); cpl++, mp->ncptone++) {
	unsigned j;
	cpl->tonelist = dupmem(cplist[mp->ncptone].tonelist,
		sizeof(*cplist[mp->ncptone].tonelist)
		* cplist[mp->ncptone].listlen);
	if (!cpl->tonelist) return 1;
	for (j=0; j < cplist[mp->ncptone].listlen; j++) {
		const struct cptone *cpt = &cpl->tonelist[j];
		if (cpt->maxdur == ~0u && cpt->mindur < mp->mincont) {
			mp->mincont = cpt->mindur;
		}
	}
	cpl->listlen = cplist[mp->ncptone].listlen;
	cpl->id = cplist[mp->ncptone].id;
	cpl->dbgname = cplist[mp->ncptone].dbgname;
	if (mp->maxhist < cpl->listlen) mp->maxhist = cpl->listlen;
 }
 mp->cptoneset = 1;
 return 0;
}

LOCALDEF struct module *init_mod(struct card *card, unsigned long modmask)
{
 struct module *mp = alloc(0, sizeof(*mp));
 unsigned j;
 if (!mp) return 0;
 mp->unlock = 0;
 cardconn(&mp->cx);
 mp->ev = 0;
 mp->refs = 0;
 mp->genpair = 0;
 mp->gentone = 0;
 mp->ncptone = 0;
 mp->cplist = 0;
 mp->nmodel = 0;
 mp->iwr = 0;
 mutx_init(&mp->pm.mx);
 	// can now call mod_dtor to clean up
 mp->card = card;
 mp->modmask = modmask;
 mp->fwid = 0;
 mp->in_st0 = mp->in_st1 = STREAM_NONE;
 mp->in_type = TiNG_TSTYPE_COMPAND_ALAW; 
 mp->out_st0 = mp->out_st1 = STREAM_NONE;
 mp->out_type = TiNG_TSTYPE_COMPAND_ALAW; 
 cardconn(&mp->confman.chan.cx);
 mp->confman.maxconf = 0;
 mp->confman.id2in = 0;
 mp->confman.id2out = 0;
 cardconn(&mp->civman.chan.cx);
 mp->civman.maxconf = 0;
 mp->civman.id2in = 0;
 mp->civman.id2out = 0;
 mp->ngentone = arlen(defgentone);
 mp->gentone = alloc(0, sizeof(*mp->gentone) * arlen(defgentone));
 if (!mp->gentone) return 0;
 mp->ngenpair = arlen(defgenpair);
 mp->genpair = dupmem(defgenpair, sizeof(defgenpair));
 if (!mp->genpair) {
	mod_dtor(mp);
	return 0;
 }
 mutx_init(&mp->det.mx);
 mp->det.ntone = arlen(defdettones);
 mp->det.tonelist = dupmem(defdettones, sizeof(defdettones));
 if (!mp->det.tonelist) {
	mod_dtor(mp);
	return 0;
 }
 mp->det.ntoneset = 0;
 mp->det.tonesetlist = alloc(0, arlen(defdettoneset) * sizeof(*mp->det.tonesetlist));
 if (!mp->det.tonesetlist) {
	mod_dtor(mp);
	return 0;
 }
 for (j=0; j < arlen(defdettoneset); j++) {
	struct dettoneset *sp = dupmem(defdettoneset + j, sizeof(defdettoneset[j]));
	if (!sp) {
		mod_dtor(mp);
		return 0;
	}
	sp->tonelist = dupmem(sp->tonelist, sizeof(*sp->tonelist) * sp->numtones);
	mp->det.tonesetlist[j] = sp;
	cardconn(&sp->chan.cx);
	mp->det.ntoneset++;
 }
 for (j=0; j < arlen(defgentone); j++) {
	mkfgentone(&mp->gentone[j], defgentone[j].freq, defgentone[j].ampl);
 }
 if (init_mod_cptone(mp)) {
	mod_dtor(mp);
 	return 0;
 }
 mp->catsigpar = defcatsigpar;
 make_object(mp);
 return mp;
}
