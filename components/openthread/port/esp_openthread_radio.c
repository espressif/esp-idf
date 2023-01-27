/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_radio.h"

#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_ieee802154.h"
#include "esp_ieee802154_types.h"
#include "esp_mac.h"
#include "esp_openthread_common_macro.h"
#include "esp_openthread_platform.h"
#include "esp_openthread_types.h"
#include "esp_random.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_vfs.h"
#include "esp_vfs_eventfd.h"
#include "rom/ets_sys.h"

#include "openthread-core-config.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/radio.h"

#define ESP_RECEIVE_SENSITIVITY -120

#define EVENT_TX_DONE (1 << 0)
#define EVENT_TX_FAILED (1 << 1)
#define EVENT_RX_DONE (1 << 2)
#define EVENT_ENERGY_DETECT_DONE (1 << 3)

typedef struct {
    uint8_t length;
    uint8_t psdu[OT_RADIO_FRAME_MAX_SIZE];
} esp_openthread_radio_tx_psdu;

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t used;
} esp_openthread_circular_queue_info_t;

static otRadioFrame s_transmit_frame;

static esp_openthread_radio_tx_psdu s_transmit_psdu;
static otRadioFrame s_receive_frame[CONFIG_IEEE802154_RX_BUFFER_SIZE];
static otRadioFrame s_ack_frame;
static int s_ed_power;
static int s_rssi = 127;
static esp_ieee802154_tx_error_t s_tx_error;
static int s_radio_event_fd = -1;
static bool s_diag_mode = false;
static const char *s_radio_workflow = "radio";
static uint8_t s_txrx_events;

static esp_openthread_circular_queue_info_t s_recv_queue = {.head = 0, .tail = 0, .used = 0};

static void set_event(uint8_t event)
{
    uint64_t event_write = event;
    s_txrx_events |= event;
    int ret = write(s_radio_event_fd, &event_write, sizeof(event_write));
    assert(ret == sizeof(event_write));
}

static inline void clr_event(uint8_t event)
{
    s_txrx_events &= ~event;
}

static inline bool get_event(uint8_t event)
{
    return s_txrx_events & event;
}

esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config)
{
    ESP_RETURN_ON_FALSE(s_radio_event_fd == -1, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "Radio was initalized already!");

    s_radio_event_fd = eventfd(0, EFD_SUPPORT_ISR);

    s_transmit_frame.mPsdu = s_transmit_psdu.psdu;

    for (uint8_t i = 0; i < CONFIG_IEEE802154_RX_BUFFER_SIZE; i++) {
        s_receive_frame[i].mPsdu = NULL;
    }

    s_ack_frame.mPsdu = NULL;
    memset(&s_recv_queue, 0, sizeof(esp_openthread_circular_queue_info_t));

    esp_ieee802154_enable();
    esp_ieee802154_set_promiscuous(false);
    esp_ieee802154_set_rx_when_idle(true);

    return esp_openthread_platform_workflow_register(&esp_openthread_radio_update, &esp_openthread_radio_process,
                                                     s_radio_workflow);
}

void esp_openthread_radio_deinit(void)
{
    if (s_radio_event_fd > 0) {
        close(s_radio_event_fd);
        s_radio_event_fd = -1;
    }

    esp_ieee802154_disable();
    esp_openthread_platform_workflow_unregister(s_radio_workflow);
}

void esp_openthread_radio_update(esp_openthread_mainloop_context_t *mainloop)
{
    FD_SET(s_radio_event_fd, &mainloop->read_fds);
    if (s_radio_event_fd > mainloop->max_fd) {
        mainloop->max_fd = s_radio_event_fd;
    }
}

