/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Test for spi_flash_{read,write}.

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "unity.h"
#include "spi_flash_mmap.h"
#include "esp_private/cache_utils.h"
#include "soc/timer_periph.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_rom_spiflash.h"
#include "esp_flash.h"
#include "esp_partition.h"

#if CONFIG_IDF_TARGET_ESP32
// Used for rom_fix function
#include "esp32/rom/spi_flash.h"
#endif

extern const esp_partition_t *get_test_data_partition(void);

#define MIN_BLOCK_SIZE  12
/* Base offset in flash for tests. */

static size_t start;

static void setup_tests(void)
{
    if (start == 0) {
        const esp_partition_t *part = get_test_data_partition();
        start = part->address;
        printf("Test data partition @ 0x%x\n", start);
    }
}

#ifndef CONFIG_SPI_FLASH_MINIMAL_TEST
#define CONFIG_SPI_FLASH_MINIMAL_TEST 1
#endif

static void fill(char *dest, int32_t start, int32_t len)
{
    for (int32_t i = 0; i < len; i++) {
        *(dest + i) = (char) (start + i);
    }
}

static int cmp_or_dump(const void *a, const void *b, size_t len)
{
    int r = memcmp(a, b, len);
    if (r != 0) {
        for (int i = 0; i < len; i++) {
            fprintf(stderr, "%02x", ((unsigned char *) a)[i]);
        }
        fprintf(stderr, "\n");
        for (int i = 0; i < len; i++) {
            fprintf(stderr, "%02x", ((unsigned char *) b)[i]);
        }
        fprintf(stderr, "\n");
    }
    return r;
}

static void IRAM_ATTR test_read(int src_off, int dst_off, int len)
{
    uint32_t src_buf[16];
    char dst_buf[64], dst_gold[64];

    fprintf(stderr, "src=%d dst=%d len=%d\n", src_off, dst_off, len);
    memset(src_buf, 0xAA, sizeof(src_buf));
    fill(((char *) src_buf) + src_off, src_off, len);
    ESP_ERROR_CHECK(esp_flash_erase_region(NULL, (start + src_off) & ~(0x10000 - 1), SPI_FLASH_SEC_SIZE));
    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_spiflash_result_t rc = esp_rom_spiflash_write(start, src_buf, sizeof(src_buf));
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_HEX(rc, ESP_ROM_SPIFLASH_RESULT_OK);
    memset(dst_buf, 0x55, sizeof(dst_buf));
    memset(dst_gold, 0x55, sizeof(dst_gold));
    fill(dst_gold + dst_off, src_off, len);
    ESP_ERROR_CHECK(esp_flash_read(NULL, dst_buf + dst_off, start + src_off, len));
    TEST_ASSERT_EQUAL_INT(cmp_or_dump(dst_buf, dst_gold, sizeof(dst_buf)), 0);
}

TEST_CASE("Test spi_flash_read", "[spi_flash][esp_flash]")
{
    setup_tests();
#if CONFIG_SPI_FLASH_MINIMAL_TEST
    test_read(0, 0, 0);
    test_read(0, 0, 4);
    test_read(0, 0, 16);
    test_read(0, 0, 64);
    test_read(0, 0, 1);
    test_read(0, 1, 1);
    test_read(1, 0, 1);
    test_read(1, 1, 1);
    test_read(1, 1, 2);
    test_read(1, 1, 3);
    test_read(1, 1, 4);
    test_read(1, 1, 5);
    test_read(3, 2, 5);
    test_read(0, 0, 17);
    test_read(0, 1, 17);
    test_read(1, 0, 17);
    test_read(1, 1, 17);
    test_read(1, 1, 18);
    test_read(1, 1, 19);
    test_read(1, 1, 20);
    test_read(1, 1, 21);
    test_read(3, 2, 21);
    test_read(4, 4, 60);
    test_read(59, 0, 5);
    test_read(60, 0, 4);
    test_read(60, 0, 3);
    test_read(60, 0, 2);
    test_read(63, 0, 1);
    test_read(64, 0, 0);
    test_read(59, 59, 5);
    test_read(60, 60, 4);
    test_read(60, 60, 3);
    test_read(60, 60, 2);
    test_read(63, 63, 1);
    test_read(64, 64, 0);
#else
    /* This will run a more thorough test but will slam flash pretty hard. */
    for (int src_off = 1; src_off < 16; src_off++) {
        for (int dst_off = 0; dst_off < 16; dst_off++) {
            for (int len = 0; len < 32; len++) {
                test_read(dst_off, src_off, len);
            }
        }
    }
#endif
}

extern void spi_common_set_dummy_output(esp_rom_spiflash_read_mode_t mode);
extern void spi_dummy_len_fix(uint8_t spi, uint8_t freqdiv);

