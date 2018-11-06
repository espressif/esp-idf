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
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/fcntl.h>
#include "esp_vfs.h"
#include "esp_vfs_dev.h"

#if (defined BTC_SPP_INCLUDED && BTC_SPP_INCLUDED == TRUE)

typedef struct {
    uint8_t serial;
    bool connected;
    uint8_t scn;
    uint8_t max_session;
    RingbufHandle_t ringbuf_read;
    RingbufHandle_t ringbuf_write;
    uint32_t id;
    uint32_t mtu;//unused
    uint32_t sdp_handle;
    uint32_t rfc_handle;
    uint32_t rfc_port_handle;
    int fd;
    uint8_t *write_data;
    esp_spp_role_t role;
    esp_spp_sec_t security;
    esp_bd_addr_t addr;
    list_t *list;
    list_t *incoming_list;
    uint8_t service_uuid[16];
    char service_name[ESP_SPP_SERVER_NAME_MAX + 1];
} spp_slot_t;

typedef struct {
    spp_slot_t *spp_slots[BTA_JV_MAX_RFC_SR_SESSION + 1];
    uint32_t spp_slot_id;
    esp_spp_mode_t spp_mode;
    osi_mutex_t spp_slot_mutex;
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

static spp_slot_t *spp_malloc_slot(void)
{
    if (++spp_local_param.spp_slot_id == 0) {
        spp_local_param.spp_slot_id = 1;
    }
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] == NULL) {
            spp_local_param.spp_slots[i] = (spp_slot_t *)osi_malloc(sizeof(spp_slot_t));
            if (!spp_local_param.spp_slots[i]) {
                return NULL;
            }
            spp_local_param.spp_slots[i]->id = spp_local_param.spp_slot_id;
            spp_local_param.spp_slots[i]->serial = i;
            spp_local_param.spp_slots[i]->connected = FALSE;
            spp_local_param.spp_slots[i]->write_data = NULL;
            spp_local_param.spp_slots[i]->list = list_new(spp_osi_free);
            if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
                spp_local_param.spp_slots[i]->incoming_list = list_new(spp_osi_free);
                if (esp_vfs_register_fd(spp_local_param.spp_vfs_id, &spp_local_param.spp_slots[i]->fd) != ESP_OK) {
                    osi_free(spp_local_param.spp_slots[i]);
                    return NULL;
                }
                spp_local_param.spp_slots[i]->ringbuf_read = xRingbufferCreate(ESP_SPP_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
                spp_local_param.spp_slots[i]->ringbuf_write = xRingbufferCreate(ESP_SPP_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
            }
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_id(uint32_t id)
{
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->id == id) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_handle(uint32_t handle)
{
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->rfc_handle == handle) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static spp_slot_t *spp_find_slot_by_fd(int fd)
{
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->fd == fd) {
            return spp_local_param.spp_slots[i];
        }
    }
    return NULL;
}

static void spp_free_slot(spp_slot_t *slot)
{
    if (!slot) {
        return;
    }
    spp_local_param.spp_slots[slot->serial] = NULL;
    list_free(slot->list);
    if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
        (void) esp_vfs_unregister_fd(spp_local_param.spp_vfs_id, slot->fd);
        list_free(slot->incoming_list);
        vRingbufferDelete(slot->ringbuf_read);
        vRingbufferDelete(slot->ringbuf_write);
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
    btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
}

static void btc_disconnect_cb(uint32_t handle)
{
    esp_spp_cb_param_t param;
    param.close.status = ESP_SPP_SUCCESS;
    param.close.handle = handle;
    param.close.async = FALSE;
    btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
}

