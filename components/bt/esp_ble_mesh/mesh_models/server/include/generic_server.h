/* Bluetooth: Mesh Generic OnOff, Generic Level, Lighting & Vendor Models
 *
 * SPDX-FileCopyrightText: 2018 Vikrant More
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _GENERIC_SERVER_H_
#define _GENERIC_SERVER_H_

#include "server_common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_gen_onoff_state {
    uint8_t onoff;
    uint8_t target_onoff;
};

struct bt_mesh_gen_onoff_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_onoff_state state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
};

struct bt_mesh_gen_level_state {
    int16_t level;
    int16_t target_level;

    int16_t last_level;
    int32_t last_delta;

    bool move_start;
    bool positive;
};

struct bt_mesh_gen_level_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_level_state state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    int32_t tt_delta_level;
};

struct bt_mesh_gen_def_trans_time_state {
    uint8_t trans_time;
};

struct bt_mesh_gen_def_trans_time_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_def_trans_time_state state;
};

struct bt_mesh_gen_onpowerup_state {
    uint8_t onpowerup;
};

struct bt_mesh_gen_power_onoff_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_onpowerup_state *state;
};

struct bt_mesh_gen_power_onoff_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_onpowerup_state *state;
};

struct bt_mesh_gen_power_level_state {
    uint16_t power_actual;
    uint16_t target_power_actual;

    uint16_t power_last;
    uint16_t power_default;

    uint8_t  status_code;
    uint16_t power_range_min;
    uint16_t power_range_max;
};

struct bt_mesh_gen_power_level_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_power_level_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
    int32_t tt_delta_level;
};

struct bt_mesh_gen_power_level_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_power_level_state *state;
};

struct bt_mesh_gen_battery_state {
    uint32_t battery_level : 8,
             time_to_discharge : 24;
    uint32_t time_to_charge : 24,
             battery_flags : 8;
};

struct bt_mesh_gen_battery_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_battery_state state;
};

struct bt_mesh_gen_location_state {
    int32_t  global_latitude;
    int32_t  global_longitude;
    int16_t  global_altitude;
    int16_t  local_north;
    int16_t  local_east;
    int16_t  local_altitude;
    uint8_t  floor_number;
    uint16_t uncertainty;
};

struct bt_mesh_gen_location_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_location_state *state;
};

struct bt_mesh_gen_location_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_gen_location_state *state;
};

/**
 * According to the hierarchy of Generic Property states (Model Spec section 3.1.8),
 * the Manufacturer Properties and Admin Properties may contain multiple Property
 * states. User Properties just a collection of which can be accessed.
 *
 * property_count: Number of the properties contained in the table
 * properties:     Table of the properties
 *
 * These variables need to be initialized in the application layer, the precise
 * number of the properties should be set and memories used to store the property
 * values should be allocated.
 */

enum bt_mesh_gen_user_prop_access {
    USER_ACCESS_PROHIBIT,
    USER_ACCESS_READ,
    USER_ACCESS_WRITE,
    USER_ACCESS_READ_WRITE,
};

enum bt_mesh_gen_admin_prop_access {
    ADMIN_NOT_USER_PROP,
    ADMIN_ACCESS_READ,
    ADMIN_ACCESS_WRITE,
    ADMIN_ACCESS_READ_WRITE,
};

enum bt_mesh_gen_manu_prop_access {
    MANU_NOT_USER_PROP,
    MANU_ACCESS_READ,
};

struct bt_mesh_generic_property {
    uint16_t id;
    uint8_t  user_access;
    uint8_t  admin_access;
    uint8_t  manu_access;
    struct net_buf_simple *val;
};

struct bt_mesh_gen_user_prop_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    uint8_t property_count;
    struct bt_mesh_generic_property *properties;
};

struct bt_mesh_gen_admin_prop_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    uint8_t property_count;
    struct bt_mesh_generic_property *properties;
};

struct bt_mesh_gen_manu_prop_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    uint8_t property_count;
    struct bt_mesh_generic_property *properties;
};

