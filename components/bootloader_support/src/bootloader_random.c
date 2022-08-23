/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "esp_cpu.h"
#include "soc/wdev_reg.h"

#ifndef BOOTLOADER_BUILD
#include "esp_random.h"
#include "esp_private/periph_ctrl.h"

 __attribute__((weak)) void bootloader_fill_random(void *buffer, size_t length)
{
    return esp_fill_random(buffer, length);
}

#else

#if !defined CONFIG_IDF_TARGET_ESP32S3
#define RNG_CPU_WAIT_CYCLE_NUM (80 * 32 * 2) /* extra factor of 2 is precautionary */
#else
#define RNG_CPU_WAIT_CYCLE_NUM (80 * 23) /* 45 KHz reading frequency is the maximum we have tested so far on S3 */
#endif

 __attribute__((weak)) void bootloader_fill_random(void *buffer, size_t length)
{
    uint8_t *buffer_bytes = (uint8_t *)buffer;
    uint32_t random;
    uint32_t start, now;

    assert(buffer != NULL);

    for (size_t i = 0; i < length; i++) {
        if (i == 0 || i % 4 == 0) { /* redundant check is for a compiler warning */
            /* in bootloader with ADC feeding HWRNG, we accumulate 1
               bit of entropy per 40 APB cycles (==80 CPU cycles.)

               To avoid reading the entire RNG hardware state out
               as-is, we repeatedly read the RNG register and XOR all
               values.
            */
            random = REG_READ(WDEV_RND_REG);
            start = esp_cpu_get_cycle_count();
            do {
                random ^= REG_READ(WDEV_RND_REG);
                now = esp_cpu_get_cycle_count();
            } while (now - start < RNG_CPU_WAIT_CYCLE_NUM);
        }
        buffer_bytes[i] = random >> ((i % 4) * 8);
    }
}

#ifndef CONFIG_IDF_ENV_FPGA

#else // CONFIG_IDF_ENV_FPGA
#include "esp_log.h"

static void s_non_functional(const char *func)
{
    ESP_EARLY_LOGW("rand", "%s non-functional for FPGA builds", func);
}

void bootloader_random_enable()
{
    s_non_functional(__func__);
}

void bootloader_random_disable()
{
    s_non_functional(__func__);
}

#endif // CONFIG_IDF_ENV_FPGA

#endif // BOOTLOADER_BUILD
