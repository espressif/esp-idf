// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <sys/param.h>
#include <esp_log.h>
#include <esp_gatt_common_api.h>

#include <protocomm.h>
#include <protocomm_ble.h>

#include "protocomm_priv.h"
#include "simple_ble.h"

#define CHAR_VAL_LEN_MAX         (256 + 1)
#define PREPARE_BUF_MAX_SIZE     CHAR_VAL_LEN_MAX

static const char *TAG = "protocomm_ble";

/* BLE specific configuration parameters */
const uint16_t GATTS_SERVICE_UUID_PROV      = 0xFFFF;
const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;
const uint8_t char_prop_read_write          = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

typedef struct {
    uint8_t                *prepare_buf;
    int                     prepare_len;
    uint16_t                handle;
} prepare_type_env_t;

static prepare_type_env_t prepare_write_env;

typedef struct _protocomm_ble {
    protocomm_t *pc_ble;
    protocomm_ble_name_uuid_t *g_nu_lookup;
    ssize_t g_nu_lookup_count;
    uint16_t gatt_mtu;
} _protocomm_ble_internal_t;

static _protocomm_ble_internal_t *protoble_internal;

static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x100,
    .adv_int_max         = 0x100,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static char* protocomm_ble_device_name = NULL;

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x100,
    .max_interval        = 0x100,
    .appearance          = ESP_BLE_APPEARANCE_UNKNOWN,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = 0,
    .p_service_uuid      = NULL,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static void hexdump(const char *msg, uint8_t *buf, int len)
{
    ESP_LOGD(TAG, "%s:", msg);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_DEBUG);
}

static const char *handle_to_handler(uint16_t handle)
{
    uint16_t uuid = simple_ble_get_uuid(handle);
    for (int i = 0; i < protoble_internal->g_nu_lookup_count; i++) {
        if (protoble_internal->g_nu_lookup[i].uuid == uuid ) {
            return protoble_internal->g_nu_lookup[i].name;
        }
    }
    return NULL;
}

static void transport_simple_ble_read(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    static const uint8_t *read_buf = NULL;
    static uint16_t read_len = 0;
    esp_gatt_status_t status = ESP_OK;

    ESP_LOGD(TAG, "Inside read w/ session - %d on param %d %d",
             param->read.conn_id, param->read.handle, read_len);
    if (!read_len) {
        ESP_LOGD(TAG, "Reading attr value first time");
        status = esp_ble_gatts_get_attr_value(param->read.handle, &read_len,  &read_buf);
    } else {
        ESP_LOGD(TAG, "Subsequent read request for attr value");
    }

    esp_gatt_rsp_t gatt_rsp = {0};
    gatt_rsp.attr_value.len = MIN(read_len, (protoble_internal->gatt_mtu - 1));
    gatt_rsp.attr_value.handle = param->read.handle;
    gatt_rsp.attr_value.offset = param->read.offset;
    gatt_rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
    if (gatt_rsp.attr_value.len && read_buf) {
        memcpy(gatt_rsp.attr_value.value,
                read_buf + param->read.offset,
                gatt_rsp.attr_value.len);
    }
    read_len -= gatt_rsp.attr_value.len;
    esp_err_t err = esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                                param->read.trans_id, status, &gatt_rsp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Send response error in read");
    }
}

