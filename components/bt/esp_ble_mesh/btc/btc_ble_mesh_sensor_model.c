/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "btc_ble_mesh_model_common.h"
#include "btc_ble_mesh_sensor_model.h"
#include "esp_ble_mesh_sensor_model_api.h"

#if CONFIG_BLE_MESH_SENSOR_CLI
#include "mesh/sensor_client.h"

/* Sensor Client Models related functions */

static inline void btc_ble_mesh_sensor_client_cb_to_app(esp_ble_mesh_sensor_client_cb_event_t event,
                                                        esp_ble_mesh_sensor_client_cb_param_t *param)
{
    esp_ble_mesh_sensor_client_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_sensor_client_cb_t)btc_profile_cb_get(BTC_PID_SENSOR_CLIENT);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

void btc_ble_mesh_sensor_client_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_mesh_sensor_client_args_t *dst = (btc_ble_mesh_sensor_client_args_t *)p_dest;
    btc_ble_mesh_sensor_client_args_t *src = (btc_ble_mesh_sensor_client_args_t *)p_src;
    uint16_t length = 0U;

    if (!msg || !dst || !src) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_GET_STATE: {
        dst->sensor_client_get_state.params = NULL;
        dst->sensor_client_get_state.get_state = NULL;

        dst->sensor_client_get_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!dst->sensor_client_get_state.params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        dst->sensor_client_get_state.get_state = (esp_ble_mesh_sensor_client_get_state_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_sensor_client_get_state_t));
        if (!dst->sensor_client_get_state.get_state) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            /* Free the previously allocated resources */
            bt_mesh_free(dst->sensor_client_get_state.params);
            dst->sensor_client_get_state.params = NULL;
            return;
        }

        memcpy(dst->sensor_client_get_state.params, src->sensor_client_get_state.params,
               sizeof(esp_ble_mesh_client_common_param_t));
        memcpy(dst->sensor_client_get_state.get_state, src->sensor_client_get_state.get_state,
               sizeof(esp_ble_mesh_sensor_client_get_state_t));

        switch (src->sensor_client_get_state.params->opcode) {
        case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
            if (src->sensor_client_get_state.get_state->column_get.raw_value_x) {
                length = src->sensor_client_get_state.get_state->column_get.raw_value_x->len;
                dst->sensor_client_get_state.get_state->column_get.raw_value_x = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_get_state.get_state->column_get.raw_value_x) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    bt_mesh_free(dst->sensor_client_get_state.params);
                    dst->sensor_client_get_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_get_state.get_state);
                    dst->sensor_client_get_state.get_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_get_state.get_state->column_get.raw_value_x,
                                       src->sensor_client_get_state.get_state->column_get.raw_value_x->data,
                                       src->sensor_client_get_state.get_state->column_get.raw_value_x->len);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
            if (src->sensor_client_get_state.get_state->series_get.raw_value_x1) {
                length = src->sensor_client_get_state.get_state->series_get.raw_value_x1->len;
                dst->sensor_client_get_state.get_state->series_get.raw_value_x1 = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_get_state.get_state->series_get.raw_value_x1) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    bt_mesh_free(dst->sensor_client_get_state.params);
                    dst->sensor_client_get_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_get_state.get_state);
                    dst->sensor_client_get_state.get_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_get_state.get_state->series_get.raw_value_x1,
                                       src->sensor_client_get_state.get_state->series_get.raw_value_x1->data,
                                       src->sensor_client_get_state.get_state->series_get.raw_value_x1->len);
            }
            if (src->sensor_client_get_state.get_state->series_get.raw_value_x2) {
                length = src->sensor_client_get_state.get_state->series_get.raw_value_x2->len;
                dst->sensor_client_get_state.get_state->series_get.raw_value_x2 = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_get_state.get_state->series_get.raw_value_x2) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    if (dst->sensor_client_get_state.get_state->series_get.raw_value_x1) {
                        bt_mesh_free_buf(dst->sensor_client_get_state.get_state->series_get.raw_value_x1);
                        dst->sensor_client_get_state.get_state->series_get.raw_value_x1 = NULL;
                    }
                    bt_mesh_free(dst->sensor_client_get_state.params);
                    dst->sensor_client_get_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_get_state.get_state);
                    dst->sensor_client_get_state.get_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_get_state.get_state->series_get.raw_value_x2,
                                       src->sensor_client_get_state.get_state->series_get.raw_value_x2->data,
                                       src->sensor_client_get_state.get_state->series_get.raw_value_x2->len);
            }
            break;
        default:
            break;
        }
        break;
    }
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_SET_STATE: {
        dst->sensor_client_set_state.params = NULL;
        dst->sensor_client_set_state.set_state = NULL;

        dst->sensor_client_set_state.params = (esp_ble_mesh_client_common_param_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!dst->sensor_client_set_state.params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        dst->sensor_client_set_state.set_state = (esp_ble_mesh_sensor_client_set_state_t *)bt_mesh_calloc(sizeof(esp_ble_mesh_sensor_client_set_state_t));
        if (!dst->sensor_client_set_state.set_state) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            /* Free the previously allocated resources */
            bt_mesh_free(dst->sensor_client_set_state.params);
            dst->sensor_client_set_state.params = NULL;
            return;
        }

        memcpy(dst->sensor_client_set_state.params, src->sensor_client_set_state.params,
               sizeof(esp_ble_mesh_client_common_param_t));
        memcpy(dst->sensor_client_set_state.set_state, src->sensor_client_set_state.set_state,
               sizeof(esp_ble_mesh_sensor_client_set_state_t));

        switch (src->sensor_client_set_state.params->opcode) {
        case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
            if (src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                length = src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down->len;
                dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    bt_mesh_free(dst->sensor_client_set_state.params);
                    dst->sensor_client_set_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_set_state.set_state);
                    dst->sensor_client_set_state.set_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down,
                                       src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down->data,
                                       src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down->len);
            }
            if (src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up) {
                length = src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up->len;
                dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down = NULL;
                    }
                    bt_mesh_free(dst->sensor_client_set_state.params);
                    dst->sensor_client_set_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_set_state.set_state);
                    dst->sensor_client_set_state.set_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up,
                                       src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up->data,
                                       src->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up->len);
            }
            if (src->sensor_client_set_state.set_state->cadence_set.fast_cadence_low) {
                length = src->sensor_client_set_state.set_state->cadence_set.fast_cadence_low->len;
                dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up = NULL;
                    }
                    bt_mesh_free(dst->sensor_client_set_state.params);
                    dst->sensor_client_set_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_set_state.set_state);
                    dst->sensor_client_set_state.set_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low,
                                       src->sensor_client_set_state.set_state->cadence_set.fast_cadence_low->data,
                                       src->sensor_client_set_state.set_state->cadence_set.fast_cadence_low->len);
            }
            if (src->sensor_client_set_state.set_state->cadence_set.fast_cadence_high) {
                length = src->sensor_client_set_state.set_state->cadence_set.fast_cadence_high->len;
                dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low);
                        dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low = NULL;
                    }
                    bt_mesh_free(dst->sensor_client_set_state.params);
                    dst->sensor_client_set_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_set_state.set_state);
                    dst->sensor_client_set_state.set_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high,
                                       src->sensor_client_set_state.set_state->cadence_set.fast_cadence_high->data,
                                       src->sensor_client_set_state.set_state->cadence_set.fast_cadence_high->len);
            }
            break;
        case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
            if (src->sensor_client_set_state.set_state->setting_set.sensor_setting_raw) {
                length = src->sensor_client_set_state.set_state->setting_set.sensor_setting_raw->len;
                dst->sensor_client_set_state.set_state->setting_set.sensor_setting_raw = bt_mesh_alloc_buf(length);
                if (!dst->sensor_client_set_state.set_state->setting_set.sensor_setting_raw) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    /* Free the previously allocated resources */
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up);
                        dst->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low);
                        dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_low = NULL;
                    }
                    if (dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high) {
                        bt_mesh_free_buf(dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high);
                        dst->sensor_client_set_state.set_state->cadence_set.fast_cadence_high = NULL;
                    }
                    bt_mesh_free(dst->sensor_client_set_state.params);
                    dst->sensor_client_set_state.params = NULL;
                    bt_mesh_free(dst->sensor_client_set_state.set_state);
                    dst->sensor_client_set_state.set_state = NULL;
                    return;
                }
                net_buf_simple_add_mem(dst->sensor_client_set_state.set_state->setting_set.sensor_setting_raw,
                                       src->sensor_client_set_state.set_state->setting_set.sensor_setting_raw->data,
                                       src->sensor_client_set_state.set_state->setting_set.sensor_setting_raw->len);
            }
            break;
        default:
            break;
        }
        break;
    }
    default:
        BT_DBG("%s, Unknown act %d", __func__, msg->act);
        break;
    }
}

