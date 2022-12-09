/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */



#ifndef __HID_DEVICE_LE_PRF__
#define __HID_DEVICE_LE_PRF__
#include <stdbool.h>
#include "esp_gatts_api.h"
#include "esp_gatt_defs.h"
#include "esp_hidd_prf_api.h"
#include "esp_gap_ble_api.h"
#include "hid_dev.h"

#define SUPPORT_REPORT_VENDOR                 false
//HID BLE profile log tag
#define HID_LE_PRF_TAG                        "HID_LE_PRF"

/// Maximal number of HIDS that can be added in the DB
#ifndef USE_ONE_HIDS_INSTANCE
#define HIDD_LE_NB_HIDS_INST_MAX              (2)
#else
#define HIDD_LE_NB_HIDS_INST_MAX              (1)
#endif

#define HIDD_GREAT_VER   0x01  //Version + Subversion
#define HIDD_SUB_VER     0x00  //Version + Subversion
#define HIDD_VERSION     ((HIDD_GREAT_VER<<8)|HIDD_SUB_VER)  //Version + Subversion

#define HID_MAX_APPS                 1

// Number of HID reports defined in the service
#define HID_NUM_REPORTS          9

// HID Report IDs for the service
#define HID_RPT_ID_MOUSE_IN      1   // Mouse input report ID
#define HID_RPT_ID_KEY_IN        2   // Keyboard input report ID
#define HID_RPT_ID_CC_IN         3   //Consumer Control input report ID
#define HID_RPT_ID_VENDOR_OUT    4   // Vendor output report ID
#define HID_RPT_ID_LED_OUT       2  // LED output report ID
#define HID_RPT_ID_FEATURE       0  // Feature report ID

#define HIDD_APP_ID			0x1812//ATT_SVC_HID

#define BATTRAY_APP_ID       0x180f


#define ATT_SVC_HID          0x1812

/// Maximal number of Report Char. that can be added in the DB for one HIDS - Up to 11
#define HIDD_LE_NB_REPORT_INST_MAX            (5)

/// Maximal length of Report Char. Value
#define HIDD_LE_REPORT_MAX_LEN                (255)
/// Maximal length of Report Map Char. Value
#define HIDD_LE_REPORT_MAP_MAX_LEN            (512)

/// Length of Boot Report Char. Value Maximal Length
#define HIDD_LE_BOOT_REPORT_MAX_LEN           (8)

/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_KB_IN_NTF_CFG_MASK       (0x40)
/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_MOUSE_IN_NTF_CFG_MASK    (0x80)
/// Boot Report Notification Configuration Bit Mask
#define HIDD_LE_REPORT_NTF_CFG_MASK           (0x20)


/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           0x01      // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  0x02      // NormallyConnectable

/* Control point commands */
#define HID_CMD_SUSPEND                 0x00      // Suspend
#define HID_CMD_EXIT_SUSPEND            0x01      // Exit Suspend

/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT          0x00      // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT        0x01      // Report Protocol Mode

/* Attribute value lengths */
#define HID_PROTOCOL_MODE_LEN           1         // HID Protocol Mode
#define HID_INFORMATION_LEN             4         // HID Information
#define HID_REPORT_REF_LEN              2         // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN          2         // External Report Reference Descriptor

// HID feature flags
#define HID_KBD_FLAGS             HID_FLAGS_REMOTE_WAKE

/* HID Report type */
#define HID_REPORT_TYPE_INPUT       1
#define HID_REPORT_TYPE_OUTPUT      2
#define HID_REPORT_TYPE_FEATURE     3


/// HID Service Attributes Indexes
enum {
    HIDD_LE_IDX_SVC,

    // Included Service
    HIDD_LE_IDX_INCL_SVC,

    // HID Information
    HIDD_LE_IDX_HID_INFO_CHAR,
    HIDD_LE_IDX_HID_INFO_VAL,

    // HID Control Point
    HIDD_LE_IDX_HID_CTNL_PT_CHAR,
    HIDD_LE_IDX_HID_CTNL_PT_VAL,

