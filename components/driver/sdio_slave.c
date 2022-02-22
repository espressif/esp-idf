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
interrupts and shared registers), the sending FIFO and the receiving FIFO. A document ``esp_slave_protocol.rst``
describes the functionality of the peripheral detailedly.
The host can access only one of those parts at once, and the hardware functions of these parts are totally
independent. Hence this driver is designed into these three independent parts. The shared registers are quite
simple. As well as the interrupts: when a slave interrupt is written by the host, the slave gets an interrupt;
when one of the host interrupt bits is active, slave hardware output interrupt signals on the DAT1 line.

For the FIFOs, the peripheral provides counters as registers so that the host can always know whether the slave
is ready to send/receive data. The driver resets the counters during initialization, and the host should somehow
inform the slave to reset the counters again if it should reboot (or lose the counter value for some reasons).
Then the host can read/write the FIFOs by CMD53 commands according to the counters.

Since we don't want to copy all the data from the buffer each time we use sending/receiving buffer,
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
    and the counter of receiving buffers is increased so that the host will know this by the receiving interrupt.
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
#include "hal/sdio_slave_hal.h"


#define SDIO_SLAVE_CHECK(res, str, ret_val) do { if(!(res)){\
    SDIO_SLAVE_LOGE("%s", str);\
    return ret_val;\
} }while (0)

static const char TAG[] = "sdio_slave";

#define SDIO_SLAVE_LOGE(s, ...) ESP_LOGE(TAG, "%s:%d (%s):"s, __FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__)
#define SDIO_SLAVE_LOGW(s, ...) ESP_LOGW(TAG, "%s: "s, __FUNCTION__,##__VA_ARGS__)


// sdio_slave_buf_handle_t is of type recv_desc_t*;
typedef struct recv_desc_s{
    union {
        struct {
            // the third word, pointer to next desc, is shared with the tailq entry.
            sdio_slave_hal_recv_desc_t hal_desc;
            // when the forth word is used (not NULL), means the tailq is used, not in the receiving state.
            uint32_t not_receiving;
        };
        struct {
            // first 3 WORDs of this struct is defined by and compatible to the DMA link list format.
            uint32_t _reserved0;
            uint32_t _reserved1;
            TAILQ_ENTRY(recv_desc_s) te; // tailq used to store the registered descriptors.
        };
    };
} recv_desc_t;


typedef TAILQ_HEAD(recv_tailq_head_s, recv_desc_s) recv_tailq_t;

typedef struct {
    sdio_slave_config_t     config;
    sdio_slave_context_t    *hal;
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
    SemaphoreHandle_t   remain_cnt;
    portMUX_TYPE        write_spinlock;
    QueueHandle_t       ret_queue;
    /*------- receiving ---------------*/
    recv_tailq_t        recv_reg_list;  // removed from the link list, registered but not used now
    portMUX_TYPE        recv_spinlock;
} sdio_context_t;

#define CONTEXT_INIT_VAL { \
    .intr_handle = NULL, \
    .hal = NULL, \
    /*------- events ---------------*/ \
    .events     =   {}, \
    .reg_spinlock = portMUX_INITIALIZER_UNLOCKED, \
    /*------- sending ---------------*/ \
    .ret_queue      =   NULL, \
    .write_spinlock =   portMUX_INITIALIZER_UNLOCKED, \
    /*------- receiving ---------------*/ \
    .recv_reg_list  =   TAILQ_HEAD_INITIALIZER(context.recv_reg_list), \
    .recv_spinlock  =   portMUX_INITIALIZER_UNLOCKED, \
}

static sdio_context_t context = CONTEXT_INIT_VAL;

static void sdio_intr(void*);
static void sdio_intr_host(void*);
static void sdio_intr_send(void*);
static void sdio_intr_recv(void*);

static esp_err_t send_flush_data(void);
static esp_err_t recv_flush_data(void);

static inline void critical_enter_recv(void);
static inline void critical_exit_recv(void);

static void deinit_context(void);

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
    if (context.remain_cnt != NULL) vSemaphoreDelete(context.remain_cnt);
    free(context.hal->send_desc_queue.data);
    context.hal->send_desc_queue.data = NULL;
    free(context.hal);
    context.hal = NULL;
}

