// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _IOT_BUTTON_H_
#define _IOT_BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/gpio.h"
#include "freertos/portmacro.h"
typedef void (* button_cb)(void*);
typedef void* button_handle_t;

typedef enum {
    BUTTON_ACTIVE_HIGH = 1,    /*!<button active level: high level*/
    BUTTON_ACTIVE_LOW = 0,     /*!<button active level: low level*/
} button_active_t;

typedef enum {
    BUTTON_CB_PUSH = 0,   /*!<button push callback event */
    BUTTON_CB_RELEASE,    /*!<button release callback event */
    BUTTON_CB_TAP,        /*!<button quick tap callback event(will not trigger if there already is a "PRESS" event) */
    BUTTON_CB_SERIAL,     /*!<button serial trigger callback event */
} button_cb_type_t;

/**
 * @brief Init button functions
 *
 * @param gpio_num GPIO index of the pin that the button uses
 * @param active_level button hardware active level.
 *        For "BUTTON_ACTIVE_LOW" it means when the button pressed, the GPIO will read low level.
 *
 * @return A button_handle_t handle to the created button object, or NULL in case of error.
 */
button_handle_t iot_button_create(gpio_num_t gpio_num, button_active_t active_level);

/**
 * @brief Register a callback function for a serial trigger event.
 *
 * @param btn_handle handle of the button object
 * @start_after_sec define the time after which to start serial trigger action
 * @interval_tick serial trigger interval
 * @cb callback function for "TAP" action.
 * @arg Parameter for callback function
 * @note
 *        Button callback functions execute in the context of the timer service task.
 *        It is therefore essential that button callback functions never attempt to block.
 *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
 *        or specify a non zero block time when accessing a queue or a semaphore.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t iot_button_set_serial_cb(button_handle_t btn_handle, uint32_t start_after_sec, TickType_t interval_tick, button_cb cb, void* arg);

/**
 * @brief Register a callback function for a button_cb_type_t action.
 *
 * @param btn_handle handle of the button object
 * @param type callback function type
 * @param cb callback function for "TAP" action.
 * @param arg Parameter for callback function
 * @note
 *        Button callback functions execute in the context of the timer service task.
 *        It is therefore essential that button callback functions never attempt to block.
 *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
 *        or specify a non zero block time when accessing a queue or a semaphore.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t iot_button_set_evt_cb(button_handle_t btn_handle, button_cb_type_t type, button_cb cb, void* arg);

/**
 * @brief
 *
 * @param btn_handle handle of the button object
 * @param press_sec the callback function would be called if you press the button for a specified period of time
 * @param cb callback function for "PRESS" action.
 * @param arg Parameter for callback function
 *
 * @note
 *        Button callback functions execute in the context of the timer service task.
 *        It is therefore essential that button callback functions never attempt to block.
 *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
 *        or specify a non zero block time when accessing a queue or a semaphore.
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t iot_button_add_custom_cb(button_handle_t btn_handle, uint32_t press_sec, button_cb cb, void* arg);

/**
 * @brief Delete button object and free memory
 * @param btn_handle handle of the button object
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL Parameter error
 */
esp_err_t iot_button_delete(button_handle_t btn_handle);

/**
 * @brief Remove callback
 *
 * @param btn_handle The handle of the button object
 * @param type callback function event type
 *
 * @return
 *     - ESP_OK Success
 */
esp_err_t iot_button_rm_cb(button_handle_t btn_handle, button_cb_type_t type);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/**
 * class of button
 * simple usage:
 * CButton* btn = new CButton(BUTTON_IO_NUM, BUTTON_ACTIVE_LEVEL, BUTTON_SERIAL_TRIGGER, 3);
 * btn->add_cb(BUTTON_CB_PUSH, button_tap_cb, (void*) push, 50 / portTICK_PERIOD_MS);
 * btn->add_custom_cb(5, button_press_5s_cb, NULL);
 * ......
 * delete btn;
 */
class CButton
{
private:
    button_handle_t m_btn_handle;

    /**
     * prevent copy constructing
     */
    CButton(const CButton&);
    CButton& operator = (const CButton&);
public:

    /**
     * @brief constructor of CButton
     * 
     * @param gpio_num GPIO index of the pin that the button uses
     * @param active_level button hardware active level.
     *        For "BUTTON_ACTIVE_LOW" it means when the button pressed, the GPIO will read low level.
     */
    CButton(gpio_num_t gpio_num, button_active_t active_level = BUTTON_ACTIVE_LOW);
    
    ~CButton();

    /**
     * @brief Register a callback function for a button_cb_type_t action.
     *
     * @param type callback function type
     * @param cb callback function for "TAP" action.
     * @param arg Parameter for callback function
     * @note
     *        Button callback functions execute in the context of the timer service task.
     *        It is therefore essential that button callback functions never attempt to block.
     *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
     *        or specify a non zero block time when accessing a queue or a semaphore.
     * @return
     *     - ESP_OK Success
     *     - ESP_FAIL Parameter error
     */
    esp_err_t set_evt_cb(button_cb_type_t type, button_cb cb, void* arg);

    /**
     * @brief Register a callback function for a serial trigger event.
     *
     * @param btn_handle handle of the button object
     * @start_after_sec define the time after which to start serial trigger action
     * @interval_tick serial trigger interval
     * @cb callback function for "TAP" action.
     * @arg Parameter for callback function
     * @note
     *        Button callback functions execute in the context of the timer service task.
     *        It is therefore essential that button callback functions never attempt to block.
     *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
     *        or specify a non zero block time when accessing a queue or a semaphore.
     * @return
     *     - ESP_OK Success
     *     - ESP_FAIL Parameter error
     */
    esp_err_t set_serial_cb(button_cb cb, void* arg, TickType_t interval_tick, uint32_t start_after_sec);

    /**
     * @brief
     *
     * @param press_sec the callback function would be called if you press the button for a specified period of time
     * @param cb callback function for "PRESS" action.
     * @param arg Parameter for callback function
     *
     * @note
     *        Button callback functions execute in the context of the timer service task.
     *        It is therefore essential that button callback functions never attempt to block.
     *        For example, a button callback function must not call vTaskDelay(), vTaskDelayUntil(),
     *        or specify a non zero block time when accessing a queue or a semaphore.
     * @return
     *     - ESP_OK Success
     *     - ESP_FAIL Parameter error
     */
    esp_err_t add_custom_cb(uint32_t press_sec, button_cb cb, void* arg);

    /**
     * @brief Remove callback
     *
     * @param type callback function event type
     *
     * @return
     *     - ESP_OK Success
     */
    esp_err_t rm_cb(button_cb_type_t type);
};
#endif

#endif
