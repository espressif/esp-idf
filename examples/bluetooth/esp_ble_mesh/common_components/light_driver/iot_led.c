/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "soc/ledc_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/ledc_struct.h"
#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "esp_attr.h"
#include "iot_led.h"
#include "esp_log.h"

#define LEDC_FADE_MARGIN (10)
#define LEDC_VALUE_TO_DUTY(value) (value * ((1 << LEDC_TIMER_13_BIT) - 1) / UINT16_MAX)
#define LEDC_DUTY_TO_VALUE(value) (value * UINT16_MAX / ((1 << LEDC_TIMER_13_BIT) - 1) )
#define LEDC_FIXED_Q (8)
#define FLOATINT_2_FIXED(X, Q) ((int)((X)*(0x1U << Q)))
#define FIXED_2_FLOATING(X, Q) ((int)((X)/(0x1U << Q)))
#define GET_FIXED_INTEGER_PART(X, Q) (X >> Q)
#define GET_FIXED_DECIMAL_PART(X, Q) (X & ((0x1U << Q) - 1))

#define GPTIMER_RESOLUTION_HZ 1000000 // 1MHz, 1 tick=1us

typedef struct {
    int cur;
    int final;
    int step;
    int cycle;
    size_t num;
} ledc_fade_data_t;

typedef struct {
    ledc_fade_data_t fade_data[LEDC_CHANNEL_MAX];
    ledc_mode_t speed_mode;
    ledc_timer_t timer_num;
    gptimer_handle_t gptimer;
} iot_light_t;

static const char *TAG = "iot_light";
static DRAM_ATTR iot_light_t *g_light_config = NULL;
static DRAM_ATTR uint16_t *g_gamma_table = NULL;
static DRAM_ATTR bool g_hw_timer_started = false;

static bool fade_timercb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

static void iot_timer_start(gptimer_handle_t gptimer)
{
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = DUTY_SET_CYCLE / 1000 * GPTIMER_RESOLUTION_HZ,
        .flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_start(gptimer);
    g_hw_timer_started = true;
}

static IRAM_ATTR void iot_timer_stop(gptimer_handle_t gptimer)
{
    gptimer_stop(gptimer);
    g_hw_timer_started = false;
}

static IRAM_ATTR esp_err_t iot_ledc_duty_config(ledc_mode_t speed_mode, ledc_channel_t channel, int hpoint_val, int duty_val,
        uint32_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    if (hpoint_val >= 0) {
#if CONFIG_IDF_TARGET_ESP32C3
        LEDC.channel_group[speed_mode].channel[channel].hpoint.hpoint = hpoint_val & LEDC_HPOINT_LSCH1_V;
#elif CONFIG_IDF_TARGET_ESP32
        LEDC.channel_group[speed_mode].channel[channel].hpoint.hpoint = hpoint_val & LEDC_HPOINT_HSCH1_V;
#endif
    }

    if (duty_val >= 0) {
        LEDC.channel_group[speed_mode].channel[channel].duty.duty = duty_val;
    }

#if CONFIG_IDF_TARGET_ESP32C3
    LEDC.channel_group[speed_mode].channel[channel].conf1.val = ((duty_direction & LEDC_DUTY_INC_LSCH0_V) << LEDC_DUTY_INC_LSCH0_S) |
            ((duty_num & LEDC_DUTY_NUM_LSCH0_V) << LEDC_DUTY_NUM_LSCH0_S) |
            ((duty_cycle & LEDC_DUTY_CYCLE_LSCH0_V) << LEDC_DUTY_CYCLE_LSCH0_S) |
            ((duty_scale & LEDC_DUTY_SCALE_LSCH0_V) << LEDC_DUTY_SCALE_LSCH0_S);
#elif CONFIG_IDF_TARGET_ESP32
    LEDC.channel_group[speed_mode].channel[channel].conf1.val = ((duty_direction & LEDC_DUTY_INC_HSCH0_V) << LEDC_DUTY_INC_HSCH0_S) |
            ((duty_num & LEDC_DUTY_NUM_HSCH0_V) << LEDC_DUTY_NUM_HSCH0_S) |
            ((duty_cycle & LEDC_DUTY_CYCLE_HSCH0_V) << LEDC_DUTY_CYCLE_HSCH0_S) |
            ((duty_scale & LEDC_DUTY_SCALE_HSCH0_V) << LEDC_DUTY_SCALE_HSCH0_S);
#endif

    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 1;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;

    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        LEDC.channel_group[speed_mode].channel[channel].conf0.low_speed_update = 1;
    }

    return ESP_OK;
}