static void IRAM_ATTR fix_rom_func(void)
{
    uint32_t freqdiv = 0;

#if CONFIG_ESPTOOLPY_FLASHFREQ_80M && !CONFIG_ESPTOOLPY_OCT_FLASH
    freqdiv = 1;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_80M && CONFIG_ESPTOOLPY_OCT_FLASH
    freqdiv = 2;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    freqdiv = 2;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_26M
    freqdiv = 3;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
    freqdiv = 4;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    freqdiv = 2;
#endif

#if CONFIG_IDF_TARGET_ESP32
    uint32_t dummy_bit = 0;
#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_80M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_40M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_26M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_26M;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
    dummy_bit = ESP_ROM_SPIFLASH_DUMMY_LEN_PLUS_20M;
#endif
    g_rom_spiflash_dummy_len_plus[1] = dummy_bit;
#else
    spi_dummy_len_fix(1, freqdiv);
#endif//CONFIG_IDF_TARGET_ESP32

    esp_rom_spiflash_read_mode_t read_mode;
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO
    read_mode = ESP_ROM_SPIFLASH_QIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    read_mode = ESP_ROM_SPIFLASH_QOUT_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DIO
    read_mode = ESP_ROM_SPIFLASH_DIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DOUT
    read_mode = ESP_ROM_SPIFLASH_DOUT_MODE;
#elif CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_STR
    read_mode = ESP_ROM_SPIFLASH_OPI_STR_MODE;
#elif CONFIG_ESPTOOLPY_FLASH_SAMPLE_MODE_DTR
    read_mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
#endif

#if !CONFIG_IDF_TARGET_ESP32S2 && !CONFIG_IDF_TARGET_ESP32
    spi_common_set_dummy_output(read_mode);
#endif //!CONFIG_IDF_TARGET_ESP32S2
    esp_rom_spiflash_config_clk(freqdiv, 1);
#if !CONFIG_ESPTOOLPY_OCT_FLASH
    esp_rom_spiflash_config_readmode(read_mode);
#endif
}

static void IRAM_ATTR test_write(int dst_off, int src_off, int len)
{
    char src_buf[64], dst_gold[64];
    uint32_t dst_buf[16];
    fprintf(stderr, "dst=%d src=%d len=%d\n", dst_off, src_off, len);
    memset(src_buf, 0x55, sizeof(src_buf));
    fill(src_buf + src_off, src_off, len);
    // Fills with 0xff
    ESP_ERROR_CHECK(esp_flash_erase_region(NULL, (start + src_off) & ~(0x10000 - 1), SPI_FLASH_SEC_SIZE));
    memset(dst_gold, 0xff, sizeof(dst_gold));
    if (len > 0) {
        int pad_left_off = (dst_off & ~3U);
        memset(dst_gold + pad_left_off, 0xff, 4);
        if (dst_off + len > pad_left_off + 4 && (dst_off + len) % 4 != 0) {
            int pad_right_off = ((dst_off + len) & ~3U);
            memset(dst_gold + pad_right_off, 0xff, 4);
        }
        fill(dst_gold + dst_off, src_off, len);
    }
    ESP_ERROR_CHECK(esp_flash_write(NULL, src_buf + src_off, start + dst_off, len));

    fix_rom_func();

    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_spiflash_result_t rc = esp_rom_spiflash_read(start, dst_buf, sizeof(dst_buf));
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_HEX(rc, ESP_ROM_SPIFLASH_RESULT_OK);

    TEST_ASSERT_EQUAL_INT(cmp_or_dump(dst_buf, dst_gold, sizeof(dst_buf)), 0);
}

TEST_CASE("Test esp_flash_write", "[spi_flash][esp_flash]")
{
    setup_tests();
#if CONFIG_SPI_FLASH_MINIMAL_TEST
    test_write(0, 0, 0);
    test_write(0, 0, 4);
    test_write(0, 0, 16);
    test_write(0, 0, 64);
    test_write(0, 0, 1);
    test_write(0, 1, 1);
    test_write(1, 0, 1);
    test_write(1, 1, 1);
    test_write(1, 1, 2);
    test_write(1, 1, 3);
    test_write(1, 1, 4);
    test_write(1, 1, 5);
    test_write(3, 2, 5);
    test_write(4, 4, 60);
    test_write(59, 0, 5);
    test_write(60, 0, 4);
    test_write(60, 0, 3);
    test_write(60, 0, 2);
    test_write(63, 0, 1);
    test_write(64, 0, 0);
    test_write(59, 59, 5);
    test_write(60, 60, 4);
    test_write(60, 60, 3);
    test_write(60, 60, 2);
    test_write(63, 63, 1);
    test_write(64, 64, 0);
#else
    /* This will run a more thorough test but will slam flash pretty hard. */
    for (int dst_off = 1; dst_off < 16; dst_off++) {
        for (int src_off = 0; src_off < 16; src_off++) {
            for (int len = 0; len < 16; len++) {
                test_write(dst_off, src_off, len);
            }
        }
    }
#endif
    /*
     * Test writing from ROM, IRAM and caches. We don't know what exactly will be
     * written, we're testing that there's no crash here.
     *
     * NB: At the moment these only support aligned addresses, because memcpy
     * is not aware of the 32-but load requirements for these regions.
     */
#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32H2 || CONFIG_IDF_TARGET_ESP32C6
#define TEST_SOC_IROM_ADDR              (SOC_IROM_LOW)
#define TEST_SOC_CACHE_RAM_BANK0_ADDR   (SOC_IRAM_LOW)
#define TEST_SOC_CACHE_RAM_BANK1_ADDR   (SOC_IRAM_LOW + 0x2000)
#define TEST_SOC_CACHE_RAM_BANK2_ADDR   (SOC_IRAM_LOW + 0x4000)
#define TEST_SOC_CACHE_RAM_BANK3_ADDR   (SOC_IRAM_LOW + 0x6000)
#define TEST_SOC_IRAM_ADDR              (SOC_IRAM_LOW + 0x8000)
#define TEST_SOC_RTC_IRAM_ADDR          (SOC_RTC_IRAM_LOW)
#define TEST_SOC_RTC_DRAM_ADDR          (SOC_RTC_DRAM_LOW)
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_IROM_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_IRAM_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_CACHE_RAM_BANK0_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_CACHE_RAM_BANK1_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_CACHE_RAM_BANK2_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_CACHE_RAM_BANK3_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_RTC_IRAM_ADDR, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) TEST_SOC_RTC_DRAM_ADDR, start, 16));
#else
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) 0x40000000, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) 0x40070000, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) 0x40078000, start, 16));
    ESP_ERROR_CHECK(esp_flash_write(NULL, (char *) 0x40080000, start, 16));
