// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_FAST_PROV_SERVER_MODEL_H
#define _ESP_FAST_PROV_SERVER_MODEL_H

#include "esp_fast_prov_common.h"

#define DISABLE_FAST_PROV_TIMEOUT       K_SECONDS(10)
#define GATT_PROXY_ENABLE_TIMEOUT       K_SECONDS(10)

#define FAST_PROV_NODE_COUNT_MIN        0x01

enum {
    DISABLE_FAST_PROV_START,    /* Flag indicates the timer used to disable fast provisioning has been started */
    GATT_PROXY_ENABLE_START,    /* Flag indicates the timer used to enable Mesh GATT Proxy has been started */
    RELAY_PROXY_DISABLED,       /* Flag indicates if relay & proxy_adv are enabled or disabled */
    SRV_MAX_FLAGS,
};

enum {
    STATE_IDLE,
    STATE_PEND,
    STATE_ACTIVE,
    STATE_MAX,
};

struct fast_prov_info_set {
    bool set_succeed;
    uint16_t node_addr_cnt;
    uint16_t unicast_min;
    uint16_t unicast_max;
    uint8_t  flags;
    uint32_t iv_index;
    uint16_t net_idx;
    uint16_t group_addr;
    uint16_t pri_prov_addr;
    uint8_t  match_val[16];
    uint8_t  match_len;
    uint8_t  action;
};

typedef struct {
    esp_ble_mesh_model_t *model;    /* Fast Prov Server model pointer */
    BLE_MESH_ATOMIC_DEFINE(srv_flags, SRV_MAX_FLAGS);

    bool     primary_role;  /* Indicate if the device is a Primary Provisioner */
    uint8_t  max_node_num;  /* The maximum number of devices can be provisioned by the Provisioner */
    uint8_t  prov_node_cnt; /* Number of self-provisioned nodes */
    uint16_t app_idx;       /* AppKey index of the application key added by other Provisioner */
    uint16_t top_address;   /* Address of the device(e.g. phone) which triggers fast provisioning */

    esp_ble_mesh_msg_ctx_t ctx; /* the context stored for sending fast prov status message */
    struct fast_prov_info_set *set_info;    /* Used to store received fast prov info set context */

    uint16_t node_addr_cnt;     /* Number of node address shall be received */
    uint16_t unicast_min;       /* Minimum unicast address can be send to other nodes */
    uint16_t unicast_max;       /* Maximum unicast address can be send to other nodes */
    uint16_t unicast_cur;       /* Current unicast address can be assigned */
    uint16_t unicast_step;      /* Unicast address change step */
    uint8_t  flags;             /* Flags state */
    uint32_t iv_index;          /* Iv_index state */
    uint16_t net_idx;           /* Netkey index state */
    uint16_t group_addr;        /* Subscribed group address */
    uint16_t prim_prov_addr;    /* Unicast address of Primary Provisioner */
    uint8_t  match_val[16];     /* Match value to be compared with unprovisioned device UUID */
    uint8_t  match_len;         /* Length of match value to be compared */

    uint8_t  pend_act;          /* Pending action to be performed */
    uint8_t  state;             /* Fast prov state -> 0: idle, 1: pend, 2: active */

    struct k_delayed_work disable_fast_prov_timer;  /* Used to disable fast provisioning */
    struct k_delayed_work gatt_proxy_enable_timer;  /* Used to enable Mesh GATT Proxy functionality */
} __attribute__((packed)) example_fast_prov_server_t;

esp_err_t example_store_remote_node_address(uint16_t node_addr);

esp_err_t example_fast_prov_server_recv_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, struct net_buf_simple *buf);

esp_err_t example_handle_fast_prov_info_set_comp_evt(esp_ble_mesh_model_t *model, uint8_t status_unicast,
        uint8_t status_net_idx, uint8_t status_match);

esp_err_t example_handle_fast_prov_action_set_comp_evt(esp_ble_mesh_model_t *model, uint8_t status_action);

esp_err_t example_handle_fast_prov_status_send_comp_evt(int err_code, uint32_t opcode,
        esp_ble_mesh_model_t *model, esp_ble_mesh_msg_ctx_t *ctx);

esp_err_t example_fast_prov_server_init(esp_ble_mesh_model_t *model);

#endif /* _ESP_FAST_PROV_SERVER_MODEL_H */