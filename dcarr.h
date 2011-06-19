/*********************************************************************
 * dcarr.h - A generic dynamic array implemented as macros.          *
 *                                                                   *
 * The array uses a circular buffer, thus it has amortized constant  *
 * time insertion/removal at both ends. It is therefore well suited  *
 * for FIFO queues and deques, and as a general purpose dynamic      *
 * array. This implementation is also know as a deque array.         *
 *                                                                   *
 * The memory overhead is 3 * sizeof(int) + n * sizeof(elemtype)     *
 * where 0 <= n <= N / 2, and N is the number of elements.           *
 *                                                                   *
 * Compared to a basic dynamic array, it only stores one extra       *
 * integer: an internal offset.                                      *
 *********************************************************************/

#ifndef DCARR_H
#define DCARR_H

/* redefine these at will */
#define dcarr_alloc   malloc
#define dcarr_realloc realloc
#define dcarr_free    free
#define dcarr_oom()   exit(-1)

#include <string.h> /* memmove, memset */

/*
 * Defines arraytype as an array with elements of type elemtype.
 *
 * Expands to a typedef struct.
 */
#define dcarr_define_type(arraytype, elemtype) \
	typedef struct arraytype { \
		elemtype *els; \
		unsigned int cap; \
		unsigned int off; \
		unsigned int len; \
	} arraytype

/*
 * Initializes an array and sets the initial capacity
 */
#define dcarr_init(a, valtype, capacity) do{\
	if ((capacity) > 0) { \
		(a).els = (valtype *)dcarr_alloc((capacity) * sizeof(valtype)); \
		if (!(a).els) dcarr_oom(); \
	} \
	else \
		(a).els = (valtype *)0; \
	(a).cap = (capacity); \
	(a).off = 0; \
	(a).len = 0; \
}while(0)

/*
 * Access the element at index i, possible to assign to.
 *
 * If you access an index out of bounds, the result is undefined.
 */
#define dcarr_elem(a, i) \
	((a).els[dcarr_idx((a), (i))])

/*
 * Returns the number of elements in the array.
 */
#define dcarr_len(a) ((a).len)

/*
 * Insert an element at the beginning
 */
#define dcarr_unshift(a, valtype, value) do{ \
	dcarr_reserve((a), valtype, 1); \
	(a).off = ((a).off + (a).cap - 1) % (a).cap; \
	(a).els[(a).off] = (value); \
	(a).len++; \
}while(0)

/*
 * Remove an element at the beginning and return its value
 */
#define dcarr_shift(a, value) do{ \
	(value) = (a).els[(a).off]; \
	if (++(a).off >= (a).cap) (a).off = 0; \
	(a).len--; \
}while(0)

/*
 * Insert an element at the end
 */
#define dcarr_push(a, valtype, value) do{ \
	dcarr_reserve((a), valtype, 1); \
	(a).els[dcarr_idx((a), (a).len++)] = (value); \
}while(0)

/*
 * Remove an element at the end and return its value, O(1)
 */
#define dcarr_pop(a, value) \
	((value) = (a).els[dcarr_idx((a), --(a).len)])

/*
 * Insert at an arbitrary position, O(n)
 */
#define dcarr_insert(a, i, valtype, value) do{ \
	dcarr_reserve((a), valtype, 1); \
	if ((i) < (a).len) { \
		/* need to move elements */ \
		if (dcarr_idx(a, i) > (a).off && (a).off != 0) { \
			/* move the beginning of the list backwards */ \
			memmove(&((a).els[(a).off-1]), \
			        &((a).els[(a).off]), \
			        sizeof(valtype) * (dcarr_idx(a, i) - (a).off)); \
			(a).off--; \
		} else { \
			/* move the end of the list forward */ \
			memmove(&((a).els[dcarr_idx(a, i)+1]), \
			        &((a).els[dcarr_idx(a, i)]), \
			        sizeof(valtype) * ((a).len - i)); \
		} \
	}\
	(a).len++; \
	dcarr_elem((a), (i)) = (value); \
}while(0)

/*
 * Resizes the array.
 *
 * If increasing it, the values at unassigned indices are undefined.
 * If decreasing it, the removed values are not cleared in memory.
 */
#define dcarr_resize(a, valtype, newsize) do{ \
	if ((newsize) > (a).len) {\
		dcarr_reserve((a), valtype, (newsize) - (a).len); \
	(a).len = (newsize); \
}while(0)


/*
 * Macros mainly for internal use
 */

/*
 * Convert external index to internal one
 */
#define dcarr_idx(a, i) \
	(((a).off + (i)) % (a).cap)

/*
 * Reserve space for at least n more elements
 */
#define dcarr_reserve(a, eltype, n) do{ \
	if ((a).len + (n) > (a).cap) { \
		unsigned int _cap = (a).cap; \
		while((a).len + (n) > (a).cap){ \
			(a).cap += ((a).cap > 3 ? ((a).cap) / 2 : 8); \
		} \
		(a).els = (eltype *)dcarr_realloc((a).els, (a).cap * sizeof(eltype)); \
		if (!(a).els) dcarr_oom(); \
		if ((a).off + (a).len > _cap) { \
			/* it warps around */ \
			memmove(&((a).els[(a).off + (a).cap - _cap]), \
			        &((a).els[(a).off]), \
			        sizeof(eltype) * ((a).cap - _cap)); \
			memset(&((a).els[(a).off]), 0, \
			       sizeof(eltype) * ((a).cap - _cap)); \
			(a).off += (a).cap - _cap; \
		} \
	} \
}while(0)

#endif
