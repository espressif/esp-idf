/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SDIO slave (common part)

#include <string.h>
#include <inttypes.h>
#include "soc/sdio_slc_struct.h"
#include "soc/sdio_hinf_struct.h"
#include "hal/sdio_slave_types.h"
#include "soc/sdio_slc_host_struct.h"
#include "hal/sdio_slave_hal.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "esp_attr.h"

#define SDIO_SLAVE_CHECK(res, str, ret_val) do { if(!(res)){\
    HAL_LOGE(TAG, "%s", str);\
    return ret_val;\
} }while (0)

HAL_LOG_ATTR_TAG(TAG, "SDIO_HAL");

static esp_err_t init_send_queue(sdio_slave_context_t *hal);

/**************** Ring buffer for SDIO sending use *****************/
typedef enum {
    RINGBUF_GET_ONE = 0,
    RINGBUF_GET_ALL = 1,
} ringbuf_get_all_t;

typedef enum {
    RINGBUF_WRITE_PTR,
    RINGBUF_READ_PTR,
    RINGBUF_FREE_PTR,
} sdio_ringbuf_pointer_t;

static esp_err_t sdio_ringbuf_send(sdio_ringbuf_t *buf, esp_err_t (*copy_callback)(uint8_t *, void *), void *arg);
static inline esp_err_t sdio_ringbuf_recv(sdio_ringbuf_t *buf, sdio_slave_hal_send_desc_t **start, sdio_slave_hal_send_desc_t **end, ringbuf_get_all_t get_all);
static inline int sdio_ringbuf_return(sdio_ringbuf_t* buf, uint8_t *ptr);

#define _SEND_DESC_NEXT(x)    STAILQ_NEXT(&((sdio_slave_hal_send_desc_t*)x)->dma_desc, qe)
#define SEND_DESC_NEXT(x)    (sdio_slave_hal_send_desc_t*)_SEND_DESC_NEXT(x)
#define SEND_DESC_NEXT_SET(x, target)    do { \
        _SEND_DESC_NEXT(x)=(sdio_slave_ll_desc_t*)target; \
    }while(0)

static esp_err_t link_desc_to_last(uint8_t* desc, void* arg)
{
    SEND_DESC_NEXT_SET(arg, desc);
    return ESP_OK;
}

//calculate a pointer with offset to a original pointer of the specific ringbuffer
static inline uint8_t* sdio_ringbuf_offset_ptr(sdio_ringbuf_t *buf, sdio_ringbuf_pointer_t ptr, uint32_t offset)
{
    uint8_t *buf_ptr;
    switch (ptr) {
    case RINGBUF_WRITE_PTR:
        buf_ptr = buf->write_ptr;
        break;
    case RINGBUF_READ_PTR:
        buf_ptr = buf->read_ptr;
        break;
    case RINGBUF_FREE_PTR:
        buf_ptr = buf->free_ptr;
        break;
    default:
        abort();
    }

    uint8_t *offset_ptr = buf_ptr + offset;
    if (offset_ptr >= buf->data + buf->size) {
        offset_ptr -= buf->size;
    }
    return offset_ptr;
}

static esp_err_t sdio_ringbuf_send(sdio_ringbuf_t *buf, esp_err_t (*copy_callback)(uint8_t *, void *), void *arg)
{
    uint8_t* get_ptr = sdio_ringbuf_offset_ptr(buf, RINGBUF_WRITE_PTR, SDIO_SLAVE_SEND_DESC_SIZE);
    esp_err_t err = ESP_OK;
    if (copy_callback) {
        err = (*copy_callback)(get_ptr, arg);
    }
    if (err != ESP_OK) {
        return err;
    }

    buf->write_ptr = get_ptr;
    return ESP_OK;
}

