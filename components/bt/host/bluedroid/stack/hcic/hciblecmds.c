/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains function of the HCIC unit to format and send HCI
 *  commands.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include "osi/allocator.h"
#include "stack/hcidefs.h"
#include "stack/hcimsgs.h"
#include "stack/hcidefs.h"
#include "stack/btu.h"

#include <stddef.h>
#include <string.h>

#if (BLE_50_FEATURE_SUPPORT == TRUE)
static BlE_SYNC ble_sync_info;

void btsnd_hcic_ble_sync_sem_init(void)
{
    ble_sync_info.opcode = 0;
    osi_sem_new(&ble_sync_info.sync_sem, 1, 0);
}

void btsnd_hcic_ble_sync_sem_deinit(void)
{
    ble_sync_info.opcode = 0;
    osi_sem_free(&ble_sync_info.sync_sem);
}

BlE_SYNC *btsnd_hcic_ble_get_sync_info(void)
{
    return &ble_sync_info;
}

uint8_t btsnd_hcic_ble_get_status(void)
{
    return ble_sync_info.status;
}

void btsnd_hci_ble_set_status(UINT8 hci_status)
{
    ble_sync_info.status = hci_status;
    return;
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE)

BOOLEAN btsnd_hcic_ble_set_local_used_feat (UINT8 feat_set[8])
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_SET_USED_FEAT_CMD)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_SET_USED_FEAT_CMD;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_LOCAL_SPT_FEAT);
    ARRAY_TO_STREAM (pp, feat_set, HCIC_PARAM_SIZE_SET_USED_FEAT_CMD);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_random_addr (BD_ADDR random_bda)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_WRITE_RANDOM_ADDR_CMD)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_RANDOM_ADDR_CMD;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_RANDOM_ADDR);
    UINT8_TO_STREAM  (pp,  HCIC_PARAM_SIZE_WRITE_RANDOM_ADDR_CMD);

    BDADDR_TO_STREAM (pp, random_bda);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_write_adv_params (UINT16 adv_int_min, UINT16 adv_int_max,
        UINT8 adv_type, UINT8 addr_type_own,
        UINT8 addr_type_dir, BD_ADDR direct_bda,
        UINT8 channel_map, UINT8 adv_filter_policy)
{
    BT_HDR *p;
    UINT8 *pp;
    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS ;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_ADV_PARAMS);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS );

    UINT16_TO_STREAM (pp, adv_int_min);
    UINT16_TO_STREAM (pp, adv_int_max);
    UINT8_TO_STREAM (pp, adv_type);
    UINT8_TO_STREAM (pp, addr_type_own);
    UINT8_TO_STREAM (pp, addr_type_dir);
    BDADDR_TO_STREAM (pp, direct_bda);
    UINT8_TO_STREAM (pp, channel_map);
    UINT8_TO_STREAM (pp, adv_filter_policy);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}
BOOLEAN btsnd_hcic_ble_read_adv_chnl_tx_power (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_READ_CMD)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_READ_CMD;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_READ_ADV_CHNL_TX_POWER);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_READ_CMD);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);

}

BOOLEAN btsnd_hcic_ble_set_adv_data (UINT8 data_len, UINT8 *p_data)
{
    BT_HDR *p;
    UINT8 *pp;

    for (int i = 0; i < data_len; i++) {
        HCI_TRACE_DEBUG("p_data[%d] = %x\n", i, p_data[i]);
    }

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA + 1)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA + 1;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_ADV_DATA);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA + 1);

    memset(pp, 0, HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA);

    if (p_data != NULL && data_len > 0) {
        if (data_len > HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA) {
            data_len = HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA;
            HCI_TRACE_WARNING("Data length exceeds 31 bytes, only the first 31 bytes are used.\n");
        }

        UINT8_TO_STREAM (pp, data_len);

        ARRAY_TO_STREAM (pp, p_data, data_len);
    }
    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);

    return (TRUE);
}
BOOLEAN btsnd_hcic_ble_set_scan_rsp_data (UINT8 data_len, UINT8 *p_scan_rsp)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP + 1)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP + 1;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_SCAN_RSP_DATA);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP + 1);

    memset(pp, 0, HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP);

    if (p_scan_rsp != NULL && data_len > 0) {

        if (data_len > HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP ) {
            data_len = HCIC_PARAM_SIZE_BLE_WRITE_SCAN_RSP;
            HCI_TRACE_WARNING("Data length exceeds 31 bytes, only the first 31 bytes are used.\n");
        }

        UINT8_TO_STREAM (pp, data_len);

        ARRAY_TO_STREAM (pp, p_scan_rsp, data_len);
    }

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_adv_enable (UINT8 adv_enable)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_WRITE_ADV_ENABLE)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_ADV_ENABLE;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_ADV_ENABLE);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_WRITE_ADV_ENABLE);

    UINT8_TO_STREAM (pp, adv_enable);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}
BOOLEAN btsnd_hcic_ble_set_scan_params (UINT8 scan_type,
                                        UINT16 scan_int, UINT16 scan_win,
                                        UINT8 addr_type_own, UINT8 scan_filter_policy)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_WRITE_SCAN_PARAM)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_SCAN_PARAM;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_SCAN_PARAMS);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_WRITE_SCAN_PARAM);

    UINT8_TO_STREAM (pp, scan_type);
    UINT16_TO_STREAM (pp, scan_int);
    UINT16_TO_STREAM (pp, scan_win);
    UINT8_TO_STREAM (pp, addr_type_own);
    UINT8_TO_STREAM (pp, scan_filter_policy);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_scan_enable (UINT8 scan_enable, UINT8 duplicate)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_WRITE_SCAN_ENABLE)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_WRITE_SCAN_ENABLE;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_WRITE_SCAN_ENABLE);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_WRITE_SCAN_ENABLE);

    UINT8_TO_STREAM (pp, scan_enable);
    UINT8_TO_STREAM (pp, duplicate);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

/* link layer connection management commands */
BOOLEAN btsnd_hcic_ble_create_ll_conn (UINT16 scan_int, UINT16 scan_win,
                                       UINT8 init_filter_policy,
                                       UINT8 addr_type_peer, BD_ADDR bda_peer,
                                       UINT8 addr_type_own,
                                       UINT16 conn_int_min, UINT16 conn_int_max,
                                       UINT16 conn_latency, UINT16 conn_timeout,
                                       UINT16 min_ce_len, UINT16 max_ce_len)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_CREATE_LL_CONN)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_CREATE_LL_CONN;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_CREATE_LL_CONN);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_CREATE_LL_CONN);

    UINT16_TO_STREAM (pp, scan_int);
    UINT16_TO_STREAM (pp, scan_win);
    UINT8_TO_STREAM (pp, init_filter_policy);

    UINT8_TO_STREAM (pp, addr_type_peer);
    BDADDR_TO_STREAM (pp, bda_peer);
    UINT8_TO_STREAM (pp, addr_type_own);

    UINT16_TO_STREAM (pp, conn_int_min);
    UINT16_TO_STREAM (pp, conn_int_max);
    UINT16_TO_STREAM (pp, conn_latency);
    UINT16_TO_STREAM (pp, conn_timeout);

    UINT16_TO_STREAM (pp, min_ce_len ? min_ce_len : BLE_CE_LEN_MIN);
    UINT16_TO_STREAM (pp, max_ce_len ? max_ce_len : BLE_CE_LEN_MIN);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_create_conn_cancel (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_CREATE_CONN_CANCEL)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_CREATE_CONN_CANCEL;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_CREATE_CONN_CANCEL);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_CREATE_CONN_CANCEL);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_clear_white_list (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_CLEAR_WHITE_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_CLEAR_WHITE_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_CLEAR_WHITE_LIST);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_CLEAR_WHITE_LIST);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_add_white_list (UINT8 addr_type, BD_ADDR bda)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_ADD_WHITE_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_ADD_WHITE_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_ADD_WHITE_LIST);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_ADD_WHITE_LIST);

    UINT8_TO_STREAM (pp, addr_type);
    BDADDR_TO_STREAM (pp, bda);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_remove_from_white_list (UINT8 addr_type, BD_ADDR bda)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_REMOVE_WHITE_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_REMOVE_WHITE_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_REMOVE_WHITE_LIST);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_REMOVE_WHITE_LIST);

    UINT8_TO_STREAM (pp, addr_type);
    BDADDR_TO_STREAM (pp, bda);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_upd_ll_conn_params (UINT16 handle,
        UINT16 conn_int_min, UINT16 conn_int_max,
        UINT16 conn_latency, UINT16 conn_timeout,
        UINT16 min_ce_len, UINT16 max_ce_len)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_UPD_LL_CONN_PARAMS)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_UPD_LL_CONN_PARAMS;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_UPD_LL_CONN_PARAMS);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_UPD_LL_CONN_PARAMS);

    UINT16_TO_STREAM (pp, handle);

    UINT16_TO_STREAM (pp, conn_int_min);
    UINT16_TO_STREAM (pp, conn_int_max);
    UINT16_TO_STREAM (pp, conn_latency);
    UINT16_TO_STREAM (pp, conn_timeout);
    UINT16_TO_STREAM (pp, min_ce_len);
    UINT16_TO_STREAM (pp, max_ce_len);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_host_chnl_class (UINT8  chnl_map[HCIC_BLE_CHNL_MAP_SIZE])
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_SET_HOST_CHNL_CLASS)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_SET_HOST_CHNL_CLASS;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_SET_HOST_CHNL_CLASS);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_SET_HOST_CHNL_CLASS);

    ARRAY_TO_STREAM (pp, chnl_map, HCIC_BLE_CHNL_MAP_SIZE);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_read_chnl_map (UINT16 handle)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_READ_CHNL_MAP)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_READ_CHNL_MAP;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_READ_CHNL_MAP);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_READ_CHNL_MAP);

    UINT16_TO_STREAM (pp, handle);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_read_remote_feat (UINT16 handle)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_READ_REMOTE_FEAT)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_READ_REMOTE_FEAT;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_READ_REMOTE_FEAT);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_READ_REMOTE_FEAT);

    UINT16_TO_STREAM (pp, handle);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

