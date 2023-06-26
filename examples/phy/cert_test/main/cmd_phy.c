/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_phy_cert_test.h"
#include "cmd_phy.h"

#define TAG "cmd_phy"

#define CERT_TASK_PRIO 2

#if CONFIG_ESP_PHY_ENABLE_CERT_TEST

static phy_args_t       phy_args;
#if SOC_WIFI_SUPPORTED
static phy_wifi_tx_t    phy_wifi_tx_args;
static phy_wifi_rx_t    phy_wifi_rx_args;
static phy_wifiscwout_t phy_wifiscwout_args;
#endif
#if SOC_BT_SUPPORTED
static phy_ble_tx_t     phy_ble_tx_args;
static phy_ble_rx_t     phy_ble_rx_args;
static phy_bt_tx_tone_t phy_bt_tx_tone_args;
#endif

#if CONFIG_ESP_PHY_LEGACY_COMMANDS
#define arg_int0(_a, _b, _c, _d) arg_int0(NULL, NULL, _c, _d)
#define arg_int1(_a, _b, _c, _d) arg_int1(NULL, NULL, _c, _d)
#endif

static int esp_phy_tx_contin_en_func(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &phy_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_args.end, argv[0]);
        return 1;
    }

    if (phy_args.enable->count == 1) {
        esp_phy_tx_contin_en(phy_args.enable->ival[0]);
    } else {
        ESP_LOGW(TAG, "Please enter the enable parameter");
    }
    return 0;
}

static int esp_phy_cmdstop_func(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &phy_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_args.end, argv[0]);
        return 1;
    }

    esp_phy_test_start_stop(0);

    return 0;
}

static int esp_phy_get_rx_result_func(int argc, char **argv)
{
    esp_phy_rx_result_t rx_result;
    int nerrors = arg_parse(argc, argv, (void **) &phy_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_args.end, argv[0]);
        return 1;
    }

    esp_phy_get_rx_result(&rx_result);

    ESP_LOGI(TAG, "Desired: %lu, Correct: %lu, RSSI: %d, flag: %lu", rx_result.phy_rx_total_count,
                rx_result.phy_rx_correct_count, rx_result.phy_rx_rssi, rx_result.phy_rx_result_flag);

    return 0;
}

#if SOC_WIFI_SUPPORTED
void cert_wifi_tx(void *arg)
{
    phy_wifi_tx_s *cmd  = (phy_wifi_tx_s*)arg;

    esp_phy_test_start_stop(3);
    esp_phy_wifi_tx(cmd->channel, cmd->rate, cmd->backoff, cmd->length_byte, cmd->packet_delay, cmd->packet_num);

    vTaskDelete(NULL);
}

void cert_wifi_rx(void *arg)
{
    phy_wifi_rx_s *cmd  = (phy_wifi_rx_s*)arg;

    esp_phy_test_start_stop(3);
    esp_phy_wifi_rx(cmd->channel, cmd->rate);

    vTaskDelete(NULL);
}

static int esp_phy_cbw40m_en_func(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &phy_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_args.end, argv[0]);
        return 1;
    }
    if (phy_args.enable->count == 1) {
        esp_phy_cbw40m_en(phy_args.enable->ival[0]);
    } else {
        ESP_LOGW(TAG, "Please enter the enable parameter");
    }
    return 0;
}

