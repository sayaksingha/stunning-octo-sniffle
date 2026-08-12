/* bfile.h - buffered file I/O */

#ifndef INCLUDED_BFILE_H
#define INCLUDED_BFILE_H

#ifdef TiNGTYPE_WINNT
#ifdef TiNG_USE_DLL
#ifndef ACUAPI
#define ACUAPI __declspec(dllimport)
#endif
#endif
#endif

#ifndef ACUAPI
#define ACUAPI
#endif

#ifndef ACUTiNG_WINAPI
#define ACUTiNG_WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * General information
 * -------------------
 *
 * Every function returns zero to indicate success, or an errno value
 * if an error occurs. Errors are also 'sticky', so a BFILE object
 * which has experienced an error will keep returning the error code
 * for all operations attempted until either bfclose() or bferror()
 * is called. This avoids the need to continually check for errors
 * when performing a sequence of small reads, writes, and seeks.
 */

typedef struct BFILE BFILE;

#include <stddef.h>
#include <sys/types.h>

/*
 * Create a BFILE object. If it is successfully created, it must eventually
 * be destroyed by calling bfile_dtor(). If this function indicates an
 * error, no object has been created.
 */
ACUAPI int ACUTiNG_WINAPI bfile(BFILE **bfilepp);

/*
 * Make a BFILE object refer to some underlying representation. The
 * functions provided are passed the parameter 'p'. Each must return
 * 0 when they succeed or an errno value for failure. They are:
 *	read	Read up to `bsize' bytes into buffer `buf'. Store the
 *		number of bytes read in `*nr' (even on error). If no
 *		data is read, this is considered to be an end of file
 *		condition, and the caller of bfread() is guaranteed
 *		to receive exactly one end-of-file condition.
 *	write	Write up to `bsize' bytes from buffer `buf'. Store the
 *		number of bytes written in `*nr' (even on error).
 *	seek	Seek to given position (`offset' and `whence' have
 *		the same meaning as for lseek(). Store the new
 *		position at `*pos' which is an offset from the start
 *		of the file.
 *	close	Close the file.
 */
ACUAPI int ACUTiNG_WINAPI bfsetup(BFILE *bfp,
		int (ACUTiNG_WINAPI *read)(size_t *nr, void *buf, size_t bsize, void *p),
		int (ACUTiNG_WINAPI *write)(size_t *nw, void *buf, size_t bsize, void *p),
		int (ACUTiNG_WINAPI *seek)(off_t *pos, void *p, off_t offset, int whence),
		int (ACUTiNG_WINAPI *close)(void *p),
		void *p);

/*
 * seek to a specific position in the open file. The resulting
 * position, as an offset from the start of the file, is stored in
 * `*pos'. The `offset' and `whence' parameters have the same meaning
 * as for the standard 'lseek' function.
 */
ACUAPI int ACUTiNG_WINAPI bfseek(BFILE *bfp, off_t *pos, off_t offset, int whence);

/*
 * write data to an open file. The file must have been opened for
 * writing (or for update). Up to `size' bytes from the memory area
 * pointed to by * `ptr' are written. All the data is written unless an
 * error occurs. The number of bytes not written is stored at `*left',
 * so this is zero if the function returns zero (i.e. no error).
 * However, a non-zero return value does not necessarily indicate that
 * any data remains unwritten.
 */
ACUAPI int ACUTiNG_WINAPI bfwrite(BFILE *bfp, size_t *left, void *ptr, size_t size);

/*
 * flush any data which has been written and which remains buffered.
 */
ACUAPI int ACUTiNG_WINAPI bfflush(BFILE *bfp);

/*
 * read data from an open file. The file must have been opened for
 * reading (or for update). Up to `size' bytes are read and stored at
 * the memory area pointed to by `ptr'. The number of bytes actually
 * read is stored at `*nread'. If the end-of-file has already been
 * reached, no data will be read, so `*nread' will be zero. This is not
 * an error. If bfread() is called again, another attempt will be made
 * to read data. Note that the number of bytes read is independent of
 * whether or not an error occurred, since some data may have been read
 * before the error occurred.
 */
ACUAPI int ACUTiNG_WINAPI bfread(BFILE *bfp, size_t *nread, void *ptr, size_t size);

/*
 * report the last error, 0 if none. This also clears the error.
 */
ACUAPI int ACUTiNG_WINAPI bferror(BFILE *bfp);

/*
 * Destroy a BFILE object, closing the related file, if any.
 */
ACUAPI int ACUTiNG_WINAPI bfile_dtor(BFILE *bfp);

#ifdef __cplusplus
}
#endif

#endif
