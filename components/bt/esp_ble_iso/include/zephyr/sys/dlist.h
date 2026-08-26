/*
 * SPDX-FileCopyrightText: 2013-2015 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup doubly-linked-list_apis Doubly-linked list
 * @ingroup datastructure_apis
 *
 * @brief Doubly-linked list implementation
 *
 * Doubly-linked list implementation using inline macros/functions.
 * This API is not thread safe, and thus if a list is used across threads,
 * calls to functions must be protected with synchronization primitives.
 *
 * The lists are expected to be initialized such that both the head and tail
 * pointers point to the list itself.  Initializing the lists in such a fashion
 * simplifies the adding and removing of nodes to/from the list.
 *
 * @{
 */

#ifndef ZEPHYR_INCLUDE_SYS_DLIST_H_
#define ZEPHYR_INCLUDE_SYS_DLIST_H_

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _dnode {
    union {
        struct _dnode *head; /* ptr to head of list (sys_dlist_t) */
        struct _dnode *next; /* ptr to next node    (sys_dnode_t) */
    };
    union {
        struct _dnode *tail; /* ptr to tail of list (sys_dlist_t) */
        struct _dnode *prev; /* ptr to previous node (sys_dnode_t) */
    };
};

/**
 * @brief Doubly-linked list structure.
 */
typedef struct _dnode sys_dlist_t;
/**
 * @brief Doubly-linked list node structure.
 */
typedef struct _dnode sys_dnode_t;

/**
 * @brief Provide the primitive to iterate on a list
 * Note: the loop is unsafe and thus __dn should not be removed
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_NODE(l, n) {
 *         <user code>
 *     }
 *
 * This and other SYS_DLIST_*() macros are not thread safe.
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __dn A sys_dnode_t pointer to peek each node of the list
 */
#define SYS_DLIST_FOR_EACH_NODE(__dl, __dn)             \
    for (__dn = sys_dlist_peek_head(__dl); __dn != NULL;        \
         __dn = sys_dlist_peek_next(__dl, __dn))

/**
 * @brief Provide the primitive to iterate on a list, from a node in the list
 * Note: the loop is unsafe and thus __dn should not be removed
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_ITERATE_FROM_NODE(l, n) {
 *         <user code>
 *     }
 *
 * Like SYS_DLIST_FOR_EACH_NODE(), but __dn already contains a node in the list
 * where to start searching for the next entry from. If NULL, it starts from
 * the head.
 *
 * This and other SYS_DLIST_*() macros are not thread safe.
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __dn A sys_dnode_t pointer to peek each node of the list;
 *             it contains the starting node, or NULL to start from the head
 */
#define SYS_DLIST_ITERATE_FROM_NODE(__dl, __dn) \
    for (__dn = __dn ? sys_dlist_peek_next_no_check(__dl, __dn) \
             : sys_dlist_peek_head(__dl); \
         __dn != NULL; \
         __dn = sys_dlist_peek_next(__dl, __dn))

/**
 * @brief Provide the primitive to safely iterate on a list
 * Note: __dn can be removed, it will not break the loop.
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_NODE_SAFE(l, n, s) {
 *         <user code>
 *     }
 *
 * This and other SYS_DLIST_*() macros are not thread safe.
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __dn A sys_dnode_t pointer to peek each node of the list
 * @param __dns A sys_dnode_t pointer for the loop to run safely
 */
#define SYS_DLIST_FOR_EACH_NODE_SAFE(__dl, __dn, __dns)         \
    for ((__dn) = sys_dlist_peek_head(__dl),            \
             (__dns) = sys_dlist_peek_next((__dl), (__dn)); \
         (__dn) != NULL; (__dn) = (__dns),              \
             (__dns) = sys_dlist_peek_next(__dl, __dn))

