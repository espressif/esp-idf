/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "sdkconfig.h"

#include "soc/soc_caps.h"
#include "soc/lldesc.h"
#include "soc/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "driver/rtc_io.h"
#include "driver/dac_new.h"
#include "esp_private/dac_dma.h"

#include "esp_check.h"

#define DAC_DMA_MAX_BUF_SIZE        4092
#if CONFIG_DAC_ISR_IRAM_SAFE
    #define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
    #define DAC_MEM_ALLOC_CAPS      (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#else
    #define DAC_INTR_ALLOC_FLAGS    (ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_SHARED)
    #define DAC_MEM_ALLOC_CAPS      MALLOC_CAP_DEFAULT
#endif
#define DAC_DMA_ALLOC_CAPS    (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)

typedef enum {
    DAC_STATE_INITIAL,              /* Initial state, dac has been registered */
    DAC_STATE_OUTPUT_READY,         /* DAC channels enabled and ready to output constant voltage */
    DAC_STATE_DMA_READY,            /* DMA mode initialized, but not started */
    DAC_STATE_CW_READY,             /* Cosine wave mode is initialized, but not started */
    DAC_STATE_DMA_RUNNING,          /* DAC DMA dma is running, can't switch mode in this stage */
    DAC_STATE_CW_RUNNING,           /* DAC cosine wave generator is running, can't switch mode in this stage */
} dac_state_t;

typedef struct {
    QueueHandle_t       msg_que;
#if CONFIG_DAC_ISR_IRAM_SAFE
    StaticQueue_t       *msg_que_struct;    /*!< Static message queue struct */
    void                *msg_que_storage;   /*!< Static message queue storage */
#endif
    dac_dma_config_t    cfg;
    lldesc_t            **desc;
} dac_dma_t;

typedef struct dac_channel_chain_s {
    dac_channel_t                   id;
    struct dac_channel_chain_s      *next;
} dac_channel_chain_t;

struct dac_channel_group_s {
    uint32_t                        chan_num;
    dac_state_t                     state;
    SemaphoreHandle_t               mutex;
#if CONFIG_DAC_ISR_IRAM_SAFE
    StaticSemaphore_t               *mutex_struct;      /*!< Static mutex struct */
#endif
#if CONFIG_PM_ENABLE
    esp_pm_lock_handle_t            pm_lock;
#endif
    bool                            is_enabled;
    dac_dma_t                       dma;
    dac_cosine_config_t             sw_cfg;
    dac_channel_chain_t             *head;
};

typedef struct {
    dac_channel_chain_t     *chan[SOC_DAC_PERIPH_NUM];
} dac_platform_t;

static const char *TAG = "DAC";

static dac_platform_t s_dac = {
    .chan = {
        [0 ... SOC_DAC_PERIPH_NUM - 1] = NULL
    },
};

/* Global dac spin lock for the whole DAC driver */
portMUX_TYPE dac_spinlock = portMUX_INITIALIZER_UNLOCKED;  // TODO: check rtc_spinlock

#define DAC_NULL_POINTER_CHECK(p)     ESP_RETURN_ON_FALSE((p), ESP_ERR_INVALID_ARG, TAG, "input parameter '"#p"' is NULL")

static void dac_free_dma_desc(dac_channel_group_handle_t handle)
{
    if (handle->dma.desc == NULL) {
        return;
    }
    for (int i = 0; i < handle->dma.cfg.desc_num; i++) {
        if (handle->dma.desc[i]) {
            free(handle->dma.desc[i]);
            handle->dma.desc[i] = NULL;
        }
    }
    free(handle->dma.desc);
    handle->dma.desc = NULL;
}

