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

#ifndef _DEVICE_PROPERTY_H_
#define _DEVICE_PROPERTY_H_

#include "mesh_types.h"

/**
 * BLE Mesh Device Properties.
 *
 * Name                                                        Type                                                                                ID          Characteristic                               Type Size
 * Average Ambient Temperature In A Period Of Day              org.bluetooth.property.average_ambient_temperature_in_a_period_of_day               0x0001      Temperature 8 In A Period Of Day             3
 * Average Input Current                                       org.bluetooth.property.average_input_current                                        0x0002      Average Current                              3
 * Average Input Voltage                                       org.bluetooth.property.average_input_voltage                                        0x0003      Average Voltage                              3
 * Average Output Current                                      org.bluetooth.property.average_output_current                                       0x0004      Average Current                              3
 * Average Output Voltage                                      org.bluetooth.property.average_output_voltage                                       0x0005      Average Voltage                              3
 * Center Beam Intensity At Full Power                         org.bluetooth.property.center_beam_intensity_at_full_power                          0x0006      Luminous Intensity                           2
 * Chromaticity Tolerance                                      org.bluetooth.property.chromaticity_tolerance                                       0x0007      Chromaticity Tolerance                       1
 * Color Rendering Index R9                                    org.bluetooth.property.color_rendering_index_r9                                     0x0008      Cie 13.3-1995 Color Rendering Index          1
 * Color Rendering Index Ra                                    org.bluetooth.property.color_rendering_index_ra                                     0x0009      Cie 13.3-1995 Color Rendering Index          1
 * Device Appearance                                           org.bluetooth.property.device_appearance                                            0x000A      Gap.Appearance                               2
 * Device Country Of Origin                                    org.bluetooth.property.device_country_of_origin                                     0x000B      Country Code                                 2
 * Device Date Of Manufacture                                  org.bluetooth.property.device_date_of_manufacture                                   0x000C      Date Utc                                     4
 * Device Energy Use Since Turn On                             org.bluetooth.property.device_energy_use_since_turn_on                              0x000D      Energy                                       4
 * Device Firmware Revision                                    org.bluetooth.property.device_firmware_revision                                     0x000E      Fixed String 8                               8
 * Device Global Trade Item Number                             org.bluetooth.property.device_global_trade_item_number                              0x000F      Global Trade Item Number                     8
 * Device Hardware Revision                                    org.bluetooth.property.device_hardware_revision                                     0x0010      Fixed String 16                              16
 * Device Manufacturer Name                                    org.bluetooth.property.device_manufacturer_name                                     0x0011      Fixed String 36                              36
 * Device Model Number                                         org.bluetooth.property.device_model_number                                          0x0012      Fixed String 24                              24
 * Device Operating Temperature Range Specification            org.bluetooth.property.device_operating_temperature_range_specification             0x0013      Temperature Range                            4
 * Device Operating Temperature Statistical Values             org.bluetooth.property.device_operating_temperature_statistical_values              0x0014      Temperature Statistics                       9
 * Device Over Temperature Event Statistics                    org.bluetooth.property.device_over_temperature_event_statistics                     0x0015      Event Statistics                             6
 * Device Power Range Specification                            org.bluetooth.property.device_power_range_specification                             0x0016      Power Specification                          12
 * Device Runtime Since Turn On                                org.bluetooth.property.device_runtime_since_turn_on                                 0x0017      Time Hour 24                                 4
 * Device Runtime Warranty                                     org.bluetooth.property.device_runtime_warranty                                      0x0018      Time Hour 24                                 4
 * Device Serial Number                                        org.bluetooth.property.device_serial_number                                         0x0019      Fixed String 16                              16
 * Device Software Revision                                    org.bluetooth.property.device_software_revision                                     0x001A      Fixed String 8                               8
 * Device Under Temperature Event Statistics                   org.bluetooth.property.device_under_temperature_event_statistics                    0x001B      Event Statistics                             6
 * Indoor Ambient Temperature Statistical Values               org.bluetooth.property.indoor_ambient_temperature_statistical_values                0x001C      Temperature 8 Statistics                     5
 * Initial CIE 1931 Chromaticity Coordinates                   org.bluetooth.property.initial_cie_1931_chromaticity_coordinates                    0x001D      Chromaticity Coordinates                     4
 * Initial Correlated Color Temperature                        org.bluetooth.property.initial_correlated_color_temperature                         0x001E      Correlated Color Temperature                 2
 * Initial Luminous Flux                                       org.bluetooth.property.initial_luminous_flux                                        0x001F      Luminous Flux                                2
 * Initial Planckian Distance                                  org.bluetooth.property.initial_planckian_distance                                   0x0020      Chromatic Distance From Planckian            2
 * Input Current Range Specification                           org.bluetooth.property.input_current_range_specification                            0x0021      Electric Current Specification               6
 * Input Current Statistics                                    org.bluetooth.property.input_current_statistics                                     0x0022      Electric Current Statistics                  9
 * Input Over Current Event Statistics                         org.bluetooth.property.input_over_current_event_statistics                          0x0023      Event Statistics                             6
 * Input Over Ripple Voltage Event Statistics                  org.bluetooth.property.input_over_ripple_voltage_event_statistics                   0x0024      Event Statistics                             6
 * Input Over Voltage Event Statistics                         org.bluetooth.property.input_over_voltage_event_statistics                          0x0025      Event Statistics                             6
 * Input Under Current Event Statistics                        org.bluetooth.property.input_under_current_event_statistics                         0x0026      Event Statistics                             6
 * Input Under Voltage Event Statistics                        org.bluetooth.property.input_under_voltage_event_statistics                         0x0027      Event Statistics                             6
 * Input Voltage Range Specification                           org.bluetooth.property.input_voltage_range_specification                            0x0028      Voltage Specification                        6
 * Input Voltage Ripple Specification                          org.bluetooth.property.input_voltage_ripple_specification                           0x0029      Percentage 8                                 1
 * Input Voltage Statistics                                    org.bluetooth.property.input_voltage_statistics                                     0x002A      Voltage Statistics                           9
 * Light Control Ambient LuxLevel On                           org.bluetooth.property.light_control_ambient_luxlevel_on                            0x002B      Illuminance                                  4
 * Light Control Ambient LuxLevel Prolong                      org.bluetooth.property.light_control_ambient_luxlevel_prolong                       0x002C      Illuminance                                  4
 * Light Control Ambient LuxLevel Standby                      org.bluetooth.property.light_control_ambient_luxlevel_standby                       0x002D      Illuminance                                  4
 * Light Control Lightness On                                  org.bluetooth.property.light_control_lightness_on                                   0x002E      Perceived Lightness                          2
 * Light Control Lightness Prolong                             org.bluetooth.property.light_control_lightness_prolong                              0x002F      Perceived Lightness                          2
 * Light Control Lightness Standby                             org.bluetooth.property.light_control_lightness_standby                              0x0030      Perceived Lightness                          2
 * Light Control Regulator Accuracy                            org.bluetooth.property.light_control_regulator_accuracy                             0x0031      Percentage 8                                 1
 * Light Control Regulator Kid                                 org.bluetooth.property.light_control_regulator_kid                                  0x0032      Coefficient                                  4
 * Light Control Regulator Kiu                                 org.bluetooth.property.light_control_regulator_kiu                                  0x0033      Coefficient                                  4
 * Light Control Regulator Kpd                                 org.bluetooth.property.light_control_regulator_kpd                                  0x0034      Coefficient                                  4
 * Light Control Regulator Kpu                                 org.bluetooth.property.light_control_regulator_kpu                                  0x0035      Coefficient                                  4
 * Light Control Time Fade                                     org.bluetooth.property.light_control_time_fade                                      0x0036      Time Millisecond 24                          4
 * Light Control Time Fade On                                  org.bluetooth.property.light_control_time_fade_on                                   0x0037      Time Millisecond 24                          4
 * Light Control Time Fade Standby Auto                        org.bluetooth.property.light_control_time_fade_standby_auto                         0x0038      Time Millisecond 24                          4
 * Light Control Time Fade Standby Manual                      org.bluetooth.property.light_control_time_fade_standby_manual                       0x0039      Time Millisecond 24                          4
 * Light Control Time Occupancy Delay                          org.bluetooth.property.light_control_time_occupancy_delay                           0x003A      Time Millisecond 24                          4
 * Light Control Time Prolong                                  org.bluetooth.property.light_control_time_prolong                                   0x003B      Time Millisecond 24                          4
 * Light Control Time Run On                                   org.bluetooth.property.light_control_time_run_on                                    0x003C      Time Millisecond 24                          4
 * Lumen Maintenance Factor                                    org.bluetooth.property.lumen_maintenance_factor                                     0x003D      Percentage 8                                 1
 * Luminous Efficacy                                           org.bluetooth.property.luminous_efficacy                                            0x003E      Luminous Efficacy                            2
 * Luminous Energy Since Turn On                               org.bluetooth.property.luminous_energy_since_turn_on                                0x003F      Luminous Energy                              4
 * Luminous Exposure                                           org.bluetooth.property.luminous_exposure                                            0x0040      Luminous Exposure                            4
 * Luminous Flux Range                                         org.bluetooth.property.luminous_flux_range                                          0x0041      Luminous Flux Range                          4
 * Motion Sensed                                               org.bluetooth.property.motion_sensed                                                0x0042      Percentage 8                                 1
 * Motion Threshold                                            org.bluetooth.property.motion_threshold                                             0x0043      Percentage 8                                 1
 * Open Circuit Event Statistics                               org.bluetooth.property.open_circuit_event_statistics                                0x0044      Event Statistics                             6
 * Outdoor Statistical Values                                  org.bluetooth.property.outdoor_statistical_values                                   0x0045      Temperature 8 Statistics                     5
 * Output Current Range                                        org.bluetooth.property.output_current_range                                         0x0046      Electric Current Range                       4
 * Output Current Statistics                                   org.bluetooth.property.output_current_statistics                                    0x0047      Electric Current Statistics                  9
 * Output Ripple Voltage Specification                         org.bluetooth.property.output_ripple_voltage_specification                          0x0048      Percentage 8                                 1
 * Output Voltage Range                                        org.bluetooth.property.output_voltage_range                                         0x0049      Voltage Specification                        6
 * Output Voltage Statistics                                   org.bluetooth.property.output_voltage_statistics                                    0x004A      Voltage Statistics                           9
 * Over Output Ripple Voltage Event Statistics                 org.bluetooth.property.over_output_ripple_voltage_event_statistics                  0x004B      Event Statistics                             6
 * People Count                                                org.bluetooth.property.people_count                                                 0x004C      Count 16                                     2
 * Presence Detected                                           org.bluetooth.property.presence_detected                                            0x004D      Boolean                                      1
 * Present Ambient Light Level                                 org.bluetooth.property.present_ambient_light_level                                  0x004E      Illuminance                                  4
 * Present Ambient Temperature                                 org.bluetooth.property.present_ambient_temperature                                  0x004F      Temperature 8                                1
 * Present CIE 1931 Chromaticity Coordinates                   org.bluetooth.property.present_cie_1931_chromaticity                                0x0050      Chromaticity Coordinates                     4
 * Present Correlated Color Temperature                        org.bluetooth.property.present_correlated_color_temperature                         0x0051      Correlated Color Temperature                 2
 * Present Device Input Power                                  org.bluetooth.property.present_device_input_power                                   0x0052      Power                                        4
 * Present Device Operating Efficiency                         org.bluetooth.property.present_device_operating_efficiency                          0x0053      Percentage 8                                 1
 * Present Device Operating Temperature                        org.bluetooth.property.present_device_operating_temperature                         0x0054      Temperature                                  2
 * Present Illuminance                                         org.bluetooth.property.present_illuminance                                          0x0055      Illuminance                                  4
 * Present Indoor Ambient Temperature                          org.bluetooth.property.present_indoor_ambient_temperature                           0x0056      Temperature 8                                1
 * Present Input Current                                       org.bluetooth.property.present_input_current                                        0x0057      Electric Current                             2
 * Present Input Ripple Voltage                                org.bluetooth.property.present_input_ripple_voltage                                 0x0058      Percentage 8                                 1
 * Present Input Voltage                                       org.bluetooth.property.present_input_voltage                                        0x0059      Voltage                                      2
 * Present Luminous Flux                                       org.bluetooth.property.present_luminous_flux                                        0x005A      Luminous Flux                                2
 * Present Outdoor Ambient Temperature                         org.bluetooth.property.present_outdoor_ambient_temperature                          0x005B      Temperature 8                                1
 * Present Output Current                                      org.bluetooth.property.present_output_current                                       0x005C      Electric Current                             2
 * Present Output Voltage                                      org.bluetooth.property.present_output_voltage                                       0x005D      Voltage                                      2
 * Present Planckian Distance                                  org.bluetooth.property.present_planckian_distance                                   0x005E      Chromatic Distance From Planckian            2
 * Present Relative Output Ripple Voltage                      org.bluetooth.property.present_relative_output_ripple_voltage                       0x005F      Percentage 8                                 1
 * Relative Device Energy Use In A Period Of Day               org.bluetooth.property.relative_device_energy_use_in_a_period_of_day                0x0060      Energy In A Period Of Day                    6
 * Relative Device Runtime In A Generic Level Range            org.bluetooth.property.relative_device_runtime_in_a_generic_level_range             0x0061      Relative Runtime In A Generic Level Range    5
 * Relative Exposure Time In An Illuminance Range              org.bluetooth.property.relative_exposure_time_in_an_illuminance_range               0x0062      Relative Value In An Illuminance Range       9
 * Relative Runtime In A Correlated Color Temperature Range    org.bluetooth.property.relative_runtime_in_a_correlated_color_temperature_range     0x0063      Luminous Energy                              4
 * Relative Runtime In A Device Operating Temperature Range    org.bluetooth.property.relative_runtime_in_a_device_operating_temperature_range     0x0064      Relative Value In A Temperature Range        5
 * Relative Runtime In An Input Current Range                  org.bluetooth.property.relative_runtime_in_an_input_current_range                   0x0065      Relative Runtime In A Current Range          5
 * Relative Runtime In An Input Voltage Range                  org.bluetooth.property.relative_runtime_in_an_input_voltage_range                   0x0066      Relative Value In A Voltage Range            5
 * Short Circuit Event Statistics                              org.bluetooth.property.short_circuit_event_statistics                               0x0067      Event Statistics                             6
 * Time Since Motion Sensed                                    org.bluetooth.property.time_since_motion_sensed                                     0x0068      Time Second 16                               2
 * Time Since Presence Detected                                org.bluetooth.property.time_since_presence_detected                                 0x0069      Time Second 16                               2
 * Total Device Energy Use                                     org.bluetooth.property.total_device_energy_use                                      0x006A      Energy                                       4
 * Total Device Off On Cycles                                  org.bluetooth.property.total_device_off_on_cycles                                   0x006B      Count 24                                     4
 * Total Device Power On Cycles                                org.bluetooth.property.total_device_power_on_cycles                                 0x006C      Count 24                                     4
 * Total Device Power On Time                                  org.bluetooth.property.total_device_power_on_time                                   0x006D      Time Hour 24                                 4
 * Total Device Runtime                                        org.bluetooth.property.total_device_runtime                                         0x006E      Time Hour 24                                 4
 * Total Light Exposure Time                                   org.bluetooth.property.total_light_exposure_time                                    0x006F      Time Hour 24                                 4
 * Total Luminous Energy                                       org.bluetooth.property.total_luminous_energy                                        0x0070      Luminous Energy                              4
 */

