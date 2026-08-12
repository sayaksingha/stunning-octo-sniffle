PROG_CC_DEFINES= \
	-D_REENTRANT

PROG_THIS_CC_DEFINES=
PROG_INCLUDES=
PROGS=

include ../Makefile.inc

# need to make errtable.i for distribution
dist: gen/errtable.i

gen/errtable.i: ../../pubdoc/errlist gen/.exists errs/gen/.exists
	sed -ne 's/^[^\t]*	\(ERR.*\)$$/case \1: return "\1";/p' $< >$@
