/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_err.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "cmd_decl.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "board.h"
#include "ble_mesh_fast_prov_operation.h"
#include "ble_mesh_fast_prov_client_model.h"
#include "ble_mesh_fast_prov_server_model.h"
#include "ble_mesh_example_init.h"

#define TAG "EXAMPLE"

extern struct _led_state led_state[3];
extern struct k_delayed_work send_self_prov_node_addr_timer;
extern bt_mesh_atomic_t fast_prov_cli_flags;

static uint8_t dev_uuid[16] = { 0xdd, 0xdd };
static uint8_t prov_start_num = 0;
static bool prov_start = false;

static const esp_ble_mesh_client_op_pair_t fast_prov_cli_op_pair[] = {
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,      ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS      },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,   ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS   },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR,     ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK    },
    { ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET, ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS },
};

/* Configuration Client Model user_data */
esp_ble_mesh_client_t config_client;

/* Configuration Server Model user_data */
esp_ble_mesh_cfg_srv_t config_server = {
    /* 3 transmissions with 20ms interval */
    .net_transmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .relay = ESP_BLE_MESH_RELAY_ENABLED,
    .relay_retransmit = ESP_BLE_MESH_TRANSMIT(2, 20),
    .beacon = ESP_BLE_MESH_BEACON_DISABLED,
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
#if defined(CONFIG_BLE_MESH_FRIEND)
    .friend_state = ESP_BLE_MESH_FRIEND_ENABLED,
#else
    .friend_state = ESP_BLE_MESH_FRIEND_NOT_SUPPORTED,
#endif
    .default_ttl = 7,
};

/* Fast Prov Client Model user_data */
esp_ble_mesh_client_t fast_prov_client = {
    .op_pair_size = ARRAY_SIZE(fast_prov_cli_op_pair),
    .op_pair = fast_prov_cli_op_pair,
};

/* Fast Prov Server Model user_data */
example_fast_prov_server_t fast_prov_server = {
    .primary_role  = false,
    .max_node_num  = 6,
    .prov_node_cnt = 0x0,
    .app_idx       = ESP_BLE_MESH_KEY_UNUSED,
    .unicast_min   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_max   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_cur   = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .unicast_step  = 0x0,
    .flags         = 0x0,
    .iv_index      = 0x0,
    .net_idx       = ESP_BLE_MESH_KEY_UNUSED,
    .group_addr    = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .prim_prov_addr = ESP_BLE_MESH_ADDR_UNASSIGNED,
    .match_len     = 0x0,
    .pend_act      = FAST_PROV_ACT_NONE,
    .state         = STATE_IDLE,
};

ESP_BLE_MESH_MODEL_PUB_DEFINE(onoff_pub, 2 + 3, ROLE_FAST_PROV);
static esp_ble_mesh_gen_onoff_srv_t onoff_server = {
    .rsp_ctrl.get_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
    .rsp_ctrl.set_auto_rsp = ESP_BLE_MESH_SERVER_AUTO_RSP,
};

static esp_ble_mesh_model_op_t fast_prov_srv_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET,          3),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD,      16),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR,         2),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET,     0),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_ADD,    2),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_DELETE, 2),
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_op_t fast_prov_cli_op[] = {
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS,    1),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS, 2),
    ESP_BLE_MESH_MODEL_OP(ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK,  0),
    ESP_BLE_MESH_MODEL_OP_END,
};

static esp_ble_mesh_model_t root_models[] = {
    ESP_BLE_MESH_MODEL_CFG_SRV(&config_server),
    ESP_BLE_MESH_MODEL_CFG_CLI(&config_client),
    ESP_BLE_MESH_MODEL_GEN_ONOFF_SRV(&onoff_pub, &onoff_server),
};

static esp_ble_mesh_model_t vnd_models[] = {
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV,
                              fast_prov_srv_op, NULL, &fast_prov_server),
    ESP_BLE_MESH_VENDOR_MODEL(CID_ESP, ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI,
                              fast_prov_cli_op, NULL, &fast_prov_client),
};

static esp_ble_mesh_elem_t elements[] = {
    ESP_BLE_MESH_ELEMENT(0, root_models, vnd_models),
};

static esp_ble_mesh_comp_t comp = {
    .cid           = CID_ESP,
    .elements      = elements,
    .element_count = ARRAY_SIZE(elements),
};

