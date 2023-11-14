/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include "sdmmc_test_board.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "soc/sdmmc_pins.h"
#endif

#if CONFIG_SDMMC_BOARD_ESP32_WROVER_KIT

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32-WROVER-KIT",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = 4,
            .clk = SDMMC_SLOT1_IOMUX_PIN_NUM_CLK,
            .cmd_mosi = SDMMC_SLOT1_IOMUX_PIN_NUM_CMD,
            .d0_miso = SDMMC_SLOT1_IOMUX_PIN_NUM_D0,
            .d1 = SDMMC_SLOT1_IOMUX_PIN_NUM_D1,
            .d2 = SDMMC_SLOT1_IOMUX_PIN_NUM_D2,
            .d3_cs = SDMMC_SLOT1_IOMUX_PIN_NUM_D3,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 18,
        }
    },
};

#elif CONFIG_SDMMC_BOARD_ESP32_EMMC_TEST

#define SD_TEST_BOARD_VSEL_GPIO     26
#define SD_TEST_BOARD_VSEL_3V3      1
#define SD_TEST_BOARD_VSEL_1V8      0
#define SD_TEST_BOARD_EN_GPIO  27
#define SD_TEST_BOARD_EN_LEVEL 1
#define SD_TEST_BOARD_PWR_RST_DELAY_MS  5
#define SD_TEST_BOARD_PWR_ON_DELAY_MS   50

static void card_power_set_esp32_emmc(bool en)
{
    if (en) {
        /* set voltage */
        gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, SD_TEST_BOARD_VSEL_3V3);
        /* power off to make sure card is reset */
        gpio_set_direction(SD_TEST_BOARD_EN_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, !SD_TEST_BOARD_EN_LEVEL);
        usleep(SD_TEST_BOARD_PWR_RST_DELAY_MS * 1000);
        /* power on */
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, SD_TEST_BOARD_EN_LEVEL);
        usleep(SD_TEST_BOARD_PWR_ON_DELAY_MS * 1000);
    } else {
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, !SD_TEST_BOARD_EN_LEVEL);
        gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, 0);
        gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_INPUT);
        gpio_set_direction(SD_TEST_BOARD_EN_GPIO, GPIO_MODE_INPUT);
    }
}

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32 eMMC test board v1",
    .slot = {
        {
            .slot_exists = true,
            .is_emmc = true,
            .bus_width = 8,
            .clk = SDMMC_SLOT0_IOMUX_PIN_NUM_CLK,
            .cmd_mosi = SDMMC_SLOT0_IOMUX_PIN_NUM_CMD,
            .d0_miso = SDMMC_SLOT0_IOMUX_PIN_NUM_D0,
            .d1 = SDMMC_SLOT0_IOMUX_PIN_NUM_D1,
            .d2 = SDMMC_SLOT0_IOMUX_PIN_NUM_D2,
            .d3_cs = SDMMC_SLOT0_IOMUX_PIN_NUM_D3,
            .d4 = SDMMC_SLOT0_IOMUX_PIN_NUM_D4,
            .d5 = SDMMC_SLOT0_IOMUX_PIN_NUM_D5,
            .d6 = SDMMC_SLOT0_IOMUX_PIN_NUM_D6,
            .d7 = SDMMC_SLOT0_IOMUX_PIN_NUM_D7,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 18,
        },
        {
            .slot_exists = true,
            .bus_width = 4,
            .clk = SDMMC_SLOT1_IOMUX_PIN_NUM_CLK,
            .cmd_mosi = SDMMC_SLOT1_IOMUX_PIN_NUM_CMD,
            .d0_miso = SDMMC_SLOT1_IOMUX_PIN_NUM_D0,
            .d1 = SDMMC_SLOT1_IOMUX_PIN_NUM_D1,
            .d2 = SDMMC_SLOT1_IOMUX_PIN_NUM_D2,
            .d3_cs = SDMMC_SLOT1_IOMUX_PIN_NUM_D3,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 18,
        }
    },
    .card_power_set = card_power_set_esp32_emmc
};
#elif CONFIG_SDMMC_BOARD_ESP32S2_S3_USB_OTG

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32-S2/ESP32-S3 USB_OTG",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = 4,
            .clk = 36,
            .cmd_mosi = 35,
            .d0_miso = 37,
            .d1 = 38,
            .d2 = 33,
            .d3_cs = 34,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 2,
        }
    },
};

#elif CONFIG_SDMMC_BOARD_ESP32S3_EMMC_TEST

