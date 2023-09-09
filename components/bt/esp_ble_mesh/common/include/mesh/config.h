/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_CONFIG_H_
#define _BLE_MESH_CONFIG_H_

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BLE_MESH_PBA_SAME_TIME
#define CONFIG_BLE_MESH_PBA_SAME_TIME   0
#endif

#ifndef CONFIG_BLE_MESH_PBG_SAME_TIME
#define CONFIG_BLE_MESH_PBG_SAME_TIME   0
#endif

#define CONFIG_BLE_MESH_PRIVATE_BEACON (CONFIG_BLE_MESH_PRB_SRV || \
                                        CONFIG_BLE_MESH_PRB_CLI)

#define CONFIG_BLE_MESH_GENERIC_CLIENT (CONFIG_BLE_MESH_GENERIC_ONOFF_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_LEVEL_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_DEF_TRANS_TIME_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_POWER_ONOFF_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_POWER_LEVEL_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_BATTERY_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_LOCATION_CLI || \
                                        CONFIG_BLE_MESH_GENERIC_PROPERTY_CLI)

#define CONFIG_BLE_MESH_TIME_SCENE_CLIENT (CONFIG_BLE_MESH_TIME_CLI || \
                                           CONFIG_BLE_MESH_SCENE_CLI || \
                                           CONFIG_BLE_MESH_SCHEDULER_CLI)

#define CONFIG_BLE_MESH_LIGHTING_CLIENT (CONFIG_BLE_MESH_LIGHT_LIGHTNESS_CLI || \
                                         CONFIG_BLE_MESH_LIGHT_CTL_CLI || \
                                         CONFIG_BLE_MESH_LIGHT_HSL_CLI || \
                                         CONFIG_BLE_MESH_LIGHT_XYL_CLI || \
                                         CONFIG_BLE_MESH_LIGHT_LC_CLI)

#define CONFIG_BLE_MESH_SERVER_MODEL (CONFIG_BLE_MESH_GENERIC_SERVER || \
                                      CONFIG_BLE_MESH_SENSOR_SERVER || \
                                      CONFIG_BLE_MESH_TIME_SCENE_SERVER || \
                                      CONFIG_BLE_MESH_LIGHTING_SERVER)

#define CONFIG_BLE_MESH_BLE_COEX_SUPPORT (CONFIG_BLE_MESH_SUPPORT_BLE_ADV || \
                                          CONFIG_BLE_MESH_SUPPORT_BLE_SCAN)

#define CONFIG_BLE_MESH_PROXY_SOLIC (CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX || \
                                     CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX)

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_CONFIG_H_ */
