# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
from pathlib import Path

from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import file_contains


def test_sdkconfig_preserves_managed_component_options(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    logging.info('sdkconfig options from managed components should be preserved during reconfigure')

    # Create a local managed component outside the project directory.
    # Since it is referenced via path in idf_component.yml, the component
    # manager processes it as a managed component whose Kconfig is only
    # available after the CM run -- i.e. after the initial kconfgen round.
    managed_comp_dir = test_app_copy / '..' / 'managed_comp'
    managed_comp_dir.mkdir(parents=True)
    (managed_comp_dir / 'CMakeLists.txt').write_text('idf_component_register()\n')
    (managed_comp_dir / 'Kconfig').write_text(
        'config MANAGED_COMP_TEST_OPTION\n    int "Managed component test option"\n    default 10\n'
    )

    # Reference the local component as a managed dependency
    (test_app_copy / 'main' / 'idf_component.yml').write_text(
        'dependencies:\n  managed_comp:\n    path: "../../managed_comp"\n'
    )

    # Write a non-default value directly into sdkconfig (not sdkconfig.defaults).
    # Before the fix, kconfgen would drop this unknown option via --output config
    # during the first generation round (before the component manager makes the
    # managed component's Kconfig available), causing the value to revert to the
    # default (10) in the final sdkconfig.
    (test_app_copy / 'sdkconfig').write_text('CONFIG_MANAGED_COMP_TEST_OPTION=30\n')

    idf_py('reconfigure')

    assert file_contains(test_app_copy / 'sdkconfig', 'CONFIG_MANAGED_COMP_TEST_OPTION=30')
