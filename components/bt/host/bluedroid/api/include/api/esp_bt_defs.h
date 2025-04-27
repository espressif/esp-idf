/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BT_DEFS_H__
#define __ESP_BT_DEFS_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLUEDROID_STATUS_CHECK(status)           \
    if (esp_bluedroid_get_status() != (status)) {    \
        return ESP_ERR_INVALID_STATE;                \
    }

#define ESP_BT_STATUS_BASE_FOR_HCI_ERR           0X0100  /* base for converting HCI error code to ESP status */

/* See [Vol 1] Part F, Controller Error Codes for a list of error codes and descriptions */

#define ESP_HCI_ERR_SUCCESS                      0x00 /* Success */
#define ESP_HCI_ERR_UNKNOWN_CMD                  0x01 /* Unknown HCI Command */
#define ESP_HCI_ERR_UNKNOWN_CONN_ID              0x02 /* Unknown Connection Identifier */
#define ESP_HCI_ERR_HW_FAILURE                   0x03 /* Hardware Failure */
#define ESP_HCI_ERR_PAGE_TIMEOUT                 0x04 /* Page Timeout */
#define ESP_HCI_ERR_AUTH_FAIL                    0x05 /* Authentication Failure */
#define ESP_HCI_ERR_PIN_OR_KEY_MISSING           0x06 /* PIN or Key Missing */
#define ESP_HCI_ERR_MEM_CAPACITY_EXCEEDED        0x07 /* Memory Capacity Exceeded */
#define ESP_HCI_ERR_CONN_TIMEOUT                 0x08 /* Connection Timeout */
#define ESP_HCI_ERR_CONN_LIMIT_EXCEEDED          0x09 /* Connection Limit Exceeded */
#define ESP_HCI_ERR_SYNC_CONN_LIMIT_EXCEEDED     0x0A /* ASynchronous Connection Limit To A Device Exceeded */
#define ESP_HCI_ERR_CONN_ALREADY_EXISTS          0x0B /* Connection Already Exists */
#define ESP_HCI_ERR_CMD_DISALLOWED               0x0C /* Command Disallowed */
#define ESP_HCI_ERR_INSUFFICIENT_RESOURCES       0x0D /* Connection Rejected due to Limited Resources */
#define ESP_HCI_ERR_INSUFFICIENT_SECURITY        0x0E /* Connection Rejected Due To Security Reasons */
#define ESP_HCI_ERR_BD_ADDR_UNACCEPTABLE         0x0F /* Connection Rejected due to Unacceptable BD_ADDR */
#define ESP_HCI_ERR_CONN_ACCEPT_TIMEOUT          0x10 /* Connection Accept Timeout Exceeded */
#define ESP_HCI_ERR_UNSUPP_FEATURE_PARAM_VAL     0x11 /* Unsupported Feature or Parameter Value */
#define ESP_HCI_ERR_INVALID_PARAM                0x12 /* Invalid HCI Command Parameters */
#define ESP_HCI_ERR_REMOTE_USER_TERM_CONN        0x13 /* Remote User Terminated Connection */
#define ESP_HCI_ERR_REMOTE_LOW_RESOURCES         0x14 /* Remote Device Terminated Connection due to Low Resources */
#define ESP_HCI_ERR_REMOTE_POWER_OFF             0x15 /* Remote Device Terminated Connection due to Power Off */
#define ESP_HCI_ERR_LOCALHOST_TERM_CONN          0x16 /* Connection Terminated By Local Host */
#define ESP_HCI_ERR_REPEATED_ATTEMPTS            0x17 /* Repeated Attempts */
#define ESP_HCI_ERR_PAIRING_NOT_ALLOWED          0x18 /* Pairing Not Allowed */
#define ESP_HCI_ERR_UNKNOWN_LMP_PDU              0x19 /* Unknown LMP PDU */
#define ESP_HCI_ERR_UNSUPP_REMOTE_FEATURE        0x1A /* Unsupported Remote Feature */
#define ESP_HCI_ERR_SCO_OFFSET_REJECTED          0x1B /* SCO Offset Rejected */
#define ESP_HCI_ERR_SCO_INTERVAL_REJECTED        0x1C /* SCO Interval Rejected */
#define ESP_HCI_ERR_SCO_AIR_MODE_REJECTED        0x1D /* SCO Air Mode Rejected */
#define ESP_HCI_ERR_INVALID_LL_PARAM             0x1E /* Invalid LMP Parameters / Invalid LL Parameters */
#define ESP_HCI_ERR_UNSPECIFIED                  0x1F /* Unspecified Error */
#define ESP_HCI_ERR_UNSUPP_LL_PARAM_VAL          0x20 /* Unsupported LMP Parameter Value / Unsupported LL Parameter Value */
#define ESP_HCI_ERR_ROLE_CHANGE_NOT_ALLOWED      0x21 /* Role Change Not Allowed */
#define ESP_HCI_ERR_LL_RESP_TIMEOUT              0x22 /* LMP Response Timeout / LL Response Timeout */
#define ESP_HCI_ERR_LL_PROC_COLLISION            0x23 /* LMP Error Transaction Collision / LL Procedure Collision */
#define ESP_HCI_ERR_LMP_PDU_NOT_ALLOWED          0x24 /* LMP PDU Not Allowed */
#define ESP_HCI_ERR_ENC_MODE_NOT_ACCEPTABLE      0x25 /* Encryption Mode Not Acceptable */
#define ESP_HCI_ERR_LINK_KEY_CANNOT_BE_CHANGED   0x26 /* Link Key cannot be Changed */
#define ESP_HCI_ERR_REQUESTED_QOS_NOT_SUPPORTED  0x27 /* Requested QoS Not Supported */
#define ESP_HCI_ERR_INSTANT_PASSED               0x28 /* Instant Passed */
#define ESP_HCI_ERR_PAIRING_NOT_SUPPORTED        0x29 /* Pairing With Unit Key Not Supported */
#define ESP_HCI_ERR_DIFF_TRANS_COLLISION         0x2A /* Different Transaction Collision */
#define ESP_HCI_ERR_UNDEFINED_0x2B               0x2B /* Reserved for future use */
#define ESP_HCI_ERR_QOS_UNACCEPTABLE_PARAM       0x2C /* QoS Unacceptable Parameter */
#define ESP_HCI_ERR_QOS_REJECTED                 0x2D /* QoS Rejected */
#define ESP_HCI_ERR_CHAN_ASSESS_NOT_SUPPORTED    0x2E /* Channel Classification Not Supported */
#define ESP_HCI_ERR_INSUFF_SECURITY              0x2F /* Insufficient Security */
#define ESP_HCI_ERR_PARAM_OUT_OF_MANDATORY_RANGE 0x30 /* Parameter Out Of Mandatory Range */
#define ESP_HCI_ERR_UNDEFINED_0x31               0x31 /* Reserved for future use */
#define ESP_HCI_ERR_ROLE_SWITCH_PENDING          0x32 /* Role Switch Pending */
#define ESP_HCI_ERR_UNDEFINED_0x33               0x33 /* Reserved for future use */
#define ESP_HCI_ERR_RESERVED_SLOT_VIOLATION      0x34 /* Reserved Slot Violation */
#define ESP_HCI_ERR_ROLE_SWITCH_FAILED           0x35 /* Role Switch Failed */
#define ESP_HCI_ERR_EXT_INQ_RESP_TOO_LARGE       0x36 /* Extended Inquiry Response Too Large */
#define ESP_HCI_ERR_SIMPLE_PAIR_NOT_SUPP_BY_HOST 0x37 /* Secure Simple Pairing Not Supported By Host */
#define ESP_HCI_ERR_HOST_BUSY_PAIRING            0x38 /* Host Busy - Pairing*/
#define ESP_HCI_ERR_CONN_REJECTED_DUE_TO_NO_CHAN 0x39 /* Connection Rejected due to No Suitable Channel Found */
#define ESP_HCI_ERR_CONTROLLER_BUSY              0x3A /* Controller Busy */
#define ESP_HCI_ERR_UNACCEPT_CONN_PARAM          0x3B /* Unacceptable Connection Parameters */
#define ESP_HCI_ERR_ADV_TIMEOUT                  0x3C /* Advertising Timeout */
#define ESP_HCI_ERR_TERM_DUE_TO_MIC_FAIL         0x3D /* Connection Terminated due to MIC Failure */
#define ESP_HCI_ERR_CONN_FAIL_TO_ESTAB           0x3E /* Connection Failed to be Established / Synchronization Timeout */
#define ESP_HCI_ERR_MAC_CONN_FAILED              0x3F /* Previously used */
#define ESP_HCI_ERR_CLOCK_ADJUST_REJECTED        0x40 /* Coarse Clock Adjustment Rejected but Will Try to Adjust Using Clock Dragging */
#define ESP_HCI_ERR_SUBMAP_NOT_DEFINED           0x41 /* Type0 Submap Not Defined */
#define ESP_HCI_ERR_UNKNOWN_ADV_IDENTIFIER       0x42 /* Unknown Advertising Identifier */
#define ESP_HCI_ERR_LIMIT_REACHED                0x43 /* Limit Reached */
#define ESP_HCI_ERR_OP_CANCELLED_BY_HOST         0x44 /* Operation Cancelled by Host */
#define ESP_HCI_ERR_PACKET_TOO_LONG              0x45 /* Packet Too Long */
#define ESP_HCI_ERR_TOO_LATE                     0x46 /* Too Late */
#define ESP_HCI_ERR_TOO_EARLY                    0x47 /* Too Early */
#define ESP_HCI_ERR_INSUFFICIENT_CHANNELS        0x48 /* Insufficient Channels */

