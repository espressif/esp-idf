/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/idf_additions.h"
#include "sdkconfig.h"

#include "rom/lldesc.h"
#include "soc/soc_caps.h"
#include "driver/dac_continuous.h"

#include "dac_priv_common.h"
#include "dac_priv_dma.h"

#if CONFIG_DAC_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"
#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#endif

#define DAC_DMA_MAX_BUF_SIZE        4092        // Max DMA buffer size is 4095 but better to align with 4 bytes, so set 4092 here
#if CONFIG_DAC_ISR_IRAM_SAFE || CONFIG_DAC_CTRL_FUNC_IN_IRAM
#define DAC_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
#define DAC_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif

#if CONFIG_DAC_ISR_IRAM_SAFE
#define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#else
#define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#endif

#define DAC_DMA_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)

#define DAC_STAILQ_REMOVE(head, elm, type, field) do {          \
    if ((head)->stqh_first == (elm)) {                          \
        STAILQ_REMOVE_HEAD((head), field);                      \
    } else {                                                    \
        struct type *curelm = (head)->stqh_first;               \
        while (curelm->field.stqe_next != (elm) &&              \
               curelm->field.stqe_next != NULL)                 \
            curelm = curelm->field.stqe_next;                   \
        if (curelm->field.stqe_next && (curelm->field.stqe_next = \
            curelm->field.stqe_next->field.stqe_next) == NULL)  \
                (head)->stqh_last = &(curelm)->field.stqe_next; \
    }                                                           \
} while (/*CONSTCOND*/0)

struct dac_continuous_s {
    uint32_t                chan_cnt;
    dac_continuous_config_t cfg;
    atomic_bool             is_enabled;
    atomic_bool             is_cyclic;
    atomic_bool             is_running;
    atomic_bool             is_async;
    intr_handle_t           intr_handle;                /* Interrupt handle */
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t    pm_lock;
#endif
    SemaphoreHandle_t       mutex;
    QueueHandle_t           desc_pool;                  /* The pool of available descriptors
                                                         * The descriptors in the pool are not linked in to pending chain */

    lldesc_t                **desc;
    uint8_t                 **bufs;
    STAILQ_HEAD(desc_chain_s, lldesc_s) head;           /* Head of the descriptor chain
                                                         * The descriptors in the chain are pending to be sent or sending now */
    dac_event_callbacks_t   cbs;                        /* Interrupt callbacks */
    void                    *user_data;
};

static const char *TAG = "dac_continuous";

static bool s_dma_in_use = false;
static portMUX_TYPE desc_spinlock = portMUX_INITIALIZER_UNLOCKED;

#define DESC_ENTER_CRITICAL()        portENTER_CRITICAL(&desc_spinlock)
#define DESC_EXIT_CRITICAL()         portEXIT_CRITICAL(&desc_spinlock)

#define DESC_ENTER_CRITICAL_ISR()    portENTER_CRITICAL_ISR(&desc_spinlock)
#define DESC_EXIT_CRITICAL_ISR()     portEXIT_CRITICAL_ISR(&desc_spinlock)

static void s_dac_free_dma_desc(dac_continuous_handle_t handle)
{
    STAILQ_INIT(&handle->head);
    if (handle->desc != NULL) {
        if (handle->desc[0]) {
            free(handle->desc[0]);
        }
        free(handle->desc);
        handle->desc = NULL;
    }

    if (handle->bufs != NULL) {
        for (int i = 0; i < handle->cfg.desc_num; i++) {
            if (handle->bufs[i]) {
                free(handle->bufs[i]);
                handle->bufs[i] = NULL;
            }
        }
        free(handle->bufs);
        handle->bufs = NULL;
    }
}

