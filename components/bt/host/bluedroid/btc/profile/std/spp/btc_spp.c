// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
// limitations under the License.

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
    uint8_t serial;
    bool connected;
    uint8_t scn;
    uint8_t max_session;
    uint32_t id;
    uint32_t mtu;//unused
    uint32_t sdp_handle;
    uint32_t rfc_handle;
    uint32_t rfc_port_handle;
    int fd;
    uint8_t *write_data;
    osi_alarm_t *close_alarm;
    esp_spp_role_t role;
    esp_spp_sec_t security;
    esp_bd_addr_t addr;
    slot_data_t rx;
    slot_data_t tx;
    uint8_t service_uuid[16];
    char service_name[ESP_SPP_SERVER_NAME_MAX + 1];
} spp_slot_t;

typedef struct {
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

void free_slot_data(slot_data_t *slot_data)
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
            (*slot)->connected = FALSE;
            (*slot)->write_data = NULL;
            (*slot)->close_alarm = NULL;
            if (init_slot_data(&(*slot)->rx, QUEUE_SIZE_MAX)) {
                BTC_TRACE_ERROR("%s unable to malloc rx queue!", __func__);
                err_no = 1;
                break;
            }
            if (init_slot_data(&(*slot)->tx, SLOT_TX_QUEUE_SIZE)) {
                BTC_TRACE_ERROR("%s unable to malloc tx queue!", __func__);
                err_no = 2;
                break;
            }
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                if (esp_vfs_register_fd(spp_local_param.spp_vfs_id, &(*slot)->fd) != ESP_OK) {
                    BTC_TRACE_ERROR("%s unable to register fd!", __func__);
                    err_no = 3;
                    break;
                }
            }
            return (*slot);
        }
    }
    switch (err_no) {
        case 3:
            free_slot_data(&(*slot)->tx);
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
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->id == id) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_handle(uint32_t handle)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->rfc_handle == handle) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_fd(int fd)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->fd == fd) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_scn(uint32_t scn)
{
    for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->scn == (uint8_t)scn) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static void close_timeout_handler(void *arg)
{
    btc_msg_t msg;
    bt_status_t status;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = BTA_JV_RFCOMM_CLOSE_EVT;

    status = btc_transfer_context(&msg, arg, sizeof(tBTA_JV), NULL);

    if (arg) {
        free(arg);
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
    }
    free_slot_data(&slot->tx);
    free_slot_data(&slot->rx);
    if (slot->close_alarm) {
        osi_alarm_free(slot->close_alarm);
    }
    osi_free(slot);
}

