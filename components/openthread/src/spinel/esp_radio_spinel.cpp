/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_log.h"
#include "platform/exit_code.h"
#include "radio_spinel.hpp"
#include "esp_radio_spinel.h"
#include "esp_radio_spinel_platform.h"
#include "esp_radio_spinel_adapter.hpp"
#include "esp_radio_spinel_uart_interface.hpp"
#include "spinel_driver.hpp"
#include "openthread/link.h"

#define SPINEL_VENDOR_PROPERTY_BIT_PENDINGMODE BIT(0)
#define SPINEL_VENDOR_PROPERTY_BIT_COORDINATOR BIT(1)
static esp_ieee802154_pending_mode_t s_spinel_vendor_property_pendingmode[ot::Spinel::kSpinelHeaderMaxNumIid] = {ESP_IEEE802154_AUTO_PENDING_DISABLE};
static bool s_spinel_vendor_property_coordinator[ot::Spinel::kSpinelHeaderMaxNumIid] = {false};
static uint64_t s_spinel_vendor_property_mask[ot::Spinel::kSpinelHeaderMaxNumIid] = {0};

using ot::Spinel::RadioSpinel;
using ot::Spinel::RadioSpinelCallbacks;
using esp::radio_spinel::SpinelInterfaceAdapter;
using esp::radio_spinel::UartSpinelInterface;
using ot::Spinel::SpinelDriver;

static SpinelInterfaceAdapter<UartSpinelInterface> s_spinel_interface[ot::Spinel::kSpinelHeaderMaxNumIid];
static RadioSpinel s_radio[ot::Spinel::kSpinelHeaderMaxNumIid];
static esp_radio_spinel_callbacks_t s_esp_radio_spinel_callbacks[ot::Spinel::kSpinelHeaderMaxNumIid];
static SpinelDriver s_spinel_driver[ot::Spinel::kSpinelHeaderMaxNumIid];
otRadioFrame s_transmit_frame;

static otRadioCaps s_radio_caps = (OT_RADIO_CAPS_ENERGY_SCAN       |
                                   OT_RADIO_CAPS_TRANSMIT_SEC      |
                                   OT_RADIO_CAPS_RECEIVE_TIMING    |
                                   OT_RADIO_CAPS_TRANSMIT_TIMING   |
                                   OT_RADIO_CAPS_ACK_TIMEOUT       |
                                   OT_RADIO_CAPS_SLEEP_TO_TX);

static esp_radio_spinel_compatibility_error_callback s_radio_spinel_compatibility_error_callback = NULL;

static esp_radio_spinel_idx_t get_index_from_instance(otInstance *instance)
{
    // TZ-563: Implement the function to get the esp radio spinel idx from otInstance for multipan rcp
    return ESP_RADIO_SPINEL_ZIGBEE;
}

static otInstance* get_instance_from_index(esp_radio_spinel_idx_t idx)
{
    // TZ-563: Implement the function to get otInstance pointer from esp radio spinel idx
    return nullptr;
}

static void esp_radio_spinel_restore_vendor_properities(void *context)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance((otInstance*)context);
    if (s_spinel_vendor_property_mask[idx] & SPINEL_VENDOR_PROPERTY_BIT_PENDINGMODE) {
        if (esp_radio_spinel_set_pending_mode(s_spinel_vendor_property_pendingmode[idx], idx) != ESP_OK) {
            ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to restore pendingmode: %d", idx);
        }
    }

    if (s_spinel_vendor_property_mask[idx] & SPINEL_VENDOR_PROPERTY_BIT_COORDINATOR) {
        if (esp_radio_spinel_set_pan_coord(s_spinel_vendor_property_coordinator[idx], idx) != ESP_OK) {
            ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to restore coordinator: %d", idx);
        }
    }
}

static void radio_spinel_compatibility_error_callback(void *context)
{
    OT_UNUSED_VARIABLE(context);
    if (s_radio_spinel_compatibility_error_callback) {
        s_radio_spinel_compatibility_error_callback();
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "None callback to handle compatibility error of openthread spinel");
        assert(false);
    }
}

