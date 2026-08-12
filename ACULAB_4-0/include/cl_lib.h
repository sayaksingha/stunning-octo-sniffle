/*****************************************************************************\
*
* CL_LIB.H : V6 Call API Declarations
*
* (C)2002-2005 Aculab Plc
*
*
\*****************************************************************************/
#ifndef CL_TYPE_H_INCLUDED
#define CL_TYPE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "acu_type.h"
#define MAXEXTENDEDLISTENPARAMS 20 /* maximum number of extended listen parameters to be set in the SIP service */

#ifndef ACU_LINUX
#ifndef ACU_QNX
# pragma pack (push, 1 )
# define ACU_PACK_DIRECTIVE
#else
# define ACU_PACK_DIRECTIVE __attribute__ ((packed))
#endif
#else
# define ACU_PACK_DIRECTIVE __attribute__ ((packed))
#endif /* ACU_LINUX */

#ifdef IGNORE_CL_LIB_DEPRECATED_WARNINGS
#undef ACU_DEPRECATED
#define ACU_DEPRECATED
#endif

enum {
    /*---- system configuration information ----*/
    /*------------------------------------------*/
    MAXCNTRL        =   20,                     /* maximum number of controllers        */
    MAXPPC          =   19,                     /* maximum number of ports per card + 2 possible ip ports */
    MAXIO           =   3,                      /* maximum types of handle              */
    MAXPORT         =   (MAXCNTRL * MAXPPC),    /* maximum number of ports in system    */
    MAXNUM          =   32,                     /* maximum number of subscriber digits  */
    MAXSIGSYS       =   8,                      /* maximum number of signalling type    */
    MAXBEARER       =   16,                     /* maximum length of bearer code        */
    MAXCLC          =   10,                     /* maximum length of calling category   */
    MAXSPID         =   32,                     /* maximum length of spid               */
    MAXHILAYER      =   16,                     /* maximum length of high layer         */
    MAXLOLAYER      =   16,                     /* maximum length of low layer          */
    MAXPROGRESS     =   4,                      /* maximum length of progress indicator */
    MAXNETSPECIFICFACIL= 11,                    /* maximum length of network specific facilities indicator */
    MAXCAUSE        =   32,                     /* maximum length of cause */
    MAXNOTIFY       =   4,                      /* maximum length of notify indicator   */
    MAXDISPLAY      =   34,                     /* maximum length of display            */
    MAXMESSWAIT     =   16,                     /* maximum length of message waiting    */
    MAXKEYPAD       =   34,                     /* maximum length of keypad information */
    MAX_CARD_DSP_COUNT= 5,                      /** Maximum number of DSP positions on a card */
    MAX_DSPMAN_PORTS=   16,                     /** Number of ports on a card supported by the DSP Manager */
    CL_MAX_MODULE_KEY = 60,
    CL_MAX_FMW_PARMS = 1024,                    /** Max length of firmware parameters */


    MAX_HW_DESC     =   20,                     /* maximum length of hardware description string */
    MAX_FW_DESC     =   20,                     /* max length of firmware description string */
    MAX_FW_EXTN     =   20,                     /* max length of firmware extension */
    MAX_VER_DESC    =   30,                     /* max length of a version description */
    MAXUUI_INFO     =   252,                    /* maximum length of User To User information */
    MAXUUI_INFO_ATT  =  248,                    /* maximum length of User To User information for AT&T UI message */
    MAXFACILITY_INFO =  255,                    /* maximum length of Facility information */
    MAXRAWDATA      =   260,                    /* maximum length of raw data information */
    MAXNOTIFYMSG    =   255,
    MAXOID          =   32,                     /* maximum length of an object object identifier */
    MAX_L1_TEXT     =   16,                     /* Max length of text in layer 1 parms */
    MAXFEATUREACTIVATION = 3,                   /* maximum length of feature activation */
    MAXINFORMATIONREQUEST = 2,                  /* maximum length of information request */

    MAX_FEAT_MSG    =   5,                      /* dpnss enhanced only: Max Msg for DPNSS feature Info          */
    MAXNSI          =   50,                     /* dpnss enhanced only: maximum number of DPNSS NSI characters  */
    MAXCALLREF      =   40,                     /* dpnss enhanced only: maximum number of digits in CAll Reference field  */
    MAXSIC          =   5,                      /* dpnss enhanced only: maximum number of digits in SIC used with BSS  */
    MAXRED          =   3,                      /* dpnss enhanced only: max digits for redirecting reason */
    MAXRCF          =   3,                      /* dpnss enhanced only: */
    MAXCALLID       =   2,                      /* dpnss enhanced only: maximum number of digits in CAll Reference ID Length Field  */
    MAXTXT          =   30,                     /* dpnss enhanced only: maximum number of DPNSS TXT characters  */
    MAXTID          =   15,                     /* dpnss enhanced only: maximum number of Trunk ID characters   */
    MAXNAE          =    32,                    /* dpnss enhanced only: maximum number of Netword Address Extension characters   */
    TRANSIT_MSG_LENGTH  =   50,                 /* dpnss enhanced only: DPNSS transit message length            */

    MAXL3LENGTH     =   260,                    /* The Max Packet size for a layer 3 packet to be transported accross layer 2 */

    MAXSERIALNO     =   12,                     /* Maximum length of the serial number of the PCI cards */
    MAXHWVERSION    =   20,                     /* Maximum length of the hardware version number */
    MAXNAME         =   20,                     /* Maximum length of a port's name */
    MAXADDR         =   512,                    /* Maximum size of address arrays within out/detail xparms */
    MAXHOSTADDRESS          =       64,
    MAXID                   =       128,
    MAXREGISTRATIONS        =       1024,
    MAXMEDIACALLTYPE        =       50,         /* Maximum size of media_call_type */
    MAXVOIPALIASESS         =       10,

    MAX_NAME_PRESENTATION_DATA      = 50,       /* Maximum length of data octets sent in NAME_PRESENTATION_XPARMS */

    /* Configuration constants */
    ACU_CALL_CONFIG_OPTION_LEN      = 30,
    ACU_CALL_CONFIG_DESC_LEN        = 100,

    /* Call direction returned by call_details() */
    INCOMING        =   1,                      /* incoming call */
    OUTGOING        =   2,                      /* outgoing call */

    /* Constants for call_handle_2_io() */
    ACU_CALL_IO_INCOMING        = 0,    /* Incoming call */
    ACU_CALL_IO_OUTGOING        = 1,    /* Outgoing call */
    ACU_CALL_IO_ENQUIRY         = 2,    /* Enquiry call */

    /* constants for call_port_info() */
    ACU_PACKET_SWITCHED             =   1,
    ACU_CIRCUIT_SWITCHED            =   2,
    ACU_CHANNEL_FIXED               =   1,
    ACU_CHANNEL_DYNAMIC             =   2,
    ACU_CHANNEL_DYNAMIC_UNLIMITED   =   3,

    /* Constants for DSP types (call_dsp_config()) */
    ACU_DSP_TYPE_NOT_FITTED =   0,              /* DSP not fitted */
    ACU_DSP_TYPE_DSP32      =   1,              /* DSP 32 fitted (use .bin firmware) */
    ACU_DSP_TYPE_DSP65      =   2,              /* DSP 65 fitted (use .b65 firmware) */

    /* Constants for DSP types (call_card_dsp_config()) */
    ACU_DSP_TYPE_DSP32_ONE      = 1,
    ACU_DSP_TYPE_DSP32_TWO      = 2,
    ACU_DSP_TYPE_DSP32_FOUR     = 3,
    ACU_DSP_TYPE_DSP65_ONE      = 4,
    ACU_DSP_TYPE_DSP65_TWO      = 5,
    ACU_DSP_TYPE_DSP65_FOUR     = 6,
    ACU_DSP_TYPE_DSP65_EIGHT    = 7,
    ACU_DSP_TYPE_PMX_HIGH_CAP   = 8,
    ACU_DSP_TYPE_PMX_LOW_CAP    = 9,


    /*------ call charging ------*/
    /* call charging information */
    /*---------------------------*/

    CHARGE_NONE             =   0,              /* no valid information */
    CHARGE_INFO             =   1,              /* call charge valid    */
    CHARGE_METER            =   2,              /* meter pulse valid    */
    CHARGEMAX               =   34,             /* maximum number of call charging digits */

    CHARGE_IND_NO_INDICATION    =   0,          /* no indication of charge present */
    CHARGE_IND_NO_CHARGE        =   1,          /* no charge */
    CHARGE_IND_CHARGE           =   2,          /* charge */


    /*---------- service octet defintions ---------*/
    /* and additional information octet definitions*/
    /*---------------------------------------------*/

    UNKNOWN_SERVICE         =   0xff,           /* undetermined service */

    TELEPHONY               =   1,              /* service octet - telephony */
        ISDN_3K1            =   1,              /* 3.1khz telephony */
        ANALOGUE            =   2,              /* analogue         */
        ISDN_7K             =   3,              /* 7Khz telephony   */

    ABSERVICE               =   2,              /* service octet - a/b services */
        FAXGP2              =   1,              /* group fax 2 */
        FAXGP3              =   2,              /* group fax 3 */

    X21SERVICE              =   3,              /* service octet - X.21 Services     */
        UC4                 =   4,              /* UC 4  */
        UC5                 =   5,              /* UC 5  */
        UC6                 =   6,              /* UC 6  */
        UC19                =   12,             /* UC 19 */

    FAXGP4                  =   4,              /* service octet - Fax Group 4       */
        VIDEO64K            =   5,              /* service octet - 64Kbits Videotext */
        DATA64K             =   7,              /* service octet - 64Kbits data      */

    X25SERVICE              =   8,              /* service octet - X.25 Services     */
        UC8                 =   1,              /* UC 8  */
        UC9                 =   2,              /* UC 9  */
        UC10                =   3,              /* UC 10 */
        UC11                =   4,              /* UC 11 */
        UC13                =   5,              /* UC 13 */
        UC19K2              =   6,              /* 19.2k */

    TELTEXT64               =   9,              /* service octet - Teletext 64       */
    MIXEDMODE               =   10,             /* service octet - Mixed Mode        */
    TELEACTION              =   13,             /* service octet - Teleaction        */
    GRAPHIC                 =   14,             /* service octet - Graphic Telephone */
    VIDEOTEXT               =   15,             /* service octet - Videotext         */

    VIDEOPHONE              =   16,             /* service octet - Videophone        */
        SOUND_3K1           =   1,              /* 3.1khz sound     */
        SOUND_7K            =   2,              /* 7Khz sound       */
        IMAGE               =   3,              /* image            */

    /*---------------------------------------*/
    /*  VoIP media parameters                */
    /*---------------------------------------*/

    NOT_INITIALISED     =   0,
    G711_ALAW           =   1,
    G711_ULAW           =   2,
    G723                =   3,
    G729                =   4,
    G729A               =   5,
    T38                 =   6,
    G726                =   7,
    GSM_FR              =   8,
    iLBC                =   9,
    RFC4040             =   10,
    G728                =   11,
    AMR_NB              =   12,
    AMR_WB              =   13,
    EVRC                =   14,
    EVRC0               =   15,     /* headerless EVRC */
    SMV                 =   16,
    SMV0                =   17,     /* headerless SMV */

    MAX_CODEC_ID        =   17,
    MAXCODECS           =   11,

    G726_OPTION_16K     =   1,
    G726_OPTION_24K     =   2,
    G726_OPTION_32K     =   4,
    G726_OPTION_40K     =   8,

    G728_OPTION_9K6     =   1,
    G728_OPTION_12K8    =   2,
    G728_OPTION_16K     =   4,

    iLBC_OPTION_13K3    =   1,
    iLBC_OPTION_15K2    =   2,

    GSM_FR_OPTION_MS    =   1,

    VAD_OFF             =   0,
    VAD_ON              =   1,

    TDM_ULAW            =   1,
    TDM_ALAW            =   2,
    TDM_DATA            =   3,

    /* echo cancellation and echo suppression values */
    EC_OFF              =   1,
    EC_G165             =   2,
    EC_G165_NLP         =   3,
    EC_ON               =   2,  /* intentionally the same as EC_G165 */
    EC_ON_NLP           =   3,  /* intentionally the same as EC_G165_NLP */

    ES_OFF              =   1,
    ES_12DB             =   12,

    /* clearing cause types */
    H225_RCR            =   1,
    Q931_CAUSE          =   2,
    ACU_INTERNAL_CAUSE  =   3,

    /* Marker to say VMPs will be provided later */
    ACU_WILL_PROVIDE_VMP    =   -1,

    /* SIP warning strings */
    MAXSIPWARNING               =   64,     /* maximum length that we   */
                                            /* accept for a SIP warning */
    MAXSIPRESPONSEEXPLANATION   =   128,    /* maximum length of non-200  */
                                            /* final response explanation */

    /* H.323 call progress location and description info */
    L_USER                  =   0,
    L_PRIV_NET_LOCAL_USER   =   1,
    L_PUB_NET_LOCAL_USER    =   2,
    L_TRANSIT_NET           =   3,
    L_PUB_NET_REMOTE_USER   =   4,
    L_PRIV_NET_REMOTE_USER  =   5,
    L_BEYOND_IW_POINT       =   10,

    D_NOT_END_TO_END_ISDN   =   1,
    D_DEST_ADDR_NON_ISDN    =   2,
    D_ORIG_ADDR_NON_ISDN    =   3,
    D_CALL_RETURNED_TO_ISDN =   4,
    D_IW_OCCURRED           =   5,
    D_INBAND_INFO_AVAIL     =   8,

    /* H.323 hold types */
    ACU_HOLD_NEAR_END       =   0,
    ACU_HOLD_REMOTE         =   1,
    ACU_HOLD_MEDIA          =   2,

    /*---------------------------------------------*/



    /*---------------------------------------*/
    /*  Signalling Protocol Identifiers      */
    /*---------------------------------------*/

    S_UNKNOWN           =   0,

    S_1TR6              =   1,     /* user end definitions */
    S_DASS              =   2,
    S_DPNSS             =   3,
    S_CAS               =   4,
    S_AUSTEL            =   5,
    S_ETS300            =   6,
    S_VN3               =   7,
    S_ATT               =   8,
    S_CAS_TONE          =   9,
    S_TNA_NZ            =   10,
    S_FETEX_150         =   11,
    S_SWETS300          =   12,
    S_IDAP              =   13,
    S_T1CAS             =   14,
    S_T1CAS_TONE        =   15,
    S_NI2               =   16,
    S_DPNSS_EN          =   17,
    S_ATT_T1            =   18,
    S_QSIG              =   19,

    S_1TR6NET           =   20,     /* network end definitions */
    S_VN3NET            =   21,
    S_ETSNET            =   22,
    S_AUSTNET           =   23,
    S_ATTNET            =   24,
    S_DASSNET           =   25,
    S_TNANET            =   26,
    S_FETEXNET          =   27,
    S_SWETSNET          =   28,
    S_IDAPNET           =   29,
    S_NI2NET            =   30,
    S_ATTNET_T1         =   31,
    S_DPNSS_T1          =   32,
    S_FETEX_150_T1      =   33,
    S_FETEXNET_T1       =   34,

    S_INS_T1            =   35,
    S_INSNET_T1         =   36,

    S_INS               =   37,
    S_INSNET            =   38,

    S_ISUP              =   39,

    S_GLOBAND           =   40,
    S_GLOBNET           =   41,

    S_MON               =   42,
    S_MON_T1            =   43,

    S_QSIG_T1           =   44,
    S_DPNSS_EN_T1       =   45,

    S_ETS300_T1         =   46,
    S_ETSNET_T1         =   47,
    S_H323              =   48,
    S_SIP               =   49,

    S_DMS100            =   60,
    S_DMS1NET           =   61,

    NCHAN               =   255,        /* number of channels      */

    S_SS5_TONE          =   90,

    S_BASE              =   99,


    /*---------------------------------------*/
    /*  Line Interface Types                 */
    /*---------------------------------------*/

    L_E1                =   1,
    L_T1_CAS            =   2,
    L_T1_ISDN           =   3,
    L_BASIC_RATE        =   4,
    L_PSN               =   5,


    /*---------------------------------------*/
    /*  Card Types                           */
    /*---------------------------------------*/

    /* NOTE To developers:
     * Any new card types should be added to the END of this list
     * and the 'NUM_SUPPORTED_CARDS' macro increased accordingly
     */
#ifdef ACU_ISA
    C_REV4          =   0,
    C_REV5          =   1,
    C_BR4           =   2,
    C_BR8           =   3,
#endif
    C_P1_P2         =   4,  /* PCI P1 & P2 */
    C_C1            =   5,
    C_C2            =   6,
    C_VOIP          =   7,
    C_C3            =   8,
    C_PROSODY_X_PCI =   9,
    C_PROSODY_X_CPCI=   10,
    C_PROSODY_X_PXI =   11,
    C_VIRTUAL       =   99,

    NUM_SUPPORTED_CARDS     =   12, /* The number of card types we support */

    /*---------------------------------------*/
    /*  PM module types                      */
    /*---------------------------------------*/

    MODULE_NOT_FITTED           =   0,
    PM4_NOTFITTED               =   0,
    PM4_ONE_PORT_E1             =   1,
    PM4_TWO_PORT_E1             =   2,
    PM4_FOUR_PORT_E1            =   3,
    PM4_ONE_PORT_T1             =   5,
    PM4_TWO_PORT_T1             =   6,
    PM4_FOUR_PORT_T1            =   7,
    PM4_TWO_PORT_E1T1           =   8,
    PM4_FOUR_PORT_E1T1          =   9,
    PM4_TWO_PORT_E1_MONITOR     =   10,     /* high impedance, passive monitor */
    PM4_FOUR_PORT_E1_MONITOR    =   11,     /* high impedance, passive monitor */
    PM4_TWO_PORT_T1_MONITOR     =   12,     /* high impedance, passive monitor */
    PM4_FOUR_PORT_T1_MONITOR    =   13,     /* high impedance, passive monitor */
    UPM_ONE_PORT_E1T1           =   17,     /* UPM  - PM4 Rev 2 */
    UPM_TWO_PORT_E1T1           =   18,
    UPM_FOUR_PORT_E1T1_SS7      =   19,     /* PM4 Rev 2.4 SS7 'special' */
    UPM_FOUR_PORT_E1T1          =   20,
    UPM_TWO_PORT_E1T1_MONITOR   =   26,
    UPM_FOUR_PORT_E1T1_MONITOR  =   28,
    PMX_SIXTEEN_PORT_E1T1       =   100,
    PMX_EIGHT_PORT_E1T1         =   101,
    PMX_FOUR_PORT_E1T1          =   102,
    PMX_TWO_PORT_E1T1           =   103,
    PMX_ONE_PORT_E1T1           =   104,
    VIRTUAL_PORT_E1T1           =   120,

    ACU_PORT_CAP_E1_ISDN            =   1,
    ACU_PORT_CAP_T1_ISDN            =   2,
    ACU_PORT_CAP_E1_CAS             =   4,
    ACU_PORT_CAP_T1_CAS             =   8,
    ACU_PORT_CAP_IP                 =   16,
    ACU_PORT_CAP_E1_ISDN_MONITOR    =   32,
    ACU_PORT_CAP_T1_ISDN_MONITOR    =   64,
    ACU_PORT_CAP_SS7                =   128,
    ACU_PORT_CAP_E1_ISUP            =   256,
    ACU_PORT_CAP_T1_ISUP            =   512,
    ACU_PORT_CAP_IP_OFFLOAD         =   1024,

    /*-------------------------------------*/
    /* call time configuration switches    */
    /*-------------------------------------*/

    CNF_CALL_COLLECT    =   0x00010000, /* Currently ISUP ONLY: call collect request */
    CNF_REM_DISC        =   0x00008000, /* remote disconnect  */
    CNF_CALL_CHARGE     =   0x00004000, /* call charging      */
    CNF_TSPREFER        =   0x00002000, /* preferred timeslot */
    CNF_ENABLE_V5API    =   0x00001000, /* enable V5 api      */
    CNF_TSVIRTUAL       =   0x00000800, /* virtual tineslot   */
    CNF_RAW_MSG         =   0x00000400, /* Currently ISUP ONLY: enable raw message reception */
    CNF_NET_ANY         =   0x00000200, /* NOT IMPLEMENTED, PROVIDED FOR SOURCE COMPATIBILITY ONLY */
    CNF_COMPLETE        =   0x00000100, /* ISRMI/ISRMC        */
    CNF_NET_MASK        =   0x0000001f, /* for aculab use only*/

    /*-------------------------------------*/
    /* Q.931 Message types                 */
    /*-------------------------------------*/

    Q931_NO_MSG             =   0x00,
    Q931_ALERTING           =   0x01,
    Q931_CALL_PROCEEDING    =   0x02,
    Q931_PROGRESS           =   0x03,
    Q931_SETUP              =   0x05,
    Q931_CONNECT            =   0x07,
    Q931_SETUP_ACK          =   0x0D,
    Q931_CONNECT_ACK        =   0x0F,
    Q931_USER_INFO          =   0x20,
    Q931_SUSPEND_REJ        =   0x21,
    Q931_RESUME_REJ         =   0x22,
    Q931_HOLD               =   0x24,
    Q931_SUSPEND            =   0x25,
    Q931_RESUME             =   0x26,
    Q931_HOLD_ACK           =   0x28,
    Q931_SUSPEND_ACK        =   0x2D,
    Q931_RESUME_ACK         =   0x2E,
    Q931_HOLD_REJECT        =   0x30,
    Q931_RETRIEVE           =   0x31,
    Q931_RETRIEVE_ACK       =   0x33,
    Q931_RETRIEVE_REJECT    =   0x37,
    Q931_DISCONNECT         =   0x45,
    Q931_RESTART            =   0x46,
    Q931_RELEASE            =   0x4D,
    Q931_RESTART_ACK        =   0x4E,
    Q931_RELEASE_CMPL       =   0x5A,
    Q931_SEGMENT            =   0x60,
    Q931_FACILITY           =   0x62,
    Q931_NOTIFY             =   0x6E,
    Q931_STATUS_ENQ         =   0x75,
    Q931_CONGESTION_CNTRL   =   0x79,
    Q931_INFORMATION        =   0x7B,
    Q931_STATUS             =   0x7D,


    /*------------------------------*/
    /* DPNSS Feature MSG Types      */
    /*------------------------------*/

    NO_MSG                  =   0,      /* DPNSS Enhanced Only */
    ACKNOWLEDGE             =   40,     /* DPNSS Enhanced Only */
    REJECT                  =   41,     /* DPNSS Enhanced Only */
    STATE_OF_DEST_FREE      =   42,     /* DPNSS Enhanced Only */
    STATE_OF_DEST_BUSY      =   43,     /* DPNSS Enhanced Only */
    STATE_OF_DEST_REQ       =   44,     /* DPNSS Enhanced Only */
    EXTENSION_STATUS_CALL   =   45,     /* DPNSS Enhanced Only */
    LOOP_AVOIDANCE          =   46,     /* DPNSS Enhanced Only */
    CALL_OFFER              =   47,     /* DPNSS Enhanced Only */
    DIVERT_REDIRECTION      =   48,     /* DPNSS Enhanced Only */
    REDIRECTING             =   49,     /* DPNSS Enhanced Only */

    /*--- DPNSS Call types : Virtual/Real ---*/

    REAL                    =   0,      /* DPNSS Enhanced Only */
    VIRTUAL                 =   1,      /* DPNSS Enhanced Only */


    /*-------------------------------------*/
    /* Call Diversion messages             */
    /*-------------------------------------*/

    DIVERT_IMMEDIATE            =   1,      /* DPNSS Enhanced Only */
    DIVERT_BUSY                 =   2,      /* DPNSS Enhanced Only */
    DIVERT_NO_REPLY             =   3,      /* DPNSS Enhanced Only */
    DIVERTING_IMM               =   4,      /* DPNSS Enhanced Only */
    DIVERTING_BSY               =   5,      /* Generic */
    DIVERTING_RNR               =   6,      /* Generic */
    DIVERTED_IMM                =   7,      /* DPNSS Enhanced Only */
    DIVERTED_BSY                =   8,      /* DPNSS Enhanced Only */
    DIVERTED_RNR                =   9,      /* DPNSS Enhanced Only */
    DIV_VALIDATION              =   10,     /* DPNSS Enhanced Only */
    DIV_BYPASS                  =   11,     /* DPNSS Enhanced Only */
    DIVERTING_UNKNOWN           =   12,     /* Generic */
    DIVERTING_UNCONDITIONAL     =   13,     /* Generic */
    DIVERTING_CD                =   14,     /* Generic */
    DEFLECTION_RINGING          =   15,     /* Generic */
    DEFLECTION_IMM              =   16,     /* Generic */
    DIVERTING_OUT_OF_ORDER      =   17,     /* NI-2 only */
    DIVERTING_NETWORK_BSY       =   18,     /* ATT */

