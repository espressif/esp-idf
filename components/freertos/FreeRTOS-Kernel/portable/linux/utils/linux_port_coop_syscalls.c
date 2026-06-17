/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Cooperative syscall interposition for Linux/macOS FreeRTOS simulator.
 *
 * This file provides:
 *  - Shadow FD state table (user-visible blocking/non-blocking tracking)
 *  - Cooperative I/O primitives (freertos_linux_coop_read/write/open/close/…)
 *  - Weak POSIX symbol definitions (read/write/open/close/fcntl/select/…)
 *    that VFS can override with strong definitions
 *  - Socket, scatter/gather, multiplexing, sleep wrappers
 *
 * Real libc entry points are resolved via dlsym(RTLD_NEXT, …).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <poll.h>
#include <dlfcn.h>
#include <pthread.h>

#include "linux_port_coop_internal.h"
#include "esp_private/freertos_linux_coop_syscalls.h"

static ssize_t  (*real_readv)(int, const struct iovec *, int);
static ssize_t  (*real_writev)(int, const struct iovec *, int);
static ssize_t  (*real_recv)(int, void *, size_t, int);
static ssize_t  (*real_send)(int, const void *, size_t, int);
static ssize_t  (*real_recvfrom)(int, void *, size_t, int,
                                 struct sockaddr *, socklen_t *);
static ssize_t  (*real_sendto)(int, const void *, size_t, int,
                                const struct sockaddr *, socklen_t);
static ssize_t  (*real_recvmsg)(int, struct msghdr *, int);
static ssize_t  (*real_sendmsg)(int, const struct msghdr *, int);
static int      (*real_connect)(int, const struct sockaddr *, socklen_t);
static int      (*real_accept)(int, struct sockaddr *, socklen_t *);
static int      (*real_pselect)(int, fd_set *, fd_set *, fd_set *,
                                const struct timespec *, const sigset_t *);
static int      (*real_poll)(struct pollfd *, nfds_t, int);
static int      (*real_socket)(int, int, int);
static int      (*real_socketpair)(int, int, int, int[2]);
static int      (*real_pipe)(int[2]);
static int      (*real_pipe2)(int[2], int);
static int      (*real_dup)(int);
static int      (*real_dup2)(int, int);
static int      (*real_nanosleep)(const struct timespec *, struct timespec *);
static ssize_t  (*real_read)(int, void *, size_t);
static ssize_t  (*real_write)(int, const void *, size_t);
static ssize_t  (*real_pread)(int, void *, size_t, off_t);
static ssize_t  (*real_pwrite)(int, const void *, size_t, off_t);
static int      (*real_close)(int);
static int      (*real_select)(int, fd_set *, fd_set *, fd_set *,
                                struct timeval *);

typedef int (*linux_coop_fcntl_fn_t)(int, int, ...);
static linux_coop_fcntl_fn_t real_fcntl;

typedef int (*linux_coop_open_fn_t)(const char *, int, mode_t);
static linux_coop_open_fn_t real_open;

typedef struct linux_coop_fd_state {
    int fd;
    int user_flags;
    struct linux_coop_fd_state *next;
} linux_coop_fd_state_t;

static linux_coop_fd_state_t *s_fd_state_list;
static pthread_mutex_t s_fd_state_lock = PTHREAD_MUTEX_INITIALIZER;

