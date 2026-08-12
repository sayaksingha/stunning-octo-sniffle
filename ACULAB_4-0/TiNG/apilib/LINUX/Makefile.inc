# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#	Linux	gcc		GCC
#
# The system is selected by the TiNGTARGET environment variable.

GCC_COMPILE=$(CFLAGS_LINUX) -c -MMD -o "$@" $< 2>&1 | grep -v "declared ['\`]static' but never defined" | t0 errs/$@; perl ../libutil/fixdep.pl $@

CC_SPECIFIC_DEFINES := \
	$(CC_DEFINES) \
	-DTiNGTYPE_LINUX \
	-D_REENTRANT \

CFLAGS_LINUX := \
	$(CC_SPECIFIC_DEFINES) \
	$(INCLUDES) \
	-Wall \
	-W \
	-Wshadow \
	-Wpointer-arith \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-g \
	-O2

#	-finstrument-functions \
#

# objects for LINUX variant
PX_ENTRY := ../pubdoc/gen/pros_x.TiNG_entry
PX_LIB := $(GEN)/prosody_x.o
PX_SEPARATE = $(PX_OBJS)

OBJS := \
	$(COMMON_PX_OBJS) \
	$(COMMON_SHARED_OBJS)

LINUX_OBJS :=

POSIX_OBJS := \
	$(GEN)/condvar.o \
	$(GEN)/errcode.o \
	$(GEN)/event.o \
	$(GEN)/mutx.o \
	$(GEN)/once.o

TiNGCORE_EXTRA_OBJS := \
	$(COMMON_CORE_OBJS)

TiNGCORE_OBJS := \
	$(GEN)/TiNGcore.o \
	$(GEN)/condvar.o \
	$(GEN)/mutx.o \
	$(COMMON_CORE_OBJS)

X_OBJS := \
	../assp/$(GEN)/assp.o \
	$(COMMON_PX_OBJS)

TiNGLIB_OBJS := \
	$(GEN)/TiNGglobal.o \
	$(S_OBJS) \
	$(TiNGCORE_OBJS)

API_ENTRY := \
	unpub.TiNG_entry \
	$(PX_ENTRY) \
	../pubdoc/gen/prosdc.TiNG_entry \
	../pubdoc/gen/prosgen.TiNG_entry \
	../pubdoc/gen/prospapi.TiNG_entry \

# dummy targets

default-$(TiNGTARGET): \
	$(GEN)/TiNGcore.o \
	$(GEN)/TiNGtrace.o \
	$(PX_LIB) \

all-$(TiNGTARGET): \
	$(GEN)/TiNGcore.o \
	$(GEN)/TiNGlib.o \
	$(GEN)/TiNGtrace.o \
	$(PX_LIB) \

separate: $(TiNGLIB_OBJS) $(PX_SEPARATE)

# for resource manager
dist-$(TiNGTARGET): $(GEN)/TiNGcore.o

# specials

$(GEN)/prosody_x.o: prosody_x.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_COMPILE)

$(GEN)/TiNGcore.o: TiNGcore.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_COMPILE)

$(GEN)/TiNGlib.o: TiNGlib.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(GCC_COMPILE)

# static rules

$(OBJS): $(GEN)/%.o: %.c gen/apiversion.i $(GEN)/.exists errs/$(GEN)/.exists
	gcc -DLOCALDEF= -DLOCALDEC=extern $(GCC_COMPILE)

$(LINUX_OBJS): $(GEN)/%.o: LINUX/%.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc -DLOCALDEF= -DLOCALDEC=extern $(GCC_COMPILE)

$(POSIX_OBJS): $(GEN)/%.o: POSIX/%.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc -DLOCALDEF= -DLOCALDEC=extern $(GCC_COMPILE)

# initial dependencies

$(GEN)/TiNGcore.o: ../gen/version.i gen/apiversion.i gen/coreversion.i gen/errorname.i gen/relver.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/prosody_x.o: gen/checkcore_decl.i gen/checkcore_call.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/TiNGlib.o: ../gen/version.i gen/relver.i gen/coreversion.i gen/checkcore_decl.i gen/checkcore_call.i gen/errorname.i $(GEN)/.exists errs/$(GEN)/.exists

