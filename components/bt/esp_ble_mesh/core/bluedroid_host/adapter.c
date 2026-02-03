/*
 * SPDX-FileCopyrightText: 2017 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "bta/bta_gatt_common.h"
#include "bta_gattc_int.h"
#include "stack/btm_ble_api.h"
#include "p_256_ecc_pp.h"
#include "osi/future.h"
#include "device/controller.h"
#include "mesh/hci.h"
#include "mesh/adapter.h"
#include "mesh/common.h"
#include "prov_pvnr.h"
#include "scan.h"
#include "net.h"
#include "beacon.h"
#include "btc_ble_mesh_ble.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif

struct bt_mesh_dev bt_mesh_dev;

#define BLE_MESH_BTM_CHECK_STATUS(func) do {                                                     \
        tBTM_STATUS __status = (func);                                                           \
        if ((__status != BTM_SUCCESS) && (__status != BTM_CMD_STARTED)) {                        \
            BT_ERR("%s, Invalid status %d", __func__, __status);                                 \
            return -1;                                                                           \
        }                                                                                        \
    } while(0);

#define BLE_MESH_GATT_GET_CONN_ID(conn_id)              (((uint16_t)(conn_id)) >> 8)
#define BLE_MESH_GATT_CREATE_CONN_ID(gatt_if, conn_id)  ((uint16_t)((((uint8_t)(conn_id)) << 8) | ((uint8_t)(gatt_if))))

/* We don't need to manage the BLE_MESH_DEV_ADVERTISING flags in the version of Bluedroid,
 * it will manage it in the BTM layer.
 */
#define BLE_MESH_DEV    0

/* Scan related functions */
static bt_mesh_scan_cb_t *bt_mesh_scan_dev_found_cb;

#if !CONFIG_BLE_MESH_USE_BLE_50
static void bt_mesh_scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
#endif

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
/* Using UUID with a fixed pattern 0x96 for BLE Mesh GATT Proxy Server */
#define BLE_MESH_GATTS_APP_UUID_BYTE     0x96
/* the gatt database list to save the attribute table */
static sys_slist_t bt_mesh_gatts_db;

/* Static Variables */
static struct bt_mesh_conn bt_mesh_gatts_conn[BLE_MESH_MAX_CONN];
static struct bt_mesh_conn_cb *bt_mesh_gatts_conn_cb;
static tBTA_GATTS_IF bt_mesh_gatts_if;
static uint8_t bt_mesh_gatts_addr[BLE_MESH_ADDR_LEN];
static uint16_t svc_handle, char_handle;
static future_t *gatts_future_mesh;

/* Static Functions */
static struct bt_mesh_gatt_attr *bt_mesh_gatts_find_attr_by_handle(uint16_t handle);
#endif

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
/* Using UUID with a fixed pattern 0x97 for BLE Mesh GATT Proxy Client */
#define BLE_MESH_GATTC_APP_UUID_BYTE     0x97
static struct gattc_prov_info {
    /* Service to be found depends on the type of adv pkt received */
    struct bt_mesh_conn conn;
    bt_mesh_addr_t addr;
    uint16_t service_uuid;
    uint16_t mtu;
    bool     wr_desc_done;    /* Indicate if write char descriptor event is received */
    uint16_t start_handle;    /* Service attribute start handle */
    uint16_t end_handle;      /* Service attribute end handle */
    uint16_t data_in_handle;  /* Data In Characteristic attribute handle */
    uint16_t data_out_handle; /* Data Out Characteristic attribute handle */
    uint16_t ccc_handle;      /* Data Out Characteristic CCC attribute handle */
} bt_mesh_gattc_info[BLE_MESH_MAX_CONN];
static struct bt_mesh_prov_conn_cb *bt_mesh_gattc_conn_cb;
static tBTA_GATTC_IF bt_mesh_gattc_if;
#endif

#if CONFIG_BLE_MESH_USE_BLE_50      && \
    CONFIG_BLE_MESH_SUPPORT_BLE_ADV && \
    (!CONFIG_BLE_MESH_SUPPORT_MULTI_ADV)
static inline void bt_mesh_set_ble_adv_running();

static inline void bt_mesh_unset_ble_adv_running();

static inline bool bt_mesh_is_ble_adv_running();
#endif

int bt_mesh_host_init(void)
{
    return 0;
}

int bt_mesh_host_deinit(void)
{
    return 0;
}

void bt_mesh_hci_init(void)
{
    const uint8_t *features = controller_get_interface()->get_features_ble()->as_array;
    if (features != NULL) {
        memcpy(bt_mesh_dev.features[0], features, 8);
        memcpy(bt_mesh_dev.le.features, features, 8);
    }

    /**
     * Currently 20ms non-connectable adv interval is supported, and we need to add
     * a flag to indicate this support.
     */
#ifdef CONFIG_BLE_MESH_HCI_5_0
    bt_mesh_dev.hci_version = BLE_MESH_HCI_VERSION_5_0;
#else
    bt_mesh_dev.hci_version = controller_get_interface()->get_bt_version()->hci_version;
#endif
    bt_mesh_dev.lmp_version = controller_get_interface()->get_bt_version()->lmp_version;
    bt_mesh_dev.hci_revision = controller_get_interface()->get_bt_version()->hci_revision;
    bt_mesh_dev.lmp_subversion = controller_get_interface()->get_bt_version()->lmp_subversion;
    bt_mesh_dev.manufacturer = controller_get_interface()->get_bt_version()->manufacturer;

    const uint8_t *p = controller_get_interface()->get_ble_supported_states();
    uint64_t states_fh = 0, states_sh = 0;

    /* macro STREAM_TO_UINT32 expansion */
    states_fh = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24));
    (p) += 4;

    states_sh = (((uint32_t)(*(p))) + ((((uint32_t)(*((p) + 1)))) << 8) + ((((uint32_t)(*((p) + 2)))) << 16) + ((((uint32_t)(*((p) + 3)))) << 24));
    (p) += 4;

    bt_mesh_dev.le.states = (states_sh << 32) | states_fh;
}

#if !CONFIG_BLE_MESH_USE_BLE_50
static void bt_mesh_scan_results_change_2_bta(tBTM_INQ_RESULTS *p_inq, uint8_t *p_eir,
                                              tBTA_DM_SEARCH_CBACK *p_scan_cback)
{
    tBTM_INQ_INFO *p_inq_info = NULL;
    tBTA_DM_SEARCH result = {0};

    bdcpy(result.inq_res.bd_addr, p_inq->remote_bd_addr);
    result.inq_res.rssi = p_inq->rssi;
    result.inq_res.ble_addr_type    = p_inq->ble_addr_type;
    result.inq_res.inq_result_type  = p_inq->inq_result_type;
    result.inq_res.device_type      = p_inq->device_type;
    result.inq_res.flag             = p_inq->flag;
    result.inq_res.adv_data_len     = p_inq->adv_data_len;
    result.inq_res.scan_rsp_len     = p_inq->scan_rsp_len;
    memcpy(result.inq_res.dev_class, p_inq->dev_class, sizeof(DEV_CLASS));
    result.inq_res.ble_evt_type     = p_inq->ble_evt_type;

    /* application will parse EIR to find out remote device name */
    result.inq_res.p_eir = p_eir;

    if ((p_inq_info = BTM_InqDbRead(p_inq->remote_bd_addr)) != NULL) {
        /* initialize remt_name_not_required to FALSE so that we get the name by default */
        result.inq_res.remt_name_not_required = FALSE;
    }

    if (p_scan_cback) {
        p_scan_cback(BTA_DM_INQ_RES_EVT, &result);
    }

    if (p_inq_info) {
        /* application indicates if it knows the remote name, inside the callback
         copy that to the inquiry data base*/
        if (result.inq_res.remt_name_not_required) {
            p_inq_info->appl_knows_rem_name = TRUE;
        }
    }
}

static void bt_mesh_scan_results_cb(tBTM_INQ_RESULTS *p_inq, uint8_t *p_eir)
{
    bt_mesh_scan_results_change_2_bta(p_inq, p_eir, bt_mesh_scan_result_callback);
}
#endif

#if CONFIG_BLE_MESH_USE_BLE_50
extern void btc_ble_5_gap_callback(tBTA_DM_BLE_5_GAP_EVENT event,
                                   tBTA_DM_BLE_5_GAP_CB_PARAMS *params);

void bt_mesh_ble_ext_adv_report(tBTM_BLE_EXT_ADV_REPORT *ext_adv_report)
{
#if CONFIG_BLE_MESH_SUPPORT_BLE_SCAN
    bt_mesh_ble_adv_report_t adv_rpt = {0};

    if (bt_mesh_ble_scan_state_get()) {
        memcpy(adv_rpt.addr, ext_adv_report->addr, BLE_MESH_ADDR_LEN);
        memcpy(adv_rpt.dir_addr, ext_adv_report->dir_addr, BLE_MESH_ADDR_LEN);

        adv_rpt.addr_type           = ext_adv_report->addr_type;
        adv_rpt.data                = ext_adv_report->adv_data_len ? ext_adv_report->adv_data : NULL;
        adv_rpt.length              = ext_adv_report->adv_data_len;
        adv_rpt.rssi                = ext_adv_report->rssi;
        adv_rpt.event_type          = ext_adv_report->event_type;
        adv_rpt.primary_phy         = ext_adv_report->primary_phy;
        adv_rpt.secondary_phy       = ext_adv_report->secondry_phy;
        adv_rpt.sid                 = ext_adv_report->sid;
        adv_rpt.tx_power            = ext_adv_report->tx_power;
        adv_rpt.dir_addr_type       = ext_adv_report->dir_addr_type;
        adv_rpt.data_status         = ext_adv_report->data_status;
        adv_rpt.per_adv_interval    = ext_adv_report->per_adv_interval;

        bt_mesh_ble_scan_cb_evt_to_btc(&adv_rpt);
    }
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_SCAN */
}

#if CONFIG_BLE_MESH_LONG_PACKET
static struct {
    struct bt_mesh_adv_report adv_rpt;
    uint8_t adv_data_len;
    uint8_t adv_data[2 + CONFIG_BLE_MESH_LONG_PACKET_ADV_LEN];
} adv_report_cache;
#endif /* CONFIG_BLE_MESH_LONG_PACKET */

