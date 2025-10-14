/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "unity.h"
#include "unity_test_utils_cache.h"
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "esp_private/spi_flash_os.h"

IRAM_ATTR void unity_utils_run_cache_disable_stub(void (*post_cache_disable)(void *), void *user_ctx)
{
#if !CONFIG_SPI_FLASH_AUTO_SUSPEND
    // callback function must reside in IRAM
    TEST_ASSERT_TRUE(esp_ptr_in_iram(post_cache_disable));
    // disable flash cache
    spi_flash_guard_get()->start();
#endif
    post_cache_disable(user_ctx);
    // enable flash cache
#if !CONFIG_SPI_FLASH_AUTO_SUSPEND
    spi_flash_guard_get()->end();
#endif
}
