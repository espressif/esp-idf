# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging

import pytest
from test_build_system_helpers import IdfPyFunc

_IDF_TARGET = 'esp32c5'


@pytest.mark.test_app_copy('components/riscv/test_apps/test_zcmp_workaround_checking')
@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.buildv2_skip('Uses build system v1 project.cmake test app')
def test_zcmp_workaround_checking(idf_py: IdfPyFunc) -> None:
    """ZCMP post-ELF check must fail when mstatus.mie is cleared without mintthresh workaround."""
    logging.info('Building zcmp workaround checking test app')

    idf_py('set-target', _IDF_TARGET)
    ret = idf_py('-DTEST_INVALID_WORKAROUND=1', 'build', check=False)
    output = (ret.stdout or '') + (ret.stderr or '')

    assert ret.returncode != 0, 'Build must fail when ZCMP workaround is violated'
    assert 'ZCMP workaround violation' in output, 'Build output must report the ZCMP workaround violation'
