/** @file
 *  @brief Buffer management.
 */

/*
 * Copyright (c) 2015 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __NET_BUF_H
#define __NET_BUF_H

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
#define UNALIGNED_GET(p)                        \
__extension__ ({                            \
    struct  __attribute__((__packed__)) {               \
        __typeof__(*(p)) __v;                   \
    } *__p = (__typeof__(__p)) (p);                 \
    __p->__v;                           \
})

/**
 * @brief Network buffer library
 * @defgroup net_buf Network Buffer Library
 * @ingroup networking
 * @{
 */

/* Alignment needed for various parts of the buffer definition */
#define __net_buf_align __aligned(sizeof(int))

/** @def NET_BUF_SIMPLE
 *  @brief Define a net_buf_simple stack variable and get a pointer to it.
 *
 *  This is a helper macro which is used to define a net_buf_simple object on
 *  the stack and the get a pointer to it as follows:
 *
 *  struct net_buf_simple *my_buf = NET_BUF_SIMPLE(10);
 *
 *  After creating the object it needs to be initialized by calling
 *  net_buf_simple_init().
 *
 *  @param _size Maximum data storage for the buffer.
 *
 *  @return Pointer to stack-allocated net_buf_simple object.
 */
#define NET_BUF_SIMPLE(_size)                        \
    ((struct net_buf_simple *)(&(struct {        \
        struct net_buf_simple buf;           \
        u8_t data[_size] __net_buf_align; \
    }) {                                         \
        .buf.size = _size,                   \
    }))

/** @brief Simple network buffer representation.
 *
 *  This is a simpler variant of the net_buf object (in fact net_buf uses
 *  net_buf_simple internally). It doesn't provide any kind of reference
 *  counting, user data, dynamic allocation, or in general the ability to
 *  pass through kernel objects such as FIFOs.
 *
 *  The main use of this is for scenarios where the meta-data of the normal
 *  net_buf isn't needed and causes too much overhead. This could be e.g.
 *  when the buffer only needs to be allocated on the stack or when the
 *  access to and lifetime of the buffer is well controlled and constrained.
 *
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
    u8_t __buf[0] __net_buf_align;
};

/** @def NET_BUF_SIMPLE_DEFINE_STATIC
 *  @brief Define a static net_buf_simple variable.
 *
 *  This is a helper macro which is used to define a static net_buf_simple
 *  object.
 *
 *  @param _name Name of the net_buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE_STATIC(_name, _size)        \
    static __noinit u8_t net_buf_data_##_name[_size]; \
    static struct net_buf_simple _name = {            \
        .data   = net_buf_data_##_name,           \
        .len    = 0,                              \
        .size   = _size,                          \
        .__buf  = net_buf_data_##_name,           \
    }

/** @brief Initialize a net_buf_simple object.
 *
 *  This needs to be called after creating a net_buf_simple object e.g. using
 *  the NET_BUF_SIMPLE macro.
 *
 *  @param buf Buffer to initialize.
 *  @param reserve_head Headroom to reserve.
 */
static inline void net_buf_simple_init(struct net_buf_simple *buf,
                                       size_t reserve_head)
{
    buf->data = buf->__buf + reserve_head;
    buf->len = 0;
}

/**
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
void *net_buf_simple_add(struct net_buf_simple *buf, size_t len);

/**
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
void *net_buf_simple_add_mem(struct net_buf_simple *buf, const void *mem,
                             size_t len);

/**
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
u8_t *net_buf_simple_add_u8(struct net_buf_simple *buf, u8_t val);

/**
 *  @brief Add 16-bit value at the end of the buffer
 *
 *  Adds 16-bit value in little endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be added.
 */
void net_buf_simple_add_le16(struct net_buf_simple *buf, u16_t val);

/**
 *  @brief Add 16-bit value at the end of the buffer
 *
 *  Adds 16-bit value in big endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be added.
 */
void net_buf_simple_add_be16(struct net_buf_simple *buf, u16_t val);

/**
 *  @brief Add 32-bit value at the end of the buffer
 *
 *  Adds 32-bit value in little endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 32-bit value to be added.
 */
void net_buf_simple_add_le32(struct net_buf_simple *buf, u32_t val);

/**
 *  @brief Add 32-bit value at the end of the buffer
 *
 *  Adds 32-bit value in big endian format at the end of buffer.
 *  Increments the data length of a buffer to account for more data
 *  at the end.
 *
 *  @param buf Buffer to update.
 *  @param val 32-bit value to be added.
 */
