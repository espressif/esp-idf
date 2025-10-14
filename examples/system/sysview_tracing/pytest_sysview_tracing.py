# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_sysview_tracing_jtag(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    # Construct trace log paths
    trace_log = [
        os.path.join(dut.logdir, 'sys_log0.svdat')  # pylint: disable=protected-access
    ]
    if not dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') or dut.target == 'esp32s3':
        trace_log.append(os.path.join(dut.logdir, 'sys_log1.svdat'))  # pylint: disable=protected-access
    trace_files = ' '.join([f'file://{log}' for log in trace_log])

    # Prepare gdbinit file
    gdb_logfile = os.path.join(dut.logdir, 'gdb.txt')
    gdbinit_orig = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'gdbinit')
    gdbinit = os.path.join(dut.logdir, 'gdbinit')
    with open(gdbinit_orig) as f_r, open(gdbinit, 'w') as f_w:
        for line in f_r:
            if line.startswith('mon esp sysview start'):
                f_w.write(f'mon esp sysview start {trace_files}\n')
            else:
                f_w.write(line)

    def dut_expect_task_event() -> None:
        dut.expect(re.compile(rb'example: Task\[0x[0-9A-Fa-f]+\]: received event \d+'), timeout=30)

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
        openocd.write('esp sysview stop')


@pytest.mark.jtag
@idf_parametrize('config', ['sysview_jtag'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_sysview_tracing_jtag(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_sysview_tracing_jtag(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['sysview_jtag'], indirect=['config'])
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_sysview_tracing_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_sysview_tracing_jtag(openocd_dut, dut)


def _test_sysview_tracing_uart(dut: IdfDut) -> None:
    dut.serial.close()
    time.sleep(2)  # Wait for the DUT to reboot
    with serial.Serial(dut.serial.port, baudrate=dut.app.sdkconfig.get('APPTRACE_UART_BAUDRATE'), timeout=10) as ser:
        trace_log = os.path.join(dut.logdir, 'sys_log_uart.svdat')  # pylint: disable=protected-access
        # Send Start command to start SysView tracing
        ser.write(b'\x01')
        with open(trace_log, 'w+b') as f:
            start_time = time.time()
            while True:
                try:
                    if ser.in_waiting:
                        data = ser.read(1024)
                        f.write(data)
                    if time.time() - start_time > 3:
                        break
                except serial.SerialTimeoutException:
                    assert False, 'Timeout reached while reading from serial port, exiting...'

            # Send Stop command
            ser.write(b'\x02')

            f.seek(0)
            content = f.read()
            search_str = f'N=FreeRTOS Application,D={dut.target},C=core0,O=FreeRTOS'.encode()
            assert search_str in content, 'SysView trace data not found in the log file'


@pytest.mark.generic
@idf_parametrize('config', ['sysview_uart'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_sysview_tracing_uart(dut: IdfDut) -> None:
    _test_sysview_tracing_uart(dut)


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@idf_parametrize('config', ['sysview_uart_esp32c2_26Mhz'], indirect=['config'])
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_sysview_tracing_uart_c2(dut: IdfDut) -> None:
    _test_sysview_tracing_uart(dut)
