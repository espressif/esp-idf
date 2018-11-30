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