    /*-------------------------------------*/
    /* Call Hold messages                  */
    /*-------------------------------------*/

    HOLD_CALL               =   20,     /* DPNSS Enhanced Only */
    HOLD_ACK                =   21,     /* DPNSS Enhanced Only */
    HOLD_REJECT             =   22,     /* DPNSS Enhanced Only */
    HOLD_NOT_SUPPORTED      =   23,     /* DPNSS Enhanced Only */
    RECONNECT_CALL          =   24,     /* DPNSS Enhanced Only */

    /*-------------------------------------*/
    /* DPNSS Enquiry Call messages         */
    /*-------------------------------------*/

    ENQUIRY                 =   30,     /* DPNSS Enhanced Only */

    /*-------------------------------------*/
    /* DPNSS Call Transfer messages        */
    /*-------------------------------------*/

    TRANSFER_O              =   31,     /* DPNSS Enhanced Only */
    TRANSFER_T              =   32,     /* DPNSS Enhanced Only */
    TRANSFERRED             =   33,     /* DPNSS Enhanced Only */
    TRANSFERRED_INFO        =   34,     /* DPNSS Enhanced Only */

    /*-------------------------------------*/
    /* DPNSS Route OPtimisation            */
    /*-------------------------------------*/

    ROUTE_OPT_REQUEST       =   35,     /* DPNSS Enhanced Only */
    ROUTE_OPT_CALL_SETUP    =   36,     /* DPNSS Enhanced Only */
    ROUTE_OPT_CONNECTED     =   37,     /* DPNSS Enhanced Only */
    ROUTE_OPT_INVITE        =   38,     /* DPNSS Enhanced Only */
    ROUTE_OPT_INVITE_ACK    =   39,     /* DPNSS Enhanced Only */

    /*-------------------------------------*/
    /* DPNSS Mitel Call Charging           */
    /*-------------------------------------*/

    CHARGE_REQUEST          =   50,     /* DPNSS Enhanced Only */
    CHARGE_UNITS_USED       =   51,     /* DPNSS Enhanced Only */
    CHARGE_ACTIVATE         =   52,     /* DPNSS Enhanced Only */
    CHARGE_ACCOUNT_REQUEST  =   53,     /* DPNSS Enhanced Only */
    CHARGE_ACCOUNT_CODE     =   54,     /* DPNSS Enhanced Only */


    /*-------------------------------------*/
    /* DPNSS Do Not Disturb                */
    /*-------------------------------------*/

    DO_NOT_DISTURB          =   55,     /* DPNSS Enhanced Only */
    DO_NOT_DISTURB_OVERRIDE =   56,     /* DPNSS Enhanced Only */


    /*-------------------------------------*/
    /* DPNSS Conference                    */
    /*-------------------------------------*/

    ADD_ON_VALIDATION       =   60,     /* DPNSS Enhanced Only */
    ADD_ON_ACK              =   61,     /* DPNSS Enhanced Only */
    ADDED_ON                =   62,     /* DPNSS Enhanced Only */
    ADD_ON_REJECT           =   63,     /* DPNSS Enhanced Only */
    ADD_ON_NOT_SUPPORTED    =   64,     /* DPNSS Enhanced Only */
    ADD_ON_CLEARDOWN        =   65,     /* DPNSS Enhanced Only */
    TWO_PARTY_O             =   66,     /* DPNSS Enhanced Only */
    TWO_PARTY_T             =   67,     /* DPNSS Enhanced Only */


    /*-------------------------------------*/
    /* Executive Intrusion                 */
    /*-------------------------------------*/

    INTRUSION_REQUEST       =   70,     /* DPNSS Enhanced Only */
    PV_INTRUSION            =   71,     /* DPNSS Enhanced Only */
    INTRUSION_ACK           =   72,     /* DPNSS Enhanced Only */
    INTRUDING               =   73,     /* DPNSS Enhanced Only */
    IPL_REQUEST             =   74,     /* DPNSS Enhanced Only */
    IPL_RESPONSE            =   75,     /* DPNSS Enhanced Only */
    INTRUSION_WITHDRAW      =   76,     /* DPNSS Enhanced Only */
    WITHDRAW_ACK            =   77,     /* DPNSS Enhanced Only */
    WITHDRAW_NOT_SUPPORTED  =   78,     /* DPNSS Enhanced Only */


    /*-------------------------------------*/
    /* DPNSS Call Back Messaging           */
    /*-------------------------------------*/

    CALL_BACK_MESSAGE_REQ   =   80,     /* DPNSS Enhanced Only */
    CALL_BACK_MESSAGE_CAN   =   81,     /* DPNSS Enhanced Only */
    DPNSS_RAW               =   82,     /* DPNSS Enhanced Only */

    /*-------------------------------------*/
    /* DPNSS Call Back When Next Used      */
    /*-------------------------------------*/

    CBWNU_REQUEST           =   83,


    /* Following added since V5.8: */
    /*-------------------------------------*/
    /* DPNSS Call Back When Free           */
    /*-------------------------------------*/

    CBWF_REQUEST                =   90,
    CBWF_CANCEL                 =   91,
    CBWF_FREE_NOTIFY            =   92,
    RING_OUT                    =   93,
    CALL_BACK_COMPLETE          =   94,
    CBWF_CALL_SETUP_IMMEDIATE   =   95,
    CBWF_CALL_SETUP_DELAYED     =   96,


    /*-------------------------------------*/
    /* DPNSS Night Service Diversion       */
    /*-------------------------------------*/

    NIGHT_SERVICE_DIVERT        =   97,
    NIGHT_SERVICE_DIVERTING     =   98,
    NIGHT_SERVICE_DIVERTED      =   99,

    /*---------------------------------------*/
    /* DPNSS Number Presentation Restriction */
    /*---------------------------------------*/

    NPR_A_PARTY_SUFFIX_B    =   100,

    /*---------------------------------------*/
    /* DPNSS Bearer Service Selection  */
    /*---------------------------------------*/

    BEARER_SERVICE_SELECTION_P  =   101,
    BEARER_SERVICE_SELECTION_M  =   102,

    /*-----------------------------------------------------*/
    /* DPNSS Send or receive a connect in connected state  */
    /*-----------------------------------------------------*/

    CONNECTED_CCM   =   103,

    /*-----------------------------------------------------*/
    /* DPNSS Section 22 redirect messages  */
    /*-----------------------------------------------------*/

    DIVERT_ON_FAILURE = 104,
    REDIRECTION_BYPASS = 105,
    REDIRECTION_ON_FAILURE = 106,


    /*-----------------------------------------------*/
    /* Message to be used with q'ing to show q empty */
    /*-----------------------------------------------*/

    DPNSS_Q_EMPTY = 107,

    /*---------------------------------------------------------------*/
    /* Message to set the drivers stored CLI for the DPNSS end point */
    /*---------------------------------------------------------------*/

    SET_CLI = 108,
    
    /*----------------------------------------------------------------------------------------------*/
    /* Message to notify the application that a RMC has been met with a RRM (recall Reject Message) */
    /*----------------------------------------------------------------------------------------------*/

    RECALL_REJECT_MESSAGE = 109,

    /*-----------------------------------------------------------------------*/
    /* Message to notify the application that a RMC has been met with a NAM  */
    /*-----------------------------------------------------------------------*/

    RECALL_NAM_MESSAGE = 110,

    /*---------------------------------*/
    /* DPNSS layer 2 states / commands */
    /*---------------------------------*/

    DPNS_L2_ENABLE          =   1,      /* DPNSS Enhanced Only */
    DPNS_L2_DISABLE         =   2,      /* DPNSS Enhanced Only */

    /*--------------------------------------*/
    /* dpnss Calling/Called Line Categories */
    /*--------------------------------------*/

    NO_CLC                  =   0,          /* DPNSS Enhanced Only */
    ORDINARY                =   1,          /* DPNSS Enhanced Only */
    DECADIC                 =   2,          /* DPNSS Enhanced Only */
    DASS2                   =   3,          /* DPNSS Enhanced Only */
    PSTN                    =   4,          /* DPNSS Enhanced Only */
    MF5                     =   5,          /* DPNSS Enhanced Only */
    OPERATOR                =   6,          /* DPNSS Enhanced Only */
    NETWORK                 =   7,          /* DPNSS Enhanced Only */
    CONFERENCE              =   8,          /* DPNSS Enhanced Only */

    /*---------------------------------------*/
    /* CLI presentation indicators for Q.931 */
    /*---------------------------------------*/

    PR_ALLOWED              =   0,
    PR_RESTRICTED           =   1,
    PR_NOTAVAILABLE         =   2,
    PR_RESERVED             =   3,

    /*---------------------------------------*/
    /* CLI Screening indicators for Q.931    */
    /*---------------------------------------*/

    SC_NOTSCREENED          =   0,
    SC_VERIFYPASS           =   1,
    SC_VERIFYFAIL           =   2,
    SC_NETWORKPROVIDED      =   3,

    /*---------------------------------------*/
    /* Keypad locations                      */
    /*---------------------------------------*/

    KEYPAD_GLOBAL           =   0,
    KEYPAD_CALL_REF         =   1,
    KEYPAD_CONNECT          =   2,

    /*-------------------------------------*/
    /* state values                        */
    /*-------------------------------------*/
    CS_IDLE                 =   0x00000000,
    CS_WAIT_FOR_INCOMING    =   0x00000001,
    CS_INCOMING_CALL_DET    =   0x00000002,
    CS_CALL_CONNECTED       =   0x00000004,
    CS_WAIT_FOR_OUTGOING    =   0x00000008,
    CS_OUTGOING_RINGING     =   0x00000010,
    EV_INCOMING_DETAILS     =   0x00000020, /* DUPLICATE */
    EV_CALL_CHARGE          =   0x00000021,
    /*
    spare                       0x00000040
    */
    CS_EMERGENCY_CONNECT    =   0x00000080,
    CS_TEST_CONNECT         =   0x00000100,
    /*
    spare                       0x00000200
    */
    CS_REMOTE_DISCONNECT    =   0x00000400,
    CS_WAIT_FOR_ACCEPT      =   0x00000800,
    CS_PROGRESS             =   0x00001000,
    CS_OUTGOING_PROCEEDING  =   0x00002000,
    EV_NOTIFY               =   0x00004000,
    CS_INFO                 =   0x00008000,
    CS_HOLD                 =   0x00010000,
    CS_HOLD_REJECT          =   0x00020000,
    CS_TRANSFER_REJECT      =   0x00040000,
    CS_RECONNECT_REJECT     =   0x00080000,
    EV_CHARGE_INT           =   0x00100000,
    /*-----------------------------------------------------*/
    /* Extension to indicate new events in extended_state  */
    /*-----------------------------------------------------*/
    CS_EXTENDED             =   0x00200000,
    /*
    spare                       0x00400000
    spare                       0x00800000
    */
    CS_DPNS_TRANSIT         =   0x01000000,         /* DPNSS Enhanced Only */
    CS_DPNS_IN_TRANSIT      =   0x02000000,         /* DPNSS Enhanced Only */
    CS_DPNS_HOLDING         =   0x04000000,         /* DPNSS Enhanced Only */
    CS_DPNS_HELD            =   0x08000000,         /* DPNSS Enhanced Only */
    CS_DPNS_CONFERENCE      =   0x10000000,         /* DPNSS Enhanced Only */
    CS_DPNS_INTRUDING       =   0x20000000,         /* DPNSS Enhanced Only */
    EV_DPNS_IN_TRANSIT      =   0x40000000,         /* DPNSS Enhanced Only */

    EV_MEDIA                                = 0x00000101,       /* IP only */
    EV_MEDIA_PROPOSE                        = 0x00000102,       /* IP only */
    EV_MEDIA_REJECT_PROPOSAL                = 0x00000103,       /* IP only */
    EV_MEDIA_REQUEST_PROPOSAL               = 0x00000104,       /* IP only */
    EV_MEDIA_REJECT_REQUEST_PROPOSAL        = 0x00000105,       /* IP only */
    EV_INSUFFICIENT_MEDIA_RESOURCE_FOR_CALL = 0x00000106,       /* IP only */
    EV_SIP_MEMORY_LIMIT_EXCEEDED            = 0x00000107,       /* IP only */
    EV_SIP_MEMORY_LIMIT_WARNING             = 0x00000108,       /* IP only */
    EV_SIP_MEMORY_OK                        = 0x00000109,       /* IP only */
    EV_SIP_RSS_PEER_CONNECTED               = 0x0000010A,       /* IP only */
    EV_SIP_RSS_SERVER_LOST                  = 0x0000010B,       /* IP only */
    EV_SIP_RSS_HEARTBEAT_ESTABLISHED        = 0x0000010C,       /* IP only  - internal event - not raised to application */
    EV_SIP_RSS_HEARTBEAT_LOST               = 0x0000010D,       /* IP only  - internal event - not raised to application */
    EV_SIP_RSS_MAINTENANCE_STARTED          = 0x0000010E,       /* IP only */
    EV_SIP_RSS_MAINTENANCE_COMPLETED        = 0x0000010F,       /* IP only */
    EV_SIP_RSS_MAINTENANCE_TIMEOUT          = 0x00000110,       /* IP only */
    EV_SIP_RSS_SERVER_CONFIG_ERROR          = 0x00000111,       /* IP only */
    EV_SIP_RSS_SERVER_HALT                  = 0x00000112,       /* IP only */
    EV_SIP_RSS_CALL_MIGRATION               = 0x00000113,       /* IP only */
    EV_SIP_RSS_CALL_STATE_CHECK_REQUESTED   = 0x00000114,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_CALL_LOST                    = 0x00000115,       /* IP only */
    EV_SIP_RSS_UNCONFIGURED_SERVER_DETECTED = 0x00000116,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_SERVER_CONFIGURED            = 0x00000117,       /* IP only */
    EV_SIP_RSS_MAINTENANCE_REQUIRED         = 0x00000118,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_TAKEOVER_STARTED             = 0x00000119,       /* IP only */
    EV_SIP_WAIT_FOR_SUBSCRIBER              = 0x0000011A,       /* IP only */
    EV_SIP_WAIT_FOR_NOTIFIER                = 0x0000011B,       /* IP only */
    EV_SIP_SUBSCRIBED                       = 0x0000011C,       /* IP only */
    EV_SIP_SUBSCRIPTION_PENDING             = 0x0000011D,       /* IP only */
    EV_SIP_SUBSCRIPTION_CANCELLED           = 0x0000011E,       /* IP only */
    EV_SIP_SUBSCRIPTION_REQUEST             = 0x0000011F,       /* IP only */
    EV_SIP_RSS_TAKEOVER_COMPLETED           = 0x00000120,       /* IP only */
    EV_SIP_RSS_FINALISE_MAINTENANCE         = 0x00000121,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_CALL_REOPENED                = 0x00000122,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_CONGESTION                   = 0x00000123,       /* IP only - internal event - not raised to application */
    EV_SIP_RSS_PROCESS_ATTACHED             = 0x00000124,       /* IP only - internal event - not raised to application */
    EV_MEDIA_REJECT_COLLISION               = 0x00000125,       /* IP only */
    EV_SIP_SUBSCRIPTION_NOTIFICATION        = 0x00000126,       /* IP only */
    EV_SIP_SUBSCRIPTION_REFRESH             = 0x00000127,       /* IP only */
    EV_SIP_SUBSCRIPTION_REFRESH_FAILED      = 0x00000128,       /* IP only */
    EV_SIP_RSS_IPTAKEOVER_SUCCESSFUL        = 0x00000129,       /* IP only */
    EV_FORKED                               = 0x0000012A,

    /*-------------------------------------*/
    /* event values                        */
    /*-------------------------------------*/

    EV_IDLE                 =   CS_IDLE,
    EV_WAIT_FOR_INCOMING    =   CS_WAIT_FOR_INCOMING,
    EV_INCOMING_CALL_DET    =   CS_INCOMING_CALL_DET,
    EV_CALL_CONNECTED       =   CS_CALL_CONNECTED,
    EV_EMERGENCY_CONNECT    =   CS_EMERGENCY_CONNECT,
    EV_TEST_CONNECT         =   CS_TEST_CONNECT,
    EV_WAIT_FOR_OUTGOING    =   CS_WAIT_FOR_OUTGOING,
    EV_OUTGOING_RINGING     =   CS_OUTGOING_RINGING,
    EV_REMOTE_DISCONNECT    =   CS_REMOTE_DISCONNECT,
    EV_WAIT_FOR_ACCEPT      =   CS_WAIT_FOR_ACCEPT,
    EV_HOLD                 =   CS_HOLD,
    EV_HOLD_REJECT          =   CS_HOLD_REJECT,
    EV_TRANSFER_REJECT      =   CS_TRANSFER_REJECT,
    EV_RECONNECT_REJECT     =   CS_RECONNECT_REJECT,
    EV_PROGRESS             =   CS_PROGRESS,
    EV_OUTGOING_PROCEEDING  =   CS_OUTGOING_PROCEEDING,
    EV_DETAILS              =   EV_INCOMING_DETAILS,
    EV_EXTENDED             =   CS_EXTENDED,

    /*-------------------------------------*/
    /* DPNSS Enhanced Events               */
    /*-------------------------------------*/

    EV_DPNS_TRANSIT     =   CS_DPNS_TRANSIT,        /* DPNSS Enhanced Only */
    EV_DPNS_HOLDING     =   CS_DPNS_HOLDING,        /* DPNSS Enhanced Only */
    EV_DPNS_HELD        =   CS_DPNS_HELD,           /* DPNSS Enhanced Only */
    EV_DPNS_CONFERENCE  =   CS_DPNS_CONFERENCE,     /* DPNSS Enhanced Only */
    EV_DPNS_INTRUDING   =   CS_DPNS_INTRUDING,      /* DPNSS Enhanced Only */

    /*-----------------------------------------------*/
    /* Extended Events - look in - extended_state    */
    /*-----------------------------------------------*/
    EV_EXT_NO_EXTENDED_EVENT    =   0x00000000,
    EV_EXT_FACILITY             =   0x00000001,
    EV_EXT_UUI_PENDING          =   0x00000002,
    EV_EXT_UUI_CONGESTED        =   0x00000004,
    EV_EXT_UUI_UNCONGESTED      =   0x00000008,
    EV_EXT_UUS_SERVICE_REQUEST  =   0x00000010,
    EV_EXT_HOLD_REQUEST         =   0x00000020,
    EV_EXT_RECONNECT_REQUEST    =   0x00000040,
    EV_EXT_TRANSFER_INFORMATION =   0x00000080,
    EV_EXT_DIVERSION            =   0x00000100, /* Added since V5.8 */
    EV_EXT_NON_STANDARD_DATA    =   0x00000200,
    EV_EXT_RAW_DATA             =   0x00000400,
    EV_EXT_RAW_MSG              =   0x00000800,
    EV_EXT_CALL_WAITING         =   0x00001000,
    EV_EXT_FEATURE_NOTIFY       =   0x00080000,
    EV_EXT_FEATURE_ACTIVATION   =   0x00100000,
    EV_EXT_INFORMATION_REQUEST  =   0x00200000,
    EV_EXT_MEDIA_CHANGE_REQUEST       = 0x00400000,
    EV_EXT_MEDIA_CHANGE_ACCEPT        = 0x00400001,
    EV_EXT_MEDIA_CHANGE_REJECT        = 0x00400002,
    EV_EXT_MEDIA_CHANGE_TIMEOUT       = 0x00400003,
    EV_EXT_MEDIA_CHANGE_COMPLETED     = 0x00400004,

    /*-------------------------------------*/
    /* Feature Indications                 */
    /*-------------------------------------*/
    FEATURE_FACILITY        =   0x00000001,
    FEATURE_USER_USER       =   0x00000002,
    FEATURE_DIVERSION       =   0x00000004,
    FEATURE_MESSAGE_WAITING =   0x00000008,
    FEATURE_REGISTER        =   0x00000010,
    FEATURE_KEYPAD          =   0x00000020,
    FEATURE_DISPLAY         =   0x00000040,
    FEATURE_VIRTUAL         =   0x00000080,
    FEATURE_HOLD_RECONNECT  =   0x00000100,
    FEATURE_TRANSFER        =   0x00000200,
    FEATURE_RAW_DATA        =   0x00000400,
    FEATURE_NON_STANDARD    =   0x00000800,
    FEATURE_MLPP            =   0x00001000,
    FEATURE_RAW_MSG         =   0x00002000,
    FEATURE_CALL_WAITING    =   0x00004000,
    FEATURE_RESTART_CHANNELS    =   0x00008000,
    FEATURE_NSM_RAS             =   0x00010000,
    FEATURE_XRS                 =   0x00020000,
    FEATURE_CONNECTIONLESS_FACILITY =   0x00040000,
    FEATURE_NOTIFY                  =   0x00080000, 
    FEATURE_FEATURE_ACTIVATION      =   0x00100000, 
    FEATURE_INFORMATION_REQUEST     =   0x00200000,
    FEATURE_NAME_PRESENTATION       =   0x00400000,

    /*-------------------------------------*/
    /* DPNSS layer 2 states                */
    /*-------------------------------------*/

    DPNS_L2_NO_STATE        =   0x00,       /* DPNSS Enhanced Only */
    DPNS_L2_ENABLED         =   0x01,       /* DPNSS Enhanced Only */
    DPNS_L2_DISABLED        =   0x02,       /* DPNSS Enhanced Only */

    /*------------------------------*/
    /*  Layer 3 to Data Link Layer  */
    /*------------------------------*/
    DL_ESTABLISH_REQ        =   1,          /* Q921 Driver Only */
    DL_RELEASE_REQ          =   2,          /* Q921 Driver Only */
    DL_DATA_REQ             =   3,          /* Q921 Driver Only */
    DL_UNIT_DATA_REQ        =   4,          /* Q921 Driver Only */

    /*------------------------------*/
    /*  Data Link Layer to Layer 3  */
    /*------------------------------*/
    DL_ESTABLISH_IND        =   5,          /* Q921 Driver Only */
    DL_RELEASE_IND          =   6,          /* Q921 Driver Only */
    DL_UNIT_DATA_IND        =   7,          /* Q921 Driver Only */
    DL_DATA_IND             =   8,          /* Q921 Driver Only */
    DL_ESTABLISH_CON        =   9,          /* Q921 Driver Only */
    DL_RELEASE_CON          =   10,         /* Q921 Driver Only */

    /*-------------------------*/
    /* numbering type          */
    /*-------------------------*/
    NT_UNKNOWN              =   0,
    NT_INTERNATIONAL        =   1,
    NT_NATIONAL             =   2,
    NT_NETWORK_SPECIFIC     =   3,
    NT_SUBSCRIBER_NUMBER    =   4,
    NT_ABBREVIATED_NUMBER   =   6,

    /*-------------------------*/
    /* numbering plan          */
    /*-------------------------*/

    NP_UNKNOWN              =   0,
    NP_ISDN                 =   1,
    NP_DATA                 =   3,
    NP_TELEX                =   4,
    NP_NATIONAL_STANDARD    =   8,
    NP_PRIVATE              =   9,

    /*-------------------------*/
    /* Nature of address (ISUP)*/
    /*-------------------------*/

    NOA_SUBSCRIBER_NUMBER   =   1,
    NOA_NATIONAL_RESERVED   =   2,
    NOA_NATIONAL            =   3,
    NOA_INTERNATIONAL       =   4,

    /*------------------------------*/
    /* Calling Party Category (ISUP)*/
    /*------------------------------*/

    CPC_FRENCH_OPERATOR     =   1,
    CPC_ENGLISH_OPERATOR    =   2,
    CPC_GERMAN_OPERATOR     =   3,
    CPC_RUSSIAN_OPERATOR    =   4,
    CPC_SPANISH_OPERATOR    =   5,
    CPC_ORDINARY_SUBSCRIBER =   10,
    CPC_PRIORITY_SUBSCRIBER =   11,
    CPC_DATA                =   12,
    CPC_TEST                =   13,
    CPC_PAYPHONE            =   15,

    /*--------------------------------------------------------*/
    /* Continuity Check Indicator (ISUP continuity_check_ind) */
    /*--------------------------------------------------------*/

    CCI_NOT_REQUIRED        =   0,
    CCI_REQUIRED            =   1,
    CCI_PREVIOUS            =   2,
    CCI_CCR_TEST            =   4,  /* CCR test call */

    /*------------------------------*/
    /* ISUP Preference Indicators   */
    /*------------------------------*/

    PI_ISUP_PREFERRED       =   0,
    PI_ISUP_NOT_REQUIRED    =   1,
    PI_ISUP_REQUIRED        =   2,