#define SD_TEST_BOARD_EN_GPIO  47
#define SD_TEST_BOARD_EN_LEVEL 0
// Pin pulled down to discharge VDD_SDIO capacitors. CMD pin used here.
#define SD_TEST_BOARD_DISCHARGE_GPIO 4
#define SD_TEST_BOARD_PWR_RST_DELAY_MS  100
#define SD_TEST_BOARD_PWR_ON_DELAY_MS   100

static void card_power_set_esp32s3_emmc(bool en)
{
    if (en) {
        /* power off to make sure the card is reset */
        gpio_reset_pin(SD_TEST_BOARD_EN_GPIO);
        gpio_set_direction(SD_TEST_BOARD_EN_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, !SD_TEST_BOARD_EN_LEVEL);
        /* discharge capacitors on VDD_SDIO */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_direction(SD_TEST_BOARD_DISCHARGE_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_DISCHARGE_GPIO, 0);
        usleep(SD_TEST_BOARD_PWR_RST_DELAY_MS * 1000);
        /* power on */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, SD_TEST_BOARD_EN_LEVEL);
        usleep(SD_TEST_BOARD_PWR_ON_DELAY_MS * 1000);
    } else {
        /* power off the card */
        gpio_set_level(SD_TEST_BOARD_EN_GPIO, !SD_TEST_BOARD_EN_LEVEL);
        gpio_set_direction(SD_TEST_BOARD_EN_GPIO, GPIO_MODE_INPUT);
        /* discharge capacitors on VDD_SDIO */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_direction(SD_TEST_BOARD_DISCHARGE_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_DISCHARGE_GPIO, 0);
        usleep(SD_TEST_BOARD_PWR_RST_DELAY_MS * 1000);
        /* reset the pin but leaving it floating so that VDD_SDIO won't be charged again */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_pullup_dis(SD_TEST_BOARD_DISCHARGE_GPIO);
    }
}

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32-S3 eMMC test board v1",
    .slot = {
        {
            .slot_exists = true,
            .is_emmc = true,
            .bus_width = 8,
            .clk = 34,
            .cmd_mosi = 33,
            .d0_miso = 37,
            .d1 = 38,
            .d2 = 39,
            .d3_cs = 36,
            .d4 = 35,
            .d5 = 40,
            .d6 = 42,
            .d7 = 41,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 1,
        },
        {
            .slot_exists = true,
            .bus_width = 4,
            .clk = 2,
            .cmd_mosi = 4,
            .d0_miso = 5,
            .d1 = 6,
            .d2 = 7,
            .d3_cs = 8,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = 21,
            .wp = GPIO_NUM_NC,
            .unused_pin = 1,
        }
    },
    .card_power_set = card_power_set_esp32s3_emmc
};

#elif CONFIG_SDMMC_BOARD_ESP32S3_EYE

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32-S3-EYE",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = 1,
            .clk = 39,
            .cmd_mosi = 38,
            .d0_miso = 40,
            .d1 = GPIO_NUM_NC,
            .d2 = GPIO_NUM_NC,
            .d3_cs = GPIO_NUM_NC,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 1,
        }
    },
};

#elif CONFIG_SDMMC_BOARD_ESP32C3_BREAKOUT

#define SD_BREAKOUT_BOARD_EN_GPIO  10
#define SD_BREAKOUT_BOARD_EN_LEVEL 0
// Pin pulled down to discharge VDD_SDIO capacitors. CMD pin used here.
#define SD_TEST_BOARD_DISCHARGE_GPIO 4
#define SD_BREAKOUT_BOARD_PWR_RST_DELAY_MS  100
#define SD_BREAKOUT_BOARD_PWR_ON_DELAY_MS   100

static void card_power_set_esp32c3_breakout(bool en)
{
    if (en) {
        /* power off to make sure card is reset */
        gpio_reset_pin(SD_BREAKOUT_BOARD_EN_GPIO);
        gpio_set_direction(SD_BREAKOUT_BOARD_EN_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_BREAKOUT_BOARD_EN_GPIO, !SD_BREAKOUT_BOARD_EN_LEVEL);
        /* discharge capacitors on VDD_SDIO */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_direction(SD_TEST_BOARD_DISCHARGE_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_DISCHARGE_GPIO, 0);
        usleep(SD_BREAKOUT_BOARD_PWR_RST_DELAY_MS * 1000);
        /* power on */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_level(SD_BREAKOUT_BOARD_EN_GPIO, SD_BREAKOUT_BOARD_EN_LEVEL);
        usleep(SD_BREAKOUT_BOARD_PWR_ON_DELAY_MS * 1000);
    } else {
        /* power off the card */
        gpio_set_level(SD_BREAKOUT_BOARD_EN_GPIO, !SD_BREAKOUT_BOARD_EN_LEVEL);
        gpio_set_direction(SD_BREAKOUT_BOARD_EN_GPIO, GPIO_MODE_INPUT);
        /* set CMD low to discharge capacitors on VDD_SDIO */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_set_direction(SD_TEST_BOARD_DISCHARGE_GPIO, GPIO_MODE_OUTPUT);
        gpio_set_level(SD_TEST_BOARD_DISCHARGE_GPIO, 0);
        usleep(SD_BREAKOUT_BOARD_PWR_RST_DELAY_MS * 1000);
        /* reset the pin but leaving it floating so that VDD_SDIO won't be charged again */
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
        gpio_reset_pin(SD_TEST_BOARD_DISCHARGE_GPIO);
    }
}

