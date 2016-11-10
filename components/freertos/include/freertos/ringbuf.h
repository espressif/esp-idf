#ifndef FREERTOS_RINGBUF_H
#define FREERTOS_RINGBUF_H

/*
Header definitions for a FreeRTOS ringbuffer object

A ringbuffer instantiated by these functions essentially acts like a FreeRTOS queue, with the
difference that it's strictly FIFO and with the main advantage that you can put in randomly-sized
items. The capacity, accordingly, isn't measured in the amount of items, but the amount of memory
that is used for storing the items. Dependent on the size of the items, more or less of them will 
fit in the ring buffer.

This ringbuffer tries to be efficient with memory: when inserting an item, the item data will
be copied to the ringbuffer memory. When retrieving an item, however, a reference to ringbuffer
memory will be returned. The returned memory is guaranteed to be 32-bit aligned and contiguous.
The application can use this memory, but as long as it does, ringbuffer writes that would write
to this bit of memory will block.

The requirement for items to be contiguous is slightly problematic when the only way to place
the next item would involve a wraparound from the end to the beginning of the ringbuffer. This can
be solved (or not) in a few ways:
- type = RINGBUF_TYPE_ALLOWSPLIT: The insertion code will split the item in two items; one which fits
in the space left at the end of the ringbuffer, one that contains the remaining data which is placed
in the beginning. Two xRingbufferReceive calls will be needed to retrieve the data.
- type = RINGBUF_TYPE_NOSPLIT: The insertion code will leave the room at the end of the ringbuffer
unused and instead will put the entire item at the start of the ringbuffer, as soon as there is 
enough free space.
- type = RINGBUF_TYPE_BYTEBUF: This is your conventional byte-based ringbuffer. It does have no
overhead, but it has no item contiguousness either: a read will just give you the entire written
buffer space, or the space up to the end of the buffer, and writes can be broken up in any way 
possible. Note that this type cannot do a 2nd read before returning the memory of the 1st.

The maximum size of an item will be affected by this decision. When split items are allowed, it's
acceptable to push items of (buffer_size)-16 bytes into the buffer. When it's not allowed, the
maximum size is (buffer_size/2)-8 bytes. The bytebuf can fill the entire buffer with data, it has
no overhead.
*/

//An opaque handle for a ringbuff object.
typedef void * RingbufHandle_t;

//The various types of buffer
typedef enum {
	RINGBUF_TYPE_NOSPLIT = 0,
	RINGBUF_TYPE_ALLOWSPLIT,
	RINGBUF_TYPE_BYTEBUF
} ringbuf_type_t;


/**
 * @brief  Create a ring buffer
 *
 * @param  buf_length : Length of circular buffer, in bytes. Each entry will take up its own length, plus a header
 *                      that at the moment is equal to sizeof(size_t).
 * @param allow_split_items : pdTRUE if it is acceptable that item data is inserted as two
 *                            items instead of one.
 *
 * @return A RingbufHandle_t handle to the created ringbuffer, or NULL in case of error.
 */
RingbufHandle_t xRingbufferCreate(size_t buf_length, ringbuf_type_t type);


/**
 * @brief  Delete a ring buffer
 *
 * @param  ringbuf - Ring buffer to delete
 *
 * @return void
 */
void vRingbufferDelete(RingbufHandle_t ringbuf);


/**
 * @brief  Get maximum size of an item that can be placed in the ring buffer
 *
 * @param  ringbuf - Ring buffer to query
 *
 * @return Maximum size, in bytes, of an item that can be placed in a ring buffer.
 */
size_t xRingbufferGetMaxItemSize(RingbufHandle_t ringbuf);


/**
 * @brief  Insert an item into the ring buffer
 *
 * @param  ringbuf - Ring buffer to insert the item into
 * @param  data - Pointer to data to insert. NULL is allowed if data_size is 0.
 * @param  data_size - Size of data to insert. A value of 0 is allowed.
 * @param  xTicksToWait - Ticks to wait for room in the ringbuffer.
 *
 * @return pdTRUE if succeeded, pdFALSE on time-out or when the buffer is larger 
 *         than indicated by xRingbufferGetMaxItemSize(ringbuf).
 */
BaseType_t xRingbufferSend(RingbufHandle_t ringbuf, void *data, size_t data_size, TickType_t ticks_to_wait);


/**
 * @brief  Insert an item into the ring buffer from an ISR
 *
 * @param  ringbuf - Ring buffer to insert the item into
 * @param  data - Pointer to data to insert. NULL is allowed if data_size is 0.
 * @param  data_size - Size of data to insert. A value of 0 is allowed.
 * @param  higher_prio_task_awoken - Value pointed to will be set to pdTRUE if the push woke up a higher
 *                                     priority task.
 *
 * @return pdTRUE if succeeded, pdFALSE when the ring buffer does not have space.
 */
