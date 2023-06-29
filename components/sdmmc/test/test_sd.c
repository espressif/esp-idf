/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_rom_gpio.h"
#include "test_utils.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_reg.h"

// Currently no runners for S3
#define WITH_SD_TEST    (SOC_SDMMC_HOST_SUPPORTED && !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3))
// Currently, no runners for S3, C2, and C6
#define WITH_SDSPI_TEST (!TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3, ESP32C2, ESP32C6, ESP32H2))
// Can't test eMMC (slot 0) and PSRAM together
#define WITH_EMMC_TEST  (SOC_SDMMC_HOST_SUPPORTED && !CONFIG_SPIRAM && !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S3))

/* power supply enable pin */
#define SD_TEST_BOARD_VSEL_EN_GPIO  27

/* power supply voltage select pin */
#define SD_TEST_BOARD_VSEL_GPIO     26
#define SD_TEST_BOARD_VSEL_3V3      1
#define SD_TEST_BOARD_VSEL_1V8      0

/* time to wait for reset / power-on */
#define SD_TEST_BOARD_PWR_RST_DELAY_MS  5
#define SD_TEST_BOARD_PWR_ON_DELAY_MS   50

/* gpio which is not connected to actual CD pin, used to simulate CD behavior */
#define CD_WP_TEST_GPIO 18

/* default GPIO selection */
#ifdef CONFIG_IDF_TARGET_ESP32S2
#define SDSPI_TEST_MOSI_PIN GPIO_NUM_35
#define SDSPI_TEST_MISO_PIN GPIO_NUM_37
#define SDSPI_TEST_SCLK_PIN GPIO_NUM_36
#define SDSPI_TEST_CS_PIN GPIO_NUM_34
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define SDSPI_TEST_MOSI_PIN GPIO_NUM_4
#define SDSPI_TEST_MISO_PIN GPIO_NUM_6
#define SDSPI_TEST_SCLK_PIN GPIO_NUM_5
#define SDSPI_TEST_CS_PIN GPIO_NUM_1
#else
#define SDSPI_TEST_MOSI_PIN GPIO_NUM_15
#define SDSPI_TEST_MISO_PIN GPIO_NUM_2
#define SDSPI_TEST_SCLK_PIN GPIO_NUM_14
#define SDSPI_TEST_CS_PIN GPIO_NUM_13
#endif

#define GPIO_ACTIVE_LOW     0
#define GPIO_ACTIVE_HIGH    1

TEST_CASE("MMC_RSP_BITS", "[sd]")
{
    uint32_t data[2] = { 0x01234567, 0x89abcdef };
    TEST_ASSERT_EQUAL_HEX32(0x7,   MMC_RSP_BITS(data, 0, 4));
    TEST_ASSERT_EQUAL_HEX32(0x567, MMC_RSP_BITS(data, 0, 12));
    TEST_ASSERT_EQUAL_HEX32(0xf0,  MMC_RSP_BITS(data, 28, 8));
    TEST_ASSERT_EQUAL_HEX32(0x3,   MMC_RSP_BITS(data, 1, 3));
    TEST_ASSERT_EQUAL_HEX32(0x11,  MMC_RSP_BITS(data, 59, 5));
}

#if WITH_SD_TEST || WITH_EMMC_TEST
static void sd_test_board_power_on(void)
{
    gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, SD_TEST_BOARD_VSEL_3V3);
    gpio_set_direction(SD_TEST_BOARD_VSEL_EN_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 0);
    usleep(SD_TEST_BOARD_PWR_RST_DELAY_MS * 1000);
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 1);
    usleep(SD_TEST_BOARD_PWR_ON_DELAY_MS * 1000);
}

static void sd_test_board_power_off(void)
{
    gpio_set_level(SD_TEST_BOARD_VSEL_EN_GPIO, 0);
    gpio_set_direction(SD_TEST_BOARD_VSEL_GPIO, GPIO_MODE_INPUT);
    gpio_set_level(SD_TEST_BOARD_VSEL_GPIO, 0);
    gpio_set_direction(SD_TEST_BOARD_VSEL_EN_GPIO, GPIO_MODE_INPUT);
}

static void probe_sd(int slot, int width, int freq_khz, int ddr)
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.slot = slot;
    config.max_freq_khz = freq_khz;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    if (width == 1) {
        config.flags = SDMMC_HOST_FLAG_1BIT;
        slot_config.width = 1;
    } else if (width == 4) {
        config.flags &= ~SDMMC_HOST_FLAG_8BIT;
        slot_config.width = 4;
    } else {
        assert(!ddr && "host driver does not support 8-line DDR mode yet");
    }
    if (!ddr) {
        config.flags &= ~SDMMC_HOST_FLAG_DDR;
    }
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    uint8_t* buffer = heap_caps_malloc(512, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, 0, 1));
    free(buffer);
    TEST_ESP_OK(sdmmc_host_deinit());
    free(card);
    sd_test_board_power_off();
}

