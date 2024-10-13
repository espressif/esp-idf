/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/obex_api.h"
#include "obex_int.h"
#include "obex_tl.h"
#include "obex_tl_l2cap.h"

#if (OBEX_INCLUDED == TRUE)

static inline void obex_server_to_tl_server(tOBEX_SVR_INFO *server, tOBEX_TL_SVR_INFO *tl_server)
{
    if (server->tl == OBEX_OVER_L2CAP) {
        tl_server->l2cap.psm = server->l2cap.psm;
        tl_server->l2cap.sec_mask = server->l2cap.sec_mask;
        tl_server->l2cap.pref_mtu = server->l2cap.pref_mtu;
        bdcpy(tl_server->l2cap.addr, server->l2cap.addr);
    }
    else {
        OBEX_TRACE_ERROR("Unsupported OBEX transport type\n");
        assert(0);
    }
}

static inline void obex_updata_packet_length(BT_HDR *p_buf, UINT16 len)
{
    UINT8 *p_pkt_len = (UINT8 *)(p_buf + 1) + p_buf->offset + 1;
    STORE16BE(p_pkt_len, len);
}

/*******************************************************************************
**
** Function         OBEX_Init
**
** Description      Initialize OBEX Profile, must call before using any other
**                  OBEX APIs
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_Init(void)
{
#if (OBEX_DYNAMIC_MEMORY)
    if (!obex_cb_ptr) {
        obex_cb_ptr = (tOBEX_CB *)osi_malloc(sizeof(tOBEX_CB));
        if (!obex_cb_ptr) {
            return OBEX_NO_RESOURCES;
        }
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
    memset(&obex_cb, 0, sizeof(tOBEX_CB));
    obex_cb.tl_ops[OBEX_OVER_L2CAP] = obex_tl_l2cap_ops_get();
    if (obex_cb.tl_ops[OBEX_OVER_L2CAP]->init != NULL) {
        obex_cb.tl_ops[OBEX_OVER_L2CAP]->init(obex_tl_l2cap_callback);
    }
    /* Not implement yet */
    /*
    obex_cb.tl_ops[OBEX_OVER_RFCOMM] = obex_tl_rfcomm_ops_get();
    if (obex_cb.tl_ops[OBEX_OVER_RFCOMM]->init != NULL) {
        obex_cb.tl_ops[OBEX_OVER_RFCOMM]->init(obex_tl_rfcomm_callback);
    }
    */
    obex_cb.trace_level = BT_TRACE_LEVEL_ERROR;
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_Deinit
**
** Description      Deinit OBEX profile, once deinit, can not use any other
**                  APIs until call OBEX_Init again
**
*******************************************************************************/
void OBEX_Deinit(void)
{
    if (obex_cb.tl_ops[OBEX_OVER_L2CAP]->deinit != NULL) {
        obex_cb.tl_ops[OBEX_OVER_L2CAP]->deinit();
    }
    /*
    if (obex_cb.tl_ops[OBEX_OVER_RFCOMM]->deinit != NULL) {
        obex_cb.tl_ops[OBEX_OVER_RFCOMM]->deinit();
    }
    */
#if (OBEX_DYNAMIC_MEMORY)
    if (obex_cb_ptr) {
        osi_free(obex_cb_ptr);
        obex_cb_ptr = NULL;
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
}

/*******************************************************************************
**
** Function         OBEX_CreateConn
**
** Description      Start the progress of creating an OBEX connection
**
** Returns          OBEX_SUCCESS if successful, otherwise failed, when the
**                  connection is opened, an OBEX_CONNECT_EVT will come
**
*******************************************************************************/
UINT16 OBEX_CreateConn(tOBEX_SVR_INFO *server, tOBEX_MSG_CBACK callback, UINT16 *out_handle)
{
    UINT16 ret = OBEX_SUCCESS;
    tOBEX_CCB *p_ccb = NULL;

    do {
        if (server->tl >= OBEX_NUM_TL) {
            ret = OBEX_INVALID_PARAM;
            break;
        }

        p_ccb = obex_allocate_ccb();
        if (p_ccb == NULL) {
            ret = OBEX_NO_RESOURCES;
            break;
        }

        tOBEX_TL_SVR_INFO tl_server = {0};
        obex_server_to_tl_server(server, &tl_server);
        p_ccb->tl = server->tl;
        p_ccb->tl_hdl = obex_cb.tl_ops[p_ccb->tl]->connect(&tl_server);
        if (p_ccb->tl_hdl == 0) {
            ret =  OBEX_ERROR_TL;
            break;
        }

        p_ccb->callback = callback;
        p_ccb->role = OBEX_ROLE_CLIENT;
        p_ccb->state = OBEX_STATE_OPENING;
        *out_handle = p_ccb->allocated;
    } while (0);

    if (ret != OBEX_SUCCESS && p_ccb != NULL) {
        obex_free_ccb(p_ccb);
    }
    return ret;
}

/*******************************************************************************
**
** Function         OBEX_RemoveConn
**
** Description      Remove an OBEX connection
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_RemoveConn(UINT16 handle)
{
    tOBEX_CCB *p_ccb = NULL;

    UINT16 ccb_idx = handle - 1;
    if (ccb_idx >= OBEX_MAX_CONNECTION || !obex_cb.ccb[ccb_idx].allocated) {
        return OBEX_BAD_HANDLE;
    }

    p_ccb = &obex_cb.ccb[ccb_idx];
    obex_cb.tl_ops[p_ccb->tl]->disconnect(p_ccb->tl_hdl);
    obex_free_ccb(p_ccb);

    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_RegisterServer
**
** Description      Register an OBEX server and listen the incoming connection
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_RegisterServer(tOBEX_SVR_INFO *server, tOBEX_MSG_CBACK callback, UINT16 *out_svr_handle)
{
    UINT8 ret = OBEX_SUCCESS;
    tOBEX_SCB *p_scb = NULL;

    do {
        if (server->tl >= OBEX_NUM_TL) {
            ret = OBEX_INVALID_PARAM;
            break;
        }

        p_scb = obex_allocate_scb();
        if (p_scb == NULL) {
            ret = OBEX_NO_RESOURCES;
            break;
        }

        tOBEX_TL_SVR_INFO tl_server = {0};
        obex_server_to_tl_server(server, &tl_server);
        p_scb->tl = server->tl;
        p_scb->tl_hdl = obex_cb.tl_ops[p_scb->tl]->bind(&tl_server);
        if (p_scb->tl_hdl == 0) {
            ret =  OBEX_ERROR_TL;
            break;
        }
        p_scb->callback = callback;

        if (out_svr_handle) {
            /* To avoid confuse with connection handle, left shift 8 bit */
            *out_svr_handle = p_scb->allocated << 8;
        }
    } while (0);

    if (ret != OBEX_SUCCESS && p_scb != NULL) {
        obex_free_scb(p_scb);
    }
    return ret;
}

