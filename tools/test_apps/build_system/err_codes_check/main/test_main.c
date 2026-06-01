/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_err.h"

/* Defined in the generated err_codes_test_cases.c */
extern int err_codes_check_all(void);

void app_main(void)
{
    printf("Starting error codes registration check...\n");

    int failures = err_codes_check_all();

    if (failures == 0) {
        printf("SUCCESS: All error codes are correctly registered.\n");
    } else {
        printf("FAILURE: %d error codes are not correctly registered.\n", failures);
    }
}
