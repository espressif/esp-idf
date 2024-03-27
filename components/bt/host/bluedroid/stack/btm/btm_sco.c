/******************************************************************************
 *
 *  Copyright (C) 2000-2012 Broadcom Corporation
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
 *  This file contains functions that handle SCO connections. This includes
 *  operations such as connect, disconnect, change supported packet types.
 *
 ******************************************************************************/

#include <string.h>
#include "stack/bt_types.h"
#include "common/bt_target.h"
#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "stack/btm_api.h"
#include "osi/allocator.h"
#include "btm_int.h"
#include "stack/hcidefs.h"
//#include "bt_utils.h"

#if BTM_SCO_INCLUDED == TRUE

/********************************************************************************/
/*                 L O C A L    D A T A    D E F I N I T I O N S                */
/********************************************************************************/

#define SCO_ST_UNUSED           0
#define SCO_ST_LISTENING        1
#define SCO_ST_W4_CONN_RSP      2
#define SCO_ST_CONNECTING       3
#define SCO_ST_CONNECTED        4
#define SCO_ST_DISCONNECTING    5
#define SCO_ST_PEND_UNPARK      6
#define SCO_ST_PEND_ROLECHANGE  7

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/

static const tBTM_ESCO_PARAMS btm_esco_defaults = {
    BTM_64KBITS_RATE,               /* TX Bandwidth (64 kbits/sec)              */
    BTM_64KBITS_RATE,               /* RX Bandwidth (64 kbits/sec)              */
    0x000a,                         /* 10 ms (HS/HF can use EV3, 2-EV3, 3-EV3)  */
    0x0060,                         /* Inp Linear, Air CVSD, 2s Comp, 16bit     */
    (BTM_SCO_PKT_TYPES_MASK_HV1 +   /* Packet Types                             */
    BTM_SCO_PKT_TYPES_MASK_HV2 +
    BTM_SCO_PKT_TYPES_MASK_HV3 +
    BTM_SCO_PKT_TYPES_MASK_EV3 +
    BTM_SCO_PKT_TYPES_MASK_EV4 +
    BTM_SCO_PKT_TYPES_MASK_EV5),
    BTM_ESCO_RETRANS_POWER        /* Retransmission Effort (Power)   */
};

/*******************************************************************************
**
** Function         btm_sco_flush_sco_data
**
** Description      This function is called to flush the SCO data for this channel.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_flush_sco_data(UINT16 sco_inx)
{
#if BTM_SCO_HCI_INCLUDED == TRUE
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p ;
    BT_HDR      *p_buf;

    if (sco_inx < BTM_MAX_SCO_LINKS) {
        p = &btm_cb.sco_cb.sco_db[sco_inx];
        while ((p_buf = (BT_HDR *)fixed_queue_dequeue(p->xmit_data_q, 0)) != NULL) {
            osi_free(p_buf);
        }
    }
#else
    UNUSED(sco_inx);
#endif
#else
    UNUSED(sco_inx);
#endif
}
/*******************************************************************************
**
** Function         btm_sco_init
**
** Description      This function is called at BTM startup to initialize
**
** Returns          void
**
*******************************************************************************/
void btm_sco_init (void)
{
#if 0  /* cleared in btm_init; put back in if called from anywhere else! */
    memset (&btm_cb.sco_cb, 0, sizeof(tSCO_CB));
#endif
#if (BTM_SCO_HCI_INCLUDED == TRUE)
    for (int i = 0; i < BTM_MAX_SCO_LINKS; i++) {
        btm_cb.sco_cb.sco_db[i].xmit_data_q = fixed_queue_new(QUEUE_SIZE_MAX);
    }
#endif
    /* Initialize nonzero defaults */
    btm_cb.sco_cb.sco_disc_reason  = BTM_INVALID_SCO_DISC_REASON;

    btm_cb.sco_cb.def_esco_parms = btm_esco_defaults; /* Initialize with defaults */
    btm_cb.sco_cb.desired_sco_mode = BTM_DEFAULT_SCO_MODE;
}

/*******************************************************************************
**
** Function         btm_esco_conn_rsp
**
** Description      This function is called upon receipt of an (e)SCO connection
**                  request event (BTM_ESCO_CONN_REQ_EVT) to accept or reject
**                  the request. Parameters used to negotiate eSCO links.
**                  If p_parms is NULL, then default values are used.
**                  If the link type of the incoming request is SCO, then only
**                  the tx_bw, max_latency, content format, and packet_types are
**                  valid.  The hci_status parameter should be
**                  ([0x0] to accept, [0x0d..0x0f] to reject)
**
** Returns          void
**
*******************************************************************************/
static void btm_esco_conn_rsp (UINT16 sco_inx, UINT8 hci_status, BD_ADDR bda,
                               tBTM_ESCO_PARAMS *p_parms)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN        *p_sco = NULL;
    tBTM_ESCO_PARAMS *p_setup;
    UINT16            temp_pkt_types;

    if (sco_inx < BTM_MAX_SCO_LINKS) {
        p_sco = &btm_cb.sco_cb.sco_db[sco_inx];
    }

    /* Reject the connect request if refused by caller or wrong state */
    if (hci_status != HCI_SUCCESS || p_sco == NULL) {
        if (p_sco) {
            p_sco->state = (p_sco->state == SCO_ST_W4_CONN_RSP) ? SCO_ST_LISTENING
                           : SCO_ST_UNUSED;
        }

        if (!btm_cb.sco_cb.esco_supported) {
            if (!btsnd_hcic_reject_conn (bda, hci_status)) {
                BTM_TRACE_ERROR("Could not reject (e)SCO conn: No Buffer!!!");
            }
        } else {
            if (!btsnd_hcic_reject_esco_conn (bda, hci_status)) {
                BTM_TRACE_ERROR("Could not reject (e)SCO conn: No Buffer!!!");
            }
        }
    } else { /* Connection is being accepted */
        p_sco->state = SCO_ST_CONNECTING;
        p_setup = &p_sco->esco.setup;
        /* If parameters not specified use the default */
        if (p_parms) {
            *p_setup = *p_parms;
        } else { /* Use the last setup passed thru BTM_SetEscoMode (or defaults) */
            *p_setup = btm_cb.sco_cb.def_esco_parms;
        }

        temp_pkt_types = (p_setup->packet_types &
                          BTM_SCO_SUPPORTED_PKTS_MASK &
                          btm_cb.btm_sco_pkt_types_supported);

        /* Make sure at least one eSCO packet type is sent, else might confuse peer */
        /* Taking this out to confirm with BQB tests
        ** Real application would like to include this though, as many devices
        ** do not retry with SCO only if an eSCO connection fails.
        if (!(temp_pkt_types & BTM_ESCO_LINK_ONLY_MASK))
        {
            temp_pkt_types |= BTM_SCO_PKT_TYPES_MASK_EV3;
        }
        */
        /* If SCO request, remove eSCO packet types (conformance) */
        if (p_sco->esco.data.link_type == BTM_LINK_TYPE_SCO) {
            temp_pkt_types &= BTM_SCO_LINK_ONLY_MASK;
            temp_pkt_types |= BTM_SCO_EXCEPTION_PKTS_MASK;
        } else {
            /* OR in any exception packet types */
            temp_pkt_types |= ((p_setup->packet_types & BTM_SCO_EXCEPTION_PKTS_MASK) |
                               (btm_cb.btm_sco_pkt_types_supported & BTM_SCO_EXCEPTION_PKTS_MASK));
        }

        if (btsnd_hcic_accept_esco_conn (bda, p_setup->tx_bw, p_setup->rx_bw,
                                         p_setup->max_latency, p_setup->voice_contfmt,
                                         p_setup->retrans_effort, temp_pkt_types)) {
            p_setup->packet_types = temp_pkt_types;
        } else {
            BTM_TRACE_ERROR("Could not accept SCO conn: No Buffer!!!");
        }
    }
#endif
}


#if BTM_SCO_HCI_INCLUDED == TRUE
void btm_sco_process_num_bufs (UINT16 num_lm_sco_bufs)
{
    BTM_TRACE_DEBUG("%s, %d", __FUNCTION__, num_lm_sco_bufs);
    btm_cb.sco_cb.num_lm_sco_bufs = btm_cb.sco_cb.xmit_window_size = num_lm_sco_bufs;
}

