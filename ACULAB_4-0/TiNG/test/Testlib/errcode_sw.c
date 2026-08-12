#include <stdio.h>

#define arlen(s) (sizeof(s)/sizeof(*(s)))
#include "acu_type.h"
#include "sw_lib.h"

#include "errcode_sw.h"

static struct {
	int code;
	const char *name;
} errs[] = {
	/* -200 */ { ERR_SW_INVALID_COMMAND, "Command code is not supported", },
	/* -202 */ { ERR_SW_DEVICE_ERROR, "An error was returned from a device", },
	/* -204 */ { ERR_SW_NO_RESOURCES, "An internal driver resource has", },
	/* -209 */ { ERR_SW_INVALID_SWITCH, "Out of range switch driver index", },
	/* -210 */ { ERR_SW_INVALID_STREAM, "Stream number in parameter list is", },
	/* -211 */ { ERR_SW_INVALID_TIMESLOT, "Time slot in parameter list is out", },
	/* -213 */ { ERR_SW_INVALID_CLOCK_PARM, "Invalid clock configuration", },
	/* -216 */ { ERR_SW_INVALID_MODE, "Incorrect SET_OUTPUT or", },
	/* -217 */ { ERR_SW_INVALID_MINOR_SWITCH, "Minor (internal) switch error.", },
	/* -218 */ { ERR_SW_INVALID_PARAMETER, "General invalid parameter error.", },
	/* -220 */ { ERR_SW_NO_PATH, "Connection cannot be made due to switch limitation", },
	/* -224 */ { ERR_SW_NO_SCBUS_CLOCK, "No card driving SCbus clock.", },
	/* -225 */ { ERR_SW_OTHER_SCBUS_CLOCK, "Another non-MVSWDEV controlled card is driving scbus clock", },
	/* -226 */ { ERR_SW_PATH_BLOCKED, "eg. because of MVIP output MUX", },
	/* -200 */ { ERR_SW_INVALID_COMMAND, "Command code is not supported", },
	/* -202 */ { ERR_SW_DEVICE_ERROR, "An error was returned from a device", },
	/* -204 */ { ERR_SW_NO_RESOURCES, "An internal driver resource has", },
	/* -210 */ { ERR_SW_INVALID_STREAM, "Stream number in parameter list is", },
	/* -211 */ { ERR_SW_INVALID_TIMESLOT, "Time slot in parameter list is out", },
	/* -213 */ { ERR_SW_INVALID_CLOCK_PARM, "Invalid clock configuration", },
	/* -216 */ { ERR_SW_INVALID_MODE, "Incorrect SET_OUTPUT or", },
	/* -217 */ { ERR_SW_INVALID_MINOR_SWITCH, "Minor (internal) switch error.", },
	/* -218 */ { ERR_SW_INVALID_PARAMETER, "General invalid parameter error.", },
	/* -220 */ { ERR_SW_NO_PATH, "Connection cannot be made due to switch limitation", },
	/* -224 */ { ERR_SW_NO_SCBUS_CLOCK, "No card driving SCbus clock.", },
	/* -225 */ { ERR_SW_OTHER_SCBUS_CLOCK, "Another non-MVSWDEV controlled card is driving scbus clock", },
	/* -226 */ { ERR_SW_PATH_BLOCKED, "eg. because of MVIP output MUX", },
};

const char *errcode_sw(int err)
{
 unsigned int i;
 for (i=0; i<arlen(errs); i++)
	if (err == errs[i].code) return errs[i].name;
 return 0;
}

int printerr_sw(const char *name, int r)
{
 const char *err = errcode_sw(r);
 if (!err) err = "Unknown error";
 fprintf(stderr, "%s() returns %d (%s)\n", name, r, err);
 return r;
}
