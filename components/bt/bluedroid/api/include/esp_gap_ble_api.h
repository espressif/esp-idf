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

#define ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT       0
#define ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT  1
#define ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT     2
#define ESP_GAP_BLE_SCAN_RESULT_EVT                 3

#define ESP_BLE_ADV_DATA_LEN_MAX        31

/****************** define the adv type macro***************************************/
#define ESP_BLE_AD_TYPE_FLAG                            0x01
#define ESP_BLE_AD_TYPE_16SRV_PART                      0x02
#define ESP_BLE_AD_TYPE_16SRV_CMPL                      0x03
#define ESP_BLE_AD_TYPE_32SRV_PART                      0x04
#define ESP_BLE_AD_TYPE_32SRV_CMPL                      0x05
#define ESP_BLE_AD_TYPE_128SRV_PART                     0x06
#define ESP_BLE_AD_TYPE_128SRV_CMPL                     0x07
#define ESP_BLE_AD_TYPE_NAME_SHORT                      0x08
#define ESP_BLE_AD_TYPE_NAME_CMPL                       0x09
#define ESP_BLE_AD_TYPE_TX_PWR                          0x0A
#define ESP_BLE_AD_TYPE_DEV_CLASS                       0x0D
#define ESP_BLE_AD_TYPE_SM_TK                           0x10
#define ESP_BLE_AD_TYPE_SM_OOB_FLAG                     0x11
#define ESP_BLE_AD_TYPE_INT_RANGE                       0x12
#define ESP_BLE_AD_TYPE_SOL_SRV_UUID                    0x14
#define ESP_BLE_AD_TYPE_128SOL_SRV_UUID                 0x15
#define ESP_BLE_AD_TYPE_SERVICE_DATA                    0x16
#define ESP_BLE_AD_TYPE_PUBLIC_TARGET                   0x17
#define ESP_BLE_AD_TYPE_RANDOM_TARGET                   0x18
#define ESP_BLE_AD_TYPE_APPEARANCE                      0x19
#define ESP_BLE_AD_TYPE_ADV_INT                         0x1A
#define ESP_BLE_AD_TYPE_32SOL_SRV_UUID                  0x1B
#define ESP_BLE_AD_TYPE_32SERVICE_DATA                  0x1C
#define ESP_BLE_AD_TYPE_128SERVICE_DATA                 0x1D
#define ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE           0xFF


typedef uint32_t esp_gap_ble_event_t;

/// Advertising mode
typedef enum {
    ADV_TYPE_IND                = 0x00,
    ADV_TYPE_DIRECT_IND_HIGH    = 0x01,
    ADV_TYPE_SCAN_IND           = 0x02,
    ADV_TYPE_NONCONN_IND        = 0x03,
    ADV_TYPE_DIRECT_IND_LOW     = 0x04,
} esp_ble_adv_type_t;

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


typedef struct {
    uint16_t                adv_int_min;
    uint16_t                adv_int_max;
    esp_ble_adv_type_t      adv_type;
    esp_ble_addr_type_t     own_addr_type;
    esp_bd_addr_t           peer_addr;
    esp_ble_addr_type_t     peer_addr_type;
    esp_ble_adv_channel_t   channel_map;
    esp_ble_adv_filter_t    adv_filter_policy;
} esp_ble_adv_params_t;

typedef struct {
    bool                    set_scan_rsp;
    bool                    include_name;
    bool                    include_txpower;
    int                     min_interval;
    int                     max_interval;
    int                     appearance;
    uint16_t                manufacturer_len;
    uint8_t                 *p_manufacturer_data;
    uint16_t                service_data_len;
    uint8_t                 *p_service_data;
    uint16_t                service_uuid_len;
    uint8_t                 *p_service_uuid;
    uint8_t                 flag;
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


typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0,
    BLE_SCAN_TYPE_ACTIVE    =   0x1,
} esp_ble_scan_type_t;

typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST     = 0x1,
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR       = 0x2,
    BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR = 0x3,
} esp_ble_scan_filter_t;

typedef struct {
    esp_ble_scan_type_t     scan_type;
    esp_ble_addr_type_t     own_addr_type;
    esp_ble_scan_filter_t   scan_filter_policy;
    uint16_t                scan_interval;
    uint16_t                scan_window;
} esp_ble_scan_params_t;

typedef struct {
    esp_bd_addr_t bda;
    uint16_t min_int;
    uint16_t max_int;
    uint16_t latency;
    uint16_t timeout;
} esp_ble_conn_update_params_t;

typedef void (*esp_gap_ble_cb_t)(esp_gap_ble_event_t event, void *param);

