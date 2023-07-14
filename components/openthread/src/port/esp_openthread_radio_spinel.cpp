/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_radio.h"

#include "esp_check.h"
#include "esp_err.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_system.h"
#include "esp_spi_spinel_interface.hpp"
#include "esp_uart_spinel_interface.hpp"
#include "openthread-core-config.h"
#include "lib/spinel/radio_spinel.hpp"
#include "lib/spinel/spinel.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/radio.h"

using ot::Spinel::RadioSpinel;

#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
using esp::openthread::UartSpinelInterface;
static RadioSpinel<UartSpinelInterface> s_radio;
#else // CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
using esp::openthread::SpiSpinelInterface;
static RadioSpinel<SpiSpinelInterface> s_radio;
#endif // CONFIG_OPENTHREAD_RADIO_SPINEL_UART

static const char *radiospinel_workflow = "radio_spinel";

esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config)
{
#if CONFIG_OPENTHREAD_RADIO_SPINEL_UART
    ESP_RETURN_ON_ERROR(s_radio.GetSpinelInterface().Init(config->radio_config.radio_uart_config), OT_PLAT_LOG_TAG,
                        "Spinel interface init falied");
#else   // CONFIG_OPENTHREAD_RADIO_SPINEL_SPI
    ESP_RETURN_ON_ERROR(s_radio.GetSpinelInterface().Init(config->radio_config.radio_spi_config), OT_PLAT_LOG_TAG,
                        "Spinel interface init failed");
#endif  // CONFIG_OPENTHREAD_RADIO_SPINEL_UART
    s_radio.Init(/*reset_radio=*/true, /*skip_rcp_compatibility_check=*/false);
    return esp_openthread_platform_workflow_register(&esp_openthread_radio_update, &esp_openthread_radio_process,
                                                     radiospinel_workflow);
}

void esp_openthread_register_rcp_failure_handler(esp_openthread_rcp_failure_handler handler)
{
    s_radio.GetSpinelInterface().RegisterRcpFailureHandler(handler);
}

void esp_openthread_rcp_deinit(void)
{
    s_radio.GetSpinelInterface().Deinit();
}

void esp_openthread_radio_deinit(void)
{
    s_radio.Deinit();
    esp_openthread_platform_workflow_unregister(radiospinel_workflow);
}

esp_err_t esp_openthread_radio_process(otInstance *instance, const esp_openthread_mainloop_context_t *mainloop)
{
    s_radio.Process((void *)mainloop);

    return ESP_OK;
}

void esp_openthread_radio_update(esp_openthread_mainloop_context_t *mainloop)
{
    s_radio.GetSpinelInterface().Update((void *)mainloop);
}

void otPlatRadioGetIeeeEui64(otInstance *instance, uint8_t *ieee_eui64)
{
    SuccessOrDie(s_radio.GetIeeeEui64(ieee_eui64));
}

void otPlatRadioSetPanId(otInstance *instance, uint16_t pan_id)
{
    SuccessOrDie(s_radio.SetPanId(pan_id));
}

void otPlatRadioSetExtendedAddress(otInstance *instance, const otExtAddress *address)
{
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++) {
        addr.m8[i] = address->m8[sizeof(addr) - 1 - i];
    }

    SuccessOrDie(s_radio.SetExtendedAddress(addr));
}

void otPlatRadioSetShortAddress(otInstance *instance, uint16_t address)
{
    SuccessOrDie(s_radio.SetShortAddress(address));
}

void otPlatRadioSetPromiscuous(otInstance *instance, bool enable)
{
    SuccessOrDie(s_radio.SetPromiscuous(enable));
}

bool otPlatRadioIsEnabled(otInstance *instance)
{
    return s_radio.IsEnabled();
}

otError otPlatRadioEnable(otInstance *instance)
{
    return s_radio.Enable(instance);
}

otError otPlatRadioDisable(otInstance *instance)
{
    return s_radio.Disable();
}

otError otPlatRadioSleep(otInstance *instance)
{
    return s_radio.Sleep();
}

otError otPlatRadioReceive(otInstance *instance, uint8_t channel)
{
    return s_radio.Receive(channel);
}

otError otPlatRadioTransmit(otInstance *instance, otRadioFrame *frame)
{
    return s_radio.Transmit(*frame);
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *instance)
{
    return &s_radio.GetTransmitFrame();
}

int8_t otPlatRadioGetRssi(otInstance *instance)
{
    return s_radio.GetRssi();
}

otRadioCaps otPlatRadioGetCaps(otInstance *instance)
{
    return s_radio.GetRadioCaps();
}

bool otPlatRadioGetPromiscuous(otInstance *instance)
{
    return s_radio.IsPromiscuous();
}

