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

#ifndef __ESP_GAP_BT_API_H__
#define __ESP_GAP_BT_API_H__

#include <stdint.h>
#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/// Discoverability and Connectability mode
typedef enum {
    ESP_BT_SCAN_MODE_NONE = 0,                      /*!< Neither discoverable nor connectable */
    ESP_BT_SCAN_MODE_CONNECTABLE,                   /*!< Connectable but not discoverable */
    ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE       /*!< both discoverable and connectable */
} esp_bt_scan_mode_t;

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
    esp_bt_gap_dev_prop_type_t type;                /*!< device property type */
    int len;                                        /*!< device property value length */
    void *val;                                      /*!< devlice prpoerty value */
} esp_bt_gap_dev_prop_t;

/// Extended Inquiry Response data type
typedef enum {
    ESP_BT_EIR_TYPE_FLAGS                    = 0x01,     /*!< Flag with information such as BR/EDR and LE support */
    ESP_BT_EIR_TYPE_INCMPL_16BITS_UUID       = 0x02,     /*!< Incomplete list of 16-bit service UUIDs */
    ESP_BT_EIR_TYPE_CMPL_16BITS_UUID         = 0x03,     /*!< Complete list of 16-bit service UUIDs */
    ESP_BT_EIR_TYPE_INCMPL_32BITS_UUID       = 0x04,     /*!< Incomplete list of 32-bit service UUIDs */
    ESP_BT_EIR_TYPE_CMPL_32BITS_UUID         = 0x05,     /*!< Complete list of 32-bit service UUIDs */
    ESP_BT_EIR_TYPE_INCMPL_128BITS_UUID      = 0x06,     /*!< Incomplete list of 128-bit service UUIDs */
    ESP_BT_EIR_TYPE_CMPL_128BITS_UUID        = 0x07,     /*!< Complete list of 128-bit service UUIDs */
    ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME         = 0x08,     /*!< Shortened Local Name */
    ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME          = 0x09,     /*!< Complete Local Name */
    ESP_BT_EIR_TYPE_TX_POWER_LEVEL           = 0x0a,     /*!< Tx power level, value is 1 octet ranging from  -127 to 127, unit is dBm*/
    ESP_BT_EIR_TYPE_MANU_SPECIFIC            = 0xff,     /*!< Manufacturer specific data */
} esp_bt_eir_type_t;

/// Major service class field of Class of Device, mutiple bits can be set
typedef enum {
    ESP_BT_COD_SRVC_NONE                     =     0,    /*!< None indicates an invalid value */
    ESP_BT_COD_SRVC_LMTD_DISCOVER            =   0x1,    /*!< Limited Discoverable Mode */
    ESP_BT_COD_SRVC_POSITIONING              =   0x8,    /*!< Positioning (Location identification) */
    ESP_BT_COD_SRVC_NETWORKING               =  0x10,    /*!< Networking, e.g. LAN, Ad hoc */
    ESP_BT_COD_SRVC_RENDERING                =  0x20,    /*!< Rendering, e.g. Printing, Speakers */
    ESP_BT_COD_SRVC_CAPTURING                =  0x40,    /*!< Capturing, e.g. Scanner, Microphone */
    ESP_BT_COD_SRVC_OBJ_TRANSFER             =  0x80,    /*!< Object Transfer, e.g. v-Inbox, v-Folder */
    ESP_BT_COD_SRVC_AUDIO                    = 0x100,    /*!< Audio, e.g. Speaker, Microphone, Headerset service */
    ESP_BT_COD_SRVC_TELEPHONY                = 0x200,    /*!< Telephony, e.g. Cordless telephony, Modem, Headset service */
    ESP_BT_COD_SRVC_INFORMATION              = 0x400,    /*!< Information, e.g., WEB-server, WAP-server */
} esp_bt_cod_srvc_t;

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
    ESP_BT_GAP_DISCOVERY_STOPPED,                   /*!< device discovery stopped */
    ESP_BT_GAP_DISCOVERY_STARTED,                   /*!< device discovery started */
} esp_bt_gap_discovery_state_t;

/// BT GAP callback events
typedef enum {
    ESP_BT_GAP_DISC_RES_EVT = 0,                    /*!< device discovery result event */
    ESP_BT_GAP_DISC_STATE_CHANGED_EVT,              /*!< discovery state changed event */
    ESP_BT_GAP_RMT_SRVCS_EVT,                       /*!< get remote services event */
    ESP_BT_GAP_RMT_SRVC_REC_EVT,                    /*!< get remote service record event */
} esp_bt_gap_cb_event_t;

/** Inquiry Mode */
typedef enum {
    ESP_BT_INQ_MODE_GENERAL_INQUIRY,                /*!< General inquiry mode */
    ESP_BT_INQ_MODE_LIMITED_INQIURY,                /*!< Limited inquiry mode */
} esp_bt_inq_mode_t;

