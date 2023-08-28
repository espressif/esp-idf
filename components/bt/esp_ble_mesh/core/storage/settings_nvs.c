/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh/common.h"
#include "settings_uid.h"
#include "settings.h"

#if CONFIG_BLE_MESH_SETTINGS

enum settings_type {
    SETTINGS_CORE,
#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
    SETTINGS_UID,
#endif
};

struct settings_context {
    char *nvs_name;
    bt_mesh_nvs_handle_t handle;

    int (*settings_init)(void);
    int (*settings_load)(void);
    int (*settings_commit)(void);
#if CONFIG_BLE_MESH_DEINIT
    int (*settings_deinit)(void);
    int (*settings_erase)(void);
#endif /* CONFIG_BLE_MESH_DEINIT */
};

static struct settings_context settings_ctx[] = {
    [SETTINGS_CORE] = {
        .nvs_name = "mesh_core",
        .settings_init = settings_core_init,
        .settings_load = settings_core_load,
        .settings_commit = settings_core_commit,
#if CONFIG_BLE_MESH_DEINIT
        .settings_deinit = settings_core_deinit,
        .settings_erase = settings_core_erase,
#endif /* CONFIG_BLE_MESH_DEINIT */
    },
#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
    [SETTINGS_UID] = {
        .nvs_name = "mesh_user_id",
        .settings_init = settings_uid_init,
        .settings_load = settings_uid_load,
        .settings_commit = NULL,
#if CONFIG_BLE_MESH_DEINIT
        .settings_deinit = settings_uid_deinit,
        .settings_erase = settings_uid_erase,
#endif /* CONFIG_BLE_MESH_DEINIT */
    },
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */
};

/* API used to initialize, load and commit BLE Mesh related settings */

int bt_mesh_settings_nvs_open(const char* name, bt_mesh_nvs_handle_t *handle)
{
#if CONFIG_BLE_MESH_SPECIFIC_PARTITION
    return nvs_open_from_partition(CONFIG_BLE_MESH_PARTITION_NAME, name, NVS_READWRITE, handle);
#else
    return nvs_open(name, NVS_READWRITE, handle);
#endif
}

void bt_mesh_settings_nvs_close(bt_mesh_nvs_handle_t handle)
{
    nvs_close(handle);
}

void bt_mesh_settings_init_foreach(void)
{
    int err = 0;
    int i;

#if CONFIG_BLE_MESH_SPECIFIC_PARTITION
    err = nvs_flash_init_partition(CONFIG_BLE_MESH_PARTITION_NAME);
    if (err != ESP_OK) {
        BT_ERR("Init mesh partition failed, name %s, err %d", CONFIG_BLE_MESH_PARTITION_NAME, err);
        return;
    }
#endif /* CONFIG_BLE_MESH_SPECIFIC_PARTITION */

    for (i = 0; i < ARRAY_SIZE(settings_ctx); i++) {
        struct settings_context *ctx = &settings_ctx[i];

        /* Settings initialization is always needed. */
        if (ctx->settings_init && ctx->settings_init()) {
            BT_ERR("Init settings failed, name %s", ctx->nvs_name);
            return;
        }

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
        /* If multiple nvs namespace functionality is enabled,
         * no need to perform the following operations during
         * initialization. And they will be performed when the
         * application layer tries to open settings.
         */
        if (i != SETTINGS_UID) {
            continue;
        }
#endif /* CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE */

        err = bt_mesh_settings_nvs_open(ctx->nvs_name, &ctx->handle);
        if (err) {
            BT_ERR("Open nvs failed, name %s, err %d", ctx->nvs_name, err);
            return;
        }

        if (ctx->settings_load && ctx->settings_load()) {
            BT_ERR("Load settings failed, name %s", ctx->nvs_name);
            return;
        }

        /* If not using multiple nvs namespaces, we will follow the normal
         * procedure, i.e. restoring all the mesh information.
         * If using multiple nvs namespaces, we will only restore user_id.
         */
        if (ctx->settings_commit && ctx->settings_commit()) {
            BT_ERR("Commit settings failed, name %s", ctx->nvs_name);
            return;
        }
    }
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_settings_deinit_foreach(bool erase)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(settings_ctx); i++) {
        struct settings_context *ctx = &settings_ctx[i];

        if (ctx->settings_deinit && ctx->settings_deinit()) {
            BT_ERR("Deinit settings failed, name %s", ctx->nvs_name);
            continue;
        }

        if (erase && ctx->settings_erase && ctx->settings_erase()) {
            BT_ERR("Erase settings failed, name %s", ctx->nvs_name);
            continue;
        }

        bt_mesh_settings_nvs_close(ctx->handle);
    }

