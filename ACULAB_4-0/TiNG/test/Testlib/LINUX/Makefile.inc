
PROG_INCLUDES=  \
	-I../../../call/include \
	-I../../../switch/include

dummy: \
	$(GEN)/cardlist.o \
	$(GEN)/cardopen.o \
	$(GEN)/checkplay.o \
	$(GEN)/checkrec.o \
	$(GEN)/errcode_cl.o \
	$(GEN)/errcode_sm.o \
	$(GEN)/errcode_sw.o \
	$(GEN)/error.o \
	$(GEN)/eventset.o \
	$(GEN)/fmtcode.o \
	$(GEN)/modlist.o \
	$(GEN)/modopen.o \
	$(GEN)/perthread_trace.o \
	$(GEN)/remote_trace.o \
	$(GEN)/rtpclerk_cli.o \
	$(GEN)/rtprx_codec.o \
	$(GEN)/rtptx_codec.o \
	$(GEN)/taistamp.o \
	$(GEN)/tracehistory.o \
	$(GEN)/watchl1.o
	@true

all-LINUX:  dummy

$(GEN)/errcode_sm.o: gen/errtable.i $(GEN)/.exists errs/$(GEN)/.exists
