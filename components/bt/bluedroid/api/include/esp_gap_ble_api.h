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

#ifndef __ESP_GAP_BLE_API_H__
#define __ESP_GAP_BLE_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@{
 * BLE_ADV_DATA_FLAG data flag bit definition used for advertising data flag
 */
#define ESP_BLE_ADV_FLAG_LIMIT_DISC         (0x01 << 0)
#define ESP_BLE_ADV_FLAG_GEN_DISC           (0x01 << 1)
#define ESP_BLE_ADV_FLAG_BREDR_NOT_SPT      (0x01 << 2)
#define ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT (0x01 << 3)
#define ESP_BLE_ADV_FLAG_DMT_HOST_SPT       (0x01 << 4)
#define ESP_BLE_ADV_FLAG_NON_LIMIT_DISC     (0x00 )
/**
 * @}
 */

#define ESP_LE_KEY_NONE                    0
#define ESP_LE_KEY_PENC                    (1 << 0)                     /*!< encryption key, encryption information of peer device */
#define ESP_LE_KEY_PID                     (1 << 1)                     /*!< identity key of the peer device */
#define ESP_LE_KEY_PCSRK                   (1 << 2)                     /*!< peer SRK */
#define ESP_LE_KEY_PLK                     (1 << 3)                     /*!< Link key*/	
#define ESP_LE_KEY_LLK                     (ESP_LE_KEY_PLK << 4)
#define ESP_LE_KEY_LENC                    (ESP_LE_KEY_PENC << 4)                   /*!< master role security information:div */
#define ESP_LE_KEY_LID                     (ESP_LE_KEY_PID << 4)                    /*!< master device ID key */
#define ESP_LE_KEY_LCSRK                   (ESP_LE_KEY_PCSRK << 4)                  /*!< local CSRK has been deliver to peer */
typedef uint8_t	esp_ble_key_type_t;

#define ESP_LE_AUTH_NO_BOND                 0x00                                     /*!< 0*/
#define ESP_LE_AUTH_BOND                    0x01                                     /*!< 1 << 0 */
#define ESP_LE_AUTH_REQ_MITM                (1 << 2)                                 /*!< 1 << 2 */
#define ESP_LE_AUTH_REQ_SC_ONLY             (1 << 3)                                 /*!< 1 << 3 */
#define ESP_LE_AUTH_REQ_SC_BOND             (ESP_LE_AUTH_BOND | ESP_LE_AUTH_REQ_SC_ONLY)            /*!< 1001 */
#define ESP_LE_AUTH_REQ_SC_MITM             (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY)        /*!< 1100 */
#define ESP_LE_AUTH_REQ_SC_MITM_BOND        (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY | ESP_LE_AUTH_BOND)   /*!< 1101 */
typedef uint8_t   esp_ble_auth_req_t;         /*!< combination of the above bit pattern */

#define ESP_IO_CAP_OUT                      0   /*!< DisplayOnly */
#define ESP_IO_CAP_IO                       1   /*!< DisplayYesNo */
#define ESP_IO_CAP_IN                       2   /*!< KeyboardOnly */
#define ESP_IO_CAP_NONE                     3   /*!< NoInputNoOutput */
#define ESP_IO_CAP_KBDISP                   4   /*!< Keyboard display */
typedef uint8_t esp_ble_io_cap_t;               /*!< combination of the io capability */


/// GAP BLE callback event type
typedef enum {
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,       /*!< When advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,             /*!< When scan response data set complete, the event comes */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,                /*!< When scan parameters set complete, the event comes */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                            /*!< When one scan result ready, the event comes each time */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,              /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,         /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,                     /*!< When start advertising complete, the event comes */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,                    /*!< When start scan complete, the event comes */
    ESP_GAP_BLE_AUTH_CMPL_EVT,                              /* Authentication complete indication. */
    ESP_GAP_BLE_KEY_EVT,                                    /* BLE  key event for peer device keys */
    ESP_GAP_BLE_SEC_REQ_EVT,                                /* BLE  security request */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                          /* passkey notification event */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                            /* passkey request event */
    ESP_GAP_BLE_OOB_REQ_EVT,                                /* OOB request event */
    ESP_GAP_BLE_LOCAL_IR_EVT,                               /* BLE local IR event */
    ESP_GAP_BLE_LOCAL_ER_EVT,                               /* BLE local ER event */
    ESP_GAP_BLE_NC_REQ_EVT,                                 /* Numeric Comparison request event */
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                      /*!< When stop adv complete, the event comes */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,                     /*!< When stop scan complete, the event comes */
} esp_gap_ble_cb_event_t;

