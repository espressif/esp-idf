/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include "soc/ledc_reg.h"
#include "soc/timer_group_struct.h"
#include "soc/ledc_struct.h"
#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "iot_light.h"

static const char *TAG = "light";

#define IOT_CHECK(tag, a, ret)  if(!(a)) {       \
        return (ret);                            \
    }
#define ERR_ASSERT(tag, param, ret)  IOT_CHECK(tag, (param) == ESP_OK, ret)
#define POINT_ASSERT(tag, param)    IOT_CHECK(tag, (param) != NULL, ESP_FAIL)
#define LIGHT_NUM_MAX   4

#define GPTIMER_RESOLUTION_HZ 1000000 // 1MHz, 1 tick=1us

typedef enum {
    LIGHT_CH_NUM_1 = 1,             /*!< Light channel number */
    LIGHT_CH_NUM_2 = 2,             /*!< Light channel number */
    LIGHT_CH_NUM_3 = 3,             /*!< Light channel number */
    LIGHT_CH_NUM_4 = 4,             /*!< Light channel number */
    LIGHT_CH_NUM_5 = 5,             /*!< Light channel number */
    LIGHT_CH_NUM_MAX,               /*!< user shouldn't use this */
} light_channel_num_t;

typedef struct {
    gpio_num_t io_num;
    ledc_mode_t mode;
    ledc_channel_t channel;
    int breath_period;
    int blink_period;
    uint32_t fade_step_num;
    uint32_t *fade_duty_step;
    int fade_duty_counter;
    bool fade_step_up;
    bool fade_once;
    bool fade_start;
} light_channel_t;

typedef struct {
    uint8_t channel_num;
    ledc_mode_t mode;
    ledc_timer_t ledc_timer;
    uint32_t full_duty;
    uint32_t freq_hz;
    ledc_timer_bit_t timer_bit;
    gptimer_handle_t gptimer;
    light_channel_t *channel_group[];
} light_t;

static bool g_fade_installed                 = false;
static bool g_hw_timer_started               = false;
static light_t *g_light_group[LIGHT_NUM_MAX] = {NULL};
static esp_err_t iot_light_duty_set(light_handle_t light_handle, uint8_t channel_id, uint32_t duty);

static IRAM_ATTR void iot_ledc_ls_channel_update(ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    if (speed_mode == LEDC_LOW_SPEED_MODE) {
        LEDC.channel_group[speed_mode].channel[channel_num].conf0.low_speed_update = 1;
    }
}

static IRAM_ATTR esp_err_t iot_ledc_duty_config(ledc_mode_t speed_mode, ledc_channel_t channel_num, int hpoint_val, int duty_val,
        uint32_t duty_direction, uint32_t duty_num, uint32_t duty_cycle, uint32_t duty_scale)
{
    if (hpoint_val >= 0) {
        LEDC.channel_group[speed_mode].channel[channel_num].hpoint.hpoint = hpoint_val & LEDC_HPOINT_HSCH1_V;
    }

    if (duty_val >= 0) {
        LEDC.channel_group[speed_mode].channel[channel_num].duty.duty = duty_val;
    }

    LEDC.channel_group[speed_mode].channel[channel_num].conf1.val = ((duty_direction & LEDC_DUTY_INC_HSCH0_V) << LEDC_DUTY_INC_HSCH0_S) |
            ((duty_num & LEDC_DUTY_NUM_HSCH0_V) << LEDC_DUTY_NUM_HSCH0_S) |
            ((duty_cycle & LEDC_DUTY_CYCLE_HSCH0_V) << LEDC_DUTY_CYCLE_HSCH0_S) |
            ((duty_scale & LEDC_DUTY_SCALE_HSCH0_V) << LEDC_DUTY_SCALE_HSCH0_S);
    iot_ledc_ls_channel_update(speed_mode, channel_num);
    return ESP_OK;
}

static IRAM_ATTR esp_err_t iot_ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty)
{
    return iot_ledc_duty_config(speed_mode,
                                channel,         //uint32_t chan_num,
                                -1,
                                duty << 4,       //uint32_t duty_val,the least 4 bits are decimal part
                                1,               //uint32_t increase,
                                1,               //uint32_t duty_num,
                                1,               //uint32_t duty_cycle,
                                0                //uint32_t duty_scale
                               );
}

