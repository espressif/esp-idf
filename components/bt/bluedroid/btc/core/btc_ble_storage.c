// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

#include "bta_api.h"
#include "btc_config.h"
#include "bdaddr.h"
#include "btc_ble_storage.h"
#include "bta_gatts_co.h"
#include "btc_util.h"

#if (SMP_INCLUDED == TRUE)

btc_dm_pairing_cb_t pairing_cb;
btc_dm_local_key_cb_t ble_local_key_cb;

/*******************************************************************************
**
** Function         btc_storage_load_bonded_devices
**
** Description      btc storage API - Loads all the bonded devices from NVRAM
**                  and adds to the BTA.
**                  Additionally, this API also invokes the adaper_properties_cb
**                  and remote_device_properties_cb for each of the bonded devices.
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_load_bonded_ble_devices(void)
{
    bt_status_t status;
    status = btc_in_fetch_bonded_ble_devices(1);
    LOG_DEBUG("Storage load rslt %d\n", status);
    return status;
}

bt_status_t btc_in_fetch_bonded_ble_devices(int add)
{
    btc_bonded_devices_t bonded_devices;
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (!string_is_bdaddr(name)) {
            continue;
        }

        if (btc_in_fetch_bonded_ble_device(name, add, &bonded_devices) != BT_STATUS_SUCCESS) {
            LOG_DEBUG("Remote device:%s, no link key or ble key found", name);
        }
    }

    return BT_STATUS_SUCCESS;
}

void btc_dm_remove_ble_bonding_keys(void)
{
    bt_bdaddr_t bd_addr;
    LOG_DEBUG("%s\n",__func__);

    bdcpy(bd_addr.address, pairing_cb.bd_addr);
    btc_storage_remove_ble_bonding_keys(&bd_addr);
}

void btc_save_ble_bonding_keys(void)
{
    bt_bdaddr_t bd_addr;

    bdcpy(bd_addr.address, pairing_cb.bd_addr);
    bdstr_t bdstr;
    bdaddr_to_string(&bd_addr, bdstr, sizeof(bdstr));
    btc_config_set_int(bdstr, "DevType", BT_DEVICE_TYPE_BLE);
    if (pairing_cb.ble.is_penc_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &pairing_cb.ble.penc_key,
                                        BTM_LE_KEY_PENC,
                                        sizeof(tBTM_LE_PENC_KEYS));
    }

    if (pairing_cb.ble.is_pid_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &pairing_cb.ble.pid_key,
                                        BTM_LE_KEY_PID,
                                        sizeof(tBTM_LE_PID_KEYS));
    }


    if (pairing_cb.ble.is_pcsrk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &pairing_cb.ble.pcsrk_key,
                                        BTM_LE_KEY_PCSRK,
                                        sizeof(tBTM_LE_PCSRK_KEYS));
    }


    if (pairing_cb.ble.is_lenc_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &pairing_cb.ble.lenc_key,
                                        BTM_LE_KEY_LENC,
                                        sizeof(tBTM_LE_LENC_KEYS));
    }

    if (pairing_cb.ble.is_lcsrk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        (char *) &pairing_cb.ble.lcsrk_key,
                                        BTM_LE_KEY_LCSRK,
                                        sizeof(tBTM_LE_LCSRK_KEYS));
    }

    if (pairing_cb.ble.is_lidk_key_rcvd) {
        btc_storage_add_ble_bonding_key(&bd_addr,
                                        NULL,
                                        BTM_LE_KEY_LID,
                                        0);
    }
}

static void btc_read_le_key(const uint8_t key_type, const size_t key_len, bt_bdaddr_t bd_addr,
                 const uint8_t addr_type, const bool add_key, bool *device_added, bool *key_found)
{
    assert(device_added);
    assert(key_found);

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));
    if (btc_storage_get_ble_bonding_key(&bd_addr, key_type, buffer, key_len) == BT_STATUS_SUCCESS) {
        if (add_key) {
            BD_ADDR bta_bd_addr;
            bdcpy(bta_bd_addr, bd_addr.address);

            if (!*device_added) {
                BTA_DmAddBleDevice(bta_bd_addr, addr_type, BT_DEVICE_TYPE_BLE);
                *device_added = true;
            }

            char bd_str[20] = {0};
            LOG_DEBUG("%s() Adding key type %d for %s", __func__,
                key_type, bdaddr_to_string(&bd_addr, bd_str, sizeof(bd_str)));
            BTA_DmAddBleKey(bta_bd_addr, (tBTA_LE_KEY_VALUE *)buffer, key_type);
        }

        *key_found = true;
    }
}

bt_status_t btc_storage_add_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            char *key,
                                            uint8_t key_type,
                                            uint8_t key_length)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    const char* name;
    switch (key_type) {
    case BTM_LE_KEY_PENC:
        name = "LE_KEY_PENC";
        break;
    case BTM_LE_KEY_PID:
        name = "LE_KEY_PID";
        break;
    case BTM_LE_KEY_PCSRK:
        name = "LE_KEY_PCSRK";
        break;
    case BTM_LE_KEY_LENC:
        name = "LE_KEY_LENC";
        break;
    case BTM_LE_KEY_LCSRK:
        name = "LE_KEY_LCSRK";
        break;
    case BTM_LE_KEY_LID:
        name = "LE_KEY_LID";
        break;
    default:
        return BT_STATUS_FAIL;
    }

    int ret = btc_config_set_bin(bdstr, name, (const uint8_t *)key, key_length);
    btc_config_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}


/*******************************************************************************
**
** Function         btc_storage_get_ble_bonding_key
**
** Description
**
** Returns       BT_STATUS_SUCCESS if the fetch was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_get_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            uint8_t key_type,
                                            char *key_value,
                                            int key_length)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    const char* name;
    switch (key_type) {
    case BTM_LE_KEY_PENC:
        name = "LE_KEY_PENC";
        break;
    case BTM_LE_KEY_PID:
        name = "LE_KEY_PID";
        break;
    case BTM_LE_KEY_PCSRK:
        name = "LE_KEY_PCSRK";
        break;
    case BTM_LE_KEY_LENC:
        name = "LE_KEY_LENC";
        break;
    case BTM_LE_KEY_LCSRK:
        name = "LE_KEY_LCSRK";
        break;
    case BTM_LE_KEY_LID:
        name =  "LE_KEY_LID";
    default:
        return BT_STATUS_FAIL;
    }
    size_t length = key_length;
    int ret = btc_config_get_bin(bdstr, name, (uint8_t *)key_value, &length);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;

}

bool btc_storage_compare_address_key_value(uint8_t key_type, void *key_value, int key_length)
{
    char *key_type_str;
    switch (key_type) {
    case BTM_LE_KEY_PENC:
        key_type_str = "LE_KEY_PENC";
        break;
    case BTM_LE_KEY_PID:
        key_type_str = "LE_KEY_PID";
        break;
    case BTM_LE_KEY_PCSRK:
        key_type_str = "LE_KEY_PCSRK";
        break;
    case BTM_LE_KEY_LENC:
        key_type_str = "LE_KEY_LENC";
        break;
    case BTM_LE_KEY_LCSRK:
        key_type_str = "LE_KEY_LCSRK";
        break;
    case BTM_LE_KEY_LID:
        key_type_str =  "LE_KEY_LID";
    default:
        return false;
    }

    return btc_compare_address_key_value(key_type_str, key_value, key_length);
}


/*******************************************************************************
**
** Function         btc_storage_remove_ble_bonding_keys
**
** Description      btc storage API - Deletes the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if the deletion was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_remove_ble_bonding_keys(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    BTIF_TRACE_DEBUG(" %s in bd addr:%s",__FUNCTION__, bdstr);
    int ret = 1;
    if (btc_config_exist(bdstr, "LE_KEY_PENC")) {
        ret &= btc_config_remove(bdstr, "LE_KEY_PENC");
    }
    if (btc_config_exist(bdstr, "LE_KEY_PID")) {
        ret &= btc_config_remove(bdstr, "LE_KEY_PID");
    }
    if (btc_config_exist(bdstr, "LE_KEY_PCSRK")) {
        ret &= btc_config_remove(bdstr, "LE_KEY_PCSRK");
    }
    if (btc_config_exist(bdstr, "LE_KEY_LENC")) {
        ret &= btc_config_remove(bdstr, "LE_KEY_LENC");
    }
    if (btc_config_exist(bdstr, "LE_KEY_LCSRK")) {
        ret &= btc_config_remove(bdstr, "LE_KEY_LCSRK");
    }
    btc_config_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_storage_add_ble_local_key
**
** Description      BTIF storage API - Adds the ble key to NVRAM
**
** Returns          BT_STATUS_SUCCESS if the store was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_add_ble_local_key(char *key,
                                          uint8_t key_type,
                                          uint8_t key_length)
{
    const char* name;
    switch (key_type) {
    case BTC_LE_LOCAL_KEY_IR:
        name = "LE_LOCAL_KEY_IR";
        break;
    case BTC_LE_LOCAL_KEY_IRK:
        name = "LE_LOCAL_KEY_IRK";
        break;
    case BTC_LE_LOCAL_KEY_DHK:
        name = "LE_LOCAL_KEY_DHK";
        break;
    case BTC_LE_LOCAL_KEY_ER:
        name = "LE_LOCAL_KEY_ER";
        break;
    default:
        return BT_STATUS_FAIL;
    }
    int ret = btc_config_set_bin("Adapter", name, (const uint8_t *)key, key_length);
    btc_config_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_storage_get_ble_local_key
**
** Description
**
** Returns          BT_STATUS_SUCCESS if the fetch was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_get_ble_local_key(uint8_t key_type,
                                          char *key_value,
                                          int key_length)
{
    const char* name;
    switch (key_type) {
    case BTC_LE_LOCAL_KEY_IR:
        name = "LE_LOCAL_KEY_IR";
        break;
    case BTC_LE_LOCAL_KEY_IRK:
        name = "LE_LOCAL_KEY_IRK";
        break;
    case BTC_LE_LOCAL_KEY_DHK:
        name = "LE_LOCAL_KEY_DHK";
        break;
    case BTC_LE_LOCAL_KEY_ER:
        name = "LE_LOCAL_KEY_ER";
        break;
    default:
        return BT_STATUS_FAIL;
    }
    size_t length = key_length;
    int ret = btc_config_get_bin("Adapter", name, (uint8_t *)key_value, &length);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_storage_remove_ble_local_keys
**
** Description      BTC storage API - Deletes the bonded device from NVRAM
**
** Returns          BT_STATUS_SUCCESS if the deletion was successful,
**                  BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_remove_ble_local_keys(void)
{
    int ret = 1;
    if (btc_config_exist("Adapter", "LE_LOCAL_KEY_IR")) {
        ret &= btc_config_remove("Adapter", "LE_LOCAL_KEY_IR");
    }
    if (btc_config_exist("Adapter", "LE_LOCAL_KEY_IRK")) {
        ret &= btc_config_remove("Adapter", "LE_LOCAL_KEY_IRK");
    }
    if (btc_config_exist("Adapter", "LE_LOCAL_KEY_DHK")) {
        ret &= btc_config_remove("Adapter", "LE_LOCAL_KEY_DHK");
    }
    if (btc_config_exist("Adapter", "LE_LOCAL_KEY_ER")) {
        ret &= btc_config_remove("Adapter", "LE_LOCAL_KEY_ER");
    }
    btc_config_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}


bt_status_t btc_in_fetch_bonded_ble_device(const char *remote_bd_addr, int add, 
                                           btc_bonded_devices_t *p_bonded_devices)
{
    int device_type;
    int addr_type;
    bt_bdaddr_t bd_addr;
    BD_ADDR bta_bd_addr;
    bool device_added = false;
    bool key_found = false;

    if (!btc_config_get_int(remote_bd_addr, "DevType", &device_type)) {
        LOG_ERROR("%s, device_type = %x", __func__, device_type);
        return BT_STATUS_FAIL;
    }
   
    string_to_bdaddr(remote_bd_addr, &bd_addr);
    bdcpy(bta_bd_addr, bd_addr.address);

    if (btc_storage_get_remote_addr_type(&bd_addr, &addr_type) != BT_STATUS_SUCCESS) {
        addr_type = BLE_ADDR_PUBLIC;
        btc_storage_set_remote_addr_type(&bd_addr, BLE_ADDR_PUBLIC);
    }

    btc_read_le_key(BTM_LE_KEY_PENC, sizeof(tBTM_LE_PENC_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    btc_read_le_key(BTM_LE_KEY_PID, sizeof(tBTM_LE_PID_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    btc_read_le_key(BTM_LE_KEY_LID, sizeof(tBTM_LE_PID_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    btc_read_le_key(BTM_LE_KEY_PCSRK, sizeof(tBTM_LE_PCSRK_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    btc_read_le_key(BTM_LE_KEY_LENC, sizeof(tBTM_LE_LENC_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    btc_read_le_key(BTM_LE_KEY_LCSRK, sizeof(tBTM_LE_LCSRK_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    if (key_found) {
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

bt_status_t btc_storage_set_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
        uint8_t addr_type)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bt_bdaddr_t));
    int ret = btc_config_set_int(bdstr, "AddrType", (int)addr_type);
    btc_config_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_storage_get_remote_addr_type
**
** Description      btc storage API - Fetches the remote addr type
**
** Returns          BT_STATUS_SUCCESS if the fetch was successful,
**                      BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btc_storage_get_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
                                             int*addr_type)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = btc_config_get_int(bdstr, "AddrType", addr_type);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}


void btc_dm_load_ble_local_keys(void)
{
    memset(&ble_local_key_cb, 0, sizeof(btc_dm_local_key_cb_t));

    if (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_ER,(char*)&ble_local_key_cb.er[0],
                                      BT_OCTET16_LEN)== BT_STATUS_SUCCESS) {
        ble_local_key_cb.is_er_rcvd = TRUE;
        LOG_DEBUG("%s BLE ER key loaded",__func__ );
    }

    if ((btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_IR,(char*)&ble_local_key_cb.id_keys.ir[0],
                                       BT_OCTET16_LEN)== BT_STATUS_SUCCESS )&&
            (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_IRK, (char*)&ble_local_key_cb.id_keys.irk[0],
                                           BT_OCTET16_LEN)== BT_STATUS_SUCCESS)&&
            (btc_storage_get_ble_local_key(BTC_LE_LOCAL_KEY_DHK,(char*)&ble_local_key_cb.id_keys.dhk[0],
                                           BT_OCTET16_LEN)== BT_STATUS_SUCCESS)) {
        ble_local_key_cb.is_id_keys_rcvd = TRUE;
        LOG_DEBUG("%s BLE ID keys loaded", __func__);
    }

}
void btc_dm_get_ble_local_keys(tBTA_DM_BLE_LOCAL_KEY_MASK *p_key_mask, BT_OCTET16 er,
                               tBTA_BLE_LOCAL_ID_KEYS *p_id_keys)
{
    if (ble_local_key_cb.is_er_rcvd ) {
        memcpy(&er[0], &ble_local_key_cb.er[0], sizeof(BT_OCTET16));
        *p_key_mask |= BTA_BLE_LOCAL_KEY_TYPE_ER;
    }

    if (ble_local_key_cb.is_id_keys_rcvd) {
        memcpy(&p_id_keys->ir[0], &ble_local_key_cb.id_keys.ir[0], sizeof(BT_OCTET16));
        memcpy(&p_id_keys->irk[0],  &ble_local_key_cb.id_keys.irk[0], sizeof(BT_OCTET16));
        memcpy(&p_id_keys->dhk[0],  &ble_local_key_cb.id_keys.dhk[0], sizeof(BT_OCTET16));
        *p_key_mask |= BTA_BLE_LOCAL_KEY_TYPE_ID;
    }
    LOG_DEBUG("%s  *p_key_mask=0x%02x",__func__,   *p_key_mask);
}

#endif  ///SMP_INCLUDED == TRUE
                                       
