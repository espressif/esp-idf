/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_private/periph_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"
#include "driver/lp_io.h"
#include "hal/rtc_io_hal.h"
#include "soc/rtc_io_periph.h"
#include "soc/soc_caps.h"

#if SOC_LP_IO_CLOCK_IS_INDEPENDENT && !SOC_RTCIO_RCC_IS_INDEPENDENT
// For `rtcio_hal_function_select` using, clock reg option is inlined in it,
// so remove the declaration check of __DECLARE_RCC_RC_ATOMIC_ENV
#define RTCIO_RCC_ATOMIC()                              \
    for (int i = 1; i ? (periph_rcc_enter(), 1) : 0;    \
         periph_rcc_exit(), i--)
#else
#define RTCIO_RCC_ATOMIC()
#endif

static const char __attribute__((__unused__)) *RTCIO_TAG = "RTCIO";

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define RTCIO_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define RTCIO_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

bool rtc_gpio_is_valid_gpio(gpio_num_t gpio_num)
{
#if SOC_RTCIO_PIN_COUNT > 0
    return (gpio_num < GPIO_PIN_COUNT && rtc_io_num_map[gpio_num] >= 0);
#else
    return false;
#endif
}

#if SOC_RTCIO_PIN_COUNT > 0
/*---------------------------------------------------------------
                        RTC IO
---------------------------------------------------------------*/
int rtc_io_number_get(gpio_num_t gpio_num)
{
    return rtc_io_num_map[gpio_num];
}

esp_err_t rtc_gpio_init(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    RTCIO_RCC_ATOMIC() {
        rtcio_hal_function_select(rtc_io_number_get(gpio_num), RTCIO_LL_FUNC_RTC);
    }
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    RTCIO_RCC_ATOMIC() {
        // Select Gpio as Digital Gpio
        rtcio_hal_function_select(rtc_io_number_get(gpio_num), RTCIO_LL_FUNC_DIGITAL);
    }
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_level(rtc_io_number_get(gpio_num), level);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

uint32_t rtc_gpio_get_level(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    return rtcio_hal_get_level(rtc_io_number_get(gpio_num));
}

esp_err_t rtc_gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "Output pad only");
    ESP_RETURN_ON_FALSE(strength < GPIO_DRIVE_CAP_MAX, ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO drive capability error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_drive_capability(rtc_io_number_get(gpio_num), strength);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "Output pad only");
    ESP_RETURN_ON_FALSE(strength != NULL, ESP_ERR_INVALID_ARG, RTCIO_TAG, "GPIO drive pointer error");
    *strength = (gpio_drive_cap_t)rtcio_hal_get_drive_capability(rtc_io_number_get(gpio_num));

    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_direction(rtc_io_number_get(gpio_num), mode);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction_in_sleep(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_direction_in_sleep(rtc_io_number_get(gpio_num), mode);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pullup_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pullup_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pulldown_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pulldown_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_iomux_func_sel(gpio_num_t gpio_num, int func)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_iomux_func_sel(rtc_io_number_get(gpio_num), func);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

#if SOC_LP_GPIO_MATRIX_SUPPORTED
esp_err_t lp_gpio_connect_in_signal(gpio_num_t gpio_num, uint32_t signal_idx, bool inv)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "LP_IO number error");
    rtcio_hal_matrix_in(rtc_io_number_get(gpio_num), signal_idx, inv);
    return ESP_OK;
}

esp_err_t lp_gpio_connect_out_signal(gpio_num_t gpio_num, uint32_t signal_idx, bool out_inv, bool out_en_inv)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "LP_IO number error");
    rtcio_hal_matrix_out(rtc_io_number_get(gpio_num), signal_idx, out_inv, out_en_inv);
    return ESP_OK;
}
#endif // SOC_LP_GPIO_MATRIX_SUPPORTED

#endif // SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_RTCIO_HOLD_SUPPORTED

esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_hold_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_hold_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rtc_gpio_force_hold_en_all(void)
{
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_hold_all();
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_force_hold_dis_all(void)
{
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_unhold_all();
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}
#endif // SOC_RTCIO_HOLD_SUPPORTED

#if SOC_RTCIO_HOLD_SUPPORTED && SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
esp_err_t rtc_gpio_isolate(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_isolate(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}
#endif // SOC_RTCIO_HOLD_SUPPORTED && SOC_RTCIO_INPUT_OUTPUT_SUPPORTED

#if SOC_RTCIO_WAKE_SUPPORTED

esp_err_t rtc_gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    if (intr_type == GPIO_INTR_POSEDGE || intr_type == GPIO_INTR_NEGEDGE || intr_type == GPIO_INTR_ANYEDGE) {
        return ESP_ERR_INVALID_ARG; // Dont support this mode.
    }
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_wakeup_enable(rtc_io_number_get(gpio_num), intr_type);
    RTCIO_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t rtc_gpio_wakeup_disable(gpio_num_t gpio_num)
{
    ESP_RETURN_ON_FALSE(rtc_gpio_is_valid_gpio(gpio_num), ESP_ERR_INVALID_ARG, RTCIO_TAG, "RTCIO number error");
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_wakeup_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();
    return ESP_OK;
}

#endif // SOC_RTCIO_WAKE_SUPPORTED

#endif // SOC_RTCIO_PIN_COUNT > 0
