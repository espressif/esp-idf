/*
 * Copyright (c) 2015 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "sdkconfig.h"

#include "mesh_buf.h"
#include "mesh_trace.h"

int net_buf_id(struct net_buf *buf)
{
    struct net_buf_pool *pool = buf->pool;

    return buf - pool->__bufs;
}

static inline struct net_buf *pool_get_uninit(struct net_buf_pool *pool,
        u16_t uninit_count)
{
    struct net_buf *buf;

    buf = &pool->__bufs[pool->buf_count - uninit_count];

    buf->pool = pool;

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

void *net_buf_simple_pull_mem(struct net_buf_simple *buf, size_t len)
{
    void *data = buf->data;

    NET_BUF_SIMPLE_DBG("buf %p len %zu", buf, len);

    NET_BUF_SIMPLE_ASSERT(buf->len >= len);

    buf->len -= len;
    buf->data += len;

    return data;
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

    net_buf_simple_reset(&buf->b);
}

void net_buf_simple_reserve(struct net_buf_simple *buf, size_t reserve)
{
    NET_BUF_ASSERT(buf);
    NET_BUF_ASSERT(buf->len == 0U);
    NET_BUF_DBG("buf %p reserve %zu", buf, reserve);

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

    key = bt_mesh_irq_lock();
    sys_slist_append_list(list, &buf->node, &tail->node);
    bt_mesh_irq_unlock(key);
}

struct net_buf *net_buf_slist_get(sys_slist_t *list)
{
    struct net_buf *buf, *frag;
    unsigned int key;

    NET_BUF_ASSERT(list);

    key = bt_mesh_irq_lock();
    buf = (void *)sys_slist_get(list);
    bt_mesh_irq_unlock(key);

    if (!buf) {
        return NULL;
    }

    /* Get any fragments belonging to this buffer */
    for (frag = buf; (frag->flags & NET_BUF_FRAGS); frag = frag->frags) {
        key = bt_mesh_irq_lock();
        frag->frags = (void *)sys_slist_get(list);
        bt_mesh_irq_unlock(key);

        NET_BUF_ASSERT(frag->frags);

        /* The fragments flag is only for list-internal usage */
        frag->flags &= ~NET_BUF_FRAGS;
    }

    /* Mark the end of the fragment list */
    frag->frags = NULL;

    return buf;
}

struct net_buf *net_buf_ref(struct net_buf *buf)
{
    NET_BUF_ASSERT(buf);

    NET_BUF_DBG("buf %p (old) ref %u pool %p", buf, buf->ref, buf->pool);

    buf->ref++;
    return buf;
}

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
void net_buf_unref_debug(struct net_buf *buf, const char *func, int line)
#else
void net_buf_unref(struct net_buf *buf)
#endif
{
    NET_BUF_ASSERT(buf);

    while (buf) {
        struct net_buf *frags = buf->frags;
        struct net_buf_pool *pool;

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
        if (!buf->ref) {
            NET_BUF_ERR("%s():%d: buf %p double free", func, line,
                        buf);
            return;
        }
#endif
        NET_BUF_DBG("buf %p ref %u pool %p frags %p", buf, buf->ref,
                    buf->pool, buf->frags);

        /* Changed by Espressif. Add !buf->ref to avoid minus 0 */
        if (!buf->ref || --buf->ref > 0) {
            return;
        }

        buf->frags = NULL;

        pool = buf->pool;

        pool->uninit_count++;
#if defined(CONFIG_BLE_MESH_NET_BUF_POOL_USAGE)
        pool->avail_count++;
        NET_BUF_DBG("%s, pool %p, avail_count %d, uninit_count %d", __func__,
                    pool, pool->avail_count, pool->uninit_count);
        NET_BUF_ASSERT(pool->avail_count <= pool->buf_count);
#endif

        if (pool->destroy) {
            pool->destroy(buf);
        }

        buf = frags;
    }
}

static u8_t *fixed_data_alloc(struct net_buf *buf, size_t *size, s32_t timeout)
{
    struct net_buf_pool *pool = buf->pool;
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    *size = MIN(fixed->data_size, *size);

    return fixed->data_pool + fixed->data_size * net_buf_id(buf);
}

static void fixed_data_unref(struct net_buf *buf, u8_t *data)
{
    /* Nothing needed for fixed-size data pools */
}

const struct net_buf_data_cb net_buf_fixed_cb = {
    .alloc = fixed_data_alloc,
    .unref = fixed_data_unref,
};

static u8_t *data_alloc(struct net_buf *buf, size_t *size, s32_t timeout)
{
    struct net_buf_pool *pool = buf->pool;

    return pool->alloc->cb->alloc(buf, size, timeout);
}

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
struct net_buf *net_buf_alloc_len_debug(struct net_buf_pool *pool, size_t size,
                                        s32_t timeout, const char *func, int line)
#else
struct net_buf *net_buf_alloc_len(struct net_buf_pool *pool, size_t size,
                                  s32_t timeout)
#endif
{
    struct net_buf *buf = NULL;
    unsigned int key;
    int i;

    NET_BUF_ASSERT(pool);

    NET_BUF_DBG("%s, pool %p, uninit_count %d, buf_count %d", __func__,
                pool, pool->uninit_count, pool->buf_count);

    /* We need to lock interrupts temporarily to prevent race conditions
     * when accessing pool->uninit_count.
     */
    key = bt_mesh_irq_lock();

    /* If there are uninitialized buffers we're guaranteed to succeed
     * with the allocation one way or another.
     */
    if (pool->uninit_count) {
        /* Changed by Espressif. Use buf when buf->ref is 0 */
        for (i = pool->buf_count; i > 0; i--) {
            buf = pool_get_uninit(pool, i);
            if (!buf->ref) {
                bt_mesh_irq_unlock(key);
                goto success;
            }
        }
    }

    bt_mesh_irq_unlock(key);

    NET_BUF_ERR("%s, Failed to get free buffer", __func__);
    return NULL;

success:
    NET_BUF_DBG("allocated buf %p", buf);

    if (size) {
        buf->__buf = data_alloc(buf, &size, timeout);
        if (!buf->__buf) {
            NET_BUF_ERR("%s, Failed to allocate data", __func__);
            return NULL;
        }
    } else {
        NET_BUF_WARN("%s, Zero data size", __func__);
        buf->__buf = NULL;
    }

    buf->ref   = 1;
    buf->flags = 0;
    buf->frags = NULL;
    buf->size  = size;
    net_buf_reset(buf);

    pool->uninit_count--;
#if defined(CONFIG_BLE_MESH_NET_BUF_POOL_USAGE)
    pool->avail_count--;
    NET_BUF_ASSERT(pool->avail_count >= 0);
#endif

    return buf;
}

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
struct net_buf *net_buf_alloc_fixed_debug(struct net_buf_pool *pool,
        s32_t timeout, const char *func,
        int line)
{
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    return net_buf_alloc_len_debug(pool, fixed->data_size, timeout, func, line);
}
#else
struct net_buf *net_buf_alloc_fixed(struct net_buf_pool *pool, s32_t timeout)
{
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    return net_buf_alloc_len(pool, fixed->data_size, timeout);
}
#endif