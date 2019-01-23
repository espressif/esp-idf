// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include <dirent.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_vfs.h"
#include "sdkconfig.h"

#ifdef CONFIG_SUPPRESS_SELECT_DEBUG_OUTPUT
#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#endif //CONFIG_SUPPRESS_SELECT_DEBUG_OUTPUT
#include "esp_log.h"

static const char *TAG = "vfs";

#define VFS_MAX_COUNT   8   /* max number of VFS entries (registered filesystems) */
#define LEN_PATH_PREFIX_IGNORED SIZE_MAX /* special length value for VFS which is never recognised by open() */
#define FD_TABLE_ENTRY_UNUSED   (fd_table_t) { .permanent = false, .vfs_index = -1, .local_fd = -1 }

typedef uint8_t local_fd_t;
_Static_assert((1 << (sizeof(local_fd_t)*8)) >= MAX_FDS, "file descriptor type too small");

typedef int8_t vfs_index_t;
_Static_assert((1 << (sizeof(vfs_index_t)*8)) >= VFS_MAX_COUNT, "VFS index type too small");
_Static_assert(((vfs_index_t) -1) < 0, "vfs_index_t must be a signed type");

typedef struct {
    bool permanent;
    vfs_index_t vfs_index;
    local_fd_t local_fd;
} fd_table_t;

typedef struct vfs_entry_ {
    esp_vfs_t vfs;          // contains pointers to VFS functions
    char path_prefix[ESP_VFS_PATH_MAX]; // path prefix mapped to this VFS
    size_t path_prefix_len; // micro-optimization to avoid doing extra strlen
    void* ctx;              // optional pointer which can be passed to VFS
    int offset;             // index of this structure in s_vfs array
} vfs_entry_t;

typedef struct {
    bool isset; // none or at least one bit is set in the following 3 fd sets
    fd_set readfds;
    fd_set writefds;
    fd_set errorfds;
} fds_triple_t;

static vfs_entry_t* s_vfs[VFS_MAX_COUNT] = { 0 };
static size_t s_vfs_count = 0;

static fd_table_t s_fd_table[MAX_FDS] = { [0 ... MAX_FDS-1] = FD_TABLE_ENTRY_UNUSED };
static _lock_t s_fd_table_lock;

static esp_err_t esp_vfs_register_common(const char* base_path, size_t len, const esp_vfs_t* vfs, void* ctx, int *vfs_index)
{
    if (len != LEN_PATH_PREFIX_IGNORED) {
        if ((len != 0 && len < 2) || (len > ESP_VFS_PATH_MAX)) {
            return ESP_ERR_INVALID_ARG;
        }
        if ((len > 0 && base_path[0] != '/') || base_path[len - 1] == '/') {
            return ESP_ERR_INVALID_ARG;
        }
    }
    vfs_entry_t *entry = (vfs_entry_t*) malloc(sizeof(vfs_entry_t));
    if (entry == NULL) {
        return ESP_ERR_NO_MEM;
    }
    size_t index;
    for (index = 0; index < s_vfs_count; ++index) {
        if (s_vfs[index] == NULL) {
            break;
        }
    }
    if (index == s_vfs_count) {
        if (s_vfs_count >= VFS_MAX_COUNT) {
            free(entry);
            return ESP_ERR_NO_MEM;
        }
        ++s_vfs_count;
    }
    s_vfs[index] = entry;
    if (len != LEN_PATH_PREFIX_IGNORED) {
        strcpy(entry->path_prefix, base_path); // we have already verified argument length
    } else {
        bzero(entry->path_prefix, sizeof(entry->path_prefix));
    }
    memcpy(&entry->vfs, vfs, sizeof(esp_vfs_t));
    entry->path_prefix_len = len;
    entry->ctx = ctx;
    entry->offset = index;

    if (vfs_index) {
        *vfs_index = index;
    }

    return ESP_OK;
}

esp_err_t esp_vfs_register(const char* base_path, const esp_vfs_t* vfs, void* ctx)
{
    return esp_vfs_register_common(base_path, strlen(base_path), vfs, ctx, NULL);
}

