/*
 * Console example - declarations of command registration functions.
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_ble_mesh_defs.h"

// Register system functions
void register_system(void);

// Register blutooth
void register_bluetooth(void);

// Register mesh node cmd
void ble_mesh_register_mesh_node(void);

// Register Test Perf client cmd
void ble_mesh_register_mesh_test_performance_client(void);

#if (CONFIG_BLE_MESH_CFG_CLI)
// Register mesh config client operation cmd
void ble_mesh_register_configuration_client_model(void);
#endif

// Register mesh config server and generic server operation cmd
void ble_mesh_register_server(void);

#if (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI)
// Register mesh client operation cmd
void ble_mesh_register_gen_onoff_client(void);
#endif

#if (CONFIG_BLE_MESH_CFG_CLI)
// Register mesh config client operation cmd
void ble_mesh_register_configuration_client_model(void);
#endif
