/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"

#define MAX_ADDR_24BIT      0x1000000
#define TEST_SPI_SPEED      10
#define TEST_SPI_READ_MODE  SPI_FLASH_FASTRD
// #define FORCE_GPIO_MATRIX

#if CONFIG_IDF_TARGET_ESP32
#define EXTRA_SPI1_CLK_IO   17  //the pin which is usually used by the PSRAM clk
#define SPI1_CS_IO          16  //the pin which is usually used by the PSRAM cs

#define HSPI_PIN_NUM_MOSI   HSPI_IOMUX_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   HSPI_IOMUX_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    HSPI_IOMUX_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     HSPI_IOMUX_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     HSPI_IOMUX_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     HSPI_IOMUX_PIN_NUM_CS

#define VSPI_PIN_NUM_MOSI   VSPI_IOMUX_PIN_NUM_MOSI
#define VSPI_PIN_NUM_MISO   VSPI_IOMUX_PIN_NUM_MISO
#define VSPI_PIN_NUM_CLK    VSPI_IOMUX_PIN_NUM_CLK
#define VSPI_PIN_NUM_HD     VSPI_IOMUX_PIN_NUM_HD
#define VSPI_PIN_NUM_WP     VSPI_IOMUX_PIN_NUM_WP
#define VSPI_PIN_NUM_CS     VSPI_IOMUX_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32S2
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   35
#define FSPI_PIN_NUM_MISO   37
#define FSPI_PIN_NUM_CLK    36
#define FSPI_PIN_NUM_HD     33
#define FSPI_PIN_NUM_WP     38
#define FSPI_PIN_NUM_CS     34

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32S3
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   11
#define FSPI_PIN_NUM_MISO   13
#define FSPI_PIN_NUM_CLK    12
#define FSPI_PIN_NUM_HD     9
#define FSPI_PIN_NUM_WP     14
#define FSPI_PIN_NUM_CS     10

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C6
#define SPI1_CS_IO          26  //the pin which is usually used by the PSRAM cs
#define SPI1_HD_IO          27  //the pin which is usually used by the PSRAM hd
#define SPI1_WP_IO          28  //the pin which is usually used by the PSRAM wp

#define FSPI_PIN_NUM_MOSI   7
#define FSPI_PIN_NUM_MISO   2
#define FSPI_PIN_NUM_CLK    6
#define FSPI_PIN_NUM_HD     4
#define FSPI_PIN_NUM_WP     5
#define FSPI_PIN_NUM_CS     10

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32H2

#define FSPI_PIN_NUM_MOSI   5
#define FSPI_PIN_NUM_MISO   0
#define FSPI_PIN_NUM_CLK    4
#define FSPI_PIN_NUM_HD     3
#define FSPI_PIN_NUM_WP     2
#define FSPI_PIN_NUM_CS     1

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS

#elif CONFIG_IDF_TARGET_ESP32P4

// Normal IOMUX pins
#define FSPI_PIN_NUM_MOSI   8
#define FSPI_PIN_NUM_MISO   10
#define FSPI_PIN_NUM_CLK    9
#define FSPI_PIN_NUM_HD     6
#define FSPI_PIN_NUM_WP     11
#define FSPI_PIN_NUM_CS     7

// Just use the same pins for HSPI
#define HSPI_PIN_NUM_MOSI   FSPI_PIN_NUM_MOSI
#define HSPI_PIN_NUM_MISO   FSPI_PIN_NUM_MISO
#define HSPI_PIN_NUM_CLK    FSPI_PIN_NUM_CLK
#define HSPI_PIN_NUM_HD     FSPI_PIN_NUM_HD
#define HSPI_PIN_NUM_WP     FSPI_PIN_NUM_WP
#define HSPI_PIN_NUM_CS     FSPI_PIN_NUM_CS
#endif

#define TEST_CONFIG_NUM (sizeof(config_list)/sizeof(flashtest_config_t))

typedef void (*flash_test_func_t)(const esp_partition_t *part);

/* Use TEST_CASE_FLASH for SPI flash tests that only use the main SPI flash chip
*/
#define TEST_CASE_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

#define TEST_CASE_FLASH_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR, "[esp_flash][ignore]") {flash_test_func(FUNC_TO_RUN, 1 /* first index reserved for main flash */ );}

/* Use TEST_CASE_MULTI_FLASH for tests which also run on external flash, which sits in the place of PSRAM
   (these tests are incompatible with PSRAM)

   These tests run for all the flash chip configs shown in config_list, below (internal and external).
 */
