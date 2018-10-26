/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2016 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include "mesh_bearer_adapt.h"
#include "mesh_trace.h"

#include "bta/bta_api.h"
#include "bta/bta_gatt_api.h"
#include "bta/bta_gatt_common.h"
#include "stack/btm_ble_api.h"
#include "p_256_ecc_pp.h"

#include "stack/hcimsgs.h"
#include "osi/future.h"
#include "include/mesh_buf.h"
#include "osi/allocator.h"
#include "include/mesh_atomic.h"
#include "include/mesh_hci.h"

#include "mbedtls/aes.h"
#include "mesh_aes_encrypt.h"
#include "sdkconfig.h"
#if CONFIG_BT_MESH

#include "bta/bta_api.h"
#include <errno.h>
#include "include/mesh_buf.h"
#include "osi/allocator.h"
#include "include/mesh_atomic.h"
#include "bta_gattc_int.h"
#include "provisioner_prov.h"

/* Macros */
#define BT_DEV 1
#define MESH_GATT_GET_CONN_ID(conn_id)  (((uint16_t)(conn_id)) >> 8)
#define MESH_GATT_CREATE_CONN_ID(gatt_if, conn_id)  ((uint16_t) ((((uint8_t)(conn_id)) << 8) | ((uint8_t)(gatt_if))))
#define CONFIG_BT_MAX_CONN CONFIG_BT_ACL_CONNECTIONS

/* Global Variables */
extern struct bt_dev bt_dev;

/* the gatt database list to save the attribute table */
static sys_slist_t db;
static tBTA_GATTS_IF mesh_gatts_if;
static uint16_t svc_handle = 0;
static uint16_t char_handle = 0;
static u8_t pub_key[64];
BT_OCTET32  mesh_private_key = {0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9, \
                                0xb3, 0xe3, 0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b, \
                                0xeb, 0x40, 0xb7, 0x99, 0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd
                               };

uint8_t test_peer_publ_key[64] = {0x1e, 0xa1, 0xf0, 0xf0, 0x1f, 0xaf, 0x1d, 0x96, 0x09, 0x59, 0x22, 0x84, \
                                  0xf1, 0x9e, 0x4c, 0x00, 0x47, 0xb5, 0x8a, 0xfd, 0x86, 0x15, 0xa6, 0x9f, \
                                  0x55, 0x90, 0x77, 0xb2, 0x2f, 0xaa, 0xa1, 0x90, 0x4c, 0x55, 0xf3, 0x3e, \
                                  0x42, 0x9d, 0xad, 0x37, 0x73, 0x56, 0x70, 0x3a, 0x9a, 0xb8, 0x51, 0x60, \
                                  0x47, 0x2d, 0x11, 0x30, 0xe2, 0x8e, 0x36, 0x76, 0x5f, 0x89, 0xaf, 0xf9, \
                                  0x15, 0xb1, 0x21, 0x4a
                                 };

struct bt_conn mesh_conn[CONFIG_BT_MAX_CONN];

static struct bt_conn_cb *mesh_conn_cb = NULL;
future_t *future_mesh = NULL;

u16_t temp_handle = 0;

/* Static Variables */
static bt_le_scan_cb_t *scan_dev_found_cb = NULL;
/* Static Functions */
static void scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);
struct bt_gatt_attr *bt_gatt_find_attr_by_handle(u16_t handle);
static void bt_mesh_rand_cb(tBTM_RAND_ENC *p);

#if CONFIG_BT_MESH_PROVISIONER
#define BT_MESH_GATTC_APP_UUID_BYTE 0x97
struct gattc_prov_info {
    /* Service to be found depends on the type of adv pkt received */
    u16_t          srvc_uuid;
    BD_ADDR        addr;
    u8_t           addr_type;
    struct bt_conn conn;
    u16_t          mtu;
    bool           wr_desc_done;    /* Indicate if write char descriptor event is received */
    u16_t          start_handle;    /* Service attribute start handle */
    u16_t          end_handle;      /* Service attribute end handle */
    u16_t          data_in_handle;  /* Data In Characteristic attribute handle */
    u16_t          data_out_handle; /* Data Out Characteristic attribute handle */
    u16_t          ccc_handle;      /* Data Out Characteristic CCC attribute handle */
} gattc_info[CONFIG_BT_MESH_PBG_SAME_TIME];
static struct bt_prov_conn_cb *mesh_prov_conn_cb;
static tBTA_GATTC_IF mesh_gattc_if;
#endif /* CONFIG_BT_MESH_PROVISIONER */