/// Advertising data maximum length
#define ESP_BLE_ADV_DATA_LEN_MAX               31
/// Scan response data maximum length
#define ESP_BLE_SCAN_RSP_DATA_LEN_MAX          31

/// The type of advertising data(not adv_type)
typedef enum {
    ESP_BLE_AD_TYPE_FLAG                     = 0x01,
    ESP_BLE_AD_TYPE_16SRV_PART               = 0x02,
    ESP_BLE_AD_TYPE_16SRV_CMPL               = 0x03,
    ESP_BLE_AD_TYPE_32SRV_PART               = 0x04,
    ESP_BLE_AD_TYPE_32SRV_CMPL               = 0x05,
    ESP_BLE_AD_TYPE_128SRV_PART              = 0x06,
    ESP_BLE_AD_TYPE_128SRV_CMPL              = 0x07,
    ESP_BLE_AD_TYPE_NAME_SHORT               = 0x08,
    ESP_BLE_AD_TYPE_NAME_CMPL                = 0x09,
    ESP_BLE_AD_TYPE_TX_PWR                   = 0x0A,
    ESP_BLE_AD_TYPE_DEV_CLASS                = 0x0D,
    ESP_BLE_AD_TYPE_SM_TK                    = 0x10,
    ESP_BLE_AD_TYPE_SM_OOB_FLAG              = 0x11,
    ESP_BLE_AD_TYPE_INT_RANGE                = 0x12,
    ESP_BLE_AD_TYPE_SOL_SRV_UUID             = 0x14,
    ESP_BLE_AD_TYPE_128SOL_SRV_UUID          = 0x15,
    ESP_BLE_AD_TYPE_SERVICE_DATA             = 0x16,
    ESP_BLE_AD_TYPE_PUBLIC_TARGET            = 0x17,
    ESP_BLE_AD_TYPE_RANDOM_TARGET            = 0x18,
    ESP_BLE_AD_TYPE_APPEARANCE               = 0x19,
    ESP_BLE_AD_TYPE_ADV_INT                  = 0x1A,
    ESP_BLE_AD_TYPE_32SOL_SRV_UUID           = 0x1B,
    ESP_BLE_AD_TYPE_32SERVICE_DATA           = 0x1C,
    ESP_BLE_AD_TYPE_128SERVICE_DATA          = 0x1D,
    ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE    = 0xFF,
} esp_ble_adv_data_type;

/// Advertising mode
typedef enum {
    ADV_TYPE_IND                = 0x00,
    ADV_TYPE_DIRECT_IND_HIGH    = 0x01,
    ADV_TYPE_SCAN_IND           = 0x02,
    ADV_TYPE_NONCONN_IND        = 0x03,
    ADV_TYPE_DIRECT_IND_LOW     = 0x04,
} esp_ble_adv_type_t;

/// Advertising channel mask
typedef enum {
    ADV_CHNL_37     = 0x01,
    ADV_CHNL_38     = 0x02,
    ADV_CHNL_39     = 0x04,
    ADV_CHNL_ALL    = 0x07,
} esp_ble_adv_channel_t;

typedef enum {
    ///Allow both scan and connection requests from anyone
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY  = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST,
    ///Enumeration end value for advertising filter policy value check
} esp_ble_adv_filter_t;


