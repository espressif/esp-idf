/******************************************************************************
 *
 *  Copyright (C) 2008-2014 Broadcom Corporation
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
 *  this file contains ATT protocol functions
 *
 ******************************************************************************/

#include "bt_target.h"

#if BLE_INCLUDED == TRUE

#include "gatt_int.h"
#include "l2c_api.h"

#define GATT_HDR_FIND_TYPE_VALUE_LEN    21
#define GATT_OP_CODE_SIZE   1
/**********************************************************************
**   ATT protocl message building utility                              *
***********************************************************************/
/*******************************************************************************
**
** Function         attp_build_mtu_exec_cmd
**
** Description      Build a exchange MTU request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_mtu_cmd(UINT8 op_code, UINT16 rx_mtu)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR) + GATT_HDR_SIZE + L2CAP_MIN_OFFSET)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, op_code);
        UINT16_TO_STREAM (p, rx_mtu);

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = GATT_HDR_SIZE; /* opcode + 2 bytes mtu */
    }
    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_exec_write_cmd
**
** Description      Build a execute write request or response.
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_exec_write_cmd (UINT8 op_code, UINT8 flag)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getpoolbuf(GATT_BUF_POOL_ID)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = GATT_OP_CODE_SIZE;

        UINT8_TO_STREAM (p, op_code);

        if (op_code == GATT_REQ_EXEC_WRITE)
        {
            flag &= GATT_PREP_WRITE_EXEC;
            UINT8_TO_STREAM (p, flag);
            p_buf->len += 1;
        }

    }

    return p_buf;
}

/*******************************************************************************
**
** Function         attp_build_err_cmd
**
** Description      Build a exchange MTU request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_err_cmd(UINT8 cmd_code, UINT16 err_handle, UINT8 reason)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR) + L2CAP_MIN_OFFSET + 5)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, GATT_RSP_ERROR);
        UINT8_TO_STREAM (p, cmd_code);
        UINT16_TO_STREAM(p, err_handle);
        UINT8_TO_STREAM (p, reason);

        p_buf->offset = L2CAP_MIN_OFFSET;
        /* GATT_HDR_SIZE (1B ERR_RSP op code+ 2B handle) + 1B cmd_op_code  + 1B status */
        p_buf->len = GATT_HDR_SIZE + 1 + 1;
    }
    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_browse_cmd
**
** Description      Build a read information request or read by type request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_browse_cmd(UINT8 op_code, UINT16 s_hdl, UINT16 e_hdl, tBT_UUID uuid)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR) + 8 + L2CAP_MIN_OFFSET)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;
        /* Describe the built message location and size */
        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = GATT_OP_CODE_SIZE + 4;

        UINT8_TO_STREAM (p, op_code);
        UINT16_TO_STREAM (p, s_hdl);
        UINT16_TO_STREAM (p, e_hdl);
        p_buf->len += gatt_build_uuid_to_stream(&p, uuid);
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_read_handles_cmd
**
** Description      Build a read by type and value request.
**
** Returns          pointer to the command buffer.
**
*******************************************************************************/
BT_HDR *attp_build_read_by_type_value_cmd (UINT16 payload_size, tGATT_FIND_TYPE_VALUE *p_value_type)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;
    UINT16      len = p_value_type->value_len;

    if ((p_buf = (BT_HDR *)GKI_getbuf((UINT16)(sizeof(BT_HDR) + payload_size + L2CAP_MIN_OFFSET))) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = 5; /* opcode + s_handle + e_handle */

        UINT8_TO_STREAM  (p, GATT_REQ_FIND_TYPE_VALUE);
        UINT16_TO_STREAM (p, p_value_type->s_handle);
        UINT16_TO_STREAM (p, p_value_type->e_handle);

        p_buf->len += gatt_build_uuid_to_stream(&p, p_value_type->uuid);

        if (p_value_type->value_len +  p_buf->len > payload_size )
            len = payload_size - p_buf->len;

        memcpy (p, p_value_type->value, len);
        p_buf->len += len;
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_read_multi_cmd
**
** Description      Build a read multiple request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_read_multi_cmd(UINT16 payload_size, UINT16 num_handle, UINT16 *p_handle)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p, i = 0;

    if ((p_buf = (BT_HDR *)GKI_getbuf((UINT16)(sizeof(BT_HDR) + num_handle * 2 + 1 + L2CAP_MIN_OFFSET))) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = 1;

        UINT8_TO_STREAM (p, GATT_REQ_READ_MULTI);

        for (i = 0; i < num_handle && p_buf->len + 2 <= payload_size; i ++)
        {
            UINT16_TO_STREAM (p, *(p_handle + i));
            p_buf->len += 2;
        }
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_handle_cmd
**
** Description      Build a read /read blob request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_handle_cmd(UINT8 op_code, UINT16 handle, UINT16 offset)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR) + 5 + L2CAP_MIN_OFFSET)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        p_buf->offset = L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, op_code);
        p_buf->len  = 1;

        UINT16_TO_STREAM (p, handle);
        p_buf->len += 2;

        if (op_code == GATT_REQ_READ_BLOB)
        {
            UINT16_TO_STREAM (p, offset);
            p_buf->len += 2;
        }

    }

    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_opcode_cmd
