/******************************************************************************
 *
 *  Copyright (C) 2008-2012 Broadcom Corporation
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
 *  this file contains the GATT server functions
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include "osi/allocator.h"

#if BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE
#include <string.h>
#include "gatt_int.h"
#include "stack/l2c_api.h"
#include "l2c_int.h"
#define GATT_MTU_REQ_MIN_LEN        2



/*******************************************************************************
**
** Function         gatt_send_packet 
**
** Description      This function is called to send gatt packets directly

**
** Returns          status 
**
*******************************************************************************/
tGATT_STATUS gatt_send_packet (tGATT_TCB *p_tcb, UINT8 *p_data, UINT16 len)
{
    BT_HDR          *p_msg = NULL;
    UINT8           *p_m = NULL;
    UINT16          buf_len;
    tGATT_STATUS    status;

    if (len > p_tcb->payload_size){
        return  GATT_ILLEGAL_PARAMETER;
    }

    buf_len = (UINT16)(sizeof(BT_HDR) + p_tcb->payload_size + L2CAP_MIN_OFFSET);
    if ((p_msg = (BT_HDR *)osi_malloc(buf_len)) == NULL) {
        return GATT_NO_RESOURCES;
    }

    memset(p_msg, 0, buf_len);
    p_msg->len = len;
    p_m = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET;
    memcpy(p_m, p_data, len);

    status = attp_send_sr_msg(p_tcb, p_msg);
    return status;
} 

/*******************************************************************************
**
** Function         gatt_sr_enqueue_cmd
**
** Description      This function enqueue the request from client which needs a
**                  application response, and update the transaction ID.
**
** Returns          void
**
*******************************************************************************/
UINT32 gatt_sr_enqueue_cmd (tGATT_TCB *p_tcb, UINT8 op_code, UINT16 handle)
{
    tGATT_SR_CMD   *p_cmd = &p_tcb->sr_cmd;
    UINT32          trans_id = 0;

    if ( (p_cmd->op_code == 0) ||
            (op_code == GATT_HANDLE_VALUE_CONF)) { /* no pending request */
        if (op_code == GATT_CMD_WRITE ||
                op_code == GATT_SIGN_CMD_WRITE  ||
                op_code == GATT_REQ_MTU ||
                op_code == GATT_HANDLE_VALUE_CONF) {
            trans_id = ++p_tcb->trans_id;
        } else {
            p_cmd->trans_id   = ++p_tcb->trans_id;
            p_cmd->op_code    = op_code;
            p_cmd->handle     = handle;
            p_cmd->status     = GATT_NOT_FOUND;
            p_tcb->trans_id %= GATT_TRANS_ID_MAX;
            trans_id = p_cmd->trans_id;
        }
    }

    return trans_id;
}

/*******************************************************************************
**
** Function         gatt_sr_cmd_empty
**
** Description      This function check the server command queue is empty or not.
**
** Returns          TRUE if empty, FALSE if there is pending command.
**
*******************************************************************************/
BOOLEAN gatt_sr_cmd_empty (tGATT_TCB *p_tcb)
{
    return (p_tcb->sr_cmd.op_code == 0);
}

/*******************************************************************************
**
** Function         gatt_dequeue_sr_cmd
**
** Description      This function dequeue the request from command queue.
**
** Returns          void
**
*******************************************************************************/
void gatt_dequeue_sr_cmd (tGATT_TCB *p_tcb)
{
    /* Double check in case any buffers are queued */
    GATT_TRACE_DEBUG("gatt_dequeue_sr_cmd" );
    if (p_tcb->sr_cmd.p_rsp_msg) {
        GATT_TRACE_ERROR("%s free msg %p", __func__, p_tcb->sr_cmd.p_rsp_msg);

        osi_free(p_tcb->sr_cmd.p_rsp_msg);
        p_tcb->sr_cmd.p_rsp_msg = NULL;
    }

    if (p_tcb->sr_cmd.multi_rsp_q) {
        while (!fixed_queue_is_empty(p_tcb->sr_cmd.multi_rsp_q)) {
            osi_free(fixed_queue_dequeue(p_tcb->sr_cmd.multi_rsp_q, 0));
        }
        fixed_queue_free(p_tcb->sr_cmd.multi_rsp_q, NULL);
    }

    memset( &p_tcb->sr_cmd, 0, sizeof(tGATT_SR_CMD));
}

/*******************************************************************************
**
** Function         process_read_multi_rsp
**
** Description      This function check the read multiple response.
**
** Returns          BOOLEAN if all replies have been received
**
*******************************************************************************/
static BOOLEAN process_read_multi_rsp (tGATT_SR_CMD *p_cmd, tGATT_STATUS status,
                                       tGATTS_RSP *p_msg, UINT16 mtu)
{
    UINT16          ii, total_len, len;
    UINT8           *p;
    BOOLEAN         is_overflow = FALSE;

    GATT_TRACE_DEBUG ("process_read_multi_rsp status=%d mtu=%d", status, mtu);

	if (p_cmd->multi_rsp_q == NULL) {
        p_cmd->multi_rsp_q = fixed_queue_new(QUEUE_SIZE_MAX);
	}

    /* Enqueue the response */
    BT_HDR  *p_buf = (BT_HDR *)osi_malloc(sizeof(tGATTS_RSP));
    if (p_buf == NULL) {
        p_cmd->status = GATT_INSUF_RESOURCE;
        return FALSE;
    }
    memcpy((void *)p_buf, (const void *)p_msg, sizeof(tGATTS_RSP));

    fixed_queue_enqueue(p_cmd->multi_rsp_q, p_buf, FIXED_QUEUE_MAX_TIMEOUT);

    p_cmd->status = status;
    if (status == GATT_SUCCESS) {
        GATT_TRACE_DEBUG ("Multi read count=%d num_hdls=%d",
                         fixed_queue_length(p_cmd->multi_rsp_q),
                         p_cmd->multi_req.num_handles);
        /* Wait till we get all the responses */
        if (fixed_queue_length(p_cmd->multi_rsp_q) == p_cmd->multi_req.num_handles) {
            len = sizeof(BT_HDR) + L2CAP_MIN_OFFSET + mtu;
            if ((p_buf = (BT_HDR *)osi_calloc(len)) == NULL) {
                p_cmd->status = GATT_INSUF_RESOURCE;
                return (TRUE);
            }

            p_buf->offset = L2CAP_MIN_OFFSET;
            p = (UINT8 *)(p_buf + 1) + p_buf->offset;

            /* First byte in the response is the opcode */
            *p++ = GATT_RSP_READ_MULTI;
            p_buf->len = 1;

            /* Now walk through the buffers puting the data into the response in order */
            list_t *list = NULL;
            const list_node_t *node = NULL;
            if (! fixed_queue_is_empty(p_cmd->multi_rsp_q)) {
                list = fixed_queue_get_list(p_cmd->multi_rsp_q);
			}
            for (ii = 0; ii < p_cmd->multi_req.num_handles; ii++) {
                tGATTS_RSP *p_rsp = NULL;
                if (list != NULL) {
                    if (ii == 0) {
                        node = list_begin(list);
                    } else {
                        node = list_next(node);
					}
                    if (node != list_end(list)) {
                        p_rsp = (tGATTS_RSP *)list_node(node);
					}
                }

                if (p_rsp != NULL) {

                    total_len = (p_buf->len + p_rsp->attr_value.len);

                    if (total_len >  mtu) {
                        /* just send the partial response for the overflow case */
                        len = p_rsp->attr_value.len - (total_len - mtu);
                        is_overflow = TRUE;
                        GATT_TRACE_DEBUG ("multi read overflow available len=%d val_len=%d", len, p_rsp->attr_value.len );
                    } else {
                        len = p_rsp->attr_value.len;
                    }

                    if (p_rsp->attr_value.handle == p_cmd->multi_req.handles[ii]) {
                        memcpy (p, p_rsp->attr_value.value, len);
                        if (!is_overflow) {
                            p += len;
                        }
                        p_buf->len += len;
                    } else {
                        p_cmd->status        = GATT_NOT_FOUND;
                        break;
                    }

                    if (is_overflow) {
                        break;
                    }

                } else {
                    p_cmd->status        = GATT_NOT_FOUND;
                    break;
                }

            } /* loop through all handles*/


            /* Sanity check on the buffer length */
            if (p_buf->len == 0) {
                GATT_TRACE_ERROR("process_read_multi_rsp - nothing found!!");
                p_cmd->status = GATT_NOT_FOUND;
                osi_free (p_buf);
                GATT_TRACE_DEBUG(" osi_free (p_buf)");
            } else if (p_cmd->p_rsp_msg != NULL) {
                osi_free (p_buf);
            } else {
                p_cmd->p_rsp_msg = p_buf;
            }

            return (TRUE);
        }
    } else { /* any handle read exception occurs, return error */
        return (TRUE);
    }

    /* If here, still waiting */
    return (FALSE);
}

