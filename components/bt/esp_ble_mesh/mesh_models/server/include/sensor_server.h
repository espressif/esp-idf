/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SENSOR_SERVER_H_
#define _SENSOR_SERVER_H_

#include "server_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sensor Property ID related */
#define INVALID_SENSOR_PROPERTY_ID          0x0000

#define SENSOR_PROPERTY_ID_LEN              0x02

/* Sensor Descriptor state related */
#define SENSOR_DESCRIPTOR_LEN               0x08

#define SENSOR_UNSPECIFIED_POS_TOLERANCE    0x000
#define SENSOR_UNSPECIFIED_NEG_TOLERANCE    0x000

#define SENSOR_NOT_APPL_MEASURE_PERIOD      0x00

#define SENSOR_NOT_APPL_UPDATE_INTERVAL     0x00

/* Sensor Setting state related */
#define INVALID_SENSOR_SETTING_PROPERTY_ID  0x0000

#define SENSOR_SETTING_PROPERTY_ID_LEN      0x02
#define SENSOR_SETTING_ACCESS_LEN           0x01

#define SENSOR_SETTING_ACCESS_READ          0x01
#define SENSOR_SETTING_ACCESS_READ_WRITE    0x03

/* Sensor Cadence state related */
#define SENSOR_DIVISOR_TRIGGER_TYPE_LEN     0x01
#define SENSOR_STATUS_MIN_INTERVAL_LEN      0x01

#define SENSOR_PERIOD_DIVISOR_MAX_VALUE     15

#define SENSOR_STATUS_MIN_INTERVAL_MAX      26

#define SENSOR_STATUS_TRIGGER_TYPE_CHAR     0
#define SENSOR_STATUS_TRIGGER_TYPE_UINT16   1

#define SENSOR_STATUS_TRIGGER_UINT16_LEN    0x02

/* Sensor Data state related */
#define SENSOR_DATA_FORMAT_A                0x00
#define SENSOR_DATA_FORMAT_B                0x01

#define SENSOR_DATA_FORMAT_A_MPID_LEN       0x02
#define SENSOR_DATA_FORMAT_B_MPID_LEN       0x03

#define SENSOR_DATA_ZERO_LEN                0x7F

enum bt_mesh_sensor_sample_func {
    UNSPECIFIED,
    INSTANTANEOUS,
    ARITHMETIC_MEAN,
    RMS,
    MAXIMUM,
    MINIMUM,
    ACCUMULATED,
    COUNT,
};

struct sensor_descriptor {
    uint32_t positive_tolerance : 12,
             negative_tolerance : 12,
             sample_function : 8;
    uint8_t  measure_period;
    uint8_t  update_interval;
};

struct sensor_setting {
    uint16_t property_id;
    uint8_t  access;
    /* Or use union to include all possible types */
    struct net_buf_simple *raw;
};

struct sensor_cadence {
    uint8_t period_divisor : 7,
            trigger_type : 1;
    struct net_buf_simple *trigger_delta_down;
    struct net_buf_simple *trigger_delta_up;
    uint8_t min_interval;
    struct net_buf_simple *fast_cadence_low;
    struct net_buf_simple *fast_cadence_high;
};

struct sensor_data {
    /**
     * Format A: The Length field is a 1-based uint4 value (valid range 0x0–0xF,
     *           representing range of 1 – 16).
     * Format B: The Length field is a 1-based uint7 value (valid range 0x0–0x7F,
     *           representing range of 1 – 127). The value 0x7F represents a
     *           length of zero.
     */
    uint8_t format : 1,
            length : 7;
    struct net_buf_simple *raw_value;
};

struct sensor_series_column {
    struct net_buf_simple *raw_value_x;
    struct net_buf_simple *column_width;
    struct net_buf_simple *raw_value_y;
};

struct bt_mesh_sensor_state {
    uint16_t sensor_property_id;

