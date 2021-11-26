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

#define TE_MAT_POS_MAX  (0xff)      //!< Matrix button startup position

typedef struct te_matrix_handle_list {
    te_matrix_handle_t matrix_handle;               //Matrix handle
    SLIST_ENTRY(te_matrix_handle_list) next;        //Matrix handle list entry
} te_matrix_handle_list_t;

typedef struct {
    SLIST_HEAD(te_matrix_handle_list_head, te_matrix_handle_list) handle_list;      //Matrix handle (instance) list
    touch_matrix_global_config_t *global_config;                                                   //Matrix global configuration
    SemaphoreHandle_t mutex;                                                        //Matrix object mutex
} te_matrix_obj_t;

te_matrix_obj_t *s_te_mat_obj = NULL;
/* ---------------------------------------- Matrix handle(instance) methods ----------------------------------------- */
static bool matrix_channel_check(te_matrix_handle_t matrix_handle, touch_pad_t channel_num);
static esp_err_t matrix_set_threshold(te_matrix_handle_t matrix_handle);
static inline te_state_t matrix_get_state(te_matrix_state_t x_axis_state, te_matrix_state_t y_axis_state);
static void matrix_reset_state(te_matrix_handle_t matrix_handle);
static void matrix_update_state(te_matrix_handle_t matrix_handle, touch_pad_t channel_num, te_state_t channel_state);
static void matrix_update_position(te_matrix_handle_t matrix_handle, touch_matrix_position_t new_pos);
static void matrix_proc_state(te_matrix_handle_t matrix_handle);
static void matrix_event_give(te_matrix_handle_t matrix_handle);
static inline void matrix_dispatch(te_matrix_handle_t matrix_handle, touch_elem_dispatch_t dispatch_method);
/* ------------------------------------------ Matrix object(class) methods ------------------------------------------ */
static esp_err_t matrix_object_add_instance(te_matrix_handle_t matrix_handle);
static esp_err_t matrix_object_remove_instance(te_matrix_handle_t matrix_handle);
static bool matrix_object_check_channel(touch_pad_t channel_num);
static esp_err_t matrix_object_set_threshold(void);
static void matrix_object_process_state(void);
static void matrix_object_update_state(touch_pad_t channel_num, te_state_t channel_state);
/* ------------------------------------------------------------------------------------------------------------------ */

esp_err_t touch_matrix_install(const touch_matrix_global_config_t *global_config)
{
    TE_CHECK(te_system_check_state() == true, ESP_ERR_INVALID_STATE);
    TE_CHECK(global_config != NULL, ESP_ERR_INVALID_ARG);

    //Fixme: Make it thread-safe
    s_te_mat_obj = (te_matrix_obj_t *)calloc(1, sizeof(te_matrix_obj_t));
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_NO_MEM);
    s_te_mat_obj->global_config = (touch_matrix_global_config_t *)calloc(1, sizeof(touch_matrix_global_config_t));
    s_te_mat_obj->mutex = xSemaphoreCreateMutex();
    TE_CHECK_GOTO(s_te_mat_obj->global_config != NULL && s_te_mat_obj->mutex != NULL, cleanup);
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    SLIST_INIT(&s_te_mat_obj->handle_list);
    memcpy(s_te_mat_obj->global_config, global_config, sizeof(touch_matrix_global_config_t));
    te_object_methods_t matrix_methods = {
        .handle = s_te_mat_obj,
        .check_channel = matrix_object_check_channel,
        .set_threshold = matrix_object_set_threshold,
        .process_state = matrix_object_process_state,
        .update_state = matrix_object_update_state
    };
    te_object_method_register(&matrix_methods, TE_CLS_TYPE_MATRIX);
    xSemaphoreGive(s_te_mat_obj->mutex);
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(s_te_mat_obj->global_config);
    if (s_te_mat_obj->mutex != NULL) {
        vSemaphoreDelete(s_te_mat_obj->mutex);
    }
    TE_FREE_AND_NULL(s_te_mat_obj);
    return ESP_ERR_NO_MEM;
}