esp_err_t esp_openthread_radio_process(otInstance *aInstance, const esp_openthread_mainloop_context_t *mainloop)
{
    uint64_t event_read;
    int ret = read(s_radio_event_fd, &event_read, sizeof(event_read));
    assert(ret == sizeof(event_read));

    if (get_event(EVENT_TX_DONE)) {
        clr_event(EVENT_TX_DONE);
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet()) {
            otPlatDiagRadioTransmitDone(aInstance, &s_transmit_frame, OT_ERROR_NONE);
        } else
#endif
        {
            if (s_ack_frame.mPsdu == NULL) {
                otPlatRadioTxDone(aInstance, &s_transmit_frame, NULL, OT_ERROR_NONE);
            } else {
                otPlatRadioTxDone(aInstance, &s_transmit_frame, &s_ack_frame, OT_ERROR_NONE);
                s_ack_frame.mPsdu = NULL;
            }
        }
    }

    if (get_event(EVENT_TX_FAILED)) {
        clr_event(EVENT_TX_FAILED);
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet()) {
            otPlatDiagRadioTransmitDone(aInstance, &s_transmit_frame, OT_ERROR_CHANNEL_ACCESS_FAILURE);
        } else
#endif
        {
            otError err = OT_ERROR_NONE;

            switch (s_tx_error) {
            case ESP_IEEE802154_TX_ERR_CCA_BUSY:
            case ESP_IEEE802154_TX_ERR_ABORT:
            case ESP_IEEE802154_TX_ERR_COEXIST:
            case ESP_IEEE802154_TX_ERR_COEXIST_REJ:
                err = OT_ERROR_CHANNEL_ACCESS_FAILURE;
                break;

            case ESP_IEEE802154_TX_ERR_NO_ACK:
            case ESP_IEEE802154_TX_ERR_INVALID_ACK:
            case ESP_IEEE802154_TX_ERR_COEXIST_ACK:
                err = OT_ERROR_NO_ACK;
                break;

            default:
                ETS_ASSERT(false);
                break;
            }

            otPlatRadioTxDone(aInstance, &s_transmit_frame, NULL, err);
        }
    }

    if (get_event(EVENT_ENERGY_DETECT_DONE)) {
        clr_event(EVENT_ENERGY_DETECT_DONE);
        otPlatRadioEnergyScanDone(aInstance, s_ed_power);
    }

    while (s_recv_queue.used) {
        if (s_receive_frame[s_recv_queue.head].mPsdu != NULL) {
#if OPENTHREAD_CONFIG_DIAG_ENABLE
            if (otPlatDiagModeGet()) {
                otPlatDiagRadioReceiveDone(aInstance, &s_receive_frame[s_recv_queue.head], OT_ERROR_NONE);
            } else
#endif
            {
                otPlatRadioReceiveDone(aInstance, &s_receive_frame[s_recv_queue.head], OT_ERROR_NONE);
            }
            s_receive_frame[s_recv_queue.head].mPsdu = NULL;
            s_recv_queue.head = (s_recv_queue.head + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
            s_recv_queue.used--;
        }
    }

    return ESP_OK;
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    uint8_t eui64[8] = {0};
    esp_read_mac(eui64, ESP_MAC_IEEE802154);
    memcpy(aIeeeEui64, eui64, sizeof(eui64));
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t panid)
{
    esp_ieee802154_set_panid(panid);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    esp_ieee802154_set_extended_address(aAddress->m8);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    esp_ieee802154_set_short_address(aAddress);
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    esp_ieee802154_set_promiscuous(aEnable);
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    return esp_ieee802154_get_state() != ESP_IEEE802154_RADIO_DISABLE;
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    // radio has been enabled in esp_openthread_radio_init()

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    esp_ieee802154_disable();

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    esp_ieee802154_sleep();

    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    esp_ieee802154_set_channnel(aChannel);
    esp_ieee802154_receive();

    return OT_ERROR_NONE;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    esp_ieee802154_set_channnel(aFrame->mChannel);

    aFrame->mPsdu[-1] = aFrame->mLength; // lenth locates one byte before the psdu (esp_openthread_radio_tx_psdu);

    esp_ieee802154_transmit(&aFrame->mPsdu[-1], aFrame->mInfo.mTxInfo.mCsmaCaEnabled);

    otPlatRadioTxStarted(aInstance, aFrame);

    return OT_ERROR_NONE;
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    return &s_transmit_frame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    return s_rssi;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    return (otRadioCaps)(OT_RADIO_CAPS_ENERGY_SCAN | OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_SLEEP_TO_TX);
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    return esp_ieee802154_get_promiscuous();
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ENABLE);
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    esp_ieee802154_add_pending_addr((uint8_t *)&aShortAddress, true);
    return OT_ERROR_NONE;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    esp_ieee802154_add_pending_addr(aExtAddress->m8, false);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    esp_ieee802154_clear_pending_addr((uint8_t *)&aShortAddress, true);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    esp_ieee802154_clear_pending_addr(aExtAddress->m8, false);
    return OT_ERROR_NONE;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    esp_ieee802154_reset_pending_table(true);
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    esp_ieee802154_reset_pending_table(false);
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    esp_ieee802154_energy_detect(aScanDuration);

    return OT_ERROR_NONE;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    *aPower = esp_ieee802154_get_txpower();

    return OT_ERROR_NONE;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    esp_ieee802154_set_txpower(aPower);

    return OT_ERROR_NONE;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    *aThreshold = esp_ieee802154_get_cca_threshold();

    return OT_ERROR_NONE;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    esp_ieee802154_set_cca_threshold(aThreshold);

    return OT_ERROR_NONE;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    return ESP_RECEIVE_SENSITIVITY;
}