/* security management commands */
BOOLEAN btsnd_hcic_ble_encrypt (UINT8 *key, UINT8 key_len,
                                UINT8 *plain_text, UINT8 pt_len,
                                void *p_cmd_cplt_cback)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_ENCRYPT)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_ENCRYPT;
    p->offset = 0;

    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(p);
    metadata->context = p_cmd_cplt_cback;

    UINT16_TO_STREAM (pp, HCI_BLE_ENCRYPT);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_ENCRYPT);

    memset(pp, 0, HCIC_PARAM_SIZE_BLE_ENCRYPT);

    if (key_len > HCIC_BLE_ENCRYT_KEY_SIZE) {
        key_len = HCIC_BLE_ENCRYT_KEY_SIZE;
    }
    if (pt_len > HCIC_BLE_ENCRYT_KEY_SIZE) {
        pt_len = HCIC_BLE_ENCRYT_KEY_SIZE;
    }

    ARRAY_TO_STREAM (pp, key, key_len);
    pp += (HCIC_BLE_ENCRYT_KEY_SIZE - key_len);
    ARRAY_TO_STREAM (pp, plain_text, pt_len);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_rand (void *p_cmd_cplt_cback)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_RAND)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_RAND;
    p->offset = 0;

    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(p);
    metadata->context = p_cmd_cplt_cback;

    UINT16_TO_STREAM (pp, HCI_BLE_RAND);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_RAND);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_start_enc (UINT16 handle, UINT8 rand[HCIC_BLE_RAND_DI_SIZE],
                                  UINT16 ediv, UINT8 ltk[HCIC_BLE_ENCRYT_KEY_SIZE])
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_START_ENC)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_START_ENC;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_START_ENC);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_START_ENC);

    UINT16_TO_STREAM (pp, handle);
    ARRAY_TO_STREAM (pp, rand, HCIC_BLE_RAND_DI_SIZE);
    UINT16_TO_STREAM (pp, ediv);
    ARRAY_TO_STREAM (pp, ltk, HCIC_BLE_ENCRYT_KEY_SIZE);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_ltk_req_reply (UINT16 handle, UINT8 ltk[HCIC_BLE_ENCRYT_KEY_SIZE])
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_LTK_REQ_REPLY)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_LTK_REQ_REPLY;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_LTK_REQ_REPLY);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_LTK_REQ_REPLY);

    UINT16_TO_STREAM (pp, handle);
    ARRAY_TO_STREAM (pp, ltk, HCIC_BLE_ENCRYT_KEY_SIZE);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_ltk_req_neg_reply (UINT16 handle)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_LTK_REQ_NEG_REPLY)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_LTK_REQ_NEG_REPLY;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_LTK_REQ_NEG_REPLY);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_LTK_REQ_NEG_REPLY);

    UINT16_TO_STREAM (pp, handle);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

#if (BLE_42_DTM_TEST_EN == TRUE)
BOOLEAN btsnd_hcic_ble_receiver_test(UINT8 rx_freq)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_WRITE_PARAM1)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_PARAM1;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_RECEIVER_TEST);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_WRITE_PARAM1);

    UINT8_TO_STREAM (pp, rx_freq);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_transmitter_test(UINT8 tx_freq, UINT8 test_data_len, UINT8 payload)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_WRITE_PARAM3)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_PARAM3;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_TRANSMITTER_TEST);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_WRITE_PARAM3);

    UINT8_TO_STREAM (pp, tx_freq);
    UINT8_TO_STREAM (pp, test_data_len);
    UINT8_TO_STREAM (pp, payload);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}
#endif // // #if (BLE_42_DTM_TEST_EN == TRUE)

#if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))
BOOLEAN btsnd_hcic_ble_test_end(void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_READ_CMD)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_READ_CMD;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_TEST_END);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_READ_CMD);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}
#endif // #if ((BLE_42_DTM_TEST_EN == TRUE) || (BLE_50_DTM_TEST_EN == TRUE))

BOOLEAN btsnd_hcic_ble_read_host_supported (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_READ_CMD)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_READ_CMD;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_READ_LE_HOST_SUPPORT);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_READ_CMD);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

#if (defined BLE_LLT_INCLUDED) && (BLE_LLT_INCLUDED == TRUE)

BOOLEAN btsnd_hcic_ble_rc_param_req_reply(  UINT16 handle,
        UINT16 conn_int_min, UINT16 conn_int_max,
        UINT16 conn_latency, UINT16 conn_timeout,
        UINT16 min_ce_len, UINT16 max_ce_len  )
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_REPLY)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_REPLY;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_RC_PARAM_REQ_REPLY);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_REPLY);

    UINT16_TO_STREAM (pp, handle);
    UINT16_TO_STREAM (pp, conn_int_min);
    UINT16_TO_STREAM (pp, conn_int_max);
    UINT16_TO_STREAM (pp, conn_latency);
    UINT16_TO_STREAM (pp, conn_timeout);
    UINT16_TO_STREAM (pp, min_ce_len);
    UINT16_TO_STREAM (pp, max_ce_len);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_rc_param_req_neg_reply(UINT16 handle, UINT8 reason)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_NEG_REPLY)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_NEG_REPLY;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_RC_PARAM_REQ_NEG_REPLY);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_RC_PARAM_REQ_NEG_REPLY);

    UINT16_TO_STREAM (pp, handle);
    UINT8_TO_STREAM (pp, reason);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}
#endif

BOOLEAN btsnd_hcic_ble_add_device_resolving_list (UINT8 addr_type_peer, BD_ADDR bda_peer,
        UINT8 irk_peer[HCIC_BLE_IRK_SIZE],
        UINT8 irk_local[HCIC_BLE_IRK_SIZE])
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_ADD_DEV_RESOLVING_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_ADD_DEV_RESOLVING_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_ADD_DEV_RESOLVING_LIST);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_ADD_DEV_RESOLVING_LIST);
    UINT8_TO_STREAM (pp, addr_type_peer);
    BDADDR_TO_STREAM (pp, bda_peer);
    ARRAY_TO_STREAM (pp, irk_peer, HCIC_BLE_ENCRYT_KEY_SIZE);
    ARRAY_TO_STREAM (pp, irk_local, HCIC_BLE_ENCRYT_KEY_SIZE);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_rm_device_resolving_list (UINT8 addr_type_peer, BD_ADDR bda_peer)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_RM_DEV_RESOLVING_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_RM_DEV_RESOLVING_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_RM_DEV_RESOLVING_LIST);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_RM_DEV_RESOLVING_LIST);
    UINT8_TO_STREAM (pp, addr_type_peer);
    BDADDR_TO_STREAM (pp, bda_peer);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_clear_resolving_list (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_CLEAR_RESOLVING_LIST)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_CLEAR_RESOLVING_LIST;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_CLEAR_RESOLVING_LIST);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_CLEAR_RESOLVING_LIST);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_read_resolvable_addr_peer (UINT8 addr_type_peer, BD_ADDR bda_peer)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_PEER)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_PEER;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_READ_RESOLVABLE_ADDR_PEER);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_PEER);
    UINT8_TO_STREAM (pp, addr_type_peer);
    BDADDR_TO_STREAM (pp, bda_peer);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_read_resolvable_addr_local (UINT8 addr_type_peer, BD_ADDR bda_peer)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_LOCAL)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_LOCAL;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_READ_RESOLVABLE_ADDR_LOCAL);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_READ_RESOLVABLE_ADDR_LOCAL);
    UINT8_TO_STREAM (pp, addr_type_peer);
    BDADDR_TO_STREAM (pp, bda_peer);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_addr_resolution_enable (UINT8 addr_resolution_enable)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_SET_ADDR_RESOLUTION_ENABLE)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_ADDR_RESOLUTION_ENABLE;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_SET_ADDR_RESOLUTION_ENABLE);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_SET_ADDR_RESOLUTION_ENABLE);
    UINT8_TO_STREAM (pp, addr_resolution_enable);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_rand_priv_addr_timeout (UINT16 rpa_timout)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_SET_RAND_PRIV_ADDR_TIMOUT)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_RAND_PRIV_ADDR_TIMOUT;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_SET_RAND_PRIV_ADDR_TIMOUT);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_SET_RAND_PRIV_ADDR_TIMOUT);
    UINT16_TO_STREAM (pp, rpa_timout);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);

    return (TRUE);
}