void touch_matrix_uninstall(void)
{
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    if (s_te_mat_obj == NULL) {
        xSemaphoreGive(s_te_mat_obj->mutex);
        return;
    }
    te_object_method_unregister(TE_CLS_TYPE_MATRIX);
    free(s_te_mat_obj->global_config);
    s_te_mat_obj->global_config = NULL;
    while (!SLIST_EMPTY(&s_te_mat_obj->handle_list)) {
        SLIST_FIRST(&s_te_mat_obj->handle_list);
        SLIST_REMOVE_HEAD(&s_te_mat_obj->handle_list, next);
    }
    xSemaphoreGive(s_te_mat_obj->mutex);
    vSemaphoreDelete(s_te_mat_obj->mutex);
    free(s_te_mat_obj);
    s_te_mat_obj = NULL;
}

esp_err_t touch_matrix_create(const touch_matrix_config_t *matrix_config, touch_matrix_handle_t *matrix_handle)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL && matrix_config != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(matrix_config->x_channel_array != NULL &&
             matrix_config->y_channel_array != NULL &&
             matrix_config->x_sensitivity_array != NULL &&
             matrix_config->y_sensitivity_array != NULL &&
             matrix_config->x_channel_num > 1 &&
             matrix_config->x_channel_num < TOUCH_PAD_MAX &&
             matrix_config->y_channel_num > 1 &&
             matrix_config->y_channel_num < TOUCH_PAD_MAX,
             ESP_ERR_INVALID_ARG);
    TE_CHECK(te_object_check_channel(matrix_config->x_channel_array, matrix_config->x_channel_num) == false &&
                 te_object_check_channel(matrix_config->y_channel_array, matrix_config->y_channel_num) == false,
             ESP_ERR_INVALID_ARG);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)calloc(1, sizeof(struct te_slider_s));
    TE_CHECK(te_matrix != NULL, ESP_ERR_NO_MEM);

    esp_err_t ret = ESP_ERR_NO_MEM;
    te_matrix->config = (te_matrix_handle_config_t *)calloc(1, sizeof(te_matrix_handle_config_t));
    te_matrix->device = (te_dev_t **)calloc(matrix_config->x_channel_num + matrix_config->y_channel_num, sizeof(te_dev_t *));
    TE_CHECK_GOTO(te_matrix->config != NULL && te_matrix->device != NULL, cleanup);
    for (int idx = 0; idx < matrix_config->x_channel_num + matrix_config->y_channel_num; idx++) {
        te_matrix->device[idx] = (te_dev_t *)calloc(1, sizeof(te_dev_t));
        if (te_matrix->device[idx] == NULL) {
            ret = ESP_ERR_NO_MEM;
            goto cleanup;
        }
    }
    /*< Initialize x-axis */
    ret = te_dev_init(&te_matrix->device[0], matrix_config->x_channel_num, TOUCH_ELEM_TYPE_MATRIX,
                      matrix_config->x_channel_array, matrix_config->x_sensitivity_array,
                      TE_DEFAULT_THRESHOLD_DIVIDER(s_te_mat_obj));
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    /*< Initialize y-axis */
    ret = te_dev_init(&te_matrix->device[matrix_config->x_channel_num], matrix_config->y_channel_num,
                      TOUCH_ELEM_TYPE_MATRIX, matrix_config->y_channel_array, matrix_config->y_sensitivity_array,
                      TE_DEFAULT_THRESHOLD_DIVIDER(s_te_mat_obj));
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);

    te_matrix->config->event_mask = TOUCH_ELEM_EVENT_NONE;
    te_matrix->config->dispatch_method = TOUCH_ELEM_DISP_MAX;
    te_matrix->config->callback = NULL;
    te_matrix->config->arg = NULL;
    te_matrix->current_state = TE_STATE_IDLE;
    te_matrix->last_state = TE_STATE_IDLE;
    te_matrix->event = TOUCH_MATRIX_EVT_MAX;
    te_matrix->x_channel_num = matrix_config->x_channel_num;
    te_matrix->y_channel_num = matrix_config->y_channel_num;
    te_matrix->trigger_cnt = 0;
    te_matrix->trigger_thr = 0xffffffff;
    te_matrix->position.x_axis = TE_MAT_POS_MAX;
    te_matrix->position.y_axis = TE_MAT_POS_MAX;
    te_matrix->position.index = TE_MAT_POS_MAX;
    ret = matrix_object_add_instance(te_matrix);
    TE_CHECK_GOTO(ret == ESP_OK, cleanup);
    *matrix_handle = (touch_elem_handle_t) te_matrix;
    return ESP_OK;

