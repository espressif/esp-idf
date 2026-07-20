/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/lock.h>
#include "esp_vfs.h"
#include "esp_vfs_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/ringbuf.h"
#include "common/bt_target.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "osi/mutex.h"
#include "osi/fixed_queue.h"
#include "bta/bta_opp_api.h"
#include "btc_opp.h"

#if BTC_OPP_INCLUDED

#define OPP_VFS_MAX_SLOTS           2
#if BTC_OPP_CLIENT_INCLUDED
#define OPP_VFS_TX_BUFFER_SIZE      (4 * 1024)
#define OPP_VFS_WRITE_TIMEOUT_MS    (40 * 1000)
#define SLOT_WRITE_BIT(i)           (1UL << (i))
#define SLOT_CLOSE_BIT(i)           (1UL << (OPP_VFS_MAX_SLOTS + (i)))
#endif
#if BTC_OPP_SERVER_INCLUDED
/* Keep the RX queue small so body copies cannot starve L2CAP SAR (10KB). */
#define OPP_VFS_RX_QUEUE_SIZE       4
#define OPP_VFS_RX_CREDIT_LOW       1
#endif

typedef enum {
    OPP_VFS_ROLE_NONE = 0,
#if BTC_OPP_CLIENT_INCLUDED
    OPP_VFS_ROLE_CLIENT,
#endif
#if BTC_OPP_SERVER_INCLUDED
    OPP_VFS_ROLE_SERVER,
#endif
} opp_vfs_role_t;

#if BTC_OPP_SERVER_INCLUDED
typedef struct {
    uint8_t *data;
    uint16_t len;
    uint16_t offset;
    bool final;
} opp_vfs_rx_item_t;
#endif

typedef struct {
    bool used;
    uint8_t serial;
    opp_vfs_role_t role;
    esp_opp_conn_hdl_t handle;
    int fd;
#if BTC_OPP_CLIENT_INCLUDED
    RingbufHandle_t ringbuf_write;
    uint32_t tx_len;
    uint32_t bytes_written;
    uint32_t bytes_sent;
    bool writer_closed;
    bool length_error;
#endif
#if BTC_OPP_SERVER_INCLUDED
    fixed_queue_t *rx_queue;
    bool reader_eof;
#endif
    bool active;
} opp_vfs_slot_t;

typedef struct {
    osi_mutex_t mutex;
#if BTC_OPP_CLIENT_INCLUDED
    EventGroupHandle_t tx_event_group;
#endif
    esp_vfs_id_t vfs_id;
    bool registered;
    int refcount;                           /*!< Number of OPP roles holding the VFS. */
    opp_vfs_slot_t slots[OPP_VFS_MAX_SLOTS];
} opp_vfs_local_param_t;

static opp_vfs_local_param_t s_opp_vfs;

#if BTC_OPP_SERVER_INCLUDED
static void opp_vfs_free_rx_item(void *p)
{
    opp_vfs_rx_item_t *item = (opp_vfs_rx_item_t *)p;
    if (item) {
        if (item->data) {
            osi_free(item->data);
        }
        osi_free(item);
    }
}
#endif

static opp_vfs_slot_t *opp_vfs_find_by_fd(int fd)
{
    for (int i = 0; i < OPP_VFS_MAX_SLOTS; i++) {
        if (s_opp_vfs.slots[i].used && s_opp_vfs.slots[i].fd == fd) {
            return &s_opp_vfs.slots[i];
        }
    }
    return NULL;
}

static opp_vfs_slot_t *opp_vfs_find_by_handle(esp_opp_conn_hdl_t handle)
{
    for (int i = 0; i < OPP_VFS_MAX_SLOTS; i++) {
        if (s_opp_vfs.slots[i].used && s_opp_vfs.slots[i].handle == handle) {
            return &s_opp_vfs.slots[i];
        }
    }
    return NULL;
}