static int esp_phy_wifi_tx_func(int argc, char **argv)
{
    static phy_wifi_tx_s cmd;
    int nerrors = arg_parse(argc, argv, (void **) &phy_wifi_tx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_wifi_tx_args.end, argv[0]);
        return 1;
    }

    if (phy_wifi_tx_args.channel->count == 1) {
        cmd.channel = phy_wifi_tx_args.channel->ival[0];
    } else {
        cmd.channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_wifi_tx_args.rate->count == 1) {
        cmd.rate = phy_wifi_tx_args.rate->ival[0];
    } else {
        cmd.rate = PHY_RATE_1M;
        ESP_LOGW(TAG, "Default rate is PHY_RATE_1M");
    }

    if (phy_wifi_tx_args.attenuation->count == 1) {
        cmd.backoff = phy_wifi_tx_args.attenuation->ival[0];
    } else {
        cmd.backoff = 0;
        ESP_LOGW(TAG, "Default backoff is 0");
    }

    if (phy_wifi_tx_args.length_byte->count == 1) {
        cmd.length_byte = phy_wifi_tx_args.length_byte->ival[0];
    } else {
        cmd.length_byte = 1000;
        ESP_LOGW(TAG, "Default length_byte is 1000");
    }

    if (phy_wifi_tx_args.packet_delay->count == 1) {
        cmd.packet_delay = phy_wifi_tx_args.packet_delay->ival[0];
    } else {
        cmd.packet_delay = 1000;
        ESP_LOGW(TAG, "Default packet_delay is 1000");
    }

    if (phy_wifi_tx_args.packet_num->count == 1) {
        cmd.packet_num = phy_wifi_tx_args.packet_num->ival[0];
    } else {
        cmd.packet_num = 0;
        ESP_LOGW(TAG, "Default packet_num is 0");
    }

    xTaskCreate(cert_wifi_tx, "cert_wifi_tx", 1024 * 10, (void *)&cmd, CERT_TASK_PRIO, NULL);

    return 0;
}

static int esp_phy_wifi_rx_func(int argc, char **argv)
{
    static phy_wifi_rx_s cmd;
    int nerrors = arg_parse(argc, argv, (void **) &phy_wifi_rx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_wifi_rx_args.end, argv[0]);
        return 1;
    }

    if (phy_wifi_rx_args.channel->count == 1) {
        cmd.channel = phy_wifi_rx_args.channel->ival[0];
    } else {
        cmd.channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_wifi_rx_args.rate->count == 1) {
        cmd.rate = phy_wifi_rx_args.rate->ival[0];
    } else {
        cmd.rate = PHY_RATE_1M;
        ESP_LOGW(TAG, "Default rate is PHY_RATE_1M");
    }

    xTaskCreate(cert_wifi_rx, "cert_wifi_rx", 1024 * 20, (void *)&cmd, CERT_TASK_PRIO, NULL);
    return 0;
}

static int esp_phy_wifiscwout_func(int argc, char **argv)
{
    uint32_t enable;
    uint32_t channel;
    uint32_t attenuation;
    int nerrors = arg_parse(argc, argv, (void **) &phy_wifiscwout_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_wifiscwout_args.end, argv[0]);
        return 1;
    }

    if (phy_wifiscwout_args.enable->count == 1) {
        enable = phy_wifiscwout_args.enable->ival[0];
    } else {
        enable = 1;
        ESP_LOGW(TAG, "Default enable is 1");
    }

    if (phy_wifiscwout_args.channel->count == 1) {
        channel = phy_wifiscwout_args.channel->ival[0];
    } else {
        channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_wifiscwout_args.attenuation->count == 1) {
        attenuation = phy_wifiscwout_args.attenuation->ival[0];
    } else {
        attenuation = 0;
        ESP_LOGW(TAG, "Default attenuation is 0");
    }

    esp_phy_wifi_tx_tone(enable, channel, attenuation);

    return 0;
}
#endif

#if SOC_BT_SUPPORTED
void cert_ble_tx(void *arg)
{
    phy_ble_tx_s *cmd = (phy_ble_tx_s *)arg;

    esp_phy_test_start_stop(3);
    esp_phy_ble_tx(cmd->txpwr, cmd->channel, cmd->len, cmd->data_type, cmd->syncw, cmd->rate, cmd->tx_num_in);

    vTaskDelete(NULL);
}

void cert_ble_rx(void *arg)
{
    phy_ble_rx_s *cmd = (phy_ble_rx_s *)arg;

    esp_phy_test_start_stop(3);
    esp_phy_ble_rx(cmd->channel, cmd->syncw, cmd->rate);

    vTaskDelete(NULL);
}

