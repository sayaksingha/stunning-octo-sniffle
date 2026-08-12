#include "rtcp_show.h"
#include "rtcp_accumulate.h"
#include "rtcp_parser.h"
#include <inttypes.h>
#include <stdio.h>

static char *sdes_name(unsigned type)
{
 switch (type) {
 case 1: return " (CNAME)";
 case 2: return " (NAME)";
 case 3: return " (EMAIL)";
 case 4: return " (PHONE)";
 case 5: return " (LOC)";
 case 6: return " (TOOL)";
 case 7: return " (NOTE)";
 case 8: return " (PRIV)";
 }
 return "";
}

static void showtext(FILE *outf, char *prefix, unsigned char *s, unsigned len)
{
 fprintf(outf, "%s \"", prefix);
 for (; len; len--) {
	int c = *s++;
	if (c < ' ' || c >= 127) {
		fprintf(outf, "\\%03o", c);
	} else if (c == '"' || c == '\\') {
		fprintf(outf, "\\%c", c);
	} else putc(c, outf);
 }
 fprintf(outf, "\"\n");
}

static int show_xr_block(FILE *outf, RTCP_XR *xr)
{
 switch (xr->blocktype) {
 case 1: {
 	RTCP_XR_BT_1 xrb;
 	unsigned base;
 	unsigned u;
 	fprintf(outf, "\tLoss RLE report:\n");
 	rtcp_analyse_xr_bt_1(&xrb, xr);
 	fprintf(outf, "\t\tthinning: 0x%x\n", xrb.t);
 	fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xrb.ssrc);
 	fprintf(outf, "\t\tbegin_seq: 0x%"PRIxFAST32"\n", xrb.begin_seq);
 	fprintf(outf, "\t\tend_seq: 0x%"PRIxFAST32"\n", xrb.end_seq);
		
 	base = xrb.begin_seq;
 	for (u=0; u < xrb.numchunks; u++) {
 		RTCP_XR_BT_1_CHUNK xrc;
 		rtcp_analyse_xr_bt_1_chunk(&xrc, &xrb, u);
 		if (xrc.chunk & 0x8000) {
			int j;
			fprintf(outf, "\t\tVector chunk: %u: ", base);
			for (j=0x4000; j; j >>= 1) {
				putc(xrc.chunk & j ? '1' : '0', outf);
			}
			base += 15 * xrb.t;
		} else if (xrc.chunk) {
			unsigned b = base;
			base += (xrc.chunk & 0x3fff) * xrb.t;
			fprintf(outf, "\t\tRun chunk: %u .. %u of %c\n", b,
				base, xrc.chunk & 0x4000 ? '1' : '0');
		} else fprintf(outf, "\t\tNull chunk\n");
		base &= 0xffff;
	}
 }
 break;
 case 2: {
 	RTCP_XR_BT_2 xrb;
 	unsigned base;
 	unsigned u;
 	fprintf(outf, "\tDuplicate RLE report:\n");
 	rtcp_analyse_xr_bt_2(&xrb, xr);
 	fprintf(outf, "\t\tthinning: 0x%x\n", xrb.t);
 	fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xrb.ssrc);
 	fprintf(outf, "\t\tbegin_seq: 0x%"PRIxFAST32"\n", xrb.begin_seq);
 	fprintf(outf, "\t\tend_seq: 0x%"PRIxFAST32"\n", xrb.end_seq);
 	base = xrb.begin_seq;
 	for (u=0; u < xrb.numchunks; u++) {
 		RTCP_XR_BT_2_CHUNK xrc;
 		rtcp_analyse_xr_bt_2_chunk(&xrc, &xrb, u);
 		if (xrc.chunk & 0x8000) {
			int j;
			fprintf(outf, "\t\tVector chunk: %u: ", base);
			for (j=0x4000; j; j >>= 1) {
				putc(xrc.chunk & j ? '1' : '0', outf);
			}
			base += 15 * xrb.t;
		} else if (xrc.chunk) {
			unsigned b = base;
			base += (xrc.chunk & 0x3fff) * xrb.t;
			fprintf(outf, "\t\tRun chunk: %u .. %u of %c\n", b,
				base, xrc.chunk & 0x4000 ? '1' : '0');
		} else fprintf(outf, "\t\tNull chunk\n");
		base &= 0xffff;
	}
 }
 	break;
 case 3: {
 	RTCP_XR_BT_3 xrb;
 	unsigned base;
 	unsigned u;
 	fprintf(outf, "\tPacket receipt times report:\n");
 	rtcp_analyse_xr_bt_3(&xrb, xr);
 	fprintf(outf, "\t\tthinning: 0x%x\n", xrb.t);
 	fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xrb.ssrc);
 	fprintf(outf, "\t\tbegin_seq: 0x%"PRIxFAST32"\n", xrb.begin_seq);
 	fprintf(outf, "\t\tend_seq: 0x%"PRIxFAST32"\n", xrb.end_seq);
 	base = xrb.begin_seq;
 	for (u=0; u < xrb.numtimes; u++) {
 		RTCP_XR_BT_3_TIME xrc;
 		rtcp_analyse_xr_bt_3_time(&xrc, &xrb, u);
 		fprintf(outf, "\t\tTime for %u: %"PRIuFAST32"\n", base, xrc.time);
		base += xrb.t;
		base &= 0xffff;
	}
 }
 	break;
 case 4: {
 	RTCP_XR_BT_4 xrb;
 	fprintf(outf, "\tReceiver reference time report:\n");
 	rtcp_analyse_xr_bt_4(&xrb, xr);
 	fprintf(outf, "\t\tNTP timestamp: %" PRIuFAST64 "\n", xrb.ntp_timestamp);
 }
 	break;
 case 5: {
 	RTCP_XR_BT_5 xrb;
 	unsigned u;
 	fprintf(outf, "\tDLRR report:\n");
 	rtcp_analyse_xr_bt_5(&xrb, xr);
 	for (u=0; u < xrb.subblocks; u++) {
 		RTCP_XR_BT_5_SUBBLOCK xsb;
 		rtcp_analyse_xr_bt_5_subblock(&xsb, xr);
 		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xsb.ssrc);
 		fprintf(outf, "\t\tlast RR: 0x%"PRIxFAST32"\n", xsb.lrr);
 		fprintf(outf, "\t\tdelay since last RR: 0x%"PRIxFAST32"\n", xsb.dlrr);
	}
 }
 	break;
 case 6: {
 	RTCP_XR_BT_6 xrb;
 	fprintf(outf, "\tStatistics summary report:\n");
 	rtcp_analyse_xr_bt_6(&xrb, xr);
 	fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xrb.ssrc);
 	fprintf(outf, "\t\tbegin_seq: %"PRIuFAST32"\n", xrb.begin_seq);
 	fprintf(outf, "\t\tend_seq: %"PRIuFAST32"\n", xrb.end_seq);
 	if (xrb.l) fprintf(outf, "\t\tlost_packets: %"PRIuFAST32"\n", xrb.lost_packets);
 	if (xrb.d) fprintf(outf, "\t\tdup_packets: %"PRIuFAST32"\n", xrb.dup_packets);
 	if (xrb.j) {
		fprintf(outf, "\t\tmin_jitter: %"PRIuFAST32"\n", xrb.min_jitter);
		fprintf(outf, "\t\tmax_jitter: %"PRIuFAST32"\n", xrb.max_jitter);
		fprintf(outf, "\t\tmean_jitter: %"PRIuFAST32"\n", xrb.mean_jitter);
		fprintf(outf, "\t\tdev_jitter: %"PRIuFAST32"\n", xrb.dev_jitter);
	}
	switch (xrb.ToH) {
	case 0: break;	// nothing
	case 1:
		fprintf(outf, "\t\tmin_ttl: %d\n", xrb.min_ttl_or_hl);
		fprintf(outf, "\t\tmax_ttl: %d\n", xrb.max_ttl_or_hl);
		fprintf(outf, "\t\tmean_ttl: %d\n", xrb.mean_ttl_or_hl);
		fprintf(outf, "\t\tdev_ttl: %d\n", xrb.dev_ttl_or_hl);
		break;
	case 2:
		fprintf(outf, "\t\tmin_hl: %d\n", xrb.min_ttl_or_hl);
		fprintf(outf, "\t\tmax_hl: %d\n", xrb.max_ttl_or_hl);
		fprintf(outf, "\t\tmean_hl: %d\n", xrb.mean_ttl_or_hl);
		fprintf(outf, "\t\tdev_hl: %d\n", xrb.dev_ttl_or_hl);
		break;
	case 3: fprintf(outf, "\t\t*** ToH == 3\n");	// bogus
	}
 }
 	break;
 case 7: {
 	RTCP_XR_BT_7 xrb;
 	fprintf(outf, "\tVoIP metrics report:\n");
 	rtcp_analyse_xr_bt_7(&xrb, xr);
 	fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", xrb.ssrc);
	fprintf(outf, "\t\tloss rate: %f\n", xrb.loss_rate / 256.0);
	fprintf(outf, "\t\tdiscard rate: %f\n", xrb.discard_rate / 256.0);
	fprintf(outf, "\t\tburst density: %f\n", xrb.burst_density / 256.0);
	fprintf(outf, "\t\tgap density: %f\n", xrb.gap_density / 256.0);
	fprintf(outf, "\t\tburst duration: %d mS\n", xrb.burst_duration);
	fprintf(outf, "\t\tgap duration: %d mS\n", xrb.gap_duration);
	fprintf(outf, "\t\tround trip delay: %d\n", xrb.round_trip_delay);
	fprintf(outf, "\t\tend system delay: %d\n", xrb.end_system_delay);
	if (xrb.signal_level != 127) fprintf(outf, "\t\tsignal level: %d\n", xrb.signal_level);
	if (xrb.noise_level != 127) fprintf(outf, "\t\tnoise level: %d\n", xrb.noise_level);
	if (xrb.RERL != 127) fprintf(outf, "\t\tRERL: %d\n", xrb.RERL);
	fprintf(outf, "\t\tGmin: %d\n", xrb.Gmin);
	if (xrb.R_factor != 127) fprintf(outf, "\t\tR factor: %d\n", xrb.R_factor);
	if (xrb.ext_R_factor != 127) fprintf(outf, "\t\text. R_factor: %d\n", xrb.ext_R_factor);
	if (xrb.MOS_LQ != 127) fprintf(outf, "\t\tMOS LQ: %d\n", xrb.MOS_LQ);
	if (xrb.MOS_CQ != 127) fprintf(outf, "\t\tMOS CQ: %d\n", xrb.MOS_CQ);
	fprintf(outf, "\t\tRX config:\n");
	switch (xrb.RX_config & 0xc0) {
	case 0: fprintf(outf, "\t\t\tPLC: unspecified:\n"); break;
	case 0x40: fprintf(outf, "\t\t\tPLC: disabled:\n"); break;
	case 0x80: fprintf(outf, "\t\t\tPLC: enhanced:\n"); break;
	case 0xc0: fprintf(outf, "\t\t\tPLC: standard:\n"); break;
	}
	switch (xrb.RX_config & 0x30) {
	case 0: fprintf(outf, "\t\t\tJBA: unknown:\n"); break;
	case 0x10: fprintf(outf, "\t\t\tJBA: reserved:\n"); break;
	case 0x20: fprintf(outf, "\t\t\tJBA: non-adaptive:\n"); break;
	case 0x30: fprintf(outf, "\t\t\tJBA: adaptive:\n"); break;
	}
	fprintf(outf, "\t\t\tJB rate: %d:\n", xrb.RX_config & 0xf);
	fprintf(outf, "\t\tJB nominal: %d\n", xrb.JB_nominal);
	fprintf(outf, "\t\tJB maximum: %d\n", xrb.JB_maximum);
	fprintf(outf, "\t\tJB abs max: %d\n", xrb.JB_abs_max);
 }
 	break;
 default:
 	fprintf(outf, "\tUnknown block type: %d\n", xr->blocktype);
 }
 return 0;
}

