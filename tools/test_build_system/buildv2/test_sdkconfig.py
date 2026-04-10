# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import file_contains
from test_build_system_helpers import replace_in_file


@pytest.mark.usefixtures('test_app_copy')
def test_kconfig_menus_json_not_generated(idf_py: IdfPyFunc) -> None:
    """v2 should NOT generate kconfig_menus.json (v1 generates it)."""
    logging.info('Testing that kconfig_menus.json is NOT generated in v2')

    idf_py('reconfigure')

    assert not Path('build/config/kconfig_menus.json').exists(), 'kconfig_menus.json should not be generated in v2'


@pytest.mark.usefixtures('test_app_copy')
def test_all_discovered_components_contribute_kconfig(idf_py: IdfPyFunc) -> None:
    """All discovered components contribute Kconfig even if not in the dependency tree."""
    logging.info('Testing all discovered components contribute Kconfig')

    comp_dir = Path('components/kconfig_only_comp')
    comp_dir.mkdir(parents=True, exist_ok=True)
    (comp_dir / 'CMakeLists.txt').write_text('idf_component_register()\n')
    (comp_dir / 'Kconfig').write_text(
        'config KCONFIG_ONLY_COMP_TEST_OPTION\n    bool "Test option from kconfig_only_comp"\n    default n\n'
    )

    idf_py('reconfigure')

    sdkconfig_json = json.loads(Path('build/config/sdkconfig.json').read_text())
    assert any('KCONFIG_ONLY_COMP_TEST_OPTION' in key for key in sdkconfig_json), (
        'KCONFIG_ONLY_COMP_TEST_OPTION should appear in sdkconfig.json - '
        'all discovered components must contribute Kconfig in v2'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_config_report_target(idf_py: IdfPyFunc) -> None:
    """idf_create_config_report() creates a config-report target that produces a report file."""
    logging.info('Testing config-report target')

    idf_py('reconfigure')
    ninja = Path('build/build.ninja').read_text()
    assert 'config-report' in ninja, '"config-report" target not found in build.ninja'

    idf_py('config-report')
    assert Path('build/config/kconfig_parse_report.json').exists(), (
        'config-report should produce kconfig_parse_report.json'
    )


@pytest.mark.usefixtures('test_app_copy')
def test_save_defconfig_target(idf_py: IdfPyFunc) -> None:
    """idf_create_save_defconfig() creates a save-defconfig target."""
    logging.info('Testing save-defconfig target')

    idf_py('reconfigure')
    ninja = Path('build/build.ninja').read_text()
    assert 'save-defconfig' in ninja, '"save-defconfig" target not found in build.ninja'


@pytest.mark.usefixtures('test_app_copy')
def test_kconfig_driven_requires(idf_py: IdfPyFunc) -> None:
    """CONFIG_ variables should be usable in REQUIRES."""
    logging.info('Testing Kconfig-driven REQUIRES')

    opt_comp = Path('components/optional_comp')
    opt_comp.mkdir(parents=True, exist_ok=True)
    (opt_comp / 'CMakeLists.txt').write_text('idf_component_register(SRCS "optional.c")\n')
    (opt_comp / 'optional.c').write_text('void optional_func(void) {}')
    (opt_comp / 'Kconfig').write_text('config OPTIONAL_COMP_ENABLE\n    bool "Enable optional_comp"\n    default n\n')

    cond_comp = Path('components/cond_dep_comp')
    cond_comp.mkdir(parents=True, exist_ok=True)
    (cond_comp / 'CMakeLists.txt').write_text(
        'if(CONFIG_OPTIONAL_COMP_ENABLE)\n'
        '  set(opt_reqs optional_comp)\n'
        'else()\n'
        '  set(opt_reqs "")\n'
        'endif()\n'
        'idf_component_register(SRCS "cond.c" REQUIRES ${opt_reqs})\n'
    )
    (cond_comp / 'cond.c').write_text('void cond_func(void) {}')

    replace_in_file(
        'main/CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES cond_dep_comp\n    # placeholder_inside_idf_component_register',
    )

    Path('sdkconfig.defaults').write_text('CONFIG_OPTIONAL_COMP_ENABLE=y\n')

    idf_py('build')

    proj_desc = json.loads(Path('build/project_description.json').read_text())
    build_components = proj_desc.get('build_components', [])
    assert 'optional_comp' in build_components, (
        f'optional_comp should be in build_components when CONFIG_OPTIONAL_COMP_ENABLE=y '
        f'(Kconfig-driven REQUIRES), got: {build_components}'
    )


def test_sdkconfig_preserves_managed_component_options(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Managed component Kconfig options should survive reconfigure rounds.

    kconfgen's --output config pass can drop options it does not recognise
    (e.g. from managed components discovered after the initial configure).
    This test verifies the value is preserved across reconfigure.
    """
    logging.info('sdkconfig options from managed components should be preserved during reconfigure')

    managed_comp_dir = test_app_copy / '..' / 'managed_comp'
    managed_comp_dir.mkdir(parents=True)
    (managed_comp_dir / 'CMakeLists.txt').write_text('idf_component_register()\n')
    (managed_comp_dir / 'Kconfig').write_text(
        'config MANAGED_COMP_TEST_OPTION\n    int "Managed component test option"\n    default 10\n'
    )

    (test_app_copy / 'main' / 'idf_component.yml').write_text(
        'dependencies:\n  managed_comp:\n    path: "../../managed_comp"\n'
    )

    (test_app_copy / 'sdkconfig').write_text('CONFIG_MANAGED_COMP_TEST_OPTION=30\n')

    idf_py('reconfigure')

    assert file_contains(test_app_copy / 'sdkconfig', 'CONFIG_MANAGED_COMP_TEST_OPTION=30')
