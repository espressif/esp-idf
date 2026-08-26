/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <memory>

// Compiled with the project-wide atomic lock policy (CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY).
// std::make_shared instantiates the __gnu_cxx::_Lock_policy-parameterized
// reference counting templates.
int cxx_atomic_use_a(void)
{
    auto p = std::make_shared<int>(1);
    return *p;
}
