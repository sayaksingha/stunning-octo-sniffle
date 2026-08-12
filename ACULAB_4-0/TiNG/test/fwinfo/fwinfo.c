/* fwinfo.c - show version codes of downloaded firmware */

#include <stdio.h>
#include <string.h>
#include "../Testlib/cardopen.h"
#include "../Testlib/errcode_sm.h"
#include "../Testlib/modopen.h"
#include "smdrvr.h"

static int fwinfo(tSMModuleId module)
{
 SM_FWINFO_PARMS fwp;
 memset(&fwp, 0, sizeof(fwp));
 fwp.module = module;
 for (;;) {
 	int r = sm_get_firmware_info(&fwp);
	if (r) return printerr_sm("sm_get_firmware_info", r);
	if (!fwp.position) break;
	printf("0x%08lx (%c%c%c%c) v%x\n",
		(unsigned long) fwp.tasktypecode,
		(char) (fwp.tasktypecode & 0xff),
		(char) ((fwp.tasktypecode >> 8) & 0xff),
		(char) ((fwp.tasktypecode >> 16) & 0xff),
		(char) ((fwp.tasktypecode >> 24) & 0xff),
		(unsigned) fwp.version);
 }
 return 0;
}

#include "gen/fwinfo.args.i"

int main(int argc, char **argv)
{
 tSMModuleId mod;
 tSMCardId card;
 int iErr;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.serialnumber) {
	err_t e;
	e = modopen(&card, &mod, arg.serialnumber, arg.module);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 iErr = fwinfo(mod);
 if (arg.serialnumber) {
	modclose(mod);
	cardclose(card);
 }
 return iErr;
}
