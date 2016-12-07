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
 * @brief Pullup/pulldown information for a single GPIO pad
 */
typedef struct {
    uint32_t reg;       /*!< Register of Rtc Pad */
    uint32_t mux;       /*!< Mux seletct the Rtc pad is Digital Pad or Rtc pad */
    uint32_t func;      /*!< Select Rtc Pad Func */
    uint32_t ie;        /*!< Input Enable */
    uint32_t pullup;    /*!< Pullup Enable */
    uint32_t pulldown;  /*!< PullDown Enable */
    int rtc_num;        /*!< The Rtc number */
} rtc_gpio_desc_t;

typedef enum {
    RTC_GPIO_MODE_INPUT_ONLY , /*!< Pad output           */
    RTC_GPIO_MODE_OUTPUT_ONLY, /*!< Pad input          */
    RTC_GPIO_MODE_INPUT_OUTUT, /*!< Pad pull output + input      */
    RTC_GPIO_MODE_DISABLED, /*!< Pad (output + input) disable       */
} rtc_gpio_mode_t;

#define RTC_GPIO_IS_VALID_GPIO(gpio_num)      ((gpio_num < GPIO_PIN_COUNT && rtc_gpio_desc[gpio_num].reg != 0))   //to decide whether it is a valid GPIO number

extern const rtc_gpio_desc_t rtc_gpio_desc[GPIO_PIN_COUNT] ;

/*
 * @brief Init a gpio as rtc gpio
 *
 *        when init a pad as analog function,need to call this funciton
 *
 * @param  gpio_num gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t rtc_gpio_init(gpio_num_t gpio_num);

/**
 * @brief Init a gpio as digital gpio
 *
 * @param  gpio_num gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - ESP_OK success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num);

/**
 * @brief Get the rtc io input level
 *
 * @param  gpio_num gpio_num GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - 1 High level
 *     - 0 Low level
 */
uint32_t rtc_gpio_get_level(gpio_num_t gpio_num);

/**
 * @brief Set the rtc io output level
 *
 * @param  gpio_num  GPIO number. If you want to set the trigger type of e.g. of GPIO16, gpio_num should be GPIO_NUM_12 (12);
 * @param  level output level;
 *
 * @return
 *     - 1 High level
 *     - 0 Low level
 */
esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level);

/**
 * @brief    Rtc gpio set direction
 *
 * Configure Rtc gpio direction,such as output_only,input_only,output_and_input
 *
 * @param  gpio_num  Configure GPIO pins number, it should be GPIO number. If you want to set direction of e.g. GPIO12, gpio_num should be GPIO_NUM_12 (12);
 * @param  mode GPIO direction
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG GPIO error
 */
esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode);

/**
 * @brief  Rtc gpio pullup enable
 *
 * If the user needs to configure the GPIO pull ,Please call gpio_set_pull_mode.This function will be called in gpio_set_pull
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO12, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - True  the gpio number is Rts pad
 *     - False the gpio number is Digital pad
 */
esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num);

/**
 * @brief  Rtc gpio pulldown enable
 *
 * If the user needs to configure the GPIO pull ,Please call gpio_set_pull_mode.This function will be called in gpio_set_pull
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO12, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - True  the gpio number is Rts pad
 *     - False the gpio number is Digital pad
 */
esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num);

/**
 * @brief  Rtc gpio pullup clear
 *
 * If the user needs to configure the GPIO pull ,Please call gpio_set_pull_mode.This function will be called in gpio_set_pull
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO12, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - True  the gpio number is Rts pad
 *     - False the gpio number is Digital pad
 */
esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num);

/**
 * @brief  Rtc gpio pulldown clear
 *
 * If the user needs to configure the GPIO pull ,Please call gpio_set_pull_mode.This function will be called in gpio_set_pull
 *
 * @param  gpio_num GPIO number. If you want to set pull up or down mode for e.g. GPIO12, gpio_num should be GPIO_NUM_12 (12);
 *
 * @return
 *     - True  the gpio number is Rts pad
 *     - False the gpio number is Digital pad
 */
esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num);

#ifdef __cplusplus
}
#endif

#endif
