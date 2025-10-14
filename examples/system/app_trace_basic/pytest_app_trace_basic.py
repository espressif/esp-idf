# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import time
import typing

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

if typing.TYPE_CHECKING:
    from conftest import OpenOCD


def _test_examples_app_trace_basic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
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
            search_strings.append('App trace params: from {} cores,'.format(cores))
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
            for sample_num in range(1, 51):
                log_str = 'Apptrace test data[{}]:{}'.format(sample_num, sample_num * sample_num)
                found = False
                for line in apptrace_log:
                    if log_str in line:
                        found = True
                        break
                if found is not True:
                    raise RuntimeError('"{}" could not be found in {}'.format(log_str, 'apptrace.log'))


@pytest.mark.jtag
@idf_parametrize('target', ['esp32', 'esp32c2', 'esp32s2'], indirect=['target'])
def test_examples_app_trace_basic(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_app_trace_basic(openocd_dut, dut)


@pytest.mark.usb_serial_jtag
@idf_parametrize(
    'target', ['esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'], indirect=['target']
)
def test_examples_app_trace_basic_usj(openocd_dut: 'OpenOCD', dut: IdfDut) -> None:
    _test_examples_app_trace_basic(openocd_dut, dut)
