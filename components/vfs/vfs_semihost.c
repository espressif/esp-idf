/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_cpu.h"
#include "include/esp_vfs.h"
#include "openocd_semihosting.h"

#ifndef CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS
#define CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS 1
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

const static char *TAG = "esp_semihost";

typedef struct {
    char path[256];         /*!< VFS DIR stream path */
    struct dirent e;        /*!< Last open dirent */
    long id;                /*!< DIR* unique id */
} vfs_semihost_dir_t;

/* Additional open flags */

/* There is no O_BINARY flag defined in newlib, as well as on Linux,
 * but we are leaving it to have the flags table identical to OpenOCD.
 */
#define O_BINARY 0

/* The table is identical to the one in OpenOCD semihosting_common.c */
static const int open_modeflags[12] = {
	O_RDONLY,
	O_RDONLY | O_BINARY,
	O_RDWR,
	O_RDWR   | O_BINARY,
	O_WRONLY | O_CREAT | O_TRUNC,
	O_WRONLY | O_CREAT | O_TRUNC  | O_BINARY,
	O_RDWR   | O_CREAT | O_TRUNC,
	O_RDWR   | O_CREAT | O_TRUNC  | O_BINARY,
	O_WRONLY | O_CREAT | O_APPEND,
	O_WRONLY | O_CREAT | O_APPEND | O_BINARY,
	O_RDWR   | O_CREAT | O_APPEND,
	O_RDWR   | O_CREAT | O_APPEND | O_BINARY
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
} vfs_semihost_ctx_t;

static vfs_semihost_ctx_t s_semhost_ctx[CONFIG_VFS_SEMIHOSTFS_MAX_MOUNT_POINTS];

static inline bool ctx_is_unused(const vfs_semihost_ctx_t* ctx)
{
    return ctx->base_path[0] == 0;
}

#define FAIL_IF_NO_DEBUGGER() \
    do { \
        if (!esp_cpu_dbgr_is_attached()) { \
            errno = EIO; \
            return -1; \
        } \
    } while(0)

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

static int vfs_semihost_open(void* ctx, const char* path, int flags, int mode)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s 0x%x 0x%x'", __func__, path, flags, mode);
    int o_mode = get_o_mode(flags);
    if (o_mode == -1) { /* if wrong flags - error */
        errno = EINVAL;
        return -1;
    }
    return semihosting_open(path, o_mode, mode);
}

static ssize_t vfs_semihost_write(void* ctx, int fd, const void * data, size_t size)
{
    FAIL_IF_NO_DEBUGGER();

    if (data == NULL) {
        errno = EINVAL;
        return -1;
    }

    ESP_LOGV(TAG, "%s: %d %u bytes", __func__, fd, size);
    return semihosting_write(fd, data, size);
}

static ssize_t vfs_semihost_read(void* ctx, int fd, void* data, size_t size)
{
    FAIL_IF_NO_DEBUGGER();

    if (data == NULL) {
        errno = EINVAL;
        return -1;
    }

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

static int vfs_semihost_fstat(void* ctx, int fd, struct stat *restrict statbuf)
{
    FAIL_IF_NO_DEBUGGER();

    if (statbuf == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '0x%x'", __func__, fd);
    return semihosting_fstat(fd, statbuf);
}

static int vfs_semihost_fsync(void* ctx, int fd)
{
    FAIL_IF_NO_DEBUGGER();

    ESP_LOGV(TAG, "%s: '0x%x'", __func__, fd);
    return semihosting_fsync(fd);
}

#ifdef CONFIG_VFS_SUPPORT_DIR
static int vfs_semihost_mkdir(void* ctx, const char* path, mode_t mode)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s 0x%x'", __func__, path, (unsigned) mode);
    return semihosting_mkdir(path, mode);
}

static int vfs_semihost_rmdir(void* ctx, const char* path)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s'", __func__, path);
    return semihosting_rmdir(path);
}