/*******************************************************************************
**
** Function         gatt_sr_process_app_rsp
**
** Description      This function checks whether the response message from application
**                  match any pending request or not.
**
** Returns          void
**
*******************************************************************************/
tGATT_STATUS gatt_sr_process_app_rsp (tGATT_TCB *p_tcb, tGATT_IF gatt_if,
                                      UINT32 trans_id, UINT8 op_code,
                                      tGATT_STATUS status, tGATTS_RSP *p_msg)
{
    tGATT_STATUS    ret_code = GATT_SUCCESS;
    UNUSED(trans_id);

    GATT_TRACE_DEBUG("gatt_sr_process_app_rsp gatt_if=%d\n", gatt_if);

    gatt_sr_update_cback_cnt(p_tcb, gatt_if, FALSE, FALSE);

    if (op_code == GATT_REQ_READ_MULTI) {
        /* If no error and still waiting, just return */
        if (!process_read_multi_rsp (&p_tcb->sr_cmd, status, p_msg, p_tcb->payload_size)) {
            return (GATT_SUCCESS);
        }
    } else {
        if (op_code == GATT_REQ_PREPARE_WRITE && status == GATT_SUCCESS) {
            gatt_sr_update_prep_cnt(p_tcb, gatt_if, TRUE, FALSE);
        }

        if (op_code == GATT_REQ_EXEC_WRITE && status != GATT_SUCCESS) {
            gatt_sr_reset_cback_cnt(p_tcb);
        }

        p_tcb->sr_cmd.status = status;

        if (gatt_sr_is_cback_cnt_zero(p_tcb)
                && status == GATT_SUCCESS) {
            if (p_tcb->sr_cmd.p_rsp_msg == NULL) {
                p_tcb->sr_cmd.p_rsp_msg = attp_build_sr_msg (p_tcb, (UINT8)(op_code + 1), (tGATT_SR_MSG *)p_msg);
            } else {
                GATT_TRACE_ERROR("Exception!!! already has respond message\n");
            }
        }
    }
    if (gatt_sr_is_cback_cnt_zero(p_tcb)) {
        if ( (p_tcb->sr_cmd.status == GATT_SUCCESS) && (p_tcb->sr_cmd.p_rsp_msg) ) {
            ret_code = attp_send_sr_msg (p_tcb, p_tcb->sr_cmd.p_rsp_msg);
            p_tcb->sr_cmd.p_rsp_msg = NULL;
        } else {
            if (p_tcb->sr_cmd.status == GATT_SUCCESS){
                status = GATT_UNKNOWN_ERROR;
            }
            ret_code = gatt_send_error_rsp (p_tcb, status, op_code, p_tcb->sr_cmd.handle, FALSE);
        }

        gatt_dequeue_sr_cmd(p_tcb);
    }

    GATT_TRACE_DEBUG("gatt_sr_process_app_rsp ret_code=%d\n", ret_code);

    return ret_code;
}

/*******************************************************************************
**
** Function         gatt_process_exec_write_req
**
** Description      This function is called to process the execute write request
**                  from client.
**
** Returns          void
**
*******************************************************************************/
void gatt_process_exec_write_req (tGATT_TCB *p_tcb, UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    UINT8   *p = p_data, flag, i = 0;
    UINT32  trans_id = 0;
    tGATT_IF gatt_if;
    UINT16  conn_id;
    UINT16  queue_num = 0;
    BOOLEAN is_first = TRUE;
    BOOLEAN is_prepare_write_valid = FALSE;
    BOOLEAN is_need_dequeue_sr_cmd = FALSE;
    tGATT_PREPARE_WRITE_RECORD *prepare_record = NULL;
    tGATT_PREPARE_WRITE_QUEUE_DATA * queue_data = NULL;
    UNUSED(len);

#if GATT_CONFORMANCE_TESTING == TRUE
    if (gatt_cb.enable_err_rsp && gatt_cb.req_op_code == op_code) {
        GATT_TRACE_DEBUG("Conformance tst: forced err rspv for Execute Write: error status=%d",
                         gatt_cb.err_status);

        gatt_send_error_rsp (p_tcb, gatt_cb.err_status, gatt_cb.req_op_code, gatt_cb.handle, FALSE);

        return;
    }
#endif

    STREAM_TO_UINT8(flag, p);

    /* mask the flag */
    flag &= GATT_PREP_WRITE_EXEC;

    prepare_record = &(p_tcb->prepare_write_record);
    queue_num = fixed_queue_length(prepare_record->queue);

    //if received prepare_write packets include stack_rsp and app_rsp, 
    //stack respond to execute_write only when stack_rsp handle has invalid_offset
    //or invalid_length error; 
    //app need to respond to execute_write if it has received app_rsp handle packets
    if (((prepare_record->error_code_app == GATT_SUCCESS) && 
        (prepare_record->total_num == queue_num))
        || (flag == GATT_PREP_WRITE_CANCEL)){
        tGATT_EXEC_WRITE_RSP gatt_exec_write_rsp;
        gatt_exec_write_rsp.op_code = GATT_RSP_EXEC_WRITE;
        gatt_send_packet(p_tcb, (UINT8 *)(&gatt_exec_write_rsp), sizeof(gatt_exec_write_rsp)); 
        gatt_dequeue_sr_cmd(p_tcb);
        if (flag != GATT_PREP_WRITE_CANCEL){
            is_prepare_write_valid = TRUE;
        }
        GATT_TRACE_DEBUG("Send execute_write_rsp\n");
    } else if ((prepare_record->error_code_app == GATT_SUCCESS) && 
        (prepare_record->total_num > queue_num)){
        //No error for stack_rsp's handles and there exist some app_rsp's handles, 
        //so exec_write_rsp depends to app's response; but stack_rsp's data is valid
        //TODO: there exist problem if stack_rsp's data is valid but app_rsp's data is not valid.
        is_prepare_write_valid = TRUE;
    } else if(prepare_record->total_num < queue_num) {
        GATT_TRACE_ERROR("Error in %s, line=%d, prepare write total number (%d) \
                        should not smaller than prepare queue number (%d)\n", \
                        __func__, __LINE__,prepare_record->total_num, queue_num); 
    } else if (prepare_record->error_code_app != GATT_SUCCESS){
        GATT_TRACE_DEBUG("Send error code for execute_write, code=0x%x\n", prepare_record->error_code_app);
        is_need_dequeue_sr_cmd = (prepare_record->total_num == queue_num) ? TRUE : FALSE;
        gatt_send_error_rsp(p_tcb, prepare_record->error_code_app, GATT_REQ_EXEC_WRITE, 0, is_need_dequeue_sr_cmd);
    }

    //dequeue prepare write data
    while(fixed_queue_try_peek_first(prepare_record->queue)) {
        queue_data = fixed_queue_dequeue(prepare_record->queue, FIXED_QUEUE_MAX_TIMEOUT);
        if (is_prepare_write_valid){
            if((queue_data->p_attr->p_value != NULL) && (queue_data->p_attr->p_value->attr_val.attr_val != NULL)){
                if(is_first) {
                    //clear attr_val.attr_len before handle prepare write data
                    queue_data->p_attr->p_value->attr_val.attr_len = 0;
                    is_first = FALSE;
                }
                memcpy(queue_data->p_attr->p_value->attr_val.attr_val+queue_data->offset, queue_data->value, queue_data->len);
                //don't forget to increase the attribute value length in the gatts database.
                queue_data->p_attr->p_value->attr_val.attr_len += queue_data->len;
            }
        }
        osi_free(queue_data);
    }
    fixed_queue_free(prepare_record->queue, NULL);
    prepare_record->queue = NULL;

    /* according to ble spec, even if there is no prep write queued, 
     * need to respond execute_write_response
     * Note: exec_write_rsp callback should be called after all data has been written*/
    if (!gatt_sr_is_prep_cnt_zero(p_tcb)) {
        if (prepare_record->total_num > queue_num){
            trans_id = gatt_sr_enqueue_cmd(p_tcb, op_code, 0);
            gatt_sr_copy_prep_cnt_to_cback_cnt(p_tcb);
        }

        for (i = 0; i < GATT_MAX_APPS; i++) {
            if (p_tcb->prep_cnt[i]) {
                gatt_if = (tGATT_IF) (i + 1);
                conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, gatt_if);
                gatt_sr_send_req_callback(conn_id,
                                          trans_id,
                                          GATTS_REQ_TYPE_WRITE_EXEC,
                                          (tGATTS_DATA *)&flag);
                p_tcb->prep_cnt[i] = 0;
            }
        }
    }

    prepare_record->total_num = 0;
    prepare_record->error_code_app = GATT_SUCCESS;
}

