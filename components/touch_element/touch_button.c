/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/queue.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "touch_element/touch_element_private.h"

typedef struct te_button_handle_list {
    te_button_handle_t button_handle;               //Button handle
    SLIST_ENTRY(te_button_handle_list) next;        //Button handle list entry
} te_button_handle_list_t;

typedef struct {
    SLIST_HEAD(te_button_handle_list_head, te_button_handle_list) handle_list;      //Button handle (instance) list
    touch_button_global_config_t *global_config;                                    //Button global configuration
    SemaphoreHandle_t mutex;                                                        //Button object mutex
} te_button_obj_t;

static te_button_obj_t *s_te_btn_obj = NULL;    //Button object
/* ---------------------------------------- Button handle(instance) methods ----------------------------------------- */
static bool button_channel_check(te_button_handle_t button_handle, touch_pad_t channel_num);
static esp_err_t button_set_threshold(te_button_handle_t button_handle);
static inline te_state_t button_get_state(te_dev_t *device);
static void button_reset_state(te_button_handle_t button_handle);
static void button_update_state(te_button_handle_t button_handle, touch_pad_t channel_num, te_state_t channel_state);
static void button_proc_state(te_button_handle_t button_handle);
static void button_event_give(te_button_handle_t button_handle);
static inline void button_dispatch(te_button_handle_t button_handle, touch_elem_dispatch_t dispatch_method);
/* ------------------------------------------ Button object(class) methods ------------------------------------------ */
static esp_err_t button_object_add_instance(te_button_handle_t button_handle);
static esp_err_t button_object_remove_instance(te_button_handle_t button_handle);
static bool button_object_check_channel(touch_pad_t channel_num);
static esp_err_t button_object_set_threshold(void);
static void button_object_process_state(void);
static void button_object_update_state(touch_pad_t channel_num, te_state_t channel_state);
/* ------------------------------------------------------------------------------------------------------------------ */

esp_err_t touch_button_install(const touch_button_global_config_t *global_config)
{
    TE_CHECK(te_system_check_state() == true, ESP_ERR_INVALID_STATE);
    TE_CHECK(global_config != NULL, ESP_ERR_INVALID_ARG);
    //Fixme: Make it thread-safe
    s_te_btn_obj = (te_button_obj_t *)calloc(1, sizeof(te_button_obj_t));
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_NO_MEM);
    s_te_btn_obj->global_config = (touch_button_global_config_t *)calloc(1, sizeof(touch_button_global_config_t));
    s_te_btn_obj->mutex = xSemaphoreCreateMutex();
    TE_CHECK_GOTO(s_te_btn_obj->global_config != NULL && s_te_btn_obj->global_config != NULL, cleanup);
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    SLIST_INIT(&s_te_btn_obj->handle_list);
    memcpy(s_te_btn_obj->global_config, global_config, sizeof(touch_button_global_config_t));
    te_object_methods_t button_methods = {
        .handle = s_te_btn_obj,
        .check_channel = button_object_check_channel,
        .set_threshold = button_object_set_threshold,
        .process_state = button_object_process_state,
        .update_state = button_object_update_state
    };
    te_object_method_register(&button_methods, TE_CLS_TYPE_BUTTON);
    xSemaphoreGive(s_te_btn_obj->mutex);
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(s_te_btn_obj->global_config);
    if (s_te_btn_obj->mutex != NULL) {
        vSemaphoreDelete(s_te_btn_obj->mutex);
    }
    TE_FREE_AND_NULL(s_te_btn_obj);
    return ESP_ERR_NO_MEM;
}

void touch_button_uninstall(void)
{
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    if (s_te_btn_obj == NULL) {
        xSemaphoreGive(s_te_btn_obj->mutex);
        return;
    }
    te_object_method_unregister(TE_CLS_TYPE_BUTTON);
    free(s_te_btn_obj->global_config);
    s_te_btn_obj->global_config = NULL;
    while (!SLIST_EMPTY(&s_te_btn_obj->handle_list)) {
        SLIST_FIRST(&s_te_btn_obj->handle_list);
        SLIST_REMOVE_HEAD(&s_te_btn_obj->handle_list, next);
    }
    xSemaphoreGive(s_te_btn_obj->mutex);
    vSemaphoreDelete(s_te_btn_obj->mutex);
    free(s_te_btn_obj);
    s_te_btn_obj = NULL;
}