/*******************************************************************************
**
** Function         BTM_ConfigScoPath
**
** Description      This function enable/disable SCO over HCI and registers SCO
**                  data callback if SCO over HCI is enabled.
**
** Parameter        path: SCO or HCI
**                  p_sco_data_cb: callback function or SCO data if path is set
**                                 to transport.
**                  p_pcm_param: pointer to the PCM interface parameter. If a NULL
**                               pointer is used, PCM parameter maintained in
**                               the control block will be used; otherwise update
**                               control block value.
**                  err_data_rpt: Lisbon feature to enable the erronous data report
**                                or not.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_NO_RESOURCES: no rsource to start the command.
**                  BTM_ILLEGAL_VALUE: invalid callback function pointer.
**                  BTM_CMD_STARTED :Command sent. Waiting for command cmpl event.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ConfigScoPath (tBTM_SCO_ROUTE_TYPE path,
                               tBTM_SCO_DATA_CB *p_sco_data_cb,
                               tBTM_SCO_PCM_PARAM *p_pcm_param,
                               BOOLEAN err_data_rpt)
{
    UNUSED(err_data_rpt);
    UNUSED(p_pcm_param);
    btm_cb.sco_cb.sco_path = path;
    if (path == BTM_SCO_ROUTE_PCM) {
        return BTM_SUCCESS;
    } else if (path == BTM_SCO_ROUTE_HCI) {
        if (p_sco_data_cb) {
            btm_cb.sco_cb.p_data_cb = p_sco_data_cb;
        }
    }

    return BTM_SUCCESS;
}

static void hci_sco_data_to_lower(BT_HDR *p_buf)
{
    p_buf->event = BT_EVT_TO_LM_HCI_SCO;
    if (p_buf->offset == 0) {
        BTM_TRACE_ERROR("offset cannot be 0");
        osi_free(p_buf);
    }

    bte_main_hci_send(p_buf, (UINT16)(BT_EVT_TO_LM_HCI_SCO | LOCAL_BLE_CONTROLLER_ID));
}
/*******************************************************************************
**
** Function         btm_sco_check_send_pkts
**
** Description      This function is called to check if it can send packets
**                  to the Host Controller.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_check_send_pkts (UINT16 sco_inx)
{
    tSCO_CB  *p_cb = &btm_cb.sco_cb;
    tSCO_CONN   *p_ccb = &p_cb->sco_db[sco_inx];

    /* If there is data to send, send it now */
    BT_HDR  *p_buf;
    while (p_cb->xmit_window_size != 0)
    {
        if ((p_buf = (BT_HDR *)fixed_queue_dequeue(p_ccb->xmit_data_q, 0)) == NULL) {
            break;
        }
#if BTM_SCO_HCI_DEBUG
        BTM_TRACE_DEBUG("btm: [%d] buf in xmit_data_q",
                        fixed_queue_length(p_ccb->xmit_data_q) + 1);
#endif
        /* Don't go negative */
        p_cb->xmit_window_size -= 1;
        p_ccb->sent_not_acked += 1;

        // HCI_SCO_DATA_TO_LOWER(p_buf);
        hci_sco_data_to_lower(p_buf);
    }
}

void btm_sco_process_num_completed_pkts (UINT8 *p)
{
    UINT8       num_handles, xx;
    UINT16      handle;
    UINT16      num_sent;
    UINT16      sco_inx;
    tSCO_CB  *p_cb = &btm_cb.sco_cb;
    tSCO_CONN * p_ccb;
    STREAM_TO_UINT8 (num_handles, p);
    for (xx = 0; xx < num_handles; xx++) {
        STREAM_TO_UINT16 (handle, p);
        STREAM_TO_UINT16 (num_sent, p);
        if ((sco_inx = btm_find_scb_by_handle(handle)) == BTM_MAX_SCO_LINKS) {
            continue;
        }
        BTM_TRACE_DEBUG("%s, %d, %u", __FUNCTION__, handle, p_cb->xmit_window_size); //debug
        p_ccb = &p_cb->sco_db[sco_inx];
        p_ccb->sent_not_acked -= num_sent;
        // don't go negative
        if (p_ccb->sent_not_acked < 0) {
            BTM_TRACE_WARNING("SCO: un-acked underf: %u", p_ccb->sent_not_acked);
            p_ccb->sent_not_acked = 0;
        }
        p_cb->xmit_window_size += num_sent;
        if (p_cb->xmit_window_size > p_cb->num_lm_sco_bufs) {
            BTM_TRACE_WARNING("SCO xwind: %d, max %d", p_cb->xmit_window_size, p_cb->num_lm_sco_bufs);
            p_cb->xmit_window_size = p_cb->num_lm_sco_bufs;
        }
        btm_sco_check_send_pkts (sco_inx);
    }

    return;
}

/*******************************************************************************
**
** Function         btm_pkt_stat_nums_update
**
** Description      Update the number of received SCO data packet status.
**
** Returns          void
**
*******************************************************************************/
static void btm_pkt_stat_nums_update(uint16_t sco_inx, uint8_t pkt_status)
{
    tSCO_CONN   *p_ccb = &btm_cb.sco_cb.sco_db[sco_inx];
    p_ccb->pkt_stat_nums.rx_total++;
    if (pkt_status == BTM_SCO_DATA_CORRECT) {
        p_ccb->pkt_stat_nums.rx_correct++;
    } else if (pkt_status == BTM_SCO_DATA_PAR_ERR) {
        p_ccb->pkt_stat_nums.rx_err++;
    } else if (pkt_status == BTM_SCO_DATA_NONE) {
        p_ccb->pkt_stat_nums.rx_none++;
    } else {
        p_ccb->pkt_stat_nums.rx_lost++;
    }
}

/*******************************************************************************
**
** Function         btm_pkt_stat_send_nums_update
**
** Description      Update the number of send packet status.
**
** Returns          void
**
*******************************************************************************/
static void btm_pkt_stat_send_nums_update(uint16_t sco_inx, uint8_t pkt_status)
{
    tSCO_CONN   *p_ccb = &btm_cb.sco_cb.sco_db[sco_inx];
    p_ccb->pkt_stat_nums.tx_total++;
    if (pkt_status != BTM_SUCCESS && pkt_status != BTM_NO_RESOURCES && pkt_status != BTM_SCO_BAD_LENGTH) {
        p_ccb->pkt_stat_nums.tx_discarded++;
    }
}

/*******************************************************************************
**
** Function         btm_pkt_stat_nums_reset
**
** Description      This function is called to reset the number of packet status struct
**
** Returns          void
**
*******************************************************************************/
static void btm_pkt_stat_nums_reset(uint16_t sco_inx)
{
    memset(&btm_cb.sco_cb.sco_db[sco_inx].pkt_stat_nums, 0, sizeof(tBTM_SCO_PKT_STAT_NUMS));
}

/*******************************************************************************
**
** Function         BTM_PktStatNumsGet
**
** Description      This function is called to get the number of packet status struct
**
** Returns          void
**
*******************************************************************************/
void BTM_PktStatNumsGet(uint16_t sync_conn_handle, tBTM_SCO_PKT_STAT_NUMS *p_pkt_nums)
{
    uint16_t sco_inx = btm_find_scb_by_handle(sync_conn_handle);
    if (sco_inx < BTM_MAX_SCO_LINKS) {
        memcpy(p_pkt_nums, &btm_cb.sco_cb.sco_db[sco_inx].pkt_stat_nums, sizeof(tBTM_SCO_PKT_STAT_NUMS));
    } else {
        memset(p_pkt_nums, 0, sizeof(tBTM_SCO_PKT_STAT_NUMS));
    }
}

#endif /* BTM_SCO_HCI_INCLUDED == TRUE */

/*******************************************************************************
**
** Function         btm_route_sco_data
**
** Description      Route received SCO data.
**
** Returns          void
**
*******************************************************************************/
void  btm_route_sco_data(BT_HDR *p_msg)
{
#if BTM_SCO_HCI_INCLUDED == TRUE
    UINT16      sco_inx, handle;
    UINT8       *p = (UINT8 *)(p_msg + 1) + p_msg->offset;
    UINT8       pkt_size = 0;
    UINT8       pkt_status = 0;

    /* Extract Packet_Status_Flag and handle */
    STREAM_TO_UINT16 (handle, p);
    pkt_status = HCID_GET_EVENT(handle);
    handle   = HCID_GET_HANDLE (handle);

    STREAM_TO_UINT8 (pkt_size, p);
    UNUSED(pkt_size);
    if ((sco_inx = btm_find_scb_by_handle(handle)) != BTM_MAX_SCO_LINKS ) {
        /* send data callback */
        if (!btm_cb.sco_cb.p_data_cb )
            /* if no data callback registered,  just free the buffer  */
        {
            osi_free (p_msg);
        } else {
            btm_pkt_stat_nums_update(sco_inx, pkt_status);
            (*btm_cb.sco_cb.p_data_cb)(sco_inx, p_msg, (tBTM_SCO_DATA_FLAG) pkt_status);
        }
    } else { /* no mapping handle SCO connection is active, free the buffer */
        osi_free (p_msg);
    }
    BTM_TRACE_DEBUG ("SCO: hdl %x, len %d, pkt_sz %d\n", handle, p_msg->len, pkt_size);
#else
    osi_free(p_msg);
#endif
}