static linux_coop_fd_state_t *linux_coop_find_fd_locked(int fd)
{
    linux_coop_fd_state_t *node = s_fd_state_list;
    while (node != NULL) {
        if (node->fd == fd) {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

static void __attribute__((constructor)) linux_coop_resolve_all(void)
{
    LINUX_COOP_RESOLVE(readv);
    LINUX_COOP_RESOLVE(writev);
    LINUX_COOP_RESOLVE(recv);
    LINUX_COOP_RESOLVE(send);
    LINUX_COOP_RESOLVE(recvfrom);
    LINUX_COOP_RESOLVE(sendto);
    LINUX_COOP_RESOLVE(recvmsg);
    LINUX_COOP_RESOLVE(sendmsg);
    LINUX_COOP_RESOLVE(connect);
    LINUX_COOP_RESOLVE(accept);
    LINUX_COOP_RESOLVE(pselect);
    LINUX_COOP_RESOLVE(poll);
    LINUX_COOP_RESOLVE(socket);
    LINUX_COOP_RESOLVE(socketpair);
    LINUX_COOP_RESOLVE(pipe);
    LINUX_COOP_RESOLVE(pipe2);     /* may be NULL on macOS */
    LINUX_COOP_RESOLVE(dup);
    LINUX_COOP_RESOLVE(dup2);
    LINUX_COOP_RESOLVE(nanosleep);
    LINUX_COOP_RESOLVE(fcntl);
    LINUX_COOP_RESOLVE(read);
    LINUX_COOP_RESOLVE(write);
    LINUX_COOP_RESOLVE(pread);
    LINUX_COOP_RESOLVE(pwrite);
    LINUX_COOP_RESOLVE(close);
    LINUX_COOP_RESOLVE(select);
    LINUX_COOP_RESOLVE(open);
}

void linux_coop_set_nonblocking(int fd)
{
    if (fd >= 0) {
        int flags = real_fcntl(fd, F_GETFL, 0);
        if (flags >= 0) {
            real_fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    }
}

void linux_coop_track_fd(int fd, int user_flags)
{
    if (fd < 0) {
        return;
    }

    pthread_mutex_lock(&s_fd_state_lock);
    linux_coop_fd_state_t *node = linux_coop_find_fd_locked(fd);
    if (node != NULL) {
        node->user_flags = user_flags;
        pthread_mutex_unlock(&s_fd_state_lock);
        return;
    }

    node = calloc(1, sizeof(*node));
    if (node != NULL) {
        node->fd = fd;
        node->user_flags = user_flags;
        node->next = s_fd_state_list;
        s_fd_state_list = node;
    }
    pthread_mutex_unlock(&s_fd_state_lock);
}

void linux_coop_untrack_fd(int fd)
{
    if (fd < 0) {
        return;
    }

    pthread_mutex_lock(&s_fd_state_lock);
    linux_coop_fd_state_t *prev = NULL;
    linux_coop_fd_state_t *node = s_fd_state_list;
    while (node != NULL) {
        if (node->fd == fd) {
            if (prev == NULL) {
                s_fd_state_list = node->next;
            } else {
                prev->next = node->next;
            }
            free(node);
            break;
        }
        prev = node;
        node = node->next;
    }
    pthread_mutex_unlock(&s_fd_state_lock);
}

void linux_coop_set_user_flags(int fd, int user_flags)
{
    linux_coop_track_fd(fd, user_flags);
}

bool linux_coop_get_user_flags(int fd, int *flags)
{
    if (fd < 0 || flags == NULL) {
        return false;
    }

    pthread_mutex_lock(&s_fd_state_lock);
    linux_coop_fd_state_t *node = linux_coop_find_fd_locked(fd);
    if (node == NULL) {
        pthread_mutex_unlock(&s_fd_state_lock);
        return false;
    }
    *flags = node->user_flags;
    pthread_mutex_unlock(&s_fd_state_lock);
    return true;
}

bool linux_coop_fd_user_nonblocking(int fd)
{
    int user_flags = 0;
    if (linux_coop_get_user_flags(fd, &user_flags)) {
        return (user_flags & O_NONBLOCK) != 0;
    }
    return false;
}


ssize_t freertos_linux_coop_read(int fd, void *buf, size_t count)
{
    LINUX_COOP_IO_LOOP(fd, real_read(fd, buf, count))
}

ssize_t freertos_linux_coop_write(int fd, const void *buf, size_t count)
{
    LINUX_COOP_IO_LOOP(fd, real_write(fd, buf, count))
}

ssize_t freertos_linux_coop_pread(int fd, void *buf, size_t count, off_t offset)
{
    LINUX_COOP_IO_LOOP(fd, real_pread(fd, buf, count, offset))
}

ssize_t freertos_linux_coop_pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    LINUX_COOP_IO_LOOP(fd, real_pwrite(fd, buf, count, offset))
}

int freertos_linux_coop_open(const char *path, int flags, int mode)
{
    int fd = real_open(path, flags, (mode_t)mode);
    linux_coop_set_nonblocking(fd);
    linux_coop_track_fd(fd, flags & O_NONBLOCK);
    return fd;
}

int freertos_linux_coop_close(int fd)
{
    int ret = real_close(fd);
    if (ret == 0 || errno == EINTR) {
        /* On Linux, close() always releases the fd even when returning
         * EINTR.  Retrying would risk closing a fd reused by another
         * thread.  Treat EINTR as success. */
        linux_coop_untrack_fd(fd);
        return 0;
    }
    return -1;
}

int freertos_linux_coop_fcntl(int fd, int cmd, int arg)
{
    if (cmd == F_GETFL) {
        int real_flags = real_fcntl(fd, F_GETFL, 0);
        if (real_flags < 0) {
            return -1;
        }
        int user_flags = 0;
        if (linux_coop_get_user_flags(fd, &user_flags)) {
            real_flags &= ~O_NONBLOCK;
            real_flags |= (user_flags & O_NONBLOCK);
        }
        return real_flags;
    }
    if (cmd == F_SETFL) {
        linux_coop_set_user_flags(fd, arg);
        int internal_flags = arg | O_NONBLOCK;
        return real_fcntl(fd, F_SETFL, internal_flags);
    }
    return real_fcntl(fd, cmd, arg);
}

int freertos_linux_coop_select(int nfds, fd_set *readfds, fd_set *writefds,
                      fd_set *exceptfds, struct timeval *timeout)
{
    long timeout_ms = -1;
    if (timeout != NULL) {
        timeout_ms = (long)timeout->tv_sec * 1000 +
                     (timeout->tv_usec + 999) / 1000;
        if (timeout_ms == 0) {
            return real_select(nfds, readfds, writefds, exceptfds, timeout);
        }
    }

    /* real_select() mutates the fd_sets in place (clears them, then sets bits
     * for ready descriptors). To keep polling the same descriptors across
     * iterations we must snapshot the caller's sets and restore them on every
     * loop. */
    fd_set rfds_in, wfds_in, efds_in;
    if (readfds != NULL) {
        rfds_in = *readfds;
    }
    if (writefds != NULL) {
        wfds_in = *writefds;
    }
    if (exceptfds != NULL) {
        efds_in = *exceptfds;
    }

    long waited_ms = 0;
    while (1) {
        if (readfds != NULL) {
            *readfds = rfds_in;
        }
        if (writefds != NULL) {
            *writefds = wfds_in;
        }
        if (exceptfds != NULL) {
            *exceptfds = efds_in;
        }

        struct timeval zero_tv = {0, 0};
        int ret = real_select(nfds, readfds, writefds, exceptfds, &zero_tv);
        if (ret != 0) {
            return ret;
        }
        if (timeout_ms == 0) {
            return 0;
        }
        if (timeout_ms > 0 && waited_ms >= timeout_ms) {
            return 0;
        }
        linux_coop_yield(LINUX_COOP_TICK_MS);
        waited_ms += LINUX_COOP_TICK_MS;
    }
}

void freertos_linux_coop_syscalls_init(void)
{
    linux_coop_set_nonblocking(STDIN_FILENO);
    linux_coop_set_nonblocking(STDOUT_FILENO);
    linux_coop_set_nonblocking(STDERR_FILENO);
    linux_coop_track_fd(STDIN_FILENO, 0);
    linux_coop_track_fd(STDOUT_FILENO, 0);
    linux_coop_track_fd(STDERR_FILENO, 0);
}

/* These are overridden by strong definitions in VFS when that component
 * is linked.  When VFS is absent the weak versions provide cooperative
 * behaviour directly via real libc. */

__attribute__((weak)) ssize_t read(int fd, void *buf, size_t count)
{
    return freertos_linux_coop_read(fd, buf, count);
}

__attribute__((weak)) ssize_t write(int fd, const void *buf, size_t count)
{
    return freertos_linux_coop_write(fd, buf, count);
}

__attribute__((weak)) ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    return freertos_linux_coop_pread(fd, buf, count, offset);
}

__attribute__((weak)) ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    return freertos_linux_coop_pwrite(fd, buf, count, offset);
}

__attribute__((weak)) int open(const char *path, int flags, ...)
{
    int mode = 0;
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, int);
        va_end(ap);
    }
    return freertos_linux_coop_open(path, flags, mode);
}

