#include "replayfile.hpp"

static int checkerr(Prosody::Error e)
{
 if (e) {
 	fprintf(stderr, "Prosody Error %d during recording\n", int(e));
	return 1;
 }
 return 0;
}

int ReplayFile::unsafe_handler()
{
 enum Prosody::kSMReplayStatus sts;
 Prosody::Error e = chan_.replay_status().status(&sts);
 if (checkerr(e)) return 1;
 switch (sts) {
 case Prosody::kSMReplayStatusComplete: return 1;
 case Prosody::kSMReplayStatusCompleteData: return 0;	// ignore it
 case Prosody::kSMReplayStatusNoCapacity: return 0;	// ignore it
 case Prosody::kSMReplayStatusUnderrun:
	printf("Underrun\n");
 	return 0;	// ignore it
 case Prosody::kSMReplayStatusHasCapacity:
	char buff[2048];
	int nc = fread(buff, 1, sizeof(buff), pf_);
	if (nc < 0) {
		perror("fread() failed");
		fprintf(stderr, "Cannot read replay file\n");
		return 1;
	}
	if (!nc) {
		e = chan_.put_last_replay_data().data(buff).length(nc);
	} else {
		e = chan_.put_replay_data().data(buff).length(nc);
	}
	if (checkerr(e)) return 1;
	return 0;
 }
 return 0;
}

int ReplayFile::handler()
{
 pthread_mutex_lock(&mx_);
 int sts = unsafe_handler();
 pthread_mutex_unlock(&mx_);
 return sts;
}

void ReplayFile::cleanup()
{
 printf("replay finished\n");
}
