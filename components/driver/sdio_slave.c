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
//

/*
Architecture:

The whole SDIO slave peripheral consists of three parts: the registers (including the control registers of
interrupts and shared registers), the sending FIFO and the receving FIFO. A document ``esp_slave_protocol.rst``
describes the functionality of the peripheral detailedly.
The host can access only one of those parts at once, and the hardware functions of these parts are totally
independent. Hence this driver is designed into these three independent parts. The shared registers are quite
simple. As well as the interrupts: when a slave interrupt is written by the host, the slave gets an interrupt;
when one of the host interrupt bits is active, slave hardware output interrupt signals on the DAT1 line.

For the FIFOs, the peripheral provides counters as registers so that the host can always know whether the slave
is ready to send/receive data. The driver resets the counters during initialization, and the host should somehow
inform the slave to reset the counters again if it should reboot (or lose the counter value for some reasons).
Then the host can read/write the FIFOs by CMD53 commands according to the counters.

Since we don't want to copy all the data from the buffer each time we use sending/receving buffer,
the buffers are directly loaded onto the sending/receiving linked-list and taken off only after use.
Hence the driver takes ownership of the buffer when the buffer is fed to the driver.

The driver returns the ownership of buffers when a "finish" function is called. When the hardware finishes
the sending/receiving of a buffer, the ISR is invoked and it goes through the linked-list to see how many buffers
are freed after last interrupt, and send corresponding signals to the app.

The driver of FIFOs works as below:

1.  The receive driver requires application to "register" a buffer before it's used. The driver
    dynamically allocate a linked-list descriptor for the buffer, and return the descriptor as a handle
    to the app.

    Each time the app asks to receive by a buffer, the descriptor of the buffer is loaded onto the linked-list,
    and the counter of receiving buffers is inreased so that the host will know this by the receiving interrupt.
    The hardware will automatically go through the linked list and write data into the buffers loaded on the
    list.

    The receiving driver sends a counting semaphore to the app for each buffer finished receiving. A task can only
    check the linked list and fetch one finished buffer for a received semaphore.

2.  The sending driver is slightly different due to different hardware working styles.
    (TODO: re-write this part if the stitch mode is released)
    The hardware has a cache, so that once a descriptor is loaded onto the linked-list, it cannot be modified
    until returned (used) by the hardware. This forbids us from loading descriptors onto the linked list during
    the transfer (or the time waiting for host to start a transfer). However, we use a "ringbuffer" (different from
    the one in ``freertos/`` folder) holding descriptors to solve this:

    1.  The driver allocates continuous memory for several buffer descriptors (the maximum buffer number) during
        initialization. Then the driver points the STAILQ_NEXT pointer of all the descriptors except the last one
        to the next descriptor of each of them. Then the pointer of the last descriptor points back to the first one:
        now the descriptor is in a ring.

    2.  The "ringbuffer" has a write pointer points to where app can write new descriptor. The app writes the new descriptor
        indicated by the write pointer without touching the STAILQ_NEXT pointer so that the descriptors are always in a
        ring-like linked-list. The app never touches the part of linked-list being used by the hardware.

    3.  When the hardware needs some data to send, it automatically pick a part of connected descriptors. According to the mode:
            - Buffer mode: only pick the next one of the last sent one;
            - Stream mode: pick the one above to the latest one.

        The driver removes the STAILQ_NEXT pointer of the last descriptor and put the head of the part to the DMA controller so
        that it looks like just a linear linked-list rather than a ring to the hardware.

    4.  The counter of sending FIFO can increase when app load new buffers (in STREAM_MODE) or when new transfer should
        start (in PACKET_MODE).

    5.  When the sending transfer is finished, the driver goes through the descriptors just send in the ISR and push all
        the ``arg`` member of descriptors to the queue back to the app, so that the app can handle finished buffers. The
        driver also fix the STAILQ_NEXT pointer of the last descriptor so that the descriptors are now in a ring again.
*/



#include <string.h>
#include "driver/sdio_slave.h"
#include "soc/sdio_slave_periph.h"
#include "esp32/rom/lldesc.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "soc/soc_memory_layout.h"
#include "soc/gpio_periph.h"
#include "freertos/semphr.h"
#include "xtensa/core-macros.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"


#define SDIO_SLAVE_CHECK(res, str, ret_val) do { if(!(res)){\
    SDIO_SLAVE_LOGE("%s", str);\
    return ret_val;\
} }while (0)

#define SDIO_SLAVE_LOGE(s, ...) ESP_LOGE(TAG, "%s:%d (%s):"s, __FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#define SDIO_SLAVE_LOGW(s, ...) ESP_LOGW(TAG, "%s: "s, __FUNCTION__,##__VA_ARGS__)


static const char TAG[] = "sdio_slave";

typedef enum {
    STATE_IDLE = 1,
    STATE_WAIT_FOR_START = 2,
    STATE_SENDING = 3,
} send_state_t;

// first 3 WORDs of this struct is defined by and compatible to the DMA link list format.
// sdio_slave_buf_handle_t is of type buf_desc_t*;
typedef struct  buf_desc_s{
    volatile uint32_t size  :12,
             length:12,
             offset: 5, /* h/w reserved 5bit, s/w use it as offset in buffer */
             sosf  : 1, /* start of sub-frame */
             eof   : 1, /* end of frame */
             owner : 1; /* hw or sw */
    uint8_t* buf;
    union{
        TAILQ_ENTRY(buf_desc_s) te; // tailq used by receving
        struct {
            STAILQ_ENTRY(buf_desc_s) qe; // stailq used by sending and receiving
            union {
                uint32_t pkt_len;
                // shared with the tqe_prev in tailq, happen to be non-zero in the stailq. only
                // write to 0 when removed from tailq, set to other will bring invalid pointer.
                uint32_t not_receiving;
            };
        };
    };
    void*   arg;        /* to hold some parameters */
} buf_desc_t;

typedef STAILQ_HEAD(bufdesc_stailq_head_s, buf_desc_s) buf_stailq_t;
typedef TAILQ_HEAD(bufdesc_tailq_head_s, buf_desc_s) buf_tailq_t;

typedef struct {
    uint8_t* data;
    uint8_t* write_ptr;
    uint8_t* read_ptr;
    uint8_t* free_ptr;
    size_t  item_size;
    size_t  size;
    portMUX_TYPE      write_spinlock;
    SemaphoreHandle_t remain_cnt;
} sdio_ringbuf_t;