typedef enum {
    ESP_BLE_SEC_NONE,                
    ESP_BLE_SEC_ENCRYPT,             
    ESP_BLE_SEC_ENCRYPT_NO_MITM,     
    ESP_BLE_SEC_ENCRYPT_MITM,        
}esp_ble_sec_act_t;

typedef enum {
    ESP_BLE_SM_PASSKEY,
    ESP_BLE_SM_AUTHEN_REQ_MODE,
    ESP_BLE_SM_IOCAP_MODE,
    ESP_BLE_SM_SET_INIT_KEY,
    ESP_BLE_SM_SET_RSP_KEK,
    ESP_BLE_SM_MAX_KEY_SIZE,
} esp_ble_sm_param_t;

/// Advertising parameters
typedef struct {
    uint16_t                adv_int_min;        /*!< Minimum advertising interval for
                                                  undirected and low duty cycle directed advertising.
                                                  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
                                                  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec */
    uint16_t                adv_int_max;        /*!< Maximum advertising interval for
                                                  undirected and low duty cycle directed advertising.
                                                  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
                                                  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec Advertising max interval */
    esp_ble_adv_type_t      adv_type;           /*!< Advertising type */
    esp_ble_addr_type_t     own_addr_type;      /*!< Owner bluetooth device address type */
    esp_bd_addr_t           peer_addr;          /*!< Peer device bluetooth device address */
    esp_ble_addr_type_t     peer_addr_type;     /*!< Peer device bluetooth device address type */
    esp_ble_adv_channel_t   channel_map;        /*!< Advertising channel map */
    esp_ble_adv_filter_t    adv_filter_policy;  /*!< Advertising filter policy */
} esp_ble_adv_params_t;

/// Advertising data content, according to "Supplement to the Bluetooth Core Specification"
typedef struct {
    bool                    set_scan_rsp;           /*!< Set this advertising data as scan response or not*/
    bool                    include_name;           /*!< Advertising data include device name or not */
    bool                    include_txpower;        /*!< Advertising data include TX power */
    int                     min_interval;           /*!< Advertising data show advertising min interval */
    int                     max_interval;           /*!< Advertising data show advertising max interval */
    int                     appearance;             /*!< External appearance of device */
    uint16_t                manufacturer_len;       /*!< Manufacturer data length */
    uint8_t                 *p_manufacturer_data;   /*!< Manufacturer data point */
    uint16_t                service_data_len;       /*!< Service data length */
    uint8_t                 *p_service_data;        /*!< Service data point */
    uint16_t                service_uuid_len;       /*!< Service uuid length */
    uint8_t                 *p_service_uuid;        /*!< Service uuid array point */
    uint8_t                 flag;                   /*!< Advertising flag of discovery mode, see BLE_ADV_DATA_FLAG detail */
} esp_ble_adv_data_t;

/// Ble scan type 
typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0,            /*!< Passive scan */
    BLE_SCAN_TYPE_ACTIVE    =   0x1,            /*!< Active scan */
} esp_ble_scan_type_t;

/// Ble scan filter type
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,  /*!< Accept all :
                                                  1. advertisement packets except directed advertising packets not addressed to this device (default). */
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST     = 0x1,  /*!< Accept only : 
                                                  1. advertisement packets from devices where the advertiser’s address is in the White list.
                                                  2. Directed advertising packets which are not addressed for this device shall be ignored. */
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR   = 0x2,  /*!< Accept all :
                                                  1. undirected advertisement packets, and
                                                  2. directed advertising packets where the initiator address is a resolvable private address, and
                                                  3. directed advertising packets addressed to this device. */
    BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR = 0x3,  /*!< Accept all :
                                                  1. advertisement packets from devices where the advertiser’s address is in the White list, and
                                                  2. directed advertising packets where the initiator address is a resolvable private address, and
                                                  3. directed advertising packets addressed to this device.*/
} esp_ble_scan_filter_t;