static IRAM_ATTR esp_err_t iot_ledc_update_duty(ledc_mode_t speed_mode, ledc_channel_t channel)
{
    LEDC.channel_group[speed_mode].channel[channel].conf0.sig_out_en = 1;
    LEDC.channel_group[speed_mode].channel[channel].conf1.duty_start = 1;
    iot_ledc_ls_channel_update(speed_mode, channel);
    return ESP_OK;
}

static IRAM_ATTR bool breath_timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    for (int i = 0; i < LIGHT_NUM_MAX; i++) {
        if (g_light_group[i] != NULL) {
            light_t *light = g_light_group[i];

            for (int j = 0; j < light->channel_num; j++) {
                light_channel_t *l_chn = light->channel_group[j];

                if (l_chn->fade_start == true) {
                    if (l_chn->fade_step_up == true) {
                        l_chn->fade_duty_counter++;
                    } else {
                        l_chn->fade_duty_counter--;
                    }

                    if ((l_chn->fade_duty_counter >= 0) && (l_chn->fade_duty_counter < l_chn->fade_step_num)) {
                        iot_ledc_set_duty(l_chn->mode, l_chn->channel, l_chn->fade_duty_step[l_chn->fade_duty_counter]);
                        iot_ledc_update_duty(l_chn->mode, l_chn->channel);
                    } else {
                        if (l_chn->fade_once != true) {
                            l_chn->fade_step_up = 1 - l_chn->fade_step_up;
                        } else {
                            l_chn->fade_start = false;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static light_channel_t *light_channel_create(gpio_num_t io_num, ledc_channel_t channel, ledc_mode_t mode, ledc_timer_t timer)
{
    ledc_channel_config_t ledc_channel = {
        .channel    = channel,
        .duty       = 0,
        .gpio_num   = io_num,
        .intr_type  = LEDC_INTR_FADE_END,
        .speed_mode = mode,
        .timer_sel  = timer
    };
    ERR_ASSERT(TAG, ledc_channel_config(&ledc_channel), NULL);
    light_channel_t *pwm = (light_channel_t *)calloc(1, sizeof(light_channel_t));
    pwm->io_num            = io_num;
    pwm->channel           = channel;
    pwm->mode              = mode;
    pwm->breath_period     = 0;
    pwm->blink_period      = 0;
    pwm->fade_step_num     = 0;
    pwm->fade_duty_step    = NULL;
    pwm->fade_duty_counter = 0;
    pwm->fade_step_up      = true;
    pwm->fade_once         = false;
    pwm->fade_start        = false;
    return pwm;
}

static esp_err_t light_channel_delete(light_channel_t *light_channel)
{
    POINT_ASSERT(TAG, light_channel);

    if (light_channel->fade_duty_step != NULL) {
        heap_caps_free(light_channel->fade_duty_step);
        light_channel->fade_duty_step = NULL;
    }

    free(light_channel);
    return ESP_OK;
}

static esp_err_t light_channel_fade_clear(light_channel_t *light_channel)
{
    light_channel->fade_start        = false;
    light_channel->fade_step_num     = 0;
    light_channel->fade_duty_counter = 0;
    light_channel->fade_step_up      = true;
    light_channel->fade_once         = false;

    if (light_channel->fade_duty_step != NULL) {
        heap_caps_free(light_channel->fade_duty_step);
        light_channel->fade_duty_step = NULL;
    }

    return ESP_OK;
}

light_handle_t iot_light_create(ledc_timer_t timer, ledc_mode_t speed_mode, uint32_t freq_hz, uint8_t channel_num, ledc_timer_bit_t timer_bit)
{
    IOT_CHECK(TAG, channel_num != 0, NULL);
    ledc_timer_config_t timer_conf = {
        .timer_num = timer,
        .speed_mode = speed_mode,
        .freq_hz = freq_hz,
        .bit_num = timer_bit
    };
    ERR_ASSERT(TAG, ledc_timer_config(&timer_conf), NULL);
    light_t *light_ptr = (light_t *)calloc(1, sizeof(light_t) + sizeof(light_channel_t *) * channel_num);
    light_ptr->channel_num          = channel_num;
    light_ptr->ledc_timer           = timer;
    light_ptr->full_duty            = (1 << timer_bit) - 1;
    light_ptr->freq_hz              = freq_hz;
    light_ptr->mode                 = speed_mode;
    light_ptr->timer_bit            = timer_bit;

    if (g_hw_timer_started == false) {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = GPTIMER_RESOLUTION_HZ,
        };
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &light_ptr->gptimer));
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = DUTY_SET_CYCLE / 1000 * GPTIMER_RESOLUTION_HZ,
            .reload_count = 0,
            .flags.auto_reload_on_alarm = true,
        };
        gptimer_event_callbacks_t cbs = {
            .on_alarm = breath_timer_callback,
        };
        gptimer_register_event_callbacks(light_ptr->gptimer, &cbs, NULL);
        gptimer_set_alarm_action(light_ptr->gptimer, &alarm_config);
        gptimer_enable(light_ptr->gptimer);
        gptimer_start(light_ptr->gptimer);
        g_hw_timer_started = true;
    }

    for (int i = 0; i < channel_num; i++) {
        light_ptr->channel_group[i] = NULL;
    }

    for (int i = 0; i < LIGHT_NUM_MAX; i++) {
        if (g_light_group[i] == NULL) {
            g_light_group[i] = light_ptr;
            break;
        }
    }

    return (light_handle_t)light_ptr;
}

esp_err_t iot_light_delete(light_handle_t light_handle)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);

    for (int i = 0; i < light->channel_num; i++) {
        if (light->channel_group[i] != NULL) {
            iot_light_duty_set(light, i, 0);
            light_channel_delete(light->channel_group[i]);
        }
    }

    for (int i = 0; i < LIGHT_NUM_MAX; i++) {
        if (g_light_group[i] == light) {
            g_light_group[i] = NULL;
            break;
        }
    }

    for (int i = 0; i < LIGHT_NUM_MAX; i++) {
        if (g_light_group[i] != NULL) {
            goto FREE_MEM;
        }
    }

    ledc_fade_func_uninstall();
    g_fade_installed = false;
    g_hw_timer_started = false;
    gptimer_stop(light->gptimer);
    gptimer_disable(light->gptimer);
    gptimer_del_timer(light->gptimer);
FREE_MEM:
    free(light_handle);
    return ESP_OK;
}

