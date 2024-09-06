/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/param.h>
#include <esp_log.h>
#include <esp_gatt_common_api.h>
#include <esp_gap_bt_api.h>

#include <protocomm.h>
#include <protocomm_ble.h>

#include "protocomm_priv.h"
#include "simple_ble.h"

ESP_EVENT_DEFINE_BASE(PROTOCOMM_TRANSPORT_BLE_EVENT);

/* NOTE: For the security2 scheme, the payload size is quite larger
 * than that for security1. The increased value has been selected
 * keeping in mind the largest packet size for security2 and the
 * factors affecting it.
 */
#if CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2
    #define CHAR_VAL_LEN_MAX         (480 + 1)
#else
    #define CHAR_VAL_LEN_MAX         (256 + 1)
#endif // CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_2

#define PREPARE_BUF_MAX_SIZE     CHAR_VAL_LEN_MAX

static const char *TAG = "protocomm_ble";

/* BLE specific configuration parameters */
static const uint16_t primary_service_uuid       = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_user_description = ESP_GATT_UUID_CHAR_DESCRIPTION;
static const uint8_t  character_prop_read_write  = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t  character_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | \
                                                         ESP_GATT_CHAR_PROP_BIT_NOTIFY;

typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t *data_p;
} raw_data_info_t;

