/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "bta/bta_api.h"
#include "btc/btc_config.h"
#include "device/bdaddr.h"
#include "btc/btc_ble_storage.h"
#include "bta/bta_gatts_co.h"
#include "btc/btc_util.h"

#if (SMP_INCLUDED == TRUE)

//the maximum nubmer of bonded devices
#define BONED_DEVICES_MAX_COUNT (BTM_SEC_MAX_DEVICE_RECORDS)

static void _btc_storage_save(void)
{
    uint16_t addr_section_count = 0;
    bt_bdaddr_t bd_addr;

    const btc_config_section_iter_t *need_remove_iter = NULL;
    const btc_config_section_iter_t *iter = btc_config_section_begin();

    while (iter != btc_config_section_end()) {
        //store the next iter, if remove section, then will not loss the point

        const char *section = btc_config_section_name(iter);

        if (string_is_bdaddr(section) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_DEV_TYPE_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_ADDR_TYPE_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LINK_KEY_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PENC_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PID_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_LENC_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_GATT_CL_SUPP_FEAT_STR) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_GATT_DB_HASH_STR)) {
            iter = btc_config_section_next(iter);
            btc_config_remove_section(section);
            continue;
        }

        if (!string_is_bdaddr(section)) {
            iter = btc_config_section_next(iter);
            continue;
        }

        if(addr_section_count == BONED_DEVICES_MAX_COUNT) {
            need_remove_iter = iter;
        }
        addr_section_count ++;
        iter = btc_config_section_next(iter);
    }
    /*exceeded the maximum nubmer of bonded devices, delete them */
    if (need_remove_iter) {
        while(need_remove_iter != btc_config_section_end()) {
            const char *need_remove_section = btc_config_section_name(need_remove_iter);
            if (!string_is_bdaddr(need_remove_section)) {
                need_remove_iter = btc_config_section_next(need_remove_iter);
                continue;
            }
            need_remove_iter = btc_config_section_next(need_remove_iter);
            //delete device info
            string_to_bdaddr(need_remove_section, &bd_addr);
            BTA_DmRemoveDevice(bd_addr.address, BT_TRANSPORT_LE);
            BTA_DmRemoveDevice(bd_addr.address, BT_TRANSPORT_BR_EDR);
            //delete config info
            if (btc_config_remove_section(need_remove_section)) {
                // The need_remove_section has been freed
                BTIF_TRACE_WARNING("Exceeded the maximum number of bonded devices. Deleting the last device info: %02x:%02x:%02x:%02x:%02x:%02x",
                                bd_addr.address[0], bd_addr.address[1], bd_addr.address[2], bd_addr.address[3], bd_addr.address[4], bd_addr.address[5]);
            }
        }
    }
    btc_config_flush();
}

void btc_storage_save(void)
{
    btc_config_lock();
    _btc_storage_save();
    btc_config_unlock();
}

#if (BLE_INCLUDED == TRUE)
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

static bt_status_t _btc_storage_remove_all_ble_keys(const char *name)
{
    int ret = 0;

    if (name == NULL) {
        return BT_STATUS_FAIL;
    }

    if (btc_config_exist(name, BTC_BLE_STORAGE_ADDR_TYPE_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_ADDR_TYPE_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_PENC_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_PENC_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_PID_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_PID_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_PCSRK_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_LENC_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_LENC_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_LCSRK_STR);
    }
    if (btc_config_exist(name, BTC_BLE_STORAGE_LE_KEY_LID_STR)) {
        ret |= btc_config_remove(name, BTC_BLE_STORAGE_LE_KEY_LID_STR);
    }

    return ret;
}

