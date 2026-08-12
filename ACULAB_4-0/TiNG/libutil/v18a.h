/* v18a.h - V.18 Annex A definitions */

#ifndef INCLUDED_V18A_H
#define INCLUDED_V18A_H

typedef struct {
	unsigned nsame;
	unsigned char mode;
} V18ASTATE;

/* construct a V18STATE */
void v18astate(V18ASTATE *ts);

/* destroy a V18STATE */
void v18astate_dtor(V18ASTATE *ts);

/* translate characters from V.18 Annex A 5-bit codes to T.50 7-bit codes */
char *v18a_fromT50(char *os, char *oe, V18ASTATE *ts, unsigned char **ibuf, unsigned char *ie);

/* translate characters from T.50 7-bit codes to V.18 Annex A 5-bit codes */
char *v18a_toT50(char *os, char *oe, V18ASTATE *ts, unsigned char **ibuf, unsigned char *ie);

/* display symbolic representation of V.18 Annex A 5-bit codes in a form
 * showing details for debugging
 */
void v18a_show(void (*ofn)(void *p, char c), void *p, V18ASTATE *ts, unsigned char c);

#endif
