LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(PROG_CPP_OBJS) $(PROG_CPP_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_THIS_CC_DEFINES= \
	-D_REENTRANT

$(GEN)/pathtest: ../../gen/version.i gen/pathtest.args.i $(LINK_OBJS)
	$(LINKPP) $(LINK_OBJS) -lpthread $(LIBS) 2>&1 | t0 errs/$@

all-LINUX:  $(GEN)/pathtest

$(LINK_OBJS): gen/cfg.hpp $(GEN)/.exists errs/$(GEN)/.exists
