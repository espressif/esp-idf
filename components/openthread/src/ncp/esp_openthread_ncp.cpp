/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ieee802154.h"
#include "esp_openthread_ncp.h"
#include "ncp_base.hpp"

#if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK

#if CONFIG_OPENTHREAD_RCP_UART
#include "utils/uart.h"
#endif

#if CONFIG_OPENTHREAD_RCP_UART
extern "C" {
    static int NcpSend(const uint8_t *aBuf, uint16_t aBufLength)
    {
        IgnoreError(otPlatUartSend(aBuf, aBufLength));
        return aBufLength;
    }
}
#endif

extern "C" void otAppNcpInit(otInstance *aInstance)
{
#if CONFIG_OPENTHREAD_RCP_SPI
    otNcpSpiInit(aInstance);
#else
    IgnoreError(otPlatUartEnable());

    otNcpHdlcInit(aInstance, NcpSend);
#endif
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

    default:
        error = OT_ERROR_NOT_FOUND;
        break;
    }

    return error;
}

} // namespace Ncp
} // namespace ot

#endif // #if OPENTHREAD_ENABLE_NCP_VENDOR_HOOK
