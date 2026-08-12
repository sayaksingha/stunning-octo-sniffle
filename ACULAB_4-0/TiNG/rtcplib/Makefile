# Note that this makefile describes how to build various programs for
# various systems.

# The system is selected by the TiNGTARGET environment variable.

GEN := gen-$(TiNGTARGET)

INCLUDES= -I../include

	# supply a default value
TiNG_TRACE=1

CC_DEFINES= \
	-DTiNGTYPE_$(TiNGTARGET) \
	-D_REENTRANT \
	-DSTATIC=static \
	-DTiNG_TRACE=$(TiNG_TRACE) \
	$(TiNG_EXTRAS)

#	-DTiNG_PARANOIA \
#
#

PROGS=

# dummy targets

all: all-$(TiNGTARGET)

dist: $(GEN)/.exists errs/$(GEN)/.exists

include $(TiNGTARGET)/Makefile.inc

# static rules

%/.exists:
	mkdir -p $(dir $@)
	:>$@

$(PROGS): gen/%.args.i: %.args ../libutil/mkargs.pl ../spTiNG-v ../relver gen/.exists errs/gen/.exists
	perl ../libutil/mkargs.pl -v ../spTiNG-v -r ../relver $< >$@

# special targets

-include $(shell find gen -name \*.M)
-include $(shell find gen-$(TiNGTARGET) -name \*.M)
