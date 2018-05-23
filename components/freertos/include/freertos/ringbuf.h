// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FREERTOS_RINGBUF_H
#define FREERTOS_RINGBUF_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h" must appear in source files before "include ringbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <freertos/queue.h>

/**
 * Type by which ring buffers are referenced. For example, a call to xRingbufferCreate()
 * returns a RingbufHandle_t variable that can then be used as a parameter to
 * xRingbufferSend(), xRingbufferReceive(), etc.
 */
typedef void * RingbufHandle_t;

typedef enum {
	/**
	 * No-split buffers will only store an item in contiguous memory and will
	 * never split an item. Each item requires an 8 byte overhead for a header
	 * and will always internally occupy a 32-bit aligned size of space.
	 */
	RINGBUF_TYPE_NOSPLIT = 0,
	/**
	 * Allow-split buffers will split an item into two parts if necessary in
	 * order to store it. Each item requires an 8 byte overhead for a header,
	 * splitting incurs an extra header. Each item will always internally occupy
	 * a 32-bit aligned size of space.
	 */
	RINGBUF_TYPE_ALLOWSPLIT,
	/**
	 * Byte buffers store data as a sequence of bytes and do not maintain separate
	 * items, therefore byte buffers have no overhead. All data is stored as a
	 * sequence of byte and any number of bytes can be sent or retrieved each
	 * time.
	 */
	RINGBUF_TYPE_BYTEBUF
} ringbuf_type_t;

/**
 * @brief       Create a ring buffer
 *
 * @param[in]   xBufferSize Size of the buffer in bytes. Note that items require
 *              space for overhead in no-split/allow-split buffers
 * @param[in]   xBufferType Type of ring buffer, see documentation.
 *
 * @note    xBufferSize of no-split/allow-split buffers will be rounded up to the nearest 32-bit aligned size.
 *
 * @return  A handle to the created ring buffer, or NULL in case of error.
 */
RingbufHandle_t xRingbufferCreate(size_t xBufferSize, ringbuf_type_t xBufferType);

/**
 * @brief Create a ring buffer of type RINGBUF_TYPE_NOSPLIT for a fixed item_size
 *
 * This API is similar to xRingbufferCreate(), but it will internally allocate
 * additional space for the headers.
 *
 * @param[in]   xItemSize   Size of each item to be put into the ring buffer
 * @param[in]   xItemNum    Maximum number of items the buffer needs to hold simultaneously
 *
 * @return  A RingbufHandle_t handle to the created ring buffer, or NULL in case of error.
 */
RingbufHandle_t xRingbufferCreateNoSplit(size_t xItemSize, size_t xItemNum);

/**
 * @brief       Insert an item into the ring buffer
 *
 * Attempt to insert an item into the ring buffer. This function will block until
 * enough free space is available or until it timesout.
 *
 * @param[in]   xRingbuffer     Ring buffer to insert the item into
 * @param[in]   pvItem          Pointer to data to insert. NULL is allowed if xItemSize is 0.
 * @param[in]   xItemSize       Size of data to insert.
 * @param[in]   xTicksToWait    Ticks to wait for room in the ring buffer.
 *
 * @note    For no-split/allow-split ring buffers, the actual size of memory that
 *          the item will occupy will be rounded up to the nearest 32-bit aligned
 *          size. This is done to ensure all items are always stored in 32-bit
 *          aligned fashion.
 *
 * @return
 *      - pdTRUE if succeeded
 *      - pdFALSE on time-out or when the data is larger than the maximum permissible size of the buffer
 */
BaseType_t xRingbufferSend(RingbufHandle_t xRingbuffer, const void *pvItem, size_t xItemSize, TickType_t xTicksToWait);

/**
 * @brief       Insert an item into the ring buffer in an ISR
 *
 * Attempt to insert an item into the ring buffer from an ISR. This function
 * will return immediately if there is insufficient free space in the buffer.
 *
 * @param[in]   xRingbuffer Ring buffer to insert the item into
 * @param[in]   pvItem      Pointer to data to insert. NULL is allowed if xItemSize is 0.
 * @param[in]   xItemSize   Size of data to insert.
 * @param[out]  pxHigherPriorityTaskWoken   Value pointed to will be set to pdTRUE if the function woke up a higher priority task.
 *
 * @note    For no-split/allow-split ring buffers, the actual size of memory that
 *          the item will occupy will be rounded up to the nearest 32-bit aligned
 *          size. This is done to ensure all items are always stored in 32-bit
 *          aligned fashion.
 *
 * @return
 *      - pdTRUE if succeeded
 *      - pdFALSE when the ring buffer does not have space.
 */