#define offset_of(type, field) ((unsigned int)&(((type *)(0))->field))
typedef enum {
    ringbuf_write_ptr = offset_of(sdio_ringbuf_t, write_ptr),
    ringbuf_read_ptr = offset_of(sdio_ringbuf_t, read_ptr),
    ringbuf_free_ptr = offset_of(sdio_ringbuf_t, free_ptr),
} sdio_ringbuf_pointer_t;

#define SDIO_RINGBUF_INITIALIZER()    (sdio_ringbuf_t){.write_spinlock = portMUX_INITIALIZER_UNLOCKED,}

typedef struct {
    sdio_slave_config_t     config;
    intr_handle_t           intr_handle; //allocated interrupt handle
    /*------- events ---------------*/
    union {
        SemaphoreHandle_t       events[9]; // 0-7 for gp intr
        struct {
            SemaphoreHandle_t   _events[8];
            SemaphoreHandle_t   recv_event; // 8 for recv
        };
    };
    portMUX_TYPE            reg_spinlock;
    /*------- sending ---------------*/
    //desc in the send_link_list are temporary, taken information and space from the ringbuf, return to ringbuf after use.
    send_state_t        send_state;
    sdio_ringbuf_t      sendbuf;
    QueueHandle_t       ret_queue;
    buf_desc_t*         in_flight;
    buf_desc_t*         in_flight_end;
    buf_desc_t*         in_flight_next;
    /*------- receiving ---------------*/
    buf_stailq_t    recv_link_list; // now ready to/already hold data
    buf_tailq_t     recv_reg_list;  // removed from the link list, registered but not used now
    volatile buf_desc_t*     recv_cur_ret;   // next desc to return, NULL if all loaded descriptors are returned
    portMUX_TYPE    recv_spinlock;
} sdio_context_t;

static sdio_context_t context = {
    .intr_handle = NULL,
    /*------- events ---------------*/
    .events     =   {},
    .reg_spinlock = portMUX_INITIALIZER_UNLOCKED,
    /*------- sending ---------------*/
    .send_state     =   STATE_IDLE,
    .sendbuf        =   SDIO_RINGBUF_INITIALIZER(),
    .ret_queue      =   NULL,
    .in_flight      =   NULL,
    .in_flight_end  =   NULL,
    .in_flight_next =   NULL,
    /*------- receiving ---------------*/
    .recv_link_list =   STAILQ_HEAD_INITIALIZER(context.recv_link_list),
    .recv_reg_list  =   TAILQ_HEAD_INITIALIZER(context.recv_reg_list),
    .recv_cur_ret   =   NULL,
    .recv_spinlock  =   portMUX_INITIALIZER_UNLOCKED,
};

static void sdio_intr(void*);
static void sdio_intr_host(void*);
static void sdio_intr_send(void*);
static void sdio_intr_recv(void*);

static esp_err_t send_flush_data(void);
static esp_err_t send_reset_counter(void);
static void recv_flush_data(void);
static void recv_reset_counter(void);

static esp_err_t send_start(void);
static void send_stop(void);
static esp_err_t recv_start(void);
static void recv_stop(void);

static void deinit_context(void);


/**************** Ring buffer for SDIO use *****************/
typedef enum {
    RINGBUF_GET_ONE = 0,
    RINGBUF_GET_ALL = 1,
} ringbuf_get_all_t;

static void sdio_ringbuf_deinit(sdio_ringbuf_t* buf)
{
    if (buf->remain_cnt != NULL) vSemaphoreDelete(buf->remain_cnt);
    if (buf->data != NULL) free(buf->data);
    *buf = SDIO_RINGBUF_INITIALIZER();
}

static esp_err_t sdio_ringbuf_init(sdio_ringbuf_t* buf, int item_size, int item_cnt)
{
    if (buf->data != NULL) {
        SDIO_SLAVE_LOGE("sdio_ringbuf_init: already initialized");
        return ESP_ERR_INVALID_STATE;
    }
    buf->item_size = item_size;
    //one item is not used.
    buf->size = item_size * (item_cnt+1);
    //apply for resources
    buf->data = (uint8_t*)malloc(buf->size);
    if (buf->data == NULL) goto no_mem;
    buf->remain_cnt = xSemaphoreCreateCounting(item_cnt, item_cnt);
    if (buf->remain_cnt == NULL) goto no_mem;
    //initialize pointers
    buf->write_ptr = buf->data;
    buf->read_ptr = buf->data;
    buf->free_ptr = buf->data;
    return ESP_OK;
no_mem:
    sdio_ringbuf_deinit(buf);
    return ESP_ERR_NO_MEM;
}

//calculate a pointer with offset to a original pointer of the specific ringbuffer
static inline uint8_t* sdio_ringbuf_offset_ptr(sdio_ringbuf_t *buf, sdio_ringbuf_pointer_t ptr, uint32_t offset)
{
    uint8_t *buf_ptr = (uint8_t*)*(uint32_t*)(((uint8_t*)buf)+ptr);   //get the specific pointer of the buffer
    uint8_t *offset_ptr=buf_ptr+offset;
    if (offset_ptr >= buf->data + buf->size) offset_ptr -= buf->size;
    return offset_ptr;
}

static esp_err_t sdio_ringbuf_send(sdio_ringbuf_t* buf, esp_err_t (*copy_callback)(uint8_t*, void*), void* arg, TickType_t wait)
{
    portBASE_TYPE ret = xSemaphoreTake(buf->remain_cnt, wait);
    if (ret != pdTRUE) return ESP_ERR_TIMEOUT;

    portENTER_CRITICAL(&buf->write_spinlock);
    uint8_t* get_ptr = sdio_ringbuf_offset_ptr(buf, ringbuf_write_ptr, buf->item_size);
    esp_err_t err = ESP_OK;
    if (copy_callback) (*copy_callback)(get_ptr, arg);
    if (err != ESP_OK) {
        portEXIT_CRITICAL(&buf->write_spinlock);
        return err;
    }
    buf->write_ptr = get_ptr;
    portEXIT_CRITICAL(&buf->write_spinlock);
    return ESP_OK;
}

// this ringbuf is a return-before-recv-again strategy
// since this is designed to be called in the ISR, no parallel logic
static inline esp_err_t sdio_ringbuf_recv(sdio_ringbuf_t* buf, uint8_t **start, uint8_t **end, ringbuf_get_all_t get_all, TickType_t wait)
{
    assert(buf->free_ptr == buf->read_ptr);   //must return before recv again
    assert(wait == 0);                          //only implement wait = 0 case now
    if (start == NULL && end == NULL) return ESP_ERR_INVALID_ARG; // must have a output
    if (buf->read_ptr == buf->write_ptr) return ESP_ERR_NOT_FOUND; // no data

    uint8_t *get_start = sdio_ringbuf_offset_ptr(buf, ringbuf_read_ptr, buf->item_size);

    if (get_all != RINGBUF_GET_ONE) {
        buf->read_ptr = buf->write_ptr;
    } else {
        buf->read_ptr = get_start;
    }

    if (start != NULL) *start = get_start;
    if (end != NULL)   *end = buf->read_ptr;
    return ESP_OK;
}

