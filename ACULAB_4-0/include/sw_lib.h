/*****************************************************************************\
*
* SW_LIB.H : V6 Switch API Declarations
*
* (C) 2002-2011 Aculab Plc
*
* Version 6.5.1
*
\*****************************************************************************/
#ifndef SW_LIB_H_INCLUDED
#define SW_LIB_H_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif

#include "acu_type.h"

#ifdef UNIX_SYSTEM

#ifdef SW_POLL_UNIX
typedef struct tSWEventId { ACU_INT fd; ACU_INT mode; } tSWEventId;
#else /* NOT POLL */

#ifdef SW_SEL_UNIX
typedef struct tSWEventId { ACU_INT fd; ACU_INT mode; } tSWEventId;
#else
typedef int tSWEventId;
#endif

#endif /* POLL */

#endif /* NOT UNIX */

#ifdef _WIN32
#pragma pack(push, 8)
#endif

enum
{
/*
 * Some stream no. definitions.
 */
	kSW_LI_OUT_TO_DSI  =   0,
	kSW_LI_OUT_TO_DSO  =   8,
	kSW_LI_IN_FROM_DSO =   0,
	kSW_LI_IN_FROM_DSI =   8,

/*
 * Prosody X card stream numbering
 */

	/*
	 *  Streams for network ports
	 */
	kSW_LI_PX_NET1  =  32,
	kSW_LI_PX_NET2  =  33,
	kSW_LI_PX_NET3  =  34,
	kSW_LI_PX_NET4  =  35,
	kSW_LI_PX_NET5  =  36,
	kSW_LI_PX_NET6  =  37,
	kSW_LI_PX_NET7  =  38,
	kSW_LI_PX_NET8  =  39,
	kSW_LI_PX_NET9  =  40,
	kSW_LI_PX_NET10 =  41,
	kSW_LI_PX_NET11 =  42,
	kSW_LI_PX_NET12 =  43,
	kSW_LI_PX_NET13 =  44,
	kSW_LI_PX_NET14 =  45,
	kSW_LI_PX_NET15 =  46,
	kSW_LI_PX_NET16 =  47,

	/*
	 * Streams for (CAS/SS7) DSPs fitted on PMX modules on Prosody X V2 cards
	 * or FPGA logic equivalents on Prosody X V3 and Prosody T cards.
	 * The application should use call_get_port_dsp_stream() to get the stream
	 * for CAS 'DSPs'. This might return values other than those below.
	 */
	kSW_LI_PX_DSP0_A1 =  48,
	kSW_LI_PX_DSP0_A2 =  49,
	kSW_LI_PX_DSP0_A3 =  50,
	kSW_LI_PX_DSP0_A4 =  51,
	kSW_LI_PX_DSP0_B1 =  52,
	kSW_LI_PX_DSP0_B2 =  53,
	kSW_LI_PX_DSP0_B3 =  54,
	kSW_LI_PX_DSP0_B4 =  55,
	kSW_LI_PX_DSP1_A1 =  56,
	kSW_LI_PX_DSP1_A2 =  57,
	kSW_LI_PX_DSP1_A3 =  58,
	kSW_LI_PX_DSP1_A4 =  59,
	kSW_LI_PX_DSP1_B1 =  60,
	kSW_LI_PX_DSP1_B2 =  61,
	kSW_LI_PX_DSP1_B3 =  62,
	kSW_LI_PX_DSP1_B4 =  63,

	/*
	 * Streams (128 timeslots each) for TiNG DSP modules on Prosody X cards.
	 * On Prosody T cards use sw_get_prosody_s_stream_info() to find the streams.
	 * The values may be outside the range below.
	 */
	kSW_LI_PX_TiNG1_0 =  64,
	kSW_LI_PX_TiNG1_1 =  65,
	kSW_LI_PX_TiNG2_0 =  66,
	kSW_LI_PX_TiNG2_1 =  67,
	kSW_LI_PX_TiNG3_0 =  68,
	kSW_LI_PX_TiNG3_1 =  69,
	kSW_LI_PX_TiNG4_0 =  70,
	kSW_LI_PX_TiNG4_1 =  71,
	/*
	 * 16 streams available on some obsolete cards.
	 */
	kSW_LI_PX_TiNG1_2 =  72,
	kSW_LI_PX_TiNG1_3 =  73,
	kSW_LI_PX_TiNG2_2 =  74,
	kSW_LI_PX_TiNG2_3 =  75,
	kSW_LI_PX_TiNG3_2 =  76,
	kSW_LI_PX_TiNG3_3 =  77,
	kSW_LI_PX_TiNG4_2 =  78,
	kSW_LI_PX_TiNG4_3 =  79,
	kSW_LI_PX_TiNG5_0 =  80,
	kSW_LI_PX_TiNG5_1 =  81,
	kSW_LI_PX_TiNG6_0 =  82,
	kSW_LI_PX_TiNG6_1 =  83,
	kSW_LI_PX_TiNG7_0 =  84,
	kSW_LI_PX_TiNG7_1 =  85,
	kSW_LI_PX_TiNG8_0 =  86,
	kSW_LI_PX_TiNG8_1 =  87,

