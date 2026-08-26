# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import hashlib
import os
import subprocess
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file

FULL_PROJECT_APIS = (
    'ulp_project_init',
    'ulp_project_default',
    'ulp_build_executable',
)

LEGACY_PROJECT_APIS = (
    'ulp_apply_default_options',
    'ulp_apply_default_sources',
)

ALL_APIS = FULL_PROJECT_APIS + LEGACY_PROJECT_APIS


def _cmake_path(path: Path) -> str:
    return path.resolve().as_posix()


def _write_api_probe_project(project_dir: Path, entry_point: str, add_native_executable: bool) -> None:
    native_executable = 'add_executable(${ULP_APP_NAME} main.c)' if add_native_executable else ''
    if entry_point.endswith('ulp_project.cmake'):
        pre_project_include = f'include({entry_point})'
        post_project_include = ''
    else:
        pre_project_include = ''
        post_project_include = f'include({entry_point})'

    (project_dir / 'sdkconfig.h').write_text('', encoding='utf-8')
    (project_dir / 'main.c').write_text('int main(void) { return 0; }\n', encoding='utf-8')
    (project_dir / 'sdkconfig.cmake').write_text(
        '\n'.join(
            (
                'set(CONFIG_ULP_COPROC_ENABLED y)',
                'set(CONFIG_ULP_COPROC_TYPE_LP_CORE y)',
                '',
            )
        ),
        encoding='utf-8',
    )
    (project_dir / 'CMakeLists.txt').write_text(
        f"""
cmake_minimum_required(VERSION 3.22)

{pre_project_include}
project(ulp_api_probe C CXX ASM)
{native_executable}

{post_project_include}

set(api_commands_file "${{CMAKE_BINARY_DIR}}/api_commands.txt")
file(WRITE "${{api_commands_file}}" "")
foreach(name IN ITEMS {' '.join(ALL_APIS)})
    if(COMMAND ${{name}})
        file(APPEND "${{api_commands_file}}" "${{name}}=1\\n")
    else()
        file(APPEND "${{api_commands_file}}" "${{name}}=0\\n")
    endif()
endforeach()
""",
        encoding='utf-8',
    )


def _read_api_commands(commands_file: Path) -> dict[str, bool]:
    return {
        name: value == '1'
        for name, value in (line.split('=', 1) for line in commands_file.read_text(encoding='utf-8').splitlines())
    }


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


@pytest.mark.parametrize(
    'entry_point,add_native_executable,expected_available,expected_unavailable',
    (
        (
            '${IDF_PATH}/components/ulp/cmake/ulp_project.cmake',
            False,
            FULL_PROJECT_APIS,
            LEGACY_PROJECT_APIS,
        ),
        (
            'IDFULPProject',
            True,
            LEGACY_PROJECT_APIS,
            FULL_PROJECT_APIS,
        ),
    ),
    ids=('full_project_entry_point', 'legacy_project_entry_point'),
)
def test_ulp_cmake_api_availability(
    tmp_path: Path,
    entry_point: str,
    add_native_executable: bool,
    expected_available: tuple[str, ...],
    expected_unavailable: tuple[str, ...],
) -> None:
    # Full ULP subprojects and legacy ULP child projects intentionally expose
    # different CMake API surfaces. Configure a minimal child project through
    # each entry point and verify that only the expected commands exist.
    idf_path = Path(os.environ['IDF_PATH'])
    project_dir = tmp_path / 'project'
    build_dir = tmp_path / 'build'
    project_dir.mkdir()
    _write_api_probe_project(project_dir, entry_point, add_native_executable)
    ulp_cmake_dir = idf_path / 'components' / 'ulp' / 'cmake'

    cmake_args = (
        'cmake',
        '-G',
        'Ninja',
        '-S',
        _cmake_path(project_dir),
        '-B',
        _cmake_path(build_dir),
        f'-DCMAKE_MODULE_PATH={_cmake_path(ulp_cmake_dir)}',
        f'-DCMAKE_TOOLCHAIN_FILE={_cmake_path(ulp_cmake_dir / "toolchain-lp-core-riscv.cmake")}',
        f'-DIDF_PATH={_cmake_path(idf_path)}',
        '-DIDF_TARGET=esp32c6',
        f'-DSDKCONFIG_CMAKE={_cmake_path(project_dir / "sdkconfig.cmake")}',
        f'-DSDKCONFIG_HEADER={_cmake_path(project_dir / "sdkconfig.h")}',
        '-D__ULP_BUILDV2=1',
        '-DULP_APP_NAME=ulp_api_probe',
        '-DULP_TYPE=lp_core',
        '-DIDF_BUILD_V2=y',
        '-DIDF_CUSTOM_TOOLCHAIN=1',
    )
    result = subprocess.run(
        cmake_args,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        check=False,
    )

    assert result.returncode == 0, result.stdout

    command_availability = _read_api_commands(build_dir / 'api_commands.txt')
    for name in expected_available:
        assert command_availability[name], f'{name} should be available'
    for name in expected_unavailable:
        assert not command_availability[name], f'{name} should not be available'


