// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Test for spi_flash_{read,write}.

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <unity.h>
#include <esp_spi_flash.h>
#include <rom/spi_flash.h>
#include "../cache_utils.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include "test_config.h"

/* Base offset in flash for tests. */
#define FLASH_BASE TEST_REGION_START

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
    ESP_ERROR_CHECK(spi_flash_erase_sector((FLASH_BASE + src_off) / SPI_FLASH_SEC_SIZE));
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc = SPIWrite(FLASH_BASE, src_buf, sizeof(src_buf));
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_INT(rc, SPI_FLASH_RESULT_OK);
    memset(dst_buf, 0x55, sizeof(dst_buf));
    memset(dst_gold, 0x55, sizeof(dst_gold));
    fill(dst_gold + dst_off, src_off, len);

    ESP_ERROR_CHECK(spi_flash_read(FLASH_BASE + src_off, dst_buf + dst_off, len));
    TEST_ASSERT_EQUAL_INT(cmp_or_dump(dst_buf, dst_gold, sizeof(dst_buf)), 0);
}

TEST_CASE("Test spi_flash_read", "[spi_flash_read]")
{
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

static void IRAM_ATTR test_write(int dst_off, int src_off, int len)
{
    char src_buf[64], dst_gold[64];
    uint32_t dst_buf[16];
    fprintf(stderr, "dst=%d src=%d len=%d\n", dst_off, src_off, len);
    memset(src_buf, 0x55, sizeof(src_buf));
    fill(src_buf + src_off, src_off, len);
    // Fills with 0xff
    ESP_ERROR_CHECK(spi_flash_erase_sector((FLASH_BASE + dst_off) / SPI_FLASH_SEC_SIZE));
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
    ESP_ERROR_CHECK(spi_flash_write(FLASH_BASE + dst_off, src_buf + src_off, len));
    spi_flash_disable_interrupts_caches_and_other_cpu();
    SpiFlashOpResult rc = SPIRead(FLASH_BASE, dst_buf, sizeof(dst_buf));
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_INT(rc, SPI_FLASH_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(cmp_or_dump(dst_buf, dst_gold, sizeof(dst_buf)), 0);
}

TEST_CASE("Test spi_flash_write", "[spi_flash_write]")
{
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
    ESP_ERROR_CHECK(spi_flash_write(FLASH_BASE, (char *) 0x40000000, 16));
    ESP_ERROR_CHECK(spi_flash_write(FLASH_BASE, (char *) 0x40070000, 16));
    ESP_ERROR_CHECK(spi_flash_write(FLASH_BASE, (char *) 0x40078000, 16));
    ESP_ERROR_CHECK(spi_flash_write(FLASH_BASE, (char *) 0x40080000, 16));
}
