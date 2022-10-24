/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ieee802154.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "ieee802154_cmd.h"
#include "esp_phy_init.h"

static uint8_t s_tx_frame[131] = { 0 };
static const char* TAG = "i154test";

static void register_rx(void);
static void register_tx(void);

void register_ieee802154_cmd(void)
{
    register_rx();
    register_tx();
}

static struct {
    struct arg_int *length;
    struct arg_lit *cca_enable;
    struct arg_int *frame;
    struct arg_int *offset;
    struct arg_end *end;
} tx_args;

static int process_tx(int argc, char **argv)
{
    uint8_t tx_frame[128] = { 0 };
    uint8_t offset = 0;
    bool cca = false;
    int nerrors = arg_parse(argc, argv, (void **) &tx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tx_args.end, argv[0]);
        return 1;
    }
    memset(tx_frame, 0, sizeof(tx_frame));

    if (tx_args.offset->count && (tx_args.offset->ival[0] >= 0 && tx_args.offset->ival[0] <= 3)) {
        offset = tx_args.offset->ival[0];
    }

    if (tx_args.length->count) {
        tx_frame[0] = tx_args.length->ival[0];

        for (uint8_t i = 0; i < tx_frame[0]; i++) {
            tx_frame[i + 1] = i;
        }
    } else if (tx_args.frame->count) {
        tx_frame[0] = tx_args.frame->count;

        for (uint8_t i = 0; i < tx_frame[0]; i++) {
            tx_frame[i + 1] = tx_args.frame->ival[i];
        }
    }
    if (tx_args.cca_enable->count) {
        cca = true;
    }
    memcpy(s_tx_frame+offset, tx_frame, sizeof(tx_frame));
    esp_ieee802154_transmit(s_tx_frame, cca);
    return 0;
}

static void register_tx(void)
{
    tx_args.length =
        arg_int0("l", "length", "<len>", "frame length");
    tx_args.cca_enable =
        arg_lit0("C", "cca_enable", "set cca trigging tx mode true");
    tx_args.frame =
        arg_intn(NULL, NULL, "<n>", 0, 127, "transmit raw frame");
    tx_args.offset =
        arg_int0("o", "offset", "<offset>", "tx buffer offset");
    tx_args.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "tx",
        .help = "transmit IEEE 802154 MAC frame",
        .hint = NULL,
        .func = &process_tx,
        .argtable = &tx_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *receive;
    struct arg_end *end;
} rx_args;

static int process_rx(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &rx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, rx_args.end, argv[0]);
        return 1;
    }

    if (rx_args.receive->count == 1) {
        if (rx_args.receive->ival[0] == 0) {
            esp_ieee802154_sleep();
            ESP_LOGI(TAG, "radio exit receive mode");
        } else if (rx_args.receive->ival[0] == 1) { // receive, non-blocking
            ESP_LOGI(TAG, "RX Start");
            esp_ieee802154_receive();
            esp_ieee802154_set_rx_when_idle(true);
        } else {
            ESP_LOGE(TAG, "no valid arguments");
            return 1;
        }
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_rx(void)
{
    rx_args.receive =
        arg_int0("r", "receive", "<mode>", "configure the radio receive mode, 0: disable; 1: enable");
    rx_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "rx",
        .help = "enable/disable the receive mode",
        .hint = NULL,
        .func = &process_rx,
        .argtable = &rx_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info)
{
    ESP_EARLY_LOGI(TAG, "Tx Done %d bytes", frame[0]);
    for(uint8_t idx = 1; idx < frame[0]; idx+=8) {
        ESP_EARLY_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x",
        frame[idx], frame[idx+1], frame[idx+2], frame[idx+3],
        frame[idx+4], frame[idx+5], frame[idx+6], frame[idx+7]);
    }
}

void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info)
{
    ESP_EARLY_LOGI(TAG, "Rx Done %d bytes", frame[0]);
    for(uint8_t idx = 1; idx < frame[0]; idx+=8) {
        ESP_EARLY_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x",
        frame[idx], frame[idx+1], frame[idx+2], frame[idx+3],
        frame[idx+4], frame[idx+5], frame[idx+6], frame[idx+7]);
    }
}