static esp_ble_mesh_prov_t prov = {
    .uuid                = dev_uuid,
    .output_size         = 0,
    .output_actions      = 0,
    .prov_attention      = 0x00,
    .prov_algorithm      = 0x00,
    .prov_pub_key_oob    = 0x00,
    .prov_static_oob_val = NULL,
    .prov_static_oob_len = 0x00,
    .flags               = 0x00,
    .iv_index            = 0x00,
};

static void example_change_led_state(uint8_t onoff)
{
    struct _led_state *led = &led_state[1];

    board_led_operation(led->pin, onoff);

    /* When the node receives the first Generic OnOff Get/Set/Set Unack message, it will
     * start the timer used to disable fast provisioning functionality.
     */
#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
    if (!bt_mesh_atomic_test_and_set_bit(fast_prov_server.srv_flags, DISABLE_FAST_PROV_START)) {
        k_delayed_work_submit(&fast_prov_server.disable_fast_prov_timer, DISABLE_FAST_PROV_TIMEOUT);
    }
#pragma GCC diagnostic pop
}

static void node_prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    ESP_LOGI(TAG, "net_idx: 0x%04x, unicast_addr: 0x%04x", net_idx, addr);
    ESP_LOGI(TAG, "flags: 0x%02x, iv_index: 0x%08" PRIx32, flags, iv_index);
    board_prov_complete();
    /* Updates the net_idx used by Fast Prov Server model, and it can also
     * be updated if the Fast Prov Info Set message contains a valid one.
     */
    fast_prov_server.net_idx = net_idx;
}

static void provisioner_prov_link_open(esp_ble_mesh_prov_bearer_t bearer)
{
    ESP_LOGI(TAG, "%s: bearer %s", __func__, bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
}

static void provisioner_prov_link_close(esp_ble_mesh_prov_bearer_t bearer, uint8_t reason)
{
    ESP_LOGI(TAG, "%s: bearer %s, reason 0x%02x", __func__,
             bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT", reason);
    if (prov_start_num) {
        prov_start_num--;
    }
}

static void provisioner_prov_complete(int node_idx, const uint8_t uuid[16], uint16_t unicast_addr,
                                      uint8_t element_num, uint16_t net_idx)
{
    example_node_info_t *node = NULL;
    esp_err_t err;

    if (example_is_node_exist(uuid) == false) {
        fast_prov_server.prov_node_cnt++;
    }

    ESP_LOG_BUFFER_HEX("Device uuid", uuid + 2, 6);
    ESP_LOGI(TAG, "Unicast address 0x%04x", unicast_addr);

    /* Sets node info */
    err = example_store_node_info(uuid, unicast_addr, element_num, net_idx,
                                  fast_prov_server.app_idx, LED_OFF);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to set node info", __func__);
        return;
    }

    /* Gets node info */
    node = example_get_node_info(unicast_addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    if (fast_prov_server.primary_role == true) {
        /* If the Provisioner is the primary one (i.e. provisioned by the phone), it shall
         * store self-provisioned node addresses;
         * If the node_addr_cnt configured by the phone is small than or equal to the
         * maximum number of nodes it can provision, it shall reset the timer which is used
         * to send all node addresses to the phone.
         */
        err = example_store_remote_node_address(unicast_addr);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to store node address 0x%04x", __func__, unicast_addr);
            return;
        }
#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
        if (fast_prov_server.node_addr_cnt != FAST_PROV_NODE_COUNT_MIN &&
                fast_prov_server.node_addr_cnt <= fast_prov_server.max_node_num) {
            if (bt_mesh_atomic_test_and_clear_bit(fast_prov_server.srv_flags, GATT_PROXY_ENABLE_START)) {
                k_delayed_work_cancel(&fast_prov_server.gatt_proxy_enable_timer);
            }
            if (!bt_mesh_atomic_test_and_set_bit(fast_prov_server.srv_flags, GATT_PROXY_ENABLE_START)) {
                k_delayed_work_submit(&fast_prov_server.gatt_proxy_enable_timer, GATT_PROXY_ENABLE_TIMEOUT);
            }
        }
#pragma GCC diagnostic pop
    } else {
        /* When a device is provisioned, the non-primary Provisioner shall reset the timer
         * which is used to send node addresses to the primary Provisioner.
         */
        if (bt_mesh_atomic_test_and_clear_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START)) {
            k_delayed_work_cancel(&send_self_prov_node_addr_timer);
        }
        if (!bt_mesh_atomic_test_and_set_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START)) {
            k_delayed_work_submit(&send_self_prov_node_addr_timer, SEND_SELF_PROV_NODE_ADDR_TIMEOUT);
        }
    }

