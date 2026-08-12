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
 Prosody::TiNGtrace = 3;
 	// open the card
 int x;	// dummy to have an address to get a unique card ID
 Prosody::Card card((Prosody::tSMCardId) &x);
 Prosody::Error e = card.open_prosody_x()
 	.ip_address(("172.16.1.250"));
 checkerr(e);
 	// open the module
 Prosody::Module mod;
 e = mod.open_module().card_id(card).module_ix(0);
 checkerr(e);
 	// allocate a channel
 Prosody::Channel playchan;
 e = playchan.alloc_placed()
	.type(Prosody::kSMChannelTypeOutput)
	.module(mod);
 checkerr(e);
 	// allocate a TDM transmitter
 Prosody::Tdmtx tdmtx;
 e = tdmtx.create()
 	.module(mod).stream(48).timeslot(0).type(Prosody::kSMTimeslotTypeALaw);
 checkerr(e);
 	// get the channel datafeed
 Prosody::tSMDatafeedId df;
 e = playchan.get_datafeed().datafeed(&df);
 checkerr(e);
 	// connect the TDM to the datafeed
 e = tdmtx.datafeed_connect().data_source(df);
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
 // e = playchan.release();
 // checkerr(e);
 e = tdmtx.destroy();
 checkerr(e);
 return 0;
}
