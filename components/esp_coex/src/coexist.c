/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_coexist.h"
#include "esp_coexist_internal.h"

#if CONFIG_EXTERNAL_COEX_ENABLE
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
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

#if SOC_EXTERNAL_COEX_ADVANCE
static const char *TAG = "external_coex";

external_coex_classification s_external_coex_partner[EXTERNAL_COEX_UNKNOWN_ROLE][EXTERN_COEX_WIRE_NUM] = {
    { wire_1_leader_mode, wire_2_leader_mode, wire_3_leader_mode },
    {},
    { wire_1_follower_mode, wire_2_follower_mode, wire_3_follower_mode },
};

static esp_external_coex_advance_t g_external_coex_params = { EXTERNAL_COEX_LEADER_ROLE, 0, true };
esp_external_coex_follower_pti_t g_external_coex_follower_pti_val = { 0, 0 };

esp_err_t esp_external_coex_set_work_mode(esp_extern_coex_work_mode_t work_mode)
{
    g_external_coex_params.work_mode = work_mode;

    if(EXTERNAL_COEX_FOLLOWER_ROLE == work_mode) {
        g_external_coex_follower_pti_val.pti_val1 = 8;
        g_external_coex_follower_pti_val.pti_val2 = 13;
    }

    return ESP_OK;
}

esp_err_t esp_external_coex_set_grant_delay(uint8_t delay_us)
{
    g_external_coex_params.delay_us = delay_us;

    return ESP_OK;
}

esp_err_t esp_external_coex_set_validate_high(bool is_high_valid)
{
    g_external_coex_params.is_high_valid = is_high_valid;

    return ESP_OK;
}

bool is_legal_external_coex_gpio(external_coex_wire_t wire_type, esp_external_coex_gpio_set_t gpio_pin)
{
    external_coex_classification external_coex_configure = s_external_coex_partner[g_external_coex_params.work_mode][wire_type];

    switch (external_coex_configure)
    {
        case wire_3_leader_mode:
        {
            if(gpio_pin.in_pin0 == gpio_pin.in_pin1) {
                return false;
            }
            if(gpio_pin.in_pin0 == gpio_pin.out_pin0) {
                return false;
            }
            if(gpio_pin.in_pin1 == gpio_pin.out_pin0) {
                return false;
            }
            if(gpio_pin.in_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            if(gpio_pin.in_pin1 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            if(gpio_pin.out_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            return true;
        }
        case wire_3_follower_mode:
        {
            if(gpio_pin.in_pin0 == gpio_pin.out_pin0) {
                return false;
            }
            if(gpio_pin.in_pin0 == gpio_pin.out_pin1) {
                return false;
            }
            if(gpio_pin.out_pin0 == gpio_pin.out_pin1) {
                return false;
            }
            if(gpio_pin.in_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            if(gpio_pin.out_pin1 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            if(gpio_pin.out_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            return true;
        }
        case wire_2_leader_mode:
        case wire_2_follower_mode:
        {
            if(gpio_pin.in_pin0 == gpio_pin.out_pin0) {
                return false;
            }
            if(gpio_pin.in_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            if(gpio_pin.out_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            return true;
        }
        case wire_1_leader_mode:
        {
            if(gpio_pin.in_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            return true;
        }
        case wire_1_follower_mode:
        {
            if(gpio_pin.out_pin0 >= SOC_GPIO_PIN_COUNT) {
                return false;
            }
            return true;
        }
        default:
            return false;
    }
}

esp_err_t esp_external_coex_leader_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t in_pin0, uint32_t in_pin1, uint32_t out_pin0)
{
    esp_external_coex_set_work_mode(EXTERNAL_COEX_LEADER_ROLE);
    esp_external_coex_gpio_set_t gpio_pin;

    switch (wire_type) {
        case EXTERN_COEX_WIRE_3:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.in_pin1  = in_pin1;
            gpio_pin.out_pin0 = out_pin0;
            break;
        }
        case EXTERN_COEX_WIRE_2:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.out_pin0 = out_pin0;
            break;
        }
        case EXTERN_COEX_WIRE_1:
        {
            gpio_pin.in_pin0  = in_pin0;
            break;
        }
        default:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.in_pin1  = in_pin1;
            gpio_pin.out_pin0 = out_pin0;
            break;
        }
    }

    return esp_enable_extern_coex_gpio_pin(wire_type, gpio_pin);
}

esp_err_t esp_external_coex_follower_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t in_pin0, uint32_t out_pin0, uint32_t out_pin1)
{
    esp_external_coex_set_work_mode(EXTERNAL_COEX_FOLLOWER_ROLE);
    esp_external_coex_gpio_set_t gpio_pin;

    switch (wire_type) {
        case EXTERN_COEX_WIRE_3:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.out_pin0 = out_pin0;
            gpio_pin.out_pin1 = out_pin1;
            break;
        }
        case EXTERN_COEX_WIRE_2:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.out_pin0 = out_pin0;
            break;
        }
        case EXTERN_COEX_WIRE_1:
        {
            gpio_pin.out_pin0 = out_pin0;
            break;
        }
        default:
        {
            gpio_pin.in_pin0  = in_pin0;
            gpio_pin.out_pin0 = out_pin0;
            gpio_pin.out_pin1 = out_pin1;
            break;
        }
    }

    return esp_enable_extern_coex_gpio_pin(wire_type, gpio_pin);
}
#endif

esp_err_t esp_enable_extern_coex_gpio_pin(external_coex_wire_t wire_type, esp_external_coex_gpio_set_t gpio_pin)
{
#if SOC_EXTERNAL_COEX_ADVANCE
    if(false == is_legal_external_coex_gpio(wire_type, gpio_pin))
    {
        ESP_LOGE(TAG, "Configure external coex with unexpected gpio pin!!!\n");
        return ESP_ERR_INVALID_ARG;
    }
    phy_coex_force_rx_ant();

    esp_coex_external_params(g_external_coex_params, g_external_coex_follower_pti_val.pti_val1,
                    g_external_coex_follower_pti_val.pti_val2);
#endif

    switch (wire_type)
    {
        case EXTERN_COEX_WIRE_3:
        {
#if SOC_EXTERNAL_COEX_ADVANCE
        if(EXTERNAL_COEX_LEADER_ROLE == g_external_coex_params.work_mode) {
#endif
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, EXTERN_ACTIVE_I_IDX, false);
#else
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);
#endif

            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin1], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin1, GPIO_MODE_INPUT);

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin1, EXTERN_PRIORITY_I_IDX, false);
#else
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin1, GPIO_BT_PRIORITY_IDX, false);
#endif

            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, EXTERN_ACTIVE_O_IDX, false, false);
