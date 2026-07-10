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

/******************************************************************************
 *
 *  Filename:      bta_pan_co.c
 *
 *  Description:   PAN stack callout api for ESP-IDF
 *
 ******************************************************************************/
#include "common/bt_target.h"

#if (defined BTC_PAN_INCLUDED && BTC_PAN_INCLUDED == TRUE)

#include <string.h>

#include "bt_common.h"
#include "osi/allocator.h"
#include "bta/bta_pan_co.h"
#include "bta/bta_api.h"
#include "bta/bta_pan_api.h"
#include "bta/bta_pan_ci.h"
#include "btc_pan.h"

/*******************************************************************************
 *
 * Function         bta_pan_co_init
 *
 * Description      Initialize PAN data path configuration.
 *
 * Returns          Data flow mask.
 *
 ******************************************************************************/
UINT8 bta_pan_co_init(UINT8 *q_level)
{
    APPL_TRACE_API("%s", __func__);

    *q_level = 30;
    return (BTA_PAN_RX_PUSH_BUF | BTA_PAN_RX_PUSH | BTA_PAN_TX_PULL);
}

/******************************************************************************
 *
 * Function         bta_pan_co_open
 *
 * Description      PAN connection opened.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_open(UINT16 handle, UINT8 app_id, tBTA_PAN_ROLE local_role,
                     tBTA_PAN_ROLE peer_role, BD_ADDR peer_addr)
{
    APPL_TRACE_API("%s: handle:%d app_id:%d local_role:%d peer_role:%d",
                   __func__, handle, app_id, local_role, peer_role);
    (void)peer_addr;
    bta_pan_ci_rx_ready(handle);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_close
 *
 * Description      PAN connection closed.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_close(UINT16 handle, UINT8 app_id)
{
    APPL_TRACE_API("%s: handle:%d app_id:%d", __func__, handle, app_id);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_path
 *
 * Description      Deliver received Ethernet frames to application.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_tx_path(UINT16 handle, UINT8 app_id)
{
    BT_HDR *p_buf;
    BD_ADDR src;
    BD_ADDR dst;
    UINT16 protocol;
    BOOLEAN ext;
    BOOLEAN forward;

    APPL_TRACE_API("%s: handle:%d app_id:%d", __func__, handle, app_id);

    do {
        p_buf = bta_pan_ci_readbuf(handle, src, dst, &protocol, &ext, &forward);
        if (p_buf) {
            btc_pan_co_data_ind(handle, src, dst, protocol,
                                (UINT8 *)(p_buf + 1) + p_buf->offset, p_buf->len,
                                ext, forward);
            osi_free(p_buf);
        }
    } while (p_buf != NULL);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_rx_path
 *
 * Description      Not used in push RX mode.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_rx_path(UINT16 handle, UINT8 app_id)
{
    APPL_TRACE_API("%s: handle:%d app_id:%d", __func__, handle, app_id);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_write
 *
 * Description      Not used in pull TX mode.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_tx_write(UINT16 handle, UINT8 app_id, BD_ADDR src, BD_ADDR dst,
                         UINT16 protocol, UINT8 *p_data, UINT16 len, BOOLEAN ext,
                         BOOLEAN forward)
{
    APPL_TRACE_API("%s: handle:%d len:%d", __func__, handle, len);
    (void)app_id;
    (void)src;
    (void)dst;
    (void)protocol;
    (void)p_data;
    (void)ext;
    (void)forward;
}

/*******************************************************************************
 *
 * Function         bta_pan_co_tx_writebuf
 *
 * Description      Zero-copy TX push path. Not used while TX is configured
 *                  for pull mode (BTA_PAN_TX_PULL). Caller transfers buffer
 *                  ownership; free it even if the data is unused.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_tx_writebuf(UINT16 handle, UINT8 app_id, BD_ADDR src, BD_ADDR dst,
                            UINT16 protocol, BT_HDR *p_buf, BOOLEAN ext,
                            BOOLEAN forward)
{
    APPL_TRACE_API("%s: handle:%d", __func__, handle);
    (void)app_id;
    (void)src;
    (void)dst;
    (void)protocol;
    (void)ext;
    (void)forward;
    osi_free(p_buf);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_rx_flow
 *
 * Description      Notify application of TX flow control state.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_rx_flow(UINT16 handle, UINT8 app_id, BOOLEAN enable)
{
    APPL_TRACE_API("%s: handle:%d enable:%d", __func__, handle, enable);
    (void)app_id;
    btc_pan_co_rx_flow(handle, enable);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_pfilt_ind
 *
 * Description      Protocol filter indication from peer device.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_pfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                          UINT16 len, UINT8 *p_filters)
{
    APPL_TRACE_API("%s: handle:%d indication:%d len:%d", __func__, handle, indication, len);
    btc_pan_co_pfilt_ind(handle, indication, result, len, p_filters);
}

/*******************************************************************************
 *
 * Function         bta_pan_co_mfilt_ind
 *
 * Description      Multicast filter indication from peer device.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_pan_co_mfilt_ind(UINT16 handle, BOOLEAN indication, tBTA_PAN_STATUS result,
                          UINT16 len, UINT8 *p_filters)
{
    APPL_TRACE_API("%s: handle:%d indication:%d len:%d", __func__, handle, indication, len);
    btc_pan_co_mfilt_ind(handle, indication, result, len, p_filters);
}

#endif /* BTC_PAN_INCLUDED */
