/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "sdkconfig.h"

#include "rom/lldesc.h"
#include "soc/soc_caps.h"
#include "soc/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "driver/rtc_io.h"
#include "driver/dac_driver.h"
#include "dac_dma.h"
#include "esp_memory_utils.h"
#include "clk_ctrl_os.h"

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
    #define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
#else
    #define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
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

typedef enum {
    DAC_STATE_INITIAL,              /* Initial state, dac has been registered */
    DAC_STATE_OUTPUT_READY,         /* DAC channels enabled and ready to output constant voltage */
    DAC_STATE_DMA_READY,            /* DMA mode initialized, but not started */
    DAC_STATE_CW_READY,             /* Cosine wave mode is initialized, but not started */
    DAC_STATE_DMA_ENABLED,          /* DAC DMA dma is enabled, can't switch mode in this stage */
    DAC_STATE_CW_RUNNING,           /* DAC cosine wave generator is running, can't switch mode in this stage */
} dac_state_t;

typedef struct {
    QueueHandle_t       desc_pool;            /* The pool of available descriptors
                                             * The descriptors in the pool are not linked in to pending chain */
#if CONFIG_DAC_ISR_IRAM_SAFE
    StaticQueue_t       *desc_pool_struct;    /* Static message queue struct */
    void                *desc_pool_storage;   /* Static message queue storage */
#endif
    dac_conti_config_t  cfg;
    STAILQ_HEAD(desc_chain_s, lldesc_s) head;   /* Head of the descriptor chain
                                                 * The descriptors in the chain are pending to be sent or sending now */
    lldesc_t            **desc;
    uint8_t             **bufs;
    volatile bool       is_running;             /* Is DMA running or stopped */
    volatile bool       is_cyclic;              /* Is transport data cyclicly */
} dac_dma_t;

typedef struct dac_channel_chain_node_s {
    dac_channel_t                           id;
    SLIST_ENTRY(dac_channel_chain_node_s)   next;
} dac_channel_info_t;

typedef SLIST_HEAD(dac_channel_chain_s, dac_channel_chain_node_s) dac_channel_chain_t;

struct dac_channels_s {
    uint32_t                        chan_num;
    dac_state_t                     state;
    SemaphoreHandle_t               mutex;
    StaticSemaphore_t               *mutex_struct;      /*!< Static mutex struct */
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t            pm_lock;
#endif
    bool                            is_enabled;
    dac_dma_t                       dma;
    dac_cosine_config_t             sw_cfg;
    dac_channel_chain_t             head;
};

typedef struct {
    dac_channel_info_t      *chan[SOC_DAC_PERIPH_NUM];
    bool                    dma_in_use;
} dac_platform_t;

static const char *TAG = "DAC";

static dac_platform_t s_dac = {
    .chan = {
        [0 ... SOC_DAC_PERIPH_NUM - 1] = NULL,
    },
    .dma_in_use = false,
};

/* Global dac spin lock for the whole DAC driver */
portMUX_TYPE dac_spinlock = portMUX_INITIALIZER_UNLOCKED;  // TODO: check rtc_spinlock

#define DAC_NULL_POINTER_CHECK(p)     ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")

static void dac_free_dma_desc(dac_channels_handle_t handle)
{
    STAILQ_EMPTY(&handle->dma.head);
    if (handle->dma.desc != NULL) {
        for (int i = 0; i < handle->dma.cfg.desc_num; i++) {
            if (handle->dma.desc[i]) {
                free(handle->dma.desc[i]);
                handle->dma.desc[i] = NULL;
            }
        }
        free(handle->dma.desc);
        handle->dma.desc = NULL;
    }

    if (handle->dma.bufs != NULL) {
        for (int i = 0; i < handle->dma.cfg.desc_num; i++) {
            if (handle->dma.bufs[i]) {
                free(handle->dma.bufs[i]);
                handle->dma.bufs[i] = NULL;
            }
        }
        free(handle->dma.bufs);
        handle->dma.bufs = NULL;
    }
}

