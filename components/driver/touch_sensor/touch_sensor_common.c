/*
 * SPDX-FileCopyrightText: 2016-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/soc_pins.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "driver/touch_pad.h"
#include "esp_private/rtc_ctrl.h"
#include "driver/gpio.h"
#include "hal/touch_sensor_types.h"
#include "hal/touch_sensor_hal.h"

static const char *TOUCH_TAG = "TOUCH_SENSOR";
#define TOUCH_CHECK(a, str, ret_val) ({                                             \
    if (!(a)) {                                                                     \
        ESP_LOGE(TOUCH_TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str);              \
        return (ret_val);                                                           \
    }                                                                               \
})
#ifdef CONFIG_IDF_TARGET_ESP32
#define TOUCH_CHANNEL_CHECK(channel) do { \
        TOUCH_CHECK(channel < SOC_TOUCH_SENSOR_NUM && channel >= 0, "Touch channel error", ESP_ERR_INVALID_ARG); \
    } while (0);
#else // !CONFIG_IDF_TARGET_ESP32
#define TOUCH_CHANNEL_CHECK(channel) do { \
        TOUCH_CHECK(channel < SOC_TOUCH_SENSOR_NUM && channel >= 0, "Touch channel error", ESP_ERR_INVALID_ARG); \
        TOUCH_CHECK(channel != SOC_TOUCH_DENOISE_CHANNEL, "TOUCH0 is internal denoise channel", ESP_ERR_INVALID_ARG); \
    } while (0);
#endif // CONFIG_IDF_TARGET_ESP32

#define TOUCH_GET_IO_NUM(channel) (touch_sensor_channel_io_map[channel])

_Static_assert(TOUCH_PAD_MAX == SOC_TOUCH_SENSOR_NUM, "Touch sensor channel number not equal to chip capabilities");

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define TOUCH_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define TOUCH_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

esp_err_t touch_pad_isr_deregister(intr_handler_t fn, void *arg)
{
    return rtc_isr_deregister(fn, arg);
}

esp_err_t touch_pad_set_voltage(touch_high_volt_t refh, touch_low_volt_t refl, touch_volt_atten_t atten)
{
    TOUCH_CHECK(((refh < TOUCH_HVOLT_MAX) && (refh >= (int)TOUCH_HVOLT_KEEP)), "touch refh error",
                ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(((refl < TOUCH_LVOLT_MAX) && (refh >= (int)TOUCH_LVOLT_KEEP)), "touch refl error",
                ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(((atten < TOUCH_HVOLT_ATTEN_MAX) && (refh >= (int)TOUCH_HVOLT_ATTEN_KEEP)), "touch atten error",
                ESP_ERR_INVALID_ARG);

    const touch_hal_volt_t volt = {
        .refh = refh,
        .refl = refl,
        .atten = atten,
    };
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_voltage(&volt);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_voltage(touch_high_volt_t *refh, touch_low_volt_t *refl, touch_volt_atten_t *atten)
{
    touch_hal_volt_t volt = {0};
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_voltage(&volt);
    TOUCH_EXIT_CRITICAL();
    *refh = volt.refh;
    *refl = volt.refl;
    *atten = volt.atten;

    return ESP_OK;
}

esp_err_t touch_pad_set_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t slope, touch_tie_opt_t opt)
{
    TOUCH_CHECK(touch_num < SOC_TOUCH_SENSOR_NUM, "Touch channel error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(slope < TOUCH_PAD_SLOPE_MAX, "touch slope error", ESP_ERR_INVALID_ARG);
    TOUCH_CHECK(opt < TOUCH_PAD_TIE_OPT_MAX, "touch opt error", ESP_ERR_INVALID_ARG);

    const touch_hal_meas_mode_t meas = {
        .slope = slope,
        .tie_opt = opt,
    };
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_meas_mode(touch_num, &meas);
    TOUCH_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t touch_pad_get_cnt_mode(touch_pad_t touch_num, touch_cnt_slope_t *slope, touch_tie_opt_t *opt)
{
    TOUCH_CHECK(touch_num < SOC_TOUCH_SENSOR_NUM, "Touch channel error", ESP_ERR_INVALID_ARG);

    touch_hal_meas_mode_t meas = {0};
    TOUCH_ENTER_CRITICAL();
    touch_hal_get_meas_mode(touch_num, &meas);
    TOUCH_EXIT_CRITICAL();
    *slope = meas.slope;
    *opt = meas.tie_opt;

    return ESP_OK;
}

esp_err_t touch_pad_io_init(touch_pad_t touch_num)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    gpio_num_t gpio_num = TOUCH_GET_IO_NUM(touch_num);
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    return ESP_OK;
}

esp_err_t touch_pad_fsm_start(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_start_fsm();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_fsm_stop(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_stop_fsm();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

esp_err_t touch_pad_set_fsm_mode(touch_fsm_mode_t mode)
{
    TOUCH_CHECK((mode < TOUCH_FSM_MODE_MAX), "touch fsm mode error", ESP_ERR_INVALID_ARG);

    TOUCH_ENTER_CRITICAL();
    touch_hal_set_fsm_mode(mode);
    TOUCH_EXIT_CRITICAL();
#ifdef CONFIG_IDF_TARGET_ESP32
    if (mode == TOUCH_FSM_MODE_TIMER) {
        touch_pad_fsm_start();
    } else {
        touch_pad_fsm_stop();
    }
#endif
    return ESP_OK;
}

esp_err_t touch_pad_get_fsm_mode(touch_fsm_mode_t *mode)
{
    touch_hal_get_fsm_mode(mode);
    return ESP_OK;
}

esp_err_t touch_pad_sw_start(void)
{
    TOUCH_ENTER_CRITICAL();
    touch_hal_start_sw_meas();
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}

#ifdef CONFIG_IDF_TARGET_ESP32
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint16_t threshold)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_threshold(touch_num, threshold);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}
#else // !CONFIG_IDF_TARGET_ESP32
esp_err_t touch_pad_set_thresh(touch_pad_t touch_num, uint32_t threshold)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_CHECK(touch_num != SOC_TOUCH_DENOISE_CHANNEL,
                "TOUCH0 is internal denoise channel", ESP_ERR_INVALID_ARG);
    TOUCH_ENTER_CRITICAL();
    touch_hal_set_threshold(touch_num, threshold);
    TOUCH_EXIT_CRITICAL();
    return ESP_OK;
}
#endif // CONFIG_IDF_TARGET_ESP32

#ifdef CONFIG_IDF_TARGET_ESP32
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint16_t *threshold)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    touch_hal_get_threshold(touch_num, threshold);
    return ESP_OK;
}
#else // !CONFIG_IDF_TARGET_ESP32
esp_err_t touch_pad_get_thresh(touch_pad_t touch_num, uint32_t *threshold)
{
    TOUCH_CHANNEL_CHECK(touch_num);
    TOUCH_CHECK(touch_num != SOC_TOUCH_DENOISE_CHANNEL,
                "TOUCH0 is internal denoise channel", ESP_ERR_INVALID_ARG);
    touch_hal_get_threshold(touch_num, threshold);
    return ESP_OK;
}
#endif // CONFIG_IDF_TARGET_ESP32

esp_err_t touch_pad_get_wakeup_status(touch_pad_t *pad_num)
{
    touch_hal_get_wakeup_status(pad_num);
    TOUCH_CHANNEL_CHECK(*pad_num);
    return ESP_OK;
}

uint32_t IRAM_ATTR touch_pad_get_status(void)
{
    uint32_t status = 0;
    touch_hal_read_trigger_status_mask(&status);
    return status;
}

esp_err_t IRAM_ATTR touch_pad_clear_status(void)
{
    touch_hal_clear_trigger_status_mask();
    return ESP_OK;
}
