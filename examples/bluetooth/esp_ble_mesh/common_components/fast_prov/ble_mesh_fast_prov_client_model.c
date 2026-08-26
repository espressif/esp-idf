/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_local_data_operation_api.h"

#include "ble_mesh_fast_prov_operation.h"
#include "ble_mesh_fast_prov_client_model.h"
#include "ble_mesh_fast_prov_server_model.h"

#define TAG "FAST_PROV_CLIENT"

/* Note: these variables are used by ble_mesh_fast_prov_client demo */

#define GET_ALL_NODE_ADDR_TIMEOUT   K_SECONDS(60)

/* Timer used to send Fast Prov All Node Addr Get message */
#if !defined(CONFIG_BLE_MESH_FAST_PROV)
static struct k_delayed_work    get_all_node_addr_timer;
#endif

#if !defined(CONFIG_BLE_MESH_FAST_PROV)
/* Unicast address of the Primary Provisioner */
static uint16_t prim_prov_addr;
#endif

/* Note: these variables are used by ble_mesh_fast_prov_server demo */

/* Send 4 node addresses (8 octets) most each time to prevent segmentation */
#define NODE_ADDR_SEND_MAX_LEN      8

/* Timer used to send self-provisioned node addresses to Primary Provisioner */
struct k_delayed_work   send_self_prov_node_addr_timer;
bt_mesh_atomic_t fast_prov_cli_flags;

/* Self-provisioned node addresses that are being sent */
typedef struct {
    struct net_buf_simple *addr;    /* Unicast addresses of self-provisioned nodes being sent */
    bool send_succeed;  /* Indicate if sent operation is successful */
    bool ack_received;  /* Indicate if sent address has been acked */
} example_send_prov_node_addr_t;

NET_BUF_SIMPLE_DEFINE_STATIC(send_addr, NODE_ADDR_SEND_MAX_LEN);
static example_send_prov_node_addr_t node_addr_send = {
    .addr = &send_addr,
    .send_succeed = true,
    .ack_received = true,
};

/* Self-provisioned node addresses that have been sent successfully */
uint16_t addr_already_sent[CONFIG_BLE_MESH_MAX_PROV_NODES];

static example_fast_prov_server_t *get_fast_prov_srv_user_data(void)
{
    esp_ble_mesh_model_t *model = NULL;

    model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                               ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV, CID_ESP);
    if (!model) {
        ESP_LOGE(TAG, "%s: Failed to get config server model", __func__);
        return NULL;
    }

    return (example_fast_prov_server_t *)(model->user_data);
}

/* Timeout handler for send_self_prov_node_addr_timer */
void example_send_self_prov_node_addr(struct k_work *work)
{
    example_fast_prov_server_t *fast_prov_srv = NULL;
    esp_ble_mesh_model_t *model = NULL;
    int i, j, err;

    bt_mesh_atomic_test_and_clear_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START);

    fast_prov_srv = get_fast_prov_srv_user_data();
    if (!fast_prov_srv) {
        ESP_LOGE(TAG, "%s: Failed to get fast prov server model user_data", __func__);
        return;
    }

    model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                               ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI, CID_ESP);
    if (!model) {
        ESP_LOGE(TAG, "%s: Failed to get fast prov client model", __func__);
        return;
    }

    if (node_addr_send.send_succeed == true && node_addr_send.ack_received == false) {
        ESP_LOGW(TAG, "%s: Previous node address message is being sent", __func__);
        return;
    }

    if (node_addr_send.send_succeed == true) {
        /* If the previous node address message has been sent successfully, and when
         * timeout event comes, we will update the send buffer (node_addr_send).
         */
        net_buf_simple_reset(node_addr_send.addr);
        for (i = 0; i < CONFIG_BLE_MESH_MAX_PROV_NODES; i++) {
            uint16_t addr = example_get_node_address(i);
            if (!ESP_BLE_MESH_ADDR_IS_UNICAST(addr)) {
                continue;
            }
            for (j = 0; j < ARRAY_SIZE(addr_already_sent); j++) {
                if (addr == addr_already_sent[j]) {
                    ESP_LOGW(TAG, "%s: node addr 0x%04x has already been sent", __func__, addr);
                    break;
                }
            }
            if (j != ARRAY_SIZE(addr_already_sent)) {
                continue;
            }
            net_buf_simple_add_le16(node_addr_send.addr, addr);
            if (node_addr_send.addr->len == NODE_ADDR_SEND_MAX_LEN) {
                break;
            }
        }
    }

    if (node_addr_send.addr->len) {
        example_msg_common_info_t info = {
            .net_idx = fast_prov_srv->net_idx,
            .app_idx = fast_prov_srv->app_idx,
            .dst = fast_prov_srv->prim_prov_addr,
            .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
            .role = ROLE_FAST_PROV,
#endif
        };
        err = example_send_fast_prov_self_prov_node_addr(model, &info, node_addr_send.addr);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to send node address", __func__);
            node_addr_send.send_succeed = false;
        } else {
            node_addr_send.send_succeed = true;
        }
        node_addr_send.ack_received = false;
    }

    /* If sending node addresses failed, the Provisioner will start the timer in case
     * no other devices will be provisioned and the timer will never start.
     */
    if (node_addr_send.send_succeed == false && node_addr_send.ack_received == false) {
        if (!bt_mesh_atomic_test_and_set_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START)) {
            k_delayed_work_submit(&send_self_prov_node_addr_timer, SEND_SELF_PROV_NODE_ADDR_TIMEOUT);
        }
    }

    return;
}