    /*------------------------------*/
    /* ISUP Exchange Type           */
    /*------------------------------*/
    EXCHANGE_TYPE_A         =   1,
    EXCHANGE_TYPE_B         =   2,

    /*--------------------------------*/
    /* Port blocking reason codes,    */
    /* Meanings are protocol-specific */
    /* ISUP Blocking Reasons          */
    /*--------------------------------*/

    ISUP_MAINTENANCE_BLOCK  =   0x0,
    ISUP_HW_BLOCK           =   0x1,

    /* Generic flag settings valid for acuc_maintenance_xparms */
    ACUC_MAINT_SYNC         =   0x80,  /* Command is to execute synchronously */
    ACUC_MAINT_EVENT        =   0x40,  /* Raise port event on completion */



    /*--------------------------------*/
    /* Blocking Flags                 */
    /*--------------------------------*/

    /* Applicable to ETS300 only*/
    ACU_MAINT_ETS_D_CHAN            = 0x02, /* Block or unblock the ETS300 D-Channel by */
                                            /* removing or reestablishing layer 2.      */

    /* Applicable to QSIG only RFC2562 */
    ACU_MAINT_QSIG_D_CHAN           = 0x02, /* Block or unblock the QSIG D-Channel by */
                                            /* removing or reestablishing layer 2.      */

    /* Applicable to ETS300 and QSIG */
    ACU_MAINT_BLOCK_B_CHAN          = 0x03, /* Block or unblock B-Channels */


    /* Applicable to AT&T, NI-2 & DMS-100 */
    ACU_MAINT_SERVICE_BLOCKING      = 0x04, /* Use Layer 3 Service messages to change status of timeslots */

    /*----------------------------------*/
    /* port_status_xparms.rqst values   */
    /*----------------------------------*/

    ACU_MAINT_PORT_GET_BLOCK_STATE          = 0,
    ACU_MAINT_PORT_GET_RESET_STATE          = 1,
    ACU_MAINT_PORT_GET_SERVICE_STATE        = 2,
    ACU_MAINT_PORT_GET_APPLICATION_REF      = 3,
    /* ISUP circuit values, returned 8 bits at a time */
    ACU_MAINT_PORT_GET_CIC_0_7              = 4,   /* CIC number */
    ACU_MAINT_PORT_GET_CIC_8_15             = 5,
    ACU_MAINT_PORT_GET_CIC_16_23            = 6,   /* For BICC support */
    ACU_MAINT_PORT_GET_CIC_24_31            = 7,   /* Not yet implemented */
    ACU_MAINT_PORT_GET_LPC_0_7              = 8,   /* Local pointcode */
    ACU_MAINT_PORT_GET_LPC_8_15             = 9,
    ACU_MAINT_PORT_GET_LPC_16_23            = 10,
    ACU_MAINT_PORT_GET_RPC_0_7              = 11,  /* Remote pointcode */
    ACU_MAINT_PORT_GET_RPC_8_15             = 12,
    ACU_MAINT_PORT_GET_RPC_16_23            = 13,  

    /*--------------------------------------------------------*/
    /* port_status_xparms.ts_info block/reset response bits   */
    /*--------------------------------------------------------*/

    ACU_MAINT_PORT_BLOCKED_LOC_MAINT        = 0x01,
    ACU_MAINT_PORT_BLOCKED_LOC_HW           = 0x02,
    ACU_MAINT_PORT_BLOCKED_REM_MAINT        = 0x04,
    ACU_MAINT_PORT_BLOCKED_REM_HW           = 0x08,
    ACU_MAINT_PORT_BLOCKED_UCIC             = 0x10,
    ACU_MAINT_PORT_BLOCKED_BEFORE_RESET     = 0x20,  /* Reset in progress */
    ACU_MAINT_PORT_UNBLOCK_IN_PROGRESS      = 0x40,
    ACU_MAINT_PORT_BLOCK_IN_PROGRESS        = 0x80,
    ACU_MAINT_PORT_RESET_INWARDS            = 0x40,
    ACU_MAINT_PORT_RESET_IN_PROGRESS        = 0x80,


    // RFC 2792. Coding Standards (CST) for clearing cause
    CST_ITUT                    =    0,
    CST_NATIONAL                =    2,
    CST_NETWORK_SPECIFIC        =    3,

    /*--------------------------------*/
    /* clearing cause FROM the driver */
    /*--------------------------------*/

    LC_NORMAL                   =   0,
    LC_NUMBER_BUSY              =   1,
    LC_NO_ANSWER                =   2,
    LC_NUMBER_UNOBTAINABLE      =   3,
    LC_NUMBER_CHANGED           =   4,
    LC_OUT_OF_ORDER             =   5,
    LC_INCOMING_CALLS_BARRED    =   6,
    LC_CALL_REJECTED            =   7,
    LC_CALL_FAILED              =   8,
    LC_CHANNEL_BUSY             =   9,
    LC_NO_CHANNELS              =   10,
    LC_CONGESTION               =   11,
    LC_TCP_CONNECT_FAILED           =   12,
    LC_SSL_ERROR                    =   13,
    LC_SSL_PEER_CERT_NOT_TRUSTED    =   14,
    LC_SSL_PEER_CERT_INVALID        =   15,

    /*--------------------------------*/
    /* clearing cause FROM the driver */
    /*--------------------------------*/

    AC_NORMAL           =   0,      /* default acceptance code      */
    AC_CHARGE           =   100,    /* answer call with charging    */
    AC_NOCHARGE         =   101,    /* answer call without charging */
    AC_LAST_RELEASE     =   102,    /* last party release           */
    AC_SPARE1           =   103,    /* spare                        */
    AC_SPARE2           =   104,    /* spare                        */

    /*-------------------------------------*/
    /* layer 1 stat values                 */
    /*-------------------------------------*/

    LSTAT_NOS       =   0x8000,     /* Received Signal Lost */
    LSTAT_LOS       =   0x4000,     /* Lost Synchronisation */
    LSTAT_AIS       =   0x2000,     /* Alarm in Service */
    LSTAT_FEC       =   0x1000,     /* Error Rate > 1 in 1000 bits average */
    LSTAT_RRA       =   0x0800,     /* Received Remote Alarm */
    LSTAT_SLP       =   0x0400,     /* Frame Slip in Receive Buffer */
    LSTAT_CVC       =   0x0200,     /* Bipolar Violation Detected */
    LSTAT_CRC       =   0x0100,     /* CRC Error */
    LSTAT_FFA       =   0x0080,     /* False Frame Alignment detected */
    LSTAT_CML       =   0x0040,     /* CAS Multiframe Lost */

    /*-------------------------------------*/
    /* layer 1 alarm values                */
    /*-------------------------------------*/


    ALARM_NONE      =   0x0000,
    ALARM_AIS       =   LSTAT_AIS,
    ALARM_RRA       =   LSTAT_RRA,
    ALARM_CML       =   LSTAT_CML,

    /*-------------------------------------*/
    /* DSP Configurations                  */
    /*-------------------------------------*/

    DSPA    =   0x01,    /* Rev4/5 Lucent DSP fitted */
    DSPB    =   0x02,

    DSP1A   =   0x01,  /* BR4/4e/8 Lucent DSP fitted */
    DSP1B   =   0x02,
    DSP2A   =   0x04,
    DSP2B   =   0x08,

    /*-------------------------------------*/
    /* DPR Memory Access Macros            */
    /*-------------------------------------*/

    /*-------------------------*/
    /* UUS services/protocols  */
    /*-------------------------*/
    /* UU Service Request Types */
    UUS_1_IMPLICITLY_PREFERRED      =   (1<<0),
    UUS_1_REQUIRED                  =   (1<<1),
    UUS_1_PREFERRED                 =   (1<<2),
    UUS_2_REQUIRED                  =   (1<<3),
    UUS_2_PREFERRED                 =   (1<<4),
    UUS_3_REQUIRED                  =   (1<<5),
    UUS_3_PREFERRED                 =   (1<<6),

    /* UU Service Response Types */
    UUS_1_ACCEPTED                  =   (1<<1),
    UUS_1_REFUSED                   =   (1<<2),
    UUS_1_REFUSED_BY_PEER           =   (1<<3),
    UUS_1_REFUSED_BY_NET            =   (1<<4),
    UUS_2_ACCEPTED                  =   (1<<5),
    UUS_2_REFUSED                   =   (1<<6),
    UUS_2_REFUSED_BY_PEER           =   (1<<7),
    UUS_2_REFUSED_BY_NET            =   (1<<8),
    UUS_3_ACCEPTED                  =   (1<<9),
    UUS_3_REFUSED                   =   (1<<10),
    UUS_3_REFUSED_BY_PEER           =   (1<<11),
    UUS_3_REFUSED_BY_NET            =   (1<<12),

    /* UU Protocol Types */
    UUI_PROTOCOL_USER_SPECIFIC      =   0,
    UUI_PROTOCOL_OSI_HIGHER_LAYER   =   1,
    UUI_PROTOCOL_CCITT_X244         =   2,
    UUI_PROTOCOL_SYSMNG_CONV        =   3,
    UUI_PROTOCOL_IA5                =   4,
    UUI_PROTOCOL_CCITT_V120         =   7,
    UUI_PROTOCOL_CCITT_Q931         =   8,


    /* UU Flow Control  */
    UUI_FC_STOP_SENDING             =   (1<<0),
    UUI_FC_DATA_DISCARDED           =   (1<<1),

    /* UU Commands  */
    UU_SERVICE_CMD                  =   1,
    UU_DATA_CMD                     =   2,
    UU_GET_PENDING_DATA_CMD         =   3,

    UUI_CONTROL_CCM_DATA            =   (1<<0),

    /* UU ATT Commands */
    UUS_ATT_MA_UUI                  =   1,  /* Message Associated (user IE) */
    UUS_ATT_CA_TSC                  =   2,  /* Call Associated (user message) */
    UUS_ATT_NCA_TSC                 =   3,  /* Non Call Associated (user message) */
    UUS_ATT_NCA_TSC_NSF_SDN         =   11, /* Request for NSF NCA - Software Defined Network (SDN) */
    UUS_ATT_NCA_TSC_NSF_ACCUNET     =   12, /* Request for NSF NCA - ACCUNET */
    UUS_ATT_CA_TSC_CLEAR            =   4,  /* Clear CA-TSC */
    UUS_ATT_NCA_TSC_CLEAR           =   5,  /* Clear NCA-TSC */
    UUS_ATT_GET_DATA                =   6,  /* Get pending AT&T user-user data */
    UUS_ATT_CA_TSC_CMD              =   7,  /* Send a TSC CMD - ack/rej of request */
    UUS_ATT_CA_TSC_DATA             =   8,  /* Send TSC Data */
    UUS_ATT_NCA_TSC_CMD             =   9,  /* Send a TSC CMD - ack/rej of request */
    UUS_ATT_NCA_TSC_DATA            =   10, /* Send TSC Data */
    UUS_ATT_CONGESTION_RR           =   13,
    UUS_ATT_CONGESTION_RNR          =   14,

    UUI_NO_DATA_IN_MESSAGE          = 0xFF,

    /* UU AT&T Feature tx/rx responses: */
    ATNT_UUS_CA_TSC_ACCEPTED        =   1,
    ATNT_UUS_CA_TSC_REJECTED        =   2,
    ATNT_UUS_NCA_TSC_ACCEPTED       =   3,
    ATNT_UUS_NCA_TSC_REJECTED       =   4,



    /* MLPP Precedence levels */
    MLPP_Prec_level_flashOverride   =   0,
    MLPP_Prec_level_flash           =   1,
    MLPP_Prec_level_immediate       =   2,
    MLPP_Prec_level_priority        =   3,
    MLPP_Prec_level_routine         =   4,

    /* MLPP Look ahead For Busy Indicators */
    MLPP_LFB_Indictn_lfbAllowed     =   0,
    MLPP_LFB_Indictn_lfbNotAllowed  =   1,
    MLPP_LFB_Indictn_pathReservd    =   2,

    /* MLPP Status Request field */
    MLPP_StatusRequest_successCalledUserMLPPSubscriber      =   1,
    MLPP_StatusRequest_successCalledUserNotMLPPSubscriber   =   2,
    MLPP_StatusRequest_failureCaseA                         =   3,
    MLPP_StatusRequest_failureCaseB                         =   4,

    /* MLPP Error field */
    MLPP_Error_userNotSubscribed            =   0,
    MLPP_Error_rejectedByNetwork            =   1,
    MLPP_Error_unauthorizedPrecedenceLevel  =   44,

    /* MLPP Preempt field */
    MLPP_preempt_circuitReservedForReuse    =   1,
    MLPP_preempt_circuitNotReservedForReuse =   2,


    /* Settings for 'control' fields */
    /* Info is sent immediately */
    CONTROL_DEFAULT                 =   0,
    /* send in next ALERT, CONNECT, DISCONNECT, RELEASE message */
    CONTROL_NEXT_CC_MESSAGE         =   1,
    /* More information will be sent in this message - don't transmit yet */
    CONTROL_DEFERRED                =   2,
    CONTROL_DEFERRED_SETUP          =   3,
    /* extra information (after CONTROL_DEFERRED) - don't transmit yet */
    CONTROL_EXTRA_INFO              =   4,
    CONTROL_EXTRA_INFO_SETUP        =   5,
    /* extra information (after CONTROL_DEFERRED/CONTROL_EXTRA_INFO)- send now */
    CONTROL_LAST_INFO               =   6,
    CONTROL_LAST_INFO_SETUP         =   7,
    /* FEATURE_RAW_MSG specified a whole message. More information
     * to follow */
    CONTROL_DEFERRED_MESSAGE        =   8,

    /* Facility Commands For 'call_send_connectionless()'  */
    FAC_CLESS_DL_DATA_CMD           =   1,
    FAC_CLESS_DL_UNIT_DATA_CMD      =   2,
    /* Facility Commands For proprietary transfer mechanism with Hicom switch
    * for use on EuroISDN with a Hicom switch (of course) - This is the
    * second stage of a two part operation
    * Part 1 - send keypad 'R' for Hold
    * Part 2 call_feature_send - facility - command= FAC_HICOM_TRANSFER -
    *  destination_addr = 'transfered to' number
    */
    FAC_HICOM_TRANSFER              =   3,

    /* Commands for use with FEATURE_HOLD_XPARMS */
    HOLD_ACKNOWLEDGE_CMD                =   1,
    HOLD_REJECT_CMD                     =   2,
    RECONNECT_ACKNOWLEDGE_CMD           =   3,
    RECONNECT_REJECT_CMD                =   4,

    FE_LINKID_NOT_ASSIGNED_BY_NETWORK   =   21,

    /* This error code is to be used when rejecting
     * a link ID request message  See Appendix K of
     * the call control guide for more information */
    FE_RESOURCE_UNAVAILABLE         =   11,

    /* Operation Types - used with FEATURE_TRANSFER_XPARMS */
    /* EuroISDN                                            */
    OP_EXPLICIT_ECT_EXECUTE         =   1,
    OP_ECT_LINK_ID_REQUEST          =   4,
    OP_ECT_EXECUTE                  =   7,
    OP_ECT_SETUP                    =   10,
    OP_ECT_ABANDON                  =   13,

    /* Values for QSIG and H.323 call diversion - used with DIVERSION_XPARMS */
    OP_CALL_REROUTE_REQ             =   20,
    OP_DIVERTING_LEG_INFO1          =   27,
    OP_DIVERTING_LEG_INFO2          =   21,
    OP_DIVERTING_LEG_INFO3          =   22,

    /* H.323 call diversion values */
    OP_ACTIVATE_DIVERSION           =   23,
    OP_DEACTIVATE_DIVERSION         =   24,
    OP_INTERROGATE_DIVERSION        =   25,

    /* Operation value for mlpp - used with MLPP_XPARMS */
    OP_MLPP_CALL_REQUEST            =   25,
    OP_MLPP_CALL_PREEMPT            =   26,

    /* QSIG Name Presentation Operation - used with NAME_PRESENTATION_XPARMS */
    OP_NAME_PRESENTATION_CALLING    =   0,
    OP_NAME_PRESENTATION_CALLED     =   1,
    OP_NAME_PRESENTATION_CONNECTED  =   2,
    OP_NAME_PRESENTATION_BUSY       =   3,

    /* QSIG Name Presentation Type - used with NAME_PRESENTATION_XPARMS */
    NAME_PRESENTATION_TYPE_ALLOWED_SIMPLE       = 0,
    NAME_PRESENTATION_TYPE_ALLOWED_EXTENDED     = 1,
    NAME_PRESENTATION_TYPE_RESTRICTED_SIMPLE    = 2,
    NAME_PRESENTATION_TYPE_RESTRICTED_EXTENDED  = 3,
    NAME_PRESENTATION_TYPE_NAME_NOT_AVAILABLE   = 4,
    NAME_PRESENTATION_TYPE_RESTRICTED_NULL      = 7,

    /* Operation Values */
    INVOKE                          =   1,
    RETURN_RESULT                   =   2,
    RETURN_ERROR                    =   3,

    /* Non standard data id types */
    NON_STANDARD_ID_TYPE_H221       =   1,
    NON_STANDARD_ID_TYPE_OBJECT     =   2,

    /*
     * Feature errors
     *
     *  These error codes are to be used when rejecting
     *  a transfer message  See Appendix K of the call
     *  control guide for more information
     */
    FE_NOT_SUBSCRIBED               =   0,
    FE_NOT_AVAILABLE                =   3,
    FE_INVALID_CALL_STATE           =   7,
    FE_SS_INTERACTION_NOT_ALLOWED   =   10,


    ACU_CALL_EVT_NO_EVENT                   =   0,
    ACU_CALL_EVT_L1_CHANGE                  =   1,
    ACU_CALL_EVT_L2_CHANGE                  =   2,
    ACU_CALL_EVT_HW_CLOCK_STOP              =   3,
    ACU_CALL_EVT_CONNECTIONLESS             =   4,
    ACU_CALL_EVT_FIRMWARE_CHANGE            =   5,
    ACU_CALL_EVT_D_CHAN_SWITCHOVER          =   6,
    ACU_CALL_EVT_PORT_COMMS_LOST            =   7,
    ACU_IPT_EV_ADD_ALIAS_SUCCEEDED          =   8,
    ACU_IPT_EV_ADD_ALIAS_FAILED             =   9,
    ACU_IPT_EV_ALIAS_REMOVED                =   10,
    ACU_CALL_EV_INSUFFICIENT_MEDIA_RESOURCE =   11,
    ACU_SIP_EV_RESPONSE                     =   12,
    ACU_SIP_EV_REQUEST                      =   13,
    ACU_SIP_EV_REQUEST_TIMEOUT              =   14,     /* Deprecated */
    ACU_SIP_EV_REQUEST_FAILED               =   14,
    ACU_CALL_EVT_NO_CHANNEL_AVAILABLE       =   15,
    ACU_CALL_EVT_BLOCKING_STATE_CHANGE      =   16,
    ACU_CALL_EVT_RESET_STATE_CHANGE         =   17,


    ACU_CALL_EVT_TRACE_MODE         =   100,
    ACU_CALL_EVT_TRACE_SNAPSHOT     =   101,

    /*-------------------------------------*/
    /* TRACE FLAGS                         */
    /*-------------------------------------*/
    TRACE_OFF           =   0x00000000, /* Unnecessary, as we always trace to Buff */
    TRACE_LEVEL_LOW     =   0x0,        /* trace to be shown all the time */
    TRACE_LEVEL_MEDIUM  =   0x1,        /* medium level trace */
    TRACE_LEVEL_HIGH    =   0x4,        /* seriously detailed trace */
    TRACE_TO_BUFF       =   0x8,        /* Unnecessary, as we always do it */
    TRACE_PROTOCOL      =   0x10,       /* Trace protocol info */
    TRACE_API_CALL      =   0x20,       /* Trace API calls */
    TRACE_DRIVER_INFO   =   TRACE_LEVEL_MEDIUM, /* All the remaining driver stuff */
    TRACE_EXTREME       =   TRACE_LEVEL_HIGH,   /* trace *everything* */
    TRACE_EVENT         =   0x40,       /* event management/dispatch */
    TRACE_HW            =   0x80,       /* hardware information */
    TRACE_TLS           =   0x100,      /* thin layer trace */
    TRACE_NO_STAMP      =   0x200,      /* Turn off time-stamp & id */
    TRACE_RESOURCE      =   0x400,      /* resource allocation/deallocation */
    TRACE_GENERIC       =   0x800,      /* generic trace */
    TRACE_SYSLOG        =   0x1000,     /* send trace to remote syslog host */
    TRACE_NOWAIT        =   0x2000,     /* exit v6trace when there is no more trace */
    TRACE_NO_FORMAT     =   0x4000,     /* Internal driver use only */

    RESTART_CLASS_SINGLE_INTERFACE  =   1,
    RESTART_CLASS_ALL_INTERFACES    =   2,
    RESTART_CLASS_INDICATED_CHANNEL =   3,  /* included in case this is needed in future */

    /* Layer 1 RX/TX flags */
    PORT_RX_DISABLE                 =   0x01,
    PORT_TX_DISABLE                 =   0x02
};

/** Timeslot selection */
enum {
    USE_ANY_TIMESLOT    =   -1,         /* Call can use any available timeslot */
    /* Apply to ts field to use slotmap */
    USE_SLOTMAP         =   -3          /* timeslot(s) to be specified in slotmap field*/
};

/** Call control error codes */
enum {
    /*-------------------------*/
    /* error codes             */
    /*-------------------------*/

    ERR_HANDLE                      =   ( -2  ),    /* Illegal handle, or out of handles      */
    ERR_COMMAND                     =   ( -3  ),    /* Command not valid in current state     */
    ERR_NET                         =   ( -4  ),    /* Illegal network number                 */
    ERR_RESPONSE                    =   ( -6  ),    /* Application failed to respond within response time */
    ERR_NOCALLIP                    =   ( -7  ),    /* No call in progress                    */
    ERR_TSBAR                       =   ( -8  ),    /* Timeslot barred                        */
    ERR_TSBUSY                      =   ( -9  ),    /* Timeslot busy                          */
    ERR_CFAIL                       =   ( -10 ),    /* Command failed to execute              */
    ERR_SERVICE                     =   ( -11 ),    /* Invalid service code                   */
    ERR_BUFF_FAIL                   =   ( -12 ),    /* Out of buffer resources                */

    /* download errors defined */
    ERR_DNLD_ZAP                    =   ( -13 ),    /* Failed waiting for board bootstrap code to respond.  Check that firmware is appropriate for the hardware. */
    ERR_DNLD_NOCMD                  =   ( -14 ),    /* Firmware not loaded - command denied   */
    ERR_DNLD_NODNLD                 =   ( -15 ),    /* Firmware installed - download denied   */
    ERR_DNLD_GEN                    =   ( -16 ),    /* General failure during download        */
    ERR_DNLD_NOSIG                  =   ( -17 ),    /* Downloaded firmware failed to start    */
    ERR_DNLD_NOEXEC                 =   ( -18 ),    /* Downloaded firmware is not executing   */
    ERR_DNLD_NOCARD                 =   ( -19 ),    /* Downloaded firmware is not executing   */
    ERR_DNLD_SYSTAT                 =   ( -20 ),    /* Downloaded firmware detected an error  */
    ERR_DNLD_BADTLS                 =   ( -21 ),    /* Driver does not support the firmware   */
    ERR_DNLD_POST                   =   ( -22 ),    /* Board failed power on self test        */
    ERR_DNLD_SW                     =   ( -23 ),    /* Switch setup error                     */
    ERR_DNLD_MEM                    =   ( -24 ),    /* Could not allocate memory for download */
    ERR_DNLD_FILE                   =   ( -25 ),    /* Could not find the file to download    */
    ERR_DNLD_TYPE                   =   ( -26 ),    /* The file is not downloadable           */

    /* systat errors defined */

    ERR_LIB_INCOMPAT                =   ( -27 ),    /* Incompatible library used               */
    ERR_DRV_INCOMPAT                =   ( -28 ),    /* Incompatible driver used               */
    ERR_DRV_CALLINIT                =   ( -29 ),    /* Another process attempted to call 'call_init' while other processing are accessing the driver */
    ERR_TS_BLOCKED                  =   ( -30 ),    /* Timeslot blocked */
    /* -31 is used */
    ERR_PORT_BLOCKED                =   ( -32 ),    /* Port blocked */

