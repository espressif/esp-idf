/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define TEST_MEMORY_LEAK_THRESHOLD (400)

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
    /*
     _____         _     ___________  ___________ _____
    |_   _|       | |   /  ___|  _  \/  ___| ___ \_   _|
      | | ___  ___| |_  \ `--.| | | |\ `--.| |_/ / | |
      | |/ _ \/ __| __|  `--. \ | | | `--. \  __/  | |
      | |  __/\__ \ |_  /\__/ / |/ / /\__/ / |    _| |_
      \_/\___||___/\__| \____/|___/  \____/\_|    \___/
    */

    printf(" _____         _     ___________  ___________ _____\n");
    printf("|_   _|       | |   /  ___|  _  \\/  ___| ___ \\_   _|\n");
    printf("  | | ___  ___| |_  \\ `--.| | | |\\ `--.| |_/ / | |\n");
    printf("  | |/ _ \\/ __| __|  `--. \\ | | | `--. \\  __/  | |\n");
    printf("  | |  __/\\__ \\ |_  /\\__/ / |/ / /\\__/ / |    _| |_\n");
    printf("  \\_/\\___||___/\\__| \\____/|___/  \\____/\\_|    \\___/\n");

    unity_run_menu();
}
