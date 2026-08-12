# Note that this makefile describes how to build various programs for
# various systems.

# The system is selected by the TiNGTARGET environment variable.

GEN := gen-$(TiNGTARGET)

INCLUDES := 

	# supply a default value
TiNG_TRACE := 1

CC_DEFINES := \
	-D_REENTRANT \
	-DSTATIC=static \
	-DTiNG_TRACE=$(TiNG_TRACE) \
	-DASSP_SHARE_TINGTRACE \
	-DASSP_PATH_ETHSIZE \
	$(TiNG_EXTRAS)

#	-DTiNG_PARANOIA \
#
#

COMMON_SHARED_OBJS := \
	$(GEN)/TiNGanyevent.o \
	$(GEN)/TiNGcommon.o \
	$(GEN)/TiNGconf.o \
	$(GEN)/TiNGdetect.o \
	$(GEN)/TiNGevent.o \
	$(GEN)/TiNGframerlib.o \
	$(GEN)/TiNGgenlib.o \
	$(GEN)/TiNGglobal.o \
	$(GEN)/TiNGi460lib.o \
	$(GEN)/TiNGiwr.o \
	$(GEN)/TiNGo_card.o \
	$(GEN)/TiNGo_cardmap.o \
	$(GEN)/TiNGo_chan.o \
	$(GEN)/TiNGo_coll.o \
	$(GEN)/TiNGo_disp.o \
	$(GEN)/TiNGo_framer.o \
	$(GEN)/TiNGo_i460mux.o \
	$(GEN)/TiNGo_mod.o \
	$(GEN)/TiNGo_tdmrx.o \
	$(GEN)/TiNGo_tdmtx.o \
	$(GEN)/TiNGsmbesp.o \
	$(GEN)/TiNGsmdc.o \
	$(GEN)/TiNGsmdc_async.o \
	$(GEN)/TiNGsmdc_bulkrx.o \
	$(GEN)/TiNGsmdc_cpu.o \
	$(GEN)/TiNGsmdc_cwrx.o \
	$(GEN)/TiNGsmdc_cwtx.o \
	$(GEN)/TiNGsmdc_fskrx.o \
	$(GEN)/TiNGsmdc_fsktx.o \
	$(GEN)/TiNGsmdc_hdlc.o \
	$(GEN)/TiNGsmdc_heap.o \
	$(GEN)/TiNGsmdc_ifprx.o \
	$(GEN)/TiNGsmdc_ifptx.o \
	$(GEN)/TiNGsmdc_none.o \
	$(GEN)/TiNGsmdc_rawrx.o \
	$(GEN)/TiNGsmdc_rawtx.o \
	$(GEN)/TiNGsmdc_rdbg.o \
	$(GEN)/TiNGsmdc_rlprx.o \
	$(GEN)/TiNGsmdc_rlptx.o \
	$(GEN)/TiNGsmdc_speech.o \
	$(GEN)/TiNGsmdc_sync.o \
	$(GEN)/TiNGsmdc_ttyrx.o \
	$(GEN)/TiNGsmdc_v110.o \
	$(GEN)/TiNGsmdc_v17rx.o \
	$(GEN)/TiNGsmdc_v17tx.o \
	$(GEN)/TiNGsmdc_v27rx.o \
	$(GEN)/TiNGsmdc_v27tx.o \
	$(GEN)/TiNGsmdc_v29rx.o \
	$(GEN)/TiNGsmdc_v29tx.o \
	$(GEN)/TiNGsmdc_v32.o \
	$(GEN)/TiNGsmdc_v34.o \
	$(GEN)/TiNGsmdiag.o \
	$(GEN)/TiNGsmtone.o \
	$(GEN)/TiNGsync.o \
	$(GEN)/TiNGtrace.o \
	$(GEN)/confmask.o \
	$(GEN)/globaltime.o \
	$(GEN)/iir4.o \
	$(GEN)/paranoia.o \
	$(GEN)/smbesp.o \
	$(GEN)/smdc.o \
	$(GEN)/smdc_encode.o \
	$(GEN)/trace.o \
	$(GEN)/trace_gen_show.o \
	$(GEN)/trace_smbesp_show.o \
	$(GEN)/trace_smdc_show.o

