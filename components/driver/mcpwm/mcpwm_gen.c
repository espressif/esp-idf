/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdarg.h>
#include <sys/cdefs.h>
#include "sdkconfig.h"
#if CONFIG_MCPWM_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "freertos/FreeRTOS.h"
#include "esp_attr.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/mcpwm_periph.h"
#include "hal/mcpwm_ll.h"
#include "driver/gpio.h"
#include "driver/mcpwm_gen.h"
#include "mcpwm_private.h"

static const char *TAG = "mcpwm";

static esp_err_t mcpwm_generator_register_to_operator(mcpwm_gen_t *gen, mcpwm_oper_t *oper)
{
    int gen_id = -1;
    portENTER_CRITICAL(&oper->spinlock);
    for (int i = 0; i < SOC_MCPWM_GENERATORS_PER_OPERATOR; i++) {
        if (!oper->generators[i]) {
            oper->generators[i] = gen;
            gen_id = i;
            break;
        }
    }
    portEXIT_CRITICAL(&oper->spinlock);
    ESP_RETURN_ON_FALSE(gen_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free generator in operator (%d,%d)", oper->group->group_id, oper->oper_id);

    gen->gen_id = gen_id;
    gen->oper = oper;
    return ESP_OK;
}

static void mcpwm_generator_unregister_from_operator(mcpwm_gen_t *gen)
{
    mcpwm_oper_t *oper = gen->oper;
    int gen_id = gen->gen_id;

    portENTER_CRITICAL(&oper->spinlock);
    oper->generators[gen_id] = NULL;
    portEXIT_CRITICAL(&oper->spinlock);
}

static esp_err_t mcpwm_generator_destory(mcpwm_gen_t *gen)
{
    if (gen->oper) {
        mcpwm_generator_unregister_from_operator(gen);
    }
    free(gen);
    return ESP_OK;
}

esp_err_t mcpwm_new_generator(mcpwm_oper_handle_t oper, const mcpwm_generator_config_t *config, mcpwm_gen_handle_t *ret_gen)
{
    esp_err_t ret = ESP_OK;
    mcpwm_gen_t *gen = NULL;
    ESP_GOTO_ON_FALSE(oper && config && ret_gen, ESP_ERR_INVALID_ARG, err, TAG, "invalid argument");

    gen = heap_caps_calloc(1, sizeof(mcpwm_gen_t), MCPWM_MEM_ALLOC_CAPS);
    ESP_GOTO_ON_FALSE(gen, ESP_ERR_NO_MEM, err, TAG, "no mem for generator");

    ESP_GOTO_ON_ERROR(mcpwm_generator_register_to_operator(gen, oper), err, TAG, "register generator failed");
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    int gen_id = gen->gen_id;

    // reset generator
    mcpwm_hal_generator_reset(hal, oper_id, gen_id);

    // GPIO configuration
    gpio_config_t gpio_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT | (config->flags.io_loop_back ? GPIO_MODE_INPUT : 0), // also enable the input path if `io_loop_back` is enabled
        .pin_bit_mask = (1ULL << config->gen_gpio_num),
        .pull_down_en = false,
        .pull_up_en = true,
    };
    ESP_GOTO_ON_ERROR(gpio_config(&gpio_conf), err, TAG, "config gen GPIO failed");
    esp_rom_gpio_connect_out_signal(config->gen_gpio_num,
                                    mcpwm_periph_signals.groups[group->group_id].operators[oper_id].generators[gen_id].pwm_sig,
                                    config->flags.invert_pwm, 0);

    // fill in other generator members
    gen->gen_gpio_num = config->gen_gpio_num;
    gen->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    *ret_gen = gen;
    ESP_LOGD(TAG, "new generator (%d,%d,%d) at %p, GPIO %d", group->group_id, oper_id, gen_id, gen, gen->gen_gpio_num);
    return ESP_OK;

err:
    if (gen) {
        mcpwm_generator_destory(gen);
    }
    return ret;
}

