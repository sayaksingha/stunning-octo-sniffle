#ifndef INCLUDED_AVF_H
#define INCLUDED_AVF_H
#endif//INCLUDED_AVF_H

// Define bool, true and false
#ifndef true
typedef unsigned char bool;
#define true (1)
#define false (0)
#endif

typedef struct AVF
{
	// private
	char magic[4];
	unsigned int length;
	unsigned int type;
	unsigned int subType;
	unsigned int ticksPerSecond;
	unsigned int timestamp;
	unsigned int payloadDataLength;
	unsigned int immediateDataLength;
}
AVF;

unsigned int AVF_isValid(AVF const * avf);
unsigned int AVF_getAVFLength(AVF const * avf);
unsigned int AVF_getPayloadType(AVF const * avf);
unsigned int AVF_getPayloadSubtype(AVF const * avf);
unsigned int AVF_getTicksPerSecond(AVF const * avf);
unsigned int AVF_getTimestamp(AVF const * avf);
unsigned int AVF_getPayloadLength(AVF const * avf);
unsigned char const * AVF_getPayload(AVF const * avf);


typedef struct AVFBuilder
{
	// private
	unsigned int reserved1; // NOT YET WRITTEN
}
AVFBuilder;

typedef struct AVFPacket
{
	// private
	AVF const * avf;
	unsigned char const * p;

	// public
	unsigned int relativeTime;
	unsigned int timestampOffset;
	unsigned char rtpFlags1;
	unsigned char rtpFlags2;
	unsigned short sequenceSeed;
}
AVFPacket;

void AVFP_getFirst(AVFPacket * avfp, AVF const * avf);
bool AVFP_notDone(AVFPacket * avfp);
void AVFP_next(AVFPacket * avfp);

typedef struct AVFFragment
{
	// private
	AVF const * avf;
	unsigned char const * p;

	// public
	unsigned char * data;
	unsigned int length;
	bool isFromPayload;
	bool isCorrupt;
}
AVFFragment;

void AVFF_getFirst(AVFFragment * avff, AVFPacket const * avfp);
bool AVFF_notDone(AVFFragment * avff);
void AVFF_next(AVFFragment * avff);

