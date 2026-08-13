/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <time.h>
#include "arpa/inet.h" // for ntohs, etc.
#include "errno.h"

#include "esp_vfs_l2tap.h"

#include "lwip/prot/ethernet.h" // Ethernet headers
#include "esp_vfs.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_netif.h"
#include "esp_eth_driver.h"
#include "esp_private/esp_eth_sublayer_iodriver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#define INVALID_FD          (-1)

#define L2TAP_MAX_FDS       CONFIG_ESP_NETIF_L2_TAP_MAX_FDS
#define RX_QUEUE_MAX_SIZE   CONFIG_ESP_NETIF_L2_TAP_RX_QUEUE_SIZE

typedef enum {
    L2TAP_SOCK_STATE_READY,
    L2TAP_SOCK_STATE_OPENING,
    L2TAP_SOCK_STATE_OPENED,
    L2TAP_SOCK_STATE_CLOSING
} l2tap_socket_state_t;

typedef enum {
    L2TAP_FLAG_NON_BLOCK = BIT(0),
    L2TAP_FLAG_TS        = BIT(1)
} l2tap_socket_flags_t;

typedef struct {
    _Atomic l2tap_socket_state_t state;
    l2tap_socket_flags_t flags;
    l2tap_iodriver_handle driver_handle;
    uint16_t ethtype_filter;
    QueueHandle_t rx_queue;

    SemaphoreHandle_t close_done_sem;

    // Sublayer transmit functions
    esp_err_t (*iodriver_transmit)(l2tap_iodriver_handle io_handle, void *buf, size_t len);
    esp_err_t (*iodriver_transmit_wrap)(l2tap_iodriver_handle io_handle, void *buf, size_t len, void *eb);
    void (*iodriver_free_rx_buffer)(l2tap_iodriver_handle io_handle, void *buffer);
    esp_err_t (*iodriver_get_ll_driver)(l2tap_iodriver_handle io_handle, void **ll_driver);

    // TODO create JIRA to remove this along with Ethernet glue or update glue to have `get_io_fns`
    // Direct Ethernet Driver transmit functions (legacy mode)
    union {
        esp_err_t (*eth_transmit)(l2tap_iodriver_handle io_handle, void *buffer, size_t len);
        esp_err_t (*eth_transmit_ctrl_bufs)(l2tap_iodriver_handle io_handle, void *ctrl, const esp_eth_buf_desc_t *bufs, size_t buf_count);
    };
    void (*driver_free_rx_buffer)(l2tap_iodriver_handle io_handle, void* buffer);
} l2tap_context_t;

