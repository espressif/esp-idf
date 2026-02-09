/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/reent.h>
#include <sys/unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include <dirent.h>
#include "inttypes_ext.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_private/socket.h"
#include "esp_vfs_private.h"
#include "esp_vfs_utils.h"
#include "sdkconfig.h"

static const char __attribute__((unused)) *TAG = "vfs_calls";

int esp_vfs_open(struct _reent *r, const char *path, int flags, int mode)
{
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    int acc_mode = flags & O_ACCMODE;
    int ro_filesystem = vfs->flags & ESP_VFS_FLAG_READONLY_FS;
    if (acc_mode != O_RDONLY && ro_filesystem) {
        __errno_r(r) = EROFS;
        return -1;
    }

    const char *path_within_vfs = translate_path(vfs, path);
    int fd_within_vfs;
    CHECK_AND_CALL(fd_within_vfs, r, vfs, open, path_within_vfs, flags, mode);

    if (fd_within_vfs < 0) {
        __errno_r(r) = errno;
        return -1;
    }

    int fd = register_fd(vfs->offset, fd_within_vfs, false);
    if (fd >= 0) {
        return fd;
    }

    int ret;
    CHECK_AND_CALL(ret, r, vfs, close, fd_within_vfs);
    (void) ret; // remove "set but not used" warning
    __errno_r(r) = ENFILE;
    return -1;
}

ssize_t esp_vfs_write(struct _reent *r, int fd, const void *data, size_t size)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
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
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    off_t ret;
    CHECK_AND_CALL(ret, r, vfs, lseek, local_fd, size, mode);
    return ret;
}

ssize_t esp_vfs_read(struct _reent *r, int fd, void *dst, size_t size)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, read, local_fd, dst, size);
    return ret;
}

ssize_t esp_vfs_pread(int fd, void *dst, size_t size, off_t offset)
{
    struct _reent __attribute__((unused)) *r = __getreent();
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, pread, local_fd, dst, size, offset);
    return ret;
}

ssize_t esp_vfs_pwrite(int fd, const void *src, size_t size, off_t offset)
{
    struct _reent __attribute__((unused)) *r = __getreent();
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    ssize_t ret;
    CHECK_AND_CALL(ret, r, vfs, pwrite, local_fd, src, size, offset);
    return ret;
}

int esp_vfs_close(struct _reent *r, int fd)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, close, local_fd);

    unregister_fd(fd);

    return ret;
}

int esp_vfs_fstat(struct _reent *r, int fd, struct stat *st)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fstat, local_fd, st);
    return ret;
}

int esp_vfs_fcntl_r(struct _reent *r, int fd, int cmd, int arg)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fcntl, local_fd, cmd, arg);
    return ret;
}

int esp_vfs_ioctl(int fd, int cmd, ...)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }

    if (vfs->vfs->ioctl == NULL) {
        __errno_r(r) = ENOSYS;
        return -1;
    }

    int ret;
    va_list args;
    va_start(args, cmd);

    if (vfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) {
        ret = vfs->vfs->ioctl_p(vfs->ctx, local_fd, cmd, args);
    } else {
        ret = vfs->vfs->ioctl(local_fd, cmd, args);
    }

    va_end(args);
    return ret;
}

int esp_vfs_fsync(int fd)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL(ret, r, vfs, fsync, local_fd);
    return ret;
}

#ifdef CONFIG_VFS_SUPPORT_DIR

int esp_vfs_stat(struct _reent *r, const char *path, struct stat *st)
{
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char *path_within_vfs = translate_path(vfs, path);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, stat, path_within_vfs, st);
    return ret;
}

int esp_vfs_utime(const char *path, const struct utimbuf *times)
{
    int ret;
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char *path_within_vfs = translate_path(vfs, path);
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, utime, path_within_vfs, times);
    return ret;
}

