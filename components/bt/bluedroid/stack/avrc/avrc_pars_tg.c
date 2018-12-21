/******************************************************************************
 *
 *  Copyright (C) 2003-2013 Broadcom Corporation
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

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)

/*****************************************************************************
**  Global data
*****************************************************************************/
#if (AVRC_METADATA_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         avrc_pars_vendor_cmd
**
** Description      This function parses the vendor specific commands defined by
**                  Bluetooth SIG
**
** Returns          AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
**                  Otherwise, the error code defined by AVRCP 1.4
**
*******************************************************************************/
static tAVRC_STS avrc_pars_vendor_cmd(tAVRC_MSG_VENDOR *p_msg, tAVRC_COMMAND *p_result,
                                      UINT8 *p_buf, UINT16 buf_len)
{
    tAVRC_STS  status = AVRC_STS_NO_ERROR;
    UINT8   *p;
    UINT16  len;
    UINT8   xx, yy;
    UINT8   *p_u8;
    UINT16  *p_u16;
    UINT32  u32, u32_2, *p_u32;
    tAVRC_APP_SETTING       *p_app_set;
    UINT16  size_needed;

    /* Check the vendor data */
    if (p_msg->vendor_len == 0) {
        return AVRC_STS_NO_ERROR;
    }
    if (p_msg->p_vendor_data == NULL) {
        return AVRC_STS_INTERNAL_ERR;
    }

    p = p_msg->p_vendor_data;
    p_result->pdu = *p++;
    AVRC_TRACE_DEBUG("avrc_pars_vendor_cmd() pdu:0x%x", p_result->pdu);
    if (!AVRC_IsValidAvcType (p_result->pdu, p_msg->hdr.ctype)) {
        AVRC_TRACE_DEBUG("avrc_pars_vendor_cmd() detects wrong AV/C type!");
        status = AVRC_STS_BAD_CMD;
    }

    p++; /* skip the reserved byte */
    BE_STREAM_TO_UINT16 (len, p);
    if ((len + 4) != (p_msg->vendor_len)) {
        status = AVRC_STS_INTERNAL_ERR;
    }

    if (status != AVRC_STS_NO_ERROR) {
        return status;
    }

    switch (p_result->pdu) {
    case AVRC_PDU_GET_CAPABILITIES:         /* 0x10 */
        p_result->get_caps.capability_id = *p++;
        if (!AVRC_IS_VALID_CAP_ID(p_result->get_caps.capability_id)) {
            status = AVRC_STS_BAD_PARAM;
        } else if (len != 1) {
            status = AVRC_STS_INTERNAL_ERR;
        }
        break;

    case AVRC_PDU_LIST_PLAYER_APP_ATTR:     /* 0x11 */
        /* no additional parameters */
        if (len != 0) {
            status = AVRC_STS_INTERNAL_ERR;
        }
        break;

    case AVRC_PDU_LIST_PLAYER_APP_VALUES:   /* 0x12 */
        p_result->list_app_values.attr_id = *p++;
        if (!AVRC_IS_VALID_ATTRIBUTE(p_result->list_app_values.attr_id)) {
            status = AVRC_STS_BAD_PARAM;
        } else if (len != 1) {
            status = AVRC_STS_INTERNAL_ERR;
        }
        break;

    case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE: /* 0x13 */
    case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT: /* 0x15 */
        BE_STREAM_TO_UINT8 (p_result->get_cur_app_val.num_attr, p);
        if (len != (p_result->get_cur_app_val.num_attr + 1)) {
            status = AVRC_STS_INTERNAL_ERR;
            break;
        }
        p_u8 = p_result->get_cur_app_val.attrs;
        for (xx = 0, yy = 0; xx < p_result->get_cur_app_val.num_attr; xx++) {
            /* only report the valid player app attributes */
            if (AVRC_IsValidPlayerAttr(*p)) {
                p_u8[yy++] = *p;
            }
            p++;
        }
        p_result->get_cur_app_val.num_attr = yy;
        if (yy == 0) {
            status = AVRC_STS_BAD_PARAM;
        }
        break;

    case AVRC_PDU_SET_PLAYER_APP_VALUE:     /* 0x14 */
        BE_STREAM_TO_UINT8 (p_result->set_app_val.num_val, p);
        size_needed = sizeof(tAVRC_APP_SETTING);
        if (p_buf && (len == ((p_result->set_app_val.num_val << 1) + 1))) {
            p_result->set_app_val.p_vals = (tAVRC_APP_SETTING *)p_buf;
            p_app_set = p_result->set_app_val.p_vals;
            for (xx = 0; ((xx < p_result->set_app_val.num_val) && (buf_len > size_needed)); xx++) {
                p_app_set[xx].attr_id = *p++;
                p_app_set[xx].attr_val = *p++;
                if (!avrc_is_valid_player_attrib_value(p_app_set[xx].attr_id, p_app_set[xx].attr_val)) {
                    status = AVRC_STS_BAD_PARAM;
                }
            }
            if (xx != p_result->set_app_val.num_val) {
                AVRC_TRACE_ERROR("AVRC_PDU_SET_PLAYER_APP_VALUE not enough room:%d orig num_val:%d",
                                 xx, p_result->set_app_val.num_val);
                p_result->set_app_val.num_val = xx;
            }
        } else {
            AVRC_TRACE_ERROR("AVRC_PDU_SET_PLAYER_APP_VALUE NULL decode buffer or bad len");
            status = AVRC_STS_INTERNAL_ERR;
        }
        break;

    case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT:/* 0x16 */
        if (len < 3) {
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            BE_STREAM_TO_UINT8 (p_result->get_app_val_txt.attr_id, p);
            if (!AVRC_IS_VALID_ATTRIBUTE(p_result->get_app_val_txt.attr_id)) {
                status = AVRC_STS_BAD_PARAM;
            } else {
                BE_STREAM_TO_UINT8 (p_result->get_app_val_txt.num_val, p);
                if ( (len - 2/* attr_id & num_val */) != p_result->get_app_val_txt.num_val) {
                    status = AVRC_STS_INTERNAL_ERR;
                } else {
                    p_u8 = p_result->get_app_val_txt.vals;
                    for (xx = 0; xx < p_result->get_app_val_txt.num_val; xx++) {
                        p_u8[xx] = *p++;
                        if (!avrc_is_valid_player_attrib_value(p_result->get_app_val_txt.attr_id,
                                                               p_u8[xx])) {
                            status = AVRC_STS_BAD_PARAM;
                            break;
                        }
                    }
                }
            }
        }
        break;

    case AVRC_PDU_INFORM_DISPLAY_CHARSET:  /* 0x17 */
        if (len < 3) {
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            BE_STREAM_TO_UINT8 (p_result->inform_charset.num_id, p);
            if ( (len - 1/* num_id */) != p_result->inform_charset.num_id * 2) {
                status = AVRC_STS_INTERNAL_ERR;
            } else {
                p_u16 = p_result->inform_charset.charsets;
                if (p_result->inform_charset.num_id > AVRC_MAX_CHARSET_SIZE) {
                    p_result->inform_charset.num_id = AVRC_MAX_CHARSET_SIZE;
                }
                for (xx = 0; xx < p_result->inform_charset.num_id; xx++) {
                    BE_STREAM_TO_UINT16 (p_u16[xx], p);
                }
            }
        }
        break;

    case AVRC_PDU_INFORM_BATTERY_STAT_OF_CT:/* 0x18 */
        if (len != 1) {
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            p_result->inform_battery_status.battery_status = *p++;
            if (!AVRC_IS_VALID_BATTERY_STATUS(p_result->inform_battery_status.battery_status)) {
                status = AVRC_STS_BAD_PARAM;
            }
        }
        break;

    case AVRC_PDU_GET_ELEMENT_ATTR:         /* 0x20 */
        if (len < 9) { /* UID/8 and num_attr/1 */
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            BE_STREAM_TO_UINT32 (u32, p);
            BE_STREAM_TO_UINT32 (u32_2, p);
            if (u32 == 0 && u32_2 == 0) {
                BE_STREAM_TO_UINT8 (p_result->get_elem_attrs.num_attr, p);
                if ( (len - 9/* UID/8 and num_attr/1 */) != (p_result->get_elem_attrs.num_attr * 4)) {
                    status = AVRC_STS_INTERNAL_ERR;
                } else {
                    p_u32 = p_result->get_elem_attrs.attrs;
                    if (p_result->get_elem_attrs.num_attr > AVRC_MAX_ELEM_ATTR_SIZE) {
                        p_result->get_elem_attrs.num_attr = AVRC_MAX_ELEM_ATTR_SIZE;
                    }
                    for (xx = 0; xx < p_result->get_elem_attrs.num_attr; xx++) {
                        BE_STREAM_TO_UINT32 (p_u32[xx], p);
                    }
                }
            } else {
                status = AVRC_STS_NOT_FOUND;
            }
        }
        break;

    case AVRC_PDU_GET_PLAY_STATUS:          /* 0x30 */
        /* no additional parameters */
        if (len != 0) {
            status = AVRC_STS_INTERNAL_ERR;
        }
        break;

    case AVRC_PDU_REGISTER_NOTIFICATION:    /* 0x31 */
        if (len != 5) {
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            BE_STREAM_TO_UINT8 (p_result->reg_notif.event_id, p);
            BE_STREAM_TO_UINT32 (p_result->reg_notif.param, p);
        }
        break;

    case AVRC_PDU_SET_ABSOLUTE_VOLUME: {
        if (len != 1) {
            status = AVRC_STS_INTERNAL_ERR;
        } else {
            BE_STREAM_TO_UINT8 (p_result->volume.volume, p);
            p_result->volume.volume &= 0x7F; // remove the top bit
        }
        break;
    }

    /* case AVRC_PDU_REQUEST_CONTINUATION_RSP: 0x40 */
    /* case AVRC_PDU_ABORT_CONTINUATION_RSP:   0x41 */

    default:
        status = AVRC_STS_BAD_CMD;
        break;
    }

    return status;
}

