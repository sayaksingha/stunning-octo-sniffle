# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#
# The system is selected by the TiNGTARGET environment variable.

PX_LIB := ../../apilib/$(GEN)/prosody_x.o

GCC_COMPILE=gcc $(CFLAGS_LINUX)	-Wstrict-prototypes -Wmissing-prototypes -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../../libutil/fixdep.pl $@
GPP_COMPILE=g++ $(CFLAGS_LINUX) -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../../libutil/fixdep.pl $@
LIBS=-lm -lpthread
LINK=gcc -g -o "$@"
LINKPP=g++ -g -o "$@"

CFLAGS_LINUX= \
	$(CC_DEFINES) \
	$(PROG_CC_DEFINES) \
	$(PROG_THIS_CC_DEFINES) \
	-pthread \
	-DACU_LINUX \
	-DTiNGTYPE_LINUX \
	-DLINUX \
	-DUNIX_SYSTEM \
	$(INCLUDES) \
	$(PROG_INCLUDES) \
	-Wall \
	-W \
	-Wshadow \
	-Wpointer-arith \
	-g \
	-O

#	-finstrument-functions \
#

TiNG_LIB = \
	$(PX_LIB) \
	../../apilib/$(GEN)/TiNGcore.o

CALL_LIB = \
	../../call/$(GEN)/cllib.o \
	../../call/$(GEN)/clunix.o \
	../../call/$(GEN)/common.o

SWITCH_LIB = \
	../../switch/$(GEN)/swlib.o \
	../../switch/$(GEN)/swunix.o

-include $(GEN)/objs.mak

# static rules

$(PROG_OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

$(PROG_CPP_OBJS): $(GEN)/%.o: %.cpp $(GEN)/.exists errs/$(GEN)/.exists
	$(GPP_COMPILE)

$(PROG_THIS_OBJS): $(GEN)/%.o: LINUX/%.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

$(PROG_CPP_THIS_OBJS): $(GEN)/%.o: LINUX/%.cpp $(GEN)/.exists errs/$(GEN)/.exists
	$(GPP_COMPILE)

$(GEN)/%.s: %.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(CFLAGS_LINUX) -O2 -S -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../../libutil/fixdep.pl $@

-include /dev/null $(shell find $(GEN) -name \*.M)
