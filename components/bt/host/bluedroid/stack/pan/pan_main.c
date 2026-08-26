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
 *  This file contains main functions to support PAN profile
 *  commands and events.
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if (PAN_INCLUDED == TRUE)

#include <string.h>
#include "osi/allocator.h"
#include "stack/bnep_api.h"
#include "bt_common.h"
#include "stack/bt_types.h"
#include "stack/hcidefs.h"
#include "stack/l2c_api.h"
#include "stack/pan_api.h"
#include "pan_int.h"
#include "stack/sdp_api.h"
#include "stack/sdpdefs.h"

tPAN_CB pan_cb;

#define UUID_CONSTANT_PART 12
UINT8 constant_pan_uuid[UUID_CONSTANT_PART] = {
    0, 0, 0x10, 0, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
};

/*******************************************************************************
 *
 * Function         pan_register_with_bnep
 *
 * Description      This function registers PAN profile with BNEP
 *
 * Parameters:      none
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_register_with_bnep(void)
{
    tBNEP_REGISTER reg_info;

    memset(&reg_info, 0, sizeof(tBNEP_REGISTER));

    reg_info.p_conn_ind_cb = pan_conn_ind_cb;
    reg_info.p_conn_state_cb = pan_connect_state_cb;
    reg_info.p_data_buf_cb = pan_data_buf_ind_cb;
    reg_info.p_data_ind_cb = NULL;
    reg_info.p_tx_data_flow_cb = pan_tx_data_flow_cb;
    reg_info.p_filter_ind_cb = pan_proto_filt_ind_cb;
    reg_info.p_mfilter_ind_cb = pan_mcast_filt_ind_cb;

    BNEP_Register(&reg_info);
}

/*******************************************************************************
 *
 * Function         pan_conn_ind_cb
 *
 * Description      This function is registered with BNEP as connection
 *                  indication callback. BNEP will call this when there is
 *                  connection request from the peer. PAN should call
 *                  BNEP_ConnectResp to indicate whether to accept the
 *                  connection or reject
 *
 * Parameters:      handle      - handle for the connection
 *                  p_bda       - BD Addr of the peer requesting the connection
 *                  remote_uuid     - UUID of the source role (peer device role)
 *                  local_uuid      - UUID of the destination role (local device
 *                                                                  role)
 *                  is_role_change  - Flag to indicate that it is a role change
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_conn_ind_cb(UINT16 handle, BD_ADDR p_bda,
                     tBT_UUID *remote_uuid, tBT_UUID *local_uuid,
                     BOOLEAN is_role_change)
{
    tPAN_CONN *pcb;
    UINT8 req_role;
    BOOLEAN wrong_uuid;

    /*
    ** If we are in GN or NAP role and have one or more
    ** active connections and the received connection is
    ** for user role reject it.
    ** If we are in user role with one connection active
    ** reject the connection.
    ** Allocate PCB and store the parameters
    ** Make bridge request to the host system if connection
    ** is for NAP
    */
    wrong_uuid = FALSE;
    if (remote_uuid->len == 16) {
        /*
        ** If the UUID is 16 bytes first two bytes should be zeros
        ** and last 12 bytes should match the spec defined constant value
        */
        if (memcmp(constant_pan_uuid, remote_uuid->uu.uuid128 + 4,
                   UUID_CONSTANT_PART)) {
            wrong_uuid = TRUE;
        }

        if (remote_uuid->uu.uuid128[0] || remote_uuid->uu.uuid128[1]) {
            wrong_uuid = TRUE;
        }

        /* Extract the 16 bit equivalent of the UUID */
        remote_uuid->uu.uuid16 = (UINT16)((remote_uuid->uu.uuid128[2] << 8) |
                                          remote_uuid->uu.uuid128[3]);
        remote_uuid->len = 2;
    }
    if (remote_uuid->len == 4) {
        /* First two bytes should be zeros */
        if (remote_uuid->uu.uuid32 & 0xFFFF0000) {
            wrong_uuid = true;
        }

        remote_uuid->uu.uuid16 = (UINT16)remote_uuid->uu.uuid32;
        remote_uuid->len = 2;
    }

    if (wrong_uuid) {
        PAN_TRACE_ERROR("PAN Connection failed because of wrong remote UUID ");
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_SRC_UUID);
        return;
    }

    wrong_uuid = FALSE;
    if (local_uuid->len == 16) {
        /*
        ** If the UUID is 16 bytes first two bytes should be zeros
        ** and last 12 bytes should match the spec defined constant value
        */
        if (memcmp(constant_pan_uuid, local_uuid->uu.uuid128 + 4,
                   UUID_CONSTANT_PART)) {
            wrong_uuid = TRUE;
        }

        if (local_uuid->uu.uuid128[0] || local_uuid->uu.uuid128[1]) {
            wrong_uuid = TRUE;
        }

        /* Extract the 16 bit equivalent of the UUID */
        local_uuid->uu.uuid16 = (UINT16)((local_uuid->uu.uuid128[2] << 8) |
                                         local_uuid->uu.uuid128[3]);
        local_uuid->len = 2;
    }
    if (local_uuid->len == 4) {
        /* First two bytes should be zeros */
        if (local_uuid->uu.uuid32 & 0xFFFF0000) {
            wrong_uuid = true;
        }

        local_uuid->uu.uuid16 = (UINT16)local_uuid->uu.uuid32;
        local_uuid->len = 2;
    }

    if (wrong_uuid) {
        PAN_TRACE_ERROR("PAN Connection failed because of wrong local UUID ");
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_DST_UUID);
        return;
    }

    PAN_TRACE_EVENT(
        "pan_conn_ind_cb - for handle %d, current role %d, dst uuid 0x%x, src "
        "uuid 0x%x, role change %s",
        handle, pan_cb.role, local_uuid->uu.uuid16, remote_uuid->uu.uuid16,
        is_role_change ? "YES" : "NO");
    /* The acceptable UUID size is only 2 */
    if (remote_uuid->len != 2) {
        PAN_TRACE_ERROR("PAN Connection failed because of wrong UUID size %d",
                        remote_uuid->len);
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_UUID_SIZE);
        return;
    }

    /* Check if the source UUID is a valid one */
    if (remote_uuid->uu.uuid16 != UUID_SERVCLASS_PANU &&
            remote_uuid->uu.uuid16 != UUID_SERVCLASS_NAP &&
            remote_uuid->uu.uuid16 != UUID_SERVCLASS_GN) {
        PAN_TRACE_ERROR("Src UUID 0x%x is not valid", remote_uuid->uu.uuid16);
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_SRC_UUID);
        return;
    }

    /* Check if the destination UUID is a valid one */
    if (local_uuid->uu.uuid16 != UUID_SERVCLASS_PANU &&
            local_uuid->uu.uuid16 != UUID_SERVCLASS_NAP &&
            local_uuid->uu.uuid16 != UUID_SERVCLASS_GN) {
        PAN_TRACE_ERROR("Dst UUID 0x%x is not valid", local_uuid->uu.uuid16);
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_DST_UUID);
        return;
    }

    /* Check if currently we support the destination role requested */
    if (((!(pan_cb.role & PAN_ROLE_CLIENT)) &&
            local_uuid->uu.uuid16 == UUID_SERVCLASS_PANU) ||
            ((!(pan_cb.role & PAN_ROLE_GN_SERVER)) &&
             local_uuid->uu.uuid16 == UUID_SERVCLASS_GN) ||
            ((!(pan_cb.role & PAN_ROLE_NAP_SERVER)) &&
             local_uuid->uu.uuid16 == UUID_SERVCLASS_NAP)) {
        PAN_TRACE_ERROR(
            "PAN Connection failed because of unsupported destination UUID 0x%x",
            local_uuid->uu.uuid16);
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED_DST_UUID);
        return;
    }

    /* Requested destination role is */
    if (local_uuid->uu.uuid16 == UUID_SERVCLASS_PANU) {
        req_role = PAN_ROLE_CLIENT;
    } else if (local_uuid->uu.uuid16 == UUID_SERVCLASS_GN) {
        req_role = PAN_ROLE_GN_SERVER;
    } else {
        req_role = PAN_ROLE_NAP_SERVER;
    }

    /* If the connection indication is for the existing connection
    ** Check if the new destination role is acceptable
    */
    pcb = pan_get_pcb_by_handle(handle);
    if (pcb) {
        if (pan_cb.num_conns > 1 && local_uuid->uu.uuid16 == UUID_SERVCLASS_PANU) {
            /* There are connections other than this one
            ** so we can't accept PANU role. Reject
            */
            PAN_TRACE_ERROR(
                "Dst UUID should be either GN or NAP only because there are other "
                "connections");
            BNEP_ConnectResp(handle, BNEP_CONN_FAILED_DST_UUID);
            return;
        }

        /* If it is already in connected state check for bridging status */
        if (pcb->con_state == PAN_STATE_CONNECTED) {
            PAN_TRACE_EVENT("PAN Role changing New Src 0x%x Dst 0x%x",
                            remote_uuid->uu.uuid16, local_uuid->uu.uuid16);

            pcb->prv_src_uuid = pcb->src_uuid;
            pcb->prv_dst_uuid = pcb->dst_uuid;

            if (pcb->src_uuid == UUID_SERVCLASS_NAP &&
                    local_uuid->uu.uuid16 != UUID_SERVCLASS_NAP) {
                /* Remove bridging */
                if (pan_cb.pan_bridge_req_cb) {
                    (*pan_cb.pan_bridge_req_cb)(pcb->rem_bda, FALSE);
                }
            }
        }
        /* Set the latest active PAN role */
        pan_cb.active_role = req_role;
        pcb->src_uuid = local_uuid->uu.uuid16;
        pcb->dst_uuid = remote_uuid->uu.uuid16;
        BNEP_ConnectResp(handle, BNEP_SUCCESS);
        return;
    } else {
        /* If this a new connection and destination is PANU role and
        ** we already have a connection then reject the request.
        ** If we have a connection in PANU role then reject it
        */
        if (pan_cb.num_conns && (local_uuid->uu.uuid16 == UUID_SERVCLASS_PANU ||
                                 pan_cb.active_role == PAN_ROLE_CLIENT)) {
            PAN_TRACE_ERROR("PAN already have a connection and can't be user");
            BNEP_ConnectResp(handle, BNEP_CONN_FAILED_DST_UUID);
            return;
        }
    }

    /* This is a new connection */
    PAN_TRACE_DEBUG("New connection indication for handle %d", handle);
    pcb = pan_allocate_pcb(p_bda, handle);
    if (!pcb) {
        PAN_TRACE_ERROR("PAN no control block for new connection");
        BNEP_ConnectResp(handle, BNEP_CONN_FAILED);
        return;
    }

    PAN_TRACE_EVENT("PAN connection destination UUID is 0x%x",
                    local_uuid->uu.uuid16);
    /* Set the latest active PAN role */
    pan_cb.active_role = req_role;
    pcb->src_uuid = local_uuid->uu.uuid16;
    pcb->dst_uuid = remote_uuid->uu.uuid16;
    pcb->con_state = PAN_STATE_CONN_START;
    pan_cb.num_conns++;

    BNEP_ConnectResp(handle, BNEP_SUCCESS);
    return;
}