static esp_err_t s_dac_alloc_dma_desc(dac_continuous_handle_t handle)
{
    esp_err_t ret = ESP_OK;

    STAILQ_INIT(&handle->head);
    handle->desc = (lldesc_t **) heap_caps_calloc(handle->cfg.desc_num, sizeof(lldesc_t *), DAC_DMA_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle->desc, ESP_ERR_NO_MEM, TAG, "failed to allocate dma descriptor array");
    handle->bufs = (uint8_t **) heap_caps_calloc(handle->cfg.desc_num, sizeof(uint8_t *), DAC_DMA_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle->bufs, ESP_ERR_NO_MEM, TAG, "failed to allocate dma buffer array");
    lldesc_t *descs = (lldesc_t *)heap_caps_calloc(handle->cfg.desc_num, sizeof(lldesc_t), DAC_DMA_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(descs, ESP_ERR_NO_MEM, TAG, "failed to allocate dma descriptors");
    for (int cnt = 0; cnt < handle->cfg.desc_num; cnt++) {
        /* Allocate DMA descriptor */
        handle->desc[cnt] = &descs[cnt];
        ESP_GOTO_ON_FALSE(handle->desc[cnt], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma descriptor");
        ESP_LOGD(TAG, "desc[%d] %p", cnt, handle->desc[cnt]);
        /* Allocate DMA buffer */
        handle->bufs[cnt] = (uint8_t *) heap_caps_calloc(1, handle->cfg.buf_size, DAC_DMA_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(handle->bufs[cnt], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma buffer");
        /* Assign initial value */
        lldesc_config(handle->desc[cnt], LLDESC_SW_OWNED, 1, 0, handle->cfg.buf_size);
        handle->desc[cnt]->size = handle->cfg.buf_size;
        handle->desc[cnt]->buf = handle->bufs[cnt];
        handle->desc[cnt]->offset = 0;
    }

    return ESP_OK;
err:
    /* Free DMA buffer if failed to allocate memory */
    s_dac_free_dma_desc(handle);
    return ret;
}

static void IRAM_ATTR s_dac_default_intr_handler(void *arg)
{
    dac_continuous_handle_t handle = (dac_continuous_handle_t)arg;
    uint32_t dummy;
    BaseType_t need_awoke = pdFALSE;
    BaseType_t tmp = pdFALSE;
    uint32_t intr_mask = dac_dma_periph_intr_is_triggered();
    if (intr_mask & DAC_DMA_EOF_INTR) {
        lldesc_t *fdesc = (lldesc_t *)dac_dma_periph_intr_get_eof_desc();
        if (!atomic_load(&handle->is_cyclic)) {
            /* Remove the descriptor in the chain that finished sent */
            DESC_ENTER_CRITICAL_ISR();
            if (STAILQ_FIRST(&handle->head) != NULL) {
                DAC_STAILQ_REMOVE(&handle->head, fdesc, lldesc_s, qe);
            }
            DESC_EXIT_CRITICAL_ISR();

            if (xQueueIsQueueFullFromISR(handle->desc_pool) == pdTRUE) {
                xQueueReceiveFromISR(handle->desc_pool, &dummy, &tmp);
                need_awoke |= tmp;
            }
            xQueueSendFromISR(handle->desc_pool, &fdesc, &tmp);
            need_awoke |= tmp;
        }
        if (handle->cbs.on_convert_done) {
            dac_event_data_t evt_data = {
                .buf = (void *)fdesc->buf,
                .buf_size = handle->cfg.buf_size,
                .write_bytes = fdesc->length,
            };
            need_awoke |= handle->cbs.on_convert_done(handle, &evt_data, handle->user_data);
        }
    }
    if (intr_mask & DAC_DMA_TEOF_INTR) {
        /* Total end of frame interrupt received, DMA stopped */
        atomic_store(&handle->is_running, false);
        if (handle->cbs.on_stop) {
            need_awoke |= handle->cbs.on_stop(handle, NULL, handle->user_data);
        }
    }
    if (need_awoke == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

esp_err_t dac_continuous_new_channels(const dac_continuous_config_t *cont_cfg, dac_continuous_handle_t *ret_handle)
{
#if CONFIG_DAC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Parameters validation */
    DAC_NULL_POINTER_CHECK(cont_cfg);
    DAC_NULL_POINTER_CHECK(ret_handle);
    ESP_RETURN_ON_FALSE(cont_cfg->chan_mask <= DAC_CHANNEL_MASK_ALL, ESP_ERR_INVALID_ARG, TAG, "invalid dac channel id");
    ESP_RETURN_ON_FALSE(cont_cfg->desc_num > 1, ESP_ERR_INVALID_STATE, TAG, "at least two DMA descriptor needed");
    ESP_RETURN_ON_FALSE(!s_dma_in_use, ESP_ERR_INVALID_STATE, TAG, "DMA already in use");

    esp_err_t ret = ESP_OK;

    /* Register the channels */
    for (uint32_t i = 0, mask = cont_cfg->chan_mask; mask; mask >>= 1, i++) {
        if (mask & 0x01) {
            ESP_GOTO_ON_ERROR(dac_priv_register_channel(i, "dac continuous"),
                              err4, TAG, "register dac channel %"PRIu32" failed", i);
        }
    }

    /* Allocate continuous mode struct */
    dac_continuous_handle_t handle = heap_caps_calloc(1, sizeof(struct dac_continuous_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle, ESP_ERR_NO_MEM, TAG, "no memory for the dac continuous mode structure");

    /* Allocate queue and mutex*/
    handle->desc_pool = xQueueCreateWithCaps(cont_cfg->desc_num, sizeof(lldesc_t *), DAC_MEM_ALLOC_CAPS);
    handle->mutex = xSemaphoreCreateMutexWithCaps(DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->desc_pool, ESP_ERR_NO_MEM, err3, TAG, "no memory for message queue");
    ESP_GOTO_ON_FALSE(handle->mutex, ESP_ERR_NO_MEM, err3, TAG, "no memory for channels mutex");

    /* Create PM lock */
#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock_type = ESP_PM_APB_FREQ_MAX;
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, "dac_driver", &handle->pm_lock), err3, TAG, "Failed to create DAC pm lock");
#endif
    handle->chan_cnt = __builtin_popcount(cont_cfg->chan_mask);
    memcpy(&(handle->cfg), cont_cfg, sizeof(dac_continuous_config_t));

    atomic_init(&handle->is_enabled, false);
    atomic_init(&handle->is_cyclic, false);
    atomic_init(&handle->is_running, false);
    atomic_init(&handle->is_async, false);

    /* Allocate DMA buffer */
    ESP_GOTO_ON_ERROR(s_dac_alloc_dma_desc(handle), err2, TAG, "Failed to allocate memory for DMA buffers");

    /* Initialize DAC DMA peripheral */
    ESP_GOTO_ON_ERROR(dac_dma_periph_init(cont_cfg->freq_hz,
                                          cont_cfg->chan_mode == DAC_CHANNEL_MODE_ALTER,
                                          cont_cfg->clk_src == DAC_DIGI_CLK_SRC_APLL),
                      err2, TAG, "Failed to initialize DAC DMA peripheral");
    /* Register DMA interrupt */
    ESP_GOTO_ON_ERROR(esp_intr_alloc(dac_dma_periph_get_intr_signal(), DAC_INTR_ALLOC_FLAGS,
                                     s_dac_default_intr_handler, handle, &(handle->intr_handle)),
                      err1, TAG, "Failed to register DAC DMA interrupt");
    /* Connect DAC module to the DMA peripheral */
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(true);
    DAC_RTC_EXIT_CRITICAL();
    s_dma_in_use = true;
    *ret_handle = handle;
    return ret;

err1:
    dac_dma_periph_deinit();
err2:
    s_dac_free_dma_desc(handle);
err3:
    if (handle->desc_pool) {
        vQueueDeleteWithCaps(handle->desc_pool);
    }
    if (handle->mutex) {
        vSemaphoreDeleteWithCaps(handle->mutex);
    }
    free(handle);
err4:
    /* Deregister the channels */
    for (uint32_t i = 0, mask = cont_cfg->chan_mask; mask; mask >>= 1, i++) {
        if (mask & 0x01) {
            dac_priv_deregister_channel(i);
        }
    }
    return ret;
}

esp_err_t dac_continuous_del_channels(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "dac continuous output not disabled yet");

    /* Deregister DMA interrupt */
    if (handle->intr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_free(handle->intr_handle), TAG, "Failed to deregister DMA interrupt");
        handle->intr_handle = NULL;
    }
    /* Deinitialize DMA peripheral */
    ESP_RETURN_ON_ERROR(dac_dma_periph_deinit(), TAG, "Failed to deinitialize DAC DMA peripheral");
    /* Disconnect DAC module from the DMA peripheral */
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(false);
    DAC_RTC_EXIT_CRITICAL();

    /* Free allocated resources */
    s_dac_free_dma_desc(handle);
    if (handle->desc_pool) {
        vQueueDeleteWithCaps(handle->desc_pool);
        handle->desc_pool = NULL;
    }
    if (handle->mutex) {
        vSemaphoreDeleteWithCaps(handle->mutex);
        handle->mutex = NULL;
    }
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
        handle->pm_lock = NULL;
    }
#endif

    /* Deregister the channels */
    for (uint32_t i = 0, mask = handle->cfg.chan_mask; mask; mask >>= 1, i++) {
        if (mask & 0x01) {
            dac_priv_deregister_channel(i);
        }
    }
    free(handle);
    s_dma_in_use = false;

    return ESP_OK;
}

esp_err_t dac_continuous_register_event_callback(dac_continuous_handle_t handle, const dac_event_callbacks_t *callbacks, void *user_data)
{
    DAC_NULL_POINTER_CHECK(handle);
    if (!callbacks) {
        memset(&handle->cbs, 0, sizeof(dac_event_callbacks_t));
        return ESP_OK;
    }
#if CONFIG_DAC_ISR_IRAM_SAFE
    if (callbacks->on_convert_done) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_convert_done), ESP_ERR_INVALID_ARG, TAG, "on_convert_done callback not in IRAM");
    }
    if (callbacks->on_stop) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callbacks->on_stop), ESP_ERR_INVALID_ARG, TAG, "on_stop callback not in IRAM");
    }
    if (user_data) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
    }