unsigned show_rtcp(FILE *outf, unsigned char *dp, unsigned length)
{
 RTCP rtcp;
 rtcp_analyse(&rtcp, dp);
 if (rtcp.length > length) {
	fprintf(outf, "Malformed packet: RTCP length (%d) exceeds packet length (%d)\n", rtcp.length, length);
	return 0;
 }
 fprintf(outf, "# RTCP: v=%d, p=%d, count=%d, pt=%d, length=%d\n",
	rtcp.v, rtcp.p, rtcp.count, rtcp.pt, rtcp.length);
 if (rtcp.v != 2) {
	fprintf(outf, "RTCP version is %d, expected 2\n", rtcp.v);
	return 0;
 }
 switch (rtcp.pt) {
 case 200: {	// SR
	unsigned char *pse;
	unsigned pselen;
	RTCP_SR rtcp_sr;
	unsigned u;
	rtcp_analyse_sr(&rtcp_sr, &rtcp);
	fprintf(outf, "SR:\n");
	fprintf(outf, "\tSSRC: 0x%"PRIxFAST32"\n", rtcp_sr.ssrc);
	fprintf(outf, "\tNTP timestamp: %" PRIuFAST64 "\n", rtcp_sr.ntp_timestamp);
	fprintf(outf, "\tRTP timestamp: %"PRIuFAST32"\n", rtcp_sr.rtp_timestamp);
	fprintf(outf, "\tsenders packet count: %"PRIdFAST32"\n", rtcp_sr.senders_packet_count);
	fprintf(outf, "\tsenders octet count: %"PRIdFAST32"\n", rtcp_sr.senders_octet_count);
	for (u=0; u < rtcp.count; u++) {
		RTCP_SR_REPORT_BLOCK rb;
		rtcp_analyse_sr_report_block(&rb, u, &rtcp);
		fprintf(outf, "\tReport %u\n", u);
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", rb.ssrc);
		fprintf(outf, "\t\tfraction lost: %g\n", rb.fraction_lost / 256.0);
		fprintf(outf, "\t\tpackets lost: %"PRIuFAST32"\n", rb.packets_lost);
		fprintf(outf, "\t\textended highest sequence number received: %"PRIuFAST32"\n", rb.ehsnr);
		fprintf(outf, "\t\tinterarrival jitter: %"PRIuFAST32" TSU\n", rb.jitter);
		fprintf(outf, "\t\tlast SR timestamp (as NTP timestamp): %" PRIuFAST64 "\n", (uint_fast64_t) rb.lsr << 16);
		fprintf(outf, "\t\tdelay since last SR: %g seconds\n", rb.dlsr / 65536.0);
	}
	rtcp_get_sr_pse(&pse, &pselen, &rtcp);
	if (pse) showtext(outf, "\tPSE:", pse, pselen);
 }
	break;
 case 201: {	// RR
	unsigned char *pse;
	unsigned pselen;
	RTCP_RR rtcp_rr;
	unsigned u;
	rtcp_analyse_rr(&rtcp_rr, &rtcp);
	fprintf(outf, "RR:\n");
	fprintf(outf, "\tSSRC: 0x%"PRIxFAST32"\n", rtcp_rr.ssrc);
	for (u=0; u < rtcp.count; u++) {
		RTCP_RR_REPORT_BLOCK rb;
		rtcp_analyse_rr_report_block(&rb, u, &rtcp);
		fprintf(outf, "\tReport %u\n", u);
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", rb.ssrc);
		fprintf(outf, "\t\tfraction lost: %g\n", rb.fraction_lost / 256.0);
		fprintf(outf, "\t\tpackets lost: %"PRIuFAST32"\n", rb.packets_lost);
		fprintf(outf, "\t\textended highest sequence number received: %"PRIuFAST32"\n", rb.ehsnr);
		fprintf(outf, "\t\tinterarrival jitter: %"PRIuFAST32" TSU\n", rb.jitter);
		fprintf(outf, "\t\tlast SR timestamp (as NTP timestamp): %" PRIuFAST64 "\n", (uint_fast64_t) rb.lsr << 16);
		fprintf(outf, "\t\tdelay since last SR: %g seconds\n", rb.dlsr / 65536.0);
	}
	rtcp_get_rr_pse(&pse, &pselen, &rtcp);
	if (pse) showtext(outf, "\tPSE:", pse, pselen);
 }
	break;
 case 202: {	// SDES
	RTCP_SDES_CHUNK rsc;
	unsigned u;
	rtcp_analyse_sdes(&rsc, &rtcp);
	fprintf(outf, "SDES:\n");
	for (u=0; u < rtcp.count; u++) {
		fprintf(outf, "\tChunk %u:\n", u);
		rtcp_analyse_sdes_next_chunk(&rsc, &rtcp);
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", rsc.ssrc_csrc);
		for (;;) {
			if (!rsc.item_type) break;
			fprintf(outf, "\t\t\tItem %u%s", rsc.item_type, sdes_name(rsc.item_type));
			showtext(outf, ":", rsc.item_data, rsc.item_length);
			rtcp_analyse_sdes_next_item(&rsc);
		}
	}
 }
	break;
 case 203: {	// BYE
	RTCP_BYE bye;
	unsigned u;
	rtcp_analyse_bye(&bye, &rtcp);
	fprintf(outf, "BYE:\n");
	for (u=0; u < rtcp.count; u++) {
		RTCP_BYE_SSRC_CSRC rbsc;
		rtcp_analyse_bye_ssrc_csrc(&rbsc, u, &rtcp);
		fprintf(outf, "\tSSRC/CSRC: 0x%"PRIxFAST32"\n", rbsc.ssrc_csrc);
	}
	if (bye.reason_text) showtext(outf, "\tReason:", bye.reason_text, bye.reason_length);
 }
	break;
 case 204: {	// APP
	RTCP_APP app;
	rtcp_analyse_app(&app, &rtcp);
	fprintf(outf, "APP:\n");
	fprintf(outf, "\tSSRC: 0x%"PRIxFAST32"\n", app.ssrc_csrc);
	showtext(outf, "\tName:", (unsigned char*)app.name, 4);
	showtext(outf, "\tData:", app.data, app.data_length);
 }
	break;
 case 207: {	// XR
 	RTCP_XR xr;
 	rtcp_analyse_xr(&xr, &rtcp);
 	fprintf(outf, "XR:\n");
 	for (;;) {
 		rtcp_analyse_xr_next_block(&xr);
 		if (xr.blocktype == ~0u) break;
 		show_xr_block(outf, &xr);
	}
 }
 	break;
 default:
	fprintf(outf, "Unknown RTCP PT (%d)\n", rtcp.pt);
	break;
 }
 return rtcp.length;
}