	/*
	 * Special pseudo stream on PT cards to configure the tone generators's base tones.
	 * Timeslot number is the tone number. Set mode to BASE_TONE_MODE.
	 * Tone 0 is silence, tones 1 to 63 are fixed (used by CAS), tones 64 to 127 are user-settable.
	 */
	kSW_LI_PT_TONE_GEN = 255,
	/*
	 * Special pseudo stream on PT cards to read the tones detected by the FFT block from the
	 * input TDM ports.
	 * Used with sw_query_output() even though it is really an input.
	 * Three tones are returned with a resolution of about 8Hz (accuracy is a bit worse).
	 * mode will be set to FFT_RESULT_MODE.
	 */
	kSW_LI_PT_FFT_RESULT = 96,    /* Add TDM port number */

	ERR_SW_INVALID_COMMAND        =  (-200),  /* Command code is not supported */
	ERR_SW_DEVICE_ERROR           =  (-202),  /* An error was returned from a device driver called by this driver. */
	ERR_SW_NO_RESOURCES           =  (-204),  /* An internal driver resource has been exhausted. */
	ERR_SW_INVALID_SWITCH         =  (-209),  /* Out of range switch driver index */
	ERR_SW_INVALID_STREAM         =  (-210),  /* Stream number in parameter list is out of range. */
	ERR_SW_INVALID_TIMESLOT       =  (-211),  /* Time slot in parameter list is out of range. */
	ERR_SW_INVALID_CLOCK_PARM     =  (-213),  /* Invalid clock configuration parameter */
	ERR_SW_INVALID_MODE           =  (-216),  /* Incorrect SET_OUTPUT or QUERY_OUTPUT mode. */
	ERR_SW_INVALID_MINOR_SWITCH   =  (-217),  /* Minor (internal) switch error. */
	ERR_SW_INVALID_PARAMETER      =  (-218),  /* General invalid parameter error. */
	ERR_SW_NO_PATH                =  (-220),  /* Connection cannot be made due to switch limitation */
	ERR_SW_NO_SCBUS_CLOCK         =  (-224),  /* No card driving SCbus clock. */
	ERR_SW_OTHER_SCBUS_CLOCK      =  (-225),  /* Another non-MVSWDEV controlled card is driving scbus clock */
	ERR_SW_PATH_BLOCKED           =  (-226),  /* eg. because of MVIP output MUX */
	ERR_SW_OS_INTERRUPTED         =  (-227),  /* eg. Unix event wait interrupted through signal */
	ERR_SW_OS_INCONSISTENT_STATE  =  (-228),  /* Switch device and driver inconsistent */
	ERR_SW_PORT_NOT_LOADED        =  (-229),  /* Firmware is not running on PMX port  */
	ERR_SW_PORT_RATE_MISMATCH     =  (-230),  /* NETREF rate does not match line rate */
	ERR_SW_PMX_FPGA               =  (-231),  /* request not supported by PMX FPGA */
	ERR_SW_COMPONENT_MISMATCH     =  (-232),  /* pxscs and t8110.ko components do not match on Prosody X */
	ERR_SW_NO_SUCH_DSP            =  (-233),  /* No DSP is fitted in requested position */
	ERR_SW_NO_SUCH_DSP_PORT       =  (-234),  /* Requested DSP serial port does not exist */
	ERR_SW_PXSCS_TIMED_OUT        =  (-235),  /* Prosody X only: an API call has timed out */
	ERR_SW_PXSCS_UNKNOWN_API_CALL =  (-236),  /* Prosody X only: a new PX_SW_Driver package may need to be installed */
	ERR_SW_T8110_UNKNOWN_IOCTL    =  (-237),  /* Prosody X only: a new Prosody_IP_Firmware image may need to be flashed */
	ERR_SW_PXSCS_COMMS_FAILED     =  (-238),  /* Prosody X only: problem communicating with pxscs on Prosody X card */
	ERR_SW_API_CALL_ABORTED       =  (-239),  /* Prosody X only: an application has called sw_abort_api_calls() */
	ERR_SW_SIZE_PARAMETER         =  (-240),  /* Struct size in API call less than size expected by switch library */
	ERR_SW_FPGA_NOT_READY         =  (-241),  /* Prosody X V3 only: driver unable to access switch device */

/*
 * For compatibility with old error names.
 */
	MVIP_INVALID_COMMAND      =  ERR_SW_INVALID_COMMAND,
	MVIP_DEVICE_ERROR         =  ERR_SW_DEVICE_ERROR,
	MVIP_NO_RESOURCES         =  ERR_SW_NO_RESOURCES,
	MVIP_INVALID_STREAM       =  ERR_SW_INVALID_STREAM,
	MVIP_INVALID_TIMESLOT     =  ERR_SW_INVALID_TIMESLOT,
	MVIP_INVALID_CLOCK_PARM   =  ERR_SW_INVALID_CLOCK_PARM,
	MVIP_INVALID_MODE         =  ERR_SW_INVALID_MODE,
	MVIP_INVALID_MINOR_SWITCH =  ERR_SW_INVALID_MINOR_SWITCH,
	MVIP_INVALID_PARAMETER    =  ERR_SW_INVALID_PARAMETER,
	MVIP_NO_PATH              =  ERR_SW_NO_PATH,
	MVIP_PATH_BLOCKED         =  ERR_SW_PATH_BLOCKED,

#ifdef MVSW_EXTENDED_NSWITCH
	NSWITCH =  16,  /* maximum switch drivers supported */
#else
	NSWITCH =  10,  /* maximum switch drivers supported */
#endif

