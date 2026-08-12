# Note that this makefile describes how to build various programs for
# various systems.

# The system is selected by the TiNGTARGET environment variable.

GEN := gen-$(TiNGTARGET)

CC_DEFINES= \
	-DTiNG_TRACE \
	-D_REENTRANT \
	$(TiNG_EXTRAS)

INCLUDES= -I../include

# dummy targets

all: all-$(TiNGTARGET)

dist: dist-$(TiNGTARGET)

all-args:

%/.exists:
	mkdir -p $(dir $@)
	:>$@

include $(TiNGTARGET)/Makefile.inc

include $(shell find gen -name \*.M)
include $(shell find gen-$(TiNGTARGET) -name \*.M)