cleanup:
    TE_FREE_AND_NULL(te_matrix->config);
    if (te_matrix->device != NULL) {
        for (int idx = 0; idx < matrix_config->x_channel_num + matrix_config->y_channel_num; idx++) {
            TE_FREE_AND_NULL(te_matrix->device[idx]);
        }
        free(te_matrix->device);
        te_matrix->device = NULL;
    }
    TE_FREE_AND_NULL(te_matrix);
    return ret;
}

esp_err_t touch_matrix_delete(touch_matrix_handle_t matrix_handle)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL, ESP_ERR_INVALID_ARG);
    /*< Release touch sensor application resource */
    esp_err_t ret = matrix_object_remove_instance(matrix_handle);
    TE_CHECK(ret == ESP_OK, ret);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)matrix_handle;
    /*< Release touch sensor device resource */
    te_dev_deinit(te_matrix->device, te_matrix->x_channel_num + te_matrix->y_channel_num);
    for (int idx = 0; idx < te_matrix->x_channel_num + te_matrix->y_channel_num; idx++) {
        free(te_matrix->device[idx]);
    }
    free(te_matrix->config);
    free(te_matrix->device);
    free(te_matrix);
    return ESP_OK;
}

esp_err_t touch_matrix_set_dispatch_method(touch_matrix_handle_t matrix_handle, touch_elem_dispatch_t dispatch_method)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(dispatch_method >= TOUCH_ELEM_DISP_EVENT && dispatch_method <= TOUCH_ELEM_DISP_MAX, ESP_ERR_INVALID_ARG);
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)matrix_handle;
    te_matrix->config->dispatch_method = dispatch_method;
    xSemaphoreGive(s_te_mat_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_matrix_subscribe_event(touch_matrix_handle_t matrix_handle, uint32_t event_mask, void *arg)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL, ESP_ERR_INVALID_ARG);
    if (!(event_mask & TOUCH_ELEM_EVENT_ON_PRESS) && !(event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) &&
        !(event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) && !(event_mask & TOUCH_ELEM_EVENT_NONE)) {
        ESP_LOGE(TE_TAG, "Touch button only support TOUCH_ELEM_EVENT_ON_PRESS, "
                         "TOUCH_ELEM_EVENT_ON_RELEASE, TOUCH_ELEM_EVENT_ON_LONGPRESS event mask");
        return ESP_ERR_INVALID_ARG;
    }
    if (event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) {
        touch_matrix_set_longpress(matrix_handle, TE_DEFAULT_LONGPRESS_TIME(s_te_mat_obj));  //set the default time(1000ms) for long press
    }
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)matrix_handle;
    te_matrix->config->event_mask = event_mask;
    te_matrix->config->arg = arg;
    xSemaphoreGive(s_te_mat_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_matrix_set_callback(touch_matrix_handle_t matrix_handle, touch_matrix_callback_t matrix_callback)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(matrix_callback != NULL, ESP_ERR_INVALID_ARG);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)matrix_handle;
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    te_matrix->config->callback = matrix_callback;
    xSemaphoreGive(s_te_mat_obj->mutex);
    return ESP_OK;
}

esp_err_t touch_matrix_set_longpress(touch_matrix_handle_t matrix_handle, uint32_t threshold_time)
{
    TE_CHECK(s_te_mat_obj != NULL, ESP_ERR_INVALID_STATE);
    TE_CHECK(matrix_handle != NULL, ESP_ERR_INVALID_ARG);
    TE_CHECK(threshold_time > 0, ESP_ERR_INVALID_ARG);
    te_matrix_handle_t te_matrix = (te_matrix_handle_t)matrix_handle;
    touch_elem_dispatch_t dispatch_method = te_matrix->config->dispatch_method;
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    }
    uint8_t timer_period = te_get_timer_period();
    te_matrix->trigger_thr = threshold_time / timer_period;
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        xSemaphoreGive(s_te_mat_obj->mutex);
    }
    return ESP_OK;
}

