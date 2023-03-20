/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "freertos/portmacro.h"
#include "soc/periph_defs.h"
#include "soc/soc.h"
#include "soc/ieee802154_periph.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_check.h"
#include "esp_coex_i154.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_ieee802154_ack.h"
#include "esp_ieee802154_dev.h"
#include "esp_ieee802154_frame.h"
#include "esp_ieee802154_pib.h"
#include "esp_ieee802154_sec.h"
#include "esp_ieee802154_util.h"
#include "esp_ieee802154_timer.h"
#include "hal/ieee802154_ll.h"
#include "esp_attr.h"

#define CCA_DETECTION_TIME 8

extern void bt_bb_set_zb_tx_on_delay(uint16_t time);

static volatile ieee802154_state_t s_ieee802154_state;
static uint8_t *s_tx_frame = NULL;
static uint8_t s_rx_frame[CONFIG_IEEE802154_RX_BUFFER_SIZE][127 + 1 + 1]; // +1: len, +1: for dma test
static esp_ieee802154_frame_info_t s_rx_frame_info[CONFIG_IEEE802154_RX_BUFFER_SIZE];
static uint8_t s_rx_index = 0;
static uint8_t s_enh_ack_frame[128];
static uint8_t s_recent_rx_frame_info_index;
static portMUX_TYPE s_ieee802154_spinlock = portMUX_INITIALIZER_UNLOCKED;

static IRAM_ATTR void event_end_process(void)
{
    ieee802154_etm_channel_clear(IEEE802154_ETM_CHANNEL0);
    ieee802154_etm_channel_clear(IEEE802154_ETM_CHANNEL1);
    ieee802154_ll_set_transmit_security(false);
    ieee802154_timer0_stop();
}

static IRAM_ATTR void receive_ack_timeout_timer_start(uint32_t duration)
{
    ieee802154_ll_enable_events(IEEE802154_EVENT_TIMER0_OVERFLOW);
    ieee802154_timer0_set_threshold(duration);
    ieee802154_timer0_start();
}

static void ieee802154_rx_frame_info_update(void)
{
    uint8_t len = s_rx_frame[s_rx_index][0];
    int8_t rssi = s_rx_frame[s_rx_index][len - 1]; // crc is not written to rx buffer
    uint8_t lqi = s_rx_frame[s_rx_index][len];

    s_rx_frame_info[s_rx_index].channel = ieee802154_freq_to_channel(ieee802154_ll_get_freq());
    s_rx_frame_info[s_rx_index].rssi = rssi;
    s_rx_frame_info[s_rx_index].lqi = lqi;

    s_recent_rx_frame_info_index = s_rx_index;
}

int8_t ieee802154_get_recent_rssi(void)
{
    return s_rx_frame_info[s_recent_rx_frame_info_index].rssi;
}

uint8_t ieee802154_get_recent_lqi(void)
{
    return s_rx_frame_info[s_recent_rx_frame_info_index].lqi;
}

static void set_next_rx_buffer(void)
{
    if (s_rx_frame[s_rx_index][0] != 0) {
        s_rx_index++;
        if (s_rx_index == CONFIG_IEEE802154_RX_BUFFER_SIZE) {
            s_rx_index = 0;
        }
        memset(s_rx_frame[s_rx_index], 0, sizeof(s_rx_frame[s_rx_index]));
    }

    ieee802154_ll_set_rx_addr((uint8_t *)&s_rx_frame[s_rx_index]);
}

static bool stop_rx(void)
{
    ieee802154_ll_events events;

    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);

    events = ieee802154_ll_get_events();
    if (events & IEEE802154_EVENT_RX_DONE) {
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
    }

    ieee802154_ll_clear_events(IEEE802154_EVENT_RX_DONE | IEEE802154_EVENT_RX_ABORT | IEEE802154_EVENT_RX_SFD_DONE);

    return true;
}

static bool stop_tx_ack(void)
{
    ieee802154_ll_events events;

    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);

    events = ieee802154_ll_get_events();
    if (events & IEEE802154_EVENT_ACK_TX_DONE) {
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
    }

    ieee802154_ll_clear_events(IEEE802154_EVENT_ACK_TX_DONE | IEEE802154_EVENT_RX_ABORT | IEEE802154_EVENT_TX_SFD_DONE); // ZB-81: clear TX_SFD_DONE event

    return true;
}