/**
 * @brief Provide the primitive to resolve the container of a list node
 * Note: it is safe to use with NULL pointer nodes
 *
 * @param __dn A pointer on a sys_dnode_t to get its container
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_CONTAINER(__dn, __cn, __n) \
    (((__dn) != NULL) ? CONTAINER_OF(__dn, __typeof__(*(__cn)), __n) : NULL)
/**
 * @brief Provide the primitive to peek container of the list head
 *
 * @param __dl A pointer on a sys_dlist_t to peek
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n) \
    SYS_DLIST_CONTAINER(sys_dlist_peek_head(__dl), __cn, __n)

/**
 * @brief Provide the primitive to peek the next container
 *
 * @param __dl A pointer on a sys_dlist_t to peek
 * @param __cn Container struct type pointer
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n) \
    (((__cn) != NULL) ? \
     SYS_DLIST_CONTAINER(sys_dlist_peek_next((__dl), &((__cn)->__n)),   \
                      __cn, __n) : NULL)

/**
 * @brief Provide the primitive to iterate on a list under a container
 * Note: the loop is unsafe and thus __cn should not be detached
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_CONTAINER(l, c, n) {
 *         <user code>
 *     }
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __cn A container struct type pointer to peek each entry of the list
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_FOR_EACH_CONTAINER(__dl, __cn, __n)           \
    for ((__cn) = SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n);     \
         (__cn) != NULL;                                              \
         (__cn) = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n))

/**
 * @brief Provide the primitive to safely iterate on a list under a container
 * Note: __cn can be detached, it will not break the loop.
 *
 * User _MUST_ add the loop statement curly braces enclosing its own code:
 *
 *     SYS_DLIST_FOR_EACH_CONTAINER_SAFE(l, c, cn, n) {
 *         <user code>
 *     }
 *
 * @param __dl A pointer on a sys_dlist_t to iterate on
 * @param __cn A container struct type pointer to peek each entry of the list
 * @param __cns A container struct type pointer for the loop to run safely
 * @param __n The field name of sys_dnode_t within the container struct
 */
#define SYS_DLIST_FOR_EACH_CONTAINER_SAFE(__dl, __cn, __cns, __n)   \
    for ((__cn) = SYS_DLIST_PEEK_HEAD_CONTAINER(__dl, __cn, __n),   \
         (__cns) = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n);    \
         (__cn) != NULL; (__cn) = (__cns),              \
         (__cns) = SYS_DLIST_PEEK_NEXT_CONTAINER(__dl, __cn, __n))

/**
 * @brief initialize list to its empty state
 *
 * @param list the doubly-linked list
 */

static inline void sys_dlist_init(sys_dlist_t *list)
{
    list->head = (sys_dnode_t *)list;
    list->tail = (sys_dnode_t *)list;
}

/**
 * @brief Static initializer for a doubly-linked list
 */
#define SYS_DLIST_STATIC_INIT(ptr_to_list) { {(ptr_to_list)}, {(ptr_to_list)} }

/**
 * @brief initialize node to its state when not in a list
 *
 * @param node the node
 */

static inline void sys_dnode_init(sys_dnode_t *node)
{
    node->next = NULL;
    node->prev = NULL;
}

/**
 * @brief check if a node is a member of any list
 *
 * @param node the node
 *
 * @return true if node is linked into a list, false if it is not
 */

static inline bool sys_dnode_is_linked(const sys_dnode_t *node)
{
    return node->next != NULL;
}

/**
 * @brief check if a node is the list's head
 *
 * @param list the doubly-linked list to operate on
 * @param node the node to check
 *
 * @return true if node is the head, false otherwise
 */

static inline bool sys_dlist_is_head(const sys_dlist_t *list, const sys_dnode_t *node)
{
    return list->head == node;
}

/**
 * @brief check if a node is the list's tail
 *
 * @param list the doubly-linked list to operate on
 * @param node the node to check
 *
 * @return true if node is the tail, false otherwise
 */

static inline bool sys_dlist_is_tail(const sys_dlist_t *list, const sys_dnode_t *node)
{
    return list->tail == node;
}

