/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This file contains the GATT client utility function.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(GATTS_INCLUDED) && (GATTS_INCLUDED == TRUE)

#include <string.h>
#include "utl.h"
#include "gki.h"
#include "bta_sys.h"
#include "bta_gatts_int.h"

static const UINT8  base_uuid[LEN_UUID_128] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
                                               0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                              };

/*******************************************************************************
**
** Function         bta_gatt_convert_uuid16_to_uuid128
**
** Description      Convert a 16 bits UUID to be an standard 128 bits one.
**
** Returns          TRUE if two uuid match; FALSE otherwise.
**
*******************************************************************************/
static void bta_gatt_convert_uuid16_to_uuid128(UINT8 uuid_128[LEN_UUID_128], UINT16 uuid_16)
{
    UINT8   *p = &uuid_128[LEN_UUID_128 - 4];

    memcpy (uuid_128, base_uuid, LEN_UUID_128);

    UINT16_TO_STREAM(p, uuid_16);
}
/*******************************************************************************
**
** Function         bta_gatts_alloc_srvc_cb
**
** Description      allocate a service control block.
**
** Returns          pointer to the control block, or otherwise NULL when failed.
**
*******************************************************************************/
UINT8 bta_gatts_alloc_srvc_cb(tBTA_GATTS_CB *p_cb, UINT8 rcb_idx)
{
    UINT8 i;

    for (i = 0; i < BTA_GATTS_MAX_SRVC_NUM; i ++) {
        if (!p_cb->srvc_cb[i].in_use) {
            p_cb->srvc_cb[i].in_use = TRUE;
            p_cb->srvc_cb[i].rcb_idx = rcb_idx;
            return i;
        }
    }
    return BTA_GATTS_INVALID_APP;
}

/*******************************************************************************
**
** Function         bta_gatts_find_app_rcb_by_app_if
**
** Description      find the index of the application control block by app ID.
**
** Returns          pointer to the control block if success, otherwise NULL
**
*******************************************************************************/
tBTA_GATTS_RCB *bta_gatts_find_app_rcb_by_app_if(tBTA_GATTS_IF server_if)
{
    UINT8 i;
    tBTA_GATTS_RCB *p_reg;

    for (i = 0, p_reg = bta_gatts_cb.rcb; i < BTA_GATTS_MAX_APP_NUM; i ++, p_reg++) {
        if (p_reg->in_use && p_reg->gatt_if == server_if) {
            return p_reg;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         bta_gatts_find_app_rcb_idx_by_app_if
**
** Description      find the index of the application control block by app ID.
**
** Returns          index of the control block, or BTA_GATTS_INVALID_APP if failed.
**
*******************************************************************************/

UINT8 bta_gatts_find_app_rcb_idx_by_app_if(tBTA_GATTS_CB *p_cb, tBTA_GATTS_IF server_if)
{
    UINT8 i;

    for (i = 0; i < BTA_GATTS_MAX_APP_NUM; i ++) {
        if (p_cb->rcb[i].in_use && p_cb->rcb[i].gatt_if == server_if) {
            return i;
        }
    }
    return BTA_GATTS_INVALID_APP;
}
/*******************************************************************************
**
** Function         bta_gatts_find_srvc_cb_by_srvc_id
**
** Description      find the service control block by service ID.
**
** Returns          pointer to the rcb.
**
*******************************************************************************/
tBTA_GATTS_SRVC_CB *bta_gatts_find_srvc_cb_by_srvc_id(tBTA_GATTS_CB *p_cb, UINT16 service_id)
{
    UINT8 i;
    APPL_TRACE_DEBUG("bta_gatts_find_srvc_cb_by_srvc_id  service_id=%d", service_id);
    for (i = 0; i < BTA_GATTS_MAX_SRVC_NUM; i ++) {
        if (p_cb->srvc_cb[i].in_use &&
                p_cb->srvc_cb[i].service_id == service_id) {
            APPL_TRACE_DEBUG("bta_gatts_find_srvc_cb_by_srvc_id  found service cb index =%d", i);
            return &p_cb->srvc_cb[i];
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         bta_gatts_find_srvc_cb_by_attr_id
**
** Description      find the service control block by attribute ID.
**
** Returns          pointer to the rcb.
**
*******************************************************************************/
tBTA_GATTS_SRVC_CB *bta_gatts_find_srvc_cb_by_attr_id(tBTA_GATTS_CB *p_cb, UINT16 attr_id)
{
    UINT8 i;

    for (i = 0; i < (BTA_GATTS_MAX_SRVC_NUM); i ++) {
        if (/* middle service */
            (i < (BTA_GATTS_MAX_SRVC_NUM - 1) &&
             p_cb->srvc_cb[i].in_use &&
             p_cb->srvc_cb[i + 1].in_use &&
             attr_id >= p_cb->srvc_cb[i].service_id &&
             attr_id < p_cb->srvc_cb[i + 1].service_id) ||
            /* last active service */
            (i < (BTA_GATTS_MAX_SRVC_NUM - 1) &&
             p_cb->srvc_cb[i].in_use &&
             !p_cb->srvc_cb[i + 1].in_use &&
             attr_id >= p_cb->srvc_cb[i].service_id)    ||
            /* last service incb */
            (i == (BTA_GATTS_MAX_SRVC_NUM - 1) &&
             attr_id >= p_cb->srvc_cb[i].service_id)
        ) {
            return &p_cb->srvc_cb[i];
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         bta_gatts_uuid_compare
**
** Description      Compare two UUID to see if they are the same.
**
** Returns          TRUE if two uuid match; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_gatts_uuid_compare(tBT_UUID tar, tBT_UUID src)
{
    UINT8  su[LEN_UUID_128], tu[LEN_UUID_128];
    UINT8  *ps, *pt;

    /* any of the UUID is unspecified */
    if (src.len == 0 || tar.len == 0) {
        return TRUE;
    }

    /* If both are 16-bit, we can do a simple compare */
    if (src.len == 2 && tar.len == 2) {
        return src.uu.uuid16 == tar.uu.uuid16;
    }

    /* One or both of the UUIDs is 128-bit */
    if (src.len == LEN_UUID_16) {
        /* convert a 16 bits UUID to 128 bits value */
        bta_gatt_convert_uuid16_to_uuid128(su, src.uu.uuid16);
        ps = su;
    } else {
        ps = src.uu.uuid128;
    }

    if (tar.len == LEN_UUID_16) {
        /* convert a 16 bits UUID to 128 bits value */
        bta_gatt_convert_uuid16_to_uuid128(tu, tar.uu.uuid16);
        pt = tu;
    } else {
        pt = tar.uu.uuid128;
    }

    return (memcmp(ps, pt, LEN_UUID_128) == 0);
}




#endif  /* GATTS_INCLUDED */