/* px_prostype.h - extra types for PX */

#ifndef INCLUDED_PX_PROSTYPE_H
#define INCLUDED_PX_PROSTYPE_H

typedef struct tSMRTCPHandId_struct *tSMRTCPHandId;
#define kSMNullRTCPHandId (tSMRTCPHandId) 0
typedef struct tSMVMPrxId_struct *tSMVMPrxId;
#define kSMNullVMPrxId (tSMVMPrxId) 0
typedef struct tSMVMPtxId_struct *tSMVMPtxId;
#define kSMNullVMPtxId (tSMVMPtxId) 0
typedef struct tSMVMPTxToneSetId_struct *tSMVMPTxToneSetId;
#define kSMNullVMPTxToneSetId (tSMVMPTxToneSetId) 0

typedef struct tSMDTLSrxId_struct *tSMDTLSrxId;
#define kSMNullDTLSrxId (tSMDTLSrxId) 0
typedef struct tSMDTLStxId_struct *tSMDTLStxId;
#define kSMNullDTLStxId (tSMDTLStxId) 0

typedef struct tSMFMPtxId_struct *tSMFMPtxId;
#define kSMNullFMPtxId (tSMFMPtxId) 0
typedef struct tSMFMPrxId_struct *tSMFMPrxId;
#define kSMNullFMPrxId (tSMFMPrxId) 0

typedef struct tSMFramerId_struct *tSMFramerId;

typedef struct tSMH223muxId_struct *tSMH223muxId;

typedef struct tSMI460muxId_struct *tSMI460muxId;
typedef tSM_INT tSMI460InputId;

typedef struct tSMSNTPId_struct *tSMSNTPId;

typedef struct tSMSPRTId_struct *tSMSPRTId;
#define kSMNullSPRTId (tSMSPRTId) 0

typedef struct tSMVMPTxCSRCListId_struct *tSMVMPTxCSRCListId;
#define kSMNullVMPTxCSRCListId (tSMVMPTxCSRCListId) 0

typedef struct tSMVidMPrxId_struct *tSMVidMPrxId;
#define kSMNullVidMPrxId (tSMVidMPrxId) 0
typedef struct tSMVidMPtxId_struct *tSMVidMPtxId;
#define kSMNullVidMPtxId (tSMVidMPtxId) 0

typedef struct ttSMVMPTxTone {
        double f1_min;		// lower limit of lower tone (Hz)
        double f1_max;		// upper limit of lower tone (Hz)
        double f2_min;		// lower limit of upper tone (Hz)
        double f2_max;		// upper limit of upper tone (Hz)
        tSM_INT id;		// ID to use when this tone is detected
} tSMVMPTxTone;

typedef struct ttSMVMPTxToneSet {
        double req_minimum_power;
        double req_twist_for_dual_tone;
        double req_third_peak;
        double req_signal_to_noise_ratio;
        tSM_INT number_of_tones;	// number of entries on tone list
        tSMVMPTxTone* tones;		// tone list
} tSMVMPTxToneSet;

#endif