@pytest.mark.test_app_copy('components/ulp/test_apps/ulp_fsm')
def test_ulp_embed_binary_builds_v2_child_project(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # Legacy ulp_embed_binary() callers are built through the builtin ULP child
    # project as a CMake v2 subproject. The child references parent-owned ULP
    # sources by path, so editing those sources must still rebuild the child
    # artifact on the next parent build.
    assert test_app_copy.exists()

    idf_py('-DIDF_TARGET=esp32', '-DSDKCONFIG=build/sdkconfig', 'build')

    ulp_binary_dirs = sorted(p for p in Path('build/subprojects').glob('*') if p.is_dir())
    assert len(ulp_binary_dirs) == 1

    ulp_binary_dir = ulp_binary_dirs[0]
    ulp_binary = ulp_binary_dir / f'{ulp_binary_dir.name}.bin'
    assert ulp_binary.exists()
    original_ulp_binary_hash = _sha256(ulp_binary)

    ulp_source = test_app_copy / 'main/ulp/test_jumps.S'
    ulp_source.write_text(
        ulp_source.read_text(encoding='utf-8')
        + '\n\t.global rebuild_dependency_probe\n'
        + 'rebuild_dependency_probe:\n'
        + '\t.long 0x1234\n',
        encoding='utf-8',
    )

    idf_py('build')
    assert _sha256(ulp_binary) != original_ulp_binary_hash


@pytest.mark.test_app_copy('examples/system/ulp/ulp_riscv/interrupts')
def test_ulp_embed_binary_uses_parent_project_config(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # Legacy ulp_embed_binary() sources are compiled as part of the builtin ULP
    # child project, but still rely on project-local CONFIG_* symbols from the
    # parent app's Kconfig.projbuild.
    assert test_app_copy.exists()

    idf_py('-DIDF_TARGET=esp32s2', '-DSDKCONFIG=build/sdkconfig', 'build')


@pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/lp_core')
def test_ulp_full_project_regenerates_config_from_parent_sdkconfig(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    # Full ULP subprojects use the parent sdkconfig as their source of truth.
    # Changing the parent config should regenerate the child's sdkconfig.cmake.
    assert test_app_copy.exists()

    sdkconfig = Path('build/sdkconfig')

    idf_py('-DIDF_TARGET=esp32c6', '-DSDKCONFIG=build/sdkconfig', 'build')
    ulp_sdkconfig_cmake_files = sorted(Path('build/subprojects').glob('*/config/sdkconfig.cmake'))
    assert len(ulp_sdkconfig_cmake_files) == 1
    ulp_sdkconfig_cmake = ulp_sdkconfig_cmake_files[0]
    assert 'set(CONFIG_ULP_COPROC_RESERVE_MEM "4096")' in ulp_sdkconfig_cmake.read_text()

    replace_in_file(sdkconfig, 'CONFIG_ULP_COPROC_RESERVE_MEM=4096', 'CONFIG_ULP_COPROC_RESERVE_MEM=8192')

    idf_py('build')
    assert 'set(CONFIG_ULP_COPROC_RESERVE_MEM "8192")' in ulp_sdkconfig_cmake.read_text()


@pytest.mark.parametrize(
    'target,subproject_dir,linked_artifact,custom_section,mapped_entry,symbol',
    (
        pytest.param(
            'esp32',
            'ulp_main',
            'ulp_main.sym',
            '.ulp_fsm_extra_data',
            '*libmain.a:counter.*(.ulp_fsm_extra_data .ulp_fsm_extra_data.*)',
            'ldgen_section_marker',
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/fsm')],
            id='fsm',
        ),
        pytest.param(
            'esp32s2',
            'ulp_main',
            'ulp_main.map',
            '.ulp_riscv_extra_data',
            '*libmain.a:main.*(.ulp_riscv_extra_data .ulp_riscv_extra_data.*)',
            'ldgen_section_marker',
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/riscv')],
            id='riscv',
        ),
        pytest.param(
            'esp32c6',
            'ulp_build_system_example',
            'ulp_build_system_example.map',
            '.ulp_math_fast',
            '*libulp_math.a:ulp_math.*(.ulp_math_fast .ulp_math_fast.*)',
            'ulp_math_multiply',
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/lp_core')],
            id='lp_core',
        ),
    ),
)
def test_ulp_full_project_applies_component_ldgen_mapping(
    idf_py: IdfPyFunc,
    test_app_copy: Path,
    target: str,
    subproject_dir: str,
    linked_artifact: str,
    custom_section: str,
    mapped_entry: str,
    symbol: str,
) -> None:
    # These examples declare component LDFRAGMENTS and target_linker_script()
    # templates. Build the ULP child project, then check that ldgen expanded the
    # template mapping and the custom section/symbol survived the ULP link.
    assert test_app_copy.exists()
    idf_py(f'-DIDF_TARGET={target}', '-DSDKCONFIG=build/sdkconfig', 'build')

    ulp_project_dir = Path('build/subprojects') / subproject_dir
    processed_scripts = sorted(ulp_project_dir.glob('**/ulp_sections.ld_library_*'))
    assert processed_scripts
    assert any(mapped_entry in script.read_text(encoding='utf-8') for script in processed_scripts)

    linked_output = (ulp_project_dir / linked_artifact).read_text(encoding='utf-8')
    assert custom_section in linked_output
    assert symbol in linked_output


@pytest.mark.parametrize(
    'target,ulp_project_cmake,subproject_dir,project_replacement,enabled_languages,disabled_languages',
    (
        pytest.param(
            'esp32',
            'main/ulp/CMakeLists.txt',
            'ulp_main',
            None,
            ('ASM',),
            ('C', 'CXX'),
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/fsm')],
            id='fsm_asm_only',
        ),
        pytest.param(
            'esp32s2',
            'main/ulp/CMakeLists.txt',
            'ulp_main',
            ('project(${IDF_DEFAULT_PROJECT_NAME} C CXX ASM)', 'project(${IDF_DEFAULT_PROJECT_NAME} C ASM)'),
            ('C', 'ASM'),
            ('CXX',),
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/riscv')],
            id='riscv_c_asm',
        ),
        pytest.param(
            'esp32c6',
            'main/ulp/CMakeLists.txt',
            'ulp_build_system_example',
            ('project(${IDF_DEFAULT_PROJECT_NAME} C CXX ASM)', 'project(${IDF_DEFAULT_PROJECT_NAME} C ASM)'),
            ('C', 'ASM'),
            ('CXX',),
            marks=[pytest.mark.test_app_copy('tools/test_apps/system/ulp/full_subproject/lp_core')],
            id='lp_core_c_asm',
        ),
    ),
)
def test_ulp_full_project_respects_child_project_enabled_languages(
    idf_py: IdfPyFunc,
    test_app_copy: Path,
    target: str,
    ulp_project_cmake: str,
    subproject_dir: str,
    project_replacement: tuple[str, str] | None,
    enabled_languages: tuple[str, ...],
    disabled_languages: tuple[str, ...],
) -> None:
    # The ULP child project should use the languages declared by its own
    # project() call. FSM can build as an ASM-only project, while RISC-V and
    # LP-core ULP projects do not need CXX when their child project only enables
    # C and ASM.
    if project_replacement is not None:
        replace_in_file(test_app_copy / ulp_project_cmake, project_replacement[0], project_replacement[1])
    ulp_project_cmake_path = test_app_copy / ulp_project_cmake
    ulp_project_cmake_path.write_text(
        ulp_project_cmake_path.read_text(encoding='utf-8')
        + '\nget_property(ulp_enabled_languages GLOBAL PROPERTY ENABLED_LANGUAGES)\n'
        + 'file(WRITE "${CMAKE_BINARY_DIR}/enabled_languages.txt" "${ulp_enabled_languages}\\n")\n',
        encoding='utf-8',
    )

    idf_py(f'-DIDF_TARGET={target}', '-DSDKCONFIG=build/sdkconfig', 'build')

    child_languages = (
        (Path('build/subprojects') / subproject_dir / 'enabled_languages.txt').read_text(encoding='utf-8').strip()
    )
    for language in enabled_languages:
        assert language in child_languages.split(';')
    for language in disabled_languages:
        assert language not in child_languages.split(';')