#if CONFIG_BLE_MESH_SPECIFIC_PARTITION
    nvs_flash_deinit_partition(CONFIG_BLE_MESH_PARTITION_NAME);
#endif
}
#endif /* CONFIG_BLE_MESH_DEINIT */

int bt_mesh_settings_direct_open(bt_mesh_nvs_handle_t *handle)
{
    int err = 0;
    int i;

#if CONFIG_BLE_MESH_SPECIFIC_PARTITION
    err = nvs_flash_init_partition(CONFIG_BLE_MESH_PARTITION_NAME);
    if (err != ESP_OK) {
        BT_ERR("Init mesh partition failed, name %s, err %d", CONFIG_BLE_MESH_PARTITION_NAME, err);
        return -EIO;
    }
#endif /* CONFIG_BLE_MESH_SPECIFIC_PARTITION */

    for (i = 0; i < ARRAY_SIZE(settings_ctx); i++) {
        struct settings_context *ctx = &settings_ctx[i];

        err = bt_mesh_settings_nvs_open(ctx->nvs_name, &ctx->handle);
        if (err) {
            BT_ERR("Open nvs failed, name %s, err %d", ctx->nvs_name, err);
            return -EIO;
        }

        if (i == SETTINGS_CORE && handle) {
            *handle = ctx->handle;
        }
    }

    return 0;
}

void bt_mesh_settings_direct_close(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(settings_ctx); i++) {
        bt_mesh_settings_nvs_close(settings_ctx[i].handle);
    }

#if CONFIG_BLE_MESH_SPECIFIC_PARTITION
    nvs_flash_deinit_partition(CONFIG_BLE_MESH_PARTITION_NAME);
#endif
}

/* API used to get BLE Mesh related nvs handle */

static inline bt_mesh_nvs_handle_t settings_get_nvs_handle(enum settings_type type)
{
#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
    if (type == SETTINGS_CORE) {
        extern bt_mesh_nvs_handle_t get_core_settings_handle(void);
        return get_core_settings_handle();
    }
#endif
    return settings_ctx[type].handle;
}

/* API used to store/erase BLE Mesh related settings */

static int settings_save(bt_mesh_nvs_handle_t handle, const char *key, const uint8_t *val, size_t len)
{
    int err = 0;

    if (key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("nvs %s, key %s", val ? "set" : "erase", key);

    if (val) {
        err = nvs_set_blob(handle, key, val, len);
    } else {
        err = nvs_erase_key(handle, key);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            BT_DBG("%s not exists", key);
            return 0;
        }
    }
    if (err != ESP_OK) {
        BT_ERR("Failed to %s %s data (err %d)",
                val ? "set" : "erase", key, err);
        return -EIO;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        BT_ERR("Failed to commit settings (err %d)", err);
        return -EIO;
    }

    return 0;
}

int bt_mesh_save_settings(bt_mesh_nvs_handle_t handle, const char *key,
                          const uint8_t *val, size_t len)
{
    int err = 0;
    bt_mesh_settings_lock();
    err = settings_save(handle, key, val, len);
    bt_mesh_settings_unlock();
    return err;
}

int bt_mesh_save_core_settings(const char *key, const uint8_t *val, size_t len)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_CORE);
    return bt_mesh_save_settings(handle, key, val, len);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int bt_mesh_save_uid_settings(const char *key, const uint8_t *val, size_t len)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_UID);
    return bt_mesh_save_settings(handle, key, val, len);
}
#endif

int bt_mesh_erase_settings(bt_mesh_nvs_handle_t handle, const char *key)
{
    return bt_mesh_save_settings(handle, key, NULL, 0);
}