void show_rtcp_summary(FILE *outf, unsigned char *dp, unsigned length)
{
 for (; length;) {
	RTCP_SUMMARY summary;
	unsigned slen;
	rtcp_analyse_summary(&summary, dp);
	slen = (unsigned)(summary.end - summary.start);
	if (length < slen) {
		fprintf(outf, "Malformed packet: summary length (%d) > data length (%d)\n", slen, length);
		return;
	}
	switch (summary.type) {
	case 1:	{	// tx summary report
		RTCP_SUMMARY_TX stx;
		rtcp_analyse_summary_tx(&stx, &summary);
		fprintf(outf, "\tTx Summary:\n");
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", stx.ssrc);
		fprintf(outf, "\t\tPackets sent: %"PRIdFAST32"\n", stx.packetsSent);
		fprintf(outf, "\t\tOctets sent: %"PRIdFAST32"\n", stx.octetsSent);
	}
		break;
	case 2: {	// rx summary report
		RTCP_SUMMARY_RX srx;
		rtcp_analyse_summary_rx(&srx, &summary);
		fprintf(outf, "\tRx Summary:\n");
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", srx.ssrc);
		fprintf(outf, "\t\tJitter: 0x%"PRIxFAST32"\n", srx.jitter);
		fprintf(outf, "\t\tPackets expected: %"PRIdFAST32"\n", srx.expected);
		fprintf(outf, "\t\tPackets seen: %"PRIdFAST32"\n", srx.seen);
		fprintf(outf, "\t\tPackets discarded: %"PRIdFAST32"\n", srx.discarded);
	}
		break;
	case 3: {	// rx augmented summary report
		RTCP_SUMMARY_AUGMENTED_RX sarx;
		rtcp_analyse_summary_augmented_rx(&sarx, &summary);
		fprintf(outf, "\tRx Summary:\n");
		fprintf(outf, "\t\tSSRC: 0x%"PRIxFAST32"\n", sarx.ssrc);
		fprintf(outf, "\t\tJitter: 0x%"PRIxFAST32"\n", sarx.jitter);
		fprintf(outf, "\t\tPackets expected: %"PRIdFAST32"\n", sarx.expected);
		fprintf(outf, "\t\tPackets seen: %"PRIdFAST32"\n", sarx.seen);
		fprintf(outf, "\t\tPackets discarded: %"PRIdFAST32"\n", sarx.discarded);
		fprintf(outf, "\t\tVMPRx last resync timestamp: 0x%"PRIxFAST32"\n", sarx.lastResyncTS);
		fprintf(outf, "\t\tPacket timestamp last packet: 0x%"PRIxFAST32"\n", sarx.lastPktInTS);
		fprintf(outf, "\t\tVMPRx timestamp last packet: 0x%"PRIxFAST32"\n", sarx.lastPktInVMPTS);
		fprintf(outf, "\t\tTo epoch delay last packet: %"PRIdFAST32"\n", sarx.lastPktInMSToEpoch);
	}
		break;
	default:
		fprintf(outf, "Unknown Aculab summary type (%d)\n", summary.type);
		break;
	}
	dp += slen;
	length -= slen;
 }
}