    /* VoIP error codes */
    ERR_INVALID_ADDR                =   ( -40 ),    /* Tried to access invalid address                */
    ERR_INVALID_PORT                =   ( -41 ),    /* Tried to access invalid port                   */
    ERR_MANAGEMENT_RPC              =   ( -42 ),    /* Failed to startup Management RPC session       */
    ERR_SESSION_RPC                 =   ( -43 ),    /* Failed to startup Session RPC session          */
    ERR_NO_SERVICE                  =   ( -44 ),    /* The service did not respond                    */
    ERR_NO_BOARD                    =   ( -45 ),    /* The board did not respond                      */
    ERR_BOARD_UNLOADED              =   ( -46 ),    /* The board has not been downloaded              */
    ERR_BOARD_VERSION               =   ( -47 ),    /* Board software incompatible with host software */
    ERR_DNLD_CRCERRORS              =   ( -48 ),    /* CRC error detected in board firmware during download */
    /* -50 is used */
    ERR_INVALID_FW_PARM             =   ( -51 ),    /* Unrecognised firmware switch */
    /* -52 is used */
    /* -53 is used */
    ERR_NO_PLUGINS                  =   ( -54 ),    /* Library found no plugins for communicating with drivers */
    ERR_DUPLICATE_PLUGINS_FOUND     =   ( -55 ) ,   /* Call library found multiple plugins with the same id */
    ERR_NO_PORTS                    =   ( -56 ),    /* The call library was unable to find any ports on this card */
    ERR_DNLD_ALREADY_IN_USE         =   ( -57 ),    /* Port is in use by another host */
    ERR_DNLD_NOT_RESPONDING         =   ( -58 ),    /* The port is not responding */
    ERR_NO_FREE_CHANNELS            =   ( -59 ),    /* There are no free channels on this port */
    ERR_INVALID_PLUGIN              =   ( -60 ),    /* An invalid file was found in the call control plugins directory */

    /* transport layer security errors */
    ERR_TLS_UNINITIALISED           =   ( -61 ),    /* The TLS library has not been initialised */
    ERR_TLS_WAITING_FOR_PASSWD      =   ( -62 ),    /* The TLS library is waiting for a password */
    ERR_TLS_BAD_TRUSTED_CERTIFICATE =   ( -63 ),    /* The TLS library has encountered a bad trusted certificate */
    ERR_TLS_BAD_SERVER_CERTIFICATE  =   ( -64 ),    /* The TLS library has encountered a bad server certificate */



    /* trace error codes */
    ERR_ALREADY_TRACED              =   ( -70 ),    /*  Port already being traced by the application */
    ERR_FAILED_CREATING_TRACE_FILE  =   ( -71 ),    /*  Could not create output file */
    ERR_TRACE_PORT_ADDED_FAILED     =   ( -72 ),    /*  Could not add port to list of ports being traced */
    ERR_FAILED_SETTING_TRACE_MODE   =   ( -73 ),    /*  Could not change the port's trace mode */
    ERR_PORT_NOT_BEING_TRACED       =   ( -74 ),    /*  Port Id not being traced */
    ERR_IOCTL_NOT_FINISHED          =   ( -75 ),    /*  Port Id not being traced */

    ERR_UNAVAILABLE                 =   ( -76)      /*  Endpoint cannot be reached */
};

#define DEFAULT_FIRMWARE_STRING "NO F/W"

/* Error "message" for received numbers (e.g. CLI/DDI) that are too long */
#define ERR_NUMBER_OVERRUN "Number Overrun"

/* structures used to pad unions so that we can more easily maintain binary compatibility between versions */
typedef struct tACU_COMPAT_PADDING {
    ACU_CHAR    dummy[0x600];
} ACU_PACK_DIRECTIVE ACU_COMPAT_PADDING;

typedef struct tACU_FEATURE_COMPAT_PADDING {
    ACU_CHAR    dummy[0xA0D];
} ACU_PACK_DIRECTIVE ACU_FEATURE_COMPAT_PADDING;

typedef struct bearer {
    ACU_UCHAR   ie[MAXBEARER];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  BEARER;


typedef struct hilayer {
    ACU_UCHAR   ie[MAXHILAYER];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  HILAYER;

typedef struct lolayer {
    ACU_UCHAR   ie[MAXLOLAYER];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  LOLAYER;

typedef struct progress_indicator {
    ACU_UCHAR   ie[MAXPROGRESS];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  PROGRESS_INDICATOR;


typedef struct notify_indicator {
    ACU_UCHAR   ie[MAXNOTIFY];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  NOTIFY_INDICATOR;

typedef struct keypad {
    ACU_UCHAR   ie[MAXNUM];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  KEYPAD;

typedef struct display {
    ACU_UCHAR   ie[MAXDISPLAY];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  DISPLAY;

typedef struct keypadAsFeature {
    ACU_UCHAR    length;
    KEYPAD       keypad;
} ACU_PACK_DIRECTIVE  KEYPAD_AS_FEATURE;

typedef struct cause {
    ACU_UCHAR   ie[MAXCAUSE];
    ACU_UCHAR   last_msg;
} ACU_PACK_DIRECTIVE  CAUSE;

typedef struct ep {               /* End point addressing */
    ACU_UCHAR    usid;
    ACU_UCHAR    tid;
    ACU_UCHAR    interpreter;
} ACU_PACK_DIRECTIVE  EP;



/*-------------------------------------*/
/* API structure definitions           */
/*-------------------------------------*/


typedef struct uniquex_q931 {
    ACU_UCHAR   service_octet;
    ACU_UCHAR   add_info_octet;
    ACU_UCHAR   dest_numbering_type;
    ACU_UCHAR   dest_numbering_plan;
    BEARER      bearer;              /* V4 ends here */

    ACU_UCHAR   orig_numbering_type;
    ACU_UCHAR   orig_numbering_plan;
    ACU_UCHAR   orig_numbering_presentation;
    ACU_UCHAR   orig_numbering_screening;
    ACU_UCHAR   conn_numbering_type;
    ACU_UCHAR   conn_numbering_plan;
    ACU_UCHAR   conn_numbering_presentation;
    ACU_UCHAR   conn_numbering_screening;

    ACU_UCHAR   dest_subaddr[MAXNUM];
    ACU_UCHAR   orig_subaddr[MAXNUM];

    HILAYER     hilayer;
    LOLAYER     lolayer;
    PROGRESS_INDICATOR  progress_indicator;
    NOTIFY_INDICATOR    notify_indicator;
    KEYPAD      keypad;
    DISPLAY     display;

    ACU_LONG    slotmap;

    EP          endpoint_id;
    ACU_UCHAR   oli_ani_2;
    CAUSE       cause;
    ACU_UCHAR   additional_orig_addr[MAXADDR];
    ACU_UCHAR   add_orig_numbering_type;
    ACU_UCHAR   add_orig_numbering_plan;
    ACU_UCHAR   add_orig_numbering_presentation;
    ACU_UCHAR   add_orig_numbering_screening;
    ACU_UCHAR   omit_calling_party_ie;
    ACU_USHORT  call_ref_value;
    ACU_UCHAR   conn_subaddr[MAXNUM];
} ACU_PACK_DIRECTIVE  UNIQUEX_Q931;


typedef struct uniquex_dass {
    ACU_UCHAR   sic1;
    ACU_UCHAR   sic2;
} ACU_PACK_DIRECTIVE  UNIQUEX_DASS;

typedef struct uniquex_dpnss {
    ACU_UCHAR   sic1;
    ACU_UCHAR   sic2;
    ACU_UCHAR   clc[MAXCLC];
} ACU_PACK_DIRECTIVE  UNIQUEX_DPNSS;

typedef struct uniquex_cas {
    ACU_UCHAR   category;
} ACU_PACK_DIRECTIVE  UNIQUEX_CAS;


/*--------------------------------------------------------*/
/* SS7 ISUP Rate Unique Structure                         */
/*--------------------------------------------------------*/
typedef struct uniquex_isup   {
    ACU_UCHAR           service_octet;
    ACU_UCHAR           add_info_octet;
    ACU_UCHAR           dest_natureof_addr;
    ACU_UCHAR           dest_numbering_plan;
    BEARER              bearer;
    ACU_UCHAR           orig_natureof_addr;
    ACU_UCHAR           orig_numbering_plan;
    ACU_UCHAR           orig_numbering_presentation;
    ACU_UCHAR           orig_numbering_screening;
    ACU_UCHAR           conn_natureof_addr;
    ACU_UCHAR           conn_numbering_plan;
    ACU_UCHAR           conn_numbering_presentation;
    ACU_UCHAR           conn_numbering_screening;
    ACU_UCHAR           conn_number_req;
    ACU_UCHAR           orig_category;
    ACU_UCHAR           orig_number_incomplete;

    ACU_UCHAR           dest_subaddr[MAXNUM];
    ACU_UCHAR           orig_subaddr[MAXNUM];

    HILAYER             hilayer;
    LOLAYER             lolayer;
    PROGRESS_INDICATOR  progress_indicator;

    ACU_UCHAR           in_band;

    ACU_UCHAR           nat_inter_call_ind;
    ACU_UCHAR           interworking_ind;
    ACU_UCHAR           isdn_userpart_ind;
    ACU_UCHAR           isdn_userpart_pref_ind;
    ACU_UCHAR           isdn_access_ind;
    ACU_UCHAR           dest_int_nw_ind;

    ACU_UCHAR           continuity_check_ind;
    ACU_UCHAR           satellite_ind;
    ACU_UCHAR           charge_ind;
    ACU_UCHAR           dest_category;

    ACU_UCHAR           add_calling_num_qualifier_ind;
    ACU_UCHAR           add_calling_num_natureof_addr;
    ACU_UCHAR           add_calling_num_plan;
    ACU_UCHAR           add_calling_num_presentation;
    ACU_UCHAR           add_calling_num_screening;
    ACU_UCHAR           add_calling_num_incomplete;
    ACU_UCHAR           add_calling_num[MAXNUM];

    ACU_UCHAR           exchange_type;
    ACU_UCHAR           collect_call_request_ind;

    ACU_UCHAR           raw_fci[2];   /* for/from IAM */
    ACU_UCHAR           raw_nci;      /* for/from IAM */
    ACU_UCHAR           raw_bci[2];   /* received only */
} ACU_PACK_DIRECTIVE  UNIQUEX_ISUP;

/*--------------------------------------------------------*/
/* IP telephony structures                                */
/*--------------------------------------------------------*/

typedef struct acu_codec {
    ACU_INT         codec_type;
    ACU_INT         vad;
    ACU_INT         fpp;
    ACU_ULONG       options;
} ACU_PACK_DIRECTIVE ACU_CODEC;

typedef struct media_settings {
    ACU_INT         tdm_encoding;
    ACU_INT         encode_gain;
    ACU_INT         decode_gain;
    ACU_INT         echo_cancellation;
    ACU_INT         echo_suppression;
    ACU_INT         echo_span;
    ACU_UINT        rtp_tos;
    ACU_UINT        rtcp_tos;
    ACU_UINT        dtmf_detector;
} ACU_PACK_DIRECTIVE MEDIA_SETTINGS;

union protocol_union {
    struct {
        ACU_CHAR    destination_alias[MAXADDR];
        ACU_CHAR    originating_alias[MAXADDR];
        ACU_INT     h245_tunneling;
        ACU_INT     faststart;
        ACU_INT     early_h245;
        ACU_CHAR    dtmf[MAXNUM];
        ACU_INT     progress_location;
        ACU_INT     progress_description;
    } sig_h323;

    struct {
        ACU_CHAR    contact_address[MAXADDR];
        ACU_INT     zero_connection_address_hold;
        ACU_INT     disable_reliable_provisional_response;
        ACU_INT     disable_early_media;
    } sig_sip;
};

typedef struct uniquex_iptel {
    ACU_CHAR        destination_display_name[MAXDISPLAY];
    ACU_CHAR        originating_display_name[MAXDISPLAY];
    ACU_CODEC       codecs[MAXCODECS];
    MEDIA_SETTINGS  media_settings;
    ACU_POINTER     vmprxid;
    ACU_POINTER     vmptxid;
    ACU_CHAR        media_call_type[MAXMEDIACALLTYPE];
    union protocol_union    protocol_specific;
    ACU_UINT        ipv6_media;
} ACU_PACK_DIRECTIVE UNIQUEX_IPTEL;

/*--------------------------------------------------------*/
/* Union of Unique structures passed to the driver        */
/*--------------------------------------------------------*/


typedef union uniquex {
    UNIQUEX_DASS        sig_dass;
    UNIQUEX_DPNSS       sig_dpnss;
    UNIQUEX_CAS         sig_cas;
    UNIQUEX_Q931        sig_q931;
    UNIQUEX_ISUP        sig_isup;
    UNIQUEX_IPTEL       sig_iptel;
    ACU_COMPAT_PADDING  dummy;
} ACU_PACK_DIRECTIVE  UNIQUEXU;

/* open for outgoing structure */

typedef struct out_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_INT         ts;
    ACU_INT         cnf;
    ACU_INT         sending_complete;
    char            destination_addr[MAXADDR];
    char            originating_addr[MAXADDR];
    ACU_ACT         app_context_token;
    ACU_EVENT_QUEUE queue_id;
    union uniquex   unique_xparms;
} ACU_PACK_DIRECTIVE  OUT_XPARMS;


/* call details structure */

typedef struct detail_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_LONG        timeout;
    ACU_INT         valid;
    ACU_INT         stream;
    ACU_INT         ts;
    ACU_INT         calltype;
    ACU_INT         sending_complete;
    char            destination_addr[MAXADDR];
    char            originating_addr[MAXADDR];
    char            connected_addr[MAXADDR];
    ACU_COMPAT_ACT  old_app_context_token;
    ACU_ULONG       feature_information;     /* This has been changed into an ACU_LONG from an ACU_UCHAR - we need to use some of the following spare bytes */
    ACU_ACT         app_context_token;
    char            pad[4];
    /* There was once a MAXNUM array here, now adjusted
    * to keep 'unique_xparms' at a fixed offset. */
    ACU_UCHAR       spare[(MAXNUM-1)-(sizeof(ACU_ACT))-(sizeof(ACU_LONG)-1)-4];
    union uniquex   unique_xparms;
} ACU_PACK_DIRECTIVE  DETAIL_XPARMS;

/* port info parameters */
typedef struct tPORT_INFO_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_ULONG       valid_vector;
    ACU_UINT        connection_type;
    ACU_UINT        channel_allocation;
    ACU_UINT        channel_count;
    ACU_UINT        port_type;
    ACU_UINT        lim_type;
    ACU_UINT        physical_index;
    ACU_CHAR        name[MAXNAME];
    ACU_CHAR        sig_sys[MAXSIGSYS];
    ACU_CHAR        hw_ver[MAXHWVERSION];
    ACU_CHAR        hw_desc[MAX_HW_DESC];
    ACU_CHAR        fw_desc[MAX_FW_DESC];
    ACU_CHAR        fw_extension[MAX_FW_EXTN];
} PORT_INFO_PARMS;


/* overlap structure */

typedef struct overlap_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         sending_complete;
    char            destination_addr[MAXNUM];
} ACU_PACK_DIRECTIVE  OVERLAP_XPARMS;

/* open for incoming structure */

typedef struct in_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_INT         ts;
    ACU_INT         cnf;
    ACU_EVENT_QUEUE queue_id;
    ACU_ACT         app_context_token;
    union           uniquex unique_xparms;
} ACU_PACK_DIRECTIVE  IN_XPARMS;


/* call state structure */
typedef struct state_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_LONG        state;
    ACU_LONG        timeout;
    ACU_LONG        extended_state;
    ACU_ACT         app_context_token;
    ACU_TOKEN       raw_msg_seq;
} ACU_PACK_DIRECTIVE  STATE_XPARMS;


/* call charge structure */

typedef struct get_charge_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         type;
    char            charge[CHARGEMAX];
    ACU_UINT        meter;
    union {
        struct {
            ACU_UINT    tariff_type;
        } sig_isup;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE GET_CHARGE_XPARMS;

typedef struct put_charge_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    char            charge[CHARGEMAX];
    ACU_UINT        meter;
    union {
        struct {
            ACU_UINT    tariff_type;
        } sig_isup;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE PUT_CHARGE_XPARMS;

/* disconnect cause structure */

typedef struct cause_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         cause;
    ACU_INT         raw;
    ACU_INT         location;
} ACU_PACK_DIRECTIVE CAUSE_XPARMS;

/* force release structure */

typedef struct force_release_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
} ACU_PACK_DIRECTIVE FORCE_RELEASE_XPARMS;


/* call transfer structure */

typedef struct transfer_xparms {
    ACU_ULONG           size;
    ACU_CALL_HANDLE     handlea;
    ACU_CALL_HANDLE     handlec;
} ACU_PACK_DIRECTIVE TRANSFER_XPARMS;


/* Common structure for isup/ss7 call_progress and call_proceeding.
 * call_incoming_ringing is subtly different, but should not diverge further. */
struct sig_isup_progress {
    PROGRESS_INDICATOR  progress_indicator;
    ACU_UCHAR           in_band;
    ACU_UCHAR           charge_ind;
    ACU_UCHAR           dest_category;
    struct {
        ACU_UCHAR           valid;
        ACU_UCHAR           value;
    } isdn_access_ind;
    struct {
        ACU_UCHAR           valid;
        ACU_UCHAR           value;
    } isdn_userpart_ind;
    struct {
        ACU_UCHAR           valid;
        ACU_UCHAR           value;
    } interworking_ind;
    struct {
        ACU_UCHAR           valid;
        ACU_UCHAR           value[2];
    } raw_bci;
} ACU_PACK_DIRECTIVE;

/* progress structure */

typedef struct progress_xparms {
    ACU_ULONG size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            PROGRESS_INDICATOR  progress_indicator;
            DISPLAY display;
            CAUSE cause;
        } sig_q931;
        struct sig_isup_progress  sig_isup;
        struct {
            ACU_CHAR        destination_display_name[MAXDISPLAY];
            ACU_CODEC       codecs[MAXCODECS];
            MEDIA_SETTINGS  media_settings;
            ACU_POINTER     vmprxid;
            ACU_POINTER     vmptxid;
            union {
                struct {
                    ACU_INT  send_reliable_provisional_response;
                    ACU_CHAR contact_address[MAXADDR];
                } sig_sip;
                struct {
                    ACU_INT h245_tunneling;
                    ACU_INT faststart;
                    ACU_INT early_h245;
                    ACU_INT location;
                    ACU_INT description;
                } sig_h323;
            } protocol_specific;
        } sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE PROGRESS_XPARMS;


/* proceeding structure */

typedef struct proceeding_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            PROGRESS_INDICATOR    progress_indicator;
            DISPLAY               display;
            NOTIFY_INDICATOR      notify_indicator;
        } sig_q931;
        struct sig_isup_progress sig_isup;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE PROCEEDING_XPARMS;

/* setup_ack structure */

typedef struct setup_ack_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            PROGRESS_INDICATOR  progress_indicator;
            DISPLAY             display;
        } sig_q931;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE SETUP_ACK_XPARMS;

/* notify structure */

typedef struct notify_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            NOTIFY_INDICATOR    notify_indicator;
            DISPLAY             display;
        } sig_q931;
     } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE NOTIFY_XPARMS;

/* keypad structure */

typedef struct keypad_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    union {
        struct {
            ACU_INT     device;
            KEYPAD      keypad;
            ACU_INT     location;
            DISPLAY     display;
        } sig_q931;
        struct {
            union {
                struct {
                    ACU_CHAR    dtmf[MAXNUM];
                } sig_h323;
            } protocol_specific;
        } sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE KEYPAD_XPARMS;

/* disconnect structure */

typedef struct disconnect_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         cause;
    union {
        struct {
            ACU_INT             raw;
            PROGRESS_INDICATOR  progress_indicator;
            DISPLAY             display;
            NOTIFY_INDICATOR    notify_indicator;
            ACU_INT             location;    /* cause location */
            ACU_INT             coding_standard; /* RFC 2792 */
        } sig_q931;
        struct {
            ACU_INT             raw;
            PROGRESS_INDICATOR  progress_indicator;
            ACU_INT             location;
            ACU_INT             reattempt;
        } sig_isup;
        struct {
            ACU_INT             raw;
        } sig_1tr6;
        struct {
            ACU_INT             raw;
        } sig_dass;
        struct {
            ACU_INT             raw;
        } sig_dpnss;
        struct {
            ACU_INT             raw;
        } sig_cas;
        struct {
            union {
                struct {
                    ACU_INT     raw;
                    ACU_CHAR    warning[MAXSIPWARNING];
                    ACU_CHAR    response_explanation[MAXSIPRESPONSEEXPLANATION];
                } sig_sip;
                struct {
                    ACU_INT     raw;
                    ACU_INT     raw_type;
                } sig_h323;
            } protocol_specific;
        } sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE DISCONNECT_XPARMS;

/* accept structure */

typedef struct accept_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            PROGRESS_INDICATOR  progress_indicator;
            LOLAYER             lolayer;
            DISPLAY             display;
            char                connected_addr[MAXNUM];
            ACU_UCHAR           conn_numbering_type;
            ACU_UCHAR           conn_numbering_plan;
            ACU_UCHAR           conn_numbering_presentation;
            ACU_UCHAR           conn_numbering_screening;
            ACU_UCHAR           conn_subaddr[MAXNUM];
        } sig_q931;
        struct {
            PROGRESS_INDICATOR  progress_indicator;
            LOLAYER             lolayer;
            char                connected_addr[MAXNUM];
            ACU_UCHAR           conn_natureof_addr;
            ACU_UCHAR           conn_numbering_plan;
            ACU_UCHAR           conn_numbering_presentation;
            ACU_UCHAR           conn_numbering_screening;
            ACU_UCHAR           charge_ind;
            ACU_UCHAR           dest_category;
            struct {
                ACU_UCHAR       valid;
                ACU_UCHAR       value;
            } isdn_access_ind;
            struct {
                ACU_UCHAR       valid;
                ACU_UCHAR       value;
            } isdn_userpart_ind;
            struct {
                ACU_UCHAR       valid;
                ACU_UCHAR       value;
            } interworking_ind;
            struct {
                ACU_UCHAR       valid;
                ACU_UCHAR       value[2];
            } raw_bci;
        } sig_isup;
        UNIQUEX_IPTEL       sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE ACCEPT_XPARMS;


/* incoming_ringing structure */

typedef struct incoming_ringing_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        struct {
            PROGRESS_INDICATOR    progress_indicator;
            DISPLAY               display;
            NOTIFY_INDICATOR      notify_indicator;
        } sig_q931;
        struct {
            /* Note that the charge_ind and
            * in_band fields are swapped from
            * struct sig_isup_progress. */
            PROGRESS_INDICATOR    progress_indicator;
            ACU_UCHAR             charge_ind;
            ACU_UCHAR             in_band;
            ACU_UCHAR             dest_category;
            struct {
                ACU_UCHAR         valid;
                ACU_UCHAR         value;
            } isdn_access_ind;
            struct {
                ACU_UCHAR         valid;
                ACU_UCHAR         value;
            } isdn_userpart_ind;
            struct {
                ACU_UCHAR         valid;
                ACU_UCHAR         value;
            } interworking_ind;
            struct {
                ACU_UCHAR         valid;
                ACU_UCHAR         value[2];
            } raw_bci;
        } sig_isup;
        struct {
            ACU_CHAR              destination_display_name[MAXDISPLAY];
            ACU_CODEC             codecs[MAXCODECS];
            MEDIA_SETTINGS        media_settings;
            ACU_POINTER           vmprxid;
            ACU_POINTER           vmptxid;
            union {
                struct {
                    ACU_INT       send_early_media;
                    ACU_INT       use_183_response_for_early_media;
                    ACU_INT       send_reliable_provisional_response;
                    ACU_CHAR      contact_address[MAXADDR];
                } sig_sip;
                struct {
                    ACU_INT       h245_tunneling;
                    ACU_INT       faststart;
                    ACU_INT       early_h245;
                    ACU_INT       progress_location;
                    ACU_INT       progress_description;
                } sig_h323;
            } protocol_specific;
        } sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE INCOMING_RINGING_XPARMS;

/* hold structure */

typedef struct hold_xparms {
    ACU_ULONG        size;
    ACU_CALL_HANDLE  handle;
    union {
        struct {
            ACU_INT  hold_type;
        } sig_h323;
    } ACU_PACK_DIRECTIVE  unique_xparms;
} ACU_PACK_DIRECTIVE HOLD_XPARMS;

/* get_originating_addr structure */

typedef struct get_originating_addr_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
} ACU_PACK_DIRECTIVE GET_ORIGINATING_ADDR_XPARMS;

/* Port (Group) reset */
typedef struct port_reset_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         flags;
    ACU_INT         reserved;
    union {
        ACU_ULONG   ts_mask;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE PORT_RESET_XPARMS;


/* Port (Group) blocking/unblocking */

typedef struct port_blocking_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         flags;
    ACU_INT         type;
    ACU_INT         reserved;
    union {
        ACU_ULONG   ts_mask;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE PORT_BLOCKING_XPARMS;

/* Port blocking/reset status */
typedef struct port_status_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         rqst;           /* ACU_MAINT_PORT_GET_xxx */
    ACU_INT         flags;          /* No flags are currently defined */
    ACU_ULONG       ts_mask;        /* Timeslots of interest (updated) */
    ACU_UCHAR       ts_info[32];    /* State of requested timeslots */
} ACU_PACK_DIRECTIVE PORT_STATUS_XPARMS;


/* Timeslot blocking/unblocking */

typedef struct ts_blocking_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         ts;
    ACU_INT         flags;
} ACU_PACK_DIRECTIVE TS_BLOCKING_XPARMS;