#pragma GCC diagnostic push
#if     __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif
    if (bt_mesh_atomic_test_bit(fast_prov_server.srv_flags, DISABLE_FAST_PROV_START)) {
        /* When a device is provisioned, and the stop_prov flag of the Provisioner has been
         * set, the Provisioner shall reset the timer which is used to stop the provisioner
         * functionality.
         */
        k_delayed_work_cancel(&fast_prov_server.disable_fast_prov_timer);
        k_delayed_work_submit(&fast_prov_server.disable_fast_prov_timer, DISABLE_FAST_PROV_TIMEOUT);
    }
#pragma GCC diagnostic pop

    /* The Provisioner will send Config AppKey Add to the node. */
    example_msg_common_info_t info = {
        .net_idx = node->net_idx,
        .app_idx = node->app_idx,
        .dst = node->unicast_addr,
        .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        .role = ROLE_FAST_PROV,
#endif
    };
    err = example_send_config_appkey_add(config_client.model, &info, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Config AppKey Add message", __func__);
        return;
    }
}

static void example_recv_unprov_adv_pkt(uint8_t dev_uuid[16], uint8_t addr[BLE_MESH_ADDR_LEN],
                                        esp_ble_mesh_addr_type_t addr_type, uint16_t oob_info,
                                        uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer)
{
    esp_ble_mesh_unprov_dev_add_t add_dev = {0};
    esp_ble_mesh_dev_add_flag_t flag;
    esp_err_t err;

    /* In Fast Provisioning, the Provisioner should only use PB-ADV to provision devices. */
    if (prov_start && (bearer & ESP_BLE_MESH_PROV_ADV)) {
        /* Checks if the device is a reprovisioned one. */
        if (example_is_node_exist(dev_uuid) == false) {
            if ((prov_start_num >= fast_prov_server.max_node_num) ||
                    (fast_prov_server.prov_node_cnt >= fast_prov_server.max_node_num)) {
                return;
            }
        }

        add_dev.addr_type = (esp_ble_mesh_addr_type_t)addr_type;
        add_dev.oob_info = oob_info;
        add_dev.bearer = (esp_ble_mesh_prov_bearer_t)bearer;
        memcpy(add_dev.uuid, dev_uuid, 16);
        memcpy(add_dev.addr, addr, BLE_MESH_ADDR_LEN);
        flag = ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_START_PROV_NOW_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG;
        err = esp_ble_mesh_provisioner_add_unprov_dev(&add_dev, flag);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to start provisioning device", __func__);
            return;
        }

        /* If adding unprovisioned device successfully, increase prov_start_num */
        prov_start_num++;
    }

    return;
}

