#include "bt_gap.h"
#include "bta_api.h"
#include "bt_trace.h"

bt_status_t API_BT_GapSetScanMode(bt_scan_mode_t mode)
{
    tBTA_DM_DISC disc_mode;
    tBTA_DM_CONN conn_mode;

    switch(mode) {
    case BT_SCAN_MODE_NONE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_NON_CONN;
        break;

    case BT_SCAN_MODE_CONNECTABLE:
        disc_mode = BTA_DM_NON_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    case BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE:
        disc_mode = BTA_DM_GENERAL_DISC;
        conn_mode = BTA_DM_CONN;
        break;

    default:
        BTIF_TRACE_ERROR("invalid scan mode (0x%x)", mode);
        return BT_STATUS_PARM_INVALID;
    }

    BTIF_TRACE_EVENT("set property scan mode : %x", mode);

    BTA_DmSetVisibility(disc_mode, conn_mode, BTA_DM_IGNORE, BTA_DM_IGNORE);
    
    return BT_STATUS_SUCCESS;
}
