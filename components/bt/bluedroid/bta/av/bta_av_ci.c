/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
 *  This is the implementation file for advanced audio/video call-in
 *  functions.
 *
 ******************************************************************************/

#include "bt_target.h"
#include "bta_api.h"
#include "bta_sys.h"
#include "bta_av_int.h"
#include "bta_av_ci.h"

#include <string.h>

#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         bta_av_ci_src_data_ready
**
** Description      This function sends an event to the AV indicating that
**                  the phone has audio stream data ready to send and AV
**                  should call bta_av_co_audio_src_data_path() or
**                  bta_av_co_video_src_data_path().
**
** Returns          void
**
*******************************************************************************/
void bta_av_ci_src_data_ready(tBTA_AV_CHNL chnl)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) GKI_getbuf(sizeof(BT_HDR))) != NULL) {
        p_buf->layer_specific   = chnl;
        p_buf->event = BTA_AV_CI_SRC_DATA_READY_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_av_ci_setconfig
**
** Description      This function must be called in response to function
**                  bta_av_co_audio_setconfig() or bta_av_co_video_setconfig.
**                  Parameter err_code is set to an AVDTP status value;
**                  AVDT_SUCCESS if the codec configuration is ok,
**                  otherwise error.
**
** Returns          void
**
*******************************************************************************/
void bta_av_ci_setconfig(tBTA_AV_HNDL hndl, UINT8 err_code, UINT8 category,
                         UINT8 num_seid, UINT8 *p_seid, BOOLEAN recfg_needed, UINT8 avdt_handle)
{
    tBTA_AV_CI_SETCONFIG  *p_buf;

    if ((p_buf = (tBTA_AV_CI_SETCONFIG *) GKI_getbuf(sizeof(tBTA_AV_CI_SETCONFIG))) != NULL) {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event = (err_code == AVDT_SUCCESS) ?
                           BTA_AV_CI_SETCONFIG_OK_EVT : BTA_AV_CI_SETCONFIG_FAIL_EVT;
        p_buf->err_code = err_code;
        p_buf->category = category;
        p_buf->recfg_needed = recfg_needed;
        p_buf->num_seid = num_seid;
        p_buf->avdt_handle = avdt_handle;
        if (p_seid && num_seid) {
            p_buf->p_seid   = (UINT8 *)(p_buf + 1);
            memcpy(p_buf->p_seid, p_seid, num_seid);
        } else {
            p_buf->p_seid   = NULL;
            p_buf->num_seid = 0;
        }

        bta_sys_sendmsg(p_buf);
    }
}

#endif /* #if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE) */
