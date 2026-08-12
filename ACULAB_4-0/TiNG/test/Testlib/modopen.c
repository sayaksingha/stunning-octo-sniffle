#include <stdlib.h>
#include <string.h>

#include "cardopen.h"
#include "modopen.h"
#include "errcode_sm.h"

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

err_t modopen(tSMCardId *card_idp, tSMModuleId *modp, char *spec, unsigned modno)
{
 SM_OPEN_MODULE_PARMS mp;
 int r;
 err_t e = cardopen(card_idp, spec);
 if (e) return e;
 memset(&mp, 0, sizeof(mp));
 mp.card_id = *card_idp;
 mp.module_ix = modno;
 r = sm_open_module(&mp);
 if (r) return prosody_error(r, "sm_open_module() failed");
 *modp = mp.module_id;
 return 0;
}

err_t modclose(tSMModuleId mod)
{
 SM_CLOSE_MODULE_PARMS mp;
 int r;
 memset(&mp, 0, sizeof(mp));
 mp.module_id = mod;
 r = sm_close_module(&mp);
 if (r) return prosody_error(r, "sm_close_module() failed");
 return 0;
}
