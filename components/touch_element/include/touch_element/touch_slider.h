/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "touch_element/touch_element.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------- General slider instance default configuration  --------------------------------- */
#define TOUCH_SLIDER_GLOBAL_DEFAULT_CONFIG()                                  \
{                                                                             \
    .quantify_lower_threshold = 0.3,                                          \
    .threshold_divider = 0.8,                                                 \
    .filter_reset_time = 50,                                                  \
    .benchmark_update_time = 500,                                             \
    .position_filter_size = 10,                                               \
    .position_filter_factor = 2,                                              \
    .calculate_channel_count = 3                                              \
}
/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief   Slider initialization configuration passed to touch_slider_install
 */
typedef struct {
    float quantify_lower_threshold;            //!< Slider signal quantification threshold
    float threshold_divider;                   //!< Slider channel threshold divider
    uint16_t filter_reset_time;                //!< Slider position filter reset time (Unit is esp_timer callback tick)
    uint16_t benchmark_update_time;            //!< Slider benchmark update time (Unit is esp_timer callback tick)
    uint8_t position_filter_size;              //!< Moving window filter buffer size
    uint8_t position_filter_factor;            //!< One-order IIR filter factor
    uint8_t calculate_channel_count;           //!< The number of channels which will take part in calculation
} touch_slider_global_config_t;

/**
 * @brief   Slider configuration (for new instance) passed to touch_slider_create()
 */
typedef struct {
    const touch_pad_t *channel_array;    //!< Slider channel array
    const float *sensitivity_array;      //!< Slider channel sensitivity array
    uint8_t channel_num;                 //!< The number of slider channels
    uint8_t position_range;              //!< The right region of touch slider position range, [0, position_range (less than or equal to 255)]
} touch_slider_config_t;

/**
 * @brief   Slider event type
 */
typedef enum {
    TOUCH_SLIDER_EVT_ON_PRESS,                  //!< Slider on Press event
    TOUCH_SLIDER_EVT_ON_RELEASE,                //!< Slider on Release event
    TOUCH_SLIDER_EVT_ON_CALCULATION,            //!< Slider on Calculation event
    TOUCH_SLIDER_EVT_MAX
} touch_slider_event_t;

typedef uint32_t touch_slider_position_t;       //!< Slider position data type

/**
 * @brief   Slider message type
 */
typedef struct {
    touch_slider_event_t event;                 //!< Slider event
    touch_slider_position_t position;           //!< Slider position
} touch_slider_message_t;

typedef touch_elem_handle_t touch_slider_handle_t;   //!< Slider instance handle
typedef void(*touch_slider_callback_t)(touch_slider_handle_t, touch_slider_message_t *, void *); //!< Slider callback type

/**
 * @brief   Touch slider initialize
 *
 * This function initializes touch slider object and acts on all
 * touch slider instances.
 *
 * @param[in] global_config   Touch slider global initialization configuration
 *
 * @return
 *      - ESP_OK: Successfully initialized touch slider
 *      - ESP_ERR_INVALID_STATE: Touch element library was not initialized
 *      - ESP_ERR_INVALID_ARG: slider_init is NULL
 *      - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t touch_slider_install(const touch_slider_global_config_t *global_config);

/**
 * @brief   Release resources allocated using touch_slider_install()
 *
 */
void touch_slider_uninstall(void);

/**
* @brief   Create a new touch slider instance
*
* @param[in]  slider_config     Slider configuration
* @param[out] slider_handle     Slider handle
*
* @note    The index of Channel array and sensitivity array must be one-one correspondence
*
* @return
*      - ESP_OK: Successfully create touch slider
*      - ESP_ERR_INVALID_STATE: Touch slider driver was not initialized
*      - ESP_ERR_INVALID_ARG: Invalid configuration struct or arguments is NULL
*      - ESP_ERR_NO_MEM: Insufficient memory
*/
esp_err_t touch_slider_create(const touch_slider_config_t *slider_config, touch_slider_handle_t *slider_handle);

/**
 * @brief   Release resources allocated using touch_slider_create
 *
 * @param[in] slider_handle   Slider handle
 * @return
 *      - ESP_OK: Successfully released resources
 *      - ESP_ERR_INVALID_STATE: Touch slider driver was not initialized
 *      - ESP_ERR_INVALID_ARG: slider_handle is null
 *      - ESP_ERR_NOT_FOUND: Input handle is not a slider handle
 */
esp_err_t touch_slider_delete(touch_slider_handle_t slider_handle);

/**
 * @brief   Touch slider subscribes event
 *
 * This function uses event mask to subscribe to touch slider events, once one of
 * the subscribed events occurs, the event message could be retrieved by calling
 * touch_element_message_receive() or input callback routine.
 *
 * @param[in] slider_handle     Slider handle
 * @param[in] event_mask        Slider subscription event mask
 * @param[in] arg               User input argument
 *
 * @note    Touch slider only support three kind of event masks, they are
 *          TOUCH_ELEM_EVENT_ON_PRESS, TOUCH_ELEM_EVENT_ON_RELEASE. You can use those event masks in any
 *          combination to achieve the desired effect.
 *
 * @return
 *      - ESP_OK: Successfully subscribed touch slider event
 *      - ESP_ERR_INVALID_STATE: Touch slider driver was not initialized
 *      - ESP_ERR_INVALID_ARG: slider_handle is null or event is not supported
 */
esp_err_t touch_slider_subscribe_event(touch_slider_handle_t slider_handle, uint32_t event_mask, void *arg);

/**
 * @brief   Touch slider set dispatch method
 *
 * This function sets a dispatch method that the driver core will use
 * this method as the event notification method.
 *
 * @param[in] slider_handle     Slider handle
 * @param[in] dispatch_method   Dispatch method (By callback/event)
 *
 * @return
 *      - ESP_OK: Successfully set dispatch method
 *      - ESP_ERR_INVALID_STATE: Touch slider driver was not initialized
 *      - ESP_ERR_INVALID_ARG: slider_handle is null or dispatch_method is invalid
 */
esp_err_t touch_slider_set_dispatch_method(touch_slider_handle_t slider_handle, touch_elem_dispatch_t dispatch_method);

/**
 * @brief   Touch slider set callback
 *
 * This function sets a callback routine into touch element driver core,
 * when the subscribed events occur, the callback routine will be called.
 *
 * @param[in] slider_handle     Slider handle
 * @param[in] slider_callback   User input callback
 *
 * @note        Slider message will be passed from the callback function and it will be destroyed when the callback function return.
 *
 * @warning     Since this input callback routine runs on driver core (esp-timer callback routine),
 *              it should not do something that attempts to Block, such as calling vTaskDelay().
 *
 * @return
 *      - ESP_OK: Successfully set callback
 *      - ESP_ERR_INVALID_STATE: Touch slider driver was not initialized
 *      - ESP_ERR_INVALID_ARG: slider_handle or slider_callback is null
 */
esp_err_t touch_slider_set_callback(touch_slider_handle_t slider_handle, touch_slider_callback_t slider_callback);

/**
 * @brief   Touch slider get message
 *
 * This function decodes the element message from touch_element_message_receive() and return
 * a slider message pointer.
 *
 * @param[in] element_message   element message
 *
 * @return  Touch slider message pointer
 */
const touch_slider_message_t* touch_slider_get_message(const touch_elem_message_t* element_message);

#ifdef __cplusplus
}
#endif