typedef struct {
    void *buff;
    size_t len;
    void *l2_buff;
    l2tap_timestamp_t ts;
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

typedef struct l2tap_provider_node {
    l2tap_iodriver_provider_handle handle;
    SLIST_ENTRY(l2tap_provider_node) next;
} l2tap_provider_node_t;

static SLIST_HEAD(l2tap_provider_list, l2tap_provider_node) s_provider_list =
    SLIST_HEAD_INITIALIZER(s_provider_list);

static const char *TAG = "vfs_l2tap";

static void l2tap_select_notify(int fd, l2tap_select_notif_e select_notif);

/* ================== Utils ====================== */
static inline void default_free_rx_buffer(l2tap_iodriver_handle io_handle, void* buffer)
{
    free(buffer);
}

static void l2tap_free_rx_buffer(l2tap_context_t *l2tap_socket, void *buffer)
{
    if (l2tap_socket->iodriver_free_rx_buffer != NULL) {
        l2tap_socket->iodriver_free_rx_buffer(l2tap_socket->driver_handle, buffer);
        return;
    }
    l2tap_socket->driver_free_rx_buffer(l2tap_socket->driver_handle, buffer);
}

static esp_err_t init_rx_queue(l2tap_context_t *l2tap_socket)
{
    l2tap_socket->rx_queue = xQueueCreate(RX_QUEUE_MAX_SIZE, sizeof(frame_queue_entry_t));
    ESP_RETURN_ON_FALSE(l2tap_socket->rx_queue, ESP_ERR_NO_MEM, TAG, "create Rx queue failed");
    return ESP_OK;
}

static esp_err_t push_rx_queue(l2tap_context_t *l2tap_socket, void *buff, size_t len, void *l2_buff, l2tap_timestamp_t *ts)
{
    frame_queue_entry_t rx_frame_info = {0};

    rx_frame_info.buff = buff;
    rx_frame_info.len = len;
    rx_frame_info.l2_buff = l2_buff;
    if (ts) {
        rx_frame_info.ts = *ts;
    }
    // try send to queue and check if the queue is full
    if (xQueueSend(l2tap_socket->rx_queue, &rx_frame_info, 0) != pdTRUE) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static esp_err_t pop_rx_queue(l2tap_context_t *l2tap_socket, void *buff, size_t len, ssize_t *copy_len)
{
    uint8_t *copy_buff;
    TickType_t timeout = portMAX_DELAY;
    if (l2tap_socket->flags & L2TAP_FLAG_NON_BLOCK) {
        timeout = 0;
    }
    *copy_len = -1;

    frame_queue_entry_t rx_frame_info;
    if (xQueueReceive(l2tap_socket->rx_queue, &rx_frame_info, timeout) == pdTRUE) {
        // empty queue was issued indicating the fd is going to be closed
        if (rx_frame_info.len == 0) {
            // indicate to "clean_task" that task waiting for queue was unblocked
            push_rx_queue(l2tap_socket, NULL, 0, NULL, NULL);
            *copy_len = 0;
            return ESP_OK;
        }

        // when len == 0, extended buffer is going to be used
        if (len == 0) {
            l2tap_extended_buff_t *ext_buff = (l2tap_extended_buff_t *)buff;
            copy_buff = ext_buff->buff;
            if (ext_buff->buff_len > rx_frame_info.len) {
                *copy_len = rx_frame_info.len;
            } else {
                *copy_len = ext_buff->buff_len;
            }
            ext_buff->buff_len = *copy_len;
            // check if fd has TS enabled
            if (l2tap_socket->flags & L2TAP_FLAG_TS) {
                // find the record allocated for the time stamp info
                l2tap_irec_hdr_t *info_rec = L2TAP_IREC_FIRST(ext_buff);
                while(info_rec != NULL) {
                    if (info_rec->type == L2TAP_IREC_TIME_STAMP) {
                        break;
                    }
                    info_rec = L2TAP_IREC_NEXT(ext_buff, info_rec);
                }
                if (info_rec != NULL) {
                    // check if there is enough space to store TS
                    if (info_rec->len - sizeof(l2tap_irec_hdr_t) >= sizeof(struct timespec)) {
                        struct timespec *ts = (struct timespec *)info_rec->data;
                        ts->tv_sec = rx_frame_info.ts.sec;
                        ts->tv_nsec = rx_frame_info.ts.nsec;
                    } else {
                        info_rec->type = L2TAP_IREC_INVALID;
                    }
                }
            }
        } else {
            copy_buff = buff;
            if (len > rx_frame_info.len) {
                *copy_len = rx_frame_info.len;
            } else {
                *copy_len = len;
            }
        }
        memcpy(copy_buff, rx_frame_info.buff, *copy_len);
        l2tap_free_rx_buffer(l2tap_socket, rx_frame_info.l2_buff != NULL ? rx_frame_info.l2_buff : rx_frame_info.buff);
    } else {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

static bool rx_queue_empty(l2tap_context_t *l2tap_socket)
{
    return (uxQueueMessagesWaiting(l2tap_socket->rx_queue) == 0);
}

static void flush_rx_queue(l2tap_context_t *l2tap_socket)
{
    frame_queue_entry_t rx_frame_info;
    while (xQueueReceive(l2tap_socket->rx_queue, &rx_frame_info, 0) == pdTRUE) {
        if (rx_frame_info.len > 0) {
            l2tap_free_rx_buffer(l2tap_socket, rx_frame_info.l2_buff != NULL ? rx_frame_info.l2_buff : rx_frame_info.buff);
        }
    }
}

static void delete_rx_queue(l2tap_context_t *l2tap_socket)
{
    vQueueDelete(l2tap_socket->rx_queue);
    l2tap_socket->rx_queue = NULL;
}

static inline void l2tap_enter_critical(void)
{
    portENTER_CRITICAL(&s_critical_section_lock);
}

static inline void l2tap_exit_critical(void)
{
    portEXIT_CRITICAL(&s_critical_section_lock);
}

static esp_err_t l2tap_transmit(l2tap_context_t *l2tap_socket, void *data, size_t size)
{
    if (l2tap_socket->iodriver_transmit != NULL) {
        return l2tap_socket->iodriver_transmit(l2tap_socket->driver_handle, data, size);
    }
    if (l2tap_socket->eth_transmit != NULL) {
        return l2tap_socket->eth_transmit(l2tap_socket->driver_handle, data, size);
    }
    return ESP_ERR_INVALID_STATE;
}

static esp_err_t l2tap_transmit_ts(l2tap_context_t *l2tap_socket, void *data, size_t size, l2tap_timestamp_t *hw_ts)
{
    if (l2tap_socket->iodriver_transmit_wrap != NULL) {
        return l2tap_socket->iodriver_transmit_wrap(l2tap_socket->driver_handle, data, size, hw_ts);
    }
    if (l2tap_socket->eth_transmit_ctrl_bufs != NULL) {
        const esp_eth_buf_desc_t bufs[] = {
            { .buf = (uint8_t *)data, .len = size },
        };
        return l2tap_socket->eth_transmit_ctrl_bufs(l2tap_socket->driver_handle, hw_ts, bufs, 1);
    }
    return ESP_ERR_INVALID_STATE;
}


/* ================== ESP NETIF L2 TAP intf ====================== */
esp_err_t esp_vfs_l2tap_eth_filter_frame(l2tap_iodriver_handle driver_handle, void *buff, size_t *size, l2tap_eth_filter_info_t *info)
{
    struct eth_hdr *eth_header = buff;
    uint16_t eth_type = ntohs(eth_header->type);

    for (int i = 0; i < L2TAP_MAX_FDS; i++) {
        if (atomic_load(&s_l2tap_sockets[i].state) == L2TAP_SOCK_STATE_OPENED) {
            l2tap_enter_critical(); // read of socket config needs to be atomic since it can be manipulated from other task
            if (s_l2tap_sockets[i].driver_handle == driver_handle && (s_l2tap_sockets[i].ethtype_filter == eth_type ||
                    // IEEE 802.2 Frame is identified based on its length which is less than IEEE802.3 max length (Ethernet II Types IDs start over this value)
                    // Note that IEEE 802.2 LLC resolution is expected to be performed by upper stream app
                    (s_l2tap_sockets[i].ethtype_filter <= ETH_IEEE802_3_MAX_LEN && eth_type <= ETH_IEEE802_3_MAX_LEN))) {
                l2tap_exit_critical();
                l2tap_timestamp_t *ts = NULL;
                void *l2_buffer = NULL;
                if (info != NULL) {
                    if (s_l2tap_sockets[i].flags & L2TAP_FLAG_TS) {
                        ts = info->hw_ts;
                    }
                    l2_buffer = info->l2_buffer;
                }
                if (push_rx_queue(&s_l2tap_sockets[i], buff, *size, l2_buffer, ts) != ESP_OK) {
                    // just tail drop when queue is full
                    l2tap_free_rx_buffer(&s_l2tap_sockets[i], l2_buffer != NULL ? l2_buffer : buff);
                    ESP_LOGD(TAG, "fd %d rx queue is full", i);
                }
                l2tap_enter_critical();
                if (s_registered_select_cnt) {
                    l2tap_select_notify(i, L2TAP_SELECT_READ_NOTIF);
                }
                l2tap_exit_critical();
                *size = 0; // the frame is not passed to IP stack when size set to 0
            } else {
                l2tap_exit_critical();
            }
        }
    }
    return ESP_OK;
}

/* ====================== vfs ====================== */
static int l2tap_open(__attribute__((unused)) void *ctx, const char *path, int flags, int mode)
{
    int fd;

    // Find free fd and initialize
    for (fd = 0; fd < L2TAP_MAX_FDS; fd++) {
        l2tap_socket_state_t exp_state = L2TAP_SOCK_STATE_READY;
        if (atomic_compare_exchange_strong(&s_l2tap_sockets[fd].state, &exp_state,
                                           L2TAP_SOCK_STATE_OPENING)) {
            if (init_rx_queue(&s_l2tap_sockets[fd]) != ESP_OK) {
                goto err;
            }
            s_l2tap_sockets[fd].ethtype_filter = 0x0;
            s_l2tap_sockets[fd].flags = 0;
            s_l2tap_sockets[fd].driver_handle = NULL;
            s_l2tap_sockets[fd].iodriver_transmit = NULL;
            s_l2tap_sockets[fd].iodriver_transmit_wrap = NULL;
            s_l2tap_sockets[fd].iodriver_free_rx_buffer = NULL;
            s_l2tap_sockets[fd].iodriver_get_ll_driver = NULL;
            s_l2tap_sockets[fd].flags |= ((flags & O_NONBLOCK) == O_NONBLOCK) ? L2TAP_FLAG_NON_BLOCK : 0;
            s_l2tap_sockets[fd].eth_transmit = esp_eth_transmit;
            s_l2tap_sockets[fd].driver_free_rx_buffer = default_free_rx_buffer;
            atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_OPENED);
            return fd;
        }
    }
err:
    if (fd < L2TAP_MAX_FDS) {
        if (s_l2tap_sockets[fd].rx_queue) {
            delete_rx_queue(&s_l2tap_sockets[fd]);
        }
        atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_READY);
    }
    return INVALID_FD;
}

static int l2tap_tx_esp_err_to_errno(esp_err_t esp_err)
{
    switch(esp_err) {
        case ESP_ERR_INVALID_ARG:
            return EINVAL;
        case ESP_ERR_TIMEOUT:
            return EBUSY;
        case ESP_ERR_NO_MEM:
            return ENOBUFS;
        case ESP_ERR_INVALID_STATE:
        // fall through
        default:
            return EIO;
    }
}

static ssize_t l2tap_write(__attribute__((unused)) void *ctx, int fd, const void *data, size_t size)
{
    void *eth_buff;
    l2tap_extended_buff_t *ext_buff;
    ssize_t ret = -1;
    esp_err_t esp_ret;

    // for certain fd modes, size 0 indicates to use a size from extended buffer header
    int flags_set = s_l2tap_sockets[fd].flags & L2TAP_FLAG_TS;
    if ((flags_set && size != 0) || (!flags_set && size == 0)) {
        if (flags_set) {
            // Invalid argument
            errno = EINVAL;
            return -1;
        } else {
            return 0;
        }
    }

    if (size == 0) {
        ext_buff = (l2tap_extended_buff_t *)data;
        // check if extended buffer holds pointer to valid IO frame buffer
        if (ext_buff->buff == NULL) {
            errno = EFAULT;
            goto err;
        }
        eth_buff = ext_buff->buff;
        size = ext_buff->buff_len;
    } else {
        eth_buff = (void *)data;
        ext_buff = NULL;
    }

    if (atomic_load(&s_l2tap_sockets[fd].state) == L2TAP_SOCK_STATE_OPENED) {
        if (s_l2tap_sockets[fd].ethtype_filter > ETH_IEEE802_3_MAX_LEN &&
                ((struct eth_hdr *)eth_buff)->type != htons(s_l2tap_sockets[fd].ethtype_filter)) {
            // bad message
            errno = EBADMSG;
            goto err;
        }

        if (s_l2tap_sockets[fd].flags & L2TAP_FLAG_TS) {
            l2tap_timestamp_t hw_ts;
            if ((esp_ret = l2tap_transmit_ts(&s_l2tap_sockets[fd], eth_buff, size, &hw_ts)) == ESP_OK){
                // find the record allocated for the time stamp info
                l2tap_irec_hdr_t *info_rec = L2TAP_IREC_FIRST(ext_buff);
                while(info_rec != NULL) {
                    if (info_rec->type == L2TAP_IREC_TIME_STAMP) {
                        break;
                    }
                    info_rec = L2TAP_IREC_NEXT(ext_buff, info_rec);
                }
                // if there is a record to retrieve time stamp
                if (info_rec != NULL) {
                    if (info_rec->len - sizeof(l2tap_irec_hdr_t) >= sizeof(struct timespec)) {
                        struct timespec *ts = (struct timespec *)info_rec->data;
                        ts->tv_sec = hw_ts.sec;
                        ts->tv_nsec = hw_ts.nsec;
                    } else {
                        info_rec->type = L2TAP_IREC_INVALID;
                    }
                }
                ret = size;
            } else {
                ESP_LOGE(TAG, "l2tap_transmit_ts failed: %d", esp_ret);
                errno = l2tap_tx_esp_err_to_errno(esp_ret);
            }
        } else {
            if ((esp_ret = l2tap_transmit(&s_l2tap_sockets[fd], eth_buff, size)) == ESP_OK) {
                ret = size;
            } else {
                errno = l2tap_tx_esp_err_to_errno(esp_ret);
            }
        }
    } else {
        // bad file desc
        errno = EBADF;
    }
err:
    return ret;
}

static int l2tap_rx_esp_err_to_errno(esp_err_t esp_err)
{
    switch(esp_err) {
        case ESP_ERR_INVALID_ARG:
            return EINVAL;
        case ESP_ERR_TIMEOUT:
            return EAGAIN;
        case ESP_ERR_INVALID_STATE:
            return EPERM;
        default:
            return EIO;
    }
}

static ssize_t l2tap_read(__attribute__((unused)) void *ctx, int fd, void *data, size_t size)
{
    // fd might be in process of opening/closing (close was already called but preempted)
    if (atomic_load(&s_l2tap_sockets[fd].state) != L2TAP_SOCK_STATE_OPENED) {
        // bad file desc
        errno = EBADF;
        return -1;
    }

    // for certain fd modes, size 0 indicates to use a size from extended buffer header
    int flags_set = s_l2tap_sockets[fd].flags & L2TAP_FLAG_TS;
    if ((flags_set && size != 0) || (!flags_set && size == 0)) {
        if (flags_set) {
            // Invalid argument
            errno = EINVAL;
            return -1;
        } else {
            return 0;
        }
    }

    if (size == 0) {
        l2tap_extended_buff_t *ext_buff = (l2tap_extended_buff_t *)data;
        // check if extended buffer holds pointer to valid IO frame buffer
        if (ext_buff->buff == NULL) {
            errno = EFAULT;
            return -1;
        }
    }

    esp_err_t esp_ret;
    ssize_t actual_size;
    if ((esp_ret = pop_rx_queue(&s_l2tap_sockets[fd], data, size, &actual_size)) != ESP_OK) {
        errno = l2tap_rx_esp_err_to_errno(esp_ret);
    }

    return actual_size;
}

static void l2tap_clean_task(void *task_param)
{
    l2tap_context_t *l2tap_socket = (l2tap_context_t *)task_param;

    // flush queued frames to not affect "empty queue" signalling below
    flush_rx_queue(l2tap_socket);

    // push empty queue to unblock possibly blocking task
    push_rx_queue(l2tap_socket, NULL, 0, NULL, NULL);
    // wait for the indication that blocking task was executed (unblocked)
    ssize_t actual_size;
    pop_rx_queue(l2tap_socket, NULL, 0, &actual_size);

    // now, all higher priority tasks should finished their execution and new accesses to the queue were prevented
    // by L2TAP_SOCK_STATE_CLOSING => we are free to free queue resources
    delete_rx_queue(l2tap_socket);

    // unblock task which originally called close
    xSemaphoreGive(l2tap_socket->close_done_sem);

    // all done, delete itsefl
    vTaskDelete(NULL);
}

static int l2tap_close(__attribute__((unused)) void *ctx, int fd)
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
        goto close_failed;
    }
    // If one task is blocked in I/O operation and another task tries to close the fd, the first task is
    // unblocked by pushing empty queue in low priority task (to ensure context switch to the first task).
    // The first's task read operation then ends with error and the low priority task frees the queue resources.
    if (xTaskCreate(l2tap_clean_task, "l2tap_clean_task", 1024, &s_l2tap_sockets[fd], tskIDLE_PRIORITY, NULL) == pdFAIL) {
        ESP_LOGE(TAG, "create l2tap_clean_task failed");
        vSemaphoreDelete(s_l2tap_sockets[fd].close_done_sem);
        s_l2tap_sockets[fd].close_done_sem = NULL;
        goto close_failed;
    }

    // wait for the low priority close task & then delete the semaphore
    xSemaphoreTake(s_l2tap_sockets[fd].close_done_sem, portMAX_DELAY);
    vSemaphoreDelete(s_l2tap_sockets[fd].close_done_sem); // no worries to delete, this task owns the semaphore

    // indicate that socket is ready to be used again
    atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_READY);
    return 0;

