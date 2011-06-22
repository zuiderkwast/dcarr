/*********************************************************************
 * dcarr.h - A generic dynamic array for C, implemented as macros.   *
 *                                                                   *
 * 2011 June 22                                                      *
 * The author disclaims copyright to this source code.               *
 *                                                                   *
 * The array uses a circular buffer, thus it has amortized constant  *
 * time insertion/removal at both ends. This implementation is also  *
 * known as an array deque.                                          *
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
 * Initializes an array and sets the initial capacity to zero.
 *
 * This does not allocate anything. The first allocation occurs when the
 * first element is inserted.
 *
 * This is identical to clearing the array using memset.
 */
#define dcarr_init(a) do{\
	(a).els = NULL; \
	(a).cap = (a).off = (a).len = 0; \
}while(0)

/*
 * Frees all allocated memory. To use the array again after this, it must
 * be re-initialized using dcarr_init.
 */
#define dcarr_destroy(a) do{ \
	dcarr_free((a).els); \
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
#define dcarr_unshift(a, elemtype, value) do{ \
	dcarr_reserve((a), elemtype, 1); \
	(a).off = dcarr_idx((a), (a).cap - 1); \
	(a).els[(a).off] = (value); \
	(a).len++; \
}while(0)

/*
 * Remove an element at the beginning and return its value
 */
#define dcarr_shift(a, elemtype, value) do{ \
	(value) = (a).els[(a).off]; \
	(a).off = dcarr_idx((a), 1); \
	(a).len--; \
	dcarr_reduce_size((a), elemtype); \
}while(0)

/*
 * Insert an element at the end
 */
#define dcarr_push(a, elemtype, value) do{ \
	dcarr_reserve((a), elemtype, 1); \
	(a).els[dcarr_idx((a), (a).len++)] = (value); \
}while(0)

/*
 * Remove an element at the end and return its value, O(1)
 */
#define dcarr_pop(a, elemtype, value) do{\
	(value) = (a).els[dcarr_idx((a), --(a).len)]; \
	dcarr_reduce_size((a), elemtype); \
}while(0)

/*
 * Insert at an arbitrary position, O(n)
 */
#define dcarr_insert(a, i, elemtype, value) do{ \
	dcarr_reserve((a), elemtype, 1); \
	if ((i) < (a).len) { \
		/* need to move elements */ \
		if (dcarr_idx(a, i) > (a).off && (a).off != 0) { \
			/* move the beginning of the list backwards */ \
			memmove(&((a).els[(a).off-1]), \
			        &((a).els[(a).off]), \
			        sizeof(elemtype) * (dcarr_idx(a, i) - (a).off)); \
			(a).off--; \
		} else { \
			/* move the end of the list forward */ \
			memmove(&((a).els[dcarr_idx(a, i)+1]), \
			        &((a).els[dcarr_idx(a, i)]), \
			        sizeof(elemtype) * ((a).len - i)); \
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
#define dcarr_resize(a, elemtype, newsize) do{ \
	if ((newsize) > (a).len) {\
		dcarr_reserve((a), elemtype, (newsize) - (a).len); \
	(a).len = (newsize); \
	dcarr_reduce_size(a, elemtype); \
}while(0)

/*
 * Sort using a compare function of the type which qsort expects
 */
#define dcarr_sort(a, elemtype, cmp) do{\
	if ((a).off + (a).len > (a).cap) { \
		/* it warps around. just move the parts together. */ \
		memmove(&((a).els[(a).off + (a).len - (a).cap]), \
		        &((a).els[(a).off]), \
		        sizeof(elemtype) * ((a).cap - (a).off)); \
		(a).off = 0; \
	}\
	qsort(&((a).els[(a).off]), (a).len, sizeof(elemtype), (cmp)); \
}while(0)

/*
 * Convert external index to internal one. (Used internally.)
 *
 * i % cap == i & (cap - 1), since cap always is a power of 2.
 */
#define dcarr_idx(a, i) \
	(((a).off + (i)) & ((a).cap - 1))

/*
 * Reserve space for at least n more elements.
 */
#define dcarr_reserve(a, eltype, n) do{ \
	if ((a).len + (n) > (a).cap) { \
		unsigned int _cap = (a).cap; \
		/* calulate and set new capacity */ \
		do{ \
			(a).cap = (a).cap >= 8 ? (a).cap << 1 : 8; \
		}while((a).len + (n) > (a).cap); \
		/* allocate more mem */ \
		(a).els = (eltype *)dcarr_realloc((a).els, (a).cap * sizeof(eltype)); \
		if (!(a).els) dcarr_oom(); \
		/* adjust content to the increased capacity */ \
		if ((a).off + (a).len > _cap) { \
			/* it warps around. make it warp around the new boundary. */ \
			memmove(&((a).els[(a).off + (a).cap - _cap]), \
			        &((a).els[(a).off]), \
			        sizeof(eltype) * ((a).cap - _cap)); \
			memset(&((a).els[(a).off]), 0, \
			       sizeof(eltype) * ((a).cap - _cap)); \
			(a).off += (a).cap - _cap; \
		} \
	} \
}while(0)

/*
 * Reduces the capacity somewhat if less than 25% full
 */
#define dcarr_reduce_size(a, eltype) do{\
	if ((a).len << 2 <= (a).cap && (a).cap > 8) {\
		/* it is down to 1/4. reduce cap to len * 2 so it is half full */ \
		unsigned int _cap = (a).cap; \
		/* calulate and set new capacity */ \
		do{ \
			(a).cap = (a).cap >> 1; \
		}while((a).len << 2 <= (a).cap && (a).cap > 8); \
		/* adjust content to decreased capacity */ \
		if ((a).off >= (a).cap) {\
			/* the whole content is outside, but in one piece */ \
			memcpy(&((a).els[0]), \
			       &((a).els[(a).off]), \
			       sizeof(eltype) * (a).len); \
			(a).off = 0; \
		} \
		else if ((a).off + (a).len >= _cap) { \
			/* it warpped around already. adjust to new boundary. */ \
			memmove(&((a).els[(a).off + (a).cap - _cap]), \
			        &((a).els[(a).off]), \
			        sizeof(eltype) * ((a).cap - _cap)); \
			(a).off += (a).cap - _cap; \
		} \
		else if ((a).off + (a).len > (a).cap) { \
			/* it overflows the new cap. make it warp. */ \
			memcpy(&((a).els[0]), \
			       &((a).els[(a).cap]), \
			       sizeof(eltype) * ((a).off + (a).len - (a).cap)); \
		} \
		/* free the unused part */ \
		(a).els = (eltype *)dcarr_realloc((a).els, (a).cap * sizeof(eltype)); \
	} \
}while(0)


#endif