static bool bt_mesh_scan_result_process(tBTM_BLE_EXT_ADV_REPORT *ext_adv_report)
{
    struct bt_mesh_adv_report adv_rpt = {0};
    uint8_t *adv_data = NULL;
    uint8_t adv_data_len = 0;

    assert(ext_adv_report);

    adv_rpt.addr.type = ext_adv_report->addr_type;
    memcpy(adv_rpt.addr.val, ext_adv_report->addr, BLE_MESH_ADDR_LEN);
    adv_rpt.primary_phy = ext_adv_report->primary_phy;
    adv_rpt.secondary_phy = ext_adv_report->secondry_phy;
    adv_rpt.rssi = ext_adv_report->rssi;
    adv_rpt.tx_power = ext_adv_report->tx_power;
    adv_data = ext_adv_report->adv_data;
    adv_data_len = ext_adv_report->adv_data_len;

#if !CONFIG_BLE_MESH_EXT_ADV
    if (!(ext_adv_report->event_type & BTM_BLE_ADV_LEGACY_MASK)) {
        return false;
    }
#endif

    BT_DBG("Recv adv report type %04x", ext_adv_report->event_type);

    if (ext_adv_report->adv_data_len > BLE_MESH_GAP_ADV_MAX_LEN) {
        return false;
    }

    switch (ext_adv_report->event_type) {
    case BLE_MESH_ADV_IND:
    case BLE_MESH_ADV_DIRECT_IND:
    case BLE_MESH_ADV_SCAN_IND:
    case BLE_MESH_ADV_NONCONN_IND:
    case BLE_MESH_ADV_SCAN_RSP:
#if CONFIG_BLE_MESH_EXT_ADV
    case BLE_MESH_EXT_ADV_NONCONN_IND:
#endif
        adv_rpt.adv_type = ext_adv_report->event_type;
    break;
    default:
        return false;
    break;
    }

#if CONFIG_BLE_MESH_LONG_PACKET
    switch (ext_adv_report->data_status) {
    case BTM_BLE_EXT_ADV_DATA_COMPLETE:
        if (adv_report_cache.adv_data_len) {
            if (adv_report_cache.adv_data_len + ext_adv_report->adv_data_len > BLE_MESH_GAP_ADV_MAX_LEN) {
                memset(&adv_report_cache, 0, sizeof(adv_report_cache));
                return false;
            }
            memcpy(adv_report_cache.adv_data + adv_report_cache.adv_data_len,
                   ext_adv_report->adv_data, ext_adv_report->adv_data_len);
            adv_report_cache.adv_data_len += ext_adv_report->adv_data_len;
            adv_data = adv_report_cache.adv_data;
            adv_data_len = adv_report_cache.adv_data_len;
            adv_report_cache.adv_data_len = 0;
        }
        break;
    case BTM_BLE_EXT_ADV_DATA_INCOMPLETE:
        if ((adv_report_cache.adv_data_len + ext_adv_report->adv_data_len) > BLE_MESH_GAP_ADV_MAX_LEN) {
            adv_report_cache.adv_data_len = 0;
            return false;
        }
        if (adv_report_cache.adv_data_len == 0) {
            memcpy(&adv_report_cache.adv_rpt, &adv_rpt, sizeof(struct bt_mesh_adv_report));
        }
        memcpy(adv_report_cache.adv_data + adv_report_cache.adv_data_len,
                ext_adv_report->adv_data, ext_adv_report->adv_data_len);
        adv_report_cache.adv_data_len += ext_adv_report->adv_data_len;
        /* To avoid discarding user's packets,
         * it is assumed here that this packet
         * is not mesh's packet */
        return false;
    case BTM_BLE_EXT_ADV_DATA_TRUNCATED:
         if (adv_report_cache.adv_data_len) {
            memset(&adv_report_cache, 0, sizeof(adv_report_cache));
         }
         return false;
    default:
        assert(0);
    }
#else /* CONFIG_BLE_MESH_LONG_PACKET */
    if (ext_adv_report->data_status != BTM_BLE_EXT_ADV_DATA_COMPLETE) {
        return false;
    }
#endif /* CONFIG_BLE_MESH_LONG_PACKET */

    if (bt_mesh_scan_dev_found_cb) {
        net_buf_simple_init_with_data(&adv_rpt.adv_data, adv_data, adv_data_len);
        bt_mesh_scan_dev_found_cb(&adv_rpt);
        if (adv_rpt.adv_data.len != adv_data_len) {
            /* The advertising data has been processed by Mesh Protocol */
            return true;
        }
    }
    return false;
}

void ble_mesh_5_gap_callback(tBTA_DM_BLE_5_GAP_EVENT event,
                             tBTA_DM_BLE_5_GAP_CB_PARAMS *params)
{
    BT_DBG("recv event %d", event);

    switch (event) {
    case BTA_DM_BLE_5_GAP_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        if (!bt_mesh_is_adv_inst_used(params->set_params.instance)) {
            goto transfer_to_user;
        }
        if (params->set_params.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_ADV_SET_PARAMS_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_ADV_DATA_SET_COMPLETE_EVT:
        if (!bt_mesh_is_adv_inst_used(params->adv_data_set.instance)) {
            goto transfer_to_user;
        }
        if (params->adv_data_set.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_ADV_DATA_SET_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        if (!bt_mesh_is_adv_inst_used(params->scan_rsp_data_set.instance)) {
            goto transfer_to_user;
        }
        if (params->scan_rsp_data_set.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_ADV_START_COMPLETE_EVT:
        if (!bt_mesh_is_adv_inst_used(params->adv_start.instance[0])) {
            goto transfer_to_user;
        }
        if (params->adv_start.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_ADV_START_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_ADV_STOP_COMPLETE_EVT:
        if (!bt_mesh_is_adv_inst_used(params->adv_start.instance[0])) {
            goto transfer_to_user;
        }
        if (params->adv_start.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_ADV_STOP_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_ADV_TERMINATED_EVT:
        if (!bt_mesh_is_adv_inst_used(params->adv_term.adv_handle)) {
            goto transfer_to_user;
        }
#if CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
        bt_mesh_adv_task_wakeup(ADV_TASK_ADV_INST_EVT(params->adv_term.adv_handle));
#else /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
        if (params->adv_term.status == 0x43 ||  /* Limit reached */
            params->adv_term.status == 0x3C) {  /* Advertising timeout */
            bt_mesh_adv_task_wakeup(ADV_TASK_MESH_ADV_INST_EVT);
        }
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
        /**
         * This judgment is to distinguish between the termination
         * events of BLE connectable broadcasting and proxy connectable
         * adv under the same instance ID, that is, when the status is 0.
         *
         * Since the host task and adv task are currently operated in
         * series, there is no need to consider competition issues between
         * tasks.
         *
         * @attention: once multiple adv instances are used, the adv task
         * and host will be asynchronous, and it is necessary to consider
         * the issue of resource competition.
         */
        if (bt_mesh_is_ble_adv_running() &&
            params->adv_term.status == 0x00) {
            /* The unset operation must be performed before waking up the
                * adv task; performing the unset after waking up the adv task
                * could lead to resource contention issues.
                */
            bt_mesh_unset_ble_adv_running();
            bt_mesh_adv_task_wakeup(ADV_TASK_MESH_ADV_INST_EVT);
        }
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */
#endif /* CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */
        break;
    case BTA_DM_BLE_5_GAP_EXT_ADV_REPORT_EVT:
        if (!bt_mesh_scan_result_process(&params->ext_adv_report)) {
            bt_mesh_ble_ext_adv_report(&params->ext_adv_report);
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_SCAN_START_COMPLETE_EVT:
        if (params->scan_start.status != BTM_SUCCESS) {
            BT_ERR("BTA_DM_BLE_5_GAP_EXT_SCAN_START_COMPLETE_EVT Failed");
        }
        break;
    case BTA_DM_BLE_5_GAP_EXT_SCAN_STOP_COMPLETE_EVT:
        if (params->scan_stop.status != BTM_SUCCESS) {
            BT_ERR("BTM_BLE_5_GAP_EXT_SCAN_START_COMPLETE_EVT Failed");
        }
        break;
    default:
        goto transfer_to_user;
    }

    return;

transfer_to_user:
    btc_ble_5_gap_callback(event, params);
}
#endif

static bool valid_adv_param(const struct bt_mesh_adv_param *param)
{
    if (!(param->options & BLE_MESH_ADV_OPT_CONNECTABLE)) {
#if BLE_MESH_DEV
        if (bt_mesh_dev.hci_version < BLE_MESH_HCI_VERSION_5_0 &&
            param->interval_min < 0x00a0) {
            return false;
        }
#endif
    }

    if (param->interval_min > param->interval_max ||
        param->interval_min < 0x0020 || param->interval_max > 0x4000) {
        return false;
    }

    return true;
}

#if CONFIG_BLE_MESH_USE_BLE_50
static int set_adv_data(uint16_t hci_op, const uint8_t inst_id,
                        const struct bt_mesh_adv_data *ad, size_t ad_len)
#else
static int set_adv_data(uint16_t hci_op, const struct bt_mesh_adv_data *ad, size_t ad_len)
#endif
{
    struct bt_mesh_hci_cp_set_adv_data param = {0};
    int i;

    if (ad == NULL || ad_len == 0) {
        return 0;
    }

    for (i = 0; i < ad_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (param.len + ad[i].data_len + 2 > sizeof(param.data)) {
            return -EINVAL;
        }

        param.data[param.len++] = ad[i].data_len + 1;
        param.data[param.len++] = ad[i].type;

        memcpy(&param.data[param.len], ad[i].data, ad[i].data_len);
        param.len += ad[i].data_len;
    }

#if CONFIG_BLE_MESH_USE_BLE_50
        BTA_DmBleGapConfigExtAdvDataRaw(hci_op == BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA,
                                        inst_id, param.len, param.data);
#else
    /* Set adv data and scan rsp data. */
    if (hci_op == BLE_MESH_HCI_OP_SET_ADV_DATA) {
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteAdvDataRaw(param.data, param.len));
    } else if (hci_op == BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA) {
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteScanRspRaw(param.data, param.len));
    }
#endif

    return 0;
}

#if !CONFIG_BLE_MESH_USE_BLE_50
static void start_adv_completed_cb(uint8_t status)
{
#if BLE_MESH_DEV
    if (!status) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
    }
#endif
}
#endif

static bool valid_scan_param(const struct bt_mesh_scan_param *param)
{
    if (param->type != BLE_MESH_SCAN_PASSIVE &&
        param->type != BLE_MESH_SCAN_ACTIVE) {
        return false;
    }

    if (param->filter_dup != BLE_MESH_SCAN_FILTER_DUP_DISABLE &&
        param->filter_dup != BLE_MESH_SCAN_FILTER_DUP_ENABLE) {
        return false;
    }

    if (param->interval < 0x0004 || param->interval > 0x4000) {
        return false;
    }

    if (param->window < 0x0004 || param->window > 0x4000) {
        return false;
    }

    if (param->window > param->interval) {
        return false;
    }

    return true;
}

static int start_le_scan(uint8_t scan_type, uint16_t interval, uint16_t window,
                         uint8_t filter_dup, uint8_t scan_fil_policy)
{
#if !CONFIG_BLE_MESH_USE_BLE_50
    uint8_t addr_type_own = BLE_MESH_ADDR_PUBLIC;  /* Currently only support Public Address */
    tGATT_IF client_if = 0xFF; /* Default GATT interface id */
#endif

#if CONFIG_BLE_MESH_USE_BLE_50
    tBTA_DM_BLE_EXT_SCAN_PARAMS ext_scan_params = {0};

    if (interval == 0 ||
        interval < window) {
        BT_ERR("invalid scan param itvl %d win %d", interval, window);
        return EINVAL;
    }

    ext_scan_params.own_addr_type = BLE_MESH_ADDR_PUBLIC;
    ext_scan_params.filter_policy = scan_fil_policy;
    ext_scan_params.scan_duplicate = filter_dup;

    if (window == 0) {
        ext_scan_params.cfg_mask = BTM_BLE_GAP_EXT_SCAN_CODE_MASK;
    } else if (interval > window) {
        ext_scan_params.cfg_mask = BTM_BLE_GAP_EXT_SCAN_UNCODE_MASK | BTM_BLE_GAP_EXT_SCAN_CODE_MASK;
    } else {
        // interval == window
        ext_scan_params.cfg_mask = BTM_BLE_GAP_EXT_SCAN_UNCODE_MASK;
    }

    ext_scan_params.uncoded_cfg.scan_type = scan_type;
    ext_scan_params.uncoded_cfg.scan_interval = interval;
    ext_scan_params.uncoded_cfg.scan_window = window;

    ext_scan_params.coded_cfg.scan_type = scan_type;
    ext_scan_params.coded_cfg.scan_interval = interval;
    ext_scan_params.coded_cfg.scan_window = MAX(interval - window, BTM_BLE_SCAN_WIN_MIN);

    BTA_DmBleGapSetExtScanParams(&ext_scan_params);

    BTM_BleGapRegisterCallback(ble_mesh_5_gap_callback);

    BTA_DmBleGapExtScan(true, 0, 0);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
    BLE_MESH_BTM_CHECK_STATUS(
        BTM_BleSetScanFilterParams(client_if, interval, window, scan_type, addr_type_own,
                                   filter_dup, scan_fil_policy, NULL));

    /* BLE Mesh scan permanently, so no duration of scan here */
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleScan(true, 0, bt_mesh_scan_results_cb, NULL, NULL));
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

#if BLE_MESH_DEV
    if (scan_type == BLE_MESH_SCAN_ACTIVE) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ACTIVE_SCAN);
    } else {
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ACTIVE_SCAN);
    }
#endif

    return 0;
}