int esp_vfs_link(struct _reent *r, const char *n1, const char *n2)
{
    const vfs_entry_t *vfs = get_vfs_for_path(n1);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const vfs_entry_t *vfs2 = get_vfs_for_path(n2);
    if (vfs != vfs2) {
        __errno_r(r) = EXDEV;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs2->flags);

    const char *path1_within_vfs = translate_path(vfs, n1);
    const char *path2_within_vfs = translate_path(vfs, n2);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, link, path1_within_vfs, path2_within_vfs);
    return ret;
}

int esp_vfs_unlink(struct _reent *r, const char *path)
{
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    const char *path_within_vfs = translate_path(vfs, path);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, unlink, path_within_vfs);
    return ret;
}

int esp_vfs_rename(struct _reent *r, const char *src, const char *dst)
{
    const vfs_entry_t *vfs = get_vfs_for_path(src);
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    const vfs_entry_t *vfs_dst = get_vfs_for_path(dst);
    if (vfs != vfs_dst) {
        __errno_r(r) = EXDEV;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs_dst->flags);

    const char *src_within_vfs = translate_path(vfs, src);
    const char *dst_within_vfs = translate_path(vfs, dst);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, rename, src_within_vfs, dst_within_vfs);
    return ret;
}

DIR *esp_vfs_opendir(const char *name)
{
    const vfs_entry_t *vfs = get_vfs_for_path(name);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return NULL;
    }
    const char *path_within_vfs = translate_path(vfs, name);
    DIR *ret;
    CHECK_AND_CALL_SUBCOMPONENTP(ret, r, vfs, dir, opendir, path_within_vfs);
    if (ret != NULL) {
        ret->dd_vfs_idx = vfs->offset;
    }
    return ret;
}

struct dirent *esp_vfs_readdir(DIR *pdir)
{
    const vfs_entry_t *vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
       __errno_r(r) = EBADF;
        return NULL;
    }
    struct dirent *ret;
    CHECK_AND_CALL_SUBCOMPONENTP(ret, r, vfs, dir, readdir, pdir);
    return ret;
}

int esp_vfs_readdir_r(DIR *pdir, struct dirent *entry, struct dirent* *out_dirent)
{
    const vfs_entry_t *vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, readdir_r, pdir, entry, out_dirent);
    return ret;
}

long esp_vfs_telldir(DIR *pdir)
{
    const vfs_entry_t *vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    long ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, telldir, pdir);
    return ret;
}

void esp_vfs_seekdir(DIR *pdir, long loc)
{
    const vfs_entry_t *vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return;
    }
    CHECK_AND_CALL_SUBCOMPONENTV(r, vfs, dir, seekdir, pdir, loc);
}

void esp_vfs_rewinddir(DIR *pdir)
{
    seekdir(pdir, 0);
}

int esp_vfs_closedir(DIR *pdir)
{
    const vfs_entry_t *vfs = get_vfs_for_index(pdir->dd_vfs_idx);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        errno = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, closedir, pdir);
    return ret;
}

int esp_vfs_mkdir(const char *name, mode_t mode)
{
    const vfs_entry_t *vfs = get_vfs_for_path(name);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    const char *path_within_vfs = translate_path(vfs, name);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, mkdir, path_within_vfs, mode);
    return ret;
}

int esp_vfs_rmdir(const char *name)
{
    const vfs_entry_t *vfs = get_vfs_for_path(name);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    const char *path_within_vfs = translate_path(vfs, name);
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, rmdir, path_within_vfs);
    return ret;
}

int esp_vfs_access(const char *path, int amode)
{
    int ret;
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }
    const char *path_within_vfs = translate_path(vfs, path);
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, access, path_within_vfs, amode);
    return ret;
}

int esp_vfs_truncate(const char *path, off_t length)
{
    int ret;
    const vfs_entry_t *vfs = get_vfs_for_path(path);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL) {
        __errno_r(r) = ENOENT;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    const char *path_within_vfs = translate_path(vfs, path);
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, truncate, path_within_vfs, length);
    return ret;
}

int esp_vfs_ftruncate(int fd, off_t length)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }

    CHECK_VFS_READONLY_FLAG(vfs->flags);

    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, dir, ftruncate, local_fd, length);
    return ret;
}

