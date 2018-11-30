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

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"

//32-bit alignment macros
#define rbALIGN_SIZE( xSize )       ( ( xSize + portBYTE_ALIGNMENT_MASK ) & ~portBYTE_ALIGNMENT_MASK )
#define rbCHECK_ALIGNED( pvPtr )    ( ( ( UBaseType_t ) pvPtr & portBYTE_ALIGNMENT_MASK ) == 0 )

//Ring buffer flags
#define rbALLOW_SPLIT_FLAG          ( ( UBaseType_t ) 1 )   //The ring buffer allows items to be split
#define rbBYTE_BUFFER_FLAG          ( ( UBaseType_t ) 2 )   //The ring buffer is a byte buffer
#define rbBUFFER_FULL_FLAG          ( ( UBaseType_t ) 4 )   //The ring buffer is currently full (write pointer == free pointer)

//Item flags
#define rbITEM_FREE_FLAG            ( ( UBaseType_t ) 1 )   //Item has been retrieved and returned by application, free to overwrite
#define rbITEM_DUMMY_DATA_FLAG      ( ( UBaseType_t ) 2 )   //Data from here to end of the ring buffer is dummy data. Restart reading at start of head of the buffer
#define rbITEM_SPLIT_FLAG           ( ( UBaseType_t ) 4 )   //Valid for RINGBUF_TYPE_ALLOWSPLIT, indicating that rest of the data is wrapped around

typedef struct {
    //This size of this structure must be 32-bit aligned
    size_t xItemLen;
    UBaseType_t uxItemFlags;
} ItemHeader_t;

#define rbHEADER_SIZE     sizeof(ItemHeader_t)
typedef struct Ringbuffer_t Ringbuffer_t;
typedef BaseType_t (*CheckItemFitsFunction_t)(Ringbuffer_t *pxRingbuffer, size_t xItemSize);
typedef void (*CopyItemFunction_t)(Ringbuffer_t *pxRingbuffer, const uint8_t *pcItem, size_t xItemSize);
typedef BaseType_t (*CheckItemAvailFunction_t) (Ringbuffer_t *pxRingbuffer);
typedef void *(*GetItemFunction_t)(Ringbuffer_t *pxRingbuffer, BaseType_t *pxIsSplit, size_t xMaxSize, size_t *pxItemSize);
typedef void (*ReturnItemFunction_t)(Ringbuffer_t *pxRingbuffer, uint8_t *pvItem);
typedef size_t (*GetCurMaxSizeFunction_t)(Ringbuffer_t *pxRingbuffer);

struct Ringbuffer_t {
    size_t xSize;                               //Size of the data storage
    UBaseType_t uxRingbufferFlags;              //Flags to indicate the type and status of ring buffer
    size_t xMaxItemSize;                        //Maximum item size

    CheckItemFitsFunction_t xCheckItemFits;     //Function to check if item can currently fit in ring buffer
    CopyItemFunction_t vCopyItem;               //Function to copy item to ring buffer
    GetItemFunction_t pvGetItem;                //Function to get item from ring buffer
    ReturnItemFunction_t vReturnItem;           //Function to return item to ring buffer
    GetCurMaxSizeFunction_t xGetCurMaxSize;     //Function to get current free size

    uint8_t *pucWrite;                          //Write Pointer. Points to where the next item should be written
    uint8_t *pucRead;                           //Read Pointer. Points to where the next item should be read from
    uint8_t *pucFree;                           //Free Pointer. Points to the last item that has yet to be returned to the ring buffer
    uint8_t *pucHead;                           //Pointer to the start of the ring buffer storage area
    uint8_t *pucTail;                           //Pointer to the end of the ring buffer storage area

    BaseType_t xItemsWaiting;                   //Number of items/bytes(for byte buffers) currently in ring buffer that have not yet been read
    SemaphoreHandle_t xFreeSpaceSemaphore;      //Binary semaphore, wakes up writing threads when more free space becomes available or when another thread times out attempting to write
    SemaphoreHandle_t xItemsBufferedSemaphore;  //Binary semaphore, indicates there are new packets in the circular buffer. See remark.
    portMUX_TYPE mux;                           //Spinlock required for SMP
};

/*
Remark: A counting semaphore for items_buffered_sem would be more logical, but counting semaphores in
FreeRTOS need a maximum count, and allocate more memory the larger the maximum count is. Here, we
would need to set the maximum to the maximum amount of times a null-byte unit first in the buffer,
which is quite high and so would waste a fair amount of memory.
*/

/* ------------------------------------------------ Static Declarations ------------------------------------------ */
/*
 * WARNING: All of the following static functions (except generic functions)
 * ARE NOT THREAD SAFE. Therefore they should only be called within a critical
 * section (using spin locks)
 */

//Calculate current amount of free space (in bytes) in the ring buffer
static size_t prvGetFreeSize(Ringbuffer_t *pxRingbuffer);

//Checks if an item/data is currently available for retrieval
static BaseType_t prvCheckItemAvail(Ringbuffer_t *pxRingbuffer);

//Checks if an item will currently fit in a no-split/allow-split ring buffer
static BaseType_t prvCheckItemFitsDefault( Ringbuffer_t *pxRingbuffer, size_t xItemSize);

//Checks if an item will currently fit in a byte buffer
static BaseType_t prvCheckItemFitsByteBuffer( Ringbuffer_t *pxRingbuffer, size_t xItemSize);

//Copies an item to a no-split ring buffer. Only call this function after calling prvCheckItemFitsDefault()
static void prvCopyItemNoSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

//Copies an item to a allow-split ring buffer. Only call this function after calling prvCheckItemFitsDefault()
static void prvCopyItemAllowSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

//Copies an item to a byte buffer. Only call this function  after calling prvCheckItemFitsByteBuffer()
static void prvCopyItemByteBuf(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize);