static int show_ip_header(FILE *outf, unsigned char *data, unsigned len)
{
 const unsigned ihl = data[0] & 0xf;
 //const unsigned version = data[0] >> 4;
 const unsigned tos = data[1];
 const unsigned tlen = (data[2] << 8) + data[3];
 const unsigned id = (data[4] << 8) + data[5];
 const unsigned frag = (data[6] << 8) + data[7];
 const int ttl = data[8];
 unsigned protocol = data[9];
 const unsigned char *prdata = data + ihl * 4;
 if (len > tlen) {
	fprintf(outf, "! Warning: data accumulated (%u) exceeds packet length (%u)\n",
		len, tlen);
 } else if (len < tlen) {
	fprintf(outf, "! Warning: packet length (%u) exceeds data accumulated (%u)\n",
		tlen, len);
 }
 if (frag & ~0xc000) {
	fprintf(outf, "\tPacket is a fragment (ofs=%d%s)\n",
		frag & 0x3fff, frag & 0x4000 ? ", MF" : "");
	return 0;
 }
 if (frag & 0x8000) {
	fprintf(outf, "\tPacket has reserved fragment bit set: contents may be misinterpreted\n");
}
 if (protocol != 17) {
	if (!protocol) {
		fprintf(outf, "Foolishly assuming protocol is UDP\n");
		protocol = 17;
	}
 }
 if (protocol == 17) {
	const unsigned srcp = (prdata[0] << 8) + prdata[1];
	const unsigned dstp = (prdata[2] << 8) + prdata[3];
	fprintf(outf, "\t%d.%d.%d.%d:%u -> %d.%d.%d.%d:%u tos=0x%x ttl=%d id=%x\n",
		data[12], data[13], data[14], data[15], srcp,
		data[16], data[17], data[18], data[19], dstp,
		tos, ttl, id);
	return ihl * 4 + 8;
 }
 fprintf(outf, "Protocol is %u - not 17 (UDP)\n", protocol);
 return 0;
}

