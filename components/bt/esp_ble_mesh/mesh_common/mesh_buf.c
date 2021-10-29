/*
 * SPDX-FileCopyrightText: 2015 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "mesh_common.h"

int net_buf_id(struct net_buf *buf)
{
    struct net_buf_pool *pool = buf->pool;

    return buf - pool->__bufs;
}

static inline struct net_buf *pool_get_uninit(struct net_buf_pool *pool,
                                              uint16_t uninit_count)
{
    struct net_buf *buf = NULL;

    buf = &pool->__bufs[pool->buf_count - uninit_count];

    buf->pool = pool;

    return buf;
}

void net_buf_simple_clone(const struct net_buf_simple *original,
                          struct net_buf_simple *clone)
{
    memcpy(clone, original, sizeof(struct net_buf_simple));
}

void *net_buf_simple_add(struct net_buf_simple *buf, size_t len)
{
    uint8_t *tail = net_buf_simple_tail(buf);

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

uint8_t *net_buf_simple_add_u8(struct net_buf_simple *buf, uint8_t val)
{
    uint8_t *u8 = NULL;

    NET_BUF_SIMPLE_DBG("buf %p val 0x%02x", buf, val);

    u8 = net_buf_simple_add(buf, 1);
    *u8 = val;

    return u8;
}

void net_buf_simple_add_le16(struct net_buf_simple *buf, uint16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le16(val, net_buf_simple_add(buf, sizeof(val)));
}

void net_buf_simple_add_be16(struct net_buf_simple *buf, uint16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be16(val, net_buf_simple_add(buf, sizeof(val)));
}

void net_buf_simple_add_le24(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le24(val, net_buf_simple_add(buf, 3));
}

void net_buf_simple_add_be24(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be24(val, net_buf_simple_add(buf, 3));
}

void net_buf_simple_add_le32(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le32(val, net_buf_simple_add(buf, sizeof(val)));
}

void net_buf_simple_add_be32(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be32(val, net_buf_simple_add(buf, sizeof(val)));
}

void net_buf_simple_add_le48(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_le48(val, net_buf_simple_add(buf, 6));
}

void net_buf_simple_add_be48(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_be48(val, net_buf_simple_add(buf, 6));
}

void net_buf_simple_add_le64(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_le64(val, net_buf_simple_add(buf, sizeof(val)));
}

void net_buf_simple_add_be64(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_be64(val, net_buf_simple_add(buf, sizeof(val)));
}

void *net_buf_simple_push(struct net_buf_simple *buf, size_t len)
{
    NET_BUF_SIMPLE_DBG("buf %p len %u", buf, len);

    NET_BUF_SIMPLE_ASSERT(net_buf_simple_headroom(buf) >= len);

    buf->data -= len;
    buf->len += len;
    return buf->data;
}

void net_buf_simple_push_le16(struct net_buf_simple *buf, uint16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le16(val, net_buf_simple_push(buf, sizeof(val)));
}

void net_buf_simple_push_be16(struct net_buf_simple *buf, uint16_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be16(val, net_buf_simple_push(buf, sizeof(val)));
}

void net_buf_simple_push_u8(struct net_buf_simple *buf, uint8_t val)
{
    uint8_t *data = net_buf_simple_push(buf, 1);

    *data = val;
}

void net_buf_simple_push_le24(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le24(val, net_buf_simple_push(buf, 3));
}

void net_buf_simple_push_be24(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be24(val, net_buf_simple_push(buf, 3));
}

void net_buf_simple_push_le32(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_le32(val, net_buf_simple_push(buf, sizeof(val)));
}

void net_buf_simple_push_be32(struct net_buf_simple *buf, uint32_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %u", buf, val);

    sys_put_be32(val, net_buf_simple_push(buf, sizeof(val)));
}

void net_buf_simple_push_le48(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_le48(val, net_buf_simple_push(buf, 6));
}

void net_buf_simple_push_be48(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_be48(val, net_buf_simple_push(buf, 6));
}

void net_buf_simple_push_le64(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_le64(val, net_buf_simple_push(buf, sizeof(val)));
}

void net_buf_simple_push_be64(struct net_buf_simple *buf, uint64_t val)
{
    NET_BUF_SIMPLE_DBG("buf %p val %" PRIu64, buf, val);

    sys_put_be64(val, net_buf_simple_push(buf, sizeof(val)));
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

uint8_t net_buf_simple_pull_u8(struct net_buf_simple *buf)
{
    uint8_t val = 0U;

    val = buf->data[0];
    net_buf_simple_pull(buf, 1);

    return val;
}

uint16_t net_buf_simple_pull_le16(struct net_buf_simple *buf)
{
    uint16_t val = 0U;

    val = UNALIGNED_GET((uint16_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le16_to_cpu(val);
}

uint16_t net_buf_simple_pull_be16(struct net_buf_simple *buf)
{
    uint16_t val = 0U;

    val = UNALIGNED_GET((uint16_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be16_to_cpu(val);
}

uint32_t net_buf_simple_pull_le24(struct net_buf_simple *buf)
{
    struct uint24 {
        uint32_t u24:24;
    } __packed val;

    val = UNALIGNED_GET((struct uint24 *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le24_to_cpu(val.u24);
}

uint32_t net_buf_simple_pull_be24(struct net_buf_simple *buf)
{
    struct uint24 {
        uint32_t u24:24;
    } __packed val;

    val = UNALIGNED_GET((struct uint24 *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be24_to_cpu(val.u24);
}

uint32_t net_buf_simple_pull_le32(struct net_buf_simple *buf)
{
    uint32_t val = 0U;

    val = UNALIGNED_GET((uint32_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le32_to_cpu(val);
}

uint32_t net_buf_simple_pull_be32(struct net_buf_simple *buf)
{
    uint32_t val = 0U;

    val = UNALIGNED_GET((uint32_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be32_to_cpu(val);
}

uint64_t net_buf_simple_pull_le48(struct net_buf_simple *buf)
{
    struct uint48 {
        uint64_t u48:48;
    } __packed val;

    val = UNALIGNED_GET((struct uint48 *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le48_to_cpu(val.u48);
}

uint64_t net_buf_simple_pull_be48(struct net_buf_simple *buf)
{
    struct uint48 {
        uint64_t u48:48;
    } __packed val;

    val = UNALIGNED_GET((struct uint48 *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be48_to_cpu(val.u48);
}

uint64_t net_buf_simple_pull_le64(struct net_buf_simple *buf)
{
    uint64_t val;

    val = UNALIGNED_GET((uint64_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_le64_to_cpu(val);
}

uint64_t net_buf_simple_pull_be64(struct net_buf_simple *buf)
{
    uint64_t val;

    val = UNALIGNED_GET((uint64_t *)buf->data);
    net_buf_simple_pull(buf, sizeof(val));

    return sys_be64_to_cpu(val);
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

void net_buf_simple_init_with_data(struct net_buf_simple *buf,
                                   void *data, size_t size)
{
    buf->__buf = data;
    buf->data  = data;
    buf->size  = size;
    buf->len   = size;
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
    struct net_buf *tail = NULL;

    NET_BUF_ASSERT(list);
    NET_BUF_ASSERT(buf);

    for (tail = buf; tail->frags; tail = tail->frags) {
        tail->flags |= NET_BUF_FRAGS;
    }

    bt_mesh_list_lock();
    sys_slist_append_list(list, &buf->node, &tail->node);
    bt_mesh_list_unlock();
}

struct net_buf *net_buf_slist_get(sys_slist_t *list)
{
    struct net_buf *buf = NULL, *frag = NULL;

    NET_BUF_ASSERT(list);

    bt_mesh_list_lock();
    buf = (void *)sys_slist_get(list);
    bt_mesh_list_unlock();

    if (!buf) {
        return NULL;
    }

    /* Get any fragments belonging to this buffer */
    for (frag = buf; (frag->flags & NET_BUF_FRAGS); frag = frag->frags) {
        bt_mesh_list_lock();
        frag->frags = (void *)sys_slist_get(list);
        bt_mesh_list_unlock();

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
        struct net_buf_pool *pool = NULL;

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
        NET_BUF_DBG("Unref, pool %p, avail_count %d, uninit_count %d",
                    pool, pool->avail_count, pool->uninit_count);
        NET_BUF_ASSERT(pool->avail_count <= pool->buf_count);
#endif

        if (pool->destroy) {
            pool->destroy(buf);
        }

        buf = frags;
    }
}

