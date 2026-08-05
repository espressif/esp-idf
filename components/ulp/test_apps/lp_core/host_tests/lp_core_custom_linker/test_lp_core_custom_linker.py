# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
# Host build tests for custom LP-core linker layouts. Each test builds this app
# with a different layout via the LINKER_LAYOUT option and checks the result:
# positive cases verify the build succeeds and the marker symbol resolves;
# negative cases verify that the expected ASSERT fires and the build fails.
from __future__ import annotations

import os
import shutil
import subprocess
import tempfile

import pytest
from esp_bool_parser import parse_bool_expr
from esp_bool_parser.constants import PREVIEW_TARGETS
from esp_bool_parser.constants import SUPPORTED_TARGETS

APP_DIR = os.path.dirname(os.path.abspath(__file__))
LAYOUTS = os.path.join(APP_DIR, 'main', 'ulp', 'layouts')


def _targets(soc_expr: str) -> list[str]:
    # Evaluate a SoC-caps expression against every known target, so the set
    # tracks the config as the LP core lands on new targets.
    expr = parse_bool_expr(soc_expr)
    return [t for t in (*SUPPORTED_TARGETS, *PREVIEW_TARGETS) if expr.get_value(t, '')]


LP_CORE_TARGETS = _targets('SOC_LP_CORE_SUPPORTED == 1')
# MEMPROT needs the LP core PMP.
LP_CORE_MEMPROT_TARGETS = _targets('SOC_LP_CORE_SUPPORTED == 1 and SOC_LP_CORE_HAS_PMP == 1')
HP_MEM_DEFAULTS = 'CONFIG_ULP_COPROC_RUN_FROM_HP_MEM=y\nCONFIG_ULP_COPROC_RESERVE_HP_MEM_BYTES=0x4000'


def _build(tag: str, custom_layout: str, target: str, extra_defaults: str | None = None) -> tuple[int, str, str]:
    build_dir = os.path.join(APP_DIR, f'build_{tag}_{target}')
    shutil.rmtree(build_dir, ignore_errors=True)
    # --preview keeps the command uniform across released and preview targets.
    # Keep sdkconfig inside the build dir so parametrized targets never share (or
    # stale-contaminate) the app-dir sdkconfig.
    common = [
        'idf.py',
        '--preview',
        '-B',
        build_dir,
        f'-DSDKCONFIG={os.path.join(build_dir, "sdkconfig")}',
        f'-DCUSTOM_ULP_LAYOUT={custom_layout}',
    ]
    combined: str | None = None
    if extra_defaults:
        fd, combined = tempfile.mkstemp(suffix='.defaults')
        with os.fdopen(fd, 'w') as out_f:
            with open(os.path.join(APP_DIR, 'sdkconfig.defaults')) as base_f:
                out_f.write(base_f.read())
            out_f.write('\n' + extra_defaults + '\n')
        common.append(f'-DSDKCONFIG_DEFAULTS={combined}')
    try:
        cfg = subprocess.run(common + ['set-target', target], cwd=APP_DIR, capture_output=True, text=True)
        out = cfg.stdout + cfg.stderr
        if cfg.returncode != 0:
            return cfg.returncode, out, build_dir
        bld = subprocess.run(common + ['build'], cwd=APP_DIR, capture_output=True, text=True)
        return bld.returncode, out + bld.stdout + bld.stderr, build_dir
    finally:
        if combined:
            os.unlink(combined)


def _rebuild(build_dir: str, custom_layout: str) -> tuple[int, str]:
    # Incremental reconfigure + build of an existing build dir with a different
    # LINKER_LAYOUT: no wipe, no re-run of set-target. This is the path that must
    # still pick up the new layout.
    r = subprocess.run(
        [
            'idf.py',
            '-B',
            build_dir,
            f'-DSDKCONFIG={os.path.join(build_dir, "sdkconfig")}',
            f'-DCUSTOM_ULP_LAYOUT={custom_layout}',
            'build',
        ],
        cwd=APP_DIR,
        capture_output=True,
        text=True,
    )
    return r.returncode, r.stdout + r.stderr


def _nm(build_dir: str) -> str:
    # The ULP ELF is emitted at a fixed path under the build dir.
    elf = os.path.join(build_dir, 'esp-idf', 'main', 'ulp_main', 'ulp_main.elf')
    assert os.path.isfile(elf), 'ULP elf was not produced'
    return subprocess.run(['riscv32-esp-elf-nm', elf], capture_output=True, text=True).stdout


