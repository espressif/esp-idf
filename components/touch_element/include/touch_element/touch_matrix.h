// Copyright 2016-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "touch_element/touch_element.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ----------------------------- General matrix button instance default configuration  ------------------------------ */
#define TOUCH_MATRIX_GLOBAL_DEFAULT_CONFIG()                                  \
{                                                                             \
    .threshold_divider = 0.8,                                                 \
    .default_lp_time = 1000                                                   \
}
/* ------------------------------------------------------------------------------------------------------------------ */
/**
 * @brief   Matrix button initialization configuration passed to touch_matrix_install
 */
typedef struct {
    float threshold_divider;        //!< Matrix button channel threshold divider
    uint32_t default_lp_time;       //!< Matrix button default LongPress event time (ms)
} touch_matrix_global_config_t;

/**
 * @brief   Matrix button configuration (for new instance) passed to touch_matrix_create()
 */
typedef struct {
    const touch_pad_t *x_channel_array;      //!< Matrix button x-axis channels array
    const touch_pad_t *y_channel_array;      //!< Matrix button y-axis channels array
    const float *x_sensitivity_array;        //!< Matrix button x-axis channels sensitivity array
    const float *y_sensitivity_array;        //!< Matrix button y-axis channels sensitivity array
    uint8_t x_channel_num;                   //!< The number of channels in x-axis
    uint8_t y_channel_num;                   //!< The number of channels in y-axis
} touch_matrix_config_t;

/**
 * @brief   Matrix button event type
 */
typedef enum {
    TOUCH_MATRIX_EVT_ON_PRESS,            //!< Matrix button Press event
    TOUCH_MATRIX_EVT_ON_RELEASE,          //!< Matrix button Press event
    TOUCH_MATRIX_EVT_ON_LONGPRESS,        //!< Matrix button LongPress event
    TOUCH_MATRIX_EVT_MAX
} touch_matrix_event_t;

/**
 * @brief   Matrix button position data type
 */
typedef struct {
    uint8_t x_axis;     //!< Matrix button x axis position
    uint8_t y_axis;     //!< Matrix button y axis position
    uint8_t index;      //!< Matrix button position index
} touch_matrix_position_t;

/**
 * @brief   Matrix message type
 */
typedef struct {
    touch_matrix_event_t event;             //!< Matrix event
    touch_matrix_position_t position;       //!< Matrix position
} touch_matrix_message_t;

typedef touch_elem_handle_t touch_matrix_handle_t;  //!< Matrix button instance handle
typedef void(*touch_matrix_callback_t)(touch_matrix_handle_t, touch_matrix_message_t *, void *); //!< Matrix button callback type

/**
 * @brief   Touch matrix button initialize
 *
 * This function initializes touch matrix button object and acts on all
 * touch matrix button instances.
 *
 * @param[in] global_config   Touch matrix global initialization configuration
 *
 * @return
 *      - ESP_OK: Successfully initialized touch matrix button
 *      - ESP_ERR_INVALID_STATE: Touch element library was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_init is NULL
 *      - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t touch_matrix_install(const touch_matrix_global_config_t *global_config);

/**
 * @brief   Release resources allocated using touch_matrix_install()
 *
 * @return
 *      - ESP_OK: Successfully released resources
 */
void touch_matrix_uninstall(void);

/**
 * @brief   Create a new touch matrix button instance
 *
 * @param[in]  matrix_config    Matrix button configuration
 * @param[out] matrix_handle    Matrix button handle
 *
 * @note    Channel array and sensitivity array must be one-one correspondence in those array
 *
 * @note    Touch matrix button does not support Multi-Touch now
 *
 * @return
 *      - ESP_OK: Successfully create touch matrix button
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: Invalid configuration struct or arguments is NULL
 *      - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t touch_matrix_create(const touch_matrix_config_t *matrix_config, touch_matrix_handle_t *matrix_handle);

/**
 * @brief   Release resources allocated using touch_matrix_create()
 *
 * @param[in] matrix_handle    Matrix handle
 * @return
 *      - ESP_OK: Successfully released resources
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_handle is null
 *      - ESP_ERR_NOT_FOUND: Input handle is not a matrix handle
 */
