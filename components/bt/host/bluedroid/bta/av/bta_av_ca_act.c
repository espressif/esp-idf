/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/bt_target.h"
#if BTA_AV_CA_INCLUDED

#include <string.h>
#include "bta/bta_av_api.h"
#include "bta_av_int.h"
#include "stack/avdt_api.h"
#include "bta/utl.h"
#include "stack/l2c_api.h"
#include "osi/allocator.h"
#include "osi/list.h"
#include "stack/goep_common.h"
#include "stack/goepc_api.h"
#include "stack/obex_api.h"
#include "common/bt_trace.h"

#define COVER_ART_HEADER_ID_IMG_HANDLE      0x30
#define COVER_ART_HEADER_ID_IMG_DESCRIPTOR  0x71
static const UINT8 cover_art_uuid[16] = {0x71, 0x63, 0xDD, 0x54, 0x4A, 0x7E, 0x11, 0xE2, 0xB4, 0x7C, 0x00, 0x50, 0xC2, 0x49, 0x00, 0x48};
static const char *cover_art_img_type_img = "x-bt/img-img";
static const char *cover_art_img_type_thm = "x-bt/img-thm";
static const char *cover_art_img_type_prop = "x-bt/img-properties";

#define COVER_ART_IMG_TYPE_IMG_LEN          13
#define COVER_ART_IMG_TYPE_THM_LEN          13
#define COVER_ART_IMG_TYPE_PROP_LEN         20


static BOOLEAN find_rcb_idx_by_goep_handle(UINT16 handle, UINT16 *out_idx)
{
    for (UINT16 i = 0; i < BTA_AV_NUM_RCB; ++i) {
        if (bta_av_cb.rcb[i].handle != BTA_AV_RC_HANDLE_NONE && bta_av_cb.rcb[i].cover_art_goep_hdl == handle) {
            *out_idx = i;
            return TRUE;
        }
    }
    return FALSE;
}

static UINT8 get_rcb_idx(tBTA_AV_RCB *p_rcb)
{
    return (p_rcb - &bta_av_cb.rcb[0]);
}

static void get_peer_bd_addr(tBTA_AV_RCB *p_rcb, BD_ADDR out_addr)
{
    /* check if this rcb is related to a scb */
    if (p_rcb->shdl && p_rcb->shdl <= BTA_AV_NUM_STRS) {
        tBTA_AV_SCB *p_scb = bta_av_cb.p_scb[p_rcb->shdl - 1];
        bdcpy(out_addr, p_scb->peer_addr);
    }
    /* else, try get peer addr from lcb */
    else if (p_rcb->lidx && p_rcb->lidx <= BTA_AV_NUM_LINKS + 1)
    {
        bdcpy(out_addr, bta_av_cb.lcb[p_rcb->lidx-1].addr);
    }
}

static void report_data_event(BT_HDR *pkt, UINT8 *p_data, UINT16 data_len, BOOLEAN final)
{
    tBTA_AV_CA_DATA ca_data;
    ca_data.status = BT_STATUS_SUCCESS;
    ca_data.final = final;
    ca_data.data_len = data_len;
    ca_data.p_data = p_data;
    ca_data.p_hdr = pkt;
    (*bta_av_cb.p_cback)(BTA_AV_CA_DATA_EVT, (tBTA_AV *) &ca_data);
}

static void report_error_data_event(UINT16 status)
{
    tBTA_AV_CA_DATA ca_data;
    ca_data.status = status;
    ca_data.final = TRUE;
    ca_data.data_len = 0;
    ca_data.p_data = NULL;
    ca_data.p_hdr = NULL;
    (*bta_av_cb.p_cback)(BTA_AV_CA_DATA_EVT, (tBTA_AV *) &ca_data);
}

static void build_and_send_connect_req(tBTA_AV_RCB *p_rcb)
{
    tOBEX_PARSE_INFO info = {0};
    info.opcode = OBEX_OPCODE_CONNECT;
    info.obex_version_number = 0x15;
    info.max_packet_length = p_rcb->cover_art_max_rx;
    /* before OBEX connect response, we dont know cover_art_max_tx, use BT_SMALL_BUFFER_SIZE as tx buff size */
    if (GOEPC_PrepareRequest(p_rcb->cover_art_goep_hdl, &info, BT_SMALL_BUFFER_SIZE) == GOEP_SUCCESS) {
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_TARGET, (UINT8 *)cover_art_uuid, 16);
        GOEPC_SendRequest(p_rcb->cover_art_goep_hdl);
    }
}