esp_err_t esp_vfs_register_fd_range(const esp_vfs_t *vfs, void *ctx, int min_fd, int max_fd)
{
    if (min_fd < 0 || max_fd < 0 || min_fd > MAX_FDS || max_fd > MAX_FDS || min_fd > max_fd) {
        ESP_LOGD(TAG, "Invalid arguments: esp_vfs_register_fd_range(0x%x, 0x%x, %d, %d)", (int) vfs, (int) ctx, min_fd, max_fd);
        return ESP_ERR_INVALID_ARG;
    }

    int index = -1;
    esp_err_t ret = esp_vfs_register_common("", LEN_PATH_PREFIX_IGNORED, vfs, ctx, &index);

    if (ret == ESP_OK) {
        _lock_acquire(&s_fd_table_lock);
        for (int i = min_fd; i < max_fd; ++i) {
            if (s_fd_table[i].vfs_index != -1) {
                free(s_vfs[i]);
                s_vfs[i] = NULL;
                for (int j = min_fd; j < i; ++j) {
                    if (s_fd_table[j].vfs_index == index) {
                        s_fd_table[j] = FD_TABLE_ENTRY_UNUSED;
                    }
                }
                _lock_release(&s_fd_table_lock);
                ESP_LOGD(TAG, "esp_vfs_register_fd_range cannot set fd %d (used by other VFS)", i);
                return ESP_ERR_INVALID_ARG;
            }
            s_fd_table[i].permanent = true;
            s_fd_table[i].vfs_index = index;
            s_fd_table[i].local_fd = i;
        }
        _lock_release(&s_fd_table_lock);
    }

    ESP_LOGD(TAG, "esp_vfs_register_fd_range is successful for range <%d; %d) and VFS ID %d", min_fd, max_fd, index);

    return ret;
}