**
** Description      Build a  request/response with opcode only.
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_opcode_cmd(UINT8 op_code)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p;

    if ((p_buf = (BT_HDR *)GKI_getbuf(sizeof(BT_HDR) + 1 + L2CAP_MIN_OFFSET)) != NULL)
    {
        p = (UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;
        p_buf->offset = L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, op_code);
        p_buf->len  = 1;
    }

    return p_buf;
}
/*******************************************************************************
**
** Function         attp_build_value_cmd
**
** Description      Build a attribute value request
**
** Returns          None.
**
*******************************************************************************/
BT_HDR *attp_build_value_cmd (UINT16 payload_size, UINT8 op_code, UINT16 handle,
                              UINT16 offset, UINT16 len, UINT8 *p_data)
{
    BT_HDR      *p_buf = NULL;
    UINT8       *p, *pp, pair_len, *p_pair_len;

    if ((p_buf = (BT_HDR *)GKI_getbuf((UINT16)(sizeof(BT_HDR) + payload_size + L2CAP_MIN_OFFSET))) != NULL)
    {
        p = pp =(UINT8 *)(p_buf + 1) + L2CAP_MIN_OFFSET;

        UINT8_TO_STREAM (p, op_code);
        p_buf->offset = L2CAP_MIN_OFFSET;
        p_buf->len = 1;

        if (op_code == GATT_RSP_READ_BY_TYPE)
        {
            p_pair_len = p;
            pair_len = len + 2;
            UINT8_TO_STREAM (p, pair_len);
            p_buf->len += 1;
        }
        if (op_code != GATT_RSP_READ_BLOB && op_code != GATT_RSP_READ)
        {
            UINT16_TO_STREAM (p, handle);
            p_buf->len += 2;
        }

        if (op_code == GATT_REQ_PREPARE_WRITE ||op_code == GATT_RSP_PREPARE_WRITE )
        {
            UINT16_TO_STREAM (p, offset);
            p_buf->len += 2;
        }

        if (len > 0 && p_data != NULL)
        {
            /* ensure data not exceed MTU size */
            if (payload_size - p_buf->len < len)
            {
                len = payload_size - p_buf->len;
                /* update handle value pair length */
                if (op_code == GATT_RSP_READ_BY_TYPE)
                    *p_pair_len = (len + 2);

                GATT_TRACE_WARNING("attribute value too long, to be truncated to %d", len);
            }

            ARRAY_TO_STREAM (p, p_data, len);
            p_buf->len += len;
        }
    }
    return p_buf;
}

/*******************************************************************************
**
** Function         attp_send_msg_to_l2cap
**
** Description      Send message to L2CAP.
**
*******************************************************************************/
tGATT_STATUS attp_send_msg_to_l2cap(tGATT_TCB *p_tcb, BT_HDR *p_toL2CAP)
{
    UINT16      l2cap_ret;


    if (p_tcb->att_lcid == L2CAP_ATT_CID)
        l2cap_ret = L2CA_SendFixedChnlData (L2CAP_ATT_CID, p_tcb->peer_bda, p_toL2CAP);
    else
        l2cap_ret = (UINT16) L2CA_DataWrite (p_tcb->att_lcid, p_toL2CAP);

    if (l2cap_ret == L2CAP_DW_FAILED)
    {
        GATT_TRACE_ERROR("ATT   failed to pass msg:0x%0x to L2CAP",
            *((UINT8 *)(p_toL2CAP + 1) + p_toL2CAP->offset));
        return GATT_INTERNAL_ERROR;
    }
    else if (l2cap_ret == L2CAP_DW_CONGESTED)
    {
        GATT_TRACE_DEBUG("ATT congested, message accepted");
        return GATT_CONGESTED;
    }
    return GATT_SUCCESS;
}

