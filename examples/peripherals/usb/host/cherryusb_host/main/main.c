/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#if CONFIG_EXAMPLE_HAL_USE_ESP32_S3_USB_OTG
#include "driver/gpio.h"
#define BOOST_EN 13
#define DEV_VBUS_EN 12
#define LIMIT_EN 17
#define USB_SEL 18
#endif

#include "usbh_core.h"
#include "usbh_hid.h"

static char *TAG = "HOST";

void app_main(void)
{
#if CONFIG_EXAMPLE_HAL_USE_ESP32_S3_USB_OTG
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = 0,
        .pull_up_en = 0,
        .pin_bit_mask = (1ULL << BOOST_EN) | (1ULL << DEV_VBUS_EN) | (1ULL << LIMIT_EN) | (1ULL << USB_SEL),
    };
    gpio_config(&io_conf);
    gpio_set_level(BOOST_EN, 0);
    gpio_set_level(DEV_VBUS_EN, 1);
    gpio_set_level(LIMIT_EN, 1);
    gpio_set_level(USB_SEL, 1);
#endif

#if CONFIG_EXAMPLE_CHERRYUSB_INIT_DEINIT_LOOP
    while (1)
#endif
    {
#if CONFIG_EXAMPLE_USB_HOST_RHPORT_HS
        usbh_initialize(0, ESP_USB_HS0_BASE);
#else
        usbh_initialize(0, ESP_USB_FS0_BASE);
#endif

        ESP_LOGI(TAG, "Init usb");

#if CONFIG_EXAMPLE_CHERRYUSB_INIT_DEINIT_LOOP
        for (int i = 10; i >= 0; i--) {
            ESP_LOGW(TAG, "Deinit usb after %d seconds...", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGW(TAG, "Deinit usb");
        usbh_deinitialize(0);
#endif
    }

}
