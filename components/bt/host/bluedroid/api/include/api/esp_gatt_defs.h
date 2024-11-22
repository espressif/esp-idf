/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief GATT INVALID UUID. */
#define ESP_GATT_ILLEGAL_UUID               0

/** @brief GATT INVALID HANDLE. */
#define ESP_GATT_ILLEGAL_HANDLE             0

/** @brief GATT attribute max handle. */
#define ESP_GATT_ATTR_HANDLE_MAX            UC_CONFIG_BT_GATT_MAX_SR_ATTRIBUTES

/** @brief Maximum number of attributes to read in one request. */
#define ESP_GATT_MAX_READ_MULTI_HANDLES     10


/**
 * @defgroup GATT_UUIDs GATT Service UUIDs
 * @brief Definitions of GATT Service UUIDs.
 *
 * This module contains the definitions of standard GATT service UUIDs. These UUIDs
 * identify the type of GATT service.
 * @{
 */

/** @brief Immediate Alert Service UUID. */
#define ESP_GATT_UUID_IMMEDIATE_ALERT_SVC           0x1802
/** @brief Link Loss Service UUID. */
#define ESP_GATT_UUID_LINK_LOSS_SVC                 0x1803
/** @brief TX Power Service UUID. */
#define ESP_GATT_UUID_TX_POWER_SVC                  0x1804
/** @brief Current Time Service UUID. */
#define ESP_GATT_UUID_CURRENT_TIME_SVC              0x1805
/** @brief Reference Time Update Service UUID. */
#define ESP_GATT_UUID_REF_TIME_UPDATE_SVC           0x1806
/** @brief Next DST Change Service UUID. */
#define ESP_GATT_UUID_NEXT_DST_CHANGE_SVC           0x1807
/** @brief Glucose Service UUID. */
#define ESP_GATT_UUID_GLUCOSE_SVC                   0x1808
/** @brief Health Thermometer Service UUID. */
#define ESP_GATT_UUID_HEALTH_THERMOM_SVC            0x1809
/** @brief Device Information Service UUID. */
#define ESP_GATT_UUID_DEVICE_INFO_SVC               0x180A
/** @brief Heart Rate Service UUID. */
#define ESP_GATT_UUID_HEART_RATE_SVC                0x180D
/** @brief Phone Alert Status Service UUID. */
#define ESP_GATT_UUID_PHONE_ALERT_STATUS_SVC        0x180E
/** @brief Battery Service UUID. */
#define ESP_GATT_UUID_BATTERY_SERVICE_SVC           0x180F
/** @brief Blood Pressure Service UUID. */
#define ESP_GATT_UUID_BLOOD_PRESSURE_SVC            0x1810
/** @brief Alert Notification Service UUID. */
#define ESP_GATT_UUID_ALERT_NTF_SVC                 0x1811
/** @brief HID Service UUID. */
#define ESP_GATT_UUID_HID_SVC                       0x1812
/** @brief Scan Parameters Service UUID. */
#define ESP_GATT_UUID_SCAN_PARAMETERS_SVC           0x1813
/** @brief Running Speed and Cadence Service UUID. */
#define ESP_GATT_UUID_RUNNING_SPEED_CADENCE_SVC     0x1814
/** @brief Automation IO Service UUID. */
#define ESP_GATT_UUID_Automation_IO_SVC             0x1815
/** @brief Cycling Speed and Cadence Service UUID. */
#define ESP_GATT_UUID_CYCLING_SPEED_CADENCE_SVC     0x1816
/** @brief Cycling Power Service UUID. */
#define ESP_GATT_UUID_CYCLING_POWER_SVC             0x1818
/** @brief Location and Navigation Service UUID. */
#define ESP_GATT_UUID_LOCATION_AND_NAVIGATION_SVC   0x1819
/** @brief Environmental Sensing Service UUID. */
#define ESP_GATT_UUID_ENVIRONMENTAL_SENSING_SVC     0x181A
/** @brief Body Composition Service UUID. */
#define ESP_GATT_UUID_BODY_COMPOSITION              0x181B
/** @brief User Data Service UUID. */
#define ESP_GATT_UUID_USER_DATA_SVC                 0x181C
/** @brief Weight Scale Service UUID. */
#define ESP_GATT_UUID_WEIGHT_SCALE_SVC              0x181D
/** @brief Bond Management Service UUID. */
#define ESP_GATT_UUID_BOND_MANAGEMENT_SVC           0x181E
/** @brief Continuous Glucose Monitoring Service UUID. */
#define ESP_GATT_UUID_CONT_GLUCOSE_MONITOR_SVC      0x181F
/** @brief Primary Service UUID. */
#define ESP_GATT_UUID_PRI_SERVICE                   0x2800
/** @brief Secondary Service UUID. */
#define ESP_GATT_UUID_SEC_SERVICE                   0x2801
/** @brief Include Service UUID. */
#define ESP_GATT_UUID_INCLUDE_SERVICE               0x2802
/** @brief Characteristic Declaration UUID. */
#define ESP_GATT_UUID_CHAR_DECLARE                  0x2803
/** @brief Characteristic Extended Properties UUID. */
#define ESP_GATT_UUID_CHAR_EXT_PROP                 0x2900
/** @brief Characteristic User Description UUID. */
#define ESP_GATT_UUID_CHAR_DESCRIPTION              0x2901
/** @brief Client Characteristic Configuration UUID. */
#define ESP_GATT_UUID_CHAR_CLIENT_CONFIG            0x2902
/** @brief Server Characteristic Configuration UUID. */
#define ESP_GATT_UUID_CHAR_SRVR_CONFIG              0x2903
/** @brief Characteristic Presentation Format UUID. */
#define ESP_GATT_UUID_CHAR_PRESENT_FORMAT           0x2904
/** @brief Characteristic Aggregate Format UUID. */
#define ESP_GATT_UUID_CHAR_AGG_FORMAT               0x2905
/** @brief Characteristic Valid Range UUID. */
#define ESP_GATT_UUID_CHAR_VALID_RANGE              0x2906
/** @brief External Report Reference Descriptor UUID. */
#define ESP_GATT_UUID_EXT_RPT_REF_DESCR             0x2907
/** @brief Report Reference Descriptor UUID. */
#define ESP_GATT_UUID_RPT_REF_DESCR                 0x2908
/** @brief Number of Digitals Descriptor UUID. */
#define ESP_GATT_UUID_NUM_DIGITALS_DESCR            0x2909
/** @brief Value Trigger Setting Descriptor UUID. */
#define ESP_GATT_UUID_VALUE_TRIGGER_DESCR           0x290A
/** @brief Environmental Sensing Configuration Descriptor UUID. */
#define ESP_GATT_UUID_ENV_SENSING_CONFIG_DESCR      0x290B
/** @brief Environmental Sensing Measurement Descriptor UUID. */
#define ESP_GATT_UUID_ENV_SENSING_MEASUREMENT_DESCR 0x290C
/** @brief Environmental Sensing Trigger Setting Descriptor UUID. */
#define ESP_GATT_UUID_ENV_SENSING_TRIGGER_DESCR     0x290D
/** @brief Time Trigger Setting Descriptor UUID. */
#define ESP_GATT_UUID_TIME_TRIGGER_DESCR            0x290E