static void opp_vfs_free_slot(opp_vfs_slot_t *slot)
{
    if (slot == NULL || !slot->used) {
        return;
    }

#if BTC_OPP_CLIENT_INCLUDED
    if (s_opp_vfs.tx_event_group) {
        xEventGroupSetBits(s_opp_vfs.tx_event_group, SLOT_CLOSE_BIT(slot->serial));
    }
#endif

    if (slot->fd >= 0 && s_opp_vfs.vfs_id >= 0) {
        (void)esp_vfs_unregister_fd(s_opp_vfs.vfs_id, slot->fd);
    }
#if BTC_OPP_CLIENT_INCLUDED
    if (slot->ringbuf_write) {
        vRingbufferDelete(slot->ringbuf_write);
        slot->ringbuf_write = NULL;
    }
#endif
#if BTC_OPP_SERVER_INCLUDED
    if (slot->rx_queue) {
        fixed_queue_free(slot->rx_queue, opp_vfs_free_rx_item);
        slot->rx_queue = NULL;
    }
#endif

    memset(slot, 0, sizeof(*slot));
    slot->fd = -1;
}

static esp_err_t opp_vfs_alloc_slot(esp_opp_conn_hdl_t handle, opp_vfs_role_t role,
                                    uint32_t tx_len, int *out_fd)
{
    opp_vfs_slot_t *slot = NULL;

    if (!s_opp_vfs.registered || out_fd == NULL || handle == ESP_OPP_INVALID_HANDLE) {
        return ESP_ERR_INVALID_STATE;
    }
#if !BTC_OPP_CLIENT_INCLUDED
    (void)tx_len;
#endif

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    {
        opp_vfs_slot_t *existing = opp_vfs_find_by_handle(handle);
        if (existing != NULL) {
            /* Replace a drained/stale slot for the same connection. */
            opp_vfs_free_slot(existing);
        }
    }

    for (int i = 0; i < OPP_VFS_MAX_SLOTS; i++) {
        if (!s_opp_vfs.slots[i].used) {
            slot = &s_opp_vfs.slots[i];
            slot->serial = (uint8_t)i;
            break;
        }
    }
    if (slot == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return ESP_ERR_NO_MEM;
    }

    memset(slot, 0, sizeof(*slot));
    slot->used = true;
    slot->serial = (uint8_t)(slot - s_opp_vfs.slots);
    slot->role = role;
    slot->handle = handle;
    slot->fd = -1;
#if BTC_OPP_CLIENT_INCLUDED
    slot->tx_len = tx_len;
#endif
    slot->active = true;

#if BTC_OPP_CLIENT_INCLUDED
    if (s_opp_vfs.tx_event_group) {
        xEventGroupClearBits(s_opp_vfs.tx_event_group,
                             SLOT_WRITE_BIT(slot->serial) | SLOT_CLOSE_BIT(slot->serial));
    }

    if (role == OPP_VFS_ROLE_CLIENT) {
        slot->ringbuf_write = xRingbufferCreate(OPP_VFS_TX_BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
        if (slot->ringbuf_write == NULL) {
            slot->used = false;
            osi_mutex_unlock(&s_opp_vfs.mutex);
            return ESP_ERR_NO_MEM;
        }
    }
#endif
#if BTC_OPP_SERVER_INCLUDED
    if (role == OPP_VFS_ROLE_SERVER) {
        slot->rx_queue = fixed_queue_new(OPP_VFS_RX_QUEUE_SIZE);
        if (slot->rx_queue == NULL) {
            slot->used = false;
            osi_mutex_unlock(&s_opp_vfs.mutex);
            return ESP_ERR_NO_MEM;
        }
    }
#endif

    if (esp_vfs_register_fd(s_opp_vfs.vfs_id, &slot->fd) != ESP_OK) {
#if BTC_OPP_CLIENT_INCLUDED
        if (slot->ringbuf_write) {
            vRingbufferDelete(slot->ringbuf_write);
        }
#endif
#if BTC_OPP_SERVER_INCLUDED
        if (slot->rx_queue) {
            fixed_queue_free(slot->rx_queue, opp_vfs_free_rx_item);
        }
#endif
        slot->used = false;
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return ESP_FAIL;
    }

    *out_fd = slot->fd;
    osi_mutex_unlock(&s_opp_vfs.mutex);
    return ESP_OK;
}

bool btc_opp_vfs_is_registered(void)
{
    return s_opp_vfs.registered;
}

#if BTC_OPP_CLIENT_INCLUDED
esp_err_t btc_opp_vfs_alloc_client_slot(esp_opp_conn_hdl_t handle, uint32_t len, int *out_fd)
{
    return opp_vfs_alloc_slot(handle, OPP_VFS_ROLE_CLIENT, len, out_fd);
}
#endif

#if BTC_OPP_SERVER_INCLUDED
esp_err_t btc_opp_vfs_alloc_server_slot(esp_opp_conn_hdl_t handle, int *out_fd)
{
    return opp_vfs_alloc_slot(handle, OPP_VFS_ROLE_SERVER, 0, out_fd);
}

esp_err_t btc_opp_vfs_get_fd(esp_opp_conn_hdl_t handle, int *out_fd)
{
    if (out_fd == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot == NULL || slot->fd < 0) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return ESP_ERR_NOT_FOUND;
    }
    *out_fd = slot->fd;
    osi_mutex_unlock(&s_opp_vfs.mutex);
    return ESP_OK;
}
#endif