static bool valid_adv_param(const struct bt_le_adv_param *param)
{
    if (!(param->options & BT_LE_ADV_OPT_CONNECTABLE)) {
        /*
         * BT Core 4.2 [Vol 2, Part E, 7.8.5]
         * The Advertising_Interval_Min and Advertising_Interval_Max
         * shall not be set to less than 0x00A0 (100 ms) if the
         * Advertising_Type is set to ADV_SCAN_IND or ADV_NONCONN_IND.
         */
#if BT_DEV
        /**TODO: Need to optimize controller to make it supports 20ms interval */
        if (bt_dev.hci_version < BT_HCI_VERSION_5_0 &&
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

static int set_ad(u16_t hci_op, const struct bt_data *ad, size_t ad_len)
{
    struct bt_hci_cp_le_set_adv_data set_data_param;
    struct bt_hci_cp_le_set_adv_data *set_data = &set_data_param;
    int i;

#if 0
    struct net_buf *buf;
    buf = bt_hci_cmd_create(hci_op, sizeof(*set_data));
    if (!buf) {
        return -ENOBUFS;
    }
    I_OP_LE_SET_ADV_DATA
#endif

    memset(set_data, 0, sizeof(*set_data));

    for (i = 0; i < ad_len; i++) {
        /* Check if ad fit in the remaining buffer */
        if (set_data->len + ad[i].data_len + 2 > 31) {
#if 0
            net_buf_unref(buf);
#endif
            return -EINVAL;
        }

        set_data->data[set_data->len++] = ad[i].data_len + 1;
        set_data->data[set_data->len++] = ad[i].type;

        memcpy(&set_data->data[set_data->len], ad[i].data,
               ad[i].data_len);
        set_data->len += ad[i].data_len;
    }

    /* Set adv data and scan rsp data, TODO: how to process null adv data? Do we need to set adv data every time even though the adv data is not changed? */

#if 0
    return bt_hci_cmd_send_sync(hci_op, buf, NULL);
#else
    if (hci_op == BT_HCI_OP_LE_SET_ADV_DATA) {
        BTA_DmBleSetAdvConfigRaw (set_data->data, set_data->len, NULL);
    } else if (hci_op == BT_HCI_OP_LE_SET_SCAN_RSP_DATA) {
        BTA_DmBleSetScanRspRaw (set_data->data, set_data->len, NULL);
    }
    return 0;
#endif

}

static void start_adv_completed_cb(u8_t status)
{
    /**TODO: It is too late to wait for completed event*/
#if 0
    if (!status) {
        atomic_set_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#endif
}

static void stop_adv_completed_cb(u8_t status)
{
    /**TODO: It is too late to wait for completed event*/
#if 0
    if (!status) {
        atomic_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#endif
}
static bool valid_le_scan_param(const struct bt_le_scan_param *param)
{
    if (param->type != BT_HCI_LE_SCAN_PASSIVE &&
            param->type != BT_HCI_LE_SCAN_ACTIVE) {
        return false;
    }

    if (param->filter_dup != BT_HCI_LE_SCAN_FILTER_DUP_DISABLE &&
            param->filter_dup != BT_HCI_LE_SCAN_FILTER_DUP_ENABLE) {
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

static int start_le_scan(u8_t scan_type, u16_t interval, u16_t window)
{
    int err = 0;

#if 0
    struct bt_hci_cp_le_set_scan_param set_param;
    struct net_buf *buf;
    memset(&set_param, 0, sizeof(set_param));

    set_param.scan_type = scan_type;
    /* for the rest parameters apply default values according to
     *  spec 4.2, vol2, part E, 7.8.10
     */
    set_param.interval = sys_cpu_to_le16(interval);
    set_param.window = sys_cpu_to_le16(window);
    set_param.filter_policy = 0x00;
#endif


    /* TODO: Currently support only RPA and set only once beore scan */
#if 0
    if (IS_ENABLED(CONFIG_BT_PRIVACY)) {
        err = le_set_private_addr();
        if (err) {
            return err;
        }

        if (BT_FEAT_LE_PRIVACY(bt_dev.le.features)) {
            set_param.addr_type = BT_HCI_OWN_ADDR_RPA_OR_RANDOM;
        } else {
            set_param.addr_type = BT_ADDR_LE_RANDOM;
        }
    } else {
        set_param.addr_type =  bt_dev.id_addr.type;

        /* Use NRPA unless identity has been explicitly requested
         * (through Kconfig), or if there is no advertising ongoing.
         */
        if (!IS_ENABLED(CONFIG_BT_SCAN_WITH_IDENTITY) &&
                scan_type == BT_HCI_LE_SCAN_ACTIVE &&
                !atomic_test_bit(bt_dev.flags, BT_DEV_ADVERTISING)) {
            err = le_set_private_addr();
            if (err) {
                return err;
            }

            set_param.addr_type = BT_ADDR_LE_RANDOM;
        }
    }
#endif

#if 0
    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_SCAN_PARAM, sizeof(set_param));
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, &set_param, sizeof(set_param));

    bt_hci_cmd_send(BT_HCI_OP_LE_SET_SCAN_PARAM, buf);

    err = set_le_scan_enable(BT_HCI_LE_SCAN_ENABLE);
    if (err) {
        return err;
    }
#else

    UINT16 client_if = 0xFF; //default GATT interface id
    UINT8 scan_fil_policy = 0x00; //No white-list for mesh
    UINT8 addr_type_own = 0x0; //TODO: currently support only RPA

    /*TODO: Need to process scan_param_setup_cback
     * Need to add menuconfig for duplicate scan*/
#if defined(CONFIG_BLE_MESH_SCAN_DUPLICATE_EN) && CONFIG_BLE_MESH_SCAN_DUPLICATE_EN
    BTA_DmSetBleScanFilterParams(client_if, interval, window, scan_type, scan_fil_policy, addr_type_own, BT_HCI_LE_SCAN_FILTER_DUP_ENABLE, NULL);

#else
    BTA_DmSetBleScanFilterParams(client_if, interval, window, scan_type, scan_fil_policy, addr_type_own, BT_HCI_LE_SCAN_FILTER_DUP_DISABLE, NULL);
#endif

    /*TODO: Need to process p_start_stop_scan_cb to check if start successfully*/
    /* BLE Mesh scan permanently, so no duration of scan here */
    BTA_DmBleScan(true, 0, scan_result_callback, NULL);


#endif

#if BT_DEV
    if (scan_type == BT_HCI_LE_SCAN_ACTIVE) {
        atomic_set_bit(bt_dev.flags, BT_DEV_ACTIVE_SCAN);
    } else {
        atomic_clear_bit(bt_dev.flags, BT_DEV_ACTIVE_SCAN);
    }
#endif

    return err;
}

static void scan_result_callback(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data)
{
    bt_addr_le_t addr;
    UINT8 rssi;
    UINT8 adv_type;
    //BT_DBG("%s, event = %d", __func__, event);
    //BT_DBG("addr_type = %d", p_data->inq_res.ble_addr_type);
    //BT_DBG("adv_data: %s", bt_hex(p_data->inq_res.p_eir, p_data->inq_res.adv_data_len));
    if (event == BTA_DM_INQ_RES_EVT) {
        /*TODO: How to process scan_rsp here? */
        addr.type = p_data->inq_res.ble_addr_type;
        memcpy(addr.a.val, p_data->inq_res.bd_addr, 6);
        rssi = p_data->inq_res.rssi;
        adv_type = p_data->inq_res.ble_evt_type;
        struct net_buf_simple *buf;
        buf = (struct net_buf_simple *) osi_malloc (sizeof(struct net_buf_simple) + p_data->inq_res.adv_data_len);
        buf->data = buf->__buf;
        buf->len = p_data->inq_res.adv_data_len;
        //scan rsp len: p_data->inq_res.scan_rsp_len
        buf->size = p_data->inq_res.adv_data_len;
        memcpy(buf->__buf, p_data->inq_res.p_eir, p_data->inq_res.adv_data_len);
        if (scan_dev_found_cb != NULL) {
            scan_dev_found_cb(&addr, rssi, adv_type, buf);
        }
        osi_free(buf);
    } else if (event == BTA_DM_INQ_CMPL_EVT) {
        BT_INFO("%s, Scan completed. Num Resp %d\n", __func__, p_data->inq_cmpl.num_resps);
    } else {
        BT_ERR("%s, unexpected events 0x%x\n", __func__, p_data->inq_cmpl.num_resps);
    }
}

int bt_le_scan_update(bool fast_scan)
{
#if BT_DEV
    if (atomic_test_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
        return 0;
    }
#endif /* #if BT_DEV */

#if BT_DEV
    if (atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING)) {
#else /* #if BT_DEV */
    if (1) {
#endif /* #if BT_DEV */

#if 0
        int err;

        err = set_le_scan_enable(BT_HCI_LE_SCAN_DISABLE);
        if (err) {
            return err;
        }
#else /* #if 0 */
        /* TODO: need to process stop_scan_cb */
        atomic_clear_bit(bt_dev.flags, BT_DEV_SCANNING);
        BTA_DmBleScan(false, 0, NULL, NULL);
#endif /* #if 0 */
    }

    /* We don't need to stay pasive scan for central device */
#if 0
    if (IS_ENABLED(CONFIG_BT_CENTRAL)) {
        u16_t interval, window;
        struct bt_conn *conn;

        conn = bt_conn_lookup_state_le(NULL, BT_CONN_CONNECT_SCAN);
        if (!conn) {
            return 0;
        }

        atomic_set_bit(bt_dev.flags, BT_DEV_SCAN_FILTER_DUP);

        bt_conn_unref(conn);

        if (fast_scan) {
            interval = BT_GAP_SCAN_FAST_INTERVAL;
            window = BT_GAP_SCAN_FAST_WINDOW;
        } else {
            interval = BT_GAP_SCAN_SLOW_INTERVAL_1;
            window = BT_GAP_SCAN_SLOW_WINDOW_1;
        }

        return start_le_scan(BT_HCI_LE_SCAN_PASSIVE, interval, window);
    }
#endif /* #if 0 */

    return 0;
}

/* APIs functions */
int bt_le_adv_start(const struct bt_le_adv_param *param,
                    const struct bt_data *ad, size_t ad_len,
                    const struct bt_data *sd, size_t sd_len)
{
    /** TODO:
     *- Need to support 20ms interval in 4.2
     *- Need to check adv start HCI event
     *- Random address check, currently we set privacy only once in application
     *- Process ADV_OPT_ONE_TIME
     **/
    UINT16 adv_int_min;
    UINT16 adv_int_max;
    tBLE_ADDR_TYPE addr_type_own;
    UINT8 adv_type;
    tBTM_BLE_ADV_CHNL_MAP chnl_map;
    tBTM_BLE_AFP adv_fil_pol;
    tBLE_BD_ADDR p_dir_bda;
    tBTA_START_ADV_CMPL_CBACK *p_start_adv_cb;
    UINT8 status;

    struct bt_hci_cp_le_set_adv_param set_param;
    int err = 0;

    if (!valid_adv_param(param)) {
        return -EINVAL;
    }

#if BT_DEV
#if 0
    /**TODO: We cancel this check temporarily*/
    if (atomic_test_bit(bt_dev.flags, BT_DEV_ADVERTISING)) {
        return -EALREADY;
    }
#endif /* BT_DEV */
#endif /* 0 */

    err = set_ad(BT_HCI_OP_LE_SET_ADV_DATA, ad, ad_len);
    if (err) {
        return err;
    }

    /*
     * We need to set SCAN_RSP when enabling advertising type that allows
     * for Scan Requests.
     *
     * If sd was not provided but we enable connectable undirected
     * advertising sd needs to be cleared from values set by previous calls.
     * Clearing sd is done by calling set_ad() with NULL data and zero len.
     * So following condition check is unusual but correct.
     */
    if (sd || (param->options & BT_LE_ADV_OPT_CONNECTABLE)) {
        err = set_ad(BT_HCI_OP_LE_SET_SCAN_RSP_DATA, sd, sd_len);
        if (err) {
            return err;
        }
    }

    memset(&set_param, 0, sizeof(set_param));

    set_param.min_interval = sys_cpu_to_le16(param->interval_min);
    set_param.max_interval = sys_cpu_to_le16(param->interval_max);
    set_param.channel_map  = 0x07;

    /** TODO: Currently we support only RPA address */
#if 0
    if (param->options & BT_LE_ADV_OPT_CONNECTABLE) {
        if (IS_ENABLED(CONFIG_BT_PRIVACY)) {
            err = le_set_private_addr();
            if (err) {
                return err;
            }

            if (BT_FEAT_LE_PRIVACY(bt_dev.le.features)) {
                set_param.own_addr_type =
                    BT_HCI_OWN_ADDR_RPA_OR_RANDOM;
            } else {
                set_param.own_addr_type = BT_ADDR_LE_RANDOM;
            }
        } else {
            /*
             * If Static Random address is used as Identity
             * address we need to restore it before advertising
             * is enabled. Otherwise NRPA used for active scan
             * could be used for advertising.
             */
            if (atomic_test_bit(bt_dev.flags,
                                BT_DEV_ID_STATIC_RANDOM)) {
                set_random_address(&bt_dev.id_addr.a);
            }

            set_param.own_addr_type = bt_dev.id_addr.type;
        }

        set_param.type = BT_LE_ADV_IND;
    } else {
        if (param->own_addr) {
            /* Only NRPA is allowed */
            if (!BT_ADDR_IS_NRPA(param->own_addr)) {
                return -EINVAL;
            }

            err = set_random_address(param->own_addr);
        } else {
            err = le_set_private_addr();
        }

        if (err) {
            return err;
        }

        set_param.own_addr_type = BT_ADDR_LE_RANDOM;

        if (sd) {
            set_param.type = BT_LE_ADV_SCAN_IND;
        } else {
            set_param.type = BT_LE_ADV_NONCONN_IND;
        }
    }
#else /* #if 0 */
    /* TODO: Has been simplified here, currently support only RPA addr */
    set_param.own_addr_type = 0x0; //BT_ADDR_LE_RANDOM
    if (param->options & BT_LE_ADV_OPT_CONNECTABLE) {
        set_param.type = 0x00; //ADV_TYPE_IND;
    } else if (sd != NULL) {
        set_param.type = 0x02; //ADV_TYPE_SCAN_IND;
    } else {
        set_param.type = 0x03; //ADV_TYPE_NONCONN_IND;
    }
#endif /* #if 0 */



#if 0
    buf = bt_hci_cmd_create(BT_HCI_OP_LE_SET_ADV_PARAM, sizeof(set_param));
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_mem(buf, &set_param, sizeof(set_param));

    err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_ADV_PARAM, buf, NULL);
    if (err) {
        return err;
    }

    err = set_advertise_enable(true);
    if (err) {
        return err;
    }
#else /* #if 0 */
    addr_type_own = set_param.own_addr_type;
    /* Set adv parameters */
    adv_int_min = param->interval_min;
    adv_int_max = param->interval_max;
    adv_type = set_param.type;
    chnl_map = BTA_BLE_ADV_CHNL_37 | BTA_BLE_ADV_CHNL_38 | BTA_BLE_ADV_CHNL_39;
    adv_fil_pol = AP_SCAN_CONN_ALL;
    memset(&p_dir_bda, 0, sizeof(p_dir_bda));
    p_start_adv_cb = start_adv_completed_cb;

    /*TODO: We need to add a check function, to make sure adv start successfully and then set bit
     * So currently we call BTM_BleSetAdvParamsStartAdv instead of BTA_DmSetBleAdvParamsAll
     * */
#if 1
    /* Check if we can start adv using BTM_BleSetAdvParamsStartAdvCheck */
    status = BTM_BleSetAdvParamsStartAdvCheck(adv_int_min, adv_int_max, adv_type, addr_type_own,
             &p_dir_bda, chnl_map, adv_fil_pol, p_start_adv_cb);
    if (status) {
        return status;
    } else {
        BTA_DmSetBleAdvParamsAll (adv_int_min, adv_int_max, adv_type, addr_type_own, chnl_map,
                                  adv_fil_pol, &p_dir_bda, p_start_adv_cb);
        atomic_set_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#else /* #if 1 */
    status = BTM_BleSetAdvParamsStartAdv (adv_int_min, adv_int_max, adv_type, addr_type_own, &p_dir_bda, chnl_map, adv_fil_pol, p_start_adv_cb);
    if (!status) {
        atomic_set_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#endif /* #if 1 */
#endif /* #if 0 */


    /**TODO: To implement BT_LE_ADV_OPT_ONE_TIME */
#if BT_DEV
    if (!(param->options & BT_LE_ADV_OPT_ONE_TIME)) {
        atomic_set_bit(bt_dev.flags, BT_DEV_KEEP_ADVERTISING);
    }
#endif /* #if BT_DEV */

    return err;
}

int bt_le_adv_stop(void)
{
    int err = 0;
    UINT8 status;

    /* Make sure advertising is not re-enabled later even if it's not
     * currently enabled (i.e. BT_DEV_ADVERTISING is not set).
     */
#if BT_DEV
    atomic_clear_bit(bt_dev.flags, BT_DEV_KEEP_ADVERTISING);

#if 0
    /**TODO: We cancel this check temporarily*/
    if (!atomic_test_bit(bt_dev.flags, BT_DEV_ADVERTISING)) {
        return 0;
    }
#endif //end of "#if 0"
#endif /* #if BT_DEV */

#if 0
    err = set_advertise_enable(false);
    if (err) {
        return err;
    }
#else
    /*TODO: We need to add a check function, to make sure adv start successfully and then set bit
     * So currently we use BTM_BleBroadcast instead of BTA_DmBleBroadcast*/
#if 1
    status = BTM_BleBroadcastCheck(false, stop_adv_completed_cb);
    if (status) {
        return status;
    } else {
        BTA_DmBleBroadcast (false, stop_adv_completed_cb);
        atomic_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#else
    status = BTM_BleBroadcast(false, stop_adv_completed_cb);
    if (!status) {
        atomic_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING);
    }
#endif
#endif

#if 0
    /*TODO: Currently support RPA only */
    if (!IS_ENABLED(CONFIG_BT_PRIVACY)) {
        /* If active scan is ongoing set NRPA */
        if (atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING) &&
                atomic_test_bit(bt_dev.flags, BT_DEV_ACTIVE_SCAN)) {
            le_set_private_addr();
        }
    }
#endif

    return err;
}


int bt_le_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb)
{
    /** TODO:
     *- Do we need to use duplicate mode for mesh scan?
     *-
     *-
     *-
     **/
    int err = 0;

    /* Check that the parameters have valid values */
    if (!valid_le_scan_param(param)) {
        return -EINVAL;
    }

    /* Return if active scan is already enabled */
#if BT_DEV
    if (atomic_test_and_set_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
        return -EALREADY;
    }
#endif
//TODO: Currently we use bluedroid APIs, which will stop scan automatically while setting parameters
#if 0
    if (atomic_test_bit(bt_dev.flags, BT_DEV_SCANNING)) {
        err = set_le_scan_enable(BT_HCI_LE_SCAN_DISABLE);
        if (err) {
            atomic_clear_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN);
            return err;
        }
    }
#endif

#if BT_DEV
    if (param->filter_dup) {
        atomic_set_bit(bt_dev.flags, BT_DEV_SCAN_FILTER_DUP);
    } else {
        atomic_clear_bit(bt_dev.flags, BT_DEV_SCAN_FILTER_DUP);
    }
#endif

    err = start_le_scan(param->type, param->interval, param->window);
    if (err) {
#if BT_DEV
        atomic_clear_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN);
#endif
        return err;
    }

    atomic_set_bit(bt_dev.flags, BT_DEV_SCANNING);
    scan_dev_found_cb = cb;

    return err;
}

int bt_le_scan_stop(void)
{
    /* Return if active scanning is already disabled */
#if BT_DEV
    if (!atomic_test_and_clear_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
        return -EALREADY;
    }
#endif

    scan_dev_found_cb = NULL;

    return bt_le_scan_update(false);
}

#if CONFIG_BT_MESH_NODE

static void mesh_bta_gatts_cb(tBTA_GATTS_EVT event, tBTA_GATTS *p_data)
{
    switch (event) {
    case BTA_GATTS_REG_EVT:
        if (p_data->reg_oper.status == BTA_GATT_OK) {
            mesh_gatts_if = p_data->reg_oper.server_if;
        }
        break;
    case BTA_GATTS_READ_EVT: {
        uint8_t buf[100] = {0};
        uint16_t len = 0;
        tBTA_GATTS_RSP rsp;
        BT_DBG("%s, read: handle = %d", __func__, p_data->req_data.p_data->read_req.handle);
        uint8_t index = MESH_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        struct bt_gatt_attr *mesh_attr = bt_gatt_find_attr_by_handle(p_data->req_data.p_data->read_req.handle);
        if (mesh_attr != NULL && mesh_attr->read != NULL) {
            if ((len = mesh_attr->read(&mesh_conn[index], mesh_attr, buf, 100,
                                       p_data->req_data.p_data->read_req.offset)) > 0) {
                rsp.attr_value.handle = p_data->req_data.p_data->read_req.handle;
                rsp.attr_value.len = len;
                memcpy(&rsp.attr_value.value[0], buf, len);
                BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                  p_data->req_data.status, &rsp);
                BT_DBG("%s, send mesh read rsp, handle = %x", __func__, mesh_attr->handle);
            } else {
                BT_WARN("%s, read perm Invalid", __func__);
            }
        }
        break;
    }
    case BTA_GATTS_WRITE_EVT: {
        //void *buf = NULL;
        uint16_t len = 0;
        BT_DBG("%s, write: handle = %d, len = %d, data = %s", __func__, p_data->req_data.p_data->write_req.handle,
               p_data->req_data.p_data->write_req.len,
               bt_hex(p_data->req_data.p_data->write_req.value, p_data->req_data.p_data->write_req.len));
        uint8_t index = MESH_GATT_GET_CONN_ID(p_data->req_data.conn_id);
        struct bt_gatt_attr *mesh_attr = bt_gatt_find_attr_by_handle(p_data->req_data.p_data->write_req.handle);
        if (mesh_attr != NULL && mesh_attr->write != NULL) {
            if ((len = mesh_attr->write(&mesh_conn[index], mesh_attr,
                                        p_data->req_data.p_data->write_req.value,
                                        p_data->req_data.p_data->write_req.len,
                                        p_data->req_data.p_data->write_req.offset, 0)) > 0) {
                if (p_data->req_data.p_data->write_req.need_rsp) {
                    BTA_GATTS_SendRsp(p_data->req_data.conn_id, p_data->req_data.trans_id,
                                      p_data->req_data.status, NULL);
                    BT_DBG("%s, send mesh write rsp, handle = %x", __func__, mesh_attr->handle);
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
    case BTA_GATTS_CREATE_EVT: {
        svc_handle = p_data->create.service_id;
        BT_DBG("%s, svc_handle = %d, future_mesh = %p", __func__, svc_handle, future_mesh);
        if (future_mesh != NULL) {
            future_ready(future_mesh, FUTURE_SUCCESS);
        }
        break;
    }
    case BTA_GATTS_ADD_INCL_SRVC_EVT: {
        svc_handle = p_data->add_result.attr_id;
        if (future_mesh != NULL) {
            future_ready(future_mesh, FUTURE_SUCCESS);
        }
        break;
    }
    case BTA_GATTS_ADD_CHAR_EVT:
        char_handle = p_data->add_result.attr_id;
        if (future_mesh != NULL) {
            future_ready(future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_ADD_CHAR_DESCR_EVT:
        char_handle = p_data->add_result.attr_id;
        if (future_mesh != NULL) {
            future_ready(future_mesh, FUTURE_SUCCESS);
        }
        break;
    case BTA_GATTS_DELELTE_EVT:
        break;
    case BTA_GATTS_START_EVT:
        break;
    case BTA_GATTS_STOP_EVT:
        break;
    case BTA_GATTS_CONNECT_EVT: {
        /*Adv disabled*/
        // atomic_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING);
        if (mesh_conn_cb != NULL && mesh_conn_cb->connected != NULL) {
            uint8_t index = MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
            if (index < CONFIG_BT_MAX_CONN) {
                mesh_conn[index].handle = MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
                (mesh_conn_cb->connected)(&mesh_conn[index], 0);
            }
        }
        break;
    }
    case BTA_GATTS_DISCONNECT_EVT: {
        /*Adv disabled*/
        // atomic_clear_bit(bt_dev.flags, BT_DEV_ADVERTISING);
        if (mesh_conn_cb != NULL && mesh_conn_cb->disconnected != NULL) {
            uint8_t index = MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
            if (index < CONFIG_BT_MAX_CONN) {
                mesh_conn[index].handle = MESH_GATT_GET_CONN_ID(p_data->conn.conn_id);
                (mesh_conn_cb->disconnected)(&mesh_conn[index], p_data->conn.reason);
            }
        }
        break;
    }
    case BTA_GATTS_CLOSE_EVT:
        break;
    default:
        break;
    }
}

#endif /* CONFIG_BT_MESH_NODE */

#if CONFIG_BT_MESH_PROVISIONER

void bt_prov_conn_cb_register(struct bt_prov_conn_cb *cb)
{
    mesh_prov_conn_cb = cb;
}

bool bt_prov_check_gattc_id(int id, const bt_addr_le_t *addr)
{
    u8_t zero[6] = {0};

    if (!addr || !memcmp(addr->a.val, zero, 6) || (addr->type > BLE_ADDR_RANDOM)) {
        BT_ERR("%s: invalid parameters", __func__);
        return false;
    }

    if (gattc_info[id].conn.handle == 0xFFFF) {
        memcpy(gattc_info[id].addr, addr->a.val, 6);
        gattc_info[id].addr_type = addr->type;
        return true;
    }

    return false;
}

u16_t bt_mesh_get_srvc_uuid(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == &gattc_info[i].conn) {
            break;
        }
    }

    if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
        return 0;
    }

    return gattc_info[i].srvc_uuid;
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

void bt_gattc_conn_create(int id, u16_t srvc_uuid)
{
    BD_ADDR remote_bda;

    if (atomic_test_and_clear_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
        BTA_DmBleScan(false, 0, NULL, NULL);
    }

    memcpy(remote_bda, gattc_info[id].addr, 6);

    /** Min_interval:        250ms
     *  Max_interval:        250ms
     *  Slave_latency:       0x0
     *  Supervision_timeout: 32sec
    */
    BTA_DmSetBlePrefConnParams(remote_bda, 0xC8, 0xC8, 0x00, 0xC80);

    BTA_GATTC_Open(mesh_gattc_if, remote_bda, gattc_info[id].addr_type, true, BTA_GATT_TRANSPORT_LE);

    /* Increment pbg_count */
    provisioner_pbg_count_inc();

    /* Service to be found after exhanging mtu size */
    gattc_info[id].srvc_uuid = srvc_uuid;
}

void bt_gattc_exchange_mtu(int id)
{
    /** Set local MTU and exchange with GATT server.
     *  ATT_MTU >= 69 for Mesh GATT Prov Service
     *  ATT_NTU >= 33 for Mesh GATT Proxy Service
    */
    u16_t conn_id;

    conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gattc_if, gattc_info[id].conn.handle);

    BTA_GATTC_ConfigureMTU(conn_id);
}

u16_t bt_gatt_prov_get_mtu(struct bt_conn *conn)
{
    int i;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == &gattc_info[i].conn) {
            return gattc_info[i].mtu;
        }
    }

    return 0;
}

int bt_gattc_write_no_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *data, u16_t len)
{
    int i;
    u16_t conn_id;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == &gattc_info[i].conn) {
            break;
        }
    }

    if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
        BT_ERR("No conn found");
        /** Here we return 0 for prov_send() return value check in provisioner.c
         */
        return 0;
    }

    conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gattc_if, gattc_info[i].conn.handle);

    BTA_GATTC_WriteCharValue(conn_id, gattc_info[i].data_in_handle,
                             BTA_GATTC_TYPE_WRITE_NO_RSP, len,
                             (u8_t *)data, BTA_GATT_AUTH_REQ_NONE);

    return 0;
}

