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
 *  This is the implementation for the audio/video registration module.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include <string.h>
#include "bta/bta_ar_api.h"
#include "bta_ar_int.h"

#if BTA_AR_INCLUDED

/* AV control block */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_AR_CB  bta_ar_cb;
#else
tBTA_AR_CB  *bta_ar_cb_ptr;
#endif

/*******************************************************************************
**
** Function         bta_ar_id
**
** Description      This function maps sys_id to ar id mask.
**
** Returns          void
**
*******************************************************************************/
static UINT8 bta_ar_id(tBTA_SYS_ID sys_id)
{
    UINT8   mask = 0;
    if (sys_id == BTA_ID_AV) {
        mask = BTA_AR_AV_MASK;
    } else if (sys_id == BTA_ID_AVK) {
        mask = BTA_AR_AVK_MASK;
    }

    return mask;
}

/*******************************************************************************
**
** Function         bta_ar_init
**
** Description      This function is called to register to AVDTP.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_init(void)
{
    /* initialize control block */
    memset(&bta_ar_cb, 0, sizeof(tBTA_AR_CB));
}

/*******************************************************************************
**
** Function         bta_ar_reg_avdt
**
** Description      This function is called to register to AVDTP.
**
** Returns          void
**
*******************************************************************************/
static void bta_ar_avdt_cback(UINT8 handle, BD_ADDR bd_addr, UINT8 event, tAVDT_CTRL *p_data)
{
    /* route the AVDT registration callback to av or avk */
    if (bta_ar_cb.p_av_conn_cback) {
        (*bta_ar_cb.p_av_conn_cback)(handle, bd_addr, event, p_data);
    }
    if (bta_ar_cb.p_avk_conn_cback) {
        (*bta_ar_cb.p_avk_conn_cback)(handle, bd_addr, event, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_ar_reg_avdt
**
** Description      AR module registration to AVDT.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_reg_avdt(tAVDT_REG *p_reg, tAVDT_CTRL_CBACK *p_cback, tBTA_SYS_ID sys_id)
{
    UINT8   mask = 0;

    if (sys_id == BTA_ID_AV) {
        bta_ar_cb.p_av_conn_cback = p_cback;
        mask = BTA_AR_AV_MASK;
    } else if (sys_id == BTA_ID_AVK) {
        bta_ar_cb.p_avk_conn_cback = p_cback;
        mask = BTA_AR_AVK_MASK;
    }
#if (BTA_AR_DEBUG == TRUE)
    else {
        APPL_TRACE_ERROR("bta_ar_reg_avdt: the registration is from wrong sys_id:%d", sys_id);
    }
#endif

    if (mask) {
        if (bta_ar_cb.avdt_registered == 0) {
            AVDT_Register(p_reg, bta_ar_avdt_cback);
        }
        bta_ar_cb.avdt_registered |= mask;
    }
}

/*******************************************************************************
**
** Function         bta_ar_dereg_avdt
**
** Description      This function is called to de-register from AVDTP.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_dereg_avdt(tBTA_SYS_ID sys_id)
{
    UINT8   mask = 0;

    if (sys_id == BTA_ID_AV) {
        bta_ar_cb.p_av_conn_cback = NULL;
        mask = BTA_AR_AV_MASK;
    } else if (sys_id == BTA_ID_AVK) {
        bta_ar_cb.p_avk_conn_cback = NULL;
        mask = BTA_AR_AVK_MASK;
    }
    bta_ar_cb.avdt_registered &= ~mask;

    if (bta_ar_cb.avdt_registered == 0) {
        AVDT_Deregister();
    }
}

/*******************************************************************************
**
** Function         bta_ar_avdt_conn
**
** Description      This function is called to let ar know that some AVDTP profile
**                  is connected for this sys_id.
**                  If the other sys modules started a timer for PENDING_EVT,
**                  the timer can be stopped now.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_avdt_conn(tBTA_SYS_ID sys_id, BD_ADDR bd_addr)
{
    UINT8       event = BTA_AR_AVDT_CONN_EVT;
    tAVDT_CTRL  data;

    if (sys_id == BTA_ID_AV) {
        if (bta_ar_cb.p_avk_conn_cback) {
            (*bta_ar_cb.p_avk_conn_cback)(0, bd_addr, event, &data);
        }
    } else if (sys_id == BTA_ID_AVK) {
        if (bta_ar_cb.p_av_conn_cback) {
            (*bta_ar_cb.p_av_conn_cback)(0, bd_addr, event, &data);
        }
    }
}

/*******************************************************************************
**
** Function         bta_ar_reg_avct
**
** Description      This function is called to register to AVCTP.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_reg_avct(UINT16 mtu, UINT16 mtu_br, UINT8 sec_mask, tBTA_SYS_ID sys_id)
{
    UINT8   mask = bta_ar_id (sys_id);

    if (mask) {
        if (bta_ar_cb.avct_registered == 0) {
            AVCT_Register(mtu, mtu_br, sec_mask);
        }
        bta_ar_cb.avct_registered |= mask;
    }
}

/*******************************************************************************
**
** Function         bta_ar_dereg_avct
**
** Description      This function is called to deregister from AVCTP.
**
** Returns          void
**
*******************************************************************************/
void bta_ar_dereg_avct(tBTA_SYS_ID sys_id)
{
    UINT8   mask = bta_ar_id (sys_id);

    bta_ar_cb.avct_registered &= ~mask;

    if (bta_ar_cb.avct_registered == 0) {
        AVCT_Deregister();
    }
}

/******************************************************************************
**
** Function         bta_ar_reg_avrc
**
** Description      This function is called to register an SDP record for AVRCP.
**
** Returns          void
**
******************************************************************************/
void bta_ar_reg_avrc(UINT16 service_uuid, char *service_name, char *provider_name,
                     UINT16 categories, tBTA_SYS_ID sys_id, BOOLEAN browsing_en)
{
    UINT8   mask = bta_ar_id (sys_id);
    UINT8   temp[8], *p;

    if (!mask || !categories) {
        return;
    }

    if (service_uuid == UUID_SERVCLASS_AV_REM_CTRL_TARGET) {
        if (bta_ar_cb.sdp_tg_handle == 0) {
            bta_ar_cb.tg_registered = mask;
            bta_ar_cb.sdp_tg_handle = SDP_CreateRecord();
            AVRC_AddRecord(service_uuid, service_name, provider_name, categories, bta_ar_cb.sdp_tg_handle, browsing_en);
            bta_sys_add_uuid(service_uuid);
        }
        /* only one TG is allowed (first-come, first-served).
         * If sdp_tg_handle is non-0, ignore this request */
    } else if ((service_uuid == UUID_SERVCLASS_AV_REMOTE_CONTROL) || (service_uuid == UUID_SERVCLASS_AV_REM_CTRL_CONTROL)) {
        bta_ar_cb.ct_categories [mask - 1] = categories;
        categories = bta_ar_cb.ct_categories[0] | bta_ar_cb.ct_categories[1];
        if (bta_ar_cb.sdp_ct_handle == 0) {
            bta_ar_cb.sdp_ct_handle = SDP_CreateRecord();
            AVRC_AddRecord(service_uuid, service_name, provider_name, categories, bta_ar_cb.sdp_ct_handle, browsing_en);
            bta_sys_add_uuid(service_uuid);
        } else {
            /* multiple CTs are allowed.
             * Change supported categories on the second one */
            p = temp;
            UINT16_TO_BE_STREAM(p, categories);
            SDP_AddAttribute(bta_ar_cb.sdp_ct_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE,
                             (UINT32)2, (UINT8 *)temp);
        }
    }
}

/******************************************************************************
**
** Function         bta_ar_dereg_avrc
**
** Description      This function is called to de-register/delete an SDP record for AVRCP.
**
** Returns          void
**
******************************************************************************/
void bta_ar_dereg_avrc(UINT16 service_uuid, tBTA_SYS_ID sys_id)
{
    UINT8   mask = bta_ar_id (sys_id);
    UINT16  categories = 0;
    UINT8   temp[8], *p;

    if (!mask) {
        return;
    }

    if (service_uuid == UUID_SERVCLASS_AV_REM_CTRL_TARGET) {
        if (bta_ar_cb.sdp_tg_handle && mask == bta_ar_cb.tg_registered) {
            bta_ar_cb.tg_registered = 0;
            SDP_DeleteRecord(bta_ar_cb.sdp_tg_handle);
            bta_ar_cb.sdp_tg_handle = 0;
            bta_sys_remove_uuid(service_uuid);
        }
    } else if (service_uuid == UUID_SERVCLASS_AV_REMOTE_CONTROL) {
        if (bta_ar_cb.sdp_ct_handle) {
            bta_ar_cb.ct_categories [mask - 1] = 0;
            categories = bta_ar_cb.ct_categories[0] | bta_ar_cb.ct_categories[1];
            if (!categories) {
                /* no CT is still registered - cleaup */
                SDP_DeleteRecord(bta_ar_cb.sdp_ct_handle);
                bta_ar_cb.sdp_ct_handle = 0;
                bta_sys_remove_uuid(service_uuid);
            } else {
                /* change supported categories to the remaning one */
                p = temp;
                UINT16_TO_BE_STREAM(p, categories);
                SDP_AddAttribute(bta_ar_cb.sdp_ct_handle, ATTR_ID_SUPPORTED_FEATURES, UINT_DESC_TYPE,
                                 (UINT32)2, (UINT8 *)temp);
            }
        }
    }

}

#endif /* #if BTA_AR_INCLUDED */