typedef struct {
    uint8_t                *prepare_buf;
    int                     prepare_len;
    uint16_t                handle;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;

typedef struct name_uuid128 {
    const char *name;
    uint8_t uuid128[ESP_UUID_LEN_128];
} name_uuid128_t;

typedef struct _protocomm_ble {
    protocomm_t *pc_ble;
    name_uuid128_t *g_nu_lookup;
    ssize_t g_nu_lookup_count;
    uint16_t gatt_mtu;
    uint8_t *service_uuid;
    unsigned ble_link_encryption:1;
    unsigned ble_notify:1;
} _protocomm_ble_internal_t;

static _protocomm_ble_internal_t *protoble_internal;

// config adv data
static esp_ble_adv_data_t adv_config = {
    .set_scan_rsp = false,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = 0,  // Filled later
    .p_service_uuid = NULL, // Filled later
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// config scan response data
static esp_ble_adv_data_t scan_rsp_config = {
    .set_scan_rsp = true,
    .include_name = true,
    .manufacturer_len = 0,          // Filled later
    .p_manufacturer_data = NULL,    // Filler later
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x100,
    .adv_int_max         = 0x100,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static char *protocomm_ble_device_name = NULL;
static uint8_t *protocomm_ble_mfg_data = NULL;
static size_t protocomm_ble_mfg_data_len;
static uint8_t *protocomm_ble_addr = NULL;

static void hexdump(const char *msg, uint8_t *buf, int len)
{
    ESP_LOGD(TAG, "%s:", msg);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_DEBUG);
}

static const uint16_t *uuid128_to_16(const uint8_t *uuid128)
{
    return (const uint16_t *) &uuid128[12];
}

static const char *handle_to_handler(uint16_t handle)
{
    const uint8_t *uuid128 = simple_ble_get_uuid128(handle);
    if (!uuid128) {
        return NULL;
    }
    for (int i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
        if (*uuid128_to_16(protoble_internal->g_nu_lookup[i].uuid128) == *uuid128_to_16(uuid128)) {
            return protoble_internal->g_nu_lookup[i].name;
        }
    }
    return NULL;
}

static void transport_simple_ble_read(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    static const uint8_t *read_buf = NULL;
    static uint16_t read_len = 0;
    static uint16_t max_read_len = 0;
    esp_gatt_status_t status = ESP_OK;

    ESP_LOGD(TAG, "Inside read w/ session - %d on param %d %d",
             param->read.conn_id, param->read.handle, read_len);
    if (!read_len && !param->read.offset) {
        ESP_LOGD(TAG, "Reading attr value first time");
        status = esp_ble_gatts_get_attr_value(param->read.handle, &read_len, &read_buf);
        max_read_len = read_len;
    } else if ((read_len + param->read.offset) > max_read_len) {
        status = ESP_GATT_INVALID_OFFSET;
    } else {
        ESP_LOGD(TAG, "Subsequent read request for attr value");
    }

    esp_gatt_rsp_t gatt_rsp = {0};
    gatt_rsp.attr_value.handle = param->read.handle;
    gatt_rsp.attr_value.offset = param->read.offset;

    if (status == ESP_GATT_OK) {
        gatt_rsp.attr_value.len = MIN(read_len, (protoble_internal->gatt_mtu - 1));
        gatt_rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
        if (gatt_rsp.attr_value.len && read_buf) {
            memcpy(gatt_rsp.attr_value.value,
                   read_buf + param->read.offset,
                   gatt_rsp.attr_value.len);
        }
        read_len -= gatt_rsp.attr_value.len;
    } else {
        read_len = 0;
        max_read_len = 0;
        read_buf = NULL;
    }
    esp_err_t err = esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                    param->read.trans_id, status, &gatt_rsp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Send response error in read");
    }
}

static esp_err_t prepare_write_event_env(esp_gatt_if_t gatts_if,
        esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGD(TAG, "prepare write, handle = %d, value len = %d, offset = %d",
             param->write.handle, param->write.len, param->write.offset);
    esp_gatt_status_t status = ESP_GATT_OK;

    /* Ensure that write data is not larger than max attribute length */
    if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
        status = ESP_GATT_INVALID_OFFSET;
    } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
        status = ESP_GATT_INVALID_ATTR_LEN;
    } else {
        /* If prepare buffer is not allocated, then allocate it */
        if (prepare_write_env.prepare_buf == NULL) {
            prepare_write_env.prepare_len = 0;
            prepare_write_env.prepare_buf = (uint8_t *) malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
            if (prepare_write_env.prepare_buf == NULL) {
                ESP_LOGE(TAG, "%s , failed to allocate prepare buf", __func__);
                status = ESP_GATT_NO_RESOURCES;
            }
        }
    }

    /* If prepare buffer is allocated copy incoming data into it */
    if (status == ESP_GATT_OK) {
        memcpy(prepare_write_env.prepare_buf + param->write.offset,
               param->write.value,
               param->write.len);
        prepare_write_env.prepare_len += param->write.len;
        prepare_write_env.handle = param->write.handle;
    }

    /* Send write response if needed */
    if (param->write.need_rsp) {
        esp_err_t response_err;
        /* If data was successfully appended to prepare buffer
         * only then have it reflected in the response */
        if (status == ESP_GATT_OK) {
            esp_gatt_rsp_t gatt_rsp = {0};
            gatt_rsp.attr_value.len = param->write.len;
            gatt_rsp.attr_value.handle = param->write.handle;
            gatt_rsp.attr_value.offset = param->write.offset;
            gatt_rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            if (gatt_rsp.attr_value.len && param->write.value) {
                memcpy(gatt_rsp.attr_value.value, param->write.value, param->write.len);
            }
            response_err = esp_ble_gatts_send_response(gatts_if,
                           param->write.conn_id, param->write.trans_id, status, &gatt_rsp);
        } else {
            response_err = esp_ble_gatts_send_response(gatts_if,
                           param->write.conn_id, param->write.trans_id, status, NULL);
        }
        if (response_err != ESP_OK) {
            ESP_LOGE(TAG, "Send response error in prep write");
        }
    }
    if (status != ESP_GATT_OK) {
        if (prepare_write_env.prepare_buf) {
            free(prepare_write_env.prepare_buf);
            prepare_write_env.prepare_buf = NULL;
            prepare_write_env.prepare_len = 0;
        }
        return ESP_FAIL;
    }
    return ESP_OK;
}

static void transport_simple_ble_write(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    uint8_t *outbuf = NULL;
    ssize_t outlen = 0;
    esp_err_t ret;

    ESP_LOGD(TAG, "Inside write with session - %d on attr handle = %d \nlen = %d, is_prep = %d",
             param->write.conn_id, param->write.handle, param->write.len, param->write.is_prep);

    if (param->write.is_prep) {
        ret = prepare_write_event_env(gatts_if, param);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error appending to prepare buffer");
        }
        return;
    } else {
        ESP_LOGD(TAG, "is_prep not set");
    }

    ret = protocomm_req_handle(protoble_internal->pc_ble,
                               handle_to_handler(param->write.handle),
                               param->write.conn_id,
                               param->write.value,
                               param->write.len,
                               &outbuf, &outlen);
    if (ret == ESP_OK) {
        ret = esp_ble_gatts_set_attr_value(param->write.handle, outlen, outbuf);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to set the session attribute value");
        }
        ret = esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                          param->write.trans_id, ESP_GATT_OK, NULL);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Send response error in write");
        }
        hexdump("Response from  write", outbuf, outlen);

    } else {
        ESP_LOGE(TAG, "Invalid content received, killing connection");
        esp_ble_gatts_close(gatts_if, param->write.conn_id);
    }
    if (outbuf) {
        free(outbuf);
    }
}

