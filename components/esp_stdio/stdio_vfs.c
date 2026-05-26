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
#include "esp_heap_caps.h"
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

#if !CONFIG_ESP_STDIO_BASIC_MODE
_Static_assert(CONFIG_ESP_STDIO_MAX_FDS >= 3, "Invalid value for CONFIG_ESP_STDIO_MAX_FDS");
#endif

#define ESP_STDIO_IS_BASIC (CONFIG_ESP_STDIO_BASIC_MODE)

typedef struct {
    const esp_vfs_fs_ops_t *ops;
    int vfs_flags;
    void *vfs_ctx;
    const char *path;
    int fd;
} mux_entry_t;

typedef struct {
    bool in_use;
    int flags;
} fd_entry_t;

typedef struct {
    mux_entry_t entries[CONFIG_ESP_STDIO_MAX_VFS_ENTRIES];
    size_t used;

#if !ESP_STDIO_IS_BASIC
    fd_entry_t fds[CONFIG_ESP_STDIO_MAX_FDS];
#endif

    size_t fd_count;
} context_t;

static context_t s_ctx = {0};
static _lock_t s_ctx_lock;

static const char *TAG = "esp_stdio";

static inline bool is_fd_valid_nolock(int fd)
{
#if ESP_STDIO_IS_BASIC
    return (fd >= 0) && (s_ctx.fd_count > 0);
#else
    return (fd >= 0) && (fd < CONFIG_ESP_STDIO_MAX_FDS) && s_ctx.fds[fd].in_use;
#endif
}

static inline bool is_fd_valid(int fd)
{
    bool valid;

    _lock_acquire(&s_ctx_lock);
    valid = is_fd_valid_nolock(fd);
    _lock_release(&s_ctx_lock);

    return valid;
}

#if !ESP_STDIO_IS_BASIC
static inline int get_fd_flags_nolock(int fd)
{
    assert((fd >= 0) && (fd < CONFIG_ESP_STDIO_MAX_FDS));
    return s_ctx.fds[fd].flags;
}

static inline bool fd_allows_read_nolock(int fd)
{
    const int flags = get_fd_flags_nolock(fd);
    return (flags & O_ACCMODE) != O_WRONLY;
}

static inline bool fd_allows_write_nolock(int fd)
{
    const int flags = get_fd_flags_nolock(fd);
    return (flags & O_ACCMODE) != O_RDONLY;
}
#endif

static inline mux_entry_t *get_primary_entry(void)
{
    assert(s_ctx.used > 0);
    return &s_ctx.entries[0];
}

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
static const mux_entry_t *get_primary_termios_entry(int fd, const esp_vfs_termios_ops_t **out_ops)
{
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return NULL;
    }

    const mux_entry_t *entry = get_primary_entry();
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

static esp_err_t esp_stdio_register_sink(const esp_vfs_fs_ops_t *ops, int vfs_flags, void *vfs_ctx, const char *path, int fd)
{
    if (!ops || !path) {
        return ESP_ERR_INVALID_ARG;
    }

    if (s_ctx.used >= CONFIG_ESP_STDIO_MAX_VFS_ENTRIES) {
        ESP_EARLY_LOGE(TAG, "Too many stdio sinks registered");
        return ESP_ERR_NO_MEM;
    }

    int mask = ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC;

    if ((vfs_flags & mask) != mask) {
        ESP_EARLY_LOGE(TAG, "The sink FS needs to use both context pointer api and be static allocated");
        return ESP_ERR_INVALID_ARG;
    }

    if (!ops->write_p) {
        ESP_EARLY_LOGE(TAG, "Stdio sink missing mandatory write handler");
        return ESP_ERR_INVALID_ARG;
    }

    bool is_primary = (s_ctx.used == 0);
    if (is_primary && (!ops->read_p || !ops->fstat_p || !ops->fcntl_p || !ops->fsync_p)) {
        ESP_EARLY_LOGE(TAG, "Primary stdio sink missing mandatory read/fstat/fcntl/fsync handlers");
        return ESP_ERR_INVALID_ARG;
    }

    size_t idx = s_ctx.used++;

    s_ctx.entries[idx] = (mux_entry_t) {
        .ops = ops,
        .vfs_flags = vfs_flags,
        .vfs_ctx = vfs_ctx,
        .path = path,
        .fd = fd,
    };

    return ESP_OK;
}

