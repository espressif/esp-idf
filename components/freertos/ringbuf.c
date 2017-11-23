// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "freertos/ringbuf.h"
#include "esp_attr.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    flag_allowsplit = 1,
    flag_bytebuf = 2,
} rbflag_t;

typedef enum {
    iflag_free = 1,             //Buffer is not read and given back by application, free to overwrite
    iflag_dummydata = 2,        //Data from here to end of ringbuffer is dummy. Restart reading at start of ringbuffer.
    iflag_wrap = 4,             //Valid for RINGBUF_TYPE_ALLOWSPLIT, indicating that rest of the data is wrapped around
} itemflag_t;


typedef struct ringbuf_t ringbuf_t;

//The ringbuffer structure
struct  ringbuf_t {
    SemaphoreHandle_t free_space_sem;           //Binary semaphore, wakes up writing threads when there's more free space
    SemaphoreHandle_t items_buffered_sem;       //Binary semaphore, indicates there are new packets in the circular buffer. See remark.
    size_t size;                                //Size of the data storage
    uint8_t *write_ptr;                         //Pointer where the next item is written
    uint8_t *read_ptr;                          //Pointer from where the next item is read
    uint8_t *free_ptr;                          //Pointer to the last block that hasn't been given back to the ringbuffer yet
    uint8_t *data;                              //Data storage
    portMUX_TYPE mux;                           //Spinlock for actual data/ptr/struct modification
    rbflag_t flags;
    size_t maxItemSize;
   //The following keep function pointers to hold different implementations for ringbuffer management.
    BaseType_t (*copyItemToRingbufImpl)(ringbuf_t *rb, uint8_t *buffer, size_t buffer_size);
    uint8_t *(*getItemFromRingbufImpl)(ringbuf_t *rb, size_t *length, int wanted_length);
    void (*returnItemToRingbufImpl)(ringbuf_t *rb, void *item);
    size_t (*getFreeSizeImpl)(ringbuf_t *rb);
};



/*
Remark: A counting semaphore for items_buffered_sem would be more logical, but counting semaphores in
FreeRTOS need a maximum count, and allocate more memory the larger the maximum count is. Here, we
would need to set the maximum to the maximum amount of times a null-byte unit firs in the buffer,
which is quite high and so would waste a fair amount of memory.
*/


//The header prepended to each ringbuffer entry. Size is assumed to be a multiple of 32bits.
typedef struct {
    size_t len;
    itemflag_t flags;
} buf_entry_hdr_t;


//Calculate space free in the buffer
static int ringbufferFreeMem(ringbuf_t *rb) 
{
    int free_size = rb->free_ptr-rb->write_ptr;
    if (free_size <= 0) free_size += rb->size;
    //Reserve one byte. If we do not do this and the entire buffer is filled, we get a situation 
    //where read_ptr == free_ptr, messing up the next calculation.
    return free_size-1;
}

