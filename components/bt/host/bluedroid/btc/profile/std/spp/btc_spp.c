/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#define SLOT_TX_QUEUE_SIZE 10
#define SLOT_TX_QUEUE_LOW_WM 4
#define SLOT_TX_DATA_HIGH_WM (SLOT_TX_QUEUE_SIZE * BTA_JV_DEF_RFC_MTU)
#define VFS_CLOSE_TIMEOUT (20 * 1000)

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
    uint8_t serial;
    uint8_t scn;
    uint8_t max_session;
    uint16_t mtu;
    uint16_t credit_rx;
    uint16_t write_data_len;
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
    union {
        slot_data_t tx;
        RingbufHandle_t ringbuf_write;
    };
    uint8_t service_uuid[16];
    char service_name[ESP_SPP_SERVER_NAME_MAX + 1];
} spp_slot_t;

typedef struct {
    uint16_t tx_buffer_size;
    spp_slot_t *spp_slots[MAX_RFC_PORTS + 1];
    uint32_t spp_slot_id;
    esp_spp_mode_t spp_mode;
    osi_mutex_t spp_slot_mutex;
    EventGroupHandle_t tx_event_group;
    esp_vfs_id_t spp_vfs_id;
} spp_local_param_t;

#if SPP_DYNAMIC_MEMORY == FALSE
static spp_local_param_t spp_local_param;
#else
static spp_local_param_t *spp_local_param_ptr;
#define spp_local_param (*spp_local_param_ptr)
#endif

static void btc_spp_vfs_register(void);
static void btc_spp_vfs_unregister(void);

static void spp_osi_free(void *p)
{
    osi_free(p);
}

