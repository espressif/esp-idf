/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread_radio.h"

#include "esp_err.h"
#include "sdkconfig.h"
#include "esp_check.h"
#include "esp_ieee802154.h"
#include "esp_ieee802154_multipan.h"
#include "esp_ieee802154_types.h"
#include "esp_mac.h"
#include "esp_openthread.h"
#include "esp_openthread_common.h"
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
#include "openthread/platform/multipan.h"
#include "openthread/platform/radio.h"
#include "openthread/platform/time.h"
#include "utils/link_metrics.h"
#include "utils/mac_frame.h"
#include "psa/crypto.h"

#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
#include "esp_coex_i154.h"
#endif

#define ESP_OPENTHREAD_XTAL_ACCURACY CONFIG_OPENTHREAD_XTAL_ACCURACY
#define ESP_OPENTHREAD_CSL_ACCURACY CONFIG_OPENTHREAD_CSL_ACCURACY
#define ESP_OPENTHREAD_CSL_UNCERTAIN CONFIG_OPENTHREAD_CSL_UNCERTAIN

#define EVENT_TX_DONE (1 << 0)
#define EVENT_TX_FAILED (1 << 1)
#define EVENT_RX_DONE (1 << 2)
#define EVENT_ENERGY_DETECT_DONE (1 << 3)
#define EVENT_SLEEP (1 << 4)

typedef struct {
    uint8_t length;
    uint8_t psdu[OT_RADIO_FRAME_MAX_SIZE];
} esp_openthread_radio_tx_psdu;

typedef struct esp_openthread_radio_transmit_context {
    int8_t index;
    otRadioFrame *aFrame;
} esp_openthread_radio_transmit_context_t;

typedef struct esp_openthread_radio_receive_context {
    int8_t index;
    otRadioFrame frame;
} esp_openthread_radio_receive_context_t;

static otRadioFrame s_transmit_frame[OT_INSTANCE_COUNT];
static esp_openthread_radio_tx_psdu s_transmit_psdu[OT_INSTANCE_COUNT];
static uint8_t *s_enhack;
static esp_openthread_radio_receive_context_t s_receive_frame[CONFIG_IEEE802154_RX_BUFFER_SIZE];
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
static otRadioIeInfo s_transmit_ie_info[OT_INSTANCE_COUNT];
#endif // OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
static uint32_t s_csl_period;
static uint32_t s_csl_sample_time;
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
typedef struct {
    uint32_t mac_frame_counter;
    uint8_t key_id;
    struct otMacKeyMaterial previous_key;
    struct otMacKeyMaterial current_key;
    struct otMacKeyMaterial next_key;
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    uint8_t previous_key_bytes[16];
    uint8_t current_key_bytes[16];
    uint8_t next_key_bytes[16];
#endif
    bool with_security_enh_ack;
    uint32_t ack_frame_counter;
    uint8_t ack_key_id;
    uint8_t security_key[16];
    uint8_t security_addr[8];
} esp_openthread_radio_security_ctx_t;

static esp_openthread_radio_security_ctx_t s_sec_ctx[OT_INSTANCE_COUNT];
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

/* Head is the in-flight TX; remaining entries are pending.
 * Only touched in OpenThread task context (ISR only reads head/index). */
typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t used;
} esp_openthread_tx_queue_info_t;

