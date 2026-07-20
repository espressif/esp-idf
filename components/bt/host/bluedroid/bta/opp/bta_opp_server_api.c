/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "osi/allocator.h"
#include "common/bt_target.h"
#include "bta/bta_sys.h"
#include "bta_opp_int.h"

#if BTA_OPP_SERVER_INCLUDED

static const tBTA_SYS_REG bta_opp_server_reg = {
    bta_opp_server_hdl_event,
    BTA_OppServerDisable
};

void BTA_OppServerEnable(tBTA_OPP_CBACK *p_cback)
{
    tBTA_OPP_SERVER_API_ENABLE *p_buf;

    if (!bta_sys_is_register(BTA_ID_OPS)) {
        bta_sys_register(BTA_ID_OPS, &bta_opp_server_reg);
    }

    if ((p_buf = (tBTA_OPP_SERVER_API_ENABLE *)osi_malloc(sizeof(tBTA_OPP_SERVER_API_ENABLE))) != NULL) {
        p_buf->hdr.event = BTA_OPP_SERVER_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppServerDisable(void)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

bt_status_t BTA_OppStartServer(tBTA_OPP_SERVER_CFG *cfg)
{
    tBTA_OPP_SERVER_API_START *p_buf;

    if ((p_buf = (tBTA_OPP_SERVER_API_START *)osi_malloc(sizeof(tBTA_OPP_SERVER_API_START))) != NULL) {
        p_buf->hdr.event = BTA_OPP_SERVER_API_START_EVT;
        memcpy(&p_buf->cfg, cfg, sizeof(tBTA_OPP_SERVER_CFG));
        bta_sys_sendmsg(p_buf);
    } else {
        return BT_STATUS_NOMEM;
    }

    return BT_STATUS_SUCCESS;
}

void BTA_OppStopServer(void)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_STOP_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppAccept(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_ACCEPT_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppReject(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_REJECT_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppServerCancel(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_CANCEL_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppServerRxReady(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_SERVER_API_RX_READY_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

#endif /* BTA_OPP_SERVER_INCLUDED */
