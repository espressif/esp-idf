/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "hal/ieee802154_ll.h"
#include "esp_ieee802154_util.h"
#include "esp_log.h"

#if CONFIG_IEEE802154_DEBUG
ieee802154_probe_info_t g_ieee802154_probe;
#define TAG "ieee802154_debug"

#if CONFIG_IEEE802154_RECORD_EVENT
static char *ieee802154_get_event_string(ieee802154_ll_event_t events)
{
    char *event_string = "";
    switch (events) {
    case IEEE802154_EVENT_TX_DONE:
        event_string = "TX_DONE";
        break;
    case IEEE802154_EVENT_RX_DONE:
        event_string = "RX_DONE";
        break;
    case IEEE802154_EVENT_ACK_TX_DONE:
        event_string = "ACK_TX_DONE";
        break;
    case IEEE802154_EVENT_ACK_RX_DONE:
        event_string = "ACK_RX_DONE";
        break;
    case IEEE802154_EVENT_RX_ABORT:
        event_string = "RX_ABORT";
        break;
    case IEEE802154_EVENT_TX_ABORT:
        event_string = "TX_ABORT";
        break;
    case IEEE802154_EVENT_ED_DONE:
        event_string = "ED_DONE";
        break;
    case IEEE802154_EVENT_TIMER0_OVERFLOW:
        event_string = "TIMER0_OVERFLOW";
        break;
    case IEEE802154_EVENT_TIMER1_OVERFLOW:
        event_string = "TIMER1_OVERFLOW";
        break;
    case IEEE802154_EVENT_CLOCK_COUNT_MATCH:
        event_string = "CLOCK_COUNT_MATCH";
        break;
    case IEEE802154_EVENT_TX_SFD_DONE:
        event_string = "TX_SFD_DONE";
        break;
    case IEEE802154_EVENT_RX_SFD_DONE:
        event_string = "RX_SFD_DONE";
        break;
    default:
        event_string = "Multi events";
        break;
    }
    return event_string;
}
#endif // CONFIG_IEEE802154_RECORD_EVENT

#if CONFIG_IEEE802154_RECORD_STATE || CONFIG_IEEE802154_RECORD_EVENT
static char *ieee802154_state_string[] = {
    "DISABLE",
    "IDLE",
    "SLEEP",
    "RX",
    "TX_ACK",
    "TX_ENH_ACK",
    "TX_CCA",
    "TX",
    "DTM_TX",
    "RX_ACK",
    "ED",
    "CCA",
};
#endif // CONFIG_IEEE802154_RECORD_STATE

#if CONFIG_IEEE802154_RECORD_CMD
static char *ieee802154_get_cmd_string(ieee802154_ll_cmd_t cmd)
{
    char *cmd_string = "";
    switch (cmd) {
    case IEEE802154_CMD_TX_START:
        cmd_string = "tx";
        break;
    case IEEE802154_CMD_RX_START:
        cmd_string = "rx";
        break;
    case IEEE802154_CMD_CCA_TX_START:
        cmd_string = "tx cca";
        break;
    case IEEE802154_CMD_ED_START:
        cmd_string = "ed";
        break;
    case IEEE802154_CMD_STOP:
        cmd_string = "stop";
        break;
    case IEEE802154_CMD_TEST_TX_START:
        cmd_string = "test tx";
        break;
    case IEEE802154_CMD_TEST_RX_START:
        cmd_string = "test rx";
        break;
    case IEEE802154_CMD_TEST_STOP:
        cmd_string = "test stop";
        break;
    case IEEE802154_CMD_TIMER0_START:
        cmd_string = "timer0 start";
        break;
    case IEEE802154_CMD_TIMER0_STOP:
        cmd_string = "timer0 stop";
        break;
    case IEEE802154_CMD_TIMER1_START:
        cmd_string = "timer1 start";
        break;
    case IEEE802154_CMD_TIMER1_STOP:
        cmd_string = "timer1 stop";
        break;
    }
    return cmd_string;
}
#endif // CONFIG_IEEE802154_RECORD_CMD

