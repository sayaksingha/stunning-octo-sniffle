/* paranoia.h - do extensive object check on each use */

#ifdef PARANOIA

#define check_object(obj) \
	check_object_fn(&(obj), sizeof(obj), __FILE__, __LINE__)

#define commit_object(obj) \
	commit_object_fn(&(obj), sizeof(obj), __FILE__, __LINE__)

#define make_object(obj) \
	(((void *) &obj != (void *) &(obj).pm \
		? fprintf(stderr, "%s:%d: paracheck not at start of object\n", __FILE__, __LINE__), \
			abort() \
		: 0), \
	commit_object_fn(&(obj), sizeof(obj), __FILE__, __LINE__))

typedef struct {
	char *lfile;
	unsigned lline;
	U16 check;
} PARACHECK;

void check_object_fn(void *obj, unsigned len, char *file, int line);
void commit_object_fn(void *obj, unsigned len, char *file, int line);

#else

typedef struct {
	int dummy;
} PARACHECK;

#define check_object(obj) ((void) obj)

#define commit_object(obj) ((void) obj)

#define make_object(obj) ((void) obj)

#endif