def _symbols(build_dir: str) -> dict[str, int]:
    # nm prints "<hex value> <type> <name>"; keep the defined symbols we can resolve.
    syms: dict[str, int] = {}
    for line in _nm(build_dir).splitlines():
        parts = line.split()
        if len(parts) == 3 and all(c in '0123456789abcdefABCDEF' for c in parts[0]):
            syms[parts[2]] = int(parts[0], 16)
    return syms


def _assert_marker(syms: dict[str, int]) -> None:
    assert '_custom_linker_marker' in syms, 'custom marker symbol absent'
    assert syms['_custom_linker_marker'] == 0xCAFEBABE, 'custom marker not resolved to its PROVIDE value'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_positive_custom_layout(target: str) -> None:
    rc, out, build_dir = _build('good', os.path.join(LAYOUTS, 'good.ld'), target)
    assert rc == 0, f'custom layout should build:\n{out[-2000:]}'
    _assert_marker(_symbols(build_dir))


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_positive_multiregion_layout(target: str) -> None:
    rc, out, build_dir = _build('good_multiregion', os.path.join(LAYOUTS, 'good_multiregion.ld'), target)
    assert rc == 0, f'multi-region custom layout should build:\n{out[-2000:]}'
    _assert_marker(_symbols(build_dir))


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_positive_hp_mem_custom_layout(target: str) -> None:
    rc, out, build_dir = _build(
        'good_hp_mem', os.path.join(LAYOUTS, 'good_hp_mem.ld'), target, extra_defaults=HP_MEM_DEFAULTS
    )
    assert rc == 0, f'custom layout routing to HP RAM should build:\n{out[-2000:]}'
    _assert_marker(_symbols(build_dir))


@pytest.mark.parametrize('target', LP_CORE_MEMPROT_TARGETS)
def test_positive_custom_layout_memprot(target: str) -> None:
    rc, out, build_dir = _build(
        'good_memprot', os.path.join(LAYOUTS, 'good.ld'), target, extra_defaults='CONFIG_ULP_LP_CORE_MEMPROT=y'
    )
    assert rc == 0, f'custom layout defining _lp_text_end should build under MEMPROT:\n{out[-2000:]}'
    _assert_marker(_symbols(build_dir))


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_positive_stack_window_override(target: str) -> None:
    rc, out, build_dir = _build('good_stack_window', os.path.join(LAYOUTS, 'good_stack_window.ld'), target)
    assert rc == 0, f'layout overriding __stack_top/__stack_size should build:\n{out[-2000:]}'
    syms = _symbols(build_dir)
    _assert_marker(syms)
    for sym in ('__stack_top', '__stack_size', '_lp_data_end'):
        assert sym in syms, f'{sym} symbol absent from the linked image'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_positive_stack_override_takes_effect(target: str) -> None:
    # A layout that sets __stack_top/__stack_size to values distinct from the
    # base defaults must win: the linked symbols resolve to the layout's values,
    # proving the base PROVIDE() defaults yield.
    rc, out, build_dir = _build('good_stack_override', os.path.join(LAYOUTS, 'good_stack_override.ld'), target)
    assert rc == 0, f'layout with distinct stack overrides should build:\n{out[-2000:]}'
    syms = _symbols(build_dir)
    _assert_marker(syms)
    for sym in ('__stack_top', '__stack_size', '_lp_shared_start', '_lp_data_end'):
        assert sym in syms, f'{sym} symbol absent from the linked image'
    window_end = syms['_lp_shared_start']  # the base default for __stack_top
    # __stack_top overridden to window_end - 0x40 (not the default window end).
    assert syms['__stack_top'] == window_end - 0x40, (
        f'__stack_top override ignored: {syms["__stack_top"]:#x} != {window_end - 0x40:#x}'
    )
    assert syms['__stack_top'] != window_end, '__stack_top still at the default (override did not win)'
    # __stack_size overridden to 0x200 (not the free-space default).
    assert syms['__stack_size'] == 0x200, f'__stack_size override ignored: {syms["__stack_size"]:#x} != 0x200'
    default_size = syms['__stack_top'] - syms['_lp_data_end']
    assert syms['__stack_size'] != default_size, '__stack_size still at the default (override did not win)'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_incremental_layout_switch_picks_up_new_layout(target: str) -> None:
    # The LINKER_LAYOUT path reaches the preprocessor only as a flag inside the
    # response file, and a committed layout file's mtime is normally older than
    # the already-generated script. Switching the layout on an incremental build
    # (no wipe) must still regenerate the linker script; otherwise the image
    # silently links against the previous layout.
    rc, out, build_dir = _build('switch', os.path.join(LAYOUTS, 'good.ld'), target)
    assert rc == 0, f'initial build with good.ld should succeed:\n{out[-2000:]}'
    assert _symbols(build_dir)['_custom_linker_marker'] == 0xCAFEBABE, 'initial layout not linked'

    # Age the target layout so it is older than the script the first build just
    # generated -- the mtime relationship that hides the change from the build.
    switch_layout = os.path.join(LAYOUTS, 'good_switch.ld')
    old = 1600000000  # 2020-09-13, safely older than any build artifact
    os.utime(switch_layout, (old, old))

    rc, out = _rebuild(build_dir, switch_layout)
    assert rc == 0, f'incremental rebuild after switching the layout should succeed:\n{out[-2000:]}'
    marker = _symbols(build_dir)['_custom_linker_marker']
    assert marker == 0xD00DFEED, (
        f'stale linker script: switching the LINKER_LAYOUT on an incremental build did not '
        f'regenerate the script (marker still {marker:#x}, expected 0xd00dfeed)'
    )


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_stack_size_too_large(target: str) -> None:
    rc, out, _ = _build('bad_stack_oversize', os.path.join(LAYOUTS, 'bad_stack_oversize.ld'), target)
    assert rc != 0, 'a __stack_size that overruns the data region must fail the build'
    assert 'stack region overlaps the data region' in out, 'bottom-bounds stack check did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_missing_data_end(target: str) -> None:
    rc, out, _ = _build('bad_no_data_end', os.path.join(LAYOUTS, 'bad_no_data_end.ld'), target)
    assert rc != 0, 'a layout not marking _lp_data_end must fail the build'
    assert 'end of writable data is not marked' in out, 'the _lp_data_end contract check did not fire'


