/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * Copyright (c) 2018 Vikrant More
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LIGHTING_SERVER_H_
#define _LIGHTING_SERVER_H_

#include "server_common.h"

struct bt_mesh_light_lightness_state {
    u16_t lightness_linear;
    u16_t target_lightness_linear;

    u16_t lightness_actual;
    u16_t target_lightness_actual;

    u16_t lightness_last;
    u16_t lightness_default;

    u8_t  status_code;
    u16_t lightness_range_min;
    u16_t lightness_range_max;
};

struct bt_mesh_light_lightness_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_lightness_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition actual_transition;
    struct bt_mesh_state_transition linear_transition;
    s32_t tt_delta_lightness_actual;
    s32_t tt_delta_lightness_linear;
};

struct bt_mesh_light_lightness_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_lightness_state *state;
};

struct bt_mesh_light_ctl_state {
    u16_t lightness;
    u16_t target_lightness;

    u16_t temperature;
    u16_t target_temperature;

    s16_t delta_uv;
    s16_t target_delta_uv;

    u8_t  status_code;
    u16_t temperature_range_min;
    u16_t temperature_range_max;

    u16_t lightness_default;
    u16_t temperature_default;
    s16_t delta_uv_default;
};

struct bt_mesh_light_ctl_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_ctl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_lightness;
    s32_t tt_delta_temperature;
    s32_t tt_delta_delta_uv;
};

struct bt_mesh_light_ctl_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_ctl_state *state;
};

struct bt_mesh_light_ctl_temp_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_ctl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_temperature;
    s32_t tt_delta_delta_uv;
};

struct bt_mesh_light_hsl_state {
    u16_t lightness;
    u16_t target_lightness;

    u16_t hue;
    u16_t target_hue;

    u16_t saturation;
    u16_t target_saturation;

    u16_t lightness_default;
    u16_t hue_default;
    u16_t saturation_default;

    u8_t  status_code;
    u16_t hue_range_min;
    u16_t hue_range_max;
    u16_t saturation_range_min;
    u16_t saturation_range_max;
};

struct bt_mesh_light_hsl_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_hsl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_lightness;
    s32_t tt_delta_hue;
    s32_t tt_delta_saturation;
};

struct bt_mesh_light_hsl_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_hsl_state *state;
};

struct bt_mesh_light_hsl_hue_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_hsl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_hue;
};

struct bt_mesh_light_hsl_sat_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_hsl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_saturation;
};

struct bt_mesh_light_xyl_state {
    u16_t lightness;
    u16_t target_lightness;

    u16_t x;
    u16_t target_x;

    u16_t y;
    u16_t target_y;

    u16_t lightness_default;
    u16_t x_default;
    u16_t y_default;

    u8_t  status_code;
    u16_t x_range_min;
    u16_t x_range_max;
    u16_t y_range_min;
    u16_t y_range_max;
};

struct bt_mesh_light_xyl_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_xyl_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    s32_t tt_delta_lightness;
    s32_t tt_delta_x;
    s32_t tt_delta_y;
};

struct bt_mesh_light_xyl_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_xyl_state *state;
};

struct bt_mesh_light_lc_state {
    u32_t mode : 1,                 /* default 0 */
          occupancy_mode : 1,       /* default 1 */
          light_onoff : 1,
          target_light_onoff : 1,
          occupancy : 1,
          ambient_luxlevel : 24;    /* 0x000000 ~ 0xFFFFFF */

    u16_t linear_output;            /* 0x0000 ~ 0xFFFF */
};

struct bt_mesh_light_lc_property_state {
    u32_t time_occupancy_delay;         /* 0x003A */
    u32_t time_fade_on;                 /* 0x0037 */
    u32_t time_run_on;                  /* 0x003C */
    u32_t time_fade;                    /* 0x0036 */
    u32_t time_prolong;                 /* 0x003B */
    u32_t time_fade_standby_auto;       /* 0x0038 */
    u32_t time_fade_standby_manual;     /* 0x0039 */

