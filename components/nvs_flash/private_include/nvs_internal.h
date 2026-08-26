/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#if CONFIG_NVS_ASSERT_ERROR_CHECK
#define NVS_ASSERT_OR_RETURN(condition, retcode) assert(condition);
#else
#define NVS_ASSERT_OR_RETURN(condition, retcode) \
    if (!(condition)) {                          \
        return retcode;                          \
    }
#endif // CONFIG_NVS_ASSERT_ERROR_CHECK
