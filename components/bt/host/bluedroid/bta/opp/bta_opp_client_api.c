/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_target.h"
#include "bta/bta_sys.h"
#include "bta_opp_int.h"

#if BTA_OPP_CLIENT_INCLUDED

static const tBTA_SYS_REG bta_opp_client_reg = {
    bta_opp_client_hdl_event,
    BTA_OppClientDisable
};

void BTA_OppClientEnable(tBTA_OPP_CBACK *p_cback)
{
    tBTA_OPP_CLIENT_API_ENABLE *p_buf;

    if (!bta_sys_is_register(BTA_ID_OPC)) {
        bta_sys_register(BTA_ID_OPC, &bta_opp_client_reg);
    }

    if ((p_buf = (tBTA_OPP_CLIENT_API_ENABLE *)osi_malloc(sizeof(tBTA_OPP_CLIENT_API_ENABLE))) != NULL) {
        p_buf->hdr.event = BTA_OPP_CLIENT_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppClientDisable(void)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_CLIENT_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppClientConnect(tBTA_OPP_CONNECT_PARAM *param)
{
    tBTA_OPP_CLIENT_API_CONNECT *p_buf;

    if ((p_buf = (tBTA_OPP_CLIENT_API_CONNECT *)osi_malloc(sizeof(tBTA_OPP_CLIENT_API_CONNECT))) != NULL) {
        p_buf->hdr.event = BTA_OPP_CLIENT_API_CONNECT_EVT;
        memcpy(&p_buf->param, param, sizeof(tBTA_OPP_CONNECT_PARAM));
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppClientDisconnect(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_CLIENT_API_DISCONNECT_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

bt_status_t BTA_OppOpenObject(tBTA_OPP_SEND_PARAM *param)
{
    tBTA_OPP_CLIENT_API_SEND_OBJECT *p_buf;

    if ((p_buf = (tBTA_OPP_CLIENT_API_SEND_OBJECT *)osi_malloc(sizeof(tBTA_OPP_CLIENT_API_SEND_OBJECT))) != NULL) {
        p_buf->hdr.event = BTA_OPP_CLIENT_API_OPEN_OBJECT_EVT;
        memcpy(&p_buf->param, param, sizeof(tBTA_OPP_SEND_PARAM));
        bta_sys_sendmsg(p_buf);
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_NOMEM;
}

void BTA_OppClientTxReady(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_CLIENT_API_TX_READY_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_OppClientCancel(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_OPP_CLIENT_API_CANCEL_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

#endif /* BTA_OPP_CLIENT_INCLUDED */
