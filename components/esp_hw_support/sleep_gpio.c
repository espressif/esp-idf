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
#include "soc/soc_caps.h"

#include "sdkconfig.h"

#include "driver/gpio.h"
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
