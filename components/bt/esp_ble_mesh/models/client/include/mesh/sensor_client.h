/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Sensor Client Model APIs.
 */

#ifndef _SENSOR_CLIENT_H_
#define _SENSOR_CLIENT_H_

#include "mesh/client_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sensor Client Model Callback */
extern const struct bt_mesh_model_cb bt_mesh_sensor_client_cb;

/* Sensor Client Model Context */
extern const struct bt_mesh_model_op bt_mesh_sensor_cli_op[];

/** @def BLE_MESH_MODEL_SENSOR_CLI
 *
 *  Define a new sensor client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a sensor client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_sensor_cli.
 *
 *  @return New sensor client model instance.
 */
#define BLE_MESH_MODEL_SENSOR_CLI(cli_pub, cli_data)    \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_SENSOR_CLI,    \
            bt_mesh_sensor_cli_op, cli_pub, cli_data, &bt_mesh_sensor_client_cb)

typedef bt_mesh_client_user_data_t      bt_mesh_sensor_client_t;
typedef bt_mesh_client_internal_data_t  sensor_internal_data_t;

struct bt_mesh_sensor_descriptor_status {
    struct net_buf_simple *descriptor; /* Sequence of 8-octet sensor descriptors (optional) */
};

struct bt_mesh_sensor_cadence_status {
    uint16_t property_id;              /* Property for the sensor                       */
    struct net_buf_simple *sensor_cadence_value; /* Value of sensor cadence state */
};

struct bt_mesh_sensor_settings_status {
    uint16_t sensor_property_id;              /* Property ID identifying a sensor                        */
    struct net_buf_simple *sensor_setting_property_ids; /* A sequence of N sensor setting property IDs (optional) */
};

struct bt_mesh_sensor_setting_status {
    bool     op_en;                      /* Indicate whether optional parameters included       */
    uint16_t sensor_property_id;         /* Property ID identifying a sensor                    */
    uint16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor    */
    uint8_t  sensor_setting_access;      /* Read/Write access rights for the setting (optional) */
    struct net_buf_simple *sensor_setting_raw; /* Raw value for the setting */
};

struct bt_mesh_sensor_status {
    struct net_buf_simple *marshalled_sensor_data; /* Value of sensor data state (optional) */
};

struct bt_mesh_sensor_column_status {
    uint16_t property_id;             /* Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_column_value; /* Left values of sensor column status */
};

struct bt_mesh_sensor_series_status {
    uint16_t property_id;             /* Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_series_value; /* Left values of sensor series status */
};

struct bt_mesh_sensor_descriptor_get {
    bool     op_en;       /* Indicate whether optional parameters included */
    uint16_t property_id; /* Property ID for the sensor (optional)         */
};

struct bt_mesh_sensor_cadence_get {
    uint16_t property_id; /* Property ID for the sensor */
};

struct bt_mesh_sensor_cadence_set {
    uint16_t property_id;                     /* Property ID for the sensor                                */
    uint8_t  fast_cadence_period_divisor : 7, /* Divisor for the publish period                            */
             status_trigger_type : 1;         /* The unit and format of the Status Trigger Delta fields    */
    struct net_buf_simple *status_trigger_delta_down; /* Delta down value that triggers a status message */
    struct net_buf_simple *status_trigger_delta_up; /* Delta up value that triggers a status message */
    uint8_t  status_min_interval;             /* Minimum interval between two consecutive Status messages  */
    struct net_buf_simple *fast_cadence_low;  /* Low value for the fast cadence range */
    struct net_buf_simple *fast_cadence_high; /* Fast value for the fast cadence range */
};

struct bt_mesh_sensor_settings_get {
    uint16_t sensor_property_id; /* Property ID for the sensor */
};

struct bt_mesh_sensor_setting_get {
    uint16_t sensor_property_id;         /* Property ID identifying a sensor                 */
    uint16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor */
};

struct bt_mesh_sensor_setting_set {
    uint16_t sensor_property_id;         /* Property ID identifying a sensor                 */
    uint16_t sensor_setting_property_id; /* Setting ID identifying a setting within a sensor */
    struct net_buf_simple *sensor_setting_raw; /* Raw value for the setting */
};

struct bt_mesh_sensor_get {
    bool     op_en;       /* Indicate whether optional parameters included */
    uint16_t property_id; /* Property ID for the sensor (optional)         */
};

struct bt_mesh_sensor_column_get {
    uint16_t property_id;     /* Property identifying a sensor                */
    struct net_buf_simple *raw_value_x; /* Raw value identifying a column */
};

struct bt_mesh_sensor_series_get {
    bool     op_en;            /* Indicate whether optional parameters included         */
    uint16_t property_id;      /* Property identifying a sensor                         */
    struct net_buf_simple *raw_value_x1; /* Raw value identifying a starting column (optional) */
    struct net_buf_simple *raw_value_x2; /* Raw value identifying a ending column (C.1) */
};

/**
 * @brief This function is called to get sensor states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of sensor get message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_sensor_client_get_state(bt_mesh_client_common_param_t *common, void *get);

/**
 * @brief This function is called to set sensor states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of sensor set message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_sensor_client_set_state(bt_mesh_client_common_param_t *common, void *set);

#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_CLIENT_H_ */