int console_open(__attribute__((unused)) void *ctx, const char * path, int flags, int mode)
{
#if ESP_STDIO_IS_BASIC
    (void) path;
    (void) flags;
    (void) mode;

    _lock_acquire(&s_ctx_lock);
    s_ctx.fd_count++;
    _lock_release(&s_ctx_lock);

    return 0;
#else
    (void) mode;

    if (!path || strcmp(path, "/") != 0) {
        errno = ENOENT;
        return -1;
    }

    _lock_acquire(&s_ctx_lock);

    if (s_ctx.fd_count >= CONFIG_ESP_STDIO_MAX_FDS) {
        _lock_release(&s_ctx_lock);
        errno = ENFILE;
        return -1;
    }

    int fd = -1;
    for (size_t i = 0; i < CONFIG_ESP_STDIO_MAX_FDS; ++i) {
        if (!s_ctx.fds[i].in_use) {
            fd = (int)i;
            break;
        }
    }

    if (fd < 0) {
        _lock_release(&s_ctx_lock);
        errno = ENFILE;
        return -1;
    }

    s_ctx.fd_count++;
    s_ctx.fds[fd] = (fd_entry_t) {
        .in_use = true,
        .flags = flags,
    };

    _lock_release(&s_ctx_lock);
    return fd;
#endif
}

int console_close(__attribute__((unused)) void *ctx, int fd)
{
    _lock_acquire(&s_ctx_lock);

    if (!is_fd_valid_nolock(fd)) {
        _lock_release(&s_ctx_lock);
        errno = EBADF;
        return -1;
    }

#if ESP_STDIO_IS_BASIC
    s_ctx.fd_count--;
#else
    s_ctx.fds[fd].in_use = false;
    s_ctx.fd_count--;
#endif

    _lock_release(&s_ctx_lock);
    return 0;
}

ssize_t console_write(__attribute__((unused)) void *ctx, int fd, const void *data, size_t size)
{
#if ESP_STDIO_IS_BASIC
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }
#else
    _lock_acquire(&s_ctx_lock);
    if (!is_fd_valid_nolock(fd)) {
        _lock_release(&s_ctx_lock);
        errno = EBADF;
        return -1;
    }
    if (!fd_allows_write_nolock(fd)) {
        _lock_release(&s_ctx_lock);
        errno = EBADF;
        return -1;
    }
    _lock_release(&s_ctx_lock);
#endif

    const mux_entry_t *primary = get_primary_entry();
    ssize_t ret_val = primary->ops->write_p(primary->vfs_ctx, primary->fd, data, size);

    for (size_t i = 1; i < s_ctx.used; i++) {
        const mux_entry_t *entry = s_ctx.entries + i;
        if (entry->ops->write_p && entry->fd >= 0) {
            (void) entry->ops->write_p(entry->vfs_ctx, entry->fd, data, size);
        }
    }

    return ret_val;
}

int console_fstat(__attribute__((unused)) void *ctx, int fd, struct stat * st)
{
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }

    const mux_entry_t *entry = get_primary_entry();
    if (!entry->ops->fstat_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->fstat_p(entry->vfs_ctx, entry->fd, st);
}

ssize_t console_read(__attribute__((unused)) void *ctx, int fd, void * dst, size_t size)
{
#if ESP_STDIO_IS_BASIC
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }
#else
    _lock_acquire(&s_ctx_lock);
    if (!is_fd_valid_nolock(fd)) {
        _lock_release(&s_ctx_lock);
        errno = EBADF;
        return -1;
    }
    if (!fd_allows_read_nolock(fd)) {
        _lock_release(&s_ctx_lock);
        errno = EBADF;
        return -1;
    }
    _lock_release(&s_ctx_lock);
#endif

    const mux_entry_t *entry = get_primary_entry();
    if (!entry->ops->read_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->read_p(entry->vfs_ctx, entry->fd, dst, size);
}

int console_fcntl(__attribute__((unused)) void *ctx, int fd, int cmd, int arg)
{
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }

    const mux_entry_t *entry = get_primary_entry();
    if (!entry->ops->fcntl_p) {
        errno = ENOSYS;
        return -1;
    }
    return entry->ops->fcntl_p(entry->vfs_ctx, entry->fd, cmd, arg);
}