//Retrieve item from no-split/allow-split ring buffer. *pxIsSplit is set to pdTRUE if the retrieved item is split
static void *prvGetItemDefault(Ringbuffer_t *pxRingbuffer, BaseType_t *pxIsSplit, size_t xUnusedParam, size_t *pxItemSize);

//Retrieve data from byte buffer. If xMaxSize is 0, all continuous data is retrieved
static void *prvGetItemByteBuf(Ringbuffer_t *pxRingbuffer, BaseType_t *pxUnusedParam ,size_t xMaxSize,  size_t *pxItemSize);

//Return an item to a split/no-split ring buffer
static void prvReturnItemDefault(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem);

//Return data to a byte buffer
static void prvReturnItemByteBuf(Ringbuffer_t *pxRingbuffer, uint8_t *pucItem);

//Get the maximum size an item that can currently have if sent to a no-split ring buffer
static size_t prvGetCurMaxSizeNoSplit(Ringbuffer_t *pxRingbuffer);

//Get the maximum size an item that can currently have if sent to a allow-split ring buffer
static size_t prvGetCurMaxSizeAllowSplit(Ringbuffer_t *pxRingbuffer);

//Get the maximum size an item that can currently have if sent to a byte buffer
static size_t prvGetCurMaxSizeByteBuf(Ringbuffer_t *pxRingbuffer);

/**
 * Generic function used to retrieve an item/data from ring buffers. If called on
 * an allow-split buffer, and pvItem2 and xItemSize2 are not NULL, both parts of
 * a split item will be retrieved. xMaxSize will only take effect if called on
 * byte buffers.
 */
static BaseType_t prvReceiveGeneric(Ringbuffer_t *pxRingbuffer, void **pvItem1, void **pvItem2, size_t *xItemSize1, size_t *xItemSize2, size_t xMaxSize, TickType_t xTicksToWait);

//Generic function used to retrieve an item/data from ring buffers in an ISR
static BaseType_t prvReceiveGenericFromISR(Ringbuffer_t *pxRingbuffer, void **pvItem1, void **pvItem2, size_t *xItemSize1, size_t *xItemSize2, size_t xMaxSize);

/* ------------------------------------------------ Static Definitions ------------------------------------------- */

static size_t prvGetFreeSize(Ringbuffer_t *pxRingbuffer)
{
    size_t xReturn;
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        xReturn =  0;
    } else {
        BaseType_t xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucWrite;
        //Check if xFreeSize has underflowed
        if (xFreeSize <= 0) {
            xFreeSize += pxRingbuffer->xSize;
        }
        xReturn = xFreeSize;
    }
    configASSERT(xReturn <= pxRingbuffer->xSize);
    return xReturn;
}

static BaseType_t prvCheckItemFitsDefault( Ringbuffer_t *pxRingbuffer, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucWrite));              //pucWrite is always aligned in no-split ring buffers
    configASSERT(pxRingbuffer->pucWrite >= pxRingbuffer->pucHead && pxRingbuffer->pucWrite < pxRingbuffer->pucTail);    //Check write pointer is within bounds

    size_t xTotalItemSize = rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE;    //Rounded up aligned item size with header
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucFree) {
        //Buffer is either complete empty or completely full
        return (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? pdFALSE : pdTRUE;
    }
    if (pxRingbuffer->pucFree > pxRingbuffer->pucWrite) {
        //Free space does not wrap around
        return (xTotalItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucWrite) ? pdTRUE : pdFALSE;
    }
    //Free space wraps around
    if (xTotalItemSize <= pxRingbuffer->pucTail - pxRingbuffer->pucWrite) {
        return pdTRUE;      //Item fits without wrapping around
    }
    //Check if item fits by wrapping
    if (pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) {
        //Allow split wrapping incurs an extra header
        return (xTotalItemSize + rbHEADER_SIZE <= pxRingbuffer->xSize - (pxRingbuffer->pucWrite - pxRingbuffer->pucFree)) ? pdTRUE : pdFALSE;
    } else {
        return (xTotalItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucHead) ? pdTRUE : pdFALSE;
    }
}

static BaseType_t prvCheckItemFitsByteBuffer( Ringbuffer_t *pxRingbuffer, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(pxRingbuffer->pucWrite >= pxRingbuffer->pucHead && pxRingbuffer->pucWrite < pxRingbuffer->pucTail);    //Check write pointer is within bounds

    if (pxRingbuffer->pucWrite == pxRingbuffer->pucFree) {
        //Buffer is either complete empty or completely full
        return (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) ? pdFALSE : pdTRUE;
    }
    if (pxRingbuffer->pucFree > pxRingbuffer->pucWrite) {
        //Free space does not wrap around
        return (xItemSize <= pxRingbuffer->pucFree - pxRingbuffer->pucWrite) ? pdTRUE : pdFALSE;
    }
    //Free space wraps around
    return (xItemSize <= pxRingbuffer->xSize - (pxRingbuffer->pucWrite - pxRingbuffer->pucFree)) ? pdTRUE : pdFALSE;
}

