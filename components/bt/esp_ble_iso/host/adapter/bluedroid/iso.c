/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <../host/conn_internal.h>
#include <../host/iso_internal.h>

#include "stack/hcimsgs.h"
#include "stack/btm_ble_api.h"

#include "bluedroid/btm_error.h"
#include "bluedroid/hci.h"

#include "common/host.h"

LOG_MODULE_REGISTER(ISO_BISO, CONFIG_BT_ISO_LOG_LEVEL);

/* All adapter entrypoints (hci_cmd_* helpers, bt_le_bluedroid_iso_*) are
 * invoked from the iso task — single caller. Neither the DIRECT_HCI
 * path (bt_le_bluedroid_hci_send_sync) nor the fire-and-forget BTM_*
 * calls need bt_le_host_lock; single-task affinity is the serialization.
 *
 * iso_sem is the legacy sync-response mechanism for the USE_DIRECT_HCI=0
 * fallback only (set_cig_params / read_tx_sync): iso_evt_rx fills the
 * file-scope tx_sync / set_cig_params buffers on BTU, then gives the
 * sem; the caller takes the sem and reads the buffers. Under
 * USE_DIRECT_HCI=1 (default) BTU emits no synthesized
 * SET_CIG_PARAMS_EVT / READ_TX_SYNC_EVT for our cmds (they bypass
 * BTM), so iso_sem is created but never given/taken.
 *
 * Lock order on the BTM fallback path:
 *   bt_le_host_lock → k_sem_take(iso_sem)
 * iso_evt_rx runs on BTU and MUST NOT acquire bt_le_host_lock — the
 * blocked taker still holds it.
 *
 * iso_sem timeout (K_SEM_SHORT) is fatal: the caller sets
 * status = BTM_ERR_PROCESSING and does NOT read the response (memset-
 * zero would yield bogus packet_seq_num / cis_handle to the lib).
 * K_SEM_SHORT must exceed the worst-case BTU dispatch chain. */

static struct k_sem iso_sem;

static struct {
    uint8_t  status;
    uint16_t conn_handle;
    uint16_t packet_seq_num;
    uint32_t tx_time_stamp;
    uint32_t time_offset;
} tx_sync;

static struct {
    uint8_t  status;
    uint8_t  cig_id;
    uint8_t  cis_count;
    uint16_t cis_handle[CONFIG_BT_ISO_MAX_CHAN];
} set_cig_params;

static int hci_cmd_read_iso_tx_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t conn_handle;
    tBTM_STATUS status;

    assert(rsp);

    conn_handle = sys_get_le16(buf->data + 3);

    memset(&tx_sync, 0, sizeof(tx_sync));

#if USE_DIRECT_HCI
    /* Return params layout: Connection_Handle(2) | Packet_Seq_Num(2) |
     * TX_Time_Stamp(4) | Time_Offset(3, 24-bit). Mirror BTM mask of
     * high bits on the 16-bit / 24-bit fields. */
    uint8_t rsp_buf[11];

    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_READ_TX_SYNC,
                                           buf->data + 3, 2,
                                           rsp_buf, sizeof(rsp_buf));
    if (status == BTM_SUCCESS) {
        tx_sync.conn_handle    = sys_get_le16(rsp_buf)     & 0x0FFF;
        tx_sync.packet_seq_num = sys_get_le16(rsp_buf + 2);
        tx_sync.tx_time_stamp  = sys_get_le32(rsp_buf + 4);
        tx_sync.time_offset    = sys_get_le24(rsp_buf + 8) & 0xFFFFFF;
        assert(tx_sync.conn_handle == conn_handle);
    }
#else /* USE_DIRECT_HCI */
    bt_le_host_lock();
    status = BTM_BleIsoReadTxSync(conn_handle);
    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]RdIsoTxSyncFail[0x%03x][%02x]", conn_handle, status);
    } else if (k_sem_take(&iso_sem, K_SEM_SHORT) != 0) {
        LOG_ERR("[B]RdIsoTxSyncRspTimeout[0x%03x]", conn_handle);
        status = BTM_ERR_PROCESSING;
    } else {
        assert(tx_sync.conn_handle == conn_handle);
        status = tx_sync.status;
    }
    bt_le_host_unlock();
#endif /* USE_DIRECT_HCI */

    /* On failure return BTM status verbatim and leave *rsp NULL so the caller
     * short-circuits via bluedroid_err_to_errno() before parsing rsp — BTM
     * encodes errors as BTM_HCI_ERROR|hci_status, which is not a valid HCI
     * status byte. Mirrors NimBLE adapter pattern. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_le16(buf, conn_handle);
    net_buf_add_le16(buf, tx_sync.packet_seq_num);
    net_buf_add_le32(buf, tx_sync.tx_time_stamp);
    net_buf_add_le24(buf, tx_sync.time_offset);

    *rsp = buf;

    return 0;
}

static int hci_cmd_set_cig_params(struct net_buf *buf, struct net_buf **rsp)
{
    tBTM_STATUS status;
    uint8_t cis_count;
    uint8_t cig_id;

    assert(rsp);

    cig_id    = buf->data[3];
    cis_count = buf->data[17];

    memset(&set_cig_params, 0, sizeof(set_cig_params));

#if USE_DIRECT_HCI
    /* Forward lib-constructed params verbatim. Return params layout:
     * CIG_ID(1) | CIS_Count(1) | Connection_Handle[CIS_Count](2*N). */
    uint8_t rsp_buf[2 + 2 * CONFIG_BT_ISO_MAX_CHAN];
    uint8_t rsp_cis_count;

    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_SET_CIG_PARAMS,
                                           buf->data + 3, buf->data[2],
                                           rsp_buf, sizeof(rsp_buf));
    if (status == BTM_SUCCESS) {
        set_cig_params.cig_id = rsp_buf[0];
        rsp_cis_count = rsp_buf[1];
        if (rsp_cis_count > CONFIG_BT_ISO_MAX_CHAN) {
            LOG_ERR("[B]CigParamsCisCountOverflow[%u>%d]",
                    rsp_cis_count, CONFIG_BT_ISO_MAX_CHAN);
            rsp_cis_count = CONFIG_BT_ISO_MAX_CHAN;
        }
        set_cig_params.cis_count = rsp_cis_count;
        for (uint8_t i = 0; i < rsp_cis_count; i++) {
            set_cig_params.cis_handle[i] = sys_get_le16(rsp_buf + 2 + i * 2);
        }
        assert(set_cig_params.cig_id == cig_id);
        assert(set_cig_params.cis_count == cis_count);
    }