#else
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, GPIO_WLAN_ACTIVE_IDX, false, false);
#endif

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin1), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin1), GPIO_PIN1_SYNC2_BYPASS, 2);
#if SOC_EXTERNAL_COEX_ADVANCE
        }
        else if(EXTERNAL_COEX_FOLLOWER_ROLE == g_external_coex_params.work_mode) {
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);

            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, EXTERN_ACTIVE_I_IDX, false);

            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, EXTERN_ACTIVE_O_IDX, false, false);

            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin1], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin1, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin1));
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin1, EXTERN_PRIORITY_O_IDX, false, false);

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);
        }
#else
#endif
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

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, EXTERN_ACTIVE_I_IDX, false);
#else
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);
#endif

            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, EXTERN_ACTIVE_O_IDX, false, false);
#else
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, GPIO_WLAN_ACTIVE_IDX, false, false);
#endif

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
#if SOC_EXTERNAL_COEX_ADVANCE
        if(EXTERNAL_COEX_LEADER_ROLE == g_external_coex_params.work_mode) {
#endif
            /*Input gpio pin setup --> GPIO_BT_PRIORITY_IDX：GPIO_BT_ACTIVE_IDX*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.in_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.in_pin0, GPIO_MODE_INPUT);

#if SOC_EXTERNAL_COEX_ADVANCE
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, EXTERN_ACTIVE_I_IDX, false);
#else
            esp_rom_gpio_connect_in_signal(gpio_pin.in_pin0, GPIO_BT_ACTIVE_IDX, false);
#endif

            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC1_BYPASS, 2);
            REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.in_pin0), GPIO_PIN1_SYNC2_BYPASS, 2);
#if SOC_EXTERNAL_COEX_ADVANCE
        }
        else if(EXTERNAL_COEX_FOLLOWER_ROLE == g_external_coex_params.work_mode) {
            /*Output gpio pin setup --> GPIO_WLAN_ACTIVE_IDX: 1 BT, 0 WiFi*/
            gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.out_pin0], PIN_FUNC_GPIO);
            gpio_set_direction(gpio_pin.out_pin0, GPIO_MODE_OUTPUT);
            REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.out_pin0));
            esp_rom_gpio_connect_out_signal(gpio_pin.out_pin0, EXTERN_ACTIVE_O_IDX, false, false);
        }
#else
#endif

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
#if SOC_EXTERNAL_COEX_ADVANCE
    phy_coex_dismiss_rx_ant();
#endif
    esp_coex_external_stop();

    return ESP_OK;
}
#endif/*External Coex*/