/*******************************************************************************
 *
 * Function         pan_connect_state_cb
 *
 * Description      This function is registered with BNEP as connection state
 *                  change callback. BNEP will call this when the connection
 *                  is established successfully or terminated
 *
 * Parameters:      handle  - handle for the connection given in the connection
 *                            indication callback
 *                  rem_bda - remote device bd addr
 *                  result  - indicates whether the connection is up or down
 *                            BNEP_SUCCESS if the connection is up all other
 *                            values indicate appropriate errors.
 *                  is_role_change - flag to indicate that it is a role change
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_connect_state_cb(UINT16 handle, BD_ADDR rem_bda,
                          tBNEP_RESULT result, BOOLEAN is_role_change)
{
    tPAN_CONN *pcb;
    UINT8 peer_role;

    PAN_TRACE_EVENT("pan_connect_state_cb - for handle %d, result %d", handle,
                    result);
    pcb = pan_get_pcb_by_handle(handle);
    if (!pcb) {
        PAN_TRACE_ERROR("PAN State change indication for wrong handle %d", handle);
        return;
    }

    /* If the connection is getting terminated remove bridging */
    if (result != BNEP_SUCCESS) {
        /* Inform the application that connection is down */
        if (pan_cb.pan_conn_state_cb)
            (*pan_cb.pan_conn_state_cb)(pcb->handle, pcb->rem_bda, result,
                                        is_role_change, PAN_ROLE_INACTIVE,
                                        PAN_ROLE_INACTIVE);

        /* Check if this failure is for role change only */
        if (pcb->con_state != PAN_STATE_CONNECTED &&
                (pcb->con_flags & PAN_FLAGS_CONN_COMPLETED)) {
            /* restore the original values */
            PAN_TRACE_EVENT("restoring the connection state to active");
            pcb->con_state = PAN_STATE_CONNECTED;
            pcb->con_flags &= (~PAN_FLAGS_CONN_COMPLETED);

            pcb->src_uuid = pcb->prv_src_uuid;
            pcb->dst_uuid = pcb->prv_dst_uuid;
            pan_cb.active_role = pan_cb.prv_active_role;

            if ((pcb->src_uuid == UUID_SERVCLASS_NAP) && pan_cb.pan_bridge_req_cb) {
                (*pan_cb.pan_bridge_req_cb)(pcb->rem_bda, TRUE);
            }

            return;
        }

        if (pcb->con_state == PAN_STATE_CONNECTED) {
            /* If the connections destination role is NAP remove bridging */
            if ((pcb->src_uuid == UUID_SERVCLASS_NAP) && pan_cb.pan_bridge_req_cb) {
                (*pan_cb.pan_bridge_req_cb)(pcb->rem_bda, FALSE);
            }
        }

        pan_cb.num_conns--;
        pan_release_pcb(pcb);
        return;
    }

    /* Requested destination role is */
    if (pcb->src_uuid == UUID_SERVCLASS_PANU) {
        pan_cb.active_role = PAN_ROLE_CLIENT;
    } else if (pcb->src_uuid == UUID_SERVCLASS_GN) {
        pan_cb.active_role = PAN_ROLE_GN_SERVER;
    } else {
        pan_cb.active_role = PAN_ROLE_NAP_SERVER;
    }

    if (pcb->dst_uuid == UUID_SERVCLASS_PANU) {
        peer_role = PAN_ROLE_CLIENT;
    } else if (pcb->dst_uuid == UUID_SERVCLASS_GN) {
        peer_role = PAN_ROLE_GN_SERVER;
    } else {
        peer_role = PAN_ROLE_NAP_SERVER;
    }

    pcb->con_state = PAN_STATE_CONNECTED;

    /* Inform the application that connection is down */
    if (pan_cb.pan_conn_state_cb)
        (*pan_cb.pan_conn_state_cb)(pcb->handle, pcb->rem_bda, PAN_SUCCESS,
                                    is_role_change, pan_cb.active_role, peer_role);

    /* Create bridge if the destination role is NAP */
    if (pan_cb.pan_bridge_req_cb && pcb->src_uuid == UUID_SERVCLASS_NAP) {
        PAN_TRACE_EVENT("PAN requesting for bridge");
        (*pan_cb.pan_bridge_req_cb)(pcb->rem_bda, TRUE);
    }
}

