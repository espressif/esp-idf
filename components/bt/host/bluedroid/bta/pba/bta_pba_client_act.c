/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/allocator.h"
#include "common/bt_target.h"
#include "stack/bt_types.h"
#include "stack/obex_api.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "bta_pba_client_int.h"

#if BTA_PBA_CLIENT_INCLUDED

static const UINT8 pbap_target_uuid[16] = {0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8, 0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66};
static const char *type_pull_phone_book = "x-bt/phonebook";
static const char *type_pull_vcard_listing = "x-bt/vcard-listing";
static const char *type_pull_vcard_entry = "x-bt/vcard";

#define TYPE_LEN_PULL_PHONE_BOOK            15
#define TYPE_LEN_PULL_VCARD_LISTING         19
#define TYPE_LEN_PULL_VCARD_ENTRY           11

static void free_ccb(tBTA_PBA_CLIENT_CCB *p_ccb)
{
    /* free sdp db */
    bta_pba_client_free_db(p_ccb);

    /* clear all field, set allocated to 0 */
    memset(p_ccb, 0, sizeof(tBTA_PBA_CLIENT_CCB));
}

static void close_goepc_and_report(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_ERR reason)
{
    /* remove goep connection */
    if (p_ccb->goep_handle != 0) {
        GOEPC_Close(p_ccb->goep_handle);
        p_ccb->goep_handle = 0;
    }

    /* report connection closed event */
    tBTA_PBA_CLIENT_CONN conn;
    conn.handle = p_ccb->allocated;
    conn.error = reason;
    bdcpy(conn.bd_addr, p_ccb->bd_addr);
    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_CLOSE_EVT, (tBTA_PBA_CLIENT *)&conn);

    /* free ccb */
    free_ccb(p_ccb);
}

static void build_and_send_empty_get_req(tBTA_PBA_CLIENT_CCB *p_ccb)
{
    tOBEX_PARSE_INFO info = {0};
    info.opcode = OBEX_OPCODE_GET_FINAL;
    /* empty get request, try to use a smaller buff size */
    UINT16 tx_buff_size = BT_SMALL_BUFFER_SIZE < p_ccb->max_tx ? BT_SMALL_BUFFER_SIZE : p_ccb->max_tx;
    UINT16 ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, tx_buff_size);
    ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_ccb->goep_cid), 4);
    ret |= GOEPC_SendRequest(p_ccb->goep_handle);
    if (ret != GOEP_SUCCESS) {
        close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_GOEP_ERROR);
    }
}

static uint8_t get_operation_response_event(tBTA_PBA_CLIENT_OP operation)
{
    uint8_t event = 0;
    switch (operation)
    {
    case BTA_PBA_CLIENT_OP_PULL_PHONE_BOOK:
        event = BTA_PBA_CLIENT_PULL_PHONE_BOOK_RSP_EVT;
        break;
    case BTA_PBA_CLIENT_OP_SET_PHONE_BOOK:
        event = BTA_PBA_CLIENT_SET_PHONE_BOOK_RSP_EVT;
        break;
    case BTA_PBA_CLIENT_OP_PULL_VCARD_LISTING:
        event = BTA_PBA_CLIENT_PULL_VCARD_LISTING_RSP_EVT;
        break;
    case BTA_PBA_CLIENT_OP_PULL_VCARD_ENTRY:
        event = BTA_PBA_CLIENT_PULL_VCARD_ENTRY_RSP_EVT;
        break;
    default:
        assert(0);
        break;
    }
    return event;
}

