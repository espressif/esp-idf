// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_GATT_DEFS_H__
#define __ESP_GATT_DEFS_H__

#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// GATT INVALID UUID
#define ESP_GATT_ILLEGAL_UUID                0

/**@{
 * All "ESP_GATT_UUID_xxx" is attribute types
 */
#define ESP_GATT_UUID_PRI_SERVICE            0x2800
#define ESP_GATT_UUID_SEC_SERVICE            0x2801
#define ESP_GATT_UUID_INCLUDE_SERVICE        0x2802
#define ESP_GATT_UUID_CHAR_DECLARE           0x2803      /*  Characteristic Declaration*/

#define ESP_GATT_UUID_CHAR_EXT_PROP          0x2900      /*  Characteristic Extended Properties */
#define ESP_GATT_UUID_CHAR_DESCRIPTION       0x2901      /*  Characteristic User Description*/
#define ESP_GATT_UUID_CHAR_CLIENT_CONFIG     0x2902      /*  Client Characteristic Configuration */
#define ESP_GATT_UUID_CHAR_SRVR_CONFIG       0x2903      /*  Server Characteristic Configuration */
#define ESP_GATT_UUID_CHAR_PRESENT_FORMAT    0x2904      /*  Characteristic Presentation Format*/
#define ESP_GATT_UUID_CHAR_AGG_FORMAT        0x2905      /*  Characteristic Aggregate Format*/
#define ESP_GATT_UUID_CHAR_VALID_RANGE       0x2906      /*  Characteristic Valid Range */
#define ESP_GATT_UUID_EXT_RPT_REF_DESCR      0x2907
#define ESP_GATT_UUID_RPT_REF_DESCR          0x2908

/* GAP Profile Attributes */
#define ESP_GATT_UUID_GAP_DEVICE_NAME        0x2A00
#define ESP_GATT_UUID_GAP_ICON               0x2A01
#define ESP_GATT_UUID_GAP_PREF_CONN_PARAM    0x2A04
#define ESP_GATT_UUID_GAP_CENTRAL_ADDR_RESOL 0x2AA6

/* Attribute Profile Attribute UUID */
#define ESP_GATT_UUID_GATT_SRV_CHGD          0x2A05

/* Link ESP_Loss Service */
#define ESP_GATT_UUID_ALERT_LEVEL            0x2A06      /* Alert Level */
#define ESP_GATT_UUID_TX_POWER_LEVEL         0x2A07      /* TX power level */

/* Current Time Service */
#define ESP_GATT_UUID_CURRENT_TIME           0x2A2B      /* Current Time */
#define ESP_GATT_UUID_LOCAL_TIME_INFO        0x2A0F      /* Local time info */
#define ESP_GATT_UUID_REF_TIME_INFO          0x2A14      /* reference time information */

/* Network availability Profile */
#define ESP_GATT_UUID_NW_STATUS              0x2A18      /* network availability status */
#define ESP_GATT_UUID_NW_TRIGGER             0x2A1A      /* Network availability trigger */

/* Phone alert */
#define ESP_GATT_UUID_ALERT_STATUS           0x2A3F    /* alert status */
#define ESP_GATT_UUID_RINGER_CP              0x2A40    /* ringer control point */
#define ESP_GATT_UUID_RINGER_SETTING         0x2A41    /* ringer setting */

/* Glucose Service */
#define ESP_GATT_UUID_GM_MEASUREMENT         0x2A18
#define ESP_GATT_UUID_GM_CONTEXT             0x2A34
#define ESP_GATT_UUID_GM_CONTROL_POINT       0x2A52
#define ESP_GATT_UUID_GM_FEATURE             0x2A51

/* device information characteristic */
#define ESP_GATT_UUID_SYSTEM_ID              0x2A23
#define ESP_GATT_UUID_MODEL_NUMBER_STR       0x2A24
#define ESP_GATT_UUID_SERIAL_NUMBER_STR      0x2A25
#define ESP_GATT_UUID_FW_VERSION_STR         0x2A26
#define ESP_GATT_UUID_HW_VERSION_STR         0x2A27
#define ESP_GATT_UUID_SW_VERSION_STR         0x2A28
#define ESP_GATT_UUID_MANU_NAME              0x2A29
#define ESP_GATT_UUID_IEEE_DATA              0x2A2A
#define ESP_GATT_UUID_PNP_ID                 0x2A50

