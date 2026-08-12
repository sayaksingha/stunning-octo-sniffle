/* prosdc.i - Prosody data communications trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI smdc_channel_config(struct smdc_channel_config_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_channel_config(channel=%p, protocol=%d, config_length=%d, encoding=%d, encoding_config_length=%d", timestamp_now(), configp->channel, configp->protocol, configp->config_length, configp->encoding, configp->encoding_config_length);
	show_config_enc_in(configp);
 }
 sts = real_smdc_channel_config(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_line_control(struct smdc_line_control_parms *controlp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_line_control(channel=%p, cmd=%d, aux_mask=%u, aux_toggle=%u, shorttrain=%u, tep=%d, prefix_bitlength=%u, suffix_bitlength=%u, retrain_speed=%u", timestamp_now(), controlp->channel, controlp->cmd, controlp->aux_mask, controlp->aux_toggle, controlp->shorttrain, controlp->tep, controlp->prefix_bitlength, controlp->suffix_bitlength, controlp->retrain_speed);
	show_prefdat_sufdat_in(controlp);
 }
 sts = real_smdc_line_control(controlp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_line_control_v34(struct smdc_line_control_v34_parms *controlp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_line_control_v34(channel=%p, cmd=%d, min_bitrate=%d, max_bitrate=%d)", timestamp_now(), controlp->channel, controlp->cmd, controlp->u.bitrate.min_bitrate, controlp->u.bitrate.max_bitrate);
 sts = real_smdc_line_control_v34(controlp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_line_status(struct smdc_line_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_line_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_smdc_line_status(statusp);
 if (TiNGtrace > 1) olog(" == %d link_status=%d, rx_status=%d, tx_status=%d, tx_cts=%d\n", sts, statusp->link_status, statusp->rx_status, statusp->tx_status, statusp->tx_cts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_line_status_v34(struct smdc_line_status_v34_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_line_status_v34(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_smdc_line_status_v34(statusp);
 if (TiNGtrace > 1) olog(" == %d link_status=%d, rx_status=%d, tx_status=%d, control_bitrate=%d, primary_bitrate=%d, snr_status=%d, fdx_rx_bitrate=%d, fdx_tx_bitrate=%d\n", sts, statusp->link_status, statusp->rx_status, statusp->tx_status, statusp->control_bitrate, statusp->primary_bitrate, statusp->snr_status, statusp->fdx_rx_bitrate, statusp->fdx_tx_bitrate);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_config_t38(struct smdc_rx_config_t38_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_config_t38(channel=%p, PreCorrigendum=%d, Protocol=%d)", timestamp_now(), configp->channel, configp->PreCorrigendum, configp->Protocol);
 sts = real_smdc_rx_config_t38(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_config_v34(struct smdc_rx_config_v34_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_config_v34(channel=%p, duplex=%d, min_bitrate=%d, max_bitrate=%d, v8fsk_metric=%d)", timestamp_now(), configp->channel, configp->duplex, configp->min_bitrate, configp->max_bitrate, configp->v8fsk_metric);
 sts = real_smdc_rx_config_v34(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_control(struct smdc_rx_control_parms *controlp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_control(channel=%p, cmd=%d, min_to_collect=%d, min_idle=%d, blocking=%d)", timestamp_now(), controlp->channel, controlp->cmd, controlp->min_to_collect, controlp->min_idle, controlp->blocking);
 sts = real_smdc_rx_control(controlp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_data(struct smdc_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_rx_data(channel=%p, max_length=%d", timestamp_now(), datap->channel, datap->max_length);
	show_rxtx_data_in(datap);
 }
 sts = real_smdc_rx_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_encoding(struct smdc_encoding_parms *encp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_rx_encoding(channel=%p, encoding=%d, encoding_config_length=%d", timestamp_now(), encp->channel, encp->encoding, encp->encoding_config_length);
	show_enc_in(encp);
 }
 sts = real_smdc_rx_encoding(encp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_status(struct smdc_rx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_smdc_rx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d available_octets=%d, status=%d, flow=%d, carrier=%d, metric=%d\n", sts, statusp->available_octets, statusp->status, statusp->flow, statusp->u.carrier.carrier, statusp->u.metric.metric);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_status_t38(struct smdc_rx_status_t38_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_status_t38(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_smdc_rx_status_t38(statusp);
 if (TiNGtrace > 1) olog(" == %d available_octets=%d, status=%d, flow=%d, tone_type=%d, v34_type=%d, carrier=%d, rate=%d, protocol=%d\n", sts, statusp->available_octets, statusp->status, statusp->flow, statusp->u.tone.tone_type, statusp->u.v34.v34_type, statusp->u.carrier.carrier, statusp->u.rate.rate, statusp->u.protocol.protocol);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_rx_stop(struct smdc_stop_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_rx_stop(channel=%p)", timestamp_now(), pp->channel);
 sts = real_smdc_rx_stop(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_stop(struct smdc_stop_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_stop(channel=%p)", timestamp_now(), pp->channel);
 sts = real_smdc_stop(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_config_t38(struct smdc_tx_config_t38_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_config_t38(channel=%p, PreCorrigendum=%d, Protocol=%d, max_packet_length=%u)", timestamp_now(), configp->channel, configp->PreCorrigendum, configp->Protocol, configp->max_packet_length);
 sts = real_smdc_tx_config_t38(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_config_v34(struct smdc_tx_config_v34_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_config_v34(channel=%p, mode=%d, polling=%d, duplex=%d, min_bitrate=%d, max_bitrate=%d)", timestamp_now(), configp->channel, configp->mode, configp->polling, configp->duplex, configp->min_bitrate, configp->max_bitrate);
 sts = real_smdc_tx_config_v34(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_control(struct smdc_tx_control_parms *controlp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_control(channel=%p, cmd=%d, capacity=%d, blocking=%d)", timestamp_now(), controlp->channel, controlp->cmd, controlp->capacity, controlp->blocking);
 sts = real_smdc_tx_control(controlp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_data(struct smdc_data_parms *datap)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_tx_data(channel=%p, max_length=%d", timestamp_now(), datap->channel, datap->max_length);
	show_rxtx_data_in(datap);
 }
 sts = real_smdc_tx_data(datap);
 if (TiNGtrace > 1) olog(" == %d done_length=%d\n", sts, datap->done_length);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_encoding(struct smdc_encoding_parms *encp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) {
	olog(TIME_FMT "smdc_tx_encoding(channel=%p, encoding=%d, encoding_config_length=%d", timestamp_now(), encp->channel, encp->encoding, encp->encoding_config_length);
	show_enc_in(encp);
 }
 sts = real_smdc_tx_encoding(encp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_status(struct smdc_tx_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_status(channel=%p)", timestamp_now(), statusp->channel);
 sts = real_smdc_tx_status(statusp);
 if (TiNGtrace > 1) olog(" == %d capacity=%d, status=%d, flow=%d\n", sts, statusp->capacity, statusp->status, statusp->flow);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_stop(struct smdc_stop_parms *pp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_stop(channel=%p)", timestamp_now(), pp->channel);
 sts = real_smdc_tx_stop(pp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_tone_indicator_t38(struct smdc_tx_tone_indicator_t38_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_tone_indicator_t38(channel=%p, tone_type=%d)", timestamp_now(), tonep->channel, tonep->tone_type);
 sts = real_smdc_tx_tone_indicator_t38(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_v34_indicator_t38(struct smdc_tx_v34_indicator_t38_parms *tonep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_v34_indicator_t38(channel=%p, v34_type=%d)", timestamp_now(), tonep->channel, tonep->v34_type);
 sts = real_smdc_tx_v34_indicator_t38(tonep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smdc_tx_v34_line_control_t38(struct smdc_tx_v34_line_control_t38_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smdc_tx_v34_line_control_t38(channel=%p, primary_bitrate=%u)", timestamp_now(), configp->channel, configp->primary_bitrate);
 sts = real_smdc_tx_v34_line_control_t38(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
