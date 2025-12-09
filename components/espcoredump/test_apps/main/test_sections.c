/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "unity.h"
#include "esp_attr.h"
#include "esp_core_dump_types.h"
#include "core_dump_checksum.h"

/* Global variables that should be part of the coredump */
COREDUMP_IRAM_DATA_ATTR uint32_t var_iram = 0x42;
COREDUMP_DRAM_ATTR uint32_t var_dram = 0x43;
#if SOC_RTC_MEM_SUPPORTED
COREDUMP_RTC_DATA_ATTR uint32_t var_rtc = 0x44;
COREDUMP_RTC_FAST_ATTR uint32_t var_rtcfast = 0x45;
#endif // SOC_RTC_MEM_SUPPORTED

/* Memory regions to dump, defined at compile time. */
extern int _coredump_dram_start;
extern int _coredump_dram_end;
extern int _coredump_iram_start;
extern int _coredump_iram_end;
#if SOC_RTC_MEM_SUPPORTED
extern int _coredump_rtc_start;
extern int _coredump_rtc_end;
extern int _coredump_rtc_fast_start;
extern int _coredump_rtc_fast_end;
#endif // SOC_RTC_MEM_SUPPORTED

static inline bool is_addr_in_region(void *addr, uint8_t *region, int region_size)
{
    const void *start = (void *) region;
    const void *end = (void *)(region + region_size);
    return addr >= start && addr < end;
}

TEST_CASE("test variables presence in core dump sections", "[espcoredump]")
{
    uint32_t section_start = 0;
    uint32_t section_size = 0;

    /* Check DRAM coredump section */
    section_start = (uint32_t)&_coredump_dram_start;
    section_size = (uint8_t *)&_coredump_dram_end - (uint8_t *)&_coredump_dram_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_dram, (uint8_t *) section_start, section_size));
#if IRAM_8BIT_ACCESSIBLE
    /* Check IRAM coredump section */
    section_start = (uint32_t)&_coredump_iram_start;
    section_size = (uint8_t *)&_coredump_iram_end - (uint8_t *)&_coredump_iram_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_iram, (uint8_t *) section_start, section_size));
#endif
#if SOC_RTC_MEM_SUPPORTED
    /* Check RTC coredump section */
    section_start = (uint32_t)&_coredump_rtc_start;
    section_size = (uint8_t *)&_coredump_rtc_end - (uint8_t *)&_coredump_rtc_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_rtc, (uint8_t *) section_start, section_size));
    /* Check RTC Fast coredump section */
    section_start = (uint32_t)&_coredump_rtc_fast_start;
    section_size = (uint8_t *)&_coredump_rtc_fast_end - (uint8_t *)&_coredump_rtc_fast_start;
    TEST_ASSERT(section_size > 0);
    TEST_ASSERT(is_addr_in_region(&var_rtcfast, (uint8_t *) section_start, section_size));
#endif // SOC_RTC_MEM_SUPPORTED
}

/*
 * This section tests the SHA256 checksum functionality for the espcoredump component
 */
TEST_CASE("espcoredump SHA256 checksum API", "[espcoredump]")
{
#define SHA256_RESULT_LEN 32
    ESP_LOGI("espcoredump", "Testing SHA256 checksum API");

    /* Known test vector for SHA-256 */
    static const char *test_str = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    static const uint8_t expected_sha[SHA256_RESULT_LEN] = {
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
        0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
        0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
        0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
    };

    // Verify size and version
#if CONFIG_ESP_COREDUMP_CHECKSUM_SHA256
    TEST_ASSERT_EQUAL(SHA256_RESULT_LEN, esp_core_dump_checksum_size());
#endif // CONFIG_ESP_COREDUMP_CHECKSUM_SHA256

#if CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF
    TEST_ASSERT_EQUAL(COREDUMP_VERSION_ELF_SHA256, esp_core_dump_elf_version());
#endif // CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

    // Test both single update and multiple updates with the same input
    checksum_ctx_t sha_ctx1, sha_ctx2;
    core_dump_checksum_bytes checksum1, checksum2;

    // Test 1: Multiple updates (split string)
    const char *part1 = "abcdbc";
    const char *part2 = "decdefdefgefgh";
    const char *part3 = "fghighijhijkijkljklmklmnlmnomnopnopq";

    esp_core_dump_checksum_init(&sha_ctx1);
    esp_core_dump_checksum_update(&sha_ctx1, (void*)part1, strlen(part1));
    esp_core_dump_checksum_update(&sha_ctx1, (void*)part2, strlen(part2));
    esp_core_dump_checksum_update(&sha_ctx1, (void*)part3, strlen(part3));
    esp_core_dump_checksum_finish(&sha_ctx1, &checksum1);

    // Test 2: Single update (whole string)
    esp_core_dump_checksum_init(&sha_ctx2);
    esp_core_dump_checksum_update(&sha_ctx2, (void*)test_str, strlen(test_str));
    esp_core_dump_checksum_finish(&sha_ctx2, &checksum2);

    // Check against known vector and ensure both methods match
    TEST_ASSERT_EQUAL_MEMORY(expected_sha, checksum1, SHA256_RESULT_LEN);
    TEST_ASSERT_EQUAL_MEMORY(checksum1, checksum2, SHA256_RESULT_LEN);
}
