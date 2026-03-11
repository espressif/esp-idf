/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief Linux target VFS syscall wrappers (strong POSIX symbols)
 *
 * This file provides strong POSIX symbol definitions that dispatch through
 * esp_vfs_* for registered FDs, and fall back to the cooperative syscall
 * layer for unregistered FDs (e.g. during early boot before VFS init).
 *
 * All kernel FDs created by open/pipe/socket/dup/accept are registered
 * with VFS via esp_vfs_register_fd_with_local_fd, so the application
 * only sees VFS-allocated FD numbers.  This eliminates FD collisions
 * between kernel FDs and VFS-internal FD allocations.
 *
 * Init and ops registration live in vfs_linux_default_coop.c.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <poll.h>
#include "esp_vfs.h"
#include "esp_vfs_private.h"
#include "esp_private/freertos_linux_coop_syscalls.h"

/* Defined in vfs_linux_default_coop.c — set during init_vfs_linux_coop() */
extern esp_vfs_id_t s_linux_host_vfs_id;

/**
 * Helper: register a kernel FD with VFS after a successful FD-creating syscall.
 * Returns the VFS FD on success, or the raw kernel FD if VFS is not yet initialized.
 */
static int register_kernel_fd(int kernel_fd)
{
    if (kernel_fd < 0) {
        return kernel_fd;
    }
    if (s_linux_host_vfs_id < 0) {
        return kernel_fd;  /* VFS not initialized yet (early boot) */
    }
    int vfs_fd;
    esp_err_t err = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, kernel_fd, false, &vfs_fd);
    if (err != ESP_OK) {
        freertos_linux_coop_close(kernel_fd);
        errno = ENFILE;
        return -1;
    }
    return vfs_fd;
}

/**
 * Helper: translate a VFS FD to a kernel FD.
 * Returns the kernel (local) FD, or -1 with errno=EBADF.
 */
static int vfs_fd_to_kernel_fd(int fd)
{
    const vfs_entry_t *vfs = get_vfs_for_fd(fd);
    if (vfs == NULL) {
        return fd;  /* Not registered — use directly (early boot) */
    }
    int local_fd = get_local_fd(vfs, fd);
    if (local_fd < 0) {
        errno = EBADF;
        return -1;
    }
    return local_fd;
}

#ifdef CONFIG_VFS_SUPPORT_DIR
int stat(const char * path, struct stat * st)
{
    struct _reent r;
    return esp_vfs_stat(&r, path, st);
}

int link(const char* n1, const char* n2)
{
    struct _reent r;
    return esp_vfs_link(&r, n1, n2);
}

int unlink(const char *path)
{
    struct _reent r;
    return esp_vfs_unlink(&r, path);
}

int rename(const char *src, const char *dst)
{
    struct _reent r;
    return esp_vfs_rename(&r, src, dst);
}

int truncate(const char *path, off_t length)
{
    return esp_vfs_truncate(path, length);
}

int ftruncate(int fd, off_t length)
{
    return esp_vfs_ftruncate(fd, length);
}

int access(const char *path, int amode)
{
    return esp_vfs_access(path, amode);
}

int utime(const char *path, const struct utimbuf *times)
{
    return esp_vfs_utime(path, times);
}

int rmdir(const char* name)
{
    return esp_vfs_rmdir(name);
}

int mkdir(const char* name, mode_t mode)
{
    return esp_vfs_mkdir(name, mode);
}

DIR* opendir(const char* name)
{
    return esp_vfs_opendir(name);
}

int closedir(DIR* pdir)
{
    return esp_vfs_closedir(pdir);
}

int readdir_r(DIR* pdir, struct dirent* entry, struct dirent** out_dirent)
{
    return esp_vfs_readdir_r(pdir, entry, out_dirent);
}

struct dirent* readdir(DIR* pdir)
{
    return esp_vfs_readdir(pdir);
}

long telldir(DIR* pdir)
{
    return esp_vfs_telldir(pdir);
}

void seekdir(DIR* pdir, long loc)
{
    return esp_vfs_seekdir(pdir, loc);
}

void rewinddir(DIR* pdir)
{
    return esp_vfs_rewinddir(pdir);
}

#endif // CONFIG_VFS_SUPPORT_DIR


