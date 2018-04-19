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
#include <sys/param.h>
#include <dirent.h>
#include "esp_vfs.h"
#include "esp_log.h"
#include "port/arch/sys_arch.h"
#include "lwip/sys.h"

#define VFS_MAX_COUNT   8   /* max number of VFS entries (registered filesystems) */
#define MIN_LWIP_FD     (FD_SETSIZE - CONFIG_LWIP_MAX_SOCKETS)

#define LEN_PATH_PREFIX_IGNORED SIZE_MAX /* special length value for VFS which is never recognised by open() */

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

static short fd_vfs[FD_SETSIZE] = { [0 ... FD_SETSIZE-1] = -1 };

static esp_err_t esp_vfs_register_common(const char* base_path, size_t len, const esp_vfs_t* vfs, void* ctx, int *vfs_entry_index)
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

    if (vfs_entry_index) {
        *vfs_entry_index = index;
    }

    return ESP_OK;
}

esp_err_t esp_vfs_register(const char* base_path, const esp_vfs_t* vfs, void* ctx)
{
    return esp_vfs_register_common(base_path, strlen(base_path), vfs, ctx, NULL);
}

esp_err_t esp_vfs_register_socket_space(const esp_vfs_t *vfs, void *ctx, int *p_min_fd, int *p_max_fd)
{
    int index = -1;
    esp_err_t ret = esp_vfs_register_common("", LEN_PATH_PREFIX_IGNORED, vfs, ctx, &index);

    if (ret == ESP_OK) {
        if (p_min_fd != NULL) {
            *p_min_fd = MIN_LWIP_FD;
        }

        if (p_max_fd != NULL) {
            *p_max_fd = FD_SETSIZE;
        }
    }

    for (int i = MIN_LWIP_FD; i < FD_SETSIZE; ++i) {
        fd_vfs[i] = index;
    }

    return ret;
}

esp_err_t esp_vfs_unregister(const char* base_path)
{
    for (size_t i = 0; i < s_vfs_count; ++i) {
        vfs_entry_t* vfs = s_vfs[i];
        if (vfs == NULL) {
            continue;
        }
        if (memcmp(base_path, vfs->path_prefix, vfs->path_prefix_len) == 0) {
            free(vfs);
            s_vfs[i] = NULL;

            // Delete all references from the FD lookup-table
            for (int j = 0; j < FD_SETSIZE; ++j) {
                if (fd_vfs[j] == i) {
                    fd_vfs[j] = -1;
                }
            }

            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}

static inline const vfs_entry_t* get_vfs_for_index(int index)
{
    if (index < 0 || index >= s_vfs_count) {
        return NULL;
    } else {
        return s_vfs[index];
    }
}

static inline bool fd_valid(int fd)
{
    return (fd < FD_SETSIZE) && (fd >= 0);
}

static inline const vfs_entry_t* get_vfs_for_fd(int fd)
{
    if (fd_valid(fd)) {
        return get_vfs_for_index(fd_vfs[fd]);
    } else {
        return NULL;
    }
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
    const vfs_entry_t* vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char* path_within_vfs = translate_path(vfs, path);
    int ret;
    CHECK_AND_CALL(ret, r, vfs, open, path_within_vfs, flags, mode);
    if (ret >= MIN_LWIP_FD) {
        CHECK_AND_CALL(ret, r, vfs, close, ret);
        __errno_r(r) = ENFILE;
        return -1;
    }
    if (ret >= 0) {
        fd_vfs[ret] = vfs->offset;
    }
    return ret;
}

ssize_t esp_vfs_write(struct _reent *r, int fd, const void * data, size_t size)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, write, fd, data, size);
    return ret;
}

off_t esp_vfs_lseek(struct _reent *r, int fd, off_t size, int mode)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    off_t ret;
    CHECK_AND_CALL(ret, r, vfs, lseek, fd, size, mode);
    return ret;
}

ssize_t esp_vfs_read(struct _reent *r, int fd, void * dst, size_t size)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, read, fd, dst, size);
    return ret;
}


int esp_vfs_close(struct _reent *r, int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, close, fd);
    if (fd < MIN_LWIP_FD) {
        fd_vfs[fd] = -1;
    } // else { Leave alone socket FDs because sockets are not created by
    // esp_vfs_open and therefore fd_vfs won't be set again.
    //}
    return ret;
}

int esp_vfs_fstat(struct _reent *r, int fd, struct stat * st)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fstat, fd, st);
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
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    va_list args;
    va_start(args, cmd);
    CHECK_AND_CALL(ret, r, vfs, fcntl, fd, cmd, args);
    va_end(args);
    return ret;
}

int ioctl(int fd, int cmd, ...)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    va_list args;
    va_start(args, cmd);
    CHECK_AND_CALL(ret, r, vfs, ioctl, fd, cmd, args);
    va_end(args);
    return ret;
}

int fsync(int fd)
{
    const vfs_entry_t* vfs = get_vfs_for_fd(fd);
    struct _reent* r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fsync, fd);
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

