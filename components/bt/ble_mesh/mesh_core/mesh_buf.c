/*
 * Copyright (c) 2015 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "mesh_buf.h"
#include "mesh_trace.h"
#include "esp_bt_defs.h"
#include "sdkconfig.h"
#if CONFIG_BT_MESH

/* Helpers to access the storage array, since we don't have access to its
 * type at this point anymore.
 */
#define BUF_SIZE(pool) (sizeof(struct net_buf) + \
            ROUND_UP(pool->buf_size, 4) + \
            ROUND_UP(pool->user_data_size, 4))
#define UNINIT_BUF(pool, n) (struct net_buf *)(((u8_t *)(pool->__bufs)) + \
                           ((n) * BUF_SIZE(pool)))

/* Linker-defined symbol bound to the static pool structs */
struct net_buf_pool *net_buf_pool_get(int id)
{
    return (struct net_buf_pool *)id;
}

//static int pool_id(struct net_buf_pool *pool)
//{
//    BT_DBG("%s, the pool id = %p", __func__, pool);
//  return (int)&pool[pool->buf_count - pool->uninit_count - 1];
//}

static inline struct net_buf *pool_get_uninit(struct net_buf_pool *pool,
        u16_t uninit_count)
{
    struct net_buf *buf;

    buf = UNINIT_BUF(pool, pool->buf_count - uninit_count);

    buf->pool_id = pool;
    buf->size = pool->buf_size;
    BT_DBG("%s, pool_id =  %p, uninit_count = %d, pool->buf_count = %d, buf = %p, buf->size = %d, pool = %p",
           __func__, buf->pool_id, uninit_count, pool->buf_count, buf, buf->size, pool);

    return buf;
}

void *net_buf_simple_add(struct net_buf_simple *buf, size_t len)
{
    u8_t *tail = net_buf_simple_tail(buf);

    NET_BUF_SIMPLE_DBG("buf %p len %u", buf, len);

    NET_BUF_SIMPLE_ASSERT(net_buf_simple_tailroom(buf) >= len);

    buf->len += len;
    return tail;
}

void *net_buf_simple_add_mem(struct net_buf_simple *buf, const void *mem,
                             size_t len)
{
    NET_BUF_SIMPLE_DBG("buf %p len %u", buf, len);

    return memcpy(net_buf_simple_add(buf, len), mem, len);
}

u8_t *net_buf_simple_add_u8(struct net_buf_simple *buf, u8_t val)
{
    u8_t *u8;

    NET_BUF_SIMPLE_DBG("buf %p val 0x%02x", buf, val);

    u8 = net_buf_simple_add(buf, 1);
    *u8 = val;

    return u8;
}

void net_buf_simple_add_le16(struct net_buf_simple *buf, u16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_le16(val);
    memcpy(net_buf_simple_add(buf, sizeof(val)), &val, sizeof(val));
}

void net_buf_simple_add_be16(struct net_buf_simple *buf, u16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_be16(val);
    memcpy(net_buf_simple_add(buf, sizeof(val)), &val, sizeof(val));
}

void net_buf_simple_add_le32(struct net_buf_simple *buf, u32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_le32(val);
    memcpy(net_buf_simple_add(buf, sizeof(val)), &val, sizeof(val));
}

void net_buf_simple_add_be32(struct net_buf_simple *buf, u32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_be32(val);
    memcpy(net_buf_simple_add(buf, sizeof(val)), &val, sizeof(val));
}

void *net_buf_simple_push(struct net_buf_simple *buf, size_t len)
{
    NET_BUF_SIMPLE_DBG("buf %p len %u", buf, len);

    NET_BUF_SIMPLE_ASSERT(net_buf_simple_headroom(buf) >= len);

    buf->data -= len;
    buf->len += len;
    return buf->data;
}

void net_buf_simple_push_le16(struct net_buf_simple *buf, u16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_le16(val);
    memcpy(net_buf_simple_push(buf, sizeof(val)), &val, sizeof(val));
}

void net_buf_simple_push_be16(struct net_buf_simple *buf, u16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    val = sys_cpu_to_be16(val);
    memcpy(net_buf_simple_push(buf, sizeof(val)), &val, sizeof(val));
}

void net_buf_simple_push_u8(struct net_buf_simple *buf, u8_t val)
{
    u8_t *data = net_buf_simple_push(buf, 1);

    *data = val;
}

void *net_buf_simple_pull(struct net_buf_simple *buf, size_t len)
{
    NET_BUF_SIMPLE_DBG("buf %p len %u", buf, len);

    NET_BUF_SIMPLE_ASSERT(buf->len >= len);

    buf->len -= len;
    return buf->data += len;
}

u8_t net_buf_simple_pull_u8(struct net_buf_simple *buf)
{
    u8_t val;

    val = buf->data[0];
    net_buf_simple_pull(buf, 1);

    return val;
}

