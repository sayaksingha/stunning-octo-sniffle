#ifndef INCLUDED_DATAFEED_H
#define INCLUDED_DATAFEED_H

typedef struct {
	void *vtbl_data;
} DATAFEED;

enum dfconntyp {
	DFC_IN,			// ordinary input
	DFC_OUT,		// ordinary output
	DFC_BG,			// output background
	DFC_ECREF,		// echo canceller reference
	DFC_IWRSIDE,		// sidetone for IWR
	DFC_H223_H245_IN,	// H.245 to H.223 mux
	DFC_H223_AUDIO_IN,	// Audio to H.223 mux
	DFC_H223_VIDEO_IN,	// Video to H.223 mux
	DFC_H223_H245_OUT,	// H.245 from H.223 demux
	DFC_H223_AUDIO_OUT,	// Audio from H.223 demux
	DFC_H223_VIDEO_OUT,	// Video from H.223 demux
	DFC_I460_0_IN,
	DFC_I460_1_IN,
	DFC_I460_2_IN,
	DFC_I460_3_IN,
	DFC_I460_4_IN,
	DFC_I460_5_IN,
	DFC_I460_6_IN,
	DFC_I460_7_IN,
	DFC_REDIRECT_RX,
	DFC_REDIRECT_TX,
	DFC_FRAMER_RX_IN,
	DFC_FRAMER_RX_OUT,
	DFC_FRAMER_TX_IN,
	DFC_FRAMER_TX_OUT,
};

#define nonnull_datafeed(df) (!!(df).vtbl_data)
	// this is a C99 compound literal
	// only available in gcc before that, so use static const instead
//#define NULL_DATAFEED ((DATAFEED) {0})
static const DATAFEED NULL_DATAFEED;

#endif
