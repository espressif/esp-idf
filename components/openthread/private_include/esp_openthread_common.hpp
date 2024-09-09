/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_heap_caps.h"
#include <utility>
#include "common/new.hpp"

template <typename T, typename... Args>
inline T *New(uint32_t alloc_caps, Args &&...args)
{
    void *p = heap_caps_calloc(1, sizeof(T), alloc_caps);
    if (p != nullptr) {
        return new (p) T(std::forward<Args>(args)...);
    }
    return nullptr;
}