esp_err_t touch_button_create(const touch_button_config_t *button_config, touch_button_handle_t *button_handle)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL && button_config != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(button_config->channel_num > TOUCH_PAD_NUM0 &&
             button_config->channel_num < TOUCH_PAD_MAX &&
             button_config->channel_sens > 0,
             ESP_ERR_INVALID_ARG);
    TE_CHECK(te_object_check_channel(&button_config->channel_num, 1) == false, ESP_ERR_INVALID_ARG);
    te_button_handle_t te_button = (te_button_handle_t)calloc(1, sizeof(struct te_button_s));
    TE_CHECK(te_button != NULL, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_ERR_NO_MEM;
    te_button->config = (te_button_handle_config_t *)calloc(1, sizeof(te_button_handle_config_t));
    te_button->device = (te_dev_t *)calloc(1, sizeof(te_dev_t));
    TE_CHECK_GOTO(te_button->config != NULL && te_button->device != NULL, cleanup);

    ret = te_dev_init(&te_button->device, 1, TOUCH_ELEM_TYPE_BUTTON,
                      &button_config->channel_num, &button_config->channel_sens, TE_DEFAULT_THRESHOLD_DIVIDER(s_te_btn_obj));
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);

    te_button->config->event_mask = TOUCH_ELEM_EVENT_NONE;
    te_button->config->dispatch_method = TOUCH_ELEM_DISP_MAX;
    te_button->config->callback = NULL;
    te_button->config->arg = NULL;
    te_button->current_state = TE_STATE_IDLE;
    te_button->last_state = TE_STATE_IDLE;
    te_button->event = TOUCH_BUTTON_EVT_MAX;
    te_button->trigger_cnt = 0;
    te_button->trigger_thr = 0xffffffff;
    ret = button_object_add_instance(te_button);
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    *button_handle = (touch_elem_handle_t)te_button;
    ESP_LOGD(TE_DEBUG_TAG, "channel: %d, channel_sens: %f", button_config->channel_num, button_config->channel_sens);
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(te_button->config);
    TE_FREE_AND_NULL(te_button->device);
    TE_FREE_AND_NULL(te_button);
    return ret;
}

esp_err_t touch_button_delete(touch_button_handle_t button_handle)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL, ESP_ERR_INVALID_ARG);
    esp_err_t ret = button_object_remove_instance(button_handle);
    TE_CHECK(ret == ESP_OK, ret);
    te_button_handle_t te_button = (te_button_handle_t)button_handle;
    te_dev_deinit(&te_button->device, 1);
    free(te_button->config);
    free(te_button->device);
    free(te_button);
    return ESP_OK;
}

