/*
 * Sigamadelta Test:
 * 1. basic configuration test
 * 2. pin, duty, prescale test
 */
#include "unity.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sigmadelta.h"

TEST_CASE("SigmaDelta config test", "[sigma_delta]")
{
    sigmadelta_config_t sigmadelta_cfg = {
        .channel = SIGMADELTA_CHANNEL_0,
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 45,
        .sigmadelta_gpio = 4,
    };
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_1;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_2;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_3;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_4;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_5;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_6;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_7;
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

     //ERROR PARAM
    sigmadelta_cfg.channel = SIGMADELTA_CHANNEL_MAX;
    TEST_ASSERT(sigmadelta_config(&sigmadelta_cfg) == ESP_ERR_INVALID_ARG);

}

// connect GPIO4 with LED positive pin, and the GND pin connect LED negative pin
// logic analyzer help also to see the wave form(more standard and accurate)
// can't test it in CI, ignore
TEST_CASE("SigmaDelta pin, duty, prescale set", "[sigma_delta][ignore]")
{
    sigmadelta_config_t sigmadelta_cfg = {
        .channel = SIGMADELTA_CHANNEL_0,
        .sigmadelta_prescale = 80,
        .sigmadelta_duty = 0,
        .sigmadelta_gpio = 4,
    };
    TEST_ESP_OK(sigmadelta_config(&sigmadelta_cfg));

    int8_t duty = 0;
    int inc = 1;
    for(int i=0; i<1000; i++) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, duty);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        }
    }

    TEST_ESP_OK(sigmadelta_set_prescale(SIGMADELTA_CHANNEL_0, 200));
    for(int i=0; i<1000; i++) {
        sigmadelta_set_duty(SIGMADELTA_CHANNEL_0, duty);
        vTaskDelay(10 / portTICK_PERIOD_MS);

        duty += inc;
        if (duty == 127 || duty == -127) {
            inc = (-1) * inc;
        }
    }

    TEST_ESP_OK(sigmadelta_set_pin(SIGMADELTA_CHANNEL_0, 5));
    vTaskDelay(3000 / portTICK_PERIOD_MS);
}
