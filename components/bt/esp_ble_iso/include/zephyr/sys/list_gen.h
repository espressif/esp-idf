/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS_LIST_GEN_H_
#define ZEPHYR_INCLUDE_SYS_LIST_GEN_H_

#include <stddef.h>
#include <stdbool.h>
#include <zephyr/sys/util.h>

#define Z_GENLIST_FOR_EACH_NODE(__lname, __l, __sn)         \
    for ((__sn) = sys_ ## __lname ## _peek_head(__l); (__sn) != NULL;   \
         (__sn) = sys_ ## __lname ## _peek_next(__sn))

#define Z_GENLIST_ITERATE_FROM_NODE(__lname, __l, __sn)         \
    for ((__sn) = (__sn) ? sys_ ## __lname ## _peek_next_no_check(__sn) \
             : sys_ ## __lname ## _peek_head(__l);      \
         (__sn) != NULL;                        \
         (__sn) = sys_ ## __lname ## _peek_next(__sn))

#define Z_GENLIST_FOR_EACH_NODE_SAFE(__lname, __l, __sn, __sns)     \
    for ((__sn) = sys_ ## __lname ## _peek_head(__l),           \
             (__sns) = sys_ ## __lname ## _peek_next(__sn); \
         (__sn) != NULL ; (__sn) = (__sns),             \
             (__sns) = sys_ ## __lname ## _peek_next(__sn))

#define Z_GENLIST_CONTAINER(__ln, __cn, __n)                \
    ((__ln) ? CONTAINER_OF((__ln), __typeof__(*(__cn)), __n) : NULL)

#define Z_GENLIST_PEEK_HEAD_CONTAINER(__lname, __l, __cn, __n)      \
    Z_GENLIST_CONTAINER(sys_ ## __lname ## _peek_head(__l), __cn, __n)

#define Z_GENLIST_PEEK_TAIL_CONTAINER(__lname, __l, __cn, __n)      \
    Z_GENLIST_CONTAINER(sys_ ## __lname ## _peek_tail(__l), __cn, __n)

#define Z_GENLIST_PEEK_NEXT_CONTAINER(__lname, __cn, __n)       \
    ((__cn) ? Z_GENLIST_CONTAINER(                  \
            sys_ ## __lname ## _peek_next(&((__cn)->__n)),  \
            __cn, __n) : NULL)

#define Z_GENLIST_FOR_EACH_CONTAINER(__lname, __l, __cn, __n)       \
    for ((__cn) = Z_GENLIST_PEEK_HEAD_CONTAINER(__lname, __l, __cn, \
                          __n);         \
         (__cn) != NULL;                        \
         (__cn) = Z_GENLIST_PEEK_NEXT_CONTAINER(__lname, __cn, __n))

#define Z_GENLIST_FOR_EACH_CONTAINER_SAFE(__lname, __l, __cn, __cns, __n)     \
    for ((__cn) = Z_GENLIST_PEEK_HEAD_CONTAINER(__lname, __l, __cn, __n),   \
         (__cns) = Z_GENLIST_PEEK_NEXT_CONTAINER(__lname, __cn, __n); \
         (__cn) != NULL; (__cn) = (__cns),              \
         (__cns) = Z_GENLIST_PEEK_NEXT_CONTAINER(__lname, __cn, __n))

#define Z_GENLIST_IS_EMPTY(__lname)                 \
    static inline bool                      \
    sys_ ## __lname ## _is_empty(const sys_ ## __lname ## _t *list) \
    {                               \
        return (sys_ ## __lname ## _peek_head(list) == NULL);   \
    }

#define Z_GENLIST_PEEK_NEXT_NO_CHECK(__lname, __nname)              \
    static inline sys_ ## __nname ## _t *                   \
    sys_ ## __lname ## _peek_next_no_check(const sys_ ## __nname ## _t *node) \
    {                                   \
        return z_ ## __nname ## _next_peek(node);           \
    }

#define Z_GENLIST_PEEK_NEXT(__lname, __nname)                    \
    static inline sys_ ## __nname ## _t *                    \
    sys_ ## __lname ## _peek_next(const sys_ ## __nname ## _t *node)     \
    {                                    \
        return (node != NULL) ?                                        \
            sys_ ## __lname ## _peek_next_no_check(node) :       \
                  NULL;                      \
    }

#define Z_GENLIST_PREPEND(__lname, __nname)                   \
    static inline void                            \
    sys_ ## __lname ## _prepend(sys_ ## __lname ## _t *list,          \
                    sys_ ## __nname ## _t *node)          \
    {                                     \
        z_ ## __nname ## _next_set(node,                  \
                    sys_ ## __lname ## _peek_head(list)); \
        z_ ## __lname ## _head_set(list, node);               \
                                          \
        if (sys_ ## __lname ## _peek_tail(list) == NULL) {        \
            z_ ## __lname ## _tail_set(list,              \
                    sys_ ## __lname ## _peek_head(list)); \
        }                                 \
    }

#define Z_GENLIST_APPEND(__lname, __nname)              \
    static inline void                      \
    sys_ ## __lname ## _append(sys_ ## __lname ## _t *list,     \
                   sys_ ## __nname ## _t *node)     \
    {                               \
        z_ ## __nname ## _next_set(node, NULL);         \
                                    \
        if (sys_ ## __lname ## _peek_tail(list) == NULL) {  \
            z_ ## __lname ## _tail_set(list, node);     \
            z_ ## __lname ## _head_set(list, node);     \
        } else {                        \
            z_ ## __nname ## _next_set(         \
                sys_ ## __lname ## _peek_tail(list),    \
                node);                  \
            z_ ## __lname ## _tail_set(list, node);     \
        }                           \
    }

#define Z_GENLIST_APPEND_LIST(__lname, __nname)             \
    static inline void                      \
    sys_ ## __lname ## _append_list(sys_ ## __lname ## _t *list,    \
                    void *head, void *tail)     \
{                                   \
    if (head != NULL && tail != NULL) {             \
        if (sys_ ## __lname ## _peek_tail(list) == NULL) {  \
            z_ ## __lname ## _head_set(list,        \
                (sys_ ## __nname ## _t *)head);     \
        } else {                        \
            z_ ## __nname ## _next_set(         \
                sys_ ## __lname ## _peek_tail(list),    \
                (sys_ ## __nname ## _t *)head);     \
        }                           \
        z_ ## __lname ## _tail_set(list,            \
            (sys_ ## __nname ## _t *)tail);         \
    }                               \
}

#define Z_GENLIST_MERGE_LIST(__lname, __nname)              \
    static inline void                      \
    sys_ ## __lname ## _merge_ ## __lname (             \
                sys_ ## __lname ## _t *list,        \
                sys_ ## __lname ## _t *list_to_append)  \
    {                               \
        sys_ ## __nname ## _t *head, *tail;         \
        head = sys_ ## __lname ## _peek_head(list_to_append);   \
        tail = sys_ ## __lname ## _peek_tail(list_to_append);   \
        sys_ ## __lname ## _append_list(list, head, tail);  \
        sys_ ## __lname ## _init(list_to_append);       \
    }

#define Z_GENLIST_INSERT(__lname, __nname)              \
    static inline void                      \
    sys_ ## __lname ## _insert(sys_ ## __lname ## _t *list,     \
                   sys_ ## __nname ## _t *prev,     \
                   sys_ ## __nname ## _t *node)     \
    {                               \
        if (prev == NULL) {                 \
            sys_ ## __lname ## _prepend(list, node);    \
        } else if (z_ ## __nname ## _next_peek(prev) == NULL) { \
            sys_ ## __lname ## _append(list, node);     \
        } else {                        \
            z_ ## __nname ## _next_set(node,        \
                z_ ## __nname ## _next_peek(prev)); \
            z_ ## __nname ## _next_set(prev, node);     \
        }                           \
    }

#define Z_GENLIST_GET_NOT_EMPTY(__lname, __nname)           \
    static inline sys_ ## __nname ## _t *               \
    sys_ ## __lname ## _get_not_empty(sys_ ## __lname ## _t *list)  \
    {                               \
        sys_ ## __nname ## _t *node =               \
                sys_ ## __lname ## _peek_head(list);    \
                                    \
        z_ ## __lname ## _head_set(list,            \
                z_ ## __nname ## _next_peek(node)); \
        if (sys_ ## __lname ## _peek_tail(list) == node) {  \
            z_ ## __lname ## _tail_set(list,        \
                sys_ ## __lname ## _peek_head(list));   \
        }                           \
                                    \
        return node;                        \
    }

#define Z_GENLIST_GET(__lname, __nname)                 \
    static inline sys_ ## __nname ## _t *               \
    sys_ ## __lname ## _get(sys_ ## __lname ## _t *list)        \
    {                               \
        return sys_ ## __lname ## _is_empty(list) ? NULL :  \
            sys_ ## __lname ## _get_not_empty(list);    \
    }

#define Z_GENLIST_REMOVE(__lname, __nname)                    \
    static inline void                            \
    sys_ ## __lname ## _remove(sys_ ## __lname ## _t *list,           \
                   sys_ ## __nname ## _t *prev_node,          \
                   sys_ ## __nname ## _t *node)           \
    {                                     \
        if (prev_node == NULL) {                      \
            z_ ## __lname ## _head_set(list,              \
                z_ ## __nname ## _next_peek(node));       \
                                          \
            /* Was node also the tail? */                 \
            if (sys_ ## __lname ## _peek_tail(list) == node) {    \
                z_ ## __lname ## _tail_set(list,          \
                    sys_ ## __lname ## _peek_head(list)); \
            }                             \
        } else {                              \
            z_ ## __nname ## _next_set(prev_node,             \
                z_ ## __nname ## _next_peek(node));       \
                                          \
            /* Was node the tail? */                  \
            if (sys_ ## __lname ## _peek_tail(list) == node) {    \
                z_ ## __lname ## _tail_set(list,          \
                                 prev_node);      \
            }                             \
        }                                 \
                                          \
        z_ ## __nname ## _next_set(node, NULL);               \
    }

#define Z_GENLIST_FIND_AND_REMOVE(__lname, __nname)          \
    static inline bool                       \
    sys_ ## __lname ## _find_and_remove(sys_ ## __lname ## _t *list, \
                        sys_ ## __nname ## _t *node) \
    {                                \
        sys_ ## __nname ## _t *prev = NULL;          \
        sys_ ## __nname ## _t *test;                 \
                                     \
        Z_GENLIST_FOR_EACH_NODE(__lname, list, test) {       \
            if (test == node) {              \
                sys_ ## __lname ## _remove(list, prev,   \
                               node);    \
                return true;                 \
            }                        \
                                     \
            prev = test;                     \
        }                            \
                                     \
        return false;                        \
    }

#define Z_GENLIST_FIND(__lname, __nname)                                                 \
    static inline bool sys_##__lname##_find(                                         \
        const sys_##__lname##_t *list, const sys_##__nname##_t *node,                      \
        sys_##__nname##_t **prev)                                                          \
    {                                                                                          \
        sys_##__nname##_t *current = NULL;                                                 \
        sys_##__nname##_t *previous = NULL;                                                \
                                                                                                   \
        Z_GENLIST_FOR_EACH_NODE(__lname, list, current) {                                  \
            if (current == node) {                                                     \
                if (prev != NULL) {                                                \
                    *prev = previous;                                          \
                }                                                                  \
                return true;                                                       \
            }                                                                          \
                                                                                                   \
            previous = current;                                                        \
        }                                                                                  \
                                                                                                   \
        if (prev != NULL) {                                                                \
            *prev = previous;                                                          \
        }                                                                                  \
                                                                                                   \
        return false;                                                                      \
    }

#define Z_GENLIST_LEN(__lname, __nname)                                                            \
    static inline size_t sys_##__lname##_len(const sys_##__lname##_t * list)                   \
    {                                                                                          \
        size_t len = 0;                                                                    \
        static sys_##__nname##_t * node;                                                   \
        Z_GENLIST_FOR_EACH_NODE(__lname, list, node) {                                     \
            len++;                                                                     \
        }                                                                                  \
        return len;                                                                        \
    }

#endif /* ZEPHYR_INCLUDE_SYS_LIST_GEN_H_ */
