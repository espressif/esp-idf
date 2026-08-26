/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <assert.h>
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_instance.h"
#include "esp_openthread_ncp.h"
#include "esp_openthread_platform.h"
#include "openthread/instance.h"
#include "openthread/tasklet.h"

static otInstance *s_active_instance = NULL;
static otInstance *s_instances[OT_INSTANCE_COUNT] = {NULL};

bool esp_openthread_instances_init(void)
{
    for (int i = 0; i < OT_INSTANCE_COUNT; i++) {
        s_instances[i] = otInstanceInitMultiple(i);
        assert(s_instances[i]);
    }
    esp_openthread_set_active_instance(s_instances[0]);
    return true;
}

void esp_openthread_instances_deinit(otInstance *instance)
{
    (void)instance;
    for (int i = 0; i < OT_INSTANCE_COUNT; i++) {
        otInstanceFinalize(s_instances[i]);
    }
}

bool esp_openthread_tasklets_are_pending(otInstance *instance)
{
    (void)instance;

    for (int idx = 0; idx < OT_INSTANCE_COUNT; idx++) {
        if (otTaskletsArePending(esp_openthread_get_instance_from_idx(idx))) {
            return true;
        }
    }
    return false;
}

void esp_openthread_tasklets_process(otInstance *instance)
{
    (void)instance;

    for (int idx = 0; idx < OT_INSTANCE_COUNT; idx++) {
        otInstance *inst = esp_openthread_get_instance_from_idx(idx);
        while (otTaskletsArePending(inst)) {
            otTaskletsProcess(inst);
        }
    }
}

#if CONFIG_OPENTHREAD_RADIO
void esp_openthread_ncp_app_init(otInstance *instance)
{
    (void)instance;

    otInstance *instances[OT_INSTANCE_COUNT] = {nullptr};
    for (int idx = 0; idx < OT_INSTANCE_COUNT; idx++) {
        instances[idx] = esp_openthread_get_instance_from_idx(idx);
    }
    otAppNcpInitMulti(instances, OT_INSTANCE_COUNT);
}
#endif

otInstance *esp_openthread_get_instance(void)
{
    return s_active_instance;
}

int8_t esp_openthread_get_idx_from_instance(otInstance *instance)
{
    if (instance == nullptr) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Instance is NULL");
        assert(instance != nullptr);
    }

    uint8_t idx = otInstanceGetIndex(instance);
    if (idx >= OT_INSTANCE_COUNT) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Index %u is out of range", idx);
        assert(idx < OT_INSTANCE_COUNT);
    }

    return static_cast<int8_t>(idx);
}

otInstance *esp_openthread_get_instance_from_idx(int8_t idx)
{
    if (idx < 0 || idx >= OT_INSTANCE_COUNT) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Index %d is out of range", idx);
        assert(idx >= 0 && idx < OT_INSTANCE_COUNT);
    }

    otInstance *instance = otInstanceGetInstance(static_cast<uint8_t>(idx));
    if (instance == nullptr) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "No instance for index %d", idx);
        assert(instance != nullptr);
    }

    return instance;
}

void esp_openthread_set_active_instance(otInstance *instance)
{
    s_active_instance = instance;
}
