/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "esp_vfs_pipe.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_default_fcntl.h>
#include <sys/_intsup.h>
#include <sys/lock.h>
#include <sys/select.h>
#include <sys/types.h>
#include "freertos/idf_additions.h"
#include "sys/queue.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#define FD_INVALID -1

#define VFS_PIPE_DEBUG(tag, fmt, ...)
//#define VFS_PIPE_DEBUG(tag, fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)


/*
 * This struct holds the arguments for a pending select() call on a pipe fd.
 * There can be one on the write part of the pipe, and one on the read part.
 * Its part of two linked lists:
 *  - a single linked list in end_select_args containing all pending selects in this select call
 *  - a double linked list in pipe_context_t::select_args containing all pending selects on this fd
 *
 */
typedef struct event_select_args_t {
    int                         local_fd; // Encoded file descriptor value (with WRITE_BIT); index in s_pipes is derived via FD_TO_IDX(local_fd)
    fd_set                      *read_fds; // This is where to set the fd if data is available
    fd_set                      *error_fds; // This is where to set the fd if an error occurs

    esp_vfs_select_sem_t        signal_sem;

    // linked list node in pipe_context_t::select_args
    LIST_ENTRY(event_select_args_t) in_fd;

    // linked list node in end_select_args
    SLIST_ENTRY(event_select_args_t) in_args;
} pipe_select_args_t;

// Offset to add to internal fd to get the fd returned by eventfd()
#define WRITE_BIT 0x1

#define IS_WRITE(fd) ((fd) & WRITE_BIT)
#define IS_READ(fd) (!IS_WRITE(fd))
#define FD_TO_IDX(fd) ((fd) >> 1)
#define IDX_TO_FD_READ(fd) (((fd) << 1) | (0 & WRITE_BIT))
#define IDX_TO_FD_WRITE(fd) (((fd) << 1) | (1 & WRITE_BIT))

typedef struct {
    int                     read_fd;
    int                     write_fd;
    int                     read_flags;
    int                     write_flags;
    const void *data;
    size_t data_size;
    // a double-linked list head for all pending select args with this fd
    LIST_HEAD(select_args_list, event_select_args_t) select_args;
    _lock_t                 lock;
    TaskHandle_t            writer_task;
    TaskHandle_t            reader_task;
} pipe_context_t;

esp_vfs_id_t s_pipe_vfs_id = -1;

static pipe_context_t *s_pipes;
static size_t s_pipes_size;

static void trigger_select_for_event(pipe_context_t *pipe)
{
    pipe_select_args_t *select_args;
    LIST_FOREACH(select_args, &pipe->select_args, in_fd) {
        esp_vfs_select_triggered(select_args->signal_sem);
    }
}

