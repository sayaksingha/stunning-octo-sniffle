#ifdef TiNGTYPE_LINUX
#include <unistd.h>
#endif
#ifdef TiNGTYPE_QNX
#include <unistd.h>
#endif
#ifdef TiNGTYPE_WINNT
#define sleep(x) Sleep((x)*1000)
#endif

#include "eventset.hpp"
#include "prosody.hpp"
#include "recordfile.hpp"
#include "replayfile.hpp"

static void checkerr(Prosody::Error e)
{
 if (e) {
 	fprintf(stderr, "Prosody Error %d found\n", int(e));
	exit(1);
 }
}

void start_record(Prosody::Channel &c, Prosody::Event &ev)
{
 Prosody::Error e = c.alloc_placed()
	.type(Prosody::kSMChannelTypeInput)
	.module(0);
 checkerr(e);
 e = c.switch_input().st(48).ts(0).type(Prosody::kSMTimeslotTypeALaw);
 checkerr(e);
 e = ev.create(Prosody::kSMEventTypeReadData, c);
 checkerr(e);
 e = c.record_start()
	.type(Prosody::kSMDataFormatALawPCM)
	.sampling_rate(8000);
 checkerr(e);
}

void start_replay(Prosody::Channel &c, Prosody::Event &ev)
{
 Prosody::Error e = c.alloc_placed()
	.type(Prosody::kSMChannelTypeOutput)
	.module(0);
 checkerr(e);
 e = c.switch_output().st(48).ts(0).type(Prosody::kSMTimeslotTypeALaw);
 checkerr(e);
 e = ev.create(Prosody::kSMEventTypeWriteData, c);
 checkerr(e);
 e = c.replay_start()
	.type(Prosody::kSMDataFormatALawPCM)
	.sampling_rate(8000);
 checkerr(e);
}

void *waiter_thread(void *p)
{
 EventSet *es = (EventSet *) p;
 error e = es->wait();
 if (e) {
 	e.print(stderr);
	return (void *) 1;
 }
 return 0;
}

int main(int argc, char **argv)
{
#define NCHAN 64
 (void) argc;
 if (!argv[1] || !argv[2] || argv[3]) {
	fprintf(stderr, "Usage: demofile infilname outfilename\n");
	return 1;
 }
 char *infile = argv[1];
 char *outfile = argv[2];
 //Prosody::TiNGtrace = 3;
 		// prepare the event set and its waiter thread
 EventSet es;
 pthread_t tid;
 int err = pthread_create(&tid, 0, waiter_thread, &es);
 if (err) {
 	fprintf(stderr, "pthread_create() failed: %d\n", err);
	return 1;
 }
 		// now do something with channels
		// let's have some replays
 Prosody::Channel playchan[NCHAN];
 Prosody::Event playevent[NCHAN];
 FILE *inf[NCHAN];
 ReplayFile *repf[NCHAN];
 for (int i=0; i < NCHAN; i++) {
	inf[i] = fopen(infile, "rb");
	if (!inf[i]) {
		perror("fopen() failed");
		fprintf(stderr, "Cannot open file: %s\n", infile);
		return 1;
	}
	start_replay(playchan[i], playevent[i]);
	repf[i] = new ReplayFile(playchan[i], playevent[i], inf[i]);
	error e = es.insert(repf[i]);
	if (e) {
		e.print(stderr);
		return 1;
	}
 }
 		// and some records
 Prosody::Channel recchan[NCHAN];
 Prosody::Event recevent[NCHAN];
 FILE *outf[NCHAN];
 RecordFile *recf[NCHAN];
 for (int j=0; j < NCHAN; j++) {
	char oname[80];
	sprintf(oname, "%s.%d", outfile, j);
	outf[j] = fopen(oname, "wb");
	if (!inf[j]) {
		perror("fopen() failed");
		fprintf(stderr, "Cannot create file: %s\n", oname);
		return 1;
	}
	start_record(recchan[j], recevent[j]);
	recf[j] = new RecordFile(recchan[j], recevent[j], outf[j]);
	error e = es.insert(recf[j]);
	if (e) {
		e.print(stderr);
		return 1;
	}
 }
 		// here we have started lots of operations on channels
		// the recordings would go on forever, so kill them off
		// after suitable delays
 for (int k=0; k < NCHAN; k++) {
	sleep(1);
	Prosody::Error e = recchan[k].record_abort();
	checkerr(e);
 }
		// make eventset waiter stop after the operations have
		// finished
 error e = es.stop();
 if (e) {
	e.print(stderr);
	return 1;
 }
 		// now clean up various resources we allocated
 void *sts;
 err = pthread_join(tid, &sts);
 if (err) {
 	fprintf(stderr, "pthread_join() failed: %d\n", err);
	return 1;
 }
 for (int n=0; n < NCHAN; n++) {
 	delete repf[n];
 	delete recf[n];
	fclose(inf[n]);
	fclose(outf[n]);
 }
 return (int) sts;
}