esp_err_t touch_button_set_dispatch_method(touch_button_handle_t button_handle, touch_elem_dispatch_t dispatch_method)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(dispatch_method >= TOUCH_ELEM_DISP_EVENT && dispatch_method <= TOUCH_ELEM_DISP_MAX, ESP_ERR_INVALID_ARG);
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    te_button_handle_t te_button = (te_button_handle_t)button_handle;
    te_button->config->dispatch_method = dispatch_method;
    xSemaphoreGive(s_te_btn_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_button_subscribe_event(touch_button_handle_t button_handle, uint32_t event_mask, void *arg)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL, ESP_ERR_INVALID_ARG);
    if (!(event_mask & TOUCH_ELEM_EVENT_ON_PRESS) && !(event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) &&
        !(event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) && !(event_mask & TOUCH_ELEM_EVENT_NONE)) {
        ESP_LOGE(TE_TAG, "Touch button only support TOUCH_ELEM_EVENT_ON_PRESS, "
                         "TOUCH_ELEM_EVENT_ON_RELEASE, TOUCH_ELEM_EVENT_ON_LONGPRESS event mask");
        return ESP_ERR_INVALID_ARG;
    }
    if (event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) {
        touch_button_set_longpress(button_handle, TE_DEFAULT_LONGPRESS_TIME(s_te_btn_obj));  //set the default time(1000ms) for long press
    }
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    te_button_handle_t te_button = (te_button_handle_t)button_handle;
    te_button->config->event_mask = event_mask;
    te_button->config->arg = arg;
    xSemaphoreGive(s_te_btn_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_button_set_callback(touch_button_handle_t button_handle, touch_button_callback_t button_callback)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(button_callback != NULL, ESP_ERR_INVALID_ARG);
    te_button_handle_t te_button = (te_button_handle_t)button_handle;
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    te_button->config->callback = button_callback;
    xSemaphoreGive(s_te_btn_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_button_set_longpress(touch_button_handle_t button_handle, uint32_t threshold_time)
{
    TE_CHECK(s_te_btn_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(button_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(threshold_time > 0, ESP_ERR_INVALID_ARG);
    te_button_handle_t te_button = (te_button_handle_t)button_handle;
    touch_elem_dispatch_t dispatch_method = te_button->config->dispatch_method;
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    }
    uint8_t timer_period = te_get_timer_period();
    te_button->trigger_thr = threshold_time / timer_period;
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        xSemaphoreGive(s_te_btn_obj->mutex);
    }
    return ESP_OK;
}

const touch_button_message_t* touch_button_get_message(const touch_elem_message_t* element_message)
{
    return (touch_button_message_t*)&element_message->child_msg;
    _Static_assert(sizeof(element_message->child_msg) >= sizeof(touch_button_message_t), "Message size overflow");
}

static bool button_object_check_channel(touch_pad_t channel_num)
{
    te_button_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        if (button_channel_check(item->button_handle, channel_num)) {
            return true;
        }
    }
    return false;
}

static esp_err_t button_object_set_threshold(void)
{
    esp_err_t ret = ESP_OK;
    te_button_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        ret = button_set_threshold(item->button_handle);
        if (ret != ESP_OK) {
            break;
        }
    }
    return ret;
}

static void button_object_process_state(void)
{
    te_button_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->button_handle)) {
            button_reset_state(item->button_handle);
            continue;
        }
        button_proc_state(item->button_handle);
    }
}

static void button_object_update_state(touch_pad_t channel_num, te_state_t channel_state)
{
    te_button_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->button_handle)) {
            continue;
        }
        button_update_state(item->button_handle, channel_num, channel_state);
    }
}

static esp_err_t button_object_add_instance(te_button_handle_t button_handle)
{
    te_button_handle_list_t *item = (te_button_handle_list_t *)calloc(1, sizeof(te_button_handle_list_t));
    TE_CHECK(item != NULL, ESP_ERR_NO_MEM);
    item->button_handle = button_handle;
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    SLIST_INSERT_HEAD(&s_te_btn_obj->handle_list, item, next);
    xSemaphoreGive(s_te_btn_obj->mutex);
    return ESP_OK;
}

static esp_err_t button_object_remove_instance(te_button_handle_t button_handle)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    te_button_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        if (button_handle == item->button_handle) {
            xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
            SLIST_REMOVE(&s_te_btn_obj->handle_list, item, te_button_handle_list, next);
            xSemaphoreGive(s_te_btn_obj->mutex);
            free(item);
            ret = ESP_OK;
            break;
        }
    }
    return ret;
}

bool is_button_object_handle(touch_elem_handle_t element_handle)
{
    te_button_handle_list_t *item;
    xSemaphoreTake(s_te_btn_obj->mutex, portMAX_DELAY);
    SLIST_FOREACH(item, &s_te_btn_obj->handle_list, next) {
        if (element_handle == item->button_handle) {
            xSemaphoreGive(s_te_btn_obj->mutex);
            return true;
        }
    }
    xSemaphoreGive(s_te_btn_obj->mutex);
    return false;
}

static bool button_channel_check(te_button_handle_t button_handle, touch_pad_t channel_num)
{
    return (channel_num == button_handle->device->channel);
}

static esp_err_t button_set_threshold(te_button_handle_t button_handle)
{
    return te_dev_set_threshold(button_handle->device);
}

static void button_update_state(te_button_handle_t button_handle, touch_pad_t channel_num, te_state_t channel_state)
{
    te_dev_t *device = button_handle->device;
    if (channel_num != device->channel) {
        return;
    }
    device->state = channel_state;
}

static void button_reset_state(te_button_handle_t button_handle)
{
    button_handle->trigger_cnt = 0;
    button_handle->current_state = TE_STATE_IDLE;
    button_handle->device->state = TE_STATE_IDLE;
}