#else /* USE_DIRECT_HCI */
    struct ble_hci_le_cis_params *cis_params;
    uint32_t sdu_interval_c_to_p;
    uint32_t sdu_interval_p_to_c;
    uint8_t worst_case_sca;
    uint16_t mtl_c_to_p;
    uint16_t mtl_p_to_c;
    uint8_t packing;
    uint8_t framing;

    sdu_interval_c_to_p = sys_get_le24(buf->data + 4);
    sdu_interval_p_to_c = sys_get_le24(buf->data + 7);
    worst_case_sca      = buf->data[10];
    packing             = buf->data[11];
    framing             = buf->data[12];
    mtl_c_to_p          = sys_get_le16(buf->data + 13);
    mtl_p_to_c          = sys_get_le16(buf->data + 15);

    cis_params = calloc(1, cis_count * sizeof(struct ble_hci_le_cis_params));
    assert(cis_params);

    for (size_t i = 0; i < cis_count; i++) {
        cis_params[i].cis_id         = buf->data[18 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].max_sdu_c_to_p = sys_get_le16(buf->data + 19 + i * sizeof(struct ble_hci_le_cis_params));
        cis_params[i].max_sdu_p_to_c = sys_get_le16(buf->data + 21 + i * sizeof(struct ble_hci_le_cis_params));
        cis_params[i].phy_c_to_p     = buf->data[23 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].phy_p_to_c     = buf->data[24 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].rtn_c_to_p     = buf->data[25 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].rtn_p_to_c     = buf->data[26 + i * sizeof(struct ble_hci_le_cis_params)];
    }

    bt_le_host_lock();
    status = BTM_BleSetCigParams(cig_id,
                                 sdu_interval_c_to_p,
                                 sdu_interval_p_to_c,
                                 worst_case_sca,
                                 packing,
                                 framing,
                                 mtl_c_to_p,
                                 mtl_p_to_c,
                                 cis_count,
                                 (void *)cis_params);
    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]SetCigParamsFail[%u][%02x]", cig_id, status);
    } else if (k_sem_take(&iso_sem, K_SEM_SHORT) != 0) {
        LOG_ERR("[B]SetCigParamsRspTimeout[%u]", cig_id);
        status = BTM_ERR_PROCESSING;
    } else {
        assert(set_cig_params.cig_id == cig_id);
        assert(set_cig_params.cis_count == cis_count);
        status = set_cig_params.status;
        if (status) {
            LOG_ERR("[B]SetCigParamsCtrlFail[%u][%02x]", cig_id, status);
        }
    }
    bt_le_host_unlock();
    free(cis_params);
#endif /* USE_DIRECT_HCI */

    /* See hci_cmd_read_iso_tx_sync — same fail-path contract. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_u8(buf, cig_id);
    net_buf_add_u8(buf, cis_count);
    for (size_t i = 0; i < cis_count; i++) {
        net_buf_add_le16(buf, set_cig_params.cis_handle[i]);
    }

    *rsp = buf;

    return 0;
}

static int hci_cmd_set_cig_params_test(struct net_buf *buf, struct net_buf **rsp)
{
    tBTM_STATUS status;
    uint8_t cis_count;
    uint8_t cig_id;

    assert(rsp);

    cig_id    = buf->data[3];
    cis_count = buf->data[17];

    memset(&set_cig_params, 0, sizeof(set_cig_params));

#if USE_DIRECT_HCI
    /* Return params layout matches SET_CIG_PARAMS:
     * CIG_ID(1) | CIS_Count(1) | Connection_Handle[CIS_Count](2*N). */
    uint8_t rsp_buf[2 + 2 * CONFIG_BT_ISO_MAX_CHAN];
    uint8_t rsp_cis_count;

    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_SET_CIG_PARAMS_TEST,
                                           buf->data + 3, buf->data[2],
                                           rsp_buf, sizeof(rsp_buf));
    if (status == BTM_SUCCESS) {
        set_cig_params.cig_id = rsp_buf[0];
        rsp_cis_count = rsp_buf[1];
        if (rsp_cis_count > CONFIG_BT_ISO_MAX_CHAN) {
            LOG_ERR("[B]CigParamsCisCountOverflow[%u>%d]",
                    rsp_cis_count, CONFIG_BT_ISO_MAX_CHAN);
            rsp_cis_count = CONFIG_BT_ISO_MAX_CHAN;
        }
        set_cig_params.cis_count = rsp_cis_count;
        for (uint8_t i = 0; i < rsp_cis_count; i++) {
            set_cig_params.cis_handle[i] = sys_get_le16(rsp_buf + 2 + i * 2);
        }
        assert(set_cig_params.cig_id == cig_id);
        assert(set_cig_params.cis_count == cis_count);
    }