extern void sdmmc_host_get_clk_dividers(const int freq_khz, int *host_div, int *card_div);

static void sd_test_check_clk_dividers(const int freq_khz, const int expected_host_div, const int expected_card_div)
{
    printf("    %6d      |         %2d        |         %2d\n", freq_khz, expected_host_div, expected_card_div);
    int host_divider, card_divider;
    sdmmc_host_get_clk_dividers(freq_khz, &host_divider, &card_divider);
    TEST_ASSERT_EQUAL(host_divider, expected_host_div);
    TEST_ASSERT_EQUAL(card_divider, expected_card_div);
}
#endif //WITH_SD_TEST || WITH_EMMC_TEST

#if WITH_SD_TEST
TEST_CASE("probe SD, slot 1, 4-bit", "[sd][test_env=UT_T1_SDMODE]")
{
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 4, SDMMC_FREQ_HIGHSPEED, 0);
    //custom frequency test
    probe_sd(SDMMC_HOST_SLOT_1, 4, 10000, 0);
}

TEST_CASE("probe SD, slot 1, 1-bit", "[sd][test_env=UT_T1_SDMODE]")
{
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, 0);
}

//No runners for slot 0
TEST_CASE("probe SD, slot 0, 4-bit", "[sd][ignore]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 0);
}

TEST_CASE("probe SD, slot 0, 1-bit", "[sd][ignore]")
{
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 1, SDMMC_FREQ_HIGHSPEED, 0);
}

TEST_CASE("SD clock dividers calculation", "[sd][test_env=UT_T1_SDMODE]")
{
    printf("Frequency (kHz) | Expected host.div | Expected card.div\n");
    sd_test_check_clk_dividers(SDMMC_FREQ_PROBING, 10, 20);
    sd_test_check_clk_dividers(SDMMC_FREQ_DEFAULT, 8, 0);
    sd_test_check_clk_dividers(SDMMC_FREQ_HIGHSPEED, 4, 0);
    sd_test_check_clk_dividers(36000, 5, 0);
    sd_test_check_clk_dividers(30000, 6, 0);
    sd_test_check_clk_dividers(16000, 10, 0);
    sd_test_check_clk_dividers(10000, 2, 4);
    sd_test_check_clk_dividers(6000, 2, 7);
    sd_test_check_clk_dividers(1000, 2, 40);
    sd_test_check_clk_dividers(600, 2, 67);
}
#endif //WITH_SD_TEST

#if WITH_EMMC_TEST
TEST_CASE("probe eMMC, slot 0, 4-bit", "[sd][test_env=EMMC][ignore]")
{
    //Test with SDR
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 0);
    //Test with DDR
    probe_sd(SDMMC_HOST_SLOT_0, 4, SDMMC_FREQ_HIGHSPEED, 1);
}

TEST_CASE("probe eMMC, slot 0, 8-bit", "[sd][test_env=EMMC][ignore]")
{
    //8-bit DDR not supported yet, test with SDR only
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_PROBING, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_DEFAULT, 0);
    probe_sd(SDMMC_HOST_SLOT_0, 8, SDMMC_FREQ_HIGHSPEED, 0);
}
#endif // WITH_EMMC_TEST

#if WITH_SDSPI_TEST

#if !WITH_SD_TEST && !WITH_EMMC_TEST
static void sd_test_board_power_on(void)
{
    // do nothing
}

static void sd_test_board_power_off(void)
{
    // do nothing
}
#endif

static void test_sdspi_init_bus(spi_host_device_t host, int mosi_pin, int miso_pin, int clk_pin, int dma_chan)
{
    spi_bus_config_t bus_config = {
        .mosi_io_num = mosi_pin,
        .miso_io_num = miso_pin,
        .sclk_io_num = clk_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    esp_err_t err = spi_bus_initialize(host, &bus_config, dma_chan);
    TEST_ESP_OK(err);
}

static void test_sdspi_deinit_bus(spi_host_device_t host)
{
    esp_err_t err = spi_bus_free(host);
    TEST_ESP_OK(err);
}

static void probe_core(int slot, int freq_khz)
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.slot = slot;
    config.max_freq_khz = freq_khz;

    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    free(card);
}

