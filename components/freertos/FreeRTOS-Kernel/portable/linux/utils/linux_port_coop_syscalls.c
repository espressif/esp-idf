/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Cooperative wrappers for Linux FreeRTOS simulator.
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
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "task.h"

#define COOP_SYSCALLS_WAIT_MS (1000 / CONFIG_FREERTOS_HZ)

extern bool linux_port_in_freertos_task(void);

static inline __attribute__((always_inline))
void coop_set_fd_nonblocking(int fd)
{
    if (fd >= 0) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags >= 0) {
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
    }
}

static inline __attribute__((always_inline))
void coop_wait(int ms)
{
    if (linux_port_in_freertos_task()) {
        vTaskDelay(ms);
    } else {
        struct timespec ts;
        ts.tv_sec  = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000000L;
        while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
            // Retry with remaining time if interrupted
        }
    }
}

/* Generic cooperative loop template */
#define COOP_LOOP(start_expr)                                   \
    while (1)                                                   \
    {                                                           \
        ssize_t n = start_expr;                                 \
        if (n >= 0) {                                           \
            return n;                                           \
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {   \
            coop_wait(COOP_SYSCALLS_WAIT_MS);                   \
            continue;                                           \
        } else {                                                \
            return -1;                                          \
        }                                                       \
    }

ssize_t __real_read(int fd, void *buf, size_t count);
ssize_t __real_write(int fd, const void *buf, size_t count);
ssize_t __real_pread(int fd, void *buf, size_t count, off_t offset);
ssize_t __real_pwrite(int fd, const void *buf, size_t count, off_t offset);
ssize_t __real_readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t __real_writev(int fd, const struct iovec *iov, int iovcnt);
ssize_t __real_recv(int sockfd, void *buf, size_t len, int flags);
ssize_t __real_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t __real_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t __real_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
ssize_t __real_recvmsg(int sockfd, struct msghdr *msg, int flags);
ssize_t __real_sendmsg(int sockfd, const struct msghdr *msg, int flags);
int __real_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int __real_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int __real_close(int fd);
int __real_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int __real_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask);
int __real_poll(struct pollfd *fds, nfds_t nfds, int timeout);
unsigned int __real_sleep(unsigned int seconds);
int __real_usleep(useconds_t usec);

int __real_socket(int domain, int type, int protocol);
int __real_socketpair(int domain, int type, int protocol, int sv[2]);
int __real_pipe(int fds[2]);
int __real_pipe2(int fds[2], int flags);
int __real_dup(int oldfd);
int __real_dup2(int oldfd, int newfd);
int __real_open(const char *path, int flags, ...);

ssize_t __wrap_read(int fd, void *buf, size_t count)
{
    COOP_LOOP(__real_read(fd, buf, count))
}

ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
    COOP_LOOP(__real_write(fd, buf, count))
}

ssize_t __wrap_pread(int fd, void *buf, size_t count, off_t offset)
{
    COOP_LOOP(__real_pread(fd, buf, count, offset))
}

ssize_t __wrap_pwrite(int fd, const void *buf, size_t count, off_t offset)
{
    COOP_LOOP(__real_pwrite(fd, buf, count, offset))
}

ssize_t __wrap_readv(int fd, const struct iovec *iov, int iovcnt)
{
    COOP_LOOP(__real_readv(fd, iov, iovcnt))
}

ssize_t __wrap_writev(int fd, const struct iovec *iov, int iovcnt)
{
    COOP_LOOP(__real_writev(fd, iov, iovcnt))
}

ssize_t __wrap_recv(int sockfd, void *buf, size_t len, int flags)
{
    COOP_LOOP(__real_recv(sockfd, buf, len, flags | MSG_DONTWAIT))
}

ssize_t __wrap_send(int sockfd, const void *buf, size_t len, int flags)
{
    COOP_LOOP(__real_send(sockfd, buf, len, flags | MSG_DONTWAIT))
}

ssize_t __wrap_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
    COOP_LOOP(__real_recvfrom(sockfd, buf, len, flags | MSG_DONTWAIT, src_addr, addrlen))
}

ssize_t __wrap_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    COOP_LOOP(__real_sendto(sockfd, buf, len, flags | MSG_DONTWAIT, dest_addr, addrlen))
}