#if !CONFIG_BLE_MESH_USE_BLE_50
static void bt_mesh_scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    struct bt_mesh_adv_report adv_rpt = {0};

    BT_DBG("%s, event %d", __func__, event);

    if (event == BTA_DM_INQ_RES_EVT) {
        /* TODO: How to process scan response here? PS: p_data->inq_res.scan_rsp_len */

        adv_rpt.addr.type = p_data->inq_res.ble_addr_type;
        adv_rpt.rssi = p_data->inq_res.rssi;
        adv_rpt.adv_type = p_data->inq_res.ble_evt_type;

        memcpy(adv_rpt.addr.val, p_data->inq_res.bd_addr, BLE_MESH_ADDR_LEN);

        net_buf_simple_init_with_data(&adv_rpt.adv_data, p_data->inq_res.p_eir, p_data->inq_res.adv_data_len);

        if (bt_mesh_scan_dev_found_cb) {
            bt_mesh_scan_dev_found_cb(&adv_rpt);

            if (p_data->inq_res.scan_rsp_len) {
                adv_rpt.adv_type = BLE_MESH_ADV_SCAN_RSP;
                net_buf_simple_init_with_data(&adv_rpt.adv_data, p_data->inq_res.p_eir + p_data->inq_res.adv_data_len, p_data->inq_res.scan_rsp_len);
                bt_mesh_scan_dev_found_cb(&adv_rpt);
            }
        }
    } else if (event == BTA_DM_INQ_CMPL_EVT) {
        BT_INFO("Scan completed, number of scan response %d", p_data->inq_cmpl.num_resps);
    } else {
        BT_WARN("Unexpected scan result event %d", event);
    }
}
#endif

#if CONFIG_BLE_MESH_USE_BLE_50
static struct {
    bool set;
    tBTA_DM_BLE_GAP_EXT_ADV_PARAMS param;
} last_param[BLE_MESH_ADV_INST_TYPES_NUM];

int bt_le_ext_adv_start(const uint8_t inst_id,
                        const struct bt_mesh_adv_param *param,
                        const struct bt_mesh_adv_data *ad, size_t ad_len,
                        const struct bt_mesh_adv_data *sd, size_t sd_len)
{
    tBTA_DM_BLE_GAP_EXT_ADV_PARAMS ext_adv_params = {0};
    tBTA_DM_BLE_EXT_ADV ext_adv = {0};
    uint16_t interval = 0U;
    int err = 0;

    assert(param);

#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return -EALREADY;
    }
#endif

    if (!valid_adv_param(param)) {
        BT_ERR("Invalid adv parameters");
        return -EINVAL;
    }

    memset(&ext_adv_params, 0, sizeof(tBTA_DM_BLE_GAP_EXT_ADV_PARAMS));

    if (param->options & BLE_MESH_ADV_OPT_CONNECTABLE) {
        ext_adv_params.type = BTA_DM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_IND;
    } else if (sd != NULL) {
        ext_adv_params.type = BTA_DM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_SCAN;
    } else {
        if (param->primary_phy == BLE_MESH_ADV_PHY_1M &&
            param->secondary_phy == BLE_MESH_ADV_PHY_1M &&
            param->include_tx_power == false &&
            ad->data_len <= 29) {
                ext_adv_params.type = BTA_DM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_NONCONN;
        }
    }

#if CONFIG_BLE_MESH_PRB_SRV
    /* NOTE: When a Mesh Private beacon is advertised, the Mesh Private beacon shall
     * use a resolvable private address or a non-resolvable private address in the
     * AdvA field of the advertising PDU.
     */
    if (ad->type == BLE_MESH_DATA_MESH_BEACON && ad->data[0] == BEACON_TYPE_PRIVATE) {
        ext_adv_params.own_addr_type = BLE_MESH_ADDR_RANDOM;
    } else {
        ext_adv_params.own_addr_type = BLE_MESH_ADDR_PUBLIC;
    }
#else
    ext_adv_params.own_addr_type = BLE_MESH_ADDR_PUBLIC;
#endif

    ext_adv_params.sid = inst_id;
    ext_adv_params.max_skip = 0;
    ext_adv_params.tx_power = param->tx_power;
    ext_adv_params.scan_req_notif = false;
    ext_adv_params.primary_phy = param->primary_phy;
    ext_adv_params.secondary_phy = param->secondary_phy;
    ext_adv_params.filter_policy = BLE_MESH_AP_SCAN_CONN_ALL;
    ext_adv_params.channel_map = param->channel_map;

    if (param->include_tx_power) {
        ext_adv_params.type |= BTA_DM_BLE_GAP_SET_EXT_ADV_PROP_INCLUDE_TX_PWR;
    }

    interval = param->interval_min;

#if CONFIG_BLE_MESH_RANDOM_ADV_INTERVAL
    /* If non-connectable mesh packets are transmitted with an adv interval
     * not smaller than 10ms, then we will use a random adv interval between
     * [interval / 2, interval] for them.
     */
    if (ext_adv_params.type == BTA_DM_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_NONCONN
        && interval >= 16) {
        interval >>= 1;
        interval += (bt_mesh_get_rand() % (interval + 1));

        BT_DBG("%u->%u", param->interval_min, interval);
    }
#endif

    ext_adv_params.interval_min = interval;
    ext_adv_params.interval_max = interval;

    if (memcmp(&ext_adv_params, &last_param[inst_id].param, sizeof(tBTA_DM_BLE_GAP_EXT_ADV_PARAMS))) {
        if (last_param[inst_id].set) {
            BTA_DmBleGapExtAdvSetRemove(inst_id);
        }

        last_param[inst_id].set = true;

        /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */
        BTA_DmBleGapExtAdvSetParams(inst_id, &ext_adv_params);

        memcpy(&last_param[inst_id].param, &ext_adv_params, sizeof(tBTA_DM_BLE_GAP_EXT_ADV_PARAMS));
    }

    err = set_adv_data(BLE_MESH_HCI_OP_SET_ADV_DATA, inst_id, ad, ad_len);
    if (err) {
        BT_ERR("Failed to set adv data, err %d", err);
        return err;
    }

    /*
     * We need to set SCAN_RSP when enabling advertising type that allows
     * for Scan Requests.
     *
     * If sd was not provided but we enable connectable undirected
     * advertising sd needs to be cleared from values set by previous calls.
     * Clearing sd is done by calling set_adv_data() with NULL data and zero len.
     * So following condition check is unusual but correct.
     */
    if (sd) {
        err = set_adv_data(BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA, inst_id, sd, sd_len);
        if (err) {
            BT_ERR("Failed to set scan rsp data err %d", err);
            return err;
        }
    }

    ext_adv.instance = inst_id;
    ext_adv.duration = param->adv_duration / 10;
    ext_adv.max_events = param->adv_count;

    BTA_DmBleGapExtAdvEnable(true, 1, &ext_adv);

#if BLE_MESH_DEV
    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);

    if (!(param->options & BLE_MESH_ADV_OPT_ONE_TIME)) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    }
#endif

    return 0;
}
#else  /* CONFIG_BLE_MESH_USE_BLE_50 */
/* APIs functions */
int bt_le_adv_start(const struct bt_mesh_adv_param *param,
                    const struct bt_mesh_adv_data *ad, size_t ad_len,
                    const struct bt_mesh_adv_data *sd, size_t sd_len)
{
    tBTA_START_ADV_CMPL_CBACK *p_start_adv_cb = NULL;
    tBLE_ADDR_TYPE addr_type_own = 0U;
    tBLE_BD_ADDR p_dir_bda = {0};
    tBTM_BLE_AFP adv_fil_pol = 0U;
    uint16_t interval = 0U;
    uint8_t adv_type = 0U;
    int err = 0;

#if BLE_MESH_DEV
    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return -EALREADY;
    }
#endif

    if (!valid_adv_param(param)) {
        BT_ERR("Invalid adv parameters");
        return -EINVAL;
    }

    err = set_adv_data(BLE_MESH_HCI_OP_SET_ADV_DATA, ad, ad_len);
    if (err) {
        BT_ERR("Failed to set adv data, err %d", err);
        return err;
    }

    /*
     * We need to set SCAN_RSP when enabling advertising type that allows
     * for Scan Requests.
     *
     * If sd was not provided but we enable connectable undirected
     * advertising sd needs to be cleared from values set by previous calls.
     * Clearing sd is done by calling set_adv_data() with NULL data and zero len.
     * So following condition check is unusual but correct.
     */
    if (sd) {
        err = set_adv_data(BLE_MESH_HCI_OP_SET_SCAN_RSP_DATA, sd, sd_len);
        if (err) {
            BT_ERR("Failed to set scan rsp data, err %d", err);
            return err;
        }
    }

    if (param->options & BLE_MESH_ADV_OPT_CONNECTABLE) {
        adv_type = BLE_MESH_ADV_IND;
    } else if (sd != NULL) {
        adv_type = BLE_MESH_ADV_SCAN_IND;
    } else {
        adv_type = BLE_MESH_ADV_NONCONN_IND;
    }

#if CONFIG_BLE_MESH_PRB_SRV
    /* NOTE: When a Mesh Private beacon is advertised, the Mesh Private beacon shall
     * use a resolvable private address or a non-resolvable private address in the
     * AdvA field of the advertising PDU.
     */
    if (ad->type == BLE_MESH_DATA_MESH_BEACON && ad->data[0] == BEACON_TYPE_PRIVATE) {
        addr_type_own = BLE_MESH_ADDR_RANDOM;
    } else {
        addr_type_own = BLE_MESH_ADDR_PUBLIC;
    }
#else
    addr_type_own = BLE_MESH_ADDR_PUBLIC;
#endif

    adv_fil_pol = BLE_MESH_AP_SCAN_CONN_ALL;
    p_start_adv_cb = start_adv_completed_cb;

    interval = param->interval_min;

#if CONFIG_BLE_MESH_RANDOM_ADV_INTERVAL
    /* If non-connectable mesh packets are transmitted with an adv interval
     * not smaller than 10ms, then we will use a random adv interval between
     * [interval / 2, interval] for them.
     */
    if (adv_type == BLE_MESH_ADV_NONCONN_IND && interval >= 16) {
        interval >>= 1;
        interval += (bt_mesh_get_rand() % (interval + 1));

        BT_INFO("%u->%u", param->interval_min, interval);
    }
#endif

    /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */
    BLE_MESH_BTM_CHECK_STATUS(
        BTM_BleSetAdvParamsAll(interval, interval, adv_type,
                               addr_type_own, &p_dir_bda,
                               param->channel_map, adv_fil_pol, p_start_adv_cb));
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleStartAdv());

#if BLE_MESH_DEV
    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);

    if (!(param->options & BLE_MESH_ADV_OPT_ONE_TIME)) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    }
#endif

    return 0;
}
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
#if CONFIG_BLE_MESH_USE_BLE_50
#if !CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
static bool _ble_adv_running_flag;

static inline void bt_mesh_set_ble_adv_running()
{
    _ble_adv_running_flag = true;
}

static inline void bt_mesh_unset_ble_adv_running()
{
    _ble_adv_running_flag = false;
}

static inline bool bt_mesh_is_ble_adv_running()
{
    return _ble_adv_running_flag == true;
}
#endif /* !CONFIG_BLE_MESH_SUPPORT_MULTI_ADV */