static int esp_phy_ble_tx_func(int argc, char **argv)
{
    static phy_ble_tx_s cmd;
    int nerrors = arg_parse(argc, argv, (void **) &phy_ble_tx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_ble_tx_args.end, argv[0]);
        return 1;
    }

    if (phy_ble_tx_args.txpwr->count == 1) {
        cmd.txpwr = phy_ble_tx_args.txpwr->ival[0];
    } else {
        cmd.txpwr = 8;
        ESP_LOGW(TAG, "Default txpwr is 8");
    }

    if (phy_ble_tx_args.channel->count == 1) {
        cmd.channel = phy_ble_tx_args.channel->ival[0];
    } else {
        cmd.channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_ble_tx_args.len->count == 1) {
        cmd.len = phy_ble_tx_args.len->ival[0];
    } else {
        cmd.len = 37;
        ESP_LOGW(TAG, "Default len is 37");
    }

    if (phy_ble_tx_args.data_type->count == 1) {
        cmd.data_type = phy_ble_tx_args.data_type->ival[0];
    } else {
        cmd.data_type = PHY_BLE_TYPE_prbs9;
        ESP_LOGW(TAG, "Default data_type is PHY_BLE_TYPE_prbs9");
    }

    if (phy_ble_tx_args.syncw->count == 1) {
        cmd.syncw = phy_ble_tx_args.syncw->ival[0];
    } else {
        cmd.syncw = 0x71764129;
        ESP_LOGW(TAG, "Default syncw is 0x71764129");
    }

    if (phy_ble_tx_args.rate->count == 1) {
        cmd.rate = phy_ble_tx_args.rate->ival[0];
    } else {
        cmd.rate = PHY_BLE_RATE_1M;
        ESP_LOGW(TAG, "Default rate is PHY_BLE_RATE_1M");
    }

    if (phy_ble_tx_args.tx_num_in->count == 1) {
        cmd.tx_num_in = phy_ble_tx_args.tx_num_in->ival[0];
    } else {
        cmd.tx_num_in = 0;
        ESP_LOGW(TAG, "Default tx_num_in is 0");
    }

    xTaskCreate(cert_ble_tx, "cert_ble_tx", 4096, (void *)&cmd, CERT_TASK_PRIO, NULL);

    return 0;
}

static int esp_phy_ble_rx_func(int argc, char **argv)
{
    static phy_ble_rx_s cmd;
    int nerrors = arg_parse(argc, argv, (void **) &phy_ble_rx_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_ble_rx_args.end, argv[0]);
        return 1;
    }

    if (phy_ble_rx_args.channel->count == 1) {
        cmd.channel = phy_ble_rx_args.channel->ival[0];
    } else {
        cmd.channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_ble_rx_args.syncw->count == 1) {
        cmd.syncw = phy_ble_rx_args.syncw->ival[0];
    } else {
        cmd.syncw = 0x71764129;
        ESP_LOGW(TAG, "Default syncw is 0x71764129");
    }

    if (phy_ble_rx_args.rate->count == 1) {
        cmd.rate = phy_ble_rx_args.rate->ival[0];
    } else {
        cmd.rate = PHY_BLE_RATE_1M;
        ESP_LOGW(TAG, "Default rate is PHY_BLE_RATE_1M");
    }

    xTaskCreate(cert_ble_rx, "cert_ble_rx", 4096, (void *)&cmd, CERT_TASK_PRIO, NULL);

    return 0;
}