/*----------------------------------------------*/
/* primary rate layer 1 and layer 2 information */
/*----------------------------------------------*/

/* these can be read and reset */

typedef struct getset {
    ACU_INT         linestat;
    ACU_INT         bipvios;
    ACU_INT         faserrs;
    ACU_INT         sliperrs;
} ACU_PACK_DIRECTIVE GETSET;

/* these can be read only */

typedef struct get {
    ACU_UCHAR       nos;
    ACU_UCHAR       ais;
    ACU_UCHAR       los;
    ACU_UCHAR       rra;
    ACU_UCHAR       tra;
    ACU_UCHAR       rma;
    ACU_UCHAR       tma;
    ACU_UCHAR       usr;
    ACU_UCHAR       majorrev;
    ACU_UCHAR       minorrev;
    ACU_LONG        clock;
    char            buildstr[MAX_L1_TEXT];
    char            manstr[MAX_L1_TEXT];
    char            sigstr[MAX_L1_TEXT];
} ACU_PACK_DIRECTIVE GET;


typedef struct l1_xstats {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    GETSET          getset;
    GET             get;
} ACU_PACK_DIRECTIVE L1_XSTATS;


typedef struct l2_xstate {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_LONG        state;
} ACU_PACK_DIRECTIVE L2_XSTATE;


typedef struct alarm_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         alarm;
} ACU_PACK_DIRECTIVE ALARM_XPARMS;


typedef struct watchdog_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         alarm;
    ACU_LONG        timeout;
} ACU_PACK_DIRECTIVE WATCHDOG_XPARMS;


typedef struct dcba_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_UCHAR       tdcba[16];
    ACU_UCHAR       rdcba[16];
} ACU_PACK_DIRECTIVE DCBA_XPARMS;


typedef struct aculablog {
    ACU_LONG        TimeStamp;
    ACU_UCHAR       RxTx;
    ACU_UCHAR       Data_Packet[75];
} ACU_PACK_DIRECTIVE LOG;


typedef struct log_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    LOG             log;
} ACU_PACK_DIRECTIVE LOG_XPARMS;

/*-----------------------------------*/
/* DPNSS Enhanced Feature Structures */
/*-----------------------------------*/

typedef struct feature_xparms {
    ACU_INT     msg[MAX_FEAT_MSG];      /* Feature information message */
    ACU_UCHAR   call_type;              /* Call type - real or virtual */
    ACU_CHAR    digits[MAXNUM];         /* Feature digits              */
    ACU_CHAR    cli [MAXNUM];           /* Called Line Identity        */
    ACU_CHAR    nsi[MAXNSI];            /* Non Specified Information   */
    ACU_CHAR    txt[MAXTXT];            /* Text                        */
    ACU_CHAR    tid[MAXTID];            /* Trunk ID                    */
    ACU_UCHAR   clc;                    /* Call/Called Line category   */
    ACU_UCHAR   held_clc;               /* Held Calling Line Category  */
    ACU_UCHAR   ipl;                    /* Intrusion protection level  */
    ACU_UCHAR   icl;                    /* Intrusion capability level  */
    ACU_UCHAR   routes;                 /* The number of further Routes - Loop Avoidance */
    ACU_UCHAR   transits;               /* The number of further Transits - Loop Avoidance  */
    ACU_UCHAR   sod;                    /* State Of Destination */
    ACU_UCHAR   sodq;                   /* State Of Destination Qualifier */

    /* For PMX/Prosody X Products only below */

    ACU_CHAR    dnae[MAXNAE];            /* Call Line Network Address Extension */
    ACU_CHAR    cnae[MAXNAE];            /* Destination Network Address Extension */
    ACU_CHAR    call_ref[MAXCALLREF];    /* Call Reference, for Route Optimisation */
    ACU_CHAR    call_id_len[MAXCALLID];  /* Call ID Length, for Route Optimisation */
    ACU_CHAR    bss_sic[MAXSIC];         /* 1B2I encoded SIC value */
    ACU_CHAR    red_reason[MAXRED];      /* 2 letters to describe redirecting reason */
    ACU_CHAR    rcf_cause[MAXRCF];       /* 2 letters to represent 1B2I encoding of clearing cause i.e. 08 -normal  would be  @H */
    ACU_CHAR    rrm_cause;               /* clearing cause from a Recall Reject Message. Associated with RECALL_REJECT_MESSAGE feature msg.*/
} ACU_PACK_DIRECTIVE FEATURE_XPARMS;


typedef struct dpns_out_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_INT         ts;
    ACU_INT         cnf;
    ACU_INT         sending_complete;
    ACU_CHAR        destination_addr[MAXNUM];
    ACU_CHAR        originating_addr[MAXNUM];
    ACU_ACT         app_context_token;
    ACU_EVENT_QUEUE queue_id;
    union  uniquex  unique_xparms;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_OUT_XPARMS;


typedef struct dpns_overlap_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         sending_complete;
    ACU_CHAR        destination_addr[MAXNUM];
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_OVERLAP_XPARMS;


typedef struct dpns_feature_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_FEATURE_XPARMS;


typedef struct dpns_incoming_ring_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_INCOMING_RING_XPARMS;


typedef struct dpns_call_accept_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_CALL_ACCEPT_XPARMS;


typedef struct dpns_detail_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_LONG        timeout;
    ACU_INT         valid;
    ACU_INT         stream;
    ACU_INT         ts;
    ACU_INT         calltype;
    ACU_INT         sending_complete;
    ACU_CHAR        destination_addr[MAXNUM];
    ACU_CHAR        originating_addr[MAXNUM];
    ACU_CHAR        connected_addr[MAXNUM];
    ACU_CHAR        redirected_addr[MAXNUM];
    union  uniquex  unique_xparms;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_DETAIL_XPARMS;


typedef struct dpns_transit_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_LONG        timeout;
    ACU_INT         valid;
    ACU_CHAR        trans_msg[TRANSIT_MSG_LENGTH];
} ACU_PACK_DIRECTIVE DPNS_TRANSIT_XPARMS;


typedef struct dpns_set_transit_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
} ACU_PACK_DIRECTIVE DPNS_SET_TRANSIT_XPARMS;


typedef struct dpns_cause_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_INT         cause;
    ACU_INT         raw;
    FEATURE_XPARMS  feature_info;
} ACU_PACK_DIRECTIVE DPNS_CAUSE_XPARMS;


typedef struct dpns_l2_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         channel;
    ACU_UCHAR       state;
    ACU_LONG        timeout;
} ACU_PACK_DIRECTIVE DPNS_L2_XPARMS;

/*------------ End of DPNSS -------------*/

/*-----------------------------------*/
/* Resilience Structures             */
/*-----------------------------------*/

typedef ACU_POINTER ACU_FAILOVER_ID;

typedef struct tCALL_GET_FAILOVER_ID_PARMS {
    ACU_ULONG       size;       /* IN */
    ACU_CALL_HANDLE handle;     /* IN */
    ACU_FAILOVER_ID failover_id;/* OUT */
} ACU_PACK_DIRECTIVE CALL_GET_FAILOVER_ID_PARMS;

typedef struct tCALL_REOPEN_PARMS {
    ACU_ULONG       size;       /* IN */
    ACU_PORT_ID     net;        /* IN */
    ACU_FAILOVER_ID failover_id;        /* IN */
    ACU_EVENT_QUEUE queue_id;       /* IN */
    ACU_ACT     app_context_token;  /* IN */
    ACU_CALL_HANDLE handle;         /* OUT */
} ACU_PACK_DIRECTIVE CALL_REOPEN_PARMS;

typedef struct tCALL_RELEASE_LOST_FAILOVER_IDS_PARMS {
    ACU_ULONG       size;       /* IN */
    ACU_PORT_ID     net;        /* IN */
} ACU_PACK_DIRECTIVE CALL_RELEASE_LOST_FAILOVER_IDS_PARMS;


/* RFC2407  start */
typedef enum ePresentedNumberUnscreened {
    ets_196_presentation_allowed_number              = 0,
    ets_196_presentation_restricted                  = 1,
    ets_196_number_not_available_due_to_interworking = 2,
    ets_196_presentation_restricted_number           = 3
} EPRESENTEDNUMBERUNSCREENED;

typedef enum ePartyNumber {
    ets_196_unknown_party_number           = 0,
    ets_196_public_party_number            = 1,
    ets_196_nsap_encoded_number            = 2,
    ets_196_data_party_number              = 3,
    ets_196_telex_party_number             = 4,
    ets_196_private_party_number           = 5,
    ets_196_national_standard_party_number = 8
} EPARTYNUMBER;

typedef enum ePublicTypeOfNumber {
    ets_196_public_type_of_num_unknown                 = 0,
    ets_196_public_type_of_num_international_number    = 1,
    ets_196_public_type_of_num_national_number         = 2,
    ets_196_public_type_of_num_network_specific_number = 3,
    ets_196_public_type_of_num_subscriber_number       = 4,
    ets_196_public_type_of_num_abbreviated_number      = 6
} EPUBLICTYPEOFNUMBER;


typedef enum ePrivateTypeOfNumber {
    ets_196_private_type_of_num_unknown                = 0,
    ets_196_private_type_of_num_level2_regional_number = 1,
    ets_196_private_type_of_num_level1_regional_number = 2,
    ets_196_private_type_of_num_ptn_specific_number    = 3,
    ets_196_private_type_of_num_local_number           = 4,
    ets_196_private_type_of_num_abbreviated_number     = 6
} EPRIVATETYPEOFNUMBER;
/* RFC2407 end */



/*-------------------------------*/
/* Enhanced SIP helper structures*/
/*-------------------------------*/

/******************************************************************************
 * Message body structure, for appending to SIP messages
 */

typedef struct acu_raw_message_body {
    char            *body_type;
    unsigned char   *body;
    ACU_INT         body_length;
    char            *additional_body_headers;

    struct acu_raw_message_body  *next;
} ACU_PACK_DIRECTIVE ACU_RAW_MESSAGE_BODY;

/******************************************************************************
 * IP address configuration
 */

typedef enum acu_ip_type {
    ACU_IP_DEFAULT = 0,
    ACU_IPv4       = 1,
    ACU_IPv6       = 2
} ACU_IP_TYPE;

/******************************************************************************
 *
 * ACU_IP_ADDRESS - this structure is used to encapsulate IP addresses.
 */

typedef struct acu_ip_address {
    unsigned char   address_type;
    const char      *address;
} ACU_PACK_DIRECTIVE ACU_IP_ADDRESS;

/******************************************************************************
 * Payload definitions
 */

enum ePAYLOAD_DEFINITIONS {
    ACU_EIGHT_K = 8000
};


/* static RTP payload number */
#define ACU_PCMU_PAYLOAD_NUMBER 0
#define ACU_PCMA_PAYLOAD_NUMBER 8
#define ACU_G723_PAYLOAD_NUMBER 4
#define ACU_G729_PAYLOAD_NUMBER 18
#define ACU_G728_PAYLOAD_NUMBER 15
#define ACU_GSM_PAYLOAD_NUMBER 3

/* commonly used RTP payload names */
#define ACU_PCMU "PCMU"
#define ACU_PCMA "PCMA"
#define ACU_G723 "G723"
#define ACU_G729 "G729"
#define ACU_G726_OPTION_16K "g726-16"
#define ACU_G726_OPTION_24K "g726-24"
#define ACU_G726_OPTION_32K "g726-32"
#define ACU_G726_OPTION_40K "g726-40"
#define ACU_G728 "G728"
#define ACU_GSM_EFR "GSM-EFR"
#define ACU_GSM_FR "GSM"
#define ACU_RFC4040 "CLEARMODE"
#define ACU_ILBC "iLBC"
#define ACU_AMR_NB "AMR"
#define ACU_AMR_WB "AMR-WB"
#define ACU_EVRC "EVRC"
#define ACU_EVRC0 "EVRC0"

#define ACU_TELEPHONE_EVENT "telephone-event"

/* the RTP payload used for DTMF */
#define ACU_DTMF_RTP_PAYLOAD ACU_TELEPHONE_EVENT

/* Fax over IP (FoIP) payload name */
#define ACU_T38 "t38"

/* static RTP payload number */
#define ACU_PCMU_PAYLOAD_NUMBER 0
#define ACU_PCMA_PAYLOAD_NUMBER 8
#define ACU_G723_PAYLOAD_NUMBER 4
#define ACU_G729_PAYLOAD_NUMBER 18
#define ACU_G728_PAYLOAD_NUMBER 15
#define ACU_GSM_PAYLOAD_NUMBER 3


/* the RTP payload used for DTMF */
#define ACU_DTMF_RTP_PAYLOAD ACU_TELEPHONE_EVENT

/* Fax over IP (FoIP) payload name */
#define ACU_T38 "t38"

/* TOS bit masks */
#define ACU_TOS_LOW_DELAY        0x10
#define ACU_TOS_HIGH_THROUGHPUT  0x08
#define ACU_TOS_HIGH_RELIABILITY 0x04
#define ACU_TOS_ECT_BIT          0x02
#define ACU_TOS_CE_BIT           0x01

/*
ASTERISK options are...
be (best effort), cs1, af11, af12, af13, cs2, af21, af22, af23, cs3, 
af31, af32, af33, cs4, af41, af42, af42, ef (expedited forwarding), 
lowdelay, throughput, reliability, mincost, none
*/


/******************************************************************************
 *
 * ACU_PAYLOAD - this structure conveys the information in a payload described
 * on an m= and associated a= lines in an SDP body.
 */

typedef struct acu_payload {
    union {
        struct {
            const char  *rtp_payload_name; /* e.g. "PCMU", "telephone-event" */
            ACU_INT     rtp_payload_number; /* number as defined in RFC 1890 or dynamic assigned payload */
            ACU_INT     packet_length;
            ACU_INT     clock_rate;
            const char  *payload_specific_options;
        } audio_video;

        struct {
            char        *image_payload_name;
        } image;

        struct {
            ACU_INT     dummy;
        } control;

        struct {
            ACU_INT     dummy;
        } application;

        struct {
            char        *pad1;
            ACU_INT     pad2;
            ACU_INT     pad3;
            unsigned char pad4;
            ACU_INT     pad5;
            ACU_INT     pad6;
        } padding;

    } payload;

    struct acu_payload  *next;
} ACU_PACK_DIRECTIVE ACU_PAYLOAD;

/******************************************************************************
 * media descriptions
 */

/* media types... */
typedef enum acu_media_types {
    ACU_AUDIO,
    ACU_VIDEO,
    ACU_IMAGE,
    ACU_CONTROL,
    ACU_TEXT,
    ACU_APPLICATION,
    ACU_UNKNOWN_MEDIA_TYPE,
    ACU_SESSION
} ACU_MEDIA_TYPES;

/* media direction attributes for SDP */
typedef enum acu_media_direction {
    ACU_SEND_RECV,
    ACU_SEND_ONLY,
    ACU_RECV_ONLY,
    ACU_INACTIVE
} ACU_MEDIA_DIRECTION;

/* optional SDP media attributes, which cannot be easily categorised will be placed here */
typedef struct acu_miscellaneous_media_attribute {
    char  *attribute;
    struct acu_miscellaneous_media_attribute  *next;
} ACU_PACK_DIRECTIVE ACU_MISCELLANEOUS_MEDIA_ATTRIBUTE;

/******************************************************************************
 *
 * ACU_MEDIA_DESCRIPTION - this structure the m= and associated a= and c=
 * lines in a piece of SDP.
 */

typedef struct acu_media_description {
    ACU_IP_ADDRESS                     connection_address;
    ACU_USHORT                         port;
    ACU_USHORT                         number_of_ports;
    unsigned char                      media_direction;
    unsigned char                      media_type;
    char                               *transport;
    ACU_INT                            packet_length;
    ACU_MISCELLANEOUS_MEDIA_ATTRIBUTE  *miscellaneous_attributes;
    ACU_PAYLOAD                        *payloads;

    struct acu_media_description  *next;
} ACU_PACK_DIRECTIVE ACU_MEDIA_DESCRIPTION;

/******************************************************************************
 *
 * ACU_MEDIA_OFFER_ANSWER - this structure represents all the information in an
 * SDP body relevant to setting one side of a media session
 */

typedef struct acu_media_offer_answer {
    ACU_IP_ADDRESS connection_address;
    ACU_MEDIA_DESCRIPTION  *media_descriptions;
    char  *raw_sdp;
} ACU_PACK_DIRECTIVE ACU_MEDIA_OFFER_ANSWER;

/******************************************************************************
 *
 * ACU_MEDIA_SESSION - this structure is used to convey in call_details all the
 * information required to setup a media session between 2 parties.
 */

typedef struct acu_media_session {
    ACU_MEDIA_OFFER_ANSWER sent_media;
    ACU_MEDIA_OFFER_ANSWER received_media;
    char                   sent_is_answer;
} ACU_PACK_DIRECTIVE ACU_MEDIA_SESSION;

/******************************************************************************
 *
 * types of SIP messages, which the app. may directly send or receive mid-call
 */
typedef enum acu_sip_message_type {
    ACU_SIP_MESSAGE_NULL        = 0,
    ACU_SIP_MESSAGE_INFO        = 1,
    ACU_SIP_MESSAGE_NOTIFY      = 2,
    ACU_SIP_MESSAGE_REGISTER    = 3,
    ACU_SIP_MESSAGE_SUBSCRIBE   = 4,
    ACU_SIP_MESSAGE_OPTIONS     = 5,
    ACU_SIP_MESSAGE_UPDATE      = 6,
    ACU_SIP_MESSAGE_MESSAGE     = 7,
    ACU_SIP_MESSAGE_REFER       = 8,
    ACU_SIP_MESSAGE_PING        = 9
} ACU_SIP_MESSAGE_TYPE;

/* SIP message masks - this is used to setup request_notification_mask
 * and response_notification_mask in the sig_sip structure below.
 */

typedef enum acu_sip_message_notification_masks {
    ACU_SIP_INITIAL_INVITE_NOTIFICATION         = 0x00000001, /* Not valid for use with enable_response_mask */
    ACU_SIP_REINVITE_NOTIFICATION               = 0x00000002, /* Not valid for use with enable_response_mask */
    ACU_SIP_TRANSFER_INVITE_NOTIFICATION        = 0x00000004, /* Not valid for use with enable_response_mask */
    ACU_SIP_INFO_NOTIFICATION                   = 0x00000008,
    ACU_SIP_NOTIFY_NOTIFICATION                 = 0x00000010,
    ACU_SIP_REGISTER_NOTIFICATION               = 0x00000020, /* Only valid for use with the out of dialog API */
    ACU_SIP_SUBSCRIBE_NOTIFICATION              = 0x00000040, /* Only valid for use with the out of dialog API */
    ACU_SIP_OPTIONS_NOTIFICATION                = 0x00000080,
    ACU_SIP_BYE_NOTIFICATION                    = 0x00000100, 
    ACU_SIP_MESSAGE_NOTIFICATION                = 0x00000200,
    ACU_SIP_UPDATE_NOTIFICATION                 = 0x00000400,
    ACU_SIP_PRACK_NOTIFICATION                  = 0x00000800, /* Not valid for use with enable_response_mask */
    ACU_SIP_REFER_NOTIFICATION                  = 0x00001000, /* Not valid for use with enable_response_mask */
    ACU_SIP_INITIAL_ACK_NOTIFICATION            = 0x00002000, /* Not valid for use with enable_response_mask */
    ACU_SIP_REINVITE_ACK_NOTIFICATION           = 0x00004000, /* Not valid for use with enable_response_mask */
    ACU_SIP_TRANSFER_ACK_NOTIFICATION           = 0x00008000  /* Not valid for use with enable_response_mask */
} ACU_SIP_MESSAGE_NOTIFICATION_MASKS;

typedef struct acu_sip_message {
    unsigned char   *message;
    ACU_INT         message_length;
    ACU_UINT        message_handle;
} ACU_PACK_DIRECTIVE ACU_SIP_MESSAGE;

typedef struct _ACU_STRING_LIST {
    char  *string;
    struct _ACU_STRING_LIST  *next;
} ACU_PACK_DIRECTIVE ACU_STRING_LIST;

typedef struct _ACU_REDIRECT_INFO {
    int sip_response_code;
    ACU_STRING_LIST  *contact_list;
} ACU_PACK_DIRECTIVE ACU_REDIRECT_INFO;

typedef struct _ACU_SUBSCRIPTION_INFO {
    ACU_INT             expires;
    ACU_INT             dialog_count;
    ACU_INT             dialog_id;
    ACU_INT             notify_required;
    ACU_STRING_LIST     *body_types;
    char                *event_id_param;
} ACU_PACK_DIRECTIVE ACU_SUBSCRIPTION_INFO;
/*-------------------------------*/
/* Enhanced SIP API structures   */
/*-------------------------------*/

/* SIP call options */

typedef enum acu_sip_call_options {
    ACU_RAISE_MEDIA_EVENT_FOR_HOLD        =0x00000001,
    ACU_RAISE_HOLD_EVENT_FOR_C_EQUAL_ZERO =0x00000002,
    ACU_USE_C_EQUAL_ZERO_FOR_HOLD         =0x00000004,
    ACU_DISABLE_EARLY_MEDIA               =0x00000008,
    ACU_RAISE_HOLD_EVENT_FOR_INACTIVE     =0x00000010,
    ACU_USE_INACTIVE_FOR_HOLD             =0x00000020,
    ACU_DISABLE_TRANSFER_ON_REFER         =0x00000040,
    ACU_USE_MEDIA_DESCRIPTION_FOR_SESSION =0x00000080,
    ACU_DO_NOT_USE_PROXY_IF_SET           =0x00000100,
    ACU_USE_ASYNC_MODE                    =0x00000200,
    ACU_NO_HOLD_EVENTS                    =0x00000400,
    ACU_RAISE_FORKED_EVENT                =0x00000800
} ACU_SIP_CALL_OPTIONS;

/* Extended SIP API outgoing parameters */

typedef struct sip_out_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_PORT_ID             net;
    ACU_INT                 cnf;
    char                    *destination_addr;
    char                    *originating_addr;
    char                    *contact_address;
    char                    *destination_display_name;
    char                    *originating_display_name;
    ACU_ACT                 app_context_token;
    ACU_EVENT_QUEUE         queue_id;
    ACU_UINT                request_notification_mask;
    ACU_UINT                response_notification_mask;
    ACU_UINT                enable_midcall_response_mask;
    char                    *request_uri;
    char                    *custom_headers;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
    ACU_UINT                call_options;
    char                    transport_type;
    char                    *callid;
    char                    *user;
} ACU_PACK_DIRECTIVE SIP_OUT_PARMS;

/* Extended SIP API incoming parameters */

typedef struct sip_in_parms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_INT         cnf;
    ACU_EVENT_QUEUE queue_id;
    ACU_ACT         app_context_token;
    ACU_UINT        request_notification_mask;
    ACU_UINT        response_notification_mask;
    ACU_UINT        enable_midcall_response_mask;
    ACU_UINT        call_options;
} ACU_PACK_DIRECTIVE SIP_IN_PARMS;

/* Extended SIP API accept parameters */

typedef struct sip_accept_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    char                    *contact_address;
    char                    *custom_headers; /* reserved for future */
    ACU_RAW_MESSAGE_BODY    *message_bodies; /* reserved for future */
    ACU_MEDIA_OFFER_ANSWER  media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_ACCEPT_PARMS;

/* Extended SIP API details parameters */

typedef struct sip_detail_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 valid;
    ACU_INT                 calltype;
    char                    *destination_addr;
    char                    *originating_addr;
    char                    *connected_addr;
    char                    *destination_display_name;
    char                    *originating_display_name;
    char                    *request_uri;
    ACU_ACT                 app_context_token;
    char                    reliable_provisional_response_supported;
    ACU_SIP_MESSAGE         *sip_message;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
    ACU_MEDIA_SESSION       *media_session;
    char                    transfer_refer_received;
    char                    transport_type;
    ACU_REDIRECT_INFO       *redirect_info;
    ACU_SUBSCRIPTION_INFO   *subscription_info;
} ACU_PACK_DIRECTIVE SIP_DETAIL_PARMS;

/* Extended SIP API incoming ringing parameters */

typedef struct sip_incoming_ringing_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 send_early_media;
    ACU_INT                 use_183_response_for_early_media;
    ACU_INT                 send_reliable_provisional_response;
    char                    *contact_address;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_INCOMING_RINGING_PARMS;

