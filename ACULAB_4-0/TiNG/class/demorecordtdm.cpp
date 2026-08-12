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
	fprintf(stderr, "Usage: demorecord filename\n");
	return 1;
 }
 char *outfile = argv[1];
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
 Prosody::Channel recchan;
 e = recchan.alloc_placed()
	.type(Prosody::kSMChannelTypeInput)
	.module(mod);
 checkerr(e);
 	// allocate a TDM receiver
 Prosody::Tdmrx tdmrx;
 e = tdmrx.create()
 	.module(mod).stream(48).timeslot(0).type(Prosody::kSMTimeslotTypeALaw);
 checkerr(e);
 	// get the TDM datafeed
 Prosody::tSMDatafeedId df;
 e = tdmrx.get_datafeed().datafeed(&df);
 checkerr(e);
 	// connect the TDM to the datafeed
 e = recchan.datafeed_connect().data_source(df);
 checkerr(e);
 	// create a suitable event
 Prosody::Event ev;
 e = ev.create(Prosody::kSMEventTypeReadData, recchan);
 checkerr(e);
 	// start recording
 e = recchan.record_start()
	.type(Prosody::kSMDataFormatALawPCM)
	.sampling_rate(8000)
	.max_octets(8000);
 checkerr(e);
 FILE *outf = fopen(outfile, "wb");
 if (!outf) {
	perror("fopen() failed");
	fprintf(stderr, "Cannot create file: %s\n", outfile);
	return 1;
 }
 for (;;) {
	e = ev.wait();
	checkerr(e);
	enum Prosody::kSMRecordStatus sts;
	e = recchan.record_status().status(&sts);
	checkerr(e);
	switch (sts) {
	case Prosody::kSMRecordStatusComplete: goto done;
	case Prosody::kSMRecordStatusRecognition: break;	// ignore it
	case Prosody::kSMRecordStatusCompleteData: break;	// ignore it
	case Prosody::kSMRecordStatusNoData: break;	// ignore it
	case Prosody::kSMRecordStatusOverrun:
		printf("Overrun\n");
		break;	// tell user, but otherwise ignore it
	case Prosody::kSMRecordStatusData:
		char buff[2048];
		int nc;
		e = recchan.get_recorded_data().data(buff).length(&nc);
		checkerr(e);
		int nw = fwrite(buff, 1, nc, outf);
		if (nc != nw) {
			perror("fwrite() failed");
			fprintf(stderr, "Cannot write record file\n");
			return 1;
		}
		break;
	}
 }
 done:
 // e = recchan.release();
 // checkerr(e);
 e = tdmrx.destroy();
 checkerr(e);
 return 0;
}