static void probe_spi(int freq_khz, int pin_miso, int pin_mosi, int pin_sck, int pin_cs)
{
    sd_test_board_power_on();

    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.gpio_cs = pin_cs;
    test_sdspi_init_bus(dev_config.host_id, pin_mosi, pin_miso, pin_sck, SPI_DMA_CH_AUTO);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    probe_core(handle, freq_khz);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}


TEST_CASE("probe SD in SPI mode", "[sd][test_env=UT_T1_SPIMODE]")
{
    probe_spi(SDMMC_FREQ_DEFAULT, SDSPI_TEST_MISO_PIN, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_SCLK_PIN, SDSPI_TEST_CS_PIN);
    //custom frequency test
    probe_spi(10000, SDSPI_TEST_MISO_PIN, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_SCLK_PIN, SDSPI_TEST_CS_PIN);
}

// No runner for this
TEST_CASE("probe SD in SPI mode, slot 0", "[sd][ignore]")
{
    probe_spi(SDMMC_FREQ_DEFAULT, 7, 11, 6, 10);
}
#endif //WITH_SDSPI_TEST

#if WITH_SD_TEST || WITH_SDSPI_TEST || WITH_EMMC_TEST
// Fill buffer pointed to by 'dst' with 'count' 32-bit ints generated
// from 'rand' with the starting value of 'seed'
static void fill_buffer(uint32_t seed, uint8_t* dst, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = rand();
        memcpy(dst + i * sizeof(uint32_t), &val, sizeof(val));
    }
}

// Check if the buffer pointed to by 'dst' contains 'count' 32-bit
// ints generated from 'rand' with the starting value of 'seed'
static void check_buffer(uint32_t seed, const uint8_t* src, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val;
        memcpy(&val, src + i * sizeof(uint32_t), sizeof(val));
        TEST_ASSERT_EQUAL_HEX32(rand(), val);
    }
}

static void do_single_write_read_test(sdmmc_card_t* card, size_t start_block,
                    size_t block_count, size_t alignment, bool performance_log)
{
    size_t block_size = card->csd.sector_size;
    size_t total_size = block_size * block_count;
    printf(" %8d |  %3d  |   %d   |    %4.1f  ", start_block, block_count, alignment, total_size / 1024.0f);

    uint32_t* buffer = heap_caps_malloc(total_size + 4, MALLOC_CAP_DMA);
    size_t offset = alignment % 4;
    uint8_t* c_buffer = (uint8_t*) buffer + offset;
    fill_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));

    struct timeval t_start_wr;
    gettimeofday(&t_start_wr, NULL);
    TEST_ESP_OK(sdmmc_write_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_wr = 1e3f * (t_stop_wr.tv_sec - t_start_wr.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_wr.tv_usec);

    memset(buffer, 0xbb, total_size + 4);

    struct timeval t_start_rd;
    gettimeofday(&t_start_rd, NULL);
    TEST_ESP_OK(sdmmc_read_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_rd;
    gettimeofday(&t_stop_rd, NULL);
    float time_rd = 1e3f * (t_stop_rd.tv_sec - t_start_rd.tv_sec) + 1e-3f * (t_stop_rd.tv_usec - t_start_rd.tv_usec);

    printf(" |   %6.2f    |      %5.2f      |    %6.2f     |     %5.2f\n",
            time_wr, total_size / (time_wr / 1000) / (1024 * 1024),
            time_rd, total_size / (time_rd / 1000) / (1024 * 1024));
    check_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));
    free(buffer);

    if (performance_log) {
        static const char wr_speed_str[] = "SDMMC_WR_SPEED";
        static const char rd_speed_str[] = "SDMMC_RD_SPEED";
        int aligned = ((alignment % 4) == 0)? 1: 0;
        IDF_LOG_PERFORMANCE(wr_speed_str, "%d, blk_n: %d, aligned: %d",
                            (int)(total_size * 1000 / time_wr), block_count, aligned);
        IDF_LOG_PERFORMANCE(rd_speed_str, "%d, blk_n: %d, aligned: %d",
                            (int)(total_size * 1000 / time_rd), block_count, aligned);
    }
}

typedef void (*sd_test_func_t)(sdmmc_card_t* card);

static void test_read_write_performance(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    const int offset = 0;
    const bool do_log = true;
    //aligned
    do_single_write_read_test(card, offset, 1, 4, do_log);
    do_single_write_read_test(card, offset, 4, 4, do_log);
    do_single_write_read_test(card, offset, 8, 4, do_log);
    do_single_write_read_test(card, offset, 16, 4, do_log);
    do_single_write_read_test(card, offset, 32, 4, do_log);
    do_single_write_read_test(card, offset, 64, 4, do_log);
    do_single_write_read_test(card, offset, 128, 4, do_log);
    //unaligned
    do_single_write_read_test(card, offset, 1, 1, do_log);
    do_single_write_read_test(card, offset, 8, 1, do_log);
    do_single_write_read_test(card, offset, 128, 1, do_log);
}