//Copies a single item to the ring buffer; refuses to split items. Assumes there is space in the ringbuffer and
//the ringbuffer is locked. Increases write_ptr to the next item. Returns pdTRUE on
//success, pdFALSE if it can't make the item fit and the calling routine needs to retry
//later or fail.
//This function by itself is not threadsafe, always call from within a muxed section.
static BaseType_t copyItemToRingbufNoSplit(ringbuf_t *rb, uint8_t *buffer, size_t buffer_size) 
{
    size_t rbuffer_size;
    rbuffer_size=(buffer_size+3)&~3; //Payload length, rounded to next 32-bit value
    configASSERT(((int)rb->write_ptr&3)==0); //write_ptr needs to be 32-bit aligned
    configASSERT(rb->write_ptr-(rb->data+rb->size) >= sizeof(buf_entry_hdr_t)); //need to have at least the size 
                                            //of a header to the end of the ringbuff
    size_t rem_len=(rb->data + rb->size) - rb->write_ptr; //length remaining until end of ringbuffer
    
    //See if we have enough contiguous space to write the buffer.
    if (rem_len < rbuffer_size + sizeof(buf_entry_hdr_t)) {
        //Buffer plus header is not going to fit in the room from wr_pos to the end of the 
        //ringbuffer... but we're not allowed to split the buffer. We need to fill the 
        //rest of the ringbuffer with a dummy item so we can place the data at the _start_ of
        //the ringbuffer..
        //First, find out if we actually have enough space at the start of the ringbuffer to
        //make this work (Again, we need 4 bytes extra because otherwise read_ptr==free_ptr)
        if (rb->free_ptr-rb->data < rbuffer_size+sizeof(buf_entry_hdr_t)+4) {
            //Will not fit.
            return pdFALSE;
        }
        //If the read buffer hasn't wrapped around yet, there's no way this will work either.
        if (rb->free_ptr > rb->write_ptr) {
            //No luck.
            return pdFALSE;
        }

        //Okay, it will fit. Mark the rest of the ringbuffer space with a dummy packet.
        buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->write_ptr;
        hdr->flags=iflag_dummydata;
        //Reset the write pointer to the start of the ringbuffer so the code later on can
        //happily write the data.
        rb->write_ptr=rb->data;
    } else {
        //No special handling needed. Checking if it's gonna fit probably still is a good idea.
        if (ringbufferFreeMem(rb) < sizeof(buf_entry_hdr_t)+rbuffer_size) {
            //Buffer is not going to fit, period.
            return pdFALSE;
        }
    }

    //If we are here, the buffer is guaranteed to fit in the space starting at the write pointer.
    buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->write_ptr;
    hdr->len=buffer_size;
    hdr->flags=0;
    rb->write_ptr+=sizeof(buf_entry_hdr_t);
    memcpy(rb->write_ptr, buffer, buffer_size);
    rb->write_ptr+=rbuffer_size;

    //The buffer will wrap around if we don't have room for a header anymore.
    if ((rb->data+rb->size)-rb->write_ptr < sizeof(buf_entry_hdr_t)) {
        //'Forward' the write buffer until we are at the start of the ringbuffer.
        //The read pointer will always be at the start of a full header, which cannot 
        //exist at the point of the current write pointer, so there's no chance of overtaking
        //that.
        rb->write_ptr=rb->data;
    }
    return pdTRUE;
}

