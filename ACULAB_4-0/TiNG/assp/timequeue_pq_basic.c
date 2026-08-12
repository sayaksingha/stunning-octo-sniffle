#include <stdlib.h>
#include <stdio.h>
#include "timequeue_pq_basic.h"
#include "asspdbg.h"

#ifdef TiNGTYPE_STARCORE
#include "debugger/udbg.h"
#define tq_basic_faultmsg udbg
#else
#define tq_basic_faultmsg assp_log
#endif

//#define TQ_BASIC_CHECKTREE

typedef struct timequeue_impl TQI;

#define arlen(x) (sizeof(x)/sizeof(*(x)))

/* Based on priority queues from:
 * 	The Art of Computer Programming, Volume 3, Sorting and Searching
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

#ifdef TQ_BASIC_CHECKTREE

static void badtree(char *how)
{
 fprintf(stderr, "tree malformed: %s\n", how);
 abort();
}

static void checktree(TIMER *t)
{
 if (!t) return;
 if (t->left) {
	checktree(t->left);
	if (time_cmp(t->when, t->left->when) > 0) badtree("T.key > T.left.key");
 } else {
 	if (t->right) badtree("T.left.dist == 0 < T.right.dist");
 }
 if (t->right) {
	checktree(t->right);
	if (time_cmp(t->when, t->right->when) > 0) badtree("T.key > T.right.key");
	if (t->left && t->left->dist < t->right->dist) badtree("T.left.dist < T.right.dist");
	if (t->dist != 1 + t->right->dist) badtree("T.dist != 1 + T.right.dist");
 } else {
	if (t->dist != 1) badtree("T.dist != 1 + distT.right) == 0");
 }
}
#else
#define checktree(t)
#endif

#if 0
static void showtree(TIMER *t, char *pfx, int lvl)
{
 while (t) {
	char buff[20];
	pfx[lvl] = 0;
	time_string(buff, buff+arlen(buff), time_until(t->when));
	Log(0, "-%s d=%2d tim=%s", pfx, t->dist, buff);
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
 tqi->stats.nstart++;
 if (!tqi->root || time_cmp(tqi->root->when, when) < 0) {
 	tqi->stats.atend++;
 }
 //printf("TQ(%d)\n", tqi->stats.nstart - tqi->stats.nrun);
#endif
 if (tim->fn) {
	tq_basic_faultmsg("timer already running\n");
 	assp_error();
 }
 tim->when = when;
 tim->fn = fn;
 tim->arg = arg;
 checktree(tqi->root);
 tqi->root = pq_enqueue(tqi->root, tim);
 checktree(tqi->root);
 tqi->qlen++;
 //if (tqi->qlen > tqi->maxqlen) printf("maxq=%d\n", tqi->maxqlen);
}

static void stoptimer(TQI *tqi, TIMER *tim)
{
 if (!tim->fn) {
	tq_basic_faultmsg("stopping timer that is not running\n");
 	assp_error();
 }
 checktree(tqi->root);
 tqi->root = pq_delnode(tim);
 checktree(tqi->root);
 tim->fn = 0;
}

static long next_timer_mS(TQI *tqi)
{
 for (;;) {
	TIMER *t = tqi->root;
	void (*fn)(void *p);
	void *arg;
	int_fast64_t nx;
	if (!t) return -1;
	nx = time_rel2i64uS(time_until(t->when));
	if (nx > 0) return (nx + 999) / 1000;
	tqi->root = pq_delnode(t);
	tqi->qlen--;
#ifdef TIMEQUEUE_STATS
	tqi->stats.nrun++;
#endif
	fn = t->fn;
	arg = t->arg;
	t->fn = 0;
	fn(arg);	// NB: might restart same timer
 }
}

static void timequeue_pq_dtor(TIMEQUEUE_IMPL *tq)
{
 (void) tq;
#ifdef TIMEQUEUE_STATS
 printf("Timequeue:\n\tStart: %u\n\tRun: %u\nAtend: %u\n",
	tq->stats.nstart, tq->stats.nrun, tq->stats.atend);
#endif
}

static struct timequeue_vtbl timequeue_pq_vtbl = {
	starttimer,
	stoptimer,
	next_timer_mS,
	timequeue_pq_dtor,
};

void timequeue_pq_init(TIMEQUEUE *tq, TIMEQUEUE_IMPL *tqi)
{
 tq->vtbl = &timequeue_pq_vtbl;
 tq->tqi = tqi;
 tqi->root = 0;
 tqi->qlen = 0;
 tqi->maxqlen = 0;
#ifdef TIMEQUEUE_STATS
 tqi->stats.atend = 0;
 tqi->stats.nstart = 0;
 tqi->stats.nrun = 0;
#endif
}
