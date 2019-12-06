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
#include "common/bt_target.h"
#include "stack/avrc_api.h"
#include "stack/avrc_defs.h"
#include "avrc_int.h"
#include "osi/allocator.h"

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)

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
static tAVRC_STS avrc_bld_register_change_notfn(UINT8 event_id, UINT32 event_parameter, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_vol_change");
    /* get the existing length, if any, and also the num attributes */
    // Set the notify value
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add fixed length 5 -*/
    UINT16_TO_BE_STREAM(p_data, 5);
    UINT8_TO_BE_STREAM(p_data, event_id);
    UINT32_TO_BE_STREAM(p_data, event_parameter);
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
    UINT8  opcode = avrc_opcode_from_pdu(p_cmd->pdu);
    AVRC_TRACE_API("avrc_bld_init_cmd_buffer: pdu=%x, opcode=%x", p_cmd->pdu, opcode);

    UINT16 offset = 0;
    switch (opcode) {
    case AVRC_OP_PASS_THRU:
        offset  = AVRC_MSG_PASS_THRU_OFFSET;
        break;

    case AVRC_OP_VENDOR:
        offset  = AVRC_MSG_VENDOR_OFFSET;
        break;
    }

    /* allocate and initialize the buffer */
    BT_HDR *p_pkt = (BT_HDR *)osi_malloc(AVRC_META_CMD_BUF_SIZE);
    if (p_pkt) {
        UINT8 *p_data, *p_start;

        p_pkt->layer_specific = AVCT_DATA_CTRL;
        p_pkt->event    = opcode;
        p_pkt->offset   = offset;
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        p_start = p_data;

        /* pass thru - group navigation - has a two byte op_id, so dont do it here */
        if (opcode != AVRC_OP_PASS_THRU) {
            *p_data++ = p_cmd->pdu;
        }

        switch (opcode) {
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
** Function         avrc_bld_set_player_value_cmd
**
** Description      This function builds the Set Player Application Value command.
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_set_player_value_cmd(tAVRC_SET_APP_VALUE_CMD *p_cmd, BT_HDR *p_pkt)
{
    UINT8 *p_data, *p_start;

    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add length */
    UINT16_TO_BE_STREAM(p_data, 3);
    /* Number of attributes */
    UINT8_TO_BE_STREAM(p_data, 1);
    UINT8_TO_BE_STREAM(p_data, p_cmd->p_vals->attr_id);
    UINT8_TO_BE_STREAM(p_data, p_cmd->p_vals->attr_val);

    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_get_element_attr_cmd
**
** Description      This function builds the Get Element Attribute command.
**
** Returns          AVRC_STS_NO_ERROR, if the command is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_element_attr_cmd (tAVRC_GET_ELEM_ATTRS_CMD *p_cmd, BT_HDR *p_pkt)
{
    int i;
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_get_element_attr_cmd num_attr: %d", p_cmd->num_attr);
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add length */
    UINT16_TO_BE_STREAM(p_data, 8 + 1 /* id + attr count */ + p_cmd->num_attr * sizeof(UINT32));
    /* Identifier 0x0 (PLAYING) */
    UINT64_TO_BE_STREAM(p_data, (UINT64)(0));
    /* Attribute count */
    UINT8_TO_BE_STREAM(p_data, p_cmd->num_attr);

    for (i = 0; i < p_cmd->num_attr; i++) {
        AVRC_TRACE_API("avrc_bld_get_element_attr_cmd attr_id: %d", p_cmd->attrs[i]);
        UINT32_TO_BE_STREAM(p_data, p_cmd->attrs[i]);
    }

    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}

static tAVRC_STS avrc_bld_get_caps_cmd(tAVRC_GET_CAPS_CMD *p_cmd, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_get_caps");
    /* get the existing length, if any, and also the num attributes */
    // Set the notify value
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2; /* pdu + rsvd */
    /* add fixed length 1 */
    UINT16_TO_BE_STREAM(p_data, 1);
    /* capability id */
    UINT8_TO_BE_STREAM(p_data, p_cmd->capability_id);
    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
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
    if (!p_cmd || !pp_pkt) {
        AVRC_TRACE_API("AVRC_BldCommand. Invalid parameters passed. p_cmd=%p, pp_pkt=%p",
                       p_cmd, pp_pkt);
        return AVRC_STS_BAD_PARAM;
    }

    if (*pp_pkt == NULL) {
        if ((*pp_pkt = avrc_bld_init_cmd_buffer(p_cmd)) == NULL) {
            AVRC_TRACE_API("AVRC_BldCommand: Failed to initialize command buffer");
            return AVRC_STS_INTERNAL_ERR;
        }
        alloc = TRUE;
    }
    status = AVRC_STS_NO_ERROR;
    p_pkt = *pp_pkt;

    switch (p_cmd->pdu) {
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

    case AVRC_PDU_SET_PLAYER_APP_VALUE:       /* 0x14 */
        status = avrc_bld_set_player_value_cmd(&p_cmd->set_app_val, p_pkt);
        break;

    case AVRC_PDU_GET_ELEMENT_ATTR:         /* 0x20 */
        status = avrc_bld_get_element_attr_cmd(&p_cmd->get_elem_attrs, p_pkt);
        break;

    case AVRC_PDU_REGISTER_NOTIFICATION:      /* 0x31 */
        status = avrc_bld_register_change_notfn(p_cmd->reg_notif.event_id, p_cmd->reg_notif.param, p_pkt);
        break;
    case AVRC_PDU_GET_CAPABILITIES:
        status = avrc_bld_get_caps_cmd(&p_cmd->get_caps, p_pkt);
        break;
    }

    if (alloc && (status != AVRC_STS_NO_ERROR) ) {
        osi_free(p_pkt);
        *pp_pkt = NULL;
    }
    AVRC_TRACE_API("AVRC_BldCommand: returning %d", status);
    return status;
}
#endif /* (AVRC_METADATA_INCLUDED == TRUE) */

#endif /* #if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE) */
