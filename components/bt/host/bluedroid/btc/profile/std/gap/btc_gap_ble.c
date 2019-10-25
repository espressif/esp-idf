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

#include <string.h>

#include "osi/allocator.h"
#include "stack/bt_types.h"
#include "common/bt_defs.h"
#include "bta/bta_api.h"
#include "bta/bta_dm_co.h"
#include "btc/btc_task.h"
#include "btc/btc_manage.h"
#include "btc_gap_ble.h"
#include "btc_gatt_util.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "btc/btc_ble_storage.h"
#include "btc/btc_dm.h"
#include "btc/btc_util.h"
#include "osi/mutex.h"
#include "esp_bt.h"

#if (BLE_INCLUDED == TRUE)
#if BTC_DYNAMIC_MEMORY == FALSE
static tBTA_BLE_ADV_DATA gl_bta_adv_data;
static tBTA_BLE_ADV_DATA gl_bta_scan_rsp_data;
#else
tBTA_BLE_ADV_DATA *gl_bta_adv_data_ptr;
tBTA_BLE_ADV_DATA *gl_bta_scan_rsp_data_ptr;
#endif

#if SCAN_QUEUE_CONGEST_CHECK
static list_t *adv_filter_list;
static osi_mutex_t adv_list_lock;
bool btc_check_adv_list(uint8_t * addr, uint8_t addr_type);
uint32_t btc_get_adv_list_length(void);
void btc_adv_list_refresh(void);
void btc_adv_list_lock(void);
void btc_adv_list_unlock(void);
static uint16_t btc_adv_list_count = 0;

#define  BTC_ADV_LIST_MAX_LENGTH    50
#define  BTC_ADV_LIST_MAX_COUNT     200
#endif

static inline void btc_gap_ble_cb_to_app(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_gap_ble_cb_t btc_gap_ble_cb = (esp_gap_ble_cb_t)btc_profile_cb_get(BTC_PID_GAP_BLE);
    if (btc_gap_ble_cb) {
        btc_gap_ble_cb(event, param);
    }
}

static void btc_gap_adv_point_cleanup(void **buf)
{
    if (NULL == *buf) {
        return;
    }
    osi_free(*buf);
    *buf = NULL;
}


static void btc_cleanup_adv_data(tBTA_BLE_ADV_DATA *bta_adv_data)
{
    if (bta_adv_data == NULL) {
        return;
    }

    // Manufacturer data cleanup
    if (bta_adv_data->p_manu != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_manu->p_val);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_manu);
    }

    // Proprietary data cleanup
    if (bta_adv_data->p_proprietary != NULL) {
        int i = 0;
        tBTA_BLE_PROP_ELEM *p_elem = bta_adv_data->p_proprietary->p_elem;
        while (i++ != bta_adv_data->p_proprietary->num_elem
                && p_elem) {
            btc_gap_adv_point_cleanup((void **) &p_elem->p_val);
            ++p_elem;
        }

        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_proprietary->p_elem);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_proprietary);
    }

    // Service list cleanup
    if (bta_adv_data->p_services != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_services->p_uuid);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_services);
    }

    // Service data cleanup
    if (bta_adv_data->p_service_data != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_service_data->p_val);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_service_data);
    }

    btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_services_128b);

    if (bta_adv_data->p_service_32b != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_service_32b->p_uuid);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_service_32b);
    }

    if (bta_adv_data->p_sol_services != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_sol_services->p_uuid);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_sol_services);
    }

    if (bta_adv_data->p_sol_service_32b != NULL) {
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_sol_service_32b->p_uuid);
        btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_sol_service_32b);
    }

    btc_gap_adv_point_cleanup((void **) &bta_adv_data->p_sol_service_128b);
}