/// Ble scan parameters
typedef struct {
    esp_ble_scan_type_t     scan_type;              /*!< Scan type */
    esp_ble_addr_type_t     own_addr_type;          /*!< Owner address type */
    esp_ble_scan_filter_t   scan_filter_policy;     /*!< Scan filter policy */
    uint16_t                scan_interval;          /*!< Scan interval. This is defined as the time interval from
                                                      when the Controller started its last LE scan until it begins the subsequent LE scan.
                                                      Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
                                                      Time = N * 0.625 msec
                                                      Time Range: 2.5 msec to 10.24 seconds*/
    uint16_t                scan_window;            /*!< Scan window. The duration of the LE scan. LE_Scan_Window
                                                      shall be less than or equal to LE_Scan_Interval
                                                      Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
                                                      Time = N * 0.625 msec
                                                      Time Range: 2.5 msec to 10240 msec */
} esp_ble_scan_params_t;

/// Connection update parameters
typedef struct {
    esp_bd_addr_t bda;                              /*!< Bluetooth device address */
    uint16_t min_int;                               /*!< Min connection interval */
    uint16_t max_int;                               /*!< Max connection interval */
    uint16_t latency;                               /*!< Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3 */
    uint16_t timeout;                               /*!< Supervision timeout for the LE Link. Range: 0x000A to 0x0C80.
                                                      Mandatory Range: 0x000A to 0x0C80 Time = N * 10 msec
                                                      Time Range: 100 msec to 32 seconds */
} esp_ble_conn_update_params_t;

/**
* @brief BLE encryption keys
*/
typedef struct
{
    esp_bt_octet16_t     ltk;          /*!< The long term key*/
    esp_bt_octet8_t      rand;         /*!< The random number*/
    uint16_t             ediv;         /*!< The ediv value*/
    uint8_t              sec_level;    /*!< The security level of the security link*/
    uint8_t              key_size;     /*!< The key size(7~16) of the security link*/
}esp_ble_penc_keys_t;                  /*!< The key type*/

/**
* @brief  BLE CSRK keys 
*/
typedef struct
{
    uint32_t            counter;      /*!< The counter */
    esp_bt_octet16_t    csrk;         /*!< The csrk key */
    uint8_t             sec_level;    /*!< The security level */
}esp_ble_pcsrk_keys_t;                /*!< The pcsrk key type */

/**
* @brief  BLE pid keys 
*/
typedef struct
{
    esp_bt_octet16_t          irk;           /*!< The irk value */
    esp_ble_addr_type_t       addr_type;     /*!< The address type */
    esp_bd_addr_t             static_addr;   /*!< The static address */
}esp_ble_pid_keys_t;                         /*!< The pid key type */

/**
* @brief  BLE Encryption reproduction keys
*/
typedef struct
{
    esp_bt_octet16_t  ltk;                  /*!< The long term key */
    uint16_t          div;                  /*!< The div value */
    uint8_t           key_size;             /*!< The key size of the security link */
    uint8_t           sec_level;            /*!< The security level of the security link */
}esp_ble_lenc_keys_t;                       /*!< The  key type */

/**
* @brief  BLE SRK keys
*/
typedef struct
{
    uint32_t          counter;              /*!< The counter value */
    uint16_t          div;                  /*!< The div value */
    uint8_t           sec_level;            /*!< The security level of the security link */
    esp_bt_octet16_t            csrk;       /*!< The csrk key value */
}esp_ble_lcsrk_keys;                        /*!< The csrk key type */

/**
* @brief  Structure associated with ESP_KEY_NOTIF_EVT 
*/
typedef struct
{
    esp_bd_addr_t  bd_addr;        /*!< peer address */
    uint32_t       passkey;        /*!< the numeric value for comparison. If just_works, do not show this number to UI */
} esp_ble_sec_key_notif_t;         /*!< BLE key notify type*/

/**
* @brief  Structure of the security request
*/
typedef struct
{
    esp_bd_addr_t  bd_addr;        /*!< peer address */
}esp_ble_sec_req_t;                /*!< BLE security request type*/

