# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#	Linux	gcc		GCC
#
# The system is selected by the TiNGTARGET environment variable.

GCC_COMPILE=gcc $(CFLAGS) -c -MD -o "$@" $< 2>&1 > errs/$@; cat errs/$@; perl ../libutil/fixdep.pl $@

GCC_WARNINGS := \
	-W \
	-Wshadow \
	-Wpointer-arith

CFLAGS= \
	$(CC_DEFINES) \
	-DTiNGTYPE_LINUX \
	$(INCLUDES) \
	$(GCC_WARNINGS) \
	-Wall \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-O2 -g

# objects for this variant in this directory

GENERIC_OBJS= \
	$(GEN)/aculog.o \
	$(GEN)/alaw.o \
	$(GEN)/bfile.o \
	$(GEN)/bfopen.o \
	$(GEN)/complx.o \
	$(GEN)/fir.o \
	$(GEN)/floio.o \
	$(GEN)/gasdev.o \
	$(GEN)/iir.o \
	$(GEN)/mulaw.o \
	$(GEN)/random.o \
	$(GEN)/remez.o \
	$(GEN)/v18a.o \
	$(GEN)/vseprintf.o

# objects for this variant in the POSIX directory

POSIX_OBJS := \
	$(GEN)/generic_io.o

# objects for this variant in its directory

SPECIFIC_OBJS=

# dummy targets

all-$(TiNGTARGET): $(GEN)/t0 $(GENERIC_OBJS) $(POSIX_OBJS) $(SPECIFIC_OBJS) $(GEN)/gcc-tsprofile.o $(GEN)/gcc-tspfunc.o

# linking for Linux

$(GEN)/t0: t0.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc -o $@ t0.c


$(GEN)/gcc-tsprofile.o: gcc-tsprofile.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_WARNINGS) -g -O3 -fomit-frame-pointer -c -o "$@" $< 2>&1 | t0 errs/"$@"

$(GEN)/gcc-tspfunc.o: gcc-tspfunc.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_WARNINGS) -g -O3 -fomit-frame-pointer -finstrument-functions -c -o "$@" $< 2>&1 | t0 errs/"$@"

# static rules

$(GENERIC_OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

$(POSIX_OBJS): $(GEN)/%.o: POSIX/%.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

$(SPECIFIC_OBJS): $(GEN)/%.o: LINUX/%.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

# initial dependencies

$(GEN)/generic_io.o: ../gen/version.i $(GEN)/.exists errs/$(GEN)/.exists