/* relate to BT_STATUS_xxx in bt_def.h */
/// Status Return Value
typedef enum {
    ESP_BT_STATUS_SUCCESS       = 0,            /* relate to BT_STATUS_SUCCESS in bt_def.h */
    ESP_BT_STATUS_FAIL,                         /* relate to BT_STATUS_FAIL in bt_def.h */
    ESP_BT_STATUS_NOT_READY,                    /* relate to BT_STATUS_NOT_READY in bt_def.h */
    ESP_BT_STATUS_NOMEM,                        /* relate to BT_STATUS_NOMEM in bt_def.h */
    ESP_BT_STATUS_BUSY,                         /* relate to BT_STATUS_BUSY in bt_def.h */
    ESP_BT_STATUS_DONE          = 5,            /* relate to BT_STATUS_DONE in bt_def.h */
    ESP_BT_STATUS_UNSUPPORTED,                  /* relate to BT_STATUS_UNSUPPORTED in bt_def.h */
    ESP_BT_STATUS_PARM_INVALID,                 /* relate to BT_STATUS_PARM_INVALID in bt_def.h */
    ESP_BT_STATUS_UNHANDLED,                    /* relate to BT_STATUS_UNHANDLED in bt_def.h */
    ESP_BT_STATUS_AUTH_FAILURE,                 /* relate to BT_STATUS_AUTH_FAILURE in bt_def.h */
    ESP_BT_STATUS_RMT_DEV_DOWN  = 10,           /* relate to BT_STATUS_RMT_DEV_DOWN in bt_def.h */
    ESP_BT_STATUS_AUTH_REJECTED,                /* relate to BT_STATUS_AUTH_REJECTED in bt_def.h */
    ESP_BT_STATUS_INVALID_STATIC_RAND_ADDR,     /* relate to BT_STATUS_INVALID_STATIC_RAND_ADDR in bt_def.h */
    ESP_BT_STATUS_PENDING,                      /* relate to BT_STATUS_PENDING in bt_def.h */
    ESP_BT_STATUS_UNACCEPT_CONN_INTERVAL,       /* relate to BT_UNACCEPT_CONN_INTERVAL in bt_def.h */
    ESP_BT_STATUS_PARAM_OUT_OF_RANGE,           /* relate to BT_PARAM_OUT_OF_RANGE in bt_def.h */
    ESP_BT_STATUS_TIMEOUT,                      /* relate to BT_STATUS_TIMEOUT in bt_def.h */
    ESP_BT_STATUS_PEER_LE_DATA_LEN_UNSUPPORTED, /* relate to BTM_PEER_LE_DATA_LEN_UNSUPPORTED in stack/btm_api.h */
    ESP_BT_STATUS_CONTROL_LE_DATA_LEN_UNSUPPORTED,/* relate to BTM_CONTROL_LE_DATA_LEN_UNSUPPORTED in stack/btm_api.h */
    ESP_BT_STATUS_ERR_ILLEGAL_PARAMETER_FMT,    /* relate to HCI_ERR_ILLEGAL_PARAMETER_FMT in stack/hcidefs.h */
    ESP_BT_STATUS_MEMORY_FULL   = 20,           /* relate to BT_STATUS_MEMORY_FULL in bt_def.h */
    ESP_BT_STATUS_EIR_TOO_LARGE,                /* relate to BT_STATUS_EIR_TOO_LARGE in bt_def.h */
    ESP_BT_STATUS_HCI_SUCCESS   = ESP_BT_STATUS_BASE_FOR_HCI_ERR,
    ESP_BT_STATUS_HCI_ILLEGAL_COMMAND,
    ESP_BT_STATUS_HCI_NO_CONNECTION,
    ESP_BT_STATUS_HCI_HW_FAILURE,
    ESP_BT_STATUS_HCI_PAGE_TIMEOUT,
    ESP_BT_STATUS_HCI_AUTH_FAILURE,
    ESP_BT_STATUS_HCI_KEY_MISSING,
    ESP_BT_STATUS_HCI_MEMORY_FULL,
    ESP_BT_STATUS_HCI_CONNECTION_TOUT,
    ESP_BT_STATUS_HCI_MAX_NUM_OF_CONNECTIONS,
    ESP_BT_STATUS_HCI_MAX_NUM_OF_SCOS,
    ESP_BT_STATUS_HCI_CONNECTION_EXISTS,
    ESP_BT_STATUS_HCI_COMMAND_DISALLOWED,
    ESP_BT_STATUS_HCI_HOST_REJECT_RESOURCES,
    ESP_BT_STATUS_HCI_HOST_REJECT_SECURITY,
    ESP_BT_STATUS_HCI_HOST_REJECT_DEVICE,
    ESP_BT_STATUS_HCI_HOST_TIMEOUT,
    ESP_BT_STATUS_HCI_UNSUPPORTED_VALUE,
    ESP_BT_STATUS_HCI_ILLEGAL_PARAMETER_FMT,
    ESP_BT_STATUS_HCI_PEER_USER,
    ESP_BT_STATUS_HCI_PEER_LOW_RESOURCES,
    ESP_BT_STATUS_HCI_PEER_POWER_OFF,
    ESP_BT_STATUS_HCI_CONN_CAUSE_LOCAL_HOST,
    ESP_BT_STATUS_HCI_REPEATED_ATTEMPTS,
    ESP_BT_STATUS_HCI_PAIRING_NOT_ALLOWED,
    ESP_BT_STATUS_HCI_UNKNOWN_LMP_PDU,
    ESP_BT_STATUS_HCI_UNSUPPORTED_REM_FEATURE,
    ESP_BT_STATUS_HCI_SCO_OFFSET_REJECTED,
    ESP_BT_STATUS_HCI_SCO_INTERVAL_REJECTED,
    ESP_BT_STATUS_HCI_SCO_AIR_MODE,
    ESP_BT_STATUS_HCI_INVALID_LMP_PARAM,
    ESP_BT_STATUS_HCI_UNSPECIFIED,
    ESP_BT_STATUS_HCI_UNSUPPORTED_LMP_PARAMETERS,
    ESP_BT_STATUS_HCI_ROLE_CHANGE_NOT_ALLOWED,
    ESP_BT_STATUS_HCI_LMP_RESPONSE_TIMEOUT,
    ESP_BT_STATUS_HCI_LMP_ERR_TRANS_COLLISION,
    ESP_BT_STATUS_HCI_LMP_PDU_NOT_ALLOWED,
    ESP_BT_STATUS_HCI_ENCRY_MODE_NOT_ACCEPTABLE,
    ESP_BT_STATUS_HCI_UNIT_KEY_USED,
    ESP_BT_STATUS_HCI_QOS_NOT_SUPPORTED,
    ESP_BT_STATUS_HCI_INSTANT_PASSED,
    ESP_BT_STATUS_HCI_PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED,
    ESP_BT_STATUS_HCI_DIFF_TRANSACTION_COLLISION,
    ESP_BT_STATUS_HCI_UNDEFINED_0x2B,
    ESP_BT_STATUS_HCI_QOS_UNACCEPTABLE_PARAM,
    ESP_BT_STATUS_HCI_QOS_REJECTED,
    ESP_BT_STATUS_HCI_CHAN_CLASSIF_NOT_SUPPORTED,
    ESP_BT_STATUS_HCI_INSUFFCIENT_SECURITY,
    ESP_BT_STATUS_HCI_PARAM_OUT_OF_RANGE,
    ESP_BT_STATUS_HCI_UNDEFINED_0x31,
    ESP_BT_STATUS_HCI_ROLE_SWITCH_PENDING,
    ESP_BT_STATUS_HCI_UNDEFINED_0x33,
    ESP_BT_STATUS_HCI_RESERVED_SLOT_VIOLATION,
    ESP_BT_STATUS_HCI_ROLE_SWITCH_FAILED,
    ESP_BT_STATUS_HCI_INQ_RSP_DATA_TOO_LARGE,
    ESP_BT_STATUS_HCI_SIMPLE_PAIRING_NOT_SUPPORTED,
    ESP_BT_STATUS_HCI_HOST_BUSY_PAIRING,
    ESP_BT_STATUS_HCI_REJ_NO_SUITABLE_CHANNEL,
    ESP_BT_STATUS_HCI_CONTROLLER_BUSY,
    ESP_BT_STATUS_HCI_UNACCEPT_CONN_INTERVAL,
    ESP_BT_STATUS_HCI_DIRECTED_ADVERTISING_TIMEOUT,
    ESP_BT_STATUS_HCI_CONN_TOUT_DUE_TO_MIC_FAILURE,
    ESP_BT_STATUS_HCI_CONN_FAILED_ESTABLISHMENT,
    ESP_BT_STATUS_HCI_MAC_CONNECTION_FAILED,
    ESP_BT_STATUS_HCI_CCA_REJECTED,
    ESP_BT_STATUS_HCI_TYPE0_SUBMAP_NOT_DEFINED,
    ESP_BT_STATUS_HCI_UNKNOWN_ADV_ID,
    ESP_BT_STATUS_HCI_LIMIT_REACHED,
    ESP_BT_STATUS_HCI_OPT_CANCEL_BY_HOST,
    ESP_BT_STATUS_HCI_PKT_TOO_LONG,
    ESP_BT_STATUS_HCI_TOO_LATE,
    ESP_BT_STATUS_HCI_TOO_EARLY,
} esp_bt_status_t;


