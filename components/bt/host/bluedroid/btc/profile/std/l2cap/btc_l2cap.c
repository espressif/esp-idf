/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "btc_l2cap.h"
#include "stack/l2c_api.h"
#include "btc/btc_manage.h"
#include "btc/btc_task.h"
#include "bta/bta_jv_api.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "esp_l2cap_bt_api.h"
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
#include "btc_sdp.h"

#include "btc/btc_task.h"
#include "stack/btu.h"

#if (defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE)

#define SLOT_WRITE_BIT(i) (1UL << (i - 1))
#define SLOT_CLOSE_BIT(i) (1UL << (i + BTA_JV_MAX_L2C_CONN - 1))
#define VFS_WRITE_TIMEOUT (40 * 1000)
#define SLOT_TX_QUEUE_SIZE 10
#define SLOT_TX_QUEUE_LOW_WM 4
#define SLOT_TX_DATA_HIGH_WM (SLOT_TX_QUEUE_SIZE * BTA_JV_DEF_RFC_MTU)
#define VFS_CLOSE_TIMEOUT (20 * 1000)
#define BTC_L2CAP_ROLE_MASTER  0
#define BTC_L2CAP_ROLE_SLAVE   1

typedef struct {
    bool peer_fc;         /* true if flow control is set based on peer's request */
    bool user_fc;         /* true if flow control is set based on user's request  */
    fixed_queue_t *queue; /* Queue of buffers waiting to be sent */
    uint32_t data_size;   /* Number of data bytes in the queue */
} slot_data_t;

typedef struct {
    bool connected;
    bool is_server;
    bool fix_chan; // unused
    uint16_t  psm;
    uint8_t serial;
    uint8_t max_session;
    uint32_t id;
    uint32_t handle;
    int fd;
    int tx_mtu;
    uint8_t *write_data;
    osi_alarm_t *close_alarm;
    void *alarm_arg;
    uint8_t role;
    uint16_t security;
    esp_bd_addr_t addr;
    slot_data_t rx;
    slot_data_t tx;
    uint8_t service_uuid[16];
} l2cap_slot_t;


typedef struct {
    l2cap_slot_t *l2cap_slots[BTA_JV_MAX_L2C_CONN + 1];
    uint32_t l2cap_slot_id;
    osi_mutex_t l2cap_slot_mutex;
    EventGroupHandle_t tx_event_group;
    esp_vfs_id_t l2cap_vfs_id;
} l2cap_local_param_t;

static l2cap_local_param_t l2cap_local_param;

/* L2CAP default options for OBEX connections */
static const tL2CAP_FCR_OPTS obex_l2c_fcr_opts_def =
{
    L2CAP_FCR_ERTM_MODE,            /* Mandatory for OBEX over l2cap */
    OBX_FCR_OPT_TX_WINDOW_SIZE_BR_EDR,/* Tx window size */
    OBX_FCR_OPT_MAX_TX_B4_DISCNT,   /* Maximum transmissions before disconnecting */
    OBX_FCR_OPT_RETX_TOUT,          /* Retransmission timeout (2 secs) */
    OBX_FCR_OPT_MONITOR_TOUT,       /* Monitor timeout (12 secs) */
    OBX_FCR_OPT_MAX_PDU_SIZE        /* MPS segment size */
};
static const tL2CAP_ERTM_INFO obex_l2c_etm_opt =
{
    L2CAP_FCR_ERTM_MODE,            /* Mandatory for OBEX over l2cap */
    L2CAP_FCR_CHAN_OPT_ERTM,        /* Mandatory for OBEX over l2cap */
    OBX_USER_RX_POOL_ID,
    OBX_USER_TX_POOL_ID,
    OBX_FCR_RX_POOL_ID,
    OBX_FCR_TX_POOL_ID
};


#if L2CAP_DYNAMIC_MEMORY == FALSE
#define is_l2cap_init() (l2cap_local_param.l2cap_slot_mutex != NULL)
#else
#define is_l2cap_init() (&l2cap_local_param != NULL && l2cap_local_param.l2cap_slot_mutex != NULL)
#endif

static void l2cap_osi_free(void *p)
{
    osi_free(p);
}

static l2cap_slot_t *l2cap_find_slot_by_handle(uint32_t handle)
{
    l2cap_slot_t *slot = NULL;

    for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
        slot = l2cap_local_param.l2cap_slots[i];
        if (slot != NULL && slot->handle == handle) {
            return slot;
        }
    }
    return NULL;
}

static l2cap_slot_t *l2cap_find_slot_by_id(uint32_t id)
{
    l2cap_slot_t *slot = NULL;

    for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
        slot = l2cap_local_param.l2cap_slots[i];
        if (slot != NULL && slot->id == id) {
            return slot;
        }
    }
    return NULL;
}

static l2cap_slot_t *l2cap_find_slot_by_fd(int fd)
{
    l2cap_slot_t *slot = NULL;

    for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
        slot = l2cap_local_param.l2cap_slots[i];
        if (slot != NULL && slot->fd == fd) {
            return slot;
        }
    }
    return NULL;
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
    fixed_queue_free(slot_data->queue, l2cap_osi_free);
    slot_data->queue = NULL;
}

