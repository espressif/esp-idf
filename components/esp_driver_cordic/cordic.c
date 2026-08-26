/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "cordic_private.h"
#include "driver/cordic_types.h"
#include "driver/cordic.h"
#include "hal/cordic_hal.h"
#include "hal/cordic_ll.h"
#include "soc/cordic_reg.h"
#include "soc/soc.h"
#include "hal/cordic_periph.h"

ESP_LOG_ATTR_TAG(TAG, "cordic");

typedef struct cordic_platform_t {
    _lock_t mutex;                         // platform level mutex lock.
    cordic_engine_handle_t engine_handle;  // array of cordic instances.
} cordic_platform_t;

static cordic_platform_t s_cordic_platform = {}; // singleton platform

static bool cordic_engine_occupied(void)
{
    return s_cordic_platform.engine_handle != NULL;
}

esp_err_t cordic_new_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine)
{
    esp_err_t ret = ESP_OK;
    cordic_engine_handle_t engine = NULL;
    ESP_RETURN_ON_FALSE(cordic_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ret_engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    engine = (cordic_engine_handle_t)heap_caps_calloc(1, sizeof(cordic_engine_t), CORDIC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(engine, ESP_ERR_NO_MEM, TAG, "no memory for cordic engine");

    bool engine_found = false;
    _lock_acquire(&s_cordic_platform.mutex);
    if (cordic_engine_occupied() == false) {
        s_cordic_platform.engine_handle = engine;
        engine_found = true;
    }
    _lock_release(&s_cordic_platform.mutex);
    ESP_GOTO_ON_FALSE(engine_found, ESP_ERR_NOT_FOUND, err, TAG, "no free controller");

    cordic_hal_init(&engine->hal);
    cordic_ll_enable_bus_clock(true);
    cordic_ll_reset_module_register();
    cordic_ll_enable_clock(true);

    cordic_clock_source_t clk_src = cordic_cfg->clock_source ? cordic_cfg->clock_source : CORDIC_CLK_SRC_DEFAULT;
    cordic_ll_set_clock_source(clk_src);
    *ret_engine = engine;

    return ESP_OK;

err:
    cordic_delete_engine(engine);
    return ret;
}

esp_err_t cordic_calculate_polling(cordic_engine_handle_t engine, const cordic_calculate_config_t *calc_cfg, cordic_input_buffer_desc_t *input_buffer_desc, cordic_output_buffer_desc_t *output_buffer_desc, size_t buffer_depth)
{
    ESP_RETURN_ON_FALSE_ISR(engine && calc_cfg && input_buffer_desc && input_buffer_desc->p_data_arg1 && output_buffer_desc && output_buffer_desc->p_data_res1 && buffer_depth > 0, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(calc_cfg->function < ESP_CORDIC_FUNC_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid function");
    ESP_RETURN_ON_FALSE_ISR(calc_cfg->iq_format < ESP_CORDIC_IQ_SIZE_MAX, ESP_ERR_INVALID_ARG, TAG, "invalid iq_format");
    ESP_RETURN_ON_FALSE_ISR(calc_cfg->scale_exp >= cordic_hal_algorithm_allowable_scale[calc_cfg->function][0] && calc_cfg->scale_exp <= cordic_hal_algorithm_allowable_scale[calc_cfg->function][1], ESP_ERR_INVALID_ARG, TAG, "invalid scale_exp");
    ESP_RETURN_ON_FALSE_ISR(calc_cfg->iteration_count <= CORDIC_LL_PRECISION_MAX && calc_cfg->iteration_count > 0, ESP_ERR_INVALID_ARG, TAG, "invalid iteration_count");
    // Determine if function requires two arguments
    bool is_two_args = (calc_cfg->function == ESP_CORDIC_FUNC_PHASE || calc_cfg->function == ESP_CORDIC_FUNC_MODULUS);
    ESP_RETURN_ON_FALSE_ISR(is_two_args == (input_buffer_desc->p_data_arg2 != NULL), ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    BaseType_t in_isr = xPortInIsrContext();

    // Convert timeout from milliseconds to ticks
    uint32_t timeout_ms = (buffer_depth / 100 + 1) * CORDIC_ONESHOT_CALCULATE_TIMEOUT_MS;
    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    TickType_t start_tick = 0;
    if (in_isr) {
        start_tick = xTaskGetTickCountFromISR();
    } else {
        start_tick = xTaskGetTickCount();
    }

    // Configure hardware registers based on calculation config
    cordic_ll_set_calculate_function(engine->hal.dev, calc_cfg->function);
    cordic_ll_set_calculate_mode(engine->hal.dev, CORDIC_LL_MODE_REG);
    cordic_ll_set_calculate_precision(engine->hal.dev, calc_cfg->iteration_count - 1);
    cordic_ll_set_calculate_scale(engine->hal.dev, calc_cfg->scale_exp);
    cordic_ll_set_calculate_result_number(engine->hal.dev, 2);

    if (is_two_args) {
        cordic_ll_set_calculate_argument_number(engine->hal.dev, 2);
    } else {
        cordic_ll_set_calculate_argument_number(engine->hal.dev, 1);
    }
    cordic_ll_set_calculate_result_format(engine->hal.dev, calc_cfg->iq_format);
    cordic_ll_set_calculate_argument_format(engine->hal.dev, calc_cfg->iq_format);

    // Get function pointers from HAL layer arrays using format and argument count as indices
    size_t arg_num = is_two_args ? 1 : 0;
    cordic_set_argument_func_t set_argument_func = cordic_hal_set_argument_funcs[calc_cfg->iq_format][arg_num];
    cordic_get_result_func_t get_result_func = cordic_hal_get_result_funcs[calc_cfg->iq_format];

    for (size_t i = 0; i < buffer_depth; i++) {
        // Use function pointer to set arguments
        set_argument_func(&engine->hal, input_buffer_desc, i);

        cordic_ll_start_calculate(engine->hal.dev);

        // Wait for result ready with timeout
        while (cordic_ll_is_calculate_result_ready(engine->hal.dev) == 0) {
            TickType_t current_tick = 0;
            if (in_isr) {
                current_tick = xTaskGetTickCountFromISR();
            } else {
                current_tick = xTaskGetTickCount();
            }
            // TickType_t is unsigned, so subtraction handles overflow correctly
            TickType_t elapsed_ticks = current_tick - start_tick;
            if (elapsed_ticks >= timeout_ticks) {
                ESP_DRAM_LOGE(TAG, "CORDIC calculation timeout after %d ms", timeout_ms);
                cordic_ll_reset_module_register();
                return ESP_ERR_TIMEOUT;
            }
        }
        // Use function pointer to get results
        get_result_func(&engine->hal, output_buffer_desc, i);
    }

    return ESP_OK;
}

esp_err_t cordic_delete_engine(cordic_engine_handle_t engine)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    cordic_ll_enable_bus_clock(false);
    cordic_ll_enable_clock(false);
    cordic_hal_deinit(&engine->hal);

    _lock_acquire(&s_cordic_platform.mutex);
    s_cordic_platform.engine_handle = NULL;
    _lock_release(&s_cordic_platform.mutex);

    heap_caps_free(engine);

    return ret;
}

float cordic_convert_fixed_to_float(uint32_t fixed_value, cordic_iq_format_t iq_format)
{
    if (iq_format == ESP_CORDIC_FORMAT_Q15) {
        return (float)(int16_t)(fixed_value & 0xFFFF) / CORDIC_Q15_SCALE_FACTOR;
    } else if (iq_format == ESP_CORDIC_FORMAT_Q31) {
        return (float)(int32_t)fixed_value / CORDIC_Q31_SCALE_FACTOR;
    } else {
        return 0.0f;
    }
}

uint32_t cordic_convert_float_to_fixed(float float_value, cordic_iq_format_t iq_format)
{
    if (iq_format == ESP_CORDIC_FORMAT_Q15) {
        return (uint32_t)(int16_t)(float_value * CORDIC_Q15_SCALE_FACTOR) & 0xFFFF;
    } else if (iq_format == ESP_CORDIC_FORMAT_Q31) {
        return (uint32_t)(int32_t)(float_value * CORDIC_Q31_SCALE_FACTOR);
    } else {
        return 0;
    }
}
