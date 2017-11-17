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
#include "bt_target.h"
#include "avrc_api.h"
#include "avrc_defs.h"
#include "avrc_int.h"
#include "bt_utils.h"
#include "allocator.h"

#if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE)

/*****************************************************************************
**  Global data
*****************************************************************************/
#if (AVRC_METADATA_INCLUDED == TRUE)

/*******************************************************************************
**
** Function         avrc_bld_get_capability_rsp
**
** Description      This function builds the Get Capability response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_capability_rsp (tAVRC_GET_CAPS_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_count;
    UINT16  len = 0;
    UINT8   xx;
    UINT32  *p_company_id;
    UINT8   *p_event_id;
    tAVRC_STS status = AVRC_STS_NO_ERROR;

    if (!(AVRC_IS_VALID_CAP_ID(p_rsp->capability_id))) {
        AVRC_TRACE_ERROR("avrc_bld_get_capability_rsp bad parameter. p_rsp: %x", (int)p_rsp);
        status = AVRC_STS_BAD_PARAM;
        return status;
    }

    AVRC_TRACE_API("avrc_bld_get_capability_rsp");
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    BE_STREAM_TO_UINT16(len, p_data);
    UINT8_TO_BE_STREAM(p_data, p_rsp->capability_id);
    p_count = p_data;

    if (len == 0) {
        *p_count = p_rsp->count;
        p_data++;
        len = 2; /* move past the capability_id and count */
    } else {
        p_data = p_start + p_pkt->len;
        *p_count += p_rsp->count;
    }

    if (p_rsp->capability_id == AVRC_CAP_COMPANY_ID) {
        p_company_id = p_rsp->param.company_id;
        for (xx = 0; xx < p_rsp->count; xx++) {
            UINT24_TO_BE_STREAM(p_data, p_company_id[xx]);
        }
        len += p_rsp->count * 3;
    } else {
        p_event_id = p_rsp->param.event_id;
        *p_count = 0;
        for (xx = 0; xx < p_rsp->count; xx++) {
            if (AVRC_IS_VALID_EVENT_ID(p_event_id[xx])) {
                (*p_count)++;
                UINT8_TO_BE_STREAM(p_data, p_event_id[xx]);
            }
        }
        len += (*p_count);
    }
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);
    status = AVRC_STS_NO_ERROR;

    return status;
}

/*******************************************************************************
**
** Function         avrc_bld_list_app_settings_attr_rsp
**
** Description      This function builds the List Application Settings Attribute
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_list_app_settings_attr_rsp (tAVRC_LIST_APP_ATTR_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_num;
    UINT16  len = 0;
    UINT8   xx;

    AVRC_TRACE_API("avrc_bld_list_app_settings_attr_rsp");
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    BE_STREAM_TO_UINT16(len, p_data);
    p_num = p_data;
    if (len == 0) {
        /* first time initialize the attribute count */
        *p_num = 0;
        p_data++;
    } else {
        p_data = p_start + p_pkt->len;
    }

    for (xx = 0; xx < p_rsp->num_attr; xx++) {
        if (AVRC_IsValidPlayerAttr(p_rsp->attrs[xx])) {
            (*p_num)++;
            UINT8_TO_BE_STREAM(p_data, p_rsp->attrs[xx]);
        }
    }

    len = *p_num + 1;
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);

    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_list_app_settings_values_rsp
