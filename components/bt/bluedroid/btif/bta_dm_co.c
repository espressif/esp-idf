/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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
#include <stdio.h>
#include <stdlib.h>

#include "bta_api.h"
#include "bta_sys.h"
#include "bta_dm_co.h"
#include "bta_dm_ci.h"
#include "btc_ble_storage.h"
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
#include "bt_utils.h"
#if (BTM_OOB_INCLUDED == TRUE)
#include "btif_dm.h"
#endif
#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
#include "bte_appl.h"
tBTE_APPL_CFG bte_appl_cfg = {
#if SMP_INCLUDED == TRUE
    BTA_LE_AUTH_REQ_SC_MITM_BOND, // Authentication requirements
#else
    BTM_AUTH_SPGB_YES,            // Authentication requirements
#endif
    BTM_LOCAL_IO_CAPS_BLE,
    BTM_BLE_INITIATOR_KEY_SIZE,
    BTM_BLE_RESPONDER_KEY_SIZE,
    BTM_BLE_MAX_KEY_SIZE
};
#endif

/*******************************************************************************
**
** Function         bta_dm_co_get_compress_memory
**
** Description      This callout function is executed by DM to get memory for compression

** Parameters       id  -  BTA SYS ID
**                  memory_p - memory return by callout
**                  memory_size - memory size
**
** Returns          TRUE for success, FALSE for fail.
**
*******************************************************************************/
BOOLEAN bta_dm_co_get_compress_memory(tBTA_SYS_ID id, UINT8 **memory_p, UINT32 *memory_size)
{
    UNUSED(id);
    UNUSED(memory_p);
    UNUSED(memory_size);
    return TRUE;
}

/*******************************************************************************
**
** Function         bta_dm_co_io_req
**
** Description      This callout function is executed by DM to get IO capabilities
**                  of the local device for the Simple Pairing process
**
** Parameters       bd_addr  - The peer device
**                  *p_io_cap - The local Input/Output capabilities
**                  *p_oob_data - TRUE, if OOB data is available for the peer device.
**                  *p_auth_req - TRUE, if MITM protection is required.
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_io_req(BD_ADDR bd_addr, tBTA_IO_CAP *p_io_cap, tBTA_OOB_DATA *p_oob_data,
                      tBTA_AUTH_REQ *p_auth_req, BOOLEAN is_orig)
{
    UNUSED(bd_addr);
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
#if (BTM_OOB_INCLUDED == TRUE)
    btif_dm_set_oob_for_io_req(p_oob_data);
#endif
    btif_dm_proc_io_req(bd_addr, p_io_cap, p_oob_data, p_auth_req, is_orig);
#else
    BTIF_TRACE_DEBUG("bta_dm_co_io_req: func not ported\n");
#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
    BTIF_TRACE_DEBUG("bta_dm_co_io_req *p_oob_data = %d", *p_oob_data);
    BTIF_TRACE_DEBUG("bta_dm_co_io_req *p_io_cap = %d", *p_io_cap);
    BTIF_TRACE_DEBUG("bta_dm_co_io_req *p_auth_req = %d", *p_auth_req);
    BTIF_TRACE_DEBUG("bta_dm_co_io_req is_orig = %d", is_orig);
}

/*******************************************************************************
**
** Function         bta_dm_co_io_rsp
**
** Description      This callout function is executed by DM to report IO capabilities
**                  of the peer device for the Simple Pairing process
**
** Parameters       bd_addr  - The peer device
**                  io_cap - The remote Input/Output capabilities
**                  oob_data - TRUE, if OOB data is available for the peer device.
**                  auth_req - TRUE, if MITM protection is required.
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_io_rsp(BD_ADDR bd_addr, tBTA_IO_CAP io_cap,
                      tBTA_OOB_DATA oob_data, tBTA_AUTH_REQ auth_req)
{
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
    btif_dm_proc_io_rsp(bd_addr, io_cap, oob_data, auth_req);
#else
    BTIF_TRACE_DEBUG("bta_dm_co_io_rsp: func not ported\n");
#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
}

/*******************************************************************************
**
** Function         bta_dm_co_lk_upgrade
**
** Description      This callout function is executed by DM to check if the
**                  platform wants allow link key upgrade
**
** Parameters       bd_addr  - The peer device
**                  *p_upgrade - TRUE, if link key upgrade is desired.
**
** Returns          void.
**
*******************************************************************************/
void  bta_dm_co_lk_upgrade(BD_ADDR bd_addr, BOOLEAN *p_upgrade )
{
    UNUSED(bd_addr);
    UNUSED(p_upgrade);
}