esp_err_t mcpwm_del_generator(mcpwm_gen_handle_t gen)
{
    ESP_RETURN_ON_FALSE(gen, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = gen->oper;
    mcpwm_group_t *group = oper->group;

    ESP_LOGD(TAG, "del generator (%d,%d,%d)", group->group_id, oper->oper_id, gen->gen_id);
    // recycle memory resource
    ESP_RETURN_ON_ERROR(mcpwm_generator_destory(gen), TAG, "destory generator failed");
    return ESP_OK;
}

esp_err_t mcpwm_generator_set_force_level(mcpwm_gen_handle_t gen, int level, bool hold_on)
{
    ESP_RETURN_ON_FALSE(gen && level <= 1, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = gen->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;
    int gen_id = gen->gen_id;

    if (level < 0) { // to remove the force level
        if (hold_on) {
            mcpwm_ll_gen_disable_continue_force_action(hal->dev, oper_id, gen_id);
        } else {
            mcpwm_ll_gen_disable_noncontinue_force_action(hal->dev, oper_id, gen_id);
        }
    } else { // to enable the force output level
        if (hold_on) {
            mcpwm_ll_gen_set_continue_force_level(hal->dev, oper_id, gen_id, level);
        } else {
            mcpwm_ll_gen_set_noncontinue_force_level(hal->dev, oper_id, gen_id, level);
            mcpwm_ll_gen_trigger_noncontinue_force_action(hal->dev, oper_id, gen_id);
        }
    }
    return ESP_OK;
}

esp_err_t mcpwm_generator_set_actions_on_timer_event(mcpwm_gen_handle_t gen, mcpwm_gen_timer_event_action_t ev_act, ...)
{
    ESP_RETURN_ON_FALSE(gen, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = gen->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_timer_t *timer = oper->timer;
    ESP_RETURN_ON_FALSE(timer, ESP_ERR_INVALID_STATE, TAG, "no timer is connected to the operator");
    mcpwm_gen_timer_event_action_t ev_act_itor = ev_act;
    bool invalid_utep = false;
    bool invalid_dtez = false;
    va_list it;
    va_start(it, ev_act);
    while (ev_act_itor.event != MCPWM_TIMER_EVENT_INVALID) {
        invalid_utep = (timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN) &&
                       (ev_act_itor.direction == MCPWM_TIMER_DIRECTION_UP) &&
                       (ev_act_itor.event == MCPWM_TIMER_EVENT_FULL);
        invalid_dtez = (timer->count_mode == MCPWM_TIMER_COUNT_MODE_UP_DOWN) &&
                       (ev_act_itor.direction == MCPWM_TIMER_DIRECTION_DOWN) &&
                       (ev_act_itor.event == MCPWM_TIMER_EVENT_EMPTY);
        if (invalid_utep || invalid_dtez) {
            va_end(it);
            ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "UTEP and DTEZ can't be reached under MCPWM_TIMER_COUNT_MODE_UP_DOWN mode");
        }
        mcpwm_ll_generator_set_action_on_timer_event(group->hal.dev, oper->oper_id, gen->gen_id,
                ev_act_itor.direction, ev_act_itor.event, ev_act_itor.action);
        ev_act_itor = va_arg(it, mcpwm_gen_timer_event_action_t);
    }
    va_end(it);
    return ESP_OK;
}

esp_err_t mcpwm_generator_set_actions_on_compare_event(mcpwm_gen_handle_t gen, mcpwm_gen_compare_event_action_t ev_act, ...)
{
    ESP_RETURN_ON_FALSE(gen, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = gen->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_gen_compare_event_action_t ev_act_itor = ev_act;
    va_list it;
    va_start(it, ev_act);
    while (ev_act_itor.comparator) {
        mcpwm_ll_generator_set_action_on_compare_event(group->hal.dev, oper->oper_id, gen->gen_id,
                ev_act_itor.direction, ev_act_itor.comparator->cmpr_id, ev_act_itor.action);
        ev_act_itor = va_arg(it, mcpwm_gen_compare_event_action_t);
    }
    va_end(it);
    return ESP_OK;
}

esp_err_t mcpwm_generator_set_actions_on_brake_event(mcpwm_gen_handle_t gen, mcpwm_gen_brake_event_action_t ev_act, ...)
{
    ESP_RETURN_ON_FALSE(gen, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    mcpwm_oper_t *oper = gen->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_gen_brake_event_action_t ev_act_itor = ev_act;
    va_list it;
    va_start(it, ev_act);
    while (ev_act_itor.brake_mode != MCPWM_OPER_BRAKE_MODE_INVALID) {
        mcpwm_ll_generator_set_action_on_brake_event(group->hal.dev, oper->oper_id, gen->gen_id,
                ev_act_itor.direction, ev_act_itor.brake_mode, ev_act_itor.action);
        ev_act_itor = va_arg(it, mcpwm_gen_brake_event_action_t);
    }
    va_end(it);
    return ESP_OK;
}

esp_err_t mcpwm_generator_set_dead_time(mcpwm_gen_handle_t in_generator, mcpwm_gen_handle_t out_generator, const mcpwm_dead_time_config_t *config)
{
    ESP_RETURN_ON_FALSE(in_generator && out_generator && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(in_generator->oper == out_generator->oper, ESP_ERR_INVALID_ARG, TAG, "in/out generator are not derived from the same operator");
    ESP_RETURN_ON_FALSE(config->negedge_delay_ticks < MCPWM_LL_MAX_DEAD_DELAY && config->posedge_delay_ticks < MCPWM_LL_MAX_DEAD_DELAY,
                        ESP_ERR_INVALID_ARG, TAG, "delay time out of range");
    mcpwm_oper_t *oper = in_generator->oper;
    mcpwm_group_t *group = oper->group;
    mcpwm_hal_context_t *hal = &group->hal;
    int oper_id = oper->oper_id;

    // Note: to better understand the following code, you should read the deadtime module topology diagram in the TRM
    // check if we want to bypass the deadtime module
    bool bypass = (config->negedge_delay_ticks == 0) && (config->posedge_delay_ticks == 0);
    // check is we want to delay on the both edge
    bool delay_on_both_edge = config->posedge_delay_ticks && config->negedge_delay_ticks;
    int out_path_id = -1;
    if (bypass) {
        // out path is same to the input path of generator
        out_path_id = in_generator->gen_id;
    } else if (config->negedge_delay_ticks) {
        out_path_id = 1; // FED path
    } else {
        out_path_id = 0; // RED path
    }
    bool swap_path = out_path_id != out_generator->gen_id;
    mcpwm_ll_deadtime_bypass_path(hal->dev, oper_id, out_path_id, bypass); // S0/1
    if (!bypass) {
        if (config->posedge_delay_ticks) {
            mcpwm_ll_deadtime_red_select_generator(hal->dev, oper_id, in_generator->gen_id); // S4
        } else {
            mcpwm_ll_deadtime_fed_select_generator(hal->dev, oper_id, in_generator->gen_id); // S5
        }
        mcpwm_ll_deadtime_enable_deb(hal->dev, oper_id, delay_on_both_edge); // S8
        mcpwm_ll_deadtime_invert_outpath(hal->dev, oper_id, out_path_id, config->flags.invert_output); // S2/3
        mcpwm_ll_deadtime_swap_out_path(hal->dev, oper_id, out_generator->gen_id, swap_path); // S6/S7
    }
    // set delay time
    if (config->posedge_delay_ticks) {
        mcpwm_ll_deadtime_set_rising_delay(hal->dev, oper_id, config->posedge_delay_ticks);
    }
    if (config->negedge_delay_ticks) {
        mcpwm_ll_deadtime_set_falling_delay(hal->dev, oper_id, config->negedge_delay_ticks);
    }

    ESP_LOGD(TAG, "operator (%d,%d) dead time (R:%"PRIu32",F:%"PRIu32"), topology code:%"PRIx32, group->group_id, oper_id,
             config->posedge_delay_ticks, config->negedge_delay_ticks, mcpwm_ll_deadtime_get_switch_topology(hal->dev, oper_id));
    return ESP_OK;
}
