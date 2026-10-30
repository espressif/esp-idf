/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SLIST_H_
#define _SLIST_H_

#include "sys/queue.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct slist_node {
    STAILQ_ENTRY(slist_node) next;
} slist_node_t;

struct slist;
typedef struct slist slist_t;

typedef void (*slist_free_cb)(slist_node_t *node);
typedef bool (*slist_iter_cb)(slist_node_t *node, void *context);

/* Allocate the list head. Nodes are caller-owned and are not allocated here.
 * |callback| runs when a node is removed via the list_* path. NULL skips it.
 */
slist_t *slist_new(slist_free_cb callback);

/* Clear with the stored callback, then free the head. NULL is a no-op. */
void slist_free(slist_t *list);

bool slist_is_empty(const slist_t *list);
bool slist_contains(const slist_t *list, const slist_node_t *node);
size_t slist_length(const slist_t *list);

/* Endpoints. |list| must be non-empty. */
slist_node_t *slist_front(const slist_t *list);

bool slist_prepend(slist_t *list, slist_node_t *node);
bool slist_append(slist_t *list, slist_node_t *node);

/* Unlink the first matching node. remove runs the stored callback; delete does not. */
bool slist_remove(slist_t *list, slist_node_t *node);
bool slist_delete(slist_t *list, slist_node_t *node);

/* Unlink |node| when |prev| is its predecessor. NULL |prev| means |node| is the head. */
void slist_delete_after(slist_t *list, slist_node_t *prev, slist_node_t *node);

/* Return the node for which |callback| returned false, or NULL after the last node.
 * The successor is saved first, so the current node may be removed inside |callback|.
 */
slist_node_t *slist_foreach(const slist_t *list, slist_iter_cb callback, void *context);

slist_node_t *slist_begin(const slist_t *list);
slist_node_t *slist_end(const slist_t *list);
slist_node_t *slist_next(const slist_node_t *node);

#ifdef __cplusplus
}
#endif

#endif /* _SLIST_H_ */