void esp_radio_spinel_set_compatibility_error_callback(esp_radio_spinel_compatibility_error_callback callback)
{
    s_radio_spinel_compatibility_error_callback = callback;
}

void ReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].receive_done);
    uint8_t *frame = (uint8_t *)malloc(aFrame->mLength + 1);
    esp_ieee802154_frame_info_t frame_info;
    if (frame) {
        frame[0] = aFrame->mLength;
        memcpy((void *)(frame + 1), aFrame->mPsdu, frame[0]);
        frame_info.rssi = aFrame->mInfo.mRxInfo.mRssi;
        frame_info.timestamp = aFrame->mInfo.mRxInfo.mTimestamp;
        frame_info.pending = aFrame->mInfo.mRxInfo.mAckedWithFramePending;
        s_esp_radio_spinel_callbacks[idx].receive_done(frame, &frame_info);
        free(frame);
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for frame");
    }
}

void TransmitDone(otInstance *aInstance, otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].transmit_done && s_esp_radio_spinel_callbacks[idx].transmit_failed);
    if (aError == OT_ERROR_NONE) {
        uint8_t *frame = (uint8_t *)malloc(aFrame->mLength + 1);
        uint8_t *ack = nullptr;
        if (frame) {
            esp_ieee802154_frame_info_t ack_info;
            frame[0] = aFrame->mLength;
            memcpy((void *)(frame + 1), aFrame->mPsdu, frame[0]);
            if (aAckFrame) {
                ack = (uint8_t *)malloc(aAckFrame->mLength + 1);
                if (ack) {
                    ack[0] = aAckFrame->mLength;
                    memcpy((void *)(ack + 1), aAckFrame->mPsdu, ack[0]);
                } else {
                    ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for ack");
                }
            }
            s_esp_radio_spinel_callbacks[idx].transmit_done(frame, ack, &ack_info);
            free(frame);
            free(ack);
        } else {
            ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for frame");
        }
    } else {
        switch (aError) {
        case OT_ERROR_CHANNEL_ACCESS_FAILURE:
            s_esp_radio_spinel_callbacks[idx].transmit_failed(ESP_IEEE802154_TX_ERR_CCA_BUSY);
            break;
        case OT_ERROR_NO_ACK:
            s_esp_radio_spinel_callbacks[idx].transmit_failed(ESP_IEEE802154_TX_ERR_NO_ACK);
            break;
        default:
            s_esp_radio_spinel_callbacks[idx].transmit_failed(ESP_IEEE802154_TX_ERR_ABORT);
            break;
        }
    }
}

void EnergyScanDone(otInstance *aInstance, int8_t aMaxRssi)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].energy_scan_done);
    s_esp_radio_spinel_callbacks[idx].energy_scan_done(aMaxRssi);
}

void TxStarted(otInstance *aInstance, otRadioFrame *aFrame)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].transmit_started);
    uint8_t *frame = (uint8_t *)malloc(aFrame->mLength + 1);
    if (frame) {
        frame[0] = aFrame->mLength;
        memcpy((void *)(frame + 1), aFrame->mPsdu, frame[0]);
        s_esp_radio_spinel_callbacks[idx].transmit_started(frame);
        free(frame);
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for frame");
    }
}

void SwitchoverDone(otInstance *aInstance, bool aSuccess)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].switchover_done);
    s_esp_radio_spinel_callbacks[idx].switchover_done(aSuccess);
}

#if CONFIG_OPENTHREAD_DIAG
void DiagReceiveDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].diag_receive_done);
    uint8_t *frame = (uint8_t *)malloc(aFrame->mLength + 1);
    esp_ieee802154_frame_info_t frame_info;
    if (frame) {
        frame[0] = aFrame->mLength;
        memcpy((void *)(frame + 1), aFrame->mPsdu, frame[0]);
        frame_info.rssi = aFrame->mInfo.mRxInfo.mRssi;
        frame_info.timestamp = aFrame->mInfo.mRxInfo.mTimestamp;
        frame_info.pending = aFrame->mInfo.mRxInfo.mAckedWithFramePending;
        s_esp_radio_spinel_callbacks[idx].diag_receive_done(frame, &frame_info);
        free(frame);
    } else {
        ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for frame");
    }
}

void DiagTransmitDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    assert(s_esp_radio_spinel_callbacks[idx].diag_transmit_done && s_esp_radio_spinel_callbacks[idx].diag_transmit_failed);
    if (aError == OT_ERROR_NONE) {
        uint8_t *frame = (uint8_t *)malloc(aFrame->mLength + 1);
        if (frame) {
            esp_ieee802154_frame_info_t ack_info;
            frame[0] = aFrame->mLength;
            memcpy((void *)(frame + 1), aFrame->mPsdu, frame[0]);
            s_esp_radio_spinel_callbacks[idx].diag_transmit_done(frame, &ack_info);
            free(frame);
        } else {
            ESP_LOGE(ESP_SPINEL_LOG_TAG, "Fail to alloc memory for frame");
        }
    } else {
        switch (aError) {
        case OT_ERROR_CHANNEL_ACCESS_FAILURE:
            s_esp_radio_spinel_callbacks[idx].diag_transmit_failed(ESP_IEEE802154_TX_ERR_CCA_BUSY);
            break;
        case OT_ERROR_NO_ACK:
            s_esp_radio_spinel_callbacks[idx].diag_transmit_failed(ESP_IEEE802154_TX_ERR_NO_ACK);
            break;
        default:
            s_esp_radio_spinel_callbacks[idx].diag_transmit_failed(ESP_IEEE802154_TX_ERR_CCA_BUSY);
            break;
        }
    }
}
#endif // CONFIG_OPENTHREAD_DIAG


void esp_radio_spinel_set_callbacks(const esp_radio_spinel_callbacks_t aCallbacks, esp_radio_spinel_idx_t idx)
{
    s_esp_radio_spinel_callbacks[idx] = aCallbacks;
    RadioSpinelCallbacks Callbacks;
    Callbacks.mReceiveDone = ReceiveDone;
    Callbacks.mTransmitDone = TransmitDone;
    Callbacks.mEnergyScanDone = EnergyScanDone;
    Callbacks.mTxStarted = TxStarted;
    Callbacks.mSwitchoverDone = SwitchoverDone;
#if CONFIG_OPENTHREAD_DIAG
    Callbacks.mDiagReceiveDone = DiagReceiveDone;
    Callbacks.mDiagTransmitDone = DiagTransmitDone;
#endif // CONFIG_OPENTHREAD_DIAG

    s_radio[idx].SetCallbacks(Callbacks);
}

esp_err_t esp_radio_spinel_uart_interface_enable(const esp_radio_spinel_uart_config_t *radio_uart_config,
                                                 esp_radio_spinel_uart_init_handler aUartInitHandler,
                                                 esp_radio_spinel_uart_deinit_handler aUartDeinitHandler,
                                                 esp_radio_spinel_idx_t idx)
{
    ESP_RETURN_ON_FALSE(aUartInitHandler != nullptr, ESP_FAIL, ESP_SPINEL_LOG_TAG, "UartInitHandler can not be set to NULL");
    ESP_RETURN_ON_FALSE(aUartDeinitHandler != nullptr, ESP_FAIL, ESP_SPINEL_LOG_TAG, "UartDeinitHandler can not be set to NULL");
    s_spinel_interface[idx].GetSpinelInterface().RegisterUartInitHandler(aUartInitHandler);
    s_spinel_interface[idx].GetSpinelInterface().RegisterUartDeinitHandler(aUartDeinitHandler);
    ESP_RETURN_ON_FALSE(s_spinel_interface[idx].GetSpinelInterface().Enable(*radio_uart_config) == OT_ERROR_NONE, ESP_FAIL, ESP_SPINEL_LOG_TAG, "Spinel UART interface failed to enable");
    ESP_LOGI(ESP_SPINEL_LOG_TAG, "Spinel UART interface has been successfully enabled");
    return ESP_OK;
}

