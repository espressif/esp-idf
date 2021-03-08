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
#include <stdlib.h>
#include <string.h>
#include <sys/lock.h>
#include <sys/select.h>
#include <sys/types.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "soc/spinlock.h"

#define FD_INVALID -1
#define FD_PENDING_SELECT -2

typedef struct event_select_args_t {
    int                         fd;
    fd_set                      *read_fds;
    fd_set                      *error_fds;
    esp_vfs_select_sem_t        signal_sem;
    struct event_select_args_t  *prev_in_fd;
    struct event_select_args_t  *next_in_fd;
    struct event_select_args_t  *next_in_args;
} event_select_args_t;

typedef struct {
    int                     fd;
    bool                    support_isr;
    volatile bool           is_set;
    volatile uint64_t       value;
    event_select_args_t     *select_args;
    _lock_t                 lock;
    spinlock_t              data_spin_lock; // only for event fds that support ISR.
} event_context_t;

esp_vfs_id_t s_eventfd_vfs_id = -1;

static size_t s_event_size;
static event_context_t *s_events;

static void trigger_select_for_event(event_context_t *event)
{
    event_select_args_t *select_args = event->select_args;
    while (select_args != NULL) {
        esp_vfs_select_triggered(select_args->signal_sem);
        select_args = select_args->next_in_fd;
    }
}

static void trigger_select_for_event_isr(event_context_t *event, BaseType_t *task_woken)
{
    event_select_args_t *select_args = event->select_args;
    while (select_args != NULL) {
        BaseType_t local_woken;
        esp_vfs_select_triggered_isr(select_args->signal_sem, &local_woken);
        *task_woken = (local_woken || *task_woken);
        select_args = select_args->next_in_fd;
    }
}

static esp_err_t event_start_select(int                  nfds,
                                    fd_set              *readfds,
                                    fd_set              *writefds,
                                    fd_set              *exceptfds,
                                    esp_vfs_select_sem_t signal_sem,
                                    void               **end_select_args)
{
    esp_err_t error = ESP_OK;
    bool should_trigger = false;
    nfds = nfds < s_event_size ? nfds : (int)s_event_size;
    event_select_args_t *select_args_list = NULL;

    // FIXME: end_select_args should be a list to all select args

    for (int i = 0; i < nfds; i++) {
        _lock_acquire_recursive(&s_events[i].lock);
        if (s_events[i].fd == i) {
            if (s_events[i].support_isr) {
                portENTER_CRITICAL(&s_events[i].data_spin_lock);
            }

            event_select_args_t *event_select_args = (event_select_args_t *)malloc(sizeof(event_select_args_t));
            event_select_args->fd = i;
            event_select_args->signal_sem = signal_sem;

            if (FD_ISSET(i, exceptfds)) {
                FD_CLR(i, exceptfds);
                event_select_args->error_fds = exceptfds;
            } else {
                event_select_args->error_fds = NULL;
            }
            FD_CLR(i, exceptfds);
            // event fds are always writable
            if (FD_ISSET(i, writefds)) {
                should_trigger = true;
            }
            if (FD_ISSET(i, readfds)) {
                event_select_args->read_fds = readfds;
                if (s_events[i].is_set) {
                    should_trigger = true;
                } else {
                    FD_CLR(i, readfds);
                }
            } else {
                event_select_args->read_fds = NULL;
            }
            event_select_args->prev_in_fd = NULL;
            event_select_args->next_in_fd = s_events[i].select_args;
            if (s_events[i].select_args) {
                s_events[i].select_args->prev_in_fd = event_select_args;
            }
            event_select_args->next_in_args = select_args_list;
            select_args_list = event_select_args;
            s_events[i].select_args = event_select_args;

            if (s_events[i].support_isr) {
                portEXIT_CRITICAL(&s_events[i].data_spin_lock);
            }
        }
        _lock_release_recursive(&s_events[i].lock);
    }

    *end_select_args = select_args_list;

    if (should_trigger) {
        esp_vfs_select_triggered(signal_sem);
    }

    return error;
}

