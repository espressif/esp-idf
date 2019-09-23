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

/******************************************************************************
 *
 *  This is the interface file for BTA GATT client call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_GATTC_CO_H
#define BTA_GATTC_CO_H

#include "bta/bta_gatt_api.h"
#include "osi/hash_functions.h"

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
extern tBTA_GATT_STATUS bta_gattc_co_cache_open(BD_ADDR server_bda, BOOLEAN to_save, UINT8 *index);

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
extern void bta_gattc_co_cache_close(BD_ADDR server_bda, UINT16 conn_id);

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
extern void bta_gattc_co_cache_save (BD_ADDR server_bda, UINT16 num_attr,
                              tBTA_GATTC_NV_ATTR *p_attr_list);

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
extern tBTA_GATT_STATUS bta_gattc_co_cache_load(tBTA_GATTC_NV_ATTR *attr, UINT8 index);

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
extern void bta_gattc_co_cache_reset(BD_ADDR server_bda);

extern size_t bta_gattc_get_cache_attr_length(UINT8 index);

extern void bta_gattc_co_cache_addr_init(void);

extern void bta_gattc_co_cache_addr_deinit(void);

extern BOOLEAN bta_gattc_co_addr_in_cache(BD_ADDR bda);

extern uint8_t bta_gattc_co_find_addr_in_cache(BD_ADDR bda);

extern uint8_t bta_gattc_co_find_hash_in_cache(hash_key_t hash_key);

extern UINT8 bta_gattc_co_get_addr_num(void);

extern void bta_gattc_co_get_addr_list(BD_ADDR *addr_list);

extern void bta_gattc_co_cache_addr_save(BD_ADDR bd_addr, hash_key_t hash_key);

extern BOOLEAN bta_gattc_co_cache_new_assoc_list(BD_ADDR src_addr, uint8_t index);

extern BOOLEAN bta_gattc_co_cache_append_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr);

extern BOOLEAN bta_gattc_co_cache_remove_assoc_addr(BD_ADDR src_addr, BD_ADDR assoc_addr);

uint8_t* bta_gattc_co_cache_find_src_addr(BD_ADDR assoc_addr, uint8_t *index);

extern BOOLEAN bta_gattc_co_cache_clear_assoc_addr(BD_ADDR src_addr);

#endif /* BTA_GATT_CO_H */
