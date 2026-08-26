/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Sensor Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_SENSOR_MODEL_API_H_
#define _ESP_BLE_MESH_SENSOR_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @def    ESP_BLE_MESH_MODEL_SENSOR_CLI
 *
 *  @brief  Define a new Sensor Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Sensor Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Sensor Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_SENSOR_CLI(cli_pub, cli_data)            \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SENSOR_CLI,    \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Sensor Client Model Get and Set parameters structure.
 */

/** Parameters of Sensor Descriptor Get */
typedef struct {
    bool  op_en;        /*!< Indicate if optional parameters are included */
    uint16_t property_id;  /*!< Property ID of a sensor (optional) */
} esp_ble_mesh_sensor_descriptor_get_t;

/** Parameter of Sensor Cadence Get */
typedef struct {
    uint16_t property_id;  /*!< Property ID of a sensor */
} esp_ble_mesh_sensor_cadence_get_t;

/** Parameters of Sensor Cadence Set */
typedef struct {
    uint16_t property_id;                               /*!< Property ID for the sensor */
    uint8_t  fast_cadence_period_divisor : 7,           /*!< Divisor for the publish period */
             status_trigger_type : 1;                   /*!< The unit and format of the Status Trigger Delta fields */
    struct net_buf_simple *status_trigger_delta_down;   /*!< Delta down value that triggers a status message */
    struct net_buf_simple *status_trigger_delta_up;     /*!< Delta up value that triggers a status message */
    uint8_t  status_min_interval;                       /*!< Minimum interval between two consecutive Status messages */
    struct net_buf_simple *fast_cadence_low;            /*!< Low value for the fast cadence range */
    struct net_buf_simple *fast_cadence_high;           /*!< Fast value for the fast cadence range */
} esp_ble_mesh_sensor_cadence_set_t;

/** Parameter of Sensor Settings Get */
typedef struct {
    uint16_t sensor_property_id;   /*!< Property ID of a sensor */
} esp_ble_mesh_sensor_settings_get_t;

/** Parameters of Sensor Setting Get */
typedef struct {
    uint16_t sensor_property_id;           /*!< Property ID of a sensor */
    uint16_t sensor_setting_property_id;   /*!< Setting ID identifying a setting within a sensor */
} esp_ble_mesh_sensor_setting_get_t;

/** Parameters of Sensor Setting Set */
typedef struct {
    uint16_t sensor_property_id;           /*!< Property ID identifying a sensor */
    uint16_t sensor_setting_property_id;   /*!< Setting ID identifying a setting within a sensor */
    struct net_buf_simple *sensor_setting_raw;  /*!< Raw value for the setting */
} esp_ble_mesh_sensor_setting_set_t;

/** Parameters of Sensor Get */
typedef struct {
    bool     op_en;        /*!< Indicate if optional parameters are included  */
    uint16_t property_id;  /*!< Property ID for the sensor (optional) */
} esp_ble_mesh_sensor_get_t;

/** Parameters of Sensor Column Get */
typedef struct {
    uint16_t property_id;  /*!< Property identifying a sensor */
    struct net_buf_simple *raw_value_x; /*!< Raw value identifying a column */
} esp_ble_mesh_sensor_column_get_t;

/** Parameters of Sensor Series Get */
typedef struct {
    bool     op_en;        /*!< Indicate if optional parameters are included */
    uint16_t property_id;  /*!< Property identifying a sensor */
    struct net_buf_simple *raw_value_x1;    /*!< Raw value identifying a starting column (optional) */
    struct net_buf_simple *raw_value_x2;    /*!< Raw value identifying an ending column (C.1) */
} esp_ble_mesh_sensor_series_get_t;

/**
 * @brief Sensor Client Model get message union
 */
