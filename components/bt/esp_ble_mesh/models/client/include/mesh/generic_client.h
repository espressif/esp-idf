/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Generic Client Model APIs.
 */

#ifndef _GENERIC_CLIENT_H_
#define _GENERIC_CLIENT_H_

#include "mesh/client_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_generic_client_t;
typedef bt_mesh_client_internal_data_t  generic_internal_data_t;

/* Generic Client Model Callback */
extern const struct bt_mesh_model_cb bt_mesh_generic_client_cb;

/* Generic OnOff Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_onoff_cli_op[];

/** @def BLE_MESH_MODEL_GEN_ONOFF_CLI
 *
 *  Define a new generic onoff client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a generic onoff client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_onoff_cli.
 *
 *  @return New generic onoff client model instance.
 */
#define BLE_MESH_MODEL_GEN_ONOFF_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_ONOFF_CLI,     \
            bt_mesh_gen_onoff_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_onoff_client_t;

struct bt_mesh_gen_onoff_status {
    bool    op_en;         /* Indicate whether optional parameters included  */
    uint8_t present_onoff; /* Present value of Generic OnOff state           */
    uint8_t target_onoff;  /* Target value of Generic OnOff state (optional) */
    uint8_t remain_time;   /* Time to complete state transition (C.1)        */
};

struct bt_mesh_gen_onoff_set {
    bool    op_en;      /* Indicate whether optional parameters included */
    uint8_t onoff;      /* Target value of Generic OnOff state           */
    uint8_t tid;        /* Transaction Identifier                        */
    uint8_t trans_time; /* Time to complete state transition (optional)  */
    uint8_t delay;      /* Indicate message execution delay (C.1)        */
};

/* Generic Level Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_level_cli_op[];

/** @def BLE_MESH_MODEL_GEN_LEVEL_CLI
 *
 *  Define a new generic level client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a generic level client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_level_cli.
 *
 *  @return New generic level client model instance.
 */
#define BLE_MESH_MODEL_GEN_LEVEL_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_LEVEL_CLI,     \
            bt_mesh_gen_level_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_level_client_t;

struct bt_mesh_gen_level_status {
    bool    op_en;         /* Indicate whether optional parameters included      */
    int16_t present_level; /* Present value of Generic Level state               */
    int16_t target_level;  /* Target value of the Generic Level state (optional) */
    uint8_t remain_time;   /* Time to complete state transition (C.1)            */
};

