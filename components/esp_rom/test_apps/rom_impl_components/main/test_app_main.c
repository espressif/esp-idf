/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "unity_test_runner.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/wdt_hal.h"
#include "hal/systimer_hal.h"

static bool fn_in_rom(void *fn)
{
    const int fnaddr = (int)fn;
    return (fnaddr >= SOC_IROM_MASK_LOW && fnaddr < SOC_IROM_MASK_HIGH);
}

#if CONFIG_HAL_WDT_USE_ROM_IMPL
TEST_CASE("Test that WDT implementation from ROM is used", "[rom-impl-components]")
{
    TEST_ASSERT_TRUE(fn_in_rom(wdt_hal_feed));
}

#else
TEST_CASE("Test that WDT implementation from ROM is NOT used", "[rom-impl-components]")
{
    TEST_ASSERT_FALSE(fn_in_rom(wdt_hal_feed));
}
#endif // CONFIG_HAL_WDT_USE_ROM_IMPL


#if CONFIG_HAL_SYSTIMER_USE_ROM_IMPL
TEST_CASE("Test that systimer implementation from ROM is used", "[rom-impl-components]")
{
    TEST_ASSERT_TRUE(fn_in_rom(systimer_hal_get_counter_value));
}

#else
TEST_CASE("Test that systimer implementation from ROM is NOT used", "[rom-impl-components]")
{
    TEST_ASSERT_FALSE(fn_in_rom(systimer_hal_get_counter_value));
}
#endif // CONFIG_HAL_SYSTIMER_USE_ROM_IMPL

extern uint32_t tlsf_create;
#if CONFIG_HEAP_TLSF_USE_ROM_IMPL
TEST_CASE("Test that HEAP implementation from ROM is used", "[rom-impl-components]")
{
    TEST_ASSERT_TRUE(fn_in_rom((&tlsf_create)));
}

#else
TEST_CASE("Test that HEAP implementation from ROM is NOT used", "[rom-impl-components]")
{
    TEST_ASSERT_FALSE(fn_in_rom(&tlsf_create));
}
#endif // CONFIG_HEAP_TLSF_USE_ROM_IMPL


extern uint32_t spi_flash_chip_generic_probe;
#if CONFIG_SPI_FLASH_ROM_IMPL
TEST_CASE("Test that SPI flash implementation from ROM is used", "[rom-impl-components]")
{
    TEST_ASSERT_TRUE(fn_in_rom(&spi_flash_chip_generic_probe));
}

#else
TEST_CASE("Test that SPI flash implementation from ROM is NOT used", "[rom-impl-components]")
{
    TEST_ASSERT_FALSE(fn_in_rom(&spi_flash_chip_generic_probe));
}
#endif // CONFIG_SPI_FLASH_ROM_IMPL


#define TEST_MEMORY_LEAK_THRESHOLD (-100)

static size_t before_free_8bit;
static size_t before_free_32bit;

static void check_leak(size_t before_free, size_t after_free, const char *type)
{
    ssize_t delta = after_free - before_free;
    printf("MALLOC_CAP_%s: Before %u bytes free, After %u bytes free (delta %d)\n", type, before_free, after_free, delta);
    TEST_ASSERT_MESSAGE(delta >= TEST_MEMORY_LEAK_THRESHOLD, "memory leak");
}

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    check_leak(before_free_8bit, after_free_8bit, "8BIT");
    check_leak(before_free_32bit, after_free_32bit, "32BIT");
}


void app_main(void)
{
    unity_run_menu();
}
