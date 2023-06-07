/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_check.h"
#include "hal/touch_sensor_hal.h"  //TODO: remove hal
#include "touch_element/touch_element_private.h"

#include "esp_rom_sys.h"


#define TE_CLASS_ITEM(cls, cls_type, cls_item)  ((&((cls)[cls_type]))->cls_item)

#define TE_CLASS_FOREACH(cls_var, cls_start, cls_end)                         \
    for ((cls_var) = (cls_start);                                             \
        (cls_var) < (cls_end);                                                \
        (cls_var)++)

#define TE_CLS_METHODS_INITIALIZER(cls, cls_start, cls_end)  do {             \
    typeof(cls_start) cls_method;                                             \
    TE_CLASS_FOREACH(cls_method, cls_start, cls_end) {                        \
        TE_CLASS_ITEM(cls, cls_method, handle) = NULL;                        \
    }                                                                         \
} while (0)

#define TE_CLASS_FOREACH_CHECK_CHANNEL(cls, cls_start, cls_end, channel) ({   \
    bool ret = false;                                                         \
    typeof(cls_start) cls_method;                                             \
    TE_CLASS_FOREACH(cls_method, cls_start, cls_end) {                        \
        if (TE_CLASS_ITEM(cls, cls_method, handle) != NULL) {                 \
            ret |= TE_CLASS_ITEM(cls, cls_method, check_channel(channel));    \
        }                                                                     \
    }                                                                         \
    ret;                                                                      \
})

#define TE_CLASS_FOREACH_SET_THRESHOLD(cls, cls_start, cls_end) do {          \
        typeof(cls_start) cls_method;                                         \
        TE_CLASS_FOREACH(cls_method, cls_start, cls_end) {                    \
            if (TE_CLASS_ITEM(cls, cls_method, handle) != NULL) {             \
                TE_CLASS_ITEM(cls, cls_method, set_threshold());              \
            }                                                                 \
        }                                                                     \
} while (0)

#define TE_CLASS_FOREACH_PROCESS_STATE(cls, cls_start, cls_end) do {          \
        typeof(cls_start) cls_method;                                         \
        TE_CLASS_FOREACH(cls_method, cls_start, cls_end) {                    \
            if (TE_CLASS_ITEM(cls, cls_method, handle) != NULL) {             \
                TE_CLASS_ITEM(cls, cls_method, process_state());              \
            }                                                                 \
        }                                                                     \
} while (0)

#define TE_CLASS_FOREACH_UPDATE_STATE(cls, cls_start, cls_end, channel, state) do {\
        typeof(cls_start) cls_method;                                         \
        TE_CLASS_FOREACH(cls_method, cls_start, cls_end) {                    \
            if (TE_CLASS_ITEM(cls, cls_method, handle) != NULL) {             \
                TE_CLASS_ITEM(cls, cls_method, update_state(channel, state)); \
            }                                                                 \
        }                                                                     \
} while (0)

#define TE_PROCESSING_PERIOD(obj)                 ((obj)->global_config->software.processing_period)
#define TE_WATERPROOF_DIVIDER(obj)                ((obj)->global_config->software.waterproof_threshold_divider)

typedef enum {
    TE_INTR_PRESS = 0,          //Touch sensor press interrupt(TOUCH_PAD_INTR_MASK_ACTIVE)
    TE_INTR_RELEASE,            //Touch sensor release interrupt(TOUCH_PAD_INTR_MASK_INACTIVE)
    TE_INTR_TIMEOUT,            //Touch sensor scan timeout interrupt(TOUCH_PAD_INTR_MASK_TIMEOUT)
    TE_INTR_SCAN_DONE,          //Touch sensor scan done interrupt(TOUCH_PAD_INTR_MASK_SCAN_DONE), now just use for setting threshold
    TE_INTR_MAX
} te_intr_t;

typedef struct {
    te_intr_t intr_type;                //channel interrupt type
    te_state_t channel_state;           //channel state
    touch_pad_t channel_num;            //channel index
} te_intr_msg_t;

typedef struct {
    te_object_methods_t object_methods[TE_CLS_TYPE_MAX];    //Class(object) methods
    touch_elem_global_config_t *global_config;              //Global initialization
    te_waterproof_handle_t waterproof_handle;               //Waterproof configuration
    te_sleep_handle_t sleep_handle;
    esp_timer_handle_t proc_timer;                          //Processing timer handle
    QueueHandle_t event_msg_queue;                          //Application event message queue (for user)
    QueueHandle_t intr_msg_queue;                           //Interrupt message (for internal)
    SemaphoreHandle_t mutex;                                //Global resource mutex
    bool is_set_threshold;                                  //Threshold configuration state bit
    uint32_t denoise_channel_raw;                           //De-noise channel(TO) raw signal
} te_obj_t;

static te_obj_t *s_te_obj = NULL;
RTC_FAST_ATTR uint32_t threshold_shadow[TOUCH_PAD_MAX - 1] = {0};

/**
 * Internal de-noise channel(Touch channel 0) equivalent capacitance table, depends on hardware design
 *
 * Units: pF
 */
static const float denoise_channel_equ_cap[TOUCH_PAD_DENOISE_CAP_MAX] = {5.0f, 6.4f, 7.8f, 9.2f, 10.6f, 12.0f, 13.4f, 14.8f};

/**
 * Waterproof shield channel(Touch channel 14) equivalent capacitance table, depends on hardware design
 *
 * Units: pF
 */
static const float shield_channel_ref_cap[TOUCH_PAD_SHIELD_DRV_MAX] = {40.0f, 80.0f, 120.0f, 160.0f, 200.0f, 240.0f, 280.0f, 320.0f};

