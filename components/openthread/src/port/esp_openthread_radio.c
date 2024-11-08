/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdatomic.h>
#include "esp_openthread_radio.h"

#include "esp_err.h"
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
#include "openthread/error.h"
#include "openthread/link.h"
#include "openthread/platform/diag.h"
#include "openthread/platform/radio.h"
#include "openthread/platform/time.h"
#include "utils/link_metrics.h"
#include "utils/mac_frame.h"

#define ESP_RECEIVE_SENSITIVITY -120
#define ESP_OPENTHREAD_XTAL_ACCURACY CONFIG_OPENTHREAD_XTAL_ACCURACY
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#define ESP_OPENTHREAD_CSL_ACCURACY CONFIG_OPENTHREAD_CSL_ACCURACY
#define ESP_OPENTHREAD_CSL_UNCERTAIN CONFIG_OPENTHREAD_CSL_UNCERTAIN
#endif

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
    atomic_uint_fast8_t used;
} esp_openthread_circular_queue_info_t;

static otRadioFrame s_transmit_frame;

static esp_openthread_radio_tx_psdu s_transmit_psdu;
static uint8_t *s_enhack;
static otRadioFrame s_receive_frame[CONFIG_IEEE802154_RX_BUFFER_SIZE];
static otRadioFrame s_ack_frame;
static int s_ed_power;
static esp_ieee802154_tx_error_t s_tx_error;
static int s_radio_event_fd = -1;
static const char *s_radio_workflow = "radio";
static uint8_t s_txrx_events;

#if CONFIG_OPENTHREAD_DIAG
static bool s_diag_mode = false;
#endif

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
static otRadioIeInfo s_transmit_ie_info;
#endif // OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
static uint32_t s_csl_period;
static uint32_t s_csl_sample_time;
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
static uint32_t s_mac_frame_counter;
static uint8_t s_key_id;
static struct otMacKeyMaterial s_pervious_key;
static struct otMacKeyMaterial s_current_key;
static struct otMacKeyMaterial s_next_key;
static bool s_with_security_enh_ack = false;
static uint32_t s_ack_frame_counter;
static uint8_t s_ack_key_id;
static uint8_t s_security_key[16];
static uint8_t s_security_addr[8];
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

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
                        "Radio was initialized already!");

    s_radio_event_fd = eventfd(0, EFD_SUPPORT_ISR);

    s_transmit_frame.mPsdu = s_transmit_psdu.psdu;

    for (uint8_t i = 0; i < CONFIG_IEEE802154_RX_BUFFER_SIZE; i++) {
        s_receive_frame[i].mPsdu = NULL;
    }

    s_ack_frame.mPsdu = NULL;
    memset(&s_recv_queue, 0, sizeof(esp_openthread_circular_queue_info_t));

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
    s_transmit_frame.mInfo.mTxInfo.mIeInfo = &s_transmit_ie_info;
#endif

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
#if CONFIG_OPENTHREAD_DIAG
        if (otPlatDiagModeGet()) {
            otPlatDiagRadioTransmitDone(aInstance, &s_transmit_frame, OT_ERROR_NONE);
        } else
