// Copyright 2020 Espressif Systems (Shanghai) Co. Ltd.
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

#include "soc/cpu.h"
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "hal/cpu_hal.h"
#include "openocd_semihosting.h"

#ifndef CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS
#define CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS 1
#endif

#ifndef CONFIG_VFS_SEMIHOSTFS_HOST_PATH_MAX_LEN
#define CONFIG_VFS_SEMIHOSTFS_HOST_PATH_MAX_LEN 128
#endif

const static char *TAG = "esp_semihost";


/* Additional open flags */


/* ESP-specific file open flag.
 * Indicates that path passed to open() is absolute host path.
 */
#define ESP_O_SEMIHOST_ABSPATH  0x80000000

/* There is no O_BINARY flag defined in newlib, as well as on Linux,
 * but we are leaving it to have the flags table identical to OpenOCD.
 */
#define O_BINARY 0

/* The table is identical to the one in OpenOCD semihosting_common.c */
static const int open_modeflags[12] = {
    O_RDONLY,
    O_RDONLY | O_BINARY,
    O_RDWR,
    O_RDWR | O_BINARY,
    O_WRONLY | O_CREAT | O_TRUNC,
    O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
    O_RDWR | O_CREAT | O_TRUNC,
    O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
    O_WRONLY | O_CREAT | O_APPEND,
    O_WRONLY | O_CREAT | O_APPEND | O_BINARY,
    O_RDWR | O_CREAT | O_APPEND,
    O_RDWR | O_CREAT | O_APPEND | O_BINARY
};

/**
 * @brief Get the number of appropriate file open mode set from open_modeflags and add some esp flags to them
 *
 * @param flags value, every bit of which reflects state of some open-file flag
 * @return index of the flag from @ref open_modeflags[], or -1 if invalid flags combination is given.
 */
static inline int get_o_mode(int flags) {
    if (flags & O_EXCL) { // bypassing lacking of this at table above
        flags &= ~(O_EXCL);
        flags |= O_CREAT;
    }

    for (int i = 0; i < sizeof(open_modeflags) / sizeof(open_modeflags[0]); i++) {
        if (flags == open_modeflags[i]) {
            return i;
        }
    }
    return -1; // there is no corresponding mode in the table
}

typedef struct {
    char base_path[ESP_VFS_PATH_MAX + 1];                    /* base path in VFS where host semihosting dir is mounted */
    char host_path[CONFIG_VFS_SEMIHOSTFS_HOST_PATH_MAX_LEN + 1]; /* host path to use as base dir for open files */
} vfs_semihost_ctx_t;

static vfs_semihost_ctx_t s_semhost_ctx[CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS];

static inline bool ctx_is_unused(const vfs_semihost_ctx_t* ctx)
{
    return ctx->base_path[0] == 0;
}

static inline bool ctx_uses_abspath(const vfs_semihost_ctx_t* ctx)
{
    return ctx->host_path[0];
}

#define FAIL_IF_NO_DEBUGGER() \
    do { \
        if (!cpu_hal_is_debugger_attached()) { \
            errno = EIO; \
            return -1; \
        } \
    } while(0)

#if __XTENSA__
static esp_err_t vfs_semihost_drvinfo(vfs_semihost_ctx_t *ctx)
{
    FAIL_IF_NO_DEBUGGER();

    int ret = semihosting_ver_info();
    if (ret == -1) {
        /* Unsupported syscall - old version of OpenOCD */
        return ESP_ERR_INVALID_VERSION;
    }
    return ESP_OK;
}
#endif // __XTENSA__