static void prvCopyItemNoSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    //Check arguments and buffer state
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);                  //Rounded up aligned item size
    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucWrite;    //Length from pucWrite until end of buffer
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucWrite));              //pucWrite is always aligned in no-split ring buffers
    configASSERT(pxRingbuffer->pucWrite >= pxRingbuffer->pucHead && pxRingbuffer->pucWrite < pxRingbuffer->pucTail);    //Check write pointer is within bounds
    configASSERT(xRemLen >= rbHEADER_SIZE);                             //Remaining length must be able to at least fit an item header

    //If remaining length can't fit item, set as dummy data and wrap around
    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        ItemHeader_t *pxDummy = (ItemHeader_t *)pxRingbuffer->pucWrite;
        pxDummy->uxItemFlags = rbITEM_DUMMY_DATA_FLAG;      //Set remaining length as dummy data
        pxDummy->xItemLen = 0;                              //Dummy data should have no length
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;     //Reset write pointer to wrap around
    }

    //Item should be guaranteed to fit at this point. Set item header and copy data
    ItemHeader_t *pxHeader = (ItemHeader_t *)pxRingbuffer->pucWrite;
    pxHeader->xItemLen = xItemSize;
    pxHeader->uxItemFlags = 0;
    pxRingbuffer->pucWrite += rbHEADER_SIZE;    //Advance pucWrite past header
    memcpy(pxRingbuffer->pucWrite, pucItem, xItemSize);
    pxRingbuffer->xItemsWaiting++;
    pxRingbuffer->pucWrite += xAlignedItemSize; //Advance pucWrite past item to next aligned address

    //If current remaining length can't fit a header, wrap around write pointer
    if (pxRingbuffer->pucTail - pxRingbuffer->pucWrite < rbHEADER_SIZE) {
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;   //Wrap around pucWrite
    }
    //Check if buffer is full
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucFree) {
        //Mark the buffer as full to distinguish with an empty buffer
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
}

static void prvCopyItemAllowSplit(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    //Check arguments and buffer state
    size_t xAlignedItemSize = rbALIGN_SIZE(xItemSize);                  //Rounded up aligned item size
    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucWrite;    //Length from pucWrite until end of buffer
    configASSERT(rbCHECK_ALIGNED(pxRingbuffer->pucWrite));              //pucWrite is always aligned in split ring buffers
    configASSERT(pxRingbuffer->pucWrite >= pxRingbuffer->pucHead && pxRingbuffer->pucWrite < pxRingbuffer->pucTail);    //Check write pointer is within bounds
    configASSERT(xRemLen >= rbHEADER_SIZE);                             //Remaining length must be able to at least fit an item header

    //Split item if necessary
    if (xRemLen < xAlignedItemSize + rbHEADER_SIZE) {
        //Write first part of the item
        ItemHeader_t *pxFirstHeader = (ItemHeader_t *)pxRingbuffer->pucWrite;
        pxFirstHeader->uxItemFlags = 0;
        pxFirstHeader->xItemLen = xRemLen - rbHEADER_SIZE;  //Fill remaining length with first part
        pxRingbuffer->pucWrite += rbHEADER_SIZE;            //Advance pucWrite past header
        xRemLen -= rbHEADER_SIZE;
        if (xRemLen > 0) {
            memcpy(pxRingbuffer->pucWrite, pucItem, xRemLen);
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
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;             //Reset write pointer to start of buffer
    }

    //Item (whole or second part) should be guaranteed to fit at this point
    ItemHeader_t *pxSecondHeader = (ItemHeader_t *)pxRingbuffer->pucWrite;
    pxSecondHeader->xItemLen = xItemSize;
    pxSecondHeader->uxItemFlags = 0;
    pxRingbuffer->pucWrite += rbHEADER_SIZE;     //Advance write pointer past header
    memcpy(pxRingbuffer->pucWrite, pucItem, xItemSize);
    pxRingbuffer->xItemsWaiting++;
    pxRingbuffer->pucWrite += xAlignedItemSize;  //Advance pucWrite past item to next aligned address

    //If current remaining length can't fit a header, wrap around write pointer
    if (pxRingbuffer->pucTail - pxRingbuffer->pucWrite < rbHEADER_SIZE) {
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;   //Wrap around pucWrite
    }
    //Check if buffer is full
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucFree) {
        //Mark the buffer as full to distinguish with an empty buffer
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;
    }
}

static void prvCopyItemByteBuf(Ringbuffer_t *pxRingbuffer, const uint8_t *pucItem, size_t xItemSize)
{
    //Check arguments and buffer state
    configASSERT(pxRingbuffer->pucWrite >= pxRingbuffer->pucHead && pxRingbuffer->pucWrite < pxRingbuffer->pucTail);    //Check write pointer is within bounds

    size_t xRemLen = pxRingbuffer->pucTail - pxRingbuffer->pucWrite;    //Length from pucWrite until end of buffer
    if (xRemLen < xItemSize) {
        //Copy as much as possible into remaining length
        memcpy(pxRingbuffer->pucWrite, pucItem, xRemLen);
        pxRingbuffer->xItemsWaiting += xRemLen;
        //Update item arguments to account for data already written
        pucItem += xRemLen;
        xItemSize -= xRemLen;
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;     //Reset write pointer to start of buffer
    }
    //Copy all or remaining portion of the item
    memcpy(pxRingbuffer->pucWrite, pucItem, xItemSize);
    pxRingbuffer->xItemsWaiting += xItemSize;
    pxRingbuffer->pucWrite += xItemSize;

    //Wrap around pucWrite if it reaches the end
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucTail) {
        pxRingbuffer->pucWrite = pxRingbuffer->pucHead;
    }
    //Check if buffer is full
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucFree) {
        pxRingbuffer->uxRingbufferFlags |= rbBUFFER_FULL_FLAG;      //Mark the buffer as full to avoid confusion with an empty buffer
    }
}

static BaseType_t prvCheckItemAvail(Ringbuffer_t *pxRingbuffer)
{
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && pxRingbuffer->pucRead != pxRingbuffer->pucFree) {
        return pdFALSE;     //Byte buffers do not allow multiple retrievals before return
    }
    if ((pxRingbuffer->xItemsWaiting > 0) && ((pxRingbuffer->pucRead != pxRingbuffer->pucWrite) || (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG))) {
        return pdTRUE;      //Items/data available for retrieval
    } else {
        return pdFALSE;     //No items/data available for retrieval
    }
}

