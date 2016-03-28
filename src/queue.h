// vim: cin:sts=4:sw=4 
#ifndef QUEUE_H
#define QUEUE_H
#include <stdlib.h>
#include <stdint.h>

struct cbuf;

struct cbuf* cb_create( size_t size );
int cb_read (struct cbuf* cb, void* o_ptr, size_t size);
int cb_write (struct cbuf* cb, void* ptr, size_t size);
int cb_destroy(struct cbuf* cb);

#endif //QUEUE_H