/* GAP Profile Attributes */
/** @brief GAP Device Name UUID. */
#define ESP_GATT_UUID_GAP_DEVICE_NAME               0x2A00
/** @brief GAP Icon UUID. */
#define ESP_GATT_UUID_GAP_ICON                      0x2A01
/** @brief GAP Preferred Connection Parameters UUID. */
#define ESP_GATT_UUID_GAP_PREF_CONN_PARAM           0x2A04
/** @brief GAP Central Address Resolution UUID. */
#define ESP_GATT_UUID_GAP_CENTRAL_ADDR_RESOL        0x2AA6

/* Attribute Profile Attribute UUID */
/** @brief GATT Service Changed UUID. */
#define ESP_GATT_UUID_GATT_SRV_CHGD                 0x2A05

/* Link Loss Service */
/** @brief Alert Level UUID. */
#define ESP_GATT_UUID_ALERT_LEVEL                   0x2A06
/** @brief TX Power Level UUID. */
#define ESP_GATT_UUID_TX_POWER_LEVEL                0x2A07

/* Current Time Service */
/** @brief Current Time UUID. */
#define ESP_GATT_UUID_CURRENT_TIME                  0x2A2B
/** @brief Local Time Info UUID. */
#define ESP_GATT_UUID_LOCAL_TIME_INFO               0x2A0F
/** @brief Reference Time Information UUID. */
#define ESP_GATT_UUID_REF_TIME_INFO                 0x2A14

/* Network Availability Service */
/** @brief Network Availability Status UUID. */
#define ESP_GATT_UUID_NW_STATUS                     0x2A18
/** @brief Network Availability Trigger UUID. */
#define ESP_GATT_UUID_NW_TRIGGER                    0x2A1A

/* Phone Alert Status Service */
/** @brief Alert Status UUID. */
#define ESP_GATT_UUID_ALERT_STATUS                  0x2A3F
/** @brief Ringer Control Point UUID. */
#define ESP_GATT_UUID_RINGER_CP                     0x2A40
/** @brief Ringer Setting UUID. */
#define ESP_GATT_UUID_RINGER_SETTING                0x2A41

/* Glucose Service */
/** @brief Glucose Measurement Characteristic UUID. */
#define ESP_GATT_UUID_GM_MEASUREMENT                0x2A18
/** @brief Glucose Measurement Context Characteristic UUID. */
#define ESP_GATT_UUID_GM_CONTEXT                    0x2A34
/** @brief Glucose Control Point Characteristic UUID. */
#define ESP_GATT_UUID_GM_CONTROL_POINT              0x2A52
/** @brief Glucose Feature Characteristic UUID. */
#define ESP_GATT_UUID_GM_FEATURE                    0x2A51

/* Device Information Service Characteristics */
/** @brief System ID Characteristic UUID. */
#define ESP_GATT_UUID_SYSTEM_ID                     0x2A23
/** @brief Model Number String Characteristic UUID. */
#define ESP_GATT_UUID_MODEL_NUMBER_STR              0x2A24
/** @brief Serial Number String Characteristic UUID. */
#define ESP_GATT_UUID_SERIAL_NUMBER_STR             0x2A25
/** @brief Firmware Revision String Characteristic UUID. */
#define ESP_GATT_UUID_FW_VERSION_STR                0x2A26
/** @brief Hardware Revision String Characteristic UUID. */
#define ESP_GATT_UUID_HW_VERSION_STR                0x2A27
/** @brief Software Revision String Characteristic UUID. */
#define ESP_GATT_UUID_SW_VERSION_STR                0x2A28
/** @brief Manufacturer Name String Characteristic UUID. */
#define ESP_GATT_UUID_MANU_NAME                     0x2A29
/** @brief IEEE 11073-20601 Regulatory Certification Data List Characteristic UUID. */
#define ESP_GATT_UUID_IEEE_DATA                     0x2A2A
/** @brief PnP ID Characteristic UUID. */
#define ESP_GATT_UUID_PNP_ID                        0x2A50

