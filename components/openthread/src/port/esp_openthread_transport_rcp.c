/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_OPENTHREAD_RCP_CUSTOM

#include "esp_openthread_transport_priv.h"

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "esp_check.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_vfs_eventfd.h"
#include "common/code_utils.hpp"
#include "openthread/instance.h"
#include "utils/uart.h"

static esp_openthread_transport_config_t s_transport_config;
static int s_notify_eventfd = -1;
static const char s_transport_rcp_workflow[] = "transport_rcp";

static esp_err_t transport_rcp_notify_eventfd_init(void)
{
    ESP_RETURN_ON_FALSE(s_notify_eventfd < 0, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG, "custom transport RCP eventfd already initialized");
    s_notify_eventfd = eventfd(0, EFD_SUPPORT_ISR);
    ESP_RETURN_ON_FALSE(s_notify_eventfd >= 0, ESP_FAIL, OT_PLAT_LOG_TAG, "custom transport RCP eventfd create failed");
    return ESP_OK;
}

static void transport_rcp_notify_eventfd_deinit(void)
{
    if (s_notify_eventfd >= 0) {
        close(s_notify_eventfd);
        s_notify_eventfd = -1;
    }
}

void esp_openthread_transport_notify_rcp_rx(void)
{
    if (s_notify_eventfd >= 0) {
        uint64_t one = 1;
        ssize_t w = write(s_notify_eventfd, &one, sizeof(one));
        (void)w;
    }
}

void esp_openthread_transport_rcp_update(esp_openthread_mainloop_context_t *mainloop)
{
    if (s_notify_eventfd >= 0) {
        FD_SET(s_notify_eventfd, &mainloop->read_fds);
        if (s_notify_eventfd > mainloop->max_fd) {
            mainloop->max_fd = s_notify_eventfd;
        }
    }
}

static uint8_t s_rx_chunk[ESP_OPENTHREAD_TRANSPORT_BUFFER_SIZE];

esp_err_t esp_openthread_transport_rcp_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    OT_UNUSED_VARIABLE(instance);

    if (s_notify_eventfd >= 0 && FD_ISSET(s_notify_eventfd, &mainloop->read_fds)) {
        uint64_t v;
        ssize_t r = read(s_notify_eventfd, &v, sizeof(v));
        (void)r;
    }

    size_t n = 0;
    esp_err_t err = s_transport_config.transport_rx(s_rx_chunk, sizeof(s_rx_chunk), &n);
    if (err == ESP_OK) {
        if (n <= sizeof(s_rx_chunk)) {
            otPlatUartReceived(s_rx_chunk, (uint16_t)n);
        } else {
            ESP_LOGE(OT_PLAT_LOG_TAG, "invalid Rx data length");
        }
    }
    return ESP_OK;
}

esp_err_t esp_openthread_host_rcp_transport_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, OT_PLAT_LOG_TAG, "config is NULL");
    ESP_RETURN_ON_FALSE(config->host_config.host_connection_mode == HOST_CONNECTION_MODE_RCP_TRANSPORT, ESP_ERR_INVALID_ARG,
                        OT_PLAT_LOG_TAG, "host connection mode must be RCP Transport");
    ESP_RETURN_ON_FALSE(config->host_config.host_transport_config.transport_tx != NULL, ESP_ERR_INVALID_ARG,
                        OT_PLAT_LOG_TAG, "transport_tx is NULL");
    ESP_RETURN_ON_FALSE(config->host_config.host_transport_config.transport_rx != NULL, ESP_ERR_INVALID_ARG,
                        OT_PLAT_LOG_TAG, "transport_rx is NULL");

    memcpy(&s_transport_config, &config->host_config.host_transport_config, sizeof(s_transport_config));

    ESP_RETURN_ON_ERROR(transport_rcp_notify_eventfd_init(), OT_PLAT_LOG_TAG, "custom transport RCP notify init failed");

    return esp_openthread_platform_workflow_register(&esp_openthread_transport_rcp_update, &esp_openthread_transport_rcp_process,
                                                     s_transport_rcp_workflow);
}

void esp_openthread_host_rcp_transport_deinit(void)
{
    esp_openthread_platform_workflow_unregister(s_transport_rcp_workflow);
    memset(&s_transport_config, 0, sizeof(s_transport_config));
    transport_rcp_notify_eventfd_deinit();
}

otError otPlatUartEnable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *buf, uint16_t buf_length)
{
    if (!s_transport_config.transport_tx) {
        return OT_ERROR_FAILED;
    }
    esp_err_t err = s_transport_config.transport_tx(buf, buf_length);
    otPlatUartSendDone();
    if (err != ESP_OK) {
        return OT_ERROR_FAILED;
    }
    return OT_ERROR_NONE;
}

#endif /* CONFIG_OPENTHREAD_RCP_CUSTOM */
