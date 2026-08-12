#include "eventset.hpp"
#include "prosody.hpp"

void checkerr(Prosody::Error e)
{
 if (e) {
 	fprintf(stderr, "Prosody Error %d found\n", int(e));
	exit(1);
 }
}

static void assoc(Prosody::tSMChannelId chan, int typ, int iss, Prosody::tSMEventId ev)
{
 Prosody::SM_CHANNEL_SET_EVENT_PARMS ep;
 memset(&ep, 0, sizeof(ep));
 ep.channel = chan;
 ep.event_type = typ;
 ep.issue_events = iss;
 ep.event = ev;
 int rc = sm_channel_set_event(&ep);
 if (rc) {
 	fprintf(stderr, "sm_channel_set_event() returned %d\n", rc);
	exit(1);
 }
}

int main(int argc, char **argv)
{
 (void) argc;
 (void) argv;
 Prosody::TiNGtrace = 3;	// turn on TiNGtrace to show what happens

 Prosody::Event ev;	// create an 'any=channel' event
 Prosody::Error e = ev.create(Prosody::kSMEventTypeWriteData);
 assoc(Prosody::kSMNullChannelId, Prosody::kSMEventTypeWriteData, Prosody::kSMAnyChannelEvent, ev);

 printf("@%d\n", __LINE__);
 Prosody::Channel c;	// create a channel
 printf("@%d\n", __LINE__);
 e = c.alloc_placed()
	.type(Prosody::kSMChannelTypeOutput)
	.module(0);
 printf("@%d\n", __LINE__);
 checkerr(e);
 printf("@%d\n", __LINE__);

 	// associate the event with the channel
 assoc(c, Prosody::kSMEventTypeWriteData, Prosody::kSMAnyChannelEvent, ev);

 	// start shutting down - disassociate event and channel
 assoc(c, Prosody::kSMEventTypeWriteData, Prosody::kSMChannelNoEvent, ev);

 assoc(Prosody::kSMNullChannelId, Prosody::kSMEventTypeWriteData, Prosody::kSMChannelNoEvent, ev);

 e = ev.free();
 checkerr(e);

 e = c.free();
 checkerr(e);

 return 0;
}
