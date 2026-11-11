/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "btc_spp.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "bta/bta_jv_api.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "esp_spp_api.h"
#include "osi/list.h"
#include "freertos/ringbuf.h"
#include "osi/mutex.h"
#include "osi/alarm.h"
#include "osi/thread.h"
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "stack/port_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "btc/btc_task.h"
#include "stack/btu.h"

#if (defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE)

#define SLOT_WRITE_BIT(i) (1UL << (i - 1))
#define SLOT_CLOSE_BIT(i) (1UL << (i + MAX_RFC_PORTS - 1))
#define VFS_WRITE_TIMEOUT (40 * 1000)
#define VFS_CLOSE_TIMEOUT (20 * 1000)
#define SPP_TX_LOW_WM(sz) ((sz) / 2)
#define SPP_TX_RESUME_TO  (100)

typedef struct {
    bool peer_fc;         /* true if flow control is set based on peer's request */
    bool user_fc;         /* true if flow control is set based on user's request  */
    fixed_queue_t *queue; /* Queue of buffers waiting to be sent */
    uint32_t data_size;   /* Number of data bytes in the queue */
} slot_data_t;

typedef struct {
    bool connected;
    bool is_server;
    bool is_writing;
    bool upper_cong; /* TX ringbuf congested for the application; cleared at the low watermark */
    bool lower_cong; /* RFCOMM congested; stops BTC drain and is not reported upward */
    bool create_spp_record;
    uint8_t serial;
    uint8_t scn;
    uint8_t max_session;
    uint16_t mtu;
    uint16_t credit_rx;
    uint16_t write_data_len;
    uint16_t write_held; /* Bytes checked out of the ringbuf and not yet returned */
    uint32_t id;
    uint32_t sdp_handle;
    uint32_t rfc_handle;
    uint32_t rfc_port_handle;
    int fd;
    uint8_t *write_data;
    osi_alarm_t *close_alarm;
    void *alarm_arg;
    esp_spp_role_t role;
    esp_spp_sec_t security;
    esp_bd_addr_t addr;
    slot_data_t rx;
    RingbufHandle_t ringbuf_write;
    uint8_t service_uuid[16];
    char service_name[ESP_SPP_SERVER_NAME_MAX + 1];
} spp_slot_t;

typedef struct {
    uint16_t tx_buffer_size;
    spp_slot_t *spp_slots[MAX_RFC_PORTS + 1];
    uint32_t spp_slot_id;
    esp_spp_mode_t spp_mode;
    osi_alarm_t *tx_resume_alarm;
    esp_vfs_id_t spp_vfs_id;
} spp_local_param_t;

/*
 * The mutex and tx_event_group stay allocated across SPP and Bluedroid deinit.
 * spp_local_param may be freed while an API or VFS task is still running.
 * Deinit clears ready under mutex, so a late write locks, sees not-ready, and
 * returns. VFS write() waits on tx_event_group after unlocking, so that group
 * must not be deleted either.
 * tx_drain_evt lives for one Bluedroid session. osi_event_delete() does not free it,
 * so SPP deinit keeps the pointer. Bluedroid deinit frees every session event;
 * btc_spp_clear_tx_drain_evt() drops the pointer first.
 */
typedef struct {
    bool ready;
    bool tx_drain_bound;
    osi_mutex_t mutex;
    EventGroupHandle_t tx_event_group;
    struct osi_event *tx_drain_evt;
} spp_resident_t;

static spp_resident_t s_spp_resident = {0};

/*
 * Non-BTC tasks (API, VFS, BTU/BTA callbacks) that touch spp_local_param must:
 *   1. Optionally check is_spp_ready() before spp_lock() to avoid locking when
 *      SPP was never initialized.
 *   2. spp_lock().
 *   3. Check is_spp_ready() again under the lock before using spp_local_param.
 *
 * Deinit clears s_spp_resident.ready under this mutex on the BTC task, then may
 * free spp_local_param. Without the post-lock check, a thread that saw ready==true
 * before lock can still enter after clr_ready and hit a UAF.
 * BTC-task handlers need no post-lock check: DISABLE shares the BTC queue and
 * cannot run until the current handler returns.
 */
#define spp_lock()         osi_mutex_lock(&s_spp_resident.mutex, OSI_MUTEX_MAX_TIMEOUT)
#define spp_unlock()       osi_mutex_unlock(&s_spp_resident.mutex)
#define is_spp_ready()     (s_spp_resident.ready)
#define spp_set_ready()    do { spp_lock(); s_spp_resident.ready = true;  spp_unlock(); } while (0)
#define spp_clr_ready()    do { spp_lock(); s_spp_resident.ready = false; spp_unlock(); } while (0)

#if SPP_DYNAMIC_MEMORY == FALSE
static spp_local_param_t spp_local_param;
#else
static spp_local_param_t *spp_local_param_ptr;
#define spp_local_param (*spp_local_param_ptr)
#endif

static void btc_spp_vfs_register(void);
static void btc_spp_vfs_unregister(void);
static void spp_arm_tx_resume_under_lock(void);
static void spp_tx_drain_cb(void *arg);
static void spp_post_tx_drain(void);

static void spp_osi_free(void *p)
{
    osi_free(p);
}

static int init_slot_data(slot_data_t *slot_data, size_t queue_size)
{
    memset(slot_data, 0, sizeof(slot_data_t));
    if ((slot_data->queue = fixed_queue_new(queue_size)) == NULL) {
        return -1;
    }
    slot_data->data_size = 0;
    return 0;
}

static void free_slot_data(slot_data_t *slot_data)
{
    fixed_queue_free(slot_data->queue, spp_osi_free);
    slot_data->queue = NULL;
}

static spp_slot_t *spp_malloc_slot(void)
{
    uint8_t err_no = 0;
    spp_slot_t **slot = NULL;
    if (++spp_local_param.spp_slot_id == 0) {
        spp_local_param.spp_slot_id = 1;
    }
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = &spp_local_param.spp_slots[i];
        if ((*slot) == NULL) {
            if (((*slot) = (spp_slot_t *)osi_malloc(sizeof(spp_slot_t))) == NULL) {
                return NULL;
            }
            (*slot)->id = spp_local_param.spp_slot_id;
            (*slot)->serial = i;
            (*slot)->sdp_handle = 0;
            (*slot)->rfc_handle = 0;
            (*slot)->rfc_port_handle = 0;
            (*slot)->fd = -1;
            (*slot)->connected = false;
            (*slot)->create_spp_record = false;
            (*slot)->is_server = false;
            (*slot)->mtu = 0;
            (*slot)->credit_rx = BTA_JV_MAX_CREDIT_NUM;
            (*slot)->write_data = NULL;
            (*slot)->write_data_len = 0;
            (*slot)->write_held = 0;
            (*slot)->is_writing = false;
            (*slot)->upper_cong = false;
            (*slot)->lower_cong = false;
            (*slot)->close_alarm = NULL;
            (*slot)->alarm_arg = NULL;
            (*slot)->ringbuf_write = NULL;
            /* clear the old event bits */
            if (s_spp_resident.tx_event_group) {
                xEventGroupClearBits(s_spp_resident.tx_event_group, SLOT_WRITE_BIT(i) | SLOT_CLOSE_BIT(i));
            }

            if (init_slot_data(&(*slot)->rx, QUEUE_SIZE_MAX)) {
                BTC_TRACE_ERROR("%s unable to malloc rx queue!", __func__);
                err_no = 1;
                goto err;
            }
            if (((*slot)->ringbuf_write = xRingbufferCreate(spp_local_param.tx_buffer_size, RINGBUF_TYPE_BYTEBUF)) == NULL) {
                BTC_TRACE_ERROR("%s write ringbuffer create error!", __func__);
                err_no = 2;
                goto err;
            }
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                if (esp_vfs_register_fd(spp_local_param.spp_vfs_id, &(*slot)->fd) != ESP_OK) {
                    BTC_TRACE_ERROR("%s unable to register fd!", __func__);
                    err_no = 3;
                    goto err;
                }
            }
            return (*slot);
        }
    }

    return NULL;
