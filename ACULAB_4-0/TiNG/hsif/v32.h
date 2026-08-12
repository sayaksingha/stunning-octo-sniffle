/* v32.h - messages passed between host and sharc for V32 tx & rx */
#ifndef INCLUDED_V32_H
#define INCLUDED_V32_H

#define MSG_DIRECTION 0xcbc
	/*
	 * DIRECTION(isans)
	 * sets local behaviour: 0=originate, 1=answer
	 * h->s only
	 */

#define MSG_NO_ANS_TONE 0xcb4
	/*
	 * NO_ANS_TONE(ans_tone)
	 * sets whether to send/waitfor ANS: 0=send/wait, 1=don't send/wait
	 * h->s only
	 */

#define MSG_V32_LINE_STATUS 0xcb0
	/*
	 * V32_LINE_STATUS(ev, val)
	 * reports status change from Netbricks code. Parameters
	 * are as for dp_event_notify()
	 * s->h only
	 */

#define MSG_RETRAIN 0xcb5
	/*
	 * RETRAIN(speed)
	 * initiate a retrain with speed as given
	 * i.e. 4800, 9600 or 0 (zero means either speed allowed)
	 * h->s only
	 *
	 * RETRAIN
	 * a retrain has started
	 * s->h only
	 */

#define MSG_ALLOW_AUTO_SPEED_CHANGE 0xcb6
	/*
	 * ALLOW_AUTO_SPEED_CHANGE(allow)
	 * sets whether retrains not initiated by the api are allowed to change speed
	 * allow = 0 means fix speed to established bitrate
	 * allow = 1 means use speed from initial MSG_SPEED or a MSG_RETRAIN to determine allowed bitrates
	 * h->s only
	 */

#endif