static int esp_phy_bt_tx_tone_func(int argc, char **argv)
{
    uint32_t start;
    uint32_t channel;
    uint32_t attenuation;
    int nerrors = arg_parse(argc, argv, (void **) &phy_bt_tx_tone_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, phy_bt_tx_tone_args.end, argv[0]);
        return 1;
    }

    if (phy_bt_tx_tone_args.start->count == 1) {
        start = phy_bt_tx_tone_args.start->ival[0];
    } else {
        start = 1;
        ESP_LOGW(TAG, "Default start is 1");
    }

    if (phy_bt_tx_tone_args.channel->count == 1) {
        channel = phy_bt_tx_tone_args.channel->ival[0];
    } else {
        channel = 1;
        ESP_LOGW(TAG, "Default channel is 1");
    }

    if (phy_bt_tx_tone_args.attenuation->count == 1) {
        attenuation = phy_bt_tx_tone_args.attenuation->ival[0];
    } else {
        attenuation = 0;
        ESP_LOGW(TAG, "Default backoff is 0");
    }

    esp_phy_bt_tx_tone(start, channel, attenuation);

    return 0;
}
#endif

void register_phy_cmd(void)
{
    phy_args.enable  = arg_int0(NULL, NULL, "<enable>", "enable");
    phy_args.end = arg_end(1);

    const esp_console_cmd_t tx_contin_cmd = {
        .command = "tx_contin_en",
        .help = "TX Continuous mode, 1: enable, 0: disable",
        .hint = NULL,
        .func = &esp_phy_tx_contin_en_func,
        .argtable = &phy_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&tx_contin_cmd) );

    const esp_console_cmd_t cmdstop_cmd = {
        .command = "cmdstop",
        .help = "TX/RX test stop command",
        .hint = NULL,
        .func = &esp_phy_cmdstop_func,
        .argtable = NULL
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmdstop_cmd) );

    const esp_console_cmd_t get_rx_result = {
        .command = "get_rx_result",
        .help = "Get RX information",
        .hint = NULL,
        .func = &esp_phy_get_rx_result_func,
        .argtable = NULL
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&get_rx_result) );

#if SOC_WIFI_SUPPORTED
    const esp_console_cmd_t cbw40m_cmd = {
        .command = "cbw40m_en",
        .help = "HT40/HT20 mode selection, 0: HT20, 1: HT40",
        .hint = NULL,
        .func = &esp_phy_cbw40m_en_func,
        .argtable = &phy_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cbw40m_cmd) );

    phy_wifi_tx_args.channel     = arg_int0("n", "channel"     , "<channel>"     , "channel setting, 1~14");
    phy_wifi_tx_args.rate        = arg_int0("r", "rate"        , "<rate>"        , "rate setting");
    phy_wifi_tx_args.attenuation = arg_int0("p", "attenuation" , "<attenuation>" , "Transmit power attenuation");
    phy_wifi_tx_args.length_byte = arg_int0("l", "length_byte" , "<length_byte>" , "TX packet length configuration");
    phy_wifi_tx_args.packet_delay= arg_int0("d", "packet_delay", "<packet_delay>", "TX packet interval configuration");
    phy_wifi_tx_args.packet_num  = arg_int0("c", "packet_num"  , "<packet_num>"  , "The number of packets to send");
    phy_wifi_tx_args.end         = arg_end(1);

    const esp_console_cmd_t esp_tx_cmd = {
        .command = "esp_tx",
        .help = "WiFi TX command",
        .hint = NULL,
        .func = &esp_phy_wifi_tx_func,
        .argtable = &phy_wifi_tx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&esp_tx_cmd) );

    phy_wifi_rx_args.channel     = arg_int0("n", "channel", "<channel>", "channel setting, 1~14");
    phy_wifi_rx_args.rate        = arg_int0("r", "rate"   , "<rate>"   , "rate setting");
    phy_wifi_rx_args.end         = arg_end(1);

    const esp_console_cmd_t esp_rx_cmd = {
        .command = "esp_rx",
        .help = "WiFi RX command",
        .hint = NULL,
        .func = &esp_phy_wifi_rx_func,
        .argtable = &phy_wifi_rx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&esp_rx_cmd) );

    phy_wifiscwout_args.enable      = arg_int0("e", "start"      , "<start>"      , "enable CW");
    phy_wifiscwout_args.channel     = arg_int0("c", "channel"    , "<channel>"    , "channel setting, 1~14");
    phy_wifiscwout_args.attenuation = arg_int0("p", "attenuation", "<attenuation>", "Transmit power attenuation");
    phy_wifiscwout_args.end         = arg_end(1);

    const esp_console_cmd_t wifiscwout_cmd = {
        .command = "wifiscwout",
        .help = "Wi-Fi CW TX command",
        .hint = NULL,
        .func = &esp_phy_wifiscwout_func,
        .argtable = &phy_wifiscwout_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&wifiscwout_cmd) );