void bt_gattc_disconnect(struct bt_conn *conn)
{
    /** Disconnect
     *  Clear proper proxy server information
     *  Clear proper prov_link information
     *  Clear proper gattc_info information
     *  Here in adapter, we just clear proper gattc_info, and
     *  when proxy_disconnected callback comes, the proxy server
     *  information and prov_link information should be cleared.
     */
    int i;
    u16_t conn_id;

    for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        if (conn == &gattc_info[i].conn) {
            break;
        }
    }

    if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
        BT_ERR("No conn found");
        return;
    }

    conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gattc_if, gattc_info[i].conn.handle);

    BTA_GATTC_Close(conn_id);
}

/** Mesh Provisioning Service:  0x1827
 *  Mesh Provisioning Data In:  0x2ADB
 *  Mesh Provisioning Data Out: 0x2ADC
 *  Mesh Proxy Service:  0x1828
 *  Mesh Proxy Data In:  0x2ADD
 *  Mesh PROXY Data Out: 0x2ADE
 */
static void mesh_bta_gattc_cb(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    int     i      = 0;
    u16_t   handle = 0;
    ssize_t len    = 0;
    struct bt_conn *conn = NULL;

    switch (event) {
    case BTA_GATTC_REG_EVT:
        if (p_data->reg_oper.status == BTA_GATT_OK) {
            u8_t uuid[16] = { [0 ... 15] = BT_MESH_GATTC_APP_UUID_BYTE };

            BT_DBG("BTA_GATTC_REG_EVT");

            if (p_data->reg_oper.app_uuid.len == LEN_UUID_128 &&
                    !memcmp(p_data->reg_oper.app_uuid.uu.uuid128, uuid, 16)) {
                mesh_gattc_if = p_data->reg_oper.client_if;
                BT_DBG("mesh_gattc_if is %d", mesh_gattc_if);
            }
        }
        break;

    case BTA_GATTC_CFG_MTU_EVT:
        if (p_data->cfg_mtu.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_CFG_MTU_EVT, cfg_mtu is %d", p_data->cfg_mtu.mtu);

            handle = MESH_GATT_GET_CONN_ID(p_data->cfg_mtu.conn_id);

            for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
                if (gattc_info[i].conn.handle == handle) {
                    gattc_info[i].mtu = p_data->cfg_mtu.mtu;
                    break;
                }
            }

            /** Once mtu exchanged accomplished, start to find services, and here
             *  need a flag to indicate which service to find(Mesh Prov Service or
             *  Mesh Proxy Service)
             */
            if (i != CONFIG_BT_MESH_PBG_SAME_TIME) {
                tBT_UUID service_uuid;
                u16_t conn_id;

                conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gattc_if, gattc_info[i].conn.handle);
                service_uuid.len       = sizeof(gattc_info[i].srvc_uuid);
                service_uuid.uu.uuid16 = gattc_info[i].srvc_uuid;

                /* Search Mesh Provisioning Service or Mesh Proxy Service */
                BTA_GATTC_ServiceSearchRequest(conn_id, &service_uuid);
            }
        }
        break;

    case BTA_GATTC_SEARCH_RES_EVT: {
        BT_DBG("BTA_GATTC_SEARCH_RES_EVT");

        handle = MESH_GATT_GET_CONN_ID(p_data->srvc_res.conn_id);

        for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
            if (gattc_info[i].conn.handle == handle) {
                break;
            }
        }

        if (i != CONFIG_BT_MESH_PBG_SAME_TIME) {
            if (p_data->srvc_res.service_uuid.uuid.len == 2 &&
                    p_data->srvc_res.service_uuid.uuid.uu.uuid16 == gattc_info[i].srvc_uuid) {
                gattc_info[i].start_handle = p_data->srvc_res.start_handle;
                gattc_info[i].end_handle   = p_data->srvc_res.end_handle;
            }
        }
    }
    break;

    case BTA_GATTC_SEARCH_CMPL_EVT: {
        if (p_data->search_cmpl.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_SEARCH_CMPL_EVT");

            handle = MESH_GATT_GET_CONN_ID(p_data->search_cmpl.conn_id);

            for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
                if (gattc_info[i].conn.handle == handle) {
                    break;
                }
            }

            if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
                BT_ERR("BTA_GATTC_SEARCH_CMPL_EVT: conn_id not found");
                return;
            }

            conn = &gattc_info[i].conn;

            if (gattc_info[i].start_handle == 0x00 ||
                    gattc_info[i].end_handle   == 0x00 ||
                    (gattc_info[i].start_handle > gattc_info[i].end_handle)) {
                bt_gattc_disconnect(conn);
                return;
            }

            int count = 0;
            int num = 0;
            u16_t conn_id;
            tBT_UUID char_uuid;
            btgatt_db_element_t *result = NULL;
            tBTA_GATT_STATUS status;
            u16_t notify_en = BT_GATT_CCC_NOTIFY;
            tBTA_GATT_UNFMT write;

            /* Get the characteristic num within Mesh Provisioning/Proxy Service */
            conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gattc_if, gattc_info[i].conn.handle);
            BTA_GATTC_GetDBSizeByType(conn_id, BTGATT_DB_CHARACTERISTIC, gattc_info[i].start_handle,
                                      gattc_info[i].end_handle, BTA_GATTC_INVALID_HANDLE, &count);
            if (count != 2) {
                bt_gattc_disconnect(conn);
                return;
            }

            /* Get Mesh Provisioning/Proxy Data In/Out Characteristic */
            for (int j = 0; j != 2; j++) {
                /** First:  find Mesh Provisioning/Proxy Data In Characteristic
                 *  Second: find Mesh Provisioning/Proxy Data Out Characteristic
                 */
                char_uuid.len = 2;
                if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROV_VAL) {
                    char_uuid.uu.uuid16 = BT_UUID_MESH_PROV_DATA_IN_VAL + j;
                } else if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROXY_VAL) {
                    char_uuid.uu.uuid16 = BT_UUID_MESH_PROXY_DATA_IN_VAL + j;
                }

                BTA_GATTC_GetCharByUUID(conn_id, gattc_info[i].start_handle,
                                        gattc_info[i].end_handle, char_uuid, &result, &num);

                if (!result) {
                    bt_gattc_disconnect(conn);
                    return;
                }

                if (num != 1) {
                    osi_free(result);
                    bt_gattc_disconnect(conn);
                    return;
                }

                if (!j) {
                    if (!(result[0].properties & BT_GATT_CHRC_WRITE_WITHOUT_RESP)) {
                        osi_free(result);
                        bt_gattc_disconnect(conn);
                        return;
                    }
                    gattc_info[i].data_in_handle = result[0].attribute_handle;
                } else {
                    if (!(result[0].properties & BT_GATT_CHRC_NOTIFY)) {
                        osi_free(result);
                        bt_gattc_disconnect(conn);
                        return;
                    }
                    gattc_info[i].data_out_handle = result[0].attribute_handle;
                }
                osi_free(result);
                result = NULL;
            }

            /* Register Notification fot Mesh Provisioning/Proxy Data Out Characteristic */
            status = BTA_GATTC_RegisterForNotifications(mesh_gattc_if, gattc_info[i].addr,
                     gattc_info[i].data_out_handle);
            if (status != BTA_GATT_OK) {
                bt_gattc_disconnect(conn);
                return;
            }

            /** After notification is registered, get descriptor number of the
             *  Mesh Provisioning/Proxy Data Out Characteristic
             */
            BTA_GATTC_GetDBSizeByType(conn_id, BTGATT_DB_DESCRIPTOR, gattc_info[i].start_handle,
                                      gattc_info[i].end_handle, gattc_info[i].data_out_handle, &num);
            if (!num) {
                bt_gattc_disconnect(conn);
                return;
            }

            /* Get CCC of Mesh Provisioning/Proxy Data Out Characteristic */
            char_uuid.len = 2;
            char_uuid.uu.uuid16 = BT_UUID_GATT_CCC_VAL;
            BTA_GATTC_GetDescrByCharHandle(conn_id, gattc_info[i].data_out_handle,
                                           char_uuid, &result, &num);

            if (!result) {
                bt_gattc_disconnect(conn);
                return;
            }

            if (num != 1) {
                osi_free(result);
                bt_gattc_disconnect(conn);
                return;
            }

            gattc_info[i].ccc_handle = result[0].attribute_handle;

            /** Enable Notification of Mesh Provisioning/Proxy Data Out
             *  Characteristic Descriptor.
             */
            write.len = sizeof(notify_en);
            write.p_value = (u8_t *)&notify_en;
            BTA_GATTC_WriteCharDescr(conn_id, result[0].attribute_handle,
                                     BTA_GATTC_TYPE_WRITE, &write, BTA_GATT_AUTH_REQ_NONE);

            osi_free(result);
            result = NULL;
        }
    }
    break;

    case BTA_GATTC_READ_DESCR_EVT:
        break;

    case BTA_GATTC_WRITE_DESCR_EVT:
        if (p_data->write.status == BTA_GATT_OK) {
            BT_DBG("BTA_GATTC_WRITE_DESCR_EVT");

            handle = MESH_GATT_GET_CONN_ID(p_data->write.conn_id);

            for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
                if (gattc_info[i].conn.handle == handle) {
                    break;
                }
            }

            if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
                BT_ERR("BTA_GATTC_WRITE_DESCR_EVT: conn_id not found");
                return;
            }

            conn = &gattc_info[i].conn;

            if (gattc_info[i].ccc_handle != p_data->write.handle) {
                BT_WARN("BTA_GATTC_WRITE_DESCR_EVT: ccc_handle not match");
                bt_gattc_disconnect(conn);
                return;
            }

            if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROV_VAL) {
                if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->prov_write_descr != NULL) {
                    len = mesh_prov_conn_cb->prov_write_descr(&gattc_info[i].conn, gattc_info[i].addr);
                    if (len < 0) {
                        BT_ERR("prov_write_descr fail");
                        bt_gattc_disconnect(conn);
                        return;
                    }
                    gattc_info[i].wr_desc_done = true;
                }
            } else if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROXY_VAL) {
                if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->proxy_write_descr != NULL) {
                    len = mesh_prov_conn_cb->proxy_write_descr(&gattc_info[i].conn);
                    if (len < 0) {
                        BT_ERR("proxy_write_descr fail");
                        bt_gattc_disconnect(conn);
                        return;
                    }
                }
            }
        }
        break;

    case BTA_GATTC_NOTIF_EVT: {
        BT_DBG("BTA_GATTC_NOTIF_EVT");

        handle = MESH_GATT_GET_CONN_ID(p_data->notify.conn_id);

        for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
            if (gattc_info[i].conn.handle == handle) {
                break;
            }
        }

        if (i == CONFIG_BT_MESH_PBG_SAME_TIME) {
            BT_ERR("BTA_GATTC_WRITE_DESCR_EVT: conn_id not found");
            return;
        }

        conn = &gattc_info[i].conn;

        if (memcmp(gattc_info[i].addr, p_data->notify.bda, 6) ||
                gattc_info[i].data_out_handle != p_data->notify.handle ||
                p_data->notify.is_notify == false) {
            BT_ERR("Notification error");
            bt_gattc_disconnect(conn);
            return;
        }

        if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROV_VAL) {
            if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->prov_notify != NULL) {
                len = mesh_prov_conn_cb->prov_notify(&gattc_info[i].conn,
                                                     p_data->notify.value, p_data->notify.len);
                if (len < 0) {
                    BT_ERR("Receive prov_notify fail");
                    bt_gattc_disconnect(conn);
                    return;
                }
            }
        } else if (gattc_info[i].srvc_uuid == BT_UUID_MESH_PROXY_VAL) {
            if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->proxy_notify != NULL) {
                len = mesh_prov_conn_cb->proxy_notify(&gattc_info[i].conn,
                                                      p_data->notify.value, p_data->notify.len);
                if (len < 0) {
                    BT_ERR("Receive proxy_notify fail");
                    bt_gattc_disconnect(conn);
                    return;
                }
            }
        }
    }
    break;

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
        /** After current connection is established, provisioner can
         *  use BTA_DmBleScan() to re-enable scan.
         */
        if (!atomic_test_and_set_bit(bt_dev.flags, BT_DEV_EXPLICIT_SCAN)) {
            BTA_DmBleScan(true, 0, scan_result_callback, NULL);
        }
        break;

    case BTA_GATTC_CLOSE_EVT:
        BT_DBG("BTA_GATTC_CLOSE_EVT");
        break;

    case BTA_GATTC_CONNECT_EVT: {
        BT_DBG("BTA_GATTC_CONNECT_EVT");

        if (mesh_gattc_if != p_data->connect.client_if) {
            BT_ERR("mesh_gattc_if & connect_if don't match");
            return;
        }

        if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->connected != NULL) {
            for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
                if (!memcmp(gattc_info[i].addr, p_data->connect.remote_bda, 6)) {
                    gattc_info[i].conn.handle = MESH_GATT_GET_CONN_ID(p_data->connect.conn_id);
                    (mesh_prov_conn_cb->connected)(&gattc_info[i].conn, i);
                    break;
                }
            }
        }
    }
    break;

    case BTA_GATTC_DISCONNECT_EVT: {
        BT_DBG("BTA_GATTC_DISCONNECT_EVT");

        if (mesh_gattc_if != p_data->disconnect.client_if) {
            BT_ERR("mesh_gattc_if & disconnect_if don't match");
            return;
        } else {
            /* Increment pbg_count */
            provisioner_pbg_count_dec();
        }

        handle = MESH_GATT_GET_CONN_ID(p_data->disconnect.conn_id);

        if (mesh_prov_conn_cb != NULL && mesh_prov_conn_cb->disconnected != NULL) {
            for (i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
                if (!memcmp(gattc_info[i].addr, p_data->disconnect.remote_bda, 6)) {
                    if (gattc_info[i].conn.handle == handle) {
                        (mesh_prov_conn_cb->disconnected)(&gattc_info[i].conn, p_data->disconnect.reason);
                        if (!gattc_info[i].wr_desc_done) {
                            /** Add this in case connection is established, connected event comes, but
                             *  connection is terminated before server->filter_type is set to PROV.
                             */
                            provisioner_clear_connecting(i);
                        }
                    } else {
                        /**
                         *  Add this in case connection is failed to be established, and here we
                         *  need to clear some provision link info, like connecting flag, device
                         *  uuid, address info, etc.
                         */
                        provisioner_clear_connecting(i);
                    }
                    memset(&gattc_info[i], 0, sizeof(gattc_info[i]));
                    gattc_info[i].conn.handle  = 0xFFFF;
                    gattc_info[i].mtu          = 23;
                    gattc_info[i].wr_desc_done = false;
                    break;
                }
            }
        }
    }
    break;

    case BTA_GATTC_CONGEST_EVT:
        break;

    case BTA_GATTC_SRVC_CHG_EVT:
        break;

    default:
        break;
    }
}

