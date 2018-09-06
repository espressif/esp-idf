// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "esp_attr.h"
#include "soc/uart_struct.h"
#include "lwip/sockets.h"
#include "sdkconfig.h"
#include "lwip/sys.h"

_Static_assert(MAX_FDS >= CONFIG_LWIP_MAX_SOCKETS, "MAX_FDS < CONFIG_LWIP_MAX_SOCKETS");

static void lwip_stop_socket_select()
{
    sys_sem_signal(sys_thread_sem_get()); //socket_select will return
}

static void lwip_stop_socket_select_isr(BaseType_t *woken)
{
    if (sys_sem_signal_isr(sys_thread_sem_get()) && woken) {
        *woken = pdTRUE;
    }
}

static int lwip_fcntl_r_wrapper(int fd, int cmd, va_list args)
{
    return lwip_fcntl_r(fd, cmd, va_arg(args, int));
}

static int lwip_ioctl_r_wrapper(int fd, int cmd, va_list args)
{
    return lwip_ioctl_r(fd, cmd, va_arg(args, void *));
}

void esp_vfs_lwip_sockets_register()
{
    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &lwip_write_r,
        .open = NULL,
        .fstat = NULL,
        .close = &lwip_close_r,
        .read = &lwip_read_r,
        .fcntl = &lwip_fcntl_r_wrapper,
        .ioctl = &lwip_ioctl_r_wrapper,
        .socket_select = &lwip_select,
        .stop_socket_select = &lwip_stop_socket_select,
        .stop_socket_select_isr = &lwip_stop_socket_select_isr,
    };
    /* Non-LWIP file descriptors are from 0 to (LWIP_SOCKET_OFFSET-1). LWIP
     * file descriptors are registered from LWIP_SOCKET_OFFSET to
     * MAX_FDS-1.
     */

    ESP_ERROR_CHECK(esp_vfs_register_fd_range(&vfs, NULL, LWIP_SOCKET_OFFSET, MAX_FDS));
}
