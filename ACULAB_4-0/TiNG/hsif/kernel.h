/* kernel.h - kernel messages passed between host and sharc */

#ifndef INCLUDED_KERNEL_H
#define INCLUDED_KERNEL_H

#define TASK_ID_POS 0
#define TASK_GEN_END 16

#define MSGCODE(m, len) (((m) << 4) + (len))

#ifdef __SUNPRO_C
#define MESSAGE(m, l) (((unsigned) (m)<<20)+((l)<<16))
#else
#define MESSAGE(m, l) ((MSGCODE((m), (l))<<16) & 0xffffffff)
#endif

#define FEXT(v, pos, bits) (((v) >> (pos)) & ((1 << (bits)) - 1))
#define MSG2ANYCODE(m) (((m) >> 20) & 0xfff)
#define MSG2CODE(m) (((m) >> 16) & 0xffff)
#define MSG2LEN(m) (((m) >> 16) & 0xf)

#define TASK_GENID_S(gen, id) (((gen) << TASK_GEN_POS) | (id & ((1 << TASK_ID_LOW_BITS)-1)))

	//Shift the long id by the number of gen bits to move the 
	//extra bits above the generation number. Mask off the bottom
	//16 bits. OR in the gen number in the corect position and
	//finally or in the bottom 'n' bits of the task id
#define TASK_GENID_L(gen, id) ((((id) << TASK_GEN_BITS)& 0xffff0000) | ((gen) << TASK_GEN_POS) | (id & ((1 << TASK_ID_LOW_BITS)-1)))


#define MSG_RESET 0x800
	/*
	 * RESET(bufstart, bufend)
	 * indicates the bounds of the buffer for messages sent to this DSP
	 * s->h only
	 */

#define MSG_READY 0x801
	/*
	 * READY(bufstart, bufend, direction)
	 * indicates the bounds of the buffer for transfers to or from
	 * this task
	 * s->h only
	 */

	/* direction is s->h */
#define MSG_READY_DIR_RX 0
	/* direction is h->s */
#define MSG_READY_DIR_TX 1


#define MSG_CREATE 0x802
	/*
	 * CREATE(id, type)
	 * creates a task with given id to run the specified function
	 * h->s only
	 */

#define MSG_DEAD 0x803
	/*
	 * DEAD(cause [, cputimeL, cputimeH])
	 * reports the termination of a task. If the termination is normal,
	 * the accumulated CPU time is reported (low word, high word)
	 * s->h only
	 *
	 * DEAD
	 * kills task
	 * h->s only
	 */

	/* normal task termination (no outside cause) */
#define MSG_DEAD_CAUSE_NATURAL 0
	/* pseudo task termination - got message for nonexistant task */
#define MSG_DEAD_CAUSE_NOTASK 1
	/* pseudo task termination - cannot create task - task type unknown */
#define MSG_DEAD_CAUSE_BADFUNC 2
	/* pseudo task termination - cannot create task - ID in use */
#define MSG_DEAD_CAUSE_DUPID 3
	/* pseudo task termination - cannot create task - ID in use */
#define MSG_DEAD_CAUSE_BADGEN 4
	/* pseudo task termination - cannot create task - out of memory */
#define MSG_DEAD_CAUSE_NOMEM 5
	/* task termination - got DEAD message */
#define MSG_DEAD_CAUSE_MSG 6
	/* normal task termination due to failure (no outside cause) */
#define MSG_DEAD_CAUSE_FAILED 7
	/* task termination - internal error - task not initialised */
#define MSG_DEAD_CAUSE_NOINIT 8
	/* task termination - got bad parameter */
#define MSG_DEAD_CAUSE_PARAM 9
	/* task termination - no more data to send */
#define MSG_DEAD_CAUSE_NOMORE 10
	/* pseudo task termination - lost connection to DSP */
#define MSG_DEAD_CAUSE_DISCO 11
	/* task termination - licence problem */
#define MSG_DEAD_CAUSE_LICENCE 12
	/* task termination - read invalid data (e.g. AVF from host buffer) */
