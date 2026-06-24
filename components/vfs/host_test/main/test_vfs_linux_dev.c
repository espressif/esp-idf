/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include "esp_vfs.h"
#include "sdkconfig.h"

#define NONE -1

typedef int (*open_func_t)(const char * path, int flags, ...);
typedef int (*close_func_t)(int s);
typedef ssize_t (*write_func_t)(int fd, const void * data, size_t size);
typedef ssize_t (*read_func_t)(int fd, void* data, size_t size);
typedef ssize_t (*pread_func_t)(int fd, void *dst, size_t size, off_t offset);
typedef ssize_t (*pwrite_func_t)(int fd, const void *src, size_t size, off_t offset);
typedef off_t (*lseek_func_t)(int fd, off_t size, int mode);
typedef int (*fstat_func_t)(int fd, struct stat *st);
typedef int (*fcntl_func_t)(int fd, int cmd, int arg);
typedef int (*fsync_func_t)(int fd);
typedef int (*ioctl_func_t)(int fd, int cmd, va_list args);
typedef int (*unlink_func_t)(const char *path);
typedef int (*ftruncate_func_t)(int fd, off_t length);

static open_func_t __real_open = NULL;
static close_func_t __real_close = NULL;
static write_func_t __real_write = NULL;
static read_func_t __real_read = NULL;
static pread_func_t __real_pread = NULL;
static pwrite_func_t __real_pwrite = NULL;
static lseek_func_t __real_lseek = NULL;
static fstat_func_t __real_fstat = NULL;
static fcntl_func_t __real_fcntl = NULL;
static fsync_func_t __real_fsync = NULL;
static ioctl_func_t __real_ioctl = NULL;
static unlink_func_t __real_unlink = NULL;
static ftruncate_func_t __real_ftruncate = NULL;

void __attribute__((constructor)) init_real_funcs(void)
{
    __real_open = (open_func_t) dlsym(RTLD_NEXT, "open");
    __real_close = (close_func_t) dlsym(RTLD_NEXT, "close");
    __real_write = (write_func_t) dlsym(RTLD_NEXT, "write");
    __real_read = (read_func_t) dlsym(RTLD_NEXT, "read");
    __real_pread = (pread_func_t) dlsym(RTLD_NEXT, "pread");
    __real_pwrite = (pwrite_func_t) dlsym(RTLD_NEXT, "pwrite");
    __real_lseek = (lseek_func_t) dlsym(RTLD_NEXT, "lseek");
    __real_fstat = (fstat_func_t) dlsym(RTLD_NEXT, "fstat");
    __real_fcntl = (fcntl_func_t) dlsym(RTLD_NEXT, "fcntl");
    __real_fsync = (fsync_func_t) dlsym(RTLD_NEXT, "fsync");
    __real_ioctl = (ioctl_func_t) dlsym(RTLD_NEXT, "ioctl");
    __real_unlink = (unlink_func_t) dlsym(RTLD_NEXT, "unlink");
    __real_ftruncate = (ftruncate_func_t) dlsym(RTLD_NEXT, "ftruncate");
}

static int linux_open(const char *path, int flags, int mode)
{
    return  __real_open(path+1, flags, mode);
}

static ssize_t linux_write(int fd, const void * data, size_t size)
{
    return  __real_write(fd, data, size);
}

static ssize_t linux_read(int fd, void* data, size_t size)
{
    return __real_read(fd, data, size);
}

static int linux_close(int fd)
{
    return __real_close(fd);
}

static ssize_t linux_pread(int fd, void *dst, size_t size, off_t offset)
{
    return __real_pread(fd, dst, size, offset);
}

static ssize_t linux_pwrite(int fd, const void *src, size_t size, off_t offset)
{
    return __real_pwrite(fd, src, size, offset);
}

static off_t linux_lseek(int fd, off_t size, int mode)
{
    return __real_lseek(fd, size, mode);
}

static int linux_fstat(int fd, struct stat *st)
{
    return __real_fstat(fd, st);
}

static int linux_fcntl(int fd, int cmd, int arg)
{
    return __real_fcntl(fd, cmd, arg);
}

static int linux_fsync(int fd)
{
    return __real_fsync(fd);
}

static int linux_ioctl(int fd, int request, va_list args)
{
    return __real_ioctl(fd, request, args);
}

static int linux_unlink(const char *path)
{
    return __real_unlink(path+1);
}

static int linux_ftruncate(int fd, off_t length)
{
    return __real_ftruncate(fd, length);
}

static const esp_vfs_t linux_vfs = {
    .flags = ESP_VFS_FLAG_DEFAULT,
    .write = &linux_write,
    .open = &linux_open,
    .close = &linux_close,
    .read = &linux_read,
    .pread = &linux_pread,
    .pwrite = &linux_pwrite,
    .lseek = &linux_lseek,
    .fstat = &linux_fstat,
    .fcntl = &linux_fcntl,
    .fsync = &linux_fsync,
    .ioctl = &linux_ioctl,
    .unlink = &linux_unlink,
    .ftruncate = &linux_ftruncate,
};

void linux_vfs_dev_register(void)
{
    ESP_ERROR_CHECK(esp_vfs_register("/linux", &linux_vfs, NULL));
}

void linux_vfs_dev_unregister(void)
{
    ESP_ERROR_CHECK(esp_vfs_unregister("/linux"));
}