static void test_read_write_with_offset(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    const bool no_log = false;;
    //aligned
    do_single_write_read_test(card, 1, 16, 4, no_log);
    do_single_write_read_test(card, 16, 32, 4, no_log);
    do_single_write_read_test(card, 48, 64, 4, no_log);
    do_single_write_read_test(card, 128, 128, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity - 64, 32, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity - 64, 64, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity - 8, 1, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 1, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 4, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 16, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 32, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 64, 4, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 4, no_log);
    //unaligned
    do_single_write_read_test(card, card->csd.capacity/2, 1, 1, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 1, no_log);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 1, no_log);
}
#endif //WITH_SD_TEST || WITH_SDSPI_TEST || WITH_EMMC_TEST

#if WITH_SD_TEST || WITH_EMMC_TEST
void sd_test_rw_blocks(int slot, int width, sd_test_func_t test_func)
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    config.slot = slot;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    if (width != 0) {
        slot_config.width = width;
    }
    if (slot_config.width == 8) {
        config.flags &= ~SDMMC_HOST_FLAG_DDR;
    }
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    test_func(card);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
}
#endif //WITH_SD_TEST || WITH_EMMC_TEST

#if WITH_SD_TEST
TEST_CASE("SDMMC performance test (SD slot 1, 4 line)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 4, test_read_write_performance);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC performance test (SD slot 1, 1 line)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 1, test_read_write_performance);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC test read/write with offset (SD slot 1)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 4, test_read_write_with_offset);
    sd_test_board_power_off();
}
#endif //WITH_SD_TEST

#if WITH_EMMC_TEST
TEST_CASE("SDMMC performance test (eMMC slot 0, 4 line DDR)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 4, test_read_write_performance);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC test read/write with offset (eMMC slot 0, 4 line DDR)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 4, test_read_write_with_offset);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC performance test (eMMC slot 0, 8 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 8, test_read_write_performance);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC test read/write with offset (eMMC slot 0, 8 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 8, test_read_write_with_offset);
    sd_test_board_power_off();
}
#endif // WITH_EMMC_TEST

#if WITH_SDSPI_TEST
void sdspi_test_rw_blocks(sd_test_func_t test_func)
{
    sd_test_board_power_on();

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.host_id = config.slot;
    dev_config.gpio_cs = SDSPI_TEST_CS_PIN;
    test_sdspi_init_bus(dev_config.host_id, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_MISO_PIN, SDSPI_TEST_SCLK_PIN, SPI_DMA_CH_AUTO);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    // This test can only run under 20MHz on ESP32, because the runner connects the card to
    // non-IOMUX pins of HSPI.

    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    test_func(card);
    TEST_ESP_OK(sdspi_host_deinit());
    free(card);
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC performance (SPI mode)", "[sdspi][test_env=UT_T1_SPIMODE]")
{
    sdspi_test_rw_blocks(test_read_write_performance);
}

TEST_CASE("SDMMC test read/write with offset (SPI mode)", "[sdspi][test_env=UT_T1_SPIMODE]")
{
    sdspi_test_rw_blocks(test_read_write_with_offset);
}
#endif //WITH_SDSPI_TEST

#if WITH_SD_TEST
TEST_CASE("reads and writes with an unaligned buffer", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init());

    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));

    const size_t buffer_size = 4096;
    const size_t block_count = buffer_size / 512;
    const size_t extra = 4;
    uint8_t* buffer = heap_caps_malloc(buffer_size + extra, MALLOC_CAP_DMA);

    // Check read behavior: do aligned write, then unaligned read
    const uint32_t seed = 0x89abcdef;
    fill_buffer(seed, buffer, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer, 0, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer + 1, 0, block_count));
    check_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));

    // Check write behavior: do unaligned write, then aligned read
    fill_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer + 1, 8, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, 8, block_count));
    check_buffer(seed, buffer, buffer_size / sizeof(uint32_t));

    free(buffer);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif //WITH_SD_TEST