**
** Description      This function builds the List Application Setting Values
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_list_app_settings_values_rsp (tAVRC_LIST_APP_VALUES_RSP *p_rsp,
        BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_num;
    UINT8   xx;
    UINT16  len;

    AVRC_TRACE_API("avrc_bld_list_app_settings_values_rsp");

    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    /* get the existing length, if any, and also the num attributes */
    BE_STREAM_TO_UINT16(len, p_data);
    p_num = p_data;
    /* first time initialize the attribute count */
    if (len == 0) {
        *p_num = p_rsp->num_val;
        p_data++;
    } else {
        p_data = p_start + p_pkt->len;
        *p_num += p_rsp->num_val;
    }


    for (xx = 0; xx < p_rsp->num_val; xx++) {
        UINT8_TO_BE_STREAM(p_data, p_rsp->vals[xx]);
    }

    len = *p_num + 1;
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_get_cur_app_setting_value_rsp
**
** Description      This function builds the Get Current Application Setting Value
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_cur_app_setting_value_rsp (tAVRC_GET_CUR_APP_VALUE_RSP *p_rsp,
        BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_count;
    UINT16  len;
    UINT8   xx;

    if (!p_rsp->p_vals) {
        AVRC_TRACE_ERROR("avrc_bld_get_cur_app_setting_value_rsp NULL parameter");
        return AVRC_STS_BAD_PARAM;
    }

    AVRC_TRACE_API("avrc_bld_get_cur_app_setting_value_rsp");
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    BE_STREAM_TO_UINT16(len, p_data);
    p_count = p_data;
    if (len == 0) {
        /* first time initialize the attribute count */
        *p_count = 0;
        p_data++;
    } else {
        p_data = p_start + p_pkt->len;
    }

    for (xx = 0; xx < p_rsp->num_val; xx++) {
        if (avrc_is_valid_player_attrib_value(p_rsp->p_vals[xx].attr_id, p_rsp->p_vals[xx].attr_val)) {
            (*p_count)++;
            UINT8_TO_BE_STREAM(p_data, p_rsp->p_vals[xx].attr_id);
            UINT8_TO_BE_STREAM(p_data, p_rsp->p_vals[xx].attr_val);
        }
    }
    len = ((*p_count) << 1) + 1;
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);

    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_set_app_setting_value_rsp
**
** Description      This function builds the Set Application Setting Value
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_set_app_setting_value_rsp (tAVRC_RSP *p_rsp, BT_HDR *p_pkt)
{
    UNUSED(p_rsp);
    UNUSED(p_pkt);

    /* nothing to be added. */
    AVRC_TRACE_API("avrc_bld_set_app_setting_value_rsp");
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_app_setting_text_rsp
**
** Description      This function builds the Get Application Settings Attribute Text
**                  or Get Application Settings Value Text response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_app_setting_text_rsp (tAVRC_GET_APP_ATTR_TXT_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_count;
    UINT16  len, len_left;
    UINT8   xx;
    tAVRC_STS   sts = AVRC_STS_NO_ERROR;
    UINT8       num_added = 0;

    if (!p_rsp->p_attrs) {
        AVRC_TRACE_ERROR("avrc_bld_app_setting_text_rsp NULL parameter");
        return AVRC_STS_BAD_PARAM;
    }
    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    /*
     * NOTE: The buffer is allocated within avrc_bld_init_rsp_buffer(), and is
     * always of size BT_DEFAULT_BUFFER_SIZE.
     */
    len_left = BT_DEFAULT_BUFFER_SIZE - BT_HDR_SIZE - p_pkt->offset - p_pkt->len;

    BE_STREAM_TO_UINT16(len, p_data);
    p_count = p_data;

    if (len == 0) {
        *p_count = 0;
        p_data++;
    } else {
        p_data = p_start + p_pkt->len;
    }

    for (xx = 0; xx < p_rsp->num_attr; xx++) {
        if  (len_left < (p_rsp->p_attrs[xx].str_len + 4)) {
            AVRC_TRACE_ERROR("avrc_bld_app_setting_text_rsp out of room %d(str_len:%d, left:%d)",
                             xx, p_rsp->p_attrs[xx].str_len, len_left);
            p_rsp->num_attr = num_added;
            sts = AVRC_STS_INTERNAL_ERR;
            break;
        }
        if ( !p_rsp->p_attrs[xx].str_len || !p_rsp->p_attrs[xx].p_str ) {
            AVRC_TRACE_ERROR("avrc_bld_app_setting_text_rsp NULL attr text[%d]", xx);
            continue;
        }
        UINT8_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].attr_id);
        UINT16_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].charset_id);
        UINT8_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].str_len);
        ARRAY_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].p_str, p_rsp->p_attrs[xx].str_len);
        (*p_count)++;
        num_added++;
    }
    len = p_data - p_count;
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);

    return sts;
}

