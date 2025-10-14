/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_ieee802154.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "ieee802154_debug.h"

#if CONFIG_IEEE802154_DEBUG
static const char* TAG = "i154cmd";
#if CONFIG_IEEE802154_RX_BUFFER_STATISTIC
static void register_rx_buffer_statistic(void);
#endif

#if CONFIG_IEEE802154_TXRX_STATISTIC
static void register_txrx_statistic(void);
#endif

#if CONFIG_IEEE802154_RECORD
static void register_record(void);
#endif

void register_ieee802154_debug_cmd(void)
{
#if CONFIG_IEEE802154_RX_BUFFER_STATISTIC
    register_rx_buffer_statistic();
#endif
#if CONFIG_IEEE802154_TXRX_STATISTIC
    register_txrx_statistic();
#endif
#if CONFIG_IEEE802154_RECORD
    register_record();
#endif
}
#endif

#if CONFIG_IEEE802154_DEBUG
#if CONFIG_IEEE802154_RX_BUFFER_STATISTIC
static struct {
    struct arg_lit *clear;
    struct arg_lit *print;
    struct arg_end *end;
} rx_buff_stat_args;

static int process_rx_buffer_statistic(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &rx_buff_stat_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, rx_buff_stat_args.end, argv[0]);
        return 1;
    }
    if (rx_buff_stat_args.print->count) {
        esp_ieee802154_rx_buffer_statistic_print();
    }
    if (rx_buff_stat_args.clear->count) {
        esp_ieee802154_rx_buffer_statistic_clear();
        ESP_LOGI(TAG, "clear the rx buffer statistics");
    }
    if (!rx_buff_stat_args.print->count && !rx_buff_stat_args.clear->count) {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_rx_buffer_statistic(void)
{
    rx_buff_stat_args.print =
        arg_lit0("p", "print", "print a summary table of rx buffer statistics");
    rx_buff_stat_args.clear =
        arg_lit0("c", "clear", "clear the rx buffer statistics");
    rx_buff_stat_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "rxbufstat",
        .help = "rx buffer statistics",
        .hint = NULL,
        .func = &process_rx_buffer_statistic,
        .argtable = &rx_buff_stat_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif // CONFIG_IEEE802154_RX_BUFFER_STATISTIC

#if CONFIG_IEEE802154_TXRX_STATISTIC
static struct {
    struct arg_lit *clear;
    struct arg_lit *print;
    struct arg_end *end;
} txrx_stat_args;

static int process_txrx_statistic(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &txrx_stat_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, txrx_stat_args.end, argv[0]);
        return 1;
    }
    if (txrx_stat_args.print->count) {
        esp_ieee802154_txrx_statistic_print();
    }
    if (txrx_stat_args.clear->count) {
        esp_ieee802154_txrx_statistic_clear();
        ESP_LOGI(TAG, "clear the txrx statistics");
    }
    if (!txrx_stat_args.print->count && !txrx_stat_args.clear->count) {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_txrx_statistic(void)
{
    txrx_stat_args.print =
        arg_lit0("p", "print", "print the result of txrx statistic");
    txrx_stat_args.clear =
        arg_lit0("c", "clear", "clear the result of txrx statistic");
    txrx_stat_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "txrxstat",
        .help = "txrx statistic",
        .hint = NULL,
        .func = &process_txrx_statistic,
        .argtable = &txrx_stat_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif // CONFIG_IEEE802154_TXRX_STATISTIC

#if CONFIG_IEEE802154_RECORD
static struct {
    struct arg_lit *print;
    struct arg_end *end;
} record_args;

static int process_record(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &record_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, record_args.end, argv[0]);
        return 1;
    }
    if (record_args.print->count) {
        esp_ieee802154_record_print();
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_record(void)
{
    record_args.print =
        arg_lit0("p", "print", "print the result of the recording");
    record_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "record",
        .help = "print the recorded IEEE802154 state/event/cmd etc.",
        .hint = NULL,
        .func = &process_record,
        .argtable = &record_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}
#endif // CONFIG_IEEE802154_RECORD
#endif // CONFIG_IEEE802154_DEBUG
