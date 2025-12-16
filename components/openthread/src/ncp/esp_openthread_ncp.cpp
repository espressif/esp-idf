/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_ieee802154.h"
#include "esp_openthread_ncp.h"
#include "esp_spinel_ncp_vendor_macro.h"
#include "ncp_base.hpp"

#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
#include "esp_coex_i154.h"
#endif

#if (CONFIG_OPENTHREAD_RCP_UART || CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG)
#include "utils/uart.h"
#endif

#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
#include "esp_console.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_openthread_common_macro.h"

static constexpr size_t s_console_command_max_length = 256;
static constexpr size_t s_console_command_queue_length = 8;
static QueueHandle_t s_console_command_queue = nullptr;

#define NO_LOG_TAG "" // don't use a tag to reduce log lengths from RCP

struct ConsoleCmdMsg
{
    char cmd[s_console_command_max_length];
};
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE

#if (CONFIG_OPENTHREAD_RCP_UART || CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG)
extern "C" {
    static int NcpSend(const uint8_t *aBuf, uint16_t aBufLength)
    {
        IgnoreError(otPlatUartSend(aBuf, aBufLength));
        return aBufLength;
    }
}
#endif

#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
static void console_command_worker_task(void *arg)
{
    ConsoleCmdMsg msg;

    for (;;) {
        if (xQueueReceive(s_console_command_queue, &msg, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        int ret = 0;
        esp_err_t err = esp_console_run(msg.cmd, &ret);

        if (err == ESP_ERR_NOT_FOUND){
            ESP_LOGI(NO_LOG_TAG, "Unrecognized command: %s", msg.cmd);
        } else if (err == ESP_ERR_INVALID_ARG) {
            ESP_LOGI(NO_LOG_TAG, "Command is empty");
        } else if (err == ESP_OK && ret != ESP_OK) {
            ESP_LOGI(NO_LOG_TAG, "Command returned non-zero error code: 0x%x (%s)", ret, esp_err_to_name(ret));
        } else if (err != ESP_OK) {
            ESP_LOGI(NO_LOG_TAG, "Internal error running '%s': %s", msg.cmd, esp_err_to_name(err));
        }
    }
}

static esp_err_t init_console_command_worker()
{
    if (s_console_command_queue) {
        return ESP_OK;
    }

    s_console_command_queue = xQueueCreate(s_console_command_queue_length, sizeof(ConsoleCmdMsg));
    ESP_RETURN_ON_FALSE(s_console_command_queue, ESP_ERR_NO_MEM, OT_PLAT_LOG_TAG, "Failed to create s_console_command_queue");

    BaseType_t ret = xTaskCreatePinnedToCore(console_command_worker_task, "ot_console", 3072, nullptr, 3, nullptr, tskNO_AFFINITY);

    if (ret != pdPASS) {
        vQueueDelete(s_console_command_queue);
        s_console_command_queue = nullptr;
        return ESP_FAIL;
    }

    return ESP_OK;
}
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE

extern "C" void otAppNcpInit(otInstance *aInstance)
{
#if (CONFIG_OPENTHREAD_RCP_UART || CONFIG_OPENTHREAD_RCP_USB_SERIAL_JTAG)
    IgnoreError(otPlatUartEnable());
    otNcpHdlcInit(aInstance, NcpSend);
#else
    otNcpSpiInit(aInstance);
#endif

#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
    esp_err_t err = esp_console_redirect_to_otlog();
    if (err != ESP_OK) {
        ESP_LOGE(NO_LOG_TAG, "Failed to redirect console to otPlatLog: %s", esp_err_to_name(err));
    }

    init_console_command_worker();
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
}

namespace ot {
namespace Ncp {

otError NcpBase::VendorCommandHandler(uint8_t aHeader, unsigned int aCommand)
{
    otError error = OT_ERROR_NONE;

    otPlatLog(OT_LOG_LEVEL_WARN, OT_LOG_REGION_NCP, "VendorCommandHandler Not Implemented");

    switch (aCommand)
    {

    default:
        error = PrepareLastStatusResponse(aHeader, SPINEL_STATUS_INVALID_COMMAND);
        break;
    }

    return error;
}

void NcpBase::VendorHandleFrameRemovedFromNcpBuffer(Spinel::Buffer::FrameTag aFrameTag)
{
    OT_UNUSED_VARIABLE(aFrameTag);
}

otError NcpBase::VendorGetPropertyHandler(spinel_prop_key_t aPropKey)
{
    otError error = OT_ERROR_NONE;

    switch (aPropKey)
    {
    case SPINEL_PROP_VENDOR_ESP_COEX_EVENT: {
#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
        esp_ieee802154_coex_config_t config = esp_ieee802154_get_coex_config();
        const uint8_t *args = reinterpret_cast<const uint8_t *>(&config);
        error = mEncoder.WriteDataWithLen(args, sizeof(esp_ieee802154_coex_config_t));
#else
        error = OT_ERROR_NOT_IMPLEMENTED;
#endif
        break;
    }

    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

otError NcpBase::VendorSetPropertyHandler(spinel_prop_key_t aPropKey)
{
    otError error = OT_ERROR_NONE;

    switch (aPropKey)
    {

    // TZ-566: Add mechanism to allow users to register callback functions.
    case SPINEL_PROP_VENDOR_ESP_SET_COORDINATOR: {
        bool coordinator = false;
        mDecoder.ReadBool(coordinator);
        esp_ieee802154_set_coordinator(coordinator);
        break;
    }
    case SPINEL_PROP_VENDOR_ESP_SET_PENDINGMODE: {

        int32_t pending_mode = 0;
        mDecoder.ReadInt32(pending_mode);
        esp_ieee802154_set_pending_mode(static_cast<esp_ieee802154_pending_mode_t>(pending_mode));
        break;
    }
    case SPINEL_PROP_VENDOR_ESP_COEX_EVENT: {
#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
        const uint8_t *args = nullptr;
        uint16_t len = 0;
        mDecoder.ReadDataWithLen(args, len);
        if (len == sizeof(esp_ieee802154_coex_config_t)) {
            esp_ieee802154_coex_config_t config;
            memcpy(&config, args, len);
            esp_ieee802154_set_coex_config(config);
        } else {
            error = OT_ERROR_INVALID_ARGS;
        }
#else
        error = OT_ERROR_NOT_IMPLEMENTED;
#endif
        break;
    }
#if CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE
    case SPINEL_PROP_VENDOR_ESP_SET_CONSOLE_CMD: {
        const uint8_t *data = nullptr;
        uint16_t len = 0;
        mDecoder.ReadDataWithLen(data, len);

        if (len >= s_console_command_max_length) {
            ESP_LOGW(NO_LOG_TAG, "Console command too long (%u bytes, max %zu), dropping", len, s_console_command_max_length - 1);
            break;
        }

        ConsoleCmdMsg msg{};
        memcpy(msg.cmd, data, len);
        msg.cmd[len] = '\0';

        // Use a separate task to run the command so that RCP does not time out
        if (s_console_command_queue == nullptr) {
            ESP_LOGW(NO_LOG_TAG, "Console worker not initialized, dropping cmd: %s", msg.cmd);
        } else if (xQueueSend(s_console_command_queue, &msg, 0) != pdTRUE) {
            ESP_LOGW(NO_LOG_TAG, "Console cmd queue full, dropping cmd: %s", msg.cmd);
        }
        break;
    }
#endif // CONFIG_OPENTHREAD_RCP_SPINEL_CONSOLE

    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

} // namespace Ncp
} // namespace ot
