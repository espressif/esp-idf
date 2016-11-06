#include "esp_bt_defs.h"
#include "esp_bt_common.h"
#include "esp_err.h"

extern int bte_main_boot_entry(void *cb);
extern int bte_main_shutdown(void);

// tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK *p_cback)
esp_err_t esp_enable_bluetooth(esp_bt_sec_cb_t *p_cback)
{
	return BTA_EnableBluetooth(p_cback) == BTA_SUCCESS ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_disable_bluetooth(void)
{
	return BTA_DisableBluetooth() == BTA_SUCCESS ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_init_bluetooth(bluetooth_init_cb_t cb)
{
	return bte_main_boot_entry(cb) == 0 ? ESP_OK : ESP_FAIL;
}


void esp_deinit_bluetooth(void)
{
	bte_main_shutdown();
}


