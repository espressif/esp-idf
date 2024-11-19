/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/list.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "esp_heap_caps.h"

// ------------------------------------------------- Macros and Types --------------------------------------------------

//32-bit alignment macros
#define rbALIGN_MASK (0x03)
#define rbALIGN_SIZE( xSize )       ( ( xSize + rbALIGN_MASK ) & ~rbALIGN_MASK )
#define rbCHECK_ALIGNED( pvPtr )    ( ( ( UBaseType_t ) ( pvPtr ) & rbALIGN_MASK ) == 0 )

//Ring buffer flags
#define rbALLOW_SPLIT_FLAG          ( ( UBaseType_t ) 1 )   //The ring buffer allows items to be split
#define rbBYTE_BUFFER_FLAG          ( ( UBaseType_t ) 2 )   //The ring buffer is a byte buffer
#define rbBUFFER_FULL_FLAG          ( ( UBaseType_t ) 4 )   //The ring buffer is currently full (write pointer == free pointer)
#define rbBUFFER_STATIC_FLAG        ( ( UBaseType_t ) 8 )   //The ring buffer is statically allocated
#define rbUSING_QUEUE_SET           ( ( UBaseType_t ) 16 )  //The ring buffer has been added to a queue set

//Item flags
#define rbITEM_FREE_FLAG            ( ( UBaseType_t ) 1 )   //Item has been retrieved and returned by application, free to overwrite
#define rbITEM_DUMMY_DATA_FLAG      ( ( UBaseType_t ) 2 )   //Data from here to end of the ring buffer is dummy data. Restart reading at start of head of the buffer
#define rbITEM_SPLIT_FLAG           ( ( UBaseType_t ) 4 )   //Valid for RINGBUF_TYPE_ALLOWSPLIT, indicating that rest of the data is wrapped around
#define rbITEM_WRITTEN_FLAG         ( ( UBaseType_t ) 8 )   //Item has been written to by the application, thus can be read

typedef struct {
    //This size of this structure must be 32-bit aligned
    size_t xItemLen;
    UBaseType_t uxItemFlags;
} ItemHeader_t;

#define rbHEADER_SIZE     sizeof(ItemHeader_t)
typedef struct RingbufferDefinition Ringbuffer_t;
typedef BaseType_t (*CheckItemFitsFunction_t)(Ringbuffer_t *pxRingbuffer, size_t xItemSize);
typedef void (*CopyItemFunction_t)(Ringbuffer_t *pxRingbuffer, const uint8_t *pcItem, size_t xItemSize);
typedef BaseType_t (*CheckItemAvailFunction_t)(Ringbuffer_t *pxRingbuffer);
typedef void *(*GetItemFunction_t)(Ringbuffer_t *pxRingbuffer, BaseType_t *pxIsSplit, size_t xMaxSize, size_t *pxItemSize);
typedef void (*ReturnItemFunction_t)(Ringbuffer_t *pxRingbuffer, uint8_t *pvItem);
typedef size_t (*GetCurMaxSizeFunction_t)(Ringbuffer_t *pxRingbuffer);

typedef struct RingbufferDefinition {
    size_t xSize;                               //Size of the data storage
    size_t xMaxItemSize;                        //Maximum item size
    UBaseType_t uxRingbufferFlags;              //Flags to indicate the type and status of ring buffer

    CheckItemFitsFunction_t xCheckItemFits;     //Function to check if item can currently fit in ring buffer
    CopyItemFunction_t vCopyItem;               //Function to copy item to ring buffer
    GetItemFunction_t pvGetItem;                //Function to get item from ring buffer
    ReturnItemFunction_t vReturnItem;           //Function to return item to ring buffer
    GetCurMaxSizeFunction_t xGetCurMaxSize;     //Function to get current free size

    uint8_t *pucAcquire;                        //Acquire Pointer. Points to where the next item should be acquired.
    uint8_t *pucWrite;                          //Write Pointer. Points to where the next item should be written
    uint8_t *pucRead;                           //Read Pointer. Points to where the next item should be read from
    uint8_t *pucFree;                           //Free Pointer. Points to the last item that has yet to be returned to the ring buffer
    uint8_t *pucHead;                           //Pointer to the start of the ring buffer storage area
    uint8_t *pucTail;                           //Pointer to the end of the ring buffer storage area

    BaseType_t xItemsWaiting;                   //Number of items/bytes(for byte buffers) currently in ring buffer that have not yet been read
    List_t xTasksWaitingToSend;                 //List of tasks that are blocked waiting to send/acquire onto this ring buffer. Stored in priority order.
    List_t xTasksWaitingToReceive;              //List of tasks that are blocked waiting to receive from this ring buffer. Stored in priority order.
    QueueSetHandle_t xQueueSet;                 //Ring buffer's read queue set handle.

    portMUX_TYPE mux;                           //Spinlock required for SMP
} Ringbuffer_t;

_Static_assert(sizeof(StaticRingbuffer_t) == sizeof(Ringbuffer_t), "StaticRingbuffer_t != Ringbuffer_t");

// ------------------------------------------------ Forward Declares ---------------------------------------------------

/*
 * WARNING: All of the following static functions (except generic functions)
 * ARE NOT THREAD SAFE. Therefore they should only be called within a critical
 * section (using spin locks)
 */

//Initialize a ring buffer after space has been allocated for it
static void prvInitializeNewRingbuffer(size_t xBufferSize,
                                       RingbufferType_t xBufferType,
                                       Ringbuffer_t *pxNewRingbuffer,
                                       uint8_t *pucRingbufferStorage);

//Calculate current amount of free space (in bytes) in the ring buffer
static size_t prvGetFreeSize(Ringbuffer_t *pxRingbuffer);

//Checks if an item/data is currently available for retrieval
static BaseType_t prvCheckItemAvail(Ringbuffer_t *pxRingbuffer);

//Checks if an item will currently fit in a no-split/allow-split ring buffer
static BaseType_t prvCheckItemFitsDefault(Ringbuffer_t *pxRingbuffer, size_t xItemSize);

//Checks if an item will currently fit in a byte buffer
static BaseType_t prvCheckItemFitsByteBuffer(Ringbuffer_t *pxRingbuffer, size_t xItemSize);

/*
Copies an item to a no-split ring buffer
Entry:
    - Must have already guaranteed there is sufficient space for item by calling prvCheckItemFitsDefault()
Exit:
    - New item copied into ring buffer
    - pucAcquire and pucWrite updated.
    - Dummy item added if necessary
*/
static void prvCopyItemNoSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

/*
Copies an item to a allow-split ring buffer
Entry:
    - Must have already guaranteed there is sufficient space for item by calling prvCheckItemFitsDefault()
Exit:
    - New item copied into ring buffer
    - pucAcquire and pucWrite updated
    - Item may be split
*/
static void prvCopyItemAllowSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