/*Define the bt octet 16 bit size*/
#define ESP_BT_OCTET16_LEN    16
typedef uint8_t esp_bt_octet16_t[ESP_BT_OCTET16_LEN];   /* octet array: size 16 */

#define ESP_BT_OCTET8_LEN    8
typedef uint8_t esp_bt_octet8_t[ESP_BT_OCTET8_LEN];   /* octet array: size 8 */

typedef uint8_t esp_link_key[ESP_BT_OCTET16_LEN];      /* Link Key */

/// Default GATT interface id
#define ESP_DEFAULT_GATT_IF             0xff

#if BLE_HIGH_DUTY_ADV_INTERVAL
#define ESP_BLE_PRIM_ADV_INT_MIN            0x000008     /*!< Minimum advertising interval for undirected and low duty cycle directed advertising */
#else
#define ESP_BLE_PRIM_ADV_INT_MIN            0x000020     /*!< Minimum advertising interval for undirected and low duty cycle directed advertising */
#endif
#define ESP_BLE_PRIM_ADV_INT_MAX            0xFFFFFF     /*!< Maximum advertising interval for undirected and low duty cycle directed advertising */
#define ESP_BLE_CONN_INT_MIN                0x0006       /*!< relate to BTM_BLE_CONN_INT_MIN in stack/btm_ble_api.h */
#define ESP_BLE_CONN_INT_MAX                0x0C80       /*!< relate to BTM_BLE_CONN_INT_MAX in stack/btm_ble_api.h */
#define ESP_BLE_CONN_LATENCY_MAX            499          /*!< relate to ESP_BLE_CONN_LATENCY_MAX in stack/btm_ble_api.h */
#define ESP_BLE_CONN_SUP_TOUT_MIN           0x000A       /*!< relate to BTM_BLE_CONN_SUP_TOUT_MIN in stack/btm_ble_api.h */
#define ESP_BLE_CONN_SUP_TOUT_MAX           0x0C80       /*!< relate to ESP_BLE_CONN_SUP_TOUT_MAX in stack/btm_ble_api.h */

