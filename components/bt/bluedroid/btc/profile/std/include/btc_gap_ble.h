#ifndef __BTC_GAP_BLE_H__
#define __BTC_GAP_BLE_H__

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"

typedef enum {
	BTC_GAP_BLE_ACT_CFG_ADV_DATA = 0,
	BTC_GAP_BLE_ACT_SET_SCAN_PARAM,
	BTC_GAP_BLE_ACT_START_SCAN,
	BTC_GAP_BLE_ACT_STOP_SCAN,
	BTC_GAP_BLE_ACT_START_ADV,
	BTC_GAP_BLE_ACT_STOP_ADV,
	BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM,
	BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN,
	BTC_GAP_BLE_ACT_SET_RAND_ADDRESS,
	BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY,
	BTC_GAP_BLE_ACT_SET_DEV_NAME,
} btc_gap_ble_act_t;

/* btc_ble_gap_args_t */
typedef struct {
	esp_ble_adv_data_t adv_data;
	esp_ble_adv_params_t adv_params;
	esp_ble_scan_params_t  scan_params;
	esp_ble_conn_update_params_t conn_params;
	esp_bd_addr_t remote_device;
	esp_bd_addr_t rand_addr;
	uint32_t duration;
	uint16_t tx_data_length;
	bool privacy_enable;
#define ESP_GAP_DEVICE_NAME_MAX	(32)
	char device_name[ESP_GAP_DEVICE_NAME_MAX+1]; 
} btc_ble_gap_args_t;

void btc_gap_ble_call_handler(btc_msg_t *msg);
void btc_gap_ble_cb_handler(btc_msg_t *msg);

void btc_gap_ble_arg_deep_copy(btc_msg_t *msg, void *p_dest, void *p_src);

#endif /* __BTC_GAP_BLE_H__ */
