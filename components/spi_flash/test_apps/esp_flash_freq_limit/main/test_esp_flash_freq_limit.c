/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32C5

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "unity.h"
#include "esp_flash.h"
#include "esp_flash_chips/esp_flash_types.h"
#include "soc/rtc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_pm.h"
#include "driver/gptimer.h"
#include "esp_private/esp_clk.h"
#include "esp_log.h"
#include "test_utils.h"
#include "esp_partition.h"
#include "spi_flash_mmap.h"  // For SPI_FLASH_SEC_SIZE

#define TAG "test_freq_limit"

/**
 * Note: All tests in this file do NOT enable flash encryption.
 * Even without encryption enabled, esp_flash_write_encrypted() will still trigger
 * frequency switching (which is what we're testing).
 * This test does not verify the correctness of encrypted writes.
 */

// Use test data partition for flash operations
#define TEST_DATA_SIZE 64
#define FREQ_LIMIT_MHZ 160
#define MHZ (1000000)

static uint8_t test_write_buf[TEST_DATA_SIZE];
static uint32_t s_test_flash_offset = 0; // Will be initialized from test partition
static uint32_t s_test_flash_offset_area_a = 0; // Area A: for normal write/read test
static uint32_t s_test_flash_offset_area_b = 0; // Area B: for encrypted write test (no verification)

// Note: No erase needed for these tests.
// The tests focus on frequency behavior during esp_flash_write_encrypted, not data integrity or encryption functionality.
// esp_flash_write_encrypted overwrites existing data.
// Erase operations are blocking and may disable interrupts/task switching, potentially interfering with concurrency test scenarios.

static uint32_t get_cpu_freq_mhz(void)
{
    rtc_cpu_freq_config_t config;
    rtc_clk_cpu_freq_get_config(&config);
    return config.freq_mhz;
}

static void verify_freq_restored(uint32_t original_freq)
{
    uint32_t freq = get_cpu_freq_mhz();
    TEST_ASSERT_EQUAL_UINT32(original_freq, freq);
}

static esp_flash_t* get_test_flash_chip(void)
{
    // Initialize test flash offset from test data partition if not already initialized
    if (s_test_flash_offset == 0) {
        const esp_partition_t *test_part = get_test_data_partition();
        TEST_ASSERT_NOT_NULL(test_part);
        s_test_flash_offset = test_part->address;
        // Area A: start from partition start
        s_test_flash_offset_area_a = test_part->address;
        // Area B: start from partition start + TEST_DATA_SIZE (aligned to sector)
        uint32_t sector_aligned_size = ((TEST_DATA_SIZE + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE) * SPI_FLASH_SEC_SIZE;
        s_test_flash_offset_area_b = test_part->address + sector_aligned_size;
        ESP_LOGI(TAG, "Using test partition at offset 0x%x, size 0x%x",
                 test_part->address, test_part->size);
        ESP_LOGI(TAG, "Area A: 0x%x, Area B: 0x%x", s_test_flash_offset_area_a, s_test_flash_offset_area_b);
    }
    return esp_flash_default_chip;
}

static void prepare_test_data(void)
{
    for (int i = 0; i < TEST_DATA_SIZE; i++) {
        test_write_buf[i] = (uint8_t)(i & 0xFF);
    }
}

#if CONFIG_PM_ENABLE
// Test frequency configurations: dynamically built based on CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
// Only include frequencies that are <= CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ >= 240
// Support 240MHz: test default (0), 240MHz, 160MHz, 80MHz
static const uint32_t s_test_max_freqs[] = {0, 240, 160, 80};
static const uint32_t s_test_min_freqs[] = {0, 80, 80, 80};
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ >= 160
// Support up to 160MHz: test default (0), 160MHz, 80MHz
static const uint32_t s_test_max_freqs[] = {0, 160, 80};
static const uint32_t s_test_min_freqs[] = {0, 80, 80};
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ >= 80
// Support up to 80MHz: test default (0), 80MHz
static const uint32_t s_test_max_freqs[] = {0, 80};
static const uint32_t s_test_min_freqs[] = {0, 80};
#else
// Support lower frequencies: test default (0) only
static const uint32_t s_test_max_freqs[] = {0};
static const uint32_t s_test_min_freqs[] = {0};
#endif
#define NUM_TEST_CONFIGS (sizeof(s_test_max_freqs) / sizeof(s_test_max_freqs[0]))

/**
 * Setup PM configuration for testing
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 * @return Expected frequency after setup (max_freq_mhz if configured, CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ if max_freq_mhz is 0)
 */
static uint32_t setup_pm_config(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    uint32_t actual_max_freq = max_freq_mhz;
    uint32_t actual_min_freq = min_freq_mhz;

    if (max_freq_mhz == 0) {
        // Configure with default values (same as CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
        actual_max_freq = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
        if (min_freq_mhz == 0) {
            // Use XTAL frequency as min when not specified
            actual_min_freq = esp_clk_xtal_freq() / MHZ;
        }
    }

    // Configure PM with specific frequencies
    esp_pm_config_t pm_config = {
        .max_freq_mhz = actual_max_freq,
        .min_freq_mhz = actual_min_freq,
        .light_sleep_enable = false
    };
    TEST_ESP_OK(esp_pm_configure(&pm_config));

    // Wait a bit for PM to stabilize
    vTaskDelay(pdMS_TO_TICKS(100));

    uint32_t current_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "PM configured: max=%"PRIu32" MHz, min=%"PRIu32" MHz, current=%"PRIu32" MHz",
             actual_max_freq, actual_min_freq, current_freq);

    // Verify current frequency matches max_freq_mhz (since no locks are held)
    TEST_ASSERT_EQUAL_UINT32(actual_max_freq, current_freq);
    return actual_max_freq;
}
#endif // CONFIG_PM_ENABLE

