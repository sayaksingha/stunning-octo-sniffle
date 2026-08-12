LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_THIS_CC_DEFINES=

$(GEN)/cardinfo: ../../gen/version.i gen/cardinfo.args.i $(LINK_OBJS)
	$(LINK) $(LINK_OBJS) 2>&1 | t0 errs/$@

all-$(TiNGTARGET): $(GEN)/cardinfo