err:
    switch (err_no) {
        case 3:
            vRingbufferDelete((*slot)->ringbuf_write);
            (*slot)->ringbuf_write = NULL;
            /* fall through */
        case 2:
            free_slot_data(&(*slot)->rx);
            /* fall through */
        case 1:
            osi_free((*slot));
            (*slot) = NULL;
            break;
        default:
            break;
    }
    return (*slot);
}

static spp_slot_t *spp_find_slot_by_id(uint32_t id)
{
    spp_slot_t *slot = NULL;
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot != NULL && slot->id == id) {
            return slot;
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_handle(uint32_t handle)
{
#if SPP_DYNAMIC_MEMORY == TRUE
        if (!spp_local_param_ptr) {
            return NULL;
        }
#endif

    spp_slot_t *slot = NULL;
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot != NULL && slot->rfc_handle == handle) {
            return slot;
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_fd(int fd)
{
#if SPP_DYNAMIC_MEMORY == TRUE
        if (!spp_local_param_ptr) {
            return NULL;
        }
#endif

    spp_slot_t *slot = NULL;
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot != NULL && slot->fd == fd) {
            return slot;
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_scn(uint32_t scn)
{
#if SPP_DYNAMIC_MEMORY == TRUE
        if (!spp_local_param_ptr) {
            return NULL;
        }
#endif

    spp_slot_t *slot = NULL;
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot != NULL && slot->is_server && slot->scn == (uint8_t)scn) {
            return slot;
        }
    }
    return NULL;
}

static void close_timeout_handler(void *arg)
{
    btc_msg_t msg;
    bt_status_t status;
    spp_slot_t *slot = (spp_slot_t *)arg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = BTA_JV_RFCOMM_CLOSE_EVT;

    status = btc_transfer_context(&msg, slot->alarm_arg, sizeof(tBTA_JV), NULL, NULL);

    if (slot->alarm_arg) {
        osi_free(slot->alarm_arg);
        slot->alarm_arg = NULL;
    }

    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed", __func__);
    }
}

static void spp_free_slot(spp_slot_t *slot)
{
    if (!slot) {
        return;
    }
    spp_local_param.spp_slots[slot->serial] = NULL;
    if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
        (void) esp_vfs_unregister_fd(spp_local_param.spp_vfs_id, slot->fd);
        xEventGroupSetBits(s_spp_resident.tx_event_group, SLOT_CLOSE_BIT(slot->serial));
    }
    vRingbufferDelete(slot->ringbuf_write);
    free_slot_data(&slot->rx);
    if (slot->close_alarm) {
        osi_alarm_free(slot->close_alarm);
        if (slot->alarm_arg) {
            osi_free(slot->alarm_arg);
            slot->alarm_arg = NULL;
        }
    }
    osi_free(slot);
}

static void spp_free_pending_slots(void)
{
    spp_slot_t *slot = NULL;
    spp_lock();
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot) {
            BTC_TRACE_WARNING("%s found slot(rfc_handle=0x%x) pending to close, close it now!", __func__, slot->rfc_handle);
            spp_free_slot(slot);
        }
    }
    spp_unlock();
}

static size_t spp_get_tx_items_waiting_under_lock(spp_slot_t *slot)
{
    size_t items_waiting = 0;
    if (slot) {
        if (slot->ringbuf_write) {
            vRingbufferGetInfo(slot->ringbuf_write, NULL, NULL, NULL, NULL, &items_waiting);
        }
        /* A checked-out chunk is no longer in xItemsWaiting, but its space stays reserved. */
        items_waiting += slot->write_held;
    }
    return items_waiting;
}

/*
 * false: write_data is held and BTA_JvRfcommWrite did not accept it; the caller retries.
 * true: otherwise.
 * Caller holds spp_lock across the BTA post.
 */
static bool spp_drain_tx_under_lock(spp_slot_t *slot)
{
    if (!slot || !slot->connected || slot->is_writing || slot->lower_cong || !slot->ringbuf_write) {
        return true;
    }

    bool ret = true;
    uint8_t *data = slot->write_data;
    size_t item_size = slot->write_data_len;

    if (!data) {
        data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, slot->mtu);
        if (data) {
            /* Keep the original checkout size. A partial accept shrinks write_data_len only. */
            slot->write_held = (uint16_t)item_size;
        }
    }

    if (data) {
        slot->write_data = data;
        slot->write_data_len = item_size;
        if (BTA_JvRfcommWrite(slot->rfc_handle, slot->id, item_size, data) == BTA_JV_SUCCESS) {
            slot->is_writing = true;
        } else {
            ret = false;
        }
    }

    return ret;
}

/* Delivered on the BTC task by btc_alarm_handler. Re-post the shared drain only.
 * BTA writes and freeing this alarm stay in spp_tx_drain_cb, which runs on the BTC task.
 */
static void spp_tx_resume_cb(void *arg)
{
    (void)arg;
    spp_post_tx_drain();
}

/*Caller holds spp_lock. Created on first use and freed when no slot needs a resume. */
static void spp_arm_tx_resume_under_lock(void)
{
    if (!is_spp_ready()) {
        return;
    }
    if (!spp_local_param.tx_resume_alarm) {
        spp_local_param.tx_resume_alarm = osi_alarm_new("spp_tx_resume", spp_tx_resume_cb, NULL, SPP_TX_RESUME_TO);
        if (!spp_local_param.tx_resume_alarm) {
            BTC_TRACE_ERROR("%s create tx_resume_alarm failed\n", __func__);
            return;
        }
    } else {
        osi_alarm_cancel(spp_local_param.tx_resume_alarm);
    }
    osi_alarm_set(spp_local_param.tx_resume_alarm, SPP_TX_RESUME_TO);
}

/* One BTC work item for every slot. Runs on the BTC task. */
static void spp_tx_drain_cb(void *arg)
{
    (void)arg;
    if (!is_spp_ready()) {
        return;
    }
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        return;
    }
    bool need_resume = false;
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (!spp_drain_tx_under_lock(spp_local_param.spp_slots[i])) {
            need_resume = true;
        }
    }
    if (need_resume) {
        spp_arm_tx_resume_under_lock();
    } else if (spp_local_param.tx_resume_alarm) {
        osi_alarm_free(spp_local_param.tx_resume_alarm);
        spp_local_param.tx_resume_alarm = NULL;
    }
    spp_unlock();
}

static inline void btc_spp_cb_to_app(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    esp_spp_cb_t btc_spp_cb = (esp_spp_cb_t)btc_profile_cb_get(BTC_PID_SPP);
    if (btc_spp_cb) {
        btc_spp_cb(event, param);
    }
}

static void btc_create_server_fail_cb(void)
{
    esp_spp_cb_param_t param;
    param.start.status = ESP_SPP_FAILURE;
    param.start.handle = 0;
    param.start.sec_id = 0;
    param.start.scn = BTC_SPP_INVALID_SCN;
    param.start.use_co = FALSE;
    btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
}