void esp_radio_spinel_init(esp_radio_spinel_idx_t idx)
{
    spinel_iid_t iidList[ot::Spinel::kSpinelHeaderMaxNumIid];
    otInstance *instance = get_instance_from_index(idx);

    // Multipan is not currently supported
    iidList[0] = 0;
    s_spinel_driver[idx].Init(s_spinel_interface[idx].GetSpinelInterface(), true, iidList, ot::Spinel::kSpinelHeaderMaxNumIid);
    s_radio[idx].SetCompatibilityErrorCallback(radio_spinel_compatibility_error_callback, instance);
    s_radio[idx].Init(/*skip_rcp_compatibility_check=*/false, /*reset_radio=*/true, &s_spinel_driver[idx], s_radio_caps, false);
    s_radio[idx].SetVendorRestorePropertiesCallback(esp_radio_spinel_restore_vendor_properities, instance);
}

esp_err_t esp_radio_spinel_enable(esp_radio_spinel_idx_t idx)
{
    otInstance *instance = get_instance_from_index(idx);
    return (s_radio[idx].Enable(instance) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_get_eui64(uint8_t *eui64, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].GetIeeeEui64(eui64) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_panid(uint16_t panid, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].SetPanId(panid) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_short_address(uint16_t short_address, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].SetShortAddress(short_address) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_extended_address(uint8_t *ext_address, esp_radio_spinel_idx_t idx)
{
    otExtAddress aExtAddress;
    memcpy(aExtAddress.m8, (void *)ext_address, OT_EXT_ADDRESS_SIZE);
    return (s_radio[idx].SetExtendedAddress(aExtAddress) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_receive(uint8_t channel, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].Receive(channel) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_energy_scan(uint8_t scan_channel, uint16_t scan_duration, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].EnergyScan(scan_channel, scan_duration) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_transmit(uint8_t *frame, uint8_t channel, bool cca, esp_radio_spinel_idx_t idx)
{
    s_transmit_frame.mLength = frame[0];
    s_transmit_frame.mPsdu = frame + 1;
    s_transmit_frame.mInfo.mTxInfo.mCsmaCaEnabled = cca;
    s_transmit_frame.mInfo.mTxInfo.mMaxCsmaBackoffs = CONFIG_OPENTHREAD_SPINEL_MAC_MAX_CSMA_BACKOFFS_DIRECT;
    s_transmit_frame.mChannel = channel;
    s_transmit_frame.mInfo.mTxInfo.mRxChannelAfterTxDone = channel;
    return (s_radio[idx].Transmit(s_transmit_frame) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_clear_short_entries(esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].ClearSrcMatchShortEntries() == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_add_short_entry(uint16_t short_address, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].AddSrcMatchShortEntry(short_address) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_clear_extended_entries(esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].ClearSrcMatchExtEntries() == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_add_extended_entry(uint8_t *ext_address, esp_radio_spinel_idx_t idx)
{
    otExtAddress aExtAddress;
    memcpy(aExtAddress.m8, (void *)ext_address, OT_EXT_ADDRESS_SIZE);
    return (s_radio[idx].AddSrcMatchExtEntry(aExtAddress) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_promiscuous_mode(bool enable, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].SetPromiscuous(enable) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

/*------------------------------------------------Vendor Property Set-------------------------------------------------------*/
esp_err_t esp_radio_spinel_set_pending_mode(esp_ieee802154_pending_mode_t pending_mode, esp_radio_spinel_idx_t idx)
{
    s_spinel_vendor_property_pendingmode[idx] = pending_mode;
    s_spinel_vendor_property_mask[idx] |= SPINEL_VENDOR_PROPERTY_BIT_PENDINGMODE;
    return (s_radio[idx].Set(SPINEL_PROP_VENDOR_ESP_SET_PENDINGMODE, SPINEL_DATATYPE_INT32_S, static_cast<int32_t>(pending_mode)) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_pan_coord(bool enable, esp_radio_spinel_idx_t idx)
{
    s_spinel_vendor_property_coordinator[idx] = enable;
    s_spinel_vendor_property_mask[idx] |= SPINEL_VENDOR_PROPERTY_BIT_COORDINATOR;
    return (s_radio[idx].Set(SPINEL_PROP_VENDOR_ESP_SET_COORDINATOR, SPINEL_DATATYPE_BOOL_S, enable) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

/*---------------------------------------------------------------------------------------------------------------------------*/


void esp_radio_spinel_radio_update(esp_radio_spinel_mainloop_context_t *mainloop_context, esp_radio_spinel_idx_t idx)
{
    s_spinel_interface[idx].GetSpinelInterface().UpdateFdSet(static_cast<void *>(mainloop_context));
}

void esp_radio_spinel_radio_process(esp_radio_spinel_mainloop_context_t *mainloop_context, esp_radio_spinel_idx_t idx)
{
    s_spinel_driver[idx].Process((void *)mainloop_context);
    s_radio[idx].Process(static_cast<void *>(mainloop_context));
}

esp_err_t esp_radio_spinel_sleep(esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].Sleep() == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_set_tx_power(int8_t power, esp_radio_spinel_idx_t idx)
{
    return (s_radio[idx].SetTransmitPower(power) == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_radio_spinel_get_tx_power(int8_t *power, esp_radio_spinel_idx_t idx)
{
    otError error = OT_ERROR_NONE;
    int8_t aPower;
    error = s_radio[idx].GetTransmitPower(aPower);
    *power = aPower;
    return (error == OT_ERROR_NONE) ? ESP_OK : ESP_FAIL;
}

void esp_radio_spinel_register_rcp_failure_handler(esp_radio_spinel_rcp_failure_handler handler, esp_radio_spinel_idx_t idx)
{
    s_spinel_interface[idx].GetSpinelInterface().RegisterRcpFailureHandler(handler);
}

esp_err_t esp_radio_spinel_rcp_deinit(esp_radio_spinel_idx_t idx)
{
    if (s_radio[idx].IsEnabled()) {
        ESP_RETURN_ON_FALSE(s_radio[idx].Sleep() == OT_ERROR_NONE, ESP_ERR_INVALID_STATE, ESP_SPINEL_LOG_TAG, "Radio fails to sleep");
        ESP_RETURN_ON_FALSE(s_radio[idx].Disable() == OT_ERROR_NONE, ESP_ERR_INVALID_STATE, ESP_SPINEL_LOG_TAG, "Fail to disable radio");
    }
    ESP_RETURN_ON_FALSE(s_spinel_interface[idx].GetSpinelInterface().Disable() == OT_ERROR_NONE, ESP_ERR_INVALID_STATE, ESP_SPINEL_LOG_TAG, "Fail to deinitialize UART");
    return ESP_OK;
}


esp_err_t esp_radio_spinel_rcp_version_get(char *running_rcp_version, esp_radio_spinel_idx_t idx)
{
    const char *rcp_version = s_radio[idx].GetVersion();
    ESP_RETURN_ON_FALSE(rcp_version != nullptr, ESP_FAIL, ESP_SPINEL_LOG_TAG, "Fail to get rcp version");
    strcpy(running_rcp_version, rcp_version);
    return ESP_OK;
}

esp_err_t esp_radio_spinel_set_rcp_ready(esp_radio_spinel_idx_t idx)
{
    s_spinel_driver[idx].SetCoprocessorReady();
    return ESP_OK;
}

// TZ-1261
uint32_t otLinkGetFrameCounter(otInstance *aInstance)
{
    esp_radio_spinel_idx_t idx = get_index_from_instance(aInstance);
    return esp_radio_spinel_extern_get_frame_counter(idx);
}

__attribute__((weak)) uint32_t esp_radio_spinel_extern_get_frame_counter(esp_radio_spinel_idx_t idx)
{
    ESP_LOGW(ESP_SPINEL_LOG_TAG, "None function to get frame counter");
    return 0;
}

namespace ot {
namespace Spinel {

otError RadioSpinel::VendorHandleValueIs(spinel_prop_key_t aPropKey)
{
    otError error = OT_ERROR_NONE;

    switch (aPropKey)
    {
    default:
        ESP_LOGW(ESP_SPINEL_LOG_TAG, "Not Implemented!");
        error = OT_ERROR_NOT_FOUND;
        break;
    }
    return error;
}

} // namespace Spinel
} // namespace ot