close_failed:
    flush_rx_queue(&s_l2tap_sockets[fd]);
    (void)push_rx_queue(&s_l2tap_sockets[fd], NULL, 0, NULL, NULL);
    delete_rx_queue(&s_l2tap_sockets[fd]);
    atomic_store(&s_l2tap_sockets[fd].state, L2TAP_SOCK_STATE_READY);
    errno = ENOMEM;
    return -1;
}

// used to find a netif with the attached driver matching the argument
static bool netif_driver_matches(esp_netif_t *netif, void* driver)
{
    return esp_netif_get_io_driver(netif) == driver;
}

static void l2tap_socket_apply_io_fns(l2tap_context_t *l2tap_socket, const esp_eth_iodriver_io_fns_t *io_fns)
{
    l2tap_socket->driver_handle = io_fns->io_handle;
    l2tap_socket->iodriver_transmit = io_fns->iodriver_transmit;
    l2tap_socket->iodriver_transmit_wrap = io_fns->iodriver_transmit_wrap;
    l2tap_socket->iodriver_free_rx_buffer = io_fns->iodriver_free_rx_buffer;
    l2tap_socket->iodriver_get_ll_driver = io_fns->iodriver_get_ll_driver;
}

static void l2tap_bind_io_handle(l2tap_context_t *l2tap_socket, l2tap_iodriver_handle io_handle)
{
    l2tap_provider_node_t *it;
    esp_eth_iodriver_provider_base_t *provider;
    esp_eth_iodriver_io_fns_t io_fns;

    l2tap_enter_critical();
    // NULL handle can't be served by any provider, don't even try and bind it directly (legacy mode)
    if (io_handle != NULL) {
        SLIST_FOREACH(it, &s_provider_list, next) {
            provider = (esp_eth_iodriver_provider_base_t *)it->handle;
            if (provider->get_io_fns != NULL &&
                    provider->get_io_fns(provider, io_handle, &io_fns) == ESP_OK) {
                l2tap_socket_apply_io_fns(l2tap_socket, &io_fns);
                l2tap_exit_critical();
                return;
            }
        }
    }
    // No iodriver provider matched: bind the handle directly (legacy mode).
    l2tap_socket->driver_handle = io_handle;
    l2tap_socket->iodriver_transmit = NULL;
    l2tap_socket->iodriver_transmit_wrap = NULL;
    l2tap_socket->iodriver_free_rx_buffer = NULL;
    l2tap_socket->iodriver_get_ll_driver = NULL;
    l2tap_exit_critical();
}