void net_buf_simple_add_be32(struct net_buf_simple *buf, u32_t val);

/**
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
void *net_buf_simple_push(struct net_buf_simple *buf, size_t len);

/**
 *  @brief Push 16-bit value to the beginning of the buffer
 *
 *  Adds 16-bit value in little endian format to the beginning of the
 *  buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_le16(struct net_buf_simple *buf, u16_t val);

/**
 *  @brief Push 16-bit value to the beginning of the buffer
 *
 *  Adds 16-bit value in big endian format to the beginning of the
 *  buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 16-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_be16(struct net_buf_simple *buf, u16_t val);

/**
 *  @brief Push 8-bit value to the beginning of the buffer
 *
 *  Adds 8-bit value the beginning of the buffer.
 *
 *  @param buf Buffer to update.
 *  @param val 8-bit value to be pushed to the buffer.
 */
void net_buf_simple_push_u8(struct net_buf_simple *buf, u8_t val);

/**
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
void *net_buf_simple_pull(struct net_buf_simple *buf, size_t len);

/**
 *  @brief Remove a 8-bit value from the beginning of the buffer
 *
 *  Same idea as with net_buf_simple_pull(), but a helper for operating
 *  on 8-bit values.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return The 8-bit removed value
 */
u8_t net_buf_simple_pull_u8(struct net_buf_simple *buf);

/**
 *  @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_simple_pull(), but a helper for operating
 *  on 16-bit little endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 16-bit value converted from little endian to host endian.
 */
u16_t net_buf_simple_pull_le16(struct net_buf_simple *buf);

/**
 *  @brief Remove and convert 16 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_simple_pull(), but a helper for operating
 *  on 16-bit big endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 16-bit value converted from big endian to host endian.
 */
u16_t net_buf_simple_pull_be16(struct net_buf_simple *buf);

/**
 *  @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_simple_pull(), but a helper for operating
 *  on 32-bit little endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 32-bit value converted from little endian to host endian.
 */
u32_t net_buf_simple_pull_le32(struct net_buf_simple *buf);

/**
 *  @brief Remove and convert 32 bits from the beginning of the buffer.
 *
 *  Same idea as with net_buf_simple_pull(), but a helper for operating
 *  on 32-bit big endian data.
 *
 *  @param buf A valid pointer on a buffer.
 *
 *  @return 32-bit value converted from big endian to host endian.
 */
u32_t net_buf_simple_pull_be32(struct net_buf_simple *buf);

/**
 *  @brief Get the tail pointer for a buffer.
 *
 *  Get a pointer to the end of the data in a buffer.
 *
 *  @param buf Buffer.
 *
 *  @return Tail pointer for the buffer.
 */
static inline u8_t *net_buf_simple_tail(struct net_buf_simple *buf)
{
    return buf->data + buf->len;
}

/**
 *  @brief Check buffer headroom.
 *
 *  Check how much free space there is in the beginning of the buffer.
 *
 *  buf A valid pointer on a buffer
 *
 *  @return Number of bytes available in the beginning of the buffer.
 */
size_t net_buf_simple_headroom(struct net_buf_simple *buf);

/**
 *  @brief Check buffer tailroom.
 *
 *  Check how much free space there is at the end of the buffer.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return Number of bytes available at the end of the buffer.
 */
size_t net_buf_simple_tailroom(struct net_buf_simple *buf);

/**
 *  @brief Parsing state of a buffer.
 *
 *  This is used for temporarily storing the parsing state of a buffer
 *  while giving control of the parsing to a routine which we don't
 *  control.
 */
struct net_buf_simple_state {
    /** Offset of the data pointer from the beginning of the storage */
    u16_t offset;
    /** Length of data */
    u16_t len;
};

/**
 *  @brief Save the parsing state of a buffer.
 *
 *  Saves the parsing state of a buffer so it can be restored later.
 *
 *  @param buf Buffer from which the state should be saved.
 *  @param state Storage for the state.
 */
static inline void net_buf_simple_save(struct net_buf_simple *buf,
                                       struct net_buf_simple_state *state)
{
    state->offset = net_buf_simple_headroom(buf);
    state->len = buf->len;
}

/**
 *  @brief Restore the parsing state of a buffer.
 *
 *  Restores the parsing state of a buffer from a state previously stored
 *  by net_buf_simple_save().
 *
 *  @param buf Buffer to which the state should be restored.
 *  @param state Stored state.
 */