#ifdef CONFIG_VFS_SUPPORT_SELECT
static esp_err_t pipe_start_select(int                  nfds,
                                    fd_set              *readfds,
                                    fd_set              *writefds,
                                    fd_set              *exceptfds,
                                    esp_vfs_select_sem_t signal_sem,
                                    void               **end_select_args)
{
    esp_err_t error = ESP_OK;
    bool should_trigger_directly = false;
    nfds = nfds < s_pipes_size ? nfds : (int)s_pipes_size;

    // Use the first entry as the head of the list
    pipe_select_args_t *select_args_list = NULL;

    VFS_PIPE_DEBUG(TAG, "pipe_start_select(nfds=%d)", nfds);

    for (int local_fd = 0; local_fd < nfds; local_fd++) {
        size_t pipe_idx = FD_TO_IDX(local_fd);
        if (pipe_idx >= s_pipes_size) {
            continue;
        }
        if (!FD_ISSET(local_fd, readfds) &&
            !FD_ISSET(local_fd, writefds) &&
            !FD_ISSET(local_fd, exceptfds)) {
            continue;
        }

        pipe_context_t *pipe = &s_pipes[pipe_idx];
        _lock_acquire(&pipe->lock);

        // This is the context for this specific fd in this select() call
        pipe_select_args_t *event_select_args =
            (pipe_select_args_t *)calloc(1, sizeof(pipe_select_args_t));
        if (!event_select_args) {
            error = ESP_ERR_NO_MEM;
            _lock_release(&pipe->lock);
            break;
        }
        event_select_args->local_fd = local_fd;
        event_select_args->signal_sem = signal_sem;

        // Exceptions occurs if you wait for a pipe that is closed
        if (FD_ISSET(local_fd, exceptfds)) {
            event_select_args->error_fds = exceptfds; // Copy, so we can update later
            // If any sides of the pipe is closed, or we are waiting on the write pipe, we need to trigger directly
            if ((pipe->read_fd == FD_INVALID || pipe->write_fd == FD_INVALID) || IS_WRITE(local_fd)) {
                should_trigger_directly = true;
            }
        }


        // event fds are always writable, so trigger directly
        // marking writefd as ready.
        if (FD_ISSET(local_fd, writefds)) {
            should_trigger_directly = true;
        }

        // If we are waiting for read, check if there is data already available
        if (FD_ISSET(local_fd, readfds)) {
            event_select_args->read_fds = readfds; // Copy, so we can update later

            // If there is data already, or we are reading from the write end, we need to trigger directly
            if (pipe->data_size > 0 || IS_WRITE(local_fd)) {
                should_trigger_directly = true;
            }
        }

        // Insert into the per-fd linked list (LIST)
        // This is used for triggering the select when a write occur on this
        // event fd. This is so we know what to trigger when a write occurs
        LIST_INSERT_HEAD(&pipe->select_args, event_select_args, in_fd);

        // Insert into the per-select-call singly-linked list
        // Use the first entry as the head - simple pointer chaining
        SLIST_NEXT(event_select_args, in_args) = select_args_list;
        select_args_list = event_select_args;


        _lock_release(&pipe->lock);
    }

    if (error != ESP_OK) {
        // Cleanup any allocated select args
        pipe_select_args_t *select_args = select_args_list;
        while (select_args != NULL) {
            size_t pipe_idx = FD_TO_IDX(select_args->local_fd);
            if (pipe_idx < s_pipes_size) {
                pipe_context_t *pipe = &s_pipes[pipe_idx];
                _lock_acquire(&pipe->lock);
                // Remove from the per-fd list
                LIST_REMOVE(select_args, in_fd);
                _lock_release(&pipe->lock);
            }
            pipe_select_args_t *next = SLIST_NEXT(select_args, in_args);
            free(select_args);
            select_args = next;
        }
        select_args_list = NULL;
    }


    *end_select_args = select_args_list;

    if (should_trigger_directly) {
        esp_vfs_select_triggered(signal_sem);
    }

    return error;
}

static esp_err_t pipe_end_select(void *end_select_args)
{
    pipe_select_args_t *select_args = (pipe_select_args_t *)end_select_args;

    while (select_args != NULL) {
        size_t pipe_idx = FD_TO_IDX(select_args->local_fd);
        if (pipe_idx >= s_pipes_size) {
            continue;
        }
        VFS_PIPE_DEBUG(TAG, "pipe_end_select: ending select local_fd: %d pipe_idx: %zu is_write: %d", select_args->local_fd, pipe_idx, IS_WRITE(select_args->local_fd));
        pipe_context_t *pipe = &s_pipes[pipe_idx];

        _lock_acquire(&pipe->lock);
        VFS_PIPE_DEBUG(TAG, "pipe_end_select: pipe read_fd: %d write_fd: %d", pipe->read_fd, pipe->write_fd);

        if (select_args->error_fds) {
            // If any of the ends of the pipe is closed, set error
            if ((pipe->read_fd == FD_INVALID || pipe->write_fd == FD_INVALID) || IS_WRITE(select_args->local_fd)) {

                VFS_PIPE_DEBUG(TAG, "pipe_end_select: setting error fd local_fd: %d pipe_idx: %zu", select_args->local_fd, pipe_idx);
                FD_SET(select_args->local_fd, select_args->error_fds);
            } else {
                VFS_PIPE_DEBUG(TAG, "pipe_end_select: clearing error fd local_fd: %d pipe_idx: %zu", select_args->local_fd, pipe_idx);
                FD_CLR(select_args->local_fd, select_args->error_fds);
            }
        }

        // Select fd from the arg, as it might be closed now
        if (select_args->read_fds) {
            if (pipe->data_size > 0 || IS_WRITE(select_args->local_fd)) {

                VFS_PIPE_DEBUG(TAG, "pipe_end_select setting read fd local_fd: %d pipe_idx: %zu", select_args->local_fd, pipe_idx);
                FD_SET(select_args->local_fd, select_args->read_fds);
            } else {
                VFS_PIPE_DEBUG(TAG, "pipe_end_select clearing read fd local_fd: %d pipe_idx: %zu", select_args->local_fd, pipe_idx);
                FD_CLR(select_args->local_fd, select_args->read_fds);
            }
        }

        // Remove from the per-fd list
        LIST_REMOVE(select_args, in_fd);

        _lock_release(&pipe->lock);

        // Get next before freeing current
        pipe_select_args_t *next = SLIST_NEXT(select_args, in_args);
        free(select_args);
        select_args = next;
    }

    return ESP_OK;
}
#endif // CONFIG_VFS_SUPPORT_SELECT