#endif /* CONFIG_BT_MESH_PROVISIONER */

void bt_conn_cb_register(struct bt_conn_cb *cb)
{
    mesh_conn_cb = cb;
    /**TODO: currently we register gatt in bt_mesh_init */
    //bt_mesh_gatt_init();
    return;
}

struct bt_gatt_attr *bt_gatt_find_attr_by_handle(u16_t handle)
{
    struct bt_gatt_service *svc;
    struct bt_gatt_attr *attr = NULL;
    SYS_SLIST_FOR_EACH_CONTAINER(&db, svc, node) {
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

void bt_gatt_foreach_attr(u16_t start_handle, u16_t end_handle,
                          bt_gatt_attr_func_t func, void *user_data)
{
    struct bt_gatt_service *svc;

    SYS_SLIST_FOR_EACH_CONTAINER(&db, svc, node) {
        int i;

        for (i = 0; i < svc->attr_count; i++) {
            struct bt_gatt_attr *attr = &svc->attrs[i];

            /* Check if attribute handle is within range */
            if (attr->handle < start_handle ||
                    attr->handle > end_handle) {
                continue;
            }

            if (func(attr, user_data) == BT_GATT_ITER_STOP) {
                return;
            }
        }
    }
}

static u8_t find_next(const struct bt_gatt_attr *attr, void *user_data)
{
    struct bt_gatt_attr **next = user_data;

    *next = (struct bt_gatt_attr *)attr;

    return BT_GATT_ITER_STOP;
}

struct bt_gatt_attr *bt_gatt_attr_next(const struct bt_gatt_attr *attr)
{
    struct bt_gatt_attr *next = NULL;

    bt_gatt_foreach_attr(attr->handle + 1, attr->handle + 1, find_next,
                         &next);

    return next;
}

ssize_t bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, u16_t buf_len, u16_t offset,
                          const void *value, u16_t value_len)
{
    u16_t len;

    if (offset > value_len) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    len = min(buf_len, value_len - offset);

    BT_DBG("handle 0x%04x offset %u length %u", attr->handle, offset,
           len);

    memcpy(buf, value + offset, len);

    return len;
}

struct gatt_incl {
    u16_t start_handle;
    u16_t end_handle;
    u16_t uuid16;
} __packed;

ssize_t bt_gatt_attr_read_included(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, u16_t len, u16_t offset)
{
    struct bt_gatt_attr *incl = attr->user_data;
    struct bt_uuid *uuid = incl->user_data;
    struct gatt_incl pdu;
    u8_t value_len;

    /* first attr points to the start handle */
    pdu.start_handle = sys_cpu_to_le16(incl->handle);
    value_len = sizeof(pdu.start_handle) + sizeof(pdu.end_handle);

    /*
     * Core 4.2, Vol 3, Part G, 3.2,
     * The Service UUID shall only be present when the UUID is a
     * 16-bit Bluetooth UUID.
     */
    if (uuid->type == BT_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);
        value_len += sizeof(pdu.uuid16);
    }

