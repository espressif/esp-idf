#ifndef __ESP_BT_MAIN_H__
#define __ESP_BT_MAIN_H__

#include "btc_main.h"
#include "esp_err.h"

esp_err_t esp_enable_bluetooth(void);

esp_err_t esp_disable_bluetooth(void);

esp_err_t esp_init_bluetooth(void);

esp_err_t esp_deinit_bluetooth(void);


#endif /* __ESP_BT_MAIN_H__ */
