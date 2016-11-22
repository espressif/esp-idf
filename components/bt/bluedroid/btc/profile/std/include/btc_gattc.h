#ifndef __BTC_GATTC_H__
#define __BTC_GATTC_H__

#include "btc_task.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"

typedef enum {
    BTC_GATTC_ACT_APP_REGISTER,
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
    BTC_GATTC_ACT_EXECUTE_WRITE,
    BTC_GATTC_ACT_REG_FOR_NOTIFY,
    BTC_GATTC_ACT_UNREG_FOR_NOTIFY
} btc_gattc_act_t;

/* btc_ble_gattc_args_t */
typedef struct {
    esp_gatt_if_t gatt_if;	 /* internal is server_if or client_if */    
    esp_gatt_srvc_id_t service_id;
    esp_gatt_srvc_id_t start_service_id;
    esp_gatt_id_t char_id;
    esp_gatt_id_t descr_id;
    esp_gatt_auth_req_t auth_req;
    esp_bd_addr_t remote_bda;
    esp_bt_uuid_t uuid;
	bool have_uuid;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t mtu;
    uint16_t offset;
    bool is_direct;
    bool is_execute;
    uint16_t value_len;
    uint8_t value[ESP_GATT_MAX_ATTR_LEN];
} btc_ble_gattc_args_t;

void btc_gattc_call_handler(btc_msg_t *msg);
void btc_gattc_cb_handler(btc_msg_t *msg);

#endif /* __BTC_GATTC_H__ */