static l2cap_slot_t *l2cap_malloc_slot(void)
{
    uint8_t err_no = 0;
    l2cap_slot_t **slot = NULL;

    if (++l2cap_local_param.l2cap_slot_id == 0) {
        l2cap_local_param.l2cap_slot_id = 1;
    }
    for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
        slot = &l2cap_local_param.l2cap_slots[i];
        if ((*slot) == NULL) {
            if (((*slot) = (l2cap_slot_t *)osi_malloc(sizeof(l2cap_slot_t))) == NULL) {
                return NULL;
            }
            (*slot)->id = l2cap_local_param.l2cap_slot_id;
            (*slot)->psm = 0;
            (*slot)->serial = i;
            (*slot)->handle = 0xffff;
            (*slot)->fd = -1;
            (*slot)->connected = false;
            (*slot)->is_server = false;
            (*slot)->write_data = NULL;
            (*slot)->close_alarm = NULL;
            (*slot)->alarm_arg = NULL;
            /* clear the old event bits */
            if (l2cap_local_param.tx_event_group) {
                xEventGroupClearBits(l2cap_local_param.tx_event_group, SLOT_WRITE_BIT(i) | SLOT_CLOSE_BIT(i));
            }

            if (init_slot_data(&(*slot)->rx, QUEUE_SIZE_MAX)) {
                BTC_TRACE_ERROR("%s unable to malloc rx queue!", __func__);
                err_no = 1;
                goto err;
            }
            if (init_slot_data(&(*slot)->tx, SLOT_TX_QUEUE_SIZE)) {
                BTC_TRACE_ERROR("%s unable to malloc tx queue!", __func__);
                err_no = 2;
                goto err;
            }
            if (esp_vfs_register_fd(l2cap_local_param.l2cap_vfs_id, &(*slot)->fd) != ESP_OK) {
                BTC_TRACE_ERROR("%s unable to register fd!", __func__);
                err_no = 3;
                goto err;
            }

            return (*slot);
        }
    }

    return NULL;
err:
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

static void l2cap_free_slot(l2cap_slot_t *slot)
{
    if (!slot) {
        return;
    }
    l2cap_local_param.l2cap_slots[slot->serial] = NULL;
    esp_vfs_unregister_fd(l2cap_local_param.l2cap_vfs_id, slot->fd);
    xEventGroupSetBits(l2cap_local_param.tx_event_group, SLOT_CLOSE_BIT(slot->serial));
    free_slot_data(&slot->tx);
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

static void l2cap_free_pending_slots(void)
{
    l2cap_slot_t *slot = NULL;
    for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
        slot = l2cap_local_param.l2cap_slots[i];
        if (slot) {
            BTC_TRACE_WARNING("%s found slot(handle=0x%x) pending to close, close it now!", __func__, slot->handle);
            l2cap_free_slot(slot);
        }
    }
}

static void close_timeout_handler(void *arg)
{
    btc_msg_t msg;
    bt_status_t status;
    l2cap_slot_t *slot = (l2cap_slot_t *)arg;

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_L2CAP;
    msg.act = BTA_JV_L2CAP_CLOSE_EVT;

    status = btc_transfer_context(&msg, slot->alarm_arg, sizeof(tBTA_JV), NULL, NULL);

    if (slot->alarm_arg) {
        free(slot->alarm_arg);
        slot->alarm_arg = NULL;
    }

    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed", __func__);
    }
}

static inline void btc_l2cap_cb_to_app(esp_bt_l2cap_cb_event_t event, esp_bt_l2cap_cb_param_t  *param)
{
    esp_bt_l2cap_cb_t btc_l2cap_cb = (esp_bt_l2cap_cb_t)btc_profile_cb_get(BTC_PID_L2CAP);
    if (btc_l2cap_cb) {
        btc_l2cap_cb(event, param);
    }
}