// this ringbuf is a return-before-recv-again strategy
// since this is designed to be called in the ISR, no parallel logic
static inline esp_err_t sdio_ringbuf_recv(sdio_ringbuf_t *buf, sdio_slave_hal_send_desc_t **start, sdio_slave_hal_send_desc_t **end, ringbuf_get_all_t get_all)
{
    HAL_ASSERT(buf->free_ptr == buf->read_ptr);   //must return before recv again
    if (start == NULL && end == NULL) {
        return ESP_ERR_INVALID_ARG;    // must have a output
    }
    if (buf->read_ptr == buf->write_ptr) {
        return ESP_ERR_NOT_FOUND;    // no data
    }

    uint8_t *get_start = sdio_ringbuf_offset_ptr(buf, RINGBUF_READ_PTR, SDIO_SLAVE_SEND_DESC_SIZE);

    if (get_all != RINGBUF_GET_ONE) {
        buf->read_ptr = buf->write_ptr;
    } else {
        buf->read_ptr = get_start;
    }

    if (start != NULL) {
        *start = (sdio_slave_hal_send_desc_t *) get_start;
    }
    if (end != NULL) {
        *end = (sdio_slave_hal_send_desc_t *) buf->read_ptr;
    }
    return ESP_OK;
}

static inline int sdio_ringbuf_return(sdio_ringbuf_t* buf, uint8_t *ptr)
{
    HAL_ASSERT(sdio_ringbuf_offset_ptr(buf, RINGBUF_FREE_PTR, SDIO_SLAVE_SEND_DESC_SIZE) == ptr);
    size_t size = (buf->read_ptr + buf->size - buf->free_ptr) % buf->size;
    size_t count = size / SDIO_SLAVE_SEND_DESC_SIZE;
    HAL_ASSERT(count * SDIO_SLAVE_SEND_DESC_SIZE == size);
    buf->free_ptr = buf->read_ptr;
    return count;
}

static inline uint8_t* sdio_ringbuf_peek_front(sdio_ringbuf_t* buf)
{
    if (buf->read_ptr != buf->write_ptr) {
        return sdio_ringbuf_offset_ptr(buf, RINGBUF_READ_PTR, SDIO_SLAVE_SEND_DESC_SIZE);
    } else {
        return NULL;
    }
}

static inline uint8_t* sdio_ringbuf_peek_rear(sdio_ringbuf_t *buf)
{
    return buf->write_ptr;
}

static inline bool sdio_ringbuf_empty(sdio_ringbuf_t* buf)
{
    return (buf->read_ptr == buf->write_ptr);
}

/**************** End of Ring buffer *****************/

void sdio_slave_hal_init(sdio_slave_context_t *hal)
{
    hal->host = sdio_slave_ll_get_host(0);
    hal->slc = sdio_slave_ll_get_slc(0);
    hal->hinf = sdio_slave_ll_get_hinf(0);
    hal->send_state = STATE_IDLE;
    hal->recv_link_list = (sdio_slave_hal_recv_stailq_t)STAILQ_HEAD_INITIALIZER(hal->recv_link_list);

    init_send_queue(hal);
}

void sdio_slave_hal_hw_init(sdio_slave_context_t *hal)
{
    sdio_slave_ll_init(hal->slc);
    sdio_slave_ll_enable_hs(hal->hinf, !hal->no_highspeed);
    sdio_slave_ll_set_timing(hal->host, hal->timing);
    sdio_slave_ll_slvint_t intr_ena = 0xff;
    sdio_slave_ll_slvint_set_ena(hal->slc, &intr_ena);
}