/* HID Service Characteristics */
/** @brief HID Information Characteristic UUID. */
#define ESP_GATT_UUID_HID_INFORMATION               0x2A4A
/** @brief HID Report Map Characteristic UUID. */
#define ESP_GATT_UUID_HID_REPORT_MAP                0x2A4B
/** @brief HID Control Point Characteristic UUID. */
#define ESP_GATT_UUID_HID_CONTROL_POINT             0x2A4C
/** @brief HID Report Characteristic UUID. */
#define ESP_GATT_UUID_HID_REPORT                    0x2A4D
/** @brief HID Protocol Mode Characteristic UUID. */
#define ESP_GATT_UUID_HID_PROTO_MODE                0x2A4E
/** @brief HID Bluetooth Keyboard Input Characteristic UUID. */
#define ESP_GATT_UUID_HID_BT_KB_INPUT               0x2A22
/** @brief HID Bluetooth Keyboard Output Characteristic UUID. */
#define ESP_GATT_UUID_HID_BT_KB_OUTPUT              0x2A32
/** @brief HID Bluetooth Mouse Input Characteristic UUID. */
#define ESP_GATT_UUID_HID_BT_MOUSE_INPUT            0x2A33

/* Heart Rate Service Characteristics */
/** @brief Heart Rate Measurement Characteristic UUID. */
#define ESP_GATT_HEART_RATE_MEAS                    0x2A37
/** @brief Body Sensor Location Characteristic UUID. */
#define ESP_GATT_BODY_SENSOR_LOCATION               0x2A38
/** @brief Heart Rate Control Point Characteristic UUID. */
#define ESP_GATT_HEART_RATE_CNTL_POINT              0x2A39

/* Battery Service Characteristics */
/** @brief Battery Level Characteristic UUID. */
#define ESP_GATT_UUID_BATTERY_LEVEL                 0x2A19

/* Sensor Service Characteristics */
/** @brief Sensor Control Point Characteristic UUID. */
#define ESP_GATT_UUID_SC_CONTROL_POINT              0x2A55
/** @brief Sensor Location Characteristic UUID. */
#define ESP_GATT_UUID_SENSOR_LOCATION               0x2A5D

/* Running Speed and Cadence Service Characteristics */
/** @brief RSC Measurement Characteristic UUID. */
#define ESP_GATT_UUID_RSC_MEASUREMENT               0x2A53
/** @brief RSC Feature Characteristic UUID. */
#define ESP_GATT_UUID_RSC_FEATURE                   0x2A54

/* Cycling Speed and Cadence Service Characteristics */
/** @brief CSC Measurement Characteristic UUID. */
#define ESP_GATT_UUID_CSC_MEASUREMENT               0x2A5B
/** @brief CSC Feature Characteristic UUID. */
#define ESP_GATT_UUID_CSC_FEATURE                   0x2A5C

/* Scan Parameters Service Characteristics */
/** @brief Scan Interval Window Characteristic UUID. */
#define ESP_GATT_UUID_SCAN_INT_WINDOW               0x2A4F
/** @brief Scan Refresh UUID. */
#define ESP_GATT_UUID_SCAN_REFRESH                  0x2A31
/* Additional GATT Services not covered yet */
/** @} */ // End of group GATT_UUIDs


/**
 * @brief Defines the attribute write operation types from the client.
 *
 * These values are used to specify the type of write operation in a prepare write sequence.
 * relate to BTA_GATT_PREP_WRITE_xxx in bta/bta_gatt_api.h.
 */
typedef enum {
    ESP_GATT_PREP_WRITE_CANCEL    = 0x00, /*!< Prepare write cancel. Corresponds to BTA_GATT_PREP_WRITE_CANCEL. */
    ESP_GATT_PREP_WRITE_EXEC      = 0x01, /*!< Prepare write execute. Corresponds to BTA_GATT_PREP_WRITE_EXEC. */
} esp_gatt_prep_write_type;


/**
 * @brief GATT operation status codes.
 *
 * These status codes are used to indicate the result of various GATT operations.
 * relate to BTA_GATT_xxx in bta/bta_gatt_api.h .
 */
