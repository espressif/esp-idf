/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_CFG_SRV_MODEL_H_
#define _BLE_MESH_CFG_SRV_MODEL_H_

#include "esp_ble_mesh_defs.h"
#if (CONFIG_BLE_MESH_CFG_CLI)
#include "esp_ble_mesh_config_model_api.h"
#endif
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
#include "esp_ble_mesh_generic_model_api.h"
#endif //CONFIG_BLE_MESH_GENERIC_ONOFF_CLI

#define NODE_MAX_GROUP_CONFIG 3
#define CID_ESP 0x02C4

extern uint8_t dev_uuid[16];

typedef struct {
    uint16_t net_idx;
    uint16_t unicast_addr;
} ble_mesh_node_config_params;
extern ble_mesh_node_config_params ble_mesh_node_prestore_params[NODE_MAX_GROUP_CONFIG];

extern esp_ble_mesh_prov_t prov;

extern esp_ble_mesh_model_pub_t vendor_model_pub_config;

// Configuration Server model
extern esp_ble_mesh_cfg_srv_t cfg_srv;
extern esp_ble_mesh_model_t config_server_models[];
extern esp_ble_mesh_elem_t config_server_elements[];
extern esp_ble_mesh_comp_t config_server_comp;

// Configuration Client model
extern esp_ble_mesh_client_t cfg_cli;
extern esp_ble_mesh_model_t config_client_models[];
extern esp_ble_mesh_elem_t config_client_elements[];
extern esp_ble_mesh_comp_t config_client_comp;

// Generic OnOff Server model
extern esp_ble_mesh_gen_onoff_srv_t onoff_server;
extern esp_ble_mesh_model_op_t gen_onoff_srv_model_op_config[];

// Generic OnOff Client model
#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
extern esp_ble_mesh_client_t gen_onoff_cli;
extern esp_ble_mesh_model_t gen_onoff_cli_models[];
#endif //CONFIG_BLE_MESH_GENERIC_ONOFF_CLI

//CONFIG VENDOR MODEL TEST PERFORMANCE
#define ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_SRV 0x2000
#define ESP_BLE_MESH_VND_MODEL_ID_TEST_PERF_CLI 0x2001

#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_GET                 ESP_BLE_MESH_MODEL_OP_3(0x01, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET                 ESP_BLE_MESH_MODEL_OP_3(0x02, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_SET_UNACK           ESP_BLE_MESH_MODEL_OP_3(0x03, CID_ESP)
#define ESP_BLE_MESH_VND_MODEL_OP_TEST_PERF_STATUS              ESP_BLE_MESH_MODEL_OP_3(0x04, CID_ESP)

extern esp_ble_mesh_client_t test_perf_cli;
extern esp_ble_mesh_model_op_t test_perf_srv_op[];
extern esp_ble_mesh_model_op_t test_perf_cli_op[];
extern esp_ble_mesh_model_t config_models[];

#if (CONFIG_BLE_MESH_CFG_CLI)
void ble_mesh_configuration_client_model_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                            esp_ble_mesh_cfg_client_cb_param_t *param);
#endif

#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
void ble_mesh_generic_onoff_client_model_cb(esp_ble_mesh_generic_client_cb_event_t event,
                                            esp_ble_mesh_generic_client_cb_param_t *param);
#endif

#if(CONFIG_BLE_MESH_GENERIC_SERVER)
void ble_mesh_generic_server_model_cb(esp_ble_mesh_generic_server_cb_event_t event,
                                      esp_ble_mesh_generic_server_cb_param_t *param);
#endif

void ble_mesh_vendor_model_cb(esp_ble_mesh_model_cb_event_t event, esp_ble_mesh_model_cb_param_t *param);

#endif //_BLE_MESH_CFG_SRV_MODEL_H_
