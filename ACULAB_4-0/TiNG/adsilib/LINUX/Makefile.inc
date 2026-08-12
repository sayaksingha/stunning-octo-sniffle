# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#	Linux	gcc		GCC
#
# The system is selected by the TiNGTARGET environment variable.

GCC_COMPILE=$(CFLAGS_LINUX) -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../libutil/fixdep.pl $@ $<

CFLAGS_LINUX= \
	$(CC_DEFINES) \
	-DTiNGTYPE_LINUX \
	$(INCLUDES) \
	-Wall \
	-W \
	-Wshadow \
	-Wpointer-arith \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-g \
	-O2


# objects for LINUX variant

OBJS= \
	$(GEN)/adsilib.o

# dummy targets

all-LINUX: \
	$(GEN)/adsilib.o


# static rules

$(OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_COMPILE)
