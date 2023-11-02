/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdlib>
#include "esp_heap_caps.h"

#pragma once

/**
 * @brief Type that is only usable with new (std::nothrow) to avoid exceptions.
 *
 * This struct shall be inherited by all types in NVS that may be allocated dynamically (with new).
 *
 * NVS allocates memory at runtime. Because we use C++, we need to avoid the global ``operator new`` from libstdc++,
 * since it throws exceptions and we compile NVS with ``-fno-exceptions``. We also need to avoid the global
 * non-throwing version of that operator from libstdc++, since it is merely a wrapper around the original operator
 * catching all exceptions.
 *
 * This struct removes the normal operator new from this and all types inheriting from it. It furthermore provides
 * a custom version of operator new (..., const std::nothrow_t&) noexcept that will not use C++ exceptions.
 *
 * E.g., if you have a type MyType inheriting from ExceptionlessAllocatable, you want to use it as follows:
 * @code{c++}
 * MyType : public ExceptionlessAllocatable {
 *     ExceptionlessAllocatable();
 *     ExceptionlessAllocatable(int param);
 * };
 * // ...
 * MyType *m0 = new (std::nothrow) MyType;
 * MyType *m1 = new (std::nothrow) MyType(47);
 * // ...
 * delete m1;
 * delete m0;
 * @endcode
 */
struct ExceptionlessAllocatable {
    /**
     * Disallow use of the default new operator, all of NVS is currently tailored to not throw exceptions
     */
    static void *operator new( std::size_t ) = delete;

    static void *operator new[]( std::size_t ) = delete;

    /**
     * Simple implementation with malloc(). No exceptions are thrown if the allocation fails.
     * To use this operator, your type must inherit from this class and then allocate with:
     * @code{c}
     * new (std::nothrow) <YourType>;                           // default constructor
     * new (std::nothrow) <YourType>(<constructor parameters>); // non-default constructor
     * @endcode
     */
    void *operator new (size_t size, const std::nothrow_t&) noexcept {
#ifdef CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM
        return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM,
                                                MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
#else
        return std::malloc(size);
#endif
    }

    void *operator new [](size_t size, const std::nothrow_t&) noexcept {
#ifdef CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM
        return heap_caps_malloc_prefer(size, 2, MALLOC_CAP_DEFAULT | MALLOC_CAP_SPIRAM,
                                                MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
#else
        return std::malloc(size);
#endif
    }

    /**
     * Use \c delete as normal. This operator will be called automatically instead of the global one from libstdc++.
     */
    void operator delete (void *obj) noexcept {
#ifdef CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM
        return heap_caps_free(obj);
#else
        return std::free(obj);
#endif
    }

    void operator delete [](void *obj) noexcept {
#ifdef CONFIG_NVS_ALLOCATE_CACHE_IN_SPIRAM
        return heap_caps_free(obj);
#else
        return std::free(obj);
#endif
    }
};