static bool stop_tx(void)
{
    ieee802154_ll_events events;

    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);

    events = ieee802154_ll_get_events();

    if (s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK) {
        // if current operation is sending 2015 Enh-ack, SW should create the receive-done event.
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
        ieee802154_ll_clear_events(IEEE802154_EVENT_ACK_TX_DONE);
    } else if ((events & IEEE802154_EVENT_TX_DONE) && (!ieee802154_frame_is_ack_required(s_tx_frame) || !ieee802154_ll_get_rx_auto_ack())) {
        // if the tx is already done, and the frame is not ack request OR auto ack rx is disabled.
        esp_ieee802154_transmit_done(s_tx_frame, NULL, NULL);
    } else {
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_ABORT);
    }

    ieee802154_ll_clear_events(IEEE802154_EVENT_TX_DONE | IEEE802154_EVENT_TX_ABORT | IEEE802154_EVENT_TX_SFD_DONE);

    return true;
}

static bool stop_cca(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);
    ieee802154_ll_clear_events(IEEE802154_EVENT_ED_DONE | IEEE802154_EVENT_RX_ABORT);
    return true;
}

static bool stop_tx_cca(void)
{
    stop_tx(); // in case the transmission already started
    ieee802154_ll_clear_events(IEEE802154_EVENT_TX_ABORT);
    return true;
}

static bool stop_rx_ack(void)
{
    ieee802154_ll_events events;

    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);

    events = ieee802154_ll_get_events();

    ieee802154_timer0_stop();
    ieee802154_ll_disable_events(IEEE802154_EVENT_TIMER0_OVERFLOW);

    if (events & IEEE802154_EVENT_TIMER0_OVERFLOW) {
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_NO_ACK);
    }

    if (events & IEEE802154_EVENT_ACK_RX_DONE) {
        esp_ieee802154_transmit_done(s_tx_frame, (uint8_t *)&s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
    }

    ieee802154_ll_clear_events(IEEE802154_EVENT_ACK_RX_DONE | IEEE802154_EVENT_RX_SFD_DONE | IEEE802154_EVENT_TX_ABORT);

    return true;
}

static bool stop_ed(void)
{
    ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);

    ieee802154_ll_clear_events(IEEE802154_EVENT_ED_DONE | IEEE802154_EVENT_RX_ABORT);

    return true;
}

static bool stop_current_operation(void)
{
    event_end_process();
    switch (s_ieee802154_state) {
    case IEEE802154_STATE_DISABLE:
        break;

    case IEEE802154_STATE_IDLE:
        // do nothing
        break;

    case IEEE802154_STATE_RX:
        stop_rx();
        break;

    case IEEE802154_STATE_TX_ACK:
        stop_tx_ack();
        break;

    case IEEE802154_STATE_TX_CCA:
        stop_tx_cca();
        break;
    case IEEE802154_STATE_CCA:
        stop_cca();
        break;

    case IEEE802154_STATE_TX:
    case IEEE802154_STATE_TX_ENH_ACK:
        stop_tx();
        break;

    case IEEE802154_STATE_RX_ACK:
        stop_rx_ack();
        break;

    case IEEE802154_STATE_ED:
        stop_ed();
        break;

    default:
        assert(false);
        break;
    }

    return true;
}

static void enable_rx(void)
{
    set_next_rx_buffer();
    IEEE802154_SET_TXRX_PTI(IEEE802154_SCENE_RX);

    ieee802154_ll_set_cmd(IEEE802154_CMD_RX_START);

    s_ieee802154_state = IEEE802154_STATE_RX;
}

static IRAM_ATTR void next_operation(void)
{
    if (ieee802154_pib_get_rx_when_idle()) {
        enable_rx();
    } else {
        s_ieee802154_state = IEEE802154_STATE_IDLE;
    }
}

static void isr_handle_timer0_done(void)
{
    if (s_ieee802154_state == IEEE802154_STATE_RX_ACK) {
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_NO_ACK);
        next_operation();
    }
}

static void isr_handle_timer1_done(void)
{
    // timer 1 is now unused.
}