ssize_t read(int fd, void *buf, size_t count)
{
    struct _reent r;
    return esp_vfs_read(&r, fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    struct _reent r;
    return esp_vfs_write(&r, fd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    return esp_vfs_pread(fd, buf, count, offset);
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    return esp_vfs_pwrite(fd, buf, count, offset);
}

int open(const char *path, int flags, ...)
{
    int mode = 0;
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    struct _reent r;
    return esp_vfs_open(&r, path, flags, mode);
}

int close(int fd)
{
    struct _reent r;
    return esp_vfs_close(&r, fd);
}

int fcntl(int fd, int cmd, ...)
{
    int arg = 0;
    if (cmd != F_GETFL) {
        va_list list;
        va_start(list, cmd);
        arg = va_arg(list, int);
        va_end(list);
    }
    struct _reent r;
    return esp_vfs_fcntl_r(&r, fd, cmd, arg);
}

off_t lseek(int fd, off_t size, int mode)
{
    struct _reent r;
    return esp_vfs_lseek(&r, fd, size, mode);
}

int fstat(int fd, struct stat *st)
{
    struct _reent r;
    return esp_vfs_fstat(&r, fd, st);
}

int ioctl(int fd, signed long cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int ret = esp_vfs_ioctl(fd, cmd, args);
    va_end(args);
    return ret;
}

int fsync(int fd)
{
    return esp_vfs_fsync(fd);
}

int pipe(int fds[2])
{
    int ret = freertos_linux_coop_pipe(fds);
    if (ret != 0) {
        return ret;
    }
    if (s_linux_host_vfs_id >= 0) {
        int vfs_fd0, vfs_fd1;
        esp_err_t e0 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, fds[0], false, &vfs_fd0);
        esp_err_t e1 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, fds[1], false, &vfs_fd1);
        if (e0 != ESP_OK || e1 != ESP_OK) {
            freertos_linux_coop_close(fds[0]);
            freertos_linux_coop_close(fds[1]);
            errno = ENFILE;
            return -1;
        }
        fds[0] = vfs_fd0;
        fds[1] = vfs_fd1;
    }
    return 0;
}

int pipe2(int fds[2], int flags)
{
    int ret = freertos_linux_coop_pipe2(fds, flags);
    if (ret != 0) {
        return ret;
    }
    if (s_linux_host_vfs_id >= 0) {
        int vfs_fd0, vfs_fd1;
        esp_err_t e0 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, fds[0], false, &vfs_fd0);
        esp_err_t e1 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, fds[1], false, &vfs_fd1);
        if (e0 != ESP_OK || e1 != ESP_OK) {
            freertos_linux_coop_close(fds[0]);
            freertos_linux_coop_close(fds[1]);
            errno = ENFILE;
            return -1;
        }
        fds[0] = vfs_fd0;
        fds[1] = vfs_fd1;
    }
    return 0;
}

int socket(int domain, int type, int protocol)
{
    int kernel_fd = freertos_linux_coop_socket(domain, type, protocol);
    return register_kernel_fd(kernel_fd);
}

int socketpair(int domain, int type, int protocol, int sv[2])
{
    int ret = freertos_linux_coop_socketpair(domain, type, protocol, sv);
    if (ret != 0) {
        return ret;
    }
    if (s_linux_host_vfs_id >= 0) {
        int vfs_fd0, vfs_fd1;
        esp_err_t e0 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, sv[0], false, &vfs_fd0);
        esp_err_t e1 = esp_vfs_register_fd_with_local_fd(s_linux_host_vfs_id, sv[1], false, &vfs_fd1);
        if (e0 != ESP_OK || e1 != ESP_OK) {
            freertos_linux_coop_close(sv[0]);
            freertos_linux_coop_close(sv[1]);
            errno = ENFILE;
            return -1;
        }
        sv[0] = vfs_fd0;
        sv[1] = vfs_fd1;
    }
    return 0;
}

int dup(int oldfd)
{
    int kernel_old = vfs_fd_to_kernel_fd(oldfd);
    if (kernel_old < 0) {
        return -1;
    }
    int kernel_fd = freertos_linux_coop_dup(kernel_old);
    return register_kernel_fd(kernel_fd);
}