static esp_err_t dac_alloc_dma_desc(dac_channel_group_handle_t handle)
{
    esp_err_t ret = ESP_OK;

    handle->dma.desc = (lldesc_t **) heap_caps_calloc(handle->dma.cfg.desc_num, sizeof(lldesc_t *), DAC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(handle->dma.desc, ESP_ERR_NO_MEM, TAG, "Faild to allocate dma descriptor buffer");
    for (int cnt = 0; cnt < handle->dma.cfg.desc_num; cnt++) {
        /* Allocate DMA descpriptor */
        handle->dma.desc[cnt] = (lldesc_t *) heap_caps_calloc(1, sizeof(lldesc_t), DAC_DMA_ALLOC_CAPS);
        ESP_GOTO_ON_FALSE(handle->dma.desc[cnt], ESP_ERR_NO_MEM, err, TAG,  "failed to allocate dma descriptor");
    }

    return ESP_OK;
err:
    /* Free DMA buffer if failed to allocate memory */
    dac_free_dma_desc(handle);
    return ret;
}

static void IRAM_ATTR dac_default_intr_handler(void *arg)
{
    dac_channel_group_handle_t handle = (dac_channel_group_handle_t)arg;
    uint32_t dummy;
    BaseType_t need_awoke = pdFALSE;
    BaseType_t tmp;
    if (dac_dma_periph_intr_is_triggered()) {
        lldesc_t *fdesc = (lldesc_t *)dac_dma_periph_intr_get_eof_desc();
        if (xQueueIsQueueFullFromISR(handle->dma.msg_que) == pdTRUE) {
            xQueueReceiveFromISR(handle->dma.msg_que, &dummy, &tmp);
            need_awoke |= tmp;
        }
        xQueueSendFromISR(handle->dma.msg_que, fdesc, &tmp);
        need_awoke |= tmp;
    }
    if (need_awoke == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void dac_free_channel_chain(dac_channel_chain_t *head)
{
    if (head->next) {
        dac_free_channel_chain(head->next);
    }
    s_dac.chan[head->id - 1] = NULL;
    free(head);
}

/*--------------------------------------------------------------------------
                            DAC common APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_new_channel_group(const dac_group_config_t *dac_cfg, dac_channel_group_handle_t *handle)
{
    esp_err_t ret = ESP_OK;

    DAC_NULL_POINTER_CHECK(dac_cfg);
    DAC_NULL_POINTER_CHECK(handle);

    ESP_RETURN_ON_FALSE((uint32_t)dac_cfg->chan_sel > 0, ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel");
    ESP_RETURN_ON_FALSE((32 - __builtin_clz((uint32_t)dac_cfg->chan_sel)) <= SOC_DAC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid DAC channel");

    dac_channel_group_handle_t group = (dac_channel_group_handle_t)calloc(1, sizeof(struct dac_channel_group_s));
    ESP_RETURN_ON_FALSE(group, ESP_ERR_NO_MEM, TAG, "No memory for DAC channel group");
    group->chan_num = 0;
    group->is_enabled = false;
    group->state = DAC_STATE_INITIAL; // Set static output as default
#if CONFIG_DAC_ISR_IRAM_SAFE
    group->mutex_struct = (StaticSemaphore_t *)heap_caps_calloc(1, sizeof(StaticSemaphore_t), DAC_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(group->mutex_struct, ESP_ERR_NO_MEM, err, TAG, "No memory for group mutex struct");
    group->mutex = xSemaphoreCreateMutexStatic(group->mutex_struct);
#else
    group->mutex = xSemaphoreCreateMutex();
#endif
    ESP_GOTO_ON_FALSE(group->mutex, ESP_ERR_NO_MEM, err, TAG, "No memory for group mutex");
#if CONFIG_PM_ENABLE
    ESP_GOTO_ON_ERROR(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "dac_driver", &group->pm_lock), err, TAG, "Failed to create DAC pm lock");
#endif
    /* Register selected channels and link into a chain*/
    dac_channel_chain_t *temp = NULL;
    for (uint32_t msk = (uint32_t)dac_cfg->chan_sel, i = 0; msk != 0; msk >>= 1, i++) {
        if (msk & 0x01) {
            /* Allocate memory for the channel when it is enabled */
            ESP_GOTO_ON_FALSE(!s_dac.chan[i], ESP_ERR_INVALID_STATE, err, TAG, "DAC channel %d has been registered already", i + 1);
            dac_channel_chain_t *node = (dac_channel_chain_t *)calloc(1, sizeof(dac_channel_chain_t));
            ESP_GOTO_ON_FALSE(node, ESP_ERR_NO_MEM, err, TAG, "No memory for DAC channel object");
            node->id = i;
            s_dac.chan[i] = node;
            group->chan_num++;
            /* Link the channel into a chain */
            if (!temp) {
                temp = node;
                group->head = node;
            } else {
                temp->next = node;
                temp = temp->next;
            }
        }
    }

    *handle = group;

    return ret;

err:
    /* Free the resource when error occurs */
    dac_del_channel_group(group);
    group = NULL;

    return ret;
}

esp_err_t dac_del_channel_group(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_INITIAL, ESP_ERR_INVALID_STATE, TAG, "This DAC group is not deinitialized");
    ESP_RETURN_ON_FALSE(!handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group is not disabled");

    if (handle->mutex) {
        vSemaphoreDelete(handle->mutex);
    }
#if CONFIG_DAC_ISR_IRAM_SAFE
    if (handle->mutex_struct) {
        free(handle_mutex_struct);
    }
#endif
#if CONFIG_PM_ENABLE
    if (handle->pm_lock) {
        esp_pm_lock_delete(handle->pm_lock);
        handle->pm_lock = NULL;
    }
#endif
    if (handle->head) {
        dac_free_channel_chain(handle->head);
    }
    free(handle);

    return ESP_OK;
}

esp_err_t dac_channel_group_enable(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(!handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has been enabled already");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

    for (dac_channel_chain_t *p = handle->head; p != NULL; p = p->next) {
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
    /* If the group has not been intialized to other mode, set it `DAC_STATE_OUTPUT_READY` as default */
    if (handle->state == DAC_STATE_INITIAL) {
        handle->state = DAC_STATE_OUTPUT_READY;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channel_group_disable(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled yet");
    ESP_RETURN_ON_FALSE(handle->state < DAC_STATE_DMA_RUNNING, ESP_ERR_INVALID_STATE, TAG, "This DAC group is still running");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    for (dac_channel_chain_t *p = handle->head; p != NULL; p = p->next) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[p->id];
        rtc_gpio_deinit(gpio_num);
        portENTER_CRITICAL(&dac_spinlock);
        dac_ll_power_down(p->id);
        portEXIT_CRITICAL(&dac_spinlock);
    }
    handle->is_enabled = false;
    /* If the group has not been intialized to other mode, set it `DAC_STATE_INITIAL` as default */
    if (handle->state == DAC_STATE_OUTPUT_READY) {
        handle->state = DAC_STATE_INITIAL;
    }
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

/*--------------------------------------------------------------------------
                    DAC constant voltage outputting APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_channel_group_output_constant_voltage(dac_channel_group_handle_t handle, uint8_t value)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_OUTPUT_READY, ESP_ERR_INVALID_STATE, TAG, "This DAC group has been configured to other mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    /* Set the constant voltage for each channel in the group */
    for (dac_channel_chain_t *p = handle->head; p != NULL; p = p->next) {
        portENTER_CRITICAL(&dac_spinlock);
        dac_ll_update_output_value(p->id, value);
        portEXIT_CRITICAL(&dac_spinlock);
    }
    xSemaphoreGive(handle->mutex);
    return ESP_OK;
}

/*--------------------------------------------------------------------------
                          DAC DMA outputting APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_channel_group_init_dma_output(dac_channel_group_handle_t handle, const dac_dma_config_t *dma_cfg)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state < DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG, "This DAC group has been initialized already");

    esp_err_t ret = ESP_OK;

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

#if CONFIG_DAC_ISR_IRAM_SAFE
    handle->dma.msg_que_storage = (uint8_t *)heap_caps_calloc(desc_num - 1, sizeof(lldesc_t *), I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.msg_que_storage, ESP_ERR_NO_MEM, err, TAG, "No memory for message queue storage");
    handle->dma.msg_que_struct = (StaticQueue_t *)heap_caps_calloc(1, sizeof(StaticQueue_t), I2S_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(handle->dma.msg_que_storage, ESP_ERR_NO_MEM, err, TAG, "No memory for message queue struct");
    handle->dma.msg_queue =  xQueueCreateStatic(desc_num - 1, sizeof(lldesc_t *), handle->dma.msg_que_storage, handle->dma.msg_que_struct);
#else
    handle->dma.msg_que = xQueueCreate(dma_cfg->desc_num - 1, sizeof(lldesc_t *));
#endif
    ESP_GOTO_ON_FALSE(handle->dma.msg_que, ESP_ERR_NO_MEM, err3, TAG, "No memory for message queue");
    /* Allocate DMA buffer */
    memcpy(&(handle->dma.cfg), dma_cfg, sizeof(dac_dma_config_t));
    ESP_GOTO_ON_ERROR(dac_alloc_dma_desc(handle), err2, TAG, "Failed to allocate memory for DMA buffers");

    /* Initialize DAC DMA peripheral */
    ESP_GOTO_ON_ERROR(dac_dma_periph_init(handle->chan_num, dma_cfg->freq_hz, dma_cfg->chan_mode == DAC_CHANNEL_ALTERNATE), err2, TAG, "Failed to initialize DAC DMA peripheral");
    /* Register DMA interrupt */
    ESP_GOTO_ON_ERROR(dac_dma_periph_register_intr(dac_default_intr_handler, handle), err1, TAG, "Failed to register DMA interrupt");
    /* Connect DAC module to the DMA peripheral */
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(true);
    portEXIT_CRITICAL(&dac_spinlock);

    handle->state = DAC_STATE_DMA_READY;

    xSemaphoreGive(handle->mutex);

    return ret;
err1:
    dac_dma_periph_deinit();
err2:
    dac_free_dma_desc(handle);
err3:
    if (handle->dma.msg_que) {
        vQueueDelete(handle->dma.msg_que);
    }
#if CONFIG_DAC_ISR_IRAM_SAFE
    if (handle->dma.msq_que_struct) {
        free(handle->dma.msq_que_struct);
    }
    if (handle->dma.msq_que_storage) {
        free(handle->dma.msq_que_storage);
    }
#endif
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t dac_channel_group_deinit_dma_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG,
                           "This DAC group is still running or has been configured to other mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);

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
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channel_group_start_dma_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This DAC group has started already or not working at DMA mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
#ifdef CONFIG_PM_ENABLE
    esp_pm_lock_acquire(handle->pm_lock);
#endif
    dac_dma_periph_enable();
    portENTER_CRITICAL(&dac_spinlock);
    dac_ll_digi_enable_dma(true);
    portEXIT_CRITICAL(&dac_spinlock);
    handle->state = DAC_STATE_DMA_RUNNING;
    xSemaphoreGive(handle->mutex);

    return ESP_OK;
}

esp_err_t dac_channel_group_stop_dma_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_RUNNING, ESP_ERR_INVALID_STATE, TAG,
                            "This DAC group has stopped already or not working at DMA mode");

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

static uint32_t dac_load_dma_data(lldesc_t *desc, uint8_t *data, uint32_t size)
{
    uint32_t byte_to_load =  size > DAC_DMA_MAX_BUF_SIZE ? DAC_DMA_MAX_BUF_SIZE : size;
    desc->owner = 1;
    desc->eof = 1;
    desc->sosf = 0;
    desc->length = byte_to_load;
    desc->size = byte_to_load;
    desc->buf = data;
    desc->offset = 0;
    return byte_to_load;
}
// TODO: wait until all data sent or all data loaded? If all data loaded, need to monitor end of frame
esp_err_t dac_channel_group_write_acyclicly(dac_channel_group_handle_t handle, uint8_t *buf, size_t buf_size, uint32_t timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(buf);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_RUNNING, ESP_ERR_INVALID_STATE, TAG, "This DAC group is not started");
#if CONFIG_DAC_ISR_IRAM_SAFE
    ESP_RETURN_ON_ERROR(esp_ptr_internal(buf), ESP_ERR_INVALID_ARG, err, TAG, "the buffer is not in internal RAM");
#endif

    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex,  pdMS_TO_TICKS(timeout_ms) == pdTRUE), ESP_ERR_TIMEOUT, TAG, "Take semaphore timeout");
    /* Reset the queue to drop the legacy descriptors */
    xQueueReset(handle->dma.msg_que);
    /* Break the legacy descriptor chain for the new data */
    for (int i=0; i < handle->dma.cfg.desc_num; i++) {
        handle->dma.desc[i]->empty = 0;
    }
    /* Pre-load data to DMA */
    size_t index = 0;
    uint32_t pending_desc_cnt = 0;
    for (int i = 0;  i < handle->dma.cfg.desc_num && index < buf_size; i++, pending_desc_cnt++) {
        index += dac_load_dma_data(handle->dma.desc[i], &buf[index], buf_size - index);
        /* Link to the previous descriptor */
        if (i > 0) {
            handle->dma.desc[i-1]->empty = (uint32_t)handle->dma.desc[i];
        }
    }
    /* Link the start and end desc as a ring if the buffer not loaded conmpletely */
    handle->dma.desc[pending_desc_cnt-1]->empty = index < buf_size ? (uint32_t)handle->dma.desc[0] : 0;
    dac_dma_periph_dma_trans_start((uint32_t)handle->dma.desc[0]);
    /* Wait until all data be sent */
    for (lldesc_t *finish_desc = NULL; pending_desc_cnt > 0; pending_desc_cnt--) {
        ESP_GOTO_ON_FALSE(xQueueReceive(handle->dma.msg_que, &finish_desc, pdMS_TO_TICKS(timeout_ms)) == pdTRUE,
                              ESP_ERR_TIMEOUT, err, TAG, "Receive message queue timeout");
        /* Load those unsent data */
        if (index < buf_size) {
            index += dac_load_dma_data(finish_desc, &buf[index], buf_size - index);
            pending_desc_cnt++;
            /* If all date loaded, break the ring desc */
            if (index >= buf_size) {
                finish_desc->empty = 0;
            }
        }
    }
err:
    xSemaphoreGive(handle->mutex);
    return ret;
}

