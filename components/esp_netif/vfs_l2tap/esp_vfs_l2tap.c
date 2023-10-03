/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdatomic.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/queue.h>
#include "arpa/inet.h" // for ntohs, etc.
#include "errno.h"

#include "esp_vfs_l2tap.h"

#include "lwip/prot/ethernet.h" // Ethernet headers
#include "esp_vfs.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_eth_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"


#define INVALID_FD          (-1)

#define L2TAP_MAX_FDS       CONFIG_ESP_NETIF_L2_TAP_MAX_FDS
#define RX_QUEUE_MAX_SIZE   CONFIG_ESP_NETIF_L2_TAP_RX_QUEUE_SIZE

typedef enum {
    L2TAP_SOCK_STATE_READY,
    L2TAP_SOCK_STATE_OPENED,
    L2TAP_SOCK_STATE_CLOSING
} l2tap_socket_state_t;

typedef struct {
    _Atomic l2tap_socket_state_t state;
    bool non_blocking;
    l2tap_iodriver_handle driver_handle;
    uint16_t ethtype_filter;
    QueueHandle_t rx_queue;
    SemaphoreHandle_t close_done_sem;

    esp_err_t (*driver_transmit)(l2tap_iodriver_handle io_handle, void *buffer, size_t len);
    void (*driver_free_rx_buffer)(l2tap_iodriver_handle io_handle, void* buffer);
} l2tap_context_t;

typedef struct {
    void *buff;
    size_t len;
} frame_queue_entry_t;

typedef struct {
    esp_vfs_select_sem_t select_sem;
    fd_set *readfds;
    fd_set *writefds;
    fd_set *errorfds;
    fd_set readfds_orig;
    fd_set writefds_orig;
    fd_set errorfds_orig;
} l2tap_select_args_t;

typedef enum {
    L2TAP_SELECT_READ_NOTIF,
    L2TAP_SELECT_WRITE_NOTIF,
    L2TAP_SELECT_ERR_NOTIF
} l2tap_select_notif_e;

static l2tap_context_t s_l2tap_sockets[L2TAP_MAX_FDS] = {0};

static bool s_is_registered = false;

static portMUX_TYPE s_critical_section_lock = portMUX_INITIALIZER_UNLOCKED;

static l2tap_select_args_t **s_registered_selects = NULL;
static int32_t s_registered_select_cnt = 0;

static const char *TAG = "vfs_l2tap";

static void l2tap_select_notify(int fd, l2tap_select_notif_e select_notif);

/* ================== Utils ====================== */
static esp_err_t init_rx_queue(l2tap_context_t *l2tap_socket)
{
    l2tap_socket->rx_queue = xQueueCreate(RX_QUEUE_MAX_SIZE, sizeof(frame_queue_entry_t));
    ESP_RETURN_ON_FALSE(l2tap_socket->rx_queue, ESP_ERR_NO_MEM, TAG, "create work queue failed");
    return ESP_OK;
}