// fd, will be the internal fd of the eventfd
static ssize_t pipe_write(int fd, const void *data, size_t size)
{

    if (!IS_WRITE(fd)) {
        errno = EBADF;
        return -1;
    }

    // get the internal fd, by removing the offset
    const size_t idx = FD_TO_IDX(fd);

    if (idx >= s_pipes_size || data == NULL) {
        errno = EINVAL;
        return -1;
    }

    // NOTE! This will cause deadlock if output goes to this pipe_write
    VFS_PIPE_DEBUG(TAG, "pipe_write(fd=%d, idx=%zu size=%zu)", fd, idx, size);

    pipe_context_t *pipe = &s_pipes[idx];
    _lock_acquire(&pipe->lock);

    if (pipe->read_fd == FD_INVALID || pipe->write_fd == FD_INVALID) {
        // Reader has closed the pipe
        errno = EPIPE;
        VFS_PIPE_DEBUG(TAG, "pipe_write: pipe closed");
        _lock_release(&pipe->lock);
        return -1;
    }

    if (pipe->writer_task != NULL) {
        // There is already a writer blocked on this pipe
        errno = EBUSY;
        VFS_PIPE_DEBUG(TAG, "pipe_write: pipe busy");
        _lock_release(&pipe->lock);
        return -1;
    }

    // If we run in non-blocking mode, we need a reader to stand by reading.
    if (pipe->write_flags & O_NONBLOCK && pipe->reader_task == NULL) {
        // No reader is waiting, and we are in non-blocking mode
        errno = EAGAIN;
        _lock_release(&pipe->lock);
        VFS_PIPE_DEBUG(TAG, "pipe_write: no reader");
        return -1;
    }

    pipe->data = data;
    pipe->data_size = size;
    pipe->writer_task = xTaskGetCurrentTaskHandle();

    if (pipe->reader_task) {
        // Notify the reader that data is available
        const TaskHandle_t reader_task = pipe->reader_task;
        pipe->reader_task = NULL;
        VFS_PIPE_DEBUG(TAG, "pipe_write: notifying reader xTaskNotifyGive");
        xTaskNotifyGive(reader_task);
    }

    // Wake up the select waiting for this pipe
    trigger_select_for_event(pipe);

    _lock_release(&pipe->lock);

    VFS_PIPE_DEBUG(TAG, "pipe_write: waiting for reader to read data ulTaskNotifyTake");

    // Block this task here, until the reader has read the data
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    VFS_PIPE_DEBUG(TAG, "pipe_write: data read completed");
    return size;
}