void btc_storage_remove_unused_sections(uint8_t *cur_addr, tBTM_LE_PID_KEYS *del_pid_key)
{
    bt_bdaddr_t bd_addr;
    uint32_t device_type = 0;

    if (del_pid_key == NULL) {
        return;
    }

    btc_config_lock();

    const btc_config_section_iter_t *iter = btc_config_section_begin();

    while (iter != btc_config_section_end()) {
        //store the next iter, if remove section, then will not loss the point

        const char *section = btc_config_section_name(iter);
        if (string_is_bdaddr(section) &&
                !btc_config_exist(section, BTC_BLE_STORAGE_DEV_TYPE_STR) &&
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

        if (!string_is_bdaddr(section) ||
            !btc_config_get_int(section, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
            ((device_type & BT_DEVICE_TYPE_BLE) != BT_DEVICE_TYPE_BLE)) {
            iter = btc_config_section_next(iter);
            continue;
        }

        string_to_bdaddr(section, &bd_addr);

        char buffer[sizeof(tBTM_LE_KEY_VALUE)] = {0};

        if (_btc_storage_get_ble_bonding_key(&bd_addr, BTM_LE_KEY_PID, buffer, sizeof(tBTM_LE_PID_KEYS)) == BT_STATUS_SUCCESS) {

            tBTM_LE_PID_KEYS *pid_key = (tBTM_LE_PID_KEYS *) buffer;

            iter = btc_config_section_next(iter);

            if (memcmp(del_pid_key->static_addr, pid_key->static_addr, 6) == 0 && memcmp(cur_addr, bd_addr.address, 6) != 0 &&  del_pid_key->addr_type == pid_key->addr_type) {
                if (device_type == BT_DEVICE_TYPE_DUMO) {
                    btc_config_set_int(section, BTC_BLE_STORAGE_DEV_TYPE_STR, BT_DEVICE_TYPE_BREDR);
                    _btc_storage_remove_all_ble_keys(section);
                } else {
                    //delete unused sections
                    BTIF_TRACE_DEBUG("delete section %s\n", section);
                    btc_config_remove_section(section);
                }
            }
        } else {
            iter = btc_config_section_next(iter);
        }
    }

    btc_config_unlock();

}

void btc_storage_delete_duplicate_ble_devices(void)
{
    bt_bdaddr_t bd_addr;
    char buffer[sizeof(tBTM_LE_KEY_VALUE)] = {0};
    char temp_buffer[sizeof(tBTM_LE_KEY_VALUE)] = {0};
    tBTM_LE_PID_KEYS *pid_key;
    tBTM_LE_PID_KEYS *temp_pid_key;
    uint32_t device_type = 0;

    bt_bdaddr_t temp_bd_addr;

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);

        if (!string_is_bdaddr(name) ||
            !btc_config_get_int(name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
            ((device_type & BT_DEVICE_TYPE_BLE) != BT_DEVICE_TYPE_BLE)) {
            continue;
        }

        string_to_bdaddr(name, &bd_addr);
        if (_btc_storage_get_ble_bonding_key(&bd_addr, BTM_LE_KEY_PID, buffer, sizeof(tBTM_LE_PID_KEYS)) == BT_STATUS_SUCCESS)
        {
            pid_key = (tBTM_LE_PID_KEYS *) buffer;

            const btc_config_section_iter_t *temp_iter = btc_config_section_next(iter);
            while (temp_iter != NULL)
            {
                const char *temp_name = btc_config_section_name(temp_iter);
                if (!string_is_bdaddr(temp_name) || !btc_config_get_int(temp_name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
                    ((device_type & BT_DEVICE_TYPE_BLE) != BT_DEVICE_TYPE_BLE)) {
                    temp_iter = btc_config_section_next(temp_iter);
                    continue;
                }
                string_to_bdaddr(temp_name, &temp_bd_addr);
                if (_btc_storage_get_ble_bonding_key(&temp_bd_addr, BTM_LE_KEY_PID, temp_buffer, sizeof(tBTM_LE_PID_KEYS)) == BT_STATUS_SUCCESS)
                {
                    temp_pid_key = (tBTM_LE_PID_KEYS *) temp_buffer;
                    if (memcmp(pid_key->static_addr, temp_pid_key->static_addr, 6) == 0 && pid_key->addr_type == temp_pid_key->addr_type) {
                        const char *temp_name = btc_config_section_name(temp_iter);
                        temp_iter = btc_config_section_next(temp_iter);
                        if (device_type == BT_DEVICE_TYPE_DUMO) {
                            btc_config_set_int(temp_name, BTC_BLE_STORAGE_DEV_TYPE_STR, BT_DEVICE_TYPE_BREDR);
                            _btc_storage_remove_all_ble_keys(temp_name);
                        } else {
                            BTC_TRACE_DEBUG("delete %s\n", temp_name);
                            btc_config_remove_section(temp_name);
                        }
                    } else {
                        temp_iter = btc_config_section_next(temp_iter);
                    }
                } else {
                    temp_iter = btc_config_section_next(temp_iter);
                }
            }

        }
    }
    btc_config_unlock();
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
    int ret = 0;
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    BTIF_TRACE_DEBUG(" %s in bd addr:%s",__FUNCTION__, bdstr);

    ret = _btc_storage_remove_all_ble_keys(bdstr);

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
    const char *key_type_str;
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
#endif  ///BLE_INCLUDED == TRUE

static bt_status_t _btc_storage_set_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, uint8_t auth_mode, bool flush)
{
    int ret;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr_t));
    ret = btc_config_set_int(bdstr, BTC_BLE_STORAGE_LE_AUTH_MODE_STR, (int)auth_mode);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_set_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, uint8_t auth_mode, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_set_ble_dev_auth_mode(remote_bd_addr, auth_mode, flush);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_get_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, int* auth_mode)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_LE_AUTH_MODE_STR, auth_mode);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_get_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, int* auth_mode)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_get_ble_dev_auth_mode(remote_bd_addr, auth_mode);
    btc_config_unlock();

    return ret;
}