static esp_err_t init_send_queue(sdio_slave_context_t *hal)
{
    esp_err_t ret;
    esp_err_t rcv_res __attribute((unused));
    sdio_ringbuf_t *buf = &(hal->send_desc_queue);

    //initialize pointers
    buf->write_ptr = buf->data;
    buf->read_ptr = buf->data;
    buf->free_ptr = buf->data;

    sdio_slave_hal_send_desc_t *first = NULL, *last = NULL;
    //no copy for the first descriptor

    ret = sdio_ringbuf_send(buf, NULL, NULL);
    if (ret != ESP_OK) {
        return ret;
    }

    //loop in the ringbuf to link all the desc one after another as a ring
    for (int i = 0; i < hal->send_queue_size + 1; i++) {
        rcv_res = sdio_ringbuf_recv(buf, &last, NULL, RINGBUF_GET_ONE);
        HAL_ASSERT(rcv_res == ESP_OK);

        ret = sdio_ringbuf_send(buf, link_desc_to_last, last);
        if (ret != ESP_OK) {
            return ret;
        }

        sdio_ringbuf_return(buf, (uint8_t *) last);
    }

    first = NULL;
    last = NULL;
    //clear the queue
    rcv_res = sdio_ringbuf_recv(buf, &first, &last, RINGBUF_GET_ALL);
    HAL_ASSERT(rcv_res == ESP_OK);
    HAL_ASSERT(first == last); //there should be only one desc remain
    sdio_ringbuf_return(buf, (uint8_t *) first);
    return ESP_OK;
}

void sdio_slave_hal_set_ioready(sdio_slave_context_t *hal, bool ready)
{
    sdio_slave_ll_set_ioready(hal->hinf, ready);   //set IO ready to 1 to allow host to use
}

/*---------------------------------------------------------------------------
 *                  Send
 *
 *  The hardware has a cache, so that once a descriptor is loaded onto the linked-list, it cannot be modified
 *  until returned (used) by the hardware. This forbids us from loading descriptors onto the linked list during
 *  the transfer (or the time waiting for host to start a transfer). However, we use a "ringbuffer" (different from
 *  the one in ``freertos/`` folder) holding descriptors to solve this:

 *  1.  The driver allocates continuous memory for several buffer descriptors (the maximum buffer number) during
 *      initialization. Then the driver points the STAILQ_NEXT pointer of all the descriptors except the last one
 *      to the next descriptor of each of them. Then the pointer of the last descriptor points back to the first one:
 *      now the descriptor is in a ring.

 *  2.  The "ringbuffer" has a write pointer points to where app can write new descriptor. The app writes the new descriptor
 *      indicated by the write pointer without touching the STAILQ_NEXT pointer so that the descriptors are always in a
 *      ring-like linked-list. The app never touches the part of linked-list being used by the hardware.

 *  3.  When the hardware needs some data to send, it automatically pick a part of linked descriptors. According to the mode:
 *          - Buffer mode: only pick the next one to the last one sent;
 *          - Stream mode: pick the whole unsent linked list, starting from the one above, to the latest linked one.

 *      The driver removes the STAILQ_NEXT pointer of the last descriptor and put the head of the part to the DMA controller so
 *      that it looks like just a linear linked-list rather than a ring to the hardware.

 *  4.  The counter of sending FIFO can increase when app load new buffers (in STREAM_MODE) or when new transfer should
 *      start (in PACKET_MODE).

 *  5.  When the sending transfer is finished, the driver goes through the descriptors just send in the ISR and push all
 *      the ``arg`` member of descriptors to the queue back to the app, so that the app can handle finished buffers. The
 *      driver also fix the STAILQ_NEXT pointer of the last descriptor so that the descriptors are now in a ring again.
----------------------------------------------------------------------------*/
static inline void send_set_state(sdio_slave_context_t *hal, send_state_t state)
{
    hal->send_state = state;
}

static inline send_state_t send_get_state(sdio_slave_context_t* hal)
{
    return hal->send_state;
}

DMA_ATTR static const sdio_slave_ll_desc_t start_desc = {
    .owner = 1,
    .buf = (void*)0x3ffbbbbb, //assign a dma-capable pointer other than NULL, which will not be used
    .size = 1,
    .length = 1,
    .eof = 1,
};