int bt_mesh_ble_ext_adv_start(const uint8_t inst_id,
                              const struct bt_mesh_ble_adv_param *param,
                              const struct bt_mesh_ble_adv_data *data)
{
    tBTA_DM_BLE_GAP_EXT_ADV_PARAMS ext_adv_params = {0};
    tBTA_DM_BLE_EXT_ADV ext_adv = {0};
    struct bt_mesh_hci_cp_set_adv_data set = {0};

    switch (param->adv_type) {
    case BLE_MESH_ADV_IND:
    case BLE_MESH_ADV_DIRECT_IND:
    case BLE_MESH_ADV_SCAN_IND:
    case BLE_MESH_ADV_NONCONN_IND:
    case BLE_MESH_ADV_SCAN_RSP:
        ext_adv_params.type = param->adv_type;
        break;
    default:
        BT_ERR("Unsupported adv type %d", param->adv_type);
        return -EINVAL;
    }

    ext_adv_params.max_skip = 0;
    ext_adv_params.tx_power = 0x7F;
    ext_adv_params.sid = inst_id;
    ext_adv_params.scan_req_notif = false;
    ext_adv_params.own_addr_type = param->own_addr_type;
    ext_adv_params.interval_min = param->interval;
    ext_adv_params.interval_max = param->interval;
    ext_adv_params.primary_phy = BLE_MESH_ADV_PHY_1M;
    ext_adv_params.secondary_phy = BLE_MESH_ADV_PHY_1M;
    ext_adv_params.filter_policy = BLE_MESH_AP_SCAN_CONN_ALL;
    ext_adv_params.channel_map = BLE_MESH_ADV_CHAN_37 | BLE_MESH_ADV_CHAN_38 | BLE_MESH_ADV_CHAN_39;

    if (param->own_addr_type == BLE_MESH_ADDR_PUBLIC_ID ||
        param->own_addr_type == BLE_MESH_ADDR_RANDOM_ID ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        ext_adv_params.peer_addr_type = param->peer_addr_type;
        memcpy(ext_adv_params.peer_addr, param->peer_addr, BLE_MESH_ADDR_LEN);
    }

    ext_adv.instance = inst_id;
    ext_adv.duration = param->duration;
    ext_adv.max_events = param->count;

    /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */
    BTA_DmBleGapExtAdvSetParams(inst_id, &ext_adv_params);

    if (data && param->adv_type != BLE_MESH_ADV_DIRECT_IND &&
        param->adv_type != BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        if (data->adv_data_len) {
            set.len = data->adv_data_len;
            memcpy(set.data, data->adv_data, data->adv_data_len);
                BTA_DmBleGapConfigExtAdvDataRaw(false, inst_id, set.len, set.data);
        }
        if (data->scan_rsp_data_len && param->adv_type != BLE_MESH_ADV_NONCONN_IND) {
            set.len = data->scan_rsp_data_len;
            memcpy(set.data, data->scan_rsp_data, data->scan_rsp_data_len);
                BTA_DmBleGapConfigExtAdvDataRaw(true, inst_id, set.len, set.data);
        }
    }

    BTA_DmBleGapExtAdvEnable(true, 1, &ext_adv);

#if !CONFIG_BLE_MESH_SUPPORT_MULTI_ADV
    bt_mesh_set_ble_adv_running();
#endif

    return 0;
}
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
int bt_mesh_ble_adv_start(const struct bt_mesh_ble_adv_param *param,
                          const struct bt_mesh_ble_adv_data *data)
{
    struct bt_mesh_hci_cp_set_adv_data set = {0};
    tBTM_BLE_ADV_CHNL_MAP channel_map = 0U;
    tBTM_BLE_AFP adv_fil_pol = 0U;
    tBLE_BD_ADDR p_dir_bda = {0};

    if (data && param->adv_type != BLE_MESH_ADV_DIRECT_IND &&
        param->adv_type != BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        if (data->adv_data_len) {
            set.len = data->adv_data_len;
            memcpy(set.data, data->adv_data, data->adv_data_len);
            BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteAdvDataRaw(set.data, set.len));
        }
        if (data->scan_rsp_data_len && param->adv_type != BLE_MESH_ADV_NONCONN_IND) {
            set.len = data->scan_rsp_data_len;
            memcpy(set.data, data->scan_rsp_data, data->scan_rsp_data_len);
            BLE_MESH_BTM_CHECK_STATUS(BTM_BleWriteScanRspRaw(set.data, set.len));
        }
    }

    channel_map = BLE_MESH_ADV_CHAN_37 | BLE_MESH_ADV_CHAN_38 | BLE_MESH_ADV_CHAN_39;
    adv_fil_pol = BLE_MESH_AP_SCAN_CONN_ALL;
    if (param->own_addr_type == BLE_MESH_ADDR_PUBLIC_ID ||
        param->own_addr_type == BLE_MESH_ADDR_RANDOM_ID ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND ||
        param->adv_type == BLE_MESH_ADV_DIRECT_IND_LOW_DUTY) {
        p_dir_bda.type = param->peer_addr_type;
        memcpy(p_dir_bda.bda, param->peer_addr, BLE_MESH_ADDR_LEN);
    }

    /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */
    BLE_MESH_BTM_CHECK_STATUS(
        BTM_BleSetAdvParamsAll(param->interval, param->interval, param->adv_type,
                               param->own_addr_type, &p_dir_bda,
                               channel_map, adv_fil_pol, NULL));
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleStartAdv());

    return 0;
}
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */
#endif /* CONFIG_BLE_MESH_SUPPORT_BLE_ADV */

#if CONFIG_BLE_MESH_USE_BLE_50
int bt_le_ext_adv_stop(uint8_t inst_id)
{
    tBTA_DM_BLE_EXT_ADV ext_adv = {0};

#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return 0;
    }
#endif

    ext_adv.instance = inst_id;

    BTA_DmBleGapExtAdvEnable(false, 1, &ext_adv);

#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif

    return 0;
}
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
int bt_le_adv_stop(void)
{
#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_KEEP_ADVERTISING);
    if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING)) {
        return 0;
    }
#endif

    BLE_MESH_BTM_CHECK_STATUS(BTM_BleBroadcast(false, NULL));

#if BLE_MESH_DEV
    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif

    return 0;
}
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

int bt_le_scan_start(const struct bt_mesh_scan_param *param, bt_mesh_scan_cb_t cb)
{
    int err = 0;

    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        BT_INFO("Scan is already started");
        return -EALREADY;
    }

    if (!valid_scan_param(param)) {
        return -EINVAL;
    }

#if BLE_MESH_DEV
    if (param->filter_dup) {
        bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCAN_FILTER_DUP);
    } else {
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCAN_FILTER_DUP);
    }
#endif

    err = start_le_scan(param->type, param->interval, param->window,
                        param->filter_dup, param->scan_fil_policy);
    if (err) {
        return err;
    }

    bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    bt_mesh_scan_dev_found_cb = cb;

    return 0;
}

int bt_le_scan_stop(void)
{
    if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
        BT_INFO("Scan is already stopped");
        return -EALREADY;
    }

#if CONFIG_BLE_MESH_USE_BLE_50
    BTA_DmBleGapExtScan(false, 0 ,0);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
    BLE_MESH_BTM_CHECK_STATUS(BTM_BleScan(false, 0, NULL, NULL, NULL));
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

    bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    bt_mesh_scan_dev_found_cb = NULL;

    return 0;
}

