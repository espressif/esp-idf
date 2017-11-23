#ifndef FREERTOS_RINGBUF_H
#define FREERTOS_RINGBUF_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h" must appear in source files before "include ringbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <freertos/queue.h>

//An opaque handle for a ringbuff object.
typedef void * RingbufHandle_t;

/**
 * @brief The various types of buffer
 *
 * A ringbuffer instantiated by these functions essentially acts like a
 * FreeRTOS queue, with the difference that it's strictly FIFO and with
 * the main advantage that you can put in randomly-sized items. The capacity,
 * accordingly, isn't measured in the amount of items, but the amount of
 * memory that is used for storing the items. Dependent on the size of
 * the items, more or less of them will fit in the ring buffer.
 *
 * This ringbuffer tries to be efficient with memory: when inserting an item,
 * the item data will be copied to the ringbuffer memory. When retrieving
 * an item, however, a reference to ringbuffer memory will be returned.
 * The returned memory is guaranteed to be 32-bit aligned and contiguous.
 * The application can use this memory, but as long as it does, ringbuffer
 * writes that would write to this bit of memory will block.
 *
 * The requirement for items to be contiguous is slightly problematic when
 * the only way to place the next item would involve a wraparound from the end
 * to the beginning of the ringbuffer. This can be solved (or not) in a few ways,
 * see descriptions of possible ringbuf_type_t types below.
 *
 * The maximum size of an item will be affected by ringbuffer type.
 * When split items are allowed, it is acceptable to push items of
 * (buffer_size)-16 bytes into the buffer.
 * When it's not allowed, the maximum size is (buffer_size/2)-8 bytes.
 * The bytebuf can fill the entire buffer with data, it has no overhead.
 */
typedef enum {
	/** The insertion code will leave the room at the end of the ringbuffer
	 * unused and instead will put the entire item at the start of the ringbuffer,
	 * as soon as there is enough free space.
	 */
	RINGBUF_TYPE_NOSPLIT = 0,
	/** The insertion code will split the item in two items; one which fits
	 * in the space left at the end of the ringbuffer, one that contains
	 * the remaining data which is placed in the beginning.
	 * Two xRingbufferReceive calls will be needed to retrieve the data.
	 */
	RINGBUF_TYPE_ALLOWSPLIT,
	/** This is your conventional byte-based ringbuffer. It does have no
	 * overhead, but it has no item contiguousness either: a read will just
	 * give you the entire written buffer space, or the space up to the end
	 * of the buffer, and writes can be broken up in any way possible.
	 * Note that this type cannot do a 2nd read before returning the memory
	 * of the 1st.
	 */
	RINGBUF_TYPE_BYTEBUF
} ringbuf_type_t;


/**
 * @brief  Create a ring buffer
 *
 * @param  buf_length  Length of circular buffer, in bytes. Each entry will
 *                     take up its own length, plus a header that at the moment
 *                     is equal to sizeof(size_t).
 * @param  type  Type of ring buffer, see ringbuf_type_t.
 *
 * @return A RingbufHandle_t handle to the created ringbuffer, or NULL in case of error.
 */
RingbufHandle_t xRingbufferCreate(size_t buf_length, ringbuf_type_t type);

/**
 * @brief Create a ring buffer of type RINGBUF_TYPE_NOSPLIT for a fixed item_size
 *
 * This API is similar to xRingbufferCreate(), but it will internally allocate
 * additional space for the headers.
 *
 * @param item_size Size of each item to be put into the ring buffer
 * @param num_item Maximum number of items the buffer needs to hold simultaneously
 *
 * @return A RingbufHandle_t handle to the created ringbuffer, or NULL in case of error.
 */
RingbufHandle_t xRingbufferCreateNoSplit(size_t item_size, size_t num_item);

/**
 * @brief  Delete a ring buffer
 *
 * @param  ringbuf  Ring buffer to delete
 */
void vRingbufferDelete(RingbufHandle_t ringbuf);


