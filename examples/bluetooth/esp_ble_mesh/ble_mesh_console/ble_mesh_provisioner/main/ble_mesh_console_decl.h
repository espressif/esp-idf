/* Console example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option).

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#include "esp_ble_mesh_defs.h"

// Register system functions
void register_system(void);

// Register bluetooth
void register_bluetooth(void);

// Register mesh node cmd
void ble_mesh_register_mesh_node(void);

// Register Test Perf client cmd
void ble_mesh_register_mesh_test_performance_client(void);

#if (CONFIG_BLE_MESH_CFG_CLI)
// Register mesh config client operation cmd
void ble_mesh_register_configuration_client_model(void);
#endif

#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
// Register mesh client operation cmd
void ble_mesh_register_gen_onoff_client(void);
#endif

#if CONFIG_BLE_MESH_PROVISIONER
// Regitster mesh provisioner cmd
void ble_mesh_register_mesh_provisioner(void);
#endif