static void *prvGetItemDefault(Ringbuffer_t *pxRingbuffer, BaseType_t *pxIsSplit, size_t xUnusedParam, size_t *pxItemSize)
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
        //Exclusive of pucTali if length is larger than zero, pcReturn should never point to pucTail
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

static void *prvGetItemByteBuf(Ringbuffer_t *pxRingbuffer, BaseType_t *pxUnusedParam ,size_t xMaxSize,  size_t *pxItemSize)
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
        //Check if pucRead requires wrap around
        if ((pxRingbuffer->pucTail - pxRingbuffer->pucFree) < rbHEADER_SIZE) {
            pxRingbuffer->pucFree = pxRingbuffer->pucHead;
        }
        pxCurHeader = (ItemHeader_t *)pxRingbuffer->pucFree;      //Update header to point to item
    }

    //Check if the buffer full flag should be reset
    if (pxRingbuffer->uxRingbufferFlags & rbBUFFER_FULL_FLAG) {
        if (pxRingbuffer->pucFree != pxRingbuffer->pucWrite) {
            pxRingbuffer->uxRingbufferFlags &= ~rbBUFFER_FULL_FLAG;
        } else if (pxRingbuffer->pucFree == pxRingbuffer->pucWrite && pxRingbuffer->pucFree == pxRingbuffer->pucRead) {
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
    if (pxRingbuffer->pucWrite < pxRingbuffer->pucFree) {
        //Free space is contiguous between pucWrite and pucFree
        xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucWrite;
    } else {
        //Free space wraps around (or overlapped at pucHead), select largest
        //contiguous free space as no-split items require contiguous space
        size_t xSize1 = pxRingbuffer->pucTail - pxRingbuffer->pucWrite;
        size_t xSize2 = pxRingbuffer->pucFree - pxRingbuffer->pucHead;
        xFreeSize = (xSize1 > xSize2) ? xSize1 : xSize2;
    }

    //No-split ring buffer items need space for a header
    xFreeSize -= rbHEADER_SIZE;
    //Limit free size to be within bounds
    if (xFreeSize > pxRingbuffer->xMaxItemSize) {
        xFreeSize = pxRingbuffer->xMaxItemSize;
    } else if (xFreeSize < 0) {
        //Occurs when free space is less than header size
        xFreeSize = 0;
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
    if (pxRingbuffer->pucWrite == pxRingbuffer->pucHead && pxRingbuffer->pucFree == pxRingbuffer->pucHead) {
        //Check for special case where pucWrite and pucFree are both at pucHead
        xFreeSize = pxRingbuffer->xSize - rbHEADER_SIZE;
    } else if (pxRingbuffer->pucWrite < pxRingbuffer->pucFree) {
        //Free space is contiguous between pucWrite and pucFree, requires single header
        xFreeSize = (pxRingbuffer->pucFree - pxRingbuffer->pucWrite) - rbHEADER_SIZE;
    } else {
        //Free space wraps around, requires two headers
        xFreeSize = (pxRingbuffer->pucFree - pxRingbuffer->pucHead) +
                    (pxRingbuffer->pucTail - pxRingbuffer->pucWrite) -
                    (rbHEADER_SIZE * 2);
    }

    //Limit free size to be within bounds
    if (xFreeSize > pxRingbuffer->xMaxItemSize) {
        xFreeSize = pxRingbuffer->xMaxItemSize;
    } else if (xFreeSize < 0) {
        xFreeSize = 0;
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
     * pointer and write pointer. There is no overhead of headers in this mode
     */
    xFreeSize = pxRingbuffer->pucFree - pxRingbuffer->pucWrite;
    if (xFreeSize <= 0) {
        xFreeSize += pxRingbuffer->xSize;
    }
    return xFreeSize;
}

static BaseType_t prvReceiveGeneric(Ringbuffer_t *pxRingbuffer, void **pvItem1, void **pvItem2, size_t *xItemSize1, size_t *xItemSize2, size_t xMaxSize, TickType_t xTicksToWait)
{
    BaseType_t xReturn = pdFALSE;
    BaseType_t xReturnSemaphore = pdFALSE;
    TickType_t xTicksEnd = xTaskGetTickCount() + xTicksToWait;
    TickType_t xTicksRemaining = xTicksToWait;
    while (xTicksRemaining <= xTicksToWait) {   //xTicksToWait will underflow once xTaskGetTickCount() > ticks_end
        //Block until more free space becomes available or timeout
        if (xSemaphoreTake(pxRingbuffer->xItemsBufferedSemaphore, xTicksRemaining) != pdTRUE) {
            xReturn = pdFALSE;     //Timed out attempting to get semaphore
            break;
        }

        //Semaphore obtained, check if item can be retrieved
        portENTER_CRITICAL(&pxRingbuffer->mux);
        if (prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
            //Item is available for retrieval
            BaseType_t xIsSplit;
            if (pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
                //Second argument (pxIsSplit) is unused for byte buffers
                *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, NULL, xMaxSize, xItemSize1);
            } else {
                //Third argument (xMaxSize) is unused for no-split/allow-split buffers
                *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize1);
            }
            //Check for item split if configured to do so
            if ((pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) && (pvItem2 != NULL) && (xItemSize2 != NULL)) {
                if (xIsSplit == pdTRUE) {
                    *pvItem2 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize2);
                    configASSERT(*pvItem2 < *pvItem1);  //Check wrap around has occurred
                    configASSERT(xIsSplit == pdFALSE);  //Second part should not have wrapped flag
                } else {
                    *pvItem2 = NULL;
                }
            }
            xReturn = pdTRUE;
            if (pxRingbuffer->xItemsWaiting > 0) {
                xReturnSemaphore = pdTRUE;
            }
            portEXIT_CRITICAL(&pxRingbuffer->mux);
            break;
        }
        //No item available for retrieval, adjust ticks and take the semaphore again
        if (xTicksToWait != portMAX_DELAY) {
            xTicksRemaining = xTicksEnd - xTaskGetTickCount();
        }
        portEXIT_CRITICAL(&pxRingbuffer->mux);
        /*
         * Gap between critical section and re-acquiring of the semaphore. If
         * semaphore is given now, priority inversion might occur (see docs)
         */
    }

    if (xReturnSemaphore == pdTRUE) {
        xSemaphoreGive(pxRingbuffer->xItemsBufferedSemaphore);  //Give semaphore back so other tasks can retrieve
    }
    return xReturn;
}