static inline void net_buf_simple_restore(struct net_buf_simple *buf,
        struct net_buf_simple_state *state)
{
    buf->data = buf->__buf + state->offset;
    buf->len = state->len;
}

/** Flag indicating that the buffer has associated fragments. Only used
  * internally by the buffer handling code while the buffer is inside a
  * FIFO, meaning this never needs to be explicitly set or unset by the
  * net_buf API user. As long as the buffer is outside of a FIFO, i.e.
  * in practice always for the user for this API, the buf->frags pointer
  * should be used instead.
  */
#define NET_BUF_FRAGS        BIT(0)

/** @brief Network buffer representation.
  *
  * This struct is used to represent network buffers. Such buffers are
  * normally defined through the NET_BUF_POOL_DEFINE() API and allocated
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

    /* Change by Espressif. Net buf index */
    u8_t index;

    /** Bit-field of buffer flags. */
    u8_t flags;

    /** Where the buffer should go when freed up. */
    struct net_buf_pool *pool_id;

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
        };

        struct net_buf_simple b;
    };

    /** Start of the data storage. Not to be accessed directly
     *  (the data pointer should be used instead).
     */
    u8_t __buf[0] __net_buf_align;

    /** After __buf (as given by the "size" field, which can be 0),
     *  there may be so-called "user data", which is actually a system
     *  metadata for this buffer. This area can be accessed using
     *  net_buf_user_data(). (Its size is equal to
     *  this->pool->user_data_size.)
     */
};

struct net_buf_pool {
    /** LIFO to place the buffer into when free */
    struct k_lifo free;

    /** Number of buffers in pool */
    const u16_t buf_count;

    /** Number of uninitialized buffers */
    u16_t uninit_count;

    /** Data size of each buffer in the pool */
    const u16_t buf_size;

    /** Size of the user data associated with each buffer. */
    const u16_t user_data_size;

#if defined(CONFIG_NET_BUF_POOL_USAGE)
    /** Amount of available buffers in the pool. */
    s16_t avail_count;

    /** Total size of the pool. */
    const u16_t pool_size;

    /** Name of the pool. Used when printing pool information. */
    const char *name;
#endif /* CONFIG_NET_BUF_POOL_USAGE */

    /** Optional destroy callback when buffer is freed. */
    void (*const destroy)(struct net_buf *buf);

    /** Helper to access the start of storage (for net_buf_pool_init) */
    struct net_buf *const __bufs;
};

#if defined(CONFIG_NET_BUF_POOL_USAGE)
#define NET_BUF_POOL_INITIALIZER(_pool, _bufs, _count, _size, _ud_size,      \
                 _destroy)                                   \
    {                                                                    \
        .free = _K_LIFO_INITIALIZER(_pool.free),                      \
        .__bufs = (struct net_buf *)_bufs,                           \
        .buf_count = _count,                                         \
        .uninit_count = _count,                                      \
        .avail_count = _count,                                       \
        .pool_size = sizeof(_net_buf_##_pool),                  \
        .buf_size = _size,                                           \
        .user_data_size = _ud_size,                                  \
        .destroy = _destroy,                                         \
        .name = STRINGIFY(_pool),                                    \
    }
#else
#define NET_BUF_POOL_INITIALIZER(_pool, _bufs, _count, _size, _ud_size,      \
                 _destroy)                                   \
    {                                                                    \
        .free = _K_LIFO_INITIALIZER(_pool.free),                      \
        .__bufs = (struct net_buf *)_bufs,                           \
        .buf_count = _count,                                         \
        .uninit_count = _count,                                      \
        .buf_size = _size,                                           \
        .user_data_size = _ud_size,                                  \
        .destroy = _destroy,                                         \
    }
#endif /* CONFIG_NET_BUF_POOL_USAGE */

/** @def NET_BUF_POOL_DEFINE
 *  @brief Define a new pool for buffers
 *
 *  Defines a net_buf_pool struct and the necessary memory storage (array of
 *  structs) for the needed amount of buffers. After this,the buffers can be
 *  accessed from the pool through net_buf_alloc. The pool is defined as a
 *  static variable, so if it needs to be exported outside the current module
 *  this needs to happen with the help of a separate pointer rather than an
 *  extern declaration.
 *
 *  If provided with a custom destroy callback this callback is
 *  responsible for eventually calling net_buf_destroy() to complete the
 *  process of returning the buffer to the pool.
 *
 *  @param _name     Name of the pool variable.
 *  @param _count    Number of buffers in the pool.
 *  @param _size     Maximum data size for each buffer.
 *  @param _ud_size  Amount of user data space to reserve.
 *  @param _destroy  Optional destroy callback when buffer is freed.
 */
