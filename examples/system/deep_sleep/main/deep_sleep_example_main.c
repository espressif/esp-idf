/* Deep sleep wake up example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "soc/rtc.h"

#if SOC_TOUCH_SENSOR_SUPPORTED
#include "soc/sens_periph.h"
#include "driver/touch_pad.h"
#endif

#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
#define DEFAULT_WAKEUP_PIN      CONFIG_EXAMPLE_GPIO_WAKEUP_PIN
#ifdef CONFIG_EXAMPLE_GPIO_WAKEUP_HIGH_LEVEL
#define DEFAULT_WAKEUP_LEVEL    ESP_GPIO_WAKEUP_GPIO_HIGH
#else
#define DEFAULT_WAKEUP_LEVEL    ESP_GPIO_WAKEUP_GPIO_LOW
#endif
#endif

static RTC_DATA_ATTR struct timeval sleep_enter_time;

#ifdef CONFIG_EXAMPLE_TOUCH_WAKEUP
#if CONFIG_IDF_TARGET_ESP32
#define TOUCH_THRESH_NO_USE 0
static void calibrate_touch_pad(touch_pad_t pad);
#endif
#endif

void app_main(void)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    int sleep_time_ms = (now.tv_sec - sleep_enter_time.tv_sec) * 1000 + (now.tv_usec - sleep_enter_time.tv_usec) / 1000;

    switch (esp_sleep_get_wakeup_cause()) {
#if CONFIG_EXAMPLE_EXT0_WAKEUP
        case ESP_SLEEP_WAKEUP_EXT0: {
            printf("Wake up from ext0\n");
            break;
        }
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP
#ifdef CONFIG_EXAMPLE_EXT1_WAKEUP
        case ESP_SLEEP_WAKEUP_EXT1: {
            uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                printf("Wake up from GPIO %d\n", pin);
            } else {
                printf("Wake up from GPIO\n");
            }
            break;
        }
#endif // CONFIG_EXAMPLE_EXT1_WAKEUP
#if SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
        case ESP_SLEEP_WAKEUP_GPIO: {
            uint64_t wakeup_pin_mask = esp_sleep_get_gpio_wakeup_status();
            if (wakeup_pin_mask != 0) {
                int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
                printf("Wake up from GPIO %d\n", pin);
            } else {
                printf("Wake up from GPIO\n");
            }
            break;
        }
#endif //SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
        case ESP_SLEEP_WAKEUP_TIMER: {
            printf("Wake up from timer. Time spent in deep sleep: %dms\n", sleep_time_ms);
            break;
        }
#ifdef CONFIG_EXAMPLE_TOUCH_WAKEUP
        case ESP_SLEEP_WAKEUP_TOUCHPAD: {
            printf("Wake up from touch on pad %d\n", esp_sleep_get_touchpad_wakeup_status());
            break;
        }
#endif // CONFIG_EXAMPLE_TOUCH_WAKEUP

        case ESP_SLEEP_WAKEUP_UNDEFINED:
        default:
            printf("Not a deep sleep reset\n");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    const int wakeup_time_sec = 20;
    printf("Enabling timer wakeup, %ds\n", wakeup_time_sec);
    esp_sleep_enable_timer_wakeup(wakeup_time_sec * 1000000);

#if CONFIG_EXAMPLE_EXT0_WAKEUP
    const int ext_wakeup_pin_0 = 3;

    printf("Enabling EXT0 wakeup on pin GPIO%d\n", ext_wakeup_pin_0);
    esp_sleep_enable_ext0_wakeup(ext_wakeup_pin_0, 1);

    // Configure pullup/downs via RTCIO to tie wakeup pins to inactive level during deepsleep.
    // EXT0 resides in the same power domain (RTC_PERIPH) as the RTC IO pullup/downs.
    // No need to keep that power domain explicitly, unlike EXT1.
    rtc_gpio_pullup_dis(ext_wakeup_pin_0);
    rtc_gpio_pulldown_en(ext_wakeup_pin_0);
#endif // CONFIG_EXAMPLE_EXT0_WAKEUP
#ifdef CONFIG_EXAMPLE_EXT1_WAKEUP
    const int ext_wakeup_pin_1 = 2;
    const uint64_t ext_wakeup_pin_1_mask = 1ULL << ext_wakeup_pin_1;
    const int ext_wakeup_pin_2 = 4;
    const uint64_t ext_wakeup_pin_2_mask = 1ULL << ext_wakeup_pin_2;

    printf("Enabling EXT1 wakeup on pins GPIO%d, GPIO%d\n", ext_wakeup_pin_1, ext_wakeup_pin_2);
    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_1_mask | ext_wakeup_pin_2_mask, ESP_EXT1_WAKEUP_ANY_HIGH);

    /* If there are no external pull-up/downs, tie wakeup pins to inactive level with internal pull-up/downs via RTC IO
     * during deepsleep. However, RTC IO relies on the RTC_PERIPH power domain. Keeping this power domain on will
     * increase some power comsumption. */
#  if CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    rtc_gpio_pullup_dis(ext_wakeup_pin_1);
    rtc_gpio_pulldown_en(ext_wakeup_pin_1);
    rtc_gpio_pullup_dis(ext_wakeup_pin_2);
    rtc_gpio_pulldown_en(ext_wakeup_pin_2);
