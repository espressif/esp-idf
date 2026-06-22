# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import os.path
import re
import shutil
import subprocess
import sys
import time
import typing

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

if typing.TYPE_CHECKING:
    from conftest import OpenOCD

# Function-trace events are a SystemView module. The first (only) registered module
# gets EventOffset 512, so enter uses ID 512 and exit uses ID 513.
FT_EVENT_ENTER = 512
FT_EVENT_EXIT = 513

# Demo source compiled with instrumentation. Its functions must be traced.
INSTRUMENTED_SOURCE = 'ft_demo.c'
# Demo sources excluded by file. Their functions must never be traced.
EXCLUDED_SOURCES = ['ft_demo_hot.c', 'ft_demo_quiet.c']


def _encode_event_id(event_id: int) -> bytes:
    """Encode a SystemView event ID the way SEGGER_SYSVIEW does (base-128, LSB first)."""
    out = bytearray()
    while True:
        b = event_id & 0x7F
        event_id >>= 7
        out.append(b | 0x80 if event_id else b)
        if not event_id:
            return bytes(out)


def _toolchain_prefix(binary_path: str) -> str:
    with open(os.path.join(binary_path, 'project_description.json')) as f:
        return str(json.load(f)['monitor_toolprefix'])


def _validate_function_trace_manual(trace_log: str) -> None:
    """Fallback validation when no toolchain is available to decode the capture.

    The module description string is not in a JTAG capture (it is recorded before
    recording is enabled and never re-sent), so check for the enter/exit event IDs.
    """
    with open(trace_log, 'rb') as f:
        content = f.read()
    enter = content.count(_encode_event_id(FT_EVENT_ENTER))
    exit_ = content.count(_encode_event_id(FT_EVENT_EXIT))
    assert enter > 0 and exit_ > 0, f'no function enter/exit events in {trace_log} (enter={enter}, exit={exit_})'


def _validate_function_trace(trace_log: str, idf_path: str, elf_file: str, binary_path: str) -> None:
    """Decode function-trace events with sysviewtrace_proc.py and check instrumented
    functions are traced while excluded sources are not. Fall back to counting
    enter/exit packets when the target toolchain is not available."""
    toolchain = _toolchain_prefix(binary_path)
    if shutil.which(f'{toolchain}addr2line') is None:
        print(f'addr2line not found for {toolchain}, using manual validation')
        _validate_function_trace_manual(trace_log)
        return

    proc_script = os.path.join(idf_path, 'tools', 'esp_app_trace', 'sysviewtrace_proc.py')
    result = subprocess.run(
        [sys.executable, proc_script, '-b', elf_file, '-t', toolchain, '-i', 'func', f'file://{trace_log}'],
        capture_output=True,
        text=True,
    )
    report = result.stdout + result.stderr
    assert result.returncode == 0, f'sysviewtrace_proc.py failed:\n{report}'

    print(f'{report}')

    m = re.search(r'Processed (\d+) function trace events\.', report)
    assert m and int(m.group(1)) > 0, f'no function trace events decoded:\n{report}'
    assert INSTRUMENTED_SOURCE in report, f'instrumented functions ({INSTRUMENTED_SOURCE}) not traced:\n{report}'
    for excluded in EXCLUDED_SOURCES:
        assert excluded not in report, f'excluded source {excluded} was traced:\n{report}'


def _validate_trace_data(trace_log: str, target: str, dual_core: bool) -> None:
    """Validate the multi-core capture contains SystemView trace data for each core."""
    with open(trace_log, 'rb') as f:
        content = f.read()
    for idx in range(2 if dual_core else 1):
        search_str = f'N=FreeRTOS Application,D={target},C=core{idx},O=FreeRTOS'.encode()
        assert search_str in content, f'SysView core{idx} trace data not found in {trace_log}'


def _test_function_tracing_jtag(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    # Single multi-core capture file (esp sysview_mcore).
    trace_log = os.path.join(dut.logdir, 'function_tracing.svdat')
    dual_core = not dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') or dut.target == 'esp32s3'

    # Prepare gdbinit file pointing at this run's capture file
    gdb_logfile = os.path.join(dut.logdir, 'gdb.txt')
    gdbinit_orig = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gdbinit')
    gdbinit = os.path.join(dut.logdir, 'gdbinit')
    with open(gdbinit_orig) as f_r, open(gdbinit, 'w') as f_w:
        for line in f_r:
            if line.startswith('mon esp sysview_mcore start'):
                f_w.write(f'mon esp sysview_mcore start file://{trace_log}\n')
            else:
                f_w.write(line)

    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact('function-tracing: Hello from function_tracing example!', timeout=5)
    with (
        openocd_dut.run() as openocd,
        open(gdb_logfile, 'w') as gdb_log,
        pexpect.spawn(
            f'idf.py -B {dut.app.binary_path} gdb --batch -x {gdbinit}',
            timeout=60,
            logfile=gdb_log,
            encoding='utf-8',
            codec_errors='ignore',
        ) as p,
    ):
        p.expect_exact('hit Breakpoint 1, app_main ()')
        # dut has been restarted by gdb since the last dut.expect()
        dut.expect(re.compile(rb'function-tracing: workload iteration \d+'), timeout=30)

        # Let function-trace samples accumulate while recording.
        time.sleep(1)
        openocd.write('esp sysview_mcore stop')
        openocd.apptrace_wait_stop()

    _validate_trace_data(trace_log, dut.target, dual_core)
    _validate_function_trace(trace_log, idf_path, dut.app.elf_file, dut.app.binary_path)


@pytest.mark.jtag
@idf_parametrize('config', ['jtag'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_function_tracing_jtag(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    _test_function_tracing_jtag(openocd_dut, idf_path, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['jtag'], indirect=['config'])
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'),
    indirect=['target'],
)
@idf_parametrize('port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['port'])
def test_function_tracing_usj(openocd_dut: 'OpenOCD', idf_path: str, dut: IdfDut) -> None:
    _test_function_tracing_jtag(openocd_dut, idf_path, dut)