#define MSG_DEAD_CAUSE_INVALIDDATA 13

#define MSG_SET_OUTTS 0x804
	/*
	 * SET_OUTTS(ts)
	 * sets output timeslot number for a task.
	 * h->s only
	 */

#define MSG_SET_INTSMASK 0x805
	/*
	 * SET_INTSMASK(mask)
	 * Sets input timeslot mask for a task. First word is bitmask
	 * for sport0 tx (bit N = timeslot N), second for sport1 tx,
	 * third for sport0 rx and fourth for sport1 rx.
	 * h->s only
	 */

	/* NB: the value parameter must match the enum comptype
	 * defined in driver/extbus.c
	 * Unfortunately, the assembler cannot understand enums,
	 * and gcc has very useful warnings for enums, so the two
	 * files must be co-ordinated manually.
	 */
#define MSG_SET_COMPAND 0x806
	/*
	 * SET_COMPAND(timeslot, value)
	 * selects the type of companding for a transmit timeslot
	 * 0=u-law, 1=A-law
	 * h->s only
	 */

#define MSG_SET_INTSC 0x807
	/*
	 * SET_INTSC(ts, comp)
	 * Sets input timeslot for a task to 'ts'. The value 0xffffffff
	 * indicates that no timeslot is to be used. 'comp' indicates the type
	 * of companding to use: 0=none, 1=A-law, 2=mu-law
	 * h->s only
	 */
#define MSG_SET_TSC_DISCONNECT 0xffffffff
#define MSG_SET_TSC_RAW 0
#define MSG_SET_TSC_ALAW 1
#define MSG_SET_TSC_MULAW 2

#define MSG_SET_OUTTSC 0x808
	/*
	 * SET_OUTTSC(ts, comp)
	 * Sets output timeslot for a task to 'ts'. The value 0xffffffff
	 * indicates that no timeslot is to be used. 'comp' indicates the type
	 * of companding to use: 0=none, 1=A-law, 2=mu-law
	 * h->s only
	 */

#define MSG_SET_MINREP 0x80d
	/*
	 * SET_MINREP(bits [, timeout])
	 * Sets the parameters for reporting data. A report is
	 * required if at least 'bits' bits of data are outstanding
	 * or if data arrives and no more data arrives in 'timesout'
	 * milliseconds (timeout value 0 disables the timesout).
	 * DATA message
	 * h->s only
	 */

#define MSG_NOMORE 0x80e
	/*
	 * NOMORE()
	 * indicates that no more data is to follow and task should
	 * terminate when all data has been sent
	 * h->s only
	 */

#define MSG_HAD_PATTERN 0x808
	/*
	 * HAD_PATTERN()
	 * reports that a pattern has been found in the data, so a
	 * ReadFile() should complete with any available data
	 * s->h only
	 */

#define MSG_FRIEND 0x809
	/*
	 * HAD_FRIEND(taskid, [type])
	 * Sets a task's friend task. The type is omitted if zero, and
	 * indicates the type of friend
	 * h->s only
	 */

#define FRIEND_TYPE_DEFAULT 0
#define FRIEND_TYPE_IWR_MODEL 1
#define FRIEND_TYPE_TONE_DETECTOR 2
#define FRIEND_TYPE_TONE_DETPAR 3
	/*
	 * NB: every DATAFEED friend type must have two variants: RX and TX
	 * and the value of the TX variant must be one greater than the value
	 * of the RX variant.
	 */
#define FRIEND_TYPE_OUT_DATAFEED_RX 4
#define FRIEND_TYPE_OUT_DATAFEED_TX 5
#define FRIEND_TYPE_IN_DATAFEED_RX 6
#define FRIEND_TYPE_IN_DATAFEED_TX 7
#define FRIEND_TYPE_ECREF_DATAFEED_RX 8
#define FRIEND_TYPE_ECREF_DATAFEED_TX 9
#define FRIEND_TYPE_BG_DATAFEED_RX 10
#define FRIEND_TYPE_BG_DATAFEED_TX 11

