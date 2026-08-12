/* TiNGiwr.i - TiNG isolated word recognition */

#include "TiNGiwr.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hsif/kernel.h"
#include "../hsif/iwr.h"
#include "../hsif/typecode.h"
#include "TiNGcommon.h"
#include "TiNGanyevent.h"
#include "TiNGo_datafeed.h"
#include "smbesp.h"
#include "trace.h"

LOCALDEF int real_sm_reset_input_vocabs(struct sm_reset_input_vocabs_parms *vocabp)
{
 struct module *mp = modid2lockedmodule(vocabp->module);
 unsigned u;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 for (u=0; u < mp->nmodel; u++) {
	if (ERR(cx_close(&mp->iwr[u].rch.cx))) {
		sts = err(ERR_SM_DEVERR);
	}
 }
 free(mp->iwr);
 mp->iwr = 0;
 mp->nmodel = 0;
 unlock_module(mp);
 return sts;
}

STATIC int readfile(char **buf, unsigned *size, char *fname)
{
 FILE *inf = fopen(fname, "rb");
 *buf = 0;
 *size = 4;		// reserve space for length
 if (!inf) {
	if (TiNGtrace) print_errno(errno, "fopen() failed");
	return err(ERR_SM_FILE_ACCESS);
 }
 for (;;) {
 	char *bp = alloc(*buf, *size + 8192);
	int nc;
	if (!bp) return err(ERR_SM_OS_RESOURCE_PROBLEM);
	*buf = bp;
	nc = fread(bp + *size, 1, 8192, inf);
	if (nc < 0) {
		if (TiNGtrace) print_errno(errno, "fread() failed");
		return err(ERR_SM_FILE_ACCESS);
	}
	if (!nc) {
		unsigned len = *size;
		if (fclose(inf)) {
			if (TiNGtrace) print_errno(errno, "fclose() failed");
			return err(ERR_SM_FILE_ACCESS);
		}
		if (len & 3) return err(ERR_SM_FILE_FORMAT);
		len >>= 2;
			// fill in length (in words)
		bp = *buf;
		*bp++ = len & 0xff; len >>= 8;
		*bp++ = len & 0xff; len >>= 8;
		*bp++ = len & 0xff; len >>= 8;
		*bp++ = len & 0xff; len >>= 8;
		return 0;
	}
	*size += nc;
 }
}

STATIC int iwr_sendfile(RAWCHAN mdl, char *fname)
{
 unsigned bufsize;
 char *buf;
 int sts = readfile(&buf, &bufsize, fname);
 if (!sts) sts = sendmsg2(&mdl.cx, MSG_IWR_MODEL_SIZE, bufsize >> 2);
 if (!sts) {
	unsigned pos = 0;
	for (pos=0; pos < bufsize;) {
		unsigned wlen;
		ERRCODE e;
		e = cx_write(&mdl.cx, buf + pos, bufsize - pos, &wlen);
		if (ERR(e)) {
			sts = err(ERR_SM_DEVERR);
			break;
		}
		pos += wlen;
		if (!wlen) {
			// not all written - must be message
			// fetch message - stop if DEAD
			sts = err(ERR_SM_DOWNLOAD);
			break;
		}
	}
 }
 if (!sts) {
	struct ting_xfer xfer;
	xfer.ioctl_length = sizeof(xfer);
	xfer.minimum_bits = 1<<24;	// very large number
	xfer.timeout_milliseconds = 0;
	xfer.nomore = 1;
	if (ERR(cx_ioctl(&mdl.cx, IOCTL_TiNG_WRXFER, &xfer, sizeof(xfer)))) {
		sts = err(ERR_SM_DEVERR);
	}
 }
 free(buf);
 return sts;
}

STATIC int iwr_download(tSM_UT32 *idp, RAWCHAN mdl, struct module *mp, char *fname)
{
 int sts = iwr_sendfile(mdl, fname);
 if (sts) return sts;
 for (;;) {
	struct ting_message msg;
	int i;
	unsigned long code;
	msg.flags = 0;
	i = rdmsg(&msg, &mdl.cx);
	if (i) return i;
	code = msg.msg[0] & ~0xffff;
	if ((code & 0xfff00000) == MESSAGE(MSG_DEAD, 0)) return err(ERR_SM_DEVERR);
	if (code == MESSAGE(MSG_IWR_MODEL, 2)) {
		unsigned id = msg.msg[1];
		unsigned u;
		lock_module(mp);
		for (u=0; ; u++) {
			if (u >= mp->nmodel) {	// must be a new one
				struct iwr_model *amp = alloc(mp->iwr, (mp->nmodel + 1) * sizeof (*amp));
				if (!amp) sts = err(ERR_SM_OS_RESOURCE_PROBLEM);
				else {
					mp->iwr = amp;
					mp->nmodel++;
					mp->iwr[u].rch = mdl;
					mp->iwr[u].modtaskid = id;
					*idp = u;
				}
				break;
			}
			if (mp->iwr[u].modtaskid == id) {
					// duplicate - use original
				*idp = u;
				if (TiNGtrace > 1) olog("(Duplicate iwr vocab file %s)\n",fname);
				cx_close(&mdl.cx);
				break;
			}
		}
		unlock_module(mp);
		return sts;
	}
 }
}

