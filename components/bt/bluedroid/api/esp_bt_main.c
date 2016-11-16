#include "esp_bt_main.h"



esp_err_t esp_enable_bluetooth(esp_bt_sec_cb_t *p_cback)
{
	return btc_enable_bluetooth(p_cback) == BT_STATUS_SUCCESS ? ESP_OK: ESP_FAIL;
}

esp_err_t esp_disable_bluetooth(void)
{
	return btc_disable_bluetooth() == BT_STATUS_SUCCESS ? ESP_OK: ESP_FAIL;
}

esp_err_t esp_init_bluetooth(bluetooth_init_cb_t cb)
{
	return btc_init_bluetooth(cb) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL;
}


void esp_deinit_bluetooth(void)
{
	btc_deinit_bluetooth();
}