BOOLEAN btsnd_hcic_ble_set_data_length(UINT16 conn_handle, UINT16 tx_octets, UINT16 tx_time)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_SET_DATA_LENGTH)) == NULL) {
        return FALSE;
    }

    pp = p->data;

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_DATA_LENGTH;
    p->offset = 0;

    UINT16_TO_STREAM(pp, HCI_BLE_SET_DATA_LENGTH);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BLE_SET_DATA_LENGTH);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT16_TO_STREAM(pp, tx_octets);
    UINT16_TO_STREAM(pp, tx_time);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_update_adv_report_flow_control (UINT16 num, BT_HDR *static_buf)
{
    BT_HDR *p;
    UINT8 *pp;

    if (static_buf != NULL) {
        p = static_buf;
    } else {
        if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_UPDATE_ADV_FLOW_CONTROL)) == NULL) {
            return (FALSE);
        }
    }

    hci_cmd_metadata_t *metadata = HCI_GET_CMD_METAMSG(p);
    metadata->flags_src = HCI_CMD_MSG_F_SRC_NOACK;
    if (static_buf == p) {
        assert(metadata->command_free_cb != NULL);
    }
    p->layer_specific = HCI_CMD_BUF_TYPE_METADATA;

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_UPDATE_ADV_FLOW_CONTROL;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_VENDOR_BLE_ADV_REPORT_FLOW_CONTROL);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_UPDATE_ADV_FLOW_CONTROL);
    UINT16_TO_STREAM (pp, num);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_set_channels (BLE_CHANNELS channels)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF(HCIC_PARAM_SIZE_BLE_SET_CHANNELS)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len    = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_CHANNELS;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_BLE_SET_HOST_CHNL_CLASS);
    UINT8_TO_STREAM  (pp, HCIC_PARAM_SIZE_BLE_SET_CHANNELS);

    ARRAY_TO_STREAM  (pp, channels, HCIC_PARAM_SIZE_BLE_SET_CHANNELS);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID,  p);
    return (TRUE);
}

#define HCIC_BLE_CMD_CREATED(p, pp, size) do{\
    if ((p = HCI_GET_CMD_BUF(size)) == NULL) { \
        return FALSE; \
    } \
    pp = p->data; \
    p->len = HCIC_PREAMBLE_SIZE + size;\
    p->offset = 0; \
} while(0)

#if (BLE_50_FEATURE_SUPPORT == TRUE)

BOOLEAN btsnd_hcic_ble_read_phy(UINT16 conn_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_EVENT("%s, conn_handle = %d", __func__, conn_handle);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BLE_READ_PHY);

    UINT16_TO_STREAM(pp, HCI_BLE_READ_PHY);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BLE_READ_PHY);
    UINT16_TO_STREAM(pp, conn_handle);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_set_prefered_default_phy(UINT8 all_phys,
                                                              UINT8 tx_phys,
                                                              UINT8 rx_phys)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_EVENT("%s, all_phys = %d, tx_phys = %d, rx_phys = %d", __func__, all_phys, tx_phys, rx_phys);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BLE_SET_DEF_PHY);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_DEFAULT_PHY);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BLE_SET_DEF_PHY);

    UINT8_TO_STREAM(pp, all_phys);
    UINT8_TO_STREAM(pp, tx_phys);
    UINT8_TO_STREAM(pp, rx_phys);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

BOOLEAN btsnd_hcic_ble_set_phy(UINT16 conn_handle,
                                           UINT8 all_phys, UINT8 tx_phys,
                                           UINT8 rx_phys, UINT16 phy_options)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, conn_handle = %d, all_phys = %d, tx_phys = %d, rx_phys = %d, phy_options = %d", __func__,
        conn_handle, all_phys, tx_phys, rx_phys, phy_options);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BLE_SET_PHY);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PHY);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BLE_SET_PHY);
    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, all_phys);
    UINT8_TO_STREAM(pp, tx_phys);
    UINT8_TO_STREAM(pp, rx_phys);
    UINT16_TO_STREAM(pp, phy_options);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

#if (BLE_50_DTM_TEST_EN == TRUE)
UINT8 btsnd_hcic_ble_enhand_rx_test(UINT8 rx_channel, UINT8 phy,
                                                         UINT8 modulation_idx)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_EVENT("%s, rx_channel = %d, phy = %d, modulation_idx = %d", __func__,
        rx_channel, phy, modulation_idx);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ENH_RX_TEST);

    UINT16_TO_STREAM(pp, HCI_BLE_ENH_RX_TEST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ENH_RX_TEST);
    UINT8_TO_STREAM(pp, rx_channel);
    UINT8_TO_STREAM(pp, phy);
    UINT8_TO_STREAM(pp, modulation_idx);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);

    return TRUE;
}

UINT8 btsnd_hcic_ble_enhand_tx_test(UINT8 tx_channel, UINT8 len,
                                                         UINT8 packect,
                                                         UINT8 phy)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_EVENT("%s, tx_channel = %d, len = %d, packect = %d, phy = %d", __func__,
            tx_channel, len, packect, phy);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ENH_TX_TEST);

    UINT16_TO_STREAM(pp, HCI_BLE_ENH_TX_TEST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ENH_TX_TEST);
    UINT8_TO_STREAM(pp, tx_channel);
    UINT8_TO_STREAM(pp, len);
    UINT8_TO_STREAM(pp, packect);
    UINT8_TO_STREAM(pp, phy);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);

    return TRUE;
}
#endif // #if (BLE_50_DTM_TEST_EN == TRUE)

#if (BLE_50_EXTEND_ADV_EN == TRUE)
UINT8 btsnd_hcic_ble_set_extend_rand_address(UINT8 adv_handle, BD_ADDR rand_addr)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_handle = %d", __func__, adv_handle);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_EXT_RAND_ADDR);

    UINT16_TO_STREAM (pp, HCI_BLE_SET_ADV_RAND_ADDR);
    UINT8_TO_STREAM  (pp,  HCIC_PARAM_SIZE_EXT_RAND_ADDR);
    UINT8_TO_STREAM(pp, adv_handle);
    BDADDR_TO_STREAM (pp, rand_addr);

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_set_ext_adv_params(UINT8 adv_handle, UINT16 properties, UINT32 interval_min,
                                          UINT32 interval_max, UINT8 channel_map, UINT8 own_addr_type,
                                          UINT8 peer_addr_type, BD_ADDR peer_addr,
                                          UINT8 adv_filter_policy, INT8 adv_tx_power,
                                          UINT8 primary_adv_phy, UINT8 secondary_adv_max_skip,
                                          UINT8 secondary_adv_phy,
                                          UINT8 adv_sid, UINT8 scan_req_ntf_enable)
{
    BT_HDR *p;
    UINT8 *pp;


    HCI_TRACE_EVENT("%s, adv_handle = %d, properties = %d, interval_min = %d, interval_max = %d, channel_map = %d,\n\
                     own_addr_type = %d, peer_addr_type = %d, adv_filter_policy = %d,\n\
                     adv_tx_power = %d, primary_adv_phy = %d, secondary_adv_max_skip = %d, secondary_adv_phy = %d,\n\
                     adv_sid = %d, scan_req_ntf_enable = %d", __func__, adv_handle, properties, interval_min, interval_max,
                     channel_map, own_addr_type, peer_addr_type, adv_filter_policy, adv_tx_power,
                     primary_adv_phy, secondary_adv_max_skip, secondary_adv_phy, adv_sid, scan_req_ntf_enable);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_EXT_ADV_SET_PARAMS);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_ADV_PARAM);
    UINT8_TO_STREAM(pp,  HCIC_PARAM_SIZE_EXT_ADV_SET_PARAMS);

    UINT8_TO_STREAM(pp, adv_handle);
    UINT16_TO_STREAM(pp, properties);
    UINT24_TO_STREAM(pp, interval_min);
    UINT24_TO_STREAM(pp, interval_max);
    UINT8_TO_STREAM(pp, channel_map);
    UINT8_TO_STREAM(pp, own_addr_type);
    UINT8_TO_STREAM(pp, peer_addr_type);
    BDADDR_TO_STREAM (pp, peer_addr);
    UINT8_TO_STREAM(pp, adv_filter_policy);
    INT8_TO_STREAM(pp, adv_tx_power);
    UINT8_TO_STREAM(pp, primary_adv_phy);
    UINT8_TO_STREAM(pp, secondary_adv_max_skip);
    UINT8_TO_STREAM(pp, secondary_adv_phy);
    UINT8_TO_STREAM(pp, adv_sid);
    UINT8_TO_STREAM(pp, scan_req_ntf_enable);

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);
}

bool ext_adv_flag = false;

UINT8 btsnd_hcic_ble_set_ext_adv_data(UINT8 adv_handle,
                                      UINT8 operation, UINT8 fragment_prefrence,
                                      UINT8 data_len, UINT8 *p_data)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_handle = %d, operation = %d, fragment_prefrence = %d,\
                     data_len = %d", __func__, adv_handle, operation, fragment_prefrence, data_len);
    ext_adv_flag = true;

    HCIC_BLE_CMD_CREATED(p, pp,  data_len + 4);
    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_ADV_DATA);
    UINT8_TO_STREAM(pp, data_len + 4);
    UINT8_TO_STREAM(pp, adv_handle);
    UINT8_TO_STREAM(pp, operation);
    UINT8_TO_STREAM(pp, fragment_prefrence);

    if (data_len > HCIC_PARAM_SIZE_EXT_ADV_WRITE_DATA) {
        data_len = HCIC_PARAM_SIZE_EXT_ADV_WRITE_DATA;
    }

    UINT8_TO_STREAM (pp, data_len);

    if (p_data != NULL && data_len > 0){
        ARRAY_TO_STREAM (pp, p_data, data_len);
    }

    uint8_t status = btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return status;

}

