# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import json
import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc


@pytest.mark.usefixtures('test_app_copy')
def test_config_selected_default_components(idf_py: IdfPyFunc) -> None:
    """Components with a project_include.cmake that calls idf_project_add_default_build_component()
    must be pulled into the default build when their Kconfig feature is enabled, even though nothing
    in the project explicitly depends on them.

    Enable coredump and gdbstub via sdkconfig.defaults, build, and assert both components end up in
    build_components.
    """
    logging.info('Testing config-selected default components (coredump, gdbstub)')

    # Coredump to UART (no dedicated coredump partition needed) and runtime gdbstub.
    Path('sdkconfig.defaults').write_text('CONFIG_ESP_COREDUMP_ENABLE_TO_UART=y\nCONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME=y\n')

    idf_py('build')

    proj_desc = json.loads(Path('build/project_description.json').read_text())
    build_components = proj_desc.get('build_components', [])

    for comp in ('espcoredump', 'esp_gdbstub'):
        assert comp in build_components, (
            f'{comp} should be added as a default build component when its feature is enabled, '
            f'got build_components: {build_components}'
        )
