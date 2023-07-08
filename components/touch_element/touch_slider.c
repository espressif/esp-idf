/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/queue.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "touch_element/touch_element_private.h"

#define TE_SLD_DEFAULT_QTF_THR(obj)                       ((obj)->global_config->quantify_lower_threshold)
#define TE_SLD_DEFAULT_POS_FILTER_FACTOR(obj)             ((obj)->global_config->position_filter_factor)
#define TE_SLD_DEFAULT_CALCULATE_CHANNEL(obj)             ((obj)->global_config->calculate_channel_count)
#define TE_SLD_DEFAULT_BCM_UPDATE_TIME(obj)               ((obj)->global_config->benchmark_update_time)
#define TE_SLD_DEFAULT_FILTER_RESET_TIME(obj)             ((obj)->global_config->filter_reset_time)
#define TE_SLD_DEFAULT_POS_FILTER_SIZE(obj)               ((obj)->global_config->position_filter_size)

typedef struct te_slider_handle_list {
    te_slider_handle_t slider_handle;           //Slider handle
    SLIST_ENTRY(te_slider_handle_list) next;    //Slider handle list entry
} te_slider_handle_list_t;

typedef struct {
    SLIST_HEAD(te_slider_handle_list_head, te_slider_handle_list) handle_list;    //Slider handle (instance) list
    touch_slider_global_config_t *global_config;                                  //Slider global configuration
    SemaphoreHandle_t mutex;                                                      //Slider object mutex
} te_slider_obj_t;

te_slider_obj_t *s_te_sld_obj = NULL;
/* ---------------------------------------- Slider handle(instance) methods ----------------------------------------- */
static bool slider_channel_check(te_slider_handle_t slider_handle, touch_pad_t channel_num);
static esp_err_t slider_set_threshold(te_slider_handle_t slider_handle);
static inline te_state_t slider_get_state(te_dev_t **device, int device_num);
static void slider_reset_state(te_slider_handle_t slider_handle);
static void slider_update_position(te_slider_handle_t slider_handle);
static void slider_reset_position(te_slider_handle_t slider_handle);
static void slider_update_benchmark(te_slider_handle_t slider_handle);
static void slider_update_state(te_slider_handle_t slider_handle, touch_pad_t channel_num, te_state_t channel_state);
static void slider_proc_state(te_slider_handle_t slider_handle);
static void slider_event_give(te_slider_handle_t slider_handle);
static inline void slider_dispatch(te_slider_handle_t slider_handle, touch_elem_dispatch_t dispatch_method);
/* ------------------------------------------ Slider object(class) methods ------------------------------------------ */
static esp_err_t slider_object_add_instance(te_slider_handle_t slider_handle);
static esp_err_t slider_object_remove_instance(te_slider_handle_t slider_handle);
static bool slider_object_check_channel(touch_pad_t channel_num);
static esp_err_t slider_object_set_threshold(void);
static void slider_object_process_state(void);
static void slider_object_update_state(touch_pad_t channel_num, te_state_t channel_state);
/* ------------------------------------------------------------------------------------------------------------------ */

esp_err_t touch_slider_install(const touch_slider_global_config_t *global_config)
{
    TE_CHECK(te_system_check_state() == true, ESP_ERR_INVALID_STATE);
    TE_CHECK(global_config != NULL, ESP_ERR_INVALID_ARG);
    //Fixme: Make it thread-safe
    s_te_sld_obj = (te_slider_obj_t *)calloc(1, sizeof(te_slider_obj_t));
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_NO_MEM);
    s_te_sld_obj->global_config = (touch_slider_global_config_t *)calloc(1, sizeof(touch_slider_global_config_t));
    s_te_sld_obj->mutex = xSemaphoreCreateMutex();
    TE_CHECK_GOTO(s_te_sld_obj->global_config != NULL && s_te_sld_obj->mutex != NULL, cleanup);
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    SLIST_INIT(&s_te_sld_obj->handle_list);
    memcpy(s_te_sld_obj->global_config, global_config, sizeof(touch_slider_global_config_t));
    te_object_methods_t slider_methods = {
        .handle = s_te_sld_obj,
        .check_channel = slider_object_check_channel,
        .set_threshold = slider_object_set_threshold,
        .process_state = slider_object_process_state,
        .update_state = slider_object_update_state
    };
    te_object_method_register(&slider_methods, TE_CLS_TYPE_SLIDER);
    xSemaphoreGive(s_te_sld_obj->mutex);
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(s_te_sld_obj->global_config);
    if (s_te_sld_obj->mutex != NULL) {
        vSemaphoreDelete(s_te_sld_obj->mutex);
    }
    TE_FREE_AND_NULL(s_te_sld_obj);
    return ESP_ERR_NO_MEM;
}