/**
 * Characteristics referenced by BLE Mesh Device Properties.
 *
 * Name                                        Uniform Type Identifier                                                     Assigned Number     Specification Level
 * Average Current                             org.bluetooth.characteristic.average_current                                2AE0                Adopted
 * Average Voltage                             org.bluetooth.characteristic.average_voltage                                2AE1                Adopted
 * Boolean                                     org.bluetooth.characteristic.boolean                                        2AE2                Adopted
 * Chromatic Distance From Planckian           org.bluetooth.characteristic.chromatic_distance_from_planckian              2AE3                Adopted
 * Chromaticity Coordinate                     org.bluetooth.characteristic.chromaticity_coordinate                        2B1C                Adopted
 * Chromaticity Coordinates                    org.bluetooth.characteristic.chromaticity_coordinates                       2AE4                Adopted
 * Chromaticity In CCT And Duv Values          org.bluetooth.characteristic.chromaticity_in_cct_and_duv_values             2AE5                Adopted
 * Chromaticity Tolerance                      org.bluetooth.characteristic.chromaticity_tolerance                         2AE6                Adopted
 * CIE 13.3-1995 Color Rendering Index         org.bluetooth.characteristic.cie_13.3-1995_color_rendering_index            2AE7                Adopted
 * Coefficient                                 org.bluetooth.characteristic.coefficient                                    2AE8                Adopted
 * Correlated Color Temperature                org.bluetooth.characteristic.correlated_color_temperature                   2AE9                Adopted
 * Count 16                                    org.bluetooth.characteristic.count_16                                       2AEA                Adopted
 * Count 24                                    org.bluetooth.characteristic.count_24                                       2AEB                Adopted
 * Country Code                                org.bluetooth.characteristic.country_code                                   2AEC                Adopted
 * Date UTC                                    org.bluetooth.characteristic.date_utc                                       2AED                Adopted
 * Electric Current                            org.bluetooth.characteristic.electric_current                               2AEE                Adopted
 * Electric Current Range                      org.bluetooth.characteristic.electric_current_range                         2AEF                Adopted
 * Electric Current Specification              org.bluetooth.characteristic.electric_current_specification                 2AF0                Adopted
 * Electric Current Statistics                 org.bluetooth.characteristic.electric_current_statistics                    2AF1                Adopted
 * Energy                                      org.bluetooth.characteristic.energy                                         2AF2                Adopted
 * Energy In A Period Of Day                   org.bluetooth.characteristic.energy_in_a_period_of_day                      2AF3                Adopted
 * Event Statistics                            org.bluetooth.characteristic.event_statistics                               2AF4                Adopted
 * Fixed String 16                             org.bluetooth.characteristic.fixed_string_16                                2AF5                Adopted
 * Fixed String 24                             org.bluetooth.characteristic.fixed_string_24                                2AF6                Adopted
 * Fixed String 36                             org.bluetooth.characteristic.fixed_string_36                                2AF7                Adopted
 * Fixed String 8                              org.bluetooth.characteristic.fixed_string_8                                 2AF8                Adopted
 * Generic Level                               org.bluetooth.characteristic.generic_level                                  2AF9                Adopted
 * Global Trade Item Number                    org.bluetooth.characteristic.global_trade_item_number                       2AFA                Adopted
 * Illuminance                                 org.bluetooth.characteristic.illuminance                                    2AFB                Adopted
 * Luminous Efficacy                           org.bluetooth.characteristic.luminous_efficacy                              2AFC                Adopted
 * Luminous Energy                             org.bluetooth.characteristic.luminous_energy                                2AFD                Adopted
 * Luminous Exposure                           org.bluetooth.characteristic.luminous_exposure                              2AFE                Adopted
 * Luminous Flux                               org.bluetooth.characteristic.luminous_flux                                  2AFF                Adopted
 * Luminous Flux Range                         org.bluetooth.characteristic.luminous_flux_range                            2B00                Adopted
 * Luminous Intensity                          org.bluetooth.characteristic.luminous_intensity                             2B01                Adopted
 * Mass Flow                                   org.bluetooth.characteristic.mass_flow                                      2B02                Adopted
 * Mesh Provisioning Data In                   org.bluetooth.characteristic.mesh_provisioning_data_in                      2ADB                Adopted
 * Mesh Provisioning Data Out                  org.bluetooth.characteristic.mesh_provisioning_data_out                     2ADC                Adopted
 * Mesh Proxy Data In                          org.bluetooth.characteristic.mesh_proxy_data_in                             2ADD                Adopted
 * Mesh Proxy Data Out                         org.bluetooth.characteristic.mesh_proxy_data_out                            2ADE                Adopted
 * Perceived Lightness                         org.bluetooth.characteristic.perceived_lightness                            2B03                Adopted
 * Percentage 8                                org.bluetooth.characteristic.percentage_8                                   2B04                Adopted
 * Power                                       org.bluetooth.characteristic.power                                          2B05                Adopted
 * Power Specification                         org.bluetooth.characteristic.power_specification                            2B06                Adopted
 * Relative Runtime In A Current Range         org.bluetooth.characteristic.relative_runtime_in_a_current_range            2B07                Adopted
 * Relative Runtime In A Generic Level Range   org.bluetooth.characteristic.relative_runtime_in_a_generic_level_range      2B08                Adopted
 * Relative Value In A Period of Day           org.bluetooth.characteristic.relative_value_in_a_period_of_day              2B0B                Adopted
 * Relative Value In A Temperature Range       org.bluetooth.characteristic.relative_value_in_a_temperature_range          2B0C                Adopted
 * Relative Value In A Voltage Range           org.bluetooth.characteristic.relative_value_in_a_voltage_range              2B09                Adopted
 * Relative Value In An Illuminance Range      org.bluetooth.characteristic.relative_value_in_an_illuminance_range         2B0A                Adopted
 * Temperature 8                               org.bluetooth.characteristic.temperature_8                                  2B0D                Adopted
 * Temperature 8 In A Period Of Day            org.bluetooth.characteristic.temperature_8_in_a_period_of_day               2B0E                Adopted
 * Temperature 8 Statistics                    org.bluetooth.characteristic.temperature_8_statistics                       2B0F                Adopted
 * Temperature Range                           org.bluetooth.characteristic.temperature_range                              2B10                Adopted
 * Temperature Statistics                      org.bluetooth.characteristic.temperature_statistics                         2B11                Adopted
 * Time Decihour 8                             org.bluetooth.characteristic.time_decihour_8                                2B12                Adopted
 * Time Exponential 8                          org.bluetooth.characteristic.time_exponential_8                             2B13                Adopted
 * Time Hour 24                                org.bluetooth.characteristic.time_hour_24                                   2B14                Adopted
 * Time Millisecond 24                         org.bluetooth.characteristic.time_millisecond_24                            2B15                Adopted
 * Time Second 16                              org.bluetooth.characteristic.time_second_16                                 2B16                Adopted
 * Time Second 8                               org.bluetooth.characteristic.time_second_8                                  2B17                Adopted
 * Voltage                                     org.bluetooth.characteristic.voltage                                        2B18                Adopted
 * Voltage Specification                       org.bluetooth.characteristic.voltage_specification                          2B19                Adopted
 * Voltage Statistics                          org.bluetooth.characteristic.voltage_statistics                             2B1A                Adopted
 * Volume Flow                                 org.bluetooth.characteristic.volume_flow                                    2B1B                Adopted
 */