/*******************************************************************************
**
** Function         attp_build_sr_msg
**
** Description      Build ATT Server PDUs.
**
*******************************************************************************/
BT_HDR *attp_build_sr_msg(tGATT_TCB *p_tcb, UINT8 op_code, tGATT_SR_MSG *p_msg)
{
    BT_HDR          *p_cmd = NULL;
    UINT16          offset = 0;

    switch (op_code)
    {
    case GATT_RSP_READ_BLOB:
    case GATT_RSP_PREPARE_WRITE:
        GATT_TRACE_EVENT ("ATT_RSP_READ_BLOB/GATT_RSP_PREPARE_WRITE: len = %d offset = %d",
                    p_msg->attr_value.len, p_msg->attr_value.offset);
        offset = p_msg->attr_value.offset;
/* Coverity: [FALSE-POSITIVE error] intended fall through */
/* Missing break statement between cases in switch statement */
        /* fall through */
    case GATT_RSP_READ_BY_TYPE:
    case GATT_RSP_READ:
    case GATT_HANDLE_VALUE_NOTIF:
    case GATT_HANDLE_VALUE_IND:
        p_cmd = attp_build_value_cmd(p_tcb->payload_size,
                                     op_code,
                                     p_msg->attr_value.handle,
                                     offset,
                                     p_msg->attr_value.len,
                                     p_msg->attr_value.value);
        break;

    case GATT_RSP_WRITE:
        p_cmd = attp_build_opcode_cmd(op_code);
        break;

    case GATT_RSP_ERROR:
        p_cmd = attp_build_err_cmd(p_msg->error.cmd_code, p_msg->error.handle, p_msg->error.reason);
        break;

    case GATT_RSP_EXEC_WRITE:
        p_cmd = attp_build_exec_write_cmd(op_code, 0);
        break;

    case GATT_RSP_MTU:
        p_cmd = attp_build_mtu_cmd(op_code, p_msg->mtu);
        break;

    default:
        GATT_TRACE_DEBUG("attp_build_sr_msg: unknown op code = %d", op_code);
        break;
    }

    if (!p_cmd)
        GATT_TRACE_ERROR("No resources");

    return p_cmd;
}

/*******************************************************************************
**
** Function         attp_send_sr_msg
**
** Description      This function sends the server response or indication message
**                  to client.
**
** Parameter        p_tcb: pointer to the connecton control block.
**                  p_msg: pointer to message parameters structure.
**
** Returns          GATT_SUCCESS if sucessfully sent; otherwise error code.
**
**
*******************************************************************************/
tGATT_STATUS attp_send_sr_msg (tGATT_TCB *p_tcb, BT_HDR *p_msg)
{
    tGATT_STATUS     cmd_sent = GATT_NO_RESOURCES;

    if (p_tcb != NULL)
    {
        if (p_msg != NULL)
        {
            p_msg->offset = L2CAP_MIN_OFFSET;
            cmd_sent = attp_send_msg_to_l2cap (p_tcb, p_msg);
        }
    }
    return cmd_sent;
}