void touch_slider_uninstall(void)
{
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    if (s_te_sld_obj == NULL) {
        xSemaphoreGive(s_te_sld_obj->mutex);
        return;
    }
    te_object_method_unregister(TE_CLS_TYPE_SLIDER);
    free(s_te_sld_obj->global_config);
    s_te_sld_obj->global_config = NULL;
    while (!SLIST_EMPTY(&s_te_sld_obj->handle_list)) {
        SLIST_FIRST(&s_te_sld_obj->handle_list);
        SLIST_REMOVE_HEAD(&s_te_sld_obj->handle_list, next);
    }
    xSemaphoreGive(s_te_sld_obj->mutex);
    vSemaphoreDelete(s_te_sld_obj->mutex);
    free(s_te_sld_obj);
    s_te_sld_obj = NULL;
}

esp_err_t touch_slider_create(const touch_slider_config_t *slider_config, touch_slider_handle_t *slider_handle)
{
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(slider_handle != NULL && slider_config != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(slider_config->channel_num > 2 &&
             slider_config->channel_num < TOUCH_PAD_MAX &&
             slider_config->channel_array != NULL &&
             slider_config->sensitivity_array != NULL &&
             slider_config->position_range > slider_config->channel_num,
             ESP_ERR_INVALID_ARG);
    TE_CHECK(te_object_check_channel(slider_config->channel_array, slider_config->channel_num) == false,
             ESP_ERR_INVALID_ARG);
    te_slider_handle_t te_slider = (te_slider_handle_t)calloc(1, sizeof(struct te_slider_s));
    TE_CHECK(te_slider != NULL, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_ERR_NO_MEM;
    te_slider->config = (te_slider_handle_config_t *)calloc(1, sizeof(te_slider_handle_config_t));
    te_slider->pos_filter_window = calloc(TE_SLD_DEFAULT_POS_FILTER_SIZE(s_te_sld_obj), sizeof(uint8_t));
    te_slider->device = (te_dev_t **)calloc(slider_config->channel_num, sizeof(te_dev_t *));
    te_slider->channel_bcm = (uint32_t *)calloc(slider_config->channel_num, sizeof(uint32_t));
    te_slider->quantify_signal_array = (float *)calloc(slider_config->channel_num, sizeof(float));
    TE_CHECK_GOTO(te_slider->config != NULL &&
                  te_slider->pos_filter_window != NULL &&
                  te_slider->device != NULL &&
                  te_slider->channel_bcm &&
                  te_slider->quantify_signal_array,
                  cleanup);
    for (int idx = 0; idx < slider_config->channel_num; idx++) {
        te_slider->device[idx] = (te_dev_t *)calloc(1, sizeof(te_dev_t));
        if (te_slider->device[idx] == NULL) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }
    ret = te_dev_init(te_slider->device, slider_config->channel_num, TOUCH_ELEM_TYPE_SLIDER,
                      slider_config->channel_array, slider_config->sensitivity_array,
                      TE_DEFAULT_THRESHOLD_DIVIDER(s_te_sld_obj));
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);

    te_slider->config->event_mask = TOUCH_ELEM_EVENT_NONE;
    te_slider->config->dispatch_method = TOUCH_ELEM_DISP_MAX;
    te_slider->config->callback = NULL;
    te_slider->config->arg = NULL;
    te_slider->channel_bcm_update_cnt = TE_SLD_DEFAULT_BCM_UPDATE_TIME(s_te_sld_obj); //update at first time
    te_slider->filter_reset_cnt = TE_SLD_DEFAULT_FILTER_RESET_TIME(s_te_sld_obj);     //reset at first time
    te_slider->channel_sum = slider_config->channel_num;
    te_slider->position_range = slider_config->position_range;
    te_slider->position_scale = (float)(slider_config->position_range) / (slider_config->channel_num - 1);
    te_slider->current_state = TE_STATE_IDLE;
    te_slider->last_state = TE_STATE_IDLE;
    te_slider->event = TOUCH_SLIDER_EVT_MAX;
    te_slider->position = 0;
    te_slider->last_position = 0;
    te_slider->pos_window_idx = 0;
    te_slider->is_first_sample = true;
    ret = slider_object_add_instance(te_slider);
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    *slider_handle = (touch_elem_handle_t)te_slider;
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(te_slider->config);
    TE_FREE_AND_NULL(te_slider->pos_filter_window);
    TE_FREE_AND_NULL(te_slider->channel_bcm);
    TE_FREE_AND_NULL(te_slider->quantify_signal_array);
    if (te_slider->device != NULL) {
        for (int idx = 0; idx < slider_config->channel_num; idx++) {
            TE_FREE_AND_NULL(te_slider->device[idx]);
        }
        free(te_slider->device);
        te_slider->device = NULL;
    }
    TE_FREE_AND_NULL(te_slider);
    return ret;
}

esp_err_t touch_slider_delete(touch_slider_handle_t slider_handle)
{
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(slider_handle != NULL, ESP_ERR_INVALID_ARG);
    /*< Release touch sensor application resource */
    esp_err_t ret = slider_object_remove_instance(slider_handle);
    TE_CHECK(ret == ESP_OK, ret);
    te_slider_handle_t te_slider = (te_slider_handle_t)slider_handle;
    /*< Release touch sensor device resource */
    te_dev_deinit(te_slider->device, te_slider->channel_sum);
    for (int idx = 0; idx < te_slider->channel_sum; idx++) {
        free(te_slider->device[idx]);
    }
    free(te_slider->config);
    free(te_slider->quantify_signal_array);
    free(te_slider->pos_filter_window);
    free(te_slider->channel_bcm);
    free(te_slider->device);
    free(te_slider);
    return ESP_OK;
}

esp_err_t touch_slider_set_dispatch_method(touch_slider_handle_t slider_handle, touch_elem_dispatch_t dispatch_method)
{
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(slider_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(dispatch_method >= TOUCH_ELEM_DISP_EVENT && dispatch_method <= TOUCH_ELEM_DISP_MAX, ESP_ERR_INVALID_ARG);
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    te_slider_handle_t te_slider = (te_slider_handle_t)slider_handle;
    te_slider->config->dispatch_method = dispatch_method;
    xSemaphoreGive(s_te_sld_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_slider_subscribe_event(touch_slider_handle_t slider_handle, uint32_t event_mask, void *arg)
{
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(slider_handle != NULL, ESP_ERR_INVALID_ARG);
    if (!(event_mask & TOUCH_ELEM_EVENT_ON_PRESS) && !(event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) &&
        !(event_mask & TOUCH_ELEM_EVENT_NONE) && !(event_mask & TOUCH_ELEM_EVENT_ON_CALCULATION)) {
        ESP_LOGE(TE_TAG, "Touch button only support TOUCH_ELEM_EVENT_ON_PRESS, "
                         "TOUCH_ELEM_EVENT_ON_RELEASE, TOUCH_ELEM_EVENT_ON_CALCULATION event mask");
        return ESP_ERR_INVALID_ARG;
    }
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    te_slider_handle_t te_slider = (te_slider_handle_t)slider_handle;
    te_slider->config->event_mask = event_mask;
    te_slider->config->arg = arg;
    xSemaphoreGive(s_te_sld_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_slider_set_callback(touch_slider_handle_t slider_handle, touch_slider_callback_t slider_callback)
{
    TE_CHECK(s_te_sld_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(slider_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(slider_callback != NULL, ESP_ERR_INVALID_ARG);
    te_slider_handle_t te_slider = (te_slider_handle_t)slider_handle;
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    te_slider->config->callback = slider_callback;
    xSemaphoreGive(s_te_sld_obj->mutex);
    return ESP_OK;
}

const touch_slider_message_t* touch_slider_get_message(const touch_elem_message_t* element_message)
{
    return (touch_slider_message_t*)&element_message->child_msg;
    _Static_assert(sizeof(element_message->child_msg) >= sizeof(touch_slider_message_t), "Message size overflow");
}

static bool slider_object_check_channel(touch_pad_t channel_num)
{
    te_slider_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        if (slider_channel_check(item->slider_handle, channel_num)) {
            return true;
        }
    }
    return false;
}

static esp_err_t slider_object_set_threshold(void)
{
    esp_err_t ret = ESP_OK;
    te_slider_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        ret = slider_set_threshold(item->slider_handle);
        if (ret != ESP_OK) {
            break;
        }
    }
    return ret;
}

// workaround for compilation error on xtensa-esp32s3-elf-gcc (crosstool-NG esp-2022r1-RC1) 11.2.0 (IDF-5725)
__attribute__((optimize("-Os")))
static void slider_object_process_state(void)
{
    te_slider_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->slider_handle)) {
            slider_reset_state(item->slider_handle);
            slider_reset_position(item->slider_handle);
            continue;
        }
        slider_proc_state(item->slider_handle);
    }
}

static void slider_object_update_state(touch_pad_t channel_num, te_state_t channel_state)
{
    te_slider_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->slider_handle)) {
            continue;
        }
        slider_update_state(item->slider_handle, channel_num, channel_state);
    }
}

