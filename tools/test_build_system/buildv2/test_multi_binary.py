# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file


def _create_app2_component(base_dir: Path) -> None:
    """Create a second application component for multi-binary tests."""
    app2_dir = base_dir / 'components' / 'app2'
    app2_dir.mkdir(parents=True, exist_ok=True)
    (app2_dir / 'CMakeLists.txt').write_text('idf_component_register(SRCS "app2.c")\n')
    (app2_dir / 'app2.c').write_text('void app_main(void) {}\n')


@pytest.mark.usefixtures('test_app_copy')
def test_multi_binary_all_features(idf_py: IdfPyFunc) -> None:
    """Comprehensive test covering all multi-binary features in a single build."""
    logging.info('Testing multi-binary: all features in single comprehensive build')

    _create_app2_component(Path('.'))

    replace_in_file(
        'CMakeLists.txt',
        'idf_project_default()',
        'idf_project_init()\n'
        # Create two executables with mapfile targets for size reports
        'idf_build_executable(app1 COMPONENTS main MAPFILE_TARGET app1_mapfile)\n'
        'idf_build_executable(app2 COMPONENTS app2 MAPFILE_TARGET app2_mapfile)\n'
        # Create binaries with flash targets
        'idf_build_binary(app1\n'
        '    TARGET app1_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/app1.bin")\n'
        'idf_build_binary(app2\n'
        '    TARGET app2_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/app2.bin")\n'
        # Add flash targets - test both FLASH flag behaviors:
        # app1: FLASH flag adds to global 'flash' target (idf.py flash)
        # app2: No FLASH flag creates only named target (idf.py app2-flash)
        'idf_flash_binary(app1_bin TARGET app1-flash NAME app1 FLASH)\n'
        'idf_flash_binary(app2_bin TARGET app2-flash NAME app2)\n'
        # Check binary size for app1
        'idf_check_binary_size(app1_bin)\n'
        # Build both binaries
        'add_custom_target(app ALL DEPENDS app1_bin app2_bin)\n'
        # Generate metadata for both executables and binaries
        'idf_build_generate_metadata(EXECUTABLE app1)\n'
        'idf_build_generate_metadata(EXECUTABLE app2\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/project_description_app2.json")\n'
        'idf_build_generate_metadata(BINARY app1_bin\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/project_description_bin.json")\n'
        # Create menuconfig and confserver targets
        'idf_create_menuconfig(app1 TARGET menuconfig-app1)\n'
        'idf_create_menuconfig(app2 TARGET menuconfig-app2)\n'
        'idf_create_confserver(app1 TARGET confserver-app1)\n'
        'idf_create_confserver(app2 TARGET confserver-app2)\n'
        # Create depgraph targets
        'idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED YES)\n'
        'idf_build_generate_depgraph(app1)\n'
        'idf_build_generate_depgraph(app2\n'
        '    OUTPUT_FILE "${CMAKE_BINARY_DIR}/component_deps_app2.dot")\n'
        # Create size report targets
        'if(TARGET app1_mapfile)\n'
        '  idf_create_size_report(app1_mapfile TARGET size-app1)\n'
        'endif()\n'
        'if(TARGET app2_mapfile)\n'
        '  idf_create_size_report(app2_mapfile TARGET size-app2)\n'
        'endif()\n'
        # Generate flasher args
        'idf_build_generate_flasher_args()\n',
    )

    idf_py('build')

    # Verify both executables are created
    assert Path('build/app1.elf').exists(), 'app1.elf not created'
    assert Path('build/app2.elf').exists(), 'app2.elf not created'

    # Verify both binaries are created
    assert Path('build/app1.bin').exists(), 'app1.bin should be produced'
    assert Path('build/app2.bin').exists(), 'app2.bin should be produced'

    # Verify executable metadata
    proj_desc = json.loads(Path('build/project_description.json').read_text())
    build_components = proj_desc.get('build_components', [])
    assert 'main' in build_components, f'main component should be in app1 build_components, got: {build_components}'

    # Verify per-binary metadata
    assert Path('build/project_description.json').exists(), 'Default project_description.json not created'
    assert Path('build/project_description_app2.json').exists(), 'Per-binary project_description_app2.json not created'
    assert Path('build/project_description_bin.json').exists(), (
        'Binary metadata project_description_bin.json not created'
    )

    # Verify flasher_args.json is generated and references both apps
    flasher_args_path = Path('build/flasher_args.json')
    assert flasher_args_path.exists(), 'flasher_args.json not generated for multi-binary build'
    flasher_args = json.loads(flasher_args_path.read_text())
    flash_files = flasher_args.get('flash_files', {})
    flash_values = ' '.join(str(v) for v in flash_files.values())
    assert 'app1' in flash_values, f'flasher_args.json should reference app1, got flash_files: {flash_files}'

    # Read build.ninja once for all checks
    ninja = Path('build/build.ninja').read_text()

    # Verify flash targets
    assert 'app1-flash' in ninja, '"app1-flash" target not found in build.ninja'
    assert 'app2-flash' in ninja, '"app2-flash" target not found in build.ninja'

    # Verify configure targets
    assert 'menuconfig-app1' in ninja, '"menuconfig-app1" target not found'
    assert 'menuconfig-app2' in ninja, '"menuconfig-app2" target not found'
    assert 'confserver-app1' in ninja, '"confserver-app1" target not found'
    assert 'confserver-app2' in ninja, '"confserver-app2" target not found'
    assert Path('build/component_deps.dot').exists(), 'component_deps.dot not created for app1'
    assert Path('build/component_deps_app2.dot').exists(), 'component_deps_app2.dot not created for app2'

    # Verify size report targets
    assert 'size-app1' in ninja, '"size-app1" target not found in build.ninja'
    assert 'size-app2' in ninja, '"size-app2" target not found in build.ninja'