#endif
        {
            if (s_ack_frame.mPsdu == NULL) {
                otPlatRadioTxDone(aInstance, &s_transmit_frame, NULL, OT_ERROR_NONE);
            } else {
                otPlatRadioTxDone(aInstance, &s_transmit_frame, &s_ack_frame, OT_ERROR_NONE);
                esp_ieee802154_receive_handle_done(s_ack_frame.mPsdu - 1);
                s_ack_frame.mPsdu = NULL;
            }
        }
    }

    if (get_event(EVENT_TX_FAILED)) {
        clr_event(EVENT_TX_FAILED);
#if CONFIG_OPENTHREAD_DIAG
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
                err = OT_ERROR_CHANNEL_ACCESS_FAILURE;
                break;

            case ESP_IEEE802154_TX_ERR_NO_ACK:
            case ESP_IEEE802154_TX_ERR_INVALID_ACK:
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

    while (atomic_load(&s_recv_queue.used)) {
        if (s_receive_frame[s_recv_queue.head].mPsdu != NULL) {
#if CONFIG_OPENTHREAD_DIAG
            if (otPlatDiagModeGet()) {
                otPlatDiagRadioReceiveDone(aInstance, &s_receive_frame[s_recv_queue.head], OT_ERROR_NONE);
            } else
#endif
            {
                otPlatRadioReceiveDone(aInstance, &s_receive_frame[s_recv_queue.head], OT_ERROR_NONE);
            }
            esp_ieee802154_receive_handle_done(s_receive_frame[s_recv_queue.head].mPsdu - 1);
            s_receive_frame[s_recv_queue.head].mPsdu = NULL;
            s_recv_queue.head = (s_recv_queue.head + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
            atomic_fetch_sub(&s_recv_queue.used, 1);
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
    // radio will be disabled in esp_openthread_radio_deinit()

    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    esp_ieee802154_sleep();

    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    esp_ieee802154_set_channel(aChannel);
    esp_ieee802154_receive();

    return OT_ERROR_NONE;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    esp_ieee802154_set_channel(aFrame->mChannel);

    aFrame->mPsdu[-1] = aFrame->mLength; // length locates one byte before the psdu (esp_openthread_radio_tx_psdu);

    if (otMacFrameIsSecurityEnabled(aFrame) && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed) {
        if (!s_transmit_frame.mInfo.mTxInfo.mIsARetx) {
            otMacFrameSetFrameCounter(aFrame, s_mac_frame_counter++);
        }
        if (otMacFrameIsKeyIdMode1(aFrame)) {
            s_transmit_frame.mInfo.mTxInfo.mAesKey = &s_current_key;
            if (!s_transmit_frame.mInfo.mTxInfo.mIsARetx) {
                otMacFrameSetKeyId(aFrame, s_key_id);
            }
            esp_ieee802154_get_extended_address(s_security_addr);
        }
        memcpy(s_security_key, s_current_key.mKeyMaterial.mKey.m8, sizeof(s_current_key.mKeyMaterial.mKey.m8));
        esp_ieee802154_set_transmit_security(&aFrame->mPsdu[-1], s_security_key, s_security_addr);
    }

    if (aFrame->mInfo.mTxInfo.mTxDelay != 0) {
        esp_ieee802154_transmit_at(&aFrame->mPsdu[-1], aFrame->mInfo.mTxInfo.mCsmaCaEnabled,
                                   (aFrame->mInfo.mTxInfo.mTxDelayBaseTime + aFrame->mInfo.mTxInfo.mTxDelay));
    } else
    {
        esp_ieee802154_transmit(&aFrame->mPsdu[-1], aFrame->mInfo.mTxInfo.mCsmaCaEnabled);
    }

    otPlatRadioTxStarted(aInstance, aFrame);

    return OT_ERROR_NONE;
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    return &s_transmit_frame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    return esp_ieee802154_get_recent_rssi();
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
//  FIXME: Remove `CONFIG_OPENTHREAD_RX_ON_WHEN_IDLE` when JIRA: TZ-609 fixed.
    return (otRadioCaps)(OT_RADIO_CAPS_ENERGY_SCAN |
#if CONFIG_OPENTHREAD_RX_ON_WHEN_IDLE
                        OT_RADIO_CAPS_RX_ON_WHEN_IDLE |
#endif
                        OT_RADIO_CAPS_TRANSMIT_SEC | OT_RADIO_CAPS_RECEIVE_TIMING | OT_RADIO_CAPS_TRANSMIT_TIMING |
                        OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_SLEEP_TO_TX);
}

otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    esp_ieee802154_receive_at((aStart + aDuration));
    return OT_ERROR_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    return esp_ieee802154_get_promiscuous();
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ENHANCED);
#else
    esp_ieee802154_set_pending_mode(ESP_IEEE802154_AUTO_PENDING_ENABLE);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
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
    esp_ieee802154_set_channel(aScanChannel);
    esp_ieee802154_energy_detect(aScanDuration * US_PER_MS / US_PER_SYMBLE);

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

#if CONFIG_OPENTHREAD_DIAG

void otPlatDiagSetOutputCallback(otInstance *aInstance, otPlatDiagOutputCallback aCallback, void *aContext)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aCallback);
    OT_UNUSED_VARIABLE(aContext);
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

#endif // CONFIG_OPENTHREAD_DIAG

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
void otPlatRadioSetMacKey(otInstance *aInstance, uint8_t aKeyIdMode, uint8_t aKeyId, const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey, const otMacKeyMaterial *aNextKey, otRadioKeyType aKeyType)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyIdMode);
    assert(aKeyType == OT_KEY_TYPE_LITERAL_KEY);
    assert(aPrevKey != NULL && aCurrKey != NULL && aNextKey != NULL);

    s_key_id = aKeyId;
    s_pervious_key = *aPrevKey;
    s_current_key = *aCurrKey;
    s_next_key = *aNextKey;
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    OT_UNUSED_VARIABLE(aInstance);

    s_mac_frame_counter = aMacFrameCounter;
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return otPlatTimeGet();
}

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);
    s_csl_sample_time = aCslSampleTime;
}