//Copies a single item to the ring buffer; allows split items. Assumes there is space in the ringbuffer and
//the ringbuffer is locked. Increases write_ptr to the next item. Returns pdTRUE on
//success, pdFALSE if it can't make the item fit and the calling routine needs to retry
//later or fail.
//This function by itself is not threadsafe, always call from within a muxed section.
static BaseType_t copyItemToRingbufAllowSplit(ringbuf_t *rb, uint8_t *buffer, size_t buffer_size) 
{
    size_t rbuffer_size;
    rbuffer_size=(buffer_size+3)&~3; //Payload length, rounded to next 32-bit value
    configASSERT(((int)rb->write_ptr&3)==0); //write_ptr needs to be 32-bit aligned
    configASSERT(rb->write_ptr-(rb->data+rb->size) >= sizeof(buf_entry_hdr_t)); //need to have at least the size 
                                            //of a header to the end of the ringbuff
    size_t rem_len=(rb->data + rb->size) - rb->write_ptr; //length remaining until end of ringbuffer
    
    //See if we have enough contiguous space to write the buffer.
    if (rem_len < rbuffer_size + sizeof(buf_entry_hdr_t)) {
        //The buffer can't be contiguously written to the ringbuffer, but needs special handling. Do
        //that depending on how the ringbuffer is configured.
        //The code here is also expected to check if the buffer, mangled in whatever way is implemented,
        //will still fit, and return pdFALSE if that is not the case.
        //Buffer plus header is not going to fit in the room from wr_pos to the end of the 
        //ringbuffer... we need to split the write in two.
        //First, see if this will fit at all.
        if (ringbufferFreeMem(rb) < (sizeof(buf_entry_hdr_t)*2)+rbuffer_size) {
            //Will not fit.
            return pdFALSE;
        }
         //Because the code at the end of the function makes sure we always have 
        //room for a header, this should never assert.
        configASSERT(rem_len>=sizeof(buf_entry_hdr_t));
         //Okay, it should fit. Write everything.
        //First, place bit of buffer that does fit. Write header first...
        buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->write_ptr;
        hdr->flags=0;
        hdr->len=rem_len-sizeof(buf_entry_hdr_t);
        rb->write_ptr+=sizeof(buf_entry_hdr_t);
        rem_len-=sizeof(buf_entry_hdr_t);
        if (rem_len!=0) {
            //..then write the data bit that fits.
            memcpy(rb->write_ptr, buffer, rem_len);
            //Update vars so the code later on will write the rest of the data.
            buffer+=rem_len;
            buffer_size-=rem_len;
            //Re-adjust the rbuffer value to be 4 byte aligned
            rbuffer_size=(buffer_size+3)&~3;
            //It is possible that we are here because we checked for 4byte aligned
            //size, but actual data was smaller.
            //Eg. For buffer_size = 34, rbuffer_size will be 36. Suppose we had only
            //42 bytes of memory available, the top level check will fail, as it will
            //check for availability of 36 + 8 = 44 bytes.
            //However, the 42 bytes available memory is sufficient for 34 + 8 bytes data
            //and so, we can return after writing the data. Hence, this check
            if (buffer_size == 0) {
                rb->write_ptr=rb->data;
                return pdTRUE;
            } else {
                /* Indicate the wrapping */
                hdr->flags|=iflag_wrap;
            }
        } else {
            //Huh, only the header fit. Mark as dummy so the receive function doesn't receive
            //an useless zero-byte packet.
            hdr->flags|=iflag_dummydata;
        }
        rb->write_ptr=rb->data;
    } else {
        //No special handling needed. Checking if it's gonna fit probably still is a good idea.
        if (ringbufferFreeMem(rb) < sizeof(buf_entry_hdr_t)+rbuffer_size) {
            //Buffer is not going to fit, period.
            return pdFALSE;
        }
    }

    //If we are here, the buffer is guaranteed to fit in the space starting at the write pointer.
    buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->write_ptr;
    hdr->len=buffer_size;
    hdr->flags=0;
    rb->write_ptr+=sizeof(buf_entry_hdr_t);
    memcpy(rb->write_ptr, buffer, buffer_size);
    rb->write_ptr+=rbuffer_size;

    //The buffer will wrap around if we don't have room for a header anymore.
    if ((rb->data+rb->size)-rb->write_ptr < sizeof(buf_entry_hdr_t)) {
        //'Forward' the write buffer until we are at the start of the ringbuffer.
        //The read pointer will always be at the start of a full header, which cannot 
        //exist at the point of the current write pointer, so there's no chance of overtaking
        //that.
        rb->write_ptr=rb->data;
    }
    return pdTRUE;
}


//Copies a bunch of daya to the ring bytebuffer. Assumes there is space in the ringbuffer and
//the ringbuffer is locked. Increases write_ptr to the next item. Returns pdTRUE on
//success, pdFALSE if it can't make the item fit and the calling routine needs to retry
//later or fail.
//This function by itself is not threadsafe, always call from within a muxed section.
static BaseType_t copyItemToRingbufByteBuf(ringbuf_t *rb, uint8_t *buffer, size_t buffer_size) 
{
    size_t rem_len=(rb->data + rb->size) - rb->write_ptr; //length remaining until end of ringbuffer
    
    //See if we have enough contiguous space to write the buffer.
    if (rem_len < buffer_size) {
        //...Nope. Write the data bit that fits.
        memcpy(rb->write_ptr, buffer, rem_len);
        //Update vars so the code later on will write the rest of the data.
        buffer+=rem_len;
        buffer_size-=rem_len;
        rb->write_ptr=rb->data;
    }

    //If we are here, the buffer is guaranteed to fit in the space starting at the write pointer.
    memcpy(rb->write_ptr, buffer, buffer_size);
    rb->write_ptr+=buffer_size;
    //The buffer will wrap around if we're at the end.
    if ((rb->data+rb->size)==rb->write_ptr) {
        rb->write_ptr=rb->data;
    }
    return pdTRUE;
}