UINT8 btsnd_hcic_ble_set_ext_adv_scan_rsp_data(UINT8 adv_handle,
                                                          UINT8 operation, UINT8 fragment_prefrence,
                                                          UINT8 data_len, UINT8 *p_data)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_EVENT("%s, adv_handle = %d, operation = %d, fragment_prefrence = %d,\n\
                     data_len = %d", __func__, adv_handle, operation, fragment_prefrence, data_len);

    HCIC_BLE_CMD_CREATED(p, pp,  data_len + 4);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_SCAN_RSP_DATA);
    UINT8_TO_STREAM(pp, data_len + 4);
    UINT8_TO_STREAM(pp, adv_handle);
    UINT8_TO_STREAM(pp, operation);
    UINT8_TO_STREAM(pp, fragment_prefrence);

    memset(pp, 0, data_len);

    if (data_len > HCIC_PARAM_SIZE_EXT_ADV_WRITE_DATA) {
        data_len = HCIC_PARAM_SIZE_EXT_ADV_WRITE_DATA;
    }

    UINT8_TO_STREAM (pp, data_len);
    if (p_data != NULL && data_len > 0) {
        ARRAY_TO_STREAM (pp, p_data, data_len);
    }

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_ext_adv_enable(UINT8 enable, UINT8 num_of_sets, UINT8 *adv_handle,
                                                       UINT16 *duration, UINT8 *max_adv_evt)
{
    BT_HDR *p;
    UINT8 *pp;
    UINT8 ext_adv_size = num_of_sets*4 + 2;
    HCIC_BLE_CMD_CREATED(p, pp,  ext_adv_size);

    HCI_TRACE_EVENT("%s, enable = %d, num_of_sets = %d", __func__, enable, num_of_sets);

    for (int k = 0; k < num_of_sets; k++) {
        HCI_TRACE_EVENT("adv_handle[%d] = %d, duration[%d] = %d, max_adv_evt[%d] = %d", k, adv_handle[k],
            k, duration[k], k, max_adv_evt[k]);
    }

    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_ADV_ENABLE);
    UINT8_TO_STREAM(pp, ext_adv_size);
    UINT8_TO_STREAM(pp, enable);
    UINT8_TO_STREAM(pp, num_of_sets);

    for (int i = 0; i < num_of_sets; i++) {
        UINT8_TO_STREAM(pp, adv_handle[i]);
        UINT16_TO_STREAM(pp, duration[i]);
        UINT8_TO_STREAM(pp, max_adv_evt[i]);
    }

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_read_max_adv_len(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_READ_MAX_ADV_SIZE + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_RD_MAX_ADV_DATA_LEN);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_MAX_ADV_SIZE);

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_read_num_support_adv_set(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_NUM_SUPPORT_ADV_SET + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_RD_NUM_OF_ADV_SETS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_NUM_SUPPORT_ADV_SET);

    return btu_hcif_send_cmd_sync (LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_remove_adv_set(UINT8 adv_handle)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_handle = %d", __func__, adv_handle);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_REMOVE_ADV_SET + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_REMOVE_ADV_SET);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_REMOVE_ADV_SET);
    UINT8_TO_STREAM(pp, adv_handle);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_clear_adv_set(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_CLEAR_ADV_SET + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_CLEAR_ADV_SETS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_CLEAR_ADV_SET);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}
#endif // #if (BLE_50_EXTEND_ADV_EN == TRUE)

#if (BLE_50_PERIODIC_ADV_EN == TRUE)
UINT8 btsnd_hcic_ble_set_periodic_adv_params(UINT8 adv_handle,
                                                                     UINT16 interval_min,
                                                                     UINT16 interval_max,
                                                                     UINT8 propertics)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_handle = %d, interval_min = %d, interval_max = %d, propertics = %d",
                   __func__, adv_handle, interval_min, interval_max, propertics);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_SET_PERIODIC_ADV_PARAMS);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PERIOD_ADV_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_PERIODIC_ADV_PARAMS);

    UINT8_TO_STREAM(pp, adv_handle);
    UINT16_TO_STREAM(pp, interval_min);
    UINT16_TO_STREAM(pp, interval_max);
    UINT16_TO_STREAM(pp, propertics);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_set_periodic_adv_data(UINT8 adv_handle,
                                                                  UINT8 operation,
                                                                  UINT8 len,
                                                                  UINT8 *p_data)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_handle = %d, operation = %d, len = %d",
                       __func__, adv_handle, operation, len);

    HCIC_BLE_CMD_CREATED(p, pp,  len + 3);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PERIOD_ADV_DATA);
    UINT8_TO_STREAM(pp, len + 3);
    UINT8_TO_STREAM(pp, adv_handle);
    UINT8_TO_STREAM(pp, operation);

    //memset(pp, 0, len);

    if (len > HCIC_PARAM_SIZE_WRITE_PERIODIC_ADV_DATA) {
        len = HCIC_PARAM_SIZE_WRITE_PERIODIC_ADV_DATA;
    }

    UINT8_TO_STREAM (pp, len);

    if (p_data != NULL && len > 0) {
        ARRAY_TO_STREAM (pp, p_data, len);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_periodic_adv_enable(UINT8 enable, UINT8 adv_handle)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, enable = %d, adv_handle = %d",
                           __func__, enable, adv_handle);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_PERIODIC_ADV_ENABLE);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PERIOD_ADV_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_ENABLE);
    UINT8_TO_STREAM(pp, enable);
    UINT8_TO_STREAM(pp, adv_handle);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}
#endif // #if (BLE_50_PERIODIC_ADV_EN == TRUE)

#if (BLE_50_EXTEND_SCAN_EN == TRUE)
UINT8 btsnd_hcic_ble_set_ext_scan_params(UINT8 own_addr_type, UINT8 filter_policy,
                                                               UINT8 phy_mask, UINT8 phy_count,
                                                               tHCI_EXT_SCAN_PARAMS *params)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, own_addr_type = %d, filter_policy = %d, phy_mask = %d, phy_count = %d",
                           __func__, own_addr_type, filter_policy, phy_mask, phy_count);
    UINT8 params_size = HCIC_PARAM_SIZE_SET_EXT_SCAN_PARAMS + phy_count*5;

    HCIC_BLE_CMD_CREATED(p, pp,  params_size);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_SCAN_PARAMS);
    UINT8_TO_STREAM(pp, params_size);
    UINT8_TO_STREAM(pp, own_addr_type);
    UINT8_TO_STREAM(pp, filter_policy);
    UINT8_TO_STREAM(pp, phy_mask);

    for (int i = 0; i < phy_count; i++) {
        UINT8_TO_STREAM(pp, params[i].scan_type);
        UINT16_TO_STREAM(pp, params[i].scan_interval);
        UINT16_TO_STREAM(pp, params[i].scan_window);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_ext_scan_enable(UINT8 enable, UINT8 filter_dups,
                                                         UINT16 duration, UINT16 period)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, enable = %d, filter_dups = %d, duration = %d, period = %d",
                               __func__, enable, filter_dups, duration, period);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_EXT_SCAN_ENABLE);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_EXT_SCAN_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_EXT_SCAN_ENABLE);
    UINT8_TO_STREAM(pp, enable);
    UINT8_TO_STREAM(pp, filter_dups);
    UINT16_TO_STREAM(pp, duration);
    UINT16_TO_STREAM(pp, period);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_50_EXTEND_SCAN_EN == TRUE)

BOOLEAN btsnd_hcic_ble_create_ext_conn(tHCI_CreatExtConn *p_conn)
{
    BT_HDR *p;
    UINT8 *pp;
    tHCI_ExtConnParams *params;
    HCI_TRACE_EVENT("%s", __func__);
    uint8_t size = HCIC_PARAM_SIZE_EXT_CONN_CREATE_BASE;

    if (p_conn->init_phy_mask & 0x01) {
        size += sizeof(tHCI_ExtConnParams);
    }

    if (p_conn->init_phy_mask & 0x02) {
        size += sizeof(tHCI_ExtConnParams);
    }

    if (p_conn->init_phy_mask & 0x04) {
        size += sizeof(tHCI_ExtConnParams);
    }

    HCIC_BLE_CMD_CREATED(p, pp,  size);

    UINT16_TO_STREAM(pp, HCI_BLE_EXT_CREATE_CONN);
    UINT8_TO_STREAM(pp, size);
    UINT8_TO_STREAM(pp, p_conn->filter_policy);
    UINT8_TO_STREAM(pp, p_conn->own_addr_type);
    UINT8_TO_STREAM(pp, p_conn->peer_addr_type);
    BDADDR_TO_STREAM(pp, p_conn->peer_addr);
    UINT8_TO_STREAM(pp, p_conn->init_phy_mask);

    if (p_conn->init_phy_mask & 0x01) {
        params = &p_conn->params[0];
        UINT16_TO_STREAM(pp, params->scan_interval);
        UINT16_TO_STREAM(pp, params->scan_window);
        UINT16_TO_STREAM(pp, params->conn_interval_min);
        UINT16_TO_STREAM(pp, params->conn_interval_max);
        UINT16_TO_STREAM(pp, params->conn_latency);
        UINT16_TO_STREAM(pp, params->sup_timeout);
        UINT16_TO_STREAM(pp, params->min_ce_len ? params->min_ce_len : BLE_CE_LEN_MIN);
        UINT16_TO_STREAM(pp, params->max_ce_len ? params->max_ce_len : BLE_CE_LEN_MIN);
    }

    if (p_conn->init_phy_mask & 0x02) {
        params = &p_conn->params[1];
        UINT16_TO_STREAM(pp, params->scan_interval);
        UINT16_TO_STREAM(pp, params->scan_window);
        UINT16_TO_STREAM(pp, params->conn_interval_min);
        UINT16_TO_STREAM(pp, params->conn_interval_max);
        UINT16_TO_STREAM(pp, params->conn_latency);
        UINT16_TO_STREAM(pp, params->sup_timeout);
        UINT16_TO_STREAM(pp, params->min_ce_len ? params->min_ce_len : BLE_CE_LEN_MIN);
        UINT16_TO_STREAM(pp, params->max_ce_len ? params->max_ce_len : BLE_CE_LEN_MIN);
    }

    if (p_conn->init_phy_mask & 0x04) {
        params = &p_conn->params[2];
        UINT16_TO_STREAM(pp, params->scan_interval);
        UINT16_TO_STREAM(pp, params->scan_window);
        UINT16_TO_STREAM(pp, params->conn_interval_min);
        UINT16_TO_STREAM(pp, params->conn_interval_max);
        UINT16_TO_STREAM(pp, params->conn_latency);
        UINT16_TO_STREAM(pp, params->sup_timeout);
        UINT16_TO_STREAM(pp, params->min_ce_len ? params->min_ce_len : BLE_CE_LEN_MIN);
        UINT16_TO_STREAM(pp, params->max_ce_len ? params->max_ce_len : BLE_CE_LEN_MIN);
    }

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;

}