static inline void btc_spp_cb_to_app(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    esp_spp_cb_t *btc_spp_cb = (esp_spp_cb_t *)btc_profile_cb_get(BTC_PID_SPP);
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

    uint32_t id = (uintptr_t)user_data;
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
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_start.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->rfc_handle = p_data->rfc_start.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_start.handle);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        slot = p_data->rfc_srv_open.handle ? spp_find_slot_by_id(id) : spp_find_slot_by_scn((uint32_t)user_data);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_srv_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }

        if (p_data->rfc_srv_open.handle) {
            new_user_data = (void *)(uintptr_t)slot->id;
            memcpy(slot->addr, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
            slot->connected = TRUE;
            slot->rfc_handle = p_data->rfc_srv_open.handle;
            slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.handle);
            BTA_JvSetPmProfile(p_data->rfc_srv_open.handle, BTA_JV_PM_ALL, BTA_JV_CONN_OPEN);
        }

        if (p_data->rfc_srv_open.handle != p_data->rfc_srv_open.new_listen_handle) {
            slot_new = spp_malloc_slot();
            if (!slot_new) {
                BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
                p_data->rfc_srv_open.status = ESP_SPP_NO_RESOURCE;
                break;
            }
            new_user_data = (void *)(uintptr_t)slot_new->id;
            slot_new->security = slot->security;
            slot_new->role = slot->role;
            slot_new->scn = slot->scn;
            slot_new->max_session = slot->max_session;
            strcpy(slot_new->service_name, slot->service_name);
            slot_new->sdp_handle = slot->sdp_handle;
            slot_new->rfc_handle = p_data->rfc_srv_open.new_listen_handle;
            slot_new->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.new_listen_handle);
        }
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_open.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        slot->connected = TRUE;
        slot->rfc_handle = p_data->rfc_open.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_open.handle);
        BTA_JvSetPmProfile(p_data->rfc_open.handle, BTA_JV_PM_ID_1, BTA_JV_CONN_OPEN);
        break;
    case BTA_JV_RFCOMM_CLOSE_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            p_data->rfc_close.status = ESP_SPP_NO_CONNECTION;
            break;
        }
        if (slot->connected && p_data->rfc_close.port_status != PORT_LOCAL_CLOSED) {
            BTA_JvRfcommClose(slot->rfc_handle, NULL, (void *)slot->id);
        }
        p_data->rfc_close.status = BTA_JV_SUCCESS;
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
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
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
                                  sizeof(tBTA_JV), NULL);

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
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
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
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
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

        status = btc_transfer_context(&msg, p_data, sizeof(tBTA_JV), NULL);

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
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        if ((spp_local_param.tx_event_group = xEventGroupCreate()) == NULL) {
            BTC_TRACE_ERROR("%s create tx_event_group failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }
        spp_local_param.spp_mode = arg->init.mode;
        spp_local_param.spp_slot_id = 0;
        BTA_JvEnable((tBTA_JV_DM_CBACK *)btc_spp_dm_inter_cb);
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
            if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
                BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb,
                                  (void *)spp_local_param.spp_slots[i]->id);
            }
        }
        // second, remove all server
        for (size_t i = 1; i <= MAX_RFC_PORTS; i++) {
            if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->connected) {
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
                    esp_spp_cb_param_t param;
                    BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                    param.srv_stop.status = ESP_SPP_NO_RESOURCE;
                    param.srv_stop.scn = spp_local_param.spp_slots[i]->scn;
                    btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
                }
                BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM,
                                  (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)user_data);
            }
        }
        BTA_JvDisable((tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    } while(0);

    if (spp_local_param.tx_event_group) {
        vEventGroupDelete(spp_local_param.tx_event_group);
        spp_local_param.tx_event_group = NULL;
    }

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
        BTA_JvRfcommClose(arg->disconnect.handle, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
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
    uint8_t *srv_scn_arr = osi_malloc(MAX_RFC_PORTS);
    if (arg->stop_srv.scn == BTC_SPP_INVALID_SCN) {
        is_remove_all = true;
    }

    do {
        if (!is_spp_init()) {
            BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
            ret = ESP_SPP_NEED_INIT;
            break;
        }
        if (srv_scn_arr == NULL) {
            BTC_TRACE_ERROR("%s malloc srv_scn_arr failed\n", __func__);
            ret = ESP_SPP_NO_RESOURCE;
            break;
        }

        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        // [1] find all server
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
                if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected &&
                    spp_local_param.spp_slots[i]->sdp_handle > 0 &&
                    spp_local_param.spp_slots[i]->scn == srv_scn_arr[j]) {
                    BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle,
                                      (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb,
                                      (void *)spp_local_param.spp_slots[i]->id);
                }
            }
        }

        // [3] remove all server
        for (j = 0; j < srv_cnt; j++) {
            for (i = 1; i <= MAX_RFC_PORTS; i++) {
                if (spp_local_param.spp_slots[i] != NULL && !spp_local_param.spp_slots[i]->connected &&
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
                        esp_spp_cb_param_t param;
                        BTC_TRACE_ERROR("%s unable to malloc user data!", __func__);
                        param.srv_stop.status = ESP_SPP_NO_RESOURCE;
                        param.srv_stop.scn = spp_local_param.spp_slots[i]->scn;
                        btc_spp_cb_to_app(ESP_SPP_SRV_STOP_EVT, &param);
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
            BT_HDR *p_buf;
            if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0) {
                p_buf->event++;
                p_buf->layer_specific = 1;
                BTA_JvRfcommWrite(arg->write.handle, slot->id, p_buf->len, p_buf->data + p_buf->offset);
            }
        } else {
            if (fixed_queue_enqueue(slot->tx.queue, arg->write.p_data, 0)) {
                BTA_JvRfcommWrite(arg->write.handle, slot->id, arg->write.len, arg->write.p_data);
            } else {
                ret = ESP_SPP_NO_RESOURCE;
                break;
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
    switch (msg->act) {
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
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                    param.open.status = ESP_SPP_NO_CONNECTION;
                    break;
                }
                param.open.fd = slot->fd;
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
                        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                        param.srv_open.status = ESP_SPP_NO_CONNECTION;
                        break;
                    }
                    param.srv_open.fd = slot->fd;
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
        slot = spp_find_slot_by_handle(p_data->rfc_write.handle);
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
                BT_HDR *p_buf;
                serial = slot->serial;
                if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) == NULL) {
                    break;
                }
                if (p_data->rfc_write.status == BTA_JV_SUCCESS) {
                    p_buf->len -= p_data->rfc_write.len;
                    p_buf->offset += p_data->rfc_write.len;
                    p_buf->layer_specific = 0;
                    if (p_buf->len == 0) {
                        osi_free(fixed_queue_dequeue(slot->tx.queue, FIXED_QUEUE_MAX_TIMEOUT));
                        if (fixed_queue_length(slot->tx.queue) <= SLOT_TX_QUEUE_LOW_WM) {
                            xEventGroupSetBits(spp_local_param.tx_event_group, SLOT_WRITE_BIT(serial));
                        }
                    }

                    if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0 &&
                        !p_data->rfc_write.cong) {
                        p_buf->layer_specific = 1;
                        p_buf->event++;
                        BTA_JvRfcommWrite(p_data->rfc_write.handle, slot->id, p_buf->len, p_buf->data + p_buf->offset);
                    }
                } else {
                    if (!p_data->rfc_write.old_cong) {
                        // PORT_WriteDataCO failed
                        BTC_TRACE_ERROR("PORT_WriteDataCO failed p_buf:%p, handle:%d\n", p_buf,
                                        p_data->rfc_write.handle);
                    } else {
                        // need rewrite
                        p_buf->layer_specific = 0;
                    }
                }
            }
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        break;
    case BTA_JV_RFCOMM_CLOSE_EVT:
        param.close.status = p_data->rfc_close.status;
        param.close.port_status = p_data->rfc_close.port_status;
        param.close.handle = p_data->rfc_close.handle;
        param.close.async = p_data->rfc_close.async;
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
            btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->rfc_close.handle);
            if (!slot) {
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                break;
            }
            spp_free_slot(slot);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        } else {
            bool need_call = true;
            do {
                osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
                slot = spp_find_slot_by_handle(p_data->rfc_close.handle);
                if (!slot) {
                    param.close.status = ESP_SPP_NO_CONNECTION;
                    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                    BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                    break;
                }
                // if rx still has data, delay free slot
                if (slot->close_alarm == NULL && slot->rx.queue && fixed_queue_length(slot->rx.queue) > 0) {
                    tBTA_JV *p_arg = NULL;
                    if ((p_arg = malloc(sizeof(tBTA_JV))) == NULL) {
                        param.close.status = ESP_SPP_NO_RESOURCE;
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s unable to malloc slot close_alarm arg!", __func__);
                        break;
                    }
                    memcpy(p_arg, p_data, sizeof(tBTA_JV));
                    if ((slot->close_alarm =
                             osi_alarm_new("slot", close_timeout_handler, (void *)p_arg, VFS_CLOSE_TIMEOUT)) == NULL) {
                        free(p_arg);
                        param.close.status = ESP_SPP_NO_RESOURCE;
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s unable to malloc slot close_alarm!", __func__);
                        break;
                    }
                    if (osi_alarm_set(slot->close_alarm, VFS_CLOSE_TIMEOUT) != OSI_ALARM_ERR_PASS) {
                        free(p_arg);
                        osi_alarm_free(slot->close_alarm);
                        param.close.status = ESP_SPP_BUSY;
                        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                        BTC_TRACE_ERROR("%s set slot close_alarm failed!", __func__);
                        break;
                    }
                    BTC_TRACE_WARNING("%s slot rx data will be discard in %d seconds!", __func__,
                                      VFS_CLOSE_TIMEOUT / 1000);
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
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                break;
            }
            if (!p_data->rfc_cong.cong) {
                BT_HDR *p_buf;
                if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0) {
                    p_buf->event++;
                    p_buf->layer_specific = 1;
                    BTA_JvRfcommWrite(p_data->rfc_cong.handle, slot->id, p_buf->len, p_buf->data + p_buf->offset);
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
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
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
        BTA_JvFree();
        osi_mutex_free(&spp_local_param.spp_slot_mutex);
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
            status = btc_transfer_context(&msg, &p_data, sizeof(tBTA_JV), NULL);
            assert(status == BT_STATUS_SUCCESS);
        } else if (fixed_queue_length(slot->rx.queue) > 2) {
            BTC_TRACE_DEBUG("%s data post stop! %d %d", __func__, slot->rfc_handle, fixed_queue_length(slot->rx.queue));
            ret = 0; // reserved for other flow control
        }
    } else {
        fixed_queue_enqueue(slot->rx.queue, p_buf, FIXED_QUEUE_MAX_TIMEOUT);
        if (fixed_queue_length(slot->rx.queue) > 2) {
            BTC_TRACE_DEBUG("%s data post stop! %d %d", __func__, slot->rfc_handle, fixed_queue_length(slot->rx.queue));
            ret = 0; // reserved for other flow control
        }
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

    ssize_t sent = 0, write_size = 0;
    size_t tx_len;
    BT_HDR *p_buf = NULL;
    bool enqueue_status= false;
    EventBits_t tx_event_group_val = 0;
    while (1) {
        tx_event_group_val = 0;
        if (size) {
            if (p_buf == NULL) {
                write_size = size < BTA_JV_DEF_RFC_MTU ? size : BTA_JV_DEF_RFC_MTU;
                if ((p_buf = osi_malloc(sizeof(BT_HDR) + write_size)) == NULL) {
                    BTC_TRACE_ERROR("%s malloc failed!", __func__);
                    errno = ENOMEM;
                    sent = -1;
                    break;
                }
                p_buf->offset = 0;
                p_buf->len = write_size;
                p_buf->event = 0; // indicate the p_buf be sent count
                p_buf->layer_specific = 0; // indicate the p_buf whether to be sent, 0 - ready to send; 1 - have sent
                memcpy((UINT8 *)(p_buf + 1), data + sent, write_size);
            }
        } else {
            break;
        }

        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
            tx_len = fixed_queue_length(slot->tx.queue);
            enqueue_status = fixed_queue_enqueue(slot->tx.queue, p_buf, 0);
            if (!enqueue_status) {
                BTC_TRACE_DEBUG("%s tx_len:%d, fd:%d\n", __func__, fixed_queue_length(slot->tx.queue), fd);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                //block untill under water level, be closed or time out
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
            }
            if (tx_len == 0) {
                esp_spp_write(slot->rfc_handle, 0, NULL);
            }
            sent += write_size;
            size -= write_size;
            p_buf = NULL;
        } else {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            errno = EPIPE;
            sent = -1;
            break;
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    }

    //errors occur, need to cleanup
    if (p_buf) {
        osi_free(p_buf);
        p_buf = NULL;
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
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!\n", __func__);
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
        if ((slot = spp_local_param.spp_slots[serial]) != NULL) {
            BTA_JvRfcommFlowControl(slot->rfc_handle, count);
            BTC_TRACE_DEBUG("%s give credits:%d\n", __func__, count);
        }
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    }
    return item_size;
}

esp_err_t btc_spp_vfs_register(void)
{
    if (!is_spp_init()) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return ESP_FAIL;
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
        return ESP_FAIL;
    }

    return ESP_OK;
}

#endif ///defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE
