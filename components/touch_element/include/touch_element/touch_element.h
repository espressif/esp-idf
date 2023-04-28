/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/touch_sensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------- General hardware & system default configuration  -------------------------------- */
/* Since those are important hardware and algorithm parameters, user should not change them before knowing all details*/
/* ------------------------------------------------------------------------------------------------------------------ */
#define TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG()                                    \
{                                                                             \
    .hardware = {                                                             \
        .upper_voltage = TOUCH_HVOLT_2V7,                                     \
        .voltage_attenuation = TOUCH_HVOLT_ATTEN_0V5,                         \
        .lower_voltage = TOUCH_LVOLT_0V5,                                     \
        .suspend_channel_polarity = TOUCH_PAD_CONN_HIGHZ,                     \
        .denoise_level = TOUCH_PAD_DENOISE_BIT4,                              \
        .denoise_equivalent_cap = TOUCH_PAD_DENOISE_CAP_L0,                   \
        .smooth_filter_mode = TOUCH_PAD_SMOOTH_IIR_2,                         \
        .benchmark_filter_mode = TOUCH_PAD_FILTER_IIR_16,                     \
        .sample_count = 500,                                                  \
        .sleep_cycle = 0xf,                                                   \
        .benchmark_debounce_count = 2,                                        \
        .benchmark_calibration_threshold = 2,                                 \
        .benchmark_jitter_step = 5                                            \
    },                                                                        \
    .software = {                                                             \
        .waterproof_threshold_divider = 0.8,                                  \
        .processing_period = 10,                                              \
        .intr_message_size = 14,                                              \
        .event_message_size = 20                                              \
    }                                                                         \
}
/* ------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------------------------- Event subscription  ----------------------------------------------- */
#define TOUCH_ELEM_EVENT_NONE                       BIT(0)      //!< None event
#define TOUCH_ELEM_EVENT_ON_PRESS                   BIT(1)      //!< On Press event
#define TOUCH_ELEM_EVENT_ON_RELEASE                 BIT(2)      //!< On Release event
#define TOUCH_ELEM_EVENT_ON_LONGPRESS               BIT(3)      //!< On LongPress event
#define TOUCH_ELEM_EVENT_ON_CALCULATION             BIT(4)      //!< On Calculation event
/* ------------------------------------------------------------------------------------------------------------------ */
#define TOUCH_WATERPROOF_GUARD_NOUSE       (0)         //!< Waterproof no use guard sensor
/* -------------------------------- Global hardware & software configuration struct --------------------------------- */
/**
 * @brief   Touch element software configuration
 */
typedef struct {
    float waterproof_threshold_divider;        //!< Waterproof guard channel threshold divider
    uint8_t processing_period;                 //!< Processing period(ms)
    uint8_t intr_message_size;                 //!< Interrupt message queue size
    uint8_t event_message_size;                //!< Event message queue size
} touch_elem_sw_config_t;

/**
 * @brief   Touch element hardware configuration
 */
typedef struct {
    touch_high_volt_t upper_voltage;                  //!< Touch sensor channel upper charge voltage
    touch_volt_atten_t voltage_attenuation;           //!< Touch sensor channel upper charge voltage attenuation (Diff voltage is upper - attenuation - lower)
    touch_low_volt_t lower_voltage;                   //!< Touch sensor channel lower charge voltage
    touch_pad_conn_type_t suspend_channel_polarity;   //!< Suspend channel polarity (High Impedance State or GND)
    touch_pad_denoise_grade_t denoise_level;          //!< Internal de-noise level
    touch_pad_denoise_cap_t denoise_equivalent_cap;   //!< Internal de-noise channel (Touch channel 0) equivalent capacitance
    touch_smooth_mode_t smooth_filter_mode;           //!< Smooth value filter mode (This only apply to touch_pad_filter_read_smooth())
    touch_filter_mode_t benchmark_filter_mode;        //!< Benchmark filter mode
    uint16_t sample_count;                            //!< The count of sample in each measurement of touch sensor
    uint16_t sleep_cycle;                             //!< The cycle (RTC slow clock) of sleep
    uint8_t benchmark_debounce_count;                 //!< Benchmark debounce count
    uint8_t benchmark_calibration_threshold;          //!< Benchmark calibration threshold
    uint8_t benchmark_jitter_step;                    //!< Benchmark jitter filter step (This only works at while benchmark filter mode is jitter filter)
} touch_elem_hw_config_t;

/**
 * @brief   Touch element global configuration passed to touch_element_install
 */
typedef struct {
    touch_elem_hw_config_t hardware;         //!< Hardware configuration
    touch_elem_sw_config_t software;         //!< Software configuration
} touch_elem_global_config_t;

/**
 * @brief   Touch element waterproof configuration passed to touch_element_waterproof_install
 */
typedef struct {
    touch_pad_t guard_channel;     //!< Waterproof Guard-Sensor channel number (index)
    float guard_sensitivity;       //!< Waterproof Guard-Sensor sensitivity
} touch_elem_waterproof_config_t;
/* ------------------------------------------------------------------------------------------------------------------ */
typedef void *touch_elem_handle_t;        //!< Touch element handle type
typedef uint32_t touch_elem_event_t;      //!< Touch element event type

/**
 * @brief   Touch element handle type
 */
typedef enum {
    TOUCH_ELEM_TYPE_BUTTON,         //!< Touch element button
    TOUCH_ELEM_TYPE_SLIDER,         //!< Touch element slider
    TOUCH_ELEM_TYPE_MATRIX,         //!< Touch element matrix button
} touch_elem_type_t;

/**
 * @brief   Touch element event dispatch methods (event queue/callback)
 */
typedef enum {
    TOUCH_ELEM_DISP_EVENT,           //!< Event queue dispatch
    TOUCH_ELEM_DISP_CALLBACK,        //!< Callback dispatch
    TOUCH_ELEM_DISP_MAX
} touch_elem_dispatch_t;

/**
 * @brief   Touch element event message type from touch_element_message_receive()
 */
typedef struct {
    touch_elem_handle_t handle;             //!< Touch element handle
    touch_elem_type_t element_type;         //!< Touch element type
    void *arg;                              //!< User input argument
    uint8_t child_msg[8];                   //!< Encoded message
} touch_elem_message_t;
/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief   Touch element processing initialization
 *
 * @param[in]   global_config   Global initialization configuration structure
 *
 * @note    To reinitialize the touch element object, call touch_element_uninstall() first
 *
 * @return
 *      - ESP_OK: Successfully initialized
 *      - ESP_ERR_INVALID_ARG: Invalid argument
 *      - ESP_ERR_NO_MEM: Insufficient memory
 *      - ESP_ERR_INVALID_STATE: Touch element is already initialized
 *      - Others: Unknown touch driver layer or lower layer error
 */
esp_err_t touch_element_install(const touch_elem_global_config_t *global_config);

/**
 * @brief   Touch element processing start
 *
 * This function starts the touch element processing system
 *
 * @note    This function must only be called after all the touch element instances finished creating
 *
 * @return
 *      - ESP_OK: Successfully started to process
 *      - Others: Unknown touch driver layer or lower layer error
 */
esp_err_t touch_element_start(void);

/**
 * @brief   Touch element processing stop
 *
 * This function stops the touch element processing system
 *
 * @note    This function must be called before changing the system (hardware, software) parameters
 *
 * @return
 *      - ESP_OK: Successfully stopped to process
 *      - Others: Unknown touch driver layer or lower layer error
 */
esp_err_t touch_element_stop(void);

/**
 * @brief   Release resources allocated using touch_element_install
 *
 */
void touch_element_uninstall(void);

/**
 * @brief   Get current event message of touch element instance
 *
 * This function will receive the touch element message (handle, event type, etc...)
 * from te_event_give(). It will block until a touch element event or a timeout occurs.
 *
 * @param[out]  element_message          Touch element event message structure
 * @param[in]   ticks_to_wait   Number of FreeRTOS ticks to block for waiting event
 * @return
 *      - ESP_OK: Successfully received touch element event
 *      - ESP_ERR_INVALID_STATE: Touch element library is not initialized
 *      - ESP_ERR_INVALID_ARG: element_message is null
 *      - ESP_ERR_TIMEOUT: Timed out waiting for event
 */
esp_err_t touch_element_message_receive(touch_elem_message_t *element_message, uint32_t ticks_to_wait);

/**
 * @brief   Touch element waterproof initialization
 *
 * This function enables the hardware waterproof, then touch element system uses Shield-Sensor
 * and Guard-Sensor to mitigate the influence of water-drop and water-stream.
 *
 * @param[in] waterproof_config     Waterproof configuration
 *
 * @note    If the waterproof function is used, Shield-Sensor can not be disabled and it will use channel 14 as
 *          it's internal channel. Hence, the user can not use channel 14 for another propose. And the Guard-Sensor
 *          is not necessary since it is optional.
 *
 * @note    Shield-Sensor: It always uses channel 14 as the shield channel, so user must connect
 *          the channel 14 and Shield-Layer in PCB since it will generate a synchronous signal automatically
 *
 * @note    Guard-Sensor: This function is optional. If used, the user must connect the guard channel and Guard-Ring
 *          in PCB. Any channels user wants to protect should be added into Guard-Ring in PCB.
 *
 * @return
 *      - ESP_OK: Successfully initialized
 *      - ESP_ERR_INVALID_STATE: Touch element library is not initialized
 *      - ESP_ERR_INVALID_ARG: waterproof_config is null or invalid Guard-Sensor channel
 *      - ESP_ERR_NO_MEM: Insufficient memory
 */
esp_err_t touch_element_waterproof_install(const touch_elem_waterproof_config_t *waterproof_config);

/**
 * @brief   Release resources allocated using touch_element_waterproof_install()
 */
void touch_element_waterproof_uninstall(void);

/**
 * @brief   Add a masked handle to protect while Guard-Sensor has been triggered
 *
 * This function will add an application handle (button, slider, etc...) as a masked handle. While Guard-Sensor
 * has been triggered, waterproof function will start working and lock the application internal state. While the
 * influence of water is reduced, the application will be unlock and reset into IDLE state.
 *
 * @param[in] element_handle     Touch element instance handle
 *
 * @note    The waterproof protection logic must follow the real circuit in PCB, it means that all of the channels
 *          inside the input handle must be inside the Guard-Ring in real circuit.
 *
 * @return
 *      - ESP_OK: Successfully added a masked handle
 *      - ESP_ERR_INVALID_STATE: Waterproof is not initialized
 *      - ESP_ERR_INVALID_ARG: element_handle is null
 */
esp_err_t touch_element_waterproof_add(touch_elem_handle_t element_handle);

/**
 * @brief   Remove a masked handle to protect
 *
 * This function will remove an application handle from masked handle table.
 *
 * @param[in] element_handle     Touch element instance handle
 *
 * @return
 *      - ESP_OK: Successfully removed a masked handle
 *      - ESP_ERR_INVALID_STATE: Waterproof is not initialized
 *      - ESP_ERR_INVALID_ARG: element_handle is null
 *      - ESP_ERR_NOT_FOUND: Failed to search element_handle from waterproof mask_handle list
 */
esp_err_t touch_element_waterproof_remove(touch_elem_handle_t element_handle);

#ifdef __cplusplus
}
#endif