//Copies an item to a byte buffer. Only call this function  after calling prvCheckItemFitsByteBuffer()
static void prvCopyItemByteBuf(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

//Retrieve item from no-split/allow-split ring buffer. *pxIsSplit is set to pdTRUE if the retrieved item is split
/*
Entry:
    - Must have already guaranteed that there is an item available for retrieval by calling prvCheckItemAvail()
    - Guaranteed that pucREAD points to a valid item (i.e., not a dummy item)
Exit:
    - Item is returned. Only first half returned if split
    - pucREAD updated to point to next valid item to read, or equals to pucWrite if there are no more valid items to read
    - pucREAD update must skip over dummy items
*/
static void *prvGetItemDefault(Ringbuffer_t *pxRingbuffer,
                               BaseType_t *pxIsSplit,
                               size_t xUnusedParam,
                               size_t *pxItemSize);

//Retrieve data from byte buffer. If xMaxSize is 0, all continuous data is retrieved
static void *prvGetItemByteBuf(Ringbuffer_t *pxRingbuffer,
                               BaseType_t *pxUnusedParam,
                               size_t xMaxSize,
                               size_t *pxItemSize);

/*
Return an item to a split/no-split ring buffer
Exit:
    - Item is marked free rbITEM_FREE_FLAG
    - pucFree is progressed as far as possible, skipping over already freed items or dummy items
*/
static void prvReturnItemDefault(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem);

//Return data to a byte buffer
static void prvReturnItemByteBuf(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem);

//Get the maximum size an item that can currently have if sent to a no-split ring buffer
static size_t prvGetCurMaxSizeNoSplit(Ringbuffer_t *pxRingbuffer);

//Get the maximum size an item that can currently have if sent to a allow-split ring buffer
static size_t prvGetCurMaxSizeAllowSplit(Ringbuffer_t *pxRingbuffer);

//Get the maximum size an item that can currently have if sent to a byte buffer
static size_t prvGetCurMaxSizeByteBuf(Ringbuffer_t *pxRingbuffer);

/*
Generic function used to send or acquire an item/buffer.
- If sending, set ppvItem to NULL. pvItem remains unchanged on failure.
- If acquiring, set pvItem to NULL. ppvItem remains unchanged on failure.
*/
static BaseType_t prvSendAcquireGeneric(Ringbuffer_t *pxRingbuffer,
                                        const void *pvItem,
                                        void **ppvItem,
                                        size_t xItemSize,
                                        TickType_t xTicksToWait);

/*
Generic function used to retrieve an item/data from ring buffers. If called on
an allow-split buffer, and pvItem2 and xItemSize2 are not NULL, both parts of
a split item will be retrieved. xMaxSize will only take effect if called on
byte buffers. xItemSize must remain unchanged if no item is retrieved.
*/
static BaseType_t prvReceiveGeneric(Ringbuffer_t *pxRingbuffer,
                                    void **pvItem1,
                                    void **pvItem2,
                                    size_t *xItemSize1,
                                    size_t *xItemSize2,
                                    size_t xMaxSize,
                                    TickType_t xTicksToWait);

//From ISR version of prvReceiveGeneric()
static BaseType_t prvReceiveGenericFromISR(Ringbuffer_t *pxRingbuffer,
                                           void **pvItem1,
                                           void **pvItem2,
                                           size_t *xItemSize1,
                                           size_t *xItemSize2,
                                           size_t xMaxSize);

// ------------------------------------------------ Static Functions ---------------------------------------------------

static void prvInitializeNewRingbuffer(size_t xBufferSize,
                                       RingbufferType_t xBufferType,
                                       Ringbuffer_t *pxNewRingbuffer,
                                       uint8_t *pucRingbufferStorage)
{
    //Initialize values
    pxNewRingbuffer->xSize = xBufferSize;
    pxNewRingbuffer->pucHead = pucRingbufferStorage;
    pxNewRingbuffer->pucTail = pucRingbufferStorage + xBufferSize;
    pxNewRingbuffer->pucFree = pucRingbufferStorage;
    pxNewRingbuffer->pucRead = pucRingbufferStorage;
    pxNewRingbuffer->pucWrite = pucRingbufferStorage;
    pxNewRingbuffer->pucAcquire = pucRingbufferStorage;
    pxNewRingbuffer->xItemsWaiting = 0;
    pxNewRingbuffer->uxRingbufferFlags = 0;

    //Initialize type dependent values and function pointers
    if (xBufferType == RINGBUF_TYPE_NOSPLIT) {
        pxNewRingbuffer->xCheckItemFits = prvCheckItemFitsDefault;
        pxNewRingbuffer->vCopyItem = prvCopyItemNoSplit;
        pxNewRingbuffer->pvGetItem = prvGetItemDefault;
        pxNewRingbuffer->vReturnItem = prvReturnItemDefault;
        /*
         * Worst case scenario is when the read/write/acquire/free pointers are all
         * pointing to the halfway point of the buffer.
         */
        pxNewRingbuffer->xMaxItemSize = rbALIGN_SIZE(pxNewRingbuffer->xSize / 2) - rbHEADER_SIZE;
        pxNewRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeNoSplit;
    } else if (xBufferType == RINGBUF_TYPE_ALLOWSPLIT) {
        pxNewRingbuffer->uxRingbufferFlags |= rbALLOW_SPLIT_FLAG;
        pxNewRingbuffer->xCheckItemFits = prvCheckItemFitsDefault;
        pxNewRingbuffer->vCopyItem = prvCopyItemAllowSplit;
        pxNewRingbuffer->pvGetItem = prvGetItemDefault;
        pxNewRingbuffer->vReturnItem = prvReturnItemDefault;
        //Worst case an item is split into two, incurring two headers of overhead
        pxNewRingbuffer->xMaxItemSize = pxNewRingbuffer->xSize - (sizeof(ItemHeader_t) * 2);
        pxNewRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeAllowSplit;
    } else { //Byte Buffer
        pxNewRingbuffer->uxRingbufferFlags |= rbBYTE_BUFFER_FLAG;
        pxNewRingbuffer->xCheckItemFits = prvCheckItemFitsByteBuffer;
        pxNewRingbuffer->vCopyItem = prvCopyItemByteBuf;
        pxNewRingbuffer->pvGetItem = prvGetItemByteBuf;
        pxNewRingbuffer->vReturnItem = prvReturnItemByteBuf;
        //Byte buffers do not incur any overhead
        pxNewRingbuffer->xMaxItemSize = pxNewRingbuffer->xSize;
        pxNewRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeByteBuf;
    }

    vListInitialise(&pxNewRingbuffer->xTasksWaitingToSend);
    vListInitialise(&pxNewRingbuffer->xTasksWaitingToReceive);
    pxNewRingbuffer->xQueueSet = NULL;

    portMUX_INITIALIZE(&pxNewRingbuffer->mux);
}

static size_t prvGetFreeSize(Ringbuffer_t *pxRingbuffer)
{
    size_t xReturn;
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        xReturn =  0;
    } else {
        BaseType_t xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucAcquire;
        //Check if xFreeSize has underflowed
        if (xFreeSize <= 0) {
            xFreeSize += pxRingbuffer->xSize;
        }
        xReturn = xFreeSize;
    }
    configASSERT(xReturn <= pxRingbuffer->xSize);
    return xReturn;
}

static BaseType_t prvCheckItemFitsDefault(Ringbuffer_t *pxRingbuffer, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucAcquire));              //pucAcquire is always aligned in no-split/allow-split ring buffers
    configASSERT(pxRingbuffer->pucAcquire >= pxRingbuffer->pucHead && pxRingbuffer->pucAcquire < pxRingbuffer->pucTail);    //Check write pointer is within bounds

    size_t xTotalItemSize = rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE;    //Rounded up aligned item size with header
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucFree) {
        //Buffer is either complete empty or completely full
        return (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? pdFALSE : pdTRUE;
    }
    if (pxRingbuffer->pucFree > pxRingbuffer->pucAcquire) {
        //Free space does not wrap around
        return (xTotalItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucAcquire) ? pdTRUE : pdFALSE;
    }
    //Free space wraps around
    if (xTotalItemSize <= pxRingbuffer->pucTail - pxRingbuffer->pucAcquire) {
        return pdTRUE;      //Item fits without wrapping around
    }
    //Check if item fits by wrapping
    if (pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        //Allow split wrapping incurs an extra header
        return (xTotalItemSize + rbHEADER_SIZE <= pxRingbuffer->xSize - (pxRingbuffer->pucAcquire - pxRingbuffer->pucFree)) ? pdTRUE : pdFALSE;
    } else {
        return (xTotalItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucHead) ? pdTRUE : pdFALSE;
    }
}

static BaseType_t prvCheckItemFitsByteBuffer(Ringbuffer_t *pxRingbuffer, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(pxRingbuffer->pucAcquire >= pxRingbuffer->pucHead && pxRingbuffer->pucAcquire < pxRingbuffer->pucTail);    //Check acquire pointer is within bounds

    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucFree) {
        //Buffer is either complete empty or completely full
        return (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? pdFALSE : pdTRUE;
    }
    if (pxRingbuffer->pucFree > pxRingbuffer->pucAcquire) {
        //Free space does not wrap around
        return (xItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucAcquire) ? pdTRUE : pdFALSE;
    }
    //Free space wraps around
    return (xItemSize <= pxRingbuffer->xSize - (pxRingbuffer->pucAcquire - pxRingbuffer->pucFree)) ? pdTRUE : pdFALSE;
}