#endif // CONFIG_VFS_SUPPORT_DIR

#ifdef CONFIG_VFS_SUPPORT_SELECT

static void call_end_selects(int end_index, const fds_triple_t *vfs_fds_triple, void **driver_args)
{
    for (int i = 0; i < end_index; ++i) {
        const vfs_entry_t *vfs = get_vfs_for_index(i);
        const fds_triple_t *item = &vfs_fds_triple[i];
        if (vfs != NULL
            && vfs->vfs->select != NULL
            && vfs->vfs->select->end_select != NULL
            && item->isset
        ) {
            esp_err_t err = vfs->vfs->select->end_select(driver_args[i]);
            if (err != ESP_OK) {
                ESP_LOGD(TAG, "end_select failed: %s", esp_err_to_name(err));
            }
        }
    }
}

static int set_global_fd_sets(const fds_triple_t *vfs_fds_triple, int size, fd_set *readfds, fd_set *writefds, fd_set *errorfds)
{
    int ret = 0;

    for (int i = 0; i < size; ++i) {
        const fds_triple_t *item = &vfs_fds_triple[i];
        if (item->isset) {
            for (int fd = 0; fd < MAX_FDS; ++fd) {
                const fd_table_t *fd_entry = get_fd_entry(fd);
                if (fd_entry->vfs_index == i) {
                    const int local_fd = fd_entry->local_fd; // single read -> no locking is required
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
    // NOTE: Please see the "Synchronous input/output multiplexing" section of the ESP-IDF Programming Guide
    // (API Reference -> Storage -> Virtual Filesystem) for a general overview of the implementation of VFS select().
    int ret = 0;
    struct _reent __attribute__((unused)) *r = __getreent();

    ESP_LOGD(TAG, "esp_vfs_select starts with nfds = %d", nfds);
    if (timeout) {
        ESP_LOGD(TAG, "timeout is %lds + %ldus", (long)timeout->tv_sec, timeout->tv_usec);
    }
    esp_vfs_log_fd_set("readfds", readfds);
    esp_vfs_log_fd_set("writefds", writefds);
    esp_vfs_log_fd_set("errorfds", errorfds);

    if (nfds > MAX_FDS || nfds < 0) {
        ESP_LOGD(TAG, "incorrect nfds");
        __errno_r(r) = EINVAL;
        return -1;
    }

    // Capture s_vfs_count to a local variable in case a new driver is registered or removed during this actual select()
    // call. s_vfs_count cannot be protected with a mutex during a select() call (which can be one without a timeout)
    // because that could block the registration of new driver.
    const size_t vfs_count = get_vfs_count();
    fds_triple_t *vfs_fds_triple;
    if ((vfs_fds_triple = heap_caps_calloc(vfs_count, sizeof(fds_triple_t), VFS_MALLOC_FLAGS)) == NULL) {
        __errno_r(r) = ENOMEM;
        ESP_LOGD(TAG, "calloc is unsuccessful");
        return -1;
    }

    esp_vfs_select_sem_t sel_sem = {
        .is_sem_local = false,
        .sem = NULL,
    };

    int (*socket_select)(int, fd_set *, fd_set *, fd_set *, struct timeval *) = NULL;
    for (int fd = 0; fd < nfds; ++fd) {
        const fd_table_t fd_entry = start_select(fd, errorfds);
        const bool is_socket_fd = fd_entry.permanent;
        const int vfs_index = fd_entry.vfs_index;
        const int local_fd = fd_entry.local_fd;

        if (vfs_index < 0) {
            continue;
        }

        if (is_socket_fd) {
            if (!socket_select) {
                // no socket_select found yet so take a look
                if (esp_vfs_safe_fd_isset(fd, readfds) ||
                        esp_vfs_safe_fd_isset(fd, writefds) ||
                        esp_vfs_safe_fd_isset(fd, errorfds)) {
                    const vfs_entry_t *vfs = get_vfs_for_index(vfs_index);
                    socket_select = vfs->vfs->select->socket_select;
                    sel_sem.sem = vfs->vfs->select->get_socket_select_semaphore();
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

    if (!socket_select) {
        // There is no socket VFS registered or select() wasn't called for
        // any socket. Therefore, we will use our own signalization.
        sel_sem.is_sem_local = true;
        if ((sel_sem.sem = xSemaphoreCreateBinary()) == NULL) {
            free(vfs_fds_triple);
            __errno_r(r) = ENOMEM;
            ESP_LOGD(TAG, "cannot create select semaphore");
            return -1;
        }
    }

    void **driver_args = heap_caps_calloc(vfs_count, sizeof(void *), VFS_MALLOC_FLAGS);

    if (driver_args == NULL) {
        free(vfs_fds_triple);
        __errno_r(r) = ENOMEM;
        ESP_LOGD(TAG, "calloc is unsuccessful for driver args");
        return -1;
    }

    for (size_t i = 0; i < vfs_count; ++i) {
        const vfs_entry_t *vfs = get_vfs_for_index(i);
        fds_triple_t *item = &vfs_fds_triple[i];

        if (vfs == NULL || vfs->vfs->select == NULL || vfs->vfs->select->start_select == NULL) {
            ESP_LOGD(TAG, "start_select function callback for this vfs (s_vfs[%d]) is not defined", vfs->offset);
            continue;
        }

        if (!item->isset) {
            continue;
        }

        // call start_select for all non-socket VFSs with has at least one FD set in readfds, writefds, or errorfds
        // note: it can point to socket VFS but item->isset will be false for that
        ESP_LOGD(TAG, "calling start_select for VFS ID " PRIuSIZE " with the following local FDs", i);
        esp_vfs_log_fd_set("readfds", &item->readfds);
        esp_vfs_log_fd_set("writefds", &item->writefds);
        esp_vfs_log_fd_set("errorfds", &item->errorfds);
        esp_err_t err = vfs->vfs->select->start_select(nfds, &item->readfds, &item->writefds, &item->errorfds, sel_sem,
                driver_args + i);

        if (err != ESP_OK) {
            if (err != ESP_ERR_NOT_SUPPORTED) {
                call_end_selects(i, vfs_fds_triple, driver_args);
            }
            (void) set_global_fd_sets(vfs_fds_triple, vfs_count, readfds, writefds, errorfds);
            if (sel_sem.is_sem_local && sel_sem.sem) {
                vSemaphoreDelete(sel_sem.sem);
                sel_sem.sem = NULL;
            }
            free(vfs_fds_triple);
            free(driver_args);
            __errno_r(r) = EINTR;
            ESP_LOGD(TAG, "start_select failed: %s", esp_err_to_name(err));
            return -1;
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
            uint32_t timeout_ms = (timeout->tv_sec *1000) + (timeout->tv_usec / 1000);
            /* Round up the number of ticks.
             * Not only we need to round up the number of ticks, but we also need to add 1.
             * Indeed, `select` function shall wait for AT LEAST timeout, but on FreeRTOS,
             * if we specify a timeout of 1 tick to `xSemaphoreTake`, it will take AT MOST
             * 1 tick before triggering a timeout. Thus, we need to pass 2 ticks as a timeout
             * to `xSemaphoreTake`. */
            ticks_to_wait = ((timeout_ms + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS) + 1;
            ESP_LOGD(TAG, "timeout is %" PRIu32 "ms", timeout_ms);
        }
        ESP_LOGD(TAG, "waiting without calling socket_select");
        xSemaphoreTake(sel_sem.sem, ticks_to_wait);
    }

    call_end_selects(vfs_count, vfs_fds_triple, driver_args); // for VFSs for start_select was called before

    if (ret >= 0) {
        ret += set_global_fd_sets(vfs_fds_triple, vfs_count, readfds, writefds, errorfds);
    }
    if (sel_sem.sem) { // Cleanup the select semaphore
        if (sel_sem.is_sem_local) {
            vSemaphoreDelete(sel_sem.sem);
        } else if (socket_select) {
            SemaphoreHandle_t *s = sel_sem.sem;
            /* Select might have been triggered from both lwip and vfs fds at the same time, and
             * we have to make sure that the lwip semaphore is cleared when we exit select().
             * It is safe, as the semaphore belongs to the calling thread. */
            xSemaphoreTake(*s, 0);
        }
        sel_sem.sem = NULL;
    }
    close_pending(nfds);
    free(vfs_fds_triple);
    free(driver_args);

    ESP_LOGD(TAG, "esp_vfs_select returns %d", ret);
    esp_vfs_log_fd_set("readfds", readfds);
    esp_vfs_log_fd_set("writefds", writefds);
    esp_vfs_log_fd_set("errorfds", errorfds);
    return ret;
}

void esp_vfs_select_triggered(esp_vfs_select_sem_t sem)
{
    if (sem.is_sem_local) {
        xSemaphoreGive(sem.sem);
    } else {
        // Another way would be to go through s_fd_table and find the VFS
        // which has a permanent FD. But in order to avoid to lock
        // s_fd_table_lock we go through the VFS table.
        size_t vfs_count = get_vfs_count();
        for (int i = 0; i < vfs_count; ++i) {
            // Note: vfs_count could have changed since the start of vfs_select() call. However, that change doesn't
            // matter here stop_socket_select() will be called for only valid VFS drivers.
            const vfs_entry_t *vfs = get_vfs_for_index(i);
            if (vfs != NULL
                && vfs->vfs->select != NULL
                && vfs->vfs->select->stop_socket_select != NULL
            ) {
                vfs->vfs->select->stop_socket_select(sem.sem);
                break;
            }
        }
    }
}

void esp_vfs_select_triggered_isr(esp_vfs_select_sem_t sem, BaseType_t *woken)
{
    if (sem.is_sem_local) {
        xSemaphoreGiveFromISR(sem.sem, woken);
    } else {
        // Another way would be to go through s_fd_table and find the VFS
        // which has a permanent FD. But in order to avoid to lock
        // s_fd_table_lock we go through the VFS table.
        size_t vfs_count = get_vfs_count();
        for (int i = 0; i < vfs_count; ++i) {
            // Note: s_vfs_count could have changed since the start of vfs_select() call. However, that change doesn't
            // matter here stop_socket_select() will be called for only valid VFS drivers.
            const vfs_entry_t *vfs = get_vfs_for_index(i);
            if (vfs != NULL
                && vfs->vfs->select != NULL
                && vfs->vfs->select->stop_socket_select_isr != NULL
            ) {
                // Note: If the UART ISR resides in IRAM, the function referenced by stop_socket_select_isr should also be placed in IRAM.
                vfs->vfs->select->stop_socket_select_isr(sem.sem, woken);
                break;
            }
        }
    }
}

#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_TERMIOS

int tcgetattr(int fd, struct termios *p)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcgetattr, local_fd, p);
    return ret;
}

int tcsetattr(int fd, int optional_actions, const struct termios *p)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcsetattr, local_fd, optional_actions, p);
    return ret;
}

int tcdrain(int fd)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcdrain, local_fd);
    return ret;
}

int tcflush(int fd, int select)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcflush, local_fd, select);
    return ret;
}

int tcflow(int fd, int action)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcflow, local_fd, action);
    return ret;
}

