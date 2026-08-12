/* card_info.c - show result of sm_get_card_info */

#include <stdio.h>
#include <string.h>
#include "../Testlib/errcode_sm.h"
#include "../Testlib/cardopen.h"
#include "smdrvr.h"

static int card_info(tSMCardId card, int modonly)
{
 SM_CARD_INFO_PARMS cip;
 int r;
 //TiNGtrace=3;
 memset(&cip, 0, sizeof(cip));
 cip.card = card;
 r = sm_get_card_info(&cip);
 if (r) return printerr_sm("sm_get_card_info", r);
 if (modonly) {
	printf("%s\t%d\n", cip.serial_no, cip.module_count);
 } else {
	switch (cip.card_type) {
	case kSMCarrierCardTypeYetUnknown: printf("%10s", "Unknown"); break;
	case kSMCarrierCardTypeISAS2: printf("%10s", "ISAS2"); break;
	case kSMCarrierCardTypeBR4: printf("%10s", "BR4"); break;
	case kSMCarrierCardTypeBR8 : printf("%10s", "BR8 "); break;
	case kSMCarrierCardTypePCIP1: printf("%10s", "PCIP1"); break;
	case kSMCarrierCardTypePCIC1: printf("%10s", "cPCIC1"); break;
	case kSMCarrierCardTypePS: printf("%10s", "ProsodyS"); break;
	case kSMCarrierCardTypePX: printf("%10s", "ProsodyX"); break;
	default: printf("?%-9d", cip.card_type);
	}
	printf("  det=%d  mod=%-2d  io=0x%04lx  mem=0x%08lx  irq=%-2ld  '%s'\n",
		cip.card_detected,
		cip.module_count,
		cip.io_address,
		cip.physical_address,
		(long) cip.physical_irq,
		cip.serial_no);
 }
 return 0;
}

#include "gen/card_info.args.i"

int main(int argc, char **argv)
{
 int iErr;
 tSMCardId card;
 ARGS_DECL
 (void) argc;
 if (ARGS_CALL || *argv) {
	fprintf(stderr, "Usage: %s" ARGS_USAGE "\n", progname);
	return 1;
 }
 if (arg.serialnumber) {
	err_t e = cardopen(&card, arg.serialnumber);
	if (e) {
		error_log(stderr, e);
		return 1;
	}
 } else {
	fprintf(stderr, "%s: no Prosody card specified\n", progname);
	return 1;
 }
 iErr = card_info(card, arg.modonly);
 return iErr;
}
