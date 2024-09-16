#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "common.h"
#include "hash_table.h"
#include "linked_list.h"
#include "iterator.h"

#define DEFAULT_INITIAL_CAPACITY 17
#define DEFAULT_LOAD_FACTOR 0.75
#define CAPACITY_GROWTH_FACTOR 2

extern int errno;

typedef struct entry entry_t;

struct entry
{
    elem_t key;    // holds the key
    elem_t value;  // holds the value
    entry_t* next; // points to the next entry (possibly NULL)
};

struct hash_table
{
    size_t count;                       // the amount of entries (excluding dummy entries) present in the hash table
    size_t capacity;                    // the capacity (number of buckets)
    double load_factor;                 // the load factor which will be used when calculating whether or not to do a resize
    entry_t* buckets;                   // ptr to linked entries of entries
    ioopm_eq_function key_eq;           // aux function that compares key equality
    ioopm_eq_function value_eq;         // aux function that compares value equality
    ioopm_hash_table_hash_key hash_key; // aux function that will hash a key from the hash table
};

typedef struct lookup lookup_t;

/// Used in has_key and has_value functions.
struct lookup
{
    const ioopm_hash_table_t* ht; // the hash table, so that we can access aux functions
    elem_t element;               // the element to look for
};

/// @brief gets all the entries in ht
/// @param ht the hash table to get all the entries from
/// @return list of all entries
static ioopm_list_t* hash_table_entries(const ioopm_hash_table_t* ht);

static int default_hash_key(elem_t key)
{
    return key.i;
}

static entry_t* entry_create(elem_t key, elem_t value, entry_t* next)
{
    entry_t* entry = calloc(1, sizeof(entry_t));
    *entry = (entry_t){ .key = key, .value = value, .next = next };
    return entry;
}

static void entry_destroy(entry_t* entry)
{
    free(entry);
}

static bool should_resize(const ioopm_hash_table_t* ht)
{
    return ht->count >= ht->capacity * ht->load_factor;
}

static void resize(ioopm_hash_table_t* ht, size_t new_capacity)
{
    ioopm_list_t*          entries = NULL;
    ioopm_list_iterator_t* iter    = NULL;

    if (ht->buckets != NULL)
    {
        entries = hash_table_entries(ht);
        iter    = ioopm_list_iterator(entries);

        // Free previous dummy entries
        free(ht->buckets);
    }

    ht->capacity = new_capacity;
    ht->buckets  = calloc(ht->capacity, sizeof(entry_t));
    
    // Create all dummy entries
    for (size_t i = 0; i < ht->capacity; i++)
    {
        entry_t* entry = entry_create(ptr_elem(NULL), ptr_elem(NULL), NULL);
        ht->buckets[i] = *entry;
        entry_destroy(entry);
    }

    if (iter != NULL)
    {
        // Reset count as it's going to increase
        // with each call to ioopm_hash_table_insert
        ht->count = 0;

        while (ioopm_iterator_has_next(iter))
        {
            entry_t* entry = ioopm_iterator_next(iter).p;
            ioopm_hash_table_insert(ht, entry->key, entry->value);
            entry_destroy(entry);
        }

        ioopm_linked_list_destroy(entries);
        ioopm_iterator_destroy(iter);
    }
}

ioopm_hash_table_t* ioopm_hash_table_create(ioopm_eq_function key_eq, ioopm_eq_function value_eq, ioopm_hash_table_hash_key hash_key)
{
    return ioopm_hash_table_create_complex(key_eq, value_eq, hash_key, DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);
}

ioopm_hash_table_t* ioopm_hash_table_create_complex(ioopm_eq_function key_eq, ioopm_eq_function value_eq, ioopm_hash_table_hash_key hash_key, size_t initial_capacity, double load_factor)
{
    ioopm_hash_table_t* ht = calloc(1, sizeof(ioopm_hash_table_t));
    ht->load_factor = load_factor;
    resize(ht, initial_capacity);

    ht->key_eq   = key_eq != NULL ? key_eq : ioopm_compare_int_elems;
    ht->value_eq = value_eq != NULL ? value_eq : ioopm_compare_int_elems;
    ht->hash_key = hash_key != NULL ? hash_key : default_hash_key;

    return ht;
}

void ioopm_hash_table_destroy(ioopm_hash_table_t* ht)
{
    ioopm_hash_table_clear(ht);
    free(ht->buckets);
    free(ht);
}

static entry_t* get_bucket_from_key(const ioopm_hash_table_t* ht, elem_t key)
{
    ioopm_hash_table_t* ht_non_const = (ioopm_hash_table_t*)ht;
    entry_t* buckets = (ht_non_const)->buckets;
    int hash = ht->hash_key(key);
    return &(buckets[hash % ht->capacity]);
}

static entry_t* find_previous_entry_for_key(const ioopm_hash_table_t* ht, entry_t* current, elem_t key)
{
    int hash = ht->hash_key(key);
    while (current->next != NULL && ht->hash_key(current->next->key) < hash)
    {
        current = current->next;
    }
    return current;
}