typedef enum {
    ESP_GATT_OK                     =   0x0,    /*!< 0x0, Operation successful. Corresponds to BTA_GATT_OK. */
    ESP_GATT_INVALID_HANDLE         =   0x01,   /*!< 0x01, Invalid handle. Corresponds to BTA_GATT_INVALID_HANDLE. */
    ESP_GATT_READ_NOT_PERMIT        =   0x02,   /*!< 0x02, Read operation not permitted. Corresponds to BTA_GATT_READ_NOT_PERMIT. */
    ESP_GATT_WRITE_NOT_PERMIT       =   0x03,   /*!< 0x03, Write operation not permitted. Corresponds to BTA_GATT_WRITE_NOT_PERMIT. */
    ESP_GATT_INVALID_PDU            =   0x04,   /*!< 0x04, Invalid PDU. Corresponds to BTA_GATT_INVALID_PDU. */
    ESP_GATT_INSUF_AUTHENTICATION   =   0x05,   /*!< 0x05, Insufficient authentication. Corresponds to BTA_GATT_INSUF_AUTHENTICATION. */
    ESP_GATT_REQ_NOT_SUPPORTED      =   0x06,   /*!< 0x06, Request not supported. Corresponds to BTA_GATT_REQ_NOT_SUPPORTED. */
    ESP_GATT_INVALID_OFFSET         =   0x07,   /*!< 0x07, Invalid offset. Corresponds to BTA_GATT_INVALID_OFFSET. */
    ESP_GATT_INSUF_AUTHORIZATION    =   0x08,   /*!< 0x08, Insufficient authorization. Corresponds to BTA_GATT_INSUF_AUTHORIZATION. */
    ESP_GATT_PREPARE_Q_FULL         =   0x09,   /*!< 0x09, Prepare queue full. Corresponds to BTA_GATT_PREPARE_Q_FULL. */
    ESP_GATT_NOT_FOUND              =   0x0a,   /*!< 0x0a, Not found. Corresponds to BTA_GATT_NOT_FOUND. */
    ESP_GATT_NOT_LONG               =   0x0b,   /*!< 0x0b, Not long. Corresponds to BTA_GATT_NOT_LONG. */
    ESP_GATT_INSUF_KEY_SIZE         =   0x0c,   /*!< 0x0c, Insufficient key size. Corresponds to BTA_GATT_INSUF_KEY_SIZE. */
    ESP_GATT_INVALID_ATTR_LEN       =   0x0d,   /*!< 0x0d, Invalid attribute length. Corresponds to BTA_GATT_INVALID_ATTR_LEN. */
    ESP_GATT_ERR_UNLIKELY           =   0x0e,   /*!< 0x0e, Unlikely error. Corresponds to BTA_GATT_ERR_UNLIKELY. */
    ESP_GATT_INSUF_ENCRYPTION       =   0x0f,   /*!< 0x0f, Insufficient encryption. Corresponds to BTA_GATT_INSUF_ENCRYPTION. */
    ESP_GATT_UNSUPPORT_GRP_TYPE     =   0x10,   /*!< 0x10, Unsupported group type. Corresponds to BTA_GATT_UNSUPPORT_GRP_TYPE. */
    ESP_GATT_INSUF_RESOURCE         =   0x11,   /*!< 0x11, Insufficient resource. Corresponds to BTA_GATT_INSUF_RESOURCE. */

    /* Additional error codes specific to implementation or future use */
    ESP_GATT_NO_RESOURCES           =   0x80,   /*!< 0x80, No resources. Corresponds to BTA_GATT_NO_RESOURCES. */
    ESP_GATT_INTERNAL_ERROR         =   0x81,   /*!< 0x81, Internal error. Corresponds to BTA_GATT_INTERNAL_ERROR. */
    ESP_GATT_WRONG_STATE            =   0x82,   /*!< 0x82, Wrong state. Corresponds to BTA_GATT_WRONG_STATE. */
    ESP_GATT_DB_FULL                =   0x83,   /*!< 0x83, Database full. Corresponds to BTA_GATT_DB_FULL. */
    ESP_GATT_BUSY                   =   0x84,   /*!< 0x84, Busy. Corresponds to BTA_GATT_BUSY. */
    ESP_GATT_ERROR                  =   0x85,   /*!< 0x85, Generic error. Corresponds to BTA_GATT_ERROR. */
    ESP_GATT_CMD_STARTED            =   0x86,   /*!< 0x86, Command started. Corresponds to BTA_GATT_CMD_STARTED. */
    ESP_GATT_ILLEGAL_PARAMETER      =   0x87,   /*!< 0x87, Illegal parameter. Corresponds to BTA_GATT_ILLEGAL_PARAMETER. */
    ESP_GATT_PENDING                =   0x88,   /*!< 0x88, Operation pending. Corresponds to BTA_GATT_PENDING. */
    ESP_GATT_AUTH_FAIL              =   0x89,   /*!< 0x89, Authentication failed. Corresponds to BTA_GATT_AUTH_FAIL. */
    ESP_GATT_MORE                   =   0x8a,   /*!< 0x8a, More data available. Corresponds to BTA_GATT_MORE. */
    ESP_GATT_INVALID_CFG            =   0x8b,   /*!< 0x8b, Invalid configuration. Corresponds to BTA_GATT_INVALID_CFG. */
    ESP_GATT_SERVICE_STARTED        =   0x8c,   /*!< 0x8c, Service started. Corresponds to BTA_GATT_SERVICE_STARTED. */
    ESP_GATT_ENCRYPTED_MITM         =   ESP_GATT_OK, /*!< 0x0, Encrypted, with MITM protection. Corresponds to BTA_GATT_ENCRYPTED_MITM. */
    ESP_GATT_ENCRYPTED_NO_MITM      =   0x8d,   /*!< 0x8d, Encrypted, without MITM protection. Corresponds to BTA_GATT_ENCRYPTED_NO_MITM. */
    ESP_GATT_NOT_ENCRYPTED          =   0x8e,   /*!< 0x8e, Not encrypted. Corresponds to BTA_GATT_NOT_ENCRYPTED. */
    ESP_GATT_CONGESTED              =   0x8f,   /*!< 0x8f, Congested. Corresponds to BTA_GATT_CONGESTED. */
    ESP_GATT_DUP_REG                =   0x90,   /*!< 0x90, Duplicate registration. Corresponds to BTA_GATT_DUP_REG. */
    ESP_GATT_ALREADY_OPEN           =   0x91,   /*!< 0x91, Already open. Corresponds to BTA_GATT_ALREADY_OPEN. */
    ESP_GATT_CANCEL                 =   0x92,   /*!< 0x92, Operation cancelled. Corresponds to BTA_GATT_CANCEL. */
    /* 0xE0 ~ 0xFC reserved for future use */
    ESP_GATT_STACK_RSP              =   0xe0,   /*!< 0xe0, Stack response. Corresponds to BTA_GATT_STACK_RSP. */
    ESP_GATT_APP_RSP                =   0xe1,   /*!< 0xe1, Application response. Corresponds to BTA_GATT_APP_RSP. */
    /* Error caused by customer application or stack bug */
    ESP_GATT_UNKNOWN_ERROR          =   0xef,   /*!< 0xef, Unknown error. Corresponds to BTA_GATT_UNKNOWN_ERROR. */
    ESP_GATT_CCC_CFG_ERR            =   0xfd,   /*!< 0xfd, Client Characteristic Configuration Descriptor improperly configured. Corresponds to BTA_GATT_CCC_CFG_ERR. */
    ESP_GATT_PRC_IN_PROGRESS        =   0xfe,   /*!< 0xfe, Procedure already in progress. Corresponds to BTA_GATT_PRC_IN_PROGRESS. */
    ESP_GATT_OUT_OF_RANGE           =   0xff    /*!< 0xff, Attribute value out of range. Corresponds to BTA_GATT_OUT_OF_RANGE. */
} esp_gatt_status_t;