//force trigger rx_done interrupt. the interrupt is abused to invoke ISR from the app by the enable bit and never cleared.
static void send_isr_invoker_enable(const sdio_slave_context_t *hal)
{
    sdio_slave_ll_send_reset(hal->slc);
    sdio_slave_ll_send_start(hal->slc, &start_desc);
    //wait for rx_done
    while (!sdio_slave_ll_send_invoker_ready(hal->slc));
    sdio_slave_ll_send_stop(hal->slc);
    sdio_slave_ll_send_hostint_clr(hal->host);
}

static void send_isr_invoker_disable(sdio_slave_context_t *hal)
{
    sdio_slave_ll_send_part_done_clear(hal->slc);
}

void sdio_slave_hal_send_handle_isr_invoke(sdio_slave_context_t *hal)
{
    sdio_slave_ll_send_part_done_intr_ena(hal->slc, false);
}

//start hw operation with existing data (if exist)
esp_err_t sdio_slave_hal_send_start(sdio_slave_context_t *hal)
{
    SDIO_SLAVE_CHECK(send_get_state(hal) == STATE_IDLE,
                     "already started", ESP_ERR_INVALID_STATE);
    send_set_state(hal, STATE_WAIT_FOR_START);
    send_isr_invoker_enable(hal);
    sdio_slave_ll_send_intr_clr(hal->slc);
    sdio_slave_ll_send_intr_ena(hal->slc, true);
    return ESP_OK;
}

//only stop hw operations, no touch to data as well as counter
void sdio_slave_hal_send_stop(sdio_slave_context_t *hal)
{
    sdio_slave_ll_send_stop(hal->slc);
    send_isr_invoker_disable(hal);
    sdio_slave_ll_send_intr_ena(hal->slc, false);
    send_set_state(hal, STATE_IDLE);
}

static void send_new_packet(sdio_slave_context_t *hal)
{
    // since eof is changed, we have to stop and reset the link list,
    // and restart new link list operation
    sdio_slave_hal_send_desc_t *const start_desc = hal->in_flight_head;
    sdio_slave_hal_send_desc_t *const end_desc = hal->in_flight_end;
    HAL_ASSERT(start_desc != NULL && end_desc != NULL);

    sdio_slave_ll_send_stop(hal->slc);
    sdio_slave_ll_send_reset(hal->slc);
    sdio_slave_ll_send_start(hal->slc, (sdio_slave_ll_desc_t*)start_desc);

    // update pkt_len register to allow host reading.
    sdio_slave_ll_send_write_len(hal->slc, end_desc->pkt_len);
    HAL_EARLY_LOGV(TAG, "send_length_write: %"PRIu32", last_len: %08"PRIX32"", end_desc->pkt_len, sdio_slave_ll_send_read_len(hal->host));

    send_set_state(hal, STATE_SENDING);

    HAL_EARLY_LOGD(TAG, "restart new send: %p->%p, pkt_len: %"PRIu32"", start_desc, end_desc, end_desc->pkt_len);
}

static esp_err_t send_check_new_packet(sdio_slave_context_t *hal)
{
    esp_err_t ret;
    sdio_slave_hal_send_desc_t *start = NULL;
    sdio_slave_hal_send_desc_t *end = NULL;
    if (hal->sending_mode == SDIO_SLAVE_SEND_PACKET) {
        ret = sdio_ringbuf_recv(&(hal->send_desc_queue), &start, &end, RINGBUF_GET_ONE);
    } else { //stream mode
        ret = sdio_ringbuf_recv(&(hal->send_desc_queue), &start, &end, RINGBUF_GET_ALL);
    }
    if (ret == ESP_OK) {
        hal->in_flight_head = start;
        hal->in_flight_end = end;
        end->dma_desc.eof = 1;
        //temporarily break the link ring here, the ring will be re-connected in ``send_isr_eof()``.
        hal->in_flight_next = SEND_DESC_NEXT(end);
        SEND_DESC_NEXT_SET(end, NULL);
    }
    return ESP_OK;
}

