/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <assert.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_stdio.h"
#include "esp_vfs.h"
#include <sys/errno.h>

#if CONFIG_VFS_SUPPORT_IO

#if CONFIG_ESP_CONSOLE_USB_CDC
#include "esp_vfs_cdcacm.h"
#include "esp_private/esp_vfs_cdcacm.h"
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
#include "driver/esp_private/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag_vfs.h"
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED

#if CONFIG_ESP_CONSOLE_UART
#include "driver/esp_private/uart_vfs.h"
#include "driver/uart_vfs.h"
#endif // CONFIG_ESP_CONSOLE_UART

#include "esp_private/startup_internal.h"
#include "esp_private/nullfs.h"
#include <sys/lock.h>
#endif

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

/**
 * This file is to concentrate all the vfs(UART, USB_SERIAL_JTAG, CDCACM) console into one single file.
 * Get the vfs information from their component (i.e. uart_vfs.c),
 * which can help us to output some string to two different ports(i.e both through uart and usb_serial_jtag).
 * Usually, we set a port as primary and another as secondary. For primary, it is used for all the features supported by each vfs implementation,
 * while the secondary is only used for output.
 */

#if CONFIG_VFS_SUPPORT_IO

typedef struct {
    const esp_vfs_fs_ops_t *ops;  /* NULL = slot is free */
    void *vfs_ctx;
    const char *path;
    int fd;  /* -1 if not opened (used by auxiliaries) */
} mux_entry_t;

/* Primary backend (all ops forwarded except open + write + fsync) */
static mux_entry_t s_primary = { .fd = -1 };

/* Auxiliary sinks (write-only fan-out) */
#define STDIO_MAX_AUXILIARY (CONFIG_ESP_STDIO_MAX_VFS_ENTRIES - 1)
static mux_entry_t s_auxiliary[STDIO_MAX_AUXILIARY] = { [0 ... STDIO_MAX_AUXILIARY - 1] = { .fd = -1 } };
static int s_open_count = 0;
static _lock_t s_lock;

static const char *TAG = "esp_stdio";

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static const mux_entry_t *get_primary_termios_entry(int fd, const esp_vfs_termios_ops_t **out_ops)
{
    const mux_entry_t *entry = &s_primary;
    assert(entry);

    const esp_vfs_termios_ops_t *termios = entry->ops->termios;
    if (!termios) {
        errno = ENOSYS;
        return NULL;
    }

    if (out_ops) {
        *out_ops = termios;
    }

    return entry;
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

static esp_err_t __attribute__((unused)) register_auxiliary(const esp_vfs_fs_ops_t *ops, void *ctx, const char *path)
{
    if (!ops || !path || !ops->write_p) {
        return ESP_ERR_INVALID_ARG;
    }
    _lock_acquire(&s_lock);
    for (size_t i = 0; i < STDIO_MAX_AUXILIARY; i++) {
        if (s_auxiliary[i].ops == NULL) {
            s_auxiliary[i] = (mux_entry_t) {
                .ops = ops,
                .vfs_ctx = ctx,
                .path = path,
                .fd = -1,
            };
            _lock_release(&s_lock);
            return ESP_OK;
        }
    }
    _lock_release(&s_lock);
    ESP_EARLY_LOGE(TAG, "Too many auxiliary sinks");
    return ESP_ERR_NO_MEM;
}

int console_open(__attribute__((unused)) void *ctx, const char * path, int flags, int mode)
{
    (void)path;
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops || !entry->ops->open_p) {
        errno = ENOSYS;
        return -1;
    }

    int local_fd = entry->ops->open_p(entry->vfs_ctx, entry->path, flags, mode);
    if (local_fd < 0) {
        return -1;
    }

    /* Lazily open auxiliaries on first console open */
    _lock_acquire(&s_lock);
    if (s_open_count == 0) {
        for (size_t i = 0; i < STDIO_MAX_AUXILIARY; i++) {
            mux_entry_t *aux = &s_auxiliary[i];
            if (aux->ops && aux->ops->open_p && aux->fd < 0) {
                aux->fd = aux->ops->open_p(aux->vfs_ctx, aux->path, O_WRONLY, 0);
            }
        }
    }
    s_open_count++;
    _lock_release(&s_lock);

    return local_fd;
}