static esp_err_t push_rx_queue(l2tap_context_t *l2tap_socket, void *buff, size_t len)
{
    frame_queue_entry_t frame_info;

    frame_info.buff = buff;
    frame_info.len = len;
    // try send to queue and check if the queue is full
    if (xQueueSend(l2tap_socket->rx_queue, &frame_info, 0) != pdTRUE) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static ssize_t pop_rx_queue(l2tap_context_t *l2tap_socket, void *buff, size_t len)
{
    TickType_t timeout = portMAX_DELAY;
    if (l2tap_socket->non_blocking) {
        timeout = 0;
    }

    frame_queue_entry_t frame_info;
    if (xQueueReceive(l2tap_socket->rx_queue, &frame_info, timeout) == pdTRUE) {
        // empty queue was issued indicating the fd is going to be closed
        if (frame_info.len == 0) {
            // indicate to "clean_task" that task waiting for queue was unblocked
            push_rx_queue(l2tap_socket, NULL, 0);
            goto err;
        }

        if (len > frame_info.len) {
            len = frame_info.len;
        }
        memcpy(buff, frame_info.buff, len);
        l2tap_socket->driver_free_rx_buffer(l2tap_socket->driver_handle, frame_info.buff);
    } else {
        goto err;
    }

    return len;
err:
    return -1;
}

static bool rx_queue_empty(l2tap_context_t *l2tap_socket)
{
    return (uxQueueMessagesWaiting(l2tap_socket->rx_queue) == 0);
}

static void flush_rx_queue(l2tap_context_t *l2tap_socket)
{
    frame_queue_entry_t frame_info;
    while (xQueueReceive(l2tap_socket->rx_queue, &frame_info, 0) == pdTRUE) {
        if (frame_info.len > 0) {
            free(frame_info.buff);
        }
    }
}

static void delete_rx_queue(l2tap_context_t *l2tap_socket)
{
    vQueueDelete(l2tap_socket->rx_queue);
    l2tap_socket->rx_queue = NULL;
}

static inline void l2tap_lock(void)
{
    portENTER_CRITICAL(&s_critical_section_lock);
}

static inline void l2tap_unlock(void)
{
    portEXIT_CRITICAL(&s_critical_section_lock);
}

static inline void default_free_rx_buffer(l2tap_iodriver_handle io_handle, void* buffer)
{
    free(buffer);
}

/* ================== ESP NETIF L2 TAP intf ====================== */
esp_err_t esp_vfs_l2tap_eth_filter(l2tap_iodriver_handle driver_handle, void *buff, size_t *size)
{
    struct eth_hdr *eth_header = buff;
    uint16_t eth_type = ntohs(eth_header->type);

    for (int i = 0; i < L2TAP_MAX_FDS; i++) {
        if (atomic_load(&s_l2tap_sockets[i].state) == L2TAP_SOCK_STATE_OPENED) {
            l2tap_lock(); // read of socket config needs to be atomic since it can be manipulated from other task
            if (s_l2tap_sockets[i].driver_handle == driver_handle && (s_l2tap_sockets[i].ethtype_filter == eth_type ||
                    // IEEE 802.2 Frame is identified based on its length which is less than IEEE802.3 max length (Ethernet II Types IDs start over this value)
                    // Note that IEEE 802.2 LLC resolution is expected to be performed by upper stream app
                    (s_l2tap_sockets[i].ethtype_filter <= ETH_IEEE802_3_MAX_LEN && eth_type <= ETH_IEEE802_3_MAX_LEN))) {
                l2tap_unlock();
                if (push_rx_queue(&s_l2tap_sockets[i], buff, *size) != ESP_OK) {
                    // just tail drop when queue is full
                    s_l2tap_sockets[i].driver_free_rx_buffer(s_l2tap_sockets[i].driver_handle, buff);
                    ESP_LOGD(TAG, "fd %d rx queue is full", i);
                }
                l2tap_lock();
                if (s_registered_select_cnt) {
                    l2tap_select_notify(i, L2TAP_SELECT_READ_NOTIF);
                }
                l2tap_unlock();
                *size = 0; // the frame is not passed to IP stack when size set to 0
            } else {
                l2tap_unlock();
            }
        }
    }
    return ESP_OK;
}

/* ====================== vfs ====================== */
static int l2tap_open(const char *path, int flags, int mode)
{
    int fd;

    // Find free fd and initialize
    for (fd = 0; fd < L2TAP_MAX_FDS; fd++) {
        l2tap_socket_state_t exp_state = L2TAP_SOCK_STATE_READY;
        if (atomic_compare_exchange_strong(&s_l2tap_sockets[fd].state, &exp_state,
                                           L2TAP_SOCK_STATE_OPENED)) {
            if (init_rx_queue(&s_l2tap_sockets[fd]) != ESP_OK) {
                atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_READY);
                goto err;
            }
            s_l2tap_sockets[fd].ethtype_filter = 0x0;
            s_l2tap_sockets[fd].driver_handle = NULL;
            s_l2tap_sockets[fd].non_blocking = ((flags & O_NONBLOCK) == O_NONBLOCK);
            s_l2tap_sockets[fd].driver_transmit = esp_eth_transmit;
            s_l2tap_sockets[fd].driver_free_rx_buffer = default_free_rx_buffer;
            return fd;
        }
    }
err:
    return INVALID_FD;
}

static ssize_t l2tap_write(int fd, const void *data, size_t size)
{
    ssize_t ret = -1;

    if (size == 0) {
        return 0;
    }

    if (atomic_load(&s_l2tap_sockets[fd].state) == L2TAP_SOCK_STATE_OPENED) {
        if (s_l2tap_sockets[fd].ethtype_filter > ETH_IEEE802_3_MAX_LEN &&
                ((struct eth_hdr *)data)->type != htons(s_l2tap_sockets[fd].ethtype_filter)) {
            // bad message
            errno = EBADMSG;
            goto err;
        }

        if (s_l2tap_sockets[fd].driver_transmit(s_l2tap_sockets[fd].driver_handle, (void *)data, size) == ESP_OK) {
            ret = size;
        } else {
            // I/O error
            errno = EIO;
        }
    } else {
        // bad file desc
        errno = EBADF;
    }
err:
    return ret;
}

