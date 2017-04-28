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

#ifndef __BTC_GATTC_H__
#define __BTC_GATTC_H__

#include "btc_task.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"

typedef enum {
    BTC_GATTC_ACT_APP_REGISTER = 0,
    BTC_GATTC_ACT_APP_UNREGISTER,
    BTC_GATTC_ACT_OPEN,
    BTC_GATTC_ACT_CLOSE,
    BTC_GATTC_ACT_CFG_MTU,
    BTC_GATTC_ACT_SEARCH_SERVICE,
    BTC_GATTC_ACT_GET_FIRST_CHAR,
    BTC_GATTC_ACT_GET_NEXT_CHAR,
    BTC_GATTC_ACT_GET_FIRST_DESCR,
    BTC_GATTC_ACT_GET_NEXT_DESCR,
    BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE,
    BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE,
    BTC_GATTC_ACT_READ_CHAR,
    BTC_GATTC_ACT_READ_CHAR_DESCR,
    BTC_GATTC_ACT_WRITE_CHAR,
    BTC_GATTC_ACT_WRITE_CHAR_DESCR,
    BTC_GATTC_ACT_PREPARE_WRITE,
    BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR,
    BTC_GATTC_ACT_EXECUTE_WRITE,
    BTC_GATTC_ACT_REG_FOR_NOTIFY,
    BTC_GATTC_ACT_UNREG_FOR_NOTIFY
} btc_gattc_act_t;

/* btc_ble_gattc_args_t */
typedef union {
    //BTC_GATTC_ACT_APP_REGISTER,
    struct app_reg_arg {
        uint16_t app_id;
    } app_reg;
    //BTC_GATTC_ACT_APP_UNREGISTER,
    struct app_unreg_arg {
        esp_gatt_if_t gattc_if;
    } app_unreg;
    //BTC_GATTC_ACT_OPEN,
    struct open_arg {
        esp_gatt_if_t gattc_if;
        esp_bd_addr_t remote_bda;
        bool is_direct;
    } open;
    //BTC_GATTC_ACT_CLOSE,
    struct close_arg {
        uint16_t conn_id;
    } close;
    //BTC_GATTC_ACT_CFG_MTU,
    struct cfg_mtu_arg {
        uint16_t conn_id;
        uint16_t mtu;
    } cfg_mtu;
    //BTC_GATTC_ACT_SEARCH_SERVICE,
    struct search_srvc_arg {
        uint16_t conn_id;
        bool filter_uuid_enable;
        esp_bt_uuid_t filter_uuid;
    } search_srvc;
    //BTC_GATTC_ACT_GET_FIRST_CHAR,
    struct get_first_char_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
    } get_first_char;
    //BTC_GATTC_ACT_GET_NEXT_CHAR,
    struct get_next_char_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
    } get_next_char;
    //BTC_GATTC_ACT_GET_FIRST_DESCR,
    struct get_first_descr_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
    } get_first_descr;
    //BTC_GATTC_ACT_GET_NEXT_DESCR,
    struct get_next_descr_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        esp_gatt_id_t descr_id;
    } get_next_descr;
    //BTC_GATTC_ACT_GET_FIRST_INCL_SERVICE,
    struct get_first_incl_srvc_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
    } get_first_incl_srvc;
    //BTC_GATTC_ACT_GET_NEXT_INCL_SERVICE,
    struct get_next_incl_srvc_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_srvc_id_t start_service_id;
    } get_next_incl_srvc;
    //BTC_GATTC_ACT_READ_CHAR,
    struct read_char_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        esp_gatt_auth_req_t auth_req;
    } read_char;
    //BTC_GATTC_ACT_READ_CHAR_DESCR,
    struct read_descr_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        esp_gatt_id_t descr_id;
        esp_gatt_auth_req_t auth_req;
    } read_descr;
    //BTC_GATTC_ACT_WRITE_CHAR,
    struct write_char_arg {
        uint16_t conn_id;
        uint16_t value_len;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        uint8_t *value;
		esp_gatt_write_type_t write_type;
        esp_gatt_auth_req_t auth_req;
    } write_char;
    //BTC_GATTC_ACT_WRITE_CHAR_DESCR,
    struct write_descr_arg {
        uint16_t conn_id;
        uint16_t value_len;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        esp_gatt_id_t descr_id;
        uint8_t *value;
		esp_gatt_write_type_t write_type;
        esp_gatt_auth_req_t auth_req;
    } write_descr;
    //BTC_GATTC_ACT_PREPARE_WRITE,
    struct prep_write_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        uint16_t offset;
        uint16_t value_len;
        uint8_t *value;
        esp_gatt_auth_req_t auth_req;
    } prep_write;
    //BTC_GATTC_ACT_PREPARE_WRITE_CHAR_DESCR,
    struct prep_write_descr_arg {
        uint16_t conn_id;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
        esp_gatt_id_t descr_id;
        uint16_t offset;
        uint16_t value_len;
        uint8_t *value;
        esp_gatt_auth_req_t auth_req;
    } prep_write_descr;
    //BTC_GATTC_ACT_EXECUTE_WRITE,
    struct exec_write_arg {
        uint16_t conn_id;
        bool is_execute;
    } exec_write;
    //BTC_GATTC_ACT_REG_FOR_NOTIFY,
    struct reg_for_notify_arg {
        esp_gatt_if_t gattc_if;
        esp_bd_addr_t remote_bda;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
    } reg_for_notify;
    //BTC_GATTC_ACT_UNREG_FOR_NOTIFY
    struct unreg_for_notify_arg {
        esp_gatt_if_t gattc_if;
        esp_bd_addr_t remote_bda;
        esp_gatt_srvc_id_t service_id;
        esp_gatt_id_t char_id;
    } unreg_for_notify;
} btc_ble_gattc_args_t;

void btc_gattc_call_handler(btc_msg_t *msg);
void btc_gattc_cb_handler(btc_msg_t *msg);
void btc_gattc_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

#endif /* __BTC_GATTC_H__ */
