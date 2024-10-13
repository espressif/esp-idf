/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_GAP_BT_API_H__
#define __ESP_GAP_BT_API_H__

#include <stdint.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// RSSI threshold
#define ESP_BT_GAP_RSSI_HIGH_THRLD  -20             /*!< High RSSI threshold */
#define ESP_BT_GAP_RSSI_LOW_THRLD   -45             /*!< Low RSSI threshold */

/// Class of device
typedef struct {
    uint32_t      reserved_2: 2;                    /*!< undefined */
    uint32_t      minor: 6;                         /*!< minor class */
    uint32_t      major: 5;                         /*!< major class */
    uint32_t      service: 11;                      /*!< service class */
    uint32_t      reserved_8: 8;                    /*!< undefined */
} esp_bt_cod_t;

/// class of device settings
typedef enum {
    ESP_BT_SET_COD_MAJOR_MINOR     = 0x01,          /*!< overwrite major, minor class */
    ESP_BT_SET_COD_SERVICE_CLASS   = 0x02,          /*!< set the bits in the input, the current bit will remain */
    ESP_BT_CLR_COD_SERVICE_CLASS   = 0x04,          /*!< clear the bits in the input, others will remain */
    ESP_BT_SET_COD_ALL             = 0x08,          /*!< overwrite major, minor, set the bits in service class, reserved_2 remain unchanged */
    ESP_BT_INIT_COD                = 0x0a,          /*!< overwrite major, minor, and service class, reserved_2 remain unchanged */
    ESP_BT_SET_COD_RESERVED_2      = 0x10,          /*!< overwrite the two least significant bits reserved_2 whose default value is 0b00; other values of reserved_2 are invalid according to Bluetooth Core Specification 5.4 */
} esp_bt_cod_mode_t;

#define ESP_BT_GAP_AFH_CHANNELS_LEN     10
typedef uint8_t esp_bt_gap_afh_channels[ESP_BT_GAP_AFH_CHANNELS_LEN];


/// Discoverability and Connectability mode
typedef enum {
    ESP_BT_NON_CONNECTABLE,             /*!< Non-connectable */
    ESP_BT_CONNECTABLE,                 /*!< Connectable */
} esp_bt_connection_mode_t;

typedef enum {
    ESP_BT_NON_DISCOVERABLE,            /*!< Non-discoverable */
    ESP_BT_LIMITED_DISCOVERABLE,        /*!< Limited Discoverable */
    ESP_BT_GENERAL_DISCOVERABLE,        /*!< General Discoverable */
} esp_bt_discovery_mode_t;

/// Bluetooth Device Property type
typedef enum {
    ESP_BT_GAP_DEV_PROP_BDNAME = 1,                 /*!< Bluetooth device name, value type is int8_t [] */
    ESP_BT_GAP_DEV_PROP_COD,                        /*!< Class of Device, value type is uint32_t */
    ESP_BT_GAP_DEV_PROP_RSSI,                       /*!< Received Signal strength Indication, value type is int8_t, ranging from -128 to 127 */
    ESP_BT_GAP_DEV_PROP_EIR,                        /*!< Extended Inquiry Response, value type is uint8_t [] */
} esp_bt_gap_dev_prop_type_t;

/// Maximum bytes of Bluetooth device name
#define ESP_BT_GAP_MAX_BDNAME_LEN             (248)

/// Maximum size of EIR Significant part
#define ESP_BT_GAP_EIR_DATA_LEN               (240)

/// Bluetooth Device Property Descriptor
typedef struct {
    esp_bt_gap_dev_prop_type_t type;                /*!< Device property type */
    int len;                                        /*!< Device property value length */
    void *val;                                      /*!< Device property value */
} esp_bt_gap_dev_prop_t;

/// Extended Inquiry Response data type
#define ESP_BT_EIR_TYPE_FLAGS                   0x01      /*!< Flag with information such as BR/EDR and LE support */
#define ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID      0x02      /*!< Incomplete list of 16-bit service UUIDs */
#define ESP_BT_EIR_TYPE_CMPL_16BITS_UUID        0x03      /*!< Complete list of 16-bit service UUIDs */
#define ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID      0x04      /*!< Incomplete list of 32-bit service UUIDs */
#define ESP_BT_EIR_TYPE_CMPL_32BITS_UUID        0x05      /*!< Complete list of 32-bit service UUIDs */
#define ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID     0x06      /*!< Incomplete list of 128-bit service UUIDs */
#define ESP_BT_EIR_TYPE_CMPL_128BITS_UUID       0x07      /*!< Complete list of 128-bit service UUIDs */
#define ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME        0x08      /*!< Shortened Local Name */
#define ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME         0x09      /*!< Complete Local Name */
#define ESP_BT_EIR_TYPE_TX_POWER_LEVEL          0x0a      /*!< Tx power level, value is 1 octet ranging from  -127 to 127, unit is dBm*/
#define ESP_BT_EIR_TYPE_URL                     0x24      /*!< Uniform resource identifier */
#define ESP_BT_EIR_TYPE_MANU_SPECIFIC           0xff      /*!< Manufacturer specific data */
#define  ESP_BT_EIR_TYPE_MAX_NUM                12        /*!< MAX number of EIR type */

typedef uint8_t esp_bt_eir_type_t;