typedef union {
    esp_ble_mesh_sensor_descriptor_get_t descriptor_get;    /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET */
    esp_ble_mesh_sensor_cadence_get_t    cadence_get;       /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET */
    esp_ble_mesh_sensor_settings_get_t   settings_get;      /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET */
    esp_ble_mesh_sensor_setting_get_t    setting_get;       /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET */
    esp_ble_mesh_sensor_get_t            sensor_get;        /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_GET */
    esp_ble_mesh_sensor_column_get_t     column_get;        /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET */
    esp_ble_mesh_sensor_series_get_t     series_get;        /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET */
} esp_ble_mesh_sensor_client_get_state_t;

/**
 * @brief Sensor Client Model set message union
 */
typedef union {
    esp_ble_mesh_sensor_cadence_set_t cadence_set;          /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET & ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK */
    esp_ble_mesh_sensor_setting_set_t setting_set;          /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET & ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK */
} esp_ble_mesh_sensor_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Sensor Client Model Get and Set callback parameters structure.
 */

/** Parameter of Sensor Descriptor Status */
typedef struct {
    struct net_buf_simple *descriptor;  /*!< Sequence of 8-octet sensor descriptors (optional) */
} esp_ble_mesh_sensor_descriptor_status_cb_t;

/** Parameters of Sensor Cadence Status */
typedef struct {
    uint16_t property_id;  /*!< Property for the sensor */
    struct net_buf_simple *sensor_cadence_value;    /*!< Value of sensor cadence state */
} esp_ble_mesh_sensor_cadence_status_cb_t;

/** Parameters of Sensor Settings Status */
typedef struct {
    uint16_t sensor_property_id;   /*!< Property ID identifying a sensor */
    struct net_buf_simple *sensor_setting_property_ids; /*!< A sequence of N sensor setting property IDs (optional) */
} esp_ble_mesh_sensor_settings_status_cb_t;

/** Parameters of Sensor Setting Status */
typedef struct {
    bool     op_en;                        /*!< Indicate id optional parameters are included */
    uint16_t sensor_property_id;           /*!< Property ID identifying a sensor */
    uint16_t sensor_setting_property_id;   /*!< Setting ID identifying a setting within a sensor */
    uint8_t  sensor_setting_access;        /*!< Read/Write access rights for the setting (optional) */
    struct net_buf_simple *sensor_setting_raw;  /*!< Raw value for the setting */
} esp_ble_mesh_sensor_setting_status_cb_t;

/** Parameter of Sensor Status */
typedef struct {
    struct net_buf_simple *marshalled_sensor_data;  /*!< Value of sensor data state (optional) */
} esp_ble_mesh_sensor_status_cb_t;

/** Parameters of Sensor Column Status */
typedef struct {
    uint16_t property_id;  /*!< Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_column_value; /*!< Left values of sensor column status */
} esp_ble_mesh_sensor_column_status_cb_t;

/** Parameters of Sensor Series Status */
typedef struct {
    uint16_t property_id;  /*!< Property identifying a sensor and the Y axis  */
    struct net_buf_simple *sensor_series_value; /*!< Left values of sensor series status */
} esp_ble_mesh_sensor_series_status_cb_t;

/**
 * @brief Sensor Client Model received message union
 */
typedef union {
    esp_ble_mesh_sensor_descriptor_status_cb_t descriptor_status;   /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS */
    esp_ble_mesh_sensor_cadence_status_cb_t    cadence_status;      /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS */
    esp_ble_mesh_sensor_settings_status_cb_t   settings_status;     /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS */
    esp_ble_mesh_sensor_setting_status_cb_t    setting_status;      /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS */
    esp_ble_mesh_sensor_status_cb_t            sensor_status;       /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS */
    esp_ble_mesh_sensor_column_status_cb_t     column_status;       /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS */
    esp_ble_mesh_sensor_series_status_cb_t     series_status;       /*!< For ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS */
} esp_ble_mesh_sensor_client_status_cb_t;