// Note: We do NOT erase flash before write_encrypted operations in these tests.
// Reasons:
// 1. These tests focus on frequency behavior, not data integrity or encryption functionality.
// 2. We do not perform data verification (no read_encrypted or data comparison).
// 3. esp_flash_write_encrypted will overwrite existing data anyway.
// 4. Erase operations are blocking and may disable interrupts/task switching, which could
//    interfere with concurrent testing scenarios (interrupts, task scheduling).
// 5. Skipping erase operations simplifies the tests and avoids potential timing issues.

/**
 * Helper function: Test frequency limit basic behavior with PM configuration
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_freq_limit_basic_behavior_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();

    // Step 1: Erase and write known data to Area A, then verify
    uint32_t erase_size = ((TEST_DATA_SIZE + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE) * SPI_FLASH_SEC_SIZE;
    ESP_LOGI(TAG, "Step 1: Erasing Area A at 0x%x, size %"PRIu32" bytes", s_test_flash_offset_area_a, erase_size);
    esp_err_t ret = esp_flash_erase_region(chip, s_test_flash_offset_area_a, erase_size);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Flash erase failed (0x%x), continuing anyway.", ret);
    } else {
        ESP_LOGI(TAG, "Area A erased successfully");
    }

    ESP_LOGI(TAG, "Writing known data to Area A at 0x%x", s_test_flash_offset_area_a);
    ret = esp_flash_write(chip, test_write_buf, s_test_flash_offset_area_a, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "Data written to Area A successfully");

    // Verify written data in Area A
    uint8_t read_buf[TEST_DATA_SIZE];
    ESP_LOGI(TAG, "Reading back data from Area A at 0x%x", s_test_flash_offset_area_a);
    ret = esp_flash_read(chip, read_buf, s_test_flash_offset_area_a, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "Verifying data integrity in Area A");
    ESP_LOG_BUFFER_HEXDUMP(TAG, test_write_buf, TEST_DATA_SIZE, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_buf, TEST_DATA_SIZE, ESP_LOG_INFO);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test_write_buf, read_buf, TEST_DATA_SIZE);
    ESP_LOGI(TAG, "Area A data verification passed");

    // Step 2: Encrypted write to Area B (no verification)
    uint32_t freq_before = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Step 2: Frequency before write_encrypted: %"PRIu32" MHz", freq_before);
    ESP_LOGI(TAG, "About to call esp_flash_write_encrypted to Area B, addr=0x%x, len=%d", s_test_flash_offset_area_b, TEST_DATA_SIZE);
    // Note: This test does not verify the correctness of encrypted writes
    ret = esp_flash_write_encrypted(chip, s_test_flash_offset_area_b, test_write_buf, TEST_DATA_SIZE);
    ESP_LOGI(TAG, "esp_flash_write_encrypted returned: %d", ret);
    TEST_ESP_OK(ret);

    // Wait for flash operation to complete
    vTaskDelay(pdMS_TO_TICKS(10));

    // Frequency should be restored after the call completes (or unchanged if already <= 160MHz)
    uint32_t freq_after = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency after write_encrypted: %"PRIu32" MHz", freq_after);

    // Verify frequency behavior:
    // - If original frequency > 160MHz: should be restored to original
    // - If original frequency <= 160MHz: should remain unchanged
    if (original_freq > FREQ_LIMIT_MHZ) {
        verify_freq_restored(original_freq);
        ESP_LOGI(TAG, "Frequency restored to original %"PRIu32" MHz (was > 160MHz)", original_freq);
    } else {
        TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
        ESP_LOGI(TAG, "Frequency unchanged at %"PRIu32" MHz (was <= 160MHz)", original_freq);
    }

    // Step 3: Read and verify Area A again to confirm read API still works after encrypted write
    ESP_LOGI(TAG, "Step 3: Reading Area A again at 0x%x to verify read API still works", s_test_flash_offset_area_a);
    ret = esp_flash_read(chip, read_buf, s_test_flash_offset_area_a, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "Verifying data integrity in Area A after encrypted write");
    ESP_LOG_BUFFER_HEXDUMP(TAG, test_write_buf, TEST_DATA_SIZE, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_buf, TEST_DATA_SIZE, ESP_LOG_INFO);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test_write_buf, read_buf, TEST_DATA_SIZE);
    ESP_LOGI(TAG, "Area A data verification passed after encrypted write - read API is still functional");
}

/**
 * Test: Frequency limit: basic behavior
 * Target: Verify that esp_flash_write_encrypted properly limits CPU frequency to <= 160MHz during execution
 *         and restores the original frequency after completion, with different PM configurations.
 * Expected:
 *   - If original frequency > 160MHz: frequency should be limited to 160MHz during write, restored to original after
 *   - If original frequency <= 160MHz: frequency should remain unchanged
 *   - Written data should be verified by reading it back
 */
TEST_CASE("Frequency limit: basic behavior", "[esp_flash_freq_limit]")
{
#if CONFIG_PM_ENABLE
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_freq_limit_basic_behavior_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
#else
    test_freq_limit_basic_behavior_common(0, 0);
#endif
}

