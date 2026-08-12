/* prosgen.i - Prosody generic trace code */
/* Automatically generated (from m1504.) - DO NOT EDIT! */

ACUAPI int ACUTiNG_WINAPI sm_channel_alloc_placed(struct sm_channel_alloc_placed_parms *channelp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_alloc_placed(type=%d, module=" TiNG_PRINTF_MODULE ", caps_mask=%d)", timestamp_now(), channelp->type, channelp->module, channelp->caps_mask);
 sts = real_sm_channel_alloc_placed(channelp);
 if (TiNGtrace > 1) olog(" == %d channel=%p\n", sts, channelp->channel);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_collector_connect(struct sm_channel_collector_connect_parms *dp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_collector_connect(source=%p, channel=%p)", timestamp_now(), dp->source, dp->channel);
 sts = real_sm_channel_collector_connect(dp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_datafeed_connect(struct sm_channel_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_datafeed_connect(data_source=%p, channel=%p)", timestamp_now(), datafeedp->data_source, datafeedp->channel);
 sts = real_sm_channel_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_dispatcher_connect(struct sm_channel_dispatcher_connect_parms *dp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_dispatcher_connect(dest=%p, channel=%p)", timestamp_now(), dp->dest, dp->channel);
 sts = real_sm_channel_dispatcher_connect(dp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_get_datafeed(struct sm_channel_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_get_datafeed(channel=%p)", timestamp_now(), datafeedp->channel);
 sts = real_sm_channel_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_info(struct sm_channel_info_parms *infop)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_info(channel=%p)", timestamp_now(), infop->channel);
 sts = real_sm_channel_info(infop);
 if (TiNGtrace > 1) olog(" == %d card=%d, ost=%d, ots=%d, otype=%d, ist=%d, its=%d, itype=%d, group=%d, caps_mask=%d\n", sts, infop->card, infop->ost, infop->ots, infop->otype, infop->ist, infop->its, infop->itype, infop->group, infop->caps_mask);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_input_data_redirect(struct sm_channel_input_data_redirect_parms *redp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_input_data_redirect(channel=%p, enable=%d)", timestamp_now(), redp->channel, redp->enable);
 sts = real_sm_channel_input_data_redirect(redp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, redp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_output_source_datafeed_connect(struct sm_channel_output_source_datafeed_connect_parms *outsrcp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_output_source_datafeed_connect(channel=%p, data_source=%p, threshold=%d, timeout=%d)", timestamp_now(), outsrcp->channel, outsrcp->data_source, outsrcp->threshold, outsrcp->timeout);
 sts = real_sm_channel_output_source_datafeed_connect(outsrcp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_release(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_release(channel=%p)", timestamp_now(), channel);
 sts = real_sm_channel_release(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_set_event(struct sm_channel_set_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_set_event(channel=%p, event_type=%d, issue_events=%d, event=" FORMAT_EVENT ")", timestamp_now(), eventp->channel, eventp->event_type, eventp->issue_events, ARGS_EVENT(eventp->event));
 sts = real_sm_channel_set_event(eventp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_channel_set_output_rate(struct sm_channel_set_output_rate_parms *ratep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_channel_set_output_rate(channel=%p, sample_rate=%d)", timestamp_now(), ratep->channel, ratep->sample_rate);
 sts = real_sm_channel_set_output_rate(ratep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_close_module(struct sm_close_module_parms *closep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_close_module(module_id=" TiNG_PRINTF_MODULE ")", timestamp_now(), closep->module_id);
 sts = real_sm_close_module(closep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_close_prosody(struct sm_close_prosody_parms *closep)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_close_prosody(card_id=%d)", timestamp_now(), closep->card_id);
 sts = real_sm_close_prosody(closep);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_collector_destroy(tSMCollectorId collector)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_collector_destroy(collector=%p)", timestamp_now(), collector);
 sts = real_sm_collector_destroy(collector);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_collector_get_event(struct sm_collector_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_collector_get_event(collector=%p)", timestamp_now(), eventp->collector);
 sts = real_sm_collector_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_collector_status(struct sm_collector_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_collector_status(collector=%p)", timestamp_now(), statusp->collector);
 sts = real_sm_collector_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d, address=" FORMAT_IN_ADDR ", port=%d, address=" FORMAT_IN_ADDR ", port=%d, address=" FORMAT_IN6_ADDR ", port=%d, address=" FORMAT_IN6_ADDR ", port=%d\n", sts, statusp->status, ARGS_IN_ADDR(statusp->u.ports.address), statusp->u.ports.port, ARGS_IN_ADDR(statusp->u.peer.address), statusp->u.peer.port, ARGS_IN6_ADDR(statusp->u.ports_ipv6.address), statusp->u.ports_ipv6.port, ARGS_IN6_ADDR(statusp->u.peer_ipv6.address), statusp->u.peer_ipv6.port);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_dispatcher_destroy(tSMDispatcherId dispatcher)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_dispatcher_destroy(dispatcher=%p)", timestamp_now(), dispatcher);
 sts = real_sm_dispatcher_destroy(dispatcher);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI char *ACUTiNG_WINAPI sm_error_name(struct sm_error_name_parms *errp)
{
 char * sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_error_name(code=%d)", timestamp_now(), errp->code);
 sts = real_sm_error_name(errp);
 if (TiNGtrace > 1) olog(" == %p name='%s'\n", sts, errp->name);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_card_info(struct sm_card_info_parms *cardinfop)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_card_info(card=%d)", timestamp_now(), cardinfop->card);
 sts = real_sm_get_card_info(cardinfop);
 if (TiNGtrace > 1) olog(" == %d card_type=%d, card_detected=%d, module_count=%d, physical_address=%u, io_address=%u, physical_irq=%u, serial_no='%s'\n", sts, cardinfop->card_type, cardinfop->card_detected, cardinfop->module_count, cardinfop->physical_address, cardinfop->io_address, cardinfop->physical_irq, cardinfop->serial_no);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_card_rev(struct sm_card_rev_parms *cardrevp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_card_rev(card=%d)", timestamp_now(), cardrevp->card);
 sts = real_sm_get_card_rev(cardrevp);
 if (TiNGtrace > 1) {
	olog(" == %d rev_additional='%s'", sts, cardrevp->rev_additional);
	show_rev_out(cardrevp);
 }
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_channel_type(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_channel_type(channel=%p)", timestamp_now(), channel);
 sts = real_sm_get_channel_type(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_driver_info(struct sm_driver_info_parms *drvinfop)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_driver_info()", timestamp_now());
 sts = real_sm_get_driver_info(drvinfop);
 if (TiNGtrace > 1) olog(" == %d major=%d, minor=%d, step=%d, custom=%d, quality=%d, buildno0=%d, buildno1=%d\n", sts, drvinfop->major, drvinfop->minor, drvinfop->step, drvinfop->custom, drvinfop->quality, drvinfop->buildno0, drvinfop->buildno1);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_module_info(struct sm_module_info_parms *moduleinfop)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_module_info(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), moduleinfop->module);
 sts = real_sm_get_module_info(moduleinfop);
 if (TiNGtrace > 1) olog(" == %d firmware_running=%d, firmware_id=%d, average_loading=%d, max_loading=%d, module_slot=%d, min_stream=%d, stream_count=%d\n", sts, moduleinfop->firmware_running, moduleinfop->firmware_id, moduleinfop->average_loading, moduleinfop->max_loading, moduleinfop->module_slot, moduleinfop->min_stream, moduleinfop->stream_count);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_stream_info(struct sm_stream_info_parms *streaminfop)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_stream_info(module=" TiNG_PRINTF_MODULE ", stream=%d)", timestamp_now(), streaminfop->module, streaminfop->stream);
 sts = real_sm_get_stream_info(streaminfop);
 if (TiNGtrace > 1) olog(" == %d serial_no='%s', serial_port_ix=%d, timeslot_count=%d\n", sts, streaminfop->serial_no, streaminfop->serial_port_ix, streaminfop->timeslot_count);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_module_get_event(struct sm_module_event_parms *eventp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_module_get_event(module_id=" TiNG_PRINTF_MODULE ")", timestamp_now(), eventp->module_id);
 sts = real_sm_module_get_event(eventp);
 if (TiNGtrace > 1) olog(" == %d event=" FORMAT_EVENT "\n", sts, ARGS_EVENT(eventp->event));
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_module_status(struct sm_module_status_parms *statusp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_module_status(module_id=" TiNG_PRINTF_MODULE ")", timestamp_now(), statusp->module_id);
 sts = real_sm_module_status(statusp);
 if (TiNGtrace > 1) olog(" == %d status=%d\n", sts, statusp->status);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_open_module(struct sm_open_module_parms *module_parmsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_open_module(card_id=%d, module_ix=%u)", timestamp_now(), module_parmsp->card_id, module_parmsp->module_ix);
 sts = real_sm_open_module(module_parmsp);
 if (TiNGtrace > 1) olog(" == %d module_id=" TiNG_PRINTF_MODULE "\n", sts, module_parmsp->module_id);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_sec_module(struct sm_sec_module_parms *module_parmsp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_sec_module(module_id=" TiNG_PRINTF_MODULE ", exclude_data_to_module=%d, exclude_data_from_module=%d, key_id=%d, context='%s', encdec=%p)", timestamp_now(), module_parmsp->module_id, module_parmsp->exclude_data_to_module, module_parmsp->exclude_data_from_module, module_parmsp->key_id, module_parmsp->context, module_parmsp->encdec);
 sts = real_sm_sec_module(module_parmsp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_shutdown_module(struct sm_shutdown_module_parms *shutdownp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_shutdown_module(module_id=" TiNG_PRINTF_MODULE ")", timestamp_now(), shutdownp->module_id);
 sts = real_sm_shutdown_module(shutdownp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_switch_channel_input(struct sm_switch_channel_parms *switchp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_switch_channel_input(channel=%p, st=%d, ts=%d, type=%d)", timestamp_now(), switchp->channel, switchp->st, switchp->ts, switchp->type);
 sts = real_sm_switch_channel_input(switchp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_switch_channel_output(struct sm_switch_channel_parms *switchp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_switch_channel_output(channel=%p, st=%d, ts=%d, type=%d)", timestamp_now(), switchp->channel, switchp->st, switchp->ts, switchp->type);
 sts = real_sm_switch_channel_output(switchp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_create(struct sm_tdmrx_create_parms *tdmrxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmrx_create(module=" TiNG_PRINTF_MODULE ", stream=%d, timeslot=%d, type=%d)", timestamp_now(), tdmrxp->module, tdmrxp->stream, tdmrxp->timeslot, tdmrxp->type);
 sts = real_sm_tdmrx_create(tdmrxp);
 if (TiNGtrace > 1) olog(" == %d tdmrx=%p\n", sts, tdmrxp->tdmrx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_destroy(tSMTDMrxId tdmrx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmrx_destroy(tdmrx=%p)", timestamp_now(), tdmrx);
 sts = real_sm_tdmrx_destroy(tdmrx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmrx_get_datafeed(struct sm_tdmrx_datafeed_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmrx_get_datafeed(tdmrx=%p)", timestamp_now(), datafeedp->tdmrx);
 sts = real_sm_tdmrx_get_datafeed(datafeedp);
 if (TiNGtrace > 1) olog(" == %d datafeed=%p\n", sts, datafeedp->datafeed);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_create(struct sm_tdmtx_create_parms *tdmtxp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmtx_create(module=" TiNG_PRINTF_MODULE ", stream=%d, timeslot=%d, type=%d)", timestamp_now(), tdmtxp->module, tdmtxp->stream, tdmtxp->timeslot, tdmtxp->type);
 sts = real_sm_tdmtx_create(tdmtxp);
 if (TiNGtrace > 1) olog(" == %d tdmtx=%p\n", sts, tdmtxp->tdmtx);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_datafeed_connect(struct sm_tdmtx_datafeed_connect_parms *datafeedp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmtx_datafeed_connect(data_source=%p, tdmtx=%p)", timestamp_now(), datafeedp->data_source, datafeedp->tdmtx);
 sts = real_sm_tdmtx_datafeed_connect(datafeedp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_tdmtx_destroy(tSMTDMtxId tdmtx)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_tdmtx_destroy(tdmtx=%p)", timestamp_now(), tdmtx);
 sts = real_sm_tdmtx_destroy(tdmtx);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_udp_collector_create(struct sm_udp_collector_create_parms *dp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_udp_collector_create(module=" TiNG_PRINTF_MODULE ", type=%d, address=" FORMAT_IN_ADDR ", ipv6_address=" FORMAT_IN6_ADDR ")", timestamp_now(), dp->module, dp->type, ARGS_IN_ADDR(dp->address), ARGS_IN6_ADDR(dp->ipv6_address));
 sts = real_sm_udp_collector_create(dp);
 if (TiNGtrace > 1) olog(" == %d collector=%p\n", sts, dp->collector);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_udp_dispatcher_create(struct sm_udp_dispatcher_create_parms *dp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_udp_dispatcher_create(module=" TiNG_PRINTF_MODULE ", type=%d, destination=" FORMAT_SOCKADDR_IN ", source=" FORMAT_SOCKADDR_IN ", TOS=%d, destination_ipv6=" FORMAT_SOCKADDR_IN6 ", source_ipv6=" FORMAT_SOCKADDR_IN6 ")", timestamp_now(), dp->module, dp->type, ARGS_SOCKADDR_IN(dp->destination), ARGS_SOCKADDR_IN(dp->source), dp->TOS, ARGS_SOCKADDR_IN6(dp->destination_ipv6), ARGS_SOCKADDR_IN6(dp->source_ipv6));
 sts = real_sm_udp_dispatcher_create(dp);
 if (TiNGtrace > 1) olog(" == %d dispatcher=%p\n", sts, dp->dispatcher);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smd_ev_create(tSMEventId *eventId, tSMChannelId channelId, int eventType, int eventChannelBinding)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smd_ev_create(eventId=%p, channelId=%p, eventType=%d, eventChannelBinding=%d)", timestamp_now(), eventId, channelId, eventType, eventChannelBinding);
 sts = real_smd_ev_create(eventId, channelId, eventType, eventChannelBinding);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smd_ev_free(tSMEventId eventId)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smd_ev_free(eventId=" FORMAT_EVENT ")", timestamp_now(), ARGS_EVENT(eventId));
 sts = real_smd_ev_free(eventId);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI smd_ev_wait(tSMEventId eventId)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "smd_ev_wait(eventId=" FORMAT_EVENT ")", timestamp_now(), ARGS_EVENT(eventId));
 sts = real_smd_ev_wait(eventId);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_config_module_switching(struct sm_config_module_sw_parms *configp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_config_module_switching(module=" TiNG_PRINTF_MODULE ", auto_assign_out_ts=%d, auto_assign_in_ts=%d, out_st0=%d, out_st1=%d, out_base_ts=%d, in_type=%d, in_st0=%d, in_st1=%d, in_base_ts=%d, out_type=%d)", timestamp_now(), configp->module, configp->auto_assign_out_ts, configp->auto_assign_in_ts, configp->out_st0, configp->out_st1, configp->out_base_ts, configp->in_type, configp->in_st0, configp->in_st1, configp->in_base_ts, configp->out_type);
 sts = real_sm_config_module_switching(configp);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_card_switch_id(tSMCardId card)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_card_switch_id(card=%d)", timestamp_now(), card);
 sts = real_sm_get_card_switch_id(card);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_channel_ix(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_channel_ix(channel=%p)", timestamp_now(), channel);
 sts = real_sm_get_channel_ix(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_channel_module_id(tSMChannelId channel)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_channel_module_id(channel=%p)", timestamp_now(), channel);
 sts = real_sm_get_channel_module_id(channel);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_firmware_info(struct sm_fwinfo_parms *fp)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_firmware_info(module=" TiNG_PRINTF_MODULE ", position=%u)", timestamp_now(), fp->module, fp->position);
 sts = real_sm_get_firmware_info(fp);
 if (TiNGtrace > 1) olog(" == %d position=%u, tasktypecode=%u, version=%u\n", sts, fp->position, fp->tasktypecode, fp->version);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_get_module_card_id(tSMModuleId mod_id)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_get_module_card_id(mod_id=" TiNG_PRINTF_MODULE ")", timestamp_now(), mod_id);
 sts = real_sm_get_module_card_id(mod_id);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}

ACUAPI int ACUTiNG_WINAPI sm_reset_module(tSMModuleId module)
{
 int  sts = run_once(&run_once_init);
 if (sts) return sts;
 checktrace();
 if (TiNGtrace > 1) olog(TIME_FMT "sm_reset_module(module=" TiNG_PRINTF_MODULE ")", timestamp_now(), module);
 sts = real_sm_reset_module(module);
 if (TiNGtrace > 1) olog(" == %d\n", sts);
 return sts;
}