    /* Lookup for service end handle */
    //bt_gatt_foreach_attr(incl->handle + 1, 0xffff, get_service_handles,
    //           &pdu);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}


ssize_t bt_gatt_attr_read_service(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf, u16_t len, u16_t offset)
{
    struct bt_uuid *uuid = attr->user_data;

    if (uuid->type == BT_UUID_TYPE_16) {
        u16_t uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);

        return bt_gatt_attr_read(conn, attr, buf, len, offset,
                                 &uuid16, 2);
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             BT_UUID_128(uuid)->val, 16);
}

struct gatt_chrc {
    u8_t properties;
    u16_t value_handle;
    union {
        u16_t uuid16;
        u8_t  uuid[16];
    };
} __packed;

ssize_t bt_gatt_attr_read_chrc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               u16_t len, u16_t offset)
{
    struct bt_gatt_chrc *chrc = attr->user_data;
    struct gatt_chrc pdu;
    const struct bt_gatt_attr *next;
    u8_t value_len;

    pdu.properties = chrc->properties;
    /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part G] page 534:
     * 3.3.2 Characteristic Value Declaration
     * The Characteristic Value declaration contains the value of the
     * characteristic. It is the first Attribute after the characteristic
     * declaration. All characteristic definitions shall have a
     * Characteristic Value declaration.
     */
    next = bt_gatt_attr_next(attr);
    if (!next) {
        BT_WARN("No value for characteristic at 0x%04x", attr->handle);
        pdu.value_handle = 0x0000;
    } else {
        pdu.value_handle = sys_cpu_to_le16(next->handle);
    }
    value_len = sizeof(pdu.properties) + sizeof(pdu.value_handle);

    if (chrc->uuid->type == BT_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(chrc->uuid)->val);
        value_len += 2;
    } else {
        memcpy(pdu.uuid, BT_UUID_128(chrc->uuid)->val, 16);
        value_len += 16;
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

static void bta_uuid_to_mesh_uuid(tBT_UUID *bta_uuid, const struct bt_uuid  *uuid)
{
    assert(uuid != NULL && bta_uuid != NULL);
    if (uuid->type == BT_UUID_TYPE_16) {
        bta_uuid->len = LEN_UUID_16;
        bta_uuid->uu.uuid16 = BT_UUID_16(uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_32) {
        bta_uuid->len = LEN_UUID_32;
        bta_uuid->uu.uuid32 = BT_UUID_32(uuid)->val;
    } else if (uuid->type == BT_UUID_TYPE_128) {
        bta_uuid->len = LEN_UUID_128;
        memcpy(bta_uuid->uu.uuid128, BT_UUID_128(uuid)->val, LEN_UUID_128);
    } else {
        BT_ERR("%s, Invalid mesh uuid type = %d", __func__, uuid->type);
    }

    return;
}

static int gatt_register(struct bt_gatt_service *svc)
{
    struct bt_gatt_service *last;
    u16_t handle;
    //struct bt_gatt_attr *attrs = svc->attrs;
    //u16_t count = svc->attr_count;

    if (sys_slist_is_empty(&db)) {
        handle = 0;
        goto populate;
    }

    last = SYS_SLIST_PEEK_TAIL_CONTAINER(&db, last, node);
    handle = last->attrs[last->attr_count - 1].handle;
    BT_DBG("%s, handle =  %d", __func__, handle);
populate:
#if 0
    /* Populate the handles and append them to the list */
    for (; attrs && count; attrs++, count--) {
        if (!attrs->handle) {
            /* Allocate handle if not set already */
            attrs->handle = ++handle;
        } else if (attrs->handle > handle) {
            /* Use existing handle if valid */
            handle = attrs->handle;
        } else {
            /* Service has conflicting handles */
            BT_ERR("Unable to register handle 0x%04x",
                   attrs->handle);
            return -EINVAL;
        }

        BT_DBG("attr %p handle 0x%04x uuid %s perm 0x%02x",
               attrs, attrs->handle, bt_uuid_str(attrs->uuid),
               attrs->perm);
    }
#endif
    sys_slist_append(&db, &svc->node);

    return 0;
}

static tBTA_GATT_PERM mesh_perm_to_bta_perm(u8_t perm)
{
    tBTA_GATT_PERM bta_perm = 0;
    if ((perm & BT_GATT_PERM_READ) == BT_GATT_PERM_READ) {
        bta_perm |= BTA_GATT_PERM_READ;
    }

    if ((perm & BT_GATT_PERM_WRITE) == BT_GATT_PERM_WRITE) {
        bta_perm |= BTA_GATT_PERM_WRITE;
    }

    if ((perm & BT_GATT_PERM_READ_ENCRYPT) ==  BT_GATT_PERM_READ_ENCRYPT) {
        bta_perm |= BTA_GATT_PERM_READ_ENCRYPTED;
    }

    if ((perm & BT_GATT_PERM_WRITE_ENCRYPT) == BT_GATT_PERM_WRITE_ENCRYPT) {
        bta_perm |= BTA_GATT_PERM_WRITE_ENCRYPTED;
    }

    if ((perm & BT_GATT_PERM_READ_AUTHEN) == BT_GATT_PERM_READ_AUTHEN) {
        bta_perm |= BTA_GATT_PERM_READ_ENC_MITM;
    }

    if ((perm & BT_GATT_PERM_WRITE_AUTHEN) == BT_GATT_PERM_WRITE_AUTHEN) {
        bta_perm |= BTA_GATT_PERM_WRITE_ENC_MITM;
    }

    return bta_perm;
}

int bt_gatt_service_register(struct bt_gatt_service *svc)
{
    assert(svc != NULL);
    tBT_UUID bta_uuid = {0};

    for (int i = 0; i < svc->attr_count; i++) {
        if (svc->attrs[i].uuid->type == BT_UUID_TYPE_16) {
            switch (BT_UUID_16(svc->attrs[i].uuid)->val) {
            case BT_UUID_GATT_PRIMARY_VAL: {
                future_mesh = future_new();
                bta_uuid_to_mesh_uuid(&bta_uuid, (struct bt_uuid *)svc->attrs[i].user_data);
                BTA_GATTS_CreateService(mesh_gatts_if,
                                        &bta_uuid, 0, svc->attr_count, true);
                if (future_await(future_mesh) == FUTURE_FAIL) {
                    BT_ERR("add primary service failed.");
                    return ESP_FAIL;
                }
                svc->attrs[i].handle = svc_handle;
                BT_DBG("############## create service ############");
                BT_DBG("add pri service: svc_uuid = %x, perm = %d, svc_handle = %d", bta_uuid.uu.uuid16, svc->attrs[i].perm, svc_handle);
                break;
            }
            case BT_UUID_GATT_SECONDARY_VAL: {
                future_mesh = future_new();
                bta_uuid_to_mesh_uuid(&bta_uuid, (struct bt_uuid *)svc->attrs[i].user_data);
                BTA_GATTS_CreateService(mesh_gatts_if,
                                        &bta_uuid, 0, svc->attr_count, false);
                if (future_await(future_mesh) == FUTURE_FAIL) {
                    BT_ERR("add secondary service failed.");
                    return ESP_FAIL;
                }
                svc->attrs[i].handle = svc_handle;
                BT_DBG("add sec service: svc_uuid = %x, perm = %d, svc_handle = %d", bta_uuid.uu.uuid16, svc->attrs[i].perm, svc_handle);
                break;
            }
            case BT_UUID_GATT_INCLUDE_VAL: {
                break;
            }
            case BT_UUID_GATT_CHRC_VAL: {
                future_mesh = future_new();
                struct bt_gatt_chrc *gatt_chrc = (struct bt_gatt_chrc *)svc->attrs[i].user_data;
                bta_uuid_to_mesh_uuid(&bta_uuid, gatt_chrc->uuid);
                BTA_GATTS_AddCharacteristic(svc_handle, &bta_uuid, mesh_perm_to_bta_perm(svc->attrs[i + 1].perm), gatt_chrc->properties, NULL, NULL);
                if (future_await(future_mesh) == FUTURE_FAIL) {
                    BT_ERR("Add characristic failed.");
                    return ESP_FAIL;
                }
                /* All the characristic should have two handle: the declaration handle and the value handle */
                svc->attrs[i].handle = char_handle - 1;
                svc->attrs[i + 1].handle =  char_handle;
                BT_DBG("add char: char_uuid = %x, char_handle = %d, perm = %d, char_pro = %d", BT_UUID_16(gatt_chrc->uuid)->val, char_handle, svc->attrs[i + 1].perm, gatt_chrc->properties);
                break;
            }
            case BT_UUID_GATT_CEP_VAL:
            case BT_UUID_GATT_CUD_VAL:
            case BT_UUID_GATT_CCC_VAL:
            case BT_UUID_GATT_SCC_VAL:
            case BT_UUID_GATT_CPF_VAL:
            case BT_UUID_VALID_RANGE_VAL:
            case BT_UUID_HIDS_EXT_REPORT_VAL:
            case BT_UUID_HIDS_REPORT_REF_VAL:
            case BT_UUID_ES_CONFIGURATION_VAL:
            case BT_UUID_ES_MEASUREMENT_VAL:
            case BT_UUID_ES_TRIGGER_SETTING_VAL: {
                future_mesh = future_new();
                bta_uuid_to_mesh_uuid(&bta_uuid, svc->attrs[i].uuid);
                BTA_GATTS_AddCharDescriptor(svc_handle, mesh_perm_to_bta_perm(svc->attrs[i].perm), &bta_uuid, NULL, NULL);
                if (future_await(future_mesh) == FUTURE_FAIL) {
                    BT_ERR("add primary service failed.");
                    return ESP_FAIL;
                }
                svc->attrs[i].handle = char_handle;
                BT_DBG("add descr: descr_uuid = %x, perm= %d, descr_handle = %d", BT_UUID_16(svc->attrs[i].uuid)->val, svc->attrs[i].perm, char_handle);
                break;
            }
            default:
                break;
            }
        }
    }

    if (svc_handle != 0) {
        /**TODO: Currently we start service according to function like
         * bt_mesh_proxy_gatt_enable, etc*/
        //BTA_GATTS_StartService(svc_handle, BTA_GATT_TRANSPORT_LE);
        svc_handle = 0;
    }
    BT_DBG("#######################################");
    // Still should regitster to the adapt db.
    gatt_register(svc);
    return 0;
}


int bt_conn_disconnect(struct bt_conn *conn, u8_t reason)
{
    UNUSED(reason);
    BTA_GATTS_Close(conn->handle);
    return 0;
}

struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
    atomic_inc(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, atomic_get(&conn->ref));

    return conn;
}

void bt_conn_unref(struct bt_conn *conn)
{
    atomic_dec(&conn->ref);

    BT_DBG("handle %u ref %u", conn->handle, atomic_get(&conn->ref));
}

int bt_gatt_service_unregister(struct bt_gatt_service *svc)
{
    assert(svc != NULL);

    BTA_GATTS_DeleteService(svc->attrs[0].handle);
    return 0;
}

int bt_gatt_notify(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                   const void *data, u16_t len)
{
    //MESH_GATT_CREATE_CONN_ID
    uint16_t conn_id = MESH_GATT_CREATE_CONN_ID(mesh_gatts_if, conn->handle);
    BTA_GATTS_HandleValueIndication(conn_id, attr->handle, len, (uint8_t *)data, false);
    return 0;
}

u16_t bt_gatt_get_mtu(struct bt_conn *conn)
{
    return BTA_GATT_GetLocalMTU();
}

int bt_rand(void *buf, size_t len)
{
    if (!len) {
        return -EAGAIN;
    }
    // Reset the buf value to the fixed value.
    memset(buf, 0x55, len);

    for (int i = 0; i < (int)(len / sizeof(uint32_t)); i++) {
        uint32_t rand = esp_random();
        memcpy(buf + i * sizeof(uint32_t), &rand, sizeof(uint32_t));
    }

    BT_DBG("%s, rand: %s", __func__, bt_hex(buf, len));
    return 0;
}

void bt_mesh_gatt_init(void)
{
    tBT_UUID app_uuid = {LEN_UUID_128, {0}};

#if CONFIG_BT_MESH_NODE
    /* Fill our internal UUID with a fixed pattern 0x96 for the ble mesh */
    memset (&app_uuid.uu.uuid128, 0x96, LEN_UUID_128);

    BTA_GATTS_AppRegister(&app_uuid, mesh_bta_gatts_cb);
#endif

#if CONFIG_BT_MESH_PROVISIONER
    BTA_GATT_SetLocalMTU(69);

    for (int i = 0; i < CONFIG_BT_MESH_PBG_SAME_TIME; i++) {
        gattc_info[i].conn.handle  = 0xFFFF;
        gattc_info[i].mtu          = 23; /* Default MTU_SIZE 23 */
        gattc_info[i].wr_desc_done = false;
    }

    memset(&app_uuid.uu.uuid128, BT_MESH_GATTC_APP_UUID_BYTE, LEN_UUID_128);
    BTA_GATTC_AppRegister(&app_uuid, mesh_bta_gattc_cb);
#endif
}

void ecb_encrypt(u8_t const *const key_le, u8_t const *const clear_text_le,
                 u8_t *const cipher_text_le, u8_t *const cipher_text_be)
{
    struct ecb_param ecb;
    mbedtls_aes_context aes_ctx = {0};

    aes_ctx.key_bytes = 16;
    mem_rcopy(&aes_ctx.key[0], key_le, 16);
    mem_rcopy(&ecb.clear_text[0], clear_text_le, sizeof(ecb.clear_text));
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, &ecb.clear_text[0], &ecb.cipher_text[0]);

    if (cipher_text_le) {
        mem_rcopy(cipher_text_le, &ecb.cipher_text[0],
                  sizeof(ecb.cipher_text));
    }

    if (cipher_text_be) {
        memcpy(cipher_text_be, &ecb.cipher_text[0],
               sizeof(ecb.cipher_text));
    }
}