/* -------------------------------------------- Internal shared methods --------------------------------------------- */
/*                                 -------------------------------------------------                                  */
/* ------------------------------------------------- System methods ------------------------------------------------- */
static esp_err_t te_hw_init(const touch_elem_hw_config_t *hardware_init);
static esp_err_t te_sw_init(const touch_elem_sw_config_t *software_init);
static inline float te_get_internal_equ_cap(touch_pad_denoise_cap_t denoise_level);
static float te_channel_get_equ_cap(touch_pad_t channel_num);
static uint32_t te_read_raw_signal(touch_pad_t channel_num);
static void te_intr_cb(void *arg);
static void te_proc_timer_cb(void *arg);
static inline esp_err_t te_object_set_threshold(void);
static inline void te_object_process_state(void);
static inline void te_object_update_state(te_intr_msg_t te_intr_msg);
/* ----------------------------------------------- Waterproof methods ----------------------------------------------- */
static inline bool waterproof_check_state(void);
static inline bool waterproof_shield_check_state(void);
static inline bool waterproof_guard_check_state(void);
static bool waterproof_channel_check(touch_pad_t channel_num);
static void waterproof_guard_set_threshold(void);
static void waterproof_guard_update_state(touch_pad_t current_channel, te_state_t current_state);
static touch_pad_shield_driver_t waterproof_get_shield_level(touch_pad_t guard_channel_num);
/* ------------------------------------------------------------------------------------------------------------------ */