/* Extended SIP API call progress parameters */

typedef struct sip_progress_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 send_reliable_provisional_response;
    char                    *contact_address;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_PROGRESS_PARMS;

/* Extended SIP API feature detail parameters */

typedef struct sip_feature_detail_parms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_ULONG       feature_type;

    union {
        struct {
            ACU_INT command;
        } hold;

        struct {
            ACU_INT failure_code;
            ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
        } transfer;
    } feature;
} ACU_PACK_DIRECTIVE SIP_FEATURE_DETAIL_PARMS;

/* Extended SIP API media propose parameters */

typedef struct tSIP_MEDIA_PROPOSE_PARMS {
    ACU_ULONG              size;
    ACU_CALL_HANDLE        handle;
    ACU_MEDIA_OFFER_ANSWER media_offer_answer;
    char                   *custom_headers;
} ACU_PACK_DIRECTIVE SIP_MEDIA_PROPOSE_PARMS;

/* Extended SIP API media accept parameters */

typedef struct tSIP_MEDIA_ACCEPT_PARMS {
    ACU_ULONG              size;
    ACU_CALL_HANDLE        handle;
    ACU_MEDIA_OFFER_ANSWER media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_MEDIA_ACCEPT_PARMS;

/* Extended SIP API media request proposal parameters */

typedef struct tSIP_MEDIA_REQUEST_PROPOSAL_PARMS {
    ACU_ULONG           size;
    ACU_CALL_HANDLE     handle;
    char                *custom_headers;
} ACU_PACK_DIRECTIVE SIP_MEDIA_REQUEST_PROPOSAL_PARMS;

/* Extended SIP API media reject proposal parameters */

typedef struct tSIP_MEDIA_REJECT_PROPOSAL_PARMS {
    ACU_ULONG           size;
    ACU_CALL_HANDLE     handle;

    union {
        ACU_INT sip_code;
    } protocol_specific;
} ACU_PACK_DIRECTIVE SIP_MEDIA_REJECT_PROPOSAL_PARMS;

/* Extended SIP API send request parameters */

typedef struct tSIP_SEND_REQUEST_PARMS {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    unsigned char           message_type;
    char                    *request_uri;       /* Reserved for future use. */
    char                    *custom_headers;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_SEND_REQUEST_PARMS;


/* SIP out of dialog transaction failure causes */

typedef enum acu_sip_ood_causes {
    ACU_OOD_NO_ERROR                    =   0,
    ACU_OOD_REQUEST_TIMEOUT             =   1,
    ACU_OOD_TCP_CONNECT_FAILED          =   2,
    ACU_OOD_SSL_ERROR                   =   3,
    ACU_OOD_SSL_PEER_CERT_NOT_TRUSTED   =   4,
    ACU_OOD_SSL_PEER_CERT_INVALID       =   5,
    ACU_OOD_UNRESOLVABLE_NAME           =   6,
    ACU_OOD_REQUEST_FAILED              =   7
} ACU_SIP_OOD_CAUSES;

/* Extended SIP API read request parameters*/

typedef struct tSIP_READ_MESSAGE_PARMS {
    ACU_LONG            size;
    ACU_SIP_MESSAGE     message;
    ACU_PORT_ID         port_id;
    ACU_POINTER         transaction_id;
    ACU_UINT            cause;
} ACU_PACK_DIRECTIVE SIP_READ_MESSAGE_PARMS;


/* Extended SIP API send response parameters */

typedef struct tSIP_MESSAGE_NOTIFICATION_PARMS {
    ACU_LONG    size;
    ACU_PORT_ID port_id;
    ACU_UINT    request_notification_mask;
    ACU_UINT    response_notification_mask;
    ACU_UINT    enable_response_mask;
} ACU_PACK_DIRECTIVE SIP_MESSAGE_NOTIFICATION_PARMS;

typedef struct tSIP_SEND_RESPONSE_PARMS {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_UINT                message_handle;
    ACU_INT                 sip_code;
    char                    *custom_headers;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
} ACU_PACK_DIRECTIVE SIP_SEND_RESPONSE_PARMS;

typedef struct  tSIP_SEND_OUT_OF_DIALOG_REQUEST_PARMS {
    ACU_ULONG             size;
    unsigned char         message_type;
    char                  *request_uri;
    char                  *to;
    char                  *from;
    char                  *local_address;
    char                  *custom_headers;
    ACU_RAW_MESSAGE_BODY  *message_bodies;
    ACU_POINTER           transaction_id;
} ACU_PACK_DIRECTIVE SIP_SEND_OUT_OF_DIALOG_REQUEST_PARMS;

typedef struct tSIP_SEND_OUT_OF_DIALOG_RESPONSE_PARMS {
    ACU_ULONG             size;
    ACU_INT               sip_code;
    ACU_POINTER           transaction_id;
    ACU_LONG              port_id;
    char                  *custom_headers;
    ACU_RAW_MESSAGE_BODY  *message_bodies;
} ACU_PACK_DIRECTIVE SIP_SEND_OUT_OF_DIALOG_RESPONSE_PARMS;

/* Extended SIP API set reason phrase parameters */

typedef struct tSIP_SET_REASON_PHRASE_PARMS {
    ACU_ULONG size;
    ACU_INT sip_code;
    const char  *reason_phrase;
} ACU_PACK_DIRECTIVE SIP_SET_REASON_PHRASE_PARMS;

/* SIP authentication credentials for answering authentication challenges */

typedef struct tSIP_ANSWER_CHALLENGE_CREDENTIALS_PARMS {
    ACU_ULONG size;
    char  *realm;
    char  *user;
    char  *password;
    ACU_INT retries;
} ACU_PACK_DIRECTIVE SIP_ANSWER_CHALLENGE_CREDENTIALS_PARMS;

/* Structure for globally changing SIP service options */

typedef struct tSIP_SET_OPTIONS_PARMS {
    ACU_ULONG   size;
    ACU_UINT    option;
    ACU_POINTER option_value;
} ACU_PACK_DIRECTIVE SIP_SET_OPTIONS_PARMS;

/* Structure for globally changing SIP service options */

typedef struct tSIP_SET_GLOBAL_TOS_PARMS {
    ACU_ULONG   size;
    ACU_UCHAR   tos_value;
} ACU_PACK_DIRECTIVE SIP_SET_GLOBAL_TOS_PARMS;

/* Structure for dialling alternative contact on existing handle*/

typedef struct tSIP_RECALL_PARMS {
    ACU_ULONG size;
    ACU_CALL_HANDLE handle;
    char  *destination_addr;
} ACU_PACK_DIRECTIVE SIP_RECALL_PARMS;

/* Structure for disconnecting a call with additional details. Currently only redirect info*/
typedef struct tSIP_DISCONNECT_PARMS {
    ACU_ULONG size;
    ACU_CALL_HANDLE handle;
    ACU_INT sip_code;
    ACU_STRING_LIST  *redirect_contact_list;
    ACU_INT generic_cause;
    char  *custom_headers;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
} ACU_PACK_DIRECTIVE SIP_DISCONNECT_PARMS;

/* Structure for setting private key password for TLS*/
typedef struct tSIP_SET_TLS_PRIVATE_KEY_PASSWD_PARMS {
    ACU_ULONG size;
    char  *password;
    char  *reserved;
    ACU_UINT options;
} ACU_PACK_DIRECTIVE SIP_SET_TLS_PRIVATE_KEY_PASSWD_PARMS;

/* Structure for loading TLS certificates at runtime */
typedef struct tSIP_TLS_CONFIG_PARMS {
    ACU_ULONG size;
    char  *trusted_certificates_file;
    char  *server_certificates_file;
    char  *dh512_file;
    char  *dh1024_file;
    char  *password;
    ACU_INT reserved;
} ACU_PACK_DIRECTIVE SIP_TLS_CONFIG_PARMS;

/* Enum to describe resilient SIP server states */
typedef enum acu_rss_state {
    RSS_NONE,               // default - no resilience
    RSS_NOT_CONFIGURED,     // resilient but not configured yet
    RSS_PASSIVE,
    RSS_ACTIVE,
    RSS_FAILED,             // This server is no longer working
    RSS_NO_LICENCE
} ACU_RSS_STATE;

/* Structure to hold SIP server details */
typedef struct tRSS_SERVER_DETAILS {
    ACU_CHAR address[ACU_MAX_IP_ADDRESS];
    ACU_USHORT port;
    ACU_UINT state;
    ACU_CHAR heartbeat_present;
    ACU_CHAR server_alive;
    ACU_UINT calls;
    ACU_UINT error;
    struct tRSS_SERVER_DETAILS  *next;
} ACU_PACK_DIRECTIVE RSS_SERVER_DETAILS;

/* Structure for retrieving redundant SIP server details */
typedef struct tSIP_RSS_SERVER_DETAILS_PARMS {
    ACU_ULONG size;
    RSS_SERVER_DETAILS  *server_details;
} ACU_PACK_DIRECTIVE SIP_RSS_SERVER_DETAILS_PARMS;

/* Structure for stopping the active SIP server */
typedef struct tSIP_RSS_MAINTENANCE_PARMS {
    ACU_ULONG   size;
    ACU_INT     timeout;
    ACU_UINT    shutdown;
} ACU_PACK_DIRECTIVE SIP_RSS_MAINTENANCE_PARMS;

typedef struct tSIP_TARGET_REFRESH_PARMS {
    ACU_ULONG   size;
    ACU_CALL_HANDLE handle;
} ACU_PACK_DIRECTIVE SIP_TARGET_REFRESH_PARMS;

typedef struct tSIP_SEND_INVITE_RESPONSE_PARMS {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 sip_code;
    ACU_INT                 send_reliably;
    ACU_STRING_LIST         *contact_list;
    char                    *custom_headers;
    ACU_MEDIA_OFFER_ANSWER  *media_offer_answer;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
} ACU_PACK_DIRECTIVE SIP_SEND_INVITE_RESPONSE_PARMS;

typedef struct sip_sub_subscriber_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_PORT_ID             net;
    ACU_EVENT_QUEUE         queue_id;
    ACU_ACT                 app_context_token;
    ACU_INT                 response_notification;
    ACU_INT                 enable_response;
    ACU_INT                 expires;
    ACU_INT                 allow_multiple_dialogs;
    char                    *event_package_token;
    char                    *event_id_param;
    char                    *request_uri;
    char                    *to_header;
    char                    *from_header;
    char                    *contact_address;
    char                    *custom_headers;
    ACU_STRING_LIST         *body_types;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
} ACU_PACK_DIRECTIVE SIP_SUB_SUBSCRIBER_PARMS;

typedef struct sip_sub_notifier_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_PORT_ID             net;
    ACU_EVENT_QUEUE         queue_id;
    ACU_ACT                 app_context_token;
    ACU_INT                 request_notification;
    ACU_INT                 response_notification;
    ACU_INT                 enable_response;
    char                    *event_package_token;
} ACU_PACK_DIRECTIVE SIP_SUB_NOTIFIER_PARMS;

typedef struct sip_sub_notify_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    char                    *custom_headers;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
    char                    pending;
    char                    *event_id_param;
} ACU_PACK_DIRECTIVE SIP_SUB_NOTIFY_PARMS;

typedef struct sip_sub_cancel_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 dialog_id;
    ACU_INT                 response_code;
    char                    *reason;
    char                    *custom_headers;
    char                    *event_id_param;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
} 
ACU_PACK_DIRECTIVE SIP_SUB_CANCEL_PARMS;

typedef struct sip_sub_release_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
} 
ACU_PACK_DIRECTIVE SIP_SUB_RELEASE_PARMS;

typedef struct sip_sub_accept_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 expires;
    char                    *contact_address;
    char                    *custom_headers;
    char                    *event_id_param;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
    char                    acknowledge;
} ACU_PACK_DIRECTIVE SIP_SUB_ACCEPT_PARMS;

typedef struct sip_sub_fetch_parms {
    ACU_ULONG               size;
    ACU_CALL_HANDLE         handle;
    ACU_INT                 dialog_id;
    char                    *custom_headers;
    char                    *event_id_param;
    ACU_RAW_MESSAGE_BODY    *message_bodies;
} ACU_PACK_DIRECTIVE SIP_SUB_FETCH_PARMS;


/* structure for defining a host machine */
typedef struct sip_host_details {
    ACU_CHAR address[MAXHOSTADDRESS];
    ACU_UINT port;
    ACU_INT  transport_type;
} ACU_PACK_DIRECTIVE SIP_HOST_DETAILS;


typedef struct {
    ACU_ULONG               size;
    ACU_UINT                num_of_listen_params;
    SIP_HOST_DETAILS        listen_details[MAXEXTENDEDLISTENPARAMS];
} ACU_PACK_DIRECTIVE SIP_LISTEN_LIST_XPARMS;


/*-------------------------------*/
/* Enhanced Feature Structures   */
/*-------------------------------*/

typedef struct uui_xparms {
    ACU_INT         command;
    ACU_UINT        request;
    ACU_INT         tx_response;
    ACU_INT         rx_response;
    char            control;
    char            flow_control;
    ACU_UCHAR       protocol;
    char            more;
    ACU_UCHAR       length;
    ACU_UCHAR       data[MAXUUI_INFO];
} ACU_PACK_DIRECTIVE UUI_XPARMS;

typedef struct facility_xparms {
    ACU_INT         command;
    ACU_UCHAR       control;
    ACU_UCHAR       length;
    ACU_UCHAR       data[MAXFACILITY_INFO];
    /* a connectionless message may contain the following information */
    char            destination_addr[MAXNUM];
    char            originating_addr[MAXNUM];
    ACU_UCHAR       dest_subaddr[MAXNUM];
    ACU_UCHAR       dest_numbering_type;
    ACU_UCHAR       dest_numbering_plan;
    ACU_UCHAR       orig_numbering_type;
    ACU_UCHAR       orig_numbering_plan;
    ACU_UCHAR       orig_numbering_presentation;
    ACU_UCHAR       orig_numbering_screening;
    /* when multiple Facility information elements are present this
    * indicates how many more are present
    */
    ACU_UCHAR       more;
} ACU_PACK_DIRECTIVE FACILITY_XPARMS;

/* RFC2407 Add divertingNr structure for ETS DivertingLegInformation2 message */
typedef struct divertingNr {
    EPRESENTEDNUMBERUNSCREENED  presented_number_unscreened;
    EPARTYNUMBER                party_number;
    EPUBLICTYPEOFNUMBER         public_type_of_number;
    EPRIVATETYPEOFNUMBER        private_type_of_number;
} ACU_PACK_DIRECTIVE DIVERTINGNR;

/* RFC2407 Add originalCalledNr structure for ETS DivertingLegInformation2 message */
typedef struct originalCalledNr {
    EPRESENTEDNUMBERUNSCREENED  presented_number_unscreened;
    EPARTYNUMBER                party_number;
    EPUBLICTYPEOFNUMBER         public_type_of_number;
    EPRIVATETYPEOFNUMBER        private_type_of_number;
} ACU_PACK_DIRECTIVE ORIGINALCALLEDNR;

typedef struct diversion_xparms {
    ACU_UCHAR       diverting_reason;
    ACU_UCHAR       diverting_counter;
    char            diverting_to_addr[MAXNUM];
    char            diverting_from_addr[MAXNUM];
    char            original_called_addr[MAXNUM];
    ACU_UCHAR       diverting_from_type;
    ACU_UCHAR       diverting_from_plan;
    ACU_UCHAR       diverting_from_presentation;
    ACU_UCHAR       diverting_from_screening;
    ACU_UCHAR       diverting_indicator;            /* supported only by ISUP drivers */
    ACU_UCHAR       original_diverting_reason;      /* supported only by ISUP & QSIG / NI-2 drivers */
    ACU_UCHAR       diverting_to_type;              /* supported only by ISUP drivers */
    ACU_UCHAR       diverting_to_plan;              /* supported only by ISUP drivers */
    ACU_UCHAR       diverting_to_int_nw_indicator;  /* supported only by ISUP drivers */
    ACU_UCHAR       original_called_type;           /* supported only by ISUP / NI-2 drivers */
    ACU_UCHAR       original_called_plan;           /* supported only by ISUP / NI-2 drivers */
    ACU_UCHAR       original_called_presentation;   /* supported only by ISUP & QSIG / NI-2 drivers */
    ACU_INT         operation;                      /* supported only by QSIG & H.323 */
    ACU_INT         operation_type;                 /* supported only by QSIG */
    ACU_INT         error;                          /* supported only by QSIG */
    ACU_UCHAR       original_called_screening;      /* supported only by NI-2 drivers */
    ACU_UCHAR       diverting_to_presentation;      /* supported only by ISUP drivers */

    ACU_UCHAR       notification_options;           /* supported only by ISUP drivers */
    char            redirection_addr[MAXNUM];       /* supported only by H.323 */
    DIVERTINGNR     diverting_number;               /* RFC2407 */
    ORIGINALCALLEDNR original_called_number;        /* RFC2407 */
} ACU_PACK_DIRECTIVE DIVERSION_XPARMS;

typedef struct feature_hold_xparms {
    ACU_INT     command;
    ACU_INT     cause;
    union {
        struct {
            /* RETRIEVE in ETS300 196 uses timeslot information*/
            ACU_INT     ts;
            ACU_INT     raw;
            DISPLAY     display;
        } sig_q931;
        struct {
            ACU_INT     near_end_hold_status;
            ACU_INT     remote_hold_status;
            ACU_INT     media_hold_status;
        } sig_h323;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE  FEATURE_HOLD_XPARMS;

typedef struct feature_transfer_xparms {
    char    control;
    union {
        struct {
            ACU_INT     operation;
            ACU_INT     operation_type;
            ACU_INT     error;
            union {
                struct {
                    ACU_INT LinkID;
                } ets;
            } specific;
        } sig_q931;
        struct {
            ACU_INT     failure_code;
            ACU_CODEC   codecs[MAXCODECS];
        } sig_sip;
        struct {
            ACU_INT     operation_type;
            ACU_CHAR    link_id[MAXADDR];
            ACU_CHAR    destination_addr[MAXADDR];
            ACU_CHAR    destination_alias[MAXADDR];
            ACU_CHAR    originating_addr[MAXADDR];
            ACU_CHAR    originating_alias[MAXADDR];
            ACU_INT     reason;
            ACU_INT     operation;
        } ACU_PACK_DIRECTIVE sig_h323;
    } ACU_PACK_DIRECTIVE  unique_xparms;
} ACU_PACK_DIRECTIVE FEATURE_TRANSFER_XPARMS;

typedef struct raw_data_struct {
    ACU_INT     length;
    ACU_UCHAR   data[MAXRAWDATA];
    ACU_UCHAR   more;
    ACU_INT     nsf_length;
    ACU_UCHAR   nsf_data[MAXNETSPECIFICFACIL];
} ACU_PACK_DIRECTIVE RAW_DATA_STRUCT;

typedef struct mlpp_xparms {
    char        control;
    ACU_INT     operation;
    ACU_INT     operation_type;
    ACU_INT     error;
    ACU_INT     Prec_level;
    ACU_INT     LFB_Indictn;
    char        MLPP_Svc_Domn[5];
    ACU_INT     StatusRequest;
    ACU_INT     Preempt;
} ACU_PACK_DIRECTIVE MLPP_XPARMS;

typedef struct non_standard_data_xparms {
    ACU_INT     id_type;
    ACU_INT     length;
    ACU_CHAR    data[MAXRAWDATA];
    union {
        struct {
            ACU_USHORT  cc;
            ACU_USHORT  ext;
            ACU_USHORT  code;
        } h221_id;
        struct {
            ACU_USHORT  length;
            ACU_ULONG   id[MAXOID];
        } object_id;
    } id;
} ACU_PACK_DIRECTIVE NON_STANDARD_DATA_XPARMS;

typedef struct addressed_non_standard_data_xparms {
    ACU_INT   sequence_number;
    ACU_CHAR  remote_address[ACU_MAX_IP_ADDRESS];
    NON_STANDARD_DATA_XPARMS data;
} ACU_PACK_DIRECTIVE ADDRESSED_NON_STANDARD_DATA_XPARMS;

#define MAXRAWMSGDATA 536

/* 560 bytes */
typedef struct raw_msg_xparms {
    ACU_TOKEN       raw_msg_seq;
    ACU_USHORT      length;
    ACU_USHORT      spare1;
    ACU_UINT        spare2[1];
    ACU_UINT        lpc;
    ACU_UINT        rpc;
    ACU_UINT        cic;
    ACU_UCHAR       data[MAXRAWMSGDATA];
} ACU_PACK_DIRECTIVE RAW_MSG_XPARMS;

typedef struct notify_msg_xparms {
    ACU_INT         length;
    ACU_UCHAR       data[MAXNOTIFYMSG];
} ACU_PACK_DIRECTIVE NOTIFY_MSG_XPARMS;

typedef struct call_waiting_xparms {
    ACU_USHORT      waiting_calls;
    union {
        UNIQUEX_IPTEL   sig_iptel;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE CALL_WAITING_XPARMS;

typedef struct restart_channels_xparms {
    union {
        struct {
            ACU_UINT    restart_class;
            ACU_INT     ts;     /* included in case this is needed in future */
        } sig_q931;
    } unique_xparms;
} ACU_PACK_DIRECTIVE RESTART_CHANNELS_XPARMS;

typedef struct feature_activation_xparms {
    union {
        struct {
            ACU_UCHAR   ie[MAXFEATUREACTIVATION];
            ACU_UCHAR   message_type;
        } sig_q931;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE FEATURE_ACTIVATION_XPARMS;

typedef struct information_request_xparms {
    union {
        struct {
            ACU_UCHAR   ie[MAXINFORMATIONREQUEST];
            ACU_UCHAR   message_type;
        } sig_q931;
    } ACU_PACK_DIRECTIVE unique_xparms;
} ACU_PACK_DIRECTIVE INFORMATION_REQUEST_XPARMS;

typedef struct name_presentation_xparms {
    ACU_UCHAR   operation;                          // See OP_NAME_PRESENTATION_*
    ACU_UCHAR   type;                               // See NAME_PRESENTATION_TYPE_*
    ACU_UCHAR   data_length;                        // Number of octets stored in data[]
    ACU_UCHAR   data[MAX_NAME_PRESENTATION_DATA];
    ACU_UCHAR   character_set;                      // Only applicable when type is NAME_PRESENTATION_TYPE_*_EXTENDED
} ACU_PACK_DIRECTIVE NAME_PRESENTATION_XPARMS;

typedef union feature_union {
    UUI_XPARMS                  uui;
    FACILITY_XPARMS             facility;
    DIVERSION_XPARMS            diversion;
    FEATURE_HOLD_XPARMS         hold;
    FEATURE_TRANSFER_XPARMS     transfer;
    RAW_DATA_STRUCT             raw_data;
    MLPP_XPARMS                 mlpp;
    NON_STANDARD_DATA_XPARMS    non_standard;
    RAW_MSG_XPARMS              raw_msg;
    CALL_WAITING_XPARMS         call_waiting;
    RESTART_CHANNELS_XPARMS     restart;
    ADDRESSED_NON_STANDARD_DATA_XPARMS addressed_non_standard;
    NOTIFY_MSG_XPARMS           notify;
    KEYPAD_AS_FEATURE           keypadAsFeature;    
    FEATURE_ACTIVATION_XPARMS   feature_activation;
    INFORMATION_REQUEST_XPARMS  information_request;
    NAME_PRESENTATION_XPARMS    name_presentation;
} ACU_PACK_DIRECTIVE FEATURE_UNION;


typedef struct feature_detail_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_ULONG       feature_type;
    ACU_INT         message_control;
    FEATURE_UNION   feature;
} ACU_PACK_DIRECTIVE FEATURE_DETAIL_XPARMS;

typedef struct feature_enable_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_ULONG       feature_type;
    ACU_INT         enable;
} ACU_PACK_DIRECTIVE FEATURE_ENABLE_XPARMS;

/* open for outgoing structure with feature information */
typedef struct feature_out_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_PORT_ID     net;
    ACU_INT         ts;
    ACU_INT         cnf;
    ACU_INT         sending_complete;
    char            destination_addr[MAXADDR];
    char            originating_addr[MAXADDR];
    ACU_ULONG       feature_information;
    ACU_INT         message_control;
    ACU_ACT         app_context_token;
    ACU_EVENT_QUEUE queue_id;
    union           uniquex unique_xparms;
    FEATURE_UNION   feature;
} ACU_PACK_DIRECTIVE FEATURE_OUT_XPARMS;


typedef struct tOPEN_PORT_PARMS {
    ACU_ULONG       size;
    ACU_CARD_ID     card_id;
    ACU_UINT        port_ix;
    ACU_PORT_ID     port_id;
    ACU_EVENT_QUEUE queue_id;
} ACU_PACK_DIRECTIVE OPEN_PORT_PARMS;


typedef struct tCLOSE_PORT_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
} ACU_PACK_DIRECTIVE CLOSE_PORT_PARMS;


/* interrogate dsp fitted per port */

typedef struct dsp_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_INT         config;
    ACU_INT         dspa_type;
    ACU_CHAR        dspa_desc[MAX_HW_DESC];     /* hardware description */
    ACU_CHAR        dspa_fw_desc[MAX_HW_DESC];  /* firmware running on the DSP */
    ACU_INT         dspb_type;
    ACU_CHAR        dspb_desc[MAX_HW_DESC];     /* hardware description */
    ACU_CHAR        dspb_fw_desc[MAX_HW_DESC];  /* firmware running on the DSP */
} ACU_PACK_DIRECTIVE DSP_XPARMS;

typedef struct call_port_notification_parms {
    ACU_ULONG       size;       /* IN */
    ACU_PORT_ID     port_id;    /* IN/OUT */
    ACU_INT         event;      /* OUT */
} ACU_PACK_DIRECTIVE CALL_PORT_NOTIFICATION_PARMS;

typedef struct call_card_notification_parms {
    ACU_ULONG       size;       /* IN */
    ACU_CARD_ID     card_id;    /* IN/OUT */
    ACU_INT         event;      /* OUT */
} ACU_PACK_DIRECTIVE CALL_CARD_NOTIFICATION_PARMS;

typedef struct _PORT_DIAGNOSTICS_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port;
    ACU_ULONG       memory;
    ACU_ULONG       handles_allocated;
    ACU_ULONG       msgs_allocated;
    ACU_ULONG       icds_allocated;
    ACU_ULONG       rejected_calls;
} ACU_PACK_DIRECTIVE PORT_DIAGNOSTICS_PARMS;

