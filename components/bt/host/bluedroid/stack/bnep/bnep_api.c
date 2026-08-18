/******************************************************************************
 *
 *  Copyright (C) 2001-2012 Broadcom Corporation
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
 *  This file contains the BNEP API code
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if (BNEP_INCLUDED == TRUE)

#include <string.h>

#include "osi/allocator.h"
#include "osi/fixed_queue.h"

#include "stack/bnep_api.h"
#include "bnep_int.h"

extern fixed_queue_t* btu_general_alarm_queue;

/*******************************************************************************
 *
 * Function         BNEP_Init
 *
 * Description      This function initializes the BNEP unit. It should be called
 *                  before accessing any other APIs to initialize the control
 *                  block.
 *
 * Returns          void
 *
 ******************************************************************************/
void BNEP_Init(void)
{
    memset(&bnep_cb, 0, sizeof(tBNEP_CB));

#if defined(BNEP_INITIAL_TRACE_LEVEL)
    bnep_cb.trace_level = BNEP_INITIAL_TRACE_LEVEL;
#else
    bnep_cb.trace_level = BT_TRACE_LEVEL_NONE; /* No traces */
#endif
}

/*******************************************************************************
 *
 * Function         BNEP_Register
 *
 * Description      This function is called by the upper layer to register
 *                  its callbacks with BNEP
 *
 * Parameters:      p_reg_info - contains all callback function pointers
 *
 *
 * Returns          BNEP_SUCCESS        if registered successfully
 *                  BNEP_FAILURE        if connection state callback is missing
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_Register(tBNEP_REGISTER *p_reg_info)
{
    /* There should be connection state call back registered */
    if ((!p_reg_info) || (!(p_reg_info->p_conn_state_cb))) {
        return BNEP_SECURITY_FAIL;
    }

    bnep_cb.p_conn_ind_cb = p_reg_info->p_conn_ind_cb;
    bnep_cb.p_conn_state_cb = p_reg_info->p_conn_state_cb;
    bnep_cb.p_data_ind_cb = p_reg_info->p_data_ind_cb;
    bnep_cb.p_data_buf_cb = p_reg_info->p_data_buf_cb;
    bnep_cb.p_filter_ind_cb = p_reg_info->p_filter_ind_cb;
    bnep_cb.p_mfilter_ind_cb = p_reg_info->p_mfilter_ind_cb;
    bnep_cb.p_tx_data_flow_cb = p_reg_info->p_tx_data_flow_cb;

    if (bnep_register_with_l2cap()) {
        return BNEP_SECURITY_FAIL;
    }

    bnep_cb.profile_registered = TRUE;
    return BNEP_SUCCESS;
}

/*******************************************************************************
 *
 * Function         BNEP_Deregister
 *
 * Description      This function is called by the upper layer to de-register
 *                  its callbacks.
 *
 * Parameters:      void
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void BNEP_Deregister(void)
{
    /* Clear all the call backs registered */
    bnep_cb.p_conn_ind_cb = NULL;
    bnep_cb.p_conn_state_cb = NULL;
    bnep_cb.p_data_ind_cb = NULL;
    bnep_cb.p_data_buf_cb = NULL;
    bnep_cb.p_filter_ind_cb = NULL;
    bnep_cb.p_mfilter_ind_cb = NULL;
    bnep_cb.p_tx_data_flow_cb = NULL;

    bnep_cb.profile_registered = FALSE;
    L2CA_Deregister(BT_PSM_BNEP);
}