esp_err_t touch_element_install(const touch_elem_global_config_t *global_config)
{
    TE_CHECK(s_te_obj == NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(global_config != NULL, ESP_ERR_INVALID_ARG);

    s_te_obj = (te_obj_t *)calloc(1, sizeof(te_obj_t));
    TE_CHECK(s_te_obj != NULL, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_ERR_NO_MEM;
    s_te_obj->global_config = (touch_elem_global_config_t *)calloc(1, sizeof(touch_elem_global_config_t));
    s_te_obj->mutex = xSemaphoreCreateMutex();
    TE_CHECK_GOTO(s_te_obj->global_config != NULL && s_te_obj->mutex != NULL, cleanup);
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    TE_CLS_METHODS_INITIALIZER(s_te_obj->object_methods, TE_CLS_TYPE_BUTTON, TE_CLS_TYPE_MAX);
    ret = te_hw_init(&global_config->hardware);
    if (ret != ESP_OK) {
        abort();
    }
    ret = te_sw_init(&global_config->software);
    if (ret != ESP_OK) {
        xSemaphoreGive(s_te_obj->mutex);
        goto cleanup;
    }
    xSemaphoreGive(s_te_obj->mutex);
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(s_te_obj->global_config);
    if (s_te_obj->mutex != NULL) {
        vSemaphoreDelete(s_te_obj->mutex);
    }
    TE_FREE_AND_NULL(s_te_obj);
    return ret;
}

esp_err_t touch_element_start(void)
{
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    esp_err_t ret;
    uint16_t inited_channel_mask;
    do {
        xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
        ret = touch_pad_get_channel_mask(&inited_channel_mask);
        if (inited_channel_mask == 0x0) {
            ESP_LOGE(TE_TAG, "Can not find Touch Sensor channel that has been initialized");
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        if (ret != ESP_OK) {
            break;
        }
        s_te_obj->is_set_threshold = false;  //Threshold configuration will be set on touch sense start
        ret = esp_timer_start_periodic(s_te_obj->proc_timer, TE_PROCESSING_PERIOD(s_te_obj) * 1000);
        if (ret != ESP_OK) {
            break;
        }
        ret = touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_SCAN_DONE); //Use scan done interrupt to set threshold
        if (ret != ESP_OK) {
            break;
        }
        ret = touch_pad_fsm_start();
        if (ret != ESP_OK) {
            break;
        }
        xQueueReset(s_te_obj->event_msg_queue);
        xQueueReset(s_te_obj->intr_msg_queue);
        xSemaphoreGive(s_te_obj->mutex);
        return ESP_OK;
    } while (0);

    ESP_LOGE(TE_TAG, "Touch interface start failed:(%s)", __FUNCTION__ );
    xSemaphoreGive(s_te_obj->mutex);
    return ret;
}

esp_err_t touch_element_stop(void)
{
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    esp_err_t ret;
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    ret = touch_pad_fsm_stop();
    if (ret != ESP_OK) {
        return ret;
    }
    ret = touch_pad_intr_disable(TOUCH_PAD_INTR_MASK_SCAN_DONE);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = esp_timer_stop(s_te_obj->proc_timer);
    if (ret != ESP_OK) {
        return ret;
    }
    xSemaphoreGive(s_te_obj->mutex);
    return ESP_OK;
}

//TODO: add a new api that output system's run-time state

void touch_element_uninstall(void)
{
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    if (s_te_obj == NULL) {
        xSemaphoreGive(s_te_obj->mutex);
        return;
    }
    esp_err_t ret;
    ret = touch_pad_deinit();
    if (ret != ESP_OK) {
        abort();
    }
    ret = esp_timer_delete(s_te_obj->proc_timer);
    if (ret != ESP_OK) {
        abort();
    }
    ret = touch_pad_intr_disable(TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE | TOUCH_PAD_INTR_MASK_TIMEOUT);
    if (ret != ESP_OK) {
        abort();
    }
    ret = touch_pad_isr_deregister(te_intr_cb, NULL);
    if (ret != ESP_OK) {
        abort();
    }
    vQueueDelete(s_te_obj->event_msg_queue);
    vQueueDelete(s_te_obj->intr_msg_queue);
    xSemaphoreGive(s_te_obj->mutex);
    vSemaphoreDelete(s_te_obj->mutex);
    free(s_te_obj->global_config);
    s_te_obj->global_config = NULL;
    free(s_te_obj);
    s_te_obj = NULL;
}

esp_err_t touch_element_message_receive(touch_elem_message_t *element_message, uint32_t ticks_to_wait)
{
    //TODO: Use the generic data struct to refactor this api
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(element_message != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(s_te_obj->event_msg_queue != NULL, ESP_ERR_INVALID_STATE);
    int ret = xQueueReceive(s_te_obj->event_msg_queue, element_message, ticks_to_wait);
    return (ret == pdTRUE) ? ESP_OK : ESP_ERR_TIMEOUT;
}

static uint32_t te_read_raw_signal(touch_pad_t channel_num)
{
    uint32_t raw_signal = 0;
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    if (channel_num != sleep_channel_info.touch_num) {
        touch_pad_read_raw_data(channel_num, &raw_signal);
    } else {
        touch_pad_sleep_channel_read_data(channel_num, &raw_signal);
    }
    return raw_signal;
}

uint32_t te_read_smooth_signal(touch_pad_t channel_num)
{
    uint32_t smooth_signal = 0;
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    if (channel_num != sleep_channel_info.touch_num) {
        touch_pad_filter_read_smooth(channel_num, &smooth_signal);
    } else {
        touch_pad_sleep_channel_read_smooth(channel_num, &smooth_signal);
    }
    return smooth_signal;
}

esp_err_t te_event_give(touch_elem_message_t te_message)
{
    //TODO: add queue overwrite here when the queue is full
    int ret = xQueueSend(s_te_obj->event_msg_queue, &te_message, 0);
    if (ret != pdTRUE) {
        ESP_LOGE(TE_TAG, "event queue send failed, event message queue is full");
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

uint32_t te_get_threshold(touch_pad_t channel_num)
{
    uint32_t threshold = 0;
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    if (channel_num != sleep_channel_info.touch_num) {
        touch_pad_get_thresh(channel_num, &threshold);
    } else {
        touch_pad_sleep_get_threshold(channel_num, &threshold);
    }
    return threshold;
}

bool te_is_touch_dsleep_wakeup(void)
{
    soc_reset_reason_t reset_reason = esp_rom_get_reset_reason(0);
    if (reset_reason != RESET_REASON_CORE_DEEP_SLEEP) {
        return false;
    }
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    return wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD;
}

touch_pad_t te_get_sleep_channel(void)
{
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    return sleep_channel_info.touch_num;
}

/**
 * @brief Touch sensor interrupt service routine
 *
 * This function is touch sensor ISR, all the touch
 * sensor channel state will be updated here.
 */
static void te_intr_cb(void *arg)
{
    TE_UNUSED(arg);
    static int scan_done_cnt = 0;
    static uint32_t touch_pre_trig_status = 0;
    int task_awoken = pdFALSE;
    te_intr_msg_t te_intr_msg = {};
    /*< Figure out which touch sensor channel is triggered and the trigger type */
    uint32_t intr_mask = touch_pad_read_intr_status_mask();
    if (intr_mask == 0x0) {  //For dummy interrupt
        return;
    }

    bool need_send_queue = true;
    uint8_t pad_num = 0;
    uint32_t touch_trig_status = touch_pad_get_status();
    uint32_t touch_trig_diff = touch_trig_status ^ touch_pre_trig_status;
    while (touch_trig_diff) {
        if (touch_trig_diff & 0x1) {
            if (touch_trig_status & BIT(pad_num)) {
                if (s_te_obj->sleep_handle != NULL) {
#ifdef CONFIG_PM_ENABLE
                    esp_pm_lock_acquire(s_te_obj->sleep_handle->pm_lock);
#endif
                }
                te_intr_msg.channel_state = TE_STATE_PRESS;
                te_intr_msg.intr_type = TE_INTR_PRESS;
            } else {
                te_intr_msg.channel_state = TE_STATE_RELEASE;
                te_intr_msg.intr_type = TE_INTR_RELEASE;
            }
            touch_pre_trig_status = touch_trig_status;
            te_intr_msg.channel_num = pad_num;
        }
        pad_num++;
        touch_trig_diff >>= 1;
    }

    if (intr_mask & TOUCH_PAD_INTR_MASK_TIMEOUT) {
        te_intr_msg.channel_state = TE_STATE_IDLE;
        te_intr_msg.intr_type = TE_INTR_TIMEOUT;
    } else if (intr_mask & TOUCH_PAD_INTR_MASK_SCAN_DONE) {
        te_intr_msg.channel_state = TE_STATE_IDLE;
        te_intr_msg.intr_type = TE_INTR_SCAN_DONE;
        need_send_queue = false;
        /*< Due to a hardware issue, all of the data read operation(read raw, read smooth, read benchmark) */
        /*< must be after the second times of measure_done interrupt. */
        if (++scan_done_cnt >= 5) {
            touch_hal_intr_disable(TOUCH_PAD_INTR_MASK_SCAN_DONE);  //TODO: remove hal
            scan_done_cnt = 0;
            need_send_queue = true;
        }
        /*< De-noise channel signal must be read at the time between SCAN_DONE and next measurement beginning(sleep)!!! */
        touch_pad_denoise_read_data(&s_te_obj->denoise_channel_raw); //Update de-noise signal
    }
    if (need_send_queue) {
        xQueueSendFromISR(s_te_obj->intr_msg_queue, &te_intr_msg, &task_awoken);
    }
    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief esp-timer callback routine
 *
 * This function is an esp-timer daemon routine, all the touch sensor
 * application(button, slider, etc...) will be processed in here.
 *
 */
static void te_proc_timer_cb(void *arg)
{
    TE_UNUSED(arg);
    te_intr_msg_t te_intr_msg;
    te_intr_msg.intr_type = TE_INTR_MAX;
    BaseType_t ret = xSemaphoreTake(s_te_obj->mutex, 0);
    if (ret != pdPASS) {
        return;
    }
    ret = xQueueReceive(s_te_obj->intr_msg_queue, &te_intr_msg, 0);
    if (ret == pdPASS) {
        if (te_intr_msg.intr_type == TE_INTR_PRESS || te_intr_msg.intr_type == TE_INTR_RELEASE) {
            te_object_update_state(te_intr_msg);
            if ((s_te_obj->sleep_handle != NULL) && (te_intr_msg.intr_type == TE_INTR_RELEASE)) {
#ifdef CONFIG_PM_ENABLE
                esp_pm_lock_release(s_te_obj->sleep_handle->pm_lock);
#endif
            }
        } else if (te_intr_msg.intr_type == TE_INTR_SCAN_DONE) {
            if (s_te_obj->is_set_threshold != true) {
                s_te_obj->is_set_threshold = true;
                te_object_set_threshold();  //TODO: add set threshold error processing
                ESP_LOGD(TE_DEBUG_TAG, "Set threshold");
                if (s_te_obj->sleep_handle != NULL) {
#ifdef CONFIG_PM_ENABLE
                    esp_pm_lock_release(s_te_obj->sleep_handle->pm_lock);
#endif
                }
            }
            if (waterproof_check_state()) {
                te_waterproof_handle_t waterproof_handle = s_te_obj->waterproof_handle;
                if (waterproof_handle->is_shield_level_set != true) {
                    waterproof_handle->is_shield_level_set = true;
                    touch_pad_waterproof_t wp_conf;
                    wp_conf.shield_driver = waterproof_get_shield_level(waterproof_handle->shield_channel);
                    wp_conf.guard_ring_pad = (waterproof_guard_check_state() ? waterproof_handle->guard_device->channel : TOUCH_WATERPROOF_GUARD_NOUSE);
                    touch_pad_waterproof_set_config(&wp_conf);
                    touch_pad_waterproof_enable();
                    ESP_LOGD(TE_DEBUG_TAG, "Set waterproof shield level");
                }
            }
            ESP_LOGD(TE_DEBUG_TAG, "read denoise channel %"PRIu32, s_te_obj->denoise_channel_raw);
        } else if (te_intr_msg.intr_type == TE_INTR_TIMEOUT) { //Timeout processing
            touch_pad_timeout_resume();
        }
    }
    te_object_process_state();
    xSemaphoreGive(s_te_obj->mutex);
}

void te_object_method_register(te_object_methods_t *object_methods, te_class_type_t object_type)
{
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, handle) = object_methods->handle;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, check_channel) = object_methods->check_channel;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, set_threshold) = object_methods->set_threshold;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, process_state) = object_methods->process_state;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, update_state) = object_methods->update_state;
    xSemaphoreGive(s_te_obj->mutex);
}

void te_object_method_unregister(te_class_type_t object_type)
{
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, handle) = NULL;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, check_channel) = NULL;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, set_threshold) = NULL;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, process_state) = NULL;
    TE_CLASS_ITEM(s_te_obj->object_methods, object_type, update_state) = NULL;
    xSemaphoreGive(s_te_obj->mutex);
}