static esp_err_t slider_object_add_instance(te_slider_handle_t slider_handle)
{
    te_slider_handle_list_t *item = (te_slider_handle_list_t *)calloc(1, sizeof(te_slider_handle_list_t));
    TE_CHECK(item != NULL, ESP_ERR_NO_MEM);
    item->slider_handle = slider_handle;
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    SLIST_INSERT_HEAD(&s_te_sld_obj->handle_list, item, next);
    xSemaphoreGive(s_te_sld_obj->mutex);
    return ESP_OK;
}

static esp_err_t slider_object_remove_instance(te_slider_handle_t slider_handle)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    te_slider_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        if (slider_handle == item->slider_handle) {
            xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
            SLIST_REMOVE(&s_te_sld_obj->handle_list, item, te_slider_handle_list, next);
            xSemaphoreGive(s_te_sld_obj->mutex);
            free(item);
            ret = ESP_OK;
            break;
        }
    }
    return ret;
}

bool is_slider_object_handle(touch_elem_handle_t element_handle)
{
    te_slider_handle_list_t *item;
    xSemaphoreTake(s_te_sld_obj->mutex, portMAX_DELAY);
    SLIST_FOREACH(item, &s_te_sld_obj->handle_list, next) {
        if (element_handle == item->slider_handle) {
            xSemaphoreGive(s_te_sld_obj->mutex);
            return true;
        }
    }
    xSemaphoreGive(s_te_sld_obj->mutex);
    return false;
}