static void *btc_spp_rfcomm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;
    void *new_user_data = NULL;
    uint32_t id = (uintptr_t)user_data, id_temp = 0;
    spp_slot_t *slot = NULL, *slot_new = NULL;

    if (!is_spp_ready()) {
        BTC_TRACE_WARNING("%s SPP have been deinit, incoming events ignore!\n", __func__);
        return new_user_data;
    }
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        BTC_TRACE_WARNING("%s SPP have been deinit, incoming events ignore!\n", __func__);
        return new_user_data;
    }
    switch (event) {
    case BTA_JV_RFCOMM_START_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            p_data->rfc_start.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->rfc_handle = p_data->rfc_start.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_start.handle);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        slot = p_data->rfc_srv_open.handle ? spp_find_slot_by_id(id) : spp_find_slot_by_scn((uint32_t)user_data);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            p_data->rfc_srv_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }

        if (!p_data->rfc_srv_open.handle) {
            /* match with the exist server solt */
            slot->rfc_handle = p_data->rfc_srv_open.new_listen_handle;
            slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(slot->rfc_handle);
        } else {
            slot_new = spp_malloc_slot();
            if (!slot_new) {
                BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
                p_data->rfc_srv_open.status = ESP_SPP_NO_RESOURCE;
                break;
            }
            slot_new->connected = true;
            slot_new->security = slot->security;
            slot_new->role = slot->role;
            slot_new->scn = slot->scn;
            slot_new->max_session = slot->max_session;
            strcpy(slot_new->service_name, slot->service_name);
            slot_new->sdp_handle = slot->sdp_handle;
            slot_new->mtu = p_data->rfc_srv_open.peer_mtu;
            slot_new->create_spp_record = slot->create_spp_record;
            slot_new->rfc_handle = p_data->rfc_srv_open.handle;
            slot_new->rfc_port_handle = BTA_JvRfcommGetPortHdl(slot_new->rfc_handle);
            BTA_JvSetPmProfile(p_data->rfc_srv_open.handle, BTA_JV_PM_ALL, BTA_JV_CONN_OPEN);

            if (p_data->rfc_srv_open.new_listen_handle) {
                slot->rfc_handle = p_data->rfc_srv_open.new_listen_handle;
                slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(slot->rfc_handle);
            } else {
                /* means lower layer can alloc port */
                slot->rfc_handle = 0;
                slot->rfc_port_handle = 0;
            }
            /* swap slot id */
            id_temp = slot->id;
            slot->id = slot_new->id;
            slot_new->id = id_temp;
        }
        new_user_data = (void *)(uintptr_t)slot->id;
        break;
    case BTA_JV_RFCOMM_CL_INIT_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            p_data->rfc_cl_init.status = ESP_SPP_FAILURE;
            break;
        }

        if (p_data->rfc_cl_init.status == BTA_JV_SUCCESS) {
            slot->rfc_handle = p_data->rfc_cl_init.handle;
        } else {
            spp_free_slot(slot);
        }
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            p_data->rfc_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->connected = true;
        slot->rfc_handle = p_data->rfc_open.handle;
        slot->mtu = p_data->rfc_open.peer_mtu;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_open.handle);
        BTA_JvSetPmProfile(p_data->rfc_open.handle, BTA_JV_PM_ID_1, BTA_JV_CONN_OPEN);
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        break;
    case BTA_JV_FREE_SCN_EVT:
        if (user_data) {
            id = ((tBTA_JV_FREE_SCN_USER_DATA *)user_data)->slot_id;
            slot = spp_find_slot_by_id(id);
            if (slot) {
                spp_free_slot(slot);
            } else {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                p_data->free_scn.status = ESP_SPP_NO_CONNECTION;
            }
            osi_free(user_data);
        }
        break;
    default:
        break;
    }
    spp_unlock();
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = event;


    status = btc_transfer_context(&msg, p_data,
                                  sizeof(tBTA_JV), NULL, NULL);

    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed", __func__);
    }
    return new_user_data;
}

static void btc_spp_dm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg = {0};

    uint32_t id = (uintptr_t)user_data;
    spp_slot_t *slot = NULL;
    switch (event) {
    case BTA_JV_GET_SCN_EVT:
        spp_lock();
        if (!is_spp_ready()) {
            spp_unlock();
            break;
        }
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            spp_unlock();
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            break;
        }
        if (p_data->scn == 0) {
            spp_free_slot(slot);
            spp_unlock();
            btc_create_server_fail_cb();
            BTC_TRACE_ERROR("%s unable to get scn, start server fail!", __func__);
            break;
        }

        slot->scn = p_data->scn;
        if (slot->create_spp_record) {
            BTA_JvCreateRecordByUser(slot->service_name, slot->scn, (void *)slot->id);
        } else {
            slot->sdp_handle = 0xffff;
            BTA_JvRfcommStartServer(slot->security, slot->role, slot->scn,
                                    slot->max_session, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        }

        spp_unlock();
        break;
    case BTA_JV_CREATE_RECORD_EVT:
        spp_lock();
        if (!is_spp_ready()) {
            spp_unlock();
            break;
        }
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            spp_unlock();
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            break;
        }
        if (p_data->create_rec.status == BTA_JV_SUCCESS) {
            slot->sdp_handle = p_data->create_rec.handle;
            BTA_JvRfcommStartServer(slot->security, slot->role, slot->scn,
                                    slot->max_session, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        } else {
            BTC_TRACE_ERROR("%s unable to create record, start server fail!", __func__);
            tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
            if (user_data) {
                user_data->server_status = BTA_JV_SERVER_START_FAILED;
                user_data->slot_id = slot->id;
            } else {
                BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                assert(0);
            }
            tBTA_JV_STATUS status = BTA_JvFreeChannel(slot->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                                      (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
            if (status != BTA_JV_SUCCESS) {
                osi_free(user_data);
                spp_free_slot(slot);
            }
            spp_unlock();
            btc_create_server_fail_cb();
            break;
        }
        spp_unlock();
        break;
    default:
        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_SPP;
        msg.act = event;

        status = btc_transfer_context(&msg, p_data, sizeof(tBTA_JV), NULL, NULL);

        if (status != BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
        }
        break;
    }
}

static void btc_spp_init(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP has been initiated, shall uninit first!", __func__);
            ret = ESP_SPP_NEED_DEINIT;
            break;
        }

        if (s_spp_resident.mutex == NULL && osi_mutex_new(&s_spp_resident.mutex) != 0) {
            BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }

#if SPP_DYNAMIC_MEMORY == TRUE
        if ((spp_local_param_ptr = (spp_local_param_t *)osi_malloc(sizeof(spp_local_param_t))) == NULL) {
            BTC_TRACE_ERROR("%s malloc failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        memset((void *)spp_local_param_ptr, 0, sizeof(spp_local_param_t));
#else
        memset(&spp_local_param, 0, sizeof(spp_local_param));
#endif

        if (s_spp_resident.tx_event_group == NULL) {
            s_spp_resident.tx_event_group = xEventGroupCreate();
            if (s_spp_resident.tx_event_group == NULL) {
                BTC_TRACE_ERROR("%s create tx_event_group failed\n", __func__);
#if SPP_DYNAMIC_MEMORY == TRUE
                osi_free(spp_local_param_ptr);
                spp_local_param_ptr = NULL;
#endif
                ret = ESP_SPP_NO_RESOURCE;
                break;
            }
        } else {
            xEventGroupClearBits(s_spp_resident.tx_event_group, (EventBits_t)((1UL << (2 * MAX_RFC_PORTS)) - 1));
        }
        /* One event per Bluedroid session. SPP re-init reuses it; Bluedroid deinit clears it. */
        if (s_spp_resident.tx_drain_evt == NULL) {
            s_spp_resident.tx_drain_evt = osi_event_create(spp_tx_drain_cb, NULL);
            if (s_spp_resident.tx_drain_evt == NULL) {
                BTC_TRACE_ERROR("%s create tx_drain_evt failed\n", __func__);
#if SPP_DYNAMIC_MEMORY == TRUE
                osi_free(spp_local_param_ptr);
                spp_local_param_ptr = NULL;
#endif
                ret = ESP_SPP_NO_RESOURCE;
                break;
            }
        }
        if (!s_spp_resident.tx_drain_bound) {
            if (!osi_event_bind(s_spp_resident.tx_drain_evt, btc_get_current_thread(), 0)) {
                BTC_TRACE_ERROR("%s bind tx_drain_evt failed\n", __func__);
#if SPP_DYNAMIC_MEMORY == TRUE
                osi_free(spp_local_param_ptr);
                spp_local_param_ptr = NULL;
#endif
                ret = ESP_SPP_NO_RESOURCE;
                break;
            }
            s_spp_resident.tx_drain_bound = true;
        }
        if (arg->init.mode == ESP_SPP_MODE_VFS) {
            spp_local_param.spp_vfs_id = -1;
        }
        spp_local_param.spp_mode = arg->init.mode;
        spp_local_param.spp_slot_id = 0;
        spp_local_param.tx_buffer_size = arg->init.tx_buffer_size;

        spp_set_ready();
        BTA_JvEnable((tBTA_JV_DM_CBACK *)btc_spp_dm_inter_cb);
        BTA_JvRfcommConfig(arg->init.enable_l2cap_ertm);
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.init.status = ret;
        btc_spp_cb_to_app(ESP_SPP_INIT_EVT, &param);
    }
}

static void btc_spp_uninit(void)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP has not been initiated, shall init first!", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_lock();
        // first, remove all connection
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->is_server) {
                BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (void *)spp_local_param.spp_slots[i]->id);
            }
        }
        // second, remove all server
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->is_server) {
                if (spp_local_param.spp_slots[i]->sdp_handle > 0) {
                    BTA_JvDeleteRecord(spp_local_param.spp_slots[i]->sdp_handle);
                }

                if (spp_local_param.spp_slots[i]->rfc_handle > 0) {
                    BTA_JvRfcommStopServer(spp_local_param.spp_slots[i]->rfc_handle,
                                           (void *)spp_local_param.spp_slots[i]->id);
                }

                tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
                if (user_data) {
                    user_data->server_status = BTA_JV_SERVER_RUNNING;
                    user_data->slot_id = spp_local_param.spp_slots[i]->id;
                } else {
                    BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                    assert(0);
                }
                tBTA_JV_STATUS status = BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                                          (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
                if (status != BTA_JV_SUCCESS) {
                    osi_free(user_data);
                }
            }
        }
        BTA_JvDisable((tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb);
        spp_unlock();
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.uninit.status = ret;
        btc_spp_cb_to_app(ESP_SPP_UNINIT_EVT, &param);
    }
}