static int vfs_semihost_open(void* ctx, const char* path, int flags, int mode)
{
    int ret_fd = -1;
    char *host_path;
    vfs_semihost_ctx_t *semi_ctx = ctx;
    FAIL_IF_NO_DEBUGGER();

    ESP_LOGV(TAG, "%s: %p '%s 0x%x 0x%x'", __func__, semi_ctx, path, flags, mode);

    int o_mode = get_o_mode(flags);
    if (o_mode == -1) { /* if wrong flags - error */
        errno = EINVAL;
    } else {
        if (ctx_uses_abspath(semi_ctx)) {
            /* Create full absolute path on the host by concatenating host base
             * path and file path relative to the filesystem root.
             */
            host_path = malloc(strlen(semi_ctx->host_path) + strlen(path) + 1);
            if (host_path == NULL) { /* if no valid pointer - error and return */
                errno = ENOMEM;
                return -1;
            }
            strcpy(host_path, semi_ctx->host_path);
            strcat(host_path, path);
#ifdef __XTENSA__
            /* By default, OpenOCD for Xtensa prepends ESP_SEMIHOST_BASEDIR to
             * the path passed from the target. Adding this special flag to o_mode
             * inhibits this behavior.
             * This is not necessary for RISC-V since standard semihosting
             * implementation is used there and paths aren't mangled on OpenOCD side.
             */
            if (ctx_uses_abspath(semi_ctx)) {
                o_mode |= ESP_O_SEMIHOST_ABSPATH;
            }
#endif // __XTENSA__
        } else {
            host_path = (char *)path;
            /* For Xtensa targets in OpenOCD there is additional logic related to
             * semihosting paths handling that isn't there for other targets.
             * When ESP_SEMIHOST_BASEDIR OpenOCD variable is not set, OpenOCD will
             * by default prepend '.' to the path passed from the target.
             * By contrast, for RISC-V there is no such logic and the path will be
             * used as is, no matter whether it is absolute or relative.
             * See esp_xtensa_semihosting_get_file_name in esp_xtensa_semihosting.c
             * for details.
             */
#ifndef __XTENSA__
            if (*host_path == '/') {
                ++host_path;
            }
#endif // !__XTENSA__
        }
        /* everything is ready: syscall and cleanup */
        ret_fd = semihosting_open(host_path, o_mode, mode);
        if (ctx_uses_abspath(semi_ctx)) {
            free(host_path);
        }
    }
    return ret_fd;
}

static ssize_t vfs_semihost_write(void* ctx, int fd, const void * data, size_t size)
{
    FAIL_IF_NO_DEBUGGER();

    ESP_LOGV(TAG, "%s: %d %u bytes", __func__, fd, size);
    return semihosting_write(fd, data, size);
}

static ssize_t vfs_semihost_read(void* ctx, int fd, void* data, size_t size)
{
    FAIL_IF_NO_DEBUGGER();

    ESP_LOGV(TAG, "%s: %d %u bytes", __func__, fd, size);
    return semihosting_read(fd, data, size);
}


static int vfs_semihost_close(void* ctx, int fd)
{
    FAIL_IF_NO_DEBUGGER();

    ESP_LOGV(TAG, "%s: %d", __func__, fd);
    return semihosting_close(fd);
}

static off_t vfs_semihost_lseek(void* ctx, int fd, off_t offset, int mode)
{
    FAIL_IF_NO_DEBUGGER();

    return semihosting_seek(fd, offset, mode);
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
    if (!cpu_hal_is_debugger_attached()) {
        ESP_LOGE(TAG, "OpenOCD is not connected!");
        return ESP_ERR_NOT_SUPPORTED;
    }
    int i = 0;
    for (i = 0; i < CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS; i++) {
        if (ctx_is_unused(&s_semhost_ctx[i])) {
            break;
        }
        if (strcmp(base_path, s_semhost_ctx[i].base_path) == 0) {
            return ESP_ERR_INVALID_STATE;
        }
    }
    if (i == CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_semhost_ctx[i].base_path, base_path, sizeof(s_semhost_ctx[i].base_path) - 1);
    if (host_path) {
        strlcpy(s_semhost_ctx[i].host_path, host_path, sizeof(s_semhost_ctx[i].host_path) - 1);
    }
    ESP_LOGD(TAG, "Register semihosting driver %d %p", i, &s_semhost_ctx[i]);

    esp_err_t err;
#if __XTENSA__
    /* Check for older OpenOCD versions */
    err = vfs_semihost_drvinfo(&s_semhost_ctx[i]); // define semihosting version
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Incompatible OpenOCD version detected. Please follow the getting started guides to install the required version.");
    }
#endif // __XTENSA__

    err = esp_vfs_register(base_path, &vfs, &s_semhost_ctx[i]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't register the semihosting! Error: %s", esp_err_to_name(err));
        return err;
    }
    return err;
}

esp_err_t esp_vfs_semihost_unregister(const char* base_path)
{
    ESP_LOGD(TAG, "Unregister semihosting driver @ '%s'", base_path);
    int i = 0;
    for (i = 0; i < CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS; i++) {
        if (s_semhost_ctx[i].base_path[0] != 0 && strcmp(base_path, s_semhost_ctx[i].base_path) == 0) {
            break;
        }
    }
    if (i == CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS) {
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