static bool slider_channel_check(te_slider_handle_t slider_handle, touch_pad_t channel_num)
{
    te_dev_t *device;
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        device = slider_handle->device[idx];
        if (device->channel == channel_num) {
            return true;
        }
    }
    return false;
}

static esp_err_t slider_set_threshold(te_slider_handle_t slider_handle)
{
    esp_err_t ret = ESP_OK;
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        ret |= te_dev_set_threshold(slider_handle->device[idx]);
    }
    slider_update_benchmark(slider_handle);  //Update benchmark at startup
    return ret;
}

static void slider_update_benchmark(te_slider_handle_t slider_handle)
{
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        uint32_t bcm_val;
        te_dev_t *device = slider_handle->device[idx];
        bcm_val = te_read_smooth_signal(device->channel);
        slider_handle->channel_bcm[idx] = bcm_val;
    }
}

static void slider_update_state(te_slider_handle_t slider_handle, touch_pad_t channel_num, te_state_t channel_state)
{
    te_dev_t *device;
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        device = slider_handle->device[idx];
        if (channel_num == device->channel) {
            device->state = channel_state;
        }
    }
}

static void slider_reset_state(te_slider_handle_t slider_handle)
{
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        slider_handle->device[idx]->state = TE_STATE_IDLE;
    }
    slider_handle->current_state = TE_STATE_IDLE;
}