static void btc_spp_start_discovery(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        tBTA_JV_STATUS status = BTA_JvStartDiscovery(arg->start_discovery.bd_addr, arg->start_discovery.num_uuid, arg->start_discovery.p_uuid_list, NULL);
        if (status != BTA_JV_SUCCESS) {
            BTC_TRACE_ERROR("%s SPP failed to start discovery\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
        }
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param = { 0 };
        param.disc_comp.status = ret;
        param.disc_comp.scn_num = 0;
        memset(param.disc_comp.scn, 0xff, ESP_SPP_MAX_SCN);
        btc_spp_cb_to_app(ESP_SPP_DISCOVERY_COMP_EVT, &param);
    }
}

static void btc_spp_connect(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_lock();
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            spp_unlock();
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        slot->security = arg->connect.sec_mask;
        slot->role = arg->connect.role;
        slot->scn = arg->connect.remote_scn;

        memcpy(slot->addr, arg->connect.peer_bd_addr, ESP_BD_ADDR_LEN);
        tBTA_JV_STATUS status = BTA_JvRfcommConnect(arg->connect.sec_mask, arg->connect.role, arg->connect.remote_scn,
                                                    arg->connect.peer_bd_addr, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        if (status != BTA_JV_SUCCESS) {
            spp_free_slot(slot);
            ret = ESP_SPP_FAILURE;
            spp_unlock();
            break;
        }
        spp_unlock();
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.open.status = ret;
        param.open.handle = 0;
        param.open.fd = -1;
        memset(param.open.rem_bda, 0, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_OPEN_EVT, &param);
    }
}

static void btc_spp_disconnect(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_slot_t *slot = NULL;
        spp_lock();
        slot = spp_find_slot_by_handle(arg->disconnect.handle);
        if (!slot || !slot->connected) {
            spp_unlock();
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot! disconnect fail!", __func__);
            } else {
                BTC_TRACE_ERROR("%s RFCOMM has been disconnected already!", __func__);
            }
            ret = ESP_SPP_NO_CONNECTION;
            break;
        }
        if (BTA_JvRfcommClose(arg->disconnect.handle, (void *)slot->id) != BTA_JV_SUCCESS) {
            spp_unlock();
            ret = ESP_SPP_FAILURE;
            break;
        }
        spp_unlock();
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.close.status = ret;
        param.close.port_status = PORT_ERR_MAX;
        param.close.handle = arg->disconnect.handle;
        param.close.async = FALSE;
        btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
    }
}

static void btc_spp_start_srv(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_lock();
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            spp_unlock();
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        /**
         * make this slot become a listening slot
         */
        slot->is_server = true;
        slot->create_spp_record = arg->start_srv.create_spp_record;
        slot->security = arg->start_srv.sec_mask;
        slot->role = arg->start_srv.role;
        slot->scn = arg->start_srv.local_scn;
        slot->max_session = arg->start_srv.max_session;
        strcpy(slot->service_name, arg->start_srv.name);

        tBTA_JV_STATUS status = BTA_JvGetChannelId(BTA_JV_CONN_TYPE_RFCOMM, (void *)slot->id, arg->start_srv.local_scn);
        if (status != BTA_JV_SUCCESS) {
            spp_free_slot(slot);
            spp_unlock();
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        spp_unlock();
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.start.status = ret;
        param.start.handle = 0;
        param.start.sec_id = 0;
        param.start.scn = BTC_SPP_INVALID_SCN;
        param.start.use_co = FALSE;
        btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
    }
}