const touch_matrix_message_t* touch_matrix_get_message(const touch_elem_message_t* element_message)
{
    return (touch_matrix_message_t*)&element_message->child_msg;
    _Static_assert(sizeof(element_message->child_msg) >= sizeof(touch_matrix_message_t), "Message size overflow");
}

static bool matrix_object_check_channel(touch_pad_t channel_num)
{
    te_matrix_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        if (matrix_channel_check(item->matrix_handle, channel_num)) {
            return true;
        }
    }
    return false;
}

static esp_err_t matrix_object_set_threshold(void)
{
    esp_err_t ret = ESP_OK;
    te_matrix_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        ret = matrix_set_threshold(item->matrix_handle);
        if (ret != ESP_OK) {
            break;
        }
    }
    return ret;
}

static void matrix_object_process_state(void)
{
    te_matrix_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->matrix_handle)) {
            matrix_reset_state(item->matrix_handle);
            continue;
        }
        matrix_proc_state(item->matrix_handle);
    }
}

static void matrix_object_update_state(touch_pad_t channel_num, te_state_t channel_state)
{
    te_matrix_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        if (waterproof_check_mask_handle(item->matrix_handle)) {
            continue;
        }
        matrix_update_state(item->matrix_handle, channel_num, channel_state);
    }
}

static esp_err_t matrix_object_add_instance(te_matrix_handle_t matrix_handle)
{
    te_matrix_handle_list_t *item = (te_matrix_handle_list_t *)calloc(1, sizeof(te_matrix_handle_list_t));
    TE_CHECK(item != NULL, ESP_ERR_NO_MEM);
    item->matrix_handle = matrix_handle;
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    SLIST_INSERT_HEAD(&s_te_mat_obj->handle_list, item, next);
    xSemaphoreGive(s_te_mat_obj->mutex);
    return ESP_OK;
}

static esp_err_t matrix_object_remove_instance(te_matrix_handle_t matrix_handle)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    te_matrix_handle_list_t *item;
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        if (matrix_handle == item->matrix_handle) {
            xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
            SLIST_REMOVE(&s_te_mat_obj->handle_list, item, te_matrix_handle_list, next);
            xSemaphoreGive(s_te_mat_obj->mutex);
            free(item);
            ret = ESP_OK;
            break;
        }
    }
    return ret;
}

bool is_matrix_object_handle(touch_elem_handle_t element_handle)
{
    te_matrix_handle_list_t *item;
    xSemaphoreTake(s_te_mat_obj->mutex, portMAX_DELAY);
    SLIST_FOREACH(item, &s_te_mat_obj->handle_list, next) {
        if (element_handle == item->matrix_handle) {
            xSemaphoreGive(s_te_mat_obj->mutex);
            return true;
        }
    }
    xSemaphoreGive(s_te_mat_obj->mutex);
    return false;
}

static bool matrix_channel_check(te_matrix_handle_t matrix_handle, touch_pad_t channel_num)
{
    te_dev_t *device;
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
        device = matrix_handle->device[idx];
        if (device->channel == channel_num) {
            return true;
        }
    }
    return false;
}

static esp_err_t matrix_set_threshold(te_matrix_handle_t matrix_handle)
{
    esp_err_t ret = ESP_OK;
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
        ret |= te_dev_set_threshold(matrix_handle->device[idx]);
    }
    return ret;
}

static void matrix_update_state(te_matrix_handle_t matrix_handle, touch_pad_t channel_num, te_state_t channel_state)
{
    te_dev_t *device;
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
        device = matrix_handle->device[idx];
        if (channel_num == device->channel) {
            device->state = channel_state;
        }
    }
}

static void matrix_reset_state(te_matrix_handle_t matrix_handle)
{
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
        matrix_handle->device[idx]->state = TE_STATE_IDLE;
    }
    matrix_handle->trigger_cnt = 0;
    matrix_handle->current_state = TE_STATE_IDLE;
}