static tBTA_PBA_CLIENT_ERR calculate_response_error(uint8_t response_code)
{
    tBTA_PBA_CLIENT_ERR error = BTA_PBA_CLIENT_FAIL;
    /* always treat error response code as final response */
    response_code |= OBEX_FINAL_BIT_MASK;
    switch (response_code)
    {
    case (OBEX_RESPONSE_CODE_BAD_REQUEST | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_BAD_REQUEST;
        break;
    case (OBEX_RESPONSE_CODE_UNAUTHORIZED | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_UNAUTHORIZED;
        break;
    case (OBEX_RESPONSE_CODE_FORBIDDEN | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_FORBIDDEN;
        break;
    case (OBEX_RESPONSE_CODE_NOT_FOUND | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_NOT_FOUND;
        break;
    case (OBEX_RESPONSE_CODE_NOT_ACCEPTABLE | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_NOT_ACCEPTABLE;
        break;
    case (OBEX_RESPONSE_CODE_PRECONDITION_FAILED | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_PRECONDITION_FAILED;
        break;
    case (OBEX_RESPONSE_CODE_NOT_IMPLEMENTED | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_NOT_IMPLEMENTED;
        break;
    case (OBEX_RESPONSE_CODE_SERVICE_UNAVAILABLE | OBEX_FINAL_BIT_MASK):
        error = BTA_PBA_CLIENT_SERVICE_UNAVAILABLE;
        break;
    default:
        break;
    }
    return error;
}

static void report_data_event(tBTA_PBA_CLIENT_CCB *p_ccb, UINT8 *data, UINT16 data_len, UINT8 *app_param,
                              UINT16 app_param_len, BOOLEAN final, BT_HDR *pkt)
{
    uint8_t event = get_operation_response_event(p_ccb->operation);
    tBTA_PBA_CLIENT_RESPONSE response;
    response.status = BTA_PBA_CLIENT_NO_ERROR;
    response.handle = p_ccb->allocated;
    response.final = final;
    response.data_len = data_len;
    response.data = data;
    response.app_param_len = app_param_len;
    response.app_param = app_param;
    response.pkt = pkt;
    bta_pba_client_cb.p_cback(event, (tBTA_PBA_CLIENT *)&response);
}

static void report_error_data_event(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_ERR status)
{
    uint8_t event = get_operation_response_event(p_ccb->operation);
    tBTA_PBA_CLIENT_RESPONSE response;
    memset(&response, 0, sizeof(tBTA_PBA_CLIENT_RESPONSE));
    response.status = status;
    response.handle = p_ccb->allocated;
    response.final = TRUE;
    bta_pba_client_cb.p_cback(event, (tBTA_PBA_CLIENT *)&response);
}

static void ascii_to_utf16(const UINT8 *src, UINT16 len, UINT8 *dst)
{
    UINT16 pos = 0;
    for (int i = 0 ; i < len ; i++){
        dst[pos++] = 0;
        dst[pos++] = src[i];
    }
}

void bta_pba_client_api_enable(tBTA_PBA_CLIENT_DATA *p_data)
{
    /* initialize control block */
    memset(&bta_pba_client_cb, 0, sizeof(tBTA_PBA_CLIENT_CB));

    /* store callback function */
    bta_pba_client_cb.p_cback = p_data->api_enable.p_cback;

    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_ENABLE_EVT, NULL);
}

void bta_pba_client_api_disable(tBTA_PBA_CLIENT_DATA *p_data)
{
    if (!bta_sys_is_register(BTA_ID_PBC)) {
        return;
    }
    /* deregister with BTA system manager */
    bta_sys_deregister(BTA_ID_PBC);

    /* close all connections */
    for (int i = 0; i < PBA_CLIENT_MAX_CONNECTION; ++i) {
        if (bta_pba_client_cb.ccb[i].allocated) {
            close_goepc_and_report(&bta_pba_client_cb.ccb[i], BTA_PBA_CLIENT_DISABLE);
        }
    }

    /* store and clear callback function */
    tBTA_PBA_CLIENT_CBACK *p_cback = bta_pba_client_cb.p_cback;
    bta_pba_client_cb.p_cback = NULL;

    if(p_cback) {
        p_cback(BTA_PBA_CLIENT_DISABLE_EVT, NULL);
    }
}

void bta_pba_client_api_register(tBTA_PBA_CLIENT_DATA *p_data)
{
    /* create SDP records */
    bta_pba_client_create_record(p_data->api_register.name);

    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_REGISTER_EVT, NULL);
}

void bta_pba_client_api_deregister(tBTA_PBA_CLIENT_DATA *p_data)
{
    /* delete SDP records */
    bta_pba_client_del_record();

    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_DEREGISTER_EVT, NULL);
}

void bta_pba_client_api_open(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    p_ccb->sec_mask = p_data->api_open.sec_mask;
    if (p_data->api_open.mtu < PBA_CLIENT_MIN_MTU || p_data->api_open.mtu > PBA_CLIENT_MAX_MTU) {
        p_ccb->max_rx = PBA_CLIENT_MAX_MTU;
        p_ccb->max_tx = PBA_CLIENT_MAX_MTU;
    }
    else {
        p_ccb->max_rx = p_data->api_open.mtu;
        p_ccb->max_tx = p_data->api_open.mtu;
    }
    bdcpy(p_ccb->bd_addr, p_data->api_open.bd_addr);
    p_ccb->our_supported_feat = p_data->api_open.supported_feat;
    if (!bta_pba_client_do_disc(p_ccb)) {
        close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_SDP_ERROR);
    }
}

void bta_pba_client_api_close(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    UNUSED(p_data);
    tOBEX_PARSE_INFO info = {0};

    info.opcode = OBEX_OPCODE_DISCONNECT;
    UINT16 tx_buff_size = BT_SMALL_BUFFER_SIZE < p_ccb->max_tx ? BT_SMALL_BUFFER_SIZE : p_ccb->max_tx;
    UINT16 ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, tx_buff_size);
    ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_ccb->goep_cid), 4);
    ret |= GOEPC_SendRequest(p_ccb->goep_handle);

    if (ret != GOEP_SUCCESS) {
        /* anyway, this close operation is requested by upper, set reason to success */
        close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_NO_ERROR);
    }
}