BaseType_t xRingbufferSendFromISR(RingbufHandle_t xRingbuffer, const void *pvItem, size_t xItemSize, BaseType_t *pxHigherPriorityTaskWoken);

/**
 * @brief   Retrieve an item from the ring buffer
 *
 * Attempt to retrieve an item from the ring buffer. This function will block
 * until an item is available or until it timesout.
 *
 * @param[in]   xRingbuffer     Ring buffer to retrieve the item from
 * @param[out]  pxItemSize      Pointer to a variable to which the size of the retrieved item will be written.
 * @param[in]   xTicksToWait    Ticks to wait for items in the ring buffer.
 *
 * @note    A call to vRingbufferReturnItem() is required after this to free the item retrieved.
 *
 * @return
 *      - Pointer to the retrieved item on success; *pxItemSize filled with the length of the item.
 *      - NULL on timeout, *pxItemSize is untouched in that case.
 */
void *xRingbufferReceive(RingbufHandle_t xRingbuffer, size_t *pxItemSize, TickType_t xTicksToWait);

/**
 * @brief   Retrieve an item from the ring buffer in an ISR
 *
 * Attempt to retrieve an item from the ring buffer. This function returns immediately
 * if there are no items available for retrieval
 *
 * @param[in]   xRingbuffer     Ring buffer to retrieve the item from
 * @param[out]  pxItemSize      Pointer to a variable to which the size of the
 *                              retrieved item will be written.
 *
 * @note    A call to vRingbufferReturnItemFromISR() is required after this to free the item retrieved.
 * @note    Byte buffers do not allow multiple retrievals before returning an item
 *
 * @return
 *      - Pointer to the retrieved item on success; *pxItemSize filled with the length of the item.
 *      - NULL when the ring buffer is empty, *pxItemSize is untouched in that case.
 */
void *xRingbufferReceiveFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize);

/**
 * @brief   Retrieve a split item from an allow-split ring buffer
 *
 * Attempt to retrieve a split item from an allow-split ring buffer. If the item
 * is not split, only a single item is retried. If the item is split, both parts
 * will be retrieved. This function will block until an item is available or
 * until it timesout.
 *
 * @param[in]   xRingbuffer     Ring buffer to retrieve the item from
 * @param[out]  ppvHeadItem     Double pointer to first part (set to NULL if no items were retrieved)
 * @param[out]  ppvTailItem     Double pointer to second part (set to NULL if item is not split)
 * @param[out]  pxHeadItemSize  Pointer to size of first part (unmodified if no items were retrieved)
 * @param[out]  pxTailItemSize  Pointer to size of second part (unmodified if item is not split)
 * @param[in]   xTicksToWait    Ticks to wait for items in the ring buffer.
 *
 * @note    Call(s) to vRingbufferReturnItem() is required after this to free up the item(s) retrieved.
 * @note    This function should only be called on allow-split buffers
 *
 * @return
 *      - pdTRUE if an item (split or unsplit) was retrieved
 *      - pdFALSE when no item was retrieved
 */
BaseType_t xRingbufferReceiveSplit(RingbufHandle_t xRingbuffer, void **ppvHeadItem, void **ppvTailItem, size_t *pxHeadItemSize, size_t *pxTailItemSize, TickType_t xTicksToWait);

/**
 * @brief   Retrieve a split item from an allow-split ring buffer in an ISR
 *
 * Attempt to retrieve a split item from an allow-split ring buffer. If the item
 * is not split, only a single item is retried. If the item is split, both parts
 * will be retrieved. This function returns immediately if there are no items
 * available for retrieval
 *
 * @param[in]   xRingbuffer     Ring buffer to retrieve the item from
 * @param[out]  ppvHeadItem     Double pointer to first part (set to NULL if no items were retrieved)
 * @param[out]  ppvTailItem     Double pointer to second part (set to NULL if item is not split)
 * @param[out]  pxHeadItemSize  Pointer to size of first part (unmodified if no items were retrieved)
 * @param[out]  pxTailItemSize  Pointer to size of second part (unmodified if item is not split)
 *
 * @note    Calls to vRingbufferReturnItemFromISR() is required after this to free up the item(s) retrieved.
 * @note    This function should only be called on allow-split buffers
 *
 * @return
 *      - pdTRUE if an item (split or unsplit) was retrieved
 *      - pdFALSE when no item was retrieved
 */