static uint8_t* prvAcquireItemNoSplit(Ringbuffer_t *pxRingbuffer, size_t xItemSize)
{
    //Check arguments and buffer state
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);                  //Rounded up aligned item size
    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucAcquire;    //Length from pucAcquire until end of buffer
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucAcquire));              //pucAcquire is always aligned in no-split ring buffers
    configASSERT(pxRingbuffer->pucAcquire >= pxRingbuffer->pucHead && pxRingbuffer->pucAcquire < pxRingbuffer->pucTail);    //Check write pointer is within bounds
    configASSERT(xRemLen >= rbHEADER_SIZE);                             //Remaining length must be able to at least fit an item header

    //If remaining length can't fit item, set as dummy data and wrap around
    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        ItemHeader_t *pxDummy = (ItemHeader_t *)pxRingbuffer->pucAcquire;
        pxDummy->uxItemFlags = rbITEM_DUMMY_DATA_FLAG;      //Set remaining length as dummy data
        pxDummy->xItemLen = 0;                              //Dummy data should have no length
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;     //Reset acquire pointer to wrap around
    }

    //Item should be guaranteed to fit at this point. Set item header and copy data
    ItemHeader_t *pxHeader = (ItemHeader_t *)pxRingbuffer->pucAcquire;
    pxHeader->xItemLen = xItemSize;
    pxHeader->uxItemFlags = 0;

    //hold the buffer address without touching pucWrite
    uint8_t* item_address = pxRingbuffer->pucAcquire + rbHEADER_SIZE;
    pxRingbuffer->pucAcquire += rbHEADER_SIZE + xAlignedItemSize;    //Advance pucAcquire past header and the item to next aligned address

    //After the allocation, add some padding after the buffer and correct the flags
    //If current remaining length can't fit a header, wrap around write pointer
    if (pxRingbuffer->pucTail - pxRingbuffer->pucAcquire < rbHEADER_SIZE) {
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;   //Wrap around pucAcquire
    }
    //Check if buffer is full
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucFree) {
        //Mark the buffer as full to distinguish with an empty buffer
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
    return item_address;
}

static void prvSendItemDoneNoSplit(Ringbuffer_t *pxRingbuffer, uint8_t* pucItem)
{
    //Check arguments and buffer state
    configASSERT(rbCHECK_ALIGNED(pucItem));
    configASSERT(pucItem >= pxRingbuffer->pucHead);
    configASSERT(pucItem <= pxRingbuffer->pucTail);     //Inclusive of pucTail in the case of zero length item at the very end

    //Get and check header of the item
    ItemHeader_t *pxCurHeader = (ItemHeader_t *)(pucItem - rbHEADER_SIZE);
    configASSERT(pxCurHeader->xItemLen <= pxRingbuffer->xMaxItemSize);
    configASSERT((pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) == 0); //Dummy items should never have been written
    configASSERT((pxCurHeader->uxItemFlags & rbITEM_WRITTEN_FLAG) == 0);       //Indicates item has already been written before
    pxCurHeader->uxItemFlags &= ~rbITEM_SPLIT_FLAG;                         //Clear wrap flag if set (not strictly necessary)
    pxCurHeader->uxItemFlags |= rbITEM_WRITTEN_FLAG;                           //Mark as written

    pxRingbuffer->xItemsWaiting++;

    /*
     * Items might not be written in the order they were acquired. Move the
     * write pointer up to the next item that has not been marked as written (by
     * written flag) or up till the acquire pointer. When advancing the write
     * pointer, items that have already been written or items with dummy data
     * should be skipped over
     */
    pxCurHeader = (ItemHeader_t *)pxRingbuffer->pucWrite;
    //Skip over Items that have already been written or are dummy items
    while (((pxCurHeader->uxItemFlags & rbITEM_WRITTEN_FLAG) || (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG)) && pxRingbuffer->pucWrite != pxRingbuffer->pucAcquire) {
        if (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
            pxCurHeader->uxItemFlags |= rbITEM_WRITTEN_FLAG;   //Mark as freed (not strictly necessary but adds redundancy)
            pxRingbuffer->pucWrite = pxRingbuffer->pucHead;    //Wrap around due to dummy data
        } else {
            //Item with data that has already been written, advance write pointer past this item
            size_t xAlignedItemSize = rbALIGN_SIZE(pxCurHeader->xItemLen);
            pxRingbuffer->pucWrite += xAlignedItemSize + rbHEADER_SIZE;
            //Redundancy check to ensure write pointer has not overshot buffer bounds
            configASSERT(pxRingbuffer->pucWrite <= pxRingbuffer->pucHead + pxRingbuffer->xSize);
        }
        //Check if pucWrite requires wrap around
        if ((pxRingbuffer->pucTail - pxRingbuffer->pucWrite) < rbHEADER_SIZE) {
            pxRingbuffer->pucWrite = pxRingbuffer->pucHead;
        }
        pxCurHeader = (ItemHeader_t *)pxRingbuffer->pucWrite;      //Update header to point to item
    }
}

static void prvCopyItemNoSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    uint8_t* item_addr = prvAcquireItemNoSplit(pxRingbuffer, xItemSize);
    memcpy(item_addr, pucItem, xItemSize);
    prvSendItemDoneNoSplit(pxRingbuffer, item_addr);
}

static void prvCopyItemAllowSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    //Check arguments and buffer state
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);                  //Rounded up aligned item size
    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucAcquire;    //Length from pucAcquire until end of buffer
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucAcquire));              //pucAcquire is always aligned in split ring buffers
    configASSERT(pxRingbuffer->pucAcquire >= pxRingbuffer->pucHead && pxRingbuffer->pucAcquire < pxRingbuffer->pucTail);    //Check write pointer is within bounds
    configASSERT(xRemLen >= rbHEADER_SIZE);                             //Remaining length must be able to at least fit an item header

    //Split item if necessary
    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        //Write first part of the item
        ItemHeader_t *pxFirstHeader = (ItemHeader_t *)pxRingbuffer->pucAcquire;
        pxFirstHeader->uxItemFlags = 0;
        pxFirstHeader->xItemLen = xRemLen - rbHEADER_SIZE;  //Fill remaining length with first part
        pxRingbuffer->pucAcquire += rbHEADER_SIZE;            //Advance pucAcquire past header
        xRemLen -= rbHEADER_SIZE;
        if (xRemLen > 0) {
            memcpy(pxRingbuffer->pucAcquire, pucItem, xRemLen);
            pxRingbuffer->xItemsWaiting++;
            //Update item arguments to account for data already copied
            pucItem += xRemLen;
            xItemSize -= xRemLen;
            xAlignedItemSize -= xRemLen;
            pxFirstHeader->uxItemFlags |= rbITEM_SPLIT_FLAG;        //There must be more data
        } else {
            //Remaining length was only large enough to fit header
            pxFirstHeader->uxItemFlags |= rbITEM_DUMMY_DATA_FLAG;   //Item will completely be stored in 2nd part
        }
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;             //Reset acquire pointer to start of buffer
    }

    //Item (whole or second part) should be guaranteed to fit at this point
    ItemHeader_t *pxSecondHeader = (ItemHeader_t *)pxRingbuffer->pucAcquire;
    pxSecondHeader->xItemLen = xItemSize;
    pxSecondHeader->uxItemFlags = 0;
    pxRingbuffer->pucAcquire += rbHEADER_SIZE;     //Advance acquire pointer past header
    memcpy(pxRingbuffer->pucAcquire, pucItem, xItemSize);
    pxRingbuffer->xItemsWaiting++;
    pxRingbuffer->pucAcquire += xAlignedItemSize;  //Advance pucAcquire past item to next aligned address

    //If current remaining length can't fit a header, wrap around write pointer
    if (pxRingbuffer->pucTail - pxRingbuffer->pucAcquire < rbHEADER_SIZE) {
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;   //Wrap around pucAcquire
    }
    //Check if buffer is full
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucFree) {
        //Mark the buffer as full to distinguish with an empty buffer
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }

    //currently the Split mode is not supported, pucWrite tracks the pucAcquire
    pxRingbuffer->pucWrite = pxRingbuffer->pucAcquire;
}