void btc_ble_mesh_sensor_client_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_mesh_sensor_client_args_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_sensor_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_GET_STATE:
        if (arg->sensor_client_get_state.get_state) {
            if (arg->sensor_client_get_state.params) {
                switch (arg->sensor_client_get_state.params->opcode) {
                case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
                    bt_mesh_free_buf(arg->sensor_client_get_state.get_state->column_get.raw_value_x);
                    break;
                case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
                    bt_mesh_free_buf(arg->sensor_client_get_state.get_state->series_get.raw_value_x1);
                    bt_mesh_free_buf(arg->sensor_client_get_state.get_state->series_get.raw_value_x2);
                    break;
                default:
                    break;
                }
            }
            bt_mesh_free(arg->sensor_client_get_state.get_state);
        }
        if (arg->sensor_client_get_state.params) {
            bt_mesh_free(arg->sensor_client_get_state.params);
        }
        break;
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_SET_STATE:
        if (arg->sensor_client_set_state.set_state) {
            if (arg->sensor_client_set_state.params) {
                switch (arg->sensor_client_set_state.params->opcode) {
                case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
                    bt_mesh_free_buf(arg->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_down);
                    bt_mesh_free_buf(arg->sensor_client_set_state.set_state->cadence_set.status_trigger_delta_up);
                    bt_mesh_free_buf(arg->sensor_client_set_state.set_state->cadence_set.fast_cadence_low);
                    bt_mesh_free_buf(arg->sensor_client_set_state.set_state->cadence_set.fast_cadence_high);
                    break;
                case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
                    bt_mesh_free_buf(arg->sensor_client_set_state.set_state->setting_set.sensor_setting_raw);
                    break;
                default:
                    break;
                }
            }
            bt_mesh_free(arg->sensor_client_set_state.set_state);
        }
        if (arg->sensor_client_set_state.params) {
            bt_mesh_free(arg->sensor_client_set_state.params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_sensor_client_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_sensor_client_cb_param_t *p_dest_data = (esp_ble_mesh_sensor_client_cb_param_t *)p_dest;
    esp_ble_mesh_sensor_client_cb_param_t *p_src_data = (esp_ble_mesh_sensor_client_cb_param_t *)p_src;
    uint16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    if (p_src_data->params) {
        p_dest_data->params = bt_mesh_calloc(sizeof(esp_ble_mesh_client_common_param_t));
        if (!p_dest_data->params) {
            BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
            return;
        }

        memcpy(p_dest_data->params, p_src_data->params, sizeof(esp_ble_mesh_client_common_param_t));
    }

    switch (msg->act) {
    case ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT:
        if (p_src_data->params) {
            switch (p_src_data->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS:
                if (p_src_data->status_cb.descriptor_status.descriptor) {
                    length = p_src_data->status_cb.descriptor_status.descriptor->len;
                    p_dest_data->status_cb.descriptor_status.descriptor = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.descriptor_status.descriptor) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.descriptor_status.descriptor,
                                           p_src_data->status_cb.descriptor_status.descriptor->data,
                                           p_src_data->status_cb.descriptor_status.descriptor->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS:
                if (p_src_data->status_cb.cadence_status.sensor_cadence_value) {
                    length = p_src_data->status_cb.cadence_status.sensor_cadence_value->len;
                    p_dest_data->status_cb.cadence_status.sensor_cadence_value = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.cadence_status.sensor_cadence_value) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.cadence_status.sensor_cadence_value,
                                           p_src_data->status_cb.cadence_status.sensor_cadence_value->data,
                                           p_src_data->status_cb.cadence_status.sensor_cadence_value->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS:
                if (p_src_data->status_cb.settings_status.sensor_setting_property_ids) {
                    length = p_src_data->status_cb.settings_status.sensor_setting_property_ids->len;
                    p_dest_data->status_cb.settings_status.sensor_setting_property_ids = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.settings_status.sensor_setting_property_ids) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.settings_status.sensor_setting_property_ids,
                                           p_src_data->status_cb.settings_status.sensor_setting_property_ids->data,
                                           p_src_data->status_cb.settings_status.sensor_setting_property_ids->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS:
                if (p_src_data->status_cb.setting_status.sensor_setting_raw) {
                    length = p_src_data->status_cb.setting_status.sensor_setting_raw->len;
                    p_dest_data->status_cb.setting_status.sensor_setting_raw = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.setting_status.sensor_setting_raw) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.setting_status.sensor_setting_raw,
                                           p_src_data->status_cb.setting_status.sensor_setting_raw->data,
                                           p_src_data->status_cb.setting_status.sensor_setting_raw->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS:
                if (p_src_data->status_cb.sensor_status.marshalled_sensor_data) {
                    length = p_src_data->status_cb.sensor_status.marshalled_sensor_data->len;
                    p_dest_data->status_cb.sensor_status.marshalled_sensor_data = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.sensor_status.marshalled_sensor_data) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.sensor_status.marshalled_sensor_data,
                                           p_src_data->status_cb.sensor_status.marshalled_sensor_data->data,
                                           p_src_data->status_cb.sensor_status.marshalled_sensor_data->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS:
                if (p_src_data->status_cb.column_status.sensor_column_value) {
                    length = p_src_data->status_cb.column_status.sensor_column_value->len;
                    p_dest_data->status_cb.column_status.sensor_column_value = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.column_status.sensor_column_value) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.column_status.sensor_column_value,
                                           p_src_data->status_cb.column_status.sensor_column_value->data,
                                           p_src_data->status_cb.column_status.sensor_column_value->len);
                }
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS:
                if (p_src_data->status_cb.series_status.sensor_series_value) {
                    length = p_src_data->status_cb.series_status.sensor_series_value->len;
                    p_dest_data->status_cb.series_status.sensor_series_value = bt_mesh_alloc_buf(length);
                    if (!p_dest_data->status_cb.series_status.sensor_series_value) {
                        BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                        /* Free the previously allocated resources */
                        bt_mesh_free(p_dest_data->params);
                        p_dest_data->params = NULL;
                        return;
                    }
                    net_buf_simple_add_mem(p_dest_data->status_cb.series_status.sensor_series_value,
                                           p_src_data->status_cb.series_status.sensor_series_value->data,
                                           p_src_data->status_cb.series_status.sensor_series_value->len);
                }
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT:
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_sensor_client_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_sensor_client_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_sensor_client_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT:
    case ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT:
    case ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT:
        if (arg->params) {
            switch (arg->params->opcode) {
            case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS:
                bt_mesh_free_buf(arg->status_cb.descriptor_status.descriptor);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS:
                bt_mesh_free_buf(arg->status_cb.cadence_status.sensor_cadence_value);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS:
                bt_mesh_free_buf(arg->status_cb.settings_status.sensor_setting_property_ids);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS:
                bt_mesh_free_buf(arg->status_cb.setting_status.sensor_setting_raw);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS:
                bt_mesh_free_buf(arg->status_cb.sensor_status.marshalled_sensor_data);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS:
                bt_mesh_free_buf(arg->status_cb.column_status.sensor_column_value);
                break;
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET:
            case ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS:
                bt_mesh_free_buf(arg->status_cb.series_status.sensor_series_value);
                break;
            default:
                break;
            }
        }
    case ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT:
        if (arg->params) {
            bt_mesh_free(arg->params);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_sensor_client_callback(esp_ble_mesh_sensor_client_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_SENSOR_CLIENT)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SENSOR_CLIENT;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_sensor_client_cb_param_t),
                         btc_ble_mesh_sensor_client_copy_req_data, btc_ble_mesh_sensor_client_free_req_data);
}

void bt_mesh_sensor_client_cb_evt_to_btc(uint32_t opcode, uint8_t evt_type,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         const uint8_t *val, size_t len)
{
    esp_ble_mesh_sensor_client_cb_param_t cb_params = {0};
    esp_ble_mesh_client_common_param_t params = {0};
    uint8_t act = 0U;

    if (!model || !ctx || len > sizeof(cb_params.status_cb)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_SENSOR_CLIENT_GET_STATE:
        act = ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_SENSOR_CLIENT_SET_STATE:
        act = ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT;
        break;
    case BTC_BLE_MESH_EVT_SENSOR_CLIENT_PUBLISH:
        act = ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT;
        break;
    case BTC_BLE_MESH_EVT_SENSOR_CLIENT_TIMEOUT:
        act = ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT;
        break;
    default:
        BT_ERR("Unknown Sensor client event type %d", evt_type);
        return;
    }

    params.opcode = opcode;
    params.model = (esp_ble_mesh_model_t *)model;
    params.ctx.net_idx = ctx->net_idx;
    params.ctx.app_idx = ctx->app_idx;
    params.ctx.addr = ctx->addr;
    params.ctx.recv_ttl = ctx->recv_ttl;
    params.ctx.recv_op = ctx->recv_op;
    params.ctx.recv_dst = ctx->recv_dst;
    params.ctx.recv_rssi = ctx->recv_rssi;
    params.ctx.send_ttl = ctx->send_ttl;

    cb_params.error_code = 0;
    cb_params.params = &params;

    if (val && len) {
        memcpy(&cb_params.status_cb, val, len);
    }

    btc_ble_mesh_sensor_client_callback(&cb_params, act);
}

void btc_ble_mesh_sensor_client_publish_callback(uint32_t opcode,
                                                 struct bt_mesh_model *model,
                                                 struct bt_mesh_msg_ctx *ctx,
                                                 struct net_buf_simple *buf)
{
    if (!model || !ctx || !buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    bt_mesh_sensor_client_cb_evt_to_btc(opcode, BTC_BLE_MESH_EVT_SENSOR_CLIENT_PUBLISH,
                                        model, ctx, buf->data, buf->len);
}

void btc_ble_mesh_sensor_client_call_handler(btc_msg_t *msg)
{
    esp_ble_mesh_sensor_client_cb_param_t cb = {0};
    btc_ble_mesh_sensor_client_args_t *arg = NULL;
    bt_mesh_client_common_param_t param = {0};

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (btc_ble_mesh_sensor_client_args_t *)(msg->arg);

    switch (msg->act) {
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_GET_STATE:
        btc_ble_mesh_set_client_common_param(arg->sensor_client_get_state.params, &param, false);

        cb.params = arg->sensor_client_get_state.params;
        cb.error_code = bt_mesh_sensor_client_get_state(&param, arg->sensor_client_get_state.get_state);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_sensor_client_callback(&cb, ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT);
        }
        break;
    case BTC_BLE_MESH_ACT_SENSOR_CLIENT_SET_STATE:
        btc_ble_mesh_set_client_common_param(arg->sensor_client_set_state.params, &param, false);

        cb.params = arg->sensor_client_set_state.params;
        cb.error_code = bt_mesh_sensor_client_set_state(&param, arg->sensor_client_set_state.set_state);
        if (cb.error_code) {
            /* If send failed, callback error_code to app layer immediately */
            btc_ble_mesh_sensor_client_callback(&cb, ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT);
        }
        break;
    default:
        break;
    }

    btc_ble_mesh_sensor_client_arg_deep_free(msg);
}

void btc_ble_mesh_sensor_client_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_sensor_client_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_sensor_client_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_SENSOR_CLIENT_EVT_MAX) {
        btc_ble_mesh_sensor_client_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_sensor_client_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_SENSOR_CLI */

#if CONFIG_BLE_MESH_SENSOR_SERVER

/* Sensor Server Models related functions */

static inline void btc_ble_mesh_sensor_server_cb_to_app(esp_ble_mesh_sensor_server_cb_event_t event,
                                                        esp_ble_mesh_sensor_server_cb_param_t *param)
{
    esp_ble_mesh_sensor_server_cb_t btc_ble_mesh_cb =
        (esp_ble_mesh_sensor_server_cb_t)btc_profile_cb_get(BTC_PID_SENSOR_SERVER);
    if (btc_ble_mesh_cb) {
        btc_ble_mesh_cb(event, param);
    }
}

static void btc_ble_mesh_sensor_server_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    esp_ble_mesh_sensor_server_cb_param_t *p_dest_data = (esp_ble_mesh_sensor_server_cb_param_t *)p_dest;
    esp_ble_mesh_sensor_server_cb_param_t *p_src_data = (esp_ble_mesh_sensor_server_cb_param_t *)p_src;
    uint16_t length = 0U;

    if (!msg || !p_src_data || !p_dest_data) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (msg->act) {
    case ESP_BLE_MESH_SENSOR_SERVER_STATE_CHANGE_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET ||
                p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK) {
            if (p_src_data->value.state_change.sensor_cadence_set.trigger_delta_down) {
                length = p_src_data->value.state_change.sensor_cadence_set.trigger_delta_down->len;
                p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_down = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_down == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_down,
                                       p_src_data->value.state_change.sensor_cadence_set.trigger_delta_down->data,
                                       p_src_data->value.state_change.sensor_cadence_set.trigger_delta_down->len);
            }
            if (p_src_data->value.state_change.sensor_cadence_set.trigger_delta_up) {
                length = p_src_data->value.state_change.sensor_cadence_set.trigger_delta_up->len;
                p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_up = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_up == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.sensor_cadence_set.trigger_delta_up,
                                       p_src_data->value.state_change.sensor_cadence_set.trigger_delta_up->data,
                                       p_src_data->value.state_change.sensor_cadence_set.trigger_delta_up->len);
            }
            if (p_src_data->value.state_change.sensor_cadence_set.fast_cadence_low) {
                length = p_src_data->value.state_change.sensor_cadence_set.fast_cadence_low->len;
                p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_low = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_low == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_low,
                                       p_src_data->value.state_change.sensor_cadence_set.fast_cadence_low->data,
                                       p_src_data->value.state_change.sensor_cadence_set.fast_cadence_low->len);
            }
            if (p_src_data->value.state_change.sensor_cadence_set.fast_cadence_high) {
                length = p_src_data->value.state_change.sensor_cadence_set.fast_cadence_high->len;
                p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_high = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_high == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.sensor_cadence_set.fast_cadence_high,
                                       p_src_data->value.state_change.sensor_cadence_set.fast_cadence_high->data,
                                       p_src_data->value.state_change.sensor_cadence_set.fast_cadence_high->len);
            }
        } else if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET ||
                   p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK) {
            if (p_src_data->value.state_change.sensor_setting_set.setting_value) {
                length = p_src_data->value.state_change.sensor_setting_set.setting_value->len;
                p_dest_data->value.state_change.sensor_setting_set.setting_value = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.state_change.sensor_setting_set.setting_value == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.state_change.sensor_setting_set.setting_value,
                                       p_src_data->value.state_change.sensor_setting_set.setting_value->data,
                                       p_src_data->value.state_change.sensor_setting_set.setting_value->len);
            }
        }
        break;
    case ESP_BLE_MESH_SENSOR_SERVER_RECV_GET_MSG_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET) {
            if (p_src_data->value.get.sensor_column.raw_value_x) {
                length = p_src_data->value.get.sensor_column.raw_value_x->len;
                p_dest_data->value.get.sensor_column.raw_value_x = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.get.sensor_column.raw_value_x == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.get.sensor_column.raw_value_x,
                                       p_src_data->value.get.sensor_column.raw_value_x->data,
                                       p_src_data->value.get.sensor_column.raw_value_x->len);
            }
        } else if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET) {
            if (p_src_data->value.get.sensor_series.raw_value) {
                length = p_src_data->value.get.sensor_series.raw_value->len;
                p_dest_data->value.get.sensor_series.raw_value = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.get.sensor_series.raw_value == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.get.sensor_series.raw_value,
                                       p_src_data->value.get.sensor_series.raw_value->data,
                                       p_src_data->value.get.sensor_series.raw_value->len);
            }
        }
        break;
    case ESP_BLE_MESH_SENSOR_SERVER_RECV_SET_MSG_EVT:
        if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET ||
                p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK) {
            if (p_src_data->value.set.sensor_cadence.cadence) {
                length = p_src_data->value.set.sensor_cadence.cadence->len;
                p_dest_data->value.set.sensor_cadence.cadence = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.set.sensor_cadence.cadence == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.set.sensor_cadence.cadence,
                                       p_src_data->value.set.sensor_cadence.cadence->data,
                                       p_src_data->value.set.sensor_cadence.cadence->len);
            }
        } else if (p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET ||
                   p_src_data->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK) {
            if (p_src_data->value.set.sensor_setting.setting_raw) {
                length = p_src_data->value.set.sensor_setting.setting_raw->len;
                p_dest_data->value.set.sensor_setting.setting_raw = bt_mesh_alloc_buf(length);
                if (p_dest_data->value.set.sensor_setting.setting_raw == NULL) {
                    BT_ERR("%s, Out of memory, act %d", __func__, msg->act);
                    return;
                }
                net_buf_simple_add_mem(p_dest_data->value.set.sensor_setting.setting_raw,
                                       p_src_data->value.set.sensor_setting.setting_raw->data,
                                       p_src_data->value.set.sensor_setting.setting_raw->len);
            }
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_sensor_server_free_req_data(btc_msg_t *msg)
{
    esp_ble_mesh_sensor_server_cb_param_t *arg = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    arg = (esp_ble_mesh_sensor_server_cb_param_t *)(msg->arg);

    switch (msg->act) {
    case ESP_BLE_MESH_SENSOR_SERVER_STATE_CHANGE_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET ||
            arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK) {
            bt_mesh_free_buf(arg->value.state_change.sensor_cadence_set.trigger_delta_down);
            bt_mesh_free_buf(arg->value.state_change.sensor_cadence_set.trigger_delta_up);
            bt_mesh_free_buf(arg->value.state_change.sensor_cadence_set.fast_cadence_low);
            bt_mesh_free_buf(arg->value.state_change.sensor_cadence_set.fast_cadence_high);
        } else if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET ||
                   arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK) {
            bt_mesh_free_buf(arg->value.state_change.sensor_setting_set.setting_value);
        }
        break;
    case ESP_BLE_MESH_SENSOR_SERVER_RECV_GET_MSG_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET) {
            bt_mesh_free_buf(arg->value.get.sensor_column.raw_value_x);
        } else if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET) {
            bt_mesh_free_buf(arg->value.get.sensor_series.raw_value);
        }
        break;
    case ESP_BLE_MESH_SENSOR_SERVER_RECV_SET_MSG_EVT:
        if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET ||
            arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK) {
            bt_mesh_free_buf(arg->value.set.sensor_cadence.cadence);
        } else if (arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET ||
                   arg->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK) {
            bt_mesh_free_buf(arg->value.set.sensor_setting.setting_raw);
        }
        break;
    default:
        break;
    }
}