static int l2tap_ioctl(__attribute__((unused)) void *ctx, int fd, int cmd, va_list args)
{
    esp_netif_t *esp_netif;
    switch (cmd) {
    case L2TAP_S_RCV_FILTER:{
        uint16_t *new_ethtype_filter = va_arg(args, uint16_t *);
        l2tap_enter_critical();
        // socket needs to be assigned to interface at first
        if (s_l2tap_sockets[fd].driver_handle == NULL) {
            // Permission denied (filter change is denied at this state)
            errno = EACCES;
            l2tap_exit_critical();
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
                    l2tap_exit_critical();
                    goto err;
                }
            }
            s_l2tap_sockets[fd].ethtype_filter = *new_ethtype_filter;
        }
        l2tap_exit_critical();
        break;
    }
    case L2TAP_G_RCV_FILTER:{
        uint16_t *ethtype_filter_dest = va_arg(args, uint16_t *);
        *ethtype_filter_dest = s_l2tap_sockets[fd].ethtype_filter;
        break;
    }
    case L2TAP_S_INTF_DEVICE:{
        const char *str = va_arg(args, const char *);
        // get netif handle from if key (the highest layer)
        esp_netif = esp_netif_get_handle_from_ifkey(str);
        if (esp_netif == NULL) {
            // No such device
            errno = ENODEV;
            goto err;
        }
        // find the iodriver provider based on the netif iodriver
        esp_netif_iodriver_handle netif_iodriver = esp_netif_get_io_driver(esp_netif);
        if (netif_iodriver == NULL) {
            // No such device (netif does not have any IO driver attached)
            errno = ENODEV;
            goto err;
        }
        l2tap_bind_io_handle(&s_l2tap_sockets[fd], netif_iodriver);
        break;
    }
    case L2TAP_G_INTF_DEVICE:{
        const char **str_p = va_arg(args, const char **);
        *str_p = NULL;
        if ((esp_netif = esp_netif_find_if(netif_driver_matches, s_l2tap_sockets[fd].driver_handle)) != NULL) {
            *str_p = esp_netif_get_ifkey(esp_netif);
        }
        break;
    }
    case L2TAP_S_DEVICE_DRV_HNDL:{
        l2tap_iodriver_handle device_driver_hdl = va_arg(args, l2tap_iodriver_handle);
        if (device_driver_hdl == NULL) {
            // No such device (not valid driver handle)
            errno = EINVAL;
            goto err;
        }
        l2tap_bind_io_handle(&s_l2tap_sockets[fd], device_driver_hdl);
        break;
    }
    case L2TAP_G_DEVICE_DRV_HNDL:{
        l2tap_iodriver_handle *get_driver_hdl = va_arg(args, l2tap_iodriver_handle*);
        // the driver handle and the associated getter must be read consistently since they can be
        // reassigned from other task
        l2tap_enter_critical();
        // no iodriver provider registered, get the driver handle directly (legacy mode)
        if (s_l2tap_sockets[fd].iodriver_get_ll_driver == NULL) {
            *get_driver_hdl = s_l2tap_sockets[fd].driver_handle;
        } else {
            s_l2tap_sockets[fd].iodriver_get_ll_driver(s_l2tap_sockets[fd].driver_handle, get_driver_hdl);
        }
        l2tap_exit_critical();
        break;
    }
    case L2TAP_S_TIMESTAMP_EN:
        l2tap_enter_critical();
        s_l2tap_sockets[fd].flags |= L2TAP_FLAG_TS;
        // TODO add JIRA ticket - maybe not needed tests with sublayer where L2TAP_S_TIMESTAMP_EN may not be needed
        s_l2tap_sockets[fd].eth_transmit_ctrl_bufs = esp_eth_transmit_ctrl_bufs;
        l2tap_exit_critical();
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