/**
 * @brief BLE Mesh Device Property IDs
 */
#define BLE_MESH_AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY             0x0001
#define BLE_MESH_AVERAGE_INPUT_CURRENT                                      0x0002
#define BLE_MESH_AVERAGE_INPUT_VOLTAGE                                      0x0003
#define BLE_MESH_AVERAGE_OUTPUT_CURRENT                                     0x0004
#define BLE_MESH_AVERAGE_OUTPUT_VOLTAGE                                     0x0005
#define BLE_MESH_CENTER_BEAM_INTENSITY_AT_FULL_POWER                        0x0006
#define BLE_MESH_CHROMATICITY_TOLERANCE                                     0x0007
#define BLE_MESH_COLOR_RENDERING_INDEX_R9                                   0x0008
#define BLE_MESH_COLOR_RENDERING_INDEX_RA                                   0x0009
#define BLE_MESH_DEVICE_APPEARANCE                                          0x000A
#define BLE_MESH_DEVICE_COUNTRY_OF_ORIGIN                                   0x000B
#define BLE_MESH_DEVICE_DATE_OF_MANUFACTURE                                 0x000C
#define BLE_MESH_DEVICE_ENERGY_USE_SINCE_TURN_ON                            0x000D
#define BLE_MESH_DEVICE_FIRMWARE_REVISION                                   0x000E
#define BLE_MESH_DEVICE_GLOBAL_TRADE_ITEM_NUMBER                            0x000F
#define BLE_MESH_DEVICE_HARDWARE_REVISION                                   0x0010
#define BLE_MESH_DEVICE_MANUFACTURER_NAME                                   0x0011
#define BLE_MESH_DEVICE_MODEL_NUMBER                                        0x0012
#define BLE_MESH_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION           0x0013
#define BLE_MESH_DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES            0x0014
#define BLE_MESH_DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS                   0x0015
#define BLE_MESH_DEVICE_POWER_RANGE_SPECIFICATION                           0x0016
#define BLE_MESH_DEVICE_RUNTIME_SINCE_TURN_ON                               0x0017
#define BLE_MESH_DEVICE_RUNTIME_WARRANTY                                    0x0018
#define BLE_MESH_DEVICE_SERIAL_NUMBER                                       0x0019
#define BLE_MESH_DEVICE_SOFTWARE_REVISION                                   0x001A
#define BLE_MESH_DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS                  0x001B
#define BLE_MESH_INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES              0x001C
#define BLE_MESH_INITIAL_CIE_1931_CHROMATICITY_COORDINATES                  0x001D
#define BLE_MESH_INITIAL_CORRELATED_COLOR_TEMPERATURE                       0x001E
#define BLE_MESH_INITIAL_LUMINOUS_FLUX                                      0x001F
#define BLE_MESH_INITIAL_PLANCKIAN_DISTANCE                                 0x0020
#define BLE_MESH_INPUT_CURRENT_RANGE_SPECIFICATION                          0x0021
#define BLE_MESH_INPUT_CURRENT_STATISTICS                                   0x0022
#define BLE_MESH_INPUT_OVER_CURRENT_EVENT_STATISTICS                        0x0023
#define BLE_MESH_INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS                 0x0024
#define BLE_MESH_INPUT_OVER_VOLTAGE_EVENT_STATISTICS                        0x0025
#define BLE_MESH_INPUT_UNDER_CURRENT_EVENT_STATISTICS                       0x0026
#define BLE_MESH_INPUT_UNDER_VOLTAGE_EVENT_STATISTICS                       0x0027
#define BLE_MESH_INPUT_VOLTAGE_RANGE_SPECIFICATION                          0x0028
#define BLE_MESH_INPUT_VOLTAGE_RIPPLE_SPECIFICATION                         0x0029
#define BLE_MESH_INPUT_VOLTAGE_STATISTICS                                   0x002A
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON                          0x002B
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG                     0x002C
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY                     0x002D
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_ON                                 0x002E
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_PROLONG                            0x002F
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_STANDBY                            0x0030
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_ACCURACY                           0x0031
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KID                                0x0032
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KIU                                0x0033
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KPD                                0x0034
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KPU                                0x0035
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE                                    0x0036
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_ON                                 0x0037
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO                       0x0038
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL                     0x0039
#define BLE_MESH_LIGHT_CONTROL_TIME_OCCUPANCY_DELAY                         0x003A
#define BLE_MESH_LIGHT_CONTROL_TIME_PROLONG                                 0x003B
#define BLE_MESH_LIGHT_CONTROL_TIME_RUN_ON                                  0x003C
#define BLE_MESH_LUMEN_MAINTENANCE_FACTOR                                   0x003D
#define BLE_MESH_LUMINOUS_EFFICACY                                          0x003E
#define BLE_MESH_LUMINOUS_ENERGY_SINCE_TURN_ON                              0x003F
#define BLE_MESH_LUMINOUS_EXPOSURE                                          0x0040
#define BLE_MESH_LUMINOUS_FLUX_RANGE                                        0x0041
#define BLE_MESH_MOTION_SENSED                                              0x0042
#define BLE_MESH_MOTION_THRESHOLD                                           0x0043
#define BLE_MESH_OPEN_CIRCUIT_EVENT_STATISTICS                              0x0044
#define BLE_MESH_OUTDOOR_STATISTICAL_VALUES                                 0x0045
#define BLE_MESH_OUTPUT_CURRENT_RANGE                                       0x0046
#define BLE_MESH_OUTPUT_CURRENT_STATISTICS                                  0x0047
#define BLE_MESH_OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION                        0x0048
#define BLE_MESH_OUTPUT_VOLTAGE_RANGE                                       0x0049
#define BLE_MESH_OUTPUT_VOLTAGE_STATISTICS                                  0x004A
#define BLE_MESH_OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS                0x004B
#define BLE_MESH_PEOPLE_COUNT                                               0x004C
#define BLE_MESH_PRESENCE_DETECTED                                          0x004D
#define BLE_MESH_PRESENT_AMBIENT_LIGHT_LEVEL                                0x004E
#define BLE_MESH_PRESENT_AMBIENT_TEMPERATURE                                0x004F
#define BLE_MESH_PRESENT_CIE_1931_CHROMATICITY                              0x0050
#define BLE_MESH_PRESENT_CORRELATED_COLOR_TEMPERATURE                       0x0051
#define BLE_MESH_PRESENT_DEVICE_INPUT_POWER                                 0x0052
#define BLE_MESH_PRESENT_DEVICE_OPERATING_EFFICIENCY                        0x0053
#define BLE_MESH_PRESENT_DEVICE_OPERATING_TEMPERATURE                       0x0054
#define BLE_MESH_PRESENT_ILLUMINANCE                                        0x0055
#define BLE_MESH_PRESENT_INDOOR_AMBIENT_TEMPERATURE                         0x0056
#define BLE_MESH_PRESENT_INPUT_CURRENT                                      0x0057
#define BLE_MESH_PRESENT_INPUT_RIPPLE_VOLTAGE                               0x0058
#define BLE_MESH_PRESENT_INPUT_VOLTAGE                                      0x0059
#define BLE_MESH_PRESENT_LUMINOUS_FLUX                                      0x005A
#define BLE_MESH_PRESENT_OUTDOOR_AMBIENT_TEMPERATURE                        0x005B
#define BLE_MESH_PRESENT_OUTPUT_CURRENT                                     0x005C
#define BLE_MESH_PRESENT_OUTPUT_VOLTAGE                                     0x005D
#define BLE_MESH_PRESENT_PLANCKIAN_DISTANCE                                 0x005E
#define BLE_MESH_PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE                     0x005F
#define BLE_MESH_RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY              0x0060
#define BLE_MESH_RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE           0x0061
#define BLE_MESH_RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE             0x0062
#define BLE_MESH_RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE   0x0063
#define BLE_MESH_RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE   0x0064
#define BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE                 0x0065
#define BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE                 0x0066
#define BLE_MESH_SHORT_CIRCUIT_EVENT_STATISTICS                             0x0067
#define BLE_MESH_TIME_SINCE_MOTION_SENSED                                   0x0068
#define BLE_MESH_TIME_SINCE_PRESENCE_DETECTED                               0x0069
#define BLE_MESH_TOTAL_DEVICE_ENERGY_USE                                    0x006A
#define BLE_MESH_TOTAL_DEVICE_OFF_ON_CYCLES                                 0x006B
#define BLE_MESH_TOTAL_DEVICE_POWER_ON_CYCLES                               0x006C
#define BLE_MESH_TOTAL_DEVICE_POWER_ON_TIME                                 0x006D
#define BLE_MESH_TOTAL_DEVICE_RUNTIME                                       0x006E
#define BLE_MESH_TOTAL_LIGHT_EXPOSURE_TIME                                  0x006F
#define BLE_MESH_TOTAL_LUMINOUS_ENERGY                                      0x0070