static bt_status_t _btc_storage_remove_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bool ret = true;
    bdstr_t bdstr;
    uint32_t auth_mode = 0;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    ret = btc_config_get_int(bdstr, BTC_BLE_STORAGE_LE_AUTH_MODE_STR, (int *)&auth_mode);
    if (ret == false) {
        //cannot find the key, just return SUCCESS, indicate already removed
        return BT_STATUS_SUCCESS;
    }

    ret = btc_config_remove(bdstr, BTC_BLE_STORAGE_LE_AUTH_MODE_STR);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    if (flush) {
        _btc_storage_save();
    }

    return  BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_remove_ble_dev_auth_mode(bt_bdaddr_t *remote_bd_addr, bool flush)
{
    bt_status_t ret;

    btc_config_lock();
    ret = _btc_storage_remove_ble_dev_auth_mode(remote_bd_addr, flush);
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

#if (BLE_INCLUDED == TRUE)
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
                int auth_mode = 0;
                if(_btc_storage_get_ble_dev_auth_mode(&bd_addr, &auth_mode) != BT_STATUS_SUCCESS) {
                    BTC_TRACE_WARNING("%s Failed to get auth mode from flash, please erase flash and download the firmware again", __func__);
                }
                BTA_DmAddBleDevice(bta_bd_addr, addr_type, auth_mode, BT_DEVICE_TYPE_BLE);
                *device_added = true;
            }

#if (!CONFIG_BT_STACK_NO_LOG)
            char bd_str[20] = {0};