//Retrieves a pointer to the data of the next item, or NULL if this is not possible.
//This function by itself is not threadsafe, always call from within a muxed section.
//Because we always return one item, this function ignores the wanted_length variable.
static uint8_t *getItemFromRingbufDefault(ringbuf_t *rb, size_t *length, int wanted_length)
{
    uint8_t *ret;
    configASSERT(((int)rb->read_ptr&3)==0);
    if (rb->read_ptr == rb->write_ptr) {
        //No data available.
        return NULL;
    }
    //The item written at the point of the read pointer may be a dummy item.
    //We need to skip past it first, if that's the case.
    buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->read_ptr;
    configASSERT((hdr->len < rb->size) || (hdr->flags & iflag_dummydata));
    if (hdr->flags & iflag_dummydata) {
        //Hdr is dummy data. Reset to start of ringbuffer.
        rb->read_ptr=rb->data;
        //Get real header
        hdr=(buf_entry_hdr_t *)rb->read_ptr;
        configASSERT(hdr->len < rb->size);
        //No need to re-check if the ringbuffer is empty: the write routine will
        //always write a dummy item plus the real data item in one go, so now we must
        //be at the real data item by definition.
    }
    //Okay, pass the data back.
    ret=rb->read_ptr+sizeof(buf_entry_hdr_t);
    *length=hdr->len;
    //...and move the read pointer past the data.
    rb->read_ptr+=sizeof(buf_entry_hdr_t)+((hdr->len+3)&~3);
    //The buffer will wrap around if we don't have room for a header anymore.
    //Integer typecasting is used because the first operand can result into a -ve
    //value for cases wherein the ringbuffer size is not a multiple of 4, but the
    //implementation logic aligns read_ptr to 4-byte boundary
    if ((int)((rb->data + rb->size) - rb->read_ptr) < (int)sizeof(buf_entry_hdr_t)) {
        rb->read_ptr=rb->data;
    }
    return ret;
}

//Retrieves a pointer to the data in the buffer, or NULL if this is not possible.
//This function by itself is not threadsafe, always call from within a muxed section.
//This function honours the wanted_length and will never return more data than this.
static uint8_t *getItemFromRingbufByteBuf(ringbuf_t *rb, size_t *length, int wanted_length)
{
    uint8_t *ret;
    if (rb->read_ptr != rb->free_ptr) {
        //This type of ringbuff does not support multiple outstanding buffers.
        return NULL;
    }
    if (rb->read_ptr == rb->write_ptr) {
        //No data available.
        return NULL;
    }
    ret=rb->read_ptr;
    if (rb->read_ptr > rb->write_ptr) {
        //Available data wraps around. Give data until the end of the buffer.
        *length=rb->size-(rb->read_ptr - rb->data);
        if (wanted_length != 0 && *length > wanted_length) {
            *length=wanted_length;
            rb->read_ptr+=wanted_length;
        } else {
            rb->read_ptr=rb->data;
        }
    } else {
        //Return data up to write pointer.
        *length=rb->write_ptr -rb->read_ptr;
        if (wanted_length != 0 && *length > wanted_length) {
            *length=wanted_length;
            rb->read_ptr+=wanted_length;
        } else {
            rb->read_ptr=rb->write_ptr;
        }
    }
    return ret;
}


