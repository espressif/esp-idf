#ifndef __BTC_BT_COMMON_H__
#define __BTC_BT_COMMON_H__

#include "bt_types.h"
#include "bta_api.h"

typedef tBTA_DM_SEC_CBACK esp_bt_sec_cb_t;

typedef void (*bluetooth_init_cb_t)(void);


bt_status_t btc_enable_bluetooth(esp_bt_sec_cb_t *p_cback);
bt_status_t btc_disable_bluetooth(void);
bt_status_t btc_init_bluetooth(bluetooth_init_cb_t cb);
void btc_deinit_bluetooth(void);

#endif /* __BTC_BT_COMMON_H__ */