esp_err_t esp_vfs_register_with_id(const esp_vfs_t *vfs, void *ctx, esp_vfs_id_t *vfs_id)
{
    if (vfs_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *vfs_id = -1;
    return esp_vfs_register_common("", LEN_PATH_PREFIX_IGNORED, vfs, ctx, vfs_id);
}

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
            free(vfs);
            s_vfs[i] = NULL;

            _lock_acquire(&s_fd_table_lock);
            // Delete all references from the FD lookup-table
            for (int j = 0; j < MAX_FDS; ++j) {
                if (s_fd_table[j].vfs_index == i) {
                    s_fd_table[j] = FD_TABLE_ENTRY_UNUSED;
                }
            }
            _lock_release(&s_fd_table_lock);

            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

esp_err_t esp_vfs_register_fd(esp_vfs_id_t vfs_id, int *fd)
{
    if (vfs_id < 0 || vfs_id >= s_vfs_count || fd == NULL) {
        ESP_LOGD(TAG, "Invalid arguments for esp_vfs_register_fd(%d, 0x%x)", vfs_id, (int) fd);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_ERR_NO_MEM;
    _lock_acquire(&s_fd_table_lock);
    for (int i = 0; i < MAX_FDS; ++i) {
        if (s_fd_table[i].vfs_index == -1) {
            s_fd_table[i].permanent = true;
            s_fd_table[i].vfs_index = vfs_id;
            s_fd_table[i].local_fd = i;
            *fd = i;
            ret = ESP_OK;
            break;
        }
    }
    _lock_release(&s_fd_table_lock);

    ESP_LOGD(TAG, "esp_vfs_register_fd(%d, 0x%x) finished with %s", vfs_id, (int) fd, esp_err_to_name(ret));

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

static inline const vfs_entry_t *get_vfs_for_index(int index)
{
    if (index < 0 || index >= s_vfs_count) {
        return NULL;
    } else {
        return s_vfs[index];
    }
}

static inline bool fd_valid(int fd)
{
    return (fd < MAX_FDS) && (fd >= 0);
}

static const vfs_entry_t *get_vfs_for_fd(int fd)
{
    const vfs_entry_t *vfs = NULL;
    if (fd_valid(fd)) {
        const int index = s_fd_table[fd].vfs_index; // single read -> no locking is required
        vfs = get_vfs_for_index(index);
    }
    return vfs;
}

static inline int get_local_fd(const vfs_entry_t *vfs, int fd)
{
    int local_fd = -1;

    if (vfs && fd_valid(fd)) {
        local_fd = s_fd_table[fd].local_fd; // single read -> no locking is required
    }

    return local_fd;
}

static const char* translate_path(const vfs_entry_t* vfs, const char* src_path)
{
    assert(strncmp(src_path, vfs->path_prefix, vfs->path_prefix_len) == 0);
    if (strlen(src_path) == vfs->path_prefix_len) {
        // special case when src_path matches the path prefix exactly
        return "/";
    }
    return src_path + vfs->path_prefix_len;
}

static const vfs_entry_t* get_vfs_for_path(const char* path)
{
    const vfs_entry_t* best_match = NULL;
    ssize_t best_match_prefix_len = -1;
    size_t len = strlen(path);
    for (size_t i = 0; i < s_vfs_count; ++i) {
        const vfs_entry_t* vfs = s_vfs[i];
        if (!vfs || vfs->path_prefix_len == LEN_PATH_PREFIX_IGNORED) {
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

/*
 * Using huge multi-line macros is never nice, but in this case
 * the only alternative is to repeat this chunk of code (with different function names)
 * for each syscall being implemented. Given that this define is contained within a single
 * file, this looks like a good tradeoff.
 *
 * First we check if syscall is implemented by VFS (corresponding member is not NULL),
 * then call the right flavor of the method (e.g. open or open_p) depending on
 * ESP_VFS_FLAG_CONTEXT_PTR flag. If ESP_VFS_FLAG_CONTEXT_PTR is set, context is passed
 * in as first argument and _p variant is used for the call.
 * It is enough to check just one of them for NULL, as both variants are part of a union.
 */
#define CHECK_AND_CALL(ret, r, pvfs, func, ...) \
    if (pvfs->vfs.func == NULL) { \
        __errno_r(r) = ENOSYS; \
        return -1; \
    } \
    if (pvfs->vfs.flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs.func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs.func)(__VA_ARGS__);\
    }


#define CHECK_AND_CALLV(r, pvfs, func, ...) \
    if (pvfs->vfs.func == NULL) { \
        __errno_r(r) = ENOSYS; \
        return; \
    } \
    if (pvfs->vfs.flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        (*pvfs->vfs.func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        (*pvfs->vfs.func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALLP(ret, r, pvfs, func, ...) \
    if (pvfs->vfs.func == NULL) { \
        __errno_r(r) = ENOSYS; \
        return NULL; \
    } \
    if (pvfs->vfs.flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs.func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs.func)(__VA_ARGS__);\
    }

int esp_vfs_open(struct _reent *r, const char * path, int flags, int mode)
{
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char *path_within_vfs = translate_path(vfs, path);
    int fd_within_vfs;
    CHECK_AND_CALL(fd_within_vfs, r, vfs, open, path_within_vfs, flags, mode);
    if (fd_within_vfs >= 0) {
        _lock_acquire(&s_fd_table_lock);
        for (int i = 0; i < MAX_FDS; ++i) {
            if (s_fd_table[i].vfs_index == -1) {
                s_fd_table[i].permanent = false;
                s_fd_table[i].vfs_index = vfs->offset;
                s_fd_table[i].local_fd = fd_within_vfs;
                _lock_release(&s_fd_table_lock);
                return i;
            }
        }
        _lock_release(&s_fd_table_lock);
        int ret;
        CHECK_AND_CALL(ret, r, vfs, close, fd_within_vfs);
        (void) ret; // remove "set but not used" warning
        __errno_r(r) = ENOMEM;
        return -1;
    }
    __errno_r(r) = ENOENT;
    return -1;
}

ssize_t esp_vfs_write(struct _reent *r, int fd, const void * data, size_t size)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, write, local_fd, data, size);
    return ret;
}

off_t esp_vfs_lseek(struct _reent *r, int fd, off_t size, int mode)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    off_t ret;
    CHECK_AND_CALL(ret, r, vfs, lseek, local_fd, size, mode);
    return ret;
}

ssize_t esp_vfs_read(struct _reent *r, int fd, void * dst, size_t size)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, read, local_fd, dst, size);
    return ret;
}


int esp_vfs_close(struct _reent *r, int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, close, local_fd);

    _lock_acquire(&s_fd_table_lock);
    if (!s_fd_table[fd].permanent) {
        s_fd_table[fd] = FD_TABLE_ENTRY_UNUSED;
    }
    _lock_release(&s_fd_table_lock);
    return ret;
}

int esp_vfs_fstat(struct _reent *r, int fd, struct stat * st)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fstat, local_fd, st);
    return ret;
}