    u16_t lightness_on;                 /* 0x002E */
    u16_t lightness_prolong;            /* 0x002F */
    u16_t lightness_standby;            /* 0x0030 */

    u16_t ambient_luxlevel_on;          /* 0x002B, 0x0000 ~ 0xFFFF */
    u16_t ambient_luxlevel_prolong;     /* 0x002C, 0x0000 ~ 0xFFFF */
    u16_t ambient_luxlevel_standby;     /* 0x002D, 0x0000 ~ 0xFFFF */

    float regulator_kiu;                /* 0x0033, 0.0 ~ 1000.0, default 250.0 */
    float regulator_kid;                /* 0x0032, 0.0 ~ 1000.0, default 25.0 */
    float regulator_kpu;                /* 0x0035, 0.0 ~ 1000.0, default 80.0 */
    float regulator_kpd;                /* 0x0034, 0.0 ~ 1000.0, default 80.0 */
    s8_t  regulator_accuracy;           /* 0x0031, 0.0 ~ 100.0, default 2.0 */

    u32_t set_occupancy_to_1_delay;
};

typedef enum {
    LC_OFF,
    LC_STANDBY,
    LC_FADE_ON,
    LC_RUN,
    LC_FADE,
    LC_PROLONG,
    LC_FADE_STANDBY_AUTO,
    LC_FADE_STANDBY_MANUAL,
} bt_mesh_lc_state;

struct bt_mesh_light_lc_state_machine {
    struct {
        u8_t fade_on;
        u8_t fade;
        u8_t fade_standby_auto;
        u8_t fade_standby_manual;
    } trans_time;
    bt_mesh_lc_state state;
    struct k_delayed_work timer;
};

struct bt_mesh_light_control {
    struct bt_mesh_light_lc_state          state;
    struct bt_mesh_light_lc_property_state prop_state;
    struct bt_mesh_light_lc_state_machine  state_machine;
};

struct bt_mesh_light_lc_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_control *lc;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
};

struct bt_mesh_light_lc_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_light_control *lc;
};

typedef union {
    struct {
        u16_t lightness;
    } lightness_set;
    struct {
        u16_t lightness;
    } lightness_linear_set;
    struct {
        u16_t lightness;
    } lightness_default_set;
    struct {
        u16_t range_min;
        u16_t range_max;
    } lightness_range_set;
    struct {
        u16_t lightness;
        u16_t temperature;
        s16_t delta_uv;
    } ctl_set;
    struct {
        u16_t temperature;
        s16_t delta_uv;
    } ctl_temp_set;
    struct {
        u16_t range_min;
        u16_t range_max;
    } ctl_temp_range_set;
    struct {
        u16_t lightness;
        u16_t temperature;
        s16_t delta_uv;
    } ctl_default_set;
    struct {
        u16_t lightness;
        u16_t hue;
        u16_t saturation;
    } hsl_set;
    struct {
        u16_t hue;
    } hsl_hue_set;
    struct {
        u16_t saturation;
    } hsl_saturation_set;
    struct {
        u16_t lightness;
        u16_t hue;
        u16_t saturation;
    } hsl_default_set;
    struct {
        u16_t hue_range_min;
        u16_t hue_range_max;
        u16_t sat_range_min;
        u16_t sat_range_max;
    } hsl_range_set;
    struct {
        u16_t lightness;
        u16_t x;
        u16_t y;
    } xyl_set;
    struct {
        u16_t lightness;
        u16_t x;
        u16_t y;
    } xyl_default_set;
    struct {
        u16_t x_range_min;
        u16_t x_range_max;
        u16_t y_range_min;
        u16_t y_range_max;
    } xyl_range_set;
    struct {
        u8_t mode;
    } lc_mode_set;
    struct {
        u8_t mode;
    } lc_om_set;
    struct {
        u8_t onoff;
    } lc_light_onoff_set;
    struct {
        u16_t id;
        struct net_buf_simple *value;
    } lc_property_set;
    struct {
        u16_t property_id;
        union {
            u8_t  occupancy;
            u32_t set_occupancy_to_1_delay;
            u32_t ambient_luxlevel;
        } state;
    } sensor_status;
} bt_mesh_light_server_state_change_t;

