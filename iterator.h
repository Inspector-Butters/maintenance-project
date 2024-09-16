#pragma once

#include <stdbool.h>
#include "common.h"
#include "linked_list.h"

/**
 * @file iterator.h
 * @author Emil Engelin and Erdem Garip
 * @date 3 Oct 2021
 * @brief Iterator for a linked list.
 *
 * Implementation of an iterator for a linked list. You must
 * destroy the iterator manually (using ioopm_iterator_destroy)
 * to free the memory it uses. The iterator does not take ownership
 * of any values passed back from the underlying list.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gf5efa1610dfd73b58fef071f6c1d7a90
 */

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if the next call to ioopm_iterator_next will return a valid element, otherwise false
bool ioopm_iterator_has_next(const ioopm_list_iterator_t* iter);

/// @brief Step the iterator forward one step
/// @exception EPERM if there is no next element
/// @param iter the iterator
/// @return the next element
elem_t ioopm_iterator_next(ioopm_list_iterator_t* iter);

/// @brief Remove the current element from the underlying list
/// @param iter the iterator
/// @return the removed element
elem_t ioopm_iterator_remove(ioopm_list_iterator_t* iter);

/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_iterator_insert(ioopm_list_iterator_t* iter, elem_t element);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t* iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_iterator_current(const ioopm_list_iterator_t* iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t* iter);
