/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_ncp.h"

#include "esp_app_format.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread.h"
#include "esp_openthread_common_macro.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "common/instance.hpp"
#include "common/new.hpp"
#include "common/non_copyable.hpp"
#include "lib/spinel/spinel.h"
#include "ncp/ncp_base.hpp"
#include "ncp/ncp_hdlc.hpp"
#include "openthread/error.h"
#include "openthread/platform/toolchain.h"
#include "utils/uart.h"

static bool s_ota_triggered = false;
static bool s_header_checked = false;
static esp_ota_handle_t s_update_handle;
static const esp_partition_t *s_update_partition = NULL;

static otError start_ota(const uint8_t *header, uint16_t header_size)
{
    esp_app_desc_t new_app_info;
    esp_app_desc_t running_app_info;
    esp_app_desc_t invalid_app_info;
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    const esp_partition_t *invalid_partition = esp_ota_get_last_invalid_partition();

    s_update_partition = esp_ota_get_next_update_partition(NULL);
    ESP_RETURN_ON_FALSE(s_update_partition != NULL, OT_ERROR_FAILED, OT_PLAT_LOG_TAG, "Failed to get update partition");
    ESP_RETURN_ON_FALSE(header_size >=
                            sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t),
                        OT_ERROR_INVALID_ARGS, OT_PLAT_LOG_TAG, "Image header size too small");
    memcpy(&new_app_info, &header[sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t)],
           sizeof(esp_app_desc_t));

    ESP_RETURN_ON_FALSE(esp_ota_get_partition_description(running_partition, &running_app_info) == ESP_OK,
                        OT_ERROR_FAILED, OT_PLAT_LOG_TAG, "Failed to get running app description");
    ESP_LOGD(OT_PLAT_LOG_TAG, "Running firmware version: %s", running_app_info.version);
    ESP_LOGD(OT_PLAT_LOG_TAG, "New firmware version: %s", new_app_info.version);

    if (esp_ota_get_partition_description(invalid_partition, &invalid_app_info) == ESP_OK) {
        ESP_RETURN_ON_FALSE(strncmp(invalid_app_info.version, new_app_info.version, sizeof(invalid_app_info.version)),
                            OT_ERROR_FAILED, OT_PLAT_LOG_TAG, "The updated firmware has been rolled back, reject...");
    }
    ESP_RETURN_ON_FALSE(esp_ota_begin(s_update_partition, OTA_WITH_SEQUENTIAL_WRITES, &s_update_handle) == ESP_OK,
                        OT_ERROR_FAILED, OT_PLAT_LOG_TAG, "Failed to start OTA");
    s_header_checked = true;
    return OT_ERROR_NONE;
}

namespace ot {
namespace Ncp {

otError NcpBase::VendorCommandHandler(uint8_t header, unsigned int command)
{
    return OT_ERROR_NOT_FOUND;
}

otError NcpBase::VendorGetPropertyHandler(spinel_prop_key_t key)
{
    return OT_ERROR_NOT_FOUND;
}

void NcpBase::VendorHandleFrameRemovedFromNcpBuffer(Spinel::Buffer::FrameTag tag)
{
}

otError NcpBase::VendorSetPropertyHandler(spinel_prop_key_t key)
{
    otError error = OT_ERROR_NONE;
    const uint8_t *data = NULL;
    uint16_t data_size = 0;

    switch (key) {
    case SPINEL_PROP_VENDOR_ESP_OTA_TRIGGER:
        ESP_LOGD(OT_PLAT_LOG_TAG, "Trigger OTA");
        if (error != OT_ERROR_NONE) {
            break;
        }
        s_ota_triggered = true;
        break;
    case SPINEL_PROP_VENDOR_ESP_OTA_DATA:
        if (s_ota_triggered) {
            error = mDecoder.ReadDataWithLen(data, data_size);
            if (!s_header_checked) {
                ESP_LOGD(OT_PLAT_LOG_TAG, "Start OTA");
                error = start_ota(data, data_size);
            }
            if (esp_ota_write(s_update_handle, data, data_size) != ESP_OK) {
                ESP_LOGE(OT_PLAT_LOG_TAG, "esp_ota_write failed");
                error = OT_ERROR_FAILED;
            }
            if (error != OT_ERROR_NONE) {
                esp_ota_abort(s_update_handle);
                s_ota_triggered = false;
            }
        } else {
            error = OT_ERROR_INVALID_STATE;
        }
        break;
    case SPINEL_PROP_VENDOR_ESP_OTA_FINISH:
        ESP_LOGD(OT_PLAT_LOG_TAG, "Finish OTA");
        if (!s_ota_triggered || !s_header_checked) {
            error = OT_ERROR_INVALID_STATE;
            break;
        }
        if (esp_ota_set_boot_partition(s_update_partition) != ESP_OK) {
            ESP_LOGE(OT_PLAT_LOG_TAG, "esp_ota_set_boot_partition failed");
            error = OT_ERROR_FAILED;
            break;
        }
        esp_restart();
        break;
    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

} // namespace Ncp
} // namespace ot

static OT_DEFINE_ALIGNED_VAR(s_ncp, sizeof(ot::Ncp::NcpHdlc), uint64_t);

static int NcpSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    IgnoreError(otPlatUartSend(aBuf, aBufLength));
    return aBufLength;
}

extern "C" void otAppNcpInit(otInstance *instance)
{
    ot::Ncp::NcpHdlc *ncp = nullptr;
    ot::Instance *ot_instance = static_cast<ot::Instance *>(instance);

    IgnoreError(otPlatUartEnable());
    ncp = new (&s_ncp) ot::Ncp::NcpHdlc(ot_instance, NcpSend);

    if (ncp == nullptr || ncp != ot::Ncp::NcpBase::GetNcpInstance()) {
        OT_ASSERT(false);
    }
}
