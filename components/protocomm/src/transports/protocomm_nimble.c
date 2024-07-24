/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <esp_log.h>
#include <string.h>
#include <inttypes.h>

#include <protocomm.h>
#include <protocomm_ble.h>
#include "protocomm_priv.h"

/* NimBLE */
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"

static const char *TAG = "protocomm_nimble";

ESP_EVENT_DEFINE_BASE(PROTOCOMM_TRANSPORT_BLE_EVENT);

int ble_uuid_flat(const ble_uuid_t *, void *);
static uint8_t ble_uuid_base[BLE_UUID128_VAL_LENGTH];
static int num_chr_dsc;
static uint16_t s_cached_conn_handle;

/*  Standard 16 bit UUID for characteristic User Description*/
#define BLE_GATT_UUID_CHAR_DSC              0x2901

/********************************************************
*       Maintain database for Attribute specific data   *
********************************************************/
struct data_mbuf {
    SLIST_ENTRY(data_mbuf) node;
    uint8_t *outbuf;
    ssize_t outlen;
    uint16_t attr_handle;
};

static SLIST_HEAD(data_mbuf_head, data_mbuf) data_mbuf_list =
    SLIST_HEAD_INITIALIZER(data_mbuf_list);

static struct data_mbuf *find_attr_with_handle(uint16_t attr_handle)
{
    struct data_mbuf *cur;
    SLIST_FOREACH(cur, &data_mbuf_list, node) {
        if (cur->attr_handle == attr_handle) {
            return cur;
        }
    }
    return NULL;
}
/**************************************************************
*         Initialize GAP, protocomm parameters                *
**************************************************************/
static int simple_ble_gap_event(struct ble_gap_event *event, void *arg);
static uint8_t own_addr_type;
void ble_store_config_init(void);

typedef struct _protocomm_ble {
    protocomm_t *pc_ble;
    protocomm_ble_name_uuid_t *g_nu_lookup;
    ssize_t g_nu_lookup_count;
    uint16_t gatt_mtu;
    unsigned ble_link_encryption:1;
    unsigned ble_notify:1;
} _protocomm_ble_internal_t;

static _protocomm_ble_internal_t *protoble_internal;
static struct ble_gap_adv_params adv_params;
static char *protocomm_ble_device_name;
static struct ble_hs_adv_fields adv_data, resp_data;

static uint8_t *protocomm_ble_mfg_data;
static size_t protocomm_ble_mfg_data_len;

static uint8_t *protocomm_ble_addr;
/**********************************************************************
* Maintain database of uuid_name addresses to free memory afterwards  *
**********************************************************************/
struct uuid128_name_buf {
    SLIST_ENTRY(uuid128_name_buf) link;
    ble_uuid128_t *uuid128_name_table;
};

static SLIST_HEAD(uuid128_name_buf_head, uuid128_name_buf) uuid128_name_list =
    SLIST_HEAD_INITIALIZER(uuid128_name_list);

/**********************************************************************
* Initialize simple BLE parameters, advertisement, scan response etc  *
**********************************************************************/
static int
gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *ctxt,
                    void *arg);
static int
gatt_svr_dsc_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *ctxt,
                    void *arg);

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

typedef void (simple_ble_cb_t)(struct ble_gap_event *event, void *arg);
static void transport_simple_ble_connect(struct ble_gap_event *event, void *arg);
static void transport_simple_ble_disconnect(struct ble_gap_event *event, void *arg);
static void transport_simple_ble_set_mtu(struct ble_gap_event *event, void *arg);

typedef struct {
    /** Name to be displayed to devices scanning for ESP32 */
    const char *device_name;
    /** Advertising data content, according to "Supplement to the Bluetooth Core Specification" */
    struct ble_hs_adv_fields adv_data;
    /** Parameters to configure the nature of advertising */
    struct ble_gap_adv_params adv_params;
    /** Descriptor table which consists the configuration required by services and characteristics */
    struct ble_gatt_svc_def *gatt_db;
    /** Client disconnect callback */
    simple_ble_cb_t *disconnect_fn;
    /** Client connect callback */
    simple_ble_cb_t *connect_fn;
    /** MTU set callback */
    simple_ble_cb_t *set_mtu_fn;
    /** BLE bonding */
    unsigned ble_bonding:1;
    /** BLE Secure Connection flag */
    unsigned ble_sm_sc:1;
    /** BLE Link Encryption flag */
    unsigned ble_link_encryption:1;
    /** BLE address */
    uint8_t *ble_addr;
    /**  Flag to keep BLE on */
    unsigned keep_ble_on:1;
    /** BLE Characteristic notify flag */
    unsigned ble_notify:1;
} simple_ble_cfg_t;

