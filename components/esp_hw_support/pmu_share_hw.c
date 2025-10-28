/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * PMU is currently only used by the software mailbox, but it could be used by multiple components
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_private/critical_section.h"
#include "esp_private/pmu_share_hw.h"


DEFINE_CRIT_SECTION_LOCK_STATIC(s_pmu_lock);

void pmu_lock_acquire(void)
{
    esp_os_enter_critical(&s_pmu_lock);
}


void pmu_lock_release(void)
{
    esp_os_exit_critical(&s_pmu_lock);
}