/*******************************************************************************
 *
 * Function         pan_data_ind_cb
 *
 * Description      This function is registered with BNEP as data indication
 *                  callback. BNEP will call this when the peer sends any data
 *                  on this connection
 *
 * Parameters:      handle      - handle for the connection
 *                  src         - source BD Addr
 *                  dst         - destination BD Addr
 *                  protocol    - Network protocol of the Eth packet
 *                  p_data      - pointer to the data
 *                  len         - length of the data
 *                  fw_ext_present - to indicate whether the data contains any
 *                                         extension headers before the payload
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_data_ind_cb(UINT16 handle, BD_ADDR src,
                     BD_ADDR dst, UINT16 protocol, UINT8 *p_data,
                     UINT16 len, BOOLEAN ext)
{
    tPAN_CONN *pcb;
    UINT16 i;
    BOOLEAN forward;

    /*
    ** Check the connection status
    ** If the destination address is MAC broadcast send on all links
    ** except on the one received
    ** If the destination uuid is for NAP send to host system also
    ** If the destination address is one of the devices connected
    ** send the packet to over that link
    ** If the destination address is unknown and destination uuid is NAP
    ** send it to the host system
    */

    PAN_TRACE_EVENT("pan_data_ind_cb - for handle %d", handle);
    pcb = pan_get_pcb_by_handle(handle);
    if (!pcb) {
        PAN_TRACE_ERROR("PAN Data indication for wrong handle %d", handle);
        return;
    }

    if (pcb->con_state != PAN_STATE_CONNECTED) {
        PAN_TRACE_ERROR("PAN Data indication in wrong state %d for handle %d",
                        pcb->con_state, handle);
        return;
    }

    /* Check if it is broadcast packet */
    if (dst[0] & 0x01) {
        PAN_TRACE_DEBUG("PAN received broadcast packet on handle %d, src uuid 0x%x",
                        handle, pcb->src_uuid);
        for (i = 0; i < MAX_PAN_CONNS; i++) {
            if (pan_cb.pcb[i].con_state == PAN_STATE_CONNECTED &&
                    pan_cb.pcb[i].handle != handle &&
                    pcb->src_uuid == pan_cb.pcb[i].src_uuid) {
                BNEP_Write(pan_cb.pcb[i].handle, dst, p_data, len, protocol, src, ext);
            }
        }

        if (pan_cb.pan_data_ind_cb)
            (*pan_cb.pan_data_ind_cb)(pcb->handle, src, dst, protocol, p_data, len,
                                      ext, TRUE);

        return;
    }

    /* Check if it is for any other PAN connection */
    for (i = 0; i < MAX_PAN_CONNS; i++) {
        if (pan_cb.pcb[i].con_state == PAN_STATE_CONNECTED &&
                pcb->src_uuid == pan_cb.pcb[i].src_uuid) {
            if (!bdcmp(pan_cb.pcb[i].rem_bda, dst)) {
                BNEP_Write(pan_cb.pcb[i].handle, dst, p_data, len, protocol, src, ext);
                return;
            }
        }
    }

    if (pcb->src_uuid == UUID_SERVCLASS_NAP) {
        forward = TRUE;
    } else {
        forward = FALSE;
    }

    /* Send it over the LAN or give it to host software */
    if (pan_cb.pan_data_ind_cb)
        (*pan_cb.pan_data_ind_cb)(pcb->handle, src, dst, protocol, p_data, len, ext,
                                  forward);

    return;
}