#endif
    memcpy(&handle->cbs, callbacks, sizeof(dac_event_callbacks_t));
    handle->user_data = user_data;

    return ESP_OK;
}

esp_err_t dac_continuous_enable(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "dac continuous has already enabled");
    esp_err_t ret = ESP_OK;
    /* Reset the descriptor pool */
    xQueueReset(handle->desc_pool);
    for (int i = 0; i < handle->cfg.desc_num; i++) {
        ESP_GOTO_ON_FALSE(xQueueSend(handle->desc_pool, &handle->desc[i], 0) == pdTRUE,
                          ESP_ERR_INVALID_STATE, err, TAG, "the descriptor pool is not cleared");
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    for (uint32_t i = 0, mask = handle->cfg.chan_mask; mask; mask >>= 1, i++) {
        if (mask & 0x01) {
            dac_priv_enable_channel(i);
        }
    }
    dac_dma_periph_enable();
    esp_intr_enable(handle->intr_handle);
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(true);
    DAC_RTC_EXIT_CRITICAL();
    atomic_store(&handle->is_enabled, true);
err:
    return ret;
}

esp_err_t dac_continuous_disable(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "dac continuous has already disabled");
    atomic_store(&handle->is_enabled, false);
    dac_dma_periph_disable();
    esp_intr_disable(handle->intr_handle);
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(false);
    DAC_RTC_EXIT_CRITICAL();
    atomic_store(&handle->is_running, false);
    for (uint32_t i = 0, mask = handle->cfg.chan_mask; mask; mask >>= 1, i++) {
        if (mask & 0x01) {
            dac_priv_disable_channel(i);
        }
    }
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    return ESP_OK;
}