static void build_and_send_disconnect_req(tBTA_AV_RCB *p_rcb)
{
    tOBEX_PARSE_INFO info = {0};
    info.opcode = OBEX_OPCODE_DISCONNECT;
    if (GOEPC_PrepareRequest(p_rcb->cover_art_goep_hdl, &info, BT_SMALL_BUFFER_SIZE) == GOEP_SUCCESS) {
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_rcb->cover_art_cid), 4);
        GOEPC_SendRequest(p_rcb->cover_art_goep_hdl);
    }
}

static void build_and_send_empty_get_req(tBTA_AV_RCB *p_rcb)
{
    tOBEX_PARSE_INFO info = {0};
    info.opcode = OBEX_OPCODE_GET_FINAL;
    /* empty get request, use a small buff size */
    UINT16 tx_buff_size = BT_SMALL_BUFFER_SIZE < p_rcb->cover_art_max_tx ? BT_SMALL_BUFFER_SIZE : p_rcb->cover_art_max_tx;
    if (GOEPC_PrepareRequest(p_rcb->cover_art_goep_hdl, &info, tx_buff_size) == GOEP_SUCCESS) {
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_rcb->cover_art_cid), 4);
        GOEPC_SendRequest(p_rcb->cover_art_goep_hdl);
    }
}

static void close_goepc_and_disconnect(tBTA_AV_RCB *p_rcb)
{
    if (p_rcb->cover_art_goep_hdl) {
        GOEPC_Close(p_rcb->cover_art_goep_hdl);
    }
    p_rcb->cover_art_goep_hdl = 0;
    p_rcb->cover_art_cid = 0;
    p_rcb->cover_art_max_tx = 0;
    p_rcb->cover_art_max_rx = 0;

    tBTA_AV_DATA *p_data = (tBTA_AV_DATA *) osi_malloc(sizeof(tBTA_AV_DATA));
    if (p_data == NULL) {
        assert(0);
    }
    p_data->hdr.event = BTA_AV_CA_GOEP_DISCONNECT_EVT;
    p_data->hdr.layer_specific = get_rcb_idx(p_rcb);
    p_data->ca_disconnect.reason = BT_STATUS_FAIL;
    bta_sys_sendmsg(p_data);
}

static void image_handle_to_utf16(const UINT8 *image_handle, UINT8 *buffer)
{
    UINT8 pos = 0;
    for (int i = 0 ; i < BTA_AV_CA_IMG_HDL_LEN ; i++){
        buffer[pos++] = 0;
        buffer[pos++] = image_handle[i];
    }
    buffer[pos++] = 0;
    buffer[pos++] = 0;
}

void bta_av_ca_goep_event_handler(UINT16 handle, UINT8 event, tGOEPC_MSG *p_msg)
{
    tBTA_AV_DATA *p_data = NULL;
    UINT16 rcb_idx;
    if (!find_rcb_idx_by_goep_handle(handle, &rcb_idx)) {
        /* can not find a rcb, go error */
        goto error;
    }

    if (event == GOEPC_RESPONSE_EVT || event == GOEPC_OPENED_EVT || event == GOEPC_CLOSED_EVT) {
        p_data = (tBTA_AV_DATA *) osi_malloc(sizeof(tBTA_AV_DATA));
        assert(p_data != NULL);
    }

    switch (event)
    {
    case GOEPC_OPENED_EVT:
        p_data->hdr.layer_specific = rcb_idx;
        p_data->hdr.event = BTA_AV_CA_GOEP_CONNECT_EVT;
        p_data->ca_connect.max_rx = p_msg->opened.our_mtu;
        break;
    case GOEPC_CLOSED_EVT:
        p_data->hdr.layer_specific = rcb_idx;
        p_data->hdr.event = BTA_AV_CA_GOEP_DISCONNECT_EVT;
        p_data->ca_disconnect.reason = BT_STATUS_FAIL;
        break;
    case GOEPC_RESPONSE_EVT:
        p_data->hdr.layer_specific = rcb_idx;
        p_data->ca_response.pkt = p_msg->response.pkt;
        p_data->ca_response.opcode = p_msg->response.opcode;
        p_data->ca_response.srm_en = p_msg->response.srm_en;
        p_data->ca_response.srm_wait = p_msg->response.srm_wait;
        if (p_msg->response.final) {
            p_data->hdr.event = BTA_AV_CA_RESPONSE_FINAL_EVT;
        }
        else {
            p_data->hdr.event = BTA_AV_CA_RESPONSE_EVT;
        }
        break;
    case GOEPC_MTU_CHANGED_EVT:
    case GOEPC_CONGEST_EVT:
    case GOEPC_UNCONGEST_EVT:
        /* ignore these event */
        break;
    default:
        goto error;
        break;
    }
    if (p_data) {
        bta_sys_sendmsg(p_data);
    }
    return;

error:
    /* can not find rcb, just free resource and disconnect */
    if (p_data != NULL) {
        osi_free(p_data);
    }
    if (event == GOEPC_RESPONSE_EVT && p_msg->response.pkt != NULL) {
        osi_free(p_msg->response.pkt);
    }
    if (event != GOEPC_CLOSED_EVT) {
        GOEPC_Close(handle);
    }
}