/* ACL Packet Types */
#define ESP_BT_ACL_PKT_TYPES_MASK_DM1           0x0008
#define ESP_BT_ACL_PKT_TYPES_MASK_DH1           0x0010
#define ESP_BT_ACL_PKT_TYPES_MASK_DM3           0x0400
#define ESP_BT_ACL_PKT_TYPES_MASK_DH3           0x0800
#define ESP_BT_ACL_PKT_TYPES_MASK_DM5           0x4000
#define ESP_BT_ACL_PKT_TYPES_MASK_DH5           0x8000
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH1      0x0002
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH1      0x0004
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH3      0x0100
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH3      0x0200
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH5      0x1000
#define ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH5      0x2000

// DM1 can not be disabled. All options are mandatory to include DM1.
#define ESP_BT_ACL_DM1_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DM1 | 0x330e)         /* 0x330e */
#define ESP_BT_ACL_DH1_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DH1 | 0x330e)         /* 0x331e */
#define ESP_BT_ACL_DM3_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DM3 | 0x330e)         /* 0x370e */
#define ESP_BT_ACL_DH3_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DH3 | 0x330e)         /* 0x3b0e */
#define ESP_BT_ACL_DM5_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DM5 | 0x330e)         /* 0x730e */
#define ESP_BT_ACL_DH5_ONLY     (ESP_BT_ACL_PKT_TYPES_MASK_DH5 | 0x330e)         /* 0xb30e */
#define ESP_BT_ACL_2_DH1_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH1 & 0x330e)   /* 0x330c */
#define ESP_BT_ACL_3_DH1_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH1 & 0x330e)   /* 0x330a */
#define ESP_BT_ACL_2_DH3_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH3 & 0x330e)   /* 0x320e */
#define ESP_BT_ACL_3_DH3_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH3 & 0x330e)   /* 0x310e */
#define ESP_BT_ACL_2_DH5_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_2_DH5 & 0x330e)   /* 0x230e */
#define ESP_BT_ACL_3_DH5_ONLY   (~ESP_BT_ACL_PKT_TYPES_MASK_NO_3_DH5 & 0x330e)   /* 0x130e */

typedef uint16_t esp_bt_acl_pkt_type_t;

/* Range of encryption key size */
#define ESP_BT_ENC_KEY_SIZE_CTRL_MAX            (16)
#define ESP_BT_ENC_KEY_SIZE_CTRL_MIN            (7)

/* ESP_BT_EIR_FLAG bit definition */
#define ESP_BT_EIR_FLAG_LIMIT_DISC         (0x01 << 0)
#define ESP_BT_EIR_FLAG_GEN_DISC           (0x01 << 1)
#define ESP_BT_EIR_FLAG_BREDR_NOT_SPT      (0x01 << 2)
#define ESP_BT_EIR_FLAG_DMT_CONTROLLER_SPT (0x01 << 3)
#define ESP_BT_EIR_FLAG_DMT_HOST_SPT       (0x01 << 4)

#define ESP_BT_EIR_MAX_LEN                  240
/// EIR data content, according to "Supplement to the Bluetooth Core Specification"
typedef struct {
    bool                    fec_required;           /*!< FEC is required or not, true by default */
    bool                    include_txpower;        /*!< EIR data include TX power, false by default */
    bool                    include_uuid;           /*!< EIR data include UUID, false by default */
    bool                    include_name;           /*!< EIR data include device name, true by default */
    uint8_t                 flag;                   /*!< EIR flags, see ESP_BT_EIR_FLAG for details, EIR will not include flag if it is 0, 0 by default */
    uint16_t                manufacturer_len;       /*!< Manufacturer data length, 0 by default */
    uint8_t                 *p_manufacturer_data;   /*!< Manufacturer data point */
    uint16_t                url_len;                /*!< URL length, 0 by default */
    uint8_t                 *p_url;                 /*!< URL point */
} esp_bt_eir_data_t;

/// Major service class field of Class of Device, multiple bits can be set
typedef enum {
    ESP_BT_COD_SRVC_NONE                     =     0,    /*!< None indicates an invalid value */
    ESP_BT_COD_SRVC_LMTD_DISCOVER            =   0x1,    /*!< Limited Discoverable Mode */
    ESP_BT_COD_SRVC_POSITIONING              =   0x8,    /*!< Positioning (Location identification) */
    ESP_BT_COD_SRVC_NETWORKING               =  0x10,    /*!< Networking, e.g. LAN, Ad hoc */
    ESP_BT_COD_SRVC_RENDERING                =  0x20,    /*!< Rendering, e.g. Printing, Speakers */
    ESP_BT_COD_SRVC_CAPTURING                =  0x40,    /*!< Capturing, e.g. Scanner, Microphone */
    ESP_BT_COD_SRVC_OBJ_TRANSFER             =  0x80,    /*!< Object Transfer, e.g. v-Inbox, v-Folder */
    ESP_BT_COD_SRVC_AUDIO                    = 0x100,    /*!< Audio, e.g. Speaker, Microphone, Headset service */
    ESP_BT_COD_SRVC_TELEPHONY                = 0x200,    /*!< Telephony, e.g. Cordless telephony, Modem, Headset service */
    ESP_BT_COD_SRVC_INFORMATION              = 0x400,    /*!< Information, e.g., WEB-server, WAP-server */
} esp_bt_cod_srvc_t;

typedef enum{
    ESP_BT_PIN_TYPE_VARIABLE = 0,                       /*!< Refer to BTM_PIN_TYPE_VARIABLE */
    ESP_BT_PIN_TYPE_FIXED    = 1,                       /*!< Refer to BTM_PIN_TYPE_FIXED */
} esp_bt_pin_type_t;

#define ESP_BT_PIN_CODE_LEN        16                   /*!< Max pin code length */
typedef uint8_t esp_bt_pin_code_t[ESP_BT_PIN_CODE_LEN]; /*!< Pin Code (upto 128 bits) MSB is 0 */

