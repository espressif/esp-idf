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

static void _btc_storage_save(void)
{
    const btc_config_section_iter_t *iter = btc_config_section_begin();

    while (iter != btc_config_section_end()) {
        //store the next iter, if remove section, then will not loss the point

        const char *section = btc_config_section_name(iter);
        if (!string_is_bdaddr(section)) {
            iter = btc_config_section_next(iter);
            continue;
        }

        if (!btc_config_exist(section, BTC_BLE_STORAGE_DEV_TYPE_STR) && 
                !btc_config_exist(section, BTC_BLE_STORAGE_ADDR_TYPE_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LINK_KEY_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PENC_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PID_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_LENC_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR)) {
            iter = btc_config_section_next(iter);
            btc_config_remove_section(section);
            continue;
        }

        iter = btc_config_section_next(iter);
    }

    btc_config_flush();
}

void btc_storage_save(void)
{
    btc_config_lock();
    _btc_storage_save();
    btc_config_unlock();
}

static bt_status_t _btc_storage_add_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            char *key,
                                            uint8_t key_type,
                                            uint8_t key_length)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    const char* name;

    switch (key_type) {
    case BTM_LE_KEY_PENC:
        name = BTC_BLE_STORAGE_LE_KEY_PENC_STR;
        break;
    case BTM_LE_KEY_PID:
        name = BTC_BLE_STORAGE_LE_KEY_PID_STR;
        break;
    case BTM_LE_KEY_PCSRK:
        name = BTC_BLE_STORAGE_LE_KEY_PCSRK_STR;
        break;
    case BTM_LE_KEY_LENC:
        name = BTC_BLE_STORAGE_LE_KEY_LENC_STR;
        break;
    case BTM_LE_KEY_LCSRK:
        name = BTC_BLE_STORAGE_LE_KEY_LCSRK_STR;
        break;
    case BTM_LE_KEY_LID:
        name = BTC_BLE_STORAGE_LE_KEY_LID_STR;
        break;
    default:
        return BT_STATUS_FAIL;
    }

    int ret = btc_config_set_bin(bdstr, name, (const uint8_t *)key, key_length);
    _btc_storage_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_add_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            char *key,
                                            uint8_t key_type,
                                            uint8_t key_length)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_add_ble_bonding_key(remote_bd_addr, key, key_type, key_length);
    btc_config_unlock();

    return ret;
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
static bt_status_t _btc_storage_get_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            uint8_t key_type,
                                            char *key_value,
                                            int key_length)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    const char* name;
    switch (key_type) {
    case BTM_LE_KEY_PENC:
        name = BTC_BLE_STORAGE_LE_KEY_PENC_STR;
        break;
    case BTM_LE_KEY_PID:
        name = BTC_BLE_STORAGE_LE_KEY_PID_STR;
        break;
    case BTM_LE_KEY_PCSRK:
        name = BTC_BLE_STORAGE_LE_KEY_PCSRK_STR;
        break;
    case BTM_LE_KEY_LENC:
        name = BTC_BLE_STORAGE_LE_KEY_LENC_STR;
        break;
    case BTM_LE_KEY_LCSRK:
        name = BTC_BLE_STORAGE_LE_KEY_LCSRK_STR;
        break;
    case BTM_LE_KEY_LID:
        name =  BTC_BLE_STORAGE_LE_KEY_LID_STR;
    default:
        return BT_STATUS_FAIL;
    }
    size_t length = key_length;
    int ret = btc_config_get_bin(bdstr, name, (uint8_t *)key_value, &length);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;

}

