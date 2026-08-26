/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <memory>

// By default compiled with the same project-wide atomic lock policy as
// cxx_atomic_a.cpp. When TEST_INVALID_LOCK_POLICIES is set, main/CMakeLists.txt
// adds -U_GLIBCXX_HAVE_ATOMIC_LOCK_POLICY so this TU instantiates a different
// __gnu_cxx::_Lock_policy.
int cxx_atomic_use_b(void)
{
    auto p = std::make_shared<int>(2);
    return *p;
}