void bta_av_ca_api_open(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    tOBEX_SVR_INFO svr = {0};
    svr.tl = OBEX_OVER_L2CAP;
    svr.l2cap.psm = p_rcb->cover_art_l2cap_psm;
    svr.l2cap.pref_mtu = p_data->api_ca_open.mtu;
    /* reuse the security mask store in bta_av_cb, when support multi connection, this may need change */
    svr.l2cap.sec_mask = bta_av_cb.sec_mask;
    p_rcb->cover_art_max_rx = p_data->api_ca_open.mtu;
    get_peer_bd_addr(p_rcb, svr.l2cap.addr);

    if (GOEPC_Open(&svr, bta_av_ca_goep_event_handler, &p_rcb->cover_art_goep_hdl) != GOEP_SUCCESS) {
        /* open failed */
        if ((p_data = (tBTA_AV_DATA *) osi_malloc(sizeof(tBTA_AV_DATA))) == NULL) {
            assert(0);
        }
        p_data->hdr.event = BTA_AV_CA_GOEP_DISCONNECT_EVT;
        p_data->hdr.layer_specific = get_rcb_idx(p_rcb);
        p_data->ca_disconnect.reason = BT_STATUS_FAIL;
        bta_sys_sendmsg(p_data);
    }
}

void bta_av_ca_api_close(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    /* this is a normal disconnect, just build and send OBEX disconnect request */
    build_and_send_disconnect_req(p_rcb);
}

void bta_av_ca_api_get(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    tOBEX_PARSE_INFO info = {0};
    UINT8 image_handle_utf16[BTA_AV_CA_IMG_HDL_UTF16_LEN];
    info.opcode = OBEX_OPCODE_GET_FINAL;
    /* limit the tx buff size to BT_DEFAULT_BUFFER_SIZE */
    UINT16 tx_buff_size = BT_DEFAULT_BUFFER_SIZE < p_rcb->cover_art_max_tx ? BT_DEFAULT_BUFFER_SIZE : p_rcb->cover_art_max_tx;
    if (GOEPC_PrepareRequest(p_rcb->cover_art_goep_hdl, &info, tx_buff_size) != GOEP_SUCCESS) {
        /* something error */
        goto error;
    }

    GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_CONNECTION_ID, (UINT8 *)(&p_rcb->cover_art_cid), 4);
    switch (p_data->api_ca_get.type)
    {
    case BTA_AV_CA_GET_IMAGE_PROPERTIES:
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_TYPE, (const UINT8 *)cover_art_img_type_prop, COVER_ART_IMG_TYPE_PROP_LEN);
        break;
    case BTA_AV_CA_GET_IMAGE:
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_TYPE, (const UINT8 *)cover_art_img_type_img, COVER_ART_IMG_TYPE_IMG_LEN);
        break;
    case BTA_AV_CA_GET_LINKED_THUMBNAIL:
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, OBEX_HEADER_ID_TYPE, (const UINT8 *)cover_art_img_type_thm, COVER_ART_IMG_TYPE_THM_LEN);
        break;
    default:
        /* should not go here */
        assert(0);
        break;
    }
    image_handle_to_utf16(p_data->api_ca_get.image_handle, image_handle_utf16);
    GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, COVER_ART_HEADER_ID_IMG_HANDLE, (UINT8 *)image_handle_utf16, BTA_AV_CA_IMG_HDL_UTF16_LEN);
    if (p_data->api_ca_get.type == BTA_AV_CA_GET_IMAGE) {
        GOEPC_RequestAddHeader(p_rcb->cover_art_goep_hdl, COVER_ART_HEADER_ID_IMG_DESCRIPTOR, (UINT8 *)p_data->api_ca_get.image_descriptor, p_data->api_ca_get.image_descriptor_len);
    }
    /* always request to enable srm */
    GOEPC_RequestSetSRM(p_rcb->cover_art_goep_hdl, TRUE, FALSE);

    if (GOEPC_SendRequest(p_rcb->cover_art_goep_hdl) != GOEP_SUCCESS) {
        goto error;
    }
    return;
