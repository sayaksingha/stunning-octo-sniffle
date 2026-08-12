#include "avf.h"
#pragma warning(disable:4505) // unreferenced local function has been removed

static unsigned short read16(void const * p)
{
	unsigned char const * q = p;
	return (unsigned short)((q[0] << 8) + (q[1]));
}

static unsigned int read32(void const * p)
{
	unsigned char const * q = p;
	return (q[0] << 24) + (q[1] << 16) + (q[2] << 8) + (q[3]);
}

static unsigned int AVF_getPayloadDataLength(AVF const * avf)
{
	return read32(&avf->payloadDataLength);
}

static unsigned int AVF_getImmediateDataLength(AVF const * avf)
{
	return read32(&avf->immediateDataLength);
}

static unsigned char const * AVF_getImmediate(AVF const * avf)
{
	return AVF_getPayload(avf) + AVF_getPayloadLength(avf);
}

static unsigned char const * AVF_getConstructorList(AVF const * avf)
{
	return AVF_getImmediate(avf) + AVF_getImmediateDataLength(avf);
}

unsigned int AVF_getAVFLength(AVF const * avf)
{
	return read32(&avf->length);
}

unsigned int AVF_getPayloadLength(AVF const * avf)
{
	return read32(&avf->payloadDataLength);
}

unsigned int AVF_getPayloadSubtype(AVF const * avf)
{
	return read32(&avf->subType);
}

unsigned int AVF_getPayloadType(AVF const * avf)
{
	return read32(&avf->type);
}

unsigned int AVF_getTicksPerSecond(AVF const * avf)
{
	return read32(&avf->ticksPerSecond);
}

unsigned int AVF_getTimestamp(AVF const * avf)
{
	return read32(&avf->timestamp);
}

unsigned int AVF_isValid(AVF const * avf)
{
	return (avf->magic[0] == 'a') && (avf->magic[1] == 'v') && (avf->magic[2] == 'f') && (avf->magic[3] == ' ');
}

unsigned char const * AVF_getPayload(AVF const * avf)
{
	return (unsigned char const *)(&avf[1]);
}

static void AVFP_fillInDetails(AVFPacket * avfp)
{
	unsigned char const * p = avfp->p;
	avfp->relativeTime = read32(p+1);
	avfp->timestampOffset = read32(p+5);
	avfp->rtpFlags1 = p[9];
	avfp->rtpFlags2 = p[10];
	avfp->sequenceSeed = read16(p+11);
}

void AVFP_getFirst(AVFPacket * avfp, AVF const * avf)
{
	avfp->avf = avf;
	avfp->p = AVF_getConstructorList(avf);
	AVFP_fillInDetails(avfp);
}

bool AVFP_notDone(AVFPacket * avfp)
{
	return (bool)(*(avfp->p) == 'P');
}

void AVFP_next(AVFPacket * avfp)
{
	avfp->p += 13;
	while (*avfp->p < 4) avfp->p += 7;
	if (AVFP_notDone(avfp))
	{
		AVFP_fillInDetails(avfp);
	}
}

static void AVFF_fillInDetails(AVFFragment * avff)
{
	unsigned char const * p = avff->p;
	int type = *p;
	int offset = read32(p+1);
	avff->length = read16(p+5);
	avff->isCorrupt = false;
	if (type == 1)
	{
		avff->data = (unsigned char *)AVF_getImmediate(avff->avf) + offset;
		avff->isFromPayload = false;
	}
	else
	{
		avff->data = (unsigned char *)AVF_getPayload(avff->avf) + offset;
		avff->isFromPayload = true;
	}
}

void AVFF_getFirst(AVFFragment * avff, AVFPacket const * avfp)
{
	avff->avf = avfp->avf;
	avff->p = avfp->p + 13;
	AVFF_fillInDetails(avff);
}

bool AVFF_notDone(AVFFragment * avff)
{
	return (bool)(*(avff->p) < 4);
}

void AVFF_next(AVFFragment * avff)
{
	avff->p += 7;
	if (AVFF_notDone(avff))
	{
		AVFF_fillInDetails(avff);
	}
}


