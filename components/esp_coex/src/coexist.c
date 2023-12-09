/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_coexist.h"
#include "private/esp_coexist_internal.h"

#if CONFIG_EXTERNAL_COEX_ENABLE
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "soc/gpio_periph.h"
#include "soc/gpio_struct.h"
#include "esp_attr.h"
#endif

#if SOC_EXTERNAL_COEX_ADVANCE
#define EXTERNAL_COEX_SIGNAL_I0_IDX           EXTERN_ACTIVE_I_IDX
#define EXTERNAL_COEX_SIGNAL_I1_IDX           EXTERN_PRIORITY_I_IDX
#define EXTERNAL_COEX_SIGNAL_O0_IDX           EXTERN_ACTIVE_O_IDX
#define EXTERNAL_COEX_SIGNAL_O1_IDX           EXTERN_PRIORITY_O_IDX
#else
#define EXTERNAL_COEX_SIGNAL_I0_IDX           GPIO_BT_ACTIVE_IDX
#define EXTERNAL_COEX_SIGNAL_I1_IDX           GPIO_BT_PRIORITY_IDX
#define EXTERNAL_COEX_SIGNAL_O0_IDX           GPIO_WLAN_ACTIVE_IDX
#endif

#if SOC_EXTERNAL_COEX_LEADER_TX_LINE
#define EXTERNAL_COEX_SIGNAL_O1_TXLINE_IDX    BB_DIAG9_IDX
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
static const char *TAG = "external_coex";

static esp_external_coex_advance_t g_external_coex_params = { EXTERNAL_COEX_LEADER_ROLE, 0, true };

esp_err_t esp_external_coex_set_work_mode(esp_extern_coex_work_mode_t work_mode)
{
#if !SOC_EXTERNAL_COEX_ADVANCE
    if(work_mode != EXTERNAL_COEX_LEADER_ROLE)
    {
        return ESP_ERR_INVALID_ARG;
    }
#endif
    g_external_coex_params.work_mode = work_mode;
    return ESP_OK;
}

bool is_legal_external_coex_gpio(external_coex_wire_t wire_type, esp_external_coex_gpio_set_t gpio_pin)
{
    switch (wire_type)
    {
        case EXTERN_COEX_WIRE_4:
        {
            if(!GPIO_IS_VALID_GPIO(gpio_pin.tx_line)
                || gpio_pin.tx_line == gpio_pin.priority || gpio_pin.tx_line == gpio_pin.grant || gpio_pin.tx_line == gpio_pin.request) {
                return false;
            }
        }
        __attribute__((fallthrough));
        case EXTERN_COEX_WIRE_3:
        {
            if(!GPIO_IS_VALID_GPIO(gpio_pin.priority) || gpio_pin.priority == gpio_pin.grant || gpio_pin.priority == gpio_pin.request) {
                return false;
            }
        }
        __attribute__((fallthrough));
        case EXTERN_COEX_WIRE_2:
        {
            if(!GPIO_IS_VALID_GPIO(gpio_pin.grant) || gpio_pin.grant == gpio_pin.request) {
                return false;
            }
        }
        __attribute__((fallthrough));
        case EXTERN_COEX_WIRE_1:
        {
            if(!GPIO_IS_VALID_GPIO(gpio_pin.request)) {
                return false;
            }
            break;
        }
        default:
            return false;
    }
    return true;
}

#if SOC_EXTERNAL_COEX_ADVANCE
esp_err_t esp_external_coex_set_gpio_pin(esp_external_coex_gpio_set_t *gpio_pin, external_coex_wire_t wire_type, uint32_t request, uint32_t priority, uint32_t grant)
{
    switch (wire_type) {
        case EXTERN_COEX_WIRE_3:
            gpio_pin->priority  = priority;
            __attribute__((fallthrough));
        case EXTERN_COEX_WIRE_2:
            gpio_pin->grant = grant;
            __attribute__((fallthrough));
        case EXTERN_COEX_WIRE_1:
        {
            gpio_pin->request  = request;
            break;
        }
        default:
        {
            gpio_pin->request  = request;
            gpio_pin->priority  = priority;
            gpio_pin->grant = grant;
            break;
        }
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

esp_err_t esp_external_coex_leader_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t request, uint32_t priority, uint32_t grant)
{
    esp_external_coex_set_work_mode(EXTERNAL_COEX_LEADER_ROLE);
    esp_external_coex_gpio_set_t gpio_pin;
    esp_external_coex_set_gpio_pin(&gpio_pin, wire_type, request, priority, grant);
    return esp_enable_extern_coex_gpio_pin(wire_type, gpio_pin);
}

esp_err_t esp_external_coex_follower_role_set_gpio_pin(external_coex_wire_t wire_type, uint32_t request, uint32_t priority, uint32_t grant)
{
    esp_external_coex_set_work_mode(EXTERNAL_COEX_FOLLOWER_ROLE);
    esp_external_coex_gpio_set_t gpio_pin;
    esp_external_coex_set_gpio_pin(&gpio_pin, wire_type, request, priority, grant);
    return esp_enable_extern_coex_gpio_pin(wire_type, gpio_pin);
}
#endif /* SOC_EXTERNAL_COEX_ADVANCE */

esp_err_t esp_enable_extern_coex_gpio_pin(external_coex_wire_t wire_type, esp_external_coex_gpio_set_t gpio_pin)
{
    if(false == is_legal_external_coex_gpio(wire_type, gpio_pin))
    {
        ESP_LOGE(TAG, "Configure external coex with unexpected gpio pin!!!");
        return ESP_ERR_INVALID_ARG;
    }
    esp_coex_external_set_wire_type(wire_type);
#if SOC_EXTERNAL_COEX_ADVANCE
    esp_coex_external_params(g_external_coex_params, 0, 0);
#endif

    if(EXTERNAL_COEX_LEADER_ROLE == g_external_coex_params.work_mode) {
        switch (wire_type)
        {
#if SOC_EXTERNAL_COEX_LEADER_TX_LINE
            case EXTERN_COEX_WIRE_4:
            {
                esp_coex_external_set_txline(true);
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.tx_line], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.tx_line, GPIO_MODE_OUTPUT);
                REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.tx_line));
                esp_rom_gpio_connect_out_signal(gpio_pin.tx_line, EXTERNAL_COEX_SIGNAL_O1_TXLINE_IDX, false, false);
            }
            __attribute__((fallthrough));