static IRAM_ATTR uint16_t get_csl_phase()
{
    uint32_t cur_time = otPlatTimeGet();
    uint32_t csl_period_us = s_csl_period * OT_US_PER_TEN_SYMBOLS;
    uint32_t diff = (csl_period_us - (cur_time % csl_period_us) + (s_csl_sample_time % csl_period_us)) % csl_period_us;

    return (uint16_t)(diff / OT_US_PER_TEN_SYMBOLS + 1);
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

uint16_t otPlatTimeGetXtalAccuracy(void)
{
    return ESP_OPENTHREAD_XTAL_ACCURACY;
}

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance *aInstance, otLinkMetrics aLinkMetrics,
                                          const otShortAddress aShortAddress, const otExtAddress *aExtAddress)
{
    otError error = otLinkMetricsConfigureEnhAckProbing(aShortAddress, aExtAddress, aLinkMetrics);
    return error;
}
#endif // OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
otError otPlatRadioEnableCsl(otInstance *aInstance, uint32_t aCslPeriod, otShortAddress aShortAddr,
                             const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddr);
    OT_UNUSED_VARIABLE(aExtAddr);
    s_csl_period = aCslPeriod;

    return OT_ERROR_NONE;
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    return ESP_OPENTHREAD_CSL_ACCURACY;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    return ESP_OPENTHREAD_CSL_UNCERTAIN;
}

#endif

// events
void IRAM_ATTR esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack,
                                            esp_ieee802154_frame_info_t *ack_frame_info)
{
    ETS_ASSERT(frame == (uint8_t *)&s_transmit_psdu);

    if (ack != NULL) {
        s_ack_frame.mLength = (uint16_t)(*ack);
        s_ack_frame.mPsdu = (uint8_t *)(ack + 1);
        s_ack_frame.mChannel = ack_frame_info->channel;
        s_ack_frame.mInfo.mRxInfo.mRssi = ack_frame_info->rssi;
        s_ack_frame.mInfo.mRxInfo.mLqi = ack_frame_info->lqi;
        s_ack_frame.mInfo.mRxInfo.mTimestamp = ack_frame_info->timestamp;
    }

    set_event(EVENT_TX_DONE);
}

static void IRAM_ATTR convert_to_ot_frame(uint8_t *data, esp_ieee802154_frame_info_t *frame_info,
                                          otRadioFrame *radio_frame)
{
    radio_frame->mPsdu = data + 1;
    radio_frame->mLength = *data;
    radio_frame->mChannel = frame_info->channel;
    radio_frame->mInfo.mRxInfo.mRssi = frame_info->rssi;
    radio_frame->mInfo.mRxInfo.mLqi = frame_info->lqi;
    radio_frame->mInfo.mRxInfo.mAckedWithFramePending = frame_info->pending;
    radio_frame->mInfo.mRxInfo.mTimestamp = otPlatTimeGet();

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    radio_frame->mInfo.mRxInfo.mTimestamp = frame_info->timestamp;
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
}

