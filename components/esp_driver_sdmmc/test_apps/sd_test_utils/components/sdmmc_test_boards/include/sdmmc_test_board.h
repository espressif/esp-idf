/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "driver/sdspi_host.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif
#include "driver/gpio.h"
#include "driver/spi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sdmmc_test.h
 * This file contains APIs to get the board specific information (pin mappings) for SDMMC tests.
 */

/**
 * @brief Information about a slot on the test board
 */
typedef struct {
    bool slot_exists;   /*< Whether the slot exists on the dev board */
    bool is_emmc;       /*< True if this slot is connected to eMMC */
    int  bus_width;     /*< SD interface width (1, 4 or 8) */
    int  max_freq_khz;  /*< Max frequency (kHz) of SD interface, supported by the board */
    gpio_num_t clk;     /*< CLK pin number */
    gpio_num_t cmd_mosi;    /*< CMD pin number (SD mode) or MOSI (SPI mode) */
    gpio_num_t d0_miso; /*< D0 pin number (SD mode) or MISO (SPI mode) */
    gpio_num_t d1;      /*< D1 pin number */
    gpio_num_t d2;      /*< D2 pin number */
    gpio_num_t d3_cs;   /*< D3 pin number (SD mode) or CS (SPI mode) */
    gpio_num_t d4;      /*< D4 pin number */
    gpio_num_t d5;      /*< D5 pin number */
    gpio_num_t d6;      /*< D6 pin number */
    gpio_num_t d7;      /*< D7 pin number */
    gpio_num_t cd;      /*< Card detect pin number */
    gpio_num_t wp;      /*< Write protect pin number */
    gpio_num_t unused_pin;      /*< Pin not routed on the board, for CD/WP loopback tests */
} sdmmc_test_board_slot_info_t;

#define SDMMC_TEST_BOARD_MAX_SLOTS  2   /*< Number of slots we need to support on one test board */

typedef struct {
    const char* name;                                                   /*< name of the board */
    sdmmc_test_board_slot_info_t slot[SDMMC_TEST_BOARD_MAX_SLOTS];      /*< array of information about slots */
    void (*card_power_set)(bool);                                       /*< function to turn card power on or off */
} sdmmc_test_board_info_t;

/**
 * @brief Get information about the test board
 * @return Pointer to the board information structure
 */
const sdmmc_test_board_info_t* sdmmc_test_board_get_info(void);

/**
 * @brief Get information about a slot on the test board
 * @param slot_index Index of the slot (0 to SDMMC_TEST_BOARD_MAX_SLOTS-1)
 * @return Pointer to the slot information structure
 */
const sdmmc_test_board_slot_info_t* sdmmc_test_board_get_slot_info(int slot_index);

#if SOC_SDMMC_HOST_SUPPORTED
/**
 * @brief Fill SDMMC host and slot configuration structures with information about a slot on the test board
 * @note Before calling this function, initialize the host/slot configuration structures using default initializer macros.
 * @param slot_index Index of the slot (0 to SDMMC_TEST_BOARD_MAX_SLOTS-1)
 * @param[out] out_host_config  Output, pointer to the host configuration structure to be filled.
 * @param[out] out_slot_config  Output, pointer to the slot configuration structure to be filled.
 */
void sdmmc_test_board_get_config_sdmmc(int slot_index, sdmmc_host_t *out_host_config, sdmmc_slot_config_t *out_slot_config);
#endif

/**
 * @brief Fill SDSPI host, bus and device configuration structures with information about a slot on the test board
 * @note Before calling this function, initialize the these configuration structures using default initializer macros.
 * @param slot_index  Index of the slot (0 to SDMMC_TEST_BOARD_MAX_SLOTS-1)
 * @param out_host_config  Output, pointer to the host configuration structure to be filled.
 * @param out_spi_bus_config  Output, pointer to the SPI bus configuration structure to be filled.
 * @param out_dev_config  Output, pointer to the device configuration structure to be filled.
 */
void sdmmc_test_board_get_config_sdspi(int slot_index, sdmmc_host_t *out_host_config,
                                       spi_bus_config_t *out_spi_bus_config, sdspi_device_config_t *out_dev_config);

/**
 * @brief Set card power on or off
 * For boards with card power control circuit, this function allows powering the card up or down.
 * @param enable  True to turn power on, false to turn power off
 */
void sdmmc_test_board_card_power_set(bool enable);

/**
 * @brief Check if a slot exists on the test board
 * "Exists" here means that something (card slot or eMMC) is connected to that slot.
 * @param slot_index  Index of the slot (0 to SDMMC_TEST_BOARD_MAX_SLOTS-1)
 * @return  True if the slot exists on the board
 */
bool sdmmc_test_board_has_slot(int slot_index);

/**
 * @brief Check if a slot is connected to eMMC
 *
 * Note that this function simply returns what the board definitions in sdmmc_test_board_defs.c say.
 * It does not check if the card is actually eMMC.
 * The main purpose of this function is to prevent SPI-related tests
 * from being run on slots which are known to be connected to eMMC.
 *
 * @param slot_index  Index of the slot (0 to SDMMC_TEST_BOARD_MAX_SLOTS-1)
 * @return  True if the slot is connected to eMMC
 */
bool sdmmc_test_board_slot_is_emmc(int slot_index);

/**
 * @brief Print information about the test board (pin mapping)
 */
void sdmmc_test_board_print_info(void);

#ifdef __cplusplus
}
#endif