static void prvCopyItemByteBuf(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(pxRingbuffer->pucAcquire >= pxRingbuffer->pucHead && pxRingbuffer->pucAcquire < pxRingbuffer->pucTail);    //Check acquire pointer is within bounds

    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucAcquire;    //Length from pucAcquire until end of buffer
    if (xRemLen < xItemSize) {
        //Copy as much as possible into remaining length
        memcpy(pxRingbuffer->pucAcquire, pucItem, xRemLen);
        pxRingbuffer->xItemsWaiting += xRemLen;
        //Update item arguments to account for data already written
        pucItem += xRemLen;
        xItemSize -= xRemLen;
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;     //Reset acquire pointer to start of buffer
    }
    //Copy all or remaining portion of the item
    memcpy(pxRingbuffer->pucAcquire, pucItem, xItemSize);
    pxRingbuffer->xItemsWaiting += xItemSize;
    pxRingbuffer->pucAcquire += xItemSize;

    //Wrap around pucAcquire if it reaches the end
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucTail) {
        pxRingbuffer->pucAcquire = pxRingbuffer->pucHead;
    }
    //Check if buffer is full
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucFree) {
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;      //Mark the buffer as full to avoid confusion with an empty buffer
    }

    //Currently, acquiring memory is not supported in byte mode. pucWrite tracks the pucAcquire.
    pxRingbuffer->pucWrite = pxRingbuffer->pucAcquire;
}

static BaseType_t prvCheckItemAvail(Ringbuffer_t *pxRingbuffer)
{
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && pxRingbuffer->pucRead != pxRingbuffer->pucFree) {
        return pdFALSE;     //Byte buffers do not allow multiple retrievals before return
    }
    if ((pxRingbuffer->xItemsWaiting > 0) && ((pxRingbuffer->pucRead != pxRingbuffer->pucWrite) || (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG))) {
        // If the ring buffer is a no-split buffer, the read pointer must point to an item that has been written to.
        if ((pxRingbuffer->uxRingbufferFlags & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG)) == 0) {
            ItemHeader_t *pxHeader = (ItemHeader_t *)pxRingbuffer->pucRead;
            if ((pxHeader->uxItemFlags & rbITEM_WRITTEN_FLAG) == 0) {
                return pdFALSE;
            }
        }
        return pdTRUE;      //Items/data available for retrieval
    } else {
        return pdFALSE;     //No items/data available for retrieval
    }
}

static void *prvGetItemDefault(Ringbuffer_t *pxRingbuffer,
                               BaseType_t *pxIsSplit,
                               size_t xUnusedParam,
                               size_t *pxItemSize)
{
    //Check arguments and buffer state
    ItemHeader_t *pxHeader = (ItemHeader_t *)pxRingbuffer->pucRead;
    configASSERT(pxIsSplit != NULL);
    configASSERT((pxRingbuffer->xItemsWaiting > 0) && ((pxRingbuffer->pucRead != pxRingbuffer->pucWrite) || (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG)));   //Check there are items to be read
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucRead));           //pucRead is always aligned in split ring buffers
    configASSERT(pxRingbuffer->pucRead >= pxRingbuffer->pucHead && pxRingbuffer->pucRead < pxRingbuffer->pucTail);      //Check read pointer is within bounds
    configASSERT((pxHeader->xItemLen <= pxRingbuffer->xMaxItemSize) || (pxHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG));

    uint8_t *pcReturn;
    //Wrap around if dummy data (dummy data indicates wrap around in no-split buffers)
    if (pxHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
        pxRingbuffer->pucRead = pxRingbuffer->pucHead;
        //Check for errors with the next item
        pxHeader = (ItemHeader_t *)pxRingbuffer->pucRead;
        configASSERT(pxHeader->xItemLen <= pxRingbuffer->xMaxItemSize);
    }
    pcReturn = pxRingbuffer->pucRead + rbHEADER_SIZE;    //Get pointer to part of item containing data (point past the header)
    if (pxHeader->xItemLen == 0) {
        //Inclusive of pucTail for special case where item of zero length just fits at the end of the buffer
        configASSERT(pcReturn >= pxRingbuffer->pucHead && pcReturn <= pxRingbuffer->pucTail);
    } else {
        //Exclusive of pucTail if length is larger than zero, pcReturn should never point to pucTail
        configASSERT(pcReturn >= pxRingbuffer->pucHead && pcReturn < pxRingbuffer->pucTail);
    }
    *pxItemSize = pxHeader->xItemLen;   //Get length of item
    pxRingbuffer->xItemsWaiting --;     //Update item count
    *pxIsSplit = (pxHeader->uxItemFlags & rbITEM_SPLIT_FLAG) ? pdTRUE : pdFALSE;

    pxRingbuffer->pucRead += rbHEADER_SIZE + rbALIGN_SIZE(pxHeader->xItemLen);   //Update pucRead
    //Check if pucRead requires wrap around
    if ((pxRingbuffer->pucTail - pxRingbuffer->pucRead) < rbHEADER_SIZE) {
        pxRingbuffer->pucRead = pxRingbuffer->pucHead;
    }
    return (void *)pcReturn;
}

static void *prvGetItemByteBuf(Ringbuffer_t *pxRingbuffer,
                               BaseType_t *pxUnusedParam,
                               size_t xMaxSize,
                               size_t *pxItemSize)
{
    //Check arguments and buffer state
    configASSERT((pxRingbuffer->xItemsWaiting > 0) && ((pxRingbuffer->pucRead != pxRingbuffer->pucWrite) || (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG)));   //Check there are items to be read
    configASSERT(pxRingbuffer->pucRead >= pxRingbuffer->pucHead && pxRingbuffer->pucRead < pxRingbuffer->pucTail);    //Check read pointer is within bounds
    configASSERT(pxRingbuffer->pucRead == pxRingbuffer->pucFree);

    uint8_t *ret = pxRingbuffer->pucRead;
    if ((pxRingbuffer->pucRead > pxRingbuffer->pucWrite) || (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG)) {     //Available data wraps around
        //Return contiguous piece from read pointer until buffer tail, or xMaxSize
        if (xMaxSize == 0 || pxRingbuffer->pucTail - pxRingbuffer->pucRead <= xMaxSize) {
            //All contiguous data from read pointer to tail
            *pxItemSize = pxRingbuffer->pucTail - pxRingbuffer->pucRead;
            pxRingbuffer->xItemsWaiting -= pxRingbuffer->pucTail - pxRingbuffer->pucRead;
            pxRingbuffer->pucRead = pxRingbuffer->pucHead;  //Wrap around read pointer
        } else {
            //Return xMaxSize amount of data
            *pxItemSize = xMaxSize;
            pxRingbuffer->xItemsWaiting -= xMaxSize;
            pxRingbuffer->pucRead += xMaxSize;  //Advance read pointer past retrieved data
        }
    } else {    //Available data is contiguous between read and write pointer
        if (xMaxSize == 0 || pxRingbuffer->pucWrite - pxRingbuffer->pucRead <= xMaxSize) {
            //Return all contiguous data from read to write pointer
            *pxItemSize = pxRingbuffer->pucWrite - pxRingbuffer->pucRead;
            pxRingbuffer->xItemsWaiting -= pxRingbuffer->pucWrite - pxRingbuffer->pucRead;
            pxRingbuffer->pucRead = pxRingbuffer->pucWrite;
        } else {
            //Return xMaxSize data from read pointer
            *pxItemSize = xMaxSize;
            pxRingbuffer->xItemsWaiting -= xMaxSize;
            pxRingbuffer->pucRead += xMaxSize;  //Advance read pointer past retrieved data

        }
    }
    return (void *)ret;
}

