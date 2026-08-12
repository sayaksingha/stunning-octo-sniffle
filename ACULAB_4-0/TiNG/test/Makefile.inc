# Note that this makefile describes how to build various programs for
# various systems.
#
# The system is selected by the TiNGTARGET environment variable.
#
# Typically, a program foo is in directory foo and its Makefile includes
# this file, leading to a sequence of Makefile inclusions like this:
#	foo/Makefile
#	 Makefile.inc
#	  LINUX_V6/Makefile.inc
#	   LINUX_V6/Makefile.v6
#	    LINUX/Makefile.inc (first part - variables)
#	     foo/gen-LINUX_V6/objs.mak
#	    LINUX/Makefile.inc (second part - static rules)
#	     foo/gen-LINUX_V6/*.M
#	 foo/LINUX_V6/Makefile.inc
#

GEN := gen-$(TiNGTARGET)

CC_DEFINES= $(TiNG_EXTRAS)

INCLUDES= \
	-I../../apilib \
	-I../../libutil \
	-I../../highapi

# dummy targets

all: all-$(TiNGTARGET)

all-args: $(PROGS) ../../gen/version.i

dist: $(PROGS) $(DIST_EXTRA)

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
