/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/reent.h>
#include <sys/lock.h>
#include <dirent.h>
#include "inttypes_ext.h"
#include "freertos/FreeRTOS.h"
#include "esp_vfs.h"
#include "esp_vfs_private.h"
#include "esp_private/socket.h"
#include "sdkconfig.h"

// Warn about using deprecated option
#ifdef CONFIG_LWIP_USE_ONLY_LWIP_SELECT
#warning CONFIG_LWIP_USE_ONLY_LWIP_SELECT is deprecated: Please use CONFIG_VFS_SUPPORT_SELECT instead
#endif

#ifdef CONFIG_VFS_SUPPRESS_SELECT_DEBUG_OUTPUT
#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#endif //CONFIG_VFS_SUPPRESS_SELECT_DEBUG_OUTPUT
#include "esp_log.h"

static const char *TAG = "vfs";

/* Max number of VFS entries (registered filesystems) */
#ifdef CONFIG_VFS_MAX_COUNT
#define VFS_MAX_COUNT  CONFIG_VFS_MAX_COUNT
#else
/* If IO support is disabled, keep this defined to 1 to avoid compiler warnings in this file.
 * The s_vfs array and the functions defined here will be removed by the linker, anyway.
 */
#define VFS_MAX_COUNT 1
#endif

#define LEN_PATH_PREFIX_IGNORED SIZE_MAX /* special length value for VFS which is never recognised by open() */
#define FD_TABLE_ENTRY_UNUSED   (fd_table_t) { .permanent = false, .has_pending_close = false, .has_pending_select = false, .vfs_index = -1, .local_fd = -1 }

_Static_assert((1 << (sizeof(local_fd_t)*8)) >= MAX_FDS, "file descriptor type too small");

_Static_assert((1 << (sizeof(vfs_index_t)*8)) >= VFS_MAX_COUNT, "VFS index type too small");
_Static_assert(((vfs_index_t) -1) < 0, "vfs_index_t must be a signed type");

static vfs_entry_t* s_vfs[VFS_MAX_COUNT] = { 0 };
static size_t s_vfs_count = 0;

static fd_table_t s_fd_table[MAX_FDS] = { [0 ... MAX_FDS-1] = FD_TABLE_ENTRY_UNUSED };
static _lock_t s_fd_table_lock;

static ssize_t esp_get_free_index(void) {
    for (ssize_t i = 0; i < VFS_MAX_COUNT; i++) {
        if (s_vfs[i] == NULL) {
            return i;
        }
    }
    return -1;
}

static void esp_vfs_free_fs_ops(esp_vfs_fs_ops_t *vfs) {
// We can afford to cast away the const qualifier here, because we know that we allocated the struct and therefore its safe
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    free((void*)vfs->termios);
#endif

#ifdef CONFIG_VFS_SUPPORT_DIR
    free((void*)vfs->dir);
#endif

#ifdef CONFIG_VFS_SUPPORT_SELECT
    free((void*)vfs->select);
#endif

    free(vfs);
}

static void esp_vfs_free_entry(vfs_entry_t *entry) {
    if (entry == NULL) { // Necessary because of the following flags check
        return;
    }

    if (!(entry->flags & ESP_VFS_FLAG_STATIC)) {
        esp_vfs_free_fs_ops((esp_vfs_fs_ops_t*)entry->vfs); // const cast, but we know it's not static from the flag
    }

    free(entry);
}

typedef struct {
#ifdef CONFIG_VFS_SUPPORT_DIR
    esp_vfs_dir_ops_t *dir;
#endif
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    esp_vfs_termios_ops_t *termios;
#endif
#ifdef CONFIG_VFS_SUPPORT_SELECT
    esp_vfs_select_ops_t *select;
#endif
} vfs_component_proxy_t;

static void free_proxy_members(vfs_component_proxy_t *proxy) {
#ifdef CONFIG_VFS_SUPPORT_DIR
    free(proxy->dir);
#endif
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    free(proxy->termios);
#endif
#ifdef CONFIG_VFS_SUPPORT_SELECT
    free(proxy->select);
#endif
}

