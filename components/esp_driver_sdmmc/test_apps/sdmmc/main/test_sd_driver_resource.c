/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/sd_host_sdmmc.h"
#include "driver/sd_host.h"
#include "hal/sdmmc_ll.h"
#include "soc/sdmmc_pins.h"

/*---------------------------------------------------------------
                      SDMMC
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32
#define SDMMC_SLOT0_CLK    SDMMC_SLOT0_IOMUX_PIN_NUM_CLK
#define SDMMC_SLOT0_CMD    SDMMC_SLOT0_IOMUX_PIN_NUM_CMD
#define SDMMC_SLOT0_D0     SDMMC_SLOT0_IOMUX_PIN_NUM_D0
#define SDMMC_SLOT0_D1     SDMMC_SLOT0_IOMUX_PIN_NUM_D1
#define SDMMC_SLOT0_D2     SDMMC_SLOT0_IOMUX_PIN_NUM_D2
#define SDMMC_SLOT0_D3     SDMMC_SLOT0_IOMUX_PIN_NUM_D3
#define SDMMC_SLOT1_CLK    SDMMC_SLOT1_IOMUX_PIN_NUM_CLK
#define SDMMC_SLOT1_CMD    SDMMC_SLOT1_IOMUX_PIN_NUM_CMD
#define SDMMC_SLOT1_D0     SDMMC_SLOT1_IOMUX_PIN_NUM_D0
#define SDMMC_SLOT1_D1     SDMMC_SLOT1_IOMUX_PIN_NUM_D1
#define SDMMC_SLOT1_D2     SDMMC_SLOT1_IOMUX_PIN_NUM_D2
#define SDMMC_SLOT1_D3     SDMMC_SLOT1_IOMUX_PIN_NUM_D3
#elif CONFIG_IDF_TARGET_ESP32S3
#define SDMMC_SLOT0_CLK    10
#define SDMMC_SLOT0_CMD    11
#define SDMMC_SLOT0_D0     12
#define SDMMC_SLOT0_D1     13
#define SDMMC_SLOT0_D2     14
#define SDMMC_SLOT0_D3     15
#define SDMMC_SLOT1_CLK    10
#define SDMMC_SLOT1_CMD    11
#define SDMMC_SLOT1_D0     12
#define SDMMC_SLOT1_D1     13
#define SDMMC_SLOT1_D2     14
#define SDMMC_SLOT1_D3     15
#elif CONFIG_IDF_TARGET_ESP32P4
#define SDMMC_SLOT0_CLK    SDMMC_SLOT0_IOMUX_PIN_NUM_CLK
#define SDMMC_SLOT0_CMD    SDMMC_SLOT0_IOMUX_PIN_NUM_CMD
#define SDMMC_SLOT0_D0     SDMMC_SLOT0_IOMUX_PIN_NUM_D0
#define SDMMC_SLOT0_D1     SDMMC_SLOT0_IOMUX_PIN_NUM_D1
#define SDMMC_SLOT0_D2     SDMMC_SLOT0_IOMUX_PIN_NUM_D2
#define SDMMC_SLOT0_D3     SDMMC_SLOT0_IOMUX_PIN_NUM_D3
#define SDMMC_SLOT1_CLK    10
#define SDMMC_SLOT1_CMD    11
#define SDMMC_SLOT1_D0     12
#define SDMMC_SLOT1_D1     13
#define SDMMC_SLOT1_D2     14
#define SDMMC_SLOT1_D3     15
#endif

TEST_CASE("SDMMC controller exhausted allocation", "[sdmmc]")
{
    sd_host_sdmmc_cfg_t cfg = {
        .event_queue_items = 4,
    };
    sd_host_ctlr_handle_t ctlr[SDMMC_LL_HOST_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SDMMC_LL_HOST_CTLR_NUMS; i++) {
        TEST_ESP_OK(sd_host_create_sdmmc_controller(&cfg, &ctlr[i]));
    }

    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, sd_host_create_sdmmc_controller(&cfg, &ctlr[SDMMC_LL_HOST_CTLR_NUMS]));

    for (int i = 0; i < SDMMC_LL_HOST_CTLR_NUMS; i++) {
        TEST_ESP_OK(sd_host_del_controller(ctlr[i]));
    }
}

TEST_CASE("SDMMC slot exhausted allocation", "[sdmmc]")
{
    sd_host_sdmmc_cfg_t cfg = {
        .event_queue_items = 4,
    };
    sd_host_ctlr_handle_t ctlr = NULL;
    TEST_ESP_OK(sd_host_create_sdmmc_controller(&cfg, &ctlr));

    sd_host_slot_sdmmc_init_cfg_t slot_cfg = {
        .slot_id = 0,
        .sd_mode = SD_MODE_NORMAL,
        .io_config = {
            .width = 1,
            .clk_io = SDMMC_SLOT0_CLK,
            .cmd_io = SDMMC_SLOT0_CMD,
            .d0_io = SDMMC_SLOT0_D0,
            .d1_io = SDMMC_SLOT0_D1,
            .d2_io = SDMMC_SLOT0_D2,
            .d3_io = SDMMC_SLOT0_D3,
        }
    };
    sd_host_slot_handle_t slot[SOC_SDMMC_NUM_SLOTS] = {};

#if !CONFIG_IDF_TARGET_ESP32
    //Slot 0 on the ESP32 overlaps with the default SPI Flash pins
    TEST_ESP_OK(sd_host_sdmmc_controller_add_slot(ctlr, &slot_cfg, &slot[0]));
#endif

    slot_cfg.slot_id = 1;
    slot_cfg.io_config.clk_io = SDMMC_SLOT1_CLK;
    slot_cfg.io_config.cmd_io = SDMMC_SLOT1_CMD;
    slot_cfg.io_config.d0_io = SDMMC_SLOT1_D0;
    slot_cfg.io_config.d1_io = SDMMC_SLOT1_D1;
    slot_cfg.io_config.d2_io = SDMMC_SLOT1_D2;
    slot_cfg.io_config.d3_io = SDMMC_SLOT1_D3;
    TEST_ESP_OK(sd_host_sdmmc_controller_add_slot(ctlr, &slot_cfg, &slot[1]));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sd_host_sdmmc_controller_add_slot(ctlr, &slot_cfg, &slot[SOC_SDMMC_NUM_SLOTS]));

#if !CONFIG_IDF_TARGET_ESP32
    //Slot 0 on the ESP32 overlaps with the default SPI Flash pins
    TEST_ESP_OK(sd_host_remove_slot(slot[0]));
#endif
    TEST_ESP_OK(sd_host_remove_slot(slot[1]));
    TEST_ESP_OK(sd_host_del_controller(ctlr));
}