static esp_openthread_radio_transmit_context_t s_tx_queue[OT_INSTANCE_COUNT];
static esp_openthread_tx_queue_info_t s_tx_queue_info = {.head = 0, .tail = 0, .used = 0};

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
static void ot_set_security_key_from_key_material(int8_t idx, struct otMacKeyMaterial a_key_material)
{
    if (idx < 0 || idx >= OT_INSTANCE_COUNT) {
        return;
    }
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    /* Key bytes are pre-exported in task context (otPlatRadioSetMacKey).
     * Identify which cached buffer to use by comparing the key reference.
     * Jira TZ-2472 */
    if (a_key_material.mKeyMaterial.mKeyRef == s_sec_ctx[idx].previous_key.mKeyMaterial.mKeyRef) {
        memcpy(s_sec_ctx[idx].security_key, s_sec_ctx[idx].previous_key_bytes, 16);
    } else if (a_key_material.mKeyMaterial.mKeyRef == s_sec_ctx[idx].next_key.mKeyMaterial.mKeyRef) {
        memcpy(s_sec_ctx[idx].security_key, s_sec_ctx[idx].next_key_bytes, 16);
    } else {
        memcpy(s_sec_ctx[idx].security_key, s_sec_ctx[idx].current_key_bytes, 16);
    }
#else
    memcpy(s_sec_ctx[idx].security_key, a_key_material.mKeyMaterial.mKey.m8,
           sizeof(a_key_material.mKeyMaterial.mKey.m8));
#endif
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

static esp_openthread_circular_queue_info_t s_recv_queue = {.head = 0, .tail = 0, .used = 0};

static void radio_tx_queue_pop_head(void)
{
    assert(s_tx_queue_info.used > 0);
    s_tx_queue_info.head = (s_tx_queue_info.head + 1) % OT_INSTANCE_COUNT;
    s_tx_queue_info.used--;
}

static void radio_start_transmit(otInstance *aInstance, otRadioFrame *aFrame, int8_t idx)
{
    esp_ieee802154_set_channel(aFrame->mChannel);

    aFrame->mPsdu[-1] = aFrame->mLength; // length locates one byte before the psdu (esp_openthread_radio_tx_psdu);

    if (otMacFrameIsSecurityEnabled(aFrame) && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed) {
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
        if (!s_transmit_frame[idx].mInfo.mTxInfo.mIsARetx || s_csl_period > 0) {
#else
        if (!s_transmit_frame[idx].mInfo.mTxInfo.mIsARetx) {
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
            otMacFrameSetFrameCounter(aFrame, s_sec_ctx[idx].mac_frame_counter++);
        }
        if (otMacFrameIsKeyIdMode1(aFrame)) {
            s_transmit_frame[idx].mInfo.mTxInfo.mAesKey = &s_sec_ctx[idx].current_key;
            if (!s_transmit_frame[idx].mInfo.mTxInfo.mIsARetx) {
                otMacFrameSetKeyId(aFrame, s_sec_ctx[idx].key_id);
            }
            esp_ieee802154_get_multipan_extended_address((esp_ieee802154_multipan_index_t)idx, s_sec_ctx[idx].security_addr);
        }
        ot_set_security_key_from_key_material(idx, s_sec_ctx[idx].current_key);
        esp_ieee802154_set_transmit_security(&aFrame->mPsdu[-1], s_sec_ctx[idx].security_key,
                                             s_sec_ctx[idx].security_addr);
    }

    if (aFrame->mInfo.mTxInfo.mTxDelay != 0) {
        esp_ieee802154_transmit_at(&aFrame->mPsdu[-1], aFrame->mInfo.mTxInfo.mCsmaCaEnabled,
                                   (aFrame->mInfo.mTxInfo.mTxDelayBaseTime + aFrame->mInfo.mTxInfo.mTxDelay));
    } else {
        esp_ieee802154_transmit(&aFrame->mPsdu[-1], aFrame->mInfo.mTxInfo.mCsmaCaEnabled);
    }

    otPlatRadioTxStarted(aInstance, aFrame);
}

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

static void ot_radio_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info);
static void ot_radio_receive_sfd_done(void);
static void ot_radio_transmit_done(const uint8_t *frame, const uint8_t *ack,
                                   esp_ieee802154_frame_info_t *ack_frame_info);
static void ot_radio_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error);
static void ot_radio_transmit_sfd_done(uint8_t *frame);
static void ot_radio_energy_detect_done(int8_t power);
static esp_err_t ot_radio_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info,
                                            uint8_t *enhack_frame);

static void radio_init_software_context(void)
{
    for (int idx = 0; idx < OT_INSTANCE_COUNT; idx++) {
        s_transmit_frame[idx].mPsdu = s_transmit_psdu[idx].psdu;
    }

    for (uint8_t i = 0; i < CONFIG_IEEE802154_RX_BUFFER_SIZE; i++) {
        s_receive_frame[i].frame.mPsdu = NULL;
    }

    s_ack_frame.mPsdu = NULL;
    memset(&s_recv_queue, 0, sizeof(esp_openthread_circular_queue_info_t));
    memset(&s_tx_queue_info, 0, sizeof(esp_openthread_tx_queue_info_t));
    s_txrx_events = 0;

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT
    for (int idx = 0; idx < OT_INSTANCE_COUNT; idx++) {
        s_transmit_frame[idx].mInfo.mTxInfo.mIeInfo = &s_transmit_ie_info[idx];
    }
#endif
}