#if CONFIG_IEEE802154_RECORD_EVENT || CONFIG_IEEE802154_RECORD_ABORT
static char *ieee80154_rx_abort_reason_string[] = {
    "RSVD",                                         //   = 0,
    "RX_STOP",                                      //   = 1,
    "SFD_TIMEOUT",                                  //   = 2,
    "CRC_ERROR  ",                                  //   = 3,
    "INVALID_LEN",                                  //   = 4,
    "FILTER_FAIL",                                  //   = 5,
    "NO_RSS     ",                                  //   = 6,
    "COEX_BREAK ",                                  //   = 7,
    "UNEXPECTED_ACK",                               //   = 8,
    "RX_RESTART",                                   //   = 9,
    "RSVD", "RSVD", "RSVD", "RSVD", "RSVD", "RSVD", //   = 10~15,
    "TX_ACK_TIMEOUT",                               //   = 16,
    "TX_ACK_STOP",                                  //   = 17,
    "TX_ACK_COEX_BREAK",                            //   = 18,
    "ENHACK_SECURITY_ERROR",                        //   = 19,
    "RSVD", "RSVD", "RSVD", "RSVD",                 //   = 20~23
    "ED_ABORT",                                     //   = 24,
    "ED_STOP",                                      //   = 25,
    "ED_COEX_REJECT",                               //   = 26,
};

static char *ieee80154_tx_abort_reason_string[] = {
    "RSVD",                                         //   = 0,
    "RX_ACK_STOP",                                  //   = 1,
    "RX_ACK_SFD_TIMEOUT",                           //   = 2,
    "RX_ACK_CRC_ERROR",                             //   = 3,
    "RX_ACK_INVALID_LEN",                           //   = 4,
    "RX_ACK_FILTER_FAIL",                           //   = 5,
    "RX_ACK_NO_RSS",                                //   = 6,
    "RX_ACK_COEX_BREAK",                            //   = 7,
    "RX_ACK_TYPE_NOT_ACK",                          //   = 8,
    "RX_ACK_RESTART",                               //   = 9,
    "RSVD", "RSVD", "RSVD", "RSVD", "RSVD", "RSVD", //   = 10~15,
    "RX_ACK_TIMEOUT",                               //   = 16,
    "TX_STOP",                                      //   = 17,
    "TX_COEX_BREAK",                                //   = 18,
    "TX_SECURITY_ERROR",                            //   = 19,
    "RSVD", "RSVD", "RSVD", "RSVD",                 //   = 20~23
    "CCA_FAILED",                                   //   = 24,
    "CCA_BUSY",                                     //   = 25,
};

#endif // CONFIG_IEEE802154_RECORD_EVENT