/**
 * @brief Enumerates reasons for GATT connection.
 */
typedef enum {
    ESP_GATT_CONN_UNKNOWN = 0,                      /*!< Unknown connection reason. Corresponds to BTA_GATT_CONN_UNKNOWN in bta/bta_gatt_api.h */
    ESP_GATT_CONN_L2C_FAILURE = 1,                  /*!< General L2CAP failure. Corresponds to BTA_GATT_CONN_L2C_FAILURE in bta/bta_gatt_api.h */
    ESP_GATT_CONN_TIMEOUT = 0x08,                   /*!< Connection timeout. Corresponds to BTA_GATT_CONN_TIMEOUT in bta/bta_gatt_api.h */
    ESP_GATT_CONN_TERMINATE_PEER_USER = 0x13,       /*!< Connection terminated by peer user. Corresponds to BTA_GATT_CONN_TERMINATE_PEER_USER in bta/bta_gatt_api.h */
    ESP_GATT_CONN_TERMINATE_LOCAL_HOST = 0x16,      /*!< Connection terminated by local host. Corresponds to BTA_GATT_CONN_TERMINATE_LOCAL_HOST in bta/bta_gatt_api.h */
    ESP_GATT_CONN_FAIL_ESTABLISH = 0x3e,            /*!< Failure to establish connection. Corresponds to BTA_GATT_CONN_FAIL_ESTABLISH in bta/bta_gatt_api.h */
    ESP_GATT_CONN_LMP_TIMEOUT = 0x22,               /*!< Connection failed due to LMP response timeout. Corresponds to BTA_GATT_CONN_LMP_TIMEOUT in bta/bta_gatt_api.h */
    ESP_GATT_CONN_CONN_CANCEL = 0x0100,             /*!< L2CAP connection cancelled. Corresponds to BTA_GATT_CONN_CONN_CANCEL in bta/bta_gatt_api.h */
    ESP_GATT_CONN_NONE = 0x0101                     /*!< No connection to cancel. Corresponds to BTA_GATT_CONN_NONE in bta/bta_gatt_api.h */
} esp_gatt_conn_reason_t;


/**
 * @brief Represents a GATT identifier.
 */
typedef struct {
    esp_bt_uuid_t   uuid;       /*!< @brief The UUID component of the GATT ID. */
    uint8_t         inst_id;    /*!< @brief The instance ID component of the GATT ID, providing further differentiation of the GATT ID. */
} __attribute__((packed)) esp_gatt_id_t;


/**
 * @brief Represents a GATT service identifier.
 */
typedef struct {
    esp_gatt_id_t   id;            /*!< @brief Encapsulates the UUID and instance ID of the GATT service. */
    bool            is_primary;    /*!< @brief Indicates if the service is primary. A value of true means it is a primary service, false indicates a secondary service. */
} __attribute__((packed)) esp_gatt_srvc_id_t;

/**
 * @brief Defines the GATT authentication request types.
 *
 * This enumeration lists the types of authentication requests that can be made.
 * It corresponds to the `BTA_GATT_AUTH_REQ_xxx` values defined in `bta/bta_gatt_api.h`.
 * The types include options for no authentication, unauthenticated encryption, authenticated encryption,
 * and both signed versions with and without MITM (Man-In-The-Middle) protection.
 */