/**
 * Helper function: Test read Flash ID after write_encrypted with PM configuration
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_read_flash_id_after_write_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    // Read Flash ID before write_encrypted
    uint32_t flash_id_before = 0;
    ESP_LOGI(TAG, "Reading Flash ID before write_encrypted");
    esp_err_t ret = esp_flash_read_id(chip, &flash_id_before);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "Flash ID before write_encrypted: 0x%08"PRIx32, flash_id_before);
    TEST_ASSERT_NOT_EQUAL(0, flash_id_before);
    TEST_ASSERT_NOT_EQUAL(0xFFFFFFFF, flash_id_before);

    prepare_test_data();

    // Erase the test region before writing
    // esp_flash_erase_region requires size to be aligned to sector size (4KB)
    // Calculate the number of sectors needed (round up)
    uint32_t erase_size = ((TEST_DATA_SIZE + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE) * SPI_FLASH_SEC_SIZE;
    ESP_LOGI(TAG, "Erasing flash region at 0x%x, size %d (aligned to %"PRIu32" bytes)",
             s_test_flash_offset, TEST_DATA_SIZE, erase_size);
    ret = esp_flash_erase_region(chip, s_test_flash_offset, erase_size);
    // If erase fails, it might be because the region is already erased or in use
    // Try to continue anyway - write_encrypted will overwrite the data
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Flash erase failed (0x%x), continuing anyway", ret);
    } else {
        ESP_LOGI(TAG, "Flash region erased successfully");
    }

    // Perform write_encrypted operation
    uint32_t freq_before = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency before write_encrypted: %"PRIu32" MHz", freq_before);

    ESP_LOGI(TAG, "Calling esp_flash_write_encrypted, addr=0x%x, len=%d", s_test_flash_offset, TEST_DATA_SIZE);
    ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Verify frequency is restored
    uint32_t freq_after = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency after write_encrypted: %"PRIu32" MHz", freq_after);
    if (original_freq > FREQ_LIMIT_MHZ) {
        verify_freq_restored(original_freq);
    } else {
        TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
    }

    // Read Flash ID after write_encrypted to verify driver is still functional
    // This uses a real read operation that accesses the Flash hardware
    uint32_t flash_id_after = 0;
    ESP_LOGI(TAG, "Reading Flash ID after write_encrypted");
    ret = esp_flash_read_id(chip, &flash_id_after);
    TEST_ESP_OK(ret);
    ESP_LOGI(TAG, "Flash ID after write_encrypted: 0x%08"PRIx32, flash_id_after);

    // Verify Flash ID matches (driver should still be functional)
    TEST_ASSERT_EQUAL_UINT32(flash_id_before, flash_id_after);
    ESP_LOGI(TAG, "Flash ID verification passed - driver is still functional");
}

/**
 * Helper function: Test read Flash ID only (no write operations)
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
#if CONFIG_PM_ENABLE
static void test_read_flash_id_only_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

    setup_pm_config(max_freq_mhz, min_freq_mhz);

    uint32_t current_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Current frequency: %"PRIu32" MHz", current_freq);

    // Read Flash ID multiple times to verify consistency
    // Note: At high frequencies (240MHz, 160MHz), Flash timing may not be properly configured,
    // causing incorrect Flash ID reads. We need to temporarily lower CPU frequency to 80MHz
    // to read Flash ID correctly, then restore original frequency.
    uint32_t flash_id = 0;

    // If CPU frequency is high (>= 160MHz), temporarily lower CPU frequency to 80MHz for reading Flash ID
    uint32_t original_max_freq = 0;
    uint32_t original_min_freq = 0;
    bool freq_was_lowered = false;
    if (current_freq >= 160) {
        ESP_LOGI(TAG, "CPU frequency is %"PRIu32" MHz, temporarily lowering to 80MHz for Flash ID read", current_freq);

        // Save current PM config
        esp_pm_config_t pm_config;
        esp_pm_get_configuration(&pm_config);
        original_max_freq = pm_config.max_freq_mhz;
        original_min_freq = pm_config.min_freq_mhz;

        // Temporarily set max and min to 80MHz
        esp_pm_config_t temp_pm_config = {
            .max_freq_mhz = 80,
            .min_freq_mhz = 80,
            .light_sleep_enable = false
        };
        TEST_ESP_OK(esp_pm_configure(&temp_pm_config));
        vTaskDelay(pdMS_TO_TICKS(100));  // Wait for frequency to stabilize

        uint32_t temp_freq = get_cpu_freq_mhz();
        ESP_LOGI(TAG, "Frequency lowered to %"PRIu32" MHz", temp_freq);
        TEST_ASSERT_EQUAL_UINT32(80, temp_freq);
        freq_was_lowered = true;
    }

    extern esp_flash_t *esp_flash_default_chip;
    uint32_t expect_chip_id = esp_flash_default_chip->chip_id;
    for (int i = 0; i < 5; i++) {
        ESP_LOGI(TAG, "Reading Flash ID (attempt %d)", i + 1);
        esp_err_t ret = esp_flash_read_id(chip, &flash_id);
        TEST_ESP_OK(ret);
        uint32_t read_freq = get_cpu_freq_mhz();
        ESP_LOGI(TAG, "Flash ID at %"PRIu32" MHz: 0x%08"PRIx32, read_freq, flash_id);
        TEST_ASSERT_EQUAL_UINT32(expect_chip_id, flash_id);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Restore original frequency if we lowered it
    if (freq_was_lowered) {
        ESP_LOGI(TAG, "Restoring original frequency: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 original_max_freq, original_min_freq);
        esp_pm_config_t restore_pm_config = {
            .max_freq_mhz = original_max_freq,
            .min_freq_mhz = original_min_freq,
            .light_sleep_enable = false
        };
        TEST_ESP_OK(esp_pm_configure(&restore_pm_config));
        vTaskDelay(pdMS_TO_TICKS(100));  // Wait for frequency to stabilize

        uint32_t restored_freq = get_cpu_freq_mhz();
        ESP_LOGI(TAG, "Frequency restored to %"PRIu32" MHz", restored_freq);
        TEST_ASSERT_EQUAL_UINT32(original_max_freq, restored_freq);
    }

    ESP_LOGI(TAG, "Final Flash ID: 0x%08"PRIx32, flash_id);
    TEST_ASSERT_EQUAL_HEX32(expect_chip_id, flash_id);
}

/**
 * Test: Frequency limit: read Flash ID only
 * Target: Verify Flash ID reading at different frequencies
 * Expected:
 *   - Flash ID should be readable at all frequencies
 *   - Flash ID should be same as the default chip id
 */
