/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Linux target VFS host filesystem registration + init
 *
 * Registers the cooperative libc ops (backed by freertos_linux_coop_*)
 * as a VFS filesystem using esp_vfs_register_fs_with_id, so that
 * file descriptors registered against this VFS ID are dispatched
 * through the cooperative syscall layer.
 *
 * stdin/stdout/stderr are registered with VFS here, and all other
 * kernel FDs are registered by the strong POSIX overrides in vfs_linux.c.
 *
 * Non-blocking syscalls (lseek, fstat, ioctl, fsync) are resolved directly
 * via dlsym and bypass the cooperative interposition layer because they
 * complete synchronously and never return EAGAIN/EWOULDBLOCK.
 */

#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_vfs.h"
#include "esp_private/freertos_linux_coop_syscalls.h"
#include "esp_private/startup_internal.h"

/* VFS ID for the Linux host filesystem — used by vfs_linux.c to register FDs */
esp_vfs_id_t s_linux_host_vfs_id = -1;

/* Real libc entry points for non-blocking syscalls, resolved at startup */
static off_t (*real_lseek)(int, off_t, int);
static int   (*real_fstat)(int, struct stat *);
static int   (*real_fsync)(int);

typedef int (*real_ioctl_fn_t)(int, unsigned long, ...);
static real_ioctl_fn_t real_ioctl;

static ssize_t default_read(void *ctx, int fd, void *dst, size_t size)
{
    (void)ctx;
    return freertos_linux_coop_read(fd, dst, size);
}

static ssize_t default_write(void *ctx, int fd, const void *data, size_t size)
{
    (void)ctx;
    return freertos_linux_coop_write(fd, data, size);
}

static ssize_t default_pread(void *ctx, int fd, void *dst, size_t size, off_t offset)
{
    (void)ctx;
    return freertos_linux_coop_pread(fd, dst, size, offset);
}

static ssize_t default_pwrite(void *ctx, int fd, const void *src, size_t size, off_t offset)
{
    (void)ctx;
    return freertos_linux_coop_pwrite(fd, src, size, offset);
}

static int default_open(void *ctx, const char *path, int flags, int mode)
{
    (void)ctx;
    return freertos_linux_coop_open(path, flags, mode);
}

static int default_close(void *ctx, int fd)
{
    (void)ctx;
    return freertos_linux_coop_close(fd);
}

static int default_fcntl(void *ctx, int fd, int cmd, int arg)
{
    (void)ctx;
    return freertos_linux_coop_fcntl(fd, cmd, arg);
}

/* lseek never blocks — call real libc directly, no cooperative retry needed */
static off_t default_lseek(void *ctx, int fd, off_t offset, int whence)
{
    (void)ctx;
    return real_lseek(fd, offset, whence);
}

/* fstat never blocks — call real libc directly, no cooperative retry needed */
static int default_fstat(void *ctx, int fd, struct stat *st)
{
    (void)ctx;
    return real_fstat(fd, st);
}

/* ioctl never blocks — call real libc directly, no cooperative retry needed */
static int default_ioctl(void *ctx, int fd, int cmd, va_list args)
{
    (void)ctx;
    void *arg = va_arg(args, void *);
    return real_ioctl(fd, (unsigned long)cmd, arg);
}

/* fsync never blocks — call real libc directly, no cooperative retry needed */
static int default_fsync(void *ctx, int fd)
{
    (void)ctx;
    return real_fsync(fd);
}

static const esp_vfs_fs_ops_t s_linux_host_ops = {
    .read_p   = default_read,
    .write_p  = default_write,
    .pread_p  = default_pread,
    .pwrite_p = default_pwrite,
    .open_p   = default_open,
    .close_p  = default_close,
    .fcntl_p  = default_fcntl,
    .lseek_p  = default_lseek,
    .fstat_p  = default_fstat,
    .ioctl_p  = default_ioctl,
    .fsync_p  = default_fsync,
};

ESP_SYSTEM_INIT_FN(init_vfs_linux_coop, CORE, BIT(0), 99)
{
    /* Resolve real libc symbols for non-blocking syscalls */
    real_lseek = dlsym(RTLD_NEXT, "lseek");
    real_fstat = dlsym(RTLD_NEXT, "fstat");
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    real_fsync = dlsym(RTLD_NEXT, "fsync");

    freertos_linux_coop_syscalls_init();

    /* Register the Linux host FS with VFS (id-based, no path prefix) */
    esp_err_t err = esp_vfs_register_fs_with_id(&s_linux_host_ops,
                                                 ESP_VFS_FLAG_CONTEXT_PTR,
                                                 NULL,
                                                 &s_linux_host_vfs_id);
    if (err != ESP_OK) {
        return err;
    }

    /* Register stdin/stdout/stderr so apps see them as VFS FDs 0, 1, 2 */
    int fd;
    esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, STDIN_FILENO,  true, &fd);
    esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, STDOUT_FILENO, true, &fd);
    esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, STDERR_FILENO, true, &fd);

    return ESP_OK;
}

void esp_vfs_include_linux_coop_init(void)
{
    // Linker hook function, exists to make the linker examine this file
}