/*******************************************************************************
 *
 * Function         pan_data_buf_ind_cb
 *
 * Description      This function is registered with BNEP as data buffer
 *                  indication callback. BNEP will call this when the peer sends
 *                  any data on this connection. PAN is responsible to release
 *                  the buffer
 *
 * Parameters:      handle      - handle for the connection
 *                  src         - source BD Addr
 *                  dst         - destination BD Addr
 *                  protocol    - Network protocol of the Eth packet
 *                  p_buf       - pointer to the data buffer
 *                  ext         - to indicate whether the data contains any
 *                                         extension headers before the payload
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_data_buf_ind_cb(UINT16 handle, BD_ADDR src,
                         BD_ADDR dst, UINT16 protocol,
                         BT_HDR *p_buf, BOOLEAN ext)
{
    tPAN_CONN *pcb, *dst_pcb;
    tBNEP_RESULT result;
    UINT16 i, len;
    UINT8 *p_data;
    BOOLEAN forward = FALSE;

    /* Check if the connection is in right state */
    pcb = pan_get_pcb_by_handle(handle);
    if (!pcb) {
        PAN_TRACE_ERROR("PAN Data buffer indication for wrong handle %d", handle);
        osi_free(p_buf);
        return;
    }

    if (pcb->con_state != PAN_STATE_CONNECTED) {
        PAN_TRACE_ERROR("PAN Data indication in wrong state %d for handle %d",
                        pcb->con_state, handle);
        osi_free(p_buf);
        return;
    }

    p_data = (UINT8 *)(p_buf + 1) + p_buf->offset;
    len = p_buf->len;

    PAN_TRACE_EVENT(
        "pan_data_buf_ind_cb - for handle %d, protocol 0x%x, length %d, ext %d",
        handle, protocol, len, ext);

    if (pcb->src_uuid == UUID_SERVCLASS_NAP) {
        forward = TRUE;
    } else {
        forward = FALSE;
    }

    /* Check if it is broadcast or multicast packet */
    if (pcb->src_uuid != UUID_SERVCLASS_PANU) {
        if (dst[0] & 0x01) {
            PAN_TRACE_DEBUG(
                "PAN received broadcast packet on handle %d, src uuid 0x%x", handle,
                pcb->src_uuid);
            for (i = 0; i < MAX_PAN_CONNS; i++) {
                if (pan_cb.pcb[i].con_state == PAN_STATE_CONNECTED &&
                        pan_cb.pcb[i].handle != handle &&
                        pcb->src_uuid == pan_cb.pcb[i].src_uuid) {
                    BNEP_Write(pan_cb.pcb[i].handle, dst, p_data, len, protocol, src,
                               ext);
                }
            }

            if (pan_cb.pan_data_buf_ind_cb)
                (*pan_cb.pan_data_buf_ind_cb)(pcb->handle, src, dst, protocol, p_buf,
                                              ext, forward);
            else if (pan_cb.pan_data_ind_cb) {
                (*pan_cb.pan_data_ind_cb)(pcb->handle, src, dst, protocol, p_data, len,
                                          ext, forward);
                osi_free(p_buf);
            } else {
                osi_free(p_buf);
            }

            return;
        }

        /* Check if it is for any other PAN connection */
        dst_pcb = pan_get_pcb_by_addr(dst);
        if (dst_pcb) {
            PAN_TRACE_EVENT(
                "%s - destination PANU found on handle %d and sending data, len: %d",
                __func__, dst_pcb->handle, len);

            result =
                BNEP_Write(dst_pcb->handle, dst, p_data, len, protocol, src, ext);
            if (result != BNEP_SUCCESS && result != BNEP_IGNORE_CMD)
                PAN_TRACE_ERROR("Failed to write data for PAN connection handle %d",
                                dst_pcb->handle);
            osi_free(p_buf);
            return;
        }
    }

    /* Send it over the LAN or give it to host software */
    if (pan_cb.pan_data_buf_ind_cb)
        (*pan_cb.pan_data_buf_ind_cb)(pcb->handle, src, dst, protocol, p_buf, ext,
                                      forward);
    else if (pan_cb.pan_data_ind_cb) {
        (*pan_cb.pan_data_ind_cb)(pcb->handle, src, dst, protocol, p_data, len, ext,
                                  forward);
        osi_free(p_buf);
    } else {
        osi_free(p_buf);
    }

    return;
}

