/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdmmc_test_board.h"
#include "sdkconfig.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "sd_pwr_ctrl.h"

const sdmmc_test_board_slot_info_t* sdmmc_test_board_get_slot_info(int slot_index)
{
    assert(slot_index >= 0 && slot_index < SDMMC_TEST_BOARD_MAX_SLOTS);
    return &sdmmc_test_board_get_info()->slot[slot_index];
}

#if SOC_SDMMC_HOST_SUPPORTED

void sdmmc_test_board_get_config_sdmmc(int slot_index, sdmmc_host_t *out_host_config, sdmmc_slot_config_t *out_slot_config)
{
    const sdmmc_test_board_slot_info_t* slot = sdmmc_test_board_get_slot_info(slot_index);
    assert(slot->slot_exists);

    out_host_config->slot = slot_index;
    if (slot->bus_width < 8) {
        out_host_config->flags &= ~SDMMC_HOST_FLAG_8BIT;
    }
    if (slot->bus_width < 4) {
        out_host_config->flags &= ~SDMMC_HOST_FLAG_4BIT;
    }
    if (slot->max_freq_khz > 0) {
        out_host_config->max_freq_khz = slot->max_freq_khz;
    }
    if (slot->uhs1_supported) {
        out_slot_config->flags |= SDMMC_SLOT_FLAG_UHS1;
    }

#if SOC_SDMMC_USE_GPIO_MATRIX
    out_slot_config->clk = slot->clk;
    out_slot_config->cmd = slot->cmd_mosi;
    out_slot_config->d0  = slot->d0_miso;
    out_slot_config->d1  = slot->d1;
    out_slot_config->d2  = slot->d2;
    out_slot_config->d3  = slot->d3_cs;
    out_slot_config->d4  = slot->d4;
    out_slot_config->d5  = slot->d5;
    out_slot_config->d6  = slot->d6;
    out_slot_config->d7  = slot->d7;
#endif // SOC_SDMMC_USE_GPIO_MATRIX
    out_slot_config->wp  = slot->wp;
    out_slot_config->cd  = slot->cd;
    out_slot_config->width = slot->bus_width;
}

#endif // SOC_SDMMC_HOST_SUPPORTED

void sdmmc_test_board_get_config_sdspi(int slot_index, sdmmc_host_t *out_host_config, spi_bus_config_t *out_spi_bus_config, sdspi_device_config_t *out_dev_config)
{
    const sdmmc_test_board_slot_info_t* slot = sdmmc_test_board_get_slot_info(slot_index);
    assert(slot->slot_exists);

    if (slot->max_freq_khz > 0) {
        out_host_config->max_freq_khz = slot->max_freq_khz;
    }

    out_spi_bus_config->mosi_io_num = slot->cmd_mosi;
    out_spi_bus_config->miso_io_num = slot->d0_miso;
    out_spi_bus_config->sclk_io_num = slot->clk;
    out_dev_config->gpio_cs = slot->d3_cs;
    out_dev_config->gpio_cd = slot->cd;
    out_dev_config->gpio_wp = slot->wp;
}

void sdmmc_test_board_card_power_set(bool enable)
{
    const sdmmc_test_board_info_t* board = sdmmc_test_board_get_info();
    if (board->card_power_set != NULL) {
        board->card_power_set(enable);
    }
}

bool sdmmc_test_board_has_slot(int slot_index)
{
    return sdmmc_test_board_get_slot_info(slot_index)->slot_exists;
}

bool sdmmc_test_board_slot_is_emmc(int slot_index)
{
    return sdmmc_test_board_get_slot_info(slot_index)->is_emmc;
}

void sdmmc_test_board_print_info()
{
    const sdmmc_test_board_info_t *board = sdmmc_test_board_get_info();
    printf("\nTest app built for %s\n", board->name);
    for (int slot_index = 0; slot_index < SDMMC_TEST_BOARD_MAX_SLOTS; ++slot_index) {
        const sdmmc_test_board_slot_info_t *slot = sdmmc_test_board_get_slot_info(slot_index);
        if (!slot->slot_exists) {
            printf("- Slot %d: not present\n", slot_index);
            continue;
        }
        printf("- Slot %d: %d-bit", slot_index, slot->bus_width);
        if (slot->max_freq_khz > 0) {
            printf(", freq limit: %d kHz", slot->max_freq_khz);
        }
        if (slot->is_emmc > 0) {
            printf(", eMMC");
        }
        printf("\n");
        printf("    CMD/MOSI: %2d\n    CLK:      %2d\n    D0/MISO:  %2d\n", slot->cmd_mosi, slot->clk, slot->d0_miso);
        if (slot->bus_width > 1 || slot->d3_cs) {
            printf("    D1:       %2d\n    D2:       %2d\n    D3/CS:    %2d\n", slot->d1, slot->d2, slot->d3_cs);
        }
        if (slot->bus_width > 4) {
            printf("    D4:       %2d\n    D5:       %2d\n    D6:       %2d\n    D7:       %2d\n", slot->d4, slot->d5, slot->d6, slot->d7);
        }
    }
}