static inline void sdio_ringbuf_return_from_isr(sdio_ringbuf_t* buf, uint8_t *ptr, portBASE_TYPE *yield)
{
    assert(sdio_ringbuf_offset_ptr(buf, ringbuf_free_ptr, buf->item_size) == ptr);
    int size = (buf->read_ptr + buf->size - buf->free_ptr)%buf->size;
    int count = size/buf->item_size;
    assert(count*buf->item_size==size);
    buf->free_ptr = buf->read_ptr;
    for(int i = 0; i < count; i++) {
        portBASE_TYPE ret = xSemaphoreGiveFromISR(buf->remain_cnt, yield);
        assert(ret == pdTRUE);
    }
}

static inline void sdio_ringbuf_return(sdio_ringbuf_t* buf, uint8_t *ptr)
{
    assert(sdio_ringbuf_offset_ptr(buf, ringbuf_free_ptr, buf->item_size) == ptr);
    int size = (buf->read_ptr + buf->size - buf->free_ptr)%buf->size;
    int count = size/buf->item_size;
    assert(count*buf->item_size==size);
    buf->free_ptr = buf->read_ptr;
    for(int i = 0; i < count; i++) {
        portBASE_TYPE ret = xSemaphoreGive(buf->remain_cnt);
        assert(ret == pdTRUE);
    }
}