esp_err_t esp_openthread_radio_init(const esp_openthread_platform_config_t *config)
{
    esp_err_t ret = ESP_OK;
    const uint8_t multipan_mask = (uint8_t)((1U << CONFIG_IEEE802154_INTERFACE_NUM) - 1U);

    ESP_RETURN_ON_FALSE(s_radio_event_fd == -1, ESP_ERR_INVALID_STATE, OT_PLAT_LOG_TAG,
                        "Radio was initialized already!");

    esp_ieee802154_event_cb_list_t cb_list = {
        .rx_done_cb = ot_radio_receive_done,
        .rx_sfd_done_cb = ot_radio_receive_sfd_done,
        .tx_done_cb = ot_radio_transmit_done,
        .tx_failed_cb = ot_radio_transmit_failed,
        .tx_sfd_done_cb = ot_radio_transmit_sfd_done,
        .ed_done_cb = ot_radio_energy_detect_done,
        .enh_ack_generator_cb = ot_radio_enh_ack_generator,
    };
    ESP_RETURN_ON_ERROR(esp_ieee802154_event_callback_list_register(cb_list), OT_PLAT_LOG_TAG,
                        "Failed to register ieee802154 event callbacks");

    s_radio_event_fd = eventfd(0, EFD_SUPPORT_ISR);
    ESP_GOTO_ON_FALSE(s_radio_event_fd != -1, ESP_FAIL, exit_unregister, OT_PLAT_LOG_TAG, "Failed to create eventfd");

    radio_init_software_context();

    esp_ieee802154_enable();
    ESP_GOTO_ON_ERROR(esp_ieee802154_set_multipan_enable(multipan_mask), exit, OT_PLAT_LOG_TAG,
                      "Fail to set multipan mask");

    for (int i = 0; i < CONFIG_IEEE802154_INTERFACE_NUM; i++) {
        if (multipan_mask & (1U << i)) {
            esp_ieee802154_multipan_set_rx_when_idle(i, true);
        }
    }
    esp_ieee802154_set_promiscuous(false);

    ESP_GOTO_ON_ERROR(esp_openthread_platform_workflow_register(&esp_openthread_radio_update,
                                                                &esp_openthread_radio_process,
                                                                s_radio_workflow),
                      exit, OT_PLAT_LOG_TAG, "Fail to register radio workflow");

    return ESP_OK;

exit:
    esp_ieee802154_disable();
    if (s_radio_event_fd != -1) {
        close(s_radio_event_fd);
        s_radio_event_fd = -1;
    }
exit_unregister:
    esp_ieee802154_event_callback_list_unregister();
    return ret;
}