/** Sensor Client Model callback parameters */
typedef struct {
    int error_code;     /*!< 0: success,
                         *   otherwise failure. For the error code values please refer to errno.h file.
                         *   A negative sign is added to the standard error codes in errno.h. */
    esp_ble_mesh_client_common_param_t    *params;      /*!< The client common parameters. */
    esp_ble_mesh_sensor_client_status_cb_t status_cb;   /*!< The sensor status message callback values */
} esp_ble_mesh_sensor_client_cb_param_t;

/** This enum value is the event of Sensor Client Model */
typedef enum {
    ESP_BLE_MESH_SENSOR_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_SENSOR_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_SENSOR_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_SENSOR_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_SENSOR_CLIENT_EVT_MAX,
} esp_ble_mesh_sensor_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Sensor Client Model function.
 */

/**
 * @brief   Sensor Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_sensor_client_cb_t)(esp_ble_mesh_sensor_client_cb_event_t event,
                                                 esp_ble_mesh_sensor_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Sensor Client Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_sensor_client_callback(esp_ble_mesh_sensor_client_cb_t callback);

/**
 * @brief       Get the value of Sensor Server Model states using the Sensor Client Model get messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_sensor_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   get_state: Pointer to sensor get message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_sensor_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_sensor_client_get_state_t *get_state);

/**
 * @brief       Set the value of Sensor Server Model states using the Sensor Client Model set messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_sensor_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer to sensor set message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_sensor_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                               esp_ble_mesh_sensor_client_set_state_t *set_state);

/**
 * @brief Sensor Server Models related context.
 */

/** @def    ESP_BLE_MESH_MODEL_SENSOR_SRV
 *
 *  @brief  Define a new Sensor Server Model.
 *
 *  @note   1. The Sensor Server model is a root model. When this model is present
 *             on an element, the corresponding Sensor Setup Server model shall
 *             also be present.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_sensor_srv_t.
 *
 *  @return New Sensor Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SENSOR_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SENSOR_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_SENSOR_SETUP_SRV
 *
 *  @brief  Define a new Sensor Setup Server Model.
 *
 *  @note   1. The Sensor Setup Server model extends the Sensor Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_sensor_setup_srv_t.
 *
 *  @return New Sensor Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_SENSOR_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

#define ESP_BLE_MESH_INVALID_SENSOR_PROPERTY_ID 0x0000  /*!< Invalid Sensor Property ID */

#define ESP_BLE_MESH_SENSOR_PROPERTY_ID_LEN     0x02    /*!< Length of Sensor Property ID */

#define ESP_BLE_MESH_SENSOR_DESCRIPTOR_LEN      0x08    /*!< Length of Sensor Descriptor state */

#define ESP_BLE_MESH_SENSOR_UNSPECIFIED_POS_TOLERANCE   0x000   /*!< Unspecified Sensor Positive Tolerance */
#define ESP_BLE_MESH_SENSOR_UNSPECIFIED_NEG_TOLERANCE   0x000   /*!< Unspecified Sensor Negative Tolerance */

#define ESP_BLE_MESH_SENSOR_NOT_APPL_MEASURE_PERIOD     0x00    /*!< Not applicable Sensor Measurement Period */

#define ESP_BLE_MESH_SENSOR_NOT_APPL_UPDATE_INTERVAL    0x00    /*!< Not applicable Sensor Update Interval */

#define ESP_BLE_MESH_INVALID_SENSOR_SETTING_PROPERTY_ID 0x0000  /*!< Invalid Sensor Setting Property ID */

#define ESP_BLE_MESH_SENSOR_SETTING_PROPERTY_ID_LEN     0x02    /*!< Length of Sensor Setting Property ID */
#define ESP_BLE_MESH_SENSOR_SETTING_ACCESS_LEN          0x01    /*!< Length of Sensor Setting Access */

