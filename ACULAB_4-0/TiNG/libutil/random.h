/* random.c - random number generation */

#ifndef INCLUDED_RANDOM_H
#define INCLUDED_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

#define RANDMT_N 624
typedef struct {
	unsigned long mt[RANDMT_N]; /* the array for the state vector  */
	int mti; /* mti==RANDMT_N+1 means mt[RANDMT_N] is not initialized */
} RANDMT;

/* initailise generator from seed */
void randmt(RANDMT *rp, unsigned long s);

/* initialise generator from array of values */
void randmt_by_array(RANDMT *rp, unsigned long init_key[], int key_length);

/* generates a random number on [0,0xffffffff]-interval */
unsigned long randmt_int32(RANDMT *rp);

/* generates a random number on [0,0x7fffffff]-interval */
long randmt_int31(RANDMT *rp);

#ifndef TiNGTYPE_STARCORE
/* generates a random number on [0,1]-real-interval */
double randmt_real1(RANDMT *rp);

/* generates a random number on [0,1)-real-interval */
double randmt_real2(RANDMT *rp);

/* generates a random number on (0,1)-real-interval */
double randmt_real3(RANDMT *rp);

/* generates a random number on [0,1) with 53-bit resolution */
double randmt_res53(RANDMT *rp);
#endif

#ifdef __cplusplus
}
#endif
#endif