static void slider_event_give(te_slider_handle_t slider_handle)
{
    touch_elem_message_t element_message;
    touch_slider_message_t slider_message = {
        .event = slider_handle->event,
        .position = slider_handle->position
    };
    element_message.handle = (touch_elem_handle_t)slider_handle;
    element_message.element_type = TOUCH_ELEM_TYPE_SLIDER;
    element_message.arg = slider_handle->config->arg;
    memcpy(element_message.child_msg, &slider_message, sizeof(slider_message));
    te_event_give(element_message);
}

static inline void slider_dispatch(te_slider_handle_t slider_handle, touch_elem_dispatch_t dispatch_method)
{
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        slider_event_give(slider_handle);  //Event queue
    } else if (dispatch_method == TOUCH_ELEM_DISP_CALLBACK) {
        touch_slider_message_t slider_info;
        slider_info.event = slider_handle->event;
        slider_info.position = slider_handle->position;
        void *arg = slider_handle->config->arg;
        slider_handle->config->callback(slider_handle, &slider_info, arg);  //Event callback
    }
}

void slider_enable_wakeup_calibration(te_slider_handle_t slider_handle, bool en)
{
    for (int idx = 0; idx < slider_handle->channel_sum; ++idx) {
        slider_handle->device[idx]->is_use_last_threshold = !en;
    }
}

/**
 * @brief Slider process
 *
 * This function will process the slider state and maintain a slider FSM:
 *              IDLE ----> Press ----> Release ----> IDLE
 *
 * The state transition procedure is as follow:
 *       (channel state ----> slider state)
 *
 * TODO: add state transition diagram
 */
static void slider_proc_state(te_slider_handle_t slider_handle)
{
    uint32_t event_mask = slider_handle->config->event_mask;
    touch_elem_dispatch_t dispatch_method = slider_handle->config->dispatch_method;
    BaseType_t mux_ret = xSemaphoreTake(s_te_sld_obj->mutex, 0);
    if (mux_ret != pdPASS) {
        return;
    }

    slider_handle->current_state = slider_get_state(slider_handle->device, slider_handle->channel_sum);

    if (slider_handle->current_state == TE_STATE_PRESS) {
        slider_handle->channel_bcm_update_cnt = 0;  // Reset benchmark update counter
        slider_update_position(slider_handle);
        if (slider_handle->last_state == TE_STATE_IDLE) { //IDLE ---> Press = On_Press
            ESP_LOGD(TE_DEBUG_TAG, "slider press");
            if (event_mask & TOUCH_ELEM_EVENT_ON_PRESS) {
                slider_handle->event = TOUCH_SLIDER_EVT_ON_PRESS;
                slider_dispatch(slider_handle, dispatch_method);
            }
        } else if (slider_handle->last_state == TE_STATE_PRESS) { //Press ---> Press = On_Calculation
            ESP_LOGD(TE_DEBUG_TAG, "slider calculation");
            if (event_mask & TOUCH_ELEM_EVENT_ON_CALCULATION) {
                slider_handle->event = TOUCH_SLIDER_EVT_ON_CALCULATION;
                slider_dispatch(slider_handle, dispatch_method);
            }
        }
    } else if (slider_handle->current_state == TE_STATE_RELEASE) {
        if (slider_handle->last_state == TE_STATE_PRESS) { //Press ---> Release = On_Release
            ESP_LOGD(TE_DEBUG_TAG, "slider release");
            if (event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) {
                slider_handle->event = TOUCH_SLIDER_EVT_ON_RELEASE;
                slider_dispatch(slider_handle, dispatch_method);
            }
        } else if (slider_handle->last_state == TE_STATE_RELEASE) {  // Release ---> Release = On_IDLE (Not dispatch)
            slider_reset_state(slider_handle);//Reset the slider state for the next time touch action detection
        }
    } else if (slider_handle->current_state == TE_STATE_IDLE) {
        if (slider_handle->last_state == TE_STATE_RELEASE) { //Release ---> IDLE = On_IDLE (Not dispatch)
            //Nothing
        } else if (slider_handle->last_state == TE_STATE_IDLE)  { //IDLE ---> IDLE = Running IDLE (Not dispatch)
            if (++slider_handle->channel_bcm_update_cnt >= TE_SLD_DEFAULT_BCM_UPDATE_TIME(s_te_sld_obj)) {  //Update channel benchmark
                slider_handle->channel_bcm_update_cnt = 0;
                slider_update_benchmark(slider_handle);
                ESP_LOGD(TE_DEBUG_TAG, "slider bcm update");
            }
            if (++slider_handle->filter_reset_cnt >= TE_SLD_DEFAULT_FILTER_RESET_TIME(s_te_sld_obj)) {
                slider_reset_position(slider_handle);  //Reset slider filter so as to speed up next time position calculation
            }
        }
    }
    slider_handle->last_state = slider_handle->current_state;
    xSemaphoreGive(s_te_sld_obj->mutex);
}

