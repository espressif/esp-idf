# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_v1_component_works_in_v2(idf_py: IdfPyFunc) -> None:
    """Standard v1-style idf_component_register() component should work in v2"""
    logging.info('Testing v1 component compatibility in v2')

    # The buildv2_test_app already uses idf_component_register - just build it
    idf_py('build')
    assert Path('build/build_test_app.elf').exists()
    # Verify the v1-style component (main) was discovered and included
    proj_desc = json.loads(Path('build/project_description.json').read_text())
    assert 'main' in proj_desc.get('build_components', []), (
        'v1-style "main" component should appear in build_components'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_idf_build_v2_available(idf_py: IdfPyFunc) -> None:
    """IDF_BUILD_V2 should be available as both CMake variable and build property."""
    logging.info('Testing IDF_BUILD_V2 availability')

    comp_dir = Path('components/v2_guard_comp')
    comp_dir.mkdir(parents=True, exist_ok=True)

    # Write marker in v2 branch
    (comp_dir / 'CMakeLists.txt').write_text(
        'if(IDF_BUILD_V2)\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/v2_guard_check.txt" "V2_BRANCH")\n'
        '  idf_component_register(SRCS "v2_guard.c")\n'
        'else()\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/v2_guard_check.txt" "V1_BRANCH")\n'
        '  idf_component_register(SRCS "v2_guard.c")\n'
        'endif()\n'
    )
    (comp_dir / 'v2_guard.c').write_text('void v2_guard_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES v2_guard_comp\n    # placeholder_inside_idf_component_register',
    )

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        'idf_build_get_property(is_v2 IDF_BUILD_V2)\n'
        'file(WRITE "${CMAKE_BINARY_DIR}/idf_v2_property.txt" "${is_v2}")\n',
    )

    idf_py('reconfigure')

    # Verify marker in v2 branch
    guard_check = Path('build/v2_guard_check.txt').read_text().strip()
    assert guard_check == 'V2_BRANCH', f'Expected IDF_BUILD_V2 guard to take V2_BRANCH, got: {guard_check!r}'

    # Verify build property is available
    property_file = Path('build/idf_v2_property.txt')
    assert property_file.exists(), 'IDF_BUILD_V2 property check file not created'
    value = property_file.read_text().strip()
    assert value in ('TRUE', 'true', 'YES', 'yes', 'y', '1', 'ON', 'on'), (
        f'IDF_BUILD_V2 property should be truthy, got: {value!r}'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_minimal_build_ignored_with_warning(idf_py: IdfPyFunc) -> None:
    """Setting MINIMAL_BUILD via idf_build_set_property should produce an obsolete warning."""
    logging.info('Testing MINIMAL_BUILD property warning')

    replace_in_file(
        'CMakeLists.txt', '# placeholder_after_project_default', 'idf_build_set_property(MINIMAL_BUILD ON)\n'
    )

    result = idf_py('reconfigure')
    combined = (result.stdout or '') + (result.stderr or '')
    assert 'MINIMAL_BUILD' in combined, 'MINIMAL_BUILD property name should appear in warning output'
    lower = combined.lower()
    assert 'obsolete' in lower or 'ignored' in lower or 'deprecated' in lower, (
        'Warning should mention MINIMAL_BUILD is obsolete, ignored, or deprecated'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_build_components_not_available(idf_py: IdfPyFunc) -> None:
    """BUILD_COMPONENTS is not available in v2."""
    logging.info('Testing BUILD_COMPONENTS unavailability in v2')

    replace_in_file(
        'CMakeLists.txt',
        '# placeholder_after_project_default',
        # Check the CMake variable
        'if(DEFINED BUILD_COMPONENTS)\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/build_components_check.txt" "FOUND")\n'
        'else()\n'
        '  file(WRITE "${CMAKE_BINARY_DIR}/build_components_check.txt" "NOT_FOUND")\n'
        'endif()\n'
        # Using BUILD_COMPONENTS property must cause a fatal error in v2
        'idf_build_get_property(_bc BUILD_COMPONENTS)\n',
    )

    result = idf_py('reconfigure', check=False)

    assert result.returncode != 0, 'reconfigure must fail when BUILD_COMPONENTS property is used'
    combined = (result.stdout or '') + (result.stderr or '')
    assert 'BUILD_COMPONENTS' in combined, 'Error output must mention BUILD_COMPONENTS'
    assert 'not supported' in combined.lower(), 'Error output must state that BUILD_COMPONENTS is not supported'

    check_file = Path('build/build_components_check.txt')
    assert check_file.exists(), 'Variable check file should be written before the fatal error'
    assert check_file.read_text().strip() == 'NOT_FOUND', 'BUILD_COMPONENTS variable should not be available in v2'
