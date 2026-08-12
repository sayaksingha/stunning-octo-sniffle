#include <stdlib.h>
#include <stdio.h>
#include "timequeue_pq_evnt.h"
#include "trace.h"
#include "TiNGcommon.h"

typedef struct timequeue_impl TQI;

#define arlen(x) (sizeof(x)/sizeof(*(x)))

/* Based on priority queues from:
 *	The Art of Computer Programming, Volume 3, Sorting and Searching
 * Section 5.2.3
 *
 * adapted to have 'up' pointers in the nodes.
 * tree properties: for all nodes, x:
 *	key(0) == +infinity
 *	key(x) <= key(left(x))
 *	key(x) <= key(right(x))
 *	dist(0) == 0
 *	dist(left(x)) >= dist(right(x))
 *	dist(x) == 1 + dist(right(x))
 */

#if 0
static void verify_tree(TIMER *t, TIMER *up)
{
 unsigned dl = 0, dr = 0;
 if (!t) return;
 if (t->up != up) {
	fprintf(stderr, "%p->up = %p, expected %p\n", t, t->up, up);
	abort();
 }
 if (t->left) {
	if (time_cmp(t->when, t->left->when) > 0) {
		fprintf(stderr, "x=%p: key(x) > key(left(x))\n", t);
		abort();
	}
	dl = t->left->dist;
	verify_tree(t->left, t);
 }
 if (t->right) {
	if (time_cmp(t->when, t->right->when) > 0) {
		fprintf(stderr, "x=%p: key(x) > key(right(x))\n", t);
		abort();
	}
	dr = t->right->dist;
	verify_tree(t->right, t);
 }
 if (dl < dr) {
	fprintf(stderr, "x=%p: x->left=%p->dist=%u < x->right=%p->dist=%u\n",
		t, t->left, dl, t->right, dr);
	abort();
 }
 if (t->dist != dr + 1) {
 	fprintf(stderr, "x=%p: x->dist=%u != 1 + %u\n", t, t->dist, dr);
	abort();
 }
}
#else
#define verify_tree(x, y)
#endif

#if 0
static void showtree(TIMER *t, char *pfx, int lvl)
{
 while (t) {
	char buff[20];
	pfx[lvl] = 0;
	time_string(buff, buff+arlen(buff), time_until(t->when));
	olog("-%s d=%2d tim=%s\n", pfx, t->dist, buff);
	pfx[lvl] = '<';
	showtree(t->left, pfx, lvl+1);
	pfx[lvl++] = '>';
	t = t->right;
 }
}

static void dumppq(TIMER *t, char *pfx)
{
 showtree(t, pfx, strlen(pfx));
}
#endif

static TIMER *pq_merge(TIMER *p, TIMER *q)
{
 /*
  * When this function is called to delete from the middle of a tree,
  * p points to the tree and q is the old right branch of the node to
  * be deleted. Therefore we must still perform the corrections
  * upwards from p even if q is null. Similarly, if p is null but q
  * isn't, we must explicitly clear the up pointer in q before
  * returning q, since that field was pointing to the deleted node.
  *
  * In this loop, p points into a tree and q points to the root of a
  * tree to be merged into the first tree at p.
  */
 if (!p) {
	if (q) q->up = 0;
	return q;
 }
 if (q) {
	for (;;) {     // M2
		TIMER *t;
		// assert: p && q && irrelevant(q->up) && (!p->up || p->up->right == p)
		if (time_cmp(p->when, q->when) > 0) {
			// swap q tree with p subtree
			if ((q->up = p->up)) {
				q->up->right = q;
			}
			t = p;
			p = q;
			q = t;
		}
		t = p->right;
		if (!t) {
			p->right = q;
			q->up = p;
			break;
		}
		p = t;
	}
  }
 /*
  * At this point p points to the lowest part of the tree that was
  * modified. We must fix its dist field and propagate the change
  * back up the tree. At each node we also may need to swap the
  * left and right subtrees to guarantee the dist(l) >= dist(r)
  * property. This will take us to the top of the tree, which is
  * useful as we need to return a pointer to the root.
  */
 // M3
 for (;;) {
	TIMER *r = p->right;
	if (!r) {
		p->dist = 1;
	} else {
		TIMER *l = p->left;
		if (!l) {
			p->left = r;
			p->right = 0;
			p->dist = 1;
		} else if (l->dist < r->dist) {
			p->left = r;
			p->right = l;
			p->dist = l->dist+1;
		} else p->dist = r->dist+1;
	}
	q = p->up;
	if (!q) return p;
	p = q;
 }
}