/*******************************************************************************
**
** Function         BTM_WriteScoData
**
** Description      This function write SCO data to a specified instance. The data
**                  to be written p_buf needs to carry an offset of
**                  HCI_SCO_PREAMBLE_SIZE bytes, and the data length can not
**                  exceed BTM_SCO_DATA_SIZE_MAX bytes, whose default value is set
**                  to 60 and is configurable. Data longer than the maximum bytes
**                  will be truncated.
**
** Returns          BTM_SUCCESS: data write is successful
**                  BTM_ILLEGAL_VALUE: SCO data contains illegal offset value.
**                  BTM_SCO_BAD_LENGTH: SCO data length exceeds the max SCO packet
**                                      size.
**                  BTM_NO_RESOURCES: no resources.
**                  BTM_UNKNOWN_ADDR: unknown SCO connection handle, or SCO is not
**                                    routed via HCI.
**                  BTM_ERR_PROCESSING: transmit queue overflow
**
**
*******************************************************************************/
tBTM_STATUS BTM_WriteScoData (UINT16 sco_inx, BT_HDR *p_buf)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);
#if (BTM_SCO_HCI_INCLUDED == TRUE) && (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p_ccb = &btm_cb.sco_cb.sco_db[sco_inx];
    UINT8   *p;
    tBTM_STATUS     status = BTM_SUCCESS;

    if (sco_inx < BTM_MAX_SCO_LINKS && btm_cb.sco_cb.p_data_cb &&
            p_ccb->state == SCO_ST_CONNECTED) {
        /* Ensure we have enough space in the buffer for the SCO and HCI headers */
        if (p_buf->offset < HCI_SCO_PREAMBLE_SIZE) {
            BTM_TRACE_ERROR ("BTM SCO - cannot send buffer, offset: %d", p_buf->offset);
            status = BTM_ILLEGAL_VALUE;
        } else { /* write HCI header */
            /* Step back 3 bytes to add the headers */
            p_buf->offset -= HCI_SCO_PREAMBLE_SIZE;
            /* Set the pointer to the beginning of the data */
            p = (UINT8 *)(p_buf + 1) + p_buf->offset;
            /* add HCI handle */
            UINT16_TO_STREAM (p, p_ccb->hci_handle);
            /* only sent the first BTM_SCO_DATA_SIZE_MAX bytes data if more than max,
               and set warning status */
            if (p_buf->len > BTM_SCO_DATA_SIZE_MAX) {
                BTM_TRACE_WARNING ("BTM SCO hdl %x, bad len %u", p_ccb->hci_handle, p_buf->len);
                p_buf->len = BTM_SCO_DATA_SIZE_MAX;
                status = BTM_SCO_BAD_LENGTH;
            }

            UINT8_TO_STREAM (p, (UINT8)p_buf->len);

            p_buf->len += HCI_SCO_PREAMBLE_SIZE;

            if (fixed_queue_length(p_ccb->xmit_data_q) < BTM_SCO_XMIT_QUEUE_THRS) {
                if (fixed_queue_length(p_ccb->xmit_data_q) >= BTM_SCO_XMIT_QUEUE_HIGH_WM) {
                    status = BTM_NO_RESOURCES;
                }
                fixed_queue_enqueue(p_ccb->xmit_data_q, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
                btm_sco_check_send_pkts (sco_inx);
            } else {
                BTM_TRACE_WARNING ("SCO xmit Q overflow, pkt dropped");
                status = BTM_ERR_PROCESSING;
            }
        }
    } else {
        BTM_TRACE_WARNING ("BTM_WriteScoData, invalid sco index: %d at state [%d]",
                           sco_inx, btm_cb.sco_cb.sco_db[sco_inx].state);
        status = BTM_UNKNOWN_ADDR;
    }

    if (status != BTM_SUCCESS && status!= BTM_NO_RESOURCES && status != BTM_SCO_BAD_LENGTH) {
        BTM_TRACE_WARNING ("stat %d", status);
        osi_free(p_buf);
    }
    btm_pkt_stat_send_nums_update(sco_inx, status);
    return (status);

#else
    UNUSED(sco_inx);
    UNUSED(p_buf);
    return (BTM_NO_RESOURCES);
#endif
}

#if (BTM_MAX_SCO_LINKS>0)
/*******************************************************************************
**
** Function         btm_send_connect_request
**
** Description      This function is called to respond to SCO connect indications
**
** Returns          void
**
*******************************************************************************/
static tBTM_STATUS btm_send_connect_request(UINT16 acl_handle,
        tBTM_ESCO_PARAMS *p_setup)
{
    UINT16 temp_pkt_types;
    tACL_CONN *p_acl;

    /* Send connect request depending on version of spec */
    if (!btm_cb.sco_cb.esco_supported) {
        if (!btsnd_hcic_add_SCO_conn (acl_handle, BTM_ESCO_2_SCO(p_setup->packet_types))) {
            return (BTM_NO_RESOURCES);
        }
    } else {
        temp_pkt_types = (p_setup->packet_types & BTM_SCO_SUPPORTED_PKTS_MASK &
                          btm_cb.btm_sco_pkt_types_supported);

        /* OR in any exception packet types */
        temp_pkt_types |= ((p_setup->packet_types & BTM_SCO_EXCEPTION_PKTS_MASK) |
                           (btm_cb.btm_sco_pkt_types_supported & BTM_SCO_EXCEPTION_PKTS_MASK));

        /* Finally, remove EDR eSCO if the remote device doesn't support it */
        /* UPF25:  Only SCO was brought up in this case */
        p_acl = btm_handle_to_acl(acl_handle);
        if (p_acl) {
            if (!HCI_EDR_ESCO_2MPS_SUPPORTED(p_acl->peer_lmp_features[HCI_EXT_FEATURES_PAGE_0])) {

                BTM_TRACE_WARNING("BTM Remote does not support 2-EDR eSCO");
                temp_pkt_types |= (HCI_ESCO_PKT_TYPES_MASK_NO_2_EV3 |
                                   HCI_ESCO_PKT_TYPES_MASK_NO_2_EV5);
            }
            if (!HCI_EDR_ESCO_3MPS_SUPPORTED(p_acl->peer_lmp_features[HCI_EXT_FEATURES_PAGE_0])) {

                BTM_TRACE_WARNING("BTM Remote does not support 3-EDR eSCO");
                temp_pkt_types |= (HCI_ESCO_PKT_TYPES_MASK_NO_3_EV3 |
                                   HCI_ESCO_PKT_TYPES_MASK_NO_3_EV5);
            }

            /* Check to see if BR/EDR Secure Connections is being used
            ** If so, we cannot use SCO-only packet types (HFP 1.7)
            */
            if (BTM_BothEndsSupportSecureConnections(p_acl->remote_addr)) {
                temp_pkt_types &= ~(BTM_SCO_PKT_TYPE_MASK);
                BTM_TRACE_DEBUG("%s: SCO Conn: pkt_types after removing SCO (0x%04x)", __FUNCTION__,
                                temp_pkt_types);

                /* Return error if no packet types left */
                if (temp_pkt_types == 0) {
                    BTM_TRACE_ERROR("%s: SCO Conn (BR/EDR SC): No packet types available",__FUNCTION__);
                    return (BTM_WRONG_MODE);
                }
            } else {
                BTM_TRACE_DEBUG("%s: SCO Conn(BR/EDR SC):local or peer does not support BR/EDR SC",__FUNCTION__);
            }
        }


        BTM_TRACE_API("txbw 0x%x, rxbw 0x%x, lat 0x%x, voice 0x%x, retrans 0x%02x, pkt 0x%04x",
                      p_setup->tx_bw, p_setup->rx_bw,
                      p_setup->max_latency, p_setup->voice_contfmt,
                      p_setup->retrans_effort, temp_pkt_types);

        if (!btsnd_hcic_setup_esco_conn(acl_handle,
                                        p_setup->tx_bw,
                                        p_setup->rx_bw,
                                        p_setup->max_latency,
                                        p_setup->voice_contfmt,
                                        p_setup->retrans_effort,
                                        temp_pkt_types)) {
            return (BTM_NO_RESOURCES);
        } else {
            p_setup->packet_types = temp_pkt_types;
        }
    }

    return (BTM_CMD_STARTED);
}
#endif

/*******************************************************************************
**
** Function         btm_set_sco_ind_cback
**
** Description      This function is called to register for TCS SCO connect
**                  indications.
**
** Returns          void
**
*******************************************************************************/
void btm_set_sco_ind_cback( tBTM_SCO_IND_CBACK *sco_ind_cb )
{
    btm_cb.sco_cb.app_sco_ind_cb = sco_ind_cb;
}

/*******************************************************************************
**
** Function         btm_accept_sco_link
**
** Description      This function is called to respond to TCS SCO connect
**                  indications
**
** Returns          void
**
*******************************************************************************/
void btm_accept_sco_link(UINT16 sco_inx, tBTM_ESCO_PARAMS *p_setup,
                         tBTM_SCO_CB *p_conn_cb, tBTM_SCO_CB *p_disc_cb)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN        *p_sco;

    if (sco_inx >= BTM_MAX_SCO_LINKS) {
        BTM_TRACE_ERROR("btm_accept_sco_link: Invalid sco_inx(%d)", sco_inx);
        return;
    }

    /* Link role is ignored in for this message */
    p_sco = &btm_cb.sco_cb.sco_db[sco_inx];
    p_sco->p_conn_cb = p_conn_cb;
    p_sco->p_disc_cb = p_disc_cb;
    p_sco->esco.data.link_type = BTM_LINK_TYPE_ESCO; /* Accept with all supported types */

    BTM_TRACE_DEBUG("TCS accept SCO: Packet Types 0x%04x", p_setup->packet_types);

    btm_esco_conn_rsp(sco_inx, HCI_SUCCESS, p_sco->esco.data.bd_addr, p_setup);
