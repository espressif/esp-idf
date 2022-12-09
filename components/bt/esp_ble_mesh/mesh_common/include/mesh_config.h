// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _BLE_MESH_CONFIG_H_
#define _BLE_MESH_CONFIG_H_

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_CONFIG_H_ */