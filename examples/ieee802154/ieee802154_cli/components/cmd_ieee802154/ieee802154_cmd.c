/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/soc.h"
#include "soc/ieee802154_reg.h"

static uint8_t s_tx_frame[131] = { 0 };
static const char* TAG = "i154cmd";

static void register_rx(void);
static void register_tx(void);
static void register_energy(void);
static void register_channel(void);
static void register_txpower(void);
static void register_promisc(void);
static void register_panid(void);
static void register_shortaddr(void);
static void register_extaddr(void);
static void register_coordinator(void);
static void register_pending(void);
static void register_cca(void);
static void register_esp154(void);
static void register_reg(void);

void register_ieee802154_cmd(void)
{
    register_rx();
    register_tx();
    register_energy();
    register_channel();
    register_txpower();
    register_promisc();
    register_panid();
    register_shortaddr();
    register_extaddr();
    register_coordinator();
    register_pending();
    register_cca();
    register_esp154();
    register_reg();
}

static struct {
    struct arg_int *duration;
    struct arg_end *end;
} energy_args;

static int process_energy(int argc, char **argv)
{
    uint32_t duration = 0;

    int nerrors = arg_parse(argc, argv, (void **) &energy_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, energy_args.end, argv[0]);
        return 1;
    }

    if (energy_args.duration->count) {
        duration = energy_args.duration->ival[0];
        if (energy_args.duration->ival[0]) {
            ESP_LOGI(TAG, "ed start");
            esp_ieee802154_energy_detect(duration);
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

static void register_energy(void)
{
    energy_args.duration =
        arg_int0("d", "duration", "<duration>", "configure the duration of energy detection");
    energy_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "ed",
        .help = "configure the duration",
        .hint = NULL,
        .func = &process_energy,
        .argtable = &energy_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'channel' command gets/sets the channel number which the radio will work on */
static struct {
    struct arg_lit *get_channel;
    struct arg_int *set_channel;
    struct arg_end *end;
} channel_args;

static int process_channel(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &channel_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, channel_args.end, argv[0]);
        return 1;
    }
    if (channel_args.get_channel->count) {
        uint8_t channel = esp_ieee802154_get_channel();
        ESP_LOGI(TAG, "current channel: %d", channel);
    } else if (channel_args.set_channel->count) {
        uint8_t channel = channel_args.set_channel->ival[0];
        esp_ieee802154_set_channel(channel);
        ESP_LOGI(TAG, "set channel: %d", channel);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_channel()
{
    channel_args.get_channel =
        arg_lit0("g", "get", "get channel number");
    channel_args.set_channel =
        arg_int0("s", "set", "<n>", "set channel number");
    channel_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "channel",
        .help = "get/set the channel number",
        .hint = NULL,
        .func = &process_channel,
        .argtable = &channel_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'txpower' command gets/sets the txpower */
static struct {
    struct arg_lit *get_txpower;
    struct arg_int *set_txpower;
    struct arg_end *end;
} txpower_args;

static int process_txpower(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &txpower_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, txpower_args.end, argv[0]);
        return 1;
    }
    if (txpower_args.get_txpower->count) {
        int8_t txpower = esp_ieee802154_get_txpower();
        ESP_LOGI(TAG, "current txpower: %d", txpower);
    } else if (txpower_args.set_txpower->count) {
        int8_t txpower = txpower_args.set_txpower->ival[0];
        esp_ieee802154_set_txpower(txpower);
        ESP_LOGI(TAG, "set txpower: %d", txpower);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_txpower()
{
    txpower_args.get_txpower =
        arg_lit0("g", "get", "get txpower");
    txpower_args.set_txpower =
        arg_int0("s", "set", "<n>", "set txpower,value range -80dBm~-10dBm");
    txpower_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "txpower",
        .help = "get/set the txpower",
        .hint = NULL,
        .func = &process_txpower,
        .argtable = &txpower_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'promisc' command enable/disable the promiscuous mode */
static struct {
    struct arg_lit *enable;
    struct arg_lit *disable;
    struct arg_lit *get_promisc;
    struct arg_end *end;
} promisc_args;

static int process_promisc(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &promisc_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, promisc_args.end, argv[0]);
        return 1;
    }

    if (promisc_args.enable->count) {
        esp_ieee802154_set_promiscuous(true);
        ESP_LOGI(TAG, "promiscuous mode is enabled");
    } else if (promisc_args.disable->count) {
        esp_ieee802154_set_promiscuous(false);
        ESP_LOGI(TAG, "promiscuous mode is disabled");
    } else if (promisc_args.get_promisc->count) {
        ESP_LOGI(TAG, "hardware promiscuous mode was %s", esp_ieee802154_get_promiscuous() ? "enabled" : "disabled");
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_promisc()
{
    promisc_args.enable =
        arg_lit0("e", "enable", "enable the promiscuous mode");
    promisc_args.disable =
        arg_lit0("d", "disable", "disable the promiscuous mode");
    promisc_args.get_promisc =
        arg_lit0("g", "get", "get the promiscuous mode");
    promisc_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "promisc",
        .help = "enable/disable/get the promiscuous mode",
        .hint = NULL,
        .func = &process_promisc,
        .argtable = &promisc_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'panid' command set the device Pan ID */
static struct {
    struct arg_int *set_panid;
    struct arg_lit *get_panid;
    struct arg_end *end;
} panid_args;

static int process_panid(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &panid_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, panid_args.end, argv[0]);
        return 1;
    }

    if (panid_args.set_panid->count == 1) {
        uint16_t panid = panid_args.set_panid->ival[0];
        esp_ieee802154_set_panid(panid);
        ESP_LOGI(TAG, "set panid: 0x%x", panid);
    } else if (panid_args.get_panid->count) {
        uint16_t panid = esp_ieee802154_get_panid();
        ESP_LOGI(TAG, "current panid: 0x%x", panid);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
    }

    return 0;
}

static void register_panid()
{
    panid_args.set_panid =
        arg_int0(NULL, NULL, "<panid>", "set the device Pan ID");
    panid_args.get_panid =
        arg_lit0("g", "get", "get the device Pan ID");
    panid_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "panid",
        .help = "get/set the device Pan ID",
        .hint = NULL,
        .func = &process_panid,
        .argtable = &panid_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *set_shortaddr;
    struct arg_lit *get_shortaddr;
    struct arg_end *end;
} shortaddr_args;

static int process_shortaddr(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &shortaddr_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, shortaddr_args.end, argv[0]);
        return 1;
    }

    if (shortaddr_args.set_shortaddr->count == 1) {
        uint16_t short_address = shortaddr_args.set_shortaddr->ival[0];
        esp_ieee802154_set_short_address(short_address);
        ESP_LOGI(TAG, "set shortaddr: 0x%x", short_address);
    } else if (shortaddr_args.get_shortaddr->count) {
        uint16_t short_address = esp_ieee802154_get_short_address();
        ESP_LOGI(TAG, "current shortaddr: 0x%x", short_address);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
    }

    return 0;
}

static void register_shortaddr()
{
    shortaddr_args.set_shortaddr =
        arg_int0(NULL, NULL, "<shortaddr>", "set the device short address");
    shortaddr_args.get_shortaddr =
        arg_lit0("g", "get", "get the device short address");
    shortaddr_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "shortaddr",
        .help = "get/set the device short address",
        .hint = NULL,
        .func = &process_shortaddr,
        .argtable = &shortaddr_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_lit *get_extaddr;
    struct arg_int *set_extaddr;
    struct arg_end *end;
} extaddr_args;

static int process_extaddr(int argc, char **argv)
{
    uint8_t extaddr[8] = {0};
    int nerrors = arg_parse(argc, argv, (void **) &extaddr_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, extaddr_args.end, argv[0]);
        return 1;
    }
    if (extaddr_args.set_extaddr->count == 8) {
        for (uint8_t i = 0; i < 8; i++) {
            extaddr[i] = extaddr_args.set_extaddr->ival[i];
        }
        esp_ieee802154_set_extended_address(extaddr);
        ESP_LOGI(TAG, "set extaddr: %02x%02x%02x%02x%02x%02x%02x%02x", extaddr[7], extaddr[6], extaddr[5], extaddr[4],
                 extaddr[3], extaddr[2], extaddr[1], extaddr[0]);
    } else if (extaddr_args.get_extaddr->count) {
        esp_ieee802154_get_extended_address(extaddr);
        ESP_LOGI(TAG, "get extaddr: %02x%02x%02x%02x%02x%02x%02x%02x", extaddr[7], extaddr[6], extaddr[5], extaddr[4],
                 extaddr[3], extaddr[2], extaddr[1], extaddr[0]);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
    }

    return 0;
}

static void register_extaddr()
{
    extaddr_args.set_extaddr =
        arg_intn(NULL, NULL, "<extaddr>", 0, 127, "set the device extended address");
    extaddr_args.get_extaddr =
        arg_lit0("g", "get", "get the device extended address");
    extaddr_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "extaddr",
        .help = "set/get the device extended address",
        .hint = NULL,
        .func = &process_extaddr,
        .argtable = &extaddr_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'coordinator' command enable/disable the coordinator */
static struct {
    struct arg_lit *enable;
    struct arg_lit *disable;
    struct arg_lit *get_coordinator;
    struct arg_end *end;
} coordinator_args;

static int process_coordinator(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &coordinator_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, coordinator_args.end, argv[0]);
        return 1;
    }

    if (coordinator_args.enable->count) {
        esp_ieee802154_set_coordinator(true);
        ESP_LOGI(TAG, "hardware coordinator is enabled");
    } else if (coordinator_args.disable->count) {
        esp_ieee802154_set_coordinator(false);
        ESP_LOGI(TAG, "hardware coordinator is disabled");
    } else if (coordinator_args.get_coordinator->count) {
        ESP_LOGI(TAG, "hardware coordinator was %s", esp_ieee802154_get_coordinator() ? "enabled" : "disabled");
    }
    return 0;
}