static void prvReturnItemDefault(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem)
{
    //Check arguments and buffer state
    configASSERT(rbCHECK_ALIGNED(pucItem));
    configASSERT(pucItem >= pxRingbuffer->pucHead);
    configASSERT(pucItem <= pxRingbuffer->pucTail);     //Inclusive of pucTail in the case of zero length item at the very end

    //Get and check header of the item
    ItemHeader_t *pxCurHeader = (ItemHeader_t *)(pucItem - rbHEADER_SIZE);
    configASSERT(pxCurHeader->xItemLen <= pxRingbuffer->xMaxItemSize);
    configASSERT((pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) == 0); //Dummy items should never have been read
    configASSERT((pxCurHeader->uxItemFlags & rbITEM_FREE_FLAG) == 0);       //Indicates item has already been returned before
    pxCurHeader->uxItemFlags &= ~rbITEM_SPLIT_FLAG;                         //Clear wrap flag if set (not strictly necessary)
    pxCurHeader->uxItemFlags |= rbITEM_FREE_FLAG;                           //Mark as free

    /*
     * Items might not be returned in the order they were retrieved. Move the free pointer
     * up to the next item that has not been marked as free (by free flag) or up
     * till the read pointer. When advancing the free pointer, items that have already been
     * freed or items with dummy data should be skipped over
     */
    pxCurHeader = (ItemHeader_t *)pxRingbuffer->pucFree;
    //Skip over Items that have already been freed or are dummy items
    while (((pxCurHeader->uxItemFlags & rbITEM_FREE_FLAG) || (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG)) && pxRingbuffer->pucFree != pxRingbuffer->pucRead) {
        if (pxCurHeader->uxItemFlags & rbITEM_DUMMY_DATA_FLAG) {
            pxCurHeader->uxItemFlags |= rbITEM_FREE_FLAG;   //Mark as freed (not strictly necessary but adds redundancy)
            pxRingbuffer->pucFree = pxRingbuffer->pucHead;    //Wrap around due to dummy data
        } else {
            //Item with data that has already been freed, advance free pointer past this item
            size_t xAlignedItemSize = rbALIGN_SIZE(pxCurHeader->xItemLen);
            pxRingbuffer->pucFree += xAlignedItemSize + rbHEADER_SIZE;
            //Redundancy check to ensure free pointer has not overshot buffer bounds
            configASSERT(pxRingbuffer->pucFree <= pxRingbuffer->pucHead + pxRingbuffer->xSize);
        }
        //Check if pucFree requires wrap around
        if ((pxRingbuffer->pucTail - pxRingbuffer->pucFree) < rbHEADER_SIZE) {
            pxRingbuffer->pucFree = pxRingbuffer->pucHead;
        }
        pxCurHeader = (ItemHeader_t *)pxRingbuffer->pucFree;      //Update header to point to item
    }

    //Check if the buffer full flag should be reset
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        if (pxRingbuffer->pucFree != pxRingbuffer->pucAcquire) {
            pxRingbuffer->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
        } else if (pxRingbuffer->pucFree == pxRingbuffer->pucAcquire && pxRingbuffer->pucFree == pxRingbuffer->pucRead) {
            //Special case where a full buffer is completely freed in one go
            pxRingbuffer->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
        }
    }
}

static void prvReturnItemByteBuf(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem)
{
    //Check pointer points to address inside buffer
    configASSERT((uint8_t *)pucItem >= pxRingbuffer->pucHead);
    configASSERT((uint8_t *)pucItem < pxRingbuffer->pucTail);
    //Free the read memory. Simply moves free pointer to read pointer as byte buffers do not allow multiple outstanding reads
    pxRingbuffer->pucFree = pxRingbuffer->pucRead;
    //If buffer was full before, reset full flag as free pointer has moved
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        pxRingbuffer->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
    }
}

static size_t prvGetCurMaxSizeNoSplit(Ringbuffer_t *pxRingbuffer)
{
    BaseType_t xFreeSize;
    //Check if buffer is full
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    if (pxRingbuffer->pucAcquire < pxRingbuffer->pucFree) {
        //Free space is contiguous between pucAcquire and pucFree
        xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucAcquire;
    } else {
        //Free space wraps around (or overlapped at pucHead), select largest
        //contiguous free space as no-split items require contiguous space
        size_t xSize1 = pxRingbuffer->pucTail - pxRingbuffer->pucAcquire;
        size_t xSize2 = pxRingbuffer->pucFree - pxRingbuffer->pucHead;
        xFreeSize = (xSize1 > xSize2) ? xSize1 : xSize2;
    }

    //No-split ring buffer items need space for a header
    xFreeSize -= rbHEADER_SIZE;

    //Check for xFreeSize < 0 before checking xFreeSize > pxRingbuffer->xMaxItemSize
    //to avoid incorrect comparison operation when xFreeSize is negative
    if (xFreeSize < 0) {
        //Occurs when free space is less than header size
        xFreeSize = 0;
    } else if (xFreeSize > pxRingbuffer->xMaxItemSize) {
        //Limit free size to be within bounds
        xFreeSize = pxRingbuffer->xMaxItemSize;
    }
    return xFreeSize;
}

static size_t prvGetCurMaxSizeAllowSplit(Ringbuffer_t *pxRingbuffer)
{
    BaseType_t xFreeSize;
    //Check if buffer is full
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }
    if (pxRingbuffer->pucAcquire == pxRingbuffer->pucHead && pxRingbuffer->pucFree == pxRingbuffer->pucHead) {
        //Check for special case where pucAcquire and pucFree are both at pucHead
        xFreeSize = pxRingbuffer->xSize - rbHEADER_SIZE;
    } else if (pxRingbuffer->pucAcquire < pxRingbuffer->pucFree) {
        //Free space is contiguous between pucAcquire and pucFree, requires single header
        xFreeSize = (pxRingbuffer->pucFree - pxRingbuffer->pucAcquire) - rbHEADER_SIZE;
    } else {
        //Free space wraps around, requires two headers
        xFreeSize = (pxRingbuffer->pucFree - pxRingbuffer->pucHead) +
                    (pxRingbuffer->pucTail - pxRingbuffer->pucAcquire) -
                    (rbHEADER_SIZE * 2);
    }

    //Check for xFreeSize < 0 before checking xFreeSize > pxRingbuffer->xMaxItemSize
    //to avoid incorrect comparison operation when xFreeSize is negative
    if (xFreeSize < 0) {
        xFreeSize = 0;
    } else if (xFreeSize > pxRingbuffer->xMaxItemSize) {
        //Limit free size to be within bounds
        xFreeSize = pxRingbuffer->xMaxItemSize;
    }
    return xFreeSize;
}

static size_t prvGetCurMaxSizeByteBuf(Ringbuffer_t *pxRingbuffer)
{
    BaseType_t xFreeSize;
    //Check if buffer is full
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        return 0;
    }

    /*
     * Return whatever space is available depending on relative positions of the free
     * pointer and Acquire pointer. There is no overhead of headers in this mode
     */
    xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucAcquire;
    if (xFreeSize <= 0) {
        xFreeSize += pxRingbuffer->xSize;
    }
    return xFreeSize;
}

