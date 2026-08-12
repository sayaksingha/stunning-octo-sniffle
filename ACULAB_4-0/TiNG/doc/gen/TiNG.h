// Automatically generated (from m1504.) - do not edit

#define IOCTL_TiNG_RDIO IOCTL_TiNG(0)
// #define IOCTL_TiNG_WRIO IOCTL_TiNG(1)
// 
// performs direct access to I/O registers on the card. Only
// reading is permitted as writing is too dangerous.
struct ting_cardreg {
	U32 ioctl_length;
	U32 nreg;	 //The number of registers to access.
	struct  {
		U32 ofs;	 //The offset to an I/O register.
		U32 val;	 //The value read from an I/O register.
	} reg[TiNG_VARIABLE_LENGTH];
		// An array of registers accessed. The nreg
		// field specifies the number of entries in this array.
} ;

#define IOCTL_TiNG_RDMEM IOCTL_TiNG(2)
#define IOCTL_TiNG_WRMEM IOCTL_TiNG(3)
// 
// reads or writes SHARC memory.
struct ting_sharcmem {
	U32 ioctl_length;
	SHARCADDR address;
		// the address of the start of the block to access
	U32 length;	 //the number of words to read or write
	SHARCDATA val[TiNG_VARIABLE_LENGTH];
		// the data to write (if a write) or the data read (if a read).
		// The length
		// field specified the number of entries in this array.
} ;

#define IOCTL_TiNG_RDSTATS IOCTL_TiNG(4)
// #define IOCTL_TiNG_WRSTATS IOCTL_TiNG(5)
// 
// reads or writes statistics.
struct ting_statistics {
	U32 ioctl_length;
	U32 numirq_run;
		// the number of times the interrupt service routine (ISR) was run
	U32 numirq_bogus;
		// the number of times that the ISR found the device was not
		// interrupting
	U32 numirq_dma;
		// the number of times that the ISR found the device was
		// interrupting because of DMA completion
	U32 numirq_ours;
		// the number of times that the ISR found this module on the device
		// needed service
	struct avdata {
		U32 num;	 //The number of transfers
		U32 max;	 //The maximum transfer size
		U32 average;	 //The average transfer size
		S32 remainder;	 //The remainder when computing the average
		U32 sqaverage;	 //The average of squared transfer sizes
		S32 sqremainder;
			// The remainder when computing the average of squared transfer sizes
	} rwsize[2], qsize[2];
		// Statistics for read/write and message queue transfers. The
		// [0] subscript is the Output (write) value and [1] holds the
		// Input (read) values.
} ;

#define IOCTL_TiNG_RDMSG IOCTL_TiNG(6)
#define IOCTL_TiNG_WRMSG IOCTL_TiNG(7)
// 
// performs message transfer with a channel.
struct ting_message {
	U32 ioctl_length;
	U32 msg[17];
		// The message data. The first word encodes the message length.
	U32 flags;	 //A bitmask of these flags.
#define IOCTL_TiNG_RDMSG_FLAG_NOWAIT 1
		// do not wait when reading if there is no message available
#define IOCTL_TiNG_RDMSG_FLAG_KEEPSTRINGS 2
		// report string messages rather than discard them
	U32 capacity;
		// The current capacity in bits. This is the amount of data
		// or space available, depending on the type of task running on
		// this channel.
	U32 hasmsg;
		// Indicates whether or not a message follows any data reported
		// in the capacity
		// field. This is needed by the speech API to decide whether or
		// not to report the capacity.
} ;

// #define IOCTL_TiNG_RDMSG_NOWAIT IOCTL_TiNG(8)
// #define IOCTL_TiNG_WRMSG_NOWAIT IOCTL_TiNG(9)
// 
	// obsolete

