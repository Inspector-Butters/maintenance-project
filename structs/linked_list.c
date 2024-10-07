#include <stdlib.h>
#include <errno.h>
#include "linked_list.h"
#include "iterator.h"

extern int errno;

typedef struct lookup lookup_t;
typedef struct node node_t;

struct node
{
    elem_t value;     // value that the node contains
    node_t* previous; // the previous node, can be NULL
    node_t* next;     // the next node, can be NULL
};

struct list
{
    size_t count;         // amount of elements in list
    node_t* first;        // the first element, can be NULL
    node_t* last;         // the last element, can be NULL
    ioopm_eq_function eq; // an equality function used to compare for equality between values
};

struct iterator
{
    struct list* list; // the underlying list
    node_t* current;   // the current node, can be NULL
    size_t index;      // what index the node is at
};

/// Used in has_value.
struct lookup
{
    const ioopm_list_t* list; // list pointer so that we can use equality function
    elem_t element;           // element to compare with
};

static bool default_eq(elem_t a, elem_t b)
{
    return a.i == b.i;
}

ioopm_list_t* ioopm_linked_list_create(ioopm_eq_function eq)
{
    ioopm_list_t* list = calloc(1, sizeof(ioopm_list_t));
    list->eq = eq != NULL ? eq : default_eq;
    return list;
}

static node_t* node_create(ioopm_list_t* list, elem_t value, node_t* previous, node_t* next)
{
    node_t* node = calloc(1, sizeof(node_t));
    *node = (node_t) { .value = value, .previous = previous, .next = next };

    if (list->first == NULL)
    {
        list->first = node;
    }
    if (list->last == NULL)
    {
        list->last = node;
    }
    if (previous != NULL)
    {
        previous->next = node;
    }
    if (next != NULL)
    {
        next->previous = node;
    }

    return node;
}

static void node_destroy(node_t* node)
{
    free(node);
}

void ioopm_linked_list_destroy(ioopm_list_t* list)
{
    ioopm_linked_list_clear(list);
    free(list);
}

elem_t ioopm_linked_list_first(const ioopm_list_t* list)
{
    if (list->first != NULL)
    {
        return list->first->value;
    }
    errno = EPERM;
    return empty_elem();
}

elem_t ioopm_linked_list_last(const ioopm_list_t* list)
{
    if (list->last != NULL)
    {
        return list->last->value;
    }
    errno = EPERM;
    return empty_elem();
}

void ioopm_linked_list_append(ioopm_list_t* list, elem_t value)
{
    list->last = node_create(list, value, list->last, NULL);
    list->count++;
}

void ioopm_linked_list_prepend(ioopm_list_t* list, elem_t value)
{
    list->first = node_create(list, value, NULL, list->first);
    list->count++;
}

void ioopm_linked_list_insert(ioopm_list_t* list, size_t index, elem_t value)
{
    node_t* previous = NULL;
    node_t* next     = list->first;
    for (size_t i = 0; i < index; i++)
    {
        previous = next;
        if (next)
        {
            next = next->next;
        }
    }

    node_t* node = node_create(list, value, previous, next);

    if (previous == NULL)
    {
        list->first = node;
    }
    if (next == NULL)
    {
        list->last = node;
    }

    list->count++;
}

elem_t ioopm_linked_list_remove(ioopm_list_t* list, size_t index)
{
    node_t* current = list->first;
    for (size_t i = 0; i < index && current; i++, current = current->next);

    if (current != NULL)
    {
        if (current->previous != NULL)
        {
            current->previous->next = current->next;
        }
        if (current->next != NULL)
        {
            current->next->previous = current->previous;
        }

        if (list->first == current)
        {
            list->first = current->next;
        }
        if (list->last == current)
        {
            list->last = current->previous;
        }

        list->count--;

        elem_t value = current->value;
        node_destroy(current);

        return value;
    }

    errno = EPERM;
    return empty_elem();
}

elem_t ioopm_linked_list_get(const ioopm_list_t* list, size_t index)
{
    node_t* current = list->first;
    for (size_t i = 0; i < index && current; i++, current = current->next);

    if (current != NULL)
    {
        return current->value;
    }
    
    errno = EPERM;
    return empty_elem();
}

static bool has_value(elem_t key_ignored, elem_t value, void* lookup_ptr)
{
    lookup_t* lookup = (lookup_t*)(lookup_ptr);
    return lookup->list->eq(value, lookup->element);
}

bool ioopm_linked_list_contains(const ioopm_list_t* list, elem_t element)
{
    lookup_t lookup = (lookup_t){ .list = list, .element = element };
    return ioopm_linked_list_any(list, has_value, &lookup);
}

size_t ioopm_linked_list_size(const ioopm_list_t* list)
{
    return list->count;
}

bool ioopm_linked_list_is_empty(const ioopm_list_t* list)
{
    return list->count == 0; 
}

void ioopm_linked_list_clear(ioopm_list_t* list)
{
    node_t* current = list->first;
    while (current)
    {
        node_t* temporary = current;
        current = current->next;
        node_destroy(temporary);
    }

    list->first = NULL;
    list->last  = NULL;
    list->count = 0;
}

bool ioopm_linked_list_all(const ioopm_list_t* list, ioopm_predicate prop, const void* extra)
{
    node_t* current = list->first;
    size_t i = 0;
    while (current)
    {
        if (!prop(size_elem(i++), current->value, (void*)extra))
        {
            return false;
        }
        current = current->next;
    }
    return true;
}

bool ioopm_linked_list_any(const ioopm_list_t* list, ioopm_predicate prop, const void* extra)
{
    node_t* current = list->first;
    size_t i = 0;
    while (current)
    {
        if (prop(size_elem(i++), current->value, (void*)extra))
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

void ioopm_linked_list_apply_to_all(ioopm_list_t* list, ioopm_apply_function fun, const void* extra)
{
    node_t* current = list->first;
    size_t i = 0;
    while (current)
    {
        fun(size_elem(i++), &current->value, (void*)extra);
        current = current->next;
    }
}

ioopm_list_iterator_t* ioopm_list_iterator(const ioopm_list_t* list)
{
    ioopm_list_iterator_t* iter = calloc(1, sizeof(ioopm_list_iterator_t));
    *iter = (ioopm_list_iterator_t){ .list = (ioopm_list_t*)list, .current = list->first, .index = 0 };
    return iter;
}


bool ioopm_iterator_has_next(const ioopm_list_iterator_t* iter)
{
    return iter->current != NULL;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t* iter)
{
    node_t* current = iter->current;
    if (current == NULL)
    {
        errno = EPERM;
        return empty_elem();
    }
   
    iter->current = iter->current->next;
    iter->index++;
    return current->value;
}

elem_t ioopm_iterator_remove(ioopm_list_iterator_t* iter)
{
    node_t* next = iter->current->next;
    elem_t removed_value = ioopm_linked_list_remove(iter->list, iter->index);
    iter->current = next;
    return removed_value;
}

void ioopm_iterator_insert(ioopm_list_iterator_t* iter, elem_t element)
{
    ioopm_linked_list_insert(iter->list, iter->index, element);
    iter->current = iter->list->first;
    for (int i = 0; i < iter->index; i++) iter->current = iter->current->next;
}

void ioopm_iterator_reset(ioopm_list_iterator_t* iter)
{
    iter->current = iter->list->first;
    iter->index = 0;
}

elem_t ioopm_iterator_current(const ioopm_list_iterator_t* iter)
{
    return iter->current->value;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t* iter)
{
    free(iter);
}
