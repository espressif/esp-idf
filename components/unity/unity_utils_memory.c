/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_utils.h"

static size_t s_allowed_leak_level;

void unity_utils_set_leak_level(size_t leak_level)
{
    s_allowed_leak_level = leak_level;
}

void unity_utils_check_leak(unsigned int before_free,
        unsigned int after_free,
        const char *type,
        unsigned int threshold)
{
    int free_delta = (int)after_free - (int)before_free;
    printf("MALLOC_CAP_%s usage: Free memory delta: %d Leak threshold: -%u \n",
           type,
           free_delta,
           threshold);

    if (free_delta > 0) {
        return; // free memory went up somehow
    }

    unsigned int leaked = (size_t)(free_delta * -1);

    if (leaked > 0) {
        printf("MALLOC_CAP_%s %s leak: Before %u bytes free, After %u bytes free (delta %u)\n",
               type,
               leaked <= threshold ? "potential" : "critical",
               before_free, after_free, leaked);
        fflush(stdout);
    }
    TEST_ASSERT_MESSAGE(leaked <= threshold, "The test leaked too much memory");
}

void unity_utils_evaluate_leaks(void)
{
    unity_utils_evaluate_leaks_direct(s_allowed_leak_level);
}