#if (BLE_50_EXTEND_SYNC_EN == TRUE)
BOOLEAN btsnd_hcic_ble_periodic_adv_create_sync(UINT8 option, UINT8 adv_sid,
                                                                       UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                       UINT16 sync_timeout, UINT8 sync_cte_type)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, option = %d, adv_sid = %d, adv_addr_type = %d, sync_timeout = %d, sync_cte_type = %d",
                                   __func__, option, adv_sid, adv_addr_type, sync_timeout, sync_cte_type);

    HCI_TRACE_EVENT("addr %02x %02x %02x %02x %02x %02x", adv_addr[0], adv_addr[1], adv_addr[2], adv_addr[3], adv_addr[4], adv_addr[5]);
    uint16_t skip = 0;
    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC + 2);

    UINT16_TO_STREAM(pp, HCI_BLE_PERIOD_ADV_CREATE_SYNC);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC + 2);
    UINT8_TO_STREAM(pp, option);
    UINT8_TO_STREAM(pp, adv_sid);
    UINT8_TO_STREAM(pp, adv_addr_type);
    BDADDR_TO_STREAM(pp, adv_addr);
    UINT16_TO_STREAM(pp, skip);
    UINT16_TO_STREAM(pp, sync_timeout);
    UINT8_TO_STREAM(pp, sync_cte_type);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_periodic_adv_create_sync_cancel(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC_CANCEL);

    UINT16_TO_STREAM(pp, HCI_BLE_PERIOD_ADV_CREATE_SYNC_CANCEL);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_CREATE_SYNC_CANCEL);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_periodic_adv_term_sync(UINT16 sync_handle)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, sync_handle = %d", __func__, sync_handle);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_PERIODIC_ADV_TERM_SYNC);

    UINT16_TO_STREAM(pp, HCI_BLE_PERIOD_ADV_TERM_SYNC);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_TERM_SYNC);

    UINT16_TO_STREAM(pp, sync_handle);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_add_dev_to_periodic_adv_list(UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                             UINT8 adv_sid)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_addr_type = %d, adv_sid = %d", __func__, adv_addr_type, adv_sid);
    esp_log_buffer_hex_internal("addr", adv_addr, sizeof(BD_ADDR), ESP_LOG_WARN);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_ADD_DEV_TO_PERIODIC_ADV_LIST);

    UINT16_TO_STREAM(pp, HCI_BLE_ADV_DEV_TO_PERIOD_ADV_LIST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ADD_DEV_TO_PERIODIC_ADV_LIST);
    UINT8_TO_STREAM(pp, adv_addr_type);
    BDADDR_TO_STREAM(pp, adv_addr);
    UINT8_TO_STREAM(pp, adv_sid);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_rm_dev_from_periodic_adv_list(UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                             UINT8 adv_sid)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, adv_addr_type = %d, adv_sid = %d", __func__, adv_addr_type, adv_sid);
    esp_log_buffer_hex_internal("addr", adv_addr, sizeof(BD_ADDR), ESP_LOG_WARN);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_RM_DEV_FROM_PERIODIC_ADV_LIST);

    UINT16_TO_STREAM(pp, HCI_BLE_REMOVE_DEV_FROM_PERIOD_ADV_LIST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_RM_DEV_FROM_PERIODIC_ADV_LIST);
    UINT8_TO_STREAM(pp, adv_addr_type);
    BDADDR_TO_STREAM(pp, adv_addr);
    UINT8_TO_STREAM(pp, adv_sid);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);

}

UINT8 btsnd_hcic_ble_clear_periodic_adv_list(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_CLEAR_PERIODIC_ADV_LIST);

    UINT16_TO_STREAM(pp, HCI_BLE_CLEAR_PERIOD_ADV_LIST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_CLEAR_PERIODIC_ADV_LIST);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_read_periodic_adv_list_size(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_READ_PERIODIC_ADV_LIST);

    UINT16_TO_STREAM(pp, HCI_BLE_RD_PERIOD_ADV_LIST_SIZE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_PERIODIC_ADV_LIST);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_50_EXTEND_SYNC_EN == TRUE)

UINT8 btsnd_hcic_ble_read_trans_power(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_READ_TRANS_POWER);

    UINT16_TO_STREAM(pp, HCI_BLE_RD_TRANSMIT_POWER);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_TRANS_POWER);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_read_rf_path_compensation(void)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s", __func__);

    HCIC_BLE_CMD_CREATED(p, pp,  HCIC_PARAM_SIZE_READ_RF_PATH_COMPENSATION);

    UINT16_TO_STREAM(pp, HCI_BLE_RD_RF_PATH_COMPENSATION);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_RF_PATH_COMPENSATION);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_write_rf_path_compensation(UINT16 rf_tx_path, UINT16 rf_rx_path)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, rf_tx_path = %d, rf_rx_path = %d", __func__, rf_tx_path, rf_rx_path);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_WRITE_RF_PATH_COMPENSATION);

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_WRITE_RF_PATH_COMPENSATION;
    p->offset = 0;

    UINT16_TO_STREAM(pp, HCI_BLE_WR_RF_PATH_COMPENSATION);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_WRITE_RF_PATH_COMPENSATION);

    UINT16_TO_STREAM(pp, rf_tx_path);
    UINT16_TO_STREAM(pp, rf_tx_path);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
UINT8 btsnd_hcic_ble_set_periodic_adv_recv_enable(UINT16 sync_handle, UINT8 enable)
{
    BT_HDR *p;
    UINT8 *pp;

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_PERIODIC_ADV_RECV_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PERIOD_ADV_RECV_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_RECV_ENABLE);

    UINT16_TO_STREAM(pp, sync_handle);
    UINT8_TO_STREAM(pp, enable);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

BOOLEAN btsnd_hcic_ble_periodic_adv_sync_trans(UINT16 conn_handle, UINT16 service_data, UINT16 sync_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_PERIODIC_ADV_SYNC_TRANS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_PERIOD_ADV_SYNC_TRANS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_SYNC_TRANS);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT16_TO_STREAM(pp, service_data);
    UINT16_TO_STREAM(pp, sync_handle);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_periodic_adv_set_info_trans(UINT16 conn_handle, UINT16 service_data, UINT8 adv_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s conn handle %x, adv handle %x", __func__, conn_handle, adv_handle);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_PERIODIC_ADV_SET_INFO_TRANS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_PERIOD_ADV_SET_INFO_TRANS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_PERIODIC_ADV_SET_INFO_TRANS);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT16_TO_STREAM(pp, service_data);
    UINT8_TO_STREAM(pp, adv_handle);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_set_periodic_adv_sync_trans_params(UINT16 conn_handle, UINT8 mode, UINT16 skip, UINT16 sync_timeout, UINT8 cte_type)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s conn handle %x, mode %x, sync timeout %x", __func__, conn_handle, mode, sync_timeout);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_PAST_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PAST_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_PAST_PARAMS);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, mode);
    UINT16_TO_STREAM(pp, skip);
    UINT16_TO_STREAM(pp, sync_timeout);
    UINT8_TO_STREAM(pp, cte_type);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_set_default_periodic_adv_sync_trans_params(UINT8 mode, UINT16 skip, UINT16 sync_timeout, UINT8 cte_type)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s mode %x, skip %x, sync timeout %x", __func__, mode, skip, sync_timeout);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_DEFAULT_PAST_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_DEFAULT_PAST_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_DEFAULT_PAST_PARAMS);

    UINT8_TO_STREAM(pp, mode);
    UINT16_TO_STREAM(pp, skip);
    UINT16_TO_STREAM(pp, sync_timeout);
    UINT8_TO_STREAM(pp, cte_type);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

BOOLEAN btsnd_hcic_ble_set_privacy_mode(UINT8 addr_type, BD_ADDR addr, UINT8 privacy_mode)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_SET_PRIVACY_MODE)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);
    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_SET_PRIVACY_MODE;
    p->offset = 0;

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PRIVACY_MODE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_PRIVACY_MODE);

    UINT8_TO_STREAM(pp, addr_type);
    BDADDR_TO_STREAM(pp, addr);
    UINT8_TO_STREAM(pp, privacy_mode);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return (TRUE);
}