void btc_opp_vfs_free_slot_by_handle(esp_opp_conn_hdl_t handle)
{
    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot) {
        opp_vfs_free_slot(slot);
    }
    osi_mutex_unlock(&s_opp_vfs.mutex);
}

void btc_opp_vfs_on_conn_close(esp_opp_conn_hdl_t handle)
{
    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return;
    }

#if BTC_OPP_SERVER_INCLUDED
    if (slot->role == OPP_VFS_ROLE_SERVER) {
        /* Peer may disconnect immediately after the last object. Keep the
         * VFS fd alive so the app can drain any remaining queued body bytes;
         * close(fd) will release the slot. */
        slot->reader_eof = true;
        slot->active = false;
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return;
    }
#endif
    opp_vfs_free_slot(slot);
    osi_mutex_unlock(&s_opp_vfs.mutex);
}

void btc_opp_vfs_on_transfer_complete(esp_opp_conn_hdl_t handle)
{
    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return;
    }

#if BTC_OPP_SERVER_INCLUDED
    if (slot->role == OPP_VFS_ROLE_SERVER) {
        slot->reader_eof = true;
        slot->active = false;
        /* Keep the slot while unread body remains so the app can still read(fd). */
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return;
    }
#endif
    opp_vfs_free_slot(slot);
    osi_mutex_unlock(&s_opp_vfs.mutex);
}

#if BTC_OPP_SERVER_INCLUDED
bool btc_opp_vfs_enqueue_rx(esp_opp_conn_hdl_t handle, const uint8_t *data, uint16_t data_len, bool final)
{
    bool ok = false;
    bool give_credit = false;

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot == NULL || slot->role != OPP_VFS_ROLE_SERVER || slot->rx_queue == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return false;
    }

    opp_vfs_rx_item_t *item = (opp_vfs_rx_item_t *)osi_calloc(sizeof(opp_vfs_rx_item_t));
    if (item == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return false;
    }

    if (data_len > 0 && data != NULL) {
        item->data = (uint8_t *)osi_malloc(data_len);
        if (item->data == NULL) {
            osi_free(item);
            osi_mutex_unlock(&s_opp_vfs.mutex);
            return false;
        }
        memcpy(item->data, data, data_len);
        item->len = data_len;
    }
    item->final = final;

    /* Non-blocking: backpressure is applied by deferring OBEX Continue. */
    if (!fixed_queue_enqueue(slot->rx_queue, item, 0)) {
        opp_vfs_free_rx_item(item);
    } else {
        if (final) {
            slot->reader_eof = true;
        } else if (fixed_queue_length(slot->rx_queue) <= OPP_VFS_RX_CREDIT_LOW) {
            give_credit = true;
        }
        ok = true;
    }
    osi_mutex_unlock(&s_opp_vfs.mutex);

    if (give_credit) {
        BTA_OppServerRxReady(handle);
    }
    return ok;
}
#endif /* BTC_OPP_SERVER_INCLUDED */