esp_err_t dac_continuous_start_async_writing(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "dac continuous has not been enabled");
    ESP_RETURN_ON_FALSE(handle->cbs.on_convert_done, ESP_ERR_INVALID_STATE, TAG,
                        "please register 'on_convert_done' callback before starting asynchronous writing");

    atomic_store(&handle->is_async, true);

    if (atomic_load(&handle->is_cyclic)) {
        /* Break the DMA descriptor chain to stop the DMA first */
        for (int i = 0; i < handle->cfg.desc_num; i++) {
            STAILQ_NEXT(handle->desc[i], qe) = NULL;
        }
    }
    /* Wait for the previous DMA stop */
    while (atomic_load(&handle->is_running)) {}

    /* Link all descriptors as a ring */
    for (int i = 0; i < handle->cfg.desc_num; i++) {
        memset(handle->bufs[i], 0, handle->cfg.buf_size);
        STAILQ_NEXT(handle->desc[i], qe) = (i < handle->cfg.desc_num - 1) ? handle->desc[i + 1] : handle->desc[0];
    }
    dac_dma_periph_dma_trans_start((uint32_t)handle->desc[0]);
    atomic_store(&handle->is_running, true);

    return ESP_OK;
}

esp_err_t dac_continuous_stop_async_writing(dac_continuous_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(atomic_load(&handle->is_async), ESP_ERR_INVALID_STATE, TAG, "dac asynchronous writing has not been started");

    /* Break the DMA descriptor chain to stop the DMA first */
    for (int i = 0; i < handle->cfg.desc_num; i++) {
        STAILQ_NEXT(handle->desc[i], qe) = NULL;
    }
    /* Wait for the previous DMA stop */
    while (atomic_load(&handle->is_running)) {}
    atomic_store(&handle->is_async, false);

    return ESP_OK;
}