#endif
}

#ifdef CONFIG_SPIRAM

TEST_CASE("spi_flash_read can read into buffer in external RAM", "[spi_flash]")
{
    uint8_t* buf_ext = (uint8_t*) heap_caps_malloc(SPI_FLASH_SEC_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_ext);

    uint8_t* buf_int = (uint8_t*) heap_caps_malloc(SPI_FLASH_SEC_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_int);

    TEST_ESP_OK(esp_flash_read(NULL, buf_int, 0x1000, SPI_FLASH_SEC_SIZE));
    TEST_ESP_OK(esp_flash_read(NULL, buf_ext, 0x1000, SPI_FLASH_SEC_SIZE));

    TEST_ASSERT_EQUAL(0, memcmp(buf_ext, buf_int, SPI_FLASH_SEC_SIZE));
    free(buf_ext);
    free(buf_int);
}

TEST_CASE("esp_flash_write can write from external RAM buffer", "[spi_flash]")
{
    uint32_t* buf_ext = (uint32_t*) heap_caps_malloc(SPI_FLASH_SEC_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_ext);

    srand(0);
    for (size_t i = 0; i < SPI_FLASH_SEC_SIZE / sizeof(uint32_t); i++)
    {
        uint32_t val = rand();
        buf_ext[i] = val;
    }

    uint8_t* buf_int = (uint8_t*) heap_caps_malloc(SPI_FLASH_SEC_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_int);

    /* Write to flash from buf_ext */
    const esp_partition_t *part = get_test_data_partition();
    TEST_ESP_OK(esp_flash_erase_region(NULL, part->address & ~(0x10000 - 1), SPI_FLASH_SEC_SIZE));
    TEST_ESP_OK(esp_flash_write(NULL, buf_ext, part->address, SPI_FLASH_SEC_SIZE));

    /* Read back to buf_int and compare */
    TEST_ESP_OK(esp_flash_read(NULL, buf_int, part->address, SPI_FLASH_SEC_SIZE));
    TEST_ASSERT_EQUAL(0, memcmp(buf_ext, buf_int, SPI_FLASH_SEC_SIZE));

    free(buf_ext);
    free(buf_int);
}

TEST_CASE("spi_flash_read less than 16 bytes into buffer in external RAM", "[spi_flash]")
{
    uint8_t *buf_ext_8 = (uint8_t *) heap_caps_malloc(MIN_BLOCK_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_ext_8);

    uint8_t *buf_int_8 = (uint8_t *) heap_caps_malloc(MIN_BLOCK_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(buf_int_8);

    uint8_t data_8[MIN_BLOCK_SIZE];
    for (int i = 0; i < MIN_BLOCK_SIZE; i++) {
        data_8[i] = i;
    }

    const esp_partition_t *part = get_test_data_partition();
    TEST_ESP_OK(esp_flash_erase_region(NULL, part->address & ~(0x10000 - 1), SPI_FLASH_SEC_SIZE));
    TEST_ESP_OK(esp_flash_write(NULL, data_8, part->address, MIN_BLOCK_SIZE));
    TEST_ESP_OK(esp_flash_read(NULL, buf_ext_8, part->address, MIN_BLOCK_SIZE));
    TEST_ESP_OK(esp_flash_read(NULL, buf_int_8, part->address, MIN_BLOCK_SIZE));

    TEST_ASSERT_EQUAL(0, memcmp(buf_ext_8, data_8, MIN_BLOCK_SIZE));
    TEST_ASSERT_EQUAL(0, memcmp(buf_int_8, data_8, MIN_BLOCK_SIZE));

    if (buf_ext_8) {
        free(buf_ext_8);
        buf_ext_8 = NULL;
    }
    if (buf_int_8) {
        free(buf_int_8);
        buf_int_8 = NULL;
    }
}

#endif // CONFIG_SPIRAM