__attribute__((weak)) int close(int fd)
{
    return freertos_linux_coop_close(fd);
}

__attribute__((weak)) int fcntl(int fd, int cmd, ...)
{
    int arg = 0;
    if (cmd != F_GETFL) {
        va_list list;
        va_start(list, cmd);
        arg = va_arg(list, int);
        va_end(list);
    }
    return freertos_linux_coop_fcntl(fd, cmd, arg);
}

__attribute__((weak)) int select(int nfds, fd_set *readfds, fd_set *writefds,
                                 fd_set *exceptfds, struct timeval *timeout)
{
    return freertos_linux_coop_select(nfds, readfds, writefds, exceptfds, timeout);
}

ssize_t freertos_linux_coop_readv(int fd, const struct iovec *iov, int iovcnt)
{
    LINUX_COOP_IO_LOOP(fd, real_readv(fd, iov, iovcnt))
}

ssize_t freertos_linux_coop_writev(int fd, const struct iovec *iov, int iovcnt)
{
    LINUX_COOP_IO_LOOP(fd, real_writev(fd, iov, iovcnt))
}

ssize_t freertos_linux_coop_recv(int sockfd, void *buf, size_t len, int flags)
{
    LINUX_COOP_IO_LOOP(sockfd, real_recv(sockfd, buf, len, flags | MSG_DONTWAIT))
}