/* Buffer expanding coefficient, the input buffer will expand to twice length while enabled AUTO_16_BIT */
#if CONFIG_DAC_DMA_AUTO_16BIT_ALIGN
#define DAC_16BIT_ALIGN_COEFF   2
#else
#define DAC_16BIT_ALIGN_COEFF   1
#endif

static size_t s_dac_load_data_into_buf(dac_continuous_handle_t handle, uint8_t *dest, size_t dest_len, const uint8_t *src, size_t src_len)
{
    size_t load_bytes = 0;
#if CONFIG_DAC_DMA_AUTO_16BIT_ALIGN
    /* Load the data to the high 8 bit in the 16-bit width slot */
    load_bytes = (src_len * 2 > dest_len) ? dest_len : src_len * 2;
    for (int i = 0; i < load_bytes; i += 2) {
        dest[i + 1] = src[i / 2] + handle->cfg.offset;
    }
#else
    /* Load the data into the DMA buffer */
    load_bytes = (src_len > dest_len) ? dest_len : src_len;
    for (int i = 0; i < load_bytes; i++) {
        dest[i] = src[i] + handle->cfg.offset;
    }
#endif
    return load_bytes;
}

esp_err_t dac_continuous_write_asynchronously(dac_continuous_handle_t handle, uint8_t *dma_buf,
                                              size_t dma_buf_len, const uint8_t *data,
                                              size_t data_len, size_t *bytes_loaded)
{
    DAC_NULL_POINTER_CHECK_ISR(handle);
    DAC_NULL_POINTER_CHECK_ISR(dma_buf);
    DAC_NULL_POINTER_CHECK_ISR(data);
    ESP_RETURN_ON_FALSE_ISR(atomic_load(&handle->is_async), ESP_ERR_INVALID_STATE, TAG, "The asynchronous writing has not started");
    int i;
    for (i = 0; i < handle->cfg.desc_num; i++) {
        if (dma_buf == handle->bufs[i]) {
            break;
        }
    }
    /* Fail to find the DMA buffer address */
    ESP_RETURN_ON_FALSE_ISR(i < handle->cfg.desc_num, ESP_ERR_NOT_FOUND, TAG, "Not found the corresponding DMA buffer");
    size_t load_bytes = s_dac_load_data_into_buf(handle, dma_buf, dma_buf_len, data, data_len);
    lldesc_config(handle->desc[i], LLDESC_HW_OWNED, 1, 0, load_bytes);
    if (bytes_loaded) {
        *bytes_loaded = load_bytes / DAC_16BIT_ALIGN_COEFF;
    }
    return ESP_OK;
}