void esp_openthread_radio_deinit(void)
{
    if (s_radio_event_fd != -1) {
        close(s_radio_event_fd);
        s_radio_event_fd = -1;
    }

    memset(&s_recv_queue, 0, sizeof(esp_openthread_circular_queue_info_t));
    memset(&s_tx_queue_info, 0, sizeof(esp_openthread_tx_queue_info_t));
    s_txrx_events = 0;

    esp_ieee802154_disable();
    esp_ieee802154_event_callback_list_unregister();
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
    bool start_pending_tx = false;

    if (get_event(EVENT_TX_DONE)) {
        clr_event(EVENT_TX_DONE);
        assert(s_tx_queue_info.used > 0);
        int8_t tx_idx = s_tx_queue[s_tx_queue_info.head].index;
        assert(tx_idx >= 0 && tx_idx < OT_INSTANCE_COUNT);
        otInstance *tx_instance = esp_openthread_get_instance_from_idx(tx_idx);

        if (s_ack_frame.mPsdu == NULL) {
            otPlatRadioTxDone(tx_instance, &s_transmit_frame[tx_idx], NULL, OT_ERROR_NONE);
        } else {
            otPlatRadioTxDone(tx_instance, &s_transmit_frame[tx_idx], &s_ack_frame, OT_ERROR_NONE);
            esp_ieee802154_receive_handle_done(s_ack_frame.mPsdu - 1);
            s_ack_frame.mPsdu = NULL;
        }
        radio_tx_queue_pop_head();
        start_pending_tx = true;
    }

    if (get_event(EVENT_TX_FAILED)) {
        clr_event(EVENT_TX_FAILED);
        assert(s_tx_queue_info.used > 0);
        int8_t tx_idx = s_tx_queue[s_tx_queue_info.head].index;
        assert(tx_idx >= 0 && tx_idx < OT_INSTANCE_COUNT);
        otInstance *tx_instance = esp_openthread_get_instance_from_idx(tx_idx);

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

        otPlatRadioTxDone(tx_instance, &s_transmit_frame[tx_idx], NULL, err);
        radio_tx_queue_pop_head();
        start_pending_tx = true;
    }

    // After in-flight TX finishes, start the next frame already in the queue
    // before processing RX, to reduce TX idle time.
    if (start_pending_tx && s_tx_queue_info.used > 0) {
        esp_openthread_radio_transmit_context_t *packet = &s_tx_queue[s_tx_queue_info.head];
        otInstance *instance = esp_openthread_get_instance_from_idx(packet->index);
        radio_start_transmit(instance, packet->aFrame, packet->index);
    }

    if (get_event(EVENT_ENERGY_DETECT_DONE)) {
        clr_event(EVENT_ENERGY_DETECT_DONE);
        otPlatRadioEnergyScanDone(aInstance, s_ed_power);
    }

    while (atomic_load(&s_recv_queue.used)) {
        if (s_receive_frame[s_recv_queue.head].frame.mPsdu != NULL) {
            if (s_receive_frame[s_recv_queue.head].index == ESP_IEEE802154_MULTIPAN_MAX) {
                // should check all used instances and receive packets for multicast packet.
                // TZ-2585
                for (int i = 0; i < OT_INSTANCE_COUNT; i++) {
                    otInstance *ins = esp_openthread_get_instance_from_idx(i);
                    otPlatRadioReceiveDone(ins, &s_receive_frame[s_recv_queue.head].frame, OT_ERROR_NONE);
                }
            } else {
                otInstance *ins = esp_openthread_get_instance_from_idx(s_receive_frame[s_recv_queue.head].index);
                otPlatRadioReceiveDone(ins, &s_receive_frame[s_recv_queue.head].frame, OT_ERROR_NONE);
            }
            esp_ieee802154_receive_handle_done(s_receive_frame[s_recv_queue.head].frame.mPsdu - 1);
            s_receive_frame[s_recv_queue.head].frame.mPsdu = NULL;
            s_recv_queue.head = (s_recv_queue.head + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
            atomic_fetch_sub(&s_recv_queue.used, 1);
        }
    }

    if (get_event(EVENT_SLEEP)) {
        clr_event(EVENT_SLEEP);
        esp_ieee802154_sleep();
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
    esp_ieee802154_multipan_index_t idx = (esp_ieee802154_multipan_index_t)esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_set_multipan_panid(idx, panid);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    esp_ieee802154_multipan_index_t idx = (esp_ieee802154_multipan_index_t)esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_set_multipan_extended_address(idx, aAddress->m8);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    esp_ieee802154_multipan_index_t idx = (esp_ieee802154_multipan_index_t)esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_set_multipan_short_address(idx, aAddress);
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
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_sleep(idx);

    return OT_ERROR_NONE;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_set_channel(aChannel);
    esp_ieee802154_multipan_receive(idx);

    return OT_ERROR_NONE;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);

    // One in-flight + pending entries share this queue; under normal OT scheduling
    // there is at most one outstanding TX per instance.
    assert(s_tx_queue_info.used < OT_INSTANCE_COUNT);

    bool was_idle = (s_tx_queue_info.used == 0);
    esp_openthread_radio_transmit_context_t *packet = &s_tx_queue[s_tx_queue_info.tail];
    packet->aFrame = aFrame;
    packet->index = idx;
    s_tx_queue_info.tail = (s_tx_queue_info.tail + 1) % OT_INSTANCE_COUNT;
    s_tx_queue_info.used++;

    if (was_idle) {
        radio_start_transmit(aInstance, aFrame, idx);
    }

    return OT_ERROR_NONE;
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    return &s_transmit_frame[idx];
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
    // Multipan RCP does not support receive_at yet.
    esp_ieee802154_set_channel(aChannel);
    esp_ieee802154_receive_at(aStart, aDuration);
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
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_add_pending_addr(idx, (uint8_t *)&aShortAddress, true);
    return OT_ERROR_NONE;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_add_pending_addr(idx, aExtAddress->m8, false);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, uint16_t aShortAddress)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_clear_pending_addr(idx, (uint8_t *)&aShortAddress, true);
    return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_clear_pending_addr(idx, aExtAddress->m8, false);
    return OT_ERROR_NONE;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_reset_pending_table(idx, true);
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_reset_pending_table(idx, false);
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    // TZ-2587
    esp_ieee802154_set_channel(aScanChannel);
    esp_ieee802154_energy_detect(aScanDuration * US_PER_MS / US_PER_SYMBOL);

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
    return esp_ieee802154_get_receive_sensitivity();
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
    OT_UNUSED_VARIABLE(aKeyIdMode);
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    assert(aKeyType == OT_KEY_TYPE_KEY_REF);
#else
    assert(aKeyType == OT_KEY_TYPE_LITERAL_KEY);
#endif // OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    assert(aPrevKey != NULL && aCurrKey != NULL && aNextKey != NULL);

    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);

    s_sec_ctx[idx].key_id = aKeyId;
    s_sec_ctx[idx].previous_key = *aPrevKey;
    s_sec_ctx[idx].current_key  = *aCurrKey;
    s_sec_ctx[idx].next_key     = *aNextKey;
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
    /* Pre-export raw key bytes in task context to avoid calling psa_export_key()
     * from ISR context (enh_ack_generator), which would attempt to take a mutex. */
    size_t keyLength = 0;
    psa_export_key(aPrevKey->mKeyMaterial.mKeyRef, s_sec_ctx[idx].previous_key_bytes, 16, &keyLength);
    psa_export_key(aCurrKey->mKeyMaterial.mKeyRef, s_sec_ctx[idx].current_key_bytes,  16, &keyLength);
    psa_export_key(aNextKey->mKeyMaterial.mKeyRef, s_sec_ctx[idx].next_key_bytes,     16, &keyLength);