static void btc_spp_stop_srv(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    bool is_remove_all = false;
    uint8_t i, j, srv_cnt = 0;
    uint8_t *srv_scn_arr = NULL;
    if (arg->stop_srv.scn == BTC_SPP_INVALID_SCN) {
        is_remove_all = true;
    }

    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        srv_scn_arr = osi_malloc(MAX_RFC_PORTS);
        if (srv_scn_arr == NULL) {
            BTC_TRACE_ERROR("%s malloc srv_scn_arr failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }

        spp_lock();
        // [1] find all unconnected server
        for (i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->connected &&
                spp_local_param.spp_slots[i]->sdp_handle > 0) {
                if (is_remove_all) {
                    srv_scn_arr[srv_cnt++] = spp_local_param.spp_slots[i]->scn;
                } else if (spp_local_param.spp_slots[i]->scn == arg->stop_srv.scn) {
                    srv_scn_arr[srv_cnt++] = spp_local_param.spp_slots[i]->scn;
                    break;
                }
            }
        }
        if (srv_cnt == 0) {
            if (is_remove_all) {
                BTC_TRACE_ERROR("%s can not find any server!\n", __func__);
            } else {
                BTC_TRACE_ERROR("%s can not find server:%d!\n", __func__, arg->stop_srv.scn);
            }
            ret = ESP_SPP_NO_SERVER;
            spp_unlock();
            break;
        }

        // [2] remove all local related connection
        for (j = 0; j < srv_cnt; j++) {
            for (i = 1; i <= MAX_RFC_PORTS; i++) {
                if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->is_server &&
                    spp_local_param.spp_slots[i]->sdp_handle > 0 &&
                    spp_local_param.spp_slots[i]->scn == srv_scn_arr[j]) {
                    BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (void *)spp_local_param.spp_slots[i]->id);
                }
            }
        }

        // [3] remove all server
        for (j = 0; j < srv_cnt; j++) {
            for (i = 1; i <= MAX_RFC_PORTS; i++) {
                if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->is_server &&
                    spp_local_param.spp_slots[i]->sdp_handle > 0 &&
                    spp_local_param.spp_slots[i]->scn == srv_scn_arr[j]) {
                    if (spp_local_param.spp_slots[i]->sdp_handle > 0 && spp_local_param.spp_slots[i]->create_spp_record) {
                        BTA_JvDeleteRecord(spp_local_param.spp_slots[i]->sdp_handle);
                    }

                    if (spp_local_param.spp_slots[i]->rfc_handle > 0) {
                        BTA_JvRfcommStopServer(spp_local_param.spp_slots[i]->rfc_handle,
                                               (void *)spp_local_param.spp_slots[i]->id);
                    }

                    tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
                    if (user_data) {
                        user_data->server_status = BTA_JV_SERVER_RUNNING;
                        user_data->slot_id = spp_local_param.spp_slots[i]->id;
                    } else {
                        BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                        assert(0);
                    }
                    tBTA_JV_STATUS status = BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                                              (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
                    if (status != BTA_JV_SUCCESS) {
                        osi_free(user_data);
                    }
                }
            }
        }
        spp_unlock();
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.srv_stop.status = ret;
        param.srv_stop.scn = BTC_SPP_INVALID_SCN;
        btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
    }

    if (srv_scn_arr) {
        osi_free(srv_scn_arr);
        srv_scn_arr = NULL;
    }
}

void btc_spp_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_spp_args_t *dst = (btc_spp_args_t *) p_dest;
    btc_spp_args_t *src = (btc_spp_args_t *) p_src;

    switch (msg->act) {
    case BTC_SPP_ACT_START_DISCOVERY:
        dst->start_discovery.p_uuid_list = (tSDP_UUID *)osi_malloc(src->start_discovery.num_uuid * sizeof(tSDP_UUID));
        if (dst->start_discovery.p_uuid_list) {
            memcpy(dst->start_discovery.p_uuid_list, src->start_discovery.p_uuid_list, src->start_discovery.num_uuid * sizeof(tSDP_UUID));
        } else if (src->start_discovery.num_uuid == 0) {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        } else {
            BTC_TRACE_ERROR("%s %d osi_malloc failed\n", __func__, msg->act);
        }
        break;
    default:
        break;
    }
}

void btc_spp_arg_deep_free(btc_msg_t *msg)
{
    btc_spp_args_t *arg = (btc_spp_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_SPP_ACT_START_DISCOVERY:
        if (arg->start_discovery.p_uuid_list) {
            osi_free(arg->start_discovery.p_uuid_list);
            arg->start_discovery.p_uuid_list = NULL;
        }
        break;
    default:
        break;
    }
}

void btc_spp_call_handler(btc_msg_t *msg)
{
    btc_spp_args_t *arg = (btc_spp_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_SPP_ACT_INIT:
        btc_spp_init(arg);
        break;
    case BTC_SPP_ACT_UNINIT:
        btc_spp_uninit();
        break;
    case BTC_SPP_ACT_START_DISCOVERY:
        btc_spp_start_discovery(arg);
        break;
    case BTC_SPP_ACT_CONNECT:
        btc_spp_connect(arg);
        break;
    case BTC_SPP_ACT_DISCONNECT:
        btc_spp_disconnect(arg);
        break;
    case BTC_SPP_ACT_START_SRV:
        btc_spp_start_srv(arg);
        break;
    case BTC_SPP_ACT_STOP_SRV:
        btc_spp_stop_srv(arg);
        break;
    case BTC_SPP_ACT_VFS_REGISTER:
        btc_spp_vfs_register();
        break;
    case BTC_SPP_ACT_VFS_UNREGISTER:
        btc_spp_vfs_unregister();
        break;
    default:
        BTC_TRACE_ERROR("%s: Unhandled event (%d)!\n", __FUNCTION__, msg->act);
        break;
    }
    btc_spp_arg_deep_free(msg);
}