#if (BLE_VENDOR_HCI_EN == TRUE)
BOOLEAN btsnd_hcic_ble_clear_adv (void)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_CLEAR_ADV)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_CLEAR_ADV;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_VENDOR_BLE_CLEAR_ADV);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_CLEAR_ADV);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_set_csa_support (UINT8 csa_select)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_SET_CSA_SUPPORT)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_CSA_SUPPORT;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_VENDOR_BLE_SET_CSA_SUPPORT);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_SET_CSA_SUPPORT);
    UINT8_TO_STREAM (pp, csa_select);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

BOOLEAN btsnd_hcic_ble_set_vendor_evt_mask (UINT32 evt_mask)
{
    BT_HDR *p;
    UINT8 *pp;

    if ((p = HCI_GET_CMD_BUF (HCIC_PARAM_SIZE_BLE_SET_VENDOR_EVT_MASK)) == NULL) {
        return (FALSE);
    }

    pp = (UINT8 *)(p + 1);

    p->len = HCIC_PREAMBLE_SIZE + HCIC_PARAM_SIZE_BLE_SET_VENDOR_EVT_MASK;
    p->offset = 0;

    UINT16_TO_STREAM (pp, HCI_VENDOR_BLE_SET_EVT_MASK);
    UINT8_TO_STREAM (pp, HCIC_PARAM_SIZE_BLE_SET_VENDOR_EVT_MASK);
    UINT32_TO_STREAM (pp, evt_mask);

    btu_hcif_send_cmd (LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}
#endif // #if (BLE_VENDOR_HCI_EN == TRUE)
#endif

#if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
UINT8 btsnd_hcic_ble_big_create(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                                uint32_t sdu_interval, uint16_t max_sdu, uint16_t max_transport_latency,
                                uint8_t rtn, uint8_t phy, uint8_t packing, uint8_t framing,
                                uint8_t encryption, uint8_t *broadcast_code)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci big create: big_handle %d, adv_handle %d, num_bis %d sdu_interval %d max_sdu %d max_transport_latency %d \
    rtn %d phy %d packing %d framing %d encryption %d", big_handle, adv_handle, num_bis, sdu_interval, max_sdu,\
    max_transport_latency, rtn, phy, packing, framing, encryption);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BIG_CREATE_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_CREATE_BIG);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BIG_CREATE_PARAMS);

    UINT8_TO_STREAM(pp, big_handle);
    UINT8_TO_STREAM(pp, adv_handle);
    UINT8_TO_STREAM(pp, num_bis);
    UINT24_TO_STREAM(pp, sdu_interval);
    UINT16_TO_STREAM(pp, max_sdu);
    UINT16_TO_STREAM(pp, max_transport_latency);
    UINT8_TO_STREAM(pp, rtn);
    UINT8_TO_STREAM(pp, phy);
    UINT8_TO_STREAM(pp, packing);
    UINT8_TO_STREAM(pp, framing);
    UINT8_TO_STREAM(pp, encryption);
    ARRAY_TO_STREAM(pp, broadcast_code, 16);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_big_create_test(uint8_t big_handle, uint8_t adv_handle, uint8_t num_bis,
                                    uint32_t sdu_interval, uint16_t iso_interval, uint8_t nse,
                                    uint16_t max_sdu, uint16_t max_pdu, uint8_t phy,
                                    uint8_t packing, uint8_t framing, uint8_t bn, uint8_t irc,
                                    uint8_t pto, uint8_t encryption, uint8_t *broadcast_code)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s big_handle %d, adv_handle %d, num_bis %d sdu_interval %d max_sdu %d max_pdu %d iso_interval %d \
    nse %d phy %d packing %d framing %d bn %d irc %dencryption %d", __func__, big_handle, adv_handle, num_bis, sdu_interval, max_sdu, max_pdu, \
    iso_interval, nse, phy, packing, framing, bn, irc, encryption);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BIG_CREATE_TEST_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_CREATE_BIG_TEST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BIG_CREATE_TEST_PARAMS);

    UINT8_TO_STREAM(pp, big_handle);
    UINT8_TO_STREAM(pp, adv_handle);
    UINT8_TO_STREAM(pp, num_bis);
    UINT24_TO_STREAM(pp, sdu_interval);
    UINT16_TO_STREAM(pp, iso_interval);
    UINT8_TO_STREAM(pp, nse);
    UINT16_TO_STREAM(pp, max_sdu);
    UINT16_TO_STREAM(pp, max_pdu);
    UINT8_TO_STREAM(pp, phy);
    UINT8_TO_STREAM(pp, packing);
    UINT8_TO_STREAM(pp, framing);
    UINT8_TO_STREAM(pp, bn);
    UINT8_TO_STREAM(pp, irc);
    UINT8_TO_STREAM(pp, pto);
    UINT8_TO_STREAM(pp, encryption);
    ARRAY_TO_STREAM(pp, broadcast_code, 16);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_big_terminate(uint8_t big_handle, uint8_t reason)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s big_handle %d reason 0x%x", __func__, big_handle, reason);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BIG_TERMINATE_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_TERMINATE_BIG);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BIG_TERMINATE_PARAMS);

    UINT8_TO_STREAM(pp, big_handle);
    UINT8_TO_STREAM(pp, reason);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}
#endif // #if (BLE_FEAT_ISO_BIG_BROCASTER_EN == TRUE)
#if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
UINT8 btsnd_hcic_ble_big_sync_create(uint8_t big_handle, uint16_t sync_handle,
                                    uint8_t encryption, uint8_t *bc_code,
                                    uint8_t mse, uint16_t big_sync_timeout,
                                    uint8_t num_bis, uint8_t *bis)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("big sync create: big_handle %d sync_handle %d encryption %d mse %d big_sync_timeout %d", big_handle, sync_handle, encryption, mse, big_sync_timeout);

    // for (uint8_t i = 0; i < num_bis; i++)
    // {
    //     HCI_TRACE_ERROR("i %d bis %d", bis[i]);
    // }

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BIG_SYNC_CREATE_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_BIG_CREATE_SYNC);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BIG_SYNC_CREATE_PARAMS);

    UINT8_TO_STREAM(pp, big_handle);
    UINT16_TO_STREAM(pp, sync_handle);
    UINT8_TO_STREAM(pp, encryption);
    ARRAY_TO_STREAM(pp, bc_code, 16);
    UINT8_TO_STREAM(pp, mse);
    UINT16_TO_STREAM(pp, big_sync_timeout);
    UINT8_TO_STREAM(pp, num_bis);
    ARRAY_TO_STREAM(pp, bis, num_bis);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}

UINT8 btsnd_hcic_ble_big_sync_terminate(uint8_t big_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("%s big_handle %d", __func__, big_handle);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_BIG_SYNC_TERMINATE_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_BIG_TERMINATE_SYNC);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_BIG_SYNC_TERMINATE_PARAMS);

    UINT8_TO_STREAM(pp, big_handle);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_ISO_BIG_SYNCER_EN == TRUE)
UINT8 btsnd_hcic_ble_iso_set_data_path(uint16_t conn_handle, uint8_t data_path_dir, uint8_t data_path_id, uint8_t coding_fmt,
                                    uint16_t company_id, uint16_t vs_codec_id, uint32_t controller_delay, uint8_t codec_len,
                                    uint8_t *codec_cfg)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set data path: conn_handle %d data_path_dir %d data_path_id %d coding_fmt %d company_id 0x%x vs_codec_id %d controller_delay %ld codec_len %d",
                    conn_handle, data_path_dir, data_path_id, coding_fmt, company_id, vs_codec_id, controller_delay, codec_len);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_SET_DATA_PATH_PARAMS + codec_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_SET_DATA_PATH);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_SET_DATA_PATH_PARAMS + codec_len);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, data_path_dir);
    UINT8_TO_STREAM(pp, data_path_id);
    UINT8_TO_STREAM(pp, coding_fmt);
    UINT16_TO_STREAM(pp, company_id);
    UINT16_TO_STREAM(pp, vs_codec_id);
    UINT24_TO_STREAM(pp, controller_delay);
    UINT8_TO_STREAM(pp, codec_len);
    if (codec_len && codec_cfg) {
        ARRAY_TO_STREAM(pp, codec_cfg, codec_len);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_iso_remove_data_path(uint16_t conn_handle, uint8_t data_path_dir)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci remove data path: conn_handle %d data_path_dir %d", conn_handle, data_path_dir);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_REMOVE_DATA_PATH_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_REMOVE_DATA_PATH);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_REMOVE_DATA_PATH_PARAMS);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, data_path_dir);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_iso_read_tx_sync(uint16_t iso_hdl)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci read iso tx sync: iso_hdl %d", iso_hdl);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_READ_TX_SYNC_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_READ_TX_SYNC);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_READ_TX_SYNC_PARAMS);
    // Bit_Number V1
    UINT16_TO_STREAM(pp, iso_hdl);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)