/*******************************************************************************
**
** Function         OBEX_DeregisterServer
**
** Description      Deregister an OBEX server, if there are still a connection
**                  alive, the behavior depend on the implementation of transport
**                  layer
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_DeregisterServer(UINT16 svr_handle)
{
    tOBEX_SCB *p_scb = NULL;

    UINT16 scb_idx = (svr_handle >> 8) - 1;
    if (scb_idx >= OBEX_MAX_SERVER || !obex_cb.scb[scb_idx].allocated) {
        return OBEX_BAD_HANDLE;
    }

    p_scb = &obex_cb.scb[scb_idx];
    obex_cb.tl_ops[p_scb->tl]->unbind(p_scb->tl_hdl);
    obex_free_scb(p_scb);
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_SendPacket
**
** Description      Send a packet to peer OBEX server or client, once call
**                  this function, the ownership of pkt is lost, do not free
**                  or modify the pkt any more
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_SendPacket(UINT16 handle, BT_HDR *pkt)
{
    UINT16 ret = OBEX_SUCCESS;
    BOOLEAN free_pkt = true;
    tOBEX_CCB *p_ccb = NULL;
    do {
        if (pkt == NULL) {
            ret = OBEX_INVALID_PARAM;
            break;
        }

        UINT16 ccb_idx = handle - 1;
        if (ccb_idx >= OBEX_MAX_CONNECTION || !obex_cb.ccb[ccb_idx].allocated) {
            ret = OBEX_BAD_HANDLE;
            break;
        }

        p_ccb = &obex_cb.ccb[ccb_idx];
        if (p_ccb->state != OBEX_STATE_OPENED) {
            ret = OBEX_NOT_OPEN;
            break;
        }

        if (pkt->len > p_ccb->tl_peer_mtu) {
            ret = OBEX_PACKET_TOO_LARGE;
            break;
        }

        ret = obex_cb.tl_ops[p_ccb->tl]->send(p_ccb->tl_hdl, pkt);
        /* packet has pass to lower layer, do not free it */
        free_pkt = false;
        if (ret == OBEX_TL_SUCCESS || ret == OBEX_TL_CONGESTED) {
            ret = OBEX_SUCCESS;
        }
        else {
            ret = OBEX_ERROR_TL;
        }
    } while (0);

    if (free_pkt) {
        osi_free(pkt);
    }
    return ret;
}