typedef enum {
    ESP_BT_SP_IOCAP_MODE = 0,                            /*!< Set IO mode */
    //ESP_BT_SP_OOB_DATA, //TODO                         /*!< Set OOB data */
} esp_bt_sp_param_t;

/* relate to BTM_IO_CAP_xxx in stack/btm_api.h */
#define ESP_BT_IO_CAP_OUT                      0        /*!< DisplayOnly */         /* relate to BTM_IO_CAP_OUT in stack/btm_api.h */
#define ESP_BT_IO_CAP_IO                       1        /*!< DisplayYesNo */        /* relate to BTM_IO_CAP_IO in stack/btm_api.h */
#define ESP_BT_IO_CAP_IN                       2        /*!< KeyboardOnly */        /* relate to BTM_IO_CAP_IN in stack/btm_api.h */
#define ESP_BT_IO_CAP_NONE                     3        /*!< NoInputNoOutput */     /* relate to BTM_IO_CAP_NONE in stack/btm_api.h */
typedef uint8_t esp_bt_io_cap_t;                        /*!< Combination of the IO Capability */


/* BTM Power manager modes */
#define ESP_BT_PM_MD_ACTIVE                 0x00        /*!< Active mode */
#define ESP_BT_PM_MD_HOLD                   0x01        /*!< Hold mode */
#define ESP_BT_PM_MD_SNIFF                  0x02        /*!< Sniff mode */
#define ESP_BT_PM_MD_PARK                   0x03        /*!< Park state */
typedef uint8_t esp_bt_pm_mode_t;



/// Bits of major service class field
#define ESP_BT_COD_SRVC_BIT_MASK              (0xffe000) /*!< Major service bit mask */
#define ESP_BT_COD_SRVC_BIT_OFFSET            (13)       /*!< Major service bit offset */

/// Major device class field of Class of Device
typedef enum {
    ESP_BT_COD_MAJOR_DEV_MISC                = 0,    /*!< Miscellaneous */
    ESP_BT_COD_MAJOR_DEV_COMPUTER            = 1,    /*!< Computer */
    ESP_BT_COD_MAJOR_DEV_PHONE               = 2,    /*!< Phone(cellular, cordless, pay phone, modem */
    ESP_BT_COD_MAJOR_DEV_LAN_NAP             = 3,    /*!< LAN, Network Access Point */
    ESP_BT_COD_MAJOR_DEV_AV                  = 4,    /*!< Audio/Video(headset, speaker, stereo, video display, VCR */
    ESP_BT_COD_MAJOR_DEV_PERIPHERAL          = 5,    /*!< Peripheral(mouse, joystick, keyboard) */
    ESP_BT_COD_MAJOR_DEV_IMAGING             = 6,    /*!< Imaging(printer, scanner, camera, display */
    ESP_BT_COD_MAJOR_DEV_WEARABLE            = 7,    /*!< Wearable */
    ESP_BT_COD_MAJOR_DEV_TOY                 = 8,    /*!< Toy */
    ESP_BT_COD_MAJOR_DEV_HEALTH              = 9,    /*!< Health */
    ESP_BT_COD_MAJOR_DEV_UNCATEGORIZED       = 31,   /*!< Uncategorized: device not specified */
} esp_bt_cod_major_dev_t;

/// Minor device class field of Class of Device for Peripheral Major Class
typedef enum {
    ESP_BT_COD_MINOR_PERIPHERAL_KEYBOARD            = 0x10, /*!< Keyboard */
    ESP_BT_COD_MINOR_PERIPHERAL_POINTING            = 0x20, /*!< Pointing */
    ESP_BT_COD_MINOR_PERIPHERAL_COMBO               = 0x30, /*!< Combo
                                                            ESP_BT_COD_MINOR_PERIPHERAL_KEYBOARD, ESP_BT_COD_MINOR_PERIPHERAL_POINTING
                                                            and ESP_BT_COD_MINOR_PERIPHERAL_COMBO can be OR'd with one of the
                                                            following values to identify a multifunctional device. e.g.
                                                                ESP_BT_COD_MINOR_PERIPHERAL_KEYBOARD | ESP_BT_COD_MINOR_PERIPHERAL_GAMEPAD
                                                                ESP_BT_COD_MINOR_PERIPHERAL_POINTING | ESP_BT_COD_MINOR_PERIPHERAL_SENSING_DEVICE
                                                             */
    ESP_BT_COD_MINOR_PERIPHERAL_JOYSTICK            = 0x01, /*!< Joystick */
    ESP_BT_COD_MINOR_PERIPHERAL_GAMEPAD             = 0x02, /*!< Gamepad */
    ESP_BT_COD_MINOR_PERIPHERAL_REMOTE_CONTROL      = 0x03, /*!< Remote Control */
    ESP_BT_COD_MINOR_PERIPHERAL_SENSING_DEVICE      = 0x04, /*!< Sensing Device */
    ESP_BT_COD_MINOR_PERIPHERAL_DIGITIZING_TABLET   = 0x05, /*!< Digitizing Tablet */
    ESP_BT_COD_MINOR_PERIPHERAL_CARD_READER         = 0x06, /*!< Card Reader */
    ESP_BT_COD_MINOR_PERIPHERAL_DIGITAL_PAN         = 0x07, /*!< Digital Pan */
    ESP_BT_COD_MINOR_PERIPHERAL_HAND_SCANNER        = 0x08, /*!< Hand Scanner */
    ESP_BT_COD_MINOR_PERIPHERAL_HAND_GESTURAL_INPUT = 0x09, /*!< Hand Gestural Input */
} esp_bt_cod_minor_peripheral_t;