#if (BTM_OOB_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_dm_co_loc_oob
**
** Description      This callout function is executed by DM to report the OOB
**                  data of the local device for the Simple Pairing process
**
** Parameters       valid - TRUE, if the local OOB data is retrieved from LM
**                  c     - Simple Pairing Hash C
**                  r     - Simple Pairing Randomnizer R
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_loc_oob(BOOLEAN valid, BT_OCTET16 c, BT_OCTET16 r)
{
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
    BTIF_TRACE_DEBUG("bta_dm_co_loc_oob, valid = %d", valid);
#ifdef BTIF_DM_OOB_TEST
    btif_dm_proc_loc_oob(valid, c, r);
#endif
#else
    BTIF_TRACE_DEBUG("bta_dm_co_loc_oob: func not ported\n");
#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
}

/*******************************************************************************
**
** Function         bta_dm_co_rmt_oob
**
** Description      This callout function is executed by DM to request the OOB
**                  data for the remote device for the Simple Pairing process
**                  Need to call bta_dm_ci_rmt_oob() in response
**
** Parameters       bd_addr  - The peer device
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_rmt_oob(BD_ADDR bd_addr)
{
    BT_OCTET16 p_c;
    BT_OCTET16 p_r;
    BOOLEAN result = FALSE;

#ifdef BTIF_DM_OOB_TEST
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
    result = btif_dm_proc_rmt_oob(bd_addr, p_c, p_r);
#else
    BTIF_TRACE_DEBUG("bta_dm_rmt_oob: func not ported\n");
#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
#endif

    BTIF_TRACE_DEBUG("bta_dm_co_rmt_oob: result=%d", result);
    bta_dm_ci_rmt_oob(result, bd_addr, p_c, p_r);
}

#endif /* BTM_OOB_INCLUDED */


// REMOVE FOR BLUEDROID ?

#if (BTM_SCO_HCI_INCLUDED == TRUE ) && (BTM_SCO_INCLUDED == TRUE)
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         btui_sco_codec_callback
**
** Description      Callback for btui codec.
**
**
** Returns          void
**
*******************************************************************************/
static void btui_sco_codec_callback(UINT16 event, UINT16 sco_handle)
{
    bta_dm_sco_ci_data_ready(event, sco_handle);
}
/*******************************************************************************
**
** Function         bta_dm_sco_co_init
**
** Description      This function can be used by the phone to initialize audio
**                  codec or for other initialization purposes before SCO connection
**                  is opened.
**
**
** Returns          tBTA_DM_SCO_ROUTE_TYPE: SCO routing configuration type.
**
*******************************************************************************/
tBTA_DM_SCO_ROUTE_TYPE bta_dm_sco_co_init(UINT32 rx_bw, UINT32 tx_bw,
        tBTA_CODEC_INFO *p_codec_type, UINT8 app_id)
{
    tBTM_SCO_ROUTE_TYPE route = BTA_DM_SCO_ROUTE_PCM;

    BTIF_TRACE_DEBUG("bta_dm_sco_co_init");

    /* set up SCO routing configuration if SCO over HCI app ID is used and run time
        configuration is set to SCO over HCI */
    /* HS invoke this call-out */
    if (
#if (BTA_HS_INCLUDED == TRUE ) && (BTA_HS_INCLUDED == TRUE)
        (app_id == BTUI_DM_SCO_4_HS_APP_ID && btui_cfg.hs_sco_over_hci) ||
#endif
        /* AG invoke this call-out */
        (app_id != BTUI_DM_SCO_4_HS_APP_ID && btui_cfg.ag_sco_over_hci )) {
        route = btui_cb.sco_hci = BTA_DM_SCO_ROUTE_HCI;
    }
    /* no codec is is used for the SCO data */
    if (p_codec_type->codec_type == BTA_SCO_CODEC_PCM && route == BTA_DM_SCO_ROUTE_HCI) {
        /* initialize SCO codec */
        if (!btui_sco_codec_init(rx_bw, tx_bw)) {
            BTIF_TRACE_ERROR("codec initialization exception!");
        }
    }

    return route;
}