esp_err_t dac_channel_group_write_cyclicly(dac_channel_group_handle_t handle, uint8_t *buf, size_t buf_size, uint32_t timeout_ms)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_DMA_RUNNING, ESP_ERR_INVALID_STATE, TAG, "This DAC group is not started");
    ESP_RETURN_ON_FALSE(buf_size < (DAC_DMA_MAX_BUF_SIZE * handle->dma.cfg.desc_num),
                            ESP_ERR_INVALID_ARG, TAG, "The cyclic buffer size exceeds the total DMA buffer size: desc_num * %d = %d",
                            DAC_DMA_MAX_BUF_SIZE, DAC_DMA_MAX_BUF_SIZE * handle->dma.cfg.desc_num);
#if CONFIG_DAC_ISR_IRAM_SAFE
    ESP_RETURN_ON_ERROR(esp_ptr_internal(buf), ESP_ERR_INVALID_ARG, err, TAG, "the buffer is not in internal RAM");
#endif
    ESP_RETURN_ON_FALSE(xSemaphoreTake(handle->mutex,  pdMS_TO_TICKS(timeout_ms) == pdTRUE), ESP_ERR_TIMEOUT, TAG, "Take semaphore timeout");
    /* If the buffer size is small, split it into two descriptors */
    if (buf_size > DAC_DMA_MAX_BUF_SIZE) {
        size_t index = 0;
        int i = 0;
        for (i = 0; (i < handle->dma.cfg.desc_num) && (index < buf_size); i++) {
            index += dac_load_dma_data(handle->dma.desc[i], &buf[index], buf_size - index);
            /* Link to the previous descriptor */
            if (i > 0) {
                handle->dma.desc[i-1]->empty = (uint32_t)handle->dma.desc[i];
            }
        }
        /* Link as a loop */
        handle->dma.desc[i-1]->empty = (uint32_t)handle->dma.desc[0];
    }
    /* If the buffer size is small, split it into two descriptors */
    else {
        uint32_t half = buf_size / 2;
        dac_load_dma_data(handle->dma.desc[0], buf, half);
        dac_load_dma_data(handle->dma.desc[1], &buf[half], buf_size - half);
        handle->dma.desc[0]->empty = (uint32_t)handle->dma.desc[1];
        handle->dma.desc[1]->empty = (uint32_t)handle->dma.desc[0];
    }
    dac_dma_periph_dma_trans_start((uint32_t)handle->dma.desc[0]);
    xSemaphoreGive(handle->mutex);
    return ESP_OK;
}