void otPlatDiagModeSet(bool mode)
{
    s_diag_mode = mode;
}

bool otPlatDiagModeGet(void)
{
    return s_diag_mode;
}

void otPlatDiagTxPowerSet(int8_t tx_power)
{
    OT_UNUSED_VARIABLE(tx_power);
}

void otPlatDiagChannelSet(uint8_t channel)
{
    OT_UNUSED_VARIABLE(channel);
}

void otPlatDiagRadioReceived(otInstance *aInstance, otRadioFrame *frame, otError error)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(frame);
    OT_UNUSED_VARIABLE(error);
}

void otPlatDiagAlarmCallback(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
}

// events
void IRAM_ATTR esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack,
                                            esp_ieee802154_frame_info_t *s_ack_frame_info)
{
    ETS_ASSERT(frame == (uint8_t *)&s_transmit_psdu);

    if (ack != NULL) {
        s_ack_frame.mLength = (uint16_t)(*ack);
        s_ack_frame.mPsdu = (uint8_t *)(ack + 1);
    }

    set_event(EVENT_TX_DONE);
}

void IRAM_ATTR esp_ieee802154_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info)
{
    if (s_recv_queue.used == CONFIG_IEEE802154_RX_BUFFER_SIZE) {
        ESP_EARLY_LOGE(OT_PLAT_LOG_TAG, "radio receive buffer full!");
        return;
    }

    s_rssi = frame_info->rssi;

    s_receive_frame[s_recv_queue.tail].mPsdu = data + 1;
    s_receive_frame[s_recv_queue.tail].mLength = *data;
    s_receive_frame[s_recv_queue.tail].mChannel = frame_info->channel;
    s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mRssi = frame_info->rssi;
    s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mAckedWithFramePending = frame_info->pending;
    s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mTimestamp = esp_timer_get_time();

    s_recv_queue.tail = (s_recv_queue.tail + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
    s_recv_queue.used++;
    set_event(EVENT_RX_DONE);
}

void IRAM_ATTR esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    ETS_ASSERT(frame == (uint8_t *)&s_transmit_psdu);

    s_tx_error = error;

    set_event(EVENT_TX_FAILED);
}

void IRAM_ATTR esp_ieee802154_receive_sfd_done(void)
{
}

void IRAM_ATTR esp_ieee802154_transmit_sfd_done(uint8_t *frame)
{
}

void IRAM_ATTR esp_ieee802154_energy_detect_done(int8_t power)
{
    s_ed_power = power;

    set_event(EVENT_ENERGY_DETECT_DONE);
}

void IRAM_ATTR esp_ieee802154_cca_done(bool channel_free)
{
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    esp_fill_random(aOutput, aOutputLength);

    return OT_ERROR_NONE;
}