UINT8 btsnd_hcic_ble_iso_set_cig_params(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t worse_case_SCA, uint8_t packing,
                                        uint8_t framing, uint16_t mtl_c_to_p, uint16_t mtl_p_to_c, uint8_t cis_cnt, struct ble_hci_le_cis_params *cis_params)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set cig params: cig_id %d sdu_int_c_to_p %d sdu_int_p_to_c %d worse_case_SCA %d packing %d framing %d mtl_c_to_p %d mtl_p_to_c %d cis_cnt %d",
                    cig_id, sdu_int_c_to_p, sdu_int_p_to_c, worse_case_SCA, packing, framing, mtl_c_to_p, mtl_p_to_c, cis_cnt);
    UINT8 cis_param_len = cis_cnt * 9;
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_SET_CIG_PARAMS + cis_param_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_SET_CIG_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_SET_CIG_PARAMS + cis_param_len);

    UINT8_TO_STREAM(pp, cig_id);
    UINT24_TO_STREAM(pp, sdu_int_c_to_p);
    UINT24_TO_STREAM(pp, sdu_int_p_to_c);
    UINT8_TO_STREAM(pp, worse_case_SCA);
    UINT8_TO_STREAM(pp, packing);
    UINT8_TO_STREAM(pp, framing);
    UINT16_TO_STREAM(pp, mtl_c_to_p);
    UINT16_TO_STREAM(pp, mtl_p_to_c);
    UINT8_TO_STREAM(pp, cis_cnt);

    for (uint8_t i = 0; i < cis_cnt; i++)
    {
        UINT8_TO_STREAM(pp, cis_params[i].cis_id);
        UINT16_TO_STREAM(pp, cis_params[i].max_sdu_c_to_p);
        UINT16_TO_STREAM(pp, cis_params[i].max_sdu_p_to_c);
        UINT8_TO_STREAM(pp, cis_params[i].phy_c_to_p);
        UINT8_TO_STREAM(pp, cis_params[i].phy_p_to_c);
        UINT8_TO_STREAM(pp, cis_params[i].rtn_c_to_p);
        UINT8_TO_STREAM(pp, cis_params[i].rtn_p_to_c);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_iso_set_cig_params_test(uint8_t cig_id, uint32_t sdu_int_c_to_p, uint32_t sdu_int_p_to_c, uint8_t ft_c_to_p, uint8_t ft_p_to_c,
                                            uint16_t iso_interval, uint8_t worse_case_SCA, uint8_t packing, uint8_t framing, uint8_t cis_cnt,
                                            struct ble_hci_le_cis_params_test *cis_params_test)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set cig params test: cig_id %d sdu_int_c_to_p %d sdu_int_p_to_c %d ft_c_to_p %d ft_p_to_c %d iso_interval %d worse_case_SCA %d packing %d framing %d cis_cnt %d",
                    cig_id, sdu_int_c_to_p, sdu_int_p_to_c, ft_c_to_p, ft_p_to_c, iso_interval, worse_case_SCA, packing, framing, cis_cnt);
    UINT8 cis_param_len = cis_cnt * 14;
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_SET_CIG_TEST_PARAMS + cis_param_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_SET_CIG_PARAMS_TEST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_SET_CIG_TEST_PARAMS + cis_param_len);

    UINT8_TO_STREAM(pp, cig_id);
    UINT24_TO_STREAM(pp, sdu_int_c_to_p);
    UINT24_TO_STREAM(pp, sdu_int_p_to_c);
    UINT8_TO_STREAM(pp, ft_c_to_p);
    UINT8_TO_STREAM(pp, ft_p_to_c);
    UINT16_TO_STREAM(pp, iso_interval);
    UINT8_TO_STREAM(pp, worse_case_SCA);
    UINT8_TO_STREAM(pp, packing);
    UINT8_TO_STREAM(pp, framing);
    UINT8_TO_STREAM(pp, cis_cnt);

    for (uint8_t i = 0; i < cis_cnt; i++)
    {
        UINT8_TO_STREAM(pp, cis_params_test[i].cis_id);
        UINT8_TO_STREAM(pp, cis_params_test[i].nse);
        UINT16_TO_STREAM(pp, cis_params_test[i].max_sdu_c_to_p);
        UINT16_TO_STREAM(pp, cis_params_test[i].max_sdu_p_to_c);
        UINT16_TO_STREAM(pp, cis_params_test[i].max_pdu_c_to_p);
        UINT16_TO_STREAM(pp, cis_params_test[i].max_pdu_p_to_c);
        UINT8_TO_STREAM(pp, cis_params_test[i].phy_c_to_p);
        UINT8_TO_STREAM(pp, cis_params_test[i].phy_p_to_c);
        UINT8_TO_STREAM(pp, cis_params_test[i].bn_c_to_p);
        UINT8_TO_STREAM(pp, cis_params_test[i].bn_p_to_c);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_iso_create_cis(uint8_t cis_count, struct ble_hci_cis_hdls *cis_hdls)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci create cis: cig_id ", cis_count);
    // for (uint8_t i = 0; i < cis_count; i++)
    // {
    //     HCI_TRACE_ERROR("i %d cis_hdl %d acl_hdl %d", i, cis_hdls[i].cis_hdl, cis_hdls[i].acl_hdl);
    // }

    UINT8 cis_param_len = cis_count * 4;
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_CREATE_CIS_PARAMS + cis_param_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_CREATE_CIS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_CREATE_CIS_PARAMS + cis_param_len);

    UINT8_TO_STREAM(pp, cis_count);

    for (uint8_t i = 0; i < cis_count; i++)
    {
        UINT16_TO_STREAM(pp, cis_hdls[i].cis_hdl);
        UINT16_TO_STREAM(pp, cis_hdls[i].acl_hdl);
    }

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);

    return TRUE;
}

UINT8 btsnd_hcic_ble_iso_remove_cig(uint8_t cig_id)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci remove cig: cig_id %d", cig_id);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_REMOVE_CIG_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_REMOVE_CIG);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_REMOVE_CIG_PARAMS);

    UINT8_TO_STREAM(pp, cig_id);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_ISO_CIG_CENTRAL_EN == TRUE)

#if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)
UINT8 btsnd_hcic_ble_iso_accept_cis_req(uint16_t cis_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci accept cis req: cis_handle %d", cis_handle);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_ACCEPT_CIS_REQ_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_ACCEPT_CIS_REQ);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_ACCEPT_CIS_REQ_PARAMS);

    UINT16_TO_STREAM(pp, cis_handle);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);

    return TRUE;
}

UINT8 btsnd_hcic_ble_iso_reject_cis_req(uint16_t cis_handle, uint8_t reason)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci reject cis req: cis_handle %d reason %d", cis_handle, reason);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_REJECT_CIS_REQ_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_REJECT_CIS_REQ);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_REJECT_CIS_REQ_PARAMS);

    UINT16_TO_STREAM(pp, cis_handle);
    UINT8_TO_STREAM(pp, reason);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_ISO_CIG_PERIPHERAL_EN == TRUE)

UINT8 btsnd_hcic_ble_iso_read_iso_link_quality(uint16_t iso_handle)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci read iso link quality: cis_handle %d", iso_handle);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ISO_READ_LINK_QUALITY_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ISO_READ_ISO_LINK_QUALITY);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ISO_READ_LINK_QUALITY_PARAMS);

    UINT16_TO_STREAM(pp, iso_handle);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

#endif // #if (BLE_FEAT_ISO_EN == TRUE)

