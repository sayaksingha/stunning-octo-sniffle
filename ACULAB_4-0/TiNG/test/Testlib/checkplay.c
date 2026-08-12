/* checkplay.c - handle a replay channel becoming ready */

#include "checkplay.h"
#include "bfile.h"
#include "smbesp.h"
#include <stdio.h>
#include <string.h>

int checkplay(REPLAY *cp)
{
	/* Note: for Prosody version 2 (TiNG) this does not need to loop
	 * because the channel stays ready until it has been completely
	 * serviced (i.e. if it is ready for data and you write an amount
	 * which is smaller than it can handle, it'll leave the channel
	 * event set, so the wait will instantly wake and call us again).
	 * However, version 1 only sets the event once, so if we left it
	 * with insufficient data we would never get another event.
	 */
 for (;;) {	// keep checking until the channel is no longer interesting
	struct sm_replay_status_parms rp;
	int err;
	memset(&rp, 0, sizeof(rp));
	rp.channel = cp->chan;
	err = sm_replay_status(&rp);
	if (err) {
		fprintf(stderr, "%s: sm_replay_status returned %d\n",
			cp->dbgname, err);
		return 1;
	}
	switch (rp.status) {
		struct sm_ts_data_parms dp;
		char buff[2048];
		size_t nc;
	case kSMReplayStatusComplete:
			// finished (presumably this replay was limited by
			// a data_length value in sm_replay_start or it has
			// been aborted because the call cleared)
		cp->pstate = PLAY_DONE;
		return 0;
	case kSMReplayStatusCompleteData:
			// almost finished - just flushing data
		return 0;
	case kSMReplayStatusHasCapacity:
		err = bfread(cp->bf, &nc, buff, sizeof(buff));
		if (err) {
			fprintf(stderr, "%s: bfread failed: %d: %s\n",
				cp->dbgname, err, strerror(err));
			return 1;
		}
		if (!nc) {
			cp->pstate = PLAY_EOF;
			return 0;
		}
		memset(&dp, 0, sizeof(dp));
		dp.channel = cp->chan;
		dp.data = buff;
		dp.length = nc;
		err = sm_put_replay_data(&dp);
		if (err) {
			fprintf(stderr, "%s: sm_put_replay_data returned %d\n",
				cp->dbgname, err);
			return 1;
		}
		cp->bytes += nc;
			// go round again - maybe there's more space
		break;
	case kSMReplayStatusUnderrun:
		printf("%s: underrun\n", cp->dbgname);
			// and go round again - probably space available
		break;
	case kSMReplayStatusNoCapacity:
			// it's full - nothing more to do
		return 0;
	}
 }
}