static esp_vfs_fs_ops_t *esp_minify_vfs(const esp_vfs_t * const vfs, vfs_component_proxy_t proxy) {
    assert(vfs != NULL);

#ifdef CONFIG_VFS_SUPPORT_DIR
    // If the dir functions are not implemented, we don't need to convert them
    if (proxy.dir != NULL) {
        esp_vfs_dir_ops_t tmp = {
            .stat = vfs->stat,
            .link = vfs->link,
            .unlink = vfs->unlink,
            .rename = vfs->rename,
            .opendir = vfs->opendir,
            .readdir = vfs->readdir,
            .readdir_r = vfs->readdir_r,
            .telldir = vfs->telldir,
            .seekdir = vfs->seekdir,
            .closedir = vfs->closedir,
            .mkdir = vfs->mkdir,
            .rmdir = vfs->rmdir,
            .access = vfs->access,
            .truncate = vfs->truncate,
            .ftruncate = vfs->ftruncate,
            .utime = vfs->utime,
        };

        memcpy(proxy.dir, &tmp, sizeof(esp_vfs_dir_ops_t));
    }
#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    // If the termios functions are not implemented, we don't need to convert them
    if (proxy.termios != NULL) {
        esp_vfs_termios_ops_t tmp = {
            .tcsetattr = vfs->tcsetattr,
            .tcgetattr = vfs->tcgetattr,
            .tcdrain = vfs->tcdrain,
            .tcflush = vfs->tcflush,
            .tcflow = vfs->tcflow,
            .tcgetsid = vfs->tcgetsid,
            .tcsendbreak = vfs->tcsendbreak,
        };

        memcpy(proxy.termios, &tmp, sizeof(esp_vfs_termios_ops_t));
    }
#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifdef CONFIG_VFS_SUPPORT_SELECT
    // If the select functions are not implemented, we don't need to convert them
    if (proxy.select != NULL) {
        esp_vfs_select_ops_t tmp = {
            .start_select = vfs->start_select,
            .socket_select = vfs->socket_select,
            .stop_socket_select = vfs->stop_socket_select,
            .stop_socket_select_isr = vfs->stop_socket_select_isr,
            .get_socket_select_semaphore = vfs->get_socket_select_semaphore,
            .end_select = vfs->end_select,
        };

        memcpy(proxy.select, &tmp, sizeof(esp_vfs_select_ops_t));
    }
#endif // CONFIG_VFS_SUPPORT_SELECT

    esp_vfs_fs_ops_t tmp = {
        .write = vfs->write,
        .lseek = vfs->lseek,
        .read = vfs->read,
        .pread = vfs->pread,
        .pwrite = vfs->pwrite,
        .open = vfs->open,
        .close = vfs->close,
        .fstat = vfs->fstat,
        .fcntl = vfs->fcntl,
        .ioctl = vfs->ioctl,
        .fsync = vfs->fsync,
#ifdef CONFIG_VFS_SUPPORT_DIR
        .dir = proxy.dir,
#endif
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
        .termios = proxy.termios,
#endif
#ifdef CONFIG_VFS_SUPPORT_SELECT
        .select = proxy.select,
#endif
    };

    esp_vfs_fs_ops_t *out = heap_caps_malloc(sizeof(esp_vfs_fs_ops_t), VFS_MALLOC_FLAGS);
    if (out == NULL) {
        return NULL;
    }

    // Doing this is the only way to correctly initialize const members of a struct according to C standard
    memcpy(out, &tmp, sizeof(esp_vfs_fs_ops_t));

    return out;
}