/* HID characteristics */
#define ESP_GATT_UUID_HID_INFORMATION        0x2A4A
#define ESP_GATT_UUID_HID_REPORT_MAP         0x2A4B
#define ESP_GATT_UUID_HID_CONTROL_POINT      0x2A4C
#define ESP_GATT_UUID_HID_REPORT             0x2A4D
#define ESP_GATT_UUID_HID_PROTO_MODE         0x2A4E
#define ESP_GATT_UUID_HID_BT_KB_INPUT        0x2A22
#define ESP_GATT_UUID_HID_BT_KB_OUTPUT       0x2A32
#define ESP_GATT_UUID_HID_BT_MOUSE_INPUT     0x2A33

/* Battery Service characteristics */
#define ESP_GATT_UUID_BATTERY_LEVEL          0x2A19

/* Sensor Service */
#define ESP_GATT_UUID_SC_CONTROL_POINT       0x2A55
#define ESP_GATT_UUID_SENSOR_LOCATION        0x2A5D

/* Runners speed and cadence service */
#define ESP_GATT_UUID_RSC_MEASUREMENT        0x2A53
#define ESP_GATT_UUID_RSC_FEATURE            0x2A54

/* Cycling speed and cadence service */
#define ESP_GATT_UUID_CSC_MEASUREMENT        0x2A5B
#define ESP_GATT_UUID_CSC_FEATURE            0x2A5C

/* Scan ESP_Parameter characteristics */
#define ESP_GATT_UUID_SCAN_INT_WINDOW        0x2A4F
#define ESP_GATT_UUID_SCAN_REFRESH           0x2A31
/**
 * @}
 */

/// Attribute write data type from the client
typedef enum {
	ESP_GATT_PREP_WRITE_CANCEL    = 0x00,		/*!< Prepare write cancel */
	ESP_GATT_PREP_WRITE_EXEC      = 0x01,		/*!< Prepare write execute */
} esp_gatt_prep_write_type;

/**
 * @brief GATT success code and error codes
 */