#if BTC_OPP_CLIENT_INCLUDED
int btc_opp_vfs_pull_tx(esp_opp_conn_hdl_t handle, uint8_t *buf, uint16_t max_len,
                        uint16_t *out_len, bool *is_final, bool *waiting)
{
    size_t item_size = 0;
    uint8_t *item = NULL;
    uint32_t remaining;
    uint16_t want;

    if (out_len == NULL || is_final == NULL || waiting == NULL || buf == NULL) {
        return -1;
    }
    *out_len = 0;
    *is_final = false;
    *waiting = false;

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_handle(handle);
    if (slot == NULL || slot->role != OPP_VFS_ROLE_CLIENT || !slot->active) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return -1;
    }
    if (slot->length_error) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return -1;
    }

    remaining = (slot->bytes_sent < slot->tx_len) ? (slot->tx_len - slot->bytes_sent) : 0;
    if (remaining == 0) {
        if (slot->writer_closed) {
            *is_final = true;
            osi_mutex_unlock(&s_opp_vfs.mutex);
            return 0;
        }
        *waiting = true;
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return 0;
    }

    want = max_len;
    if (want > remaining) {
        want = (uint16_t)remaining;
    }

    item = (uint8_t *)xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, want);
    if (item == NULL || item_size == 0) {
        if (slot->writer_closed) {
            /* Writer finished but declared length not fully queued. */
            slot->length_error = true;
            osi_mutex_unlock(&s_opp_vfs.mutex);
            return -1;
        }
        *waiting = true;
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return 0;
    }

    memcpy(buf, item, item_size);
    vRingbufferReturnItem(slot->ringbuf_write, item);
    slot->bytes_sent += item_size;
    *out_len = (uint16_t)item_size;
    *is_final = (slot->bytes_sent >= slot->tx_len);

    if (s_opp_vfs.tx_event_group) {
        xEventGroupSetBits(s_opp_vfs.tx_event_group, SLOT_WRITE_BIT(slot->serial));
    }
    osi_mutex_unlock(&s_opp_vfs.mutex);
    return 0;
}

static ssize_t opp_vfs_write(int fd, const void *data, size_t size)
{
    ssize_t sent = 0;
    size_t items_waiting = 0;
    size_t item_size = 0;
    EventBits_t bits = 0;
    uint8_t serial = 0;

    errno = 0;
    if (size == 0) {
        return 0;
    }
    if (!s_opp_vfs.registered || data == NULL) {
        errno = ESRCH;
        return -1;
    }

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_fd(fd);
    if (slot == NULL || slot->role != OPP_VFS_ROLE_CLIENT || slot->writer_closed || !slot->active) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        errno = EPIPE;
        return -1;
    }
    serial = slot->serial;
    osi_mutex_unlock(&s_opp_vfs.mutex);

    while (size) {
        osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = &s_opp_vfs.slots[serial];
        if (!slot->used || slot->fd != fd || slot->writer_closed || !slot->active) {
            osi_mutex_unlock(&s_opp_vfs.mutex);
            if (sent > 0) {
                return sent;
            }
            errno = EPIPE;
            return -1;
        }
        if (slot->bytes_written >= slot->tx_len) {
            osi_mutex_unlock(&s_opp_vfs.mutex);
            if (sent > 0) {
                return sent;
            }
            errno = EFBIG;
            return -1;
        }

        items_waiting = 0;
        vRingbufferGetInfo(slot->ringbuf_write, NULL, NULL, NULL, NULL, &items_waiting);
        if (items_waiting < OPP_VFS_TX_BUFFER_SIZE) {
            item_size = OPP_VFS_TX_BUFFER_SIZE - items_waiting;
            if (item_size > size) {
                item_size = size;
            }
            if (item_size > (slot->tx_len - slot->bytes_written)) {
                item_size = (size_t)(slot->tx_len - slot->bytes_written);
            }
            if (xRingbufferSend(slot->ringbuf_write, (void *)((const uint8_t *)data + sent), item_size, 0)) {
                esp_opp_conn_hdl_t ready_handle = slot->handle;
                slot->bytes_written += item_size;
                sent += item_size;
                size -= item_size;
                osi_mutex_unlock(&s_opp_vfs.mutex);
                BTA_OppClientTxReady(ready_handle);
                continue;
            }
        }
        osi_mutex_unlock(&s_opp_vfs.mutex);

        bits = xEventGroupWaitBits(s_opp_vfs.tx_event_group,
                                   SLOT_WRITE_BIT(serial) | SLOT_CLOSE_BIT(serial),
                                   pdTRUE, pdFALSE,
                                   OPP_VFS_WRITE_TIMEOUT_MS / portTICK_PERIOD_MS);
        if (bits & SLOT_CLOSE_BIT(serial)) {
            if (sent > 0) {
                return sent;
            }
            errno = EPIPE;
            return -1;
        }
        if (!(bits & (SLOT_WRITE_BIT(serial) | SLOT_CLOSE_BIT(serial)))) {
            if (sent > 0) {
                return sent;
            }
            errno = EBUSY;
            return -1;
        }
    }

    return sent;
}
#endif /* BTC_OPP_CLIENT_INCLUDED */

