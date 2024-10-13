/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"

namespace nvs
{
    class Lock
    {
    public:
        Lock();
        ~Lock();
        static esp_err_t init();
        static void uninit();
#ifndef LINUX_TARGET
    private:
        static _lock_t mSemaphore;
#endif
    };
} // namespace nvs