typedef enum {
    ESP_GATT_OK                     =   0x0,
    ESP_GATT_INVALID_HANDLE         =   0x01,   /* 0x0001 */
    ESP_GATT_READ_NOT_PERMIT        =   0x02,   /* 0x0002 */
    ESP_GATT_WRITE_NOT_PERMIT       =   0x03,   /* 0x0003 */
    ESP_GATT_INVALID_PDU            =   0x04,   /* 0x0004 */
    ESP_GATT_INSUF_AUTHENTICATION   =   0x05,   /* 0x0005 */
    ESP_GATT_REQ_NOT_SUPPORTED      =   0x06,   /* 0x0006 */
    ESP_GATT_INVALID_OFFSET         =   0x07,   /* 0x0007 */
    ESP_GATT_INSUF_AUTHORIZATION    =   0x08,   /* 0x0008 */
    ESP_GATT_PREPARE_Q_FULL         =   0x09,   /* 0x0009 */
    ESP_GATT_NOT_FOUND              =   0x0a,   /* 0x000a */
    ESP_GATT_NOT_LONG               =   0x0b,   /* 0x000b */
    ESP_GATT_INSUF_KEY_SIZE         =   0x0c,   /* 0x000c */
    ESP_GATT_INVALID_ATTR_LEN       =   0x0d,   /* 0x000d */
    ESP_GATT_ERR_UNLIKELY           =   0x0e,   /* 0x000e */
    ESP_GATT_INSUF_ENCRYPTION       =   0x0f,   /* 0x000f */
    ESP_GATT_UNSUPPORT_GRP_TYPE     =   0x10,   /* 0x0010 */
    ESP_GATT_INSUF_RESOURCE         =   0x11,   /* 0x0011 */

    ESP_GATT_NO_RESOURCES           =   0x80,   /* 0x80 */
    ESP_GATT_INTERNAL_ERROR         =   0x81,   /* 0x81 */
    ESP_GATT_WRONG_STATE            =   0x82,   /* 0x82 */
    ESP_GATT_DB_FULL                =   0x83,   /* 0x83 */
    ESP_GATT_BUSY                   =   0x84,   /* 0x84 */
    ESP_GATT_ERROR                  =   0x85,   /* 0x85 */
    ESP_GATT_CMD_STARTED            =   0x86,   /* 0x86 */
    ESP_GATT_ILLEGAL_PARAMETER      =   0x87,   /* 0x87 */
    ESP_GATT_PENDING                =   0x88,   /* 0x88 */
    ESP_GATT_AUTH_FAIL              =   0x89,   /* 0x89 */
    ESP_GATT_MORE                   =   0x8a,   /* 0x8a */
    ESP_GATT_INVALID_CFG            =   0x8b,   /* 0x8b */
    ESP_GATT_SERVICE_STARTED        =   0x8c,   /* 0x8c */
    ESP_GATT_ENCRYPED_MITM          =   ESP_GATT_OK,
    ESP_GATT_ENCRYPED_NO_MITM       =   0x8d,   /* 0x8d */
    ESP_GATT_NOT_ENCRYPTED          =   0x8e,   /* 0x8e */
    ESP_GATT_CONGESTED              =   0x8f,   /* 0x8f */
    ESP_GATT_DUP_REG                =   0x90,   /* 0x90 */
    ESP_GATT_ALREADY_OPEN           =   0x91,   /* 0x91 */
    ESP_GATT_CANCEL                 =   0x92,   /* 0x92 */

    /* 0xE0 ~ 0xFC reserved for future use */
    ESP_GATT_CCC_CFG_ERR            =   0xfd,   /* 0xFD Client Characteristic Configuration Descriptor Improperly Configured */
    ESP_GATT_PRC_IN_PROGRESS        =   0xfe,   /* 0xFE Procedure Already in progress */
    ESP_GATT_OUT_OF_RANGE           =   0xff,   /* 0xFFAttribute value out of range */
} esp_gatt_status_t;

/**
 * @brief Gatt Connection reason enum
 */
typedef enum {
    ESP_GATT_CONN_UNKNOWN = 0,						/*!< Gatt connection unknown */
    ESP_GATT_CONN_L2C_FAILURE = 1,					/*!< General L2cap failure  */
    ESP_GATT_CONN_TIMEOUT = 0x08,					/*!< Connection timeout  */
    ESP_GATT_CONN_TERMINATE_PEER_USER = 0x13,		/*!< Connection terminate by peer user  */
    ESP_GATT_CONN_TERMINATE_LOCAL_HOST = 0x16,		/*!< Connectionterminated by local host */
    ESP_GATT_CONN_FAIL_ESTABLISH = 0x3e,			/*!< Connection fail to establish  */
    ESP_GATT_CONN_LMP_TIMEOUT = 0x22,				/*!< Connection fail for LMP response tout */
    ESP_GATT_CONN_CONN_CANCEL = 0x0100,				/*!< L2CAP connection cancelled  */
    ESP_GATT_CONN_NONE = 0x0101						/*!< No connection to cancel  */
} esp_gatt_conn_reason_t;

/**
 * @brief Gatt id, include uuid and instance id
 */
typedef struct {
    esp_bt_uuid_t   uuid;					/*!< UUID */
    uint8_t         inst_id;				/*!< Instance id */
} __attribute__((packed)) esp_gatt_id_t;

/**
 * @brief Gatt service id, include id
 *        (uuid and instance id) and primary flag
 */
typedef struct {
    esp_gatt_id_t   id;						/*!< Gatt id, include uuid and instance */
    bool            is_primary;				/*!< This service is primary or not */
} __attribute__((packed)) esp_gatt_srvc_id_t;

/**
 * @brief Gatt authentication request type
 */
