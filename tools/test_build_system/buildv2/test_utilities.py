# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_idf_messaging_and_compile_options(idf_py: IdfPyFunc) -> None:
    """idf_msg/idf_warn produce output; idf_build_get_compile_options returns flags."""
    logging.info('Testing idf messaging functions and compile options')

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        '# placeholder_after_project_default\n'
        'idf_msg("TEST_MSG_OUTPUT")\n'
        'idf_warn("TEST_WARN_OUTPUT")\n'
        'idf_build_get_compile_options(opts)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/compile_opts.txt" "${opts}")\n',
    )

    result = idf_py('reconfigure')

    combined = (result.stdout or '') + (result.stderr or '')
    assert 'TEST_MSG_OUTPUT' in combined and 'TEST_WARN_OUTPUT' in combined, (
        'Both idf_msg and idf_warn should produce output'
    )

    assert Path('build/compile_opts.txt').exists()
    opts_content = Path('build/compile_opts.txt').read_text()
    assert len(opts_content) > 0, 'compile options should not be empty'


@pytest.mark.usefixtures('test_app_copy')
def test_idf_die_aborts_configuration(idf_py: IdfPyFunc) -> None:
    """idf_die() should abort cmake configuration with a clear message."""
    logging.info('Testing idf_die aborts configuration')

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        '# placeholder_after_project_default\nidf_die("FATAL_TEST_MESSAGE_UNIQUE_STRING")\n',
    )

    with pytest.raises(subprocess.CalledProcessError) as exc_info:
        idf_py('reconfigure')
    err_output = (exc_info.value.stdout or '') + (exc_info.value.stderr or '')
    assert 'FATAL_TEST_MESSAGE_UNIQUE_STRING' in err_output, 'idf_die message should appear in the error output'


@pytest.mark.usefixtures('test_app_copy')
def test_idf_deprecated_produces_warning(idf_py: IdfPyFunc) -> None:
    """idf_deprecated() should emit a deprecation warning."""
    logging.info('Testing idf_deprecated warning output')

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        '# placeholder_after_project_default\n'
        'set(CMAKE_WARN_DEPRECATED ON)\n'
        'idf_deprecated("DEPRECATED_TEST_MSG_UNIQUE")\n',
    )

    result = idf_py('reconfigure')
    combined = (result.stdout or '') + (result.stderr or '')
    assert 'DEPRECATED_TEST_MSG_UNIQUE' in combined, 'idf_deprecated message should appear in the output'