/*******************************************************************************
**
** Function         attp_cl_send_cmd
**
** Description      Send a ATT command or enqueue it.
**
** Returns          GATT_SUCCESS if command sent
**                  GATT_CONGESTED if command sent but channel congested
**                  GATT_CMD_STARTED if command queue up in GATT
**                  GATT_ERROR if command sending failure
**
*******************************************************************************/
tGATT_STATUS attp_cl_send_cmd(tGATT_TCB *p_tcb, UINT16 clcb_idx, UINT8 cmd_code, BT_HDR *p_cmd)
{
    tGATT_STATUS att_ret = GATT_SUCCESS;

    if (p_tcb != NULL)
    {
        cmd_code &= ~GATT_AUTH_SIGN_MASK;

        /* no pending request or value confirmation */
        if (p_tcb->pending_cl_req == p_tcb->next_slot_inq ||
            cmd_code == GATT_HANDLE_VALUE_CONF)
        {
            att_ret = attp_send_msg_to_l2cap(p_tcb, p_cmd);
            if (att_ret == GATT_CONGESTED || att_ret == GATT_SUCCESS)
            {
                /* do not enq cmd if handle value confirmation or set request */
                if (cmd_code != GATT_HANDLE_VALUE_CONF && cmd_code != GATT_CMD_WRITE)
                {
                    gatt_start_rsp_timer (clcb_idx);
                    gatt_cmd_enq(p_tcb, clcb_idx, FALSE, cmd_code, NULL);
                }
            }
            else
                att_ret = GATT_INTERNAL_ERROR;
        }
        else
        {
            att_ret = GATT_CMD_STARTED;
            gatt_cmd_enq(p_tcb, clcb_idx, TRUE, cmd_code, p_cmd);
        }
    }
    else
        att_ret = GATT_ERROR;

    return att_ret;
}
/*******************************************************************************
**
** Function         attp_send_cl_msg
**
** Description      This function sends the client request or confirmation message
**                  to server.
**
** Parameter        p_tcb: pointer to the connectino control block.
**                  clcb_idx: clcb index
**                  op_code: message op code.
**                  p_msg: pointer to message parameters structure.
**
** Returns          GATT_SUCCESS if sucessfully sent; otherwise error code.
**
**
*******************************************************************************/
tGATT_STATUS attp_send_cl_msg (tGATT_TCB *p_tcb, UINT16 clcb_idx, UINT8 op_code, tGATT_CL_MSG *p_msg)
{
    tGATT_STATUS     status = GATT_NO_RESOURCES;
    BT_HDR          *p_cmd = NULL;
    UINT16          offset = 0, handle;

    if (p_tcb != NULL)
    {
        switch (op_code)
        {
        case GATT_REQ_MTU:
            if (p_msg->mtu <= GATT_MAX_MTU_SIZE)
            {
                p_tcb->payload_size = p_msg->mtu;
                p_cmd = attp_build_mtu_cmd(GATT_REQ_MTU, p_msg->mtu);
            }
            else
                status = GATT_ILLEGAL_PARAMETER;
            break;

        case GATT_REQ_FIND_INFO:
        case GATT_REQ_READ_BY_TYPE:
        case GATT_REQ_READ_BY_GRP_TYPE:
            if (GATT_HANDLE_IS_VALID (p_msg->browse.s_handle) &&
                GATT_HANDLE_IS_VALID (p_msg->browse.e_handle)  &&
                p_msg->browse.s_handle <= p_msg->browse.e_handle)
            {
                p_cmd = attp_build_browse_cmd(op_code,
                                            p_msg->browse.s_handle,
                                            p_msg->browse.e_handle,
                                            p_msg->browse.uuid);
            }
            else
                status = GATT_ILLEGAL_PARAMETER;
            break;

        case GATT_REQ_READ_BLOB:
            offset = p_msg->read_blob.offset;
            /* fall through */
        case GATT_REQ_READ:
            handle = (op_code == GATT_REQ_READ) ? p_msg->handle: p_msg->read_blob.handle;
            /*  handle checking */
            if (GATT_HANDLE_IS_VALID (handle))
            {
                p_cmd = attp_build_handle_cmd(op_code, handle, offset);
            }
            else
                status = GATT_ILLEGAL_PARAMETER;
            break;

        case GATT_HANDLE_VALUE_CONF:
            p_cmd = attp_build_opcode_cmd(op_code);
            break;

        case GATT_REQ_PREPARE_WRITE:
            offset = p_msg->attr_value.offset;
            /* fall through */
        case GATT_REQ_WRITE:
        case GATT_CMD_WRITE:
        case GATT_SIGN_CMD_WRITE:
            if (GATT_HANDLE_IS_VALID (p_msg->attr_value.handle))
            {
                p_cmd = attp_build_value_cmd (p_tcb->payload_size,
                                              op_code, p_msg->attr_value.handle,
                                              offset,
                                              p_msg->attr_value.len,
                                              p_msg->attr_value.value);
            }
            else
                status = GATT_ILLEGAL_PARAMETER;
            break;

        case GATT_REQ_EXEC_WRITE:
            p_cmd = attp_build_exec_write_cmd(op_code, p_msg->exec_write);
            break;

        case GATT_REQ_FIND_TYPE_VALUE:
            p_cmd = attp_build_read_by_type_value_cmd(p_tcb->payload_size, &p_msg->find_type_value);
            break;

        case GATT_REQ_READ_MULTI:
            p_cmd = attp_build_read_multi_cmd(p_tcb->payload_size,
                                              p_msg->read_multi.num_handles,
                                              p_msg->read_multi.handles);
            break;

        default:
            break;
        }

        if (p_cmd != NULL)
            status = attp_cl_send_cmd(p_tcb, clcb_idx, op_code, p_cmd);

    }
    else
    {
        GATT_TRACE_ERROR("Peer device not connected");
    }

    return status;
}
#endif