#define ESP_BLE_MESH_SENSOR_SETTING_ACCESS_READ         0x01    /*!< Sensor Setting Access - Read */
#define ESP_BLE_MESH_SENSOR_SETTING_ACCESS_READ_WRITE   0x03    /*!< Sensor Setting Access - Read & Write */

#define ESP_BLE_MESH_SENSOR_DIVISOR_TRIGGER_TYPE_LEN    0x01    /*!< Length of Sensor Divisor Trigger Type */
#define ESP_BLE_MESH_SENSOR_STATUS_MIN_INTERVAL_LEN     0x01    /*!< Length of Sensor Status Min Interval */

#define ESP_BLE_MESH_SENSOR_PERIOD_DIVISOR_MAX_VALUE    15  /*!< Maximum value of Sensor Period Divisor */

#define ESP_BLE_MESH_SENSOR_STATUS_MIN_INTERVAL_MAX     26  /*!< Maximum value of Sensor Status Min Interval */

/**
 * Sensor Status Trigger Type - Format Type of the characteristic
 * that the Sensor Property ID state references
 */
#define ESP_BLE_MESH_SENSOR_STATUS_TRIGGER_TYPE_CHAR    0
/** Sensor Status Trigger Type - Format Type "uint16" */
#define ESP_BLE_MESH_SENSOR_STATUS_TRIGGER_TYPE_UINT16  1

#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_A   0x00    /*!< Sensor Data Format A */
#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_B   0x01    /*!< Sensor Data Format B */

#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_A_MPID_LEN  0x02    /*!< MPID length of Sensor Data Format A */
#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_B_MPID_LEN  0x03    /*!< MPID length of Sensor Data Format B */

/**
 * Zero length of Sensor Data.
 *
 * Note:
 * The Length field is a 1-based uint7 value (valid range 0x0–0x7F,
 * representing range of 1–127). The value 0x7F represents a length
 * of zero.
 */
#define ESP_BLE_MESH_SENSOR_DATA_ZERO_LEN   0x7F

/** @def    ESP_BLE_MESH_GET_SENSOR_DATA_FORMAT
 *
 *  @brief  Get format of the sensor data.
 *
 *  @note   Multiple sensor data may be concatenated. Make sure the _data pointer is
 *          updated before getting the format of the corresponding sensor data.
 *
 *  @param  _data Pointer to the start of the sensor data.
 *
 *  @return Format of the sensor data.
 */
#define ESP_BLE_MESH_GET_SENSOR_DATA_FORMAT(_data)      (((_data)[0]) & BIT_MASK(1))

/** @def    ESP_BLE_MESH_GET_SENSOR_DATA_LENGTH
 *
 *  @brief  Get length of the sensor data.
 *
 *  @note   Multiple sensor data may be concatenated. Make sure the _data pointer is
 *          updated before getting the length of the corresponding sensor data.
 *
 *  @param  _data Pointer to the start of the sensor data.
 *  @param  _fmt  Format of the sensor data.
 *
 *  @return Length (zero-based) of the sensor data.
 */
#define ESP_BLE_MESH_GET_SENSOR_DATA_LENGTH(_data, _fmt)    \
            (((_fmt) == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A) ? ((((_data)[0]) >> 1) & BIT_MASK(4)) : ((((_data)[0]) >> 1) & BIT_MASK(7)))

/** @def    ESP_BLE_MESH_GET_SENSOR_DATA_PROPERTY_ID
 *
 *  @brief  Get Sensor Property ID of the sensor data.
 *
 *  @note   Multiple sensor data may be concatenated. Make sure the _data pointer is
 *          updated before getting Sensor Property ID of the corresponding sensor data.
 *
 *  @param  _data Pointer to the start of the sensor data.
 *  @param  _fmt  Format of the sensor data.
 *
 *  @return Sensor Property ID of the sensor data.
 */
