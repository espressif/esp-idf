#ifndef __BTC_GATTS_H__
#define __BTC_GATTS_H__

#include "btc_task.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum {
	BTC_GATTS_ACT_APP_REGISTER = 0,
	BTC_GATTS_ACT_APP_UNREGISTER,
	BTC_GATTS_ACT_CREATE_SERVICE,
	BTC_GATTS_ACT_DELETE_SERVICE,
	BTC_GATTS_ACT_START_SERVICE,
	BTC_GATTS_ACT_STOP_SERVICE,
	BTC_GATTS_ACT_ADD_INCLUDE_SERVICE,
	BTC_GATTS_ACT_ADD_CHAR,
	BTC_GATTS_ACT_ADD_CHAR_DESCR,
	BTC_GATTS_ACT_SEND_INDICATE,
	BTC_GATTS_ACT_SEND_RESPONSE,
	BTC_GATTS_ACT_OPEN,
	BTC_GATTS_ACT_CLOSE,
} btc_gatts_act_t;

/* btc_ble_gatts_args_t */
typedef struct {
	esp_gatt_if_t gatt_if;	 /* internal is server_if or client_if */
	esp_gatt_srvc_id_t service_id;
	esp_gatt_id_t char_id;
	esp_gatt_id_t descr_uuid;
	esp_bt_uuid_t uuid;
	esp_gatt_rsp_t rsp;
	esp_gatt_perm_t perm;
	esp_gatt_char_prop_t property;
	esp_bd_addr_t remote_bda;
	esp_gatt_status_t status;
	uint16_t service_handle;
	uint16_t included_service_handle;
	uint16_t attr_handle;
	uint16_t num_handle;
	uint16_t conn_id;
	uint16_t trans_id;
	bool need_confirm;
	bool is_direct;
	uint16_t app_uuid;
	uint16_t value_len;
	uint8_t value[ESP_GATT_MAX_ATTR_LEN];
} btc_ble_gatts_args_t;


void btc_gatts_call_handler(btc_msg_t *msg);
void btc_gatts_cb_handler(btc_msg_t *msg);

#endif /* __BTC_GATTS_H__ */
