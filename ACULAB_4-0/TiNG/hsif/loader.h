/* loader.h - loader messages passed between host and sharc */

#ifndef INCLUDED_LOADER_H
#define INCLUDED_LOADER_H

#define MSG_ALLOC 0xc80
	/*
	 * ALLOC(resourcetype, amount)
	 * requests resource allocation.
	 * h->s only
	 */
#define MSG_ALLOC_TYPE_PM 0
#define MSG_ALLOC_TYPE_INT0 1
#define MSG_ALLOC_TYPE_INT1 2
#define MSG_ALLOC_TYPE_EXT 3

#define MSG_BADALLOC 0xc81
	/*
	 * BADALLOC()
	 * indicates requested resource allocation failed
	 * s->h only
	 */

#define MSG_LOADED 0x880
	/*
	 * LOADED(tasktype)
	 * indicates loader has finished loading a module
	 * the tasktype is the value that can be given in a
	 * CREATE message to create a task of the type loaded.
	 * s->h only
	 */

#endif