static void *btc_l2cap_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;
    uint32_t id = (uintptr_t)user_data;
    l2cap_slot_t *slot = NULL;

    switch (event) {
    case BTA_JV_L2CAP_OPEN_EVT:
        slot = l2cap_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            p_data->l2c_open.status = ESP_BT_L2CAP_NO_CONNECTION;
            break;
        }
        slot->connected = TRUE;
        slot->handle = p_data->l2c_open.handle;
        slot->tx_mtu = p_data->l2c_open.tx_mtu;
        BTA_JvSetPmProfile(p_data->l2c_open.handle, BTA_JV_PM_ID_1, BTA_JV_CONN_OPEN);
        break;
    case BTA_JV_L2CAP_START_EVT:
        slot = l2cap_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            p_data->l2c_open.status = ESP_BT_L2CAP_NO_CONNECTION;
            break;
        }
        slot->handle = p_data->l2c_start.handle;
        break;
    case BTA_JV_L2CAP_CLOSE_EVT:
        slot = l2cap_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
            p_data->l2c_close.status = ESP_BT_L2CAP_NO_CONNECTION;
            break;
        }
        p_data->l2c_close.status = BTA_JV_SUCCESS;
        p_data->l2c_close.user_data = (void *)(uintptr_t)slot->id;
        break;
    case BTA_JV_L2CAP_CL_INIT_EVT:
        slot = l2cap_find_slot_by_id(id);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            p_data->l2c_cl_init.status = ESP_BT_L2CAP_FAILURE;
            break;
        }

        if (p_data->l2c_cl_init.status == BTA_JV_SUCCESS) {
            slot->handle = p_data->l2c_cl_init.handle;
        } else {
            l2cap_free_slot(slot);
        }
        break;
    case BTA_JV_L2CAP_DATA_IND_EVT:
        // to do
        break;
    case BTA_JV_FREE_SCN_EVT:
        slot = l2cap_find_slot_by_id(id);
        if (slot) {
            l2cap_free_slot(slot);
        } else {
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            p_data->free_scn.status = ESP_BT_L2CAP_NO_CONNECTION;
        }
        break;
    default:
        break;
    }
    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_L2CAP;
    msg.act = event;

    status = btc_transfer_context(&msg, p_data, sizeof(tBTA_JV), NULL, NULL);
    if (status != BT_STATUS_SUCCESS) {
        BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
    }

    return NULL;
}

static void btc_l2cap_dm_inter_cb(tBTA_JV_EVT event, tBTA_JV *p_data, void *user_data)
{
    bt_status_t status;
    btc_msg_t msg;

    switch (event) {
    default:
        msg.sig = BTC_SIG_API_CB;
        msg.pid = BTC_PID_L2CAP;
        msg.act = event;

        status = btc_transfer_context(&msg, p_data, sizeof(tBTA_JV), NULL, NULL);
        if (status != BT_STATUS_SUCCESS) {
            BTC_TRACE_ERROR("%s btc_transfer_context failed\n", __func__);
        }
        break;
    }

    return;
}

static void btc_l2cap_init(void)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;
    esp_bt_l2cap_cb_param_t param;

    do {
        if (is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP has been initiated, shall uninit first!", __func__);
            ret = ESP_BT_L2CAP_NEED_DEINIT;
            break;
        }

#if L2CAP_DYNAMIC_MEMORY == TRUE
        if ((l2cap_local_param_ptr = (l2cap_local_param_t *)osi_malloc(sizeof(l2cap_local_param_t))) == NULL) {
            BTC_TRACE_ERROR("%s malloc failed\n", __func__);
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }
        memset((void *)l2cap_local_param_ptr, 0, sizeof(l2cap_local_param_t));
#endif
        l2cap_local_param.l2cap_vfs_id = -1;

        if (osi_mutex_new(&l2cap_local_param.l2cap_slot_mutex) != 0) {
#if L2CAP_DYNAMIC_MEMORY == TRUE
            osi_free(l2cap_local_param_ptr);
            l2cap_local_param_ptr = NULL;
#endif
            BTC_TRACE_ERROR("%s osi_mutex_new failed\n", __func__);
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }
        if ((l2cap_local_param.tx_event_group = xEventGroupCreate()) == NULL) {
            BTC_TRACE_ERROR("%s create tx_event_group failed\n", __func__);
            osi_mutex_free(&l2cap_local_param.l2cap_slot_mutex);
#if L2CAP_DYNAMIC_MEMORY == TRUE
            osi_free(l2cap_local_param_ptr);
            l2cap_local_param_ptr = NULL;
#endif
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }
        l2cap_local_param.l2cap_slot_id = 0;
        ret = BTA_JvEnable((tBTA_JV_DM_CBACK *)btc_l2cap_dm_inter_cb);
        if (BTA_JV_ALREADY_DONE == ret) {
            ret = ESP_BT_L2CAP_SUCCESS;
            param.init.status = ESP_BT_L2CAP_SUCCESS;
            btc_l2cap_cb_to_app(ESP_BT_L2CAP_INIT_EVT, &param);
        }
    } while (0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        param.init.status = ret;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_INIT_EVT, &param);
    }
}

static void btc_l2cap_uninit(void)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP has not been initiated, shall init first!", __func__);
            ret = ESP_BT_L2CAP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        // first, remove all connection
        for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
            if (l2cap_local_param.l2cap_slots[i] != NULL && !l2cap_local_param.l2cap_slots[i]->is_server) {
                BTA_JvL2capClose(l2cap_local_param.l2cap_slots[i]->handle, (tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb,
                                 (void *)l2cap_local_param.l2cap_slots[i]->id);
            }
        }
        // second, remove all server
        for (size_t i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
            if (l2cap_local_param.l2cap_slots[i] != NULL && l2cap_local_param.l2cap_slots[i]->is_server) {
                if (l2cap_local_param.l2cap_slots[i]->handle != 0xffff) {
                    BTA_JvL2capStopServer(l2cap_local_param.l2cap_slots[i]->psm,
                                           (void *)l2cap_local_param.l2cap_slots[i]->id);
                }

                BTA_JvFreeChannel(l2cap_local_param.l2cap_slots[i]->psm, BTA_JV_CONN_TYPE_L2CAP,
                                      (tBTA_JV_RFCOMM_CBACK *)btc_l2cap_inter_cb, (void *)l2cap_local_param.l2cap_slots[i]->id);
            }
        }
        BTA_JvDisable((tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb);
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    } while(0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        esp_bt_l2cap_cb_param_t param;
        param.uninit.status = ret;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_UNINIT_EVT, &param);
    }
}

