# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
import re
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_gdbinit_single_binary(idf_py: IdfPyFunc) -> None:
    """A single-executable build writes gdbinit into build/gdbinit/ referencing the app elf."""
    logging.info('Testing gdbinit: single-binary default output directory and elf reference')

    idf_py('build')

    symbols = Path('build/gdbinit/symbols')
    assert symbols.exists(), 'build/gdbinit/symbols was not generated for a single-binary build'
    content = symbols.read_text()
    assert 'build_test_app.elf' in content, f'gdbinit symbols must reference the application elf, got: {content!r}'


def _create_app2_component(base_dir: Path) -> None:
    """Create a second application component for multi-binary tests."""
    app2_dir = base_dir / 'components' / 'app2'
    app2_dir.mkdir(parents=True, exist_ok=True)
    (app2_dir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "app2.c")\n')
    (app2_dir / 'app2.c').write_text('void app_main(void) {}\n')


def _write_two_executable_cmakelists(app1_extra: str = '', app2_extra: str = '') -> None:
    """Rewrite the app CMakeLists to build app1 and app2 and generate metadata for each.

    app1_extra and app2_extra are appended inside the respective
    idf_build_generate_metadata() calls (e.g. a ``GDBINIT_DIR`` argument).
    """
    _create_app2_component(Path('.'))
    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        'idf_build_executable(app1 COMPONENTS main)\n'
        'idf_build_executable(app2 COMPONENTS app2)\n'
        'add_custom_target(app ALL DEPENDS app1 app2)\n'
        f'idf_build_generate_metadata(EXECUTABLE app1{app1_extra})\n'
        'idf_build_generate_metadata(EXECUTABLE app2\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/project_description_app2.json"'
        f'{app2_extra})\n',
    )


@pytest.mark.usefixtures('test_app_copy')
def test_gdbinit_multi_binary_per_executable(idf_py: IdfPyFunc) -> None:
    """Each executable in a multi-binary build gets its own gdbinit dir referencing its own elf."""
    logging.info('Testing gdbinit: per-executable output directories in a multi-binary build')

    _write_two_executable_cmakelists(
        '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/app1"',
        '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/app2"',
    )

    idf_py('build')

    app1_symbols = Path('build/gdbinit/app1/symbols')
    app2_symbols = Path('build/gdbinit/app2/symbols')

    assert app1_symbols.exists(), 'per-executable gdbinit for app1 was not generated'
    assert app2_symbols.exists(), 'per-executable gdbinit for app2 was not generated'

    app1_content = app1_symbols.read_text()
    app2_content = app2_symbols.read_text()

    # Each executable's gdbinit must reference its own elf, proving the outputs
    # did not overwrite each other in the shared build/gdbinit/ directory.
    assert 'app1.elf' in app1_content, f'app1 gdbinit must reference app1.elf, got: {app1_content!r}'
    assert 'app2.elf' in app2_content, f'app2 gdbinit must reference app2.elf, got: {app2_content!r}'
    assert 'app2.elf' not in app1_content, 'app1 gdbinit was overwritten with app2 symbols'
    assert 'app1.elf' not in app2_content, 'app2 gdbinit was overwritten with app1 symbols'

    # Each executable's project_description.json must point gdbinit_files at that
    # executable's GDBINIT_DIR, which is what idf.py gdb/debug consumes.
    app1_desc = json.loads(Path('build/project_description.json').read_text())
    app2_desc = json.loads(Path('build/project_description_app2.json').read_text())
    app1_symbols_path = app1_desc['gdbinit_files']['01_symbols']
    app2_symbols_path = app2_desc['gdbinit_files']['01_symbols']
    assert app1_symbols_path.endswith('gdbinit/app1/symbols'), app1_symbols_path
    assert app2_symbols_path.endswith('gdbinit/app2/symbols'), app2_symbols_path


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.test_app_copy('examples/build_system/cmakev2/features/idf_as_lib', 'idf_as_lib')
def test_gdbinit_add_executable_elf_target(idf_py: IdfPyFunc) -> None:
    """A plain add_executable(<name>.elf) target references the ELF that exists on disk."""
    logging.info('Testing gdbinit: add_executable(.elf) target ELF resolution (idf_as_lib pattern)')

    idf_py('build')

    symbols = Path('build/gdbinit/symbols')
    assert symbols.exists(), 'build/gdbinit/symbols was not generated'
    match = re.search(r'^file (\S+)$', symbols.read_text(), re.MULTILINE)
    assert match, f'no application "file" line in gdbinit symbols: {symbols.read_text()!r}'
    elf_ref = match.group(1)

    # The executable target is named idf_as_lib.elf (add_executable, no OUTPUT_NAME
    # or SUFFIX): the ELF on disk is idf_as_lib.elf, not idf_as_lib.elf.elf, so the
    # gdbinit must reference a file that actually exists.
    assert Path(elf_ref).name == 'idf_as_lib.elf', f'gdbinit must reference the real ELF, got: {elf_ref!r}'
    assert Path(elf_ref).exists(), f'gdbinit references a non-existent ELF: {elf_ref!r}'


@pytest.mark.usefixtures('test_app_copy')
@pytest.mark.parametrize(
    'app1_extra, app2_extra',
    [
        pytest.param(
            '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/shared"',
            '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/shared"',
            id='explicit-shared-dir',
        ),
        pytest.param('', '', id='both-default-dir'),
    ],
)
def test_gdbinit_shared_dir_rejected(idf_py: IdfPyFunc, app1_extra: str, app2_extra: str) -> None:
    """Two different executables generating gdbinit into the same directory is rejected at configure time."""
    logging.info('Testing gdbinit: two executables sharing a GDBINIT_DIR is rejected')

    _write_two_executable_cmakelists(app1_extra, app2_extra)

    with pytest.raises(subprocess.CalledProcessError) as exc_info:
        idf_py('reconfigure')
    err_output = (exc_info.value.stdout or '') + (exc_info.value.stderr or '')
    assert 'both generate gdbinit' in err_output, f'expected a GDBINIT_DIR collision error, got: {err_output!r}'


@pytest.mark.usefixtures('test_app_copy')
def test_gdbinit_similar_dir_names_not_conflated(idf_py: IdfPyFunc) -> None:
    """GDBINIT_DIRs that differ only by punctuation are distinct directories, not a false collision."""
    logging.info('Testing gdbinit: dirs differing only by punctuation are not conflated')

    _write_two_executable_cmakelists(
        '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/app-x"',
        '\n    GDBINIT_DIR "${CMAKE_BINARY_DIR}/gdbinit/app_x"',
    )

    idf_py('reconfigure')

    assert Path('build/gdbinit/app-x/symbols').exists(), 'gdbinit for app-x was not generated'
    assert Path('build/gdbinit/app_x/symbols').exists(), 'gdbinit for app_x was not generated'