static inline te_state_t slider_get_state(te_dev_t **device, int device_num)
{
    /*< Scan the state of all the slider channel and calculate the number of them if the state is Press*/
    uint8_t press_cnt = 0;
    uint8_t idle_cnt = 0;
    for (int idx = 0; idx < device_num; idx++) {  //Calculate how many channel is pressed
        if (device[idx]->state == TE_STATE_PRESS) {
            press_cnt++;
        } else if (device[idx]->state == TE_STATE_IDLE) {
            idle_cnt++;
        }
    }
    if (press_cnt > 0) {
        return TE_STATE_PRESS;
    } else if (idle_cnt == device_num) {
        return TE_STATE_IDLE;
    } else {
        return TE_STATE_RELEASE;
    }
}

/**
 * @brief Slider channel difference-rate re-quantization
 *
 * This function will re-quantifies the touch sensor slider channel difference-rate
 * so as to make the different size of touch pad in PCB has the same difference value
 *
 */
static inline void slider_quantify_signal(te_slider_handle_t slider_handle)
{
    float weight_sum = 0;
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        te_dev_t *device = slider_handle->device[idx];
        weight_sum += device->sens;
        uint32_t current_signal = te_read_smooth_signal(device->channel);
        int ans = current_signal - slider_handle->channel_bcm[idx];
        float diff_rate = (float)ans / slider_handle->channel_bcm[idx];
        slider_handle->quantify_signal_array[idx] = diff_rate / device->sens;
        if (slider_handle->quantify_signal_array[idx] < TE_SLD_DEFAULT_QTF_THR(s_te_sld_obj)) {
            slider_handle->quantify_signal_array[idx] = 0;
        }
    }
    for (int idx = 0; idx < slider_handle->channel_sum; idx++) {
        te_dev_t *device = slider_handle->device[idx];
        slider_handle->quantify_signal_array[idx] = slider_handle->quantify_signal_array[idx] * weight_sum / device->sens;
    }
}

/**
 * @brief Calculate max sum subarray
 *
 * This function will figure out the max sum subarray from the
 * input array, return the max sum and max sum start index
 *
 */
static inline float slider_search_max_subarray(const float *array, int array_size, int *max_array_idx)
{
    *max_array_idx = 0;
    float max_array_sum = 0;
    float current_array_sum = 0;
    for (int idx = 0; idx <= (array_size - TE_SLD_DEFAULT_CALCULATE_CHANNEL(s_te_sld_obj)); idx++) {
        for (int x = idx; x < idx + TE_SLD_DEFAULT_CALCULATE_CHANNEL(s_te_sld_obj); x++) {
            current_array_sum += array[x];
        }
        if (max_array_sum < current_array_sum) {
            max_array_sum = current_array_sum;
            *max_array_idx = idx;
        }
        current_array_sum = 0;
    }
    return max_array_sum;
}

/**
 * @brief Calculate zero number
 *
 * This function will figure out the number of non-zero items from
 * the subarray
 */
static inline uint8_t slider_get_non_zero_num(const float *array, uint8_t array_idx)
{
    uint8_t zero_cnt = 0;
    for (int idx = array_idx; idx < array_idx + TE_SLD_DEFAULT_CALCULATE_CHANNEL(s_te_sld_obj); idx++) {
        zero_cnt += (array[idx] > 0) ? 1 : 0;
    }
    return zero_cnt;
}

