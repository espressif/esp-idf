/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>

#include "bt_common.h"
#include "osi/allocator.h"
#include "osi/osi.h"
#include "osi/slist.h"

#ifndef STAILQ_REMOVE_AFTER
#define STAILQ_REMOVE_AFTER(head, elm, field) do {                      \
    if ((STAILQ_NEXT((elm), field) =                                    \
         STAILQ_NEXT(STAILQ_NEXT((elm), field), field)) == NULL)        \
        (head)->stqh_last = &STAILQ_NEXT((elm), field);                 \
} while (0)
#endif

#ifndef STAILQ_FOREACH_SAFE
#define STAILQ_FOREACH_SAFE(var, head, field, tvar)                     \
    for ((var) = STAILQ_FIRST((head));                                  \
        (var) && ((tvar) = STAILQ_NEXT((var), field), 1);               \
        (var) = (tvar))
#endif

STAILQ_HEAD(slist_head, slist_node);

struct slist {
    struct slist_head head;
    size_t length;
    slist_free_cb free_cb;
};

static void slist_link_tail(slist_t *list, slist_node_t *node)
{
    STAILQ_INSERT_TAIL(&list->head, node, next);
    list->length++;
}

/* The caller has already unlinked |node|; decrement length and dispose of
 * |node| via |free_cb| if set.
 */
static slist_node_t *slist_free_node(slist_t *list, slist_node_t *node)
{
    slist_node_t *next;

    assert(list != NULL);
    assert(node != NULL);

    next = STAILQ_NEXT(node, next);
    if (list->free_cb != NULL) {
        list->free_cb(node);
    }
    --list->length;
    return next;
}

/* The caller has already unlinked |node|; decrement length without running |free_cb|. */
static slist_node_t *slist_delete_node(slist_t *list, slist_node_t *node)
{
    slist_node_t *next;

    assert(list != NULL);
    assert(node != NULL);

    next = STAILQ_NEXT(node, next);
    --list->length;
    return next;
}

static void slist_unlink_after(slist_t *list, slist_node_t *prev, slist_node_t *node, bool call_free_cb)
{
    struct slist_head *head = &list->head;

    if (prev == NULL) {
        assert(STAILQ_FIRST(head) == node);
        STAILQ_REMOVE_HEAD(head, next);
    } else {
        assert(STAILQ_NEXT(prev, next) == node);
        STAILQ_REMOVE_AFTER(head, prev, next);
    }

    if (call_free_cb) {
        (void)slist_free_node(list, node);
    } else {
        (void)slist_delete_node(list, node);
    }
}

static bool slist_detach(slist_t *list, slist_node_t *node, bool call_free_cb)
{
    struct slist_head *head = &list->head;
    slist_node_t *prev = NULL;
    slist_node_t *cur;
    slist_node_t *tmp;

    STAILQ_FOREACH_SAFE(cur, head, next, tmp) {
        if (cur != node) {
            prev = cur;
            continue;
        }

        slist_unlink_after(list, prev, node, call_free_cb);
        return true;
    }

    return false;
}

static void slist_clear(slist_t *list)
{
    slist_node_t *node;
    slist_node_t *tmp;

    assert(list != NULL);

    STAILQ_FOREACH_SAFE(node, &list->head, next, tmp) {
        STAILQ_REMOVE_HEAD(&list->head, next);
        (void)slist_free_node(list, node);
    }
}

slist_t *slist_new(slist_free_cb callback)
{
    slist_t *list = (slist_t *)osi_calloc(sizeof(slist_t));
    if (list == NULL) {
        OSI_TRACE_ERROR("%s osi_calloc failed.\n", __func__);
        return NULL;
    }

    STAILQ_INIT(&list->head);
    list->length = 0;
    list->free_cb = callback;
    return list;
}

void slist_free(slist_t *list)
{
    if (list == NULL) {
        return;
    }

    slist_clear(list);
    osi_free(list);
}

bool slist_is_empty(const slist_t *list)
{
    assert(list != NULL);
    return (list->length == 0);
}

bool slist_contains(const slist_t *list, const slist_node_t *node)
{
    const slist_node_t *cur;
    const slist_node_t *tmp;

    assert(list != NULL);
    assert(node != NULL);

    STAILQ_FOREACH_SAFE(cur, &list->head, next, tmp) {
        if (cur == node) {
            return true;
        }
    }
    return false;
}

size_t slist_length(const slist_t *list)
{
    assert(list != NULL);
    return list->length;
}

slist_node_t *slist_front(const slist_t *list)
{
    assert(list != NULL);
    assert(!slist_is_empty(list));
    return STAILQ_FIRST(&list->head);
}

bool slist_prepend(slist_t *list, slist_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);

    STAILQ_INSERT_HEAD(&list->head, node, next);
    ++list->length;
    return true;
}

bool slist_append(slist_t *list, slist_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);

    slist_link_tail(list, node);
    return true;
}

bool slist_remove(slist_t *list, slist_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);
    return slist_detach(list, node, true);
}

bool slist_delete(slist_t *list, slist_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);
    return slist_detach(list, node, false);
}

void slist_delete_after(slist_t *list, slist_node_t *prev, slist_node_t *node)
{
    assert(list != NULL);
    assert(node != NULL);
    slist_unlink_after(list, prev, node, false);
}

slist_node_t *slist_foreach(const slist_t *list, slist_iter_cb callback, void *context)
{
    slist_node_t *node;
    slist_node_t *tmp;

    assert(list != NULL);
    assert(callback != NULL);

    STAILQ_FOREACH_SAFE(node, &list->head, next, tmp) {
        if (!callback(node, context)) {
            return node;
        }
    }
    return NULL;
}

slist_node_t *slist_begin(const slist_t *list)
{
    assert(list != NULL);
    return STAILQ_FIRST(&list->head);
}

slist_node_t *slist_end(UNUSED_ATTR const slist_t *list)
{
    assert(list != NULL);
    return NULL;
}

slist_node_t *slist_next(const slist_node_t *node)
{
    assert(node != NULL);
    return STAILQ_NEXT(node, next);
}