#if CONFIG_IEEE802154_ASSERT
void ieee802154_assert_print(void)
{
#if CONFIG_IEEE802154_RECORD_EVENT
    ESP_EARLY_LOGW(TAG, "Print the record event, current event index: %d", g_ieee802154_probe.event_index);
    for (uint8_t i = 0; i < IEEE802154_ASSERT_RECORD_EVENT_SIZE; i++) {
        char event_log[200] = { 0 };
        char abort_log[100] = { 0 };
        snprintf(event_log, 200,"index %2d: event: 0x%4x, %15s, state:%10s, timestamp: %lld", i, g_ieee802154_probe.event[i].event,
            ieee802154_get_event_string(g_ieee802154_probe.event[i].event),
            ieee802154_state_string[g_ieee802154_probe.event[i].state],
            g_ieee802154_probe.event[i].timestamp);
        if (g_ieee802154_probe.event[i].event == IEEE802154_EVENT_RX_ABORT) {
            snprintf(abort_log, 100, "rx abort reason: %4x, %20s", g_ieee802154_probe.event[i].abort_reason.rx,
                ieee80154_rx_abort_reason_string[g_ieee802154_probe.event[i].abort_reason.rx]);
        } else if (g_ieee802154_probe.event[i].event == IEEE802154_EVENT_TX_ABORT) {
            snprintf(abort_log, 100, "tx abort reason: %4x, %20s", g_ieee802154_probe.event[i].abort_reason.tx,
                ieee80154_tx_abort_reason_string[g_ieee802154_probe.event[i].abort_reason.tx]);
        }
        ESP_EARLY_LOGW(TAG, "%s %s", event_log, abort_log);
    }
    ESP_EARLY_LOGW(TAG,"Print the record event done.\n");
#endif // CONFIG_IEEE802154_RECORD_EVENT

#if CONFIG_IEEE802154_RECORD_STATE
    ESP_EARLY_LOGW(TAG, "Print the record state, current state index: %d", g_ieee802154_probe.state_index);
    for (uint8_t i = 0; i < IEEE802154_ASSERT_RECORD_STATE_SIZE; i++) {
        ESP_EARLY_LOGW(TAG, "index %2d: line:%5s, state:%10s, timestamp: %lld",
            i, g_ieee802154_probe.state[i].line_str,
            ieee802154_state_string[g_ieee802154_probe.state[i].state],
            g_ieee802154_probe.state[i].timestamp);
    }
    ESP_EARLY_LOGW(TAG,"Print the record state done.\n");
#endif // CONFIG_IEEE802154_RECORD_STATE

#if CONFIG_IEEE802154_RECORD_CMD
    ESP_EARLY_LOGW(TAG, "Print the record cmd, current cmd index: %d", g_ieee802154_probe.cmd_index);
    for (uint8_t i = 0; i < IEEE802154_ASSERT_RECORD_CMD_SIZE; i++) {
        ESP_EARLY_LOGW(TAG, "index %2d: line:%5s, cmd:%10s, timestamp: %lld",
            i, g_ieee802154_probe.cmd[i].line_str,
            ieee802154_get_cmd_string(g_ieee802154_probe.cmd[i].cmd),
            g_ieee802154_probe.cmd[i].timestamp);
    }
    ESP_EARLY_LOGW(TAG,"Print the record cmd done.\n");
#endif // CONFIG_IEEE802154_RECORD_CMD

#if CONFIG_IEEE802154_RECORD_ABORT
    ESP_EARLY_LOGW(TAG, "Print the record abort, current abort index: %d", g_ieee802154_probe.abort_index);
    for (uint8_t i = 0; i < IEEE802154_ASSERT_RECORD_ABORT_SIZE; i++) {
        if (g_ieee802154_probe.abort[i].is_tx_abort) {
            ESP_EARLY_LOGW(TAG, "index %2d: tx abort: %4x, %15s, timestamp: %lld",
                i, g_ieee802154_probe.abort[i].abort_reason.tx,
                ieee80154_tx_abort_reason_string[g_ieee802154_probe.abort[i].abort_reason.tx],
                g_ieee802154_probe.abort[i].timestamp);
        } else {
            ESP_EARLY_LOGW(TAG, "index %2d: rx abort: %4x, %15s, timestamp: %lld",
                i, g_ieee802154_probe.abort[i].abort_reason.rx,
                ieee80154_rx_abort_reason_string[g_ieee802154_probe.abort[i].abort_reason.rx],
                g_ieee802154_probe.abort[i].timestamp);
        }
    }
    ESP_EARLY_LOGW(TAG,"Print the record abort done.\n");
#endif // CONFIG_IEEE802154_RECORD_ABORT
}
#endif // CONFIG_IEEE802154_ASSERT

#if CONFIG_IEEE802154_TXRX_STATISTIC
static ieee802154_txrx_statistic_t s_ieee802154_txrx_statistic;

void ieee802154_txrx_statistic_clear(void)
{
    memset(&s_ieee802154_txrx_statistic, 0, sizeof(ieee802154_txrx_statistic_t));
}

