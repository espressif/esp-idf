// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License


#include "esp_vfs_eventfd.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/select.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "soc/spinlock.h"

#define FD_INVALID -1
#define NUM_EVENT_FDS 5

typedef struct {
    int                     fd;
    bool                    support_isr;
    volatile bool           is_set;
    volatile uint64_t       value;
    fd_set                 *read_fds;
    fd_set                 *write_fds;
    fd_set                 *error_fds;
    esp_vfs_select_sem_t    signal_sem;
    _lock_t                 lock;
    spinlock_t              data_spin_lock; // only for event fds that support ISR.
} Event;

static Event s_events[NUM_EVENT_FDS];

static esp_err_t event_start_select(int                  nfds,
                                    fd_set              *readfds,
                                    fd_set              *writefds,
                                    fd_set              *exceptfds,
                                    esp_vfs_select_sem_t signal_sem,
                                    void               **end_select_args)
{
    esp_err_t error = ESP_OK;
    bool should_trigger = false;

    for (size_t i = 0; i < NUM_EVENT_FDS; i++) {
        _lock_acquire_recursive(&s_events[i].lock);
        int fd = s_events[i].fd;

        if (fd != FD_INVALID && fd < nfds) {
            if (s_events[i].support_isr) {
                portENTER_CRITICAL(&s_events[i].data_spin_lock);
            }
            s_events[i].signal_sem = signal_sem;
            s_events[i].error_fds = exceptfds;
            // event fds shouldn't report error
            FD_CLR(fd, exceptfds);

            // event fds are always writable
            if (FD_ISSET(fd, writefds)) {
                s_events[i].write_fds = writefds;
                should_trigger = true;
            }
            if (FD_ISSET(fd, readfds)) {
                s_events[i].read_fds = readfds;
                if (s_events[i].is_set) {
                    should_trigger = true;
                } else {
                    FD_CLR(fd, readfds);
                }
            }

            if (s_events[i].support_isr) {
                portEXIT_CRITICAL(&s_events[i].data_spin_lock);
            }

        }
        _lock_release_recursive(&s_events[i].lock);
    }

    if (should_trigger) {
        esp_vfs_select_triggered(signal_sem);
    }

    return error;
}

static esp_err_t event_end_select(void *end_select_args)
{
    for (size_t i = 0; i < NUM_EVENT_FDS; i++) {
        _lock_acquire_recursive(&s_events[i].lock);
        if (s_events[i].support_isr) {
            portENTER_CRITICAL(&s_events[i].data_spin_lock);
        }
        memset(&s_events[i].signal_sem, 0, sizeof(s_events[i].signal_sem));
        if (s_events[i].read_fds && s_events[i].is_set) {
            FD_SET(s_events[i].fd, s_events[i].read_fds);
            s_events[i].read_fds = NULL;
        }
        if (s_events[i].write_fds) {
            FD_SET(s_events[i].fd, s_events[i].write_fds);
            s_events[i].write_fds = NULL;
        }

        if (s_events[i].support_isr) {
            portEXIT_CRITICAL(&s_events[i].data_spin_lock);
        }
        _lock_release_recursive(&s_events[i].lock);
    }
    return ESP_OK;
}

static int event_open(const char *path, int flags, int mode)
{

    (void)flags;
    (void)mode;

    if (path == NULL || path[0] != '/') {
        return -1;
    }

    char *endPath;
    int fd = strtol(path + 1, &endPath, 10);

    if (endPath == NULL || *endPath != '\0' || fd >= NUM_EVENT_FDS) {
        return -1;
    }

    return fd;
}


static ssize_t signal_event_fd_from_isr(int fd, const void *data, size_t size)
{
    BaseType_t task_woken = pdFALSE;
    const uint64_t *val = (const uint64_t *)data;

    portENTER_CRITICAL_ISR(&s_events[fd].data_spin_lock);

    s_events[fd].is_set = true;
    s_events[fd].value += *val;
    if (s_events[fd].signal_sem.sem != NULL) {
        esp_vfs_select_triggered_isr(s_events[fd].signal_sem, &task_woken);
    }

    portEXIT_CRITICAL_ISR(&s_events[fd].data_spin_lock);

    if (task_woken) {
        portYIELD_FROM_ISR();
    }
    return size;
}