int esp_vfs_stat(struct _reent *r, const char * path, struct stat * st)
{
    const vfs_entry_t* vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, path);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, stat, path_within_vfs, st);
    return ret;
}

int esp_vfs_link(struct _reent *r, const char* n1, const char* n2)
{
    const vfs_entry_t* vfs = get_vfs_for_path(n1);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const vfs_entry_t* vfs2 = get_vfs_for_path(n2);
    if (vfs != vfs2) {
        __errno_r(r) = EXDEV;
        return -1;
    }
    const char* path1_within_vfs = translate_path(vfs, n1);
    const char* path2_within_vfs = translate_path(vfs, n2);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, link, path1_within_vfs, path2_within_vfs);
    return ret;
}

int esp_vfs_unlink(struct _reent *r, const char *path)
{
    const vfs_entry_t* vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, path);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, unlink, path_within_vfs);
    return ret;
}

int esp_vfs_rename(struct _reent *r, const char *src, const char *dst)
{
    const vfs_entry_t* vfs = get_vfs_for_path(src);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const vfs_entry_t* vfs_dst = get_vfs_for_path(dst);
    if (vfs != vfs_dst) {
        __errno_r(r) = EXDEV;
        return -1;
    }
    const char* src_within_vfs = translate_path(vfs, src);
    const char* dst_within_vfs = translate_path(vfs, dst);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, rename, src_within_vfs, dst_within_vfs);
    return ret;
}

DIR* opendir(const char* name)
{
    const vfs_entry_t* vfs = get_vfs_for_path(name);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return NULL;
    }
    const char* path_within_vfs = translate_path(vfs, name);
    DIR* ret;
    CHECK_AND_CALLP(ret, r, vfs, opendir, path_within_vfs);
    if (ret != NULL) {
        ret->dd_vfs_idx = vfs->offset;
    }
    return ret;
}

struct dirent* readdir(DIR* pdir)
{
    const vfs_entry_t* vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
       __errno_r(r) = EBADF;
        return NULL;
    }
    struct dirent* ret;
    CHECK_AND_CALLP(ret, r, vfs, readdir, pdir);
    return ret;
}

int readdir_r(DIR* pdir, struct dirent* entry, struct dirent** out_dirent)
{
    const vfs_entry_t* vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, readdir_r, pdir, entry, out_dirent);
    return ret;
}

long telldir(DIR* pdir)
{
    const vfs_entry_t* vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    long ret;
    CHECK_AND_CALL(ret, r, vfs, telldir, pdir);
    return ret;
}

void seekdir(DIR* pdir, long loc)
{
    const vfs_entry_t* vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return;
    }
    CHECK_AND_CALLV(r, vfs, seekdir, pdir, loc);
}

void rewinddir(DIR* pdir)
{
    seekdir(pdir, 0);
}

int closedir(DIR* pdir)
{
    const vfs_entry_t* vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, closedir, pdir);
    return ret;
}

int mkdir(const char* name, mode_t mode)
{
    const vfs_entry_t* vfs = get_vfs_for_path(name);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, name);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, mkdir, path_within_vfs, mode);
    return ret;
}

int rmdir(const char* name)
{
    const vfs_entry_t* vfs = get_vfs_for_path(name);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, name);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, rmdir, path_within_vfs);
    return ret;
}

int fcntl(int fd, int cmd, ...)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    va_list args;
    va_start(args, cmd);
    CHECK_AND_CALL(ret, r, vfs, fcntl, local_fd, cmd, args);
    va_end(args);
    return ret;
}

