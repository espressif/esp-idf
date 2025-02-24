# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import time

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_jtag import OpenOcd


def apptrace_wait_stop(openocd: OpenOcd, timeout: int = 30) -> None:
    stopped = False
    end_before = time.time() + timeout
    while not stopped:
        cmd_out = openocd.write('esp apptrace status')
        for line in cmd_out.splitlines():
            if line.startswith('Tracing is STOPPED.'):
                stopped = True
                break
        if not stopped and time.time() > end_before:
            raise pexpect.TIMEOUT('Failed to wait for apptrace stop!')

        time.sleep(1)


@idf_parametrize('embedded_services', ['esp,idf,jtag'], indirect=['embedded_services'])
@idf_parametrize('no_gdb', ['y'], indirect=['no_gdb'])
@idf_parametrize(
    'openocd_cli_args,port,target,markers',
    [
        (None, None, 'esp32', (pytest.mark.jtag,)),
        ('-f board/esp32s2-kaluga-1.cfg', None, 'esp32s2', (pytest.mark.jtag,)),
        ('-f board/esp32c2-ftdi.cfg', None, 'esp32c2', (pytest.mark.jtag,)),
        ('-f board/esp32s3-builtin.cfg', '/dev/serial_ports/ttyACM-esp32', 'esp32s3', (pytest.mark.usb_serial_jtag,)),
        ('-f board/esp32c3-builtin.cfg', '/dev/serial_ports/ttyACM-esp32', 'esp32c3', (pytest.mark.usb_serial_jtag,)),
    ],
    indirect=['openocd_cli_args', 'port', 'target'],
)
def test_examples_app_trace_basic(dut: IdfDut, openocd: OpenOcd) -> None:
    dut.openocd.write('reset')
    dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
    assert 'Targets connected.' in dut.openocd.write('esp apptrace start file://apptrace.log 0 2000 3 0 0')
    apptrace_wait_stop(dut.openocd)

    with open(openocd._logfile, encoding='utf-8') as oocd_log:  # pylint: disable=protected-access
        cores = 1 if dut.app.sdkconfig.get('ESP_SYSTEM_SINGLE_CORE_MODE') is True else 2
        params_str = 'App trace params: from {} cores,'.format(cores)
        found = False
        for line in oocd_log:
            if params_str in line:
                found = True
                break
        if found is not True:
            raise RuntimeError(
                '"{}" could not be found in {}'.format(params_str, openocd._logfile)  # pylint: disable=protected-access
            )

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