#define IDF_LOG_PERFORMANCE(item, value_fmt, value, ...) \
    printf("[Performance][%s]: " value_fmt "\n", item, value, ##__VA_ARGS__)

#define LOG_DATA(bus, suffix, chip) IDF_LOG_PERFORMANCE("FLASH_SPEED_BYTE_PER_SEC_"#bus#suffix, "%ld, flash_chip: %s", speed_##suffix, chip)
#define LOG_ERASE(bus, var, chip) IDF_LOG_PERFORMANCE("FLASH_SPEED_BYTE_PER_SEC_"#bus"ERASE", "%ld, flash_chip: %s", var, chip)

// Erase time may vary a lot, can increase threshold if this fails with a reasonable speed
#define LOG_PERFORMANCE(bus, chip) do {\
            LOG_DATA(bus, WR_4B, chip); \
            LOG_DATA(bus, RD_4B, chip); \
            LOG_DATA(bus, WR_2KB, chip); \
            LOG_DATA(bus, RD_2KB, chip); \
            LOG_ERASE(bus, erase_1, chip); \
            LOG_ERASE(bus, erase_2, chip); \
        } while (0)


#if defined(CONFIG_SPIRAM)
//SPI1 CS1 occupied by PSRAM
#define BYPASS_MULTIPLE_CHIP    1
#endif

#if CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C6
//chips without PSRAM
#define TEST_CHIP_NUM   2
#elif CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define TEST_CHIP_NUM   3
#endif

#define _STRINGIFY(s)   #s
#define STRINGIFY(s)   _STRINGIFY(s)
#define TEST_CHIP_NUM_STR   STRINGIFY(TEST_CHIP_NUM)

#if BYPASS_MULTIPLE_CHIP
#define TEST_CASE_MULTI_FLASH   TEST_CASE_MULTI_FLASH_IGNORE
#define TEST_CASE_MULTI_FLASH_LONG   TEST_CASE_MULTI_FLASH_IGNORE
#else
#if CONFIG_FREERTOS_SMP // IDF-5260
#define TEST_CASE_MULTI_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][timeout=60]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}
#else
#define TEST_CASE_MULTI_FLASH(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][timeout=35]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}
#endif

#define TEST_CASE_MULTI_FLASH_LONG(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][timeout=120]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}

#endif

#define TEST_CASE_MULTI_FLASH_IGNORE(STR, FUNC_TO_RUN) \
    TEST_CASE(STR", "TEST_CHIP_NUM_STR" chips", "[esp_flash_multi][test_env=UT_T1_ESP_FLASH][ignore]") {flash_test_func(FUNC_TO_RUN, TEST_CONFIG_NUM);}


//currently all the configs are the same with esp_flash_spi_device_config_t, no more information required
typedef esp_flash_spi_device_config_t flashtest_config_t;

static const char TAG[] = "test_esp_flash";

#define FLASHTEST_CONFIG_COMMON     \
    /* 0 always reserved for main flash */ \
    { \
        /* no need to init */ \
        .host_id = -1, \
    } \
    , \
    { \
        .io_mode = TEST_SPI_READ_MODE,\
        .freq_mhz = TEST_SPI_SPEED, \
        .host_id = SPI1_HOST, \
        .cs_id = 1, \
        /* the pin which is usually used by the PSRAM */ \
        .cs_io_num = SPI1_CS_IO, \
        .input_delay_ns = 0, \
    }

#if CONFIG_IDF_TARGET_ESP32
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    /* current runner doesn't have a flash on HSPI */
    // {
    //     .io_mode = TEST_SPI_READ_MODE,
    //     .freq_mhz = TEST_SPI_SPEED,
    //     .host_id = HSPI_HOST,
    //     .cs_id = 0,
    //     // uses GPIO matrix on esp32s2 regardless if FORCE_GPIO_MATRIX
    //     .cs_io_num = HSPI_PIN_NUM_CS,
    //     .input_delay_ns = 20,
    // },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = VSPI_HOST,
        .cs_id = 0,
        .cs_io_num = VSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32S2
flashtest_config_t config_list[] = {
    FLASHTEST_CONFIG_COMMON,
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = FSPI_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = HSPI_HOST,
        .cs_id = 0,
        // uses GPIO matrix on esp32s2 regardless of FORCE_GPIO_MATRIX
        .cs_io_num = HSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#elif CONFIG_IDF_TARGET_ESP32S3
flashtest_config_t config_list[] = {
    /* No SPI1 CS1 flash on esp32S3 test */
    {
        /* no need to init */
        .host_id = -1,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = SPI2_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#else
flashtest_config_t config_list[] = {
    /* No SPI1 CS1 flash on esp32c3 test */
    {
        /* no need to init */
        .host_id = -1,
    },
    {
        .io_mode = TEST_SPI_READ_MODE,
        .freq_mhz = TEST_SPI_SPEED,
        .host_id = SPI2_HOST,
        .cs_id = 0,
        .cs_io_num = FSPI_PIN_NUM_CS,
        .input_delay_ns = 0,
    },
};
#endif
