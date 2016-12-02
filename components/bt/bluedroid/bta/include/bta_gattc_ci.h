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
 *  This is the interface file for GATT call-in functions.
 *
 ******************************************************************************/
#ifndef BTA_GATTC_CI_H
#define BTA_GATTC_CI_H

#include "bta_gatt_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Open Complete Event */
typedef struct {
    BT_HDR            hdr;
    tBTA_GATT_STATUS  status;
} tBTA_GATTC_CI_EVT;

#define BTA_GATTC_NV_LOAD_MAX   10

/* Read Ready Event */
typedef struct {
    BT_HDR              hdr;
    tBTA_GATT_STATUS    status;
    UINT16              num_attr;
    tBTA_GATTC_NV_ATTR  attr[BTA_GATTC_NV_LOAD_MAX];
} tBTA_GATTC_CI_LOAD;


/*****************************************************************************
**  Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

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
extern void bta_gattc_ci_cache_open(BD_ADDR server_bda, UINT16 evt,
                                    tBTA_GATT_STATUS status, UINT16 conn_id);

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
extern void bta_gattc_ci_cache_load(BD_ADDR server_bda, UINT16 evt,
                                    UINT16 num_attr, tBTA_GATTC_NV_ATTR *p_atrr,
                                    tBTA_GATT_STATUS status, UINT16 conn_id);

/*******************************************************************************
**
** Function         bta_gattc_ci_save
**
** Description      This function sends an event to BTA indicating the phone has
**                  save the server cache.
**
** Parameters       server_bda - server BDA of this cache.
**                  status - BTA_GATT_OK if full buffer of data,
**                           BTA_GATT_FAIL if an error has occurred.
**
** Returns          void
**
*******************************************************************************/
extern void bta_gattc_ci_cache_save(BD_ADDR server_bda, UINT16 evt,
                                    tBTA_GATT_STATUS status, UINT16 conn_id);


#ifdef __cplusplus
}
#endif

#endif /* BTA_GATTC_CI_H */
