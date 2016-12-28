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

#include "bt_types.h"
#include "bta_api.h"
#include "btc_task.h"
#include "btc_manage.h"
#include "btc_gap_ble.h"
#include "btc_gatt_util.h"
#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

static tBTA_BLE_ADV_DATA gl_bta_adv_data;
static tBTA_BLE_ADV_DATA gl_bta_scan_rsp_data;

#define BTC_GAP_BLE_CB_TO_APP(event, param) ((esp_gap_ble_cb_t)btc_profile_cb_get(BTC_PID_GAP_BLE))((event), (param))


static void btc_gap_adv_point_cleanup(void **buf)
{
    if (NULL == *buf) {
        return;
    }
    GKI_freebuf(*buf);
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
        bta_adv_data->p_manu = GKI_getbuf(sizeof(tBTA_BLE_MANU));
        if (bta_adv_data->p_manu != NULL) {
            bta_adv_data->p_manu->p_val = GKI_getbuf(p_adv_data->manufacturer_len);
            if (bta_adv_data->p_manu->p_val != NULL) {
                mask |= BTM_BLE_AD_BIT_MANU;
                bta_adv_data->p_manu->len = p_adv_data->manufacturer_len;
                memcpy(bta_adv_data->p_manu->p_val, p_adv_data->p_manufacturer_data, p_adv_data->manufacturer_len);
            }
        }
    }

    tBTA_BLE_PROP_ELEM *p_elem_service_data = NULL;
    if (p_adv_data->service_data_len > 0 && p_adv_data->p_service_data != NULL) {
        p_elem_service_data = GKI_getbuf(sizeof(tBTA_BLE_PROP_ELEM));
        if (p_elem_service_data != NULL) {
            p_elem_service_data->p_val = GKI_getbuf(p_adv_data->service_data_len);
            if (p_elem_service_data->p_val != NULL) {
                p_elem_service_data->adv_type = BTM_BLE_AD_TYPE_SERVICE_DATA;
                p_elem_service_data->len = p_adv_data->service_data_len;
                memcpy(p_elem_service_data->p_val, p_adv_data->p_service_data,
                       p_adv_data->service_data_len);
            } else {
                GKI_freebuf(p_elem_service_data);
                p_elem_service_data = NULL;
            }
        }
    }

    if (NULL != p_elem_service_data) {
        bta_adv_data->p_proprietary = GKI_getbuf(sizeof(tBTA_BLE_PROPRIETARY));
        if (NULL != bta_adv_data->p_proprietary) {
            tBTA_BLE_PROP_ELEM *p_elem = NULL;
            tBTA_BLE_PROPRIETARY *p_prop = bta_adv_data->p_proprietary;
            p_prop->num_elem = 0;
            mask |= BTM_BLE_AD_BIT_PROPRIETARY;
            p_prop->num_elem = 1;
            p_prop->p_elem = GKI_getbuf(sizeof(tBTA_BLE_PROP_ELEM) * p_prop->num_elem);
            p_elem = p_prop->p_elem;
            if (NULL != p_elem) {
                memcpy(p_elem++, p_elem_service_data, sizeof(tBTA_BLE_PROP_ELEM));
            }
            GKI_freebuf(p_elem_service_data);
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
                    bta_adv_data->p_services = GKI_getbuf(sizeof(tBTA_BLE_SERVICE));
                    bta_adv_data->p_services->list_cmpl = FALSE;
                    bta_adv_data->p_services->num_service = 0;
                    bta_adv_data->p_services->p_uuid = GKI_getbuf(p_adv_data->service_uuid_len / LEN_UUID_128 * LEN_UUID_16);
                    p_uuid_out16 = bta_adv_data->p_services->p_uuid;
                }

                if (NULL != bta_adv_data->p_services->p_uuid) {
                    LOG_DEBUG("%s - In 16-UUID_data", __FUNCTION__);
                    mask |= BTM_BLE_AD_BIT_SERVICE;
                    ++bta_adv_data->p_services->num_service;
                    *p_uuid_out16++ = bt_uuid.uu.uuid16;
                }
                break;
            }

            case (LEN_UUID_32): {
                if (NULL == bta_adv_data->p_service_32b) {
                    bta_adv_data->p_service_32b =
                        GKI_getbuf(sizeof(tBTA_BLE_32SERVICE));
                    bta_adv_data->p_service_32b->list_cmpl = FALSE;
                    bta_adv_data->p_service_32b->num_service = 0;
                    bta_adv_data->p_service_32b->p_uuid =
                        GKI_getbuf(p_adv_data->service_uuid_len / LEN_UUID_128 * LEN_UUID_32);
                    p_uuid_out32 = bta_adv_data->p_service_32b->p_uuid;
                }

                if (NULL != bta_adv_data->p_service_32b->p_uuid) {
                    LOG_DEBUG("%s - In 32-UUID_data", __FUNCTION__);
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
                        GKI_getbuf(sizeof(tBTA_BLE_128SERVICE));
                    if (NULL != bta_adv_data->p_services_128b) {
                        LOG_ERROR("%s - In 128-UUID_data", __FUNCTION__);
                        mask |= BTM_BLE_AD_BIT_SERVICE_128;
                        memcpy(bta_adv_data->p_services_128b->uuid128,
                               bt_uuid.uu.uuid128, LEN_UUID_128);
                        LOG_DEBUG("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x", bt_uuid.uu.uuid128[0],
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
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
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
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
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

static void btc_ble_start_advertising (esp_ble_adv_params_t *ble_adv_params)
{
    tBLE_BD_ADDR peer_addr;

    if (!BLE_ISVALID_PARAM(ble_adv_params->adv_int_min, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX) ||
            !BLE_ISVALID_PARAM(ble_adv_params->adv_int_max, BTM_BLE_ADV_INT_MIN, BTM_BLE_ADV_INT_MAX)) {
        LOG_ERROR("Invalid advertisting interval parameters.\n");
        return ;
    }

    if ((ble_adv_params->adv_type < ADV_TYPE_IND) &&
            (ble_adv_params->adv_type > ADV_TYPE_DIRECT_IND_LOW) ) {
        LOG_ERROR("Invalid advertisting type parameters.\n");
        return;
    }

    if ((ble_adv_params->adv_filter_policy < ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY) &&
            (ble_adv_params->adv_filter_policy > ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST) ) {
        LOG_ERROR("Invalid advertisting type parameters.\n");
        return;
    }
    LOG_DEBUG("API_Ble_AppStartAdvertising\n");

    memcpy(peer_addr.bda, ble_adv_params->peer_addr, ESP_BD_ADDR_LEN);
    peer_addr.type = ble_adv_params->peer_addr_type;
    BTA_DmSetBleAdvParamsAll(ble_adv_params->adv_int_min,
                             ble_adv_params->adv_int_max,
                             ble_adv_params->adv_type,
                             ble_adv_params->own_addr_type,
                             ble_adv_params->channel_map,
                             ble_adv_params->adv_filter_policy,
                             &peer_addr);
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
        LOG_ERROR("%s btc_transfer_context failed\n", __func__);
    }
}

static void btc_ble_set_scan_params(esp_ble_scan_params_t *scan_params, tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback)
{
    if (BLE_ISVALID_PARAM(scan_params->scan_interval, BTM_BLE_SCAN_INT_MIN, BTM_BLE_SCAN_INT_MAX) &&
            BLE_ISVALID_PARAM(scan_params->scan_window, BTM_BLE_SCAN_WIN_MIN, BTM_BLE_SCAN_WIN_MAX) &&
            (scan_params->scan_type == BTM_BLE_SCAN_MODE_ACTI || scan_params->scan_type == BTM_BLE_SCAN_MODE_PASS)) {
        BTA_DmSetBleScanFilterParams(ESP_DEFAULT_GATT_IF,	/*client_if*/
                                     scan_params->scan_interval,
                                     scan_params->scan_window,
                                     scan_params->scan_type,
                                     scan_params->own_addr_type,
                                     scan_params->scan_filter_policy,
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
        bdcpy(param.scan_rst.bda, p_data->inq_res.bd_addr);
        param.scan_rst.dev_type = p_data->inq_res.device_type;
        param.scan_rst.rssi = p_data->inq_res.rssi;
        param.scan_rst.ble_addr_type = p_data->inq_res.ble_addr_type;
		param.scan_rst.ble_evt_type = p_data->inq_res.ble_evt_type;
        param.scan_rst.flag = p_data->inq_res.flag;
        memcpy(param.scan_rst.ble_adv, p_data->inq_res.p_eir,
               ESP_BLE_ADV_DATA_LEN_MAX);
        break;
    }
    case BTA_DM_INQ_CMPL_EVT: {
        param.scan_rst.num_resps = p_data->inq_cmpl.num_resps;
        LOG_ERROR("%s  BLE observe complete. Num Resp %d\n", __FUNCTION__, p_data->inq_cmpl.num_resps);
        break;
    }
    case BTA_DM_DISC_RES_EVT:
        LOG_ERROR("BTA_DM_DISC_RES_EVT\n");
        break;
    case BTA_DM_DISC_BLE_RES_EVT:
        LOG_ERROR("BTA_DM_DISC_BLE_RES_EVT\n");
        break;
    case BTA_DM_DISC_CMPL_EVT:
        LOG_ERROR("BTA_DM_DISC_CMPL_EVT\n");
        break;
    case BTA_DM_DI_DISC_CMPL_EVT:
        LOG_ERROR("BTA_DM_DI_DISC_CMPL_EVT\n");
        break;
    case BTA_DM_SEARCH_CANCEL_CMPL_EVT:
        LOG_ERROR("BTA_DM_SEARCH_CANCEL_CMPL_EVT\n");
        break;
    default:
        LOG_ERROR("%s : Unknown event 0x%x\n", __FUNCTION__, event);
        return;
    }
    btc_transfer_context(&msg, &param, sizeof(esp_ble_gap_cb_param_t), NULL);
}


static void btc_ble_start_scanning(uint8_t duration, tBTA_DM_SEARCH_CBACK *results_cb)
{
    if ((duration != 0) && (results_cb != NULL)) {
        ///Start scan the device
        BTA_DmBleObserve(true, duration, results_cb);
    } else {
        LOG_ERROR("The scan duration or p_results_cb invalid\n");
    }
}

static void btc_ble_stop_scanning(void)
{
    uint8_t duration = 0;
    BTA_DmBleObserve(false, duration, NULL);
}


static void btc_ble_stop_advertising(void)
{
    bool stop_adv = false;

    BTA_DmBleBroadcast(stop_adv);
}

static void btc_ble_update_conn_params(BD_ADDR bd_addr, uint16_t min_int,
                                       uint16_t max_int, uint16_t latency, uint16_t timeout)
{
    if (min_int > max_int) {
        min_int = max_int;
    }

    if (min_int < BTM_BLE_CONN_INT_MIN || max_int > BTM_BLE_CONN_INT_MAX) {
        LOG_ERROR("Invalid interval value.\n");
    }

    BTA_DmBleUpdateConnectionParams(bd_addr, min_int, max_int,
                                    latency, timeout);
}

static void btc_ble_set_pkt_data_len(BD_ADDR remote_device, uint16_t tx_data_length)
{
    if (tx_data_length > BTM_BLE_DATA_SIZE_MAX) {
        tx_data_length =  BTM_BLE_DATA_SIZE_MAX;
    } else if (tx_data_length < BTM_BLE_DATA_SIZE_MIN) {
        tx_data_length =  BTM_BLE_DATA_SIZE_MIN;
    }

    BTA_DmBleSetDataLength(remote_device, tx_data_length);
}

static void btc_ble_set_rand_addr (BD_ADDR rand_addr)
{
    if (rand_addr != NULL) {
        BTA_DmSetRandAddress(rand_addr);
    } else {
        LOG_ERROR("Invalid randrom address.\n");
    }
}

static void btc_ble_config_local_privacy(bool privacy_enable)
{
    BTA_DmBleConfigLocalPrivacy(privacy_enable);
}


void btc_gap_ble_cb_handler(btc_msg_t *msg)
{
    esp_ble_gap_cb_param_t *param = (esp_ble_gap_cb_param_t *)msg->arg;

    switch (msg->act) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        BTC_GAP_BLE_CB_TO_APP(ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT, param);
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT :
        BTC_GAP_BLE_CB_TO_APP(ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT, param);
        break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        BTC_GAP_BLE_CB_TO_APP(ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT, param);
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
        BTC_GAP_BLE_CB_TO_APP(ESP_GAP_BLE_SCAN_RESULT_EVT, param);
        break;
    default:
        break;

    }

}

void btc_gap_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    switch (msg->act) {
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA: {
        btc_ble_gap_args_t *src = (btc_ble_gap_args_t *)p_src;
        btc_ble_gap_args_t  *dst = (btc_ble_gap_args_t *) p_dest;

        if (src->cfg_adv_data.adv_data.p_manufacturer_data) {
            dst->cfg_adv_data.adv_data.p_manufacturer_data = GKI_getbuf(src->cfg_adv_data.adv_data.manufacturer_len);
            memcpy(dst->cfg_adv_data.adv_data.p_manufacturer_data, src->cfg_adv_data.adv_data.p_manufacturer_data,
                   src->cfg_adv_data.adv_data.manufacturer_len);
        }

        if (src->cfg_adv_data.adv_data.p_service_data) {
            dst->cfg_adv_data.adv_data.p_service_data = GKI_getbuf(src->cfg_adv_data.adv_data.service_data_len);
            memcpy(dst->cfg_adv_data.adv_data.p_service_data, src->cfg_adv_data.adv_data.p_service_data, src->cfg_adv_data.adv_data.service_data_len);
        }

        if (src->cfg_adv_data.adv_data.p_service_uuid) {
            dst->cfg_adv_data.adv_data.p_service_uuid = GKI_getbuf(src->cfg_adv_data.adv_data.service_uuid_len);
            memcpy(dst->cfg_adv_data.adv_data.p_service_uuid, src->cfg_adv_data.adv_data.p_service_uuid, src->cfg_adv_data.adv_data.service_uuid_len);
        }
        break;
    }
    default:
        LOG_ERROR("Unhandled deep copy %d\n", msg->act);
        break;
    }
}

static void btc_gap_ble_arg_deep_free(btc_msg_t *msg)
{
    LOG_DEBUG("%s \n", __func__);
    switch (msg->act) {
    case BTC_GAP_BLE_ACT_CFG_ADV_DATA: {
        esp_ble_adv_data_t *adv = &((btc_ble_gap_args_t *)msg->arg)->cfg_adv_data.adv_data;
        if (adv->p_service_data) {
            GKI_freebuf(adv->p_service_data);
        }

        if (adv->p_service_uuid) {
            GKI_freebuf(adv->p_service_uuid);
        }

        if (adv->p_manufacturer_data) {
            GKI_freebuf(adv->p_manufacturer_data);
        }
        break;
    }
    default:
        LOG_DEBUG("Unhandled deep free %d\n", msg->act);
        break;
    }
}

void btc_gap_ble_call_handler(btc_msg_t *msg)
{
    btc_ble_gap_args_t *arg = (btc_ble_gap_args_t *)msg->arg;

    LOG_DEBUG("%s act %d\n", __FUNCTION__, msg->act);

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
        btc_ble_start_scanning(arg->start_scan.duration, btc_search_callback);
        break;
    case BTC_GAP_BLE_ACT_STOP_SCAN:
        btc_ble_stop_scanning();
        break;
    case BTC_GAP_BLE_ACT_START_ADV:
        btc_ble_start_advertising(&arg->start_adv.adv_params);
        break;
    case BTC_GAP_BLE_ACT_STOP_ADV:
        btc_ble_stop_advertising();
        break;
    case BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM:
        btc_ble_update_conn_params(arg->conn_update_params.conn_params.bda,
                                   arg->conn_update_params.conn_params.min_int,
                                   arg->conn_update_params.conn_params.max_int,
                                   arg->conn_update_params.conn_params.latency,
                                   arg->conn_update_params.conn_params.timeout);
        break;
    case BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN:
        btc_ble_set_pkt_data_len(arg->set_pkt_data_len.remote_device, arg->set_pkt_data_len.tx_data_length);
        break;
    case BTC_GAP_BLE_ACT_SET_RAND_ADDRESS:
        btc_ble_set_rand_addr(arg->set_rand_addr.rand_addr);
        break;
    case BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY:
        btc_ble_config_local_privacy(arg->cfg_local_privacy.privacy_enable);
        break;
    case BTC_GAP_BLE_ACT_SET_DEV_NAME:
        BTA_DmSetDeviceName(arg->set_dev_name.device_name);
        break;
    default:
        break;
    }

    btc_gap_ble_arg_deep_free(msg);
}
