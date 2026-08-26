| Supported Targets | ESP32-C5 |
| ----------------- | -------- |

# C++ atomic lock policy test

Build-only app. Validates ``CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY``: both
translation units use ``std::shared_ptr`` under the forced lock-free policy, and
the cxx post-build hook accepts the linked executable.

For the negative build-system check (mixed ``__gnu_cxx::_Lock_policy``), see
``tools/test_build_system/test_cxx_atomic_policy.py``, which builds this app with
``-DTEST_INVALID_LOCK_POLICIES=1``.