static void format_ipv6(char *buf, const unsigned char *a)
{
    const unsigned char *a_lim = a + 16;
    static const char seps[] = "::\0\0:0:";
    int sep = 7;

    for (; a < a_lim; a += 2) {
		unsigned short val = (a[0] << 8) + a[1];
        if (val == 0 && sep != 1) {
            if (sep >= 6)
                /* Appends "0:" to separator */
                sep -= 2;
            else
                /* Separator was "0:" or ":0:" - change to "::" */
                sep = 0;
            continue;
        }
        buf += sprintf(buf, "%s%x", seps + sep, val);
        /* Set separator to ":", 1 suppresses later "::" */
        sep = sep <= 1 ? 1 : 6;
    }

    /* Finally add terminating ":0" or "::" */
    if ((sep & 3) == 0)
        buf += sprintf(buf, "%.2s", seps + sep);
}

static int show_ipv6_headers(FILE *outf, unsigned char *data, unsigned len)
{
 const unsigned tc = ((data[0] & 0xf) << 4) + ((data[1] & 0xf0) >> 4);
 const unsigned fl = ((data[1] & 0xf) << 16) + (data[2] << 8) + data[3];
 const unsigned plen = (data[4] << 8) + data[5];
 unsigned hl = 40; // fixed IPv6 header size
 const unsigned tlen = plen + hl;
 unsigned protocol = data[6];
 const int hoplim = data[7];
 unsigned char *nexthdr = data + hl;
 if (len > tlen) {
	fprintf(outf, "! Warning: data accumulated (%u) exceeds packet length (%u)\n",
		len, tlen);
 } else if (len < tlen) {
	fprintf(outf, "! Warning: packet length (%u) exceeds data accumulated (%u)\n",
		tlen, len);
 }
 while (protocol != 17) {
	 char *ht;
	 int stop = 0;
	 switch (protocol) {
	 case 0:
		 ht = "Hop-by-hop";
		 break;
	 case 43:
		 ht = "Routing";
		 break;
	 case 44:
		 ht = "Fragment";
		 break;
	 case 50:
		 ht = "ESP";
		 stop = 1;
		 break;
	 case 51:
		 ht = "AH";
		 break;
	 case 59:
		 ht = "NoNextHdr";
		 stop = 1;
		 break;
	 case 60:
		 ht = "Dest opts";
		 break;
	 default:
		 ht = "Unknown";
		 stop = 1;
		 break;
	 }
	 hl = nexthdr[1] * 8 + 8;
	 fprintf(outf, "Extension header type: %s(%u), length %u\n", ht, protocol, hl);
	 if (stop) {
		 fprintf(outf, "Can continue after that header type\n");
		 return 0;
	 }
	 protocol = nexthdr[0];
	 nexthdr += hl;
 }
 if (protocol == 17) {
	const unsigned srcp = (nexthdr[0] << 8) + nexthdr[1];
	const unsigned dstp = (nexthdr[2] << 8) + nexthdr[3];
	char srcaddr[40];
	char dstaddr[40];
	format_ipv6(srcaddr, &data[8]);
	format_ipv6(dstaddr, &data[24]);
	fprintf(outf, "\t[%s]:%u -> [%s]:%u tc=0x%x fl=0x%x hoplimit=%u\n",
		srcaddr, srcp, dstaddr, dstp, tc, fl, hoplim);
	return ((int)(nexthdr - data)) + 8; // plus 8 for UDP header
 }
 fprintf(outf, "Protocol is %u - not 17 (UDP)\n", protocol);
 return 0;
}