/*--------------------------------------------------------------------------
                        DAC cosine wave outputting APIs
 ---------------------------------------------------------------------------*/
esp_err_t dac_channel_group_init_cosine_output(dac_channel_group_handle_t handle, const dac_cosine_config_t *cw_cfg)
{
    DAC_NULL_POINTER_CHECK(handle);
    DAC_NULL_POINTER_CHECK(cw_cfg);
    ESP_RETURN_ON_FALSE((handle->state == DAC_STATE_INITIAL) | (handle->state == DAC_STATE_OUTPUT_READY),
                            ESP_ERR_INVALID_STATE, TAG, "This DAC group has been initialized already");
    ESP_RETURN_ON_FALSE(cw_cfg->freq_hz >= 130, ESP_ERR_NOT_SUPPORTED, TAG, "The cosine wave generator doesn't support frequency below 130 Hz");
    ESP_RETURN_ON_FALSE(cw_cfg->freq_hz <= 55000, ESP_ERR_NOT_SUPPORTED, TAG, "The cosine wave generator doesn't support frequency beyond 55000 Hz");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    portENTER_CRITICAL(&dac_spinlock);
    for (dac_channel_chain_t *p = handle->head; p != NULL; p = p->next) {
        /* Connect DAC module to cosine wave generator */
        dac_ll_cw_set_channel(p->id, true);
        /* Set coefficients for cosine wave generator */
        dac_ll_cw_set_freq(cw_cfg->freq_hz);
        dac_ll_cw_set_scale(p->id, cw_cfg->scale);
        dac_ll_cw_set_phase(p->id, cw_cfg->phase);
        dac_ll_cw_set_dc_offset(p->id, cw_cfg->offset);
    }
    portEXIT_CRITICAL(&dac_spinlock);
    handle->state = DAC_STATE_CW_READY;
    xSemaphoreGive(handle->mutex);
    return ESP_OK;
}

esp_err_t dac_channel_group_deinit_cosine_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This DAC group is still running or not working at cosine wave mode");

    xSemaphoreTake(handle->mutex, portMAX_DELAY);
    portENTER_CRITICAL(&dac_spinlock);
    for (dac_channel_chain_t *p = handle->head; p != NULL; p = p->next) {
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

esp_err_t dac_channel_group_start_cosine_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_READY, ESP_ERR_INVALID_STATE, TAG,
                            "This DAC group has started already or not working at cosine wave mode");

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

esp_err_t dac_channel_group_stop_cosine_output(dac_channel_group_handle_t handle)
{
    DAC_NULL_POINTER_CHECK(handle);
    ESP_RETURN_ON_FALSE(handle->is_enabled, ESP_ERR_INVALID_STATE, TAG, "This DAC group has not been enabled");
    ESP_RETURN_ON_FALSE(handle->state == DAC_STATE_CW_RUNNING, ESP_ERR_INVALID_STATE, TAG,
                            "This DAC group has stopped already or not working at cosine wave mode");

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