int bt_mesh_erase_core_settings(const char *key)
{
    return bt_mesh_save_core_settings(key, NULL, 0);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int bt_mesh_erase_uid_settings(const char *name)
{
    return bt_mesh_save_uid_settings(name, NULL, 0);
}
#endif

/* API used to load BLE Mesh related settings */

static int settings_load(bt_mesh_nvs_handle_t handle, const char *key,
                         uint8_t *buf, size_t buf_len, bool *exist)
{
    int err = 0;

    if (key == NULL || buf == NULL || exist == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    err = nvs_get_blob(handle, key, buf, &buf_len);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            BT_DBG("Settings %s not found", key);
            *exist = false;
            return 0;
        }

        BT_ERR("Failed to get %s data (err %d)", key, err);
        return -EIO;
    }

    *exist = true;
    return 0;
}

int bt_mesh_load_settings(bt_mesh_nvs_handle_t handle, const char *key,
                          uint8_t *buf, size_t buf_len, bool *exist)
{
    int err = 0;
    bt_mesh_settings_lock();
    err = settings_load(handle, key, buf, buf_len, exist);
    bt_mesh_settings_unlock();
    return err;
}

int bt_mesh_load_core_settings(const char *key, uint8_t *buf, size_t buf_len, bool *exist)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_CORE);
    return bt_mesh_load_settings(handle, key, buf, buf_len, exist);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int bt_mesh_load_uid_settings(const char *key, uint8_t *buf, size_t buf_len, bool *exist)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_UID);
    return bt_mesh_load_settings(handle, key, buf, buf_len, exist);
}
#endif

/* API used to get length of BLE Mesh related settings */

static size_t settings_get_length(bt_mesh_nvs_handle_t handle, const char *key)
{
    size_t len = 0U;
    int err = 0;

    if (key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return 0;
    }

    err = nvs_get_blob(handle, key, NULL, &len);
    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            BT_ERR("Failed to get %s length (err %d)", key, err);
        }
        return 0;
    }

    return len;
}

/* API used to get BLE Mesh related items. Here items mean model key, NetKey/AppKey
 * Index, etc. which are going to be used as the prefix of the nvs keys of the BLE
 * Mesh settings.
 */

static struct net_buf_simple *settings_get_item(bt_mesh_nvs_handle_t handle, const char *key)
{
    struct net_buf_simple *buf = NULL;
    size_t length = 0U;
    bool exist = false;
    int err = 0;

    length = settings_get_length(handle, key);
    if (!length) {
        BT_DBG("Empty %s", key);
        return NULL;
    }

    buf = bt_mesh_alloc_buf(length);
    if (!buf) {
        BT_ERR("%s, Out of memory", __func__);
        /* TODO: in this case, erase all related settings? */
        return NULL;
    }

    err = settings_load(handle, key, buf->data, length, &exist);
    if (err) {
        BT_ERR("Failed to load %s", key);
        /* TODO: in this case, erase all related settings? */
        bt_mesh_free_buf(buf);
        return NULL;
    }

    if (exist == false) {
        bt_mesh_free_buf(buf);
        return NULL;
    }

    buf->len = length;
    return buf;
}

struct net_buf_simple *bt_mesh_get_settings_item(bt_mesh_nvs_handle_t handle, const char *key)
{
    struct net_buf_simple *buf = NULL;
    bt_mesh_settings_lock();
    buf = settings_get_item(handle, key);
    bt_mesh_settings_unlock();
    return buf;
}

struct net_buf_simple *bt_mesh_get_core_settings_item(const char *key)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_CORE);
    return bt_mesh_get_settings_item(handle, key);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
struct net_buf_simple *bt_mesh_get_uid_settings_item(const char *key)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_UID);
    return bt_mesh_get_settings_item(handle, key);
}
#endif

/* API used to check if the settings item exists */

bool bt_mesh_is_settings_item_exist(struct net_buf_simple *buf, const uint16_t val)
{
    struct net_buf_simple_state state = {0};
    size_t length = 0U;
    int i;

    if (!buf) {
        return false;
    }

    net_buf_simple_save(buf, &state);

    length = buf->len;
    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t item = net_buf_simple_pull_le16(buf);
        if (item == val) {
            net_buf_simple_restore(buf, &state);
            return true;
        }
    }

    net_buf_simple_restore(buf, &state);
    return false;
}

/* API used to add the settings item */

