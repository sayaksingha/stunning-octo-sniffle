LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_INCLUDES=  \
	-I../../../call/include \
	-I../../../switch/include

PROG_THIS_CC_DEFINES= \
	-D_REENTRANT

$(GEN)/incall: ../../gen/version.i gen/incall.args.i $(LINK_OBJS)
	$(LINK) $(LINK_OBJS) -lpthread $(LIBS) 2>&1 | t0 errs/$@

all-$(TiNGTARGET): $(GEN)/incall
