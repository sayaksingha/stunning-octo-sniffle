# Note that this makefile describes how to build various programs for
# various systems.
#
# The system is selected by the TiNGTARGET environment variable.

INCLUDES=

CC_DEFINES=$(TiNG_EXTRAS)

GEN := gen-$(TiNGTARGET)

# dummy targets

all: all-$(TiNGTARGET)

dist: all

all-args: ../gen/version.i

include $(TiNGTARGET)/Makefile.inc

# static rules

%/.exists:
	mkdir -p $(dir $@)
	:>$@

../gen/version.i: ../libutil/mkver.pl ../gen/spTiNG-mod ../relver gen/.exists errs/gen/.exists
	perl ../libutil/mkver.pl -v ../gen/spTiNG-ver -r ../relver >gen/tmp
	if cmp gen/tmp $@; then rm gen/tmp; else mv gen/tmp $@; fi

../gen/spTiNG-mod: ../spTiNG-v ../libutil/checkmod.pl
	cd ..; mkdir -p gen; perl libutil/checkmod.pl spTiNG

-include $(shell find gen -name \*.M)
-include $(shell find gen-$(TiNGTARGET) -name \*.M)
