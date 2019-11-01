/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "mesh_types.h"
#include "mesh_kernel.h"
#include "mesh_trace.h"
#include "mesh.h"
#include "model_opcode.h"

#include "server_common.h"
#include "state_binding.h"
#include "state_transition.h"
#include "generic_server.h"
#include "lighting_server.h"
#include "time_scene_server.h"

#include "btc_ble_mesh_generic_model.h"
#include "btc_ble_mesh_lighting_model.h"
#include "btc_ble_mesh_time_scene_model.h"
#include "btc_ble_mesh_sensor_model.h"

/* Function to calculate Remaining Time (Start) */

void bt_mesh_server_calc_remain_time(struct bt_mesh_state_transition *transition)
{
    u8_t steps, resolution;
    s32_t duration_remainder;
    s64_t now;

    if (transition->just_started) {
        transition->remain_time = transition->trans_time;
    } else {
        now = k_uptime_get();
        duration_remainder = transition->total_duration -
                             (now - transition->start_timestamp);
        if (duration_remainder > 620000) {
            /* > 620 seconds -> resolution = 0b11 [10 minutes] */
            resolution = 0x03;
            steps = duration_remainder / 600000;
        } else if (duration_remainder > 62000) {
            /* > 62 seconds -> resolution = 0b10 [10 seconds] */
            resolution = 0x02;
            steps = duration_remainder / 10000;
        } else if (duration_remainder > 6200) {
            /* > 6.2 seconds -> resolution = 0b01 [1 seconds] */
            resolution = 0x01;
            steps = duration_remainder / 1000;
        } else if (duration_remainder > 0) {
            /* <= 6.2 seconds -> resolution = 0b00 [100 ms] */
            resolution = 0x00;
            steps = duration_remainder / 100;
        } else {
            resolution = 0x00;
            steps = 0x00;
        }

        transition->remain_time = (resolution << 6) | steps;
    }
}

/* Function to calculate Remaining Time (End) */

static void tt_values_calculator(struct bt_mesh_state_transition *transition)
{
    u8_t steps_multiplier, resolution;

    resolution = (transition->trans_time >> 6);
    steps_multiplier = (transition->trans_time & 0x3F);

    switch (resolution) {
    case 0: /* 100ms */
        transition->total_duration = steps_multiplier * 100;
        break;
    case 1: /* 1 second */
        transition->total_duration = steps_multiplier * 1000;
        break;
    case 2: /* 10 seconds */
        transition->total_duration = steps_multiplier * 10000;
        break;
    case 3: /* 10 minutes */
        transition->total_duration = steps_multiplier * 600000;
        break;
    }

    transition->counter = ((float) transition->total_duration / 100);

    if (transition->counter > BLE_MESH_DEVICE_SPECIFIC_RESOLUTION) {
        transition->counter = BLE_MESH_DEVICE_SPECIFIC_RESOLUTION;
    }
}

static void transition_time_values(struct bt_mesh_state_transition *transition,
                                   u8_t trans_time, u8_t delay)
{
    transition->trans_time = trans_time;
    transition->delay = delay;

    if (trans_time == 0U) {
        return;
    }

    tt_values_calculator(transition);
    transition->quo_tt = transition->total_duration / transition->counter;
}

void generic_onoff_tt_values(struct bt_mesh_gen_onoff_srv *srv,
                             u8_t trans_time, u8_t delay)
{
    return transition_time_values(&srv->transition, trans_time, delay);
}

void generic_level_tt_values(struct bt_mesh_gen_level_srv *srv,
                             u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_level =
        ((float) (srv->state.level - srv->state.target_level) / srv->transition.counter);
}

void generic_power_level_tt_values(struct bt_mesh_gen_power_level_srv *srv,
                                   u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_level =
        ((float) (srv->state->power_actual - srv->state->target_power_actual) / srv->transition.counter);
}

void light_lightness_actual_tt_values(struct bt_mesh_light_lightness_srv *srv,
                                      u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->actual_transition, trans_time, delay);
    srv->tt_delta_lightness_actual =
        ((float) (srv->state->lightness_actual - srv->state->target_lightness_actual) / srv->actual_transition.counter);
}