static simple_ble_cfg_t *ble_cfg_p;

/************************************************************
* Functions to set and get attr value based on attr Handle  *
************************************************************/
static int simple_ble_gatts_set_attr_value(uint16_t attr_handle, ssize_t outlen,
        uint8_t *outbuf)
{
    struct data_mbuf *attr_mbuf = find_attr_with_handle(attr_handle);
    if (!attr_mbuf) {
        attr_mbuf = calloc(1, sizeof(struct data_mbuf));
        if (!attr_mbuf) {
            ESP_LOGE(TAG, "Failed to allocate memory for storing outbuf and outlen");
            return ESP_ERR_NO_MEM;
        }
        SLIST_INSERT_HEAD(&data_mbuf_list, attr_mbuf, node);
        attr_mbuf->attr_handle = attr_handle;
    } else {
        free(attr_mbuf->outbuf);
    }
    attr_mbuf->outbuf = outbuf;
    attr_mbuf->outlen = outlen;
    return ESP_OK;
}

static int simple_ble_gatts_get_attr_value(uint16_t attr_handle, ssize_t
        *outlen, uint8_t **outbuf)
{
    struct data_mbuf *attr_mbuf = find_attr_with_handle(attr_handle);
    if (!attr_mbuf) {
        ESP_LOGE(TAG, "Outbuf with handle %d not found", attr_handle);
        return ESP_ERR_NOT_FOUND;
    }
    *outbuf = attr_mbuf->outbuf;
    *outlen = attr_mbuf->outlen;
    return ESP_OK;
}

/*****************************************************************************************/
/*                             SIMPLE BLE INTEGRATION                                    */
/*****************************************************************************************/
static void
simple_ble_advertise(void)
{
    int rc;

    adv_data.flags = (BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    adv_data.num_uuids128 = 1;
    adv_data.uuids128_is_complete = 1;

    rc = ble_gap_adv_set_fields(&adv_data);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error setting advertisement data; rc = %d", rc);
        return;
    }

    rc = ble_gap_adv_rsp_set_fields((const struct ble_hs_adv_fields *) &resp_data);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error in setting scan response; rc = %d", rc);
        return;
    }

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    adv_params.itvl_min = 0x100;
    adv_params.itvl_max = 0x100;

    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, simple_ble_gap_event, NULL);
    if (rc != 0) {
        /* If BLE Host is disabled, it probably means device is already
         * provisioned in previous session. Avoid error prints for this case.*/
        if (rc == BLE_HS_EDISABLED) {
            ESP_LOGD(TAG, "BLE Host is disabled !!");
        } else {
            ESP_LOGE(TAG, "Error enabling advertisement; rc = %d", rc);
        }
        return;
    }
    /*   Take note of free heap space  */
    ESP_LOGD(TAG, "Minimum free heap size = %" PRIu32 ", free Heap size = %" PRIu32,
             esp_get_minimum_free_heap_size(), esp_get_free_heap_size());
}

static int
simple_ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            transport_simple_ble_connect(event, arg);
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG, "No open connection with the specified handle");
                return rc;
            }
            s_cached_conn_handle = event->connect.conn_handle;
        } else {
            /* Connection failed; resume advertising. */
            simple_ble_advertise();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGD(TAG, "disconnect; reason=%d ", event->disconnect.reason);
        transport_simple_ble_disconnect(event, arg);
        /* Clear conn_handle value */
        s_cached_conn_handle = 0;
        if (esp_event_post(PROTOCOMM_TRANSPORT_BLE_EVENT, PROTOCOMM_TRANSPORT_BLE_DISCONNECTED, NULL, 0, portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post pairing event");
        }
        /* Connection terminated; resume advertising. */
        simple_ble_advertise();
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        simple_ble_advertise();
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle,
                 event->mtu.channel_id,
                 event->mtu.value);
        transport_simple_ble_set_mtu(event, arg);
        return 0;
    case BLE_GAP_EVENT_NOTIFY_TX:
        ESP_LOGI(TAG, "notify_tx event; conn_handle=%d attr_handle=%d "
                    "status=%d is_indication=%d",
                    event->notify_tx.conn_handle,
                    event->notify_tx.attr_handle,
                    event->notify_tx.status,
                    event->notify_tx.indication);
        return 0;
    }
    return 0;
}