void bta_pba_client_api_req(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    UINT16 ret;

    if (p_data->api_req.operation == BTA_PBA_CLIENT_OP_SET_PHONE_BOOK) {
        info.opcode = OBEX_OPCODE_SETPATH;
        info.flags = p_data->api_req.flags;
    }
    else {
        info.opcode = OBEX_OPCODE_GET_FINAL;
    }
    ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, p_ccb->max_tx);
    if (ret != GOEP_SUCCESS) {
        goto error;
    }

    ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_ccb->goep_cid), 4);

    if (p_data->api_req.operation != BTA_PBA_CLIENT_OP_SET_PHONE_BOOK) {
        ret |= GOEPC_RequestSetSRM(p_ccb->goep_handle, TRUE, FALSE);
    }

    if (p_data->api_req.name) {
        UINT16 name_len = strlen(p_data->api_req.name) + 1;
        if (name_len == 1) {
            /* empty string, add empty name header */
            ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_NAME, NULL, 0);
        }
        else {
            UINT8 *utf16_name = osi_malloc(2 * name_len);
            assert(utf16_name != NULL);
            ascii_to_utf16((UINT8 *)p_data->api_req.name, name_len, utf16_name);
            ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_NAME, utf16_name, 2 * name_len);
            osi_free(utf16_name);
        }
        osi_free(p_data->api_req.name);
        p_data->api_req.name = NULL;
    }

    switch (p_data->api_req.operation)
    {
    case BTA_PBA_CLIENT_OP_PULL_PHONE_BOOK:
        ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_TYPE, (const UINT8 *)type_pull_phone_book, TYPE_LEN_PULL_PHONE_BOOK);
        break;
    case BTA_PBA_CLIENT_OP_PULL_VCARD_LISTING:
        ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_TYPE, (const UINT8 *)type_pull_vcard_listing, TYPE_LEN_PULL_VCARD_LISTING);
        break;
    case BTA_PBA_CLIENT_OP_PULL_VCARD_ENTRY:
        ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_TYPE, (const UINT8 *)type_pull_vcard_entry, TYPE_LEN_PULL_VCARD_ENTRY);
        break;
    default:
        break;
    }

    if (p_data->api_req.app_param) {
        ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_APP_PARAM, p_data->api_req.app_param, p_data->api_req.app_param_len);
        osi_free(p_data->api_req.app_param);
    }

    ret |= GOEPC_SendRequest(p_ccb->goep_handle);
    p_ccb->operation = p_data->api_req.operation;
    if (ret != GOEP_SUCCESS) {
        goto error;
    }
    return;