#define FRIEND_TYPE_H223_MUX_H245_DATAFEED_RX 12
#define FRIEND_TYPE_H223_MUX_H245_DATAFEED_TX 13
#define FRIEND_TYPE_H223_MUX_AUDIO_DATAFEED_RX 14
#define FRIEND_TYPE_H223_MUX_AUDIO_DATAFEED_TX 15
#define FRIEND_TYPE_H223_MUX_VIDEO_DATAFEED_RX 16
#define FRIEND_TYPE_H223_MUX_VIDEO_DATAFEED_TX 17

#define FRIEND_TYPE_H223_DEMUX_H245_DATAFEED_RX 18
#define FRIEND_TYPE_H223_DEMUX_H245_DATAFEED_TX 19
#define FRIEND_TYPE_H223_DEMUX_AUDIO_DATAFEED_RX 20
#define FRIEND_TYPE_H223_DEMUX_AUDIO_DATAFEED_TX 21
#define FRIEND_TYPE_H223_DEMUX_VIDEO_DATAFEED_RX 22
#define FRIEND_TYPE_H223_DEMUX_VIDEO_DATAFEED_TX 23

#define FRIEND_TYPE_I460_MUX_0_DATAFEED_RX 24
#define FRIEND_TYPE_I460_MUX_0_DATAFEED_TX 25
#define FRIEND_TYPE_I460_MUX_1_DATAFEED_RX 26
#define FRIEND_TYPE_I460_MUX_1_DATAFEED_TX 27
#define FRIEND_TYPE_I460_MUX_2_DATAFEED_RX 28
#define FRIEND_TYPE_I460_MUX_2_DATAFEED_TX 29
#define FRIEND_TYPE_I460_MUX_3_DATAFEED_RX 30
#define FRIEND_TYPE_I460_MUX_3_DATAFEED_TX 31
#define FRIEND_TYPE_I460_MUX_4_DATAFEED_RX 32
#define FRIEND_TYPE_I460_MUX_4_DATAFEED_TX 33
#define FRIEND_TYPE_I460_MUX_5_DATAFEED_RX 34
#define FRIEND_TYPE_I460_MUX_5_DATAFEED_TX 35
#define FRIEND_TYPE_I460_MUX_6_DATAFEED_RX 36
#define FRIEND_TYPE_I460_MUX_6_DATAFEED_TX 37
#define FRIEND_TYPE_I460_MUX_7_DATAFEED_RX 38
#define FRIEND_TYPE_I460_MUX_7_DATAFEED_TX 39

#define FRIEND_TYPE_REDIRECT_RX_DATAFEED_RX 40
#define FRIEND_TYPE_REDIRECT_RX_DATAFEED_TX 41
#define FRIEND_TYPE_REDIRECT_TX_DATAFEED_RX 42
#define FRIEND_TYPE_REDIRECT_TX_DATAFEED_TX 43

#define FRIEND_TYPE_FRAMER_RX_IN_DATAFEED_RX 44
#define FRIEND_TYPE_FRAMER_RX_IN_DATAFEED_TX 45
#define FRIEND_TYPE_FRAMER_RX_OUT_DATAFEED_RX 46
#define FRIEND_TYPE_FRAMER_RX_OUT_DATAFEED_TX 47
#define FRIEND_TYPE_FRAMER_TX_IN_DATAFEED_RX 48
#define FRIEND_TYPE_FRAMER_TX_IN_DATAFEED_TX 49
#define FRIEND_TYPE_FRAMER_TX_OUT_DATAFEED_RX 50
#define FRIEND_TYPE_FRAMER_TX_OUT_DATAFEED_TX 51

#define FRIEND_TYPE_SYNCED_AUDIO_VIDEO_BUFFER 52
	// collector
#define FRIEND_TYPE_COLL 64
	// dispatcher
#define FRIEND_TYPE_DISP 65
	// V.32 tx for rx
#define FRIEND_TYPE_V32 66
	// civman
