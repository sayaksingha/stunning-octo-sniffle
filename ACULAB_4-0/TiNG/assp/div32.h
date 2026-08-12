#ifndef INCLUDED_DIV32_H
#define INCLUDED_DIV32_H

#include <stdint.h>

	// on retuen *qp << *sc == n / d << s
	// as if all were floating point
void div32(uint_fast32_t *qp, int_fast32_t *sc, uint_fast32_t n, uint_fast32_t d, int_fast32_t s);

#endif