void btc_spp_cb_handler(btc_msg_t *msg)
{
    esp_spp_cb_param_t param = {0};
    tBTA_JV *p_data = (tBTA_JV *)msg->arg;
    spp_slot_t *slot = NULL;
    uint8_t serial = 0;
    uint8_t event = msg->act;

    switch (event) {
    case BTA_JV_ENABLE_EVT:
        param.init.status = p_data->status;
        btc_spp_cb_to_app(ESP_SPP_INIT_EVT, &param);
        break;
    case BTA_JV_DISCOVERY_COMP_EVT:
        param.disc_comp.status = p_data->disc_comp.status;
        if (param.disc_comp.status == BTA_JV_SUCCESS) {
            param.disc_comp.scn_num = p_data->disc_comp.scn_num;
            memcpy(param.disc_comp.scn, p_data->disc_comp.scn, p_data->disc_comp.scn_num);
            memcpy(param.disc_comp.service_name, p_data->disc_comp.service_name,
                p_data->disc_comp.scn_num * sizeof(const char *));
        }
        btc_spp_cb_to_app(ESP_SPP_DISCOVERY_COMP_EVT, &param);
        break;
    case BTA_JV_RFCOMM_CL_INIT_EVT:
        param.cl_init.status = p_data->rfc_cl_init.status;
        param.cl_init.handle = p_data->rfc_cl_init.handle;
        param.cl_init.sec_id = p_data->rfc_cl_init.sec_id;
        param.cl_init.use_co = p_data->rfc_cl_init.use_co;
        btc_spp_cb_to_app(ESP_SPP_CL_INIT_EVT, &param);
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        do {
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                spp_lock();
                slot = spp_find_slot_by_handle(p_data->rfc_open.handle);
                if (!slot) {
                    spp_unlock();
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                    param.open.status = ESP_SPP_NO_CONNECTION;
                    break;
                }
                param.open.fd = slot->fd;
                spp_unlock();
            } else {
                param.open.fd = -1;
            }
            param.open.status = p_data->rfc_open.status;
        } while (0);
        param.open.handle = p_data->rfc_open.handle;
        memcpy(param.open.rem_bda, p_data->rfc_open.rem_bda, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_OPEN_EVT, &param);
        break;
    case BTA_JV_RFCOMM_START_EVT:
        param.start.status = p_data->rfc_start.status;
        param.start.handle = p_data->rfc_start.handle;
        param.start.sec_id = p_data->rfc_start.sec_id;
        param.start.scn = p_data->rfc_start.scn;
        param.start.use_co = p_data->rfc_start.use_co;
        btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        if (p_data->rfc_srv_open.handle) {
            do {
                if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                    spp_lock();
                    slot = spp_find_slot_by_handle(p_data->rfc_srv_open.handle);
                    if (!slot) {
                        spp_unlock();
                        BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                        param.srv_open.status = ESP_SPP_NO_CONNECTION;
                        break;
                    }
                    param.srv_open.fd = slot->fd;
                    spp_unlock();
                } else {
                    param.srv_open.fd = -1;
                }
                param.srv_open.status = p_data->rfc_srv_open.status;
            } while (0);
            param.srv_open.handle = p_data->rfc_srv_open.handle;
            param.srv_open.new_listen_handle = p_data->rfc_srv_open.new_listen_handle;
            memcpy(param.srv_open.rem_bda, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
            btc_spp_cb_to_app(ESP_SPP_SRV_OPEN_EVT, &param);
        }
        break;
    case BTA_JV_RFCOMM_WRITE_EVT: {
        bool report_write = false;
        bool report_cong = false;
        bool is_cb_mode = false;
        bool write_cong = false;
        int accepted_len = p_data->rfc_write.len;
        spp_lock();
        is_cb_mode = (spp_local_param.spp_mode == ESP_SPP_MODE_CB);
        do {
            slot = spp_find_slot_by_id(p_data->rfc_write.req_id);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!, handle:%d", __func__, p_data->rfc_write.handle);
                break;
            }

            slot->lower_cong = p_data->rfc_write.cong;
            slot->is_writing = false;

            if (p_data->rfc_write.status == BTA_JV_SUCCESS) {
                if (accepted_len == slot->write_data_len) {
                    vRingbufferReturnItem(slot->ringbuf_write, slot->write_data);
                    slot->write_data = NULL;
                    slot->write_data_len = 0;
                    slot->write_held = 0;
                } else {
                    slot->write_data += accepted_len;
                    slot->write_data_len -= accepted_len;
                }

                if (accepted_len > 0) {
                    size_t items_waiting = spp_get_tx_items_waiting_under_lock(slot);
                    if (!is_cb_mode) {
                        if (spp_local_param.tx_buffer_size > items_waiting) {
                            xEventGroupSetBits(s_spp_resident.tx_event_group, SLOT_WRITE_BIT(slot->serial));
                        }
                    } else {
                        report_write = true;
                        write_cong = slot->upper_cong;
                        if (slot->upper_cong && items_waiting <= SPP_TX_LOW_WM(spp_local_param.tx_buffer_size)) {
                            slot->upper_cong = false;
                            report_cong = true;
                        }
                    }
                }
            }

            if (accepted_len == 0 && !p_data->rfc_write.old_cong && !slot->lower_cong) {
                spp_arm_tx_resume_under_lock();
            } else if (!spp_drain_tx_under_lock(slot)) {
                spp_arm_tx_resume_under_lock();
            }
        } while (0);
        spp_unlock();
        if (report_write) {
            param.write.status = ESP_SPP_SUCCESS;
            param.write.handle = p_data->rfc_write.handle;
            param.write.len = accepted_len;
            param.write.cong = write_cong;
            btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
        }
        if (report_cong) {
            param.cong.status = ESP_SPP_SUCCESS;
            param.cong.handle = p_data->rfc_write.handle;
            param.cong.cong = false;
            btc_spp_cb_to_app(ESP_SPP_CONG_EVT, &param);
        }
        break;
    }
    case BTA_JV_RFCOMM_CLOSE_EVT:
        param.close.status = BTA_JV_SUCCESS;
        param.close.port_status = p_data->rfc_close.port_status;
        param.close.handle = p_data->rfc_close.handle;
        param.close.async = p_data->rfc_close.async;
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
            spp_lock();
            slot = spp_find_slot_by_id((uint32_t)p_data->rfc_close.user_data);
            if (!slot) {
                param.close.status = ESP_SPP_NO_CONNECTION;
                spp_unlock();
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            spp_free_slot(slot);
            spp_unlock();
            btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
        } else {
            bool need_call = true;
            do {
                spp_lock();
                slot = spp_find_slot_by_id((uint32_t)p_data->rfc_close.user_data);
                if (!slot) {
                    param.close.status = ESP_SPP_NO_CONNECTION;
                    spp_unlock();
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                    break;
                }
                // if rx still has data, delay free slot
                if (slot->close_alarm == NULL && slot->rx.queue && fixed_queue_length(slot->rx.queue) > 0) {
                    tBTA_JV *p_arg = NULL;
                    if ((p_arg = osi_malloc(sizeof(tBTA_JV))) == NULL) {
                        param.close.status = ESP_SPP_NO_RESOURCE;
                        spp_unlock();
                        BTC_TRACE_ERROR("%s unable to malloc slot close_alarm arg!", __func__);
                        break;
                    }
                    memcpy(p_arg, p_data, sizeof(tBTA_JV));
                    slot->alarm_arg = (void *)p_arg;
                    if ((slot->close_alarm =
                             osi_alarm_new("slot", close_timeout_handler, (void *)slot, VFS_CLOSE_TIMEOUT)) == NULL) {
                        osi_free(p_arg);
                        slot->alarm_arg = NULL;
                        param.close.status = ESP_SPP_NO_RESOURCE;
                        spp_unlock();
                        BTC_TRACE_ERROR("%s unable to malloc slot close_alarm!", __func__);
                        break;
                    }
                    if (osi_alarm_set(slot->close_alarm, VFS_CLOSE_TIMEOUT) != OSI_ALARM_ERR_PASS) {
                        osi_free(p_arg);
                        slot->alarm_arg = NULL;
                        osi_alarm_free(slot->close_alarm);
                        param.close.status = ESP_SPP_BUSY;
                        spp_unlock();
                        BTC_TRACE_ERROR("%s set slot close_alarm failed!", __func__);
                        break;
                    }
                    BTC_TRACE_WARNING("%s slot rx data will be discard in %d milliseconds!",
                                      __func__, VFS_CLOSE_TIMEOUT);
                    slot->connected = false;
                    need_call = false;
                }
                spp_unlock();
            } while (0);

            if (need_call) {
                btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
                spp_lock();
                spp_free_slot(slot);
                spp_unlock();
            }
        }
        break;
    case BTA_JV_RFCOMM_CONG_EVT: {
        bool report_cong = false;
        spp_lock();
        slot = spp_find_slot_by_handle(p_data->rfc_cong.handle);
        if (!slot) {
            spp_unlock();
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            break;
        }
        slot->lower_cong = p_data->rfc_cong.cong;
        if (!slot->lower_cong) {
            if (!spp_drain_tx_under_lock(slot)) {
                spp_arm_tx_resume_under_lock();
            }
            if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
                if (slot->upper_cong &&
                    (spp_get_tx_items_waiting_under_lock(slot) <= SPP_TX_LOW_WM(spp_local_param.tx_buffer_size))) {
                    slot->upper_cong = false;
                    report_cong = true;
                }
            }
        }
        spp_unlock();

        /* ESP_SPP_CONG_EVT reports upper_cong, not lower_cong. Always uncongest here. */
        if (report_cong) {
            param.cong.status = ESP_SPP_SUCCESS;
            param.cong.handle = p_data->rfc_cong.handle;
            param.cong.cong = false;
            btc_spp_cb_to_app(ESP_SPP_CONG_EVT, &param);
        }
        break;
    }
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        do {
            BT_HDR *p_buf;
            UINT16 count = 0;
            spp_lock();
            slot = spp_find_slot_by_handle(p_data->data_ind.handle);
            if (!slot) {
                spp_unlock();
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            serial = slot->serial;
            spp_unlock();

            while (1) {
                // get incoming_data from slot incoming list
                spp_lock();
                if ((slot = spp_local_param.spp_slots[serial]) != NULL &&
                    slot->rfc_handle == p_data->data_ind.handle &&
                    fixed_queue_length(slot->rx.queue) > 0) {
                    p_buf = (BT_HDR *)fixed_queue_dequeue(slot->rx.queue, FIXED_QUEUE_MAX_TIMEOUT);
                } else {
                    spp_unlock();
                    break;
                }
                spp_unlock();

                // invoke callback
                if (p_buf) {
                    count += 1;
                    param.data_ind.status = ESP_SPP_SUCCESS;
                    param.data_ind.handle = p_data->data_ind.handle;
                    param.data_ind.len = p_buf->len;
                    param.data_ind.data = p_buf->data + p_buf->offset;
                    btc_spp_cb_to_app(ESP_SPP_DATA_IND_EVT, &param);
                    BTC_TRACE_DEBUG("data cb to app: len %d\n", p_buf->len);
                    osi_free(p_buf);
                }
            }
            if (count != 0) {
                spp_lock();
                slot = spp_local_param.spp_slots[serial];
                if (slot == NULL || slot->rfc_handle != p_data->data_ind.handle) {
                    spp_unlock();
                    break;
                }
                slot->credit_rx += count;
                spp_unlock();
                BTA_JvRfcommFlowControl(p_data->data_ind.handle, count);
                BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
            }
        } while (0);
        break;
    case BTA_JV_FREE_SCN_EVT:
        if (p_data->free_scn.server_status == BTA_JV_SERVER_RUNNING) {
            param.srv_stop.status = p_data->free_scn.status;
            param.srv_stop.scn = p_data->free_scn.scn;
            btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
        }
        break;
    case BTA_JV_DISABLE_EVT:
        param.uninit.status = ESP_SPP_SUCCESS;
        spp_free_pending_slots();
        BTA_JvFree();
        spp_clr_ready();
        spp_lock();
        if (spp_local_param.tx_resume_alarm) {
            osi_alarm_free(spp_local_param.tx_resume_alarm);
            spp_local_param.tx_resume_alarm = NULL;
        }
        /* s_spp_resident.tx_event_group and tx_drain_evt stay allocated. spp_free_pending_slots() already set the close bits. */
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            if (spp_local_param.spp_vfs_id != -1) {
                esp_vfs_unregister_with_id(spp_local_param.spp_vfs_id);
                spp_local_param.spp_vfs_id = -1;
            }
        }
        spp_unlock();