/*******************************************************************************
**
** Function         gatt_process_read_multi_req
**
** Description      This function is called to process the read multiple request
**                  from client.
**
** Returns          void
**
*******************************************************************************/
void gatt_process_read_multi_req (tGATT_TCB *p_tcb, UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    UINT32          trans_id;
    UINT16          handle = 0, ll = len;
    UINT8           *p = p_data, i_rcb;
    tGATT_STATUS    err = GATT_SUCCESS;
    UINT8           sec_flag, key_size;
    tGATTS_RSP       *p_msg;

    GATT_TRACE_DEBUG("gatt_process_read_multi_req" );
    p_tcb->sr_cmd.multi_req.num_handles = 0;

    gatt_sr_get_sec_info(p_tcb->peer_bda,
                         p_tcb->transport,
                         &sec_flag,
                         &key_size);

#if GATT_CONFORMANCE_TESTING == TRUE
    if (gatt_cb.enable_err_rsp && gatt_cb.req_op_code == op_code) {
        GATT_TRACE_DEBUG("Conformance tst: forced err rspvofr ReadMultiple: error status=%d\n", gatt_cb.err_status);

        STREAM_TO_UINT16(handle, p);

        gatt_send_error_rsp (p_tcb, gatt_cb.err_status, gatt_cb.req_op_code, handle, FALSE);

        return;
    }
#endif

    while (ll >= 2 && p_tcb->sr_cmd.multi_req.num_handles < GATT_MAX_READ_MULTI_HANDLES) {
        STREAM_TO_UINT16(handle, p);

        if ((i_rcb = gatt_sr_find_i_rcb_by_handle(handle)) < GATT_MAX_SR_PROFILES) {
            p_tcb->sr_cmd.multi_req.handles[p_tcb->sr_cmd.multi_req.num_handles++] = handle;

            /* check read permission */
            if ((err = gatts_read_attr_perm_check(   gatt_cb.sr_reg[i_rcb].p_db,
                       FALSE,
                       handle,
                       sec_flag,
                       key_size))
                    != GATT_SUCCESS) {
                GATT_TRACE_DEBUG("read permission denied : 0x%02x", err);
                break;
            }
        } else {
            /* invalid handle */
            err = GATT_INVALID_HANDLE;
            break;
        }
        ll -= 2;
    }

    if (ll != 0) {
        GATT_TRACE_ERROR("max attribute handle reached in ReadMultiple Request.");
    }

    if (p_tcb->sr_cmd.multi_req.num_handles == 0) {
        err = GATT_INVALID_HANDLE;
    }

    if (err == GATT_SUCCESS) {
        if ((trans_id = gatt_sr_enqueue_cmd (p_tcb, op_code, p_tcb->sr_cmd.multi_req.handles[0])) != 0) {
            gatt_sr_reset_cback_cnt(p_tcb); /* read multiple use multi_rsp_q's count*/

            for (ll = 0; ll < p_tcb->sr_cmd.multi_req.num_handles; ll ++) {
                if ((p_msg = (tGATTS_RSP *)osi_malloc(sizeof(tGATTS_RSP))) != NULL) {
                    memset(p_msg, 0, sizeof(tGATTS_RSP))
                    ;
                    handle = p_tcb->sr_cmd.multi_req.handles[ll];
                    i_rcb = gatt_sr_find_i_rcb_by_handle(handle);

                    p_msg->attr_value.handle = handle;
                    err = gatts_read_attr_value_by_handle(p_tcb,
                                                          gatt_cb.sr_reg[i_rcb].p_db,
                                                          op_code,
                                                          handle,
                                                          0,
                                                          p_msg->attr_value.value,
                                                          &p_msg->attr_value.len,
                                                          GATT_MAX_ATTR_LEN,
                                                          sec_flag,
                                                          key_size,
                                                          trans_id);

                    if (err == GATT_SUCCESS || err == GATT_STACK_RSP) {
                        gatt_sr_process_app_rsp(p_tcb, gatt_cb.sr_reg[i_rcb].gatt_if , trans_id, op_code, GATT_SUCCESS, p_msg);
                    }
                    /* either not using or done using the buffer, release it now */
                    osi_free(p_msg);
                } else {
                    err = GATT_NO_RESOURCES;
                    gatt_dequeue_sr_cmd(p_tcb);
                    break;
                }
            }
        } else {
            err = GATT_NO_RESOURCES;
        }
    }
    /* in theroy BUSY is not possible(should already been checked), protected check */
    if (err != GATT_SUCCESS && err != GATT_STACK_RSP && err != GATT_PENDING && err != GATT_BUSY) {
        gatt_send_error_rsp(p_tcb, err, op_code, handle, FALSE);
    }
}

/*******************************************************************************
**
** Function         gatt_build_primary_service_rsp
**
** Description      Primamry service request processed internally. Theretically
**                  only deal with ReadByTypeVAlue and ReadByGroupType.
**
** Returns          void
**
*******************************************************************************/
static tGATT_STATUS gatt_build_primary_service_rsp (BT_HDR *p_msg, tGATT_TCB *p_tcb,
        UINT8 op_code, UINT16 s_hdl,
        UINT16 e_hdl, UINT8 *p_data, tBT_UUID value)
{
    tGATT_STATUS    status = GATT_NOT_FOUND;
    UINT8           handle_len = 4, *p ;
    tGATT_SR_REG    *p_rcb;
    tGATT_SRV_LIST_INFO *p_list = &gatt_cb.srv_list_info;
    tGATT_SRV_LIST_ELEM  *p_srv = NULL;
    tBT_UUID       *p_uuid;

    UNUSED(p_data);

    p = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET;

    p_srv = p_list->p_first;

    while (p_srv) {
        p_rcb = GATT_GET_SR_REG_PTR(p_srv->i_sreg);

        if (p_rcb->in_use &&
                p_rcb->s_hdl >= s_hdl &&
                p_rcb->s_hdl <= e_hdl &&
                p_rcb->type == GATT_UUID_PRI_SERVICE) {
            if ((p_uuid = gatts_get_service_uuid (p_rcb->p_db)) != NULL) {
                if (op_code == GATT_REQ_READ_BY_GRP_TYPE) {
                    handle_len = 4 + p_uuid->len;
                }

                /* get the length byte in the repsonse */
                if (p_msg->offset == 0) {
                    *p ++ = op_code + 1;
                    p_msg->len ++;
                    p_msg->offset = handle_len;

                    if (op_code == GATT_REQ_READ_BY_GRP_TYPE) {
                        *p ++ =  (UINT8)p_msg->offset; /* length byte */
                        p_msg->len ++;
                    }
                }

                if (p_msg->len + p_msg->offset <= p_tcb->payload_size &&
                        handle_len == p_msg->offset) {
                    if (op_code != GATT_REQ_FIND_TYPE_VALUE ||
                            gatt_uuid_compare(value, *p_uuid)) {
                        UINT16_TO_STREAM(p, p_rcb->s_hdl);

                        if (p_list->p_last_primary == p_srv &&
                                p_list->p_last_primary == p_list->p_last) {
                            GATT_TRACE_DEBUG("Use 0xFFFF for the last primary attribute");
                            UINT16_TO_STREAM(p, 0xFFFF); /* see GATT ERRATA 4065, 4063, ATT ERRATA 4062 */
                        } else {
                            UINT16_TO_STREAM(p, p_rcb->e_hdl);
                        }

                        if (op_code == GATT_REQ_READ_BY_GRP_TYPE) {
                            gatt_build_uuid_to_stream(&p, *p_uuid);
                        }

                        status = GATT_SUCCESS;
                        p_msg->len += p_msg->offset;
                    }
                } else {
                    break;
                }
            }
        }
        p_srv = p_srv->p_next;
    }
    p_msg->offset = L2CAP_MIN_OFFSET;

    return status;
}

