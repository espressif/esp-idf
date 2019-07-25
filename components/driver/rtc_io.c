// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"
#include "hal/rtc_io_hal.h"

static const char *RTCIO_TAG = "RTCIO";

#define RTCIO_CHECK(a, str, ret_val) ({                                             \
    if (!(a)) {                                                                     \
        ESP_LOGE(RTCIO_TAG,"%s:%d (%s):%s", __FILE__, __LINE__, __FUNCTION__, str); \
        return (ret_val);                                                           \
    }                                                                               \
})

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define RTCIO_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define RTCIO_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/*---------------------------------------------------------------
                        RTC IO
---------------------------------------------------------------*/
esp_err_t rtc_gpio_init(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_function_select(rtc_io_number_get(gpio_num), RTCIO_FUNC_RTC);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_deinit(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    // Select Gpio as Digital Gpio
    rtcio_hal_function_select(rtc_io_number_get(gpio_num), RTCIO_FUNC_DIGITAL);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_set_level(gpio_num_t gpio_num, uint32_t level)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_level(rtc_io_number_get(gpio_num), level);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

uint32_t rtc_gpio_get_level(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    return rtcio_hal_get_level(rtc_io_number_get(gpio_num));
}

esp_err_t rtc_gpio_set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTCIO_CHECK(strength < GPIO_DRIVE_CAP_MAX, "RTCIO drive capability error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_drive_capability(rtc_io_number_get(gpio_num), strength);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_get_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t *strength)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(gpio_num), "Output pad only", ESP_ERR_INVALID_ARG);
    RTCIO_CHECK(strength != NULL, "GPIO drive pointer error", ESP_ERR_INVALID_ARG);
    *strength = (gpio_drive_cap_t)rtcio_hal_get_drive_capability(rtc_io_number_get(gpio_num));

    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_direction(rtc_io_number_get(gpio_num), mode);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_set_direction_in_sleep(gpio_num_t gpio_num, rtc_gpio_mode_t mode)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_set_direction_in_sleep(rtc_io_number_get(gpio_num), mode);
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_en(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pullup_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pullup_dis(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pullup_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_en(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pulldown_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_pulldown_dis(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_pulldown_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_hold_en(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_hold_enable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_hold_dis(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_hold_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t rtc_gpio_isolate(gpio_num_t gpio_num)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_isolate(rtc_io_number_get(gpio_num));
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

esp_err_t rtc_gpio_wakeup_enable(gpio_num_t gpio_num, gpio_int_type_t intr_type)
{
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
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
    RTCIO_CHECK(rtc_gpio_is_valid_gpio(gpio_num), "RTCIO number error", ESP_ERR_INVALID_ARG);
    RTCIO_ENTER_CRITICAL();
    rtcio_hal_wakeup_disable(rtc_io_number_get(gpio_num));
    RTCIO_EXIT_CRITICAL();

    return ESP_OK;
}