static inline uint8_t* sdio_ringbuf_peek_front(sdio_ringbuf_t* buf)
{
    if (buf->read_ptr != buf->write_ptr) {
        return sdio_ringbuf_offset_ptr(buf, ringbuf_read_ptr, buf->item_size);
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
    return (buf->read_ptr == buf->write_ptr? true : false);
}
/**************** End of Ring buffer for SDIO *****************/

static inline void show_queue_item(buf_desc_t *item)
{
    ESP_EARLY_LOGI(TAG, "=> %p: size: %d(%d), eof: %d, owner: %d", item, item->size, item->length, item->eof, item->owner);
    ESP_EARLY_LOGI(TAG, "   buf: %p, stqe_next: %p, tqe-prev: %p", item->buf, item->qe.stqe_next, item->te.tqe_prev);
}

static void __attribute((unused)) dump_queue(buf_stailq_t *queue)
{
    int cnt = 0;
    buf_desc_t *item = NULL;
    ESP_EARLY_LOGI(TAG, ">>>>> first: %p, last: %p <<<<<", queue->stqh_first, queue->stqh_last);
    STAILQ_FOREACH(item, queue, qe) {
        cnt++;
        show_queue_item(item);
    }
    ESP_EARLY_LOGI(TAG, "total: %d", cnt);
}

static inline void show_ll(lldesc_t *item)
{
    ESP_EARLY_LOGI(TAG, "=> %p: size: %d(%d), eof: %d, owner: %d", item, item->size, item->length, item->eof, item->owner);
    ESP_EARLY_LOGI(TAG, "   buf: %p, stqe_next: %p", item->buf, item->qe.stqe_next);
}
static void __attribute((unused)) dump_ll(lldesc_t *queue)
{
    int cnt = 0;
    lldesc_t *item = queue;
    while (item != NULL) {
        cnt++;
        show_ll(item);
        item = STAILQ_NEXT(item, qe);
    }
    ESP_EARLY_LOGI(TAG, "total: %d", cnt);
}

static inline void deinit_context(void)
{
    context.config = (sdio_slave_config_t){};
    for(int i = 0; i < 9; i++) {
        if (context.events[i] != NULL) {
            vSemaphoreDelete(context.events[i]);
            context.events[i] = NULL;
        }
    }
    if (context.ret_queue != NULL) {
        vQueueDelete(context.ret_queue);
        context.ret_queue = NULL;
    }
    sdio_ringbuf_deinit(&context.sendbuf);
}

esp_err_t link_desc_to_last(uint8_t* desc, void* arg)
{
    STAILQ_NEXT((buf_desc_t*)arg, qe) = (buf_desc_t*)desc;
    return ESP_OK;
}

static esp_err_t init_ringbuf(void)
{
    esp_err_t ret = sdio_ringbuf_init(&context.sendbuf, sizeof(buf_desc_t), context.config.send_queue_size);
    if (ret != ESP_OK) return ret;

    esp_err_t rcv_res;
    buf_desc_t *first=NULL, *last=NULL;

    //no copy for the first descriptor
    ret = sdio_ringbuf_send(&context.sendbuf, NULL, NULL, portMAX_DELAY);
    if (ret != ESP_OK) return ret;

    //loop in the ringbuf to link all the desc one after another as a ring
    for (int i = 0; i < context.config.send_queue_size+1; i++) {
        rcv_res = sdio_ringbuf_recv(&context.sendbuf, (uint8_t**)&last, NULL, RINGBUF_GET_ONE, 0);
        assert (rcv_res == ESP_OK);
        ret = sdio_ringbuf_send(&context.sendbuf, link_desc_to_last, last, portMAX_DELAY);
        if (ret != ESP_OK) return ret;
        sdio_ringbuf_return(&context.sendbuf, (uint8_t*)last);
    }
    first = NULL;
    last = NULL;
    //clear the queue
    rcv_res = sdio_ringbuf_recv(&context.sendbuf, (uint8_t**)&first, (uint8_t**)&last, RINGBUF_GET_ALL, 0);
    assert (rcv_res == ESP_OK);
    assert(first == last); //there should be only one desc remain
    sdio_ringbuf_return(&context.sendbuf, (uint8_t*)first);
    return ESP_OK;
}

static esp_err_t init_context(sdio_slave_config_t *config)
{
    SDIO_SLAVE_CHECK(*(uint32_t*)&context.config == 0, "sdio slave already initialized", ESP_ERR_INVALID_STATE);

    context.config = *config;

    // in theory we can queue infinite buffers in the linked list, but for multi-core reason we have to use a queue to
    // count the finished buffers.
    context.recv_event = xSemaphoreCreateCounting(UINT32_MAX, 0);
    for(int i = 0; i < 9; i++) {
        if (i < 8) {
            context.events[i] = xSemaphoreCreateBinary();
        }   //for 8, already created.
        if (context.events[i] == NULL) {
            SDIO_SLAVE_LOGE("event initialize failed");
            goto no_mem;
        }
    }

    esp_err_t ret = init_ringbuf();
    if (ret != ESP_OK) goto no_mem;

    context.ret_queue = xQueueCreate(config->send_queue_size, sizeof(void*));
    if (context.ret_queue == NULL) goto no_mem;

    context.recv_link_list = (buf_stailq_t)STAILQ_HEAD_INITIALIZER(context.recv_link_list);
    context.recv_reg_list  = (buf_tailq_t)TAILQ_HEAD_INITIALIZER(context.recv_reg_list);
    return ESP_OK;

no_mem:
    deinit_context();
    return ESP_ERR_NO_MEM;
}

static void configure_pin(int pin, uint32_t func, bool pullup)
{
    const int sdmmc_func = func;
    const int drive_strength = 3;
    assert(pin!=-1);
    uint32_t reg = GPIO_PIN_MUX_REG[pin];
    assert(reg!=UINT32_MAX);

    PIN_INPUT_ENABLE(reg);
    PIN_FUNC_SELECT(reg, sdmmc_func);
    PIN_SET_DRV(reg, drive_strength);
    gpio_pulldown_dis(pin);
    if (pullup) {
        gpio_pullup_en(pin);
    }
}

static inline esp_err_t sdio_slave_hw_init(sdio_slave_config_t *config)
{
    //enable interrupts
    SLC.slc0_int_ena.val = 0;

    //initialize pin
    const sdio_slave_slot_info_t *slot = &sdio_slave_slot_info[1];

    bool pullup = config->flags & SDIO_SLAVE_FLAG_INTERNAL_PULLUP;
    configure_pin(slot->clk_gpio, slot->func, false);   //clk doesn't need a pullup
    configure_pin(slot->cmd_gpio, slot->func, pullup);
    configure_pin(slot->d0_gpio, slot->func, pullup);
    if ((config->flags & SDIO_SLAVE_FLAG_HOST_INTR_DISABLED)==0) {
        configure_pin(slot->d1_gpio, slot->func, pullup);
    }
    if ((config->flags & SDIO_SLAVE_FLAG_DAT2_DISABLED)==0) {
       configure_pin(slot->d2_gpio, slot->func, pullup);
    }
    configure_pin(slot->d3_gpio, slot->func, pullup);

    //enable module and config
    periph_module_reset(PERIPH_SDIO_SLAVE_MODULE);
    periph_module_enable(PERIPH_SDIO_SLAVE_MODULE);

    SLC.conf0.slc0_rx_auto_wrback = 1;
    SLC.conf0.slc0_token_auto_clr = 0;
    SLC.conf0.slc0_rx_loop_test = 0;
    SLC.conf0.slc0_tx_loop_test = 0;

    SLC.conf1.slc0_rx_stitch_en = 0;
    SLC.conf1.slc0_tx_stitch_en = 0;
    SLC.conf1.slc0_len_auto_clr = 0;

    SLC.rx_dscr_conf.slc0_token_no_replace = 1;
    HINF.cfg_data1.highspeed_enable = 1;
    HINF.cfg_data1.sdio_ver = 0x232;

    switch(config->timing) {
        case SDIO_SLAVE_TIMING_PSEND_PSAMPLE:
            HOST.conf.frc_sdio20 = 0x1f;
            HOST.conf.frc_sdio11 = 0;
            HOST.conf.frc_pos_samp = 0x1f;
            HOST.conf.frc_neg_samp = 0;
            break;
        case SDIO_SLAVE_TIMING_PSEND_NSAMPLE:
            HOST.conf.frc_sdio20 = 0x1f;
            HOST.conf.frc_sdio11 = 0;
            HOST.conf.frc_pos_samp = 0;
            HOST.conf.frc_neg_samp = 0x1f;
            break;
        case SDIO_SLAVE_TIMING_NSEND_PSAMPLE:
            HOST.conf.frc_sdio20 = 0;
            HOST.conf.frc_sdio11 = 0x1f;
            HOST.conf.frc_pos_samp = 0x1f;
            HOST.conf.frc_neg_samp = 0;
            break;
        case SDIO_SLAVE_TIMING_NSEND_NSAMPLE:
            HOST.conf.frc_sdio20 = 0;
            HOST.conf.frc_sdio11 = 0x1f;
            HOST.conf.frc_pos_samp = 0;
            HOST.conf.frc_neg_samp = 0x1f;
            break;
    }

    SLC.slc0_int_ena.frhost_bit0 = 1;
    SLC.slc0_int_ena.frhost_bit1 = 1;
    SLC.slc0_int_ena.frhost_bit2 = 1;
    SLC.slc0_int_ena.frhost_bit3 = 1;
    SLC.slc0_int_ena.frhost_bit4 = 1;
    SLC.slc0_int_ena.frhost_bit5 = 1;
    SLC.slc0_int_ena.frhost_bit6 = 1;
    SLC.slc0_int_ena.frhost_bit7 = 1;

    return ESP_OK;
}

esp_err_t sdio_slave_initialize(sdio_slave_config_t *config)
{
    esp_err_t r;
    intr_handle_t intr_handle = NULL;
    const int flags = 0;
    r = esp_intr_alloc(ETS_SLC0_INTR_SOURCE, flags, sdio_intr, NULL, &intr_handle);
    if (r != ESP_OK) return r;

    r = sdio_slave_hw_init(config);
    if (r != ESP_OK) return r;
    r = init_context(config);
    if (r != ESP_OK) return r;
    context.intr_handle = intr_handle;

    sdio_slave_reset();
    return ESP_OK;
}

void sdio_slave_deinit(void)
{
    esp_err_t ret = esp_intr_free(context.intr_handle);
    assert(ret==ESP_OK);
    context.intr_handle = NULL;
    deinit_context();
}

esp_err_t sdio_slave_start(void)
{
    esp_err_t ret;
    HOST.slc0_int_clr.val = UINT32_MAX;//clear all interrupts
    ret = send_start();
    if (ret != ESP_OK) return ret;
    ret = recv_start();
    if (ret != ESP_OK) return ret;
    HINF.cfg_data1.sdio_ioready1 = 1;   //set IO ready to 1 to allow host to use
    return ESP_OK;
}

esp_err_t sdio_slave_reset(void)
{
    send_flush_data();
    send_reset_counter();
    recv_flush_data();
    recv_reset_counter();
    return ESP_OK;
}

void sdio_slave_stop(void)
{
    HINF.cfg_data1.sdio_ioready1 = 0;   //set IO ready to 1 to stop host from using
    send_stop();
    recv_stop();
}

#define SDIO_SLAVE_SLC_INT_TX_MASK      (SLC_SLC0_TX_ERR_EOF_INT_ST_M | SLC_SLC0_TX_DSCR_EMPTY_INT_ST_M | SLC_SLC0_TX_DSCR_ERR_INT_ST_M | SLC_SLC0_TX_SUC_EOF_INT_ST_M | SLC_SLC0_TX_DONE_INT_ST_M | SLC_SLC0_TX_OVF_INT_ST_M | SLC_SLC0_TX_START_INT_ST_M)
#define SDIO_SLAVE_SLC_INT_RX_MASK      (SLC_SLC0_RX_DSCR_ERR_INT_ST_M | SLC_SLC0_RX_EOF_INT_ST_M | SLC_SLC0_RX_DONE_INT_ST_M | SLC_SLC0_RX_UDF_INT_ST_M | SLC_SLC0_RX_START_INT_ST_M)
#define SDIO_SLAVE_SLC_INT_HOST_MASK    (SLC_FRHOST_BIT7_INT_ST_M | SLC_FRHOST_BIT6_INT_ST_M | SLC_FRHOST_BIT5_INT_ST_M | SLC_FRHOST_BIT4_INT_ST_M | SLC_FRHOST_BIT3_INT_ST_M | SLC_FRHOST_BIT2_INT_ST_M | SLC_FRHOST_BIT1_INT_ST_M | SLC_FRHOST_BIT0_INT_ST_M)

//strange but `tx_*` regs for host->slave transfers while `rx_*` regs for slave->host transfers
static void sdio_intr(void* arg)
{
    uint32_t int_val = SLC.slc0_int_st.val;
    uint32_t int_raw = SLC.slc0_int_raw.val;
    ESP_EARLY_LOGV(TAG, "sdio_intr: %08X(%08X)", int_val, int_raw);

    if (int_val & SDIO_SLAVE_SLC_INT_RX_MASK) sdio_intr_send(arg);
    if (int_val & SDIO_SLAVE_SLC_INT_TX_MASK) sdio_intr_recv(arg);
    if (int_val & SDIO_SLAVE_SLC_INT_HOST_MASK) sdio_intr_host(arg);
}

/*---------------------------------------------------------------------------
 *                  Host
 *--------------------------------------------------------------------------*/
static void sdio_intr_host(void* arg)
{
    uint8_t int_val = SLC.slc0_int_st.val & 0xff;

    portBASE_TYPE yield = pdFALSE;
    SLC.slc0_int_clr.val = int_val;
    for(int i = 0; i < 8; i++) {
        if (BIT(i) & int_val) {
            if (context.config.event_cb != NULL) (*context.config.event_cb)(i);
            xSemaphoreGiveFromISR(context.events[i], &yield);
        }
    }
    if (yield) portYIELD_FROM_ISR();
}

esp_err_t sdio_slave_wait_int(int pos, TickType_t wait)
{
    SDIO_SLAVE_CHECK(pos >= 0 && pos < 8, "interrupt num invalid", ESP_ERR_INVALID_ARG);
    return xSemaphoreTake(context.events[pos], wait);
}


uint8_t sdio_slave_read_reg(int pos)
{
    if (pos >= 28 && pos <= 31) SDIO_SLAVE_LOGW("%s: interrupt reg, for reference", __FUNCTION__);
    if (pos < 0 || pos >= 64) SDIO_SLAVE_LOGE("read register address wrong");

    return *(uint8_t*)(HOST_SLCHOST_CONF_W_REG(pos));
}

esp_err_t sdio_slave_write_reg(int pos, uint8_t reg)
{
    if (pos >= 28 && pos <= 31) {
        SDIO_SLAVE_LOGE("interrupt reg, please use sdio_slave_clear_int");
        return ESP_ERR_INVALID_ARG;
    }
    if (pos < 0 || pos >= 64) {
        SDIO_SLAVE_LOGE("write register address wrong");
        return ESP_ERR_INVALID_ARG;
    }
    uint32_t addr = HOST_SLCHOST_CONF_W_REG(pos) & (~3);
    uint32_t shift = (pos % 4)*8;

    portENTER_CRITICAL(&context.reg_spinlock);
    int val = *(uint32_t*)addr;
    *(uint32_t*)addr = (val & ~(0xff << shift)) | (reg<<shift);
    portEXIT_CRITICAL(&context.reg_spinlock);
    return ESP_OK;
}

sdio_slave_hostint_t sdio_slave_get_host_intena(void)
{
    return HOST.slc0_func1_int_ena.val;
}

void sdio_slave_set_host_intena(sdio_slave_hostint_t ena)
{
    HOST.slc0_func1_int_ena.val = ena;
}

void sdio_slave_clear_host_int(uint8_t mask)
{
    SLC.intvec_tohost.slc0_intvec = mask;
}

esp_err_t sdio_slave_send_host_int(uint8_t pos)
{
    SDIO_SLAVE_CHECK(pos < 8, "interrupt num invalid", ESP_ERR_INVALID_ARG);
    SLC.intvec_tohost.slc0_intvec = BIT(pos);
    return ESP_OK;
}


/*---------------------------------------------------------------------------
 *                  Send
 *--------------------------------------------------------------------------*/
//it's strange but the register is really called 'rx' for slave->host transfers.
/* The link list is handled in the app, while counter and pointer processed in ISR.
 * Driver abuse rx_done bit to invoke ISR.
 * If driver is stopped, the link list is stopped as well as the ISR invoker.
 */
static inline void send_length_write(uint32_t len)
{
    SLC.slc0_len_conf.val = FIELD_TO_VALUE2(SLC_SLC0_LEN_WDATA, len) | FIELD_TO_VALUE2(SLC_SLC0_LEN_WR, 1);
    ESP_EARLY_LOGV(TAG, "send_length_write: %d, last_len: %08X", len, HOST.pkt_len.reg_slc0_len);
}

static inline void send_start_transmission(const void* desc)
{
    //reset to flush previous packets
    SLC.conf0.slc0_rx_rst = 1;
    SLC.conf0.slc0_rx_rst = 0;
    SLC.slc0_rx_link.addr = (uint32_t)desc;
    SLC.slc0_rx_link.start = 1;
}

static inline void send_stop_ll_operation(void)
{
    SLC.slc0_rx_link.stop = 1;
}

static inline uint32_t send_length_read(void)
{
    return HOST.pkt_len.reg_slc0_len;
}

DMA_ATTR static const buf_desc_t start_desc = {
    .owner = 1,
    .buf = (void*)0x3ffbbbbb, //assign a dma-capable pointer other than NULL, which will not be used
    .size = 1,
    .length = 1,
    .eof = 1,
};

static inline void send_isr_invoker_enable(void)
{
    //force trigger rx_done interrupt. the interrupt is abused to invoke ISR from the app by the enable bit and never cleared.
    send_start_transmission(&start_desc);
    //wait for rx_done
    while(!SLC.slc0_int_raw.rx_done);
    HOST.slc0_int_clr.rx_new_packet = 1;
    send_stop_ll_operation();
}

static inline void send_isr_invoker_disable(void)
{
    SLC.slc0_int_clr.rx_done = 1;
}

static inline void send_intr_enable(void)
{
    SLC.slc0_int_ena.rx_eof = 1;
    send_isr_invoker_enable();
}

static inline void send_intr_disable(void)
{
    send_isr_invoker_disable();
    SLC.slc0_int_ena.rx_eof = 0;
}

static inline void send_isr_invoke(void)
{
    SLC.slc0_int_ena.rx_done = 1;
}

static inline send_state_t send_get_state(void)
{
    return context.send_state;
}

static inline void send_set_state(send_state_t state)
{
    context.send_state = state;
}

//start hw operation with existing data (if exist)
static esp_err_t send_start(void)
{
    SDIO_SLAVE_CHECK(send_get_state() == STATE_IDLE,
        "already started", ESP_ERR_INVALID_STATE);
    SLC.slc0_int_clr.rx_eof = 1;
    send_set_state(STATE_WAIT_FOR_START);
    send_intr_enable();
    return ESP_OK;
}

//only stop hw operations, no touch to data as well as counter
static void send_stop(void)
{
    SLC.slc0_rx_link.stop = 1;
    send_intr_disable();

    send_set_state(STATE_IDLE);
}

static inline esp_err_t send_isr_eof(portBASE_TYPE *yield)
{
    // inform app to recycle descs
    portBASE_TYPE ret = pdTRUE;
    buf_desc_t *desc = context.in_flight;
    assert(desc != NULL);

    do {
        ESP_EARLY_LOGV(TAG, "end: %x", desc->arg);
        ret = xQueueSendFromISR(context.ret_queue, &desc->arg, yield);
        assert(ret == pdTRUE);
        buf_desc_t* next = STAILQ_NEXT(desc, qe);
        desc = next;
    } while(desc!=NULL);
    STAILQ_NEXT(context.in_flight_end, qe) = context.in_flight_next;
    sdio_ringbuf_return_from_isr(&context.sendbuf, (uint8_t*)context.in_flight, yield);
    context.in_flight = NULL;
    context.in_flight_end = NULL;
    // Go to wait for packet state
    send_set_state(STATE_WAIT_FOR_START);
    return ESP_OK;
}

static inline esp_err_t send_isr_check_new_pkt(portBASE_TYPE *yield)
{
    esp_err_t ret;
    buf_desc_t *start = NULL;
    buf_desc_t *end = NULL;
    if (context.config.sending_mode == SDIO_SLAVE_SEND_PACKET) {
        ret = sdio_ringbuf_recv(&context.sendbuf, (uint8_t**)&start, (uint8_t**)&end, RINGBUF_GET_ONE, 0);
    } else { //stream mode
        ret = sdio_ringbuf_recv(&context.sendbuf, (uint8_t**)&start, (uint8_t**)&end, RINGBUF_GET_ALL, 0);
    }
    if (ret == ESP_OK) {
        context.in_flight = start;
        context.in_flight_end = end;
        end->eof = 1;
        //temporarily break the link ring here, the ring will be re-connected in ``send_isr_eof()``.
        context.in_flight_next = STAILQ_NEXT(end, qe);
        STAILQ_NEXT(end, qe) = NULL;
    }
    return ESP_OK;
}

static inline esp_err_t send_isr_new_packet(void)
{
    // since eof is changed, we have to stop and reset the link list,
    // and restart new link list operation
    buf_desc_t *const start_desc = context.in_flight;
    buf_desc_t *const end_desc = context.in_flight_end;
    assert(start_desc != NULL && end_desc != NULL);

    send_stop_ll_operation();
    send_start_transmission(start_desc);

    // update pkt_len register to allow host reading.
    send_length_write(end_desc->pkt_len);

    send_set_state(STATE_SENDING);

    ESP_EARLY_LOGD(TAG, "restart new send: %p->%p, pkt_len: %d", start_desc, end_desc, end_desc->pkt_len);
    return ESP_OK;
}

static void sdio_intr_send(void* arg)
{
    ESP_EARLY_LOGV(TAG, "intr_send");
    portBASE_TYPE yield = pdFALSE;

    // this interrupt is abused to get ISR invoked by app
    if (SLC.slc0_int_st.rx_done) SLC.slc0_int_ena.rx_done = 0;

    // Goto idle state (cur_start=NULL) if transmission done,
    // also update sequence and recycle descs.
    if (SLC.slc0_int_st.rx_eof) {
        SLC.slc0_int_clr.rx_eof = 1;
        //check current state
        assert(send_get_state() == STATE_SENDING);// context.send_start != NOT_YET && context.send_end != NOT_YET);
        send_isr_eof(&yield);
    }

    // Go to wait sending state (cur_start!=NULL && cur_end==NULL) if not sending and new packet ready.
    // Note we may also enter this state by stopping sending in the app.
    if (send_get_state() == STATE_WAIT_FOR_START) {
        if (context.in_flight == NULL) send_isr_check_new_pkt(&yield);
        // Go to sending state (cur_start and cur_end != NULL) if has packet to send.
        if (context.in_flight) send_isr_new_packet();
    }

    if (yield) portYIELD_FROM_ISR();
}

esp_err_t send_write_desc(uint8_t* desc, void* arg)
{
    buf_desc_t *new_desc = (buf_desc_t*)arg;
    buf_desc_t *tail = (buf_desc_t*)sdio_ringbuf_peek_rear(&context.sendbuf);
    new_desc->pkt_len = tail->pkt_len + new_desc->size;
    //copy and keep the link
    STAILQ_NEXT(new_desc, qe) = STAILQ_NEXT((buf_desc_t*)desc, qe);

    memcpy(desc, new_desc, sizeof(buf_desc_t));
    return ESP_OK;
}

esp_err_t sdio_slave_send_queue(uint8_t* addr, size_t len, void* arg, TickType_t wait)
{
    SDIO_SLAVE_CHECK(len > 0, "len <= 0", ESP_ERR_INVALID_ARG);
    SDIO_SLAVE_CHECK(esp_ptr_dma_capable(addr) && (uint32_t)addr%4==0, "buffer to send should be DMA capable and 32-bit aligned",
        ESP_ERR_INVALID_ARG);

    buf_desc_t new_desc = {
        .size   =   len,
        .length =   len,
        .buf    =   addr,
        .owner  =   1,
        // in stream mode, the eof is only appended (in ISR) when new packet is ready to be sent
        .eof    =   (context.config.sending_mode == SDIO_SLAVE_SEND_PACKET?1:0),
        .arg    =   arg,
    };

    esp_err_t ret = sdio_ringbuf_send(&context.sendbuf, send_write_desc, &new_desc, wait);
    if (ret != ESP_OK) return ret;

    send_isr_invoke();
    return ESP_OK;
}

esp_err_t sdio_slave_send_get_finished(void** out_arg, TickType_t wait)
{
    void* arg = NULL;
    portBASE_TYPE err = xQueueReceive(context.ret_queue, &arg, wait);
    if (out_arg) *out_arg = arg;
    if (err != pdTRUE) return ESP_ERR_TIMEOUT;
    return ESP_OK;
}

esp_err_t sdio_slave_transmit(uint8_t* addr, size_t len)
{
    uint32_t timestamp = XTHAL_GET_CCOUNT();
    uint32_t ret_stamp;

    esp_err_t err = sdio_slave_send_queue(addr, len, (void*)timestamp, portMAX_DELAY);
    if (err != ESP_OK) return err;
    err = sdio_slave_send_get_finished((void**)&ret_stamp, portMAX_DELAY);
    if (err != ESP_OK) return err;
    SDIO_SLAVE_CHECK(ret_stamp == timestamp, "already sent without return before", ESP_ERR_INVALID_STATE);

    return ESP_OK;
}

//clear data but keep counter
static esp_err_t send_flush_data(void)
{
    //only works in idle state / wait to send state
    SDIO_SLAVE_CHECK(send_get_state() == STATE_IDLE,
        "flush data when transmission started", ESP_ERR_INVALID_STATE);

    HOST.slc0_int_clr.rx_new_packet = 1;

    buf_desc_t *last = NULL;
    if (context.in_flight) {
        buf_desc_t *desc = context.in_flight;
        while (desc != NULL) {
            xQueueSend(context.ret_queue, &desc->arg, portMAX_DELAY);
            last = desc;
            desc = STAILQ_NEXT(desc, qe);
        }
        STAILQ_NEXT(context.in_flight_end, qe) = context.in_flight_next;
        sdio_ringbuf_return(&context.sendbuf, (uint8_t*)context.in_flight);
        context.in_flight = NULL;
        context.in_flight_end = NULL;
    }

    buf_desc_t *head, *tail;
    esp_err_t ret = sdio_ringbuf_recv(&context.sendbuf, (uint8_t**)&head, (uint8_t**)&tail, RINGBUF_GET_ALL, 0);
    if (ret == ESP_OK) {
        buf_desc_t *desc = head;
        while (1) {
            xQueueSend(context.ret_queue, &desc->arg, portMAX_DELAY);
            last = desc;
            if (desc == tail) break;
            desc = STAILQ_NEXT(desc, qe);
        }
        sdio_ringbuf_return(&context.sendbuf, (uint8_t*)head);
    }

    // if in wait to send state, set the sequence number of tail to the value last sent, just as if the packet wait to
    // send never queued.
    // Go to idle state (cur_end!=NULL and cur_start=NULL)
    send_set_state(STATE_IDLE);

    if (last == NULL) last = (buf_desc_t*)sdio_ringbuf_peek_rear(&context.sendbuf);
    last->pkt_len = send_length_read();
    return ESP_OK;
}

//clear counter but keep data
static esp_err_t send_reset_counter(void)
{
    SDIO_SLAVE_CHECK(send_get_state() == STATE_IDLE,
        "reset counter when transmission started", ESP_ERR_INVALID_STATE);

    send_length_write(0);

    uint32_t last_cnt=0;
    buf_desc_t *desc = context.in_flight;
    buf_desc_t *last = NULL;
    while(desc != NULL) {
        last_cnt += desc->length;
        desc->pkt_len = last_cnt;
        last = desc;
        desc = STAILQ_NEXT(desc, qe);
    }
    // in theory the desc should be the one right next to the last of in_flight,
    // but the link of last is NULL, so get the desc from the ringbuf directly.
    desc = (buf_desc_t*)sdio_ringbuf_peek_front(&context.sendbuf);
    while(desc != NULL) {
        last_cnt += desc->length;
        desc->pkt_len = last_cnt;
        last = desc;
        desc = STAILQ_NEXT(desc, qe);
    }
    if (last == NULL) {
        last = (buf_desc_t*)sdio_ringbuf_peek_rear(&context.sendbuf);
        last->pkt_len = 0;
    }

    return ESP_OK;
}


/*---------------------------------------------------------------------------
 *                  Recv
 *--------------------------------------------------------------------------*/
//strange but the registers for host->slave transfers are really called "tx*".

#define CHECK_HANDLE_IDLE(desc) do { if (desc == NULL || !desc->not_receiving) {\
    return ESP_ERR_INVALID_ARG; } } while(0)