	DISABLE_MODE        =  0,
	PATTERN_MODE        =  1,
	CONNECT_MODE        =  2,
	FRAMED_CONNECT_MODE =  3,   /* Not used */
	TONE_MODE           =  4,   /* Sum of two base tones */
	BASE_TONE_MODE      =  5,   /* For stream kSW_LI_PT_TONE_GEN */
	FFT_RESULT_MODE     =  6,   /* For stream kSW_LI_PT_FFT_RESULT */
	LAYER_1_LOOP_MODE   =  7,   /* For streams kSW_LI_PX_NETnn (PT cards) */

	/* Predefined base tones (for TONE_MODE) */
	BASE_TONE_SILENCE			= 0,
	BASE_TONE_700_HZ_7_DBM		= 1,	/* 1-9 are for CAS R1 digits */
	BASE_TONE_900_HZ_7_DBM		= 2,
	BASE_TONE_1100_HZ_7_DBM		= 3,
	BASE_TONE_1300_HZ_7_DBM		= 4,
	BASE_TONE_1500_HZ_7_DBM		= 5,
	BASE_TONE_1700_HZ_7_DBM		= 6,
	BASE_TONE_1900_HZ_7_DBM		= 7,
	BASE_TONE_2400_HZ_7_DBM		= 8,
	BASE_TONE_2600_HZ_7_DBM		= 9,
	BASE_TONE_1500_HZ_8_DBM		= 10,	/* 10-19 are for CAS R2 forward digits */
	BASE_TONE_1620_HZ_8_DBM		= 11,
	BASE_TONE_1740_HZ_8_DBM		= 12,
	BASE_TONE_1860_HZ_8_DBM		= 13,
	BASE_TONE_1980_HZ_8_DBM		= 14,
	BASE_TONE_1380_HZ_8_DBM		= 15,
	// BASE_TONE_1500_HZ_8_DBM		= 16,	/* Same as tone 10 */
	// BASE_TONE_1620_HZ_8_DBM		= 17,	/* Same as tone 11 */
	// BASE_TONE_1740_HZ_8_DBM		= 18,	/* Same as tone 12 */
	// BASE_TONE_1860_HZ_8_DBM		= 19,	/* Same as tone 13 */
	BASE_TONE_1020_HZ_8_DBM		= 20,	/* 10-19 are for CAS R2 backward digits */
	BASE_TONE_900_HZ_8_DBM		= 21,
	BASE_TONE_780_HZ_8_DBM		= 22,
	BASE_TONE_660_HZ_8_DBM		= 23,
	BASE_TONE_540_HZ_8_DBM		= 24,
	BASE_TONE_1140_HZ_8_DBM		= 25,
	// BASE_TONE_1020_HZ_8_DBM		= 26,	/* Same as tone 20 */
	// BASE_TONE_900_HZ_8_DBM		= 27,	/* Same as tone 21 */
	// BASE_TONE_780_HZ_8_DBM		= 28,	/* Same as tone 22 */
	// BASE_TONE_660_HZ_8_DBM		= 29,	/* Same as tone 23 */
	BASE_TONE_697_HZ_12_DBM		= 30,	/* 30-37 are the DTMF tones */
	BASE_TONE_770_HZ_12_DBM		= 31,
	BASE_TONE_852_HZ_12_DBM		= 32,
	BASE_TONE_941_HZ_12_DBM		= 33,
	BASE_TONE_1209_HZ_10_DBM	= 34,
	BASE_TONE_1336_HZ_10_DBM	= 35,
	BASE_TONE_1477_HZ_10_DBM	= 36,
	BASE_TONE_1633_HZ_10_DBM	= 37,
	BASE_TONE_425_HZ_11_DBM		= 38,	/* 38-52 are various call progress tones */
	BASE_TONE_425_HZ_4_5_DBM	= 39,
	BASE_TONE_425_HZ_6_DBM		= 40,
	BASE_TONE_440_HZ_8_DBM		= 41,
	BASE_TONE_440_HZ_3_5_DBM	= 42,
	BASE_TONE_440_HZ_10_DBM		= 43,
	BASE_TONE_350_HZ_10_DBM		= 44,
	BASE_TONE_400_HZ_10_DBM		= 45,
	BASE_TONE_450_HZ_10_DBM		= 46,
	BASE_TONE_330_HZ_14_DBM		= 47,
	BASE_TONE_440_HZ_14_DBM		= 48,
	BASE_TONE_480_HZ_19_DBM		= 49,
	BASE_TONE_620_HZ_19_DBM		= 50,
	BASE_TONE_330_HZ_5_DBM		= 51,
	BASE_TONE_440_HZ_8_5_DBM	= 52,
	/* Tones 53 to 60 are not defined */
	BASE_TONE_2001_HZ_12_DBM	= 61,   /* ITU ISUP continuity test */
	BASE_TONE_2010_HZ_12_DBM	= 62,   /* ANSI ISUP continuity test */
	BASE_TONE_1780_HZ_12_DBM	= 63,   /* ANSI ISUP 2-wire backwards tone */

