// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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
#include <stdio.h>
#include <stddef.h>
#include <errno.h>

#include "nvs.h"
#include "sdkconfig.h"

#include "mesh_util.h"
#include "mesh_types.h"
#include "mesh_common.h"

#include "settings_nvs.h"
#include "settings.h"

#if CONFIG_BLE_MESH_SETTINGS

enum settings_type {
    SETTINGS_CORE,
    SETTINGS_SERVER,
};

struct settings_context {
    char *nvs_name;
    nvs_handle handle;

    int (*settings_init)(void);
    int (*settings_load)(void);
    int (*settings_commit)(void);
};

static struct settings_context settings_ctx[] = {
    [SETTINGS_CORE] = {
        .nvs_name = "mesh_core",
        .settings_init = settings_core_init,
        .settings_load = settings_core_load,
        .settings_commit = settings_core_commit,
    },
    [SETTINGS_SERVER] = {
        .nvs_name = "mesh_server",
        .settings_init = NULL,
        .settings_load = NULL,
        .settings_commit = NULL,
    },
};

/* API used to initialize, load and commit BLE Mesh related settings */

void bt_mesh_settings_foreach(void)
{
    int i, err;

    for (i = 0; i < ARRAY_SIZE(settings_ctx); i++) {
        struct settings_context *ctx = &settings_ctx[i];

        err = nvs_open(ctx->nvs_name, NVS_READWRITE, &ctx->handle);
        if (err != ESP_OK) {
            BT_ERR("%s, Open nvs failed, name %s, err %d", __func__, ctx->nvs_name, err);
            continue;
        }

        if (ctx->settings_init && ctx->settings_init()) {
            BT_ERR("%s, Init settings failed, name %s", __func__, ctx->nvs_name);
            continue;
        }

        if (ctx->settings_load && ctx->settings_load()) {
            BT_ERR("%s, Load settings failed, name %s", __func__, ctx->nvs_name);
            continue;
        }

        if (ctx->settings_commit && ctx->settings_commit()) {
            BT_ERR("%s, Commit settings failed, name %s", __func__, ctx->nvs_name);
            continue;
        }
    }
}

/* API used to get BLE Mesh related nvs handle */

static inline nvs_handle settings_get_nvs_handle(enum settings_type type)
{
    return settings_ctx[type].handle;
}

/* API used to store/erase BLE Mesh related settings */

static int settings_save(nvs_handle handle, const char *key, const u8_t *val, size_t len)
{
    int err;

    if (key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_DBG("%s, nvs %s, key %s", __func__, val ? "set" : "erase", key);

    if (val) {
        err = nvs_set_blob(handle, key, val, len);
    } else {
        err = nvs_erase_key(handle, key);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            BT_DBG("%s, %s does not exist", __func__, key);
            return 0;
        }
    }
    if (err != ESP_OK) {
        BT_ERR("%s, Failed to %s %s data (err %d)", __func__,
               val ? "set" : "erase", key, err);
        return -EIO;
    }

    err = nvs_commit(handle);
    if (err != ESP_OK) {
        BT_ERR("%s, Failed to commit settings (err %d)", __func__, err);
        return -EIO;
    }

    return 0;
}

int bt_mesh_save_core_settings(const char *key, const u8_t *val, size_t len)
{
    nvs_handle handle = settings_get_nvs_handle(SETTINGS_CORE);
    return settings_save(handle, key, val, len);
}

/* API used to load BLE Mesh related settings */

static int settings_load(nvs_handle handle, const char *key,
                         u8_t *buf, size_t buf_len, bool *exist)
{
    int err;

    if (key == NULL || buf == NULL || exist == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    err = nvs_get_blob(handle, key, buf, &buf_len);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            BT_DBG("%s, Settings %s is not found", __func__, key);
            *exist = false;
            return 0;
        }

        BT_ERR("%s, Failed to get %s data (err %d)", __func__, key, err);
        return -EIO;
    }

    *exist = true;
    return 0;
}

int bt_mesh_load_core_settings(const char *key, u8_t *buf, size_t buf_len, bool *exist)
{
    nvs_handle handle = settings_get_nvs_handle(SETTINGS_CORE);
    return settings_load(handle, key, buf, buf_len, exist);
}