/**
* @brief  union type of the security key value
*/
typedef union
{
    esp_ble_penc_keys_t   penc_key;       /*!< received peer encryption key */
    esp_ble_pcsrk_keys_t  pcsrk_key;      /*!< received peer device SRK */
    esp_ble_pid_keys_t    pid_key;        /*!< peer device ID key */
    esp_ble_lenc_keys_t   lenc_key;       /*!< local encryption reproduction keys LTK = = d1(ER,DIV,0)*/
    esp_ble_lcsrk_keys    lcsrk_key;      /*!< local device CSRK = d1(ER,DIV,1)*/
}esp_ble_key_value_t;                     /*!< ble key value type*/


/**
* @brief  union type of the security key value
*/
typedef struct
{
    esp_bd_addr_t               bd_addr;        /*!< peer address */
    esp_ble_key_type_t          key_type;       /*!< key type of the security link */
    esp_ble_key_value_t         p_key_value;    /*!< the pointer to the key value */
}esp_ble_key_t;                                 /*!< the union to the ble key value type*/

/**
* @brief  structure type of the ble local id keys value
*/
typedef struct {
    esp_bt_octet16_t       ir;                  /*!< the 16 bits of the ir value */
    esp_bt_octet16_t       irk;                 /*!< the 16 bits of the ir key value */
    esp_bt_octet16_t       dhk;                 /*!< the 16 bits of the dh key value */
}esp_ble_local_id_keys_t;                       /*!< the structure of the ble local id keys value type*/


/**
  * @brief Structure associated with ESP_AUTH_CMPL_EVT
  */
typedef struct
{
    esp_bd_addr_t         bd_addr;               /*!< BD address peer device. */	
    bool                  key_present;           /*!< Valid link key value in key element */
    esp_link_key          key;                   /*!< Link key associated with peer device. */
    uint8_t               key_type;              /*!< The type of Link Key */
    bool                  success;               /*!< TRUE of authentication succeeded, FALSE if failed. */
    uint8_t               fail_reason;           /*!< The HCI reason/error code for when success=FALSE */
    esp_ble_addr_type_t   addr_type;             /*!< Peer device address type */
    esp_bt_dev_type_t     dev_type;              /*!< Device type */
}esp_ble_auth_cmpl_t;                            /*!< The ble authentication complite cb type */

/**
  * @brief union associated with ble security
  */
typedef union
{
    esp_ble_sec_key_notif_t    key_notif;      /*!< passkey notification */
    esp_ble_sec_req_t          ble_req;        /*!< BLE SMP related request */
    esp_ble_key_t              ble_key;        /*!< BLE SMP keys used when pairing */
    esp_ble_local_id_keys_t    ble_id_keys;    /*!< BLE IR event */
    esp_ble_auth_cmpl_t        auth_cmpl;      /*!< Authentication complete indication. */
}esp_ble_sec_t;                                /*!< Ble  secutity type */

/// Sub Event of ESP_GAP_BLE_SCAN_RESULT_EVT
typedef enum {
    ESP_GAP_SEARCH_INQ_RES_EVT             = 0,      /*!< Inquiry result for a peer device. */
    ESP_GAP_SEARCH_INQ_CMPL_EVT            = 1,      /*!< Inquiry complete. */
    ESP_GAP_SEARCH_DISC_RES_EVT            = 2,      /*!< Discovery result for a peer device. */
    ESP_GAP_SEARCH_DISC_BLE_RES_EVT        = 3,      /*!< Discovery result for BLE GATT based service on a peer device. */
    ESP_GAP_SEARCH_DISC_CMPL_EVT           = 4,      /*!< Discovery complete. */
    ESP_GAP_SEARCH_DI_DISC_CMPL_EVT        = 5,      /*!< Discovery complete. */
    ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT  = 6,      /*!< Search cancelled */
} esp_gap_search_evt_t;

/**
 * @brief Ble scan result event type, to indicate the
 *        result is scan response or advertising data or other
 */