ssize_t __wrap_recvmsg(int sockfd, struct msghdr *msg, int flags)
{
    COOP_LOOP(__real_recvmsg(sockfd, msg, flags | MSG_DONTWAIT))
}

ssize_t __wrap_sendmsg(int sockfd, const struct msghdr *msg, int flags)
{
    COOP_LOOP(__real_sendmsg(sockfd, msg, flags | MSG_DONTWAIT))
}

int __wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    while (1)
    {
        int ret = __real_connect(sockfd, addr, addrlen);
        if (ret == 0) {
            return ret;
        }

        if (errno == EINPROGRESS || errno == EALREADY) {
            /* Poll for writability (use __real_poll in cooperative loop,
               but do not block the kernel thread). We'll emulate blocking
               by repeatedly polling with 0 timeout and yielding. */
            struct pollfd pfd;
            pfd.fd = sockfd;
            pfd.events = POLLOUT;
            pfd.revents = 0;

            while (1)
            {
                int press = __real_poll(&pfd, 1, 0);
                if (press > 0) {
                    /* socket reported an event; check if connect succeeded */
                    int so_err = 0;
                    socklen_t len = sizeof(so_err);
                    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_err, &len) < 0) {
                        /* getsockopt failed; treat as error */
                        return -1;
                    }

                    if (so_err == 0) {
                        return 0; /* connected */
                    } else {
                        errno = so_err;
                        return -1;
                    }
                } else if (press == 0) {
                    /* no event yet -> yield cooperatively and retry */
                    coop_wait(COOP_SYSCALLS_WAIT_MS);
                    continue;
                } else {
                    /* press < 0 */
                    if (errno == EINTR) {
                        continue; /* retry poll */
                    }

                    /* treat other errors as transient and yield */
                    coop_wait(COOP_SYSCALLS_WAIT_MS);
                    continue;
                }
            }
        }

        if (errno == EINTR) {
            /* POSIX: connect may fail with EINTR; return -1 with errno==EINTR */
            return -1;
        }

        /* other fatal errors */
        return -1;
    }
}

int __wrap_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    COOP_LOOP(__real_accept(sockfd, addr, addrlen))
}

int __wrap_close(int fd)
{
    while (1)
    {
        int ret = __real_close(fd);
        if (ret == 0) {
            return 0;
        } else if (errno == EINTR) {
            coop_wait(COOP_SYSCALLS_WAIT_MS);
            continue;
        } else {
            return -1;
        }
    }
}

int __wrap_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
    /* compute timeout in milliseconds; -1 => infinite */
    long timeout_ms = -1;
    if (timeout != NULL) {
        /* convert timeval -> ms, rounding up microseconds */
        timeout_ms = (long)timeout->tv_sec * 1000 + (timeout->tv_usec + 999) / 1000;
        if (timeout_ms == 0) {
            /* immediate poll: call real_select with provided timeout */
            return __real_select(nfds, readfds, writefds, exceptfds, timeout);
        }
    }

    long waited_ms = 0;
    while (1)
    {
        /* nonblocking check */
        struct timeval zero_tv = {0, 0};
        int ret = __real_select(nfds, readfds, writefds, exceptfds, &zero_tv);
        if (ret != 0) {
            /* ret > 0 => ready; ret < 0 => error and errno set */
            return ret;
        }

        /* no descriptors ready */
        if (timeout_ms == 0) {
            return 0; /* expired */
        }

        /* check timeout expiration */
        if (timeout_ms > 0 && waited_ms >= timeout_ms) {
            return 0; /* timeout expired */
        }

        /* yield cooperatively */
        coop_wait(COOP_SYSCALLS_WAIT_MS);
        waited_ms += COOP_SYSCALLS_WAIT_MS;
    }
}

