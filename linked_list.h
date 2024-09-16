#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"


typedef struct iterator ioopm_list_iterator_t;
typedef struct list ioopm_list_t;

/**
 * @file linked_list.h
 * @author Emil Engelin and Erdem Garip
 * @date 3 Oct 2021
 * @brief Simple linked list that contains values.
 *
 * A standard linked list implementation that contains elem values. You yourself
 * must free the memory allocated to the linked list using ioopm_linked_list_destroy,
 * however, the linked list does not own the values given added to it.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gf5efa1610dfd73b58fef071f6c1d7a90
 */

/// @brief Creates a new empty list
/// @param eq function to compare equality with, if null: will compare for equality using integer values
/// @return an empty linked list
ioopm_list_t* ioopm_linked_list_create(ioopm_eq_function eq);

/// @brief Tear down the linked list and return all its memory
/// @param list the list to be destroyed
void ioopm_linked_list_destroy(ioopm_list_t* list);

/// @brief Get the first element of a linked list in O(1) time
/// @exception EPERM if list does not have a first element
/// @param list the linked list to get the first element of
/// @return the first element in list
elem_t ioopm_linked_list_first(const ioopm_list_t* list);

/// @brief Get the last element of a linked list in O(1) time
/// @exception EPERM if list does not have a last element
/// @param list the linked list to get the last element of
/// @return the last element in list
elem_t ioopm_linked_list_last(const ioopm_list_t* list);

/// @brief Insert at the end of a linked list in O(1) time
/// @param list the linked list that will be appended
/// @param value the value to be appended
void ioopm_linked_list_append(ioopm_list_t* list, elem_t value);

/// @brief Insert at the front of a linked list in O(1) time
/// @param list the linked list that will be prepended
/// @param value the value to be appended
void ioopm_linked_list_prepend(ioopm_list_t* list, elem_t value);

/// @brief Insert an element into a linked list in O(n) time.
/// The valid values of index are [0,n] for a list of n elements,
/// where 0 means before the first element and n means after
/// the last element.
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be appended
void ioopm_linked_list_insert(ioopm_list_t* list, size_t index, elem_t value);

/// @brief Remove an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @exception EPERM if index is not valid
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @param value the value to be appended
/// @return the value returned (*)
elem_t ioopm_linked_list_remove(ioopm_list_t* list, size_t index);

/// @brief Retrieve an element from a linked list in O(n) time.
/// The valid values of index are [0,n-1] for a list of n elements,
/// where 0 means the first element and n-1 means the last element.
/// @exception EPERM if index is not valid
/// @param list the linked list that will be extended
/// @param index the position in the list
/// @return the value at the given position
elem_t ioopm_linked_list_get(const ioopm_list_t* list, size_t index);

/// @brief Test if an element is in the list
/// @param list the linked list
/// @param element the element sought
/// @return true if element is in the list, else false
bool ioopm_linked_list_contains(const ioopm_list_t* list, elem_t element);

/// @brief Lookup the number of elements in the linked list in O(1) time
/// @param list the linked list
/// @return the number of elements in the list
size_t ioopm_linked_list_size(const ioopm_list_t* list);

/// @brief Test whether a list is empty or not
/// @param list the linked list
/// @return true if the number of elements int the list is 0, else false
bool ioopm_linked_list_is_empty(const ioopm_list_t* list);

/// @brief Remove all elements from a linked list
/// @param list the linked list
void ioopm_linked_list_clear(ioopm_list_t* list);

/// @brief Test if a supplied property holds for all elements in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested (function pointer)
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for all elements in the list, else false
bool ioopm_linked_list_all(const ioopm_list_t* list, ioopm_predicate prop, const void* extra);

/// @brief Test if a supplied property holds for any element in a list.
/// The function returns as soon as the return value can be determined.
/// @param list the linked list
/// @param prop the property to be tested
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of prop
/// @return true if prop holds for any elements in the list, else false
bool ioopm_linked_list_any(const ioopm_list_t* list, ioopm_predicate prop, const void* extra);

/// @brief Apply a supplied function to all elements in a list.
/// @param list the linked list
/// @param fun the function to be applied
/// @param extra an additional argument (may be NULL) that will be passed to all internal calls of fun
void ioopm_linked_list_apply_to_all(ioopm_list_t* list, ioopm_apply_function fun, const void* extra);

/// @brief Create an iterator for a given list
/// @param list the list to be iterated over
/// @return an iteration positioned at the start of list
ioopm_list_iterator_t* ioopm_list_iterator(const ioopm_list_t* list);
