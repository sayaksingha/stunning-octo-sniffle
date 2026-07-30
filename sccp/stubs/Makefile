# @(#) $Id: Makefile,v 1.1.4.1.4.1 2021/12/01 12:43:25 jamesjac Exp $
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
SRCS = sccp.cc 


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
OBJS1 = sccp.o 
OBJS1 := $(OBJS1:%=$(OBJDIR)/%)

# list of libraries.
LIBS1 = 

# output file name.
OFILE1 = SCCP_Handler_ACULAB

# Program dependencies
$(OFILE1): $(LIBDIR)/$(OFILE1)
$(BINDIR)/$(OFILE1): $(OBJS1) $(LIBS1)

#################################################################
# List of all object files
OBJS = $(OBJS1) 

# List of all libraries to be created
OLIB =  
OLIB := $(OLIB:%=$(LIBDIR)/%)

# List of all executables to be created
PROG = $(OFILE1) 
PROG := $(PROG:%=$(BINDIR)/%)
#################################################################

include $(PROJ_HOME)/makeutil/footer.mod