u16_t net_buf_simple_pull_le16(struct net_buf_simple *buf)
{
    u16_t val;

    val = UNALIGNED_GET((u16_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le16_to_cpu(val);
}

u16_t net_buf_simple_pull_be16(struct net_buf_simple *buf)
{
    u16_t val;

    val = UNALIGNED_GET((u16_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be16_to_cpu(val);
}

u32_t net_buf_simple_pull_le32(struct net_buf_simple *buf)
{
    u32_t val;

    val = UNALIGNED_GET((u32_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le32_to_cpu(val);
}

u32_t net_buf_simple_pull_be32(struct net_buf_simple *buf)
{
    u32_t val;

    val = UNALIGNED_GET((u32_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be32_to_cpu(val);
}

size_t net_buf_simple_headroom(struct net_buf_simple *buf)
{
    return buf->data - buf->__buf;
}

size_t net_buf_simple_tailroom(struct net_buf_simple *buf)
{
    return buf->size - net_buf_simple_headroom(buf) - buf->len;
}

void net_buf_reset(struct net_buf *buf)
{
    NET_BUF_ASSERT(buf->flags == 0);
    NET_BUF_ASSERT(buf->frags == NULL);

    buf->len   = 0;
    buf->data  = buf->__buf;
}

void net_buf_reserve(struct net_buf *buf, size_t reserve)
{
    NET_BUF_ASSERT(buf);
    NET_BUF_ASSERT(buf->len == 0);
    NET_BUF_DBG("buf %p reserve %u", buf, reserve);

    buf->data = buf->__buf + reserve;
}

void net_buf_slist_put(sys_slist_t *list, struct net_buf *buf)
{
    struct net_buf *tail;
    unsigned int key;

    NET_BUF_ASSERT(list);
    NET_BUF_ASSERT(buf);

    for (tail = buf; tail->frags; tail = tail->frags) {
        tail->flags |= NET_BUF_FRAGS;
    }

    key = irq_lock();
    sys_slist_append_list(list, &buf->node, &tail->node);
    irq_unlock(key);
}

struct net_buf *net_buf_slist_get(sys_slist_t *list)
{
    struct net_buf *buf, *frag;
    unsigned int key;

    NET_BUF_ASSERT(list);

    key = irq_lock();
    buf = (void *)sys_slist_get(list);
    irq_unlock(key);

    if (!buf) {
        return NULL;
    }

    /* Get any fragments belonging to this buffer */
    for (frag = buf; (frag->flags & NET_BUF_FRAGS); frag = frag->frags) {
        key = irq_lock();
        frag->frags = (void *)sys_slist_get(list);
        irq_unlock(key);

        NET_BUF_ASSERT(frag->frags);

        /* The fragments flag is only for list-internal usage */
        frag->flags &= ~NET_BUF_FRAGS;
    }

    /* Mark the end of the fragment list */
    frag->frags = NULL;

    return buf;
}

void net_buf_put(struct k_fifo *fifo, struct net_buf *buf)
{
    struct net_buf *tail;

    //NET_BUF_ASSERT(fifo);
    //NET_BUF_ASSERT(buf);

    //ble_mesh_msg_t msg = {0};

    //ble_mesh_task_post(ble_mesh_msg_t *msg, 0);
    for (tail = buf; tail->frags; tail = tail->frags) {
        tail->flags |= NET_BUF_FRAGS;
    }

    //k_fifo_put_list(fifo, buf, tail);
}

struct net_buf *net_buf_ref(struct net_buf *buf)
{
    NET_BUF_ASSERT(buf);

    NET_BUF_DBG("buf %p (old) ref %u pool_id %p",
                buf, buf->ref, buf->pool_id);
    buf->ref++;
    return buf;
}

struct net_buf *net_buf_clone(struct net_buf *buf, s32_t timeout)
{
    struct net_buf_pool *pool;
    struct net_buf *clone;

    NET_BUF_ASSERT(buf);

    pool = buf->pool_id;//net_buf_pool_get(buf->pool_id);

    clone = net_buf_alloc(pool, timeout);
    if (!clone) {
        return NULL;
    }

    net_buf_reserve(clone, net_buf_headroom(buf));

    /* TODO: Add reference to the original buffer instead of copying it. */
    memcpy(net_buf_add(clone, buf->len), buf->data, buf->len);

    return clone;
}

struct net_buf *net_buf_frag_last(struct net_buf *buf)
{
    NET_BUF_ASSERT(buf);

    while (buf->frags) {
        buf = buf->frags;
    }

    return buf;
}

void net_buf_frag_insert(struct net_buf *parent, struct net_buf *frag)
{
    NET_BUF_ASSERT(parent);
    NET_BUF_ASSERT(frag);

    if (parent->frags) {
        net_buf_frag_last(frag)->frags = parent->frags;
    }
    /* Take ownership of the fragment reference */
    parent->frags = frag;
}

#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *net_buf_frag_del_debug(struct net_buf *parent,
                                       struct net_buf *frag,
                                       const char *func, int line)
#else
struct net_buf *net_buf_frag_del(struct net_buf *parent, struct net_buf *frag)
#endif
{
    struct net_buf *next_frag;

    NET_BUF_ASSERT(frag);

    if (parent) {
        NET_BUF_ASSERT(parent->frags);
        NET_BUF_ASSERT(parent->frags == frag);
        parent->frags = frag->frags;
    }

    next_frag = frag->frags;

    frag->frags = NULL;

#if defined(CONFIG_NET_BUF_LOG)
    net_buf_unref_debug(frag, func, line);
#else
    net_buf_unref(frag);
#endif

    return next_frag;
}

#if defined(CONFIG_NET_BUF_LOG)
void net_buf_unref_debug(struct net_buf *buf, const char *func, int line)
#else
void net_buf_unref(struct net_buf *buf)
#endif
{
    NET_BUF_ASSERT(buf);

    while (buf) {
        struct net_buf *frags = buf->frags;
        struct net_buf_pool *pool;

#if defined(CONFIG_NET_BUF_LOG)
        if (!buf->ref) {
            NET_BUF_ERR("%s():%d: buf %p double free", func, line,
                        buf);
            return;
        }
#endif
        NET_BUF_DBG("buf %p ref %u pool_id %p frags %p", buf, buf->ref,
                    buf->pool_id, buf->frags);

        /* Change by Espressif. Add !buf->ref to avoid minus 0 */
        if (!buf->ref || --buf->ref > 0) {
            return;
        }

        buf->frags = NULL;

        pool = buf->pool_id;//net_buf_pool_get(buf->pool_id);
        pool->uninit_count++;
#if defined(CONFIG_NET_BUF_POOL_USAGE)
        pool->avail_count++;
        BT_DBG("%s, pool->avail_count = %d, pool->uninit_count = %d", __func__,
               pool->avail_count, pool->uninit_count);
        NET_BUF_ASSERT(pool->avail_count <= pool->buf_count);
#endif

        if (pool->destroy) {
            pool->destroy(buf);
        } else {
            net_buf_destroy(buf);
        }

        buf = frags;
    }
}

#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *net_buf_alloc_debug(struct net_buf_pool *pool, s32_t timeout,
                                    const char *func, int line)
#else
struct net_buf *net_buf_alloc(struct net_buf_pool *pool, s32_t timeout)
#endif
{
    struct net_buf *buf = NULL;
    unsigned int key;
    int i;

    NET_BUF_ASSERT(pool);

    NET_BUF_DBG("%s():%d: pool %p timeout %d", func, line, pool, timeout);
    BT_DBG("%s, pool = %p , pool->uninit_count= %d, buf_count = %d", __func__,
           pool, pool->uninit_count, pool->buf_count);
    /* We need to lock interrupts temporarily to prevent race conditions
     * when accessing pool->uninit_count.
     */
    key = irq_lock();

    /* If there are uninitialized buffers we're guaranteed to succeed
     * with the allocation one way or another.
     */
    if (pool->uninit_count) {
        /* Change by Espressif. Use buf when buf->ref is 0 */
        for (i = pool->buf_count; i > 0; i--) {
            buf = pool_get_uninit(pool, i);
            if (!buf->ref) {
                goto success;
            }
        }
        // u16_t uninit_count;

        /* If this is not the first access to the pool, we can
         * be opportunistic and try to fetch a previously used
         * buffer from the LIFO with K_NO_WAIT.
         */
        //if (pool->uninit_count < pool->buf_count) {
        //  buf = &pool->__bufs[pool->buf_count - pool->uninit_count];
        //if (buf) {
        //  irq_unlock(key);
        //  goto success;
        //}
        //}

        // uninit_count = pool->uninit_count--;
        // irq_unlock(key);

        // buf = pool_get_uninit(pool, uninit_count);
        // goto success;
    }

    irq_unlock(key);

    if (!buf) {
        BT_ERR("%s():Failed to get free buffer", __func__);
    }
    return buf;

success:
    NET_BUF_DBG("allocated buf %p", buf);
    irq_unlock(key);

    buf->ref   = 1;
    buf->index = pool->buf_count - i;
    buf->flags = 0;
    buf->frags = NULL;
    net_buf_reset(buf);

    pool->uninit_count--;
#if defined(CONFIG_NET_BUF_POOL_USAGE)
    pool->avail_count--;
    NET_BUF_ASSERT(pool->avail_count >= 0);
#endif

    return buf;
}

int net_buf_id(struct net_buf *buf)
{
    struct net_buf_pool *pool = buf->pool_id;//net_buf_pool_get(buf->pool_id);
    u8_t *pool_start = (u8_t *)pool->__bufs;
    u8_t *buf_ptr = (u8_t *)buf;

    return (buf_ptr - pool_start) / BUF_SIZE(pool);
}

#endif /* #if CONFIG_BT_MESH */

