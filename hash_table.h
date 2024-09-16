#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "linked_list.h"

/**
 * @file hash_table.h
 * @author Emil Engelin and Erdem Garip
 * @date 3 Oct 2021
 * @brief Simple hash table that maps keys to values.
 *
 * A standard hash table implementation of elem-to-elem key-value pairs. You
 * yourself need to free the data passed to the hash table (ie. the hash table
 * doesn't own the data passed to it), but also the hash table itself
 * (using ioopm_hash_table_destroy).
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gf5efa1610dfd73b58fef071f6c1d7a90
 */

typedef struct hash_table ioopm_hash_table_t;

typedef int(*ioopm_hash_table_hash_key)(elem_t key);

/// @brief Create a new hash table, a default initial capacity and load factor will be used
/// @param key_eq function to compare key equality with, if null: will default to comparing integer values
/// @param value_eq function to compare value equality with, if null: will default to comparing integer values
/// @param hash_key function to extract a hash from a key, if null: will default to hashing integer values
/// @return A new empty hash table
ioopm_hash_table_t* ioopm_hash_table_create(ioopm_eq_function key_eq, ioopm_eq_function value_eq, ioopm_hash_table_hash_key hash_key);

/// @brief Create a new hash table of a certain capacity amd load factor
/// @param key_eq function to compare key equality with, if null: will default to comparing integer values
/// @param value_eq function to compare value equality with, if null: will default to comparing integer values
/// @param hash_key function to extract a hash from a key, if null: will default to hashing integer values
/// @param initial_capacity initial capacity of hash table
/// @param load_factor load factor for the hash table
/// @return A new empty hash table
ioopm_hash_table_t* ioopm_hash_table_create_complex(ioopm_eq_function key_eq, ioopm_eq_function value_eq, ioopm_hash_table_hash_key hash_key, size_t initial_capacity, double load_factor);

/// @brief Delete a hash table and free its memory
/// @param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t* ht);

/// @brief Add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t* ht, elem_t key, elem_t value);

/// @brief Lookup value for key in hash table ht
/// @exception EPERM if result is NULL
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result pointer to where to place result, this can't be NULL
/// @return true if an entry with key key exists, otherwise false
bool ioopm_hash_table_lookup(const ioopm_hash_table_t* ht, elem_t key, elem_t* result);

/// @brief Remove any mapping from key to a value
/// @exception EPERM if result is NULL
/// @param ht hash table operated upon
/// @param key key to remove
/// @param result pointer to where to place result, this can't be NULL
/// @return true if an entry with key key exists, otherwise false
bool ioopm_hash_table_remove(ioopm_hash_table_t* ht, elem_t key, elem_t* result);

/// @brief Returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(const ioopm_hash_table_t* ht);

/// @brief Checks if the hash table is empty
/// @param ht hash table operated upon
/// @return true if size == 0, else false
bool ioopm_hash_table_is_empty(const ioopm_hash_table_t* ht);

/// @brief Clear all the entries in a hash table
/// @param ht hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t* ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param ht hash table operated upon
/// @return a list of keys for hash table ht, must be manually freed
ioopm_list_t* ioopm_hash_table_keys(const ioopm_hash_table_t* ht);

/// @brief Return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param ht hash table operated upon
/// @return a list of values for hash table ht, must be manually freed
ioopm_list_t* ioopm_hash_table_values(const ioopm_hash_table_t* ht);

/// @brief Check if a hash table has an entry with a given key
/// @param ht hash table operated upon
/// @param key the key sought
/// @return true if key is found in ht, otherwise false
bool ioopm_hash_table_has_key(const ioopm_hash_table_t* ht, elem_t key);

/// @brief Check if a hash table has an entry with a given value
/// @param ht hash table operated upon
/// @param value the value sought
/// @return true if value is found in ht, otherwise false
bool ioopm_hash_table_has_value(const ioopm_hash_table_t* ht, elem_t value);

/// @brief Check if a predicate is satisfied by all entries in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred (may be NULL)
/// @return true if predicate is satisfied by all entries in ht, otherwise false
bool ioopm_hash_table_all(const ioopm_hash_table_t* ht, ioopm_predicate pred, const void* arg);

/// @brief Check if a predicate is satisfied by any entry in a hash table
/// @param ht hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred (may be NULL)
/// @return true if predicate is satisfied by any entries in ht, otherwise false
bool ioopm_hash_table_any(const ioopm_hash_table_t* ht, ioopm_predicate pred, const void* arg);

/// @brief Apply a function to all entries in a hash table
/// @param ht hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun (may be NULL)
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t* ht, ioopm_apply_function apply_fun, const void* arg);
