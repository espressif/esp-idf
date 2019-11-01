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

#include <string.h>
#include <stdint.h>

#include "mesh_types.h"

#include "server_common.h"
#include "device_property.h"

static struct bt_mesh_dev_prop {
    u16_t prop_id;
    u8_t  len;
} device_properties [] = {
    { BLE_MESH_INVALID_DEVICE_PROPERTY_ID,                               0xFF },
    { BLE_MESH_AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY,           0x03 },
    { BLE_MESH_AVERAGE_INPUT_CURRENT,                                    0x03 },
    { BLE_MESH_AVERAGE_INPUT_VOLTAGE,                                    0x03 },
    { BLE_MESH_AVERAGE_OUTPUT_CURRENT,                                   0x03 },
    { BLE_MESH_AVERAGE_OUTPUT_VOLTAGE,                                   0x03 },
    { BLE_MESH_CENTER_BEAM_INTENSITY_AT_FULL_POWER,                      0x02 },
    { BLE_MESH_CHROMATICITY_TOLERANCE,                                   0x01 },
    { BLE_MESH_COLOR_RENDERING_INDEX_R9,                                 0x01 },
    { BLE_MESH_COLOR_RENDERING_INDEX_RA,                                 0x01 },
    { BLE_MESH_DEVICE_APPEARANCE,                                        0x02 },
    { BLE_MESH_DEVICE_COUNTRY_OF_ORIGIN,                                 0x02 },
    { BLE_MESH_DEVICE_DATE_OF_MANUFACTURE,                               0x04 },
    { BLE_MESH_DEVICE_ENERGY_USE_SINCE_TURN_ON,                          0x04 },
    { BLE_MESH_DEVICE_FIRMWARE_REVISION,                                 0x08 },
    { BLE_MESH_DEVICE_GLOBAL_TRADE_ITEM_NUMBER,                          0x08 },
    { BLE_MESH_DEVICE_HARDWARE_REVISION,                                 0x16 },
    { BLE_MESH_DEVICE_MANUFACTURER_NAME,                                 0x36 },
    { BLE_MESH_DEVICE_MODEL_NUMBER,                                      0x24 },
    { BLE_MESH_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION,         0x04 },
    { BLE_MESH_DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES,          0x09 },
    { BLE_MESH_DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS,                 0x06 },
    { BLE_MESH_DEVICE_POWER_RANGE_SPECIFICATION,                         0x12 },
    { BLE_MESH_DEVICE_RUNTIME_SINCE_TURN_ON,                             0x04 },
    { BLE_MESH_DEVICE_RUNTIME_WARRANTY,                                  0x04 },
    { BLE_MESH_DEVICE_SERIAL_NUMBER,                                     0x16 },
    { BLE_MESH_DEVICE_SOFTWARE_REVISION,                                 0x08 },
    { BLE_MESH_DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS,                0x06 },
    { BLE_MESH_INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES,            0x05 },
    { BLE_MESH_INITIAL_CIE_1931_CHROMATICITY_COORDINATES,                0x04 },
    { BLE_MESH_INITIAL_CORRELATED_COLOR_TEMPERATURE,                     0x02 },
    { BLE_MESH_INITIAL_LUMINOUS_FLUX,                                    0x02 },
    { BLE_MESH_INITIAL_PLANCKIAN_DISTANCE,                               0x02 },
    { BLE_MESH_INPUT_CURRENT_RANGE_SPECIFICATION,                        0x06 },
    { BLE_MESH_INPUT_CURRENT_STATISTICS,                                 0x09 },
    { BLE_MESH_INPUT_OVER_CURRENT_EVENT_STATISTICS,                      0x06 },
    { BLE_MESH_INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS,               0x06 },
    { BLE_MESH_INPUT_OVER_VOLTAGE_EVENT_STATISTICS,                      0x06 },
    { BLE_MESH_INPUT_UNDER_CURRENT_EVENT_STATISTICS,                     0x06 },
    { BLE_MESH_INPUT_UNDER_VOLTAGE_EVENT_STATISTICS,                     0x06 },
    { BLE_MESH_INPUT_VOLTAGE_RANGE_SPECIFICATION,                        0x06 },
    { BLE_MESH_INPUT_VOLTAGE_RIPPLE_SPECIFICATION,                       0x01 },
    { BLE_MESH_INPUT_VOLTAGE_STATISTICS,                                 0x09 },
    { BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON,                        0x04 },
    { BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG,                   0x04 },
    { BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY,                   0x04 },
    { BLE_MESH_LIGHT_CONTROL_LIGHTNESS_ON,                               0x02 },
    { BLE_MESH_LIGHT_CONTROL_LIGHTNESS_PROLONG,                          0x02 },
    { BLE_MESH_LIGHT_CONTROL_LIGHTNESS_STANDBY,                          0x02 },
    { BLE_MESH_LIGHT_CONTROL_REGULATOR_ACCURACY,                         0x01 },
    { BLE_MESH_LIGHT_CONTROL_REGULATOR_KID,                              0x04 },
    { BLE_MESH_LIGHT_CONTROL_REGULATOR_KIU,                              0x04 },
    { BLE_MESH_LIGHT_CONTROL_REGULATOR_KPD,                              0x04 },
    { BLE_MESH_LIGHT_CONTROL_REGULATOR_KPU,                              0x04 },
    { BLE_MESH_LIGHT_CONTROL_TIME_FADE,                                  0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_FADE_ON,                               0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO,                     0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL,                   0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_OCCUPANCY_DELAY,                       0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_PROLONG,                               0x03 },
    { BLE_MESH_LIGHT_CONTROL_TIME_RUN_ON,                                0x03 },
    { BLE_MESH_LUMEN_MAINTENANCE_FACTOR,                                 0x01 },
    { BLE_MESH_LUMINOUS_EFFICACY,                                        0x02 },
    { BLE_MESH_LUMINOUS_ENERGY_SINCE_TURN_ON,                            0x04 },
    { BLE_MESH_LUMINOUS_EXPOSURE,                                        0x04 },
    { BLE_MESH_LUMINOUS_FLUX_RANGE,                                      0x04 },
    { BLE_MESH_MOTION_SENSED,                                            0x01 },
    { BLE_MESH_MOTION_THRESHOLD,                                         0x01 },
    { BLE_MESH_OPEN_CIRCUIT_EVENT_STATISTICS,                            0x06 },
    { BLE_MESH_OUTDOOR_STATISTICAL_VALUES,                               0x05 },
    { BLE_MESH_OUTPUT_CURRENT_RANGE,                                     0x04 },
    { BLE_MESH_OUTPUT_CURRENT_STATISTICS,                                0x09 },
    { BLE_MESH_OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION,                      0x01 },
    { BLE_MESH_OUTPUT_VOLTAGE_RANGE,                                     0x06 },
    { BLE_MESH_OUTPUT_VOLTAGE_STATISTICS,                                0x09 },
    { BLE_MESH_OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS,              0x06 },
    { BLE_MESH_PEOPLE_COUNT,                                             0x02 },
    { BLE_MESH_PRESENCE_DETECTED,                                        0x01 },
    { BLE_MESH_PRESENT_AMBIENT_LIGHT_LEVEL,                              0x04 },
    { BLE_MESH_PRESENT_AMBIENT_TEMPERATURE,                              0x01 },
    { BLE_MESH_PRESENT_CIE_1931_CHROMATICITY,                            0x04 },
    { BLE_MESH_PRESENT_CORRELATED_COLOR_TEMPERATURE,                     0x02 },
    { BLE_MESH_PRESENT_DEVICE_INPUT_POWER,                               0x04 },
    { BLE_MESH_PRESENT_DEVICE_OPERATING_EFFICIENCY,                      0x01 },
    { BLE_MESH_PRESENT_DEVICE_OPERATING_TEMPERATURE,                     0x02 },
    { BLE_MESH_PRESENT_ILLUMINANCE,                                      0x04 },
    { BLE_MESH_PRESENT_INDOOR_AMBIENT_TEMPERATURE,                       0x01 },
    { BLE_MESH_PRESENT_INPUT_CURRENT,                                    0x02 },
    { BLE_MESH_PRESENT_INPUT_RIPPLE_VOLTAGE,                             0x01 },
    { BLE_MESH_PRESENT_INPUT_VOLTAGE,                                    0x02 },
    { BLE_MESH_PRESENT_LUMINOUS_FLUX,                                    0x02 },
    { BLE_MESH_PRESENT_OUTDOOR_AMBIENT_TEMPERATURE,                      0x01 },
    { BLE_MESH_PRESENT_OUTPUT_CURRENT,                                   0x02 },
    { BLE_MESH_PRESENT_OUTPUT_VOLTAGE,                                   0x02 },
    { BLE_MESH_PRESENT_PLANCKIAN_DISTANCE,                               0x02 },
    { BLE_MESH_PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE,                   0x01 },
    { BLE_MESH_RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY,            0x06 },
    { BLE_MESH_RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE,         0x05 },
    { BLE_MESH_RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE,           0x09 },
    { BLE_MESH_RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE, 0x04 },
    { BLE_MESH_RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE, 0x05 },
    { BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE,               0x05 },
    { BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE,               0x05 },
    { BLE_MESH_SHORT_CIRCUIT_EVENT_STATISTICS,                           0x06 },
    { BLE_MESH_TIME_SINCE_MOTION_SENSED,                                 0x02 },
    { BLE_MESH_TIME_SINCE_PRESENCE_DETECTED,                             0x02 },
    { BLE_MESH_TOTAL_DEVICE_ENERGY_USE,                                  0x04 },
    { BLE_MESH_TOTAL_DEVICE_OFF_ON_CYCLES,                               0x04 },
    { BLE_MESH_TOTAL_DEVICE_POWER_ON_CYCLES,                             0x04 },
    { BLE_MESH_TOTAL_DEVICE_POWER_ON_TIME,                               0x04 },
    { BLE_MESH_TOTAL_DEVICE_RUNTIME,                                     0x04 },
    { BLE_MESH_TOTAL_LIGHT_EXPOSURE_TIME,                                0x04 },
    { BLE_MESH_TOTAL_LUMINOUS_ENERGY,                                    0x04 },
};

u8_t bt_mesh_get_dev_prop_len(u16_t prop_id)
{
    if (prop_id > BLE_MESH_TOTAL_LUMINOUS_ENERGY) {
        BT_ERR("%s, Unknown Device Property ID 0x%04x", __func__, prop_id);
        return UINT8_MAX;
    }

    return device_properties[prop_id].len;
}