#else
    btm_reject_sco_link(sco_inx);
#endif
}

/*******************************************************************************
**
** Function         btm_reject_sco_link
**
** Description      This function is called to respond to SCO connect indications
**
** Returns          void
**
*******************************************************************************/
void btm_reject_sco_link( UINT16 sco_inx )
{
    btm_esco_conn_rsp(sco_inx, HCI_ERR_HOST_REJECT_RESOURCES,
                      btm_cb.sco_cb.sco_db[sco_inx].esco.data.bd_addr, NULL);
}

/*******************************************************************************
**
** Function         BTM_CreateSco
**
** Description      This function is called to create an SCO connection. If the
**                  "is_orig" flag is TRUE, the connection will be originated,
**                  otherwise BTM will wait for the other side to connect.
**
**                  NOTE:  If BTM_IGNORE_SCO_PKT_TYPE is passed in the pkt_types
**                      parameter the default packet types is used.
**
** Returns          BTM_UNKNOWN_ADDR if the ACL connection is not up
**                  BTM_BUSY         if another SCO being set up to
**                                   the same BD address
**                  BTM_NO_RESOURCES if the max SCO limit has been reached
**                  BTM_CMD_STARTED  if the connection establishment is started.
**                                   In this case, "*p_sco_inx" is filled in
**                                   with the sco index used for the connection.
**
*******************************************************************************/
tBTM_STATUS BTM_CreateSco (BD_ADDR remote_bda, BOOLEAN is_orig, UINT16 pkt_types,
                           UINT16 *p_sco_inx, tBTM_SCO_CB *p_conn_cb,
                           tBTM_SCO_CB *p_disc_cb)
{
#if (BTM_MAX_SCO_LINKS > 0)
    tBTM_ESCO_PARAMS *p_setup;
    tSCO_CONN        *p = &btm_cb.sco_cb.sco_db[0];
    UINT16            xx;
    UINT16            acl_handle = 0;
    UINT16            temp_pkt_types;
    tACL_CONN        *p_acl;

#if (BTM_SCO_WAKE_PARKED_LINK == TRUE)
    tBTM_PM_MODE      md;
    tBTM_PM_PWR_MD    pm;
#else  // BTM_SCO_WAKE_PARKED_LINK
    UINT8             mode;
#endif  // BTM_SCO_WAKE_PARKED_LINK

    *p_sco_inx = BTM_INVALID_SCO_INDEX;

    /* If originating, ensure that there is an ACL connection to the BD Address */
    if (is_orig) {
        if ((!remote_bda) || ((acl_handle = BTM_GetHCIConnHandle (remote_bda, BT_TRANSPORT_BR_EDR)) == 0xFFFF)) {
            return (BTM_UNKNOWN_ADDR);
        }
    }

    if (remote_bda) {
        /* If any SCO is being established to the remote BD address, refuse this */
        for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
            if (((p->state == SCO_ST_CONNECTING) || (p->state == SCO_ST_LISTENING)
                    || (p->state == SCO_ST_PEND_UNPARK))
                    && (!memcmp (p->esco.data.bd_addr, remote_bda, BD_ADDR_LEN))) {
                return (BTM_BUSY);
            }
        }
    } else {
        /* Support only 1 wildcard BD address at a time */
        for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
            if ((p->state == SCO_ST_LISTENING) && (!p->rem_bd_known)) {
                return (BTM_BUSY);
            }
        }
    }

    /* Now, try to find an unused control block, and kick off the SCO establishment */
    for (xx = 0, p = &btm_cb.sco_cb.sco_db[0]; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (p->state == SCO_ST_UNUSED) {
            if (remote_bda) {
                if (is_orig) {
                    /* can not create SCO link if in park mode */
#if BTM_SCO_WAKE_PARKED_LINK == TRUE
                    if (BTM_ReadPowerMode(remote_bda, &md) == BTM_SUCCESS) {
                        if (md == BTM_PM_MD_PARK || md == BTM_PM_MD_SNIFF) {
                            memset( (void *)&pm, 0, sizeof(pm));
                            pm.mode = BTM_PM_MD_ACTIVE;
                            BTM_SetPowerMode(BTM_PM_SET_ONLY_ID, remote_bda, &pm);
                            p->state = SCO_ST_PEND_UNPARK;
                        }
                    }
#else  // BTM_SCO_WAKE_PARKED_LINK
                    if ( (BTM_ReadPowerMode(remote_bda, &mode) == BTM_SUCCESS) && (mode == BTM_PM_MD_PARK) ) {
                        return (BTM_WRONG_MODE);
                    }
#endif  // BTM_SCO_WAKE_PARKED_LINK
                }
                memcpy (p->esco.data.bd_addr, remote_bda, BD_ADDR_LEN);
                p->rem_bd_known = TRUE;
            } else {
                p->rem_bd_known = FALSE;
            }

            /* Link role is ignored in for this message */
            if (pkt_types == BTM_IGNORE_SCO_PKT_TYPE) {
                pkt_types = btm_cb.sco_cb.def_esco_parms.packet_types;
            }

            p_setup = &p->esco.setup;
            *p_setup = btm_cb.sco_cb.def_esco_parms;
            p_setup->packet_types = (btm_cb.sco_cb.desired_sco_mode == BTM_LINK_TYPE_SCO)
                                    ? (pkt_types & BTM_SCO_LINK_ONLY_MASK) : pkt_types;

            temp_pkt_types = (p_setup->packet_types & BTM_SCO_SUPPORTED_PKTS_MASK &
                              btm_cb.btm_sco_pkt_types_supported);

            /* OR in any exception packet types */
            if (btm_cb.sco_cb.desired_sco_mode == HCI_LINK_TYPE_ESCO) {
                temp_pkt_types |= ((p_setup->packet_types & BTM_SCO_EXCEPTION_PKTS_MASK) |
                                   (btm_cb.btm_sco_pkt_types_supported & BTM_SCO_EXCEPTION_PKTS_MASK));
            } else { /* Only using SCO packet types; turn off EDR also */
                temp_pkt_types |= BTM_SCO_EXCEPTION_PKTS_MASK;
            }

            p_setup->packet_types = temp_pkt_types;
            p->p_conn_cb  = p_conn_cb;
            p->p_disc_cb  = p_disc_cb;
            p->hci_handle = BTM_INVALID_HCI_HANDLE;
            p->is_orig = is_orig;

            if ( p->state != SCO_ST_PEND_UNPARK ) {
                if (is_orig) {
                    /* If role change is in progress, do not proceed with SCO setup
                     * Wait till role change is complete */
                    p_acl = btm_bda_to_acl(remote_bda, BT_TRANSPORT_BR_EDR);
                    if (p_acl && p_acl->switch_role_state != BTM_ACL_SWKEY_STATE_IDLE) {
                        BTM_TRACE_API("Role Change is in progress for ACL handle 0x%04x", acl_handle);
                        p->state = SCO_ST_PEND_ROLECHANGE;

                    }
                }
            }

            if ( p->state != SCO_ST_PEND_UNPARK && p->state != SCO_ST_PEND_ROLECHANGE ) {
                if (is_orig) {
                    BTM_TRACE_API("BTM_CreateSco -> (e)SCO Link for ACL handle 0x%04x, Desired Type %d",
                                  acl_handle, btm_cb.sco_cb.desired_sco_mode);

                    if ((btm_send_connect_request(acl_handle, p_setup)) != BTM_CMD_STARTED) {
                        return (BTM_NO_RESOURCES);
                    }

                    p->state = SCO_ST_CONNECTING;
                } else {
                    p->state = SCO_ST_LISTENING;
                }
            }

            *p_sco_inx = xx;

            return (BTM_CMD_STARTED);
        }
    }

#endif
    /* If here, all SCO blocks in use */
    return (BTM_NO_RESOURCES);
}