error:
    if (p_data->api_req.name) {
        osi_free(p_data->api_req.name);
    }
    if (p_data->api_req.app_param) {
        osi_free(p_data->api_req.app_param);
    }
    close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_GOEP_ERROR);
}

static void goep_event_callback(UINT16 handle, UINT8 event, tGOEPC_MSG *p_msg)
{
    tBTA_PBA_CLIENT_DATA *p_data = NULL;

    switch (event)
    {
    case GOEPC_OPENED_EVT:
        p_data = (tBTA_PBA_CLIENT_DATA *)osi_malloc(sizeof(tBTA_PBA_CLIENT_DATA));
        assert(p_data != NULL);
        p_data->goep_connect.hdr.event = BTA_PBA_CLIENT_GOEP_CONNECT_EVT;
        p_data->goep_connect.hdr.layer_specific = handle;
        p_data->goep_connect.our_mtu = p_msg->opened.our_mtu;
        p_data->goep_connect.peer_mtu = p_msg->opened.peer_mtu;
        break;
    case GOEPC_CLOSED_EVT:
        p_data = (tBTA_PBA_CLIENT_DATA *)osi_malloc(sizeof(tBTA_PBA_CLIENT_DATA));
        assert(p_data != NULL);
        p_data->goep_disconnect.hdr.event = BTA_PBA_CLIENT_GOEP_DISCONNECT_EVT;
        p_data->goep_disconnect.hdr.layer_specific = handle;
        p_data->goep_disconnect.reason = p_msg->closed.reason;
        break;
    case GOEPC_RESPONSE_EVT:
        p_data = (tBTA_PBA_CLIENT_DATA *)osi_malloc(sizeof(tBTA_PBA_CLIENT_DATA));
        assert(p_data != NULL);
        p_data->goep_response.hdr.layer_specific = handle;
        p_data->goep_response.pkt = p_msg->response.pkt;
        p_data->goep_response.opcode = p_msg->response.opcode;
        p_data->goep_response.srm_en = p_msg->response.srm_en;
        p_data->goep_response.srm_wait = p_msg->response.srm_wait;
        if (p_msg->response.final) {
            p_data->goep_response.hdr.event = BTA_PBA_CLIENT_RESPONSE_FINAL_EVT;
        }
        else {
            p_data->hdr.event = BTA_PBA_CLIENT_RESPONSE_EVT;
        }
        break;
    case GOEPC_MTU_CHANGED_EVT:
    case GOEPC_CONGEST_EVT:
    case GOEPC_UNCONGEST_EVT:
        /* ignore these event */
        break;
    default:
        APPL_TRACE_WARNING("%s, unknown goep event: %d", __FUNCTION__, event);
        break;
    }
    if (p_data != NULL) {
        bta_sys_sendmsg(p_data);
    }
}