void light_lightness_linear_tt_values(struct bt_mesh_light_lightness_srv *srv,
                                      u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->linear_transition, trans_time, delay);
    srv->tt_delta_lightness_linear =
        ((float) (srv->state->lightness_linear - srv->state->target_lightness_linear) / srv->linear_transition.counter);
}

void light_ctl_tt_values(struct bt_mesh_light_ctl_srv *srv,
                         u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_lightness =
        ((float) (srv->state->lightness - srv->state->target_lightness) / srv->transition.counter);
    srv->tt_delta_temperature =
        ((float) (srv->state->temperature - srv->state->target_temperature) / srv->transition.counter);
    srv->tt_delta_delta_uv =
        ((float) (srv->state->delta_uv - srv->state->target_delta_uv) / srv->transition.counter);
}

void light_ctl_temp_tt_values(struct bt_mesh_light_ctl_temp_srv *srv,
                              u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_temperature =
        ((float) (srv->state->temperature - srv->state->target_temperature) / srv->transition.counter);
    srv->tt_delta_delta_uv =
        ((float) (srv->state->delta_uv - srv->state->target_delta_uv) / srv->transition.counter);
}

void light_hsl_tt_values(struct bt_mesh_light_hsl_srv *srv,
                         u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_lightness =
        ((float) (srv->state->lightness - srv->state->target_lightness) / srv->transition.counter);
    srv->tt_delta_hue =
        ((float) (srv->state->hue - srv->state->target_hue) / srv->transition.counter);
    srv->tt_delta_saturation =
        ((float) (srv->state->saturation - srv->state->target_saturation) / srv->transition.counter);
}

void light_hsl_hue_tt_values(struct bt_mesh_light_hsl_hue_srv *srv,
                             u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_hue =
        ((float) (srv->state->hue - srv->state->target_hue) / srv->transition.counter);
}

void light_hsl_sat_tt_values(struct bt_mesh_light_hsl_sat_srv *srv,
                             u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_saturation =
        ((float) (srv->state->saturation - srv->state->target_saturation) / srv->transition.counter);
}

void light_xyl_tt_values(struct bt_mesh_light_xyl_srv *srv,
                         u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
    srv->tt_delta_lightness =
        ((float) (srv->state->lightness - srv->state->target_lightness) / srv->transition.counter);
    srv->tt_delta_x =
        ((float) (srv->state->x - srv->state->target_x) / srv->transition.counter);
    srv->tt_delta_y =
        ((float) (srv->state->y - srv->state->target_y) / srv->transition.counter);
}

void light_lc_tt_values(struct bt_mesh_light_lc_srv *srv,
                        u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
}

void scene_tt_values(struct bt_mesh_scene_srv *srv, u8_t trans_time, u8_t delay)
{
    transition_time_values(&srv->transition, trans_time, delay);
}

static void transition_timer_start(struct bt_mesh_state_transition *transition)
{
    transition->start_timestamp = k_uptime_get();
    k_delayed_work_submit_periodic(&transition->timer, K_MSEC(transition->quo_tt));
    bt_mesh_atomic_set_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
}

static void transition_timer_stop(struct bt_mesh_state_transition *transition)
{
    k_delayed_work_cancel(&transition->timer);
    bt_mesh_atomic_clear_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
}

/* Timers related handlers & threads (Start) */
void generic_onoff_work_handler(struct k_work *work)
{
    struct bt_mesh_gen_onoff_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_gen_onoff_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_gen_server_state_change_t change = {0};

    if (srv == NULL || srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_generic_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.gen_onoff_set.onoff = srv->state.onoff;
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            /**
             * Because binary states cannot support transitions, when changing to
             * 0x01 (On), the Generic OnOff state shall change immediately when
             * the transition starts, and when changing to 0x00, the state shall
             * change when the transition finishes.
             */
            if (srv->state.target_onoff == BLE_MESH_STATE_ON) {
                srv->state.onoff = BLE_MESH_STATE_ON;
                change.gen_onoff_set.onoff = srv->state.onoff;
                bt_mesh_generic_server_cb_evt_to_btc(
                    BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            }
            transition_timer_start(&srv->transition);
        }

        bt_mesh_generic_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state.onoff = srv->state.target_onoff;
        if (srv->state.target_onoff != BLE_MESH_STATE_ON) {
            change.gen_onoff_set.onoff = srv->state.onoff;
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
        }
    }

    gen_onoff_publish(srv->model);

    bt_mesh_generic_server_unlock();
    return;
}

