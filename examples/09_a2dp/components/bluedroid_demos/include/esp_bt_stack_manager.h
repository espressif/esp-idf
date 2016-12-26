#ifndef __ESP_BT_STACK_MANAGER_H__
#define __ESP_BT_STACK_MANAGER_H__

#include "esp_err.h"

esp_err_t esp_bt_init_stack(void);

esp_err_t esp_bt_deinit_stack(void);

esp_err_t esp_bt_enable_stack(void);

esp_err_t esp_bt_disable_stack(void);

#endif /* __ESP_BT_STACK_MANAGER_H__ */
