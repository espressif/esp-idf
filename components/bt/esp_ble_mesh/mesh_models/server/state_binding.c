/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
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

#define MINDIFF (2.25e-308)

static float bt_mesh_sqrt(float square)
{
    float root, last, diff;

    root = square / 3.0;
    diff = 1;

    if (square <= 0) {
        return 0;
    }

    do {
        last = root;
        root = (root + square / root) / 2.0;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);

    return root;
}

static s32_t bt_mesh_ceiling(float num)
{
    s32_t inum = (s32_t)num;

    if (num == (float)inum) {
        return inum;
    }

    return inum + 1;
}

u16_t bt_mesh_convert_lightness_actual_to_linear(u16_t actual)
{
    float tmp = ((float) actual / UINT16_MAX);

    return bt_mesh_ceiling(UINT16_MAX * tmp * tmp);
}

u16_t bt_mesh_convert_lightness_linear_to_actual(u16_t linear)
{
    return (u16_t) (UINT16_MAX * bt_mesh_sqrt(((float) linear / UINT16_MAX)));
}

s16_t bt_mesh_convert_temperature_to_gen_level(u16_t temp, u16_t min, u16_t max)
{
    float tmp = (temp - min) * UINT16_MAX / (max - min);
    return (s16_t) (tmp + INT16_MIN);
}

u16_t bt_mesh_covert_gen_level_to_temperature(s16_t level, u16_t min, u16_t max)
{
    float diff = (float) (max - min) / UINT16_MAX;
    u16_t tmp = (u16_t) ((level - INT16_MIN) * diff);
    return (u16_t) (min + tmp);
}

s16_t bt_mesh_convert_hue_to_level(u16_t hue)
{
    return (s16_t) (hue + INT16_MIN);
}

u16_t bt_mesh_convert_level_to_hue(s16_t level)
{
    return (u16_t) (level - INT16_MIN);
}

s16_t bt_mesh_convert_saturation_to_level(u16_t saturation)
{
    return (s16_t) (saturation + INT16_MIN);
}

u16_t bt_mesh_convert_level_to_saturation(s16_t level)
{
    return (u16_t) (level - INT16_MIN);
}

int bt_mesh_update_binding_state(struct bt_mesh_model *model,
                                 bt_mesh_server_state_type_t type,
                                 bt_mesh_server_state_value_t *value)
{
    if (model == NULL || model->user_data == NULL ||
            value == NULL || type > BIND_STATE_MAX) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    switch (type) {
    case GENERIC_ONOFF_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_ONOFF_SRV) {
            BT_ERR("%s, Not a Generic OnOff Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_onoff_srv *srv = model->user_data;
        bt_mesh_server_stop_transition(&srv->transition);
        srv->state.onoff = value->gen_onoff.onoff;
        gen_onoff_publish(model);
        break;
    }
    case GENERIC_LEVEL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_LEVEL_SRV) {
            BT_ERR("%s, Not a Generic Level Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_level_srv *srv = model->user_data;
        bt_mesh_server_stop_transition(&srv->transition);
        srv->state.level = value->gen_level.level;
        gen_level_publish(model);
        break;
    }
    case GENERIC_ONPOWERUP_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV) {
            BT_ERR("%s, Not a Generic Power OnOff Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power OnOff Server state", __func__);
            return -EINVAL;
        }

        srv->state->onpowerup = value->gen_onpowerup.onpowerup;
        gen_onpowerup_publish(model);
        break;
    }
    case GENERIC_POWER_ACTUAL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV) {
            BT_ERR("%s, Not a Generic Power Level Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_power_level_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Generic Power Level Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->power_actual = value->gen_power_actual.power;
        /**
         * Whenever the Generic Power Actual state is changed to a non-zero value
         * as a result of a non-transactional message or a completed sequence of
         * transactional messages, the value of the Generic Power Last state shall
         * be set to the value of the Generic Power Actual state.
         */
        if (srv->state->power_actual) {
            srv->state->power_last = srv->state->power_actual;
        }
        gen_power_level_publish(model, BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS);
        break;
    }
    case LIGHT_LIGHTNESS_ACTUAL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            BT_ERR("%s, Not a Light Lightness Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light Lightness Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->actual_transition);
        srv->state->lightness_actual = value->light_lightness_actual.lightness;
        light_lightness_publish(model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
        break;
    }
    case LIGHT_LIGHTNESS_LINEAR_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            BT_ERR("%s, Not a Light Lightness Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light Lightness Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->linear_transition);
        srv->state->lightness_linear = value->light_lightness_linear.lightness;
        light_lightness_publish(model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
        break;
    }
    case LIGHT_CTL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_CTL_SRV) {
            BT_ERR("%s, Not a Light CTL Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light CTL Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_ctl_lightness.lightness;
        light_ctl_publish(model, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
        break;
    }
    case LIGHT_CTL_TEMP_DELTA_UV_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV) {
            BT_ERR("%s, Not a Light CTL Temperature Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light CTL Temperature Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->temperature = value->light_ctl_temp_delta_uv.temperature;
        srv->state->delta_uv = value->light_ctl_temp_delta_uv.delta_uv;
        light_ctl_publish(model, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
        break;
    }
    case LIGHT_HSL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_SRV) {
            BT_ERR("%s, Not a Light HSL Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light HSL Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_hsl_lightness.lightness;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        break;
    }
    case LIGHT_HSL_HUE_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV) {
            BT_ERR("%s, Not a Light HSL Hue Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light HSL Hue Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->hue = value->light_hsl_hue.hue;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
        break;
    }
    case LIGHT_HSL_SATURATION_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV) {
            BT_ERR("%s, Not a Light HSL Saturation Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light HSL Saturation Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->saturation = value->light_hsl_saturation.saturation;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
        break;
    }
    case LIGHT_XYL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_XYL_SRV) {
            BT_ERR("%s, Not a Light xyL Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("%s, Invalid Light xyL Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_xyl_lightness.lightness;
        light_xyl_publish(model, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        break;
    }
    case LIGHT_LC_LIGHT_ONOFF_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LC_SRV) {
            BT_ERR("%s, Not a Light LC Server Model, id 0x%04x", __func__, model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lc_srv *srv = model->user_data;
        if (srv->lc == NULL) {
            BT_ERR("%s, Invalid Light LC Server state", __func__);
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->lc->state.light_onoff = value->light_lc_light_onoff.onoff;
        light_lc_publish(model, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
        break;
    }
    default:
        BT_WARN("%s, Unknown binding state type 0x%02x", __func__, type);
        return -EINVAL;
    }

    return 0;
}