static BaseType_t prvReceiveGenericFromISR(Ringbuffer_t *pxRingbuffer, void **pvItem1, void **pvItem2, size_t *xItemSize1, size_t *xItemSize2, size_t xMaxSize)
{
    BaseType_t xReturn = pdFALSE;
    BaseType_t xReturnSemaphore = pdFALSE;

    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    if(prvCheckItemAvail(pxRingbuffer) == pdTRUE) {
        BaseType_t xIsSplit;
        if (pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) {
            //Second argument (pxIsSplit) is unused for byte buffers
            *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, NULL, xMaxSize, xItemSize1);
        } else {
            //Third argument (xMaxSize) is unused for no-split/allow-split buffers
            *pvItem1 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize1);
        }
        //Check for item split if configured to do so
        if ((pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG) && pvItem2 != NULL && xItemSize2 != NULL) {
            if (xIsSplit == pdTRUE) {
                *pvItem2 = pxRingbuffer->pvGetItem(pxRingbuffer, &xIsSplit, 0, xItemSize2);
                configASSERT(*pvItem2 < *pvItem1);  //Check wrap around has occurred
                configASSERT(xIsSplit == pdFALSE);  //Second part should not have wrapped flag
            } else {
                *pvItem2 = NULL;
            }
        }
        xReturn = pdTRUE;
        if (pxRingbuffer->xItemsWaiting > 0) {
            xReturnSemaphore = pdTRUE;
        }
    }
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);

    if (xReturnSemaphore == pdTRUE) {
        xSemaphoreGiveFromISR(pxRingbuffer->xItemsBufferedSemaphore, NULL);  //Give semaphore back so other tasks can retrieve
    }
    return xReturn;
}

/* ------------------------------------------------- Public Definitions -------------------------------------------- */

RingbufHandle_t xRingbufferCreate(size_t xBufferSize, ringbuf_type_t xBufferType)
{
    //Allocate memory
    Ringbuffer_t *pxRingbuffer = calloc(1, sizeof(Ringbuffer_t));
    if (pxRingbuffer == NULL) {
        goto err;
    }
    if (xBufferType != RINGBUF_TYPE_BYTEBUF) {
        xBufferSize = rbALIGN_SIZE(xBufferSize);    //xBufferSize is rounded up for no-split/allow-split buffers
    }
    pxRingbuffer->pucHead = malloc(xBufferSize);
    if (pxRingbuffer->pucHead == NULL) {
        goto err;
    }

    //Initialize values
    pxRingbuffer->xSize = xBufferSize;
    pxRingbuffer->pucTail = pxRingbuffer->pucHead + xBufferSize;
    pxRingbuffer->pucFree = pxRingbuffer->pucHead;
    pxRingbuffer->pucRead = pxRingbuffer->pucHead;
    pxRingbuffer->pucWrite = pxRingbuffer->pucHead;
    pxRingbuffer->xItemsWaiting = 0;
    pxRingbuffer->xFreeSpaceSemaphore = xSemaphoreCreateBinary();
    pxRingbuffer->xItemsBufferedSemaphore = xSemaphoreCreateBinary();
    pxRingbuffer->uxRingbufferFlags = 0;

    //Initialize type dependent values and function pointers
    if (xBufferType == RINGBUF_TYPE_NOSPLIT) {
        pxRingbuffer->xCheckItemFits = prvCheckItemFitsDefault;
        pxRingbuffer->vCopyItem = prvCopyItemNoSplit;
        pxRingbuffer->pvGetItem = prvGetItemDefault;
        pxRingbuffer->vReturnItem = prvReturnItemDefault;
        /*
         * Buffer lengths are always aligned. No-split buffer (read/write/free)
         * pointers are also always aligned. Therefore worse case scenario is
         * the write pointer is at the most aligned halfway point.
         */
        pxRingbuffer->xMaxItemSize = rbALIGN_SIZE(pxRingbuffer->xSize / 2) - rbHEADER_SIZE;
        pxRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeNoSplit;
    } else if (xBufferType == RINGBUF_TYPE_ALLOWSPLIT) {
        pxRingbuffer->uxRingbufferFlags |= rbALLOW_SPLIT_FLAG;
        pxRingbuffer->xCheckItemFits = prvCheckItemFitsDefault;
        pxRingbuffer->vCopyItem = prvCopyItemAllowSplit;
        pxRingbuffer->pvGetItem = prvGetItemDefault;
        pxRingbuffer->vReturnItem = prvReturnItemDefault;
        //Worst case an item is split into two, incurring two headers of overhead
        pxRingbuffer->xMaxItemSize = pxRingbuffer->xSize - (sizeof(ItemHeader_t) * 2);
        pxRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeAllowSplit;
    } else if (xBufferType == RINGBUF_TYPE_BYTEBUF) {
        pxRingbuffer->uxRingbufferFlags |= rbBYTE_BUFFER_FLAG;
        pxRingbuffer->xCheckItemFits = prvCheckItemFitsByteBuffer;
        pxRingbuffer->vCopyItem = prvCopyItemByteBuf;
        pxRingbuffer->pvGetItem = prvGetItemByteBuf;
        pxRingbuffer->vReturnItem = prvReturnItemByteBuf;
        //Byte buffers do not incur any overhead
        pxRingbuffer->xMaxItemSize = pxRingbuffer->xSize;
        pxRingbuffer->xGetCurMaxSize = prvGetCurMaxSizeByteBuf;
    } else {
        //Unsupported type
        configASSERT(0);
    }

    if (pxRingbuffer->xFreeSpaceSemaphore == NULL || pxRingbuffer->xItemsBufferedSemaphore == NULL) {
        goto err;
    }
    xSemaphoreGive(pxRingbuffer->xFreeSpaceSemaphore);
    vPortCPUInitializeMutex(&pxRingbuffer->mux);

    return (RingbufHandle_t)pxRingbuffer;

err:
    //Some error has happened. Free/destroy all allocated things and return NULL.
    if (pxRingbuffer) {
        free(pxRingbuffer->pucHead);
        if (pxRingbuffer->xFreeSpaceSemaphore) {
            vSemaphoreDelete(pxRingbuffer->xFreeSpaceSemaphore);
        }
        if (pxRingbuffer->xItemsBufferedSemaphore) {
            vSemaphoreDelete(pxRingbuffer->xItemsBufferedSemaphore);
        }
    }
    free(pxRingbuffer);
    return NULL;
}