/**
 * @brief  Get maximum size of an item that can be placed in the ring buffer
 *
 * @param  ringbuf  Ring buffer to query
 *
 * @return Maximum size, in bytes, of an item that can be placed in a ring buffer.
 */
size_t xRingbufferGetMaxItemSize(RingbufHandle_t ringbuf);

/**
 * @brief Get current free size available in the buffer
 *
 * This gives the real time free space available in the ring buffer. So basically,
 * this will be the maximum size of the entry that can be sent into the buffer.
 *
 * @note This API is not thread safe. So, if multiple threads are accessing the same
 * ring buffer, it is the application's responsibility to ensure atomic access to this
 * API and the subsequent Send
 *
 * @param ringbuf - Ring buffer to query
 *
 * @return Current free size, in bytes,  available for an entry
 */
size_t xRingbufferGetCurFreeSize(RingbufHandle_t ringbuf);

/**
 * @brief Check if the next item is wrapped
 *
 * This API tells if the next item that is available for a Receive is wrapped
 * or not. This is valid only if the ring buffer type is RINGBUF_TYPE_ALLOWSPLIT
 *
 * @note This API is not thread safe. So, if multiple threads are accessing the same
 * ring buffer, it is the application's responsibility to ensure atomic access to this
 * API and the subsequent Receive
 *
 * @param ringbuf - Ring buffer to query
 *
 * @return true if the next item is wrapped around
 * @return false if the next item is not wrapped
 */
bool xRingbufferIsNextItemWrapped(RingbufHandle_t ringbuf);

/**
 * @brief  Insert an item into the ring buffer
 *
 * @param  ringbuf  Ring buffer to insert the item into
 * @param  data  Pointer to data to insert. NULL is allowed if data_size is 0.
 * @param  data_size  Size of data to insert. A value of 0 is allowed.
 * @param  ticks_to_wait  Ticks to wait for room in the ringbuffer.
 *
 * @return
 *      - pdTRUE if succeeded
 *      - pdFALSE on time-out or when the buffer is larger than indicated
 *        by xRingbufferGetMaxItemSize(ringbuf).
 */
BaseType_t xRingbufferSend(RingbufHandle_t ringbuf, void *data, size_t data_size, TickType_t ticks_to_wait);


/**
 * @brief  Insert an item into the ring buffer from an ISR
 *
 * @param  ringbuf  Ring buffer to insert the item into
 * @param  data  Pointer to data to insert. NULL is allowed if data_size is 0.
 * @param  data_size  Size of data to insert. A value of 0 is allowed.
 * @param[out]  higher_prio_task_awoken  Value pointed to will be set to pdTRUE
 *                                  if the push woke up a higher priority task.
 *
 * @return pdTRUE if succeeded, pdFALSE when the ring buffer does not have space.
 */
BaseType_t xRingbufferSendFromISR(RingbufHandle_t ringbuf, void *data, size_t data_size, BaseType_t *higher_prio_task_awoken);

/**
 * @brief  Retrieve an item from the ring buffer
 *
 * @note A call to vRingbufferReturnItem() is required after this to free up
 * the data received.
 *
 * @param  ringbuf  Ring buffer to retrieve the item from
 * @param[out] item_size  Pointer to a variable to which the size of the
 *                        retrieved item will be written.
 * @param  ticks_to_wait  Ticks to wait for items in the ringbuffer.
 *
 * @return
 *      - pointer to the retrieved item on success; *item_size filled with
 *        the length of the item.
 *      - NULL on timeout, *item_size is untouched in that case.
 */
void *xRingbufferReceive(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait);


/**
 * @brief  Retrieve an item from the ring buffer from an ISR
 *
 * @note A call to vRingbufferReturnItemFromISR() is required after this to
 * free up the data received
 *
 * @param  ringbuf  Ring buffer to retrieve the item from
 * @param[out] item_size  Pointer to a variable to which the size of the
 *                        retrieved item will be written.
 *
 * @return
 *      - Pointer to the retrieved item on success; *item_size filled with
 *        the length of the item.
 *      - NULL when the ringbuffer is empty, *item_size is untouched in that case.
 */
