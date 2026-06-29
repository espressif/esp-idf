# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


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