/// Bits of major device class field
#define ESP_BT_COD_MAJOR_DEV_BIT_MASK         (0x1f00) /*!< Major device bit mask */
#define ESP_BT_COD_MAJOR_DEV_BIT_OFFSET       (8)      /*!< Major device bit offset */

/// Bits of minor device class field
#define ESP_BT_COD_MINOR_DEV_BIT_MASK         (0xfc)   /*!< Minor device bit mask */
#define ESP_BT_COD_MINOR_DEV_BIT_OFFSET       (2)      /*!< Minor device bit offset */

/// Bits of format type
#define ESP_BT_COD_FORMAT_TYPE_BIT_MASK       (0x03)   /*!< Format type bit mask */
#define ESP_BT_COD_FORMAT_TYPE_BIT_OFFSET     (0)      /*!< Format type bit offset */

/// Class of device format type 1
#define ESP_BT_COD_FORMAT_TYPE_1              (0x00)

/** Bluetooth Device Discovery state */
typedef enum {
    ESP_BT_GAP_DISCOVERY_STOPPED,                   /*!< Device discovery stopped */
    ESP_BT_GAP_DISCOVERY_STARTED,                   /*!< Device discovery started */
} esp_bt_gap_discovery_state_t;

/// Type of link key
#define ESP_BT_LINK_KEY_COMB                (0x00)  /*!< Combination Key */
#define ESP_BT_LINK_KEY_DBG_COMB            (0x03)  /*!< Debug Combination Key */
#define ESP_BT_LINK_KEY_UNAUTHED_COMB_P192  (0x04)  /*!< Unauthenticated Combination Key generated from P-192 */
#define ESP_BT_LINK_KEY_AUTHED_COMB_P192    (0x05)  /*!< Authenticated Combination Key generated from P-192 */
#define ESP_BT_LINK_KEY_CHG_COMB            (0x06)  /*!< Changed Combination Key */
#define ESP_BT_LINK_KEY_UNAUTHED_COMB_P256  (0x07)  /*!< Unauthenticated Combination Key generated from P-256 */
#define ESP_BT_LINK_KEY_AUTHED_COMB_P256    (0x08)  /*!< Authenticated Combination Key generated from P-256 */
typedef uint8_t esp_bt_link_key_type_t;

/// Type of encryption
#define ESP_BT_ENC_MODE_OFF                 (0x00)  /*!< Link Level Encryption is OFF */
#define ESP_BT_ENC_MODE_E0                  (0x01)  /*!< Link Level Encryption is ON with E0 */
#define ESP_BT_ENC_MODE_AES                 (0x02)  /*!< Link Level Encryption is ON with AES-CCM */
typedef uint8_t esp_bt_enc_mode_t;

/// BT GAP callback events
typedef enum {
    ESP_BT_GAP_DISC_RES_EVT = 0,                    /*!< Device discovery result event */
    ESP_BT_GAP_DISC_STATE_CHANGED_EVT,              /*!< Discovery state changed event */
    ESP_BT_GAP_RMT_SRVCS_EVT,                       /*!< Get remote services event */
    ESP_BT_GAP_RMT_SRVC_REC_EVT,                    /*!< Get remote service record event */
    ESP_BT_GAP_AUTH_CMPL_EVT,                       /*!< Authentication complete event */
    ESP_BT_GAP_PIN_REQ_EVT,                         /*!< Legacy Pairing Pin code request */
    ESP_BT_GAP_CFM_REQ_EVT,                         /*!< Security Simple Pairing User Confirmation request. */
    ESP_BT_GAP_KEY_NOTIF_EVT,                       /*!< Security Simple Pairing Passkey Notification */
    ESP_BT_GAP_KEY_REQ_EVT,                         /*!< Security Simple Pairing Passkey request */
    ESP_BT_GAP_READ_RSSI_DELTA_EVT,                 /*!< Read rssi event */
    ESP_BT_GAP_CONFIG_EIR_DATA_EVT,                 /*!< Config EIR data event */
    ESP_BT_GAP_SET_AFH_CHANNELS_EVT,                /*!< Set AFH channels event */
    ESP_BT_GAP_READ_REMOTE_NAME_EVT,                /*!< Read Remote Name event */
    ESP_BT_GAP_MODE_CHG_EVT,
    ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT,         /*!< remove bond device complete event */
    ESP_BT_GAP_QOS_CMPL_EVT,                        /*!< QOS complete event */
    ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT,              /*!< ACL connection complete status event */
    ESP_BT_GAP_ACL_DISCONN_CMPL_STAT_EVT,           /*!< ACL disconnection complete status event */
    ESP_BT_GAP_SET_PAGE_TO_EVT,                     /*!< Set page timeout event */
    ESP_BT_GAP_GET_PAGE_TO_EVT,                     /*!< Get page timeout event */
    ESP_BT_GAP_ACL_PKT_TYPE_CHANGED_EVT,            /*!< Set ACL packet types event */
    ESP_BT_GAP_ENC_CHG_EVT,                         /*!< Encryption change event */
    ESP_BT_GAP_SET_MIN_ENC_KEY_SIZE_EVT,            /*!< Set minimum encryption key size */
    ESP_BT_GAP_GET_DEV_NAME_CMPL_EVT,               /*!< Get device name complete event */
    ESP_BT_GAP_EVT_MAX,
} esp_bt_gap_cb_event_t;

/** Inquiry Mode */
typedef enum {
    ESP_BT_INQ_MODE_GENERAL_INQUIRY,                /*!< General inquiry mode */
    ESP_BT_INQ_MODE_LIMITED_INQUIRY,                /*!< Limited inquiry mode */
} esp_bt_inq_mode_t;

