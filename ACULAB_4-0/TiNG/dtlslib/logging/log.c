#include <stdio.h>
#include <stdarg.h>
#include "logging/log.h"

void AcuDTLSLog( const char* frmt, ... ) {
  	char buffer[256];
  	va_list args;
  	va_start (args, frmt);
  	vsnprintf (buffer,256,frmt, args);
  	printf("%s\n",buffer);
  	va_end (args);
}