COMMON_CORE_OBJS := \
	$(GEN)/TiNGanyevent.o \
	$(GEN)/TiNGcommon.o \
	$(GEN)/TiNGconf.o \
	$(GEN)/TiNGdetect.o \
	$(GEN)/TiNGevent.o \
	$(GEN)/TiNGgenlib.o \
	$(GEN)/TiNGframerlib.o \
	$(GEN)/TiNGglobal.o \
	$(GEN)/TiNGi460lib.o \
	$(GEN)/TiNGiwr.o \
	$(GEN)/TiNGo_card.o \
	$(GEN)/TiNGo_cardmap.o \
	$(GEN)/TiNGo_chan.o \
	$(GEN)/TiNGo_coll.o \
	$(GEN)/TiNGo_disp.o \
	$(GEN)/TiNGo_framer.o \
	$(GEN)/TiNGo_i460mux.o \
	$(GEN)/TiNGo_mod.o \
	$(GEN)/TiNGo_tdmrx.o \
	$(GEN)/TiNGo_tdmtx.o \
	$(GEN)/TiNGsmbesp.o \
	$(GEN)/TiNGsmdc.o \
	$(GEN)/TiNGsmdc_async.o \
	$(GEN)/TiNGsmdc_bulkrx.o \
	$(GEN)/TiNGsmdc_cpu.o \
	$(GEN)/TiNGsmdc_cwrx.o \
	$(GEN)/TiNGsmdc_cwtx.o \
	$(GEN)/TiNGsmdc_fskrx.o \
	$(GEN)/TiNGsmdc_fsktx.o \
	$(GEN)/TiNGsmdc_hdlc.o \
	$(GEN)/TiNGsmdc_heap.o \
	$(GEN)/TiNGsmdc_ifprx.o \
	$(GEN)/TiNGsmdc_ifptx.o \
	$(GEN)/TiNGsmdc_none.o \
	$(GEN)/TiNGsmdc_rawrx.o \
	$(GEN)/TiNGsmdc_rawtx.o \
	$(GEN)/TiNGsmdc_rdbg.o \
	$(GEN)/TiNGsmdc_rlprx.o \
	$(GEN)/TiNGsmdc_rlptx.o \
	$(GEN)/TiNGsmdc_speech.o \
	$(GEN)/TiNGsmdc_sync.o \
	$(GEN)/TiNGsmdc_ttyrx.o \
	$(GEN)/TiNGsmdc_v110.o \
	$(GEN)/TiNGsmdc_v17rx.o \
	$(GEN)/TiNGsmdc_v17tx.o \
	$(GEN)/TiNGsmdc_v27rx.o \
	$(GEN)/TiNGsmdc_v27tx.o \
	$(GEN)/TiNGsmdc_v29rx.o \
	$(GEN)/TiNGsmdc_v29tx.o \
	$(GEN)/TiNGsmdc_v32.o \
	$(GEN)/TiNGsmdc_v34.o \
	$(GEN)/TiNGsmdiag.o \
	$(GEN)/TiNGsmtone.o \
	$(GEN)/TiNGsync.o \
	$(GEN)/confmask.o \
	$(GEN)/errcode.o \
	$(GEN)/event.o \
	$(GEN)/globaltime.o \
	$(GEN)/iir4.o \
	$(GEN)/once.o \
	$(GEN)/paranoia.o \
	$(GEN)/smbesp.o \
	$(GEN)/smdc.o \
	$(GEN)/smdc_encode.o \
	$(GEN)/trace.o \
	$(GEN)/trace_gen_show.o \
	$(GEN)/trace_smbesp_show.o \
	$(GEN)/trace_smdc_show.o

COMMON_PX_OBJS := \
	$(GEN)/TiNGdtls.o \
	$(GEN)/TiNGfmplib.o \
	$(GEN)/TiNGh223lib.o \
	$(GEN)/TiNGsntp.o \
	$(GEN)/TiNGsprt.o \
	$(GEN)/TiNGo_dtlsrx.o \
	$(GEN)/TiNGo_dtlstx.o \
	$(GEN)/TiNGo_fmprx.o \
	$(GEN)/TiNGo_fmptx.o \
	$(GEN)/TiNGo_h223mux.o \
	$(GEN)/TiNGo_path.o \
	$(GEN)/TiNGo_rtcph.o \
	$(GEN)/TiNGo_sntp.o \
	$(GEN)/TiNGo_sprt.o \
	$(GEN)/TiNGo_vmpcsrc.o \
	$(GEN)/TiNGo_vidmprx.o \
	$(GEN)/TiNGo_vidmptx.o \
	$(GEN)/TiNGo_vidplay.o \
	$(GEN)/TiNGo_vidrec.o \
	$(GEN)/TiNGo_vmprx.o \
	$(GEN)/TiNGo_vmptone.o \
	$(GEN)/TiNGo_vmptx.o \
	$(GEN)/TiNGpath.o \
	$(GEN)/TiNGrtplib.o \
	$(GEN)/TiNGvideo.o \
	$(GEN)/assp_conn.o \
	$(GEN)/assp_group.o \
	$(GEN)/cardconn_px.o \
	$(GEN)/create_socket.o \
	$(GEN)/pathipv4.o \
	$(GEN)/pros_x.o \
	$(GEN)/px_assp.o \
	$(GEN)/px_chan.o \
	$(GEN)/px_event.o \
	$(GEN)/px_ioctl_allocts.o \
	$(GEN)/px_ioctl_cardinfo.o \
	$(GEN)/px_ioctl_chevent.o \
	$(GEN)/px_ioctl_confin.o \
	$(GEN)/px_ioctl_confinfo.o \
	$(GEN)/px_ioctl_dbg.o \
	$(GEN)/px_ioctl_dest.o \
	$(GEN)/px_ioctl_friend.o \
	$(GEN)/px_ioctl_intslist.o \
	$(GEN)/px_ioctl_io.o \
	$(GEN)/px_ioctl_mem.o \
	$(GEN)/px_ioctl_msg.o \
	$(GEN)/px_ioctl_modinfo.o \
	$(GEN)/px_ioctl_mstate.o \
	$(GEN)/px_ioctl_outts.o \
	$(GEN)/px_ioctl_stats.o \
	$(GEN)/px_ioctl_strinfo.o \
	$(GEN)/px_ioctl_switch.o \
	$(GEN)/px_ioctl_task.o \
	$(GEN)/px_ioctl_tasklist.o \
	$(GEN)/px_ioctl_tsdest.o \
	$(GEN)/px_ioctl_xfer.o \
	$(GEN)/px_ioctls.o \
	$(GEN)/px_mod.o \
	$(GEN)/px_util.o \
	$(GEN)/refcnt.o \
	$(GEN)/smgroove.o \
	$(GEN)/timequeue_pq_evnt.o \
	$(GEN)/trace_rtp_show.o \
	$(GEN)/trace_smvideo_show.o \
	$(GEN)/trace_sprt_show.o