void show_prosody_rtcp(FILE *outf, RTCP_BUFFER *rb)
{
 unsigned char *data = rtcp_buffer_data(rb);
 int length = rtcp_buffer_length(rb) - 4;	// type takes 4 bytes
 int type = rtcp_buffer_type(rb);
 switch (type) {
	int skip;
 case 2:
	fprintf(outf, "Transmitted RTCP packet:\n");
	if ((*data & 0xf0) == 0x40) {	// has IPv4 header
		skip = show_ip_header(outf, data, length);
		if (!skip) {
			fprintf(outf, "Cannot display remainder of packet\n");
			return;
		}

	} else if ((*data & 0xf0) == 0x60) {	// has IPv6 header
		skip = show_ipv6_headers(outf, data, length);
		if (!skip) {
			fprintf(outf, "Cannot display remainder of packet\n");
			return;
		}

	} else {
		fprintf(outf, "Malformed packet: IP version (%d)\n", *data >> 4);
		return;
	}
	if (skip > length) {
		fprintf(outf, "Malformed packet: length (%d) < offset to RTCP data (%d)\n", length, skip);
		return;
	}
	data += skip;
	length -= skip;
	do {
		skip = show_rtcp(outf, data, length);
		fprintf(outf, "\n");
		if (!skip) break;
		data += skip;
		length -= skip;
	} while (length >= 4);
	break;
 case 3:
 case 5:
	fprintf(outf, "Received RTCP packet:\n");
	if (type == 5) {
		skip = 8; // skip over NTP timestamp
		if (skip > length) {
			fprintf(outf, "Malformed packet: length (%d) < offset to IP data (%d)\n", length, skip);
			return;
		}
		data += skip;
		length -= skip;
	}
	if ((*data & 0xf0) == 0x40) {	// has IPv4 header
		skip = show_ip_header(outf, data, length);
		if (!skip) {
			fprintf(outf, "Cannot display remainder of packet\n");
			return;
		}
	} else if ((*data & 0xf0) == 0x60) {	// has IPv6 header
		skip = show_ipv6_headers(outf, data, length);
		if (!skip) {
			fprintf(outf, "Cannot display remainder of packet\n");
			return;
		}

	} else {
		fprintf(outf, "Malformed packet: IP version (%d)\n", *data >> 4);
		return;
	}
	if (skip > length) {
		fprintf(outf, "Malformed packet: length (%d) < offset to RTCP data (%d)\n", length, skip);
		return;
	}
	data += skip;
	length -= skip;
	do {
		skip = show_rtcp(outf, data, length);
		fprintf(outf, "\n");
		if (!skip) break;
		data += skip;
		length -= skip;
	} while (length >= 4);
	break;
 case 4:
	fprintf(outf, "Received summary report:\n");
	show_rtcp_summary(outf, data, length);
	break;
 default:
	fprintf(outf, "Unknown RTCP handler data, type=%d\n", type);
	break;
 }
}