/* ESP Toolchain doesn't support section */
#define NET_BUF_POOL_DEFINE(_name, _count, _size, _ud_size, _destroy)        \
    static struct {                                                          \
        struct net_buf buf;                                                  \
        u8_t data[_size] __net_buf_align;                                    \
        u8_t ud[ROUND_UP(_ud_size, 4)] __net_buf_align;                      \
    } _net_buf_##_name[_count];                                              \
    struct net_buf_pool _name __net_buf_align                                \
            __in_section(_net_buf_pool, static, _name) =                     \
        NET_BUF_POOL_INITIALIZER(_name, _net_buf_##_name,                    \
                     _count, _size, _ud_size, _destroy)


/**
 *  @brief Looks up a pool based on its ID.
 *
 *  @param id Pool ID (e.g. from buf->pool_id).
 *
 *  @return Pointer to pool.
 */
struct net_buf_pool *net_buf_pool_get(int id);

/**
 *  @brief Get a zero-based index for a buffer.
 *
 *  This function will translate a buffer into a zero-based index,
 *  based on its placement in its buffer pool. This can be useful if you
 *  want to associate an external array of meta-data contexts with the
 *  buffers of a pool.
 *
 *  @param buf  Network buffer.
 *
 *  @return Zero-based index for the buffer.
 */
int net_buf_id(struct net_buf *buf);

/**
 *  @brief Allocate a new buffer from a pool.
 *
 *  Allocate a new buffer from a pool.
 *
 *  @param pool Which pool to allocate the buffer from.
 *  @param timeout Affects the action taken should the pool be empty.
 *         If K_NO_WAIT, then return immediately. If K_FOREVER, then
 *         wait as long as necessary. Otherwise, wait up to the specified
 *         number of milliseconds before timing out.
 *
 *  @return New buffer or NULL if out of buffers.
 */
#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *net_buf_alloc_debug(struct net_buf_pool *pool, s32_t timeout,
                                    const char *func, int line);
#define net_buf_alloc(_pool, _timeout) \
    net_buf_alloc_debug(_pool, _timeout, __func__, __LINE__)
#else
struct net_buf *net_buf_alloc(struct net_buf_pool *pool, s32_t timeout);
#endif

/**
 *  @brief Get a buffer from a FIFO.
 *
 *  Get buffer from a FIFO.
 *
 *  @param fifo Which FIFO to take the buffer from.
 *  @param timeout Affects the action taken should the FIFO be empty.
 *         If K_NO_WAIT, then return immediately. If K_FOREVER, then wait as
 *         long as necessary. Otherwise, wait up to the specified number of
 *         milliseconds before timing out.
 *
 *  @return New buffer or NULL if the FIFO is empty.
 */
#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *net_buf_get_debug(struct k_fifo *fifo, s32_t timeout,
                                  const char *func, int line);
#define net_buf_get(_fifo, _timeout) \
    net_buf_get_debug(_fifo, _timeout, __func__, __LINE__)
#else
struct net_buf *net_buf_get(struct k_fifo *fifo, s32_t timeout);
#endif

/**
 *  @brief Destroy buffer from custom destroy callback
 *
 *  This helper is only intended to be used from custom destroy callbacks.
 *  If no custom destroy callback is given to NET_BUF_POOL_DEFINE() then
 *  there is no need to use this API.
 *
 *  @param buf Buffer to destroy.
 */
static inline void net_buf_destroy(struct net_buf *buf)
{
    //struct net_buf_pool *pool = net_buf_pool_get(buf->pool_id);
    // TODO: should replace the following funcion in our system
    //k_lifo_put(&pool->free, buf);
}

/**
 *  @brief Reset buffer
 *
 *  Reset buffer data and flags so it can be reused for other purposes.
 *
 *  @param buf Buffer to reset.
 */
void net_buf_reset(struct net_buf *buf);

/**
 *  @brief Initialize buffer with the given headroom.
 *
 *  Initializes a buffer with a given headroom. The buffer is not expected to
 *  contain any data when this API is called.
 *
 *  @param buf Buffer to initialize.
 *  @param reserve How much headroom to reserve.
 */
