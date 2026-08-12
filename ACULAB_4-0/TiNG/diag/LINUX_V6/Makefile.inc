CFLAGS_CPU = \
	-m32 \
	-DTiNGCPUTYPE_X86

LINK=gcc -m32 -g -L../../../lib -o "$@"

include ../LINUX_V6/Makefile.v6
