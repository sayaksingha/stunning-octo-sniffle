/* msgstring.h - unique ID portion for string messages */

#ifndef INCLUDED_MSGSTRING_H
#define INCLUDED_MSGSTRING_H

#define MSGSTRING_ID(uniqueid, stringno) (((uniqueid)<<16)+((stringno)&0xffff))

#define MSGSTRING_ID2UID(stringid) (((stringid)>>16)&0xffff)
#define MSGSTRING_ID2STRNO(stringid) ((stringid)&0xffff)

	// this happens to be the RFC which defines RTCP
#define MSGSTRING_RTCP 3550

#define MSGSTRING_ICME 3560
#define MSGSTRING_ICMD 3570
#define MSGSTRING_CBCE 3580
#define MSGSTRING_CBCD 3590
#define MSGSTRING_HMAE 3600
#define MSGSTRING_HMAD 3610

#define MSGSTRING_H223_TBL_ENTRY 3700

#define MSGSTRING_VMPRX_CODEC 0x1001
	// VMPrx codecs use this to send status info to the api

#define MSGSTRING_CODEC_SSE 0x1002
	// used to send SSE via VMPtx subtask

#define MSGSTRING_STUN 0x1003
	// used to send stun credential config messages

#define MSGSTRING_VMPRX_PROFILE 0x1004
	// VMPrx profile specific plugins use this to receive parameter and send status info to the api

#define MSGSTRING_VMPTX_PROFILE 0x1005
	// VMPtx profile specific plugins use this to receive parameters (id = 0 for initial, 1 for updates)

#define MSGSTRING_SPRT 0x2001
	// used to send or recv sprt moip messages

#define MSGSTRING_CATSIG_PLUGIN 0x3001
	// used to send psaiamd classify params (business etc)

#endif
