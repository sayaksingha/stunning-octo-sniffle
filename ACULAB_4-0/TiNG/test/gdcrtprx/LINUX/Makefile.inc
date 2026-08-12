LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_THIS_CC_DEFINES=

$(GEN)/gdcrtprx: ../../gen/version.i gen/gdcrtprx.args.i $(LINK_OBJS)
	$(LINK) $(LINK_OBJS) $(LIBS) 2>&1 | t0 errs/$@

all-LINUX:  $(GEN)/gdcrtprx
