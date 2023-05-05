/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/socket.h>
#include <stdarg.h>

extern int __real_fcntl(int s, int cmd, ...);
extern int __real_close(int s);
extern ssize_t __real_write (int fd, const void *buf, size_t n);
extern ssize_t __real_read (int fd, void *buf, size_t n);
extern int __real_select (int fd, fd_set * rfds, fd_set * wfds, fd_set *efds, struct timeval *tval);

ssize_t __wrap_write (int fd, const void *buf, size_t n)
{
#ifdef CONFIG_LWIP_MAX_SOCKETS
    if (fd >= LWIP_SOCKET_OFFSET)
        return lwip_write(fd, buf, n);
#endif
    return __real_write(fd, buf, n);
}

ssize_t __wrap_read (int fd, void *buf, size_t n)
{
#ifdef CONFIG_LWIP_MAX_SOCKETS
    if (fd >= LWIP_SOCKET_OFFSET)
        return lwip_read(fd, buf, n);
#endif
    return __real_read(fd, buf, n);
}

int __wrap_select (int fd, fd_set * rds, fd_set * wfds, fd_set *efds, struct timeval *tval)
{
#ifdef CONFIG_LWIP_MAX_SOCKETS
    if (fd >= LWIP_SOCKET_OFFSET)
        return lwip_select(fd, rds, wfds, efds, tval);
#endif
    return __real_select(fd, rds, wfds, efds, tval);
}

int __wrap_fcntl(int fd, int cmd, ...)
{
    va_list args;
#ifdef CONFIG_LWIP_MAX_SOCKETS
    if (fd >= LWIP_SOCKET_OFFSET) {
        va_start(args, cmd);
        int arg = va_arg(args, int);
        va_end(args);
        return lwip_fcntl(fd, cmd, arg);
    }
#endif

    return __real_fcntl(fd, cmd, args);
}

int __wrap_close(int fd)
{
#ifdef CONFIG_LWIP_MAX_SOCKETS
    if (fd >= LWIP_SOCKET_OFFSET)
        return lwip_close(fd);
#endif
    return __real_close(fd);
}