static void register_coordinator()
{
    coordinator_args.enable =
        arg_lit0("e", "enable", "enable the coordinator");
    coordinator_args.disable =
        arg_lit0("d", "disable", "disable the coordinator");
    coordinator_args.get_coordinator =
        arg_lit0("g", "get", "get the coordinator");
    coordinator_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "coordinator",
        .help = "enable/disable/get the coordinator",
        .hint = NULL,
        .func = &process_coordinator,
        .argtable = &coordinator_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

static struct {
    struct arg_int *set_mode;
    struct arg_lit *get_mode;
    struct arg_lit *reset;
    struct arg_int *add_addr;
    struct arg_int *del_addr;
    struct arg_end *end;
} pending_args;

static int process_pending(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &pending_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, pending_args.end, argv[0]);
        return 1;
    }

    esp_ieee802154_pending_mode_t pending_mode = 0;
    if (pending_args.set_mode->count) {
        pending_mode = pending_args.set_mode->ival[0];
        esp_ieee802154_set_pending_mode(pending_mode);
        ESP_LOGI(TAG, "set auto pending mode as: %d", pending_mode);
    } else if (pending_args.get_mode->count) {
        pending_mode = esp_ieee802154_get_pending_mode();
        ESP_LOGI(TAG, "get auto pending mode as: %d", pending_mode);
    } else if (pending_args.reset->count) {
        esp_ieee802154_reset_pending_table(false);
        esp_ieee802154_reset_pending_table(true);
        ESP_LOGI(TAG, "clear the pending address table");
    } else if (pending_args.add_addr->count == 2) {
        uint8_t short_addr[2];

        short_addr[0] = pending_args.add_addr->ival[0];
        short_addr[1] = pending_args.add_addr->ival[1];

        esp_ieee802154_add_pending_addr(short_addr, true);
        ESP_LOGI(TAG, "add pending shortaddr: 0x%02x%02x", short_addr[1], short_addr[0]);
    } else if (pending_args.add_addr->count == 8) {
        uint8_t extaddr[8] = {0};

        for (uint8_t i = 0; i < 8; i++) {
            extaddr[i] = pending_args.add_addr->ival[i];
        }

        esp_ieee802154_add_pending_addr(extaddr, false);
        ESP_LOGI(TAG, "add pending extaddr: 0x%02x%02x%02x%02x%02x%02x%02x%02x", extaddr[7], extaddr[6], extaddr[5], extaddr[4],
                 extaddr[3], extaddr[2], extaddr[1], extaddr[0]);
    } else if (pending_args.del_addr->count == 2) {
        uint8_t short_addr[2];

        short_addr[0] = pending_args.del_addr->ival[0];
        short_addr[1] = pending_args.del_addr->ival[1];

        esp_ieee802154_clear_pending_addr(short_addr, true);
        ESP_LOGI(TAG, "delete pending shortaddr: 0x%02x%02x", short_addr[1], short_addr[0]);
    } else if (pending_args.del_addr->count == 8) {
        uint8_t extaddr[8] = {0};

        for (uint8_t i = 0; i < 8; i++) {
            extaddr[i] = pending_args.del_addr->ival[i];
        }

        esp_ieee802154_clear_pending_addr(extaddr, false);
        ESP_LOGI(TAG, "delete pending extaddr: 0x%02x%02x%02x%02x%02x%02x%02x%02x", extaddr[7], extaddr[6], extaddr[5], extaddr[4],
                 extaddr[3], extaddr[2], extaddr[1], extaddr[0]);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
    }

    return 0;
}