void bta_pba_client_do_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    tBTA_PBA_CLIENT_ERR reason = BTA_PBA_CLIENT_FAIL;

    /* find attr in sdp discovery result */
    BOOLEAN found = bta_pba_client_sdp_find_attr(p_ccb);
    if (found) {
        tOBEX_SVR_INFO svr = {0};
        if (p_ccb->peer_l2cap_psm != 0) {
            /* peer support obex over l2cap, use it */
            svr.tl = OBEX_OVER_L2CAP;
            svr.l2cap.psm = p_ccb->peer_l2cap_psm;
            svr.l2cap.sec_mask = p_ccb->sec_mask;
            svr.l2cap.pref_mtu = 0;
            bdcpy(svr.l2cap.addr, p_ccb->bd_addr);
        }
        else {
            /* otherwise, use obex over rfcomm */
            svr.tl = OBEX_OVER_RFCOMM;
            svr.rfcomm.scn = p_ccb->peer_rfcomm_scn;
            svr.rfcomm.sec_mask = p_ccb->sec_mask;
            svr.rfcomm.pref_mtu = 0;
            bdcpy(svr.rfcomm.addr, p_ccb->bd_addr);
        }

        if (GOEPC_Open(&svr, goep_event_callback, &p_ccb->goep_handle) == GOEP_SUCCESS) {
            /* start connection success */
            return;
        }
        else {
            reason = BTA_PBA_CLIENT_GOEP_ERROR;
        }
    }
    else {
        reason = BTA_PBA_CLIENT_SDP_ERROR;
    }

    /* critical sdp attribute not found or start goep connection failed */
    tBTA_PBA_CLIENT_CONN conn;
    conn.handle = 0;
    conn.error = reason;
    bdcpy(conn.bd_addr, p_ccb->bd_addr);
    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_CLOSE_EVT, (tBTA_PBA_CLIENT *)&conn);

    /* free ccb */
    free_ccb(p_ccb);
}

void bta_pba_client_authenticate(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    /* [todo]: support authenticate */
}

void bta_pba_client_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    tBTA_PBA_CLIENT_CONN conn;
    conn.handle = p_ccb->allocated;
    conn.peer_supported_repo = p_ccb->peer_supported_repo;
    conn.peer_supported_feat = p_ccb->peer_supported_feat;
    conn.error = BTA_PBA_CLIENT_NO_ERROR;
    bdcpy(conn.bd_addr, p_ccb->bd_addr);
    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_OPEN_EVT, (tBTA_PBA_CLIENT *)&conn);
}

void bta_pba_client_force_disconnect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    /* force disconnect is requested by upper, set reason to success */
    close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_NO_ERROR);
}

void bta_pba_client_response(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    tOBEX_PARSE_INFO info;
    tBTA_PBA_CLIENT_ERR reason = BTA_PBA_CLIENT_GOEP_ERROR;

    OBEX_ParseResponse(p_data->goep_response.pkt, p_data->goep_response.opcode, &info);
    if (p_data->goep_response.opcode == OBEX_OPCODE_GET_FINAL &&
        (info.response_code == OBEX_RESPONSE_CODE_CONTINUE || info.response_code == (OBEX_RESPONSE_CODE_CONTINUE | OBEX_FINAL_BIT_MASK))) {
        UINT8 *header = NULL;
        UINT8 *body_data = NULL;
        UINT16 body_data_len = 0;
        UINT8 *app_param = NULL;
        UINT16 app_param_len = 0;
        while((header = OBEX_GetNextHeader(p_data->goep_response.pkt, &info)) != NULL) {
            switch (*header)
            {
            case OBEX_HEADER_ID_BODY:
            case OBEX_HEADER_ID_END_OF_BODY:
                if (body_data == NULL) {
                    /* first body header */
                    body_data = header + 3;     /* skip opcode, length */
                    body_data_len = OBEX_GetHeaderLength(header) - 3;
                }
                else {
                    /* another body header found */
                    report_data_event(p_ccb, body_data, body_data_len, NULL, 0, FALSE, NULL);
                    body_data = header + 3;     /* skip opcode, length */
                    body_data_len = OBEX_GetHeaderLength(header) - 3;
                }
                break;
            case OBEX_HEADER_ID_APP_PARAM:
                app_param = header + 3;
                app_param_len = OBEX_GetHeaderLength(header) - 3;
                break;
            default:
                break;
            }
        }
        if (body_data != NULL || app_param != NULL) {
            /* report body data and app param, dont free packet here */
            report_data_event(p_ccb, body_data, body_data_len, app_param, app_param_len, FALSE, p_data->goep_response.pkt);
        }
        else {
            /* not any body data or app param */
            osi_free(p_data->goep_response.pkt);
        }

        /* if SRM not enable, we need to send a empty get request */
        if (!p_data->goep_response.srm_en || p_data->goep_response.srm_wait) {
            build_and_send_empty_get_req(p_ccb);
        }
    }
    else {
        /* unexpected opcode or response code */
        reason = calculate_response_error(info.response_code);
        goto error;
    }
    return;

error:
    if (p_data->goep_response.pkt != NULL) {
        osi_free(p_data->goep_response.pkt);
    }
    close_goepc_and_report(p_ccb, reason);
}

