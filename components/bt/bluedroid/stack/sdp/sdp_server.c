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
 *  This file contains functions that handle the SDP server functions.
 *  This is mainly dealing with client requests
 *
 ******************************************************************************/

//#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include "bt_types.h"
#include "allocator.h"
#include "btu.h"
#include "bt_defs.h"
#include "l2cdefs.h"
#include "hcidefs.h"
#include "hcimsgs.h"

#include "sdp_api.h"
#include "sdpint.h"


#if SDP_SERVER_ENABLED == TRUE

/* Maximum number of bytes to reserve out of SDP MTU for response data */
#define SDP_MAX_SERVICE_RSPHDR_LEN      12
#define SDP_MAX_SERVATTR_RSPHDR_LEN     10
#define SDP_MAX_ATTR_RSPHDR_LEN         10

/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/
static void process_service_search (tCONN_CB *p_ccb, UINT16 trans_num,
                                    UINT16 param_len, UINT8 *p_req,
                                    UINT8 *p_req_end);

static void process_service_attr_req (tCONN_CB *p_ccb, UINT16 trans_num,
                                      UINT16 param_len, UINT8 *p_req,
                                      UINT8 *p_req_end);

static void process_service_search_attr_req (tCONN_CB *p_ccb, UINT16 trans_num,
        UINT16 param_len, UINT8 *p_req,
        UINT8 *p_req_end);


/********************************************************************************/
/*                  E R R O R   T E X T   S T R I N G S                         */
/*                                                                              */
/* The default is to have no text string, but we allow the strings to be        */
/* configured in target.h if people want them.                                  */
/********************************************************************************/
#ifndef SDP_TEXT_BAD_HEADER
#define SDP_TEXT_BAD_HEADER     NULL
#endif

#ifndef SDP_TEXT_BAD_PDU
#define SDP_TEXT_BAD_PDU        NULL
#endif

#ifndef SDP_TEXT_BAD_UUID_LIST
#define SDP_TEXT_BAD_UUID_LIST  NULL
#endif

#ifndef SDP_TEXT_BAD_HANDLE
#define SDP_TEXT_BAD_HANDLE     NULL
#endif

#ifndef SDP_TEXT_BAD_ATTR_LIST
#define SDP_TEXT_BAD_ATTR_LIST  NULL
#endif

#ifndef SDP_TEXT_BAD_CONT_LEN
#define SDP_TEXT_BAD_CONT_LEN   NULL
#endif

#ifndef SDP_TEXT_BAD_CONT_INX
#define SDP_TEXT_BAD_CONT_INX   NULL
#endif

#ifndef SDP_TEXT_BAD_MAX_RECORDS_LIST
#define SDP_TEXT_BAD_MAX_RECORDS_LIST   NULL
#endif

/*******************************************************************************
**
** Function         sdp_server_handle_client_req
**
** Description      This is the main dispatcher of the SDP server. It is called
**                  when any data is received from L2CAP, and dispatches the
**                  request to the appropriate handler.
**
** Returns          void
**
*******************************************************************************/
void sdp_server_handle_client_req (tCONN_CB *p_ccb, BT_HDR *p_msg)
{
    UINT8   *p_req     = (UINT8 *) (p_msg + 1) + p_msg->offset;
    UINT8   *p_req_end = p_req + p_msg->len;
    UINT8   pdu_id;
    UINT16  trans_num, param_len;


    /* Start inactivity timer */
    btu_start_timer (&p_ccb->timer_entry, BTU_TTYPE_SDP, SDP_INACT_TIMEOUT);

    /* The first byte in the message is the pdu type */
    pdu_id = *p_req++;

    /* Extract the transaction number and parameter length */
    BE_STREAM_TO_UINT16 (trans_num, p_req);
    BE_STREAM_TO_UINT16 (param_len, p_req);

    if ((p_req + param_len) != p_req_end) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_PDU_SIZE, SDP_TEXT_BAD_HEADER);
        return;
    }

    switch (pdu_id) {
    case SDP_PDU_SERVICE_SEARCH_REQ:
        process_service_search (p_ccb, trans_num, param_len, p_req, p_req_end);
        break;

    case SDP_PDU_SERVICE_ATTR_REQ:
        process_service_attr_req (p_ccb, trans_num, param_len, p_req, p_req_end);
        break;

    case SDP_PDU_SERVICE_SEARCH_ATTR_REQ:
        process_service_search_attr_req (p_ccb, trans_num, param_len, p_req, p_req_end);
        break;

    default:
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_PDU);
        SDP_TRACE_WARNING ("SDP - server got unknown PDU: 0x%x\n", pdu_id);
        break;
    }
}



