/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_board.h"
#include "sdmmc_test_begin_end_spi.h"
#include "sdmmc_test_cd_wp_common.h"
#include "sd_pwr_ctrl.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

TEST_CASE("CD input works in SPI mode", "[sdspi]")
{
    const int slot = 1;
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_config = {};
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    sdmmc_test_board_get_config_sdspi(slot, &config, &bus_config, &dev_config);
    const int test_gpio = sdmmc_test_board_get_slot_info(slot)->unused_pin;
    dev_config.gpio_cd = test_gpio;

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

    config.slot = handle;

    sdmmc_test_cd_input(test_gpio, &config);

    TEST_ESP_OK(sdspi_host_remove_device(handle));
    TEST_ESP_OK(sdspi_host_deinit());
    TEST_ESP_OK(spi_bus_free(SDSPI_DEFAULT_HOST));
    sdmmc_test_board_card_power_set(false);
#if SOC_SDMMC_IO_POWER_EXTERNAL
    TEST_ESP_OK(sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle));
#endif
}

TEST_CASE("WP input works in SPI mode", "[sdspi]")
{
    const int slot = 1;
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_config = {};
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    sdmmc_test_board_get_config_sdspi(slot, &config, &bus_config, &dev_config);
    const int test_gpio = sdmmc_test_board_get_slot_info(slot)->unused_pin;
    dev_config.gpio_wp = test_gpio;
    sdmmc_test_board_card_power_set(true);
    TEST_ESP_OK(spi_bus_initialize(dev_config.host_id, &bus_config, SPI_DMA_CH_AUTO));
#if SOC_SDMMC_IO_POWER_EXTERNAL
#define SDMMC_PWR_LDO_CHANNEL   4
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = SDMMC_PWR_LDO_CHANNEL,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    TEST_ESP_OK(sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle));
    config.pwr_ctrl_handle = pwr_ctrl_handle;
#endif
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    config.slot = handle;

    sdmmc_test_wp_input(test_gpio, &config);

    TEST_ESP_OK(sdspi_host_remove_device(handle));
    TEST_ESP_OK(sdspi_host_deinit());
    TEST_ESP_OK(spi_bus_free(SDSPI_DEFAULT_HOST));
    sdmmc_test_board_card_power_set(false);
#if SOC_SDMMC_IO_POWER_EXTERNAL
    TEST_ESP_OK(sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle));
#endif
}
