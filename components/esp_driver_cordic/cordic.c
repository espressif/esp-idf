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
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "cordic_private.h"
#include "driver/cordic_types.h"
#include "driver/cordic.h"
#include "hal/cordic_hal.h"
#include "hal/cordic_ll.h"
#include "soc/cordic_reg.h"
#include "soc/soc.h"
#include "hal/cordic_periph.h"

ESP_LOG_ATTR_TAG(TAG, "cordic");

// Per result; generous margin over the maximum 15 CORDIC iterations.
#define CORDIC_ONESHOT_CALCULATE_MAX_RETRIES (1024)

typedef struct cordic_platform_t {
    _lock_t mutex;                         // protects the reference count and engine lifecycle
    portMUX_TYPE spinlock;                 // serializes CORDIC register access from tasks and ISRs
    cordic_engine_handle_t engine_handle;  // the live engine, NULL while the hardware is down
    uint32_t ref_count;                    // references handed out by cordic_acquire_engine
} cordic_platform_t;

static cordic_platform_t s_cordic_platform = {
    .spinlock = portMUX_INITIALIZER_UNLOCKED,
};

static bool cordic_engine_occupied(void)
{
    return s_cordic_platform.engine_handle != NULL;
}

// Allocates the engine and brings the hardware up. Does not touch the reference count.
// Caller must hold s_cordic_platform.mutex;
static esp_err_t cordic_internal_new_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine)
{
    cordic_clock_source_t clk_src = cordic_cfg->clock_source ? cordic_cfg->clock_source : CORDIC_CLK_SRC_DEFAULT;

    cordic_engine_handle_t engine = (cordic_engine_handle_t)heap_caps_calloc(1, sizeof(cordic_engine_t), CORDIC_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(engine, ESP_ERR_NO_MEM, TAG, "no memory for cordic engine");

    cordic_hal_init(&engine->hal);
    cordic_ll_enable_bus_clock(true);
    cordic_ll_enable_clock(true);
    cordic_ll_reset_module();
    cordic_ll_set_clock_source(clk_src);
    cordic_ll_set_clock_div(1, 0, 0);

    s_cordic_platform.engine_handle = engine;
    *ret_engine = engine;
    return ESP_OK;
}

// Tears the hardware down and frees the engine. Does not touch the reference count.
// Caller must hold s_cordic_platform.mutex;
static void cordic_internal_del_engine(cordic_engine_handle_t engine)
{
    cordic_ll_enable_clock(false);
    cordic_ll_enable_bus_clock(false);
    cordic_hal_deinit(&engine->hal);
    s_cordic_platform.engine_handle = NULL;
    heap_caps_free(engine);
}

// Deprecated exclusive create. Pairs with cordic_delete_engine(). Mutually exclusive with
// acquire/release: a second create of the same hardware instance is rejected.
esp_err_t cordic_new_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine)
{
    ESP_RETURN_ON_FALSE(cordic_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ret_engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cordic_cfg->instance_id < CORDIC_LL_INST_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid instance_id");

    esp_err_t ret = ESP_OK;
    _lock_acquire(&s_cordic_platform.mutex);
    ESP_GOTO_ON_FALSE(!cordic_engine_occupied(), ESP_ERR_NOT_FOUND, unlock, TAG,
                      "engine already created for this instance");
    ret = cordic_internal_new_engine(cordic_cfg, ret_engine);

unlock:
    _lock_release(&s_cordic_platform.mutex);
    return ret;
}

esp_err_t cordic_acquire_engine(const cordic_engine_config_t *cordic_cfg, cordic_engine_handle_t *ret_engine)
{
    ESP_RETURN_ON_FALSE(cordic_cfg, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ret_engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(cordic_cfg->instance_id < CORDIC_LL_INST_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid instance_id");

    esp_err_t ret = ESP_OK;

    _lock_acquire(&s_cordic_platform.mutex);
    // The first acquirer allocates and initializes. Later acquirers just take another reference to the same engine
    if (s_cordic_platform.ref_count == 0) {
        ESP_GOTO_ON_FALSE(!cordic_engine_occupied(), ESP_ERR_INVALID_STATE, unlock, TAG,
                          "engine was created by cordic_new_engine");
        ESP_GOTO_ON_ERROR(cordic_internal_new_engine(cordic_cfg, ret_engine), unlock, TAG, "failed to create engine");
    } else {
        *ret_engine = s_cordic_platform.engine_handle;
    }
    s_cordic_platform.ref_count++;

unlock:
    _lock_release(&s_cordic_platform.mutex);
    return ret;
}

esp_err_t cordic_calculate_polling(cordic_engine_handle_t engine, const cordic_calculate_config_t *calc_cfg, cordic_input_buffer_desc_t *input_buffer_desc, cordic_output_buffer_desc_t *output_buffer_desc, size_t buffer_depth)
{
    // No error logs: this path can run from an ISR, and the return code is enough.
    if (!(engine && calc_cfg && input_buffer_desc && input_buffer_desc->p_data_arg1 &&
            output_buffer_desc && output_buffer_desc->p_data_res1 && buffer_depth > 0)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (calc_cfg->function >= ESP_CORDIC_FUNC_MAX ||
            calc_cfg->iq_format >= ESP_CORDIC_IQ_SIZE_MAX ||
            calc_cfg->scale_exp < cordic_hal_algorithm_allowable_scale[calc_cfg->function][0] ||
            calc_cfg->scale_exp > cordic_hal_algorithm_allowable_scale[calc_cfg->function][1] ||
            calc_cfg->iteration_count == 0 ||
            calc_cfg->iteration_count > CORDIC_LL_PRECISION_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    bool is_two_args = (calc_cfg->function == ESP_CORDIC_FUNC_PHASE || calc_cfg->function == ESP_CORDIC_FUNC_MODULUS);
    if (is_two_args != (input_buffer_desc->p_data_arg2 != NULL)) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t arg_num = is_two_args ? 1 : 0;
    cordic_set_argument_func_t set_argument_func = cordic_hal_set_argument_funcs[calc_cfg->iq_format][arg_num];
    cordic_get_result_func_t get_result_func = cordic_hal_get_result_funcs[calc_cfg->iq_format];

    esp_err_t ret = ESP_OK;
    // Short critical section: typical callers pass a few points, so masking interrupts for the
    // batch is acceptable and callers do not have to handle a busy return.
    portENTER_CRITICAL_SAFE(&s_cordic_platform.spinlock);

    // polling mode doesn't use DMA, so set the mode to register mode.
    // All csr_cfg fields are programmed in one RMW for performance's sake.
    cordic_ll_set_calculate_config(engine->hal.dev,
                                   calc_cfg->function,
                                   CORDIC_LL_MODE_REG,
                                   calc_cfg->iteration_count - 1,
                                   calc_cfg->scale_exp,
                                   is_two_args ? 2 : 1,
                                   2,
                                   calc_cfg->iq_format,
                                   calc_cfg->iq_format);

    for (size_t i = 0; i < buffer_depth; i++) {
        set_argument_func(&engine->hal, input_buffer_desc, i);
        cordic_ll_start_calculate(engine->hal.dev);

        uint32_t retries = 0;
        while (cordic_ll_is_calculate_result_ready(engine->hal.dev) == 0) {
            if (++retries >= CORDIC_ONESHOT_CALCULATE_MAX_RETRIES) {
                cordic_ll_reset_module();
                ret = ESP_ERR_TIMEOUT;
                goto exit;
            }
        }
        get_result_func(&engine->hal, output_buffer_desc, i);
    }

exit:
    portEXIT_CRITICAL_SAFE(&s_cordic_platform.spinlock);
    return ret;
}

esp_err_t cordic_release_engine(cordic_engine_handle_t engine)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    _lock_acquire(&s_cordic_platform.mutex);
    // Covers a stale handle, a double release, and a handle that does not belong to this instance
    ESP_GOTO_ON_FALSE(s_cordic_platform.engine_handle == engine, ESP_ERR_INVALID_STATE, unlock, TAG, "engine was not acquired");
    ESP_GOTO_ON_FALSE(s_cordic_platform.ref_count > 0, ESP_ERR_INVALID_STATE, unlock, TAG, "no reference is held");

    if (--s_cordic_platform.ref_count == 0) {
        cordic_internal_del_engine(engine);
    }

unlock:
    _lock_release(&s_cordic_platform.mutex);
    return ret;
}

// Deprecated exclusive destroy. Pairs with cordic_new_engine(). Mutually exclusive with
// acquire/release; refuses if the engine is currently held via acquire.
esp_err_t cordic_delete_engine(cordic_engine_handle_t engine)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(engine, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    _lock_acquire(&s_cordic_platform.mutex);
    ESP_GOTO_ON_FALSE(s_cordic_platform.engine_handle == engine, ESP_ERR_INVALID_STATE, unlock, TAG, "engine was not created");
    ESP_GOTO_ON_FALSE(s_cordic_platform.ref_count == 0, ESP_ERR_INVALID_STATE, unlock, TAG, "engine is still acquired");
    cordic_internal_del_engine(engine);

unlock:
    _lock_release(&s_cordic_platform.mutex);
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