bt_status_t btc_storage_get_ble_bonding_key(bt_bdaddr_t *remote_bd_addr,
                                            uint8_t key_type,
                                            char *key_value,
                                            int key_length)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_get_ble_bonding_key(remote_bd_addr, key_type, key_value, key_length);
    btc_config_unlock();

    return ret;
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
static bt_status_t _btc_storage_remove_ble_bonding_keys(bt_bdaddr_t *remote_bd_addr)
{
    int ret = 1;
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    BTIF_TRACE_DEBUG(" %s in bd addr:%s",__FUNCTION__, bdstr);

    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR);
    }
    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_LE_KEY_PENC_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_KEY_PENC_STR);
    }
    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_LE_KEY_PID_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_KEY_PID_STR);
    }
    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR);
    }
    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_LE_KEY_LENC_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_KEY_LENC_STR);
    }
    if (btc_config_exist(bdstr, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR)) {
        ret &= btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR);
    }
    //here don't remove section, because config_save will check it
    _btc_storage_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_remove_ble_bonding_keys(bt_bdaddr_t *remote_bd_addr)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_remove_ble_bonding_keys(remote_bd_addr);
    btc_config_unlock();

    return ret;
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
static bt_status_t _btc_storage_add_ble_local_key(char *key,
                                          uint8_t key_type,
                                          uint8_t key_length)
{
    const char* name;
    switch (key_type) {
    case BTC_LE_LOCAL_KEY_IR:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_IR_STR;
        break;
    case BTC_LE_LOCAL_KEY_IRK:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_IRK_STR;
        break;
    case BTC_LE_LOCAL_KEY_DHK:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_DHK_STR;
        break;
    case BTC_LE_LOCAL_KEY_ER:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_ER_STR;
        break;
    default:
        return BT_STATUS_FAIL;
    }

    int ret = btc_config_set_bin(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, name, (const uint8_t *)key, key_length);
    _btc_storage_save();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_add_ble_local_key(char *key,
                                          uint8_t key_type,
                                          uint8_t key_length)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_add_ble_local_key(key, key_type, key_length);
    btc_config_unlock();

    return ret;
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
static bt_status_t _btc_storage_get_ble_local_key(uint8_t key_type,
                                          char *key_value,
                                          int key_length)
{
    const char* name;
    switch (key_type) {
    case BTC_LE_LOCAL_KEY_IR:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_IR_STR;
        break;
    case BTC_LE_LOCAL_KEY_IRK:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_IRK_STR;
        break;
    case BTC_LE_LOCAL_KEY_DHK:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_DHK_STR;
        break;
    case BTC_LE_LOCAL_KEY_ER:
        name = BTC_BLE_STORAGE_LE_LOCAL_KEY_ER_STR;
        break;
    default:
        return BT_STATUS_FAIL;
    }
    size_t length = key_length;

    int ret = btc_config_get_bin(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, name, (uint8_t *)key_value, &length);

    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_get_ble_local_key(uint8_t key_type,
                                          char *key_value,
                                          int key_length)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_get_ble_local_key(key_type, key_value, key_length);
    btc_config_unlock();

    return ret;
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
static bt_status_t _btc_storage_remove_ble_local_keys(void)
{
    int ret = 1;

    if (btc_config_exist(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_IR_STR)) {
        ret &= btc_config_remove(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_IR_STR);
    }
    if (btc_config_exist(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_IRK_STR)) {
        ret &= btc_config_remove(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_IRK_STR);
    }
    if (btc_config_exist(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_DHK_STR)) {
        ret &= btc_config_remove(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_DHK_STR);
    }
    if (btc_config_exist(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_ER_STR)) {
        ret &= btc_config_remove(BTC_BLE_STORAGE_LOCAL_ADAPTER_STR, BTC_BLE_STORAGE_LE_LOCAL_KEY_ER_STR);
    }
    _btc_storage_save();

    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_remove_ble_local_keys(void)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_remove_ble_local_keys();
    btc_config_unlock();

    return ret;
}

bool _btc_storage_compare_address_key_value(bt_bdaddr_t *remote_bd_addr,
                                                   uint8_t key_type, void *key_value, int key_length)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    char *key_type_str;
    switch (key_type) {
    case BTM_LE_KEY_PENC:
        key_type_str = BTC_BLE_STORAGE_LE_KEY_PENC_STR;
        break;
    case BTM_LE_KEY_PID:
        key_type_str = BTC_BLE_STORAGE_LE_KEY_PID_STR;
        break;
    case BTM_LE_KEY_PCSRK:
        key_type_str = BTC_BLE_STORAGE_LE_KEY_PCSRK_STR;
        break;
    case BTM_LE_KEY_LENC:
        key_type_str = BTC_BLE_STORAGE_LE_KEY_LENC_STR;
        break;
    case BTM_LE_KEY_LCSRK:
        key_type_str = BTC_BLE_STORAGE_LE_KEY_LCSRK_STR;
        break;
    case BTM_LE_KEY_LID:
        key_type_str =  BTC_BLE_STORAGE_LE_KEY_LID_STR;
    default:
        return false;
    }

    return btc_compare_address_key_value(bdstr, key_type_str, key_value, key_length);
}

bool btc_storage_compare_address_key_value(bt_bdaddr_t *remote_bd_addr,
                                                   uint8_t key_type, void *key_value, int key_length)
{
    bool ret;

    btc_config_lock();
    ret = _btc_storage_compare_address_key_value(remote_bd_addr, key_type, key_value, key_length);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_set_ble_dev_type(bt_bdaddr_t *bd_addr, bool flush)
{
    bool ret = 1;
    bdstr_t bdstr;
    uint32_t dev_type = 0;

    bdaddr_to_string(bd_addr, bdstr, sizeof(bdstr));

    btc_config_get_int(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&dev_type);
    ret = btc_config_set_int(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR, BT_DEVICE_TYPE_BLE|dev_type);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_set_ble_dev_type(bt_bdaddr_t *bd_addr, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_set_ble_dev_type(bd_addr, flush);
    btc_config_unlock();

    return ret;
}

static bool _btc_storage_get_ble_dev_type(bt_bdaddr_t *bd_addr)
{
    bool ret = 1;
    bdstr_t bdstr;
    uint32_t dev_type = 0;

    bdaddr_to_string(bd_addr, bdstr, sizeof(bdstr));

    BTIF_TRACE_DEBUG(" %s in bd addr:%s",__FUNCTION__, bdstr);

    ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&dev_type);
    if (ret == false) {
        return false;
    }

    return (dev_type & BT_DEVICE_TYPE_BLE);
}

bool btc_storage_get_ble_dev_type(bt_bdaddr_t *bd_addr)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_get_ble_dev_type(bd_addr);
    btc_config_unlock();

    return ret;
}


static bt_status_t _btc_storage_remove_ble_dev_type(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bool ret = true;
    bdstr_t bdstr;
    uint32_t dev_type = 0;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    BTIF_TRACE_DEBUG(" %s in bd addr:%s",__FUNCTION__, bdstr);

    ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&dev_type);
    if (ret == false) {
        //cannot find the key, just return SUCCESS, indicate already removed
        return BT_STATUS_SUCCESS;
    }

    if (dev_type == BT_DEVICE_TYPE_DUMO) {
        ret = btc_config_set_int(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR, BT_DEVICE_TYPE_BREDR);
    } else if (dev_type == BT_DEVICE_TYPE_BLE) {
        ret = btc_config_remove(bdstr, BTC_BLE_STORAGE_DEV_TYPE_STR);
    }

    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_remove_ble_dev_type(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_remove_ble_dev_type(remote_bd_addr, flush);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_set_remote_addr_type(bt_bdaddr_t *remote_bd_addr, uint8_t addr_type, bool flush)
{
    int ret;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr_t));
    ret = btc_config_set_int(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR, (int)addr_type);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_set_remote_addr_type(bt_bdaddr_t *remote_bd_addr, uint8_t addr_type, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_set_remote_addr_type(remote_bd_addr, addr_type, flush);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_remove_remote_addr_type(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bool ret = true;
    bdstr_t bdstr;
    uint32_t dev_type = 0;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR, (int *)&dev_type);
    if (ret == false) {
        //cannot find the key, just return SUCCESS, indicate already removed
        return BT_STATUS_SUCCESS;
    }

    ret = btc_config_remove(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return  BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_remove_remote_addr_type(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_remove_remote_addr_type(remote_bd_addr, flush);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_get_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
                                             int*addr_type)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_ADDR_TYPE_STR, addr_type);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_get_remote_addr_type(bt_bdaddr_t *remote_bd_addr,
                                             int*addr_type)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_get_remote_addr_type(remote_bd_addr, addr_type);
    btc_config_unlock();

    return ret;
}

static void _btc_read_le_key(const uint8_t key_type, const size_t key_len, bt_bdaddr_t bd_addr,
                 const uint8_t addr_type, const bool add_key, bool *device_added, bool *key_found)
{
    assert(device_added);
    assert(key_found);

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));

    bt_status_t ret = _btc_storage_get_ble_bonding_key(&bd_addr, key_type, buffer, key_len);

    if (ret == BT_STATUS_SUCCESS) {
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
static bt_status_t _btc_storage_in_fetch_bonded_ble_device(const char *remote_bd_addr, int add)
{
    uint32_t device_type;
    int addr_type;
    bt_bdaddr_t bd_addr;
    BD_ADDR bta_bd_addr;
    bool device_added = false;
    bool key_found = false;

    if (!btc_config_get_int(remote_bd_addr, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type)) {
        LOG_ERROR("%s, device_type = %x", __func__, device_type);
        return BT_STATUS_FAIL;
    }
   
    string_to_bdaddr(remote_bd_addr, &bd_addr);
    bdcpy(bta_bd_addr, bd_addr.address);

    if (_btc_storage_get_remote_addr_type(&bd_addr, &addr_type) != BT_STATUS_SUCCESS) {
        addr_type = BLE_ADDR_PUBLIC;
        _btc_storage_set_remote_addr_type(&bd_addr, BLE_ADDR_PUBLIC, true);
    }

    _btc_read_le_key(BTM_LE_KEY_PENC, sizeof(tBTM_LE_PENC_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    _btc_read_le_key(BTM_LE_KEY_PID, sizeof(tBTM_LE_PID_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    _btc_read_le_key(BTM_LE_KEY_LID, sizeof(tBTM_LE_PID_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    _btc_read_le_key(BTM_LE_KEY_PCSRK, sizeof(tBTM_LE_PCSRK_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    _btc_read_le_key(BTM_LE_KEY_LENC, sizeof(tBTM_LE_LENC_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    _btc_read_le_key(BTM_LE_KEY_LCSRK, sizeof(tBTM_LE_LCSRK_KEYS),
                    bd_addr, addr_type, add, &device_added, &key_found);

    if (key_found) {
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}

static bt_status_t btc_storage_in_fetch_bonded_ble_devices(int add)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint32_t device_type = 0;

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        
        if (!string_is_bdaddr(name) ||
            !btc_config_get_int(name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
            ((device_type & BT_DEVICE_TYPE_BLE) != BT_DEVICE_TYPE_BLE)) {
            continue;
        }
        LOG_DEBUG("%s, name = %s", __func__, name);
        if (_btc_storage_in_fetch_bonded_ble_device(name, add) != BT_STATUS_SUCCESS) {
            LOG_DEBUG("Remote device:%s, no link key or ble key found", name);
        } else {
            status = BT_STATUS_SUCCESS;
        }
    }
    btc_config_unlock();

    return status;
}

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
    status = btc_storage_in_fetch_bonded_ble_devices(1);
    LOG_DEBUG("Storage load rslt %d\n", status);
    return status;
}

bt_status_t btc_storage_get_bonded_ble_devices_list(esp_ble_bond_dev_t *bond_dev, int dev_num)
{
    bt_bdaddr_t bd_addr;
    uint32_t device_type = 0;
    char buffer[sizeof(tBTM_LE_KEY_VALUE)] = {0};

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {

        if (dev_num-- <= 0) {
            break;
        }

        const char *name = btc_config_section_name(iter);

        if (!string_is_bdaddr(name) ||
                !btc_config_get_int(name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
                !(device_type & BT_DEVICE_TYPE_BLE)) {
            continue;
        }

        string_to_bdaddr(name, &bd_addr);
        memcpy(bond_dev->bd_addr, bd_addr.address, sizeof(bt_bdaddr_t));
        //resolve the peer device long term key
        if (_btc_storage_get_ble_bonding_key(&bd_addr, BTM_LE_KEY_PENC, buffer, sizeof(tBTM_LE_PENC_KEYS)) == BT_STATUS_SUCCESS) {
            bond_dev->bond_key.key_mask |= ESP_BLE_ENC_KEY_MASK;
            memcpy(&bond_dev->bond_key.penc_key, buffer, sizeof(tBTM_LE_PENC_KEYS));
        }
        //resolve the peer device csrk
        if (_btc_storage_get_ble_bonding_key(&bd_addr, BTM_LE_KEY_PCSRK, buffer, sizeof(tBTM_LE_PCSRK_KEYS)) == BT_STATUS_SUCCESS) {
            bond_dev->bond_key.key_mask |= ESP_BLE_CSR_KEY_MASK;
            memcpy(&bond_dev->bond_key.pcsrk_key, buffer, sizeof(tBTM_LE_PCSRK_KEYS));
        }
        //resolve the peer device irk
        if (_btc_storage_get_ble_bonding_key(&bd_addr, BTM_LE_KEY_PID, buffer, sizeof(tBTM_LE_PID_KEYS)) == BT_STATUS_SUCCESS) {
            bond_dev->bond_key.key_mask |= ESP_BLE_ID_KEY_MASK;
            memcpy(&bond_dev->bond_key.pid_key, buffer, sizeof(tBTM_LE_PID_KEYS));
        }
        //serch for the next bond device
        bond_dev++;
    }
    btc_config_unlock();

    return BT_STATUS_SUCCESS;
}

int btc_storage_get_num_ble_bond_devices(void)
{
    int num_dev = 0;
    uint32_t device_type = 0;

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (!string_is_bdaddr(name) ||
                !btc_config_get_int(name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
                !(device_type & BT_DEVICE_TYPE_BLE)) {
            continue;
        }

        num_dev++;
    }
    btc_config_unlock();

    return num_dev;
}
#endif  ///SMP_INCLUDED == TRUE
                                       
