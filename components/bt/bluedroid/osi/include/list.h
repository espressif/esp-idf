#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stddef.h>
struct list_node_t;
typedef struct list_node_t list_node_t;

struct list_t;
typedef struct list_t list_t;

typedef void (*list_free_cb)(void *data);
typedef bool (*list_iter_cb)(void *data);

// Returns a new, empty list. Returns NULL if not enough memory could be allocated
// for the list structure. The returned list must be freed with |list_free|. The
// |callback| specifies a function to be called whenever a list element is removed
// from the list. It can be used to release resources held by the list element, e.g.
// memory or file descriptor. |callback| may be NULL if no cleanup is necessary on
// element removal.
list_t *list_new(list_free_cb callback);


list_node_t *list_free_node(list_t *list, list_node_t *node);
//list_node_t *list_free_node(list_t *list, list_node_t *node);
// Frees the list. This function accepts NULL as an argument, in which case it
// behaves like a no-op.
void list_free(list_t *list);

// Returns true if |list| is empty (has no elements), false otherwise.
// |list| may not be NULL.
bool list_is_empty(const list_t *list);

// Returns true if the list contains |data|, false otherwise.
// |list| may not be NULL.
bool list_contains(const list_t *list, const void *data);

// Returns the length of the |list|. |list| may not be NULL.
size_t list_length(const list_t *list);

// Returns the first element in the list without removing it. |list| may not
// be NULL or empty.
void *list_front(const list_t *list);

// Returns the last element in the list without removing it. |list| may not
// be NULL or empty.
//void *list_back(const list_t *list);

// Inserts |data| after |prev_node| in |list|. |data|, |list|, and |prev_node|
// may not be NULL. This function does not make a copy of |data| so the pointer
// must remain valid at least until the element is removed from the list or the
// list is freed. Returns true if |data| could be inserted, false otherwise
// (e.g. out of memory).
bool list_insert_after(list_t *list, list_node_t *prev_node, void *data);

// Inserts |data| at the beginning of |list|. Neither |data| nor |list| may be NULL.
// This function does not make a copy of |data| so the pointer must remain valid
// at least until the element is removed from the list or the list is freed.
// Returns true if |data| could be inserted, false otherwise (e.g. out of memory).
bool list_prepend(list_t *list, void *data);

// Inserts |data| at the end of |list|. Neither |data| nor |list| may be NULL.
// This function does not make a copy of |data| so the pointer must remain valid
// at least until the element is removed from the list or the list is freed.
// Returns true if |data| could be inserted, false otherwise (e.g. out of memory).
bool list_append(list_t *list, void *data);

// Removes |data| from the list. Neither |list| nor |data| may be NULL. If |data|
// is inserted multiple times in the list, this function will only remove the first
// instance. If a free function was specified in |list_new|, it will be called back
// with |data|. This function returns true if |data| was found in the list and removed,
// false otherwise.
//list_node_t list_remove_node(list_t *list, list_node_t *prev_node, list_node_t *node);
//list_node_t list_insert_node(list_t *list, list_node_t *prev_node, list_node_t *node);

bool list_remove(list_t *list, void *data);

// Removes all elements in the list. Calling this function will return the list to the
// same state it was in after |list_new|. |list| may not be NULL.
void list_clear(list_t *list);

// Iterates through the entire |list| and calls |callback| for each data element.
// If the list is empty, |callback| will never be called. It is safe to mutate the
// list inside the callback. If an element is added before the node being visited,
// there will be no callback for the newly-inserted node. Neither |list| nor
// |callback| may be NULL.
void list_foreach(const list_t *list, list_iter_cb callback);

// Returns an iterator to the first element in |list|. |list| may not be NULL.
// The returned iterator is valid as long as it does not equal the value returned
// by |list_end|.
list_node_t *list_begin(const list_t *list);

// Returns an iterator that points past the end of the list. In other words,
// this function returns the value of an invalid iterator for the given list.
// When an iterator has the same value as what's returned by this function, you
// may no longer call |list_next| with the iterator. |list| may not be NULL.
list_node_t *list_end(const list_t *list);

// Given a valid iterator |node|, this function returns the next value for the
// iterator. If the returned value equals the value returned by |list_end|, the
// iterator has reached the end of the list and may no longer be used for any
// purpose.
list_node_t *list_next(const list_node_t *node);

// Returns the value stored at the location pointed to by the iterator |node|.
// |node| must not equal the value returned by |list_end|.
void *list_node(const list_node_t *node);

#endif /* _LIST_H_ */
