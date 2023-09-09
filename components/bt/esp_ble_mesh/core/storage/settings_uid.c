/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "mesh.h"
#include "rpl.h"
#include "mesh/main.h"
#include "mesh/common.h"
#include "settings_nvs.h"
#include "settings.h"
#include "transport.h"
#include "prov_pvnr.h"

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
/*
 * key: "mesh/uid" -> write/read to set/get all the user ids.
 * key: "mesh/id/xxxx" -> write/read to set/get the "xxxx" user id.
 */
#define SETTINGS_NVS_NAME_SIZE      15
#define SETTINGS_UID_SIZE           20
#define INVALID_SETTINGS_INDEX      0xFF
#define INVALID_SETTINGS_HANDLE     UINT32_MAX

static struct settings_uid {
    bool open;                              /* Indicate if settings is open */
    char id[SETTINGS_UID_SIZE + 1];         /* Settings user id */
    char name[SETTINGS_NVS_NAME_SIZE + 1];  /* Settings nvs namespace */
    bt_mesh_nvs_handle_t handle;            /* Core Settings nvs handle */
} user_ids[CONFIG_BLE_MESH_MAX_NVS_NAMESPACE];

static int settings_direct_erase(uint8_t index);

static inline bool settings_uid_empty(struct settings_uid *uid)
{
    return (uid->id[0] == '\0') ? true : false;
}

bt_mesh_nvs_handle_t get_core_settings_handle(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        if (user_ids[i].open) {
            return user_ids[i].handle;
        }
    }

    BT_ERR("No settings handle found");
    return INVALID_SETTINGS_HANDLE;
}

int settings_uid_init(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        memset(&user_ids[i], 0, sizeof(struct settings_uid));
        user_ids[i].handle = INVALID_SETTINGS_HANDLE;
    }

    return 0;
}

int settings_uid_load(void)
{
    struct net_buf_simple *buf = NULL;
    char name[16] = {'\0'};
    bool exist = false;
    size_t length = 0;
    int err = 0;
    int i;

    /* Before using user id to search settings, we need to
     * restore all the settings user_ids properly.
     */

    buf = bt_mesh_get_uid_settings_item("mesh/uid");
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t index = net_buf_simple_pull_le16(buf);

        sprintf(name, "mesh/id/%04x", index);

        err = bt_mesh_load_uid_settings(name, (uint8_t *)user_ids[index].id,
                                        SETTINGS_UID_SIZE, &exist);
        if (err) {
            continue;
        }

        if (exist == false) {
            continue;
        }

        BT_INFO("Restored settings %d, uid %s", index, user_ids[index].id);
    }

    bt_mesh_free_buf(buf);
    return err;
}

#if CONFIG_BLE_MESH_DEINIT
int settings_uid_deinit(bool erase)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        memset(&user_ids[i], 0, offsetof(struct settings_uid, handle));
        /* Can not reset handle here, since it will be used
         * in the settings_uid_erase().
         */
    }

    return 0;
}

int settings_uid_erase(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        if (user_ids[i].open == true) {
            /* When a nvs namespace is open, which means it is
             * being used currently. And its information will
             * be erased when the deinit function is invoked,
             * no need to erase it here.
             */
            bt_mesh_settings_nvs_close(user_ids[i].handle);
        } else if (settings_uid_empty(&user_ids[i]) == false) {
            /* When a user id is not empty, which means the nvs
             * namespace may contains mesh information, need to
             * erase it here.
             */
            settings_direct_erase(i);
        }

        user_ids[i].handle = INVALID_SETTINGS_HANDLE;
    }

    bt_mesh_erase_uid_settings("mesh/uid");
    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static int settings_direct_erase(uint8_t index)
{
    bt_mesh_nvs_handle_t handle = 0;
    char name[16] = {'\0'};
    int err = 0;

    sprintf(name, "%s_%02x", "mesh_core", index);

    /* Get handle for core settings */
    err = bt_mesh_settings_nvs_open(name, &handle);
    if (err) {
        BT_ERR("Open nvs failed, name %s, err %d", name, err);
        return -EIO;
    }

    /* Erase mesh information */
    err = bt_mesh_settings_erase_all(handle);
    if (err) {
        BT_ERR("Erase settings failed, index %d", index);
        return err;
    }

    bt_mesh_settings_nvs_close(handle);

    /* Erase settings user id */
    memset(name, 0, sizeof(name));
    sprintf(name, "mesh/id/%04x", index);

    bt_mesh_erase_uid_settings(name);
    bt_mesh_remove_uid_settings_item("mesh/uid", index);

    return 0;
}