#endif
            BTC_TRACE_DEBUG("%s() Adding key type %d for %s", __func__,
                key_type, bdaddr_to_string(&bd_addr, bd_str, sizeof(bd_str)));
            BTA_DmAddBleKey(bta_bd_addr, (tBTA_LE_KEY_VALUE *)buffer, key_type);
        }

        *key_found = true;
    }
}
bt_status_t _btc_storage_in_fetch_bonded_ble_device(const char *remote_bd_addr, int add)
{
    uint32_t device_type;
    int addr_type;
    bt_bdaddr_t bd_addr;
    BD_ADDR bta_bd_addr;
    bool device_added = false;
    bool key_found = false;

    if (!btc_config_get_int(remote_bd_addr, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type)) {
        BTC_TRACE_ERROR("%s, device_type = %x", __func__, device_type);
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
    } else {
        BTC_TRACE_DEBUG("Remote device:%s, no link key or ble key found", remote_bd_addr);
    }

    return BT_STATUS_FAIL;
}

bt_status_t btc_storage_get_bonded_ble_devices_list(esp_ble_bond_dev_t *bond_dev, int dev_num)
{
    bt_bdaddr_t bd_addr;
    char buffer[sizeof(tBTM_LE_KEY_VALUE)] = {0};

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {

        if (dev_num-- <= 0) {
            break;
        }
        uint32_t device_type = 0;
        const char *name = btc_config_section_name(iter);

        if (!string_is_bdaddr(name) ||
                !btc_config_get_int(name, BTC_BLE_STORAGE_DEV_TYPE_STR, (int *)&device_type) ||
                !(device_type & BT_DEVICE_TYPE_BLE)) {
            dev_num ++;
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
            tBTM_LE_PID_KEYS *pid_key = (tBTM_LE_PID_KEYS *) buffer;
            //Note: The memory size of the pid_key.addr_type in bond_key is different from that of (tBTM_LE_PID_KEYS) *pid_key.
            memcpy(&bond_dev->bond_key.pid_key.irk, pid_key->irk, ESP_BT_OCTET16_LEN);
            bond_dev->bond_key.pid_key.addr_type = pid_key->addr_type;
            memcpy(&bond_dev->bond_key.pid_key.static_addr, pid_key->static_addr, ESP_BD_ADDR_LEN);
        }
        //search for the next bond device
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

bt_status_t btc_storage_get_gatt_cl_supp_feat(bt_bdaddr_t *remote_bd_addr, uint8_t *value, int len)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = btc_config_get_bin(bdstr, BTC_BLE_STORAGE_GATT_CL_SUPP_FEAT_STR, value, (size_t *)&len);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_set_gatt_cl_supp_feat(bt_bdaddr_t *remote_bd_addr, uint8_t *value, int len)
{
    int ret;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr_t));
    ret = btc_config_set_bin(bdstr, BTC_BLE_STORAGE_GATT_CL_SUPP_FEAT_STR, value, (size_t)len);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_get_gatt_db_hash(bt_bdaddr_t *remote_bd_addr, uint8_t *value, int len)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = btc_config_get_bin(bdstr, BTC_BLE_STORAGE_GATT_DB_HASH_STR, value, (size_t *)&len);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_storage_set_gatt_db_hash(bt_bdaddr_t *remote_bd_addr, uint8_t *value, int len)
{
    int ret;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr_t));
    ret = btc_config_set_bin(bdstr, BTC_BLE_STORAGE_GATT_DB_HASH_STR, value, (size_t)len);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_remove_gatt_cl_supp_feat(bt_bdaddr_t *remote_bd_addr)
{
    bool ret = true;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    ret = btc_config_remove(bdstr, BTC_BLE_STORAGE_GATT_CL_SUPP_FEAT_STR);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    return  BT_STATUS_SUCCESS;
}

bt_status_t btc_storage_remove_gatt_db_hash(bt_bdaddr_t *remote_bd_addr)
{
    bool ret = true;
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));

    ret = btc_config_remove(bdstr, BTC_BLE_STORAGE_GATT_DB_HASH_STR);
    if (ret == false) {
        return BT_STATUS_FAIL;
    }

    return  BT_STATUS_SUCCESS;
}
#endif  ///BLE_INCLUDED == TRUE
#endif  ///SMP_INCLUDED == TRUE
