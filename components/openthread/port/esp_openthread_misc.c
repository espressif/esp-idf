// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_system.h"
#include "common/logging.hpp"
#include "openthread/platform/misc.h"

static otPlatMcuPowerState s_mcu_power_state = OT_PLAT_MCU_POWER_STATE_ON;

void otPlatReset(otInstance *aInstance)
{
    esp_restart();
}

otPlatResetReason otPlatGetResetReason(otInstance *instance)
{
    switch (esp_reset_reason()) {
    case ESP_RST_UNKNOWN:
        return OT_PLAT_RESET_REASON_UNKNOWN;
    case ESP_RST_POWERON:
        return OT_PLAT_RESET_REASON_POWER_ON;
    case ESP_RST_EXT:
        return OT_PLAT_RESET_REASON_EXTERNAL;
    case ESP_RST_SW:
        return OT_PLAT_RESET_REASON_SOFTWARE;
    case ESP_RST_PANIC:
        return OT_PLAT_RESET_REASON_FAULT;
    case ESP_RST_INT_WDT:
        return OT_PLAT_RESET_REASON_WATCHDOG;
    case ESP_RST_TASK_WDT:
        return OT_PLAT_RESET_REASON_WATCHDOG;
    case ESP_RST_WDT:
        return OT_PLAT_RESET_REASON_WATCHDOG;
    default:
        return OT_PLAT_RESET_REASON_OTHER;
    }
}

void otPlatWakeHost(void)
{
    // Not Implemented.
}

otError otPlatSetMcuPowerState(otInstance *instance, otPlatMcuPowerState state)
{
    otError error = OT_ERROR_NONE;

    OT_UNUSED_VARIABLE(instance);

    switch (state) {
    case OT_PLAT_MCU_POWER_STATE_ON:
    case OT_PLAT_MCU_POWER_STATE_LOW_POWER:
        s_mcu_power_state = state;
        break;

    default:
        error = OT_ERROR_FAILED;
        break;
    }

    return error;
}

otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *instance)
{
    OT_UNUSED_VARIABLE(instance);
    return s_mcu_power_state;
}

void otPlatAssertFail(const char *filename, int line)
{
    otLogCritPlat("Assert failed at %s:%d", filename, line);
    assert(false);
}