static void matrix_event_give(te_matrix_handle_t matrix_handle)
{
    touch_elem_message_t element_message;
    touch_matrix_message_t matrix_message = {
        .event = matrix_handle->event,
        .position = matrix_handle->position
    };
    element_message.handle = (touch_elem_handle_t)matrix_handle;
    element_message.element_type = TOUCH_ELEM_TYPE_MATRIX;
    element_message.arg = matrix_handle->config->arg;
    memcpy(element_message.child_msg, &matrix_message, sizeof(matrix_message));
    te_event_give(element_message);
}

static inline void matrix_dispatch(te_matrix_handle_t matrix_handle, touch_elem_dispatch_t dispatch_method)
{
    if (dispatch_method == TOUCH_ELEM_DISP_EVENT) {
        matrix_event_give(matrix_handle);  //Event queue
    } else if (dispatch_method == TOUCH_ELEM_DISP_CALLBACK) {
        touch_matrix_message_t matrix_info;
        matrix_info.event = matrix_handle->event;
        matrix_info.position = matrix_handle->position;
        void *arg = matrix_handle->config->arg;
        matrix_handle->config->callback(matrix_handle, &matrix_info, arg);  //Event callback
    }
}

void matrix_enable_wakeup_calibration(te_matrix_handle_t matrix_handle, bool en)
{
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; ++idx) {
        matrix_handle->device[idx]->is_use_last_threshold = !en;
    }
}

/**
 * @brief   Scan the matrix channel
 *
 * This function will output the press position and release position info
 * so as to determine which operation(press/release) is happening, since there
 * will get the invalid state if user operates multi-points at the same time.
 *
 */
static void matrix_scan_axis(te_matrix_handle_t matrix_handle, touch_matrix_position_t *press_pos,
                             uint8_t *press_cnt, touch_matrix_position_t *release_pos, uint8_t *release_cnt)
{
    press_pos->x_axis = TE_MAT_POS_MAX;
    press_pos->y_axis = TE_MAT_POS_MAX;
    release_pos->x_axis = TE_MAT_POS_MAX;
    release_pos->y_axis = TE_MAT_POS_MAX;
    for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
        te_dev_t *device = matrix_handle->device[idx];
        if (device->state == TE_STATE_PRESS) {
            if (idx < matrix_handle->x_channel_num) {
                press_pos->x_axis = idx;                             //Write down the x axis info
            } else {
                press_pos->y_axis = idx - matrix_handle->x_channel_num;  //Write down the y axis info
            }
            (*press_cnt)++;
        } else if (device->state == TE_STATE_RELEASE) {
            if (idx < matrix_handle->x_channel_num) {
                release_pos->x_axis = idx;
            } else {
                release_pos->y_axis = idx - matrix_handle->x_channel_num;
            }
            (*release_cnt)++;
        }
    }
}

/**
 * @brief   Pre-check and fix
 *
 * This function will pre-check and fix the invalid state, preparing for the
 * next detection.
 *
 */