typedef enum {
    ESP_BLE_EVT_CONN_ADV         = 0x00,        /*!< Connectable undirected advertising (ADV_IND) */
    ESP_BLE_EVT_CONN_DIR_ADV     = 0x01,        /*!< Connectable directed advertising (ADV_DIRECT_IND) */
    ESP_BLE_EVT_DISC_ADV         = 0x02,        /*!< Scannable undirected advertising (ADV_SCAN_IND) */
    ESP_BLE_EVT_NON_CONN_ADV     = 0x03,        /*!< Non connectable undirected advertising (ADV_NONCONN_IND) */
    ESP_BLE_EVT_SCAN_RSP         = 0x04,        /*!< Scan Response (SCAN_RSP) */
} esp_ble_evt_type_t;

/**
 * @brief Gap callback parameters union
 */
typedef union {
    /**
     * @brief ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT
     */
    struct ble_adv_data_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set advertising data operation success status */
    } adv_data_cmpl;                                /*!< Event parameter of ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT */ 
    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT
     */
    struct ble_scan_rsp_data_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set scan response data operation success status */
    } scan_rsp_data_cmpl;                           /*!< Event parameter of ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT
     */
    struct ble_scan_param_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set scan param operation success status */
    } scan_param_cmpl;                              /*!< Event parameter of ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_RESULT_EVT
     */
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;            /*!< Search event type */
        esp_bd_addr_t bda;                          /*!< Bluetooth device address which has been searched */
        esp_bt_dev_type_t dev_type;                 /*!< Device type */
        esp_ble_addr_type_t ble_addr_type;          /*!< Ble device address type */
        esp_ble_evt_type_t ble_evt_type;            /*!< Ble scan result event type */
        int rssi;                                   /*!< Searched device's RSSI */
        uint8_t  ble_adv[ESP_BLE_ADV_DATA_LEN_MAX + ESP_BLE_SCAN_RSP_DATA_LEN_MAX];     /*!< Received EIR */
        int flag;                                   /*!< Advertising data flag bit */
        int num_resps;                              /*!< Scan result number */
        uint8_t adv_data_len;                       /*!< Adv data length */
        uint8_t scan_rsp_len;                       /*!< Scan response length */
    } scan_rst;                                     /*!< Event parameter of ESP_GAP_BLE_SCAN_RESULT_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT
     */
    struct ble_adv_data_raw_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set raw advertising data operation success status */
    } adv_data_raw_cmpl;                            /*!< Event parameter of ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT */ 
    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT
     */
    struct ble_scan_rsp_data_raw_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set raw advertising data operation success status */
    } scan_rsp_data_raw_cmpl;                       /*!< Event parameter of ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_START_COMPLETE_EVT
     */
    struct ble_adv_start_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising start operation success status */
    } adv_start_cmpl;                               /*!< Event parameter of ESP_GAP_BLE_ADV_START_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_START_COMPLETE_EVT
     */
    struct ble_scan_start_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate scan start operation success status */
    } scan_start_cmpl;                              /*!< Event parameter of ESP_GAP_BLE_SCAN_START_COMPLETE_EVT */

    esp_ble_sec_t ble_security;                     /*!< ble gap security union type */
    /**
     * @brief ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT
     */
    struct ble_scan_stop_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate scan stop operation success status */
    } scan_stop_cmpl;                               /*!< Event parameter of ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT
     */
    struct ble_adv_stop_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate adv stop operation success status */
    } adv_stop_cmpl;                                /*!< Event parameter of ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT */
} esp_ble_gap_cb_param_t;

