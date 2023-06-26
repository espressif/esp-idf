/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "driver/gpio_filter.h"
#include "driver/dedic_gpio.h"
#include "soc/soc_caps.h"

#if SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER

TEST_CASE("GPIO pin glitch filter life cycle", "[gpio_filter]")
{
    gpio_glitch_filter_handle_t filter = NULL;
    gpio_pin_glitch_filter_config_t config = {
        .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
    };
    TEST_ESP_OK(gpio_new_pin_glitch_filter(&config, &filter));

    TEST_ESP_OK(gpio_glitch_filter_enable(filter));
    // can't delete filter if it's not disabled
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gpio_del_glitch_filter(filter));

    TEST_ESP_OK(gpio_glitch_filter_disable(filter));

    TEST_ESP_OK(gpio_del_glitch_filter(filter));
}

#endif // SOC_GPIO_SUPPORT_PIN_GLITCH_FILTER

#if SOC_GPIO_FLEX_GLITCH_FILTER_NUM > 0

TEST_CASE("GPIO flex glitch filter life cycle", "[gpio_filter]")
{
    gpio_glitch_filter_handle_t filters[SOC_GPIO_FLEX_GLITCH_FILTER_NUM];
    gpio_flex_glitch_filter_config_t config = {
        .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
    };

    // install filter with wrong parameters
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, gpio_new_flex_glitch_filter(&config, &filters[0]));

    config.window_thres_ns = 75;
    config.window_width_ns = 100;
    for (int i = 0; i < SOC_GPIO_FLEX_GLITCH_FILTER_NUM; i++) {
        TEST_ESP_OK((gpio_new_flex_glitch_filter(&config, &filters[i])));
    }
    // no more hardware resource
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, gpio_new_flex_glitch_filter(&config, &filters[0]));

    TEST_ESP_OK(gpio_glitch_filter_enable(filters[0]));
    // can't delete filter if it's not disabled
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, gpio_del_glitch_filter(filters[0]));
    TEST_ESP_OK(gpio_glitch_filter_disable(filters[0]));

    for (int i = 0; i < SOC_GPIO_FLEX_GLITCH_FILTER_NUM; i++) {
        TEST_ESP_OK(gpio_del_glitch_filter(filters[i]));
    }
}

/**
 * @brief In order to generate the short glitch pulse, we use CPU CSR register to control the GPIO level,
 *        which is also called the Fast GPIO or Dedicated GPIO.
 * @note Because the CPU instruction / CSR register is not compatible in all ESP chips,
 *       at the moment, this test only works for Espressif's RISC-V core (e.g. ESP32C6)
 */
#if SOC_DEDICATED_GPIO_SUPPORTED

#include "hal/dedic_gpio_cpu_ll.h"

static void test_gpio_intr_callback(void *args)
{
    SemaphoreHandle_t sem = (SemaphoreHandle_t)args;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("event fired\r\n");
    xSemaphoreGiveFromISR(sem, &high_task_wakeup);
    if (high_task_wakeup) {
        esp_rom_printf("high priority task wake up\r\n");
    }
}

// put the simulation code in the IRAM to avoid cache miss
NOINLINE_ATTR IRAM_ATTR static void test_gpio_simulate_glitch_pulse(void)
{
    static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
    // the following code is used to generate a short glitch pulse
    // around 20ns @CPU160MHz, 40ns @CPU96MHz
    // pull high for 4 CPU cycles, to ensure the short pulse can be sampled by GPIO
    // we don't want any preemption to happen during the glitch signal generation
    portENTER_CRITICAL(&s_lock);
    asm volatile(
        "csrrsi zero, %0, 0x1\n"
        "csrrsi zero, %0, 0x1\n"
        "csrrsi zero, %0, 0x1\n"
        "csrrsi zero, %0, 0x1\n"
        "csrrci zero, %0, 0x1"
        :: "i"(CSR_GPIO_OUT_USER)
    );
    portEXIT_CRITICAL(&s_lock);
}

TEST_CASE("GPIO flex glitch filter enable/disable", "[gpio_filter]")
{
    const gpio_num_t test_gpio = 2;

    printf("initialize GPIO for input and out\r\n");
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = BIT64(test_gpio),
        .intr_type = GPIO_INTR_POSEDGE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));

    printf("install fast gpio to generate the glitch signal\r\n");
    dedic_gpio_bundle_handle_t bundle = NULL;
    dedic_gpio_bundle_config_t bundle_cfg = {
        .gpio_array = (int[]){test_gpio},
        .array_size = 1,
        .flags.out_en = true,
    };
    TEST_ESP_OK(dedic_gpio_new_bundle(&bundle_cfg, &bundle));
    // initial output value to zero
    asm volatile("csrrci zero, %0, 0x1" :: "i"(CSR_GPIO_OUT_USER));

    printf("apply glitch filter to the GPIO\r\n");
    gpio_glitch_filter_handle_t filter;
    gpio_flex_glitch_filter_config_t filter_cfg = {
        .clk_src = GLITCH_FILTER_CLK_SRC_DEFAULT,
        .gpio_num = test_gpio,
        .window_thres_ns = 500,
        .window_width_ns = 500,
    };
    TEST_ESP_OK((gpio_new_flex_glitch_filter(&filter_cfg, &filter)));

    printf("install gpio interrupt\r\n");
    gpio_install_isr_service(0);
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    TEST_ESP_OK(gpio_isr_handler_add(test_gpio, test_gpio_intr_callback, sem));

    printf("enable the glitch filter\r\n");
    TEST_ESP_OK(gpio_glitch_filter_enable(filter));

    printf("generate rising edge glitch signal\r\n");
    for (int i = 0; i < 10; i++) {
        test_gpio_simulate_glitch_pulse();
    }

    // should timeout, because the glitch is filtered out
    TEST_ASSERT_EQUAL(pdFALSE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));

    printf("disable the glitch filter\r\n");
    TEST_ESP_OK(gpio_glitch_filter_disable(filter));

    printf("generate rising edge glitch signal again\r\n");
    test_gpio_simulate_glitch_pulse();

    // this time we should see the GPIO interrupt fired up
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(1000)));

    TEST_ESP_OK(gpio_isr_handler_remove(test_gpio));
    gpio_uninstall_isr_service();

    printf("disable gpio glitch filter\r\n");
    TEST_ESP_OK(gpio_del_glitch_filter(filter));
    TEST_ESP_OK(dedic_gpio_del_bundle(bundle));
    vSemaphoreDelete(sem);
}

#endif // SOC_DEDICATED_GPIO_SUPPORTED
#endif // SOC_GPIO_FLEX_GLITCH_FILTER_NUM > 0