#if !defined(CONFIG_BLE_MESH_FAST_PROV)
/* Timeout handler for get_all_node_addr_timer */
static void example_get_all_node_addr(struct k_work *work)
{
    esp_ble_mesh_model_t *model = NULL;
    example_node_info_t *node = NULL;
    esp_err_t err;

    node = example_get_node_info(prim_prov_addr);
    if (!node) {
        ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
        return;
    }

    model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                               ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI, CID_ESP);
    if (!model) {
        ESP_LOGE(TAG, "%s: Failed to get model info", __func__);
        return;
    }

    example_msg_common_info_t info = {
        .net_idx = node->net_idx,
        .app_idx = node->app_idx,
        .dst = node->unicast_addr,
        .timeout = 10000,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        .role = ROLE_PROVISIONER,
#endif
    };
    err = example_send_fast_prov_all_node_addr_get(model, &info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: Failed to send Fast Prov Node Address Get message", __func__);
        return;
    }
}
#endif

esp_err_t example_fast_prov_client_recv_timeout(uint32_t opcode, esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx)
{
#if defined(CONFIG_BLE_MESH_FAST_PROV)
    example_fast_prov_server_t *fast_prov_srv = NULL;
#endif
    example_node_info_t *node = NULL;
    esp_err_t err;

    ESP_LOGW(TAG, "%s: Receive fast prov server status timeout", __func__);

    if (!model || !ctx) {
        return ESP_ERR_INVALID_ARG;
    }

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    fast_prov_srv = get_fast_prov_srv_user_data();
    if (!fast_prov_srv) {
        ESP_LOGE(TAG, "%s: Failed to get fast prov server model user_data", __func__);
        return ESP_FAIL;
    }
#endif

    switch (opcode) {
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET: {
        example_fast_prov_info_set_t set = {0};
        node = example_get_node_info(ctx->addr);
        if (!node) {
            return ESP_FAIL;
        }
        example_msg_common_info_t info = {
            .net_idx = node->net_idx,
            .app_idx = node->app_idx,
            .dst = node->unicast_addr,
            .timeout = 0,
        };
#if defined(CONFIG_BLE_MESH_FAST_PROV)
        if (node->lack_of_addr == false) {
            set.ctx_flags = 0x03FE;
            memcpy(&set.unicast_min, &node->unicast_min,
                   sizeof(example_node_info_t) - offsetof(example_node_info_t, unicast_min));
        } else {
            set.ctx_flags  = BIT(6);
            set.group_addr = fast_prov_srv->group_addr;
        }
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        info.role = ROLE_FAST_PROV;
#endif
#else
        set.ctx_flags = 0x037F;
        memcpy(&set.node_addr_cnt, &node->node_addr_cnt,
               sizeof(example_node_info_t) - offsetof(example_node_info_t, node_addr_cnt));
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
        info.role = ROLE_PROVISIONER;
#endif
#endif
        err = example_send_fast_prov_info_set(model, &info, &set);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to send Fast Prov Info Set message", __func__);
            return ESP_FAIL;
        }
        break;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD:
        break;
#if defined(CONFIG_BLE_MESH_FAST_PROV)
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR:
        if (node_addr_send.addr->len) {
            example_msg_common_info_t info = {
                .net_idx = fast_prov_srv->net_idx,
                .app_idx = fast_prov_srv->app_idx,
                .dst = fast_prov_srv->prim_prov_addr,
                .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
                .role = ROLE_FAST_PROV,
#endif
            };
            err = example_send_fast_prov_self_prov_node_addr(model, &info, node_addr_send.addr);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: Failed to send Fast Prov Node Addr message", __func__);
                node_addr_send.send_succeed = false;
            } else {
                node_addr_send.send_succeed = true;
            }
            node_addr_send.ack_received = false;
        }
        if (node_addr_send.send_succeed == false && node_addr_send.ack_received == false) {
            if (!bt_mesh_atomic_test_and_set_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START)) {
                k_delayed_work_submit(&send_self_prov_node_addr_timer, SEND_SELF_PROV_NODE_ADDR_TIMEOUT);
            }
        }
        break;