static ssize_t l2tap_read(int fd, void *data, size_t size)
{
    // fd might be in process of closing (close was already called but preempted)
    if (atomic_load(&s_l2tap_sockets[fd].state) != L2TAP_SOCK_STATE_OPENED) {
        // bad file desc
        errno = EBADF;
        return -1;
    }

    if (size == 0) {
        return 0;
    }

    ssize_t actual_size = -1;
    if ((actual_size = pop_rx_queue(&s_l2tap_sockets[fd], data, size)) < 0) {
        errno = EAGAIN;
    }

    return actual_size;
}

void l2tap_clean_task(void *task_param)
{
    l2tap_context_t *l2tap_socket = (l2tap_context_t *)task_param;

    // push empty queue to unblock possibly blocking task
    push_rx_queue(l2tap_socket, NULL, 0);
    // wait for the indication that blocking task was executed (unblocked)
    pop_rx_queue(l2tap_socket, NULL, 0);

    // now, all higher priority tasks should finished their execution and new accesses to the queue were prevended
    // by L2TAP_SOCK_STATE_CLOSING => we are free to free queue resources
    flush_rx_queue(l2tap_socket);
    delete_rx_queue(l2tap_socket);

    // unblock task which originally called close
    xSemaphoreGive(l2tap_socket->close_done_sem);

    // all done, delete itsefl
    vTaskDelete(NULL);
}

static int l2tap_close(int fd)
{
    if (atomic_load(&s_l2tap_sockets[fd].state) != L2TAP_SOCK_STATE_OPENED) {
        // not valid opened fd
        errno = EBADF;
        return -1;
    }

    // prevent any further manipulations with the socket (already started will be finished though)
    atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_CLOSING);

    if ((s_l2tap_sockets[fd].close_done_sem = xSemaphoreCreateBinary()) == NULL) {
        ESP_LOGE(TAG, "create close_done_sem failed");
        return -1;
    }
    // If one task is blocked in I/O operation and another task tries to close the fd, the first task is
    // unblocked by pushing empty queue in low priority task (to ensure context switch to the first task).
    // The first's task read operation then ends with error and the low priority task frees the queue resources.
    if (xTaskCreate(l2tap_clean_task, "l2tap_clean_task", 1024, &s_l2tap_sockets[fd], tskIDLE_PRIORITY, NULL) == pdFAIL) {
        ESP_LOGE(TAG, "create l2tap_clean_task failed");
        return -1;
    }

    // wait for the low priority close task & then delete the semaphore
    xSemaphoreTake(s_l2tap_sockets[fd].close_done_sem, portMAX_DELAY);
    vSemaphoreDelete(s_l2tap_sockets[fd].close_done_sem); // no worries to delete, this task owns the semaphore

    // indicate that socket is ready to be used again
    atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_READY);
    return 0;
}

// used to find a netif with the attached driver matching the argument
static bool netif_driver_matches(esp_netif_t *netif, void* driver)
{
    return esp_netif_get_io_driver(netif) == driver;
}