static IRAM_ATTR void isr_handle_tx_done(void)
{
    event_end_process();
    if (s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK) {
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
        next_operation();
    } else {
        if (s_ieee802154_state == IEEE802154_STATE_TEST_TX) {
            esp_ieee802154_transmit_done(s_tx_frame, NULL, NULL);
            next_operation();
        } else if (s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA) {
            if (ieee802154_frame_is_ack_required(s_tx_frame) && ieee802154_ll_get_rx_auto_ack()) {
                s_ieee802154_state = IEEE802154_STATE_RX_ACK;
                receive_ack_timeout_timer_start(200000); // 200ms for receive ack timeout
            } else {
                esp_ieee802154_transmit_done(s_tx_frame, NULL, NULL);
                next_operation();
            }
        }
    }
}

static IRAM_ATTR void isr_handle_rx_done(void)
{
    event_end_process();
    ieee802154_rx_frame_info_update();

    if (s_ieee802154_state == IEEE802154_STATE_RX) {
        if (ieee802154_frame_is_ack_required(s_rx_frame[s_rx_index]) && ieee802154_frame_get_version(s_rx_frame[s_rx_index]) <= IEEE802154_FRAME_VERSION_1
                && ieee802154_ll_get_tx_auto_ack()) {
            // auto tx ack only works for the frame with version 0b00 and 0b01
            s_rx_frame_info[s_rx_index].pending = ieee802154_ack_config_pending_bit(s_rx_frame[s_rx_index]);
            s_ieee802154_state = IEEE802154_STATE_TX_ACK;
        } else if (ieee802154_frame_is_ack_required(s_rx_frame[s_rx_index]) && ieee802154_frame_get_version(s_rx_frame[s_rx_index]) == IEEE802154_FRAME_VERSION_2
                   && ieee802154_ll_get_tx_enhance_ack()) {
            s_rx_frame_info[s_rx_index].pending = ieee802154_ack_config_pending_bit(s_rx_frame[s_rx_index]);
            // For 2015 enh-ack, SW should generate an enh-ack then send it manually
            if (esp_ieee802154_enh_ack_generator(s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index], s_enh_ack_frame) == ESP_OK) {
                // Send the Enh-Ack frame if generator succeeds.
                ieee802154_ll_set_tx_addr(s_enh_ack_frame);
                s_tx_frame = s_enh_ack_frame;
                ieee802154_sec_update();
                ieee802154_ll_enhack_generate_done_notify();
                s_ieee802154_state = IEEE802154_STATE_TX_ENH_ACK;
            } else {
                // Stop current process if generator returns errors.
                ieee802154_ll_set_cmd(IEEE802154_CMD_STOP);
                esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
                next_operation();
            }
        } else {
            esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
            next_operation();
        }
    }
}

static IRAM_ATTR void isr_handle_ack_tx_done(void)
{
    esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
    next_operation();
}