/**
 * @brief BLE Mesh Device Property value length
 */
#define BLE_MESH_AVERAGE_AMBIENT_TEMPERATURE_IN_A_PERIOD_OF_DAY_LEN             0x03
#define BLE_MESH_AVERAGE_INPUT_CURRENT_LEN                                      0x03
#define BLE_MESH_AVERAGE_INPUT_VOLTAGE_LEN                                      0x03
#define BLE_MESH_AVERAGE_OUTPUT_CURRENT_LEN                                     0x03
#define BLE_MESH_AVERAGE_OUTPUT_VOLTAGE_LEN                                     0x03
#define BLE_MESH_CENTER_BEAM_INTENSITY_AT_FULL_POWER_LEN                        0x02
#define BLE_MESH_CHROMATICITY_TOLERANCE_LEN                                     0x01
#define BLE_MESH_COLOR_RENDERING_INDEX_R9_LEN                                   0x01
#define BLE_MESH_COLOR_RENDERING_INDEX_RA_LEN                                   0x01
#define BLE_MESH_DEVICE_APPEARANCE_LEN                                          0x02
#define BLE_MESH_DEVICE_COUNTRY_OF_ORIGIN_LEN                                   0x02
#define BLE_MESH_DEVICE_DATE_OF_MANUFACTURE_LEN                                 0x04
#define BLE_MESH_DEVICE_ENERGY_USE_SINCE_TURN_ON_LEN                            0x04
#define BLE_MESH_DEVICE_FIRMWARE_REVISION_LEN                                   0x08
#define BLE_MESH_DEVICE_GLOBAL_TRADE_ITEM_NUMBER_LEN                            0x08
#define BLE_MESH_DEVICE_HARDWARE_REVISION_LEN                                   0x16
#define BLE_MESH_DEVICE_MANUFACTURER_NAME_LEN                                   0x36
#define BLE_MESH_DEVICE_MODEL_NUMBER_LEN                                        0x24
#define BLE_MESH_DEVICE_OPERATING_TEMPERATURE_RANGE_SPECIFICATION_LEN           0x04
#define BLE_MESH_DEVICE_OPERATING_TEMPERATURE_STATISTICAL_VALUES_LEN            0x09
#define BLE_MESH_DEVICE_OVER_TEMPERATURE_EVENT_STATISTICS_LEN                   0x06
#define BLE_MESH_DEVICE_POWER_RANGE_SPECIFICATION_LEN                           0x12
#define BLE_MESH_DEVICE_RUNTIME_SINCE_TURN_ON_LEN                               0x04
#define BLE_MESH_DEVICE_RUNTIME_WARRANTY_LEN                                    0x04
#define BLE_MESH_DEVICE_SERIAL_NUMBER_LEN                                       0x16
#define BLE_MESH_DEVICE_SOFTWARE_REVISION_LEN                                   0x08
#define BLE_MESH_DEVICE_UNDER_TEMPERATURE_EVENT_STATISTICS_LEN                  0x06
#define BLE_MESH_INDOOR_AMBIENT_TEMPERATURE_STATISTICAL_VALUES_LEN              0x05
#define BLE_MESH_INITIAL_CIE_1931_CHROMATICITY_COORDINATES_LEN                  0x04
#define BLE_MESH_INITIAL_CORRELATED_COLOR_TEMPERATURE_LEN                       0x02
#define BLE_MESH_INITIAL_LUMINOUS_FLUX_LEN                                      0x02
#define BLE_MESH_INITIAL_PLANCKIAN_DISTANCE_LEN                                 0x02
#define BLE_MESH_INPUT_CURRENT_RANGE_SPECIFICATION_LEN                          0x06
#define BLE_MESH_INPUT_CURRENT_STATISTICS_LEN                                   0x09
#define BLE_MESH_INPUT_OVER_CURRENT_EVENT_STATISTICS_LEN                        0x06
#define BLE_MESH_INPUT_OVER_RIPPLE_VOLTAGE_EVENT_STATISTICS_LEN                 0x06
#define BLE_MESH_INPUT_OVER_VOLTAGE_EVENT_STATISTICS_LEN                        0x06
#define BLE_MESH_INPUT_UNDER_CURRENT_EVENT_STATISTICS_LEN                       0x06
#define BLE_MESH_INPUT_UNDER_VOLTAGE_EVENT_STATISTICS_LEN                       0x06
#define BLE_MESH_INPUT_VOLTAGE_RANGE_SPECIFICATION_LEN                          0x06
#define BLE_MESH_INPUT_VOLTAGE_RIPPLE_SPECIFICATION_LEN                         0x01
#define BLE_MESH_INPUT_VOLTAGE_STATISTICS_LEN                                   0x09
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_ON_LEN                          0x04
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_PROLONG_LEN                     0x04
#define BLE_MESH_LIGHT_CONTROL_AMBIENT_LUXLEVEL_STANDBY_LEN                     0x04
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_ON_LEN                                 0x02
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_PROLONG_LEN                            0x02
#define BLE_MESH_LIGHT_CONTROL_LIGHTNESS_STANDBY_LEN                            0x02
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_ACCURACY_LEN                           0x01
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KID_LEN                                0x04
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KIU_LEN                                0x04
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KPD_LEN                                0x04
#define BLE_MESH_LIGHT_CONTROL_REGULATOR_KPU_LEN                                0x04
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_LEN                                    0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_ON_LEN                                 0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_AUTO_LEN                       0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_FADE_STANDBY_MANUAL_LEN                     0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_OCCUPANCY_DELAY_LEN                         0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_PROLONG_LEN                                 0x03
#define BLE_MESH_LIGHT_CONTROL_TIME_RUN_ON_LEN                                  0x03
#define BLE_MESH_LUMEN_MAINTENANCE_FACTOR_LEN                                   0x01
#define BLE_MESH_LUMINOUS_EFFICACY_LEN                                          0x02
#define BLE_MESH_LUMINOUS_ENERGY_SINCE_TURN_ON_LEN                              0x04
#define BLE_MESH_LUMINOUS_EXPOSURE_LEN                                          0x04
#define BLE_MESH_LUMINOUS_FLUX_RANGE_LEN                                        0x04
#define BLE_MESH_MOTION_SENSED_LEN                                              0x01
#define BLE_MESH_MOTION_THRESHOLD_LEN                                           0x01
#define BLE_MESH_OPEN_CIRCUIT_EVENT_STATISTICS_LEN                              0x06
#define BLE_MESH_OUTDOOR_STATISTICAL_VALUES_LEN                                 0x05
#define BLE_MESH_OUTPUT_CURRENT_RANGE_LEN                                       0x04
#define BLE_MESH_OUTPUT_CURRENT_STATISTICS_LEN                                  0x09
#define BLE_MESH_OUTPUT_RIPPLE_VOLTAGE_SPECIFICATION_LEN                        0x01
#define BLE_MESH_OUTPUT_VOLTAGE_RANGE_LEN                                       0x06
#define BLE_MESH_OUTPUT_VOLTAGE_STATISTICS_LEN                                  0x09
#define BLE_MESH_OVER_OUTPUT_RIPPLE_VOLTAGE_EVENT_STATISTICS_LEN                0x06
#define BLE_MESH_PEOPLE_COUNT_LEN                                               0x02
#define BLE_MESH_PRESENCE_DETECTED_LEN                                          0x01
#define BLE_MESH_PRESENT_AMBIENT_LIGHT_LEVEL_LEN                                0x04
#define BLE_MESH_PRESENT_AMBIENT_TEMPERATURE_LEN                                0x01
#define BLE_MESH_PRESENT_CIE_1931_CHROMATICITY_LEN                              0x04
#define BLE_MESH_PRESENT_CORRELATED_COLOR_TEMPERATURE_LEN                       0x02
#define BLE_MESH_PRESENT_DEVICE_INPUT_POWER_LEN                                 0x04
#define BLE_MESH_PRESENT_DEVICE_OPERATING_EFFICIENCY_LEN                        0x01
#define BLE_MESH_PRESENT_DEVICE_OPERATING_TEMPERATURE_LEN                       0x02
#define BLE_MESH_PRESENT_ILLUMINANCE_LEN                                        0x04
#define BLE_MESH_PRESENT_INDOOR_AMBIENT_TEMPERATURE_LEN                         0x01
#define BLE_MESH_PRESENT_INPUT_CURRENT_LEN                                      0x02
#define BLE_MESH_PRESENT_INPUT_RIPPLE_VOLTAGE_LEN                               0x01
#define BLE_MESH_PRESENT_INPUT_VOLTAGE_LEN                                      0x02
#define BLE_MESH_PRESENT_LUMINOUS_FLUX_LEN                                      0x02
#define BLE_MESH_PRESENT_OUTDOOR_AMBIENT_TEMPERATURE_LEN                        0x01
#define BLE_MESH_PRESENT_OUTPUT_CURRENT_LEN                                     0x02
#define BLE_MESH_PRESENT_OUTPUT_VOLTAGE_LEN                                     0x02
#define BLE_MESH_PRESENT_PLANCKIAN_DISTANCE_LEN                                 0x02
#define BLE_MESH_PRESENT_RELATIVE_OUTPUT_RIPPLE_VOLTAGE_LEN                     0x01
#define BLE_MESH_RELATIVE_DEVICE_ENERGY_USE_IN_A_PERIOD_OF_DAY_LEN              0x06
#define BLE_MESH_RELATIVE_DEVICE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE_LEN           0x05
#define BLE_MESH_RELATIVE_EXPOSURE_TIME_IN_AN_ILLUMINANCE_RANGE_LEN             0x09
#define BLE_MESH_RELATIVE_RUNTIME_IN_A_CORRELATED_COLOR_TEMPERATURE_RANGE_LEN   0x04
#define BLE_MESH_RELATIVE_RUNTIME_IN_A_DEVICE_OPERATING_TEMPERATURE_RANGE_LEN   0x05
#define BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_CURRENT_RANGE_LEN                 0x05
#define BLE_MESH_RELATIVE_RUNTIME_IN_AN_INPUT_VOLTAGE_RANGE_LEN                 0x05
#define BLE_MESH_SHORT_CIRCUIT_EVENT_STATISTICS_LEN                             0x06
#define BLE_MESH_TIME_SINCE_MOTION_SENSED_LEN                                   0x02
#define BLE_MESH_TIME_SINCE_PRESENCE_DETECTED_LEN                               0x02
#define BLE_MESH_TOTAL_DEVICE_ENERGY_USE_LEN                                    0x04
#define BLE_MESH_TOTAL_DEVICE_OFF_ON_CYCLES_LEN                                 0x04
#define BLE_MESH_TOTAL_DEVICE_POWER_ON_CYCLES_LEN                               0x04
#define BLE_MESH_TOTAL_DEVICE_POWER_ON_TIME_LEN                                 0x04
#define BLE_MESH_TOTAL_DEVICE_RUNTIME_LEN                                       0x04
#define BLE_MESH_TOTAL_LIGHT_EXPOSURE_TIME_LEN                                  0x04
#define BLE_MESH_TOTAL_LUMINOUS_ENERGY_LEN                                      0x04

