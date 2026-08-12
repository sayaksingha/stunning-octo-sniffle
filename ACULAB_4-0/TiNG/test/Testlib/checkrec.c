/* checkrec.c - handle a record channel becoming ready */

#include "checkrec.h"
#include "bfile.h"
#include "smbesp.h"
#include <stdio.h>
#include <string.h>

int checkrec(RECORDING *cp)
{
	/* Note: for Prosody version 2 (TiNG) this does not need to loop
	 * because the channel stays ready until it has been completely
	 * serviced (i.e. if it has data and you read an amount which
	 * is smaller than is ready, it'll leave the channel event
	 * set, so the wait will instantly wake and call us again).
	 * However, version 1 only sets the event once, so if we left it
	 * with data we would never get another event.
	 */
 for (;;) {	// keep checking until the channel is no longer interesting
	struct sm_record_status_parms rp;
	int err;
	memset(&rp, 0, sizeof(rp));
	rp.channel = cp->chan;
	err = sm_record_status(&rp);
	if (err) {
		fprintf(stderr, "%s: sm_record_status returned %d\n",
			cp->dbgname, err);
		return 1;
	}
	switch (rp.status) {
		struct sm_ts_data_parms dp;
		char buff[2048];
		size_t nc;
	case kSMRecordStatusComplete:
			// finished (presumably this recording was limited by
			// data length, terminate on silence, etc or it has
			// been aborted because the call cleared)
		cp->done = 1;
		return 0;
	case kSMRecordStatusCompleteData:
			// almost finished - just fetching last data
	case kSMRecordStatusData:
		memset(&dp, 0, sizeof(dp));
		dp.channel = cp->chan;
		dp.data = buff;
		err = sm_get_recorded_data(&dp);
		if (err) {
			fprintf(stderr, "%s: sm_get_record_data returned %d\n",
				cp->dbgname, err);
			return 1;
		}
		cp->bytes += dp.length;
		err = bfwrite(cp->bf, &nc, buff, dp.length);
		if (err) {
			fprintf(stderr, "%s: bfwrite failed: %d: %s\n",
				cp->dbgname, err, strerror(err));
			return 1;
		}
			// go round again - maybe there's more data
		break;
	case kSMRecordStatusOverrun:
		printf("%s: overrun\n", cp->dbgname);
			// and go round again - probably data available
		break;
	case kSMRecordStatusNoData:
			// it's empty - nothing more to do
		return 0;
#ifdef PROSODY_TiNG
	case kSMRecordStatusRecognition:
			// don't care - we don't enable recognition
		break;
#endif
	}
 }
}
