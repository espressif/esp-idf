/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_openthread_sleep.h"
#include "esp_openthread_state.h"
#include "esp_openthread_task_queue.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "lwip/dns.h"
#include "openthread/instance.h"
#include "openthread/netdata.h"
#include "openthread/tasklet.h"
#include "openthread/thread.h"

#if CONFIG_OPENTHREAD_FTD
#include "openthread/dataset_ftd.h"
#endif

static int hex_digit_to_int(char hex)
{
    if ('A' <= hex && hex <= 'F') {
        return 10 + hex - 'A';
    }
    if ('a' <= hex && hex <= 'f') {
        return 10 + hex - 'a';
    }
    if ('0' <= hex && hex <= '9') {
        return hex - '0';
    }
    return -1;
}

static size_t hex_string_to_binary(const char *hex_string, uint8_t *buf, size_t buf_size)
{
    int num_char = strlen(hex_string);

    if (num_char != buf_size * 2) {
        return 0;
    }
    for (size_t i = 0; i < num_char; i += 2) {
        int digit0 = hex_digit_to_int(hex_string[i]);
        int digit1 = hex_digit_to_int(hex_string[i + 1]);

        if (digit0 < 0 || digit1 < 0) {
            return 0;
        }
        buf[i / 2] = (digit0 << 4) + digit1;
    }

    return buf_size;
}

esp_err_t esp_openthread_init(const esp_openthread_platform_config_t *config)
{
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE
    ESP_RETURN_ON_ERROR(esp_openthread_sleep_init(), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread esp pm_lock");
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE */
    ESP_RETURN_ON_ERROR(esp_openthread_platform_init(config), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread platform driver");
    esp_openthread_lock_acquire(portMAX_DELAY);
    ESP_RETURN_ON_FALSE(otInstanceInitSingle() != NULL, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread instance");
#if CONFIG_OPENTHREAD_DNS64_CLIENT
    ESP_RETURN_ON_ERROR(esp_openthread_dns64_client_init(), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread dns64 client");
#endif
#if !CONFIG_OPENTHREAD_RADIO
    ESP_RETURN_ON_ERROR(esp_openthread_state_event_init(esp_openthread_get_instance()), OT_PLAT_LOG_TAG,
                        "Failed to initialize OpenThread state event");
#endif
    esp_openthread_lock_release();

    return ESP_OK;
}

esp_err_t esp_openthread_auto_start(otOperationalDatasetTlvs *datasetTlvs)
{
    otInstance *instance = esp_openthread_get_instance();

    if (datasetTlvs) {
        ESP_RETURN_ON_FALSE(otDatasetSetActiveTlvs(instance, datasetTlvs) == OT_ERROR_NONE, ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Failed to set OpenThread active dataset");
    } else {
        otOperationalDataset dataset;
        size_t len = 0;
#if CONFIG_OPENTHREAD_FTD
        otDatasetCreateNewNetwork(instance, &dataset);
#else
        memset(&dataset, 0, sizeof(otOperationalDataset));
#endif
        // Active timestamp
        dataset.mActiveTimestamp.mSeconds = 1;
        dataset.mActiveTimestamp.mTicks = 0;
        dataset.mActiveTimestamp.mAuthoritative = false;
        dataset.mComponents.mIsActiveTimestampPresent = true;

        // Channel, Pan ID, Network Name
        dataset.mChannel = CONFIG_OPENTHREAD_NETWORK_CHANNEL;
        dataset.mComponents.mIsChannelPresent = true;
        dataset.mPanId = CONFIG_OPENTHREAD_NETWORK_PANID;
        dataset.mComponents.mIsPanIdPresent = true;
        len = strlen(CONFIG_OPENTHREAD_NETWORK_NAME);
        assert(len <= OT_NETWORK_NAME_MAX_SIZE);
        memcpy(dataset.mNetworkName.m8, CONFIG_OPENTHREAD_NETWORK_NAME, len + 1);
        dataset.mComponents.mIsNetworkNamePresent = true;

        // Extended Pan ID
        len = hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_EXTPANID, dataset.mExtendedPanId.m8,
                                   sizeof(dataset.mExtendedPanId.m8));
        ESP_RETURN_ON_FALSE(len == sizeof(dataset.mExtendedPanId.m8), ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Cannot convert OpenThread extended pan id");
        dataset.mComponents.mIsExtendedPanIdPresent = true;

        // Mesh Local Prefix
        otIp6Prefix prefix;
        memset(&prefix, 0, sizeof(otIp6Prefix));
        if (otIp6PrefixFromString(CONFIG_OPENTHREAD_MESH_LOCAL_PREFIX, &prefix) == OT_ERROR_NONE) {
            memcpy(dataset.mMeshLocalPrefix.m8, prefix.mPrefix.mFields.m8, sizeof(dataset.mMeshLocalPrefix.m8));
            dataset.mComponents.mIsMeshLocalPrefixPresent = true;
        } else {
            ESP_LOGE("Failed to parse mesh local prefix", CONFIG_OPENTHREAD_MESH_LOCAL_PREFIX);
        }

        // Network Key
        len = hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_MASTERKEY, dataset.mNetworkKey.m8,
                                   sizeof(dataset.mNetworkKey.m8));
        ESP_RETURN_ON_FALSE(len == sizeof(dataset.mNetworkKey.m8), ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Cannot convert OpenThread master key");
        dataset.mComponents.mIsNetworkKeyPresent = true;

        // PSKc
        len = hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_PSKC, dataset.mPskc.m8, sizeof(dataset.mPskc.m8));
        ESP_RETURN_ON_FALSE(len == sizeof(dataset.mPskc.m8), ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Cannot convert OpenThread pre-shared commissioner key");
        dataset.mComponents.mIsPskcPresent = true;

        ESP_RETURN_ON_FALSE(otDatasetSetActive(instance, &dataset) == OT_ERROR_NONE, ESP_FAIL, OT_PLAT_LOG_TAG,
                            "Failed to set OpenThread active dataset");
    }

    ESP_RETURN_ON_FALSE(otIp6SetEnabled(instance, true) == OT_ERROR_NONE, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to enable OpenThread IPv6 interface");

    ESP_RETURN_ON_FALSE(otThreadSetEnabled(instance, true) == OT_ERROR_NONE, ESP_FAIL, OT_PLAT_LOG_TAG,
                        "Failed to enable OpenThread");

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
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE
        esp_openthread_sleep_process();
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE */
        esp_openthread_lock_release();

        if (select(mainloop.max_fd + 1, &mainloop.read_fds, &mainloop.write_fds, &mainloop.error_fds,
                   &mainloop.timeout) >= 0) {
            esp_openthread_lock_acquire(portMAX_DELAY);
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE
            esp_openthread_wakeup_process();
#endif /* CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_OPENTHREAD_RADIO_NATIVE */
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