static inline void critical_enter_recv(void)
{
    portENTER_CRITICAL(&context.recv_spinlock);
}

static inline void critical_exit_recv(void)
{
    portEXIT_CRITICAL(&context.recv_spinlock);
}

static inline void recv_size_inc(void)
{
    // fields wdata and inc_more should be written by the same instruction.
    SLC.slc0_token1.val = FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WDATA, 1) | FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_INC_MORE, 1);
}

static inline void recv_size_reset(void)
{
    SLC.slc0_token1.val = FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WDATA, 0) | FIELD_TO_VALUE2(SLC_SLC0_TOKEN1_WR, 1);
}

static inline buf_desc_t* recv_get_first_empty_buf(void)
{
    buf_stailq_t *const queue = &context.recv_link_list;
    buf_desc_t *desc = STAILQ_FIRST(queue);
    while(desc && desc->owner == 0) {
        desc = STAILQ_NEXT(desc, qe);
    }
    return desc;
}

static esp_err_t recv_start(void)
{
    SLC.conf0.slc0_tx_rst = 1;
    SLC.conf0.slc0_tx_rst = 0;

    critical_enter_recv();
    buf_desc_t *desc = recv_get_first_empty_buf();
    if (!desc) {
        ESP_LOGD(TAG, "recv: restart without desc");
        critical_exit_recv();
        return ESP_OK; // if no buffer loaded, return directly.
    }
    //the counter is handled when add/flush/reset
    SLC.slc0_tx_link.addr = (uint32_t)desc;
    SLC.slc0_tx_link.start = 1;
    critical_exit_recv();

    SLC.slc0_int_ena.tx_done = 1;
    return ESP_OK;
}

