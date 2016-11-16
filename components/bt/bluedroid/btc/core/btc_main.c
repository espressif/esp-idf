#include "btc_main.h"
#include "esp_err.h"

extern int bte_main_boot_entry(void *cb);
extern int bte_main_shutdown(void);

bt_status_t btc_enable_bluetooth(esp_bt_sec_cb_t *p_cback)
{
	return BTA_EnableBluetooth(p_cback) == BTA_SUCCESS ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_disable_bluetooth(void)
{
	return BTA_DisableBluetooth() == BTA_SUCCESS ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t btc_init_bluetooth(bluetooth_init_cb_t cb)
{
	return bte_main_boot_entry(cb) == 0 ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}


void btc_deinit_bluetooth(void)
{
	bte_main_shutdown();
}
