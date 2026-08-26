# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Host build tests for custom LP-core linker layouts under the CMake v2
# full-subproject build. The ULP program is built through ulp_project_default(),
# so these exercise the buildv2 linker-script path
# (components/ulp/CMakeLists_v2.txt), the counterpart of the CMake v1 tests in
# ../lp_core_custom_linker.
from __future__ import annotations

import os
import shutil
import subprocess

import pytest
from esp_bool_parser import parse_bool_expr
from esp_bool_parser.constants import PREVIEW_TARGETS
from esp_bool_parser.constants import SUPPORTED_TARGETS

APP_DIR = os.path.dirname(os.path.abspath(__file__))
LAYOUTS = os.path.join(APP_DIR, 'main', 'ulp', 'layouts')

# Evaluate the SoC-caps expression against every known target so the set tracks
# the config as the LP core lands on new targets.
_expr = parse_bool_expr('SOC_LP_CORE_SUPPORTED == 1')
LP_CORE_TARGETS = [t for t in (*SUPPORTED_TARGETS, *PREVIEW_TARGETS) if _expr.get_value(t, '')]


def _build_dir(target: str) -> str:
    return os.path.join(APP_DIR, f'build_{target}')


def _build(target: str, custom_layout: str, clean: bool) -> tuple[int, str]:
    # The parent app is CMake v2-native (its CMakeLists includes cmakev2/idf.cmake
    # directly), so a plain build with -DIDF_TARGET drives the buildv2 path.
    build_dir = _build_dir(target)
    if clean:
        shutil.rmtree(build_dir, ignore_errors=True)
    # Keep sdkconfig inside the build dir: the buildv2 flow refuses to switch
    # IDF_TARGET while a mismatched app-dir sdkconfig exists, so per-target
    # isolation is required for the parametrized targets.
    r = subprocess.run(
        [
            'idf.py',
            '--preview',
            '-B',
            build_dir,
            f'-DIDF_TARGET={target}',
            f'-DSDKCONFIG={os.path.join(build_dir, "sdkconfig")}',
            f'-DCUSTOM_ULP_LAYOUT={custom_layout}',
            'build',
        ],
        cwd=APP_DIR,
        capture_output=True,
        text=True,
    )
    return r.returncode, r.stdout + r.stderr


def _marker(target: str) -> int:
    # The ULP executable of a full subproject is emitted under build/subprojects/.
    elf = os.path.join(_build_dir(target), 'subprojects', 'ulp_main', 'ulp_main.elf')
    assert os.path.isfile(elf), 'ULP elf was not produced'
    out = subprocess.run(['riscv32-esp-elf-nm', elf], capture_output=True, text=True).stdout
    for line in out.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[2] == '_custom_linker_marker':
            return int(parts[0], 16)
    raise AssertionError('custom marker symbol absent from the linked ULP image')


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_v2_positive_custom_layout(target: str) -> None:
    rc, out = _build(target, os.path.join(LAYOUTS, 'good.ld'), clean=True)
    assert rc == 0, f'custom layout should build under buildv2:\n{out[-2000:]}'
    assert _marker(target) == 0xCAFEBABE, 'custom layout marker not linked under buildv2'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_v2_incremental_layout_switch(target: str) -> None:
    # Frantisek's concern, verified for buildv2: switch the LINKER_LAYOUT on an
    # incremental build (no wipe) with the new layout aged older than the
    # previously generated script; the image must relink against the new layout.
    rc, out = _build(target, os.path.join(LAYOUTS, 'good.ld'), clean=True)
    assert rc == 0, f'initial buildv2 build should succeed:\n{out[-2000:]}'
    assert _marker(target) == 0xCAFEBABE

    switch_layout = os.path.join(LAYOUTS, 'good_switch.ld')
    os.utime(switch_layout, (1600000000, 1600000000))  # 2020-09-13, older than the built script
    rc, out = _build(target, switch_layout, clean=False)
    assert rc == 0, f'incremental buildv2 rebuild after switching layout should succeed:\n{out[-2000:]}'
    marker = _marker(target)
    assert marker == 0xD00DFEED, (
        f'stale linker script under buildv2: switching the LINKER_LAYOUT on an incremental '
        f'build did not relink (marker still {marker:#x}, expected 0xd00dfeed)'
    )