static IRAM_ATTR esp_err_t _iot_set_fade_with_step(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int scale, int cycle_num)
{
    uint32_t duty_cur = LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> 4;
    int step_num = 0;
    int dir = LEDC_DUTY_DIR_DECREASE;

    if (scale > 0) {
        if (duty_cur > target_duty) {
            step_num = (duty_cur - target_duty) / scale;
            step_num = step_num > 1023 ? 1023 : step_num;
            scale = (step_num == 1023) ? (duty_cur - target_duty) / step_num : scale;
        } else {
            dir = LEDC_DUTY_DIR_INCREASE;
            step_num = (target_duty - duty_cur) / scale;
            step_num = step_num > 1023 ? 1023 : step_num;
            scale = (step_num == 1023) ? (target_duty - duty_cur) / step_num : scale;
        }
    }

    if (scale > 0 && step_num > 0) {
        iot_ledc_duty_config(speed_mode, channel, -1, duty_cur << 4, dir, step_num, cycle_num, scale);
    } else {
        iot_ledc_duty_config(speed_mode, channel, -1, target_duty << 4, dir, 0, 1, 0);
    }

    return ESP_OK;
}

static IRAM_ATTR esp_err_t _iot_set_fade_with_time(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t target_duty, int max_fade_time_ms)
{
    uint32_t freq = 0;
    uint32_t duty_cur = LEDC.channel_group[speed_mode].channel[channel].duty_rd.duty_read >> 4;
    uint32_t duty_delta = target_duty > duty_cur ? target_duty - duty_cur : duty_cur - target_duty;

    uint32_t timer_source_clk = LEDC.timer_group[speed_mode].timer[g_light_config->timer_num].conf.tick_sel;
    uint32_t duty_resolution = LEDC.timer_group[speed_mode].timer[g_light_config->timer_num].conf.duty_resolution;
    uint32_t clock_divider = LEDC.timer_group[speed_mode].timer[g_light_config->timer_num].conf.clock_divider;
    uint32_t precision = (0x1U << duty_resolution);

    if (timer_source_clk == LEDC_APB_CLK) {
        freq = ((uint64_t)APB_CLK_FREQ << 8) / precision / clock_divider;
    } else {
        freq = ((uint64_t)REF_CLK_FREQ << 8) / precision / clock_divider;
    }

    if (duty_delta == 0) {
        return _iot_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }

    int total_cycles = max_fade_time_ms * freq / 1000;

    if (total_cycles == 0) {
        return _iot_set_fade_with_step(speed_mode, channel, target_duty, 0, 0);
    }

    int scale, cycle_num;

    if (total_cycles > duty_delta) {
        scale = 1;
        cycle_num = total_cycles / duty_delta;

#if CONFIG_IDF_TARGET_ESP32C3
        if (cycle_num > LEDC_DUTY_NUM_LSCH0_V) {
            cycle_num = LEDC_DUTY_NUM_LSCH0_V;
        }
#elif CONFIG_IDF_TARGET_ESP32
        if (cycle_num > LEDC_DUTY_NUM_HSCH0_V) {
            cycle_num = LEDC_DUTY_NUM_HSCH0_V;
        }
#endif
    } else {
        cycle_num = 1;
        scale = duty_delta / total_cycles;

#if CONFIG_IDF_TARGET_ESP32C3
        if (scale > LEDC_DUTY_SCALE_LSCH0_V) {
            scale = LEDC_DUTY_SCALE_LSCH0_V;
        }
#elif CONFIG_IDF_TARGET_ESP32
        if (scale > LEDC_DUTY_SCALE_HSCH0_V) {
            scale = LEDC_DUTY_SCALE_HSCH0_V;
        }
#endif
    }

    return _iot_set_fade_with_step(speed_mode, channel, target_duty, scale, cycle_num);
}

static IRAM_ATTR esp_err_t _iot_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 1;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;

    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        LEDC.channel_group[speed_mode].channel[channel].conf0.low_speed_update = 1;
    }

    return ESP_OK;
}

static IRAM_ATTR esp_err_t iot_ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    return iot_ledc_duty_config(speed_mode,
                                channel,         // uint32_t chan_num,
                                -1,
                                duty << 4,       // uint32_t duty_val,the least 4 bits are decimal part
                                1,               // uint32_t increase,
                                1,               // uint32_t duty_num,
                                1,               // uint32_t duty_cycle,
                                0                // uint32_t duty_scale
                               );
}

