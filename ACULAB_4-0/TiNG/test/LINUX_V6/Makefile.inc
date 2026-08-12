CFLAGS_EXTRA = 

LINK=gcc -m32 -g -L../../../lib -o"$@"
LINKPP=g++ -m32 -g -L../../../lib -o"$@"

include ../LINUX_V6/Makefile.v6