static void button_event_give(te_button_handle_t button_handle)
{
    touch_elem_message_t element_message;
    touch_button_message_t button_message = {
        .event = button_handle->event
    };
    element_message.handle = (touch_elem_handle_t)button_handle;
    element_message.element_type = TOUCH_ELEM_TYPE_BUTTON;
    element_message.arg = button_handle->config->arg;
    memcpy(element_message.child_msg, &button_message, sizeof(button_message));
    te_event_give(element_message);
}

static inline void button_dispatch(te_button_handle_t button_handle, touch_elem_dispatch_t dispatch_method)
{
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        button_event_give(button_handle);  //Event queue
    } else if (dispatch_method == TOUCH_ELEM_DISP_CALLBACK) {
        touch_button_message_t button_info;
        button_info.event = button_handle->event;
        button_handle->config->callback(button_handle, &button_info, button_handle->config->arg);  //Event callback
    }
}

void button_enable_wakeup_calibration(te_button_handle_t button_handle, bool en)
{
    button_handle->device->is_use_last_threshold = !en;
}

/**
 * @brief Button process
 *
 * This function will process the button state and maintain a button FSM:
 *              IDLE ----> Press ----> Release ----> IDLE
 *
 * The state transition procedure is as follow:
 *       (channel state ----> button state)
 *
 * TODO: add state transition diagram
 */
static void button_proc_state(te_button_handle_t button_handle)
{
    uint32_t event_mask = button_handle->config->event_mask;
    touch_elem_dispatch_t dispatch_method = button_handle->config->dispatch_method;

    BaseType_t mux_ret = xSemaphoreTake(s_te_btn_obj->mutex, 0);
    if (mux_ret != pdPASS) {
        return;
    }

    button_handle->current_state = button_get_state(button_handle->device);

    if (button_handle->current_state == TE_STATE_PRESS) {
        if (button_handle->last_state == TE_STATE_IDLE) {  //IDLE ---> Press = On_Press
            ESP_LOGD(TE_DEBUG_TAG, "button press");
            if (event_mask & TOUCH_ELEM_EVENT_ON_PRESS) {
                button_handle->event = TOUCH_BUTTON_EVT_ON_PRESS;
                button_dispatch(button_handle, dispatch_method);
            }
        } else if (button_handle->last_state == TE_STATE_PRESS) {  //Press ---> Press = On_LongPress
            if (event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) {
                if (++button_handle->trigger_cnt >= button_handle->trigger_thr) {
                    ESP_LOGD(TE_DEBUG_TAG, "button longpress");
                    button_handle->event = TOUCH_BUTTON_EVT_ON_LONGPRESS;
                    button_dispatch(button_handle, dispatch_method);
                    button_handle->trigger_cnt = 0;
                }
            }
        }
    } else if (button_handle->current_state == TE_STATE_RELEASE) {
        if (button_handle->last_state == TE_STATE_PRESS) {  //Press ---> Release = On_Release
            ESP_LOGD(TE_DEBUG_TAG, "button release");
            if (event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) {
                button_handle->event = TOUCH_BUTTON_EVT_ON_RELEASE;
                button_dispatch(button_handle, dispatch_method);
            }
        } else if (button_handle->last_state == TE_STATE_RELEASE) { // Release ---> Release = On_IDLE (Not dispatch)
            button_reset_state(button_handle); //Reset the button state for the next time touch action detection
        }
    } else if (button_handle->current_state == TE_STATE_IDLE) {
        if (button_handle->last_state == TE_STATE_RELEASE) {  //Release ---> IDLE = On_IDLE (Not dispatch)
            //Nothing
        } else if (button_handle->last_state == TE_STATE_IDLE) { //IDLE ---> IDLE = Running IDLE (Not dispatch)
            //Nothing
        }
    }
    button_handle->last_state = button_handle->current_state;
    xSemaphoreGive(s_te_btn_obj->mutex);
}

static inline te_state_t button_get_state(te_dev_t *device)
{
    te_state_t button_state;
    if (device->state == TE_STATE_PRESS) {
        button_state =  TE_STATE_PRESS;
    } else if (device->state == TE_STATE_RELEASE) {
        button_state = TE_STATE_RELEASE;
    } else {
        button_state = TE_STATE_IDLE;
    }
    return button_state;
}