#if CONFIG_BLE_MESH_TEST_USE_WHITE_LIST
int bt_le_update_white_list(struct bt_mesh_white_list *wl)
{
    if (wl == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (BTM_BleUpdateAdvWhitelist(wl->add_remove, wl->remote_bda,
        wl->addr_type, (tBTM_UPDATE_WHITELIST_CBACK *)wl->update_wl_comp_cb) == false) {
        return -EIO;
    }

    return 0;
}
#endif

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
static void bt_mesh_bta_gatts_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    switch (event) {
    case BTA_GATTS_REG_EVT:
        if (p_data->reg_oper.status == BTA_GATT_OK) {
            bt_mesh_gatts_if = p_data->reg_oper.server_if;
            future_ready(gatts_future_mesh, FUTURE_SUCCESS);
        } else {
            future_ready(gatts_future_mesh, FUTURE_FAIL);
        }
        break;
    case BTA_GATTS_READ_EVT: {
        struct bt_mesh_gatt_attr *attr = bt_mesh_gatts_find_attr_by_handle(p_data->req_data.p_data->read_req.handle);
        uint8_t index = BLE_MESH_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        tBTA_GATTS_RSP rsp = {0};
        uint8_t buf[100] = {0};
        uint16_t len = 0;

        BT_DBG("gatts read, handle %d", p_data->req_data.p_data->read_req.handle);

        if (attr != NULL && attr->read != NULL && index < ARRAY_SIZE(bt_mesh_gatts_conn)) {
            if ((len = attr->read(&bt_mesh_gatts_conn[index], attr, buf, 100,
                                  p_data->req_data.p_data->read_req.offset)) > 0) {
                rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
                rsp.attr_value.len = len;
                memcpy(&rsp.attr_value.value[0], buf, len);
                BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                  p_data->req_data.status, &rsp);
                BT_DBG("Send gatts read rsp, handle %d", attr->handle);
            } else {
                BT_WARN("Mesh gatts read failed");
            }
        }
        break;
    }
    case BTA_GATTS_WRITE_EVT: {
        struct bt_mesh_gatt_attr *attr = bt_mesh_gatts_find_attr_by_handle(p_data->req_data.p_data->write_req.handle);
        uint8_t index = BLE_MESH_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        uint16_t len = 0;

        BT_DBG("gatts write, handle %d, len %d, data %s", p_data->req_data.p_data->write_req.handle,
               p_data->req_data.p_data->write_req.len,
               bt_hex(p_data->req_data.p_data->write_req.value, p_data->req_data.p_data->write_req.len));

        if (attr != NULL && attr->write != NULL && index < ARRAY_SIZE(bt_mesh_gatts_conn)) {
            if ((len = attr->write(&bt_mesh_gatts_conn[index], attr,
                                   p_data->req_data.p_data->write_req.value,
                                   p_data->req_data.p_data->write_req.len,
                                   p_data->req_data.p_data->write_req.offset, 0)) > 0) {
                if (p_data->req_data.p_data->write_req.need_rsp) {
                    BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                      p_data->req_data.status, NULL);
                    BT_DBG("Send gatts write rsp, handle %d", attr->handle);
                }
            }
        }
        break;
    }
    case BTA_GATTS_EXEC_WRITE_EVT:
        break;
    case BTA_GATTS_MTU_EVT:
        break;
    case BTA_GATTS_CONF_EVT:
        break;
    case BTA_GATTS_CREATE_EVT:
        svc_handle = p_data->create.service_id;
        BT_DBG("svc_handle %d, gatts_future_mesh %p", svc_handle, gatts_future_mesh);
        if (gatts_future_mesh != NULL) {
            future_ready(gatts_future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_ADD_INCL_SRVC_EVT:
        svc_handle = p_data->add_result.attr_id;
        if (gatts_future_mesh != NULL) {
            future_ready(gatts_future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_ADD_CHAR_EVT:
        char_handle = p_data->add_result.attr_id;
        if (gatts_future_mesh != NULL) {
            future_ready(gatts_future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        char_handle = p_data->add_result.attr_id;
        if (gatts_future_mesh != NULL) {
            future_ready(gatts_future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_DELELTE_EVT:
        break;
    case BTA_GATTS_START_EVT:
        break;
    case BTA_GATTS_STOP_EVT:
        break;
    case BTA_GATTS_CONNECT_EVT:
#if BLE_MESH_DEV
        /* When connection is created, advertising will be stopped automatically. */
        bt_mesh_atomic_test_and_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
        /* Check if this connection is created by Proxy client */
        for (size_t i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (!memcmp(bt_mesh_gattc_info[i].addr.val, p_data->conn.remote_bda, BLE_MESH_ADDR_LEN)) {
                BT_WARN("Already create connection with %s by proxy client",
                        bt_hex(p_data->conn.remote_bda, BLE_MESH_ADDR_LEN));
                return;
            }
        }
#endif
        if (bt_mesh_gatts_conn_cb != NULL && bt_mesh_gatts_conn_cb->connected != NULL) {
            uint8_t index = BLE_MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
            if (index < BLE_MESH_MAX_CONN) {
                bt_mesh_gatts_conn[index].handle = BLE_MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
                (bt_mesh_gatts_conn_cb->connected)(&bt_mesh_gatts_conn[index], 0);
            }
            memcpy(bt_mesh_gatts_addr, p_data->conn.remote_bda, BLE_MESH_ADDR_LEN);
            /* This is for EspBleMesh Android app. When it tries to connect with the
             * device at the first time and it fails due to some reason. And after
             * the second connection, the device needs to send GATT service change
             * indication to the phone manually to notify it discovering service again.
             */
            BTA_GATTS_SendServiceChangeIndication(bt_mesh_gatts_if, bt_mesh_gatts_addr);
        }
        break;
    case BTA_GATTS_DISCONNECT_EVT:
#if BLE_MESH_DEV
        bt_mesh_atomic_test_and_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_ADVERTISING);
#endif
        if (bt_mesh_gatts_conn_cb != NULL && bt_mesh_gatts_conn_cb->disconnected != NULL) {
            uint8_t index = BLE_MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
            if (index < BLE_MESH_MAX_CONN) {
                bt_mesh_gatts_conn[index].handle = BLE_MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
                (bt_mesh_gatts_conn_cb->disconnected)(&bt_mesh_gatts_conn[index], p_data->conn.reason);
            }
            memset(bt_mesh_gatts_addr, 0x0, BLE_MESH_ADDR_LEN);
        }
        break;
    case BTA_GATTS_CLOSE_EVT:
        break;
    default:
        break;
    }
}

void bt_mesh_gatts_conn_cb_register(struct bt_mesh_conn_cb *cb)
{
    bt_mesh_gatts_conn_cb = cb;
}

void bt_mesh_gatts_conn_cb_deregister(void)
{
    bt_mesh_gatts_conn_cb = NULL;
}

static struct bt_mesh_gatt_attr *bt_mesh_gatts_find_attr_by_handle(uint16_t handle)
{
    struct bt_mesh_gatt_service *svc = NULL;
    struct bt_mesh_gatt_attr *attr = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER(&bt_mesh_gatts_db, svc, node) {
        int i;

        for (i = 0; i < svc->attr_count; i++) {
            attr = &svc->attrs[i];
            /* Check the attrs handle is equal to the handle or not */
            if (attr->handle == handle) {
                return attr;
            }
        }
    }

    return NULL;
}

static void bt_mesh_gatts_foreach_attr(uint16_t start_handle, uint16_t end_handle,
                                       bt_mesh_gatt_attr_func_t func, void *user_data)
{
    struct bt_mesh_gatt_service *svc = NULL;

    SYS_SLIST_FOR_EACH_CONTAINER(&bt_mesh_gatts_db, svc, node) {
        int i;

        for (i = 0; i < svc->attr_count; i++) {
            struct bt_mesh_gatt_attr *attr = &svc->attrs[i];

            /* Check if attribute handle is within range */
            if (attr->handle < start_handle ||
                    attr->handle > end_handle) {
                continue;
            }

            if (func(attr, user_data) == BLE_MESH_GATT_ITER_STOP) {
                return;
            }
        }
    }
}

static uint8_t find_next(const struct bt_mesh_gatt_attr *attr, void *user_data)
{
    struct bt_mesh_gatt_attr **next = user_data;

    *next = (struct bt_mesh_gatt_attr *)attr;

    return BLE_MESH_GATT_ITER_STOP;
}

static struct bt_mesh_gatt_attr *bt_mesh_gatts_attr_next(const struct bt_mesh_gatt_attr *attr)
{
    struct bt_mesh_gatt_attr *next = NULL;

    bt_mesh_gatts_foreach_attr(attr->handle + 1, attr->handle + 1, find_next, &next);

    return next;
}

ssize_t bt_mesh_gatts_attr_read(struct bt_mesh_conn *conn,
                                const struct bt_mesh_gatt_attr *attr,
                                void *buf, uint16_t buf_len, uint16_t offset,
                                const void *value, uint16_t value_len)
{
    uint16_t len = 0U;

    if (offset > value_len) {
        return BLE_MESH_GATT_ERR(BLE_MESH_ATT_ERR_INVALID_OFFSET);
    }

    len = MIN(buf_len, value_len - offset);

    BT_DBG("handle 0x%04x offset %u length %u", attr->handle, offset, len);

    memcpy(buf, value + offset, len);

    return len;
}

struct gatts_incl {
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid16;
} __attribute__((packed));

ssize_t bt_mesh_gatts_attr_read_included(struct bt_mesh_conn *conn,
                                         const struct bt_mesh_gatt_attr *attr,
                                         void *buf, uint16_t len, uint16_t offset)
{
    struct bt_mesh_gatt_attr *incl = attr->user_data;
    struct bt_mesh_gatt_attr *next_svc_start = NULL;
    struct bt_mesh_uuid *uuid = NULL;
    struct gatts_incl pdu = {0};
    uint8_t value_len = 0U;

    assert(incl);

    /* First attr points to the start handle */
    pdu.start_handle = sys_cpu_to_le16(incl->handle);
    next_svc_start = bt_mesh_gatts_attr_next(incl);
    pdu.end_handle = sys_cpu_to_le16(next_svc_start ? next_svc_start->handle - 1 : incl->handle);
    value_len = sizeof(pdu.start_handle) + sizeof(pdu.end_handle);

    /*
     * Core 4.2, Vol 3, Part G, 3.2,
     * The Service UUID shall only be present when the UUID is a 16-bit Bluetooth UUID.
     */
    uuid = incl->user_data;

    if (uuid->type == BLE_MESH_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(uuid)->val);
        value_len += sizeof(pdu.uuid16);
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

ssize_t bt_mesh_gatts_attr_read_service(struct bt_mesh_conn *conn,
                                        const struct bt_mesh_gatt_attr *attr,
                                        void *buf, uint16_t len, uint16_t offset)
{
    struct bt_mesh_uuid *uuid = attr->user_data;

    if (uuid->type == BLE_MESH_UUID_TYPE_16) {
        uint16_t uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(uuid)->val);

        return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &uuid16, 2);
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset,
                                   BLE_MESH_UUID_128(uuid)->val, 16);
}

struct gatts_chrc {
    uint8_t  properties;
    uint16_t value_handle;
    union {
        uint16_t uuid16;
        uint8_t  uuid[16];
    };
} __attribute__((packed));

ssize_t bt_mesh_gatts_attr_read_chrc(struct bt_mesh_conn *conn,
                                     const struct bt_mesh_gatt_attr *attr,
                                     void *buf, uint16_t len, uint16_t offset)
{
    struct bt_mesh_gatt_char *chrc = attr->user_data;
    const struct bt_mesh_gatt_attr *next = NULL;
    struct gatts_chrc pdu = {0};
    uint8_t value_len = 0U;

    pdu.properties = chrc->properties;
    /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part G] page 534:
     * 3.3.2 Characteristic Value Declaration
     * The Characteristic Value declaration contains the value of the
     * characteristic. It is the first Attribute after the characteristic
     * declaration. All characteristic definitions shall have a
     * Characteristic Value declaration.
     */
    next = bt_mesh_gatts_attr_next(attr);
    if (!next) {
        BT_WARN("No value for characteristic, handle 0x%04x", attr->handle);
        pdu.value_handle = 0x0000;
    } else {
        pdu.value_handle = sys_cpu_to_le16(next->handle);
    }
    value_len = sizeof(pdu.properties) + sizeof(pdu.value_handle);

    if (chrc->uuid->type == BLE_MESH_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BLE_MESH_UUID_16(chrc->uuid)->val);
        value_len += 2;
    } else {
        memcpy(pdu.uuid, BLE_MESH_UUID_128(chrc->uuid)->val, 16);
        value_len += 16;
    }

    return bt_mesh_gatts_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

static void bta_uuid_to_bt_mesh_uuid(tBT_UUID *bta_uuid, const struct bt_mesh_uuid  *uuid)
{
    assert(uuid != NULL && bta_uuid != NULL);

    if (uuid->type == BLE_MESH_UUID_TYPE_16) {
        bta_uuid->len = LEN_UUID_16;
        bta_uuid->uu.uuid16 = BLE_MESH_UUID_16(uuid)->val;
    } else if (uuid->type == BLE_MESH_UUID_TYPE_32) {
        bta_uuid->len = LEN_UUID_32;
        bta_uuid->uu.uuid32 = BLE_MESH_UUID_32(uuid)->val;
    } else if (uuid->type == BLE_MESH_UUID_TYPE_128) {
        bta_uuid->len = LEN_UUID_128;
        memcpy(bta_uuid->uu.uuid128, BLE_MESH_UUID_128(uuid)->val, LEN_UUID_128);
    } else {
        BT_ERR("Invalid mesh uuid type %d", uuid->type);
    }
}

static int gatts_register(struct bt_mesh_gatt_service *svc)
{
    struct bt_mesh_gatt_service *last = NULL;
    uint16_t handle = 0U;

    if (sys_slist_is_empty(&bt_mesh_gatts_db)) {
        handle = 0;
        goto populate;
    }

    last = SYS_SLIST_PEEK_TAIL_CONTAINER(&bt_mesh_gatts_db, last, node);
    handle = last->attrs[last->attr_count - 1].handle;
    BT_DBG("gatts register, handle %d", handle);

    ((void) handle);

populate:
    sys_slist_append(&bt_mesh_gatts_db, &svc->node);
    return 0;
}

static int gatts_deregister(struct bt_mesh_gatt_service *svc)
{
    if (sys_slist_is_empty(&bt_mesh_gatts_db)) {
        return 0;
    }

    sys_slist_find_and_remove(&bt_mesh_gatts_db, &svc->node);
    return 0;
}

static tBTA_GATT_PERM bt_mesh_perm_to_bta_perm(uint8_t perm)
{
    tBTA_GATT_PERM bta_perm = 0;

    if ((perm & BLE_MESH_GATT_PERM_READ) == BLE_MESH_GATT_PERM_READ) {
        bta_perm |= BTA_GATT_PERM_READ;
    }

    if ((perm & BLE_MESH_GATT_PERM_WRITE) == BLE_MESH_GATT_PERM_WRITE) {
        bta_perm |= BTA_GATT_PERM_WRITE;
    }

    if ((perm & BLE_MESH_GATT_PERM_READ_ENCRYPT) ==  BLE_MESH_GATT_PERM_READ_ENCRYPT) {
        bta_perm |= BTA_GATT_PERM_READ_ENCRYPTED;
    }

    if ((perm & BLE_MESH_GATT_PERM_WRITE_ENCRYPT) == BLE_MESH_GATT_PERM_WRITE_ENCRYPT) {
        bta_perm |= BTA_GATT_PERM_WRITE_ENCRYPTED;
    }

    if ((perm & BLE_MESH_GATT_PERM_READ_AUTHEN) == BLE_MESH_GATT_PERM_READ_AUTHEN) {
        bta_perm |= BTA_GATT_PERM_READ_ENC_MITM;
    }

    if ((perm & BLE_MESH_GATT_PERM_WRITE_AUTHEN) == BLE_MESH_GATT_PERM_WRITE_AUTHEN) {
        bta_perm |= BTA_GATT_PERM_WRITE_ENC_MITM;
    }

    return bta_perm;
}

int bt_mesh_gatts_service_register(struct bt_mesh_gatt_service *svc)
{
    bool service_created = false;
    tBT_UUID bta_uuid = {0};
    int service_idx = -1;

    assert(svc != NULL);

    for (int i = 0; i < svc->attr_count; i++) {
        if (svc->attrs[i].uuid->type == BLE_MESH_UUID_TYPE_16) {
            switch (BLE_MESH_UUID_16(svc->attrs[i].uuid)->val) {
            case BLE_MESH_UUID_GATT_PRIMARY_VAL: {
                gatts_future_mesh = future_new();
                bta_uuid_to_bt_mesh_uuid(&bta_uuid, (struct bt_mesh_uuid *)svc->attrs[i].user_data);
                BTA_GATTS_CreateService(bt_mesh_gatts_if,
                                        &bta_uuid, 0, svc->attr_count, true);
                if (future_await(gatts_future_mesh) == FUTURE_FAIL) {
                    BT_ERR("Failed to add primary service");
                    goto cleanup;
                }
                service_created = true;
                service_idx = i;
                svc->attrs[i].handle = svc_handle;
                BT_DBG("Add primary service, uuid 0x%04x, perm %d, handle %d",
                        bta_uuid.uu.uuid16, svc->attrs[i].perm, svc_handle);
                break;
            }
            case BLE_MESH_UUID_GATT_SECONDARY_VAL: {
                gatts_future_mesh = future_new();
                bta_uuid_to_bt_mesh_uuid(&bta_uuid, (struct bt_mesh_uuid *)svc->attrs[i].user_data);
                BTA_GATTS_CreateService(bt_mesh_gatts_if,
                                        &bta_uuid, 0, svc->attr_count, false);
                if (future_await(gatts_future_mesh) == FUTURE_FAIL) {
                    BT_ERR("Failed to add secondary service");
                    goto cleanup;
                }
                service_created = true;
                service_idx = i;
                svc->attrs[i].handle = svc_handle;
                BT_DBG("Add secondary service, uuid 0x%04x, perm %d, handle %d",
                        bta_uuid.uu.uuid16, svc->attrs[i].perm, svc_handle);
                break;
            }
            case BLE_MESH_UUID_GATT_INCLUDE_VAL: {
                break;
            }
            case BLE_MESH_UUID_GATT_CHRC_VAL: {
                gatts_future_mesh = future_new();
                struct bt_mesh_gatt_char *gatts_chrc = (struct bt_mesh_gatt_char *)svc->attrs[i].user_data;
                bta_uuid_to_bt_mesh_uuid(&bta_uuid, gatts_chrc->uuid);
                BTA_GATTS_AddCharacteristic(svc_handle, &bta_uuid, bt_mesh_perm_to_bta_perm(svc->attrs[i + 1].perm), gatts_chrc->properties, NULL, NULL);
                if (future_await(gatts_future_mesh) == FUTURE_FAIL) {
                    BT_ERR("Failed to add characteristic");
                    goto cleanup;
                }
                /* All the characteristic should have two handles: the declaration handle and the value handle */
                svc->attrs[i].handle = char_handle - 1;
                svc->attrs[i + 1].handle =  char_handle;
                BT_DBG("Add characteristic, uuid 0x%04x, handle %d, perm %d, properties %d",
                       BLE_MESH_UUID_16(gatts_chrc->uuid)->val, char_handle, svc->attrs[i + 1].perm, gatts_chrc->properties);
                break;
            }
            case BLE_MESH_UUID_GATT_CEP_VAL:
            case BLE_MESH_UUID_GATT_CUD_VAL:
            case BLE_MESH_UUID_GATT_CCC_VAL:
            case BLE_MESH_UUID_GATT_SCC_VAL:
            case BLE_MESH_UUID_GATT_CPF_VAL:
            case BLE_MESH_UUID_VALID_RANGE_VAL:
            case BLE_MESH_UUID_HIDS_EXT_REPORT_VAL:
            case BLE_MESH_UUID_HIDS_REPORT_REF_VAL:
            case BLE_MESH_UUID_ES_CONFIGURATION_VAL:
            case BLE_MESH_UUID_ES_MEASUREMENT_VAL:
            case BLE_MESH_UUID_ES_TRIGGER_SETTING_VAL: {
                gatts_future_mesh = future_new();
                bta_uuid_to_bt_mesh_uuid(&bta_uuid, svc->attrs[i].uuid);
                BTA_GATTS_AddCharDescriptor(svc_handle, bt_mesh_perm_to_bta_perm(svc->attrs[i].perm), &bta_uuid, NULL, NULL);
                if (future_await(gatts_future_mesh) == FUTURE_FAIL) {
                    BT_ERR("Failed to add descriptor");
                    goto cleanup;
                }
                svc->attrs[i].handle = char_handle;
                BT_DBG("Add descriptor, uuid 0x%04x, perm %d, handle %d",
                       BLE_MESH_UUID_16(svc->attrs[i].uuid)->val, svc->attrs[i].perm, char_handle);
                break;
            }
            default:
                break;
            }
        }
    }

    if (svc_handle != 0) {
        svc_handle = 0;
    }

    gatts_register(svc);
    return 0;

cleanup:
    if (service_created && service_idx >= 0) {
        BTA_GATTS_DeleteService(svc->attrs[service_idx].handle);
        svc->attrs[service_idx].handle = 0;
    }
    return -EIO;
}

int bt_mesh_gatts_service_deregister(struct bt_mesh_gatt_service *svc)
{
    assert(svc != NULL);

    gatts_deregister(svc);

    BTA_GATTS_DeleteService(svc->attrs[0].handle);

    return 0;
}

int bt_mesh_gatts_disconnect(struct bt_mesh_conn *conn, uint8_t reason)
{
    UNUSED(reason);
    uint16_t conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gatts_if, conn->handle);
    BTA_GATTS_Close(conn_id);
    return 0;
}

int bt_mesh_gatts_service_unregister(struct bt_mesh_gatt_service *svc)
{
    assert(svc != NULL);

    BTA_GATTS_DeleteService(svc->attrs[0].handle);
    return 0;
}

int bt_mesh_gatts_notify(struct bt_mesh_conn *conn,
                         const struct bt_mesh_gatt_attr *attr,
                         const void *data, uint16_t len)
{
    uint16_t conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gatts_if, conn->handle);
    BTA_GATTS_HandleValueIndication(conn_id, attr->handle, len, (uint8_t *)data, false);
    return 0;
}

uint16_t bt_mesh_gatt_get_mtu(struct bt_mesh_conn *conn)
{
    return BTA_GATT_GetLocalMTU();
}

/* APIs added by Espressif */
int bt_mesh_gatts_service_stop(struct bt_mesh_gatt_service *svc)
{
    if (!svc) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("Stop service:%d", svc->attrs[0].handle);

    BTA_GATTS_StopService(svc->attrs[0].handle);
    return 0;
}

int bt_mesh_gatts_service_start(struct bt_mesh_gatt_service *svc)
{
    struct bt_mesh_uuid_16 *uuid_16 = NULL;
    struct bt_mesh_uuid *uuid = NULL;

    if (!svc) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("Start service:%d", svc->attrs[0].handle);

    BTA_GATTS_StartService(svc->attrs[0].handle, BTA_GATT_TRANSPORT_LE);

    /* For EspBleMesh Android app, it does not disconnect after provisioning
     * is done, and hence we send GATT service change indication manually
     * when Mesh Proxy Service is started after provisioning.
     */
    uuid = (struct bt_mesh_uuid *)svc->attrs[0].user_data;
    if (uuid && uuid->type == BLE_MESH_UUID_TYPE_16) {
        uuid_16 = (struct bt_mesh_uuid_16 *)uuid;
        BT_DBG("service start, type 0x%02x, val 0x%04x", uuid_16->uuid.type, uuid_16->val);
        if (uuid_16->val == BLE_MESH_UUID_MESH_PROXY_VAL) {
            BTA_GATTS_SendServiceChangeIndication(bt_mesh_gatts_if, bt_mesh_gatts_addr);
        }
    }

    return 0;
}

int bt_mesh_gatts_set_local_device_name(const char *name)
{
    tBTM_STATUS status = BTM_SUCCESS;

    status = BTM_SetLocalDeviceName((char *)name, BT_DEVICE_TYPE_BLE);
    if (status != BTM_NO_RESOURCES) {
        BT_ERR("SetLocalDevNameFail[%d]", status);
        return -EIO;
    }

    return 0;
}

#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
void bt_mesh_gattc_conn_cb_register(struct bt_mesh_prov_conn_cb *cb)
{
    bt_mesh_gattc_conn_cb = cb;
}

void bt_mesh_gattc_conn_cb_deregister(void)
{
    bt_mesh_gattc_conn_cb = NULL;
}

uint8_t bt_mesh_gattc_get_free_conn_count(void)
{
    uint8_t count = 0U;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (bt_mesh_gattc_info[i].conn.handle == 0xFFFF &&
            bt_mesh_gattc_info[i].service_uuid == 0x0000) {
            ++count;
        }
    }

    return count;
}

