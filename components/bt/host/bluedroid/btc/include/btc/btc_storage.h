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

#ifdef __cplusplus
extern "C" {
#endif

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

#if (defined BTC_HH_INCLUDED && BTC_HH_INCLUDED == TRUE)
/*******************************************************************************
 *
 * Function         btc_storage_add_hid_device_info
 *
 * Description      BTC storage API - Adds the hid information of bonded hid
 *                  devices-to NVRAM
 *
 * Returns          BT_STATUS_SUCCESS if the store was successful,
 *                  BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/

bt_status_t btc_storage_add_hid_device_info(bt_bdaddr_t *remote_bd_addr, uint16_t attr_mask, uint8_t sub_class,
                                             uint8_t app_id, uint16_t vendor_id, uint16_t product_id, uint16_t version,
                                             uint8_t ctry_code, uint16_t ssr_max_latency, uint16_t ssr_min_tout,
                                             uint16_t dl_len, uint8_t *dsc_list);

/*******************************************************************************
 *
 * Function         btc_storage_load_bonded_hid_info
 *
 * Description      BTIF storage API - Loads hid info for all the bonded devices
 *                  from NVRAM and adds those devices  to the BTA_HH.
 *
 * Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/
bt_status_t btc_storage_load_bonded_hid_info(void);

/*******************************************************************************
 *
 * Function         btc_storage_remove_hid_info
 *
 * Description      BTC storage API - Deletes the bonded hid device info from
 *                  NVRAM
 *
 * Returns          BT_STATUS_SUCCESS if the deletion was successful,
 *                  BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/
bt_status_t btc_storage_remove_hid_info(bt_bdaddr_t *remote_bd_addr);
#endif // (defined BTC_HH_INCLUDED && BTC_HH_INCLUDED == TRUE)

#if (defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE)
/*******************************************************************************
 * Function         btc_storage_load_hidd
 *
 * Description      Loads hidd bonded device and "plugs" it into hidd
 *
 * Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/
bt_status_t btc_storage_load_hidd(void);

/*******************************************************************************
 *
 * Function         btc_storage_set_hidd
 *
 * Description      Stores hidd bonded device info in nvram.
 *
 * Returns          BT_STATUS_SUCCESS
 *
 ******************************************************************************/
bt_status_t btc_storage_set_hidd(bt_bdaddr_t *remote_bd_addr);

/*******************************************************************************
 *
 * Function         btc_storage_remove_hidd
 *
 * Description      Removes hidd bonded device info from nvram
 *
 * Returns          BT_STATUS_SUCCESS
 *
 ******************************************************************************/
bt_status_t btc_storage_remove_hidd(bt_bdaddr_t *remote_bd_addr);
#endif //(defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE)
#endif /* BTC_STORAGE_H */
#ifdef __cplusplus
}
#endif
