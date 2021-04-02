/**
 * @file elist.h
 */

#ifndef _ELIST_H_
#define _ELIST_H_

#include <sys/types.h>

struct elist;

/**
 * Adds a new element to the list by copying its in-memory contents into the
 * list's element storage.
 *
 * @param list The list to copy the element into
 * @param element The element to copy
 *
 * @return Index of the element, or -1 on failure
 */
ssize_t elist_add(struct elist *list, void *element);

/**
 * Creates storage space for a new list element and returns a pointer to it.
 * Unlike elist_add, copying memory is not required so this function may be more
 * efficient when performance is critical.
 *
 * @param list The list to add a new element to
 *
 * @return Pointer to the new element (NOTE: it will not be initialized) or NULL
 * on failure.
 */
void *elist_add_new(struct elist *list);

/**
 * Retrieves the current capacity of the list.
 *
 * @param list The list to retrieve the capacity of
 *
 * @return capacity of  the list
 */
size_t elist_capacity(struct elist *list);

/**
 * Empties a list (but does not change its capacity).
 *
 * @param list The list to clear
 */
void elist_clear(struct elist *list);

/**
 * Empties a list (but does not change its capacity) and zeroes out all its
 * elements.
 * 
 * @param list The list to clear
 */
void elist_clear_mem(struct elist *list);

/**
 * Creates a new elist data structure. If the initial size is set to 0, the
 * default capacity will be used. 
 *
 * @param init_capacity The initial capacity of the list
 * @param element_sz Size of the elements that will be stored in the list (in bytes)
 *
 * @return A pointer to the newly-created list, or NULL on failure.
 */
struct elist *elist_create(size_t init_capacity, size_t element_sz);

/**
 * Destroys the specified list and frees any memory that was allocated to it.
 *
 * @param list The list to destroy
 */
void elist_destroy(struct elist *list);

/**
 * Retrieves the element at the given index.
 *
 * @param list The list to retrieve the element from
 * @param idx Index of the element to retrieve
 *
 * @return A pointer to the element, or NULL if the index is invalid.
 */
void *elist_get(struct elist *list, size_t idx);

/**
 * Retrieves the index of the first occurrence of a given element in the list.
 * This function uses memcmp() to determine element equality, so the supplied
 * element must be an exact copy (in terms of memory representation).
 *
 * @param list The list to search for the element
 * @param element An exact memory copy of the element to retrieve the index of
 *
 * @return The index, or -1 if the element was not found
 */
ssize_t elist_index_of(struct elist *list, void *element);

/**
 * Removes the element at the given index and shifts any subsequent element to
 * the left (or in other words, subtracts one from their indices).
 *
 * @param list The list to modify
 * @param idx Index of the element to remove
 *
 * @return zero on success, nonzero on failure
 */
int elist_remove(struct elist *list, size_t idx);

/**
 * Replaces an element at a particular index.
 *
 * @param list The list to modify
 * @param idx Index of the element to replace
 * @param element Element to place at 'idx' in the list
 *
 * @return zero on success, nonzero on failure
 */
int elist_set(struct elist *list, size_t idx, void *element);

/**
 * Increases or decreases the storage capacity of a list. When decreasing the
 * capacity, any elements beyond the new capacity are freed.
 *
 * @param list The list to change the capacity of
 * @param capacity The list's new capacity
 *
 * @return zero on success, nonzero on failure
 */
int elist_set_capacity(struct elist *list, size_t capacity);

/**
 * Retrieves the size of the elist (the actual number of elements stored).
 *
 * @param list The list to retrieve the size of
 *
 * @return The list size
 */
size_t elist_size(struct elist *list);

/**
 * Sorts an elist using the provided comparator function pointer.
 *
 * The comparator signature follows the same form as the comparators of
 * qsort(3):
 *
 * int (*compar)(const void *, const void *)
 *
 * The comparison function must return an integer less than, equal to, or
 * greater  than zero  if the first argument is considered to be respectively
 * less than, equal to, or greater than the second.  If two members compare as
 * equal, their order in the sorted array is undefined.
 *
 * @param list The list to sort
 * @param comparator Comparison function to use for the sort
 */
void elist_sort(struct elist *list, int (*comparator)(const void *, const void *));

#endif