static const sdmmc_test_board_info_t s_board_info = {
    .name = "ESP32-C3 breakout board",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = 1,
            .clk = 5,
            .cmd_mosi = 4,
            .d0_miso = 6,
            .d1 = GPIO_NUM_NC,
            .d2 = GPIO_NUM_NC,
            .d3_cs = 1,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = GPIO_NUM_NC,
            .wp = GPIO_NUM_NC,
            .unused_pin = 2,
        }
    },
    .card_power_set = card_power_set_esp32c3_breakout
};

#elif CONFIG_SDMMC_BOARD_CUSTOM_SD

static const sdmmc_test_board_info_t s_board_info = {
    .name = "Custom defined board (SD)",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = CONFIG_SDMMC_BOARD_CUSTOM_BUS_WIDTH,
#ifdef SOC_SDMMC_USE_GPIO_MATRIX
            .clk = CONFIG_SDMMC_BOARD_CUSTOM_CLK,
            .cmd_mosi = CONFIG_SDMMC_BOARD_CUSTOM_CMD,
            .d0_miso = CONFIG_SDMMC_BOARD_CUSTOM_D0,
            .d1 = CONFIG_SDMMC_BOARD_CUSTOM_D1,
            .d2 = CONFIG_SDMMC_BOARD_CUSTOM_D2,
            .d3_cs = CONFIG_SDMMC_BOARD_CUSTOM_D3,
#else // SOC_SDMMC_USE_GPIO_MATRIX
            .clk = SDMMC_SLOT1_IOMUX_PIN_NUM_CLK,
            .cmd_mosi = SDMMC_SLOT1_IOMUX_PIN_NUM_CMD,
            .d0_miso = SDMMC_SLOT1_IOMUX_PIN_NUM_D0,
            .d1 = SDMMC_SLOT1_IOMUX_PIN_NUM_D1,
            .d2 = SDMMC_SLOT1_IOMUX_PIN_NUM_D2,
            .d3_cs = SDMMC_SLOT1_IOMUX_PIN_NUM_D3,
#endif // SOC_SDMMC_USE_GPIO_MATRIX
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = CONFIG_SDMMC_BOARD_CUSTOM_CD,
            .wp = CONFIG_SDMMC_BOARD_CUSTOM_WP,
            .unused_pin = CONFIG_SDMMC_BOARD_CUSTOM_UNUSED,
        }
    },
};

#elif CONFIG_SDMMC_BOARD_CUSTOM_SPI

static const sdmmc_test_board_info_t s_board_info = {
    .name = "Custom defined board (SPI)",
    .slot = {
        {
            .slot_exists = false
        },
        {
            .slot_exists = true,
            .bus_width = 1,
            .clk = CONFIG_SDMMC_BOARD_CUSTOM_SCK,
            .cmd_mosi = CONFIG_SDMMC_BOARD_CUSTOM_MOSI,
            .d0_miso = CONFIG_SDMMC_BOARD_CUSTOM_MISO,
            .d1 = GPIO_NUM_NC,
            .d2 = GPIO_NUM_NC,
            .d3_cs = CONFIG_SDMMC_BOARD_CUSTOM_CS,
            .d4 = GPIO_NUM_NC,
            .d5 = GPIO_NUM_NC,
            .d6 = GPIO_NUM_NC,
            .d7 = GPIO_NUM_NC,
            .cd = CONFIG_SDMMC_BOARD_CUSTOM_CD,
            .wp = CONFIG_SDMMC_BOARD_CUSTOM_WP,
            .unused_pin = CONFIG_SDMMC_BOARD_CUSTOM_UNUSED,
        }
    },
};

#endif  // CONFIG_SDMMC_BOARD_*


const sdmmc_test_board_info_t* sdmmc_test_board_get_info(void)
{
    return &s_board_info;
}
