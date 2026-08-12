CFLAGS_CPU = \
	-m64 \
	-DTiNGCPUTYPE_AMD64

LINK=gcc -m64 -g -L../../../lib64 -o "$@"

include ../LINUX_V6/Makefile.v6