void generic_level_work_handler(struct k_work *work)
{
    struct bt_mesh_gen_level_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_gen_level_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_gen_server_state_change_t change = {0};

    if (srv == NULL || srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_generic_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            switch (ctx->recv_op) {
            case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
            case BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:
                change.gen_level_set.level = srv->state.level;
                break;
            case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
            case BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK:
                change.gen_delta_set.level = srv->state.level;
                break;
            case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
            case BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK:
                change.gen_move_set.level = srv->state.level;
                break;
            }
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_generic_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state.level -= srv->tt_delta_level;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state.level = srv->state.target_level;
    }

    switch (ctx->recv_op) {
    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET:
    case BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK:
        change.gen_level_set.level = srv->state.level;
        break;
    case BLE_MESH_MODEL_OP_GEN_DELTA_SET:
    case BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK:
        change.gen_delta_set.level = srv->state.level;
        break;
    case BLE_MESH_MODEL_OP_GEN_MOVE_SET:
    case BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK:
        change.gen_move_set.level = srv->state.level;
        break;
    }
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    gen_level_publish(srv->model);

    bt_mesh_generic_server_unlock();
    return;
}

void generic_power_level_work_handler(struct k_work *work)
{
    struct bt_mesh_gen_power_level_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_gen_power_level_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_gen_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_generic_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.gen_power_level_set.power = srv->state->power_actual;
            bt_mesh_generic_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_generic_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->power_actual -= srv->tt_delta_level;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);

        srv->state->power_actual = srv->state->target_power_actual;
        /**
         * Whenever the Generic Power Actual state is changed to a non-zero value
         * as a result of a non-transactional message or a completed sequence of
         * transactional messages, the value of the Generic Power Last state shall
         * be set to the value of the Generic Power Actual state.
         */
        if (srv->state->power_actual) {
            srv->state->power_last = srv->state->power_actual;
        }
    }

    change.gen_power_level_set.power = srv->state->power_actual;
    bt_mesh_generic_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_GENERIC_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    gen_power_level_publish(srv->model, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);

    bt_mesh_generic_server_unlock();
    return;
}

