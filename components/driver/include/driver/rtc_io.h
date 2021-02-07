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

#ifndef _DRIVER_RTC_GPIO_H_
#define _DRIVER_RTC_GPIO_H_

#include <stdint.h>
#include "esp_err.h"
#include "soc/soc_caps.h"
#include "soc/rtc_io_periph.h"
#include "hal/rtc_io_types.h"
#include "driver/gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Determine if the specified GPIO is a valid RTC GPIO.
 *
 * @param gpio_num GPIO number
 * @return true if GPIO is valid for RTC GPIO use. false otherwise.
 */
static inline bool rtc_gpio_is_valid_gpio(gpio_num_t gpio_num)
{
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    return (gpio_num < GPIO_PIN_COUNT && rtc_io_num_map[gpio_num] >= 0);
#else
    return false;
#endif
}

#define RTC_GPIO_IS_VALID_GPIO(gpio_num) rtc_gpio_is_valid_gpio(gpio_num) // Deprecated, use rtc_gpio_is_valid_gpio()

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
/**
 * @brief Get RTC IO index number by gpio number.
 *
 * @param gpio_num GPIO number
 * @return
 *        >=0: Index of rtcio.
 *        -1 : The gpio is not rtcio.
 */
static inline int rtc_io_number_get(gpio_num_t gpio_num)
{
    return rtc_io_num_map[gpio_num];
}

/**
 * @brief Init a GPIO as RTC GPIO
 *
 * This function must be called when initializing a pad for an analog function.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_init(gpio_num_t gpio_num);

/**
 * @brief Init a GPIO as digital GPIO
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num);

/**
 * @brief Get the RTC IO input level
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - 1 High level
 *     - 0 Low level
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
uint32_t rtc_gpio_get_level(gpio_num_t gpio_num);

/**
 * @brief Set the RTC IO output level
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 * @param  level output level
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level);

/**
 * @brief    RTC GPIO set direction
 *
 * Configure RTC GPIO direction, such as output only, input only,
 * output and input.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 * @param  mode GPIO direction
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode);

/**
 * @brief RTC GPIO set direction in deep sleep mode or disable sleep status (default).
 *        In some application scenarios, IO needs to have another states during deep sleep.
 *
 * NOTE: ESP32 support INPUT_ONLY mode.
 *       ESP32S2 support INPUT_ONLY, OUTPUT_ONLY, INPUT_OUTPUT mode.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 * @param  mode GPIO direction
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_set_direction_in_sleep(gpio_num_t gpio_num, rtc_gpio_mode_t mode);

/**
 * @brief  RTC GPIO pullup enable
 *
 * This function only works for RTC IOs. In general, call gpio_pullup_en,
 * which will work both for normal GPIOs and RTC IOs.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num);

/**
 * @brief  RTC GPIO pulldown enable
 *
 * This function only works for RTC IOs. In general, call gpio_pulldown_en,
 * which will work both for normal GPIOs and RTC IOs.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num);

/**
 * @brief  RTC GPIO pullup disable
 *
 * This function only works for RTC IOs. In general, call gpio_pullup_dis,
 * which will work both for normal GPIOs and RTC IOs.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num);

/**
 * @brief  RTC GPIO pulldown disable
 *
 * This function only works for RTC IOs. In general, call gpio_pulldown_dis,
 * which will work both for normal GPIOs and RTC IOs.
 *
 * @param  gpio_num GPIO number (e.g. GPIO_NUM_12)
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num);

/**
 * @brief Set RTC GPIO pad drive capability
 *
 * @param gpio_num GPIO number, only support output GPIOs
 * @param strength Drive capability of the pad
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t rtc_gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength);

/**
 * @brief Get RTC GPIO pad drive capability
 *
 * @param gpio_num GPIO number, only support output GPIOs
 * @param strength Pointer to accept drive capability of the pad
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t rtc_gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t* strength);

#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_RTCIO_HOLD_SUPPORTED

/**
 * @brief Enable hold function on an RTC IO pad
 *
 * Enabling HOLD function will cause the pad to latch current values of
 * input enable, output enable, output value, function, drive strength values.
 * This function is useful when going into light or deep sleep mode to prevent
 * the pin configuration from changing.
 *
 * @param gpio_num GPIO number (e.g. GPIO_NUM_12)
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num);

/**
 * @brief Disable hold function on an RTC IO pad
 *
 * Disabling hold function will allow the pad receive the values of
 * input enable, output enable, output value, function, drive strength from
 * RTC_IO peripheral.
 *
 * @param gpio_num GPIO number (e.g. GPIO_NUM_12)
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num);

/**
 * @brief Helper function to disconnect internal circuits from an RTC IO
 * This function disables input, output, pullup, pulldown, and enables
 * hold feature for an RTC IO.
 * Use this function if an RTC IO needs to be disconnected from internal
 * circuits in deep sleep, to minimize leakage current.
 *
 * In particular, for ESP32-WROVER module, call
 * rtc_gpio_isolate(GPIO_NUM_12) before entering deep sleep, to reduce
 * deep sleep current.
 *
 * @param gpio_num GPIO number (e.g. GPIO_NUM_12).
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if GPIO is not an RTC IO
 */
esp_err_t rtc_gpio_isolate(gpio_num_t gpio_num);

/**
 * @brief Enable force hold signal for all RTC IOs
 *
 * Each RTC pad has a "force hold" input signal from the RTC controller.
 * If this signal is set, pad latches current values of input enable,
 * function, output enable, and other signals which come from the RTC mux.
 * Force hold signal is enabled before going into deep sleep for pins which
 * are used for EXT1 wakeup.
 */
esp_err_t rtc_gpio_force_hold_all(void);

/**
 * @brief Disable force hold signal for all RTC IOs
 */
esp_err_t rtc_gpio_force_hold_dis_all(void);

#endif // SOC_RTCIO_HOLD_SUPPORTED

#if SOC_RTCIO_WAKE_SUPPORTED

/**
 * @brief Enable wakeup from sleep mode using specific GPIO
 * @param gpio_num  GPIO number
 * @param intr_type  Wakeup on high level (GPIO_INTR_HIGH_LEVEL) or low level
 *                   (GPIO_INTR_LOW_LEVEL)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if gpio_num is not an RTC IO, or intr_type is not
 *        one of GPIO_INTR_HIGH_LEVEL, GPIO_INTR_LOW_LEVEL.
 */
esp_err_t rtc_gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type);

/**
 * @brief Disable wakeup from sleep mode using specific GPIO
 * @param gpio_num  GPIO number
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if gpio_num is not an RTC IO
 */
esp_err_t rtc_gpio_wakeup_disable(gpio_num_t gpio_num);

#endif // SOC_RTCIO_WAKE_SUPPORTED

#ifdef __cplusplus
}
#endif

#endif
