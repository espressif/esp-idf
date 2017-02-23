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
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pin function information for a single GPIO pad's RTC functions.
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
typedef struct {
    uint32_t reg;       /*!< Register of RTC pad, or 0 if not an RTC GPIO */
    uint32_t mux;       /*!< Bit mask for selecting digital pad or RTC pad */
    uint32_t func;      /*!< Shift of pad function (FUN_SEL) field */
    uint32_t ie;        /*!< Mask of input enable */
    uint32_t pullup;    /*!< Mask of pullup enable */
    uint32_t pulldown;  /*!< Mask of pulldown enable */
    uint32_t slpsel;    /*!< If slpsel bit is set, slpie will be used as pad input enabled signal in sleep mode */
    uint32_t slpie;     /*!< Mask of input enable in sleep mode */
    uint32_t hold;      /*!< Mask of hold_force bit for RTC IO in RTC_CNTL_HOLD_FORCE_REG */
    int rtc_num;        /*!< RTC IO number, or -1 if not an RTC GPIO */
} rtc_gpio_desc_t;

typedef enum {
    RTC_GPIO_MODE_INPUT_ONLY , /*!< Pad output */
    RTC_GPIO_MODE_OUTPUT_ONLY, /*!< Pad input */
    RTC_GPIO_MODE_INPUT_OUTUT, /*!< Pad pull output + input */
    RTC_GPIO_MODE_DISABLED,    /*!< Pad (output + input) disable */
} rtc_gpio_mode_t;

/**
 * @brief Provides access to a constant table of RTC I/O pin
 * function information.
 *
 * This is an internal function of the driver, and is not usually useful
 * for external use.
 */
extern const rtc_gpio_desc_t rtc_gpio_desc[GPIO_PIN_COUNT];

/**
 * @brief Determine if the specified GPIO is a valid RTC GPIO.
 *
 * @param gpio_num GPIO number
 * @return true if GPIO is valid for RTC GPIO use. talse otherwise.
 */
inline static bool rtc_gpio_is_valid_gpio(gpio_num_t gpio_num)
{
    return gpio_num < GPIO_PIN_COUNT
        && rtc_gpio_desc[gpio_num].reg != 0;
}

#define RTC_GPIO_IS_VALID_GPIO(gpio_num) rtc_gpio_is_valid_gpio(gpio_num) // Deprecated, use rtc_gpio_is_valid_gpio()

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
 * @brief Disable "hold" signal for all RTC IOs
 *
 * Each RTC pad has a "hold" input signal from the RTC controller.
 * If hold signal is set, pad latches current values of input enable,
 * function, output enable, and other signals which come from the RTC mux.
 * Hold signal is enabled before going into deep sleep for pins which
 * are used for EXT1 wakeup.
 */
void rtc_gpio_unhold_all();


#ifdef __cplusplus
}
#endif

#endif