static void register_pending()
{
    pending_args.set_mode =
        arg_int0("m", "mode", "<mode>", "set the autopending mode (0: disable; 1: enable; 2: enhanced)");
    pending_args.get_mode =
        arg_lit0("g", "get", "get the autopending mode (0: disable; 1: enable; 2: enhanced)");
    pending_args.reset =
        arg_lit0("r", "reset", "reset the pending address table");
    pending_args.add_addr =
        arg_intn(NULL, NULL, "<addr>", 0, 8, "add the address to pending table (2 bytes for short address, 8 bytes for extended address)");
    pending_args.del_addr =
        arg_intn("d", "delete", "<addr>", 0, 8, "delete the address from pending table (2 bytes for short address, 8 bytes for extended address)");
    pending_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "pending",
        .help = "configure the pending address table",
        .hint = NULL,
        .func = &process_pending,
        .argtable = &pending_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

//CCA command
static struct {
    struct arg_int *cca_threshold_set;
    struct arg_int *cca_mode_set;
    struct arg_lit *cca_cfg_get;
    struct arg_end *end;
} cca_args;

static int process_cca(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &cca_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, cca_args.end, argv[0]);
        return 1;
    }

    int8_t cca_threshold = 0;
    esp_ieee802154_cca_mode_t cca_mode = 0;
    if (cca_args.cca_threshold_set->count) {
        cca_threshold = cca_args.cca_threshold_set->ival[0];
        esp_ieee802154_set_cca_threshold(cca_threshold);
        ESP_LOGI(TAG, "set cca threshold as: %d", cca_threshold);
    }

    else if (cca_args.cca_mode_set->count) {
        cca_mode = cca_args.cca_mode_set->ival[0];
        esp_ieee802154_set_cca_mode(cca_mode);
        ESP_LOGI(TAG, "set cca mode as: %d", cca_mode);
    }

    else if (cca_args.cca_cfg_get->count) {
        ESP_LOGI(TAG, "threshold:%d dB, mode: %d", esp_ieee802154_get_cca_threshold(), esp_ieee802154_get_cca_mode());
    } else {
        ESP_LOGI(TAG, "no valid arguments");
    }

    return 0;
}