static int vfs_semihost_access(void* ctx, const char* path, int mode)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s 0x%x'", __func__, path, mode);
    return semihosting_access(path, mode);
}

static int vfs_semihost_truncate(void* ctx, const char* path, off_t length)
{
    FAIL_IF_NO_DEBUGGER();

    if (length < 0) {
        errno = EINVAL;
        return -1;
    }

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s after %ld bytes'", __func__, path, length);
    return semihosting_truncate(path, length);
}

static int vfs_semihost_utime(void* ctx, const char* path, const struct utimbuf *times)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL || times == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s'", __func__, path);
    return semihosting_utime(path, times);
}

static int vfs_semihost_stat(void* ctx, const char *restrict path, struct stat *restrict statbuf)
{
    FAIL_IF_NO_DEBUGGER();

    if (statbuf == NULL || path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s'", __func__, path);
    return semihosting_stat(path, statbuf);
}

static int vfs_semihost_rename(void* ctx, const char *restrict old_name, const char *restrict new_name)
{
    FAIL_IF_NO_DEBUGGER();

    if (old_name == NULL || new_name == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s -> %s'", __func__, old_name, new_name);
    return semihosting_rename(old_name, new_name);
}

static int vfs_semihost_link(void* ctx, const char *restrict path1, const char *restrict path2)
{
    FAIL_IF_NO_DEBUGGER();

    if (path1 == NULL || path2 == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s <-> %s'", __func__, path1, path2);
    return semihosting_link(path1, path2);
}

static int vfs_semihost_unlink(void* ctx, const char *restrict path)
{
    FAIL_IF_NO_DEBUGGER();

    if (path == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: '%s'", __func__, path);
    return semihosting_unlink(path);
}

static DIR* vfs_semihost_opendir(void* ctx, const char *restrict path)
{
    if (!esp_cpu_dbgr_is_attached()) {
        return NULL;
    }

    if (path == NULL) {
        errno = ENOENT;
        return NULL;
    }

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t*)calloc(1, sizeof(vfs_semihost_dir_t));
    if (semihost_dirp == NULL) {
        ESP_LOGE("Error", "Error on vfs_semihost_dir_t creation");
        errno = ENOMEM;
        return NULL;
    }

    strncpy(semihost_dirp->path, path, sizeof(semihost_dirp->path) - 1);
    ESP_LOGV(TAG, "%s: '%s'", __func__, path);
    int ret_fd = semihosting_opendir(path, (int)&semihost_dirp->id);
    if (ret_fd < 0) {
        free(semihost_dirp);
        return NULL;
    }
    return (DIR *)semihost_dirp;
}

static int vfs_semihost_closedir(void* ctx, DIR* dirp)
{
    FAIL_IF_NO_DEBUGGER();

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t*)dirp;
    if (semihost_dirp == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: %s %ld", __func__, semihost_dirp->path, semihost_dirp->id);
    int ret_fd = semihosting_closedir(semihost_dirp->id);
    free(semihost_dirp);
    return ret_fd;
}

static long vfs_semihost_telldir(void* ctx, DIR* dirp)
{
    FAIL_IF_NO_DEBUGGER();

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t*)dirp;
    if (semihost_dirp == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: %s %ld", __func__, semihost_dirp->path, semihost_dirp->id);
    return semihosting_telldir(semihost_dirp->id);
}

static int vfs_semihost_readdir_r(void* ctx, DIR* dirp, struct dirent* entry, struct dirent** out_dirent)
{
    FAIL_IF_NO_DEBUGGER();

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t *)dirp;
    if (semihost_dirp == NULL || entry == NULL || out_dirent == NULL) {
        errno = ENOENT;
        return -1;
    }

    ESP_LOGV(TAG, "%s: %s %ld", __func__, semihost_dirp->path, semihost_dirp->id);
    int ret_fd = semihosting_readdir((long)entry, semihost_dirp->id);
    if (ret_fd < 0) {
        if (errno == 0) { /* end of directory */
            *out_dirent = NULL;
            return 0;
        }
        return errno;
    }
    *out_dirent = entry;
    return 0;
}

static struct dirent* vfs_semihost_readdir(void* ctx, DIR* dirp)
{
    if (!esp_cpu_dbgr_is_attached()) {
        return NULL;
    }

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t *)dirp;
    struct dirent *dir_ptr;
    ESP_LOGV(TAG, "%s: %s %ld", __func__, semihost_dirp->path, semihost_dirp->id);
    int ret = vfs_semihost_readdir_r(ctx, (DIR*)semihost_dirp, &semihost_dirp->e, &dir_ptr);
    if (ret != 0) {
        errno = ret;
        return NULL;
    }
    return dir_ptr;
}

static void vfs_semihost_seekdir(void* ctx, DIR* pdir, long offset)
{
    if (!esp_cpu_dbgr_is_attached()) {
        return;
    }

    vfs_semihost_dir_t *semihost_dirp = (vfs_semihost_dir_t *) pdir;
    if (semihost_dirp != NULL) {
        ESP_LOGV(TAG, "%s: %s %ld '%ld' bytes", __func__, semihost_dirp->path, semihost_dirp->id, offset);
        semihosting_seekdir(semihost_dirp->id, offset);
    }
}
#endif

#ifdef CONFIG_VFS_SUPPORT_DIR
static const esp_vfs_dir_ops_t s_vfs_semihost_dir = {
    .mkdir_p = &vfs_semihost_mkdir,
    .rmdir_p = &vfs_semihost_rmdir,
    .access_p = &vfs_semihost_access,
    .truncate_p = &vfs_semihost_truncate,
    .utime_p = &vfs_semihost_utime,
    .stat_p = &vfs_semihost_stat,
    .rename_p = &vfs_semihost_rename,
    .link_p = &vfs_semihost_link,
    .unlink_p = &vfs_semihost_unlink,
    .opendir_p = &vfs_semihost_opendir,
    .closedir_p = &vfs_semihost_closedir,
    .telldir_p = &vfs_semihost_telldir,
    .readdir_p = &vfs_semihost_readdir,
    .readdir_r_p = &vfs_semihost_readdir_r,
    .seekdir_p = &vfs_semihost_seekdir,
};
#endif

static const esp_vfs_fs_ops_t s_vfs_semihost = {
    .write_p = &vfs_semihost_write,
    .open_p = &vfs_semihost_open,
    .close_p = &vfs_semihost_close,
    .read_p = &vfs_semihost_read,
    .lseek_p = &vfs_semihost_lseek,
    .fsync_p = &vfs_semihost_fsync,
    .fstat_p = &vfs_semihost_fstat,
#ifdef CONFIG_VFS_SUPPORT_DIR
    .dir = &s_vfs_semihost_dir,
#endif
};

esp_err_t esp_vfs_semihost_register(const char* base_path)
{
    assert(base_path);

    ESP_LOGD(TAG, "Register semihosting driver '%s'", base_path);
    if (!esp_cpu_dbgr_is_attached()) {
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

    ESP_LOGD(TAG, "Register semihosting driver %d %p", i, &s_semhost_ctx[i]);

    esp_err_t err;
    /* Check for older OpenOCD versions */
    err = vfs_semihost_drvinfo(&s_semhost_ctx[i]); // define semihosting version
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Incompatible OpenOCD version detected. Please follow the getting started guides to install the required version.");
    }

    err = esp_vfs_register_fs(base_path, &s_vfs_semihost, ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, &s_semhost_ctx[i]);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't register the semihosting! Error: %s", esp_err_to_name(err));
        return err;
    }
    return err;
}

esp_err_t esp_vfs_semihost_unregister(const char* base_path)
{
    assert(base_path);

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
    ESP_LOGD(TAG, "Unregistered semihosting driver @ '%s'", base_path);
    return ESP_OK;
}
