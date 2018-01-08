// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __BT_MESH_MODEL_OP_H_
#define __BT_MESH_MODEL_OP_H_

#include "mesh_main.h"

/* Generic OnOff Message Opcode */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET                          BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET                          BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK                    BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS                       BT_MESH_MODEL_OP_2(0x82, 0x04)

/* Generic Level Message Opcode */
#define BT_MESH_MODEL_OP_GEN_LEVEL_GET                          BT_MESH_MODEL_OP_2(0x82, 0x05)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET                          BT_MESH_MODEL_OP_2(0x82, 0x06)
#define BT_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK                    BT_MESH_MODEL_OP_2(0x82, 0x07)
#define BT_MESH_MODEL_OP_GEN_LEVEL_STATUS                       BT_MESH_MODEL_OP_2(0x82, 0x08)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET                          BT_MESH_MODEL_OP_2(0x82, 0x09)
#define BT_MESH_MODEL_OP_GEN_DELTA_SET_UNACK                    BT_MESH_MODEL_OP_2(0x82, 0x0A)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET                           BT_MESH_MODEL_OP_2(0x82, 0x0B)
#define BT_MESH_MODEL_OP_GEN_MOVE_SET_UNACK                     BT_MESH_MODEL_OP_2(0x82, 0x0C)

/* Generic Default Transition Time Message Opcode*/
#define BT_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET                 BT_MESH_MODEL_OP_2(0x82, 0x0D)
#define BT_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET                 BT_MESH_MODEL_OP_2(0x82, 0x0E)
#define BT_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK           BT_MESH_MODEL_OP_2(0x82, 0x0F)
#define BT_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS              BT_MESH_MODEL_OP_2(0x82, 0x10)

/* Generic Power OnOff Message Opcode*/
#define BT_MESH_MODEL_OP_GEN_ONPOWERUP_GET                      BT_MESH_MODEL_OP_2(0x82, 0x11)
#define BT_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS                   BT_MESH_MODEL_OP_2(0x82, 0x12)

/* Generic Power OnOff Setup Message Opcode */
#define BT_MESH_MODEL_OP_GEN_ONPOWERUP_SET                      BT_MESH_MODEL_OP_2(0x82, 0x13)
#define BT_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK                BT_MESH_MODEL_OP_2(0x82, 0x14)

/* Generic Power Level Message Opcode */
#define BT_MESH_MODEL_OP_GEN_POWER_LEVEL_GET                    BT_MESH_MODEL_OP_2(0x82, 0x15)
#define BT_MESH_MODEL_OP_GEN_POWER_LEVEL_SET                    BT_MESH_MODEL_OP_2(0x82, 0x16)
#define BT_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK              BT_MESH_MODEL_OP_2(0x82, 0x17)
#define BT_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS                 BT_MESH_MODEL_OP_2(0x82, 0x18)
#define BT_MESH_MODEL_OP_GEN_POWER_LAST_GET                     BT_MESH_MODEL_OP_2(0x82, 0x19)
#define BT_MESH_MODEL_OP_GEN_POWER_LAST_STATUS                  BT_MESH_MODEL_OP_2(0x82, 0x1A)
#define BT_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET                  BT_MESH_MODEL_OP_2(0x82, 0x1B)
#define BT_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x1C)
#define BT_MESH_MODEL_OP_GEN_POWER_RANGE_GET                    BT_MESH_MODEL_OP_2(0x82, 0x1D)
#define BT_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS                 BT_MESH_MODEL_OP_2(0x82, 0x1E)

/* Generic Power Level Setup Message Opcode */
#define BT_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET                  BT_MESH_MODEL_OP_2(0x82, 0x1F)
#define BT_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x20)
#define BT_MESH_MODEL_OP_GEN_POWER_RANGE_SET                    BT_MESH_MODEL_OP_2(0x82, 0x21)
#define BT_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK              BT_MESH_MODEL_OP_2(0x82, 0x22)

/* Generic Battery Message Opcode */
#define BT_MESH_MODEL_OP_GEN_BATTERY_GET                        BT_MESH_MODEL_OP_2(0x82, 0x23)
#define BT_MESH_MODEL_OP_GEN_BATTERY_STATUS                     BT_MESH_MODEL_OP_2(0x82, 0x24)

/* Generic Location Message Opcode */
#define BT_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET                     BT_MESH_MODEL_OP_2(0x82, 0x25)
#define BT_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS                  BT_MESH_MODEL_OP_1(0x40)
#define BT_MESH_MODEL_OP_GEN_LOC_LOCAL_GET                      BT_MESH_MODEL_OP_2(0x82, 0x26)
#define BT_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS                   BT_MESH_MODEL_OP_2(0x82, 0x27)

/* Generic Location Setup Message Opcode */
#define BT_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET                     BT_MESH_MODEL_OP_1(0x41)
#define BT_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK               BT_MESH_MODEL_OP_1(0x42)
#define BT_MESH_MODEL_OP_GEN_LOC_LOCAL_SET                      BT_MESH_MODEL_OP_2(0x82, 0x28)
#define BT_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK                BT_MESH_MODEL_OP_2(0x82, 0x29)

