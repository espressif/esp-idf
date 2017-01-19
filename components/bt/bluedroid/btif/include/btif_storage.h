/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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
#ifndef __BTIF_STORAGE_H__
#define __BTIF_STORAGE_H__

#include <stdint.h>
#include "bt_defs.h"
#include "bt_types.h"
/*******************************************************************************
**
** Function         btif_storage_add_bonded_device
**
** Description      BTIF storage API - Adds the newly bonded device to NVRAM
**                  along with the link-key, Key type and Pin key length
**
** Returns          BT_STATUS_SUCCESS if the store was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_storage_add_bonded_device(bt_bdaddr_t *remote_bd_addr,
        LINK_KEY link_key,
        uint8_t key_type,
        uint8_t pin_length);

/*******************************************************************************
**
** Function         btif_storage_remove_bonded_device
**
** Description      BTIF storage API - Deletes the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if the deletion was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_storage_remove_bonded_device(bt_bdaddr_t *remote_bd_addr);

/*******************************************************************************
**
** Function         btif_storage_remove_bonded_device
**
** Description      BTIF storage API - Deletes the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if the deletion was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_storage_load_bonded_devices(void);



#endif /* BTIF_STORAGE_H */