#if SPP_DYNAMIC_MEMORY == TRUE
        osi_free(spp_local_param_ptr);
        spp_local_param_ptr = NULL;
#endif
        /* s_spp_resident stays allocated across spp deinit */
        btc_spp_cb_to_app(ESP_SPP_UNINIT_EVT, &param);
        break;
    default:
        BTC_TRACE_DEBUG("%s: Unhandled event (%d)!", __FUNCTION__, msg->act);
        break;
    }

}

int bta_co_rfc_data_incoming(void *user_data, BT_HDR *p_buf)
{
    int ret = 1;
    bt_status_t status;
    tBTA_JV p_data;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = BTA_JV_RFCOMM_DATA_IND_EVT;

    uint32_t id = (uintptr_t)user_data;
    if (!is_spp_ready()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        osi_free(p_buf);
        return -1;
    }
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        osi_free(p_buf);
        return -1;
    }
    spp_slot_t *slot = spp_find_slot_by_id(id);
    if (!slot) {
        spp_unlock();
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_free(p_buf);
        return -1;
    }
    p_data.data_ind.handle = slot->rfc_handle;
    p_data.data_ind.p_buf = NULL;

    if (slot->credit_rx == 0) {
        spp_unlock();
        osi_free(p_buf);
        BTC_TRACE_WARNING("slot credit exhausted");
        return 0;
    }

    if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
        size_t rx_len = fixed_queue_length(slot->rx.queue);
        fixed_queue_enqueue(slot->rx.queue, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
        if (rx_len == 0) {
            BTC_TRACE_DEBUG("%s data post! %d, %d", __func__, slot->rfc_handle, rx_len);
            status = btc_transfer_context(&msg, &p_data, sizeof(tBTA_JV), NULL, NULL);
            assert(status == BT_STATUS_SUCCESS);
        }
    } else {
        fixed_queue_enqueue(slot->rx.queue, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
    }
    if (--slot->credit_rx == 0) {
        BTC_TRACE_DEBUG("%s data post stop! %d %d", __func__, slot->rfc_handle, fixed_queue_length(slot->rx.queue));
        ret = 0; // reserved for other flow control
    }
    if (slot->credit_rx > BTA_JV_MAX_CREDIT_NUM) {
        BTC_TRACE_WARNING("%s credit %d", __func__, slot->credit_rx);
        assert(0);
    }
    spp_unlock();
    return ret;
}

int bta_co_rfc_data_outgoing_size(void *user_data, int *size)
{
    return 1;
}

int bta_co_rfc_data_outgoing(void *user_data, uint8_t *buf, uint16_t size)
{
    return 1;
}

/* Caller does not hold spp_lock. Timeout 0: a full BTC queue must not block the application. */
static void spp_post_tx_drain(void)
{
    struct osi_event *evt = NULL;

    spp_lock();
    if (is_spp_ready()) {
        evt = s_spp_resident.tx_drain_evt;
    }
    spp_unlock();

    if (evt != NULL && osi_thread_post_event(evt, 0)) {
        return;
    }
    /* Already queued, or the BTC queue is full. One alarm is enough while it is pending. */
    spp_lock();
    if (is_spp_ready() &&
            (spp_local_param.tx_resume_alarm == NULL || !osi_alarm_is_active(spp_local_param.tx_resume_alarm))) {
        spp_arm_tx_resume_under_lock();
    }
    spp_unlock();
}

