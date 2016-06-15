// vim: cin:sts=4:sw=4 

#include "pqueue.h"
#include "log.h"
#include <malloc.h>
#include <assert.h>
#include <string.h>

// this is a minheap binary tree.

#define PARENT(x) ((x) >> 1)
#define LCHILD(x) ((x) << 1)
#define RCHILD(x) (((x) << 1) + 1)

#define ROOT_EL 1

typedef int (*datacmp) (void* a, void* b);

struct pqueue_t {

    struct pqel_t* elements; ///< binary tree of elements
    size_t elcnt; ///< size of binary tree, incl. empty elements
};

struct pqel_t {
    void* value;
    unsigned int priority;
};

// a binary search-ish algorithm for finding the empty element
int _pq_find_empty (struct pqueue_t* pq) {

    unsigned int lb = 1;
    unsigned int ub = (pq->elcnt - 1);

    while (lb != ub) {

	unsigned int mid = (lb + ub) / 2;

	if (pq->elements[mid].value) lb = mid+1;
	else ub = mid;

	if (lb >= (pq->elcnt)) return -1;
    }

    return lb;
}

int _pq_find_last (struct pqueue_t* pq) { //find the last non-empty element.

    //this function returns either the index of the last non-empty element
    // (which would also be the size of a proper binary heap), or
    // zero in case all the elements are zero.

    unsigned int lb = 0;
    unsigned int ub = (pq->elcnt - 1);

    while (lb != ub) {

	unsigned int mid = (lb + ub + 1) / 2;

	if (pq->elements[mid].value) lb = mid;
	else ub = mid-1;

	if (lb >= (pq->elcnt)) return 0;
	if (ub == 0) return 0;
    }

    return lb;
}

/*int _pq_find_empty (struct pqueue_t* pq) { //old algorithm

  for (int i=1; i < (pq->elcnt); i++) {
  if (!(pq->elements[i].value)) return i;
  }

  return -1;
  }*/

int _pq_swap (struct pqel_t* p1, struct pqel_t* p2) {

    struct pqel_t tmp;

    memcpy(&tmp, p1, sizeof tmp);
    memcpy(p1, p2, sizeof tmp);
    memcpy(p2, &tmp, sizeof tmp);
    return 0;
}

int _pq_down_heap (struct pqueue_t* pq, size_t index) {

    int swap_index = -1;
    unsigned int swap_prio = pq->elements[index].priority;

    if ((LCHILD(index) < pq->elcnt) && (pq->elements[LCHILD(index)].value) &&
	    (pq->elements[LCHILD(index)].priority < swap_prio)) {

	swap_index = LCHILD(index); swap_prio = pq->elements[LCHILD(index)].priority;

    }

    if ((RCHILD(index) < pq->elcnt) && (pq->elements[RCHILD(index)].value) &&
	    (pq->elements[RCHILD(index)].priority < swap_prio)) {

	swap_index = RCHILD(index); swap_prio = pq->elements[RCHILD(index)].priority;

    }

    if (swap_index != -1) {

	_pq_swap(&pq->elements[index], &pq->elements[swap_index]);
	return _pq_down_heap(pq, swap_index);
    }

    return 0;
}

void* pq_pop (struct pqueue_t* pq, unsigned int* o_priority) {

    unsigned int r_prio = pq->elements[1].priority;
    void* r_val = pq->elements[1].value;

    int index = _pq_find_last(pq);

    if (index == 0) return NULL; //empty

    if (index > 1) memcpy( &pq->elements[1], &pq->elements[index], sizeof( struct pqel_t)); //replace root
    memset( &pq->elements[index], 0, sizeof(struct pqel_t)); //zero old element

    _pq_down_heap(pq, 1);

    if (o_priority) *o_priority = r_prio;
    return r_val;
}

int _pq_up_heap (struct pqueue_t* pq, size_t index) {

    if (index == 0) return 1 ;//shouldn't happen!
    if (index == 1) return 0; //already done.
    int p_ind = PARENT(index);

    if ( pq->elements[index].priority < pq->elements[p_ind].priority ) {
	_pq_swap(&pq->elements[index], &pq->elements[p_ind]);
	return _pq_up_heap(pq, p_ind); }

    return 0;
}

int _pq_expand (struct pqueue_t* pq) {

    size_t oldelcnt = (pq->elcnt);
    size_t newelcnt = (pq->elcnt) << 1;
    struct pqel_t* newels = realloc(pq->elements, newelcnt);
    if (pq->elements == NULL) return -1;
    pq->elements = newels;

    memset (pq->elements + oldelcnt, 0, sizeof(struct pqel_t) * oldelcnt); //fill with zeroes 
    pq->elcnt = newelcnt;

    lprintf("PQ @ %p expanded to %d elements.\n",pq,newelcnt);

    return 0;

}


int _pq_find_or_expand (struct pqueue_t* pq) {

    int r = _pq_find_empty(pq);

    if (r >= 0) return r;

    return (_pq_expand(pq) == 0) ? ((pq->elcnt) / 2) : 0;
}


int _pq_find_value (struct pqueue_t* pq, void* value) {

    for (unsigned int i=1; i < (pq->elcnt); i++) {
	if ((pq->elements[i].value) == value) return i;
    }

    return -1;
}

int pq_size (struct pqueue_t* pq) {

    return _pq_find_last(pq);
}

int pq_push (struct pqueue_t* pq, void* data, unsigned int priority) {

    assert(data);
    int same = _pq_find_value(pq, data);

    if (same != -1) {

	unsigned int oldpri = pq->elements[same].priority;

	//change the old element's priority to a new value.
	pq->elements[same].priority = priority;

	if (oldpri < priority) _pq_up_heap(pq, same); else
	    if (oldpri > priority) _pq_down_heap(pq, same);

    } else {

	//add an entirely new element to the queue.

	int newi = _pq_find_or_expand(pq);

	if (newi == -1) return -1;

	pq->elements[newi].value = data; pq->elements[newi].priority = priority;

	_pq_up_heap(pq,newi);
    }

    return 0;
}

struct pqueue_t* pq_create (void) {

    struct pqueue_t* npq = malloc (sizeof *npq);
    memset(npq, 0, sizeof *npq);

    npq->elements = malloc (sizeof (struct pqel_t) * 4); // 4 elements is the minimum
    memset(npq->elements, 0, sizeof (struct pqel_t) * 4);
    npq->elcnt = 4;

    return npq;
}

int pq_destroy(struct pqueue_t* pq) {

    free(pq->elements);
    free(pq);
    return 0;
}
