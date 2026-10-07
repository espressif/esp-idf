/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Public mux API: register_driver / unregister_driver / set_primary /
 * has_user_primary.
 *
 * This file is intentionally separate from stdio_vfs.c so that it is only
 * linked into a binary when the application actually calls one of these
 * functions.  stdio_vfs.c is always pulled in (via the
 * -u esp_vfs_include_console_register linker hook) but this file is not, so
 * apps that never use the custom-IO API do not pay for the code here.
 */

#include "sdkconfig.h"

#if CONFIG_VFS_SUPPORT_IO

#include <string.h>
#include <fcntl.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_stdio.h"
#include "esp_stdio_private.h"

static const char *TAG = "esp_stdio";

bool esp_stdio_has_user_primary(void)
{
    /* A user primary is active exactly when it differs from the Kconfig
     * default that esp_stdio_register() installed at boot. */
    return s_active_primary != s_default_primary;
}

/* Allocate a free pool slot for a new driver.  Caller must hold s_lock.
 * The entry is created with no open fd; opening is left to the caller. */
static esp_err_t alloc_entry(const esp_stdio_driver_config_t *config, esp_stdio_entry_t **out)
{
    for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
        if (!s_entry_pool[i].in_use) {
            s_entry_pool[i] = (esp_stdio_entry_t) {
                .ops = config->vfs_ops,
                .vfs_ctx = config->vfs_ctx,
                .path = config->path,
                .fd = -1,
                .in_use = true,
            };
            *out = &s_entry_pool[i];
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_stdio_register_driver(const esp_stdio_driver_config_t *config,
                                    esp_stdio_handle_t *out_handle)
{
    if (!config || !config->vfs_ops || !config->path || !out_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    esp_stdio_entry_t *e = NULL;
    esp_err_t err = alloc_entry(config, &e);
    if (err != ESP_OK) {
        _lock_release(&s_lock);
        ESP_EARLY_LOGE(TAG, "No space left for stdio registration");
        return err;
    }

    /* A freshly registered driver is an auxiliary. If /dev/console is already
     * open, open it now (write-only) so it starts receiving the write/fsync
     * fan-out. */
    if (s_open_count > 0) {
        entry_open(e, O_WRONLY);
    }

    _lock_release(&s_lock);
    *out_handle = e;
    return ESP_OK;
}

esp_err_t esp_stdio_set_primary_driver(esp_stdio_handle_t handle,
                                       esp_stdio_handle_t *out_old_primary)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    if (!handle->in_use) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_ARG;
    }

    esp_stdio_entry_t *old = s_active_primary;

    /* Promote: if the console is open, ensure the new primary has a read+write
     * fd.  An entry that was serving as an auxiliary only has an O_WRONLY fd,
     * which would make reads fail, so it is re-opened O_RDWR. The old primary
     * stays registered and drops back to a write-only auxiliary sink. */
    if (handle != old && s_open_count > 0) {
        if (handle->fd >= 0) {
            entry_close(handle);
        }
        entry_open(handle, O_RDWR);
    }
    s_active_primary = handle;

    _lock_release(&s_lock);

    if (out_old_primary) {
        *out_old_primary = old;
    }
    return ESP_OK;
}

esp_err_t esp_stdio_unregister_driver(esp_stdio_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    if (!handle->in_use) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_ARG;
    }

    /* Only auxiliaries can be removed; the active primary must first be
     * replaced with esp_stdio_set_primary_driver() before it can be
     * unregistered. */
    if (handle == s_active_primary) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_STATE;
    }

    entry_close(handle);
    handle->in_use = false;
    handle->ops = NULL;

    /* Keep the has-user-primary anchor valid if the boot console is removed. */
    if (handle == s_default_primary) {
        s_default_primary = NULL;
    }

    _lock_release(&s_lock);
    return ESP_OK;
}

#endif // CONFIG_VFS_SUPPORT_IO
