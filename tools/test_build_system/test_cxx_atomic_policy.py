# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from test_build_system_helpers import IdfPyFunc

CXX_ATOMIC_POLICY_TEST_APP = 'components/cxx/test_apps/atomic_lock_policy'


@pytest.mark.test_app_copy(CXX_ATOMIC_POLICY_TEST_APP)
@pytest.mark.usefixtures('test_app_copy')
def test_cxx_atomic_policy_checking(idf_py: IdfPyFunc) -> None:
    """
    Build the atomic_lock_policy test app with -DTEST_INVALID_LOCK_POLICIES=1.

    CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY adds -D_GLIBCXX_HAVE_ATOMIC_LOCK_POLICY=1
    to all C++ files, but TEST_INVALID_LOCK_POLICIES undefines it for one
    translation unit. The linked executable therefore contains mixed
    __gnu_cxx::_Lock_policy instantiations, and the cxx post-build hook
    (check_cxx_atomic_policy.cmake) must fail the build.
    """
    ret = idf_py('-DIDF_TARGET=esp32c5', '-DTEST_INVALID_LOCK_POLICIES=1', 'build', check=False)
    assert ret.returncode != 0, 'Build must fail when atomic lock policies are mixed'
    assert 'Mixed libstdc++ atomic lock policies detected' in (ret.stdout + ret.stderr), (
        'Expected the cxx post-build hook to report mixed atomic lock policies'
    )