static void matrix_pre_fixed(te_matrix_handle_t matrix_handle, touch_matrix_position_t *press_pos,
                             uint8_t press_cnt, touch_matrix_position_t *release_pos, uint8_t release_cnt)
{
    te_dev_t *device;
    te_matrix_state_t last_state = matrix_handle->current_state;
    touch_matrix_position_t last_pos = {
        .x_axis = matrix_handle->position.x_axis,
        .y_axis = matrix_handle->position.y_axis
    };
    if (last_state == TE_STATE_IDLE) {
        if (release_cnt > 0) {
            /*< Release is not allowed while matrix is in IDLE state, */
            /*< if that happened, reset it from Release into IDLE.    */
            for (int idx = 0; idx < matrix_handle->x_channel_num + matrix_handle->y_channel_num; idx++) {
                device = matrix_handle->device[idx];
                if (device->state != TE_STATE_RELEASE) {
                    continue;
                }
                device->state = TE_STATE_IDLE;  //Reset to IDLE
            }
        }
    } else if (last_state == TE_STATE_PRESS) {
        if (release_cnt > 0) {
            /*< Release position must be the same as the last Press position, */
            /*< if it is not, reset it into IDLE.                             */
            if (release_pos->x_axis != TE_MAT_POS_MAX && release_pos->x_axis != last_pos.x_axis) {
                device = matrix_handle->device[release_pos->x_axis];
                device->state = TE_STATE_IDLE;
            }
            if (release_pos->y_axis != TE_MAT_POS_MAX && release_pos->y_axis != last_pos.y_axis) {
                device = matrix_handle->device[release_pos->y_axis + matrix_handle->x_channel_num];
                device->state = TE_STATE_IDLE;
            }
        }
        if (press_cnt > 2) { //TODO: remove or rewrite here
            /*< If the last state is Press and current press count more than 2, */
            /*< there must be multi-touch occurred, reset all of the channels   */
            /*< into IDLE except the last position channels.                    */
            if (press_pos->x_axis != TE_MAT_POS_MAX && press_pos->x_axis != last_pos.x_axis) {
                device = matrix_handle->device[press_pos->x_axis];
                device->state = TE_STATE_IDLE;
            }
            if (press_pos->y_axis != TE_MAT_POS_MAX && press_pos->y_axis != last_pos.y_axis) {
                device = matrix_handle->device[press_pos->y_axis + matrix_handle->x_channel_num];
                device->state = TE_STATE_IDLE;
            }
        }
    }
}