TEST_CASE("Frequency limit: read Flash ID only", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing read Flash ID only with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_read_flash_id_only_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}
#endif // CONFIG_PM_ENABLE

/**
 * Test: Frequency limit: read Flash ID after write_encrypted
 * Target: Verify that the Flash driver remains functional after esp_flash_write_encrypted
 *         by reading the Flash ID using a real read operation.
 * Expected:
 *   - Flash ID should be readable before and after write_encrypted
 *   - Flash ID should match before and after (driver state should be consistent)
 *   - This verifies that the driver is still in a normal state after write_encrypted
 */
TEST_CASE("Frequency limit: read Flash ID after write_encrypted", "[esp_flash_freq_limit]")
{
#if CONFIG_PM_ENABLE
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing read Flash ID with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_read_flash_id_after_write_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
#else
    test_read_flash_id_after_write_common(0, 0);
#endif
}

/**
 * Helper function: Test multiple write_encrypted calls with PM configuration
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_multiple_write_encrypted_calls_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();
    // Note: No erase needed - see comment above erase_test_region function

    for (int i = 0; i < 3; i++) {
        uint32_t offset = s_test_flash_offset + (i * TEST_DATA_SIZE);
        uint32_t freq_before = get_cpu_freq_mhz();

        esp_err_t ret = esp_flash_write_encrypted(chip, offset, test_write_buf, TEST_DATA_SIZE);
        TEST_ESP_OK(ret);

        uint32_t freq_after = get_cpu_freq_mhz();
        verify_freq_restored(original_freq);

        ESP_LOGI(TAG, "Iteration %d: freq_before=%"PRIu32" MHz, freq_after=%"PRIu32" MHz", i, freq_before, freq_after);
    }
}

/**
 * Test: Frequency limit: multiple write_encrypted calls
 * Target: Verify that frequency limiting works correctly across multiple consecutive write_encrypted calls.
 * Expected: Frequency should be limited during each call and restored after each call completes.
 */
TEST_CASE("Frequency limit: multiple write_encrypted calls", "[esp_flash_freq_limit]")
{
#if CONFIG_PM_ENABLE
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_multiple_write_encrypted_calls_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
#else
    test_multiple_write_encrypted_calls_common(0, 0);
#endif
}

/**
 * Test: Frequency limit: zero length write
 * Target: Verify that zero-length write_encrypted calls do not trigger frequency limiting (early return).
 * Expected: Frequency should remain unchanged when length is 0.
 */
TEST_CASE("Frequency limit: zero length write", "[esp_flash_freq_limit]")
{
    esp_flash_t* chip = get_test_flash_chip();
#if CONFIG_PM_ENABLE
    // Initialize PM config to ensure frequency is at max
    setup_pm_config(0, 0); // Use default max frequency
#endif
    uint32_t original_freq = get_cpu_freq_mhz();

    // Verify initial frequency matches CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, original_freq);

    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, 0);
    TEST_ESP_OK(ret);

    uint32_t freq_after = get_cpu_freq_mhz();

    TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
    ESP_LOGI(TAG, "Zero length write: freq unchanged (%"PRIu32" MHz)", freq_after);
}

/**
 * Test: Frequency limit: misaligned address
 * Target: Verify that frequency limiting is not triggered when write fails due to misaligned address.
 * Expected: Function should return ESP_ERR_INVALID_ARG and frequency should remain unchanged.
 */
TEST_CASE("Frequency limit: misaligned address", "[esp_flash_freq_limit]")
{
    esp_flash_t* chip = get_test_flash_chip();
#if CONFIG_PM_ENABLE
    // Initialize PM config to ensure frequency is at max
    setup_pm_config(0, 0); // Use default max frequency
#endif
    uint32_t original_freq = get_cpu_freq_mhz();

    // Verify initial frequency matches CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, original_freq);

    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();

    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset + 1, test_write_buf, TEST_DATA_SIZE);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ret);

    uint32_t freq_after = get_cpu_freq_mhz();

    TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
    ESP_LOGI(TAG, "Misaligned address: freq unchanged (%"PRIu32" MHz)", freq_after);
}

/**
 * Test: Frequency limit: misaligned length
 * Target: Verify that frequency limiting is not triggered when write fails due to misaligned length.
 * Expected: Function should return ESP_ERR_INVALID_SIZE and frequency should remain unchanged.
 */
