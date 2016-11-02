/******************************************************************************
 *
 *  Copyright (C) 2006-2013 Broadcom Corporation
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
#include <string.h>

#include "gki.h"
#include "avrc_api.h"
#include "avrc_defs.h"
#include "avrc_int.h"

/*****************************************************************************
**  Global data
*****************************************************************************/


#if (AVRC_METADATA_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         avrc_bld_next_cmd
**
** Description      This function builds the Request Continue or Abort command.
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_next_cmd (tAVRC_NEXT_CMD *p_cmd, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_next_cmd");

    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */

    /* add fixed lenth 1 - pdu_id (1) */
    UINT16_TO_BE_STREAM(p_data, 1);
    UINT8_TO_BE_STREAM(p_data, p_cmd->target_pdu);
    p_pkt->len = (p_data - p_start);

    return AVRC_STS_NO_ERROR;
}

/*****************************************************************************
**  the following commands are introduced in AVRCP 1.4
*****************************************************************************/

#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         avrc_bld_set_abs_volume_cmd
**
** Description      This function builds the Set Absolute Volume command.
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_set_abs_volume_cmd (tAVRC_SET_VOLUME_CMD *p_cmd, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_set_abs_volume_cmd");
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add fixed lenth 1 - volume (1) */
    UINT16_TO_BE_STREAM(p_data, 1);
    UINT8_TO_BE_STREAM(p_data, (AVRC_MAX_VOLUME & p_cmd->volume));
    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_vol_change_notfn
**
** Description      This function builds the register notification for volume change.
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_vol_change_notfn(BT_HDR * p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_vol_change");
    /* get the existing length, if any, and also the num attributes */
    // Set the notify value
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add fixed length 5 -*/
    UINT16_TO_BE_STREAM(p_data, 5);
    UINT8_TO_BE_STREAM(p_data,AVRC_EVT_VOLUME_CHANGE);
    UINT32_TO_BE_STREAM(p_data, 0);
    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}
#endif

/*******************************************************************************
**
** Function         avrc_bld_init_cmd_buffer
**
** Description      This function initializes the command buffer based on PDU
**
** Returns          NULL, if no GKI buffer or failure to build the message.
**                  Otherwise, the GKI buffer that contains the initialized message.
**
*******************************************************************************/
static BT_HDR *avrc_bld_init_cmd_buffer(tAVRC_COMMAND *p_cmd)
{
    UINT16 offset = 0, chnl = AVCT_DATA_CTRL, len=AVRC_META_CMD_POOL_SIZE;
    BT_HDR *p_pkt=NULL;
    UINT8  opcode;

    opcode = avrc_opcode_from_pdu(p_cmd->pdu);
    AVRC_TRACE_API("avrc_bld_init_cmd_buffer: pdu=%x, opcode=%x", p_cmd->pdu, opcode);

    switch (opcode)
    {
    case AVRC_OP_PASS_THRU:
        offset  = AVRC_MSG_PASS_THRU_OFFSET;
        break;

    case AVRC_OP_VENDOR:
        offset  = AVRC_MSG_VENDOR_OFFSET;
        break;
    }

    /* allocate and initialize the buffer */
    p_pkt = (BT_HDR *)GKI_getbuf(len);
    if (p_pkt)
    {
        UINT8 *p_data, *p_start;

        p_pkt->layer_specific = chnl;
        p_pkt->event    = opcode;
        p_pkt->offset   = offset;
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        p_start = p_data;

        /* pass thru - group navigation - has a two byte op_id, so dont do it here */
        if (opcode != AVRC_OP_PASS_THRU)
            *p_data++ = p_cmd->pdu;

        switch (opcode)
        {
        case AVRC_OP_VENDOR:
            /* reserved 0, packet_type 0 */
            UINT8_TO_BE_STREAM(p_data, 0);
            /* continue to the next "case to add length */
            /* add fixed lenth - 0 */
            UINT16_TO_BE_STREAM(p_data, 0);
            break;
        }

        p_pkt->len = (p_data - p_start);
    }
    p_cmd->cmd.opcode = opcode;
    return p_pkt;
}

/*******************************************************************************
**
** Function         AVRC_BldCommand
**
** Description      This function builds the given AVRCP command to the given
**                  GKI buffer
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
tAVRC_STS AVRC_BldCommand( tAVRC_COMMAND *p_cmd, BT_HDR **pp_pkt)
{
    tAVRC_STS status = AVRC_STS_BAD_PARAM;
    BT_HDR  *p_pkt;
    BOOLEAN alloc = FALSE;

    AVRC_TRACE_API("AVRC_BldCommand: pdu=%x status=%x", p_cmd->cmd.pdu, p_cmd->cmd.status);
    if (!p_cmd || !pp_pkt)
    {
        AVRC_TRACE_API("AVRC_BldCommand. Invalid parameters passed. p_cmd=%p, pp_pkt=%p",
            p_cmd, pp_pkt);
        return AVRC_STS_BAD_PARAM;
    }

    if (*pp_pkt == NULL)
    {
        if ((*pp_pkt = avrc_bld_init_cmd_buffer(p_cmd)) == NULL)
        {
            AVRC_TRACE_API("AVRC_BldCommand: Failed to initialize command buffer");
            return AVRC_STS_INTERNAL_ERR;
        }
        alloc = TRUE;
    }
    status = AVRC_STS_NO_ERROR;
    p_pkt = *pp_pkt;

    switch (p_cmd->pdu)
    {
    case AVRC_PDU_REQUEST_CONTINUATION_RSP:     /*        0x40 */
        status = avrc_bld_next_cmd(&p_cmd->continu, p_pkt);
        break;

    case AVRC_PDU_ABORT_CONTINUATION_RSP:       /*          0x41 */
        status = avrc_bld_next_cmd(&p_cmd->abort, p_pkt);
        break;
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
    case AVRC_PDU_SET_ABSOLUTE_VOLUME:         /* 0x50 */
        status = avrc_bld_set_abs_volume_cmd(&p_cmd->volume, p_pkt);
        break;
#endif

    case AVRC_PDU_REGISTER_NOTIFICATION:      /* 0x31 */
#if (AVRC_ADV_CTRL_INCLUDED == TRUE)
        if(AVRC_EVT_VOLUME_CHANGE==p_cmd->reg_notif.event_id)
           status=avrc_bld_vol_change_notfn(p_pkt);
#endif
        break;

    }

    if (alloc && (status != AVRC_STS_NO_ERROR) )
    {
        GKI_freebuf(p_pkt);
        *pp_pkt = NULL;
    }
    AVRC_TRACE_API("AVRC_BldCommand: returning %d", status);
    return status;
}
#endif /* (AVRC_METADATA_INCLUDED == TRUE) */