#if WITH_SD_TEST || WITH_SDSPI_TEST
static void test_cd_input(int gpio_cd_num, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured CD as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    esp_rom_gpio_connect_out_signal(gpio_cd_num, SIG_GPIO_OUT_IDX, false, false);
    REG_WRITE(GPIO_ENABLE_W1TS_REG, BIT(gpio_cd_num));

    // Check that card initialization fails if CD is high
    REG_WRITE(GPIO_OUT_W1TS_REG, BIT(gpio_cd_num));
    usleep(10000);
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, sdmmc_card_init(config, card));

    // Check that card initialization succeeds if CD is low
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_cd_num));
    usleep(10000);
    esp_err_t err = sdmmc_card_init(config, card);
    if (err != ESP_OK) {
        usleep(10000);
        // Try again, in case the card was not ready yet
        err = sdmmc_card_init(config, card);
    }
    TEST_ESP_OK(err);

    free(card);
}

static void test_wp_input(int gpio_wp_num, bool gpio_wp_polarity, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured WP as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    esp_rom_gpio_connect_out_signal(gpio_wp_num, SIG_GPIO_OUT_IDX, false, false);
    REG_WRITE(GPIO_ENABLE_W1TS_REG, BIT(gpio_wp_num));

    // Check that the card can be initialized with WP low
    REG_WRITE(GPIO_OUT_W1TC_REG, BIT(gpio_wp_num));
    TEST_ESP_OK(sdmmc_card_init(config, card));

    uint32_t* data = heap_caps_calloc(1, 512, MALLOC_CAP_DMA);

    // Check that card write succeeds if WP is high
    REG_WRITE((gpio_wp_polarity? GPIO_OUT_W1TC_REG : GPIO_OUT_W1TS_REG), BIT(gpio_wp_num));
    usleep(1000);
    TEST_ESP_OK(sdmmc_write_sectors(card, &data, 0, 1));

    // Check that write fails if WP is low
    REG_WRITE((gpio_wp_polarity? GPIO_OUT_W1TS_REG : GPIO_OUT_W1TC_REG), BIT(gpio_wp_num));
    usleep(1000);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdmmc_write_sectors(card, &data, 0, 1));
    // ...but reads still work
    TEST_ESP_OK(sdmmc_read_sectors(card, &data, 0, 1));

    free(data);
    free(card);
}
#endif //WITH_SD_TEST || WITH_SDSPI_TEST

#if WITH_SD_TEST
TEST_CASE("CD input works in SD mode", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_cd = CD_WP_TEST_GPIO;
    TEST_ESP_OK(sdmmc_host_init());
    usleep(10000);
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    test_cd_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}