void light_lightness_actual_work_handler(struct k_work *work)
{
    struct bt_mesh_light_lightness_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_lightness_srv, actual_transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->actual_transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->actual_transition.timer.work._reserved;

    if (srv->actual_transition.just_started) {
        srv->actual_transition.just_started = false;
        if (srv->actual_transition.counter == 0U) {
            change.lightness_set.lightness = srv->state->lightness_actual;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->actual_transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->actual_transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->actual_transition.counter != 0U) {
        srv->actual_transition.counter--;
        srv->state->lightness_actual -= srv->tt_delta_lightness_actual;
    }

    if (srv->actual_transition.counter == 0U) {
        transition_timer_stop(&srv->actual_transition);

        srv->state->lightness_actual = srv->state->target_lightness_actual;
        /**
         * Whenever the Light Lightness Actual state is changed with a non-
         * transactional message or a completed sequence of transactional
         * messages to a non-zero value, the value of the Light Lightness
         * Last shall be set to the value of the Light Lightness Actual.
         */
        if (srv->state->lightness_actual) {
            srv->state->lightness_last = srv->state->lightness_actual;
        }
    }

    change.lightness_set.lightness = srv->state->lightness_actual;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_lightness_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_lightness_linear_work_handler(struct k_work *work)
{
    struct bt_mesh_light_lightness_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_lightness_srv, linear_transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->linear_transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->linear_transition.timer.work._reserved;

    if (srv->linear_transition.just_started) {
        srv->linear_transition.just_started = false;
        if (srv->linear_transition.counter == 0U) {
            change.lightness_linear_set.lightness = srv->state->lightness_linear;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->linear_transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->linear_transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->linear_transition.counter != 0U) {
        srv->linear_transition.counter--;
        srv->state->lightness_linear -= srv->tt_delta_lightness_linear;
    }

    if (srv->linear_transition.counter == 0U) {
        transition_timer_stop(&srv->linear_transition);
        srv->state->lightness_linear = srv->state->target_lightness_linear;
    }

    change.lightness_linear_set.lightness = srv->state->lightness_linear;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_lightness_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_ctl_work_handler(struct k_work *work)
{
    struct bt_mesh_light_ctl_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_ctl_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.ctl_set.lightness = srv->state->lightness;
            change.ctl_set.temperature = srv->state->temperature;
            change.ctl_set.delta_uv = srv->state->delta_uv;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->lightness -= srv->tt_delta_lightness;
        srv->state->temperature -= srv->tt_delta_temperature;
        srv->state->delta_uv -= srv->tt_delta_delta_uv;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->lightness = srv->state->target_lightness;
        srv->state->temperature = srv->state->target_temperature;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    change.ctl_set.lightness = srv->state->lightness;
    change.ctl_set.temperature = srv->state->temperature;
    change.ctl_set.delta_uv = srv->state->delta_uv;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_ctl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_ctl_temp_work_handler(struct k_work *work)
{
    struct bt_mesh_light_ctl_temp_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_ctl_temp_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.ctl_temp_set.temperature = srv->state->temperature;
            change.ctl_temp_set.delta_uv = srv->state->delta_uv;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->temperature -= srv->tt_delta_temperature;
        srv->state->delta_uv -= srv->tt_delta_delta_uv;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->temperature = srv->state->target_temperature;
        srv->state->delta_uv = srv->state->target_delta_uv;
    }

    change.ctl_temp_set.temperature = srv->state->temperature;
    change.ctl_temp_set.delta_uv = srv->state->delta_uv;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_ctl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_hsl_work_handler(struct k_work *work)
{
    struct bt_mesh_light_hsl_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_hsl_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.hsl_set.lightness = srv->state->lightness;
            change.hsl_set.hue = srv->state->hue;
            change.hsl_set.saturation = srv->state->saturation;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->lightness -= srv->tt_delta_lightness;
        srv->state->hue -= srv->tt_delta_hue;
        srv->state->saturation -= srv->tt_delta_saturation;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->lightness = srv->state->target_lightness;
        srv->state->hue = srv->state->target_hue;
        srv->state->saturation = srv->state->target_saturation;
    }

    change.hsl_set.lightness = srv->state->lightness;
    change.hsl_set.hue = srv->state->hue;
    change.hsl_set.saturation = srv->state->saturation;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_hsl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_hsl_hue_work_handler(struct k_work *work)
{
    struct bt_mesh_light_hsl_hue_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_hsl_hue_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.hsl_hue_set.hue = srv->state->hue;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->hue -= srv->tt_delta_hue;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->hue = srv->state->target_hue;
    }

    change.hsl_hue_set.hue = srv->state->hue;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_hsl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_hsl_sat_work_handler(struct k_work *work)
{
    struct bt_mesh_light_hsl_sat_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_hsl_sat_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.hsl_saturation_set.saturation = srv->state->saturation;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->saturation -= srv->tt_delta_saturation;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->saturation = srv->state->target_saturation;
    }

    change.hsl_saturation_set.saturation = srv->state->saturation;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_hsl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_xyl_work_handler(struct k_work *work)
{
    struct bt_mesh_light_xyl_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_xyl_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.xyl_set.lightness = srv->state->lightness;
            change.xyl_set.x = srv->state->x;
            change.xyl_set.y = srv->state->y;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
        srv->state->lightness -= srv->tt_delta_lightness;
        srv->state->x -= srv->tt_delta_x;
        srv->state->y -= srv->tt_delta_y;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->lightness = srv->state->target_lightness;
        srv->state->x = srv->state->target_x;
        srv->state->y = srv->state->target_y;
    }

    change.xyl_set.lightness = srv->state->lightness;
    change.xyl_set.x = srv->state->x;
    change.xyl_set.y = srv->state->y;
    bt_mesh_lighting_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    light_xyl_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void light_lc_work_handler(struct k_work *work)
{
    struct bt_mesh_light_lc_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_light_lc_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_light_server_state_change_t change = {0};

    if (srv == NULL || srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_light_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.lc_light_onoff_set.onoff = srv->lc->state.light_onoff;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            /**
             * Because binary states cannot support transitions, when changing to
             * 0x01 (On), the Generic OnOff state shall change immediately when
             * the transition starts, and when changing to 0x00, the state shall
             * change when the transition finishes.
             */
            if (srv->lc->state.target_light_onoff == BLE_MESH_STATE_ON) {
                srv->lc->state.light_onoff = BLE_MESH_STATE_ON;
                bt_mesh_light_server_state_change_t change = {
                    .lc_light_onoff_set.onoff = srv->lc->state.light_onoff,
                };
                bt_mesh_lighting_server_cb_evt_to_btc(
                    BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            }
            transition_timer_start(&srv->transition);
        }

        bt_mesh_light_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->lc->state.light_onoff = srv->lc->state.target_light_onoff;
        if (srv->lc->state.light_onoff != BLE_MESH_STATE_ON) {
            change.lc_light_onoff_set.onoff = srv->lc->state.light_onoff;
            bt_mesh_lighting_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_LIGHTING_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
        }
    }

    light_lc_publish(srv->model, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);

    bt_mesh_light_server_unlock();
    return;
}

void scene_recall_work_handler(struct k_work *work)
{
    struct bt_mesh_scene_srv *srv =
        CONTAINER_OF(work, struct bt_mesh_scene_srv, transition.timer.work);
    struct bt_mesh_msg_ctx *ctx = NULL;
    bt_mesh_time_scene_server_state_change_t change = {0};

    if (srv == NULL || srv->state == NULL ||
            srv->transition.timer.work._reserved == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_time_scene_server_lock();

    ctx = (struct bt_mesh_msg_ctx *)srv->transition.timer.work._reserved;

    if (srv->transition.just_started) {
        srv->transition.just_started = false;
        if (srv->transition.counter == 0U) {
            change.scene_recall.scene_number = srv->state->current_scene;
            bt_mesh_time_scene_server_cb_evt_to_btc(
                BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));
            bt_mesh_atomic_clear_bit(srv->transition.flag, BLE_MESH_TRANS_TIMER_START);
        } else {
            transition_timer_start(&srv->transition);
        }

        bt_mesh_time_scene_server_unlock();
        return;
    }

    if (srv->transition.counter != 0U) {
        srv->transition.counter--;
    }

    if (srv->transition.counter == 0U) {
        transition_timer_stop(&srv->transition);
        srv->state->current_scene = srv->state->target_scene;
        srv->state->in_progress = false;
        srv->state->target_scene = INVALID_SCENE_NUMBER;
    }

    change.scene_recall.scene_number = srv->state->current_scene;
    bt_mesh_time_scene_server_cb_evt_to_btc(
        BTC_BLE_MESH_EVT_TIME_SCENE_SERVER_STATE_CHANGE, srv->model, ctx, (const u8_t *)&change, sizeof(change));

    scene_publish(srv->model, ctx, BLE_MESH_MODEL_OP_SCENE_STATUS);

    bt_mesh_time_scene_server_unlock();
    return;
}

/* Timers related handlers & threads (End) */

void bt_mesh_server_stop_transition(struct bt_mesh_state_transition *transition)
{
    memset(transition, 0x0, offsetof(struct bt_mesh_state_transition, flag));
    if (bt_mesh_atomic_test_and_clear_bit(transition->flag, BLE_MESH_TRANS_TIMER_START)) {
        k_delayed_work_cancel(&transition->timer);
    }
}

void bt_mesh_server_start_transition(struct bt_mesh_state_transition *transition)
{
    if (transition->delay) {
        k_delayed_work_submit(&transition->timer, K_MSEC(5 * transition->delay));
        bt_mesh_atomic_set_bit(transition->flag, BLE_MESH_TRANS_TIMER_START);
    } else {
        k_work_submit(&transition->timer.work);
    }
}

/* Messages handlers (End) */
