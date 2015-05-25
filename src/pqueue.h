// vim: cin:sts=4:sw=4 
#ifndef PQUEUE_H
#define PQUEUE_H
#include <unistd.h>
// an insane idea for a statically-allocated priority queue.

struct pqel {
	int p;
	void* v;
};

int pq_add_element(struct pqel* array, size_t sz, void* ptr, int priority);

void* pq_get_lowest (struct pqel* array, size_t sz);

int pq_decrease_priority (struct pqel* array, size_t sz, void* ptr, int newpriority);

int pq_decrease_or_add (struct pqel* array, size_t sz, void* ptr, int priority);

int pq_empty (struct pqel* array, size_t sz);

#endif
