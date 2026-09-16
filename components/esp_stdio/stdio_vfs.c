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

/**
 * This file is to concentrate all the vfs(UART, USB_SERIAL_JTAG, CDCACM) console into one single file.
 * Get the vfs information from their component (i.e. uart_vfs.c),
 * which can help us to output some string to two different ports(i.e both through uart and usb_serial_jtag).
 * Usually, we set a port as primary and another as secondary. For primary, it is used for all the features
 * supported by each vfs implementation, while the secondary is only used for output.
 */

#if CONFIG_VFS_SUPPORT_IO

#if CONFIG_ESP_CONSOLE_USB_CDC
#include "esp_vfs_cdcacm.h"
#include "esp_private/esp_vfs_cdcacm.h"
#endif // CONFIG_ESP_CONSOLE_USB_CDC

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
#include "driver/esp_private/usb_serial_jtag_vfs.h"
#include "driver/usb_serial_jtag_vfs.h"
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED

#if CONFIG_ESP_CONSOLE_UART && !CONFIG_IDF_TARGET_LINUX
#include "driver/esp_private/uart_vfs.h"
#include "driver/uart_vfs.h"
#endif // CONFIG_ESP_CONSOLE_UART && !CONFIG_IDF_TARGET_LINUX

#include "esp_private/startup_internal.h"
#include "esp_private/nullfs.h"
#include "esp_stdio_private.h"
#include <sys/lock.h>
#endif // CONFIG_VFS_SUPPORT_IO

#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s

/* Pool, stack and lock — declared extern in esp_stdio_private.h so that
 * stdio_mux_api.c can access them without pulling in this object file. */
esp_stdio_entry_t s_entry_pool[STDIO_MAX_ENTRIES];
esp_stdio_entry_t *s_primary_stack[STDIO_MAX_ENTRIES];
int s_primary_index = -1;
int s_open_count;
_lock_t s_lock;

/* The active primary is always the top of the stack. */
static inline esp_stdio_entry_t *active_primary(void)
{
    assert(s_primary_index >= 0);
    return s_primary_stack[s_primary_index];
}

/* Open the backend for an entry and store the fd.
 * Non-static: also called from stdio_mux_api.c. */
void entry_open(esp_stdio_entry_t *e, int flags)
{
    if (!e->ops || !e->ops->open_p || e->fd >= 0) {
        return;
    }
    e->fd = e->ops->open_p(e->vfs_ctx, e->path, flags, 0);
}

/* Close the backend fd for an entry.
 * Non-static: also called from stdio_mux_api.c. */
void entry_close(esp_stdio_entry_t *e)
{
    if (e->fd < 0) {
        return;
    }
    if (e->ops && e->ops->close_p) {
        e->ops->close_p(e->vfs_ctx, e->fd);
    }
    e->fd = -1;
}

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static esp_stdio_entry_t *get_primary_termios_entry(int fd, const esp_vfs_termios_ops_t **out_ops)
{
    esp_stdio_entry_t *entry = active_primary();
    if (!entry || !entry->ops || !entry->ops->termios) {
        errno = ENOSYS;
        return NULL;
    }
    if (out_ops) {
        *out_ops = entry->ops->termios;
    }
    return entry;
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

int console_open(__attribute__((unused)) void *ctx, const char *path, int flags, int mode)
{
    (void)path;
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops || !primary->ops->open_p) {
        errno = ENOSYS;
        return -1;
    }

    int local_fd = primary->ops->open_p(primary->vfs_ctx, primary->path, flags, mode);
    if (local_fd < 0) {
        return -1;
    }
    primary->fd = local_fd;

    /* Lazily open every other in-use entry (the auxiliaries) on first console
     * open so they receive the write/fsync fan-out. */
    _lock_acquire(&s_lock);
    if (s_open_count == 0) {
        for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
            esp_stdio_entry_t *e = &s_entry_pool[i];
            if (e->in_use && e != primary) {
                entry_open(e, O_WRONLY);
            }
        }
    }
    s_open_count++;
    _lock_release(&s_lock);

    return local_fd;
}

int console_close(__attribute__((unused)) void *ctx, int fd)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops || !primary->ops->close_p) {
        errno = ENOSYS;
        return -1;
    }

    /* Route to the active backend's own fd: after a push/pop/unregister the
     * active primary may differ from the backend that originally produced the
     * caller-visible fd, and each backend validates fds in its own namespace. */
    int ret = primary->ops->close_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd);
    if (ret != 0) {
        return ret;
    }
    primary->fd = -1;

    _lock_acquire(&s_lock);
    if (s_open_count > 0) {
        s_open_count--;
    }
    if (s_open_count == 0) {
        for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
            esp_stdio_entry_t *e = &s_entry_pool[i];
            if (e->in_use && e != primary) {
                entry_close(e);
            }
        }
    }
    _lock_release(&s_lock);

    return 0;
}

ssize_t console_write(__attribute__((unused)) void *ctx, int fd, const void *data, size_t size)
{
    esp_stdio_entry_t *primary = active_primary();
    ssize_t ret = primary->ops->write_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd, data, size);

    _lock_acquire(&s_lock);
    for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
        esp_stdio_entry_t *e = &s_entry_pool[i];
        if (e->in_use && e != primary && e->ops && e->ops->write_p && e->fd >= 0) {
            (void)e->ops->write_p(e->vfs_ctx, e->fd, data, size);
        }
    }
    _lock_release(&s_lock);

    return ret;
}

