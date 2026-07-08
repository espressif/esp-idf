# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import re
import time
import typing

import pexpect
import pytest
import serial
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _validate_trace_data(trace_log: str, target: str, dual_core: bool = False, is_uart: bool = False) -> None:
    """Validate SysView trace data in a single trace log file.

    Args:
        trace_log: Path to the trace log file
        target: Target chip name (e.g., 'esp32', 'esp32s3')
        dual_core: If True, expect a per-core description block for both cores
            (the ``esp sysview_mcore`` multi-core capture embeds one per core)
        is_uart: If True, also validate STOP record at end of file
    """
    STOP_EVENT_ID = 0x0B  # SYSVIEW_EVTID_TRACE_STOP

    with open(trace_log, 'rb') as f:
        content = f.read()

    for idx in range(2 if dual_core else 1):
        search_str = f'N=FreeRTOS Application,D={target},C=core{idx},O=FreeRTOS'.encode()
        assert search_str in content, f'SysView core{idx} trace data not found in {trace_log}'

    # The file must end with a TRACE_STOP record: the STOP event ID
    # followed by a variable-length timestamp delta. Walk back
    # over the trailing continuation bytes (0x80 bit set)
    # to find the event ID, since its offset is not fixed.
    size = len(content)
    assert size >= 2, 'Trace file too small to contain STOP record'
    i = size - 2
    while i >= 0 and (content[i] & 0x80):
        i -= 1
    assert i >= 0 and content[i] == STOP_EVENT_ID, 'STOP record does not start with STOP eventID'


def _capture_sysview_trace(ser: serial.Serial, trace_log_path: str) -> None:
    """Capture SysView trace data from serial port.

    Args:
        ser: Serial port instance
        trace_log_path: Path to save the trace log
    """
    START_CMD = b'\x01'
    STOP_CMD = b'\x02'
    STOP_EVENT_ID = 0x0B  # SYSVIEW_EVTID_TRACE_STOP

    ser.reset_input_buffer()
    # Send Start command to start SysView tracing
    ser.write(START_CMD)

    data = bytearray()

    # Capture for 3 seconds
    end_time = time.time() + 3.0
    while time.time() < end_time:
        try:
            if ser.in_waiting:
                data += ser.read(ser.in_waiting)
        except serial.SerialTimeoutException:
            assert False, 'Timeout reached while reading from serial port, exiting...'

    # Give pending trace data a short window to reach the host before requesting STOP.
    time.sleep(0.2)

    # Send Stop command
    ser.write(STOP_CMD)

    # Capture the final data produced by STOP and the transport flush.
    end_time = time.time() + 3.0
    last_data_time = time.time()
    while time.time() < end_time and (time.time() - last_data_time) <= 1.0:
        try:
            if ser.in_waiting:
                data += ser.read(ser.in_waiting)
                last_data_time = time.time()
        except serial.SerialTimeoutException:
            assert False, 'Timeout reached while reading from serial port, exiting...'

    # Drop anything after the TRACE_STOP record (e.g. the ROM boot banner printed
    # on the shared UART when pytest-embedded resets the DUT after the test).
    stop_pos = data.rfind(STOP_EVENT_ID)
    if stop_pos != -1:
        end = stop_pos + 1
        # Consume the timestamp delta: continuation bytes have the 0x80 bit set,
        # terminated by a single byte with 0x80 clear.
        while end < len(data) and (data[end] & 0x80):
            end += 1
        if end < len(data):
            end += 1
        data = data[:end]

    with open(trace_log_path, 'w+b') as f:
        f.write(data)


def _test_sysview_tracing_jtag(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    # Single multi-core capture file (esp sysview_mcore): one file is enough for
    # both single- and dual-core targets.
    trace_log = os.path.join(dut.logdir, 'sys_log.svdat')  # pylint: disable=protected-access
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

    def dut_expect_task_event() -> None:
        dut.expect(re.compile(rb'example: Task\[0x[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact('example: Hello from sysview_tracing example!', timeout=5)
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
        dut.expect('example: Created task')  # dut has been restarted by gdb since the last dut.expect()
        dut_expect_task_event()

        # Do a sleep while sysview samples are captured.
        time.sleep(3)
        openocd.write('esp sysview_mcore stop')

    _validate_trace_data(trace_log, dut.target, dual_core=dual_core)


@pytest.mark.jtag
@idf_parametrize('config', ['sysview_jtag'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_sysview_tracing_jtag(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_sysview_tracing_jtag(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['sysview_jtag'], indirect=['config'])
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'),
    indirect=['target'],
)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
@idf_parametrize('port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['port'])
def test_sysview_tracing_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_sysview_tracing_jtag(openocd_dut, dut)


def _test_sysview_tracing_uart(dut: IdfDut) -> None:
    dut.serial.close()
    time.sleep(2)  # Wait for the DUT to reboot
    with serial.Serial(dut.serial.port, baudrate=dut.app.sdkconfig.get('APPTRACE_UART_BAUDRATE'), timeout=3) as ser:
        trace_log = os.path.join(dut.logdir, 'sys_log_uart.svdat')  # pylint: disable=protected-access
        _capture_sysview_trace(ser, trace_log)
        _validate_trace_data(trace_log, dut.target, is_uart=True)


@pytest.mark.generic
@idf_parametrize('config', ['sysview_uart'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
def test_sysview_tracing_uart(dut: IdfDut) -> None:
    _test_sysview_tracing_uart(dut)


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@idf_parametrize('config', ['sysview_uart_esp32c2_26Mhz'], indirect=['config'])
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_sysview_tracing_uart_c2(dut: IdfDut) -> None:
    _test_sysview_tracing_uart(dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('target', soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.parametrize('config', [pytest.param('sysview_usj')], indirect=True)
@idf_parametrize('port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['port'])
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
def test_sysview_tracing_usj_serial(dut: IdfDut) -> None:
    time.sleep(1)  # wait for USJ port to be ready
    usj_port = '/dev/serial_ports/ttyACM-esp32'
    ser = serial.Serial(usj_port, baudrate=1000000, timeout=10)
    trace_log = os.path.join(dut.logdir, 'sys_log_usj.svdat')  # pylint: disable=protected-access
    _capture_sysview_trace(ser, trace_log)
    _validate_trace_data(trace_log, dut.target, is_uart=True)