#if (BTM_SCO_WAKE_PARKED_LINK == TRUE)
/*******************************************************************************
**
** Function         btm_sco_chk_pend_unpark
**
** Description      This function is called by BTIF when there is a mode change
**                  event to see if there are SCO commands waiting for the unpark.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_chk_pend_unpark (UINT8 hci_status, UINT16 hci_handle)
{
#if (BTM_MAX_SCO_LINKS>0)
    UINT16      xx;
    UINT16      acl_handle;
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if ((p->state == SCO_ST_PEND_UNPARK) &&
                ((acl_handle = BTM_GetHCIConnHandle (p->esco.data.bd_addr, BT_TRANSPORT_BR_EDR)) == hci_handle))

        {
            BTM_TRACE_API("btm_sco_chk_pend_unpark -> (e)SCO Link for ACL handle 0x%04x, Desired Type %d, hci_status 0x%02x",
                          acl_handle, btm_cb.sco_cb.desired_sco_mode, hci_status);

            if ((btm_send_connect_request(acl_handle, &p->esco.setup)) == BTM_CMD_STARTED) {
                p->state = SCO_ST_CONNECTING;
            }
        }
    }
#endif  // BTM_MAX_SCO_LINKS
}
#endif  // BTM_SCO_WAKE_PARKED_LINK

/*******************************************************************************
**
** Function         btm_sco_chk_pend_rolechange
**
** Description      This function is called by BTIF when there is a role change
**                  event to see if there are SCO commands waiting for the role change.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_chk_pend_rolechange (UINT16 hci_handle)
{
#if (BTM_MAX_SCO_LINKS>0)
    UINT16      xx;
    UINT16      acl_handle;
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if ((p->state == SCO_ST_PEND_ROLECHANGE) &&
                ((acl_handle = BTM_GetHCIConnHandle (p->esco.data.bd_addr, BT_TRANSPORT_BR_EDR)) == hci_handle))

        {
            BTM_TRACE_API("btm_sco_chk_pend_rolechange -> (e)SCO Link for ACL handle 0x%04x", acl_handle);

            if ((btm_send_connect_request(acl_handle, &p->esco.setup)) == BTM_CMD_STARTED) {
                p->state = SCO_ST_CONNECTING;
            }
        }
    }
#endif
}

/*******************************************************************************
**
** Function         btm_sco_conn_req
**
** Description      This function is called by BTIF when an SCO connection
**                  request is received from a remote.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_conn_req (BD_ADDR bda,  DEV_CLASS dev_class, UINT8 link_type)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CB     *p_sco = &btm_cb.sco_cb;
    tSCO_CONN   *p = &p_sco->sco_db[0];
    UINT16      xx;
    tBTM_ESCO_CONN_REQ_EVT_DATA evt_data;

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        /*
         * If the sco state is in the SCO_ST_CONNECTING state, we still need
         * to return accept sco to avoid race conditon for sco creation
         */
        int rem_bd_matches = p->rem_bd_known &&
                             !memcmp (p->esco.data.bd_addr, bda, BD_ADDR_LEN);
        if (((p->state == SCO_ST_CONNECTING) && rem_bd_matches) ||
                ((p->state == SCO_ST_LISTENING) && (rem_bd_matches || !p->rem_bd_known))) {
            /* If this guy was a wildcard, he is not one any more */
            p->rem_bd_known = TRUE;
            p->esco.data.link_type = link_type;
            p->state = SCO_ST_W4_CONN_RSP;
            memcpy (p->esco.data.bd_addr, bda, BD_ADDR_LEN);

            /* If no callback, auto-accept the connection if packet types match */
            if (!p->esco.p_esco_cback) {
                /* If requesting eSCO reject if default parameters are SCO only */
                if ((link_type == BTM_LINK_TYPE_ESCO
                        && !(p_sco->def_esco_parms.packet_types & BTM_ESCO_LINK_ONLY_MASK)
                        && ((p_sco->def_esco_parms.packet_types & BTM_SCO_EXCEPTION_PKTS_MASK)
                            == BTM_SCO_EXCEPTION_PKTS_MASK))

                        /* Reject request if SCO is desired but no SCO packets delected */
                        || (link_type == BTM_LINK_TYPE_SCO
                            && !(p_sco->def_esco_parms.packet_types & BTM_SCO_LINK_ONLY_MASK))) {
                    btm_esco_conn_rsp(xx, HCI_ERR_HOST_REJECT_RESOURCES, bda, NULL);
                } else { /* Accept the request */
                    btm_esco_conn_rsp(xx, HCI_SUCCESS, bda, NULL);
                }
            } else { /* Notify upper layer of connect indication */
                memcpy(evt_data.bd_addr, bda, BD_ADDR_LEN);
                memcpy(evt_data.dev_class, dev_class, DEV_CLASS_LEN);
                evt_data.link_type = link_type;
                evt_data.sco_inx = xx;
                p->esco.p_esco_cback(BTM_ESCO_CONN_REQ_EVT, (tBTM_ESCO_EVT_DATA *)&evt_data);
            }

            return;
        }
    }

    /* TCS usage */
    if (btm_cb.sco_cb.app_sco_ind_cb) {
        /* Now, try to find an unused control block */
        for (xx = 0, p = &btm_cb.sco_cb.sco_db[0]; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
            if (p->state == SCO_ST_UNUSED) {
                p->is_orig = FALSE;
                p->state = SCO_ST_LISTENING;

                p->esco.data.link_type = link_type;
                memcpy (p->esco.data.bd_addr, bda, BD_ADDR_LEN);
                p->rem_bd_known = TRUE;
                break;
            }
        }
        if ( xx < BTM_MAX_SCO_LINKS) {
            btm_cb.sco_cb.app_sco_ind_cb(xx);
            return;
        }
    }

#endif
    /* If here, no one wants the SCO connection. Reject it */
    BTM_TRACE_WARNING("btm_sco_conn_req: No one wants this SCO connection; rejecting it");
    btm_esco_conn_rsp(BTM_MAX_SCO_LINKS, HCI_ERR_HOST_REJECT_RESOURCES, bda, NULL);
}

/*******************************************************************************
**
** Function         btm_sco_connected
**
** Description      This function is called by BTIF when an (e)SCO connection
**                  is connected.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_connected (UINT8 hci_status, BD_ADDR bda, UINT16 hci_handle,
                        tBTM_ESCO_DATA *p_esco_data)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];
    UINT16      xx;
    BOOLEAN     spt = FALSE;
    tBTM_CHG_ESCO_PARAMS parms;
#endif

    btm_cb.sco_cb.sco_disc_reason = hci_status;
    BTM_TRACE_API("%s, handle %x", __FUNCTION__, hci_handle);
#if (BTM_MAX_SCO_LINKS>0)
    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (((p->state == SCO_ST_CONNECTING) ||
                (p->state == SCO_ST_LISTENING)  ||
                (p->state == SCO_ST_W4_CONN_RSP))
                && (p->rem_bd_known)
                && (!bda || !memcmp (p->esco.data.bd_addr, bda, BD_ADDR_LEN))) {
            if (hci_status != HCI_SUCCESS) {
                /* Report the error if originator, otherwise remain in Listen mode */
                if (p->is_orig) {
                    /* If role switch is pending, we need try again after role switch is complete */
                    if (hci_status == HCI_ERR_ROLE_SWITCH_PENDING) {
                        BTM_TRACE_API("Role Change pending for HCI handle 0x%04x", hci_handle);
                        p->state = SCO_ST_PEND_ROLECHANGE;
                    }
                    /* avoid calling disconnect callback because of sco creation race */
                    else if (hci_status != HCI_ERR_LMP_ERR_TRANS_COLLISION) {
                        p->state = SCO_ST_UNUSED;
                        (*p->p_disc_cb)(xx);
                    }
                } else {
                    /* Notify the upper layer that incoming sco connection has failed. */
                    if (p->state == SCO_ST_CONNECTING) {
                        p->state = SCO_ST_UNUSED;
                        (*p->p_disc_cb)(xx);
                    } else {
                        p->state = SCO_ST_LISTENING;
                    }
                }

                return;
            }

            if (p->state == SCO_ST_LISTENING) {
                spt = TRUE;
            }
#if BTM_SCO_HCI_INCLUDED == TRUE
            p->sent_not_acked = 0;
            btm_pkt_stat_nums_reset(xx);
#endif
            p->state = SCO_ST_CONNECTED;
            p->hci_handle = hci_handle;

            if (!btm_cb.sco_cb.esco_supported) {
                p->esco.data.link_type = BTM_LINK_TYPE_SCO;
                if (spt) {
                    parms.packet_types = p->esco.setup.packet_types;
                    /* Keep the other parameters the same for SCO */
                    parms.max_latency = p->esco.setup.max_latency;
                    parms.retrans_effort = p->esco.setup.retrans_effort;

                    BTM_ChangeEScoLinkParms(xx, &parms);
                }
            } else {
                if (p_esco_data) {
                    p->esco.data = *p_esco_data;
                }
            }

            (*p->p_conn_cb)(xx);

            return;
        }
    }
#endif
}


/*******************************************************************************
**
** Function         btm_find_scb_by_handle
**
** Description      Look through all active SCO connection for a match based on the
**                  HCI handle.
**
** Returns          index to matched SCO connection CB, or BTM_MAX_SCO_LINKS if
**                  no match.
**
*******************************************************************************/
UINT16  btm_find_scb_by_handle (UINT16 handle)
{
    int         xx;
    tSCO_CONN    *p = &btm_cb.sco_cb.sco_db[0];

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if ((p->state == SCO_ST_CONNECTED) && (p->hci_handle == handle)) {
            return (xx);
        }
    }

    /* If here, no match found */
    return (xx);
}