/*******************************************************************************
**
** Function         process_service_search
**
** Description      This function handles a service search request from the
**                  client. It builds a reply message with info from the database,
**                  and sends the reply back to the client.
**
** Returns          void
**
*******************************************************************************/
static void process_service_search (tCONN_CB *p_ccb, UINT16 trans_num,
                                    UINT16 param_len, UINT8 *p_req,
                                    UINT8 *p_req_end)
{
    UINT16          max_replies, cur_handles, rem_handles, cont_offset;
    tSDP_UUID_SEQ   uid_seq;
    UINT8           *p_rsp, *p_rsp_start, *p_rsp_param_len;
    UINT16          rsp_param_len, num_rsp_handles, xx;
    UINT32          rsp_handles[SDP_MAX_RECORDS] = {0};
    tSDP_RECORD    *p_rec = NULL;
    BT_HDR         *p_buf;
    BOOLEAN         is_cont = FALSE;
    UNUSED(p_req_end);

    p_req = sdpu_extract_uid_seq (p_req, param_len, &uid_seq);

    if ((!p_req) || (!uid_seq.num_uids)) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_UUID_LIST);
        return;
    }

    /* Get the max replies we can send. Cap it at our max anyways. */
    BE_STREAM_TO_UINT16 (max_replies, p_req);

    if (max_replies > SDP_MAX_RECORDS) {
        max_replies = SDP_MAX_RECORDS;
    }


    if ((!p_req) || (p_req > p_req_end)) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_MAX_RECORDS_LIST);
        return;
    }


    /* Get a list of handles that match the UUIDs given to us */
    for (num_rsp_handles = 0; num_rsp_handles < max_replies; ) {
        p_rec = sdp_db_service_search (p_rec, &uid_seq);

        if (p_rec) {
            rsp_handles[num_rsp_handles++] = p_rec->record_handle;
        } else {
            break;
        }
    }

    /* Check if this is a continuation request */
    if (*p_req) {
        if (*p_req++ != SDP_CONTINUATION_LEN || (p_req >= p_req_end)) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE,
                                     SDP_TEXT_BAD_CONT_LEN);
            return;
        }
        BE_STREAM_TO_UINT16 (cont_offset, p_req);

        if (cont_offset != p_ccb->cont_offset || num_rsp_handles < cont_offset) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE,
                                     SDP_TEXT_BAD_CONT_INX);
            return;
        }

        rem_handles = num_rsp_handles - cont_offset;    /* extract the remaining handles */
    } else {
        rem_handles = num_rsp_handles;
        cont_offset = 0;
        p_ccb->cont_offset = 0;
    }

    /* Calculate how many handles will fit in one PDU */
    cur_handles = (UINT16)((p_ccb->rem_mtu_size - SDP_MAX_SERVICE_RSPHDR_LEN) / 4);

    if (rem_handles <= cur_handles) {
        cur_handles = rem_handles;
    } else { /* Continuation is set */
        p_ccb->cont_offset += cur_handles;
        is_cont = TRUE;
    }

    /* Get a buffer to use to build the response */
    if ((p_buf = (BT_HDR *)osi_malloc(SDP_DATA_BUF_SIZE)) == NULL) {
        SDP_TRACE_ERROR ("SDP - no buf for search rsp\n");
        return;
    }
    p_buf->offset = L2CAP_MIN_OFFSET;
    p_rsp = p_rsp_start = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Start building a rsponse */
    UINT8_TO_BE_STREAM  (p_rsp, SDP_PDU_SERVICE_SEARCH_RSP);
    UINT16_TO_BE_STREAM (p_rsp, trans_num);

    /* Skip the length, we need to add it at the end */
    p_rsp_param_len = p_rsp;
    p_rsp += 2;

    /* Put in total and current number of handles, and handles themselves */
    UINT16_TO_BE_STREAM (p_rsp, num_rsp_handles);
    UINT16_TO_BE_STREAM (p_rsp, cur_handles);

    /*    SDP_TRACE_DEBUG("SDP Service Rsp: tothdl %d, curhdlr %d, start %d, end %d, cont %d",
                         num_rsp_handles, cur_handles, cont_offset,
                         cont_offset + cur_handles-1, is_cont); */
    for (xx = cont_offset; xx < cont_offset + cur_handles; xx++) {
        UINT32_TO_BE_STREAM (p_rsp, rsp_handles[xx]);
    }

    if (is_cont) {
        UINT8_TO_BE_STREAM  (p_rsp, SDP_CONTINUATION_LEN);
        UINT16_TO_BE_STREAM (p_rsp, p_ccb->cont_offset);
    } else {
        UINT8_TO_BE_STREAM (p_rsp, 0);
    }

    /* Go back and put the parameter length into the buffer */
    rsp_param_len = p_rsp - p_rsp_param_len - 2;
    UINT16_TO_BE_STREAM (p_rsp_param_len, rsp_param_len);

    /* Set the length of the SDP data in the buffer */
    p_buf->len = p_rsp - p_rsp_start;


    /* Send the buffer through L2CAP */
    L2CA_DataWrite (p_ccb->connection_id, p_buf);
}