/**
 * @brief Touch Sense channel check
 *
 * This function will check the input channel whether is
 * associated with the Touch Sense Object
 *
 * @return
 *      - true:  Channel has been initialized, pls adjust the input channel
 *      - false: Channel has not been initialized, pass
 */
bool te_object_check_channel(const touch_pad_t *channel_array, uint8_t channel_sum)
{
    touch_pad_t current_channel;
    for (int idx = 0; idx < channel_sum; idx++) {
        current_channel = channel_array[idx];
        if (waterproof_channel_check(current_channel)) {
            goto INITIALIZED;
        }
        if (TE_CLASS_FOREACH_CHECK_CHANNEL(s_te_obj->object_methods, TE_CLS_TYPE_BUTTON, TE_CLS_TYPE_MAX, current_channel)) {
            goto INITIALIZED;
        }
    }
    return false;

INITIALIZED:
    ESP_LOGE(TE_TAG, "Current channel [%d] has been initialized:(%s)", current_channel, __FUNCTION__ );
    return true;
}


static inline esp_err_t te_object_set_threshold(void)
{
    if (waterproof_guard_check_state() == true) {   //TODO: add to object methods
        waterproof_guard_set_threshold();
    }

    TE_CLASS_FOREACH_SET_THRESHOLD(s_te_obj->object_methods, TE_CLS_TYPE_BUTTON, TE_CLS_TYPE_MAX);
    return ESP_OK;
}

static inline void te_object_process_state(void)
{
    TE_CLASS_FOREACH_PROCESS_STATE(s_te_obj->object_methods, TE_CLS_TYPE_BUTTON, TE_CLS_TYPE_MAX);
}

static inline void te_object_update_state(te_intr_msg_t te_intr_msg)
{
    if (waterproof_guard_check_state()) {
        waterproof_guard_update_state(te_intr_msg.channel_num, te_intr_msg.channel_state);
    }
    TE_CLASS_FOREACH_UPDATE_STATE(s_te_obj->object_methods, TE_CLS_TYPE_BUTTON, TE_CLS_TYPE_MAX,
                                  te_intr_msg.channel_num, te_intr_msg.channel_state);
}

uint8_t te_get_timer_period(void)
{
    return (TE_PROCESSING_PERIOD(s_te_obj));
}

esp_err_t te_dev_init(te_dev_t **device, uint8_t device_num, te_dev_type_t type, const touch_pad_t *channel, const float *sens, float divider)
{
    for (int idx = 0; idx < device_num; idx++) {
        device[idx]->channel = channel[idx];
        device[idx]->sens = sens[idx] * divider;
        device[idx]->type = type;
        device[idx]->state = TE_STATE_IDLE;
        device[idx]->is_use_last_threshold = false;
        esp_err_t ret = touch_pad_config(device[idx]->channel);
        TE_CHECK(ret == ESP_OK, ret);
    }
    return ESP_OK;
}

void te_dev_deinit(te_dev_t **device, uint8_t device_num)
{
    for (int idx = 0; idx < device_num; idx++) {
        touch_pad_clear_channel_mask((1UL << device[idx]->channel));
    }
}