pid_t tcgetsid(int fd)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcgetsid, local_fd);
    return ret;
}

int tcsendbreak(int fd, int duration)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    const int local_fd = get_local_fd(vfs, fd);
    struct _reent __attribute__((unused)) *r = __getreent();
    if (vfs == NULL || local_fd < 0) {
        __errno_r(r) = EBADF;
        return -1;
    }
    int ret;
    CHECK_AND_CALL_SUBCOMPONENT(ret, r, vfs, termios, tcsendbreak, local_fd, duration);
    return ret;
}
#endif // CONFIG_VFS_SUPPORT_TERMIOS

#ifndef CONFIG_IDF_TARGET_LINUX

/* Create aliases for libc syscalls

   These functions are also available in ROM as stubs which use the syscall table, but linking them
   directly here saves an additional function call when a software function is linked to one, and
   makes linking with -stdlib easier.
 */
#ifdef CONFIG_VFS_SUPPORT_IO
int _open_r(struct _reent *r, const char *path, int flags, int mode)
    __attribute__((alias("esp_vfs_open")));
int _close_r(struct _reent *r, int fd)
    __attribute__((alias("esp_vfs_close")));
ssize_t _read_r(struct _reent *r, int fd, void *dst, size_t size)
    __attribute__((alias("esp_vfs_read")));