#define ESP_BLE_PHY_1M_PREF_MASK       (1 << 0) /*!< The Host prefers use the LE1M transmitter or receiver PHY */
#define ESP_BLE_PHY_2M_PREF_MASK       (1 << 1) /*!< The Host prefers use the LE2M transmitter or receiver PHY */
#define ESP_BLE_PHY_CODED_PREF_MASK    (1 << 2) /*!< The Host prefers use the LE CODED transmitter or receiver PHY */
typedef uint8_t esp_ble_phy_mask_t;

/**
* @brief create connection parameters
*/
typedef struct {
    uint16_t scan_interval;       /*!< Initial scan interval, in units of 0.625ms, the range is 0x0004(2.5ms) to 0xFFFF(10.24s). */
    uint16_t scan_window;         /*!< Initial scan window, in units of 0.625ms, the range is 0x0004(2.5ms) to 0xFFFF(10.24s). */
    uint16_t interval_min;        /*!< Minimum connection interval, in units of 1.25ms, the range is 0x0006(7.5ms) to 0x0C80(4s). */
    uint16_t interval_max;        /*!< Maximum connection interval, in units of 1.25ms, the range is 0x0006(7.5ms) to 0x0C80(4s). */
    uint16_t latency;             /*!< Connection latency, the range is 0x0000(0) to 0x01F3(499). */
    uint16_t supervision_timeout; /*!< Connection supervision timeout, in units of 10ms, the range is from 0x000A(100ms) to 0x0C80(32s). */
    uint16_t min_ce_len;          /*!< Minimum connection event length, in units of 0.625ms, setting to 0 for no preferred parameters. */
    uint16_t max_ce_len;          /*!< Maximum connection event length, in units of 0.625ms, setting to 0 for no preferred parameters. */
} esp_ble_conn_params_t;