bool sdio_slave_hal_send_eof_happened(sdio_slave_context_t* hal)
{
    // Goto idle state (cur_start=NULL) if transmission done,
    // also update sequence and recycle descs.
    if (sdio_slave_ll_send_done(hal->slc)) {
        //check current state
        HAL_ASSERT(send_get_state(hal) == STATE_SENDING);
        sdio_slave_ll_send_intr_clr(hal->slc);
        return true;
    } else {
        return false;
    }
}

//clear counter but keep data
esp_err_t sdio_slave_hal_send_reset_counter(sdio_slave_context_t* hal)
{
    SDIO_SLAVE_CHECK(send_get_state(hal) == STATE_IDLE,
                     "reset counter when transmission started", ESP_ERR_INVALID_STATE);

    sdio_slave_ll_send_write_len(hal->slc, 0);
    HAL_EARLY_LOGV(TAG, "last_len: %08X", sdio_slave_ll_send_read_len(hal->host));

    hal->tail_pkt_len = 0;
    sdio_slave_hal_send_desc_t *desc = hal->in_flight_head;
    while (desc != NULL) {
        hal->tail_pkt_len += desc->dma_desc.length;
        desc->pkt_len = hal->tail_pkt_len;
        desc = SEND_DESC_NEXT(desc);
    }
    // in theory the desc should be the one right next to the last of in_flight_head,
    // but the link of last is NULL, so get the desc from the ringbuf directly.
    desc = (sdio_slave_hal_send_desc_t*)sdio_ringbuf_peek_front(&(hal->send_desc_queue));
    while (desc != NULL) {
        hal->tail_pkt_len += desc->dma_desc.length;
        desc->pkt_len = hal->tail_pkt_len;
        desc = SEND_DESC_NEXT(desc);
    }

    return ESP_OK;
}

static esp_err_t send_get_inflight_desc(sdio_slave_context_t *hal, void **out_arg, uint32_t *out_returned_cnt,
                                        bool init)
{
    esp_err_t ret;
    if (init) {
        HAL_ASSERT(hal->returned_desc == NULL);
        hal->returned_desc = hal->in_flight_head;
        send_set_state(hal, STATE_GETTING_RESULT);
    }

    if (hal->returned_desc != NULL) {
        *out_arg = hal->returned_desc->arg;
        hal->returned_desc = SEND_DESC_NEXT(hal->returned_desc);
        ret = ESP_OK;
    } else {
        if (hal->in_flight_head != NULL) {
            // fix the link broken of last desc when being sent
            HAL_ASSERT(hal->in_flight_end != NULL);
            SEND_DESC_NEXT_SET(hal->in_flight_end, hal->in_flight_next);

            *out_returned_cnt = sdio_ringbuf_return(&(hal->send_desc_queue), (uint8_t*)hal->in_flight_head);
        }

        hal->in_flight_head = NULL;
        hal->in_flight_end = NULL;

        ret = ESP_ERR_NOT_FOUND;
    }
    return ret;
}

static esp_err_t send_get_unsent_desc(sdio_slave_context_t *hal, void **out_arg, uint32_t *out_return_cnt)
{
    esp_err_t ret;
    sdio_slave_hal_send_desc_t *head = NULL;
    sdio_slave_hal_send_desc_t *tail = NULL;
    ret = sdio_ringbuf_recv(&(hal->send_desc_queue), &head, &tail, RINGBUF_GET_ONE);

    if (ret == ESP_OK) {
        //currently each packet takes only one desc.
        HAL_ASSERT(head == tail);
        (*out_arg) = head->arg;
        (*out_return_cnt) = sdio_ringbuf_return(&(hal->send_desc_queue), (uint8_t*) head);
    } else if (ret == ESP_ERR_NOT_FOUND) {
        // if in wait to send state, set the sequence number of tail to the value last sent, just as if the packet wait to
        // send never queued.
        // Go to idle state (cur_end!=NULL and cur_start=NULL)
        send_set_state(hal, STATE_IDLE);
        hal->tail_pkt_len = sdio_slave_ll_send_read_len(hal->host);
    }
    return ret;
}