    /* Constant throughout the lifetime of an element */
    struct sensor_descriptor descriptor;

    /* Multiple Sensor Setting states may be present for each sensor.
     * The Sensor Setting Property ID values shall be unique for each
     * Sensor Property ID that identifies a sensor within an element.
     */
    const uint8_t setting_count;
    struct sensor_setting *settings;

    /* The Sensor Cadence state may be not supported by sensors based
     * on device properties referencing "non-scalar characteristics"
     * such as "histograms" or "composite characteristics".
     */
    struct sensor_cadence *cadence;

    struct sensor_data sensor_data;

    /* Values measured by sensors may be organized as arrays (and
     * represented as series of columns, such as histograms).
     * 1. The Sensor Raw Value X field has a size and representation
     *    defined by the Sensor Property ID and represents the left
     *    corner of the column on the X axis.
     * 2. The Sensor Column Width field has a size and representation
     *    defined by the Sensor Property ID and represents the width
     *    of the column on the X axis.
     * 3. The Sensor Raw Value Y field has a size and representation
     *    defined by the Sensor Property ID and represents the height
     *    of the column on the Y axis.
     * Note: Values outside the bins defined by a Sensor Property are
     * not included. For example, if the histogram is defined as 3 bins
     * representing “lamp operating hours in a given temperature range”
     * and the bins are [40,60), [60, 80), and [80,100], then any hours
     * outside that [40, 100] range would not be included.
     */
    struct sensor_series_column series_column;
};

/* 1. Multiple instances of the Sensor states may be present within the
 *    same model, provided that each instance has a unique value of the
 *    Sensor Property ID to allow the instances to be differentiated.
 * 2. Note: The number of sensors within a multisensor is limited by the
 *    size of the message payload for the Sensor Descriptor Status message.
 *    A single Sensor Descriptor may be sent using a single Unsegmented
 *    Access message. Using Segmentation and Reassembly (SAR), up to 38
 *    Sensor Descriptor states may be sent.
 */

struct bt_mesh_sensor_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    const uint8_t state_count;
    struct bt_mesh_sensor_state *states;
};

struct bt_mesh_sensor_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    const uint8_t state_count;
    struct bt_mesh_sensor_state *states;
};

typedef union {
    struct {
        uint16_t id;
        uint8_t  period_divisor : 7,
                 trigger_type : 1;
        struct net_buf_simple *trigger_delta_down;
        struct net_buf_simple *trigger_delta_up;
        uint8_t min_interval;
        struct net_buf_simple *fast_cadence_low;
        struct net_buf_simple *fast_cadence_high;
    } sensor_cadence_set;
    struct {
        uint16_t id;
        uint16_t setting_id;
        struct net_buf_simple *value;
    } sensor_setting_set;
} bt_mesh_sensor_server_state_change_t;

typedef union {
    struct {
        bool     op_en;
        uint16_t id;
    } sensor_descriptor_get;
    struct {
        uint16_t id;
    } sensor_cadence_get;
    struct {
        uint16_t id;
    } sensor_settings_get;
    struct {
        uint16_t id;
        uint16_t setting_id;
    } sensor_setting_get;
    struct {
        bool     op_en;
        uint16_t id;
    } sensor_get;
    struct {
        uint16_t id;
        struct net_buf_simple *raw_x;
    } sensor_column_get;
    struct {
        bool     op_en;
        uint16_t id;
        struct net_buf_simple *raw;
    } sensor_series_get;
} bt_mesh_sensor_server_recv_get_msg_t;

typedef union {
    struct {
        uint16_t id;
        struct net_buf_simple *cadence;
    } sensor_cadence_set;
    struct {
        uint16_t id;
        uint16_t setting_id;
        struct net_buf_simple *raw;
    } sensor_setting_set;
} bt_mesh_sensor_server_recv_set_msg_t;

#ifdef __cplusplus
}
#endif

#endif /* _SENSOR_SERVER_H_ */