/* Gets `g_nu_lookup name handler` from 128 bit UUID */
static const char *uuid128_to_handler(uint8_t *uuid)
{
    /* Use it to convert 128 bit UUID to 16 bit UUID.*/
    uint8_t *uuid16 = uuid + 12;
    for (int i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
        if (protoble_internal->g_nu_lookup[i].uuid == *(uint16_t *)uuid16 ) {
            ESP_LOGD(TAG, "UUID (0x%x) matched with proto-name = %s", *uuid16, protoble_internal->g_nu_lookup[i].name);
            return protoble_internal->g_nu_lookup[i].name;
        } else {
            ESP_LOGD(TAG, "UUID did not match... %x", *uuid16);
        }
    }
    return NULL;
}

/* Callback to handle GATT characteristic descriptor read */
static int
gatt_svr_dsc_access(uint16_t conn_handle, uint16_t attr_handle, struct
                    ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_DSC) {
        ESP_LOGE(TAG, "Invalid operation on Read-only Descriptor");
        return BLE_ATT_ERR_UNLIKELY;
    }

    int rc;
    ssize_t temp_outlen = strlen(ctxt->dsc->arg);

    rc = os_mbuf_append(ctxt->om, ctxt->dsc->arg, temp_outlen);
    return rc;
}

/* Callback to handle GATT characteristic value Read & Write */
static int
gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                    struct ble_gatt_access_ctxt *ctxt,
                    void *arg)
{
    int rc;
    esp_err_t ret;
    char buf[BLE_UUID_STR_LEN];
    ssize_t temp_outlen = 0;
    uint8_t *temp_outbuf = NULL;
    uint8_t *uuid = NULL;
    uint8_t *data_buf = NULL;
    uint16_t data_len = 0;
    uint16_t data_buf_len = 0;

    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGD(TAG, "Read attempted for characteristic UUID = %s, attr_handle = %d",
                 ble_uuid_to_str(ctxt->chr->uuid, buf), attr_handle);

        rc = simple_ble_gatts_get_attr_value(attr_handle, &temp_outlen,
                                             &temp_outbuf);
        if (rc != 0) {
            ESP_LOGE(TAG, "Characteristic with attr_handle = %d is not added to the list", attr_handle);
            return 0;
        }

        rc = os_mbuf_append(ctxt->om, temp_outbuf, temp_outlen);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* If empty packet is received, return */
        if (ctxt->om->om_len == 0) {
            ESP_LOGD(TAG,"Empty packet");
            return 0;
        }

        uuid = (uint8_t *) calloc(BLE_UUID128_VAL_LENGTH, sizeof(uint8_t));
        if (!uuid) {
            ESP_LOGE(TAG, "Error allocating memory for 128 bit UUID");
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        rc = ble_uuid_flat(ctxt->chr->uuid, uuid);
        if (rc != 0) {
            free(uuid);
            ESP_LOGE(TAG, "Error fetching Characteristic UUID128");
            return rc;
        }

        /* Save the length of entire data */
        data_len = OS_MBUF_PKTLEN(ctxt->om);
        ESP_LOGD(TAG, "Write attempt for uuid = %s, attr_handle = %d, data_len = %d",
                 ble_uuid_to_str(ctxt->chr->uuid, buf), attr_handle, data_len);

        data_buf = calloc(1, data_len);
        if (data_buf == NULL) {
            ESP_LOGE(TAG, "Error allocating memory for characteristic value");
            free(uuid);
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        rc = ble_hs_mbuf_to_flat(ctxt->om, data_buf, data_len, &data_buf_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "Error getting data from memory buffers");
            free(uuid);
            free(data_buf);
            return BLE_ATT_ERR_UNLIKELY;
        }

        ret = protocomm_req_handle(protoble_internal->pc_ble,
                                   uuid128_to_handler(uuid),
                                   conn_handle,
                                   data_buf,
                                   data_buf_len,
                                   &temp_outbuf, &temp_outlen);
        /* Release the 16 bytes allocated for uuid*/
        free(uuid);
        free(data_buf);
        if (ret == ESP_OK) {

            /* Save data address and length outbuf and outlen internally */
            rc = simple_ble_gatts_set_attr_value(attr_handle, temp_outlen,
                                                 temp_outbuf);
            if (rc != 0) {
                ESP_LOGE(TAG, "Failed to set outbuf for characteristic with attr_handle = %d",
                         attr_handle);
                free(temp_outbuf);
            }

            return rc;
        } else {
            ESP_LOGE(TAG, "Invalid content received, killing connection");
            return BLE_ATT_ERR_INVALID_PDU;
        }

    default:
        return BLE_ATT_ERR_UNLIKELY;
    }
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registering service %s with handle=%d TYPE =%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle, ctxt->svc.svc_def->uuid->type);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG, "registering characteristic %s with "
                 "def_handle=%d val_handle=%d , TYPE = %d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle,
                 ctxt->chr.val_handle, ctxt->chr.chr_def->uuid->type);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(const simple_ble_cfg_t *config)
{
    int rc;
    rc = ble_gatts_count_cfg(config->gatt_db);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(config->gatt_db);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

static void
simple_ble_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d", reason);
}