static IRAM_ATTR void isr_handle_ack_rx_done(void)
{
    ieee802154_timer0_stop();
    ieee802154_ll_disable_events(IEEE802154_EVENT_TIMER0_OVERFLOW);
    ieee802154_rx_frame_info_update();
    esp_ieee802154_transmit_done(s_tx_frame, (uint8_t *)&s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
    next_operation();
}

static IRAM_ATTR void isr_handle_rx_abort(void)
{
    event_end_process();
    uint32_t rx_status = ieee802154_ll_get_rx_status();
    ieee802154_ll_rx_abort_reason_t rx_abort_reason = ieee802154_ll_get_rx_abort_reason();
    switch (rx_abort_reason) {
    case IEEE802154_RX_ABORT_BY_RX_STOP:
    case IEEE802154_RX_ABORT_BY_TX_ACK_STOP:
    case IEEE802154_RX_ABORT_BY_ED_STOP:
        // do nothing
        break;
    case IEEE802154_RX_ABORT_BY_SFD_TIMEOUT:
    case IEEE802154_RX_ABORT_BY_CRC_ERROR:
    case IEEE802154_RX_ABORT_BY_INVALID_LEN:
    case IEEE802154_RX_ABORT_BY_FILTER_FAIL:
    case IEEE802154_RX_ABORT_BY_NO_RSS:
    case IEEE802154_RX_ABORT_BY_UNEXPECTED_ACK:
    case IEEE802154_RX_ABORT_BY_RX_RESTART:
        assert(s_ieee802154_state == IEEE802154_STATE_RX);
        break;
    case IEEE802154_RX_ABORT_BY_COEX_BREAK:
        assert(s_ieee802154_state == IEEE802154_STATE_RX);
        esp_ieee802154_receive_failed(rx_status);
        break;
    case IEEE802154_RX_ABORT_BY_ED_ABORT:
    case IEEE802154_RX_ABORT_BY_ED_COEX_REJECT:
        assert(s_ieee802154_state == IEEE802154_STATE_ED || s_ieee802154_state == IEEE802154_STATE_CCA);
        esp_ieee802154_ed_failed(rx_status);
        next_operation();
        break;
    case IEEE802154_RX_ABORT_BY_TX_ACK_TIMEOUT:
    case IEEE802154_RX_ABORT_BY_TX_ACK_COEX_BREAK:
        assert(s_ieee802154_state == IEEE802154_STATE_TX_ACK || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
        next_operation();
        break;
    case IEEE802154_RX_ABORT_BY_ENHACK_SECURITY_ERROR:
        assert(s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);
        esp_ieee802154_receive_done((uint8_t *)s_rx_frame[s_rx_index], &s_rx_frame_info[s_rx_index]);
        next_operation();
        break;
    default:
        assert(false);
    }
}

static IRAM_ATTR void isr_handle_tx_abort(void)
{
    event_end_process();
    ieee802154_ll_tx_abort_reason_t tx_abort_reason = ieee802154_ll_get_tx_abort_reason();
    switch (tx_abort_reason) {
    case IEEE802154_TX_ABORT_BY_RX_ACK_STOP:
    case IEEE802154_TX_ABORT_BY_TX_STOP:
        // do nothing
        break;
    case IEEE802154_TX_ABORT_BY_RX_ACK_SFD_TIMEOUT:
    case IEEE802154_TX_ABORT_BY_RX_ACK_CRC_ERROR:
    case IEEE802154_TX_ABORT_BY_RX_ACK_INVALID_LEN:
    case IEEE802154_TX_ABORT_BY_RX_ACK_FILTER_FAIL:
    case IEEE802154_TX_ABORT_BY_RX_ACK_NO_RSS:
    case IEEE802154_TX_ABORT_BY_RX_ACK_COEX_BREAK:
    case IEEE802154_TX_ABORT_BY_RX_ACK_TYPE_NOT_ACK:
    case IEEE802154_TX_ABORT_BY_RX_ACK_RESTART:
        assert(s_ieee802154_state == IEEE802154_STATE_RX_ACK);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_INVALID_ACK);
        break;
    case IEEE802154_TX_ABORT_BY_RX_ACK_TIMEOUT:
        assert(s_ieee802154_state == IEEE802154_STATE_RX_ACK);
        ieee802154_ll_disable_events(IEEE802154_EVENT_TIMER0_OVERFLOW);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_NO_ACK);
        next_operation();
        break;
    case IEEE802154_TX_ABORT_BY_TX_COEX_BREAK:
        assert(s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_COEXIST);
        next_operation();
        break;
    case IEEE802154_TX_ABORT_BY_TX_SECURITY_ERROR:
        assert(s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_SECURITY);
        next_operation();
        break;
    case IEEE802154_TX_ABORT_BY_CCA_FAILED:
        assert(s_ieee802154_state == IEEE802154_STATE_TX_CCA);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_ABORT);
        next_operation();
        break;
    case IEEE802154_TX_ABORT_BY_CCA_BUSY:
        assert(s_ieee802154_state == IEEE802154_STATE_TX_CCA);
        esp_ieee802154_transmit_failed(s_tx_frame, ESP_IEEE802154_TX_ERR_CCA_BUSY);
        next_operation();
        break;
    default:
        assert(false);
        break;
    }
}

static IRAM_ATTR void isr_handle_ed_done(void)
{
    if (s_ieee802154_state == IEEE802154_STATE_CCA) {
        esp_ieee802154_cca_done(ieee802154_ll_is_cca_busy());
    } else if (s_ieee802154_state == IEEE802154_STATE_ED) {
        esp_ieee802154_energy_detect_done(ieee802154_ll_get_ed_rss());
    }

    next_operation();
}