static void transport_simple_ble_exec_write(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t err;
    uint8_t *outbuf = NULL;
    ssize_t outlen = 0;
    ESP_LOGD(TAG, "Inside exec_write w/ session - %d", param->exec_write.conn_id);

    if ((param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC)
            &&
            prepare_write_env.prepare_buf) {
        err = protocomm_req_handle(protoble_internal->pc_ble,
                                   handle_to_handler(prepare_write_env.handle),
                                   param->exec_write.conn_id,
                                   prepare_write_env.prepare_buf,
                                   prepare_write_env.prepare_len,
                                   &outbuf, &outlen);

        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Invalid content received, killing connection");
            esp_ble_gatts_close(gatts_if, param->exec_write.conn_id);
        } else {
            hexdump("Response from exec write", outbuf, outlen);
            esp_ble_gatts_set_attr_value(prepare_write_env.handle, outlen, outbuf);
        }
    }
    if (prepare_write_env.prepare_buf) {
        free(prepare_write_env.prepare_buf);
        prepare_write_env.prepare_buf = NULL;
        prepare_write_env.prepare_len = 0;
    }

    err = esp_ble_gatts_send_response(gatts_if, param->exec_write.conn_id, param->exec_write.trans_id, ESP_GATT_OK, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Send response error in exec write");
    }
    if (outbuf) {
        free(outbuf);
    }
}

static void transport_simple_ble_disconnect(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t ret;
    ESP_LOGD(TAG, "Inside disconnect w/ session - %d", param->disconnect.conn_id);

    /* Ignore BLE events received after protocomm layer is stopped */
    if (protoble_internal == NULL) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    if (protoble_internal->pc_ble->sec &&
            protoble_internal->pc_ble->sec->close_transport_session) {
        ret = protoble_internal->pc_ble->sec->close_transport_session(protoble_internal->pc_ble->sec_inst,
                param->disconnect.conn_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error closing the session after disconnect");
        } else {
            protocomm_ble_event_t ble_event = {};
            /* Assign the event type */
            ble_event.evt_type = PROTOCOMM_TRANSPORT_BLE_DISCONNECTED;
            /* Set the Disconnection handle */
            ble_event.conn_handle = param->disconnect.conn_id;

            if (esp_event_post(PROTOCOMM_TRANSPORT_BLE_EVENT, PROTOCOMM_TRANSPORT_BLE_DISCONNECTED, &ble_event, sizeof(protocomm_ble_event_t), portMAX_DELAY) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post transport disconnection event");
            }
        }
    }
    protoble_internal->gatt_mtu = ESP_GATT_DEF_BLE_MTU_SIZE;
}

static void transport_simple_ble_connect(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t ret;
    ESP_LOGD(TAG, "Inside BLE connect w/ conn_id - %d", param->connect.conn_id);

    /* Ignore BLE events received after protocomm layer is stopped */
    if (protoble_internal == NULL) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    if (protoble_internal->pc_ble->sec &&
            protoble_internal->pc_ble->sec->new_transport_session) {
        ret = protoble_internal->pc_ble->sec->new_transport_session(protoble_internal->pc_ble->sec_inst,
                param->connect.conn_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error creating the session");
        } else {
            protocomm_ble_event_t ble_event = {};
            /* Assign the event type */
            ble_event.evt_type = PROTOCOMM_TRANSPORT_BLE_CONNECTED;
            /* Set the Connection handle */
            ble_event.conn_handle = param->connect.conn_id;

            if (esp_event_post(PROTOCOMM_TRANSPORT_BLE_EVENT, PROTOCOMM_TRANSPORT_BLE_CONNECTED, &ble_event, sizeof(protocomm_ble_event_t), portMAX_DELAY) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post transport pairing event");
            }
        }
    }
}