static void btc_l2cap_start_srv(btc_l2cap_args_t *arg)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;
    tL2CAP_CFG_INFO cfg;

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_BT_L2CAP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        l2cap_slot_t *slot = l2cap_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc L2CAP slot!", __func__);
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }
        /**
         * make this slot become a listening slot
         */
        slot->is_server = true;
        slot->security = arg->start_srv.sec_mask;
        slot->role = BTC_L2CAP_ROLE_SLAVE;
        slot->psm = arg->start_srv.local_psm;

        /* Setup ETM settings */
        memset(&cfg, 0, sizeof(tL2CAP_CFG_INFO));
        cfg.fcr_present = TRUE;
        cfg.fcr = obex_l2c_fcr_opts_def;
        BTA_JvL2capStartServer(slot->security, slot->role, &obex_l2c_etm_opt, slot->psm,
                                    L2CAP_MAX_SDU_LENGTH, &cfg, (tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb, (void *)slot->id);
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    } while(0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        esp_bt_l2cap_cb_param_t param;
        param.start.status = ret;
        param.start.handle = 0xffff;
        param.start.sec_id = 0;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_START_EVT, &param);
    }
    return;
}

static void btc_l2cap_stop_srv(btc_l2cap_args_t *arg)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;
    bool is_remove_all = false;
    uint8_t i, j, srv_cnt = 0;
    uint8_t *srv_psm_arr = osi_malloc(BTA_JV_MAX_L2C_CONN);

    if (arg->stop_srv.psm == BTC_L2CAP_INVALID_PSM) {
        is_remove_all = true;
    }

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_BT_L2CAP_NEED_INIT;
            break;
        }
        if (srv_psm_arr == NULL) {
            BTC_TRACE_ERROR("%s malloc srv_psm_arr failed\n", __func__);
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }

        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        // [1] find all server
        for (i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
            if (l2cap_local_param.l2cap_slots[i] != NULL && l2cap_local_param.l2cap_slots[i]->is_server &&
                l2cap_local_param.l2cap_slots[i]->handle != 0xffff) {
                if (is_remove_all) {
                    srv_psm_arr[srv_cnt++] = l2cap_local_param.l2cap_slots[i]->psm;
                } else if (l2cap_local_param.l2cap_slots[i]->psm == arg->stop_srv.psm) {
                    srv_psm_arr[srv_cnt++] = l2cap_local_param.l2cap_slots[i]->psm;
                    break;
                }
            }
        }
        if (srv_cnt == 0) {
            if (is_remove_all) {
                BTC_TRACE_ERROR("%s can not find any server!\n", __func__);
            } else {
                BTC_TRACE_ERROR("%s can not find server:%d!\n", __func__, arg->stop_srv.psm);
            }
            ret = ESP_BT_L2CAP_NO_SERVER;
            break;
        }

        // [2] remove all local related connection
        for (j = 0; j < srv_cnt; j++) {
            for (i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
                if (l2cap_local_param.l2cap_slots[i] != NULL && l2cap_local_param.l2cap_slots[i]->connected &&
                    l2cap_local_param.l2cap_slots[i]->handle != 0xffff &&
                    l2cap_local_param.l2cap_slots[i]->psm == srv_psm_arr[j]) {
                    BTA_JvL2capClose(l2cap_local_param.l2cap_slots[i]->handle, (tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb,
                                     (void *)l2cap_local_param.l2cap_slots[i]->id);
                }
            }
        }

        // [3] remove all server
        for (j = 0; j < srv_cnt; j++) {
            for (i = 1; i <= BTA_JV_MAX_L2C_CONN; i++) {
                if (l2cap_local_param.l2cap_slots[i] != NULL && l2cap_local_param.l2cap_slots[i]->is_server &&
                    l2cap_local_param.l2cap_slots[i]->handle != 0xffff &&
                    l2cap_local_param.l2cap_slots[i]->psm == srv_psm_arr[j]) {

                    if (l2cap_local_param.l2cap_slots[i]->handle > 0) {
                        BTA_JvL2capStopServer(l2cap_local_param.l2cap_slots[i]->psm,
                                           (void *)l2cap_local_param.l2cap_slots[i]->id);
                    }

                    BTA_JvFreeChannel(l2cap_local_param.l2cap_slots[i]->psm, BTA_JV_CONN_TYPE_L2CAP,
                                      (tBTA_JV_RFCOMM_CBACK *)btc_l2cap_inter_cb, (void *)l2cap_local_param.l2cap_slots[i]->id);
                }
            }
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    } while (0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        esp_bt_l2cap_cb_param_t param;
        param.srv_stop.status = ret;
        param.srv_stop.psm = BTC_L2CAP_INVALID_PSM;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_SRV_STOP_EVT, &param);
    }

    if (srv_psm_arr) {
        osi_free(srv_psm_arr);
        srv_psm_arr = NULL;
    }
}