/**
 * @brief BLE Mesh Device Property referenced Characteristic UUIDs
 */
#define BLE_MESH_UUID_AVERAGE_CURRENT_VAL                               0x2AE0
#define BLE_MESH_UUID_AVERAGE_VOLTAGE_VAL                               0x2AE1
#define BLE_MESH_UUID_BOOLEAN_VAL                                       0x2AE2
#define BLE_MESH_UUID_CHROMATIC_DISTANCE_FROM_PLANCKIAN_VAL             0x2AE3
#define BLE_MESH_UUID_CHROMATICITY_COORDINATE_VAL                       0x2B1C
#define BLE_MESH_UUID_CHROMATICITY_COORDINATES_VAL                      0x2AE4
#define BLE_MESH_UUID_CHROMATICITY_IN_CCT_AND_DUV_VALUES_VAL            0x2AE5
#define BLE_MESH_UUID_CHROMATICITY_TOLERANCE_VAL                        0x2AE6
#define BLE_MESH_UUID_CIE_13_3_1995_COLOR_RENDERING_INDEX_VAL           0x2AE7
#define BLE_MESH_UUID_COEFFICIENT_VAL                                   0x2AE8
#define BLE_MESH_UUID_CORRELATED_COLOR_TEMPERATURE_VAL                  0x2AE9
#define BLE_MESH_UUID_COUNT_16_VAL                                      0x2AEA
#define BLE_MESH_UUID_COUNT_24_VAL                                      0x2AEB
#define BLE_MESH_UUID_COUNTRY_CODE_VAL                                  0x2AEC
#define BLE_MESH_UUID_DATE_UTC_VAL                                      0x2AED
#define BLE_MESH_UUID_ELECTRIC_CURRENT_VAL                              0x2AEE
#define BLE_MESH_UUID_ELECTRIC_CURRENT_RANGE_VAL                        0x2AEF
#define BLE_MESH_UUID_ELECTRIC_CURRENT_SPECIFICATION_VAL                0x2AF0
#define BLE_MESH_UUID_ELECTRIC_CURRENT_STATISTICS_VAL                   0x2AF1
#define BLE_MESH_UUID_ENERGY_VAL                                        0x2AF2
#define BLE_MESH_UUID_ENERGY_IN_A_PERIOD_OF_DAY_VAL                     0x2AF3
#define BLE_MESH_UUID_EVENT_STATISTICS_VAL                              0x2AF4
#define BLE_MESH_UUID_FIXED_STRING_16_VAL                               0x2AF5
#define BLE_MESH_UUID_FIXED_STRING_24_VAL                               0x2AF6
#define BLE_MESH_UUID_FIXED_STRING_36_VAL                               0x2AF7
#define BLE_MESH_UUID_FIXED_STRING_8_VAL                                0x2AF8
#define BLE_MESH_UUID_GENERIC_LEVEL_VAL                                 0x2AF9
#define BLE_MESH_UUID_GLOBAL_TRADE_ITEM_NUMBER_VAL                      0x2AFA
#define BLE_MESH_UUID_ILLUMINANCE_VAL                                   0x2AFB
#define BLE_MESH_UUID_LUMINOUS_EFFICACY_VAL                             0x2AFC
#define BLE_MESH_UUID_LUMINOUS_ENERGY_VAL                               0x2AFD
#define BLE_MESH_UUID_LUMINOUS_EXPOSURE_VAL                             0x2AFE
#define BLE_MESH_UUID_LUMINOUS_FLUX_VAL                                 0x2AFF
#define BLE_MESH_UUID_LUMINOUS_FLUX_RANGE_VAL                           0x2B00
#define BLE_MESH_UUID_LUMINOUS_INTENSITY_VAL                            0x2B01
#define BLE_MESH_UUID_MASS_FLOW_VAL                                     0x2B02
/**
 * The following four have been defined in mesh_uuid.h
 * #define BLE_MESH_UUID_MESH_PROV_DATA_IN_VAL                          0x2ADB
 * #define BLE_MESH_UUID_MESH_PROV_DATA_OUT_VAL                         0x2ADC
 * #define BLE_MESH_UUID_MESH_PROXY_DATA_IN_VAL                         0x2ADD
 * #define BLE_MESH_UUID_MESH_PROXY_DATA_OUT_VAL                        0x2ADE
 */