static void transport_simple_ble_set_mtu(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    protoble_internal->gatt_mtu = param->mtu.mtu;
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

static ssize_t populate_gatt_db(esp_gatts_attr_db_t **gatt_db_generated)
{
    int i;
    /* Each endpoint requires 3 attributes:
     * 1) for Characteristic Declaration
     * 2) for Characteristic Value (for reading and writing to an endpoint)
     * 3) for Characteristic User Description (endpoint name)
     *
     * Therefore, we need esp_gatts_attr_db_t of size 3 * number of endpoints + 1 for service
     */
    ssize_t gatt_db_generated_entries = 3 * protoble_internal->g_nu_lookup_count + 1;

    *gatt_db_generated = (esp_gatts_attr_db_t *) malloc(sizeof(esp_gatts_attr_db_t) *
                         (gatt_db_generated_entries));
    if ((*gatt_db_generated) == NULL) {
        ESP_LOGE(TAG, "Failed to assign memory to gatt_db");
        return -1;
    }
    /* Declare service */
    (*gatt_db_generated)[0].attr_control.auto_rsp       = ESP_GATT_RSP_BY_APP;

    (*gatt_db_generated)[0].att_desc.uuid_length        = ESP_UUID_LEN_16;
    (*gatt_db_generated)[0].att_desc.uuid_p             = (uint8_t *) &primary_service_uuid;
    (*gatt_db_generated)[0].att_desc.perm               = ESP_GATT_PERM_READ;
    (*gatt_db_generated)[0].att_desc.max_length         = ESP_UUID_LEN_128;
    (*gatt_db_generated)[0].att_desc.length             = ESP_UUID_LEN_128;
    (*gatt_db_generated)[0].att_desc.value              = protoble_internal->service_uuid;

    /* Declare characteristics */
    for (i = 1 ; i < gatt_db_generated_entries ; i++) {
        (*gatt_db_generated)[i].attr_control.auto_rsp     = ESP_GATT_RSP_BY_APP;

        if (i % 3 == 1) {
            /* Characteristic Declaration */
            (*gatt_db_generated)[i].att_desc.perm         = ESP_GATT_PERM_READ;
            (*gatt_db_generated)[i].att_desc.uuid_length  = ESP_UUID_LEN_16;
            (*gatt_db_generated)[i].att_desc.uuid_p       = (uint8_t *) &character_declaration_uuid;
            (*gatt_db_generated)[i].att_desc.max_length   = sizeof(uint8_t);
            (*gatt_db_generated)[i].att_desc.length       = sizeof(uint8_t);

	    if (protoble_internal->ble_notify) {
                (*gatt_db_generated)[i].att_desc.value    = (uint8_t *) &character_prop_read_write_notify;
	    } else {
                (*gatt_db_generated)[i].att_desc.value    = (uint8_t *) &character_prop_read_write;
	    }
        } else if (i % 3 == 2) {
            /* Characteristic Value */
            (*gatt_db_generated)[i].att_desc.perm         = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE ;
            if (protoble_internal->ble_link_encryption) {
                (*gatt_db_generated)[i].att_desc.perm     |= ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED;
            }
            (*gatt_db_generated)[i].att_desc.uuid_length  = ESP_UUID_LEN_128;
            (*gatt_db_generated)[i].att_desc.uuid_p       = protoble_internal->g_nu_lookup[i / 3].uuid128;
            (*gatt_db_generated)[i].att_desc.max_length   = CHAR_VAL_LEN_MAX;
            (*gatt_db_generated)[i].att_desc.length       = 0;
            (*gatt_db_generated)[i].att_desc.value        = NULL;
        } else {
            /* Characteristic User Description (for keeping endpoint names) */
            (*gatt_db_generated)[i].att_desc.perm         = ESP_GATT_PERM_READ;
            (*gatt_db_generated)[i].att_desc.uuid_length  = ESP_UUID_LEN_16;
            (*gatt_db_generated)[i].att_desc.uuid_p       = (uint8_t *) &character_user_description;
            (*gatt_db_generated)[i].att_desc.max_length   = strlen(protoble_internal->g_nu_lookup[i / 3 - 1].name);
            (*gatt_db_generated)[i].att_desc.length       = (*gatt_db_generated)[i].att_desc.max_length;
            (*gatt_db_generated)[i].att_desc.value        = (uint8_t *) protoble_internal->g_nu_lookup[i / 3 - 1].name;
        }
    }
    return gatt_db_generated_entries;
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

    if (config->ble_addr != NULL) {
        protocomm_ble_addr = config->ble_addr;
    }

    protoble_internal = (_protocomm_ble_internal_t *) calloc(1, sizeof(_protocomm_ble_internal_t));
    if (protoble_internal == NULL) {
        ESP_LOGE(TAG, "Error allocating internal protocomm structure");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    protoble_internal->g_nu_lookup_count = config->nu_lookup_count;
    protoble_internal->g_nu_lookup = malloc(config->nu_lookup_count * sizeof(name_uuid128_t));
    if (protoble_internal->g_nu_lookup == NULL) {
        ESP_LOGE(TAG, "Error allocating internal name UUID table");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    for (unsigned i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
        memcpy(protoble_internal->g_nu_lookup[i].uuid128, config->service_uuid, ESP_UUID_LEN_128);
        memcpy((uint8_t *)uuid128_to_16(protoble_internal->g_nu_lookup[i].uuid128),
               &config->nu_lookup[i].uuid, ESP_UUID_LEN_16);

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
    protoble_internal->gatt_mtu = ESP_GATT_DEF_BLE_MTU_SIZE;
    protoble_internal->ble_link_encryption = config->ble_link_encryption;
    protoble_internal->ble_notify = config->ble_notify;

    // Config adv data
    adv_config.service_uuid_len = ESP_UUID_LEN_128;
    adv_config.p_service_uuid = (uint8_t *) config->service_uuid;
    protoble_internal->service_uuid = (uint8_t *) config->service_uuid;

    // Config scan response data
    scan_rsp_config.manufacturer_len = protocomm_ble_mfg_data_len;
    scan_rsp_config.p_manufacturer_data = (uint8_t *) protocomm_ble_mfg_data;

    simple_ble_cfg_t *ble_config = simple_ble_init();
    if (ble_config == NULL) {
        ESP_LOGE(TAG, "Ran out of memory for BLE config");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    /* Set function pointers required for simple BLE layer */
    ble_config->read_fn         = transport_simple_ble_read;
    ble_config->write_fn        = transport_simple_ble_write;
    ble_config->exec_write_fn   = transport_simple_ble_exec_write;
    ble_config->disconnect_fn   = transport_simple_ble_disconnect;
    ble_config->connect_fn      = transport_simple_ble_connect;
    ble_config->set_mtu_fn      = transport_simple_ble_set_mtu;

    /* Set parameters required for advertising */
    ble_config->adv_params      = adv_params;

    ble_config->adv_data_p      = &adv_config;
    ble_config->scan_rsp_data_p = &scan_rsp_config;

    ble_config->device_name     = protocomm_ble_device_name;
    ble_config->gatt_db_count   = populate_gatt_db(&ble_config->gatt_db);

    ble_config->ble_bonding = config->ble_bonding;
    ble_config->ble_sm_sc   = config->ble_sm_sc;

   /* Set parameter to keep BLE on */
    ble_config->keep_ble_on = config->keep_ble_on;

    if (config->ble_addr != NULL) {
        ble_config->ble_addr = protocomm_ble_addr;
    }

    if (ble_config->gatt_db_count == -1) {
        ESP_LOGE(TAG, "Invalid GATT database count");
        simple_ble_deinit();
        protocomm_ble_cleanup();
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = simple_ble_start(ble_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "simple_ble_start failed w/ error code 0x%x", err);
        simple_ble_deinit();
        protocomm_ble_cleanup();
        return err;
    }

    prepare_write_env.prepare_buf = NULL;
    ESP_LOGD(TAG, "Waiting for client to connect ......");
    return ESP_OK;
}

esp_err_t protocomm_ble_stop(protocomm_t *pc)
{
    if ((pc != NULL) &&
            (protoble_internal != NULL ) &&
            (pc == protoble_internal->pc_ble)) {
        esp_err_t ret = ESP_OK;

    uint8_t protocomm_keep_ble_on = get_keep_ble_on();
    if (protocomm_keep_ble_on) {
#ifdef CONFIG_ESP_PROTOCOMM_DISCONNECT_AFTER_BLE_STOP
        /* Keep BT stack on, but terminate the connection after provisioning */
	    ret = simple_ble_disconnect();
        if (ret) {
            ESP_LOGE(TAG, "BLE disconnect failed");
	    }
	    simple_ble_deinit();
#endif  // CONFIG_ESP_PROTOCOMM_DISCONNECT_AFTER_BLE_STOP
    }
    else {

	/* If flag is not enabled, stop the stack. */
        ret = simple_ble_stop();
        if (ret) {
            ESP_LOGE(TAG, "BLE stop failed");
        }
        simple_ble_deinit();
    }

        protocomm_ble_cleanup();
        return ret;
    }
    return ESP_ERR_INVALID_ARG;
}