static void ieee802154_isr(void *arg)
{
    ieee802154_ll_events events = ieee802154_ll_get_events();

    ieee802154_ll_clear_events(events);

    if (events & IEEE802154_EVENT_RX_SFD_DONE) {
        // IEEE802154_STATE_TX && IEEE802154_STATE_TX_CCA && IEEE802154_STATE_TX_ENH_ACK for isr processing delay
        assert(s_ieee802154_state == IEEE802154_STATE_RX || s_ieee802154_state == IEEE802154_STATE_RX_ACK || s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);

        s_rx_frame_info[s_rx_index].timestamp = esp_timer_get_time();
        esp_ieee802154_receive_sfd_done();

        events &= (uint16_t)(~IEEE802154_EVENT_RX_SFD_DONE);
    }

    if (events & IEEE802154_EVENT_TX_SFD_DONE) {
        // ZB-81: IEEE802154_STATE_TX_ACK is also a possible state
        assert(s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA || s_ieee802154_state == IEEE802154_STATE_TEST_TX || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK || s_ieee802154_state == IEEE802154_STATE_TX_ACK);

        esp_ieee802154_transmit_sfd_done(s_tx_frame);

        events &= (uint16_t)(~IEEE802154_EVENT_TX_SFD_DONE);
    }

    if (events & IEEE802154_EVENT_TX_DONE) {
        assert(s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA || s_ieee802154_state == IEEE802154_STATE_TEST_TX || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);

        isr_handle_tx_done();

        events &= (uint16_t)(~IEEE802154_EVENT_TX_DONE);
    }

    if (events & IEEE802154_EVENT_RX_DONE) {
        assert(s_ieee802154_state == IEEE802154_STATE_RX);

        isr_handle_rx_done();

        events &= (uint16_t)(~IEEE802154_EVENT_RX_DONE);
    }

    if (events & IEEE802154_EVENT_ACK_TX_DONE) {
        // IEEE802154_STATE_RX for isr processing delay
        assert(s_ieee802154_state == IEEE802154_STATE_TX_ACK || s_ieee802154_state == IEEE802154_STATE_RX || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);

        isr_handle_ack_tx_done();

        events &= (uint16_t)(~IEEE802154_EVENT_ACK_TX_DONE);
    }

    if (events & IEEE802154_EVENT_ACK_RX_DONE) {
        // IEEE802154_STATE_TX && IEEE802154_STATE_TX_CCA && IEEE802154_STATE_TX_ENH_ACK for isr processing delay
        assert(s_ieee802154_state == IEEE802154_STATE_RX_ACK || s_ieee802154_state == IEEE802154_STATE_TX || s_ieee802154_state == IEEE802154_STATE_TX_CCA || s_ieee802154_state == IEEE802154_STATE_TX_ENH_ACK);

        isr_handle_ack_rx_done();

        events &= (uint16_t)(~IEEE802154_EVENT_ACK_RX_DONE);
    }

    if (events & IEEE802154_EVENT_RX_ABORT) {
        isr_handle_rx_abort();

        events &= (uint16_t)(~IEEE802154_EVENT_RX_ABORT);
    }

    if (events & IEEE802154_EVENT_TX_ABORT) {
        isr_handle_tx_abort();

        events &= (uint16_t)(~IEEE802154_EVENT_TX_ABORT);
    }

    if (events & IEEE802154_EVENT_ED_DONE) {
        assert(s_ieee802154_state == IEEE802154_STATE_ED || s_ieee802154_state == IEEE802154_STATE_CCA);

        isr_handle_ed_done();

        events &= (uint16_t)(~IEEE802154_EVENT_ED_DONE);
    }

    if (events & IEEE802154_EVENT_TIMER0_OVERFLOW) {
        assert(s_ieee802154_state == IEEE802154_STATE_RX_ACK);
        isr_handle_timer0_done();

        events &= (uint16_t)(~IEEE802154_EVENT_TIMER0_OVERFLOW);
    }

    if (events & IEEE802154_EVENT_TIMER1_OVERFLOW) {
        isr_handle_timer1_done();

        events &= (uint16_t)(~IEEE802154_EVENT_TIMER1_OVERFLOW);
    }

    // all events should be handled
    assert(events == 0);

}

static IRAM_ATTR void ieee802154_enter_critical(void)
{
    portENTER_CRITICAL(&s_ieee802154_spinlock);
}

static IRAM_ATTR void ieee802154_exit_critical(void)
{
    portEXIT_CRITICAL(&s_ieee802154_spinlock);
}

