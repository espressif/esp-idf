/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dlist.h"
#include "sdkconfig.h"

#if CONFIG_BT_MESH

static sys_dlist_t test_list;

struct container_node {
    sys_dnode_t node;
    int unused;
};

static struct container_node test_node_1;
static struct container_node test_node_2;
static struct container_node test_node_3;
static struct container_node test_node_4;

static inline bool verify_emptyness(sys_dlist_t *list)
{
    sys_dnode_t *node;
    sys_dnode_t *s_node;
    struct container_node *cnode;
    struct container_node *s_cnode;
    int count;

    if (!sys_dlist_is_empty(list)) {
        return false;
    }

    if (sys_dlist_peek_head(list)) {
        return false;
    }

    if (sys_dlist_peek_tail(list)) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_NODE(list, node) {
        count++;
    }

    if (count) {
        return false;
    }

    SYS_DLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
        count++;
    }

    if (count) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_CONTAINER(list, cnode, node) {
        count++;
    }

    if (count) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
        count++;
    }

    if (count) {
        return false;
    }

    return true;
}

static inline bool verify_content_amount(sys_dlist_t *list, int amount)
{
    sys_dnode_t *node;
    sys_dnode_t *s_node;
    struct container_node *cnode;
    struct container_node *s_cnode;
    int count;

    if (sys_dlist_is_empty(list)) {
        return false;
    }

    if (!sys_dlist_peek_head(list)) {
        return false;
    }

    if (!sys_dlist_peek_tail(list)) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_NODE(list, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_NODE_SAFE(list, node, s_node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_CONTAINER(list, cnode, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    count = 0;
    SYS_DLIST_FOR_EACH_CONTAINER_SAFE(list, cnode, s_cnode, node) {
        count++;
    }

    if (count != amount) {
        return false;
    }

    return true;
}

static inline bool verify_tail_head(sys_dlist_t *list,
                                    sys_dnode_t *head,
                                    sys_dnode_t *tail,
                                    bool same)
{
    if (sys_dlist_peek_head(list) != head) {
        return false;
    }

    if (sys_dlist_peek_tail(list) != tail) {
        return false;
    }

    if (same) {
        if (sys_dlist_peek_head(list) != sys_dlist_peek_tail(list)) {
            return false;
        }
    } else {
        if (sys_dlist_peek_head(list) == sys_dlist_peek_tail(list)) {
            return false;
        }
    }

    return true;
}

void dlist_test(void)
{
    sys_dlist_init(&test_list);

    //zassert_true((verify_emptyness(&test_list)), "test_list should be empty");

    /* Appending node 1 */
    sys_dlist_append(&test_list, &test_node_1.node);
    //zassert_true((verify_content_amount(&test_list, 1)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_1.node,
    //                 &test_node_1.node, true)),
    //       "test_list head/tail are wrong");

    /* Finding and removing node 1 */
    sys_dlist_remove(&test_node_1.node);
    //zassert_true((verify_emptyness(&test_list)),
    //       "test_list should be empty");

    /* Prepending node 1 */
    sys_dlist_prepend(&test_list, &test_node_1.node);
    //zassert_true((verify_content_amount(&test_list, 1)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_1.node,
    //                 &test_node_1.node, true)),
    //       "test_list head/tail are wrong");

    /* Removing node 1 */
    sys_dlist_remove(&test_node_1.node);
    //zassert_true((verify_emptyness(&test_list)),
    //       "test_list should be empty");

    /* Appending node 1 */
    sys_dlist_append(&test_list, &test_node_1.node);
    /* Prepending node 2 */
    sys_dlist_prepend(&test_list, &test_node_2.node);

    //zassert_true((verify_content_amount(&test_list, 2)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_1.node, false)),
    //       "test_list head/tail are wrong");

    /* Appending node 3 */
    sys_dlist_append(&test_list, &test_node_3.node);

    //zassert_true((verify_content_amount(&test_list, 3)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_3.node, false)),
    //       "test_list head/tail are wrong");

    //zassert_true((sys_dlist_peek_next(&test_list, &test_node_2.node) ==
    //        &test_node_1.node),
    //       "test_list node links are wrong");

    /* Inserting node 4 after node 2 */
    sys_dlist_insert_after(&test_list, &test_node_2.node,
                           &test_node_4.node);

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_3.node, false)),
    //       "test_list head/tail are wrong");

    //zassert_true((sys_dlist_peek_next(&test_list, &test_node_2.node) ==
    //        &test_node_4.node),
    //       "test_list node links are wrong");

    /* Finding and removing node 1 */
    sys_dlist_remove(&test_node_1.node);
    //zassert_true((verify_content_amount(&test_list, 3)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_3.node, false)),
    //       "test_list head/tail are wrong");

    /* Removing node 3 */
    sys_dlist_remove(&test_node_3.node);
    //zassert_true((verify_content_amount(&test_list, 2)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_4.node, false)),
    //       "test_list head/tail are wrong");

    /* Removing node 4 */
    sys_dlist_remove(&test_node_4.node);
    //zassert_true((verify_content_amount(&test_list, 1)),
    //       "test_list has wrong content");

    //zassert_true((verify_tail_head(&test_list, &test_node_2.node,
    //                 &test_node_2.node, true)),
    //       "test_list head/tail are wrong");

    /* Removing node 2 */
    sys_dlist_remove(&test_node_2.node);
    //zassert_true((verify_emptyness(&test_list)),
    //       "test_list should be empty");

    /* test iterator from a node */
    struct data_node {
        sys_dnode_t node;
        int data;
    } data_node[6] = {
        { .data = 0 },
        { .data = 1 },
        { .data = 2 },
        { .data = 3 },
        { .data = 4 },
        { .data = 5 },
    };
    sys_dnode_t *node = NULL;
    int ii;

    sys_dlist_init(&test_list);

    for (ii = 0; ii < 6; ii++) {
        sys_dlist_append(&test_list, &data_node[ii].node);
    }

    ii = 0;
    SYS_DLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 2) {
            break;
        }
    }
    //zassert_equal(ii, 3, "");

    ii = 0;
    SYS_DLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
        if (((struct data_node *)node)->data == 3) {
            break;
        }
    }
    //zassert_equal(ii, 1, "");

    ii = 0;
    SYS_DLIST_ITERATE_FROM_NODE(&test_list, node) {
        ii++;
    }
    //zassert_equal(ii, 2, "");
}

#endif /* #if CONFIG_BT_MESH */

