/** @file
 *  @brief Buffer management.
 */

/*
 * Copyright (c) 2015 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_BUF_H_
#define _BLE_MESH_BUF_H_

#include <stddef.h>
#include "sys/cdefs.h"
#include "mesh_types.h"
#include "mesh_slist.h"
#include "mesh_kernel.h"
#include "mesh_util.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Unaligned access */
#define UNALIGNED_GET(p)                    \
__extension__ ({                            \
    struct  __attribute__((__packed__)) {   \
        __typeof__(*(p)) __v;               \
    } *__p = (__typeof__(__p)) (p);         \
    __p->__v;                               \
})

#define BLE_MESH_NET_BUF_USER_DATA_SIZE     4

/**
 * @brief Network buffer library
 * @defgroup net_buf Network Buffer Library
 * @ingroup networking
 * @{
 */

/* Alignment needed for various parts of the buffer definition */
#define __net_buf_align __aligned(sizeof(int))

/**
 *  @def NET_BUF_SIMPLE_DEFINE
 *  @brief Define a net_buf_simple stack variable.
 *
 *  This is a helper macro which is used to define a net_buf_simple object
 *  on the stack.
 *
 *  @param _name Name of the net_buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE(_name, _size) \
    u8_t net_buf_data_##_name[_size];       \
    struct net_buf_simple _name = {         \
        .data  = net_buf_data_##_name,      \
        .len   = 0,                         \
        .size  = _size,                     \
        .__buf = net_buf_data_##_name,      \
    }

/**
 * @def NET_BUF_SIMPLE_DEFINE_STATIC
 * @brief Define a static net_buf_simple variable.
 *
 * This is a helper macro which is used to define a static net_buf_simple
 * object.
 *
 * @param _name Name of the net_buf_simple object.
 * @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE_STATIC(_name, _size)  \
    static u8_t net_buf_data_##_name[_size];        \
    static struct net_buf_simple _name = {          \
        .data  = net_buf_data_##_name,              \
        .len   = 0,                                 \
        .size  = _size,                             \
        .__buf = net_buf_data_##_name,              \
    }

/**
 * @brief Simple network buffer representation.
 *
 * This is a simpler variant of the net_buf object (in fact net_buf uses
 * net_buf_simple internally). It doesn't provide any kind of reference
 * counting, user data, dynamic allocation, or in general the ability to
 * pass through kernel objects such as FIFOs.
 *
 * The main use of this is for scenarios where the meta-data of the normal
 * net_buf isn't needed and causes too much overhead. This could be e.g.
 * when the buffer only needs to be allocated on the stack or when the
 * access to and lifetime of the buffer is well controlled and constrained.
 */
struct net_buf_simple {
    /** Pointer to the start of data in the buffer. */
    u8_t *data;

    /** Length of the data behind the data pointer. */
    u16_t len;

    /** Amount of data that this buffer can store. */
    u16_t size;

    /** Start of the data storage. Not to be accessed directly
     *  (the data pointer should be used instead).
     */
    u8_t *__buf;
};

/**
 * @def NET_BUF_SIMPLE
 * @brief Define a net_buf_simple stack variable and get a pointer to it.
 *
 * This is a helper macro which is used to define a net_buf_simple object on
 * the stack and the get a pointer to it as follows:
 *
 * struct net_buf_simple *my_buf = NET_BUF_SIMPLE(10);
 *
 * After creating the object it needs to be initialized by calling
 * net_buf_simple_init().
 *
 * @param _size Maximum data storage for the buffer.
 *
 * @return Pointer to stack-allocated net_buf_simple object.
 */
#define NET_BUF_SIMPLE(_size)               \
    ((struct net_buf_simple *)(&(struct {   \
        struct net_buf_simple buf;          \
        u8_t data[_size] __net_buf_align;   \
    }) {                                    \
        .buf.size = _size,                  \
        .buf.__buf = NULL,                  \
    }))

/**
 * @brief Initialize a net_buf_simple object.
 *
 * This needs to be called after creating a net_buf_simple object using
 * the NET_BUF_SIMPLE macro.
 *
 * @param buf Buffer to initialize.
 * @param reserve_head Headroom to reserve.
 */
