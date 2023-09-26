/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_efuse_rtc_table.h"
#define esp_efuse_rtc_calib_get_ver()     esp_efuse_rtc_table_read_calib_version()
#elif SOC_ADC_CALIBRATION_V1_SUPPORTED
#include "esp_efuse_rtc_calib.h"
#else
#define esp_efuse_rtc_calib_get_ver()     -1        // Not support calibration
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

    printf("\nADC eFuse Calibration Version %d\n", esp_efuse_rtc_calib_get_ver());
    unity_run_menu();
}