#define ESP_BLE_MESH_GET_SENSOR_DATA_PROPERTY_ID(_data, _fmt)   \
            (((_fmt) == ESP_BLE_MESH_SENSOR_DATA_FORMAT_A) ? ((((_data)[1]) << 3) | (((_data)[0]) >> 5)) : ((((_data)[2]) << 8) | ((_data)[1])))

/** @def    ESP_BLE_MESH_SENSOR_DATA_FORMAT_A_MPID
 *
 *  @brief  Generate a MPID value for sensor data with Format A.
 *
 *  @note   1. The Format field is 0b0 and indicates that Format A is used.
 *          2. The Length field is a 1-based uint4 value (valid range 0x0–0xF,
 *             representing range of 1–16).
 *          3. The Property ID is an 11-bit bit field representing 11 LSb of a Property ID.
 *          4. This format may be used for Property Values that are not longer than 16
 *             octets and for Property IDs less than 0x0800.
 *
 *  @param  _len Length of Sensor Raw value.
 *  @param  _id  Sensor Property ID.
 *
 *  @return 2-octet MPID value for sensor data with Format A.
 *
 */
#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_A_MPID(_len, _id) \
        ((((_id) & BIT_MASK(11)) << 5) | (((_len) & BIT_MASK(4)) << 1) | ESP_BLE_MESH_SENSOR_DATA_FORMAT_A)

/** @def    ESP_BLE_MESH_SENSOR_DATA_FORMAT_B_MPID
 *
 *  @brief  Generate a MPID value for sensor data with Format B.
 *
 *  @note   1. The Format field is 0b1 and indicates Format B is used.
 *          2. The Length field is a 1-based uint7 value (valid range 0x0–0x7F, representing
 *             range of 1–127). The value 0x7F represents a length of zero.
 *          3. The Property ID is a 16-bit bit field representing a Property ID.
 *          4. This format may be used for Property Values not longer than 128 octets and for
 *             any Property IDs. Property values longer than 128 octets are not supported by
 *             the Sensor Status message.
 *          5. Exclude the generated 1-octet value, the 2-octet Sensor Property ID
 *
 *  @param  _len Length of Sensor Raw value.
 *  @param  _id  Sensor Property ID.
 *
 *  @return 3-octet MPID value for sensor data with Format B.
 *
 */
#define ESP_BLE_MESH_SENSOR_DATA_FORMAT_B_MPID(_len, _id) \
        (((_id) << 8) | (((_len) & BIT_MASK(7)) << 1) | ESP_BLE_MESH_SENSOR_DATA_FORMAT_B)

/** This enum value is value of Sensor Sampling Function */
enum esp_ble_mesh_sensor_sample_func {
    ESP_BLE_MESH_SAMPLE_FUNC_UNSPECIFIED,
    ESP_BLE_MESH_SAMPLE_FUNC_INSTANTANEOUS,
    ESP_BLE_MESH_SAMPLE_FUNC_ARITHMETIC_MEAN,
    ESP_BLE_MESH_SAMPLE_FUNC_RMS,
    ESP_BLE_MESH_SAMPLE_FUNC_MAXIMUM,
    ESP_BLE_MESH_SAMPLE_FUNC_MINIMUM,
    ESP_BLE_MESH_SAMPLE_FUNC_ACCUMULATED,
    ESP_BLE_MESH_SAMPLE_FUNC_COUNT,
};

/** Parameters of Sensor Descriptor state */
typedef struct {
    uint32_t positive_tolerance : 12,   /*!< The value of Sensor Positive Tolerance field */
             negative_tolerance : 12,   /*!< The value of Sensor Negative Tolerance field */
             sampling_function : 8;     /*!< The value of Sensor Sampling Function field */
    uint8_t  measure_period;            /*!< The value of Sensor Measurement Period field */
    uint8_t  update_interval;           /*!< The value of Sensor Update Interval field */
} esp_ble_mesh_sensor_descriptor_t;

