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

BOOLEAN btsnd_hcic_ble_periodic_adv_create_sync(UINT8 option, UINT8 adv_sid,
                                                                       UINT8 adv_addr_type, BD_ADDR adv_addr,
                                                                       UINT16 sync_timeout, UINT8 unused)
{
    BT_HDR *p;
    UINT8 *pp;
    HCI_TRACE_EVENT("%s, option = %d, adv_sid = %d, adv_addr_type = %d, sync_timeout = %d, unused = %d",
                                   __func__, option, adv_sid, adv_addr_type, sync_timeout, unused);

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
    UINT8_TO_STREAM(pp, unused);

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
#endif
