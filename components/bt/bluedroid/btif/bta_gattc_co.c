/******************************************************************************
 *
 *  Copyright (C) 2009-2013 Broadcom Corporation
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
#ifdef  BT_SUPPORT_NVM
#include <stdio.h>
#include <unistd.h>
#endif /* BT_SUPPORT_NVM */

#include "bta_gattc_co.h"
#include "bta_gattc_ci.h"
// #include "btif_util.h"
#include "btm_int.h"

#if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE)
#if( defined BTA_GATT_INCLUDED ) && (GATTC_INCLUDED == TRUE)

#define GATT_CACHE_PREFIX "/data/misc/bluedroid/gatt_cache_"

#ifdef BT_SUPPORT_NVM
static FILE *sCacheFD = 0;

static void getFilename(char *buffer, BD_ADDR bda)
{
    sprintf(buffer, "%s%02x%02x%02x%02x%02x%02x", GATT_CACHE_PREFIX
            , bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
}

static void cacheClose()
{
    if (sCacheFD != 0) {
        fclose(sCacheFD);
        sCacheFD = 0;
    }
}

static bool cacheOpen(BD_ADDR bda, bool to_save)
{
    char fname[255] = {0};
    getFilename(fname, bda);

    cacheClose();
    sCacheFD = fopen(fname, to_save ? "w" : "r");

    return (sCacheFD != 0);
}

static void cacheReset(BD_ADDR bda)
{
    char fname[255] = {0};
    getFilename(fname, bda);
    unlink(fname);
}

#endif /* BT_SUPPORT_NVM */
/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         bta_gattc_co_cache_open
**
** Description      This callout function is executed by GATTC when a GATT server
**                  cache is ready to be sent.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache open is done.
**                  conn_id: connection ID of this cache operation attach to.
**                  to_save: open cache to save or to load.
**
** Returns          void.
**
*******************************************************************************/
void bta_gattc_co_cache_open(BD_ADDR server_bda, UINT16 evt, UINT16 conn_id, BOOLEAN to_save)
{
#ifdef BT_SUPPORT_NVM
    /* open NV cache and send call in */
    tBTA_GATT_STATUS    status = BTA_GATT_OK;
    if (!btm_sec_is_a_bonded_dev(server_bda) || !cacheOpen(server_bda, to_save)) {
        status = BTA_GATT_ERROR;
    }

    BTIF_TRACE_DEBUG("%s() - status=%d", __FUNCTION__, status);
    bta_gattc_ci_cache_open(server_bda, evt, status, conn_id);
#else
    bta_gattc_ci_cache_open(server_bda, evt, BTA_GATT_ERROR, conn_id);
#endif /* BT_SUPPORT_NVM */
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_load
**
** Description      This callout function is executed by GATT when server cache
**                  is required to load.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache save is done.
**                  num_attr: number of attribute to be save.
**                  attr_index: starting attribute index of the save operation.
**                  conn_id: connection ID of this cache operation attach to.
** Returns
**
*******************************************************************************/
void bta_gattc_co_cache_load(BD_ADDR server_bda, UINT16 evt, UINT16 start_index, UINT16 conn_id)
{
    UINT16              num_attr = 0;
    tBTA_GATTC_NV_ATTR  attr[BTA_GATTC_NV_LOAD_MAX];
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
#ifdef BT_SUPPORT_NVM
    if (sCacheFD && (0 == fseek(sCacheFD, start_index * sizeof(tBTA_GATTC_NV_ATTR), SEEK_SET))) {
        num_attr = fread(attr, sizeof(tBTA_GATTC_NV_ATTR), BTA_GATTC_NV_LOAD_MAX, sCacheFD);
        status = (num_attr < BTA_GATTC_NV_LOAD_MAX ? BTA_GATT_OK : BTA_GATT_MORE);
    }
    BTIF_TRACE_DEBUG("%s() - sCacheFD=%p, start_index=%d, read=%d, status=%d",
                     __FUNCTION__, sCacheFD, start_index, num_attr, status);
#endif /* BT_SUPPORT_NVM */

    bta_gattc_ci_cache_load(server_bda, evt, num_attr, attr, status, conn_id);
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_save
**
** Description      This callout function is executed by GATT when a server cache
**                  is available to save.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  evt: call in event to be passed in when cache save is done.
**                  num_attr: number of attribute to be save.
**                  p_attr: pointer to the list of attributes to save.
**                  attr_index: starting attribute index of the save operation.
**                  conn_id: connection ID of this cache operation attach to.
** Returns
**
*******************************************************************************/
void bta_gattc_co_cache_save (BD_ADDR server_bda, UINT16 evt, UINT16 num_attr,
                              tBTA_GATTC_NV_ATTR *p_attr_list, UINT16 attr_index, UINT16 conn_id)
{
    tBTA_GATT_STATUS    status = BTA_GATT_OK;
    UNUSED(attr_index);
#ifdef BT_SUPPORT_NVM
    if (sCacheFD != 0) {
        int num = fwrite(p_attr_list, sizeof(tBTA_GATTC_NV_ATTR), num_attr, sCacheFD);
        BTIF_TRACE_DEBUG("%s() wrote %d", __FUNCTION__, num);
    }
#endif /* BT_SUPPORT_NVM */
    bta_gattc_ci_cache_save(server_bda, evt, status, conn_id);
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_close
**
** Description      This callout function is executed by GATTC when a GATT server
**                  cache is written completely.
**
** Parameter        server_bda: server bd address of this cache belongs to
**                  conn_id: connection ID of this cache operation attach to.
**
** Returns          void.
**
*******************************************************************************/
void bta_gattc_co_cache_close(BD_ADDR server_bda, UINT16 conn_id)
{
    UNUSED(server_bda);
    UNUSED(conn_id);
#ifdef BT_SUPPORT_NVM
    cacheClose();
#endif /* BT_SUPPORT_NVM */
    /* close NV when server cache is done saving or loading,
       does not need to do anything for now on Insight */

    BTIF_TRACE_DEBUG("%s()", __FUNCTION__);
}

/*******************************************************************************
**
** Function         bta_gattc_co_cache_reset
**
** Description      This callout function is executed by GATTC to reset cache in
**                  application
**
** Parameter        server_bda: server bd address of this cache belongs to
**
** Returns          void.
**
*******************************************************************************/
void bta_gattc_co_cache_reset(BD_ADDR server_bda)
{
    BTIF_TRACE_DEBUG("%s()", __FUNCTION__);
#ifdef BT_SUPPORT_NVM
    cacheReset(server_bda);
#endif /* BT_SUPPORT_NVM */
}

#endif /* #if( defined BLE_INCLUDED ) && (BLE_INCLUDED == TRUE) */
#endif /* #if( defined BTA_GATT_INCLUDED ) && (BTA_GATT_INCLUDED == TRUE) */