static void gamma_table_create(uint16_t *gamma_table, float correction)
{
    float value_tmp = 0;

    /**
     * @brief gamma curve formula: y=a*x^(1/gm)
     * x ∈ (0,(GAMMA_TABLE_SIZE-1)/GAMMA_TABLE_SIZE)
     * a = GAMMA_TABLE_SIZE
     */
    for (int i = 0; i < GAMMA_TABLE_SIZE; i++) {
        value_tmp = (float)(i) / GAMMA_TABLE_SIZE;
        value_tmp = powf(value_tmp, 1.0f / correction);
        gamma_table[i] = (uint16_t)FLOATINT_2_FIXED((value_tmp * GAMMA_TABLE_SIZE), LEDC_FIXED_Q);
    }
}

static IRAM_ATTR uint32_t gamma_value_to_duty(int value)
{
    uint32_t tmp_q = GET_FIXED_INTEGER_PART(value, LEDC_FIXED_Q);
    uint32_t tmp_r = GET_FIXED_DECIMAL_PART(value, LEDC_FIXED_Q);

    uint16_t cur = LEDC_VALUE_TO_DUTY(g_gamma_table[tmp_q]);
    uint16_t next = LEDC_VALUE_TO_DUTY(g_gamma_table[tmp_q + 1]);
    return (cur + (next - cur) * tmp_r / (0x1U << LEDC_FIXED_Q));
}

static IRAM_ATTR bool fade_timercb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    int idle_channel_num = 0;

    for (int channel = 0; channel < LEDC_CHANNEL_MAX; channel++) {
        ledc_fade_data_t *fade_data = g_light_config->fade_data + channel;

        if (fade_data->num > 0) {
            fade_data->num--;

            if (fade_data->step) {
                fade_data->cur += fade_data->step;

                if (fade_data->num != 0) {
                    _iot_set_fade_with_time(g_light_config->speed_mode, channel,
                                            gamma_value_to_duty(fade_data->cur),
                                            DUTY_SET_CYCLE - LEDC_FADE_MARGIN);
                } else {
                    iot_ledc_set_duty(g_light_config->speed_mode, channel, gamma_value_to_duty(fade_data->cur));
                }

                _iot_update_duty(g_light_config->speed_mode, channel);
            } else {
                iot_ledc_set_duty(g_light_config->speed_mode, channel, gamma_value_to_duty(fade_data->cur));
                _iot_update_duty(g_light_config->speed_mode, channel);
            }
        } else if (fade_data->cycle) {
            fade_data->num = fade_data->cycle - 1;

            if (fade_data->step) {
                fade_data->step *= -1;
                fade_data->cur  += fade_data->step;
            } else {
                fade_data->cur = (fade_data->cur == fade_data->final) ? 0 : fade_data->final;
            }

            _iot_set_fade_with_time(g_light_config->speed_mode, channel,
                                    gamma_value_to_duty(fade_data->cur),
                                    DUTY_SET_CYCLE - LEDC_FADE_MARGIN);
            _iot_update_duty(g_light_config->speed_mode, channel);

        } else {
            idle_channel_num++;
        }
    }

    if (idle_channel_num >= LEDC_CHANNEL_MAX) {
        iot_timer_stop(timer);
    }

    return false;
}

esp_err_t iot_led_init(ledc_timer_t timer_num, ledc_mode_t speed_mode, uint32_t freq_hz)
{
    esp_err_t ret = ESP_OK;
    const ledc_timer_config_t ledc_time_config = {
        .speed_mode = speed_mode,
        .timer_num  = timer_num,
        .freq_hz    = freq_hz,
        .duty_resolution = LEDC_TIMER_13_BIT,
    };

    ret = ledc_timer_config(&ledc_time_config);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "LEDC timer configuration");
        return ret;
    }

    if (g_gamma_table == NULL) {
        g_gamma_table = calloc(GAMMA_TABLE_SIZE, sizeof(uint16_t));
        gamma_table_create(g_gamma_table, GAMMA_CORRECTION);
    } else {
        ESP_LOGE(TAG, "gamma_table has been initialized");
    }

    if (g_light_config == NULL) {
        g_light_config = calloc(1, sizeof(iot_light_t));
        g_light_config->timer_num  = timer_num;
        g_light_config->speed_mode = speed_mode;

        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = GPTIMER_RESOLUTION_HZ,
        };
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &g_light_config->gptimer));
        gptimer_event_callbacks_t cbs = {
            .on_alarm = fade_timercb,
        };
        ESP_ERROR_CHECK(gptimer_register_event_callbacks(g_light_config->gptimer, &cbs, NULL));
        ESP_ERROR_CHECK(gptimer_enable(g_light_config->gptimer));
    } else {
        ESP_LOGE(TAG, "g_light_config has been initialized");
    }

    return ESP_OK;
}