#define IOCTL_TiNG_RDDEST IOCTL_TiNG(10)
#define IOCTL_TiNG_WRDEST IOCTL_TiNG(11)
// selects and reports destination of a handle.
struct ting_destination {
	U32 ioctl_length;
	U32 dest;	 //the type of destination - one of these values
	U32 modmask;
		// whole card (modmask is ignored on write)
		// a module
		// a channel
		// an event
		// the input half of the special trace device shared by all cards
		// the output half of the special trace device shared by all cards
		// bitmap of acceptable modules
	U32 taskid;
		// the task ID of a currently running task on this channel (read-only)
	U32 channo;	 //the channel number of this channel (read-only)
	U32 magic;	 //the magic number of this channel or event (read-only)
	U32 event;	 //an event identifier (read-only)
} ;
enum  {
	TiNG_DEST_CARD,
	TiNG_DEST_MODULE,
	TiNG_DEST_CHANNEL,
	TiNG_DEST_EVENT,
	TiNG_DEST_TRACEIN,
	TiNG_DEST_TRACEOUT,
} ;

#define IOCTL_TiNG_RDTASK IOCTL_TiNG(12)
#define IOCTL_TiNG_WRTASK IOCTL_TiNG(13)
// 
// starts a task running or reports which task is running.
struct ting_task {
	U32 ioctl_length;
	U32 tasktypecode;
		// The type code of the task. These are fixed values defined in
		// the file <code>hsif/typecode.h</code>
	U32 version;	 //The version number of the task.
} ;

#define IOCTL_TiNG_RDOUTTS IOCTL_TiNG(14)
#define IOCTL_TiNG_WROUTTS IOCTL_TiNG(15)
// 
// controls switching of outputs to DSP module's serial port.
struct ting_tslist {
	U32 ioctl_length;
	U32 numtimeslot;	 //the number of timeslots
	TiNG_TIMESLOT timeslot[TiNG_VARIABLE_LENGTH];
		// an array of timeslots. The numtimeslot
		// field specifies the number of entries in this array.
} ;

#define IOCTL_TiNG_RDINTSLIST IOCTL_TiNG(16)
#define IOCTL_TiNG_WRINTSLIST IOCTL_TiNG(17)
// 
// controls switching of inputs from DSP module's serial port.

#define IOCTL_TiNG_RDSWITCH IOCTL_TiNG(18)
// #define IOCTL_TiNG_WRSWITCH IOCTL_TiNG(19)
// 
// reads switching assignments (only for debugging).
struct ting_switch {
	U32 ioctl_length;
	U32 swtype;
		// reports which type of switch is used. One of these values:
	union  {
		struct ting_switch_mt8980 {
			struct ting_switch_mt8980_stream {
				U8 low[32];
				U8 high[32];
			} stream[8];
		} mt8980[2];
		struct ting_switch_t8100 {
			struct  {
				unsigned che:1;
				unsigned fme:1;
				unsigned pme:1;
				unsigned xcs:1;
				unsigned stream:4;
				unsigned timeslot:8;
			} local[0x800];
			struct ting_switch_t8100_cam {
				unsigned valid:1;
				unsigned isinput:1;
				unsigned pattern:1;
				unsigned fme:1;
				unsigned stream:4;
				unsigned timeslot:8;
				unsigned tag:8;
			} cam[3][256];
			U8 reg[256];
		} t8100[3];
	} u;
} ;
enum  {
	TiNG_SWITCH_NONE,
	TiNG_SWITCH_MT8980,
	TiNG_SWITCH_T8100,
} ;

#define IOCTL_TiNG_RDXFER IOCTL_TiNG(20)
#define IOCTL_TiNG_WRXFER IOCTL_TiNG(21)
// 
// sets or reports data transfer characteristics
struct ting_xfer {
	U32 ioctl_length;
	S32 minimum_bits;
		// The threshold for reporting data or space being ready.
	U32 timeout_milliseconds;
		// The timeout used in conjunction with the number of bits.
	U32 nomore;	 //flag: non-zero means no more data to transfer.
} ;