static void
simple_ble_on_sync(void)
{
    int rc;

    if (protocomm_ble_addr) {
         rc = ble_hs_id_set_rnd(protocomm_ble_addr);
	 if (rc != 0) {
             ESP_LOGE(TAG,"Error in setting address");
	     return;
	 }

         rc = ble_hs_util_ensure_addr(1);
	 if (rc != 0) {
             ESP_LOGE(TAG,"Error loading address");
	     return;
	 }
    }
    else {
        rc = ble_hs_util_ensure_addr(0);
        if (rc != 0) {
            ESP_LOGE(TAG, "Error loading address");
            return;
        }
    }

    /* Figure out address to use while advertising (no privacy for now) */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "error determining address type; rc=%d", rc);
        return;
    }

    /* Begin advertising. */
    simple_ble_advertise();
}

void
nimble_host_task(void *param)
{
    /* This function will return only when nimble_port_stop() is executed */
    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();

    nimble_port_freertos_deinit();
}

static int simple_ble_start(const simple_ble_cfg_t *cfg)
{
    ble_cfg_p = (void *)cfg;
    int rc;
    ESP_LOGD(TAG, "Free memory at start of simple_ble_init %" PRIu32, esp_get_free_heap_size());

    rc = nimble_port_init();
    if (rc != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble %d ", rc);
        return rc;
    }

    /* Initialize the NimBLE host configuration. */
    ble_hs_cfg.reset_cb = simple_ble_on_reset;
    ble_hs_cfg.sync_cb = simple_ble_on_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize security manager configuration in NimBLE host  */
    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO; /* Just Works */
    ble_hs_cfg.sm_bonding = cfg->ble_bonding;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_sc = cfg->ble_sm_sc;

    /* Distribute LTK and IRK */
    ble_hs_cfg.sm_our_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;
    ble_hs_cfg.sm_their_key_dist = BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;

    rc = gatt_svr_init(cfg);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error initializing GATT server");
        return rc;
    }

    /* Set device name, configure response data to be sent while advertising */
    rc = ble_svc_gap_device_name_set(cfg->device_name);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error setting device name");
        return rc;
    }

    resp_data.name = (void *) ble_svc_gap_device_name();
    if (resp_data.name != NULL) {
        resp_data.name_len = strlen(ble_svc_gap_device_name());
        resp_data.name_is_complete = 1;
    }

    /* Set manufacturer data if protocomm_ble_mfg_data points to valid data */
    if (protocomm_ble_mfg_data != NULL) {
        resp_data.mfg_data = protocomm_ble_mfg_data;
        resp_data.mfg_data_len = protocomm_ble_mfg_data_len;
        ESP_LOGD(TAG, "Custom manufacturer data length = %d", protocomm_ble_mfg_data_len);
    }

    /* XXX Need to have template for store */
    ble_store_config_init();
    nimble_port_freertos_init(nimble_host_task);

    return 0;
}

/* transport_simple BLE Fn */
static void transport_simple_ble_disconnect(struct ble_gap_event *event, void *arg)
{
    esp_err_t ret;
    ESP_LOGD(TAG, "Inside disconnect w/ session - %d",
             event->disconnect.conn.conn_handle);

    /* Ignore BLE events received after protocomm layer is stopped */
    if (protoble_internal == NULL) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    if (protoble_internal->pc_ble->sec &&
            protoble_internal->pc_ble->sec->close_transport_session) {
        ret =
            protoble_internal->pc_ble->sec->close_transport_session(protoble_internal->pc_ble->sec_inst, event->disconnect.conn.conn_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error closing the session after disconnect");
        } else {
            protocomm_ble_event_t ble_event = {};
            /* Assign the event type */
            ble_event.evt_type = PROTOCOMM_TRANSPORT_BLE_DISCONNECTED;
            /* Set the Disconnection handle */
            ble_event.conn_handle = event->disconnect.conn.conn_handle;
            ble_event.disconnect_reason = event->disconnect.reason;

            if (esp_event_post(PROTOCOMM_TRANSPORT_BLE_EVENT, PROTOCOMM_TRANSPORT_BLE_DISCONNECTED, &ble_event, sizeof(protocomm_ble_event_t), portMAX_DELAY) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post transport disconnection event");
            }
        }
    }
    protoble_internal->gatt_mtu = BLE_ATT_MTU_DFLT;
}

