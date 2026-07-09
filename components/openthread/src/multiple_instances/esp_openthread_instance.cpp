/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "core/instance/instance.hpp"
#include "esp_openthread.h"
#include "esp_openthread_instance.h"
#include "esp_openthread_ncp.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

bool esp_openthread_instances_init(void)
{
    return otInstanceInitSingle() != NULL;
}

void esp_openthread_instances_deinit(otInstance *instance)
{
    otInstanceFinalize(instance);
}

bool esp_openthread_tasklets_are_pending(otInstance *instance)
{
    return otTaskletsArePending(instance);
}

void esp_openthread_tasklets_process(otInstance *instance)
{
    while (otTaskletsArePending(instance)) {
        otTaskletsProcess(instance);
    }
}

#if CONFIG_OPENTHREAD_RADIO
void esp_openthread_ncp_app_init(otInstance *instance)
{
    otAppNcpInit(instance);
}
#endif

otInstance *esp_openthread_get_instance(void)
{
    return (otInstance *)&ot::Instance::Get();
}