static void btc_to_bta_adv_data(esp_ble_adv_data_t *p_adv_data, tBTA_BLE_ADV_DATA *bta_adv_data, uint32_t *data_mask)
{
    uint32_t mask;

    btc_cleanup_adv_data(bta_adv_data);

    memset(bta_adv_data, 0, sizeof(tBTA_BLE_ADV_DATA));
    mask = 0;

    if (p_adv_data->flag != 0) {
        mask = BTM_BLE_AD_BIT_FLAGS;
        bta_adv_data->flag = p_adv_data->flag;
    }

    if (p_adv_data->include_name) {
        mask |= BTM_BLE_AD_BIT_DEV_NAME;
    }

    if (p_adv_data->include_txpower) {
        mask |= BTM_BLE_AD_BIT_TX_PWR;
        bta_adv_data->tx_power = esp_ble_tx_power_get(ESP_BLE_PWR_TYPE_ADV);
    }

    if (p_adv_data->min_interval > 0 && p_adv_data->max_interval > 0 &&
            p_adv_data->max_interval >= p_adv_data->min_interval) {
        mask |= BTM_BLE_AD_BIT_INT_RANGE;
        bta_adv_data->int_range.low = p_adv_data->min_interval;
        bta_adv_data->int_range.hi = p_adv_data->max_interval;
    }

    if (p_adv_data->include_txpower) {
        //TODO
    }

    if (p_adv_data->appearance != 0) {
        mask |= BTM_BLE_AD_BIT_APPEARANCE;
        bta_adv_data->appearance = p_adv_data->appearance;
    }

    if (p_adv_data->manufacturer_len > 0 && p_adv_data->p_manufacturer_data != NULL) {
        bta_adv_data->p_manu = osi_malloc(sizeof(tBTA_BLE_MANU));
        if (bta_adv_data->p_manu != NULL) {
            bta_adv_data->p_manu->p_val = osi_malloc(p_adv_data->manufacturer_len);
            if (bta_adv_data->p_manu->p_val != NULL) {
                mask |= BTM_BLE_AD_BIT_MANU;
                bta_adv_data->p_manu->len = p_adv_data->manufacturer_len;
                memcpy(bta_adv_data->p_manu->p_val, p_adv_data->p_manufacturer_data, p_adv_data->manufacturer_len);
            }
        }
    }

    tBTA_BLE_PROP_ELEM *p_elem_service_data = NULL;
    if (p_adv_data->service_data_len > 0 && p_adv_data->p_service_data != NULL) {
        p_elem_service_data = osi_malloc(sizeof(tBTA_BLE_PROP_ELEM));
        if (p_elem_service_data != NULL) {
            p_elem_service_data->p_val = osi_malloc(p_adv_data->service_data_len);
            if (p_elem_service_data->p_val != NULL) {
                p_elem_service_data->adv_type = BTM_BLE_AD_TYPE_SERVICE_DATA;
                p_elem_service_data->len = p_adv_data->service_data_len;
                memcpy(p_elem_service_data->p_val, p_adv_data->p_service_data,
                       p_adv_data->service_data_len);
            } else {
                osi_free(p_elem_service_data);
                p_elem_service_data = NULL;
            }
        }
    }

    if (NULL != p_elem_service_data) {
        bta_adv_data->p_proprietary = osi_malloc(sizeof(tBTA_BLE_PROPRIETARY));
        if (NULL != bta_adv_data->p_proprietary) {
            tBTA_BLE_PROP_ELEM *p_elem = NULL;
            tBTA_BLE_PROPRIETARY *p_prop = bta_adv_data->p_proprietary;
            p_prop->num_elem = 0;
            mask |= BTM_BLE_AD_BIT_PROPRIETARY;
            p_prop->num_elem = 1;
            p_prop->p_elem = osi_malloc(sizeof(tBTA_BLE_PROP_ELEM) * p_prop->num_elem);
            p_elem = p_prop->p_elem;
            if (NULL != p_elem) {
                memcpy(p_elem++, p_elem_service_data, sizeof(tBTA_BLE_PROP_ELEM));
            }
            osi_free(p_elem_service_data);
        }
    }

    if (p_adv_data->service_uuid_len && p_adv_data->p_service_uuid) {
        UINT16 *p_uuid_out16 = NULL;
        UINT32 *p_uuid_out32 = NULL;
        for (int position = 0; position < p_adv_data->service_uuid_len; position += LEN_UUID_128) {
            tBT_UUID bt_uuid;

            btc128_to_bta_uuid(&bt_uuid, p_adv_data->p_service_uuid + position);

            switch (bt_uuid.len) {
            case (LEN_UUID_16): {
                if (NULL == bta_adv_data->p_services) {
                    bta_adv_data->p_services = osi_malloc(sizeof(tBTA_BLE_SERVICE));
                    bta_adv_data->p_services->list_cmpl = FALSE;
                    bta_adv_data->p_services->num_service = 0;
                    bta_adv_data->p_services->p_uuid = osi_malloc(p_adv_data->service_uuid_len / LEN_UUID_128 * LEN_UUID_16);
                    p_uuid_out16 = bta_adv_data->p_services->p_uuid;
                }

                if (NULL != bta_adv_data->p_services->p_uuid) {
                    BTC_TRACE_DEBUG("%s - In 16-UUID_data", __FUNCTION__);
                    mask |= BTM_BLE_AD_BIT_SERVICE;
                    ++bta_adv_data->p_services->num_service;
                    *p_uuid_out16++ = bt_uuid.uu.uuid16;
                }
                break;
            }

            case (LEN_UUID_32): {
                if (NULL == bta_adv_data->p_service_32b) {
                    bta_adv_data->p_service_32b =
                        osi_malloc(sizeof(tBTA_BLE_32SERVICE));
                    bta_adv_data->p_service_32b->list_cmpl = FALSE;
                    bta_adv_data->p_service_32b->num_service = 0;
                    bta_adv_data->p_service_32b->p_uuid =
                        osi_malloc(p_adv_data->service_uuid_len / LEN_UUID_128 * LEN_UUID_32);
                    p_uuid_out32 = bta_adv_data->p_service_32b->p_uuid;
                }

                if (NULL != bta_adv_data->p_service_32b->p_uuid) {
                    BTC_TRACE_DEBUG("%s - In 32-UUID_data", __FUNCTION__);
                    mask |= BTM_BLE_AD_BIT_SERVICE_32;
                    ++bta_adv_data->p_service_32b->num_service;
                    *p_uuid_out32++ = bt_uuid.uu.uuid32;
                }
                break;
            }

            case (LEN_UUID_128): {
                /* Currently, only one 128-bit UUID is supported */
                if (NULL == bta_adv_data->p_services_128b) {
                    bta_adv_data->p_services_128b =
                        osi_malloc(sizeof(tBTA_BLE_128SERVICE));
                    if (NULL != bta_adv_data->p_services_128b) {
                        BTC_TRACE_DEBUG("%s - In 128-UUID_data", __FUNCTION__);
                        mask |= BTM_BLE_AD_BIT_SERVICE_128;
                        memcpy(bta_adv_data->p_services_128b->uuid128,
                               bt_uuid.uu.uuid128, LEN_UUID_128);
                        BTC_TRACE_DEBUG("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", bt_uuid.uu.uuid128[0],
                                  bt_uuid.uu.uuid128[1], bt_uuid.uu.uuid128[2], bt_uuid.uu.uuid128[3],
                                  bt_uuid.uu.uuid128[4], bt_uuid.uu.uuid128[5], bt_uuid.uu.uuid128[6],
                                  bt_uuid.uu.uuid128[7], bt_uuid.uu.uuid128[8], bt_uuid.uu.uuid128[9],
                                  bt_uuid.uu.uuid128[10], bt_uuid.uu.uuid128[11], bt_uuid.uu.uuid128[12],
                                  bt_uuid.uu.uuid128[13], bt_uuid.uu.uuid128[14], bt_uuid.uu.uuid128[15]);
                        bta_adv_data->p_services_128b->list_cmpl = TRUE;
                    }
                }
                break;
            }

            default:
                break;
            }
        }
    }

    *data_mask = mask;
}