/*******************************************************************************
**
** Function         avrc_bld_get_app_setting_attr_text_rsp
**
** Description      This function builds the Get Application Setting Attribute Text
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_app_setting_attr_text_rsp (tAVRC_GET_APP_ATTR_TXT_RSP *p_rsp,
        BT_HDR *p_pkt)
{
    AVRC_TRACE_API("avrc_bld_get_app_setting_attr_text_rsp");
    return avrc_bld_app_setting_text_rsp(p_rsp, p_pkt);
}

/*******************************************************************************
**
** Function         avrc_bld_get_app_setting_value_text_rsp
**
** Description      This function builds the Get Application Setting Value Text
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_app_setting_value_text_rsp (tAVRC_GET_APP_ATTR_TXT_RSP *p_rsp,
        BT_HDR *p_pkt)
{
    AVRC_TRACE_API("avrc_bld_get_app_setting_value_text_rsp");
    return avrc_bld_app_setting_text_rsp(p_rsp, p_pkt);
}

/*******************************************************************************
**
** Function         avrc_bld_inform_charset_rsp
**
** Description      This function builds the Inform Displayable Character Set
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_inform_charset_rsp (tAVRC_RSP *p_rsp, BT_HDR *p_pkt)
{
    UNUSED(p_rsp);
    UNUSED(p_pkt);

    /* nothing to be added. */
    AVRC_TRACE_API("avrc_bld_inform_charset_rsp");
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_inform_battery_status_rsp
**
** Description      This function builds the Inform Battery Status
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_inform_battery_status_rsp (tAVRC_RSP *p_rsp, BT_HDR *p_pkt)
{
    UNUSED(p_rsp);
    UNUSED(p_pkt);

    /* nothing to be added. */
    AVRC_TRACE_API("avrc_bld_inform_battery_status_rsp");
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_get_elem_attrs_rsp
**
** Description      This function builds the Get Element Attributes
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_elem_attrs_rsp (tAVRC_GET_ELEM_ATTRS_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start, *p_len, *p_count;
    UINT16  len;
    UINT8   xx;

    AVRC_TRACE_API("avrc_bld_get_elem_attrs_rsp");
    if (!p_rsp->p_attrs) {
        AVRC_TRACE_ERROR("avrc_bld_get_elem_attrs_rsp NULL parameter");
        return AVRC_STS_BAD_PARAM;
    }

    /* get the existing length, if any, and also the num attributes */
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */

    BE_STREAM_TO_UINT16(len, p_data);
    p_count = p_data;

    if (len == 0) {
        *p_count = 0;
        p_data++;
    } else {
        p_data = p_start + p_pkt->len;
    }

    for (xx = 0; xx < p_rsp->num_attr; xx++) {
        if (!AVRC_IS_VALID_MEDIA_ATTRIBUTE(p_rsp->p_attrs[xx].attr_id)) {
            AVRC_TRACE_ERROR("avrc_bld_get_elem_attrs_rsp invalid attr id[%d]: %d", xx, p_rsp->p_attrs[xx].attr_id);
            continue;
        }
        if ( !p_rsp->p_attrs[xx].name.p_str ) {
            p_rsp->p_attrs[xx].name.str_len = 0;
        }
        UINT32_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].attr_id);
        UINT16_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].name.charset_id);
        UINT16_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].name.str_len);
        ARRAY_TO_BE_STREAM(p_data, p_rsp->p_attrs[xx].name.p_str, p_rsp->p_attrs[xx].name.str_len);
        (*p_count)++;
    }
    len = p_data - p_count;
    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_get_play_status_rsp
