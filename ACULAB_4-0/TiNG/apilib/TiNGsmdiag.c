/* TiNGsmdiag.i - Diagnostic API */

#include "TiNGsmdiag.h"
#include "TiNGo_mod.h"
#include "TiNGcommon.h"
#include "TiNGo_cardmap.h"
#include <stdlib.h>

LOCALDEF int real_sm_dspreg(struct sm_dspreg_parms *pp)
{
 struct module *mp = modid2lockedmodule(pp->module);
 struct ting_sharcmem mem;
 struct card *card;
 CARDCONN mf;
 int sts = 0;
 if (!mp) return err(ERR_SM_NO_SUCH_MODULE);
 card = mp->card;
 if (ERR(open_module(&mf, card, mp->modmask))) {
	unlock_module(mp);
	return err(ERR_SM_DEVERR);
 }
 unlock_module(mp);
 pp->winDiag = 0;
 pp->lastMessage[0] = 0;		// FIXME: maybe read these too
 pp->lastMessage[1] = 0;
 mem.ioctl_length = sizeof(mem);
 mem.address = pp->regId;
 mem.length = 1;
 switch (pp->cmd) {
 case kSMDSPRegCmdRead:
 	if (ERR(cx_ioctl(&mf, IOCTL_TiNG_RDMEM, &mem, sizeof(mem))))
 		sts = err(ERR_SM_DEVERR);
 	else pp->value = mem.val[0];
 	break;
 case kSMDSPRegCmdWrite:
 	mem.val[0] = pp->value;
 	if (ERR(cx_ioctl(&mf, IOCTL_TiNG_RDMEM, &mem, sizeof(mem))))
 		sts = err(ERR_SM_DEVERR);
 	break;
 default:
 	sts = err(ERR_SM_BAD_PARAMETER);
 }
 if (!sts) {
	struct ting_statistics stati;
	stati.ioctl_length = sizeof(stati);
	if (ERR(cx_ioctl(&mf, IOCTL_TiNG_RDSTATS, &stati, sizeof(stati))))
		sts = err(ERR_SM_DEVERR);
	else {
		pp->irqs0 = stati.numirq_run;
		pp->irqs1 = stati.numirq_ours;
	}
 }
 if (ERR(cx_close(&mf)) && !sts) sts = err(ERR_SM_DEVERR);
 return sts;
}

LOCALDEF int real_sm_dspregall(struct sm_dspregall_parms *pp)
{
 struct module *mod = modid2lockedmodule(pp->module);
 struct ting_sharcmem *mp;
 struct card *card;
 CARDCONN mf;
 int mpsize = sizeof(*mp)*3 + sizeof(mp->val)*7;
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 card = mod->card;
 if (ERR(open_module(&mf, card, mod->modmask))) {
	unlock_module(mod);
	return err(ERR_SM_DEVERR);
 }
 unlock_module(mod);
 pp->lastMessage[0] = 0;		// FIXME: maybe read these too
 pp->lastMessage[1] = 0;
 mp = alloc(0, mpsize);
 if (!mp) sts = err(ERR_SM_NO_RESOURCES);
 else {
	mp->ioctl_length = mpsize;
 	mp[0].address = 0x5c;		// GP9
 	mp[0].length = 1;
 	mp[1].address = pp->regId;
 	mp[1].length = 1;
 	mp[2].address = 8;		// MSGR0
 	mp[2].length = 8;
 	if (ERR(cx_ioctl(&mf, IOCTL_TiNG_RDMEM, mp, mpsize)))
 		sts = err(ERR_SM_DEVERR);
 	else {
 		int i;
 		pp->gp9 = mp[0].val[0];
 		pp->value = mp[1].val[0];
 		for (i=0; i<8; i++) pp->msgRegs[i] = mp[2].val[i];
 	}
 }
 free(mp);
 if (ERR(cx_close(&mf)) && !sts) sts = err(ERR_SM_DEVERR);
 return sts;
}

LOCALDEF int real_sm_mem(struct sm_mem_parms *pp)
{
 struct module *mod = modid2lockedmodule(pp->module);
 struct ting_sharcmem *mp;
 struct card *card;
 CARDCONN mf;
 int mpsize = sizeof(*mp) + sizeof(mp->val)*(pp->word_count-1);
 int sts = 0;
 if (!mod) return err(ERR_SM_NO_SUCH_MODULE);
 card = mod->card;
 if (ERR(open_module(&mf, card, mod->modmask))) {
	unlock_module(mod);
	return err(ERR_SM_DEVERR);
 }
 unlock_module(mod);
 mp = alloc(0, mpsize);
 if (!mp) sts = err(ERR_SM_NO_RESOURCES);
 else {
	mp->ioctl_length = mpsize;
 	mp->address = pp->addr;
 	mp->length = pp->word_count;
 	switch (pp->cmd) {
		int i;
 	case kSMMemCmdReadIntMem:
 	case kSMMemCmdReadExtMem:
		if (ERR(cx_ioctl(&mf, IOCTL_TiNG_RDMEM, mp, mpsize)))
			sts = err(ERR_SM_DEVERR);
		else {
			for (i=0; i<pp->word_count; i++)
				pp->data[i] = mp->val[i];
		}
		break;
 	case kSMMemCmdWriteIntMem:
 	case kSMMemCmdWriteExtMem:
		for (i=0; i<pp->word_count; i++)
			mp->val[i] = pp->data[i];
		if (ERR(cx_ioctl(&mf, IOCTL_TiNG_WRMEM, mp, mpsize)))
			sts = err(ERR_SM_DEVERR);
		break;
 	default:
		sts = err(ERR_SM_BAD_PARAMETER);
	}
 }
 free(mp);
 if (ERR(cx_close(&mf)) && !sts) sts = err(ERR_SM_DEVERR);
 return sts;
}

#if 0
STATIC int real_sm_diag_action()
{
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int real_sm_diag_os()
{
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int real_sm_diag_jobmsg()
{
 return err(ERR_SM_BAD_PARAMETER);
}

STATIC int real_sm_drvstats()
{
 return err(ERR_SM_BAD_PARAMETER);
}
#endif