static esp_err_t prepare_write_event_env(esp_gatt_if_t gatts_if,
                                         esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGD(TAG, "prepare write, handle = %d, value len = %d",
             param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env.prepare_buf == NULL) {
        prepare_write_env.prepare_buf = (uint8_t *) malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        if (prepare_write_env.prepare_buf == NULL) {
            ESP_LOGE(TAG, "%s , failed tp allocate preparebuf", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
        /* prepare_write_env.prepare_len = 0; */
    } else {
        if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    memcpy(prepare_write_env.prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env.prepare_len += param->write.len;
    prepare_write_env.handle = param->write.handle;
    if (param->write.need_rsp) {
        esp_gatt_rsp_t gatt_rsp = {0};
        gatt_rsp.attr_value.len = param->write.len;
        gatt_rsp.attr_value.handle = param->write.handle;
        gatt_rsp.attr_value.offset = param->write.offset;
        gatt_rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
        if (gatt_rsp.attr_value.len && param->write.value) {
            memcpy(gatt_rsp.attr_value.value, param->write.value, param->write.len);
        }
        esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                param->write.trans_id, status,
                &gatt_rsp);
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

    ESP_LOGD(TAG, "Inside write with session - %d on attr handle - %d \nLen -%d IS Prep - %d",
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
                               param->exec_write.conn_id,
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
            esp_ble_gatts_close(gatts_if, param->write.conn_id);
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
    if (protoble_internal->pc_ble->sec &&
        protoble_internal->pc_ble->sec->close_transport_session) {
        ret = protoble_internal->pc_ble->sec->close_transport_session(param->disconnect.conn_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error closing the session after disconnect");
        }
    }
    protoble_internal->gatt_mtu = ESP_GATT_DEF_BLE_MTU_SIZE;
}

static void transport_simple_ble_connect(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t ret;
    ESP_LOGD(TAG, "Inside BLE connect w/ conn_id - %d", param->connect.conn_id);
    if (protoble_internal->pc_ble->sec &&
        protoble_internal->pc_ble->sec->new_transport_session) {
        ret = protoble_internal->pc_ble->sec->new_transport_session(param->connect.conn_id);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "error creating the session");
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
    /* We need esp_gatts_attr_db_t of size 2 * number of handlers + 1 for service */
    ssize_t gatt_db_generated_entries = 2 * protoble_internal->g_nu_lookup_count + 1;

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
    (*gatt_db_generated)[0].att_desc.max_length         = sizeof(uint16_t);
    (*gatt_db_generated)[0].att_desc.length             = sizeof(GATTS_SERVICE_UUID_PROV);
    (*gatt_db_generated)[0].att_desc.value              = (uint8_t *) &GATTS_SERVICE_UUID_PROV;

    /* Declare characteristics */
    for (i = 1 ; i < gatt_db_generated_entries ; i++) {
        (*gatt_db_generated)[i].attr_control.auto_rsp   = ESP_GATT_RSP_BY_APP;

        (*gatt_db_generated)[i].att_desc.uuid_length    = ESP_UUID_LEN_16;
        (*gatt_db_generated)[i].att_desc.perm           = ESP_GATT_PERM_READ |
                                                          ESP_GATT_PERM_WRITE;

        if (i % 2 == 1) { /* Char Declaration */
            (*gatt_db_generated)[i].att_desc.uuid_p     = (uint8_t *) &character_declaration_uuid;
            (*gatt_db_generated)[i].att_desc.max_length = sizeof(uint8_t);
            (*gatt_db_generated)[i].att_desc.length     = sizeof(uint8_t);
            (*gatt_db_generated)[i].att_desc.value      = (uint8_t *) &char_prop_read_write;
        } else { /* Char Value */
            (*gatt_db_generated)[i].att_desc.uuid_p     = (uint8_t *)&protoble_internal->g_nu_lookup[i / 2 - 1].uuid;
            (*gatt_db_generated)[i].att_desc.max_length = CHAR_VAL_LEN_MAX;
            (*gatt_db_generated)[i].att_desc.length     = 0;
            (*gatt_db_generated)[i].att_desc.value      = NULL;
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
    if (adv_data.p_service_uuid) {
        free(adv_data.p_service_uuid);
        adv_data.p_service_uuid = NULL;
        adv_data.service_uuid_len = 0;
    }
}

esp_err_t protocomm_ble_start(protocomm_t *pc, const protocomm_ble_config_t *config)
{
    if (!pc || !config || !config->device_name || !config->nu_lookup) {
        return ESP_ERR_INVALID_ARG;
    }

    if (protoble_internal) {
        ESP_LOGE(TAG, "Protocomm BLE already started");
        return ESP_FAIL;
    }

    /* Store service UUID internally */
    adv_data.service_uuid_len = sizeof(config->service_uuid);
    adv_data.p_service_uuid   = malloc(sizeof(config->service_uuid));
    if (adv_data.p_service_uuid == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for storing service UUID");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }
    memcpy(adv_data.p_service_uuid, config->service_uuid, adv_data.service_uuid_len);

    /* Store BLE device name internally */
    protocomm_ble_device_name = strdup(config->device_name);
    if (protocomm_ble_device_name == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for storing BLE device name");
        protocomm_ble_cleanup();
        return ESP_ERR_NO_MEM;
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
    protoble_internal->gatt_mtu = ESP_GATT_DEF_BLE_MTU_SIZE;

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
    ble_config->adv_data        = adv_data;
    ble_config->adv_params      = adv_params;

    ble_config->device_name     = protocomm_ble_device_name;
    ble_config->gatt_db_count   = populate_gatt_db(&ble_config->gatt_db);

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
        ret = simple_ble_stop();
        if (ret) {
            ESP_LOGE(TAG, "BLE stop failed");
        }
        simple_ble_deinit();
        protocomm_ble_cleanup();
        return ret;
    }
    return ESP_ERR_INVALID_ARG;
}
