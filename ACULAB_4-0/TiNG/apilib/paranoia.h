/* paranoia.h - do extensive object check on each use */
#ifndef INCLUDED_PARANOIA_H
#define INCLUDED_PARANOIA_H

/*
 * The locking hierarchy used is that locking proceeds from the more
 * general to the less general, so for example, a module must be
 * locked before any channels within it. When it is necessary to lock
 * two items at the same level (e.g. two channels), this can be done
 * in any order but can only be done while holding the previous
 * level's mutex. This is because at various times the requirements
 * demand that we lock two channels in opposite orders, so this is
 * only safe if we can guarantee that when we have acquired the first
 * channel's lock the only threads which might hold the other channel's
 * lock cannot deadlock by trying to acquire the lock we already
 * hold.
 *
 * The most general mutex is the 'anychan' mutex.
 *
 * Note that the clever tricks used here to check for object
 * corruption do not work when anychan operation is in use because
 * part of the channel object is protected only by the anychan mutex,
 * so the channel checksum can change while the channel lock is not
 * held.
 */

#include "mutx.h"

#ifdef TiNG_PARANOIA

LOCALDEC void check_object(void *obj, unsigned len, char *file, int line);
LOCALDEC void commit_object(void *obj, unsigned len, char *file, int line);

#define prep_lock_object(obj) \
	check_object((obj), sizeof(*(obj)), __FILE__, __LINE__)

#define prep_unlock_object(obj) \
	commit_object((obj), sizeof(*(obj)), __FILE__, __LINE__)

#define make_object(obj) \
	(((void *) obj != (void *) &(obj)->pm.mx \
		? fprintf(stderr, "%s:%d: Mutex not at start of object\n", __FILE__, __LINE__), \
			abort() \
		: 0), \
	commit_object((obj), sizeof(*(obj)), __FILE__, __LINE__))

typedef struct {
	MUTX mx;
	char *lfile;
	unsigned lline;
	unsigned short check;
} PARAMUTX;

#else

typedef struct {
	MUTX mx;
} PARAMUTX;

#define prep_lock_object(obj) ((void) 0)

#define prep_unlock_object(obj) ((void) 0)

#define make_object(obj) ((void) 0)

#define PARANOIA_FIELDS

#endif

#define lock_object(obj) \
	(mutx_enter(&(obj)->pm.mx), \
	prep_lock_object(obj))

#define unlock_object(obj) \
	(prep_unlock_object(obj), \
	mutx_leave(&(obj)->pm.mx))

#endif