#else /* USE_DIRECT_HCI */
    struct ble_hci_le_cis_params_test *cis_params;
    uint32_t sdu_interval_c_to_p;
    uint32_t sdu_interval_p_to_c;
    uint8_t worst_case_sca;
    uint16_t iso_interval;
    uint8_t ft_c_to_p;
    uint8_t ft_p_to_c;
    uint8_t packing;
    uint8_t framing;

    sdu_interval_c_to_p = sys_get_le24(buf->data + 4);
    sdu_interval_p_to_c = sys_get_le24(buf->data + 7);
    ft_c_to_p           = buf->data[10];
    ft_p_to_c           = buf->data[11];
    iso_interval        = sys_get_le16(buf->data + 12);
    worst_case_sca      = buf->data[14];
    packing             = buf->data[15];
    framing             = buf->data[16];

    cis_params = calloc(1, cis_count * sizeof(struct ble_hci_le_cis_params_test));
    assert(cis_params);

    for (size_t i = 0; i < cis_count; i++) {
        cis_params[i].cis_id         = buf->data[18 + i * sizeof(struct ble_hci_le_cis_params_test)];
        cis_params[i].nse            = buf->data[19 + i * sizeof(struct ble_hci_le_cis_params_test)];
        cis_params[i].max_sdu_c_to_p = sys_get_le16(buf->data + 20 + i * sizeof(struct ble_hci_le_cis_params_test));
        cis_params[i].max_sdu_p_to_c = sys_get_le16(buf->data + 22 + i * sizeof(struct ble_hci_le_cis_params_test));
        cis_params[i].max_pdu_c_to_p = sys_get_le16(buf->data + 24 + i * sizeof(struct ble_hci_le_cis_params_test));
        cis_params[i].max_pdu_p_to_c = sys_get_le16(buf->data + 26 + i * sizeof(struct ble_hci_le_cis_params_test));
        cis_params[i].phy_c_to_p     = buf->data[28 + i * sizeof(struct ble_hci_le_cis_params_test)];
        cis_params[i].phy_p_to_c     = buf->data[29 + i * sizeof(struct ble_hci_le_cis_params_test)];
        cis_params[i].bn_c_to_p      = buf->data[30 + i * sizeof(struct ble_hci_le_cis_params_test)];
        cis_params[i].bn_p_to_c      = buf->data[31 + i * sizeof(struct ble_hci_le_cis_params_test)];
    }

    bt_le_host_lock();
    status = BTM_BleSetCigParamsTest(cig_id,
                                     sdu_interval_c_to_p,
                                     sdu_interval_p_to_c,
                                     ft_c_to_p,
                                     ft_p_to_c,
                                     iso_interval,
                                     worst_case_sca,
                                     packing,
                                     framing,
                                     cis_count,
                                     (void *)cis_params);
    if (status != BTM_SUCCESS) {
        LOG_ERR("[B]SetCigParamsTestFail[%u][%02x]", cig_id, status);
    } else if (k_sem_take(&iso_sem, K_SEM_SHORT) != 0) {
        LOG_ERR("[B]SetCigParamsTestRspTimeout[%u]", cig_id);
        status = BTM_ERR_PROCESSING;
    } else {
        assert(set_cig_params.cig_id == cig_id);
        assert(set_cig_params.cis_count == cis_count);
        status = set_cig_params.status;
        if (status) {
            LOG_ERR("[B]SetCigParamsTestCtrlFail[%u][%02x]", cig_id, status);
        }
    }
    bt_le_host_unlock();
    free(cis_params);
#endif /* USE_DIRECT_HCI */

    /* See hci_cmd_read_iso_tx_sync — same fail-path contract. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_u8(buf, cig_id);
    net_buf_add_u8(buf, cis_count);
    for (size_t i = 0; i < cis_count; i++) {
        net_buf_add_le16(buf, set_cig_params.cis_handle[i]);
    }

    *rsp = buf;

    return 0;
}

static int hci_cmd_create_cis(struct net_buf *buf, struct net_buf **rsp)
{
    struct ble_hci_cis_hdls *cis_params;
    tBTM_STATUS status;
    uint8_t cis_count;

    ARG_UNUSED(rsp);

    cis_count = buf->data[3];

    cis_params = calloc(1, cis_count * sizeof(struct ble_hci_cis_hdls));
    assert(cis_params);

    for (size_t i = 0; i < cis_count; i++) {
        cis_params[i].cis_hdl  = sys_get_le16(buf->data + 4 + i * sizeof(struct ble_hci_cis_hdls));
        cis_params[i].acl_hdl = sys_get_le16(buf->data + 6 + i * sizeof(struct ble_hci_cis_hdls));
    }

    /* No direct_hci variant: HCI Create CIS returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_CIS_ESTABLISHED_EVT. */
    status = BTM_BleCreateCis(cis_count, (void *)cis_params);

    net_buf_unref(buf);
    free(cis_params);

    return status;
}

static int hci_cmd_remove_cig(struct net_buf *buf, struct net_buf **rsp)
{
    tBTM_STATUS status;

    ARG_UNUSED(rsp);

#if USE_DIRECT_HCI
    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_REMOVE_CIG,
                                           buf->data + 3, 1, NULL, 0);
#else /* USE_DIRECT_HCI */
    uint8_t cig_id = buf->data[3];
    status = BTM_BleRemoveCig(cig_id);