BaseType_t xRingbufferReceiveSplitFromISR(RingbufHandle_t xRingbuffer, void **ppvHeadItem, void **ppvTailItem, size_t *pxHeadItemSize, size_t *pxTailItemSize);

/**
 * @brief   Retrieve bytes from a byte buffer, specifying the maximum amount of bytes to retrieve
 *
 * Attempt to retrieve data from a byte buffer whilst specifying a maximum number
 * of bytes to retrieve. This function will block until there is data available
 * for retrieval or until it timesout.
 *
 * @param[in]   xRingbuffer     Ring buffer to retrieve the item from
 * @param[out]  pxItemSize      Pointer to a variable to which the size of the retrieved item will be written.
 * @param[in]   xTicksToWait    Ticks to wait for items in the ring buffer.
 * @param[in]   xMaxSize        Maximum number of bytes to return.
 *
 * @note    A call to vRingbufferReturnItem() is required after this to free up the data retrieved.
 * @note    This function should only be called on byte buffers
 * @note    Byte buffers do not allow multiple retrievals before returning an item
 *
 * @return
 *      - Pointer to the retrieved item on success; *pxItemSize filled with
 *        the length of the item.
 *      - NULL on timeout, *pxItemSize is untouched in that case.
 */
void *xRingbufferReceiveUpTo(RingbufHandle_t xRingbuffer, size_t *pxItemSize, TickType_t xTicksToWait, size_t xMaxSize);

/**
 * @brief   Retrieve bytes from a byte buffer, specifying the maximum amount of
 *          bytes to retrieve. Call this from an ISR.
 *
 * Attempt to retrieve bytes from a byte buffer whilst specifying a maximum number
 * of bytes to retrieve. This function will return immediately if there is no data
 * available for retrieval.
 *
 * @param[in]   xRingbuffer Ring buffer to retrieve the item from
 * @param[out]  pxItemSize  Pointer to a variable to which the size of the retrieved item will be written.
 * @param[in]   xMaxSize    Maximum number of bytes to return.
 *
 * @note    A call to vRingbufferReturnItemFromISR() is required after this to free up the data received.
 * @note    This function should only be called on byte buffers
 * @note    Byte buffers do not allow multiple retrievals before returning an item
 *
 * @return
 *      - Pointer to the retrieved item on success; *pxItemSize filled with
 *        the length of the item.
 *      - NULL when the ring buffer is empty, *pxItemSize is untouched in that case.
 */
void *xRingbufferReceiveUpToFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize, size_t xMaxSize);

/**
 * @brief   Return a previously-retrieved item to the ring buffer
 *
 * @param[in]   xRingbuffer Ring buffer the item was retrieved from
 * @param[in]   pvItem      Item that was received earlier
 *
 * @note    If a split item is retrieved, both parts should be returned by calling this function twice
 */
void vRingbufferReturnItem(RingbufHandle_t xRingbuffer, void *pvItem);

/**
 * @brief   Return a previously-retrieved item to the ring buffer from an ISR
 *
 * @param[in]   xRingbuffer Ring buffer the item was retrieved from
 * @param[in]   pvItem      Item that was received earlier
 * @param[out]  pxHigherPriorityTaskWoken   Value pointed to will be set to pdTRUE
 *                                          if the function woke up a higher priority task.
 *
 * @note    If a split item is retrieved, both parts should be returned by calling this function twice
 */
void vRingbufferReturnItemFromISR(RingbufHandle_t xRingbuffer, void *pvItem, BaseType_t *pxHigherPriorityTaskWoken);

/**
 * @brief   Delete a ring buffer
 *
 * @param[in]   xRingbuffer     Ring buffer to delete
 */
void vRingbufferDelete(RingbufHandle_t xRingbuffer);

/**
 * @brief   Get maximum size of an item that can be placed in the ring buffer
 *
 * This function returns the maximum size an item can have if it was placed in
 * an empty ring buffer.
 *
 * @param[in]   xRingbuffer     Ring buffer to query
 *
 * @return  Maximum size, in bytes, of an item that can be placed in a ring buffer.
 */
size_t xRingbufferGetMaxItemSize(RingbufHandle_t xRingbuffer);

