#ifndef __BTC_MANAGE_H__
#define __BTC_MANAGE_H__

#include "bta_api.h"
#include "btc_task.h"
#include "esp_bt_defs.h"

/* reset gatt callback table */
void esp_profile_cb_reset(void);

int btc_profile_cb_set(btc_pid_t profile_id, esp_profile_cb_t cb);
esp_profile_cb_t btc_profile_cb_get(btc_pid_t profile_id);

#endif /* __BTC_MANAGE_H__ */