void bta_pba_client_response_final(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    tOBEX_PARSE_INFO info;
    UINT8 *header = NULL;
    tBTA_PBA_CLIENT_ERR reason = BTA_PBA_CLIENT_FAIL;

    OBEX_ParseResponse(p_data->goep_response.pkt, p_data->goep_response.opcode, &info);
    if (p_data->goep_response.opcode == OBEX_OPCODE_CONNECT) {
        if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
            /* obex connect success */
            if (info.max_packet_length < 255) {
                p_ccb->max_tx = 255;
            }
            else if (p_ccb->max_tx > info.max_packet_length) {
                p_ccb->max_tx = info.max_packet_length;
            }
            BOOLEAN cid_found = false;
            while((header = OBEX_GetNextHeader(p_data->goep_response.pkt, &info)) != NULL) {
                if (*header == OBEX_HEADER_ID_CONNECTION_ID) {
                    cid_found = true;
                    memcpy((UINT8 *)(&p_ccb->goep_cid), header + 1, 4);
                    break;
                }
            }
            if (!cid_found) {
                goto error;
            }

            BT_HDR *p_buf = (BT_HDR *)osi_malloc(sizeof(BT_HDR));
            assert(p_buf != NULL);
            p_buf->event = BTA_PBA_CLIENT_CONNECT_EVT;
            p_buf->layer_specific = p_ccb->allocated;
            bta_sys_sendmsg(p_buf);
        }
        else if (info.response_code == (OBEX_RESPONSE_CODE_UNAUTHORIZED | OBEX_FINAL_BIT_MASK)){
            /* need to authenticate */
            if (p_ccb->authenticate) {
                /* already try to authenticate, but failed */
                reason = BTA_PBA_CLIENT_AUTH_FAIL;
                goto error;
            }

            p_ccb->authenticate = TRUE;
            /* [todo]: we don't support authenticate currently */
            goto error;
        }
        else {
            /* other unexpected response code */
            goto error;
        }
        osi_free(p_data->goep_response.pkt);
    }
    else if (p_data->goep_response.opcode == OBEX_OPCODE_GET_FINAL) {
        /* check response code is success */
        if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
            UINT8 *body_data = NULL;
            UINT16 body_data_len = 0;
            UINT8 *app_param = NULL;
            UINT16 app_param_len = 0;
            while((header = OBEX_GetNextHeader(p_data->goep_response.pkt, &info)) != NULL) {
                switch (*header)
                {
                /* actually, BODY should not in this final response */
                case OBEX_HEADER_ID_BODY:
                case OBEX_HEADER_ID_END_OF_BODY:
                    if (body_data == NULL) {
                        /* first body header */
                        body_data = header + 3;     /* skip opcode, length */
                        body_data_len = OBEX_GetHeaderLength(header) - 3;
                    }
                    else {
                        /* another body header found */
                        report_data_event(p_ccb, body_data, body_data_len, NULL, 0, FALSE, NULL);
                        body_data = header + 3;     /* skip opcode, length */
                        body_data_len = OBEX_GetHeaderLength(header) - 3;
                    }
                    break;
                case OBEX_HEADER_ID_APP_PARAM:
                    app_param = header + 3;
                    app_param_len = OBEX_GetHeaderLength(header) - 3;
                    break;
                default:
                    break;
                }
            }
            if (body_data != NULL || app_param != NULL) {
                /* report body data and app param, dont free packet here */
                report_data_event(p_ccb, body_data, body_data_len, app_param, app_param_len, TRUE, p_data->goep_response.pkt);
                /* done, return */
                return;
            }
        }
        /* unexpected response code or body data not found */
        reason = calculate_response_error(info.response_code);
        report_error_data_event(p_ccb, reason);
        osi_free(p_data->goep_response.pkt);

        /* state machine is good, don't goto error */
    }
    else if (p_data->goep_response.opcode == OBEX_OPCODE_SETPATH) {
        if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
            report_data_event(p_ccb, NULL, 0, NULL, 0, TRUE, NULL);
        }
        else {
            reason = calculate_response_error(info.response_code);
            report_error_data_event(p_ccb, reason);
        }
        osi_free(p_data->goep_response.pkt);
    }
    else if (p_data->goep_response.opcode == OBEX_OPCODE_DISCONNECT) {
        /* received disconnect response, close goep connection now */
        reason = BTA_PBA_CLIENT_NO_ERROR;
        close_goepc_and_report(p_ccb, reason);
        osi_free(p_data->goep_response.pkt);
    }
    else {
        /* unexpected opcode or response code */
        reason = calculate_response_error(info.response_code);
        goto error;
    }
    return;