#endif /* USE_DIRECT_HCI */

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_accept_cis_req(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t cis_handle;
    tBTM_STATUS status;

    ARG_UNUSED(rsp);

    cis_handle = sys_get_le16(buf->data + 3);

    /* No direct_hci variant: HCI Accept CIS Request returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_CIS_ESTABLISHED_EVT. */
    status = BTM_BleAcceptCisReq(cis_handle);

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_reject_cis_req(struct net_buf *buf, struct net_buf **rsp)
{
    tBTM_STATUS status;

    ARG_UNUSED(rsp);

#if USE_DIRECT_HCI
    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_REJECT_CIS_REQ,
                                           buf->data + 3, 3, NULL, 0);
#else /* USE_DIRECT_HCI */
    uint16_t cis_handle = sys_get_le16(buf->data + 3);
    uint8_t reason = buf->data[5];
    status = BTM_BleRejectCisReq(cis_handle, reason);
#endif /* USE_DIRECT_HCI */

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_create_big(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint32_t sdu_interval;
    uint16_t max_sdu;
    uint16_t mtl;
    uint8_t rtn;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t encryption;
    uint8_t *bst_code;
    tBTM_STATUS status;

    ARG_UNUSED(rsp);

    big_handle   = buf->data[3];
    adv_handle   = buf->data[4];
    num_bis      = buf->data[5];
    sdu_interval = sys_get_le24(buf->data + 6);
    max_sdu      = sys_get_le16(buf->data + 9);
    mtl          = sys_get_le16(buf->data + 11);
    rtn          = buf->data[13];
    phy          = buf->data[14];
    packing      = buf->data[15];
    framing      = buf->data[16];
    encryption   = buf->data[17];
    bst_code     = buf->data + 18;

    /* No direct_hci variant: HCI Create BIG returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_BIG_CREATE_COMPLETE_EVT. */
    status = BTM_BleBigCreate(big_handle,
                              adv_handle,
                              num_bis,
                              sdu_interval,
                              max_sdu,
                              mtl,
                              rtn,
                              phy,
                              packing,
                              framing,
                              encryption,
                              bst_code);

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_create_big_test(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    uint8_t adv_handle;
    uint8_t num_bis;
    uint32_t sdu_interval;
    uint16_t iso_interval;
    uint16_t max_sdu;
    uint16_t max_pdu;
    uint8_t nse;
    uint8_t phy;
    uint8_t packing;
    uint8_t framing;
    uint8_t bn;
    uint8_t irc;
    uint8_t pto;
    uint8_t encryption;
    uint8_t *bst_code;
    tBTM_STATUS status;

    ARG_UNUSED(rsp);

    big_handle   = buf->data[3];
    adv_handle   = buf->data[4];
    num_bis      = buf->data[5];
    sdu_interval = sys_get_le24(buf->data + 6);
    iso_interval = sys_get_le16(buf->data + 9);
    nse          = buf->data[11];
    max_sdu      = sys_get_le16(buf->data + 12);
    max_pdu      = sys_get_le16(buf->data + 14);
    phy          = buf->data[16];
    packing      = buf->data[17];
    framing      = buf->data[18];
    bn           = buf->data[19];
    irc          = buf->data[20];
    pto          = buf->data[21];
    encryption   = buf->data[22];
    bst_code     = buf->data + 23;

    /* No direct_hci variant: HCI Create BIG Test returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_BIG_CREATE_COMPLETE_EVT. */
    status = BTM_BleBigCreateTest(big_handle,
                                  adv_handle,
                                  num_bis,
                                  sdu_interval,
                                  iso_interval,
                                  nse,
                                  max_sdu,
                                  max_pdu,
                                  phy,
                                  packing,
                                  framing,
                                  bn,
                                  irc,
                                  pto,
                                  encryption,
                                  bst_code);

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_terminate_big(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    tBTM_STATUS status;
    uint8_t reason;

    ARG_UNUSED(rsp);

    big_handle = buf->data[3];
    reason     = buf->data[4];

    /* No direct_hci variant: HCI Terminate BIG returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT. */
    status = BTM_BleBigTerminate(big_handle, reason);

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_big_create_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t sync_timeout;
    uint16_t sync_handle;
    tBTM_STATUS status;
    uint8_t big_handle;
    uint8_t encryption;
    uint8_t *bst_code;
    uint8_t num_bis;
    uint8_t *bis;
    uint8_t mse;

    ARG_UNUSED(rsp);

    big_handle   = buf->data[3];
    sync_handle  = sys_get_le16(buf->data + 4);
    encryption   = buf->data[6];
    bst_code     = buf->data + 7;
    mse          = buf->data[23];
    sync_timeout = sys_get_le16(buf->data + 24);
    num_bis      = buf->data[26];
    bis          = buf->data + 27;

    /* No direct_hci variant: HCI BIG Create Sync returns Command_Status;
     * outcome arrives via BTM_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT. */
    status = BTM_BleBigSyncCreate(big_handle,
                                  sync_handle,
                                  encryption,
                                  bst_code,
                                  mse,
                                  sync_timeout,
                                  num_bis,
                                  bis);

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_big_terminate_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    tBTM_STATUS status;

    assert(rsp);

    big_handle = buf->data[3];

#if USE_DIRECT_HCI
    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_BIG_TERMINATE_SYNC,
                                           &big_handle, 1, NULL, 0);
#else /* USE_DIRECT_HCI */
    status = BTM_BleBigSyncTerminate(big_handle);
#endif /* USE_DIRECT_HCI */

    /* See hci_cmd_read_iso_tx_sync — same fail-path contract. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_u8(buf, big_handle);

    *rsp = buf;

    return 0;
}

static int hci_cmd_setup_iso_data_path(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t conn_handle;
    tBTM_STATUS status;

    assert(rsp);

    conn_handle = sys_get_le16(buf->data + 3);

#if USE_DIRECT_HCI
    /* params are variable-length (13 + codec_cfg_len); use lib-supplied
     * preamble length byte rather than recomputing. */
    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_SET_DATA_PATH,
                                           buf->data + 3, buf->data[2],
                                           NULL, 0);
#else /* USE_DIRECT_HCI */
    uint8_t data_path_direction = buf->data[5];
    uint8_t data_path_id        = buf->data[6];
    uint8_t coding_format       = buf->data[7];
    uint16_t company_id         = sys_get_le16(buf->data + 8);
    uint16_t vs_codec_id        = sys_get_le16(buf->data + 10);
    uint32_t controller_delay   = sys_get_le24(buf->data + 12);
    uint8_t codec_cfg_len       = buf->data[15];
    uint8_t *codec_cfg          = buf->data + 16;

    /* TODO: this branch is dead today (USE_DIRECT_HCI is hardcoded to 1).
     * If re-enabled, BTM_BleIsoSetDataPath is fire-and-forget — its
     * return is the queueing status, NOT the controller Command_Complete.
     * Real status arrives later via BTM_BLE_ISO_DATA_PATH_UPFATE_EVT in
     * iso_evt_rx. Need a data_path static + k_sem_give in the cb + sem
     * wait here (mirror tx_sync/set_cig_params pattern) before treating
     * `status` as the final result. Same fix applies to
     * hci_cmd_remove_iso_data_path below. */
    status = BTM_BleIsoSetDataPath(conn_handle,
                                   data_path_direction,
                                   data_path_id,
                                   coding_format,
                                   company_id,
                                   vs_codec_id,
                                   controller_delay,
                                   codec_cfg_len,
                                   codec_cfg);
#endif /* USE_DIRECT_HCI */

    /* See hci_cmd_read_iso_tx_sync — same fail-path contract. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_le16(buf, conn_handle);

    *rsp = buf;

    return 0;
}

static int hci_cmd_remove_iso_data_path(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t conn_handle;
    tBTM_STATUS status;

    assert(rsp);

    conn_handle = sys_get_le16(buf->data + 3);

#if USE_DIRECT_HCI
    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_ISO_REMOVE_DATA_PATH,
                                           buf->data + 3, 3, NULL, 0);
#else /* USE_DIRECT_HCI */
    uint8_t data_path_direction = buf->data[5];
    status = BTM_BleIsoRemoveDataPath(conn_handle, data_path_direction);
#endif /* USE_DIRECT_HCI */

    /* See hci_cmd_read_iso_tx_sync — same fail-path contract. */
    if (status != BTM_SUCCESS) {
        net_buf_unref(buf);
        *rsp = NULL;
        return status;
    }

    net_buf_reset(buf);
    net_buf_add_u8(buf, status);
    net_buf_add_le16(buf, conn_handle);

    *rsp = buf;

    return 0;
}

int bt_le_bluedroid_hci_iso_cmd_send_sync(uint16_t opcode,
                                          struct net_buf *buf,
                                          struct net_buf **rsp)
{
    int rc;

    switch (opcode) {
    case BT_HCI_OP_LE_READ_ISO_TX_SYNC:
        rc = hci_cmd_read_iso_tx_sync(buf, rsp);
        break;

    case BT_HCI_OP_LE_SET_CIG_PARAMS:
        rc = hci_cmd_set_cig_params(buf, rsp);
        break;

    case BT_HCI_OP_LE_SET_CIG_PARAMS_TEST:
        rc = hci_cmd_set_cig_params_test(buf, rsp);
        break;

    case BT_HCI_OP_LE_CREATE_CIS:
        rc = hci_cmd_create_cis(buf, rsp);
        break;

    case BT_HCI_OP_LE_REMOVE_CIG:
        rc = hci_cmd_remove_cig(buf, rsp);
        break;

    case BT_HCI_OP_LE_ACCEPT_CIS:
        rc = hci_cmd_accept_cis_req(buf, rsp);
        break;

    case BT_HCI_OP_LE_REJECT_CIS:
        rc = hci_cmd_reject_cis_req(buf, rsp);
        break;

    case BT_HCI_OP_LE_CREATE_BIG:
        rc = hci_cmd_create_big(buf, rsp);
        break;

    case BT_HCI_OP_LE_CREATE_BIG_TEST:
        rc = hci_cmd_create_big_test(buf, rsp);
        break;

    case BT_HCI_OP_LE_TERMINATE_BIG:
        rc = hci_cmd_terminate_big(buf, rsp);
        break;

    case BT_HCI_OP_LE_BIG_CREATE_SYNC:
        rc = hci_cmd_big_create_sync(buf, rsp);
        break;

    case BT_HCI_OP_LE_BIG_TERMINATE_SYNC:
        rc = hci_cmd_big_terminate_sync(buf, rsp);
        break;

    case BT_HCI_OP_LE_SETUP_ISO_PATH:
        rc = hci_cmd_setup_iso_data_path(buf, rsp);
        break;

    case BT_HCI_OP_LE_REMOVE_ISO_PATH:
        rc = hci_cmd_remove_iso_data_path(buf, rsp);
        break;

    default:
        /* All other case branches consume buf via their helper (net_buf_unref
         * inside fire-and-forget helpers, or transparent reuse as response in
         * sync helpers). The default path is the only one that returns without
         * a helper, so it must unref buf itself — hci_cmd_pool is fixed at
         * size 1 (see host/common/hci.c), a single leak permanently exhausts
         * the pool and blocks all subsequent HCI commands. */
        LOG_ERR("[B]IsoCmdNotSupp[0x%04x]", opcode);
        net_buf_unref(buf);
        return -ENOTSUP;
    }

    return bluedroid_err_to_errno(rc);
}

static void iso_evt_handler(tBTM_BLE_ISO_EVENT event, tBTM_BLE_ISO_CB_PARAMS *params)
{
    uint8_t *qdata = NULL;
    size_t qdata_len = 0;
    int err;

    /* Note:
     * For NimBLE Host, the LE meta event structures contain the subevent_code,
     * but for Zephyr, the subevent_codes are not included.
     * Hence while allocating buffer for holding each event, one more octet
     * will be allocated for each LE meta event.
     */

    if (event != BTM_BLE_ISO_BIGINFO_ADV_REPORT_EVT) {
        LOG_DBG("[B]ISOEvtRx[%02x]", event);
    }

    switch (event) {
    case BTM_BLE_ISO_CIS_DISCONNECTED_EVT: {
        struct bt_hci_evt_disconn_complete ev = {0};

        qdata_len = 2 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.status = 0x00;
        ev.handle = params->btm_cis_disconnectd_evt.cis_handle;
        ev.reason = params->btm_cis_disconnectd_evt.reason;

        qdata[0] = false;   /* Not LE meta event */
        qdata[1] = BT_HCI_EVT_DISCONN_COMPLETE;
        memcpy(qdata + 2, &ev, sizeof(ev));
        break;
    }

    case BTM_BLE_ISO_CIS_ESTABLISHED_EVT: {
        struct bt_hci_evt_le_cis_established ev = {0};

        qdata_len = 2 + 1 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.status = params->btm_cis_established_evt.status;
        ev.conn_handle = params->btm_cis_established_evt.conn_handle;
        sys_put_le24(params->btm_cis_established_evt.cig_sync_delay, ev.cig_sync_delay);
        sys_put_le24(params->btm_cis_established_evt.cis_sync_delay, ev.cis_sync_delay);
        sys_put_le24(params->btm_cis_established_evt.trans_lat_c_to_p, ev.c_latency);
        sys_put_le24(params->btm_cis_established_evt.trans_lat_p_to_c, ev.p_latency);
        ev.c_phy = params->btm_cis_established_evt.phy_c_to_p;
        ev.p_phy = params->btm_cis_established_evt.phy_p_to_c;
        ev.nse = params->btm_cis_established_evt.nse;
        ev.c_bn = params->btm_cis_established_evt.bn_c_to_p;
        ev.p_bn = params->btm_cis_established_evt.bn_p_to_c;
        ev.c_ft = params->btm_cis_established_evt.ft_c_to_p;
        ev.p_ft = params->btm_cis_established_evt.ft_p_to_c;
        ev.c_max_pdu = params->btm_cis_established_evt.max_pdu_c_to_p;
        ev.p_max_pdu = params->btm_cis_established_evt.max_pdu_p_to_c;
        ev.interval = params->btm_cis_established_evt.iso_interval;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_CIS_ESTABLISHED;
        qdata[2] = BT_HCI_EVT_LE_CIS_ESTABLISHED;
        memcpy(qdata + 3, &ev, sizeof(ev));
        break;
    }

    case BTM_BLE_ISO_CIS_REQUEST_EVT: {
        struct bt_hci_evt_le_cis_req ev = {0};

        qdata_len = 2 + 1 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.acl_handle = params->btm_cis_request_evt.acl_handle;
        ev.cis_handle = params->btm_cis_request_evt.cis_handle;
        ev.cig_id = params->btm_cis_request_evt.cig_id;
        ev.cis_id = params->btm_cis_request_evt.cis_id;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_CIS_REQ;
        qdata[2] = BT_HCI_EVT_LE_CIS_REQ;
        memcpy(qdata + 3, &ev, sizeof(ev));
        break;
    }

    case BTM_BLE_ISO_BIG_CREATE_COMPLETE_EVT: {
        struct bt_hci_evt_le_big_complete ev = {0};

        qdata_len = 2 + 1 + sizeof(ev) + params->btm_big_cmpl.num_bis * 2;
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.status = params->btm_big_cmpl.status;
        ev.big_handle = params->btm_big_cmpl.big_handle;
        sys_put_le24(params->btm_big_cmpl.big_sync_delay, ev.sync_delay);
        sys_put_le24(params->btm_big_cmpl.transport_latency, ev.latency);
        ev.phy = params->btm_big_cmpl.phy;
        ev.nse = params->btm_big_cmpl.nse;
        ev.bn = params->btm_big_cmpl.bn;
        ev.pto = params->btm_big_cmpl.pto;
        ev.irc = params->btm_big_cmpl.irc;
        ev.max_pdu = params->btm_big_cmpl.max_pdu;
        ev.iso_interval = params->btm_big_cmpl.iso_interval;
        ev.num_bis = params->btm_big_cmpl.num_bis;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_BIG_COMPLETE;
        qdata[2] = BT_HCI_EVT_LE_BIG_COMPLETE;
        memcpy(qdata + 3, &ev, sizeof(ev));

        for (size_t i = 0; i < ev.num_bis; i++) {
            sys_put_le16(params->btm_big_cmpl.bis_handle[i], qdata + 3 + sizeof(ev) + i * 2);
        }
        break;
    }

    case BTM_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT: {
        struct bt_hci_evt_le_big_terminate ev = {0};

        qdata_len = 2 + 1 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.big_handle = params->btm_big_term.big_handle;
        ev.reason = params->btm_big_term.reason;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_BIG_TERMINATE;
        qdata[2] = BT_HCI_EVT_LE_BIG_TERMINATE;
        memcpy(qdata + 3, &ev, sizeof(ev));
        break;
    }

    case BTM_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT: {
        struct bt_hci_evt_le_big_sync_established ev = {0};

        qdata_len = 2 + 1 + sizeof(ev) + params->btm_big_sync_estab.num_bis * 2;
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.status = params->btm_big_sync_estab.status;
        ev.big_handle = params->btm_big_sync_estab.big_handle;
        sys_put_le24(params->btm_big_sync_estab.transport_latency_big, ev.latency);
        ev.nse = params->btm_big_sync_estab.nse;
        ev.bn = params->btm_big_sync_estab.bn;
        ev.pto = params->btm_big_sync_estab.pto;
        ev.irc = params->btm_big_sync_estab.irc;
        ev.max_pdu = params->btm_big_sync_estab.max_pdu;
        ev.iso_interval = params->btm_big_sync_estab.iso_interval;
        ev.num_bis = params->btm_big_sync_estab.num_bis;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_BIG_SYNC_ESTABLISHED;
        qdata[2] = BT_HCI_EVT_LE_BIG_SYNC_ESTABLISHED;
        memcpy(qdata + 3, &ev, sizeof(ev));

        for (size_t i = 0; i < ev.num_bis; i++) {
            sys_put_le16(params->btm_big_sync_estab.bis_handle[i], qdata + 3 + sizeof(ev) + i * 2);
        }
        break;
    }

    case BTM_BLE_ISO_BIG_SYNC_LOST_EVT: {
        struct bt_hci_evt_le_big_sync_lost ev = {0};

        qdata_len = 2 + 1 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.big_handle = params->btm_big_sync_lost.big_handle;
        ev.reason = params->btm_big_sync_lost.reason;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_BIG_SYNC_LOST;
        qdata[2] = BT_HCI_EVT_LE_BIG_SYNC_LOST;
        memcpy(qdata + 3, &ev, sizeof(ev));
        break;
    }

    case BTM_BLE_ISO_BIGINFO_ADV_REPORT_EVT: {
        struct bt_hci_evt_le_biginfo_adv_report ev = {0};

        qdata_len = 2 + 1 + sizeof(ev);
        qdata = calloc(1, qdata_len);
        assert(qdata);

        ev.sync_handle = params->btm_biginfo_report.sync_handle;
        ev.num_bis = params->btm_biginfo_report.num_bis;
        ev.nse = params->btm_biginfo_report.nse;
        ev.iso_interval = params->btm_biginfo_report.iso_interval;
        ev.bn = params->btm_biginfo_report.bn;
        ev.pto = params->btm_biginfo_report.pto;
        ev.irc = params->btm_biginfo_report.irc;
        ev.max_pdu = params->btm_biginfo_report.max_pdu;
        sys_put_le24(params->btm_biginfo_report.sdu_interval, ev.sdu_interval);
        ev.max_sdu = params->btm_biginfo_report.max_sdu;
        ev.phy = params->btm_biginfo_report.phy;
        ev.framing = params->btm_biginfo_report.framing;
        ev.encryption = params->btm_biginfo_report.encryption;

        qdata[0] = true;    /* LE meta event */
        qdata[1] = BT_HCI_EVT_LE_BIGINFO_ADV_REPORT;
        qdata[2] = BT_HCI_EVT_LE_BIGINFO_ADV_REPORT;
        memcpy(qdata + 3, &ev, sizeof(ev));
        break;
    }

    default:
        LOG_WRN("[B]IsoEvtUnexp[%u]", event);
        return;
    }

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT, qdata, qdata_len);
    if (err) {
        LOG_ERR("[B]IsoPostEvtFail[%d][%02x]", err, event);
        free(qdata);
    }
}