static esp_err_t te_config_thresh(touch_pad_t channel_num, uint32_t threshold)
{
    esp_err_t ret;
    touch_pad_sleep_channel_t sleep_channel_info;
    touch_pad_sleep_channel_get_info(&sleep_channel_info);
    if (channel_num != sleep_channel_info.touch_num) {
        ret = touch_pad_set_thresh(channel_num, threshold);
    } else {
        ret = touch_pad_sleep_set_threshold(channel_num, threshold);
    }
    return ret;
}

esp_err_t te_dev_set_threshold(te_dev_t *device)
{
    esp_err_t ret = ESP_OK;
    uint32_t smo_val = 0;

    if (s_te_obj->sleep_handle && device->is_use_last_threshold) {
        if (te_is_touch_dsleep_wakeup()) {  //Deep sleep wakeup reset
            ret = te_config_thresh(device->channel, s_te_obj->sleep_handle->non_volatile_threshold[device->channel - 1]);
        } else {  //Other reset
            smo_val = te_read_smooth_signal(device->channel);
            ret = te_config_thresh(device->channel, device->sens * smo_val);
            uint32_t threshold = te_get_threshold(device->channel);
            s_te_obj->sleep_handle->non_volatile_threshold[device->channel - 1] = threshold;  //Write threshold into RTC Fast Memory
        }
    } else {
        smo_val = te_read_smooth_signal(device->channel);
        ret = te_config_thresh(device->channel, device->sens * smo_val);
    }
    ESP_LOGD(TE_DEBUG_TAG, "channel: %"PRIu8", smo_val: %"PRIu32, (uint8_t)device->channel, smo_val);
    return ret;
}

/**
 * This function returns the s_te_obj whether is initialized
 *
 * @return
 *      - true: initialized
 *      - false: not initialized
 */
bool te_system_check_state(void)
{
    return (s_te_obj != NULL);
}

static inline float te_get_internal_equ_cap(touch_pad_denoise_cap_t denoise_level)
{
    return denoise_channel_equ_cap[denoise_level];
}

/**
 * @brief   Get channel equivalent capacitance
 *
 * This function calculates the equivalent capacitance of input channel by
 * using the Touch channel 0 equivalent capacitance. The formula is:
 *
 *                      Raw_N / Raw_0 = Cap_N / Cap_0
 *
 * Note that Raw_N and Raw_0 are the raw data of touch channel N and touch channel 0 respectively,
 * Cap_N and Cap_0 are the equivalent capacitance of touch channel N and touch channel 0.
 *
 * @param[in] channel_num   Input touch sensor channel
 *
 * @note The unit is pF
 *
 * @return  Specified channel equivalent capacitance.
 */
static float te_channel_get_equ_cap(touch_pad_t channel_num)
{
    //Fixme: add a mutex in here and prevent the system call this function
    TE_CHECK(channel_num > TOUCH_PAD_NUM0 && channel_num < TOUCH_PAD_MAX, 0);
    uint32_t tn_raw, t0_raw;
    float tn_ref_cap, t0_ref_cap;
    touch_pad_denoise_t denoise_channel_conf;
    touch_pad_denoise_get_config(&denoise_channel_conf);
    tn_raw = te_read_raw_signal(channel_num);
    t0_raw = s_te_obj->denoise_channel_raw;
    t0_ref_cap = te_get_internal_equ_cap(denoise_channel_conf.cap_level);
    if (t0_raw == 0) {
        return 0;
    }
    tn_ref_cap = (float)tn_raw / t0_raw * t0_ref_cap;
    return tn_ref_cap;
}

/**
 * @brief  Touch sensor driver default init [ESP32S2 only]
 *
 * 1. Channel measure time: Raw_value / RTC_FAST_CLK  ==> Raw_value / 8000 000
 * 2. Channel sleep time: TOUCH_PAD_SLEEP_CYCLE_DEFAULT / RTC_SLOW_CLK ==> 0xf / 90 000(default) = 0.16ms
 * 3. Channel charge voltage threshold(upper/lower):  2.7V upper voltage, 0.5V lower voltage, 0.5V attenuation voltage
 * 4. IDLE channel processing:  Connecting to GND
 * 5. Interrupt type:  ACTIVE, INACTIVE, TIMEOUT
 *
 * @note A touch sensor channel will spend the time = measure time + sleep time, RTC_FAST_CLK is 8M
 *
 */
static esp_err_t te_hw_init(const touch_elem_hw_config_t *hardware_init)
{
    esp_err_t ret;
    ret = touch_pad_init();
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_set_measurement_interval(hardware_init->sleep_cycle);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_set_charge_discharge_times(hardware_init->sample_count);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_set_voltage(hardware_init->upper_voltage, hardware_init->lower_voltage,
                                 hardware_init->voltage_attenuation);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_set_idle_channel_connect(hardware_init->suspend_channel_polarity);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_isr_register(te_intr_cb, NULL,
                                 TOUCH_PAD_INTR_MASK_ACTIVE | TOUCH_PAD_INTR_MASK_INACTIVE |
                                 TOUCH_PAD_INTR_MASK_TIMEOUT | TOUCH_PAD_INTR_MASK_SCAN_DONE);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_intr_enable(TOUCH_PAD_INTR_MASK_ACTIVE |
                                 TOUCH_PAD_INTR_MASK_INACTIVE | TOUCH_PAD_INTR_MASK_TIMEOUT);
    TE_CHECK(ret == ESP_OK, ret);

    /*< Internal de-noise configuration */
    touch_pad_denoise_t denoise_config;
    denoise_config.grade = hardware_init->denoise_level;
    denoise_config.cap_level = hardware_init->denoise_equivalent_cap;
    ret = touch_pad_denoise_set_config(&denoise_config);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_denoise_enable();
    TE_CHECK(ret == ESP_OK, ret);

    /*< benchmark filter configuration */
    touch_filter_config_t filter_config;
    filter_config.smh_lvl = hardware_init->smooth_filter_mode;
    filter_config.mode = hardware_init->benchmark_filter_mode;
    filter_config.debounce_cnt = hardware_init->benchmark_debounce_count;
    filter_config.noise_thr = hardware_init->benchmark_calibration_threshold;
    filter_config.jitter_step = hardware_init->benchmark_jitter_step;
    ret = touch_pad_filter_set_config(&filter_config);
    TE_CHECK(ret == ESP_OK, ret);
    ret = touch_pad_filter_enable();
    TE_CHECK(ret == ESP_OK, ret);
    memcpy(&s_te_obj->global_config->hardware, hardware_init, sizeof(touch_elem_hw_config_t));
    return ESP_OK;
}