void ecb_encrypt_be(u8_t const *const key_be, u8_t const *const clear_text_be,
                    u8_t *const cipher_text_be)
{
    struct ecb_param ecb;
    mbedtls_aes_context aes_ctx = {0};

    aes_ctx.key_bytes = 16;
    memcpy(&aes_ctx.key[0], key_be, 16);
    memcpy(&ecb.clear_text[0], clear_text_be, sizeof(ecb.clear_text));
    mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, &ecb.clear_text[0], &ecb.cipher_text[0]);

    memcpy(cipher_text_be, &ecb.cipher_text[0], sizeof(ecb.cipher_text));
}

int bt_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
                  u8_t enc_data[16])
{
#if CONFIG_MBEDTLS_HARDWARE_AES
    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    ecb_encrypt(key, plaintext, enc_data, NULL);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));
    return 0;
#else /* CONFIG_MBEDTLS_HARDWARE_AES */
    struct tc_aes_key_sched_struct s;
    u8_t tmp[16];

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    sys_memcpy_swap(tmp, key, 16);

    if (tc_aes128_set_encrypt_key(&s, tmp) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_memcpy_swap(tmp, plaintext, 16);

    if (tc_aes_encrypt(enc_data, tmp, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    sys_mem_swap(enc_data, 16);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */
}

int bt_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
                  u8_t enc_data[16])
{
#if CONFIG_MBEDTLS_HARDWARE_AES
    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    ecb_encrypt_be(key, plaintext, enc_data);

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#else /* CONFIG_MBEDTLS_HARDWARE_AES */

    struct tc_aes_key_sched_struct s;

    BT_DBG("key %s plaintext %s", bt_hex(key, 16), bt_hex(plaintext, 16));

    if (tc_aes128_set_encrypt_key(&s, key) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    if (tc_aes_encrypt(enc_data, plaintext, &s) == TC_CRYPTO_FAIL) {
        return -EINVAL;
    }

    BT_DBG("enc_data %s", bt_hex(enc_data, 16));

    return 0;
#endif /* CONFIG_MBEDTLS_HARDWARE_AES */
}

const u8_t *bt_pub_key_get(void)
{
    Point public_key;
    BT_OCTET32 pri_key;
    if (atomic_test_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY)) {
        return pub_key;
    }
    //memcpy(pri_key, mesh_private_key, 32);
    mem_rcopy(pri_key, mesh_private_key, 32);
    ECC_PointMult(&public_key, &(curve_p256.G), (DWORD *)pri_key, KEY_LENGTH_DWORDS_P256);
#if 0
    mem_rcopy(pub_key, public_key.x, BT_OCTET32_LEN);
    mem_rcopy(pub_key + BT_OCTET32_LEN, public_key.y, BT_OCTET32_LEN);
#else
    memcpy(pub_key, public_key.x, BT_OCTET32_LEN);
    memcpy(pub_key + BT_OCTET32_LEN, public_key.y, BT_OCTET32_LEN);
#endif
    atomic_set_bit(bt_dev.flags, BT_DEV_HAS_PUB_KEY);
    BT_DBG("gen the pub_key:%s", bt_hex(pub_key, sizeof(pub_key)));
    return pub_key;
}

void bt_mesh_adapt_init(void)
{
    BT_DBG("%s", __func__);
    //btsnd_hcic_ble_read_local_p256_public_key();
    btsnd_hcic_ble_rand(bt_mesh_rand_cb);
    /* initialization of P-256 parameters */
    p_256_init_curve(KEY_LENGTH_DWORDS_P256);
}

static void bt_mesh_rand_cb(tBTM_RAND_ENC *p)
{
    //if (p && p->opcode == HCI_BLE_RAND) {
    BT_DBG("%s, Generate the rand: %s", __func__, bt_hex(p->param_buf, p->param_len));
    memcpy(mesh_private_key, p->param_buf, p->param_len);
    //}
}

int bt_dh_key_gen(const u8_t remote_pk[64], bt_dh_key_cb_t cb)
{
    Point       peer_publ_key;
    Point    new_publ_key;
    BT_OCTET32  private_key;
    BT_OCTET32  dhkey;
    //mesh_private_key = {0x3f, 0x49, 0xf6, 0xd4, 0xa3, 0xc5, 0x5f, 0x38, 0x74, 0xc9,
    //                    0xb3, 0xe3, 0xd2, 0x10, 0x3f, 0x50, 0x4a, 0xff, 0x60, 0x7b,
    //                    0xeb, 0x40, 0xb7, 0x99, 0x58, 0x99, 0xb8, 0xa6, 0xcd, 0x3c, 0x1a, 0xbd};
    //peer_publ_key.x = {0x1e, 0xa1, 0xf0, 0xf0, 0x1f, 0xaf, 0x1d, 0x96, 0x09, 0x59, 0x22, 0x84,
    //                   0xf1, 0x9e, 0x4c, 0x00, 0x47, 0xb5, 0x8a, 0xfd, 0x86, 0x15, 0xa6, 0x9f,
    //                   0x55, 0x90, 0x77, 0xb2, 0x2f, 0xaa, 0xa1, 0x90};
    //peer_publ_key.y = {0x4c, 0x55, 0xf3, 0x3e, 0x42, 0x9d, 0xad, 0x37, 0x73, 0x56, 0x70, 0x3a,
    //                   0x9a, 0xb8, 0x51, 0x60, 0x47, 0x2d, 0x11, 0x30, 0xe2, 0x8e, 0x36, 0x76,
    //                   0x5f, 0x89, 0xaf, 0xf9, 0x15, 0xb1, 0x21, 0x4a};
    BT_DBG("private key = %s", bt_hex(mesh_private_key, BT_OCTET32_LEN));

    //BT_DBG("remote public key = %s", bt_hex(remote_pk, 64));
    mem_rcopy(private_key, mesh_private_key, BT_OCTET32_LEN);
#if 0 //should not rcopy here, because it has been reverted in send_pub_key
    mem_rcopy(peer_publ_key.x, remote_pk, BT_OCTET32_LEN);
    mem_rcopy(peer_publ_key.y, &remote_pk[BT_OCTET32_LEN], BT_OCTET32_LEN);
#else
    memcpy(peer_publ_key.x, remote_pk, BT_OCTET32_LEN);
    memcpy(peer_publ_key.y, &remote_pk[BT_OCTET32_LEN], BT_OCTET32_LEN);
#endif


#if 0
    mem_rcopy(peer_publ_key.x, test_peer_publ_key, BT_OCTET32_LEN);
    mem_rcopy(peer_publ_key.y, &test_peer_publ_key[BT_OCTET32_LEN], BT_OCTET32_LEN);
#endif
    BT_DBG("peer_publ_key.x = %s", bt_hex(peer_publ_key.x, BT_OCTET32_LEN));
    BT_DBG("peer_publ_key.y = %s", bt_hex(peer_publ_key.y, BT_OCTET32_LEN));
    ECC_PointMult(&new_publ_key, &peer_publ_key, (DWORD *) private_key, KEY_LENGTH_DWORDS_P256);
#if 0
    mem_rcopy(dhkey, new_publ_key.x, BT_OCTET32_LEN);
#endif
#if 0 //should not rcopy here, because it will be reverted in prov_dh_key_cb
    mem_rcopy(dhkey, new_publ_key.x, BT_OCTET32_LEN);
#else
    memcpy(dhkey, new_publ_key.x, BT_OCTET32_LEN);
#endif
    BT_DBG("=== x = %s", bt_hex(new_publ_key.x, 32));
    BT_DBG("===y = %s", bt_hex(new_publ_key.y, 32));

    if (cb != NULL) {
        cb((const uint8_t *)dhkey);
    }

    return 0;
}


/** APIs added by Espressif */

int bt_gatt_service_stop(struct bt_gatt_service *svc)
{
    if (!svc) {
        BT_ERR("%s, svc should not be NULL", __func__);
        return -EINVAL;
    }

    BT_DBG("Stop service:%d\n", svc->attrs[0].handle);

    BTA_GATTS_StopService(svc->attrs[0].handle);
    return 0;
}

int bt_gatt_service_start(struct bt_gatt_service *svc)
{
    if (!svc) {
        BT_ERR("%s, svc should not be NULL", __func__);
        return -EINVAL;
    }

    BT_DBG("Start service:%d\n", svc->attrs[0].handle);

    BTA_GATTS_StartService(svc->attrs[0].handle, BTA_GATT_TRANSPORT_LE);
    return 0;
}

#endif /* #if CONFIG_BT_MESH */