static void transport_simple_ble_connect(struct ble_gap_event *event, void *arg)
{
    esp_err_t ret;
    ESP_LOGD(TAG, "Inside BLE connect w/ conn_id - %d", event->connect.conn_handle);

    /* Ignore BLE events received after protocomm layer is stopped */
    if (protoble_internal == NULL) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    if (protoble_internal->pc_ble->sec &&
            protoble_internal->pc_ble->sec->new_transport_session) {
        ret =
            protoble_internal->pc_ble->sec->new_transport_session(protoble_internal->pc_ble->sec_inst, event->connect.conn_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error creating the session");
        } else {
            protocomm_ble_event_t ble_event = {};
            /* Assign the event type */
            ble_event.evt_type = PROTOCOMM_TRANSPORT_BLE_CONNECTED;
            /* Set the Connection handle */
            ble_event.conn_handle = event->connect.conn_handle;
            ble_event.conn_status = event->connect.status;

            if (esp_event_post(PROTOCOMM_TRANSPORT_BLE_EVENT, PROTOCOMM_TRANSPORT_BLE_CONNECTED, &ble_event, sizeof(protocomm_ble_event_t), portMAX_DELAY) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post transport pairing event");
            }
        }
    }
}

static void transport_simple_ble_set_mtu(struct ble_gap_event *event, void *arg)
{
    protoble_internal->gatt_mtu = event->mtu.value;
    return;
}

static esp_err_t protocomm_ble_add_endpoint(const char *ep_name,
        protocomm_req_handler_t req_handler,
        void *priv_data)
{
    /* Endpoint UUID already added when protocomm_ble_start() was called */
    return ESP_OK;
}

static esp_err_t protocomm_ble_remove_endpoint(const char *ep_name)
{
    /* Endpoint UUID will be removed when protocomm_ble_stop() is called  */
    return ESP_OK;
}

/* Function to add descriptor to characteristic. The value of descriptor is
 * filled with corresponding protocomm endpoint names. Characteristic address,
 * its serial no. and XXX 16 bit standard UUID for descriptor to be provided as
 * input parameters. Returns 0 on success and returns ESP_ERR_NO_MEM on
 * failure. */
static int
ble_gatt_add_char_dsc(struct ble_gatt_chr_def *characteristics, int idx, uint16_t dsc_uuid)
{
    ble_uuid_t *uuid16 = BLE_UUID16_DECLARE(dsc_uuid);

    /* Allocate memory for 2 descriptors, the 2nd descriptor shall be all NULLs
     * to indicate End of Descriptors. */
    (characteristics + idx)->descriptors = (struct ble_gatt_dsc_def *) calloc(2,
                                           sizeof(struct ble_gatt_dsc_def));
    if ((characteristics + idx)->descriptors == NULL) {
        ESP_LOGE(TAG, "Error while allocating memory for characteristic descriptor");
        return ESP_ERR_NO_MEM;
    }

    (characteristics + idx)->descriptors[0].uuid = (ble_uuid_t *) calloc(1,
            sizeof(ble_uuid16_t));
    if ((characteristics + idx)->descriptors[0].uuid == NULL) {
        ESP_LOGE(TAG, "Error while allocating memory for characteristic descriptor");
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)(characteristics + idx)->descriptors[0].uuid, uuid16,
           sizeof(ble_uuid16_t));
    (characteristics + idx)->descriptors[0].att_flags = BLE_ATT_F_READ;
    (characteristics + idx)->descriptors[0].access_cb = gatt_svr_dsc_access;
    (characteristics + idx)->descriptors[0].arg = (void *)
            protoble_internal->g_nu_lookup[idx].name;

    return 0;
}

/* Function to add characteristics to the service. For simplicity the
 * flags and access callbacks are same for all the characteristics. The Fn
 * requires pointer to characteristic of service and index of characteristic,
 * depending upon the index no. individual characteristics can be handled in
 * future. The fn also assumes that the required memory for all characteristics
 * is already allocated while adding corresponding service. Returns 0 on
 * success and returns ESP_ERR_NO_MEM on failure to add characteristic. */