static inline void net_buf_simple_init(struct net_buf_simple *buf,
                                       size_t reserve_head)
{
    if (!buf->__buf) {
        buf->__buf = (u8_t *)buf + sizeof(*buf);
    }

    buf->data = buf->__buf + reserve_head;
    buf->len = 0;
}

/**
 * @brief Reset buffer
 *
 * Reset buffer data so it can be reused for other purposes.
 *
 * @param buf Buffer to reset.
 */
static inline void net_buf_simple_reset(struct net_buf_simple *buf)
{
    buf->len  = 0;
    buf->data = buf->__buf;
}

/**
 * @brief Prepare data to be added at the end of the buffer
 *
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to increment the length with.
 *
 * @return The original tail of the buffer.
 */
void *net_buf_simple_add(struct net_buf_simple *buf, size_t len);

/**
 * @brief Copy given number of bytes from memory to the end of the buffer
 *
 * Increments the data length of the  buffer to account for more data at the
 * end.
 *
 * @param buf Buffer to update.
 * @param mem Location of data to be added.
 * @param len Length of data to be added
 *
 * @return The original tail of the buffer.
 */
void *net_buf_simple_add_mem(struct net_buf_simple *buf, const void *mem,
                             size_t len);

/**
 * @brief Add (8-bit) byte at the end of the buffer
 *
 * Increments the data length of the  buffer to account for more data at the
 * end.
 *
 * @param buf Buffer to update.
 * @param val byte value to be added.
 *
 * @return Pointer to the value added
 */
u8_t *net_buf_simple_add_u8(struct net_buf_simple *buf, u8_t val);

/**
 * @brief Add 16-bit value at the end of the buffer
 *
 * Adds 16-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be added.
 */
void net_buf_simple_add_le16(struct net_buf_simple *buf, u16_t val);

/**
 * @brief Add 16-bit value at the end of the buffer
 *
 * Adds 16-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be added.
 */
void net_buf_simple_add_be16(struct net_buf_simple *buf, u16_t val);

/**
 * @brief Add 32-bit value at the end of the buffer
 *
 * Adds 32-bit value in little endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be added.
 */
void net_buf_simple_add_le32(struct net_buf_simple *buf, u32_t val);

/**
 * @brief Add 32-bit value at the end of the buffer
 *
 * Adds 32-bit value in big endian format at the end of buffer.
 * Increments the data length of a buffer to account for more data
 * at the end.
 *
 * @param buf Buffer to update.
 * @param val 32-bit value to be added.
 */
void net_buf_simple_add_be32(struct net_buf_simple *buf, u32_t val);

/**
 * @brief Push data to the beginning of the buffer.
 *
 * Modifies the data pointer and buffer length to account for more data
 * in the beginning of the buffer.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to add to the beginning.
 *
 * @return The new beginning of the buffer data.
 */
void *net_buf_simple_push(struct net_buf_simple *buf, size_t len);

/**
 * @brief Push 16-bit value to the beginning of the buffer
 *
 * Adds 16-bit value in little endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_le16(struct net_buf_simple *buf, u16_t val);

/**
 * @brief Push 16-bit value to the beginning of the buffer
 *
 * Adds 16-bit value in big endian format to the beginning of the
 * buffer.
 *
 * @param buf Buffer to update.
 * @param val 16-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_be16(struct net_buf_simple *buf, u16_t val);

/**
 * @brief Push 8-bit value to the beginning of the buffer
 *
 * Adds 8-bit value the beginning of the buffer.
 *
 * @param buf Buffer to update.
 * @param val 8-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_u8(struct net_buf_simple *buf, u8_t val);

/**
 * @brief Remove data from the beginning of the buffer.
 *
 * Removes data from the beginning of the buffer by modifying the data
 * pointer and buffer length.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to remove.
 *
 * @return New beginning of the buffer data.
 */
void *net_buf_simple_pull(struct net_buf_simple *buf, size_t len);

/**
 * @brief Remove data from the beginning of the buffer.
 *
 * Removes data from the beginning of the buffer by modifying the data
 * pointer and buffer length.
 *
 * @param buf Buffer to update.
 * @param len Number of bytes to remove.
 *
 * @return Pointer to the old location of the buffer data.
 */
void *net_buf_simple_pull_mem(struct net_buf_simple *buf, size_t len);

/**
 * @brief Remove a 8-bit value from the beginning of the buffer
 *
 * Same idea as with net_buf_simple_pull(), but a helper for operating
 * on 8-bit values.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return The 8-bit removed value
 */
