/* rtcp_parser.c - RTCP parsing functions */

#include "rtcp_parser.h"
#include <memory.h>

// general stuff

static unsigned char *get16(uint_fast32_t *vp, unsigned char *s)
{
 uint_fast32_t v = *s++ << 8;
 v |= *s++;
 *vp = v;
 return s;
}

static unsigned char *get16u(unsigned *vp, unsigned char *s)
{
	uint_fast32_t v;

	s = get16(&v,s);

	*vp = (unsigned) v;

	return s;
}

static unsigned char *get24(uint_fast32_t *vp, unsigned char *s)
{
 uint_fast32_t v = *s++ << 16;
 v |= *s++ << 8;
 v |= *s++;
 *vp = v;
 return s;
}

static unsigned char *get32(uint_fast32_t *vp, unsigned char *s)
{
 uint_fast32_t v = *s++ << 24;
 v |= *s++ << 16;
 v |= *s++ << 8;
 v |= *s++;
 *vp = v;
 return s;
}

static unsigned char *get32l(uint_fast32_t *vp, unsigned char *s)
{
 uint_fast32_t v = *s++;
 v |= *s++ << 8;
 v |= *s++ << 16;
 v |= *s++ << 24;
 *vp = v;
 return s;
}

static unsigned char *get64(uint_fast64_t *vp, unsigned char *s)
{
 uint_fast64_t v = (uint_fast64_t) *s++ << 56;
 v |= (uint_fast64_t) *s++ << 48;
 v |= (uint_fast64_t) *s++ << 40;
 v |= (uint_fast64_t) *s++ << 32;
 v |= (uint_fast64_t) *s++ << 24;
 v |= (uint_fast64_t) *s++ << 16;
 v |= *s++ << 8;
 v |= *s++;
 *vp = v;
 return s;
}

// starting with the whole RTCP packet

int rtcp_analyse(RTCP *rtcp_out, unsigned char *rtcp)
{
 uint_fast32_t len;
 rtcp_out->start = rtcp;
 rtcp_out->v = (*rtcp >> 6) & 3;
 rtcp_out->p = (*rtcp >> 5) & 1;
 rtcp_out->count = *rtcp++ & 0x1f;
 rtcp_out->pt = *rtcp++;
 get16(&len, rtcp);
 len = (len + 1) * 4;	// convert to octets
 rtcp_out->length = len;
 rtcp_out->end = rtcp_out->start + rtcp_out->length;
 if (rtcp_out->p) {	// trim off padding
 	rtcp_out->end -= rtcp_out->end[-1];
 }
 return 0;
}

// a sender report

int rtcp_analyse_sr(RTCP_SR *sr, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + 4;
 dp = get32(&sr->ssrc, dp);
 dp = get64(&sr->ntp_timestamp, dp);
 dp = get32(&sr->rtp_timestamp, dp);
 dp = get32(&sr->senders_packet_count, dp);
 dp = get32(&sr->senders_octet_count, dp);
 return 0;
}

static unsigned sr_report_block_offset(unsigned blockno)
{
 return 28 + 24 * blockno;
}

int rtcp_analyse_sr_report_block(RTCP_SR_REPORT_BLOCK *rb, unsigned block_index, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + sr_report_block_offset(block_index);
 dp = get32(&rb->ssrc, dp);
 rb->fraction_lost = *dp++;
 dp = get24(&rb->packets_lost, dp);
 dp = get32(&rb->ehsnr, dp);
 dp = get32(&rb->jitter, dp);
 dp = get32(&rb->lsr, dp);
 dp = get32(&rb->dlsr, dp);
 return 0;
}

int rtcp_get_sr_pse(unsigned char **ptr, unsigned *len, RTCP *rtcp)
{
 *ptr = rtcp->start + sr_report_block_offset(rtcp->count);
 *len = (unsigned) (rtcp->end - *ptr);
 return 0;
}

// a receiver report

int rtcp_analyse_rr(RTCP_RR *rr, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + 4;
 dp = get32(&rr->ssrc, dp);
 return 0;
}

static unsigned rr_report_block_offset(unsigned blockno)
{
 return 8 + 24 * blockno;
}

int rtcp_analyse_rr_report_block(RTCP_RR_REPORT_BLOCK *rb, unsigned block_index, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + rr_report_block_offset(block_index);
 dp = get32(&rb->ssrc, dp);
 rb->fraction_lost = *dp++;
 dp = get24(&rb->packets_lost, dp);
 dp = get32(&rb->ehsnr, dp);
 dp = get32(&rb->jitter, dp);
 dp = get32(&rb->lsr, dp);
 dp = get32(&rb->dlsr, dp);
 return 0;
}

int rtcp_get_rr_pse(unsigned char **ptr, unsigned *len, RTCP *rtcp)
{
 *ptr = rtcp->start + rr_report_block_offset(rtcp->count);
 *len = (unsigned) (rtcp->end - *ptr);
 return 0;
}

// a SDES

int rtcp_analyse_sdes(RTCP_SDES_CHUNK *op, RTCP *rtcp)
{
 op->next_item = rtcp->start;
 return 0;
}