/*******************************************************************************
 *
 * Function         pan_proto_filt_ind_cb
 *
 * Description      This function is registered with BNEP to receive tx data
 *          flow status
 *
 * Parameters:      handle      - handle for the connection
 *          event       - flow status
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_tx_data_flow_cb(UINT16 handle, tBNEP_RESULT event)
{
    if (pan_cb.pan_tx_data_flow_cb) {
        (*pan_cb.pan_tx_data_flow_cb)(handle, event);
    }

    return;
}

/*******************************************************************************
 *
 * Function         pan_proto_filt_ind_cb
 *
 * Description      This function is registered with BNEP as proto filter
 *                  indication callback. BNEP will call this when the peer sends
 *                  any protocol filter set for the connection or to indicate
 *                  the result of the protocol filter set by the local device
 *
 * Parameters:      handle      - handle for the connection
 *                  indication  - true if this is indication
 *                                false if it is called to give the result of
 *                                      local device protocol filter set
 *                  result      - This gives the result of the filter set
 *                                      operation
 *                  num_filters - number of filters set by the peer device
 *                  p_filters   - pointer to the filters set by the peer device
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_proto_filt_ind_cb(UINT16 handle, BOOLEAN indication,
                           tBNEP_RESULT result, UINT16 num_filters,
                           UINT8 *p_filters)
{
    PAN_TRACE_EVENT(
        "pan_proto_filt_ind_cb - called for handle %d with ind %d, result %d, "
        "num %d",
        handle, indication, result, num_filters);

    if (pan_cb.pan_pfilt_ind_cb)
        (*pan_cb.pan_pfilt_ind_cb)(handle, indication, (tPAN_RESULT)result,
                                   num_filters, p_filters);
}

/*******************************************************************************
 *
 * Function         pan_mcast_filt_ind_cb
 *
 * Description      This function is registered with BNEP as mcast filter
 *                  indication callback. BNEP will call this when the peer sends
 *                  any multicast filter set for the connection or to indicate
 *                  the result of the multicast filter set by the local device
 *
 * Parameters:      handle      - handle for the connection
 *                  indication  - true if this is indication
 *                                false if it is called to give the result of
 *                                      local device multicast filter set
 *                  result      - This gives the result of the filter set
 *                                operation
 *                  num_filters - number of filters set by the peer device
 *                  p_filters   - pointer to the filters set by the peer device
 *
 * Returns          none
 *
 ******************************************************************************/
void pan_mcast_filt_ind_cb(UINT16 handle, BOOLEAN indication,
                           tBNEP_RESULT result, UINT16 num_filters,
                           UINT8 *p_filters)
{
    PAN_TRACE_EVENT(
        "pan_mcast_filt_ind_cb - called for handle %d with ind %d, result %d, "
        "num %d",
        handle, indication, result, num_filters);

    if (pan_cb.pan_mfilt_ind_cb)
        (*pan_cb.pan_mfilt_ind_cb)(handle, indication, (tPAN_RESULT)result,
                                   num_filters, p_filters);
}

#endif /* (PAN_INCLUDED == TRUE) */