	/* l1_loop_type values (for LAYER_1_LOOP_MODE) */
	LAYER_1_LOOP_NONE             = 0,
	LAYER_1_LOOP_REMOTE           = 1,          /* RX to TX near E1/T1 interface */
	LAYER_1_LOOP_PAYLOAD          = 2,          /* RX to TX near host interface */
	LAYER_1_LOOP_LOCAL            = 3,          /* TX to RX near E1/T1 interface */
	/* l1_loop_opts values (for LAYER_1_LOOP_MODE) */
	LAYER_1_LOOP_JITTER_ATTENUATE = (1u << 0),  /* For Remote loop */
	LAYER_1_LOOP_FW_DOWNLOAD      = (1u << 8),  /* Apply when link first comes up */


	CLOCK_REF_MVIP  =  0x0000,
	CLOCK_REF_SEC8K =  0x0001,
	CLOCK_REF_LOCAL =  0x0002,
	CLOCK_REF_NET1  =  0x0003,
	CLOCK_REF_NET2  =  0x0004,
	CLOCK_REF_H100  =  0x0007,
	CLOCK_REF_NET3  =  0x000C,
	CLOCK_REF_NET4  =  0x000D,
	CLOCK_REF_NET5  =  0x0005,
	CLOCK_REF_NET6  =  0x000A,
	CLOCK_REF_NET7  =  0x000B,
	CLOCK_REF_NET8  =  0x000E,
	CLOCK_REF_NET9  =  0x0011,
	CLOCK_REF_NET10 =  0x0012,
	CLOCK_REF_NET11 =  0x0013,
	CLOCK_REF_NET12 =  0x0014,
	CLOCK_REF_NET13 =  0x0015,
	CLOCK_REF_NET14 =  0x0016,
	CLOCK_REF_NET15 =  0x0017,
	CLOCK_REF_NET16 =  0x0018,

	CLOCK_PRIVATE   =  0x0010,
	HI_MVIPCLK      =  0x0080,

	CLOCK_NO_MVIP90_MAPPING =  0x000F,

	SEC8K_NOT_DRIVEN                =  0x0000,
	SEC8K_DRIVEN_BY_LOCAL           =  0x0200,
	SEC8K_DRIVEN_BY_NET1            =  0x0300,
	SEC8K_DRIVEN_BY_NET2            =  0x0400,
	SEC8K_DRIVEN_BY_NET3            =  0x0500,
	SEC8K_DRIVEN_BY_NET4            =  0x0600,
	SEC8K_DRIVEN_BY_NET5            =  0x0A00,
	SEC8K_DRIVEN_BY_NET6            =  0x0B00,
	SEC8K_DRIVEN_BY_NET7            =  0x0C00,
	SEC8K_DRIVEN_BY_NET8            =  0x0D00,
	SEC8K_DRIVEN_BY_NET9            =  0x0E00,
	SEC8K_DRIVEN_BY_NET10           =  0x0F00,
	SEC8K_DRIVEN_BY_NET11           =  0x1000,
	SEC8K_DRIVEN_BY_NET12           =  0x1100,
	SEC8K_DRIVEN_BY_NET13           =  0x1200,
	SEC8K_DRIVEN_BY_NET14           =  0x1300,
	SEC8K_DRIVEN_BY_NET15           =  0x1400,
	SEC8K_DRIVEN_BY_NET16           =  0x1500,

	SW_E1_T1_PCI_TRUNK_CARD        =  0x12,
	SW_E1_T1_PCI_CARD              =  0x12,  /* Alternative name */
	SW_E1_T1_CPCI_CARD             =  0x21,
	SW_E1_T1_CPCI_PMX_CARRIER_CARD =  0x23,
	SW_PROSODY_X_PCI_CARD          =  0x24,
	SW_PROSODY_X_CPCI_CARD         =  0x25,
	SW_PROSODY_X_PCIE_CARD         =  0x26,
	SW_PROSODY_X_E1_T1_CPCI_CARD   =  0x27,
	SW_PROSODY_X_LCA_CARD          =  0x28,
	SW_PROSODY_X_PCI_R3_CARD       =  0x29,
	SW_PROSODY_X_PCIE_R3_CARD      =  0x2a,
	SW_PROSODY_X_1U_CARD           =  0x2b,  /* PX 1U Enterprise */
	SW_PROSODY_T_CARD              =  0x2c,  /* thin card architecture */