void ieee802154_enable(void)
{
    modem_clock_module_enable(ieee802154_periph.module);
}

void ieee802154_disable(void)
{
    s_ieee802154_state = IEEE802154_STATE_DISABLE;
}

esp_err_t ieee802154_mac_init(void)
{
    esp_err_t ret = ESP_OK;
    ieee802154_pib_init();

    ieee802154_ll_enable_events(IEEE802154_EVENT_MASK);
    ieee802154_ll_disable_events((IEEE802154_EVENT_TIMER0_OVERFLOW) | (IEEE802154_EVENT_TIMER1_OVERFLOW));

    ieee802154_ll_enable_tx_abort_events(BIT(IEEE802154_TX_ABORT_BY_RX_ACK_TIMEOUT - 1) | BIT(IEEE802154_TX_ABORT_BY_TX_COEX_BREAK - 1) | BIT(IEEE802154_TX_ABORT_BY_TX_SECURITY_ERROR - 1) | BIT(IEEE802154_TX_ABORT_BY_CCA_FAILED - 1) | BIT(IEEE802154_TX_ABORT_BY_CCA_BUSY - 1));
    ieee802154_ll_enable_rx_abort_events(BIT(IEEE802154_RX_ABORT_BY_TX_ACK_TIMEOUT - 1) | BIT(IEEE802154_RX_ABORT_BY_TX_ACK_COEX_BREAK - 1));

    ieee802154_ll_set_ed_sample_mode(IEEE802154_ED_SAMPLE_AVG);
#if CONFIG_ESP_COEX_SW_COEXIST_ENABLE
    esp_coex_ieee802154_ack_pti_set(IEEE802154_MIDDLE);
    IEEE802154_SET_TXRX_PTI(IEEE802154_SCENE_IDLE);
#else
    ieee802154_ll_disable_coex();
#endif

#if CONFIG_IDF_ENV_FPGA
    bt_bb_set_zb_tx_on_delay(80);
#else
    bt_bb_set_zb_tx_on_delay(50);
    REG_WRITE(IEEE802154_RXON_DELAY_REG, 50);
#endif

    memset(s_rx_frame, 0, sizeof(s_rx_frame));
    s_ieee802154_state = IEEE802154_STATE_IDLE;

    // TODO: Add flags for IEEE802154 ISR allocating. TZ-102
    ret = esp_intr_alloc(ieee802154_periph.irq_id, 0, ieee802154_isr, NULL, NULL);
    ESP_RETURN_ON_FALSE(ret == ESP_OK, ESP_FAIL, IEEE802154_TAG, "IEEE802154 MAC init failed");

    return ret;
}

static void start_ed(uint32_t duration)
{
    ieee802154_ll_enable_events(IEEE802154_EVENT_ED_DONE);
    ieee802154_ll_set_ed_duration(duration);
    ieee802154_ll_set_cmd(IEEE802154_CMD_ED_START);
}

static void tx_init(const uint8_t *frame)
{
    s_tx_frame = (uint8_t *)frame;
    stop_current_operation();
    ieee802154_pib_update();
    ieee802154_sec_update();

    ieee802154_ll_set_tx_addr(s_tx_frame);

    if (ieee802154_frame_is_ack_required(frame)) {
        // set rx pointer for ack frame
        set_next_rx_buffer();
    }
}

esp_err_t ieee802154_transmit(const uint8_t *frame, bool cca)
{
    ieee802154_enter_critical();
    tx_init(frame);

    IEEE802154_SET_TXRX_PTI(IEEE802154_SCENE_TX);

    if (cca) {
        ieee802154_ll_set_cmd(IEEE802154_CMD_CCA_TX_START);
        s_ieee802154_state = IEEE802154_STATE_TX_CCA;
    } else {
        ieee802154_ll_set_cmd(IEEE802154_CMD_TX_START);
        s_ieee802154_state = IEEE802154_STATE_TX;
    }

    ieee802154_exit_critical();

    return ESP_OK;
}

static inline bool is_target_time_expired(uint32_t target, uint32_t now)
{
    return (((now - target) & (1 << 31)) == 0);
}