static void *btc_spp_rfcomm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;
    void *new_user_data = NULL;

    uint32_t id = (uintptr_t)user_data;
    spp_slot_t *slot, *slot_new;
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    switch (event) {
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            break;
        }
        slot_new = spp_malloc_slot();
        if (!slot_new) {
            BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
            break;
        }
        new_user_data = (void *)(uintptr_t)slot_new->id;
        slot_new->security = slot->security;
        slot_new->role = slot->role;
        slot_new->scn = slot->scn;;
        slot_new->max_session = slot->max_session;
        strcpy(slot_new->service_name, slot->service_name);
        slot_new->sdp_handle = slot->sdp_handle;
        slot_new->rfc_handle = p_data->rfc_srv_open.new_listen_handle;
        slot_new->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.new_listen_handle);

        memcpy(slot->addr, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
        slot->connected = TRUE;
        slot->rfc_handle = p_data->rfc_srv_open.handle;
        slot->rfc_port_handle = BTA_JvRfcommGetPortHdl(p_data->rfc_srv_open.handle);
        BTA_JvSetPmProfile(p_data->rfc_srv_open.handle, BTA_JV_PM_ALL, BTA_JV_CONN_OPEN);
        break;
    case BTA_JV_RFCOMM_OPEN_EVT:
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
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
            break;
        }
        if (slot->connected) {
            BTA_JvRfcommClose(slot->rfc_handle, (void *)slot->id);
        }
        spp_free_slot(slot);
        p_data->rfc_close.status = BTA_JV_SUCCESS;
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
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
    spp_slot_t *slot;
    switch (event) {
    case BTA_JV_GET_SCN_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }
        if (p_data->scn == 0) {
            BTC_TRACE_ERROR("%s unable to get scn, start server fail!", __func__);
            btc_create_server_fail_cb();
            spp_free_slot(slot);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }
        if (p_data->create_rec.status == BTA_JV_SUCCESS) {
            slot->sdp_handle = p_data->create_rec.handle;
            BTA_JvRfcommStartServer(slot->security, slot->role, slot->scn,
                                    slot->max_session, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
        } else {
            BTC_TRACE_ERROR("%s unable to create record, start server fail!", __func__);
            btc_create_server_fail_cb();
            BTA_JvFreeChannel(slot->scn, BTA_JV_CONN_TYPE_RFCOMM);
            spp_free_slot(slot);
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

#if SPP_DYNAMIC_MEMORY == TRUE
    if ((spp_local_param_ptr = (spp_local_param_t *)osi_malloc(sizeof(spp_local_param_t))) == NULL) {
        BTC_TRACE_ERROR("%s malloc failed\n", __func__);
        return;
    }
    memset((void *)spp_local_param_ptr, 0, sizeof(spp_local_param_t));
#endif

    if (osi_mutex_new(&spp_local_param.spp_slot_mutex) != 0) {
        BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
        return;
    }
    spp_local_param.spp_mode = arg->init.mode;
    spp_local_param.spp_slot_id = 0;
    BTA_JvEnable((tBTA_JV_DM_CBACK *)btc_spp_dm_inter_cb);
}

static void btc_spp_uninit(void)
{
    if (!spp_local_param.spp_slot_mutex) {
        BTC_TRACE_ERROR("%s SPP have not been init\n", __func__);
        return;
    }
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] != NULL && spp_local_param.spp_slots[i]->connected) {
            BTA_JvRfcommClose(spp_local_param.spp_slots[i]->rfc_handle, (void *)spp_local_param.spp_slots[i]->id);
            spp_free_slot(spp_local_param.spp_slots[i]);
            spp_local_param.spp_slots[i] = NULL;
        }
    }
    for (size_t i = 1; i <= BTA_JV_MAX_RFC_SR_SESSION; i++) {
        if (spp_local_param.spp_slots[i] != NULL && !(spp_local_param.spp_slots[i]->connected)) {
            BTA_JvRfcommStopServer(spp_local_param.spp_slots[i]->sdp_handle, (void *)spp_local_param.spp_slots[i]->id);
            BTA_JvDeleteRecord(spp_local_param.spp_slots[i]->sdp_handle);
            BTA_JvFreeChannel(spp_local_param.spp_slots[i]->scn, BTA_JV_CONN_TYPE_RFCOMM);
            spp_free_slot(spp_local_param.spp_slots[i]);
            spp_local_param.spp_slots[i] = NULL;
        }
    }

    BTA_JvDisable();
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    osi_mutex_free(&spp_local_param.spp_slot_mutex);

#if SPP_DYNAMIC_MEMORY == TRUE
    osi_free(spp_local_param_ptr);
    spp_local_param_ptr = NULL;
#endif
}

static void btc_spp_start_discovery(btc_spp_args_t *arg)
{
    BTA_JvStartDiscovery(arg->start_discovery.bd_addr, arg->start_discovery.num_uuid, arg->start_discovery.p_uuid_list, NULL);
}

static void btc_spp_connect(btc_spp_args_t *arg)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_malloc_slot();
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return;
    }
    slot->security = arg->connect.sec_mask;
    slot->role = arg->connect.role;
    slot->scn = arg->connect.remote_scn;;
    memcpy(slot->addr, arg->connect.peer_bd_addr, ESP_BD_ADDR_LEN);
    BTA_JvRfcommConnect(arg->connect.sec_mask, arg->connect.role, arg->connect.remote_scn,
                        arg->connect.peer_bd_addr, (tBTA_JV_RFCOMM_CBACK *)btc_spp_rfcomm_inter_cb, (void *)slot->id);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
}

