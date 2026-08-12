CFLAGS_EXTRA = \
	-DNO_PROSODY_S

LINK=gcc -m64 -g -L../../../lib64 -o"$@"
LINKPP=g++ -m64 -g -L../../../lib64 -o"$@"

include ../LINUX_V6/Makefile.v6