#if BTC_OPP_SERVER_INCLUDED
static ssize_t opp_vfs_read(int fd, void *dst, size_t size)
{
    size_t copied = 0;
    bool give_credit = false;
    esp_opp_conn_hdl_t credit_handle = ESP_OPP_INVALID_HANDLE;

    errno = 0;
    if (size == 0) {
        return 0;
    }
    if (!s_opp_vfs.registered || dst == NULL) {
        errno = ESRCH;
        return -1;
    }

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_fd(fd);
    if (slot == NULL || slot->role != OPP_VFS_ROLE_SERVER) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        errno = ENOENT;
        return -1;
    }

    while (copied < size) {
        opp_vfs_rx_item_t *item = (opp_vfs_rx_item_t *)fixed_queue_try_peek_first(slot->rx_queue);
        if (item == NULL) {
            break;
        }

        size_t avail = item->len - item->offset;
        size_t to_copy = size - copied;
        if (to_copy > avail) {
            to_copy = avail;
        }
        if (to_copy > 0 && item->data) {
            memcpy((uint8_t *)dst + copied, item->data + item->offset, to_copy);
            item->offset += to_copy;
            copied += to_copy;
        }

        if (item->offset >= item->len) {
            bool final = item->final;
            fixed_queue_dequeue(slot->rx_queue, FIXED_QUEUE_MAX_TIMEOUT);
            opp_vfs_free_rx_item(item);
            if (fixed_queue_length(slot->rx_queue) <= OPP_VFS_RX_CREDIT_LOW) {
                give_credit = true;
                credit_handle = slot->handle;
            }
            if (final && fixed_queue_is_empty(slot->rx_queue)) {
                slot->reader_eof = true;
                break;
            }
            if (to_copy == 0 && final) {
                slot->reader_eof = true;
                break;
            }
        } else {
            break;
        }
    }

    if (copied > 0) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        if (give_credit) {
            BTA_OppServerRxReady(credit_handle);
        }
        return (ssize_t)copied;
    }

    /* copied == 0: either true EOF or no data available yet. */
    if (slot->reader_eof &&
        (slot->rx_queue == NULL || fixed_queue_is_empty(slot->rx_queue))) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        return 0; /* EOF */
    }

    osi_mutex_unlock(&s_opp_vfs.mutex);
    errno = EAGAIN;
    return -1;
}
#endif /* BTC_OPP_SERVER_INCLUDED */

static int opp_vfs_close(int fd)
{
    esp_opp_conn_hdl_t handle = ESP_OPP_INVALID_HANDLE;

    errno = 0;
    if (!s_opp_vfs.registered) {
        errno = ESRCH;
        return -1;
    }

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    opp_vfs_slot_t *slot = opp_vfs_find_by_fd(fd);
    if (slot == NULL) {
        osi_mutex_unlock(&s_opp_vfs.mutex);
        errno = ENOENT;
        return -1;
    }

    handle = slot->handle;

#if BTC_OPP_CLIENT_INCLUDED
    if (slot->role == OPP_VFS_ROLE_CLIENT) {
        bool length_error = false;

        if (slot->bytes_written != slot->tx_len) {
            slot->length_error = true;
            length_error = true;
        }
        slot->writer_closed = true;
        osi_mutex_unlock(&s_opp_vfs.mutex);
        if (length_error) {
            BTA_OppClientCancel(handle);
        } else {
            BTA_OppClientTxReady(handle);
        }
        return 0;
    }
#endif

#if BTC_OPP_SERVER_INCLUDED
    /* Server close: only cancel when the app closes early mid-transfer.
     * After EOF / transfer complete, just free the slot and keep the OBEX
     * connection so the peer can push another object on the same session. */
    bool early_close = slot->active && !slot->reader_eof;
    opp_vfs_free_slot(slot);
    osi_mutex_unlock(&s_opp_vfs.mutex);
    if (early_close) {
        BTA_OppServerCancel(handle);
    }
    return 0;
#else
    opp_vfs_free_slot(slot);
    osi_mutex_unlock(&s_opp_vfs.mutex);
    return 0;
#endif
}

