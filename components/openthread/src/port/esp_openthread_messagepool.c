/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "openthread-core-config.h"
#include "esp_openthread_common_macro.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "openthread/instance.h"
#include "openthread/platform/messagepool.h"

int s_buffer_pool_head = -1;
otMessageBuffer **s_buffer_pool_pointer = NULL;

void otPlatMessagePoolInit(otInstance *aInstance, uint16_t aMinNumFreeBuffers, size_t aBufferSize)
{
    otMessageBuffer *buffer_pool = (otMessageBuffer *)heap_caps_calloc(aMinNumFreeBuffers, aBufferSize, MALLOC_CAP_SPIRAM);
    s_buffer_pool_pointer = (otMessageBuffer **)heap_caps_calloc(aMinNumFreeBuffers, sizeof(otMessageBuffer **), MALLOC_CAP_SPIRAM);
    if (buffer_pool == NULL || s_buffer_pool_pointer == NULL) {
        ESP_LOGE(OT_PLAT_LOG_TAG, "Failed to create message buffer pool");
        assert(false);
    }
    for (uint16_t i = 0; i < aMinNumFreeBuffers; i++) {
        s_buffer_pool_pointer[i] = buffer_pool + i * aBufferSize / sizeof(otMessageBuffer);
    }
    s_buffer_pool_head = aMinNumFreeBuffers - 1;
    ESP_LOGI(OT_PLAT_LOG_TAG, "Create message buffer pool successfully, size %d", aMinNumFreeBuffers*aBufferSize);
}

otMessageBuffer *otPlatMessagePoolNew(otInstance *aInstance)
{
    otMessageBuffer *ret = NULL;
    if (s_buffer_pool_head >= 0) {
        ret = s_buffer_pool_pointer[s_buffer_pool_head];
        s_buffer_pool_head--;
    }
    return ret;
}

void otPlatMessagePoolFree(otInstance *aInstance, otMessageBuffer *aBuffer)
{
    s_buffer_pool_head++;
    s_buffer_pool_pointer[s_buffer_pool_head] = aBuffer;
}

uint16_t otPlatMessagePoolNumFreeBuffers(otInstance *aInstance)
{
    return s_buffer_pool_head + 1;
}