**
** Description      This function builds the Get Play Status
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_get_play_status_rsp (tAVRC_GET_PLAY_STATUS_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_get_play_status_rsp");
    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2;

    /* add fixed lenth - song len(4) + song position(4) + status(1) */
    UINT16_TO_BE_STREAM(p_data, 9);
    UINT32_TO_BE_STREAM(p_data, p_rsp->song_len);
    UINT32_TO_BE_STREAM(p_data, p_rsp->song_pos);
    UINT8_TO_BE_STREAM(p_data, p_rsp->play_status);
    p_pkt->len = (p_data - p_start);

    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_notify_rsp
**
** Description      This function builds the Notification response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_notify_rsp (tAVRC_REG_NOTIF_RSP *p_rsp, BT_HDR *p_pkt)
{
    UINT8   *p_data, *p_start;
    UINT8   *p_len;
    UINT16  len = 0;
    UINT8   xx;
    tAVRC_STS status = AVRC_STS_NO_ERROR;

    AVRC_TRACE_API("avrc_bld_notify_rsp");

    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_len = p_start + 2; /* pdu + rsvd */
    p_data += 2;

    UINT8_TO_BE_STREAM(p_data, p_rsp->event_id);
    switch (p_rsp->event_id) {
    case AVRC_EVT_PLAY_STATUS_CHANGE:       /* 0x01 */
        /* p_rsp->param.play_status >= AVRC_PLAYSTATE_STOPPED is always TRUE */
        if ((p_rsp->param.play_status <= AVRC_PLAYSTATE_REV_SEEK) ||
                (p_rsp->param.play_status == AVRC_PLAYSTATE_ERROR) ) {
            UINT8_TO_BE_STREAM(p_data, p_rsp->param.play_status);
            len = 2;
        } else {
            AVRC_TRACE_ERROR("bad play state");
            status = AVRC_STS_BAD_PARAM;
        }
        break;

    case AVRC_EVT_TRACK_CHANGE:             /* 0x02 */
        ARRAY_TO_BE_STREAM(p_data, p_rsp->param.track, AVRC_UID_SIZE);
        len = (UINT8)(AVRC_UID_SIZE + 1);
        break;

    case AVRC_EVT_TRACK_REACHED_END:        /* 0x03 */
    case AVRC_EVT_TRACK_REACHED_START:      /* 0x04 */
        len = 1;
        break;

    case AVRC_EVT_PLAY_POS_CHANGED:         /* 0x05 */
        UINT32_TO_BE_STREAM(p_data, p_rsp->param.play_pos);
        len = 5;
        break;

    case AVRC_EVT_BATTERY_STATUS_CHANGE:    /* 0x06 */
        if (AVRC_IS_VALID_BATTERY_STATUS(p_rsp->param.battery_status)) {
            UINT8_TO_BE_STREAM(p_data, p_rsp->param.battery_status);
            len = 2;
        } else {
            AVRC_TRACE_ERROR("bad battery status");
            status = AVRC_STS_BAD_PARAM;
        }
        break;

    case AVRC_EVT_SYSTEM_STATUS_CHANGE:     /* 0x07 */
        if (AVRC_IS_VALID_SYSTEM_STATUS(p_rsp->param.system_status)) {
            UINT8_TO_BE_STREAM(p_data, p_rsp->param.system_status);
            len = 2;
        } else {
            AVRC_TRACE_ERROR("bad system status");
            status = AVRC_STS_BAD_PARAM;
        }
        break;

    case AVRC_EVT_APP_SETTING_CHANGE:       /* 0x08 */
        if (p_rsp->param.player_setting.num_attr > AVRC_MAX_APP_SETTINGS) {
            p_rsp->param.player_setting.num_attr = AVRC_MAX_APP_SETTINGS;
        }

        if (p_rsp->param.player_setting.num_attr > 0) {
            UINT8_TO_BE_STREAM(p_data, p_rsp->param.player_setting.num_attr);
            len = 2;
            for (xx = 0; xx < p_rsp->param.player_setting.num_attr; xx++) {
                if (avrc_is_valid_player_attrib_value(p_rsp->param.player_setting.attr_id[xx],
                                                      p_rsp->param.player_setting.attr_value[xx])) {
                    UINT8_TO_BE_STREAM(p_data, p_rsp->param.player_setting.attr_id[xx]);
                    UINT8_TO_BE_STREAM(p_data, p_rsp->param.player_setting.attr_value[xx]);
                } else {
                    AVRC_TRACE_ERROR("bad player app seeting attribute or value");
                    status = AVRC_STS_BAD_PARAM;
                    break;
                }
                len += 2;
            }
        } else {
            status = AVRC_STS_BAD_PARAM;
        }
        break;

    default:
        status = AVRC_STS_BAD_PARAM;
        AVRC_TRACE_ERROR("unknown event_id");
    }

    UINT16_TO_BE_STREAM(p_len, len);
    p_pkt->len = (p_data - p_start);

    return status;
}

/*******************************************************************************
**
** Function         avrc_bld_next_rsp
**
** Description      This function builds the Request Continue or Abort
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_next_rsp (tAVRC_RSP *p_rsp, BT_HDR *p_pkt)
{
    UNUSED(p_rsp);
    UNUSED(p_pkt);

    /* nothing to be added. */
    AVRC_TRACE_API("avrc_bld_next_rsp");
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_group_navigation_rsp
**
** Description      This function builds the Group Navigation
**                  response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
tAVRC_STS avrc_bld_group_navigation_rsp (UINT16 navi_id, BT_HDR *p_pkt)
{
    UINT8   *p_data;

    if (!AVRC_IS_VALID_GROUP(navi_id)) {
        AVRC_TRACE_ERROR("avrc_bld_group_navigation_rsp bad navigation op id: %d", navi_id);
        return AVRC_STS_BAD_PARAM;
    }

    AVRC_TRACE_API("avrc_bld_group_navigation_rsp");
    p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    UINT16_TO_BE_STREAM(p_data, navi_id);
    p_pkt->len = 2;
    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_rejected_rsp
**
** Description      This function builds the General Response response.
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
static tAVRC_STS avrc_bld_rejected_rsp( tAVRC_RSP *p_rsp, BT_HDR *p_pkt )
{
    UINT8 *p_data, *p_start;

    AVRC_TRACE_API("avrc_bld_rejected_rsp: status=%d, pdu:x%x", p_rsp->status, p_rsp->pdu);

    p_start = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
    p_data = p_start + 2;
    AVRC_TRACE_DEBUG("pdu:x%x", *p_start);

    UINT16_TO_BE_STREAM(p_data, 1);
    UINT8_TO_BE_STREAM(p_data, p_rsp->status);
    p_pkt->len = p_data - p_start;

    return AVRC_STS_NO_ERROR;
}

/*******************************************************************************
**
** Function         avrc_bld_init_rsp_buffer
**
** Description      This function initializes the response buffer based on PDU
**
** Returns          NULL, if no GKI buffer or failure to build the message.
**                  Otherwise, the GKI buffer that contains the initialized message.
**
*******************************************************************************/
static BT_HDR *avrc_bld_init_rsp_buffer(tAVRC_RESPONSE *p_rsp)
{
    UINT16 offset = AVRC_MSG_PASS_THRU_OFFSET;
    UINT16 chnl = AVCT_DATA_CTRL;
    UINT8  opcode = avrc_opcode_from_pdu(p_rsp->pdu);

    AVRC_TRACE_API("avrc_bld_init_rsp_buffer: pdu=%x, opcode=%x/%x", p_rsp->pdu, opcode,
                   p_rsp->rsp.opcode);
    if (opcode != p_rsp->rsp.opcode && p_rsp->rsp.status != AVRC_STS_NO_ERROR &&
            avrc_is_valid_opcode(p_rsp->rsp.opcode)) {
        opcode = p_rsp->rsp.opcode;
        AVRC_TRACE_API("opcode=%x", opcode);
    }

    switch (opcode) {
    case AVRC_OP_PASS_THRU:
        offset  = AVRC_MSG_PASS_THRU_OFFSET;
        break;

    case AVRC_OP_VENDOR:
        offset  = AVRC_MSG_VENDOR_OFFSET;
        break;
    }

    /* allocate and initialize the buffer */
    BT_HDR *p_pkt = (BT_HDR *)osi_malloc(BT_DEFAULT_BUFFER_SIZE);
    if (p_pkt) {
        UINT8 *p_data, *p_start;

        p_pkt->layer_specific = chnl;
        p_pkt->event    = opcode;
        p_pkt->offset   = offset;
        p_data = (UINT8 *)(p_pkt + 1) + p_pkt->offset;
        p_start = p_data;

        /* pass thru - group navigation - has a two byte op_id, so dont do it here */
        if (opcode != AVRC_OP_PASS_THRU) {
            *p_data++ = p_rsp->pdu;
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
    p_rsp->rsp.opcode = opcode;
    return p_pkt;
}

/*******************************************************************************
**
** Function         AVRC_BldResponse
**
** Description      This function builds the given AVRCP response to the given
**                  GKI buffer
**
** Returns          AVRC_STS_NO_ERROR, if the response is built successfully
**                  Otherwise, the error code.
**
*******************************************************************************/
tAVRC_STS AVRC_BldResponse( UINT8 handle, tAVRC_RESPONSE *p_rsp, BT_HDR **pp_pkt)
{
    tAVRC_STS status = AVRC_STS_BAD_PARAM;
    BT_HDR *p_pkt;
    BOOLEAN alloc = FALSE;
    UNUSED(handle);

    if (!p_rsp || !pp_pkt) {
        AVRC_TRACE_API("AVRC_BldResponse. Invalid parameters passed. p_rsp=%p, pp_pkt=%p",
                       p_rsp, pp_pkt);
        return AVRC_STS_BAD_PARAM;
    }

    if (*pp_pkt == NULL) {
        if ((*pp_pkt = avrc_bld_init_rsp_buffer(p_rsp)) == NULL) {
            AVRC_TRACE_API("AVRC_BldResponse: Failed to initialize response buffer");
            return AVRC_STS_INTERNAL_ERR;
        }
        alloc = TRUE;
    }
    status = AVRC_STS_NO_ERROR;
    p_pkt = *pp_pkt;

    AVRC_TRACE_API("AVRC_BldResponse: pdu=%x status=%x", p_rsp->rsp.pdu, p_rsp->rsp.status);
    if (p_rsp->rsp.status != AVRC_STS_NO_ERROR) {
        return ( avrc_bld_rejected_rsp(&p_rsp->rsp, p_pkt) );
    }

    switch (p_rsp->pdu) {
    case AVRC_PDU_NEXT_GROUP:
    case AVRC_PDU_PREV_GROUP:
        status = avrc_bld_group_navigation_rsp(p_rsp->pdu, p_pkt);
        break;

    case AVRC_PDU_GET_CAPABILITIES:
        status = avrc_bld_get_capability_rsp(&p_rsp->get_caps, p_pkt);
        break;

    case AVRC_PDU_LIST_PLAYER_APP_ATTR:
        status = avrc_bld_list_app_settings_attr_rsp(&p_rsp->list_app_attr, p_pkt);
        break;

    case AVRC_PDU_LIST_PLAYER_APP_VALUES:
        status = avrc_bld_list_app_settings_values_rsp(&p_rsp->list_app_values, p_pkt);
        break;

    case AVRC_PDU_GET_CUR_PLAYER_APP_VALUE:
        status = avrc_bld_get_cur_app_setting_value_rsp(&p_rsp->get_cur_app_val, p_pkt);
        break;

    case AVRC_PDU_SET_PLAYER_APP_VALUE:
        status = avrc_bld_set_app_setting_value_rsp(&p_rsp->set_app_val, p_pkt);
        break;

    case AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT:
        status = avrc_bld_get_app_setting_attr_text_rsp(&p_rsp->get_app_attr_txt, p_pkt);
        break;

    case AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT:
        status = avrc_bld_get_app_setting_value_text_rsp(&p_rsp->get_app_val_txt, p_pkt);
        break;

    case AVRC_PDU_INFORM_DISPLAY_CHARSET:
        status = avrc_bld_inform_charset_rsp(&p_rsp->inform_charset, p_pkt);
        break;

    case AVRC_PDU_INFORM_BATTERY_STAT_OF_CT:
        status = avrc_bld_inform_battery_status_rsp(&p_rsp->inform_battery_status, p_pkt);
        break;

    case AVRC_PDU_GET_ELEMENT_ATTR:
        status = avrc_bld_get_elem_attrs_rsp(&p_rsp->get_elem_attrs, p_pkt);
        break;

    case AVRC_PDU_GET_PLAY_STATUS:
        status = avrc_bld_get_play_status_rsp(&p_rsp->get_play_status, p_pkt);
        break;

    case AVRC_PDU_REGISTER_NOTIFICATION:
        status = avrc_bld_notify_rsp(&p_rsp->reg_notif, p_pkt);
        break;

    case AVRC_PDU_REQUEST_CONTINUATION_RSP:     /*        0x40 */
        status = avrc_bld_next_rsp(&p_rsp->continu, p_pkt);
        break;

    case AVRC_PDU_ABORT_CONTINUATION_RSP:       /*          0x41 */
        status = avrc_bld_next_rsp(&p_rsp->abort, p_pkt);
        break;
    }

    if (alloc && (status != AVRC_STS_NO_ERROR) ) {
        osi_free(p_pkt);
        *pp_pkt = NULL;
    }
    AVRC_TRACE_API("AVRC_BldResponse: returning %d", status);
    return status;
}

#endif /* (AVRC_METADATA_INCLUDED == TRUE)*/

#endif /* #if (defined(AVRC_INCLUDED) && AVRC_INCLUDED == TRUE) */