/*******************************************************************************
**
** Function         bta_dm_sco_co_open
**
** Description      This function is executed when a SCO connection is open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_sco_co_open(UINT16 handle, UINT8 pkt_size, UINT16 event)
{
    tBTUI_SCO_CODEC_CFG cfg;

    if (btui_cb.sco_hci) {
        BTIF_TRACE_DEBUG("bta_dm_sco_co_open handle:%d pkt_size:%d", handle, pkt_size);
        /* use dedicated SCO buffer pool for SCO TX data */
        cfg.pool_id = HCI_SCO_POOL_ID;
        cfg.p_cback = btui_sco_codec_callback;
        cfg.pkt_size = pkt_size;
        cfg.cb_event = event;
        /* open and start the codec */
        btui_sco_codec_open(&cfg);
        btui_sco_codec_start(handle);
    }
}

/*******************************************************************************
**
** Function         bta_dm_sco_co_close
**
** Description      This function is called when a SCO connection is closed
**
**
** Returns          void
**
*******************************************************************************/
void bta_dm_sco_co_close(void)
{
    if (btui_cb.sco_hci) {
        BTIF_TRACE_DEBUG("bta_dm_sco_co_close close codec");
        /* close sco codec */
        btui_sco_codec_close();

        btui_cb.sco_hci = FALSE;
    }
}