static void example_ble_mesh_provisioning_cb(esp_ble_mesh_prov_cb_event_t event,
        esp_ble_mesh_prov_cb_param_t *param)
{
    esp_err_t err;

    switch (event) {
    case ESP_BLE_MESH_PROV_REGISTER_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROV_REGISTER_COMP_EVT, err_code: %d",
                 param->prov_register_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT, err_code: %d",
                 param->node_prov_enable_comp.err_code);
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT, bearer: %s",
                 param->node_prov_link_open.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT, bearer: %s",
                 param->node_prov_link_close.bearer == ESP_BLE_MESH_PROV_ADV ? "PB-ADV" : "PB-GATT");
        break;
    case ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT");
        node_prov_complete(param->node_prov_complete.net_idx, param->node_prov_complete.addr,
                           param->node_prov_complete.flags, param->node_prov_complete.iv_index);
        break;
    case ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT");
        if (fast_prov_server.primary_role == true) {
            config_server.relay = ESP_BLE_MESH_RELAY_DISABLED;
        }
        prov_start = true;
        break;
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        example_recv_unprov_adv_pkt(param->provisioner_recv_unprov_adv_pkt.dev_uuid, param->provisioner_recv_unprov_adv_pkt.addr,
                                    param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
                                    param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT");
        provisioner_prov_link_open(param->provisioner_prov_link_open.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT");
        provisioner_prov_link_close(param->provisioner_prov_link_close.bearer,
                                    param->provisioner_prov_link_close.reason);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT");
        provisioner_prov_complete(param->provisioner_prov_complete.node_idx,
                                  param->provisioner_prov_complete.device_uuid,
                                  param->provisioner_prov_complete.unicast_addr,
                                  param->provisioner_prov_complete.element_num,
                                  param->provisioner_prov_complete.netkey_idx);
        break;
    case ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT, err_code: %d",
                 param->provisioner_add_unprov_dev_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT, err_code: %d",
                 param->provisioner_set_dev_uuid_match_comp.err_code);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT, err_code: %d",
                 param->provisioner_set_node_name_comp.err_code);
        break;
    case ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT");
        ESP_LOGI(TAG, "status_unicast: 0x%02x, status_net_idx: 0x%02x, status_match 0x%02x",
                 param->set_fast_prov_info_comp.status_unicast,
                 param->set_fast_prov_info_comp.status_net_idx,
                 param->set_fast_prov_info_comp.status_match);
        err = example_handle_fast_prov_info_set_comp_evt(fast_prov_server.model,
                param->set_fast_prov_info_comp.status_unicast,
                param->set_fast_prov_info_comp.status_net_idx,
                param->set_fast_prov_info_comp.status_match);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to handle Fast Prov Info Set complete event", __func__);
            return;
        }
        break;
    }
    case ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT, status_action 0x%02x",
                 param->set_fast_prov_action_comp.status_action);
        err = example_handle_fast_prov_action_set_comp_evt(fast_prov_server.model,
                param->set_fast_prov_action_comp.status_action);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to handle Fast Prov Action Set complete event", __func__);
            return;
        }
        break;
    default:
        break;
    }

    return;
}

static void example_ble_mesh_custom_model_cb(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param)
{
    uint32_t opcode;
    esp_err_t err;

    switch (event) {
    case ESP_BLE_MESH_MODEL_OPERATION_EVT: {
        if (!param->model_operation.model || !param->model_operation.model->op ||
                !param->model_operation.ctx) {
            ESP_LOGE(TAG, "%s: model_operation parameter is NULL", __func__);
            return;
        }
        opcode = param->model_operation.opcode;
        switch (opcode) {
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_ADD:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_DELETE: {
            ESP_LOGI(TAG, "%s: Fast prov server receives msg, opcode 0x%04" PRIx32, __func__, opcode);
            struct net_buf_simple buf = {
                .data = param->model_operation.msg,
                .len = param->model_operation.length,
            };
            err = example_fast_prov_server_recv_msg(param->model_operation.model,
                                                    param->model_operation.ctx, &buf);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to handle fast prov client message", __func__);
                return;
            }
            break;
        }
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK: {
            ESP_LOGI(TAG, "%s: Fast prov client receives msg, opcode 0x%04" PRIx32, __func__, opcode);
            err = example_fast_prov_client_recv_status(param->model_operation.model,
                    param->model_operation.ctx,
                    param->model_operation.length,
                    param->model_operation.msg);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to handle fast prov server message", __func__);
                return;
            }
            break;
        }
        default:
            ESP_LOGI(TAG, "%s: opcode 0x%04" PRIx32, __func__, param->model_operation.opcode);
            break;
        }
        break;
    }
    case ESP_BLE_MESH_MODEL_SEND_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_SEND_COMP_EVT, err_code %d", param->model_send_comp.err_code);
        switch (param->model_send_comp.opcode) {
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK:
        case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS:
            err = example_handle_fast_prov_status_send_comp_evt(param->model_send_comp.err_code,
                    param->model_send_comp.opcode,
                    param->model_send_comp.model,
                    param->model_send_comp.ctx);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to handle fast prov status send complete event", __func__);
                return;
            }
            break;
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT, err_code %d",
                 param->model_publish_comp.err_code);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_MODEL_CLIENT_RECV_PUBLISH_MSG_EVT, opcode 0x%04" PRIx32,
                 param->client_recv_publish_msg.opcode);
        break;
    case ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT, opcode 0x%04" PRIx32 ", dst 0x%04x",
                 param->client_send_timeout.opcode, param->client_send_timeout.ctx->addr);
        err = example_fast_prov_client_recv_timeout(param->client_send_timeout.opcode,
                param->client_send_timeout.model,
                param->client_send_timeout.ctx);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to resend fast prov client message", __func__);
            return;
        }
        break;
    default:
        break;
    }
}