/** Minimum and Maximum inquiry length*/
#define ESP_BT_GAP_MIN_INQ_LEN                (0x01)  /*!< Minimum inquiry duration, unit is 1.28s */
#define ESP_BT_GAP_MAX_INQ_LEN                (0x30)  /*!< Maximum inquiry duration, unit is 1.28s */

/// A2DP state callback parameters
typedef union {
    /**
     * @brief ESP_BT_GAP_DISC_RES_EVT
     */
    struct disc_res_param {
        esp_bd_addr_t bda;                     /*!< remote bluetooth device address*/
        int num_prop;                          /*!< number of properties got */
        esp_bt_gap_dev_prop_t *prop;           /*!< properties discovered from the new device */
    } disc_res;                                /*!< discovery result paramter struct */

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
} esp_bt_gap_cb_param_t;

/**
 * @brief           bluetooth GAP callback function type
 * @param           event : Event type
 * @param           param : Pointer to callback parameter
 */
typedef void (* esp_bt_gap_cb_t)(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

/**
 * @brief           get major service field of COD
 * @param[in]       cod: Class of Device
 * @return          major service bits
 */
inline uint32_t esp_bt_gap_get_cod_srvc(uint32_t cod)
{
    return (cod & ESP_BT_COD_SRVC_BIT_MASK) >> ESP_BT_COD_SRVC_BIT_OFFSET;
}

/**
 * @brief           get major device field of COD
 * @param[in]       cod: Class of Device
 * @return          major device bits
 */
inline uint32_t esp_bt_gap_get_cod_major_dev(uint32_t cod)
{
    return (cod & ESP_BT_COD_MAJOR_DEV_BIT_MASK) >> ESP_BT_COD_MAJOR_DEV_BIT_OFFSET;
}

/**
 * @brief           get minor service field of COD
 * @param[in]       cod: Class of Device
 * @return          minor service bits
 */
inline uint32_t esp_bt_gap_get_cod_minor_dev(uint32_t cod)
{
    return (cod & ESP_BT_COD_MINOR_DEV_BIT_MASK) >> ESP_BT_COD_MINOR_DEV_BIT_OFFSET;
}

/**
 * @brief           get format type of COD
 * @param[in]       cod: Class of Device
 * @return          format type
 */
inline uint32_t esp_bt_gap_get_cod_format_type(uint32_t cod)
{
    return (cod & ESP_BT_COD_FORMAT_TYPE_BIT_MASK);
}

/**
 * @brief           decide the integrity of COD
 * @param[in]       cod: Class of Device
 * @return
 *                  - true if cod is valid
 *                  - false otherise
 */
inline bool esp_bt_gap_is_valid_cod(uint32_t cod)
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
 * @param[in]       mode : one of the enums of bt_scan_mode_t
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_ERR_INVALID_ARG: if argument invalid
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_set_scan_mode(esp_bt_scan_mode_t mode);

/**
 * @brief           Start device discovery. This function should be called after esp_bluedroid_enable() completes successfully.
 *                  esp_bt_gap_cb_t will is called with ESP_BT_GAP_DISC_STATE_CHANGED_EVT if discovery is started or halted.
 *                  esp_bt_gap_cb_t will is called with ESP_BT_GAP_DISC_RES_EVT if discovery result is got.
 *
 * @param[in]       mode - inquiry mode
 * @param[in]       inq_len - inquiry duration in 1.28 sec units, ranging from 0x01 to 0x30
 * @param[in]       num_rsps - number of inquiry responses that can be received, value 0 indicates an unlimited number of responses
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_ERR_INVALID_ARG: if invalid parameters are provided
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_start_discovery(esp_bt_inq_mode_t mode, uint8_t inq_len, uint8_t num_rsps);

/**
 * @brief           Cancel device discovery. This function should be called after esp_bluedroid_enable() completes successfully
 *                  esp_bt_gap_cb_t will is called with ESP_BT_GAP_DISC_STATE_CHANGED_EVT if discovery is stopped.
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_cancel_discovery(void);

/**
 * @brief           Start SDP to get remote services. This function should be called after esp_bluedroid_enable() completes successfully.
 *                  esp_bt_gap_cb_t will is called with ESP_BT_GAP_RMT_SRVCS_EVT after service discovery ends
 *
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - ESP_FAIL: others
 */
esp_err_t esp_bt_gap_get_remote_services(esp_bd_addr_t remote_bda);

/**
 * @brief           Start SDP to look up the service matching uuid on the remote device. This function should be called after
 *                  esp_bluedroid_enable() completes successfully
 *
 *                  esp_bt_gap_cb_t will is called with ESP_BT_GAP_RMT_SRVC_REC_EVT after service discovery ends
 * @return
 *                  - ESP_OK : Succeed
 *                  - ESP_INVALID_STATE: if bluetooth stack is not yet enabled
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

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BT_API_H__ */
