/* v8.h - declarations for the V.8 protocol */
#ifndef INCLUDED_V8
#define INCLUDED_V8

#include "../apilib/smdrvr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct v8 {
	// public fields
	enum {
			// call status codes
		V8STS_CONFIG_CI,
		V8STS_CI_OFF_AWAIT_ANSAM,
		V8STS_SENDING_CI_AWAIT_ANSAM,
		V8STS_STOPPING_CI_GOT_ANSAM,
		V8STS_CONFIG_CM,
		V8STS_SENDING_CM_AWAIT_JM,
		V8STS_GOT_JM_SENDING_CJ,
			// answer status codes
		V8STS_AWAIT_CI,
		V8STS_GOT_CI_AWAIT_CM,
		V8STS_GOT_CM_CONFIG_JM,
		V8STS_SENDING_JM_AWAIT_CJ,
		V8STS_SENDING_JM_MISSING_CJ,
		V8STS_STOPPING,
		V8STS_DONE,
	} status;
	enum {
		V8ANS_STS_NONE,
		V8ANS_STS_ANS,
		V8ANS_STS_ANS_PR,
		V8ANS_STS_ANSAM,
		V8ANS_STS_ANSAM_PR,
	} ans_status;
	int await_recognition_event;
	int await_read_event;
	int await_write_event;

	// private fields
	tSMChannelId channel;
	int sending_ansam;
	struct {
		unsigned char buf[1024];
		unsigned len;
	} in, out;
	struct {
		unsigned inbits;	// bits in cc
		unsigned cc;		// current character
	} asyrx;
	unsigned outpos;
	unsigned tottx;

	unsigned CJ_wait_count;
	unsigned int in_msglen;
	unsigned char hdlc_prev;
	unsigned char have_carrier;
	unsigned char ci[2];
	unsigned char full_ansam;
	int rxfsk_metric;
	int flags_detected;
};

int v8_start_answer(struct v8 *v8, tSMChannelId chan);
int v8_do_ansam_phase_reversals(struct v8 *v8);
int v8_start_send_ci(struct v8 *v8, tSMChannelId chan);
int v8_do_full_ansam_detection(struct v8 *v8);
int v8_on_ci(struct v8 *v8);
int v8_off_ci(struct v8 *v8);
int v8_start_send_cm(struct v8 *v8, tSMChannelId chan);
int v8_handle_read(struct v8 *v8);
int v8_handle_write(struct v8 *v8);
int v8_handle_recognition(struct v8 *v8);
int v8_abort(struct v8 *v8);
int v8_fetch_inmessage(struct v8 *v8, unsigned char **msg, unsigned *length);
int v8_set_outmessage(struct v8 *v8, const unsigned char *msg, unsigned length);

void v8_show_code(unsigned char *cp, unsigned char *endp);

int v8_build_code(unsigned char *buf);
int v8_check_code(unsigned char *out, unsigned char *in, unsigned char *inend);

extern int (*v8_showtrace)(const char *fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif
