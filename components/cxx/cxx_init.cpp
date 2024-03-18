/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_private/startup_internal.h"

namespace {
const char *TAG = "C++ init";
}

/**
 * This function overwrites the same function of libsupc++ (part of libstdc++).
 * Consequently, libsupc++ will then follow our configured exception emergency pool size.
 *
 * It will be called even with -fno-exception for user code since the stdlib still uses exceptions.
 */
extern "C" size_t __cxx_eh_arena_size_get(void)
{
#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    ESP_EARLY_LOGD(TAG, "Setting C++ exception emergency pool to %u.", CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE);
    return CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE;
#else
    ESP_EARLY_LOGD(TAG, "Setting C++ exception emergency pool to 0.");
    return 0;
#endif
}

/**
 * Dummy function used to force linking this file.
 * This works via -u __cxx_init_dummy flag in CMakeLists.txt
 */
extern "C" void __cxx_init_dummy(void)
{
}