static esp_err_t init_context(const sdio_slave_config_t *config)
{
    SDIO_SLAVE_CHECK(*(uint32_t*)&context.config == 0, "sdio slave already initialized", ESP_ERR_INVALID_STATE);
    context = (sdio_context_t)CONTEXT_INIT_VAL;
    context.config = *config;

    //initialize and configure the HAL
    context.hal = (sdio_slave_context_t*)heap_caps_calloc(sizeof(sdio_slave_context_t), 1, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (context.hal == NULL) goto no_mem;

    context.hal->sending_mode = config->sending_mode;
    context.hal->timing = config->timing;
    context.hal->send_queue_size = config->send_queue_size;
    context.hal->recv_buffer_size = config->recv_buffer_size;
    //initialize ringbuffer resources
    sdio_ringbuf_t *buf = &(context.hal->send_desc_queue);
    //one item is not used.
    buf->size = SDIO_SLAVE_SEND_DESC_SIZE * (config->send_queue_size+1);
    buf->data = (uint8_t*)heap_caps_malloc(buf->size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    if (buf->data == NULL) goto no_mem;

    sdio_slave_hal_init(context.hal);

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

    context.remain_cnt = xSemaphoreCreateCounting(context.config.send_queue_size, context.config.send_queue_size);
    if (context.remain_cnt == NULL) goto no_mem;

    context.ret_queue = xQueueCreate(config->send_queue_size, sizeof(void*));
    if (context.ret_queue == NULL) goto no_mem;

    return ESP_OK;

no_mem:
    deinit_context();
    return ESP_ERR_NO_MEM;
}

static void configure_pin(int pin, uint32_t func, bool pullup)
{
    const int sdmmc_func = func;
    const int drive_strength = 3;
    assert(pin != -1);
    uint32_t reg = GPIO_PIN_MUX_REG[pin];
    assert(reg != UINT32_MAX);

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

    sdio_slave_hal_hw_init(context.hal);

    return ESP_OK;
}

static void recover_pin(int pin, int sdio_func)
{
    uint32_t reg = GPIO_PIN_MUX_REG[pin];
    assert(reg != UINT32_MAX);

    int func = REG_GET_FIELD(reg, MCU_SEL);
    if (func == sdio_func) {
        gpio_set_direction(pin, GPIO_MODE_INPUT);
        PIN_FUNC_SELECT(reg, PIN_FUNC_GPIO);
    }
}

static void sdio_slave_hw_deinit(void)
{
    const sdio_slave_slot_info_t *slot = &sdio_slave_slot_info[1];
    recover_pin(slot->clk_gpio, slot->func);
    recover_pin(slot->cmd_gpio, slot->func);
    recover_pin(slot->d0_gpio, slot->func);
    recover_pin(slot->d1_gpio, slot->func);
    recover_pin(slot->d2_gpio, slot->func);
    recover_pin(slot->d3_gpio, slot->func);
}

esp_err_t sdio_slave_initialize(sdio_slave_config_t *config)
{
    esp_err_t r;
    intr_handle_t intr_handle = NULL;
    const int flags = 0;
    r = esp_intr_alloc(ETS_SLC0_INTR_SOURCE, flags, sdio_intr, NULL, &intr_handle);
    if (r != ESP_OK) return r;

    r = init_context(config);
    if (r != ESP_OK) return r;
    context.intr_handle = intr_handle;

    r = sdio_slave_hw_init(config);
    if (r != ESP_OK) return r;

    sdio_slave_reset();
    return ESP_OK;
}

void sdio_slave_deinit(void)
{
    sdio_slave_hw_deinit();

    //unregister all buffers registered but returned (not loaded)
    recv_desc_t *temp_desc;
    recv_desc_t *desc;
    TAILQ_FOREACH_SAFE(desc, &context.recv_reg_list, te, temp_desc) {
        TAILQ_REMOVE(&context.recv_reg_list, desc, te);
        free(desc);
    }
    //unregister all buffers that is loaded and not returned
    while (1) {
        desc = (recv_desc_t*)sdio_slave_hal_recv_unload_desc(context.hal);
        if (desc == NULL) break;
        free(desc);
    }
    esp_err_t ret = esp_intr_free(context.intr_handle);
    assert(ret==ESP_OK);
    context.intr_handle = NULL;
    deinit_context();
}

esp_err_t sdio_slave_start(void)
{
    esp_err_t ret;
    sdio_slave_hostint_t intr = (sdio_slave_hostint_t)UINT32_MAX;
    sdio_slave_hal_hostint_clear(context.hal, &intr);
    ret = sdio_slave_hal_send_start(context.hal);
    if (ret != ESP_OK) return ret;

    critical_enter_recv();
    sdio_slave_hal_recv_start(context.hal);
    critical_exit_recv();
    ret = ESP_OK;
    if (ret != ESP_OK) return ret;

    sdio_slave_hal_set_ioready(context.hal, true);
    return ESP_OK;
}

esp_err_t sdio_slave_reset(void)
{
    esp_err_t err;
    err = send_flush_data();
    if (err != ESP_OK) {
        return err;
    }

    err = sdio_slave_hal_send_reset_counter(context.hal);
    if (err != ESP_OK) {
        return err;
    }

    err = recv_flush_data();
    if (err != ESP_OK) {
        return err;
    }

    critical_enter_recv();
    sdio_slave_hal_recv_reset_counter(context.hal);
    critical_exit_recv();
    err = ESP_OK;
    return err;
}

void sdio_slave_stop(void)
{
    sdio_slave_hal_set_ioready(context.hal, false);
    sdio_slave_hal_send_stop(context.hal);
    sdio_slave_hal_recv_stop(context.hal);
}

static void sdio_intr(void* arg)
{
    sdio_intr_send(arg);
    sdio_intr_recv(arg);
    sdio_intr_host(arg);
}

/*---------------------------------------------------------------------------
 *                  Host
 *--------------------------------------------------------------------------*/
static void sdio_intr_host(void* arg)
{
    sdio_slave_ll_slvint_t int_val;
    sdio_slave_hal_slvint_fetch_clear(context.hal, &int_val);
    portBASE_TYPE yield = pdFALSE;
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
    return sdio_slave_hal_host_get_reg(context.hal, pos);
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

    portENTER_CRITICAL(&context.reg_spinlock);
    sdio_slave_hal_host_set_reg(context.hal, pos, reg);
    portEXIT_CRITICAL(&context.reg_spinlock);
    return ESP_OK;
}

sdio_slave_hostint_t sdio_slave_get_host_intena(void)
{
    sdio_slave_hostint_t host_int;
    sdio_slave_hal_hostint_get_ena(context.hal, &host_int);
    return host_int;
}

void sdio_slave_set_host_intena(sdio_slave_hostint_t mask)
{
    sdio_slave_hal_hostint_set_ena(context.hal, &mask);
}

void sdio_slave_clear_host_int(sdio_slave_hostint_t mask)
{
    sdio_slave_hal_hostint_clear(context.hal, &mask);
}

static inline sdio_slave_hostint_t get_hostint_by_pos(int pos)
{
    return (sdio_slave_hostint_t)BIT(pos);
}

esp_err_t sdio_slave_send_host_int(uint8_t pos)
{
    SDIO_SLAVE_CHECK(pos < 8, "interrupt num invalid", ESP_ERR_INVALID_ARG);
    sdio_slave_hostint_t intr = get_hostint_by_pos(pos);
    sdio_slave_hal_hostint_send(context.hal, &intr);
    return ESP_OK;
}

/*---------------------------------------------------------------------------
 *                  Send
 *--------------------------------------------------------------------------*/

/* The link list is handled in the app, while counter and pointer processed in ISR.
 * Driver abuse rx_done bit to invoke ISR.
 * If driver is stopped, the link list is stopped as well as the ISR invoker.
 */

static void sdio_intr_send(void* arg)
{
    ESP_EARLY_LOGV(TAG, "intr_send");
    portBASE_TYPE yield = pdFALSE;

    // this interrupt is abused to get ISR invoked by app
    sdio_slave_hal_send_handle_isr_invoke(context.hal);

    uint32_t returned_cnt;
    if (sdio_slave_hal_send_eof_happened(context.hal)) {
        portBASE_TYPE ret = pdTRUE;

        esp_err_t err;
        while (1) {
            void *finished_arg;
            err = sdio_slave_hal_send_get_next_finished_arg(context.hal, &finished_arg, &returned_cnt);
            if (err != ESP_OK) {
                break;
            }

            assert(returned_cnt == 0);
            ESP_EARLY_LOGV(TAG, "end: %x", finished_arg);
            ret = xQueueSendFromISR(context.ret_queue, &finished_arg, &yield);
            assert(ret == pdTRUE);
        }
        //get_next_finished_arg returns the total amount of returned descs.
        for(int i = 0; i < returned_cnt; i++) {
            portBASE_TYPE ret = xSemaphoreGiveFromISR(context.remain_cnt, &yield);
            assert(ret == pdTRUE);
        }
    }

    sdio_slave_hal_send_new_packet_if_exist(context.hal);

    if (yield) portYIELD_FROM_ISR();
}

esp_err_t sdio_slave_send_queue(uint8_t* addr, size_t len, void* arg, TickType_t wait)
{
    SDIO_SLAVE_CHECK(len > 0, "len <= 0", ESP_ERR_INVALID_ARG);
    SDIO_SLAVE_CHECK(esp_ptr_dma_capable(addr) && (uint32_t)addr%4==0, "buffer to send should be DMA capable and 32-bit aligned",
        ESP_ERR_INVALID_ARG);

    portBASE_TYPE cnt_ret = xSemaphoreTake(context.remain_cnt, wait);
    if (cnt_ret != pdTRUE) return ESP_ERR_TIMEOUT;

    portENTER_CRITICAL(&context.write_spinlock);
    esp_err_t ret = sdio_slave_hal_send_queue(context.hal, addr, len, arg);
    portEXIT_CRITICAL(&context.write_spinlock);
    if (ret != ESP_OK) return ret;

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
    esp_err_t err;

    while (1) {
        void *finished_arg;
        uint32_t return_cnt = 0;
        err = sdio_slave_hal_send_flush_next_buffer(context.hal, &finished_arg, &return_cnt);
        if (err == ESP_OK) {
            portBASE_TYPE ret = xQueueSend(context.ret_queue, &finished_arg, portMAX_DELAY);
            assert(ret == pdTRUE);
            for (int i = 0; i < return_cnt; i++) {
                portBASE_TYPE ret = xSemaphoreGive(context.remain_cnt);
                assert(ret == pdTRUE);
            }
        } else {
            if (err == ESP_ERR_NOT_FOUND) {
                err = ESP_OK;
            }
            break;
        }
    }

    if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "flush data when transmission started");
    }
    return err;
}