static esp_err_t IRAM_ATTR enh_ack_set_security_addr_and_key(otRadioFrame *ack_frame)
{
    struct otMacKeyMaterial *key = NULL;
    uint8_t key_id;

    ETS_ASSERT(otMacFrameIsSecurityEnabled(ack_frame));
    key_id = otMacFrameGetKeyId(ack_frame);
    if(!(otMacFrameIsKeyIdMode1(ack_frame) && key_id != 0)) {
        return ESP_FAIL;
    }

    if (key_id == s_key_id) {
        key = &s_current_key;
    } else if (key_id == s_key_id - 1) {
        key = &s_pervious_key;
    } else if (key_id == s_key_id + 1) {
        key = &s_next_key;
    } else {
        return ESP_FAIL;
    }
    s_ack_frame_counter = s_mac_frame_counter;
    s_ack_key_id = key_id;
    s_with_security_enh_ack = true;
    if (otMacFrameIsKeyIdMode1(ack_frame)) {
        esp_ieee802154_get_extended_address(s_security_addr);
        memcpy(s_security_key, (*key).mKeyMaterial.mKey.m8, OT_MAC_KEY_SIZE);
    }

    esp_ieee802154_set_transmit_security(&ack_frame->mPsdu[-1], s_security_key, s_security_addr);
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_ieee802154_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info,
                                                uint8_t *enhack_frame)
{
    otRadioFrame ack_frame;
    otRadioFrame ot_frame;
    uint8_t ack_ie_data[OT_ACK_IE_MAX_SIZE];
    uint8_t offset = 0;
#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    uint8_t link_metrics_data_len = 0;
    uint8_t link_metrics_data[OT_ENH_PROBING_IE_DATA_MAX_SIZE];
    otMacAddress mac_addr;
#endif
    otError err;
    ack_frame.mPsdu = enhack_frame + 1;
    convert_to_ot_frame(frame, frame_info, &ot_frame);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (s_csl_period > 0) {
        offset += otMacFrameGenerateCslIeTemplate(ack_ie_data);
    }
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    otMacFrameGetSrcAddr(&ot_frame, &mac_addr);
    link_metrics_data_len = otLinkMetricsEnhAckGenData(&mac_addr, esp_ieee802154_get_recent_lqi(),
                                        esp_ieee802154_get_recent_rssi(), link_metrics_data);
    if (link_metrics_data_len > 0) {
        offset += otMacFrameGenerateEnhAckProbingIe(ack_ie_data, link_metrics_data, link_metrics_data_len);
    }
#endif
    err = otMacFrameGenerateEnhAck(&ot_frame, frame_info->pending, ack_ie_data, offset, &ack_frame);

    if (err != OT_ERROR_NONE) {
        return ESP_FAIL;
    }
    enhack_frame[0] = ack_frame.mLength;

    s_enhack = enhack_frame;

    if (otMacFrameIsSecurityEnabled(&ack_frame) && !ack_frame.mInfo.mTxInfo.mIsSecurityProcessed) {
        otMacFrameSetFrameCounter(&ack_frame, s_mac_frame_counter++);
        if (enh_ack_set_security_addr_and_key(&ack_frame) != ESP_OK) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

void IRAM_ATTR esp_ieee802154_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info)
{
    otRadioFrame ot_frame;
    ot_frame.mPsdu = data + 1;

    if (atomic_load(&s_recv_queue.used) == CONFIG_IEEE802154_RX_BUFFER_SIZE) {
        ESP_EARLY_LOGE(OT_PLAT_LOG_TAG, "radio receive buffer full!");
        return;
    }

    convert_to_ot_frame(data, frame_info, &(s_receive_frame[s_recv_queue.tail]));
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    // Inform if this frame was acknowledged with secured Enh-ACK.
    if (otMacFrameIsAckRequested(&ot_frame) && otMacFrameIsVersion2015(&ot_frame)) {
        s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mAckedWithSecEnhAck = s_with_security_enh_ack;
        s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mAckFrameCounter = s_ack_frame_counter;
        s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mAckKeyId = s_ack_key_id;
    } else {
        s_receive_frame[s_recv_queue.tail].mInfo.mRxInfo.mAckedWithSecEnhAck = false;
    }
    s_with_security_enh_ack = false;
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    s_recv_queue.tail = (s_recv_queue.tail + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
    atomic_fetch_add(&s_recv_queue.used, 1);
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
    assert(frame == (uint8_t *)&s_transmit_psdu || frame == s_enhack);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (s_csl_period > 0) {
        otRadioFrame ot_frame;
        ot_frame.mPsdu = frame + 1;
        ot_frame.mLength = frame[0];

        otMacFrameSetCslIe(&ot_frame, s_csl_period, get_csl_phase());
    }
#endif

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if (frame == (uint8_t *)&s_transmit_psdu && s_transmit_frame.mInfo.mTxInfo.mIeInfo->mTimeIeOffset != 0)
    {
        uint8_t *p_time_ie = s_transmit_frame.mPsdu + s_transmit_frame.mInfo.mTxInfo.mIeInfo->mTimeIeOffset;
        uint64_t time = (uint64_t)((int64_t)otPlatTimeGet() + s_transmit_frame.mInfo.mTxInfo.mIeInfo->mNetworkTimeOffset);

        *p_time_ie = s_transmit_frame.mInfo.mTxInfo.mIeInfo->mTimeSyncSeq;

        *(++p_time_ie) = (uint8_t)(time & 0xff);
        for (uint8_t i = 1; i < sizeof(uint64_t); i++)
        {
            time           = time >> 8;
            *(++p_time_ie) = (uint8_t)(time & 0xff);
        }
    }
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
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

otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aChannel);
    OT_UNUSED_VARIABLE(aMaxPower);

    return OT_ERROR_NONE;
}

#if CONFIG_OPENTHREAD_RX_ON_WHEN_IDLE
void otPlatRadioSetRxOnWhenIdle(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);
    esp_ieee802154_set_rx_when_idle(aEnable);
}
#endif