/**
 * @brief   Get current free size available for an item/data in the buffer
 *
 * This gives the real time free space available for an item/data in the ring
 * buffer. This represents the maximum size an item/data can have if it was
 * currently sent to the ring buffer.
 *
 * @warning This API is not thread safe. So, if multiple threads are accessing
 *          the same ring buffer, it is the application's responsibility to
 *          ensure atomic access to this API and the subsequent Send
 *
 * @param[in]   xRingbuffer     Ring buffer to query
 *
 * @return  Current free size, in bytes, available for an entry
 */
size_t xRingbufferGetCurFreeSize(RingbufHandle_t xRingbuffer);

/**
 * @brief   Add the ring buffer's read semaphore to a queue set.
 *
 * The ring buffer's read semaphore indicates that data has been written
 * to the ring buffer. This function adds the ring buffer's read semaphore to
 * a queue set.
 *
 * @param[in]   xRingbuffer     Ring buffer to add to the queue set
 * @param[in]   xQueueSet       Queue set to add the ring buffer's read semaphore to
 *
 * @return
 *      - pdTRUE on success, pdFALSE otherwise
 */
BaseType_t xRingbufferAddToQueueSetRead(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet);


/**
 * @brief   Check if the selected queue set member is the ring buffer's read semaphore
 *
 * This API checks if queue set member returned from xQueueSelectFromSet()
 * is the read semaphore of this ring buffer. If so, this indicates the ring buffer
 * has items waiting to be retrieved.
 *
 * @param[in]   xRingbuffer     Ring buffer which should be checked
 * @param[in]   xMember         Member returned from xQueueSelectFromSet
 *
 * @return
 *      - pdTRUE when semaphore belongs to ring buffer
 *      - pdFALSE otherwise.
 */
BaseType_t xRingbufferCanRead(RingbufHandle_t xRingbuffer, QueueSetMemberHandle_t xMember);

/**
 * @brief   Remove the ring buffer's read semaphore from a queue set.
 *
 * This specifically removes a ring buffer's read semaphore from a queue set. The
 * read semaphore is used to indicate when data has been written to the ring buffer
 *
 * @param[in]   xRingbuffer     Ring buffer to remove from the queue set
 * @param[in]   xQueueSet       Queue set to remove the ring buffer's read semaphore from
 *
 * @return
 *      - pdTRUE on success
 *      - pdFALSE otherwise
 */
BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet);

/**
 * @brief   Get information about ring buffer status
 *
 * Get information of the a ring buffer's current status such as
 * free/read/write pointer positions, and number of items waiting to be retrieved.
 * Arguments can be set to NULL if they are not required.
 *
 * @param[in]   xRingbuffer     Ring buffer to remove from the queue set
 * @param[out]  uxFree          Pointer use to store free pointer position
 * @param[out]  uxRead          Pointer use to store read pointer position
 * @param[out]  uxWrite         Pointer use to store write pointer position
 * @param[out]  uxItemsWaiting  Pointer use to store number of items (bytes for byte buffer) waiting to be retrieved
 */
void vRingbufferGetInfo(RingbufHandle_t xRingbuffer, UBaseType_t *uxFree, UBaseType_t *uxRead, UBaseType_t *uxWrite, UBaseType_t *uxItemsWaiting);

/**
 * @brief   Debugging function to print the internal pointers in the ring buffer
 *
 * @param   xRingbuffer Ring buffer to show
 */
void xRingbufferPrintInfo(RingbufHandle_t xRingbuffer);

/* -------------------------------- Deprecated Functions --------------------------- */

/** @cond */    //Doxygen command to hide deprecated function from API Reference
/*
 * Deprecated as function is not thread safe and does not check if an item is
 * actually available for retrieval. Use xRingbufferReceiveSplit() instead for
 * thread safe method of retrieve a split item.
 */
bool xRingbufferIsNextItemWrapped(RingbufHandle_t xRingbuffer) __attribute__((deprecated));

/*
 * Deprecated as queue sets are not meant to be used for writing to buffers. Adding
 * the ring buffer write semaphore to a queue set will break queue set usage rules,
 * as every read of a semaphore must be preceded by a call to xQueueSelectFromSet().
 * QueueSetWrite no longer supported.
 */
BaseType_t xRingbufferAddToQueueSetWrite(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet) __attribute__((deprecated));

/*
 * Deprecated as queue sets are not meant to be used for writing to buffers.
 * QueueSetWrite no longer supported.
 */
BaseType_t xRingbufferRemoveFromQueueSetWrite(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet) __attribute__((deprecated));
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_RINGBUF_H */