esp_err_t iot_light_channel_regist(light_handle_t light_handle, uint8_t channel_idx, gpio_num_t io_num, ledc_channel_t channel)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_idx < light->channel_num, FAIL);

    if (light->channel_group[channel_idx] != NULL) {
        ESP_LOGE(TAG, "this channel index has been registered");
        return ESP_FAIL;
    }

    light->channel_group[channel_idx] = light_channel_create(io_num, channel, light->mode, light->ledc_timer);

    if (g_fade_installed == false) {
        ledc_fade_func_install(0);
        g_fade_installed = true;
    }

    return ESP_OK;
}

static esp_err_t iot_light_duty_set(light_handle_t light_handle, uint8_t channel_id, uint32_t duty)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    POINT_ASSERT(TAG, light->channel_group[channel_id]);
    light_channel_t *l_chn = light->channel_group[channel_id];

    light_channel_fade_clear(l_chn);

    iot_ledc_set_duty(l_chn->mode, l_chn->channel, duty);
    iot_ledc_update_duty(l_chn->mode, l_chn->channel);
    return ESP_OK;
}

uint32_t iot_light_duty_get(light_handle_t light_handle, uint8_t channel_id)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    POINT_ASSERT(TAG, light->channel_group[channel_id]);
    light_channel_t *l_chn = light->channel_group[channel_id];

    return ledc_get_duty(l_chn->mode, l_chn->channel);
}