	SWMODE_CTBUS_MVIP  =     0,
	SWMODE_CTBUS_H100  =     2,
	SWMODE_CTBUS_H100_TERM = 5,
	SWMODE_CTBUS_NONE  =     6,

	kSWTrackTypeAppAPICall =  1,

	kSWDiagTrackCmdTrackingOff           =   0,
	kSWDiagTrackCmdTrackAPI              =   2,
	kSWDiagTrackCmdTrackAPIWithTimestamp =   3,
	kSWDiagTrackCmdTrackAPIAndDiag       =   4,
	kSWDiagTrackCmdResetTimestamp        =  16,

	H100_SOURCE_INTERNAL =  0x01,
	H100_SOURCE_NETWORK  =  0x03,
	H100_SOURCE_H100_A   =  0x08,
	H100_SOURCE_H100_B   =  0x09,
	H100_SOURCE_NETREF   =  0x0A,
	H100_SOURCE_NETREF_1 =  0x0A,
	H100_SOURCE_NETREF_2 =  0x0B,

	H100_SLAVE    =  0,
	H100_MASTER_A =  1,
	H100_MASTER_B =  2,

	H100_FALLBACK_DISABLED            =  0x000,
	H100_FALLBACK_ENABLED             =  0x001,
	H100_AUTO_RETURN                  =  0x010,
	H100_CHANGEOVER_TO_NETWORK        =  0x020,
	H100_CHANGEOVER_TO_NETREF         =  0x040,
	H100_CHANGEOVER_TO_NETREF_2       =  0x080,

	H100_DISABLE_NETREF  =  0,
	H100_ENABLE_NETREF   =  1,
	H100_ENABLE_NETREF_2 =  2,

	H100_NETREF_8KHZ    =  0,
	H100_NETREF_1544MHZ =  1,
	H100_NETREF_2048MHZ =  2,

	H100_CLOCK_STATUS_GOOD    =  0,
	H100_CLOCK_STATUS_BAD     =  1,
	H100_CLOCK_STATUS_UNKNOWN =  2,

	COMPANDING_DISABLED    =  0x00,
	COMPANDING_A_TO_MU_LAW =  0x02,
	COMPANDING_MU_TO_A_LAW =  0x03,
	COMPANDING_T1_IDLE     =  0x06,
	COMPANDING_E1_IDLE     =  0x07,
	COMPANDING_E1_SILENCE  =  0x08,
	COMPANDING_T1_SILENCE  =  0x09,
	COMPANDING_MIXED_MODES =  0x10,

	kSW_DSP_TYPE_SIGNALLING = 0,
	kSW_DSP_TYPE_PROSODY    = 1,
	kSW_DSP_TYPE_TONE_GEN   = 2,
	kSW_MAX_DSP_POSITION_IX = 3,

/* Defines for the event types */
	SW_EV_NO_EVENT                   =  0x00,  /* No events currently in the Q */
	SW_EV_PRIMARY_REF_NETWORK_PORT   =  0x02,  /* Signal change on the primary master's primary ref network port */
	SW_EV_SECONDARY_REF_NETWORK_PORT =  0x04,  /* Signal change on a network port driving CT_NETREF1/2 */ 
	SW_EV_CTBUS_PRIMARY_LOS          =  0x08,  /* Loss of primary clock reference on CT bus (A-clocks or B-clocks) */
/* V6.1 legacy event definitions */
	SW_EV_NETREF_PRIM                =  0x02,  /* Signal change on the Primary net port providing the clock reference */
	SW_EV_NETREF_SEC                 =  0x04,  /* Signal change on the Secondary net port providing the clock reference */
	SW_EV_CTBUS_LOS                  =  0x08,  /* LOS on the CTBUS we're clocking from */

	SW_EV_PMX_COMMAND_COMPLETE       =  0x10,  /* PMX last command issued has completed */
	SW_EV_PMX_DETAILS                =  0x11,  /* PMX new details have arrived */

	IDLE =  0x54,                              /* ccitt Idle Pattern */

	kSWMaxSerialNoText =  12,

	SW_MAXADDR                       = 512,     /* TODO: Migrate to ACU_MAXADDR when convenient */

	SW_CLOCKING_INDETERMINATE = 0,
	SW_CLOCKING_VALID = 1,
	SW_CLOCKING_INVALID = 2
};


typedef struct swver_parms {
	ACU_ULONG size;
	ACU_INT		major;
	ACU_INT		minor;
	ACU_INT		step;	
	ACU_INT		custom;	
	ACU_INT		quality;	
	ACU_INT		buildno0;	
	ACU_INT		buildno1;
} SWVER_PARMS;


typedef struct component_version_parms {
	ACU_ULONG   size;
	ACU_INT     component_id;
	ACU_INT     major;
	ACU_INT     minor;
	ACU_INT     step;
	ACU_INT     custom;
	ACU_INT     quality;
	ACU_INT     reserved0;
	ACU_INT     reserved1;
} COMPONENT_VERSION_PARMS;


