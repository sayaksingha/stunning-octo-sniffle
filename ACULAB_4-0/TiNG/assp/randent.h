/* randent.h - true random number generator */

#ifndef INCLUDED_RANDENT_H
#define INCLUDED_RANDENT_H

void randent_init(void);

unsigned long randent32(void); 
void randent_addrand(void *data, unsigned bytes);

#endif