static void recv_stop(void)
{
    SLC.slc0_tx_link.stop = 1;
    SLC.slc0_int_ena.tx_done = 0;
}

// reset the counter, but keep the data
static void recv_reset_counter(void)
{
    recv_size_reset();

    critical_enter_recv();
    buf_desc_t *desc = recv_get_first_empty_buf();
    while (desc != NULL) {
        assert(desc->owner == 1);
        recv_size_inc();
        desc = STAILQ_NEXT(desc, qe);
    }
    critical_exit_recv();
}

// remove data, still increase the counter
static void recv_flush_data(void)
{
    buf_stailq_t *const queue = &context.recv_link_list;

    critical_enter_recv();
    while(1) {
        portBASE_TYPE ret = xSemaphoreTake(context.recv_event, 0);
        if (ret == pdFALSE) break;

        buf_desc_t *desc = STAILQ_FIRST(queue);
        assert (desc != NULL && desc->owner == 0);
        STAILQ_REMOVE_HEAD(queue, qe);
        desc->owner = 1;
        STAILQ_INSERT_TAIL(queue, desc, qe);
        recv_size_inc();
        //we only add it to the tail here, without start the DMA nor increase buffer num.
    }
    critical_exit_recv();
}

static void sdio_intr_recv(void* arg)
{
    portBASE_TYPE yield = 0;
    if (SLC.slc0_int_raw.tx_done) {
        SLC.slc0_int_clr.tx_done = 1;
        while (context.recv_cur_ret && context.recv_cur_ret->owner == 0) {
            // This may cause the ``cur_ret`` pointer to be NULL, indicating the list is empty,
            // in this case the ``tx_done`` should happen no longer until new desc is appended.
            // The app is responsible to place the pointer to the right place again when appending new desc.
            critical_enter_recv();
            context.recv_cur_ret = STAILQ_NEXT(context.recv_cur_ret, qe);
            critical_exit_recv();
            ESP_EARLY_LOGV(TAG, "intr_recv: Give");
            xSemaphoreGiveFromISR(context.recv_event, &yield);
            SLC.slc0_int_clr.tx_done = 1;
        };
    }
    if (yield) portYIELD_FROM_ISR();
}