#endif
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    s_sec_ctx[idx].mac_frame_counter = aMacFrameCounter;
}

void otPlatRadioSetMacFrameCounterIfLarger(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    if (aMacFrameCounter > s_sec_ctx[idx].mac_frame_counter) {
        s_sec_ctx[idx].mac_frame_counter = aMacFrameCounter;
    }
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

static IRAM_ATTR uint16_t get_csl_phase(void)
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

#endif

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    return ESP_OPENTHREAD_CSL_ACCURACY;
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    return ESP_OPENTHREAD_CSL_UNCERTAIN;
}

// events
static void IRAM_ATTR ot_radio_transmit_done(const uint8_t *frame, const uint8_t *ack,
                                             esp_ieee802154_frame_info_t *ack_frame_info)
{
    int8_t tx_idx = s_tx_queue[s_tx_queue_info.head].index;
    ETS_ASSERT(tx_idx >= 0 && tx_idx < OT_INSTANCE_COUNT);
    ETS_ASSERT(frame == (uint8_t *)&s_transmit_psdu[tx_idx]);

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
    radio_frame->mInfo.mRxInfo.mTimestamp = frame_info->timestamp;
}

static esp_err_t IRAM_ATTR enh_ack_set_security_addr_and_key(otRadioFrame *ack_frame, int8_t idx)
{
    struct otMacKeyMaterial *key = NULL;
    uint8_t key_id;

    ETS_ASSERT(otMacFrameIsSecurityEnabled(ack_frame));
    if (idx < 0 || idx >= OT_INSTANCE_COUNT) {
        return ESP_FAIL;
    }
    key_id = otMacFrameGetKeyId(ack_frame);
    if(!(otMacFrameIsKeyIdMode1(ack_frame) && key_id != 0)) {
        return ESP_FAIL;
    }

    if (key_id == s_sec_ctx[idx].key_id) {
        key = &s_sec_ctx[idx].current_key;
    } else if (key_id == s_sec_ctx[idx].key_id - 1) {
        key = &s_sec_ctx[idx].previous_key;
    } else if (key_id == s_sec_ctx[idx].key_id + 1) {
        key = &s_sec_ctx[idx].next_key;
    } else {
        return ESP_FAIL;
    }
    s_sec_ctx[idx].ack_frame_counter = s_sec_ctx[idx].mac_frame_counter;
    s_sec_ctx[idx].ack_key_id = key_id;
    s_sec_ctx[idx].with_security_enh_ack = true;
    if (otMacFrameIsKeyIdMode1(ack_frame)) {
#if CONFIG_IEEE802154_MULTI_PAN_ENABLE
        esp_ieee802154_get_multipan_extended_address((esp_ieee802154_multipan_index_t)idx, s_sec_ctx[idx].security_addr);
#else
        esp_ieee802154_get_extended_address(s_sec_ctx[idx].security_addr);
#endif
        ot_set_security_key_from_key_material(idx, *key);
    }

    esp_ieee802154_set_transmit_security(&ack_frame->mPsdu[-1], s_sec_ctx[idx].security_key,
                                         s_sec_ctx[idx].security_addr);
    return ESP_OK;
}