    // Report Map
    HIDD_LE_IDX_REPORT_MAP_CHAR,
    HIDD_LE_IDX_REPORT_MAP_VAL,
    HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF,

    // Protocol Mode
    HIDD_LE_IDX_PROTO_MODE_CHAR,
    HIDD_LE_IDX_PROTO_MODE_VAL,

    // Report mouse input
    HIDD_LE_IDX_REPORT_MOUSE_IN_CHAR,
    HIDD_LE_IDX_REPORT_MOUSE_IN_VAL,
    HIDD_LE_IDX_REPORT_MOUSE_IN_CCC,
    HIDD_LE_IDX_REPORT_MOUSE_REP_REF,
    //Report Key input
    HIDD_LE_IDX_REPORT_KEY_IN_CHAR,
    HIDD_LE_IDX_REPORT_KEY_IN_VAL,
    HIDD_LE_IDX_REPORT_KEY_IN_CCC,
    HIDD_LE_IDX_REPORT_KEY_IN_REP_REF,
    ///Report Led output
    HIDD_LE_IDX_REPORT_LED_OUT_CHAR,
    HIDD_LE_IDX_REPORT_LED_OUT_VAL,
    HIDD_LE_IDX_REPORT_LED_OUT_REP_REF,

#if (SUPPORT_REPORT_VENDOR  == true)
    /// Report Vendor
    HIDD_LE_IDX_REPORT_VENDOR_OUT_CHAR,
    HIDD_LE_IDX_REPORT_VENDOR_OUT_VAL,
    HIDD_LE_IDX_REPORT_VENDOR_OUT_REP_REF,
#endif
    HIDD_LE_IDX_REPORT_CC_IN_CHAR,
    HIDD_LE_IDX_REPORT_CC_IN_VAL,
    HIDD_LE_IDX_REPORT_CC_IN_CCC,
    HIDD_LE_IDX_REPORT_CC_IN_REP_REF,

    // Boot Keyboard Input Report
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_NTF_CFG,

    // Boot Keyboard Output Report
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_VAL,

    // Boot Mouse Input Report
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_NTF_CFG,

    // Report
    HIDD_LE_IDX_REPORT_CHAR,
    HIDD_LE_IDX_REPORT_VAL,
    HIDD_LE_IDX_REPORT_REP_REF,
    //HIDD_LE_IDX_REPORT_NTF_CFG,

    HIDD_LE_IDX_NB,
};


/// Attribute Table Indexes
enum {
    HIDD_LE_INFO_CHAR,
    HIDD_LE_CTNL_PT_CHAR,
    HIDD_LE_REPORT_MAP_CHAR,
    HIDD_LE_REPORT_CHAR,
    HIDD_LE_PROTO_MODE_CHAR,
    HIDD_LE_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_CHAR_MAX //= HIDD_LE_REPORT_CHAR + HIDD_LE_NB_REPORT_INST_MAX,
};

///att read event table Indexs
enum {
    HIDD_LE_READ_INFO_EVT,
    HIDD_LE_READ_CTNL_PT_EVT,
    HIDD_LE_READ_REPORT_MAP_EVT,
    HIDD_LE_READ_REPORT_EVT,
    HIDD_LE_READ_PROTO_MODE_EVT,
    HIDD_LE_BOOT_KB_IN_REPORT_EVT,
    HIDD_LE_BOOT_KB_OUT_REPORT_EVT,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_EVT,

    HID_LE_EVT_MAX
};

/// Client Characteristic Configuration Codes
enum {
    HIDD_LE_DESC_MASK = 0x10,

    HIDD_LE_BOOT_KB_IN_REPORT_CFG     = HIDD_LE_BOOT_KB_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CFG  = HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_REPORT_CFG                = HIDD_LE_REPORT_CHAR | HIDD_LE_DESC_MASK,
};