BaseType_t xRingbufferSendFromISR(RingbufHandle_t ringbuf, void *data, size_t data_size, BaseType_t *higher_prio_task_awoken);

/**
 * @brief  Retrieve an item from the ring buffer
 *
 * @param  ringbuf - Ring buffer to retrieve the item from
 * @param  item_size - Pointer to a variable to which the size of the retrieved item will be written.
 * @param  xTicksToWait - Ticks to wait for items in the ringbuffer.
 *
 * @return Pointer to the retrieved item on success; *item_size filled with the length of the 
 *         item. NULL on timeout, *item_size is untouched in that case.
 */
void *xRingbufferReceive(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait);


/**
 * @brief  Retrieve an item from the ring buffer from an ISR
 *
 * @param  ringbuf - Ring buffer to retrieve the item from
 * @param  item_size - Pointer to a variable to which the size of the retrieved item will be written.
 *
 * @return Pointer to the retrieved item on success; *item_size filled with the length of the 
 *         item. NULL when the ringbuffer is empty, *item_size is untouched in that case.
 */
void *xRingbufferReceiveFromISR(RingbufHandle_t ringbuf, size_t *item_size);


/**
 * @brief  Retrieve bytes from a ByteBuf type of ring buffer, specifying the maximum amount of bytes
 * to return
 *
 * @param  ringbuf - Ring buffer to retrieve the item from
 * @param  item_size - Pointer to a variable to which the size of the retrieved item will be written.
 * @param  xTicksToWait - Ticks to wait for items in the ringbuffer.
 *
 * @return Pointer to the retrieved item on success; *item_size filled with the length of the 
 *         item. NULL on timeout, *item_size is untouched in that case.
 */
void *xRingbufferReceiveUpTo(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait, size_t wanted_size);


/**
 * @brief  Retrieve bytes from a ByteBuf type of ring buffer, specifying the maximum amount of bytes
 * to return. Call this from an ISR.
 *
 * @param  ringbuf - Ring buffer to retrieve the item from
 * @param  item_size - Pointer to a variable to which the size of the retrieved item will be written.
 *
 * @return Pointer to the retrieved item on success; *item_size filled with the length of the 
 *         item. NULL when the ringbuffer is empty, *item_size is untouched in that case.
 */
void *xRingbufferReceiveUpToFromISR(RingbufHandle_t ringbuf, size_t *item_size, size_t wanted_size);



/**
 * @brief  Return a previously-retrieved item to the ringbuffer
 *
 * @param  ringbuf - Ring buffer the item was retrieved from
 * @param  item - Item that was received earlier
 *
 * @return void
 */
void vRingbufferReturnItem(RingbufHandle_t ringbuf, void *item);



/**
 * @brief  Return a previously-retrieved item to the ringbuffer from an ISR
 *
 * @param  ringbuf - Ring buffer the item was retrieved from
 * @param  item - Item that was received earlier
 * @param  higher_prio_task_awoken - Value pointed to will be set to pdTRUE if the push woke up a higher
 *                                     priority task.
 *
 * @return void
 */
void vRingbufferReturnItemFromISR(RingbufHandle_t ringbuf, void *item, BaseType_t *higher_prio_task_awoken);


/**
 * @brief  Add the ringbuffer to a queue set. This specifically adds the semaphore that indicates
 *         more space has become available in the ringbuffer.
 *
 * @param  ringbuf - Ring buffer to add to the queue set
 * @param  xQueueSet - Queue set to add the ringbuffer to
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferAddToQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Add the ringbuffer to a queue set. This specifically adds the semaphore that indicates
 *         something has been written into the ringbuffer.
 *
 * @param  ringbuf - Ring buffer to add to the queue set
 * @param  xQueueSet - Queue set to add the ringbuffer to
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferAddToQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Remove the ringbuffer from a queue set. This specifically removes the semaphore that indicates
 *         more space has become available in the ringbuffer.
 *
 * @param  ringbuf - Ring buffer to remove from the queue set
 * @param  xQueueSet - Queue set to remove the ringbuffer from
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Remove the ringbuffer from a queue set. This specifically removes the semaphore that indicates
 *         something has been written to the ringbuffer.
 *
 * @param  ringbuf - Ring buffer to remove from the queue set
 * @param  xQueueSet - Queue set to remove the ringbuffer from
 *
 * @return pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferRemoveFromQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet);


/**
 * @brief  Debugging function to print the internal pointers in the ring buffer
 *
 * @param  ringbuf - Ring buffer to show
 *
 * @return void
 */
void xRingbufferPrintInfo(RingbufHandle_t ringbuf);


#endif
