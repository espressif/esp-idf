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

#ifndef _ESP_FAST_PROV_COMMON_H
#define _ESP_FAST_PROV_COMMON_H

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_config_model_api.h"

#define LED_OFF     0x00
#define LED_ON      0x01

#define CID_ESP     0x02E5
#define CID_NVAL    0xFFFF

/* Fast Prov Model ID */
#define ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_CLI                 0x0000
#define ESP_BLE_MESH_VND_MODEL_ID_FAST_PROV_SRV                 0x0001

/* Fast Prov Message Opcode */
#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_SET            ESP_BLE_MESH_MODEL_OP_3(0x00, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_INFO_STATUS         ESP_BLE_MESH_MODEL_OP_3(0x01, CID_ESP)

#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_ADD         ESP_BLE_MESH_MODEL_OP_3(0x02, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NET_KEY_STATUS      ESP_BLE_MESH_MODEL_OP_3(0x03, CID_ESP)

#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR           ESP_BLE_MESH_MODEL_OP_3(0x04, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_ACK       ESP_BLE_MESH_MODEL_OP_3(0x05, CID_ESP)

#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_GET       ESP_BLE_MESH_MODEL_OP_3(0x06, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_ADDR_STATUS    ESP_BLE_MESH_MODEL_OP_3(0x07, CID_ESP)

#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_ADD      ESP_BLE_MESH_MODEL_OP_3(0x08, CID_ESP)

#define ESP_BLE_MESH_VND_MODEL_OP_FAST_PROV_NODE_GROUP_DELETE   ESP_BLE_MESH_MODEL_OP_3(0x09, CID_ESP)

typedef struct {
    uint16_t net_idx;
    uint16_t app_idx;
    uint16_t dst;
    int32_t  timeout;
    esp_ble_mesh_dev_role_t role;
} example_msg_common_info_t;

typedef struct {
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  app_key[16];

    uint16_t node_addr_cnt; /* Number of BLE Mesh nodes in the network */
    uint16_t unicast_min;   /* Minimum unicast address to be assigned to the nodes in the network */
    uint16_t unicast_max;   /* Maximum unicast address to be assigned to the nodes in the network */
    uint16_t group_addr;    /* Group address which will be subscribed by the nodes in the network */
    uint8_t  match_val[16]; /* Match value used by Fast Provisoning Provisioner */
    uint8_t  match_len;

    uint8_t  max_node_num;  /* Maximum number of nodes can be provisioned by the client */
} __attribute__((packed)) example_prov_info_t;

/* Fast Prov Info Set Message Context */
typedef struct {
    uint16_t ctx_flags;     /* Flags indicate which part of context exists */
    uint16_t node_addr_cnt; /* Number of the nodes going to be provisioned */
    uint16_t unicast_min;   /* Assigned minimum unicast address */
    uint16_t unicast_max;   /* Assigned maximum unicast address */
    uint8_t  flags;         /* Flags used for provisioning data */
    uint32_t iv_index;      /* IV_index used for provisioning data */
    uint16_t net_idx;       /* Netkey index used for provisioning data */
    uint16_t group_addr;    /* Group address going to be added to model */
    uint16_t prov_addr;     /* Primary Provisioner address */
    uint8_t  match_val[16]; /* Match value used for provisioning */
    uint8_t  match_len;
    uint8_t  action;        /* Action used to enbale/disable Provisioner functionality */
} __attribute__((packed)) example_fast_prov_info_set_t;

typedef struct {
    /* The following is the basic information of a node */
    bool     reprov;
    uint8_t  uuid[16];
    uint16_t unicast_addr;
    uint8_t  element_num;
    uint16_t net_idx;
    uint16_t app_idx;
    uint8_t  onoff;

    /* The following is the information which will be/has been sent to the node */
    bool     lack_of_addr;
    uint16_t node_addr_cnt;
    uint16_t unicast_min;
    uint16_t unicast_max;
    uint8_t  flags;
    uint32_t iv_index;
    uint16_t fp_net_idx;
    uint16_t group_addr;
    uint16_t prov_addr;
    uint8_t  match_val[16];
    uint8_t  match_len;
    uint8_t  action;
} __attribute__((packed)) example_node_info_t;

typedef struct {
    uint8_t net_key[16]; /* Network key going to be added */
} example_fast_prov_net_key_add_t;

typedef struct {
    uint8_t status_key; /* Indicate the result of adding network key */
    uint8_t status_act; /* Indicate the result of action */
} example_fast_prov_net_key_status_t;

#endif /* _ESP_FAST_PROV_COMMON_H */