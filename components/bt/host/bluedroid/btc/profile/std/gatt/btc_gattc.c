/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD

 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "stack/btm_ble_api.h"
#include "btc_gattc.h"
#include "btc_gatt_util.h"
#include "btc/btc_manage.h"
#include "bta/bta_gatt_api.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"
#include "esp_gattc_api.h"

#if (GATTC_INCLUDED == TRUE)
static inline void btc_gattc_cb_to_app(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    esp_gattc_cb_t btc_gattc_cb = (esp_gattc_cb_t )btc_profile_cb_get(BTC_PID_GATTC);
    if (btc_gattc_cb) {
	btc_gattc_cb(event, gattc_if, param);
    }
}

void btc_gattc_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src)
{
    btc_ble_gattc_args_t *dst = (btc_ble_gattc_args_t *) p_dest;
    btc_ble_gattc_args_t *src = (btc_ble_gattc_args_t *)p_src;

    switch (msg->act) {
    case BTC_GATTC_ACT_WRITE_CHAR: {
        dst->write_char.value = (uint8_t *)osi_malloc(src->write_char.value_len);
        if (dst->write_char.value) {
            memcpy(dst->write_char.value, src->write_char.value, src->write_char.value_len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    case BTC_GATTC_ACT_WRITE_CHAR_DESCR: {
        dst->write_descr.value = (uint8_t *)osi_malloc(src->write_descr.value_len);
        if (dst->write_descr.value) {
            memcpy(dst->write_descr.value, src->write_descr.value, src->write_descr.value_len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    case BTC_GATTC_ACT_PREPARE_WRITE: {
        dst->prep_write.value = (uint8_t *)osi_malloc(src->prep_write.value_len);
        if (dst->prep_write.value) {
            memcpy(dst->prep_write.value, src->prep_write.value, src->prep_write.value_len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    case BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR: {
        dst->prep_write_descr.value = (uint8_t *)osi_malloc(src->prep_write_descr.value_len);
        if (dst->prep_write_descr.value) {
            memcpy(dst->prep_write_descr.value, src->prep_write_descr.value, src->prep_write_descr.value_len);
        } else {
            BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep copy %d\n", __func__, msg->act);
        break;
    }
}

void btc_gattc_arg_deep_free(btc_msg_t *msg)
{
    btc_ble_gattc_args_t *arg = (btc_ble_gattc_args_t *)msg->arg;

    switch (msg->act) {
    case BTC_GATTC_ACT_WRITE_CHAR: {
        if (arg->write_char.value) {
            osi_free(arg->write_char.value);
        }
        break;
    }
    case BTC_GATTC_ACT_WRITE_CHAR_DESCR: {
        if (arg->write_descr.value) {
            osi_free(arg->write_descr.value);
        }
        break;
    }
    case BTC_GATTC_ACT_PREPARE_WRITE: {
        if (arg->prep_write.value) {
            osi_free(arg->prep_write.value);
        }
        break;
    }
    case BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR: {
        if (arg->prep_write_descr.value) {
            osi_free(arg->prep_write_descr.value);
        }
        break;
    }
    default:
        BTC_TRACE_DEBUG("%s Unhandled deep free %d\n", __func__, msg->act);
        break;
    }

}

static void btc_gattc_copy_req_data(btc_msg_t *msg, void *p_dest, void *p_src)
{
    tBTA_GATTC *p_dest_data = (tBTA_GATTC *) p_dest;
    tBTA_GATTC *p_src_data = (tBTA_GATTC *) p_src;

    if (!p_src_data || !p_dest_data || !msg) {
        return;
    }

    // Allocate buffer for request data if necessary
    switch (msg->act) {
        case BTA_GATTC_READ_DESCR_EVT:
        case BTA_GATTC_READ_CHAR_EVT: 
        case BTA_GATTC_READ_MULTIPLE_EVT: {
            if (p_src_data->read.p_value && p_src_data->read.p_value->p_value) {
                p_dest_data->read.p_value = (tBTA_GATT_UNFMT  *)osi_malloc(sizeof(tBTA_GATT_UNFMT) + p_src_data->read.p_value->len);
                p_dest_data->read.p_value->p_value = (uint8_t *)(p_dest_data->read.p_value + 1);
                if (p_dest_data->read.p_value && p_dest_data->read.p_value->p_value) {
                    p_dest_data->read.p_value->len = p_src_data->read.p_value->len;
                    memcpy(p_dest_data->read.p_value->p_value, p_src_data->read.p_value->p_value, p_src_data->read.p_value->len);
                } else {
                    BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
                }
            }
            break;
        }
        case BTA_GATTC_GET_ADDR_LIST_EVT: {
            if (p_src_data->get_addr_list.bda_list != NULL) {
                uint8_t num_addr = p_src_data->get_addr_list.num_addr;
                p_dest_data->get_addr_list.bda_list = (BD_ADDR *)osi_malloc(sizeof(BD_ADDR) * num_addr);
                if (p_dest_data->get_addr_list.bda_list) {
                    memcpy(p_dest_data->get_addr_list.bda_list, p_src_data->get_addr_list.bda_list, sizeof(BD_ADDR) * num_addr);
                } else {
                    BTC_TRACE_ERROR("%s %d no mem\n", __func__, msg->act);
                }
            }
            break;
        }
        default:
            break;
    }
}

static void btc_gattc_free_req_data(btc_msg_t *msg)
{
    tBTA_GATTC *arg = (tBTA_GATTC *)(msg->arg);
    switch (msg->act) {
        case BTA_GATTC_READ_DESCR_EVT:
        case BTA_GATTC_READ_CHAR_EVT: 
        case BTA_GATTC_READ_MULTIPLE_EVT: {
            if (arg->read.p_value) {
                osi_free(arg->read.p_value);
            }
            break;
        }
        case BTA_GATTC_GET_ADDR_LIST_EVT: {
            if (arg->get_addr_list.bda_list) {
                osi_free(arg->get_addr_list.bda_list);
            }
            break;
        }
        default:
            break;
    }
    return;
}

static void btc_gattc_cback(tBTA_GATTC_EVT event, tBTA_GATTC *p_data)
{
    bt_status_t ret;
    btc_msg_t msg = {0};

    msg.sig = BTC_SIG_API_CB;
    msg.pid = BTC_PID_GATTC;
    msg.act = (uint8_t) event;
    ret = btc_transfer_context(&msg, p_data, sizeof(tBTA_GATTC),
                                btc_gattc_copy_req_data, btc_gattc_free_req_data);

    if (ret) {
        BTC_TRACE_ERROR("%s transfer failed\n", __func__);
    }
}

static void btc_gattc_app_register(btc_ble_gattc_args_t *arg)
{
    tBT_UUID app_uuid;
    app_uuid.len = 2;
    app_uuid.uu.uuid16 = arg->app_reg.app_id;
    BTA_GATTC_AppRegister(&app_uuid, btc_gattc_cback);
}

static void btc_gattc_app_unregister(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_AppDeregister(arg->app_unreg.gattc_if);
}

static void btc_gattc_open(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_TRANSPORT transport = BTA_GATT_TRANSPORT_LE;
    BTA_GATTC_Open(arg->open.gattc_if, arg->open.remote_bda, arg->open.remote_addr_type, arg->open.is_direct, transport);
}

static void btc_gattc_close(btc_ble_gattc_args_t *arg)
{
    // TODO; Review this call of BTA_API, check the usage of BTA_GATTC_CancelOpen
    BTA_GATTC_Close(arg->close.conn_id);
}

static void btc_gattc_cfg_mtu(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ConfigureMTU (arg->cfg_mtu.conn_id);
}

static esp_gatt_status_t btc_gattc_check_valid_param(int num, uint16_t offset)
{
    if (num == 0) {
        return ESP_GATT_NOT_FOUND;
    } else if (offset >= num) {
        return ESP_GATT_INVALID_OFFSET;
    }

    return ESP_GATT_OK;
}

static void btc_gattc_fill_gatt_db_conversion(uint16_t count, uint16_t num, esp_gatt_db_attr_type_t type,
                                                                  uint16_t offset, void *result, btgatt_db_element_t *db)
{
    tBT_UUID bta_uuid = {0};
    uint16_t db_size = (count + offset > num) ? (num - offset) : count;
    switch(type) {
        case ESP_GATT_DB_PRIMARY_SERVICE:
        case ESP_GATT_DB_SECONDARY_SERVICE: {
            esp_gattc_service_elem_t *svc_result = (esp_gattc_service_elem_t *)result;
            for (int i = 0; i < db_size; i++) {
                svc_result->is_primary = (db[offset + i].type == BTGATT_DB_PRIMARY_SERVICE) ? true : false;
                svc_result->start_handle = db[offset + i].start_handle;
                svc_result->end_handle = db[offset + i].end_handle;
                btc128_to_bta_uuid(&bta_uuid, db[offset + i].uuid.uu);
                bta_to_btc_uuid(&svc_result->uuid, &bta_uuid);
                svc_result++;
            }
            break;
        }
        case ESP_GATT_DB_CHARACTERISTIC: {
            esp_gattc_char_elem_t *char_result = (esp_gattc_char_elem_t *)result;
            for (int i = 0; i < db_size; i++) {
                char_result->char_handle = db[offset + i].attribute_handle;
                char_result->properties = db[offset + i].properties;
                btc128_to_bta_uuid(&bta_uuid, db[offset + i].uuid.uu);
                bta_to_btc_uuid(&char_result->uuid, &bta_uuid);
                char_result++;
            }
            break;
        }
        case ESP_GATT_DB_DESCRIPTOR: {
            esp_gattc_descr_elem_t *descr_result = (esp_gattc_descr_elem_t *)result;
            for (int i = 0; i < db_size; i++) {
                descr_result->handle = db[offset + i].attribute_handle;
                btc128_to_bta_uuid(&bta_uuid, db[offset + i].uuid.uu);
                bta_to_btc_uuid(&descr_result->uuid, &bta_uuid);
                descr_result++;
            }
            break;
        }
        case ESP_GATT_DB_INCLUDED_SERVICE: {
            esp_gattc_incl_svc_elem_t *incl_result = (esp_gattc_incl_svc_elem_t *)result;
            for (int i = 0; i < db_size; i++) {
                incl_result->handle = db[offset + i].attribute_handle;
                incl_result->incl_srvc_s_handle = db[offset + i].start_handle;
                incl_result->incl_srvc_e_handle = db[offset + i].end_handle;
                btc128_to_bta_uuid(&bta_uuid, db[offset + i].uuid.uu);
                bta_to_btc_uuid(&incl_result->uuid, &bta_uuid);
                incl_result++;
            }
            break;
        }
        default:
            BTC_TRACE_WARNING("%s(), Not support type(%d)", __func__, type);
            break;
    }
}

static void btc_gattc_search_service(btc_ble_gattc_args_t *arg)
{
    tBT_UUID srvc_uuid;

    if (arg->search_srvc.filter_uuid_enable) {
        btc_to_bta_uuid(&srvc_uuid, &arg->search_srvc.filter_uuid);
        BTA_GATTC_ServiceSearchRequest(arg->search_srvc.conn_id, &srvc_uuid);
    } else {
        BTA_GATTC_ServiceSearchRequest(arg->search_srvc.conn_id, NULL);
    }
}

esp_gatt_status_t btc_ble_gattc_get_service(uint16_t conn_id, esp_bt_uuid_t *svc_uuid,
                                                           esp_gattc_service_elem_t *result,
                                                           uint16_t *count, uint16_t offset)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t svc_num = 0;
    tBT_UUID *bta_uuid = NULL;
    if (svc_uuid) {
        bta_uuid = osi_malloc(sizeof(tBT_UUID));
        btc_to_bta_uuid(bta_uuid, svc_uuid);
    }

    BTA_GATTC_GetServiceWithUUID(conn_id, bta_uuid, &db, &svc_num);

    if ((status = btc_gattc_check_valid_param((int)svc_num, offset)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        if (bta_uuid) {
            osi_free(bta_uuid);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, svc_num, ESP_GATT_DB_PRIMARY_SERVICE, offset, (void *)result, db);
    }

    *count = svc_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    if (bta_uuid) {
        osi_free(bta_uuid);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_all_char(uint16_t conn_id,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             esp_gattc_char_elem_t *result,
                                             uint16_t *count, uint16_t offset)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t char_num = 0;
    BTA_GATTC_GetAllChar(conn_id, start_handle, end_handle, &db, &char_num);

    if ((status = btc_gattc_check_valid_param((int)char_num, offset)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, char_num, ESP_GATT_DB_CHARACTERISTIC, offset, (void *)result, db);
    }

    *count = char_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_all_descr(uint16_t conn_id,
                                              uint16_t char_handle,
                                              esp_gattc_descr_elem_t *result,
                                              uint16_t *count, uint16_t offset)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t descr_num = 0;
    BTA_GATTC_GetAllDescriptor(conn_id, char_handle, &db, &descr_num);

    if ((status = btc_gattc_check_valid_param((int)descr_num, offset)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, descr_num, ESP_GATT_DB_DESCRIPTOR, offset, (void *)result, db);
    }

    *count = descr_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_char_by_uuid(uint16_t conn_id,
                                                 uint16_t start_handle,
                                                 uint16_t end_handle,
                                                 esp_bt_uuid_t char_uuid,
                                                 esp_gattc_char_elem_t *result,
                                                 uint16_t *count)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t char_num = 0;
    tBT_UUID bta_uuid = {0};
    btc_to_bta_uuid(&bta_uuid, &char_uuid);
    BTA_GATTC_GetCharByUUID(conn_id, start_handle, end_handle, bta_uuid, &db, &char_num);

    if ((status = btc_gattc_check_valid_param((int)char_num, 0)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, char_num, ESP_GATT_DB_CHARACTERISTIC, 0, (void *)result, db);
    }

    *count = char_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_descr_by_uuid(uint16_t conn_id,
                                                  uint16_t start_handle,
                                                  uint16_t end_handle,
                                                  esp_bt_uuid_t char_uuid,
                                                  esp_bt_uuid_t descr_uuid,
                                                  esp_gattc_descr_elem_t *result,
                                                  uint16_t *count)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t descr_num = 0;
    tBT_UUID bta_char_uuid = {0};
    tBT_UUID bta_descr_uuid = {0};
    btc_to_bta_uuid(&bta_char_uuid, &char_uuid);
    btc_to_bta_uuid(&bta_descr_uuid, &descr_uuid);

    BTA_GATTC_GetDescrByUUID(conn_id, start_handle, end_handle,
                             bta_char_uuid, bta_descr_uuid, &db, &descr_num);

    if ((status = btc_gattc_check_valid_param((int)descr_num, 0)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, descr_num, ESP_GATT_DB_DESCRIPTOR, 0, (void *)result, db);
    }

    *count = descr_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_descr_by_char_handle(uint16_t conn_id,
                                                         uint16_t char_handle,
                                                         esp_bt_uuid_t descr_uuid,
                                                         esp_gattc_descr_elem_t *result,
                                                         uint16_t *count)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t descr_num = 0;
    tBT_UUID bta_descr_uuid = {0};
    btc_to_bta_uuid(&bta_descr_uuid, &descr_uuid);

    BTA_GATTC_GetDescrByCharHandle(conn_id, char_handle, bta_descr_uuid, &db, &descr_num);

    if ((status = btc_gattc_check_valid_param((int)descr_num, 0)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    } else {
        btc_gattc_fill_gatt_db_conversion(*count, descr_num, ESP_GATT_DB_DESCRIPTOR, 0, (void *)result, db);
    }

    *count = descr_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;

}

esp_gatt_status_t btc_ble_gattc_get_include_service(uint16_t conn_id,
                                                    uint16_t start_handle,
                                                    uint16_t end_handle,
                                                    esp_bt_uuid_t *incl_uuid,
                                                    esp_gattc_incl_svc_elem_t *result,
                                                    uint16_t *count)
{
    esp_gatt_status_t status;
    btgatt_db_element_t *db = NULL;
    uint16_t incl_num = 0;
    tBT_UUID bta_uuid = {0};

    if (incl_uuid != NULL) {
        btc_to_bta_uuid(&bta_uuid, incl_uuid);
        BTA_GATTC_GetIncludeService(conn_id, start_handle, end_handle, &bta_uuid, &db, &incl_num);
    } else {
        BTA_GATTC_GetIncludeService(conn_id, start_handle, end_handle, NULL, &db, &incl_num);
    }

    if ((status = btc_gattc_check_valid_param((int)incl_num, 0)) != ESP_GATT_OK) {
        if (db) {
            osi_free(db);
        }
        *count = 0;
        return status;
    }else {
        btc_gattc_fill_gatt_db_conversion(*count, incl_num, ESP_GATT_DB_INCLUDED_SERVICE, 0, (void *)result, db);
    }

    *count = incl_num;
    //don't forget to free the db buffer after used.
    if (db) {
        osi_free(db);
    }
    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_attr_count(uint16_t conn_id,
                                               esp_gatt_db_attr_type_t type,
                                               uint16_t start_handle,
                                               uint16_t end_handle,
                                               uint16_t char_handle,
                                               uint16_t *count)
{
    if (type == ESP_GATT_DB_ALL) {
        BTA_GATTC_GetDBSize(conn_id, start_handle, end_handle, count);
    } else {
        BTA_GATTC_GetDBSizeByType(conn_id, type, start_handle, end_handle, char_handle, count);
    }

    return ESP_GATT_OK;
}

esp_gatt_status_t btc_ble_gattc_get_db(uint16_t conn_id, uint16_t start_handle, uint16_t end_handle,
                                       esp_gattc_db_elem_t *db, uint16_t *count)
{
    btgatt_db_element_t *get_db = NULL;
    uint16_t num = 0;
    tBT_UUID bta_uuid;
    uint16_t db_size = 0;
    BTA_GATTC_GetGattDb(conn_id, start_handle, end_handle, &get_db, &num);

    if (num == 0) {
        if (get_db) {
            osi_free(get_db);
        }
        *count = 0;
        return ESP_GATT_NOT_FOUND;
    }

    db_size = (*count > num) ? num : (*count);
    for (int i = 0; i < db_size; i++) {
        db[i].type = get_db[i].type;
        db[i].attribute_handle = get_db[i].id;
        db[i].start_handle = get_db[i].start_handle;
        db[i].end_handle = get_db[i].end_handle;
        db[i].properties = get_db[i].properties;
        btc128_to_bta_uuid(&bta_uuid, get_db[i].uuid.uu);
        bta_to_btc_uuid(&db[i].uuid, &bta_uuid);
    }
    *count = db_size;
    //don't forget to free the db buffer after used.
    if (get_db) {
        osi_free(get_db);
    }
    return ESP_GATT_OK;
}

static void btc_gattc_read_char(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ReadCharacteristic(arg->read_char.conn_id, arg->read_char.handle, arg->read_char.auth_req);
}

static void btc_gattc_read_multiple_char(btc_ble_gattc_args_t *arg)
{
    tBTA_GATTC_MULTI bta_multi;
    bta_multi.num_attr = arg->read_multiple.num_attr;
    memcpy(bta_multi.handles, arg->read_multiple.handles, BTA_GATTC_MULTI_MAX);
    BTA_GATTC_ReadMultiple(arg->read_multiple.conn_id, &bta_multi, arg->read_multiple.auth_req);
}

static void btc_gattc_read_char_descr(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ReadCharDescr(arg->read_descr.conn_id, arg->read_descr.handle, arg->read_descr.auth_req);
}

static void btc_gattc_read_by_type(btc_ble_gattc_args_t *arg)
{
    tBT_UUID uuid;
    btc_to_bta_uuid(&uuid, &(arg->read_by_type.uuid));
    BTA_GATTC_Read_by_type(arg->read_by_type.conn_id, arg->read_by_type.s_handle, arg->read_by_type.e_handle, &uuid, arg->read_by_type.auth_req);
}

static void btc_gattc_write_char(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_WriteCharValue(arg->write_char.conn_id,
                             arg->write_char.handle,
                             arg->write_char.write_type,
                             arg->write_char.value_len,
                             arg->write_char.value,
                             arg->write_char.auth_req);
}

static void btc_gattc_write_char_descr(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_UNFMT descr_val;

    descr_val.len = arg->write_descr.value_len;
    descr_val.p_value = arg->write_descr.value;

    BTA_GATTC_WriteCharDescr(arg->write_descr.conn_id,
                             arg->write_descr.handle,
                             arg->write_descr.write_type, &descr_val,
                             arg->write_descr.auth_req);
}

static void btc_gattc_prepare_write(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_PrepareWrite(arg->prep_write.conn_id,
                           arg->prep_write.handle,
                           arg->prep_write.offset,
                           arg->prep_write.value_len,
                           arg->prep_write.value,
                           arg->prep_write.auth_req);
}
static void btc_gattc_prepare_write_char_descr(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_UNFMT descr_val;

    descr_val.len = arg->prep_write_descr.value_len;
    descr_val.p_value = arg->prep_write_descr.value;
    BTA_GATTC_PrepareWriteCharDescr(arg->prep_write_descr.conn_id,
                                    arg->prep_write_descr.handle,
                                    arg->prep_write_descr.offset,
                                    &descr_val,
                                    arg->prep_write_descr.auth_req);
}

static void btc_gattc_execute_write(btc_ble_gattc_args_t *arg)
{
    BTA_GATTC_ExecuteWrite(arg->exec_write.conn_id, arg->exec_write.is_execute);
}

static void btc_gattc_reg_for_notify(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_STATUS status;
    esp_ble_gattc_cb_param_t param;

    status = BTA_GATTC_RegisterForNotifications(arg->reg_for_notify.gattc_if,
             arg->reg_for_notify.remote_bda,
             arg->reg_for_notify.handle);

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.reg_for_notify.status = status;
    param.reg_for_notify.handle = arg->reg_for_notify.handle;
    btc_gattc_cb_to_app(ESP_GATTC_REG_FOR_NOTIFY_EVT, arg->reg_for_notify.gattc_if, &param);
}

static void btc_gattc_unreg_for_notify(btc_ble_gattc_args_t *arg)
{
    tBTA_GATT_STATUS status;
    esp_ble_gattc_cb_param_t param;

    status = BTA_GATTC_DeregisterForNotifications(arg->unreg_for_notify.gattc_if,
             arg->unreg_for_notify.remote_bda,
             arg->unreg_for_notify.handle);

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));
    param.unreg_for_notify.status = status;
    param.unreg_for_notify.handle = arg->unreg_for_notify.handle;
    btc_gattc_cb_to_app(ESP_GATTC_UNREG_FOR_NOTIFY_EVT, arg->unreg_for_notify.gattc_if, &param);
}

void btc_gattc_call_handler(btc_msg_t *msg)
{
    btc_ble_gattc_args_t *arg = (btc_ble_gattc_args_t *)(msg->arg);
    switch (msg->act) {
    case BTC_GATTC_ACT_APP_REGISTER:
        btc_gattc_app_register(arg);
        break;
    case BTC_GATTC_ACT_APP_UNREGISTER:
        btc_gattc_app_unregister(arg);
        break;
    case BTC_GATTC_ACT_OPEN:
        btc_gattc_open(arg);
        break;
    case BTC_GATTC_ACT_CLOSE:
        btc_gattc_close(arg);
        break;
    case BTC_GATTC_ACT_CFG_MTU:
        btc_gattc_cfg_mtu(arg);
        break;
    case BTC_GATTC_ACT_SEARCH_SERVICE:
        btc_gattc_search_service(arg);
        break;
    case BTC_GATTC_ACT_READ_CHAR:
        btc_gattc_read_char(arg);
        break;
    case BTC_GATTC_ACT_READ_MULTIPLE_CHAR:
        btc_gattc_read_multiple_char(arg);
        break;
    case BTC_GATTC_ACT_READ_CHAR_DESCR:
        btc_gattc_read_char_descr(arg);
        break;
    case BTC_GATTC_ACT_READ_BY_TYPE:
        btc_gattc_read_by_type(arg);
        break;
    case BTC_GATTC_ACT_WRITE_CHAR:
        btc_gattc_write_char(arg);
        break;
    case BTC_GATTC_ACT_WRITE_CHAR_DESCR:
        btc_gattc_write_char_descr(arg);
        break;
    case BTC_GATTC_ACT_PREPARE_WRITE:
        btc_gattc_prepare_write(arg);
        break;
    case BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR:
        btc_gattc_prepare_write_char_descr(arg);
        break;
    case BTC_GATTC_ACT_EXECUTE_WRITE:
        btc_gattc_execute_write(arg);
        break;
    case BTC_GATTC_ACT_REG_FOR_NOTIFY:
        btc_gattc_reg_for_notify(arg);
        break;
    case BTC_GATTC_ACT_UNREG_FOR_NOTIFY:
        btc_gattc_unreg_for_notify(arg);
        break;
    case BTC_GATTC_ACT_CACHE_REFRESH:
        BTA_GATTC_Refresh(arg->cache_refresh.remote_bda, true);
        break;
    case BTC_GATTC_ACT_CACHE_ASSOC:
        BTA_GATTC_CacheAssoc(arg->cache_assoc.gattc_if,
                                arg->cache_assoc.src_addr,
                                arg->cache_assoc.assoc_addr,
                                arg->cache_assoc.is_assoc);
        break;
    case BTC_GATTC_ATC_CACHE_GET_ADDR_LIST:
        BTA_GATTC_CacheGetAddrList(arg->get_addr_list.gattc_if);
        break;
    case BTC_GATTC_ACT_CACHE_CLEAN:
        BTA_GATTC_Clean(arg->cache_clean.remote_bda);
        break;
    default:
        BTC_TRACE_ERROR("%s: Unhandled event (%d)!\n", __FUNCTION__, msg->act);
        break;
    }

    btc_gattc_arg_deep_free(msg);
}

void btc_gattc_cb_handler(btc_msg_t *msg)
{
    tBTA_GATTC *arg = (tBTA_GATTC *)(msg->arg);
    esp_gatt_if_t gattc_if = 0;
    esp_ble_gattc_cb_param_t param = {0};

    memset(&param, 0, sizeof(esp_ble_gattc_cb_param_t));

    switch (msg->act) {
    case BTA_GATTC_REG_EVT: {
        tBTA_GATTC_REG *reg_oper = &arg->reg_oper;

        gattc_if = reg_oper->client_if;
        param.reg.status = reg_oper->status;
        param.reg.app_id = reg_oper->app_uuid.uu.uuid16;
        btc_gattc_cb_to_app(ESP_GATTC_REG_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_DEREG_EVT: {
        tBTA_GATTC_REG *reg_oper = &arg->reg_oper;

        gattc_if = reg_oper->client_if;
        btc_gattc_cb_to_app(ESP_GATTC_UNREG_EVT, gattc_if, NULL);
        break;
    }
    case BTA_GATTC_READ_CHAR_EVT: {
        set_read_value(&gattc_if, &param, &arg->read);
        btc_gattc_cb_to_app(ESP_GATTC_READ_CHAR_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_WRITE_CHAR_EVT:
    case BTA_GATTC_PREP_WRITE_EVT: {
        tBTA_GATTC_WRITE *write = &arg->write;
        uint32_t ret_evt = (msg->act == BTA_GATTC_WRITE_CHAR_EVT) ?
                           ESP_GATTC_WRITE_CHAR_EVT : ESP_GATTC_PREP_WRITE_EVT;

        gattc_if = BTC_GATT_GET_GATT_IF(write->conn_id);
        param.write.conn_id = BTC_GATT_GET_CONN_ID(write->conn_id);
        param.write.status = write->status;
        param.write.handle = write->handle;
        param.write.offset = write->offset;
        btc_gattc_cb_to_app(ret_evt, gattc_if, &param);
        break;
    }

    case BTA_GATTC_EXEC_EVT: {
        tBTA_GATTC_EXEC_CMPL *exec_cmpl = &arg->exec_cmpl;

        gattc_if = BTC_GATT_GET_GATT_IF(exec_cmpl->conn_id);
        param.exec_cmpl.conn_id = BTC_GATT_GET_CONN_ID(exec_cmpl->conn_id);
        param.exec_cmpl.status = exec_cmpl->status;
        btc_gattc_cb_to_app(ESP_GATTC_EXEC_EVT, gattc_if, &param);
        break;
    }

    case BTA_GATTC_SEARCH_CMPL_EVT: {
        tBTA_GATTC_SEARCH_CMPL *search_cmpl = &arg->search_cmpl;

        gattc_if = BTC_GATT_GET_GATT_IF(search_cmpl->conn_id);
        param.search_cmpl.conn_id = BTC_GATT_GET_CONN_ID(search_cmpl->conn_id);
        param.search_cmpl.status = search_cmpl->status;
        param.search_cmpl.searched_service_source = search_cmpl->searched_service_source;
        btc_gattc_cb_to_app(ESP_GATTC_SEARCH_CMPL_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_SEARCH_RES_EVT: {
        tBTA_GATTC_SRVC_RES *srvc_res = &arg->srvc_res;

        gattc_if = BTC_GATT_GET_GATT_IF(srvc_res->conn_id);
        param.search_res.conn_id = BTC_GATT_GET_CONN_ID(srvc_res->conn_id);
        param.search_res.start_handle = srvc_res->start_handle;
        param.search_res.end_handle = srvc_res->end_handle;
        param.search_res.is_primary = srvc_res->is_primary;
        bta_to_btc_gatt_id(&param.search_res.srvc_id, &srvc_res->service_uuid);
        btc_gattc_cb_to_app(ESP_GATTC_SEARCH_RES_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_READ_DESCR_EVT: {
        set_read_value(&gattc_if, &param, &arg->read);
        btc_gattc_cb_to_app(ESP_GATTC_READ_DESCR_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_READ_MULTIPLE_EVT: {
        set_read_value(&gattc_if, &param, &arg->read);
        btc_gattc_cb_to_app(ESP_GATTC_READ_MULTIPLE_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_WRITE_DESCR_EVT: {
        tBTA_GATTC_WRITE *write = &arg->write;

        gattc_if = BTC_GATT_GET_GATT_IF(write->conn_id);
        param.write.conn_id = BTC_GATT_GET_CONN_ID(write->conn_id);
        param.write.status = write->status;
        param.write.handle = write->handle;
        btc_gattc_cb_to_app(ESP_GATTC_WRITE_DESCR_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_NOTIF_EVT: {
        tBTA_GATTC_NOTIFY *notify = &arg->notify;

        gattc_if = BTC_GATT_GET_GATT_IF(notify->conn_id);
        param.notify.conn_id = BTC_GATT_GET_CONN_ID(notify->conn_id);
        memcpy(param.notify.remote_bda, notify->bda, sizeof(esp_bd_addr_t));
        param.notify.handle = notify->handle;
        param.notify.is_notify = (notify->is_notify == TRUE) ? true : false;
        param.notify.value_len = (notify->len > ESP_GATT_MAX_ATTR_LEN) ? \
                                 ESP_GATT_MAX_ATTR_LEN : notify->len;
        param.notify.value = notify->value;

        if (notify->is_notify == FALSE) {
            BTA_GATTC_SendIndConfirm(notify->conn_id, notify->handle);
        }

        btc_gattc_cb_to_app(ESP_GATTC_NOTIFY_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_OPEN_EVT: {
        tBTA_GATTC_OPEN *open = &arg->open;

        gattc_if = open->client_if;
        param.open.status = open->status;
        param.open.conn_id = BTC_GATT_GET_CONN_ID(open->conn_id);
        memcpy(param.open.remote_bda, open->remote_bda, sizeof(esp_bd_addr_t));
        param.open.mtu = open->mtu;
        btc_gattc_cb_to_app(ESP_GATTC_OPEN_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_CONNECT_EVT: {
        tBTA_GATTC_CONNECT *connect = &arg->connect;

        gattc_if = connect->client_if;
        param.connect.conn_id = BTC_GATT_GET_CONN_ID(connect->conn_id);
        memcpy(param.connect.remote_bda, connect->remote_bda, sizeof(esp_bd_addr_t));
        param.connect.conn_params.interval = connect->conn_params.interval;
        param.connect.conn_params.latency = connect->conn_params.latency;
        param.connect.conn_params.timeout = connect->conn_params.timeout;
        btc_gattc_cb_to_app(ESP_GATTC_CONNECT_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_CLOSE_EVT: {
        tBTA_GATTC_CLOSE *close = &arg->close;

        gattc_if = close->client_if;
        param.close.status = close->status;
        param.close.conn_id = BTC_GATT_GET_CONN_ID(close->conn_id);
        memcpy(param.close.remote_bda, close->remote_bda, sizeof(esp_bd_addr_t));
        param.close.reason = close->reason;
        btc_gattc_cb_to_app(ESP_GATTC_CLOSE_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_DISCONNECT_EVT: {
        tBTA_GATTC_DISCONNECT *disconnect = &arg->disconnect;

        gattc_if = disconnect->client_if;
        param.disconnect.reason = disconnect->reason;
        param.disconnect.conn_id = BTC_GATT_GET_CONN_ID(disconnect->conn_id);
        memcpy(param.disconnect.remote_bda, disconnect->remote_bda, sizeof(esp_bd_addr_t));
        btc_gattc_cb_to_app(ESP_GATTC_DISCONNECT_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_CFG_MTU_EVT: {
        tBTA_GATTC_CFG_MTU *cfg_mtu = &arg->cfg_mtu;

        gattc_if = BTC_GATT_GET_GATT_IF(cfg_mtu->conn_id);
        param.cfg_mtu.conn_id = BTC_GATT_GET_CONN_ID(cfg_mtu->conn_id);
        param.cfg_mtu.status = cfg_mtu->status;
        param.cfg_mtu.mtu = cfg_mtu->mtu;
        btc_gattc_cb_to_app(ESP_GATTC_CFG_MTU_EVT, gattc_if, &param);
        break;
    }

    case BTA_GATTC_ACL_EVT: {
        /* Currently, this event will never happen */
        break;
    }
    case BTA_GATTC_CANCEL_OPEN_EVT: {
        /* Currently, this event will never happen */
        break;
    }
    case BTA_GATTC_CONGEST_EVT: {
        tBTA_GATTC_CONGEST *congest = &arg->congest;

        gattc_if = BTC_GATT_GET_GATT_IF(congest->conn_id);
        param.congest.conn_id = BTC_GATT_GET_CONN_ID(congest->conn_id);
        param.congest.congested = (congest->congested == TRUE) ? true : false;
        btc_gattc_cb_to_app(ESP_GATTC_CONGEST_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_SRVC_CHG_EVT: {
        tBTA_GATTC_SERVICE_CHANGE *srvc_change = &arg->srvc_chg;
        gattc_if = BTC_GATT_GET_GATT_IF(srvc_change->conn_id);
        memcpy(param.srvc_chg.remote_bda, srvc_change->remote_bda, sizeof(esp_bd_addr_t));
        btc_gattc_cb_to_app(ESP_GATTC_SRVC_CHG_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_QUEUE_FULL_EVT: {
        tBTA_GATTC_QUEUE_FULL *queue_full = &arg->queue_full;
        gattc_if = BTC_GATT_GET_GATT_IF(queue_full->conn_id);
        param.queue_full.conn_id = BTC_GATT_GET_CONN_ID(queue_full->conn_id);
        param.queue_full.status = arg->status;
        param.queue_full.is_full = queue_full->is_full;
        btc_gattc_cb_to_app(ESP_GATTC_QUEUE_FULL_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_ASSOC_EVT: {
        gattc_if = arg->set_assoc.client_if;
        param.set_assoc_cmp.status = arg->set_assoc.status;
        btc_gattc_cb_to_app(ESP_GATTC_SET_ASSOC_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_GET_ADDR_LIST_EVT: {
        gattc_if = arg->get_addr_list.client_if;
        param.get_addr_list.status = arg->get_addr_list.status;
        param.get_addr_list.num_addr = arg->get_addr_list.num_addr;
        param.get_addr_list.addr_list = arg->get_addr_list.bda_list;
        btc_gattc_cb_to_app(ESP_GATTC_GET_ADDR_LIST_EVT, gattc_if, &param);
        break;
    }
    case BTA_GATTC_DIS_SRVC_CMPL_EVT:
        gattc_if = BTC_GATT_GET_GATT_IF(arg->dis_cmpl.conn_id);
        param.dis_srvc_cmpl.status = arg->dis_cmpl.status;
        param.dis_srvc_cmpl.conn_id = BTC_GATT_GET_CONN_ID(arg->dis_cmpl.conn_id);
        btc_gattc_cb_to_app(ESP_GATTC_DIS_SRVC_CMPL_EVT, gattc_if, &param);
        break;
    default:
        BTC_TRACE_DEBUG("%s: Unhandled event (%d)!", __FUNCTION__, msg->act);
        break;
    }

    // free the deep-copied data
    btc_gattc_free_req_data(msg);
}

void btc_gattc_congest_callback(tBTA_GATTC *param)
{
    esp_ble_gattc_cb_param_t esp_param = {0};
    memset(&esp_param, 0, sizeof(esp_ble_gattc_cb_param_t));

    uint8_t gattc_if = BTC_GATT_GET_GATT_IF(param->congest.conn_id);
    esp_param.congest.conn_id = BTC_GATT_GET_CONN_ID(param->congest.conn_id);
    esp_param.congest.congested = (param->congest.congested == TRUE) ? true : false;
    btc_gattc_cb_to_app(ESP_GATTC_CONGEST_EVT, gattc_if, &esp_param);

}

#endif  ///GATTC_INCLUDED == TRUE