static int set_global_fd_sets(const fds_triple_t *vfs_fds_triple, int size, fd_set *readfds, fd_set *writefds, fd_set *errorfds)
{
    int ret = 0;

    for (int i = 0; i < size; ++i) {
        const fds_triple_t *item = &vfs_fds_triple[i];
        if (item->isset) {
            for (int j  = 0; j < sizeof(item->readfds.fds_bits)/sizeof(item->readfds.fds_bits[0]); ++j) {
                if (readfds != NULL) {
                    readfds->fds_bits[j] |= item->readfds.fds_bits[j];
                    ret += __builtin_popcountl(item->readfds.fds_bits[j]);
                }
                if (writefds != NULL) {
                    writefds->fds_bits[j] |= item->writefds.fds_bits[j];
                    ret += __builtin_popcountl(item->writefds.fds_bits[j]);
                }
                if (errorfds != NULL) {
                    errorfds->fds_bits[j] |= item->errorfds.fds_bits[j];
                    ret += __builtin_popcountl(item->errorfds.fds_bits[j]);
                }
            }
        }
    }

    return ret;
}

int esp_vfs_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    const int non_socket_nfds = MIN(nfds, MIN_LWIP_FD);
    int ret = 0;
    struct _reent* r = __getreent();

    if (nfds > FD_SETSIZE || nfds < 0) {
        __errno_r(r) = EINVAL;
        return -1;
    }

    fds_triple_t *vfs_fds_triple;
    if ((vfs_fds_triple = calloc(s_vfs_count, sizeof(fds_triple_t))) == NULL) {
        __errno_r(r) = ENOMEM;
        return -1;
    }

    for (int i = 0; i < non_socket_nfds; ++i) {
        if (fd_vfs[i] < 0) {
            continue;
        }
        fds_triple_t *item = &vfs_fds_triple[fd_vfs[i]]; // FD sets for VFS which belongs to FD i
        if (readfds && FD_ISSET(i, readfds)) {
            item->isset = true;
            FD_SET(i, &item->readfds);
            FD_CLR(i, readfds);
        }
        if (writefds && FD_ISSET(i, writefds)) {
            item->isset = true;
            FD_SET(i, &item->writefds);
            FD_CLR(i, writefds);
        }
        if (errorfds && FD_ISSET(i, errorfds)) {
            item->isset = true;
            FD_SET(i, &item->errorfds);
            FD_CLR(i, errorfds);
        }
    }

    // all non-socket VFSs have their FD sets in vfs_fds_triple
    // the global readfds, writefds and errorfds contain only socket FDs

    void *callback_handle = sys_thread_sem_get();
    if (!callback_handle) {
        __errno_r(r) = ENOMEM;
        return -1;
    }
    sys_arch_sem_wait(callback_handle, 1); //ensure the semaphore is taken so we are not signalled out-of-sequence; 0 timeout would block forever
    for (int i = 0; i < s_vfs_count; ++i) {
        const vfs_entry_t *vfs = get_vfs_for_index(i);
        fds_triple_t *item = &vfs_fds_triple[i];

        if (vfs && vfs->vfs.start_select && item->isset) {
            // call start_select for all non-socket VFSs with has at least one FD set in readfds, writefds, or errorfds
            // note: it can point to socket VFS but item->isset will be false for that
            esp_err_t err = vfs->vfs.start_select(nfds, &item->readfds, &item->writefds, &item->errorfds, callback_handle);

            if (err != ESP_OK) {
                call_end_selects(i, vfs_fds_triple);
                (void) set_global_fd_sets(vfs_fds_triple, s_vfs_count, readfds, writefds, errorfds);
                free(vfs_fds_triple);
                __errno_r(r) = ENOMEM;
                return -1;
            }
        }
    }

    int socket_select_called = 0;
    for (int i = MIN_LWIP_FD; i < FD_SETSIZE; ++i) {
        const vfs_entry_t* vfs = get_vfs_for_fd(i);
        if (vfs != NULL && vfs->vfs.socket_select != NULL) {
            // find socket VFS and call socket_select
            ret = vfs->vfs.socket_select(nfds, readfds, writefds, errorfds, timeout);
            socket_select_called = 1;
            break;
        }
    }
    if (!socket_select_called) {
        uint32_t timeout_ms = 0;

        if (readfds) {
            FD_ZERO(readfds);
        }
        if (writefds) {
            FD_ZERO(writefds);
        }
        if (errorfds) {
            FD_ZERO(errorfds);
        }

        if (timeout) {
            timeout_ms = timeout->tv_sec * 1000 + timeout->tv_usec / 1000;
            timeout_ms = MAX(timeout_ms, 1); //make sure it is not 0 which would be in infinite wait
        }
        sys_arch_sem_wait(callback_handle, timeout_ms);
        sys_sem_free(callback_handle);
    }

    call_end_selects(s_vfs_count, vfs_fds_triple); // for VFSs for start_select was called before
    if (ret >= 0) {
        ret += set_global_fd_sets(vfs_fds_triple, s_vfs_count, readfds, writefds, errorfds);
    }
    //don't free callback_handle because it is freed in lwip_select (vfs.socket_select)
    free(vfs_fds_triple);
    return ret;
}

void esp_vfs_select_triggered(void *callback_handle)
{
    if (callback_handle) {
        sys_sem_signal(callback_handle); //vfs->vfs.socket_select will return
    }
}

void esp_vfs_select_triggered_isr(void *callback_handle, BaseType_t *woken)
{
    if (callback_handle && sys_sem_signal_isr(callback_handle) && woken) {
        *woken = pdTRUE;
    }
}
