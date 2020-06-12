/*
 * This code tests placing non-initialized, non-zero variables in PSRAM.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp32/spiram.h"
#include "esp_system.h"
#include "unity.h"

#if CONFIG_SPIRAM_ALLOW_NOINIT_EXTERNAL_MEMORY

#define TEST_BUFFER_SIZE (16*1024/4)
static EXT_RAM_NOINIT_ATTR uint32_t s_noinit_buffer[TEST_BUFFER_SIZE];

static void write_spiram_and_reset(void)
{
    // Fill the noinit buffer
    printf("Filling buffer\n");
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++) {
        s_noinit_buffer[i] = i ^ 0x55555555U;
    }
    printf("Flushing cache\n");
    // Flush the cache out to SPIRAM before resetting.
    esp_spiram_writeback_cache();

    printf("Restarting\n");
    // Reset to test that noinit memory is left intact.
    esp_restart();
}

static void check_spiram_contents(void)
{
    // Confirm that the memory contents are still what we expect
    uint32_t error_count = 0;
    for (uint32_t i = 0; i < TEST_BUFFER_SIZE; i++) {
        if (s_noinit_buffer[i] != (i ^ 0x55555555U)) {
            error_count++;
        }
    }

    printf("Found %" PRIu32 " memory errors\n", error_count);
    TEST_ASSERT(error_count == 0);
}

TEST_CASE_MULTIPLE_STAGES("Spiram test noinit memory", "[spiram]", write_spiram_and_reset, check_spiram_contents);

#endif // CONFIG_SPIRAM_ALLOW_NOINIT_EXTERNAL_MEMORY