static int settings_add_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val)
{
    struct net_buf_simple *store = NULL;
    struct net_buf_simple *buf = NULL;
    size_t length = 0U;
    int err = 0;

    buf = settings_get_item(handle, key);

    /* Check if val already exists */
    if (bt_mesh_is_settings_item_exist(buf, val) == true) {
        BT_DBG("0x%04x already exists", val);
        bt_mesh_free_buf(buf);
        return 0;
    }

    length = (buf ? buf->len : 0) + sizeof(val);

    store = bt_mesh_alloc_buf(length);
    if (!store) {
        BT_ERR("%s, Out of memory", __func__);
        bt_mesh_free_buf(buf);
        return -ENOMEM;
    }

    if (buf) {
        net_buf_simple_add_mem(store, buf->data, buf->len);
    }
    net_buf_simple_add_mem(store, &val, sizeof(val));

    err = settings_save(handle, key, store->data, store->len);

    bt_mesh_free_buf(store);
    bt_mesh_free_buf(buf);
    return err;
}

int bt_mesh_add_settings_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val)
{
    int err = 0;
    bt_mesh_settings_lock();
    err = settings_add_item(handle, key, val);
    bt_mesh_settings_unlock();
    return err;
}

int bt_mesh_add_core_settings_item(const char *key, const uint16_t val)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_CORE);
    return bt_mesh_add_settings_item(handle, key, val);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int bt_mesh_add_uid_settings_item(const char *key, const uint16_t val)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_UID);
    return bt_mesh_add_settings_item(handle, key, val);
}
#endif

/* API used to remove the settings item */

static int settings_remove_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val)
{
    struct net_buf_simple *store = NULL;
    struct net_buf_simple *buf = NULL;
    size_t length = 0U;
    size_t buf_len = 0U;
    int err = 0;
    int i;

    buf = settings_get_item(handle, key);

    /* Check if val does exist */
    if (bt_mesh_is_settings_item_exist(buf, val) == false) {
        BT_DBG("0x%04x not exists", val);
        bt_mesh_free_buf(buf);
        return 0;
    }

    length = buf->len - sizeof(val);
    if (!length) {
        settings_save(handle, key, NULL, 0);
        bt_mesh_free_buf(buf);
        return 0;
    }

    store = bt_mesh_alloc_buf(length);
    if (!store) {
        BT_ERR("%s, Out of memory", __func__);
        bt_mesh_free_buf(buf);
        return -ENOMEM;
    }

    buf_len = buf->len;
    for (i = 0; i < buf_len / SETTINGS_ITEM_SIZE; i++) {
        uint16_t item = net_buf_simple_pull_le16(buf);
        if (item != val) {
            net_buf_simple_add_le16(store, item);
        }
    }

    err = settings_save(handle, key, store->data, store->len);

    bt_mesh_free_buf(store);
    bt_mesh_free_buf(buf);
    return err;
}

int bt_mesh_remove_settings_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val)
{
    int err = 0;
    bt_mesh_settings_lock();
    err = settings_remove_item(handle, key, val);
    bt_mesh_settings_unlock();
    return err;
}

int bt_mesh_remove_core_settings_item(const char *key, const uint16_t val)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_CORE);
    return bt_mesh_remove_settings_item(handle, key, val);
}

#if CONFIG_BLE_MESH_USE_MULTIPLE_NAMESPACE
int bt_mesh_remove_uid_settings_item(const char *key, const uint16_t val)
{
    bt_mesh_nvs_handle_t handle = settings_get_nvs_handle(SETTINGS_UID);
    return bt_mesh_remove_settings_item(handle, key, val);
}
#endif

int bt_mesh_settings_erase_key(bt_mesh_nvs_handle_t handle, const char *key)
{
    int err = 0;

    err = nvs_erase_key(handle, key);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            return 0;
        }

        BT_ERR("Failed to erase %s (err %d)", key, err);
        return -EIO;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        BT_ERR("Failed to commit nvs (err %d)", err);
        return -EIO;
    }

    return 0;
}

int bt_mesh_settings_erase_all(bt_mesh_nvs_handle_t handle)
{
    int err = 0;

    err = nvs_erase_all(handle);
    if (err != ESP_OK) {
        BT_ERR("Failed to erase all (err %d)", err);
        return -EIO;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        BT_ERR("Failed to commit nvs (err %d)", err);
        return -EIO;
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_SETTINGS */