/*******************************************************************************
**
** Function         process_service_attr_req
**
** Description      This function handles an attribute request from the client.
**                  It builds a reply message with info from the database,
**                  and sends the reply back to the client.
**
** Returns          void
**
*******************************************************************************/
static void process_service_attr_req (tCONN_CB *p_ccb, UINT16 trans_num,
                                      UINT16 param_len, UINT8 *p_req,
                                      UINT8 *p_req_end)
{
    UINT16          max_list_len, len_to_send, cont_offset;
    INT16           rem_len;
    tSDP_ATTR_SEQ   attr_seq, attr_seq_sav;
    UINT8           *p_rsp, *p_rsp_start, *p_rsp_param_len;
    UINT16          rsp_param_len, xx;
    UINT32          rec_handle;
    tSDP_RECORD     *p_rec;
    tSDP_ATTRIBUTE  *p_attr;
    BT_HDR          *p_buf;
    BOOLEAN         is_cont = FALSE;
    UINT16          attr_len;

    /* Extract the record handle */
    BE_STREAM_TO_UINT32 (rec_handle, p_req);

    if (p_req > p_req_end) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_SERV_REC_HDL, SDP_TEXT_BAD_HANDLE);
        return;
    }

    /* Get the max list length we can send. Cap it at MTU size minus overhead */
    BE_STREAM_TO_UINT16 (max_list_len, p_req);

    if (max_list_len > (p_ccb->rem_mtu_size - SDP_MAX_ATTR_RSPHDR_LEN)) {
        max_list_len = p_ccb->rem_mtu_size - SDP_MAX_ATTR_RSPHDR_LEN;
    }

    p_req = sdpu_extract_attr_seq (p_req, param_len, &attr_seq);

    if ((!p_req) || (!attr_seq.num_attr) || (p_req > p_req_end)) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_ATTR_LIST);
        return;
    }

    memcpy(&attr_seq_sav, &attr_seq, sizeof(tSDP_ATTR_SEQ)) ;

    /* Find a record with the record handle */
    p_rec = sdp_db_find_record (rec_handle);
    if (!p_rec) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_SERV_REC_HDL, SDP_TEXT_BAD_HANDLE);
        return;
    }

    /* Check if this is a continuation request */
    if (*p_req) {
        /* Free and reallocate buffer */
        if (p_ccb->rsp_list) {
            osi_free(p_ccb->rsp_list);
        }

        p_ccb->rsp_list = (UINT8 *)osi_malloc(max_list_len);
        if (p_ccb->rsp_list == NULL) {
            SDP_TRACE_ERROR("%s No scratch buf for attr rsp\n", __func__);
            return;
        }

        if (*p_req++ != SDP_CONTINUATION_LEN) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE, SDP_TEXT_BAD_CONT_LEN);
            return;
        }
        BE_STREAM_TO_UINT16 (cont_offset, p_req);

        if (cont_offset != p_ccb->cont_offset) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE, SDP_TEXT_BAD_CONT_INX);
            return;
        }

        if (!p_ccb->rsp_list) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_NO_RESOURCES, NULL);
            return;
        }
        is_cont = TRUE;

        /* Initialise for continuation response */
        p_rsp = &p_ccb->rsp_list[0];
        attr_seq.attr_entry[p_ccb->cont_info.next_attr_index].start = p_ccb->cont_info.next_attr_start_id;
    } else {
	    if (p_ccb->rsp_list) {
            osi_free (p_ccb->rsp_list);
        }

        p_ccb->rsp_list = (UINT8 *)osi_malloc (max_list_len);
        if (p_ccb->rsp_list == NULL) {
            SDP_TRACE_ERROR ("SDP - no scratch buf for search rsp\n");
            return;
        }

        p_ccb->cont_offset = 0;
        p_rsp = &p_ccb->rsp_list[3];            /* Leave space for data elem descr */

        /* Reset continuation parameters in p_ccb */
        p_ccb->cont_info.prev_sdp_rec = NULL;
        p_ccb->cont_info.next_attr_index = 0;
        p_ccb->cont_info.attr_offset = 0;
    }

    /* Search for attributes that match the list given to us */
    for (xx = p_ccb->cont_info.next_attr_index; xx < attr_seq.num_attr; xx++) {
        p_attr = sdp_db_find_attr_in_rec (p_rec, attr_seq.attr_entry[xx].start, attr_seq.attr_entry[xx].end);

        if (p_attr) {
            /* Check if attribute fits. Assume 3-byte value type/length */
            rem_len = max_list_len - (INT16) (p_rsp - &p_ccb->rsp_list[0]);

            /* just in case */
            if (rem_len <= 0) {
                p_ccb->cont_info.next_attr_index = xx;
                p_ccb->cont_info.next_attr_start_id = p_attr->id;
                break;
            }

            attr_len = sdpu_get_attrib_entry_len(p_attr);
            /* if there is a partial attribute pending to be sent */
            if (p_ccb->cont_info.attr_offset) {
                p_rsp = sdpu_build_partial_attrib_entry (p_rsp, p_attr, rem_len,
                        &p_ccb->cont_info.attr_offset);

                /* If the partial attrib could not been fully added yet */
                if (p_ccb->cont_info.attr_offset != attr_len) {
                    break;
                } else { /* If the partial attrib has been added in full by now */
                    p_ccb->cont_info.attr_offset = 0;    /* reset attr_offset */
                }
            } else if (rem_len < attr_len) { /* Not enough space for attr... so add partially */
                if (attr_len >= SDP_MAX_ATTR_LEN) {
                    SDP_TRACE_ERROR("SDP attr too big: max_list_len=%d,attr_len=%d\n", max_list_len, attr_len);
                    sdpu_build_n_send_error (p_ccb, trans_num, SDP_NO_RESOURCES, NULL);
                    return;
                }

                /* add the partial attribute if possible */
                p_rsp = sdpu_build_partial_attrib_entry (p_rsp, p_attr, (UINT16)rem_len,
                        &p_ccb->cont_info.attr_offset);

                p_ccb->cont_info.next_attr_index = xx;
                p_ccb->cont_info.next_attr_start_id = p_attr->id;
                break;
            } else { /* build the whole attribute */
                p_rsp = sdpu_build_attrib_entry (p_rsp, p_attr);
            }

            /* If doing a range, stick with this one till no more attributes found */
            if (attr_seq.attr_entry[xx].start != attr_seq.attr_entry[xx].end) {
                /* Update for next time through */
                attr_seq.attr_entry[xx].start = p_attr->id + 1;

                xx--;
            }
        }
    }
    /* If all the attributes have been accomodated in p_rsp,
       reset next_attr_index */
    if (xx == attr_seq.num_attr) {
        p_ccb->cont_info.next_attr_index = 0;
    }

    len_to_send = (UINT16) (p_rsp - &p_ccb->rsp_list[0]);
    cont_offset = 0;

    if (!is_cont) {
        p_ccb->list_len = sdpu_get_attrib_seq_len(p_rec, &attr_seq_sav) + 3;
        /* Put in the sequence header (2 or 3 bytes) */
        if (p_ccb->list_len > 255) {
            p_ccb->rsp_list[0] = (UINT8) ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_WORD);
            p_ccb->rsp_list[1] = (UINT8) ((p_ccb->list_len - 3) >> 8);
            p_ccb->rsp_list[2] = (UINT8) (p_ccb->list_len - 3);
        } else {
            cont_offset = 1;

            p_ccb->rsp_list[1] = (UINT8) ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
            p_ccb->rsp_list[2] = (UINT8) (p_ccb->list_len - 3);

            p_ccb->list_len--;
            len_to_send--;
        }
    }

    /* Get a buffer to use to build the response */
    if ((p_buf = (BT_HDR *)osi_malloc(SDP_DATA_BUF_SIZE)) == NULL) {
        SDP_TRACE_ERROR ("SDP - no buf for search rsp\n");
        return;
    }
    p_buf->offset = L2CAP_MIN_OFFSET;
    p_rsp = p_rsp_start = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Start building a rsponse */
    UINT8_TO_BE_STREAM  (p_rsp, SDP_PDU_SERVICE_ATTR_RSP);
    UINT16_TO_BE_STREAM (p_rsp, trans_num);

    /* Skip the parameter length, add it when we know the length */
    p_rsp_param_len = p_rsp;
    p_rsp += 2;

    UINT16_TO_BE_STREAM (p_rsp, len_to_send);

    memcpy (p_rsp, &p_ccb->rsp_list[cont_offset], len_to_send);
    p_rsp += len_to_send;

    p_ccb->cont_offset += len_to_send;

    /* If anything left to send, continuation needed */
    if (p_ccb->cont_offset < p_ccb->list_len) {
        is_cont = TRUE;

        UINT8_TO_BE_STREAM  (p_rsp, SDP_CONTINUATION_LEN);
        UINT16_TO_BE_STREAM (p_rsp, p_ccb->cont_offset);
    } else {
        UINT8_TO_BE_STREAM (p_rsp, 0);
    }

    /* Go back and put the parameter length into the buffer */
    rsp_param_len = p_rsp - p_rsp_param_len - 2;
    UINT16_TO_BE_STREAM (p_rsp_param_len, rsp_param_len);

    /* Set the length of the SDP data in the buffer */
    p_buf->len = p_rsp - p_rsp_start;


    /* Send the buffer through L2CAP */
    L2CA_DataWrite (p_ccb->connection_id, p_buf);
}