int rtcp_analyse_sdes_next_item(RTCP_SDES_CHUNK *op)
{
 unsigned char *dp = op->next_item;
 op->item_type = *dp;
 if (op->item_type) {
 	op->item_length = *++dp;
 	op->item_data = ++dp;
 	op->next_item = dp + op->item_length;
 }
 return 0;
}

int rtcp_analyse_sdes_next_chunk(RTCP_SDES_CHUNK *op, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + ((op->next_item + 4 - rtcp->start) & ~3);
 dp = get32(&op->ssrc_csrc, dp);
 op->next_item = dp;
 return rtcp_analyse_sdes_next_item(op);
}

// a BYE

static unsigned bye_item_offset(unsigned itemno)
{
 return 4 + 4 * itemno;
}

int rtcp_analyse_bye(RTCP_BYE *bye, RTCP *rtcp)
{
 unsigned srcend = bye_item_offset(rtcp->count);
 if (srcend > rtcp->length) {
	unsigned char *dp = rtcp->start + srcend;
	bye->reason_length = *dp++;
	bye->reason_text = dp;
 } else {
	bye->reason_length = 0;
	bye->reason_text = 0;
 }
 return 0;
}

int rtcp_analyse_bye_ssrc_csrc(RTCP_BYE_SSRC_CSRC *op, unsigned itemno, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + bye_item_offset(itemno);
 dp = get32(&op->ssrc_csrc, dp);
 return 0;
}

// an APP

int rtcp_analyse_app(RTCP_APP *app, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + 4;
 dp = get32(&app->ssrc_csrc, dp);
 memcpy(app->name, dp, 4); dp += 4;
 app->data = dp;
 app->data_length = (unsigned) (rtcp->end - app->data);
 return 0;
}

// an XR

int rtcp_analyse_xr(RTCP_XR *xr, RTCP *rtcp)
{
 unsigned char *dp = rtcp->start + 4;
 xr->curblock = get32(&xr->ssrc, dp);
 xr->xr_end = rtcp->end;
 return 0;
}

int rtcp_analyse_xr_next_block(RTCP_XR *xr)
{
 if (xr->xr_end - xr->curblock >= 4) {
	uint_fast32_t len;
	xr->blocktype = *xr->curblock;
	get16(&len, xr->curblock + 2);
	xr->curblockend = xr->curblock + (len + 1) * 4;
 } else {
	xr->blocktype = ~0u;
 }
 return 0;
}

int rtcp_analyse_xr_bt_1(RTCP_XR_BT_1 *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock;
 xrb->t = xr->curblock[1] & 0xf;
 dp = get32(&xrb->ssrc, dp);
 dp = get16(&xrb->begin_seq, dp);
 dp = get16(&xrb->end_seq, dp);
 xrb->chunks = dp;
 xrb->numchunks = ((unsigned)(xr->curblockend - dp)) / 2;
 xr->curblock = xr->curblockend;
 return 0;
}

int rtcp_analyse_xr_bt_1_chunk(RTCP_XR_BT_1_CHUNK *xrb1, RTCP_XR_BT_1 *xrb, unsigned chunk_index)
{
 get16(&xrb1->chunk, xrb->chunks + chunk_index * 2);
 return 0;
}

int rtcp_analyse_xr_bt_2(RTCP_XR_BT_2 *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock;
 xrb->t = xr->curblock[1] & 0xf;
 dp = get32(&xrb->ssrc, dp);
 dp = get16(&xrb->begin_seq, dp);
 dp = get16(&xrb->end_seq, dp);
 xrb->chunks = dp;
 xrb->numchunks = ((unsigned)(xr->curblockend - dp)) / 2;
 xr->curblock = xr->curblockend;
 return 0;
}

int rtcp_analyse_xr_bt_2_chunk(RTCP_XR_BT_2_CHUNK *xrb2, RTCP_XR_BT_2 *xrb, unsigned chunk_index)
{
 get32(&xrb2->chunk, xrb->chunks + chunk_index * 2);
 return 0;
}

int rtcp_analyse_xr_bt_3(RTCP_XR_BT_3 *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock;
 xrb->t = xr->curblock[1] & 0xf;
 dp = get32(&xrb->ssrc, dp);
 dp = get16(&xrb->begin_seq, dp);
 dp = get16(&xrb->end_seq, dp);
 xrb->times = dp;
 xrb->numtimes = ((unsigned)(xr->curblockend - dp)) / 2;
 xr->curblock = xr->curblockend;
 return 0;
}

int rtcp_analyse_xr_bt_3_time(RTCP_XR_BT_3_TIME *xrb3, RTCP_XR_BT_3 *xrb, unsigned time_index)
{
 get32(&xrb3->time, xrb->times + time_index * 4);
 return 0;
}

int rtcp_analyse_xr_bt_4(RTCP_XR_BT_4 *xrb, RTCP_XR *xr)
{
 get64(&xrb->ntp_timestamp, xr->curblock + 4);
 xr->curblock = xr->curblockend;
 return 0;
}