ssize_t freertos_linux_coop_send(int sockfd, const void *buf, size_t len, int flags)
{
    LINUX_COOP_IO_LOOP(sockfd, real_send(sockfd, buf, len, flags | MSG_DONTWAIT))
}

ssize_t freertos_linux_coop_recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
    LINUX_COOP_IO_LOOP(sockfd, real_recvfrom(sockfd, buf, len, flags | MSG_DONTWAIT,
                            src_addr, addrlen))
}

ssize_t freertos_linux_coop_sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
{
    LINUX_COOP_IO_LOOP(sockfd, real_sendto(sockfd, buf, len, flags | MSG_DONTWAIT,
                          dest_addr, addrlen))
}

ssize_t freertos_linux_coop_recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    LINUX_COOP_IO_LOOP(sockfd, real_recvmsg(sockfd, msg, flags | MSG_DONTWAIT))
}

ssize_t freertos_linux_coop_sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    LINUX_COOP_IO_LOOP(sockfd, real_sendmsg(sockfd, msg, flags | MSG_DONTWAIT))
}

__attribute__((weak)) ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    return freertos_linux_coop_readv(fd, iov, iovcnt);
}

__attribute__((weak)) ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    return freertos_linux_coop_writev(fd, iov, iovcnt);
}

__attribute__((weak)) ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
    return freertos_linux_coop_recv(sockfd, buf, len, flags);
}

__attribute__((weak)) ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
    return freertos_linux_coop_send(sockfd, buf, len, flags);
}

__attribute__((weak)) ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen)
{
    return freertos_linux_coop_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

__attribute__((weak)) ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return freertos_linux_coop_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
}

__attribute__((weak)) ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    return freertos_linux_coop_recvmsg(sockfd, msg, flags);
}

__attribute__((weak)) ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    return freertos_linux_coop_sendmsg(sockfd, msg, flags);
}

