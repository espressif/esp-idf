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
#include "hal/rtc_io_hal.h"
#include "soc/rtc_io_periph.h"

#include "hal/rtc_hal.h"

#include "esp_private/gpio.h"
#include "esp_private/sleep_gpio.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/startup_internal.h"
#include "bootloader_flash.h"

static const char *TAG = "sleep";

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
#if SOC_SPI_MEM_SUPPORT_OPI_MODE
    bool octal_mspi_required = bootloader_flash_is_octal_mode_enabled();
#if CONFIG_SPIRAM_MODE_OCT
    octal_mspi_required |= true;
#endif // CONFIG_SPIRAM_MODE_OCT
    if (octal_mspi_required) {
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_DQS), GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D4),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D5),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D6),  GPIO_PULLUP_ONLY);
        gpio_sleep_set_pull_mode(esp_mspi_get_io(ESP_MSPI_IO_D7),  GPIO_PULLUP_ONLY);
    }
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE
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

#if !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP
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
#endif // !SOC_GPIO_SUPPORT_HOLD_SINGLE_IO_IN_DSLP

void esp_deep_sleep_wakeup_io_reset(void)
{
#if SOC_PM_SUPPORT_EXT1_WAKEUP
    uint32_t rtc_io_mask = rtc_hal_ext1_get_wakeup_pins();
    // Disable ext1 wakeup before releasing hold, such that wakeup status can reflect the correct wakeup pin
    rtc_hal_ext1_clear_wakeup_pins();
    for (int gpio_num = 0; gpio_num < SOC_GPIO_PIN_COUNT && rtc_io_mask != 0; ++gpio_num) {
        int rtcio_num = rtc_io_num_map[gpio_num];
        if ((rtc_io_mask & BIT(rtcio_num)) == 0) {
            continue;
        }
        rtcio_hal_hold_disable(rtcio_num);
        rtc_io_mask &= ~BIT(rtcio_num);
    }
#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
    uint32_t dl_io_mask = SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK;
    gpio_hal_context_t gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };
    while (dl_io_mask) {
        int gpio_num = __builtin_ffs(dl_io_mask) - 1;
        bool wakeup_io_enabled = gpio_hal_deepsleep_wakeup_is_enabled(&gpio_hal, gpio_num);
        if (wakeup_io_enabled) {
            // Disable the wakeup before releasing hold, such that wakeup status can reflect the correct wakeup pin
            gpio_hal_deepsleep_wakeup_disable(&gpio_hal, gpio_num);
            gpio_hal_hold_dis(&gpio_hal, gpio_num);
        }
        dl_io_mask &= ~BIT(gpio_num);
    }
#endif
}

#if CONFIG_ESP_SLEEP_GPIO_RESET_WORKAROUND || CONFIG_PM_SLP_DISABLE_GPIO
ESP_SYSTEM_INIT_FN(esp_sleep_startup_init, SECONDARY, BIT(0), 105)
{
/* If the TOP domain is powered off, the GPIO will also be powered off during sleep,
   and all configurations in the sleep state of GPIO will not take effect.*/
#if !CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // Configure to isolate (disable the Input/Output/Pullup/Pulldown
    // function of the pin) all GPIO pins in sleep state
    esp_sleep_config_gpio_isolate();
#endif
    // Enable automatic switching of GPIO configuration
    esp_sleep_enable_gpio_switch(true);
    return ESP_OK;
}
#endif