static void register_cca()
{
    cca_args.cca_threshold_set =
        arg_int0("v", "value", "<threshold>", "cca threshold setting");
    cca_args.cca_mode_set =
        arg_int0("m", "mode", "<mode>", "cca mode, 0: carrier; 1: ED; 2: carrier or ED; 3: carrier and ED");
    cca_args.cca_cfg_get =
        arg_lit0("g", "get", "get the cca cfg");
    cca_args.end = arg_end(4);

    const esp_console_cmd_t cmd = {
        .command = "cca",
        .help = "cca config",
        .hint = NULL,
        .func = &process_cca,
        .argtable = &cca_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'esp_154' command enable/disable initialize the IEEE 802.15.4 subsystem */
static struct {
    struct arg_lit *enable;
    struct arg_lit *disable;
    struct arg_end *end;
} esp154_args;

static int process_esp154(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &esp154_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, esp154_args.end, argv[0]);
        return 1;
    }

    if (esp154_args.enable->count) {
        esp_ieee802154_enable();
        ESP_LOGI(TAG, "initialize the IEEE 802.15.4 subsystem");
    } else if (esp154_args.disable->count) {
        esp_ieee802154_disable();
        ESP_LOGI(TAG, "deinitialize the IEEE 802.15.4 subsystem");
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_esp154()
{
    esp154_args.enable =
        arg_lit0("e", "enable", "initialize the IEEE 802.15.4 subsystem");
    esp154_args.disable =
        arg_lit0("d", "disable", "deinitialize the IEEE 802.15.4 subsystem");
    esp154_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "esp154",
        .help = "enable/disable initialize the IEEE 802.15.4 subsystem",
        .hint = NULL,
        .func = &process_esp154,
        .argtable = &esp154_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
}

/* 'reg' command reads/writes the registers */
static struct {
    struct arg_int *get_reg;
    struct arg_int *set_reg;
    struct arg_int *value;
    struct arg_end *end;
} reg_args;

static int process_reg(int argc, char **argv)
{
    uint32_t base = IEEE802154_REG_BASE;

    int nerrors = arg_parse(argc, argv, (void **) &reg_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, reg_args.end, argv[0]);
        return 1;
    }

    if (reg_args.get_reg->count == 1) {
        uint32_t reg_offset = reg_args.get_reg->ival[0];
        uint32_t reg_value = REG_READ(reg_offset + base);
        ESP_LOGI(TAG, "reg 0x%lx value: 0x%lx", reg_offset, reg_value);
    } else if (reg_args.set_reg->count == 1 && reg_args.value->count == 1) {
        uint32_t reg_offset = reg_args.set_reg->ival[0];
        uint32_t reg_value = reg_args.value->ival[0];
        REG_WRITE(reg_offset + base, reg_value);
        ESP_LOGI(TAG, "write reg 0x%lx: 0x%lx", reg_offset, reg_value);
    } else {
        ESP_LOGE(TAG, "no valid arguments");
        return 1;
    }
    return 0;
}

static void register_reg()
{
    reg_args.get_reg =
        arg_int0("r", "read", "<address>", "read the register value");
    reg_args.set_reg =
        arg_int0("w", "write", "<address>", "register address to be written");
    reg_args.value =
        arg_int0("v", "value", "<value>", "register value to be written");
    reg_args.end = arg_end(6);

    const esp_console_cmd_t cmd = {
        .command = "reg",
        .help = "read/write the register",
        .hint = NULL,
        .func = &process_reg,
        .argtable = &reg_args
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
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
    memcpy(s_tx_frame + offset, tx_frame, sizeof(tx_frame));
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

static void esp_ieee802154_frame_print(const uint8_t *frame)
{
    for (uint8_t idx = 1; idx < frame[0]; idx += 8) {
        ESP_EARLY_LOGI(TAG, "%02x %02x %02x %02x %02x %02x %02x %02x",
                       frame[idx], frame[idx + 1], frame[idx + 2], frame[idx + 3],
                       frame[idx + 4], frame[idx + 5], frame[idx + 6], frame[idx + 7]);
    }
}

void esp_ieee802154_transmit_done(const uint8_t *frame, const uint8_t *ack, esp_ieee802154_frame_info_t *ack_frame_info)
{
    ESP_EARLY_LOGI(TAG, "Tx Done %d bytes", frame[0]);
    esp_ieee802154_frame_print(frame);

    if (ack != NULL) {
        ESP_EARLY_LOGI(TAG, "Rx ack %d bytes", ack[0]);
        esp_ieee802154_frame_print(ack);
        esp_ieee802154_receive_handle_done(ack);
    }
}

void esp_ieee802154_receive_done(uint8_t *frame, esp_ieee802154_frame_info_t *frame_info)
{
    ESP_EARLY_LOGI(TAG, "Rx Done %d bytes", frame[0]);
    esp_ieee802154_frame_print(frame);
    esp_ieee802154_receive_handle_done(frame);
}

void esp_ieee802154_energy_detect_done(int8_t power)
{
    ESP_EARLY_LOGI(TAG, "ed_scan_rss_value: %d dB", power);
}

void esp_ieee802154_transmit_sfd_done(uint8_t *frame)
{
    ESP_EARLY_LOGI(TAG, "tx sfd done, Radio state: %d", esp_ieee802154_get_state());
}

void esp_ieee802154_receive_sfd_done(void)
{
    ESP_EARLY_LOGI(TAG, "rx sfd done, Radio state: %d", esp_ieee802154_get_state());
}

void esp_ieee802154_transmit_failed(const uint8_t *frame, esp_ieee802154_tx_error_t error)
{
    ESP_EARLY_LOGI(TAG, "the Frame Transmission failed, Failure reason: %d", error);
}
