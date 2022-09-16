/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/param.h>

#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "soc/soc_caps.h"

#include "sdkconfig.h"

#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "esp_private/gpio.h"
#include "esp_private/sleep_gpio.h"
#include "esp_private/spi_flash_os.h"

static const char *TAG = "sleep";

#if SOC_GPIO_SUPPORT_SLP_SWITCH

#if CONFIG_GPIO_ESP32_SUPPORT_SWITCH_SLP_PULL
void gpio_sleep_mode_config_apply(void)
{
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_GPIO(gpio_num)) {
            gpio_sleep_pupd_config_apply(gpio_num);
        }
    }
}

IRAM_ATTR void gpio_sleep_mode_config_unapply(void)
{
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_GPIO(gpio_num)) {
            gpio_sleep_pupd_config_unapply(gpio_num);
        }
    }
}
#endif

void esp_sleep_config_gpio_isolate(void)
{
    ESP_EARLY_LOGI(TAG, "Configure to isolate all GPIO pins in sleep state");
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_GPIO(gpio_num)) {
            gpio_sleep_set_direction(gpio_num, GPIO_MODE_DISABLE);
            gpio_sleep_set_pull_mode(gpio_num, GPIO_FLOATING);
        }
    }

#if CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND && CONFIG_SPIRAM
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_CS1), GPIO_PULLUP_ONLY);
#endif // CONFIG_ESP_SLEEP_PSRAM_LEAKAGE_WORKAROUND && CONFIG_SPIRAM

#if CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_CS0), GPIO_PULLUP_ONLY);
#endif // CONFIG_ESP_SLEEP_FLASH_LEAKAGE_WORKAROUND

#if CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_CLK), GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_Q),   GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D),   GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_HD),  GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_WP),  GPIO_PULLUP_ONLY);
#if CONFIG_SPIRAM_MODE_OCT || CONFIG_ESPTOOLPY_FLASHMODE_OPI
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_DQS), GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D4),  GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D5),  GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D6),  GPIO_PULLUP_ONLY);
    gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D7),  GPIO_PULLUP_ONLY);
#endif // CONFIG_SPIRAM_MODE_OCT || CONFIG_ESPTOOLPY_FLASHMODE_OPI
#endif // CONFIG_ESP_SLEEP_MSPI_NEED_ALL_IO_PU
}

void esp_sleep_enable_gpio_switch(bool enable)
{
    ESP_EARLY_LOGI(TAG, "%s automatic switching of GPIO sleep configuration", enable ? "Enable" : "Disable");
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_GPIO(gpio_num)) {
            if (enable) {
                gpio_sleep_sel_en(gpio_num);
            } else {
                gpio_sleep_sel_dis(gpio_num);
            }
        }
    }
}

#endif // SOC_GPIO_SUPPORT_SLP_SWITCH

// IDF does not officially support esp32h2 in v5.0
#if !CONFIG_IDF_TARGET_ESP32H2
IRAM_ATTR void esp_sleep_isolate_digital_gpio(void)
{
    gpio_hal_context_t gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };

    /* no need to do isolate if digital IOs are not being held in deep sleep */
    if (!gpio_hal_deep_sleep_hold_is_en(&gpio_hal)) {
        return;
    }

    /**
     * there is a situation where we cannot isolate digital IO before deep sleep:
     * - task stack is located in external ram(mspi ram), since we will isolate mspi io
     *
     * assert here instead of returning directly, because if digital IO is not isolated,
     * the bottom current of deep sleep will be higher than light sleep, and there is no
     * reason to use deep sleep at this time.
     */
    assert(esp_ptr_internal(&gpio_hal) && "If hold digital IO, the stack of the task calling esp_deep_sleep_start must be in internal ram!");

    /* isolate digital IO that is not held(keep the configuration of digital IOs held by users) */
    for (gpio_num_t gpio_num = GPIO_NUM_0; gpio_num < GPIO_NUM_MAX; gpio_num++) {
        if (GPIO_IS_VALID_DIGITAL_IO_PAD(gpio_num) && !gpio_hal_is_digital_io_hold(&gpio_hal, gpio_num)) {
            /* disable I/O */
            gpio_hal_input_disable(&gpio_hal, gpio_num);
            gpio_hal_output_disable(&gpio_hal, gpio_num);

            /* disable pull up/down */
            gpio_hal_pullup_dis(&gpio_hal, gpio_num);
            gpio_hal_pulldown_dis(&gpio_hal, gpio_num);

            /* make pad work as gpio(otherwise, deep sleep bottom current will rise) */
            gpio_hal_func_sel(&gpio_hal, gpio_num, PIN_FUNC_GPIO);
        }
    }
}
#endif