#define BLE_MESH_UUID_PERCEIVED_LIGHTNESS_VAL                           0x2B03
#define BLE_MESH_UUID_PERCENTAGE_8_VAL                                  0x2B04
#define BLE_MESH_UUID_POWER_VAL                                         0x2B05
#define BLE_MESH_UUID_POWER_SPECIFICATION_VAL                           0x2B06
#define BLE_MESH_UUID_RELATIVE_RUNTIME_IN_A_CURRENT_RANGE_VAL           0x2B07
#define BLE_MESH_UUID_RELATIVE_RUNTIME_IN_A_GENERIC_LEVEL_RANGE_VAL     0x2B08
#define BLE_MESH_UUID_RELATIVE_VALUE_IN_A_PERIOD_OF_DAY_VAL             0x2B0B
#define BLE_MESH_UUID_RELATIVE_VALUE_IN_A_TEMPERATURE_RANGE_VAL         0x2B0C
#define BLE_MESH_UUID_RELATIVE_VALUE_IN_A_VOLTAGE_RANGE_VAL             0x2B09
#define BLE_MESH_UUID_RELATIVE_VALUE_IN_AN_ILLUMINANCE_RANGE_VAL        0x2B0A
#define BLE_MESH_UUID_TEMPERATURE_8_VAL                                 0x2B0D
#define BLE_MESH_UUID_TEMPERATURE_8_IN_A_PERIOD_OF_DAY_VAL              0x2B0E
#define BLE_MESH_UUID_TEMPERATURE_8_STATISTICS_VAL                      0x2B0F
#define BLE_MESH_UUID_TEMPERATURE_RANGE_VAL                             0x2B10
#define BLE_MESH_UUID_TEMPERATURE_STATISTICS_VAL                        0x2B11
#define BLE_MESH_UUID_TIME_DECIHOUR_8_VAL                               0x2B12
#define BLE_MESH_UUID_TIME_EXPONENTIAL_8_VAL                            0x2B13
#define BLE_MESH_UUID_TIME_HOUR_24_VAL                                  0x2B14
#define BLE_MESH_UUID_TIME_MILLISECOND_24_VAL                           0x2B15
#define BLE_MESH_UUID_TIME_SECOND_16_VAL                                0x2B16
#define BLE_MESH_UUID_TIME_SECOND_8_VAL                                 0x2B17
#define BLE_MESH_UUID_VOLTAGE_VAL                                       0x2B18
#define BLE_MESH_UUID_VOLTAGE_SPECIFICATION_VAL                         0x2B19
#define BLE_MESH_UUID_VOLTAGE_STATISTICS_VAL                            0x2B1A
#define BLE_MESH_UUID_VOLUME_FLOW_VAL                                   0x2B1B

/**
 * @brief BLE Mesh Device Property referenced Characteristic Type Definitions
 */

/* Unit is in degrees Celsius with a resolution of 0.01 degrees Celsius. */
typedef s16_t bt_mesh_temperature_t;

typedef u16_t bt_mesh_gap_appearance_t;

/* Mesh Characteristics Type Definitions  */

/* This characteristic represents an electric current.
 * Note: Unit is ampere with a resolution of 0.01.
 *       Minimum value: 0, maximum value: 655.34;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_electric_current_t;

/* The Time Exponential 8 characteristic is used to represent a measure of period of
 * time in seconds.
 * Note: The time duration is given by the value 1.1^(N-64) in seconds, with N being
 *       the raw 8-bit value;
 *       Minimum value: 0.0, maximum value: 73216705;
 *       A raw value of 0x00 represents 0 seconds, and a raw value of 0xFF represents
 *       the total life of the device.
 */
typedef u8_t bt_mesh_time_exponential_8_t;

/* The Voltage characteristic is used to represent a measure of positive electric
 * potential difference in units of volts.
 * Note: Unit is volt with a resolution of 1/64V;
 *       Minimum value: 0.0, maximum value: 1022.0;
 *       A value of 0xFFFF represents 'value is not known'. The minimum representable
 *       value represents the minimum value or lower, the maximum representable value
 *       represents the maximum value or higher.
 */
typedef u16_t bt_mesh_voltage_t;

/* This characteristic aggregates the Electric Current characteristic and instance of
 * the Time Exponential 8 characteristic.
 */