static void btc_ble_mesh_sensor_server_callback(esp_ble_mesh_sensor_server_cb_param_t *cb_params, uint8_t act)
{
    btc_msg_t msg = {0};

    /* If corresponding callback is not registered, event will not be posted. */
    if (!btc_profile_cb_get(BTC_PID_SENSOR_SERVER)) {
        return;
    }

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SENSOR_SERVER;
    msg.act = act;

    btc_transfer_context(&msg, cb_params, cb_params == NULL ? 0 : sizeof(esp_ble_mesh_sensor_server_cb_param_t),
                         btc_ble_mesh_sensor_server_copy_req_data, btc_ble_mesh_sensor_server_free_req_data);
}

void bt_mesh_sensor_server_cb_evt_to_btc(uint8_t evt_type,
                                         struct bt_mesh_model *model,
                                         struct bt_mesh_msg_ctx *ctx,
                                         const uint8_t *val, size_t len)
{
    esp_ble_mesh_sensor_server_cb_param_t cb_params = {0};
    uint8_t act = 0U;

    if (model == NULL || ctx == NULL || len > sizeof(cb_params.value)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    switch (evt_type) {
    case BTC_BLE_MESH_EVT_SENSOR_SERVER_STATE_CHANGE:
        act = ESP_BLE_MESH_SENSOR_SERVER_STATE_CHANGE_EVT;
        break;
    case BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_GET_MSG:
        act = ESP_BLE_MESH_SENSOR_SERVER_RECV_GET_MSG_EVT;
        break;
    case BTC_BLE_MESH_EVT_SENSOR_SERVER_RECV_SET_MSG:
        act = ESP_BLE_MESH_SENSOR_SERVER_RECV_SET_MSG_EVT;
        break;
    default:
        BT_ERR("Unknown Sensor server event type %d", evt_type);
        return;
    }

    cb_params.model = (esp_ble_mesh_model_t *)model;
    cb_params.ctx.net_idx = ctx->net_idx;
    cb_params.ctx.app_idx = ctx->app_idx;
    cb_params.ctx.addr = ctx->addr;
    cb_params.ctx.recv_ttl = ctx->recv_ttl;
    cb_params.ctx.recv_op = ctx->recv_op;
    cb_params.ctx.recv_dst = ctx->recv_dst;
    cb_params.ctx.recv_rssi = ctx->recv_rssi;
    cb_params.ctx.send_ttl = ctx->send_ttl;

    if (val && len) {
        memcpy(&cb_params.value, val, len);
    }

    btc_ble_mesh_sensor_server_callback(&cb_params, act);
}

void btc_ble_mesh_sensor_server_cb_handler(btc_msg_t *msg)
{
    esp_ble_mesh_sensor_server_cb_param_t *param = NULL;

    if (!msg) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    param = (esp_ble_mesh_sensor_server_cb_param_t *)(msg->arg);

    if (msg->act < ESP_BLE_MESH_SENSOR_SERVER_EVT_MAX) {
        btc_ble_mesh_sensor_server_cb_to_app(msg->act, param);
    } else {
        BT_ERR("%s, Unknown act %d", __func__, msg->act);
    }

    btc_ble_mesh_sensor_server_free_req_data(msg);
}

#endif /* CONFIG_BLE_MESH_SENSOR_SERVER */
