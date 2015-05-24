// vim: cin:sts=4:sw=4 
#include "pqueue.h"
#include <limits.h>

struct pqel* pq_find_empty_element (struct pqel* array, size_t sz) {

	for (size_t i=0; i < sz; i++)
		if (array[i].v == NULL) return &array[i];
	return NULL;
}

int pq_add_element(struct pqel* array, size_t sz, void* ptr, int priority) {

	struct pqel* newel = pq_find_empty_element(array,sz);
	if (newel == NULL) return 1;

	newel->p = priority;
	newel->v = ptr;
	return 0;
}

void* pq_get_lowest(struct pqel* array, size_t sz) {

	int lowest_p = INT_MAX; void* lowest_v = NULL; int lowest_i = -1;
	for (size_t i=0; i < sz; i++)
		if ((array[i].v != NULL) && (array[i].p < lowest_p)) {
			lowest_p = array[i].p;
			lowest_v = array[i].v;
			lowest_i = i;
		}
	
	if (lowest_i != -1) array[lowest_i].v = NULL;
	return lowest_v;
}

int pq_decrease_priority(struct pqel* array, size_t sz, void* ptr, int newpriority) {
	
	for (size_t i=0; i < sz; i++)
		if (array[i].v == ptr) {
			array[i].p = newpriority; return 0; }

	return 1;
}

int pq_decrease_or_add(struct pqel* array, size_t sz, void* ptr, int priority) {
	
	for (size_t i=0; i < sz; i++)
		if (array[i].v == ptr) {
			array[i].p = priority; return 0; }

	return 2 + (pq_add_element(array,sz,ptr,priority));
}


int pq_empty (struct pqel* array, size_t sz) {
	for (size_t i=0; i < sz; i++)
		if (array[i].v != NULL) return 0;

	return 1;
}