static esp_err_t IRAM_ATTR ot_radio_enh_ack_generator(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info,
                                                      uint8_t *enhack_frame)
{
    otRadioFrame ack_frame;
    otRadioFrame ot_frame;
    uint8_t ack_ie_data[OT_ACK_IE_MAX_SIZE] = {0};
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
    link_metrics_data_len = otLinkMetricsEnhAckGenData(&mac_addr, frame_info->lqi,
                                        frame_info->rssi, link_metrics_data);
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
        int8_t ack_idx = (int8_t)frame_info->mpf_index;
        if (ack_idx < 0 || ack_idx >= OT_INSTANCE_COUNT) {
            return ESP_FAIL;
        }
        otMacFrameSetFrameCounter(&ack_frame, s_sec_ctx[ack_idx].mac_frame_counter++);
        if (enh_ack_set_security_addr_and_key(&ack_frame, ack_idx) != ESP_OK) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

static void IRAM_ATTR ot_radio_receive_done(uint8_t *data, esp_ieee802154_frame_info_t *frame_info)
{
    otRadioFrame ot_frame;
    ot_frame.mPsdu = data + 1;
    int8_t rx_idx = (int8_t)frame_info->mpf_index;
    assert((rx_idx >= 0 && rx_idx < OT_INSTANCE_COUNT) || rx_idx == ESP_IEEE802154_MULTIPAN_MAX);

    if (atomic_load(&s_recv_queue.used) == CONFIG_IEEE802154_RX_BUFFER_SIZE) {
        ESP_EARLY_LOGE(OT_PLAT_LOG_TAG, "radio receive buffer full!");
        esp_ieee802154_receive_handle_done(data);
        return;
    }

    convert_to_ot_frame(data, frame_info, &(s_receive_frame[s_recv_queue.tail].frame));
    s_receive_frame[s_recv_queue.tail].index = rx_idx;
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    // Default to false for broadcast/unmatched frames or non-2015/non-AR data frames.
    s_receive_frame[s_recv_queue.tail].frame.mInfo.mRxInfo.mAckedWithSecEnhAck = false;
    if (rx_idx >= 0 && rx_idx < OT_INSTANCE_COUNT &&
        otMacFrameIsAckRequested(&ot_frame) && otMacFrameIsVersion2015(&ot_frame)) {
        s_receive_frame[s_recv_queue.tail].frame.mInfo.mRxInfo.mAckedWithSecEnhAck = s_sec_ctx[rx_idx].with_security_enh_ack;
        s_receive_frame[s_recv_queue.tail].frame.mInfo.mRxInfo.mAckFrameCounter = s_sec_ctx[rx_idx].ack_frame_counter;
        s_receive_frame[s_recv_queue.tail].frame.mInfo.mRxInfo.mAckKeyId = s_sec_ctx[rx_idx].ack_key_id;
    }
    if (rx_idx >= 0 && rx_idx < OT_INSTANCE_COUNT) {
        s_sec_ctx[rx_idx].with_security_enh_ack = false;
    }
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    s_recv_queue.tail = (s_recv_queue.tail + 1) % CONFIG_IEEE802154_RX_BUFFER_SIZE;
    atomic_fetch_add(&s_recv_queue.used, 1);
    set_event(EVENT_RX_DONE);
}

static void IRAM_ATTR ot_radio_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    int8_t tx_idx = s_tx_queue[s_tx_queue_info.head].index;
    ETS_ASSERT(tx_idx >= 0 && tx_idx < OT_INSTANCE_COUNT);
    ETS_ASSERT(frame == (uint8_t *)&s_transmit_psdu[tx_idx]);

    s_tx_error = error;

    set_event(EVENT_TX_FAILED);
}

