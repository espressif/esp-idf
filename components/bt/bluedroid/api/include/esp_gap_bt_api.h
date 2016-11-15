#ifndef __ESP_GAP_BT_API_H__
#define __ESP_GAP_BT_API_H__

#include <stdint.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

typedef enum {
    BT_SCAN_MODE_NONE = 0,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
} bt_scan_mode_t;

esp_err_t esp_bt_gap_set_scan_mode(bt_scan_mode_t mode);

#endif /* __ESP_GAP_BT_API_H__ */