/**
 * @brief GAP callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (* esp_gap_ble_cb_t)(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/**
 * @brief           This function is called to occur gap event, such as scan result
 *
 * @param[in]       callback: callback function
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback);


/**
 * @brief           This function is called to override the BTA default ADV parameters.
 *
 * @param[in]       adv_data: Pointer to User defined ADV data structure. This
 *                  memory space can not be freed until callback of config_adv_data
 *                  is received.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_adv_data (esp_ble_adv_data_t *adv_data);



/**
 * @brief           This function is called to set scan parameters
 *
 * @param[in]       scan_params: Pointer to User defined scan_params data structure. This
 *                  memory space can not be freed until callback of set_scan_params
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);


/**
 * @brief           This procedure keep the device scanning the peer device which advertising on the air
 *
 * @param[in]       duration: Keeping the scanning time, the unit is second.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);


/**
 * @brief          This function call to stop the device scanning the peer device which advertising on the air
 * @return
 *                 - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_stop_scanning(void);

/**
 * @brief           This function is called to start advertising.
 *
 * @param[in]       adv_params: pointer to User defined adv_params data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_start_advertising (esp_ble_adv_params_t *adv_params);



/**
 * @brief           This function is called to stop advertising.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_stop_advertising(void);



/**
 * @brief           Update connection parameters, can only be used when connection is up.
 *
 * @param[in]       params   -  connection update parameters
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params);


/**
 * @brief           This function is to set maximum LE data packet size
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length);



/**
 * @brief           This function set the random address for the application
 *
 * @param[in]       rand_addr: the random address which should be setting
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr);



/**
 * @brief           Enable/disable privacy on the local device
 *
 * @param[in]       privacy_enable   - enable/disable privacy on remote device.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_local_privacy (bool privacy_enable);


/**
 * @brief           Set device name to the local device
 *
 * @param[in]       name   -  device name.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_device_name(const char *name);


/**
 * @brief          This function is called to get ADV data for a specific type.
 *
 * @param[in]       adv_data - pointer of ADV data which to be resolved
 * @param[in]       type   - finding ADV data type
 * @param[out]      length - return the length of ADV data not including type
 *
 * @return          - ESP_OK : success
 *                  - other  : failed
 *
 */
uint8_t *esp_ble_resolve_adv_data(uint8_t *adv_data, uint8_t type, uint8_t *length);

/**
 * @brief           This function is called to set raw advertising data. User need to fill
 *                  ADV data by self.
 *
 * @param[in]       raw_data : raw advertising data
 * @param[in]       raw_data_len : raw advertising data length , less than 31 bytes
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len);

/**
 * @brief           This function is called to set raw scan response data. User need to fill
 *                  scan response data by self.
 *
 * @param[in]       raw_data : raw scan response data
 * @param[in]       raw_data_len : raw scan response data length , less than 31 bytes
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len);


/**
* @brief             Set a GAP security parameter value. Overrides the default value.
*
* @param[in]       param_type :L the type of the param which to be set
* @param[in]       value  : the param value
* @param[out]     len : the length of the param value
*
* @return            - ESP_OK : success
*                  		- other  : failed
*
*/
esp_err_t esp_ble_gap_set_security_param(esp_ble_sm_param_t param_type,
                                         void *value, uint8_t len);

/**
* @brief             Grant security request access.
*
* @param[in]       bd_addr : BD address of the peer
* @param[in]       accept  :  accept the security request or not
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_security_rsp(esp_bd_addr_t bd_addr,  bool accept);


/**
* @brief             Set a gap parameter value. Use this function to change
*                    the default GAP parameter values.
*
* @param[in]       bd_addr : the address of the peer device need to encryption
* @param[in]       sec_act  : This is the security action to indicate
*                                  	what kind of BLE security level is required for
*                                  	the BLE link if the BLE is supported
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_set_encryption(esp_bd_addr_t bd_addr, esp_ble_sec_act_t sec_act);

/**
* @brief          Reply the key value to the peer device in the lagecy connection stage.
*
* @param[in]      bd_addr : BD address of the peer
* @param[in]      accept : passkey entry sucessful or declined.
* @param[in]      passkey : passkey value, must be a 6 digit number,
*                                     can be lead by 0.
*
* @return            - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey);


/**
* @brief           Reply the comfirm value to the peer device in the lagecy connection stage.
*
* @param[in]       bd_addr : BD address of the peer device
* @param[in]       accept : numbers to compare are the same or different.
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_confirm_reply(esp_bd_addr_t bd_addr, bool accept);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BLE_API_H__ */