esp_err_t dac_continuous_write_cyclically(dac_continuous_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(!atomic_load(&handle->is_async), ESP_ERR_INVALID_STATE, TAG, "Asynchronous writing is running, can't write cyclically");
    ESP_RETURN_ON_FALSE(buf_size <= handle->cfg.buf_size * handle->cfg.desc_num, ESP_ERR_INVALID_ARG, TAG,
                        "The cyclic buffer size exceeds the total DMA buffer size: %"PRIu32"(desc_num) * %d(buf_size) = %"PRIu32,
                        handle->cfg.desc_num, handle->cfg.buf_size, handle->cfg.buf_size * handle->cfg.desc_num);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(handle->mutex,  portMAX_DELAY);
    if (atomic_load(&handle->is_cyclic)) {
        /* Break the DMA descriptor chain to stop the DMA first */
        for (int i = 0; i < handle->cfg.desc_num; i++) {
            STAILQ_NEXT(handle->desc[i], qe) = NULL;
        }
    }
    /* Wait for the previous DMA stop */
    while (atomic_load(&handle->is_running)) {}
    atomic_store(&handle->is_cyclic, true);

    size_t src_buf_size = buf_size;
    uint32_t split = 1;
    int i;
    for (i = 0; i < handle->cfg.desc_num && buf_size > 0; i++) {
        /* To spread data more averagely, average the last two descriptors */
        split = (buf_size * DAC_16BIT_ALIGN_COEFF < handle->cfg.buf_size * 2) ? 3 - split : 1;
        size_t load_bytes = s_dac_load_data_into_buf(handle, handle->bufs[i], handle->cfg.buf_size, buf, buf_size / split);
        lldesc_config(handle->desc[i], LLDESC_HW_OWNED, 1, 0, load_bytes);
        /* Link to the next descriptor */
        STAILQ_NEXT(handle->desc[i], qe) = (i < handle->cfg.desc_num - 1) ? handle->desc[i + 1] : NULL;
        buf_size -= load_bytes / DAC_16BIT_ALIGN_COEFF;
        buf += load_bytes / DAC_16BIT_ALIGN_COEFF;
    }
    /* Link the tail to the head as a ring */
    STAILQ_NEXT(handle->desc[i - 1], qe) = handle->desc[0];

    dac_dma_periph_dma_trans_start((uint32_t)handle->desc[0]);
    atomic_store(&handle->is_running, true);
    if (bytes_loaded) {
        *bytes_loaded = src_buf_size - buf_size;
    }

    xSemaphoreGive(handle->mutex);
    return ret;
}