TEST_CASE("WP input works in SD mode", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_wp = CD_WP_TEST_GPIO;
    bool gpio_wp_polarity = GPIO_ACTIVE_LOW;
    if (gpio_wp_polarity) {
        slot_config.flags |= SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH;
    } else {
        slot_config.flags &= ~(SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH);
    }
    TEST_ESP_OK(sdmmc_host_init());
    usleep(10000);
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    test_wp_input(slot_config.gpio_wp, gpio_wp_polarity, &config);

    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif //WITH_SD_TEST

#if WITH_SDSPI_TEST
TEST_CASE("CD input works in SPI mode", "[sd][test_env=UT_T1_SPIMODE]")
{
    sd_test_board_power_on();

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.host_id = config.slot;
    dev_config.gpio_cs = SDSPI_TEST_CS_PIN;
    dev_config.gpio_cd = CD_WP_TEST_GPIO;
    test_sdspi_init_bus(dev_config.host_id, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_MISO_PIN, SDSPI_TEST_SCLK_PIN, SPI_DMA_CH_AUTO);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    config.slot = handle;

    test_cd_input(CD_WP_TEST_GPIO, &config);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}

TEST_CASE("WP input works in SPI mode", "[sd][test_env=UT_T1_SPIMODE]")
{
    sd_test_board_power_on();

    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.host_id = config.slot;
    dev_config.gpio_cs = SDSPI_TEST_CS_PIN;
    dev_config.gpio_wp = CD_WP_TEST_GPIO;
    dev_config.gpio_wp_polarity = GPIO_ACTIVE_LOW;
    test_sdspi_init_bus(dev_config.host_id, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_MISO_PIN, SDSPI_TEST_SCLK_PIN, SPI_DMA_CH_AUTO);

    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    config.slot = handle;

    test_wp_input(dev_config.gpio_wp, dev_config.gpio_wp_polarity, &config);

    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    sd_test_board_power_off();
}
#endif //WITH_SDSPI_TEST

#if WITH_SD_TEST || WITH_EMMC_TEST

#define PATTERN_SEED    0x12345678
#define FLAG_ERASE_TEST_ADJACENT    (1 << 0)
#define FLAG_VERIFY_ERASE_STATE     (1 << 1)
bool do_sanitize_flag = false;
static void ensure_sector_written(sdmmc_card_t* card, size_t sector,
        uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    TEST_ESP_OK(sdmmc_write_sectors(card, pattern_buf, sector, 1));
    memset((void *)temp_buf, 0x00, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    check_buffer(PATTERN_SEED, temp_buf, block_size / sizeof(uint32_t));
}

static void ensure_sector_intact(sdmmc_card_t* card, size_t sector,
        uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    memset((void *)temp_buf, 0x00, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    check_buffer(PATTERN_SEED, temp_buf, block_size / sizeof(uint32_t));
}

static int32_t ensure_sector_erase(sdmmc_card_t* card, size_t sector,
        uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    memset((void *)temp_buf, 0, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    return memcmp(pattern_buf, temp_buf, block_size);
}

static void do_single_erase_test(sdmmc_card_t* card, size_t start_block,
                    size_t block_count, uint8_t flags, sdmmc_erase_arg_t arg)
{
    size_t block_size = card->csd.sector_size;
    uint8_t *temp_buf = NULL;
    uint8_t *pattern_buf = NULL;
    size_t end_block = (start_block + block_count - 1);

    /*
     * To ensure erase is successful/valid
     * selected blocks after erase should have erase state data pattern
     * data of blocks adjacent to selected region should remain intact
     */
    TEST_ESP_OK((start_block + block_count) > card->csd.capacity);

    pattern_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(pattern_buf);
    temp_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(temp_buf);

    // create pattern buffer
    fill_buffer(PATTERN_SEED, pattern_buf, block_size / sizeof(uint32_t));

    // check if it's not the first block of device & write/read/verify pattern
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && start_block) {
        ensure_sector_written(card, (start_block - 1), pattern_buf, temp_buf);
    }

    ensure_sector_written(card, start_block, pattern_buf, temp_buf);

    // check if it's not the last block of device & write/read/verify pattern
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && (end_block < (card->csd.capacity - 1))) {
        ensure_sector_written(card, (end_block + 1), pattern_buf, temp_buf);
    }

    // when block count is 1, start and end block is same, hence skip
    if (block_count != 1) {
        ensure_sector_written(card, end_block, pattern_buf, temp_buf);
    }

    // fill pattern to (start_block + end_block)/2 in the erase range
    if(block_count > 2) {
        ensure_sector_written(card, (start_block + end_block)/2, pattern_buf, temp_buf);
    }

    float total_size = (block_count/1024.0f) * block_size;
    printf(" %10d |  %10d   |  %8.1f    ", start_block, block_count, total_size);
    fflush(stdout);

    // erase the blocks
    struct timeval t_start_er;
    gettimeofday(&t_start_er, NULL);
    TEST_ESP_OK(sdmmc_erase_sectors(card, start_block, block_count, arg));
    if (do_sanitize_flag) {
        TEST_ESP_OK(sdmmc_mmc_sanitize(card, block_count * 500));
    }
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_er = 1e3f * (t_stop_wr.tv_sec - t_start_er.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_er.tv_usec);
    printf(" |   %8.2f\n", time_er);

    // ensure adjacent blocks are not affected
    // block before start_block
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && start_block) {
        ensure_sector_intact(card, (start_block - 1), pattern_buf, temp_buf);
    }

    // block after end_block
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && (end_block < (card->csd.capacity - 1))) {
        ensure_sector_intact(card, (end_block + 1), pattern_buf, temp_buf);
    }

    uint8_t erase_mem_byte = 0xFF;
    // ensure all the blocks are erased and are up to after erase state.
    if (!card->is_mmc) {
        erase_mem_byte = card->scr.erase_mem_state ? 0xFF : 0x00;
    } else {
        erase_mem_byte = card->ext_csd.erase_mem_state ? 0xFF : 0x00;
    }

    memset((void *)pattern_buf, erase_mem_byte, block_size);

    // as it is block by block comparison, a time taking process. Really long
    // when you do erase and verify on complete device.
    if (flags & FLAG_VERIFY_ERASE_STATE) {
        for (size_t i  = 0; i < block_count; i++) {
            if (ensure_sector_erase(card, (start_block + i), pattern_buf, temp_buf)) {
                printf("Error: Sector %d erase\n", (start_block + i));
                break;
            }
        }
    }

    free(temp_buf);
    free(pattern_buf);
}
#endif // WITH_SD_TEST || WITH_EMMC_TEST

