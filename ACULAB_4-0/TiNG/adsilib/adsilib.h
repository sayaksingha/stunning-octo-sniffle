#include "smdrvr.h"

enum { kSASToneTypeSingleBurst,
	kSASToneTypeDoubleBurst,
	kSASToneTypeTripleBurst,
};

typedef struct sm_play_sas_cas_parms {
	tSMChannelId	channel;
	tSM_INT			sas_tone_type;
} SM_PLAY_SAS_CAS_PARMS;

typedef struct sm_play_vmwi_fsk_parms {
	tSMChannelId	channel;
	tSM_INT			do_on_hook;
	tSM_INT			is_sdmf_message;
	tSM_INT			is_indicator_on_message;
} SM_PLAY_VMWI_FSK_PARMS;

#define kCNDTypeAnonymous 0
#define kCNDTypeOutOfArea 1
#define kCNDTypeCallingDN 2

#define kCNDCallingDNMaxDigits 16
#define kCNDCallingNameMaxLen  64

typedef struct sm_play_cnd_fsk_parms {
	tSMChannelId	channel;
	tSM_INT			do_on_hook;
	tSM_INT			is_sdmf_message;
	tSM_INT			cnd_type;
	tSM_INT			month;
	tSM_INT			day;
	tSM_INT			hour;
	tSM_INT			minute;
	char			digit_string[1+kCNDCallingDNMaxDigits];
	char			name_string[1+kCNDCallingNameMaxLen];
} SM_PLAY_CND_FSK_PARMS;

typedef struct {
	tSMModuleId module;
	tSM_INT tone_id_440_Hz;	// for SAS
	tSM_INT tone_id_cas;	// 2130 + 2750 @ -15dBm0 each
} ADSI_MODULE;

enum adsi_txstatus {
 	kADSIStatusComplete,
 	kADSIStatusOngoing,
};

int adsi_init(ADSI_MODULE *mp, tSMModuleId module);

int sm_play_sas_cas(struct sm_play_sas_cas_parms *pp, ADSI_MODULE *mp);

int sm_play_vmwi_fsk(struct sm_play_vmwi_fsk_parms *);

int sm_play_cnd_fsk(struct sm_play_cnd_fsk_parms *);

int adsi_status(enum adsi_txstatus *sts, tSMChannelId chan);