u8_t net_buf_simple_pull_u8(struct net_buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 * Same idea as with net_buf_simple_pull(), but a helper for operating
 * on 16-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from little endian to host endian.
 */
u16_t net_buf_simple_pull_le16(struct net_buf_simple *buf);

/**
 * @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 * Same idea as with net_buf_simple_pull(), but a helper for operating
 * on 16-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 16-bit value converted from big endian to host endian.
 */
u16_t net_buf_simple_pull_be16(struct net_buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 * Same idea as with net_buf_simple_pull(), but a helper for operating
 * on 32-bit little endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from little endian to host endian.
 */
u32_t net_buf_simple_pull_le32(struct net_buf_simple *buf);

/**
 * @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 * Same idea as with net_buf_simple_pull(), but a helper for operating
 * on 32-bit big endian data.
 *
 * @param buf A valid pointer on a buffer.
 *
 * @return 32-bit value converted from big endian to host endian.
 */
u32_t net_buf_simple_pull_be32(struct net_buf_simple *buf);

/**
 * @brief Get the tail pointer for a buffer.
 *
 * Get a pointer to the end of the data in a buffer.
 *
 * @param buf Buffer.
 *
 * @return Tail pointer for the buffer.
 */
static inline u8_t *net_buf_simple_tail(struct net_buf_simple *buf)
{
    return buf->data + buf->len;
}

/**
 * @brief Check buffer headroom.
 *
 * Check how much free space there is in the beginning of the buffer.
 *
 * buf A valid pointer on a buffer
 *
 * @return Number of bytes available in the beginning of the buffer.
 */
size_t net_buf_simple_headroom(struct net_buf_simple *buf);

/**
 * @brief Check buffer tailroom.
 *
 * Check how much free space there is at the end of the buffer.
 *
 * @param buf A valid pointer on a buffer
 *
 * @return Number of bytes available at the end of the buffer.
 */
size_t net_buf_simple_tailroom(struct net_buf_simple *buf);

/**
 * @brief Parsing state of a buffer.
 *
 * This is used for temporarily storing the parsing state of a buffer
 * while giving control of the parsing to a routine which we don't
 * control.
 */
struct net_buf_simple_state {
    /** Offset of the data pointer from the beginning of the storage */
    u16_t offset;
    /** Length of data */
    u16_t len;
};

/**
 * @brief Save the parsing state of a buffer.
 *
 * Saves the parsing state of a buffer so it can be restored later.
 *
 * @param buf Buffer from which the state should be saved.
 * @param state Storage for the state.
 */
static inline void net_buf_simple_save(struct net_buf_simple *buf,
                                       struct net_buf_simple_state *state)
{
    state->offset = net_buf_simple_headroom(buf);
    state->len = buf->len;
}

/**
 * @brief Restore the parsing state of a buffer.
 *
 * Restores the parsing state of a buffer from a state previously stored
 * by net_buf_simple_save().
 *
 * @param buf Buffer to which the state should be restored.
 * @param state Stored state.
 */
static inline void net_buf_simple_restore(struct net_buf_simple *buf,
        struct net_buf_simple_state *state)
{
    buf->data = buf->__buf + state->offset;
    buf->len = state->len;
}

/**
 * @brief Initialize buffer with the given headroom.
 *
 * The buffer is not expected to contain any data when this API is called.
 *
 * @param buf Buffer to initialize.
 * @param reserve How much headroom to reserve.
 */
void net_buf_simple_reserve(struct net_buf_simple *buf, size_t reserve);

/**
 * Flag indicating that the buffer has associated fragments. Only used
 * internally by the buffer handling code while the buffer is inside a
 * FIFO, meaning this never needs to be explicitly set or unset by the
 * net_buf API user. As long as the buffer is outside of a FIFO, i.e.
 * in practice always for the user for this API, the buf->frags pointer
 * should be used instead.
 */
#define NET_BUF_FRAGS        BIT(0)

/**
 * @brief Network buffer representation.
 *
 * This struct is used to represent network buffers. Such buffers are
 * normally defined through the NET_BUF_POOL_*_DEFINE() APIs and allocated
 * using the net_buf_alloc() API.
 */
struct net_buf {
    union {
        /** Allow placing the buffer into sys_slist_t */
        sys_snode_t node;

        /** Fragments associated with this buffer. */
        struct net_buf *frags;
    };

    /** Reference count. */
    u8_t ref;

    /** Bit-field of buffer flags. */
    u8_t flags;

    /** Where the buffer should go when freed up. */
    struct net_buf_pool *pool;

    /* Union for convenience access to the net_buf_simple members, also
     * preserving the old API.
     */
    union {
        /* The ABI of this struct must match net_buf_simple */
        struct {
            /** Pointer to the start of data in the buffer. */
            u8_t *data;

            /** Length of the data behind the data pointer. */
            u16_t len;

            /** Amount of data that this buffer can store. */
            u16_t size;

            /** Start of the data storage. Not to be accessed
             *  directly (the data pointer should be used
             *  instead).
             */
            u8_t *__buf;
        };

        struct net_buf_simple b;
    };

    /** System metadata for this buffer. */
    u8_t user_data[BLE_MESH_NET_BUF_USER_DATA_SIZE] __net_buf_align;
};

struct net_buf_data_cb {
    u8_t * (*alloc)(struct net_buf *buf, size_t *size, s32_t timeout);
    u8_t * (*ref)(struct net_buf *buf, u8_t *data);
    void   (*unref)(struct net_buf *buf, u8_t *data);
};

struct net_buf_data_alloc {
    const struct net_buf_data_cb *cb;
    void *alloc_data;
};

struct net_buf_pool {
    /** Number of buffers in pool */
    const u16_t buf_count;

    /** Number of uninitialized buffers */
    u16_t uninit_count;

#if defined(CONFIG_BLE_MESH_NET_BUF_POOL_USAGE)
    /** Amount of available buffers in the pool. */
    s16_t avail_count;

    /** Total size of the pool. */
    const u16_t pool_size;

    /** Name of the pool. Used when printing pool information. */
    const char *name;
#endif /* CONFIG_BLE_MESH_NET_BUF_POOL_USAGE */

    /** Optional destroy callback when buffer is freed. */
    void (*const destroy)(struct net_buf *buf);

    /** Data allocation handlers. */
    const struct net_buf_data_alloc *alloc;

    /** Helper to access the start of storage (for net_buf_pool_init) */
    struct net_buf *const __bufs;
};

#if defined(CONFIG_BLE_MESH_NET_BUF_POOL_USAGE)
#define NET_BUF_POOL_INITIALIZER(_pool, _alloc, _bufs, _count, _destroy) \
    {                                                                   \
        .alloc = _alloc,                                                \
        .__bufs = (struct net_buf *)_bufs,                              \
        .buf_count = _count,                                            \
        .uninit_count = _count,                                         \
        .avail_count = _count,                                          \
        .destroy = _destroy,                                            \
        .name = STRINGIFY(_pool),                                       \
    }
#else
#define NET_BUF_POOL_INITIALIZER(_pool, _alloc, _bufs, _count, _destroy) \
    {                                                                   \
        .alloc = _alloc,                                                \
        .__bufs = (struct net_buf *)_bufs,                              \
        .buf_count = _count,                                            \
        .uninit_count = _count,                                         \
        .destroy = _destroy,                                            \
    }
#endif /* CONFIG_BLE_MESH_NET_BUF_POOL_USAGE */

struct net_buf_pool_fixed {
    size_t data_size;
    u8_t  *data_pool;
};

/** @cond INTERNAL_HIDDEN */
extern const struct net_buf_data_cb net_buf_fixed_cb;

/**
 * @def NET_BUF_POOL_FIXED_DEFINE
 * @brief Define a new pool for buffers based on fixed-size data
 *
 * Defines a net_buf_pool struct and the necessary memory storage (array of
 * structs) for the needed amount of buffers. After this, the buffers can be
 * accessed from the pool through net_buf_alloc. The pool is defined as a
 * static variable, so if it needs to be exported outside the current module
 * this needs to happen with the help of a separate pointer rather than an
 * extern declaration.
 *
 * The data payload of the buffers will be allocated from a byte array
 * of fixed sized chunks. This kind of pool does not support blocking on
 * the data allocation, so the timeout passed to net_buf_alloc will be
 * always treated as K_NO_WAIT when trying to allocate the data. This means
 * that allocation failures, i.e. NULL returns, must always be handled
 * cleanly.
 *
 * If provided with a custom destroy callback, this callback is
 * responsible for eventually calling net_buf_destroy() to complete the
 * process of returning the buffer to the pool.
 *
 * @param _name      Name of the pool variable.
 * @param _count     Number of buffers in the pool.
 * @param _data_size Maximum data payload per buffer.
 * @param _destroy   Optional destroy callback when buffer is freed.
 */
#define NET_BUF_POOL_FIXED_DEFINE(_name, _count, _data_size, _destroy)      \
    static struct net_buf net_buf_##_name[_count];                          \
    static u8_t net_buf_data_##_name[_count][_data_size];                   \
    static const struct net_buf_pool_fixed net_buf_fixed_##_name = {        \
        .data_size = _data_size,                                            \
        .data_pool = (u8_t *)net_buf_data_##_name,                          \
    };                                                                      \
    static const struct net_buf_data_alloc net_buf_fixed_alloc_##_name = {  \
        .cb = &net_buf_fixed_cb,                                            \
        .alloc_data = (void *)&net_buf_fixed_##_name,                       \
    };                                                                      \
    struct net_buf_pool _name __net_buf_align                               \
            __in_section(_net_buf_pool, static, _name) =                    \
        NET_BUF_POOL_INITIALIZER(_name, &net_buf_fixed_alloc_##_name,       \
                    net_buf_##_name, _count, _destroy)

/**
 * @def NET_BUF_POOL_DEFINE
 * @brief Define a new pool for buffers
 *
 * Defines a net_buf_pool struct and the necessary memory storage (array of
 * structs) for the needed amount of buffers. After this,the buffers can be
 * accessed from the pool through net_buf_alloc. The pool is defined as a
 * static variable, so if it needs to be exported outside the current module
 * this needs to happen with the help of a separate pointer rather than an
 * extern declaration.
 *
 * If provided with a custom destroy callback this callback is
 * responsible for eventually calling net_buf_destroy() to complete the
 * process of returning the buffer to the pool.
 *
 * @param _name     Name of the pool variable.
 * @param _count    Number of buffers in the pool.
 * @param _size     Maximum data size for each buffer.
 * @param _ud_size  Amount of user data space to reserve.
 * @param _destroy  Optional destroy callback when buffer is freed.
 */
#define NET_BUF_POOL_DEFINE(_name, _count, _size, _ud_size, _destroy)   \
    NET_BUF_POOL_FIXED_DEFINE(_name, _count, _size, _destroy)

/**
 * @brief Get a zero-based index for a buffer.
 *
 * This function will translate a buffer into a zero-based index,
 * based on its placement in its buffer pool. This can be useful if you
 * want to associate an external array of meta-data contexts with the
 * buffers of a pool.
 *
 * @param buf  Network buffer.
 *
 * @return Zero-based index for the buffer.
 */
int net_buf_id(struct net_buf *buf);

/**
 * @brief Allocate a new fixed buffer from a pool.
 *
 * @param pool Which pool to allocate the buffer from.
 * @param timeout Affects the action taken should the pool be empty.
 *        If K_NO_WAIT, then return immediately. If K_FOREVER, then
 *        wait as long as necessary. Otherwise, wait up to the specified
 *        number of milliseconds before timing out. Note that some types
 *        of data allocators do not support blocking (such as the HEAP
 *        type). In this case it's still possible for net_buf_alloc() to
 *        fail (return NULL) even if it was given K_FOREVER.
 *
 * @return New buffer or NULL if out of buffers.
 */
#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
struct net_buf *net_buf_alloc_fixed_debug(struct net_buf_pool *pool, s32_t timeout,
                                    const char *func, int line);
#define net_buf_alloc_fixed(_pool, _timeout) \
    net_buf_alloc_fixed_debug(_pool, _timeout, __func__, __LINE__)
#else
struct net_buf *net_buf_alloc_fixed(struct net_buf_pool *pool, s32_t timeout);
#endif

/**
 * @def net_buf_alloc
 *
 * @copydetails net_buf_alloc_fixed
 */
#define net_buf_alloc(pool, timeout) net_buf_alloc_fixed(pool, timeout)

/**
 *  @brief Reset buffer
 *
 *  Reset buffer data and flags so it can be reused for other purposes.
 *
 *  @param buf Buffer to reset.
 */
void net_buf_reset(struct net_buf *buf);

/**
 * @def net_buf_reserve
 * @brief Initialize buffer with the given headroom.
 *
 * The buffer is not expected to contain any data when this API is called.
 *
 * @param buf Buffer to initialize.
 * @param reserve How much headroom to reserve.
 */
#define net_buf_reserve(buf, reserve) net_buf_simple_reserve(&(buf)->b, reserve)

/**
 *  @brief Put a buffer into a list
 *
 *  Put a buffer to the end of a list. If the buffer contains follow-up
 *  fragments this function will take care of inserting them as well
 *  into the list.
 *
 *  @param list Which list to append the buffer to.
 *  @param buf Buffer.
 */
void net_buf_slist_put(sys_slist_t *list, struct net_buf *buf);

/**
 *  @brief Get a buffer from a list.
 *
 *  Get buffer from a list. If the buffer had any fragments, these will
 *  automatically be recovered from the list as well and be placed to
 *  the buffer's fragment list.
 *
 *  @param list Which list to take the buffer from.
 *
 *  @return New buffer or NULL if the FIFO is empty.
 */
struct net_buf *net_buf_slist_get(sys_slist_t *list);

/**
 *  @brief Decrements the reference count of a buffer.
 *
 *  Decrements the reference count of a buffer and puts it back into the
 *  pool if the count reaches zero.
 *
 *  @param buf A valid pointer on a buffer
 */
#if defined(CONFIG_BLE_MESH_NET_BUF_LOG)
void net_buf_unref_debug(struct net_buf *buf, const char *func, int line);
#define net_buf_unref(_buf) \
        net_buf_unref_debug(_buf, __func__, __LINE__)
#else
void net_buf_unref(struct net_buf *buf);
#endif

/**
 *  @brief Increment the reference count of a buffer.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return the buffer newly referenced
 */
struct net_buf *net_buf_ref(struct net_buf *buf);

/**
 *  @brief Get a pointer to the user data of a buffer.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return Pointer to the user data of the buffer.
 */
static inline void *net_buf_user_data(struct net_buf *buf)
{
    return (void *)buf->user_data;
}

/**
 *  @def net_buf_add
 *  @brief Prepare data to be added at the end of the buffer
 *
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param len Number of bytes to increment the length with.
 *
 *  @return The original tail of the buffer.
 */
#define net_buf_add(buf, len) net_buf_simple_add(&(buf)->b, len)

/**
 *  @def net_buf_add_mem
 *  @brief Copy bytes from memory to the end of the buffer
 *
 *  Copies the given number of bytes to the end of the buffer. Increments the
 *  data length of the  buffer to account for more data at the end.
 *
 *  @param buf Buffer to update.
 *  @param mem Location of data to be added.
 *  @param len Length of data to be added
 *
 *  @return The original tail of the buffer.
 */
#define net_buf_add_mem(buf, mem, len) net_buf_simple_add_mem(&(buf)->b, mem, len)

/**
 *  @def net_buf_add_u8
 *  @brief Add (8-bit) byte at the end of the buffer
 *
 *  Adds a byte at the end of the buffer. Increments the data length of
 *  the  buffer to account for more data at the end.
 *
 *  @param buf Buffer to update.
 *  @param val byte value to be added.
 *
 *  @return Pointer to the value added
 */
#define net_buf_add_u8(buf, val) net_buf_simple_add_u8(&(buf)->b, val)

/**
 *  @def net_buf_add_le16
 *  @brief Add 16-bit value at the end of the buffer
 *
 *  Adds 16-bit value in little endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be added.
 */
#define net_buf_add_le16(buf, val) net_buf_simple_add_le16(&(buf)->b, val)

/**
 *  @def net_buf_add_be16
 *  @brief Add 16-bit value at the end of the buffer
 *
 *  Adds 16-bit value in big endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be added.
 */
#define net_buf_add_be16(buf, val) net_buf_simple_add_be16(&(buf)->b, val)

/**
 *  @def net_buf_add_le32
 *  @brief Add 32-bit value at the end of the buffer
 *
 *  Adds 32-bit value in little endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 32-bit value to be added.
 */
#define net_buf_add_le32(buf, val) net_buf_simple_add_le32(&(buf)->b, val)

/**
 *  @def net_buf_add_be32
 *  @brief Add 32-bit value at the end of the buffer
 *
 *  Adds 32-bit value in big endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 32-bit value to be added.
 */
#define net_buf_add_be32(buf, val) net_buf_simple_add_be32(&(buf)->b, val)

/**
 *  @def net_buf_push
 *  @brief Push data to the beginning of the buffer.
 *
 *  Modifies the data pointer and buffer length to account for more data
 *  in the beginning of the buffer.
 *
 *  @param buf Buffer to update.
 *  @param len Number of bytes to add to the beginning.
 *
 *  @return The new beginning of the buffer data.
 */
#define net_buf_push(buf, len) net_buf_simple_push(&(buf)->b, len)

/**
 *  @def net_buf_push_le16
 *  @brief Push 16-bit value to the beginning of the buffer
 *
 *  Adds 16-bit value in little endian format to the beginning of the
 *  buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be pushed to the buffer.
 */
#define net_buf_push_le16(buf, val) net_buf_simple_push_le16(&(buf)->b, val)

/**
 *  @def net_buf_push_be16
 *  @brief Push 16-bit value to the beginning of the buffer
 *
 *  Adds 16-bit value in little endian format to the beginning of the
 *  buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be pushed to the buffer.
 */
#define net_buf_push_be16(buf, val) net_buf_simple_push_be16(&(buf)->b, val)

/**
 *  @def net_buf_push_u8
 *  @brief Push 8-bit value to the beginning of the buffer
 *
 *  Adds 8-bit value the beginning of the buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 8-bit value to be pushed to the buffer.
 */
#define net_buf_push_u8(buf, val) net_buf_simple_push_u8(&(buf)->b, val)

/**
 *  @def net_buf_pull
 *  @brief Remove data from the beginning of the buffer.
 *
 *  Removes data from the beginning of the buffer by modifying the data
 *  pointer and buffer length.
 *
 *  @param buf Buffer to update.
 *  @param len Number of bytes to remove.
 *
 *  @return New beginning of the buffer data.
 */
#define net_buf_pull(buf, len) net_buf_simple_pull(&(buf)->b, len)

/**
 *  @def net_buf_pull_u8
 *  @brief Remove a 8-bit value from the beginning of the buffer
 *
 *  Same idea as with net_buf_pull(), but a helper for operating on
 *  8-bit values.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return The 8-bit removed value
 */
#define net_buf_pull_u8(buf) net_buf_simple_pull_u8(&(buf)->b)

/**
 *  @def net_buf_pull_le16
 *  @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_pull(), but a helper for operating on
 *  16-bit little endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 16-bit value converted from little endian to host endian.
 */
#define net_buf_pull_le16(buf) net_buf_simple_pull_le16(&(buf)->b)

/**
 *  @def net_buf_pull_be16
 *  @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_pull(), but a helper for operating on
 *  16-bit big endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 16-bit value converted from big endian to host endian.
 */
#define net_buf_pull_be16(buf) net_buf_simple_pull_be16(&(buf)->b)

/**
 *  @def net_buf_pull_le32
 *  @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_pull(), but a helper for operating on
 *  32-bit little endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 32-bit value converted from little endian to host endian.
 */
#define net_buf_pull_le32(buf) net_buf_simple_pull_le32(&(buf)->b)

/**
 *  @def net_buf_pull_be32
 *  @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_pull(), but a helper for operating on
 *  32-bit big endian data.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return 32-bit value converted from big endian to host endian.
 */
#define net_buf_pull_be32(buf) net_buf_simple_pull_be32(&(buf)->b)

/**
 *  @def net_buf_tailroom
 *  @brief Check buffer tailroom.
 *
 *  Check how much free space there is at the end of the buffer.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return Number of bytes available at the end of the buffer.
 */
#define net_buf_tailroom(buf) net_buf_simple_tailroom(&(buf)->b)

/**
 *  @def net_buf_headroom
 *  @brief Check buffer headroom.
 *
 *  Check how much free space there is in the beginning of the buffer.
 *
 *  buf A valid pointer on a buffer
 *
 *  @return Number of bytes available in the beginning of the buffer.
 */
#define net_buf_headroom(buf) net_buf_simple_headroom(&(buf)->b)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_BUF_H_ */