static int l2tap_ioctl(int fd, int cmd, va_list args)
{
    esp_netif_t *esp_netif;
    switch (cmd) {
    case L2TAP_S_RCV_FILTER: ;
        uint16_t *new_ethtype_filter = va_arg(args, uint16_t *);
        l2tap_lock();
        // socket needs to be assigned to interface at first
        if (s_l2tap_sockets[fd].driver_handle == NULL) {
            // Permission denied (filter change is denied at this state)
            errno = EACCES;
            l2tap_unlock();
            goto err;
        }
        // do nothing when same filter is to be set
        if (s_l2tap_sockets[fd].ethtype_filter != *new_ethtype_filter) {
            // check if the ethtype filter is not already used by other socket of the same interface
            for (int i = 0; i < L2TAP_MAX_FDS; i++) {
                if (atomic_load(&s_l2tap_sockets[i].state) == L2TAP_SOCK_STATE_OPENED &&
                        s_l2tap_sockets[i].driver_handle == s_l2tap_sockets[fd].driver_handle &&
                        s_l2tap_sockets[i].ethtype_filter == *new_ethtype_filter) {
                    // invalid argument
                    errno = EINVAL;
                    l2tap_unlock();
                    goto err;
                }
            }
            s_l2tap_sockets[fd].ethtype_filter = *new_ethtype_filter;
        }
        l2tap_unlock();
        break;
    case L2TAP_G_RCV_FILTER: ;
        uint16_t *ethtype_filter_dest = va_arg(args, uint16_t *);
        *ethtype_filter_dest = s_l2tap_sockets[fd].ethtype_filter;
        break;
    case L2TAP_S_INTF_DEVICE: ;
        const char *str = va_arg(args, const char *);
        esp_netif = esp_netif_get_handle_from_ifkey(str);
        if (esp_netif == NULL) {
            // No such device
            errno = ENODEV;
            goto err;
        }
        l2tap_lock();
        s_l2tap_sockets[fd].driver_handle = esp_netif_get_io_driver(esp_netif);
        l2tap_unlock();
        break;
    case L2TAP_G_INTF_DEVICE: ;
        const char **str_p = va_arg(args, const char **);
        *str_p = NULL;
        if ((esp_netif = esp_netif_find_if(netif_driver_matches, s_l2tap_sockets[fd].driver_handle)) != NULL) {
            *str_p = esp_netif_get_ifkey(esp_netif);
        }
        break;
    case L2TAP_S_DEVICE_DRV_HNDL: ;
        l2tap_iodriver_handle set_driver_hdl = va_arg(args, l2tap_iodriver_handle);
        if (set_driver_hdl == NULL) {
            // No such device (not valid driver handle)
            errno = ENODEV;
            goto err;
        }
        l2tap_lock();
        s_l2tap_sockets[fd].driver_handle = set_driver_hdl;
        l2tap_unlock();
        break;
    case L2TAP_G_DEVICE_DRV_HNDL: ;
        l2tap_iodriver_handle *get_driver_hdl = va_arg(args, l2tap_iodriver_handle*);
        *get_driver_hdl = s_l2tap_sockets[fd].driver_handle;
        break;
    default:
        // unsupported operation
        errno = ENOSYS;
        goto err;
        break;
    }
    va_end(args);
    return 0;
err:
    va_end(args);
    return -1;
}

static int l2tap_fcntl(int fd, int cmd, int arg)
{
    int result = 0;
    if (cmd == F_GETFL) {
        if (s_l2tap_sockets[fd].non_blocking) {
            result |= O_NONBLOCK;
        }
    } else if (cmd == F_SETFL) {
        s_l2tap_sockets[fd].non_blocking = (arg & O_NONBLOCK) != 0;
    } else {
        // unsupported operation
        result = -1;
        errno = ENOSYS;
    }
    return result;
}

#ifdef CONFIG_VFS_SUPPORT_SELECT

static esp_err_t register_select(l2tap_select_args_t *args)
{
    esp_err_t ret = ESP_ERR_INVALID_ARG;

    if (args) {
        const int new_size = s_registered_select_cnt + 1;
        l2tap_select_args_t **registered_selects_new;
        if ((registered_selects_new = realloc(s_registered_selects, new_size * sizeof(l2tap_select_args_t *))) == NULL) {
            ret = ESP_ERR_NO_MEM;
        } else {
            s_registered_selects = registered_selects_new;
            s_registered_selects[s_registered_select_cnt] = args;
            s_registered_select_cnt = new_size;
            ret = ESP_OK;
        }
    }

    return ret;
}

static esp_err_t unregister_select(l2tap_select_args_t *args)
{
    esp_err_t ret = ESP_OK;
    if (args) {
        ret = ESP_ERR_INVALID_STATE;
        for (int i = 0; i < s_registered_select_cnt; ++i) {
            if (s_registered_selects[i] == args) {
                const int new_size = s_registered_select_cnt - 1;
                // The item is removed by overwriting it with the last item. The subsequent rellocation will drop the
                // last item.
                s_registered_selects[i] = s_registered_selects[new_size];
                s_registered_selects = realloc(s_registered_selects, new_size * sizeof(l2tap_select_args_t *));
                if (s_registered_selects || new_size == 0) {
                    s_registered_select_cnt = new_size;
                    ret = ESP_OK;
                } else {
                    ret = ESP_ERR_NO_MEM;
                }
                break;
            }
        }
    }
    return ret;
}

