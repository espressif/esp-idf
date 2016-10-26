#ifndef __BT_GAP_H__
#define __BT_GAP_H__
#include "bt_defs.h"

typedef enum {
    BT_SCAN_MODE_NONE,
    BT_SCAN_MODE_CONNECTABLE,
    BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE
} bt_scan_mode_t;

bt_status_t API_BT_GapSetScanMode(bt_scan_mode_t mode);

#endif /* __BT_GAP_H__ */