static BaseType_t prvSendAcquireGeneric(Ringbuffer_t *pxRingbuffer,
                                        const void *pvItem,
                                        void **ppvItem,
                                        size_t xItemSize,
                                        TickType_t xTicksToWait)
{
    BaseType_t xReturn = pdFALSE;
    BaseType_t xExitLoop = pdFALSE;
    BaseType_t xEntryTimeSet = pdFALSE;
    BaseType_t xNotifyQueueSet = pdFALSE;
    TimeOut_t xTimeOut;

    while (xExitLoop == pdFALSE) {
        portENTER_CRITICAL(&pxRingbuffer->mux);
        if (pxRingbuffer->xCheckItemFits(pxRingbuffer, xItemSize) == pdTRUE) {
            //xItemSize will fit. Copy or acquire the buffer immediately
            if (ppvItem) {
                //Acquire the buffer
                *ppvItem = prvAcquireItemNoSplit(pxRingbuffer, xItemSize);
            } else {
                //Copy item into buffer
                pxRingbuffer->vCopyItem(pxRingbuffer, pvItem, xItemSize);
                if (pxRingbuffer->xQueueSet) {
                    //If ring buffer was added to a queue set, notify the queue set
                    xNotifyQueueSet = pdTRUE;
                } else {
                    //If a task was waiting for data to arrive on the ring buffer, unblock it immediately.
                    if (listLIST_IS_EMPTY(&pxRingbuffer->xTasksWaitingToReceive) == pdFALSE) {
                        if (xTaskRemoveFromEventList(&pxRingbuffer->xTasksWaitingToReceive) == pdTRUE) {
                            //The unblocked task will preempt us. Trigger a yield here.
                            portYIELD_WITHIN_API();
                        }
                    }
                }
            }
            xReturn = pdTRUE;
            xExitLoop = pdTRUE;
            goto loop_end;
        } else if (xTicksToWait == (TickType_t) 0) {
            //No block time. Return immediately.
            xExitLoop = pdTRUE;
            goto loop_end;
        } else if (xEntryTimeSet == pdFALSE) {
            //This is our first block. Set entry time
            vTaskInternalSetTimeOutState(&xTimeOut);
            xEntryTimeSet = pdTRUE;
        }

        if (xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE) {
            //Not timed out yet. Block the current task
            vTaskPlaceOnEventList(&pxRingbuffer->xTasksWaitingToSend, xTicksToWait);
            portYIELD_WITHIN_API();
        } else {
            //We have timed out
            xExitLoop = pdTRUE;
        }
loop_end:
        portEXIT_CRITICAL(&pxRingbuffer->mux);
    }
    //Defer notifying the queue set until we are outside the loop and critical section.
    if (xNotifyQueueSet == pdTRUE) {
        xQueueSend((QueueHandle_t)pxRingbuffer->xQueueSet, (QueueSetMemberHandle_t *)&pxRingbuffer, 0);
    }

    return xReturn;
}

static BaseType_t prvReceiveGeneric(Ringbuffer_t *pxRingbuffer,
                                    void **pvItem1,
                                    void **pvItem2,
                                    size_t *xItemSize1,
                                    size_t *xItemSize2,
                                    size_t xMaxSize,
                                    TickType_t xTicksToWait)
{
    BaseType_t xReturn = pdFALSE;
    BaseType_t xExitLoop = pdFALSE;
    BaseType_t xEntryTimeSet = pdFALSE;
    TimeOut_t xTimeOut;

    ESP_STATIC_ANALYZER_CHECK(!pvItem1 || !xItemSize1, pdFALSE);

    while (xExitLoop == pdFALSE) {
        portENTER_CRITICAL(&pxRingbuffer->mux);
        if (prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
            //Item/data is available for retrieval
            BaseType_t xIsSplit = pdFALSE;
            if (pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
                //Read up to xMaxSize bytes from byte buffer
                *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, NULL, xMaxSize, xItemSize1);
            } else {
                //Get (first) item from no-split/allow-split buffers
                *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize1);
            }
            //If split buffer, check for split items
            if (pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
                ESP_STATIC_ANALYZER_CHECK(!pvItem2 || !xItemSize2, pdFALSE);
                if (xIsSplit == pdTRUE) {
                    *pvItem2 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize2);
                    configASSERT(*pvItem2 < *pvItem1);  //Check wrap around has occurred
                    configASSERT(xIsSplit == pdFALSE);  //Second part should not have wrapped flag
                } else {
                    *pvItem2 = NULL;
                }
            }
            xReturn = pdTRUE;
            xExitLoop = pdTRUE;
            goto loop_end;
        } else if (xTicksToWait == (TickType_t) 0) {
            //No block time. Return immediately.
            xExitLoop = pdTRUE;
            goto loop_end;
        } else if (xEntryTimeSet == pdFALSE) {
            //This is our first block. Set entry time
            vTaskInternalSetTimeOutState(&xTimeOut);
            xEntryTimeSet = pdTRUE;
        }

        if (xTaskCheckForTimeOut(&xTimeOut, &xTicksToWait) == pdFALSE) {
            //Not timed out yet. Block the current task
            vTaskPlaceOnEventList(&pxRingbuffer->xTasksWaitingToReceive, xTicksToWait);
            portYIELD_WITHIN_API();
        } else {
            //We have timed out.
            xExitLoop = pdTRUE;
        }
loop_end:
        portEXIT_CRITICAL(&pxRingbuffer->mux);
    }

    return xReturn;
}

static BaseType_t prvReceiveGenericFromISR(Ringbuffer_t *pxRingbuffer,
                                           void **pvItem1,
                                           void **pvItem2,
                                           size_t *xItemSize1,
                                           size_t *xItemSize2,
                                           size_t xMaxSize)
{
    BaseType_t xReturn = pdFALSE;

    ESP_STATIC_ANALYZER_CHECK(!pvItem1 || !xItemSize1, pdFALSE);

    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    if (prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
        BaseType_t xIsSplit = pdFALSE;
        if (pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
            //Read up to xMaxSize bytes from byte buffer
            *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, NULL, xMaxSize, xItemSize1);
        } else {
            //Get (first) item from no-split/allow-split buffers
            *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize1);
        }
        //If split buffer, check for split items
        if (pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
            ESP_STATIC_ANALYZER_CHECK(!pvItem2 || !xItemSize2, pdFALSE);
            if (xIsSplit == pdTRUE) {
                *pvItem2 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize2);
                configASSERT(*pvItem2 < *pvItem1);  //Check wrap around has occurred
                configASSERT(xIsSplit == pdFALSE);  //Second part should not have wrapped flag
            } else {
                *pvItem2 = NULL;
            }
        }
        xReturn = pdTRUE;
    } else {
        xReturn = pdFALSE;
    }
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);

    return xReturn;
}

// ------------------------------------------------ Public Functions ---------------------------------------------------

RingbufHandle_t xRingbufferCreate(size_t xBufferSize, RingbufferType_t xBufferType)
{
    configASSERT(xBufferSize > 0);
    configASSERT(xBufferType < RINGBUF_TYPE_MAX);

    //Allocate memory
    if (xBufferType != RINGBUF_TYPE_BYTEBUF) {
        xBufferSize = rbALIGN_SIZE(xBufferSize);    //xBufferSize is rounded up for no-split/allow-split buffers
    }
    Ringbuffer_t *pxNewRingbuffer = calloc(1, sizeof(Ringbuffer_t));
    uint8_t *pucRingbufferStorage = malloc(xBufferSize);
    if (pxNewRingbuffer == NULL || pucRingbufferStorage == NULL) {
        goto err;
    }

    prvInitializeNewRingbuffer(xBufferSize, xBufferType, pxNewRingbuffer, pucRingbufferStorage);
    return (RingbufHandle_t)pxNewRingbuffer;

err:
    //An error has occurred, Free memory and return NULL
    free(pxNewRingbuffer);
    free(pucRingbufferStorage);
    return NULL;
}

RingbufHandle_t xRingbufferCreateNoSplit(size_t xItemSize, size_t xItemNum)
{
    return xRingbufferCreate((rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE) * xItemNum, RINGBUF_TYPE_NOSPLIT);
}

RingbufHandle_t xRingbufferCreateStatic(size_t xBufferSize,
                                        RingbufferType_t xBufferType,
                                        uint8_t *pucRingbufferStorage,
                                        StaticRingbuffer_t *pxStaticRingbuffer)
{
    //Check arguments
    configASSERT(xBufferSize > 0);
    configASSERT(xBufferType < RINGBUF_TYPE_MAX);
    configASSERT(pucRingbufferStorage != NULL && pxStaticRingbuffer != NULL);
    if (xBufferType != RINGBUF_TYPE_BYTEBUF) {
        //No-split/allow-split buffer sizes must be 32-bit aligned
        configASSERT(rbCHECK_ALIGNED(xBufferSize));
    }

    Ringbuffer_t *pxNewRingbuffer = (Ringbuffer_t *)pxStaticRingbuffer;
    prvInitializeNewRingbuffer(xBufferSize, xBufferType, pxNewRingbuffer, pucRingbufferStorage);
    pxNewRingbuffer->uxRingbufferFlags |= rbBUFFER_STATIC_FLAG;
    return (RingbufHandle_t)pxNewRingbuffer;
}