static void l2tap_select_notify(int fd, l2tap_select_notif_e select_notif)
{
    for (int i = 0; i < s_registered_select_cnt; i++) {
        l2tap_select_args_t *args = s_registered_selects[i];
        if (args) {
            switch (select_notif) {
            case L2TAP_SELECT_READ_NOTIF:
                if (FD_ISSET(fd, &args->readfds_orig)) {
                    FD_SET(fd, args->readfds);
                    esp_vfs_select_triggered(args->select_sem);
                }
                break;
            case L2TAP_SELECT_WRITE_NOTIF:
                if (FD_ISSET(fd, &args->writefds_orig)) {
                    FD_SET(fd, args->writefds);
                    esp_vfs_select_triggered(args->select_sem);
                }
                break;
            case L2TAP_SELECT_ERR_NOTIF:
                if (FD_ISSET(fd, &args->errorfds_orig)) {
                    FD_SET(fd, args->errorfds);
                    esp_vfs_select_triggered(args->select_sem);
                }
                break;
            }
        }
    }
}

static esp_err_t l2tap_start_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                    esp_vfs_select_sem_t select_sem, void **end_select_args)
{
    const int max_fds = MIN(nfds, L2TAP_MAX_FDS);
    *end_select_args = NULL;

    l2tap_select_args_t *args = malloc(sizeof(l2tap_select_args_t));

    if (args == NULL) {
        return ESP_ERR_NO_MEM;
    }

    args->select_sem = select_sem;
    args->readfds = readfds;
    args->writefds = writefds;
    args->errorfds = exceptfds;
    args->readfds_orig = *readfds; // store the original values because they will be set to zero
    args->writefds_orig = *writefds;
    args->errorfds_orig = *exceptfds;
    FD_ZERO(readfds);
    FD_ZERO(writefds);
    FD_ZERO(exceptfds);

    l2tap_lock();

    for (int i = 0; i < max_fds; i++) {
        if (FD_ISSET(i, &args->readfds_orig)) {
            if (!rx_queue_empty(&s_l2tap_sockets[i])) {
                // signalize immediately when data is buffered
                FD_SET(i, readfds);
                esp_vfs_select_triggered(args->select_sem);
            }
        }
    }

    esp_err_t ret = register_select(args);
    if (ret != ESP_OK) {
        l2tap_unlock();
        free(args);
        return ret;
    }

    l2tap_unlock();

    *end_select_args = args;

    return ESP_OK;
}

static esp_err_t l2tap_end_select(void *end_select_args)
{
    l2tap_select_args_t *args = end_select_args;
    if (args == NULL) {
        return ESP_OK;
    }

    l2tap_lock();
    esp_err_t ret = unregister_select(args);
    l2tap_unlock();

    if (args) {
        free(args);
    }

    return ret;
}
#endif //CONFIG_VFS_SUPPORT_SELECT

esp_err_t esp_vfs_l2tap_intf_register(l2tap_vfs_config_t *config)
{
    l2tap_vfs_config_t def_config = L2TAP_VFS_CONFIG_DEFAULT();

    if (config == NULL) {
        ESP_LOGD(TAG, "vfs is to be registered with default settings");
        config = &def_config;
    }

    ESP_RETURN_ON_FALSE(!s_is_registered, ESP_ERR_INVALID_STATE, TAG, "vfs is already registered");
    s_is_registered = true;
    esp_vfs_t vfs = {
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &l2tap_write,
        .open = &l2tap_open,
        .close = &l2tap_close,
        .read = &l2tap_read,
        .fcntl = &l2tap_fcntl,
        .ioctl = &l2tap_ioctl,
#ifdef CONFIG_VFS_SUPPORT_SELECT
        .start_select = &l2tap_start_select,
        .end_select = &l2tap_end_select,
#endif // CONFIG_VFS_SUPPORT_SELECT
    };
    ESP_RETURN_ON_ERROR(esp_vfs_register(config->base_path, &vfs, NULL), TAG, "vfs register error");

    return ESP_OK;
}

esp_err_t esp_vfs_l2tap_intf_unregister(const char *base_path)
{
    for (int i = 0; i < L2TAP_MAX_FDS; i++) {
        ESP_RETURN_ON_FALSE(atomic_load(&s_l2tap_sockets[i].state) == L2TAP_SOCK_STATE_READY,
                                        ESP_ERR_INVALID_STATE, TAG, "all FDs need to be closed");
    }

    if (base_path == NULL) {
        ESP_RETURN_ON_ERROR(esp_vfs_unregister(L2TAP_VFS_DEFAULT_PATH), TAG, "vfs un-register error");
    } else {
        ESP_RETURN_ON_ERROR(esp_vfs_unregister(base_path), TAG, "vfs un-register error");
    }
    s_is_registered = false;

    return ESP_OK;
}