static void IRAM_ATTR ot_radio_receive_sfd_done(void)
{
}

static void IRAM_ATTR ot_radio_transmit_sfd_done(uint8_t *frame)
{
    int8_t idx = -1;

    if (frame != s_enhack) {
        idx = s_tx_queue[s_tx_queue_info.head].index;
        assert(idx >= 0 && idx < OT_INSTANCE_COUNT);
        assert(frame == (uint8_t *)&s_transmit_psdu[idx]);
    }
#if !OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    OT_UNUSED_VARIABLE(idx);
#endif

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (s_csl_period > 0) {
        otRadioFrame ot_frame;
        ot_frame.mPsdu = frame + 1;
        ot_frame.mLength = frame[0];

        otMacFrameSetCslIe(&ot_frame, s_csl_period, get_csl_phase());
    }
#endif

#if OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    if ((idx >= 0) && s_transmit_frame[idx].mInfo.mTxInfo.mIeInfo->mTimeIeOffset != 0)
    {
        uint8_t *p_time_ie = s_transmit_frame[idx].mPsdu + s_transmit_frame[idx].mInfo.mTxInfo.mIeInfo->mTimeIeOffset;
        uint64_t time = (uint64_t)((int64_t)otPlatTimeGet() + s_transmit_frame[idx].mInfo.mTxInfo.mIeInfo->mNetworkTimeOffset);

        *p_time_ie = s_transmit_frame[idx].mInfo.mTxInfo.mIeInfo->mTimeSyncSeq;

        *(++p_time_ie) = (uint8_t)(time & 0xff);
        for (uint8_t i = 1; i < sizeof(uint64_t); i++)
        {
            time           = time >> 8;
            *(++p_time_ie) = (uint8_t)(time & 0xff);
        }
    }
#endif // OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
}

static void IRAM_ATTR ot_radio_energy_detect_done(int8_t power)
{
    s_ed_power = power;

    set_event(EVENT_ENERGY_DETECT_DONE);
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
    int8_t idx = esp_openthread_get_idx_from_instance(aInstance);
    esp_ieee802154_multipan_set_rx_when_idle(idx, aEnable);
}
#endif

uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return CONFIG_OPENTHREAD_PREFERRED_CHANNEL_MASK;
}

uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return CONFIG_OPENTHREAD_SUPPORTED_CHANNEL_MASK;
}

#if (CONFIG_ESP_COEX_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE)
void esp_openthread_set_coex_config(esp_ieee802154_coex_config_t config)
{
    esp_ieee802154_set_coex_config(config);
}

esp_ieee802154_coex_config_t esp_openthread_get_coex_config(void)
{
    return esp_ieee802154_get_coex_config();
}
#endif

void esp_ieee802154_receive_at_done(void)
{
    set_event(EVENT_SLEEP);
}

otError otPlatMultipanGetActiveInstance(otInstance **aInstance)
{
    *aInstance = esp_openthread_get_instance();
    return OT_ERROR_NONE;
}

otError otPlatMultipanSetActiveInstance(otInstance *aInstance, bool aCompletePending)
{
    OT_UNUSED_VARIABLE(aCompletePending);
    esp_openthread_set_active_instance(aInstance);
    return OT_ERROR_NONE;
}