BaseType_t xRingbufferSendAcquire(RingbufHandle_t xRingbuffer, void **ppvItem, size_t xItemSize, TickType_t xTicksToWait)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer);
    configASSERT(ppvItem != NULL);
    configASSERT((pxRingbuffer->uxRingbufferFlags & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG)) == 0); //Send acquire currently only supported in NoSplit buffers

    *ppvItem = NULL;
    if (xItemSize > pxRingbuffer->xMaxItemSize) {
        return pdFALSE;     //Data will never ever fit in the queue.
    }

    return prvSendAcquireGeneric(pxRingbuffer, NULL, ppvItem, xItemSize, xTicksToWait);
}

BaseType_t xRingbufferSendComplete(RingbufHandle_t xRingbuffer, void *pvItem)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    BaseType_t xNotifyQueueSet = pdFALSE;

    //Check arguments
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL);
    configASSERT((pxRingbuffer->uxRingbufferFlags & (rbBYTE_BUFFER_FLAG | rbALLOW_SPLIT_FLAG)) == 0);

    portENTER_CRITICAL(&pxRingbuffer->mux);
    prvSendItemDoneNoSplit(pxRingbuffer, pvItem);
    if (pxRingbuffer->xQueueSet) {
        //If ring buffer was added to a queue set, notify the queue set
        xNotifyQueueSet = pdTRUE;
    } else {
        //If a task was waiting for data to arrive on the ring buffer, unblock it immediately.
        if (listLIST_IS_EMPTY(&pxRingbuffer->xTasksWaitingToReceive) == pdFALSE) {
            if (xTaskRemoveFromEventList(&pxRingbuffer->xTasksWaitingToReceive) == pdTRUE) {
                //The unblocked task will preempt us. Trigger a yield here.
                portYIELD_WITHIN_API();
            }
        }
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);

    if (xNotifyQueueSet == pdTRUE) {
        xQueueSend((QueueHandle_t)pxRingbuffer->xQueueSet, (QueueSetMemberHandle_t *)&pxRingbuffer, 0);
    }
    return pdTRUE;
}

BaseType_t xRingbufferSend(RingbufHandle_t xRingbuffer,
                           const void *pvItem,
                           size_t xItemSize,
                           TickType_t xTicksToWait)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL || xItemSize == 0);
    if (xItemSize > pxRingbuffer->xMaxItemSize) {
        return pdFALSE;     //Data will never ever fit in the queue.
    }
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && xItemSize == 0) {
        return pdTRUE;      //Sending 0 bytes to byte buffer has no effect
    }

    return prvSendAcquireGeneric(pxRingbuffer, pvItem, NULL, xItemSize, xTicksToWait);
}

BaseType_t xRingbufferSendFromISR(RingbufHandle_t xRingbuffer,
                                  const void *pvItem,
                                  size_t xItemSize,
                                  BaseType_t *pxHigherPriorityTaskWoken)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    BaseType_t xNotifyQueueSet = pdFALSE;
    BaseType_t xReturn;

    //Check arguments
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL || xItemSize == 0);
    if (xItemSize > pxRingbuffer->xMaxItemSize) {
        return pdFALSE;     //Data will never ever fit in the queue.
    }
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && xItemSize == 0) {
        return pdTRUE;      //Sending 0 bytes to byte buffer has no effect
    }

    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    if (pxRingbuffer->xCheckItemFits(xRingbuffer, xItemSize) == pdTRUE) {
        pxRingbuffer->vCopyItem(xRingbuffer, pvItem, xItemSize);
        if (pxRingbuffer->xQueueSet) {
            //If ring buffer was added to a queue set, notify the queue set
            xNotifyQueueSet = pdTRUE;
        } else {
            //If a task was waiting for data to arrive on the ring buffer, unblock it immediately.
            if (listLIST_IS_EMPTY(&pxRingbuffer->xTasksWaitingToReceive) == pdFALSE) {
                if (xTaskRemoveFromEventList(&pxRingbuffer->xTasksWaitingToReceive) == pdTRUE) {
                    //The unblocked task will preempt us. Record that a context switch is required.
                    if (pxHigherPriorityTaskWoken != NULL) {
                        *pxHigherPriorityTaskWoken = pdTRUE;
                    }
                }
            }
        }
        xReturn = pdTRUE;
    } else {
        xReturn = pdFALSE;
    }
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);
    //Defer notifying the queue set until we are outside the critical section.
    if (xNotifyQueueSet == pdTRUE) {
        xQueueSendFromISR((QueueHandle_t)pxRingbuffer->xQueueSet, (QueueSetMemberHandle_t *)&pxRingbuffer, pxHigherPriorityTaskWoken);
    }
    return xReturn;
}

void *xRingbufferReceive(RingbufHandle_t xRingbuffer, size_t *pxItemSize, TickType_t xTicksToWait)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && pxItemSize);

    //Attempt to retrieve an item
    void *pvTempItem;
    if (prvReceiveGeneric(pxRingbuffer, &pvTempItem, NULL, pxItemSize, NULL, 0, xTicksToWait) == pdTRUE) {
        return pvTempItem;
    } else {
        return NULL;
    }
}

void *xRingbufferReceiveFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && pxItemSize);

    //Attempt to retrieve an item
    void *pvTempItem;
    if (prvReceiveGenericFromISR(pxRingbuffer, &pvTempItem, NULL, pxItemSize, NULL, 0) == pdTRUE) {
        return pvTempItem;
    } else {
        return NULL;
    }
}

BaseType_t xRingbufferReceiveSplit(RingbufHandle_t xRingbuffer,
                                   void **ppvHeadItem,
                                   void **ppvTailItem,
                                   size_t *pxHeadItemSize,
                                   size_t *pxTailItemSize,
                                   TickType_t xTicksToWait)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && ppvHeadItem && ppvTailItem && pxHeadItemSize && pxTailItemSize);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG);

    return prvReceiveGeneric(pxRingbuffer, ppvHeadItem, ppvTailItem, pxHeadItemSize, pxTailItemSize, 0, xTicksToWait);
}

BaseType_t xRingbufferReceiveSplitFromISR(RingbufHandle_t xRingbuffer,
                                          void **ppvHeadItem,
                                          void **ppvTailItem,
                                          size_t *pxHeadItemSize,
                                          size_t *pxTailItemSize)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && ppvHeadItem && ppvTailItem && pxHeadItemSize && pxTailItemSize);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG);

    return prvReceiveGenericFromISR(pxRingbuffer, ppvHeadItem, ppvTailItem, pxHeadItemSize, pxTailItemSize, 0);
}

void *xRingbufferReceiveUpTo(RingbufHandle_t xRingbuffer,
                             size_t *pxItemSize,
                             TickType_t xTicksToWait,
                             size_t xMaxSize)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && pxItemSize);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG);    //This function should only be called for byte buffers

    if (xMaxSize == 0) {
        return NULL;
    }
    //Attempt to retrieve up to xMaxSize bytes
    void *pvTempItem;
    if (prvReceiveGeneric(pxRingbuffer, &pvTempItem, NULL, pxItemSize, NULL, xMaxSize, xTicksToWait) == pdTRUE) {
        return pvTempItem;
    } else {
        return NULL;
    }
}

void *xRingbufferReceiveUpToFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize, size_t xMaxSize)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;

    //Check arguments
    configASSERT(pxRingbuffer && pxItemSize);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG);    //This function should only be called for byte buffers

    if (xMaxSize == 0) {
        return NULL;
    }
    //Attempt to retrieve up to xMaxSize bytes
    void *pvTempItem;
    if (prvReceiveGenericFromISR(pxRingbuffer, &pvTempItem, NULL, pxItemSize, NULL, xMaxSize) == pdTRUE) {
        return pvTempItem;
    } else {
        return NULL;
    }
}