/** Parameters of Sensor Setting state */
typedef struct {
    uint16_t property_id;       /*!< The value of Sensor Setting Property ID field */
    uint8_t  access;            /*!< The value of Sensor Setting Access field */
    struct net_buf_simple *raw; /*!< The value of Sensor Setting Raw field */
} esp_ble_mesh_sensor_setting_t;

/** Parameters of Sensor Cadence state */
typedef struct {
    uint8_t period_divisor : 7, /*!< The value of Fast Cadence Period Divisor field */
            trigger_type : 1;   /*!< The value of Status Trigger Type field */
    /**
     * Note:
     * The parameter "size" in trigger_delta_down, trigger_delta_up, fast_cadence_low &
     * fast_cadence_high indicates the exact length of these four parameters, and they
     * are associated with the Sensor Property ID. Users need to initialize the "size"
     * precisely.
     */
    struct net_buf_simple *trigger_delta_down;  /*!< The value of Status Trigger Delta Down field */
    struct net_buf_simple *trigger_delta_up;    /*!< The value of Status Trigger Delta Up field */
    uint8_t min_interval;                       /*!< The value of Status Min Interval field */
    struct net_buf_simple *fast_cadence_low;    /*!< The value of Fast Cadence Low field */
    struct net_buf_simple *fast_cadence_high;   /*!< The value of Fast Cadence High field */
} esp_ble_mesh_sensor_cadence_t;

/** Parameters of Sensor Data state */
typedef struct {
    /**
     * Format A: The Length field is a 1-based uint4 value (valid range 0x0–0xF,
     *           representing range of 1 – 16).
     * Format B: The Length field is a 1-based uint7 value (valid range 0x0–0x7F,
     *           representing range of 1 – 127). The value 0x7F represents a
     *           length of zero.
     */
    uint8_t format : 1, /*!< The value of the Sensor Data format */
            length : 7; /*!< The value of the Sensor Data length */
    struct net_buf_simple *raw_value;   /*!< The value of Sensor Data raw value */
} esp_ble_mesh_sensor_data_t;

/** Parameters of Sensor Series Column state */
typedef struct {
    struct net_buf_simple *raw_value_x;     /*!< The value of Sensor Raw Value X field */
    struct net_buf_simple *column_width;    /*!< The value of Sensor Column Width field */
    struct net_buf_simple *raw_value_y;     /*!< The value of Sensor Raw Value Y field */
} esp_ble_mesh_sensor_series_column_t;

/** Parameters of Sensor states */
typedef struct {
    uint16_t sensor_property_id;    /*!< The value of Sensor Property ID field */

    /* Constant throughout the lifetime of an element */
    esp_ble_mesh_sensor_descriptor_t descriptor;    /*!< Parameters of the Sensor Descriptor state */

    /**
     * Multiple Sensor Setting states may be present for each sensor.
     * The Sensor Setting Property ID values shall be unique for each
     * Sensor Property ID that identifies a sensor within an element.
     */
    const uint8_t setting_count;                /*!<  */
    esp_ble_mesh_sensor_setting_t *settings;    /*!< Parameters of the Sensor Setting state */

    /**
     * The Sensor Cadence state may be not supported by sensors based
     * on device properties referencing "non-scalar characteristics"
     * such as "histograms" or "composite characteristics".
     */
    esp_ble_mesh_sensor_cadence_t *cadence;     /*!< Parameters of the Sensor Cadence state */

    esp_ble_mesh_sensor_data_t sensor_data;     /*!< Parameters of the Sensor Data state */

    esp_ble_mesh_sensor_series_column_t series_column;  /*!< Parameters of the Sensor Series Column state */
} esp_ble_mesh_sensor_state_t;

/** User data of Sensor Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Sensor Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    const uint8_t state_count;                  /*!< Sensor state count */
    esp_ble_mesh_sensor_state_t *states;        /*!< Parameters of the Sensor states */
} esp_ble_mesh_sensor_srv_t;