static esp_err_t opp_vfs_register_internal(void)
{
    if (s_opp_vfs.registered) {
        return ESP_OK;
    }

    if (osi_mutex_new(&s_opp_vfs.mutex) != 0) {
        return ESP_FAIL;
    }
#if BTC_OPP_CLIENT_INCLUDED
    s_opp_vfs.tx_event_group = xEventGroupCreate();
    if (s_opp_vfs.tx_event_group == NULL) {
        osi_mutex_free(&s_opp_vfs.mutex);
        return ESP_FAIL;
    }
#endif
    s_opp_vfs.vfs_id = -1;
    memset(s_opp_vfs.slots, 0, sizeof(s_opp_vfs.slots));
    for (int i = 0; i < OPP_VFS_MAX_SLOTS; i++) {
        s_opp_vfs.slots[i].fd = -1;
    }

    static const esp_vfs_fs_ops_t vfs = {
#if BTC_OPP_CLIENT_INCLUDED
        .write = opp_vfs_write,
#endif
        .close = opp_vfs_close,
#if BTC_OPP_SERVER_INCLUDED
        .read = opp_vfs_read,
#endif
    };

    if (esp_vfs_register_fs_with_id(&vfs, ESP_VFS_FLAG_STATIC, NULL, &s_opp_vfs.vfs_id) != ESP_OK) {
#if BTC_OPP_CLIENT_INCLUDED
        vEventGroupDelete(s_opp_vfs.tx_event_group);
        s_opp_vfs.tx_event_group = NULL;
#endif
        osi_mutex_free(&s_opp_vfs.mutex);
        return ESP_FAIL;
    }

    s_opp_vfs.registered = true;
    return ESP_OK;
}

static esp_err_t opp_vfs_unregister_internal(void)
{
    if (!s_opp_vfs.registered) {
        return ESP_OK;
    }

    osi_mutex_lock(&s_opp_vfs.mutex, OSI_MUTEX_MAX_TIMEOUT);
    for (int i = 0; i < OPP_VFS_MAX_SLOTS; i++) {
        if (s_opp_vfs.slots[i].used) {
            opp_vfs_free_slot(&s_opp_vfs.slots[i]);
        }
    }
    if (s_opp_vfs.vfs_id >= 0) {
        (void)esp_vfs_unregister_with_id(s_opp_vfs.vfs_id);
        s_opp_vfs.vfs_id = -1;
    }
    s_opp_vfs.registered = false;
    osi_mutex_unlock(&s_opp_vfs.mutex);

#if BTC_OPP_CLIENT_INCLUDED
    if (s_opp_vfs.tx_event_group) {
        vEventGroupDelete(s_opp_vfs.tx_event_group);
        s_opp_vfs.tx_event_group = NULL;
    }
#endif
    osi_mutex_free(&s_opp_vfs.mutex);
    return ESP_OK;
}

esp_err_t btc_opp_vfs_acquire(void)
{
    if (s_opp_vfs.refcount == 0) {
        esp_err_t err = opp_vfs_register_internal();
        if (err != ESP_OK) {
            return err;
        }
    }
    s_opp_vfs.refcount++;
    return ESP_OK;
}

esp_err_t btc_opp_vfs_release(void)
{
    if (s_opp_vfs.refcount <= 0) {
        return ESP_OK;
    }
    s_opp_vfs.refcount--;
    if (s_opp_vfs.refcount == 0) {
        return opp_vfs_unregister_internal();
    }
    return ESP_OK;
}

#endif /* BTC_OPP_INCLUDED */
