/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "esp_log.h"
#include "sdmmc_test_board.h"
#include "driver/sdspi_host.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_spi.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

void sdmmc_test_spi_skip_if_board_incompatible(int slot, int freq_khz)
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    spi_bus_config_t bus_config = {};

    if (!sdmmc_test_board_has_slot(slot)) {
        TEST_IGNORE_MESSAGE("Board doesn't have the required slot");
    }
    if (sdmmc_test_board_slot_is_emmc(slot)) {
        TEST_IGNORE_MESSAGE("Can't run SPI mode test on a slot with eMMC");
    }
    sdmmc_test_board_get_config_sdspi(slot, &config, &bus_config, &dev_config);
    int board_max_freq_khz = sdmmc_test_board_get_slot_info(slot)->max_freq_khz;
    if (board_max_freq_khz > 0 && board_max_freq_khz < freq_khz) {
        TEST_IGNORE_MESSAGE("Board doesn't support required max_freq_khz");
    }

}

void sdmmc_test_spi_begin(int slot, int freq_khz, sdmmc_card_t *out_card)
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    spi_bus_config_t bus_config = {};
    sdspi_dev_handle_t handle;

    /* Similar to the checks in sdmmc_test_spi_skip_if_board_incompatible, but
     * we fail the test if we somehow got to this point with an incompatible board.
     */
    if (!sdmmc_test_board_has_slot(slot)) {
        TEST_FAIL_MESSAGE("Board doesn't have the required slot");
    }
    if (sdmmc_test_board_slot_is_emmc(slot)) {
        TEST_FAIL_MESSAGE("Can't run SPI mode test on a slot with eMMC");
    }
    sdmmc_test_board_get_config_sdspi(slot, &config, &bus_config, &dev_config);
    int board_max_freq_khz = sdmmc_test_board_get_slot_info(slot)->max_freq_khz;
    if (board_max_freq_khz > 0 && board_max_freq_khz < freq_khz) {
        TEST_FAIL_MESSAGE("Board doesn't support required max_freq_khz");
    }

    config.max_freq_khz = freq_khz;

#if SOC_SDMMC_IO_POWER_EXTERNAL
#define SDMMC_PWR_LDO_CHANNEL   4
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = SDMMC_PWR_LDO_CHANNEL,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    TEST_ESP_OK(sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle));
    config.pwr_ctrl_handle = pwr_ctrl_handle;
#endif

    sdmmc_test_board_card_power_set(true);
    TEST_ESP_OK(spi_bus_initialize(dev_config.host_id, &bus_config, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));
    TEST_ESP_OK(sdmmc_card_init(&config, out_card));
}

void sdmmc_test_spi_end(int slot, sdmmc_card_t *card)
{
    TEST_ESP_OK(sdspi_host_deinit());
    TEST_ESP_OK(spi_bus_free(SDSPI_DEFAULT_HOST));

    const sdmmc_test_board_slot_info_t* slot_info = sdmmc_test_board_get_slot_info(slot);
    const int pins[] = {
        slot_info->clk,
        slot_info->cmd_mosi,
        slot_info->d0_miso,
        slot_info->d3_cs,
    };
    const int num_pins = sizeof(pins) / sizeof(pins[0]);
    // Silence logging in gpio_reset_pin, which logs at INFO level
    esp_log_level_t old_level = esp_log_level_get("gpio");
    esp_log_level_set("gpio", ESP_LOG_WARN);
    for (int i = 0; i < num_pins; i++) {
        if (pins[i] >= 0) {
            gpio_reset_pin(pins[i]);
            gpio_pullup_dis(pins[i]);
        }
    }
    esp_log_level_set("gpio", old_level);

    sdmmc_test_board_card_power_set(false);

#if SOC_SDMMC_IO_POWER_EXTERNAL
    TEST_ESP_OK(sd_pwr_ctrl_del_on_chip_ldo(card->host.pwr_ctrl_handle));
#endif
}
