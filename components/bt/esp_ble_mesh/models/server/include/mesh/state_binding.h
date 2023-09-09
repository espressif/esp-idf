/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _STATE_BINDING_H_
#define _STATE_BINDING_H_

#include "mesh/access.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GENERIC_ONOFF_STATE,
    GENERIC_LEVEL_STATE,
    GENERIC_ONPOWERUP_STATE,
    GENERIC_POWER_ACTUAL_STATE,
    LIGHT_LIGHTNESS_ACTUAL_STATE,
    LIGHT_LIGHTNESS_LINEAR_STATE,
    LIGHT_CTL_LIGHTNESS_STATE,
    LIGHT_CTL_TEMP_DELTA_UV_STATE,
    LIGHT_HSL_STATE,
    LIGHT_HSL_LIGHTNESS_STATE,
    LIGHT_HSL_HUE_STATE,
    LIGHT_HSL_SATURATION_STATE,
    LIGHT_XYL_LIGHTNESS_STATE,
    LIGHT_LC_LIGHT_ONOFF_STATE,
    BIND_STATE_MAX,
} bt_mesh_server_state_type_t;

typedef union {
    struct {
        uint8_t onoff;
    } gen_onoff;
    struct {
        int16_t level;
    } gen_level;
    struct {
        uint8_t onpowerup;
    } gen_onpowerup;
    struct {
        uint16_t power;
    } gen_power_actual;
    struct {
        uint16_t lightness;
    } light_lightness_actual;
    struct {
        uint16_t lightness;
    } light_lightness_linear;
    struct {
        uint16_t lightness;
    } light_ctl_lightness;
    struct {
        uint16_t temperature;
        int16_t  delta_uv;
    } light_ctl_temp_delta_uv;
    struct {
        uint16_t lightness;
        uint16_t hue;
        uint16_t saturation;
    } light_hsl;
    struct {
        uint16_t lightness;
    } light_hsl_lightness;
    struct {
        uint16_t hue;
    } light_hsl_hue;
    struct {
        uint16_t saturation;
    } light_hsl_saturation;
    struct {
        uint16_t lightness;
    } light_xyl_lightness;
    struct {
        uint8_t onoff;
    } light_lc_light_onoff;
} bt_mesh_server_state_value_t;

uint16_t bt_mesh_convert_lightness_actual_to_linear(uint16_t actual);

uint16_t bt_mesh_convert_lightness_linear_to_actual(uint16_t linear);

int16_t bt_mesh_convert_temperature_to_gen_level(uint16_t temp, uint16_t min, uint16_t max);

uint16_t bt_mesh_covert_gen_level_to_temperature(int16_t level, uint16_t min, uint16_t max);

int16_t bt_mesh_convert_hue_to_level(uint16_t hue);

uint16_t bt_mesh_convert_level_to_hue(int16_t level);

int16_t bt_mesh_convert_saturation_to_level(uint16_t saturation);

uint16_t bt_mesh_convert_level_to_saturation(int16_t level);

int bt_mesh_update_binding_state(struct bt_mesh_model *model,
                                 bt_mesh_server_state_type_t type,
                                 bt_mesh_server_state_value_t *value);

#ifdef __cplusplus
}
#endif

#endif /* _STATE_BINDING_H_ */
