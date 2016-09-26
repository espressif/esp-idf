/******************************************************************************
 *
 *  Copyright (C) 2010-2012 Broadcom Corporation
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
 *  This is the implementation file for the GATT call-in functions.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE)

#include <string.h>

#include "bta_api.h"
#include "bta_sys.h"
#include "bta_gattc_ci.h"
#include "gki.h"
#include "utl.h"

/*******************************************************************************
**
** Function         bta_gattc_ci_cache_open
**
** Description      This function sends an event to indicate server cache open
**                  completed.
**
** Parameters       server_bda - server BDA of this cache.
**                  status - BTA_GATT_OK if full buffer of data,
**                           BTA_GATT_FAIL if an error has occurred.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_ci_cache_open(BD_ADDR server_bda, UINT16 evt, tBTA_GATT_STATUS status,
                             UINT16 conn_id)
{
    tBTA_GATTC_CI_EVT  *p_evt;
    UNUSED(server_bda);

    if ((p_evt = (tBTA_GATTC_CI_EVT *) GKI_getbuf(sizeof(tBTA_GATTC_CI_EVT))) != NULL)
    {
        p_evt->hdr.event = evt;
        p_evt->hdr.layer_specific = conn_id;

        p_evt->status = status;
        bta_sys_sendmsg(p_evt);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_ci_cache_load
**
** Description      This function sends an event to BTA indicating the phone has
**                  load the servere cache and ready to send it to the stack.
**
** Parameters       server_bda - server BDA of this cache.
**                  num_bytes_read - number of bytes read into the buffer
**                      specified in the read callout-function.
**                  status - BTA_GATT_OK if full buffer of data,
**                           BTA_GATT_FAIL if an error has occurred.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_ci_cache_load(BD_ADDR server_bda, UINT16 evt, UINT16 num_attr,
                             tBTA_GATTC_NV_ATTR *p_attr, tBTA_GATT_STATUS status,
                             UINT16 conn_id)
{
    tBTA_GATTC_CI_LOAD  *p_evt;
    UNUSED(server_bda);

    if ((p_evt = (tBTA_GATTC_CI_LOAD *) GKI_getbuf(sizeof(tBTA_GATTC_CI_LOAD))) != NULL)
    {
        memset(p_evt, 0, sizeof(tBTA_GATTC_CI_LOAD));

        p_evt->hdr.event = evt;
        p_evt->hdr.layer_specific = conn_id;

        p_evt->status    = status;
        p_evt->num_attr  = (num_attr > BTA_GATTC_NV_LOAD_MAX) ? BTA_GATTC_NV_LOAD_MAX : num_attr;

        if (p_evt->num_attr > 0 && p_attr != NULL)
        {
            memcpy(p_evt->attr, p_attr, p_evt->num_attr * sizeof(tBTA_GATTC_NV_ATTR));
        }

        bta_sys_sendmsg(p_evt);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_ci_cache_save
**
** Description      This function sends an event to BTA indicating the phone has
**                  save the servere cache.
**
** Parameters       server_bda - server BDA of this cache.
**                  evt - callin event code.
**                  status - BTA_GATT_OK if full buffer of data,
**                           BTA_GATT_ERROR if an error has occurred.
*8                  conn_id - for this NV operation for.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_ci_cache_save(BD_ADDR server_bda, UINT16 evt, tBTA_GATT_STATUS status,
                             UINT16 conn_id)
{
    tBTA_GATTC_CI_EVT  *p_evt;
    UNUSED(server_bda);

    if ((p_evt = (tBTA_GATTC_CI_EVT *) GKI_getbuf(sizeof(tBTA_GATTC_CI_EVT))) != NULL)
    {
        p_evt->hdr.event = evt;
        p_evt->hdr.layer_specific = conn_id;

        p_evt->status = status;
        bta_sys_sendmsg(p_evt);
    }
}
#endif /* BTA_GATT_INCLUDED */
