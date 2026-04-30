/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <zephyr/logging/log.h>

#include <../host/conn_internal.h>
#include <../host/iso_internal.h>

#include "../src/ble_hs_priv.h"
#include "host/ble_hs_iso.h"
#include "host/ble_hs_iso_hci.h"

#include "common/host.h"

#include "nimble/hs_error.h"

LOG_MODULE_REGISTER(ISO_NISO, CONFIG_BT_ISO_LOG_LEVEL);

extern int ble_hs_iso_evt_rx_cb_set(void *cb);
#if CONFIG_BT_ISO_RX
extern int ble_hs_iso_pkt_rx_cb_set(ble_hs_iso_pkt_rx_fn cb);
#endif /* CONFIG_BT_ISO_RX */

static int hci_cmd_read_iso_tx_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t packet_seq_num = 0;
    uint32_t tx_time_stamp = 0;
    uint32_t time_offset = 0;
    uint16_t conn_handle;
    int status;

    assert(rsp);

    assert(buf->len >= 5);

    conn_handle = sys_get_le16(buf->data + 3);

    ble_hs_lock();

    status = ble_hs_hci_read_iso_tx_sync(conn_handle,
                                         &packet_seq_num,
                                         &tx_time_stamp,
                                         &time_offset);
    if (status) {
        LOG_ERR("[N]RdIsoTxSyncFail[%u][%02x]", conn_handle, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_le16(buf, conn_handle);
        net_buf_add_le16(buf, packet_seq_num);
        net_buf_add_le32(buf, tx_time_stamp);
        net_buf_add_le24(buf, time_offset);
        *rsp = buf;
    }

    return status;
}