/*******************************************************************************
**
** Function         BTM_RemoveSco
**
** Description      This function is called to remove a specific SCO connection.
**
** Returns          status of the operation
**
*******************************************************************************/
tBTM_STATUS BTM_RemoveSco (UINT16 sco_inx)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[sco_inx];
    UINT16       tempstate;

    /* Validity check */
    if ((sco_inx >= BTM_MAX_SCO_LINKS) || (p->state == SCO_ST_UNUSED)) {
        return (BTM_UNKNOWN_ADDR);
    }

    /* If no HCI handle, simply drop the connection and return */
    if (p->hci_handle == BTM_INVALID_HCI_HANDLE || p->state == SCO_ST_PEND_UNPARK) {
        p->hci_handle = BTM_INVALID_HCI_HANDLE;
        p->state = SCO_ST_UNUSED;
        p->esco.p_esco_cback = NULL;    /* Deregister the eSCO event callback */
        return (BTM_SUCCESS);
    }

    tempstate = p->state;
    p->state = SCO_ST_DISCONNECTING;

    if (!btsnd_hcic_disconnect (p->hci_handle, HCI_ERR_PEER_USER)) {
        p->state = tempstate;
        return (BTM_NO_RESOURCES);
    }

    return (BTM_CMD_STARTED);
#else
    return (BTM_NO_RESOURCES);
#endif
}

/*******************************************************************************
**
** Function         btm_remove_sco_links
**
** Description      This function is called to remove all sco links for an ACL link.
**
** Returns          void
**
*******************************************************************************/
void btm_remove_sco_links (BD_ADDR bda)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];
    UINT16       xx;

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (p->rem_bd_known && (!memcmp (p->esco.data.bd_addr, bda, BD_ADDR_LEN))) {
            BTM_RemoveSco(xx);
        }
    }
#endif
}

/*******************************************************************************
**
** Function         btm_sco_removed
**
** Description      This function is called by BTIF when an SCO connection
**                  is removed.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_removed (UINT16 hci_handle, UINT8 reason)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];
    UINT16      xx;
#endif

    btm_cb.sco_cb.sco_disc_reason = reason;

#if (BTM_MAX_SCO_LINKS>0)
    p = &btm_cb.sco_cb.sco_db[0];
    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if ((p->state != SCO_ST_UNUSED) && (p->state != SCO_ST_LISTENING) && (p->hci_handle == hci_handle)) {
            btm_sco_flush_sco_data(xx);

            p->state = SCO_ST_UNUSED;
#if BTM_SCO_HCI_INCLUDED == TRUE
            btm_cb.sco_cb.xmit_window_size += p->sent_not_acked;
            /* avoid overflow */
            if (btm_cb.sco_cb.xmit_window_size > btm_cb.sco_cb.num_lm_sco_bufs) {
                btm_cb.sco_cb.xmit_window_size = btm_cb.sco_cb.num_lm_sco_bufs;
            }
            p->sent_not_acked = 0;
#endif
            p->hci_handle = BTM_INVALID_HCI_HANDLE;
            p->rem_bd_known = FALSE;
            p->esco.p_esco_cback = NULL;    /* Deregister eSCO callback */
            (*p->p_disc_cb)(xx);

            return;
        }
    }
#endif
}


/*******************************************************************************
**
** Function         btm_sco_acl_removed
**
** Description      This function is called when an ACL connection is
**                  removed. If the BD address is NULL, it is assumed that
**                  the local device is down, and all SCO links are removed.
**                  If a specific BD address is passed, only SCO connections
**                  to that BD address are removed.
**
** Returns          void
**
*******************************************************************************/
void btm_sco_acl_removed (BD_ADDR bda)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[0];
    UINT16      xx;

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (p->state != SCO_ST_UNUSED) {
            if ((!bda) || (!memcmp (p->esco.data.bd_addr, bda, BD_ADDR_LEN) && p->rem_bd_known)) {
                btm_sco_flush_sco_data(xx);

                p->state = SCO_ST_UNUSED;
                p->esco.p_esco_cback = NULL;    /* Deregister eSCO callback */
                (*p->p_disc_cb)(xx);
            }
        }
    }
#endif
}


/*******************************************************************************
**
** Function         BTM_SetScoPacketTypes
**
** Description      This function is called to set the packet types used for
**                  a specific SCO connection,
**
** Parameters       pkt_types - One or more of the following
**                  BTM_SCO_PKT_TYPES_MASK_HV1
**                  BTM_SCO_PKT_TYPES_MASK_HV2
**                  BTM_SCO_PKT_TYPES_MASK_HV3
**                  BTM_SCO_PKT_TYPES_MASK_EV3
**                  BTM_SCO_PKT_TYPES_MASK_EV4
**                  BTM_SCO_PKT_TYPES_MASK_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV5
**
**                  BTM_SCO_LINK_ALL_MASK   - enables all supported types
**
** Returns          status of the operation
**
*******************************************************************************/
tBTM_STATUS BTM_SetScoPacketTypes (UINT16 sco_inx, UINT16 pkt_types)
{
#if (BTM_MAX_SCO_LINKS>0)
    tBTM_CHG_ESCO_PARAMS parms;
    tSCO_CONN           *p;

    /* Validity check */
    if (sco_inx >= BTM_MAX_SCO_LINKS) {
        return (BTM_UNKNOWN_ADDR);
    }

    p = &btm_cb.sco_cb.sco_db[sco_inx];
    parms.packet_types = pkt_types;

    /* Keep the other parameters the same for SCO */
    parms.max_latency = p->esco.setup.max_latency;
    parms.retrans_effort = p->esco.setup.retrans_effort;

    return (BTM_ChangeEScoLinkParms(sco_inx, &parms));
#else
    return (BTM_UNKNOWN_ADDR);
#endif
}


/*******************************************************************************
**
** Function         BTM_ReadScoPacketTypes
**
** Description      This function is read the packet types used for a specific
**                  SCO connection.
**
** Returns          Packet types supported for the connection
**                  One or more of the following (bitmask):
**                  BTM_SCO_PKT_TYPES_MASK_HV1
**                  BTM_SCO_PKT_TYPES_MASK_HV2
**                  BTM_SCO_PKT_TYPES_MASK_HV3
**                  BTM_SCO_PKT_TYPES_MASK_EV3
**                  BTM_SCO_PKT_TYPES_MASK_EV4
**                  BTM_SCO_PKT_TYPES_MASK_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV5
**
*******************************************************************************/
UINT16 BTM_ReadScoPacketTypes (UINT16 sco_inx)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN *p = &btm_cb.sco_cb.sco_db[sco_inx];

    /* Validity check */
    if ((sco_inx < BTM_MAX_SCO_LINKS) && (p->state == SCO_ST_CONNECTED)) {
        return (p->esco.setup.packet_types);
    } else {
        return (0);
    }
#else
    return (0);
#endif
}

/*******************************************************************************
**
** Function         BTM_ReadScoDiscReason
**
** Description      This function is returns the reason why an (e)SCO connection
**                  has been removed. It contains the value until read, or until
**                  another (e)SCO connection has disconnected.
**
** Returns          HCI reason or BTM_INVALID_SCO_DISC_REASON if not set.
**
*******************************************************************************/
UINT16 BTM_ReadScoDiscReason (void)
{
    UINT16 res = btm_cb.sco_cb.sco_disc_reason;
    btm_cb.sco_cb.sco_disc_reason = BTM_INVALID_SCO_DISC_REASON;
    return (res);
}

/*******************************************************************************
**
** Function         BTM_ReadDeviceScoPacketTypes
**
** Description      This function is read the SCO packet types that
**                  the device supports.
**
** Returns          Packet types supported by the device.
**                  One or more of the following (bitmask):
**                  BTM_SCO_PKT_TYPES_MASK_HV1
**                  BTM_SCO_PKT_TYPES_MASK_HV2
**                  BTM_SCO_PKT_TYPES_MASK_HV3
**                  BTM_SCO_PKT_TYPES_MASK_EV3
**                  BTM_SCO_PKT_TYPES_MASK_EV4
**                  BTM_SCO_PKT_TYPES_MASK_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV3
**                  BTM_SCO_PKT_TYPES_MASK_NO_2_EV5
**                  BTM_SCO_PKT_TYPES_MASK_NO_3_EV5
**
*******************************************************************************/
UINT16 BTM_ReadDeviceScoPacketTypes (void)
{
    return (btm_cb.btm_sco_pkt_types_supported);
}

/*******************************************************************************
**
** Function         BTM_ReadScoHandle
**
** Description      This function is used to read the HCI handle used for a specific
**                  SCO connection,
**
** Returns          handle for the connection, or 0xFFFF if invalid SCO index.
**
*******************************************************************************/
UINT16 BTM_ReadScoHandle (UINT16 sco_inx)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[sco_inx];

    /* Validity check */
    if ((sco_inx < BTM_MAX_SCO_LINKS) && (p->state == SCO_ST_CONNECTED)) {
        return (p->hci_handle);
    } else {
        return (BTM_INVALID_HCI_HANDLE);
    }
#else
    return (BTM_INVALID_HCI_HANDLE);
#endif
}