#endif
            case EXTERN_COEX_WIRE_3:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.priority], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.priority, GPIO_MODE_INPUT);
                esp_rom_gpio_connect_in_signal(gpio_pin.priority, EXTERNAL_COEX_SIGNAL_I1_IDX, false);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.priority), GPIO_PIN1_SYNC1_BYPASS, 2);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.priority), GPIO_PIN1_SYNC2_BYPASS, 2);
            }
            __attribute__((fallthrough));
            case EXTERN_COEX_WIRE_2:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.grant], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.grant, GPIO_MODE_OUTPUT);
                REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.grant));
                esp_rom_gpio_connect_out_signal(gpio_pin.grant, EXTERNAL_COEX_SIGNAL_O0_IDX, false, false);
            }
            __attribute__((fallthrough));
            case EXTERN_COEX_WIRE_1:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.request], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.request, GPIO_MODE_INPUT);
                esp_rom_gpio_connect_in_signal(gpio_pin.request, EXTERNAL_COEX_SIGNAL_I0_IDX, false);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.request), GPIO_PIN1_SYNC1_BYPASS, 2);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.request), GPIO_PIN1_SYNC2_BYPASS, 2);
                break;
            }
            default:
            {
                return ESP_FAIL;
            }
        }
    } else if(EXTERNAL_COEX_FOLLOWER_ROLE == g_external_coex_params.work_mode) {
#if SOC_EXTERNAL_COEX_ADVANCE
        switch (wire_type)
        {
            case EXTERN_COEX_WIRE_4:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.tx_line], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.tx_line, GPIO_MODE_INPUT);
                esp_rom_gpio_connect_in_signal(gpio_pin.tx_line, EXTERNAL_COEX_SIGNAL_I1_IDX, false);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.tx_line), GPIO_PIN1_SYNC1_BYPASS, 2);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.tx_line), GPIO_PIN1_SYNC2_BYPASS, 2);
            }
            __attribute__((fallthrough));
            case EXTERN_COEX_WIRE_3:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.priority], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.priority, GPIO_MODE_OUTPUT);
                REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.priority));
                esp_rom_gpio_connect_out_signal(gpio_pin.priority, EXTERNAL_COEX_SIGNAL_O1_IDX, false, false);
            }
            __attribute__((fallthrough));
            case EXTERN_COEX_WIRE_2:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.grant], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.grant, GPIO_MODE_INPUT);
                esp_rom_gpio_connect_in_signal(gpio_pin.grant, EXTERNAL_COEX_SIGNAL_I0_IDX, false);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.grant), GPIO_PIN1_SYNC1_BYPASS, 2);
                REG_SET_FIELD(GPIO_PIN_REG(gpio_pin.grant), GPIO_PIN1_SYNC2_BYPASS, 2);
            }
            __attribute__((fallthrough));
            case EXTERN_COEX_WIRE_1:
            {
                gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[gpio_pin.request], PIN_FUNC_GPIO);
                gpio_set_direction(gpio_pin.request, GPIO_MODE_OUTPUT);
                REG_WRITE(GPIO_ENABLE_W1TC_REG, BIT(gpio_pin.request));
                esp_rom_gpio_connect_out_signal(gpio_pin.request, EXTERNAL_COEX_SIGNAL_O0_IDX, false, false);
                break;
            }
            default:
            {
                return ESP_FAIL;
            }
        }
#else
        return ESP_ERR_INVALID_ARG;
#endif /* SOC_EXTERNAL_COEX_ADVANCE */
    }
    esp_err_t ret = esp_coex_external_set(EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_MID, EXTERN_COEX_PTI_HIGH);
    if (ESP_OK != ret) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_disable_extern_coex_gpio_pin()
{
    esp_coex_external_stop();

    return ESP_OK;
}
#endif /* External Coex */

#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE && CONFIG_SOC_IEEE802154_SUPPORTED
esp_err_t esp_coex_wifi_i154_enable(void)
{
    // TODO: Add a scheme for wifi and 154 coex.
    // Remove this function if FCC-50 closes.
    coex_enable();
    coex_schm_status_bit_set(1, 1);
    return ESP_OK;
}
#endif
