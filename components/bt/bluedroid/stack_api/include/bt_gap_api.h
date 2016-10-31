#ifndef __BT_GAP_API_H__
#define __BT_GAP_API_H__

#include <stdint.h>

#include "esp_err.h"
#include "bt_api_common.h"

/*
#define ESP_OK         0
#define ESP_FAIL       -1
#define ESP_ERR_NO_MEM              0x101
#define ESP_ERR_INVALID_ARG         0x102
#define ESP_ERR_INVALID_STATE       0x103
#define ESP_ERR_INVALID_SIZE        0x104
#define ESP_ERR_NOT_FOUND           0x105
*/

#define ESP_ERR_BT_GAP_BASE             (0xf000)       // TODO: move base address to esp_err.h
// #define ESP_ERR_BT_GAP_XXXX            (ESP_ERR_BT_GAP_BASE + 0xYY)

#define ESP_ERR_BT_SDP_BASE             (0xf100)       // TODO: move base address to esp_err.h

enum {
    BT_SCAN_MODE_NONE,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
}; // enumerations of bt_scan_mode_t
typedef uint16_t bt_scan_mode_t;

esp_err_t esp_bt_gap_set_scan_mode(bt_scan_mode_t mode);

#endif /* __BT_GAP_API_H__ */