static esp_err_t dac_alloc_dma_desc(dac_channels_handle_t handle)
{
    esp_err_t ret = ESP_OK;

    STAILQ_INIT(&handle->dma.head);
    handle->dma.desc = (lldesc_t **) heap_caps_calloc(handle->dma.cfg.desc_num, sizeof(lldesc_t *), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle->dma.desc, ESP_ERR_NO_MEM, TAG, "Faild to allocate dma descriptor array");
    handle->dma.bufs = (uint8_t **) heap_caps_calloc(handle->dma.cfg.desc_num, sizeof(uint8_t *), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle->dma.bufs, ESP_ERR_NO_MEM, TAG, "Faild to allocate dma buffer array");
    for (int cnt = 0; cnt < handle->dma.cfg.desc_num; cnt++) {
        /* Allocate DMA descpriptor */
        handle->dma.desc[cnt] = (lldesc_t *) heap_caps_calloc(1, sizeof(lldesc_t), DAC_DMA_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(handle->dma.desc[cnt], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma descriptor");
        /* Allocate DMA buffer */
        handle->dma.bufs[cnt] = (uint8_t *) heap_caps_calloc(1, handle->dma.cfg.buf_size, DAC_DMA_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(handle->dma.desc[cnt], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma buffer");
        /* Assign initial value */
        lldesc_config(handle->dma.desc[cnt], LLDESC_HW_OWNED, 1, 0, handle->dma.cfg.buf_size);
        handle->dma.desc[cnt]->size = handle->dma.cfg.buf_size;
        handle->dma.desc[cnt]->buf = handle->dma.bufs[cnt];
        handle->dma.desc[cnt]->offset = 0;
        xQueueSend(handle->dma.desc_pool, &handle->dma.desc[cnt], portMAX_DELAY);
    }

    return ESP_OK;
err:
    /* Free DMA buffer if failed to allocate memory */
    dac_free_dma_desc(handle);
    return ret;
}

static void IRAM_ATTR dac_default_intr_handler(void *arg)
{
    dac_channels_handle_t handle = (dac_channels_handle_t)arg;
    uint32_t dummy;
    BaseType_t need_awoke = pdFALSE;
    BaseType_t tmp;
    uint32_t intr_mask = dac_dma_periph_intr_is_triggered();
    if ((intr_mask & DAC_DMA_EOF_INTR) && (!handle->dma.is_cyclic)) {
        lldesc_t *fdesc = (lldesc_t *)dac_dma_periph_intr_get_eof_desc();
        /* Remove the descriptor in the chain that finished sent */
        portENTER_CRITICAL(&dac_spinlock);
        if (STAILQ_FIRST(&handle->dma.head) != NULL) {
            DAC_STAILQ_REMOVE(&handle->dma.head, fdesc, lldesc_s, qe);
        }
        portEXIT_CRITICAL(&dac_spinlock);
        if (xQueueIsQueueFullFromISR(handle->dma.desc_pool) == pdTRUE) {
            xQueueReceiveFromISR(handle->dma.desc_pool, &dummy, &tmp);
            need_awoke |= tmp;
        }
        xQueueSendFromISR(handle->dma.desc_pool, &fdesc, &tmp);
        need_awoke |= tmp;
    }
    if (intr_mask & DAC_DMA_TEOF_INTR) {
        /* Total end of frame interrupt received, DMA stopped */
        handle->dma.is_running = false;
    }
    if (need_awoke == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/*--------------------------------------------------------------------------
                            DAC common APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_new_channels(const dac_channels_config_t *dac_cfg, dac_channels_handle_t *handle)
{
#if CONFIG_DAC_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;

    DAC_NULL_POINTER_CHECK(dac_cfg);
    DAC_NULL_POINTER_CHECK(handle);

    ESP_RETURN_ON_FALSE((uint32_t)dac_cfg->chan_sel > 0, ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel");
    ESP_RETURN_ON_FALSE((32 - __builtin_clz((uint32_t)dac_cfg->chan_sel)) <= SOC_DAC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel");

    dac_channels_handle_t channels = (dac_channels_handle_t)heap_caps_calloc(1, sizeof(struct dac_channels_s), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(channels, ESP_ERR_NO_MEM, TAG, "No memory for DAC channels' structure");

    channels->mutex_struct = (StaticSemaphore_t *)heap_caps_calloc(1, sizeof(StaticSemaphore_t), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(channels->mutex_struct, ESP_ERR_NO_MEM, err, TAG, "No memory for channels mutex struct");
    channels->mutex = xSemaphoreCreateMutexStatic(channels->mutex_struct);
    ESP_GOTO_ON_FALSE(channels->mutex, ESP_ERR_NO_MEM, err, TAG, "No memory for channels mutex");
    channels->chan_num = 0;
    channels->is_enabled = false;
    channels->state = DAC_STATE_INITIAL; // Set static output as default
    /* Register selected channels and link into a chain*/
    SLIST_INIT(&channels->head);
    for (uint32_t msk = (uint32_t)dac_cfg->chan_sel, i = 0; msk != 0; msk >>= 1, i++) {
        if (msk & 0x01) {
            /* Allocate memory for the channel when it is enabled */
            ESP_GOTO_ON_FALSE(!s_dac.chan[i], ESP_ERR_INVALID_STATE, err, TAG, "DAC channel %d has been registered already", i + 1);
            dac_channel_info_t *node = (dac_channel_info_t *)calloc(1, sizeof(dac_channel_info_t));
            ESP_GOTO_ON_FALSE(node, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC channel object");
            node->id = i;
            s_dac.chan[i] = node;
            channels->chan_num++;
            /* Add the channel node into a chain */
            SLIST_INSERT_HEAD(&channels->head, node, next);
        }
    }

    *handle = channels;

    return ret;

err:
    /* Free the resource when error occurs */
    dac_del_channels(channels);
    channels = NULL;

    return ret;
}

esp_err_t dac_del_channels(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_INITIAL, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels is not deinitialized");
    ESP_RETURN_ON_FALSE(!handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels is not disabled");

    if (handle->mutex) {
        vSemaphoreDelete(handle->mutex);
    }
    if (handle->mutex_struct) {
        free(handle->mutex_struct);
    }
    while (SLIST_FIRST(&handle->head)) {
        dac_channel_info_t *node = SLIST_FIRST(&handle->head);
        SLIST_REMOVE_HEAD(&handle->head, next);
        s_dac.chan[node->id] = NULL;
        free(node);
    }
    free(handle);

    return ESP_OK;
}

esp_err_t dac_channels_enable(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has been enabled already");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

    dac_channel_info_t *p;
    SLIST_FOREACH(p, &handle->head, next) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[p->id];
        rtc_gpio_init(gpio_num);
        rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
        rtc_gpio_pullup_dis(gpio_num);
        rtc_gpio_pulldown_dis(gpio_num);
        portENTER_CRITICAL(&dac_spinlock);
        dac_ll_power_on(p->id);
        dac_ll_rtc_sync_by_adc(false);
        portEXIT_CRITICAL(&dac_spinlock);
    }
    handle->is_enabled = true;
    /* If the channels has not been intialized to other mode, set it `DAC_STATE_OUTPUT_READY` as default */
    if (handle->state == DAC_STATE_INITIAL) {
        handle->state = DAC_STATE_OUTPUT_READY;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channels_disable(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled yet");
    ESP_RETURN_ON_FALSE(handle->state < DAC_STATE_DMA_ENABLED, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels is still running");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    dac_channel_info_t *p;
    SLIST_FOREACH(p, &handle->head, next) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[p->id];
        rtc_gpio_deinit(gpio_num);
        portENTER_CRITICAL(&dac_spinlock);
        dac_ll_power_down(p->id);
        portEXIT_CRITICAL(&dac_spinlock);
    }
    handle->is_enabled = false;
    /* If the channels has not been intialized to other mode, set it `DAC_STATE_INITIAL` as default */
    if (handle->state == DAC_STATE_OUTPUT_READY) {
        handle->state = DAC_STATE_INITIAL;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

/*--------------------------------------------------------------------------
                    DAC direct voltage outputting APIs
 ---------------------------------------------------------------------------*/
#if CONFIG_DAC_ISR_IRAM_SAFE
IRAM_ATTR
#endif
esp_err_t dac_channels_set_voltage(dac_channels_handle_t handle, uint8_t value)
{
    ESP_RETURN_ON_FALSE_ISR(handle, ESP_ERR_INVALID_ARG, TAG, "NULL pointer");
    ESP_RETURN_ON_FALSE_ISR(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "Not enabled");
    ESP_RETURN_ON_FALSE_ISR(handle->state == DAC_STATE_OUTPUT_READY, ESP_ERR_INVALID_STATE, TAG, "Incorrect mode");

    /* Set the voltage for each channel in the channels */
    dac_channel_info_t *p;
    SLIST_FOREACH(p, &handle->head, next) {
        portENTER_CRITICAL(&dac_spinlock);
        dac_ll_update_output_value(p->id, value);
        portEXIT_CRITICAL(&dac_spinlock);
    }
    return ESP_OK;
}

/*--------------------------------------------------------------------------
                          DAC continuous mode APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_channels_init_continuous_mode(dac_channels_handle_t handle, const dac_conti_config_t *conti_cfg)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state < DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has been initialized already");
    ESP_RETURN_ON_FALSE(conti_cfg->desc_num > 1, ESP_ERR_INVALID_STATE, TAG, "At least two DMA descriptor needed");
    ESP_RETURN_ON_FALSE(!s_dac.dma_in_use, ESP_ERR_INVALID_STATE, TAG, "Only one set of channels can use DMA output");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

#if CONFIG_DAC_ISR_IRAM_SAFE
    handle->dma.desc_pool_storage = (uint8_t *)heap_caps_calloc(conti_cfg->desc_num, sizeof(lldesc_t *), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.desc_pool_storage, ESP_ERR_NO_MEM, err3, TAG, "No memory for message queue storage");
    handle->dma.desc_pool_struct = (StaticQueue_t *)heap_caps_calloc(1, sizeof(StaticQueue_t), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.desc_pool_struct , ESP_ERR_NO_MEM, err3, TAG, "No memory for message queue struct");
    handle->dma.desc_pool =  xQueueCreateStatic(conti_cfg->desc_num, sizeof(lldesc_t *), handle->dma.desc_pool_storage, handle->dma.desc_pool_struct);
#else
    handle->dma.desc_pool = xQueueCreate(conti_cfg->desc_num, sizeof(lldesc_t *));
#endif
    ESP_GOTO_ON_FALSE(handle->dma.desc_pool, ESP_ERR_NO_MEM, err3, TAG, "No memory for message queue");
#if CONFIG_PM_ENABLE
    esp_pm_lock_type_t pm_lock_type = conti_cfg->clk_src == DAC_DIGI_CLK_SRC_APLL ? ESP_PM_NO_LIGHT_SLEEP : ESP_PM_APB_FREQ_MAX;
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(pm_lock_type, 0, "dac_driver", &handle->pm_lock), err3, TAG, "Failed to create DAC pm lock");
#endif

    handle->dma.is_running = false;
    /* Allocate DMA buffer */
    memcpy(&(handle->dma.cfg), conti_cfg, sizeof(dac_conti_config_t));
    ESP_GOTO_ON_ERROR(dac_alloc_dma_desc(handle), err2, TAG, "Failed to allocate memory for DMA buffers");

    /* Initialize DAC DMA peripheral */
    ESP_GOTO_ON_ERROR(dac_dma_periph_init(handle->chan_num ,conti_cfg->freq_hz, conti_cfg->chan_mode == DAC_CHANNEL_MODE_ALTER, conti_cfg->clk_src == DAC_DIGI_CLK_SRC_APLL),
                      err2, TAG, "Failed to initialize DAC DMA peripheral");
    /* Register DMA interrupt */
    ESP_GOTO_ON_ERROR(dac_dma_periph_register_intr(dac_default_intr_handler, handle), err1, TAG, "Failed to register DMA interrupt");
    /* Connect DAC module to the DMA peripheral */
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(true);
    portEXIT_CRITICAL(&dac_spinlock);

    handle->state = DAC_STATE_DMA_READY;
    s_dac.dma_in_use = true;

    xSemaphoreGive(handle->mutex);

    return ret;
err1:
    dac_dma_periph_deinit();
err2:
    dac_free_dma_desc(handle);
err3:
    if (handle->dma.desc_pool) {
        vQueueDelete(handle->dma.desc_pool);
    }
#if CONFIG_DAC_ISR_IRAM_SAFE
    if (handle->dma.desc_pool_struct) {
        free(handle->dma.desc_pool_struct);
    }
    if (handle->dma.desc_pool_storage) {
        free(handle->dma.desc_pool_storage);
    }
#endif
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t dac_channels_deinit_continuous_mode(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG,
                           "This set of DAC channels is still running or has been configured to other mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

#if CONFIG_DAC_ISR_IRAM_SAFE
    free(handle->dma.desc_pool_storage);
    handle->dma.desc_pool_storage = NULL;
    free(handle->dma.desc_pool_struct);
    handle->dma.desc_pool_struct = NULL;
#endif
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
        handle->pm_lock = NULL;
    }
#endif
    vQueueDelete(handle->dma.desc_pool);

    /* Free DMA buffer */
    dac_free_dma_desc(handle);

    /* Deregister DMA interrupt */
    ESP_RETURN_ON_ERROR(dac_dma_periph_deregister_intr(), TAG, "Failed to deregister DMA interrupt");
    /* Deinitialize DMA peripheral */
    ESP_RETURN_ON_ERROR(dac_dma_periph_deinit(), TAG, "Failed to deinitialize DAC DMA peripheral");
    /* Disconnect DAC module to the DMA peripheral */
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(false);
    portEXIT_CRITICAL(&dac_spinlock);

    if (handle->is_enabled) {
        handle->state = DAC_STATE_OUTPUT_READY;
    } else {
        handle->state = DAC_STATE_INITIAL;
    }
    s_dac.dma_in_use = false;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channels_enable_continuous_mode(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This set of DAC channels has started already or not working at DMA mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    dac_dma_periph_enable();
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(true);
    portEXIT_CRITICAL(&dac_spinlock);
    handle->state = DAC_STATE_DMA_ENABLED;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channels_disable_continuous_mode(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_ENABLED, ESP_ERR_INVALID_STATE, TAG,
                            "This set of DAC channels has stopped already or not working at DMA mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    dac_dma_periph_disable();
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(false);
    portEXIT_CRITICAL(&dac_spinlock);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    handle->state = DAC_STATE_DMA_READY;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

static esp_err_t s_dac_load_dma_data(dac_channels_handle_t handle, uint8_t *buf, size_t buf_size, size_t *w_size, uint32_t timeout_ms)
{
    lldesc_t *desc = NULL;

    /* Try to get the descriptor from the pool */
    ESP_RETURN_ON_FALSE(xQueueReceive(handle->dma.desc_pool, &desc, pdMS_TO_TICKS(timeout_ms)) == pdTRUE,
                            ESP_ERR_TIMEOUT, TAG, "Get available descriptor timeout");
    if (STAILQ_FIRST(&handle->dma.head) != NULL) {
        DAC_STAILQ_REMOVE(&handle->dma.head, desc, lldesc_s, qe);
    }

    /* Get the DMA buffer address (use 'memcpy' to avoid the 'volatile' warning) */
    uint8_t *dma_buf;
    memcpy(&dma_buf, &desc->buf, sizeof(desc->buf));
#if SOC_DAC_DMA_16BIT_ALIGN && CONFIG_DAC_DMA_AUTO_16BIT_ALIGN
    /* Load the data to the high 8 bit in the 16-bit width slot */
    size_t _w_size = (buf_size > handle->dma.cfg.buf_size / 2) ? handle->dma.cfg.buf_size / 2 : buf_size;
    for (int i = 0; i < _w_size; i++) {
        dma_buf[2 * i + 1] = buf[i];
    }
    lldesc_config(desc, LLDESC_HW_OWNED, 1, 0, _w_size * 2);
    desc->size = _w_size * 2;
#else
    /* Load the data into the DMA buffer */
    size_t _w_size = (buf_size > handle->dma.cfg.buf_size) ? handle->dma.cfg.buf_size : buf_size;
    memcpy(dma_buf, buf, _w_size);
    lldesc_config(desc, LLDESC_HW_OWNED, 1, 0, _w_size);
    desc->size = _w_size;
#endif
    *w_size = _w_size;
    /* Insert the loaded descriptor to the end of the chain, waiting to be sent */
    portENTER_CRITICAL(&dac_spinlock);
    STAILQ_INSERT_TAIL(&handle->dma.head, desc, qe);
    portEXIT_CRITICAL(&dac_spinlock);

    return ESP_OK;
}

esp_err_t dac_channels_write_continuously(dac_channels_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, uint32_t timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(buf);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_ENABLED, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels is not started");
#if CONFIG_DAC_ISR_IRAM_SAFE
    ESP_RETURN_ON_FALSE(esp_ptr_internal(buf), ESP_ERR_INVALID_ARG, TAG, "the buffer is not in internal RAM");
#endif
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex,  pdMS_TO_TICKS(timeout_ms) == pdTRUE), ESP_ERR_TIMEOUT, TAG, "Take semaphore timeout");

    size_t w_size = 0;
    size_t src_buf_size = buf_size;
    /* When there is no descriptor in the chain, DMA has stopped, load data and start the DMA link */
    if (STAILQ_FIRST(&handle->dma.head) == NULL) {
        /* Break the chain if DMA still running */
        for (int i = 0; handle->dma.is_running && i < handle->dma.cfg.desc_num; i++) {
            handle->dma.desc[i]->empty = 0;
        }
        for (int i = 0;
            i < handle->dma.cfg.desc_num && buf_size > 0;
            i++, buf += w_size, buf_size -= w_size) {
            ESP_GOTO_ON_ERROR(s_dac_load_dma_data(handle, buf, buf_size, &w_size, timeout_ms), err, TAG, "Load data failed");
        }
        /* Wait for the previous DMA stop */
        while (handle->dma.is_running) {}
        handle->dma.is_cyclic = false;
        dac_dma_periph_dma_trans_start((uint32_t)(STAILQ_FIRST(&handle->dma.head)));
        handle->dma.is_running = true;
    }
    /* If the source buffer is not totally loaded, keep loading the rest data */
    while (buf_size > 0) {
        /* If the DMA stopped but there are still some decriptors not sent, start the DMA again */
        if ((!handle->dma.is_running) && STAILQ_FIRST(&handle->dma.head)) {
            dac_dma_periph_dma_trans_start((uint32_t)(STAILQ_FIRST(&handle->dma.head)));
        }
        ESP_GOTO_ON_ERROR(s_dac_load_dma_data(handle, buf, buf_size, &w_size, timeout_ms), err, TAG, "Load data failed");
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

esp_err_t dac_channels_write_cyclically(dac_channels_handle_t handle, uint8_t *buf, size_t buf_size, size_t *bytes_loaded, uint32_t timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_ENABLED, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels is not started");
#if CONFIG_DAC_ISR_IRAM_SAFE
    ESP_RETURN_ON_FALSE(esp_ptr_internal(buf), ESP_ERR_INVALID_ARG, TAG, "the buffer is not in internal RAM");
#endif

    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex,  pdMS_TO_TICKS(timeout_ms) == pdTRUE), ESP_ERR_TIMEOUT, TAG, "Take semaphore timeout");

    /* Break the DMA descriptor chain to stop the DMA first */
    for (int i = 0; i < handle->dma.cfg.desc_num; i++) {
        handle->dma.desc[i]->empty = 0;
    }
    size_t src_buf_size = buf_size;

    /* If the source buffer size is smaller than the DMA buffer size, load the source buffer into two descriptors */
    if (buf_size < handle->dma.cfg.buf_size) {
        /* Load data */
        size_t half_size =  buf_size / 2;
        memcpy(handle->dma.bufs[0], buf, half_size);
        memcpy(handle->dma.bufs[1], buf, buf_size - half_size);
        lldesc_config(handle->dma.desc[0], LLDESC_HW_OWNED, 1, 0, half_size);
        lldesc_config(handle->dma.desc[1], LLDESC_HW_OWNED, 1, 0, buf_size - half_size);
        buf_size = 0;
        /* Link as a circle */
        handle->dma.desc[0]->empty = (uint32_t)handle->dma.desc[1];
        handle->dma.desc[1]->empty = (uint32_t)handle->dma.desc[0];
    } else {
        int i;
        for (i = 0; i < handle->dma.cfg.desc_num && buf_size > 0; i++) {
            size_t w_size = buf_size > handle->dma.cfg.buf_size ? handle->dma.cfg.buf_size : buf_size;
            memcpy(handle->dma.bufs[i], buf, w_size);
            lldesc_config(handle->dma.desc[i], LLDESC_HW_OWNED, 1, 0, w_size);
            handle->dma.desc[i]->empty = (uint32_t)(i < handle->dma.cfg.desc_num - 1 ? handle->dma.desc[i+1] :0);
            buf_size -= w_size;
            buf += w_size;
        }
        handle->dma.desc[i-1]->empty = (uint32_t)(handle->dma.desc[0]);
    }
    /* Wait for the previous DMA stop */
    while (handle->dma.is_running) {}
    handle->dma.is_cyclic = true;
    dac_dma_periph_dma_trans_start((uint32_t)handle->dma.desc[0]);
    handle->dma.is_running = true;
    if (bytes_loaded) {
        *bytes_loaded = src_buf_size - buf_size;
    }
    if (buf_size > 0) {
        ESP_LOGW(TAG, "The cyclic buffer size exceeds the total DMA buffer size: %d(desc_num) * %d(buf_size) = %d",
                    handle->dma.cfg.desc_num, handle->dma.cfg.buf_size, handle->dma.cfg.buf_size * handle->dma.cfg.desc_num);
    }

    xSemaphoreGive(handle->mutex);
    return ret;
}

/*--------------------------------------------------------------------------
                        DAC cosine wave outputting APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_channels_init_cosine_mode(dac_channels_handle_t handle, const dac_cosine_config_t *cw_cfg)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(cw_cfg);
    ESP_RETURN_ON_FALSE((handle->state == DAC_STATE_INITIAL) | (handle->state == DAC_STATE_OUTPUT_READY),
                            ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has been initialized already");
    ESP_RETURN_ON_FALSE(cw_cfg->freq_hz >= 130, ESP_ERR_NOT_SUPPORTED, TAG, "The cosine wave generator doesn't support frequency below 130 Hz");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_driver", &handle->pm_lock), err, TAG, "Failed to create DAC pm lock");
#endif
    portENTER_CRITICAL(&dac_spinlock);
    dac_channel_info_t *p;
    SLIST_FOREACH(p, &handle->head, next) {
        /* Connect DAC module to cosine wave generator */
        dac_ll_cw_set_channel(p->id, true);
        /* Set coefficients for cosine wave generator */
        dac_ll_cw_set_freq(cw_cfg->freq_hz, periph_rtc_dig_clk8m_get_freq());
        dac_ll_cw_set_scale(p->id, cw_cfg->scale);
        dac_ll_cw_set_phase(p->id, cw_cfg->phase == DAC_COSINE_PHASE_180 ? DAC_LL_CW_PHASE_180 : DAC_LL_CW_PHASE_0);
        dac_ll_cw_set_dc_offset(p->id, cw_cfg->offset);
    }
    portEXIT_CRITICAL(&dac_spinlock);
    handle->state = DAC_STATE_CW_READY;
#if CONFIG_PM_ENABLE
err:
#endif
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t dac_channels_deinit_cosine_mode(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This set of DAC channels is still running or not working at cosine wave mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
        handle->pm_lock = NULL;
    }
#endif
    portENTER_CRITICAL(&dac_spinlock);
    dac_channel_info_t *p;
    SLIST_FOREACH(p, &handle->head, next) {
        /* Disonnect DAC module to cosine wave generator */
        dac_ll_cw_set_channel(p->id, false);
    }
    portEXIT_CRITICAL(&dac_spinlock);
    if (handle->is_enabled) {
        handle->state = DAC_STATE_OUTPUT_READY;
    } else {
        handle->state = DAC_STATE_INITIAL;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channels_start_cosine_output(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This set of DAC channels has started already or not working at cosine wave mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_cw_generator_enable();
    portEXIT_CRITICAL(&dac_spinlock);
    handle->state = DAC_STATE_CW_RUNNING;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channels_stop_cosine_output(dac_channels_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This set of DAC channels has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_RUNNING, ESP_ERR_INVALID_STATE, TAG,
                            "This set of DAC channels has stopped already or not working at cosine wave mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_cw_generator_disable();
    portEXIT_CRITICAL(&dac_spinlock);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_release(handle->pm_lock);
#endif
    handle->state = DAC_STATE_CW_READY;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}
