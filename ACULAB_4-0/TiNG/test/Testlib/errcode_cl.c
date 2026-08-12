#include <stdio.h>

#define arlen(s) (sizeof(s)/sizeof(*(s)))

#include "acu_type.h"
#include "cl_lib.h"

#include "errcode_cl.h"

static struct {
	int code;
	const char *name;
} errs[] = {
	/* -3 */ { ERR_COMMAND, "illegal command", },
	/* -4 */ { ERR_NET, "illegal network number", },
	/* -5 */ { ERR_PARM, "illegal parameter", },
	/* -6 */ { ERR_RESPONSE, "ERR_RESPONSE", },
	/* -7 */ { ERR_NOCALLIP, "no call in progress", },
	/* -8 */ { ERR_TSBAR, "timeslot barred", },
	/* -9 */ { ERR_TSBUSY, "timeslot busy", },
	/* -10 */ { ERR_CFAIL, "command failed to execute", },
	/* -11 */ { ERR_SERVICE, "invalid service code", },
	/* -12 */ { ERR_BUFF_FAIL, "out of buffer resources", },
	/* -13 */ { ERR_DNLD_ZAP, "debug monitor running", },
	/* -14 */ { ERR_DNLD_NOCMD, "firmware not loaded - command denied", },
	/* -15 */ { ERR_DNLD_NODNLD, "firmware installed - download denied", },
	/* -16 */ { ERR_DNLD_GEN, "general failure during download", },
	/* -17 */ { ERR_DNLD_NOSIG, "downloaded firmware failed to start", },
	/* -18 */ { ERR_DNLD_NOEXEC, "downloaded firmware is not executing", },
	/* -19 */ { ERR_DNLD_NOCARD, "downloaded firmware is not executing", },
	/* -20 */ { ERR_DNLD_SYSTAT, "downloaded firmware detected an error", },
	/* -21 */ { ERR_DNLD_BADTLS, "driver does not support the firmware", },
	/* -22 */ { ERR_DNLD_POST, "board failed power on self test", },
	/* -23 */ { ERR_DNLD_SW, "switch setup error", },
	/* -24 */ { ERR_DNLD_MEM, "could not allocate memory for download", },
	/* -25 */ { ERR_DNLD_FILE, "could not find the file to download", },
	/* -26 */ { ERR_DNLD_TYPE, "the file is not downloadable", },
};

const char *errcode_cl(int err)
{
 unsigned int i;
 for (i=0; i<arlen(errs); i++)
	if (err == errs[i].code) return errs[i].name;
 return 0;
}

int printerr_cl(const char *name, int r)
{
 const char *err = errcode_cl(r);
 if (!err) err = "Unknown error";
 fprintf(stderr, "%s() returns %d (%s)\n", name, r, err);
 return r;
}
