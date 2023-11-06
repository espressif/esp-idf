/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BTC_STORAGE_H__
#define __BTC_STORAGE_H__

#include <stdint.h>
#include "common/bt_defs.h"
#include "stack/bt_types.h"
#include "esp_gap_bt_api.h"


#define BTC_STORAGE_DEV_CLASS_STR       "DevClass"
#define BTC_STORAGE_LINK_KEY_STR        "LinkKey"    /* same as the ble */
#define BTC_STORAGE_LINK_KEY_TYPE_STR   "LinkKeyType"
#define BTC_STORAGE_PIN_LENGTH_STR      "PinLength"
#define BTC_STORAGE_SC_SUPPORT          "SCSupport"

/*******************************************************************************
**
** Function         btc_storage_add_bonded_device
**
** Description      BTC storage API - Adds the newly bonded device to NVRAM
**                  along with the link-key, Key type and Pin key length
**
** Returns          BT_STATUS_SUCCESS if the store was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_add_bonded_device(bt_bdaddr_t *remote_bd_addr,
        LINK_KEY link_key,
        uint8_t key_type,
        uint8_t pin_length,
        BOOLEAN sc_support);

/*******************************************************************************
**
** Function         btc_storage_remove_bonded_device
**
** Description      BTC storage API - Deletes the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if the deletion was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_remove_bonded_device(bt_bdaddr_t *remote_bd_addr);

/*******************************************************************************
**
** Function         btc_storage_load_bonded_devices
**
** Description      BTC storage API - Loads all the bonded devices from NVRAM
**                  and adds to the BTA.
**                  Additionally, this API also invokes the adaper_properties_cb
**                  and remote_device_properties_cb for each of the bonded devices.
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_load_bonded_devices(void);

/*******************************************************************************
**
** Function         btc_storage_get_num_bt_bond_devices
**
** Description      BTC storage API - get the num of the bonded device from NVRAM
**
** Returns          the num of the bonded device
**
*******************************************************************************/
int btc_storage_get_num_bt_bond_devices(void);

/*******************************************************************************
**
** Function         btc_storage_get_bonded_bt_devices_list
**
** Description      BTC storage API - get the list of the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if get the list successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_get_bonded_bt_devices_list(bt_bdaddr_t *bond_dev, int *dev_num);

/*******************************************************************************
**
** Function         btc_storage_get_num_all_bond_devices
**
** Description      BTC storage API - get all the num of the bonded device from NVRAM
**
** Returns          the num of the bonded device
**
*******************************************************************************/
int btc_storage_get_num_all_bond_devices(void);

/*******************************************************************************
**
** Function         btc_storage_update_active_device
**
** Description      BTC storage API - Once an ACL link is established and remote
**                  bd_addr is already stored in NVRAM, update the config and update
**                  the remote device to be the newest active device. The updates will
**                  not be stored into NVRAM immediately.
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bool btc_storage_update_active_device(bt_bdaddr_t *remote_bd_addr);

#endif /* BTC_STORAGE_H */