/*******************************************************************************
**
** Function         AVRC_ParsCommand
**
** Description      This function is a superset of AVRC_ParsMetadata to parse the command.
**
** Returns          AVRC_STS_NO_ERROR, if the message in p_data is parsed successfully.
**                  Otherwise, the error code defined by AVRCP 1.4
**
*******************************************************************************/
tAVRC_STS AVRC_ParsCommand (tAVRC_MSG *p_msg, tAVRC_COMMAND *p_result, UINT8 *p_buf, UINT16 buf_len)
{
    tAVRC_STS  status = AVRC_STS_INTERNAL_ERR;
    UINT16  id;

    if (p_msg && p_result) {
        switch (p_msg->hdr.opcode) {
        case AVRC_OP_VENDOR:     /*  0x00    Vendor-dependent commands */
            status = avrc_pars_vendor_cmd(&p_msg->vendor, p_result, p_buf, buf_len);
            break;

        case AVRC_OP_PASS_THRU:  /*  0x7C    panel subunit opcode */
            status = avrc_pars_pass_thru(&p_msg->pass, &id);
            if (status == AVRC_STS_NO_ERROR) {
                p_result->pdu = (UINT8)id;
            }
            break;

        default:
            AVRC_TRACE_ERROR("AVRC_ParsCommand() unknown opcode:0x%x", p_msg->hdr.opcode);
            break;
        }
        p_result->cmd.opcode = p_msg->hdr.opcode;
        p_result->cmd.status = status;
    }
    AVRC_TRACE_DEBUG("AVRC_ParsCommand() return status:0x%x", status);
    return status;
}

#endif /* (AVRC_METADATA_INCLUDED == TRUE) */

#endif /* #if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE) */