/**
 * @brief check if the list is empty
 *
 * @param list the doubly-linked list to operate on
 *
 * @return true if empty, false otherwise
 */

static inline bool sys_dlist_is_empty(const sys_dlist_t *list)
{
    return list->head == list;
}

/**
 * @brief check if more than one node present
 *
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 *
 * @return true if multiple nodes, false otherwise
 */

static inline bool sys_dlist_has_multiple_nodes(const sys_dlist_t *list)
{
    return list->head != list->tail;
}

/**
 * @brief get a reference to the head item in the list
 *
 * @param list the doubly-linked list to operate on
 *
 * @return a pointer to the head element, NULL if list is empty
 */

static inline sys_dnode_t *sys_dlist_peek_head(const sys_dlist_t *list)
{
    return sys_dlist_is_empty(list) ? NULL : list->head;
}

/**
 * @brief get a reference to the head item in the list
 *
 * The list must be known to be non-empty.
 *
 * @param list the doubly-linked list to operate on
 *
 * @return a pointer to the head element
 */

static inline sys_dnode_t *sys_dlist_peek_head_not_empty(const sys_dlist_t *list)
{
    return list->head;
}

/**
 * @brief get a reference to the next item in the list, node is not NULL
 *
 * Faster than sys_dlist_peek_next() if node is known not to be NULL.
 *
 * @param list the doubly-linked list to operate on
 * @param node the node from which to get the next element in the list
 *
 * @return a pointer to the next element from a node, NULL if node is the tail
 */

static inline sys_dnode_t *sys_dlist_peek_next_no_check(const sys_dlist_t *list,
                                                        const sys_dnode_t *node)
{
    return (node == list->tail) ? NULL : node->next;
}

/**
 * @brief get a reference to the next item in the list
 *
 * @param list the doubly-linked list to operate on
 * @param node the node from which to get the next element in the list
 *
 * @return a pointer to the next element from a node, NULL if node is the tail
 * or NULL (when node comes from reading the head of an empty list).
 */

static inline sys_dnode_t *sys_dlist_peek_next(const sys_dlist_t *list,
                                               const sys_dnode_t *node)
{
    return (node != NULL) ? sys_dlist_peek_next_no_check(list, node) : NULL;
}

/**
 * @brief get a reference to the previous item in the list, node is not NULL
 *
 * Faster than sys_dlist_peek_prev() if node is known not to be NULL.
 *
 * @param list the doubly-linked list to operate on
 * @param node the node from which to get the previous element in the list
 *
 * @return a pointer to the previous element from a node, NULL if node is the
 *     tail
 */

static inline sys_dnode_t *sys_dlist_peek_prev_no_check(const sys_dlist_t *list,
                                                        const sys_dnode_t *node)
{
    return (node == list->head) ? NULL : node->prev;
}

/**
 * @brief get a reference to the previous item in the list
 *
 * @param list the doubly-linked list to operate on
 * @param node the node from which to get the previous element in the list
 *
 * @return a pointer to the previous element from a node, NULL if node is the
 *     tail or NULL (when node comes from reading the head of an empty
 *     list).
 */

static inline sys_dnode_t *sys_dlist_peek_prev(const sys_dlist_t *list,
                                               const sys_dnode_t *node)
{
    return (node != NULL) ? sys_dlist_peek_prev_no_check(list, node) : NULL;
}

/**
 * @brief get a reference to the tail item in the list
 *
 * @param list the doubly-linked list to operate on
 *
 * @return a pointer to the tail element, NULL if list is empty
 */

static inline sys_dnode_t *sys_dlist_peek_tail(const sys_dlist_t *list)
{
    return sys_dlist_is_empty(list) ? NULL : list->tail;
}

/**
 * @brief add node to tail of list
 *
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to append
 */