int rtcp_analyse_xr_bt_5(RTCP_XR_BT_5 *xrb, RTCP_XR *xr)
{
 xrb->subblocks = ((unsigned)(xr->curblockend - xr->curblock - 4)) / 12;
 return 0;
}

int rtcp_analyse_xr_bt_5_subblock(RTCP_XR_BT_5_SUBBLOCK *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock;
 dp = get32(&xrb->ssrc, dp);
 dp = get32(&xrb->lrr, dp);
 dp = get32(&xrb->dlrr, dp);
 xr->curblock = dp;
 return 0;
}

int rtcp_analyse_xr_bt_6(RTCP_XR_BT_6 *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock + 4;
 xrb->l = (xr->curblock[1] >> 7) & 1;
 xrb->d = (xr->curblock[1] >> 6) & 1;
 xrb->j = (xr->curblock[1] >> 5) & 1;
 xrb->ToH = (xr->curblock[1] >> 3) & 3;
 dp = get32(&xrb->ssrc, dp);
 dp = get16(&xrb->begin_seq, dp);
 dp = get16(&xrb->end_seq, dp);
 dp = get32(&xrb->lost_packets, dp);
 dp = get32(&xrb->dup_packets, dp);
 dp = get32(&xrb->min_jitter, dp);
 dp = get32(&xrb->max_jitter, dp);
 dp = get32(&xrb->mean_jitter, dp);
 dp = get32(&xrb->dev_jitter, dp);
 xrb->min_ttl_or_hl = *dp++;
 xrb->max_ttl_or_hl = *dp++;
 xrb->mean_ttl_or_hl = *dp++;
 xrb->dev_ttl_or_hl = *dp++;
 xr->curblock = xr->curblockend;
 return 0;
}

int rtcp_analyse_xr_bt_7(RTCP_XR_BT_7 *xrb, RTCP_XR *xr)
{
 unsigned char *dp = xr->curblock + 4;
 dp = get32(&xrb->ssrc, dp);
 xrb->loss_rate = *dp++;
 xrb->discard_rate = *dp++;
 xrb->burst_density = *dp++;
 xrb->gap_density = *dp++;
 dp = get16u(&xrb->burst_duration, dp);
 dp = get16u(&xrb->gap_duration, dp);
 dp = get16u(&xrb->round_trip_delay, dp);
 dp = get16u(&xrb->end_system_delay, dp);
 xrb->signal_level = *dp++;
 xrb->noise_level = *dp++;
 xrb->RERL = *dp++;
 xrb->Gmin = *dp++;
 xrb->R_factor = *dp++;
 xrb->ext_R_factor = *dp++;
 xrb->MOS_LQ = *dp++;
 xrb->MOS_CQ = *dp++;
 xrb->RX_config = *dp++;
 dp = get16u(&xrb->JB_nominal, dp + 1);	// skipping reserved field
 dp = get16u(&xrb->JB_maximum, dp);
 dp = get16u(&xrb->JB_abs_max, dp);
 xr->curblock = xr->curblockend;
 return 0;
}

// Aculab SUMMARY reports

int rtcp_analyse_summary(RTCP_SUMMARY *rtcp_out, unsigned char *rtcp)
{
 uint_fast32_t length;
 uint_fast32_t utype;
 rtcp_out->start = rtcp;
 get32l(&length, rtcp);
 rtcp_out->end = rtcp + length;
 get32l(&utype, rtcp + 4);
 rtcp_out->type = (int) utype;
 return 0;
}

int rtcp_analyse_summary_tx(RTCP_SUMMARY_TX *rtcp_out, RTCP_SUMMARY *rtcp)
{
 unsigned char *dp = rtcp->start + 8;
 unsigned char *last_word = rtcp->end - 4;
 memset(rtcp_out, 0, sizeof(*rtcp_out));
 if (dp <= last_word) dp = get32l(&rtcp_out->ssrc, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->packetsSent, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->octetsSent, dp);
 return 0;
}

int rtcp_analyse_summary_rx(RTCP_SUMMARY_RX *rtcp_out, RTCP_SUMMARY *rtcp)
{
 unsigned char *dp = rtcp->start + 8;
 unsigned char *last_word = rtcp->end - 4;
 memset(rtcp_out, 0, sizeof(*rtcp_out));
 if (dp <= last_word) dp = get32l(&rtcp_out->ssrc, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->jitter, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->expected, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->seen, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->discarded, dp);
 return 0;
}

int rtcp_analyse_summary_augmented_rx(RTCP_SUMMARY_AUGMENTED_RX *rtcp_out, RTCP_SUMMARY *rtcp)
{
 unsigned char *dp = rtcp->start + 8;
 unsigned char *last_word = rtcp->end - 4;
 memset(rtcp_out, 0, sizeof(*rtcp_out));
 if (dp <= last_word) dp = get32l(&rtcp_out->ssrc, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->jitter, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->expected, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->seen, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->discarded, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->lastResyncTS, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->lastPktInTS, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->lastPktInVMPTS, dp);
 if (dp <= last_word) dp = get32l(&rtcp_out->lastPktInMSToEpoch, dp);
 return 0;
}

