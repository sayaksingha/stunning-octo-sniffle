LINK_OBJS= $(PROG_OBJS) $(PROG_THIS_OBJS) $(SHARED_OBJS) $(SHARED_THIS_OBJS)

PROG_INCLUDES=  \
	-I../../../include

PROG_THIS_CC_DEFINES=

$(GEN)/switch: ../../gen/version.i $(LINK_OBJS)
	$(LINK) $(LINK_OBJS) $(LIBS_COMMON) $(LIBS_SW) 2>&1 | t0 errs/$@

all-$(TiNGTARGET): $(GEN)/switch
