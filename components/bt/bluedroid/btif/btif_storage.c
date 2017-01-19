#include "btif_storage.h"
#include "btif_util.h"
#include "osi.h"
#include "bt_trace.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "bta_api.h"
#include "bdaddr.h"
#include "btif_config.h"
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
        uint8_t pin_length)
{
    bdstr_t bdstr;

    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    BTIF_TRACE_WARNING("add to storage: Remote device:%s", bdstr);

    int ret = btif_config_set_int(bdstr, "LinkKeyType", (int)key_type);
    BTIF_TRACE_WARNING("p1\n");
    ret &= btif_config_set_int(bdstr, "PinLength", (int)pin_length);
    BTIF_TRACE_WARNING("p2\n");
    ret &= btif_config_set_bin(bdstr, "LinkKey", link_key, sizeof(LINK_KEY));
    BTIF_TRACE_WARNING("p3\n");
    /* write bonded info immediately */
    // karl
    btif_config_flush();
    BTIF_TRACE_WARNING("Storage add rslt %d\n", ret);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

/*******************************************************************************
**
** Function         btif_in_fetch_bonded_devices
**
** Description      Internal helper function to fetch the bonded devices
**                  from NVRAM
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
static bt_status_t btif_in_fetch_bonded_devices(int add)
{
    BOOLEAN bt_linkkey_file_found = FALSE;
    int device_type;

    for (const btif_config_section_iter_t *iter = btif_config_section_begin(); iter != btif_config_section_end(); iter = btif_config_section_next(iter)) {
        const char *name = btif_config_section_name(iter);
        if (!string_is_bdaddr(name)) {
            continue;
        }

        BTIF_TRACE_WARNING("Remote device:%s", name);
        LINK_KEY link_key;
        size_t size = sizeof(link_key);
        if (btif_config_get_bin(name, "LinkKey", link_key, &size)) {
            int linkkey_type;
            if (btif_config_get_int(name, "LinkKeyType", &linkkey_type)) {
                //int pin_len;
                //btif_config_get_int(name, "PinLength", &pin_len))
                bt_bdaddr_t bd_addr;
                string_to_bdaddr(name, &bd_addr);
                if (add) {
                    DEV_CLASS dev_class = {0, 0, 0};
                    int cod;
                    int pin_length = 0;
                    if (btif_config_get_int(name, "DevClass", &cod)) {
                        uint2devclass((UINT32)cod, dev_class);
                    }
                    btif_config_get_int(name, "PinLength", &pin_length);
                    BTA_DmAddDevice(bd_addr.address, dev_class, link_key, 0, 0,
                                    (UINT8)linkkey_type, 0, pin_length);
                }
                bt_linkkey_file_found = TRUE;
            } else {
                BTIF_TRACE_ERROR("bounded device:%s, LinkKeyType or PinLength is invalid", name);
            }
        }
        if (!bt_linkkey_file_found) {
            BTIF_TRACE_EVENT("Remote device:%s, no link key", name);
        }
    }
    return BT_STATUS_SUCCESS;
#if 0
    int device_type;

    BTIF_TRACE_WARNING("fetch from storage");
    do {
        const char *name = "bt_host";

        bt_bdaddr_t bd_addr;
        size_t size = sizeof(bt_bdaddr_t);
        if (!btif_config_get_bin(name, "BdAddr", bd_addr.address, &size)) {
            continue;
        } else {
            bdstr_t bdstr;
            bdaddr_to_string(&bd_addr, bdstr, sizeof(bdstr));
            BTIF_TRACE_WARNING("fetch from storage: Remote device:%s\n", bdstr);
        }

        LINK_KEY link_key;
        size = sizeof(link_key);
        if (btif_config_get_bin(name, "LinkKey", link_key, &size)) {
            int linkkey_type;
            if (btif_config_get_int(name, "LinkKeyType", &linkkey_type)) {
                if (add) {
                    DEV_CLASS dev_class = {0, 0, 0};
                    int cod;
                    int pin_length = 0;
                    if (btif_config_get_int(name, "DevClass", &cod)) {
                        uint2devclass((UINT32)cod, dev_class);
                    }
                    btif_config_get_int(name, "PinLength", &pin_length);
                    BTA_DmAddDevice(bd_addr.address, dev_class, link_key, 0, 0,
                                    (UINT8)linkkey_type, 0, pin_length);

                }
            } else {
                BTIF_TRACE_ERROR("bounded device:%s, LinkKeyType or PinLength is invalid\n", name);
            }
        }
    } while (0);
    return BT_STATUS_SUCCESS;
#endif
}


/*******************************************************************************
**
** Function         btif_storage_load_bonded_devices
**
** Description      BTIF storage API - Loads all the bonded devices from NVRAM
**                  and adds to the BTA.
**                  Additionally, this API also invokes the adaper_properties_cb
**                  and remote_device_properties_cb for each of the bonded devices.
**
** Returns          BT_STATUS_SUCCESS if successful, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_storage_load_bonded_devices(void)
{
    bt_status_t status;
    status = btif_in_fetch_bonded_devices(1);
    BTIF_TRACE_WARNING("Storage load rslt %d\n", status);
    return status;
}

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
bt_status_t btif_storage_remove_bonded_device(bt_bdaddr_t *remote_bd_addr)
{
    bdstr_t bdstr;
    bdaddr_to_string(remote_bd_addr, bdstr, sizeof(bdstr));
    BTIF_TRACE_EVENT("add to storage: Remote device:%s\n", bdstr);

    int ret = 1;
    if (btif_config_exist(bdstr, "LinkKeyType")) {
        ret &= btif_config_remove(bdstr, "LinkKeyType");
    }
    if (btif_config_exist(bdstr, "PinLength")) {
        ret &= btif_config_remove(bdstr, "PinLength");
    }
    if (btif_config_exist(bdstr, "LinkKey")) {
        ret &= btif_config_remove(bdstr, "LinkKey");
    }
    /* write bonded info immediately */
    btif_config_flush();
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}