/*---------------------------------------------------------------------------
 *                  Recv
 *--------------------------------------------------------------------------*/
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

// remove data, still increase the counter
static esp_err_t recv_flush_data(void)
{
    while(1) {
        portBASE_TYPE ret = xSemaphoreTake(context.recv_event, 0);
        if (ret == pdFALSE) break;
        critical_enter_recv();
        sdio_slave_hal_recv_flush_one_buffer(context.hal);
        critical_exit_recv();
    }
    return ESP_OK;
}

static void sdio_intr_recv(void* arg)
{
    portBASE_TYPE yield = 0;
    bool triggered = sdio_slave_hal_recv_done(context.hal);
    while (triggered) {
        portENTER_CRITICAL_ISR(&context.recv_spinlock);
        bool has_next_item = sdio_slave_hal_recv_has_next_item(context.hal);
        portEXIT_CRITICAL_ISR(&context.recv_spinlock);
        if (has_next_item) {
            ESP_EARLY_LOGV(TAG, "intr_recv: Give");
            xSemaphoreGiveFromISR(context.recv_event, &yield);
            continue;   //check the linked list again skip the interrupt checking
        }
        // if no more items on the list, check the interrupt again,
        // will loop until the interrupt bit is kept cleared.
        triggered = sdio_slave_hal_recv_done(context.hal);
    }
    if (yield) portYIELD_FROM_ISR();
}