#endif

#if SOC_BT_SUPPORTED
    phy_ble_tx_args.txpwr     = arg_int0("p", "txpwr"    , "<txpwr>"    , "Transmit power level setting");
    phy_ble_tx_args.channel   = arg_int0("n", "channel"  , "<channel>"  , "TX channel setting, range is 0~39");
    phy_ble_tx_args.len       = arg_int0("l", "len"      , "<len>"      , "Payload length setting, range is 0-255");
    phy_ble_tx_args.data_type = arg_int0("t", "data_type", "<data_type>", "Data type setting");
    phy_ble_tx_args.syncw     = arg_int0("s", "syncw"    , "<syncw>"    , "Packet identification");
    phy_ble_tx_args.rate      = arg_int0("r", "rate"     , "<rate>"     , "TX rate setting,0: 1M; 1: 2M；2: 125K；3: 500K");
    phy_ble_tx_args.tx_num_in = arg_int0("m", "tx_num_in", "<tx_num_in>", "TX mode setting");
    phy_ble_tx_args.end       = arg_end(1);

    const esp_console_cmd_t esp_ble_tx_cmd = {
#if CONFIG_ESP_PHY_NEW_COMMANDS
        .command = "esp_ble_tx",
#elif CONFIG_ESP_PHY_LEGACY_COMMANDS
        .command = "fcc_le_tx",
#endif
        .help = "BLE TX command",
        .hint = NULL,
        .func = &esp_phy_ble_tx_func,
        .argtable = &phy_ble_tx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&esp_ble_tx_cmd) );

    phy_ble_rx_args.channel = arg_int0("n", "channel", "<channel>", "RX channel setting, range is 0~39");
    phy_ble_rx_args.syncw   = arg_int0("s", "syncw"  , "<syncw>"  , "Packet identification");
    phy_ble_rx_args.rate    = arg_int0("r", "rate"   , "<rate>"   , "RX rate setting,0: 1M；1: 2M；2: 125K；3： 500K");
    phy_ble_rx_args.end     = arg_end(1);

    const esp_console_cmd_t esp_ble_rx_cmd = {
#if CONFIG_ESP_PHY_NEW_COMMANDS
        .command = "esp_ble_rx",
#elif CONFIG_ESP_PHY_LEGACY_COMMANDS
        .command = "rw_le_rx_per",
#endif
        .help = "BLE RX command",
        .hint = NULL,
        .func = &esp_phy_ble_rx_func,
        .argtable = &phy_ble_rx_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&esp_ble_rx_cmd) );

    phy_bt_tx_tone_args.start       = arg_int0("e", "start"  , "<start>"  , "enable CW, 1 means transmit, 0 means stop transmitting");
    phy_bt_tx_tone_args.channel     = arg_int0("n", "channel", "<channel>", "Single carrier transmission channel selection");
    phy_bt_tx_tone_args.attenuation = arg_int0("p", "power"  , "<power>"  , "CW power attenuation parameter");
    phy_bt_tx_tone_args.end         = arg_end(1);

    const esp_console_cmd_t bt_tx_tone_cmd = {
        .command = "bt_tx_tone",
        .help = "Single carrier TX command",
        .hint = NULL,
        .func = &esp_phy_bt_tx_tone_func,
        .argtable = &phy_bt_tx_tone_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&bt_tx_tone_cmd) );
#endif
}
#endif
