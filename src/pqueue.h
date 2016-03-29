// vim: cin:sts=4:sw=4 
#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdio.h>

struct pqueue_t;

struct pqueue_t* pq_create (void);
int pq_destroy(struct pqueue_t* pq);

int pq_size (struct pqueue_t* pq);

int pq_push (struct pqueue_t* pq, void* data, unsigned int priority);
void* pq_pop (struct pqueue_t* pq, unsigned int* o_priority);

#endif /* PQUEUE_H */