/** User data of Sensor Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Sensor Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    const uint8_t state_count;                  /*!< Sensor state count */
    esp_ble_mesh_sensor_state_t *states;        /*!< Parameters of the Sensor states */
} esp_ble_mesh_sensor_setup_srv_t;

/** Parameters of Sensor Cadence Set state change event */
typedef struct {
    uint16_t property_id;           /*!< The value of Sensor Property ID state */
    uint8_t  period_divisor : 7,    /*!< The value of Fast Cadence Period Divisor state */
             trigger_type : 1;      /*!< The value of Status Trigger Type state */
    struct net_buf_simple *trigger_delta_down;  /*!< The value of Status Trigger Delta Down state */
    struct net_buf_simple *trigger_delta_up;    /*!< The value of Status Trigger Delta Up state */
    uint8_t min_interval;                       /*!< The value of Status Min Interval state */
    struct net_buf_simple *fast_cadence_low;    /*!< The value of Fast Cadence Low state */
    struct net_buf_simple *fast_cadence_high;   /*!< The value of Fast Cadence High state */
} esp_ble_mesh_state_change_sensor_cadence_set_t;

/** Parameters of Sensor Setting Set state change event */
typedef struct {
    uint16_t property_id;           /*!< The value of Sensor Property ID state */
    uint16_t setting_property_id;   /*!< The value of Sensor Setting Property ID state */
    struct net_buf_simple *setting_value;   /*!< The value of Sensor Property Value state */
} esp_ble_mesh_state_change_sensor_setting_set_t;

/**
 * @brief Sensor Server Model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_sensor_cadence_set_t sensor_cadence_set;  /*!< Sensor Cadence Set */
    esp_ble_mesh_state_change_sensor_setting_set_t sensor_setting_set;  /*!< Sensor Setting Set */
} esp_ble_mesh_sensor_server_state_change_t;

/** Context of the received Sensor Descriptor Get message */
typedef struct {
    bool  op_en;    /*!< Indicate if optional parameters are included */
    uint16_t property_id;   /*!< Property ID of a sensor (optional) */
} esp_ble_mesh_server_recv_sensor_descriptor_get_t;

/** Context of the received Sensor Cadence Get message */
typedef struct {
    uint16_t property_id;   /*!< Property ID of a sensor */
} esp_ble_mesh_server_recv_sensor_cadence_get_t;

/** Context of the received Sensor Settings Get message */
typedef struct {
    uint16_t property_id;   /*!< Property ID of a sensor */
} esp_ble_mesh_server_recv_sensor_settings_get_t;

/** Context of the received Sensor Setting Get message */
typedef struct {
    uint16_t property_id;           /*!< Property ID of a sensor */
    uint16_t setting_property_id;   /*!< Setting ID identifying a setting within a sensor */
} esp_ble_mesh_server_recv_sensor_setting_get_t;

/** Context of the received Sensor Get message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included  */
    uint16_t property_id;   /*!< Property ID for the sensor (optional) */
} esp_ble_mesh_server_recv_sensor_get_t;

/** Context of the received Sensor Column Get message */
typedef struct {
    uint16_t property_id;   /*!< Property identifying a sensor */
    struct net_buf_simple *raw_value_x; /*!< Raw value identifying a column */
} esp_ble_mesh_server_recv_sensor_column_get_t;

/** Context of the received Sensor Series Get message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t property_id;   /*!< Property identifying a sensor */
    struct net_buf_simple *raw_value;   /*!< Raw value containing X1 and X2 (optional) */
} esp_ble_mesh_server_recv_sensor_series_get_t;

/**
 * @brief Sensor Server Model received get message union
 */