int console_fsync(__attribute__((unused)) void *ctx, int fd)
{
    if (!is_fd_valid(fd)) {
        errno = EBADF;
        return -1;
    }

    const mux_entry_t *primary = get_primary_entry();
    if (!primary->ops->fsync_p) {
        errno = ENOSYS;
        return -1;
    }

    int ret_val = primary->ops->fsync_p(primary->vfs_ctx, primary->fd);

    for (size_t i = 1; i < s_ctx.used; i++) {
        const mux_entry_t *entry = s_ctx.entries + i;
        if (entry->ops->fsync_p && entry->fd >= 0) {
            (void) entry->ops->fsync_p(entry->vfs_ctx, entry->fd);
        }
    }

    return ret_val;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int console_access(__attribute__((unused)) void *ctx, const char *path, int amode)
{
    const mux_entry_t *entry = get_primary_entry();
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

typedef struct {
    void *sink_select_args;
    fd_set *readfds;
    fd_set *writefds;
    fd_set *exceptfds;
    uint64_t interested_read;
    uint64_t interested_write;
    uint64_t interested_except;
} console_select_ctx_t;

/* Kconfig allows up to 64 logical fds; uint64_t masks and (1ULL << i) require i < 64. */
enum { CONSOLE_SELECT_MAX_SCAN = 64 };

static esp_err_t console_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                      esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    const mux_entry_t *entry = get_primary_entry();
    assert(entry);
    *end_select_args = NULL;

    if (entry->fd < 0 || entry->ops->select == NULL || entry->ops->select->start_select == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    uint64_t interested_read = 0;
    uint64_t interested_write = 0;
    uint64_t interested_except = 0;

    const int scan = nfds < CONSOLE_SELECT_MAX_SCAN ? nfds : CONSOLE_SELECT_MAX_SCAN;
    for (int i = 0; i < scan; ++i) {
        if (!is_fd_valid(i)) {
            continue;
        }
        if (readfds && FD_ISSET(i, readfds)) {
            interested_read |= (1ULL << i);
        }
        if (writefds && FD_ISSET(i, writefds)) {
            interested_write |= (1ULL << i);
        }
        if (exceptfds && FD_ISSET(i, exceptfds)) {
            interested_except |= (1ULL << i);
        }
    }

    if (interested_read == 0 && interested_write == 0 && interested_except == 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    const int hw = entry->fd;

    if (readfds) {
        FD_ZERO(readfds);
        if (interested_read) {
            FD_SET(hw, readfds);
        }
    }
    if (writefds) {
        FD_ZERO(writefds);
        if (interested_write) {
            FD_SET(hw, writefds);
        }
    }
    if (exceptfds) {
        FD_ZERO(exceptfds);
        if (interested_except) {
            FD_SET(hw, exceptfds);
        }
    }

    const int forward_nfds = nfds > hw + 1 ? nfds : hw + 1;

    console_select_ctx_t *ctx = heap_caps_malloc(sizeof(console_select_ctx_t), MALLOC_CAP_INTERNAL);
    if (ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }

    ctx->sink_select_args = NULL;
    ctx->readfds = readfds;
    ctx->writefds = writefds;
    ctx->exceptfds = exceptfds;
    ctx->interested_read = interested_read;
    ctx->interested_write = interested_write;
    ctx->interested_except = interested_except;

    esp_err_t err = entry->ops->select->start_select(forward_nfds, readfds, writefds, exceptfds, select_sem,
                                                     &ctx->sink_select_args);
    if (err != ESP_OK) {
        if (readfds) {
            FD_ZERO(readfds);
            for (int i = 0; i < scan; ++i) {
                if (interested_read & (1ULL << i)) {
                    FD_SET(i, readfds);
                }
            }
        }
        if (writefds) {
            FD_ZERO(writefds);
            for (int i = 0; i < scan; ++i) {
                if (interested_write & (1ULL << i)) {
                    FD_SET(i, writefds);
                }
            }
        }
        if (exceptfds) {
            FD_ZERO(exceptfds);
            for (int i = 0; i < scan; ++i) {
                if (interested_except & (1ULL << i)) {
                    FD_SET(i, exceptfds);
                }
            }
        }
        heap_caps_free(ctx);
        return err;
    }

    *end_select_args = ctx;
    return ESP_OK;
}

static void console_expand_fdset(fd_set *fds, uint64_t interested, int hw)
{
    if (fds == NULL) {
        return;
    }
    const bool ready = FD_ISSET(hw, fds);
    FD_ZERO(fds);
    if (!ready || interested == 0) {
        return;
    }
    for (int i = 0; i < CONSOLE_SELECT_MAX_SCAN; ++i) {
        if (interested & (1ULL << i)) {
            FD_SET(i, fds);
        }
    }
}

esp_err_t console_end_select(void *end_select_args)
{
    if (end_select_args == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    console_select_ctx_t *ctx = (console_select_ctx_t *)end_select_args;
    const mux_entry_t *entry = get_primary_entry();
    assert(entry);

    esp_err_t ret = ESP_OK;
    if (entry->fd >= 0 && entry->ops->select && entry->ops->select->end_select) {
        ret = entry->ops->select->end_select(ctx->sink_select_args);
    }

    const int hw = entry->fd;
    if (hw >= 0) {
        console_expand_fdset(ctx->readfds, ctx->interested_read, hw);
        console_expand_fdset(ctx->writefds, ctx->interested_write, hw);
        console_expand_fdset(ctx->exceptfds, ctx->interested_except, hw);
    }

    heap_caps_free(ctx);
    return ret;
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
    return entry->ops->termios->tcsetattr_p(entry->vfs_ctx, entry->fd, optional_actions, p);
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
    return entry->ops->termios->tcgetattr_p(entry->vfs_ctx, entry->fd, p);
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
    return entry->ops->termios->tcdrain_p(entry->vfs_ctx, entry->fd);
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
    return entry->ops->termios->tcflush_p(entry->vfs_ctx, entry->fd, select);
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
    esp_err_t err = ESP_OK;
    _lock_init(&s_ctx_lock);
// Primary vfs part.
#if CONFIG_ESP_CONSOLE_UART
    const esp_vfs_fs_ops_t *ops = esp_vfs_uart_get_vfs();
    int fd = ops->open_p(NULL, "/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM), O_RDWR, 0);
    if (fd < 0) {
        ESP_EARLY_LOGE(TAG, "Failed to open primary UART sink, errno=%d", errno);
        return ESP_FAIL;
    }
    err = esp_stdio_register_sink(ops,
                                  ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC,
                                  NULL,
                                  "/" STRINGIFY(CONFIG_ESP_CONSOLE_UART_NUM),
                                  fd);
#elif CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    const esp_vfs_fs_ops_t *ops = esp_vfs_usb_serial_jtag_get_vfs();
    int fd = ops->open_p(NULL, "/", O_RDWR, 0);
    if (fd < 0) {
        ESP_EARLY_LOGE(TAG, "Failed to open primary USB JTAG sink, errno=%d", errno);
        return ESP_FAIL;
    }
    err = esp_stdio_register_sink(ops,
                                  ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC,
                                  NULL,
                                  "/",
                                  fd);
#elif CONFIG_ESP_CONSOLE_USB_CDC
    const esp_vfs_fs_ops_t *ops = esp_vfs_cdcacm_get_vfs();
    int fd = ops->open_p(NULL, "/", O_RDWR, 0);
    if (fd < 0) {
        ESP_EARLY_LOGE(TAG, "Failed to open primary USB CDC sink, errno=%d", errno);
        return ESP_FAIL;
    }
    err = esp_stdio_register_sink(ops,
                                  ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC,
                                  NULL,
                                  "/",
                                  fd);
#else
    const esp_vfs_fs_ops_t *ops = esp_vfs_null_get_vfs();
    int fd = ops->open_p(NULL, "/", O_RDWR, 0);
    if (fd < 0) {
        ESP_EARLY_LOGE(TAG, "Failed to open primary null sink, errno=%d", errno);
        return ESP_FAIL;
    }
    err = esp_stdio_register_sink(ops,
                                  ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC,
                                  NULL,
                                  "/",
                                  fd);
#endif

    if (err != ESP_OK) {
        return err;
    }

// Secondary vfs part.
#if CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG
    const esp_vfs_fs_ops_t *ops_secondary = esp_vfs_usb_serial_jtag_get_vfs();
    int fd_secondary = ops_secondary->open_p(NULL, "/", O_RDWR, 0);
    if (fd_secondary < 0) {
        ESP_EARLY_LOGE(TAG, "Failed to open secondary USB JTAG sink, errno=%d", errno);
        return ESP_FAIL;
    }
    err = esp_stdio_register_sink(ops_secondary,
                                  ESP_VFS_FLAG_CONTEXT_PTR | ESP_VFS_FLAG_STATIC,
                                  NULL,
                                  "/",
                                  fd_secondary);
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
