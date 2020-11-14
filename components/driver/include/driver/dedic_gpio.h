// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of Dedicated GPIO bundle
 */
typedef struct dedic_gpio_bundle_t *dedic_gpio_bundle_handle_t;

/**
 * @brief Type of Dedicated GPIO bundle configuration
 */
typedef struct {
    const int *gpio_array; /*!< Array of GPIO numbers, gpio_array[0] ~ gpio_array[size-1] <=> low_dedic_channel_num ~ high_dedic_channel_num */
    size_t array_size;     /*!< Number of GPIOs in gpio_array */
    struct {
        int in_en: 1;      /*!< Enable input */
        int in_invert: 1;  /*!< Invert input signal */
        int out_en: 1;     /*!< Enable output */
        int out_invert: 1; /*!< Invert output signal */
    } flags; /*!< Flags to control specific behaviour of GPIO bundle */
} dedic_gpio_bundle_config_t;

/**
 * @brief Create GPIO bundle and return the handle
 *
 * @param[in] config Configuration of GPIO bundle
 * @param[out] ret_bundle Returned handle of the new created GPIO bundle
 * @return
 *      - ESP_OK: Create GPIO bundle successfully
 *      - ESP_ERR_INVALID_ARG: Create GPIO bundle failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create GPIO bundle failed because of no capable memory
 *      - ESP_ERR_NOT_FOUND: Create GPIO bundle failed because of no enough continuous dedicated channels
 *      - ESP_FAIL: Create GPIO bundle failed because of other error
 *
 * @note One has to enable at least input or output mode in "config" parameter.
 */
esp_err_t dedic_gpio_new_bundle(const dedic_gpio_bundle_config_t *config, dedic_gpio_bundle_handle_t *ret_bundle);

/**
 * @brief Destory GPIO bundle
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @return
 *      - ESP_OK: Destory GPIO bundle successfully
 *      - ESP_ERR_INVALID_ARG: Destory GPIO bundle failed because of invalid argument
 *      - ESP_FAIL: Destory GPIO bundle failed because of other error
 */
esp_err_t dedic_gpio_del_bundle(dedic_gpio_bundle_handle_t bundle);

/**@{*/
/**
 * @brief Get allocated channel mask
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @param[out] mask Returned mask value for on specific direction (in or out)
 * @return
 *      - ESP_OK: Get channel mask successfully
 *      - ESP_ERR_INVALID_ARG: Get channel mask failed because of invalid argument
 *      - ESP_FAIL: Get channel mask failed because of other error
 *
 * @note Each bundle should have at least one mask (in or/and out), based on bundle configuration.
 * @note With the returned mask, user can directly invoke LL function like "cpu_ll_write_dedic_gpio_mask"
 *       or write assembly code with dedicated GPIO instructions, to get better performance on GPIO manipulation.
 */
esp_err_t dedic_gpio_get_out_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask);
esp_err_t dedic_gpio_get_in_mask(dedic_gpio_bundle_handle_t bundle, uint32_t *mask);
/**@}*/

/**
 * @brief Write value to GPIO bundle
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @param[in] mask Mask of the GPIOs to be written in the given bundle
 * @param[in] value Value to write to given GPIO bundle, low bit represents low member in the bundle
 *
 * @note The mask is seen from the view of GPIO bundle.
 *       For example, bundleA contains [GPIO10, GPIO12, GPIO17], to set GPIO17 individually, the mask should be 0x04.
 * @note For performance reasons, this function doesn't check the validity of any parameters, and is placed in IRAM.
 */
void dedic_gpio_bundle_write(dedic_gpio_bundle_handle_t bundle, uint32_t mask, uint32_t value) IRAM_ATTR;

/**
 * @brief Read the value that output from the given GPIO bundle
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @return Value that output from the GPIO bundle, low bit represents low member in the bundle
 *
 * @note For performance reasons, this function doesn't check the validity of any parameters, and is placed in IRAM.
 */
uint32_t dedic_gpio_bundle_read_out(dedic_gpio_bundle_handle_t bundle) IRAM_ATTR;

/**
 * @brief Read the value that input to the given GPIO bundle
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @return Value that input to the GPIO bundle, low bit represents low member in the bundle
 *
 * @note For performance reasons, this function doesn't check the validity of any parameters, and is placed in IRAM.
 */
uint32_t dedic_gpio_bundle_read_in(dedic_gpio_bundle_handle_t bundle) IRAM_ATTR;

#if SOC_DEDIC_GPIO_HAS_INTERRUPT

/**
 * @brief Supported type of dedicated GPIO interrupt
 */
typedef enum {
    DEDIC_GPIO_INTR_NONE,          /*!< No interrupt */
    DEDIC_GPIO_INTR_LOW_LEVEL = 2, /*!< Interrupt on low level */
    DEDIC_GPIO_INTR_HIGH_LEVEL,    /*!< Interrupt on high level */
    DEDIC_GPIO_INTR_NEG_EDGE,      /*!< Interrupt on negedge */
    DEDIC_GPIO_INTR_POS_EDGE,      /*!< Interrupt on posedge */
    DEDIC_GPIO_INTR_BOTH_EDGE      /*!< Interrupt on both negedge and posedge */
} dedic_gpio_intr_type_t;

/**
 * @brief Type of dedicated GPIO ISR callback function
 *
 * @param bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @param index Index of the GPIO in its corresponding bundle (count from 0)
 * @param args User defined arguments for the callback function. It's passed through `dedic_gpio_bundle_set_interrupt_and_callback`
 * @return If a high priority task is woken up by the callback function
 */
typedef bool (*dedic_gpio_isr_callback_t)(dedic_gpio_bundle_handle_t bundle, uint32_t index, void *args);

/**
 * @brief Set interrupt and callback function for GPIO bundle
 *
 * @param[in] bundle Handle of GPIO bundle that returned from "dedic_gpio_new_bundle"
 * @param[in] mask Mask of the GPIOs in the given bundle
 * @param[in] intr_type Interrupt type, set to DEDIC_GPIO_INTR_NONE can disable interrupt
 * @param[in] cb_isr Callback function, which got invoked in ISR context. A NULL pointer here will bypass the callback
 * @param[in] cb_args User defined argument to be passed to the callback function
 *
 * @note This function is only valid for bundle with input mode enabled. See "dedic_gpio_bundle_config_t"
 * @note The mask is seen from the view of GPIO Bundle.
 *       For example, bundleA contains [GPIO10, GPIO12, GPIO17], to set GPIO17 individually, the mask should be 0x04.
 *
 * @return
 *      - ESP_OK: Set GPIO interrupt and callback function successfully
 *      - ESP_ERR_INVALID_ARG: Set GPIO interrupt and callback function failed because of invalid argument
 *      - ESP_FAIL: Set GPIO interrupt and callback function failed because of other error
 */
esp_err_t dedic_gpio_bundle_set_interrupt_and_callback(dedic_gpio_bundle_handle_t bundle, uint32_t mask, dedic_gpio_intr_type_t intr_type, dedic_gpio_isr_callback_t cb_isr, void *cb_args);

#endif // SOC_DEDIC_GPIO_HAS_INTERRUPT

#ifdef __cplusplus
}
#endif