static void example_ble_mesh_config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
        esp_ble_mesh_cfg_client_cb_param_t *param)
{
    example_node_info_t *node = NULL;
    uint32_t opcode;
    uint16_t address;
    esp_err_t err;

    ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x",
             __func__, param->error_code, event, param->params->ctx.addr);

    opcode = param->params->opcode;
    address = param->params->ctx.addr;

    node = example_get_node_info(address);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    if (param->error_code) {
        ESP_LOGE(TAG, "Failed to send config client message, opcode: 0x%04" PRIx32, opcode);
        return;
    }

    switch (event) {
    case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:
        break;
    case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            example_fast_prov_info_set_t set = {0};
            if (node->reprov == false) {
                /* After sending Config AppKey Add successfully, start to send Fast Prov Info Set */
                if (fast_prov_server.unicast_cur >= fast_prov_server.unicast_max) {
                    /* TODO:
                     * 1. If unicast_cur is >= unicast_max, we can also send the message to enable
                     * the Provisioner functionality on the node, and need to add another vendor
                     * message used by the node to require a new unicast address range from primary
                     * Provisioner, and before get the correct response, the node should pend
                     * the fast provisioning functionality.
                     * 2. Currently if address is not enough, the Provisioner will only add the group
                     * address to the node.
                     */
                    ESP_LOGW(TAG, "%s: Not enough address to be assigned", __func__);
                    node->lack_of_addr = true;
                } else {
                    /* Send fast_prov_info_set message to node */
                    node->lack_of_addr = false;
                    node->unicast_min = fast_prov_server.unicast_cur;
                    if (fast_prov_server.unicast_cur + fast_prov_server.unicast_step >= fast_prov_server.unicast_max) {
                        node->unicast_max = fast_prov_server.unicast_max;
                    } else {
                        node->unicast_max = fast_prov_server.unicast_cur + fast_prov_server.unicast_step;
                    }
                    node->flags      = fast_prov_server.flags;
                    node->iv_index   = fast_prov_server.iv_index;
                    node->fp_net_idx = fast_prov_server.net_idx;
                    node->group_addr = fast_prov_server.group_addr;
                    node->prov_addr  = fast_prov_server.prim_prov_addr;
                    node->match_len  = fast_prov_server.match_len;
                    memcpy(node->match_val, fast_prov_server.match_val, fast_prov_server.match_len);
                    node->action = FAST_PROV_ACT_ENTER;
                    fast_prov_server.unicast_cur = node->unicast_max + 1;
                }
            }
            if (node->lack_of_addr == false) {
                set.ctx_flags = 0x03FE;
                memcpy(&set.unicast_min, &node->unicast_min,
                       sizeof(example_node_info_t) - offsetof(example_node_info_t, unicast_min));
            } else {
                set.ctx_flags  = BIT(6);
                set.group_addr = fast_prov_server.group_addr;
            }
            example_msg_common_info_t info = {
                .net_idx = node->net_idx,
                .app_idx = node->app_idx,
                .dst = node->unicast_addr,
                .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
                .role = ROLE_FAST_PROV,
#endif
            };
            err = example_send_fast_prov_info_set(fast_prov_client.model, &info, &set);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to send Fast Prov Info Set message", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
        break;
    case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:
        switch (opcode) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD: {
            example_msg_common_info_t info = {
                .net_idx = node->net_idx,
                .app_idx = node->app_idx,
                .dst = node->unicast_addr,
                .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
                .role = ROLE_FAST_PROV,
#endif
            };
            err = example_send_config_appkey_add(config_client.model, &info, NULL);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to send Config AppKey Add message", __func__);
                return;
            }
            break;
        }
        default:
            break;
        }
        break;
    default:
        return;
    }
}

