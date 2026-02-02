# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os.path
import time
import typing

import pytest
import serial
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_examples_app_trace_basic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    time.sleep(1)  # Wait for the USJ port to be ready
    dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
    with openocd_dut.run() as openocd:
        openocd.write('reset run')
        dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
        time.sleep(1)  # wait for APPTRACE_INIT semihosting call
        openocd.write('esp apptrace start file://apptrace.log 0 2000 3 0 0')
        openocd.apptrace_wait_stop()

        search_strings = ['Targets connected.', 'Disconnect targets...']
        with open(openocd.log_file, encoding='utf-8') as oocd_log:  # pylint: disable=protected-access
            cores = 1 if dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') is True else 2
            search_strings.append(f'App trace params: from {cores} cores,')
            for search_str in search_strings:
                found = False
                oocd_log.seek(0)
                for line in oocd_log:
                    if search_str in line:
                        found = True
                        break
                if found is not True:
                    raise RuntimeError(f'"{search_str}" could not be found in {openocd.log_file}')  # pylint: disable=protected-access

        with open('apptrace.log', encoding='utf-8') as apptrace_log:
            content = apptrace_log.read()
            for sample_num in range(1, 51):
                log_str = f'Apptrace test data[{sample_num}]:{sample_num * sample_num}'
                if log_str not in content:
                    raise RuntimeError('"{}" could not be found in {}'.format(log_str, 'apptrace.log'))


@pytest.mark.jtag
@idf_parametrize('config', ['apptrace_jtag'], indirect=['config'])
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_app_trace_basic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_app_trace_basic(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['apptrace_jtag'], indirect=['config'])
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_examples_app_trace_basic_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_app_trace_basic(openocd_dut, dut)


@pytest.mark.generic
@idf_parametrize('config', ['apptrace_uart'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_examples_app_trace_basic_uart(dut: IdfDut) -> None:
    dut.serial.close()
    with serial.Serial(dut.serial.port, baudrate=1000000, timeout=3) as ser:
        apptrace_log = os.path.join(dut.logdir, 'apptrace_log_uart.txt')  # pylint: disable=protected-access
        with open(apptrace_log, 'w+b') as f:
            start_time = time.time()
            while True:
                try:
                    if ser.in_waiting:
                        f.write(ser.read(ser.in_waiting))
                    if time.time() - start_time > 5:
                        break
                except serial.SerialTimeoutException:
                    assert False, 'Timeout reached while reading from serial port, exiting...'

            f.seek(0)
            content = f.read().decode('utf-8', errors='ignore')
            for sample_num in range(1, 51):
                log_str = f'Apptrace test data[{sample_num}]:{sample_num * sample_num}'
                if log_str not in content:
                    raise RuntimeError('"{}" could not be found in {}'.format(log_str, 'apptrace_log_uart.txt'))