static inline void sys_dlist_append(sys_dlist_t *list, sys_dnode_t *node)
{
    sys_dnode_t *const tail = list->tail;

    node->next = list;
    node->prev = tail;

    tail->next = node;
    list->tail = node;
}

/**
 * @brief add node to head of list
 *
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to append
 */

static inline void sys_dlist_prepend(sys_dlist_t *list, sys_dnode_t *node)
{
    sys_dnode_t *const head = list->head;

    node->next = head;
    node->prev = list;

    head->prev = node;
    list->head = node;
}

/**
 * @brief Insert a node into a list
 *
 * Insert a node before a specified node in a dlist.
 *
 * @param successor the position before which "node" will be inserted
 * @param node the element to insert
 */
static inline void sys_dlist_insert(sys_dnode_t *successor, sys_dnode_t *node)
{
    sys_dnode_t *const prev = successor->prev;

    node->prev = prev;
    node->next = successor;
    prev->next = node;
    successor->prev = node;
}

/**
 * @brief insert node at position
 *
 * Insert a node in a location depending on a external condition. The cond()
 * function checks if the node is to be inserted _before_ the current node
 * against which it is checked.
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 * @param node the element to insert
 * @param cond a function that determines if the current node is the correct
 *             insert point
 * @param data parameter to cond()
 */

static inline void sys_dlist_insert_at(sys_dlist_t *list, sys_dnode_t *node,
                                       int (*cond)(sys_dnode_t *node, void *data), void *data)
{
    if (sys_dlist_is_empty(list)) {
        sys_dlist_append(list, node);
    } else {
        sys_dnode_t *pos = sys_dlist_peek_head(list);

        while ((pos != NULL) && (cond(pos, data) == 0)) {
            pos = sys_dlist_peek_next(list, pos);
        }
        if (pos != NULL) {
            sys_dlist_insert(pos, node);
        } else {
            sys_dlist_append(list, node);
        }
    }
}

/**
 * @brief remove a specific node from a list
 *
 * Like :c:func:`sys_dlist_remove()`, this routine removes a specific node
 * from a list. However, unlike :c:func:`sys_dlist_remove()`, this routine
 * does not re-initialize the removed node. One significant implication of
 * this difference is that the function :c:func`sys_dnode_is_linked()` will
 * not work on a dequeued node.
 *
 * The list is implicit from the node. The node must be part of a list.
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param node the node to dequeue
 */
static inline void sys_dlist_dequeue(sys_dnode_t *node)
{
    sys_dnode_t *const prev = node->prev;
    sys_dnode_t *const next = node->next;

    prev->next = next;
    next->prev = prev;
}

/**
 * @brief remove a specific node from a list
 *
 * The list is implicit from the node. The node must be part of a list.
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param node the node to remove
 */

static inline void sys_dlist_remove(sys_dnode_t *node)
{
    sys_dnode_t *const prev = node->prev;
    sys_dnode_t *const next = node->next;

    prev->next = next;
    next->prev = prev;
    sys_dnode_init(node);
}

/**
 * @brief get the first node in a list
 *
 * This and other sys_dlist_*() functions are not thread safe.
 *
 * @param list the doubly-linked list to operate on
 *
 * @return the first node in the list, NULL if list is empty
 */

static inline sys_dnode_t *sys_dlist_get(sys_dlist_t *list)
{
    sys_dnode_t *node = NULL;

    if (!sys_dlist_is_empty(list)) {
        node = list->head;
        sys_dlist_remove(node);
    }

    return node;
}

/**
 * @brief Compute the size of the given list in O(n) time
 *
 * @param list A pointer on the list
 *
 * @return an integer equal to the size of the list, or 0 if empty
 */
static inline size_t sys_dlist_len(const sys_dlist_t *list)
{
    size_t len = 0;
    sys_dnode_t *node = NULL;

    SYS_DLIST_FOR_EACH_NODE(list, node) {
        len++;
    }
    return len;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS_DLIST_H_ */