typedef struct __packed average_current {
    bt_mesh_electric_current_t   electric_current;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_average_current_t;

/* This characteristic aggregates the Voltage characteristic and instance of the Time
 * Exponential 8 characateristic.
 */
typedef struct __packed average_voltage {
    bt_mesh_voltage_t            voltage;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_average_voltage_t;

/* The Boolean characteristic defines the predefined Boolean values as an enumeration.
 * Key      |   Value
 * 0        |   False
 * 1        |   True
 * 2 to 255 |   Prohibited
 */
typedef u8_t bt_mesh_boolean_t;

/* The Chromatic Distance From Planckian characteristic represents a distance of a
 * chromaticity coordinate from the Planckian locus in the (u',2/3 v') diagram as
 * defined by ANSI standard C78.377-2008. The distance is positive if the chromaticity
 * coordinate is located above the Planckian locus (i.e. has as higher y value than the
 * Planckian), and negative if it is located below. The distance is only valid within
 * the range from -0.05 to 0.05.
 * Note: Unit is unitless with a resolution of 0.00001;
 *       Minimum value: -0.05, maximum value: 0.05;
 *       A value of 0xFFFF represents 'value is not known';
 *       A value of 0xFFFE represents 'value is not valid'.
 */
typedef s16_t bt_mesh_chromatic_distance_from_planckian_t;

/* This characteristic represents a chromaticity coordinate in a color diagram such as
 * the CIE1931 diagram. It can represent an x or y coordinate.
 * Note: Unit is unitless with a resolution of 1/65535;
 *       Minimum value: 0, maximum value: 1.0.
 */
typedef u16_t bt_mesh_chromaticity_coordinate_t;

/* This characteristic represents a chromaticity coordinate as a tuple with an x and
 * y coordinate.
 */
typedef struct __packed chromaticity_coordinates {
    bt_mesh_chromaticity_coordinate_t chromaticity_x_coordinate;
    bt_mesh_chromaticity_coordinate_t chromaticity_y_coordinate;
} bt_mesh_chromaticity_coordinates_t;

/* The Correlated Color Temperature characteristic is used to represent correlated color
 * temperature in a range from 800 to 65534 Kelvin with a resolution of 1 Kelvin.
 * Note: Unit is Kelvin with a resolution of 1;
 *       Minimum value: 800, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_correlated_color_temperature_t;

/* The Chromaticity In CCT And Duv Values characteristic is a composite characteristic
 * consisting of the Correlated Color Temperature characteristic and the Chromatic
 * Distance From Planckian characteristic.
 */
typedef struct __packed chromaticity_in_cct_and_duv_values {
    bt_mesh_correlated_color_temperature_t      correlated_color_temperature;
    bt_mesh_chromatic_distance_from_planckian_t chromaticity_distance_from_planckian;
} bt_mesh_chromaticity_in_cct_and_duv_values_t;

/* The Chromaticity Tolerance characteristic is a tolerance of a tuple of chromaticity
 * values represented as a value of a radius of a circle in the CIE 1976 (u',v') diagram;
 * value corresponding to the 3-sigma values of the expected chromaticity deviations.
 * Note: Unit is unitless with a resolution of 0.0001;
 *       Minimum value: 0, maximum value: 0.0255.
 */
typedef u8_t bt_mesh_chromaticity_tolerance_t;

/* The CIE 13.3-1995 Color Rendering Index characteristic is a color rendition index value
 * for a color patch as calculated in accordance with the CIE 13.3-1995 standard.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: -128, maximum value: 100.
 */
typedef s8_t bt_mesh_cie_13_3_1995_color_rendering_index_t;

/* The Coefficient characteristic is used to represent a general coefficient value. */
typedef float bt_mesh_coefficient_t;

/* The Count 16 characteristic is used to represent a general count value.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: 0, maximum value 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_count_16_t;

/* The Count 24 characteristic is used to represent a general count value.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: 0, maximum value 16777214;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_count_24_t[3];

/* This characteristic represents a country or dependent areas in accordance with
 * the ISO 3166-1 Numeric standard.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: 0, maximum value: 4095;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_country_code_t;

/* Date as days elapsed since the Epoch (Jan 1, 1970) in the Coordinated Universal
 * Time (UTC) time zone.
 * Note: Unit is a day with a resolution of 1;
 *       Minimum value: 1, maximum value: 16777214;
 *       A value of 0x000000 represents 'value is not known'.
 */
typedef u8_t bt_mesh_date_utc_t[3];

/* This characteristic aggregates two instances of the Electric Current characteristic
 * to represent a range of Electric Current values.
 */
typedef struct __packed electric_current_range {
    bt_mesh_electric_current_t minimum_electric_current_value;
    bt_mesh_electric_current_t maximum_electric_current_value;
} bt_mesh_electric_current_range_t;

/* This characteristic aggregates three instances of the Electric Current characteristic
 * to represent a specification of electric current values.
 */
typedef struct __packed electric_current_specification {
    bt_mesh_electric_current_t minimum_electric_current_value;
    bt_mesh_electric_current_t typical_electric_current_value;
    bt_mesh_electric_current_t maximum_electric_current_value;
} bt_mesh_electric_current_specification_t;

/* This characteristic aggregates four instances of the Electric Current characteristic
 * with a Sensing Duration to represent a set of statistical electric current values.
 */
typedef struct __packed electric_current_statistics {
    bt_mesh_electric_current_t   average_electric_current_value;
    bt_mesh_electric_current_t   standard_electric_current_value;
    bt_mesh_electric_current_t   minimum_electric_current_value;
    bt_mesh_electric_current_t   maximum_electric_current_value;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_electric_current_statistics_t;

/* The Energy characteristic is used to represent a measure of energy in units of
 * kilowatt hours.
 * Note: Unit is kilowatt-hour with a resolution of 1;
 *       Minimum value: 0, maximum value: 16777214;
 *       A value of 0xFFFFFF represents ‘value is not known’.
 */
typedef u8_t bt_mesh_energy_t[3];

/* The Time Decihour 8 characteristic is used to represent a period of time in
 * tenths of an hour.
 * Note: Unit is hour with a resolution of 0.1;
 *       Minimum value: 0.0, maximum value: 24.0;
 *       A value of 0xFF represents 'value is not known'. All other values are Prohibited.
 */
typedef u8_t bt_mesh_time_decihour_8_t;

/* This characteristic aggregates the Energy characteristic, and two instances of
 * the Time Decihour 8 characteristic, to represent energy use in a period of day.
 */
typedef struct __packed energy_in_a_period_of_day {
    bt_mesh_energy_t          energy_value;
    bt_mesh_time_decihour_8_t start_time;
    bt_mesh_time_decihour_8_t end_time;
} bt_mesh_energy_in_a_period_of_day_t;

/* The Time Second 16 characteristic is used to represent a period of time with a
 * unit of 1 second.
 * Note: Unit is second with a resolution of 1;
 *       Minimum value: 0, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_time_second_16_t;

/* This characteristic aggregates the Count 16 characteristic, two instances of the
 * Time Decihour 8 characteristic and an instance of the Sensing Duration characteristic,
 * to represent statistical values of events.
 */
typedef struct __packed event_statistics {
    bt_mesh_count_16_t           number_of_events;
    bt_mesh_time_second_16_t     average_event_duration;
    bt_mesh_time_exponential_8_t time_elapsed_since_last_event;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_event_statistics_t;

/* The Fixed String 16 characteristic represents a 16-octet UTF-8 string. */
typedef char bt_mesh_fixed_string_16_t[16];

/* The Fixed String 24 characteristic represents a 24-octet UTF-8 string. */
typedef char bt_mesh_fixed_string_24_t[24];

/* The Fixed String 36 characteristic represents a 36-octet UTF-8 string. */
typedef char bt_mesh_fixed_string_36_t[36];

/* The Fixed String 8 characteristic represents an 8-octet UTF-8 string. */
typedef char bt_mesh_fixed_string_8_t[8];

/* The Generic Level characteristic represents a general level value of a
 * setting of a device.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: 0, maximum value: 65535.
 */
typedef u16_t bt_mesh_generic_level_t;

/* The Global Trade Item Number characteristic represents an identifier as
 * issued by GS1 General Specifications, which may consist up to 14 digits,
 * and is here represented as a 48-bit unsigned integer.
 */
typedef u8_t bt_mesh_global_trade_item_number_t[6];

/* The Illuminance characteristic is used to represent a measure of illuminance
 * in units of lux.
 * Note: Unit is lux with a resolution of 0.01;
 *       Minimum value: 0, maximum value: 167772.14;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_illuminance_t[3];

/* The Luminous Efficacy characteristic is used to represent a measure of luminous
 * efficacy in units of lumen per watt.
 * Note: Unit is lumen per watt with a resolution of 0.1;
 *       Minimum value: 0, maximum value: 1800;
 *       A value of 0xFFFF represents 'value is not known'. All other values are Prohibited.
 */
typedef u16_t bt_mesh_luminous_efficacy_t;

/* The Luminous Energy characteristic is used to represent a measure of luminous
 * energy in units of lumen hour.
 * Note: Unit is lumen hour with a resolution of 1000;
 *       Minimum value: 0, maximum value: 16777214000;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_luminous_energy_t[3];

/* The Luminous Exposure characteristic is used to represent a measure of luminous
 * exposure in units of lux-hour.
 * Note: Unit is lux hour with a resolution of 1000;
 *       Minimum value: 0, maximum value: 16777214000;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_luminous_exposure_t[3];

/* The Luminous Flux characteristic is used to represent a measure of luminous flux
 * in units of lumen.
 * Note: Unit is lumen with a resolution of 1;
 *       Minimum value: 0, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_luminous_flux_t;

/* This characteristic aggregates two instances of the Luminous Flux characteristic
 * to represent a luminous flux range.
 */
typedef struct __packed luminous_flux_range {
    bt_mesh_luminous_flux_t minimum_luminous_flux;
    bt_mesh_luminous_flux_t maximum_luminous_flux;
} bt_mesh_luminous_flux_range_t;

/* The Luminous Intensity characteristic is used to represent a luminous intensity of
 * a beam of light in units of candela.
 * Note: Unit is candela with a resolution of 1;
 *       Minimum value: 0, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_luminous_intensity_t;

/* The Mass Flow characteristic is used to represent a flow of mass.
 * Note: Unit is gram/second with a resolution of 1;
 *       Minimum value: 0, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_mass_flow_t;

/* The Mesh Provisioning Data In characteristic can be written to send a Proxy PDU
 * message containing Provisioning PDU to the Provisioning Server.
 */
struct mesh_provisioning_data_in {

};

/* The Mesh Provisioning Data Out characteristic can be notified to send a Proxy PDU
 * message containing Provisioning PDU from a Provisioning Server to a Provisioning Client.
 */
struct mesh_provisioning_data_out {

};

/* The Mesh Proxy Data In characteristic is used by the client to send Proxy PDUs to
 * the server.
 */
struct mesh_proxy_data_in {

};

/* The Mesh Proxy Data Out characteristic is used by the server to send Proxy PDUs to
 * the client.
 */
struct mesh_proxy_data_out {

};

/* The Perceived Lightness characteristic is used to represent the perceived lightness
 * of a light.
 * Note: Unit is unitless with a resolution of 1;
 *       Minimum value: 0, maximum value: 65535.
 */
typedef u16_t bt_mesh_perceived_lightness_t;

/* The Percentage 8 characteristic is used to represent a measure of percentage.
 * Note: Unit is a percentage with a resolution of 0.5;
 *       Minimum value: 0, maximum value: 100;
 *       A value of 0xFF represents 'value is not known'. All other values are Prohibited.
 */
typedef u8_t bt_mesh_percentage_8_t;

/* The Power characteristic is used to represent a measure of power in units of watts.
 * Note: Unit is watt with a resolution of 0.1;
 *       Minimum value: 0, maximum value: 1677721.4;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_power_t[3];

/* This characteristic aggregates three instances of the Power characteristic to
 * represent a specification of Power values.
 */
typedef struct __packed power_specification {
    bt_mesh_power_t minimum_power_value;
    bt_mesh_power_t typical_power_value;
    bt_mesh_power_t maximum_power_value;
} bt_mesh_power_specification_t;

/* This characteristic aggregates the Percentage 8 characteristic and two instances of
 * the Electric Current characteristic to represent a relative value in an electric
 * current range.
 */
typedef struct __packed relative_runtime_in_a_current_range {
    bt_mesh_percentage_8_t     relative_runtime_value;
    bt_mesh_electric_current_t minimum_current;
    bt_mesh_electric_current_t maximum_current;
} bt_mesh_relative_runtime_in_a_current_range_t;

/* This characteristic aggregates the Percentage 8 characteristic and two instances of
 * the Generic Level characteristic to represent a runtime in a generic level range.
 */
typedef struct __packed relative_runtime_in_a_generic_level_range {
    bt_mesh_percentage_8_t  relative_value;
    bt_mesh_generic_level_t minimum_generic_level;
    bt_mesh_generic_level_t maximum_generic_level;
} bt_mesh_relative_runtime_in_a_generic_level_range_t;

/* This characteristic aggregates the Percentage 8 characteristic, and two instances of
 * the Time Decihour 8 characteristic.
 */
typedef struct __packed relative_value_in_a_period_of_day {
    bt_mesh_percentage_8_t    relative_value;
    bt_mesh_time_decihour_8_t start_time;
    bt_mesh_time_decihour_8_t end_time;
} bt_mesh_relative_value_in_a_period_of_day_t;

/* This characteristic aggregates the Percentage 8 characteristic, and two instances of
 * the Temperature characteristic.
 */
typedef struct __packed relative_value_in_a_temperature_range {
    bt_mesh_percentage_8_t relative_value;
    bt_mesh_temperature_t  minimum_temperature_value;
    bt_mesh_temperature_t  maximum_temperature_value;
} bt_mesh_relative_value_in_a_temperature_range_t;

/* This characteristic aggregates the Percentage 8 characteristic and two instances of
 * the Voltage characteristic to represent a relative value in a voltage range.
 */
typedef struct __packed relative_value_in_a_voltage_range {
    bt_mesh_percentage_8_t relative_value;
    bt_mesh_voltage_t      minimum_voltage;
    bt_mesh_voltage_t      maximum_voltage;
} bt_mesh_relative_value_in_a_voltage_range_t;

/* This characteristic aggregates the Percentage 8 characteristic and two instances of
 * the Illuminance characteristic to represent a relative value in a illuminance range.
 */
typedef struct __packed relative_value_in_an_illuminance_range {
    bt_mesh_percentage_8_t relative_value;
    bt_mesh_illuminance_t  minimum_illuminance;
    bt_mesh_illuminance_t  maximum_illuminance;
} bt_mesh_relative_value_in_an_illuminance_range_t;

/* The Temperature 8 characteristic is used to represent a measure of temperature with
 * a unit of 0.5 degree Celsius.
 * Note: Unit is degree Celsius with a resolution of 0.5;
 *       Minimum value: -64.0, maximum value: 63.5;
 *       A value of 0xFF represents 'value is not known'.
 */
typedef s8_t bt_mesh_temperature_8_t;

/* This characteristic aggregates the Temperature 8 characteristic, and two instances
 * of the Time Decihour 8 characteristic, to represent a temperature value in a period
 * of day.
 */
typedef struct __packed temperature_8_in_a_period_of_day {
    bt_mesh_temperature_8_t   temperature;
    bt_mesh_time_decihour_8_t start_time;
    bt_mesh_time_decihour_8_t end_time;
} bt_mesh_temperature_8_in_a_period_of_day_t;

/* This characteristic aggregates four instances of the Temperature 8 characteristic,
 * and one instance of the Time Exponential 8 characteristic.
 */
typedef struct __packed temperature_8_statistics {
    bt_mesh_temperature_8_t      average;
    bt_mesh_temperature_8_t      standard_deviation_value;
    bt_mesh_temperature_8_t      minimum_value;
    bt_mesh_temperature_8_t      maximum_value;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_temperature_8_statistics_t;

/* This characteristic aggregates two instances of the Temperature characteristic to
 * represent a temperature range.
 */
typedef struct __packed temperature_range {
    bt_mesh_temperature_t minimum_temperature;
    bt_mesh_temperature_t maximum_temperature;
} bt_mesh_temperature_range_t;

/* This characteristic aggregates four instances of the Temperature characteristic,
 * and one instance of the Time Exponential 8 characteristic.
 */
typedef struct __packed temperature_statistics {
    bt_mesh_temperature_t        average_temperature;
    bt_mesh_temperature_t        standard_deviation_temperature;
    bt_mesh_temperature_t        minimum_temperature;
    bt_mesh_temperature_t        maximum_temperature;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_temperature_statistics_t;

/* The Time Hour 24 characteristic is used to represent a period of time in hours.
 * Note: Unit is hour with a resolution of 1;
 *       Minimum value: 0, maximum value: 16777214;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_time_hour_24_t[3];

/* The Time Millisecond 24 characteristic is used to represent a period of time with
 * a resolution of 1 millisecond.
 * Note: Unit is second with a resolution of 0.001;
 *       Minimum value: 0, maximum value: 16777.214;
 *       A value of 0xFFFFFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_time_millisecond_24_t[3];

/* The Time Second 8 characteristic is used to represent a period of time with a unit
 * of 1 second.
 * Note: Unit is second with a resolution of 1;
 *       Minimum value: 0, maximum value: 254;
 *       A value of 0xFF represents 'value is not known'.
 */
typedef u8_t bt_mesh_time_second_8_t;

/* This characteristic aggregates three instances of the Voltage characteristic to
 * represent a specification of voltage values.
 */
typedef struct __packed voltage_specification {
    bt_mesh_voltage_t minimum_voltage_value;
    bt_mesh_voltage_t typical_voltage_value;
    bt_mesh_voltage_t maximum_voltage_value;
} bt_mesh_voltage_specification_t;

/* This characteristic aggregates four instances of the Voltage characteristic and an
 * instance of the Time Exponential 8 characteristic to represent a set of statistical
 * voltage values over a period of time.
 */
typedef struct __packed voltage_statistics {
    bt_mesh_voltage_t            average_voltage_value;
    bt_mesh_voltage_t            standard_deviation_voltage_value;
    bt_mesh_voltage_t            minimum_voltage_value;
    bt_mesh_voltage_t            maximum_voltage_value;
    bt_mesh_time_exponential_8_t sensing_duration;
} bt_mesh_voltage_statistics_t;

/* The Volume Flow characteristic is used to represent a flow of a general volume such
 * as a volume of material or gas.
 * Note: Unit is liter/second with a resolution of 0.001 (1 milliliter);
 *       Minimum value: 0, maximum value: 65534;
 *       A value of 0xFFFF represents 'value is not known'.
 */
typedef u16_t bt_mesh_volume_flow_t;

/* Mesh Device Property related function */

u8_t bt_mesh_get_dev_prop_len(u16_t prop_id);

#endif /* _DEVICE_PROPERTY_H_ */