typedef enum {
    ESP_GATT_AUTH_REQ_NONE                  = 0, /*!< No authentication required. Corresponds to BTA_GATT_AUTH_REQ_NONE. */
    ESP_GATT_AUTH_REQ_NO_MITM               = 1, /*!< Unauthenticated encryption. Corresponds to BTA_GATT_AUTH_REQ_NO_MITM. */
    ESP_GATT_AUTH_REQ_MITM                  = 2, /*!< Authenticated encryption (MITM protection). Corresponds to BTA_GATT_AUTH_REQ_MITM. */
    ESP_GATT_AUTH_REQ_SIGNED_NO_MITM        = 3, /*!< Signed data, no MITM protection. Corresponds to BTA_GATT_AUTH_REQ_SIGNED_NO_MITM. */
    ESP_GATT_AUTH_REQ_SIGNED_MITM           = 4, /*!< Signed data with MITM protection. Corresponds to BTA_GATT_AUTH_REQ_SIGNED_MITM. */
} esp_gatt_auth_req_t;


/**
 * @brief Defines GATT attribute permission flags.
 *
 * These permission flags are used to specify the security requirements for GATT attributes.
 * They correlate directly with the BTA_GATT_PERM_xxx definitions found in bta/bta_gatt_api.h.
 */

/** @defgroup GATT_PERMS GATT Attribute Permissions
 *  @brief Definitions of permission flags for GATT attributes.
 *  @{
 */

/** @brief Permission to read the attribute. Corresponds to BTA_GATT_PERM_READ. */
#define ESP_GATT_PERM_READ                  (1 << 0)

/** @brief Permission to read the attribute with encryption. Corresponds to BTA_GATT_PERM_READ_ENCRYPTED. */
#define ESP_GATT_PERM_READ_ENCRYPTED        (1 << 1)

/** @brief Permission to read the attribute with encrypted MITM (Man In The Middle) protection. Corresponds to BTA_GATT_PERM_READ_ENC_MITM.*/
#define ESP_GATT_PERM_READ_ENC_MITM         (1 << 2)

/** @brief Permission to write to the attribute. Corresponds to BTA_GATT_PERM_WRITE. */
#define ESP_GATT_PERM_WRITE                 (1 << 4)

/** @brief Permission to write to the attribute with encryption. Corresponds to BTA_GATT_PERM_WRITE_ENCRYPTED. */
#define ESP_GATT_PERM_WRITE_ENCRYPTED       (1 << 5)

/** @brief Permission to write to the attribute with encrypted MITM protection. Corresponds to BTA_GATT_PERM_WRITE_ENC_MITM. */
#define ESP_GATT_PERM_WRITE_ENC_MITM        (1 << 6)

/** @brief Permission for signed writes to the attribute. Corresponds to BTA_GATT_PERM_WRITE_SIGNED. */
#define ESP_GATT_PERM_WRITE_SIGNED          (1 << 7)

/** @brief Permission for signed writes to the attribute with MITM protection. Corresponds to BTA_GATT_PERM_WRITE_SIGNED_MITM. */
#define ESP_GATT_PERM_WRITE_SIGNED_MITM     (1 << 8)

/** @brief Permission to read the attribute with authorization. */
#define ESP_GATT_PERM_READ_AUTHORIZATION    (1 << 9)

/** @brief Permission to write to the attribute with authorization. */
#define ESP_GATT_PERM_WRITE_AUTHORIZATION   (1 << 10)

/**
 * @brief Macro to specify minimum encryption key size.
 *
 * @param keysize The minimum size of the encryption key, in bytes.
 */
#define ESP_GATT_PERM_ENCRYPT_KEY_SIZE(keysize) (((keysize - 6) & 0xF) << 12)

/** @} */ // End of GATT_PERMS group

typedef uint16_t esp_gatt_perm_t; ///< Type to represent GATT attribute permissions.



/**
 * @brief Defines GATT characteristic properties.
 *
 * These properties are related to `BTA_GATT_CHAR_PROP_BIT_xxx` in `bta/bta_gatt_api.h`.
 */

/** @defgroup GATT_CHAR_PROPERTIES GATT Characteristic Properties
 *  These properties define various capabilities of a GATT characteristic.
 *  @{
 */
/** @brief Ability to broadcast.Corresponds to BTA_GATT_CHAR_PROP_BIT_BROADCAST. */
#define ESP_GATT_CHAR_PROP_BIT_BROADCAST    (1 << 0)

/** @brief Ability to read.Corresponds to BTA_GATT_CHAR_PROP_BIT_READ. */
#define ESP_GATT_CHAR_PROP_BIT_READ         (1 << 1)

/** @brief Ability to write without response.Corresponds to BTA_GATT_CHAR_PROP_BIT_WRITE_NR. */
#define ESP_GATT_CHAR_PROP_BIT_WRITE_NR     (1 << 2)

/** @brief Ability to write.Corresponds to BTA_GATT_CHAR_PROP_BIT_WRITE. */
#define ESP_GATT_CHAR_PROP_BIT_WRITE        (1 << 3)

/** @brief Ability to notify.Corresponds to BTA_GATT_CHAR_PROP_BIT_NOTIFY. */
#define ESP_GATT_CHAR_PROP_BIT_NOTIFY       (1 << 4)

/** @brief Ability to indicate.Corresponds to BTA_GATT_CHAR_PROP_BIT_INDICATE. */
#define ESP_GATT_CHAR_PROP_BIT_INDICATE     (1 << 5)

/** @brief Ability to authenticate.Corresponds to BTA_GATT_CHAR_PROP_BIT_AUTH. */
#define ESP_GATT_CHAR_PROP_BIT_AUTH         (1 << 6)