/*******************************************************************************
**
** Function         OBEX_BuildRequest
**
** Description      Build a request packet with opcode and additional info,
**                  packet can free by osi_free
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_BuildRequest(tOBEX_PARSE_INFO *info, UINT16 buff_size, BT_HDR **out_pkt)
{
    if (buff_size < OBEX_MIN_PACKET_SIZE || info == NULL || out_pkt == NULL) {
        return OBEX_INVALID_PARAM;
    }
    buff_size += sizeof(BT_HDR) + OBEX_BT_HDR_MIN_OFFSET;

    BT_HDR *p_buf= (BT_HDR *)osi_malloc(buff_size);
    if (p_buf == NULL) {
        return OBEX_NO_RESOURCES;
    }

    UINT16 pkt_len = OBEX_MIN_PACKET_SIZE;
    p_buf->offset = OBEX_BT_HDR_MIN_OFFSET;
    /* use layer_specific to store the max data length allowed */
    p_buf->layer_specific = buff_size - sizeof(BT_HDR) - OBEX_BT_HDR_MIN_OFFSET;
    UINT8 *p_data = (UINT8 *)(p_buf + 1) + p_buf->offset;
    /* byte 0: opcode */
    *p_data++ = info->opcode;
    /* byte 1, 2: packet length, skip, we will update at last */
    UINT8 *p_pkt_len = p_data;
    p_data += 2;

    switch (info->opcode)
    {
    case OBEX_OPCODE_CONNECT:
        /* byte 3: OBEX version number */
        *p_data++ = info->obex_version_number;
        /* byte 4: flags */
        *p_data++ = info->flags;
        /* byte 5, 6: maximum OBEX packet length, recommend to set as our mtu*/
        STORE16BE(p_data, info->max_packet_length);
        pkt_len += 4;
        break;
    case OBEX_OPCODE_SETPATH:
        /* byte 3: flags */
        *p_data++ = info->flags;
        /* byte 4: constants, reserved, must be zero */
        *p_data++ = 0;
        pkt_len += 2;
        break;
    default:
        break;
    }

    STORE16BE(p_pkt_len, pkt_len);
    p_buf->len = pkt_len;
    *out_pkt = p_buf;
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_BuildResponse
**
** Description      Build a response packet with opcode and response and additional
**                  info, packet can by free by osi_free
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_BuildResponse(tOBEX_PARSE_INFO *info, UINT16 buff_size, BT_HDR **out_pkt)
{
    if (buff_size < OBEX_MIN_PACKET_SIZE || info == NULL || out_pkt == NULL) {
        return OBEX_INVALID_PARAM;
    }
    buff_size += sizeof(BT_HDR) + OBEX_BT_HDR_MIN_OFFSET;

    BT_HDR *p_buf= (BT_HDR *)osi_malloc(buff_size);
    if (p_buf == NULL) {
        return OBEX_NO_RESOURCES;
    }

    UINT16 pkt_len = OBEX_MIN_PACKET_SIZE;
    p_buf->offset = OBEX_BT_HDR_MIN_OFFSET;
    /* use layer_specific to store the max data length allowed */
    p_buf->layer_specific = buff_size - sizeof(BT_HDR) - OBEX_BT_HDR_MIN_OFFSET;
    UINT8 *p_data = (UINT8 *)(p_buf + 1) + p_buf->offset;
    /* byte 0: response code */
    *p_data++ = info->response_code;
    /* byte 1, 2: packet length, skip, we will update at last */
    UINT8 *p_pkt_len = p_data;
    p_data += 2;

    /* we need to use opcode to decide the response format */
    switch (info->opcode)
    {
    case OBEX_OPCODE_CONNECT:
        /* byte 3: OBEX version number */
        *p_data++ = info->obex_version_number;
        /* byte 4: flags */
        *p_data++ = info->flags;
        /* byte 5, 6: maximum OBEX packet length, recommend to set as our mtu */
        STORE16BE(p_data, info->max_packet_length);
        pkt_len += 4;
        break;
    default:
        break;
    }

    STORE16BE(p_pkt_len, pkt_len);
    p_buf->len = pkt_len;
    *out_pkt = p_buf;
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_AppendHeader
**
** Description      Append a header to specific packet, packet can be request
**                  or response, the format of header must be valid
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_AppendHeader(BT_HDR *pkt, const UINT8 *header)
{
    if (pkt == NULL || header == NULL) {
        return OBEX_INVALID_PARAM;
    }

    UINT16 header_len = 0;
    UINT8 header_id = *header;
    switch (header_id & OBEX_HEADER_ID_U2B_MASK)
    {
    case OBEX_HEADER_ID_U2B_TYPE1:
    case OBEX_HEADER_ID_U2B_TYPE2:
        header_len = (header[1] << 8) + header[2];
        break;
    case OBEX_HEADER_ID_U2B_TYPE3:
        header_len = 2;
        break;
    case OBEX_HEADER_ID_U2B_TYPE4:
        header_len = 5;
        break;
    default:
        break;
    }
    if (header_len == 0) {
        return OBEX_INVALID_PARAM;
    }

    if (pkt->layer_specific - pkt->len < header_len) {
        /* the packet can not hold this header */
        return OBEX_NO_RESOURCES;
    }
    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    UINT8 *p_start = p_data + pkt->len;
    memcpy(p_start, header, header_len);
    pkt->len += header_len;
    /* point to packet len */
    p_data++;
    STORE16BE(p_data, pkt->len);
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_AppendHeaderRaw
**
** Description      Append a header to specific packet, packet can be request
**                  or response, data not include 2 byte length prefixed
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_AppendHeaderRaw(BT_HDR *pkt, UINT8 header_id, const UINT8 *data, UINT16 data_len)
{
    if (pkt == NULL || data == NULL) {
        return OBEX_INVALID_PARAM;
    }

    UINT16 header_len = 0;
    BOOLEAN store_header_len = FALSE;
    switch (header_id & OBEX_HEADER_ID_U2B_MASK)
    {
    case OBEX_HEADER_ID_U2B_TYPE1:
    case OBEX_HEADER_ID_U2B_TYPE2:
        /* header id + 2 byte length prefixed + data */
        header_len = data_len + 3;
        store_header_len = TRUE;
        break;
    case OBEX_HEADER_ID_U2B_TYPE3:
        header_len = 2;
        if (data_len != 1) {
            return OBEX_INVALID_PARAM;
        }
        break;
    case OBEX_HEADER_ID_U2B_TYPE4:
        header_len = 5;
        if (data_len != 4) {
            return OBEX_INVALID_PARAM;
        }
        break;
    default:
        break;
    }
    if (header_len == 0) {
        return OBEX_INVALID_PARAM;
    }

    if (pkt->layer_specific - pkt->len < header_len) {
        /* the packet can not hold this header */
        return OBEX_NO_RESOURCES;
    }
    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    UINT8 *p_start = p_data + pkt->len;
    /* store header id */
    *p_start++ = header_id;
    if (store_header_len) {
        /* store header length */
        STORE16BE(p_start, header_len);
        p_start+= 2;
    }
    /* store data */
    memcpy(p_start, data, data_len);
    pkt->len += header_len;
    /* point to packet len */
    p_data++;
    STORE16BE(p_data, pkt->len);
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_AppendHeaderSRM
**
** Description      Append a Single Response Mode header
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_AppendHeaderSRM(BT_HDR *pkt, UINT8 value)
{
    return OBEX_AppendHeaderRaw(pkt, OBEX_HEADER_ID_SRM, &value, 1);
}

/*******************************************************************************
**
** Function         OBEX_AppendHeaderSRMP
**
** Description      Append a Single Response Mode Parameters header
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_AppendHeaderSRMP(BT_HDR *pkt, UINT8 value)
{
    return OBEX_AppendHeaderRaw(pkt, OBEX_HEADER_ID_SRM_PARAM, &value, 1);
}

/*******************************************************************************
**
** Function         OBEX_GetPacketFreeSpace
**
** Description      Get the current free space of a packet, use this to check
**                  if a packet can hold a header
**
** Returns          Current free space of a packet, in bytes
**
*******************************************************************************/
UINT16 OBEX_GetPacketFreeSpace(BT_HDR *pkt)
{
    if (pkt == NULL) {
        return 0;
    }
    return pkt->layer_specific - pkt->len;
}

/*******************************************************************************
**
** Function         OBEX_GetPacketLength
**
** Description      Get the current packet length
**
** Returns          Current packet length, in bytes
**
*******************************************************************************/
UINT16 OBEX_GetPacketLength(BT_HDR *pkt)
{
    if (pkt == NULL) {
        return 0;
    }
    return pkt->len;
}

/*******************************************************************************
**
** Function         OBEX_ParseRequest
**
** Description      Parse a request packet
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_ParseRequest(BT_HDR *pkt, tOBEX_PARSE_INFO *info)
{
    if (pkt == NULL || info == NULL) {
        return OBEX_INVALID_PARAM;
    }

    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    info->opcode = *p_data;
    switch (info->opcode)
    {
    case OBEX_OPCODE_CONNECT:
        info->obex_version_number = p_data[3];
        info->flags = p_data[4];
        info->max_packet_length = (p_data[5] << 8) + p_data[6];
        info->next_header_pos = 7;
        break;
    case OBEX_OPCODE_SETPATH:
        info->flags = p_data[3];
        info->next_header_pos = 5;
        break;
    default:
        info->next_header_pos = 3;
        break;
    }
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_ParseResponse
**
** Description      Parse a request response packet
**
** Returns          OBEX_SUCCESS if successful, otherwise failed
**
*******************************************************************************/
UINT16 OBEX_ParseResponse(BT_HDR *pkt, UINT8 opcode, tOBEX_PARSE_INFO *info)
{
    if (pkt == NULL || info == NULL) {
        return OBEX_INVALID_PARAM;
    }

    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    info->opcode = opcode;
    info->response_code = *p_data;
    switch (opcode)
    {
    case OBEX_OPCODE_CONNECT:
        info->obex_version_number = p_data[3];
        info->flags = p_data[4];
        info->max_packet_length = (p_data[5] << 8) + p_data[6];
        info->next_header_pos = 7;
        break;
    default:
        info->next_header_pos = 3;
        break;
    }
    return OBEX_SUCCESS;
}

/*******************************************************************************
**
** Function         OBEX_CheckFinalBit
**
** Description      Check whether a packet had set the final bit
**
** Returns          TRUE if final bit set, otherwise, false
**
*******************************************************************************/
BOOLEAN OBEX_CheckFinalBit(BT_HDR *pkt)
{
    if (pkt == NULL) {
        return FALSE;
    }
    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    return (*p_data) & OBEX_FINAL_BIT_MASK;
}

/*******************************************************************************
**
** Function         OBEX_CheckContinueResponse
**
** Description      Check whether a packet is continue response
**
** Returns          TRUE if continue response, otherwise, false
**
*******************************************************************************/
BOOLEAN OBEX_CheckContinueResponse(BT_HDR *pkt)
{
    if (pkt == NULL) {
        return FALSE;
    }
    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    return (*p_data == 0x90) || (*p_data == 0x10);
}

/*******************************************************************************
**
** Function         OBEX_GetHeaderLength
**
** Description      Get header length
**
** Returns          header length
**
*******************************************************************************/
UINT16 OBEX_GetHeaderLength(UINT8 *header)
{
    UINT16 header_len = 0;
    UINT8 header_id = *header;
    switch (header_id & OBEX_HEADER_ID_U2B_MASK)
    {
    case OBEX_HEADER_ID_U2B_TYPE1:
    case OBEX_HEADER_ID_U2B_TYPE2:
        header_len = (header[1] << 8) + header[2];
        break;
    case OBEX_HEADER_ID_U2B_TYPE3:
        header_len = 2;
        break;
    case OBEX_HEADER_ID_U2B_TYPE4:
        header_len = 5;
        break;
    default:
        /* unreachable */
        break;
    }
    return header_len;
}

/*******************************************************************************
**
** Function         OBEX_GetNextHeader
**
** Description      Get next header pointer from a packet
**
** Returns          Pointer to start address of a header, NULL if no more header
**                  or failed
**
*******************************************************************************/
UINT8 *OBEX_GetNextHeader(BT_HDR *pkt, tOBEX_PARSE_INFO *info)
{
    if (pkt == NULL || info == NULL) {
        return NULL;
    }
    UINT8 *p_data = (UINT8 *)(pkt + 1) + pkt->offset;
    if (info->next_header_pos == 0 || info->next_header_pos >= pkt->len) {
        return NULL;
    }
    UINT8 *header = p_data + info->next_header_pos;
    UINT16 header_len = OBEX_GetHeaderLength(header);
    info->next_header_pos += header_len;
    return header;
}

#endif /* #if (OBEX_INCLUDED == TRUE) */