static ssize_t pipe_read(int fd, void *data, size_t size)
{
    if (!IS_READ(fd)) {
        errno = EBADF;
        VFS_PIPE_DEBUG(TAG, "pipe_read: bad file descriptor");
        return -1;
    }

    // get the internal fd, by removing the offset
    const size_t idx = FD_TO_IDX(fd);

    if (idx >= s_pipes_size || data == NULL) {
        errno = EINVAL;
        VFS_PIPE_DEBUG(TAG, "pipe_read: invalid argument");
        return -1;
    }

    VFS_PIPE_DEBUG(TAG, "pipe_read(fd=%d, idx=%zu size=%zu)", fd, idx, size);

    pipe_context_t *pipe = &s_pipes[idx];

    _lock_acquire(&pipe->lock);

pipe_read_restart:

    // Make sure socket is still open
    if (pipe->write_fd == FD_INVALID || pipe->read_fd == FD_INVALID) {
        // Writer has closed the pipe
        _lock_release(&pipe->lock);
        errno = EPIPE;
        VFS_PIPE_DEBUG(TAG, "pipe_read: writer has closed the pipe");
        return -1;
    }

    if (pipe->data_size == 0) {
        // No data available
        if (pipe->read_flags & O_NONBLOCK) {
            // Non-blocking mode, return EAGAIN
            _lock_release(&pipe->lock);
            errno = EAGAIN;
            VFS_PIPE_DEBUG(TAG, "pipe_read: no data available (non-blocking)");
            return -1;
        } else {
            // Blocking mode, wait for data to be written
            pipe->reader_task = xTaskGetCurrentTaskHandle();
            _lock_release(&pipe->lock);
            VFS_PIPE_DEBUG(TAG, "pipe_read: waiting for data to be written ulTaskNotifyTake");
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
            _lock_acquire(&pipe->lock);
            pipe->reader_task = NULL;

            // Things might have changed while we were blocked, so restart all checks
            goto pipe_read_restart;
        }
    }

    ssize_t ret = (pipe->data_size < size) ? pipe->data_size : size;
    memcpy(data, pipe->data, ret);

    // Move the pointer offset forward as we read.
    pipe->data += ret;
    pipe->data_size -= ret;

    if (pipe->data_size == 0) {
        pipe->data = NULL;
        // Notify the writer that the data has been read
        if (pipe->writer_task) {
            VFS_PIPE_DEBUG(TAG, "pipe_read: notifying writer xTaskNotifyGive");
            const TaskHandle_t writer_task = pipe->writer_task;
            pipe->writer_task = NULL;
            xTaskNotifyGive(writer_task);
        }
    }

    VFS_PIPE_DEBUG(TAG, "pipe_read: data read %zu bytes completed, %zu bytes left", ret, pipe->data_size);
    _lock_release(&pipe->lock);

    return ret;
}

static int pipe_fcntl(int fd, int cmd, int flags)
{

    // get the internal fd, by removing the offset
    const size_t idx = FD_TO_IDX(fd);

    if (idx >= s_pipes_size) {
        errno = EINVAL;
        return -1;
    }

    VFS_PIPE_DEBUG(TAG, "pipe_fcntl(fd=%d, idx=%zu flags=%d)", fd, idx, flags);
    pipe_context_t *pipe = &s_pipes[idx];
    _lock_acquire(&pipe->lock);

    int res = -1;
    if (cmd == F_GETFL) {
        if (IS_READ(fd)) {
            res = pipe->read_flags;
        } else if (IS_WRITE(fd)) {
            res = pipe->write_flags;
        } else {
            errno = EINVAL;
        }
    } else if (cmd == F_SETFL && (flags & O_NONBLOCK))  {
        if (IS_READ(fd)) {
            pipe->read_flags |= O_NONBLOCK;
            res = 0;
        } else {
            pipe->write_flags |= O_NONBLOCK;
            res = 0;
        }
    } else if (cmd == F_SETFL && !(flags & O_NONBLOCK)) {
        if (IS_READ(fd)) {
            pipe->read_flags &= ~O_NONBLOCK;
            res = 0;
        } else {
            pipe->write_flags &= ~O_NONBLOCK;
            res = 0;
        }
    } else {
        errno = EINVAL;
    }

    _lock_release(&pipe->lock);
    return res;
}

static int pipe_close(int fd)
{

    // get the internal fd, by removing the offset
    const size_t idx = FD_TO_IDX(fd);

    if (idx >= s_pipes_size) {
        errno = EINVAL;
        return -1;
    }

    VFS_PIPE_DEBUG(TAG, "pipe_close(fd=%d, idx=%zu)", fd, idx);

    pipe_context_t *pipe = &s_pipes[idx];
    _lock_acquire(&pipe->lock);

    if (IS_WRITE(fd)) {
        pipe->write_fd = FD_INVALID;

    } else {
        pipe->read_fd = FD_INVALID;
    }

    if (pipe->writer_task) {
        // Notify the writer that the pipe is closed
        const TaskHandle_t writer_task = pipe->writer_task;
        pipe->writer_task = NULL;
        xTaskNotifyGive(writer_task);
    }
    if (pipe->reader_task) {
        // Notify the reader that the pipe is closed
        const TaskHandle_t reader_task = pipe->reader_task;
        pipe->reader_task = NULL;
        xTaskNotifyGive(reader_task);
    }


    trigger_select_for_event(pipe);

    _lock_release(&pipe->lock);

    return 0;
}