/** Minimum and Maximum inquiry length*/
#define ESP_BT_GAP_MIN_INQ_LEN                (0x01)  /*!< Minimum inquiry duration, unit is 1.28s */
#define ESP_BT_GAP_MAX_INQ_LEN                (0x30)  /*!< Maximum inquiry duration, unit is 1.28s */

/** Minimum, Default and Maximum poll interval **/
#define ESP_BT_GAP_TPOLL_MIN                  (0x0006) /*!< Minimum poll interval, unit is 625 microseconds */
#define ESP_BT_GAP_TPOLL_DFT                  (0x0028) /*!< Default poll interval, unit is 625 microseconds */
#define ESP_BT_GAP_TPOLL_MAX                  (0x1000) /*!< Maximum poll interval, unit is 625 microseconds */

/// GAP state callback parameters
typedef union {
    /**
     * @brief ESP_BT_GAP_DISC_RES_EVT
     */
    struct disc_res_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        int num_prop;                          /*!< number of properties got */
        esp_bt_gap_dev_prop_t *prop;           /*!< properties discovered from the new device */
    } disc_res;                                /*!< discovery result parameter struct */

    /**
     * @brief  ESP_BT_GAP_DISC_STATE_CHANGED_EVT
     */
    struct disc_state_changed_param {
        esp_bt_gap_discovery_state_t state;    /*!< discovery state */
    } disc_st_chg;                             /*!< discovery state changed parameter struct */

    /**
     * @brief ESP_BT_GAP_RMT_SRVCS_EVT
     */
    struct rmt_srvcs_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_status_t stat;                  /*!< service search status */
        int num_uuids;                         /*!< number of UUID in uuid_list */
        esp_bt_uuid_t *uuid_list;              /*!< list of service UUIDs of remote device */
    } rmt_srvcs;                               /*!< services of remote device parameter struct */

    /**
     * @brief ESP_BT_GAP_RMT_SRVC_REC_EVT
     */
    struct rmt_srvc_rec_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_status_t stat;                  /*!< service search status */
    } rmt_srvc_rec;                            /*!< specific service record from remote device parameter struct */

    /**
     * @brief ESP_BT_GAP_READ_RSSI_DELTA_EVT *
     */
    struct read_rssi_delta_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_status_t stat;                  /*!< read rssi status */
        int8_t rssi_delta;                     /*!< rssi delta value range -128 ~127, The value zero indicates that the RSSI is inside the Golden Receive Power Range, the Golden Receive Power Range is from ESP_BT_GAP_RSSI_LOW_THRLD to ESP_BT_GAP_RSSI_HIGH_THRLD */
    } read_rssi_delta;                         /*!< read rssi parameter struct */

    /**
     * @brief ESP_BT_GAP_CONFIG_EIR_DATA_EVT *
     */
    struct config_eir_data_param {
        esp_bt_status_t stat;                                   /*!< config EIR status:
                                                                    ESP_BT_STATUS_SUCCESS: config success
                                                                    ESP_BT_STATUS_EIR_TOO_LARGE: the EIR data is more than 240B. The EIR may not contain the whole data.
                                                                    others: failed
                                                                */
        uint8_t eir_type_num;                                   /*!< the number of EIR types in EIR type */
        esp_bt_eir_type_t eir_type[ESP_BT_EIR_TYPE_MAX_NUM];    /*!< EIR types in EIR type */
    } config_eir_data;                                          /*!< config EIR data */

    /**
     * @brief ESP_BT_GAP_AUTH_CMPL_EVT
     */
    struct auth_cmpl_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_status_t stat;                  /*!< authentication complete status */
        esp_bt_link_key_type_t lk_type;        /*!< type of link key generated */
        uint8_t device_name[ESP_BT_GAP_MAX_BDNAME_LEN + 1]; /*!< device name */
    } auth_cmpl;                               /*!< authentication complete parameter struct */

    /**
     * @brief ESP_BT_GAP_ENC_CHG_EVT
     */
    struct enc_chg_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_enc_mode_t enc_mode;            /*!< encryption mode */
    } enc_chg;                                 /*!< encryption change parameter struct */

    /**
     * @brief ESP_BT_GAP_PIN_REQ_EVT
     */
    struct pin_req_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        bool min_16_digit;                     /*!< TRUE if the pin returned must be at least 16 digits */
    } pin_req;                                 /*!< pin request parameter struct */

    /**
     * @brief ESP_BT_GAP_CFM_REQ_EVT
     */
    struct cfm_req_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        uint32_t num_val;                      /*!< the numeric value for comparison. */
    } cfm_req;                                 /*!< confirm request parameter struct */

    /**
     * @brief ESP_BT_GAP_KEY_NOTIF_EVT
     */
    struct key_notif_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        uint32_t passkey;                      /*!< the numeric value for passkey entry. */
    } key_notif;                               /*!< passkey notif parameter struct */

    /**
     * @brief ESP_BT_GAP_KEY_REQ_EVT
     */
    struct key_req_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
    } key_req;                                 /*!< passkey request parameter struct */

    /**
     * @brief ESP_BT_GAP_SET_AFH_CHANNELS_EVT
     */
    struct set_afh_channels_param {
        esp_bt_status_t stat;                  /*!< set AFH channel status */
    } set_afh_channels;                        /*!< set AFH channel parameter struct */

    /**
     * @brief ESP_BT_GAP_READ_REMOTE_NAME_EVT
     */
    struct read_rmt_name_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        esp_bt_status_t stat;                  /*!< read Remote Name status */
        uint8_t rmt_name[ESP_BT_GAP_MAX_BDNAME_LEN + 1]; /*!< Remote device name */
    } read_rmt_name;                        /*!< read Remote Name parameter struct */

    /**
     * @brief ESP_BT_GAP_MODE_CHG_EVT
     */
    struct mode_chg_param {
        esp_bd_addr_t bda;                      /*!< remote bluetooth device address */
        esp_bt_pm_mode_t mode;                  /*!< PM mode */
        uint16_t interval;                      /*!< Number of baseband slots. unit is 0.625ms */
    } mode_chg;                                 /*!< mode change event parameter struct */

    /**
     * @brief ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT
     */
    struct bt_remove_bond_dev_cmpl_evt_param {
        esp_bd_addr_t bda;                          /*!< remote bluetooth device address*/
        esp_bt_status_t status;                     /*!< Indicate the remove bond device operation success status */
    }remove_bond_dev_cmpl;                           /*!< Event parameter of ESP_BT_GAP_REMOVE_BOND_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_BT_GAP_QOS_CMPL_EVT
     */
    struct qos_cmpl_param {
        esp_bt_status_t stat;                  /*!< QoS status */
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        uint32_t t_poll;                       /*!< poll interval, the maximum time between transmissions
                                                    which from the master to a particular slave on the ACL
                                                    logical transport. unit is 0.625ms. */
    } qos_cmpl;                                /*!< QoS complete parameter struct */

    /**
     * @brief ESP_BT_GAP_SET_PAGE_TO_EVT
     */
    struct page_to_set_param {
        esp_bt_status_t stat;                   /*!< set page timeout status*/
    } set_page_timeout;                         /*!< set page timeout parameter struct */

    /**
     * @brief ESP_BT_GAP_GET_PAGE_TO_EVT
     */
    struct page_to_get_param {
        esp_bt_status_t stat;                   /*!< get page timeout status*/
        uint16_t page_to;                       /*!< page_timeout value to be set, unit is 0.625ms. */
    } get_page_timeout;                         /*!< get page timeout parameter struct */

    /**
     * @brief ESP_BT_GAP_ACL_PKT_TYPE_CHANGED_EVT
     */
    struct set_acl_pkt_types_param {
        esp_bt_status_t status;                 /*!< set ACL packet types status */
        esp_bd_addr_t bda;                      /*!< remote bluetooth device address */
        uint16_t pkt_types;                     /*!< packet types successfully set */
    } set_acl_pkt_types;                        /*!< set ACL packet types parameter struct */

    /**
     * @brief ESP_BT_GAP_SET_MIN_ENC_KEY_SIZE_EVT
     */
    struct set_min_enc_key_size_param {
        esp_bt_status_t status;                 /*!< set minimum encryption key size status */
    } set_min_enc_key_size;                     /*!< set minimum encryption key size parameter struct */

    /**
     * @brief ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT
     */
    struct acl_conn_cmpl_stat_param {
        esp_bt_status_t stat;                  /*!< ACL connection status */
        uint16_t handle;                       /*!< ACL connection handle */
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address */
    } acl_conn_cmpl_stat;                      /*!< ACL connection complete status parameter struct */

    /**
     * @brief ESP_BT_GAP_ACL_DISCONN_CMPL_STAT_EVT
     */
    struct acl_disconn_cmpl_stat_param {
        esp_bt_status_t reason;                /*!< ACL disconnection reason */
        uint16_t handle;                       /*!< ACL connection handle */
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address */
    } acl_disconn_cmpl_stat;                   /*!< ACL disconnection complete status parameter struct */

    /**
     * @brief ESP_GAP_BT_GET_DEV_NAME_CMPL_EVT
     */
    struct get_dev_name_cmpl_evt_param {
        esp_bt_status_t status;                /*!< Indicate the get device name success status */
        char *name;                            /*!< Name of bluetooth device */
    } get_dev_name_cmpl;                       /*!< Get device name complete status parameter struct */
} esp_bt_gap_cb_param_t;