static int
ble_gatt_add_characteristics(struct ble_gatt_chr_def *characteristics, int idx)
{
    /* Prepare 128 bit UUID of characteristics using custom base 128
     * bit UUID and replacing byte 12 and 13 with corresponding protocom
     * endpoint 16 bit UUID value. */
    ble_uuid128_t temp_uuid128_name = {0};
    temp_uuid128_name.u.type = BLE_UUID_TYPE_128;
    memcpy(temp_uuid128_name.value, ble_uuid_base, BLE_UUID128_VAL_LENGTH);
    memcpy(&temp_uuid128_name.value[12], &protoble_internal->g_nu_lookup[idx].uuid, 2);

    (characteristics + idx)->flags = BLE_GATT_CHR_F_READ |
                                     BLE_GATT_CHR_F_WRITE ;

    if (protoble_internal->ble_link_encryption) {
        (characteristics + idx)->flags |= BLE_GATT_CHR_F_READ_ENC |
                                        BLE_GATT_CHR_F_WRITE_ENC;
    }

    if (protoble_internal->ble_notify) {
        (characteristics + idx)->flags |= BLE_GATT_CHR_F_NOTIFY;
    }

    (characteristics + idx)->access_cb = gatt_svr_chr_access;

    /* Out of 128 bit UUID, 16 bits from g_nu_lookup table. Currently
     * g_nu_lookup table has 16 bit UUID, XXX this can be changed to 128 bit UUID
     * in future. For time being continue using 16 bit UUID on top of base 128
     * bit service UUID */
    (characteristics + idx)->uuid = (ble_uuid_t *)calloc(1,
                                    sizeof(ble_uuid128_t));
    if ((characteristics + idx)->uuid == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for characteristic UUID");
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)(characteristics + idx)->uuid, &temp_uuid128_name,
           sizeof(ble_uuid128_t));

    return 0;
}

/* Function to add primary service. It also allocates memory for the
 * characteristics. Returns 0 on success, returns ESP_ERR_NO_MEM on failure to
 * add service. */
static int
ble_gatt_add_primary_svcs(struct ble_gatt_svc_def *gatt_db_svcs, int char_count)
{
    /* Remember the count of characteristics here, as it will be used to free
     * memory afterwards */
    num_chr_dsc = char_count;
    gatt_db_svcs->type = BLE_GATT_SVC_TYPE_PRIMARY;

    /* Allocate (number of characteristics + 1) memory for characteristics, the
     * additional characteristic consist of all 0s indicating end of
     * characteristics */
    gatt_db_svcs->characteristics = (struct ble_gatt_chr_def *) calloc((char_count + 1),
                                    sizeof(struct ble_gatt_chr_def));
    if (gatt_db_svcs->characteristics == NULL) {
        ESP_LOGE(TAG, "Memory allocation for GATT characteristics failed");
        return ESP_ERR_NO_MEM;
    }
    return 0;
}

static int
populate_gatt_db(struct ble_gatt_svc_def **gatt_db_svcs, const protocomm_ble_config_t *config)
{
    /* Allocate memory for 2 services, 2nd to be all NULL indicating end of
     * services */
    *gatt_db_svcs = (struct ble_gatt_svc_def *) calloc(2, sizeof(struct ble_gatt_svc_def));
    if (*gatt_db_svcs == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for GATT services");
        return ESP_ERR_NO_MEM;
    }

    /* Allocate space for 1st service UUID as well, assume length = 128 bit */
    (*gatt_db_svcs)->uuid = (ble_uuid_t *) calloc(1, sizeof(ble_uuid128_t));
    if ((*gatt_db_svcs)->uuid == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for GATT service UUID");
        return ESP_ERR_NO_MEM;
    }

    /* Prepare 128 bit UUID for primary service from config service UUID. */
    ble_uuid128_t uuid128 = {0};
    uuid128.u.type = BLE_UUID_TYPE_128;
    memcpy(uuid128.value, config->service_uuid, BLE_UUID128_VAL_LENGTH);
    memcpy((void *) (*gatt_db_svcs)->uuid, &uuid128, sizeof(ble_uuid128_t));

    /* GATT: Add primary service. */
    int rc = ble_gatt_add_primary_svcs(*gatt_db_svcs, config->nu_lookup_count);
    if (rc != 0) {
        ESP_LOGE(TAG, "Error adding primary service !!!");
        return rc;
    }

    for (int i = 0 ; i < config->nu_lookup_count; i++) {

        /* GATT: Add characteristics to the service at index no. i*/
        rc = ble_gatt_add_characteristics((void *) (*gatt_db_svcs)->characteristics, i);
        if (rc != 0) {
            ESP_LOGE(TAG, "Error adding GATT characteristic !!!");
            return rc;
        }
        /* GATT: Add user description to characteristic no. i*/
        rc = ble_gatt_add_char_dsc((void *) (*gatt_db_svcs)->characteristics,
                                   i, BLE_GATT_UUID_CHAR_DSC);
        if (rc != 0) {
            ESP_LOGE(TAG, "Error adding GATT Descriptor !!");
            return rc;
        }
    }
    return 0;
}