static uint8_t settings_index_get(const char *id, uint8_t *index)
{
    uint8_t idx = 0;
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        if (strlen(user_ids[i].id) != strlen(id)) {
            continue;
        }

        if (!strncmp(user_ids[i].id, id, strlen(id))) {
            idx = i;
            break;
        }
    }

    if (i == ARRAY_SIZE(user_ids)) {
        idx = INVALID_SETTINGS_INDEX;
    }

    if (index) {
        *index = idx;
    }
    return idx;
}

static int settings_open(uint8_t index)
{
    struct settings_uid *uid = &user_ids[index];
    char name[16] = {'\0'};
    int err = 0;
    int i;

    /* Check if the nvs namespace is already open */
    if (uid->open == true) {
        BT_WARN("Settings already open, index %d", index);
        return -EALREADY;
    }

    /* Check if another nvs namespace is already open */
    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        if (i != index && user_ids[i].open == true) {
            BT_ERR("Settings %d is open, close it first", i);
            return -EBUSY;
        }
    }

    memset(uid->name, 0, sizeof(uid->name));
    sprintf(uid->name, "%s_%02x", "mesh_core", index);

    err = bt_mesh_settings_nvs_open(uid->name, &uid->handle);
    if (err) {
        BT_ERR("Open nvs failed, name %s, err %d", uid->name, err);
        return -EIO;
    }

    if (settings_uid_empty(uid)) {
        /* If the settings is not open with user id, then we use
         * the index as the user id. Or when the device restarts,
         * the user id may be restored, in this case the user id
         * shall not be updated.
         */
        sprintf(uid->id, "%04x", index);
    }

    BT_INFO("Open settings, index %d, uid %s", index, uid->id);

    sprintf(name, "mesh/id/%04x", index);
    err = bt_mesh_save_uid_settings(name, (const uint8_t *)uid->id, SETTINGS_UID_SIZE);
    if (err) {
        BT_ERR("Save uid failed, name %s", name);
        return err;
    }

    err = bt_mesh_add_uid_settings_item("mesh/uid", index);
    if (err) {
        BT_ERR("Add uid failed, index %d", index);
        return err;
    }

    /* Mark this as open here, because we need this flag for
     * finding nvs handle before restoring mesh information.
     */
    uid->open = true;

    err = settings_core_load();
    if (err) {
        BT_ERR("Load settings failed, name %s", uid->name);
        return err;
    }

    err = settings_core_commit();
    if (err) {
        BT_ERR("Commit settings failed, name %s", uid->name);
        return err;
    }

    return 0;
}

int bt_mesh_provisioner_open_settings_with_index(uint8_t index)
{
    if (index >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Invalid settings index %d", index);
        return -EINVAL;
    }

    return settings_open(index);
}

int bt_mesh_provisioner_open_settings_with_uid(const char *id, uint8_t *index)
{
    uint8_t idx = 0;
    int i;

    if (!id || strlen(id) > SETTINGS_UID_SIZE) {
        BT_ERR("Invalid settings uid");
        return -EINVAL;
    }

    idx = settings_index_get(id, index);

    /* If user id not exists, try to add this as a new one. */
    if (idx >= ARRAY_SIZE(user_ids)) {
        for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
            if (settings_uid_empty(&user_ids[i])) {
                strncpy(user_ids[i].id, id, SETTINGS_UID_SIZE);
                if (index) {
                    *index = i;
                }
                break;
            }
        }

        if (i == ARRAY_SIZE(user_ids)) {
            BT_ERR("Settings uid is full!");
            return -ENOMEM;
        }

        idx = i;
    }

    return settings_open(idx);
}