//Returns an item to the ringbuffer. Will mark the item as free, and will see if the free pointer
//can be increase.
//This function by itself is not threadsafe, always call from within a muxed section.
static void returnItemToRingbufDefault(ringbuf_t *rb, void *item) {
    uint8_t *data=(uint8_t*)item;
    configASSERT(((int)rb->free_ptr&3)==0);
    configASSERT(data >= rb->data);
    configASSERT(data <= rb->data+rb->size);
    //Grab the buffer entry that preceeds the buffer
    buf_entry_hdr_t *hdr=(buf_entry_hdr_t*)(data-sizeof(buf_entry_hdr_t));
    configASSERT(hdr->len < rb->size);
    configASSERT((hdr->flags & iflag_dummydata)==0);
    configASSERT((hdr->flags & iflag_free)==0);
    //Mark the buffer as free.
    hdr->flags&=~iflag_wrap;
    hdr->flags|=iflag_free;

    //Do a cleanup pass.
    hdr=(buf_entry_hdr_t *)rb->free_ptr;
    //basically forward free_ptr until we run into either a block that is still in use or the write pointer.
    while (((hdr->flags & iflag_free) || (hdr->flags & iflag_dummydata)) && rb->free_ptr != rb->write_ptr) {
        if (hdr->flags & iflag_dummydata) {
            //Rest is dummy data. Reset to start of ringbuffer.
            rb->free_ptr=rb->data;
        } else {
            //Skip past item
            rb->free_ptr+=sizeof(buf_entry_hdr_t);
            //Check if the free_ptr overshoots the buffer.
            //Checking this before aligning free_ptr since it is possible that alignment
            //will cause pointer to overshoot, if the ringbuf size is not a multiple of 4
            configASSERT(rb->free_ptr+hdr->len<=rb->data+rb->size);
            //Align free_ptr to 4 byte boundary. Overshoot condition will result in wrap around below
            size_t len=(hdr->len+3)&~3;
            rb->free_ptr+=len;
        }
        //The buffer will wrap around if we don't have room for a header anymore.
        //Integer typecasting is used because the first operand can result into a -ve
        //value for cases wherein the ringbuffer size is not a multiple of 4, but the
        //implementation logic aligns free_ptr to 4-byte boundary
        if ((int)((rb->data+rb->size)-rb->free_ptr) < (int)sizeof(buf_entry_hdr_t)) {
            rb->free_ptr=rb->data;
        }
        //The free_ptr can not exceed read_ptr, otherwise write_ptr might overwrite read_ptr.
        //Read_ptr can not set to rb->data with free_ptr, otherwise write_ptr might wrap around to rb->data.
        if(rb->free_ptr == rb->read_ptr) break;
        //Next header
        hdr=(buf_entry_hdr_t *)rb->free_ptr;
    }
}


//Returns an item to the ringbuffer. Will mark the item as free, and will see if the free pointer
//can be increase.
//This function by itself is not threadsafe, always call from within a muxed section.
static void returnItemToRingbufBytebuf(ringbuf_t *rb, void *item) {
    uint8_t *data=(uint8_t*)item;
    configASSERT(data >= rb->data);
    configASSERT(data < rb->data+rb->size);
    //Free the read memory.
    rb->free_ptr=rb->read_ptr;
}

void xRingbufferPrintInfo(RingbufHandle_t ringbuf)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    ets_printf("Rb size %d free %d rptr %d freeptr %d wptr %d\n",
            rb->size, ringbufferFreeMem(rb), rb->read_ptr-rb->data, rb->free_ptr-rb->data, rb->write_ptr-rb->data);
}


size_t xRingbufferGetCurFreeSize(RingbufHandle_t ringbuf)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    configASSERT(rb->getFreeSizeImpl);
    int free_size = rb->getFreeSizeImpl(rb);
    //Reserve one byte. If we do not do this and the entire buffer is filled, we get a situation
    //where read_ptr == free_ptr, messing up the next calculation.
    return free_size - 1;
}

static size_t getCurFreeSizeByteBuf(ringbuf_t *rb)
{
    //Return whatever space is available depending on relative positions of
    //the free pointer and write pointer. There is no overhead of headers in
    //this mode
    int free_size = rb->free_ptr-rb->write_ptr;
    if (free_size <= 0)
        free_size += rb->size;
    return free_size;
}

static size_t getCurFreeSizeAllowSplit(ringbuf_t *rb)
{
    int free_size;
    //If Both, the write and free pointer are at the start. Hence, the entire buffer
    //is available (minus the space for the header)
    if (rb->write_ptr == rb->free_ptr && rb->write_ptr == rb->data) {
        free_size = rb->size - sizeof(buf_entry_hdr_t);
    } else if (rb->write_ptr < rb->free_ptr) {
        //Else if the free pointer is beyond the write pointer, only the space between
        //them would be available (minus the space for the header)
        free_size = rb->free_ptr - rb->write_ptr - sizeof(buf_entry_hdr_t);
    } else {
        //Else the data can wrap around and 2 headers will be required
        free_size = rb->free_ptr - rb->write_ptr + rb->size - (2 * sizeof(buf_entry_hdr_t));
    }
    return free_size;
}