#if WITH_SDSPI_TEST
static void test_sdspi_erase_blocks(size_t start_block, size_t block_count)
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_dev_handle_t handle;
    sdspi_device_config_t dev_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    dev_config.host_id = config.slot;
    dev_config.gpio_cs = SDSPI_TEST_CS_PIN;
    test_sdspi_init_bus(dev_config.host_id, SDSPI_TEST_MOSI_PIN, SDSPI_TEST_MISO_PIN, SDSPI_TEST_SCLK_PIN, SPI_DMA_CH_AUTO);
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_device(&dev_config, &handle));

    // This test can only run under 20MHz on ESP32, because the runner connects the card to
    // non-IOMUX pins of HSPI.

    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);

    // Ensure discard operation is not supported in sdspi
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, start_block, block_count, SDMMC_DISCARD_ARG));

    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);
    printf("Erasing sectors %d-%d\n", start_block, (start_block + block_count -1));
    size_t block_size = card->csd.sector_size;
    uint8_t *pattern_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(pattern_buf);
    uint8_t *temp_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(temp_buf);

    struct timeval t_start_er;
    gettimeofday(&t_start_er, NULL);
    TEST_ESP_OK(sdmmc_erase_sectors(card, start_block, block_count, SDMMC_ERASE_ARG));
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_er = 1e3f * (t_stop_wr.tv_sec - t_start_er.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_er.tv_usec);
    printf("Erase duration: %.2fms\n", time_er);

    printf("Verifying erase state...\n");
    uint8_t erase_mem_byte = 0xFF;
    // ensure all the blocks are erased and are up to after erase state.
    if (!card->is_mmc) {
        erase_mem_byte = card->scr.erase_mem_state ? 0xFF : 0x00;
    } else {
        erase_mem_byte = card->ext_csd.erase_mem_state ? 0xFF : 0x00;
    }

    memset((void *)pattern_buf, erase_mem_byte, block_size);

    size_t i;
    for (i = 0; i < block_count; i++) {
        memset((void *)temp_buf, 0, block_size);
        TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, (start_block + i), 1));
        if (memcmp(pattern_buf, temp_buf, block_size)) {
            printf("Error: Sector %d erase\n", (start_block + i));
            break;
        }
    }
    if (i == block_count) {
        printf("Sectors erase success\n");
    }
    TEST_ESP_OK(sdspi_host_deinit());
    test_sdspi_deinit_bus(dev_config.host_id);
    free(card);
    free(temp_buf);
    free(pattern_buf);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC erase (SPI mode)", "[sdspi][test_env=UT_T1_SPIMODE]")
{
    test_sdspi_erase_blocks(0, 16);
}
#endif // WITH_SDSPI_TEST

#if WITH_SD_TEST
static void test_sd_erase_blocks(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);
    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;
    sdmmc_erase_arg_t arg = SDMMC_ERASE_ARG;

    //check for adjacent blocks and erase state of blocks
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT | (uint8_t)FLAG_VERIFY_ERASE_STATE;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    // single sector erase is failing on different make cards
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 128, flags, arg);
#ifdef SDMMC_FULL_ERASE_TEST
    /*
     * check for adjacent blocks, do not check erase state of blocks as it is
     * time taking process to verify all the blocks.
     */
    flags &= ~(uint8_t)FLAG_VERIFY_ERASE_STATE; //comment this line to verify after-erase state
    // erase complete card
    do_single_erase_test(card, 0, card->csd.capacity, flags, arg);
#endif //SDMMC_FULL_ERASE_TEST
}

static void test_sd_discard_blocks(sdmmc_card_t* card)
{
    /* MMC discard applies to write blocks */
    sdmmc_card_print_info(stdout, card);
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;
    sdmmc_erase_arg_t arg = SDMMC_DISCARD_ARG;

    /*
     * This test does run two tests
     * test-1: check, sdmmc_erase_sectors to return ESP_ERR_NOT_SUPPORTED
     * when arguments are condition not met. This test runs either the card
     * supports discard or not.
     *
     * test-2: If card supports discard, perform the test accordingly and
     * validate the behavior.
     *
     */
    uint32_t prev_discard_support = card->ssr.discard_support;
    // overwrite discard_support as not-supported for -ve test
    card->ssr.discard_support = 0;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
    // restore discard_support
    card->ssr.discard_support = prev_discard_support;
    if (sdmmc_can_discard(card) != ESP_OK ) {
        printf("Card/device do not support discard\n");
        return;
    }

    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);
    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * Check for adjacent blocks only.
     * After discard operation, the original data may be remained partially or
     * fully accessible to the host dependent on device. Hence do not verify
     * the erased state of the blocks.
     */
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 128, flags, arg);
}

TEST_CASE("SDMMC erase test (SD slot 1, 1 line)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 1, test_sd_erase_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC erase test (SD slot 1, 4 line)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 4, test_sd_erase_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC discard test (SD slot 1, 4 line)", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(1, 4, test_sd_discard_blocks);
    sd_test_board_power_off();
}
#endif //WITH_SD_TEST

