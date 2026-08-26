/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "unity.h"
#include "esp_image_format.h"
#include "esp_memory_utils.h"
#include "soc/soc.h"
#include "lp_core_hp_mem_app.h"
#include "ulp_lp_core.h"

#define HP_MEM_DATA_INIT   UINT32_C(0x12345678)
#define HP_MEM_DATA_DONE   UINT32_C(0x89ABCDEF)
#define HP_MEM_BSS_DONE    UINT32_C(0x13579BDF)
#define HP_MEM_DONE_MARKER UINT32_C(0x2468ACE0)

#if CONFIG_IDF_TARGET_ESP32P4
extern uint32_t _ulp_hp_mem_resv_end;
#endif

extern uint32_t _ulp_hp_mem_resv_start;

extern const uint8_t lp_core_hp_mem_bin_start[] asm("_binary_lp_core_hp_mem_app_bin_start");
extern const uint8_t lp_core_hp_mem_bin_end[]   asm("_binary_lp_core_hp_mem_app_bin_end");

static uintptr_t hp_mem_raw_start(void)
{
    return (uintptr_t)&_ulp_hp_mem_resv_start;
}

static uintptr_t hp_mem_raw_end(void)
{
#if CONFIG_IDF_TARGET_ESP32P4
    return (uintptr_t)&_ulp_hp_mem_resv_end;
#else
    return hp_mem_raw_start() + CONFIG_ULP_COPROC_RESERVE_HP_MEM_BYTES;
#endif
}

static uintptr_t hp_mem_access_start(void)
{
    uintptr_t start = hp_mem_raw_start();
#if CONFIG_IDF_TARGET_ESP32P4
    start += SOC_NON_CACHEABLE_OFFSET_SRAM;
#endif
    return start;
}

static uintptr_t hp_mem_access_end(void)
{
    uintptr_t end = hp_mem_raw_end();
#if CONFIG_IDF_TARGET_ESP32P4
    end += SOC_NON_CACHEABLE_OFFSET_SRAM;
#endif
    return end;
}

static bool addr_in_range(uintptr_t addr, uintptr_t start, uintptr_t end)
{
    return addr >= start && addr < end;
}

/* Verify that an LP Core variable pointer is in HP SRAM, not LP SRAM.
 * On P4, mapgen applies the non-cacheable alias offset (+0x40000000) to HP SRAM
 * symbols so the HP CPU can access them without L1 cache coherency issues.
 * Strip that offset before calling esp_ptr_in_dram(), which checks the HP SRAM range. */
static bool lp_var_in_hp_sram(const void *ptr)
{
#if CONFIG_IDF_TARGET_ESP32P4
    ptr = (const void *)((uintptr_t)ptr - SOC_NON_CACHEABLE_OFFSET_SRAM);
#endif
    return esp_ptr_in_dram(ptr);
}

static uint32_t read_ulp_word(const uint32_t *symbol)
{
    return *(volatile const uint32_t *)symbol;
}

static void assert_segment_layout(const uint8_t *program_binary, size_t program_size_bytes)
{
    const esp_image_header_t *image_header = (const esp_image_header_t *)program_binary;
    const uintptr_t hp_start = hp_mem_raw_start();
    const uintptr_t hp_end = hp_mem_raw_end();
    const uintptr_t lp_start = SOC_RTC_DRAM_LOW;
    const uintptr_t lp_end = lp_start + CONFIG_ULP_COPROC_RESERVE_MEM;
    const uint8_t *seg_ptr = program_binary + sizeof(esp_image_header_t);
    bool saw_lp_segment = false;
    bool saw_hp_segment = false;

    TEST_ASSERT_EQUAL_HEX8(ESP_IMAGE_HEADER_MAGIC, image_header->magic);

    for (int i = 0; i < image_header->segment_count; i++) {
        const esp_image_segment_header_t *seg_hdr = (const esp_image_segment_header_t *)seg_ptr;
        seg_ptr += sizeof(esp_image_segment_header_t);

        TEST_ASSERT_TRUE(((seg_ptr - program_binary) + seg_hdr->data_len) <= program_size_bytes);

        printf("LP segment %d load_addr=0x%08" PRIx32 " size=0x%08" PRIx32 "\n",
               i, seg_hdr->load_addr, seg_hdr->data_len);

        if (addr_in_range(seg_hdr->load_addr, hp_start, hp_end)) {
            saw_hp_segment = true;
        } else if (addr_in_range(seg_hdr->load_addr, lp_start, lp_end)) {
            saw_lp_segment = true;
        }

        seg_ptr += seg_hdr->data_len;
    }

    TEST_ASSERT_TRUE(saw_hp_segment);
    TEST_ASSERT_TRUE(saw_lp_segment);
}