static size_t getCurFreeSizeNoSplit(ringbuf_t *rb)
{
    int free_size;
    //If the free pointer is beyond the write pointer, only the space between
    //them would be available
    if (rb->write_ptr < rb->free_ptr) {
        free_size = rb->free_ptr - rb->write_ptr;
    } else {
        //Else check which one is bigger amongst the below 2
        //1) Space from the write pointer to the end of buffer
        int size1 = rb->data + rb->size - rb->write_ptr;
        //2) Space from the start of buffer to the free pointer
        int size2 = rb->free_ptr - rb->data;
        //And then select the larger of the two
        free_size = size1 > size2 ? size1 : size2;
    }
    //In any case, a single header will be used, so subtracting the space that
    //would be required for it
    return free_size - sizeof(buf_entry_hdr_t);
}


RingbufHandle_t xRingbufferCreate(size_t buf_length, ringbuf_type_t type)
{
    ringbuf_t *rb = malloc(sizeof(ringbuf_t));
    if (rb==NULL) goto err;
    memset(rb, 0, sizeof(ringbuf_t));
    rb->data = malloc(buf_length);
    if (rb->data == NULL) goto err;
    rb->size = buf_length;
    rb->free_ptr = rb->data;
    rb->read_ptr = rb->data;
    rb->write_ptr = rb->data;
    rb->free_space_sem = xSemaphoreCreateBinary();
    rb->items_buffered_sem = xSemaphoreCreateBinary();
    rb->flags=0;
    if (type==RINGBUF_TYPE_ALLOWSPLIT) {
        rb->flags|=flag_allowsplit;
        rb->copyItemToRingbufImpl=copyItemToRingbufAllowSplit;
        rb->getItemFromRingbufImpl=getItemFromRingbufDefault;
        rb->returnItemToRingbufImpl=returnItemToRingbufDefault;
        //Calculate max item size. Worst case, we need to split an item into two, which means two headers of overhead.
        rb->maxItemSize=rb->size-(sizeof(buf_entry_hdr_t)*2)-4;
        rb->getFreeSizeImpl=getCurFreeSizeAllowSplit;
    } else if (type==RINGBUF_TYPE_BYTEBUF) {
        rb->flags|=flag_bytebuf;
        rb->copyItemToRingbufImpl=copyItemToRingbufByteBuf;
        rb->getItemFromRingbufImpl=getItemFromRingbufByteBuf;
        rb->returnItemToRingbufImpl=returnItemToRingbufBytebuf;
        //Calculate max item size. We have no headers and can split anywhere -> size is total size minus one.
        rb->maxItemSize=rb->size-1;
        rb->getFreeSizeImpl=getCurFreeSizeByteBuf;
    } else if (type==RINGBUF_TYPE_NOSPLIT) {
        rb->copyItemToRingbufImpl=copyItemToRingbufNoSplit;
        rb->getItemFromRingbufImpl=getItemFromRingbufDefault;
        rb->returnItemToRingbufImpl=returnItemToRingbufDefault;
        //Calculate max item size. Worst case, we have the write ptr in such a position that we are lacking four bytes of free
        //memory to put an item into the rest of the memory. If this happens, we have to dummy-fill
        //(item_data-4) bytes of buffer, then we only have (size-(item_data-4) bytes left to fill
        //with the real item. (item size being header+data)
        rb->maxItemSize=(rb->size/2)-sizeof(buf_entry_hdr_t)-4;
        rb->getFreeSizeImpl=getCurFreeSizeNoSplit;
    } else {
        configASSERT(0);
    }
    if (rb->free_space_sem == NULL || rb->items_buffered_sem == NULL) goto err;
    vPortCPUInitializeMutex(&rb->mux);

    return (RingbufHandle_t)rb;

err:
    //Some error has happened. Free/destroy all allocated things and return NULL.
    if (rb) {
        free(rb->data);
        if (rb->free_space_sem) vSemaphoreDelete(rb->free_space_sem);
        if (rb->items_buffered_sem) vSemaphoreDelete(rb->items_buffered_sem);
    }
    free(rb);
    return NULL;
}