error:
    close_goepc_and_disconnect(p_rcb);
}

void bta_av_ca_response(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    tOBEX_PARSE_INFO info;
    OBEX_ParseResponse(p_data->ca_response.pkt, p_data->ca_response.opcode, &info);
    /* we always use a final get */
    if (p_data->ca_response.opcode == OBEX_OPCODE_GET_FINAL
        && (info.response_code == OBEX_RESPONSE_CODE_CONTINUE || info.response_code == (OBEX_RESPONSE_CODE_CONTINUE | OBEX_FINAL_BIT_MASK)))
    {
        UINT8 *header = NULL;
        UINT8 *body_data = NULL;
        UINT16 body_data_len = 0;
        while((header = OBEX_GetNextHeader(p_data->ca_response.pkt, &info)) != NULL) {
            switch (*header)
            {
            case OBEX_HEADER_ID_BODY:
            /* actually,END_OF_BODY should not in this continue response */
            case OBEX_HEADER_ID_END_OF_BODY:
                if (body_data == NULL) {
                    /* first body header */
                    body_data = header + 3;     /* skip opcode, length */
                    body_data_len = OBEX_GetHeaderLength(header) - 3;
                }
                else {
                    /* another body header found */
                    report_data_event(NULL, body_data, body_data_len, FALSE);
                    body_data = header + 3;     /* skip opcode, length */
                    body_data_len = OBEX_GetHeaderLength(header) - 3;
                }
                break;
            default:
                break;
            }
        }
        if (body_data != NULL) {
            /* the only one or the last body data */
            report_data_event(p_data->ca_response.pkt, body_data, body_data_len, FALSE);
        }
        else {
            /* not any body data */
            osi_free(p_data->ca_response.pkt);
        }

        /* if SRM not enable, we need to send a empty get request */
        if (!p_data->ca_response.srm_en || p_data->ca_response.srm_wait) {
            build_and_send_empty_get_req(p_rcb);
        }
    }
    else {
        osi_free(p_data->ca_response.pkt);
        goto error;
    }
    return;

error:
    close_goepc_and_disconnect(p_rcb);
}