int dup2(int oldfd, int newfd)
{
    int kernel_old = vfs_fd_to_kernel_fd(oldfd);
    if (kernel_old < 0) {
        return -1;
    }
    /* If newfd is a registered VFS FD, we need its kernel FD and must unregister it */
    int kernel_new = newfd;
    if (get_vfs_for_fd(newfd) != NULL) {
        kernel_new = vfs_fd_to_kernel_fd(newfd);
        if (kernel_new < 0) {
            return -1;
        }
        unregister_fd(newfd);
    }
    int kernel_fd = freertos_linux_coop_dup2(kernel_old, kernel_new);
    return register_kernel_fd(kernel_fd);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int kernel_sock = vfs_fd_to_kernel_fd(sockfd);
    if (kernel_sock < 0) {
        return -1;
    }
    int kernel_fd = freertos_linux_coop_accept(kernel_sock, addr, addrlen);
    return register_kernel_fd(kernel_fd);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    int kfd = vfs_fd_to_kernel_fd(fd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_readv(kfd, iov, iovcnt);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    int kfd = vfs_fd_to_kernel_fd(fd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_writev(kfd, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_recv(kfd, buf, len, flags);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_send(kfd, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_recvfrom(kfd, buf, len, flags, src_addr, addrlen);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_sendto(kfd, buf, len, flags, dest_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_recvmsg(kfd, msg, flags);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_sendmsg(kfd, msg, flags);
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int kfd = vfs_fd_to_kernel_fd(sockfd);
    if (kfd < 0) {
        return -1;
    }
    return freertos_linux_coop_connect(kfd, addr, addrlen);
}

/**
 * Translate VFS fd_sets to kernel fd_sets.
 * Returns the nfds value for the kernel sets, or -1 on error (errno set).
 */
static int fdsets_vfs_to_kernel(int nfds,
                                const fd_set *readfds, const fd_set *writefds, const fd_set *exceptfds,
                                fd_set *k_readfds, fd_set *k_writefds, fd_set *k_exceptfds)
{
    int max_kfd = 0;
    FD_ZERO(k_readfds);
    FD_ZERO(k_writefds);
    FD_ZERO(k_exceptfds);

    for (int fd = 0; fd < nfds; ++fd) {
        int kfd = -1;
        bool active = false;
        if (readfds && FD_ISSET(fd, readfds)) {
            kfd = vfs_fd_to_kernel_fd(fd);
            if (kfd < 0) { return -1; }
            FD_SET(kfd, k_readfds);
            active = true;
        }
        if (writefds && FD_ISSET(fd, writefds)) {
            if (kfd < 0) { kfd = vfs_fd_to_kernel_fd(fd); }
            if (kfd < 0) { return -1; }
            FD_SET(kfd, k_writefds);
            active = true;
        }
        if (exceptfds && FD_ISSET(fd, exceptfds)) {
            if (kfd < 0) { kfd = vfs_fd_to_kernel_fd(fd); }
            if (kfd < 0) { return -1; }
            FD_SET(kfd, k_exceptfds);
            active = true;
        }
        if (active && kfd >= max_kfd) {
            max_kfd = kfd + 1;
        }
    }
    return max_kfd;
}

/**
 * Translate kernel fd_set results back to VFS fd_sets.
 */
static void fdsets_kernel_to_vfs(int nfds,
                                 fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                 fd_set *k_readfds, fd_set *k_writefds, fd_set *k_exceptfds)
{
    if (readfds)   { FD_ZERO(readfds); }
    if (writefds)  { FD_ZERO(writefds); }
    if (exceptfds) { FD_ZERO(exceptfds); }
    for (int fd = 0; fd < nfds; ++fd) {
        int kfd = vfs_fd_to_kernel_fd(fd);
        if (kfd < 0) { continue; }
        if (readfds   && FD_ISSET(kfd, k_readfds))   { FD_SET(fd, readfds); }
        if (writefds  && FD_ISSET(kfd, k_writefds))  { FD_SET(fd, writefds); }
        if (exceptfds && FD_ISSET(kfd, k_exceptfds)) { FD_SET(fd, exceptfds); }
    }
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout)
{
#ifdef CONFIG_VFS_SUPPORT_SELECT
    for (int fd = 0; fd < nfds; ++fd) {
        if ((readfds   && FD_ISSET(fd, readfds))  ||
            (writefds  && FD_ISSET(fd, writefds)) ||
            (exceptfds && FD_ISSET(fd, exceptfds))) {
            const vfs_entry_t *vfs = get_vfs_for_fd(fd);
            if (vfs != NULL && vfs->offset != s_linux_host_vfs_id) {
                return esp_vfs_select(nfds, readfds, writefds, exceptfds, timeout);
            }
        }
    }
#endif
    fd_set k_readfds, k_writefds, k_exceptfds;
    int max_kfd = fdsets_vfs_to_kernel(nfds, readfds, writefds, exceptfds,
                                       &k_readfds, &k_writefds, &k_exceptfds);
    if (max_kfd < 0) { return -1; }

    int ret = freertos_linux_coop_select(max_kfd,
                                         readfds   ? &k_readfds   : NULL,
                                         writefds  ? &k_writefds  : NULL,
                                         exceptfds ? &k_exceptfds : NULL,
                                         timeout);
    if (ret >= 0) {
        fdsets_kernel_to_vfs(nfds, readfds, writefds, exceptfds,
                             &k_readfds, &k_writefds, &k_exceptfds);
    }
    return ret;
}

int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask)
{
    fd_set k_readfds, k_writefds, k_exceptfds;
    int max_kfd = fdsets_vfs_to_kernel(nfds, readfds, writefds, exceptfds,
                                       &k_readfds, &k_writefds, &k_exceptfds);
    if (max_kfd < 0) { return -1; }

    int ret = freertos_linux_coop_pselect(max_kfd,
                                          readfds   ? &k_readfds   : NULL,
                                          writefds  ? &k_writefds  : NULL,
                                          exceptfds ? &k_exceptfds : NULL,
                                          timeout, sigmask);
    if (ret >= 0) {
        fdsets_kernel_to_vfs(nfds, readfds, writefds, exceptfds,
                             &k_readfds, &k_writefds, &k_exceptfds);
    }
    return ret;
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    struct timeval tv = {
        .tv_sec = timeout / 1000,
        .tv_usec = (timeout % 1000) * 1000,
    };
    int max_fd = -1;
    fd_set readfds;
    fd_set writefds;
    fd_set errorfds;

    if (fds == NULL) {
        errno = ENOENT;
        return -1;
    }

    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errorfds);

    for (unsigned int i = 0; i < nfds; ++i) {
        fds[i].revents = 0;

        if (fds[i].fd < 0) {
            continue;
        }

        if (fds[i].fd >= FD_SETSIZE) {
            fds[i].revents |= POLLNVAL;
            continue;
        }

        if (fds[i].events & (POLLIN | POLLRDNORM | POLLRDBAND | POLLPRI)) {
            FD_SET(fds[i].fd, &readfds);
            FD_SET(fds[i].fd, &errorfds);
            max_fd = MAX(max_fd, fds[i].fd);
        }

        if (fds[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND)) {
            FD_SET(fds[i].fd, &writefds);
            FD_SET(fds[i].fd, &errorfds);
            max_fd = MAX(max_fd, fds[i].fd);
        }
    }

    const int select_ret = select(max_fd + 1, &readfds, &writefds, &errorfds, timeout < 0 ? NULL : &tv);

    if (select_ret < 0) {
        return select_ret;
    }

    if (select_ret > 0) {
        for (unsigned int i = 0; i < nfds; ++i) {
            if (fds[i].fd < 0 || fds[i].fd >= FD_SETSIZE) {
                continue;
            }

            if (FD_ISSET(fds[i].fd, &readfds)) {
                fds[i].revents |= POLLIN;
            }

            if (FD_ISSET(fds[i].fd, &writefds)) {
                fds[i].revents |= POLLOUT;
            }

            if (FD_ISSET(fds[i].fd, &errorfds)) {
                fds[i].revents |= POLLERR;
            }
        }
    }

    int ret = 0;
    for (unsigned int i = 0; i < nfds; ++i) {
        if (fds[i].revents != 0) {
            ++ret;
        }
    }

    return ret;
}