RingbufHandle_t xRingbufferCreateNoSplit(size_t item_size, size_t num_item)
{
    size_t aligned_size = (item_size+3)&~3;
    return xRingbufferCreate((aligned_size + sizeof(buf_entry_hdr_t)) * num_item, RINGBUF_TYPE_NOSPLIT);
}

void vRingbufferDelete(RingbufHandle_t ringbuf) {
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    if (rb) {
        free(rb->data);
        if (rb->free_space_sem) vSemaphoreDelete(rb->free_space_sem);
        if (rb->items_buffered_sem) vSemaphoreDelete(rb->items_buffered_sem);
    }
    free(rb);
}

size_t xRingbufferGetMaxItemSize(RingbufHandle_t ringbuf)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    return rb->maxItemSize;
}

bool xRingbufferIsNextItemWrapped(RingbufHandle_t ringbuf)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    buf_entry_hdr_t *hdr=(buf_entry_hdr_t *)rb->read_ptr;
    return hdr->flags & iflag_wrap;
}


BaseType_t xRingbufferSend(RingbufHandle_t ringbuf, void *data, size_t dataSize, TickType_t ticks_to_wait)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    size_t needed_size=dataSize+sizeof(buf_entry_hdr_t);
    BaseType_t done=pdFALSE;
    TickType_t ticks_end = xTaskGetTickCount() + ticks_to_wait;
    TickType_t ticks_remaining = ticks_to_wait;

    configASSERT(rb);

    if (dataSize > xRingbufferGetMaxItemSize(ringbuf)) {
        //Data will never ever fit in the queue.
        return pdFALSE;
    }

    while (!done) {
        //Check if there is enough room in the buffer. If not, wait until there is.
        do {
            if (ringbufferFreeMem(rb) < needed_size) {
                //Data does not fit yet. Wait until the free_space_sem is given, then re-evaluate.

                BaseType_t r = xSemaphoreTake(rb->free_space_sem, ticks_remaining);
                if (r == pdFALSE) {
                    //Timeout.
                    return pdFALSE;
                }
                //Adjust ticks_remaining; we may have waited less than that and in the case the free memory still is not enough,
                //we will need to wait some more.
                if (ticks_to_wait != portMAX_DELAY) {
                    ticks_remaining = ticks_end - xTaskGetTickCount();
                }

                // ticks_remaining will always be less than or equal to the original ticks_to_wait,
                // unless the timeout is reached - in which case it unsigned underflows to a much
                // higher value.
                //
                // (Check is written this non-intuitive way to allow for the case where xTaskGetTickCount()
                // has overflowed but the ticks_end value has not overflowed.)
            }
        } while (ringbufferFreeMem(rb) < needed_size && ticks_remaining > 0 && ticks_remaining <= ticks_to_wait);

        //Lock the mux in order to make sure no one else is messing with the ringbuffer and do the copy.
        portENTER_CRITICAL(&rb->mux);
        //Another thread may have been able to sneak its write first. Check again now we locked the ringbuff, and retry
        //everything if this is the case. Otherwise, we can write and are done.
        done=rb->copyItemToRingbufImpl(rb, data, dataSize);
        portEXIT_CRITICAL(&rb->mux);
    }
    xSemaphoreGive(rb->items_buffered_sem);
    return pdTRUE;
}


BaseType_t xRingbufferSendFromISR(RingbufHandle_t ringbuf, void *data, size_t dataSize, BaseType_t *higher_prio_task_awoken) 
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    BaseType_t write_succeeded;
    configASSERT(rb);
    size_t needed_size=dataSize+sizeof(buf_entry_hdr_t);
    portENTER_CRITICAL_ISR(&rb->mux);
    if (needed_size>ringbufferFreeMem(rb)) {
        //Does not fit in the remaining space in the ringbuffer.
        write_succeeded=pdFALSE;
    } else {
        write_succeeded = rb->copyItemToRingbufImpl(rb, data, dataSize);
    }
    portEXIT_CRITICAL_ISR(&rb->mux);
    if (write_succeeded) {
        xSemaphoreGiveFromISR(rb->items_buffered_sem, higher_prio_task_awoken);
    }
    return write_succeeded;
}