ssize_t _write_r(struct _reent *r, int fd, const void *data, size_t size)
    __attribute__((alias("esp_vfs_write")));
ssize_t pread(int fd, void *dst, size_t size, off_t offset)
    __attribute__((alias("esp_vfs_pread")));
ssize_t pwrite(int fd, const void *src, size_t size, off_t offset)
    __attribute__((alias("esp_vfs_pwrite")));
off_t _lseek_r(struct _reent *r, int fd, off_t size, int mode)
    __attribute__((alias("esp_vfs_lseek")));
int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
    __attribute__((alias("esp_vfs_fcntl_r")));
int _fstat_r(struct _reent *r, int fd, struct stat *st)
    __attribute__((alias("esp_vfs_fstat")));
int fsync(int fd)
    __attribute__((alias("esp_vfs_fsync")));
int ioctl(int fd, int cmd, ...)
    __attribute__((alias("esp_vfs_ioctl")));
#endif // CONFIG_VFS_SUPPORT_IO

#ifdef CONFIG_VFS_SUPPORT_SELECT
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
    __attribute__((alias("esp_vfs_select")));
#endif // CONFIG_VFS_SUPPORT_SELECT

#ifdef CONFIG_VFS_SUPPORT_DIR
int _stat_r(struct _reent *r, const char *path, struct stat *st)
    __attribute__((alias("esp_vfs_stat")));
