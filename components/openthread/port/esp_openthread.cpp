/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread.h"
#include "esp_check.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_dns64.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_task_queue.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "lwip/dns.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"
#include "openthread/tasklet.h"

esp_err_t esp_openthread_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_ERROR(esp_openthread_platform_init(config), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread platform driver");
    esp_openthread_lock_acquire(portMAX_DELAY);
    ESP_RETURN_ON_FALSE(otInstanceInitSingle() != NULL, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread instance");
#if CONFIG_OPENTHREAD_DNS64_CLIENT
    ESP_RETURN_ON_ERROR(esp_openthread_dns64_client_init(), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread dns64 client");
#endif
    esp_openthread_lock_release();

    return ESP_OK;
}

esp_err_t esp_openthread_launch_mainloop(void)
{
    esp_openthread_mainloop_context_t mainloop;
    otInstance *instance = esp_openthread_get_instance();
    esp_err_t error = ESP_OK;

    while (true) {
        FD_ZERO(&mainloop.read_fds);
        FD_ZERO(&mainloop.write_fds);
        FD_ZERO(&mainloop.error_fds);

        mainloop.max_fd = -1;
        mainloop.timeout.tv_sec = 10;
        mainloop.timeout.tv_usec = 0;

        esp_openthread_lock_acquire(portMAX_DELAY);
        esp_openthread_platform_update(&mainloop);
        if (otTaskletsArePending(instance)) {
            mainloop.timeout.tv_sec = 0;
            mainloop.timeout.tv_usec = 0;
        }
        esp_openthread_lock_release();

        if (select(mainloop.max_fd + 1, &mainloop.read_fds, &mainloop.write_fds, &mainloop.error_fds,
                   &mainloop.timeout) >= 0) {
            esp_openthread_lock_acquire(portMAX_DELAY);
            error = esp_openthread_platform_process(instance, &mainloop);
            while (otTaskletsArePending(instance)) {
                otTaskletsProcess(instance);
            }
            esp_openthread_lock_release();
            if (error != ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "esp_openthread_platform_process failed");
                break;
            }
        } else {
            error = ESP_FAIL;
            ESP_LOGE(OT_PLAT_LOG_TAG, "OpenThread system polling failed");
            break;
        }
    }
    return error;
}

esp_err_t esp_openthread_deinit(void)
{
    otInstanceFinalize(esp_openthread_get_instance());
    return esp_openthread_platform_deinit();
}

static void stub_task(void *context)
{
    // this is a empty function used for ot-task signal pending
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    esp_openthread_task_queue_post(stub_task, NULL);
}