typedef struct swcard_info_parms {
	ACU_ULONG size;
	ACU_INT		card_type;
	ACU_INT		card_present;	
	ACU_INT		max_capacity;	
	ACU_INT		additional_data[4];	
	ACU_ULONG	physical_address;	
	ACU_ULONG	io_address_or_pcidev;
	ACU_ULONG	physical_irq;
	char		serial_no[kSWMaxSerialNoText];
} SWCARD_INFO_PARMS;


typedef struct swmode_parms {
	ACU_ULONG size;
	ACU_INT		ct_buses;	/* Bit mask of enabled CT-buses */
} SWMODE_PARMS;


typedef struct capabilities_parms 
{
	ACU_INT   size;
	ACU_INT   revision;
	ACU_INT   domain;
	ACU_INT   routing;
	ACU_INT   blocking;
	ACU_INT   networks;
	ACU_INT   channels[8];
} CAP_PARMS;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#endif

typedef struct output_parms 
{
	ACU_ULONG size;
	ACU_INT ost;            /* Output stream number (kSW_LI_PT_TONE_GEN for BASE_TONE_MODE) */
	ACU_INT ots;            /* Output timeslot number (tone number for BASE_TONE_MODE) */
	ACU_INT mode;           /* DISABLE_MODE, PATTERN_MODE, CONNECT_MODE (etc) */
	union {
		struct {  /* CONNECT_MODE and PATTERN_MODE */
			ACU_INT ist;            /* Source stream for CONNECT_MODE */
			ACU_INT its;            /* Source timeslot for CONNECT_MODE */
			ACU_INT pattern;        /* Data value for PATTERN_MODE */
		};
		struct { /* TONE_MODE */
			ACU_INT tone_1;         /* First base tone number */
			ACU_INT tone_2;         /* Second base tone number */
			ACU_INT tone_mu_law;    /* Generate mu_law (T1) tone */
		};
		struct { /* BASE_TONE_MODE */
			ACU_INT base_tone_freq; /* In Hz */
			ACU_INT base_tone_ampl; /* In dbm*(-10) */
		};
		struct { /* LAYER_1_LOOP_MODE */
			ACU_INT l1_loop_type;   /* Type of loop: LAYER_1_LOOP_xxx */
			ACU_INT l1_loop_opts;   /* Type specific options */
			ACU_INT l1_loop_secs;   /* Loop duration */
		};
		struct { /* FFT_RESULT_MODE */
			/* Set ost to kSW_LI_PT_FFT_RESULT + (port & 15) */
			ACU_INT fft_freq_1;     /* First frequency (Hz) */
			ACU_INT fft_freq_2;     /* Second frequency (Hz) */
			ACU_INT fft_freq_3;     /* Third frequency (usually an error) */
		};
	};
} OUTPUT_PARMS;

#ifdef _WIN32
#pragma warning(pop)
#endif

typedef struct sample_parms 
{
	ACU_ULONG size;
	ACU_INT  ist;
	ACU_INT  its;
	char sample;
} SAMPLE_PARMS;


/*
 * Aculab use only.
 *
 * On T810X type boards, normally
 *  Stream is AMR reg
 *  Slot   is LAR reg
 *  data is read value
 *  unless stream is set to 1 of following values where upon 
 *  slot is CAM location
 *  data = its + (ist<<8)
 *  cmhi = controlBits + ((valid) ? (1<<8) : 0)
 *  cmlo = tagOrPattern
 */
#define kDumpSwitchStreamForLocalCAM		256
#define kDumpSwitchStreamForEvenCAM			257
#define kDumpSwitchStreamForOddCAM			258
#define kDumpSwitchStreamForTagTableDump	259

typedef struct dump_parms 
{
	ACU_INT size;
	ACU_INT minor;
	ACU_INT stream;
	ACU_INT slot;
	ACU_INT cmhi;
	ACU_INT cmlo;
	ACU_INT data;
} DUMP_PARMS;


typedef struct qclock_parms 
{
	ACU_INT scclk_driven;
	ACU_INT driving_scclk;
} QCLOCK_PARMS;


typedef struct query_clkmode_parms 
{
	ACU_ULONG size;
	ACU_INT last_clock_mode;
	ACU_INT sysinit_clock_mode;
} QUERY_CLKMODE_PARMS;


typedef struct trace_parms 
{
	ACU_ULONG size;
	ACU_INT code;
} TRACE_PARMS;


typedef struct track_parms 
{
	ACU_ULONG size;
	ACU_INT				tracking_on;
	ACU_INT				is_tracked_data;
	ACU_INT				swdrvr;
	ACU_INT				tracked_type;
	ACU_INT				index;
	ACU_INT				words[8];
	ACU_ULONG			timestamp;
} TRACK_PARMS;


/*
 * Backwards compatibility.
 */