/// Check the param is valid or not
#define ESP_BLE_IS_VALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)) )

/// UUID type
typedef struct {
#define ESP_UUID_LEN_16     2
#define ESP_UUID_LEN_32     4
#define ESP_UUID_LEN_128    16
    uint16_t len;							/*!< UUID length, 16bit, 32bit or 128bit */
    union {
        uint16_t    uuid16;                 /*!< 16bit UUID */
        uint32_t    uuid32;                 /*!< 32bit UUID */
        uint8_t     uuid128[ESP_UUID_LEN_128]; /*!< 128bit UUID */
    } uuid;									/*!< UUID */
} __attribute__((packed)) esp_bt_uuid_t;

/// Bluetooth device type
typedef enum {
    ESP_BT_DEVICE_TYPE_BREDR   = 0x01,
    ESP_BT_DEVICE_TYPE_BLE     = 0x02,
    ESP_BT_DEVICE_TYPE_DUMO    = 0x03,
} esp_bt_dev_type_t;

/// Bluetooth address length
#define ESP_BD_ADDR_LEN     6

/// Bluetooth peer irk
#define ESP_PEER_IRK_LEN    16

/// Bluetooth device address
typedef uint8_t esp_bd_addr_t[ESP_BD_ADDR_LEN];

/// BLE device address type
typedef enum {
    BLE_ADDR_TYPE_PUBLIC        = 0x00,     /*!< Public Device Address */
    BLE_ADDR_TYPE_RANDOM        = 0x01,     /*!< Random Device Address. To set this address, use the function esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr) */
    BLE_ADDR_TYPE_RPA_PUBLIC    = 0x02,     /*!< Resolvable Private Address (RPA) with public identity address */
    BLE_ADDR_TYPE_RPA_RANDOM    = 0x03,     /*!< Resolvable Private Address (RPA) with random identity address. To set this address, use the function esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr) */
} esp_ble_addr_type_t;