void bta_av_ca_response_final(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    tOBEX_PARSE_INFO info;
    OBEX_ParseResponse(p_data->ca_response.pkt, p_data->ca_response.opcode, &info);
    UINT8 *header = NULL;
    if (p_data->ca_response.opcode == OBEX_OPCODE_CONNECT) {
        /* we expect a success response code with final bit set */
        if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
            if (info.max_packet_length < 255) {
                p_rcb->cover_art_max_tx = 255;
            }
            else {
                p_rcb->cover_art_max_tx = info.max_packet_length;
            }
            BOOLEAN cid_found = false;
            while((header = OBEX_GetNextHeader(p_data->ca_response.pkt, &info)) != NULL) {
                if (*header == OBEX_HEADER_ID_CONNECTION_ID) {
                    cid_found = true;
                    memcpy((UINT8 *)(&p_rcb->cover_art_cid), header + 1, 4);
                    break;
                }
            }
            if (!cid_found) {
                goto error;
            }
            tBTA_AV_CA_STATUS ca_status;
            ca_status.connected = TRUE;
            ca_status.reason = BT_STATUS_SUCCESS;
            (*bta_av_cb.p_cback)(BTA_AV_CA_STATUS_EVT, (tBTA_AV *) &ca_status);
            /* done, free response packet */
            osi_free(p_data->ca_response.pkt);
        }
        else {
            osi_free(p_data->ca_response.pkt);
            goto error;
        }
    }
    else if (p_data->ca_response.opcode == OBEX_OPCODE_GET_FINAL) {
        UINT8 *body_data = NULL;
        UINT16 body_data_len = 0;
        /* check response code is success */
        if (info.response_code == (OBEX_RESPONSE_CODE_OK | OBEX_FINAL_BIT_MASK)) {
            while((header = OBEX_GetNextHeader(p_data->ca_response.pkt, &info)) != NULL) {
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
                        report_data_event(NULL, body_data, body_data_len, FALSE);
                        body_data = header + 3;     /* skip opcode, length */
                        body_data_len = OBEX_GetHeaderLength(header) - 3;
                    }
                    break;
                default:
                    break;
                }
            }
            if (body_data != NULL) {
                /* the only one or the last body data, packet will be free by upper layer */
                report_data_event(p_data->ca_response.pkt, body_data, body_data_len, TRUE);
            }
            else {
                /* not any body data */
                osi_free(p_data->ca_response.pkt);
            }
        }
        else {
            report_error_data_event(BT_STATUS_FAIL);
            osi_free(p_data->ca_response.pkt);
        }
    }
    else if (p_data->ca_response.opcode == OBEX_OPCODE_DISCONNECT) {
        /* received disconnect response, close l2cap channel and reset cover art value */
        bta_av_ca_force_disconnect(p_rcb, p_data);
        osi_free(p_data->ca_response.pkt);
    }
    else {
        osi_free(p_data->ca_response.pkt);
        goto error;
    }
    return;

error:
    close_goepc_and_disconnect(p_rcb);
}

void bta_av_ca_goep_connect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    /* goep connection open, use a smaller value as max_rx */
    if (p_rcb->cover_art_max_rx > p_data->ca_connect.max_rx) {
        p_rcb->cover_art_max_rx = p_data->ca_connect.max_rx;
    }
    build_and_send_connect_req(p_rcb);
}

void bta_av_ca_goep_disconnect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    p_rcb->cover_art_goep_hdl = 0;
    p_rcb->cover_art_max_rx = 0;
    p_rcb->cover_art_max_tx = 0;
    p_rcb->cover_art_cid = 0;

    tBTA_AV_CA_STATUS ca_status;
    ca_status.connected = FALSE;
    ca_status.reason = p_data->ca_disconnect.reason;
    (*bta_av_cb.p_cback)(BTA_AV_CA_STATUS_EVT, (tBTA_AV *) &ca_status);
}

void bta_av_ca_force_disconnect(tBTA_AV_RCB *p_rcb, tBTA_AV_DATA *p_data)
{
    if (p_rcb->cover_art_goep_hdl) {
        GOEPC_Close(p_rcb->cover_art_goep_hdl);
    }
    /* dont reset p_rcb->cover_art_l2cap_psm */
    p_rcb->cover_art_goep_hdl = 0;
    p_rcb->cover_art_cid = 0;
    p_rcb->cover_art_max_tx = 0;
    p_rcb->cover_art_max_rx = 0;
    tBTA_AV_CA_STATUS ca_status;
    ca_status.connected = FALSE;
    /* force disconnect by upper, set reason to success */
    ca_status.reason = BT_STATUS_SUCCESS;
    (*bta_av_cb.p_cback)(BTA_AV_CA_STATUS_EVT, (tBTA_AV *) &ca_status);
}

void bta_av_ca_reset(tBTA_AV_RCB *p_rcb)
{
    if (p_rcb->cover_art_goep_hdl) {
        GOEPC_Close(p_rcb->cover_art_goep_hdl);
    }
    p_rcb->cover_art_l2cap_psm = 0;
    p_rcb->cover_art_goep_hdl = 0;
    p_rcb->cover_art_state = 0;
    p_rcb->cover_art_cid = 0;
    p_rcb->cover_art_max_tx = 0;
    p_rcb->cover_art_max_rx = 0;
}

#endif /* BTA_AV_CA_INCLUDED */