static void protocomm_ble_cleanup(void)
{
    if (protoble_internal) {
        if (protoble_internal->g_nu_lookup) {
            for (unsigned i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
                if (protoble_internal->g_nu_lookup[i].name) {
                    free((void *)protoble_internal->g_nu_lookup[i].name);
                }
            }
            free(protoble_internal->g_nu_lookup);
        }
        free(protoble_internal);
        protoble_internal = NULL;
    }

    if (protocomm_ble_device_name) {
        free(protocomm_ble_device_name);
        protocomm_ble_device_name = NULL;
    }

    if (protocomm_ble_mfg_data) {
        free(protocomm_ble_mfg_data);
        protocomm_ble_mfg_data = NULL;
        protocomm_ble_mfg_data_len = 0;
    }

    if (protocomm_ble_addr) {
        free(protocomm_ble_addr);
        protocomm_ble_addr = NULL;
    }
}

static void free_gatt_ble_misc_memory(simple_ble_cfg_t *ble_config)
{
    if (ble_config == NULL) {
        return;
    }

    /* Free up gatt_db memory if exists */
    if (ble_config->gatt_db && ble_config->gatt_db->characteristics) {
        for (int i = 0; i < num_chr_dsc; i++) {
            if ((ble_config->gatt_db->characteristics + i)->descriptors) {
                free((void *)(ble_config->gatt_db->characteristics + i)->descriptors->uuid);
                free((ble_config->gatt_db->characteristics + i)->descriptors);
            }
            free((void *)(ble_config->gatt_db->characteristics + i)->uuid);
        }
        free((void *)(ble_config->gatt_db->characteristics));
    }

    if (ble_config->gatt_db) {
        free((void *)ble_config->gatt_db->uuid);
        free(ble_config->gatt_db);
    }

    free(ble_config);
    ble_config = NULL;

    /* Free the uuid_name_table struct list if exists */
    struct uuid128_name_buf *cur;
    while (!SLIST_EMPTY(&uuid128_name_list)) {
        cur = SLIST_FIRST(&uuid128_name_list);
        SLIST_REMOVE_HEAD(&uuid128_name_list, link);
        if (cur->uuid128_name_table) {
            free(cur->uuid128_name_table);
        }
        free(cur);
    }

    /* Free the data_mbuf list if exists */
    struct data_mbuf *curr;
    while (!SLIST_EMPTY(&data_mbuf_list)) {
        curr = SLIST_FIRST(&data_mbuf_list);
        SLIST_REMOVE_HEAD(&data_mbuf_list, node);
        free(curr->outbuf);
        free(curr);
    }
}