static uint8_t *fixed_data_alloc(struct net_buf *buf, size_t *size, int32_t timeout)
{
    struct net_buf_pool *pool = buf->pool;
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    *size = MIN(fixed->data_size, *size);

    return fixed->data_pool + fixed->data_size * net_buf_id(buf);
}

static void fixed_data_unref(struct net_buf *buf, uint8_t *data)
{
    /* Nothing needed for fixed-size data pools */
}

const struct net_buf_data_cb net_buf_fixed_cb = {
    .alloc = fixed_data_alloc,
    .unref = fixed_data_unref,
};

static uint8_t *data_alloc(struct net_buf *buf, size_t *size, int32_t timeout)
{
    struct net_buf_pool *pool = buf->pool;

    return pool->alloc->cb->alloc(buf, size, timeout);
}

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
struct net_buf *net_buf_alloc_len_debug(struct net_buf_pool *pool, size_t size,
                                        int32_t timeout, const char *func, int line)
#else
struct net_buf *net_buf_alloc_len(struct net_buf_pool *pool, size_t size,
                                  int32_t timeout)
#endif
{
    struct net_buf *buf = NULL;
    int i;

    NET_BUF_ASSERT(pool);

    NET_BUF_DBG("Alloc, pool %p, uninit_count %d, buf_count %d",
                pool, pool->uninit_count, pool->buf_count);

    /* We need to lock interrupts temporarily to prevent race conditions
     * when accessing pool->uninit_count.
     */
    bt_mesh_buf_lock();

    /* If there are uninitialized buffers we're guaranteed to succeed
     * with the allocation one way or another.
     */
    if (pool->uninit_count) {
        /* Changed by Espressif. Use buf when buf->ref is 0 */
        for (i = pool->buf_count; i > 0; i--) {
            buf = pool_get_uninit(pool, i);
            if (!buf->ref) {
                bt_mesh_buf_unlock();
                goto success;
            }
        }
    }

    bt_mesh_buf_unlock();

    NET_BUF_ERR("Out of free buffer, pool %p", pool);
    return NULL;

success:
    NET_BUF_DBG("allocated buf %p", buf);

    if (size) {
        buf->__buf = data_alloc(buf, &size, timeout);
        if (!buf->__buf) {
            NET_BUF_ERR("Out of data, buf %p", buf);
            return NULL;
        }
    } else {
        NET_BUF_WARN("Zero data size, buf %p", buf);
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
                                          int32_t timeout, const char *func,
                                          int line)
{
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    return net_buf_alloc_len_debug(pool, fixed->data_size, timeout, func, line);
}
#else
struct net_buf *net_buf_alloc_fixed(struct net_buf_pool *pool, int32_t timeout)
{
    const struct net_buf_pool_fixed *fixed = pool->alloc->alloc_data;

    return net_buf_alloc_len(pool, fixed->data_size, timeout);
}
#endif

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

struct net_buf *net_buf_frag_add(struct net_buf *head, struct net_buf *frag)
{
    NET_BUF_ASSERT(frag);

    if (!head) {
        return net_buf_ref(frag);
    }

    net_buf_frag_insert(net_buf_frag_last(head), frag);

    return head;
}

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
struct net_buf *net_buf_frag_del_debug(struct net_buf *parent,
                                       struct net_buf *frag,
                                       const char *func, int line)
#else
struct net_buf *net_buf_frag_del(struct net_buf *parent, struct net_buf *frag)
#endif
{
    struct net_buf *next_frag = NULL;

    NET_BUF_ASSERT(frag);

    if (parent) {
        NET_BUF_ASSERT(parent->frags);
        NET_BUF_ASSERT(parent->frags == frag);
        parent->frags = frag->frags;
    }

    next_frag = frag->frags;

    frag->frags = NULL;

#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
    net_buf_unref_debug(frag, func, line);
#else
    net_buf_unref(frag);
#endif

    return next_frag;
}

size_t net_buf_linearize(void *dst, size_t dst_len, struct net_buf *src,
                         size_t offset, size_t len)
{
    struct net_buf *frag = NULL;
    size_t to_copy = 0U;
    size_t copied = 0U;

    len = MIN(len, dst_len);

    frag = src;

    /* find the right fragment to start copying from */
    while (frag && offset >= frag->len) {
        offset -= frag->len;
        frag = frag->frags;
    }

    /* traverse the fragment chain until len bytes are copied */
    copied = 0;
    while (frag && len > 0) {
        to_copy = MIN(len, frag->len - offset);
        memcpy((uint8_t *)dst + copied, frag->data + offset, to_copy);

        copied += to_copy;

        /* to_copy is always <= len */
        len -= to_copy;
        frag = frag->frags;

        /* after the first iteration, this value will be 0 */
        offset = 0;
    }

    return copied;
}

/* This helper routine will append multiple bytes, if there is no place for
 * the data in current fragment then create new fragment and add it to
 * the buffer. It assumes that the buffer has at least one fragment.
 */
size_t net_buf_append_bytes(struct net_buf *buf, size_t len,
                            const void *value, int32_t timeout,
                            net_buf_allocator_cb allocate_cb, void *user_data)
{
    struct net_buf *frag = net_buf_frag_last(buf);
    size_t added_len = 0U;
    const uint8_t *value8 = value;

    do {
        uint16_t count = MIN(len, net_buf_tailroom(frag));

        net_buf_add_mem(frag, value8, count);
        len -= count;
        added_len += count;
        value8 += count;

        if (len == 0) {
            return added_len;
        }

        frag = allocate_cb(timeout, user_data);
        if (!frag) {
            return added_len;
        }

        net_buf_frag_add(buf, frag);
    } while (1);

    /* Unreachable */
    return 0;
}
