# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import time

import pexpect
import pytest
from pytest_embedded_idf import IdfDut
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


@pytest.mark.parametrize(
    'embedded_services, no_gdb',
    [
        ('esp,idf,jtag', 'y'),
    ],
    indirect=True,
)
@pytest.mark.parametrize(
    'port, openocd_cli_args', [
        pytest.param(None, None, marks=[pytest.mark.esp32, pytest.mark.jtag]),
        pytest.param(None, '-f board/esp32s2-kaluga-1.cfg', marks=[pytest.mark.esp32s2, pytest.mark.jtag]),
        pytest.param(None, '-f board/esp32c2-ftdi.cfg', marks=[pytest.mark.esp32c2, pytest.mark.jtag]),
        pytest.param('/dev/serial_ports/ttyACM-esp32', '-f board/esp32s3-builtin.cfg', marks=[pytest.mark.esp32s3, pytest.mark.usb_serial_jtag]),
        pytest.param('/dev/serial_ports/ttyACM-esp32', '-f board/esp32c3-builtin.cfg', marks=[pytest.mark.esp32c3, pytest.mark.usb_serial_jtag]),
    ],
    indirect=True
)
def test_examples_app_trace_basic(dut: IdfDut, openocd: OpenOcd) -> None:
    dut.openocd.write('reset')
    dut.expect_exact('example: Waiting for OpenOCD connection', timeout=5)
    assert 'Targets connected.' in dut.openocd.write('esp apptrace start file://apptrace.log 0 2000 3 0 0')
    apptrace_wait_stop(dut.openocd)

    with open(openocd._logfile) as oocd_log:  # pylint: disable=protected-access
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

    with open('apptrace.log') as apptrace_log:
        for sample_num in range(1, 51):
            log_str = 'Apptrace test data[{}]:{}'.format(sample_num, sample_num * sample_num)
            found = False
            for line in apptrace_log:
                if log_str in line:
                    found = True
                    break
            if found is not True:
                raise RuntimeError(
                    '"{}" could not be found in {}'.format(log_str, 'apptrace.log')
                )