struct bt_mesh_gen_client_prop_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    uint8_t id_count;
    uint16_t *property_ids;
};

typedef union {
    struct {
        uint8_t onoff;
    } gen_onoff_set;
    struct {
        int16_t level;
    } gen_level_set;
    struct {
        int16_t level;
    } gen_delta_set;
    struct {
        int16_t level;
    } gen_move_set;
    struct {
        uint8_t trans_time;
    } gen_def_trans_time_set;
    struct {
        uint8_t onpowerup;
    } gen_onpowerup_set;
    struct {
        uint16_t power;
    } gen_power_level_set;
    struct {
        uint16_t power;
    } gen_power_default_set;
    struct {
        uint16_t range_min;
        uint16_t range_max;
    } gen_power_range_set;
    struct {
        int32_t latitude;
        int32_t longitude;
        int16_t altitude;
    } gen_loc_global_set;
    struct {
        int16_t  north;
        int16_t  east;
        int16_t  altitude;
        uint8_t  floor_number;
        uint16_t uncertainty;
    } gen_loc_local_set;
    struct {
        uint16_t id;
        struct net_buf_simple *value;
    } gen_user_prop_set;
    struct {
        uint16_t id;
        uint8_t  access;
        struct net_buf_simple *value;
    } gen_admin_prop_set;
    struct {
        uint16_t id;
        uint8_t  access;
    } gen_manu_prop_set;
} bt_mesh_gen_server_state_change_t;

typedef union {
    struct {
        uint16_t id;
    } user_property_get;
    struct {
        uint16_t id;
    } admin_property_get;
    struct {
        uint16_t id;
    } manu_property_get;
    struct {
        uint16_t id;
    } client_properties_get;
} bt_mesh_gen_server_recv_get_msg_t;

typedef union {
    struct {
        bool    op_en;
        uint8_t onoff;
        uint8_t tid;
        uint8_t trans_time;
        uint8_t delay;
    } onoff_set;
    struct {
        bool    op_en;
        int16_t level;
        uint8_t tid;
        uint8_t trans_time;
        uint8_t delay;
    } level_set;
    struct {
        bool    op_en;
        int32_t delta_level;
        uint8_t tid;
        uint8_t trans_time;
        uint8_t delay;
    } delta_set;
    struct {
        bool    op_en;
        int16_t delta_level;
        uint8_t tid;
        uint8_t trans_time;
        uint8_t delay;
    } move_set;
    struct {
        uint8_t trans_time;
    } def_trans_time_set;
    struct {
        uint8_t onpowerup;
    } onpowerup_set;
    struct {
        bool     op_en;
        uint16_t power;
        uint8_t  tid;
        uint8_t  trans_time;
        uint8_t  delay;
    } power_level_set;
    struct {
        uint16_t power;
    } power_default_set;
    struct {
        uint16_t range_min;
        uint16_t range_max;
    } power_range_set;
    struct {
        int32_t latitude;
        int32_t longitude;
        int16_t altitude;
    } loc_global_set;
    struct {
        int16_t  north;
        int16_t  east;
        int16_t  altitude;
        uint8_t  floor_number;
        uint16_t uncertainty;
    } loc_local_set;
    struct {
        uint16_t id;
        struct net_buf_simple *value;
    } user_property_set;
    struct {
        uint16_t id;
        uint8_t  access;
        struct net_buf_simple *value;
    } admin_property_set;
    struct {
        uint16_t id;
        uint8_t  access;
    } manu_property_set;
} bt_mesh_gen_server_recv_set_msg_t;

void bt_mesh_generic_server_lock(void);
void bt_mesh_generic_server_unlock(void);

void gen_onoff_publish(struct bt_mesh_model *model);
void gen_level_publish(struct bt_mesh_model *model);
void gen_onpowerup_publish(struct bt_mesh_model *model);
void gen_power_level_publish(struct bt_mesh_model *model, uint16_t opcode);

#ifdef __cplusplus
}
#endif

#endif /* _GENERIC_SERVER_H_ */
