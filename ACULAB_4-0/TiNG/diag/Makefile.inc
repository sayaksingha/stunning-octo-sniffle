# Note that this makefile describes how to build various programs for
# various systems.
#
# The system is selected by the TiNGTARGET environment variable.

GEN := gen-$(TiNGTARGET)

CC_DEFINES= \
	-DTiNGTYPE_$(TiNGTARGET) \
	$(TING_EXTRA)

INCLUDES= \
	-I../../include

# dummy targets

all: all-$(TiNGTARGET)

all-args: $(PROGS) ../../gen/version.i

dist: $(PROGS)

# static rules

$(PROGS): gen/%.args.i: %.args ../../libutil/mkargs.pl ../../spTiNG-v ../../relver gen/.exists errs/gen/.exists
	perl ../../libutil/mkargs.pl -v ../../spTiNG-v -r ../../relver $< >$@

%/.exists:
	mkdir -p $(dir $@)
	:>$@

# special targets

../../gen/version.i: ../../libutil/mkver.pl ../../relver ../../gen/spTiNG-mod gen/.exists errs/gen/.exists
	perl ../../libutil/mkver.pl -v ../../gen/spTiNG-ver -r ../../relver >gen/tmp
	if cmp gen/tmp $@; then rm gen/tmp; else mv gen/tmp $@; fi

../../gen/spTiNG-mod: ../../spTiNG-v ../../libutil/checkmod.pl
	cd ../..; perl libutil/checkmod.pl spTiNG

$(GEN)/objs.mak: objlist $(TiNGTARGET)/objlist ../../libutil/obj2make.pl $(GEN)/.exists errs/$(GEN)/.exists
	perl ../../libutil/obj2make.pl

include ../$(TiNGTARGET)/Makefile.inc
include $(TiNGTARGET)/Makefile.inc