struct bt_mesh_gen_level_set {
    bool    op_en;      /* Indicate whether optional parameters included */
    int16_t level;      /* Target value of Generic Level state           */
    uint8_t tid;        /* Transaction Identifier                        */
    uint8_t trans_time; /* Time to complete state transition (optional)  */
    uint8_t delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_gen_delta_set {
    bool    op_en;        /* Indicate whether optional parameters included */
    int32_t delta_level;  /* Delta change of Generic Level state           */
    uint8_t tid;          /* Transaction Identifier                        */
    uint8_t trans_time;   /* Time to complete state transition (optional)  */
    uint8_t delay;        /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_gen_move_set {
    bool    op_en;       /* Indicate whether optional parameters included                    */
    int16_t delta_level; /* Delta Level step to calculate Move speed for Generic Level state */
    uint8_t tid;         /* Transaction Identifier                                           */
    uint8_t trans_time;  /* Time to complete state transition (optional)                     */
    uint8_t delay;       /* Indicate message execution delay (C.1)                           */
};

/* Generic Default Transition Time Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_def_trans_time_cli_op[];

/** @def BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI
 *
 *  Define a new generic default transition time client model. Note
 *  that this API needs to be repeated for each element that the
 *  application wants to have a generic default transition client
 *  model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_def_trans_time_cli.
 *
 *  @return New generic default transition time client model instance.
 */
#define BLE_MESH_MODEL_GEN_DEF_TRANS_TIME_CLI(cli_pub, cli_data)    \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI,    \
            bt_mesh_gen_def_trans_time_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_def_trans_time_client_t;

struct bt_mesh_gen_def_trans_time_set {
    uint8_t trans_time;  /* The value of the Generic Default Transition Time state */
};

struct bt_mesh_gen_def_trans_time_status {
    uint8_t trans_time;  /* The value of the Generic Default Transition Time state */
};

/* Generic Power OnOff Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_power_onoff_cli_op[];

/** @def BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI
 *
 *  Define a new generic power onoff client model. Note that this API
 *  needs to be repeated for each element which the application wants
 *  to have a generic power onoff client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_power_onoff_cli.
 *
 *  @return New generic power onoff client model instance.
 */
#define BLE_MESH_MODEL_GEN_POWER_ONOFF_CLI(cli_pub, cli_data)   \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI,   \
            bt_mesh_gen_power_onoff_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_power_onoff_client_t;

struct bt_mesh_gen_onpowerup_set {
    uint8_t onpowerup;  /* The value of the Generic OnPowerUp state */
};

struct bt_mesh_gen_onpowerup_status {
    uint8_t onpowerup;  /* The value of the Generic OnPowerUp state */
};

/* Generic Power Level Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_power_level_cli_op[];

/** @def BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI
 *
 *  Define a new generic power level client model. Note that this API
 *  needs to be repeated for each element which the application wants
 *  to have a generic power level client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_power_level_cli.
 *
 *  @return New generic power level client model instance.
 */
#define BLE_MESH_MODEL_GEN_POWER_LEVEL_CLI(cli_pub, cli_data)   \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI,   \
            bt_mesh_gen_power_level_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_power_level_client_t;

struct bt_mesh_gen_power_level_status {
    bool     op_en;         /* Indicate whether optional parameters included         */
    uint16_t present_power; /* Present value of Generic Power Actual state           */
    uint16_t target_power;  /* Target value of Generic Power Actual state (optional) */
    uint8_t  remain_time;   /* Time to complete state transition (C.1)               */
};

struct bt_mesh_gen_power_last_status {
    uint16_t power; /* The value of the Generic Power Last state */
};

struct bt_mesh_gen_power_default_status {
    uint16_t power; /* The value of the Generic Default Last state */
};

struct bt_mesh_gen_power_range_status {
    uint8_t  status_code; /* Status Code for the requesting message                */
    uint16_t range_min;   /* Value of Range Min field of Generic Power Range state */
    uint16_t range_max;   /* Value of Range Max field of Generic Power Range state */
};

struct bt_mesh_gen_power_level_set {
    bool     op_en;      /* Indicate whether optional parameters included */
    uint16_t power;      /* Target value of Generic Power Actual state    */
    uint8_t  tid;        /* Transaction Identifier                        */
    uint8_t  trans_time; /* Time to complete state transition (optional)  */
    uint8_t  delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_gen_power_default_set {
    uint16_t power;  /* The value of the Generic Power Default state */
};

struct bt_mesh_gen_power_range_set {
    uint16_t range_min; /* Value of Range Min field of Generic Power Range state */
    uint16_t range_max; /* Value of Range Max field of Generic Power Range state */
};

/* Generic Battery Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_battery_cli_op[];

/** @def BLE_MESH_MODEL_GEN_BATTERY_CLI
 *
 *  Define a new generic battery client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a generic battery client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_battery_cli.
 *
 *  @return New generic battery client model instance.
 */
#define BLE_MESH_MODEL_GEN_BATTERY_CLI(cli_pub, cli_data)   \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_BATTERY_CLI,   \
            bt_mesh_gen_battery_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_battery_client_t;

struct bt_mesh_gen_battery_status {
    uint32_t battery_level     : 8;  /* Value of Generic Battery Level state             */
    uint32_t time_to_discharge : 24; /* Value of Generic Battery Time to Discharge state */
    uint32_t time_to_charge    : 24; /* Value of Generic Battery Time to Charge state    */
    uint32_t flags             : 8;  /* Value of Generic Battery Flags state             */
};

/* Generic Location Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_location_cli_op[];

/** @def BLE_MESH_MODEL_GEN_LOCATION_CLI
 *
 *  Define a new generic location client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a generic location client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_location_cli.
 *
 *  @return New generic location client model instance.
 */
#define BLE_MESH_MODEL_GEN_LOCATION_CLI(cli_pub, cli_data)  \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_LOCATION_CLI,  \
            bt_mesh_gen_location_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_location_client_t;

struct bt_mesh_gen_loc_global_status {
    int32_t global_latitude;  /* Global Coordinates (Latitude)  */
    int32_t global_longitude; /* Global Coordinates (Longitude) */
    int16_t global_altitude;  /* Global Altitude                */
};

struct bt_mesh_gen_loc_local_status {
    int16_t  local_north;    /* Local Coordinates (North) */
    int16_t  local_east;     /* Local Coordinates (East)  */
    int16_t  local_altitude; /* Local Altitude            */
    uint8_t  floor_number;   /* Floor Number              */
    uint16_t uncertainty;    /* Uncertainty               */
};

struct bt_mesh_gen_loc_global_set {
    int32_t global_latitude;  /* Global Coordinates (Latitude)  */
    int32_t global_longitude; /* Global Coordinates (Longitude) */
    int16_t global_altitude;  /* Global Altitude                */
};

struct bt_mesh_gen_loc_local_set {
    int16_t  local_north;    /* Local Coordinates (North) */
    int16_t  local_east;     /* Local Coordinates (East)  */
    int16_t  local_altitude; /* Local Altitude            */
    uint8_t  floor_number;   /* Floor Number              */
    uint16_t uncertainty;    /* Uncertainty               */
};

/* Generic Property Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_gen_property_cli_op[];

/** @def BLE_MESH_MODEL_GEN_LOCATION_CLI
 *
 *  Define a new generic location client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a generic location client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_gen_location_cli.
 *
 *  @return New generic location client model instance.
 */
#define BLE_MESH_MODEL_GEN_PROPERTY_CLI(cli_pub, cli_data)  \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_GEN_PROP_CLI,      \
            bt_mesh_gen_property_cli_op, cli_pub, cli_data, &bt_mesh_generic_client_cb)

typedef bt_mesh_client_user_data_t  bt_mesh_gen_property_client_t;

struct bt_mesh_gen_user_properties_status {
    struct net_buf_simple *user_property_ids; /* Buffer contains a sequence of N User Property IDs */
};

struct bt_mesh_gen_user_property_status {
    bool     op_en;                   /* Indicate whether optional parameters included      */
    uint16_t user_property_id;        /* Property ID identifying a Generic User Property    */
    uint8_t  user_access;             /* Enumeration indicating user access (optional)      */
    struct net_buf_simple *user_property_value; /* Raw value for the User Property (C.1) */
};

struct bt_mesh_gen_admin_properties_status {
    struct net_buf_simple *admin_property_ids; /* Buffer contains a sequence of N Admin Property IDs */
};

struct bt_mesh_gen_admin_property_status {
    bool     op_en;                    /* Indicate whether optional parameters included       */
    uint16_t admin_property_id;        /* Property ID identifying a Generic Admin Property    */
    uint8_t  admin_user_access;        /* Enumeration indicating user access (optional)       */
    struct net_buf_simple *admin_property_value; /* Raw value for the Admin Property (C.1) */
};

struct bt_mesh_gen_manu_properties_status {
    struct net_buf_simple *manu_property_ids; /* Buffer contains a sequence of N Manufacturer Property IDs */
};

struct bt_mesh_gen_manu_property_status {
    bool     op_en;                   /* Indicate whether optional parameters included              */
    uint16_t manu_property_id;        /* Property ID identifying a Generic Manufacturer Property    */
    uint8_t  manu_user_access;        /* Enumeration indicating user access (optional)              */
    struct net_buf_simple *manu_property_value; /* Raw value for the Manufacturer Property (C.1) */
};

struct bt_mesh_gen_client_properties_status {
    struct net_buf_simple *client_property_ids; /* Buffer contains a sequence of N Client Property IDs */
};

struct bt_mesh_gen_user_property_get {
    uint16_t user_property_id;        /* Property ID identifying a Generic User Property */
};

struct bt_mesh_gen_user_property_set {
    uint16_t user_property_id;        /* Property ID identifying a Generic User Property */
    struct net_buf_simple *user_property_value; /* Raw value for the User Property    */
};

struct bt_mesh_gen_admin_property_get {
    uint16_t admin_property_id;        /* Property ID identifying a Generic Admin Property */
};

struct bt_mesh_gen_admin_property_set {
    uint16_t admin_property_id;        /* Property ID identifying a Generic Admin Property */
    uint8_t  admin_user_access;        /* Enumeration indicating user access               */
    struct net_buf_simple *admin_property_value; /* Raw value for the Admin Property    */
};

struct bt_mesh_gen_manu_property_get {
    uint16_t manu_property_id; /* Property ID identifying a Generic Manufacturer Property */
};

struct bt_mesh_gen_manu_property_set {
    uint16_t manu_property_id; /* Property ID identifying a Generic Manufacturer Property */
    uint8_t  manu_user_access; /* Enumeration indicating user access                      */
};

struct bt_mesh_gen_client_properties_get {
    uint16_t client_property_id; /* A starting Client Property ID present within an element */
};

/**
 * @brief This function is called to get generic states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of generic get message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_generic_client_get_state(bt_mesh_client_common_param_t *common, void *get);

/**
 * @brief This function is called to set generic states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of generic set message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_generic_client_set_state(bt_mesh_client_common_param_t *common, void *set);

#ifdef __cplusplus
}
#endif

#endif /* _GENERIC_CLIENT_H_ */