/*******************************************************************************
**
** Function         gatt_build_find_info_rsp
**
** Description      fill the find information response information in the given
**                  buffer.
**
** Returns          TRUE: if data filled sucessfully.
**                  FALSE: packet full, or format mismatch.
**
*******************************************************************************/
static tGATT_STATUS gatt_build_find_info_rsp(tGATT_SR_REG *p_rcb, BT_HDR *p_msg, UINT16 *p_len,
        UINT16 s_hdl, UINT16 e_hdl)
{
    tGATT_STATUS        status = GATT_NOT_FOUND;
    UINT8               *p;
    UINT16              len = *p_len;
    tGATT_ATTR16        *p_attr = NULL;
    UINT8               info_pair_len[2] = {4, 18};

    if (!p_rcb->p_db || !p_rcb->p_db->p_attr_list) {
        return status;
    }

    /* check the attribute database */
    p_attr = (tGATT_ATTR16 *) p_rcb->p_db->p_attr_list;

    p = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET + p_msg->len;

    while (p_attr) {
        if (p_attr->handle > e_hdl) {
            break;
        }

        if (p_attr->handle >= s_hdl) {
            if (p_msg->offset == 0) {
                p_msg->offset = (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_16) ? GATT_INFO_TYPE_PAIR_16 : GATT_INFO_TYPE_PAIR_128;
            }

            if (len >= info_pair_len[p_msg->offset - 1]) {
                if (p_msg->offset == GATT_INFO_TYPE_PAIR_16 && p_attr->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                    UINT16_TO_STREAM(p, p_attr->handle);
                    UINT16_TO_STREAM(p, p_attr->uuid);
                } else if (p_msg->offset == GATT_INFO_TYPE_PAIR_128 && p_attr->uuid_type == GATT_ATTR_UUID_TYPE_128  ) {
                    UINT16_TO_STREAM(p, p_attr->handle);
                    ARRAY_TO_STREAM (p, ((tGATT_ATTR128 *) p_attr)->uuid, LEN_UUID_128);
                } else if (p_msg->offset == GATT_INFO_TYPE_PAIR_128 && p_attr->uuid_type == GATT_ATTR_UUID_TYPE_32) {
                    UINT16_TO_STREAM(p, p_attr->handle);
                    gatt_convert_uuid32_to_uuid128(p, ((tGATT_ATTR32 *) p_attr)->uuid);
                    p += LEN_UUID_128;
                } else {
                    GATT_TRACE_ERROR("format mismatch");
                    status = GATT_NO_RESOURCES;
                    break;
                    /* format mismatch */
                }
                p_msg->len += info_pair_len[p_msg->offset - 1];
                len -= info_pair_len[p_msg->offset - 1];
                status = GATT_SUCCESS;

            } else {
                status = GATT_NO_RESOURCES;
                break;
            }
        }
        p_attr = (tGATT_ATTR16 *)p_attr->p_next;
    }

    *p_len = len;
    return status;
}

/*******************************************************************************
**
** Function         gatts_internal_read_by_type_req
**
** Description      check to see if the ReadByType request can be handled internally.
**
** Returns          void
**
*******************************************************************************/
static tGATT_STATUS gatts_validate_packet_format(UINT8 op_code, UINT16 *p_len,
        UINT8 **p_data, tBT_UUID *p_uuid_filter,
        UINT16 *p_s_hdl, UINT16 *p_e_hdl)
{
    tGATT_STATUS    reason = GATT_SUCCESS;
    UINT16          uuid_len, s_hdl = 0, e_hdl = 0;
    UINT16          len = *p_len;
    UINT8           *p = *p_data;

    if (len >= 4) {
        /* obtain starting handle, and ending handle */
        STREAM_TO_UINT16(s_hdl, p);
        STREAM_TO_UINT16(e_hdl, p);
        len -= 4;

        if (s_hdl > e_hdl || !GATT_HANDLE_IS_VALID(s_hdl) || !GATT_HANDLE_IS_VALID(e_hdl)) {
            reason = GATT_INVALID_HANDLE;
        }
        /* for these PDUs, uuid filter must present */
        else if (op_code == GATT_REQ_READ_BY_GRP_TYPE ||
                 op_code == GATT_REQ_FIND_TYPE_VALUE ||
                 op_code == GATT_REQ_READ_BY_TYPE) {
            if (len >= 2 && p_uuid_filter != NULL) {
                uuid_len = (op_code == GATT_REQ_FIND_TYPE_VALUE) ? 2 : len;

                /* parse uuid now */
                if (gatt_parse_uuid_from_cmd (p_uuid_filter, uuid_len, &p) == FALSE ||
                        p_uuid_filter->len == 0) {
                    GATT_TRACE_DEBUG("UUID filter does not exsit");
                    reason = GATT_INVALID_PDU;
                } else {
                    len -= p_uuid_filter->len;
                }
            } else {
                reason = GATT_INVALID_PDU;
            }
        }
    } else {
        reason = GATT_INVALID_PDU;
    }

    *p_data     = p;
    *p_len      = len;
    *p_s_hdl    = s_hdl;
    *p_e_hdl    = e_hdl;

    return reason;
}

/*******************************************************************************
**
** Function         gatts_process_primary_service_req
**
** Description      process ReadByGroupType/ReadByTypeValue request, for discover
**                  all primary services or discover primary service by UUID request.
**
** Returns          void
**
*******************************************************************************/
void gatts_process_primary_service_req(tGATT_TCB *p_tcb, UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    UINT8           reason = GATT_INVALID_PDU;
    UINT16          s_hdl = 0, e_hdl = 0;
    tBT_UUID        uuid, value, primary_service = {LEN_UUID_16, {GATT_UUID_PRI_SERVICE}};
    BT_HDR          *p_msg = NULL;
    UINT16          msg_len = (UINT16)(sizeof(BT_HDR) + p_tcb->payload_size + L2CAP_MIN_OFFSET);

    memset (&value, 0, sizeof(tBT_UUID));
    reason = gatts_validate_packet_format(op_code, &len, &p_data, &uuid, &s_hdl, &e_hdl);

    if (reason == GATT_SUCCESS) {
        if (gatt_uuid_compare(uuid, primary_service)) {
            if (op_code == GATT_REQ_FIND_TYPE_VALUE) {
                if (gatt_parse_uuid_from_cmd(&value, len, &p_data) == FALSE) {
                    reason = GATT_INVALID_PDU;
                }
            }

            if (reason == GATT_SUCCESS) {
                if ((p_msg =  (BT_HDR *)osi_calloc(msg_len)) == NULL) {
                    GATT_TRACE_ERROR("gatts_process_primary_service_req failed. no resources.");
                    reason = GATT_NO_RESOURCES;
                } else {
                    reason = gatt_build_primary_service_rsp (p_msg, p_tcb, op_code, s_hdl, e_hdl, p_data, value);
                }
            }
        } else {
            if (op_code == GATT_REQ_READ_BY_GRP_TYPE) {
                reason = GATT_UNSUPPORT_GRP_TYPE;
                GATT_TRACE_DEBUG("unexpected ReadByGrpType Group: 0x%04x", uuid.uu.uuid16);
            } else {
                /* we do not support ReadByTypeValue with any non-primamry_service type */
                reason = GATT_NOT_FOUND;
                GATT_TRACE_DEBUG("unexpected ReadByTypeValue type: 0x%04x", uuid.uu.uuid16);
            }
        }
    }

    if (reason != GATT_SUCCESS) {
        if (p_msg) {
            osi_free(p_msg);
        }
        gatt_send_error_rsp (p_tcb, reason, op_code, s_hdl, FALSE);
    } else {
        attp_send_sr_msg(p_tcb, p_msg);
    }

}