LOCALDEF int real_sm_add_input_vocab(struct sm_input_vocab_parms *vocabp)
{
 struct module *mp = modid2lockedmodule(vocabp->module);
 RAWCHAN mdl;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 sts = open_channel(&mdl, mp);
 unlock_module(mp);
 if (sts) return sts;
 sts = starttask(&mdl.cx, TASKTC_IWRMODEL, 0);
 if (!sts) sts = iwr_download(&vocabp->item_id, mdl, mp, vocabp->filename);
 if (sts) cx_close(&mdl.cx);
 return sts;
}

static struct sm_asr_characteristics default_asr_characteristics = {
	8,	// vfr_max_frames
	0.5,	// vfr_diff_threshold
	63,	// pse_max_frames
	21,	// pse_min_frames
	0.16,	// vit_soft_threshold
	0.08,	// vit_hard_threshold
	0.0,	// vit_snr_adjust
};

STATIC int start_iwr_listen(struct sm_asr_listen_for_parms *pp, enum handles h, int isrec)
{
 const tSMChannelId cp = pp->channel;
 int sts;
 cp->recog[0].numrecog = pp->vocab_item_count;
 cp->recog[0].recog_id = alloc(0, cp->recog[0].numrecog * sizeof(*cp->recog[0].recog_id));
 if (!cp->recog[0].recog_id) return err(ERR_SM_OS_RESOURCE_PROBLEM);
 sts = startdet(cp, h, DETTYPE_IWR, isrec);
 if (!sts) {
	const struct module *mod = cp->mod;
	int i;
	for (i=0; i < pp->vocab_item_count; i++) {
		unsigned modelno = pp->vocab_item_ids[i];
		if (modelno >= mod->nmodel) {
			sts = err(ERR_SM_BAD_PARAMETER);
			break;
		}
		sts = setfriend(&cp->chan[h].cx,
			&mod->iwr[modelno].rch, FRIEND_TYPE_IWR_MODEL);
		cp->recog[0].recog_id[i] = pp->vocab_recog_ids[i];
	}
	if (!sts) {
		struct sm_asr_characteristics *acp = pp->specific_parameters;
		struct ting_message msg;
		if (!acp) acp = &default_asr_characteristics;
		memset(&msg, 0, sizeof(msg));
		msg.ioctl_length = sizeof(msg);
		msg.flags = 0;
		msg.msg[0] = MESSAGE(MSG_IWR_SETPAR, 8);
		msg.msg[1] = acp->vfr_max_frames;
		msg.msg[2] = acp->vfr_diff_threshold * 65536;
		msg.msg[3] = acp->pse_max_frames;
		msg.msg[4] = acp->pse_min_frames;
		msg.msg[5] = acp->vit_soft_threshold * 65536;
		msg.msg[6] = acp->vit_hard_threshold * 65536;
		msg.msg[7] = acp->vit_snr_adjust * 65536;
		if (ERR(cx_ioctl(&cp->chan[h].cx, IOCTL_TiNG_WRMSG, &msg, sizeof(msg))))
		sts = err(ERR_SM_DEVERR);
	}
	if (!sts && pp->sidetone) {
		const tSMChannelId side = pp->sidetone;
		if (side != cp) lock_object(side);
		sts = cp->mod->card->vtbl->engage_df(side->df[0], &cp->chan[HANDLE_DET].cx, DFC_IWRSIDE);
		if (side != cp) unlock_object(side);
	}
	if (sts) stopdet(cp, h, DETTYPE_IWR, 0);
 }
 return sts;
}

LOCALDEF int real_sm_asr_listen_for(struct sm_asr_listen_for_parms *pp)
{
 const tSMChannelId cp = pp->channel;
 int sts = 0;
#ifndef OMIT_ANYCHAN
 const int lockany = anychan.inuse & (1 << EVENT_NOTIFY);
 if (lockany) lock_object(&anychan);
#endif
 lock_module(cp->mod);
 lock_object(cp);
 	// stop any previous detections
 if (!sts) sts = stopdet(cp, HANDLE_DET, DETTYPE_IWR, 0);
 free(cp->recog[0].recog_id);
 cp->recog[0].numrecog = 0;
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate != ANYEV_NOTANY) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_IDLE);
 }
#endif
 if (!sts) switch (pp->asr_mode) {
	case kSMASRModeDisabled:
		break;
	case kSMASRModeContinuous:
		sts = start_iwr_listen(pp, HANDLE_DET, 0);
		break;
	default:
		sts = err(ERR_SM_BAD_PARAMETER);
		break;
 }
#ifndef OMIT_ANYCHAN
 if (!sts && cp->anyev[EVENT_NOTIFY].evstate == ANYEV_IDLE) {
	sts = anychan_newstate(&cp->anyev[EVENT_NOTIFY], ANYEV_WAIT);
 }
#endif
 unlock_object(cp);
 unlock_module(cp->mod);
#ifndef OMIT_ANYCHAN
 if (lockany) unlock_object(&anychan);
#endif
 return sts;
}