/*******************************************************************************
**
** Function         bta_dm_sco_co_in_data
**
** Description      This function is called to send incoming SCO data to application.
**
** Returns          void
**
*******************************************************************************/
void bta_dm_sco_co_in_data(BT_HDR  *p_buf)
{
    if (btui_cfg.sco_use_mic) {
        btui_sco_codec_inqdata (p_buf);
    } else {
        GKI_freebuf(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_dm_sco_co_out_data
**
** Description      This function is called to send SCO data over HCI.
**
** Returns          void
**
*******************************************************************************/
void bta_dm_sco_co_out_data(BT_HDR  **p_buf)
{
    btui_sco_codec_readbuf(p_buf);
}

#endif /* #if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE) */
#endif /* #if (BTM_SCO_HCI_INCLUDED == TRUE ) && (BTM_SCO_INCLUDED == TRUE)*/


#if (defined BLE_INCLUDED && BLE_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_dm_co_le_io_key_req
**
** Description      This callout function is executed by DM to get BLE key information
**                  before SMP pairing gets going.
**
** Parameters       bd_addr  - The peer device
**                  *p_max_key_size - max key size local device supported.
**                  *p_init_key - initiator keys.
**                  *p_resp_key - responder keys.
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_le_io_key_req(BD_ADDR bd_addr, UINT8 *p_max_key_size,
                             tBTA_LE_KEY_TYPE *p_init_key,
                             tBTA_LE_KEY_TYPE  *p_resp_key )
{
    UNUSED(bd_addr);
#if (SMP_INCLUDED == TRUE)
    BTIF_TRACE_DEBUG("##################################");
    BTIF_TRACE_DEBUG("bta_dm_co_le_io_key_req: only setting max size to 16");
    BTIF_TRACE_DEBUG("##################################");
    *p_max_key_size = 16;
    *p_init_key = *p_resp_key =
                      (BTA_LE_KEY_PENC | BTA_LE_KEY_PID | BTA_LE_KEY_PCSRK | BTA_LE_KEY_LENC | BTA_LE_KEY_LID | BTA_LE_KEY_LCSRK);
#endif  ///SMP_INCLUDED == TRUE
}


/*******************************************************************************
**
** Function         bta_dm_co_ble_local_key_reload
**
** Description      This callout function is to load the local BLE keys if available
**                  on the device.
**
** Parameters       none
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_ble_load_local_keys(tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
                                   tBTA_BLE_LOCAL_ID_KEYS *p_id_keys)
{
#if (defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE)
    BTIF_TRACE_DEBUG("##################################");
    BTIF_TRACE_DEBUG("bta_dm_co_ble_load_local_keys:  Load local keys if any are persisted");
    BTIF_TRACE_DEBUG("##################################");
    btif_dm_get_ble_local_keys( p_key_mask, er, p_id_keys);
#endif  ///defined(BTIF_INCLUDED) && BTIF_INCLUDED == TRUE
#if (SMP_INCLUDED == TRUE)
    btc_dm_get_ble_local_keys( p_key_mask, er, p_id_keys);
#endif  ///SMP_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         bta_dm_co_ble_io_req
**
** Description      This callout function is executed by DM to get BLE IO capabilities
**                  before SMP pairing gets going.
**
** Parameters       bd_addr  - The peer device
**                  *p_io_cap - The local Input/Output capabilities
**                  *p_oob_data - TRUE, if OOB data is available for the peer device.
**                  *p_auth_req -  Auth request setting (Bonding and MITM required or not)
**                  *p_max_key_size - max key size local device supported.
**                  *p_init_key - initiator keys.
**                  *p_resp_key - responder keys.
**
** Returns          void.
**
*******************************************************************************/
void bta_dm_co_ble_io_req(BD_ADDR bd_addr,  tBTA_IO_CAP *p_io_cap,
                          tBTA_OOB_DATA *p_oob_data,
                          tBTA_LE_AUTH_REQ *p_auth_req,
                          UINT8 *p_max_key_size,
                          tBTA_LE_KEY_TYPE *p_init_key,
                          tBTA_LE_KEY_TYPE  *p_resp_key )
{
#if (SMP_INCLUDED == TRUE)
    UNUSED(bd_addr);
    /* if OOB is not supported, this call-out function does not need to do anything
     * otherwise, look for the OOB data associated with the address and set *p_oob_data accordingly
     * If the answer can not be obtained right away,
     * set *p_oob_data to BTA_OOB_UNKNOWN and call bta_dm_ci_io_req() when the answer is available */

    *p_oob_data = FALSE;

    /* *p_auth_req by default is FALSE for devices with NoInputNoOutput; TRUE for other devices. */

    if (bte_appl_cfg.ble_auth_req) {
        *p_auth_req = bte_appl_cfg.ble_auth_req | (bte_appl_cfg.ble_auth_req & 0x04) | ((*p_auth_req) & 0x04);
    }

    if (bte_appl_cfg.ble_io_cap <= 4) {
        *p_io_cap = bte_appl_cfg.ble_io_cap;
    }

    if (bte_appl_cfg.ble_init_key <= BTM_BLE_INITIATOR_KEY_SIZE) {
        *p_init_key = bte_appl_cfg.ble_init_key;
    }

    if (bte_appl_cfg.ble_resp_key <= BTM_BLE_RESPONDER_KEY_SIZE) {
        *p_resp_key = bte_appl_cfg.ble_resp_key;
    }

    if (bte_appl_cfg.ble_max_key_size > 7 && bte_appl_cfg.ble_max_key_size <= 16) {
        *p_max_key_size = bte_appl_cfg.ble_max_key_size;
    }
#endif  ///SMP_INCLUDED == TRUE
}

void bta_dm_co_ble_set_io_cap(UINT8   ble_io_cap)
{
#if (SMP_INCLUDED == TRUE)
    if(ble_io_cap < BTM_IO_CAP_MAX ) {
        bte_appl_cfg.ble_io_cap = ble_io_cap;
    } else {
        APPL_TRACE_ERROR("%s error:Invalid io cap value.",__func__);
    }
#endif  ///SMP_INCLUDED == TRUE
}

void bta_dm_co_ble_set_auth_req(UINT8   ble_auth_req)
{
#if (SMP_INCLUDED == TRUE)
    bte_appl_cfg.ble_auth_req = ble_auth_req;
#endif  ///SMP_INCLUDED == TRUE
}

void bta_dm_co_ble_set_init_key_req(UINT8 init_key)
{
#if (SMP_INCLUDED == TRUE)
   init_key &= 0x0f;  // 4~7bit reservd, only used the 0~3bit
   bte_appl_cfg.ble_init_key &= init_key;
#endif  ///SMP_INCLUDED == TRUE
}

void bta_dm_co_ble_set_rsp_key_req(UINT8 rsp_key)
{
#if (SMP_INCLUDED == TRUE)
   rsp_key &= 0x0f;  // 4~7bit reservd, only used the 0~3bit
   bte_appl_cfg.ble_init_key &= rsp_key;
#endif  ///SMP_INCLUDED == TRUE
}

void bta_dm_co_ble_set_max_key_size(UINT8 ble_key_size)
{
#if (SMP_INCLUDED == TRUE)
    if(ble_key_size >= BTM_BLE_MIN_KEY_SIZE && ble_key_size <= BTM_BLE_MAX_KEY_SIZE) {
        bte_appl_cfg.ble_max_key_size = ble_key_size;
    } else {
        APPL_TRACE_ERROR("%s error:Invalid key size value, key_size =%d",__func__, ble_key_size);
    }
#endif  ///SMP_INCLUDED == TRUE
}
#endif