static void *xRingbufferReceiveGeneric(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait, size_t wanted_size) 
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    uint8_t *itemData;
    BaseType_t done=pdFALSE;
    configASSERT(rb);
    while(!done) {
        //See if there's any data available. If not, wait until there is.
        while (rb->read_ptr == rb->write_ptr) {
            BaseType_t r=xSemaphoreTake(rb->items_buffered_sem, ticks_to_wait);
            if (r == pdFALSE) {
                //Timeout.
                return NULL;
            }
        }
        //Okay, we seem to have data in the buffer. Grab the mux and copy it out if it's still there.
        portENTER_CRITICAL(&rb->mux);
        itemData=rb->getItemFromRingbufImpl(rb, item_size, wanted_size);
        portEXIT_CRITICAL(&rb->mux);
        if (itemData) {
            //We managed to get an item.
            done=pdTRUE;
        }
    }
    return (void*)itemData;
}

void *xRingbufferReceive(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait)
{
    return xRingbufferReceiveGeneric(ringbuf, item_size, ticks_to_wait, 0);
}


void *xRingbufferReceiveFromISR(RingbufHandle_t ringbuf, size_t *item_size) 
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    uint8_t *itemData;
    configASSERT(rb);
    portENTER_CRITICAL_ISR(&rb->mux);
    itemData=rb->getItemFromRingbufImpl(rb, item_size, 0);
    portEXIT_CRITICAL_ISR(&rb->mux);
    return (void*)itemData;
}

void *xRingbufferReceiveUpTo(RingbufHandle_t ringbuf, size_t *item_size, TickType_t ticks_to_wait, size_t wanted_size) {
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    if (wanted_size == 0) return NULL;
    configASSERT(rb);
    configASSERT(rb->flags & flag_bytebuf);
    return xRingbufferReceiveGeneric(ringbuf, item_size, ticks_to_wait, wanted_size);
}

void *xRingbufferReceiveUpToFromISR(RingbufHandle_t ringbuf, size_t *item_size, size_t wanted_size)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    uint8_t *itemData;
    if (wanted_size == 0) return NULL;
    configASSERT(rb);
    configASSERT(rb->flags & flag_bytebuf);
    portENTER_CRITICAL_ISR(&rb->mux);
    itemData=rb->getItemFromRingbufImpl(rb, item_size, wanted_size);
    portEXIT_CRITICAL_ISR(&rb->mux);
    return (void*)itemData;
}


void vRingbufferReturnItem(RingbufHandle_t ringbuf, void *item) 
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    portENTER_CRITICAL(&rb->mux);
    rb->returnItemToRingbufImpl(rb, item);
    portEXIT_CRITICAL(&rb->mux);
    xSemaphoreGive(rb->free_space_sem);
}


void vRingbufferReturnItemFromISR(RingbufHandle_t ringbuf, void *item, BaseType_t *higher_prio_task_awoken) 
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    portENTER_CRITICAL_ISR(&rb->mux);
    rb->returnItemToRingbufImpl(rb, item);
    portEXIT_CRITICAL_ISR(&rb->mux);
    xSemaphoreGiveFromISR(rb->free_space_sem, higher_prio_task_awoken);
}


BaseType_t xRingbufferAddToQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    return xQueueAddToSet(rb->items_buffered_sem, xQueueSet);
}


BaseType_t xRingbufferAddToQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    return xQueueAddToSet(rb->free_space_sem, xQueueSet);
}


BaseType_t xRingbufferRemoveFromQueueSetRead(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    return xQueueRemoveFromSet(rb->items_buffered_sem, xQueueSet);
}

BaseType_t xRingbufferRemoveFromQueueSetWrite(RingbufHandle_t ringbuf, QueueSetHandle_t xQueueSet)
{
    ringbuf_t *rb=(ringbuf_t *)ringbuf;
    configASSERT(rb);
    return xQueueRemoveFromSet(rb->free_space_sem, xQueueSet);
}