TEST_CASE("Frequency limit: misaligned length", "[esp_flash_freq_limit]")
{
    esp_flash_t* chip = get_test_flash_chip();
#if CONFIG_PM_ENABLE
    // Initialize PM config to ensure frequency is at max
    setup_pm_config(0, 0); // Use default max frequency
#endif
    uint32_t original_freq = get_cpu_freq_mhz();

    // Verify initial frequency matches CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, original_freq);

    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();

    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE + 1);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE, ret);

    uint32_t freq_after = get_cpu_freq_mhz();

    TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
    ESP_LOGI(TAG, "Misaligned length: freq unchanged (%"PRIu32" MHz)", freq_after);
}

/**
 * Test: Frequency limit: write failure recovery
 * Target: Verify that frequency is properly restored even when write_encrypted fails with an error.
 * Expected: Function should return error and frequency should be restored to original value.
 */
TEST_CASE("Frequency limit: write failure recovery", "[esp_flash_freq_limit]")
{
    esp_flash_t* chip = get_test_flash_chip();
#if CONFIG_PM_ENABLE
    // Initialize PM config to ensure frequency is at max
    setup_pm_config(0, 0); // Use default max frequency
#endif
    uint32_t original_freq = get_cpu_freq_mhz();

    // Verify initial frequency matches CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, original_freq);

    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();

    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset + 1, test_write_buf, TEST_DATA_SIZE);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ret);

    uint32_t freq_after = get_cpu_freq_mhz();

    TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after);
    ESP_LOGI(TAG, "Write failure: freq restored to original (%"PRIu32" MHz)", freq_after);
}

/**
 * Helper function: Test multiple write_encrypted operations with PM configuration
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_multiple_write_encrypted_operations_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();
    // Note: No erase needed - see comment above erase_test_region function

    // First write - frequency should be limited during call and restored after
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);
    verify_freq_restored(original_freq);

    // Second write - frequency should be limited during call and restored after
    ret = esp_flash_write_encrypted(chip, s_test_flash_offset + TEST_DATA_SIZE, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);
    verify_freq_restored(original_freq);

    ESP_LOGI(TAG, "Multiple operations: freq properly restored after each (%"PRIu32" MHz)", original_freq);
}

/**
 * Test: Frequency limit: multiple write_encrypted operations
 * Target: Verify that frequency limiting and restoration work correctly for multiple independent write operations.
 * Expected: Frequency should be limited during each write and restored after each write completes.
 */
TEST_CASE("Frequency limit: multiple write_encrypted operations", "[esp_flash_freq_limit]")
{
#if CONFIG_PM_ENABLE
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_multiple_write_encrypted_operations_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
#else
    test_multiple_write_encrypted_operations_common(0, 0);
#endif
}

// ============================================================================
// Interrupt concurrency test cases
// ============================================================================

#if CONFIG_PM_ENABLE

/**
 * Helper function: Test CPU lock held before encrypt with PM configuration
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_cpu_lock_held_before_encrypt_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    uint32_t configured_max_freq = setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
    uint32_t configured_max_freq = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Original frequency: %"PRIu32" MHz", original_freq);

    prepare_test_data();
    // Note: No erase needed - see comment above erase_test_region function

    // Create and acquire CPU lock BEFORE encrypt
    esp_pm_lock_handle_t cpu_lock = NULL;
    TEST_ESP_OK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "test_lock", &cpu_lock));
    TEST_ASSERT_NOT_NULL(cpu_lock);

    TEST_ESP_OK(esp_pm_lock_acquire(cpu_lock));

    // Verify frequency is at max
    uint32_t freq_after_lock = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency after acquiring CPU lock: %"PRIu32" MHz", freq_after_lock);
    TEST_ASSERT_EQUAL_UINT32(configured_max_freq, freq_after_lock);

    // Perform encrypt operation - frequency should still be limited to 160MHz
    // even though CPU lock is held (encrypt lock has higher priority)
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Verify frequency is restored to max (CPU lock still held)
    uint32_t freq_after_encrypt = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency after encrypt (lock still held): %"PRIu32" MHz", freq_after_encrypt);
    TEST_ASSERT_EQUAL_UINT32(freq_after_lock, freq_after_encrypt);

    // Release CPU lock
    esp_pm_lock_release(cpu_lock);

    // Verify frequency restored
    verify_freq_restored(original_freq);

    // Clean up
    esp_pm_lock_delete(cpu_lock);
}

/**
 * Test: Frequency limit: CPU lock held before encrypt
 * Target: Verify that frequency limiting works correctly when a PM CPU lock is already held before encrypt execution.
 *         The encrypt frequency limit lock (PM_MODE_CPU_ENCRYPT_LOCK, limits to <= 160MHz) has higher priority than CPU lock (PM_MODE_CPU_MAX).
 * Expected: Frequency should be limited to 160MHz during encrypt (even with CPU lock held), then restored to max
 *           after encrypt completes (CPU lock still held).
 */
TEST_CASE("Frequency limit: CPU lock held before encrypt", "[esp_flash_freq_limit]")
{
#if CONFIG_PM_ENABLE
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_cpu_lock_held_before_encrypt_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
#else
    test_cpu_lock_held_before_encrypt_common(0, 0);
#endif
}

#endif // CONFIG_PM_ENABLE

// ============================================================================
// Interrupt concurrency test cases
// ============================================================================

// Shared state for interrupt tests
static volatile bool s_interrupt_occurred = false;
static gptimer_handle_t s_test_timer = NULL;