static void btc_l2cap_connect(btc_l2cap_args_t *arg)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;
    tL2CAP_CFG_INFO cfg;

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_BT_L2CAP_NEED_INIT;
            break;
        }
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        l2cap_slot_t *slot = l2cap_malloc_slot();
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to malloc L2CAP slot!", __func__);
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            ret = ESP_BT_L2CAP_NO_RESOURCE;
            break;
        }

        slot->security = arg->connect.sec_mask;
        slot->role = BTC_L2CAP_ROLE_MASTER;
        slot->psm = arg->connect.remote_psm;
        memcpy(slot->addr, arg->connect.peer_bd_addr, ESP_BD_ADDR_LEN);
        memset(&cfg, 0, sizeof(tL2CAP_CFG_INFO));
        cfg.fcr_present = TRUE;
        cfg.fcr = obex_l2c_fcr_opts_def;

        BTA_JvL2capConnect(slot->security, slot->role, &obex_l2c_etm_opt, slot->psm,
                            L2CAP_MAX_SDU_LENGTH, &cfg, slot->addr, (tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb, (void *)slot->id);
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    } while (0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        esp_bt_l2cap_cb_param_t param;
        param.open.status = ret;
        param.open.handle = 0;
        param.open.fd = -1;
        param.open.tx_mtu = 0;
        memset(param.open.rem_bda, 0, ESP_BD_ADDR_LEN);
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_OPEN_EVT, &param);
    }
}

static void btc_l2cap_write(uint32_t handle)
{
    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            break;
        }
        l2cap_slot_t *slot = NULL;
        slot = l2cap_find_slot_by_handle(handle);
        if (!slot || (slot && !slot->connected)) {
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find l2cap slot!", __func__);
            } else {
                BTC_TRACE_ERROR("%s l2cap has been disconnected already!", __func__);
            }
            break;
        }

        BT_HDR *p_buf;
        if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0) {
            p_buf->event++;
            p_buf->layer_specific = 1;
            BTA_JvL2capWrite(handle, slot->id, p_buf->data + p_buf->offset, p_buf->len, (void *)slot->id);
        }
    } while (0);
}

static void btc_l2cap_disconnect(uint32_t handle)
{
    esp_bt_l2cap_status_t ret = ESP_BT_L2CAP_SUCCESS;

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_BT_L2CAP_NEED_INIT;
            break;
        }
        l2cap_slot_t *slot = NULL;
        slot = l2cap_find_slot_by_handle(handle);
        if (!slot || (slot && !slot->connected)) {
            if (!slot) {
                BTC_TRACE_ERROR("%s unable to find L2CAP slot! disconnect fail!", __func__);
            } else {
                BTC_TRACE_ERROR("%s L2CAP has been disconnected already!", __func__);
            }
            ret = ESP_BT_L2CAP_NO_CONNECTION;
            break;
        }
        BTA_JvL2capClose(handle, (tBTA_JV_L2CAP_CBACK *)btc_l2cap_inter_cb, (void *)slot->id);
    } while(0);

    if (ret != ESP_BT_L2CAP_SUCCESS) {
        esp_bt_l2cap_cb_param_t param;
        param.close.status = ret;
        param.close.handle = 0;
        param.close.async = FALSE;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_CLOSE_EVT, &param);
    }
}

void btc_l2cap_call_handler(btc_msg_t *msg)
{
    btc_l2cap_args_t *arg = (btc_l2cap_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_L2CAP_ACT_INIT:
        btc_l2cap_init();
        break;
    case BTC_L2CAP_ACT_UNINIT:
        btc_l2cap_uninit();
        break;
    case BTC_L2CAP_ACT_CONNECT:
        btc_l2cap_connect(arg);
        break;
    case BTC_L2CAP_ACT_START_SRV:
        btc_l2cap_start_srv(arg);
        break;
    case BTC_L2CAP_ACT_STOP_SRV:
        btc_l2cap_stop_srv(arg);
        break;
    default:
        BTC_TRACE_ERROR("%s: Unhandled event (%d)!\n", __FUNCTION__, msg->act);
        break;
    }
}

