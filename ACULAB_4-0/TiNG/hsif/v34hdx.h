/* v34.h - messages passed between host and sharc for V34 tx & rx */
#ifndef INCLUDED_V34_H
#define INCLUDED_V34_H

/* h->s: direction: 1 - originator, 0 - answerer
   primary channel dir: 0 - normal (orig->ans), 1 - reverse (ans->orig) */
#define MSG_DIR 0xcbc

/* parameters to MSG_V34_LINE_STATUS s->h */
#define V34_LINE_SILENT 0
#define V34_LINE_NEGOTIATING 1
#define V34_LINE_CONTROL_DATA 2
#define V34_LINE_PRIMARY_DATA 3

/* 1st param: tx status, 2nd param: rx status */
#define MSG_V34_LINE_STATUS 0xcb0

/* h->s: 1 - set primary channel, 0 - set control channel
   s->h: ack: 1 - primary channel set, 0 - control channel set */
#define MSG_V34_SWITCH_CHANNEL 0xcb1

/* parameter to MSG_RETRAIN h->s */
#define V34_FULL_RETRAIN 0
#define V34_RENEGOTIATE_UP 1
#define V34_RENEGOTIATE_DOWN 2

/* h->s: parameter as above */
#define MSG_RENEGOTIATE 0xcb2

/* s->h: current_snr - SNR detected
  min_snr - Minimum SNR required for good reception */
#define MSG_SNR 0xcb3

/* h->s: 1st param signal power metric recorded from FSK rx during V.8 phase (or zero if none) */
#define MSG_V8FSKMETRIC 0xcb4

#endif