static int hci_cmd_set_cig_params(struct net_buf *buf, struct net_buf **rsp)
{
    struct ble_hci_le_cis_params *cis_params = NULL;
    uint32_t sdu_interval_c_to_p;
    uint32_t sdu_interval_p_to_c;
    uint8_t worst_case_sca;
    uint16_t mtl_c_to_p;
    uint16_t mtl_p_to_c;
    uint8_t cis_count;
    uint8_t packing;
    uint8_t framing;
    uint8_t cig_id;
    uint8_t rp_len;
    uint8_t *rp;
    int status;

    assert(rsp);

    assert(buf->len >= 18);

    cig_id              = buf->data[3];
    sdu_interval_c_to_p = sys_get_le24(buf->data + 4);
    sdu_interval_p_to_c = sys_get_le24(buf->data + 7);
    worst_case_sca      = buf->data[10];
    packing             = buf->data[11];
    framing             = buf->data[12];
    mtl_c_to_p          = sys_get_le16(buf->data + 13);
    mtl_p_to_c          = sys_get_le16(buf->data + 15);
    cis_count           = buf->data[17];

    assert(buf->len >= 18 + cis_count * sizeof(struct ble_hci_le_cis_params));

    /* The cis_count can be 0 */
    if (cis_count) {
        cis_params = calloc(1, cis_count * sizeof(struct ble_hci_le_cis_params));
        assert(cis_params);
    }

    rp_len = sizeof(struct ble_hci_le_set_cig_params_rp) + cis_count * 2;
    rp = calloc(1, rp_len);
    assert(rp);

    for (size_t i = 0; i < cis_count; i++) {
        cis_params[i].cis_id         = buf->data[18 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].max_sdu_c_to_p = sys_get_le16(buf->data + 19 + i * sizeof(struct ble_hci_le_cis_params));
        cis_params[i].max_sdu_p_to_c = sys_get_le16(buf->data + 21 + i * sizeof(struct ble_hci_le_cis_params));
        cis_params[i].phy_c_to_p     = buf->data[23 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].phy_p_to_c     = buf->data[24 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].rtn_c_to_p     = buf->data[25 + i * sizeof(struct ble_hci_le_cis_params)];
        cis_params[i].rtn_p_to_c     = buf->data[26 + i * sizeof(struct ble_hci_le_cis_params)];
    }

    ble_hs_lock();

    status = ble_hs_hci_set_cig_params(cig_id,
                                       sdu_interval_c_to_p,
                                       sdu_interval_p_to_c,
                                       worst_case_sca,
                                       packing,
                                       framing,
                                       mtl_c_to_p,
                                       mtl_p_to_c,
                                       cis_count,
                                       cis_params,
                                       rp,
                                       rp_len);
    if (status) {
        LOG_ERR("[N]SetCigParamsFail[%u][%02x]", cig_id, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_u8(buf, rp[0]);
        net_buf_add_u8(buf, rp[1]);
        for (size_t i = 0; i < cis_count; i++) {
            net_buf_add_le16(buf, sys_get_le16(rp + sizeof(struct ble_hci_le_set_cig_params_rp) + i * 2));
        }
        *rsp = buf;
    }

    free(cis_params);
    free(rp);

    return status;
}

static int hci_cmd_set_cig_params_test(struct net_buf *buf, struct net_buf **rsp)
{
    struct ble_hci_le_cis_params_test *cis_params = NULL;
    uint32_t sdu_interval_c_to_p;
    uint32_t sdu_interval_p_to_c;
    uint8_t worst_case_sca;
    uint16_t iso_interval;
    uint8_t ft_c_to_p;
    uint8_t ft_p_to_c;
    uint8_t cis_count;
    uint8_t packing;
    uint8_t framing;
    uint8_t cig_id;
    uint8_t rp_len;
    uint8_t *rp;
    int status;

    assert(rsp);

    assert(buf->len >= 18);

    cig_id              = buf->data[3];
    sdu_interval_c_to_p = sys_get_le24(buf->data + 4);
    sdu_interval_p_to_c = sys_get_le24(buf->data + 7);
    ft_c_to_p           = buf->data[10];
    ft_p_to_c           = buf->data[11];
    iso_interval        = sys_get_le16(buf->data + 12);
    worst_case_sca      = buf->data[14];
    packing             = buf->data[15];
    framing             = buf->data[16];
    cis_count           = buf->data[17];

    assert(buf->len >= 18 + cis_count * sizeof(struct ble_hci_le_cis_params_test));

    /* The cis_count can be 0 */
    if (cis_count) {
        cis_params = calloc(1, cis_count * sizeof(struct ble_hci_le_cis_params_test));
        assert(cis_params);
    }

    rp_len = sizeof(struct ble_hci_le_set_cig_params_test_rp) + cis_count * 2;
    rp = calloc(1, rp_len);
    assert(rp);

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

    ble_hs_lock();

    status = ble_hs_hci_set_cig_params_test(cig_id,
                                            sdu_interval_c_to_p,
                                            sdu_interval_p_to_c,
                                            ft_c_to_p,
                                            ft_p_to_c,
                                            iso_interval,
                                            worst_case_sca,
                                            packing,
                                            framing,
                                            cis_count,
                                            cis_params,
                                            rp,
                                            rp_len);
    if (status) {
        LOG_ERR("[N]SetCigParamsTestFail[%u][%02x]", cig_id, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_u8(buf, rp[0]);
        net_buf_add_u8(buf, rp[1]);
        for (size_t i = 0; i < cis_count; i++) {
            net_buf_add_le16(buf, sys_get_le16(rp + sizeof(struct ble_hci_le_set_cig_params_test_rp) + i * 2));
        }
        *rsp = buf;
    }

    free(cis_params);
    free(rp);

    return status;
}

static int hci_cmd_create_cis(struct net_buf *buf, struct net_buf **rsp)
{
    struct ble_hci_le_create_cis_params *cis_params;
    uint8_t cis_count;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 4);

    /* The cis_count shall be at least 1 */
    cis_count = buf->data[3];

    assert(buf->len >= 4 + cis_count * sizeof(struct ble_hci_le_create_cis_params));

    cis_params = calloc(1, cis_count * sizeof(struct ble_hci_le_create_cis_params));
    assert(cis_params);

    for (size_t i = 0; i < cis_count; i++) {
        cis_params[i].cis_handle  = sys_get_le16(buf->data + 4 + i * sizeof(struct ble_hci_le_create_cis_params));
        cis_params[i].conn_handle = sys_get_le16(buf->data + 6 + i * sizeof(struct ble_hci_le_create_cis_params));
    }

    ble_hs_lock();

    status = ble_hs_hci_create_cis(cis_count, cis_params);
    if (status) {
        LOG_ERR("[N]CreateCisFail[%u][%02x]", cis_count, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);
    free(cis_params);

    return status;
}

static int hci_cmd_remove_cig(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t cig_id;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 4);

    cig_id = buf->data[3];

    ble_hs_lock();

    status = ble_hs_hci_remove_cig(cig_id);
    if (status) {
        LOG_ERR("[N]RemoveCigFail[%u][%02x]", cig_id, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_accept_cis_req(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t cis_handle;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 5);

    cis_handle = sys_get_le16(buf->data + 3);

    ble_hs_lock();

    status = ble_hs_hci_accept_cis_request(cis_handle);
    if (status) {
        LOG_ERR("[N]AcceptCisReqFail[%u][%02x]", cis_handle, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_reject_cis_req(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t cis_handle;
    uint8_t reason;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 6);

    cis_handle = sys_get_le16(buf->data + 3);
    reason = buf->data[5];

    ble_hs_lock();

    status = ble_hs_hci_reject_cis_request(cis_handle, reason);
    if (status) {
        LOG_ERR("[N]RejectCisReqFail[%u][%02x]", cis_handle, status);
    }

    ble_hs_unlock();

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
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 34);

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

    ble_hs_lock();

    status = ble_hs_hci_create_big(big_handle,
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
    if (status) {
        LOG_ERR("[N]CreateBigFail[%u][%02x]", big_handle, status);
    }

    ble_hs_unlock();

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
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 39);

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

    ble_hs_lock();

    status = ble_hs_hci_create_big_test(big_handle,
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
    if (status) {
        LOG_ERR("[N]CreateBigTestFail[%u][%02x]", big_handle, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_terminate_big(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    uint8_t reason;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 5);

    big_handle = buf->data[3];
    reason     = buf->data[4];

    ble_hs_lock();

    status = ble_hs_hci_terminate_big(big_handle, reason);
    if (status) {
        LOG_ERR("[N]TerminateBigFail[%u][%02x]", big_handle, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_big_create_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint16_t sync_timeout;
    uint16_t sync_handle;
    uint8_t big_handle;
    uint8_t encryption;
    uint8_t *bst_code;
    uint8_t num_bis;
    uint8_t *bis;
    uint8_t mse;
    int status;

    ARG_UNUSED(rsp);

    assert(buf->len >= 27);

    big_handle   = buf->data[3];
    sync_handle  = sys_get_le16(buf->data + 4);
    encryption   = buf->data[6];
    bst_code     = buf->data + 7;
    mse          = buf->data[23];
    sync_timeout = sys_get_le16(buf->data + 24);
    num_bis      = buf->data[26];
    bis          = buf->data + 27;

    assert(buf->len >= 27 + num_bis);

    ble_hs_lock();

    status = ble_hs_hci_big_create_sync(big_handle,
                                        sync_handle,
                                        encryption,
                                        bst_code,
                                        mse,
                                        sync_timeout,
                                        num_bis,
                                        bis);
    if (status) {
        LOG_ERR("[N]BigCreateSyncFail[%u][%02x]", big_handle, status);
    }

    ble_hs_unlock();

    net_buf_unref(buf);

    return status;
}

static int hci_cmd_big_terminate_sync(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t big_handle;
    int status;

    assert(rsp);

    assert(buf->len >= 4);

    big_handle = buf->data[3];

    ble_hs_lock();

    status = ble_hs_hci_big_terminate_sync(big_handle);
    if (status) {
        LOG_ERR("[N]BigTerminateSyncFail[%u][%02x]", big_handle, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_u8(buf, big_handle);
        *rsp = buf;
    }

    return status;
}

static int hci_cmd_setup_iso_data_path(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t data_path_direction;
    uint32_t controller_delay;
    uint16_t conn_handle;
    uint8_t data_path_id;
    uint8_t coding_format;
    uint16_t company_id;
    uint16_t vs_codec_id;
    uint8_t codec_cfg_len;
    uint8_t *codec_cfg;
    int status;

    assert(rsp);

    assert(buf->len >= 16);

    conn_handle         = sys_get_le16(buf->data + 3);
    data_path_direction = buf->data[5];
    data_path_id        = buf->data[6];
    coding_format       = buf->data[7];
    company_id          = sys_get_le16(buf->data + 8);
    vs_codec_id         = sys_get_le16(buf->data + 10);
    controller_delay    = sys_get_le24(buf->data + 12);
    codec_cfg_len       = buf->data[15];
    codec_cfg           = buf->data + 16;

    assert(buf->len >= 16 + codec_cfg_len);

    ble_hs_lock();

    status = ble_hs_hci_setup_iso_data_path(conn_handle,
                                            data_path_direction,
                                            data_path_id,
                                            coding_format,
                                            company_id,
                                            vs_codec_id,
                                            controller_delay,
                                            codec_cfg_len,
                                            codec_cfg);
    if (status) {
        LOG_ERR("[N]SetupIsoDataPathFail[%u][%02x]", conn_handle, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_le16(buf, conn_handle);
        *rsp = buf;
    }

    return status;
}

static int hci_cmd_remove_iso_data_path(struct net_buf *buf, struct net_buf **rsp)
{
    uint8_t data_path_direction;
    uint16_t conn_handle;
    int status;

    assert(rsp);

    assert(buf->len >= 6);

    conn_handle         = sys_get_le16(buf->data + 3);
    data_path_direction = buf->data[5];

    ble_hs_lock();

    status = ble_hs_hci_remove_iso_data_path(conn_handle, data_path_direction);
    if (status) {
        LOG_ERR("[N]RemoveIsoDataPathFail[%u][%02x]", conn_handle, status);
    }

    ble_hs_unlock();

    if (status) {
        net_buf_unref(buf);
        *rsp = NULL;
    } else {
        net_buf_reset(buf);
        net_buf_add_u8(buf, status);
        net_buf_add_le16(buf, conn_handle);
        *rsp = buf;
    }

    return status;
}

int bt_le_nimble_iso_cmd_send_sync(uint16_t opcode,
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
        LOG_ERR("[N]IsoCmdNotSupp[%04x]", opcode);
        net_buf_unref(buf);
        return -ENOTSUP;
    }

    return nimble_err_to_errno(rc);
}

static void iso_evt_rx(uint8_t event, const void *data,
                       unsigned int len, bool le_meta)
{
    size_t qdata_len;
    uint8_t *qdata;
    int err;

    if (event != BT_HCI_EVT_LE_BIGINFO_ADV_REPORT) {
        LOG_DBG("[N]ISOEvtRx[%02x][%u][%u]", event, len, le_meta);
    }

    qdata_len = len + 2;

    qdata = calloc(1, qdata_len);
    assert(qdata);

    qdata[0] = le_meta;
    qdata[1] = event;
    memcpy(qdata + 2, data, len);

    err = bt_le_iso_task_post(ISO_QUEUE_ITEM_TYPE_ISO_HCI_EVENT, qdata, qdata_len);
    if (err) {
        LOG_ERR("[N]IsoPostEvtFail[%d][%02x]", err, event);
        free(qdata);
    }
}

#if CONFIG_BT_ISO_UNICAST
static int iso_enable_cis(void)
{
    int err;

    ble_hs_lock();

    err = ble_hs_hci_set_host_feature(32, 1);
    if (err) {
        LOG_ERR("[N]IsoEnableCisFail[%d]", err);
    }

    ble_hs_unlock();

    return err;
}

static int iso_disable_cis(void)
{
    int err;

    ble_hs_lock();

    err = ble_hs_hci_set_host_feature(32, 0);
    if (err) {
        LOG_ERR("[N]IsoDisableCisFail[%d]", err);
    }

    ble_hs_unlock();

    return err;
}
#endif /* CONFIG_BT_ISO_UNICAST */

int bt_le_nimble_iso_init(void)
{
    int err;

    err = ble_hs_iso_evt_rx_cb_set(iso_evt_rx);
    if (err) {
        LOG_ERR("[N]IsoEvtRxCbSetFail[%d]", err);
        return err;
    }

#if CONFIG_BT_ISO_RX
    err = ble_hs_iso_pkt_rx_cb_set(bt_le_iso_rx);
    if (err) {
        LOG_ERR("[N]IsoPktRxCbSetFail[%d]", err);
        return err;
    }
#endif /* CONFIG_BT_ISO_RX */

#if CONFIG_BT_ISO_UNICAST
    /* Set Connected Isochronous Streams - Host support */
    err = iso_enable_cis();
    if (err) {
        return err;
    }
#endif /* CONFIG_BT_ISO_UNICAST */

    return 0;
}

void bt_le_nimble_iso_deinit(void)
{
    LOG_DBG("IsoDeinit");

    ble_hs_iso_evt_rx_cb_set(NULL);

#if CONFIG_BT_ISO_RX
    ble_hs_iso_pkt_rx_cb_set(NULL);
#endif /* CONFIG_BT_ISO_RX */

#if CONFIG_BT_ISO_UNICAST
    iso_disable_cis();
#endif /* CONFIG_BT_ISO_UNICAST */
}