/*******************************************************************************
**
** Function         process_service_search_attr_req
**
** Description      This function handles a combined service search and attribute
**                  read request from the client. It builds a reply message with
**                  info from the database, and sends the reply back to the client.
**
** Returns          void
**
*******************************************************************************/
static void process_service_search_attr_req (tCONN_CB *p_ccb, UINT16 trans_num,
        UINT16 param_len, UINT8 *p_req,
        UINT8 *p_req_end)
{
    UINT16         max_list_len;
    INT16          rem_len;
    UINT16         len_to_send, cont_offset;
    tSDP_UUID_SEQ   uid_seq;
    UINT8           *p_rsp, *p_rsp_start, *p_rsp_param_len;
    UINT16          rsp_param_len, xx;
    tSDP_RECORD    *p_rec;
    tSDP_ATTR_SEQ   attr_seq, attr_seq_sav;
    tSDP_ATTRIBUTE *p_attr;
    BT_HDR         *p_buf;
    BOOLEAN         maxxed_out = FALSE, is_cont = FALSE;
    UINT8           *p_seq_start;
    UINT16          seq_len, attr_len;
    UNUSED(p_req_end);

    /* Extract the UUID sequence to search for */
    p_req = sdpu_extract_uid_seq (p_req, param_len, &uid_seq);

    if ((!p_req) || (!uid_seq.num_uids)) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_UUID_LIST);
        return;
    }

    /* Get the max list length we can send. Cap it at our max list length. */
    BE_STREAM_TO_UINT16 (max_list_len, p_req);

    if (max_list_len > (p_ccb->rem_mtu_size - SDP_MAX_SERVATTR_RSPHDR_LEN)) {
        max_list_len = p_ccb->rem_mtu_size - SDP_MAX_SERVATTR_RSPHDR_LEN;
    }

    p_req = sdpu_extract_attr_seq (p_req, param_len, &attr_seq);

    if ((!p_req) || (!attr_seq.num_attr)) {
        sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_REQ_SYNTAX, SDP_TEXT_BAD_ATTR_LIST);
        return;
    }

    memcpy(&attr_seq_sav, &attr_seq, sizeof(tSDP_ATTR_SEQ)) ;

    /* Check if this is a continuation request */
    if (*p_req) {
        /* Free and reallocate buffer */
        if (p_ccb->rsp_list) {
            osi_free (p_ccb->rsp_list);
        }

        p_ccb->rsp_list = (UINT8 *)osi_malloc (max_list_len);
        if (p_ccb->rsp_list == NULL) {
            SDP_TRACE_ERROR ("SDP - no scratch buf for search rsp\n");
            return;
        }

        if (*p_req++ != SDP_CONTINUATION_LEN) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE, SDP_TEXT_BAD_CONT_LEN);
            return;
        }
        BE_STREAM_TO_UINT16 (cont_offset, p_req);

        if (cont_offset != p_ccb->cont_offset) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_INVALID_CONT_STATE, SDP_TEXT_BAD_CONT_INX);
            return;
        }

        if (!p_ccb->rsp_list) {
            sdpu_build_n_send_error (p_ccb, trans_num, SDP_NO_RESOURCES, NULL);
            return;
        }
        is_cont = TRUE;

        /* Initialise for continuation response */
        p_rsp = &p_ccb->rsp_list[0];
        attr_seq.attr_entry[p_ccb->cont_info.next_attr_index].start = p_ccb->cont_info.next_attr_start_id;
    } else {
        /* Get a scratch buffer to store response */
        /* Free and reallocate if the earlier allocated buffer is small */
        if (p_ccb->rsp_list) {
            osi_free (p_ccb->rsp_list);
        }

        p_ccb->rsp_list = (UINT8 *)osi_malloc (max_list_len);
        if (p_ccb->rsp_list == NULL) {
            SDP_TRACE_ERROR ("SDP - no scratch buf for search rsp\n");
            return;
        }

        p_ccb->cont_offset = 0;
        p_rsp = &p_ccb->rsp_list[3];            /* Leave space for data elem descr */

        /* Reset continuation parameters in p_ccb */
        p_ccb->cont_info.prev_sdp_rec = NULL;
        p_ccb->cont_info.next_attr_index = 0;
        p_ccb->cont_info.last_attr_seq_desc_sent = FALSE;
        p_ccb->cont_info.attr_offset = 0;
    }

    /* Get a list of handles that match the UUIDs given to us */
    for (p_rec = sdp_db_service_search (p_ccb->cont_info.prev_sdp_rec, &uid_seq); p_rec; p_rec = sdp_db_service_search (p_rec, &uid_seq)) {
        /* Allow space for attribute sequence type and length */
        p_seq_start = p_rsp;
        if (p_ccb->cont_info.last_attr_seq_desc_sent == FALSE) {
            /* See if there is enough room to include a new service in the current response */
            rem_len = max_list_len - (INT16) (p_rsp - &p_ccb->rsp_list[0]);
            if (rem_len < 3) {
                /* Not enough room. Update continuation info for next response */
                p_ccb->cont_info.next_attr_index = 0;
                p_ccb->cont_info.next_attr_start_id = attr_seq.attr_entry[0].start;
                break;
            }
            p_rsp += 3;
        }

        /* Get a list of handles that match the UUIDs given to us */
        for (xx = p_ccb->cont_info.next_attr_index; xx < attr_seq.num_attr; xx++) {
            p_attr = sdp_db_find_attr_in_rec (p_rec, attr_seq.attr_entry[xx].start, attr_seq.attr_entry[xx].end);

            if (p_attr) {
                /* Check if attribute fits. Assume 3-byte value type/length */
                rem_len = max_list_len - (INT16) (p_rsp - &p_ccb->rsp_list[0]);

                /* just in case */
                if (rem_len <= 0) {
                    p_ccb->cont_info.next_attr_index = xx;
                    p_ccb->cont_info.next_attr_start_id = p_attr->id;
                    maxxed_out = TRUE;
                    break;
                }

                attr_len = sdpu_get_attrib_entry_len(p_attr);
                /* if there is a partial attribute pending to be sent */
                if (p_ccb->cont_info.attr_offset) {
                    p_rsp = sdpu_build_partial_attrib_entry (p_rsp, p_attr, rem_len,
                            &p_ccb->cont_info.attr_offset);

                    /* If the partial attrib could not been fully added yet */
                    if (p_ccb->cont_info.attr_offset != attr_len) {
                        maxxed_out = TRUE;
                        break;
                    } else { /* If the partial attrib has been added in full by now */
                        p_ccb->cont_info.attr_offset = 0;    /* reset attr_offset */
                    }
                } else if (rem_len < attr_len) { /* Not enough space for attr... so add partially */
                    if (attr_len >= SDP_MAX_ATTR_LEN) {
                        SDP_TRACE_ERROR("SDP attr too big: max_list_len=%d,attr_len=%d\n", max_list_len, attr_len);
                        sdpu_build_n_send_error (p_ccb, trans_num, SDP_NO_RESOURCES, NULL);
                        return;
                    }

                    /* add the partial attribute if possible */
                    p_rsp = sdpu_build_partial_attrib_entry (p_rsp, p_attr, (UINT16)rem_len,
                            &p_ccb->cont_info.attr_offset);

                    p_ccb->cont_info.next_attr_index = xx;
                    p_ccb->cont_info.next_attr_start_id = p_attr->id;
                    maxxed_out = TRUE;
                    break;
                } else { /* build the whole attribute */
                    p_rsp = sdpu_build_attrib_entry (p_rsp, p_attr);
                }

                /* If doing a range, stick with this one till no more attributes found */
                if (attr_seq.attr_entry[xx].start != attr_seq.attr_entry[xx].end) {
                    /* Update for next time through */
                    attr_seq.attr_entry[xx].start = p_attr->id + 1;

                    xx--;
                }
            }
        }

        /* Go back and put the type and length into the buffer */
        if (p_ccb->cont_info.last_attr_seq_desc_sent == FALSE) {
            seq_len = sdpu_get_attrib_seq_len(p_rec, &attr_seq_sav);
            if (seq_len != 0) {
                UINT8_TO_BE_STREAM  (p_seq_start, (DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_WORD);
                UINT16_TO_BE_STREAM (p_seq_start, seq_len);

                if (maxxed_out) {
                    p_ccb->cont_info.last_attr_seq_desc_sent = TRUE;
                }
            } else {
                p_rsp = p_seq_start;
            }
        }

        if (maxxed_out) {
            break;
        }

        /* Restore the attr_seq to look for in the next sdp record */
        memcpy(&attr_seq, &attr_seq_sav, sizeof(tSDP_ATTR_SEQ)) ;

        /* Reset the next attr index */
        p_ccb->cont_info.next_attr_index = 0;
        p_ccb->cont_info.prev_sdp_rec = p_rec;
        p_ccb->cont_info.last_attr_seq_desc_sent = FALSE;
    }

    /* response length */
    len_to_send = (UINT16) (p_rsp - &p_ccb->rsp_list[0]);
    cont_offset = 0;

    // The current SDP server design has a critical flaw where it can run into an infinite
    // request/response loop with the client. Here's the scenario:
    // - client makes SDP request
    // - server returns the first fragment of the response with a continuation token
    // - an SDP record is deleted from the server
    // - client issues another request with previous continuation token
    // - server has nothing to send back because the record is unavailable but in the
    //   first fragment, it had specified more response bytes than are now available
    // - server sends back no additional response bytes and returns the same continuation token
    // - client issues another request with the continuation token, and the process repeats
    //
    // We work around this design flaw here by checking if we will make forward progress
    // (i.e. we will send > 0 response bytes) on a continued request. If not, we must have
    // run into the above situation and we tell the peer an error occurred.
    //
    // TODO(sharvil): rewrite SDP server.
    if (is_cont && len_to_send == 0) {
        sdpu_build_n_send_error(p_ccb, trans_num, SDP_INVALID_CONT_STATE, NULL);
        return;
    }

    /* If first response, insert sequence header */
    if (!is_cont) {
        /* Get the total list length for requested uid and attribute sequence */
        p_ccb->list_len = sdpu_get_list_len(&uid_seq, &attr_seq_sav) + 3;
        /* Put in the sequence header (2 or 3 bytes) */
        if (p_ccb->list_len > 255) {
            p_ccb->rsp_list[0] = (UINT8) ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_WORD);
            p_ccb->rsp_list[1] = (UINT8) ((p_ccb->list_len - 3) >> 8);
            p_ccb->rsp_list[2] = (UINT8) (p_ccb->list_len - 3);
        } else {
            cont_offset = 1;

            p_ccb->rsp_list[1] = (UINT8) ((DATA_ELE_SEQ_DESC_TYPE << 3) | SIZE_IN_NEXT_BYTE);
            p_ccb->rsp_list[2] = (UINT8) (p_ccb->list_len - 3);

            p_ccb->list_len--;
            len_to_send--;
        }
    }

    /* Get a buffer to use to build the response */
    if ((p_buf = (BT_HDR *)osi_malloc(SDP_DATA_BUF_SIZE)) == NULL) {
        SDP_TRACE_ERROR ("SDP - no buf for search rsp\n");
        return;
    }
    p_buf->offset = L2CAP_MIN_OFFSET;
    p_rsp = p_rsp_start = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

    /* Start building a rsponse */
    UINT8_TO_BE_STREAM  (p_rsp, SDP_PDU_SERVICE_SEARCH_ATTR_RSP);
    UINT16_TO_BE_STREAM (p_rsp, trans_num);

    /* Skip the parameter length, add it when we know the length */
    p_rsp_param_len = p_rsp;
    p_rsp += 2;

    /* Stream the list length to send */
    UINT16_TO_BE_STREAM (p_rsp, len_to_send);

    /* copy from rsp_list to the actual buffer to be sent */
    memcpy (p_rsp, &p_ccb->rsp_list[cont_offset], len_to_send);
    p_rsp += len_to_send;

    p_ccb->cont_offset += len_to_send;

    /* If anything left to send, continuation needed */
    if (p_ccb->cont_offset < p_ccb->list_len) {
        is_cont = TRUE;

        UINT8_TO_BE_STREAM  (p_rsp, SDP_CONTINUATION_LEN);
        UINT16_TO_BE_STREAM (p_rsp, p_ccb->cont_offset);
    } else {
        UINT8_TO_BE_STREAM (p_rsp, 0);
    }

    /* Go back and put the parameter length into the buffer */
    rsp_param_len = p_rsp - p_rsp_param_len - 2;
    UINT16_TO_BE_STREAM (p_rsp_param_len, rsp_param_len);

    /* Set the length of the SDP data in the buffer */
    p_buf->len = p_rsp - p_rsp_start;


    /* Send the buffer through L2CAP */
    L2CA_DataWrite (p_ccb->connection_id, p_buf);
}

#endif  /* SDP_SERVER_ENABLED == TRUE */