#define IOCTL_TiNG_RDDBG IOCTL_TiNG(22)
#define IOCTL_TiNG_WRDBG IOCTL_TiNG(23)
// 
// controls debugging
struct ting_debug {
	U32 ioctl_length;
	U32 debug_mask;	 //A bitmask of these values
#define TiNG_DEBUG_DUMP 1
		// the current state is to be dumped in text form to the trace
#define TiNG_DEBUG_TRACE 2
		// future operations on this object are to generate trace
#define TiNG_DEBUG_TRACE_DATA 4
		// future data transfers on this object are to generate trace
#define TiNG_DEBUG_SUPPRESS_TRACE 8
		// suppress trace from driver (ignored unless target is whole
		// card)
} ;

#define IOCTL_TiNG_RDMSTATE IOCTL_TiNG(24)
#define IOCTL_TiNG_WRMSTATE IOCTL_TiNG(25)
// 
// sets or reports current module status.
struct ting_mstate {
	U32 ioctl_length;
	U32 mstate;
	U32 avgcpuload;
		// no such module
		// module is faulty - unusable
		// crashed after booting
		// being reset
		// not finished downloading firmware kernel
		// finished downloading firmware kernel - awaiting response
		// normal running
		// the state of the DSP module
		// average CPU loading over last second
	U32 maxcpuload;	 //maximum CPU loading over last second
	SHARCADDR restbl;
		// the address of the resource table, 0 if unknown. The resource
		// table contains a list of tags and pointers to counters. The
		// tag <code>0</code> marks the end of the table.
	U32 connstate;
		// the state of the connection to the module, 0 if connected
		// and non zero if the connection has stopped
} ;
enum TiNG_mstate {
	MS_ABSENT,
	MS_FAULT,
	MS_CRASHED,
	MS_RESET,
	MS_BOOTING,
	MS_BOOTWAIT,
	MS_RUNNING,
} ;

// #define IOCTL_TiNG_RDWHOLECHAN IOCTL_TiNG(26)
// #define IOCTL_TiNG_WRWHOLECHAN IOCTL_TiNG(27)
// 
// was used for debugging
	// obsolete

#define IOCTL_TiNG_RDCHANSTATE IOCTL_TiNG(28)
#define IOCTL_TiNG_WRCHANSTATE IOCTL_TiNG(29)
// sets or reports a channel state
struct ting_chanstate {
	U32 ioctl_length;
	U32 channo;	 //a channel number
	U32 magic;	 //the channel's magic number
	U32 chstate;
} ;
	// channel is not open
	// channel is currently open
enum TiNG_chstate {
	CH_FREE,
	CH_OPEN,
} ;

#define IOCTL_TiNG_RDFRIEND IOCTL_TiNG(30)
#define IOCTL_TiNG_WRFRIEND IOCTL_TiNG(31)
// 
// sets a friend channel
struct ting_friend {
	U32 ioctl_length;
	U32 channo;	 //a channel number
	U32 magic;	 //the channel's magic number
	U32 type;
		// the friend type. This value is passed as the <code>type</code>
		// field in the message.
} ;

#define IOCTL_TiNG_RDCHEVENT IOCTL_TiNG(32)
#define IOCTL_TiNG_WRCHEVENT IOCTL_TiNG(33)
// 
// handles the linkage between events and channels
struct ting_chevent {
	U32 ioctl_length;
#if defined(TiNGTYPE_LINUX) || defined(TiNGTYPE_QNX)
	U32 event;	 //(Unix only) an event number
	U32 magic;	 //(Unix only) the event's magic number
#endif
#ifdef TiNGTYPE_WINNT
	U32 evtype;	 //(NT only) an event type - no longer used
	HANDLE event;	 //(NT only) an NT event HANDLE
#endif
} ;

#if defined(TiNGTYPE_LINUX) || defined(TiNGTYPE_QNX)
#define IOCTL_TiNG_RDDAEMON IOCTL_TiNG(34)
#define IOCTL_TiNG_WRDAEMON IOCTL_TiNG(35)
// provides a kernel thread (Unix only).
struct ting_daemon {
	U32 ioctl_length;
} ;
#endif

