// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "xtensa/core-macros.h"
#include "soc/dport_reg.h"
#include "hli_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


#define HLI_MAX_HANDLERS    4

typedef struct {
    intr_handler_t handler;
    void* arg;
    uint32_t intr_reg;
    uint32_t intr_mask;
} hli_handler_info_t;

static hli_handler_info_t s_hli_handlers[HLI_MAX_HANDLERS];
static const char* TAG = "hli_queue";

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
        ets_printf(DRAM_STR("hli_c_handler: no handler found!\n"));
        abort();
    }
}

uint32_t hli_intr_disable(void)
{
    // disable level 4 and below
    return XTOS_SET_INTLEVEL(XCHAL_DEBUGLEVEL - 2);
}

void hli_intr_restore(uint32_t state)
{
    XTOS_RESTORE_JUST_INTLEVEL(state);
}

#define HLI_META_QUEUE_SIZE     16
#define HLI_QUEUE_MAX_ELEM_SIZE 32
#define HLI_QUEUE_SW_INT_NUM    29

#define HLI_QUEUE_FLAG_SEMAPHORE    BIT(0)

struct hli_queue_t s_meta_queue;

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
    while (hli_queue_get(&s_meta_queue, &queue)) {
        static char scratch[HLI_QUEUE_MAX_ELEM_SIZE];
        while (hli_queue_get(queue, scratch)) {
            int res = pdPASS;
            if ((queue->flags & HLI_QUEUE_FLAG_SEMAPHORE) != 0) {
                res = xSemaphoreGiveFromISR((SemaphoreHandle_t) queue->downstream, &do_yield);
            } else {
                res = xQueueSendFromISR(queue->downstream, scratch, &do_yield);
            }
            if (res == pdFAIL) {
                ESP_EARLY_LOGE(TAG, "Failed to send to queue %p", queue->downstream);
            }
        }
    }
    if (do_yield) {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR queue_signal(hli_queue_handle_t queue)
{
    bool res = hli_queue_put(&s_meta_queue, &queue);
    if (!res) {
        abort();
    }
    XTHAL_SET_INTSET(BIT(HLI_QUEUE_SW_INT_NUM));
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
    queue_init(&s_meta_queue, HLI_META_QUEUE_SIZE * sizeof(void*), sizeof(void*), NULL);
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_INTERNAL_SW1_INTR_SOURCE, ESP_INTR_FLAG_IRAM, queue_isr_handler, NULL, NULL));
    xt_ints_on(BIT(HLI_QUEUE_SW_INT_NUM));
}

hli_queue_handle_t hli_queue_create(size_t nelem, size_t elem_size, QueueHandle_t downstream)
{
    const size_t buf_elem = nelem + 1;
    if (elem_size > HLI_QUEUE_MAX_ELEM_SIZE) {
        return NULL;
    }
    size_t buf_size = buf_elem * elem_size;
    hli_queue_handle_t res = (hli_queue_handle_t) heap_caps_malloc(sizeof(*res) + buf_size,
        MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT);
    if (res == NULL) {
        return NULL;
    }
    queue_init(res, buf_size, elem_size, downstream);
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
        if (was_empty && queue != &s_meta_queue) {
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
