#include "btc_task.h"
#include "btc_main.h"
#include "future.h"
#include "esp_err.h"

static future_t *main_future[BTC_MAIN_FUTURE_NUM];

extern int bte_main_boot_entry(void *cb);
extern int bte_main_shutdown(void);

future_t **btc_main_get_future_p(btc_main_future_type_t type)
{
	return &main_future[type];
}

static void btc_sec_callback(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data)
{
	switch (event) {
	case BTA_DM_ENABLE_EVT:
		future_ready(*btc_main_get_future_p(BTC_MAIN_ENABLE_FUTURE), FUTURE_SUCCESS);
		break;
	case BTA_DM_DISABLE_EVT:
		future_ready(*btc_main_get_future_p(BTC_MAIN_DISABLE_FUTURE), FUTURE_SUCCESS);
		break;
	}
}

static bt_status_t btc_enable_bluetooth(void)
{
	BTA_EnableBluetooth(btc_sec_callback);
}

static bt_status_t btc_disable_bluetooth(void)
{
	BTA_DisableBluetooth();
}

void btc_init_callback(void)
{
	future_ready(*btc_main_get_future_p(BTC_MAIN_INIT_FUTURE), FUTURE_SUCCESS);
}

static bt_status_t btc_init_bluetooth(void)
{
	bte_main_boot_entry(btc_init_callback);
}


static void btc_deinit_bluetooth(void)
{
	bte_main_shutdown();
	future_ready(*btc_main_get_future_p(BTC_MAIN_DEINIT_FUTURE), FUTURE_SUCCESS);
}

void btc_main_call_handler(btc_msg_t *msg)
{
	LOG_ERROR("%s act %d\n", __func__, msg->act);

	switch (msg->act) {
	case BTC_MAIN_ACT_INIT:
		btc_init_bluetooth();
		break;
	case BTC_MAIN_ACT_DEINIT:
		btc_deinit_bluetooth();
		break;
	case BTC_MAIN_ACT_ENABLE:
		btc_enable_bluetooth();
		break;
	case BTC_MAIN_ACT_DISABLE:
		btc_disable_bluetooth();
		break;
	default:
		LOG_ERROR("%s UNKNOWN ACT %d\n", __func__, msg->act);
		break;
	}
}