#define FRIEND_TYPE_CIVMAN 67
	// iSAC
#define FRIEND_TYPE_ISAC 68
	// VMP[tx] CSRC list
#define FRIEND_TYPE_CSRCLIST 69
	// echomain
#define FRIEND_TYPE_ECHOMAIN 70
	// sprt from grabber
#define FRIEND_TYPE_SPRT 71
	// sprt packetiser
#define FRIEND_TYPE_SPRT_PACKETISER 72
	// playAVF
#define FRIEND_TYPE_PLAYAVF 73
	// recAVF
#define FRIEND_TYPE_RECAVF 74
	// dtlsrx
#define FRIEND_TYPE_DTLSRX 75
#define FRIEND_TYPE_DTLSRX_RTCP 76

#define MSG_CONFID 0x80b
	/*
	 * CONFID(task-id, conf-id)
	 * reports the conference ID used by the specified task
	 * s->h only
	 */

#define MSG_LOADADDR 0x80c
	/*
	 * LOADADDR(avg_addr, max_addr)
	 * reports the addresses from which CPU load can be read
	 * s->h only
	 */

#define MSG_RESOURCETBL 0x80f
	/*
	 * RESOURCETBL(resdesc)
	 * reports the address of the resource descriptor. This is
	 * an array of items (tag, value) terminated by a tag value of
	 * RESOURCE_FREE_END. For other tags, the 'value' is a memory
	 * address from where the current resource usage can be read.
	 * s->h only
	 */

#define MSG_STRING 0x810
	/*
	 * STRING(stringid, strlen)
	 * reports that a string message is next to be read
	 * generated for rdmsg ioctl
	 * MUST NOT BE SENT BY API OR MODULE
	 */

#define MSG_DISCARD_DATA 0xcc6
	/*
	 * DISCARD_DATA()
	 * discard all pending data and don't allow more until resumed
	 * h->s only
	 */

#define MSG_RESUME_DATA 0xcc7
	/*
	 * RESUME_DATA()
	 * resume data processing
	 * h->s only
	 */

#define MSG_FAULT 0xbfe
	/*
	 * FAULT()
	 * causes firmware to call Fault. Not normally used, but
	 * reserved for debugging (such as for filling unused memory
	 * areas).
	 */

#define MSG_OVERRUN 0xbff
	/*
	 * OVERRUN()
	 * pseudo-message used by the driver to replace message words
	 * when they are discarded when there is insufficient space
	 * to hold them
	 *
	 * s->h only
	 */

#define MSG_BADMSG 0xc00
	/*
	 * BADMSG(msgcode, cause)
	 * reports the reception of an invalid message
	 * s->h only
	 */

#define MSG_BADMSG_CAUSE_NOTASK 0
#define MSG_BADMSG_CAUSE_UNHANDLED 1
#define MSG_BADMSG_CAUSE_RANGE 2
#define MSG_BADMSG_CAUSE_MEMORY 3
#define MSG_BADMSG_CAUSE_REDUNDANT 4
#define MSG_BADMSG_CAUSE_WOULD_CYCLE 5
#define MSG_BADMSG_CAUSE_LICENCE 6

#define MSG_UNDERRUN 0xc01
	/*
	 * UNDERRUN()
	 * indicates that a channel has experienced underrun
	 * s->H only
	 */

#define MSG_PLAY_STOP 0xe00
	/*
	 * PLAY_STOP(discarded)
	 * stops a play task
	 * h->s: no parameter, requests task to stop
	 * s->h: parameter indicates number of bytes of data discarded
	 *
	 * PLAY_STOP()
	 * signals recording subtask is stopping
	 * s->h only
	 */

#ifdef DBG_MSGPOS
#define MSG_DEBUG_MSGPOS 0xffe
	/*
	 * DEBUG_MSGPOS(ticknum)
	 * position in message stream (number of words since * start)
	 */
#endif

#define MSG_DEBUG_USER 0xfff
	/*
	 * DEBUG_USER(?)
	 * reusable message for debugging purposes
	 */

#endif