/* BTM callback — runs on the BTU task. MUST NOT acquire bt_le_host_lock:
 * sync-response callers (read_iso_tx_sync / set_cig_params*) hold host_lock
 * while blocked on iso_sem, which is given from this function — taking
 * host_lock here would deadlock. The same applies to iso_evt_handler below.
 * See file-top "Lock order" notes. */
static void iso_evt_rx(tBTM_BLE_ISO_EVENT event, tBTM_BLE_ISO_CB_PARAMS *params)
{
    if (event == BTM_BLE_ISO_CIS_DISCONNECTED_EVT ||
            event == BTM_BLE_ISO_CIS_ESTABLISHED_EVT ||
            event == BTM_BLE_ISO_CIS_REQUEST_EVT ||
            event == BTM_BLE_ISO_BIG_CREATE_COMPLETE_EVT ||
            event == BTM_BLE_ISO_BIG_TERMINATE_COMPLETE_EVT ||
            event == BTM_BLE_ISO_BIG_SYNC_ESTABLISHED_EVT ||
            event == BTM_BLE_ISO_BIG_SYNC_LOST_EVT ||
            event == BTM_BLE_ISO_BIGINFO_ADV_REPORT_EVT) {
        iso_evt_handler(event, params);
        return;
    }

    switch (event) {
    case BTM_BLE_ISO_SET_CIG_PARAMS_EVT: {
        uint8_t cis_count = params->btm_set_cig_params.cis_count;

        LOG_DBG("[B]SetCigParamsEvt[%u][%u][%u][%u]",
                params->status, params->btm_set_cig_params.status,
                params->btm_set_cig_params.cig_id, cis_count);
        for (size_t i = 0; i < cis_count; i++) {
            LOG_DBG("[B]CisHdl[%u][0x%03x]", i, params->btm_set_cig_params.conn_hdl[i]);
        }

        /* BTM's source array is sized BLE_ISO_CIS_MAX_COUNT, which can
         * exceed CONFIG_BT_ISO_MAX_CHAN per the Static_assert. Runtime
         * clamp so the copy below doesn't overrun set_cig_params.cis_handle;
         * assert alone is unsafe (NDEBUG no-ops it). Mirrors DIRECT_HCI
         * path clamp in hci_cmd_set_cig_params. */
        if (cis_count > CONFIG_BT_ISO_MAX_CHAN) {
            LOG_ERR("[B]CigParamsCisCountOverflow[%u>%d]",
                    cis_count, CONFIG_BT_ISO_MAX_CHAN);
            cis_count = CONFIG_BT_ISO_MAX_CHAN;
        }

        set_cig_params.status = params->btm_set_cig_params.status;
        set_cig_params.cig_id = params->btm_set_cig_params.cig_id;
        set_cig_params.cis_count = cis_count;
        for (size_t i = 0; i < cis_count; i++) {
            set_cig_params.cis_handle[i] = params->btm_set_cig_params.conn_hdl[i];
        }

        k_sem_give(&iso_sem);
        break;
    }

    case BTM_BLE_ISO_REMOVE_CIG_EVT:
        LOG_DBG("[B]RemoveCigEvt[%u][%u][%u]",
                params->status, params->btm_remove_cig.status,
                params->btm_remove_cig.cig_id);
        break;

    case BTM_BLE_ISO_CREATE_CIS_EVT:
        LOG_DBG("[B]CreateCisEvt[%u]", params->status);
        break;

    case BTM_BLE_ISO_ACCEPT_CIS_REQ_EVT:
        LOG_DBG("[B]AcceptCisReqEvt[%u]", params->status);
        break;

    case BTM_BLE_ISO_REJECT_CIS_REQ_EVT:
        LOG_DBG("[B]RejectCisReqEvt[%u][%u][0x%03x]",
                params->status, params->btm_reject_cis_req.status,
                params->btm_reject_cis_req.cis_handle);
        break;

    case BTM_BLE_ISO_BIG_SYNC_TERMINATE_COMPLETE_EVT:
        LOG_DBG("[B]BigSyncTermCmpl[%u][%u][%u]",
                params->status, params->btm_big_sync_ter.status,
                params->btm_big_sync_ter.big_hdl);
        break;

    case BTM_BLE_ISO_DATA_PATH_UPFATE_EVT:
        LOG_DBG("[B]DataPathUpdate[%u][%u][%s][0x%03x]",
                params->status, params->btm_data_path_update.status,
                (params->btm_data_path_update.op_type == BTM_BLE_ISO_DATA_PATH_UNKNOWN ? "unknown" :
                 (params->btm_data_path_update.op_type == BTM_BLE_ISO_DATA_PATH_SETUP ? "setup" : "remove")),
                params->btm_data_path_update.conn_hdl);
        break;

    case BTM_BLE_ISO_READ_TX_SYNC_EVT:
        LOG_DBG("[B]RdTxSyncEvt[%u][%u][0x%03x]",
                params->status, params->btm_read_tx_sync.status,
                params->btm_read_tx_sync.conn_hdl);
        LOG_DBG("[B]RdTxSyncData[%u][%u][%u]",
                params->btm_read_tx_sync.pkt_seq_num,
                params->btm_read_tx_sync.tx_time_stamp,
                params->btm_read_tx_sync.time_offset);

        tx_sync.status = params->btm_read_tx_sync.status;
        tx_sync.conn_handle = params->btm_read_tx_sync.conn_hdl;
        tx_sync.packet_seq_num = params->btm_read_tx_sync.pkt_seq_num;
        tx_sync.tx_time_stamp = params->btm_read_tx_sync.tx_time_stamp;
        tx_sync.time_offset = params->btm_read_tx_sync.time_offset;

        k_sem_give(&iso_sem);
        break;

    case BTM_BLE_ISO_READ_LINK_QUALITY_EVT:
        LOG_DBG("[B]RdLinkQuality[%u][%u][0x%04x]",
                params->status, params->btm_read_link_quality.status,
                params->btm_read_link_quality.conn_hdl);
        LOG_DBG("[B]RdLqTx[%u][%u][%u][%u]",
                params->btm_read_link_quality.tx_unacked_pkts,
                params->btm_read_link_quality.tx_flushed_pkts,
                params->btm_read_link_quality.tx_last_subevt_pkts,
                params->btm_read_link_quality.retransmitted_pkts);
        LOG_DBG("[B]RdLqRx[%u][%u][%u]",
                params->btm_read_link_quality.crc_error_pkts,
                params->btm_read_link_quality.rx_unreceived_pkts,
                params->btm_read_link_quality.duplicate_pkts);
        break;

    default:
        LOG_WRN("[B]IsoEvtRxUnexp[%u]", event);
        break;
    }
}

