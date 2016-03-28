// vim: cin:sts=4:sw=4 
#include "queue.h"
#include <stdint.h>
#include <string.h>

struct cbuf {

	char* buffer; ///< pointer to actual data
	size_t bufsz; ///< size of buffer above

	size_t availsz; ///< bytes unread from the buffer
	size_t bufpos; ///< current position in the buffer
};

struct cbuf* cb_create( size_t size ) {

	struct cbuf* nbuf = malloc( sizeof *nbuf );

	if (!nbuf) { return NULL; }

	nbuf->buffer = malloc(size);
	
	if (!(nbuf->buffer)) { free(nbuf); return NULL; }

	nbuf->bufsz = size;
	nbuf->availsz = 0;
	nbuf->bufpos = 0;

	return nbuf;
}

int _cb_expand(struct cbuf* cb, size_t newsize ) {

	//hopefully, you created a large enough buffer so that this operation isn't necessary.	

	if (newsize < (cb->bufsz)) return 1;

	if (cb->bufpos + cb->availsz < cb->bufsz) {
	
	size_t split = (cb->bufsz - cb->bufpos);
	
	cb->buffer = realloc(cb->buffer, newsize);
	size_t oldsize = cb->bufsz;
	cb->bufsz = newsize;

	if (cb->bufpos + cb->availsz < newsize) {

	//new size only fits part of the remainder.
	
	size_t split2 = (newsize - cb->bufpos);
	
	/* bbbbb___aaaaa split = aaaaa, availsz - split = bbbbb */
	/* bb______aaaaabbb split2 = aaaaabbb, availsz - split2 = bb */
	/* first move the first three bs to the end */
	/* then move the last two to the start */

	memcpy(cb->buffer + oldsize, cb->buffer, (newsize - oldsize));
	memcpy(cb->buffer, cb->buffer + (newsize - oldsize) , cb->availsz - split2);

	} else {

	//new size fits the remainder entirely.

	memcpy(cb->buffer + cb->bufpos, cb->buffer, cb->availsz - split);
	
	}

	} else {

	cb->buffer = realloc(cb->buffer, newsize);
	cb->bufsz = newsize;

	}

	return 0;
} 

int cb_read (struct cbuf* cb, void* o_ptr, size_t size) {

	if (cb->bufsz < size) return 2;
	if (cb->availsz < size) return 1;

	if (cb->bufpos + size > cb->bufsz) {

	/* ####_________#### */

	size_t split = (cb->bufsz - cb->bufpos);
	
	memcpy(o_ptr, cb->buffer + cb->bufpos, split);
	memcpy(o_ptr + split, cb->buffer, size - split);

	cb->bufpos = size - split;
	cb->availsz -= size;

	} else {

	/* ____#########____ */

	memcpy(o_ptr, cb->buffer + cb->bufpos, size);

	cb->bufpos += size;
	cb->availsz -= size;	
	
	if (cb->bufpos == cb->bufsz) cb->bufpos = 0;
	
	}
	return 0;

}

int cb_write (struct cbuf* cb, void* ptr, size_t size) {

	if (cb->availsz + size > cb->bufsz) {
	
	int r = _cb_expand(cb, (cb->availsz + size));
	if (r) return 3; }

	size_t split = (cb->bufsz + cb->availsz - cb->bufpos);

	if ( size < split ) {
	
	/* ____#########____ */
	
	memcpy(cb->buffer + cb->bufpos + cb->availsz, ptr, size);

	cb->availsz += size;	
	
	} else {
	
	/* ####_________#### */

	memcpy(cb->buffer + cb->bufpos + cb->availsz, ptr, split);
	memcpy(cb->buffer, ptr + split, size - split);
	
	cb->availsz += size;	

	}
	return 0;
} 

int cb_destroy(struct cbuf* cb) {

	free (cb->buffer);
	free (cb);
	return 0;
}