/*******************************************************************************
**
** Function         BTM_ReadScoBdAddr
**
** Description      This function is read the remote BD Address for a specific
**                  SCO connection,
**
** Returns          pointer to BD address or NULL if not known
**
*******************************************************************************/
UINT8 *BTM_ReadScoBdAddr (UINT16 sco_inx)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN   *p = &btm_cb.sco_cb.sco_db[sco_inx];

    /* Validity check */
    if ((sco_inx < BTM_MAX_SCO_LINKS) && (p->rem_bd_known)) {
        return (p->esco.data.bd_addr);
    } else {
        return (NULL);
    }
#else
    return (NULL);
#endif
}

/*******************************************************************************
**
** Function         BTM_SetEScoMode
**
** Description      This function sets up the negotiated parameters for SCO or
**                  eSCO, and sets as the default mode used for outgoing calls to
**                  BTM_CreateSco.  It does not change any currently active (e)SCO links.
**                  Note:  Incoming (e)SCO connections will always use packet types
**                      supported by the controller.  If eSCO is not desired the
**                      feature should be disabled in the controller's feature mask.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_BUSY if there are one or more active (e)SCO links.
**
*******************************************************************************/
tBTM_STATUS BTM_SetEScoMode (tBTM_SCO_TYPE sco_mode, tBTM_ESCO_PARAMS *p_parms)
{
    tSCO_CB          *p_esco = &btm_cb.sco_cb;
    tBTM_ESCO_PARAMS *p_def = &p_esco->def_esco_parms;

    if (p_esco->esco_supported) {
        if (p_parms) {
            if (sco_mode == BTM_LINK_TYPE_ESCO) {
                *p_def = *p_parms;    /* Save as the default parameters */
            } else { /* Load only the SCO packet types */
                p_def->packet_types = p_parms->packet_types;
                p_def->tx_bw            = BTM_64KBITS_RATE;
                p_def->rx_bw            = BTM_64KBITS_RATE;
                p_def->max_latency      = 0x000a;
                p_def->voice_contfmt    = 0x0060;
                p_def->retrans_effort   = 0;

                /* OR in any exception packet types */
                p_def->packet_types |= BTM_SCO_EXCEPTION_PKTS_MASK;
            }
        }
        p_esco->desired_sco_mode = sco_mode;
        BTM_TRACE_API("BTM_SetEScoMode -> mode %d",  sco_mode);
    } else {
        p_esco->desired_sco_mode = BTM_LINK_TYPE_SCO;
        p_def->packet_types &= BTM_SCO_LINK_ONLY_MASK;
        p_def->retrans_effort = 0;
        BTM_TRACE_API("BTM_SetEScoMode -> mode SCO (eSCO not supported)");
    }

    BTM_TRACE_DEBUG("    txbw 0x%08x, rxbw 0x%08x, max_lat 0x%04x, voice 0x%04x, pkt 0x%04x, rtx effort 0x%02x",
                    p_def->tx_bw, p_def->rx_bw, p_def->max_latency,
                    p_def->voice_contfmt, p_def->packet_types,
                    p_def->retrans_effort);

    return (BTM_SUCCESS);
}



/*******************************************************************************
**
** Function         BTM_RegForEScoEvts
**
** Description      This function registers a SCO event callback with the
**                  specified instance.  It should be used to received
**                  connection indication events and change of link parameter
**                  events.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_ILLEGAL_VALUE if there is an illegal sco_inx
**                  BTM_MODE_UNSUPPORTED if controller version is not BT1.2 or
**                          later or does not support eSCO.
**
*******************************************************************************/
tBTM_STATUS BTM_RegForEScoEvts (UINT16 sco_inx, tBTM_ESCO_CBACK *p_esco_cback)
{
#if (BTM_MAX_SCO_LINKS>0)
    if (!btm_cb.sco_cb.esco_supported) {
        btm_cb.sco_cb.sco_db[sco_inx].esco.p_esco_cback = NULL;
        return (BTM_MODE_UNSUPPORTED);
    }

    if (sco_inx < BTM_MAX_SCO_LINKS &&
            btm_cb.sco_cb.sco_db[sco_inx].state != SCO_ST_UNUSED) {
        btm_cb.sco_cb.sco_db[sco_inx].esco.p_esco_cback = p_esco_cback;
        return (BTM_SUCCESS);
    }
    return (BTM_ILLEGAL_VALUE);
#else
    return (BTM_MODE_UNSUPPORTED);
#endif
}

/*******************************************************************************
**
** Function         BTM_ReadEScoLinkParms
**
** Description      This function returns the current eSCO link parameters for
**                  the specified handle.  This can be called anytime a connection
**                  is active, but is typically called after receiving the SCO
**                  opened callback.
**
**                  Note: If called over a 1.1 controller, only the packet types
**                        field has meaning.
**
** Returns          BTM_SUCCESS if returned data is valid connection.
**                  BTM_WRONG_MODE if no connection with a peer device or bad sco_inx.
**
*******************************************************************************/
tBTM_STATUS BTM_ReadEScoLinkParms (UINT16 sco_inx, tBTM_ESCO_DATA *p_parms)
{
#if (BTM_MAX_SCO_LINKS>0)
    UINT8 index;

    BTM_TRACE_API("BTM_ReadEScoLinkParms -> sco_inx 0x%04x", sco_inx);

    if (sco_inx < BTM_MAX_SCO_LINKS &&
            btm_cb.sco_cb.sco_db[sco_inx].state >= SCO_ST_CONNECTED) {
        *p_parms = btm_cb.sco_cb.sco_db[sco_inx].esco.data;
        return (BTM_SUCCESS);
    }

    if (sco_inx == BTM_FIRST_ACTIVE_SCO_INDEX) {
        for (index = 0; index < BTM_MAX_SCO_LINKS; index++) {
            if (btm_cb.sco_cb.sco_db[index].state >= SCO_ST_CONNECTED) {
                BTM_TRACE_API("BTM_ReadEScoLinkParms the first active SCO index is %d", index);
                *p_parms = btm_cb.sco_cb.sco_db[index].esco.data;
                return (BTM_SUCCESS);
            }
        }
    }

#endif

    BTM_TRACE_API("BTM_ReadEScoLinkParms cannot find the SCO index!");
    memset(p_parms, 0, sizeof(tBTM_ESCO_DATA));
    return (BTM_WRONG_MODE);
}

/*******************************************************************************
**
** Function         BTM_ChangeEScoLinkParms
**
** Description      This function requests renegotiation of the parameters on
**                  the current eSCO Link.  If any of the changes are accepted
**                  by the controllers, the BTM_ESCO_CHG_EVT event is sent in
**                  the tBTM_ESCO_CBACK function with the current settings of
**                  the link. The callback is registered through the call to
**                  BTM_SetEScoMode.
**
**                  Note: If called over a SCO link (including 1.1 controller),
**                        a change packet type request is sent out instead.
**
** Returns          BTM_CMD_STARTED if command is successfully initiated.
**                  BTM_NO_RESOURCES - not enough resources to initiate command.
**                  BTM_WRONG_MODE if no connection with a peer device or bad sco_inx.
**
*******************************************************************************/
tBTM_STATUS BTM_ChangeEScoLinkParms (UINT16 sco_inx, tBTM_CHG_ESCO_PARAMS *p_parms)
{
#if (BTM_MAX_SCO_LINKS>0)
    tBTM_ESCO_PARAMS *p_setup;
    tSCO_CONN        *p_sco;
    UINT16            temp_pkt_types;

    /* Make sure sco handle is valid and on an active link */
    if (sco_inx >= BTM_MAX_SCO_LINKS ||
            btm_cb.sco_cb.sco_db[sco_inx].state != SCO_ST_CONNECTED) {
        return (BTM_WRONG_MODE);
    }

    p_sco = &btm_cb.sco_cb.sco_db[sco_inx];
    p_setup = &p_sco->esco.setup;

    /* If SCO connection OR eSCO not supported just send change packet types */
    if (p_sco->esco.data.link_type == BTM_LINK_TYPE_SCO ||
            !btm_cb.sco_cb.esco_supported) {
        p_setup->packet_types = p_parms->packet_types &
                                (btm_cb.btm_sco_pkt_types_supported & BTM_SCO_LINK_ONLY_MASK);


        BTM_TRACE_API("BTM_ChangeEScoLinkParms -> SCO Link for handle 0x%04x, pkt 0x%04x",
                      p_sco->hci_handle, p_setup->packet_types);

        if (!btsnd_hcic_change_conn_type (p_sco->hci_handle,
                                          BTM_ESCO_2_SCO(p_setup->packet_types))) {
            return (BTM_NO_RESOURCES);
        }
    } else {
        temp_pkt_types = (p_parms->packet_types & BTM_SCO_SUPPORTED_PKTS_MASK &
                          btm_cb.btm_sco_pkt_types_supported);

        /* OR in any exception packet types */
        temp_pkt_types |= ((p_parms->packet_types & BTM_SCO_EXCEPTION_PKTS_MASK) |
                           (btm_cb.btm_sco_pkt_types_supported & BTM_SCO_EXCEPTION_PKTS_MASK));

        BTM_TRACE_API("BTM_ChangeEScoLinkParms -> eSCO Link for handle 0x%04x", p_sco->hci_handle);
        BTM_TRACE_API("      txbw 0x%x, rxbw 0x%x, lat 0x%x, voice 0x%x, retrans 0x%02x, pkt 0x%04x",
                      p_setup->tx_bw, p_setup->rx_bw, p_parms->max_latency,
                      p_setup->voice_contfmt, p_parms->retrans_effort, temp_pkt_types);

        /* When changing an existing link, only change latency, retrans, and pkts */
        if (!btsnd_hcic_setup_esco_conn(p_sco->hci_handle, p_setup->tx_bw,
                                        p_setup->rx_bw, p_parms->max_latency,
                                        p_setup->voice_contfmt,
                                        p_parms->retrans_effort,
                                        temp_pkt_types)) {
            return (BTM_NO_RESOURCES);
        } else {
            p_parms->packet_types = temp_pkt_types;
        }
    }

    return (BTM_CMD_STARTED);
#else
    return (BTM_WRONG_MODE);
#endif
}