void ioopm_hash_table_insert(ioopm_hash_table_t* ht, elem_t key, elem_t value)
{
    /// Search for an existing entry for a key
    entry_t* entry = find_previous_entry_for_key(ht, get_bucket_from_key(ht, key), key);
    entry_t* next  = entry->next;

    /// Check if the next entry should be updated or not
    if (next != NULL && ht->key_eq(next->key, key))
    {
        next->value = value;
    }
    else
    {
        entry->next = entry_create(key, value, next);
        ht->count++;

        if (should_resize(ht))
        {
            resize(ht, ht->capacity * CAPACITY_GROWTH_FACTOR);
        }
    }
}

bool ioopm_hash_table_lookup(const ioopm_hash_table_t* ht, elem_t key, elem_t* result)
{
    /// Find the previous entry for key
    entry_t* entry = find_previous_entry_for_key(ht, get_bucket_from_key(ht, key), key);
    entry_t* next  = entry->next;

    bool key_found = next != NULL && ht->key_eq(next->key, key);
    if (key_found)
    {
        /// If entry was found, place its value at result
        if (result != NULL)
        {
            *result = next->value;
        }
        else
        {
            errno = EPERM;
        }
    }
    return key_found;
}

bool ioopm_hash_table_remove(ioopm_hash_table_t* ht, elem_t key, elem_t* result)
{
    entry_t* entry = find_previous_entry_for_key(ht, get_bucket_from_key(ht, key), key);
    entry_t* next  = entry->next;

    if (next != NULL && ht->key_eq(next->key, key))
    {
        entry->next = entry->next->next;
        if (result != NULL)
        {
            *result = next->value;
        }
        else
        {
            errno = EPERM;
        }
        entry_destroy(next);
        ht->count--;
        return true;
    }
    return false;
}

size_t ioopm_hash_table_size(const ioopm_hash_table_t* ht)
{
    return ht->count;
}

bool ioopm_hash_table_is_empty(const ioopm_hash_table_t* ht)
{
    return ht->count == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t* ht)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* bucket  = &ht->buckets[i];
        entry_t* current = bucket->next;
        
        bucket->next = NULL; // Remove old pointer

        while (current)
        {
            entry_t* tmp = current;
            current = current->next;
            entry_destroy(tmp);
        }
    }
    ht->count = 0;
}

static ioopm_list_t* hash_table_entries(const ioopm_hash_table_t* ht)
{
    ioopm_list_t* list = ioopm_linked_list_create(ioopm_compare_ptr_elems);

    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            ioopm_linked_list_append(list, ptr_elem(current));
            current = current->next;
        }
    }

    return list;
}

ioopm_list_t* ioopm_hash_table_keys(const ioopm_hash_table_t* ht)
{
    ioopm_list_t* list = ioopm_linked_list_create(ht->key_eq);
    
    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            ioopm_linked_list_append(list, current->key);
            current = current->next;
        }
    }

    return list;
}

ioopm_list_t* ioopm_hash_table_values(const ioopm_hash_table_t* ht)
{
    ioopm_list_t* list = ioopm_linked_list_create(ht->value_eq);

    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            ioopm_linked_list_append(list, current->value);
            current = current->next;
        }
    }

    return list;
}

static bool key_equal(elem_t key, elem_t value_ignored, void* lookup_ptr)
{
    lookup_t* lookup = (lookup_t*)lookup_ptr;
    return lookup->ht->key_eq(key, lookup->element);
}

bool ioopm_hash_table_has_key(const ioopm_hash_table_t* ht, elem_t key)
{
    lookup_t lookup = (lookup_t){ .ht = ht, .element = key };
    return ioopm_hash_table_any(ht, (ioopm_predicate)key_equal, &lookup);
}

static bool value_equal(elem_t key_ignored, elem_t value, void* lookup_ptr)
{
    lookup_t* lookup = (lookup_t*)lookup_ptr;
    return lookup->ht->value_eq(value, lookup->element);
}

bool ioopm_hash_table_has_value(const ioopm_hash_table_t* ht, elem_t value)
{
    lookup_t lookup = (lookup_t){ .ht = ht, .element = value };
    return ioopm_hash_table_any(ht, (ioopm_predicate)value_equal, &lookup);
}

bool ioopm_hash_table_all(const ioopm_hash_table_t* ht, ioopm_predicate pred, const void* arg)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            if (!pred(current->key, current->value, (void*)arg))
            {
                return false;
            }

            current = current->next;
        }
    }
    return true;
}

bool ioopm_hash_table_any(const ioopm_hash_table_t* ht, ioopm_predicate pred, const void* arg)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            if (pred(current->key, current->value, (void*)arg))
            {
                return true;
            }

            current = current->next;
        }
    }
    return false;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t* ht, ioopm_apply_function apply_fun, const void* arg)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        entry_t* current = (&ht->buckets[i])->next;

        while (current)
        {
            apply_fun(current->key, &current->value, (void*)arg);
            current = current->next;
        }
    }
}