/*******************************************************************************
**
** Function         gatts_process_find_info
**
** Description      process find information request, for discover character
**                  descriptors.
**
** Returns          void
**
*******************************************************************************/
static void gatts_process_find_info(tGATT_TCB *p_tcb, UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    UINT8           reason = GATT_INVALID_PDU, *p;
    UINT16          s_hdl = 0, e_hdl = 0, buf_len;
    BT_HDR          *p_msg = NULL;
    tGATT_SR_REG    *p_rcb;
    tGATT_SRV_LIST_INFO *p_list = &gatt_cb.srv_list_info;
    tGATT_SRV_LIST_ELEM  *p_srv = NULL;

    reason = gatts_validate_packet_format(op_code, &len, &p_data, NULL, &s_hdl, &e_hdl);

    if (reason == GATT_SUCCESS) {
        buf_len = (UINT16)(sizeof(BT_HDR) + p_tcb->payload_size + L2CAP_MIN_OFFSET);

        if ((p_msg =  (BT_HDR *)osi_calloc(buf_len)) == NULL) {
            reason = GATT_NO_RESOURCES;
        } else {
            reason = GATT_NOT_FOUND;

            p = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET;
            *p ++ = op_code + 1;
            p_msg->len = 2;

            buf_len = p_tcb->payload_size - 2;

            p_srv = p_list->p_first;

            while (p_srv) {
                p_rcb = GATT_GET_SR_REG_PTR(p_srv->i_sreg);

                if (p_rcb->in_use &&
                        !(p_rcb->s_hdl > e_hdl ||
                          p_rcb->e_hdl < s_hdl)) {
                    reason = gatt_build_find_info_rsp(p_rcb, p_msg, &buf_len, s_hdl, e_hdl);
                    if (reason == GATT_NO_RESOURCES) {
                        reason = GATT_SUCCESS;
                        break;
                    }
                }
                p_srv = p_srv->p_next;
            }
            *p = (UINT8)p_msg->offset;

            p_msg->offset = L2CAP_MIN_OFFSET;
        }
    }

    if (reason != GATT_SUCCESS) {
        if (p_msg) {
            osi_free(p_msg);
        }
        gatt_send_error_rsp (p_tcb, reason, op_code, s_hdl, FALSE);
    } else {
        attp_send_sr_msg(p_tcb, p_msg);
    }

}

/*******************************************************************************
**
** Function         gatts_process_mtu_req
**
** Description      This function is called to process excahnge MTU request.
**                  Only used on LE.
**
** Returns          void
**
*******************************************************************************/
static void gatts_process_mtu_req (tGATT_TCB *p_tcb, UINT16 len, UINT8 *p_data)
{
    UINT16        mtu = 0;
    UINT8         *p = p_data, i;
    BT_HDR        *p_buf;
    UINT16   conn_id;

    /* BR/EDR conenction, send error response */
    if (p_tcb->att_lcid != L2CAP_ATT_CID) {
        gatt_send_error_rsp (p_tcb, GATT_REQ_NOT_SUPPORTED, GATT_REQ_MTU, 0, FALSE);
    } else if (len < GATT_MTU_REQ_MIN_LEN) {
        GATT_TRACE_ERROR("invalid MTU request PDU received.\n");
        gatt_send_error_rsp (p_tcb, GATT_INVALID_PDU, GATT_REQ_MTU, 0, FALSE);
    } else {
        STREAM_TO_UINT16 (mtu, p);
        /* mtu must be greater than default MTU which is 23/48 */
        if (mtu < GATT_DEF_BLE_MTU_SIZE) {
            p_tcb->payload_size = GATT_DEF_BLE_MTU_SIZE;
        } else if (mtu > gatt_default.local_mtu) {
            p_tcb->payload_size = gatt_default.local_mtu;
        } else {
            p_tcb->payload_size = mtu;
        }

        /* host will set packet data length to 251 automatically if remote device support set packet data length,
            so l2cble_set_fixed_channel_tx_data_length() is not necessary.
            l2cble_set_fixed_channel_tx_data_length(p_tcb->peer_bda, L2CAP_ATT_CID, p_tcb->payload_size);
        */

        if ((p_buf = attp_build_sr_msg(p_tcb, GATT_RSP_MTU, (tGATT_SR_MSG *) &p_tcb->payload_size)) != NULL) {
            attp_send_sr_msg (p_tcb, p_buf);

            /* Notify all registered applicaiton with new MTU size. Us a transaction ID */
            /* of 0, as no response is allowed from applcations                    */

            for (i = 0; i < GATT_MAX_APPS; i ++) {
                if (gatt_cb.cl_rcb[i].in_use ) {
                    conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, gatt_cb.cl_rcb[i].gatt_if);
                    gatt_sr_send_req_callback(conn_id, 0, GATTS_REQ_TYPE_MTU,
                                              (tGATTS_DATA *)&p_tcb->payload_size);
                }
            }

        }
    }
}

/*******************************************************************************
**
** Function         gatts_process_read_by_type_req
**
** Description      process Read By type request.
**                  This PDU can be used to perform:
**                  - read characteristic value
**                  - read characteristic descriptor value
**                  - discover characteristic
**                  - discover characteristic by UUID
**                  - relationship discovery
**
** Returns          void 
**
*******************************************************************************/
void gatts_process_read_by_type_req(tGATT_TCB *p_tcb, UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    tBT_UUID            uuid;
    tGATT_SR_REG        *p_rcb;
    UINT16              msg_len = (UINT16)(sizeof(BT_HDR) + p_tcb->payload_size + L2CAP_MIN_OFFSET),
                        buf_len,
                        s_hdl, e_hdl, err_hdl = 0;
    BT_HDR              *p_msg = NULL;
    tGATT_STATUS        reason, ret;
    UINT8               *p;
    UINT8               sec_flag, key_size;
    tGATT_SRV_LIST_INFO *p_list = &gatt_cb.srv_list_info;
    tGATT_SRV_LIST_ELEM  *p_srv = NULL;

    reason = gatts_validate_packet_format(op_code, &len, &p_data, &uuid, &s_hdl, &e_hdl);
    GATT_TRACE_DEBUG("%s, op_code =%x, len = %x\n", __func__, op_code, len);
#if GATT_CONFORMANCE_TESTING == TRUE
    if (gatt_cb.enable_err_rsp && gatt_cb.req_op_code == op_code) {
        GATT_TRACE_DEBUG("Conformance tst: forced err rsp for ReadByType: error status=%d\n", gatt_cb.err_status);

        gatt_send_error_rsp (p_tcb, gatt_cb.err_status, gatt_cb.req_op_code, s_hdl, FALSE);

        return;
    }
#endif

    if (reason == GATT_SUCCESS) {
        if ((p_msg =  (BT_HDR *)osi_calloc(msg_len)) == NULL) {
            GATT_TRACE_ERROR("gatts_process_find_info failed. no resources.\n");

            reason = GATT_NO_RESOURCES;
        } else {
            p = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET;

            *p ++ = op_code + 1;
            /* reserve length byte */
            p_msg->len = 2;
            buf_len = p_tcb->payload_size - 2;

            reason = GATT_NOT_FOUND;

            p_srv = p_list->p_first;

            while (p_srv) {
                p_rcb = GATT_GET_SR_REG_PTR(p_srv->i_sreg);

                if (p_rcb->in_use &&
                        !(p_rcb->s_hdl > e_hdl ||
                          p_rcb->e_hdl < s_hdl)) {
                    gatt_sr_get_sec_info(p_tcb->peer_bda,
                                         p_tcb->transport,
                                         &sec_flag,
                                         &key_size);

                    ret = gatts_db_read_attr_value_by_type(p_tcb,
                                                           p_rcb->p_db,
                                                           op_code,
                                                           p_msg,
                                                           s_hdl,
                                                           e_hdl,
                                                           uuid,
                                                           &buf_len,
                                                           sec_flag,
                                                           key_size,
                                                           0,
                                                           &err_hdl);
                    if (ret != GATT_NOT_FOUND) {
                        reason = ret;

                        if (ret == GATT_NO_RESOURCES) {
                            reason = GATT_SUCCESS;
                        }
                    }
                    if (ret != GATT_SUCCESS && ret != GATT_NOT_FOUND) {
                        s_hdl = err_hdl;
                        break;
                    }
                }
                p_srv = p_srv->p_next;
            }
            *p              = (UINT8)p_msg->offset;
            p_msg->offset   = L2CAP_MIN_OFFSET;
        }
    }
    if (reason != GATT_SUCCESS && reason != GATT_STACK_RSP) {
        if (p_msg) {
            osi_free(p_msg);
        }

        /* in theroy BUSY is not possible(should already been checked), protected check */
        if (reason != GATT_PENDING && reason != GATT_BUSY) {
            gatt_send_error_rsp (p_tcb, reason, op_code, s_hdl, FALSE);
        }
    } else {
        attp_send_sr_msg(p_tcb, p_msg);
    }

}