void btc_l2cap_cb_handler(btc_msg_t *msg)
{
    esp_bt_l2cap_cb_param_t param;
    tBTA_JV *p_data = (tBTA_JV *)msg->arg;
    l2cap_slot_t *slot = NULL;
    uint8_t event = msg->act;
    uint8_t serial = 0;
    uint32_t count = 0;

    switch (event) {
    case BTA_JV_ENABLE_EVT:
        param.init.status = p_data->status;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_INIT_EVT, &param);
        break;
    case BTA_JV_DISABLE_EVT:
        param.uninit.status = ESP_BT_L2CAP_SUCCESS;
        l2cap_free_pending_slots();
        BTA_JvFree();
        osi_mutex_free(&l2cap_local_param.l2cap_slot_mutex);
        if (l2cap_local_param.tx_event_group) {
            vEventGroupDelete(l2cap_local_param.tx_event_group);
            l2cap_local_param.tx_event_group = NULL;
        }
        if (l2cap_local_param.l2cap_vfs_id != -1) {
            esp_vfs_unregister_with_id(l2cap_local_param.l2cap_vfs_id);
            l2cap_local_param.l2cap_vfs_id = -1;
        }
#if L2CAP_DYNAMIC_MEMORY == TRUE
        osi_free(l2cap_local_param_ptr);
        l2cap_local_param_ptr = NULL;
#endif
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_UNINIT_EVT, &param);
        break;
    case BTA_JV_L2CAP_OPEN_EVT:
        do {
            osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            slot = l2cap_find_slot_by_handle(p_data->l2c_open.handle);
            if (!slot) {
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
                param.open.status = ESP_BT_L2CAP_NO_CONNECTION;
                break;
            }
            param.open.fd = slot->fd;
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            param.open.status = p_data->l2c_open.status;
        } while (0);
        param.open.handle = p_data->l2c_open.handle;
        param.open.tx_mtu = p_data->l2c_open.tx_mtu;
        memcpy(param.open.rem_bda, p_data->l2c_open.rem_bda, ESP_BD_ADDR_LEN);
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_OPEN_EVT, &param);
        break;
    case BTA_JV_L2CAP_CLOSE_EVT:
        param.close.status = p_data->l2c_close.status;
        param.close.handle = p_data->l2c_close.handle;
        param.close.async = p_data->l2c_close.async;
        bool need_call = true;
        do {
            osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            uint32_t id = (uintptr_t)p_data->l2c_close.user_data;
            slot = l2cap_find_slot_by_id(id);
            if (!slot) {
                param.close.status = ESP_BT_L2CAP_NO_CONNECTION;
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                BTC_TRACE_ERROR("%s unable to find RFCOMM slot, event:%d!", __func__, event);
                break;
            }
            // if rx still has data, delay free slot
            if (slot->close_alarm == NULL && slot->rx.queue && fixed_queue_length(slot->rx.queue) > 0) {
                tBTA_JV *p_arg = NULL;
                if ((p_arg = malloc(sizeof(tBTA_JV))) == NULL) {
                    param.close.status = ESP_BT_L2CAP_NO_RESOURCE;
                    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                    BTC_TRACE_ERROR("%s unable to malloc slot close_alarm arg!", __func__);
                    break;
                }
                memcpy(p_arg, p_data, sizeof(tBTA_JV));
                slot->alarm_arg = (void *)p_arg;
                if ((slot->close_alarm =
                            osi_alarm_new("slot", close_timeout_handler, (void *)slot, VFS_CLOSE_TIMEOUT)) == NULL) {
                    free(p_arg);
                    slot->alarm_arg = NULL;
                    param.close.status = ESP_BT_L2CAP_NO_RESOURCE;
                    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                    BTC_TRACE_ERROR("%s unable to malloc slot close_alarm!", __func__);
                    break;
                }
                if (osi_alarm_set(slot->close_alarm, VFS_CLOSE_TIMEOUT) != OSI_ALARM_ERR_PASS) {
                    free(p_arg);
                    slot->alarm_arg = NULL;
                    osi_alarm_free(slot->close_alarm);
                    param.close.status = ESP_BT_L2CAP_BUSY;
                    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                    BTC_TRACE_ERROR("%s set slot close_alarm failed!", __func__);
                    break;
                }
                BTC_TRACE_WARNING("%s slot rx data will be discard in %d milliseconds!",
                                    __func__, VFS_CLOSE_TIMEOUT);
                slot->connected = false;
                need_call = false;
            }
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        } while (0);

        if (need_call) {
            btc_l2cap_cb_to_app(ESP_BT_L2CAP_CLOSE_EVT, &param);
            osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
            l2cap_free_slot(slot);
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        }
        break;
    case BTA_JV_L2CAP_START_EVT:
        param.start.status = p_data->l2c_start.status;
        param.start.handle = p_data->l2c_start.handle;
        param.start.sec_id = p_data->l2c_start.sec_id;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_START_EVT, &param);
        break;
    case BTA_JV_L2CAP_CL_INIT_EVT:
        param.cl_init.status = p_data->l2c_cl_init.status;
        param.cl_init.handle = p_data->l2c_cl_init.handle;
        param.cl_init.sec_id = p_data->l2c_cl_init.sec_id;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_CL_INIT_EVT, &param);
        break;
    case BTA_JV_L2CAP_DATA_IND_EVT:
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = l2cap_find_slot_by_handle(p_data->data_ind.handle);
        if (!slot) {
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            break;
        }
        if (BTA_JvL2capReady(p_data->data_ind.handle, &count) == BTA_JV_SUCCESS && count > 0) {
            BT_HDR *p_data_buf = osi_malloc(count + sizeof(BT_HDR));
            if (p_data_buf == NULL) {
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                BTC_TRACE_ERROR("%s, %d count = %d malloc failed!", __func__, __LINE__, count);
                break; // to do disconnect
            }
            memset(p_data_buf, 0, count + sizeof(BT_HDR));
            p_data_buf->len = BTA_JvL2capRead(p_data->data_ind.handle, slot->id, p_data_buf->data, count);
            if (p_data_buf->len > 0) {
                fixed_queue_enqueue(slot->rx.queue, p_data_buf, FIXED_QUEUE_MAX_TIMEOUT);
            } else {
                osi_free(p_data_buf);
                break;
            }
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        break;
    case BTA_JV_L2CAP_CONG_EVT:
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = l2cap_find_slot_by_handle(p_data->l2c_cong.handle);
        if (!slot) {
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            break;
        }
        if (!p_data->l2c_cong.cong) {
            BT_HDR *p_buf;
            if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0) {
                p_buf->event++;
                p_buf->layer_specific = 1;
                BTA_JvL2capWrite(p_data->l2c_cong.handle, slot->id, p_buf->data + p_buf->offset, p_buf->len, (void *)slot->id);
            }
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        break;
    case BTA_JV_L2CAP_READ_EVT:
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = l2cap_find_slot_by_handle(p_data->l2c_read.handle);
        if (!slot) {
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            BTC_TRACE_ERROR("%s unable to find L2CAP slot, event:%d!", __func__, event);
            break;
        }
        if (BTA_JvL2capReady(p_data->l2c_read.handle, &count) == BTA_JV_SUCCESS && count > 0) {
            BT_HDR *p_data_buf = osi_malloc(count + sizeof(BT_HDR));
            if (p_data_buf == NULL) {
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                BTC_TRACE_ERROR("%s, %d count = %d malloc failed!", __func__, __LINE__, count);
                break; // to do disconnect
            }
            memset(p_data_buf, 0, count + sizeof(BT_HDR));
            p_data_buf->len = BTA_JvL2capRead(p_data->data_ind.handle, slot->id, p_data_buf->data, count);
            if (p_data_buf->len > 0) {
                fixed_queue_enqueue(slot->rx.queue, p_data_buf, FIXED_QUEUE_MAX_TIMEOUT);
            } else {
                osi_free(p_data_buf);
                break;
            }
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        break;
    case BTA_JV_L2CAP_WRITE_EVT:
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        slot = l2cap_find_slot_by_handle(p_data->l2c_write.handle);
        if (!slot) {
            BTC_TRACE_ERROR("%s unable to find L2CAP slot!, handle:%d", __func__, p_data->l2c_write.handle);
        }
        if (slot) {
            BT_HDR *p_buf;
            serial = slot->serial;
            if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) == NULL) {
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                break;
            }
            if (p_data->l2c_write.status == BTA_JV_SUCCESS) {
                p_buf->len -= p_data->l2c_write.len;
                p_buf->offset += p_data->l2c_write.len;
                p_buf->layer_specific = 0;
                if (p_buf->len == 0) {
                    osi_free(fixed_queue_dequeue(slot->tx.queue, FIXED_QUEUE_MAX_TIMEOUT));
                    if (fixed_queue_length(slot->tx.queue) <= SLOT_TX_QUEUE_LOW_WM) {
                        xEventGroupSetBits(l2cap_local_param.tx_event_group, SLOT_WRITE_BIT(serial));
                    }
                }

                if ((p_buf = fixed_queue_try_peek_first(slot->tx.queue)) != NULL && p_buf->layer_specific == 0 &&
                    !p_data->l2c_write.cong) {
                    p_buf->layer_specific = 1;
                    p_buf->event++;
                    BTA_JvL2capWrite(p_data->l2c_write.handle, slot->id, p_buf->data + p_buf->offset, p_buf->len, (void *)slot->id);
                }
            }
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        break;
    case BTA_JV_FREE_SCN_EVT:
        param.srv_stop.status = p_data->free_scn.status;
        param.srv_stop.psm = p_data->free_scn.scn;
        btc_l2cap_cb_to_app(ESP_BT_L2CAP_SRV_STOP_EVT, &param);
        break;
    default:
        break;
    }

    return;
}

