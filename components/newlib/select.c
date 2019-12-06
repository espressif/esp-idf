// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <sys/select.h>
#include "esp_vfs.h"
#include "sdkconfig.h"

#ifdef CONFIG_LWIP_USE_ONLY_LWIP_SELECT
#include "lwip/sockets.h"

#ifdef CONFIG_VFS_SUPPRESS_SELECT_DEBUG_OUTPUT
#define LOG_LOCAL_LEVEL ESP_LOG_NONE
#endif //CONFIG_VFS_SUPPRESS_SELECT_DEBUG_OUTPUT
#include "esp_log.h"

static const char *TAG = "newlib_select";

static void log_fd_set(const char *fds_name, const fd_set *fds)
{
    if (fds_name && fds) {
        ESP_LOGD(TAG, "FDs in %s =", fds_name);
        for (int i = 0; i < MAX_FDS; ++i) {
            if (FD_ISSET(i, fds)) {
                ESP_LOGD(TAG, "%d", i);
            }
        }
    }
}
#endif //CONFIG_LWIP_USE_ONLY_LWIP_SELECT

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
#ifdef CONFIG_LWIP_USE_ONLY_LWIP_SELECT
    ESP_LOGD(TAG, "lwip_select starts with nfds = %d", nfds);
    if (timeout) {
        ESP_LOGD(TAG, "timeout is %lds + %ldus", timeout->tv_sec, timeout->tv_usec);
    }
    log_fd_set("readfds", readfds);
    log_fd_set("writefds", writefds);
    log_fd_set("errorfds", errorfds);

    int ret = lwip_select(nfds, readfds, writefds, errorfds, timeout);

    ESP_LOGD(TAG, "lwip_select returns %d", ret);
    log_fd_set("readfds", readfds);
    log_fd_set("writefds", writefds);
    log_fd_set("errorfds", errorfds);

    return ret;
#else
    return esp_vfs_select(nfds, readfds, writefds, errorfds, timeout);
#endif
}