uint16_t bt_mesh_gattc_get_service_uuid(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            return bt_mesh_gattc_info[i].service_uuid;
        }
    }

    BT_ERR("Conn %p not found", conn);
    return 0;
}

/** For provisioner acting as a GATT client, it may follow the procedures
 *  listed below.
 *  1. Create connection with the unprovisioned device
 *  2. Exchange MTU size
 *  3. Find Mesh Prov Service in the device's service database
 *  4. Find Mesh Prov Data In/Out characteristic within the service
 *  5. Get CCC of Mesh Prov Data Out Characteristic
 *  6. Set the Notification bit of CCC
 */

int bt_mesh_gattc_conn_create(const bt_mesh_addr_t *addr, uint16_t service_uuid)
{
    tBTA_BLE_CONN_PARAMS conn_1m_param = {0};
    uint8_t zero[6] = {0};
    int i;

    if (!addr || !memcmp(addr->val, zero, BLE_MESH_ADDR_LEN) ||
        (addr->type > BLE_ADDR_RANDOM)) {
        BT_ERR("Invalid remote address");
        return -EINVAL;
    }

    if (service_uuid != BLE_MESH_UUID_MESH_PROV_VAL &&
        service_uuid != BLE_MESH_UUID_MESH_PROXY_VAL) {
        BT_ERR("Invalid service uuid 0x%04x", service_uuid);
        return -EINVAL;
    }

    /* Check if already creating connection with the device */
    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (!memcmp(bt_mesh_gattc_info[i].addr.val, addr->val, BLE_MESH_ADDR_LEN)) {
            BT_WARN("Already create connection with %s",
                    bt_hex(addr->val, BLE_MESH_ADDR_LEN));
            return -EALREADY;
        }
    }

    /* Find empty element in queue to store device info */
    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if ((bt_mesh_gattc_info[i].conn.handle == 0xFFFF) &&
            (bt_mesh_gattc_info[i].service_uuid == 0x0000)) {
            memcpy(bt_mesh_gattc_info[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            bt_mesh_gattc_info[i].addr.type = addr->type;
            /* Service to be found after exchanging mtu size */
            bt_mesh_gattc_info[i].service_uuid = service_uuid;
            break;
        }
    }

    if (i == ARRAY_SIZE(bt_mesh_gattc_info)) {
        BT_WARN("gattc info is full");
        return -ENOMEM;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
#if CONFIG_BLE_MESH_USE_BLE_50
        BTA_DmBleGapExtScan(false, 0 ,0);
#else
        BLE_MESH_BTM_CHECK_STATUS(BTM_BleScan(false, 0, NULL, NULL, NULL));
#endif
        bt_mesh_atomic_clear_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
    }

    BT_DBG("Create conn with %s", bt_hex(addr->val, BLE_MESH_ADDR_LEN));

#if CONFIG_BLE_MESH_USE_BLE_50
    /* Min_interval: 15ms
     * Max_interval: 15ms
     * Slave_latency: 0x0
     * Supervision_timeout: 1s
     */
    conn_1m_param.scan_interval = 0x0020;
    conn_1m_param.scan_window   = 0x0020;
    conn_1m_param.interval_min  = 0x18;
    conn_1m_param.interval_max  = 0x18;
    conn_1m_param.latency       = 0;
    conn_1m_param.supervision_timeout = 0x64;
    conn_1m_param.min_ce_len = 0;
    conn_1m_param.max_ce_len = 0;

    BTA_GATTC_Enh_Open(bt_mesh_gattc_if, bt_mesh_gattc_info[i].addr.val,
                       bt_mesh_gattc_info[i].addr.type, true,
                       BTA_GATT_TRANSPORT_LE, TRUE, BLE_ADDR_UNKNOWN_TYPE,
                       false, 0xFF, 0xFF,
                       BTA_BLE_PHY_1M_MASK, &conn_1m_param, NULL, NULL);
#else /* CONFIG_BLE_MESH_USE_BLE_50 */
    /* Min_interval: 15ms
     * Max_interval: 15ms
     * Slave_latency: 0x0
     * Supervision_timeout: 1s
     */
    conn_1m_param.interval_min = 0x18;
    conn_1m_param.interval_max = 0x18;
    conn_1m_param.latency = 0;
    conn_1m_param.supervision_timeout = 0x64;

    BTA_GATTC_Enh_Open(bt_mesh_gattc_if, bt_mesh_gattc_info[i].addr.val,
                       bt_mesh_gattc_info[i].addr.type, true,
                       BTA_GATT_TRANSPORT_LE, FALSE, BLE_ADDR_UNKNOWN_TYPE,
                       false, 0xFF, 0xFF,
                       BTA_BLE_PHY_1M_MASK, &conn_1m_param, NULL, NULL);
#endif /* CONFIG_BLE_MESH_USE_BLE_50 */

    return i;
}