static void l2tap_set_nonblocking(l2tap_context_t *l2tap_socket, bool nonblock)
{
    l2tap_enter_critical();
    if (nonblock) {
        l2tap_socket->flags |= L2TAP_FLAG_NON_BLOCK;
    } else {
        l2tap_socket->flags &= ~L2TAP_FLAG_NON_BLOCK;
    }
    l2tap_exit_critical();
}

static int l2tap_fcntl(__attribute__((unused)) void *ctx, int fd, int cmd, int arg)
{
    int result = 0;
    if (cmd == F_GETFL) {
        if (s_l2tap_sockets[fd].flags & L2TAP_FLAG_NON_BLOCK) {
            result |= O_NONBLOCK;
        }
    } else if (cmd == F_SETFL) {
        // only O_NONBLOCK is supported
        if ((arg & ~O_NONBLOCK) == 0) {
            l2tap_set_nonblocking(&s_l2tap_sockets[fd], (arg & O_NONBLOCK) == O_NONBLOCK);
        } else {
            result = -1;
            errno = EINVAL;
        }
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
            s_registered_selects = NULL;
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
                // Move last element to fill gap (only if not removing the last element)
                if (i < new_size) {
                    s_registered_selects[i] = s_registered_selects[new_size];
                }
                // Handle reallocation
                if (new_size == 0) {
                    // Free the entire array
                    free(s_registered_selects);
                    s_registered_selects = NULL;
                    s_registered_select_cnt = 0;
                    ret = ESP_OK;
                } else {
                    // Shrink the array
                    l2tap_select_args_t **new_selects = realloc(s_registered_selects, new_size * sizeof(l2tap_select_args_t *));
                    if (new_selects == NULL) {
                        // Realloc failed - restore the moved element and return error
                        if (i < new_size) {
                            s_registered_selects[new_size] = s_registered_selects[i];
                        }
                        ret = ESP_ERR_NO_MEM;
                    } else {
                        // Success - update pointer and count atomically
                        s_registered_selects = new_selects;
                        s_registered_select_cnt = new_size;
                        ret = ESP_OK;
                    }
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

    l2tap_enter_critical();

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
        l2tap_exit_critical();
        free(args);
        return ret;
    }

    l2tap_exit_critical();

    *end_select_args = args;

    return ESP_OK;
}

static esp_err_t l2tap_end_select(void *end_select_args)
{
    l2tap_select_args_t *args = end_select_args;
    if (args == NULL) {
        return ESP_OK;
    }

    l2tap_enter_critical();
    esp_err_t ret = unregister_select(args);
    l2tap_exit_critical();

    if (args) {
        free(args);
    }

    return ret;
}

static const esp_vfs_select_ops_t s_vfs_l2tap_select = {
    .start_select = &l2tap_start_select,
    .end_select = &l2tap_end_select,
};
#endif //CONFIG_VFS_SUPPORT_SELECT

static const esp_vfs_fs_ops_t s_vfs_l2tap = {
    .write_p = &l2tap_write,
    .open_p = &l2tap_open,
    .close_p = &l2tap_close,
    .read_p = &l2tap_read,
    .fcntl_p = &l2tap_fcntl,
    .ioctl_p = &l2tap_ioctl,
#ifdef CONFIG_VFS_SUPPORT_SELECT
    .select = &s_vfs_l2tap_select,
#endif // CONFIG_VFS_SUPPORT_SELECT
};

/* ================== IO Driver Provider Registry ====================== */

esp_err_t esp_vfs_l2tap_iodriver_provider_register(l2tap_iodriver_provider_handle provider)
{
    ESP_RETURN_ON_FALSE(provider, ESP_ERR_INVALID_ARG, TAG, "iodriver provider handle is NULL");

    l2tap_provider_node_t *node = malloc(sizeof(*node));
    if (!node) {
        return ESP_ERR_NO_MEM;
    }
    node->handle = provider;

    esp_err_t ret = ESP_OK;
    l2tap_enter_critical();
    l2tap_provider_node_t *it;
    SLIST_FOREACH(it, &s_provider_list, next) {
        if (it->handle == provider) {
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
    }
    if (ret == ESP_OK) {
        SLIST_INSERT_HEAD(&s_provider_list, node, next);
    }
    l2tap_exit_critical();

    if (ret != ESP_OK) {
        free(node);
    }
    return ret;
}

esp_err_t esp_vfs_l2tap_iodriver_provider_unregister(l2tap_iodriver_provider_handle provider)
{
    ESP_RETURN_ON_FALSE(provider, ESP_ERR_INVALID_ARG, TAG, "iodriver provider handle is NULL");

    l2tap_provider_node_t *found = NULL;
    l2tap_enter_critical();
    l2tap_provider_node_t *it;
    SLIST_FOREACH(it, &s_provider_list, next) {
        if (it->handle == provider) {
            found = it;
            SLIST_REMOVE(&s_provider_list, it, l2tap_provider_node, next);
            break;
        }
    }
    l2tap_exit_critical();

    if (found) {
        free(found);
        return ESP_OK;
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t esp_vfs_l2tap_intf_register(l2tap_vfs_config_t *config)
{
    l2tap_vfs_config_t def_config = L2TAP_VFS_CONFIG_DEFAULT();

    if (config == NULL) {
        ESP_LOGD(TAG, "vfs is to be registered with default settings");
        config = &def_config;
    }

    ESP_RETURN_ON_FALSE(!s_is_registered, ESP_ERR_INVALID_STATE, TAG, "vfs is already registered");
    s_is_registered = true;
    ESP_RETURN_ON_ERROR(esp_vfs_register_fs(config->base_path, &s_vfs_l2tap, ESP_VFS_FLAG_STATIC | ESP_VFS_FLAG_CONTEXT_PTR, NULL), TAG, "vfs register error");

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
