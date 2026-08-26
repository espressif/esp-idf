/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <stdbool.h>
 #include "soc/soc_caps.h"
 #if SOC_ASSIST_DEBUG_SUPPORTED
 #include "hal/assist_debug_ll.h"
 #endif
 #include "esp_attr.h"

bool rv_utils_dbgr_is_attached(void)
{
#if SOC_ASSIST_DEBUG_SUPPORTED
    return assist_debug_ll_is_debugger_active();
#else
    return false;
#endif
}
