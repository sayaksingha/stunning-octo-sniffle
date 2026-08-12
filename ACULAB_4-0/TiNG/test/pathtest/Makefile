PROG_THIS_CC_DEFINES=
PROG_CC_DEFINES=
PROG_INCLUDES=

PROGS := gen/pathtest.args.i
DIST_EXTRA := gen/cfg.hpp

include ../Makefile.inc

gen/cfg.hpp gen/cfg.cpp: mkp.pl gen/.exists errs/gen/.exists
	perl mkp.pl $@ gen/cfg.cpp
