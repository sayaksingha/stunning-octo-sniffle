/* decode_str.h - decode a rx packet into a string for assp */

#ifndef INCLUDED_DECODE_STR_H
#define INCLUDED_DECODE_STR_H

#include <stdint.h>

char *decode_str(char *op, char *oep, uint8_t *sp, uint8_t *iep);

#endif
