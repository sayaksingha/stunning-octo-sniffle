# Note that this makefile describes how to build various programs for
# various systems.

# The system is selected by the TiNGTARGET environment variable.

INCLUDES := -I../include

GEN := gen-$(TiNGTARGET)

CC_DEFINES := \
	-D_REENTRANT \
	$(TiNG_EXTRAS)

#
#

PROGS=

ALL_API := \
	gen/pros_x-card.hpp \
	gen/prosdc-channel.hpp \
	gen/prosgen-channel.hpp \
	gen/prospapi-channel.hpp \
	gen/prospath-path.hpp \
	gen/prosrtpapi-vmprx.hpp \
	gen/prosvid-avfplay.hpp \
	gen/prosvid-avfrec.hpp

# dummy targets

default: all-api default-$(TiNGTARGET)

all: all-api all-$(TiNGTARGET)

all-api: $(ALL_API)

dist: all-api $(GEN)/.exists errs/$(GEN)/.exists

include $(TiNGTARGET)/Makefile.inc

gen/pros_x-card.hpp: ../pubdoc/pros_x mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/pros_x-

gen/prospapi-channel.hpp: ../pubdoc/prospapi mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prospapi-

gen/prosgen-channel.hpp: ../pubdoc/prosgen mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prosgen-

gen/prosdc-channel.hpp: ../pubdoc/prosdc mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prosdc-

gen/prospath-path.hpp: ../pubdoc/prospath mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prospath-

gen/prosvid-avfplay.hpp: ../pubdoc/prosvid mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prosvid-

gen/prosvid-avfrec.hpp: ../pubdoc/prosvid mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prosvid-

gen/prosrtpapi-vmprx.hpp: ../pubdoc/prosrtpapi mkclass.pl gen/.exists errs/gen/.exists
	perl mkclass.pl <$< gen/prosrtpapi-

# static rules

%/.exists:
	mkdir -p $(dir $@)
	:>$@

$(PROGS): gen/%.args.i: %.args ../libutil/mkargs.pl ../spTiNG-v ../relver gen/.exists errs/gen/.exists
	perl ../libutil/mkargs.pl -v ../spTiNG-v -r ../relver $< >$@

# special targets

-include $(shell find gen -name \*.M)
-include $(shell find gen-$(TiNGTARGET) -name \*.M)