/* CB mode: copy into the TX ringbuf under the resident lock and return immediately. */
esp_err_t spp_send_data_to_btc(uint32_t handle, int len, uint8_t *p_data, esp_spp_mode_t spp_mode)
{
    if (spp_mode != ESP_SPP_MODE_CB || p_data == NULL || len <= 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!is_spp_ready()) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = ESP_OK;
    bool need_drain = false;

    spp_lock();
    do {
        if (!is_spp_ready()) {
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        if (spp_local_param.spp_mode != ESP_SPP_MODE_CB) {
            ret = ESP_ERR_NOT_SUPPORTED;
            break;
        }
        spp_slot_t *slot = spp_find_slot_by_handle(handle);
        if (!slot || !slot->connected || !slot->ringbuf_write) {
            ret = ESP_ERR_INVALID_STATE;
            break;
        }
        if ((size_t)len > spp_local_param.tx_buffer_size) {
            ret = ESP_ERR_INVALID_SIZE;
            break;
        }

        size_t items_waiting = spp_get_tx_items_waiting_under_lock(slot);
        if ((items_waiting + len <= spp_local_param.tx_buffer_size) &&
            xRingbufferSend(slot->ringbuf_write, p_data, len, 0) == pdTRUE) {
            items_waiting += len;
        } else {
            slot->upper_cong = true;
            ret = ESP_ERR_NO_MEM;
        }
        /* items_waiting includes this write. write_data is a chunk already checked out. */
        need_drain = !slot->lower_cong && !slot->is_writing && (slot->write_data != NULL || items_waiting > 0);
    } while (0);
    spp_unlock();

    if (need_drain) {
        spp_post_tx_drain();
    }

    return ret;
}

static ssize_t spp_vfs_write(int fd, const void * data, size_t size)
{
    assert(data != NULL);
    errno = 0;
    if (size == 0) {
        return 0;
    }
    if (!is_spp_ready()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }

    spp_slot_t *slot = NULL;
    uint8_t serial = 0;
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        spp_unlock();
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    spp_unlock();

    ssize_t sent = 0;
    size_t items_waiting = 0;
    size_t item_size = 0;
    EventBits_t tx_event_group_val = 0;
    BaseType_t done = false;
    while (size) {
        tx_event_group_val = 0;
        spp_lock();
        if (!is_spp_ready()) {
            spp_unlock();
            errno = ESRCH;
            break;
        }
        slot = spp_local_param.spp_slots[serial];
        if (slot && slot->connected) {
            items_waiting = 0;
            item_size = 0;
            vRingbufferGetInfo(slot->ringbuf_write, NULL, NULL, NULL, NULL, &items_waiting);
            if (items_waiting < spp_local_param.tx_buffer_size) {
                if ((spp_local_param.tx_buffer_size - items_waiting) > size) {
                    item_size = size;
                    done = xRingbufferSend(slot->ringbuf_write, (void *)data + sent, item_size, 0);
                } else {
                    item_size = spp_local_param.tx_buffer_size - items_waiting;
                    done = xRingbufferSend(slot->ringbuf_write, (void *)data + sent, item_size, 0);
                }

                if (done) {
                    sent += item_size;
                    size -= item_size;
                    bool need_drain = !slot->is_writing && !slot->lower_cong;
                    spp_unlock();
                    if (need_drain) {
                        spp_post_tx_drain();
                    }
                    continue;
                }
            }

            BTC_TRACE_DEBUG("%s items_waiting:%d, fd:%d\n", __func__, items_waiting, fd);
            spp_unlock();

            // block until under water level, be closed or time out
            tx_event_group_val =
                xEventGroupWaitBits(s_spp_resident.tx_event_group, SLOT_WRITE_BIT(serial) | SLOT_CLOSE_BIT(serial), pdTRUE,
                                    pdFALSE, VFS_WRITE_TIMEOUT / portTICK_PERIOD_MS);
            if (tx_event_group_val & SLOT_CLOSE_BIT(serial)) {
                BTC_TRACE_ERROR("%s exit for RFCOMM close, fd:%d!", __func__, fd);
                errno = EPIPE;
                break;
            } else if (tx_event_group_val & SLOT_WRITE_BIT(serial)) {
                continue;
            } else if (tx_event_group_val == 0) {
                BTC_TRACE_ERROR("%s exit for time out, fd:%d!", __func__, fd);
                errno = EBUSY;
                break;
            }
        } else {
            spp_unlock();
            errno = EPIPE;
            break;
        }
    }

    if (errno != 0) {
        if (sent == 0) {
            sent = -1;
        }
    }

    return sent;
}

static int spp_vfs_close(int fd)
{
    errno = 0;
    if (!is_spp_ready()) {
        errno = ESRCH;
        return -1;
    }
    spp_slot_t *slot = NULL;
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        spp_unlock();
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    uint32_t handle = slot->rfc_handle;
    spp_unlock();
    esp_spp_disconnect(handle);
    return 0;
}

static ssize_t spp_vfs_read(int fd, void * dst, size_t size)
{
    assert(dst != NULL);
    errno = 0;
    if (!is_spp_ready()) {
        errno = ESRCH;
        return -1;
    }

    spp_slot_t *slot = NULL;
    uint8_t serial = 0;
    spp_lock();
    if (!is_spp_ready()) {
        spp_unlock();
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        spp_unlock();
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    spp_unlock();

    ssize_t item_size = 0;
    uint16_t count = 0;
    BT_HDR *p_buf;
    while (1) {
        spp_lock();
        if (!is_spp_ready()) {
            spp_unlock();
            errno = ESRCH;
            break;
        }
        if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
            if (fixed_queue_length(slot->rx.queue) > 0) {
                // free unused p_buf
                if ((p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) != NULL && p_buf->len == 0) {
                    osi_free(fixed_queue_dequeue(slot->rx.queue, FIXED_QUEUE_MAX_TIMEOUT));
                    p_buf = NULL;
                    count++;
                }
                if (size == 0 || (p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) == NULL) {
                    spp_unlock();
                    break;
                }
            } else {
                /**
                 * If close_alarm is not NULL, it means that we have received the BTA_JV_RFCOMM_CLOSE_EVT.
                 * And we can trigger close_alarm immediately.
                 */
                if (slot->close_alarm && osi_alarm_is_active(slot->close_alarm)) {
                    osi_alarm_cancel(slot->close_alarm);
                    osi_alarm_set(slot->close_alarm, 0);
                }
                spp_unlock();
                break;
            }
        } else {
            spp_unlock();
            BTC_TRACE_ERROR("%s peer close, data will be discarded!\n", __func__);
            errno = EPIPE;
            break;
        }

        if (p_buf->len <= size) {
            memcpy(dst, p_buf->data + p_buf->offset, p_buf->len);
            size -= p_buf->len;
            item_size += p_buf->len;
            dst += p_buf->len;
            p_buf->offset += p_buf->len;
            p_buf->len = 0; // indicate the p_buf is unused
        } else {
            memcpy(dst, p_buf->data + p_buf->offset, size);
            item_size += size;
            p_buf->offset += size;
            p_buf->len -= size;
            size = 0;
        }
        spp_unlock();
    }
    if (count > 0) {
        spp_lock();
        if (is_spp_ready()) {
            if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
                slot->credit_rx += count;
                BTA_JvRfcommFlowControl(slot->rfc_handle, count);
                BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
            }
        }
        spp_unlock();
    }

    if (errno != 0) {
        if (item_size == 0) {
            item_size = -1;
        }
    }

    return item_size;
}

static void btc_spp_vfs_register(void)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    esp_spp_cb_param_t param;

    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }

        if (spp_local_param.spp_mode != ESP_SPP_MODE_VFS) {
            BTC_TRACE_ERROR("SPP not in VFS mode");
            ret = ESP_SPP_FAILURE;
            break;
        }

        if (spp_local_param.spp_vfs_id != -1) {
            BTC_TRACE_ERROR("SPP VFS ID already exist");
            ret = ESP_SPP_BUSY;
            break;
        }

        static const esp_vfs_fs_ops_t vfs = {
            .write = spp_vfs_write,
            .close = spp_vfs_close,
            .read = spp_vfs_read,
        };

        // No FD range is registered here: spp_vfs_id is used to register/unregister
        // file descriptors
        if (esp_vfs_register_fs_with_id(&vfs, ESP_VFS_FLAG_STATIC, NULL, &spp_local_param.spp_vfs_id) != ESP_OK) {
            ret = ESP_SPP_FAILURE;
            break;
        }
    } while (0);

    param.vfs_register.status = ret;
    btc_spp_cb_to_app(ESP_SPP_VFS_REGISTER_EVT, &param);
}

static void btc_spp_vfs_unregister(void)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    esp_spp_cb_param_t param;

    do {
        if (!is_spp_ready()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }

        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            if (spp_local_param.spp_vfs_id != -1) {
                if (esp_vfs_unregister_with_id(spp_local_param.spp_vfs_id) != ESP_OK) {
                    ret = ESP_SPP_FAILURE;
                    break;
                }
                spp_local_param.spp_vfs_id = -1;
            }
        }
    } while (0);

    param.vfs_unregister.status = ret;
    btc_spp_cb_to_app(ESP_SPP_VFS_UNREGISTER_EVT, &param);
}

void btc_spp_clear_tx_drain_evt(void)
{
    /* Storage is freed by osi_thread_event_deinit(). Do not touch the event here. */
    s_spp_resident.tx_drain_evt = NULL;
    s_spp_resident.tx_drain_bound = false;
}

void btc_spp_get_profile_status(esp_spp_profile_status_t *param)
{
    param->spp_inited = false;

    if (!is_spp_ready()) {
        return;
    }

    spp_lock();
    if (is_spp_ready()) {
        param->spp_inited = true;
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
                param->conn_num++;
            }
        }
    }
    spp_unlock();
}

#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
