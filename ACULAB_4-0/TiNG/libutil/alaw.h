/* alaw.c : routines for converting linear to A-law and back.	*/

#ifndef INCLUDED_ALAW_H
#define INCLUDED_ALAW_H

typedef struct {
	short table[128];
} ALAW2LIN_TABLE;

typedef struct {
	unsigned char table[4096];
} LIN2ALAW_TABLE;


void init_alaw_encode(LIN2ALAW_TABLE *);

void init_alaw_decode(ALAW2LIN_TABLE *);

int alaw_to_linear(unsigned char, ALAW2LIN_TABLE *);

unsigned char linear_to_alaw(int, LIN2ALAW_TABLE *);

#endif