/// Features Flag Values
enum {
    HIDD_LE_CFG_KEYBOARD      = 0x01,
    HIDD_LE_CFG_MOUSE         = 0x02,
    HIDD_LE_CFG_PROTO_MODE    = 0x04,
    HIDD_LE_CFG_MAP_EXT_REF   = 0x08,
    HIDD_LE_CFG_BOOT_KB_WR    = 0x10,
    HIDD_LE_CFG_BOOT_MOUSE_WR = 0x20,
};

/// Report Char. Configuration Flag Values
enum {
    HIDD_LE_CFG_REPORT_IN     = 0x01,
    HIDD_LE_CFG_REPORT_OUT    = 0x02,
    //HOGPD_CFG_REPORT_FEAT can be used as a mask to check Report type
    HIDD_LE_CFG_REPORT_FEAT   = 0x03,
    HIDD_LE_CFG_REPORT_WR     = 0x10,
};

/// Pointer to the connection clean-up function
#define HIDD_LE_CLEANUP_FNCT        (NULL)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

/// HIDD Features structure
typedef struct {
    /// Service Features
    uint8_t svc_features;
    /// Number of Report Char. instances to add in the database
    uint8_t report_nb;
    /// Report Char. Configuration
    uint8_t report_char_cfg[HIDD_LE_NB_REPORT_INST_MAX];
} hidd_feature_t;


typedef struct {
    bool                        in_use;
    bool                        congest;
    uint16_t                  conn_id;
    bool                        connected;
    esp_bd_addr_t         remote_bda;
    uint32_t                  trans_id;
    uint8_t                    cur_srvc_id;

} hidd_clcb_t;

// HID report mapping table
typedef struct {
    uint16_t    handle;           // Handle of report characteristic
    uint16_t    cccdHandle;       // Handle of CCCD for report characteristic
    uint8_t     id;               // Report ID
    uint8_t     type;             // Report type
    uint8_t     mode;             // Protocol mode (report or boot)
} hidRptMap_t;


typedef struct {
    /// hidd profile id
    uint8_t app_id;
    /// Notified handle
    uint16_t ntf_handle;
    ///Attribute handle Table
    uint16_t att_tbl[HIDD_LE_IDX_NB];
    /// Supported Features
    hidd_feature_t   hidd_feature[HIDD_LE_NB_HIDS_INST_MAX];
    /// Current Protocol Mode
    uint8_t proto_mode[HIDD_LE_NB_HIDS_INST_MAX];
    /// Number of HIDS added in the database
    uint8_t hids_nb;
    uint8_t pending_evt;
    uint16_t pending_hal;
} hidd_inst_t;

/// Report Reference structure
typedef struct
{
    ///Report ID
    uint8_t report_id;
    ///Report Type
    uint8_t report_type;
}hids_report_ref_t;

/// HID Information structure
typedef struct
{
    /// bcdHID
    uint16_t bcdHID;
    /// bCountryCode
    uint8_t bCountryCode;
    /// Flags
    uint8_t flags;
}hids_hid_info_t;


/* service engine control block */
typedef struct {
    hidd_clcb_t                  hidd_clcb[HID_MAX_APPS];          /* connection link*/
    esp_gatt_if_t                gatt_if;
    bool                         enabled;
    bool                         is_take;
    bool                         is_primery;
    hidd_inst_t                  hidd_inst;
    esp_hidd_event_cb_t          hidd_cb;
    uint8_t                      inst_id;
} hidd_le_env_t;

extern hidd_le_env_t hidd_le_env;
extern uint8_t hidProtocolMode;


void hidd_clcb_alloc (uint16_t conn_id, esp_bd_addr_t bda);

bool hidd_clcb_dealloc (uint16_t conn_id);

void hidd_le_create_service(esp_gatt_if_t gatts_if);

void hidd_set_attr_value(uint16_t handle, uint16_t val_len, const uint8_t *value);

void hidd_get_attr_value(uint16_t handle, uint16_t *length, uint8_t **value);

esp_err_t hidd_register_cb(void);


#endif  ///__HID_DEVICE_LE_PRF__