int __wrap_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                        const struct timespec *timeout, const sigset_t *sigmask)
{
    /* convert timespec -> ms, -1 for infinite */
    long timeout_ms = -1;
    if (timeout != NULL) {
        timeout_ms = (long)timeout->tv_sec * 1000 + (timeout->tv_nsec + 999999) / 1000000;
        if (timeout_ms == 0) {
            /* immediate poll: call real_pselect with provided timeout */
            return __real_pselect(nfds, readfds, writefds, exceptfds, timeout, sigmask);
        }
    }

    long waited_ms = 0;
    while (1)
    {
        struct timespec zero_ts = {0, 0};
        int ret = __real_pselect(nfds, readfds, writefds, exceptfds, &zero_ts, sigmask);
        if (ret != 0) {
            return ret;
        }

        if (timeout_ms == 0 ||
            (timeout_ms > 0 && waited_ms >= timeout_ms)) {
            return 0;
        }

        coop_wait(COOP_SYSCALLS_WAIT_MS);
        waited_ms += COOP_SYSCALLS_WAIT_MS;
    }
}

int __wrap_poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
    if (timeout == 0) {
        /* immediate poll: delegate */
        return __real_poll(fds, nfds, 0);
    }

    /* compute wait semantics */
    long timeout_ms = -1;
    if (timeout > 0) {
        timeout_ms = timeout;
    }

    long waited_ms = 0;

    while (1)
    {
        int ret = __real_poll(fds, nfds, 0);
        if (ret > 0) {
            return ret;
        }
        else if (ret == 0) {
            /* no event; check timeout */
            if (timeout_ms == 0 ||
                (timeout_ms > 0 && waited_ms >= timeout_ms)) {
                return 0; /* expired */
            }

            /* yield and continue */
            coop_wait(COOP_SYSCALLS_WAIT_MS);
            waited_ms += COOP_SYSCALLS_WAIT_MS;
            continue;
        } else {
            /* ret < 0: error */
            if (errno == EINTR) {
                continue; /* retry */
            }

            /* for other errors, yield and retry (transient) */
            coop_wait(COOP_SYSCALLS_WAIT_MS);

            if (timeout_ms > 0 && waited_ms >= timeout_ms) {
                return -1;
            }
            waited_ms += COOP_SYSCALLS_WAIT_MS;
        }
    }
}

unsigned int __wrap_sleep(unsigned int seconds)
{
    coop_wait(seconds * 1000);
    return 0;
}

int __wrap_usleep(useconds_t usec)
{
    coop_wait((usec / 1000));
    return 0;
}

int __wrap_socket(int domain, int type, int protocol)
{
    int fd = __real_socket(domain, type, protocol);
    coop_set_fd_nonblocking(fd);
    return fd;
}

int __wrap_socketpair(int domain, int type, int protocol, int sv[2])
{
    int ret = __real_socketpair(domain, type, protocol, sv);
    if (ret == 0) {
        coop_set_fd_nonblocking(sv[0]);
        coop_set_fd_nonblocking(sv[1]);
    }
    return ret;
}

int __wrap_pipe(int fds[2])
{
    int ret = __real_pipe(fds);
    if (ret == 0) {
        coop_set_fd_nonblocking(fds[0]);
        coop_set_fd_nonblocking(fds[1]);
    }
    return ret;
}

int __wrap_pipe2(int fds[2], int flags)
{
    int ret = __real_pipe2(fds, flags);
    if (ret == 0) {
        coop_set_fd_nonblocking(fds[0]);
        coop_set_fd_nonblocking(fds[1]);
    }
    return ret;
}

int __wrap_dup(int oldfd)
{
    int fd = __real_dup(oldfd);
    coop_set_fd_nonblocking(fd);
    return fd;
}

int __wrap_dup2(int oldfd, int newfd)
{
    int fd = __real_dup2(oldfd, newfd);
    coop_set_fd_nonblocking(fd);
    return fd;
}

int __wrap_open(const char *path, int flags, ...)
{
    va_list ap;
    int fd;

    if (flags & O_CREAT) {
        va_start(ap, flags);
        mode_t mode = va_arg(ap, mode_t);
        va_end(ap);
        fd = __real_open(path, flags, mode);
    } else {
        fd = __real_open(path, flags);
    }
    coop_set_fd_nonblocking(fd);
    return fd;
}

void linux_port_coop_syscalls_init(void)
{
    coop_set_fd_nonblocking(STDIN_FILENO);
    coop_set_fd_nonblocking(STDOUT_FILENO);
    coop_set_fd_nonblocking(STDERR_FILENO);
}
