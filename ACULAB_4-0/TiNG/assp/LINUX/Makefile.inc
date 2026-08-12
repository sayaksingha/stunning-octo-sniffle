LINUX_GCC_COMPILE=$(CFLAGS_LINUX) -c -MD -o "$@" $< 2>&1 | t0 errs/$@; perl fixdep.pl $@

CFLAGS_LINUX= \
	$(CC_DEFINES) \
	$(CFLAGS_CPU) \
	-DTiNGTYPE_LINUX \
	-DPROVIDE_TRACE \
	-DUSE_FLOAT_KLUDGE \
	-DUSE_GETTIMEOFDAY \
	$(INCLUDES) \
	-Wall \
	-W \
	-Wshadow \
	-Wpointer-arith \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-g # -O3

# objects for LINUX variant

LINUX_OBJS= \
	$(GEN)/assp_main.o \
	$(GEN)/asspdbg.o \
	$(GEN)/assptest.o \
	$(GEN)/client.o \
	$(GEN)/cookie.o \
	$(GEN)/decode_str.o \
	$(GEN)/div32.o \
	$(GEN)/gasdev.o \
	$(GEN)/md5.o \
	$(GEN)/md5_stream.o \
	$(GEN)/pathnet.o \
	$(GEN)/pathsim.o \
	$(GEN)/pktqueue.o \
	$(GEN)/ran2.o \
	$(GEN)/randent_dev.o \
	$(GEN)/server.o \
	$(GEN)/sha.o \
	$(GEN)/sim.o \
	$(GEN)/simevent.o \
	$(GEN)/simtimer.o \
	$(GEN)/testfp.o \
	$(GEN)/timefn.o \
	$(GEN)/timequeue_pq_basic.o \
	$(GEN)/txqueue.o \
	$(GEN)/vseprintf.o

ASSP_OBJS= \
	$(GEN)/assp_main.o \
	$(GEN)/asspdbg.o \
	$(GEN)/cookie.o \
	$(GEN)/decode_str.o \
	$(GEN)/div32.o \
	$(GEN)/md5.o \
	$(GEN)/md5_stream.o \
	$(GEN)/pktqueue.o \
	$(GEN)/randent_dev.o \
	$(GEN)/sha.o \
	$(GEN)/txqueue.o \
	$(GEN)/timefn.o \
	$(GEN)/vseprintf.o

TESTFP_OBJS= \
	$(GEN)/float_em.o \
	$(GEN)/testfp.o \
	../libutil/$(GEN)/random.o

SIM_OBJS= \
	$(GEN)/assp.o \
	$(GEN)/gasdev.o \
	$(GEN)/pathsim.o \
	$(GEN)/ran2.o \
	$(GEN)/sim.o \
	$(GEN)/simevent.o \
	$(GEN)/simtimer.o \
	$(GEN)/timequeue_pq_basic.o \
	$(GEN)/float_nat.o

SERVER_OBJS= \
	$(GEN)/assp.o \
	$(GEN)/assptest.o \
	$(GEN)/pathnet.o \
	$(GEN)/server.o \
	$(GEN)/timequeue_pq_basic.o \
	$(GEN)/float_nat.o

CLIENT_OBJS= \
	$(GEN)/assp.o \
	$(GEN)/assptest.o \
	$(GEN)/client.o \
	$(GEN)/pathnet.o \
	$(GEN)/timequeue_pq_basic.o \
	$(GEN)/float_nat.o

# dummy targets

all-LINUX: $(GEN)/testfp $(GEN)/asspsim $(GEN)/asspserver $(GEN)/asspclient

dist-LINUX:

# linking for Linux

$(GEN)/assp.o: $(ASSP_OBJS)
	ld -g -r -o $@ $(ASSP_OBJS) 2>&1 | t0 errs/$@

$(GEN)/testfp: $(TESTFP_OBJS)
	gcc -g -o $@ $(TESTFP_OBJS) -lm 2>&1 | t0 errs/$@

$(GEN)/asspsim: $(SIM_OBJS)
	gcc -g -o $@ $(SIM_OBJS) -lm 2>&1 | t0 errs/$@

$(GEN)/asspserver: $(SERVER_OBJS)
	gcc -g -o $@ $(SERVER_OBJS) -lm 2>&1 | t0 errs/$@

$(GEN)/asspclient: $(CLIENT_OBJS)
	gcc -g -o $@ $(CLIENT_OBJS) -lm 2>&1 | t0 errs/$@


# extra dependencies

$(GEN)/client.o: gen/client.args.i ../gen/version.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/server.o: gen/server.args.i ../gen/version.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/sim.o: gen/sim.args.i ../gen/version.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/testfp.o: gen/testfp.args.i ../gen/version.i $(GEN)/.exists errs/$(GEN)/.exists
$(GEN)/float_em.o: gen/divtbl.i $(GEN)/.exists errs/$(GEN)/.exists
# not really true, but forces it to be built for Px kernel
$(GEN)/float_nat.o: gen/divtbl.i $(GEN)/.exists errs/$(GEN)/.exists

# static rules

$(LINUX_OBJS): $(GEN)/%.o: %.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(LINUX_GCC_COMPILE)

$(GEN)/%.s: %.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc -S $(CFLAGS_LINUX) -c -MD -o "$@" $< 2>&1 | t0 errs/$@

# special rules

$(GEN)/float_em.o: float.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc -DTiNG_EMULATE_FLOAT $(LINUX_GCC_COMPILE)

$(GEN)/float_nat.o: float.c $(GEN)/.exists errs/$(GEN)/.exists
	gcc $(LINUX_GCC_COMPILE)