esp_err_t esp_vfs_pipe_register(const esp_vfs_pipe_config_t *config)
{
    if (config == NULL || config->max_fds >= MAX_FDS) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_pipe_vfs_id != -1) {
        return ESP_ERR_INVALID_STATE;
    }

    s_pipes_size = config->max_fds;
    s_pipes = (pipe_context_t *)calloc(s_pipes_size, sizeof(pipe_context_t));
    for (size_t i = 0; i < s_pipes_size; i++) {
        _lock_init(&s_pipes[i].lock);
        s_pipes[i].read_fd = FD_INVALID;
        s_pipes[i].write_fd = FD_INVALID;
    }

    esp_vfs_t vfs = {
        .flags        = ESP_VFS_FLAG_DEFAULT,
        .write        = &pipe_write,
        .close        = &pipe_close,
        .read         = &pipe_read,
        .fcntl        = &pipe_fcntl,
#ifdef CONFIG_VFS_SUPPORT_SELECT
        .start_select = &pipe_start_select,
        .end_select   = &pipe_end_select,
#endif
    };
    return esp_vfs_register_with_id(&vfs, NULL, &s_pipe_vfs_id);
}

esp_err_t esp_vfs_pipe_unregister(void)
{
    if (s_pipe_vfs_id == -1) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t error = esp_vfs_unregister_with_id(s_pipe_vfs_id);
    if (error == ESP_OK) {
        s_pipe_vfs_id = -1;
    }
    for (size_t i = 0; i < s_pipes_size; i++) {
        _lock_close(&s_pipes[i].lock);
    }
    free(s_pipes);
    return error;
}

int pipe(int fildes[2])
{
    fildes[0] = FD_INVALID;
    fildes[1] = FD_INVALID;

    int global_read_fd = FD_INVALID;
    int global_write_fd = FD_INVALID;
    esp_err_t error = ESP_OK;

    if (s_pipe_vfs_id == -1) {
        errno = EACCES;
        return FD_INVALID;
    }

    for (size_t i = 0; i < s_pipes_size; i++) {
        _lock_acquire(&s_pipes[i].lock);

        // Make sure both ends of the pipe are free
        if (s_pipes[i].read_fd == FD_INVALID && s_pipes[i].write_fd == FD_INVALID) {

            error = esp_vfs_register_fd_with_local_fd(s_pipe_vfs_id, IDX_TO_FD_READ(i), false, &global_read_fd);
            if (error != ESP_OK) {
                _lock_release(&s_pipes[i].lock);
                break;
            }
            error = esp_vfs_register_fd_with_local_fd(s_pipe_vfs_id, IDX_TO_FD_WRITE(i), false, &global_write_fd);
            if (error != ESP_OK) {
                // Cleanup the read fd registration on failure
                esp_vfs_unregister_fd(s_pipe_vfs_id, global_read_fd);
                _lock_release(&s_pipes[i].lock);
                break;
            }
            VFS_PIPE_DEBUG(TAG, "pipe() -> read fd=%d (%d), write fd=%d (%d)", global_read_fd, IDX_TO_FD_READ(i), global_write_fd, IDX_TO_FD_WRITE(i));

            // Mark pipe as used BEFORE setting the global fd values
            // This prevents another thread from grabbing this slot
            s_pipes[i].read_fd = global_read_fd;
            s_pipes[i].write_fd = global_write_fd;

            fildes[0] = global_read_fd;
            fildes[1] = global_write_fd;
            s_pipes[i].data = NULL;
            s_pipes[i].data_size = 0;
            LIST_INIT(&s_pipes[i].select_args);
            s_pipes[i].writer_task = NULL;
            s_pipes[i].reader_task = NULL;
            s_pipes[i].read_flags = O_RDONLY;
            s_pipes[i].write_flags = O_WRONLY;
            _lock_release(&s_pipes[i].lock);
            break;
        }
        _lock_release(&s_pipes[i].lock);
    }

    switch (error) {
    case ESP_OK:
        errno = 0;
        return 0;
    case ESP_ERR_NO_MEM:
        errno = ENOMEM;
        return -1;
    case ESP_ERR_INVALID_ARG:
        errno = EINVAL;
        return -1;
    default:
        errno = EIO;
        return -1;
    }
}