void ieee802154_txrx_statistic(ieee802154_ll_events events)
{
    if (events == IEEE802154_EVENT_TX_DONE) {
        s_ieee802154_txrx_statistic.tx.done_nums++;
    } else if (events == IEEE802154_EVENT_RX_DONE) {
        s_ieee802154_txrx_statistic.rx.done_nums++;
    }
    s_ieee802154_txrx_statistic.tx.abort.cca_busy_nums += ieee802154_ll_get_cca_busy_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_CCA_BUSY_CNT_CLEAR);
    s_ieee802154_txrx_statistic.tx.abort.tx_security_error_nums += ieee802154_ll_get_tx_security_error_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_TX_SECURITY_ERROR_CNT_CLEAR);

    // Do not record TX_BREAK_COEX_ERR due to ZB-105.

    s_ieee802154_txrx_statistic.tx.abort.rx_ack_timeout_nums += ieee802154_ll_get_rx_ack_timeout_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_RX_ACK_TIMEOUT_CNT_CLEAR);
    s_ieee802154_txrx_statistic.tx.abort.rx_ack_coex_break_nums += ieee802154_ll_get_rx_ack_abort_coex_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_RX_ACK_ABORT_COEX_CNT_CLEAR);
    s_ieee802154_txrx_statistic.tx.abort.cca_failed_nums += ieee802154_ll_get_cca_fail_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_CCA_FAIL_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.tx_ack_coex_break_nums += ieee802154_ll_get_tx_ack_abort_coex_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_TX_ACK_ABORT_COEX_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.rx_restart_nums += ieee802154_ll_get_rx_restart_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_RX_RESTART_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.rx_coex_break_nums += ieee802154_ll_get_rx_abort_coex_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_RX_ABORT_COEX_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.no_rss_nums += ieee802154_ll_get_no_rss_detect_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_NO_RSS_DETECT_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.filter_fail_nums += ieee802154_ll_get_rx_fliter_fail_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_RX_FILTER_FAIL_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.ed_abort_nums += ieee802154_ll_get_ed_abort_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_ED_ABORT_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.crc_error_nums += ieee802154_ll_get_crc_error_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_CRC_ERROR_CNT_CLEAR);
    s_ieee802154_txrx_statistic.rx.abort.sfd_timeout_nums += ieee802154_ll_get_sfd_timeout_cnt();
    ieee802154_ll_clear_debug_cnt(IEEE802154_SFD_TIMEOUT_CNT_CLEAR);
}

void ieee802154_tx_nums_update(void)
{
    s_ieee802154_txrx_statistic.tx.nums++;
}

void ieee802154_tx_deferred_nums_update(void)
{
    s_ieee802154_txrx_statistic.tx.deferred_nums++;
}

void ieee802154_tx_break_coex_nums_update(void)
{
    s_ieee802154_txrx_statistic.tx.abort.tx_coex_break_nums++;
}

