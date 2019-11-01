/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _STATE_BINDING_H_
#define _STATE_BINDING_H_

typedef enum {
    GENERIC_ONOFF_STATE,
    GENERIC_LEVEL_STATE,
    GENERIC_ONPOWERUP_STATE,
    GENERIC_POWER_ACTUAL_STATE,
    LIGHT_LIGHTNESS_ACTUAL_STATE,
    LIGHT_LIGHTNESS_LINEAR_STATE,
    LIGHT_CTL_LIGHTNESS_STATE,
    LIGHT_CTL_TEMP_DELTA_UV_STATE,
    LIGHT_HSL_LIGHTNESS_STATE,
    LIGHT_HSL_HUE_STATE,
    LIGHT_HSL_SATURATION_STATE,
    LIGHT_XYL_LIGHTNESS_STATE,
    LIGHT_LC_LIGHT_ONOFF_STATE,
    BIND_STATE_MAX,
} bt_mesh_server_state_type_t;

typedef union {
    struct {
        u8_t onoff;
    } gen_onoff;
    struct {
        s16_t level;
    } gen_level;
    struct {
        u8_t onpowerup;
    } gen_onpowerup;
    struct {
        u16_t power;
    } gen_power_actual;
    struct {
        u16_t lightness;
    } light_lightness_actual;
    struct {
        u16_t lightness;
    } light_lightness_linear;
    struct {
        u16_t lightness;
    } light_ctl_lightness;
    struct {
        u16_t temperature;
        s16_t delta_uv;
    } light_ctl_temp_delta_uv;
    struct {
        u16_t lightness;
    } light_hsl_lightness;
    struct {
        u16_t hue;
    } light_hsl_hue;
    struct {
        u16_t saturation;
    } light_hsl_saturation;
    struct {
        u16_t lightness;
    } light_xyl_lightness;
    struct {
        u8_t onoff;
    } light_lc_light_onoff;
} bt_mesh_server_state_value_t;

u16_t bt_mesh_convert_lightness_actual_to_linear(u16_t actual);

u16_t bt_mesh_convert_lightness_linear_to_actual(u16_t linear);

s16_t bt_mesh_convert_temperature_to_gen_level(u16_t temp, u16_t min, u16_t max);

u16_t bt_mesh_covert_gen_level_to_temperature(s16_t level, u16_t min, u16_t max);

s16_t bt_mesh_convert_hue_to_level(u16_t hue);

u16_t bt_mesh_convert_level_to_hue(s16_t level);

s16_t bt_mesh_convert_saturation_to_level(u16_t saturation);

u16_t bt_mesh_convert_level_to_saturation(s16_t level);

int bt_mesh_update_binding_state(struct bt_mesh_model *model,
                                 bt_mesh_server_state_type_t type,
                                 bt_mesh_server_state_value_t *value);

#endif /* _STATE_BINDING_H_ */
