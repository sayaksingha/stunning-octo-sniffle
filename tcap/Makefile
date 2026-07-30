# @(#) $Id: Makefile,v 1.1.4.7.2.1 2021/12/01 12:43:26 jamesjac Exp $
ROOTDIR = $(PROJ_HOME)

   include $(PROJ_HOME)/makeutil/header.mod
#DEFS += -D_XOPEN_SOURCE=600 -DLINUX -DCOMMONOAM -DLINT_ARGS
   DECFS += -DLINUX -DLINT_ARGS -D_REENTRANT -DKAFKA_BRIDGE
#FLAGS += #-DSS7_1_4 #-DGT_IND #-D__cplusplus=1

#FLAGS += -D__cplusplus=1
#Above commented to use Kafka Releated changes in tcapmsg
FLAGS += -D__cplusplus=1 -DKAFKA_BRIDGE
LDFLAGS = -ldl -lpthread 

ifneq (,$(findstring 64,$(shell getconf LONG_BIT) ))
ACULAB_LIB = $(TP_HOME)/ACULAB/lib64/libacu_ss7tcap.so
else
ACULAB_LIB = $(TP_HOME)/ACULAB/lib/libacu_ss7tcap.so
endif

#################################################################
# define the list of source files.
SRCS = TcapAculabUtil.cc\
       TcapAculabDlgMgr.cc\
       TcapAculabTransDlgMap.cc\
       TcapAculabApi.cc\
       TcapAculabHandler.cc\
       TcapAculabHandlerMain.cc\
       TcapAculabHandlerTraffic.cc\
       TcapAculabHandlerTrafficMain.cc\
       TcapAculabDlgCleaner.cc\
       TcapAculabDlgCleanerMain.cc

#SigSegv.cc

SRCS := $(SRCS:%=$(SRCDIR)/%)

#################################################################
# define the include flags.
INCLFLAGS = -I include\
            -I $(PROJ_HOME)/include\
            -I $(PROJ_HOME)/ss7util/include\
            -I $(UTIL_HOME)/include\
            -I $(UTIL_HOME)/msgq/include \
            -I $(UTIL_HOME)/shmq/include \
            -I $(UTIL_HOME)/configinfo/include \
            -I $(UTIL_HOME)/strtok/include \
            -I $(UTIL_HOME)/socket/include \
            -I $(UTIL_HOME)/datastructures/include \
            -I $(UTIL_HOME)/diskmonitor/include \
            -I $(UTIL_HOME)/filedeletion/include \
            -I $(UTIL_HOME)/semaphore/include \
            -I $(UTIL_HOME)/timer/include \
            -I $(UTIL_HOME)/trace/include \
            -I $(UTIL_HOME)/protocolstack/include \
            -I $(UTIL_HOME)/processlock/include \
            -I $(UTIL_HOME)/diskbuffer/include \
            -I $(UTIL_HOME)/octet/include \
            -I $(UTIL_HOME)/log/include\
            -I $(UTIL_HOME)/db/include\
            -I $(UTIL_HOME)/tssutil/include\
            -I $(UTIL_HOME)/peg/include\
            -I $(TP_HOME)/SQLAPI/include \
            -I $(TP_HOME)/ACULAB/include

# define the library flags.
LIBFLAGS =$(PROJ_HOME)/ss7util/lib/libSs7Util.a \
           $(UTIL_HOME)/lib/libutil.a\
           $(ACULAB_LIB)

LIBDIR = lib

#################################################################
# list of generated object files.
OBJS1 = TcapAculabUtil.o
OBJS1 = TcapAculabUtil.o

OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 =  

# output file name.
OFILE1 = libAculabUtil.a

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(LIBDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# list of generated object files.
OBJS2 = TcapAculabDlgMgr.o 
OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 =  

# output file name.
OFILE2 = libAculabDlgMgr.a

# Program dependencies
$(OFILE2): $(LIBDIR)/$(OFILE2)
$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)


#################################################################
# list of generated object files.
OBJS3 = TcapAculabTransDlgMap.o 
OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
#LIBS3 = $(PROJ_HOME)/Aculab/tcap/lib/libAculabDlgMgr.a 
LIBS3 =  
        

# output file name.
OFILE3 = libAculabTransDlgMap.a

# Program dependencies
$(OFILE3): $(BINDIR)/$(OFILE3)
$(LIBDIR)/$(OFILE3): $(OBJS3) $(LIBS3)


#################################################################
# list of generated object files.
OBJS4 = TcapAculabUtil.o TcapAculabApi.o
OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# list of libraries.
#LIBS4 = $(PROJ_HOME)/Aculab/tcap/lib/libAculabDlgMgr.a
LIBS4 =  


# output file name.
OFILE4 = libAculabApi.a

# Program dependencies
$(OFILE4): $(BINDIR)/$(OFILE4)
$(LIBDIR)/$(OFILE4): $(OBJS4) $(LIBS4)


#################################################################
# list of generated object files.
#OBJS5 = SigSegv.o TcapAculabHandler.o TcapAculabHandlerMain.o
OBJS5 = TcapAculabHandler.o TcapAculabHandlerMain.o
OBJS5 := $(OBJS5:%=$(OBJDIR)/%)

# list of libraries.
LIBS5 = $(PROJ_HOME)/Aculab/tcap/lib/libAculabUtil.a\
        $(PROJ_HOME)/Aculab/tcap/lib/libAculabTransDlgMap.a\
        $(PROJ_HOME)/Aculab/tcap/lib/libAculabDlgMgr.a\
        $(PROJ_HOME)/Aculab/tcap/lib/libAculabApi.a
        

# output file name.
OFILE5 = TCAP_Handler_ACULAB

# Program dependencies
$(OFILE5): $(LIBDIR)/$(OFILE5)
$(BINDIR)/$(OFILE5): $(OBJS5) $(LIBS5)

#################################################################
# list of generated object files.
OBJS6 = TcapAculabHandlerTraffic.o TcapAculabHandlerTrafficMain.o
OBJS6 := $(OBJS6:%=$(OBJDIR)/%)

# list of libraries.
LIBS6 = $(PROJ_HOME)/Aculab/tcap/lib/libAculabDlgMgr.a

# output file name.
OFILE6 = TCAP_Handler_ACULAB_Traffic
$(OFILE6): $(LIBDIR)/$(OFILE6)
$(BINDIR)/$(OFILE6): $(OBJS6) $(LIBS6)

#################################################################
                       
# list of generated object files.
OBJS7 = TcapAculabDlgCleaner.o TcapAculabDlgCleanerMain.o
OBJS7 := $(OBJS7:%=$(OBJDIR)/%)

# list of libraries.
LIBS7 = $(PROJ_HOME)/Aculab/tcap/lib/libAculabUtil.a\
         $(PROJ_HOME)/Aculab/tcap/lib/libAculabDlgMgr.a

# output file name.
OFILE7 = TCAP_Handler_ACULAB_Dlg_Cleaner
$(OFILE7): $(LIBDIR)/$(OFILE7)
$(BINDIR)/$(OFILE7): $(OBJS7) $(LIBS7)
#################################################################
# List of all object files
OBJS = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJ6) $(OBJ7)

# List of all libraries to be created
OLIB = $(OFILE1) $(OFILE2) $(OFILE3)  $(OFILE4)
OLIB := $(OLIB:%=$(LIBDIR)/%)

# List of all executables to be created
PROG = $(OFILE5) $(OFILE6) $(OFILE7)
PROG := $(PROG:%=$(BINDIR)/%)
#################################################################

include $(PROJ_HOME)/makeutil/footer.mod