esp_err_t sdio_slave_recv_load_buf(sdio_slave_buf_handle_t handle)
{
    buf_desc_t *desc = (buf_desc_t*)handle;
    CHECK_HANDLE_IDLE(desc);

    buf_stailq_t *const queue = &context.recv_link_list;

    critical_enter_recv();
    assert(desc->not_receiving);
    TAILQ_REMOVE(&context.recv_reg_list, desc, te);
    desc->owner = 1;
    desc->not_receiving = 0; //manually remove the prev link (by set not_receiving=0), to indicate this is in the queue

    buf_desc_t *const tail = STAILQ_LAST(queue, buf_desc_s, qe);

    STAILQ_INSERT_TAIL(queue, desc, qe);
    if (context.recv_cur_ret == NULL) {
        context.recv_cur_ret = desc;
    }

    if (tail == NULL) {
        //no one in the ll, start new ll operation.
        SLC.slc0_tx_link.addr = (uint32_t)desc;
        SLC.slc0_tx_link.start = 1;
        ESP_LOGV(TAG, "recv_load_buf: start new");
    } else {
        //restart former ll operation
        SLC.slc0_tx_link.restart = 1;
        ESP_LOGV(TAG, "recv_load_buf: restart");
    }
    critical_exit_recv();
    recv_size_inc();

    return ESP_OK;
}