RingbufHandle_t xRingbufferCreateNoSplit(size_t xItemSize, size_t xItemNum)
{
    return xRingbufferCreate((rbALIGN_SIZE(xItemSize) + rbHEADER_SIZE) * xItemNum, RINGBUF_TYPE_NOSPLIT);
}

BaseType_t xRingbufferSend(RingbufHandle_t xRingbuffer, const void *pvItem, size_t xItemSize, TickType_t xTicksToWait)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL || xItemSize == 0);
    if (xItemSize > pxRingbuffer->xMaxItemSize) {
        return pdFALSE;     //Data will never ever fit in the queue.
    }
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && xItemSize == 0) {
        return pdTRUE;      //Sending 0 bytes to byte buffer has no effect
    }

    //Attempt to send an item
    BaseType_t xReturn = pdFALSE;
    BaseType_t xReturnSemaphore = pdFALSE;
    TickType_t xTicksEnd = xTaskGetTickCount() + xTicksToWait;
    TickType_t xTicksRemaining = xTicksToWait;
    while (xTicksRemaining <= xTicksToWait) {   //xTicksToWait will underflow once xTaskGetTickCount() > ticks_end
        //Block until more free space becomes available or timeout
        if (xSemaphoreTake(pxRingbuffer->xFreeSpaceSemaphore, xTicksRemaining) != pdTRUE) {
            xReturn = pdFALSE;
            break;
        }
        //Semaphore obtained, check if item can fit
        portENTER_CRITICAL(&pxRingbuffer->mux);
        if(pxRingbuffer->xCheckItemFits(pxRingbuffer, xItemSize) == pdTRUE) {
            //Item will fit, copy item
            pxRingbuffer->vCopyItem(pxRingbuffer, pvItem, xItemSize);
            xReturn = pdTRUE;
            //Check if the free semaphore should be returned to allow other tasks to send
            if (prvGetFreeSize(pxRingbuffer) > 0) {
                xReturnSemaphore = pdTRUE;
            }
            portEXIT_CRITICAL(&pxRingbuffer->mux);
            break;
        }
        //Item doesn't fit, adjust ticks and take the semaphore again
        if (xTicksToWait != portMAX_DELAY) {
            xTicksRemaining = xTicksEnd - xTaskGetTickCount();
        }
        portEXIT_CRITICAL(&pxRingbuffer->mux);
        /*
         * Gap between critical section and re-acquiring of the semaphore. If
         * semaphore is given now, priority inversion might occur (see docs)
         */
    }

    if (xReturn == pdTRUE) {
        //Indicate item was successfully sent
        xSemaphoreGive(pxRingbuffer->xItemsBufferedSemaphore);
    }
    if (xReturnSemaphore == pdTRUE) {
        xSemaphoreGive(pxRingbuffer->xFreeSpaceSemaphore);  //Give back semaphore so other tasks can send
    }
    return xReturn;
}

BaseType_t xRingbufferSendFromISR(RingbufHandle_t xRingbuffer, const void *pvItem, size_t xItemSize, BaseType_t *pxHigherPriorityTaskWoken)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL || xItemSize == 0);
    if (xItemSize > pxRingbuffer->xMaxItemSize) {
        return pdFALSE;     //Data will never ever fit in the queue.
    }
    if ((pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG) && xItemSize == 0) {
        return pdTRUE;      //Sending 0 bytes to byte buffer has no effect
    }

    //Attempt to send an item
    BaseType_t xReturn;
    BaseType_t xReturnSemaphore = pdFALSE;
    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    if (pxRingbuffer->xCheckItemFits(xRingbuffer, xItemSize) == pdTRUE) {
        pxRingbuffer->vCopyItem(xRingbuffer, pvItem, xItemSize);
        xReturn = pdTRUE;
        //Check if the free semaphore should be returned to allow other tasks to send
        if (prvGetFreeSize(pxRingbuffer) > 0) {
            xReturnSemaphore = pdTRUE;
        }
    } else {
        xReturn = pdFALSE;
    }
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);

    if (xReturn == pdTRUE) {
        //Indicate item was successfully sent
        xSemaphoreGiveFromISR(pxRingbuffer->xItemsBufferedSemaphore, pxHigherPriorityTaskWoken);
    }
    if (xReturnSemaphore == pdTRUE) {
        xSemaphoreGiveFromISR(pxRingbuffer->xFreeSpaceSemaphore, pxHigherPriorityTaskWoken);  //Give back semaphore so other tasks can send
    }
    return xReturn;
}

