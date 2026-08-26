/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"

#ifdef CONFIG_EXAMPLE_ENABLE_LOGGING
#include "logging_util.h"
#endif

#ifdef CONFIG_EXAMPLE_ENABLE_MATH
#include "math_util.h"
#endif

void app_main(void)
{
    printf("Conditional Component Example\n");
    printf("==============================\n\n");

    printf("This example demonstrates Build System v2's idf_component_include()\n");
    printf("function for conditional component inclusion.\n\n");

#ifdef CONFIG_EXAMPLE_ENABLE_LOGGING
    printf("Logging component is ENABLED\n");
    log_info("This is an info message from logging_util");
    log_warn("This is a warning message from logging_util");
#else
    printf("Logging component is DISABLED\n");
#endif

    printf("\n");

#ifdef CONFIG_EXAMPLE_ENABLE_MATH
    printf("Math component is ENABLED\n");
    int a = 10, b = 5;
    printf("  add(%d, %d) = %d\n", a, b, math_add(a, b));
    printf("  subtract(%d, %d) = %d\n", a, b, math_subtract(a, b));
    printf("  multiply(%d, %d) = %d\n", a, b, math_multiply(a, b));
#else
    printf("Math component is DISABLED\n");
#endif

    printf("\n");
    printf("You can change the component configuration via menuconfig:\n");
    printf("  idf.py menuconfig -> Conditional Component Example Configuration\n");
}