#if WITH_SD_TEST
TEST_CASE("sdmmc read/write/erase sector shoud return ESP_OK with sector count == 0", "[sd][test_env=UT_T1_SDMODE]")
{
    sd_test_board_power_on();
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init());

    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));

    TEST_ESP_OK(sdmmc_write_sectors(card, NULL, 0, 0));
    TEST_ESP_OK(sdmmc_read_sectors(card, NULL, 0, 0));
    TEST_ESP_OK(sdmmc_erase_sectors(card, 0, 0, SDMMC_ERASE_ARG));

    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
    sd_test_board_power_off();
}
#endif //WITH_SD_TEST

#if WITH_EMMC_TEST
static void test_mmc_sanitize_blocks(sdmmc_card_t* card)
{
    /* MMC discard applies to write blocks */
    sdmmc_card_print_info(stdout, card);
    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);

    if (sdmmc_mmc_can_sanitize(card)) {
        printf("Card/device do not support sanitize\n");
        return;
    }
    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;
    sdmmc_erase_arg_t arg = SDMMC_DISCARD_ARG;
    do_sanitize_flag = true;

    /*
     * Check for adjacent blocks only.
     * After discard operation, the original data may be remained partially or
     * fully accessible to the host dependent on device. Hence do not verify
     * the erased state of the blocks.
     *
     * Note: After sanitize blocks has to be in erased state
     */
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT | (uint8_t)FLAG_VERIFY_ERASE_STATE;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 128, flags, arg);
    do_sanitize_flag = false;
}

static void test_mmc_discard_blocks(sdmmc_card_t* card)
{
    /* MMC discard applies to write blocks */
    sdmmc_card_print_info(stdout, card);
    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);

    sdmmc_erase_arg_t arg = SDMMC_DISCARD_ARG;
    uint32_t prev_ext_csd = card->ext_csd.rev;
    // overwrite discard_support as not-supported for -ve test
    card->ext_csd.rev = 0;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
    // restore discard_support
    card->ext_csd.rev = prev_ext_csd;
    if (sdmmc_can_discard(card) != ESP_OK) {
        printf("Card/device do not support discard\n");
        return;
    }

    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;

    /*
     * Check for adjacent blocks only.
     * After discard operation, the original data may be remained partially or
     * fully accessible to the host dependent on device. Hence do not verify
     * the erased state of the blocks.
     */
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 128, flags, arg);
}

static void test_mmc_trim_blocks(sdmmc_card_t* card)
{
    /* MMC trim applies to write blocks */
    sdmmc_card_print_info(stdout, card);
    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);
    sdmmc_erase_arg_t arg = SDMMC_ERASE_ARG;
    uint8_t prev_sec_feature = card->ext_csd.sec_feature;
    // overwrite sec_feature
    card->ext_csd.sec_feature &=  ~(EXT_CSD_SEC_GB_CL_EN);
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, sdmmc_erase_sectors(card, 0, 32, arg));
    // restore sec_feature
    card->ext_csd.sec_feature = prev_sec_feature;
    if (sdmmc_can_trim(card) != ESP_OK) {
        printf("Card/device do not support trim\n");
        return;
    }
    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;

    //check for adjacent blocks and erase state of blocks
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT | (uint8_t)FLAG_VERIFY_ERASE_STATE;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity/2, 128, flags, arg);
#ifdef SDMMC_FULL_ERASE_TEST
    /*
     * check for adjacent blocks, do not check erase state of blocks as it is
     * time taking process to verify all the blocks.
     */
    flags &= ~(uint8_t)FLAG_VERIFY_ERASE_STATE; //comment this line to verify after erase state
    // erase complete card
    do_single_erase_test(card, 0, card->csd.capacity, flags, arg);
#endif //SDMMC_FULL_ERASE_TEST
}

TEST_CASE("SDMMC trim test (eMMC slot 0, 4 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 4, test_mmc_trim_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC trim test (eMMC slot 0, 8 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 8, test_mmc_trim_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC discard test (eMMC slot 0, 4 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 4, test_mmc_discard_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC discard test (eMMC slot 0, 8 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 8, test_mmc_discard_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC sanitize test (eMMC slot 0, 4 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 4, test_mmc_sanitize_blocks);
    sd_test_board_power_off();
}

TEST_CASE("SDMMC sanitize test (eMMC slot 0, 8 line)", "[sd][test_env=EMMC][ignore]")
{
    sd_test_board_power_on();
    sd_test_rw_blocks(0, 8, test_mmc_sanitize_blocks);
    sd_test_board_power_off();
}
#endif //WITH_EMMC_TEST