void *xRingbufferReceive(RingbufHandle_t xRingbuffer, size_t *pxItemSize, TickType_t xTicksToWait)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    //Attempt to retrieve an item
    void *pvTempItem;
    size_t xTempSize;
    if (prvReceiveGeneric(pxRingbuffer, &pvTempItem, NULL, &xTempSize, NULL, 0, xTicksToWait) == pdTRUE) {
        if (pxItemSize != NULL) {
            *pxItemSize = xTempSize;
        }
        return pvTempItem;
    } else {
        return NULL;
    }
}

void *xRingbufferReceiveFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    //Attempt to retrieve an item
    void *pvTempItem;
    size_t xTempSize;
    if (prvReceiveGenericFromISR(pxRingbuffer, &pvTempItem, NULL, &xTempSize, NULL, 0) == pdTRUE) {
        if (pxItemSize != NULL) {
            *pxItemSize = xTempSize;
        }
        return pvTempItem;
    } else {
        return NULL;
    }
}

BaseType_t xRingbufferReceiveSplit(RingbufHandle_t xRingbuffer, void **ppvHeadItem, void **ppvTailItem, size_t *pxHeadItemSize, size_t *pxTailItemSize, TickType_t xTicksToWait)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG);
    configASSERT(ppvHeadItem != NULL && ppvTailItem != NULL);

    //Attempt to retrieve multiple items
    void *pvTempHeadItem, *pvTempTailItem;
    size_t xTempHeadSize, xTempTailSize;
    if (prvReceiveGeneric(pxRingbuffer, &pvTempHeadItem, &pvTempTailItem, &xTempHeadSize, &xTempTailSize, 0, xTicksToWait) == pdTRUE) {
        //At least one item was retrieved
        *ppvHeadItem = pvTempHeadItem;
        if(pxHeadItemSize != NULL){
            *pxHeadItemSize = xTempHeadSize;
        }
        //Check to see if a second item was also retrieved
        if (pvTempTailItem != NULL) {
            *ppvTailItem = pvTempTailItem;
            if (pxTailItemSize != NULL) {
                *pxTailItemSize = xTempTailSize;
            }
        } else {
            *ppvTailItem = NULL;
        }
        return pdTRUE;
    } else {
        //No items retrieved
        *ppvHeadItem = NULL;
        *ppvTailItem = NULL;
        return pdFALSE;
    }
}

BaseType_t xRingbufferReceiveSplitFromISR(RingbufHandle_t xRingbuffer, void **ppvHeadItem, void **ppvTailItem, size_t *pxHeadItemSize, size_t *pxTailItemSize)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbALLOW_SPLIT_FLAG);
    configASSERT(ppvHeadItem != NULL && ppvTailItem != NULL);

    //Attempt to retrieve multiple items
    void *pvTempHeadItem, *pvTempTailItem;
    size_t xTempHeadSize, xTempTailSize;
    if (prvReceiveGenericFromISR(pxRingbuffer, &pvTempHeadItem, &pvTempTailItem, &xTempHeadSize, &xTempTailSize, 0) == pdTRUE) {
        //At least one item was received
        *ppvHeadItem = pvTempHeadItem;
        if (pxHeadItemSize != NULL) {
            *pxHeadItemSize = xTempHeadSize;
        }
        //Check to see if a second item was also retrieved
        if (pvTempTailItem != NULL) {
            *ppvTailItem = pvTempTailItem;
            if (pxTailItemSize != NULL) {
                *pxTailItemSize = xTempTailSize;
            }
        } else {
            *ppvTailItem = NULL;
        }
        return pdTRUE;
    } else {
        *ppvHeadItem = NULL;
        *ppvTailItem = NULL;
        return pdFALSE;
    }
}

void *xRingbufferReceiveUpTo(RingbufHandle_t xRingbuffer, size_t *pxItemSize, TickType_t xTicksToWait, size_t xMaxSize)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG);    //This function should only be called for byte buffers
    if (xMaxSize == 0) {
        return NULL;
    }

    //Attempt to retrieve up to xMaxSize bytes
    void *pvTempItem;
    size_t xTempSize;
    if (prvReceiveGeneric(pxRingbuffer, &pvTempItem, NULL, &xTempSize, NULL, xMaxSize, xTicksToWait) == pdTRUE) {
        if (pxItemSize != NULL) {
            *pxItemSize = xTempSize;
        }
        return pvTempItem;
    } else {
        return NULL;
    }
}

void *xRingbufferReceiveUpToFromISR(RingbufHandle_t xRingbuffer, size_t *pxItemSize, size_t xMaxSize)
{
    //Check arguments
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pxRingbuffer->uxRingbufferFlags & rbBYTE_BUFFER_FLAG);    //This function should only be called for byte buffers
    if (xMaxSize == 0) {
        return NULL;
    }

    //Attempt to retrieve up to xMaxSize bytes
    void *pvTempItem;
    size_t xTempSize;
    if (prvReceiveGenericFromISR(pxRingbuffer, &pvTempItem, NULL, &xTempSize, NULL, xMaxSize) == pdTRUE) {
        if (pxItemSize != NULL) {
            *pxItemSize = xTempSize;
        }
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
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    xSemaphoreGive(pxRingbuffer->xFreeSpaceSemaphore);
}

void vRingbufferReturnItemFromISR(RingbufHandle_t xRingbuffer, void *pvItem, BaseType_t *pxHigherPriorityTaskWoken)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    configASSERT(pvItem != NULL);

    portENTER_CRITICAL_ISR(&pxRingbuffer->mux);
    pxRingbuffer->vReturnItem(pxRingbuffer, (uint8_t *)pvItem);
    portEXIT_CRITICAL_ISR(&pxRingbuffer->mux);
    xSemaphoreGiveFromISR(pxRingbuffer->xFreeSpaceSemaphore, pxHigherPriorityTaskWoken);
}