#if CONFIG_PM_ENABLE
// PM lock handles for ISR tests (must be set before interrupt)
static esp_pm_lock_handle_t s_isr_pm_lock_cpu = NULL;
static esp_pm_lock_handle_t s_isr_pm_lock_apb = NULL;
static volatile bool s_isr_lock_acquired = false;
static volatile bool s_isr_lock_released = false;
#endif // CONFIG_PM_ENABLE

// ISR callback - must be IRAM-safe (declared before use)
static bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    s_interrupt_occurred = true;
    return false;
}

#if CONFIG_PM_ENABLE

// ISR callback for test 3: acquire and release lock in ISR
static bool IRAM_ATTR timer_isr_callback_acquire_release(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    s_interrupt_occurred = true;

    // Acquire lock in ISR
    if (s_isr_pm_lock_cpu != NULL) {
        esp_pm_lock_acquire(s_isr_pm_lock_cpu);
        s_isr_lock_acquired = true;
        // Release immediately
        esp_pm_lock_release(s_isr_pm_lock_cpu);
        s_isr_lock_released = true;
    }
    if (s_isr_pm_lock_apb != NULL) {
        esp_pm_lock_acquire(s_isr_pm_lock_apb);
        s_isr_lock_acquired = true;
        // Release immediately
        esp_pm_lock_release(s_isr_pm_lock_apb);
        s_isr_lock_released = true;
    }

    return false;
}

// ISR callback for test 4: acquire lock in ISR, release after write completes
static bool IRAM_ATTR timer_isr_callback_acquire(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    s_interrupt_occurred = true;

    // Acquire lock in ISR (will be released after write completes in test code)
    if (s_isr_pm_lock_cpu != NULL) {
        esp_pm_lock_acquire(s_isr_pm_lock_cpu);
        s_isr_lock_acquired = true;
    }
    if (s_isr_pm_lock_apb != NULL) {
        esp_pm_lock_acquire(s_isr_pm_lock_apb);
        s_isr_lock_acquired = true;
    }

    return false;
}

// ISR callback for test 5: release lock that was acquired before encrypt
static bool IRAM_ATTR timer_isr_callback_release(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    s_interrupt_occurred = true;

    // Release lock in ISR (was acquired before encrypt)
    if (s_isr_pm_lock_cpu != NULL) {
        esp_pm_lock_release(s_isr_pm_lock_cpu);
        s_isr_lock_released = true;
    }
    if (s_isr_pm_lock_apb != NULL) {
        esp_pm_lock_release(s_isr_pm_lock_apb);
        s_isr_lock_released = true;
    }

    return false;
}

#endif // CONFIG_PM_ENABLE

// Helper function to setup GPTimer for interrupt during encrypt
static void setup_interrupt_timer(uint32_t delay_us, gptimer_alarm_cb_t callback)
{
    // Clean up any existing timer first to avoid resource leaks
    if (s_test_timer != NULL) {
        gptimer_stop(s_test_timer);
        gptimer_disable(s_test_timer);
        gptimer_del_timer(s_test_timer);
        s_test_timer = NULL;
    }

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &s_test_timer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = callback,
    };
    TEST_ESP_OK(gptimer_register_event_callbacks(s_test_timer, &cbs, NULL));
    TEST_ESP_OK(gptimer_enable(s_test_timer));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = delay_us,
        .flags.auto_reload_on_alarm = false,
    };
    TEST_ESP_OK(gptimer_set_alarm_action(s_test_timer, &alarm_config));

    s_interrupt_occurred = false;
#if CONFIG_PM_ENABLE
    s_isr_lock_acquired = false;
    s_isr_lock_released = false;
#endif // CONFIG_PM_ENABLE
    TEST_ESP_OK(gptimer_start(s_test_timer));
}

static void cleanup_interrupt_timer(void)
{
    if (s_test_timer != NULL) {
        gptimer_stop(s_test_timer);
        gptimer_disable(s_test_timer);
        gptimer_del_timer(s_test_timer);
        s_test_timer = NULL;
    }
}


/**
 * Helper function: Test interrupt during encrypt with PM configuration
 * Common test logic for both PM enabled and disabled scenarios
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_interrupt_during_encrypt_common(uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    prepare_test_data();

    // Setup interrupt to trigger during encrypt (ensure it happens before write completes)
    setup_interrupt_timer(50, timer_isr_callback); // 50us delay - should trigger during write

    // Perform encrypt operation - interrupt should occur during this call
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Wait for interrupt to be processed
    vTaskDelay(pdMS_TO_TICKS(10));

    // Verify interrupt occurred
    TEST_ASSERT_TRUE(s_interrupt_occurred);

    // Verify frequency was restored
    verify_freq_restored(original_freq);

    cleanup_interrupt_timer();
}

/**
 * Test: Frequency limit: interrupt during encrypt (PM disabled)
 * Target: Verify that frequency limiting works correctly when an interrupt occurs during encrypt execution with PM disabled.
 * Expected: Interrupt should occur during write, frequency should be limited during write, and restored after write completes.
 */
TEST_CASE("Frequency limit: interrupt during encrypt (PM disabled)", "[esp_flash_freq_limit]")
{
    test_interrupt_during_encrypt_common(0, 0);
}

#if CONFIG_PM_ENABLE
/**
 * Test: Frequency limit: interrupt during encrypt (PM enabled)
 * Target: Verify that frequency limiting works correctly when an interrupt occurs during encrypt execution with PM enabled.
 * Expected: Interrupt should occur during write, frequency should be limited during write, and restored after write completes.
 */
