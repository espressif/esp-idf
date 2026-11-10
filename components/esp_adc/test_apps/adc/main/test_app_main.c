/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "hal/adc_ll.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if SOC_ADC_CALIBRATION_V1_SUPPORTED || ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
#include "hal/adc_efuse_hal.h"
#define adc_test_efuse_get_version()     adc_hal_efuse_get_version()
#else
#define adc_test_efuse_get_version()     -1        // Calibration not supported
#endif

#define TEST_MEMORY_LEAK_THRESHOLD (600)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    //  _____         _        _    ____   ____
    // |_   _|__  ___| |_     / \  |  _ \ / ___|
    //   | |/ _ \/ __| __|   / _ \ | | | | |
    //   | |  __/\__ \ |_   / ___ \| |_| | |___
    //   |_|\___||___/\__| /_/   \_\____/ \____|

    printf(" _____         _        _    ____   ____ \n");
    printf("|_   _|__  ___| |_     / \\  |  _ \\ / ___|\n");
    printf("  | |/ _ \\/ __| __|   / _ \\ | | | | |    \n");
    printf("  | |  __/\\__ \\ |_   / ___ \\| |_| | |___ \n");
    printf("  |_|\\___||___/\\__| /_/   \\_\\____/ \\____|\n");

    printf("\nADC eFuse Calibration Version %d\n", adc_test_efuse_get_version());
    unity_run_menu();
}