int ioctl(int fd, int cmd, ...)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    va_list args;
    va_start(args, cmd);
    CHECK_AND_CALL(ret, r, vfs, ioctl, local_fd, cmd, args);
    va_end(args);
    return ret;
}

int fsync(int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fsync, local_fd);
    return ret;
}

int access(const char *path, int amode)
{
    int ret;
    const vfs_entry_t* vfs = get_vfs_for_path(path);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, path);
    CHECK_AND_CALL(ret, r, vfs, access, path_within_vfs, amode);
    return ret;
}

int truncate(const char *path, off_t length)
{
    int ret;
    const vfs_entry_t* vfs = get_vfs_for_path(path);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, path);
    CHECK_AND_CALL(ret, r, vfs, truncate, path_within_vfs, length);
    return ret;
}

static void call_end_selects(int end_index, const fds_triple_t *vfs_fds_triple)
{
    for (int i = 0; i < end_index; ++i) {
        const vfs_entry_t *vfs = get_vfs_for_index(i);
        const fds_triple_t *item = &vfs_fds_triple[i];
        if (vfs && vfs->vfs.end_select && item->isset) {
            vfs->vfs.end_select();
        }
    }
}

static inline bool esp_vfs_safe_fd_isset(int fd, const fd_set *fds)
{
    return fds && FD_ISSET(fd, fds);
}

static int set_global_fd_sets(const fds_triple_t *vfs_fds_triple, int size, fd_set *readfds, fd_set *writefds, fd_set *errorfds)
{
    int ret = 0;

    for (int i = 0; i < size; ++i) {
        const fds_triple_t *item = &vfs_fds_triple[i];
        if (item->isset) {
            for (int fd = 0; fd < MAX_FDS; ++fd) {
                const int local_fd = s_fd_table[fd].local_fd; // single read -> no locking is required
                if (readfds && esp_vfs_safe_fd_isset(local_fd, &item->readfds)) {
                    ESP_LOGD(TAG, "FD %d in readfds was set from VFS ID %d", fd, i);
                    FD_SET(fd, readfds);
                    ++ret;
                }
                if (writefds && esp_vfs_safe_fd_isset(local_fd, &item->writefds)) {
                    ESP_LOGD(TAG, "FD %d in writefds was set from VFS ID %d", fd, i);
                    FD_SET(fd, writefds);
                    ++ret;
                }
                if (errorfds && esp_vfs_safe_fd_isset(local_fd, &item->errorfds)) {
                    ESP_LOGD(TAG, "FD %d in errorfds was set from VFS ID %d", fd, i);
                    FD_SET(fd, errorfds);
                    ++ret;
                }
            }
        }
    }

    return ret;
}

static void esp_vfs_log_fd_set(const char *fds_name, const fd_set *fds)
{
    if (fds_name && fds) {
        ESP_LOGD(TAG, "FDs in %s =", fds_name);
        for (int i = 0; i < MAX_FDS; ++i) {
            if (esp_vfs_safe_fd_isset(i, fds)) {
                ESP_LOGD(TAG, "%d", i);
            }
        }
    }
}