/*******************************************************************************
 *
 * Function         BNEP_Connect
 *
 * Description      This function creates a BNEP connection to a remote
 *                  device.
 *
 * Parameters:      p_rem_addr  - BD_ADDR of the peer
 *                  src_uuid    - source uuid for the connection
 *                  dst_uuid    - destination uuid for the connection
 *                  p_handle    - pointer to return the handle for the
 *                                connection
 *
 * Returns          BNEP_SUCCESS                if connection started
 *                  BNEP_NO_RESOURCES           if no resources
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_Connect(BD_ADDR p_rem_bda, tBT_UUID *src_uuid,
                          tBT_UUID *dst_uuid, UINT16 *p_handle)
{
    UINT16 cid;
    tBNEP_CONN * p_bcb = bnepu_find_bcb_by_bd_addr(p_rem_bda);

    BNEP_TRACE_DEBUG("%s BDA:%02x:%02x:%02x:%02x:%02x:%02x", __func__,
                     p_rem_bda[0], p_rem_bda[1], p_rem_bda[2], p_rem_bda[3],
                     p_rem_bda[4], p_rem_bda[5]);

    if (!bnep_cb.profile_registered) {
        return BNEP_WRONG_STATE;
    }

    /* Both source and destination UUID lengths should be same */
    if (src_uuid->len != dst_uuid->len) {
        return BNEP_CONN_FAILED_UUID_SIZE;
    }

    if (!p_bcb) {
        p_bcb = bnepu_allocate_bcb(p_rem_bda);
        if (p_bcb == NULL) {
            return (BNEP_NO_RESOURCES);
        }
    } else if (p_bcb->con_state != BNEP_STATE_CONNECTED) {
        return BNEP_WRONG_STATE;
    } else {
        /* Backup current UUID values to restore if role change fails */
        memcpy((UINT8 *) & (p_bcb->prv_src_uuid), (UINT8 *) & (p_bcb->src_uuid),
               sizeof(tBT_UUID));
        memcpy((UINT8 *) & (p_bcb->prv_dst_uuid), (UINT8 *) & (p_bcb->dst_uuid),
               sizeof(tBT_UUID));
    }

    /* We are the originator of this connection */
    p_bcb->con_flags |= BNEP_FLAGS_IS_ORIG;

    memcpy((UINT8*) & (p_bcb->src_uuid), (UINT8*)src_uuid, sizeof(tBT_UUID));
    memcpy((UINT8*) & (p_bcb->dst_uuid), (UINT8*)dst_uuid, sizeof(tBT_UUID));

    if (p_bcb->con_state == BNEP_STATE_CONNECTED) {
        /* Transition to the next appropriate state, waiting for connection confirm.
         */
        p_bcb->con_state = BNEP_STATE_SEC_CHECKING;

        BNEP_TRACE_API("BNEP initiating security procedures for src uuid 0x%x",
                       p_bcb->src_uuid.uu.uuid16);

#if (BNEP_DO_AUTH_FOR_ROLE_SWITCH == TRUE)
        btm_sec_mx_access_request(p_bcb->rem_bda, BT_PSM_BNEP, TRUE,
                                  BTM_SEC_PROTO_BNEP, bnep_get_uuid32(src_uuid),
                                  &bnep_sec_check_complete, p_bcb);
#else
        bnep_sec_check_complete(p_bcb->rem_bda, BT_TRANSPORT_BR_EDR, p_bcb,
                                BTM_SUCCESS);
#endif

    } else {
        /* Transition to the next appropriate state, waiting for connection confirm.
         */
        p_bcb->con_state = BNEP_STATE_CONN_START;

        cid = L2CA_ConnectReq(BT_PSM_BNEP, p_bcb->rem_bda);
        if (cid != 0) {
            p_bcb->l2cap_cid = cid;

        } else {
            BNEP_TRACE_ERROR("BNEP - Originate failed");
            if (bnep_cb.p_conn_state_cb)
                (*bnep_cb.p_conn_state_cb)(p_bcb->handle, p_bcb->rem_bda,
                                           BNEP_CONN_FAILED, FALSE);
            bnepu_release_bcb(p_bcb);
            return BNEP_CONN_FAILED;
        }

        /* Start timer waiting for connect */
        btu_start_timer(&p_bcb->conn_timer, BTU_TTYPE_BNEP, BNEP_CONN_TIMEOUT_SEC);
    }

    *p_handle = p_bcb->handle;
    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_ConnectResp
 *
 * Description      This function is called in response to connection indication
 *
 *
 * Parameters:      handle  - handle given in the connection indication
 *                  resp    - response for the connection indication
 *
 * Returns          BNEP_SUCCESS                if connection started
 *                  BNEP_WRONG_HANDLE           if the connection is not found
 *                  BNEP_WRONG_STATE            if the response is not expected
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_ConnectResp(UINT16 handle, tBNEP_RESULT resp)
{
    tBNEP_CONN * p_bcb;
    UINT16 resp_code = BNEP_SETUP_CONN_OK;

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    if (p_bcb->con_state != BNEP_STATE_CONN_SETUP ||
            (!(p_bcb->con_flags & BNEP_FLAGS_SETUP_RCVD))) {
        return (BNEP_WRONG_STATE);
    }

    BNEP_TRACE_API("BNEP_ConnectResp()  for handle %d, response %d", handle,
                   resp);

    /* Form appropriate response based on profile response */
    if (resp == BNEP_CONN_FAILED_SRC_UUID) {
        resp_code = BNEP_SETUP_INVALID_SRC_UUID;
    } else if (resp == BNEP_CONN_FAILED_DST_UUID) {
        resp_code = BNEP_SETUP_INVALID_DEST_UUID;
    } else if (resp == BNEP_CONN_FAILED_UUID_SIZE) {
        resp_code = BNEP_SETUP_INVALID_UUID_SIZE;
    } else if (resp == BNEP_SUCCESS) {
        resp_code = BNEP_SETUP_CONN_OK;
    } else {
        resp_code = BNEP_SETUP_CONN_NOT_ALLOWED;
    }

    bnep_send_conn_responce(p_bcb, resp_code);
    p_bcb->con_flags &= (~BNEP_FLAGS_SETUP_RCVD);

    if (resp == BNEP_SUCCESS) {
        bnep_connected(p_bcb);
    } else if (p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED) {
        /* Restore the original parameters */
        p_bcb->con_state = BNEP_STATE_CONNECTED;
        p_bcb->con_flags &= (~BNEP_FLAGS_SETUP_RCVD);

        memcpy((UINT8*) & (p_bcb->src_uuid), (UINT8*) & (p_bcb->prv_src_uuid),
               sizeof(tBT_UUID));
        memcpy((UINT8*) & (p_bcb->dst_uuid), (UINT8*) & (p_bcb->prv_dst_uuid),
               sizeof(tBT_UUID));
    }

    /* Process remaining part of the setup message (extension headers) */
    if (p_bcb->p_pending_data) {
        UINT8 extension_present = TRUE, *p, ext_type;
        UINT16 rem_len;

        rem_len = p_bcb->p_pending_data->len;
        p = (UINT8*)(p_bcb->p_pending_data + 1) + p_bcb->p_pending_data->offset;
        while (extension_present && p && rem_len) {
            ext_type = *p++;
            rem_len--;
            extension_present = ext_type >> 7;
            ext_type &= 0x7F;

            /* if unknown extension present stop processing */
            if (ext_type) {
                break;
            }

            p = bnep_process_control_packet(p_bcb, p, &rem_len, TRUE);
        }

        osi_free(p_bcb->p_pending_data);
        p_bcb->p_pending_data = NULL;
    }
    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_Disconnect
 *
 * Description      This function is called to close the specified connection.
 *
 * Parameters:      handle   - handle of the connection
 *
 * Returns          BNEP_SUCCESS                if connection is disconnected
 *                  BNEP_WRONG_HANDLE           if no connection is not found
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_Disconnect(UINT16 handle)
{
    tBNEP_CONN * p_bcb;

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    if (p_bcb->con_state == BNEP_STATE_IDLE) {
        return (BNEP_WRONG_HANDLE);
    }

    BNEP_TRACE_API("BNEP_Disconnect()  for handle %d", handle);

    L2CA_DisconnectReq(p_bcb->l2cap_cid);

    bnepu_release_bcb(p_bcb);

    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_WriteBuf
 *
 * Description      This function sends data in a GKI buffer on BNEP connection
 *
 * Parameters:      handle       - handle of the connection to write
 *                  p_dest_addr  - BD_ADDR/Ethernet addr of the destination
 *                  p_buf        - pointer to address of buffer with data
 *                  protocol     - protocol type of the packet
 *                  p_src_addr   - (optional) BD_ADDR/ethernet address of the
 *                                 source
 *                                 (should be NULL if it is local BD Addr)
 *                  fw_ext_present - forwarded extensions present
 *
 * Returns:         BNEP_WRONG_HANDLE       - if passed handle is not valid
 *                  BNEP_WRONG_STATE        - if connection is not active
 *                  BNEP_MTU_EXCEDED        - If the data length is greater than
 *                                            the MTU
 *                  BNEP_IGNORE_CMD         - If the packet is filtered out
 *                  BNEP_Q_SIZE_EXCEEDED    - If the Tx Q is full
 *                  BNEP_SUCCESS            - If written successfully
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_WriteBuf(UINT16 handle, BD_ADDR p_dest_addr,
                           BT_HDR *p_buf, UINT16 protocol,
                           BD_ADDR p_src_addr, BOOLEAN fw_ext_present)
{
    tBNEP_CONN * p_bcb;
    UINT8* p_data;

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        osi_free(p_buf);
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    /* Check the connection state */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        osi_free(p_buf);
        return (BNEP_WRONG_STATE);
    }

    /* Check MTU size */
    if (p_buf->len > BNEP_MTU_SIZE) {
        BNEP_TRACE_ERROR("BNEP_Write() length %d exceeded MTU %d", p_buf->len,
                         BNEP_MTU_SIZE);
        osi_free(p_buf);
        return (BNEP_MTU_EXCEDED);
    }

    /* Check if the packet should be filtered out */
    p_data = (UINT8*)(p_buf + 1) + p_buf->offset;
    if (bnep_is_packet_allowed(p_bcb, p_dest_addr, protocol, fw_ext_present,
                               p_data, p_buf->len) != BNEP_SUCCESS) {
        /*
        ** If packet is filtered and ext headers are present
        ** drop the data and forward the ext headers
        */
        if (fw_ext_present) {
            UINT8 ext, length;
            UINT16 org_len, new_len;
            /* parse the extension headers and find out the new packet len */
            org_len = p_buf->len;
            new_len = 0;
            do {
                if ((UINT16)(org_len - new_len) < 2) {
                    osi_free(p_buf);
                    return BNEP_IGNORE_CMD;
                }
                ext = *p_data++;
                length = *p_data++;
                new_len += 2;
                if ((UINT16)(org_len - new_len) < length) {
                    osi_free(p_buf);
                    return BNEP_IGNORE_CMD;
                }
                p_data += length;
                new_len += length;

            } while (ext & 0x80);

            if (protocol != BNEP_802_1_P_PROTOCOL) {
                protocol = 0;
            } else {
                /* Need 4 bytes for 802.1Q tag + ethertype after extensions */
                if (new_len + 4 > org_len) {
                    osi_free(p_buf);
                    return BNEP_IGNORE_CMD;
                }
                new_len += 4;
                p_data[2] = 0;
                p_data[3] = 0;
            }
            p_buf->len = new_len;
        } else {
            osi_free(p_buf);
            return BNEP_IGNORE_CMD;
        }
    }

    /* Callers must leave BNEP_MINIMUM_OFFSET headroom. In-place payload
     * shifting is unsafe with exact-sized heap buffers (no capacity field). */
    if (p_buf->offset < BNEP_MINIMUM_OFFSET) {
        BNEP_TRACE_ERROR("BNEP_WriteBuf() offset %d < minimum %d",
                         p_buf->offset, BNEP_MINIMUM_OFFSET);
        osi_free(p_buf);
        return (BNEP_INVALID_OFFSET);
    }

    /* Check transmit queue */
    if (fixed_queue_length(p_bcb->xmit_q) >= BNEP_MAX_XMITQ_DEPTH) {
        osi_free(p_buf);
        return (BNEP_Q_SIZE_EXCEEDED);
    }

    /* Build the BNEP header */
    bnepu_build_bnep_hdr(p_bcb, p_buf, protocol, p_src_addr, p_dest_addr,
                         fw_ext_present);

    /* Send the data or queue it up */
    bnepu_check_send_packet(p_bcb, p_buf);

    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_Write
 *
 * Description      This function sends data over a BNEP connection
 *
 * Parameters:      handle       - handle of the connection to write
 *                  p_dest_addr  - BD_ADDR/Ethernet addr of the destination
 *                  p_data       - pointer to data start
 *                  protocol     - protocol type of the packet
 *                  p_src_addr   - (optional) BD_ADDR/ethernet address of the
 *                                 source
 *                                 (should be NULL if it is local BD Addr)
 *                  fw_ext_present - forwarded extensions present
 *
 * Returns:         BNEP_WRONG_HANDLE       - if passed handle is not valid
 *                  BNEP_WRONG_STATE        - if connection is not active
 *                  BNEP_MTU_EXCEDED        - If the data length is greater than
 *                                            the MTU
 *                  BNEP_IGNORE_CMD         - If the packet is filtered out
 *                  BNEP_Q_SIZE_EXCEEDED    - If the Tx Q is full
 *                  BNEP_NO_RESOURCES       - If not able to allocate a buffer
 *                  BNEP_SUCCESS            - If written successfully
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_Write(UINT16 handle, BD_ADDR p_dest_addr,
                        UINT8 *p_data, UINT16 len, UINT16 protocol,
                        BD_ADDR p_src_addr, BOOLEAN fw_ext_present)
{
    tBNEP_CONN * p_bcb;
    UINT8* p;
    /* Offset of 802.1Q ethertype to clear in the private copy; UINT16_MAX = none */
    UINT16 clear_etype_off = 0xFFFF;

    /* Check MTU size. Consider the possibility of having extension headers */
    if (len > BNEP_MTU_SIZE) {
        BNEP_TRACE_ERROR("BNEP_Write() length %d exceeded MTU %d", len,
                         BNEP_MTU_SIZE);
        return (BNEP_MTU_EXCEDED);
    }

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    /* Check the connection state */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        return (BNEP_WRONG_STATE);
    }

    /* Check if the packet should be filtered out */
    if (bnep_is_packet_allowed(p_bcb, p_dest_addr, protocol, fw_ext_present,
                               p_data, len) != BNEP_SUCCESS) {
        /*
        ** If packet is filtered and ext headers are present
        ** drop the data and forward the ext headers
        */
        if (fw_ext_present) {
            UINT8 ext, length;
            UINT16 org_len, new_len;
            /* parse the extension headers and find out the new packet len */
            org_len = len;
            new_len = 0;
            p = p_data;
            do {
                if ((UINT16)(org_len - new_len) < 2) {
                    return BNEP_IGNORE_CMD;
                }
                ext = *p_data++;
                length = *p_data++;
                new_len += 2;
                if ((UINT16)(org_len - new_len) < length) {
                    return BNEP_IGNORE_CMD;
                }
                p_data += length;
                new_len += length;

            } while (ext & 0x80);

            if (protocol != BNEP_802_1_P_PROTOCOL) {
                protocol = 0;
            } else {
                /* Need 4 bytes for 802.1Q tag + ethertype after extensions */
                if (new_len + 4 > org_len) {
                    return BNEP_IGNORE_CMD;
                }
                /* Do not mutate the caller's buffer (may be shared across peers). */
                clear_etype_off = new_len + 2;
                new_len += 4;
            }
            len = new_len;
            p_data = p;
        } else {
            return BNEP_IGNORE_CMD;
        }
    }

    /* Check transmit queue */
    if (fixed_queue_length(p_bcb->xmit_q) >= BNEP_MAX_XMITQ_DEPTH) {
        return (BNEP_Q_SIZE_EXCEEDED);
    }

    /* Get a buffer to copy the data into */
    BT_HDR * p_buf = (BT_HDR *)osi_malloc(BNEP_BUF_SIZE);
    if (p_buf == NULL) {
        BNEP_TRACE_ERROR("%s: no buffer", __func__);
        return (BNEP_NO_RESOURCES);
    }

    p_buf->len = len;
    p_buf->offset = BNEP_MINIMUM_OFFSET;
    p = (UINT8*)(p_buf + 1) + BNEP_MINIMUM_OFFSET;

    memcpy(p, p_data, len);
    if (clear_etype_off != 0xFFFF) {
        p[clear_etype_off] = 0;
        p[clear_etype_off + 1] = 0;
    }

    /* Build the BNEP header */
    bnepu_build_bnep_hdr(p_bcb, p_buf, protocol, p_src_addr, p_dest_addr,
                         fw_ext_present);

    /* Send the data or queue it up */
    bnepu_check_send_packet(p_bcb, p_buf);

    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_SetProtocolFilters
 *
 * Description      This function sets the protocol filters on peer device
 *
 * Parameters:      handle        - Handle for the connection
 *                  num_filters   - total number of filter ranges
 *                  p_start_array - Array of beginnings of all protocol ranges
 *                  p_end_array   - Array of ends of all protocol ranges
 *
 * Returns          BNEP_WRONG_HANDLE           - if the connection handle is
 *                                                not valid
 *                  BNEP_SET_FILTER_FAIL        - if the connection is in wrong
 *                                                state
 *                  BNEP_TOO_MANY_FILTERS       - if too many filters
 *                  BNEP_SUCCESS                - if request sent successfully
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_SetProtocolFilters(UINT16 handle, UINT16 num_filters,
                                     UINT16 *p_start_array,
                                     UINT16 *p_end_array)
{
    UINT16 xx;
    tBNEP_CONN * p_bcb;

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    /* Check the connection state */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        return (BNEP_WRONG_STATE);
    }

    /* Validate the parameters */
    if (num_filters && (!p_start_array || !p_end_array)) {
        return (BNEP_SET_FILTER_FAIL);
    }

    if (num_filters > BNEP_MAX_PROT_FILTERS) {
        return (BNEP_TOO_MANY_FILTERS);
    }

    /* Fill the filter values in connection block */
    for (xx = 0; xx < num_filters; xx++) {
        p_bcb->sent_prot_filter_start[xx] = *p_start_array++;
        p_bcb->sent_prot_filter_end[xx] = *p_end_array++;
    }

    p_bcb->sent_num_filters = num_filters;

    bnepu_send_peer_our_filters(p_bcb);

    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_SetMulticastFilters
 *
 * Description      This function sets the filters for multicast addresses for
 *                  BNEP.
 *
 * Parameters:      handle        - Handle for the connection
 *                  num_filters   - total number of filter ranges
 *                  p_start_array - Pointer to sequence of beginnings of all
 *                                         multicast address ranges
 *                  p_end_array   - Pointer to sequence of ends of all
 *                                         multicast address ranges
 *
 * Returns          BNEP_WRONG_HANDLE           - if the connection handle is
 *                                                not valid
 *                  BNEP_SET_FILTER_FAIL        - if the connection is in wrong
 *                                                state
 *                  BNEP_TOO_MANY_FILTERS       - if too many filters
 *                  BNEP_SUCCESS                - if request sent successfully
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_SetMulticastFilters(UINT16 handle, UINT16 num_filters,
                                      UINT8 *p_start_array,
                                      UINT8 *p_end_array)
{
    UINT16 xx;
    tBNEP_CONN * p_bcb;

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    /* Check the connection state */
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        return (BNEP_WRONG_STATE);
    }

    /* Validate the parameters */
    if (num_filters && (!p_start_array || !p_end_array)) {
        return (BNEP_SET_FILTER_FAIL);
    }

    if (num_filters > BNEP_MAX_MULTI_FILTERS) {
        return (BNEP_TOO_MANY_FILTERS);
    }

    /* Fill the multicast filter values in connection block */
    for (xx = 0; xx < num_filters; xx++) {
        memcpy(p_bcb->sent_mcast_filter_start[xx], p_start_array,
               BD_ADDR_LEN);
        memcpy(p_bcb->sent_mcast_filter_end[xx], p_end_array, BD_ADDR_LEN);

        p_start_array += BD_ADDR_LEN;
        p_end_array += BD_ADDR_LEN;
    }

    p_bcb->sent_mcast_filters = num_filters;

    bnepu_send_peer_our_multi_filters(p_bcb);

    return (BNEP_SUCCESS);
}

/*******************************************************************************
 *
 * Function         BNEP_SetTraceLevel
 *
 * Description      This function sets the trace level for BNEP. If called with
 *                  a value of 0xFF, it simply reads the current trace level.
 *
 * Returns          the new (current) trace level
 *
 ******************************************************************************/
UINT8 BNEP_SetTraceLevel(UINT8 new_level)
{
    if (new_level != 0xFF) {
        bnep_cb.trace_level = new_level;
    }

    return (bnep_cb.trace_level);
}

/*******************************************************************************
 *
 * Function         BNEP_GetStatus
 *
 * Description      This function gets the status information for BNEP
 *                  connection
 *
 * Returns          BNEP_SUCCESS            - if the status is available
 *                  BNEP_NO_RESOURCES       - if no structure is passed for
 *                                            output
 *                  BNEP_WRONG_HANDLE       - if the handle is invalid
 *                  BNEP_WRONG_STATE        - if not in connected state
 *
 ******************************************************************************/
tBNEP_RESULT BNEP_GetStatus(UINT16 handle, tBNEP_STATUS *p_status)
{
#if (BNEP_SUPPORTS_STATUS_API == TRUE)
    tBNEP_CONN * p_bcb;

    if (!p_status) {
        return BNEP_NO_RESOURCES;
    }

    if ((!handle) || (handle > BNEP_MAX_CONNECTIONS)) {
        return (BNEP_WRONG_HANDLE);
    }

    p_bcb = &(bnep_cb.bcb[handle - 1]);

    memset(p_status, 0, sizeof(tBNEP_STATUS));
    if ((p_bcb->con_state != BNEP_STATE_CONNECTED) &&
            (!(p_bcb->con_flags & BNEP_FLAGS_CONN_COMPLETED))) {
        return BNEP_WRONG_STATE;
    }

    /* Read the status parameters from the connection control block */
    p_status->con_status = BNEP_STATUS_CONNECTED;
    p_status->l2cap_cid = p_bcb->l2cap_cid;
    p_status->rem_mtu_size = p_bcb->rem_mtu_size;
    p_status->xmit_q_depth = fixed_queue_length(p_bcb->xmit_q);
    p_status->sent_num_filters = p_bcb->sent_num_filters;
    p_status->sent_mcast_filters = p_bcb->sent_mcast_filters;
    p_status->rcvd_num_filters = p_bcb->rcvd_num_filters;
    p_status->rcvd_mcast_filters = p_bcb->rcvd_mcast_filters;

    bdcpy(p_status->rem_bda, p_bcb->rem_bda);
    memcpy(&(p_status->src_uuid), &(p_bcb->src_uuid), sizeof(tBT_UUID));
    memcpy(&(p_status->dst_uuid), &(p_bcb->dst_uuid), sizeof(tBT_UUID));

    return BNEP_SUCCESS;
#else
    return (BNEP_IGNORE_CMD);
#endif
}

#endif /* (BNEP_INCLUDED == TRUE) */