/// white list address type
typedef enum {
    BLE_WL_ADDR_TYPE_PUBLIC        = 0x00,
    BLE_WL_ADDR_TYPE_RANDOM        = 0x01,
} esp_ble_wl_addr_type_t;

/// Used to exchange the encryption key in the init key & response key
#define ESP_BLE_ENC_KEY_MASK    (1 << 0)            /* relate to BTM_BLE_ENC_KEY_MASK in stack/btm_api.h */
/// Used to exchange the IRK key in the init key & response key
#define ESP_BLE_ID_KEY_MASK     (1 << 1)            /* relate to BTM_BLE_ID_KEY_MASK in stack/btm_api.h */
/// Used to exchange the CSRK key in the init key & response key
#define ESP_BLE_CSR_KEY_MASK    (1 << 2)            /* relate to BTM_BLE_CSR_KEY_MASK in stack/btm_api.h */
/// Used to exchange the link key(this key just used in the BLE & BR/EDR coexist mode) in the init key & response key
#define ESP_BLE_LINK_KEY_MASK   (1 << 3)            /* relate to BTM_BLE_LINK_KEY_MASK in stack/btm_api.h */
typedef uint8_t esp_ble_key_mask_t;            /* the key mask type */

/// Minimum of the application id
#define ESP_APP_ID_MIN  0x0000
/// Maximum of the application id
#define ESP_APP_ID_MAX  0x7fff

#define ESP_BD_ADDR_STR         "%02x:%02x:%02x:%02x:%02x:%02x"
#define ESP_BD_ADDR_HEX(addr)   addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]

#define ESP_BLE_ADV_NAME_LEN_MAX 29

#define ESP_INVALID_CONN_HANDLE  0xfff

#ifdef __cplusplus
}
#endif

#endif     /* __ESP_BT_DEFS_H__ */
