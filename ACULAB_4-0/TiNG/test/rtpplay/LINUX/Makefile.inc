LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_THIS_CC_DEFINES= \
	-D_REENTRANT

$(GEN)/rtpplay: ../../gen/version.i gen/rtpplay.args.i $(LINK_OBJS)
	$(LINK) $(LINK_OBJS) -lpthread $(LIBS) 2>&1 | t0 errs/$@

all-LINUX:  $(GEN)/rtpplay
