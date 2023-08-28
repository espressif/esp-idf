/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "mesh/config.h"
#include "mesh/common.h"
#include "mesh/model_common.h"
#include "mesh/model_opcode.h"
#include "mesh/state_binding.h"
#include "mesh/state_transition.h"

#if CONFIG_BLE_MESH_SERVER_MODEL

uint16_t bt_mesh_convert_lightness_actual_to_linear(uint16_t actual)
{
    float tmp = ((float) actual / UINT16_MAX);

    return bt_mesh_ceil(UINT16_MAX * tmp * tmp);
}

uint16_t bt_mesh_convert_lightness_linear_to_actual(uint16_t linear)
{
    return (uint16_t) (UINT16_MAX * bt_mesh_sqrt(((float) linear / UINT16_MAX)));
}

int16_t bt_mesh_convert_temperature_to_gen_level(uint16_t temp, uint16_t min, uint16_t max)
{
    float tmp = (temp - min) * UINT16_MAX / (max - min);
    return (int16_t) (tmp + INT16_MIN);
}

uint16_t bt_mesh_covert_gen_level_to_temperature(int16_t level, uint16_t min, uint16_t max)
{
    float diff = (float) (max - min) / UINT16_MAX;
    uint16_t tmp = (uint16_t) ((level - INT16_MIN) * diff);
    return (uint16_t) (min + tmp);
}

int16_t bt_mesh_convert_hue_to_level(uint16_t hue)
{
    return (int16_t) (hue + INT16_MIN);
}

uint16_t bt_mesh_convert_level_to_hue(int16_t level)
{
    return (uint16_t) (level - INT16_MIN);
}

int16_t bt_mesh_convert_saturation_to_level(uint16_t saturation)
{
    return (int16_t) (saturation + INT16_MIN);
}

uint16_t bt_mesh_convert_level_to_saturation(int16_t level)
{
    return (uint16_t) (level - INT16_MIN);
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
#if CONFIG_BLE_MESH_GENERIC_SERVER
    case GENERIC_ONOFF_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_ONOFF_SRV) {
            BT_ERR("Invalid Generic OnOff Server, model id 0x%04x", model->id);
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
            BT_ERR("Invalid Generic Level Server, model id 0x%04x", model->id);
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
            BT_ERR("Invalid Generic Power OnOff Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_power_onoff_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Generic Power OnOff Server state");
            return -EINVAL;
        }

        srv->state->onpowerup = value->gen_onpowerup.onpowerup;
        gen_onpowerup_publish(model);
        break;
    }
    case GENERIC_POWER_ACTUAL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV) {
            BT_ERR("Invalid Generic Power Level Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_gen_power_level_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Generic Power Level Server state");
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
#endif /* CONFIG_BLE_MESH_GENERIC_SERVER */
#if CONFIG_BLE_MESH_LIGHTING_SERVER
    case LIGHT_LIGHTNESS_ACTUAL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            BT_ERR("Invalid Light Lightness Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->actual_transition);
        srv->state->lightness_actual = value->light_lightness_actual.lightness;
        /**
         * Whenever the Light Lightness Actual state is changed with a non-transactional
         * message or a completed sequence of transactional messages to a non-zero value,
         * the value of the Light Lightness Last shall be set to the value of the Light
         * Lightness Actual.
         */
        if (srv->state->lightness_actual) {
            srv->state->lightness_last = srv->state->lightness_actual;
        }
        light_lightness_publish(model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS);
        break;
    }
    case LIGHT_LIGHTNESS_LINEAR_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV) {
            BT_ERR("Invalid Light Lightness Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lightness_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light Lightness Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->linear_transition);
        srv->state->lightness_linear = value->light_lightness_linear.lightness;
        light_lightness_publish(model, BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS);
        break;
    }
    case LIGHT_CTL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_CTL_SRV) {
            BT_ERR("Invalid Light CTL Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_ctl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_ctl_lightness.lightness;
        light_ctl_publish(model, BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS);
        break;
    }
    case LIGHT_CTL_TEMP_DELTA_UV_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV) {
            BT_ERR("Invalid Light CTL Temperature Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_ctl_temp_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light CTL Temperature Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->temperature = value->light_ctl_temp_delta_uv.temperature;
        srv->state->delta_uv = value->light_ctl_temp_delta_uv.delta_uv;
        light_ctl_publish(model, BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS);
        break;
    }
    case LIGHT_HSL_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_SRV) {
            BT_ERR("Invalid Light HSL Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_hsl.lightness;
        srv->state->hue = value->light_hsl.hue;
        srv->state->saturation = value->light_hsl.saturation;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        break;
    }
    case LIGHT_HSL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_SRV) {
            BT_ERR("Invalid Light HSL Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_hsl_lightness.lightness;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS);
        break;
    }
    case LIGHT_HSL_HUE_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV) {
            BT_ERR("Invalid Light HSL Hue Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_hue_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Hue Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->hue = value->light_hsl_hue.hue;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS);
        break;
    }
    case LIGHT_HSL_SATURATION_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV) {
            BT_ERR("Invalid Light HSL Saturation Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_hsl_sat_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light HSL Saturation Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->saturation = value->light_hsl_saturation.saturation;
        light_hsl_publish(model, BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS);
        break;
    }
    case LIGHT_XYL_LIGHTNESS_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_XYL_SRV) {
            BT_ERR("Invalid Light xyL Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_xyl_srv *srv = model->user_data;
        if (srv->state == NULL) {
            BT_ERR("Invalid Light xyL Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->state->lightness = value->light_xyl_lightness.lightness;
        light_xyl_publish(model, BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS);
        break;
    }
    case LIGHT_LC_LIGHT_ONOFF_STATE: {
        if (model->id != BLE_MESH_MODEL_ID_LIGHT_LC_SRV) {
            BT_ERR("Invalid Light LC Server, model id 0x%04x", model->id);
            return -EINVAL;
        }

        struct bt_mesh_light_lc_srv *srv = model->user_data;
        if (srv->lc == NULL) {
            BT_ERR("Invalid Light LC Server state");
            return -EINVAL;
        }

        bt_mesh_server_stop_transition(&srv->transition);
        srv->lc->state.light_onoff = value->light_lc_light_onoff.onoff;
        light_lc_publish(model, BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS);
        break;
    }
#endif /* CONFIG_BLE_MESH_LIGHTING_SERVER */
    default:
        BT_WARN("Unknown binding state type 0x%02x", type);
        return -EINVAL;
    }

    return 0;
}

#endif /* CONFIG_BLE_MESH_SERVER_MODEL */