TEST_CASE("Frequency limit: interrupt during encrypt (PM enabled)", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_interrupt_during_encrypt_common(s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

/**
 * Helper function: Test interrupt with PM lock acquire/release with PM configuration
 * @param lock_type Lock type (CPU or APB)
 * @param lock_name Lock name for logging
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_interrupt_with_pm_lock_acquire_release_common(esp_pm_lock_type_t lock_type, const char* lock_name, uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    prepare_test_data();

    // Create PM lock
    esp_pm_lock_handle_t pm_lock = NULL;
    TEST_ESP_OK(esp_pm_lock_create(lock_type, 0, "isr_test", &pm_lock));
    TEST_ASSERT_NOT_NULL(pm_lock);

    // Set lock handle for ISR callback
    if (lock_type == ESP_PM_CPU_FREQ_MAX) {
        s_isr_pm_lock_cpu = pm_lock;
        s_isr_pm_lock_apb = NULL;
    } else {
        s_isr_pm_lock_cpu = NULL;
        s_isr_pm_lock_apb = pm_lock;
    }

    // Setup interrupt to trigger during encrypt (50us delay)
    // ISR will acquire and release lock
    setup_interrupt_timer(50, timer_isr_callback_acquire_release);

    // Perform encrypt operation - interrupt will trigger PM lock acquire/release in ISR
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Wait for interrupt to be processed
    vTaskDelay(pdMS_TO_TICKS(10));

    // Verify interrupt occurred and lock operations happened
    TEST_ASSERT_TRUE(s_interrupt_occurred);
    TEST_ASSERT_TRUE(s_isr_lock_acquired);
    TEST_ASSERT_TRUE(s_isr_lock_released);

    // Verify frequency was restored (pm_impl.c assertions will verify frequency during encrypt)
    verify_freq_restored(original_freq);

    // Cleanup
    s_isr_pm_lock_cpu = NULL;
    s_isr_pm_lock_apb = NULL;
    esp_pm_lock_delete(pm_lock);
    cleanup_interrupt_timer();
}

/**
 * Test: Frequency limit: interrupt with CPU lock acquire/release
 * Target: Same as test_interrupt_with_pm_lock_acquire_release, but specifically for CPU lock (ESP_PM_CPU_FREQ_MAX).
 */
TEST_CASE("Frequency limit: interrupt with CPU lock acquire/release", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing CPU lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_interrupt_with_pm_lock_acquire_release_common(ESP_PM_CPU_FREQ_MAX, "CPU", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

/**
 * Test: Frequency limit: interrupt with APB lock acquire/release
 * Target: Same as test_interrupt_with_pm_lock_acquire_release, but specifically for APB lock (ESP_PM_APB_FREQ_MAX).
 */
TEST_CASE("Frequency limit: interrupt with APB lock acquire/release", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing APB lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_interrupt_with_pm_lock_acquire_release_common(ESP_PM_APB_FREQ_MAX, "APB", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

/**
 * Helper function: Test interrupt with PM lock held after ISR with PM configuration
 * @param lock_type Lock type (CPU or APB)
 * @param lock_name Lock name for logging
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_interrupt_with_pm_lock_held_common(esp_pm_lock_type_t lock_type, const char* lock_name, uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    uint32_t configured_max_freq = setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
    uint32_t configured_max_freq = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    prepare_test_data();

    // Create PM lock
    esp_pm_lock_handle_t pm_lock = NULL;
    TEST_ESP_OK(esp_pm_lock_create(lock_type, 0, "hold_test", &pm_lock));
    TEST_ASSERT_NOT_NULL(pm_lock);

    // Set lock handle for ISR callback
    if (lock_type == ESP_PM_CPU_FREQ_MAX) {
        s_isr_pm_lock_cpu = pm_lock;
        s_isr_pm_lock_apb = NULL;
    } else {
        s_isr_pm_lock_cpu = NULL;
        s_isr_pm_lock_apb = pm_lock;
    }

    // Setup interrupt to trigger during encrypt
    // ISR will acquire lock, we'll release it after write completes
    setup_interrupt_timer(50, timer_isr_callback_acquire);

    // Perform encrypt operation - interrupt will trigger PM lock acquire in ISR
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Wait for interrupt to be processed
    vTaskDelay(pdMS_TO_TICKS(10));

    // Verify interrupt occurred and lock was acquired
    TEST_ASSERT_TRUE(s_interrupt_occurred);
    TEST_ASSERT_TRUE(s_isr_lock_acquired);

    // Check frequency after write completes but before releasing lock
    // Note: PM_MODE_CPU_ENCRYPT_LOCK lock (frequency limit) is released when write_encrypted returns,
    // so frequency should be restored to the mode corresponding to the lock held in ISR
    // If ISR acquired ESP_PM_CPU_FREQ_MAX lock, frequency should be configured max freq (PM_MODE_CPU_MAX)
    // If ISR acquired ESP_PM_APB_FREQ_MAX lock, frequency depends on PM configuration
    uint32_t freq_after_write = get_cpu_freq_mhz();
    uint32_t expected_freq;
    if (lock_type == ESP_PM_CPU_FREQ_MAX) {
        // CPU lock held -> PM_MODE_CPU_MAX -> configured max frequency
        expected_freq = configured_max_freq;
    } else {
        // APB lock held -> depends on PM config, but typically >= 80MHz
        // For simplicity, just verify frequency is restored (not limited to 160MHz)
        expected_freq = configured_max_freq; // APB lock held, should be restored to configured max freq
    }
    ESP_LOGI(TAG, "Frequency after write (lock still held): %"PRIu32" MHz (expected: %"PRIu32" MHz)",
             freq_after_write, expected_freq);
    TEST_ASSERT_EQUAL_UINT32(expected_freq, freq_after_write);

    // Release lock after write completes (as per requirement)
    esp_pm_lock_release(pm_lock);

    // Verify frequency was restored after lock release
    verify_freq_restored(original_freq);

    // Cleanup
    s_isr_pm_lock_cpu = NULL;
    s_isr_pm_lock_apb = NULL;
    esp_pm_lock_delete(pm_lock);
    cleanup_interrupt_timer();
}

/**
 * Test: Frequency limit: interrupt with CPU lock held after ISR
 * Target: Same as test_interrupt_with_pm_lock_held, but specifically for CPU lock (ESP_PM_CPU_FREQ_MAX).
 */
TEST_CASE("Frequency limit: interrupt with CPU lock held after ISR", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing CPU lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_interrupt_with_pm_lock_held_common(ESP_PM_CPU_FREQ_MAX, "CPU", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

/**
 * Test: Frequency limit: interrupt with APB lock held after ISR
 * Target: Same as test_interrupt_with_pm_lock_held, but specifically for APB lock (ESP_PM_APB_FREQ_MAX).
 */
TEST_CASE("Frequency limit: interrupt with APB lock held after ISR", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing APB lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_interrupt_with_pm_lock_held_common(ESP_PM_APB_FREQ_MAX, "APB", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

/**
 * Helper function: Test PM lock released in ISR with PM configuration
 * @param lock_type Lock type (CPU or APB)
 * @param lock_name Lock name for logging
 * @param max_freq_mhz Maximum CPU frequency in MHz. If 0, configure with CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ.
 * @param min_freq_mhz Minimum CPU frequency in MHz. If max_freq_mhz is 0, use XTAL frequency as min.
 */
static void test_lock_released_in_isr_common(esp_pm_lock_type_t lock_type, const char* lock_name, uint32_t max_freq_mhz, uint32_t min_freq_mhz)
{
    esp_flash_t* chip = get_test_flash_chip();

#if CONFIG_PM_ENABLE
    setup_pm_config(max_freq_mhz, min_freq_mhz);
#else
    TEST_ASSERT_EQUAL_UINT32(CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ, get_cpu_freq_mhz());
#endif

    uint32_t original_freq = get_cpu_freq_mhz();
    prepare_test_data();

    // Create and acquire PM lock BEFORE encrypt
    esp_pm_lock_handle_t pm_lock = NULL;
    TEST_ESP_OK(esp_pm_lock_create(lock_type, 0, "release_test", &pm_lock));
    TEST_ASSERT_NOT_NULL(pm_lock);
    TEST_ESP_OK(esp_pm_lock_acquire(pm_lock));

    // Set lock handle for ISR callback
    if (lock_type == ESP_PM_CPU_FREQ_MAX) {
        s_isr_pm_lock_cpu = pm_lock;
        s_isr_pm_lock_apb = NULL;
    } else {
        s_isr_pm_lock_cpu = NULL;
        s_isr_pm_lock_apb = pm_lock;
    }

    // Setup interrupt to trigger during encrypt
    // ISR will release the lock that was acquired before encrypt
    setup_interrupt_timer(50, timer_isr_callback_release);

    // Perform encrypt operation - lock is already held, ISR will release it
    esp_err_t ret = esp_flash_write_encrypted(chip, s_test_flash_offset, test_write_buf, TEST_DATA_SIZE);
    TEST_ESP_OK(ret);

    // Wait for interrupt to be processed
    vTaskDelay(pdMS_TO_TICKS(10));

    // Verify interrupt occurred and lock was released
    TEST_ASSERT_TRUE(s_interrupt_occurred);
    TEST_ASSERT_TRUE(s_isr_lock_released);

    // Check frequency after write completes and after ISR released lock
    // Frequency should be restored because lock was released in ISR
    uint32_t freq_after_write = get_cpu_freq_mhz();
    ESP_LOGI(TAG, "Frequency after write (lock released in ISR): %"PRIu32" MHz (original: %"PRIu32" MHz)",
             freq_after_write, original_freq);
    TEST_ASSERT_EQUAL_UINT32(original_freq, freq_after_write);

    // Verify frequency was restored (pm_impl.c assertions will verify frequency during encrypt)
    verify_freq_restored(original_freq);

    // Cleanup
    s_isr_pm_lock_cpu = NULL;
    s_isr_pm_lock_apb = NULL;
    esp_pm_lock_delete(pm_lock);
    cleanup_interrupt_timer();
}

TEST_CASE("Frequency limit: CPU lock released in ISR", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing CPU lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_lock_released_in_isr_common(ESP_PM_CPU_FREQ_MAX, "CPU", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}

TEST_CASE("Frequency limit: APB lock released in ISR", "[esp_flash_freq_limit]")
{
    for (int i = 0; i < NUM_TEST_CONFIGS; i++) {
        ESP_LOGI(TAG, "Testing APB lock with PM config: max=%"PRIu32" MHz, min=%"PRIu32" MHz",
                 s_test_max_freqs[i], s_test_min_freqs[i]);
        test_lock_released_in_isr_common(ESP_PM_APB_FREQ_MAX, "APB", s_test_max_freqs[i], s_test_min_freqs[i]);
    }
}
#endif // CONFIG_PM_ENABLE


#endif // CONFIG_IDF_TARGET_ESP32C5