#  endif //CONFIG_EXAMPLE_EXT1_USE_INTERNAL_PULLUPS
#endif // CONFIG_EXAMPLE_EXT1_WAKEUP

#ifdef CONFIG_EXAMPLE_GPIO_WAKEUP
    const gpio_config_t config = {
        .pin_bit_mask = BIT(DEFAULT_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(BIT(DEFAULT_WAKEUP_PIN), DEFAULT_WAKEUP_LEVEL));
    printf("Enabling GPIO wakeup on pins GPIO%d\n", DEFAULT_WAKEUP_PIN);
#endif

#ifdef CONFIG_EXAMPLE_TOUCH_WAKEUP
#if CONFIG_IDF_TARGET_ESP32
    // Initialize touch pad peripheral.
    // The default fsm mode is software trigger mode.
    ESP_ERROR_CHECK(touch_pad_init());
    // If use touch pad wake up, should set touch sensor FSM mode at 'TOUCH_FSM_MODE_TIMER'.
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    // In this case, the high reference valtage will be 2.4V - 1V = 1.4V
    // The low reference voltage will be 0.5
    // The larger the range, the larger the pulse count value.
    touch_pad_set_voltage(TOUCH_HVOLT_2V4, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    //init RTC IO and mode for touch pad.
    touch_pad_config(TOUCH_PAD_NUM8, TOUCH_THRESH_NO_USE);
    touch_pad_config(TOUCH_PAD_NUM9, TOUCH_THRESH_NO_USE);
    calibrate_touch_pad(TOUCH_PAD_NUM8);
    calibrate_touch_pad(TOUCH_PAD_NUM9);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    /* Initialize touch pad peripheral. */
    touch_pad_init();
    /* Only support one touch channel in sleep mode. */
    touch_pad_config(TOUCH_PAD_NUM9);
    /* Denoise setting at TouchSensor 0. */
    touch_pad_denoise_t denoise = {
        /* The bits to be cancelled are determined according to the noise level. */
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    printf("Denoise function init\n");
    /* Filter setting */
    touch_filter_config_t filter_info = {
        .mode = TOUCH_PAD_FILTER_IIR_16,
        .debounce_cnt = 1,      // 1 time count.
        .noise_thr = 0,         // 50%
        .jitter_step = 4,       // use for jitter mode.
        .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
    };
    touch_pad_filter_set_config(&filter_info);
    touch_pad_filter_enable();
    printf("touch pad filter init %d\n", TOUCH_PAD_FILTER_IIR_8);
    /* Set sleep touch pad. */
    touch_pad_sleep_channel_enable(TOUCH_PAD_NUM9, true);
    touch_pad_sleep_channel_enable_proximity(TOUCH_PAD_NUM9, false);
    /* Reducing the operating frequency can effectively reduce power consumption. */
    touch_pad_sleep_channel_set_work_time(1000, TOUCH_PAD_MEASURE_CYCLE_DEFAULT);
    /* Enable touch sensor clock. Work mode is "timer trigger". */
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    touch_pad_fsm_start();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    /* set touchpad wakeup threshold */
    uint32_t touch_value, wake_threshold;
    touch_pad_sleep_channel_read_smooth(TOUCH_PAD_NUM9, &touch_value);
    wake_threshold = touch_value * 0.1; // wakeup when touch sensor crosses 10% of background level
    touch_pad_sleep_set_threshold(TOUCH_PAD_NUM9, wake_threshold);
    printf("Touch pad #%d average: %d, wakeup threshold set to %d\n",
        TOUCH_PAD_NUM9, touch_value, (uint32_t)(touch_value * 0.1));
#endif
    printf("Enabling touch pad wakeup\n");
    esp_sleep_enable_touchpad_wakeup();
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif // CONFIG_EXAMPLE_TOUCH_WAKEUP

#if CONFIG_IDF_TARGET_ESP32
    // Isolate GPIO12 pin from external circuits. This is needed for modules
    // which have an external pull-up resistor on GPIO12 (such as ESP32-WROVER)
    // to minimize current consumption.
    rtc_gpio_isolate(GPIO_NUM_12);
#endif

    printf("Entering deep sleep\n");
    gettimeofday(&sleep_enter_time, NULL);

    esp_deep_sleep_start();
}

#ifdef CONFIG_EXAMPLE_TOUCH_WAKEUP
#if CONFIG_IDF_TARGET_ESP32
static void calibrate_touch_pad(touch_pad_t pad)
{
    int avg = 0;
    const size_t calibration_count = 128;
    for (int i = 0; i < calibration_count; ++i) {
        uint16_t val;
        touch_pad_read(pad, &val);
        avg += val;
    }
    avg /= calibration_count;
    const int min_reading = 300;
    if (avg < min_reading) {
        printf("Touch pad #%d average reading is too low: %d (expecting at least %d). "
               "Not using for deep sleep wakeup.\n", pad, avg, min_reading);
        touch_pad_config(pad, 0);
    } else {
        int threshold = avg - 100;
        printf("Touch pad #%d average: %d, wakeup threshold set to %d.\n", pad, avg, threshold);
        touch_pad_config(pad, threshold);
    }
}
#endif
#endif // CONFIG_EXAMPLE_TOUCH_WAKEUP