void net_buf_reserve(struct net_buf *buf, size_t reserve);

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
 *  @brief Put a buffer into a FIFO
 *
 *  Put a buffer to the end of a FIFO. If the buffer contains follow-up
 *  fragments this function will take care of inserting them as well
 *  into the FIFO.
 *
 *  @param fifo Which FIFO to put the buffer to.
 *  @param buf Buffer.
 */
void net_buf_put(struct k_fifo *fifo, struct net_buf *buf);

/**
 *  @brief Decrements the reference count of a buffer.
 *
 *  Decrements the reference count of a buffer and puts it back into the
 *  pool if the count reaches zero.
 *
 *  @param buf A valid pointer on a buffer
 */
#if defined(CONFIG_NET_BUF_LOG)
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
 *  @brief Duplicate buffer
 *
 *  Duplicate given buffer including any data and headers currently stored.
 *
 *  @param buf A valid pointer on a buffer
 *  @param timeout Affects the action taken should the pool be empty.
 *         If K_NO_WAIT, then return immediately. If K_FOREVER, then
 *         wait as long as necessary. Otherwise, wait up to the specified
 *         number of milliseconds before timing out.
 *
 *  @return Duplicated buffer or NULL if out of buffers.
 */
struct net_buf *net_buf_clone(struct net_buf *buf, s32_t timeout);

/**
 *  @brief Get a pointer to the user data of a buffer.
 *
 *  @param buf A valid pointer on a buffer
 *
 *  @return Pointer to the user data of the buffer.
 */
static inline void *net_buf_user_data(struct net_buf *buf)
{
    return (void *)ROUND_UP((buf->__buf + buf->size), sizeof(int));
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
#define net_buf_add_mem(buf, mem, len) net_buf_simple_add_mem(&(buf)->b, \
                                  mem, len)

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
 *  @def net_buf_tail
 *  @brief Get the tail pointer for a buffer.
 *
 *  Get a pointer to the end of the data in a buffer.
 *
 *  @param buf Buffer.
 *
 *  @return Tail pointer for the buffer.
 */
#define net_buf_tail(buf) net_buf_simple_tail(&(buf)->b)

/** @brief Find the last fragment in the fragment list.
 *
 * @return Pointer to last fragment in the list.
 */
struct net_buf *net_buf_frag_last(struct net_buf *frags);

/** @brief Insert a new fragment to a chain of bufs.
 *
 *  Insert a new fragment into the buffer fragments list after the parent.
 *
 *  Note: This function takes ownership of the fragment reference so the
 *  caller is not required to unref.
 *
 *  @param parent Parent buffer/fragment.
 *  @param frag Fragment to insert.
 */
void net_buf_frag_insert(struct net_buf *parent, struct net_buf *frag);

/** @brief Add a new fragment to the end of a chain of bufs.
 *
 *  Append a new fragment into the buffer fragments list.
 *
 *  Note: This function takes ownership of the fragment reference so the
 *  caller is not required to unref.
 *
 *  @param head Head of the fragment chain.
 *  @param frag Fragment to add.
 *
 *  @return New head of the fragment chain. Either head (if head
 *          was non-NULL) or frag (if head was NULL).
 */
struct net_buf *net_buf_frag_add(struct net_buf *head, struct net_buf *frag);

/** @brief Delete existing fragment from a chain of bufs.
 *
 *  @param parent Parent buffer/fragment, or NULL if there is no parent.
 *  @param frag Fragment to delete.
 *
 *  @return Pointer to the buffer following the fragment, or NULL if it
 *          had no further fragments.
 */
#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *net_buf_frag_del_debug(struct net_buf *parent,
                                       struct net_buf *frag,
                                       const char *func, int line);
#define net_buf_frag_del(_parent, _frag) \
    net_buf_frag_del_debug(_parent, _frag, __func__, __LINE__)
#else
struct net_buf *net_buf_frag_del(struct net_buf *parent, struct net_buf *frag);
#endif

/** @brief Calculate amount of bytes stored in fragments.
 *
 *  Calculates the total amount of data stored in the given buffer and the
 *  fragments linked to it.
 *
 *  @param buf Buffer to start off with.
 *
 *  @return Number of bytes in the buffer and its fragments.
 */
static inline size_t net_buf_frags_len(struct net_buf *buf)
{
    size_t bytes = 0;

    while (buf) {
        bytes += buf->len;
        buf = buf->frags;
    }

    return bytes;
}

/**
 * @}
 */

#ifdef __cplusplus
}
#endif




#endif /* __NET_BUF_H */