/*******************************************************************************
**
** Function         BTM_EScoConnRsp
**
** Description      This function is called upon receipt of an (e)SCO connection
**                  request event (BTM_ESCO_CONN_REQ_EVT) to accept or reject
**                  the request. Parameters used to negotiate eSCO links.
**                  If p_parms is NULL, then values set through BTM_SetEScoMode
**                  are used.
**                  If the link type of the incoming request is SCO, then only
**                  the tx_bw, max_latency, content format, and packet_types are
**                  valid.  The hci_status parameter should be
**                  ([0x0] to accept, [0x0d..0x0f] to reject)
**
**
** Returns          void
**
*******************************************************************************/
void BTM_EScoConnRsp (UINT16 sco_inx, UINT8 hci_status, tBTM_ESCO_PARAMS *p_parms)
{
#if (BTM_MAX_SCO_LINKS>0)
    if (sco_inx < BTM_MAX_SCO_LINKS &&
            btm_cb.sco_cb.sco_db[sco_inx].state == SCO_ST_W4_CONN_RSP) {
        btm_esco_conn_rsp(sco_inx, hci_status,
                          btm_cb.sco_cb.sco_db[sco_inx].esco.data.bd_addr,
                          p_parms);
    }
#endif
}

/*******************************************************************************
**
** Function         btm_read_def_esco_mode
**
** Description      This function copies the current default esco settings into
**                  the return buffer.
**
** Returns          tBTM_SCO_TYPE
**
*******************************************************************************/
tBTM_SCO_TYPE btm_read_def_esco_mode (tBTM_ESCO_PARAMS *p_parms)
{
#if (BTM_MAX_SCO_LINKS>0)
    *p_parms = btm_cb.sco_cb.def_esco_parms;
    return btm_cb.sco_cb.desired_sco_mode;
#else
    return BTM_LINK_TYPE_SCO;
#endif
}

/*******************************************************************************
**
** Function         btm_esco_proc_conn_chg
**
** Description      This function is called by BTIF when an SCO connection
**                  is changed.
**
** Returns          void
**
*******************************************************************************/
void btm_esco_proc_conn_chg (UINT8 status, UINT16 handle, UINT8 tx_interval,
                             UINT8 retrans_window, UINT16 rx_pkt_len,
                             UINT16 tx_pkt_len)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN               *p = &btm_cb.sco_cb.sco_db[0];
    tBTM_CHG_ESCO_EVT_DATA   data;
    UINT16                   xx;

    BTM_TRACE_EVENT("btm_esco_proc_conn_chg -> handle 0x%04x, status 0x%02x",
                    handle, status);

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (p->state == SCO_ST_CONNECTED && handle == p->hci_handle) {
            /* If upper layer wants notification */
            if (p->esco.p_esco_cback) {
                memcpy(data.bd_addr, p->esco.data.bd_addr, BD_ADDR_LEN);
                data.hci_status = status;
                data.sco_inx = xx;
                data.rx_pkt_len = p->esco.data.rx_pkt_len = rx_pkt_len;
                data.tx_pkt_len = p->esco.data.tx_pkt_len = tx_pkt_len;
                data.tx_interval = p->esco.data.tx_interval = tx_interval;
                data.retrans_window = p->esco.data.retrans_window = retrans_window;

                (*p->esco.p_esco_cback)(BTM_ESCO_CHG_EVT,
                                        (tBTM_ESCO_EVT_DATA *)&data);
            }
            return;
        }
    }
#endif
}

/*******************************************************************************
**
** Function         btm_is_sco_active
**
** Description      This function is called to see if a SCO handle is already in
**                  use.
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btm_is_sco_active (UINT16 handle)
{
#if (BTM_MAX_SCO_LINKS>0)
    UINT16     xx;
    tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if (handle == p->hci_handle && p->state == SCO_ST_CONNECTED) {
            return (TRUE);
        }
    }
#endif
    return (FALSE);
}

/*******************************************************************************
**
** Function         BTM_GetNumScoLinks
**
** Description      This function returns the number of active sco links.
**
** Returns          UINT8
**
*******************************************************************************/
UINT8 BTM_GetNumScoLinks (void)
{
#if (BTM_MAX_SCO_LINKS>0)
    tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];
    UINT16     xx;
    UINT8      num_scos = 0;

    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        switch (p->state) {
        case SCO_ST_W4_CONN_RSP:
        case SCO_ST_CONNECTING:
        case SCO_ST_CONNECTED:
        case SCO_ST_DISCONNECTING:
        case SCO_ST_PEND_UNPARK:
            num_scos++;
        }
    }
    return (num_scos);
#else
    return (0);
#endif
}


/*******************************************************************************
**
** Function         btm_is_sco_active_by_bdaddr
**
** Description      This function is called to see if a SCO active to a bd address.
**
** Returns          BOOLEAN
**
*******************************************************************************/
BOOLEAN btm_is_sco_active_by_bdaddr (BD_ADDR remote_bda)
{
#if (BTM_MAX_SCO_LINKS>0)
    UINT8 xx;
    tSCO_CONN *p = &btm_cb.sco_cb.sco_db[0];

    /* If any SCO is being established to the remote BD address, refuse this */
    for (xx = 0; xx < BTM_MAX_SCO_LINKS; xx++, p++) {
        if ((!memcmp (p->esco.data.bd_addr, remote_bda, BD_ADDR_LEN)) && (p->state == SCO_ST_CONNECTED)) {
            return (TRUE);
        }
    }
#endif
    return (FALSE);
}
#else   /* SCO_EXCLUDED == TRUE (Link in stubs) */

tBTM_STATUS BTM_CreateSco (BD_ADDR remote_bda, BOOLEAN is_orig,
                           UINT16 pkt_types, UINT16 *p_sco_inx,
                           tBTM_SCO_CB *p_conn_cb,
                           tBTM_SCO_CB *p_disc_cb)
{
    return (BTM_NO_RESOURCES);
}
tBTM_STATUS BTM_RemoveSco (UINT16 sco_inx)
{
    return (BTM_NO_RESOURCES);
}
tBTM_STATUS BTM_SetScoPacketTypes (UINT16 sco_inx, UINT16 pkt_types)
{
    return (BTM_NO_RESOURCES);
}
UINT16 BTM_ReadScoPacketTypes (UINT16 sco_inx)
{
    return (0);
}
UINT16 BTM_ReadDeviceScoPacketTypes (void)
{
    return (0);
}
UINT16 BTM_ReadScoHandle (UINT16 sco_inx)
{
    return (BTM_INVALID_HCI_HANDLE);
}
UINT8 *BTM_ReadScoBdAddr(UINT16 sco_inx)
{
    return ((UINT8 *) NULL);
}
UINT16 BTM_ReadScoDiscReason (void)
{
    return (BTM_INVALID_SCO_DISC_REASON);
}
tBTM_STATUS BTM_SetEScoMode (tBTM_SCO_TYPE sco_mode, tBTM_ESCO_PARAMS *p_parms)
{
    return (BTM_MODE_UNSUPPORTED);
}
tBTM_STATUS BTM_RegForEScoEvts (UINT16 sco_inx, tBTM_ESCO_CBACK *p_esco_cback)
{
    return (BTM_ILLEGAL_VALUE);
}
tBTM_STATUS BTM_ReadEScoLinkParms (UINT16 sco_inx, tBTM_ESCO_DATA *p_parms)
{
    return (BTM_MODE_UNSUPPORTED);
}
tBTM_STATUS BTM_ChangeEScoLinkParms (UINT16 sco_inx, tBTM_CHG_ESCO_PARAMS *p_parms)
{
    return (BTM_MODE_UNSUPPORTED);
}
void BTM_EScoConnRsp (UINT16 sco_inx, UINT8 hci_status, tBTM_ESCO_PARAMS *p_parms) {}
UINT8 BTM_GetNumScoLinks (void)
{
    return (0);
}

#endif /* If SCO is being used */
