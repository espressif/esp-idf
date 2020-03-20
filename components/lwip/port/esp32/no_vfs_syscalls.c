// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