/*******************************************************************************
**
** Function         gatts_process_write_req
**
** Description      This function is called to process the write request
**                  from client.
**
** Returns          void
**
*******************************************************************************/
void gatts_process_write_req (tGATT_TCB *p_tcb, UINT8 i_rcb, UINT16 handle,
                              UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    tGATTS_DATA     sr_data;
    UINT32          trans_id;
    tGATT_STATUS    status;
    UINT8           sec_flag, key_size, *p = p_data;
    tGATT_SR_REG    *p_sreg;
    UINT16          conn_id, offset = 0;

    memset(&sr_data, 0, sizeof(tGATTS_DATA));
    sr_data.write_req.need_rsp = FALSE;

    switch (op_code) {
    case GATT_SIGN_CMD_WRITE:
        if (op_code == GATT_SIGN_CMD_WRITE) {
            GATT_TRACE_DEBUG("Write CMD with data signing" );
            len -= GATT_AUTH_SIGN_LEN;
        }
    /* fall through */
    case GATT_CMD_WRITE:
    case GATT_REQ_WRITE:
        sr_data.write_req.handle = handle;
        sr_data.write_req.len = len;
        if (len != 0 && p != NULL) {
            memcpy (sr_data.write_req.value, p, len);
        }
        break;
    }

    gatt_sr_get_sec_info(p_tcb->peer_bda,
                         p_tcb->transport,
                         &sec_flag,
                         &key_size);

    status = gatts_write_attr_perm_check (gatt_cb.sr_reg[i_rcb].p_db,
                                          op_code,
                                          handle,
                                          sr_data.write_req.offset,
                                          p,
                                          len,
                                          sec_flag,
                                          key_size);

    if (status == GATT_SUCCESS) {
        if ((trans_id = gatt_sr_enqueue_cmd(p_tcb, op_code, handle)) != 0) {
            p_sreg = &gatt_cb.sr_reg[i_rcb];
            conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, p_sreg->gatt_if);
            status = gatts_write_attr_value_by_handle(gatt_cb.sr_reg[i_rcb].p_db,
                    handle, offset, p, len);
            if((op_code == GATT_REQ_WRITE) && (status == GATT_APP_RSP)){
                sr_data.write_req.need_rsp = TRUE;
                status = GATT_PENDING;
            }

            gatt_sr_send_req_callback(conn_id,
                    trans_id,
                    GATTS_REQ_TYPE_WRITE,
                    &sr_data);
        } else {
            GATT_TRACE_ERROR("Error in %s, line=%d, max pending command, send error\n", __func__, __LINE__);
            status = GATT_BUSY; /* max pending command, application error */
        }
    }

    /* response should be sent only for write_request */
    if ((op_code == GATT_REQ_WRITE) && (sr_data.write_req.need_rsp == FALSE)){
        if (status == GATT_SUCCESS){
            tGATT_WRITE_REQ_RSP gatt_write_req_rsp;
            gatt_write_req_rsp.op_code = GATT_RSP_WRITE;                      
            gatt_send_packet(p_tcb, (UINT8 *)(&gatt_write_req_rsp), sizeof(gatt_write_req_rsp)); 
            gatt_dequeue_sr_cmd(p_tcb);
        } else if (status != GATT_PENDING){
            /* note: in case of GATT_BUSY, will respond this application error to remote device */
            gatt_send_error_rsp (p_tcb, status, op_code, handle, TRUE);
        }
    }

    return;
}


/*******************************************************************************
 **
 ** Function         gatts_attr_process_preapre_write
 **
 ** Description      This function is called to process the prepare write request
 **                  from client.
 **
 ** Returns          void
 **
 *******************************************************************************/