# dummy targets

default: default-$(TiNGTARGET)

all: all-$(TiNGTARGET)


dist: gen/apiversion.i dist-$(TiNGTARGET)

tags:
	elvtags -g -t -v *.[ch] */*.[ch]

all-args: $(PROGS) ../gen/version.i gen/apiversion.i

include $(TiNGTARGET)/Makefile.inc

# static rules

%/.exists:
	mkdir -p $(dir $@)
	:>$@

$(PROGS): gen/%.args.i: %.args ../libutil/mkargs.pl ../spTiNG-v ../relver gen/.exists errs/gen/.exists
	perl ../libutil/mkargs.pl -v ../spTiNG-v -r ../relver $< >$@

# special targets

gen/errorname.i: mkerrname.pl ../pubdoc/errlist gen/.exists errs/gen/.exists
	perl mkerrname.pl ../pubdoc/errlist >$@

gen/relver.i: ../relver mkrelver.pl gen/.exists errs/gen/.exists
	perl mkrelver.pl

gen/apiversion.i: ../gen/spTiNG-mod gen/.exists errs/gen/.exists
	sed -e 's/[^0-9]*\([0-9]*\)\([.+]\)$$/\1, '"'"'\2'"'"'/' <../gen/spTiNG-ver >gen/apitmp
	if cmp gen/apitmp $@; then rm gen/apitmp; else mv gen/apitmp $@; fi

gen/coreversion.i: ../gen/spTiNG-mod gen/.exists errs/gen/.exists
	sed -e 's/^\(m[0-9]*\)\([+.]\)/int aculab_TiNG_version_\1(void); int aculab_TiNG_version_\1(void) { return '"'"'\2'"'"' == '"'+'"'; }/' <../gen/spTiNG-ver >gen/cvtmp
	if cmp gen/cvtmp $@; then rm gen/cvtmp; else mv gen/cvtmp $@; fi

gen/checkcore_call.i: ../gen/spTiNG-mod gen/.exists errs/gen/.exists
	sed -e 's/^\(m[0-9]*\)\([+.]\)/aculab_TiNG_version_\1();/' <../gen/spTiNG-ver >gen/ccctmp
	if cmp gen/ccctmp $@; then rm gen/ccctmp; else mv gen/ccctmp $@; fi

gen/checkcore_decl.i: ../gen/spTiNG-mod gen/.exists errs/gen/.exists
	sed -e 's/^\(m[0-9]*\)\([+.]\)/extern int aculab_TiNG_version_\1(void);/' <../gen/spTiNG-ver >gen/ccdtmp
	if cmp gen/ccdtmp $@; then rm gen/ccdtmp; else mv gen/ccdtmp $@; fi

../gen/version.i: ../libutil/mkver.pl ../gen/spTiNG-mod ../relver gen/.exists errs/gen/.exists
	perl ../libutil/mkver.pl -v ../gen/spTiNG-ver -r ../relver >gen/tmp
	if cmp gen/tmp $@; then rm gen/tmp; else mv gen/tmp $@; fi

../gen/spTiNG-mod: ../spTiNG-v ../libutil/checkmod.pl
	cd ..; perl libutil/checkmod.pl spTiNG

-include $(shell find gen-$(TiNGTARGET) -name \*.M)