#if CONFIG_BT_ISO_RX
static void iso_pkt_rx(uint8_t *data, uint16_t len)
{
    bt_le_iso_rx(data, len, NULL);
}
#endif /* CONFIG_BT_ISO_RX */

#if CONFIG_BT_ISO_UNICAST
static int iso_enable_cis(void)
{
    tBTM_STATUS status;

#if USE_DIRECT_HCI
    /* HCI LE Set Host Feature: Bit_Number(1) | Bit_Value(1).
     * bit 32 = LE ISO Channels (Host Support). */
    uint8_t cmd_params[2] = { 32, 1 };

    status = bt_le_bluedroid_hci_send_sync(HCI_BLE_SET_HOST_FEATURE,
                                           cmd_params, sizeof(cmd_params),
                                           NULL, 0);
#else /* USE_DIRECT_HCI */
    status = BTM_BleSetHostFeature(32, 1);
#endif /* USE_DIRECT_HCI */

    return bluedroid_err_to_errno(status);
}
#endif /* CONFIG_BT_ISO_UNICAST */

int bt_le_bluedroid_iso_init(void)
{
#if CONFIG_BT_ISO_UNICAST
    int err;
#endif /* CONFIG_BT_ISO_UNICAST */

    k_sem_create(&iso_sem);

#if USE_DIRECT_HCI
    bt_le_bluedroid_hci_init();
#endif /* USE_DIRECT_HCI */

    BTM_BleIsoRegisterCallback(iso_evt_rx);

#if CONFIG_BT_ISO_RX
    extern void ble_host_register_rx_iso_data_cb(void *cb);
    ble_host_register_rx_iso_data_cb(iso_pkt_rx);
#endif /* CONFIG_BT_ISO_RX */

#if CONFIG_BT_ISO_UNICAST
    /* Set Connected Isochronous Streams - Host support */
    err = iso_enable_cis();
    if (err) {
        /* Roll back local init so a retry doesn't trip k_sem_create's
         * assert(handle == NULL). Reverse order of init, skipping the
         * disable_cis step since enable never took effect. */
#if CONFIG_BT_ISO_RX
        ble_host_register_rx_iso_data_cb(NULL);
#endif /* CONFIG_BT_ISO_RX */
        /* iso_evt_rx stays installed: BTM_BleIsoRegisterCallback rejects NULL
         * (see deinit) and a retry just re-installs it (no EALREADY guard). */
#if USE_DIRECT_HCI
        bt_le_bluedroid_hci_deinit();
#endif /* USE_DIRECT_HCI */
        k_sem_delete(&iso_sem);
        return err;
    }
#endif /* CONFIG_BT_ISO_UNICAST */

    return 0;
}

