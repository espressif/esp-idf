/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 Tests for the BT common things implementation
*/

#include <stdbool.h>

#include "unity.h"

extern bool btdm_controller_compile_version_check(void);

TEST_CASE("bt_controller_git_commit_check", "[bt_common]")
{
    TEST_ASSERT(btdm_controller_compile_version_check() == true);
}
