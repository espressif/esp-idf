# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pathlib import Path

from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import get_snapshot


def _write_project_include(test_app_copy: Path, content: str) -> None:
    """Write content to the main component's project_include.cmake file."""
    project_include = test_app_copy / 'main' / 'project_include.cmake'
    project_include.write_text(content, encoding='utf-8')


def test_post_elf_callback_fires_before_binary(test_app_copy: Path, idf_py: IdfPyFunc) -> None:
    """
    Verify a POST_ELF callback fires after the executable target exists but before
    the binary (.bin) is generated. The callback attaches a POST_BUILD step to the
    executable via add_custom_command(TARGET ... POST_BUILD).
    """
    _write_project_include(
        test_app_copy,
        '\n'.join(
            [
                'function(__test_post_elf_cb target)',
                '    add_custom_command(TARGET ${target} POST_BUILD',
                '        COMMAND ${CMAKE_COMMAND} -E sleep 1',
                '        COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/postelf_file")',
                'endfunction()',
                'idf_component_register_build_event_callback(EVENT POST_ELF CALLBACK __test_post_elf_cb)',
            ]
        ),
    )

    idf_py('build')

    elf_file = test_app_copy / 'build' / 'build_test_app.elf'
    postelf_file = test_app_copy / 'build' / 'postelf_file'
    bin_timestamp = test_app_copy / 'build' / 'build_test_app.bin'

    assert elf_file.exists(), 'ELF file must exist'
    assert postelf_file.exists(), 'post-elf file must be created'
    assert bin_timestamp.exists(), 'bin timestamp must exist'

    snap = get_snapshot([str(elf_file), str(postelf_file), str(bin_timestamp)])
    mtimes = dict(snap.info)
    assert mtimes[str(postelf_file)] > mtimes[str(elf_file)], 'post-ELF file must be created after ELF file'
    assert mtimes[str(bin_timestamp)] > mtimes[str(postelf_file)], (
        'Binary generation must occur after post-ELF dependency'
    )