int _link_r(struct _reent *r, const char *n1, const char *n2)
    __attribute__((alias("esp_vfs_link")));
int _unlink_r(struct _reent *r, const char *path)
    __attribute__((alias("esp_vfs_unlink")));
int _rename_r(struct _reent *r, const char *src, const char *dst)
    __attribute__((alias("esp_vfs_rename")));
int truncate(const char *path, off_t length)
    __attribute__((alias("esp_vfs_truncate")));
int ftruncate(int fd, off_t length)
    __attribute__((alias("esp_vfs_ftruncate")));
int access(const char *path, int amode)
    __attribute__((alias("esp_vfs_access")));
int utime(const char *path, const struct utimbuf *times)
    __attribute__((alias("esp_vfs_utime")));
int rmdir(const char *name)
    __attribute__((alias("esp_vfs_rmdir")));
int mkdir(const char *name, mode_t mode)
    __attribute__((alias("esp_vfs_mkdir")));
DIR *opendir(const char *name)
    __attribute__((alias("esp_vfs_opendir")));
int closedir(DIR *pdir)
    __attribute__((alias("esp_vfs_closedir")));
int readdir_r(DIR *pdir, struct dirent *entry, struct dirent* *out_dirent)
    __attribute__((alias("esp_vfs_readdir_r")));
struct dirent *readdir(DIR *pdir)
    __attribute__((alias("esp_vfs_readdir")));
long telldir(DIR *pdir)
    __attribute__((alias("esp_vfs_telldir")));
void seekdir(DIR *pdir, long loc)
    __attribute__((alias("esp_vfs_seekdir")));
void rewinddir(DIR *pdir)
    __attribute__((alias("esp_vfs_rewinddir")));
#endif // CONFIG_VFS_SUPPORT_DIR

#endif //CONFIG_IDF_TARGET_LINUX

void vfs_include_syscalls_impl(void)
{
    // Linker hook function, exists to make the linker examine this file
}
