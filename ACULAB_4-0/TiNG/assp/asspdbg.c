
#include <stdio.h>
#include <stdlib.h>
#include "asspdbg.h"

#if defined(WIN32) && defined(_DEBUG) && defined(TiNGCPUTYPE_AMD64)
#include <intrin.h>
#pragma intrinsic(__debugbreak)
#endif

int assp_error(void)
{
#if defined(WIN32) && defined(_DEBUG)
#ifdef TiNGCPUTYPE_AMD64
 __debugbreak();
#else
 _asm { int 3 }
#endif
#endif
 abort();
 return 1;
}