//TODO: refactor this ugly implementation
static esp_err_t matrix_get_axis_state(touch_matrix_position_t *press_pos, uint8_t press_cnt, touch_matrix_position_t *release_pos,
                                       uint8_t release_cnt, te_matrix_state_t *x_axis_state, te_matrix_state_t *y_axis_state)
{
    esp_err_t ret;
    if (press_cnt >= 2) {
        if (press_pos->x_axis != TE_MAT_POS_MAX && press_pos->y_axis != TE_MAT_POS_MAX) {
            *x_axis_state = TE_STATE_PRESS;
            *y_axis_state = TE_STATE_PRESS;
            ret = ESP_OK;
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }

    } else if (release_cnt >= 2) {
        if (release_pos->x_axis != TE_MAT_POS_MAX && release_pos->y_axis != TE_MAT_POS_MAX) {
            *x_axis_state = TE_STATE_RELEASE;
            *y_axis_state = TE_STATE_RELEASE;
            ret = ESP_OK;
        } else {
            ret = ESP_ERR_INVALID_STATE;
        }
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    return ret;
}

/**
 * @brief Matrix button process
 *
 * This function will process the matrix button state and maintain a matrix FSM:
 *              IDLE ----> Press ----> Release ----> IDLE
 *
 * The state transition procedure is as follow:
 *       (channel state ----> x,y axis state ----> matrix button state)
 *
 * TODO: add state transition diagram
 */
static void matrix_proc_state(te_matrix_handle_t matrix_handle)
{
    esp_err_t ret;
    uint8_t press_cnt = 0;
    uint8_t release_cnt = 0;
    touch_matrix_position_t press_pos;
    touch_matrix_position_t release_pos;
    te_matrix_state_t x_axis_state = TE_STATE_IDLE;
    te_matrix_state_t y_axis_state = TE_STATE_IDLE;
    uint32_t event_mask = matrix_handle->config->event_mask;
    touch_elem_dispatch_t dispatch_method = matrix_handle->config->dispatch_method;

    BaseType_t mux_ret = xSemaphoreTake(s_te_mat_obj->mutex, 0);
    if (mux_ret != pdPASS) {
        return;
    }

    //TODO: refactor those functions
    /*< Scan the state of all the matrix buttons channel */
    matrix_scan_axis(matrix_handle, &press_pos, &press_cnt, &release_pos, &release_cnt);

    /*< Pre check and fixed the invalid state */
    matrix_pre_fixed(matrix_handle, &press_pos, press_cnt, &release_pos, release_cnt);

    /*< Figure out x,y axis state and take the position */
    ret = matrix_get_axis_state(&press_pos, press_cnt, &release_pos, release_cnt, &x_axis_state, &y_axis_state);
    if (ret != ESP_OK) { //TODO: remove return
        xSemaphoreGive(s_te_mat_obj->mutex);
        return;
    }

    matrix_handle->current_state = matrix_get_state(x_axis_state, y_axis_state);

    if (matrix_handle->current_state == TE_STATE_PRESS) {
        if (matrix_handle->last_state == TE_STATE_IDLE) { //IDLE ---> Press = On_Press
            matrix_update_position(matrix_handle, press_pos);
            ESP_LOGD(TE_DEBUG_TAG, "matrix press  (%"PRIu8", %"PRIu8")", matrix_handle->position.x_axis, matrix_handle->position.y_axis);
            if (event_mask & TOUCH_ELEM_EVENT_ON_PRESS) {
                matrix_handle->event = TOUCH_MATRIX_EVT_ON_PRESS;
                matrix_dispatch(matrix_handle, dispatch_method);
            }
        } else if (matrix_handle->last_state == TE_STATE_PRESS) { //Press ---> Press = On_LongPress
            if (event_mask & TOUCH_ELEM_EVENT_ON_LONGPRESS) {
                if (++matrix_handle->trigger_cnt >= matrix_handle->trigger_thr) {
                    ESP_LOGD(TE_DEBUG_TAG, "matrix longpress (%"PRIu8", %"PRIu8")", matrix_handle->position.x_axis, matrix_handle->position.y_axis);
                    matrix_handle->event = TOUCH_MATRIX_EVT_ON_LONGPRESS;
                    matrix_dispatch(matrix_handle, dispatch_method);
                    matrix_handle->trigger_cnt = 0;
                }
            }
        }
    } else if (matrix_handle->current_state == TE_STATE_RELEASE) {
        if (matrix_handle->last_state == TE_STATE_PRESS) {  //Press ---> Release = On_Release
            ESP_LOGD(TE_DEBUG_TAG, "matrix release (%"PRIu8", %"PRIu8")", matrix_handle->position.x_axis, matrix_handle->position.y_axis);
            if (event_mask & TOUCH_ELEM_EVENT_ON_RELEASE) {
                matrix_handle->event = TOUCH_MATRIX_EVT_ON_RELEASE;
                matrix_dispatch(matrix_handle, dispatch_method);
            }
        } else if (matrix_handle->last_state == TE_STATE_RELEASE) {  // Release ---> Release = On_IDLE (Not dispatch)
            matrix_reset_state(matrix_handle);
        }
    } else if (matrix_handle->current_state == TE_STATE_IDLE) {
        if (matrix_handle->last_state == TE_STATE_RELEASE) {  //Release ---> IDLE = On_IDLE (Not dispatch)
            //Nothing
        } else if (matrix_handle->last_state == TE_STATE_IDLE) { //IDLE ---> IDLE = Running IDLE (Not dispatch)
            //Move Pre-fix into here
        }
    }
    matrix_handle->last_state = matrix_handle->current_state;
    xSemaphoreGive(s_te_mat_obj->mutex);
}

static inline te_state_t matrix_get_state(te_matrix_state_t x_axis_state, te_matrix_state_t y_axis_state)
{
    te_state_t matrix_state;
    if ((x_axis_state == TE_STATE_PRESS) && (y_axis_state == TE_STATE_PRESS)) {
        matrix_state = TE_STATE_PRESS;
    } else if ((x_axis_state == TE_STATE_RELEASE) && (y_axis_state == TE_STATE_RELEASE)) {
        matrix_state = TE_STATE_RELEASE;
    } else {
        matrix_state = TE_STATE_IDLE;
    }
    return matrix_state;
}

static void matrix_update_position(te_matrix_handle_t matrix_handle, touch_matrix_position_t new_pos) {
    matrix_handle->position.x_axis = new_pos.x_axis;
    matrix_handle->position.y_axis = new_pos.y_axis;
    matrix_handle->position.index = matrix_handle->position.x_axis * matrix_handle->y_channel_num + matrix_handle->position.y_axis;
}