int esp_vfs_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    int ret = 0;
    struct _reent* r = __getreent();

    ESP_LOGD(TAG, "esp_vfs_select starts with nfds = %d", nfds);
    if (timeout) {
        ESP_LOGD(TAG, "timeout is %lds + %ldus", timeout->tv_sec, timeout->tv_usec);
    }
    esp_vfs_log_fd_set("readfds", readfds);
    esp_vfs_log_fd_set("writefds", writefds);
    esp_vfs_log_fd_set("errorfds", errorfds);

    if (nfds > MAX_FDS || nfds < 0) {
        ESP_LOGD(TAG, "incorrect nfds");
        __errno_r(r) = EINVAL;
        return -1;
    }

    fds_triple_t *vfs_fds_triple;
    if ((vfs_fds_triple = calloc(s_vfs_count, sizeof(fds_triple_t))) == NULL) {
        __errno_r(r) = ENOMEM;
        ESP_LOGD(TAG, "calloc is unsuccessful");
        return -1;
    }

    int (*socket_select)(int, fd_set *, fd_set *, fd_set *, struct timeval *) = NULL;
    for (int fd = 0; fd < nfds; ++fd) {
        _lock_acquire(&s_fd_table_lock);
        const bool is_socket_fd = s_fd_table[fd].permanent;
        const int vfs_index = s_fd_table[fd].vfs_index;
        const int local_fd = s_fd_table[fd].local_fd;
        _lock_release(&s_fd_table_lock);

        if (vfs_index < 0) {
            continue;
        }

        if (is_socket_fd) {
            if (!socket_select) {
                // no socket_select found yet so take a look
                if (esp_vfs_safe_fd_isset(fd, readfds) ||
                        esp_vfs_safe_fd_isset(fd, writefds) ||
                        esp_vfs_safe_fd_isset(fd, errorfds)) {
                    const vfs_entry_t *vfs = s_vfs[vfs_index];
                    socket_select = vfs->vfs.socket_select;
                }
            }
            continue;
        }

        fds_triple_t *item = &vfs_fds_triple[vfs_index]; // FD sets for VFS which belongs to fd
        if (esp_vfs_safe_fd_isset(fd, readfds)) {
            item->isset = true;
            FD_SET(local_fd, &item->readfds);
            FD_CLR(fd, readfds);
            ESP_LOGD(TAG, "removing %d from readfds and adding as local FD %d to fd_set of VFS ID %d", fd, local_fd, vfs_index);
        }
        if (esp_vfs_safe_fd_isset(fd, writefds)) {
            item->isset = true;
            FD_SET(local_fd, &item->writefds);
            FD_CLR(fd, writefds);
            ESP_LOGD(TAG, "removing %d from writefds and adding as local FD %d to fd_set of VFS ID %d", fd, local_fd, vfs_index);
        }
        if (esp_vfs_safe_fd_isset(fd, errorfds)) {
            item->isset = true;
            FD_SET(local_fd, &item->errorfds);
            FD_CLR(fd, errorfds);
            ESP_LOGD(TAG, "removing %d from errorfds and adding as local FD %d to fd_set of VFS ID %d", fd, local_fd, vfs_index);
        }
    }

    // all non-socket VFSs have their FD sets in vfs_fds_triple
    // the global readfds, writefds and errorfds contain only socket FDs (if
    // there any)

    /* Semaphore used for waiting select events from other VFS drivers when socket
     * select is not used (not registered or socket FDs are not observed by the
     * given call of select)
     */
    SemaphoreHandle_t select_sem = NULL;

    if (!socket_select) {
        // There is no socket VFS registered or select() wasn't called for
        // any socket. Therefore, we will use our own signalization.
        if ((select_sem = xSemaphoreCreateBinary()) == NULL) {
            free(vfs_fds_triple);
            __errno_r(r) = ENOMEM;
            ESP_LOGD(TAG, "cannot create select_sem");
            return -1;
        }
    }

    for (int i = 0; i < s_vfs_count; ++i) {
        const vfs_entry_t *vfs = get_vfs_for_index(i);
        fds_triple_t *item = &vfs_fds_triple[i];

        if (vfs && vfs->vfs.start_select && item->isset) {
            // call start_select for all non-socket VFSs with has at least one FD set in readfds, writefds, or errorfds
            // note: it can point to socket VFS but item->isset will be false for that
            ESP_LOGD(TAG, "calling start_select for VFS ID %d with the following local FDs", i);
            esp_vfs_log_fd_set("readfds", &item->readfds);
            esp_vfs_log_fd_set("writefds", &item->writefds);
            esp_vfs_log_fd_set("errorfds", &item->errorfds);
            esp_err_t err = vfs->vfs.start_select(nfds, &item->readfds, &item->writefds, &item->errorfds, &select_sem);

            if (err != ESP_OK) {
                call_end_selects(i, vfs_fds_triple);
                (void) set_global_fd_sets(vfs_fds_triple, s_vfs_count, readfds, writefds, errorfds);
                if (select_sem) {
                    vSemaphoreDelete(select_sem);
                    select_sem = NULL;
                }
                free(vfs_fds_triple);
                __errno_r(r) = EINTR;
                ESP_LOGD(TAG, "start_select failed");
                return -1;
            }
        }
    }

    if (socket_select) {
        ESP_LOGD(TAG, "calling socket_select with the following FDs");
        esp_vfs_log_fd_set("readfds", readfds);
        esp_vfs_log_fd_set("writefds", writefds);
        esp_vfs_log_fd_set("errorfds", errorfds);
        ret = socket_select(nfds, readfds, writefds, errorfds, timeout);
        ESP_LOGD(TAG, "socket_select returned %d and the FDs are the following", ret);
        esp_vfs_log_fd_set("readfds", readfds);
        esp_vfs_log_fd_set("writefds", writefds);
        esp_vfs_log_fd_set("errorfds", errorfds);
    } else {
        if (readfds) {
            FD_ZERO(readfds);
        }
        if (writefds) {
            FD_ZERO(writefds);
        }
        if (errorfds) {
            FD_ZERO(errorfds);
        }

        TickType_t ticks_to_wait = portMAX_DELAY;
        if (timeout) {
            uint32_t timeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
            ticks_to_wait = timeout_ms / portTICK_PERIOD_MS;
            ESP_LOGD(TAG, "timeout is %dms", timeout_ms);
        }
        ESP_LOGD(TAG, "waiting without calling socket_select");
        xSemaphoreTake(select_sem, ticks_to_wait);
    }

    call_end_selects(s_vfs_count, vfs_fds_triple); // for VFSs for start_select was called before
    if (ret >= 0) {
        ret += set_global_fd_sets(vfs_fds_triple, s_vfs_count, readfds, writefds, errorfds);
    }
    if (select_sem) {
        vSemaphoreDelete(select_sem);
        select_sem = NULL;
    }
    free(vfs_fds_triple);

    ESP_LOGD(TAG, "esp_vfs_select returns %d", ret);
    esp_vfs_log_fd_set("readfds", readfds);
    esp_vfs_log_fd_set("writefds", writefds);
    esp_vfs_log_fd_set("errorfds", errorfds);
    return ret;
}

