/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_coexist.h"
#include "esp_coexist_internal.h"

#if CONFIG_EXTERNAL_COEX_ENABLE
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "soc/gpio_periph.h"
#endif

const char *esp_coex_version_get(void)
{
    return coex_version_get();
}

esp_err_t esp_coex_preference_set(esp_coex_prefer_t prefer)
{
    return coex_preference_set((coex_prefer_t)prefer);
}

#if CONFIG_EXTERNAL_COEX_ENABLE
#define GPIO_PIN_REG(a) (GPIO_PIN0_REG + a * 0x04)
esp_err_t esp_enable_extern_coex_gpio_pin(external_coex_wire_t wire_type, esp_external_coex_gpio_set_t gpio_pin)
{
    switch (wire_type)
    {
        case EXTERN_COEX_WIRE_3:
        {
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);

            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin1], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin1, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin1, GPIO_BT_PRIORITY_IDX, false);

            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, GPIO_WLAN_ACTIVE_IDX, false, false);

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin1), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin1), GPIO_PIN1_SYNC2_BYPASS, 2);

            int ret = esp_coex_external_set(EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_HIGH);
            if (ESP_OK != ret) {
                return ESP_FAIL;
            }
            break;
        }
        case EXTERN_COEX_WIRE_2:
        {
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);

            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, GPIO_WLAN_ACTIVE_IDX, false, false);

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);

            int ret = esp_coex_external_set(EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_MID);
            if (ESP_OK != ret) {
                return ESP_FAIL;
            }
            break;
        }
        case EXTERN_COEX_WIRE_1:
        {
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);

            int ret = esp_coex_external_set(EXTERN_COEX_PTI_HIGH, EXTERN_COEX_PTI_HIGH, EXTERN_COEX_PTI_HIGH);
            if (ESP_OK != ret) {
                return ESP_FAIL;
            }
            break;
        }
        default:
        {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

esp_err_t esp_disable_extern_coex_gpio_pin()
{
    esp_coex_external_stop();

    return ESP_OK;
}
#endif/*External Coex*/
