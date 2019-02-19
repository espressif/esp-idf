// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "soc/cpu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs.h"

#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#include "esp_log.h"
const static char *TAG = "esp_semihost";

#define SYSCALL_INSTR           "break 1,1\n"
#define SYS_OPEN                0x01
#define SYS_CLOSE               0x02
#define SYS_WRITE               0x05
#define SYS_READ                0x06
#define SYS_SEEK                0x0A

/** ESP-specific file open flag. Indicates that path passed to open() is absolute host path. */
#define ESP_O_SEMIHOST_ABSPATH  0x80000000

typedef struct {
    char base_path[ESP_VFS_PATH_MAX+1];                 /* base path in VFS where host semohosting dir is mounted */
    char host_path[CONFIG_SEMIHOSTFS_HOST_PATH_MAX_LEN+1];    /* host path to use as base dir for open files */
} vfs_semihost_ctx_t;

static vfs_semihost_ctx_t s_semhost_ctx[CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS];


static inline int generic_syscall(int sys_nr, int arg1, int arg2, int arg3, int arg4, int* ret_errno)
{
    int host_ret, host_errno;

    if (!esp_cpu_in_ocd_debug_mode()) {
        *ret_errno = EIO;
        return -1;
    }
    __asm__ volatile (
        "mov a2, %[sys_nr]\n" \
        "mov a3, %[arg1]\n" \
        "mov a4, %[arg2]\n" \
        "mov a5, %[arg3]\n" \
        "mov a6, %[arg4]\n" \
        SYSCALL_INSTR \
        "mov %[host_ret], a2\n" \
        "mov %[host_errno], a3\n" \
        :[host_ret]"=r"(host_ret),[host_errno]"=r"(host_errno)
        :[sys_nr]"r"(sys_nr),[arg1]"r"(arg1),[arg2]"r"(arg2),[arg3]"r"(arg3),[arg4]"r"(arg4)
        :"a2","a3","a4","a5","a6");
    *ret_errno = host_errno;
    return host_ret;
}

inline bool ctx_is_unused(const vfs_semihost_ctx_t* ctx)
{
    return ctx->base_path[0] == 0;
}

inline bool ctx_uses_abspath(const vfs_semihost_ctx_t* ctx)
{
    return ctx->host_path[0];
}

static int vfs_semihost_open(void* ctx, const char * path, int flags, int mode)
{
    int fd = -1, host_err = 0;
    char *host_path;
    vfs_semihost_ctx_t *semi_ctx = ctx;

    ESP_LOGV(TAG, "%s: %p '%s 0x%x 0x%x'", __func__, semi_ctx, path, flags, mode);
    if (ctx_uses_abspath(semi_ctx)) {
        flags |= ESP_O_SEMIHOST_ABSPATH;
        host_path = malloc(strlen(semi_ctx->host_path)+strlen(path)+1);
        if(host_path == NULL) {
            errno = ENOMEM;
            return -1;
        }
        strcpy(host_path, semi_ctx->host_path);
        strcat(host_path, path);
    } else {
        host_path = (char *)path;
    }
    fd = generic_syscall(SYS_OPEN, (int)host_path, strlen(host_path), flags, mode, &host_err);
    if (ctx_uses_abspath(semi_ctx)) {
        free(host_path);
    }
    if (fd == -1) {
        errno = host_err;
    }
    return fd;
}

static ssize_t vfs_semihost_write(void* ctx, int fd, const void * data, size_t size)
{
    int host_err = 0;
    size_t ret = -1;

    ESP_LOGV(TAG, "%s: %d %u bytes", __func__, fd, size);
    ret = generic_syscall(SYS_WRITE, fd, (int)data, size, 0, &host_err);
    if (ret == -1) {
        errno = host_err;
    }
    return (ssize_t)ret;
}

static ssize_t vfs_semihost_read(void* ctx, int fd, void* data, size_t size)
{
    int host_err = 0;
    size_t ret = -1;

    ESP_LOGV(TAG, "%s: %d %u bytes", __func__, fd, size);
    ret = generic_syscall(SYS_READ, fd, (int)data, size, 0, &host_err);
    if (ret == -1) {
        errno = host_err;
    }
    return (ssize_t)ret;
}

static int vfs_semihost_close(void* ctx, int fd)
{
    int ret = -1, host_err = 0;

    ESP_LOGV(TAG, "%s: %d", __func__, fd);
    ret = generic_syscall(SYS_CLOSE, fd, 0, 0, 0, &host_err);
    if (ret == -1) {
        errno = host_err;
    }
    return ret;
}

static off_t vfs_semihost_lseek(void* ctx, int fd, off_t size, int mode)
{
    int ret = -1, host_err = 0;

    ESP_LOGV(TAG, "%s: %d %ld %d", __func__, fd, size, mode);
    ret = generic_syscall(SYS_SEEK, fd, size, mode, 0, &host_err);
    if (ret == -1) {
        errno = host_err;
    }
    return (off_t)ret;
}

esp_err_t esp_vfs_semihost_register(const char* base_path, const char* host_path)
{
    const esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .write_p = &vfs_semihost_write,
        .open_p = &vfs_semihost_open,
        .close_p = &vfs_semihost_close,
        .read_p = &vfs_semihost_read,
        .lseek_p = &vfs_semihost_lseek,
    };
    ESP_LOGD(TAG, "Register semihosting driver '%s' -> '%s'", base_path, host_path ? host_path : "null");
    if (!esp_cpu_in_ocd_debug_mode()) {
        ESP_LOGE(TAG, "OpenOCD is not connected!");
        return ESP_ERR_NOT_SUPPORTED;
    }
    int i = 0;
    for (i = 0; i < CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS; i++) {
        if (ctx_is_unused(&s_semhost_ctx[i])) {
            break;
        }
        if (strcmp(base_path, s_semhost_ctx[i].base_path) == 0) {
            return ESP_ERR_INVALID_STATE;
        }
    }
    if (i == CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_semhost_ctx[i].base_path, base_path, sizeof(s_semhost_ctx[i].base_path) - 1);
    if (host_path) {
        strlcpy(s_semhost_ctx[i].host_path, host_path, sizeof(s_semhost_ctx[i].host_path) - 1);
    }
    ESP_LOGD(TAG, "Register semihosting driver %d %p", i, &s_semhost_ctx[i]);
    return esp_vfs_register(base_path, &vfs, &s_semhost_ctx[i]);
}

esp_err_t esp_vfs_semihost_unregister(const char* base_path)
{
    ESP_LOGD(TAG, "Unregister semihosting driver @ '%s'", base_path);
    int i = 0;
    for (i = 0; i < CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS; i++) {
        if (s_semhost_ctx[i].base_path[0] != 0 && strcmp(base_path, s_semhost_ctx[i].base_path) == 0) {
            break;
        }
    }
    if (i == CONFIG_SEMIHOSTFS_MAX_MOUNT_POINTS) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = esp_vfs_unregister(s_semhost_ctx[i].base_path);
    if (ret != ESP_OK) {
        return ret;
    }
    s_semhost_ctx[i].base_path[0] = 0;
    s_semhost_ctx[i].host_path[0] = 0;
    ESP_LOGD(TAG, "Unregistered semihosting driver @ '%s'", base_path);
    return ESP_OK;
}
