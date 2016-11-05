#ifndef __ESP_GAP_API_H__
#define __ESP_GAP_API_H__

#include <stdint.h>

#include "esp_err.h"
#include "esp_bt_common.h"

enum {
    BT_SCAN_MODE_NONE,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
};
typedef uint16_t bt_scan_mode_t;

esp_err_t esp_bt_gap_set_scan_mode(bt_scan_mode_t mode);

#endif /* __ESP_GAP_API_H__ */