typedef struct _CARD_DIAGNOSTICS_PARMS {
    ACU_ULONG       size;
    ACU_CARD_ID     card_id;
    ACU_ULONG       memory;
    ACU_ULONG       apps_connected;
} ACU_PACK_DIRECTIVE CARD_DIAGNOSTICS_PARMS;

typedef struct _CARD_INFO_PARMS {
    ACU_ULONG       size;
    ACU_CARD_ID     card_id;
    ACU_UINT        ports;
    ACU_UINT        card_type;
    ACU_ULONG       irq_ticks;
    ACU_ULONG       clock_ticks;
    ACU_CHAR        serial_no[MAXSERIALNO];
    ACU_CHAR        hw_version[MAXHWVERSION];
    ACU_ULONG       interrupts_seen;
    ACU_ULONG       interrupts_claimed;
} ACU_PACK_DIRECTIVE CARD_INFO_PARMS;

typedef struct _CALL_CARD_WAIT_OBJECT_PARMS {
    ACU_ULONG       size;
    ACU_CARD_ID     card_id;
    ACU_WAIT_OBJECT wait_object;
} ACU_PACK_DIRECTIVE CALL_CARD_WAIT_OBJECT_PARMS;

typedef struct _CALL_HANDLE_WAIT_OBJECT_PARMS {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_WAIT_OBJECT wait_object;
} ACU_PACK_DIRECTIVE CALL_HANDLE_WAIT_OBJECT_PARMS;

typedef struct _CALL_GLOBAL_WAIT_OBJECT_PARMS {
    ACU_ULONG       size;
    ACU_WAIT_OBJECT wait_object;
} ACU_PACK_DIRECTIVE CALL_GLOBAL_WAIT_OBJECT_PARMS;

typedef struct restart_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_CHAR        *filenamep;
    ACU_CHAR        *config_stringp;
    ACU_CHAR        *dspa_filenamep;        /* Not used, set to NULL */
    ACU_CHAR        *dspb_filenamep;        /* Not used, set to NULL */
} ACU_PACK_DIRECTIVE RESTART_XPARMS;

typedef struct reconfig_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_CHAR        *switch_stringp;
} ACU_PACK_DIRECTIVE RECONFIG_XPARMS;

typedef struct call_reattach_parms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_CHAR        config_string[CL_MAX_FMW_PARMS];
} ACU_PACK_DIRECTIVE CALL_REATTACH_PARMS;

typedef struct _CALL_TRACE_FLAG_PARMS {
    ACU_ULONG       size;
    ACU_RESOURCE_ID resource;
    ACU_UINT        trace_flag;
    ACU_UINT        mode;
} ACU_PACK_DIRECTIVE ACU_PACK_DIRECTIVE CALL_TRACE_FLAG_PARMS;

typedef struct _CALL_PORT_WAIT_OBJECT_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_WAIT_OBJECT wait_object;
} ACU_PACK_DIRECTIVE CALL_PORT_WAIT_OBJECT_PARMS;

typedef struct _CALL_SET_PORT_NAME_PARMS {
    ACU_ULONG   size;               /* IN */
    ACU_PORT_ID port_id;            /* IN */
    ACU_CHAR    name[MAXNAME];      /* IN */
} ACU_PACK_DIRECTIVE CALL_SET_PORT_NAME_PARMS;

typedef void (*ACU_VERBOSE_CALLBACK)(ACU_RESOURCE_ID context, char* msg);

typedef struct tCALL_SET_GLOBAL_WAIT_QUEUE_PARMS {
    ACU_UINT        size;
    ACU_EVENT_QUEUE queue_id;
} ACU_PACK_DIRECTIVE CALL_SET_GLOBAL_WAIT_QUEUE_PARMS;

typedef struct tCALL_GET_GLOBAL_NOTIFICATION_PARMS {
    ACU_UINT    size;
    ACU_UINT    event;
    void        *context;
} ACU_PACK_DIRECTIVE CALL_GET_GLOBAL_NOTIFICATION_PARMS;

typedef struct tCALL_GET_GLOBAL_NOTIFICATION_WAIT_OBJECT_PARMS {
    ACU_UINT        size;
    ACU_WAIT_OBJECT wait_object;
} ACU_PACK_DIRECTIVE CALL_GET_GLOBAL_NOTIFICATION_WAIT_OBJECT_PARMS;

typedef struct tCALL_API_VERSION_PARMS {
    ACU_UINT size;                  /* IN */
    ACU_UINT major;                 /* OUT */
    ACU_UINT minor;                 /* OUT */
    ACU_UINT rev;                   /* OUT */
    ACU_CHAR desc[MAX_VER_DESC];    /* OUT */
} ACU_PACK_DIRECTIVE CALL_API_VERSION_PARMS;

typedef struct tCALL_ENABLE_API_TRACE_PARMS {
    ACU_ULONG   size;
    ACU_UINT    enable;
} ACU_PACK_DIRECTIVE CALL_ENABLE_API_TRACE_PARMS;

typedef struct tACU_OPEN_CALL_PARMS {
    ACU_ULONG   size;
    ACU_CARD_ID card_id;    /* IN */
} ACU_PACK_DIRECTIVE ACU_OPEN_CALL_PARMS;

typedef struct tACU_CLOSE_CALL_PARMS {
    ACU_ULONG   size;
    ACU_CARD_ID card_id; /* IN */
} ACU_PACK_DIRECTIVE ACU_CLOSE_CALL_PARMS;

typedef struct tCALL_DSP_INFO {
    ACU_UINT    dsp_type;                       /* Type of DSP fitted */
    ACU_CHAR    status[MAX_HW_DESC];

    /* For the PMX, "status" contains the dsp firmware file name currently downloaded */
    ACU_CHAR    dsp_desc[MAX_HW_DESC];
} CALL_DSP_INFO;

typedef struct tCALL_CARD_DSP_CONFIG_PARMS {
    ACU_ULONG       size;                           /* IN */
    ACU_CARD_ID     card_id;                        /* IN */
    ACU_UINT        dsp_count;                      /* OUT */
    CALL_DSP_INFO   dsp_info[MAX_CARD_DSP_COUNT];   /* OUT */

    /* The following elements are for debug purposes only. */
    ACU_UCHAR       port_dsp_map[MAX_DSPMAN_PORTS]; /* OUT - DSP associated with port */
    ACU_UCHAR       port_res_map[MAX_DSPMAN_PORTS]; /* OUT - DSP resource associated with port */
} ACU_PACK_DIRECTIVE CALL_CARD_DSP_CONFIG_PARMS;

typedef struct tCALL_STOP_FMW_PARMS {
    ACU_ULONG       size;                           /* IN */
    ACU_PORT_ID     port_id;                        /* IN */
} ACU_PACK_DIRECTIVE CALL_STOP_FMW_PARMS;

typedef struct tCALL_DSP_STREAM_PARMS {
    ACU_ULONG   size;       /* IN */
    ACU_PORT_ID port_id;    /* IN */
    ACU_UINT    stream;     /* OUT */
} ACU_PACK_DIRECTIVE CALL_DSP_STREAM_PARMS;

/** Retrieve firmware download parameters */
typedef struct tCALL_GET_FMW_DL_PARMS {
    ACU_UINT    size;                       /**< IN size of struct */
    ACU_PORT_ID port_id;                    /**< IN port to query */
    ACU_CHAR    parms[CL_MAX_FMW_PARMS];    /**< OUT firmware parameters */
} ACU_PACK_DIRECTIVE CALL_GET_FMW_DL_PARMS;

/** Get the call driver version */
typedef struct tCALL_DRIVER_VERSION_PARMS {
    ACU_ULONG   size;                           /* IN */
    ACU_PORT_ID port_id;                        /* IN */
    ACU_UINT    ver_maj;                        /* OUT */
    ACU_UINT    ver_min;                        /* OUT */
    ACU_UINT    ver_rev;                        /* OUT */
    ACU_CHAR    ver_desc[MAX_VER_DESC + 1];     /* OUT */
} ACU_PACK_DIRECTIVE CALL_DRIVER_VERSION_PARMS;

typedef struct tCALL_OPEN_IPTEL_PORT_PARMS {
    ACU_ULONG       size;                                   /*IN*/
    ACU_INT         protocol_type;                          /*IN*/
    ACU_PORT_ID     port_id;                                /*OUT*/
} ACU_PACK_DIRECTIVE CALL_OPEN_IPTEL_PORT_PARMS;

enum tACU_CALL_CARD_CONFIGURATION_TYPE {
    ACU_CALL_CARD_CONFIG_GENERIC    = 0
};

typedef struct tCALL_CARD_CONFIGURATION_OPTION_PARMS {
    ACU_ULONG   size;
    ACU_CARD_ID card_id;
    ACU_UINT    config_type;
    ACU_UINT    config_no;
    ACU_UINT    flags;
    ACU_CHAR    option[ACU_CALL_CONFIG_OPTION_LEN];
    ACU_CHAR    value[ACU_CALL_CONFIG_OPTION_LEN];
    ACU_CHAR    description[ACU_CALL_CONFIG_DESC_LEN];
} ACU_PACK_DIRECTIVE CALL_CARD_CONFIGURATION_OPTION_PARMS;

typedef struct tCALL_IPTEL_PROTOCOL_CONFIG {
    ACU_CODEC   codecs[MAXCODECS];
    union {
        struct {
            ACU_INT  h245_tunneling;
            ACU_INT  faststart;
            ACU_INT  early_h245;
        } sig_h323;
        struct {
            ACU_INT  zero_connection_address_hold;
            ACU_INT  disable_early_media;
        } sig_sip;
    } protocol_specific;
} ACU_PACK_DIRECTIVE CALL_IPTEL_PROTOCOL_CONFIG;

typedef struct tCALL_PORT_CONFIGURATION_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    union {
        CALL_IPTEL_PROTOCOL_CONFIG  iptel;
    } unique_xparms;
} ACU_PACK_DIRECTIVE CALL_PORT_CONFIGURATION_XPARMS;

/* H.323 registration xparms*/
typedef struct tCALL_REGISTER_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_INT         ttl;
    ACU_CHAR        registration_address[MAXADDR];
    ACU_CHAR        gatekeeper_id[MAXID];
    ACU_INT         registration_mode;
} ACU_PACK_DIRECTIVE CALL_REGISTER_XPARMS;

typedef struct tCALL_CLEAR_REGISTRATION_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
} ACU_PACK_DIRECTIVE CALL_CLEAR_REGISTRATION_XPARMS;

typedef struct tCALL_QUERY_REGISTRATION_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID port_id;
    ACU_INT         registration_status;
    ACU_CHAR        registration_address[MAXADDR];
} ACU_PACK_DIRECTIVE CALL_QUERY_REGISTRATION_XPARMS;

typedef struct tCALL_HOST_DETAILS {
    ACU_CHAR address[MAXADDR];
    ACU_UINT port;
    ACU_INT  transport_type;
} ACU_PACK_DIRECTIVE CALL_HOST_DETAILS;

/* unique SIP add alias */
typedef struct tCALL_SIP_ADD_ALIAS {
    CALL_HOST_DETAILS registrar;
    ACU_CHAR     admin_sip_url[MAXADDR];
    ACU_CHAR     contact[MAXADDR];
} ACU_PACK_DIRECTIVE CALL_SIP_ADD_ALIAS;

/* structure for adding aliases */
typedef struct tCALL_ADD_ALIAS_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID port_id;
    ACU_INT         protocol;
    ACU_UINT        registration_handle;
    ACU_CHAR        alias[MAXADDR];
    union {
      struct {
          CALL_SIP_ADD_ALIAS    sip_add_alias;
      } sig_sip;

      struct {
          ACU_INT   prefix;
      } sig_h323;
    } unique_xparms;
} ACU_PACK_DIRECTIVE CALL_ADD_ALIAS_XPARMS;

/* structure for removing and deleting aliases */
typedef struct tCALL_REMOVE_ALIAS_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UINT        protocol;
    ACU_UINT        registration_handle;
} ACU_PACK_DIRECTIVE CALL_REMOVE_ALIAS_XPARMS;

/* structure for querying aliases */
typedef struct tCALL_QUERY_ALIAS_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UINT        protocol;
    ACU_UINT        registration_handle;
    ACU_CHAR        alias[MAXADDR];
    ACU_INT         prefix;
    ACU_UINT        state;
    ACU_UINT        error;
} ACU_PACK_DIRECTIVE CALL_QUERY_ALIAS_XPARMS;

/* structure for querying set of registered aliases */
typedef struct tCALL_SNAPSHOT_REGISTRATIONS_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID port_id;
    ACU_UINT        protocol;
    ACU_UINT        count;
    ACU_UINT        handles[MAXREGISTRATIONS];
} ACU_PACK_DIRECTIVE CALL_SNAPSHOT_REGISTRATIONS_XPARMS;

typedef struct tCALL_DTMF_HANDLING_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     unused;
    ACU_CALL_HANDLE handle;
    ACU_UINT        enable_event_notification;
    ACU_UINT        relay_disabled;
} ACU_PACK_DIRECTIVE CALL_DTMF_HANDLING_XPARMS;

typedef struct tCALL_PROTOCOL_DEFAULTS_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UINT        protocol;
    union {
        struct {
            ACU_CODEC       default_codecs[MAXCODECS];
            union {
                struct {
                    ACU_INT  h245_tunneling;
                    ACU_INT  faststart;
                    ACU_INT  early_h245;
                } sig_h323;
                struct {
                    ACU_INT  zero_connection_address_hold;
                    ACU_INT  disable_early_media;
                } sig_sip;
            } protocol_specific;
        } sig_iptel;
    } unique_xparms;
} ACU_PACK_DIRECTIVE CALL_PROTOCOL_DEFAULTS_XPARMS;

typedef struct tCALL_H323_LISTEN_ADDRESSES_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_CHAR        h225_address[MAXHOSTADDRESS];
    ACU_UINT        h225_port;
    ACU_CHAR        ras_address[MAXHOSTADDRESS];
    ACU_UINT        ras_port;
} ACU_PACK_DIRECTIVE CALL_H323_LISTEN_ADDRESSES_XPARMS;

typedef struct tCALL_H323_STOP_LISTEN_XPARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_INT         protocol;
} ACU_PACK_DIRECTIVE CALL_H323_STOP_LISTEN_XPARMS;


typedef struct acu_alias_list {
    ACU_INT         no_of_aliases;
    ACU_CHAR        aliases[MAXVOIPALIASESS][MAXADDR];
} ACU_PACK_DIRECTIVE ACU_ALIAS_LIST;

typedef struct h323_detail_xparms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_LONG        timeout;
    ACU_INT         valid;
    ACU_INT         stream;
    ACU_INT         ts;
    ACU_INT         calltype;
    ACU_CHAR        destination_addr[MAXADDR];
    ACU_CHAR        originating_addr[MAXADDR];
    ACU_CHAR        connected_addr[MAXADDR];
    ACU_ACT         app_context_token;
    ACU_ULONG       feature_information;
    ACU_CHAR        destination_display_name[MAXDISPLAY];
    ACU_CHAR        originating_display_name[MAXDISPLAY];
    ACU_CODEC       codecs[MAXCODECS];
    MEDIA_SETTINGS  media_settings;
    ACU_POINTER     vmprxid;
    ACU_POINTER     vmptxid;
    ACU_CHAR        media_call_type[MAXMEDIACALLTYPE];
    ACU_CHAR        destination_alias[MAXADDR];
    ACU_CHAR        originating_alias[MAXADDR];
    ACU_INT         h245_tunneling;
    ACU_INT         faststart;
    ACU_INT         early_h245;
    ACU_CHAR        dtmf[MAXNUM];
    ACU_INT         progress_location;
    ACU_INT         progress_description;
    ACU_ALIAS_LIST  originating_aliases;
    ACU_ALIAS_LIST  destination_aliases;
} ACU_PACK_DIRECTIVE H323_DETAIL_XPARMS;

typedef struct h323_gateway_mode_parms {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_INT         mode;
} ACU_PACK_DIRECTIVE H323_GATEWAY_MODE_PARMS;

/*
 *  Structures for mid-call media negotiation
 */
typedef struct call_change_media_accept_parms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        union {           
            struct {
                ACU_CHAR rtp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT rtp_port;
                ACU_CHAR rtcp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT rtcp_port;
            } sig_h323;         
        } sig_iptel;
    } unique_xparms;
} ACU_PACK_DIRECTIVE CALL_CHANGE_MEDIA_ACCEPT_PARMS;

typedef enum h323_media_change_reject_reasons {
    H323_MEDIA_REJECT_MODE_UNAVAILABLE        = 1,
    H323_MEDIA_REJECT_MULTIPOINT_CONSTRAINT   = 2,
    H323_MEDIA_REJECT_REQUEST_DENIED          = 3
} ACU_H323_MEDIA_CHANGE_REJECT_REASONS;

typedef enum t38_fax_transport_type {
    T38_FAX_TRANSPORT_UDP = 1,
    T38_FAX_TRANSPORT_TCP = 2
} ACU_T38_FAX_TRANSPORT_TYPE;

typedef enum t38_fax_version {
    T38_FAX_VERSION_1 = 1,
    T38_FAX_VERSION_2 = 2,
    T38_FAX_VERSION_3 = 3
} ACU_T38_FAX_VERSION;

typedef enum t38_fax_error_correction {
    T38_FAX_ERROR_CORRECTION_FEC        = 1,
    T38_FAX_ERROR_CORRECTION_REDUNDANCY = 2
} ACU_T38_FAX_ERROR_CORRECTION;

typedef enum t38_fax_rate_management {
    T38_FAX_RATE_MANAGEMENT_LOCAL_TCF       = 1,
    T38_FAX_RATE_MANAGEMENT_TRANSFERRED_TCF = 2
} ACU_T38_FAX_RATE_MANAGEMENT;

typedef struct call_change_media_reject_parms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    ACU_UINT        reason;
} ACU_PACK_DIRECTIVE CALL_CHANGE_MEDIA_REJECT_PARMS;

typedef struct call_change_media_parms {
    ACU_ULONG       size;
    ACU_CALL_HANDLE handle;
    union {
        union {           
            struct {
                ACU_CHAR local_rtp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT local_rtp_port;
                ACU_CHAR local_rtcp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT local_rtcp_port;
                ACU_CHAR remote_rtp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT remote_rtp_port;
                ACU_CHAR remote_rtcp_address[ACU_MAX_IP_ADDRESS];
                ACU_UINT remote_rtcp_port;
                union {
                    struct {
                        struct {
                            ACU_UINT    transport_type;
                            ACU_UINT    bit_rate;
                            ACU_UINT    fill_bit_removal;
                            ACU_UINT    transcoding_jbig;
                            ACU_UINT    transcoding_mmr;
                            ACU_UINT    version;
                            ACU_UINT    fax_rate_management;
                            ACU_UINT    udp_max_buffer;
                            ACU_UINT    udp_max_datagram;
                            ACU_UINT    udp_error_correction;
                            ACU_UINT    tcp_bidirectional;
                        } t38Fax;
                    } data;                 
                } media;
                ACU_UINT reject_reason;
            } sig_h323;         
        } sig_iptel;
    } unique_xparms;
} ACU_PACK_DIRECTIVE CALL_CHANGE_MEDIA_PARMS;

/*
 *  Structures for trace gathering method.
 */
typedef struct tSTART_TRACE_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UINT        flags;
    ACU_UINT        extra_flags;        /** protocol specific flags */
    ACU_UINT        delay;
    ACU_CHAR        filename[MAX_TRACE_FILENAME];
} ACU_PACK_DIRECTIVE START_TRACE_PARMS;

typedef struct tTRACE_MODE_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UINT        flags;
    ACU_UINT        extra_flags;        /** protocol specific flags */
} ACU_PACK_DIRECTIVE TRACE_MODE_PARMS;

typedef struct tCALL_PORT_RXTX_PARMS {
    ACU_ULONG       size;
    ACU_PORT_ID     port_id;
    ACU_UCHAR       flags;
} ACU_PACK_DIRECTIVE CALL_PORT_RXTX_PARMS;

/*
 *  End of trace structures
 */

#ifndef DRIVER

ACU_ERR ACU_EXPORT call_openout_65          ( OUT_XPARMS * );
#define call_openout call_openout_65
ACU_ERR ACU_EXPORT call_openin_65           ( IN_XPARMS *  );
#define call_openin call_openin_65
ACU_ERR ACU_EXPORT call_event                ( STATE_XPARMS * );
ACU_ERR ACU_EXPORT call_details_65          ( DETAIL_XPARMS * );
#define call_details call_details_65
ACU_ERR ACU_EXPORT call_get_charge           ( GET_CHARGE_XPARMS * );
ACU_ERR ACU_EXPORT call_put_charge           ( PUT_CHARGE_XPARMS * );
ACU_ERR ACU_EXPORT call_send_overlap         ( OVERLAP_XPARMS * );
ACU_ERR ACU_EXPORT call_incoming_ringing     ( ACU_CALL_HANDLE handle );
ACU_ERR ACU_EXPORT call_accept               ( ACU_CALL_HANDLE handle );
ACU_ERR ACU_EXPORT call_hold                 ( ACU_CALL_HANDLE handle );
ACU_ERR ACU_EXPORT call_reconnect            ( ACU_CALL_HANDLE handle );
ACU_ERR ACU_EXPORT call_enquiry_65          ( OUT_XPARMS * );
#define call_enquiry call_enquiry_65
ACU_ERR ACU_EXPORT call_transfer             ( TRANSFER_XPARMS * );
ACU_ERR ACU_EXPORT call_answercode           ( CAUSE_XPARMS * );
ACU_ERR ACU_EXPORT call_get_originating_addr ( ACU_CALL_HANDLE handle );
ACU_ERR ACU_EXPORT call_getcause             ( CAUSE_XPARMS * );
ACU_ERR ACU_EXPORT call_disconnect           ( CAUSE_XPARMS * );
ACU_ERR ACU_EXPORT call_release              ( CAUSE_XPARMS * );
ACU_ERR ACU_EXPORT call_force_release        ( FORCE_RELEASE_XPARMS * );
ACU_ERR ACU_EXPORT call_port_2_swdrvr        ( ACU_PORT_ID );
ACU_ERR ACU_EXPORT call_port_2_stream        ( ACU_PORT_ID );
ACU_ERR ACU_EXPORT call_handle_2_io          ( ACU_CALL_HANDLE );
ACU_ERR ACU_EXPORT call_handle_2_port        ( ACU_CALL_HANDLE );
ACU_ERR ACU_EXPORT call_restart_fmw          ( RESTART_XPARMS * );
ACU_ERR ACU_EXPORT call_send_alarm           ( ALARM_XPARMS * );
ACU_ERR ACU_EXPORT call_watchdog             ( WATCHDOG_XPARMS * );
ACU_ERR ACU_EXPORT call_l1_stats             ( L1_XSTATS * );
ACU_ERR ACU_EXPORT call_l2_state             ( L2_XSTATE * );
ACU_ERR ACU_EXPORT call_type                 ( ACU_PORT_ID );
ACU_ERR ACU_EXPORT call_line                 ( ACU_PORT_ID );
ACU_ERR ACU_EXPORT call_is_download          ( ACU_PORT_ID );
ACU_ERR ACU_EXPORT call_protocol_trace       ( LOG_XPARMS * );
ACU_ERR ACU_EXPORT call_dcba                 ( DCBA_XPARMS * );
ACU_ERR ACU_EXPORT call_progress             ( PROGRESS_XPARMS * );
ACU_ERR ACU_EXPORT call_proceeding           ( PROCEEDING_XPARMS * );
ACU_ERR ACU_EXPORT call_setup_ack            ( SETUP_ACK_XPARMS * );
ACU_ERR ACU_EXPORT call_notify               ( NOTIFY_XPARMS * );
ACU_ERR ACU_EXPORT call_dsp_config           ( DSP_XPARMS * );
ACU_ERR ACU_EXPORT xcall_incoming_ringing    ( INCOMING_RINGING_XPARMS * );
ACU_ERR ACU_EXPORT xcall_accept              ( ACCEPT_XPARMS * );
ACU_ERR ACU_EXPORT xcall_getcause            ( DISCONNECT_XPARMS * );
ACU_ERR ACU_EXPORT xcall_disconnect          ( DISCONNECT_XPARMS * );
ACU_ERR ACU_EXPORT xcall_release             ( DISCONNECT_XPARMS * );
ACU_ERR ACU_EXPORT xcall_hold                ( HOLD_XPARMS * );
ACU_ERR ACU_EXPORT xcall_reconnect           ( HOLD_XPARMS * );
ACU_ERR ACU_EXPORT xcall_get_originating_addr ( GET_ORIGINATING_ADDR_XPARMS * );
ACU_ERR ACU_EXPORT call_send_keypad_info     ( KEYPAD_XPARMS * );