static ssize_t l2cap_vfs_write(int fd, const void * data, size_t size)
{
    assert(data != NULL);
    errno = 0;
    if (size == 0) {
        return 0;
    }
    if (!is_l2cap_init()) {
        BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }

    l2cap_slot_t *slot = NULL;
    uint8_t serial = 0;
    osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = l2cap_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find L2CAP slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);

    ssize_t sent = 0, write_size = 0;
    size_t tx_len;
    BT_HDR *p_buf = NULL;
    bool enqueue_status= false;
    EventBits_t tx_event_group_val = 0;
    while (1) {
        tx_event_group_val = 0;
        if (size) {
            if (p_buf == NULL) {
                write_size = size < slot->tx_mtu ? size : slot->tx_mtu;
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

        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        if ((slot = l2cap_local_param.l2cap_slots[serial]) != NULL) {
            tx_len = fixed_queue_length(slot->tx.queue);
            enqueue_status = fixed_queue_enqueue(slot->tx.queue, p_buf, 0);
            if (!enqueue_status) {
                BTC_TRACE_DEBUG("%s tx_len:%d, fd:%d\n", __func__, fixed_queue_length(slot->tx.queue), fd);
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                //block untill under water level, be closed or time out
                tx_event_group_val =
                    xEventGroupWaitBits(l2cap_local_param.tx_event_group, SLOT_WRITE_BIT(serial) | SLOT_CLOSE_BIT(serial), pdTRUE,
                                        pdFALSE, VFS_WRITE_TIMEOUT / portTICK_PERIOD_MS);
                if (tx_event_group_val & SLOT_CLOSE_BIT(serial)) {
                    BTC_TRACE_ERROR("%s exit for L2CAP close, fd:%d!", __func__, fd);
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
                btc_l2cap_write(slot->handle);
            }
            sent += write_size;
            size -= write_size;
            p_buf = NULL;
        } else {
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            errno = EPIPE;
            sent = -1;
            break;
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    }

    //errors occur, need to cleanup
    if (p_buf) {
        osi_free(p_buf);
        p_buf = NULL;
    }

    return sent;
}

static int l2cap_vfs_close(int fd)
{
    errno = 0;
    if (!is_l2cap_init()) {
        BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }
    l2cap_slot_t *slot = NULL;
    osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = l2cap_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find L2CAP slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    btc_l2cap_disconnect(slot->handle);
    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
    return 0;
}

static ssize_t l2cap_vfs_read(int fd, void * dst, size_t size)
{
    assert(dst != NULL);
    errno = 0;
    if (!is_l2cap_init()) {
        BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
        errno = ESRCH;
        return -1;
    }

    l2cap_slot_t *slot = NULL;
    uint8_t serial = 0;
    osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
    slot = l2cap_find_slot_by_fd(fd);
    if (!slot) {
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
        BTC_TRACE_ERROR("%s unable to find L2CAP slot!", __func__);
        errno = ENOENT;
        return -1;
    }
    serial = slot->serial;
    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);

    ssize_t item_size = 0;
    BT_HDR *p_buf;
    while (1) {
        osi_mutex_lock(&l2cap_local_param.l2cap_slot_mutex, OSI_MUTEX_MAX_TIMEOUT);
        if ((slot = l2cap_local_param.l2cap_slots[serial]) != NULL) {
            if (fixed_queue_length(slot->rx.queue) > 0) {
                // free unused p_buf
                if ((p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) != NULL && p_buf->len == 0) {
                    osi_free(fixed_queue_dequeue(slot->rx.queue, FIXED_QUEUE_MAX_TIMEOUT));
                    p_buf = NULL;
                }
                if (size == 0 || (p_buf = (BT_HDR *)fixed_queue_try_peek_first(slot->rx.queue)) == NULL) {
                    osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                    break;
                }
            } else {
                /**
                 * If close_alarm is not NULL, it means that we have received the BTA_JV_L2CAP_CLOSE_EVT.
                 * And we can trigger close_alarm immediately.
                 */
                if (slot->close_alarm && osi_alarm_is_active(slot->close_alarm)) {
                    osi_alarm_cancel(slot->close_alarm);
                    osi_alarm_set(slot->close_alarm, 0);
                }
                osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
                break;
            }
        } else {
            osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);
            BTC_TRACE_ERROR("%s peer close, data will be discarded!\n", __func__);
            errno = EPIPE;
            item_size = -1;
            break;
        }
        osi_mutex_unlock(&l2cap_local_param.l2cap_slot_mutex);

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
    return item_size;
}

esp_err_t btc_l2cap_vfs_register(void)
{
    esp_err_t ret = ESP_OK;

    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_FAIL;
            break;
        }

        esp_vfs_t vfs = {
            .flags = ESP_VFS_FLAG_DEFAULT,
            .write = l2cap_vfs_write,
            .open = NULL,
            .fstat = NULL,
            .close = l2cap_vfs_close,
            .read = l2cap_vfs_read,
            .fcntl = NULL
        };

        // No FD range is registered here: l2cap_vfs_id is used to register/unregister
        // file descriptors
        if (esp_vfs_register_with_id(&vfs, NULL, &l2cap_local_param.l2cap_vfs_id) != ESP_OK) {
            ret = ESP_FAIL;
            break;
        }
    } while (0);

    return ret;
}

esp_err_t btc_l2cap_vfs_unregister(void)
{
    esp_err_t ret = ESP_OK;
    do {
        if (!is_l2cap_init()) {
            BTC_TRACE_ERROR("%s L2CAP have not been init\n", __func__);
            ret = ESP_FAIL;
            break;
        }

        if (l2cap_local_param.l2cap_vfs_id != -1) {
            if (esp_vfs_unregister_with_id(l2cap_local_param.l2cap_vfs_id) != ESP_OK) {
                ret = ESP_FAIL;
            }
        }
        l2cap_local_param.l2cap_vfs_id = -1;
    } while (0);

    return ret;
}

#endif ///defined BTC_L2CAP_INCLUDED && BTC_L2CAP_INCLUDED == TRUE