static esp_err_t te_sw_init(const touch_elem_sw_config_t *software_init)
{
    TE_CHECK(software_init->processing_period > 1, ESP_ERR_INVALID_ARG);
    TE_CHECK(software_init->waterproof_threshold_divider > 0, ESP_ERR_INVALID_ARG);
    TE_CHECK(software_init->intr_message_size >= (TOUCH_PAD_MAX - 1), ESP_ERR_INVALID_ARG);
    TE_CHECK(software_init->event_message_size > 0, ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_ERR_NO_MEM;
    s_te_obj->intr_msg_queue = xQueueCreate(software_init->intr_message_size, sizeof(te_intr_msg_t));
    s_te_obj->event_msg_queue = xQueueCreate(software_init->event_message_size, sizeof(touch_elem_message_t));
    TE_CHECK_GOTO(s_te_obj->event_msg_queue != NULL && s_te_obj->intr_msg_queue != NULL, cleanup);

    const esp_timer_create_args_t te_proc_timer_args = {
        .name = "te_proc_timer_cb",
        .arg  = NULL,
        .callback = &te_proc_timer_cb,
        .skip_unhandled_events = true,
    };
    ret = esp_timer_create(&te_proc_timer_args, &s_te_obj->proc_timer);
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    memcpy(&s_te_obj->global_config->software, software_init, sizeof(touch_elem_sw_config_t));
    return ret;

cleanup:
    if (s_te_obj->event_msg_queue != NULL) {
        vQueueDelete(s_te_obj->event_msg_queue);
    }
    if (s_te_obj->intr_msg_queue != NULL) {
        vQueueDelete(s_te_obj->intr_msg_queue);
    }
    return ret;
}

//TODO: add waterproof guard-lock hysteresis
esp_err_t touch_element_waterproof_install(const touch_elem_waterproof_config_t *waterproof_config)
{
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(waterproof_config != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(waterproof_config->guard_channel >= TOUCH_PAD_NUM0 &&
             waterproof_config->guard_channel < TOUCH_PAD_MAX,
             ESP_ERR_INVALID_ARG);
    te_waterproof_handle_t waterproof_handle = (te_waterproof_handle_t)calloc(1, sizeof(struct te_waterproof_s));
    TE_CHECK(waterproof_handle != NULL, ESP_ERR_NO_MEM);
    waterproof_handle->shield_channel = TOUCH_PAD_NUM14;

    esp_err_t ret;
    if (waterproof_config->guard_channel != TOUCH_WATERPROOF_GUARD_NOUSE) { //Use guard sensor
        if (te_object_check_channel(&waterproof_config->guard_channel, 1)) {
            ret = ESP_ERR_INVALID_ARG;
            goto cleanup;
        }
        ret = ESP_ERR_NO_MEM;
        waterproof_handle->mask_handle = (touch_elem_handle_t *) calloc(TOUCH_PAD_MAX, sizeof(touch_elem_handle_t));
        waterproof_handle->guard_device = (te_dev_t *)calloc(1, sizeof(te_dev_t));
        TE_CHECK_GOTO(waterproof_handle->mask_handle != NULL && waterproof_handle->guard_device, cleanup);

        ret = te_dev_init(&waterproof_handle->guard_device, 1, TOUCH_ELEM_TYPE_BUTTON,
                          &waterproof_config->guard_channel, &waterproof_config->guard_sensitivity,
                          TE_WATERPROOF_DIVIDER(s_te_obj));
        TE_CHECK_GOTO(ret == ESP_OK, cleanup);
        waterproof_handle->guard_device->state = TE_STATE_RELEASE;
        for (int idx = 0; idx < TOUCH_PAD_MAX; idx++) {
            waterproof_handle->mask_handle[idx] = NULL;
        }
    } else {  //No use waterproof guard sensor
        waterproof_handle->guard_device = NULL;
        waterproof_handle->mask_handle = NULL;
    }
    waterproof_handle->is_shield_level_set = 0; //Set a state bit so as to configure the shield level at the run-time
    touch_pad_waterproof_t wp_conf;
    wp_conf.shield_driver = TOUCH_PAD_SHIELD_DRV_L0; //Set a default shield level
    wp_conf.guard_ring_pad = waterproof_config->guard_channel;
    ret = touch_pad_waterproof_set_config(&wp_conf);
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    ret = touch_pad_waterproof_enable();
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    s_te_obj->waterproof_handle = waterproof_handle;  //Fixme: add mutex
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(waterproof_handle->mask_handle);
    TE_FREE_AND_NULL(waterproof_handle->guard_device);
    TE_FREE_AND_NULL(waterproof_handle);
    return ret;
}

esp_err_t touch_element_waterproof_add(touch_elem_handle_t element_handle)
{
    TE_CHECK(s_te_obj->waterproof_handle != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(s_te_obj->waterproof_handle->guard_device != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(element_handle != NULL, ESP_ERR_INVALID_ARG);
    te_waterproof_handle_t waterproof_handle = s_te_obj->waterproof_handle;
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    for (int idx = 0; idx < TOUCH_PAD_MAX; idx++) {
        if (waterproof_handle->mask_handle[idx] == NULL) {
            waterproof_handle->mask_handle[idx] = element_handle;
            break;
        }
    }
    xSemaphoreGive(s_te_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_element_waterproof_remove(touch_elem_handle_t element_handle)
{
    TE_CHECK(s_te_obj->waterproof_handle != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(element_handle != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    te_waterproof_handle_t waterproof_handle = s_te_obj->waterproof_handle;
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    for (int idx = 0; idx < TOUCH_PAD_MAX; idx++) {
        if (waterproof_handle->mask_handle[idx] == element_handle) {
            waterproof_handle->mask_handle[idx] = NULL;
            ret = ESP_OK;
            break;
        }
    }
    xSemaphoreGive(s_te_obj->mutex);
    return ret;
}

void touch_element_waterproof_uninstall(void)
{
    xSemaphoreTake(s_te_obj->mutex, portMAX_DELAY);
    touch_pad_waterproof_disable();
    free(s_te_obj->waterproof_handle->guard_device);
    free(s_te_obj->waterproof_handle->mask_handle);
    free(s_te_obj->waterproof_handle);
    s_te_obj->waterproof_handle = NULL;
    xSemaphoreGive(s_te_obj->mutex);
}

static touch_pad_shield_driver_t waterproof_get_shield_level(touch_pad_t guard_channel_num)
{
    touch_pad_shield_driver_t shield_level = TOUCH_PAD_SHIELD_DRV_L7;
    float guard_ref_cap = te_channel_get_equ_cap(guard_channel_num);
    for (int level = 0; level < TOUCH_PAD_SHIELD_DRV_MAX; level++) {
        if (guard_ref_cap <= shield_channel_ref_cap[level]) {
            shield_level = (touch_pad_shield_driver_t)level;
            break;
        }
    }
    return shield_level;
}

/**
 * This function returns the waterproof_handle whether is initialized
 *
 * @return
 *      - true: initialized
 *      - false: not initialized
 */
static inline bool waterproof_check_state(void)
{
    return (s_te_obj->waterproof_handle != NULL);
}

static inline bool waterproof_shield_check_state(void)
{
    return waterproof_check_state();  //Driver does not allow to disable shield sensor after waterproof enabling
}

static inline bool waterproof_guard_check_state(void)
{
    if (waterproof_check_state() == false) {
        return false;
    }
    if (s_te_obj->waterproof_handle->guard_device == NULL || s_te_obj->waterproof_handle->mask_handle == NULL) {
        return false;
    }
    return true;
}

static bool waterproof_channel_check(touch_pad_t channel_num)
{
    if (waterproof_check_state() == false) {
        return false;
    }
    te_waterproof_handle_t waterproof_handle = s_te_obj->waterproof_handle;
    if (waterproof_shield_check_state()) {
        if (channel_num == waterproof_handle->shield_channel) {
            ESP_LOGE(TE_TAG, "TOUCH_PAD_NUM%"PRIu8" has been used for waterproof shield channel,"
                     " please change the touch sensor channel or disable waterproof", (uint8_t)channel_num);
            return true;
        }
    }
    if (waterproof_guard_check_state()) {
        if (channel_num == waterproof_handle->guard_device->channel) {
            ESP_LOGE(TE_TAG, "TOUCH_PAD_NUM%"PRIu8" has been used for waterproof guard channel,"
                     " please change the touch sensor channel or disable waterproof", (uint8_t)channel_num);
            return true;
        }
    }
    return false;
}

static void waterproof_guard_set_threshold(void)
{
    if (waterproof_check_state() == false) {
        return;
    }
    if (waterproof_guard_check_state() == false) {
        return;
    }
    te_dev_set_threshold(s_te_obj->waterproof_handle->guard_device);
}

/**
 *  This function will figure out current handle whether is a masked channel
 *  while guard channel is triggered.
 *
 * @param[in] te_handle     Touch sensor application handle
 * @return
 *      - true  current handle is a masked channel
 *      - false current handle is not a masked channel
 */
bool waterproof_check_mask_handle(touch_elem_handle_t te_handle)
{
    if (waterproof_check_state() == false) {
        return false;
    }
    if (waterproof_guard_check_state() == false) {
        return false;
    }
    te_waterproof_handle_t waterproof_handle = s_te_obj->waterproof_handle;
    bool ret = false;
    if (waterproof_handle->guard_device->state == TE_STATE_PRESS) {
        for (int idx = 0; idx < TOUCH_PAD_MAX; idx++) {
            if (waterproof_handle->mask_handle[idx] == NULL) {
                break;
            }
            if (waterproof_handle->mask_handle[idx] == te_handle) {
                ret = true;
            }
        }
    }
    return ret;
}

static void waterproof_guard_update_state(touch_pad_t current_channel, te_state_t current_state)
{
    te_dev_t *guard_device = s_te_obj->waterproof_handle->guard_device;
    if (current_channel == guard_device->channel) {
        guard_device->state = current_state;
    }
    ESP_LOGD(TE_DEBUG_TAG, "waterproof guard state update  %d", guard_device->state);
}

esp_err_t touch_element_enable_light_sleep(const touch_elem_sleep_config_t *sleep_config)
{
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(s_te_obj->sleep_handle == NULL, ESP_ERR_INVALID_STATE);
    uint16_t sample_count = 500;
    uint16_t sleep_cycle = 0x0f;
    if (sleep_config) {
        sample_count = sleep_config->sample_count;
        sleep_cycle = sleep_config->sleep_cycle;
    }

    s_te_obj->sleep_handle = calloc(1, sizeof(struct te_sleep_s));
    TE_CHECK(s_te_obj->sleep_handle, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_OK;
    touch_pad_sleep_channel_set_work_time(sleep_cycle, sample_count);
    TE_CHECK_GOTO(esp_sleep_enable_touchpad_wakeup() == ESP_OK,  cleanup);

    TE_CHECK_GOTO(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON) == ESP_OK,  cleanup);
    s_te_obj->sleep_handle->non_volatile_threshold = threshold_shadow;

#ifdef CONFIG_PM_ENABLE
    TE_CHECK_GOTO(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "touch_element", &s_te_obj->sleep_handle->pm_lock) == ESP_OK,  cleanup);
    TE_CHECK_GOTO(esp_pm_lock_acquire(s_te_obj->sleep_handle->pm_lock) == ESP_OK,  cleanup);
#endif

    return ESP_OK;

cleanup:
#ifdef CONFIG_PM_ENABLE
    if (s_te_obj->sleep_handle->pm_lock != NULL) {
        if (esp_pm_lock_delete(s_te_obj->sleep_handle->pm_lock) != ESP_OK) {
            abort();
        }
    }
#endif
    TE_FREE_AND_NULL(s_te_obj->sleep_handle);
    return ret;
}

esp_err_t touch_element_disable_light_sleep(void)
{
    TE_CHECK(s_te_obj->sleep_handle, ESP_ERR_INVALID_STATE);
#ifdef CONFIG_PM_ENABLE
    if (s_te_obj->sleep_handle->pm_lock != NULL) {
        /* Sleep channel is going to uninstall, pm lock is not needed anymore,
           but we need to make sure that pm lock has been released before delete it. */
        while(esp_pm_lock_release(s_te_obj->sleep_handle->pm_lock) == ESP_OK);
        esp_err_t ret = esp_pm_lock_delete(s_te_obj->sleep_handle->pm_lock);
        TE_CHECK(ret == ESP_OK, ret);
        s_te_obj->sleep_handle->pm_lock = NULL;
    }
#endif
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
    TE_FREE_AND_NULL(s_te_obj->sleep_handle);
    return ESP_OK;
}

esp_err_t touch_element_enable_deep_sleep(touch_elem_handle_t wakeup_elem_handle, const touch_elem_sleep_config_t *sleep_config)
{
    TE_CHECK(s_te_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(s_te_obj->sleep_handle == NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(wakeup_elem_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(sleep_config != NULL, ESP_ERR_INVALID_ARG);
    uint16_t sample_count = 500;
    uint16_t sleep_cycle = 0x0f;
    if (sleep_config) {
        sample_count = sleep_config->sample_count;
        sleep_cycle = sleep_config->sleep_cycle;
    }

    s_te_obj->sleep_handle = calloc(1, sizeof(struct te_sleep_s));
    TE_CHECK(s_te_obj->sleep_handle, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_OK;
    touch_pad_sleep_channel_set_work_time(sleep_cycle, sample_count);
    TE_CHECK_GOTO(esp_sleep_enable_touchpad_wakeup() == ESP_OK,  cleanup);

    TE_CHECK_GOTO(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON) == ESP_OK, cleanup);
    s_te_obj->sleep_handle->non_volatile_threshold = threshold_shadow;

#ifdef CONFIG_PM_ENABLE
    TE_CHECK_GOTO(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "touch_element", &s_te_obj->sleep_handle->pm_lock) == ESP_OK,  cleanup);
    TE_CHECK_GOTO(esp_pm_lock_acquire(s_te_obj->sleep_handle->pm_lock) == ESP_OK,  cleanup);
#endif
    //Only support one channel/element as the deep sleep wakeup channel/element
    TE_CHECK(is_button_object_handle(wakeup_elem_handle), ESP_ERR_NOT_SUPPORTED);
    s_te_obj->sleep_handle->wakeup_handle = wakeup_elem_handle;
    te_button_handle_t button_handle = wakeup_elem_handle;
    ret = touch_pad_sleep_channel_enable(button_handle->device->channel, true);
    TE_CHECK(ret == ESP_OK, ret);

    return ESP_OK;

cleanup:
#ifdef CONFIG_PM_ENABLE
    if (s_te_obj->sleep_handle->pm_lock != NULL) {
        if (esp_pm_lock_delete(s_te_obj->sleep_handle->pm_lock) != ESP_OK) {
            abort();
        }
    }
#endif
    TE_FREE_AND_NULL(s_te_obj->sleep_handle);
    return ret;
}

esp_err_t touch_element_disable_deep_sleep(void)
{
    TE_CHECK(s_te_obj->sleep_handle, ESP_ERR_INVALID_STATE);
    esp_err_t ret;
#ifdef CONFIG_PM_ENABLE
    if (s_te_obj->sleep_handle->pm_lock != NULL) {
        /* Sleep channel is going to uninstall, pm lock is not needed anymore,
           but we need to make sure that pm lock has been released before delete it. */
        while(esp_pm_lock_release(s_te_obj->sleep_handle->pm_lock) == ESP_OK);
        ret = esp_pm_lock_delete(s_te_obj->sleep_handle->pm_lock);
        TE_CHECK(ret == ESP_OK, ret);
        s_te_obj->sleep_handle->pm_lock = NULL;
    }
#endif
    te_button_handle_t button_handle = s_te_obj->sleep_handle->wakeup_handle;
    ret = touch_pad_sleep_channel_enable(button_handle->device->channel, false);
    TE_CHECK(ret == ESP_OK, ret);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
    s_te_obj->sleep_handle->wakeup_handle = NULL;
    TE_FREE_AND_NULL(s_te_obj->sleep_handle);
    return ESP_OK;
}

esp_err_t touch_element_sleep_enable_wakeup_calibration(touch_elem_handle_t element_handle, bool en)
{
    TE_CHECK(element_handle != NULL, ESP_ERR_INVALID_ARG);
    if (is_button_object_handle(element_handle)) {
        button_enable_wakeup_calibration(element_handle, en);
    } else if (is_slider_object_handle(element_handle)) {
        slider_enable_wakeup_calibration(element_handle, en);
    } else if (is_matrix_object_handle(element_handle)) {
        matrix_enable_wakeup_calibration(element_handle, en);
    } else {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}