sdio_slave_buf_handle_t sdio_slave_recv_register_buf(uint8_t *start)
{
    SDIO_SLAVE_CHECK(esp_ptr_dma_capable(start) && (uint32_t)start%4==0,
        "buffer to register should be DMA capable and 32-bit aligned", NULL);
    buf_desc_t *desc = (buf_desc_t*)malloc(sizeof(buf_desc_t));
    if (desc == NULL) {
        SDIO_SLAVE_LOGE("cannot allocate lldesc for new buffer");
        return NULL;
    }

    //initially in the reg list
    *desc = (buf_desc_t) {
        .size = context.config.recv_buffer_size,
        .buf = start,
        //no length required, eof always=0
    };
    critical_enter_recv();
    TAILQ_INSERT_TAIL(&context.recv_reg_list, desc, te);
    critical_exit_recv();
    return desc;
}

esp_err_t sdio_slave_recv(sdio_slave_buf_handle_t* handle_ret, uint8_t **out_addr, size_t *out_len, TickType_t wait)
{
    SDIO_SLAVE_CHECK(handle_ret != NULL, "handle address cannot be 0", ESP_ERR_INVALID_ARG);
    portBASE_TYPE ret = xSemaphoreTake(context.recv_event, wait);
    if (ret == pdFALSE) return ESP_ERR_TIMEOUT;

    buf_stailq_t *const queue = &context.recv_link_list;

    critical_enter_recv();
    //remove from queue, add back to reg list.
    buf_desc_t *desc = STAILQ_FIRST(queue);
    STAILQ_REMOVE_HEAD(queue, qe);
    TAILQ_INSERT_TAIL(&context.recv_reg_list, desc, te);
    critical_exit_recv();

    assert(desc != NULL && desc->owner == 0);
    *handle_ret = (sdio_slave_buf_handle_t)desc;
    if (out_addr) *out_addr = desc->buf;
    if (out_len) *out_len = desc->length;
    return ESP_OK;
}

esp_err_t sdio_slave_recv_unregister_buf(sdio_slave_buf_handle_t handle)
{
    buf_desc_t *desc = (buf_desc_t*)handle;
    CHECK_HANDLE_IDLE(desc); //in the queue, fail.

    critical_enter_recv();
    TAILQ_REMOVE(&context.recv_reg_list, desc, te);
    critical_exit_recv();
    free(desc);
    return ESP_OK;
}

uint8_t* sdio_slave_recv_get_buf(sdio_slave_buf_handle_t handle, size_t *len_o)
{
    buf_desc_t *desc = (buf_desc_t*)handle;
    if (handle == NULL) return NULL;

    if (len_o!= NULL) *len_o= desc->length;
    return desc->buf;
}

static void __attribute((unused)) sdio_slave_recv_get_loaded_buffer_num(void)
{
    buf_stailq_t *const queue = &context.recv_link_list;
    dump_queue(queue);
}
