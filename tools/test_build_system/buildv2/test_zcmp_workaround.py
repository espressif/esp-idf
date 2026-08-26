# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc

_IDF_TARGET = 'esp32c5'

_CMAKEV2_CMAKELISTS = """\
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)

project(test_zcmp_workaround_checking C CXX ASM)

idf_project_default()
"""


@pytest.mark.test_app_copy('components/riscv/test_apps/test_zcmp_workaround_checking')
@pytest.mark.usefixtures('test_app_copy')
def test_zcmp_workaround_checking(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """ZCMP POST_ELF check must fail when mstatus.mie is cleared without mintthresh workaround."""
    logging.info('Building zcmp workaround checking test app (build system v2)')
    (test_app_copy / 'CMakeLists.txt').write_text(_CMAKEV2_CMAKELISTS, encoding='utf-8')

    idf_py('set-target', _IDF_TARGET)
    ret = idf_py('-DTEST_INVALID_WORKAROUND=1', 'build', check=False)
    output = (ret.stdout or '') + (ret.stderr or '')

    assert ret.returncode != 0, 'Build must fail when ZCMP workaround is violated'
    assert 'ZCMP workaround violation' in output, 'Build output must report the ZCMP workaround violation'
