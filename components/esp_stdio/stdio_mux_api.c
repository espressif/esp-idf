/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Public mux API: esp_stdio_register_io / unregister_io / push_primary /
 * pop_primary / has_user_primary.
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

/* True if the entry is currently on the primary stack (at any depth). */
static bool on_stack(const esp_stdio_entry_t *e)
{
    for (int i = 0; i <= s_primary_index; i++) {
        if (s_primary_stack[i] == e) {
            return true;
        }
    }
    return false;
}

/* Remove an entry from the primary stack at any depth (does not close fd).
 * Returns true if found. */
static bool stack_remove(esp_stdio_entry_t *e)
{
    for (int i = 0; i <= s_primary_index; i++) {
        if (s_primary_stack[i] == e) {
            /* Shift everything above it down so the entry below becomes the
             * new active primary. */
            memmove(&s_primary_stack[i], &s_primary_stack[i + 1],
                    (s_primary_index - i) * sizeof(s_primary_stack[0]));
            s_primary_stack[s_primary_index--] = NULL;
            return true;
        }
    }
    return false;
}

bool esp_stdio_has_user_primary(void)
{
    /* The system-registered primary is always at the base of the stack, so a
     * user primary is active exactly when something has been pushed on top. */
    return s_primary_index > 0;
}

esp_err_t esp_stdio_register_io(const esp_stdio_io_config_t *config,
                                esp_stdio_handle_t *out_handle)
{
    if (!config || !config->vfs_ops || !config->path || !out_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    esp_stdio_entry_t *e = NULL;
    for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
        if (!s_entry_pool[i].in_use) {
            e = &s_entry_pool[i];
            break;
        }
    }
    if (!e) {
        _lock_release(&s_lock);
        ESP_EARLY_LOGE(TAG, "No space left for stdio registration");
        return ESP_ERR_NO_MEM;
    }

    *e = (esp_stdio_entry_t) {
        .ops = config->vfs_ops,
        .vfs_ctx = config->vfs_ctx,
        .path = config->path,
        .fd = -1,
        .in_use = true,
    };

    /* A freshly registered entry is an auxiliary. If /dev/console is already
     * open, open it now so it starts receiving the write/fsync fan-out. */
    if (s_open_count > 0) {
        entry_open(e, O_WRONLY);
    }

    _lock_release(&s_lock);
    *out_handle = e;
    return ESP_OK;
}

esp_err_t esp_stdio_unregister_io(esp_stdio_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    if (!handle->in_use) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_ARG;
    }

    /* If it is on the primary stack at any depth, remove it so the previous
     * primary takes over.  Auxiliaries are not on the stack: no-op for them. */
    stack_remove(handle);

    entry_close(handle);
    handle->in_use = false;
    handle->ops = NULL;

    _lock_release(&s_lock);
    return ESP_OK;
}

esp_err_t esp_stdio_push_primary(esp_stdio_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

    _lock_acquire(&s_lock);

    if (!handle->in_use || on_stack(handle)) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_ARG;
    }

    if (s_primary_index + 1 >= STDIO_MAX_ENTRIES) {
        _lock_release(&s_lock);
        return ESP_ERR_NO_MEM;
    }

    /* The previously active primary stays suspended on the stack (still open,
     * so it receives the write/fsync fan-out). Open the new primary if the
     * console is already open.
     *
     * If the entry was previously used as an auxiliary it will already have
     * an fd opened O_WRONLY.  entry_open() would return early in that case
     * without upgrading to O_RDWR, causing reads from the new primary to
     * fail.  Close first so the re-open below picks up the correct flags. */
    if (s_open_count > 0) {
        if (handle->fd >= 0) {
            entry_close(handle);
        }
        entry_open(handle, O_RDWR);
    }

    s_primary_stack[++s_primary_index] = handle;

    _lock_release(&s_lock);
    return ESP_OK;
}

esp_err_t esp_stdio_pop_primary(esp_stdio_handle_t handle)
{
    _lock_acquire(&s_lock);

    /* s_primary_stack[0] is the system-registered sentinel: it can never be
     * removed.  Its handle is never exposed to callers, but guard against it
     * explicitly. */
    if (handle == NULL) {
        /* Remove the active (top) primary. */
        if (s_primary_index <= 0) {
            _lock_release(&s_lock);
            return ESP_ERR_INVALID_STATE;
        }
        handle = s_primary_stack[s_primary_index];
    } else if (handle == s_primary_stack[0] || !on_stack(handle)) {
        _lock_release(&s_lock);
        return ESP_ERR_INVALID_STATE;
    }

    /* Remove from the stack.  The entry stays in_use and open, reverting to
     * auxiliary status.  No fd juggling required. */
    stack_remove(handle);

    _lock_release(&s_lock);
    return ESP_OK;
}

#endif // CONFIG_VFS_SUPPORT_IO