esp_err_t iot_light_fade_with_time(light_handle_t light_handle, uint8_t channel_id, uint32_t duty, uint32_t fade_period_ms)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    POINT_ASSERT(TAG, light->channel_group[channel_id]);
    light_channel_t *l_chn = light->channel_group[channel_id];

    if (fade_period_ms == 0) {
        return iot_light_duty_set(light, channel_id, duty);
    }

    light_channel_fade_clear(l_chn);

    l_chn->fade_step_num = fade_period_ms / DUTY_SET_CYCLE + 1;
    l_chn->fade_duty_step = (uint32_t *)heap_caps_malloc(l_chn->fade_step_num * sizeof(uint32_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    uint32_t duty_cur = iot_light_duty_get(light, channel_id);
    int duty_delta = duty - duty_cur;

    if (duty_delta > 0) {
        float coe = (duty_delta / pow((double)l_chn->fade_step_num, (double)1 / DUTY_SET_GAMMA));

        for (int i = 0; i < l_chn->fade_step_num; i++) {
            l_chn->fade_duty_step[i] = duty_cur + (int)(coe * pow((double)i, (double)1 / DUTY_SET_GAMMA));
        }

        l_chn->fade_step_up = true;
    } else {
        duty_delta = 0 - duty_delta;
        float coe = (duty_delta / pow((double)l_chn->fade_step_num, (double)1 / DUTY_SET_GAMMA));

        for (int i = 0; i < l_chn->fade_step_num; i++) {
            l_chn->fade_duty_step[i] = duty + (int)(coe * pow((double)i, (double)1 / DUTY_SET_GAMMA));
        }

        l_chn->fade_duty_counter = l_chn->fade_step_num;
        l_chn->fade_step_up = false;
    }

    l_chn->fade_once  = true;
    l_chn->fade_start = true;

    return ESP_OK;
}

esp_err_t iot_light_breath_config(light_handle_t light_handle, uint8_t channel_id, uint32_t duty, uint32_t breath_period_ms)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    POINT_ASSERT(TAG, light->channel_group[channel_id]);
    light_channel_t *l_chn = light->channel_group[channel_id];

    light_channel_fade_clear(l_chn);

    /**
     * @brief control light with nonlinearity fade, the duty and the fade step
     *        conform to gamma curve. gamma curve formula: y=a*x^(1/gm) firstly,
     *        use the target duty and step number to calculate coefficient `a`,
     *        secondly, calculate the duty for every step
     */

    l_chn->fade_step_num = (breath_period_ms / 2) / DUTY_SET_CYCLE + 1;
    float coe = (duty / pow((double)l_chn->fade_step_num, (double)1 / DUTY_SET_GAMMA));

    l_chn->fade_duty_step = (uint32_t *)heap_caps_malloc(l_chn->fade_step_num * sizeof(uint32_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);

    for (int i = 0; i < l_chn->fade_step_num; i++) {
        l_chn->fade_duty_step[i] = (int)(coe * pow((double)i, (double)1 / DUTY_SET_GAMMA));
    }

    return ESP_OK;
}

esp_err_t iot_light_blink_config(light_handle_t light_handle, uint8_t channel_id, uint32_t blink_period_ms)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    light_channel_t *l_chn = light->channel_group[channel_id];

    light_channel_fade_clear(l_chn);

    l_chn->fade_step_num  = (blink_period_ms / 2) / DUTY_SET_CYCLE;
    l_chn->fade_duty_step = (uint32_t *)heap_caps_malloc(l_chn->fade_step_num * sizeof(uint32_t), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    int i = 0;

    for (i = 0; i < (l_chn->fade_step_num / 2); i++) {
        l_chn->fade_duty_step[i] = 0;
    }

    for (; i < l_chn->fade_step_num; i++) {
        l_chn->fade_duty_step[i] = light->full_duty;
    }

    return ESP_OK;
}

esp_err_t iot_light_operate_start(light_handle_t light_handle, uint8_t channel_id)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    light->channel_group[channel_id]->fade_start = true;
    return ESP_OK;
}

esp_err_t iot_light_operate_stop(light_handle_t light_handle, uint8_t channel_id)
{
    light_t *light = (light_t *)light_handle;
    POINT_ASSERT(TAG, light_handle);
    IOT_CHECK(TAG, channel_id < light->channel_num, ESP_FAIL);
    light->channel_group[channel_id]->fade_start = false;
    // iot_light_duty_set(light, channel_id, 0);
    return ESP_OK;
}