esp_err_t iot_led_deinit(void)
{
    if (g_gamma_table) {
        free(g_gamma_table);
    }

    if (g_light_config) {
        gptimer_disable(g_light_config->gptimer);
        gptimer_del_timer(g_light_config->gptimer);
        free(g_light_config);
    }

    return ESP_OK;
}

esp_err_t iot_led_regist_channel(ledc_channel_t channel, gpio_num_t gpio_num)
{
    esp_err_t ret = ESP_OK;
    if (g_light_config == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first");
        return ESP_ERR_NOT_FOUND;
    }
#ifdef CONFIG_SPIRAM_SUPPORT
    if (gpio_num != GPIO_NUM_16 || gpio_num != GPIO_NUM_17) {
        ESP_LOGW(TAG, "gpio_num must not conflict to PSRAM(IO16 && IO17)");
        return ESP_ERR_INVALID_ARG;
    }
#endif
    const ledc_channel_config_t ledc_ch_config = {
        .gpio_num   = gpio_num,
        .channel    = channel,
        .intr_type  = LEDC_INTR_DISABLE,
        .speed_mode = g_light_config->speed_mode,
        .timer_sel  = g_light_config->timer_num,
    };

    ret = ledc_channel_config(&ledc_ch_config);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "LEDC channel configuration");
        return ret;
    }

    return ESP_OK;
}

esp_err_t iot_led_get_channel(ledc_channel_t channel, uint8_t *dst)
{
    if (g_light_config == NULL || dst == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first or dst should not be NULL");
        return ESP_ERR_INVALID_ARG;
    }
    int cur = g_light_config->fade_data[channel].cur;
    *dst = FIXED_2_FLOATING(cur, LEDC_FIXED_Q);
    return ESP_OK;
}

esp_err_t iot_led_set_channel(ledc_channel_t channel, uint8_t value, uint32_t fade_ms)
{
    if (g_light_config == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first");
        return ESP_ERR_INVALID_ARG;
    }
    ledc_fade_data_t *fade_data = g_light_config->fade_data + channel;

    fade_data->final = FLOATINT_2_FIXED(value, LEDC_FIXED_Q);

    if (fade_ms < DUTY_SET_CYCLE) {
        fade_data->num = 1;
    } else {
        fade_data->num   = fade_ms / DUTY_SET_CYCLE;
    }

    fade_data->step  = abs(fade_data->cur - fade_data->final) / fade_data->num;

    if (fade_data->cur > fade_data->final) {
        fade_data->step *= -1;
    }

    if (fade_data->cycle != 0) {
        fade_data->cycle = 0;
    }

    if (g_hw_timer_started != true) {
        iot_timer_start(g_light_config->gptimer);
    }

    return ESP_OK;
}

esp_err_t iot_led_start_blink(ledc_channel_t channel, uint8_t value, uint32_t period_ms, bool fade_flag)
{
    if (g_light_config == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first");
        return ESP_ERR_INVALID_ARG;
    }
    ledc_fade_data_t *fade_data = g_light_config->fade_data + channel;

    fade_data->final = fade_data->cur = FLOATINT_2_FIXED(value, LEDC_FIXED_Q);
    fade_data->cycle = period_ms / 2 / DUTY_SET_CYCLE;
    fade_data->num = (fade_flag) ? period_ms / 2 / DUTY_SET_CYCLE : 0;
    fade_data->step  = (fade_flag) ? fade_data->cur / fade_data->num * -1 : 0;

    if (g_hw_timer_started != true) {
        iot_timer_start(g_light_config->gptimer);
    }

    return ESP_OK;

}

esp_err_t iot_led_stop_blink(ledc_channel_t channel)
{
    if (g_light_config == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first");
        return ESP_ERR_INVALID_ARG;
    }
    ledc_fade_data_t *fade_data = g_light_config->fade_data + channel;
    fade_data->cycle = fade_data->num = 0;

    return ESP_OK;
}

esp_err_t iot_led_set_gamma_table(const uint16_t gamma_table[GAMMA_TABLE_SIZE])
{
    if (g_gamma_table == NULL) {
        ESP_LOGW(TAG, "iot_led_init() must be called first");
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(g_gamma_table, gamma_table, GAMMA_TABLE_SIZE * sizeof(uint16_t));
    return ESP_OK;
}