/* API used to get length of BLE Mesh related settings */

static size_t settings_get_length(nvs_handle handle, const char *key)
{
    size_t len = 0;
    int err;

    if (key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return 0;
    }

    err = nvs_get_blob(handle, key, NULL, &len);
    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            BT_ERR("%s, Failed to get %s length (err %d)", __func__, key, err);
        }
        return 0;
    }

    return len;
}

/* API used to get BLE Mesh related items. Here items mean model key, NetKey/AppKey
 * Index, etc. which are going to be used as the prefix of the nvs keys of the BLE
 * Mesh settings.
 */

static struct net_buf_simple *settings_get_item(nvs_handle handle, const char *key)
{
    struct net_buf_simple *buf = NULL;
    size_t length;
    bool exist;
    int err;

    length = settings_get_length(handle, key);
    if (!length) {
        BT_DBG("%s, Empty %s", __func__, key);
        return NULL;
    }

    buf = bt_mesh_alloc_buf(length);
    if (!buf) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        /* TODO: in this case, erase all related settings? */
        return NULL;
    }

    err = settings_load(handle, key, buf->data, length, &exist);
    if (err) {
        BT_ERR("%s, Failed to load %s", __func__, key);
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

struct net_buf_simple *bt_mesh_get_core_settings_item(const char *key)
{
    nvs_handle handle = settings_get_nvs_handle(SETTINGS_CORE);
    return settings_get_item(handle, key);
}

/* API used to check if the settings item exists */

static bool is_settings_item_exist(struct net_buf_simple *buf, const u16_t val)
{
    struct net_buf_simple_state state = {0};
    size_t length;
    int i;

    if (!buf) {
        return false;
    }

    net_buf_simple_save(buf, &state);

    length = buf->len;
    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        u16_t item = net_buf_simple_pull_le16(buf);
        if (item == val) {
            net_buf_simple_restore(buf, &state);
            return true;
        }
    }

    net_buf_simple_restore(buf, &state);
    return false;
}

/* API used to add the settings item */

static int settings_add_item(nvs_handle handle, const char *key, const u16_t val)
{
    struct net_buf_simple *store = NULL;
    struct net_buf_simple *buf = NULL;
    size_t length = 0;
    int err;

    buf = settings_get_item(handle, key);

    /* Check if val already exists */
    if (is_settings_item_exist(buf, val) == true) {
        BT_DBG("%s, 0x%04x already exists", __func__, val);
        bt_mesh_free_buf(buf);
        return 0;
    }

    length = (buf ? buf->len : 0) + sizeof(val);

    store = bt_mesh_alloc_buf(length);
    if (!store) {
        BT_ERR("%s, Failed to allocate memory", __func__);
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

int bt_mesh_add_core_settings_item(const char *key, const u16_t val)
{
    nvs_handle handle = settings_get_nvs_handle(SETTINGS_CORE);
    return settings_add_item(handle, key, val);
}

/* API used to remove the settings item */

static int settings_remove_item(nvs_handle handle, const char *key, const u16_t val)
{
    struct net_buf_simple *store = NULL;
    struct net_buf_simple *buf = NULL;
    size_t length = 0;
    size_t buf_len;
    int i, err;

    buf = settings_get_item(handle, key);

    /* Check if val does exist */
    if (is_settings_item_exist(buf, val) == false) {
        BT_DBG("%s, 0x%04x does not exist", __func__, val);
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
        BT_ERR("%s, Failed to allocate memory", __func__);
        bt_mesh_free_buf(buf);
        return -ENOMEM;
    }

    buf_len = buf->len;
    for (i = 0; i < buf_len / SETTINGS_ITEM_SIZE; i++) {
        u16_t item = net_buf_simple_pull_le16(buf);
        if (item != val) {
            net_buf_simple_add_le16(store, item);
        }
    }

    err = settings_save(handle, key, store->data, store->len);

    bt_mesh_free_buf(store);
    bt_mesh_free_buf(buf);
    return err;
}

int bt_mesh_remove_core_settings_item(const char *key, const u16_t val)
{
    nvs_handle handle = settings_get_nvs_handle(SETTINGS_CORE);
    return settings_remove_item(handle, key, val);
}

#endif /* CONFIG_BLE_MESH_SETTINGS */