TEST_CASE("LP-Core can run from HP memory", "[lp_core][hp_mem]")
{
    const uint8_t *program_binary = lp_core_hp_mem_bin_start;
    const size_t program_size_bytes = lp_core_hp_mem_bin_end - lp_core_hp_mem_bin_start;
    const uintptr_t hp_access_start = hp_mem_access_start();
    const uintptr_t hp_access_end = hp_mem_access_end();
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU,
    };

    /* The LP binary must exceed the LP RAM reservation — it can only run from HP RAM */
    TEST_ASSERT_GREATER_THAN(CONFIG_ULP_COPROC_RESERVE_MEM, program_size_bytes);
    printf("LP Core binary size: 0x%08" PRIx32 " bytes\n", (uint32_t)program_size_bytes);

    assert_segment_layout(program_binary, program_size_bytes);

    printf("HP raw memory range:    0x%08" PRIxPTR " - 0x%08" PRIxPTR "\n", hp_mem_raw_start(), hp_mem_raw_end());
    printf("HP access memory range: 0x%08" PRIxPTR " - 0x%08" PRIxPTR "\n", hp_access_start, hp_access_end);
    printf("ULP reset vector:       %p\n", &ulp_reset_vector);
    printf("ULP data marker:        %p\n", &ulp_hp_mem_data_marker);
    printf("ULP bss marker:         %p\n", &ulp_hp_mem_bss_marker);
    printf("ULP done marker:        %p\n", &ulp_hp_mem_done_marker);

    /* Address range checks using the HP access alias */
    TEST_ASSERT_FALSE(addr_in_range((uintptr_t)&ulp_reset_vector, hp_access_start, hp_access_end));
    TEST_ASSERT_TRUE(addr_in_range((uintptr_t)&ulp_hp_mem_data_marker, hp_access_start, hp_access_end));
    TEST_ASSERT_TRUE(addr_in_range((uintptr_t)&ulp_hp_mem_bss_marker, hp_access_start, hp_access_end));
    TEST_ASSERT_TRUE(addr_in_range((uintptr_t)&ulp_hp_mem_done_marker, hp_access_start, hp_access_end));

    /* Verify HP mem variables are in HP SRAM (DRAM), not LP SRAM */
    TEST_ASSERT_TRUE(lp_var_in_hp_sram(&ulp_hp_mem_data_marker));
    TEST_ASSERT_TRUE(lp_var_in_hp_sram(&ulp_hp_mem_bss_marker));
    TEST_ASSERT_TRUE(lp_var_in_hp_sram(&ulp_hp_mem_done_marker));
    TEST_ASSERT_FALSE(lp_var_in_hp_sram(&ulp_reset_vector));

    TEST_ASSERT_EQUAL(ESP_OK, ulp_lp_core_load_binary(program_binary, program_size_bytes));

    TEST_ASSERT_EQUAL_HEX32(HP_MEM_DATA_INIT, read_ulp_word(&ulp_hp_mem_data_marker));
    TEST_ASSERT_EQUAL_HEX32(0, read_ulp_word(&ulp_hp_mem_bss_marker));
    TEST_ASSERT_EQUAL_HEX32(0, read_ulp_word(&ulp_hp_mem_done_marker));

    TEST_ASSERT_EQUAL(ESP_OK, ulp_lp_core_run(&cfg));
    vTaskDelay(pdMS_TO_TICKS(1000));

    TEST_ASSERT_EQUAL_HEX32(HP_MEM_DATA_DONE, read_ulp_word(&ulp_hp_mem_data_marker));
    TEST_ASSERT_EQUAL_HEX32(HP_MEM_BSS_DONE, read_ulp_word(&ulp_hp_mem_bss_marker));
    TEST_ASSERT_EQUAL_HEX32(HP_MEM_DONE_MARKER, read_ulp_word(&ulp_hp_mem_done_marker));
}