static esp_err_t event_end_select(void *end_select_args)
{
    event_select_args_t *select_args = (event_select_args_t *)end_select_args;

    while (select_args != NULL) {
        event_context_t *event = &s_events[select_args->fd];

        _lock_acquire_recursive(&event->lock);
        if (event->support_isr) {
            portENTER_CRITICAL(&event->data_spin_lock);
        }

        if (event->fd != select_args->fd) { // already closed
            if (select_args->error_fds) {
                FD_SET(select_args->fd, select_args->error_fds);
            }
        } else {
            if (select_args->read_fds && event->is_set) {
                FD_SET(select_args->fd, select_args->read_fds);
            }
        }

        event_select_args_t *prev_in_fd = select_args->prev_in_fd;
        event_select_args_t *next_in_fd = select_args->next_in_fd;
        event_select_args_t *next_in_args = select_args->next_in_args;
        if (prev_in_fd != NULL) {
            prev_in_fd->next_in_fd = next_in_fd;
        } else {
            event->select_args = next_in_fd;
        }
        if (next_in_fd != NULL) {
            next_in_fd->prev_in_fd = prev_in_fd;
        }
        if (prev_in_fd == NULL && next_in_fd == NULL) { // The last pending select
            if (event->fd == FD_PENDING_SELECT) {
                event->fd = FD_INVALID;
            }
        }

        if (event->support_isr) {
            portEXIT_CRITICAL(&event->data_spin_lock);
        }
        _lock_release_recursive(&event->lock);

        free(select_args);
        select_args = next_in_args;
    }

    return ESP_OK;
}

static ssize_t signal_event_fd_from_isr(int fd, const void *data, size_t size)
{
    BaseType_t task_woken = pdFALSE;
    const uint64_t *val = (const uint64_t *)data;
    ssize_t ret = size;

    portENTER_CRITICAL_ISR(&s_events[fd].data_spin_lock);

    if (s_events[fd].fd == fd) {
        s_events[fd].is_set = true;
        s_events[fd].value += *val;
        trigger_select_for_event_isr(&s_events[fd], &task_woken);
    } else {
        errno = EBADF;
        ret = -1;
    }

    portEXIT_CRITICAL_ISR(&s_events[fd].data_spin_lock);

    if (task_woken) {
        portYIELD_FROM_ISR();
    }
    return ret;
}

static ssize_t event_write(int fd, const void *data, size_t size)
{
    ssize_t ret = -1;

    if (fd >= s_event_size || data == NULL || size != sizeof(uint64_t)) {
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

        if (s_events[fd].fd == fd) {
            s_events[fd].is_set = true;
            s_events[fd].value += *val;
            ret = size;
            trigger_select_for_event(&s_events[fd]);

            if (s_events[fd].support_isr) {
                portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
            }
        } else {
            errno = EBADF;
            ret = -1;
        }
        _lock_release_recursive(&s_events[fd].lock);
    }
    return ret;
}

static ssize_t event_read(int fd, void *data, size_t size)
{
    ssize_t ret = -1;

    if (fd >= s_event_size || data == NULL || size != sizeof(uint64_t)) {
        errno = EINVAL;
        return ret;
    }

    uint64_t *val = (uint64_t *)data;

    _lock_acquire_recursive(&s_events[fd].lock);
    if (s_events[fd].support_isr) {
        portENTER_CRITICAL(&s_events[fd].data_spin_lock);
    }

    if (s_events[fd].fd == fd) {
        *val = s_events[fd].value;
        s_events[fd].is_set = false;
        ret = size;
        s_events[fd].value = 0;
    } else {
        errno = EBADF;
        ret = -1;
    }

    if (s_events[fd].support_isr) {
        portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
    }
    _lock_release_recursive(&s_events[fd].lock);

    return ret;
}