void vRingbufferReturnItem(RingbufHandle_t xRingbuffer, void *pvItem)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL);

    portENTER_CRITICAL(&pxRingbuffer->mux);
    pxRingbuffer->vReturnItem(pxRingbuffer, (uint8_t *)pvItem);
    //If a task was waiting for space to send, unblock it immediately.
    if (listLIST_IS_EMPTY(&pxRingbuffer->xTasksWaitingToSend) == pdFALSE) {
        if (xTaskRemoveFromEventList(&pxRingbuffer->xTasksWaitingToSend) == pdTRUE) {
            //The unblocked task will preempt us. Trigger a yield here.
            portYIELD_WITHIN_API();
        }
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
}

void vRingbufferReturnItemFromISR(RingbufHandle_t xRingbuffer, void *pvItem, BaseType_t *pxHigherPriorityTaskWoken)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL);

    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    pxRingbuffer->vReturnItem(pxRingbuffer, (uint8_t *)pvItem);
    //If a task was waiting for space to send, unblock it immediately.
    if (listLIST_IS_EMPTY(&pxRingbuffer->xTasksWaitingToSend) == pdFALSE) {
        if (xTaskRemoveFromEventList(&pxRingbuffer->xTasksWaitingToSend) == pdTRUE) {
            //The unblocked task will preempt us. Record that a context switch is required.
            if (pxHigherPriorityTaskWoken != NULL) {
                *pxHigherPriorityTaskWoken = pdTRUE;
            }
        }
    }
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);
}

void vRingbufferDelete(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    //Ring buffer was not statically allocated. Free its memory.
    if (!(pxRingbuffer->uxRingbufferFlags & rbBUFFER_STATIC_FLAG)) {
        free(pxRingbuffer->pucHead);
        free(pxRingbuffer);
    }
}

size_t xRingbufferGetMaxItemSize(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    return pxRingbuffer->xMaxItemSize;
}

size_t xRingbufferGetCurFreeSize(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    size_t xFreeSize;
    portENTER_CRITICAL(&pxRingbuffer->mux);
    xFreeSize = pxRingbuffer->xGetCurMaxSize(pxRingbuffer);
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return xFreeSize;
}

BaseType_t xRingbufferAddToQueueSetRead(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    BaseType_t xReturn;

    configASSERT(pxRingbuffer && xQueueSet);

    portENTER_CRITICAL(&pxRingbuffer->mux);
    if (pxRingbuffer->xQueueSet != NULL || prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
        /*
        - Cannot add ring buffer to more than one queue set
        - It is dangerous to add a ring buffer to a queue set if the ring buffer currently has data to be read.
        */
        xReturn = pdFALSE;
    } else {
        //Add ring buffer to queue set
        pxRingbuffer->xQueueSet = xQueueSet;
        xReturn = pdTRUE;
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);

    return xReturn;
}

BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    BaseType_t xReturn;

    configASSERT(pxRingbuffer && xQueueSet);

    portENTER_CRITICAL(&pxRingbuffer->mux);
    if (pxRingbuffer->xQueueSet != xQueueSet || prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
        /*
        - Ring buffer was never added to this queue set
        - It is dangerous to remove a ring buffer from a queue set if the ring buffer currently has data to be read.
        */
        xReturn = pdFALSE;
    } else {
        //Remove ring buffer from queue set
        pxRingbuffer->xQueueSet = NULL;
        xReturn = pdTRUE;
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);

    return xReturn;
}

void vRingbufferGetInfo(RingbufHandle_t xRingbuffer,
                        UBaseType_t *uxFree,
                        UBaseType_t *uxRead,
                        UBaseType_t *uxWrite,
                        UBaseType_t *uxAcquire,
                        UBaseType_t *uxItemsWaiting)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    portENTER_CRITICAL(&pxRingbuffer->mux);
    if (uxFree != NULL) {
        *uxFree = (UBaseType_t)(pxRingbuffer->pucFree - pxRingbuffer->pucHead);
    }
    if (uxRead != NULL) {
        *uxRead = (UBaseType_t)(pxRingbuffer->pucRead - pxRingbuffer->pucHead);
    }
    if (uxWrite != NULL) {
        *uxWrite = (UBaseType_t)(pxRingbuffer->pucWrite - pxRingbuffer->pucHead);
    }
    if (uxAcquire != NULL) {
        *uxAcquire = (UBaseType_t)(pxRingbuffer->pucAcquire - pxRingbuffer->pucHead);
    }
    if (uxItemsWaiting != NULL) {
        *uxItemsWaiting = (UBaseType_t)(pxRingbuffer->xItemsWaiting);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
}

void xRingbufferPrintInfo(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    printf("Rb size:%" PRId32 "\tfree: %" PRId32 "\trptr: %" PRId32 "\tfreeptr: %" PRId32 "\twptr: %" PRId32 ", aptr: %" PRId32 "\n",
           (int32_t)pxRingbuffer->xSize, (int32_t)prvGetFreeSize(pxRingbuffer),
           (int32_t)(pxRingbuffer->pucRead - pxRingbuffer->pucHead),
           (int32_t)(pxRingbuffer->pucFree - pxRingbuffer->pucHead),
           (int32_t)(pxRingbuffer->pucWrite - pxRingbuffer->pucHead),
           (int32_t)(pxRingbuffer->pucAcquire - pxRingbuffer->pucHead));
}

BaseType_t xRingbufferGetStaticBuffer(RingbufHandle_t xRingbuffer, uint8_t **ppucRingbufferStorage, StaticRingbuffer_t **ppxStaticRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    BaseType_t xReturn;

    configASSERT(pxRingbuffer && ppucRingbufferStorage && ppxStaticRingbuffer);

    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_STATIC_FLAG) {
        *ppucRingbufferStorage = pxRingbuffer->pucHead;
        *ppxStaticRingbuffer = (StaticRingbuffer_t *)pxRingbuffer;
        xReturn = pdTRUE;
    } else {
        xReturn = pdFALSE;
    }

    return xReturn;
}

RingbufHandle_t xRingbufferCreateWithCaps(size_t xBufferSize, RingbufferType_t xBufferType, UBaseType_t uxMemoryCaps)
{
    RingbufHandle_t xRingbuffer;
    StaticRingbuffer_t *pxStaticRingbuffer;
    uint8_t *pucRingbufferStorage;

    //Allocate memory
    if (xBufferType != RINGBUF_TYPE_BYTEBUF) {
        xBufferSize = rbALIGN_SIZE(xBufferSize);    //xBufferSize is rounded up for no-split/allow-split buffers
    }

    pxStaticRingbuffer = heap_caps_malloc(sizeof(StaticRingbuffer_t), (uint32_t)uxMemoryCaps);
    pucRingbufferStorage = heap_caps_malloc(xBufferSize, (uint32_t)uxMemoryCaps);

    if (pxStaticRingbuffer == NULL || pucRingbufferStorage == NULL) {
        goto err;
    }

    // Create the ring buffer using static creation API
    xRingbuffer = xRingbufferCreateStatic(xBufferSize, xBufferType, pucRingbufferStorage, pxStaticRingbuffer);
    if (xRingbuffer == NULL) {
        goto err;
    }

    return xRingbuffer;

err:
    heap_caps_free(pxStaticRingbuffer);
    heap_caps_free(pucRingbufferStorage);
    return NULL;
}

void vRingbufferDeleteWithCaps(RingbufHandle_t xRingbuffer)
{
    // Return value unused if asserts are disabled
    BaseType_t __attribute__((unused)) xResult;
    StaticRingbuffer_t *pxStaticRingbuffer = NULL;
    uint8_t *pucRingbufferStorage = NULL;

    // Retrieve the buffers used to create the ring buffer before deleting it
    xResult = xRingbufferGetStaticBuffer(xRingbuffer, &pucRingbufferStorage, &pxStaticRingbuffer);
    configASSERT(xResult == pdTRUE);

    // Delete the ring buffer
    vRingbufferDelete(xRingbuffer);

    // Free the memory buffers
    heap_caps_free(pxStaticRingbuffer);
    heap_caps_free(pucRingbufferStorage);
}