static void btc_spp_disconnect(btc_spp_args_t *arg)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_handle(arg->disconnect.handle);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot! disconnect fail!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return;
    }
    BTA_JvRfcommClose(arg->disconnect.handle, (void *)slot->id);
    btc_disconnect_cb(slot->rfc_handle);
    spp_free_slot(slot);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
}

static void btc_spp_start_srv(btc_spp_args_t *arg)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_malloc_slot();
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to malloc RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return;
    }
    slot->security = arg->start_srv.sec_mask;
    slot->role = arg->start_srv.role;
    slot->scn = arg->start_srv.local_scn;;
    slot->max_session = arg->start_srv.max_session;
    strcpy(slot->service_name, arg->start_srv.name);

    BTA_JvGetChannelId(BTA_JV_CONN_TYPE_RFCOMM, (void *)slot->id, arg->start_srv.local_scn);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
}

static void btc_spp_write(btc_spp_args_t *arg)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_handle(arg->write.handle);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return;
    }
    if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
        size_t item_size = 0;
        if (slot->write_data != NULL) {
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            return;
        }
        uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0,990);
        if (item_size != 0){
            slot->write_data = data;
            BTA_JvRfcommWrite(arg->write.handle, slot->id, item_size, data);
        }
    } else {
        list_append(slot->list, arg->write.p_data);
        BTA_JvRfcommWrite(arg->write.handle, slot->id, arg->write.len, arg->write.p_data);
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
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
    spp_slot_t *slot;
    switch (msg->act) {
    case BTA_JV_ENABLE_EVT:
        param.init.status = p_data->status;
        btc_spp_cb_to_app(ESP_SPP_INIT_EVT, &param);
        break;
    case BTA_JV_DISCOVERY_COMP_EVT:
        param.disc_comp.status = p_data->disc_comp.status;
        param.disc_comp.scn_num = p_data->disc_comp.scn_num;
        memcpy(param.disc_comp.scn, p_data->disc_comp.scn, p_data->disc_comp.scn_num);
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
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->rfc_open.handle);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
            param.open.fd = slot->fd;
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        }
        param.open.status = p_data->rfc_open.status;
        param.open.handle = p_data->rfc_open.handle;
        memcpy(param.open.rem_bda, p_data->rfc_open.rem_bda, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_OPEN_EVT, &param);
        break;
    case BTA_JV_RFCOMM_START_EVT:
        param.start.status = p_data->rfc_start.status;
        param.start.handle = p_data->rfc_start.handle;
        param.start.sec_id = p_data->rfc_start.sec_id;
        param.start.use_co = p_data->rfc_start.use_co;
        btc_spp_cb_to_app(ESP_SPP_START_EVT, &param);
        break;
    case BTA_JV_RFCOMM_SRV_OPEN_EVT:
        if (spp_local_param.spp_mode == ESP_SPP_MODE_VFS) {
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = spp_find_slot_by_handle(p_data->rfc_srv_open.handle);
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
            param.srv_open.fd = slot->fd;
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        }
        param.srv_open.status = p_data->rfc_srv_open.status;
        param.srv_open.handle = p_data->rfc_srv_open.handle;
        param.srv_open.new_listen_handle = p_data->rfc_srv_open.new_listen_handle;
        memcpy(param.srv_open.rem_bda, p_data->rfc_srv_open.rem_bda, ESP_BD_ADDR_LEN);
        btc_spp_cb_to_app(ESP_SPP_SRV_OPEN_EVT, &param);
        break;
    case BTA_JV_RFCOMM_WRITE_EVT:
        osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = spp_find_slot_by_handle(p_data->rfc_write.handle);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            break;
        }
        if (spp_local_param.spp_mode == ESP_SPP_MODE_CB){
            param.write.status = p_data->rfc_write.status;
            param.write.handle = p_data->rfc_write.handle;
            param.write.len = p_data->rfc_write.len;
            param.write.cong = p_data->rfc_write.cong;
            btc_spp_cb_to_app(ESP_SPP_WRITE_EVT, &param);
            list_remove(slot->list, list_front(slot->list));
        } else {
            if (p_data->rfc_write.status != BTA_JV_SUCCESS) {
                if (slot->write_data != NULL){
                    vRingbufferReturnItem(slot->ringbuf_write,slot->write_data);
                    slot->write_data = NULL;
                }
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
            if (p_data->rfc_write.cong == 0) {
                if (slot->write_data != NULL){
                    vRingbufferReturnItem(slot->ringbuf_write,slot->write_data);
                    slot->write_data = NULL;
                }
                size_t item_size = 0;
                uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0,990);
                if (item_size != 0){
                    slot->write_data = data;
                    BTA_JvRfcommWrite(slot->rfc_handle, slot->id, item_size, data);
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
        btc_spp_cb_to_app(ESP_SPP_CLOSE_EVT, &param);
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
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
                osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
                break;
            }
            if (p_data->rfc_cong.cong == 0) {
                if (slot->write_data != NULL){
                    vRingbufferReturnItem(slot->ringbuf_write,slot->write_data);
                    slot->write_data = NULL;
                }
                size_t item_size = 0;
                uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_write, &item_size, 0,990);
                if (item_size != 0){
                    slot->write_data = data;
                    BTA_JvRfcommWrite(slot->rfc_handle, slot->id, item_size, data);
                }
            }
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        }
        break;
    case BTA_JV_RFCOMM_DATA_IND_EVT:
        param.data_ind.status = ESP_SPP_SUCCESS;
        param.data_ind.handle = p_data->data_ind.handle;
        if (p_data->data_ind.p_buf) {
            param.data_ind.len = p_data->data_ind.p_buf->len;
            param.data_ind.data = p_data->data_ind.p_buf->data + p_data->data_ind.p_buf->offset;
        } else {
            param.data_ind.len = 0;
            param.data_ind.data = NULL;
        }

        btc_spp_cb_to_app(ESP_SPP_DATA_IND_EVT, &param);
        osi_free (p_data->data_ind.p_buf);
        break;
    default:
        BTC_TRACE_DEBUG("%s: Unhandled event (%d)!", __FUNCTION__, msg->act);
        break;
    }

}