static void btc_adv_data_callback(tBTA_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT;
    param.adv_data_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_scan_rsp_data_callback(tBTA_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT;
    param.scan_rsp_data_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_adv_data_raw_callback(tBTA_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT;
    param.adv_data_raw_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_scan_rsp_data_raw_callback(tBTA_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT;
    param.scan_rsp_data_raw_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_ble_set_adv_data(esp_ble_adv_data_t *adv_data,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK p_adv_data_cback)
{
    tBTA_BLE_AD_MASK data_mask = 0;

    if (!adv_data->set_scan_rsp) {
    	btc_to_bta_adv_data(adv_data, &gl_bta_adv_data, &data_mask);
        BTA_DmBleSetAdvConfig(data_mask, &gl_bta_adv_data, p_adv_data_cback);
    } else {
    	btc_to_bta_adv_data(adv_data, &gl_bta_scan_rsp_data, &data_mask);
        BTA_DmBleSetScanRsp(data_mask, &gl_bta_scan_rsp_data, p_adv_data_cback);
    }
}

static void btc_ble_set_adv_data_raw(uint8_t *raw_adv, uint32_t raw_adv_len,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK p_adv_data_cback)
{
    BTA_DmBleSetAdvConfigRaw(raw_adv, raw_adv_len, p_adv_data_cback);
}

static void btc_ble_set_scan_rsp_data_raw(uint8_t *raw_scan_rsp, uint32_t raw_scan_rsp_len,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK p_scan_rsp_data_cback)
{
    BTA_DmBleSetScanRspRaw(raw_scan_rsp, raw_scan_rsp_len, p_scan_rsp_data_cback);
}

static void btc_start_adv_callback(uint8_t status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_ADV_START_COMPLETE_EVT;
    param.adv_start_cmpl.status = btc_hci_to_esp_status(status);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_stop_adv_callback(uint8_t status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT;
    param.adv_stop_cmpl.status = btc_hci_to_esp_status(status);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_update_duplicate_exceptional_list_callback(tBTA_STATUS status, uint8_t subcode, uint32_t length, uint8_t *device_info)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT;
    param.update_duplicate_exceptional_list_cmpl.status = status;
    param.update_duplicate_exceptional_list_cmpl.subcode = subcode;
    if(length > sizeof(param.update_duplicate_exceptional_list_cmpl.device_info)) {
        length = sizeof(param.update_duplicate_exceptional_list_cmpl.device_info);
    }
    param.update_duplicate_exceptional_list_cmpl.length = length;
    memcpy(param.update_duplicate_exceptional_list_cmpl.device_info, device_info, length);
    ret = btc_transfer_context(&msg, &param, sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_ble_update_duplicate_exceptional_list(uint8_t subcode, uint32_t info_type, BD_ADDR device_info,
                                 tBTA_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_CMPL_CBACK p_update_duplicate_ignore_list_cback)
{
    BTA_DmUpdateDuplicateExceptionalList(subcode, info_type, device_info, p_update_duplicate_ignore_list_cback);
}

static void btc_ble_start_advertising (esp_ble_adv_params_t *ble_adv_params, tBTA_START_ADV_CMPL_CBACK start_adv_cback)
{
    tBLE_BD_ADDR peer_addr;
    esp_bt_status_t status = ESP_BT_STATUS_SUCCESS;
    if (!BLE_ISVALID_PARAM(ble_adv_params->adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
            !BLE_ISVALID_PARAM(ble_adv_params->adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX)) {
        status = ESP_BT_STATUS_PARM_INVALID;
        BTC_TRACE_ERROR("Invalid advertisting interval parameters.\n");
    }

    if ((ble_adv_params->adv_type < ADV_TYPE_IND) ||
            (ble_adv_params->adv_type > ADV_TYPE_DIRECT_IND_LOW) ) {
        status = ESP_BT_STATUS_PARM_INVALID;
        BTC_TRACE_ERROR("Invalid advertisting type parameters.\n");
    }

    if ((ble_adv_params->adv_filter_policy < ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY) ||
            (ble_adv_params->adv_filter_policy > ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST) ) {
        status = ESP_BT_STATUS_PARM_INVALID;
        BTC_TRACE_ERROR("Invalid advertisting type parameters.\n");
    }

    if((ble_adv_params->channel_map | ADV_CHNL_ALL) != ADV_CHNL_ALL || ble_adv_params->channel_map == 0) {
        status = ESP_BT_STATUS_PARM_INVALID;
        BTC_TRACE_ERROR("Invalid advertisting channel map parameters.\n");
    }
    if (!BLE_ISVALID_PARAM(ble_adv_params->peer_addr_type, BLE_ADDR_TYPE_PUBLIC, BLE_ADDR_TYPE_RANDOM)) {
        status = ESP_BT_STATUS_PARM_INVALID;
        BTC_TRACE_ERROR("Invalid advertisting peer address type parameters.\n");
    }
    if(status != ESP_BT_STATUS_SUCCESS) {
        if(start_adv_cback) {
            start_adv_cback(status);
        }
        return;
    }

    BTC_TRACE_DEBUG("API_Ble_AppStartAdvertising\n");

    memcpy(peer_addr.bda, ble_adv_params->peer_addr, ESP_BD_ADDR_LEN);
    peer_addr.type = ble_adv_params->peer_addr_type;
    BTA_DmSetBleAdvParamsAll(ble_adv_params->adv_int_min,
                             ble_adv_params->adv_int_max,
                             ble_adv_params->adv_type,
                             ble_adv_params->own_addr_type,
                             ble_adv_params->channel_map,
                             ble_adv_params->adv_filter_policy,
                             &peer_addr,
                             start_adv_cback);
}


static void btc_scan_params_callback(tGATT_IF gatt_if, tBTM_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT;
    param.scan_param_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_ble_set_scan_params(esp_ble_scan_params_t *scan_params, tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback)
{
    if (BLE_ISVALID_PARAM(scan_params->scan_interval, BTM_BLE_SCAN_INT_MIN, BTM_BLE_SCAN_INT_MAX) &&
        BLE_ISVALID_PARAM(scan_params->scan_window, BTM_BLE_SCAN_WIN_MIN, BTM_BLE_SCAN_WIN_MAX) &&
        BLE_ISVALID_PARAM(scan_params->own_addr_type, BLE_ADDR_TYPE_PUBLIC, BLE_ADDR_TYPE_RPA_RANDOM) &&
        BLE_ISVALID_PARAM(scan_params->scan_filter_policy, BLE_SCAN_FILTER_ALLOW_ALL, BLE_SCAN_FILTER_ALLOW_WLIST_PRA_DIR) &&
        BLE_ISVALID_PARAM(scan_params->scan_duplicate, BLE_SCAN_DUPLICATE_DISABLE, BLE_SCAN_DUPLICATE_MAX -1) &&
        (scan_params->scan_type == BTM_BLE_SCAN_MODE_ACTI || scan_params->scan_type == BTM_BLE_SCAN_MODE_PASS)) {
        BTA_DmSetBleScanFilterParams(ESP_DEFAULT_GATT_IF,	/*client_if*/
                                     scan_params->scan_interval,
                                     scan_params->scan_window,
                                     scan_params->scan_type,
                                     scan_params->scan_filter_policy,
                                     scan_params->own_addr_type,
                                     scan_params->scan_duplicate,
                                     scan_param_setup_cback);
    } else {
        btc_scan_params_callback(ESP_DEFAULT_GATT_IF, BTM_ILLEGAL_VALUE);
    }
}

static void btc_search_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    esp_ble_gap_cb_param_t param;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_RESULT_EVT;

    param.scan_rst.search_evt = event;
    switch (event) {
    case BTA_DM_INQ_RES_EVT: {
#if SCAN_QUEUE_CONGEST_CHECK
        if(btc_check_queue_is_congest()) {
            BTC_TRACE_DEBUG("BtcQueue is congested");
            if(btc_get_adv_list_length() > BTC_ADV_LIST_MAX_LENGTH || btc_adv_list_count > BTC_ADV_LIST_MAX_COUNT) {
                btc_adv_list_refresh();
                btc_adv_list_count = 0;
            }
            if(btc_check_adv_list(p_data->inq_res.bd_addr, p_data->inq_res.ble_addr_type)) {
                return;
            }
        }
        btc_adv_list_count ++;
#endif
        bdcpy(param.scan_rst.bda, p_data->inq_res.bd_addr);
        param.scan_rst.dev_type = p_data->inq_res.device_type;
        param.scan_rst.rssi = p_data->inq_res.rssi;
        param.scan_rst.ble_addr_type = p_data->inq_res.ble_addr_type;
        param.scan_rst.ble_evt_type = p_data->inq_res.ble_evt_type;
        param.scan_rst.flag = p_data->inq_res.flag;
        param.scan_rst.num_resps = 1;
        param.scan_rst.adv_data_len = p_data->inq_res.adv_data_len;
        param.scan_rst.scan_rsp_len = p_data->inq_res.scan_rsp_len;
        memcpy(param.scan_rst.ble_adv, p_data->inq_res.p_eir, sizeof(param.scan_rst.ble_adv));
        break;
    }
    case BTA_DM_INQ_CMPL_EVT: {
        param.scan_rst.num_resps = p_data->inq_cmpl.num_resps;
        BTC_TRACE_DEBUG("%s  BLE observe complete. Num Resp %d\n", __FUNCTION__, p_data->inq_cmpl.num_resps);
        break;
    }
    case BTA_DM_DISC_RES_EVT:
        BTC_TRACE_DEBUG("BTA_DM_DISC_RES_EVT\n");
        break;
    case BTA_DM_DISC_BLE_RES_EVT:
        BTC_TRACE_DEBUG("BTA_DM_DISC_BLE_RES_EVT\n");
        break;
    case BTA_DM_DISC_CMPL_EVT:
        BTC_TRACE_DEBUG("BTA_DM_DISC_CMPL_EVT\n");
        break;
    case BTA_DM_DI_DISC_CMPL_EVT:
        BTC_TRACE_DEBUG("BTA_DM_DI_DISC_CMPL_EVT\n");
        break;
    case BTA_DM_SEARCH_CANCEL_CMPL_EVT:
        BTC_TRACE_DEBUG("BTA_DM_SEARCH_CANCEL_CMPL_EVT\n");
        break;
    case BTA_DM_INQ_DISCARD_NUM_EVT:
        param.scan_rst.num_dis = p_data->inq_dis.num_dis;
        break;
    default:
        BTC_TRACE_ERROR("%s : Unknown event 0x%x\n", __FUNCTION__, event);
        return;
    }
    btc_transfer_context(&msg, &param, sizeof(esp_ble_gap_cb_param_t), NULL);
}

static void btc_start_scan_callback(uint8_t status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_START_COMPLETE_EVT;
    param.scan_start_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_stop_scan_callback(tBTA_STATUS status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT;
    param.scan_stop_cmpl.status = status;

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
#if SCAN_QUEUE_CONGEST_CHECK
    btc_adv_list_refresh();
#endif
}

void btc_update_conn_param_callback (UINT8 status, BD_ADDR bd_addr, tBTM_LE_UPDATE_CONN_PRAMS *update_conn_params)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT;
    param.update_conn_params.status = btc_hci_to_esp_status(status);
    param.update_conn_params.min_int = update_conn_params->min_conn_int;
    param.update_conn_params.max_int = update_conn_params->max_conn_int;
    param.update_conn_params.conn_int = update_conn_params->conn_int;
    param.update_conn_params.latency = update_conn_params->slave_latency;
    param.update_conn_params.timeout = update_conn_params->supervision_tout;
    memcpy(param.update_conn_params.bda, bd_addr, sizeof(esp_bd_addr_t));
    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_set_pkt_length_callback(UINT8 status, tBTM_LE_SET_PKT_DATA_LENGTH_PARAMS *data_len_params)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT;
    param.pkt_data_lenth_cmpl.status = btc_btm_status_to_esp_status(status);
    param.pkt_data_lenth_cmpl.params.rx_len = data_len_params->rx_len;
    param.pkt_data_lenth_cmpl.params.tx_len = data_len_params->tx_len;
    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_gap_ble_set_channels_cmpl_callback(void *p_data)
{
    tBTA_BLE_SET_CHANNELS_RESULTS *result = (tBTA_BLE_SET_CHANNELS_RESULTS *)p_data;
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SET_CHANNELS_EVT;

    param.ble_set_channels.stat = btc_btm_status_to_esp_status(result->status);

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}


static void btc_add_whitelist_complete_callback(UINT8 status, tBTM_WL_OPERATION wl_opration)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT;
    param.update_whitelist_cmpl.status = btc_hci_to_esp_status(status);
    param.update_whitelist_cmpl.wl_opration = wl_opration;
    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_set_rand_addr_callback(UINT8 status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    param.set_rand_addr_cmpl.status = btc_btm_status_to_esp_status(status); //todo status
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT;
    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }

}

static void btc_set_local_privacy_callback(UINT8 status)
{
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT;
    param.local_privacy_cmpl.status = btc_btm_status_to_esp_status(status);
    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}


#if (SMP_INCLUDED == TRUE)
static void btc_set_encryption_callback(BD_ADDR bd_addr, tBTA_TRANSPORT transport, tBTA_STATUS enc_status)
{
    UNUSED(bd_addr);
    UNUSED(transport);
    BTC_TRACE_DEBUG("enc_status = %x\n", enc_status);
    return;
}
#endif  ///SMP_INCLUDED == TRUE

static void btc_read_ble_rssi_cmpl_callback(void *p_data)
{
    tBTA_RSSI_RESULTS *result = (tBTA_RSSI_RESULTS *)p_data;
    esp_ble_gap_cb_param_t param;
    bt_status_t ret;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT;
    param.read_rssi_cmpl.rssi = result->rssi;
    param.read_rssi_cmpl.status = btc_btm_status_to_esp_status(result->status);
    memcpy(param.read_rssi_cmpl.remote_addr, result->rem_bda, sizeof(BD_ADDR));

    ret = btc_transfer_context(&msg, &param,
                               sizeof(esp_ble_gap_cb_param_t), NULL);

    if (ret != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

void btc_get_whitelist_size(uint16_t *length)
{
    BTM_BleGetWhiteListSize(length);
    return;
}

static void btc_ble_start_scanning(uint32_t duration,
                                   tBTA_DM_SEARCH_CBACK *results_cb,
                                   tBTA_START_STOP_SCAN_CMPL_CBACK *start_scan_cb)
{
    if ((results_cb != NULL) && (start_scan_cb != NULL)) {
#if SCAN_QUEUE_CONGEST_CHECK
        btc_adv_list_refresh();
#endif
        //Start scan the device
        BTA_DmBleScan(true, duration, results_cb, start_scan_cb);
    } else {
        BTC_TRACE_ERROR("The start_scan_cb or results_cb invalid\n");
    }
}

static void btc_ble_stop_scanning(tBTA_START_STOP_SCAN_CMPL_CBACK *stop_scan_cb)
{
    uint8_t duration = 0;
    BTA_DmBleScan(false, duration, NULL, stop_scan_cb);
}


static void btc_ble_stop_advertising(tBTA_START_STOP_ADV_CMPL_CBACK *stop_adv_cb)
{
    bool stop_adv = false;

    BTA_DmBleBroadcast(stop_adv, stop_adv_cb);
}

static void btc_ble_update_conn_params(BD_ADDR bd_addr, uint16_t min_int,
                                       uint16_t max_int, uint16_t latency, uint16_t timeout)
{
    if (min_int > max_int) {
        min_int = max_int;
    }

    if (min_int < BTM_BLE_CONN_INT_MIN || max_int > BTM_BLE_CONN_INT_MAX) {
        BTC_TRACE_ERROR("Invalid interval value.\n");
    }

    BTA_DmBleUpdateConnectionParams(bd_addr, min_int, max_int,
                                    latency, timeout);
}

static void btc_ble_set_pkt_data_len(BD_ADDR remote_device, uint16_t tx_data_length, tBTA_SET_PKT_DATA_LENGTH_CBACK *p_set_pkt_data_cback)
{
    if (tx_data_length > BTM_BLE_DATA_SIZE_MAX) {
        tx_data_length =  BTM_BLE_DATA_SIZE_MAX;
    } else if (tx_data_length < BTM_BLE_DATA_SIZE_MIN) {
        tx_data_length =  BTM_BLE_DATA_SIZE_MIN;
    }

    BTA_DmBleSetDataLength(remote_device, tx_data_length, p_set_pkt_data_cback);
}

static void btc_ble_config_local_icon(uint16_t icon)
{
    BTA_DmBleConfigLocalIcon(icon);
}

static void btc_ble_set_rand_addr (BD_ADDR rand_addr, tBTA_SET_RAND_ADDR_CBACK *p_set_rand_addr_cback)
{
    if (rand_addr != NULL) {
        /*
        A static address is a 48-bit randomly generated address and shall meet the following requirements:
        • The two most significant bits of the address shall be equal to 1
        • All bits of the random part of the address shall not be equal to 1
        • All bits of the random part of the address shall not be equal to 0
        */
        BD_ADDR invalid_rand_addr_a, invalid_rand_addr_b;
        memset(invalid_rand_addr_a, 0xff, sizeof(BD_ADDR));
        memset(invalid_rand_addr_b, 0x00, sizeof(BD_ADDR));
        invalid_rand_addr_b[0] = invalid_rand_addr_b[0] | BT_STATIC_RAND_ADDR_MASK;
        if((rand_addr[0] & BT_STATIC_RAND_ADDR_MASK) == BT_STATIC_RAND_ADDR_MASK
            && memcmp(invalid_rand_addr_a, rand_addr, BD_ADDR_LEN) != 0
            && memcmp(invalid_rand_addr_b, rand_addr, BD_ADDR_LEN) != 0){
            BTA_DmSetRandAddress(rand_addr, btc_set_rand_addr_callback);
        } else {
            btc_set_rand_addr_callback(BTM_INVALID_STATIC_RAND_ADDR);
            BTC_TRACE_ERROR("Invalid random address, the high bit should be 0b11, bits of the random part shall not be all 1 or 0");
        }
    } else {
        btc_set_rand_addr_callback(BTM_INVALID_STATIC_RAND_ADDR);
        BTC_TRACE_ERROR("Invalid random addressm, the address value is NULL");
    }
}

static void btc_ble_clear_rand_addr (void)
{
    BTA_DmClearRandAddress();
}

static void btc_ble_config_local_privacy(bool privacy_enable, tBTA_SET_LOCAL_PRIVACY_CBACK *set_local_privacy_cback)
{
    BTA_DmBleConfigLocalPrivacy(privacy_enable, set_local_privacy_cback);
}

static void btc_ble_disconnect(BD_ADDR bd_addr)
{
    BTA_DmBleDisconnect(bd_addr);
}

static void btc_gap_ble_set_channels(esp_gap_ble_channels channels)
{
    BTA_DmBleSetChannels(channels, btc_gap_ble_set_channels_cmpl_callback);
}


void btc_gap_ble_cb_handler(btc_msg_t *msg)
{
    esp_ble_gap_cb_param_t *param = (esp_ble_gap_cb_param_t *)msg->arg;

    if (msg->act < ESP_GAP_BLE_EVT_MAX) {
        btc_gap_ble_cb_to_app(msg->act, param);
    } else {
        BTC_TRACE_ERROR("%s, unknow msg->act = %d", __func__, msg->act);
    }

    btc_gap_ble_cb_deep_free(msg);

}

void btc_gap_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t  *dst = (btc_ble_gap_args_t *) p_dest;

        if (src->cfg_adv_data.adv_data.p_manufacturer_data) {
            dst->cfg_adv_data.adv_data.p_manufacturer_data = osi_malloc(src->cfg_adv_data.adv_data.manufacturer_len);
            memcpy(dst->cfg_adv_data.adv_data.p_manufacturer_data, src->cfg_adv_data.adv_data.p_manufacturer_data,
                   src->cfg_adv_data.adv_data.manufacturer_len);
        }

        if (src->cfg_adv_data.adv_data.p_service_data) {
            dst->cfg_adv_data.adv_data.p_service_data = osi_malloc(src->cfg_adv_data.adv_data.service_data_len);
            memcpy(dst->cfg_adv_data.adv_data.p_service_data, src->cfg_adv_data.adv_data.p_service_data, src->cfg_adv_data.adv_data.service_data_len);
        }

        if (src->cfg_adv_data.adv_data.p_service_uuid) {
            dst->cfg_adv_data.adv_data.p_service_uuid = osi_malloc(src->cfg_adv_data.adv_data.service_uuid_len);
            memcpy(dst->cfg_adv_data.adv_data.p_service_uuid, src->cfg_adv_data.adv_data.p_service_uuid, src->cfg_adv_data.adv_data.service_uuid_len);
        }
        break;
    }
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t *dst = (btc_ble_gap_args_t *) p_dest;

        if (src && src->cfg_adv_data_raw.raw_adv && src->cfg_adv_data_raw.raw_adv_len > 0) {
            dst->cfg_adv_data_raw.raw_adv = osi_malloc(src->cfg_adv_data_raw.raw_adv_len);
            if (dst->cfg_adv_data_raw.raw_adv) {
                memcpy(dst->cfg_adv_data_raw.raw_adv, src->cfg_adv_data_raw.raw_adv, src->cfg_adv_data_raw.raw_adv_len);
            }
        }
        break;
    }
    case BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t *dst = (btc_ble_gap_args_t *) p_dest;

        if (src && src->cfg_scan_rsp_data_raw.raw_scan_rsp && src->cfg_scan_rsp_data_raw.raw_scan_rsp_len > 0) {
            dst->cfg_scan_rsp_data_raw.raw_scan_rsp = osi_malloc(src->cfg_scan_rsp_data_raw.raw_scan_rsp_len);
            if (dst->cfg_scan_rsp_data_raw.raw_scan_rsp) {
                memcpy(dst->cfg_scan_rsp_data_raw.raw_scan_rsp, src->cfg_scan_rsp_data_raw.raw_scan_rsp, src->cfg_scan_rsp_data_raw.raw_scan_rsp_len);
            }
        }
          break;
       }
    case BTC_GAP_BLE_SET_SECURITY_PARAM_EVT: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t  *dst = (btc_ble_gap_args_t *) p_dest;
        uint8_t length = 0;
        if (src->set_security_param.value) {
            length = dst->set_security_param.len;
            dst->set_security_param.value = osi_malloc(length);
            if (dst->set_security_param.value != NULL) {
                memcpy(dst->set_security_param.value, src->set_security_param.value, length);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
    case BTC_GAP_BLE_OOB_REQ_REPLY_EVT: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t  *dst = (btc_ble_gap_args_t *) p_dest;
        uint8_t length = 0;
        if (src->oob_req_reply.p_value) {
            length = dst->oob_req_reply.len;
            dst->oob_req_reply.p_value = osi_malloc(length);
            if (dst->oob_req_reply.p_value != NULL) {
                memcpy(dst->oob_req_reply.p_value, src->oob_req_reply.p_value, length);
            } else {
                BTC_TRACE_ERROR("%s %d no mem\n",__func__, msg->act);
            }
        }
        break;
    }
    default:
        BTC_TRACE_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

void btc_gap_ble_cb_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    default:
       BTC_TRACE_ERROR("%s, Unhandled deep copy %d\n", __func__, msg->act);
       break;
    }
}

void btc_gap_ble_arg_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s \n", __func__);
    switch (msg->act) {
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA: {
        esp_ble_adv_data_t *adv = &((btc_ble_gap_args_t *)msg->arg)->cfg_adv_data.adv_data;
        if (adv->p_service_data) {
            osi_free(adv->p_service_data);
        }

        if (adv->p_service_uuid) {
            osi_free(adv->p_service_uuid);
        }

        if (adv->p_manufacturer_data) {
            osi_free(adv->p_manufacturer_data);
        }
        break;
    }
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW: {
        uint8_t *raw_adv = ((btc_ble_gap_args_t *)msg->arg)->cfg_adv_data_raw.raw_adv;
        if (raw_adv) {
            osi_free(raw_adv);
        }
        break;
    }
    case BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW: {
        uint8_t *raw_scan_rsp = ((btc_ble_gap_args_t *)msg->arg)->cfg_scan_rsp_data_raw.raw_scan_rsp;
        if (raw_scan_rsp) {
            osi_free(raw_scan_rsp);
        }
        break;
    }
    case BTC_GAP_BLE_SET_SECURITY_PARAM_EVT: {
        uint8_t *value = ((btc_ble_gap_args_t *)msg->arg)->set_security_param.value;
        if (value) {
            osi_free(value);
        }
        break;
    }
    case BTC_GAP_BLE_OOB_REQ_REPLY_EVT: {
        uint8_t *value = ((btc_ble_gap_args_t *)msg->arg)->oob_req_reply.p_value;
        if (value) {
            osi_free(value);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_gap_ble_cb_deep_free(btc_msg_t *msg)
{
    BTC_TRACE_DEBUG("%s", __func__);
    switch (msg->act) {
        default:
            BTC_TRACE_DEBUG("Unhandled deep free %d", msg->act);
            break;
    }
}

void btc_gap_ble_call_handler(btc_msg_t *msg)
{
    btc_ble_gap_args_t *arg = (btc_ble_gap_args_t *)msg->arg;

    BTC_TRACE_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

    switch (msg->act) {
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA: {
        if (arg->cfg_adv_data.adv_data.set_scan_rsp == false) {
            btc_ble_set_adv_data(&arg->cfg_adv_data.adv_data, btc_adv_data_callback);
        } else {
            btc_ble_set_adv_data(&arg->cfg_adv_data.adv_data, btc_scan_rsp_data_callback);
        }
        break;
    }
    case BTC_GAP_BLE_ACT_SET_SCAN_PARAM:
        btc_ble_set_scan_params(&arg->set_scan_param.scan_params, btc_scan_params_callback);
        break;
    case BTC_GAP_BLE_ACT_START_SCAN:
        btc_ble_start_scanning(arg->start_scan.duration, btc_search_callback, btc_start_scan_callback);
        break;
    case BTC_GAP_BLE_ACT_STOP_SCAN:
        btc_ble_stop_scanning(btc_stop_scan_callback);
        break;
    case BTC_GAP_BLE_ACT_START_ADV:
        btc_ble_start_advertising(&arg->start_adv.adv_params, btc_start_adv_callback);
        break;
    case BTC_GAP_BLE_ACT_STOP_ADV:
        btc_ble_stop_advertising(btc_stop_adv_callback);
        break;
    case BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM:
        btc_ble_update_conn_params(arg->conn_update_params.conn_params.bda,
                                   arg->conn_update_params.conn_params.min_int,
                                   arg->conn_update_params.conn_params.max_int,
                                   arg->conn_update_params.conn_params.latency,
                                   arg->conn_update_params.conn_params.timeout);
        break;
    case BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN:
        btc_ble_set_pkt_data_len(arg->set_pkt_data_len.remote_device, arg->set_pkt_data_len.tx_data_length, btc_set_pkt_length_callback);
        break;
    case BTC_GAP_BLE_ACT_SET_RAND_ADDRESS: {
        BD_ADDR bd_addr;
        memcpy(bd_addr, arg->set_rand_addr.rand_addr, sizeof(BD_ADDR));
        btc_ble_set_rand_addr(bd_addr, btc_set_rand_addr_callback);
        break;
    }
    case BTC_GAP_BLE_ACT_CLEAR_RAND_ADDRESS: {
        btc_ble_clear_rand_addr();
        break;
    }
    case BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY:
        btc_ble_config_local_privacy(arg->cfg_local_privacy.privacy_enable, btc_set_local_privacy_callback);
        break;
    case BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON:
        btc_ble_config_local_icon(arg->cfg_local_icon.icon);
        break;
    case BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST:
        BTA_DmUpdateWhiteList(arg->update_white_list.add_remove, arg->update_white_list.remote_bda, arg->update_white_list.wl_addr_type, btc_add_whitelist_complete_callback);
        break;
    case BTC_GAP_BLE_ACT_READ_RSSI:
        BTA_DmBleReadRSSI(arg->read_rssi.remote_addr, BTA_TRANSPORT_LE, btc_read_ble_rssi_cmpl_callback);
        break;
    case BTC_GAP_BLE_ACT_SET_CONN_PARAMS:
        BTA_DmSetBlePrefConnParams(arg->set_conn_params.bd_addr, arg->set_conn_params.min_conn_int,
                                                        arg->set_conn_params.max_conn_int, arg->set_conn_params.slave_latency,
                                                        arg->set_conn_params.supervision_tout);
        break;
    case BTC_GAP_BLE_ACT_SET_DEV_NAME:
        BTA_DmSetDeviceName(arg->set_dev_name.device_name);
        break;
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW:
        btc_ble_set_adv_data_raw(arg->cfg_adv_data_raw.raw_adv,
                                 arg->cfg_adv_data_raw.raw_adv_len,
                                 btc_adv_data_raw_callback);
        break;
    case BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW:
        btc_ble_set_scan_rsp_data_raw(arg->cfg_scan_rsp_data_raw.raw_scan_rsp,
                                      arg->cfg_scan_rsp_data_raw.raw_scan_rsp_len,
                                      btc_scan_rsp_data_raw_callback);
        break;
    case BTC_GAP_BLE_UPDATE_DUPLICATE_SCAN_EXCEPTIONAL_LIST:
        btc_ble_update_duplicate_exceptional_list(arg->update_duplicate_exceptional_list.subcode,
                                                arg->update_duplicate_exceptional_list.info_type,
                                                arg->update_duplicate_exceptional_list.device_info,
                                                btc_update_duplicate_exceptional_list_callback);
        break;
#if (SMP_INCLUDED == TRUE)
    case BTC_GAP_BLE_SET_ENCRYPTION_EVT: {
        BD_ADDR bd_addr;
        memcpy(bd_addr, arg->set_encryption.bd_addr, sizeof(BD_ADDR));
        BTA_DmSetEncryption(bd_addr, BT_TRANSPORT_LE, btc_set_encryption_callback,
                                          (tBTA_DM_BLE_SEC_ACT)arg->set_encryption.sec_act);
        break;
    }
    case BTC_GAP_BLE_SET_SECURITY_PARAM_EVT: {
        uint8_t *value = arg->set_security_param.value;
        switch(arg->set_security_param.param_type) {
            case ESP_BLE_SM_PASSKEY:
                break;
            case ESP_BLE_SM_AUTHEN_REQ_MODE: {
                uint8_t authen_req = 0;
                STREAM_TO_UINT8(authen_req, value);
                bta_dm_co_ble_set_auth_req(authen_req);
                break;
            }
            case ESP_BLE_SM_IOCAP_MODE: {
                uint8_t iocap = 0;
                STREAM_TO_UINT8(iocap, value);
                bta_dm_co_ble_set_io_cap(iocap);
                break;
            }
            case ESP_BLE_SM_SET_INIT_KEY: {
                uint8_t init_key = 0;
                STREAM_TO_UINT8(init_key, value);
                bta_dm_co_ble_set_init_key_req(init_key);
                break;
            }
            case ESP_BLE_SM_SET_RSP_KEY: {
                uint8_t rsp_key = 0;
                STREAM_TO_UINT8(rsp_key, value);
                bta_dm_co_ble_set_rsp_key_req(rsp_key);
                break;
            }
            case ESP_BLE_SM_MAX_KEY_SIZE: {
                uint8_t key_size = 0;
                STREAM_TO_UINT8(key_size, value);
                bta_dm_co_ble_set_max_key_size(key_size);
                break;
            }
            case ESP_BLE_SM_MIN_KEY_SIZE: {
                uint8_t key_size = 0;
                STREAM_TO_UINT8(key_size, value);
                bta_dm_co_ble_set_min_key_size(key_size);
                break;
            }
            case ESP_BLE_SM_SET_STATIC_PASSKEY: {
                uint32_t passkey = 0;
                for(uint8_t i = 0; i < arg->set_security_param.len; i++)
                {
                    passkey += (((uint8_t *)value)[i]<<(8*i));
                }
                BTA_DmBleSetStaticPasskey(true, passkey);
                break;
            }
            case ESP_BLE_SM_CLEAR_STATIC_PASSKEY: {
                BTA_DmBleSetStaticPasskey(false, 0);
                break;
            }
            case ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH: {
                uint8_t enable = 0;
                STREAM_TO_UINT8(enable, value);
                bta_dm_co_ble_set_accept_auth_enable(enable);
                break;
            }
            case ESP_BLE_SM_OOB_SUPPORT: {
                uint8_t enable = 0;
                STREAM_TO_UINT8(enable, value);
                bta_dm_co_ble_oob_support(enable);
                break;
            }
            default:
                break;
        }
        break;
    }
    case BTC_GAP_BLE_SECURITY_RSP_EVT: {
        BD_ADDR bd_addr;
        tBTA_DM_BLE_SEC_GRANT res = arg->sec_rsp.accept ? BTA_DM_SEC_GRANTED : BTA_DM_SEC_PAIR_NOT_SPT;
        memcpy(bd_addr, arg->sec_rsp.bd_addr, sizeof(BD_ADDR));
        BTA_DmBleSecurityGrant(bd_addr, res);
        break;
    }
    case BTC_GAP_BLE_PASSKEY_REPLY_EVT: {
        BD_ADDR bd_addr;
        memcpy(bd_addr, arg->enc_passkey_replay.bd_addr, sizeof(BD_ADDR));
        BTA_DmBlePasskeyReply(bd_addr, arg->enc_passkey_replay.accept, arg->enc_passkey_replay.passkey);
        break;
    }
    case BTC_GAP_BLE_CONFIRM_REPLY_EVT: {
        BD_ADDR bd_addr;
        memcpy(bd_addr, arg->enc_comfirm_replay.bd_addr, sizeof(BD_ADDR));
        BTA_DmBleConfirmReply(bd_addr, arg->enc_comfirm_replay.accept);
        break;
    }
    case BTC_GAP_BLE_REMOVE_BOND_DEV_EVT: {
        BD_ADDR bd_addr;
        memcpy(bd_addr, arg->remove_bond_device.bd_addr, sizeof(BD_ADDR));
        BTA_DmRemoveDevice(bd_addr, BT_TRANSPORT_LE);
        break;
    }
    case BTC_GAP_BLE_OOB_REQ_REPLY_EVT:
        BTA_DmOobReply(arg->oob_req_reply.bd_addr, arg->oob_req_reply.len, arg->oob_req_reply.p_value);
        break;
#endif  ///SMP_INCLUDED == TRUE
    case BTC_GAP_BLE_DISCONNECT_EVT:
        btc_ble_disconnect(arg->disconnect.remote_device);
        break;
    case BTC_GAP_BLE_SET_AFH_CHANNELS:
        btc_gap_ble_set_channels(arg->set_channels.channels);
        break;
    default:
        break;
    }

    btc_gap_ble_arg_deep_free(msg);
}

//register connection parameter update callback
void btc_gap_callback_init(void)
{
    BTM_BleRegiseterConnParamCallback(btc_update_conn_param_callback);

}

void btc_gap_ble_deinit(void)
{
    btc_cleanup_adv_data(&gl_bta_adv_data);
    btc_cleanup_adv_data(&gl_bta_scan_rsp_data);
}

#if SCAN_QUEUE_CONGEST_CHECK
void btc_adv_list_free(void *data)
{
    osi_free(data);
}

void btc_adv_list_init(void)
{
    osi_mutex_new(&adv_list_lock);
    adv_filter_list = list_new(btc_adv_list_free);
}

void btc_adv_list_deinit(void)
{
    osi_mutex_free(&adv_list_lock);
    if(adv_filter_list) {
        list_free(adv_filter_list);
        adv_filter_list = NULL;
    }
}
void btc_adv_list_add_packet(void * data)
{
    if(!data) {
        BTC_TRACE_ERROR("%s data is NULL", __func__);
        return;
    }
    btc_adv_list_lock();
    list_prepend(adv_filter_list, data);
    btc_adv_list_unlock();
}

uint32_t btc_get_adv_list_length(void)
{
    if(!adv_filter_list) {
        BTC_TRACE_ERROR("%s adv_filter_list is NULL", __func__);
        return 0;
    }
    btc_adv_list_lock();
    size_t length = list_length(adv_filter_list);
    btc_adv_list_unlock();

    return length;
}

void btc_adv_list_refresh(void)
{
    if(!adv_filter_list) {
        BTC_TRACE_ERROR("%s adv_filter_list is NULL", __func__);
        return ;
    }
    btc_adv_list_lock();
    list_clear(adv_filter_list);
    btc_adv_list_unlock();
}

bool btc_check_adv_list(uint8_t * addr, uint8_t addr_type)
{
    bool found = false;
    if(!adv_filter_list || !addr) {
        BTC_TRACE_ERROR("%s adv_filter_list is NULL", __func__);
        return found;
    }

    btc_adv_list_lock();
    for (const list_node_t *node = list_begin(adv_filter_list); node != list_end(adv_filter_list); node = list_next(node)) {
        btc_adv_packet_t *packet = (btc_adv_packet_t *)list_node(node);
        if(!bdcmp(addr, packet->addr) && packet->addr_type == addr_type) {
            found = true;
            break;
        }
     }
     btc_adv_list_unlock();
     if(!found) {
         btc_adv_packet_t *adv_packet = osi_malloc(sizeof(btc_adv_packet_t));
         if(adv_packet) {
             adv_packet->addr_type = addr_type;
             bdcpy(adv_packet->addr, addr);
             btc_adv_list_add_packet(adv_packet);
         } else {
             BTC_TRACE_ERROR("%s adv_packet malloc failed", __func__);
         }
     }
    return found;
}

void btc_adv_list_lock(void)
{
    osi_mutex_lock(&adv_list_lock, OSI_MUTEX_MAX_TIMEOUT);
}

void btc_adv_list_unlock(void)
{
    osi_mutex_unlock(&adv_list_lock);
}
#endif
#endif  ///BLE_INCLUDED == TRUE