static int settings_close(uint8_t index, bool erase)
{
    struct settings_uid *uid = &user_ids[index];
    char name[16] = {'\0'};
    int err = 0;

    if (uid->open == false) {
        BT_ERR("Settings not open, index %d", index);
        return -EIO;
    }

    BT_INFO("Close settings, index %d, uid %s", index, uid->id);

    /* Disable Provisioner firstly */
    err = bt_mesh_provisioner_disable(BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT);
    if (err && err != -EALREADY) {
        BT_ERR("Disable Provisioner failed (err %d)", err);
        return err;
    }

    /* Reset (and erase) mesh information */
    bt_mesh_provisioner_prov_reset(erase);
    bt_mesh_provisioner_main_reset(erase);
    bt_mesh_net_reset();
    bt_mesh_rx_reset();
    bt_mesh_tx_reset();
    bt_mesh_rpl_reset(erase);
    bt_mesh_settings_reset(erase);

    if (erase == true) {
        /* Erase and reset settings user id */
        sprintf(name, "mesh/id/%04x", index);

        bt_mesh_erase_uid_settings(name);
        bt_mesh_remove_uid_settings_item("mesh/uid", index);

        memset(uid->id, 0, sizeof(uid->id));
    }

    bt_mesh_settings_nvs_close(uid->handle);
    uid->open = false;

    return 0;
}

int bt_mesh_provisioner_close_settings_with_index(uint8_t index, bool erase)
{
    if (index >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Invalid settings index %d", index);
        return -EINVAL;
    }

    return settings_close(index, erase);
}

int bt_mesh_provisioner_close_settings_with_uid(const char *id, bool erase, uint8_t *index)
{
    uint8_t idx = 0;

    if (!id || strlen(id) > SETTINGS_UID_SIZE) {
        BT_ERR("Invalid settings uid");
        return -EINVAL;
    }

    idx = settings_index_get(id, index);
    if (idx >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Settings uid %s not exists", id);
        return -ENODEV;
    }

    return settings_close(idx, erase);
}

static int settings_delete(uint8_t index)
{
    /* The function is used to erase mesh information from
     * the nvs namespace when it is not open and restored,
     * and delete the corresponding user id.
     */
    struct settings_uid *uid = &user_ids[index];

    if (uid->open == true) {
        BT_ERR("Settings being used, index %d", index);
        return -EBUSY;
    }

    BT_INFO("Delete settings, index %d, uid %s", index, uid->id);

    settings_direct_erase(index);

    memset(uid, 0, sizeof(struct settings_uid));
    uid->handle = INVALID_SETTINGS_HANDLE;

    return 0;
}

int bt_mesh_provisioner_delete_settings_with_index(uint8_t index)
{
    if (index >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Invalid settings index %d", index);
        return -EINVAL;
    }

    return settings_delete(index);
}

int bt_mesh_provisioner_delete_settings_with_uid(const char *id, uint8_t *index)
{
    uint8_t idx = 0;

    if (!id || strlen(id) > SETTINGS_UID_SIZE) {
        BT_ERR("Invalid settings uid");
        return -EINVAL;
    }

    idx = settings_index_get(id, index);
    if (idx >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Settings uid %s not exists", id);
        return -ENODEV;
    }

    return settings_delete(idx);
}

const char *bt_mesh_provisioner_get_settings_uid(uint8_t index)
{
    if (index >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Invalid settings index %d", index);
        return NULL;
    }

    return user_ids[index].id;
}

uint8_t bt_mesh_provisioner_get_settings_index(const char *id)
{
    uint8_t idx = 0;

    if (!id || strlen(id) > SETTINGS_UID_SIZE) {
        BT_ERR("Invalid settings uid");
        return INVALID_SETTINGS_INDEX;
    }

    idx = settings_index_get(id, NULL);
    if (idx >= ARRAY_SIZE(user_ids)) {
        BT_ERR("Settings uid %s not exists", id);
    }

    return idx;
}

uint8_t bt_mesh_provisioner_get_free_settings_count(void)
{
    uint8_t count = 0;
    int i;

    for (i = 0; i < ARRAY_SIZE(user_ids); i++) {
        if (settings_uid_empty(&user_ids[i])) {
            count++;
        }
    }

    return count;
}
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */

#if CONFIG_BLE_MESH_SETTINGS && CONFIG_BLE_MESH_PROVISIONER
int bt_mesh_provisioner_direct_erase_settings(void)
{
    bt_mesh_nvs_handle_t handle = 0;
    int err = 0;

    err = bt_mesh_settings_direct_open(&handle);
    if (err) {
        return err;
    }

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
    for (int i = 0; i < ARRAY_SIZE(user_ids); i++) {
        settings_direct_erase(i);
    }

    bt_mesh_erase_uid_settings("mesh/uid");
#else
    err = bt_mesh_settings_erase_all(handle);
#endif

    bt_mesh_settings_direct_close();
    return err;
}
#endif /* CONFIG_BLE_MESH_SETTINGS && CONFIG_BLE_MESH_PROVISIONER */
