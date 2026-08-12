#include <stdlib.h>
#include <string.h>

#include "cardopen.h"
#include "errcode_sm.h"


#ifndef NO_PROSODY_X
#include "prosody_x.h"
#endif

	// Convert a Prosody error code and an explanation into an err_t
static err_t prosody_error(int rc, char *text)
{
 const char *name = errcode_sm(rc);
 char buf[80];
 if (!name) {
 	sprintf(buf, "Prosody error %d\n", rc);
 	name = buf;
 }
 return error(error(0, name), text);
}

static tSMCardId nextcard = 0xacacacac;

err_t cardopen(tSMCardId *cardp, char *card)
{
#ifndef NO_PROSODY_S_V3
 if (*card == 's' && card[1] == '3' && card[2] == ':') {
	SM_OPEN_PROSODY_S_V3_PARMS pp;
	int r;
	memset(&pp, 0, sizeof(pp));
	pp.ps_locator = card + 3;
	pp.card_id = nextcard++;
	r = sm_open_prosody_s_v3(&pp);
	if (r) return prosody_error(r, "sm_open_prosody_s_v3() failed");
	*cardp = pp.card_id;
	return 0;
 }
#endif
#ifndef NO_PROSODY_X
 if (*card == 'x' && card[1] == ':') {
	SM_OPEN_PROSODY_X_PARMS pp;
	int r;
	memset(&pp, 0, sizeof(pp));
	pp.px_locator = card+2;
	pp.card_id = nextcard++;
	r = sm_open_prosody_x(&pp);
	if (r) return prosody_error(r, "sm_open_prosody_x() failed");
	*cardp = pp.card_id;
	return 0;
 }
#endif
 return error(0, "Unknown Prosody type");
}

err_t cardclose(tSMCardId card)
{
 SM_CLOSE_PROSODY_PARMS cp;
 int r;
 memset(&cp, 0, sizeof(cp));
 cp.card_id = card;
 r = sm_close_prosody(&cp);
 if (r) return prosody_error(r, "sm_close_prosody() failed");
 return 0;
}
