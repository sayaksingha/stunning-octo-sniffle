/* getmodinfo.c - show value of sm_get_module_info() */

#include <stdio.h>
#include <string.h>
#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "smdrvr.h"

static int modinfo(tSMModuleId module)
{
 SM_MODULE_INFO_PARMS mip;
 int r;
 memset(&mip, 0, sizeof(mip));
 mip.module = module;
 r = sm_get_module_info(&mip);
 if (r) return printerr_sm("sm_get_module_info", r);
 printf("%s id:%x %3d=avload %3d=maxload slot=%d minstream=%d\n",
	mip.firmware_running ? "running" : "not-running",
	mip.firmware_id,
	mip.average_loading,
	mip.max_loading,
	mip.module_slot,
	mip.min_stream);
 return 0;
}

#include "gen/getmodinfo.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 unsigned i;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (!arg.number) arg.number = 1;
 for (i = 0; i < arg.number; i++) {
	if (arg.serialnumber) {
		err_t e = modopen(&card, &mod, arg.serialnumber, arg.module);
		if (e) {
			error_log(stderr, e);
			return 1;
		}
	} else {
		fprintf(stderr, "%s: no Prosody card specified\n", progname);
		return 1;
	}
	iErr = modinfo(mod);
	if (arg.serialnumber) {
		modclose(mod);
		cardclose(card);
	}
	if (iErr) break;
 }
 return iErr;
}