esp_err_t protocomm_ble_start(protocomm_t *pc, const protocomm_ble_config_t *config)
{
    if (!pc || !config || !config->nu_lookup) {
        return ESP_ERR_INVALID_ARG;
    }

    if (protoble_internal) {
        ESP_LOGE(TAG, "Protocomm BLE already started");
        return ESP_FAIL;
    }

    /* copy the 128 bit service UUID into local buffer to use as base 128 bit
     * UUID. */
    memcpy(ble_uuid_base, config->service_uuid, BLE_UUID128_VAL_LENGTH);

    /* Store 128 bit service UUID internally. */
    ble_uuid128_t *svc_uuid128 = (ble_uuid128_t *)
                                 calloc(1, sizeof(ble_uuid128_t));
    if (svc_uuid128 == NULL) {
        ESP_LOGE(TAG, "Error while allocating memory for 128 bit UUID");
        return ESP_ERR_NO_MEM;
    }
    svc_uuid128->u.type = BLE_UUID_TYPE_128;
    memcpy(svc_uuid128->value, config->service_uuid, BLE_UUID128_VAL_LENGTH);
    adv_data.uuids128 = (void *)svc_uuid128;

    /* Store service uuid128 in SLIST, to free it afterwards */
    struct uuid128_name_buf *temp_uuid128_name_buf = (struct uuid128_name_buf *)
            calloc(1, sizeof(struct uuid128_name_buf));

    if (temp_uuid128_name_buf == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for UUID128 address database");
        return ESP_ERR_NO_MEM;
    }
    SLIST_INSERT_HEAD(&uuid128_name_list, temp_uuid128_name_buf, link);
    temp_uuid128_name_buf->uuid128_name_table = svc_uuid128;

    if (adv_data.uuids128 == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for storing service UUID");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    /* Store BLE device name internally */
    protocomm_ble_device_name = strdup(config->device_name);
    if (protocomm_ble_device_name == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for storing BLE device name");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    /* Store BLE manufacturer data pointer */
    if (config->manufacturer_data != NULL) {
        protocomm_ble_mfg_data = config->manufacturer_data;
        protocomm_ble_mfg_data_len = config->manufacturer_data_len;
    }

    protoble_internal = (_protocomm_ble_internal_t *) calloc(1, sizeof(_protocomm_ble_internal_t));
    if (protoble_internal == NULL) {
        ESP_LOGE(TAG, "Error allocating internal protocomm structure");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    protoble_internal->g_nu_lookup_count =  config->nu_lookup_count;
    protoble_internal->g_nu_lookup = malloc(config->nu_lookup_count * sizeof(protocomm_ble_name_uuid_t));
    if (protoble_internal->g_nu_lookup == NULL) {
        ESP_LOGE(TAG, "Error allocating internal name UUID table");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    for (unsigned i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
        protoble_internal->g_nu_lookup[i].uuid = config->nu_lookup[i].uuid;
        protoble_internal->g_nu_lookup[i].name = strdup(config->nu_lookup[i].name);
        if (protoble_internal->g_nu_lookup[i].name == NULL) {
            ESP_LOGE(TAG, "Error allocating internal name UUID entry");
            protocomm_ble_cleanup();
            return ESP_ERR_NO_MEM;
        }
    }

    pc->add_endpoint = protocomm_ble_add_endpoint;
    pc->remove_endpoint = protocomm_ble_remove_endpoint;
    protoble_internal->pc_ble = pc;
    protoble_internal->gatt_mtu = BLE_ATT_MTU_DFLT;
    protoble_internal->ble_link_encryption = config->ble_link_encryption;
    protoble_internal->ble_notify = config->ble_notify;

    simple_ble_cfg_t *ble_config = (simple_ble_cfg_t *) calloc(1, sizeof(simple_ble_cfg_t));
    if (ble_config == NULL) {
        ESP_LOGE(TAG, "Ran out of memory for BLE config");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    /* Set function pointers required for simple BLE layer */
    ble_config->connect_fn      = transport_simple_ble_connect;
    ble_config->disconnect_fn   = transport_simple_ble_disconnect;
    ble_config->set_mtu_fn      = transport_simple_ble_set_mtu;

    /* Set parameters required for advertising */
    ble_config->adv_data        = adv_data;
    ble_config->adv_params      = adv_params;

    ble_config->device_name     = protocomm_ble_device_name;
    ble_config->ble_bonding     = config->ble_bonding;
    ble_config->ble_sm_sc       = config->ble_sm_sc;

    if (config->ble_addr != NULL) {
        protocomm_ble_addr = config->ble_addr;
    }

    if (populate_gatt_db(&ble_config->gatt_db, config) != 0) {
        ESP_LOGE(TAG, "Error populating GATT Database");
        free_gatt_ble_misc_memory(ble_config);
        return ESP_ERR_NO_MEM;
    }

    ble_config->keep_ble_on = config->keep_ble_on;

    esp_err_t err = simple_ble_start(ble_config);
    ESP_LOGD(TAG, "Free Heap size after simple_ble_start= %" PRIu32, esp_get_free_heap_size());

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "simple_ble_start failed w/ error code 0x%x", err);
        free_gatt_ble_misc_memory(ble_config);
        protocomm_ble_cleanup();
        return err;
    }

    ESP_LOGV(TAG, "Waiting for client to connect ......");
    return ESP_OK;
}

esp_err_t protocomm_ble_stop(protocomm_t *pc)
{
    ESP_LOGD(TAG, "protocomm_ble_stop called here...");
    if ((pc != NULL) &&
            (protoble_internal != NULL ) &&
            (pc == protoble_internal->pc_ble)) {
        esp_err_t ret = ESP_OK;

        esp_err_t rc = ble_gap_adv_stop();
        if (rc) {
            ESP_LOGD(TAG, "Error in stopping advertisement with err code = %d",
                     rc);
        }

    if (ble_cfg_p->keep_ble_on) {
#ifdef CONFIG_ESP_PROTOCOMM_DISCONNECT_AFTER_BLE_STOP
       /* Keep BT stack on, but terminate the connection after provisioning */
       rc = ble_gap_terminate(s_cached_conn_handle, BLE_ERR_REM_USER_CONN_TERM);
       if (rc) {
           ESP_LOGI(TAG, "Error in terminating connection rc = %d",rc);
       }
       free_gatt_ble_misc_memory(ble_cfg_p);
#endif // CONFIG_ESP_PROTOCOMM_DISCONNECT_AFTER_BLE_STOP
    }
    else {
	    /* If flag is enabled, don't stop the stack. User application can start a new advertising to perform its BT activities */
        ret = nimble_port_stop();
        if (ret == 0) {
            nimble_port_deinit();
        }
        free_gatt_ble_misc_memory(ble_cfg_p);
    }

        protocomm_ble_cleanup();
        return ret;
    }
    return ESP_ERR_INVALID_ARG;
}