/**
 * If there is too much data not accepted, wait until accepted,or discard it when timeout!
 * @param list  list
 * @param p_buf incoming data
 */
static void spp_delay_append(list_t *list, BT_HDR *p_buf){
    uint8_t count = 0;
    while (count++ < 100) {
        if (list_length(list) < 200){
            osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            list_append(list, p_buf);
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            return;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    osi_free(p_buf);
    BTC_TRACE_WARNING("%s There is too much data not accepted, discard it!", __func__);
}

int bta_co_rfc_data_incoming(void *user_data, BT_HDR *p_buf)
{
    bt_status_t status;
    tBTA_JV p_data;
    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_SPP;
    msg.act = BTA_JV_RFCOMM_DATA_IND_EVT;

    uint32_t id = (uintptr_t)user_data;
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_id(id);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return 0;
    }
    p_data.data_ind.handle = slot->rfc_handle;

    if (spp_local_param.spp_mode == ESP_SPP_MODE_CB) {
        p_data.data_ind.p_buf = p_buf;
        status = btc_transfer_context(&msg, &p_data,
                                      sizeof(tBTA_JV), NULL);
        if (status != BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
        }
    } else {
        if (list_is_empty(slot->incoming_list)) {
            BaseType_t done = xRingbufferSend(slot->ringbuf_read, p_buf->data + p_buf->offset, p_buf->len, 0);
            if (done) {
                osi_free (p_buf);
            } else {
                list_append(slot->incoming_list, p_buf);
            }
        } else {
            list_t *list = slot->incoming_list;
            osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
            spp_delay_append(list,p_buf);
            return 1;
        }
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return 1;
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
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    BaseType_t done = xRingbufferSend(slot->ringbuf_write, (void *)data, size, 0);
    esp_spp_write(slot->rfc_handle, 0, NULL);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    if (done){
        return size;
    } else {
        return 0;
    }

}
static int spp_vfs_close(int fd)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    esp_spp_disconnect(slot->rfc_handle);
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return 0;
}

static bool incoming_list_2_ringbuf_read(spp_slot_t *slot)
{
    while (!list_is_empty(slot->incoming_list)) {
        BT_HDR *p_buf = list_front(slot->incoming_list);
        BaseType_t done = xRingbufferSend(slot->ringbuf_read, p_buf->data + p_buf->offset, p_buf->len, 0);
        if (done) {
            list_remove(slot->incoming_list, p_buf);
        } else {
            return false;
        }
    }
    return true;
}

static ssize_t spp_vfs_read(int fd, void * dst, size_t size)
{
    osi_mutex_lock(&spp_local_param.spp_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    spp_slot_t *slot = spp_find_slot_by_fd(fd);
    if (!slot) {
        BTC_TRACE_ERROR("%s unable to find RFCOMM slot!", __func__);
        osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
        return -1;
    }
    incoming_list_2_ringbuf_read(slot);
    size_t item_size = 0;
    uint8_t *data = xRingbufferReceiveUpTo(slot->ringbuf_read, &item_size, 0, size);
    if (item_size > 0){
        memcpy(dst, data, item_size);
        vRingbufferReturnItem(slot->ringbuf_read, data);
    }
    osi_mutex_unlock(&spp_local_param.spp_slot_mutex);
    return item_size;
}

esp_err_t btc_spp_vfs_register()
{
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