/**
 * @brief           bluetooth GAP callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_bt_gap_cb_t)(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

/**
 * @brief           get major service field of COD
 *
 * @param[in]       cod: Class of Device
 *
 * @return          major service bits
 */
static inline uint32_t esp_bt_gap_get_cod_srvc(uint32_t cod)
{
    return (cod & ESP_BT_COD_SRVC_BIT_MASK) >> ESP_BT_COD_SRVC_BIT_OFFSET;
}

/**
 * @brief           get major device field of COD
 *
 * @param[in]       cod: Class of Device
 *
 * @return          major device bits
 */
static inline uint32_t esp_bt_gap_get_cod_major_dev(uint32_t cod)
{
    return (cod & ESP_BT_COD_MAJOR_DEV_BIT_MASK) >> ESP_BT_COD_MAJOR_DEV_BIT_OFFSET;
}

/**
 * @brief           get minor service field of COD
 *
 * @param[in]       cod: Class of Device
 *
 * @return          minor service bits
 */
static inline uint32_t esp_bt_gap_get_cod_minor_dev(uint32_t cod)
{
    return (cod & ESP_BT_COD_MINOR_DEV_BIT_MASK) >> ESP_BT_COD_MINOR_DEV_BIT_OFFSET;
}

/**
 * @brief           get format type of COD
 *
 * @param[in]       cod: Class of Device
 *
 * @return          format type
 */
static inline uint32_t esp_bt_gap_get_cod_format_type(uint32_t cod)
{
    return (cod & ESP_BT_COD_FORMAT_TYPE_BIT_MASK);
}

/**
 * @brief           decide the integrity of COD
 *
 * @param[in]       cod: Class of Device
 *
 * @return
 *                  - true if cod is valid
 *                  - false otherwise
 */
static inline bool esp_bt_gap_is_valid_cod(uint32_t cod)
{
    if (esp_bt_gap_get_cod_format_type(cod) == ESP_BT_COD_FORMAT_TYPE_1 &&
            esp_bt_gap_get_cod_srvc(cod) != ESP_BT_COD_SRVC_NONE) {
        return true;
    }

    return false;
}

/**
 * @brief           register callback function. This function should be called after esp_bluedroid_enable() completes successfully
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_register_callback(esp_bt_gap_cb_t callback);

/**
 * @brief           Set discoverability and connectability mode for legacy bluetooth. This function should
 *                  be called after esp_bluedroid_enable() completes successfully
 *
 * @param[in]       c_mode : one of the enums of esp_bt_connection_mode_t
 *
 * @param[in]       d_mode : one of the enums of esp_bt_discovery_mode_t
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG: if argument invalid
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_set_scan_mode(esp_bt_connection_mode_t c_mode, esp_bt_discovery_mode_t d_mode);

/**
 * @brief           This function starts Inquiry and Name Discovery. This function should be called after esp_bluedroid_enable() completes successfully.
 *                  When Inquiry is halted and cached results do not contain device name, then Name Discovery will connect to the peer target to get the device name.
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_DISC_STATE_CHANGED_EVT when Inquiry is started or Name Discovery is completed.
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_DISC_RES_EVT each time the two types of discovery results are got.
 *
 * @param[in]       mode - Inquiry mode
 *
 * @param[in]       inq_len - Inquiry duration in 1.28 sec units, ranging from 0x01 to 0x30. This parameter only specifies the total duration of the Inquiry process,
 *                          - when this time expires, Inquiry will be halted.
 *
 * @param[in]       num_rsps - Number of responses that can be received before the Inquiry is halted, value 0 indicates an unlimited number of responses.
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if invalid parameters are provided
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_start_discovery(esp_bt_inq_mode_t mode, uint8_t inq_len, uint8_t num_rsps);

/**
 * @brief           Cancel Inquiry and Name Discovery. This function should be called after esp_bluedroid_enable() completes successfully.
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_DISC_STATE_CHANGED_EVT if Inquiry or Name Discovery is cancelled by
 *                  calling this function.
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_cancel_discovery(void);

/**
 * @brief           Start SDP to get remote services. This function should be called after esp_bluedroid_enable() completes successfully.
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_RMT_SRVCS_EVT after service discovery ends.
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_get_remote_services(esp_bd_addr_t remote_bda);

/**
 * @brief           Start SDP to look up the service matching uuid on the remote device. This function should be called after
 *                  esp_bluedroid_enable() completes successfully.
 *
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_RMT_SRVC_REC_EVT after service discovery ends
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_get_remote_service_record(esp_bd_addr_t remote_bda, esp_bt_uuid_t *uuid);

/**
 * @brief           This function is called to get EIR data for a specific type.
 *
 * @param[in]       eir - pointer of raw eir data to be resolved
 * @param[in]       type   - specific EIR data type
 * @param[out]      length - return the length of EIR data excluding fields of length and data type
 *
 * @return          pointer of starting position of eir data excluding eir data type, NULL if not found
 *
 */
uint8_t *esp_bt_gap_resolve_eir_data(uint8_t *eir, esp_bt_eir_type_t type, uint8_t *length);

/**
 * @brief           This function is called to config EIR data.
 *
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_CONFIG_EIR_DATA_EVT after config EIR ends.
 *
 * @param[in]       eir_data - pointer of EIR data content
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if param is invalid
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_config_eir_data(esp_bt_eir_data_t *eir_data);

/**
 * @brief           This function is called to set class of device.
 *                  The structure esp_bt_gap_cb_t will be called with ESP_BT_GAP_SET_COD_EVT after set COD ends.
 *                  This function should be called after Bluetooth profiles are initialized, otherwise the user configured
 *                  class of device can be overwritten.
 *                  Some profiles have special restrictions on class of device, and changes may make these profiles unable to work.
 *
 * @param[in]       cod - class of device
 * @param[in]       mode - setting mode
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if param is invalid
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_set_cod(esp_bt_cod_t cod, esp_bt_cod_mode_t mode);

/**
 * @brief           This function is called to get class of device.
 *
 * @param[out]      cod - class of device
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_get_cod(esp_bt_cod_t *cod);

/**
 * @brief           This function is called to read RSSI delta by address after connected. The RSSI value returned by ESP_BT_GAP_READ_RSSI_DELTA_EVT.
 *
 *
 * @param[in]       remote_addr - remote device address, corresponding to a certain connection handle
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_FAIL: others
 *
 */
esp_err_t esp_bt_gap_read_rssi_delta(esp_bd_addr_t remote_addr);

/**
* @brief           Removes a device from the security database list of
*                  peer device.
*
* @param[in]       bd_addr : BD address of the peer device
*
* @return          - ESP_OK : success
*                  - ESP_FAIL  : failed
*
*/
esp_err_t esp_bt_gap_remove_bond_device(esp_bd_addr_t bd_addr);

/**
* @brief           Get the device number from the security database list of peer device.
*                  It will return the device bonded number immediately.
*
* @return          - >= 0 : bonded devices number
*                  - ESP_FAIL  : failed
*
*/
int esp_bt_gap_get_bond_device_num(void);

/**
* @brief           Get the device from the security database list of peer device.
*                  It will return the device bonded information immediately.
*
* @param[inout]    dev_num: Indicate the dev_list array(buffer) size as input.
*                           If dev_num is large enough, it means the actual number as output.
*                           Suggest that dev_num value equal to esp_ble_get_bond_device_num().
*
* @param[out]      dev_list: an array(buffer) of `esp_bd_addr_t` type. Use for storing the bonded devices address.
*                            The dev_list should be allocated by who call this API.
*
* @return
*                  - ESP_OK : Succeed
*                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                  - ESP_FAIL: others
*/
esp_err_t esp_bt_gap_get_bond_device_list(int *dev_num, esp_bd_addr_t *dev_list);

/**
* @brief            Set pin type and default pin code for legacy pairing.
*
* @param[in]        pin_type:       Use variable or fixed pin.
*                                   If pin_type is ESP_BT_PIN_TYPE_VARIABLE, pin_code and pin_code_len
*                                   will be ignored, and ESP_BT_GAP_PIN_REQ_EVT will come when control
*                                   requests for pin code.
*                                   Else, will use fixed pin code and not callback to users.
*
* @param[in]        pin_code_len:   Length of pin_code
*
* @param[in]        pin_code:       Pin_code
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*/
esp_err_t esp_bt_gap_set_pin(esp_bt_pin_type_t pin_type, uint8_t pin_code_len, esp_bt_pin_code_t pin_code);

/**
* @brief            Reply the pin_code to the peer device for legacy pairing
*                   when ESP_BT_GAP_PIN_REQ_EVT is coming.
*
* @param[in]        bd_addr:        BD address of the peer
*
* @param[in]        accept:         Pin_code reply successful or declined.
*
* @param[in]        pin_code_len:   Length of pin_code
*
* @param[in]        pin_code:       Pin_code
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*/
esp_err_t esp_bt_gap_pin_reply(esp_bd_addr_t bd_addr, bool accept, uint8_t pin_code_len, esp_bt_pin_code_t pin_code);

/**
* @brief            Set a GAP security parameter value. Overrides the default value.
*
* @param[in]        param_type : the type of the param which is to be set
*
* @param[in]        value  : the param value
*
* @param[in]        len : the length of the param value
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_set_security_param(esp_bt_sp_param_t param_type,
                                        void *value, uint8_t len);

/**
* @brief            Reply the key value to the peer device in the legacy connection stage.
*
* @param[in]        bd_addr : BD address of the peer
*
* @param[in]        accept : passkey entry successful or declined.
*
* @param[in]        passkey : passkey value, must be a 6 digit number, can be lead by 0.
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_ssp_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey);


/**
* @brief            Reply the confirm value to the peer device in the legacy connection stage.
*
* @param[in]        bd_addr : BD address of the peer device
*
* @param[in]        accept : numbers to compare are the same or different
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_ssp_confirm_reply(esp_bd_addr_t bd_addr, bool accept);

/**
* @brief            Set the AFH channels
*
* @param[in]        channels :  The n th such field (in the range 0 to 78) contains the value for channel n :
*                               0 means channel n is bad.
*                               1 means channel n is unknown.
*                               The most significant bit is reserved and shall be set to 0.
*                               At least 20 channels shall be marked as unknown.
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_set_afh_channels(esp_bt_gap_afh_channels channels);

/**
* @brief            Read the remote device name
*
* @param[in]        remote_bda: The remote device's address
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_read_remote_name(esp_bd_addr_t remote_bda);

/**
* @brief            Config Quality of service
*
* @param[in]        remote_bda: The remote device's address
* @param[in]        t_poll:     Poll interval, the maximum time between transmissions
                                which from the master to a particular slave on the ACL
                                logical transport. unit is 0.625ms
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_bt_gap_set_qos(esp_bd_addr_t remote_bda, uint32_t t_poll);

/**
 * @brief           Set the page timeout
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_SET_PAGE_TO_EVT
 *                  after set page timeout ends. The value to be set will not be effective util the
 *                  next page procedure, it's suggested to set the page timeout before initiating
 *                  a connection.
 *
 * @param[in]       page_to: Page timeout, the maximum time the master will wait for a
                             Base-band page response from the remote device at a locally
                             initiated connection attempt. The valid range is 0x0016 ~ 0xffff,
                             the default value is 0x2000, unit is 0.625ms.
 *
 * @return          - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - other: failed
 */
esp_err_t esp_bt_gap_set_page_timeout(uint16_t page_to);

/**
 * @brief           Get the page timeout
 *                  esp_bt_gap_cb_t will be called with ESP_BT_GAP_GET_PAGE_TO_EVT
 *                  after get page timeout ends
 *
 * @return          - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - other: failed
 */
esp_err_t esp_bt_gap_get_page_timeout(void);

/**
 * @brief           Set ACL packet types
 *                  An ESP_BT_GAP_SET_ACL_PPKT_TYPES_EVT event will reported to
 *                  the APP layer.
 *
 * @return          - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - other: failed
 */
esp_err_t esp_bt_gap_set_acl_pkt_types(esp_bd_addr_t remote_bda, esp_bt_acl_pkt_type_t pkt_types);

/**
 * @brief           Set the minimal size of encryption key
 *
 * @return          - ESP_OK: success
 *                  - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - other: failed
 */
esp_err_t esp_bt_gap_set_min_enc_key_size(uint8_t key_size);

/**
 * @brief           Set device name to the local device
 *
 * @param[in]       name - device name.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_gap_set_device_name(const char *name);

/**
 * @brief           Get device name of the local device
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_gap_get_device_name(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BT_API_H__ */
