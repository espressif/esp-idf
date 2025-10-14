# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pathlib import Path

from test_build_system_helpers import append_to_file
from test_build_system_helpers import get_snapshot
from test_build_system_helpers import run_cmake_and_build


def test_post_elf_dependency_runs_before_bin(test_app_copy: Path) -> None:
    """
    Verify idf_build_add_post_elf_dependency(<elf_filename> <dep_target>) adds a post-ELF step
    prior to BIN generation.
    """
    cmake_snippet = '\n'.join(
        [
            'idf_build_get_property(elf_target EXECUTABLE GENERATOR_EXPRESSION)',
            'add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/postelf_file"',
            '    COMMAND ${CMAKE_COMMAND} -E sleep 1',
            '    COMMAND ${CMAKE_COMMAND} -E touch "${CMAKE_BINARY_DIR}/postelf_file"',
            '    DEPENDS "$<TARGET_FILE:$<GENEX_EVAL:${elf_target}>>")',
            'add_custom_target(my_postelf DEPENDS "${CMAKE_BINARY_DIR}/postelf_file")',
            'idf_build_add_post_elf_dependency("${CMAKE_PROJECT_NAME}.elf" my_postelf)',
        ]
    )

    append_to_file('main/CMakeLists.txt', f'\n# post-elf test injection\n{cmake_snippet}\n')

    run_cmake_and_build('-G', 'Ninja', str(test_app_copy))

    elf_file = test_app_copy / 'build' / 'build_test_app.elf'
    postelf_file = test_app_copy / 'build' / 'postelf_file'
    bin_timestamp = test_app_copy / 'build' / '.bin_timestamp'

    assert elf_file.exists(), 'ELF file must exist'
    assert postelf_file.exists(), 'post-elf file must be created'
    assert bin_timestamp.exists(), 'bin timestamp must exist'

    snap = get_snapshot([str(elf_file), str(postelf_file), str(bin_timestamp)])
    mtimes = dict(snap.info)
    assert mtimes[str(postelf_file)] > mtimes[str(elf_file)], 'post-ELF file must be created after ELF file'
    assert mtimes[str(bin_timestamp)] > mtimes[str(postelf_file)], (
        'Binary generation must occur after post-ELF dependency'
    )