typedef union {
    esp_ble_mesh_server_recv_sensor_descriptor_get_t sensor_descriptor; /*!< Sensor Descriptor Get */
    esp_ble_mesh_server_recv_sensor_cadence_get_t    sensor_cadence;    /*!< Sensor Cadence Get */
    esp_ble_mesh_server_recv_sensor_settings_get_t   sensor_settings;   /*!< Sensor Settings Get */
    esp_ble_mesh_server_recv_sensor_setting_get_t    sensor_setting;    /*!< Sensor Setting Get */
    esp_ble_mesh_server_recv_sensor_get_t            sensor_data;       /*!< Sensor Get */
    esp_ble_mesh_server_recv_sensor_column_get_t     sensor_column;     /*!< Sensor Column Get */
    esp_ble_mesh_server_recv_sensor_series_get_t     sensor_series;     /*!< Sensor Series Get */
} esp_ble_mesh_sensor_server_recv_get_msg_t;

/** Context of the received Sensor Cadence Set message */
typedef struct {
    uint16_t property_id;           /*!< Property ID for the sensor */
    struct net_buf_simple *cadence; /*!< Value of Sensor Cadence state */
} esp_ble_mesh_server_recv_sensor_cadence_set_t;

/** Context of the received Sensor Setting Set message */
typedef struct {
    uint16_t property_id;           /*!< Property ID identifying a sensor */
    uint16_t setting_property_id;   /*!< Setting ID identifying a setting within a sensor */
    struct net_buf_simple *setting_raw; /*!< Raw value for the setting */
} esp_ble_mesh_server_recv_sensor_setting_set_t;

/**
 * @brief Sensor Server Model received set message union
 */
typedef union {
    esp_ble_mesh_server_recv_sensor_cadence_set_t sensor_cadence;   /*!< Sensor Cadence Set */
    esp_ble_mesh_server_recv_sensor_setting_set_t sensor_setting;   /*!< Sensor Setting Set */
} esp_ble_mesh_sensor_server_recv_set_msg_t;

/**
 * @brief Sensor Server Model callback value union
 */
typedef union {
    esp_ble_mesh_sensor_server_state_change_t state_change; /*!< ESP_BLE_MESH_SENSOR_SERVER_STATE_CHANGE_EVT */
    esp_ble_mesh_sensor_server_recv_get_msg_t get;  /*!< ESP_BLE_MESH_SENSOR_SERVER_RECV_GET_MSG_EVT */
    esp_ble_mesh_sensor_server_recv_set_msg_t set;  /*!< ESP_BLE_MESH_SENSOR_SERVER_RECV_SET_MSG_EVT */
} esp_ble_mesh_sensor_server_cb_value_t;

/** Sensor Server Model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;   /*!< Pointer to Sensor Server Models */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received messages */
    esp_ble_mesh_sensor_server_cb_value_t value;    /*!< Value of the received Sensor Messages */
} esp_ble_mesh_sensor_server_cb_param_t;

/** This enum value is the event of Sensor Server Model */
typedef enum {
    /**
     * 1. When get_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, no event will be
     *    callback to the application layer when Sensor Get messages are received.
     * 2. When set_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, this event will
     *    be callback to the application layer when Sensor Set/Set Unack messages
     *    are received.
     */
    ESP_BLE_MESH_SENSOR_SERVER_STATE_CHANGE_EVT,
    /**
     * When get_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Sensor Get messages are received.
     */
    ESP_BLE_MESH_SENSOR_SERVER_RECV_GET_MSG_EVT,
    /**
     * When set_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Sensor Set/Set Unack messages are received.
     */
    ESP_BLE_MESH_SENSOR_SERVER_RECV_SET_MSG_EVT,
    ESP_BLE_MESH_SENSOR_SERVER_EVT_MAX,
} esp_ble_mesh_sensor_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Sensor Server Model function.
 */

/**
 * @brief   Sensor Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_sensor_server_cb_t)(esp_ble_mesh_sensor_server_cb_event_t event,
                                                 esp_ble_mesh_sensor_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Sensor Server Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_sensor_server_callback(esp_ble_mesh_sensor_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_SENSOR_MODEL_API_H_ */