int freertos_linux_coop_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    while (1)
    {
        int ret = real_connect(sockfd, addr, addrlen);
        if (ret == 0) {
            return ret;
        }

        if (errno == EINPROGRESS || errno == EALREADY) {
            struct pollfd pfd;
            pfd.fd = sockfd;
            pfd.events = POLLOUT;
            pfd.revents = 0;

            int poll_retries = 0;
            while (1)
            {
                int press = real_poll(&pfd, 1, 0);
                if (press > 0) {
                    int so_err = 0;
                    socklen_t len = sizeof(so_err);
                    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
                                   &so_err, &len) < 0) {
                        return -1;
                    }
                    if (so_err == 0) {
                        return 0;
                    } else {
                        errno = so_err;
                        return -1;
                    }
                } else if (press == 0) {
                    linux_coop_yield(LINUX_COOP_TICK_MS);
                    continue;
                } else {
                    if (errno == EINTR) {
                        continue;
                    }
                    /* Non-EINTR poll error; bail out after a few retries */
                    if (++poll_retries > 3) {
                        return -1;
                    }
                    linux_coop_yield(LINUX_COOP_TICK_MS);
                    continue;
                }
            }
        }

        if (errno == EINTR) {
            return -1;
        }

        return -1;
    }
}

int freertos_linux_coop_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    while (1)
    {
        int fd = real_accept(sockfd, addr, addrlen);
        if (fd >= 0) {
            linux_coop_set_nonblocking(fd);
            linux_coop_track_fd(fd, linux_coop_fd_user_nonblocking(sockfd) ? O_NONBLOCK : 0);
            return fd;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (linux_coop_fd_user_nonblocking(sockfd)) {
                return -1;
            }
            linux_coop_yield(LINUX_COOP_TICK_MS);
            continue;
        }
        return -1;
    }
}

int freertos_linux_coop_pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask)
{
    long timeout_ms = -1;
    if (timeout != NULL) {
        timeout_ms = (long)timeout->tv_sec * 1000 +
                     (timeout->tv_nsec + 999999) / 1000000;
        if (timeout_ms == 0) {
            return real_pselect(nfds, readfds, writefds, exceptfds,
                                timeout, sigmask);
        }
    }

    long waited_ms = 0;
    while (1)
    {
        struct timespec zero_ts = {0, 0};
        int ret = real_pselect(nfds, readfds, writefds, exceptfds,
                               &zero_ts, sigmask);
        if (ret != 0) {
            return ret;
        }

        if (timeout_ms == 0 ||
            (timeout_ms > 0 && waited_ms >= timeout_ms)) {
            return 0;
        }

        linux_coop_yield(LINUX_COOP_TICK_MS);
        waited_ms += LINUX_COOP_TICK_MS;
    }
}

int freertos_linux_coop_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    if (timeout == 0) {
        return real_poll(fds, nfds, 0);
    }

    long timeout_ms = -1;
    if (timeout > 0) {
        timeout_ms = timeout;
    }

    long waited_ms = 0;

    while (1)
    {
        int ret = real_poll(fds, nfds, 0);
        if (ret > 0) {
            return ret;
        } else if (ret == 0) {
            if (timeout_ms == 0 ||
                (timeout_ms > 0 && waited_ms >= timeout_ms)) {
                return 0;
            }
            linux_coop_yield(LINUX_COOP_TICK_MS);
            waited_ms += LINUX_COOP_TICK_MS;
            continue;
        } else {
            if (errno == EINTR) {
                continue;
            }
            linux_coop_yield(LINUX_COOP_TICK_MS);
            if (timeout_ms > 0 && waited_ms >= timeout_ms) {
                return -1;
            }
            waited_ms += LINUX_COOP_TICK_MS;
        }
    }
}

