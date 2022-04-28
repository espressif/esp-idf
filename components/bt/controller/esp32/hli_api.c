/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "xtensa/core-macros.h"
#include "soc/dport_reg.h"
#include "hli_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#if CONFIG_BTDM_CTRL_HLI
#define HLI_MAX_HANDLERS    4

typedef struct {
    intr_handler_t handler;
    void* arg;
    uint32_t intr_reg;
    uint32_t intr_mask;
} hli_handler_info_t;

typedef struct {
#define CUSTOMER_TYPE_REQUEST (0)
#define CUSTOMER_TYPE_RELEASE (1)
    struct {
        uint32_t cb_type;
        union {
            int (* request)(uint32_t, uint32_t, uint32_t);
            int (* release)(uint32_t);
        } cb;
    } customer_cb;
    uint32_t arg0, arg1, arg2;
} customer_swisr_t;

static void IRAM_ATTR customer_swisr_handle(customer_swisr_t *cus_swisr)
{
    if (cus_swisr->customer_cb.cb_type == CUSTOMER_TYPE_REQUEST) {
        if (cus_swisr->customer_cb.cb.request != NULL) {
            cus_swisr->customer_cb.cb.request(cus_swisr->arg0, cus_swisr->arg1, cus_swisr->arg2);
        }
    } else if(cus_swisr->customer_cb.cb_type == CUSTOMER_TYPE_RELEASE) {
        if (cus_swisr->customer_cb.cb.release != NULL) {
            cus_swisr->customer_cb.cb.release(cus_swisr->arg0);
        }
    }
}

static DRAM_ATTR hli_handler_info_t s_hli_handlers[HLI_MAX_HANDLERS];