esp_err_t sdio_slave_hal_send_get_next_finished_arg(sdio_slave_context_t *hal, void **out_arg, uint32_t* out_returned_cnt)
{
    bool init = (send_get_state(hal) == STATE_SENDING);
    if (init) {
        HAL_ASSERT(hal->in_flight_head != NULL);
    } else {
        HAL_ASSERT(send_get_state(hal) == STATE_GETTING_RESULT);
    }
    *out_returned_cnt = 0;

    esp_err_t ret = send_get_inflight_desc(hal, out_arg, out_returned_cnt, init);

    if (ret == ESP_ERR_NOT_FOUND) {
        // Go to wait for packet state
        send_set_state(hal, STATE_WAIT_FOR_START);
    }
    return ret;
}

esp_err_t sdio_slave_hal_send_flush_next_buffer(sdio_slave_context_t *hal, void **out_arg, uint32_t *out_return_cnt)
{
    esp_err_t ret = ESP_OK;
    *out_return_cnt = 0;
    bool init = (send_get_state(hal) == STATE_IDLE);
    if (!init) {
        if (send_get_state(hal) != STATE_GETTING_RESULT && send_get_state(hal) != STATE_GETTING_UNSENT_DESC) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    if (init || send_get_state(hal) == STATE_GETTING_RESULT) {
        ret = send_get_inflight_desc(hal, out_arg, out_return_cnt, init);
        if (ret == ESP_ERR_NOT_FOUND) {
            send_set_state(hal, STATE_GETTING_UNSENT_DESC);
        }
    }
    if (send_get_state(hal) == STATE_GETTING_UNSENT_DESC) {
        ret = send_get_unsent_desc(hal, out_arg, out_return_cnt);
        if (ret == ESP_ERR_NOT_FOUND) {
            send_set_state(hal, STATE_IDLE);
        }
    }
    return ret;
}

esp_err_t sdio_slave_hal_send_new_packet_if_exist(sdio_slave_context_t *hal)
{
    esp_err_t ret;
    // Go to wait sending state (cur_start!=NULL && cur_end==NULL) if not sending and new packet ready.
    // Note we may also enter this state by stopping sending in the app.
    if (send_get_state(hal) == STATE_WAIT_FOR_START) {
        if (hal->in_flight_head == NULL) {
            send_check_new_packet(hal);
        }
        // Go to sending state (cur_start and cur_end != NULL) if has packet to send.
        if (hal->in_flight_head) {
            send_new_packet(hal);
            ret = ESP_OK;
        } else {
            ret = ESP_ERR_NOT_FOUND;
        }
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    return ret;
}

static esp_err_t send_write_desc(uint8_t* desc, void* arg)
{
    sdio_slave_hal_send_desc_t* next_desc = SEND_DESC_NEXT(desc);
    memcpy(desc, arg, sizeof(sdio_slave_hal_send_desc_t));
    SEND_DESC_NEXT_SET(desc, next_desc);
    return ESP_OK;
}

static void send_isr_invoke(sdio_slave_context_t *hal)
{
    sdio_slave_ll_send_part_done_intr_ena(hal->slc, true);
}

esp_err_t sdio_slave_hal_send_queue(sdio_slave_context_t* hal, uint8_t *addr, size_t len, void *arg)
{
    hal->tail_pkt_len += len;
    sdio_slave_hal_send_desc_t new_desc = {
        .dma_desc = {
            .size   =   len,
            .length =   len,
            .buf    =   addr,
            .owner  =   1,
            // in stream mode, the eof is only appended (in ISR) when new packet is ready to be sent
            .eof    = (hal->sending_mode == SDIO_SLAVE_SEND_PACKET),
        },
        .arg    =   arg,
        .pkt_len = hal->tail_pkt_len,
    };

    esp_err_t ret = sdio_ringbuf_send(&(hal->send_desc_queue), send_write_desc, &new_desc);
    send_isr_invoke(hal);
    return ret;
}

/*---------------------------------------------------------------------------
 *                  Receive
 *--------------------------------------------------------------------------*/

static sdio_slave_ll_desc_t* recv_get_first_empty_buf(sdio_slave_context_t* hal)
{
    sdio_slave_hal_recv_stailq_t *const queue = &(hal->recv_link_list);
    sdio_slave_ll_desc_t *desc = STAILQ_FIRST(queue);
    while (desc && desc->owner == 0) {
        desc = STAILQ_NEXT(desc, qe);
    }
    return desc;
}

void sdio_slave_hal_recv_stop(sdio_slave_context_t* hal)
{
    sdio_slave_ll_set_ioready(hal->hinf, false); //set IO ready to 0 to stop host from using
    sdio_slave_ll_send_stop(hal->slc);
    sdio_slave_ll_recv_stop(hal->slc);
    sdio_slave_ll_recv_intr_ena(hal->slc, false);
}

//touching linked list, should be protected by spinlock
bool sdio_slave_hal_recv_has_next_item(sdio_slave_context_t* hal)
{

    if (hal->recv_cur_ret == NULL || hal->recv_cur_ret->owner != 0) {
        return false;
    }

    // This may cause the ``cur_ret`` pointer to be NULL, indicating the list is empty,
    // in this case the ``tx_done`` should happen no longer until new desc is appended.
    // The app is responsible to place the pointer to the right place again when appending new desc.

    hal->recv_cur_ret = STAILQ_NEXT(hal->recv_cur_ret, qe);
    return true;
}

bool sdio_slave_hal_recv_done(sdio_slave_context_t *hal)
{
    bool ret = sdio_slave_ll_recv_done(hal->slc);
    if (ret) {
        sdio_slave_ll_recv_done_clear(hal->slc);
    }
    return ret;
}

sdio_slave_ll_desc_t *sdio_slave_hal_recv_unload_desc(sdio_slave_context_t *hal)
{
    sdio_slave_hal_recv_stailq_t *const queue = &hal->recv_link_list;
    sdio_slave_ll_desc_t *desc = STAILQ_FIRST(queue);
    if (desc) {
        STAILQ_REMOVE_HEAD(queue, qe);
    }
    return desc;
}

void sdio_slave_hal_recv_init_desc(sdio_slave_context_t* hal, sdio_slave_ll_desc_t *desc, uint8_t *start)
{
    *desc = (sdio_slave_ll_desc_t) {
        .size = hal->recv_buffer_size,
        .buf = start,
    };
}

void sdio_slave_hal_recv_start(sdio_slave_context_t *hal)
{
    sdio_slave_ll_recv_reset(hal->slc);
    sdio_slave_ll_desc_t *desc = recv_get_first_empty_buf(hal);
    if (!desc) {
        HAL_LOGD(TAG, "recv: restart without desc");
    } else {
        //the counter is handled when add/flush/reset
        sdio_slave_ll_recv_start(hal->slc, desc);
        sdio_slave_ll_recv_intr_ena(hal->slc, true);
    }
}

void sdio_slave_hal_recv_reset_counter(sdio_slave_context_t *hal)
{
    sdio_slave_ll_recv_size_reset(hal->slc);
    sdio_slave_ll_desc_t *desc = recv_get_first_empty_buf(hal);
    while (desc != NULL) {
        sdio_slave_ll_recv_size_inc(hal->slc);
        desc = STAILQ_NEXT(desc, qe);
    }
}

void sdio_slave_hal_recv_flush_one_buffer(sdio_slave_context_t *hal)
{
    sdio_slave_hal_recv_stailq_t *const queue = &hal->recv_link_list;
    sdio_slave_ll_desc_t *desc = STAILQ_FIRST(queue);
    HAL_ASSERT(desc != NULL && desc->owner == 0);
    STAILQ_REMOVE_HEAD(queue, qe);
    desc->owner = 1;
    STAILQ_INSERT_TAIL(queue, desc, qe);
    sdio_slave_ll_recv_size_inc(hal->slc);
    //we only add it to the tail here, without start the DMA nor increase buffer num.
}

void sdio_slave_hal_load_buf(sdio_slave_context_t *hal, sdio_slave_ll_desc_t *desc)
{
    sdio_slave_hal_recv_stailq_t *const queue = &(hal->recv_link_list);
    desc->owner = 1;

    sdio_slave_ll_desc_t *const tail = STAILQ_LAST(queue, sdio_slave_ll_desc_s, qe);

    STAILQ_INSERT_TAIL(queue, desc, qe);
    if (hal->recv_cur_ret == NULL) {
        hal->recv_cur_ret = desc;
    }

    if (tail == NULL) {
        //no one in the ll, start new ll operation.
        sdio_slave_ll_recv_start(hal->slc, desc);
        sdio_slave_ll_recv_intr_ena(hal->slc, true);
        HAL_LOGV(TAG, "recv_load_buf: start new");
    } else {
        //restart former ll operation
        sdio_slave_ll_recv_restart(hal->slc);
        HAL_LOGV(TAG, "recv_load_buf: restart");
    }
    sdio_slave_ll_recv_size_inc(hal->slc);
}

static inline void show_queue_item(sdio_slave_ll_desc_t *item)
{
    HAL_EARLY_LOGI(TAG, "=> %p: size: %"PRIu32"(%"PRIu32"), eof: %"PRIu32", owner: %"PRIu32"", item, item->size, item->length, item->eof, item->owner);
    HAL_EARLY_LOGI(TAG, "   buf: %p, stqe_next: %p", item->buf, item->qe.stqe_next);
}

static void __attribute((unused)) dump_queue(sdio_slave_hal_recv_stailq_t *queue)
{
    int cnt = 0;
    sdio_slave_ll_desc_t *item = NULL;
    HAL_EARLY_LOGI(TAG, ">>>>> first: %p, last: %p <<<<<", queue->stqh_first, queue->stqh_last);
    STAILQ_FOREACH(item, queue, qe) {
        cnt++;
        show_queue_item(item);
    }
    HAL_EARLY_LOGI(TAG, "total: %d", cnt);
}

/*---------------------------------------------------------------------------
 *                  Host
 *--------------------------------------------------------------------------*/
void sdio_slave_hal_hostint_get_ena(sdio_slave_context_t *hal, sdio_slave_hostint_t *out_int_mask)
{
    *out_int_mask = sdio_slave_ll_host_get_intena(hal->host);
}

void sdio_slave_hal_hostint_clear(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask)
{
    sdio_slave_ll_host_intr_clear(hal->host, mask);//clear all interrupts
}

void sdio_slave_hal_hostint_set_ena(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask)
{
    sdio_slave_ll_host_set_intena(hal->host, mask);
}

void sdio_slave_hal_hostint_send(sdio_slave_context_t *hal, const sdio_slave_hostint_t *mask)
{
    sdio_slave_ll_host_send_int(hal->slc, mask);
}

uint8_t sdio_slave_hal_host_get_reg(sdio_slave_context_t *hal, int pos)
{
    return sdio_slave_ll_host_get_reg(hal->host, pos);
}
void sdio_slave_hal_host_set_reg(sdio_slave_context_t *hal, int pos, uint8_t reg)
{
    sdio_slave_ll_host_set_reg(hal->host, pos, reg);
}

void sdio_slave_hal_slvint_fetch_clear(sdio_slave_context_t *hal, sdio_slave_ll_slvint_t *out_int_mask)
{
    sdio_slave_ll_slvint_fetch_clear(hal->slc, out_int_mask);
}

volatile void* sdio_slave_hal_get_intr_status_reg(sdio_slave_context_t *hal)
{
    return sdio_slave_ll_get_intr_status_reg(hal->slc);
}