void *xRingbufferReceiveFromISR(RingbufHandle_t ringbuf, size_t *item_size);


/**
 * @brief  Retrieve bytes from a ByteBuf type of ring buffer,
 * specifying the maximum amount of bytes to return
 *
 * @note A call to vRingbufferReturnItem() is required after this to free up
 * the data received.
 *
 * @param  ringbuf  Ring buffer to retrieve the item from
 * @param[out] item_size  Pointer to a variable to which the size
 *                        of the retrieved item will be written.
 * @param  ticks_to_wait  Ticks to wait for items in the ringbuffer.
 * @param  wanted_size  Maximum number of bytes to return.
 *
 * @return
 *      - Pointer to the retrieved item on success; *item_size filled with
 *        the length of the item.
 *      - NULL on timeout, *item_size is untouched in that case.
 */
void *xRingbufferReceiveUpTo(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait, size_t wanted_size);


/**
 * @brief  Retrieve bytes from a ByteBuf type of ring buffer,
 * specifying the maximum amount of bytes to return. Call this from an ISR.
 *
 * @note A call to vRingbufferReturnItemFromISR() is required after this
 * to free up the data received.
 *
 * @param  ringbuf  Ring buffer to retrieve the item from
 * @param[out] item_size  Pointer to a variable to which the size of the
 *                        retrieved item will be written.
 * @param  wanted_size  Maximum number of bytes to return.
 *
 * @return
 *      - Pointer to the retrieved item on success; *item_size filled with
 *        the length of the item.
 *      - NULL when the ringbuffer is empty, *item_size is untouched in that case.
 */
void *xRingbufferReceiveUpToFromISR(RingbufHandle_t ringbuf, size_t *item_size, size_t wanted_size);



/**
 * @brief  Return a previously-retrieved item to the ringbuffer
 *
 * @param  ringbuf  Ring buffer the item was retrieved from
 * @param  item  Item that was received earlier
 */
void vRingbufferReturnItem(RingbufHandle_t ringbuf, void *item);



/**
 * @brief  Return a previously-retrieved item to the ringbuffer from an ISR
 *
 * @param  ringbuf  Ring buffer the item was retrieved from
 * @param  item  Item that was received earlier
 * @param[out]  higher_prio_task_awoken  Value pointed to will be set to pdTRUE
 *                                  if the push woke up a higher priority task.
 */
void vRingbufferReturnItemFromISR(RingbufHandle_t ringbuf, void *item, BaseType_t *higher_prio_task_awoken);


/**
 * @brief  Add the ringbuffer to a queue set.
 *
 * This specifically adds the semaphore that indicates more space
 * has become available in the ringbuffer.
 *
 * @param  ringbuf  Ring buffer to add to the queue set
 * @param  xQueueSet  Queue set to add the ringbuffer to
 *
 * @return
 *      - pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferAddToQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Add the ringbuffer to a queue set.
 *
 * This specifically adds the semaphore that indicates something has been
 * written into the ringbuffer.
 *
 * @param  ringbuf  Ring buffer to add to the queue set
 * @param  xQueueSet  Queue set to add the ringbuffer to
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferAddToQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Remove the ringbuffer from a queue set.
 *
 * This specifically removes the semaphore that indicates more space
 * has become available in the ringbuffer.
 *
 * @param  ringbuf  Ring buffer to remove from the queue set
 * @param  xQueueSet  Queue set to remove the ringbuffer from
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Remove the ringbuffer from a queue set.
 *
 * This specifically removes the semaphore that indicates something
 * has been written to the ringbuffer.
 *
 * @param  ringbuf  Ring buffer to remove from the queue set
 * @param  xQueueSet  Queue set to remove the ringbuffer from
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferRemoveFromQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Debugging function to print the internal pointers in the ring buffer
 *
 * @param  ringbuf  Ring buffer to show
 */
void xRingbufferPrintInfo(RingbufHandle_t ringbuf);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_RINGBUF_H */

