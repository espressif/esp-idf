/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDMMC related commands for the console application.
 */
#include <stdio.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_console.h"
#include "soc/soc_caps.h"
#include "sd_protocol_defs.h"
#include "sd_protocol_types.h"
#include "sdmmc_cmd.h"
#ifdef SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif // SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdspi_host.h"
#include "argtable3/argtable3.h"
#include "sdmmc_test_board.h"
#include "cmd_sdmmc.h"


static const char* TAG = "cmd_sdmmc";
static sdmmc_host_t s_host;
static sdmmc_card_t s_card;


/** ========== SDMMC host related commands ========== */
#ifdef SOC_SDMMC_HOST_SUPPORTED

static void register_sdmmc_host_init(void);
static void register_sdmmc_host_deinit(void);

void register_sdmmc_host(void)
{
    register_sdmmc_host_init();
    register_sdmmc_host_deinit();
}

/** ========== sdmmc_host_init command ========== */

static struct {
    struct arg_int *slot;
    struct arg_lit *hs;
    struct arg_lit *ddr;
    struct arg_int *freq;
    struct arg_int *width;
    struct arg_end *end;
} sdmmc_host_init_args;

static int sdmmc_host_init_handler(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &sdmmc_host_init_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, sdmmc_host_init_args.end, argv[0]);
        return 1;
    }

    esp_err_t err = sdmmc_host_init();
    if (err == ESP_ERR_INVALID_STATE) {
        /* already initialized */
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_init: error 0x%x (%s)", err, esp_err_to_name(err));
        return 1;
    }

    int slot = 1;
    if (sdmmc_host_init_args.slot->count > 0) {
        slot = sdmmc_host_init_args.slot->ival[0];
    }

    s_host = (sdmmc_host_t) SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_test_board_get_config_sdmmc(slot, &s_host, &slot_config);

    if (sdmmc_host_init_args.hs->count > 0) {
        s_host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    }
    if (sdmmc_host_init_args.ddr->count > 0) {
        s_host.flags |= SDMMC_HOST_FLAG_DDR;
    } else {
        s_host.flags &= ~SDMMC_HOST_FLAG_DDR;
    }
    if (sdmmc_host_init_args.freq->count > 0) {
        s_host.max_freq_khz = sdmmc_host_init_args.freq->ival[0];
    }
    if (sdmmc_host_init_args.width->count > 0) {
        slot_config.width = sdmmc_host_init_args.width->ival[0];
    }
    err = sdmmc_host_init_slot(slot, &slot_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_init_slot: error 0x%x (%s)", err, esp_err_to_name(err));
        return 1;
    }

    return 0;
}

static void register_sdmmc_host_init(void)
{
    sdmmc_host_init_args.slot = arg_int0("s", "slot", "<0|1>", "SDMMC slot number");
    sdmmc_host_init_args.hs = arg_lit0(NULL, "hs", "High-speed mode");
    sdmmc_host_init_args.ddr = arg_lit0(NULL, "ddr", "DDR mode");
    sdmmc_host_init_args.freq = arg_int0("f", "freq", "<kHz>", "Bus frequency in kHz");
    sdmmc_host_init_args.width = arg_int0("w", "width", "<1|4|8>", "Bus width (1, 4, or 8 lines)");
    sdmmc_host_init_args.end = arg_end(5);

    const esp_console_cmd_t cmd = {
        .command = "sdmmc_host_init",
        .help = "Initialize SDMMC host driver",
        .hint = NULL,
        .func = &sdmmc_host_init_handler,
        .argtable = &sdmmc_host_init_args
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** ========== sdmmc_host_deinit command ========== */

static int sdmmc_host_deinit_handler(int argc, char **argv)
{
    esp_err_t err = sdmmc_host_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "sdmmc_host_deinit: error 0x%x (%s)", err, esp_err_to_name(err));
        return 1;
    }
    return 0;
}

static void register_sdmmc_host_deinit(void)
{
    const esp_console_cmd_t cmd = {
        .command = "sdmmc_host_deinit",
        .help = "Deinitialize SDMMC host driver",
        .hint = NULL,
        .func = &sdmmc_host_deinit_handler
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

#endif // SOC_SDMMC_HOST_SUPPORTED
/** ========== end of SDMMC host related commands ========== */

/** ========== SDSPI host related commands ========== */

/* TODO */

/** ========== end of SDSPI host related commands ========== */

/** ========== Common sdmmc protocol commands ========== */

static void register_sdmmc_card_init(void);
static void register_sdmmc_card_info(void);

void register_sdmmc_common(void)
{
    register_sdmmc_card_init();
    register_sdmmc_card_info();
}

static int sdmmc_card_init_handler(int argc, char **argv)
{
    /* In case the card was already initialized: reset the host to the default settings.
     * This could be moved to sdmmc_card_init.
     */
    esp_err_t err = s_host.set_card_clk(s_host.slot, SDMMC_FREQ_PROBING);
    ESP_RETURN_ON_ERROR(err, TAG, "set_card_clk: error 0x%x (%s)", err, esp_err_to_name(err));
    s_host.set_bus_width(s_host.slot, 1);
    ESP_RETURN_ON_ERROR(err, TAG, "set_bus_width: error 0x%x (%s)", err, esp_err_to_name(err));
    s_host.set_bus_ddr_mode(s_host.slot, false);
    ESP_RETURN_ON_ERROR(err, TAG, "set_bus_ddr_mode: error 0x%x (%s)", err, esp_err_to_name(err));

    err = sdmmc_card_init(&s_host, &s_card);
    ESP_RETURN_ON_ERROR(err, TAG, "sdmmc_card_init: error 0x%x (%s)", err, esp_err_to_name(err));
    return 0;
}

static void register_sdmmc_card_init(void)
{
    const esp_console_cmd_t cmd = {
        .command = "card_init",
        .help = "Initialize the card using sdmmc_card_init function",
        .hint = NULL,
        .func = &sdmmc_card_init_handler
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

static int sdmmc_card_info_handler(int argc, char** argv)
{
    sdmmc_card_print_info(stdout, &s_card);
    return 0;
}

static void register_sdmmc_card_info(void)
{
    const esp_console_cmd_t cmd = {
        .command = "card_info",
        .help = "Print information about the card to the console",
        .hint = NULL,
        .func = &sdmmc_card_info_handler
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/** ========== End of common sdmmc protocol commands ========== */