typedef struct track_313_parms 
{
	ACU_INT				tracking_on;
	ACU_INT				is_tracked_data;
	ACU_INT				swdrvr;
	ACU_INT				tracked_type;
	ACU_INT				index;
	ACU_INT				words[8];
	ACU_ULONG			timestamp;
} TRACK_313_PARMS;


typedef struct h100_config_board_clock_parms
{
	ACU_ULONG size;
	ACU_INT clock_source; 	 		/* internal, net, H100_A, H100_B, H100_NETREF */ 
	ACU_INT network;		 		/* 1..4 - MVIP-95 uses base of 1 not 0 */
	ACU_INT h100_clock_mode;	 	/* slave, A-master, B-Master */
	ACU_INT auto_fall_back;	 		/* enable/disable */
	ACU_INT netref_clock_speed; 	/* 8KHz, 1544MHz, 2048MHz */	
} H100_CONFIG_BOARD_CLOCK_PARMS;


typedef struct h100_netref_clock_parms
{
	ACU_ULONG size;
	ACU_INT network;		 			/* 1..4 - MVIP-95 uses base of 1 not 0 */
	ACU_INT netref_clock_mode;	 		/* none, generate_netref  */
	ACU_INT netref_clock_speed; 		/* 8KHz, 1544MHz, 2048MHz */
} H100_NETREF_CLOCK_PARMS;


typedef struct h100_query_board_clock_parms
{
	ACU_ULONG size;
	ACU_INT clock_source;	 			/* internal, network, H100_A, H100_B, NETREF */
	ACU_INT network;					/* 1..4 */
	ACU_INT h100_clock_mode;			/* slave, A-master, B-master */
	ACU_INT auto_fall_back;			/* enabled/disabled */
	ACU_INT fall_back_occurred;		/* yes/no */
	ACU_INT h100_a_clock_status; 		/* good/bad/unknown */
	ACU_INT h100_b_clock_status; 		/* good/bad/unknown */
	ACU_INT netref_a_clock_status; 	/* good/bad/unknown */
	ACU_INT netref_b_clock_status; 	/* good/bad/unknown - for H.110 */
} H100_QUERY_BOARD_CLOCK_PARMS;


typedef struct timeslot_companding_parms
{
	ACU_INT   size;
	ACU_INT   stream;
	ACU_INT   timeslot;
	ACU_INT   rx_mode;
	ACU_INT   tx_mode;
} TIMESLOT_COMPANDING_PARMS;


typedef struct companding_parms
{
	ACU_INT   size;
	ACU_INT   stream;
	ACU_INT   rx_mode;
	ACU_INT   tx_mode;
} COMPANDING_PARMS;


typedef struct dsp_stream_info_parms
{
	ACU_ULONG size;
	ACU_INT   dsp_type;
	ACU_INT   dsp_position_ix;
	ACU_INT   dsp_serial_port_ix;
	ACU_INT   switching_stream;
	ACU_INT   no_ts_in_stream;
} DSP_STREAM_INFO_PARMS;


typedef struct prosody_s_stream_info_parms
{
	ACU_ULONG size;
	ACU_INT   serial_port_ix;
	ACU_INT   switching_stream;
	ACU_INT   no_ts_in_stream;
} PROSODY_S_STREAM_INFO_PARMS;


typedef struct register_event_parms {
	ACU_ULONG size;
	ACU_INT		enable_notification;
	ACU_WAIT_OBJECT	event_id;
} REGISTER_EVENT_PARMS;


/* Switch event structures */
typedef struct sw_set_notify_event_parms
{
	ACU_ULONG size;
	ACU_ACT		app_token;	/* The event to register */
} SW_SET_NOTIFY_EVENT_PARMS;


typedef struct sw_card_notification_parms
{
	ACU_ULONG size;
	int		event;		/* OUT: The event type that occurred - one of the defines above */
} SW_CARD_NOTIFICATION_PARMS;


typedef struct sw_wait_object_parms
{
	ACU_ULONG size;
	ACU_WAIT_OBJECT		wait_object;	/* OUT: App specific wait object */
} SW_WAIT_OBJECT_PARMS;


/* Structure for allowing us to trace API calls: */
typedef struct sw_track_api_parms
{
	ACU_ULONG	size;
	int			tracking_on;
	int			count;			/* IN: No. of elements in cards */
	ACU_CARD_ID	*cards;
	ACU_CHAR	remotehost[SW_MAXADDR];/* remote host to send trace to */
} SW_TRACK_API_PARMS;


typedef struct sw_clocking_valid_parms
{
	ACU_ULONG		size;
	ACU_INT			validity;
} SW_CLOCKING_VALID_PARMS;


/* open the switch driver for use with a card */
typedef struct tACU_OPEN_SWITCH_PARMS
{
	ACU_ULONG		size;
	ACU_CARD_ID		card_id;	/* IN */
} ACU_PACK_DIRECTIVE ACU_OPEN_SWITCH_PARMS;


