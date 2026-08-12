/*****************************************************************************\
*
* RES_LIB.H : V6 Resource Management API Declarations
*
* (C)2002-2003 Aculab Plc
*
*
\*****************************************************************************/
#ifndef RES_TYPE_H_INCLUDED
#define RES_TYPE_H_INCLUDED

#include "acu_type.h"

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#ifdef __cplusplus
extern "C" {

#endif

/* constants */
enum eACU_RESOURCE_CONSTANTS
{
	MAX_SNAPSHOT_CARDS			= 20,
	MAX_RESOURCE_SERIAL			= 30,
	MAX_HWMON_DESC				= 30,
	ACU_MAX_RESOURCE_NAME		= 100,
	ACU_MAX_CARD_NAME			= 20,
	ACU_MAX_CARD_DESC			= 40,
	ACU_MAX_CARD_KEY			= 60,
	ACU_MAX_FWVER				= 60,
	ACU_MAX_RES_VERSION			= 30,
	ACU_MAX_CARD_STATUS			= 40,
	ACU_MAX_CARD_DIAGNOSTIC		= 512,
	ACU_MAX_CFG_ENTRY_SIZE		= 100,
	ACU_MAX_IPV6_ADDRESSES		= 10,
	ACU_MAX_TRACE_ID			= 20,


	/* functionality available on card */
	ACU_RESOURCE_CALL           = 0x01,
	ACU_RESOURCE_SWITCH         = 0x02,
	ACU_RESOURCE_SPEECH         = 0x04,
	ACU_RESOURCE_IP_TELEPHONY   = 0x08,
/*	ACU_RESOURCE_ODPR           = 0x10, */ /* Was never used */
	ACU_RESOURCE_TRM            = 0x20,
	ACU_RESOURCE_MG             = 0x40,
	ACU_RESOURCE_STUN           = 0x80,
	ACU_RESOURCE_SOFT_DSP       = 0x100,
	ACU_RESOURCE_HPI            = 0x200,

	/* types of card */
	ACU_PROSODY_PCI_CARD			= 0x10,
	ACU_E1_T1_PCI_CARD				= 0x12,
	ACU_VOIP_PCI_CARD				= 0x14,
	ACU_IP_TELEPHONY_PCI_CARD		= 0x14,
	ACU_PROSODY_CPCI_CARD			= 0x20,
	ACU_E1_T1_CPCI_CARD				= 0x21,
	ACU_PROSODY_S_CARD				= 0x22,
	ACU_E1_T1_CPCI_PMX_CARRIER_CARD	= 0x23,
	ACU_PROSODY_X_CARD              = 0x24,
	ACU_VIRTUAL_BEARER_CARD			= 0x25,
	ACU_PROSODY_S_V3_CARD			= 0x26,

	/* types of event notification */
	ACU_RES_SYSTEM_NOTIFICATION		= 1,
	ACU_RES_CARD_NOTIFICATION		= 2,
	ACU_CALL_EVENT					= 3,
	ACU_CALL_PORT_NOTIFICATION		= 4,
	ACU_SWITCH_NOTIFICATION			= 5,
	ACU_CALL_CARD_NOTIFICATION		= 6,
	ACU_CALL_GLOBAL_NOTIFICATION	= 7,
	ACU_IPT_CARD_NOTIFICATION		 = 8,
	ACU_MG_CONTEXT_NOTIFICATION		= 9,
	ACU_MG_TERMINATION_NOTIFICATION		= 10,
	ACU_XBUS_NOTIFICATION			= 11,	
	ACU_RMSM_NOTIFICATION                   = 12,
	ACU_STUN_CARD_NOTIFICATION		= 13,
	ACU_STUN_BINDING_NOTIFICATION	= 14,

	/* types of card event */
	ACU_CARD_EVT_NO_EVENT				= 0,
	ACU_CARD_EVT_CARD_REMOVE_PENDING	= 1,
	ACU_CARD_EVT_CARD_REMOVE_COMPLETE	= 2,
	ACU_CARD_EVT_CARD_SURPRISE_REMOVE	= 3,

	/* types of system event */
	ACU_SYS_EVT_NO_EVENT				= 0,	/**< no event occurred */
	ACU_SYS_EVT_CARD_ADDED				= 1,	/**< new card found in system */
	ACU_SYS_EVT_CARD_REMOVED			= 2,	/**< card removed from system */
	ACU_SYS_EVT_RESMGR_RUNNING			= 3,	/**< Resource Manager has started up */
	ACU_SYS_EVT_RESMGR_STOPPED			= 4,	/**< Resource Manager has stopped */
	ACU_SYS_EVT_PROSODY_IP_CARD_REGISTERED		= 5,	/**< A Prosody IP card has been registered */
	ACU_SYS_EVT_PROSODY_IP_CARD_UNREGISTERED	= 6,	/**< A Prosody IP card has been unregistered */
	ACU_SYS_EVT_PROSODY_IP_CARD_IP_CONFLICT		= 7,	/**< An IP address conflict has been detected */
	ACU_SYS_EVT_PROSODY_IP_CARD_CAPS		= 8,	/**< A Prosody IP card caps request has been answered */
	ACU_SYS_EVT_PROSODY_CTRL_CARD_CAPS			= 9,	/**< A Prosody IP control card caps request has been answered */

	/* constants for acu_identify_card() */
	ACU_IDENTIFY_CARD_ON				= 1,
	ACU_IDENTIFY_CARD_OFF				= 0,

	/* types of card location information */
	ACU_POSITION_TYPE_PCI				= 0,
	ACU_POSITION_TYPE_GEOGRAPHIC		= 1, /* proposed value, not currently used! */

	/* restart methods */
	ACU_RESTART_METHOD_FORCE			= 1,
	ACU_RESTART_METHOD_REMOTE                       = 2,
	ACU_RESTART_METHOD_GRACEFUL                     = 3,

	/* Configuration tool status codes.  Diagnostic information is written to the system log */
	ACU_CONFIG_SUCCEEDED							= 0,	/**< Configuration successfully applied */
	ACU_CONFIG_INVALID_SERIAL						= 1,	/**< Configuration tool couldn't find card with the specified serial number */
	ACU_CONFIG_SWITCH_RESTART_FAILED				= 2,	/**< Switch driver needs restarting, restart failed */
	ACU_CONFIG_SWITCH_CONFIG_FAILED					= 3,	/**< Configuration tool failed applying switch configuration */
	ACU_CONFIG_PORT_CONFIG_FAILED					= 4,	/**< Configuration tool failed applying port configuration */
	ACU_CONFIG_PROSODY_CONFIG_FAILED				= 5,	/**< Prosody configuration failed to be applied */
	ACU_CONFIG_GENERAL_CONFIG_FAILED				= 6,	/**< Configuration tool failed applying general configuration */
	ACU_CONFIG_IP_CONFIG_FAILED						= 7,	/**< Configuration tool failed applying IP configuration */
	ACU_CONFIG_INVALID_COMMAND_LINE					= 8,	/**< Invalid command line option supplied */
	ACU_CONFIG_ERROR								= 9,	/**< Configuration tool encountered an error */
	ACU_CONFIG_RESTART_REQUIRED						= 10,	/**< Card restart required */
	ACU_CONFIG_NO_CFG_FILE							= 11,	/**< No configuration file for card */
	ACU_CONFIG_CLOCKING_CONFIG_FAILED				= 12,	/**< No configuration file for card */
	ACU_CONFIG_APPLYING_CONFIGURATION				= 100,	/**< Configuration currently being applied */
	ACU_CONFIG_TIMED_OUT							= 101,	/**< Configuration tool took too long and was terminated*/
	ACU_CONFIG_FAILED_TO_RUN						= 102,	/**< Configuration tool failed to run */
	ACU_CONFIG_TERMINATED_UNEXPECTEDLY				= 103,	/**< Configuration tool terminated unexpectedly */
	ACU_CONFIG_H323_CONFIG_FAILED					= 104,   /**< Configuration too failed applying H.323 on board configuration */

	/* Card types */
	ACU_CARD_PROSODY_X				= 1,
	ACU_CARD_PROSODY_S				= 2,
	ACU_CARD_PROSODY_T				= 3,

	/* These values should match the values in mam.h */
	ACU_CARD_ENDPOINT_BOARD			= 1,
	ACU_CARD_ENDPOINT_PMX			= 3,
	ACU_CARD_ENDPOINT_DSP			= 4,

	ACU_CARD_ENDPOINT_MEDIA			= ACU_CARD_ENDPOINT_DSP,

	ACU_CARD_MAX_DSPS				= 8,

	ACU_PROSODY_IP_CARD_CAPS_TDM		= 0x01,
	ACU_PROSODY_IP_CARD_CAPS_DSP		= 0x02,
	ACU_PROSODY_IP_CARD_CAPS_IPV4		= 0x04,
	ACU_PROSODY_IP_CARD_CAPS_IPV6		= 0x08,
	ACU_PROSODY_IP_CARD_CAPS_MEDIA		= 0x10,
	ACU_PROSODY_IP_CARD_CAPS_DHCP6		= 0x20,
	ACU_PROSODY_IP_CARD_CAPS_SOFT_DSP	= 0x40,
	ACU_PROSODY_IP_CARD_CAPS_HPI		= 0x80,
	ACU_PROSODY_IP_CARD_CAPS_SNMP		= 0x100,

	ACU_PROSODY_X_UNKNOWN				= 0,
	ACU_PROSODY_X_V1					= 1,
	ACU_PROSODY_X_V3					= 2,
	ACU_PROSODY_X_THIN_V1				= 3,

	ACU_PROSODY_SERIAL_FROM_HOSTNAME	= 1,
	ACU_PROSODY_SERIAL_FROM_API			= 2,

	ACU_MAX_HWMON_VALS					= 20,
	ACU_RESOURCE_HWMON_TEMP				= 1,
	ACU_RESOURCE_HWMON_VOLT				= 2,
	ACU_RESOURCE_HWMON_FAN				= 3,
	ACU_RESOURCE_HWMON_SYSTEM			= 256
};

typedef struct tACU_CARD_POSITION_PCI
{
	ACU_UINT	bus;		/**< PCI bus */
	ACU_UINT	device;		/**< PCI device */
	ACU_UINT	function;	/**< PCI function */
} ACU_PACK_DIRECTIVE ACU_CARD_POSITION_PCI;

/* note: this structure is for future use! */
typedef struct tACU_CARD_POSITION_GEOGRAPHIC
{
	ACU_UINT slot_id;		/**< proposed structure, not currently used! */
	ACU_UINT shelf_id;		/**< proposed structure, not currently used! */
	ACU_UINT pos_id;		/**< proposed structure, not currently used! */
	ACU_UINT reserved_1;	/**< proposed structure, not currently used! */
	ACU_UINT reserved_2;	/**< proposed structure, not currently used! */
	ACU_UINT reserved_3;	/**< proposed structure, not currently used! */
} ACU_PACK_DIRECTIVE ACU_CARD_POSITION_GEOGRAPHIC;

typedef union tACU_CARD_POSITION_UNION
{
	ACU_CARD_POSITION_PCI pci;
	ACU_CARD_POSITION_GEOGRAPHIC geographic;
} ACU_PACK_DIRECTIVE ACU_CARD_POSITION_UNION;


/* hardware position */
typedef struct tACU_CARD_POSITION
{
	ACU_UINT pos_type;
	ACU_CARD_POSITION_UNION pos;
} ACU_PACK_DIRECTIVE ACU_CARD_POSITION;

typedef struct tACU_ENABLE_API_TRACE_PARMS
{
	ACU_ULONG	size;
	ACU_UINT	enable;
} ACU_PACK_DIRECTIVE ACU_ENABLE_API_TRACE_PARMS;

/* function parameter blocks */
typedef struct tACU_SNAPSHOT_PARMS
{
	ACU_ULONG	size;
	ACU_UINT	count;
	ACU_CHAR	serial_no[MAX_SNAPSHOT_CARDS][MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_SNAPSHOT_PARMS;

typedef struct tACU_CARD_INFO_PARMS
{
	ACU_ULONG			size;
	ACU_CARD_ID			card_id;						/* IN */
	ACU_UINT			card_type;						/* OUT */
	ACU_UINT			resources_available;			/* OUT */
	ACU_CHAR			serial_no[MAX_RESOURCE_SERIAL];	/* OUT */
	ACU_CHAR			hw_version[ACU_MAX_HWVER];		/* OUT */
	ACU_ACT				app_context_token;				/* OUT */
	ACU_CARD_POSITION	hw_position;					/* OUT */
	ACU_CHAR			card_name[ACU_MAX_CARD_NAME];	/* OUT */
	ACU_CHAR			card_desc[ACU_MAX_CARD_DESC];	/* OUT */
	ACU_CHAR			ip_address[ACU_MAX_IP_ADDRESS];	/* OUT */
	ACU_CHAR			card_key[ACU_MAX_CARD_KEY];     /* OUT */
	ACU_UINT			ip_port;						/* OUT */
	ACU_UINT			reserved;
	ACU_UINT			px_card_type;					/* OUT */
} ACU_PACK_DIRECTIVE ACU_CARD_INFO_PARMS;

typedef struct tACU_OPEN_CARD_PARMS
{
	ACU_ULONG		size;
	ACU_CHAR		serial_no[ACU_MAX_RESOURCE_NAME];	/* IN/OUT */
	ACU_CARD_ID		card_id;						/* OUT */
	ACU_EVENT_QUEUE	notification_queue;				/* IN */
	ACU_ACT			app_context_token;				/* IN */
} ACU_PACK_DIRECTIVE ACU_OPEN_CARD_PARMS;

typedef struct tACU_CLOSE_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id; /* OUT */
} ACU_PACK_DIRECTIVE ACU_CLOSE_CARD_PARMS;

typedef struct tACU_CARD_INSERT_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_CARD_INSERT_NOTIFICATION;

typedef struct tACU_CARD_MEDIA_DEFAULTS_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_CARD_MEDIA_DEFAULTS_NOTIFICATION;

typedef struct tACU_CARD_REMOVE_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_CARD_REMOVE_NOTIFICATION;

typedef struct tACU_CARD_EJECT_PENDING_NOTIFICATION
{
	ACU_CARD_ID card_id;
} ACU_PACK_DIRECTIVE ACU_CARD_EJECT_PENDING_NOTIFICATION;

typedef struct tACU_CARD_EJECT_NOTIFICATION
{
	ACU_UINT surprise_removal;
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_CARD_EJECT_NOTIFICATION;

typedef struct tACU_PROSODY_IP_CARD_CARD_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR ip_address[ACU_MAX_IP_ADDRESS];
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_CARD_NOTIFICATION;

typedef struct tACU_PROSODY_IP_CARD_IP_CONFLICT_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR mac_address[ACU_MAX_IP_ADDRESS];
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_CARD_IP_CONFLICT_NOTIFICATION;

typedef struct tACU_PROSODY_IP_CARD_CAPS_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR model[ACU_MAX_HWVER];
	ACU_CHAR ipv4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR ipv6_address[ACU_MAX_IP_ADDRESS];
	ACU_UINT card_caps;
	ACU_UINT reserved;
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_CARD_CAPS_NOTIFICATION;

typedef struct tACU_PROSODY_CTRL_CARD_CAPS_NOTIFICATION
{
	ACU_CHAR serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR model[ACU_MAX_HWVER];
	ACU_CHAR ipv4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR ipv6_address[ACU_MAX_IP_ADDRESS];
	ACU_UINT card_caps;
	ACU_UINT reserved;
	ACU_CHAR key[ACU_MAX_CARD_KEY];
} ACU_PACK_DIRECTIVE ACU_PROSODY_CTRL_CARD_CAPS_NOTIFICATION;

typedef struct tACU_SYSTEM_NOTIFICATION_PARMS
{
	ACU_ULONG	size;	/* IN */
	ACU_UINT	event;	/* OUT */
	union
	{
		ACU_CARD_INSERT_NOTIFICATION     insert_data;
		ACU_CARD_MEDIA_DEFAULTS_NOTIFICATION  media_defaults_data;
		ACU_CARD_REMOVE_NOTIFICATION     remove_data;
		ACU_PROSODY_IP_CARD_NOTIFICATION prosody_ip_data;
		ACU_PROSODY_IP_CARD_IP_CONFLICT_NOTIFICATION prosody_ip_conflict_data;
		ACU_PROSODY_IP_CARD_CAPS_NOTIFICATION prosody_ip_card_caps;
		ACU_PROSODY_CTRL_CARD_CAPS_NOTIFICATION prosody_ctrl_card_caps;
	} data;
	
} ACU_PACK_DIRECTIVE ACU_SYSTEM_NOTIFICATION_PARMS;

typedef struct tACU_CARD_NOTIFICATION_PARMS
{
	ACU_ULONG size;			/* IN */
	ACU_CARD_ID card_id;	/* IN/OUT */
	ACU_UINT event;			/* OUT */
} ACU_PACK_DIRECTIVE ACU_CARD_NOTIFICATION_PARMS;

typedef struct tACU_GET_SYS_WAIT_OBJECT_PARMS
{
	ACU_ULONG		size;			/* IN */
	ACU_CARD_ID		card_id;		/* IN */
	ACU_WAIT_OBJECT wait_object;	/* OUT */
} ACU_PACK_DIRECTIVE ACU_GET_SYS_WAIT_OBJECT_PARMS;

typedef struct tACU_GET_CARD_WAIT_OBJECT_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID		card_id;		/* IN */
	ACU_WAIT_OBJECT wait_object;	/* OUT */
} ACU_PACK_DIRECTIVE ACU_GET_CARD_WAIT_OBJECT_PARMS;

typedef struct tACU_EJECT_CARD_PARMS
{
	ACU_ULONG	size;		/* IN */
	ACU_CARD_ID card_id;	/* IN */
} ACU_PACK_DIRECTIVE ACU_EJECT_CARD_PARMS;

typedef struct tACU_IDENTIFY_PARMS
{
	ACU_ULONG		size;			/* IN */
	ACU_CARD_ID		card_id;		/* IN */
	ACU_CHAR		serial_no[ACU_MAX_RESOURCE_NAME]; /* IN */
	ACU_UINT		identify_on;	/* IN */
} ACU_PACK_DIRECTIVE ACU_IDENTIFY_PARMS;

typedef struct tACU_EVENT_QUEUE_PARMS
{
	ACU_ULONG		size;				/* IN */
	ACU_EVENT_QUEUE queue_id;			/* IN */
	ACU_LONG		timeout;			/* IN */
	ACU_UINT		event_type;			/* OUT */
	ACU_ACT			context;			/* OUT */
#if 0
#ifndef ACU_SOLARIS_SPARC
	ACU_UINT		do_not_use_64_bit_pad;	/* DO NOT USE */
#endif /* ACU_SOLARIS_SPARC */
#endif
	ACU_RESOURCE_ID	resource_id;		/* OUT */
} ACU_PACK_DIRECTIVE ACU_EVENT_QUEUE_PARMS;

typedef struct tACU_FREE_EVENT_QUEUE_PARMS
{
	ACU_ULONG		size;		/* IN */
	ACU_EVENT_QUEUE	queue_id;	/* IN */
} ACU_PACK_DIRECTIVE ACU_FREE_EVENT_QUEUE_PARMS;

typedef struct tACU_ALLOC_EVENT_QUEUE_PARMS
{
	ACU_ULONG		size;		/* IN */
	ACU_EVENT_QUEUE	queue_id;	/* OUT */
} ACU_PACK_DIRECTIVE ACU_ALLOC_EVENT_QUEUE_PARMS;

typedef struct tACU_QUEUE_WAIT_OBJECT_PARMS
{
	ACU_ULONG		size;			/* IN */
	ACU_EVENT_QUEUE queue_id;		/* IN */
	ACU_WAIT_OBJECT wait_object;	/* OUT */
} ACU_PACK_DIRECTIVE ACU_QUEUE_WAIT_OBJECT_PARMS;

typedef struct tACU_SET_CARD_NAME_PARMS
{
	ACU_ULONG	size;							/* IN */
	ACU_CARD_ID card_id;						/* IN */
	ACU_CHAR	card_name[ACU_MAX_CARD_NAME];	/* IN */
} ACU_PACK_DIRECTIVE ACU_SET_CARD_NAME_PARMS;

typedef struct tACU_RESOURCE_VERSION_PARMS
{
	ACU_UINT size;						/* IN */
	ACU_UINT major;						/* OUT */
	ACU_UINT minor;						/* OUT */
	ACU_UINT rev;						/* OUT */
	ACU_CHAR desc[ACU_MAX_RES_VERSION];	/* OUT */
} ACU_RESOURCE_VERSION_PARMS;

typedef struct tACU_REGISTER_PROSODY_IP_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_INT         persistent;
	ACU_INT         configure;
	ACU_CHAR        ip4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_gateway[ACU_MAX_IP_ADDRESS];
	ACU_INT         use_dhcp;
	ACU_CHAR        key[ACU_MAX_CARD_KEY];
	ACU_INT         watchdog_timeout;
	ACU_CHAR		ip6_prefix[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_gateway[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_base_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_pmx_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_dsp_addrs[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_UINT		ip6_prefixlen;
	ACU_UINT		auto_ip6;			/* 1 for radvd */
	ACU_UINT		ip6_privacy;		/* Not yet implemented */
	ACU_UINT		host_card;			/* 1 for local host card */
	/* Added in release 6.8.0 */
	ACU_CHAR		ip6_media_gateways[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT		media_use_dhcp;
	ACU_UINT		use_media_device;
	ACU_UINT		ip6_base_use_dhcp;
	ACU_UINT		ip6_media_use_dhcp[ACU_CARD_MAX_DSPS];
} ACU_PACK_DIRECTIVE ACU_REGISTER_PROSODY_IP_CARD_PARMS;

typedef struct tACU_PROSODY_IP_CARD_REGISTRATION_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	card[ACU_MAX_RESOURCE_NAME];
	ACU_INT         use_remote;
	ACU_INT         persistent;
	ACU_INT         configure;
	ACU_CHAR        ip4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_gateway[ACU_MAX_IP_ADDRESS];
	ACU_INT         use_dhcp;
	ACU_CHAR        key[ACU_MAX_CARD_KEY];
	ACU_INT         watchdog_timeout;
	ACU_UINT		auto_generated_key;
	ACU_CHAR		ip6_prefix[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_gateway[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_base_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_pmx_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_dsp_addrs[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_UINT		ip6_prefixlen;
	ACU_UINT		auto_ip6;			/* 1 for radvd */
	ACU_CHAR		controlling_address[ACU_MAX_IP_ADDRESS]; /* OUT: address which resmgr
										  * is using to control the card */
	ACU_UINT		ip6_privacy;		/* Not yet implemented */
	ACU_UINT		host_card;			/* 1 for local host card */
	/* Added in release 6.8.0 */
	ACU_CHAR		ip6_media_gateways[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip4_media_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT		media_use_dhcp;
	ACU_UINT		use_media_device;
	ACU_UINT		ip6_base_use_dhcp;
	ACU_UINT		ip6_media_use_dhcp[ACU_CARD_MAX_DSPS];
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_CARD_REGISTRATION_PARMS;

typedef struct tACU_UNREGISTER_PROSODY_IP_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[ACU_MAX_RESOURCE_NAME];
} ACU_PACK_DIRECTIVE ACU_UNREGISTER_PROSODY_IP_CARD_PARMS;

typedef struct tACU_PROSODY_IP_DEVICE_INFO_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id;
	ACU_INT         device;
	ACU_CHAR        model[ACU_MAX_HWVER];
	ACU_CHAR        serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR        hw_version[ACU_MAX_HWVER];
	ACU_CHAR        description[ACU_MAX_CARD_DESC];
	ACU_CHAR        bootloader_version[ACU_MAX_FWVER];
	ACU_CHAR        firmware_version[ACU_MAX_FWVER];
	ACU_ULONG       temp0;
	ACU_ULONG       temp1;
	ACU_ULONG		voltage_12_0;
	ACU_ULONG       voltage_5_0;
	ACU_ULONG       voltage_3_3;
	ACU_SHORT       control_port;
	ACU_CHAR        module_model[ACU_MAX_HWVER];
	ACU_CHAR        module_hwver[ACU_MAX_HWVER];
	ACU_UINT		fpga_version;
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_DEVICE_INFO_PARMS;

typedef struct tACU_PROSODY_IP_CARD_RESTART_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	card[ACU_MAX_RESOURCE_NAME];
	ACU_INT		method;
} ACU_PROSODY_IP_CARD_RESTART_PARMS;

typedef struct tACU_PROSODY_IP_CARD_STATUS_PARMS
{
	ACU_ULONG		size;
	ACU_CHAR		card[ACU_MAX_RESOURCE_NAME];
	ACU_CHAR		status[ACU_MAX_CARD_STATUS];
	ACU_CHAR		diagnostic[ACU_MAX_CARD_DIAGNOSTIC];
} ACU_PROSODY_IP_CARD_STATUS_PARMS;

typedef struct tACU_PROSODY_IP_LOCAL_CARD_INFO_PARMS
{
	ACU_ULONG		size;
	ACU_CHAR		card[ACU_MAX_RESOURCE_NAME];
	ACU_CHAR		local_ethernet_device[ACU_MAX_RESOURCE_NAME];
	ACU_CHAR		local_ethernet_os_specific_info[ACU_MAX_RESOURCE_NAME];
} ACU_PROSODY_IP_LOCAL_CARD_INFO_PARMS;

typedef struct tACU_PROSODY_IP_ENDPOINT_ADDRESS_PARMS
{
	ACU_ULONG			size;
	ACU_UINT			card_id;							/* IN */
	ACU_UINT			device;								/* IN */
	ACU_UINT			index;								/* IN */
	ACU_UINT			reserved;
	ACU_UINT			ip6_count;							/* OUT */
	ACU_CHAR			ip4_address[ACU_MAX_IP_ADDRESS];	/* OUT */
	ACU_CHAR			ip6_address[ACU_MAX_IPV6_ADDRESSES][ACU_MAX_IP_ADDRESS];	/* OUT */
	ACU_UINT			min_port;							/* OUT */
	ACU_UINT			max_port;							/* OUT */
	ACU_UINT			sin6_scope_id;						/* OUT */
} ACU_PROSODY_IP_ENDPOINT_ADDRESS_PARMS;
#define HAVE_ACU_PROSODY_IP_ENDPOINT_ADDRESS_PARMS

typedef struct tACU_DEVICE_COUNT_PARMS
{
	ACU_ULONG		size;
	ACU_INT        		trunk;
	ACU_INT        		prosody;
	ACU_INT        		ip_telephony;
	ACU_INT        		prosody_x;
} ACU_DEVICE_COUNT_PARMS;

/** For Aculab use only */
typedef struct tACU_REGISTER_VIRTUAL_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_UINT	card_type;
	ACU_UINT	resources_available;
	ACU_CHAR	version[ACU_MAX_HWVER];
	ACU_CHAR	desc[ACU_MAX_CARD_DESC];
	ACU_CHAR	ip_address[ACU_MAX_IP_ADDRESS];
} ACU_PACK_DIRECTIVE ACU_REGISTER_VIRTUAL_CARD_PARMS;

/** For Aculab use only */
typedef struct tACU_UNREGISTER_VIRTUAL_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_UNREGISTER_VIRTUAL_CARD_PARMS;

/** For Aculab use only */
typedef struct tACU_REGISTER_VIRTUAL_BEARERS_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_UINT	port_count;	
} ACU_PACK_DIRECTIVE ACU_REGISTER_VIRTUAL_BEARERS_PARMS;

/** For Aculab use only */
typedef struct tACU_UNREGISTER_VIRTUAL_BEARERS_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_UNREGISTER_VIRTUAL_BEARERS_PARMS;


typedef struct tACU_CARD_CONFIGURATION_STATE_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id;
	ACU_UINT	state;
} ACU_CARD_CONFIGURATION_STATE_PARMS;

typedef struct tACU_PROSODY_IP_CARD_CAPS_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[ACU_MAX_RESOURCE_NAME];
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_CARD_CAPS_PARMS;

typedef struct tACU_CONFIGURE_PXCTRL_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR        ip4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_gateway[ACU_MAX_IP_ADDRESS];
	ACU_INT         reserved;
	ACU_CHAR		ip6_prefix[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_gateway[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_base_addr[ACU_MAX_IP_ADDRESS];
	ACU_UINT		ip6_prefixlen;
	ACU_UINT		auto_ip6;			/* 1 for radvd */
	ACU_UINT		ip6_privacy;		/* Not yet implemented */
} ACU_PACK_DIRECTIVE ACU_CONFIGURE_PXCTRL_CARD_PARMS;

typedef struct tACU_PXCTRL_CARD_CONFIG_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR        ip4_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR        ip4_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT        ip6_prefixlen;
	ACU_CHAR		ip6_prefix[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_static[ACU_MAX_IP_ADDRESS];
	ACU_CHAR		ip6_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT		ip6_address_count;
	ACU_CHAR		ip6_address[ACU_MAX_IPV6_ADDRESSES][ACU_MAX_IP_ADDRESS];
	ACU_UINT		auto_ip6;			/* 1 for radvd */
	ACU_CHAR		site_key[ACU_MAX_CARD_KEY];
	ACU_UINT		ip6_privacy;		/* Not yet implemented */
} ACU_PACK_DIRECTIVE ACU_PXCTRL_CARD_CONFIG_PARMS;

typedef struct tACU_PXCTRL_CARD_CAPS_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[ACU_MAX_RESOURCE_NAME];
} ACU_PACK_DIRECTIVE ACU_PXCTRL_CARD_CAPS_PARMS;

/** Functions for opening Prosody cards that are no longer in the TiNG library */
typedef struct tACU_OPEN_PROSODY_PARMS
{
	ACU_ULONG size;
	ACU_CARD_ID card_id;

} ACU_OPEN_PROSODY_PARMS;

typedef struct tACU_CLOSE_PROSODY_PARMS
{
	ACU_ULONG size;
	ACU_CARD_ID card_id;

} ACU_CLOSE_PROSODY_PARMS;

/** Prosody S Structures */
typedef struct tACU_REGISTER_PROSODY_S_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR	host[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	key[ACU_MAX_CARD_KEY];
	ACU_UINT	utility_port;
	ACU_UINT	cardinfo_port;
	ACU_UINT	license_port;
} ACU_PACK_DIRECTIVE ACU_REGISTER_PROSODY_S_CARD_PARMS;

typedef struct tACU_PROSODY_S_CARD_REGISTRATION_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR	host[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	key[ACU_MAX_CARD_KEY];
	ACU_UINT	utility_port;
	ACU_UINT	cardinfo_port;
	ACU_UINT	license_port;
	ACU_CHAR	status[ACU_MAX_CARD_DESC];
} ACU_PACK_DIRECTIVE ACU_PROSODY_S_CARD_REGISTRATION_PARMS;

typedef struct tACU_UNREGISTER_PROSODY_S_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[ACU_MAX_RESOURCE_NAME];
} ACU_PACK_DIRECTIVE ACU_UNREGISTER_PROSODY_S_CARD_PARMS;

/* Prosody EXT structure types */

typedef struct tACU_PROSODY_EXT_REGISTER_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_UINT	card_type;
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL];
	ACU_INT		persistent;
	ACU_INT		configure;
	ACU_CHAR	ip_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT	ip_type;
	ACU_INT		use_dhcp;
	ACU_CHAR	key[ACU_MAX_CARD_KEY];
	ACU_INT		watchdog_timeout;
	ACU_UINT	utility_port;
	ACU_UINT	cardinfo_port;
	ACU_UINT	license_port;
	ACU_CHAR	host[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	status[ACU_MAX_CARD_DESC];
	ACU_INT		use_remote;
	ACU_UINT	auto_generated_key;
	ACU_CHAR	ip6_prefix[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip6_gateway[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip6_base_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip6_pmx_addr[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip6_dsp_addrs[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_UINT	ip6_prefixlen;
	ACU_UINT	auto_ip6;			/* 1 for radvd */
	ACU_CHAR	controlling_address[ACU_MAX_IP_ADDRESS]; /* OUT: address which resmgr
									  * is using to control the card */
	ACU_UINT	ip6_privacy;		/* Not yet implemented */
	ACU_UINT	host_card;
	/* Added in release 6.8.0 */
	ACU_CHAR	ip6_media_gateways[ACU_CARD_MAX_DSPS][ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip4_media_address[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip4_media_netmask[ACU_MAX_IP_ADDRESS];
	ACU_CHAR	ip4_media_gateway[ACU_MAX_IP_ADDRESS];
	ACU_UINT	media_use_dhcp;
	ACU_UINT	use_media_device;
	ACU_UINT	ip6_base_use_dhcp;
	ACU_UINT	ip6_media_use_dhcp[ACU_CARD_MAX_DSPS];
} ACU_PACK_DIRECTIVE ACU_PROSODY_EXT_REGISTER_CARD_PARMS;

typedef struct tACU_PROSODY_EXT_UNREGISTER_CARD_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	serial_no[ACU_MAX_RESOURCE_NAME];

} ACU_PACK_DIRECTIVE ACU_PROSODY_EXT_UNREGISTER_CARD_PARMS;

typedef struct tACU_PROSODY_EXT_CARD_INFO
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id;
	ACU_UINT	card_type;
	ACU_INT         device;
	ACU_CHAR        model[ACU_MAX_HWVER];
	ACU_CHAR        serial_no[MAX_RESOURCE_SERIAL];
	ACU_CHAR        hw_version[ACU_MAX_HWVER];
	ACU_CHAR        description[ACU_MAX_CARD_DESC];
	ACU_CHAR        bootloader_version[ACU_MAX_FWVER];
	ACU_CHAR        firmware_version[ACU_MAX_FWVER];
	ACU_ULONG       temp0;
	ACU_ULONG       temp1;
	ACU_ULONG		voltage_12_0;
	ACU_ULONG       voltage_5_0;
	ACU_ULONG       voltage_3_3;
	ACU_SHORT       control_port;
	ACU_CHAR        module_model[ACU_MAX_HWVER];
	ACU_CHAR        module_hwver[ACU_MAX_HWVER];
	ACU_UINT	utility_port;
	ACU_UINT	cardinfo_port;
	ACU_UINT	license_port;
	ACU_CHAR	host[ACU_MAX_IP_ADDRESS];
	ACU_UINT	hw_type;
	ACU_UINT	hw_resources;
	ACU_UINT	hw_position;
	//ACU_HARDWARE_FUNCTIONS	*hw_funcs;
	ACU_UINT	flags;
	ACU_CHAR	card_key[ACU_MAX_CARD_KEY];
	unsigned short	call_port;
	ACU_UINT	ip_port;
	ACU_CHAR	hw_desc[ACU_MAX_CARD_DESC];
	ACU_CHAR	ip_address[ACU_MAX_IP_ADDRESS];
	ACU_UINT	fpga_version;
} ACU_PACK_DIRECTIVE ACU_PROSODY_EXT_CARD_INFO;


typedef struct tACU_SNAPSHOT_PARMS_EXT
{
	ACU_ULONG	size;
	ACU_UINT	count;
	ACU_UINT	type[MAX_SNAPSHOT_CARDS];
	ACU_CHAR	serial_no[MAX_SNAPSHOT_CARDS][MAX_RESOURCE_SERIAL];
} ACU_PACK_DIRECTIVE ACU_SNAPSHOT_PARMS_EXT;

typedef struct tACU_PROSODY_EXT_CARD_RESTART_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	card[ACU_MAX_RESOURCE_NAME];
	ACU_INT		method;
} ACU_PROSODY_EXT_CARD_RESTART_PARMS;

typedef struct tACU_PROSODY_IP_REQUEST_REMOTE_LED_PARMS
{
	ACU_ULONG	size;
	ACU_CARD_ID	card_id;
	ACU_INT		enable;
} ACU_PROSODY_IP_REQUEST_REMOTE_LED_PARMS;

typedef struct tACU_PROSODY_IP_GET_HWMON_PARMS
{
	ACU_ULONG	size;				/* IN */
	ACU_CARD_ID	card_id;			/* IN */
	ACU_UINT	hwmon_type;			/* IN: ACU_RESOURCE_HWMON_TEMP, ..._VOLT or ..._FAN */
	ACU_UINT	num_values;			/* OUT: Number of vals returned in vals array */
	struct
	{
		ACU_INT		val;			/* OUT: hwmon value */
		ACU_INT		info;			/* OUT: reserved for future use */
	} vals[ACU_MAX_HWMON_VALS];		/* OUT: */
	/* Added in release 6.8.0 */
	ACU_CHAR	source[ACU_MAX_HWMON_VALS][MAX_HWMON_DESC];		/* OUT: hwmon source */
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_GET_HWMON_PARMS;

typedef struct tACU_PROSODY_IP_STATIC_CONFIG_PARMS
{
	ACU_ULONG	size;				/* IN */
	ACU_CARD_ID	card_id;			/* IN */
	ACU_UINT	static_config;		/* IN or OUT depending on API call.
									 * does/should card use static config */
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_STATIC_CONFIG_PARMS;

typedef struct tACU_PROSODY_IP_RESET_CONFIG_PARMS
{
	ACU_ULONG	size;				/* IN */
	ACU_CHAR	serial_no[MAX_RESOURCE_SERIAL]; /* IN */
} ACU_PACK_DIRECTIVE ACU_PROSODY_IP_RESET_CONFIG_PARMS;

typedef struct tACU_TRACE_MODE_PARMS
{
	ACU_ULONG	size;
	ACU_CHAR	trace_id[ACU_MAX_TRACE_ID];
	ACU_UINT	mask;
} ACU_PACK_DIRECTIVE ACU_TRACE_MODE_PARMS;

ACU_INT ACU_EXPORT acu_open_prosody(ACU_OPEN_PROSODY_PARMS *openp);
ACU_INT ACU_EXPORT acu_close_prosody(ACU_CLOSE_PROSODY_PARMS *closep);

const ACU_CHAR* ACU_EXPORT acu_error_desc(ACU_ERR error);

/* Enable/Disable API trace.  Currently, can only be called before any other resource API, and can only disable trace */
ACU_ERR ACU_EXPORT acu_enable_api_trace(ACU_ENABLE_API_TRACE_PARMS* enablep);

/* functions for resource enumeration and management */
ACU_ERR ACU_EXPORT acu_get_system_snapshot(ACU_SNAPSHOT_PARMS* snapshotp);
ACU_ERR ACU_EXPORT acu_open_card_65(ACU_OPEN_CARD_PARMS* openp);
#define acu_open_card acu_open_card_65
ACU_ERR ACU_EXPORT acu_close_card(ACU_CLOSE_CARD_PARMS* closep);
ACU_ERR ACU_EXPORT acu_get_card_info_65(ACU_CARD_INFO_PARMS* infop);
#define acu_get_card_info acu_get_card_info_65

ACU_ERR ACU_EXPORT acu_set_system_notification_queue(ACU_QUEUE_PARMS* queue_parms);
ACU_ERR ACU_EXPORT acu_get_system_notification(ACU_SYSTEM_NOTIFICATION_PARMS* notifyp);
ACU_ERR ACU_EXPORT acu_get_system_notification_wait_object(ACU_GET_SYS_WAIT_OBJECT_PARMS* objectp);
ACU_ERR ACU_EXPORT acu_eject_card(ACU_EJECT_CARD_PARMS* ejectp);
ACU_ERR ACU_EXPORT acu_enumerate_cards(void);
ACU_ERR ACU_EXPORT acu_get_card_notification(ACU_CARD_NOTIFICATION_PARMS* notifyp);
ACU_ERR ACU_EXPORT acu_get_card_notification_wait_object(ACU_GET_CARD_WAIT_OBJECT_PARMS* objectp);
ACU_ERR ACU_EXPORT acu_identify_card(ACU_IDENTIFY_PARMS* identifyp);

/* functions for queue management */
ACU_ERR ACU_EXPORT acu_allocate_event_queue(ACU_ALLOC_EVENT_QUEUE_PARMS* alloc_parms);
ACU_ERR ACU_EXPORT acu_free_event_queue(ACU_FREE_EVENT_QUEUE_PARMS* free_parms);
ACU_ERR ACU_EXPORT acu_get_event_from_queue(ACU_EVENT_QUEUE_PARMS* event_parms);
ACU_ERR ACU_EXPORT acu_get_event_queue_wait_object(ACU_QUEUE_WAIT_OBJECT_PARMS* wo_parms);

ACU_ERR ACU_EXPORT acu_set_card_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS* contextp);
#define acu_set_card_app_context_token acu_set_card_app_context_token_65
ACU_ERR ACU_EXPORT acu_get_card_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS* contextp);
#define acu_get_card_app_context_token acu_get_card_app_context_token_65

ACU_ERR ACU_EXPORT acu_set_card_name(ACU_SET_CARD_NAME_PARMS* namep);
ACU_ERR ACU_EXPORT acu_set_card_notification_queue(ACU_QUEUE_PARMS* queue_parms);

ACU_ERR ACU_EXPORT acu_get_aculab_directory(ACU_UINT size, char* buffer);

ACU_ERR ACU_EXPORT acu_register_prosody_ip_card(ACU_REGISTER_PROSODY_IP_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_get_prosody_ip_card_config(ACU_PROSODY_IP_CARD_REGISTRATION_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_configure_prosody_ip_card(ACU_PROSODY_IP_CARD_REGISTRATION_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_unregister_prosody_ip_card(ACU_UNREGISTER_PROSODY_IP_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_device_info(ACU_PROSODY_IP_DEVICE_INFO_PARMS* info_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_card_restart(ACU_PROSODY_IP_CARD_RESTART_PARMS* restart_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_registered_cards(ACU_SNAPSHOT_PARMS *snapshot_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_card_status(ACU_PROSODY_IP_CARD_STATUS_PARMS* status_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_local_card_info(ACU_PROSODY_IP_LOCAL_CARD_INFO_PARMS *local_card_info_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_endpoint_address(ACU_PROSODY_IP_ENDPOINT_ADDRESS_PARMS *endpoint_address);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_card_caps(ACU_PROSODY_IP_CARD_CAPS_PARMS *card_caps);

ACU_ERR ACU_EXPORT acu_prosody_thin_get_registered_cards(ACU_SNAPSHOT_PARMS *snapshot_parms);

ACU_ERR ACU_EXPORT acu_pxctrl_get_card_caps(ACU_PXCTRL_CARD_CAPS_PARMS *card_caps);
ACU_ERR ACU_EXPORT acu_configure_pxctrl_card(ACU_CONFIGURE_PXCTRL_CARD_PARMS *configure_parms);
ACU_ERR ACU_EXPORT acu_get_pxctrl_card_config(ACU_PXCTRL_CARD_CONFIG_PARMS *configure_parms);

ACU_ERR ACU_EXPORT acu_get_device_count(ACU_DEVICE_COUNT_PARMS* count_parms);

ACU_ERR ACU_EXPORT acu_get_res_api_version(ACU_RESOURCE_VERSION_PARMS* version_parms);
ACU_ERR ACU_EXPORT acu_get_res_mgr_version(ACU_RESOURCE_VERSION_PARMS* version_parms);

ACU_ERR ACU_EXPORT acu_get_card_configuration_state(ACU_CARD_CONFIGURATION_STATE_PARMS* state_parms);
const ACU_CHAR* ACU_EXPORT acu_get_card_state_desc(ACU_UINT card_state);

/** Prosody S Functions */
ACU_ERR ACU_EXPORT acu_register_prosody_s_card(ACU_REGISTER_PROSODY_S_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_get_prosody_s_card_config(ACU_PROSODY_S_CARD_REGISTRATION_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_configure_prosody_s_card(ACU_PROSODY_S_CARD_REGISTRATION_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_unregister_prosody_s_card(ACU_UNREGISTER_PROSODY_S_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_prosody_s_get_registered_cards( ACU_SNAPSHOT_PARMS *snapshot_parms  );

/** Prosody X/S Functions **/

ACU_ERR ACU_EXPORT acu_unregister_prosody_ext_card(ACU_PROSODY_EXT_UNREGISTER_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_prosody_ext_get_registered_cards( ACU_SNAPSHOT_PARMS_EXT *snapshot_parms );
ACU_ERR ACU_EXPORT acu_register_prosody_ext_card(ACU_PROSODY_EXT_REGISTER_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_get_prosody_ext_card_config(ACU_PROSODY_EXT_REGISTER_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_configure_prosody_ext_card(ACU_PROSODY_EXT_REGISTER_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_prosody_ext_card_restart (ACU_PROSODY_EXT_CARD_RESTART_PARMS *restart_parms);
ACU_ERR ACU_EXPORT acu_prosody_ext_get_device_info (ACU_PROSODY_EXT_CARD_INFO* info_parms);

/** Functions for Aculab use ONLY */
ACU_ERR ACU_EXPORT acu_register_virtual_card(ACU_REGISTER_VIRTUAL_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_unregister_virtual_card(ACU_UNREGISTER_VIRTUAL_CARD_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_resmgr_reload_config(void);
ACU_ERR ACU_EXPORT acu_register_virtual_bearers(ACU_REGISTER_VIRTUAL_BEARERS_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_unregister_virtual_bearers(ACU_UNREGISTER_VIRTUAL_BEARERS_PARMS* register_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_request_remote_led(ACU_PROSODY_IP_REQUEST_REMOTE_LED_PARMS *remote_led_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_get_hwmon(ACU_PROSODY_IP_GET_HWMON_PARMS *hwmon_parms);
ACU_ERR ACU_EXPORT acu_set_trace_mode(ACU_TRACE_MODE_PARMS *trace_mode_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_use_static_config(ACU_PROSODY_IP_STATIC_CONFIG_PARMS *static_conf_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_has_static_config(ACU_PROSODY_IP_STATIC_CONFIG_PARMS *static_conf_parms);
ACU_ERR ACU_EXPORT acu_prosody_ip_reset_static_config(ACU_PROSODY_IP_RESET_CONFIG_PARMS *reset_conf_parms);

typedef struct tACU_RESMGR_CFG_OPTION_PARMS
{
	ACU_ULONG		size;
	ACU_CHAR		section[ACU_MAX_CFG_ENTRY_SIZE];
	ACU_CHAR		entry[ACU_MAX_CFG_ENTRY_SIZE];
	ACU_CHAR		value[ACU_MAX_CFG_ENTRY_SIZE];
} ACU_RESMGR_CFG_OPTION_PARMS;

ACU_ERR ACU_EXPORT acu_resmgr_set_cfg_option(ACU_RESMGR_CFG_OPTION_PARMS* option_parms);
ACU_ERR ACU_EXPORT acu_resmgr_get_cfg_option(ACU_RESMGR_CFG_OPTION_PARMS* option_parms);

#ifdef _WIN32
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif /* RES_TYPE_H_INCLUDED */