static esp_err_t s_dac_wait_to_load_dma_data(dac_continuous_handle_t handle, uint8_t *buf, size_t buf_size, size_t *w_size, TickType_t timeout_tick)
{
    lldesc_t *desc;
    /* Try to get the descriptor from the pool */
    ESP_RETURN_ON_FALSE(xQueueReceive(handle->desc_pool, &desc, timeout_tick) == pdTRUE,
                        ESP_ERR_TIMEOUT, TAG, "Get available descriptor timeout");
    /* To ensure it is not in the pending desc chain */
    if (STAILQ_FIRST(&handle->head) != NULL) {
        DAC_STAILQ_REMOVE(&handle->head, desc, lldesc_s, qe);
    }

    static bool split_flag = false;
    uint8_t *dma_buf = (uint8_t *)desc->buf;
    if (buf_size * DAC_16BIT_ALIGN_COEFF < 2 * handle->cfg.buf_size) {
        if (!split_flag) {
            buf_size >>= 1;
            split_flag = true;
        } else {
            split_flag = false;
        }
    }
    size_t load_bytes = s_dac_load_data_into_buf(handle, dma_buf, handle->cfg.buf_size, buf, buf_size);
    lldesc_config(desc, LLDESC_HW_OWNED, 1, 0, load_bytes);
    desc->size = load_bytes;
    *w_size = load_bytes / DAC_16BIT_ALIGN_COEFF;
    /* Insert the loaded descriptor to the end of the chain, waiting to be sent */
    DESC_ENTER_CRITICAL();
    STAILQ_INSERT_TAIL(&handle->head, desc, qe);
    DESC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_continuous_write(dac_continuous_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, int timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(buf);
    ESP_RETURN_ON_FALSE(atomic_load(&handle->is_enabled), ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(!atomic_load(&handle->is_async), ESP_ERR_INVALID_STATE, TAG, "Asynchronous writing is running, can't write synchronously");

    esp_err_t ret = ESP_OK;
    TickType_t timeout_tick = timeout_ms < 0 ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex,  timeout_tick) == pdTRUE, ESP_ERR_TIMEOUT, TAG, "Take semaphore timeout");

    size_t w_size = 0;
    size_t src_buf_size = buf_size;
    /* Reset the desc_pool and chain if called cyclic function last time  */
    if (atomic_load(&handle->is_cyclic)) {
        xQueueReset(handle->desc_pool);
        /* Break the chain if DMA still running */
        for (int i = 0; i < handle->cfg.desc_num; i++) {
            STAILQ_NEXT(handle->desc[i], qe) = NULL;
            xQueueSend(handle->desc_pool, &handle->desc[i], 0);
        }
        STAILQ_INIT(&handle->head);
        atomic_store(&handle->is_cyclic, false);
    }
    /* When there is no descriptor in the chain, DMA has stopped, load data and start the DMA link */
    if (STAILQ_FIRST(&handle->head) == NULL) {
        /* Wait for the previous DMA stop */
        while (atomic_load(&handle->is_running)) {}
        for (int i = 0;
                i < handle->cfg.desc_num && buf_size > 0;
                i++, buf += w_size, buf_size -= w_size) {
            ESP_GOTO_ON_ERROR(s_dac_wait_to_load_dma_data(handle, buf, buf_size, &w_size, timeout_tick), err, TAG, "Load data failed");
        }
        dac_dma_periph_dma_trans_start((uint32_t)(STAILQ_FIRST(&handle->head)));
        atomic_store(&handle->is_running, true);
    }
    /* If the source buffer is not totally loaded, keep loading the rest data */
    while (buf_size > 0) {
        ESP_GOTO_ON_ERROR(s_dac_wait_to_load_dma_data(handle, buf, buf_size, &w_size, timeout_tick), err, TAG, "Load data failed");
        /* If the DMA stopped but there are still some descriptors not sent, start the DMA again */
        DESC_ENTER_CRITICAL();
        if (STAILQ_FIRST(&handle->head) && !atomic_load(&handle->is_running)) {
            dac_dma_periph_dma_trans_start((uint32_t)(STAILQ_FIRST(&handle->head)));
            atomic_store(&handle->is_running, true);
        }
        DESC_EXIT_CRITICAL();
        buf += w_size;
        buf_size -= w_size;
    }
err:
    /* The bytes number that has been loaded */
    if (bytes_loaded) {
        *bytes_loaded = src_buf_size - buf_size;
    }
    xSemaphoreGive(handle->mutex);
    return ret;
}