int console_fstat(__attribute__((unused)) void *ctx, int fd, struct stat *st)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops->fstat_p) {
        errno = ENOSYS;
        return -1;
    }
    return primary->ops->fstat_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd, st);
}

ssize_t console_read(__attribute__((unused)) void *ctx, int fd, void *dst, size_t size)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops->read_p) {
        errno = ENOSYS;
        return -1;
    }
    return primary->ops->read_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd, dst, size);
}

int console_fcntl(__attribute__((unused)) void *ctx, int fd, int cmd, int arg)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops->fcntl_p) {
        errno = ENOSYS;
        return -1;
    }
    return primary->ops->fcntl_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd, cmd, arg);
}

int console_fsync(__attribute__((unused)) void *ctx, int fd)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops->fsync_p) {
        errno = ENOSYS;
        return -1;
    }
    int ret = primary->ops->fsync_p(primary->vfs_ctx, primary->fd >= 0 ? primary->fd : fd);

    _lock_acquire(&s_lock);
    for (int i = 0; i < STDIO_MAX_ENTRIES; i++) {
        esp_stdio_entry_t *e = &s_entry_pool[i];
        if (e->in_use && e != primary && e->ops && e->ops->fsync_p && e->fd >= 0) {
            (void)e->ops->fsync_p(e->vfs_ctx, e->fd);
        }
    }
    _lock_release(&s_lock);

    return ret;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int console_access(__attribute__((unused)) void *ctx, const char *path, int amode)
{
    esp_stdio_entry_t *primary = active_primary();
    if (!primary->ops->dir || !primary->ops->dir->access_p) {
        errno = ENOSYS;
        return -1;
    }
    (void)path;
    return primary->ops->dir->access_p(primary->vfs_ctx, primary->path, amode);
}
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT
static esp_err_t console_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                      esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    const esp_vfs_fs_ops_t *ops = active_primary()->ops;
    if (!ops || !ops->select || !ops->select->start_select) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ops->select->start_select(nfds, readfds, writefds, exceptfds, select_sem, end_select_args);
}

static esp_err_t console_end_select(void *end_select_args)
{
    const esp_vfs_fs_ops_t *ops = active_primary()->ops;
    if (!ops || !ops->select || !ops->select->end_select) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ops->select->end_select(end_select_args);
}
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
int console_tcsetattr(__attribute__((unused)) void *ctx, int fd, int optional_actions, const struct termios *p)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    esp_stdio_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry || !termios->tcsetattr_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcsetattr_p(entry->vfs_ctx, fd, optional_actions, p);
}

int console_tcgetattr(__attribute__((unused)) void *ctx, int fd, struct termios *p)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    esp_stdio_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry || !termios->tcgetattr_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcgetattr_p(entry->vfs_ctx, fd, p);
}

int console_tcdrain(__attribute__((unused)) void *ctx, int fd)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    esp_stdio_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry || !termios->tcdrain_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->termios->tcdrain_p(entry->vfs_ctx, fd);
}

int console_tcflush(__attribute__((unused)) void *ctx, int fd, int select)
{
    const esp_vfs_termios_ops_t *termios = NULL;
    esp_stdio_entry_t *entry = get_primary_termios_entry(fd, &termios);
    if (!entry || !termios->tcflush_p) {
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
#endif
#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_vfs_console_select,
#endif
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    .termios = &s_vfs_console_termios,
#endif
};

esp_err_t esp_stdio_register(void)
{
    _lock_init(&s_lock);

    /* Directly initialise the system primary at pool slot 0 and push it as the
     * immovable sentinel at stack index 0.  We bypass the public API here to
     * avoid a link-time dependency on stdio_mux_api.c so that binaries which
     * never call the mux API don't pay for it. */
    esp_stdio_entry_t *system_primary = &s_entry_pool[0];
    *system_primary = (esp_stdio_entry_t) {
        .vfs_ctx = NULL,
        .path = "/",
        .fd = -1,
        .in_use = true,
    };

#if CONFIG_ESP_CONSOLE_UART && !CONFIG_IDF_TARGET_LINUX
    /* no need to check vfs_ops for NULL: there is no config in which it can be NULL */
    system_primary->ops = esp_vfs_uart_get_vfs();
    system_primary->path = "/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    system_primary->ops = esp_vfs_usb_serial_jtag_get_vfs();
#elif CONFIG_ESP_CONSOLE_USB_CDC
    system_primary->ops = esp_vfs_cdcacm_get_vfs();
#else
    system_primary->ops = esp_vfs_null_get_vfs();
#endif

    s_primary_stack[0] = system_primary;
    s_primary_index = 0;

#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    esp_stdio_entry_t *secondary = &s_entry_pool[1];
    *secondary = (esp_stdio_entry_t) {
        .ops = esp_vfs_usb_serial_jtag_get_vfs(),
        .vfs_ctx = NULL,
        .path = "/",
        .fd = -1,
        .in_use = true,
    };
#endif

    return esp_vfs_register_fs(ESP_VFS_DEV_CONSOLE, &s_vfs_console,
                               ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, NULL);
}

ESP_SYSTEM_INIT_FN(init_vfs_console, CORE, BIT(0), 119)
{
    return esp_stdio_register();
}

void esp_vfs_include_console_register(void)
{
    // Linker hook function, exists to make the linker examine this file
}
