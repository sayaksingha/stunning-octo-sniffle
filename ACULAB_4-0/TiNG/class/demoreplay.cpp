#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif
#ifdef TiNGTYPE_QNX
#include <unistd.h>
#endif
#ifdef TiNGTYPE_WINNT
#define sleep(x) Sleep((x)*1000)
#endif

#include "prosody.hpp"

static void checkerr(Prosody::Error e)
{
 if (e) {
 	fprintf(stderr, "Prosody Error %d found\n", int(e));
	exit(1);
 }
}

int main(int argc, char **argv)
{
 (void) argc;
 if (!argv[1] || argv[2]) {
	fprintf(stderr, "Usage: demoreplay filename\n");
	return 1;
 }
 char *infile = argv[1];
 //Prosody::TiNGtrace = 3;
 	// allocate a channel
 Prosody::Channel playchan;
 Prosody::Error e = playchan.alloc_placed()
	.type(Prosody::kSMChannelTypeOutput)
	.module(0);
 checkerr(e);
 	// switch it somewhere
 e = playchan.switch_output().st(48).ts(0).type(Prosody::kSMTimeslotTypeALaw);
 checkerr(e);
 	// create a suitable event
 Prosody::Event ev;
 e = ev.create(Prosody::kSMEventTypeWriteData, playchan);
 checkerr(e);
 	// start playing
 e = playchan.replay_start()
	.type(Prosody::kSMDataFormatALawPCM)
	.sampling_rate(8000);
 checkerr(e);
 FILE *inf = fopen(infile, "rb");
 if (!inf) {
	perror("fopen() failed");
	fprintf(stderr, "Cannot open file: %s\n", infile);
	return 1;
 }
 for (;;) {
	e = ev.wait();
	checkerr(e);
	enum Prosody::kSMReplayStatus sts;
	e = playchan.replay_status().status(&sts);
	checkerr(e);
	switch (sts) {
	case Prosody::kSMReplayStatusComplete: goto done;
	case Prosody::kSMReplayStatusCompleteData: break;	// ignore it
	case Prosody::kSMReplayStatusNoCapacity: break;	// ignore it
	case Prosody::kSMReplayStatusUnderrun:
		printf("Underrun\n");
		break;	// tell user, but otherwise ignore it
	case Prosody::kSMReplayStatusHasCapacity:
		char buff[2048];
		int nc = fread(buff, 1, sizeof(buff), inf);
		if (nc < 0) {
			perror("fread() failed");
			fprintf(stderr, "Cannot read replay file\n");
			return 1;
		}
		if (!nc) {
			e = playchan.put_last_replay_data().data(buff).length(nc);
		} else {
			e = playchan.put_replay_data().data(buff).length(nc);
		}
		checkerr(e);
		break;
	}
 }
 done:
 playchan.release();
 checkerr(e);
 return 0;
}
