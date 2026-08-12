# Note that this makefile describes how to build various programs for
# various systems. These are:
#
#	System	Compiler	Abbreviation in variables etc.
#	------	--------	------------------------------
#	Linux	gcc		GCC
#
# The system is selected by the TiNGTARGET environment variable.

GCC_COMPILE=gcc $(CFLAGS_LINUX)	-Wstrict-prototypes -Wmissing-prototypes -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../libutil/fixdep.pl $@
GPP_COMPILE=g++ $(CFLAGS_LINUX) -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../libutil/fixdep.pl $@
LIBS := -lm -lpthread
LINK = g++ -g -ggdb -g3 -o "$@"

CFLAGS_LINUX= \
	$(CC_DEFINES) \
	-DTiNGTYPE_LINUX \
	$(INCLUDES) \
	-Wall \
	-W \
	-Wshadow \
	-Wpointer-arith \
	-g -ggdb -g3 \
	-O2


# objects for LINUX variant
TiNG_OBJS := \
	../apilib/$(GEN)/TiNGcore.o \
	../apilib/$(GEN)/prosody_x.o

CPP_OBJS := \
	$(GEN)/error_class.o \
	$(GEN)/eventset_poll.o \
	$(GEN)/demo.o \
	$(GEN)/demofile.o \
	$(GEN)/demoreplay.o \
	$(GEN)/demorecordtdm.o \
	$(GEN)/demoreplaytdm.o \
	$(GEN)/recordfile.o \
	$(GEN)/replayfile.o

DEMO_OBJS := \
	$(TiNG_OBJS) \
	$(GEN)/error_class.o \
	$(GEN)/eventset_poll.o \
	$(GEN)/demo.o

DEMOFILE_OBJS := \
	$(TiNG_OBJS) \
	$(GEN)/error_class.o \
	$(GEN)/eventset_poll.o \
	$(GEN)/demofile.o \
	$(GEN)/recordfile.o \
	$(GEN)/replayfile.o

DEMOREPLAY_OBJS := \
	$(TiNG_OBJS) \
	$(GEN)/demoreplay.o

DEMORECORDTDM_OBJS := \
	$(TiNG_OBJS) \
	$(GEN)/demorecordtdm.o

DEMOREPLAYTDM_OBJS := \
	$(TiNG_OBJS) \
	$(GEN)/demoreplaytdm.o

# dummy targets

default-$(TiNGTARGET): \
	$(GEN)/error_class.o \
	$(GEN)/eventset_poll.o

all-$(TiNGTARGET): \
	$(GEN)/demo \
	$(GEN)/demofile \
	$(GEN)/demoreplay \
	$(GEN)/demorecordtdm \
	$(GEN)/demoreplaytdm


$(GEN)/demo: $(DEMO_OBJS)
	$(LINK) $(DEMO_OBJS) $(LIBS) 2>&1 | t0 errs/$@

$(GEN)/demofile: $(DEMOFILE_OBJS)
	$(LINK) $(DEMOFILE_OBJS) $(LIBS) 2>&1 | t0 errs/$@

$(GEN)/demoreplay: $(DEMOREPLAY_OBJS)
	$(LINK) $(DEMOREPLAY_OBJS) $(LIBS) 2>&1 | t0 errs/$@

$(GEN)/demorecordtdm: $(DEMORECORDTDM_OBJS)
	$(LINK) $(DEMORECORDTDM_OBJS) $(LIBS) 2>&1 | t0 errs/$@

$(GEN)/demoreplaytdm: $(DEMOREPLAYTDM_OBJS)
	$(LINK) $(DEMOREPLAYTDM_OBJS) $(LIBS) 2>&1 | t0 errs/$@

# static rules

$(OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	$(GCC_COMPILE)

$(CPP_OBJS): $(GEN)/%.o: %.cpp $(GEN)/.exists errs/$(GEN)/.exists
	$(GPP_COMPILE)