int console_close(__attribute__((unused)) void *ctx, int fd)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops || !entry->ops->close_p) {
        errno = ENOSYS;
        return -1;
    }

    int ret = entry->ops->close_p(entry->vfs_ctx, fd);
    if (ret != 0) {
        return ret;
    }

    /* Close auxiliaries when last console fd is closed */
    _lock_acquire(&s_lock);
    if (s_open_count > 0) {
        s_open_count--;
    }
    if (s_open_count == 0) {
        for (size_t i = 0; i < STDIO_MAX_AUXILIARY; i++) {
            mux_entry_t *aux = &s_auxiliary[i];
            if (aux->ops && aux->ops->close_p && aux->fd >= 0) {
                aux->ops->close_p(aux->vfs_ctx, aux->fd);
                aux->fd = -1;
            }
        }
    }
    _lock_release(&s_lock);

    return 0;
}

ssize_t console_write(__attribute__((unused)) void *ctx, int fd, const void *data, size_t size)
{
    const mux_entry_t *primary = &s_primary;
    ssize_t ret_val = primary->ops->write_p(primary->vfs_ctx, fd, data, size);

    _lock_acquire(&s_lock);
    for (size_t i = 0; i < STDIO_MAX_AUXILIARY; i++) {
        const mux_entry_t *entry = &s_auxiliary[i];
        if (entry->ops && entry->ops->write_p && entry->fd >= 0) {
            (void) entry->ops->write_p(entry->vfs_ctx, entry->fd, data, size);
        }
    }
    _lock_release(&s_lock);

    return ret_val;
}

int console_fstat(__attribute__((unused)) void *ctx, int fd, struct stat * st)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops->fstat_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->fstat_p(entry->vfs_ctx, fd, st);
}

ssize_t console_read(__attribute__((unused)) void *ctx, int fd, void * dst, size_t size)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops->read_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->read_p(entry->vfs_ctx, fd, dst, size);
}

int console_fcntl(__attribute__((unused)) void *ctx, int fd, int cmd, int arg)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops->fcntl_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->fcntl_p(entry->vfs_ctx, fd, cmd, arg);
}

int console_fsync(__attribute__((unused)) void *ctx, int fd)
{
    const mux_entry_t *primary = &s_primary;
    if (!primary->ops->fsync_p) {
        errno = ENOSYS;
        return -1;
    }

    int ret_val = primary->ops->fsync_p(primary->vfs_ctx, fd);

    _lock_acquire(&s_lock);
    for (size_t i = 0; i < STDIO_MAX_AUXILIARY; i++) {
        const mux_entry_t *entry = &s_auxiliary[i];
        if (entry->ops && entry->ops->fsync_p && entry->fd >= 0) {
            (void) entry->ops->fsync_p(entry->vfs_ctx, entry->fd);
        }
    }
    _lock_release(&s_lock);

    return ret_val;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int console_access(__attribute__((unused)) void *ctx, const char *path, int amode)
{
    const mux_entry_t *entry = &s_primary;
    assert(entry);
    if (!entry->ops->dir || !entry->ops->dir->access_p) {
        errno = ENOSYS;
        return -1;
    }
    (void) path;
    return entry->ops->dir->access_p(entry->vfs_ctx, entry->path, amode);
}
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT

static esp_err_t console_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                      esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops || !entry->ops->select || !entry->ops->select->start_select) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return entry->ops->select->start_select(nfds, readfds, writefds, exceptfds, select_sem, end_select_args);
}

static esp_err_t console_end_select(void *end_select_args)
{
    const mux_entry_t *entry = &s_primary;
    if (!entry->ops || !entry->ops->select || !entry->ops->select->end_select) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return entry->ops->select->end_select(end_select_args);
}

#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

