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

/// GAP BLE callback event type
typedef enum {
	ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,		/*!< When advertising data set complete, the event comes */
	ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT ,			/*!< When scan response data set complete, the event comes */
	ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,				/*!< When scan parameters set complete, the event comes */
	ESP_GAP_BLE_SCAN_RESULT_EVT,							/*!< When one scan result ready, the event comes each time */
}esp_gap_ble_cb_event_t;

/// Advertising data maximum length
#define ESP_BLE_ADV_DATA_LEN_MAX        31

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
    ADV_CHNL_39     = 0x03,
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

/// Advertising parameters
typedef struct {
    uint16_t                adv_int_min;		/*!< Minimum advertising interval for
												  undirected and low duty cycle directed advertising.
 												  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
												  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec */
    uint16_t                adv_int_max;		/*!< Maximum advertising interval for
												  undirected and low duty cycle directed advertising.
 												  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
												  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec Advertising max interval */
    esp_ble_adv_type_t      adv_type;			/*!< Advertising type */
    esp_ble_addr_type_t     own_addr_type;		/*!< Owner bluetooth device address type */
    esp_bd_addr_t           peer_addr;			/*!< Peer device bluetooth device address */
    esp_ble_addr_type_t     peer_addr_type;		/*!< Peer device bluetooth device address type */
    esp_ble_adv_channel_t   channel_map;		/*!< Advertising channel map */
    esp_ble_adv_filter_t    adv_filter_policy;	/*!< Advertising filter policy */
} esp_ble_adv_params_t;

/// Advertising data content, according to "Supplement to the Bluetooth Core Specification"
typedef struct {
    bool                    set_scan_rsp;			/*!< Set this advertising data as scan response or not*/
    bool                    include_name;			/*!< Advertising data include device name or not */
    bool                    include_txpower;		/*!< Advertising data include TX power */
    int                     min_interval;			/*!< Advertising data show advertising min interval */
    int                     max_interval;			/*!< Advertising data show advertising max interval */
    int                     appearance;				/*!< External appearance of device */
    uint16_t                manufacturer_len;		/*!< Manufacturer data length */
    uint8_t                 *p_manufacturer_data;	/*!< Manufacturer data point */
    uint16_t                service_data_len;		/*!< Service data length */
    uint8_t                 *p_service_data;		/*!< Service data point */
    uint16_t                service_uuid_len;		/*!< Service uuid length */
    uint8_t                 *p_service_uuid;		/*!< Service uuid array point */
    uint8_t                 flag;					/*!< Advertising flag of discovery mode */
} esp_ble_adv_data_t;

/// Own BD address source of the device
typedef enum {
    /// Public Address
    ESP_PUBLIC_ADDR,
    /// Provided random address
    ESP_PROVIDED_RND_ADDR,
    /// Provided static random address
    ESP_GEN_STATIC_RND_ADDR,
    /// Generated resolvable private random address
    ESP_GEN_RSLV_ADDR,
    /// Generated non-resolvable private random address
    ESP_GEN_NON_RSLV_ADDR,
    /// Provided Reconnection address
    ESP_PROVIDED_RECON_ADDR,
} esp_ble_own_addr_src_t;

/// Ble scan type 
typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0,			/*!< Passive scan */
    BLE_SCAN_TYPE_ACTIVE    =   0x1,			/*!< Active scan */
} esp_ble_scan_type_t;

/// Ble scan filter type
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,	/*!< Accept all :
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
    esp_ble_scan_type_t     scan_type;				/*!< Scan type */
    esp_ble_addr_type_t     own_addr_type;			/*!< Owner address type */
    esp_ble_scan_filter_t   scan_filter_policy;		/*!< Scan filter policy */
    uint16_t                scan_interval;			/*!< Scan interval. This is defined as the time interval from
													  when the Controller started its last LE scan until it begins the subsequent LE scan.
													  Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
													  Time = N * 0.625 msec
													  Time Range: 2.5 msec to 10.24 seconds*/
    uint16_t                scan_window;			/*!< Scan window. The duration of the LE scan. LE_Scan_Window
													  shall be less than or equal to LE_Scan_Interval
													  Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
													  Time = N * 0.625 msec
													  Time Range: 2.5 msec to 10240 msec */
} esp_ble_scan_params_t;

/// Connection update parameters
typedef struct {
    esp_bd_addr_t bda;								/*!< Bluetooth device address */
    uint16_t min_int;								/*!< Min connection interval */
    uint16_t max_int;								/*!< Max connection interval */
    uint16_t latency;								/*!< Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3 */
    uint16_t timeout;								/*!< Supervision timeout for the LE Link. Range: 0x000A to 0x0C80.
													  Mandatory Range: 0x000A to 0x0C80 Time = N * 10 msec
													  Time Range: 100 msec to 32 seconds */
} esp_ble_conn_update_params_t;

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
	ESP_BLE_EVT_CONN_ADV         = 0x00,		/*!< Connectable undirected advertising (ADV_IND) */
	ESP_BLE_EVT_CONN_DIR_ADV     = 0x01, 		/*!< Connectable directed advertising (ADV_DIRECT_IND) */
	ESP_BLE_EVT_DISC_ADV         = 0x02,		/*!< Scannable undirected advertising (ADV_SCAN_IND) */
	ESP_BLE_EVT_NON_CONN_ADV     = 0x03,		/*!< Non connectable undirected advertising (ADV_NONCONN_IND) */
	ESP_BLE_EVT_SCAN_RSP         = 0x04,		/*!< Scan Response (SCAN_RSP) */
} esp_ble_evt_type_t;

/**
 * @brief Gap callback parameters union
 */
typedef union {
    /**
     * @brief ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT
     */
    struct ble_adv_data_cmpl_evt_param {
        esp_bt_status_t status;						/*!< Indicate the set advertising data operation success status */
    } adv_data_cmpl;								/*!< Event parameter of ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT */ 
    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT
     */
    struct ble_scan_rsp_data_cmpl_evt_param {
        esp_bt_status_t status;						/*!< Indicate the set scan response data operation success status */
    } scan_rsp_data_cmpl;							/*!< Event parameter of ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT
     */
    struct ble_scan_param_cmpl_evt_param {
        esp_bt_status_t status;						/*!< Indicate the set scan param operation success status */
    } scan_param_cmpl;								/*!< Event parameter of ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_RESULT_EVT
     */
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;			/*!< Search event type */
        esp_bd_addr_t bda;							/*!< Bluetooth device address which has been searched */
        esp_bt_dev_type_t dev_type;					/*!< Device type */
        esp_ble_addr_type_t ble_addr_type;			/*!< Ble device address type */
		esp_ble_evt_type_t ble_evt_type;			/*!< Ble scan result event type */
        int rssi;									/*!< Searched device's RSSI */
        uint8_t  ble_adv[ESP_BLE_ADV_DATA_LEN_MAX]; /*!< Received EIR */
        int flag;									/*!< Advertising data flag bit */
        int num_resps;								/*!< Scan result number */
    } scan_rst;										/*!< Event parameter of ESP_GAP_BLE_SCAN_RESULT_EVT */
} esp_ble_gap_cb_param_t;

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
esp_err_t esp_ble_gap_register_callback(esp_profile_cb_t callback);


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
 * @return          pointer of ADV data
 *
 */
uint8_t *esp_ble_resolve_adv_data(uint8_t *adv_data, uint8_t type, uint8_t *length);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BLE_API_H__ */