/* Generic Manufacturer Property Message Opcode */
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET                BT_MESH_MODEL_OP_2(0x82, 0x2A)
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS             BT_MESH_MODEL_OP_1(0x43)
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET                  BT_MESH_MODEL_OP_2(0x82, 0x2B)
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET                  BT_MESH_MODEL_OP_1(0x44)
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK            BT_MESH_MODEL_OP_1(0x45)
#define BT_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS               BT_MESH_MODEL_OP_1(0x46)

/* Generic Admin Property Message Opcode */
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET               BT_MESH_MODEL_OP_2(0x82, 0x2C)
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS            BT_MESH_MODEL_OP_1(0x47)
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET                 BT_MESH_MODEL_OP_2(0x82, 0x2D)
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET                 BT_MESH_MODEL_OP_1(0x48)
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK           BT_MESH_MODEL_OP_1(0x49)
#define BT_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS              BT_MESH_MODEL_OP_1(0x4A)

/* Generic User Property Message Opcode */
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET                BT_MESH_MODEL_OP_2(0x82, 0x2E)
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS             BT_MESH_MODEL_OP_1(0x4B)
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTY_GET                  BT_MESH_MODEL_OP_2(0x82, 0x2F)
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTY_SET                  BT_MESH_MODEL_OP_1(0x4C)
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK            BT_MESH_MODEL_OP_1(0x4D)
#define BT_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS               BT_MESH_MODEL_OP_1(0x4E)

/* Generic Client Property Message Opcode */
#define BT_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET              BT_MESH_MODEL_OP_1(0x4F)
#define BT_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS           BT_MESH_MODEL_OP_1(0x50)

/* Sensor Message Opcode */
#define BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET                  BT_MESH_MODEL_OP_2(0x82, 0x30)
#define BT_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS               BT_MESH_MODEL_OP_1(0x51)
#define BT_MESH_MODEL_OP_SENSOR_GET                             BT_MESH_MODEL_OP_2(0x82, 0x31)
#define BT_MESH_MODEL_OP_SENSOR_STATUS                          BT_MESH_MODEL_OP_1(0x52)
#define BT_MESH_MODEL_OP_SENSOR_COLUMN_GET                      BT_MESH_MODEL_OP_2(0x82, 0x32)
#define BT_MESH_MODEL_OP_SENSOR_COLUMN_STATUS                   BT_MESH_MODEL_OP_1(0x53)
#define BT_MESH_MODEL_OP_SENSOR_SERIES_GET                      BT_MESH_MODEL_OP_2(0x82, 0x33)
#define BT_MESH_MODEL_OP_SENSOR_SERIES_STATUS                   BT_MESH_MODEL_OP_1(0x54)

/* Sensor Setup Message Opcode */
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_GET                     BT_MESH_MODEL_OP_2(0x82, 0x34)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_SET                     BT_MESH_MODEL_OP_1(0x55)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK               BT_MESH_MODEL_OP_1(0x56)
#define BT_MESH_MODEL_OP_SENSOR_CADENCE_STATUS                  BT_MESH_MODEL_OP_1(0x57)
#define BT_MESH_MODEL_OP_SENSOR_SETTINGS_GET                    BT_MESH_MODEL_OP_2(0x82, 0x35)
#define BT_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS                 BT_MESH_MODEL_OP_1(0x58)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_GET                     BT_MESH_MODEL_OP_2(0x82, 0x36)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_SET                     BT_MESH_MODEL_OP_1(0x59)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK               BT_MESH_MODEL_OP_1(0x5A)
#define BT_MESH_MODEL_OP_SENSOR_SETTING_STATUS                  BT_MESH_MODEL_OP_1(0x5B)

/* Time Message Opcode */

/* Scene Message Opcode */
#define BT_MESH_MODEL_OP_SCENE_GET                              BT_MESH_MODEL_OP_2(0x82, 0x41)
#define BT_MESH_MODEL_OP_SCENE_RECALL                           BT_MESH_MODEL_OP_2(0x82, 0x42)
#define BT_MESH_MODEL_OP_SCENE_RECALL_UNACK                     BT_MESH_MODEL_OP_2(0x82, 0x43)
#define BT_MESH_MODEL_OP_SCENE_STATUS                           BT_MESH_MODEL_OP_1(0x5E)
#define BT_MESH_MODEL_OP_SCENE_REGISTER_GET                     BT_MESH_MODEL_OP_2(0x82, 0x44)
#define BT_MESH_MODEL_OP_SCENE_REGISTER_STATUS                  BT_MESH_MODEL_OP_2(0x82, 0x45)

/* Scene Setup Message Opcode */
#define BT_MESH_MODEL_OP_SCENE_STORE                            BT_MESH_MODEL_OP_2(0x82, 0x46)
#define BT_MESH_MODEL_OP_SCENE_STORE_UNACK                      BT_MESH_MODEL_OP_2(0x82, 0x47)
#define BT_MESH_MODEL_OP_SCENE_DELETE                           BT_MESH_MODEL_OP_2(0x82, 0x9E)
#define BT_MESH_MODEL_OP_SCENE_DELETE_UNACK                     BT_MESH_MODEL_OP_2(0x82, 0x9F)