void bt_mesh_gattc_exchange_mtu(uint8_t index)
{
    /** Set local MTU and exchange with GATT server.
     *  ATT_MTU >= 69 for Mesh GATT Prov Service
     *  ATT_NTU >= 33 for Mesh GATT Proxy Service
    */
    uint16_t conn_id = 0U;

    conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gattc_if, bt_mesh_gattc_info[index].conn.handle);

    BTA_GATTC_ConfigureMTU(conn_id);
}

uint16_t bt_mesh_gattc_get_mtu_info(struct bt_mesh_conn *conn)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            return bt_mesh_gattc_info[i].mtu;
        }
    }

    return 0;
}

int bt_mesh_gattc_write_no_rsp(struct bt_mesh_conn *conn,
                               const struct bt_mesh_gatt_attr *attr,
                               const void *data, uint16_t len)
{
    uint16_t conn_id = 0U;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gattc_if, bt_mesh_gattc_info[i].conn.handle);
            BTA_GATTC_WriteCharValue(conn_id, bt_mesh_gattc_info[i].data_in_handle,
                                     BTA_GATTC_TYPE_WRITE_NO_RSP, len,
                                     (uint8_t *)data, BTA_GATT_AUTH_REQ_NONE);
            return 0;
        }
    }

    BT_ERR("Conn %p not found", conn);
    return -EEXIST;
}

void bt_mesh_gattc_disconnect(struct bt_mesh_conn *conn)
{
    /** Disconnect
     *  Clear proper proxy server information
     *  Clear proper prov_link information
     *  Clear proper bt_mesh_gattc_info information
     *  Here in adapter, we just clear proper bt_mesh_gattc_info, and
     *  when proxy_disconnected callback comes, the proxy server
     *  information and prov_link information should be cleared.
     */
    uint16_t conn_id = 0U;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        if (conn == &bt_mesh_gattc_info[i].conn) {
            conn_id = BLE_MESH_GATT_CREATE_CONN_ID(bt_mesh_gattc_if, bt_mesh_gattc_info[i].conn.handle);
            BTA_GATTC_Close(conn_id);
            return;
        }
    }

    BT_ERR("Conn %p not found", conn);
}

/** Mesh Provisioning Service:  0x1827
 *  Mesh Provisioning Data In:  0x2ADB
 *  Mesh Provisioning Data Out: 0x2ADC
 *  Mesh Proxy Service:  0x1828
 *  Mesh Proxy Data In:  0x2ADD
 *  Mesh PROXY Data Out: 0x2ADE
 */
static void bt_mesh_bta_gattc_cb(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    struct bt_mesh_conn *conn = NULL;
    uint16_t handle = 0U;
    ssize_t len = 0;
    int i = 0;

    switch (event) {
    case BTA_GATTC_REG_EVT:
        if (p_data->reg_oper.status == BTA_GATT_OK) {
            uint8_t uuid[16] = { [0 ... 15] = BLE_MESH_GATTC_APP_UUID_BYTE };

            BT_DBG("BTA_GATTC_REG_EVT");

            if (p_data->reg_oper.app_uuid.len == LEN_UUID_128 &&
                !memcmp(p_data->reg_oper.app_uuid.uu.uuid128, uuid, 16)) {
                bt_mesh_gattc_if = p_data->reg_oper.client_if;
                BT_DBG("bt_mesh_gattc_if is %d", bt_mesh_gattc_if);
            }
        }
        break;
    case BTA_GATTC_CFG_MTU_EVT: {
        if (p_data->cfg_mtu.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_CFG_MTU_EVT, cfg_mtu is %d", p_data->cfg_mtu.mtu);

            handle = BLE_MESH_GATT_GET_CONN_ID(p_data->cfg_mtu.conn_id);

            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                if (bt_mesh_gattc_info[i].conn.handle == handle) {
                    bt_mesh_gattc_info[i].mtu = p_data->cfg_mtu.mtu;

                    /* Search Mesh Provisioning Service or Mesh Proxy Service */
                    tBT_UUID service_uuid = {
                        .len = sizeof(bt_mesh_gattc_info[i].service_uuid),
                        .uu.uuid16 = bt_mesh_gattc_info[i].service_uuid,
                    };
                    BTA_GATTC_ServiceSearchRequest(p_data->cfg_mtu.conn_id, &service_uuid);
                    break;
                }
            }
        }
        break;
    }
    case BTA_GATTC_SEARCH_RES_EVT: {
        BT_DBG("BTA_GATTC_SEARCH_RES_EVT");

        handle = BLE_MESH_GATT_GET_CONN_ID(p_data->srvc_res.conn_id);

        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == handle) {
                if (p_data->srvc_res.service_uuid.uuid.len == 2 &&
                        p_data->srvc_res.service_uuid.uuid.uu.uuid16 == bt_mesh_gattc_info[i].service_uuid) {
                    bt_mesh_gattc_info[i].start_handle = p_data->srvc_res.start_handle;
                    bt_mesh_gattc_info[i].end_handle = p_data->srvc_res.end_handle;
                }
                break;
            }
        }
        break;
    }
    case BTA_GATTC_SEARCH_CMPL_EVT: {
        if (p_data->search_cmpl.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_SEARCH_CMPL_EVT");

            handle = BLE_MESH_GATT_GET_CONN_ID(p_data->search_cmpl.conn_id);

            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                if (bt_mesh_gattc_info[i].conn.handle == handle) {
                    conn = &bt_mesh_gattc_info[i].conn;
                    break;
                }
            }

            if (conn == NULL) {
                BT_ERR("Conn handle 0x%04x not found", handle);
                return;
            }

            if (bt_mesh_gattc_info[i].start_handle == 0x00 ||
                    bt_mesh_gattc_info[i].end_handle == 0x00 ||
                    (bt_mesh_gattc_info[i].start_handle > bt_mesh_gattc_info[i].end_handle)) {
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            uint16_t notify_en = BLE_MESH_GATT_CCC_NOTIFY;
            btgatt_db_element_t *result = NULL;
            tBT_UUID char_uuid = {0};
            tBTA_GATT_STATUS status = 0U;
            tBTA_GATT_UNFMT write = {0};
            uint16_t count = 0U;
            uint16_t num = 0;

            /* Get the characteristic num within Mesh Provisioning/Proxy Service */
            BTA_GATTC_GetDBSizeByType(p_data->search_cmpl.conn_id, BTGATT_DB_CHARACTERISTIC,
                                      bt_mesh_gattc_info[i].start_handle, bt_mesh_gattc_info[i].end_handle,
                                      BTA_GATTC_INVALID_HANDLE, &count);
            if (count != 3 && count != 2) {
                BT_ERR("Invalid characteristic num(%d) within Mesh Provisioning/Proxy Service", count);
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            /* Get Mesh Provisioning/Proxy Data In/Out Characteristic */
            for (int j = 0; j != 2; j++) {
                /** First:  find Mesh Provisioning/Proxy Data In Characteristic
                 *  Second: find Mesh Provisioning/Proxy Data Out Characteristic
                 */
                char_uuid.len = 2;
                if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                    char_uuid.uu.uuid16 = BLE_MESH_UUID_MESH_PROV_DATA_IN_VAL + j;
                } else if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROXY_VAL) {
                    char_uuid.uu.uuid16 = BLE_MESH_UUID_MESH_PROXY_DATA_IN_VAL + j;
                }

                BTA_GATTC_GetCharByUUID(p_data->search_cmpl.conn_id, bt_mesh_gattc_info[i].start_handle,
                                        bt_mesh_gattc_info[i].end_handle, char_uuid, &result, &num);

                if (!result) {
                    bt_mesh_gattc_disconnect(conn);
                    return;
                }

                if (num != 1) {
                    bt_mesh_free(result);
                    bt_mesh_gattc_disconnect(conn);
                    return;
                }

                if (!j) {
                    if (!(result[0].properties & BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP)) {
                        bt_mesh_free(result);
                        bt_mesh_gattc_disconnect(conn);
                        return;
                    }
                    bt_mesh_gattc_info[i].data_in_handle = result[0].attribute_handle;
                } else {
                    if (!(result[0].properties & BLE_MESH_GATT_CHRC_NOTIFY)) {
                        bt_mesh_free(result);
                        bt_mesh_gattc_disconnect(conn);
                        return;
                    }
                    bt_mesh_gattc_info[i].data_out_handle = result[0].attribute_handle;
                }

                bt_mesh_free(result);
                result = NULL;
            }

            /* Register Notification for Mesh Provisioning/Proxy Data Out Characteristic */
            status = BTA_GATTC_RegisterForNotifications(bt_mesh_gattc_if, bt_mesh_gattc_info[i].addr.val,
                     bt_mesh_gattc_info[i].data_out_handle);
            if (status != BTA_GATT_OK) {
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            /** After notification is registered, get descriptor number of the
             *  Mesh Provisioning/Proxy Data Out Characteristic
             */
            BTA_GATTC_GetDBSizeByType(p_data->search_cmpl.conn_id, BTGATT_DB_DESCRIPTOR,
                                      bt_mesh_gattc_info[i].start_handle, bt_mesh_gattc_info[i].end_handle,
                                      bt_mesh_gattc_info[i].data_out_handle, &num);
            if (!num) {
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            /* Get CCC of Mesh Provisioning/Proxy Data Out Characteristic */
            char_uuid.len = 2;
            char_uuid.uu.uuid16 = BLE_MESH_UUID_GATT_CCC_VAL;
            BTA_GATTC_GetDescrByCharHandle(p_data->search_cmpl.conn_id, bt_mesh_gattc_info[i].data_out_handle,
                                           char_uuid, &result, &num);
            if (!result) {
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            if (num != 1) {
                bt_mesh_free(result);
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            bt_mesh_gattc_info[i].ccc_handle = result[0].attribute_handle;

            /** Enable Notification of Mesh Provisioning/Proxy Data Out
             *  Characteristic Descriptor.
             */
            write.len = sizeof(notify_en);
            write.p_value = (uint8_t *)&notify_en;
            BTA_GATTC_WriteCharDescr(p_data->search_cmpl.conn_id, result[0].attribute_handle,
                                     BTA_GATTC_TYPE_WRITE, &write, BTA_GATT_AUTH_REQ_NONE);

            bt_mesh_free(result);
            result = NULL;
        }
        break;
    }
    case BTA_GATTC_READ_DESCR_EVT:
        break;
    case BTA_GATTC_WRITE_DESCR_EVT: {
        if (p_data->write.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_WRITE_DESCR_EVT");

            handle = BLE_MESH_GATT_GET_CONN_ID(p_data->write.conn_id);

            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                if (bt_mesh_gattc_info[i].conn.handle == handle) {
                    conn = &bt_mesh_gattc_info[i].conn;
                    break;
                }
            }

            if (conn == NULL) {
                BT_ERR("Conn handle 0x%04x not found", handle);
                return;
            }

            if (bt_mesh_gattc_info[i].ccc_handle != p_data->write.handle) {
                BT_WARN("gattc ccc_handle not matched");
                bt_mesh_gattc_disconnect(conn);
                return;
            }

            if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->prov_write_descr != NULL) {
                    len = bt_mesh_gattc_conn_cb->prov_write_descr(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn);
                    if (len < 0) {
                        BT_ERR("prov_write_descr failed");
                        bt_mesh_gattc_disconnect(conn);
                        return;
                    }
                    bt_mesh_gattc_info[i].wr_desc_done = true;
                }
            } else if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROXY_VAL) {
                if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->proxy_write_descr != NULL) {
                    len = bt_mesh_gattc_conn_cb->proxy_write_descr(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn);
                    if (len < 0) {
                        BT_ERR("proxy_write_descr failed");
                        bt_mesh_gattc_disconnect(conn);
                        return;
                    }
                    bt_mesh_gattc_info[i].wr_desc_done = true;
                }
            }
        }
        break;
    }
    case BTA_GATTC_NOTIF_EVT: {
        BT_DBG("BTA_GATTC_NOTIF_EVT");

        handle = BLE_MESH_GATT_GET_CONN_ID(p_data->notify.conn_id);

        for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
            if (bt_mesh_gattc_info[i].conn.handle == handle) {
                if (bt_mesh_gattc_info[i].wr_desc_done == false) {
                    BT_WARN("Receive notification before finishing to write ccc");
#if !CONFIG_BLE_MESH_BQB_TEST
                    return;
#endif
                }

                conn = &bt_mesh_gattc_info[i].conn;
                break;
            }
        }

        if (conn == NULL) {
            BT_ERR("Conn handle 0x%04x not found", handle);
            return;
        }

        if (memcmp(bt_mesh_gattc_info[i].addr.val, p_data->notify.bda, BLE_MESH_ADDR_LEN) ||
            bt_mesh_gattc_info[i].data_out_handle != p_data->notify.handle ||
            p_data->notify.is_notify == false) {
            BT_ERR("Notification error");
            bt_mesh_gattc_disconnect(conn);
            return;
        }

        if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->prov_notify != NULL) {
                len = bt_mesh_gattc_conn_cb->prov_notify(&bt_mesh_gattc_info[i].conn,
                                                         p_data->notify.value,
                                                         p_data->notify.len);
                if (len < 0) {
                    BT_ERR("prov_notify failed");
                    bt_mesh_gattc_disconnect(conn);
                    return;
                }
            }
        } else if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROXY_VAL) {
            if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->proxy_notify != NULL) {
                len = bt_mesh_gattc_conn_cb->proxy_notify(&bt_mesh_gattc_info[i].conn,
                                                          p_data->notify.value,
                                                          p_data->notify.len);
                if (len < 0) {
                    BT_ERR("proxy_notify failed");
                    bt_mesh_gattc_disconnect(conn);
                    return;
                }
            }
        }
        break;
    }
    case BTA_GATTC_READ_CHAR_EVT:
        break;
    case BTA_GATTC_WRITE_CHAR_EVT:
        break;
    case BTA_GATTC_PREP_WRITE_EVT:
        break;
    case BTA_GATTC_EXEC_EVT:
        break;
    case BTA_GATTC_OPEN_EVT:
        BT_DBG("BTA_GATTC_OPEN_EVT");
        /* After current connection is established, Provisioner can
         * use BTM_BleScan() to re-enable scan.
         */
        if (!bt_mesh_atomic_test_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING)) {
#if CONFIG_BLE_MESH_USE_BLE_50
            BTA_DmBleGapExtScan(true, 0 ,0);
#else
            tBTM_STATUS status = BTM_BleScan(true, 0, bt_mesh_scan_results_cb, NULL, NULL);
            if (status != BTM_SUCCESS && status != BTM_CMD_STARTED) {
                BT_ERR("Invalid scan status %d", status);
                break;
            }
#endif
            bt_mesh_atomic_set_bit(bt_mesh_dev.flags, BLE_MESH_DEV_SCANNING);
        }
        break;
    case BTA_GATTC_CLOSE_EVT:
        bta_gattc_clcb_dealloc_by_conn_id(p_data->close.conn_id);
        BT_DBG("BTA_GATTC_CLOSE_EVT");
        break;
    case BTA_GATTC_CONNECT_EVT: {
        BT_DBG("BTA_GATTC_CONNECT_EVT");

        if (bt_mesh_gattc_if != p_data->connect.client_if) {
            BT_ERR("gattc_if & connect_if mismatch");
            return;
        }

        if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->connected != NULL) {
            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                if (!memcmp(bt_mesh_gattc_info[i].addr.val, p_data->connect.remote_bda, BLE_MESH_ADDR_LEN)) {
                    bt_mesh_gattc_info[i].conn.handle = BLE_MESH_GATT_GET_CONN_ID(p_data->connect.conn_id);
                    (bt_mesh_gattc_conn_cb->connected)(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn, i);
                    break;
                }
            }
        }
        break;
    }
    case BTA_GATTC_DISCONNECT_EVT: {
        BT_DBG("BTA_GATTC_DISCONNECT_EVT");

        if (bt_mesh_gattc_if != p_data->disconnect.client_if) {
            BT_ERR("gattc_if & disconnect_if mismatch");
            return;
        }

        handle = BLE_MESH_GATT_GET_CONN_ID(p_data->disconnect.conn_id);

        if (bt_mesh_gattc_conn_cb != NULL && bt_mesh_gattc_conn_cb->disconnected != NULL) {
            for (i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
                if (!memcmp(bt_mesh_gattc_info[i].addr.val, p_data->disconnect.remote_bda, BLE_MESH_ADDR_LEN)) {
                    if (bt_mesh_gattc_info[i].conn.handle == handle) {
                        (bt_mesh_gattc_conn_cb->disconnected)(&bt_mesh_gattc_info[i].addr, &bt_mesh_gattc_info[i].conn, p_data->disconnect.reason);
                        if (!bt_mesh_gattc_info[i].wr_desc_done) {
                            /* Add this in case connection is established, connected event comes, but
                             * connection is terminated before server->filter_type is set to PROV.
                             */
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                            if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                                bt_mesh_provisioner_clear_link_info(bt_mesh_gattc_info[i].addr.val);
                            }
#endif
                        }
                    } else {
                        /* Add this in case connection is failed to be established, and here we
                         * need to clear some provision link info, like connecting flag, device
                         * uuid, address info, etc.
                         */
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                        if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                            bt_mesh_provisioner_clear_link_info(bt_mesh_gattc_info[i].addr.val);
                        }
#endif
                    }
#if CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT
                    if (bt_mesh_gattc_info[i].service_uuid == BLE_MESH_UUID_MESH_PROV_VAL) {
                        /* Decrease provisioner pbg_count */
                        bt_mesh_provisioner_pbg_count_dec();
                    }
#endif
                    /* Reset corresponding gattc info */
                    memset(&bt_mesh_gattc_info[i], 0, sizeof(bt_mesh_gattc_info[i]));
                    bt_mesh_gattc_info[i].conn.handle = 0xFFFF;
                    bt_mesh_gattc_info[i].mtu = GATT_DEF_BLE_MTU_SIZE;
                    bt_mesh_gattc_info[i].wr_desc_done = false;
                    break;
                }
            }
        }
        break;
    }
    case BTA_GATTC_CONGEST_EVT:
        break;
    case BTA_GATTC_SRVC_CHG_EVT:
        break;
    default:
        break;
    }
}
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

