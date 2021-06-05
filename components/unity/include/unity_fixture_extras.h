// Copyright 2016-2021 Espressif Systems (Shanghai) Co. Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* IDF-specific additions to "Unity Fixture".
 * This file doesn't need to be included directly, it gets included into unity.h
 * through unity_config.h.
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(CONFIG_IDF_TARGET) || defined(CONFIG_IDF_TARGET_LINUX)
#define UNITY_MAYBE_EXIT(rc)  do { exit(rc); } while(0)
#else
#define UNITY_MAYBE_EXIT(rc)  do { (void) rc; } while(0)
#endif

/* A shorthand for running all tests called from one function "func_", from the app_main function.
 * Use this when there is more than one test group.
 *
 * Example:
 *
 *    #include "unity.h"
 *    #include "unity_fixture.h"
 *
 *    static_void run_all_tests(void) {
 *        RUN_TEST_GROUP(group1);   // test group defined in another file, e.g. test_group1.c
 *        RUN_TEST_GROUP(group2);   // test group defined in another file, e.g. test_group2.c
 *    }
 *
 *    void app_main(void) {
 *        UNITY_MAIN_FUNC(run_all_tests);
 *    }
 */
#define UNITY_MAIN_FUNC(func_) do { \
    const char* argv[] = { "test", "-v" }; \
    const int argc = sizeof(argv)/sizeof(argv[0]); \
    int rc = UnityMain(argc, argv, func_); \
    printf("\nTests finished, rc=%d\n", rc); \
    UNITY_MAYBE_EXIT(rc); \
} while(0)

/* A shorthand for running one test group from the app_main function, when there is only
 * one test group and it is defined in the same file.
 *
 * Example:
 *
 *    #include "unity.h"
 *    #include "unity_fixture.h"
 *
 *    TEST_GROUP(my_feature);
 *    // also define TEST_SETUP, TEST_TEARDOWN, TESTs, TEST_GROUP_RUNNER
 *
 *    void app_main(void) {
 *        UNITY_MAIN(my_feature);
 *    }
 */
#define UNITY_MAIN(group_) UNITY_MAIN_FUNC(TEST_ ## group_ ## _GROUP_RUNNER)

#ifdef __cplusplus
}
#endif
