/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>

#include "osi/allocator.h"
#include "common/bt_target.h"
#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta_pba_client_int.h"

#if BTA_PBA_CLIENT_INCLUDED

static const tBTA_SYS_REG bta_pba_client_reg = {
    bta_pba_client_hdl_event,
    BTA_PbaClientDisable
};

void BTA_PbaClientEnable(tBTA_PBA_CLIENT_CBACK *p_cback)
{
    tBTA_PBA_CLIENT_API_ENABLE *p_buf;

    if (bta_sys_is_register(BTA_ID_PBC)) {
        APPL_TRACE_ERROR("BTA PBA Client already enabled");
        return;
    }

    /* register with BTA system manager */
    bta_sys_register(BTA_ID_PBC, &bta_pba_client_reg);

    if ((p_buf = (tBTA_PBA_CLIENT_API_ENABLE *)osi_malloc(sizeof(tBTA_PBA_CLIENT_API_ENABLE))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_ENABLE_EVT;
        p_buf->p_cback = p_cback;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientDisable(void)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_PBA_CLIENT_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientRegister(const char *server_name)
{
    tBTA_PBA_CLIENT_API_REGISTER *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_REGISTER *) osi_malloc(sizeof(tBTA_PBA_CLIENT_API_REGISTER))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_REGISTER_EVT;
        memcpy(p_buf->name, server_name, strlen(server_name) + 1);
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientDeregister(void)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_PBA_CLIENT_API_DEREGISTER_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientOpen(BD_ADDR bd_addr, tBTA_SEC sec_mask, UINT32 supported_feat, UINT16 mtu)
{
    tBTA_PBA_CLIENT_API_OPEN *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_OPEN *)osi_malloc(sizeof(tBTA_PBA_CLIENT_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_OPEN_EVT;
        p_buf->sec_mask = sec_mask;
        p_buf->supported_feat = supported_feat;
        p_buf->mtu = mtu;
        bdcpy(p_buf->bd_addr, bd_addr);
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientClose(UINT16 handle)
{
    BT_HDR *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_PBA_CLIENT_API_CLOSE_EVT;
        p_buf->layer_specific = handle;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientPullPhoneBook(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len)
{
    tBTA_PBA_CLIENT_API_REQ *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_REQ *) osi_malloc(sizeof(tBTA_PBA_CLIENT_API_REQ))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_REQ_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->operation = BTA_PBA_CLIENT_OP_PULL_PHONE_BOOK;
        p_buf->name = name;
        p_buf->app_param = app_param;
        p_buf->app_param_len = app_param_len;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientSetPhoneBook(UINT16 handle, UINT8 flags, char *name)
{
    tBTA_PBA_CLIENT_API_REQ *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_REQ *) osi_malloc(sizeof(tBTA_PBA_CLIENT_API_REQ))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_REQ_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->operation = BTA_PBA_CLIENT_OP_SET_PHONE_BOOK;
        p_buf->flags = flags;
        p_buf->name = name;
        p_buf->app_param = NULL;
        p_buf->app_param_len = 0;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientPullvCardListing(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len)
{
    tBTA_PBA_CLIENT_API_REQ *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_REQ *) osi_malloc(sizeof(tBTA_PBA_CLIENT_API_REQ))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_REQ_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->operation = BTA_PBA_CLIENT_OP_PULL_VCARD_LISTING;
        p_buf->name = name;
        p_buf->app_param = app_param;
        p_buf->app_param_len = app_param_len;
        bta_sys_sendmsg(p_buf);
    }
}

void BTA_PbaClientPullvCardEntry(UINT16 handle, char *name, UINT8 *app_param, UINT16 app_param_len)
{
    tBTA_PBA_CLIENT_API_REQ *p_buf;

    if ((p_buf = (tBTA_PBA_CLIENT_API_REQ *) osi_malloc(sizeof(tBTA_PBA_CLIENT_API_REQ))) != NULL) {
        p_buf->hdr.event = BTA_PBA_CLIENT_API_REQ_EVT;
        p_buf->hdr.layer_specific = handle;
        p_buf->operation = BTA_PBA_CLIENT_OP_PULL_VCARD_ENTRY;
        p_buf->name = name;
        p_buf->app_param = app_param;
        p_buf->app_param_len = app_param_len;
        bta_sys_sendmsg(p_buf);
    }
}

#endif