esp_err_t ieee802154_transmit_at(const uint8_t *frame, bool cca, uint32_t time)
{
    uint32_t tx_target_time;
    uint32_t current_time;
    tx_init(frame);
    IEEE802154_SET_TXRX_PTI(IEEE802154_SCENE_TX_AT);
    if (cca) {
        tx_target_time = time - IEEE802154_ED_TRIG_TX_RAMPUP_TIME_US;
        s_ieee802154_state = IEEE802154_STATE_TX_CCA;
        ieee802154_enter_critical();
        ieee802154_etm_set_event_task(IEEE802154_ETM_CHANNEL0, ETM_EVENT_TIMER0_OVERFLOW, ETM_TASK_ED_TRIG_TX);
        current_time = (uint32_t)esp_timer_get_time();
        ieee802154_timer0_set_threshold((is_target_time_expired(tx_target_time, current_time) ? 0 : (tx_target_time - current_time))); //uint: 1us
        ieee802154_timer0_start();
        ieee802154_exit_critical();
    } else {
        tx_target_time = time - IEEE802154_TX_RAMPUP_TIME_US;
        if (ieee802154_frame_get_type(frame) == IEEE802154_FRAME_TYPE_ACK && ieee802154_frame_get_version(frame) == IEEE802154_FRAME_VERSION_2) {
            s_ieee802154_state = IEEE802154_STATE_TX_ENH_ACK;
        } else {
            s_ieee802154_state = IEEE802154_STATE_TX;
        }
        ieee802154_enter_critical();
        ieee802154_etm_set_event_task(IEEE802154_ETM_CHANNEL0, ETM_EVENT_TIMER0_OVERFLOW, ETM_TASK_TX_START);
        current_time = (uint32_t)esp_timer_get_time();
        ieee802154_timer0_set_threshold((is_target_time_expired(tx_target_time, current_time) ? 0 : (tx_target_time - current_time))); //uint: 1us
        ieee802154_timer0_start();
        ieee802154_exit_critical();
    }

    return ESP_OK;
}

static void rx_init(void)
{
    stop_current_operation();
    ieee802154_pib_update();
}

esp_err_t ieee802154_receive(void)
{
    if (((s_ieee802154_state == IEEE802154_STATE_RX) || (s_ieee802154_state == IEEE802154_STATE_TX_ACK)) && (!ieee802154_pib_is_pending())) {
        // already in rx state, don't abort current rx operation
        return ESP_OK;
    }

    ieee802154_enter_critical();
    rx_init();
    enable_rx();
    ieee802154_exit_critical();
    return ESP_OK;
}

esp_err_t ieee802154_receive_at(uint32_t time)
{
    uint32_t rx_target_time = time - IEEE802154_RX_RAMPUP_TIME_US;
    uint32_t current_time;

    rx_init();
    IEEE802154_SET_TXRX_PTI(IEEE802154_SCENE_RX_AT);
    set_next_rx_buffer();
    s_ieee802154_state = IEEE802154_STATE_RX;
    ieee802154_enter_critical();
    ieee802154_etm_set_event_task(IEEE802154_ETM_CHANNEL1, ETM_EVENT_TIMER0_OVERFLOW, ETM_TASK_RX_START);
    current_time = (uint32_t)esp_timer_get_time();
    ieee802154_timer0_set_threshold((is_target_time_expired(rx_target_time, current_time) ? 0 : (rx_target_time - current_time))); //uint: 1us
    ieee802154_timer0_start();
    ieee802154_exit_critical();
    return ESP_OK;
}

esp_err_t ieee802154_sleep(void)
{
    ieee802154_enter_critical();

    stop_current_operation();
    s_ieee802154_state = IEEE802154_STATE_IDLE;

    ieee802154_exit_critical();
    return ESP_OK;
}

esp_err_t ieee802154_energy_detect(uint32_t duration)
{
    ieee802154_enter_critical();

    stop_current_operation();

    ieee802154_pib_update();

    start_ed(duration);
    s_ieee802154_state = IEEE802154_STATE_ED;

    ieee802154_exit_critical();
    return ESP_OK;
}

esp_err_t ieee802154_cca(void)
{
    ieee802154_enter_critical();

    stop_current_operation();

    ieee802154_pib_update();

    start_ed(CCA_DETECTION_TIME);
    s_ieee802154_state = IEEE802154_STATE_CCA;

    ieee802154_exit_critical();
    return ESP_OK;
}

ieee802154_state_t ieee802154_get_state(void)
{
    return s_ieee802154_state;
}
