/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>

#include "sdkconfig.h"
#include "esp_bt_cfg.h"

/* External functions or variables
 ************************************************************************
 */
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
void adv_stack_enableClearLegacyAdvVsCmd(bool en);
void scan_stack_enableAdvFlowCtrlVsCmd(bool en);
void chanSel_stack_enableSetCsaVsCmd(bool en);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

/* Local functions definition
 ***************************************************************************
 */
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
void ble_stack_enableVsCmds(bool en)
{
#if DEFAULT_BT_LE_ROLE_BROADCASTER
    adv_stack_enableClearLegacyAdvVsCmd(en);
#endif // DEFAULT_BT_LE_ROLE_BROADCASTER

#if DEFAULT_BT_LE_ROLE_OBSERVER
    scan_stack_enableAdvFlowCtrlVsCmd(en);
#endif // DEFAULT_BT_LE_ROLE_OBSERVER

    chanSel_stack_enableSetCsaVsCmd(en);
}

void ble_stack_enableVsEvents(bool en)
{
}
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

int ble_stack_enable(void)
{
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
    ble_stack_enableVsCmds(true);
    ble_stack_enableVsEvents(true);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)

    return 0;
}

void ble_stack_disable(void)
{
#if (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
    ble_stack_enableVsEvents(false);
    ble_stack_enableVsCmds(false);
#endif // (CONFIG_BT_NIMBLE_ENABLED || CONFIG_BT_BLUEDROID_ENABLED)
}
