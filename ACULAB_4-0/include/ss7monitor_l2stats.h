/* #ident "@(#) (c) Aculab plc $Header: /home/cvs/repository/ss7/stack/src/monitor/ss7monitor_l2stats.h,v 1.3 2018/03/02 11:59:43 davidla Exp $ $Name: SS7_6_16_1 $" */

#ifndef L2STATUS_H_INCLUDED
#define L2STATUS_H_INCLUDED

#ifdef __cplusplus
extern "C"{
#endif

/**** MTP2 ioctl definitions *****/

/* SS7_IOC_LINK_MTP2_STATS Returns the counters that are available
 *                         through the maintenance API.
 * SS7_IOC_LINK_MTP2       Returns the internal state variables of mtp2
 *                         displayed by ss7maint linkstatus -2vvv.
 * SS7_IOC_LINK_TRC_FLTR   Controls the line and protocol trace.
 *
 * Request with:
 *    ss7mon_ioctl_req(ep, interface_id, SS7_IOC_LINK_xxx, &rqst, sizeof rqst)
 * (rqst is NULL for the status requests).
 * Response will be of type ACU_SS7MON_MSG_IOC_RESPONSE(SS7_IOC_LINK_xxx)
 * check mm_status and look in mm_buffer.
 */

/* l2_tc_state */
#define L2TXC_IDLE              0
#define L2TXC_IN_SERVICE        1
#define L2TXC_RETRIEVING        2

/* l2_rc_state */
#define L2RC_IDLE               0
#define L2RC_IN_SERVICE         1
#define L2RC_ACCEPT_MSU         2

/* l2_rc_congestion */
#define CONGESTION_NONE         0
#define CONGESTION_ACCEPT       1
#define CONGESTION_DISCARD      2

/* l2_iac_state */
#define L2IAC_IDLE              0
#define L2IAC_NOT_ALIGNED       1
#define L2IAC_ALIGNED           2
#define L2IAC_PROVING           3

/* l2_lsc_state */
#define L2LSCS_POWER_OFF                0
#define L2LSCS_OUT_OF_SERVICE           1
#define L2LSCS_INITIAL_ALIGNMENT        2
#define L2LSCS_ALIGNED_READY            3
#define L2LSCS_IN_SERVICE               5
#define L2LSCS_PROCESSOR_OUTAGE         6

/* l2_poc_state */
#define L2POC_LPO                       1       /* Local processor outage */
#define L2POC_RPO                       2       /* Remote processor outage */
#define L2POC_BPO                       (L2POC_LPO | L2POC_RPO)


#define SS7_IOC_LINK_MTP2_STATS 0x601       /* mtp2 statistics */
#define SS7_IOC_LINK_MTP2       0x602       /* mtp2 info */
#define SS7_IOC_LINK_TRC_FLTR   0x603       /* trace control */

/* For SS7_IOC_LINK_TRC_FLTR */
typedef struct {
    unsigned char  trf_pad;
    unsigned char  trf_set_tx;                 /* tx trace flags */
    unsigned char  trf_set_rx;                 /* rx trace flags */
#define FW_TRACE_FISU            1
#define FW_TRACE_LSSU            2
#define FW_TRACE_MSU             4
#define FW_TRACE_V6              8
#define FW_TRACE_MSU_INSEQ    0x10
#define FW_TRACE_MSU_OUTSEQ   0x20
    unsigned char  trf_set_fw;                 /* firmware/function trace */
#define FW_TRACE_FN     1
    unsigned char  trf_clr_tx;                 /* bits to clear... */
    unsigned char  trf_clr_rx;
    unsigned char  trf_clr_fw;
} ss7_ioc_link_trc_fltr_t;

/* For SS7_IOC_LINK_MTP2_STATS (for maintenance API)
 * NB: these are big_endian */
typedef struct {
    unsigned int l2_reason;              /* ACU_SS7_MTP_LINK_P_L2_* */
    unsigned int l2_stats[32];           /* Indexed by ACU_SS7_AMRC_* */
} ss7_ioc_link_mtp2_stats;

/* For SS7_IOC_LINK_MTP2
 * Note: limited to 100 bytes by DPR 'synch' interface.
 * NB: these are little_endian.
 */
typedef
#if defined(_MW_SCC_VERSION_) && _MW_SCC_VERSION_ < 0x260
__PACK__
#endif
struct {
    unsigned short l2t_interval;           /* in ms */
    unsigned short l2t_expiry;             /* time to expiry in ms - ~0 if stopped */
} l2t_t;

typedef struct {
    unsigned char  l2_ts;                  /* Timeslot to switch matrix */
    unsigned char  l2_high_speed;          /* Extra timeslots for high speed links*/
    unsigned char  l2_lsc_state;
    unsigned char  l2_poc_state;
    unsigned char  l2_iac_state;
    unsigned char  l2_tc_state;
    unsigned char  l2_rc_state;
    unsigned char  l2_cc_state;
    unsigned char  l2_aerm_state;
    unsigned char  l2_suerm_state;

    l2t_t   l2_lsc_t1;
    l2t_t   l2_iac_t2;
    l2t_t   l2_iac_t3;
    l2t_t   l2_iac_t4;
    l2t_t   l2_cc_t5;
    l2t_t   l2_tc_t6;
    l2t_t   l2_tc_t7;

    unsigned short l2_tx_fisu;             /* total fisu transmitted */
    unsigned short l2_tx_lssu;             /* total lssu transmitted */
    unsigned short l2_tx_msu;              /* total msu transmitted */
    unsigned short l2_retx_msu;            /* total msu retransmitted */
    unsigned short l2_tx_qlen;             /* total msu queued (tx + retx) */
    unsigned short l2_rx_fisu;             /* total fisu received */
    unsigned short l2_rx_lssu;             /* total lssu received */
    unsigned short l2_rx_msu;              /* total msu received */
    unsigned short l2_rx_bad;              /* total bad su received */
    unsigned short l2_rx_qlen;             /* total msu queued for mtp3 */

    unsigned char  l2_lsc_emergency_proving;

    unsigned char  l2_iac_emergency_proving;
    unsigned char  l2_iac_further_proving;
    unsigned char  l2_iac_Cp;
    unsigned short l2_iac_Pe;
    unsigned short l2_iac_Pn;

    unsigned char  l2_aerm_Ca;
    unsigned char  l2_aerm_Ti;

    unsigned short l2_suerm_error_count;

    unsigned short l2_tc_tcoc_tid;
    unsigned char  l2_tc_lssu_available;
    unsigned char  l2_tc_msu_inhibited;
    unsigned char  l2_tc_sib_received;
    unsigned char  l2_tc_status_indication;
    unsigned char  l2_tc_flush_sent;
    unsigned char  l2_tc_Cm;
    unsigned char  l2_tc_fib;
    unsigned char  l2_tc_fsnf_1;
    unsigned char  l2_tc_fsnl;
    unsigned char  l2_tc_fsnt;
    unsigned char  l2_tc_fsnx_1_bib;
    unsigned char  l2_tc_xmit_fifo;

    unsigned char  l2_rc_abnormal_bsnr;
    unsigned char  l2_rc_abnormal_fibr;
    unsigned char  l2_rc_abnormal_fsnr;
    unsigned char  l2_rc_congestion;       /* none/accept/discard */
    unsigned char  l2_rc_msu_fisu_accept;
    unsigned char  l2_rc_rtr;
    unsigned char  l2_rc_fsnf_1;           /* last received bsn byte */
    unsigned char  l2_rc_fsnx_1;           /* fsn byte expected for fisu */
    unsigned char  l2_rc_fsnt;             /* fsn byte of last tx */

    unsigned char  l2_rc_lssu;             /* type of last lssu received */

    unsigned short l2_prot_rx_qlen;
    unsigned char  l2_hdlc_rx_qlen;
    unsigned char  l2_pad[3];              /* pad to 100 bytes, pad is zeros */
} ss7_ioc_link_mtp2;

typedef char foo[sizeof (ss7_ioc_link_mtp2) == 100 ? 1 : -1];


#ifdef __cplusplus
}
#endif

#endif
