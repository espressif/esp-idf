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
/* --------------------------------- General button instance default configuration  --------------------------------- */
#define TOUCH_BUTTON_GLOBAL_DEFAULT_CONFIG()                                  \
{                                                                             \
    .threshold_divider = 0.8,                                                 \
    .default_lp_time = 1000                                                   \
}
/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief   Button initialization configuration passed to touch_button_install
 */
typedef struct {
    float threshold_divider;        //!< Button channel threshold divider
    uint32_t default_lp_time;       //!< Button default LongPress event time (ms)
} touch_button_global_config_t;

/**
 * @brief   Button configuration (for new instance) passed to touch_button_create()
 */
typedef struct {
    touch_pad_t channel_num;     //!< Button channel number (index)
    float channel_sens;          //!< Button channel sensitivity
} touch_button_config_t;

/**
 * @brief   Button event type
 */
typedef enum {
    TOUCH_BUTTON_EVT_ON_PRESS,         //!< Button Press event
    TOUCH_BUTTON_EVT_ON_RELEASE,       //!< Button Release event
    TOUCH_BUTTON_EVT_ON_LONGPRESS,     //!< Button LongPress event
    TOUCH_BUTTON_EVT_MAX
} touch_button_event_t;

/**
 * @brief   Button message type
 */
typedef struct {
    touch_button_event_t event;        //!< Button event
} touch_button_message_t;

typedef touch_elem_handle_t touch_button_handle_t;      //!< Button handle
typedef void(*touch_button_callback_t)(touch_button_handle_t, touch_button_message_t *, void *); //!< Button callback type

/**
 * @brief   Touch Button initialize
 *
 * This function initializes touch button global and acts on all
 * touch button instances.
 *
 * @param[in] global_config   Button object initialization configuration
 *
 * @return
 *      - ESP_OK: Successfully initialized touch button
 *      - ESP_ERR_INVALID_STATE: Touch element library was not initialized
 *      - ESP_ERR_INVALID_ARG: button_init is NULL
 *      - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t touch_button_install(const touch_button_global_config_t *global_config);

/**
 * @brief   Release resources allocated using touch_button_install()
 */
void touch_button_uninstall(void);

/**
 * @brief   Create a new touch button instance
 *
 * @param[in]  button_config    Button configuration
 * @param[out] button_handle    Button handle
 *
 * @note    The sensitivity has to be explored in experiments,
 *          Sensitivity = (Raw(touch) - Raw(release)) / Raw(release) * 100%
 *
 * @return
 *      - ESP_OK: Successfully create touch button
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_NO_MEM: Insufficient memory
 *      - ESP_ERR_INVALID_ARG: Invalid configuration struct or arguments is NULL
 */
esp_err_t touch_button_create(const touch_button_config_t *button_config, touch_button_handle_t *button_handle);

/**
 * @brief Release resources allocated using touch_button_create()
 *
 * @param[in] button_handle   Button handle
 * @return
 *      - ESP_OK: Successfully released resources
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_INVALID_ARG: button_handle is null
 *      - ESP_ERR_NOT_FOUND: Input handle is not a button handle
 */
esp_err_t touch_button_delete(touch_button_handle_t button_handle);

/**
 * @brief   Touch button subscribes event
 *
 * This function uses event mask to subscribe to touch button events, once one of
 * the subscribed events occurs, the event message could be retrieved by calling
 * touch_element_message_receive() or input callback routine.
 *
 * @param[in] button_handle     Button handle
 * @param[in] event_mask        Button subscription event mask
 * @param[in] arg               User input argument
 *
 * @note    Touch button only support three kind of event masks, they are
 *          TOUCH_ELEM_EVENT_ON_PRESS, TOUCH_ELEM_EVENT_ON_RELEASE, TOUCH_ELEM_EVENT_ON_LONGPRESS.
 *          You can use those event masks in any combination to achieve the desired effect.
 *
 * @return
 *      - ESP_OK: Successfully subscribed touch button event
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_INVALID_ARG: button_handle is null or event is not supported
 */
esp_err_t touch_button_subscribe_event(touch_button_handle_t button_handle, uint32_t event_mask, void *arg);

/**
 * @brief   Touch button set dispatch method
 *
 * This function sets a dispatch method that the driver core will use
 * this method as the event notification method.
 *
 * @param[in] button_handle     Button handle
 * @param[in] dispatch_method   Dispatch method (By callback/event)
 *
 * @return
 *      - ESP_OK: Successfully set dispatch method
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_INVALID_ARG: button_handle is null or dispatch_method is invalid
 */
esp_err_t touch_button_set_dispatch_method(touch_button_handle_t button_handle, touch_elem_dispatch_t dispatch_method);

/**
 * @brief   Touch button set callback
 *
 * This function sets a callback routine into touch element driver core,
 * when the subscribed events occur, the callback routine will be called.
 *
 * @param[in] button_handle     Button handle
 * @param[in] button_callback   User input callback
 *
 * @note        Button message will be passed from the callback function and it will be destroyed when the callback function return.
 *
 * @warning     Since this input callback routine runs on driver core (esp-timer callback routine),
 *              it should not do something that attempts to Block, such as calling vTaskDelay().
 *
 * @return
 *      - ESP_OK: Successfully set callback
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_INVALID_ARG: button_handle or button_callback is null
 */
esp_err_t touch_button_set_callback(touch_button_handle_t button_handle, touch_button_callback_t button_callback);

/**
 * @brief   Touch button set long press trigger time
 *
 * This function sets the threshold time (ms) for a long press event. If a button is pressed
 * and held for a period of time that exceeds the threshold time, a long press event is triggered.
 *
 * @param[in] button_handle     Button handle
 * @param[in] threshold_time    Threshold time (ms) of long press event occur
 *
 * @return
 *      - ESP_OK: Successfully set the threshold time of long press event
 *      - ESP_ERR_INVALID_STATE: Touch button driver was not initialized
 *      - ESP_ERR_INVALID_ARG: button_handle is null or time (ms) is not lager than 0
 */
esp_err_t touch_button_set_longpress(touch_button_handle_t button_handle, uint32_t threshold_time);

/**
 * @brief   Touch button get message
 *
 * This function decodes the element message from touch_element_message_receive() and return
 * a button message pointer.
 *
 * @param[in] element_message   element message
 *
 * @return  Touch button message pointer
 */
const touch_button_message_t* touch_button_get_message(const touch_elem_message_t* element_message);

#ifdef __cplusplus
}
#endif