#define IOCTL_TiNG_RDTASKLIST IOCTL_TiNG(36)
// #define IOCTL_TiNG_WRTASKLIST IOCTL_TiNG(37)
// 
// provides a list of task types available
struct ting_taskitem {
	U32 ioctl_length;
	U32 slot;	 //a slot number (0..N-1).
	U32 tasktypecode;	 //the typecode of this task type or zero.
	U32 version;	 //the version of this task type.
} ;

#define IOCTL_TiNG_RDCARDINFO IOCTL_TiNG(38)
// #define IOCTL_TiNG_WRCARDINFO IOCTL_TiNG(39)
// 
// reports information about the hardware.
struct ting_cardinfo {
	U32 ioctl_length;
	U32 cardtype;
	U32 ioaddress;
		// ISA S2 card
		// PCI P1 card
		// ISA Basic Rate BR8 card
		// CompactPCI C1 card
		// PCI P2 card
		// Prosody S softcard
		// Prosody X card
		// Unknown card type (either a card without any Prosody
		// hardware, or one designed after the driver was built).
		// The type of this card
		// An I/O base address used by the card
	U32 interrupt;	 //The interrupt used by the card
	U32 memaddress;	 //A memory base address used by the card
	U32 switchno;	 //The associated switch driver number
	char serialno[16];	 //The serial number of this card
	char verinfo[64];	 //Various version information.
} ;
enum TiNG_CARDTYPE {
	TiNG_CARDTYPE_S2,
	TiNG_CARDTYPE_P1,
	TiNG_CARDTYPE_BR8,
	TiNG_CARDTYPE_C1,
	TiNG_CARDTYPE_P2,
	TiNG_CARDTYPE_SOFT_S,
	TiNG_CARDTYPE_X,
	TiNG_CARDTYPE_UNK,
} ;

#define IOCTL_TiNG_RDMODINFO IOCTL_TiNG(40)
// #define IOCTL_TiNG_WRMODINFO IOCTL_TiNG(41)
// 
// reports information about the hardware for a module.
struct ting_modinfo {
	U32 ioctl_length;
	U32 modmask;	 //Input physical module location indication as mask.
	U32 modslot;	 //Output physical module location indication.
	U32 modfirststream;	 //Output first stream number for module
	U32 modlaststream;	 //Output last stream number for module
	U32 npstdmcards;
		// Output number of (thin) cards supporting module (non-zero if PS and if are any)
	char pstdmcards[32][16];	 //Serial numbers of above cards
	U32 cardfirststream[32];	 //Initial stream located on thin card
	U32 cardlaststream[32];	 //Last stream located on thin card
} ;

#define IOCTL_TiNG_RDSTRINFO IOCTL_TiNG(42)
// #define IOCTL_TiNG_WRSTRINFO IOCTL_TiNG(43)
// 
// reports information about a TDM stream.
struct ting_strinfo {
	U32 ioctl_length;
	U32 modstrix;	 //Input module stream index.
	U32 serialportix;	 //Output serial port ix.
	U32 timeslots;	 //Output timeslot count
	char tdmcard[16];	 //Serial numbers of underlying card
} ;

#define IOCTL_TiNG_RDCONFIN IOCTL_TiNG(44)
#define IOCTL_TiNG_WRCONFIN IOCTL_TiNG(45)
// 
// prepares a conference input
struct ting_confin {
	U32 ioctl_length;
	U32 channo;	 //The channel number of the CONFMAN task.
	U32 magic;	 //The channel magic number of the CONFMAN task.
	U32 id;	 //The returned conference ID.
} ;

#define IOCTL_TiNG_RDCONFINFO IOCTL_TiNG(46)
// #define IOCTL_TiNG_WRCONFINFO IOCTL_TiNG(47)
// 
// reports on a conference
struct ting_confinfo {
	U32 ioctl_length;
	U32 speakers[2];	 //bitmap of current active speakers
} ;