/** @brief Has extended properties.Corresponds to BTA_GATT_CHAR_PROP_BIT_EXT_PROP. */
#define ESP_GATT_CHAR_PROP_BIT_EXT_PROP     (1 << 7)

/** @} */ // end of GATT_CHAR_PROPERTIES

/**
 * @typedef esp_gatt_char_prop_t
 * @brief Type for characteristic properties bitmask.
 */
typedef uint8_t esp_gatt_char_prop_t;


/**
 * @brief Defines the maximum length of a GATT attribute.
 *
 * This definition specifies the maximum number of bytes that a GATT attribute can hold.
 */
#define ESP_GATT_MAX_ATTR_LEN   512 /*!< As same as GATT_MAX_ATTR_LEN. */

/**
 * @brief Enumerates the possible sources of a GATT service discovery.
 *
 * This enumeration identifies the source of a GATT service discovery process,
 * indicating whether the service information was obtained from a remote device,
 * from NVS (Non-Volatile Storage) flash, or the source is unknown.
 */
typedef enum {
    ESP_GATT_SERVICE_FROM_REMOTE_DEVICE = 0,    /*!< Service information from a remote device. Relates to BTA_GATTC_SERVICE_INFO_FROM_REMOTE_DEVICE. */
    ESP_GATT_SERVICE_FROM_NVS_FLASH     = 1,    /*!< Service information from NVS flash. Relates to BTA_GATTC_SERVICE_INFO_FROM_NVS_FLASH. */
    ESP_GATT_SERVICE_FROM_UNKNOWN       = 2,    /*!< Service source is unknown. Relates to BTA_GATTC_SERVICE_INFO_FROM_UNKNOWN. */
} esp_service_source_t;


/**
 * @brief Defines an attribute's description.
 *
 * This structure is used to describe an attribute in the GATT database. It includes
 * details such as the UUID of the attribute, its permissions, and its value.
 */
typedef struct
{
    uint16_t uuid_length;              /*!< Length of the UUID in bytes. */
    uint8_t  *uuid_p;                  /*!< Pointer to the UUID value. */
    uint16_t perm;                     /*!< Attribute permissions, defined by esp_gatt_perm_t. */
    uint16_t max_length;               /*!< Maximum length of the attribute's value. */
    uint16_t length;                   /*!< Current length of the attribute's value. */
    uint8_t  *value;                   /*!< Pointer to the attribute's value array. */
} esp_attr_desc_t;


/**
 * @brief Defines attribute control for GATT operations.
 *
 * This module provides definitions for controlling attribute auto responses
 * in GATT operations.
 */

/** @brief Response to Write/Read operations should be handled by the application. */
#define ESP_GATT_RSP_BY_APP 0

/** @brief Response to Write/Read operations should be automatically handled by the GATT stack. */
#define ESP_GATT_AUTO_RSP 1

/**
 * @brief Defines the auto response setting for attribute operations.
 *
 * This structure is used to control whether the GATT stack or the application
 * will handle responses to Read/Write operations.
 */
typedef struct
{
    /**
     * @brief Controls who handles the response to Read/Write operations.
     *
     * - If set to @c ESP_GATT_RSP_BY_APP, the application is responsible for
     *   generating the response.
     * - If set to @c ESP_GATT_AUTO_RSP, the GATT stack will automatically generate
     *   the response.
     */
    uint8_t auto_rsp;
} esp_attr_control_t;



/**
 * @brief attribute type added to the GATT server database
 */
typedef struct
{
    esp_attr_control_t      attr_control;                   /*!< The attribute control type */
    esp_attr_desc_t         att_desc;                       /*!< The attribute type */
} esp_gatts_attr_db_t;


/**
  * @brief set the attribute value type
  */
typedef struct
{
    uint16_t attr_max_len;                                  /*!<  attribute max value length */
    uint16_t attr_len;                                      /*!<  attribute current value length */
    uint8_t  *attr_value;                                   /*!<  the pointer to attribute value */
} esp_attr_value_t;


/**
  * @brief Gatt  include service entry element
  */
typedef struct
{
    uint16_t start_hdl;                                     /*!< Gatt  start handle value of included service */
    uint16_t end_hdl;                                       /*!< Gatt  end handle value of included service */
    uint16_t uuid;                                          /*!< Gatt  attribute value UUID of included service */
} esp_gatts_incl_svc_desc_t;                                /*!< Gatt include service entry element */

/**
  * @brief Gatt  include 128 bit service entry element
  */
typedef struct
{
    uint16_t start_hdl;                                     /*!< Gatt  start handle value of included 128 bit service */
    uint16_t end_hdl;                                       /*!< Gatt  end handle value of included 128 bit service */
} esp_gatts_incl128_svc_desc_t;                             /*!< Gatt  include 128 bit service entry element */

/**
 * @brief Represents a GATT attribute's value.
 */
typedef struct {
    uint8_t  value[ESP_GATT_MAX_ATTR_LEN]; /*!< Array holding the value of the GATT attribute. */
    uint16_t handle;                       /*!< Unique identifier (handle) of the GATT attribute. */
    uint16_t offset;                       /*!< Offset within the attribute's value, for partial updates. */
    uint16_t len;                          /*!< Current length of the data in the value array. */
    uint8_t  auth_req;                     /*!< Authentication requirements for accessing this attribute. */
} esp_gatt_value_t;

/**
 * @brief Represents the response type for a GATT remote read request.
 */