void bt_le_bluedroid_iso_deinit(void)
{
#if CONFIG_BT_ISO_UNICAST
    /* Mirror bt_le_iso_init() which enables bit 32 only on unicast build. */
#if USE_DIRECT_HCI
    {
        uint8_t cmd_params[2] = { 32, 0 };

        bt_le_bluedroid_hci_send_sync(HCI_BLE_SET_HOST_FEATURE,
                                      cmd_params, sizeof(cmd_params),
                                      NULL, 0);
    }
#else /* USE_DIRECT_HCI */
    BTM_BleSetHostFeature(32, 0);
#endif /* USE_DIRECT_HCI */
#endif /* CONFIG_BT_ISO_UNICAST */

#if CONFIG_BT_ISO_RX
    extern void ble_host_register_rx_iso_data_cb(void *cb);
    ble_host_register_rx_iso_data_cb(NULL);
#endif /* CONFIG_BT_ISO_RX */

    /* BTM_BleIsoRegisterCallback rejects NULL — iso_evt_rx stays installed
     * for the Bluedroid stack lifetime (same upstream constraint applies
     * to gap_app_cb installed via BTM_BleGapRegisterCallback, which is
     * why gap.c provides no deinit counterpart). */

#if USE_DIRECT_HCI
    bt_le_bluedroid_hci_deinit();
#endif /* USE_DIRECT_HCI */

    k_sem_delete(&iso_sem);
}
