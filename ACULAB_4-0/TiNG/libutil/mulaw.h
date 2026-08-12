/* mulaw.c : routines for converting linear to mu-law and back.	*/

#ifndef INCLUDED_MULAW_H
#define INCLUDED_MULAW_H

typedef struct {
	short table[128];
} MULAW2LIN_TABLE;

typedef struct {
	unsigned char table[8192];
} LIN2MULAW_TABLE;


void init_mulaw_encode(LIN2MULAW_TABLE *);

void init_mulaw_decode(MULAW2LIN_TABLE *);

int mulaw_to_linear(unsigned char, MULAW2LIN_TABLE *);

unsigned char linear_to_mulaw(int, LIN2MULAW_TABLE *);

#endif