static int event_close(int fd)
{
    int ret = -1;

    if (fd >= s_event_size) {
        errno = EINVAL;
        return ret;
    }

    _lock_acquire_recursive(&s_events[fd].lock);
    if (s_events[fd].fd == fd) {
        if (s_events[fd].support_isr) {
            portENTER_CRITICAL(&s_events[fd].data_spin_lock);
        }
        if (s_events[fd].select_args == NULL) {
            s_events[fd].fd = FD_INVALID;
        } else {
            s_events[fd].fd = FD_PENDING_SELECT;
            trigger_select_for_event(&s_events[fd]);
        }
        s_events[fd].value = 0;
        if (s_events[fd].support_isr) {
            portEXIT_CRITICAL(&s_events[fd].data_spin_lock);
        }
        ret = 0;
    } else {
        errno = EBADF;
    }
    _lock_release_recursive(&s_events[fd].lock);

    return ret;
}

esp_err_t esp_vfs_eventfd_register(const esp_vfs_eventfd_config_t *config)
{
    if (config == NULL || config->eventfd_max_num >= MAX_FDS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_eventfd_vfs_id != -1) {
        return ESP_ERR_INVALID_STATE;
    }

    s_event_size = config->eventfd_max_num;
    s_events = (event_context_t *)calloc(s_event_size, sizeof(event_context_t));
    for (size_t i = 0; i < s_event_size; i++) {
        _lock_init_recursive(&s_events[i].lock);
        s_events[i].fd = FD_INVALID;
    }

    esp_vfs_t vfs = {
        .flags        = ESP_VFS_FLAG_DEFAULT,
        .write        = &event_write,
        .open         = NULL,
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
    return esp_vfs_register_with_id(&vfs, NULL, &s_eventfd_vfs_id);
}

esp_err_t esp_vfs_eventfd_unregister(void)
{
    if (s_eventfd_vfs_id == -1) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t error = esp_vfs_unregister_with_id(s_eventfd_vfs_id);
    if (error == ESP_OK) {
        s_eventfd_vfs_id = -1;
    }
    for (size_t i = 0; i < s_event_size; i++) {
        _lock_close_recursive(&s_events[i].lock);
    }
    free(s_events);
    return error;
}

int eventfd(unsigned int initval, int flags)
{
    int fd = FD_INVALID;
    int global_fd = FD_INVALID;
    esp_err_t error = ESP_OK;

    if ((flags & (~EFD_SUPPORT_ISR)) != 0) {
        errno = EINVAL;
        return FD_INVALID;
    }
    if (s_eventfd_vfs_id == -1) {
        errno = EACCES;
        return FD_INVALID;
    }

    for (size_t i = 0; i < s_event_size; i++) {
        _lock_acquire_recursive(&s_events[i].lock);
        if (s_events[i].fd == FD_INVALID) {

            error = esp_vfs_register_fd(s_eventfd_vfs_id, i, /*permanent=*/false, &global_fd);
            if (error != ESP_OK) {
                _lock_release_recursive(&s_events[i].lock);
                break;
            }

            bool support_isr = flags & EFD_SUPPORT_ISR;
            fd = i;
            s_events[i].fd = i;
            s_events[i].support_isr = support_isr;
            spinlock_initialize(&s_events[i].data_spin_lock);

            if (support_isr) {
                portENTER_CRITICAL(&s_events[i].data_spin_lock);
            }
            s_events[i].is_set = false;
            s_events[i].value = initval;
            s_events[i].select_args = NULL;
            if (support_isr) {
                portEXIT_CRITICAL(&s_events[i].data_spin_lock);
            }
            _lock_release_recursive(&s_events[i].lock);
            break;
        }
        _lock_release_recursive(&s_events[i].lock);
    }

    switch (error) {
    case ESP_OK:
        fd = global_fd;
        break;
    case ESP_ERR_NO_MEM:
        errno = ENOMEM;
        break;
    case ESP_ERR_INVALID_ARG:
        errno = EINVAL;
        break;
    default:
        errno = EIO;
        break;
    }

    return fd;
}
