/* TiNGo_vmptone.c - TiNG vmptone objects */

#include "TiNGo_vmptone.h"
#include "TiNGcommon.h"
#include <stdlib.h>
#include <string.h>
#include "px_prostype.h"

#define arlen(x) (sizeof(x)/sizeof(*(x)))

LOCALDEF void vmptone_dtor(struct tSMVMPTxToneSetId_struct *vtp)
{
 refcnt_dec(&vtp->ref);
}

STATIC void free_vmptone(void *p)
{
 struct tSMVMPTxToneSetId_struct *vtp = p;
 vtp->groove.dtor(&vtp->groove);
 free(vtp);
}

LOCALDEF struct tSMVMPTxToneSetId_struct *init_vmptone(struct module *mod)
{
 struct tSMVMPTxToneSetId_struct *vtp = alloc(0, sizeof(*vtp));
 if (!vtp) return 0;
 memset(vtp, 0, sizeof(*vtp));
 if (SMGroove_module(&vtp->groove, mod)) {
        free(vtp);
        return 0;
 }
 refcnt(&vtp->ref, free_vmptone, vtp);
 return vtp;
}


static tSMVMPTxTone DEFAULTVMPTXTONES[]={{679.6875,710.9375,1179.6875,1242.1875,1},
				{679.6875,710.9375,1304.6875,1367.1875,2},
				{679.6875,710.9375,1445.3125,1507.8125,3},
				{679.6875,710.9375,1601.5625,1664.0624,12},
				{742.1875,789.0625,1179.6875,1242.1875,4},
				{742.1875,789.0625,1304.6875,1367.1875,5},
				{742.1875,789.0625,1445.3125,1507.8125,6},
				{742.1875,789.0625,1601.5625,1664.0624,13},
				{835.9375,867.1875,1179.6875,1242.1875,7},
				{835.9375,867.1875,1304.6875,1367.1875,8},
				{835.9375,867.1875,1445.3125,1507.8125,9},
				{835.9375,867.1875,1601.5625,1664.0624,14},
				{914.0625,960.9375,1179.6875,1242.1875,10},
				{914.0625,960.9375,1304.6875,1367.1875,0},
				{914.0625,960.9375,1445.3125,1507.8125,11},
				{914.0625,960.9375,1601.5625,1664.0624,15}};

static struct ttSMVMPTxToneSet DEFAULTTONES=
                {-36,10,0.079432823,0.755573455,arlen(DEFAULTVMPTXTONES),DEFAULTVMPTXTONES};

ACUAPI tSMVMPTxToneSet *kSMVMPTxDefaultToneSet = &DEFAULTTONES;