esp_err_t sdio_slave_recv_load_buf(sdio_slave_buf_handle_t handle)
{
    recv_desc_t *desc = (recv_desc_t*)handle;
    CHECK_HANDLE_IDLE(desc);
    assert(desc->not_receiving);

    critical_enter_recv();
    TAILQ_REMOVE(&context.recv_reg_list, desc, te);
    desc->not_receiving = 0; //manually remove the prev link (by set not_receiving=0), to indicate this is in the queue
    sdio_slave_hal_load_buf(context.hal, &desc->hal_desc);
    critical_exit_recv();
    return ESP_OK;
}

sdio_slave_buf_handle_t sdio_slave_recv_register_buf(uint8_t *start)
{
    SDIO_SLAVE_CHECK(esp_ptr_dma_capable(start) && (uint32_t)start%4==0,
        "buffer to register should be DMA capable and 32-bit aligned", NULL);
    recv_desc_t *desc = (recv_desc_t*)heap_caps_malloc(sizeof(recv_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    if (desc == NULL) {
        SDIO_SLAVE_LOGE("cannot allocate lldesc for new buffer");
        return NULL;
    }

    //initially in the reg list
    sdio_slave_hal_recv_init_desc(context.hal, &desc->hal_desc, start);
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

    critical_enter_recv();
    //remove from queue, add back to reg list.
    recv_desc_t *desc = (recv_desc_t*)sdio_slave_hal_recv_unload_desc(context.hal);
    assert(desc != NULL && desc->hal_desc.owner == 0);
    TAILQ_INSERT_TAIL(&context.recv_reg_list, desc, te);
    critical_exit_recv();

    *handle_ret = (sdio_slave_buf_handle_t)desc;
    if (out_addr) *out_addr = (uint8_t*)desc->hal_desc.buf;
    if (out_len) *out_len = desc->hal_desc.length;
    return ESP_OK;
}

esp_err_t sdio_slave_recv_unregister_buf(sdio_slave_buf_handle_t handle)
{
    recv_desc_t *desc = (recv_desc_t*)handle;
    CHECK_HANDLE_IDLE(desc); //in the queue, fail.

    critical_enter_recv();
    TAILQ_REMOVE(&context.recv_reg_list, desc, te);
    critical_exit_recv();
    free(desc);
    return ESP_OK;
}

uint8_t* sdio_slave_recv_get_buf(sdio_slave_buf_handle_t handle, size_t *len_o)
{
    if (handle == NULL) return NULL;
    recv_desc_t *desc = (recv_desc_t*)handle;

    if (len_o!= NULL) *len_o= desc->hal_desc.length;
    return (uint8_t*)desc->hal_desc.buf;
}