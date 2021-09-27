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

#include "btc/btc_storage.h"
#include "btc/btc_util.h"
#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_trace.h"
#include "esp_system.h"
#include "bta/bta_api.h"
#include "device/bdaddr.h"
#include "btc/btc_config.h"
#include "btc_hh.h"

/*******************************************************************************
**
** Function         btc_storage_add_bonded_device
**
** Description      BTIF storage API - Adds the newly bonded device to NVRAM
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
        BOOLEAN sc_support)
{
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    BTC_TRACE_DEBUG("add to storage: Remote device:%s\n", bdstr);

    btc_config_lock();
    int ret = btc_config_set_int(bdstr, BTC_STORAGE_LINK_KEY_TYPE_STR, (int)key_type);
    ret &= btc_config_set_int(bdstr, BTC_STORAGE_PIN_LENGTH_STR, (int)pin_length);
    ret &= btc_config_set_bin(bdstr, BTC_STORAGE_LINK_KEY_STR, link_key, sizeof(LINK_KEY));
    ret &= btc_config_set_bin(bdstr, BTC_STORAGE_SC_SUPPORT, (uint8_t *)&sc_support, sizeof(sc_support));
    /* write bonded info immediately */
    btc_config_flush();
    btc_config_unlock();

    BTC_TRACE_DEBUG("Storage add rslt %d\n", ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_in_fetch_bonded_devices
**
** Description      Internal helper function to fetch the bonded devices
**                  from NVRAM
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
static bt_status_t btc_in_fetch_bonded_devices(int add)
{
    BOOLEAN bt_linkkey_file_found = FALSE;
    UINT8 sc_support = 0;

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end(); iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (!string_is_bdaddr(name)) {
            continue;
        }

        BTC_TRACE_DEBUG("Remote device:%s\n", name);
        LINK_KEY link_key;
        size_t size = sizeof(link_key);
        if (btc_config_get_bin(name, BTC_STORAGE_LINK_KEY_STR, link_key, &size)) {
            int linkkey_type;
            if (btc_config_get_int(name, BTC_STORAGE_LINK_KEY_TYPE_STR, &linkkey_type)) {
                bt_bdaddr_t bd_addr;
                string_to_bdaddr(name, &bd_addr);
                if (add) {
                    DEV_CLASS dev_class = {0, 0, 0};
                    int cod;
                    int pin_length = 0;
                    if (btc_config_get_int(name, BTC_STORAGE_DEV_CLASS_STR, &cod)) {
                        uint2devclass((UINT32)cod, dev_class);
                    }
                    btc_config_get_int(name, BTC_STORAGE_PIN_LENGTH_STR, &pin_length);
                    size = sizeof(sc_support);
                    btc_config_get_bin(name, BTC_STORAGE_SC_SUPPORT, &sc_support, &size);
#if (SMP_INCLUDED == TRUE)
                    BTA_DmAddDevice(bd_addr.address, dev_class, link_key, 0, 0,
                                    (UINT8)linkkey_type, 0, pin_length, (UINT8)sc_support);
#endif  ///SMP_INCLUDED == TRUE
                }
                bt_linkkey_file_found = TRUE;
            } else {
                BTC_TRACE_ERROR("bounded device:%s, LinkKeyType or PinLength is invalid\n", name);
            }
        }
        if (!bt_linkkey_file_found) {
            BTC_TRACE_DEBUG("Remote device:%s, no link key\n", name);
        }
    }
    btc_config_unlock();

    return BT_STATUS_SUCCESS;
}


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
bt_status_t btc_storage_load_bonded_devices(void)
{
    bt_status_t status;
    status = btc_in_fetch_bonded_devices(1);
    BTC_TRACE_DEBUG("Storage load rslt %d\n", status);
    return status;
}

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
bt_status_t btc_storage_remove_bonded_device(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = 1;
    BTC_TRACE_DEBUG("Add to storage: Remote device:%s\n", bdstr);

    btc_config_lock();
    if (btc_config_exist(bdstr, BTC_STORAGE_LINK_KEY_TYPE_STR)) {
        ret &= btc_config_remove(bdstr, BTC_STORAGE_LINK_KEY_TYPE_STR);
    }
    if (btc_config_exist(bdstr, BTC_STORAGE_PIN_LENGTH_STR)) {
        ret &= btc_config_remove(bdstr, BTC_STORAGE_PIN_LENGTH_STR);
    }
    if (btc_config_exist(bdstr, BTC_STORAGE_LINK_KEY_STR)) {
        ret &= btc_config_remove(bdstr, BTC_STORAGE_LINK_KEY_STR);
    }
    if (btc_config_exist(bdstr, BTC_STORAGE_SC_SUPPORT)) {
        ret &= btc_config_remove(bdstr, BTC_STORAGE_SC_SUPPORT);
    }
    /* write bonded info immediately */
    btc_config_flush();
    btc_config_unlock();

    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btc_storage_get_num_bt_bond_devices
**
** Description      BTC storage API - get the num of the bonded device from NVRAM
**
** Returns          the num of the bonded device
**
*******************************************************************************/
int btc_storage_get_num_bt_bond_devices(void)
{
    int num_dev = 0;

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (string_is_bdaddr(name) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_TYPE_STR) &&
            btc_config_exist(name, BTC_STORAGE_PIN_LENGTH_STR) &&
            btc_config_exist(name, BTC_STORAGE_SC_SUPPORT) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_STR)) {
            num_dev++;
        }
    }
    btc_config_unlock();
    return num_dev;
}

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
bt_status_t btc_storage_get_bonded_bt_devices_list(bt_bdaddr_t *bond_dev, int *dev_num)
{
    bt_bdaddr_t bd_addr;
    int in_dev_num = *dev_num; /* buffer size */
    int out_dev_num = 0; /* bond_dev size */

    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
            iter = btc_config_section_next(iter)) {

        if (in_dev_num <= 0) {
            break;
        }

        const char *name = btc_config_section_name(iter);

        if (string_is_bdaddr(name) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_TYPE_STR) &&
            btc_config_exist(name, BTC_STORAGE_PIN_LENGTH_STR) &&
            btc_config_exist(name, BTC_STORAGE_SC_SUPPORT) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_STR)) {
            string_to_bdaddr(name, &bd_addr);
            memcpy(bond_dev, &bd_addr, sizeof(bt_bdaddr_t));
            in_dev_num--;
            out_dev_num++;
            bond_dev++;
        }
    }
    *dev_num = out_dev_num; /* out_dev_num <= in_dev_num */
    btc_config_unlock();

    return BT_STATUS_SUCCESS;
}

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
                                             uint16_t dl_len, uint8_t *dsc_list)
{
    BTC_TRACE_DEBUG("btc_storage_add_hid_device_info:");
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    btc_config_lock();
    int ret = btc_config_set_int(bdstr, "HidAttrMask", attr_mask);
    ret &= btc_config_set_int(bdstr, "HidSubClass", sub_class);
    ret &= btc_config_set_int(bdstr, "HidAppId", app_id);
    ret &= btc_config_set_int(bdstr, "HidVendorId", vendor_id);
    ret &= btc_config_set_int(bdstr, "HidProductId", product_id);
    ret &= btc_config_set_int(bdstr, "HidVersion", version);
    ret &= btc_config_set_int(bdstr, "HidCountryCode", ctry_code);
    ret &= btc_config_set_int(bdstr, "HidSSRMaxLatency", ssr_max_latency);
    ret &= btc_config_set_int(bdstr, "HidSSRMinTimeout", ssr_min_tout);
    if (dl_len > 0)
        btc_config_set_bin(bdstr, "HidDescriptor", dsc_list, dl_len);
    btc_config_flush();
    btc_config_unlock();

    BTC_TRACE_DEBUG("Storage add hid device info %d\n", ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

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
bt_status_t btc_storage_load_bonded_hid_info(void)
{
    int value;
    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
         iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (string_is_bdaddr(name) && btc_config_exist(name, BTC_STORAGE_LINK_KEY_TYPE_STR) &&
            btc_config_exist(name, BTC_STORAGE_PIN_LENGTH_STR) && btc_config_exist(name, BTC_STORAGE_SC_SUPPORT) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_STR) && btc_config_exist(name, "HidAttrMask")) {
            btc_config_get_int(name, "HidAttrMask", &value);
            uint16_t attr_mask = (uint16_t)value;

            tBTA_HH_DEV_DSCP_INFO dscp_info;
            memset(&dscp_info, 0, sizeof(dscp_info));

            btc_config_get_int(name, "HidSubClass", &value);
            uint8_t sub_class = (uint8_t)value;

            btc_config_get_int(name, "HidAppId", &value);
            uint8_t app_id = (uint8_t)value;

            btc_config_get_int(name, "HidVendorId", &value);
            dscp_info.vendor_id = (uint16_t)value;

            btc_config_get_int(name, "HidProductId", &value);
            dscp_info.product_id = (uint16_t)value;

            btc_config_get_int(name, "HidVersion", &value);
            dscp_info.version = (uint8_t)value;

            btc_config_get_int(name, "HidCountryCode", &value);
            dscp_info.ctry_code = (uint8_t)value;

            value = 0;
            btc_config_get_int(name, "HidSSRMaxLatency", &value);
            dscp_info.ssr_max_latency = (uint16_t)value;

            value = 0;
            btc_config_get_int(name, "HidSSRMinTimeout", &value);
            dscp_info.ssr_min_tout = (uint16_t)value;

            size_t len = btc_config_get_bin_length(name, "HidDescriptor");
            if (len > 0) {
                dscp_info.descriptor.dl_len = (uint16_t)len;
                dscp_info.descriptor.dsc_list = (uint8_t *)osi_malloc(len);
                btc_config_get_bin(name, "HidDescriptor", (uint8_t *)dscp_info.descriptor.dsc_list, &len);
            }

            // add extracted information to BTA HH
            bt_bdaddr_t bd_addr;
            if (string_to_bdaddr(name, &bd_addr) && btc_hh_add_added_dev(*(BD_ADDR *)&bd_addr, attr_mask)) {
                BTA_HhAddDev(*(BD_ADDR *)&bd_addr, attr_mask, sub_class, app_id, dscp_info);
            }

            if (dscp_info.descriptor.dsc_list) {
                osi_free(dscp_info.descriptor.dsc_list);
                dscp_info.descriptor.dsc_list = NULL;
            }
        }
    }
    btc_config_unlock();
    return BT_STATUS_SUCCESS;
}

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
bt_status_t btc_storage_remove_hid_info(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    int ret = 1;
    btc_config_lock();
    if (btc_config_exist(bdstr, "HidAttrMask")) {
        ret &= btc_config_remove(bdstr, "HidAttrMask");
    }
    if (btc_config_exist(bdstr, "HidSubClass")) {
        ret &= btc_config_remove(bdstr, "HidSubClass");
    }
    if (btc_config_exist(bdstr, "HidAppId")) {
        ret &= btc_config_remove(bdstr, "HidAppId");
    }
    if (btc_config_exist(bdstr, "HidVendorId")) {
        ret &= btc_config_remove(bdstr, "HidVendorId");
    }
    if (btc_config_exist(bdstr, "HidProductId")) {
        ret &= btc_config_remove(bdstr, "HidProductId");
    }
    if (btc_config_exist(bdstr, "HidVersion")) {
        ret &= btc_config_remove(bdstr, "HidVersion");
    }
    if (btc_config_exist(bdstr, "HidCountryCode")) {
        ret &= btc_config_remove(bdstr, "HidCountryCode");
    }
    if (btc_config_exist(bdstr, "HidSSRMaxLatency")) {
        ret &= btc_config_remove(bdstr, "HidSSRMaxLatency");
    }
    if (btc_config_exist(bdstr, "HidSSRMinTimeout")) {
        ret &= btc_config_remove(bdstr, "HidSSRMinTimeout");
    }
    if (btc_config_exist(bdstr, "HidDescriptor")) {
        ret &= btc_config_remove(bdstr, "HidDescriptor");
    }
    btc_config_flush();
    btc_config_unlock();
    BTC_TRACE_DEBUG("%s ret:%d", __func__, ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}
#endif //(defined BTC_HH_INCLUDED && BTC_HH_INCLUDED == TRUE)

#if (defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE)
#include "bta/bta_hd_api.h"
/*******************************************************************************
 * Function         btc_storage_load_hidd
 *
 * Description      Loads hidd bonded device and "plugs" it into hidd
 *
 * Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
 *
 ******************************************************************************/
bt_status_t btc_storage_load_hidd(void)
{
    bt_bdaddr_t bd_addr;
    int value;
    btc_config_lock();
    for (const btc_config_section_iter_t *iter = btc_config_section_begin(); iter != btc_config_section_end();
         iter = btc_config_section_next(iter)) {
        const char *name = btc_config_section_name(iter);
        if (string_is_bdaddr(name) && btc_config_exist(name, BTC_STORAGE_LINK_KEY_TYPE_STR) &&
            btc_config_exist(name, BTC_STORAGE_PIN_LENGTH_STR) && btc_config_exist(name, BTC_STORAGE_SC_SUPPORT) &&
            btc_config_exist(name, BTC_STORAGE_LINK_KEY_STR)) {
            BTC_TRACE_DEBUG("Remote device:%s", name);
            if (btc_config_get_int(name, "HidDeviceCabled", &value)) {
                string_to_bdaddr(name, &bd_addr);
                BTA_HdAddDevice(bd_addr.address);
                break;
            }
        }
    }
    btc_config_unlock();
    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
 *
 * Function         btc_storage_set_hidd
 *
 * Description      Stores hidd bonded device info in nvram.
 *
 * Returns          BT_STATUS_SUCCESS
 *
 ******************************************************************************/
bt_status_t btc_storage_set_hidd(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr = {0};
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    btc_config_lock();
    int ret = btc_config_set_int(bdstr, "HidDeviceCabled", 1);
    btc_config_flush();
    btc_config_unlock();
    BTC_TRACE_DEBUG("%s ret:%d", __func__, ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
 *
 * Function         btc_storage_remove_hidd
 *
 * Description      Removes hidd bonded device info from nvram
 *
 * Returns          BT_STATUS_SUCCESS
 *
 ******************************************************************************/
bt_status_t btc_storage_remove_hidd(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr;
    int ret = 0;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    btc_config_lock();
    if (btc_config_exist(bdstr, "HidVersion")) {
        ret = btc_config_remove(bdstr, "HidDeviceCabled");
    }
    btc_config_flush();
    btc_config_unlock();

    BTC_TRACE_DEBUG("%s ret:%d", __func__, ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}
#endif //(defined BTC_HD_INCLUDED && BTC_HD_INCLUDED == TRUE)