static inline uint32_t slider_calculate_position(te_slider_handle_t slider_handle, int subarray_index, float subarray_sum, int non_zero_num)
{
    int range = slider_handle->position_range;
    int array_size = slider_handle->channel_sum;
    float scale = slider_handle->position_scale;
    const float *array = slider_handle->quantify_signal_array;
    uint32_t position = 0;
    if (non_zero_num == 0) {
        position = slider_handle->position;
    } else if (non_zero_num == 1) {
        for (int index = subarray_index; index < subarray_index + TE_SLD_DEFAULT_CALCULATE_CHANNEL(s_te_sld_obj); index++) {
            if (0 != array[index]) {
                if (index == array_size - 1) {
                    position = range;
                } else {
                    position = (uint32_t)((float)index * scale);
                }
                break;
            }
        }
    } else {
        for (int idx = subarray_index; idx < subarray_index + TE_SLD_DEFAULT_CALCULATE_CHANNEL(s_te_sld_obj); idx++) {
            position += ((float)idx * array[idx]);
        }
        position = position * scale / subarray_sum;
    }
    return position;
}

static uint32_t slider_filter_average(te_slider_handle_t slider_handle, uint32_t current_position)
{
    uint32_t position_average = 0;
    if (slider_handle->is_first_sample) {
        for (int win_idx = 0; win_idx < TE_SLD_DEFAULT_POS_FILTER_SIZE(s_te_sld_obj); win_idx++) {
            slider_handle->pos_filter_window[win_idx] = current_position;  //Preload filter buffer
        }
        slider_handle->is_first_sample = false;
    } else {
        slider_handle->pos_filter_window[slider_handle->pos_window_idx++] = current_position; //Moving average filter
        if (slider_handle->pos_window_idx >= TE_SLD_DEFAULT_POS_FILTER_SIZE(s_te_sld_obj)) {
            slider_handle->pos_window_idx = 0;
        }
    }

    for (int win_idx = 0; win_idx < TE_SLD_DEFAULT_POS_FILTER_SIZE(s_te_sld_obj); win_idx++) { //Moving average filter
        position_average += slider_handle->pos_filter_window[win_idx];
    }
    position_average = (uint32_t)((float)position_average / TE_SLD_DEFAULT_POS_FILTER_SIZE(s_te_sld_obj) + 0.5F);
    return position_average;
}

static inline uint32_t slider_filter_iir(uint32_t in_now, uint32_t out_last, uint32_t k)
{
    if (k == 0) {
        return in_now;
    } else {
        uint32_t out_now = (in_now + (k - 1) * out_last) / k;
        return out_now;
    }
}

/**
 * @brief touch sensor slider position update
 *
 * This function is the core algorithm about touch sensor slider
 * position update, mainly has several steps:
 *      1. Re-quantization
 *      2. Figure out changed channel
 *      3. Calculate position
 *      4. Filter
 *
 */
static void slider_update_position(te_slider_handle_t slider_handle)
{
    int max_array_idx = 0;
    float max_array_sum;
    uint8_t non_zero_num;
    uint32_t current_position;

    slider_quantify_signal(slider_handle);
    max_array_sum = slider_search_max_subarray(slider_handle->quantify_signal_array, slider_handle->channel_sum, &max_array_idx);
    non_zero_num = slider_get_non_zero_num(slider_handle->quantify_signal_array, max_array_idx);
    current_position = slider_calculate_position(slider_handle, max_array_idx, max_array_sum, non_zero_num);
    uint32_t position_average = slider_filter_average(slider_handle, current_position);
    slider_handle->last_position = slider_handle->last_position == 0 ? (position_average << 4) : slider_handle->last_position;
    slider_handle->last_position = slider_filter_iir((position_average << 4), slider_handle->last_position, TE_SLD_DEFAULT_POS_FILTER_FACTOR(s_te_sld_obj));
    slider_handle->position = ((slider_handle->last_position + 8) >> 4);  //(x + 8) >> 4 ---->  (x + 8) / 16 ----> x/16 + 0.5
}

static void slider_reset_position(te_slider_handle_t slider_handle)
{
    slider_handle->is_first_sample = true;
    slider_handle->last_position   = 0;
    slider_handle->pos_window_idx  = 0;
}
