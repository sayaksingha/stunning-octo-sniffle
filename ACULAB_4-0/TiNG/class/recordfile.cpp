#include "recordfile.hpp"

static int checkerr(Prosody::Error e)
{
 if (e) {
 	fprintf(stderr, "Prosody Error %d during recording\n", int(e));
	return 1;
 }
 return 0;
}

int RecordFile::unsafe_handler()
{
 enum Prosody::kSMRecordStatus sts;
 enum Prosody::kSMRecognition rtype;
 Prosody::tSM_INT param0;
 Prosody::tSM_INT param1;
 enum Prosody::kSMRecordHowTerminated reason;
 Prosody::tSM_UT32 termocts;
 Prosody::Error e = chan_.record_status()
 	.status(&sts)
	.recog_type(&rtype)
	.param0(&param0)
	.param1(&param1)
	.termination_reason(&reason)
	.termination_octets(&termocts);
 if (checkerr(e)) return 1;
 switch (sts) {
 case Prosody::kSMRecordStatusComplete:
 	printf("Record termination - reason %d, octets %u\n",
		reason, (unsigned) termocts);
 	return 1;
 case Prosody::kSMRecordStatusNoData: return 0;	// ignore it
 case Prosody::kSMRecordStatusRecognition:
 	printf("record recognition: %d %d %d\n", rtype, param0, param1);
	return 0;
 case Prosody::kSMRecordStatusOverrun:
	printf("Overrun\n");
 	return 0;
 case Prosody::kSMRecordStatusCompleteData:
 case Prosody::kSMRecordStatusData:
	char buff[kSMMaxRecordDataBufferSize];
	int len;
	e = chan_.get_recorded_data().data(buff).length(&len);
	if (checkerr(e)) return 1;
	int nc = fwrite(buff, 1, len, pf_);
	if (nc < 0) {
		perror("fwrite() failed");
		fprintf(stderr, "Cannot write data to record file\n");
		return 1;
	} else if (nc < len) {
		fprintf(stderr, "Wrote only %d bytes of %d to record file\n",
			nc, len);
		return 1;
	}
	return 0;
 }
 return 0;
}

void RecordFile::cleanup()
{
 printf("record finished\n");
}

int RecordFile::handler()
{
 pthread_mutex_lock(&mx_);
 int sts = unsafe_handler();
 pthread_mutex_unlock(&mx_);
 return sts;
}