static void example_ble_mesh_config_server_cb(esp_ble_mesh_cfg_server_cb_event_t event,
        esp_ble_mesh_cfg_server_cb_param_t *param)
{
    esp_err_t err;

    ESP_LOGI(TAG, "%s, event = 0x%02x, opcode = 0x%04" PRIx32 ", addr: 0x%04x",
             __func__, event, param->ctx.recv_op, param->ctx.addr);

    switch (event) {
    case ESP_BLE_MESH_CFG_SERVER_STATE_CHANGE_EVT:
        switch (param->ctx.recv_op) {
        case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            ESP_LOGI(TAG, "Config Server get Config AppKey Add");
            err = example_handle_config_app_key_add_evt(param->value.state_change.appkey_add.app_idx);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to bind app_idx 0x%04x with non-config models",
                         __func__, param->value.state_change.appkey_add.app_idx);
                return;
            }
            break;
        default:
            break;
        }
        break;
    default:
        return;
    }
}

static void example_ble_mesh_generic_server_cb(esp_ble_mesh_generic_server_cb_event_t event,
        esp_ble_mesh_generic_server_cb_param_t *param)
{
    ESP_LOGI(TAG, "event 0x%02x, opcode 0x%04" PRIx32 ", src 0x%04x, dst 0x%04x",
             event, param->ctx.recv_op, param->ctx.addr, param->ctx.recv_dst);

    switch (event) {
    case ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT:
        ESP_LOGI(TAG, "ESP_BLE_MESH_GENERIC_SERVER_STATE_CHANGE_EVT");
        if (param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET ||
                param->ctx.recv_op == ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK) {
            ESP_LOGI(TAG, "onoff 0x%02x", param->value.state_change.onoff_set.onoff);
            example_change_led_state(param->value.state_change.onoff_set.onoff);
        }
        break;
    default:
        ESP_LOGW(TAG, "Unknown Generic Server event 0x%02x", event);
        break;
    }
}

static esp_err_t ble_mesh_init(void)
{
    esp_err_t err;

    esp_ble_mesh_register_prov_callback(example_ble_mesh_provisioning_cb);
    esp_ble_mesh_register_custom_model_callback(example_ble_mesh_custom_model_cb);
    esp_ble_mesh_register_config_client_callback(example_ble_mesh_config_client_cb);
    esp_ble_mesh_register_config_server_callback(example_ble_mesh_config_server_cb);
    esp_ble_mesh_register_generic_server_callback(example_ble_mesh_generic_server_cb);

    err = esp_ble_mesh_init(&prov, &comp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize BLE Mesh", __func__);
        return err;
    }

    err = example_fast_prov_server_init(&vnd_models[0]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize fast prov server model", __func__);
        return err;
    }

    err = esp_ble_mesh_client_model_init(&vnd_models[1]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to initialize fast prov client model", __func__);
        return err;
    }

    k_delayed_work_init(&send_self_prov_node_addr_timer, example_send_self_prov_node_addr);

    err = esp_ble_mesh_node_prov_enable(ESP_BLE_MESH_PROV_ADV | ESP_BLE_MESH_PROV_GATT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to enable node provisioning", __func__);
        return err;
    }

    ESP_LOGI(TAG, "BLE Mesh Wi-Fi Coexist Node initialized");

    board_led_operation(LED_B, LED_ON);

    return ESP_OK;
}

#define WIFI_CONNECTED_BIT BIT0

static void wifi_console_init(void)
{
    initialise_wifi();

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    // init console REPL environment
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* Register commands */
    register_wifi();

    printf("\n ==================================================\n");
    printf(" |       Steps to test WiFi throughput            |\n");
    printf(" |                                                |\n");
    printf(" |  1. Print 'help' to gain overview of commands  |\n");
    printf(" |  2. Configure device to station or soft-AP     |\n");
    printf(" |  3. Setup WiFi connection                      |\n");
    printf(" |  4. Run iperf to test UDP/TCP RX/TX throughput |\n");
    printf(" |                                                |\n");
    printf(" =================================================\n\n");

    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void app_main(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing...");

    err = board_init();
    if (err) {
        ESP_LOGE(TAG, "board_init failed (err %d)", err);
        return;
    }

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = bluetooth_init();
    if (err) {
        ESP_LOGE(TAG, "esp32_bluetooth_init failed (err %d)", err);
        return;
    }

    ble_mesh_get_dev_uuid(dev_uuid);

    /* Initialize the Bluetooth Mesh Subsystem */
    err = ble_mesh_init();
    if (err) {
        ESP_LOGE(TAG, "Bluetooth mesh init failed (err %d)", err);
        return;
    }

    wifi_console_init();
}
