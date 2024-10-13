/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include "unity_test_utils.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"

#define TEST_MEMORY_LEAK_THRESHOLD (300)

void setUp(void)
{
    printf("%s", ""); /* sneakily lazy-allocate the reent structure for this test task */
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    esp_reent_cleanup();
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    /*
     _____         _     ______________  ______  ________
    |_   _|       | |   /  ___|  _  \  \/  ||  \/  /  __ \
      | | ___  ___| |_  \ `--.| | | | .  . || .  . | /  \/
      | |/ _ \/ __| __|  `--. \ | | | |\/| || |\/| | |
      | |  __/\__ \ |_  /\__/ / |/ /| |  | || |  | | \__/\
      \_/\___||___/\__| \____/|___/ \_|  |_/\_|  |_/\____/
    */

    printf(" _____         _     ______________  ______  ________\n");
    printf("|_   _|       | |   /  ___|  _  \\  \\/  ||  \\/  /  __ \\ \n");
    printf("  | | ___  ___| |_  \\ `--.| | | | .  . || .  . | /  \\/\n");
    printf("  | |/ _ \\/ __| __|  `--. \\ | | | |\\/| || |\\/| | |\n");
    printf("  | |  __/\\__ \\ |_  /\\__/ / |/ /| |  | || |  | | \\__/\\\n");
    printf("  \\_/\\___||___/\\__| \\____/|___/ \\_|  |_/\\_|  |_/\\____/\n");

    unity_run_menu();
}