static esp_vfs_fs_ops_t* esp_vfs_duplicate_fs_ops(const esp_vfs_fs_ops_t *orig) {
    vfs_component_proxy_t proxy = {};

#ifdef CONFIG_VFS_SUPPORT_DIR
    if (orig->dir != NULL) {
        proxy.dir = (esp_vfs_dir_ops_t*) heap_caps_malloc(sizeof(esp_vfs_dir_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.dir == NULL) {
            goto fail;
        }
        memcpy(proxy.dir, orig->dir, sizeof(esp_vfs_dir_ops_t));
    }
#endif

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    if (orig->termios != NULL) {
        proxy.termios = (esp_vfs_termios_ops_t*) heap_caps_malloc(sizeof(esp_vfs_termios_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.termios == NULL) {
            goto fail;
        }
        memcpy(proxy.termios, orig->termios, sizeof(esp_vfs_termios_ops_t));
    }
#endif

#ifdef CONFIG_VFS_SUPPORT_SELECT
    if (orig->select != NULL) {
        proxy.select = (esp_vfs_select_ops_t*) heap_caps_malloc(sizeof(esp_vfs_select_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.select == NULL) {
            goto fail;
        }
        memcpy(proxy.select, orig->select, sizeof(esp_vfs_select_ops_t));
    }
#endif

    // This tediousness is required because of const members
    esp_vfs_fs_ops_t tmp = {
        .write = orig->write,
        .lseek = orig->lseek,
        .read = orig->read,
        .pread = orig->pread,
        .pwrite = orig->pwrite,
        .open = orig->open,
        .close = orig->close,
        .fstat = orig->fstat,
        .fcntl = orig->fcntl,
        .ioctl = orig->ioctl,
        .fsync = orig->fsync,
#ifdef CONFIG_VFS_SUPPORT_DIR
        .dir = proxy.dir,
#endif
#ifdef CONFIG_VFS_SUPPORT_TERMIOS
        .termios = proxy.termios,
#endif
#ifdef CONFIG_VFS_SUPPORT_SELECT
        .select = proxy.select,
#endif
    };

    esp_vfs_fs_ops_t *out = heap_caps_malloc(sizeof(esp_vfs_fs_ops_t), VFS_MALLOC_FLAGS);
    if (out == NULL) {
        goto fail;
    }

    memcpy(out, &tmp, sizeof(esp_vfs_fs_ops_t));

    return out;

fail:
    free_proxy_members(&proxy);
    return NULL;
}

static esp_err_t esp_vfs_make_fs_ops(const esp_vfs_t *vfs, esp_vfs_fs_ops_t **min) {
    if (vfs == NULL) {
        ESP_LOGE(TAG, "Cannot minify NULL VFS");
        return ESP_ERR_INVALID_ARG;
    }

    if (min == NULL) {
        ESP_LOGE(TAG, "Cannot minify VFS to NULL");
        return ESP_ERR_INVALID_ARG;
    }

    vfs_component_proxy_t proxy = {};

#ifdef CONFIG_VFS_SUPPORT_DIR
    const bool skip_dir =
        vfs->stat == NULL &&
        vfs->link == NULL &&
        vfs->unlink == NULL &&
        vfs->rename == NULL &&
        vfs->opendir == NULL &&
        vfs->readdir == NULL &&
        vfs->readdir_r == NULL &&
        vfs->telldir == NULL &&
        vfs->seekdir == NULL &&
        vfs->closedir == NULL &&
        vfs->mkdir == NULL &&
        vfs->rmdir == NULL &&
        vfs->access == NULL &&
        vfs->truncate == NULL &&
        vfs->ftruncate == NULL &&
        vfs->utime == NULL;

    if (!skip_dir) {
        proxy.dir = (esp_vfs_dir_ops_t*) heap_caps_malloc(sizeof(esp_vfs_dir_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.dir == NULL) {
            goto fail;
        }
    }
#endif

#ifdef CONFIG_VFS_SUPPORT_TERMIOS
    const bool skip_termios =
        vfs->tcsetattr == NULL &&
        vfs->tcgetattr == NULL &&
        vfs->tcdrain == NULL &&
        vfs->tcflush == NULL &&
        vfs->tcflow == NULL &&
        vfs->tcgetsid == NULL &&
        vfs->tcsendbreak == NULL;

    if (!skip_termios) {
        proxy.termios = (esp_vfs_termios_ops_t*) heap_caps_malloc(sizeof(esp_vfs_termios_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.termios == NULL) {
            goto fail;
        }
    }
#endif

#ifdef CONFIG_VFS_SUPPORT_SELECT
    const bool skip_select =
        vfs->start_select == NULL &&
        vfs->socket_select == NULL &&
        vfs->stop_socket_select == NULL &&
        vfs->stop_socket_select_isr == NULL &&
        vfs->get_socket_select_semaphore == NULL &&
        vfs->end_select == NULL;

    if (!skip_select) {
        proxy.select = (esp_vfs_select_ops_t*) heap_caps_malloc(sizeof(esp_vfs_select_ops_t), VFS_MALLOC_FLAGS);
        if (proxy.select == NULL) {
            goto fail;
        }
    }
#endif

    esp_vfs_fs_ops_t *main = esp_minify_vfs(vfs, proxy);
    if (main == NULL) {
        goto fail;
    }

    *min = main;
    return ESP_OK;

fail:

    free_proxy_members(&proxy);
    return ESP_ERR_NO_MEM;
}

static bool is_path_prefix_valid(const char *path, size_t length) {
    return (length >= 2)
        && (length <= ESP_VFS_PATH_MAX)
        && (path[0] == '/')
        && (path[length - 1] != '/');
}

static esp_err_t esp_vfs_register_fs_common(
    const char *base_path,
    const esp_vfs_fs_ops_t *vfs,
    int flags,
    void *ctx,
    int *vfs_index)
{
    if (s_vfs_count >= VFS_MAX_COUNT) {
        return  ESP_ERR_NO_MEM;
    }

    if (vfs == NULL) {
        ESP_LOGE(TAG, "VFS is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    size_t base_path_len = 0;
    const char *_base_path = "";

    if (base_path != NULL) {
        base_path_len = strlen(base_path);
        _base_path = base_path;

        if (base_path_len != 0 && !is_path_prefix_valid(base_path, base_path_len)) {
            ESP_LOGE(TAG, "Invalid path prefix");
            return ESP_ERR_INVALID_ARG;
        }
    }

    ssize_t index = esp_get_free_index();
    if (index < 0) {
        return ESP_ERR_NO_MEM;
    }

    if (s_vfs[index] != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (index == s_vfs_count) {
        s_vfs_count++;
    }

    vfs_entry_t *entry = heap_caps_malloc(sizeof(vfs_entry_t) + base_path_len + 1, VFS_MALLOC_FLAGS);
    if (entry == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_vfs[index] = entry;

    entry->path_prefix_len = base_path == NULL ? LEN_PATH_PREFIX_IGNORED : base_path_len;
    entry->vfs = vfs;
    entry->ctx = ctx;
    entry->offset = index;
    entry->flags = flags;

    memcpy((char *)(entry->path_prefix), _base_path, base_path_len + 1);

    if (vfs_index) {
        *vfs_index = index;
    }

    return ESP_OK;
}

esp_err_t esp_vfs_register_fs(const char* base_path, const esp_vfs_fs_ops_t* vfs, int flags, void* ctx)
{
    if (vfs == NULL) {
        ESP_LOGE(TAG, "VFS is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (base_path == NULL) {
        ESP_LOGE(TAG, "base_path cannot be null");
        return ESP_ERR_INVALID_ARG;
    }

    if ((flags & ESP_VFS_FLAG_STATIC)) {
        return esp_vfs_register_fs_common(base_path, vfs, flags, ctx, NULL);
    }

    esp_vfs_fs_ops_t *_vfs = esp_vfs_duplicate_fs_ops(vfs);
    if (_vfs == NULL) {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret = esp_vfs_register_fs_common(base_path, _vfs, flags, ctx, NULL);
    if (ret != ESP_OK) {
        esp_vfs_free_fs_ops(_vfs);
        return ret;
    }

    return ESP_OK;
}

esp_err_t esp_vfs_register_common(const char* base_path, size_t len, const esp_vfs_t* vfs, void* ctx, int *vfs_index)
{
    if (vfs == NULL) {
        ESP_LOGE(TAG, "VFS is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (vfs->flags & ESP_VFS_FLAG_STATIC) {
        ESP_LOGE(TAG, "ESP_VFS_FLAG_STATIC is not supported for esp_vfs_t, use esp_vfs_register_fs instead");
        return ESP_ERR_INVALID_ARG;
    }

    if (len != LEN_PATH_PREFIX_IGNORED) {
        if (base_path == NULL) {
            ESP_LOGE(TAG, "Path prefix cannot be NULL");
            return ESP_ERR_INVALID_ARG;
        }

        // This is for backwards compatibility
        if (strlen(base_path) != len) {
            ESP_LOGE(TAG, "Mismatch between real and given path prefix lengths.");
            return ESP_ERR_INVALID_ARG;
        }
    } else {
        // New API expects NULL in this case
        base_path = NULL;
    }

    esp_vfs_fs_ops_t *_vfs = NULL;
    esp_err_t ret = esp_vfs_make_fs_ops(vfs, &_vfs);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_vfs_register_fs_common(base_path, _vfs, vfs->flags, ctx, vfs_index);
    if (ret != ESP_OK) {
        esp_vfs_free_fs_ops(_vfs);
        return ret;
    }

    return ESP_OK;
}

esp_err_t esp_vfs_register(const char* base_path, const esp_vfs_t* vfs, void* ctx)
{
    return esp_vfs_register_common(base_path, strlen(base_path), vfs, ctx, NULL);
}

esp_err_t esp_vfs_register_with_id(const esp_vfs_t *vfs, void *ctx, esp_vfs_id_t *vfs_id)
{
    if (vfs_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *vfs_id = -1;
    return esp_vfs_register_common("", LEN_PATH_PREFIX_IGNORED, vfs, ctx, vfs_id);
}

esp_err_t esp_vfs_register_fd_range(const esp_vfs_fs_ops_t *vfs, int flags, void *ctx, int min_fd, int max_fd)
{
    if (min_fd < 0 || max_fd < 0 || min_fd > MAX_FDS || max_fd > MAX_FDS || min_fd > max_fd) {
        ESP_LOGD(TAG, "Invalid arguments: esp_vfs_register_fd_range(0x%p, 0x%p, %d, %d)", vfs, ctx, min_fd, max_fd);
        return ESP_ERR_INVALID_ARG;
    }

    int index = 0;
    esp_err_t ret = esp_vfs_register_fs_common(NULL, vfs, flags, ctx, &index);

    if (ret == ESP_OK) {
        _lock_acquire(&s_fd_table_lock);
        for (int i = min_fd; i < max_fd; ++i) {
            if (s_fd_table[i].vfs_index != -1) {
                free(s_vfs[index]);
                s_vfs[index] = NULL;
                for (int j = min_fd; j < i; ++j) {
                    if (s_fd_table[j].vfs_index == index) {
                        s_fd_table[j] = FD_TABLE_ENTRY_UNUSED;
                    }
                }
                _lock_release(&s_fd_table_lock);
                ESP_LOGW(TAG, "esp_vfs_register_fd_range cannot set fd %d (used by other VFS)", i);
                return ESP_ERR_INVALID_ARG;
            }
            s_fd_table[i].permanent = true;
            s_fd_table[i].vfs_index = index;
            s_fd_table[i].local_fd = i;
        }
        _lock_release(&s_fd_table_lock);

        ESP_LOGD(TAG, "esp_vfs_register_fd_range is successful for range <%d; %d) and VFS ID %d", min_fd, max_fd, index);
    }

    return ret;
}

esp_err_t esp_vfs_register_fs_with_id(const esp_vfs_fs_ops_t *vfs, int flags, void *ctx, esp_vfs_id_t *vfs_id)
{
    if (vfs_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *vfs_id = -1;
    return esp_vfs_register_fs_common(NULL, vfs, flags, ctx, vfs_id);
}

esp_err_t esp_vfs_unregister_with_id(esp_vfs_id_t vfs_id)
{
    if (vfs_id < 0 || vfs_id >= VFS_MAX_COUNT || s_vfs[vfs_id] == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    vfs_entry_t* vfs = s_vfs[vfs_id];
    esp_vfs_free_entry(vfs);
    s_vfs[vfs_id] = NULL;

    _lock_acquire(&s_fd_table_lock);
    // Delete all references from the FD lookup-table
    for (int j = 0; j < VFS_MAX_COUNT; ++j) {
        if (s_fd_table[j].vfs_index == vfs_id) {
            s_fd_table[j] = FD_TABLE_ENTRY_UNUSED;
        }
    }
    _lock_release(&s_fd_table_lock);

    return ESP_OK;

}

#ifndef CONFIG_IDF_TARGET_LINUX
esp_err_t esp_vfs_unregister_fs_with_id(esp_vfs_id_t vfs_id) __attribute__((alias("esp_vfs_unregister_with_id")));
#endif

esp_err_t esp_vfs_unregister(const char* base_path)
{
    const size_t base_path_len = strlen(base_path);
    for (size_t i = 0; i < s_vfs_count; ++i) {
        vfs_entry_t* vfs = s_vfs[i];
        if (vfs == NULL) {
            continue;
        }
        if (base_path_len == vfs->path_prefix_len &&
                memcmp(base_path, vfs->path_prefix, vfs->path_prefix_len) == 0) {
            return esp_vfs_unregister_with_id(i);
        }
    }
    return ESP_ERR_INVALID_STATE;
}

#ifndef CONFIG_IDF_TARGET_LINUX
esp_err_t esp_vfs_unregister_fs(const char* base_path) __attribute__((alias("esp_vfs_unregister")));
#endif

esp_err_t esp_vfs_register_fd(esp_vfs_id_t vfs_id, int *fd)
{
    return esp_vfs_register_fd_with_local_fd(vfs_id, -1, true, fd);
}

esp_err_t esp_vfs_register_fd_with_local_fd(esp_vfs_id_t vfs_id, int local_fd, bool permanent, int *fd)
{
    if (vfs_id < 0 || vfs_id >= s_vfs_count || fd == NULL) {
        ESP_LOGD(TAG, "Invalid arguments for esp_vfs_register_fd_with_local_fd(%d, %d, %d, 0x%p)",
                 vfs_id, local_fd, permanent, fd);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_ERR_NO_MEM;
    _lock_acquire(&s_fd_table_lock);
    for (int i = 0; i < MAX_FDS; ++i) {
        if (s_fd_table[i].vfs_index == -1) {
            s_fd_table[i].permanent = permanent;
            s_fd_table[i].vfs_index = vfs_id;
            if (local_fd >= 0) {
                s_fd_table[i].local_fd = local_fd;
            } else {
                s_fd_table[i].local_fd = i;
            }
            *fd = i;
            ret = ESP_OK;
            break;
        }
    }
    _lock_release(&s_fd_table_lock);

    ESP_LOGD(TAG, "esp_vfs_register_fd_with_local_fd(%d, %d, %d, 0x%p) finished with %s",
             vfs_id, local_fd, permanent, fd, esp_err_to_name(ret));

    return ret;
}

esp_err_t esp_vfs_unregister_fd(esp_vfs_id_t vfs_id, int fd)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;

    if (vfs_id < 0 || vfs_id >= s_vfs_count || fd < 0 || fd >= MAX_FDS) {
        ESP_LOGD(TAG, "Invalid arguments for esp_vfs_unregister_fd(%d, %d)", vfs_id, fd);
        return ret;
    }

    _lock_acquire(&s_fd_table_lock);
    fd_table_t *item = s_fd_table + fd;
    if (item->permanent == true && item->vfs_index == vfs_id && item->local_fd == fd) {
        *item = FD_TABLE_ENTRY_UNUSED;
        ret = ESP_OK;
    }
    _lock_release(&s_fd_table_lock);

    ESP_LOGD(TAG, "esp_vfs_unregister_fd(%d, %d) finished with %s", vfs_id, fd, esp_err_to_name(ret));

    return ret;
}

void esp_vfs_dump_fds(FILE *fp)
{
    const vfs_entry_t* vfs;
    fprintf(fp, "------------------------------------------------------\n");
    fprintf(fp, "<VFS Path Prefix>-<FD seen by App>-<FD seen by driver>\n");
    fprintf(fp, "------------------------------------------------------\n");
    _lock_acquire(&s_fd_table_lock);
    for (int index = 0; index < MAX_FDS; index++) {
        if (s_fd_table[index].vfs_index != -1) {
            vfs = s_vfs[s_fd_table[index].vfs_index];
            if (strcmp(vfs->path_prefix, "")) {
                fprintf(fp, "(%s) - 0x%x - 0x%x\n", vfs->path_prefix, index, s_fd_table[index].local_fd);
            } else {
                fprintf(fp, "(socket) - 0x%x - 0x%x\n", index, s_fd_table[index].local_fd);
            }
        }
    }
    _lock_release(&s_fd_table_lock);
}

void esp_vfs_dump_registered_paths(FILE *fp)
{
    fprintf(fp, "------------------------------------------------------\n");
    fprintf(fp, "<index>:<VFS Path Prefix> -> <VFS entry ptr>\n");
    fprintf(fp, "------------------------------------------------------\n");
    for (size_t i = 0; i < VFS_MAX_COUNT; ++i) {
        fprintf(
            fp,
            "%" PRIuSIZE ": %s -> %p\n",
            i,
            s_vfs[i] ? s_vfs[i]->path_prefix : "NULL",
            s_vfs[i] ? s_vfs[i]->vfs : NULL
        );
    }
}

/*
 * Set ESP_VFS_FLAG_READONLY_FS read-only flag for a registered virtual filesystem
 * for given path prefix. Should be only called from the esp_vfs_*filesystem* register
 * or helper mount functions where vfs_t is not available to set the read-only
 * flag directly (e.g. esp_vfs_fat_spiflash_mount_rw_wl).
 */
esp_err_t esp_vfs_set_readonly_flag(const char* base_path)
{
    const size_t base_path_len = strlen(base_path);
    for (size_t i = 0; i < s_vfs_count; ++i) {
        vfs_entry_t* vfs = s_vfs[i];
        if (vfs == NULL) {
            continue;
        }
        if (base_path_len == vfs->path_prefix_len &&
                memcmp(base_path, vfs->path_prefix, vfs->path_prefix_len) == 0) {
            vfs->flags |= ESP_VFS_FLAG_READONLY_FS;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

const vfs_entry_t *get_vfs_for_index(int index)
{
    if (index < 0 || index >= s_vfs_count) {
        return NULL;
    } else {
        return s_vfs[index];
    }
}

int register_fd(int vfs_index, int local_fd, bool permanent)
{
    _lock_acquire(&s_fd_table_lock);
    for (int i = 0; i < MAX_FDS; ++i) {
        if (s_fd_table[i].vfs_index == -1) {
            s_fd_table[i].permanent = permanent;
            s_fd_table[i].vfs_index = vfs_index;
            s_fd_table[i].local_fd = local_fd;
            _lock_release(&s_fd_table_lock);
            return i;
        }
    }
    _lock_release(&s_fd_table_lock);
    return -1;
}

void unregister_fd(int fd)
{
    _lock_acquire(&s_fd_table_lock);

    // Do not close permanent FDs
    if (s_fd_table[fd].permanent) {
        _lock_release(&s_fd_table_lock);
        return;
    }

    // If the FD is in use by select, mark it for closure
    if (s_fd_table[fd].has_pending_select) {
        s_fd_table[fd].has_pending_close = true;
    } else {
        s_fd_table[fd] = FD_TABLE_ENTRY_UNUSED;
    }
    _lock_release(&s_fd_table_lock);
}

static inline bool fd_valid(int fd)
{
    return (fd < MAX_FDS) && (fd >= 0);
}

const fd_table_t *get_fd_entry(int fd)
{
    return fd_valid(fd) ? &s_fd_table[fd] : NULL;
}

const vfs_entry_t *get_vfs_for_fd(int fd)
{
    const vfs_entry_t *vfs = NULL;
    if (fd_valid(fd)) {
        const int index = s_fd_table[fd].vfs_index; // single read -> no locking is required
        vfs = get_vfs_for_index(index);
    }
    return vfs;
}

int get_local_fd(const vfs_entry_t *vfs, int fd)
{
    int local_fd = -1;

    if (vfs && fd_valid(fd)) {
        local_fd = s_fd_table[fd].local_fd; // single read -> no locking is required
    }

    return local_fd;
}

const char* translate_path(const vfs_entry_t* vfs, const char* src_path)
{
    assert(strncmp(src_path, vfs->path_prefix, vfs->path_prefix_len) == 0);
    if (strlen(src_path) == vfs->path_prefix_len) {
        // special case when src_path matches the path prefix exactly
        return "/";
    }
    return src_path + vfs->path_prefix_len;
}

const vfs_entry_t* get_vfs_for_path(const char* path)
{
    const vfs_entry_t* best_match = NULL;
    ssize_t best_match_prefix_len = -1;
    size_t len = strlen(path);
    for (size_t i = 0; i < s_vfs_count; ++i) {
        const vfs_entry_t* vfs = s_vfs[i];
        if (vfs == NULL || vfs->path_prefix_len == LEN_PATH_PREFIX_IGNORED) {
            continue;
        }
        // match path prefix
        if (len < vfs->path_prefix_len ||
            memcmp(path, vfs->path_prefix, vfs->path_prefix_len) != 0) {
            continue;
        }
        // this is the default VFS and we don't have a better match yet.
        if (vfs->path_prefix_len == 0 && !best_match) {
            best_match = vfs;
            continue;
        }
        // if path is not equal to the prefix, expect to see a path separator
        // i.e. don't match "/data" prefix for "/data1/foo.txt" path
        if (len > vfs->path_prefix_len &&
                path[vfs->path_prefix_len] != '/') {
            continue;
        }
        // Out of all matching path prefixes, select the longest one;
        // i.e. if "/dev" and "/dev/uart" both match, for "/dev/uart/1" path,
        // choose "/dev/uart",
        // This causes all s_vfs_count VFS entries to be scanned when opening
        // a file by name. This can be optimized by introducing a table for
        // FS search order, sorted so that longer prefixes are checked first.
        if (best_match_prefix_len < (ssize_t) vfs->path_prefix_len) {
            best_match_prefix_len = (ssize_t) vfs->path_prefix_len;
            best_match = vfs;
        }
    }
    return best_match;
}

size_t get_vfs_count(void)
{
    return s_vfs_count;
}

void close_pending(int nfds)
{
    _lock_acquire(&s_fd_table_lock);
    for (int fd = 0; fd < nfds; ++fd) {
        if (s_fd_table[fd].has_pending_close) {
            s_fd_table[fd] = FD_TABLE_ENTRY_UNUSED;
        }
    }
    _lock_release(&s_fd_table_lock);
}

fd_table_t start_select(int fd, fd_set *errorfds)
{
    _lock_acquire(&s_fd_table_lock);

    // Set flag
    if (esp_vfs_safe_fd_isset(fd, errorfds)) {
        s_fd_table[fd].has_pending_select = true;
    }

    // Make a copy
    fd_table_t out = s_fd_table[fd];

    _lock_release(&s_fd_table_lock);

    return out;
}
