/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Disable flash cache and run user stub function and then enable flash cache again
 *
 * @note You should make sure the passed-in function is in internal RAM.
 *
 * @param post_cache_disable User function to be invoked after cache is disabled.
 * @param user_ctx User context to be passed to user function.
 */
void unity_utils_run_cache_disable_stub(void (*post_cache_disable)(void *), void *user_ctx);

#ifdef __cplusplus
}
#endif
