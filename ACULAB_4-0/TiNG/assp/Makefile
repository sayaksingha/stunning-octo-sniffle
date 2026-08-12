# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#	Windows	Microsoft C	WINNT_MS
#	Windows	gcc		WINNT_GCC
#	Linux	gcc		LINUX_GCC
#
# The system is selected by the TiNGTARGET environment variable. The
# compiler is selected by the target requested (use X.exe for MS,
# just X for gcc).

GEN := gen-$(TiNGTARGET)

INCLUDES=

PROGS= \
	gen/client.args.i \
	gen/server.args.i \
	gen/sim.args.i \
	gen/testfp.args.i

CC_DEFINES = \
	-DTIME_TAI64N \
	-DASSP_SHARE_TINGTRACE \
	$(TiNG_EXTRAS)

# dummy targets

all: all-$(TiNGTARGET)

dist: $(PROGS) gen/divtbl.i dist-$(TiNGTARGET)

$(PROGS): gen/%.args.i: %.args mkargs.pl gen/.exists errs/gen/.exists
	perl mkargs.pl $< >$@

%/.exists:
	mkdir -p $(dir $@)
	:>$@

# special targets

../gen/version.i: ../libutil/mkver.pl ../relver ../gen/spTiNG-mod gen/.exists errs/gen/.exists
	perl ../libutil/mkver.pl -v ../gen/spTiNG-ver -r ../relver >gen/tmp
	if cmp gen/tmp $@; then rm gen/tmp; else mv gen/tmp $@; fi

gen/divtbl.i: divtbl.pl gen/.exists errs/gen/.exists
	perl divtbl.pl >$@

include $(TiNGTARGET)/Makefile.inc
include $(shell find gen-$(TiNGTARGET) -name \*.M)
