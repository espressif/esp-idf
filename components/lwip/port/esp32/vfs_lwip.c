/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include "esp_attr.h"
#include "esp_vfs.h"
#include "sdkconfig.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#ifndef CONFIG_VFS_SUPPORT_IO
#error This file should only be built when CONFIG_VFS_SUPPORT_IO=y
#endif

_Static_assert(MAX_FDS >= CONFIG_LWIP_MAX_SOCKETS, "MAX_FDS < CONFIG_LWIP_MAX_SOCKETS");

#ifdef CONFIG_VFS_SUPPORT_SELECT

/**
 * @brief This function is implemented only in FreeRTOS port (ingroup sys_sem)
 * and has no official API counterpart in lwip's sys.h declarations
 * Signals a semaphore from ISR
 * @param sem the semaphore to signal
 * @return 1 if the signal has caused a high-prio task to unblock (pxHigherPriorityTaskWoken)
 */
int sys_sem_signal_isr(sys_sem_t *sem);

static void lwip_stop_socket_select(void *sem)
{
    sys_sem_signal(sem); //socket_select will return
}

static void lwip_stop_socket_select_isr(void *sem, BaseType_t *woken)
{
    if (sys_sem_signal_isr(sem) && woken) {
        *woken = pdTRUE;
    }
}

static void *lwip_get_socket_select_semaphore(void)
{
    /* Calling this from the same process as select() will ensure that the semaphore won't be allocated from
     * ISR (lwip_stop_socket_select_isr).
     */
    return (void *) sys_thread_sem_get();
}
#else // CONFIG_VFS_SUPPORT_SELECT

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    return lwip_select(nfds, readfds, writefds, errorfds, timeout);
}

#endif // CONFIG_VFS_SUPPORT_SELECT

static int lwip_fcntl_r_wrapper(int fd, int cmd, int arg)
{
    return lwip_fcntl(fd, cmd, arg);
}

static int lwip_ioctl_r_wrapper(int fd, int cmd, va_list args)
{
    return lwip_ioctl(fd, cmd, va_arg(args, void *));
}

static int lwip_fstat(int fd, struct stat * st)
{
    if (st == NULL || fd < LWIP_SOCKET_OFFSET || fd > (MAX_FDS - 1)) {
        errno = EBADF;
        return -1;
    }
    memset(st, 0, sizeof(*st));
    /* set the stat mode to socket type */
    st->st_mode = S_IFSOCK;
    return 0;
}

void esp_vfs_lwip_sockets_register(void)
{
    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &lwip_write,
        .open = NULL,
        .fstat = &lwip_fstat,
        .close = &lwip_close,
        .read = &lwip_read,
        .fcntl = &lwip_fcntl_r_wrapper,
        .ioctl = &lwip_ioctl_r_wrapper,
#ifdef CONFIG_VFS_SUPPORT_SELECT
        .socket_select = &lwip_select,
        .get_socket_select_semaphore = &lwip_get_socket_select_semaphore,
        .stop_socket_select = &lwip_stop_socket_select,
        .stop_socket_select_isr = &lwip_stop_socket_select_isr,
#endif // CONFIG_VFS_SUPPORT_SELECT
    };
    /* Non-LWIP file descriptors are from 0 to (LWIP_SOCKET_OFFSET-1). LWIP
     * file descriptors are registered from LWIP_SOCKET_OFFSET to
     * MAX_FDS-1.
     */

    ESP_ERROR_CHECK(esp_vfs_register_fd_range(&vfs, NULL, LWIP_SOCKET_OFFSET, MAX_FDS));
}
