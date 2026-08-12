/* kernel_x.h - definition specific to the Prosody X kernel */

#ifndef DEFINED_TASK_SIZES
#define DEFINED_TASK_SIZES

#define TASK_ID_LOW_BITS 10
#define TASK_ID_EXTRA_BITS 16

#define TASK_ID_END_S (TASK_ID_POS+TASK_ID_LOW_BITS)
#define TASK_ID_END_L (TASK_ID_POS+TASK_ID_LOW_BITS+TASK_ID_EXTRA_BITS)

#define TASK_GEN_POS TASK_ID_END_S
#define TASK_GEN_BITS (16-TASK_ID_LOW_BITS)

#define TASK_ID_HIGH_POS (TASK_ID_LOW_BITS)
#define TASK_ID_HIGH_MSG_POS (TASK_ID_LOW_BITS+TASK_GEN_BITS)

#define TASKID2EXTRA(id) (id >> TASK_ID_LOW_BITS)
#define TASKGENID_L2EXTRA(id) (id >>TASK_ID_HIGH_MSG_POS)
#define TASKGENID_L2TASKID_S(id) FEXT(id, TASK_ID_POS, TASK_ID_LOW_BITS)
#define TASKGENID_L2TASKID_L(id) ((TASKGENID_L2EXTRA(id)<<TASK_ID_LOW_BITS) | MSG2TASK_ID_S(id))
#define TASKGENID_L2TASKGENID_S(id) FEXT(id, TASK_ID_POS, TASK_ID_LOW_BITS + TASK_GEN_BITS)

#define MSG2TASK_ID_S(m) FEXT(m, TASK_ID_POS, TASK_ID_LOW_BITS)
#define MSG2TASK_ID_L(x,m) (((x << TASK_ID_END_S) | MSG2TASK_ID_S(m)))
#define MSG2TASK_GEN(m) FEXT(m, TASK_GEN_POS, TASK_GEN_BITS)
#define MSG2TASK_GENID_S(m) ((MSG2TASK_GEN(m) << TASK_GEN_POS) | MSG2TASK_ID_S(m))
#define MSG2TASK_GENID_L(x,m) (((x << TASK_ID_HIGH_MSG_POS) | (MSG2TASK_GEN(m) << TASK_GEN_POS) | MSG2TASK_ID_S(m)))

#define MSG_TASKID_EXTEND 0x888
	/*
	 * TASKID_EXTEND(val)
	 * indicates that the task ID field in subsequent messages
	 * should be interpreted as part of a larger task ID, with the
	 * extra bits being 'val'. In particular, a message with a task
	 * ID field of 'x' should be interpreted as referring to the task
	 * with ID:
	 *	x ? (val << 16) + x : 0
	 *
	 * 'val' must be in the range 0 .. 65535, and should be as small
	 * as possible.
	 */

#define MSG_SECKEYID 0x877
	/*
	 * SECKEYID(keyId,level)
	 * Indicates initial (or changed over) encryption for all subsequent
	 * messages will use keyId, level parameter on initial use indicates
	 * what elements of payload encrypted, if zero - all elements,
	 * if bit zero set then replay like data excluded, if bit one set
	 * then record like data excluded.
	 * Will always be last message in ASSP packet payload.
	 * h->s only
	 */

#define MSG_SECKEYCHANGE 0x879
	/*
	 * SECKEYCHANGE(keyId,k1,k2,k3,k4,k5,k6,k7,k8,k9,k10,k11)
	 * Indicates encrypt key change by module, gives ney keyId 
	 * and session key - these will be encrypted according to
	 * previous key, subsequent data from module will be
	 * encrypted according to new key. 
	 * Will always be last message in ASSP packet payload.
	 * s->h only
	 */

#define MSG_PAD 0x87A
	/*
	 * Pad packet payload out to be multiple of 64 bits
	 * NOP
	 * s->h and h->s
	 */

#define MSG_STARTED 0x811
	/*
	 * TASKID
	 * indicates created task running
	 * s->h only
	 */

#define MSG_GETLIC_INFO 0x812
	/*
	 * GETLIC_INFO(state)
	 * indicates get licence state
	 * state = 0 when failed to get licence
	 * s->h only
	 */

#endif