void gatt_attr_process_prepare_write (tGATT_TCB *p_tcb, UINT8 i_rcb, UINT16 handle,
                                     UINT8 op_code, UINT16 len, UINT8 *p_data)
{
    tGATT_STATUS status;
    tGATT_PREPARE_WRITE_QUEUE_DATA * queue_data = NULL;
    tGATT_ATTR16  *p_attr;
    tGATT_ATTR16  *p_attr_temp;
    tGATTS_DATA     sr_data;
    UINT32          trans_id = 0; 
    UINT8           sec_flag, key_size, *p = p_data;
    tGATT_SR_REG    *p_sreg;
    UINT16          conn_id, offset = 0;
    tGATT_SVC_DB    *p_db; 
    BOOLEAN is_need_prepare_write_rsp = FALSE;
    BOOLEAN is_need_queue_data = FALSE;
    tGATT_PREPARE_WRITE_RECORD *prepare_record = NULL;
    memset(&sr_data, 0, sizeof(tGATTS_DATA));

    if (len < 2) {
        GATT_TRACE_ERROR("%s: Prepare write request was invalid - missing offset, sending error response", __func__);
        gatt_send_error_rsp(p_tcb, GATT_INVALID_PDU, op_code, handle, FALSE);
        return;
    }
    //get offset from p_data
    STREAM_TO_UINT16(offset, p);
    len -= 2;
    p_sreg = &gatt_cb.sr_reg[i_rcb];
    conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, p_sreg->gatt_if);
    //prepare_record = &(prepare_write_record);
    prepare_record = &(p_tcb->prepare_write_record);

    gatt_sr_get_sec_info(p_tcb->peer_bda,
                         p_tcb->transport,
                         &sec_flag,
                         &key_size);

    status = gatts_write_attr_perm_check (gatt_cb.sr_reg[i_rcb].p_db,
                                          op_code,
                                          handle,
                                          sr_data.write_req.offset,
                                          p,
                                          len,
                                          sec_flag,
                                          key_size);

    if (status == GATT_SUCCESS){
        if ((trans_id = gatt_sr_enqueue_cmd(p_tcb, op_code, handle)) != 0) {
            p_db = gatt_cb.sr_reg[i_rcb].p_db;
            if (p_db && p_db->p_attr_list) {
                p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;
                while (p_attr && handle >= p_attr->handle) {
                    if (p_attr->handle == handle ) {
                        p_attr_temp = p_attr;
                        if (p_attr->control.auto_rsp == GATT_RSP_BY_APP) {
                            status = GATT_APP_RSP;
                        } else if (p_attr->p_value != NULL &&
                            offset > p_attr->p_value->attr_val.attr_max_len) {
                            status = GATT_INVALID_OFFSET; 
                             is_need_prepare_write_rsp = TRUE;
                             is_need_queue_data = TRUE;
                        } else if (p_attr->p_value != NULL &&
                            ((offset + len) > p_attr->p_value->attr_val.attr_max_len)){
                            status = GATT_INVALID_ATTR_LEN;
                            is_need_prepare_write_rsp = TRUE;
                            is_need_queue_data = TRUE;
                        } else if (p_attr->p_value == NULL) {
                            GATT_TRACE_ERROR("Error in %s, attribute of handle 0x%x not allocate value buffer\n",
                                        __func__, handle);
                            status = GATT_UNKNOWN_ERROR;
                        } else {
                             //valid prepare write request, need to send response and queue the data
                             //status: GATT_SUCCESS
                             is_need_prepare_write_rsp = TRUE;
                             is_need_queue_data = TRUE;
                         }
                    }
                    p_attr = (tGATT_ATTR16 *)p_attr->p_next;
                }
            }
        } else{
            status = GATT_UNKNOWN_ERROR;
            GATT_TRACE_ERROR("Error in %s, Line %d: GATT BUSY\n", __func__, __LINE__);
        }
    }

    if (is_need_queue_data){
        queue_data = (tGATT_PREPARE_WRITE_QUEUE_DATA *)osi_malloc(len + sizeof(tGATT_PREPARE_WRITE_QUEUE_DATA));
        if (queue_data == NULL){
            status = GATT_PREPARE_Q_FULL;
        } else {
            queue_data->p_attr = p_attr_temp;
            queue_data->len = len;
            queue_data->handle = handle;
            queue_data->offset = offset;
            memcpy(queue_data->value, p, len);
            if (prepare_record->queue == NULL) {
                prepare_record->queue = fixed_queue_new(QUEUE_SIZE_MAX);
            }
            fixed_queue_enqueue(prepare_record->queue, queue_data, FIXED_QUEUE_MAX_TIMEOUT);
        }
    }
    
    if (is_need_prepare_write_rsp){
        //send prepare write response
        if (queue_data != NULL){
            queue_data->op_code = op_code + 1;
            //5: op_code 1 + handle 2 + offset 2
            tGATT_STATUS rsp_send_status = gatt_send_packet(p_tcb, &(queue_data->op_code), queue_data->len + 5);
            gatt_sr_update_prep_cnt(p_tcb, p_sreg->gatt_if, TRUE, FALSE);
            gatt_dequeue_sr_cmd(p_tcb);
            
            if (rsp_send_status != GATT_SUCCESS){
                GATT_TRACE_ERROR("Error in %s, line=%d, fail to send prepare_write_rsp, status=0x%x\n",
                            __func__, __LINE__, rsp_send_status);
            }
        } else{
            GATT_TRACE_ERROR("Error in %s, line=%d, queue_data should not be NULL here, fail to send prepare_write_rsp\n",
                        __func__, __LINE__);
        }
    }

    if ((status == GATT_APP_RSP) || (is_need_prepare_write_rsp)){ 
        prepare_record->total_num++;
        memset(&sr_data, 0, sizeof(sr_data));
        sr_data.write_req.is_prep = TRUE;
        sr_data.write_req.handle = handle;
        sr_data.write_req.offset = offset;
        sr_data.write_req.len = len;
        sr_data.write_req.need_rsp = (status == GATT_APP_RSP) ? TRUE : FALSE;
        memcpy(sr_data.write_req.value, p, len);
        gatt_sr_send_req_callback(conn_id, trans_id, GATTS_REQ_TYPE_WRITE, &sr_data);
    } else{
        gatt_send_error_rsp(p_tcb, status, GATT_REQ_PREPARE_WRITE, handle, TRUE);
    }

    if ((prepare_record->error_code_app == GATT_SUCCESS) 
            && ((status == GATT_INVALID_OFFSET) || (status == GATT_INVALID_ATTR_LEN))){
            prepare_record->error_code_app = status;
    }

}

/*******************************************************************************
 **
 ** Function         gatts_process_read_req
 **
 ** Description      This function is called to process the read request
 **                  from client.
 **
 ** Returns          void
 **
 *******************************************************************************/
static void gatts_process_read_req(tGATT_TCB *p_tcb, tGATT_SR_REG *p_rcb, UINT8 op_code,
                                   UINT16 handle, UINT16 len, UINT8 *p_data)
{
    UINT16       buf_len = (UINT16)(sizeof(BT_HDR) + p_tcb->payload_size + L2CAP_MIN_OFFSET);
    tGATT_STATUS reason;
    BT_HDR       *p_msg = NULL;
    UINT8        sec_flag, key_size, *p;
    UINT16       offset = 0, value_len = 0;

    UNUSED (len);
    if ((p_msg =  (BT_HDR *)osi_calloc(buf_len)) == NULL) {
        GATT_TRACE_ERROR("gatts_process_find_info failed. no resources.\n");

        reason = GATT_NO_RESOURCES;
    } else {
        if (op_code == GATT_REQ_READ_BLOB) {
            STREAM_TO_UINT16(offset, p_data);
        }

        p = (UINT8 *)(p_msg + 1) + L2CAP_MIN_OFFSET;
        *p ++ = op_code + 1;
        p_msg->len = 1;
        buf_len = p_tcb->payload_size - 1;

        gatt_sr_get_sec_info(p_tcb->peer_bda,
                             p_tcb->transport,
                             &sec_flag,
                             &key_size);

        reason = gatts_read_attr_value_by_handle(p_tcb,
                 p_rcb->p_db,
                 op_code,
                 handle,
                 offset,
                 p,
                 &value_len,
                 buf_len,
                 sec_flag,
                 key_size,
                 0);

        p_msg->len += value_len;
    }


    if (reason != GATT_SUCCESS && reason != GATT_PENDING && reason != GATT_STACK_RSP) {
        if (p_msg) {
            osi_free(p_msg);
        }

        /* in theroy BUSY is not possible(should already been checked), protected check */
        if (reason != GATT_BUSY) {
            gatt_send_error_rsp (p_tcb, reason, op_code, handle, FALSE);
            gatt_dequeue_sr_cmd(p_tcb);
        }
    } else if (reason == GATT_SUCCESS || reason == GATT_STACK_RSP) {
        attp_send_sr_msg(p_tcb, p_msg);
        gatt_dequeue_sr_cmd(p_tcb);
    } else {
        if (p_msg) {
            osi_free(p_msg);
        }
    }

}

/*******************************************************************************
**
** Function         gatts_process_attribute_req
**
** Description      This function is called to process the per attribute handle request
**                  from client.
**
** Returns          void
**
*******************************************************************************/
void gatts_process_attribute_req (tGATT_TCB *p_tcb, UINT8 op_code,
                                  UINT16 len, UINT8 *p_data)
{
    UINT16          handle = 0;
    UINT8           *p = p_data, i;
    tGATT_SR_REG    *p_rcb = gatt_cb.sr_reg;
    tGATT_STATUS    status = GATT_INVALID_HANDLE;
    tGATT_ATTR16    *p_attr;

    if (len < 2) {
        GATT_TRACE_ERROR("Illegal PDU length, discard request\n");
        status = GATT_INVALID_PDU;
    } else {
        STREAM_TO_UINT16(handle, p);
        len -= 2;
    }

#if GATT_CONFORMANCE_TESTING == TRUE
    gatt_cb.handle = handle;
    if (gatt_cb.enable_err_rsp && gatt_cb.req_op_code == op_code) {
        GATT_TRACE_DEBUG("Conformance tst: forced err rsp: error status=%d\n", gatt_cb.err_status);

        gatt_send_error_rsp (p_tcb, gatt_cb.err_status, gatt_cb.req_op_code, handle, FALSE);

        return;
    }
#endif

    if (GATT_HANDLE_IS_VALID(handle)) {
        for (i = 0; i < GATT_MAX_SR_PROFILES; i ++, p_rcb ++) {
            if (p_rcb->in_use && p_rcb->s_hdl <= handle && p_rcb->e_hdl >= handle) {
                p_attr = (tGATT_ATTR16 *)p_rcb->p_db->p_attr_list;

                while (p_attr) {
                    if (p_attr->handle == handle) {
                        switch (op_code) {
                        case GATT_REQ_READ: /* read char/char descriptor value */
                        case GATT_REQ_READ_BLOB:
                            gatts_process_read_req(p_tcb, p_rcb, op_code, handle, len, p);
                            break;

                        case GATT_REQ_WRITE: /* write char/char descriptor value */
                        case GATT_CMD_WRITE:
                        case GATT_SIGN_CMD_WRITE:
                            gatts_process_write_req(p_tcb, i, handle, op_code, len, p);
                            break;
                        
                        case GATT_REQ_PREPARE_WRITE:
                            gatt_attr_process_prepare_write (p_tcb, i, handle, op_code, len, p);
                        default:
                            break;
                        }
                        status = GATT_SUCCESS;
                        break;
                    }
                    p_attr = (tGATT_ATTR16 *)p_attr->p_next;
                }
                break;
            }
        }
    }

    if (status != GATT_SUCCESS && op_code != GATT_CMD_WRITE && op_code != GATT_SIGN_CMD_WRITE) {
        gatt_send_error_rsp (p_tcb, status, op_code, handle, FALSE);
    }
}