esp_err_t hli_intr_register(intr_handler_t handler, void* arg, uint32_t intr_reg, uint32_t intr_mask)
{
    for (hli_handler_info_t* hip = s_hli_handlers;
         hip < s_hli_handlers + HLI_MAX_HANDLERS;
         ++hip) {
        if (hip->handler == NULL) {
            hip->arg = arg;
            hip->intr_reg = intr_reg;
            hip->intr_mask = intr_mask;
            hip->handler = handler;     /* set last, indicates the entry as valid */
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

void IRAM_ATTR hli_c_handler(void)
{
    bool handled = false;
    /* Iterate over registered interrupt handlers,
     * and check if the expected mask is present in the interrupt status register.
     */
    for (hli_handler_info_t* hip = s_hli_handlers;
         hip < s_hli_handlers + HLI_MAX_HANDLERS;
         ++hip) {
        if (hip->handler == NULL) {
            continue;
        }
        uint32_t reg = hip->intr_reg;
        uint32_t val;
        if (reg == 0) { /* special case for CPU internal interrupts */
            val = XTHAL_GET_INTERRUPT();
        } else {
            /* "reg" might not be in DPORT, but this will work in any case */
            val = DPORT_REG_READ(reg);
        }
        if ((val & hip->intr_mask) != 0) {
            handled = true;
            (*hip->handler)(hip->arg);
        }
    }
    if (!handled) {
        /* no handler found, it is OK in this case. */
    }
}

uint32_t IRAM_ATTR hli_intr_disable(void)
{
    /* disable level 4 and below */
    return XTOS_SET_INTLEVEL(XCHAL_DEBUGLEVEL - 2);
}

void IRAM_ATTR hli_intr_restore(uint32_t state)
{
    XTOS_RESTORE_JUST_INTLEVEL(state);
}

#define HLI_META_QUEUE_SIZE     16
#define HLI_QUEUE_MAX_ELEM_SIZE 32
#define HLI_QUEUE_SW_INT_NUM    29

#define HLI_QUEUE_FLAG_SEMAPHORE    BIT(0)
#define HLI_QUEUE_FLAG_CUSTOMER     BIT(1)

static DRAM_ATTR struct hli_queue_t *s_meta_queue_ptr = NULL;
static intr_handle_t ret_handle;

static inline char* IRAM_ATTR wrap_ptr(hli_queue_handle_t queue, char *ptr)
{
    return (ptr == queue->bufend) ? queue->buf : ptr;
}

static inline bool IRAM_ATTR queue_empty(hli_queue_handle_t queue)
{
    return queue->begin == queue->end;
}

static inline bool IRAM_ATTR queue_full(hli_queue_handle_t queue)
{
    return wrap_ptr(queue, queue->end + queue->elem_size) == queue->begin;
}

static void IRAM_ATTR queue_isr_handler(void* arg)
{
    int do_yield = pdFALSE;
    XTHAL_SET_INTCLEAR(BIT(HLI_QUEUE_SW_INT_NUM));
    hli_queue_handle_t queue;

    while (hli_queue_get(s_meta_queue_ptr, &queue)) {
        static DRAM_ATTR char scratch[HLI_QUEUE_MAX_ELEM_SIZE];
        while (hli_queue_get(queue, scratch)) {
            int res = pdPASS;
            if ((queue->flags & HLI_QUEUE_FLAG_CUSTOMER) != 0) {
                customer_swisr_handle((customer_swisr_t *)scratch);
            } else if ((queue->flags & HLI_QUEUE_FLAG_SEMAPHORE) != 0) {
                res = xSemaphoreGiveFromISR((SemaphoreHandle_t) queue->downstream, &do_yield);
            } else {
                res = xQueueSendFromISR(queue->downstream, scratch, &do_yield);
            }
            if (res == pdFAIL) {
                /* Failed to send to downstream queue, it is OK in this case. */
            }
        }
    }
    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

/* Notify the level 3 handler that an element is added to the given hli queue.
 * Do this by placing the queue handle onto s_meta_queue, and raising a SW interrupt.
 *
 * This function must be called with HL interrupts disabled!
 */
static void IRAM_ATTR queue_signal(hli_queue_handle_t queue)
{
    /* See if the queue is already in s_meta_queue, before adding */
    bool found = false;
    const hli_queue_handle_t *end = (hli_queue_handle_t*) s_meta_queue_ptr->end;
    hli_queue_handle_t *item = (hli_queue_handle_t*) s_meta_queue_ptr->begin;
    for (;item != end; item = (hli_queue_handle_t*) wrap_ptr(s_meta_queue_ptr, (char*) (item + 1))) {
        if (*item == queue) {
            found = true;
            break;
        }
    }
    if (!found) {
        bool res = hli_queue_put(s_meta_queue_ptr, &queue);
        if (!res) {
            esp_rom_printf(DRAM_STR("Fatal error in queue_signal: s_meta_queue full\n"));
            abort();
        }
        XTHAL_SET_INTSET(BIT(HLI_QUEUE_SW_INT_NUM));
    }
}

static void queue_init(hli_queue_handle_t queue, size_t buf_size, size_t elem_size, QueueHandle_t downstream)
{
    queue->elem_size = elem_size;
    queue->begin = queue->buf;
    queue->end = queue->buf;
    queue->bufend = queue->buf + buf_size;
    queue->downstream = downstream;
    queue->flags = 0;
}

void hli_queue_setup(void)
{
    if (s_meta_queue_ptr == NULL) {
        s_meta_queue_ptr = hli_queue_create(HLI_META_QUEUE_SIZE, sizeof(void*), NULL);
        ESP_ERROR_CHECK(esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, ESP_INTR_FLAG_IRAM, queue_isr_handler, NULL, &ret_handle));
        xt_ints_on(BIT(HLI_QUEUE_SW_INT_NUM));
    }
}

void hli_queue_shutdown(void)
{
    if (s_meta_queue_ptr != NULL) {
        hli_queue_delete(s_meta_queue_ptr);
        s_meta_queue_ptr = NULL;
        esp_intr_free(ret_handle);
        xt_ints_off(BIT(HLI_QUEUE_SW_INT_NUM));
    }
}

hli_queue_handle_t hli_queue_create(size_t nelem, size_t elem_size, QueueHandle_t downstream)
{
    const size_t buf_elem = nelem + 1;
    if (elem_size > HLI_QUEUE_MAX_ELEM_SIZE) {
        return NULL;
    }
    size_t buf_size = buf_elem * elem_size;
    hli_queue_handle_t res = (hli_queue_handle_t) heap_caps_malloc(sizeof(struct hli_queue_t) + buf_size,
        MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (res == NULL) {
        return NULL;
    }
    queue_init(res, buf_size, elem_size, downstream);
    return res;
}

hli_queue_handle_t hli_customer_queue_create(size_t nelem, size_t elem_size, QueueHandle_t downstream)
{
    hli_queue_handle_t res = hli_queue_create(nelem, elem_size, (QueueHandle_t) downstream);
    if (res == NULL) {
        return NULL;
    }
    res->flags |= HLI_QUEUE_FLAG_CUSTOMER;
    return res;
}

hli_queue_handle_t hli_semaphore_create(size_t max_count, SemaphoreHandle_t downstream)
{
    const size_t elem_size = 1;
    hli_queue_handle_t res = hli_queue_create(max_count, elem_size, (QueueHandle_t) downstream);
    if (res == NULL) {
        return NULL;
    }
    res->flags |= HLI_QUEUE_FLAG_SEMAPHORE;
    return res;
}

void hli_queue_delete(hli_queue_handle_t queue)
{
    free(queue);
}

bool IRAM_ATTR hli_queue_get(hli_queue_handle_t queue, void* out)
{
    uint32_t int_state = hli_intr_disable();
    bool res = false;
    if (!queue_empty(queue)) {
        memcpy(out, queue->begin, queue->elem_size);
        queue->begin = wrap_ptr(queue, queue->begin + queue->elem_size);
        res = true;
    }
    hli_intr_restore(int_state);
    return res;
}

bool IRAM_ATTR hli_queue_put(hli_queue_handle_t queue, const void* data)
{
    uint32_t int_state = hli_intr_disable();
    bool res = false;
    bool was_empty = queue_empty(queue);
    if (!queue_full(queue)) {
        memcpy(queue->end, data, queue->elem_size);
        queue->end = wrap_ptr(queue, queue->end + queue->elem_size);
        if (was_empty && queue != s_meta_queue_ptr) {
            queue_signal(queue);
        }
        res = true;
    }
    hli_intr_restore(int_state);
    return res;
}

bool IRAM_ATTR hli_semaphore_give(hli_queue_handle_t queue)
{
    uint8_t data = 0;
    return hli_queue_put(queue, &data);
}

#endif /* CONFIG_BTDM_CTRL_HLI */