#endif
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET: {
        node = example_get_node_info(ctx->addr);
        if (!node) {
            return ESP_FAIL;
        }
        example_msg_common_info_t info = {
            .net_idx = node->net_idx,
            .app_idx = node->app_idx,
            .dst = node->unicast_addr,
            .timeout = 10000,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
            .role = ROLE_PROVISIONER,
#endif
        };
        err = example_send_fast_prov_all_node_addr_get(model, &info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to send Fast Prov Node Address message", __func__);
            return ESP_FAIL;
        }
        break;
    }
    default:
        break;
    }

    return ESP_OK;
}

esp_err_t example_fast_prov_client_recv_status(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx,
        uint16_t len, const uint8_t *data)
{
    if (!model || !ctx) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOG_BUFFER_HEX("fast prov client receives", data, len);

    switch (ctx->recv_op) {
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS:
        ESP_LOG_BUFFER_HEX("fast prov info status", data, len);
#if !defined(CONFIG_BLE_MESH_FAST_PROV)
        prim_prov_addr = ctx->addr;
        k_delayed_work_init(&get_all_node_addr_timer, example_get_all_node_addr);
        k_delayed_work_submit(&get_all_node_addr_timer, GET_ALL_NODE_ADDR_TIMEOUT);
#endif
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS:
        ESP_LOGI(TAG, "status_key: 0x%02x, status_act: 0x%02x", data[0], data[1]);
        break;
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK: {
        /* node address message has been acked */
        int i, j;
        uint8_t length = node_addr_send.addr->len;
        node_addr_send.send_succeed = true;
        node_addr_send.ack_received = true;
        for (i = 0; i < (length >> 1); i++) {
            uint16_t addr = net_buf_simple_pull_le16(node_addr_send.addr);
            if (ESP_BLE_MESH_ADDR_IS_UNICAST(addr)) {
                for (j = 0; j < ARRAY_SIZE(addr_already_sent); j++) {
                    if (addr_already_sent[j] == addr) {
                        break;
                    }
                }
                if (j != ARRAY_SIZE(addr_already_sent)) {
                    continue;
                }
                for (j = 0; j < ARRAY_SIZE(addr_already_sent); j++) {
                    if (addr_already_sent[j] == ESP_BLE_MESH_ADDR_UNASSIGNED) {
                        addr_already_sent[j] = addr;
                        break;
                    }
                }
                if (j == ARRAY_SIZE(addr_already_sent)) {
                    ESP_LOGE(TAG, "%s: No place to store the sent node address", __func__);
                    return ESP_FAIL;
                }
            }
        }
        /* In case spending too much time on the first node address message(i.e. failed
         * to receive ack), and the timer for the second or further messages is timeout,
         * thus the Provisioner will never be able to send other addresses.
         */
        if (!bt_mesh_atomic_test_and_set_bit(&fast_prov_cli_flags, SEND_SELF_PROV_NODE_ADDR_START)) {
            k_delayed_work_submit(&send_self_prov_node_addr_timer, SEND_SELF_PROV_NODE_ADDR_TIMEOUT);
        }
        break;
    }
    case ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS: {
        ESP_LOG_BUFFER_HEX("Node address", data, len);
#if CONFIG_BLE_MESH_GENERIC_ONOFF_CLI
        esp_ble_mesh_model_t *cli_model = NULL;
        example_node_info_t *node = NULL;
        esp_err_t err;
        node = example_get_node_info(prim_prov_addr);
        if (!node) {
            ESP_LOGE(TAG, "%s: Failed to get node info", __func__);
            return ESP_FAIL;
        }
        cli_model = example_find_model(esp_ble_mesh_get_primary_element_address(),
                                       ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI, ESP_BLE_MESH_CID_NVAL);
        if (!cli_model) {
            ESP_LOGE(TAG, "%s: Failed to get Generic OnOff Client Model info", __func__);
            return ESP_FAIL;
        }
        example_msg_common_info_t info = {
            .net_idx = node->net_idx,
            .app_idx = node->app_idx,
            .dst = node->group_addr,
            .timeout = 0,
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 2, 0)
            .role = ROLE_PROVISIONER,
#endif
        };
        err = example_send_generic_onoff_set(cli_model, &info, LED_ON, 0x00, false);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: Failed to send Generic OnOff Set Unack message", __func__);
            return ESP_FAIL;
        }
#endif /* CONFIG_BLE_MESH_GENERIC_ONOFF_CLI */
        break;
    }
    default:
        ESP_LOGE(TAG, "%s: Invalid fast prov status opcode", __func__);
        return ESP_FAIL;
    }

    return ESP_OK;
}
