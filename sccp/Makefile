# @(#) $Id: Makefile,v 1.1.4.2.2.1 2021/12/01 12:43:25 jamesjac Exp $
ROOTDIR = $(PROJ_HOME)

   include $(PROJ_HOME)/makeutil/header.mod
#DEFS += -D_XOPEN_SOURCE=600 -DLINUX -DCOMMONOAM -DLINT_ARGS
   DECFS += -DLINUX -DLINT_ARGS -D_REENTRANT
#FLAGS += #-DSS7_1_4 #-DGT_IND #-D__cplusplus=1
FLAGS += -D__cplusplus=1
  LDFLAGS = -ldl -lpthread 

ifneq (,$(findstring 64,$(shell getconf LONG_BIT) ))
ACULAB_LIB = $(TP_HOME)/ACULAB/lib64/libacu_ss7sccp.so
else
ACULAB_LIB = $(TP_HOME)/ACULAB/lib/libacu_ss7sccp.so
endif

#################################################################
# define the list of source files.
SRCS = SccpAculabHandler.cc\
       SccpAculabHandlerMain.cc\

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
LIBFLAGS = $(PROJ_HOME)/ss7util/lib/libSs7Util.a \
           $(UTIL_HOME)/lib/libutil.a\
           $(ACULAB_LIB)

LIBDIR = lib

#################################################################
OBJS1 =  
OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name.
OFILE1 = 

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
OBJS2 = SccpAculabUtil.o
OBJS2 := $(OBJS2:%=$(OBJDIR)/%)

# list of libraries.
LIBS2 = 

# output file name.
OFILE2 = libSccpAculabUtil.a

# Program dependencies
$(OFILE2): $(LIBDIR)/$(OFILE2)
$(LIBDIR)/$(OFILE2): $(OBJS2) $(LIBS2)
##################################################################
OBJS3 = SccpAculabApi.o
OBJS3 := $(OBJS3:%=$(OBJDIR)/%)

# list of libraries.
LIBS3 = 

# output file name.
OFILE3 = libSccpAculabApi.a

# Program dependencies
$(OFILE3): $(LIBDIR)/$(OFILE3)
$(LIBDIR)/$(OFILE3): $(OBJS3) $(LIBS3)
##################################################################
OBJS4 = SccpAculabHandler.o SccpAculabHandlerMain.o
OBJS4 := $(OBJS4:%=$(OBJDIR)/%)

# list of libraries.
LIBS4 = $(PROJ_HOME)/Aculab/sccp/lib/libSccpAculabUtil.a\
        $(PROJ_HOME)/Aculab/sccp/lib/libSccpAculabApi.a\

OFILE4 = SCCP_Handler_ACULAB

# Program dependencies
$(OFILE4): $(LIBDIR)/$(OFILE4)
$(BINDIR)/$(OFILE4): $(OBJS4) $(LIBS4)
##################################################################

# List of all object files
OBJS = $(OBJS2) $(OBJS3) $(OBJS4)

# List of all libraries to be created
OLIB = $(OFILE2) $(OFILE3)
OLIB := $(OLIB:%=$(LIBDIR)/%)

# List of all executables to be created
PROG =  $(OFILE4)
PROG := $(PROG:%=$(BINDIR)/%)
#################################################################

   include $(PROJ_HOME)/makeutil/footer.mod


