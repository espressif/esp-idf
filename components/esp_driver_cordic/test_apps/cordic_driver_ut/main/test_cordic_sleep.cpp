/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/cordic.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

/* q1.15 residual at 12 iterations is 2^-11. */
#define CORDIC_SLEEP_TOLERANCE  0.01f

static void cordic_check_cos0(cordic_engine_handle_t engine)
{
    cordic_calculate_config_t calc_config = {
        .function = ESP_CORDIC_FUNC_COS,
        .iq_format = ESP_CORDIC_FORMAT_Q15,
        .iteration_count = 12,
        .scale_exp = 0,
    };
    uint32_t arg = 0;
    uint32_t res_cos = 0;
    uint32_t res_sin = 0;
    cordic_input_buffer_desc_t input_buffer = {
        .p_data_arg1 = &arg,
        .p_data_arg2 = NULL,
    };
    cordic_output_buffer_desc_t output_buffer = {
        .p_data_res1 = &res_cos,
        .p_data_res2 = &res_sin,
    };

    TEST_ESP_OK(cordic_calculate_polling(engine, &calc_config, &input_buffer, &output_buffer, 1));

    float hw_cos = cordic_convert_fixed_to_float(res_cos, ESP_CORDIC_FORMAT_Q15);
    float hw_sin = cordic_convert_fixed_to_float(res_sin, ESP_CORDIC_FORMAT_Q15);
    printf("cos(0)=%f sin(0)=%f\n", hw_cos, hw_sin);
    TEST_ASSERT_FLOAT_WITHIN(CORDIC_SLEEP_TOLERANCE, 1.0f, hw_cos);
    TEST_ASSERT_FLOAT_WITHIN(CORDIC_SLEEP_TOLERANCE, 0.0f, hw_sin);
}

TEST_CASE("cordic works after top domain light sleep", "[cordic]")
{
    cordic_engine_handle_t engine = NULL;
    cordic_engine_config_t engine_config = {
        .clock_source = CORDIC_CLK_SRC_DEFAULT,
        .instance_id = 0,
    };
    TEST_ESP_OK(cordic_acquire_engine(&engine_config, &engine));

    cordic_check_cos0(engine);

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    printf("go to light sleep for 1 second\r\n");
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
    TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    esp_sleep_set_sleep_context(NULL);

    cordic_check_cos0(engine);
    TEST_ESP_OK(cordic_release_engine(engine));
}