void esp_vfs_select_triggered(SemaphoreHandle_t *signal_sem)
{
    if (signal_sem && (*signal_sem)) {
        xSemaphoreGive(*signal_sem);
    } else {
        // Another way would be to go through s_fd_table and find the VFS
        // which has a permanent FD. But in order to avoid to lock
        // s_fd_table_lock we go through the VFS table.
        for (int i = 0; i < s_vfs_count; ++i) {
            const vfs_entry_t *vfs = s_vfs[i];
            if (vfs != NULL && vfs->vfs.stop_socket_select != NULL) {
                vfs->vfs.stop_socket_select();
                break;
            }
        }
    }
}

void esp_vfs_select_triggered_isr(SemaphoreHandle_t *signal_sem, BaseType_t *woken)
{
    if (signal_sem && (*signal_sem)) {
        xSemaphoreGiveFromISR(*signal_sem, woken);
    } else {
        // Another way would be to go through s_fd_table and find the VFS
        // which has a permanent FD. But in order to avoid to lock
        // s_fd_table_lock we go through the VFS table.
        for (int i = 0; i < s_vfs_count; ++i) {
            const vfs_entry_t *vfs = s_vfs[i];
            if (vfs != NULL && vfs->vfs.stop_socket_select_isr != NULL) {
                vfs->vfs.stop_socket_select_isr(woken);
                break;
            }
        }
    }
}

#ifdef CONFIG_SUPPORT_TERMIOS
int tcgetattr(int fd, struct termios *p)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcgetattr, local_fd, p);
    return ret;
}

int tcsetattr(int fd, int optional_actions, const struct termios *p)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcsetattr, local_fd, optional_actions, p);
    return ret;
}

int tcdrain(int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcdrain, local_fd);
    return ret;
}

int tcflush(int fd, int select)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcflush, local_fd, select);
    return ret;
}

int tcflow(int fd, int action)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcflow, local_fd, action);
    return ret;
}

pid_t tcgetsid(int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcgetsid, local_fd);
    return ret;
}

int tcsendbreak(int fd, int duration)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent* r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, tcsendbreak, local_fd, duration);
    return ret;
}
#endif // CONFIG_SUPPORT_TERMIOS