void otPlatRadioEnableSrcMatch(otInstance *instance, bool enable)
{
    SuccessOrDie(s_radio.EnableSrcMatch(enable));
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *instance, uint16_t short_address)
{
    return s_radio.AddSrcMatchShortEntry(short_address);
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *instance, const otExtAddress *ext_address)
{
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++) {
        addr.m8[i] = ext_address->m8[sizeof(addr) - 1 - i];
    }

    return s_radio.AddSrcMatchExtEntry(addr);
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *instance, uint16_t short_address)
{
    return s_radio.ClearSrcMatchShortEntry(short_address);
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *instance, const otExtAddress *ext_address)
{
    otExtAddress addr;

    for (size_t i = 0; i < sizeof(addr); i++) {
        addr.m8[i] = ext_address->m8[sizeof(addr) - 1 - i];
    }

    return s_radio.ClearSrcMatchExtEntry(addr);
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *instance)
{
    SuccessOrDie(s_radio.ClearSrcMatchShortEntries());
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *instance)
{
    SuccessOrDie(s_radio.ClearSrcMatchExtEntries());
}

otError otPlatRadioEnergyScan(otInstance *instance, uint8_t channel, uint16_t duration)
{
    return s_radio.EnergyScan(channel, duration);
}

otError otPlatRadioGetTransmitPower(otInstance *instance, int8_t *power)
{
    otError error;

    VerifyOrExit(power != NULL, error = OT_ERROR_INVALID_ARGS);
    error = s_radio.GetTransmitPower(*power);

exit:
    return error;
}

otError otPlatRadioSetTransmitPower(otInstance *instance, int8_t power)
{
    return s_radio.SetTransmitPower(power);
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *instance, int8_t *threshold)
{
    otError error;

    VerifyOrExit(threshold != NULL, error = OT_ERROR_INVALID_ARGS);
    error = s_radio.GetCcaEnergyDetectThreshold(*threshold);

exit:
    return error;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *instance, int8_t threshold)
{
    return s_radio.SetCcaEnergyDetectThreshold(threshold);
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *instance)
{
    return s_radio.GetReceiveSensitivity();
}

void otPlatRadioSetMacKey(otInstance *aInstance, uint8_t aKeyIdMode, uint8_t aKeyId, const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey, const otMacKeyMaterial *aNextKey, otRadioKeyType aKeyType)
{
    SuccessOrDie(s_radio.SetMacKey(aKeyIdMode, aKeyId, aPrevKey, aCurrKey, aNextKey));
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    SuccessOrDie(s_radio.SetMacFrameCounter(aMacFrameCounter, true));
}

#if OPENTHREAD_CONFIG_DIAG_ENABLE
otError otPlatDiagProcess(otInstance *instance, int argc, char *argv[], char *output, size_t output_max_len)
{
    // deliver the platform specific diags commands to radio only ncp.
    char cmd[OPENTHREAD_CONFIG_DIAG_CMD_LINE_BUFFER_SIZE] = {'\0'};
    char *cur = cmd;
    char *end = cmd + sizeof(cmd);

    for (int index = 0; index < argc; index++) {
        cur += snprintf(cur, static_cast<size_t>(end - cur), "%s ", argv[index]);
    }

    return s_radio.PlatDiagProcess(cmd, output, output_max_len);
}

void otPlatDiagModeSet(bool aMode)
{
    SuccessOrExit(s_radio.PlatDiagProcess(aMode ? "start" : "stop", NULL, 0));
    s_radio.SetDiagEnabled(aMode);

exit:
    return;
}

bool otPlatDiagModeGet(void)
{
    return s_radio.IsDiagEnabled();
}

void otPlatDiagTxPowerSet(int8_t tx_power)
{
    char cmd[OPENTHREAD_CONFIG_DIAG_CMD_LINE_BUFFER_SIZE];

    snprintf(cmd, sizeof(cmd), "power %d", tx_power);
    SuccessOrExit(s_radio.PlatDiagProcess(cmd, NULL, 0));

exit:
    return;
}

void otPlatDiagChannelSet(uint8_t channel)
{
    char cmd[OPENTHREAD_CONFIG_DIAG_CMD_LINE_BUFFER_SIZE];

    snprintf(cmd, sizeof(cmd), "channel %d", channel);
    SuccessOrExit(s_radio.PlatDiagProcess(cmd, NULL, 0));

exit:
    return;
}

void otPlatDiagRadioReceived(otInstance *instance, otRadioFrame *frame, otError error)
{
}

void otPlatDiagAlarmCallback(otInstance *instance)
{
}

const char *otPlatRadioGetVersionString(otInstance *aInstance)
{
    return s_radio.GetVersion();
}

#endif // OPENTHREAD_CONFIG_DIAG_ENABLE

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return s_radio.GetNow();
}

#if OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return s_radio.GetCslAccuracy();

}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return s_radio.GetCslUncertainty();
}
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance *aInstance, otLinkMetrics aLinkMetrics, const otShortAddress aShortAddress, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);
    return s_radio.ConfigureEnhAckProbing(aLinkMetrics, aShortAddress, *aExtAddress);
}
#endif