typedef union {
    struct {
        u16_t id;
    } lc_property_get;
} bt_mesh_light_server_recv_get_msg_t;

typedef union {
    struct {
        bool  op_en;
        u16_t lightness;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } lightness_set;
    struct {
        bool  op_en;
        u16_t lightness;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } lightness_linear_set;
    struct {
        u16_t lightness;
    } lightness_default_set;
    struct {
        u16_t range_min;
        u16_t range_max;
    } lightness_range_set;
    struct {
        bool  op_en;
        u16_t lightness;
        u16_t temperature;
        s16_t delta_uv;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } ctl_set;
    struct {
        bool  op_en;
        u16_t temperature;
        s16_t delta_uv;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } ctl_temp_set;
    struct {
        u16_t range_min;
        u16_t range_max;
    } ctl_temp_range_set;
    struct {
        u16_t lightness;
        u16_t temperature;
        s16_t delta_uv;
    } ctl_default_set;
    struct {
        bool  op_en;
        u16_t lightness;
        u16_t hue;
        u16_t saturation;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } hsl_set;
    struct {
        bool  op_en;
        u16_t hue;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } hsl_hue_set;
    struct {
        bool  op_en;
        u16_t saturation;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } hsl_saturation_set;
    struct {
        u16_t lightness;
        u16_t hue;
        u16_t saturation;
    } hsl_default_set;
    struct {
        u16_t hue_range_min;
        u16_t hue_range_max;
        u16_t sat_range_min;
        u16_t sat_range_max;
    } hsl_range_set;
    struct {
        bool  op_en;
        u16_t lightness;
        u16_t x;
        u16_t y;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } xyl_set;
    struct {
        u16_t lightness;
        u16_t x;
        u16_t y;
    } xyl_default_set;
    struct {
        u16_t x_range_min;
        u16_t x_range_max;
        u16_t y_range_min;
        u16_t y_range_max;
    } xyl_range_set;
    struct {
        u8_t mode;
    } lc_mode_set;
    struct {
        u8_t mode;
    } lc_om_set;
    struct {
        bool op_en;
        u8_t light_onoff;
        u8_t tid;
        u8_t trans_time;
        u8_t delay;
    } lc_light_onoff_set;
    struct {
        u16_t id;
        struct net_buf_simple *value;
    } lc_property_set;
} bt_mesh_light_server_recv_set_msg_t;

typedef union {
    struct {
        struct net_buf_simple *data;
    } sensor_status;
} bt_mesh_light_server_recv_status_msg_t;

void bt_mesh_light_server_lock(void);
void bt_mesh_light_server_unlock(void);

u8_t *bt_mesh_get_lc_prop_value(struct bt_mesh_model *model, u16_t prop_id);

void light_lightness_publish(struct bt_mesh_model *model, u16_t opcode);
void light_ctl_publish(struct bt_mesh_model *model, u16_t opcode);
void light_hsl_publish(struct bt_mesh_model *model, u16_t opcode);
void light_xyl_publish(struct bt_mesh_model *model, u16_t opcode);
void light_lc_publish(struct bt_mesh_model *model, u16_t opcode);

int bt_mesh_light_lightness_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_lightness_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_ctl_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_ctl_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_ctl_temp_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_hsl_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_hsl_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_hsl_hue_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_hsl_sat_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_xyl_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_xyl_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_lc_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_light_lc_setup_srv_init(struct bt_mesh_model *model, bool primary);

#endif /* _LIGHTING_SERVER_H_ */
