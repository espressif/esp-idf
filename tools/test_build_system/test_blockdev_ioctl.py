# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Build-system integration tests for the esp_blockdev ioctl overlap checker."""

import logging
from pathlib import Path

import pytest
from test_build_system_helpers import IdfPyFunc
from test_build_system_helpers import replace_in_file

COMP_A_DEFS_NO_OVERLAP = """\
#include "esp_blockdev.h"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(comp_a, 0x10, 0x1F);
"""

COMP_B_DEFS_NO_OVERLAP = """\
#include "esp_blockdev.h"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(comp_b, 0x20, 0x2F);
"""

COMP_B_DEFS_OVERLAP = """\
#include "esp_blockdev.h"

ESP_BLOCKDEV_RESERVE_CMD_RANGE(comp_b, 0x18, 0x2F);
"""

COMPONENT_CMAKELISTS = """\
idf_component_register()

idf_build_set_property(
    ESP_BLOCKDEV_IOCTL_DEF_FILES
    "${{CMAKE_CURRENT_LIST_DIR}}/include/{filename}"
    APPEND
)
"""


def _add_component(app_path: Path, name: str, defs_content: str) -> None:
    comp_dir = app_path / 'components' / name
    comp_dir.mkdir(parents=True, exist_ok=True)
    include_dir = comp_dir / 'include'
    include_dir.mkdir(exist_ok=True)

    filename = f'{name}_ioctl_defs.h'
    (include_dir / filename).write_text(defs_content)
    (comp_dir / 'CMakeLists.txt').write_text(COMPONENT_CMAKELISTS.format(filename=filename))


def _wire_components(app_path: Path) -> None:
    replace_in_file(
        app_path / 'main' / 'CMakeLists.txt',
        '# placeholder_inside_idf_component_register',
        'REQUIRES esp_blockdev comp_a comp_b\n    # placeholder_inside_idf_component_register',
    )


@pytest.mark.usefixtures('test_app_copy')
def test_ioctl_overlap_checker_passes_clean_build(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Build succeeds when registered ioctl ranges do not overlap."""
    logging.info('Testing ioctl overlap checker with non-overlapping ranges')
    _add_component(test_app_copy, 'comp_a', COMP_A_DEFS_NO_OVERLAP)
    _add_component(test_app_copy, 'comp_b', COMP_B_DEFS_NO_OVERLAP)
    _wire_components(test_app_copy)

    ret = idf_py('build')
    assert ret.returncode == 0


@pytest.mark.usefixtures('test_app_copy')
def test_ioctl_overlap_checker_fails_on_overlap(idf_py: IdfPyFunc, test_app_copy: Path) -> None:
    """Build fails when registered ioctl ranges overlap."""
    logging.info('Testing ioctl overlap checker detects overlapping ranges')
    _add_component(test_app_copy, 'comp_a', COMP_A_DEFS_NO_OVERLAP)
    _add_component(test_app_copy, 'comp_b', COMP_B_DEFS_OVERLAP)
    _wire_components(test_app_copy)

    ret = idf_py('build', check=False)
    assert ret.returncode != 0
    output = ret.stdout + ret.stderr
    assert 'ioctl reservation overlap' in output
    assert 'comp_a' in output
    assert 'comp_b' in output