/*-----------------------------------*/
/* DPNSS Feature Function Prototypes */
/*-----------------------------------*/
ACU_ERR ACU_EXPORT dpns_openout_65          (DPNS_OUT_XPARMS            *outdetailsp);
#define dpns_openout dpns_openout_65
ACU_ERR ACU_EXPORT dpns_send_overlap        (DPNS_OVERLAP_XPARMS        *overlapp);
ACU_ERR ACU_EXPORT dpns_call_details        (DPNS_DETAIL_XPARMS         *detailsp);
ACU_ERR ACU_EXPORT dpns_incoming_ringing    (DPNS_INCOMING_RING_XPARMS  *inringp);
ACU_ERR ACU_EXPORT dpns_call_accept         (DPNS_CALL_ACCEPT_XPARMS    *acceptp);
ACU_ERR ACU_EXPORT dpns_send_feat_info      (DPNS_FEATURE_XPARMS        *featurep);
ACU_ERR ACU_EXPORT dpns_set_transit         (ACU_CALL_HANDLE            handle);
ACU_ERR ACU_EXPORT xdpns_set_transit        (DPNS_SET_TRANSIT_XPARMS    *transitp);
ACU_ERR ACU_EXPORT dpns_transit_details     (DPNS_TRANSIT_XPARMS        *transitp);
ACU_ERR ACU_EXPORT dpns_send_transit        (DPNS_TRANSIT_XPARMS        *transitp);
ACU_ERR ACU_EXPORT dpns_disconnect          (DPNS_CAUSE_XPARMS          *causep);
ACU_ERR ACU_EXPORT dpns_release             (DPNS_CAUSE_XPARMS          *causep);
ACU_ERR ACU_EXPORT dpns_getcause            (DPNS_CAUSE_XPARMS          *causep);
ACU_ERR ACU_EXPORT dpns_set_l2_ch           (DPNS_L2_XPARMS             *dpns_l2_parms);
ACU_ERR ACU_EXPORT dpns_l2_state            (DPNS_L2_XPARMS             *dpns_l2_parms);


/*--------------------------------------*/
/* Enhanced Feature Function Prototypes */
/*--------------------------------------*/
ACU_ERR ACU_EXPORT call_feature_openout_65      (FEATURE_OUT_XPARMS *);
#define call_feature_openout call_feature_openout_65
ACU_ERR ACU_EXPORT call_feature_enquiry_65      (FEATURE_OUT_XPARMS *);
#define call_feature_enquiry call_feature_enquiry_65
ACU_ERR ACU_EXPORT call_feature_details         (FEATURE_DETAIL_XPARMS *);
ACU_ERR ACU_EXPORT call_feature_send            (FEATURE_DETAIL_XPARMS *);
ACU_ERR ACU_EXPORT call_send_connectionless     (FEATURE_DETAIL_XPARMS *);
ACU_ERR ACU_EXPORT call_get_connectionless      (FEATURE_DETAIL_XPARMS *);
ACU_ERR ACU_EXPORT call_enable_connectionless   (FEATURE_ENABLE_XPARMS *);

/*--------------------------------------*/
/* Maintenance-oriented prototypes      */
/* - only available for ISUP at present.*/
/*--------------------------------------*/
ACU_ERR ACU_EXPORT call_maint_ts_block  ( TS_BLOCKING_XPARMS * );
ACU_ERR ACU_EXPORT call_maint_ts_unblock  ( TS_BLOCKING_XPARMS * );
ACU_ERR ACU_EXPORT call_maint_port_block  ( PORT_BLOCKING_XPARMS * );
ACU_ERR ACU_EXPORT call_maint_port_unblock  ( PORT_BLOCKING_XPARMS * );
ACU_ERR ACU_EXPORT call_maint_port_reset  ( PORT_RESET_XPARMS * );
ACU_ERR ACU_EXPORT call_maint_port_status  ( PORT_STATUS_XPARMS * );

/*--------------------------------------*/
/* Call driver configuration functions  */
/*--------------------------------------*/
ACU_ERR ACU_EXPORT call_set_card_configuration_option(CALL_CARD_CONFIGURATION_OPTION_PARMS *config_parms);
ACU_ERR ACU_EXPORT call_get_card_configuration_option(CALL_CARD_CONFIGURATION_OPTION_PARMS *config_parms);
ACU_ERR ACU_EXPORT call_enum_card_configuration_options(CALL_CARD_CONFIGURATION_OPTION_PARMS *config_parms);


/* wait object code */
ACU_ERR ACU_EXPORT call_get_global_event_wait_object(CALL_GLOBAL_WAIT_OBJECT_PARMS *wo_parms);
ACU_ERR ACU_EXPORT call_get_handle_event_wait_object(CALL_HANDLE_WAIT_OBJECT_PARMS *wo_parms);
ACU_ERR ACU_EXPORT call_port_2_call(ACU_PORT_ID port_id);


ACU_ERR ACU_EXPORT  port_init   ( ACU_PORT_ID portnum );
ACU_ERR ACU_EXPORT  idle_net_ts ( ACU_PORT_ID portnum, int timeslot );

const char* ACU_EXPORT sigtype_2_string ( int sigtype );
const char* ACU_EXPORT error_2_string( int error );
const char* ACU_EXPORT call_port_notification_2_string(ACU_UINT notification);
const char* ACU_EXPORT call_ext_event_2_string(ACU_LONG extended_state);
const char* ACU_EXPORT call_event_2_string(ACU_LONG state);
const char* ACU_EXPORT call_feature_2_string(ACU_ULONG feature);
const char* ACU_EXPORT call_linestat_2_string(ACU_INT lstat_flag);

ACU_ERR ACU_EXPORT trace_get_snapshot        ( ACU_PORT_ID port_id );
ACU_ERR ACU_EXPORT call_dump_api_trace(void);
ACU_ERR ACU_EXPORT call_set_trace_file(ACU_CHAR *filename);
ACU_ERR ACU_EXPORT call_set_trace_flag(ACU_UINT);

int ACU_EXPORT get_last_state( ACU_CALL_HANDLE handle );

/* V6 API calls */
ACU_ERR ACU_EXPORT call_port_info(PORT_INFO_PARMS *port_info_xparms);
ACU_ERR ACU_EXPORT call_open_port(OPEN_PORT_PARMS *port_parms);
ACU_ERR ACU_EXPORT call_close_port(CLOSE_PORT_PARMS *port_parms);
ACU_ERR ACU_EXPORT call_get_port_configuration(CALL_PORT_CONFIGURATION_XPARMS *config_parms);
ACU_ERR ACU_EXPORT call_set_port_configuration(CALL_PORT_CONFIGURATION_XPARMS *config_parms);
ACU_ERR ACU_EXPORT call_get_card_info(CARD_INFO_PARMS *info_parms);
ACU_ERR ACU_EXPORT call_get_port_notification(CALL_PORT_NOTIFICATION_PARMS *eventp);
ACU_ERR ACU_EXPORT call_get_port_notification_wait_object(CALL_PORT_WAIT_OBJECT_PARMS *wo_parms);
ACU_ERR ACU_EXPORT call_get_card_notification(CALL_CARD_NOTIFICATION_PARMS *eventp);
ACU_ERR ACU_EXPORT call_get_card_notification_wait_object(CALL_CARD_WAIT_OBJECT_PARMS *wo_parms);
ACU_INT ACU_EXPORT call_get_port_trace(V6_TRACE_BUFFER *buffer);
ACU_ERR ACU_EXPORT call_get_port_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS *token_parms);
#define call_get_port_app_context_token call_get_port_app_context_token_65
ACU_ERR ACU_EXPORT call_set_port_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS *token_parms);
#define call_set_port_app_context_token call_set_port_app_context_token_65
ACU_ERR ACU_EXPORT call_get_handle_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS *token_parms);
#define call_get_handle_app_context_token call_get_handle_app_context_token_65
ACU_ERR ACU_EXPORT call_set_handle_app_context_token_65(ACU_APP_CONTEXT_TOKEN_PARMS *token_parms);
#define call_set_handle_app_context_token call_set_handle_app_context_token_65
ACU_ERR ACU_EXPORT call_set_handle_event_queue(ACU_QUEUE_PARMS *queue_parms);
ACU_ERR ACU_EXPORT call_set_port_notification_queue(ACU_QUEUE_PARMS *queue_parms);
ACU_ERR ACU_EXPORT call_set_port_default_handle_event_queue(ACU_QUEUE_PARMS *queue_parms);
ACU_ERR ACU_EXPORT call_set_card_notification_queue(ACU_QUEUE_PARMS *queue_parms);
ACU_ERR ACU_EXPORT call_code_point(char *message);
ACU_INT ACU_EXPORT call_get_card_trace(V6_TRACE_BUFFER *buffer);
ACU_ERR ACU_EXPORT call_set_port_name(CALL_SET_PORT_NAME_PARMS *name_parms);
ACU_ERR ACU_EXPORT call_set_global_notification_queue(CALL_SET_GLOBAL_WAIT_QUEUE_PARMS *queue_parms);
ACU_ERR ACU_EXPORT call_get_global_notification_wait_object(CALL_GET_GLOBAL_NOTIFICATION_WAIT_OBJECT_PARMS *wo_parms);
ACU_ERR ACU_EXPORT call_get_global_notification(CALL_GET_GLOBAL_NOTIFICATION_PARMS *notification_parms);
ACU_ERR ACU_EXPORT call_api_version(CALL_API_VERSION_PARMS *version);
ACU_ERR ACU_EXPORT call_get_fmw_dl_parms(CALL_GET_FMW_DL_PARMS *fmw_dl_parms);
ACU_ERR ACU_EXPORT call_card_dsp_config(CALL_CARD_DSP_CONFIG_PARMS *config_parms);
ACU_ERR ACU_EXPORT call_get_port_dsp_stream(CALL_DSP_STREAM_PARMS *stream_parms);
ACU_ERR ACU_EXPORT call_stop_fmw(CALL_STOP_FMW_PARMS *stop_parms);
ACU_ERR ACU_EXPORT call_driver_version(CALL_DRIVER_VERSION_PARMS *version_parms);
ACU_ERR ACU_EXPORT call_open_iptel_port(CALL_OPEN_IPTEL_PORT_PARMS *port_parms);

ACU_ERR ACU_EXPORT call_set_h323_gatekeeper(CALL_REGISTER_XPARMS *regdetailsp);
ACU_ERR ACU_EXPORT call_query_h323_gatekeeper(CALL_QUERY_REGISTRATION_XPARMS *queryp);
ACU_ERR ACU_EXPORT call_clear_h323_gatekeeper(CALL_CLEAR_REGISTRATION_XPARMS *clearp);
ACU_ERR ACU_EXPORT call_add_alias(CALL_ADD_ALIAS_XPARMS *add_aliasp);
ACU_ERR ACU_EXPORT call_remove_alias(CALL_REMOVE_ALIAS_XPARMS *remove_aliasp);
ACU_ERR ACU_EXPORT call_delete_alias(CALL_REMOVE_ALIAS_XPARMS *delete_aliasp);
ACU_ERR ACU_EXPORT call_query_alias(CALL_QUERY_ALIAS_XPARMS *query_aliasp);
ACU_ERR ACU_EXPORT call_snapshot_registrations(CALL_SNAPSHOT_REGISTRATIONS_XPARMS *snapshotp);
ACU_ERR ACU_EXPORT call_set_dtmf_handling(CALL_DTMF_HANDLING_XPARMS *dtmf_handlingp);
ACU_ERR ACU_EXPORT call_set_protocol_defaults(CALL_PROTOCOL_DEFAULTS_XPARMS *protocol_defaultsp);
ACU_ERR ACU_EXPORT call_get_protocol_defaults(CALL_PROTOCOL_DEFAULTS_XPARMS *protocol_defaultsp);
ACU_ERR ACU_EXPORT call_set_h323_listen_addresses(CALL_H323_LISTEN_ADDRESSES_XPARMS *listenp);
ACU_ERR ACU_EXPORT call_get_h323_listen_addresses(CALL_H323_LISTEN_ADDRESSES_XPARMS *listenp);
ACU_ERR ACU_EXPORT call_h323_stop_listening(CALL_H323_STOP_LISTEN_XPARMS *listenp);
ACU_ERR ACU_EXPORT call_media_details(CALL_CHANGE_MEDIA_PARMS *detailsp);
ACU_ERR ACU_EXPORT call_change_media(CALL_CHANGE_MEDIA_PARMS *mediap);
ACU_ERR ACU_EXPORT call_change_media_accept(CALL_CHANGE_MEDIA_ACCEPT_PARMS *media_acceptp);
ACU_ERR ACU_EXPORT call_change_media_reject(CALL_CHANGE_MEDIA_REJECT_PARMS *media_rejectp);

/*--------------------------------------*/
/* Additional H.323 Function Prototypes */
/*--------------------------------------*/

ACU_ERR ACU_EXPORT h323_call_details(H323_DETAIL_XPARMS *detailsp);
ACU_ERR ACU_EXPORT h323_gateway_mode(H323_GATEWAY_MODE_PARMS *modep);

/*-----------------------------------*/
/* Extended SIP  Function Prototypes */
/*-----------------------------------*/

ACU_ERR ACU_EXPORT sip_open_port(ACU_PORT_ID *sip_port);
ACU_ERR ACU_EXPORT sip_openout_65(SIP_OUT_PARMS *out_parms);
#define sip_openout sip_openout_65
ACU_ERR ACU_EXPORT sip_openin_65(SIP_IN_PARMS *in_parms);
#define sip_openin sip_openin_65
ACU_ERR ACU_EXPORT sip_accept(SIP_ACCEPT_PARMS *accept_parms);
ACU_ERR ACU_EXPORT sip_details_65(SIP_DETAIL_PARMS *detail_parms);
#define sip_details sip_details_65
ACU_ERR ACU_EXPORT sip_free_details_65(SIP_DETAIL_PARMS *detail_parms);
#define sip_free_details sip_free_details_65
ACU_ERR ACU_EXPORT sip_incoming_ringing(SIP_INCOMING_RINGING_PARMS *ringing_parms);
ACU_ERR ACU_EXPORT sip_progress(SIP_PROGRESS_PARMS *progress_parms);
ACU_ERR ACU_EXPORT sip_feature_send(SIP_FEATURE_DETAIL_PARMS *feature_parms);
ACU_ERR ACU_EXPORT sip_media_propose(SIP_MEDIA_PROPOSE_PARMS *media_propose_parms);
ACU_ERR ACU_EXPORT sip_media_accept(SIP_MEDIA_ACCEPT_PARMS *media_accept_parms);
ACU_ERR ACU_EXPORT sip_media_request_proposal(SIP_MEDIA_REQUEST_PROPOSAL_PARMS *media_request_proposal_parms);
ACU_ERR ACU_EXPORT sip_media_reject_proposal(SIP_MEDIA_REJECT_PROPOSAL_PARMS *media_reject_proposal_parms);
ACU_ERR ACU_EXPORT sip_send_request(SIP_SEND_REQUEST_PARMS *send_request_parms);
ACU_ERR ACU_EXPORT sip_send_response(SIP_SEND_RESPONSE_PARMS *send_response_parms);
ACU_ERR ACU_EXPORT sip_set_reason_phrase(SIP_SET_REASON_PHRASE_PARMS *set_reason_phrase_parms);
ACU_ERR ACU_EXPORT sip_add_answer_challenge_credentials(SIP_ANSWER_CHALLENGE_CREDENTIALS_PARMS *answer_challenge_credentials_parms);
ACU_ERR ACU_EXPORT sip_remove_answer_challenge_credentials(SIP_ANSWER_CHALLENGE_CREDENTIALS_PARMS *answer_challenge_credentials_parms);
ACU_ERR ACU_EXPORT sip_set_options(SIP_SET_OPTIONS_PARMS *set_options_parms);
ACU_ERR ACU_EXPORT sip_recall(SIP_RECALL_PARMS *recall_parms);
ACU_ERR ACU_EXPORT sip_disconnect(SIP_DISCONNECT_PARMS *disconnect_parms);
ACU_ERR ACU_EXPORT sip_read_request(SIP_READ_MESSAGE_PARMS *read_parms);
ACU_ERR ACU_EXPORT sip_free_message(SIP_READ_MESSAGE_PARMS *free_parms);
ACU_ERR ACU_EXPORT sip_read_response(SIP_READ_MESSAGE_PARMS *read_parms);
ACU_ERR ACU_EXPORT sip_read_out_of_dialog_failure(SIP_READ_MESSAGE_PARMS *read_parms);
ACU_ERR ACU_EXPORT sip_set_message_notification(SIP_MESSAGE_NOTIFICATION_PARMS *message_notification_parms);
ACU_ERR ACU_EXPORT sip_set_tls_private_key_password(SIP_SET_TLS_PRIVATE_KEY_PASSWD_PARMS *sip_set_tls_private_key_password_parms);
ACU_ERR ACU_EXPORT sip_send_out_of_dialog_request(SIP_SEND_OUT_OF_DIALOG_REQUEST_PARMS *sip_ood_request);
ACU_ERR ACU_EXPORT sip_send_out_of_dialog_response(SIP_SEND_OUT_OF_DIALOG_RESPONSE_PARMS *sip_ood_response);
ACU_ERR ACU_EXPORT sip_load_tls_configuration(SIP_TLS_CONFIG_PARMS *sip_tls_config);
ACU_ERR ACU_EXPORT sip_rss_get_server_details(SIP_RSS_SERVER_DETAILS_PARMS *sip_server_details_parms);
ACU_ERR ACU_EXPORT sip_rss_maintenance(SIP_RSS_MAINTENANCE_PARMS *sip_maintenance_parms);
ACU_ERR ACU_EXPORT sip_target_refresh(SIP_TARGET_REFRESH_PARMS *sip_target_refresh_parms);
ACU_ERR ACU_EXPORT sip_send_invite_response(SIP_SEND_INVITE_RESPONSE_PARMS *sip_send_invite_response_parms);

/* sip_set_global_tos() is for internal use only. */
ACU_ERR ACU_EXPORT sip_set_global_tos(SIP_SET_GLOBAL_TOS_PARMS *set_global_tos_parms);
ACU_ERR ACU_EXPORT sip_sub_subscriber(SIP_SUB_SUBSCRIBER_PARMS *sip_sub_subscriber_parms);
ACU_ERR ACU_EXPORT sip_sub_notifier(SIP_SUB_NOTIFIER_PARMS *sip_sub_notifier_parms);
ACU_ERR ACU_EXPORT sip_sub_notify(SIP_SUB_NOTIFY_PARMS *sip_sub_notify_parms);
ACU_ERR ACU_EXPORT sip_sub_cancel(SIP_SUB_CANCEL_PARMS *sip_sub_cancel_parms);
ACU_ERR ACU_EXPORT sip_sub_release(SIP_SUB_RELEASE_PARMS *sip_sub_release_parms);
ACU_ERR ACU_EXPORT sip_sub_accept(SIP_SUB_ACCEPT_PARMS *sip_sub_accept_parms);
ACU_ERR ACU_EXPORT sip_sub_fetch(SIP_SUB_FETCH_PARMS *sip_sub_fetch_parms);
ACU_ERR ACU_EXPORT sip_get_listen_list(SIP_LISTEN_LIST_XPARMS *sip_listen_listp);

/* Enable/Disable API trace.  Currently, can only be called before acu_open_call(), and can only disable the API trace */
ACU_ERR ACU_EXPORT acu_call_enable_api_trace(CALL_ENABLE_API_TRACE_PARMS *enablep);
/* initialise call control for a given card */
ACU_ERR ACU_EXPORT acu_open_call(ACU_OPEN_CALL_PARMS *openp);

/* shut down call control for a given card */
ACU_ERR ACU_EXPORT acu_close_call(ACU_CLOSE_CALL_PARMS *closep);

/*
*   Functions for trace gathering method.
*/
ACU_ERR ACU_EXPORT call_start_trace( START_TRACE_PARMS *start_trace_info );
ACU_ERR ACU_EXPORT call_stop_trace( ACU_PORT_ID port_id );
ACU_ERR ACU_EXPORT call_set_trace_mode( TRACE_MODE_PARMS *trace_mode_info );
ACU_ERR ACU_EXPORT call_is_trace_thread_running(void);

/*
*   Resilience routines.
*/
ACU_ERR ACU_EXPORT call_get_failover_id(CALL_GET_FAILOVER_ID_PARMS *parms);
ACU_ERR ACU_EXPORT call_reopen(CALL_REOPEN_PARMS *parms);
ACU_ERR ACU_EXPORT call_reattach_fmw(CALL_REATTACH_PARMS *reattach_parms);
ACU_ERR ACU_EXPORT call_release_lost_failover_ids(CALL_RELEASE_LOST_FAILOVER_IDS_PARMS *parms);

ACU_ERR ACU_EXPORT call_reconfig_fmw ( RECONFIG_XPARMS *reconfig );

ACU_ERR ACU_EXPORT call_set_rxtx(CALL_PORT_RXTX_PARMS *rxtx);

ACU_ERR ACU_EXPORT register_verbose_callback(ACU_VERBOSE_CALLBACK func);


/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/
/*                                                                      */
/*                      DEPRECATED API CALLS                            */
/*                                                                      */
/*         THESE API CALLS WILL BE REMOVED IN A FUTURE RELEASE.         */
/*                                                                      */
/* For each deprecated API call, there is a comment describing the best */
/* alternative.                                                         */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/************************************************************************/
/************************************************************************/

/* Use call_driver_version() instead of call_version(). */
ACU_ERR ACU_DEPRECATED ACU_EXPORT call_version(ACU_PORT_ID port_id);

/* Associate an application context token with each call handle instead of using this function */
ACU_ERR ACU_DEPRECATED ACU_EXPORT call_handle_2_chan(ACU_CALL_HANDLE handle);

/* Retrieve transaction ID of a timed out out of dialog SIP transaction */
/* Replaced by sip_read_out_of_dialog_failure which also reports transport layer errors */
ACU_ERR ACU_DEPRECATED ACU_EXPORT sip_read_timeout(SIP_READ_MESSAGE_PARMS *free_parms);

typedef struct dpns_wd_xparms {
    ACU_ULONG       size;
    ACU_PORT_ID     net;
    ACU_LONG        timeout;
} ACU_PACK_DIRECTIVE DPNS_WD_XPARMS;

/* Use call_watchdog() instead */
ACU_ERR ACU_DEPRECATED ACU_EXPORT dpns_watchdog(DPNS_WD_XPARMS *dpns_watchdogp);
/* Use call_set_trace_mode() */
ACU_ERR ACU_DEPRECATED ACU_EXPORT trace_set_mode( ACU_PORT_ID port_id, int trace_flag);



#endif /* #ifndef DRIVER */

#ifndef ACU_LINUX
#ifndef ACU_QNX
#pragma pack (pop)
#endif
#endif /* ACU_LINUX */


#ifdef __cplusplus
}
#endif



#endif /* CL_TYPE_H_INCLUDED */
