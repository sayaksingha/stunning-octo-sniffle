/* sha.h - SHA hash */

#ifndef INCLUDED_SHA_H
#define INCLUDED_SHA_H

#define SHA_HASH_BUFFER_SIZE 5
#define SHA_HASH_EXTRA_SIZE 80

#include <stdint.h>

/*
 * perform SHA hash of data with digest[0..SHA_HASH_BUFFER_SIZE-1] while
 * using digest[SHA_HASH_BUFFER_SIZE..SHA_HASH_EXTRA_SIZE-1] as
 * scratch space.
 */
void SHATransform(uint_fast32_t digest[SHA_HASH_BUFFER_SIZE + SHA_HASH_EXTRA_SIZE], uint_fast32_t const data[16]);

#endif