static TIMER *pq_delnode(TIMER *p)
{
 TIMER *l = p->left;
 TIMER *r = p->right;
 if (p->up) {
	if (!l) {	// must also have !r
		if (p->up->right == p) p->up->right = 0;
		else p->up->left = 0;
		return pq_merge(p->up, 0);
	}
	if (r && time_cmp(l->when, r->when) > 0) {
		TIMER *t = l;
		l = r;
		r = t;
	}
	if (p->up->right == p) p->up->right = l;
	else p->up->left = l;
 }
 if (l) l->up = p->up;
 return pq_merge(l, r);
}

static TIMER *pq_enqueue(TIMER *root, TIMER *t)
{
 t->up = 0;
 t->left = 0;
 t->right = 0;
 t->dist = 1;
 return pq_merge(root, t);
}

static void starttimer(TQI *tqi, TIMER *tim, void (*fn)(void *arg), void *arg, ABSTIME when)
{
#ifdef TIMEQUEUE_STATS
 tqi->dbg_nins++;
 if (!tqi->root || time_cmp(tqi->root->when, when) < 0) tqi->dbg_atend++;
 tqi->dbg_numtim++;
 if (tqi->dbg_numtim > tqi->dbg_maxnum) {
	tqi->dbg_maxnum = tqi->dbg_numtim;
	//printf("dbg_numtim = %u\n", tqi->dbg_numtim);
	//printf("dbg_nins = %u\n", tqi->dbg_nins);
	//printf("dbg_ncancel = %u\n", tqi->dbg_ncancel);
	//printf("dbg_atend = %u\n", tqi->dbg_atend);
	//printf("dbg_maxnum = %u\n", tqi->dbg_maxnum);
 }
#endif
 if (tim->fn) die("Timer already running");
 tim->when = when;
 tim->fn = fn;
 tim->arg = arg;
 verify_tree(tqi->root, 0);
 tqi->root = pq_enqueue(tqi->root, tim);
 verify_tree(tqi->root, 0);
 if (tqi->root == tim) evnt_signal(tqi->newroot);
}

static void freetimer(TQI *tqi, TIMER *t)
{
 t->fn = 0;
#ifdef TIMEQUEUE_STATS
 if (!tqi->dbg_numtim--) die("decrementing dbg_numtim from 0");
#else
 (void) tqi;
#endif
}

static void stoptimer(TQI *tqi, TIMER *tim)
{
 if (tim) {
	if (!tim->fn) die("cancelling timer which is not running\n");
	verify_tree(tqi->root, 0);
	tqi->root = pq_delnode(tim);
	verify_tree(tqi->root, 0);
	freetimer(tqi, tim);
#ifdef TIMEQUEUE_STATS
	tqi->dbg_ncancel++;
#endif
 }
}

static long next_timer_mS(TQI *tqi)
{
 for (;;) {
	TIMER *t = tqi->root;
	void (*fn)(void *p);
	void *arg;
	long nx;
	if (!t) return -1;
	nx = time_rel2longmS(time_until(t->when));
	if (nx > 0) return nx;
	tqi->root = pq_delnode(t);
	fn = t->fn;
	arg = t->arg;
	freetimer(tqi, t);
	fn(arg);	// NB: might restart same timer
 }
}

static void timequeue_pq_dtor(TQI *tqi)
{
#ifdef TIMEQUEUE_STATS
 if (tqi->dbg_numtim) {
	fprintf(stderr, "timequeue_pq_dtor: dbg_numtim = %u\n", tqi->dbg_numtim);
	//abort();
 }
 printf("dbg_numtim = %u\n", tqi->dbg_numtim);
 printf("dbg_nins = %u\n", tqi->dbg_nins);
 printf("dbg_ncancel = %u\n", tqi->dbg_ncancel);
 printf("dbg_atend = %u\n", tqi->dbg_atend);
 printf("dbg_maxnum = %u\n", tqi->dbg_maxnum);
#else
 (void) tqi;
#endif
}

static struct timequeue_vtbl timequeue_pq_vtbl = {
	starttimer,
	stoptimer,
	next_timer_mS,
	timequeue_pq_dtor,
};

LOCALDEF void timequeue_pq_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi, EVNT newroot)
{
 tq->vtbl = &timequeue_pq_vtbl;
 tq->tqi = tqi;
 tqi->root = 0;
 tqi->newroot = newroot;
#ifdef TIMEQUEUE_STATS
 tqi->dbg_numtim = 0;
 tqi->dbg_maxnum = 0;
 tqi->dbg_nins = 0;
 tqi->dbg_atend = 0;
#endif
}
