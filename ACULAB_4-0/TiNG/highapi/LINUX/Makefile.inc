COMPILE=cc $(CFLAGS_LINUX) -g -O2 -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl ../libutil/fixdep.pl $@

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
	-O2

# objects for LINUX variant

OBJS= \
	$(GEN)/smbfhlib.o \
	$(GEN)/smbfwavlib.o \
	$(GEN)/smclib.o \
	$(GEN)/smhlib.o \
	$(GEN)/smwavlib.o

# dummy targets

all-LINUX: $(OBJS)

dist-LINUX:


# static rules

$(OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	$(COMPILE)