/*******************************************************************************
**
** Function         gatts_proc_srv_chg_ind_ack
**
** Description      This function process the service changed indicaiton ACK
**
** Returns          void
**
*******************************************************************************/
static void gatts_proc_srv_chg_ind_ack(tGATT_TCB *p_tcb )
{
    tGATTS_SRV_CHG_REQ  req;
    tGATTS_SRV_CHG      *p_buf = NULL;

    GATT_TRACE_DEBUG("gatts_proc_srv_chg_ind_ack");

    if ((p_buf = gatt_is_bda_in_the_srv_chg_clt_list(p_tcb->peer_bda)) != NULL) {
        GATT_TRACE_DEBUG("NV update set srv chg = FALSE");
        p_buf->srv_changed = FALSE;
        memcpy(&req.srv_chg, p_buf, sizeof(tGATTS_SRV_CHG));
        if (gatt_cb.cb_info.p_srv_chg_callback) {
            (*gatt_cb.cb_info.p_srv_chg_callback)(GATTS_SRV_CHG_CMD_UPDATE_CLIENT, &req, NULL);
        }
    }
}

/*******************************************************************************
**
** Function         gatts_chk_pending_ind
**
** Description      This function check any pending indication needs to be sent if
**                  there is a pending indication then sent the indication
**
** Returns          void
**
*******************************************************************************/
static void gatts_chk_pending_ind(tGATT_TCB *p_tcb )
{
#if (GATTS_INCLUDED == TRUE)
    tGATT_VALUE *p_buf = (tGATT_VALUE *)fixed_queue_try_peek_first(p_tcb->pending_ind_q);
    GATT_TRACE_DEBUG("gatts_chk_pending_ind");

    if (p_buf ) {
        GATTS_HandleValueIndication (p_buf->conn_id,
                                     p_buf->handle,
                                     p_buf->len,
                                     p_buf->value);
        osi_free(fixed_queue_try_remove_from_queue(p_tcb->pending_ind_q,
                                                      p_buf));
    }
#endif  ///GATTS_INCLUDED == TRUE
}

/*******************************************************************************
**
** Function         gatts_proc_ind_ack
**
** Description      This function process the Indication ack
**
** Returns          TRUE continue to process the indication ack by the aaplication
**                  if the ACk is not a Service Changed Indication Ack
**
*******************************************************************************/
static BOOLEAN gatts_proc_ind_ack(tGATT_TCB *p_tcb, UINT16 ack_handle)
{
    BOOLEAN continue_processing = TRUE;

    GATT_TRACE_DEBUG ("gatts_proc_ind_ack ack handle=%d", ack_handle);

    if (ack_handle == gatt_cb.handle_of_h_r) {
        gatts_proc_srv_chg_ind_ack(p_tcb);
        /* there is no need to inform the application since srv chg is handled internally by GATT */
        continue_processing = FALSE;
    }

    gatts_chk_pending_ind(p_tcb);
    return continue_processing;
}

/*******************************************************************************
**
** Function         gatts_process_value_conf
**
** Description      This function is called to process the handle value confirmation.
**
** Returns          void
**
*******************************************************************************/
void gatts_process_value_conf(tGATT_TCB *p_tcb, UINT8 op_code)
{
    UINT16          handle = p_tcb->indicate_handle;
    UINT32          trans_id;
    UINT8           i;
    tGATT_SR_REG    *p_rcb = gatt_cb.sr_reg;
    BOOLEAN         continue_processing;
    UINT16          conn_id;

    btu_stop_timer (&p_tcb->conf_timer_ent);
    if (GATT_HANDLE_IS_VALID(handle)) {
        p_tcb->indicate_handle = 0;
        continue_processing = gatts_proc_ind_ack(p_tcb, handle);

        if (continue_processing) {
            for (i = 0; i < GATT_MAX_SR_PROFILES; i ++, p_rcb ++) {
                if (p_rcb->in_use && p_rcb->s_hdl <= handle && p_rcb->e_hdl >= handle) {
                    trans_id = gatt_sr_enqueue_cmd(p_tcb, op_code, handle);
                    conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, p_rcb->gatt_if);
                    tGATTS_DATA p_data = {0};
                    p_data.handle = handle;
                    gatt_sr_send_req_callback(conn_id,
                                              trans_id, GATTS_REQ_TYPE_CONF, &p_data);
                }
            }
        }
    } else {
        GATT_TRACE_ERROR("unexpected handle value confirmation");
    }
}

/*******************************************************************************
**
** Function         gatt_server_handle_client_req
**
** Description      This function is called to handle the client requests to
**                  server.
**
**
** Returns          void
**
*******************************************************************************/
void gatt_server_handle_client_req (tGATT_TCB *p_tcb, UINT8 op_code,
                                    UINT16 len, UINT8 *p_data)
{
    /* there is pending command, discard this one */
    if (!gatt_sr_cmd_empty(p_tcb) && op_code != GATT_HANDLE_VALUE_CONF) {
        return;
    }

    /* the size of the message may not be bigger than the local max PDU size*/
    /* The message has to be smaller than the agreed MTU, len does not include op code */
    if (len >= p_tcb->payload_size) {
        GATT_TRACE_ERROR("server receive invalid PDU size:%d pdu size:%d", len + 1, p_tcb->payload_size );
        /* for invalid request expecting response, send it now */
        if (op_code != GATT_CMD_WRITE &&
                op_code != GATT_SIGN_CMD_WRITE &&
                op_code != GATT_HANDLE_VALUE_CONF) {
            gatt_send_error_rsp (p_tcb, GATT_INVALID_PDU, op_code, 0, FALSE);
        }
        /* otherwise, ignore the pkt */
    } else {
        switch (op_code) {
        case GATT_REQ_READ_BY_GRP_TYPE:         /* discover primary services */
        case GATT_REQ_FIND_TYPE_VALUE:          /* discover service by UUID */
            gatts_process_primary_service_req (p_tcb, op_code, len, p_data);
            break;

        case GATT_REQ_FIND_INFO:                /* discover char descrptor */
            gatts_process_find_info(p_tcb, op_code, len, p_data);
            break;

        case GATT_REQ_READ_BY_TYPE:             /* read characteristic value, char descriptor value */
            /* discover characteristic, discover char by UUID */
            gatts_process_read_by_type_req(p_tcb, op_code, len, p_data);
            break;


        case GATT_REQ_READ:                     /* read char/char descriptor value */
        case GATT_REQ_READ_BLOB:
        case GATT_REQ_WRITE:                    /* write char/char descriptor value */
        case GATT_CMD_WRITE:
        case GATT_SIGN_CMD_WRITE:
        case GATT_REQ_PREPARE_WRITE:
            gatts_process_attribute_req (p_tcb, op_code, len, p_data);
            break;

        case GATT_HANDLE_VALUE_CONF:
            gatts_process_value_conf (p_tcb, op_code);
            break;

        case GATT_REQ_MTU:
            gatts_process_mtu_req (p_tcb, len, p_data);
            break;

        case GATT_REQ_EXEC_WRITE:
            gatt_process_exec_write_req (p_tcb, op_code, len, p_data);
            break;

        case GATT_REQ_READ_MULTI:
            gatt_process_read_multi_req (p_tcb, op_code, len, p_data);
            break;

        default:
            break;
        }
    }
}

#endif /* BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