@pytest.mark.parametrize('target', LP_CORE_MEMPROT_TARGETS)
def test_negative_missing_text_end_under_memprot(target: str) -> None:
    rc, out, _ = _build(
        'bad_no_text_end',
        os.path.join(LAYOUTS, 'bad_no_text_end.ld'),
        target,
        extra_defaults='CONFIG_ULP_LP_CORE_MEMPROT=y',
    )
    assert rc != 0, 'a MEMPROT layout missing _lp_text_end must fail the build'
    assert '_lp_text_end must be defined' in out, 'the _lp_text_end MEMPROT check did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_reset_vector_check_fails_build(target: str) -> None:
    rc, out, _ = _build('bad_origin', os.path.join(LAYOUTS, 'bad_origin.ld'), target)
    assert rc != 0, 'a misplaced lp_ram origin must fail the build'
    assert 'reset vector is not at the required offset' in out, 'reset-vector check did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_sizing_check_fails_build(target: str) -> None:
    rc, out, _ = _build('bad_oversize', os.path.join(LAYOUTS, 'bad_oversize.ld'), target)
    assert rc != 0, 'an oversized lp_ram must fail the build'
    assert 'exceeds the reserved LP coprocessor memory' in out, 'sizing check did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_shared_overrun_check_fails_build(target: str) -> None:
    rc, out, _ = _build('bad_shared', os.path.join(LAYOUTS, 'bad_shared_overrun.ld'), target)
    assert rc != 0, 'a stack overrunning the shared region must fail the build'
    assert 'overruns into the reserved shared-memory region' in out, 'shared-overrun guardrail did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_missing_reset_vector(target: str) -> None:
    rc, out, _ = _build('bad_no_rv', os.path.join(LAYOUTS, 'bad_no_reset_vector.ld'), target)
    assert rc != 0, 'omitting LP_CORE_TEXT_START must fail the build'
    assert 'reset vector is not at the required offset' in out, 'reset-vector position check did not fire'


@pytest.mark.parametrize('target', LP_CORE_TARGETS)
def test_negative_missing_linker_rejected_at_configure(target: str) -> None:
    rc, out, _ = _build('missing', '/nonexistent/custom_layout.ld', target)
    assert rc != 0, 'a non-existent LINKER_LAYOUT path must be rejected'
    assert 'LINKER_LAYOUT linker script not found' in out, 'API path validation did not fire'