esp_err_t touch_matrix_delete(touch_matrix_handle_t matrix_handle);

/**
 * @brief   Touch matrix button subscribes event
 *
 * This function uses event mask to subscribe to touch matrix events, once one of
 * the subscribed events occurs, the event message could be retrieved by calling
 * touch_element_message_receive() or input callback routine.
 *
 * @param[in] matrix_handle     Matrix handle
 * @param[in] event_mask        Matrix subscription event mask
 * @param[in] arg               User input argument
 *
 * @note    Touch matrix button only support three kind of event masks, they are
 *          TOUCH_ELEM_EVENT_ON_PRESS, TOUCH_ELEM_EVENT_ON_RELEASE, TOUCH_ELEM_EVENT_ON_LONGPRESS. You can use those event
 *          masks in any combination to achieve the desired effect.
 *
 * @return
 *      - ESP_OK: Successfully subscribed touch matrix event
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_handle is null or event is not supported
 */
esp_err_t touch_matrix_subscribe_event(touch_matrix_handle_t matrix_handle, uint32_t event_mask, void *arg);

/**
 * @brief   Touch matrix button set dispatch method
 *
 * This function sets a dispatch method that the driver core will use
 * this method as the event notification method.
 *
 * @param[in] matrix_handle     Matrix button handle
 * @param[in] dispatch_method   Dispatch method (By callback/event)
 *
 * @return
 *      - ESP_OK: Successfully set dispatch method
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_handle is null or dispatch_method is invalid
 */
esp_err_t touch_matrix_set_dispatch_method(touch_matrix_handle_t matrix_handle, touch_elem_dispatch_t dispatch_method);

/**
 * @brief   Touch matrix button set callback
 *
 * This function sets a callback routine into touch element driver core,
 * when the subscribed events occur, the callback routine will be called.
 *
 * @param[in] matrix_handle     Matrix button handle
 * @param[in] matrix_callback   User input callback
 *
 * @note        Matrix message will be passed from the callback function and it will be destroyed when the callback function return.
 *
 * @warning     Since this input callback routine runs on driver core (esp-timer callback routine),
 *              it should not do something that attempts to Block, such as calling vTaskDelay().
 *
 * @return
 *      - ESP_OK: Successfully set callback
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_handle or matrix_callback is null
 */
esp_err_t touch_matrix_set_callback(touch_matrix_handle_t matrix_handle, touch_matrix_callback_t matrix_callback);

/**
 * @brief   Touch matrix button set long press trigger time
 *
 * This function sets the threshold time (ms) for a long press event. If a matrix button is pressed
 * and held for a period of time that exceeds the threshold time, a long press event is triggered.
 *
 * @param[in] matrix_handle     Matrix button handle
 * @param[in] threshold_time    Threshold time (ms) of long press event occur
 *
 * @return
 *      - ESP_OK: Successfully set the time of long press event
 *      - ESP_ERR_INVALID_STATE: Touch matrix driver was not initialized
 *      - ESP_ERR_INVALID_ARG: matrix_handle is null or time (ms) is 0
 */
esp_err_t touch_matrix_set_longpress(touch_matrix_handle_t matrix_handle, uint32_t threshold_time);

/**
 * @brief   Touch matrix get message
 *
 * This function decodes the element message from touch_element_message_receive() and return
 * a matrix message pointer.
 *
 * @param[in] element_message   element message
 *
 * @return  Touch matrix message pointer
 */
const touch_matrix_message_t* touch_matrix_get_message(const touch_elem_message_t* element_message);

#ifdef __cplusplus
}
#endif
