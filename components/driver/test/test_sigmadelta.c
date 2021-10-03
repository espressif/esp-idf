/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "soc/soc_caps.h"
#include "driver/sigmadelta.h"

TEST_CASE("SigmaDelta config test", "[sigma_delta]")
{
    sigmadelta_config_t sigmadelta_cfg = {
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 45,
        .sigmadelta_gpio = 4,
    };
    for (int i = 0; i < SOC_SIGMADELTA_CHANNEL_NUM; i++) {
        sigmadelta_cfg.channel = i;
        TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));
    }

    sigmadelta_cfg.channel = SOC_SIGMADELTA_CHANNEL_NUM;
    TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_INVALID_ARG, sigmadelta_config(&sigmadelta_cfg), "wrong channel number should be inspected");
}

// connect GPIO4 with LED positive pin, and the GND pin connect LED negative pin
// logic analyzer help also to see the wave form(more standard and accurate)
TEST_CASE("SigmaDelta pin, duty, prescale set", "[sigma_delta][ignore]")
{
    sigmadelta_config_t sigmadelta_cfg = {
        .channel = 0,
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 0,
        .sigmadelta_gpio = 4,
    };
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    int8_t duty = 0;
    int inc = 1;
    for (int i = 0; i < 1000; i++) {
        sigmadelta_set_duty(sigmadelta_cfg.channel, duty);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        }
    }

    TEST_ESP_OK(sigmadelta_set_prescale(0, 200));
    for (int i = 0; i < 1000; i++) {
        sigmadelta_set_duty(sigmadelta_cfg.channel, duty);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        }
    }

    TEST_ESP_OK(sigmadelta_set_pin(sigmadelta_cfg.channel, 5));
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}