typedef enum {
    /* Search callback events */
    ESP_GAP_SEARCH_INQ_RES_EVT             = 0,      /* Inquiry result for a peer device. */
    ESP_GAP_SEARCH_INQ_CMPL_EVT            = 1,      /* Inquiry complete. */
    ESP_GAP_SEARCH_DISC_RES_EVT            = 2,      /* Discovery result for a peer device. */
    ESP_GAP_SEARCH_DISC_BLE_RES_EVT        = 3,      /* Discovery result for BLE GATT based servoce on a peer device. */
    ESP_GAP_SEARCH_DISC_CMPL_EVT           = 4,      /* Discovery complete. */
    ESP_GAP_SEARCH_DI_DISC_CMPL_EVT        = 5,      /* Discovery complete. */
    ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT  = 6,      /* Search cancelled */
} esp_gap_search_evt_t;

typedef union {
    //ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT     0
    struct ble_adv_data_cmpl_evt_param {
        esp_bt_status_t status;
    } adv_data_cmpl;
    //ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT    1
    struct ble_scan_rsp_data_cmpl_evt_param {
        esp_bt_status_t status;
    } scan_rsp_data_cmpl;
    //ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT       2
    struct ble_scan_param_cmpl_evt_param {
        esp_bt_status_t status;
    } scan_param_cmpl;
    //ESP_GAP_BLE_SCAN_RESULT_EVT                   3
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;
        esp_bd_addr_t bda;
        esp_bt_dev_type_t dev_type;
        esp_ble_addr_type_t ble_addr_type;
        int rssi;
        uint8_t  ble_adv[ESP_BLE_ADV_DATA_LEN_MAX];                 /* received EIR */
        int flag;
        int num_resps;
    } scan_rst;
} esp_ble_gap_cb_param_t;

/*******************************************************************************
**
** @function        esp_ble_gap_register_callback
**
** @brief           This function is called to occur gap event, such as scan result
**
** @param[in]       callback: callback function
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/

esp_err_t esp_ble_gap_register_callback(esp_profile_cb_t callback);


/*******************************************************************************
**
** @function        esp_ble_gap_config_adv_data
**
** @brief           This function is called to override the BTA default ADV parameters.
**
** @param[in]       adv_data: Pointer to User defined ADV data structure. This
**                  memory space can not be freed until p_adv_data_cback
**                  is received.
** @param[in|out]   adv_data_cback: set adv data complete callback.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_config_adv_data (esp_ble_adv_data_t *adv_data);



/*******************************************************************************
**
** @function        esp_ble_gap_set_scan_params
**
** @brief           This function is called to set scan parameters
**
** @param[in]       esp_ble_scan_params: Pointer to User defined scan_params data structure. This
**                  memory space can not be freed until scan_param_setup_cback
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);


/*******************************************************************************
**
** @function        esp_ble_gap_start_scanning
**
** @brief           This procedure keep the device scanning the peer device whith advertising on the air
**
** @param[in]       duration: Keeping the scaning time, the unit is second.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);


/*******************************************************************************
**
** @function       esp_ble_gap_stop_scanning
**
** @brief            This function call to stop the device scanning the peer device whith advertising on the air
** @param        void
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_stop_scanning(void);

/*******************************************************************************
**
** @function        esp_ble_gap_start_advertising
**
** @brief           This function is called to start advertising.
**
** @param[in]       esp_ble_adv_params_all_t: ointer to User defined adv_params data structure.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_start_advertising (esp_ble_adv_params_t *adv_params);



/*******************************************************************************
**
** @function        esp_gap_ble_stop_advertising
**
** @brief           This function is called to stop advertising.
**
** @param           None
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_stop_advertising(void);



/*******************************************************************************
**
** @function         esp_ble_update_conn_params
**
** @brief           Update connection parameters, can only be used when connection is up.
**
** @param[in]       param   -  connection update params
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params);


/*******************************************************************************
**
** @function        esp_ble_gap_set_pkt_data_len
**
** @brief           This function is to set maximum LE data packet size
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length);



/*******************************************************************************
**
** @function        esp_ble_gap_set_rand_addr
**
** @brief           This function set the random address for the appliction
**
** @param[in]       rand_addr: the random address whith should be setting
**
** @return          ESP_OK - success, other - failed
**
**
*******************************************************************************/
esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr);



/*******************************************************************************
**
** @function        esp_ble_gap_config_local_privacy
**
** @brief           Enable/disable privacy on the local device
**
** @param[in]       privacy_enable   - enable/disabe privacy on remote device.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_config_local_privacy (bool privacy_enable);


/*******************************************************************************
**
** @function        esp_ble_gap_set_device_name
**
** @brief           Set device name to the local device
**
** @param[in]       name   -  device name.
**
** @return          ESP_OK - success, other - failed
**
*******************************************************************************/
esp_err_t esp_ble_gap_set_device_name(char *name);


/*******************************************************************************
**
** @function        esp_ble_resolve_adv_data
**
** @brief             This function is called to get ADV data for a specific type.
**
** @param[in]       p_adv - pointer of ADV data whitch to be resolved
** @param[in]       type   - finding ADV data type
** @param[out]     p_length - return the length of ADV data not including type
**
** @return               pointer of ADV data
**
*******************************************************************************/
uint8_t *esp_ble_resolve_adv_data( uint8_t *p_adv, uint8_t type, uint8_t *p_length );

#endif /* __ESP_GAP_BLE_API_H__ */
