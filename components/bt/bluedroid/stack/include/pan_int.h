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
 *  This file contains internally used PAN definitions
 *
 ******************************************************************************/

#ifndef  PAN_INT_H
#define  PAN_INT_H

#include "pan_api.h"

/*
** This role is used to shutdown the profile. Used internally
** Applications should call PAN_Deregister to shutdown the profile
*/
#define PAN_ROLE_INACTIVE      0

/* Protocols supported by the host internal stack, are registered with SDP */
#define PAN_PROTOCOL_IP        0x0800
#define PAN_PROTOCOL_ARP       0x0806

#define PAN_PROFILE_VERSION    0x0100   /* Version 1.00 */

/* Define the PAN Connection Control Block
*/
typedef struct
{
#define PAN_STATE_IDLE              0
#define PAN_STATE_CONN_START        1
#define PAN_STATE_CONNECTED         2
    UINT8             con_state;

#define PAN_FLAGS_CONN_COMPLETED    0x01
    UINT8             con_flags;

    UINT16            handle;
    BD_ADDR           rem_bda;

    UINT16            bad_pkts_rcvd;
    UINT16            src_uuid;
    UINT16            dst_uuid;
    UINT16            prv_src_uuid;
    UINT16            prv_dst_uuid;
    UINT16            ip_addr_known;
    UINT32            ip_addr;

} tPAN_CONN;


/*  The main PAN control block
*/
typedef struct
{
    UINT8                       role;
    UINT8                       active_role;
    UINT8                       prv_active_role;
    tPAN_CONN                   pcb[MAX_PAN_CONNS];

    tPAN_CONN_STATE_CB          *pan_conn_state_cb;     /* Connection state callback */
    tPAN_BRIDGE_REQ_CB          *pan_bridge_req_cb;
    tPAN_DATA_IND_CB            *pan_data_ind_cb;
    tPAN_DATA_BUF_IND_CB        *pan_data_buf_ind_cb;
    tPAN_FILTER_IND_CB          *pan_pfilt_ind_cb;      /* protocol filter indication callback */
    tPAN_MFILTER_IND_CB         *pan_mfilt_ind_cb;      /* multicast filter indication callback */
    tPAN_TX_DATA_FLOW_CB        *pan_tx_data_flow_cb;

    char                        *user_service_name;
    char                        *gn_service_name;
    char                        *nap_service_name;
    UINT32                      pan_user_sdp_handle;
    UINT32                      pan_gn_sdp_handle;
    UINT32                      pan_nap_sdp_handle;
    UINT8                       num_conns;
    UINT8                       trace_level;
} tPAN_CB;


#ifdef __cplusplus
extern "C" {
#endif

/* Global PAN data
*/
#if PAN_DYNAMIC_MEMORY == FALSE
extern tPAN_CB  pan_cb;
#else
extern tPAN_CB  *pan_cb_ptr;
#define pan_cb (*pan_cb_ptr)
#endif

/*******************************************************************************/
extern void pan_register_with_bnep (void);
extern void pan_conn_ind_cb (UINT16 handle,
                             BD_ADDR p_bda,
                             tBT_UUID *remote_uuid,
                             tBT_UUID *local_uuid,
                             BOOLEAN is_role_change);
extern void pan_connect_state_cb (UINT16 handle, BD_ADDR rem_bda, tBNEP_RESULT result, BOOLEAN is_role_change);
extern void pan_data_ind_cb (UINT16 handle,
                             UINT8 *src,
                             UINT8 *dst,
                             UINT16 protocol,
                             UINT8 *p_data,
                             UINT16 len,
                             BOOLEAN fw_ext_present);
extern void pan_data_buf_ind_cb (UINT16 handle,
                                 UINT8 *src,
                                 UINT8 *dst,
                                 UINT16 protocol,
                                 BT_HDR *p_buf,
                                 BOOLEAN ext);
extern void pan_tx_data_flow_cb (UINT16 handle,
                            tBNEP_RESULT  event);
void pan_proto_filt_ind_cb (UINT16 handle,
                            BOOLEAN indication,
                            tBNEP_RESULT result,
                            UINT16 num_filters,
                            UINT8 *p_filters);
void pan_mcast_filt_ind_cb (UINT16 handle,
                            BOOLEAN indication,
                            tBNEP_RESULT result,
                            UINT16 num_filters,
                            UINT8 *p_filters);
extern UINT32 pan_register_with_sdp (UINT16 uuid, UINT8 sec_mask, char *p_name, char *p_desc);
extern tPAN_CONN *pan_allocate_pcb (BD_ADDR p_bda, UINT16 handle);
extern tPAN_CONN *pan_get_pcb_by_handle (UINT16 handle);
extern tPAN_CONN *pan_get_pcb_by_addr (BD_ADDR p_bda);
extern void pan_close_all_connections (void);
extern void pan_release_pcb (tPAN_CONN *p_pcb);
extern void pan_dump_status (void);

/********************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