#if (BLE_FEAT_CTE_EN == TRUE)
#if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)
UINT8 btsnd_hcic_ble_set_connless_cte_trans_params(uint8_t adv_hdl, uint8_t cte_len, uint8_t cte_type,
                                                uint8_t cte_cnt, uint8_t switching_pattern_len, uint8_t *antenna_ids)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connless cte trans: adv_hdl %d cte_len %d cte_type %d cte_cnt %d", adv_hdl, cte_len, cte_type, cte_cnt);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_PARAMS + switching_pattern_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONNLESS_CTE_TRANS_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_PARAMS + switching_pattern_len);

    UINT8_TO_STREAM(pp, adv_hdl);
    UINT8_TO_STREAM(pp, cte_len);
    UINT8_TO_STREAM(pp, cte_type);
    UINT8_TO_STREAM(pp, cte_cnt);
    UINT8_TO_STREAM(pp, switching_pattern_len);
    for (uint8_t i = 0; i < switching_pattern_len; i++)
    {
        UINT8_TO_STREAM(pp, antenna_ids[i]);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_set_connless_cte_enable(uint8_t adv_hdl, uint8_t cte_en)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connect cte enable: adv_hdl %d cte_en %d", adv_hdl, cte_en);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONNLESS_CTE_TRANS_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_CONNLESS_CTE_TRANS_ENABLE);

    UINT8_TO_STREAM(pp, adv_hdl);
    UINT8_TO_STREAM(pp, cte_en);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_set_connless_iq_sampling_enable(uint16_t sync_hdl, uint8_t sampling_en, uint8_t slot_dur,
                                                uint8_t max_sampled_ctes, uint8_t switching_pattern_len, uint8_t *antenna_ids)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci enable IQ sampling: sync_hdl %d sampling_en %d slot_dur %d", sync_hdl, sampling_en, slot_dur);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_CONNLESS_IQ_SAMPLING_ENABLE + switching_pattern_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONNLESS_IQ_SAMPLING_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_CONNLESS_IQ_SAMPLING_ENABLE + switching_pattern_len);

    UINT16_TO_STREAM(pp, sync_hdl);
    UINT8_TO_STREAM(pp, sampling_en);
    UINT8_TO_STREAM(pp, slot_dur);
    UINT8_TO_STREAM(pp, max_sampled_ctes);
    UINT8_TO_STREAM(pp, switching_pattern_len);
    for (uint8_t i = 0; i < switching_pattern_len; i++)
    {
        UINT8_TO_STREAM(pp, antenna_ids[i]);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_CTE_CONNECTIONLESS_EN == TRUE)

#if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)
UINT8 btsnd_hcic_ble_set_conn_cte_receive_params(uint16_t conn_hdl, uint8_t sampling_en, uint8_t slot_dur,
                                                uint8_t switching_pattern_len, uint8_t *antenna_ids)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connection cte receive params: conn_hdl %d sampling_en %d slot_dur %d", conn_hdl, sampling_en, slot_dur);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_CONN_CTE_RECEIVE_PARAMS + switching_pattern_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONN_CTE_RECEIVE_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_CONN_CTE_RECEIVE_PARAMS + switching_pattern_len);

    UINT16_TO_STREAM(pp, conn_hdl);
    UINT8_TO_STREAM(pp, sampling_en);
    UINT8_TO_STREAM(pp, slot_dur);
    UINT8_TO_STREAM(pp, switching_pattern_len);
    for (uint8_t i = 0; i < switching_pattern_len; i++)
    {
        UINT8_TO_STREAM(pp, antenna_ids[i]);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_set_conn_cte_trans_params(uint16_t conn_hdl, uint8_t cte_type, uint8_t switching_pattern_len, uint8_t *antenna_ids)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connection cte trans params: conn_hdl %d cte_type %d len %d", conn_hdl, cte_type, switching_pattern_len);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_CONN_CTE_TRANS_PARAMS + switching_pattern_len);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONN_CTE_TRANS_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_CONN_CTE_TRANS_PARAMS + switching_pattern_len);

    UINT16_TO_STREAM(pp, conn_hdl);
    UINT8_TO_STREAM(pp, cte_type);
    UINT8_TO_STREAM(pp, switching_pattern_len);
    for (uint8_t i = 0; i < switching_pattern_len; i++)
    {
        UINT8_TO_STREAM(pp, antenna_ids[i]);
    }

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_conn_cte_req_enable(uint16_t conn_hdl, uint8_t enable, uint16_t cte_req_int, uint8_t req_cte_len, uint8_t req_cte_type)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connect cte request enable: conn_hdl %d enable %d cte_req_int %d req_cte_len %d req_cte_type %d", conn_hdl, enable, cte_req_int, req_cte_len, req_cte_type);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_CONN_CTE_REQ_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONN_CTE_REQ_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_CONN_CTE_REQ_ENABLE);

    UINT16_TO_STREAM(pp, conn_hdl);
    UINT8_TO_STREAM(pp, enable);
    UINT16_TO_STREAM(pp, cte_req_int);
    UINT8_TO_STREAM(pp, req_cte_len);
    UINT8_TO_STREAM(pp, req_cte_type);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_conn_cte_rsp_enable(uint16_t conn_hdl, uint8_t enable)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set connect cte response enable: conn_hdl %d enable %d", conn_hdl, enable);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_CONN_CTE_RSP_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_CONN_CTE_RSP_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_CONN_CTE_RSP_ENABLE);

    UINT16_TO_STREAM(pp, conn_hdl);
    UINT8_TO_STREAM(pp, enable);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_CTE_CONNECTION_EN == TRUE)

UINT8 btsnd_hcic_ble_read_antenna_info(void)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci read antenna information");

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_READ_ANT_INFO);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_READ_ANT_INFOR);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_ANT_INFO);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_CTE_EN == TRUE)

#if (BLE_FEAT_POWER_CONTROL_EN == TRUE)
UINT8 btsnd_hcic_ble_enh_read_trans_power_level(uint16_t conn_handle, uint8_t phy)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci enh read trans power level, conn_handle %d phy %d", conn_handle, phy);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_ENH_READ_TRANS_PWR_LEVEL);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_ENH_READ_TRANS_POWER_LEVEL);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_ENH_READ_TRANS_PWR_LEVEL);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, phy);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_read_remote_trans_power_level(uint16_t conn_handle, uint8_t phy)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci read remote trans power level, conn_handle %d phy %d", conn_handle, phy);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_READ_REMOTE_TRANS_PWR_LEVEL);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_READ_REMOTE_TRANS_POWER_LEVEL);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_READ_REMOTE_TRANS_PWR_LEVEL);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, phy);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);

    return TRUE;
}

UINT8 btsnd_hcic_ble_set_path_loss_rpt_params(uint16_t conn_handle, uint8_t high_threshold, uint8_t high_hysteresis,
                                            uint8_t low_threshold, uint8_t low_hysteresis, uint16_t min_time_spent)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set path loss rpt params, conn_handle %d high_threshold %d high_hysteresis %d low_threshold %d low_hysteresis %d min_time_spent %d",
                    conn_handle, high_threshold, high_hysteresis, low_threshold,
                    low_hysteresis, min_time_spent);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_PARAMS);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PATH_LOSS_REPORTING_PARAMS);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_PARAMS);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, high_threshold);
    UINT8_TO_STREAM(pp, high_hysteresis);
    UINT8_TO_STREAM(pp, low_threshold);
    UINT8_TO_STREAM(pp, low_hysteresis);
    UINT16_TO_STREAM(pp, min_time_spent);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_set_path_loss_rpt_enable(uint16_t conn_handle, uint8_t enable)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set path loss rpt en, conn_handle %d enable %d", conn_handle, enable);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_PATH_LOSS_REPORTING_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_PATH_LOSS_REPORTING_ENABLE);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, enable);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_set_trans_pwr_rpt_enable(uint16_t conn_handle, uint8_t local_enable, uint8_t remote_enable)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set trans power rpt en, conn_handle %d local_enable %d remote_enable %d", conn_handle, local_enable, remote_enable);
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_TRANS_PWR_REPORTING_ENABLE);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_TRANS_POWER_REPORTING_ENABLE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_TRANS_PWR_REPORTING_ENABLE);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT8_TO_STREAM(pp, local_enable);
    UINT8_TO_STREAM(pp, remote_enable);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_FEAT_POWER_CONTROL_EN == TRUE)

#if (BLE_FEAT_CONN_SUBRATING == TRUE)
UINT8 btsnd_hcic_ble_set_default_subrate(UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency,
                                            UINT16 continuation_number, UINT16 supervision_timeout)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set default subrate, subrate_min %d subrate_max %d max_latency %d continuation_number %d supervision_timeout %d",
                                                        subrate_min, subrate_max, max_latency, continuation_number, supervision_timeout);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_DEFAULT_SUBRATE_PARAMS_LEN);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SET_DEFAULT_SUBRATE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_DEFAULT_SUBRATE_PARAMS_LEN);

    UINT16_TO_STREAM(pp, subrate_min);
    UINT16_TO_STREAM(pp, subrate_max);
    UINT16_TO_STREAM(pp, max_latency);
    UINT16_TO_STREAM(pp, continuation_number);
    UINT16_TO_STREAM(pp, supervision_timeout);

    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}

UINT8 btsnd_hcic_ble_subrate_request(UINT16 conn_handle, UINT16 subrate_min, UINT16 subrate_max, UINT16 max_latency,
                                        UINT16 continuation_number, UINT16 supervision_timeout)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci subrate req, conn_handle %d subrate_min %d subrate_max %d max_latency %d continuation_number %d supervision_timeout %d",
                                                    conn_handle, subrate_min, subrate_max, max_latency, continuation_number, supervision_timeout);

    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SUBRATE_REQ_LENGTH_PARAMS_LEN);

    pp = (UINT8 *)(p + 1);

    UINT16_TO_STREAM(pp, HCI_BLE_SUBRATE_REQUEST);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SUBRATE_REQ_LENGTH_PARAMS_LEN);

    UINT16_TO_STREAM(pp, conn_handle);
    UINT16_TO_STREAM(pp, subrate_min);
    UINT16_TO_STREAM(pp, subrate_max);
    UINT16_TO_STREAM(pp, max_latency);
    UINT16_TO_STREAM(pp, continuation_number);
    UINT16_TO_STREAM(pp, supervision_timeout);

    btu_hcif_send_cmd(LOCAL_BR_EDR_CONTROLLER_ID, p);
    return TRUE;
}
#endif // #if (BLE_FEAT_CONN_SUBRATING == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
UINT8 btsnd_hcic_ble_set_host_feature(uint16_t bit_num, uint8_t bit_val)
{
    BT_HDR *p;
    UINT8 *pp;

    HCI_TRACE_DEBUG("hci set host feature: bit_num %d bit_val %d", bit_num, bit_val);
#if (BLE_FEAT_ISO_60_EN == TRUE)
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS_V2);
#else
    HCIC_BLE_CMD_CREATED(p, pp, HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS);
#endif // #if (BLE_FEAT_ISO_60_EN == TRUE)
    pp = (UINT8 *)(p + 1);
#if (BLE_FEAT_ISO_60_EN == TRUE)
    UINT16_TO_STREAM(pp, HCI_BLE_SET_HOST_FEATURE_V2);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS_V2);
    // Bit_Number V1
    UINT16_TO_STREAM(pp, bit_num);
#else
    UINT16_TO_STREAM(pp, HCI_BLE_SET_HOST_FEATURE);
    UINT8_TO_STREAM(pp, HCIC_PARAM_SIZE_SET_HOST_FEATURE_PARAMS);
    // Bit_Number V1
    UINT8_TO_STREAM(pp, bit_num);
#endif // #if (BLE_FEAT_ISO_60_EN == TRUE)
    UINT8_TO_STREAM(pp, bit_val);
    return btu_hcif_send_cmd_sync(LOCAL_BR_EDR_CONTROLLER_ID, p);
}
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