/* close the switch driver for use with a card */
typedef struct tACU_CLOSE_SWITCH_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id; /* IN */
} ACU_PACK_DIRECTIVE ACU_CLOSE_SWITCH_PARMS;


/*--------- Multiple Driver Functions ---------*/
/*---------------------------------------------*/

#ifndef DRIVER

int ACU_EXPORT sw_ver_switch(ACU_CARD_ID, SWVER_PARMS*);
int ACU_EXPORT sw_card_info(ACU_CARD_ID, SWCARD_INFO_PARMS*);
int ACU_EXPORT sw_reset_switch(ACU_CARD_ID);
int ACU_EXPORT sw_query_switch_caps(ACU_CARD_ID, CAP_PARMS *);
int ACU_EXPORT sw_set_output(ACU_CARD_ID, OUTPUT_PARMS *);
int ACU_EXPORT sw_query_output(ACU_CARD_ID, OUTPUT_PARMS *);
int ACU_EXPORT sw_sample_input(ACU_CARD_ID, SAMPLE_PARMS *);
int ACU_EXPORT sw_sample_input0(ACU_CARD_ID, SAMPLE_PARMS *);
int ACU_EXPORT sw_clock_control(ACU_CARD_ID, int);
int ACU_EXPORT sw_query_clock_control(ACU_CARD_ID, QUERY_CLKMODE_PARMS *);
int ACU_EXPORT sw_dump_switch(ACU_CARD_ID, DUMP_PARMS *);
int ACU_EXPORT sw_track_switch(ACU_CARD_ID, TRACK_PARMS *);
int ACU_EXPORT sw_tristate_switch(ACU_CARD_ID, int);

int ACU_EXPORT sw_switch_override_mode(ACU_CARD_ID, int);
int ACU_EXPORT sw_mode_switch(ACU_CARD_ID, SWMODE_PARMS *);

int ACU_EXPORT sw_config_timeslot_companding(ACU_CARD_ID, TIMESLOT_COMPANDING_PARMS *);
int ACU_EXPORT sw_query_timeslot_companding(ACU_CARD_ID, TIMESLOT_COMPANDING_PARMS *);
int ACU_EXPORT sw_config_companding(ACU_CARD_ID, COMPANDING_PARMS *);
int ACU_EXPORT sw_query_companding(ACU_CARD_ID, COMPANDING_PARMS *);
int ACU_EXPORT sw_get_dsp_stream_info(ACU_CARD_ID, DSP_STREAM_INFO_PARMS *);
int ACU_EXPORT sw_get_prosody_s_stream_info(ACU_CARD_ID, PROSODY_S_STREAM_INFO_PARMS *);
int ACU_EXPORT sw_set_card_h100_termination(ACU_CARD_ID, int);
int ACU_EXPORT sw_component_version(ACU_CARD_ID, COMPONENT_VERSION_PARMS*);

/*--------- H.100 switch functions ------------*/
/*---------------------------------------------*/

int ACU_EXPORT sw_h100_config_board_clock(ACU_CARD_ID, H100_CONFIG_BOARD_CLOCK_PARMS*);
int ACU_EXPORT sw_h100_config_netref_clock(ACU_CARD_ID, H100_NETREF_CLOCK_PARMS*);
int ACU_EXPORT sw_h100_query_board_clock(ACU_CARD_ID, H100_QUERY_BOARD_CLOCK_PARMS*);
int ACU_EXPORT sw_h100_query_netref_clock(ACU_CARD_ID, H100_NETREF_CLOCK_PARMS*);

/*---------- Switch event functions -----------*/
/*---------------------------------------------*/

ACU_INT ACU_EXPORT sw_get_card_notification	( ACU_CARD_ID, SW_CARD_NOTIFICATION_PARMS * );

/** Associate switch instance with an event queue */
ACU_ERR ACU_EXPORT sw_set_card_notification_queue(ACU_QUEUE_PARMS* queue_parms);

/* Return to the calling app, the notification event handle */
ACU_INT ACU_EXPORT sw_get_notification_wait_object( ACU_CARD_ID, SW_WAIT_OBJECT_PARMS * );

/* initialise the switch API for a card */
ACU_ERR ACU_EXPORT acu_open_switch(ACU_OPEN_SWITCH_PARMS* openp);

/* shut down the switch API for a card */
ACU_ERR ACU_EXPORT acu_close_switch(ACU_CLOSE_SWITCH_PARMS* closep);

/* Prosody X only: abort all active api calls */
int ACU_EXPORT sw_abort_api_calls(ACU_CARD_ID);

void ACU_EXPORT sw_crack_result(int, char*);
void ACU_EXPORT sw_track_api_calls( SW_TRACK_API_PARMS *track_api );

#define HAVE_SW_CHECK_CLOCKING_VALIDITY
ACU_INT ACU_EXPORT sw_check_clocking_validity(ACU_CARD_ID card_id, SW_CLOCKING_VALID_PARMS *valid_parms);

#endif

#ifdef _WIN32
#pragma pack(pop)
#endif


#ifdef __cplusplus
}
#endif

#endif

