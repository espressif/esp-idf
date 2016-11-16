#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include "btc_main.h"
#include "esp_err.h"

esp_err_t esp_enable_bluetooth(esp_bt_sec_cb_t *p_cback);

esp_err_t esp_disable_bluetooth(void);

esp_err_t esp_init_bluetooth(bluetooth_init_cb_t cb);

void esp_deinit_bluetooth(void);


#endif /* __ESP_BT_MAIN_H__ */