error:
    if (p_data->goep_response.pkt != NULL) {
        osi_free(p_data->goep_response.pkt);
    }
    close_goepc_and_report(p_ccb, reason);
}

void bta_pba_client_goep_connect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    /* limit max rx to our goep mtu */
    if (p_ccb->max_rx > p_data->goep_connect.our_mtu) {
        p_ccb->max_rx = p_data->goep_connect.our_mtu;
    }
    /* limit max tx to peer goep mtu */
    if (p_ccb->max_tx > p_data->goep_connect.peer_mtu) {
        p_ccb->max_tx = p_data->goep_connect.peer_mtu;
    }

    /* build and send obex connect request */
    tOBEX_PARSE_INFO info = {0};
    info.opcode = OBEX_OPCODE_CONNECT;
    info.obex_version_number = OBEX_VERSION_NUMBER;
    info.max_packet_length = p_ccb->max_rx;
    /* before obex connect response, we dont know the real max_tx, use BT_SMALL_BUFFER_SIZE as tx buff size */
    UINT16 ret = GOEPC_PrepareRequest(p_ccb->goep_handle, &info, BT_SMALL_BUFFER_SIZE);
    /* add target header */
    ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_TARGET, pbap_target_uuid, 16);
    if (p_ccb->send_supported_feat) {
        /* add application parameters with supported features */
        UINT8 app_param[6];
        app_param[0] = BTA_PBAP_APP_PARAM_PBAP_SUPPORTED_FEATURES;
        app_param[1] = BTA_PBAP_APP_PARAM_LENGTH_PBAP_SUPPORTED_FEATURES;
        UINT32_TO_FIELD(&app_param[2], p_ccb->our_supported_feat);
        ret |= GOEPC_RequestAddHeader(p_ccb->goep_handle, OBEX_HEADER_ID_APP_PARAM, app_param, 6);
    }
    ret |= GOEPC_SendRequest(p_ccb->goep_handle);
    if (ret != GOEP_SUCCESS) {
        close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_GOEP_ERROR);
    }
}

void bta_pba_client_goep_disconnect(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    p_ccb->goep_handle = 0;

    /* report connection closed event */
    tBTA_PBA_CLIENT_CONN conn;
    conn.handle = p_ccb->allocated;
    bdcpy(conn.bd_addr, p_ccb->bd_addr);
    conn.error = BTA_PBA_CLIENT_GOEP_ERROR;
    bta_pba_client_cb.p_cback(BTA_PBA_CLIENT_CONN_CLOSE_EVT, (tBTA_PBA_CLIENT *)&conn);
    free_ccb(p_ccb);
}

void bta_pba_client_free_response(tBTA_PBA_CLIENT_CCB *p_ccb, tBTA_PBA_CLIENT_DATA *p_data)
{
    if (p_data->goep_response.pkt != NULL) {
        osi_free(p_data->goep_response.pkt);
    }
    close_goepc_and_report(p_ccb, BTA_PBA_CLIENT_GOEP_ERROR);
}

#endif