/* Scheduler Message Opcode */

/* Scheduler Setup Message Opcode */

/* Light Lightness Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                    BT_MESH_MODEL_OP_2(0x82, 0x4B)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                    BT_MESH_MODEL_OP_2(0x82, 0x4C)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK              BT_MESH_MODEL_OP_2(0x82, 0x4D)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS                 BT_MESH_MODEL_OP_2(0x82, 0x4E)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET             BT_MESH_MODEL_OP_2(0x82, 0x4F)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET             BT_MESH_MODEL_OP_2(0x82, 0x50)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK       BT_MESH_MODEL_OP_2(0x82, 0x51)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS          BT_MESH_MODEL_OP_2(0x82, 0x52)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET               BT_MESH_MODEL_OP_2(0x82, 0x53)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x54)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET            BT_MESH_MODEL_OP_2(0x82, 0x55)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS         BT_MESH_MODEL_OP_2(0x82, 0x56)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET              BT_MESH_MODEL_OP_2(0x82, 0x57)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x58)

/* Light Lightness Setup Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET            BT_MESH_MODEL_OP_2(0x82, 0x59)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK      BT_MESH_MODEL_OP_2(0x82, 0x5A)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET              BT_MESH_MODEL_OP_2(0x82, 0x5B)
#define BT_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x5C)

/* Light CTL Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_CTL_GET                          BT_MESH_MODEL_OP_2(0x82, 0x5D)
#define BT_MESH_MODEL_OP_LIGHT_CTL_SET                          BT_MESH_MODEL_OP_2(0x82, 0x5E)
#define BT_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                    BT_MESH_MODEL_OP_2(0x82, 0x5F)
#define BT_MESH_MODEL_OP_LIGHT_CTL_STATUS                       BT_MESH_MODEL_OP_2(0x82, 0x60)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET              BT_MESH_MODEL_OP_2(0x82, 0x61)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET        BT_MESH_MODEL_OP_2(0x82, 0x62)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS     BT_MESH_MODEL_OP_2(0x82, 0x63)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET              BT_MESH_MODEL_OP_2(0x82, 0x64)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK        BT_MESH_MODEL_OP_2(0x82, 0x65)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS           BT_MESH_MODEL_OP_2(0x82, 0x66)
#define BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET                  BT_MESH_MODEL_OP_2(0x82, 0x67)
#define BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x68)

/* Light CTL Setup Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                  BT_MESH_MODEL_OP_2(0x82, 0x69)
#define BT_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x6A)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET        BT_MESH_MODEL_OP_2(0x82, 0x6B)
#define BT_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK  BT_MESH_MODEL_OP_2(0x82, 0x6C)

/* Light HSL Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_HSL_GET                          BT_MESH_MODEL_OP_2(0x82, 0x6D)
#define BT_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                      BT_MESH_MODEL_OP_2(0x82, 0x6E)
#define BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                      BT_MESH_MODEL_OP_2(0x82, 0x6F)
#define BT_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK                BT_MESH_MODEL_OP_2(0x82, 0x70)
#define BT_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                   BT_MESH_MODEL_OP_2(0x82, 0x71)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET               BT_MESH_MODEL_OP_2(0x82, 0x72)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET               BT_MESH_MODEL_OP_2(0x82, 0x73)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK         BT_MESH_MODEL_OP_2(0x82, 0x74)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS            BT_MESH_MODEL_OP_2(0x82, 0x75)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SET                          BT_MESH_MODEL_OP_2(0x82, 0x76)
#define BT_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                    BT_MESH_MODEL_OP_2(0x82, 0x77)
#define BT_MESH_MODEL_OP_LIGHT_HSL_STATUS                       BT_MESH_MODEL_OP_2(0x82, 0x78)
#define BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                   BT_MESH_MODEL_OP_2(0x82, 0x79)
#define BT_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS                BT_MESH_MODEL_OP_2(0x82, 0x7A)
#define BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                  BT_MESH_MODEL_OP_2(0x82, 0x7B)
#define BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS               BT_MESH_MODEL_OP_2(0x82, 0x7C)
#define BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                    BT_MESH_MODEL_OP_2(0x82, 0x7D)
#define BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                 BT_MESH_MODEL_OP_2(0x82, 0x7E)

/* Light HSL Setup Message Opcode */
#define BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                  BT_MESH_MODEL_OP_2(0x82, 0x7F)
#define BT_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK            BT_MESH_MODEL_OP_2(0x82, 0x80)
#define BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                    BT_MESH_MODEL_OP_2(0x82, 0x81)
#define BT_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK              BT_MESH_MODEL_OP_2(0x82, 0x82) /* Model spec is wrong */

/* Light xyL Message Opcode */

/* Light xyL Setup Message Opcode */

/* Light Control Message Opcode */

#endif /* __BT_MESH_MODEL_OP_H_ */