void ieee802154_txrx_statistic_print(void)
{
    uint64_t tx_success_nums = s_ieee802154_txrx_statistic.tx.done_nums - s_ieee802154_txrx_statistic.tx.abort.rx_ack_coex_break_nums - s_ieee802154_txrx_statistic.tx.abort.rx_ack_timeout_nums;
    uint64_t tx_abort_nums = s_ieee802154_txrx_statistic.tx.abort.rx_ack_coex_break_nums + s_ieee802154_txrx_statistic.tx.abort.rx_ack_timeout_nums +
                             s_ieee802154_txrx_statistic.tx.abort.tx_coex_break_nums + s_ieee802154_txrx_statistic.tx.abort.tx_security_error_nums +
                             s_ieee802154_txrx_statistic.tx.abort.cca_failed_nums + s_ieee802154_txrx_statistic.tx.abort.cca_busy_nums;

    uint64_t tx_nums = s_ieee802154_txrx_statistic.tx.nums;
    uint64_t tx_direct_num = tx_nums - s_ieee802154_txrx_statistic.tx.deferred_nums;

    float tx_success_ratio = (tx_nums > 0 ? ((float)tx_success_nums / tx_nums) : 0);
    float tx_done_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.done_nums / tx_nums) : 0);
    float tx_abort_ratio = (tx_nums > 0 ? ((float)tx_abort_nums / tx_nums) : 0);

    float tx_direct_num_ratio = (tx_nums > 0 ? ((float)tx_direct_num / tx_nums) : 0);
    float tx_deferred_num_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.deferred_nums / tx_nums) : 0);

    float tx_abort_rx_ack_coex_break_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.rx_ack_coex_break_nums / tx_nums) : 0);
    float tx_abort_rx_ack_timeout_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.rx_ack_timeout_nums / tx_nums) : 0);
    float tx_abort_tx_coex_break_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.tx_coex_break_nums / tx_nums) : 0);
    float tx_abort_tx_security_error_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.tx_security_error_nums / tx_nums) : 0);
    float tx_abort_cca_failed_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.cca_failed_nums / tx_nums) : 0);
    float tx_abort_cca_busy_ratio = (tx_nums > 0 ? ((float)s_ieee802154_txrx_statistic.tx.abort.cca_busy_nums / tx_nums) : 0);

    uint64_t rx_abort_nums = s_ieee802154_txrx_statistic.rx.abort.tx_ack_coex_break_nums + s_ieee802154_txrx_statistic.rx.abort.sfd_timeout_nums +
                             s_ieee802154_txrx_statistic.rx.abort.crc_error_nums + s_ieee802154_txrx_statistic.rx.abort.filter_fail_nums +
                             s_ieee802154_txrx_statistic.rx.abort.no_rss_nums + s_ieee802154_txrx_statistic.rx.abort.rx_coex_break_nums +
                             s_ieee802154_txrx_statistic.rx.abort.rx_restart_nums + s_ieee802154_txrx_statistic.rx.abort.ed_abort_nums;
    uint64_t rx_success_nums = s_ieee802154_txrx_statistic.rx.done_nums - s_ieee802154_txrx_statistic.rx.abort.tx_ack_coex_break_nums;


    ESP_LOGW(TAG, "+--------------------+-----------------------------------+--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-10s%-15llu%9.2f%%|%-25s%-15llu%9.2f%%|", "", "Done:", s_ieee802154_txrx_statistic.tx.done_nums, tx_done_ratio*100, "Success:", tx_success_nums, tx_success_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "tx_direct_num:", tx_direct_num, tx_direct_num_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "tx_deferred_num:", s_ieee802154_txrx_statistic.tx.deferred_nums, tx_deferred_num_ratio*100);
    ESP_LOGW(TAG, "+                    +-----------------------------------+--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "rx_ack_coex_break:", s_ieee802154_txrx_statistic.tx.abort.rx_ack_coex_break_nums, tx_abort_rx_ack_coex_break_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "rx_ack_timeout:", s_ieee802154_txrx_statistic.tx.abort.rx_ack_timeout_nums, tx_abort_rx_ack_timeout_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-5s%-15llu|%-10s%-15llu%9.2f%%|%-25s%-15llu%9.2f%%|", "TX:", s_ieee802154_txrx_statistic.tx.nums, "Abort", tx_abort_nums, tx_abort_ratio*100, "tx_coex_break:", s_ieee802154_txrx_statistic.tx.abort.tx_coex_break_nums, tx_abort_tx_coex_break_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "tx_security_error:", s_ieee802154_txrx_statistic.tx.abort.tx_security_error_nums, tx_abort_tx_security_error_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "cca_failed:", s_ieee802154_txrx_statistic.tx.abort.cca_failed_nums, tx_abort_cca_failed_ratio*100);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-15llu%9.2f%%|", "", "", "cca_busy:", s_ieee802154_txrx_statistic.tx.abort.cca_busy_nums, tx_abort_cca_busy_ratio*100);
    ESP_LOGW(TAG, "+--------------------+-----------------------------------+--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-10s%-25llu|%-25s%-25llu|", "", "Done:", s_ieee802154_txrx_statistic.rx.done_nums, "Success:", rx_success_nums);
    ESP_LOGW(TAG, "+                    +-----------------------------------+--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "tx_ack_coex_break:", s_ieee802154_txrx_statistic.rx.abort.tx_ack_coex_break_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "sfd_timeout:", s_ieee802154_txrx_statistic.rx.abort.sfd_timeout_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "crc_error:", s_ieee802154_txrx_statistic.rx.abort.crc_error_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-10s%-25llu|%-25s%-25llu|", "RX", "Abort", rx_abort_nums, "filter_fail:", s_ieee802154_txrx_statistic.rx.abort.filter_fail_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "no_rss:", s_ieee802154_txrx_statistic.rx.abort.no_rss_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "rx_coex_break:", s_ieee802154_txrx_statistic.rx.abort.rx_coex_break_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "rx_restart:", s_ieee802154_txrx_statistic.rx.abort.rx_restart_nums);
    ESP_LOGW(TAG, "+                    +                                   +--------------------------------------------------+");
    ESP_LOGW(TAG, "|%-20s|%-35s|%-25s%-25llu|", "", "", "ed_abort:", s_ieee802154_txrx_statistic.rx.abort.ed_abort_nums);
    ESP_LOGW(TAG, "+--------------------+-----------------------------------+--------------------------------------------------+");
}

#endif // CONFIG_IEEE802154_TXRX_STATISTIC

#endif // CONFIG_IEEE802154_DEBUG