#if SPP_DYNAMIC_MEMORY == FALSE
#define is_spp_init() (spp_local_param.spp_slot_mutex != NULL)
#else
#define is_spp_init() (&spp_local_param != NULL && spp_local_param.spp_slot_mutex != NULL)
#endif

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
            (*slot)->is_server = false;
            (*slot)->mtu = 0;
            (*slot)->credit_rx = BTA_JV_MAX_CREDIT_NUM;
            (*slot)->write_data = NULL;
            (*slot)->write_data_len = 0;
            (*slot)->is_writing = false;
            (*slot)->close_alarm = NULL;
            (*slot)->alarm_arg = NULL;
            /* clear the old event bits */
            if (spp_local_param.tx_event_group) {
                xEventGroupClearBits(spp_local_param.tx_event_group, SLOT_WRITE_BIT(i) | SLOT_CLOSE_BIT(i));
            }

            if (init_slot_data(&(*slot)->rx, QUEUE_SIZE_MAX)) {
                BTC_TRACE_ERROR("%s unable to malloc rx queue!", __func__);
                err_no = 1;
                goto err;
            }
            if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
                if (init_slot_data(&(*slot)->tx, SLOT_TX_QUEUE_SIZE)) {
                    BTC_TRACE_ERROR("%s unable to malloc tx queue!", __func__);
                    err_no = 2;
                    goto err;
                }
            } else {
                if (((*slot)->ringbuf_write = xRingbufferCreate(spp_local_param.tx_buffer_size, RINGBUF_TYPE_BYTEBUF)) == NULL) {
                    BTC_TRACE_ERROR("%s write ringbuffer create error!", __func__);
                    err_no = 2;
                    goto err;
                }
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
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                vRingbufferDelete((*slot)->ringbuf_write);
            }
        case 2:
            free_slot_data(&(*slot)->rx);
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
        xEventGroupSetBits(spp_local_param.tx_event_group, SLOT_CLOSE_BIT(slot->serial));
        vRingbufferDelete(slot->ringbuf_write);
    } else {
        free_slot_data(&slot->tx);
    }
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
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        slot = spp_local_param.spp_slots[i];
        if (slot) {
            BTC_TRACE_WARNING("%s found slot(rfc_handle=0x%x) pending to close, close it now!", __func__, slot->rfc_handle);
            spp_free_slot(slot);
        }
    }
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

    if (!is_spp_init()) {
        BTC_TRACE_WARNING("%s SPP have been deinit, incoming events ignore!\n", __func__);
        return new_user_data;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
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
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
    btc_msg_t msg;

    uint32_t id = (uintptr_t)user_data;
    spp_slot_t *slot = NULL;
    switch (event) {
    case BTA_JV_GET_SCN_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            break;
        }
        if (p_data->scn == 0) {
            btc_create_server_fail_cb();
            spp_free_slot(slot);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            BTC_TRACE_ERROR("%s unable to get scn, start server fail!", __func__);
            break;
        }

        slot->scn = p_data->scn;
        BTA_JvCreateRecordByUser(slot->service_name, slot->scn, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    case BTA_JV_CREATE_RECORD_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            break;
        }
        if (p_data->create_rec.status == BTA_JV_SUCCESS) {
            slot->sdp_handle = p_data->create_rec.handle;
            BTA_JvRfcommStartServer(slot->security, slot->role, slot->scn,
                                    slot->max_session, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        } else {
            BTC_TRACE_ERROR("%s unable to create record, start server fail!", __func__);
            btc_create_server_fail_cb();
            tBTA_JV_FREE_SCN_USER_DATA *user_data = osi_malloc(sizeof(tBTA_JV_FREE_SCN_USER_DATA));
            if (user_data) {
                user_data->server_status = BTA_JV_SERVER_START_FAILED;
                user_data->slot_id = slot->id;
            } else {
                BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                assert(0);
            }
            BTA_JvFreeChannel(slot->scn, BTA_JV_CONN_TYPE_RFCOMM,
                              (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        if (is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP has been initiated, shall uninit first!", __func__);
            ret = ESP_SPP_NEED_DEINIT;
            break;
        }

#if SPP_DYNAMIC_MEMORY == TRUE
        if ((spp_local_param_ptr = (spp_local_param_t *)osi_malloc(sizeof(spp_local_param_t))) == NULL) {
            BTC_TRACE_ERROR("%s malloc failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        memset((void *)spp_local_param_ptr, 0, sizeof(spp_local_param_t));
#endif

        if (osi_mutex_new(&spp_local_param.spp_slot_mutex) != 0) {
            BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
#if SPP_DYNAMIC_MEMORY == TRUE
            osi_free(spp_local_param_ptr);
            spp_local_param_ptr = NULL;
#endif
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        if ((spp_local_param.tx_event_group = xEventGroupCreate()) == NULL) {
            BTC_TRACE_ERROR("%s create tx_event_group failed\n", __func__);
            osi_mutex_free(&spp_local_param.spp_slot_mutex);
#if SPP_DYNAMIC_MEMORY == TRUE
            osi_free(spp_local_param_ptr);
            spp_local_param_ptr = NULL;
#endif
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        if (arg->init.mode == ESP_SPP_MODE_VFS) {
            spp_local_param.spp_vfs_id = -1;
        }
        spp_local_param.spp_mode = arg->init.mode;
        spp_local_param.spp_slot_id = 0;
        spp_local_param.tx_buffer_size = arg->init.tx_buffer_size;
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
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP has not been initiated, shall init first!", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
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
                BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                  (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
            }
        }
        BTA_JvDisable((tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        BTA_JvStartDiscovery(arg->start_discovery.bd_addr, arg->start_discovery.num_uuid, arg->start_discovery.p_uuid_list, NULL);
    } while (0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.disc_comp.status = ret;
        param.disc_comp.scn_num = 0xff;
        memset(param.disc_comp.scn, 0xff, ESP_SPP_MAX_SCN);
        btc_spp_cb_to_app(ESP_SPP_DISCOVERY_COMP_EVT, &param);
    }
}

static void btc_spp_connect(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        slot->security = arg->connect.sec_mask;
        slot->role = arg->connect.role;
        slot->scn = arg->connect.remote_scn;

        memcpy(slot->addr, arg->connect.peer_bd_addr, ESP_BD_ADDR_LEN);
        BTA_JvRfcommConnect(arg->connect.sec_mask, arg->connect.role, arg->connect.remote_scn,
                            arg->connect.peer_bd_addr, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_slot_t *slot = NULL;
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_handle(arg->disconnect.handle);
        if (!slot || (slot && !slot->connected)) {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot! disconnect fail!", __func__);
            } else {
                BTC_TRACE_ERROR("%s RFCOMM has been disconnected already!", __func__);
            }
            ret = ESP_SPP_NO_CONNECTION;
            break;
        }
        BTA_JvRfcommClose(arg->disconnect.handle, (void *)slot->id);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (ret != ESP_SPP_SUCCESS) {
        esp_spp_cb_param_t param;
        param.close.status = ret;
        param.close.port_status = PORT_ERR_MAX;
        param.close.handle = 0;
        param.close.async = FALSE;
        btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
    }
}

static void btc_spp_start_srv(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        spp_slot_t *slot = spp_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        /**
         * make this slot become a listening slot
         */
        slot->is_server = true;
        slot->security = arg->start_srv.sec_mask;
        slot->role = arg->start_srv.role;
        slot->scn = arg->start_srv.local_scn;
        slot->max_session = arg->start_srv.max_session;
        strcpy(slot->service_name, arg->start_srv.name);

        BTA_JvGetChannelId(BTA_JV_CONN_TYPE_RFCOMM, (void *)slot->id, arg->start_srv.local_scn);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        if (!is_spp_init()) {
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

        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
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
                    BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                      (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
                }
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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

static void btc_spp_write(btc_spp_args_t *arg)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;

    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        spp_slot_t *slot = NULL;
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_handle(arg->write.handle);
        if (!slot || (slot && !slot->connected)) {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            } else {
                BTC_TRACE_ERROR("%s RFCOMM has been disconnected already!", __func__);
            }
            ret = ESP_SPP_NO_CONNECTION;
            break;
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            if (slot->is_writing) {
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                return;
            }
            size_t item_size = 0;
            uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, slot->mtu);
            if (item_size > 0) {
                slot->write_data = data;
                slot->write_data_len = item_size;
                slot->is_writing = true;
                BTA_JvRfcommWrite(arg->write.handle, slot->id, item_size, data);
            }
        } else {
            if (fixed_queue_enqueue(slot->tx.queue, arg->write.p_data, 0)) {
                BTA_JvRfcommWrite(arg->write.handle, slot->id, arg->write.len, arg->write.p_data);
                // The TX queue of SPP will handle this memory properly.
                // Set it to NULL here to prevent deep free handler from releasing it.
                arg->write.p_data = NULL;
            } else {
                ret = ESP_SPP_NO_RESOURCE;
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while (0);

    if (ret != ESP_SPP_SUCCESS && spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
        esp_spp_cb_param_t param;
        param.write.status = ret;
        param.write.handle = 0;
        param.write.len = -1;
        param.write.cong = false;
        btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
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
    case BTC_SPP_ACT_WRITE:
        dst->write.p_data = (uint8_t *)osi_malloc(src->write.len);
        if (dst->write.p_data) {
            memcpy(dst->write.p_data, src->write.p_data, src->write.len);
        } else if (src->write.len == 0) {
            BTC_TRACE_DEBUG("%s %d no mem\n", __func__, msg->act);
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
    case BTC_SPP_ACT_WRITE:
        if (arg->write.p_data) {
            osi_free(arg->write.p_data);
            arg->write.p_data = NULL;
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
    case BTC_SPP_ACT_WRITE:
        btc_spp_write(arg);
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
    esp_spp_cb_param_t param;
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
        param.disc_comp.scn_num = p_data->disc_comp.scn_num;
        memcpy(param.disc_comp.scn, p_data->disc_comp.scn, p_data->disc_comp.scn_num);
        memcpy(param.disc_comp.service_name, p_data->disc_comp.service_name,
               p_data->disc_comp.scn_num * sizeof(const char *));
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
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                slot = spp_find_slot_by_handle(p_data->rfc_open.handle);
                if (!slot) {
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                    param.open.status = ESP_SPP_NO_CONNECTION;
                    break;
                }
                param.open.fd = slot->fd;
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
                    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                    slot = spp_find_slot_by_handle(p_data->rfc_srv_open.handle);
                    if (!slot) {
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                        param.srv_open.status = ESP_SPP_NO_CONNECTION;
                        break;
                    }
                    param.srv_open.fd = slot->fd;
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
    case BTA_JV_RFCOMM_WRITE_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(p_data->rfc_write.req_id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!, handle:%d", __func__, p_data->rfc_write.handle);
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB){
            param.write.status = slot ? p_data->rfc_write.status : ESP_SPP_NO_CONNECTION;
            param.write.handle = p_data->rfc_write.handle;
            param.write.len = p_data->rfc_write.len;
            param.write.cong = p_data->rfc_write.cong;
            btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
            if (slot) {
                osi_free(fixed_queue_dequeue(slot->tx.queue, FIXED_QUEUE_MAX_TIMEOUT));
            }
        } else {
            if (slot) {
                size_t item_size = 0;
                size_t items_waiting = 0;
                serial = slot->serial;
                if (p_data->rfc_write.status == BTA_JV_SUCCESS) {
                    vRingbufferReturnItem(slot->ringbuf_write,slot->write_data);
                    slot->write_data = NULL;
                    slot->is_writing = false;
                    slot->write_data_len = 0;
                    vRingbufferGetInfo(slot->ringbuf_write, NULL, NULL, NULL, NULL, &items_waiting);
                    if (spp_local_param.tx_buffer_size > items_waiting) {
                        xEventGroupSetBits(spp_local_param.tx_event_group, SLOT_WRITE_BIT(serial));
                    }
                    if (items_waiting == 0) {
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        break;
                    }
                    if (!p_data->rfc_write.cong) {
                        uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, slot->mtu);
                        if (item_size > 0) {
                            slot->write_data = data;
                            slot->write_data_len = item_size;
                            slot->is_writing = true;
                            BTA_JvRfcommWrite(p_data->rfc_write.handle, slot->id, item_size, data);
                        }
                    }
                } else {
                    if (!p_data->rfc_write.old_cong) {
                        // PORT_WriteDataCO failed
                        BTC_TRACE_ERROR("PORT_WriteDataCO failed p_buf:%p, handle:%d\n", slot->write_data,
                                        p_data->rfc_write.handle);
                    } else {
                        // need rewrite
                        if (!p_data->rfc_write.cong && slot->connected) {
                            slot->is_writing = true;
                            BTA_JvRfcommWrite(p_data->rfc_write.handle, slot->id, slot->write_data_len, slot->write_data);
                        } else {
                            slot->is_writing = false;
                        }
                    }
                }
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    case BTA_JV_RFCOMM_CLOSE_EVT:
        param.close.status = BTA_JV_SUCCESS;
        param.close.port_status = p_data->rfc_close.port_status;
        param.close.handle = p_data->rfc_close.handle;
        param.close.async = p_data->rfc_close.async;
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_id((uint32_t)p_data->rfc_close.user_data);
            if (!slot) {
                param.close.status = ESP_SPP_NO_CONNECTION;
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            spp_free_slot(slot);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
        } else {
            bool need_call = true;
            do {
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                slot = spp_find_slot_by_id((uint32_t)p_data->rfc_close.user_data);
                if (!slot) {
                    param.close.status = ESP_SPP_NO_CONNECTION;
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                    break;
                }
                // if rx still has data, delay free slot
                if (slot->close_alarm == NULL && slot->rx.queue && fixed_queue_length(slot->rx.queue) > 0) {
                    tBTA_JV *p_arg = NULL;
                    if ((p_arg = osi_malloc(sizeof(tBTA_JV))) == NULL) {
                        param.close.status = ESP_SPP_NO_RESOURCE;
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s unable to malloc slot close_alarm!", __func__);
                        break;
                    }
                    if (osi_alarm_set(slot->close_alarm, VFS_CLOSE_TIMEOUT) != OSI_ALARM_ERR_PASS) {
                        osi_free(p_arg);
                        slot->alarm_arg = NULL;
                        osi_alarm_free(slot->close_alarm);
                        param.close.status = ESP_SPP_BUSY;
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s set slot close_alarm failed!", __func__);
                        break;
                    }
                    BTC_TRACE_WARNING("%s slot rx data will be discard in %d milliseconds!",
                                      __func__, VFS_CLOSE_TIMEOUT);
                    slot->connected = false;
                    need_call = false;
                }
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            } while (0);

            if (need_call) {
                btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                spp_free_slot(slot);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            }
        }
        break;
    case BTA_JV_RFCOMM_CONG_EVT:
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
            param.cong.status = p_data->rfc_cong.status;
            param.cong.handle = p_data->rfc_cong.handle;
            param.cong.cong = p_data->rfc_cong.cong;
            btc_spp_cb_to_app(ESP_SPP_CONG_EVT, &param);
        } else {
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->rfc_cong.handle);
            if (!slot) {
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            if (!p_data->rfc_cong.cong && !slot->is_writing) {
                if (slot->write_data == NULL && slot->write_data_len == 0) {
                    size_t item_size = 0;
                    uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0, slot->mtu);
                    if (item_size > 0) {
                        slot->write_data = data;
                        slot->write_data_len = item_size;
                        slot->is_writing = true;
                        BTA_JvRfcommWrite(p_data->rfc_cong.handle, slot->id, item_size, data);
                    }
                } else {
                    slot->is_writing = true;
                    BTA_JvRfcommWrite(p_data->rfc_cong.handle, slot->id, slot->write_data_len, slot->write_data);
                }
            }
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        }
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        do {
            BT_HDR *p_buf;
            UINT16 count = 0;
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->data_ind.handle);
            if (!slot) {
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            serial = slot->serial;
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

            while (1) {
                // get incoming_data from slot incoming list
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                if ((slot = spp_local_param.spp_slots[serial]) != NULL &&
                    slot->rfc_handle == p_data->data_ind.handle &&
                    fixed_queue_length(slot->rx.queue) > 0) {
                    p_buf = (BT_HDR *)fixed_queue_dequeue(slot->rx.queue, FIXED_QUEUE_MAX_TIMEOUT);
                } else {
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    break;
                }
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

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
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                slot->credit_rx += count;
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        osi_mutex_free(&spp_local_param.spp_slot_mutex);
        if (spp_local_param.tx_event_group) {
            vEventGroupDelete(spp_local_param.tx_event_group);
            spp_local_param.tx_event_group = NULL;
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            if (spp_local_param.spp_vfs_id != -1) {
                esp_vfs_unregister_with_id(spp_local_param.spp_vfs_id);
                spp_local_param.spp_vfs_id = -1;
            }
        }
#if SPP_DYNAMIC_MEMORY == TRUE
        osi_free(spp_local_param_ptr);
        spp_local_param_ptr = NULL;
#endif
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
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return -1;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_id(id);
    if (!slot) {
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        return -1;
    }
    p_data.data_ind.handle = slot->rfc_handle;
    p_data.data_ind.p_buf = NULL;

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
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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

esp_err_t spp_send_data_to_btc(uint32_t handle, int len, uint8_t *p_data, esp_spp_mode_t spp_mode)
{
    btc_msg_t msg;
    btc_spp_args_t arg;

    if (spp_local_param.spp_mode != spp_mode) {
        BTC_TRACE_WARNING("The current mode used is %d\n", spp_local_param.spp_mode);
        return ESP_ERR_NOT_SUPPORTED;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SPP;
    msg.act = BTC_SPP_ACT_WRITE;

    arg.write.handle = handle;
    arg.write.len = len;
    arg.write.p_data = p_data;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_spp_args_t), btc_spp_arg_deep_copy,
                btc_spp_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

static ssize_t spp_vfs_write(int fd, const void * data, size_t size)
{
    assert(data != NULL);
    errno = 0;
    if (size == 0) {
        return 0;
    }
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }

    spp_slot_t *slot = NULL;
    uint8_t serial = 0;
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

    ssize_t sent = 0;
    size_t items_waiting = 0;
    size_t item_size = 0;
    EventBits_t tx_event_group_val = 0;
    BaseType_t done = false;
    while (size) {
        tx_event_group_val = 0;
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
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
                    if (slot->write_data == NULL) {
                        spp_send_data_to_btc(slot->rfc_handle, 0, NULL, ESP_SPP_MODE_VFS);
                    }
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    continue;
                }
            }

            BTC_TRACE_DEBUG("%s items_waiting:%d, fd:%d\n", __func__, items_waiting, fd);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

            // block until under water level, be closed or time out
            tx_event_group_val =
                xEventGroupWaitBits(spp_local_param.tx_event_group, SLOT_WRITE_BIT(serial) | SLOT_CLOSE_BIT(serial), pdTRUE,
                                    pdFALSE, VFS_WRITE_TIMEOUT / portTICK_PERIOD_MS);
            if (tx_event_group_val & SLOT_CLOSE_BIT(serial)) {
                BTC_TRACE_ERROR("%s exit for RFCOMM close, fd:%d!", __func__, fd);
                errno = EPIPE;
                sent = -1;
                break;
            } else if (tx_event_group_val & SLOT_WRITE_BIT(serial)) {
                continue;
            } else if (tx_event_group_val == 0) {
                BTC_TRACE_ERROR("%s exit for time out, fd:%d!", __func__, fd);
                errno = EBUSY;
                sent = -1;
                break;
            }
        } else {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            errno = EPIPE;
            sent = -1;
            break;
        }
    }

    return sent;
}

static int spp_vfs_close(int fd)
{
    errno = 0;
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }
    spp_slot_t *slot = NULL;
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    esp_spp_disconnect(slot->rfc_handle);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return 0;
}

static ssize_t spp_vfs_read(int fd, void * dst, size_t size)
{
    assert(dst != NULL);
    errno = 0;
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }

    spp_slot_t *slot = NULL;
    uint8_t serial = 0;
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

    ssize_t item_size = 0;
    uint16_t count = 0;
    BT_HDR *p_buf;
    while (1) {
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
            if (fixed_queue_length(slot->rx.queue) > 0) {
                // free unused p_buf
                if ((p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) != NULL && p_buf->len == 0) {
                    osi_free(fixed_queue_dequeue(slot->rx.queue, FIXED_QUEUE_MAX_TIMEOUT));
                    p_buf = NULL;
                    count++;
                }
                if (size == 0 || (p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) == NULL) {
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
        } else {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            BTC_TRACE_ERROR("%s peer close, data will be discarded!\n", __func__);
            errno = EPIPE;
            item_size = -1;
            break;
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);

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
    }
    if (count > 0) {
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot->credit_rx += count;
        if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
            BTA_JvRfcommFlowControl(slot->rfc_handle, count);
            BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    }
    return item_size;
}

static void btc_spp_vfs_register(void)
{
    esp_spp_status_t ret = ESP_SPP_SUCCESS;
    esp_spp_cb_param_t param;

    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }

        esp_vfs_t vfs = {
            .flags = ESP_VFS_FLAG_DEFAULT,
            .write = spp_vfs_write,
            .open = NULL,
            .fstat = NULL,
            .close = spp_vfs_close,
            .read = spp_vfs_read,
            .fcntl = NULL
        };

        // No FD range is registered here: spp_vfs_id is used to register/unregister
        // file descriptors
        if (esp_vfs_register_with_id(&vfs, NULL, &spp_local_param.spp_vfs_id) != ESP_OK) {
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
        if (!is_spp_init()) {
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

void btc_spp_get_profile_status(esp_spp_profile_status_t *param)
{
    if (is_spp_init()) {
        param->spp_inited = true;
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
                param->conn_num++;
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } else {
        param->spp_inited = false;
    }
}

#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