typedef enum {
    ESP_GATT_AUTH_REQ_NONE              	= 0,
    ESP_GATT_AUTH_REQ_NO_MITM           	= 1,   /* unauthenticated encryption */
    ESP_GATT_AUTH_REQ_MITM              	= 2,   /* authenticated encryption */
    ESP_GATT_AUTH_REQ_SIGNED_NO_MITM    	= 3,
    ESP_GATT_AUTH_REQ_SIGNED_MITM       	= 4,
} esp_gatt_auth_req_t;

/**
 * @brief Attribute permissions
 */
typedef enum {
    ESP_GATT_PERM_READ                  =   (1 << 0),   /* bit 0 -  0x0001 */
    ESP_GATT_PERM_READ_ENCRYPTED        =   (1 << 1),   /* bit 1 -  0x0002 */
    ESP_GATT_PERM_READ_ENC_MITM         =   (1 << 2),   /* bit 2 -  0x0004 */
    ESP_GATT_PERM_WRITE                 =   (1 << 4),   /* bit 4 -  0x0010 */
    ESP_GATT_PERM_WRITE_ENCRYPTED       =   (1 << 5),   /* bit 5 -  0x0020 */
    ESP_GATT_PERM_WRITE_ENC_MITM        =   (1 << 6),   /* bit 6 -  0x0040 */
    ESP_GATT_PERM_WRITE_SIGNED          =   (1 << 7),   /* bit 7 -  0x0080 */
    ESP_GATT_PERM_WRITE_SIGNED_MITM     =   (1 << 8),   /* bit 8 -  0x0100 */
} esp_gatt_perm_t;

/* definition of characteristic properties */
typedef enum {
    ESP_GATT_CHAR_PROP_BIT_BROADCAST    =   (1 << 0),       /* 0x01 */
    ESP_GATT_CHAR_PROP_BIT_READ         =   (1 << 1),       /* 0x02 */
    ESP_GATT_CHAR_PROP_BIT_WRITE_NR     =   (1 << 2),       /* 0x04 */
    ESP_GATT_CHAR_PROP_BIT_WRITE        =   (1 << 3),       /* 0x08 */
    ESP_GATT_CHAR_PROP_BIT_NOTIFY       =   (1 << 4),       /* 0x10 */
    ESP_GATT_CHAR_PROP_BIT_INDICATE     =   (1 << 5),       /* 0x20 */
    ESP_GATT_CHAR_PROP_BIT_AUTH         =   (1 << 6),       /* 0x40 */
    ESP_GATT_CHAR_PROP_BIT_EXT_PROP     =   (1 << 7),       /* 0x80 */
} esp_gatt_char_prop_t;

/// GATT maximum attribute length
#define ESP_GATT_MAX_ATTR_LEN   600 //as same as GATT_MAX_ATTR_LEN

/// Gatt attribute value 
typedef struct {
    uint8_t           value[ESP_GATT_MAX_ATTR_LEN];			/*!< Gatt attribute value */
    uint16_t          handle;								/*!< Gatt attribute handle */
    uint16_t          offset;								/*!< Gatt attribute value offset */
    uint16_t          len;									/*!< Gatt attribute value length */
    uint8_t           auth_req;								/*!< Gatt authentication request */
} esp_gatt_value_t;

/// GATT remote read request response type
typedef union {
    esp_gatt_value_t attr_value;							/*!< Gatt attribute structure */
    uint16_t            handle;								/*!< Gatt attribute handle */
} esp_gatt_rsp_t;

/**
  * @brief Gatt write type
  */
typedef enum {
	ESP_GATT_WRITE_TYPE_NO_RSP	=	1,						/*!< Gatt write attribute need no response */
	ESP_GATT_WRITE_TYPE_RSP,								/*!< Gatt write attribute need remote response */
} esp_gatt_write_type_t;

#define ESP_GATT_IF_NONE    0xff                            /*!< If callback report gattc_if/gatts_if as this macro, means this event is not correspond to any app */

typedef uint8_t    esp_gatt_if_t;							/*!< Gatt interface type, different application on GATT client use different gatt_if */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GATT_DEFS_H__ */