#define IOCTL_TiNG_RDALLOCTS IOCTL_TiNG(48)
#define IOCTL_TiNG_WRALLOCTS IOCTL_TiNG(49)
// 
// provides serial port timeslot allocation
struct ting_allocts {
	U32 ioctl_length;
	U32 isinput;	 //Selects input or output timeslots.
	TiNG_TIMESLOT tsmin;	 //Minimum timeslot to consider / result.
	TiNG_TIMESLOT tsmax;	 //Maximum timeslot to consider.
} ;

#define IOCTL_TiNG_RDTSDEST IOCTL_TiNG(50)
#define IOCTL_TiNG_WRTSDEST IOCTL_TiNG(51)
// 
// controls switching between serial port timeslots and MVIP timeslots
struct ting_tsdest {
	U32 ioctl_length;
	U32 timeslot;	 //Timeslot number on serial port (tx{0,1}, rx{0,1})
	struct  {
		U32 stream;	 //Stream on MVIP bus.
#define TiNG_MVIP_STREAM_NONE ~0U
		U32 timeslot;	 //Timeslot on MVIP stream.
	} mvip;
} ;

#define IOCTL_TiNG_RDTRACE IOCTL_TiNG(52)
#define IOCTL_TiNG_WRTRACE IOCTL_TiNG(53)
// 
// controls remote tracing
struct ting_trace {
	U32 ioctl_length;
	U32 trace_level;	 //Desired trace level to use.
} ;



union ting_ioctl {
#ifdef IOCTL_TiNG_RDIO
	struct ting_cardreg ting_cardreg;
#endif
#ifdef IOCTL_TiNG_RDMEM
	struct ting_sharcmem ting_sharcmem;
#endif
#ifdef IOCTL_TiNG_RDSTATS
	struct ting_statistics ting_statistics;
#endif
#ifdef IOCTL_TiNG_RDMSG
	struct ting_message ting_message;
#endif
#ifdef IOCTL_TiNG_RDDEST
	struct ting_destination ting_destination;
#endif
#ifdef IOCTL_TiNG_RDTASK
	struct ting_task ting_task;
#endif
#ifdef IOCTL_TiNG_RDOUTTS
	struct ting_tslist ting_tslist;
#endif
#ifdef IOCTL_TiNG_RDSWITCH
	struct ting_switch ting_switch;
#endif
#ifdef IOCTL_TiNG_RDXFER
	struct ting_xfer ting_xfer;
#endif
#ifdef IOCTL_TiNG_RDDBG
	struct ting_debug ting_debug;
#endif
#ifdef IOCTL_TiNG_RDMSTATE
	struct ting_mstate ting_mstate;
#endif
#ifdef IOCTL_TiNG_RDCHANSTATE
	struct ting_chanstate ting_chanstate;
#endif
#ifdef IOCTL_TiNG_RDFRIEND
	struct ting_friend ting_friend;
#endif
#ifdef IOCTL_TiNG_RDCHEVENT
	struct ting_chevent ting_chevent;
#endif
#ifdef IOCTL_TiNG_RDDAEMON
	struct ting_daemon ting_daemon;
#endif
#ifdef IOCTL_TiNG_RDTASKLIST
	struct ting_taskitem ting_taskitem;
#endif
#ifdef IOCTL_TiNG_RDCARDINFO
	struct ting_cardinfo ting_cardinfo;
#endif
#ifdef IOCTL_TiNG_RDMODINFO
	struct ting_modinfo ting_modinfo;
#endif
#ifdef IOCTL_TiNG_RDSTRINFO
	struct ting_strinfo ting_strinfo;
#endif
#ifdef IOCTL_TiNG_RDCONFIN
	struct ting_confin ting_confin;
#endif
#ifdef IOCTL_TiNG_RDCONFINFO
	struct ting_confinfo ting_confinfo;
#endif
#ifdef IOCTL_TiNG_RDALLOCTS
	struct ting_allocts ting_allocts;
#endif
#ifdef IOCTL_TiNG_RDTSDEST
	struct ting_tsdest ting_tsdest;
#endif
#ifdef IOCTL_TiNG_RDTRACE
	struct ting_trace ting_trace;
#endif
};