int nanosleep(const struct timespec *req, struct timespec *rem)
{
    if (req == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (!linux_port_in_freertos_task()) {
        return real_nanosleep(req, rem);
    }
    long ms = req->tv_sec * 1000 + (req->tv_nsec + 999999) / 1000000;
    if (ms <= 0) {
        ms = 1;
    }
    vTaskDelay(ms);
    if (rem) {
        rem->tv_sec = 0;
        rem->tv_nsec = 0;
    }
    return 0;
}

unsigned int sleep(unsigned int seconds)
{
    linux_coop_yield(seconds * 1000);
    return 0;
}

int usleep(useconds_t usec)
{
    linux_coop_yield(usec / 1000);
    return 0;
}

int freertos_linux_coop_socket(int domain, int type, int protocol)
{
    int fd = real_socket(domain, type, protocol);
    linux_coop_set_nonblocking(fd);
#ifdef SOCK_NONBLOCK
    const int user_flags = (type & SOCK_NONBLOCK) ? O_NONBLOCK : 0;
#else
    const int user_flags = 0;
#endif
    linux_coop_track_fd(fd, user_flags);
    return fd;
}

int freertos_linux_coop_socketpair(int domain, int type, int protocol, int *sv)
{
    int ret = real_socketpair(domain, type, protocol, sv);
    if (ret == 0) {
        linux_coop_set_nonblocking(sv[0]);
        linux_coop_set_nonblocking(sv[1]);
#ifdef SOCK_NONBLOCK
        const int user_flags = (type & SOCK_NONBLOCK) ? O_NONBLOCK : 0;
#else
        const int user_flags = 0;
#endif
        linux_coop_track_fd(sv[0], user_flags);
        linux_coop_track_fd(sv[1], user_flags);
    }
    return ret;
}

int freertos_linux_coop_pipe(int *fds)
{
    int ret = real_pipe(fds);
    if (ret == 0) {
        linux_coop_set_nonblocking(fds[0]);
        linux_coop_set_nonblocking(fds[1]);
        linux_coop_track_fd(fds[0], 0);
        linux_coop_track_fd(fds[1], 0);
    }
    return ret;
}

int freertos_linux_coop_pipe2(int *fds, int flags)
{
    int ret = real_pipe2(fds, flags);
    if (ret == 0) {
        linux_coop_set_nonblocking(fds[0]);
        linux_coop_set_nonblocking(fds[1]);
        const int user_flags = (flags & O_NONBLOCK) ? O_NONBLOCK : 0;
        linux_coop_track_fd(fds[0], user_flags);
        linux_coop_track_fd(fds[1], user_flags);
    }
    return ret;
}

int freertos_linux_coop_dup(int oldfd)
{
    int fd = real_dup(oldfd);
    linux_coop_set_nonblocking(fd);
    linux_coop_track_fd(fd, linux_coop_fd_user_nonblocking(oldfd) ? O_NONBLOCK : 0);
    return fd;
}

int freertos_linux_coop_dup2(int oldfd, int newfd)
{
    int fd = real_dup2(oldfd, newfd);
    linux_coop_set_nonblocking(fd);
    linux_coop_track_fd(fd, linux_coop_fd_user_nonblocking(oldfd) ? O_NONBLOCK : 0);
    return fd;
}

__attribute__((weak)) int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    return freertos_linux_coop_connect(sockfd, addr, addrlen);
}

__attribute__((weak)) int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    return freertos_linux_coop_accept(sockfd, addr, addrlen);
}

__attribute__((weak)) int pselect(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, const struct timespec *timeout,
            const sigset_t *sigmask)
{
    return freertos_linux_coop_pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
}

__attribute__((weak)) int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    return freertos_linux_coop_poll(fds, nfds, timeout);
}

__attribute__((weak)) int socket(int domain, int type, int protocol)
{
    return freertos_linux_coop_socket(domain, type, protocol);
}

__attribute__((weak)) int socketpair(int domain, int type, int protocol, int sv[2])
{
    return freertos_linux_coop_socketpair(domain, type, protocol, sv);
}

__attribute__((weak)) int pipe(int fds[2])
{
    return freertos_linux_coop_pipe(fds);
}

__attribute__((weak)) int pipe2(int fds[2], int flags)
{
    return freertos_linux_coop_pipe2(fds, flags);
}

__attribute__((weak)) int dup(int oldfd)
{
    return freertos_linux_coop_dup(oldfd);
}

__attribute__((weak)) int dup2(int oldfd, int newfd)
{
    return freertos_linux_coop_dup2(oldfd, newfd);
}
