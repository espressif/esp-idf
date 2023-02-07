/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "sdkconfig.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#ifdef CONFIG_VFS_SUPPORT_IO
#error This file should only be built when CONFIG_VFS_SUPPORT_IO=n
#endif

/* Default implementations of read/write provided in newlib component,
 * used as a fallback for console I/O.
 */
extern ssize_t _write_r_console(struct _reent *r, int fd, const void * data, size_t size);
extern ssize_t _read_r_console(struct _reent *r, int fd, const void * data, size_t size);

ssize_t _write_r(struct _reent *r, int fd, const void * data, size_t size)
{
    if (fd < LWIP_SOCKET_OFFSET) {
        return _write_r_console(r, fd, data, size);
    }
    return lwip_write(fd, data, size);
}

ssize_t _read_r(struct _reent *r, int fd, void * dst, size_t size)
{
    if (fd < LWIP_SOCKET_OFFSET) {
        return _read_r_console(r, fd, dst, size);
    }
    return lwip_read(fd, dst, size);
}

int _close_r(struct _reent *r, int fd)
{
    if (fd < LWIP_SOCKET_OFFSET) {
        errno = ENOSYS;
        return -1;
    }
    return lwip_close(fd);
}

int _fcntl_r(struct _reent *r, int fd, int cmd, int arg)
{
    return lwip_fcntl(fd, cmd, arg);
}

int ioctl(int fd, int cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    int res = lwip_ioctl(fd, cmd, va_arg(args, void*));
    va_end(args);
    return res;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    return lwip_select(nfds, readfds, writefds, errorfds, timeout);
}

void esp_vfs_lwip_sockets_register(void)
{
    /* Doesn't register anything, just a hook to force linking this file */
}