static ssize_t event_write(int fd, const void *data, size_t size)
{
    ssize_t ret = -1;

    if (fd >= NUM_EVENT_FDS || data == NULL || size != sizeof(uint64_t)) {
        errno = EINVAL;
        return ret;
    }
    if (size != sizeof(uint64_t)) {
        errno = EINVAL;
        return ret;
    }

    if (xPortInIsrContext()) {
        ret = signal_event_fd_from_isr(fd, data, size);
    } else {
        const uint64_t *val = (const uint64_t *)data;
        _lock_acquire_recursive(&s_events[fd].lock);

        if (s_events[fd].support_isr) {
            portENTER_CRITICAL(&s_events[fd].data_spin_lock);
        }
        s_events[fd].is_set = true;
        s_events[fd].value += *val;
        ret = size;
        if (s_events[fd].signal_sem.sem != NULL) {
            esp_vfs_select_triggered(s_events[fd].signal_sem);
        }
        if (s_events[fd].support_isr) {
            portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
        }

        _lock_release_recursive(&s_events[fd].lock);
    }
    return ret;
}

static ssize_t event_read(int fd, void *data, size_t size)
{
    ssize_t ret = -1;

    if (fd >= NUM_EVENT_FDS) {
        errno = EINVAL;
        return ret;
    }
    if (size != sizeof(uint64_t)) {
        errno = EINVAL;
        return ret;
    }

    _lock_acquire_recursive(&s_events[fd].lock);

    if (s_events[fd].fd == fd) {
        uint64_t *val = (uint64_t *)data;
        if (s_events[fd].support_isr) {
            portENTER_CRITICAL(&s_events[fd].data_spin_lock);
        }
        *val = s_events[fd].value;
        s_events[fd].is_set = false;
        ret = size;
        s_events[fd].value = 0;
        if (s_events[fd].support_isr) {
            portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
        }
    }

    _lock_release_recursive(&s_events[fd].lock);
    return ret;
}

static int event_close(int fd)
{
    int ret = -1;

    if (fd >= NUM_EVENT_FDS) {
        return ret;
    }

    _lock_acquire_recursive(&s_events[fd].lock);

    if (s_events[fd].fd == fd) {
        if (s_events[fd].support_isr) {
            portENTER_CRITICAL(&s_events[fd].data_spin_lock);
        }
        s_events[fd].fd = FD_INVALID;
        memset(&s_events[fd].signal_sem, 0, sizeof(s_events[fd].signal_sem));
        s_events[fd].value = 0;
        if (s_events[fd].support_isr) {
            portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
        }
        ret = 0;
    }

    _lock_release_recursive(&s_events[fd].lock);
    _lock_close_recursive(&s_events[fd].lock);
    return ret;
}

esp_err_t esp_vfs_eventfd_register(void)
{
    for (size_t i = 0; i < NUM_EVENT_FDS; i++) {
        s_events[i].fd = FD_INVALID;
    }

    esp_vfs_t vfs = {
        .flags        = ESP_VFS_FLAG_DEFAULT,
        .write        = &event_write,
        .open         = &event_open,
        .fstat        = NULL,
        .close        = &event_close,
        .read         = &event_read,
        .fcntl        = NULL,
        .fsync        = NULL,
        .access       = NULL,
        .start_select = &event_start_select,
        .end_select   = &event_end_select,
#ifdef CONFIG_SUPPORT_TERMIOS
        .tcsetattr = NULL,
        .tcgetattr = NULL,
        .tcdrain   = NULL,
        .tcflush   = NULL,
#endif // CONFIG_SUPPORT_TERMIOS
    };
    return esp_vfs_register(EVENT_VFS_PREFIX, &vfs, NULL);
}

esp_err_t esp_vfs_eventfd_unregister(void)
{
    return esp_vfs_unregister(EVENT_VFS_PREFIX);
}

int eventfd(unsigned int initval, int flags)
{
    int fd = FD_INVALID;

    if ((flags & (~EFD_SUPPORT_ISR)) != 0) {
        errno = EINVAL;
        return FD_INVALID;
    }

    for (size_t i = 0; i < NUM_EVENT_FDS; i++) {
        bool support_isr = flags & EFD_SUPPORT_ISR;
        bool has_allocated = false;

        _lock_init_recursive(&s_events[i].lock);
        _lock_acquire_recursive(&s_events[i].lock);
        if (s_events[i].fd == FD_INVALID) {
            s_events[i].fd = i;
            s_events[i].support_isr = support_isr;
            spinlock_initialize(&s_events[i].data_spin_lock);

            if (support_isr) {
                portENTER_CRITICAL(&s_events[i].data_spin_lock);
            }
            s_events[i].is_set = false;
            s_events[i].value = initval;
            memset(&s_events[i].signal_sem, 0, sizeof(s_events[i].signal_sem));
            if (support_isr) {
                portEXIT_CRITICAL(&s_events[i].data_spin_lock);
            }

            char fullpath[20];
            snprintf(fullpath, sizeof(fullpath), EVENT_VFS_PREFIX "/%d", s_events[i].fd);
            fd = open(fullpath, 0, 0);
            has_allocated = true;
        }
        _lock_release_recursive(&s_events[i].lock);

        if (has_allocated) {
            return fd;
        }
    }
    return FD_INVALID;
}