int console_tcsetattr(__attribute__((unused)) void *ctx, int fd, int optional_actions, const struct termios *p)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    const mux_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry) {
        return -1;
    }
    if (!termios->tcsetattr_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcsetattr_p(entry->vfs_ctx, fd, optional_actions, p);
}

int console_tcgetattr(__attribute__((unused)) void *ctx, int fd, struct termios *p)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    const mux_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry) {
        return -1;
    }
    if (!termios->tcgetattr_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcgetattr_p(entry->vfs_ctx, fd, p);
}

int console_tcdrain(__attribute__((unused)) void *ctx, int fd)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    const mux_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry) {
        return -1;
    }
    if (!termios->tcdrain_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcdrain_p(entry->vfs_ctx, fd);
}

int console_tcflush(__attribute__((unused)) void *ctx, int fd, int select)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    const mux_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry) {
        return -1;
    }
    if (!termios->tcflush_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcflush_p(entry->vfs_ctx, fd, select);
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifdef CONFIG_VFS_SUPPORT_DIR
static const esp_vfs_dir_ops_t s_vfs_console_dir = {
    .access_p = &console_access,
};
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
static const esp_vfs_select_ops_t s_vfs_console_select = {
    .start_select = &console_start_select,
    .end_select = &console_end_select,
};
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static const esp_vfs_termios_ops_t s_vfs_console_termios = {
    .tcsetattr_p = &console_tcsetattr,
    .tcgetattr_p = &console_tcgetattr,
    .tcdrain_p = &console_tcdrain,
    .tcflush_p = &console_tcflush,
};
#endif // CONFIG_VFS_SUPPORT_TERMIOS

static const esp_vfs_fs_ops_t s_vfs_console = {
    .write_p = &console_write,
    .open_p = &console_open,
    .fstat_p = &console_fstat,
    .close_p = &console_close,
    .read_p = &console_read,
    .fcntl_p = &console_fcntl,
    .fsync_p = &console_fsync,

#ifdef CONFIG_VFS_SUPPORT_DIR
    .dir = &s_vfs_console_dir,
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_vfs_console_select,
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .termios = &s_vfs_console_termios,
#endif // CONFIG_VFS_SUPPORT_TERMIOS
};

esp_err_t esp_stdio_register(void)
{
    _lock_init(&s_lock);

// Primary vfs part.
#if CONFIG_ESP_CONSOLE_UART
    s_primary = (mux_entry_t) {
        .ops = esp_vfs_uart_get_vfs(),
        .vfs_ctx = NULL,
        .path = "/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM),
        .fd = -1,
    };
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    s_primary = (mux_entry_t) {
        .ops = esp_vfs_usb_serial_jtag_get_vfs(),
        .vfs_ctx = NULL,
        .path = "/",
        .fd = -1,
    };
#elif CONFIG_ESP_CONSOLE_USB_CDC
    s_primary = (mux_entry_t) {
        .ops = esp_vfs_cdcacm_get_vfs(),
        .vfs_ctx = NULL,
        .path = "/",
        .fd = -1,
    };
#else
    s_primary = (mux_entry_t) {
        .ops = esp_vfs_null_get_vfs(),
        .vfs_ctx = NULL,
        .path = "/",
        .fd = -1,
    };
#endif

    if (!s_primary.ops) {
        ESP_EARLY_LOGE(TAG, "No primary console backend available");
        return ESP_FAIL;
    }

// Auxiliary sinks (write-only fan-out).
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    esp_err_t err = register_auxiliary(esp_vfs_usb_serial_jtag_get_vfs(), NULL, "/");
    if (err != ESP_OK) {
        return err;
    }
#endif

    return esp_vfs_register_fs(ESP_VFS_DEV_CONSOLE, &s_vfs_console, ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, NULL);
}

ESP_SYSTEM_INIT_FN(init_vfs_console, CORE, BIT(0), 119)
{
    return esp_stdio_register();
}

#endif // CONFIG_VFS_SUPPORT_IO

void esp_vfs_include_console_register(void)
{
    // Linker hook function, exists to make the linker examine this file
}