struct bt_mesh_conn *bt_mesh_conn_ref(struct bt_mesh_conn *conn)
{
    bt_mesh_atomic_inc(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, bt_mesh_atomic_get(&conn->ref));

    return conn;
}

void bt_mesh_conn_unref(struct bt_mesh_conn *conn)
{
    bt_mesh_atomic_dec(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, bt_mesh_atomic_get(&conn->ref));
}

void bt_mesh_gatt_init(void)
{
    BTA_GATT_SetLocalMTU(GATT_DEF_BLE_MTU_SIZE);

#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    tBT_UUID gatts_app_uuid = {LEN_UUID_128, {0}};
    memset(&gatts_app_uuid.uu.uuid128, BLE_MESH_GATTS_APP_UUID_BYTE, LEN_UUID_128);

    gatts_future_mesh = future_new();
    if (!gatts_future_mesh) {
        BT_ERR("Mesh gatts sync lock alloc failed");
        return;
    }

    BTA_GATTS_AppRegister(&gatts_app_uuid, bt_mesh_bta_gatts_cb);

    if (future_await(gatts_future_mesh) == FUTURE_FAIL) {
        BT_ERR("Mesh gatts app register failed");
        return;
    }
#endif

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
    tBT_UUID gattc_app_uuid = {LEN_UUID_128, {0}};
    for (int i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        bt_mesh_gattc_info[i].conn.handle = 0xFFFF;
        bt_mesh_gattc_info[i].mtu = GATT_DEF_BLE_MTU_SIZE; /* Default MTU_SIZE 23 */
        bt_mesh_gattc_info[i].wr_desc_done = false;
    }
    memset(&gattc_app_uuid.uu.uuid128, BLE_MESH_GATTC_APP_UUID_BYTE, LEN_UUID_128);
    BTA_GATTC_AppRegister(&gattc_app_uuid, bt_mesh_bta_gattc_cb);
#endif
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_gatt_deinit(void)
{
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    BTA_GATTS_AppDeregister(bt_mesh_gatts_if);
    memset(bt_mesh_gatts_addr, 0, BLE_MESH_ADDR_LEN);
    bt_mesh_gatts_if = 0U;
    svc_handle = 0U;
    char_handle = 0U;
#endif

#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
    BTA_GATTC_AppDeregister(bt_mesh_gattc_if);
    bt_mesh_gattc_if = 0U;
    for (int i = 0; i < ARRAY_SIZE(bt_mesh_gattc_info); i++) {
        bt_mesh_gattc_info[i].conn.handle = 0xFFFF;
        memset(&bt_mesh_gattc_info[i].addr, 0, sizeof(bt_mesh_addr_t));
        bt_mesh_gattc_info[i].service_uuid = 0U;
        bt_mesh_gattc_info[i].mtu = GATT_DEF_BLE_MTU_SIZE; /* Default MTU_SIZE 23 */
        bt_mesh_gattc_info[i].wr_desc_done = false;
        bt_mesh_gattc_info[i].start_handle = 0U;
        bt_mesh_gattc_info[i].end_handle = 0U;
        bt_mesh_gattc_info[i].data_in_handle = 0U;
        bt_mesh_gattc_info[i].data_out_handle = 0U;
        bt_mesh_gattc_info[i].ccc_handle = 0U;
    }
#endif
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_adapt_init(void)
{
    /* Use unified crypto module initialization */
    bt_mesh_crypto_init();
    bt_mesh_atomic_set(bt_mesh_dev.flags, 0);
}

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
int bt_mesh_update_exceptional_list(uint8_t sub_code, uint32_t type, void *info)
{
    uint8_t value[BLE_MESH_ADDR_LEN] = {0};

    if ((sub_code > BLE_MESH_EXCEP_LIST_SUB_CODE_CLEAN) ||
        (sub_code < BLE_MESH_EXCEP_LIST_SUB_CODE_CLEAN &&
         type >= BLE_MESH_EXCEP_LIST_TYPE_MAX) ||
        (sub_code == BLE_MESH_EXCEP_LIST_SUB_CODE_CLEAN &&
         !(type & BLE_MESH_EXCEP_LIST_CLEAN_ALL_LIST))) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (type == BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID) {
        if (!info) {
            BT_ERR("Invalid Provisioning Link ID");
            return -EINVAL;
        }

        /* When removing an unused link (i.e., Link ID is 0), and since
         * Controller has never added this Link ID, it will cause error
         * log been wrongly reported.
         * Therefore, add this check here to avoid such occurrences.
         */
        if (*(uint32_t *)info == 0) {
            return 0;
        }

        sys_memcpy_swap(value, info, sizeof(uint32_t));
    }

    BT_DBG("%s exceptional list, type 0x%08x", sub_code ? "Remove" : "Add", type);

    /* The parameter "device_info" can't be NULL in the API */
    BLE_MESH_BTM_CHECK_STATUS(BTM_UpdateBleDuplicateExceptionalList(sub_code, type, value, NULL));

    return 0;
}
#endif
