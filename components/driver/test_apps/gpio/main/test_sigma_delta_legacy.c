/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "driver/sigmadelta.h"

TEST_CASE("SigmaDelta_config_test", "[sigma_delta]")
{
    sigmadelta_config_t sigmadelta_cfg = {
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 45,
        .sigmadelta_gpio = 4,
    };
    for (int i = 0; i < SIGMADELTA_CHANNEL_MAX; i++) {
        sigmadelta_cfg.channel = i;
        TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));
    }

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_MAX;
    TEST_ASSERT_EQUAL_MESSAGE(ESP_ERR_INVALID_ARG, sigmadelta_config(&sigmadelta_cfg), "wrong channel number should be inspected");
}

// connect GPIO4 with LED positive pin, and the GND pin connect LED negative pin
// logic analyzer help also to see the wave form(more standard and accurate)
TEST_CASE("SigmaDelta_pin_duty_prescale_set", "[sigma_delta][ignore]")
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