typedef union {
    esp_gatt_value_t attr_value; /*!< The GATT attribute value, including its data, handle, and metadata. */
    uint16_t         handle;     /*!< Only the handle of the GATT attribute, when that's the only required information. */
} esp_gatt_rsp_t;


/**
 * @brief Defines the types of GATT write operations.
 */
typedef enum {
    ESP_GATT_WRITE_TYPE_NO_RSP  =   1,  /*!< Write operation where no response is needed. */
    ESP_GATT_WRITE_TYPE_RSP     =   2,  /*!< Write operation that requires a remote response. */
} esp_gatt_write_type_t;


/** @brief Connection parameters for GATT. */
typedef struct {
    uint16_t interval;                         /*!< Connection interval. */
    uint16_t latency;                          /*!< Slave latency for the connection in number of connection events. */
    uint16_t timeout;                          /*!< Supervision timeout for the LE Link. */
} esp_gatt_conn_params_t;

/** @brief Macro indicating no specific GATT interface. */
#define ESP_GATT_IF_NONE    0xff              /*!< No specific application GATT interface. */

/** @brief GATT interface type for client applications. */
typedef uint8_t esp_gatt_if_t;

/** @brief Enumerates types of GATT database attributes. */
typedef enum {
    ESP_GATT_DB_PRIMARY_SERVICE,              /*!< Primary service attribute. */
    ESP_GATT_DB_SECONDARY_SERVICE,            /*!< Secondary service attribute. */
    ESP_GATT_DB_CHARACTERISTIC,               /*!< Characteristic attribute. */
    ESP_GATT_DB_DESCRIPTOR,                   /*!< Descriptor attribute. */
    ESP_GATT_DB_INCLUDED_SERVICE,             /*!< Included service attribute. */
    ESP_GATT_DB_ALL,                          /*!< All attribute types. */
} esp_gatt_db_attr_type_t;

/** @brief Represents multiple attributes for reading. */
typedef struct {
    uint8_t  num_attr;                        /*!< Number of attributes. */
    uint16_t handles[ESP_GATT_MAX_READ_MULTI_HANDLES]; /*!< List of attribute handles. */
} esp_gattc_multi_t;

/** @brief GATT database attribute element. */
typedef struct {
    esp_gatt_db_attr_type_t type;             /*!< Attribute type. */
    uint16_t attribute_handle;                /*!< Attribute handle. */
    uint16_t start_handle;                    /*!< Service start handle. */
    uint16_t end_handle;                      /*!< Service end handle. */
    esp_gatt_char_prop_t properties;          /*!< Characteristic properties. */
    esp_bt_uuid_t uuid;                       /*!< Attribute UUID. */
} esp_gattc_db_elem_t;

/** @brief Represents a GATT service element. */
typedef struct {
    bool is_primary;                          /*!< Indicates if the service is primary. */
    uint16_t start_handle;                    /*!< Service start handle. */
    uint16_t end_handle;                      /*!< Service end handle. */
    esp_bt_uuid_t uuid;                       /*!< Service UUID. */
} esp_gattc_service_elem_t;

/** @brief Represents a GATT characteristic element. */
typedef struct {
    uint16_t char_handle;                     /*!< Characteristic handle. */
    esp_gatt_char_prop_t properties;          /*!< Characteristic properties. */
    esp_bt_uuid_t uuid;                       /*!< Characteristic UUID. */
} esp_gattc_char_elem_t;

/** @brief Represents a GATT descriptor element. */
typedef struct {
    uint16_t handle;                          /*!< Descriptor handle. */
    esp_bt_uuid_t uuid;                       /*!< Descriptor UUID. */
} esp_gattc_descr_elem_t;

/** @brief Represents an included GATT service element. */
typedef struct {
    uint16_t handle;                          /*!< Current attribute handle of the included service. */
    uint16_t incl_srvc_s_handle;              /*!< Start handle of the included service. */
    uint16_t incl_srvc_e_handle;              /*!< End handle of the included service. */
    esp_bt_uuid_t uuid;                       /*!< Included service UUID. */
} esp_gattc_incl_svc_elem_t;

/** @brief Represents a creat connection element. */
typedef struct {
    esp_bd_addr_t remote_bda;                              /*!< The Bluetooth address of the remote device */
    esp_ble_addr_type_t remote_addr_type;                  /*!< Address type of the remote device */
    bool is_direct;                                        /*!< Direct connection or background auto connection(by now, background auto connection is not supported */
    bool is_aux;                                           /*!< Set to true for BLE 5.0 or higher to enable auxiliary connections; set to false for BLE 4.2 or lower. */
    esp_ble_addr_type_t own_addr_type;                     /*!< Specifies the address type used in the connection request. Set to 0xFF if the address type is unknown. */
    esp_ble_phy_mask_t phy_mask;                           /*!< Indicates which PHY connection parameters will be used. When is_aux is false, only the connection params for 1M PHY can be specified */
    const esp_ble_conn_params_t *phy_1m_conn_params;       /*!< Connection parameters for the LE 1M PHY */
    const esp_ble_conn_params_t *phy_2m_conn_params;       /*!< Connection parameters for the LE 2M PHY */
    const esp_ble_conn_params_t *phy_coded_conn_params;    /*!< Connection parameters for the LE Coded PHY */
} esp_ble_gatt_creat_conn_params_t;

#ifdef __cplusplus
}
#endif