void vRingbufferDelete(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    if (pxRingbuffer) {
        free(pxRingbuffer->pucHead);
        if (pxRingbuffer->xFreeSpaceSemaphore) {
            vSemaphoreDelete(pxRingbuffer->xFreeSpaceSemaphore);
        }
        if (pxRingbuffer->xItemsBufferedSemaphore) {
            vSemaphoreDelete(pxRingbuffer->xItemsBufferedSemaphore);
        }
    }
    free(pxRingbuffer);
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
    configASSERT(pxRingbuffer);

    BaseType_t xReturn;
    portENTER_CRITICAL(&pxRingbuffer->mux);
    //Cannot add semaphore to queue set if semaphore is not empty. Temporarily hold semaphore
    BaseType_t xHoldSemaphore = xSemaphoreTake(pxRingbuffer->xItemsBufferedSemaphore, 0);
    xReturn = xQueueAddToSet(pxRingbuffer->xItemsBufferedSemaphore, xQueueSet);
    if (xHoldSemaphore == pdTRUE) {
        //Return semaphore if temporarily held
        configASSERT(xSemaphoreGive(pxRingbuffer->xItemsBufferedSemaphore) == pdTRUE);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return xReturn;
}

BaseType_t xRingbufferCanRead(RingbufHandle_t xRingbuffer, QueueSetMemberHandle_t xMember)
{
    //Check if the selected queue set member is the ring buffer's read semaphore
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    return (pxRingbuffer->xItemsBufferedSemaphore == xMember) ? pdTRUE : pdFALSE;
}

BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    BaseType_t xReturn;
    portENTER_CRITICAL(&pxRingbuffer->mux);
    //Cannot remove semaphore from queue set if semaphore is not empty. Temporarily hold semaphore
    BaseType_t xHoldSemaphore = xSemaphoreTake(pxRingbuffer->xItemsBufferedSemaphore, 0);
    xReturn = xQueueRemoveFromSet(pxRingbuffer->xItemsBufferedSemaphore, xQueueSet);
    if (xHoldSemaphore == pdTRUE) {
        //Return semaphore if temporarily held
        configASSERT(xSemaphoreGive(pxRingbuffer->xItemsBufferedSemaphore) == pdTRUE);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return xReturn;
}

void vRingbufferGetInfo(RingbufHandle_t xRingbuffer, UBaseType_t *uxFree, UBaseType_t *uxRead, UBaseType_t *uxWrite, UBaseType_t *uxItemsWaiting)
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
    if (uxItemsWaiting != NULL) {
        *uxItemsWaiting = (UBaseType_t)(pxRingbuffer->xItemsWaiting);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
}

void xRingbufferPrintInfo(RingbufHandle_t xRingbuffer)
{
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    printf("Rb size:%d\tfree: %d\trptr: %d\tfreeptr: %d\twptr: %d\n",
           pxRingbuffer->xSize, prvGetFreeSize(pxRingbuffer),
           pxRingbuffer->pucRead - pxRingbuffer->pucHead,
           pxRingbuffer->pucFree - pxRingbuffer->pucHead,
           pxRingbuffer->pucWrite - pxRingbuffer->pucHead);
}

/* --------------------------------- Deprecated Functions ------------------------------ */
//Todo: Remove the following deprecated functions in next release

bool xRingbufferIsNextItemWrapped(RingbufHandle_t xRingbuffer)
{
    //This function is deprecated, use xRingbufferReceiveSplit() instead
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);
    bool is_wrapped;

    portENTER_CRITICAL(&pxRingbuffer->mux);
    ItemHeader_t *xHeader = (ItemHeader_t *)pxRingbuffer->pucRead;
    is_wrapped = xHeader->uxItemFlags & rbITEM_SPLIT_FLAG;
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return is_wrapped;
}

BaseType_t xRingbufferAddToQueueSetWrite(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet)
{
    //This function is deprecated. QueueSetWrite no longer supported
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    BaseType_t xReturn;
    portENTER_CRITICAL(&pxRingbuffer->mux);
    //Cannot add semaphore to queue set if semaphore is not empty. Temporary hold semaphore
    BaseType_t xHoldSemaphore = xSemaphoreTake(pxRingbuffer->xFreeSpaceSemaphore, 0);
    xReturn = xQueueAddToSet(pxRingbuffer->xFreeSpaceSemaphore, xQueueSet);
    if (xHoldSemaphore == pdTRUE) {
        //Return semaphore is temporarily held
        configASSERT(xSemaphoreGive(pxRingbuffer->xFreeSpaceSemaphore) == pdTRUE);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return xReturn;
}

BaseType_t xRingbufferRemoveFromQueueSetWrite(RingbufHandle_t xRingbuffer, QueueSetHandle_t xQueueSet)
{
    //This function is deprecated. QueueSetWrite no longer supported
    Ringbuffer_t *pxRingbuffer = (Ringbuffer_t *)xRingbuffer;
    configASSERT(pxRingbuffer);

    BaseType_t xReturn;
    portENTER_CRITICAL(&pxRingbuffer->mux);
    //Cannot remove semaphore from queue set if semaphore is not empty. Temporary hold semaphore
    BaseType_t xHoldSemaphore = xSemaphoreTake(pxRingbuffer->xFreeSpaceSemaphore, 0);
    xReturn = xQueueRemoveFromSet(pxRingbuffer->xFreeSpaceSemaphore, xQueueSet);
    if (xHoldSemaphore == pdTRUE) {
        //Return semaphore is temporarily held
        configASSERT(xSemaphoreGive(pxRingbuffer->xFreeSpaceSemaphore) == pdTRUE);
    }
    portEXIT_CRITICAL(&pxRingbuffer->mux);
    return xReturn;
}

