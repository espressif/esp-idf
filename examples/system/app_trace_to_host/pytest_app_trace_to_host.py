# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import os
import re
import time

import pexpect
import pytest
from pytest_embedded.log import DuplicateStdoutPopen, MessageQueue
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


@pytest.mark.esp32
@pytest.mark.jtag
@pytest.mark.parametrize(
    'embedded_services, no_gdb',
    [
        ('esp,idf,jtag', 'y'),
    ],
    indirect=True,
)
def test_examples_app_trace_to_host(msg_queue: MessageQueue, dut: IdfDut, openocd: OpenOcd, idf_path: str) -> None:
    dut.expect_exact('example: Enabling ADC1 on channel 6 / GPIO34.')
    dut.expect_exact('example: Enabling CW generator on DAC channel 0 / GPIO25')
    dut.expect_exact('example: Sampling ADC and sending data to the host...')
    dut.expect(r'example: Collected \d+ samples in 20 ms.')
    dut.expect_exact('example: Sampling ADC and sending data to the UART...')
    dut.expect(r'example: Sample:\d, Value:\d+')
    dut.expect(r'example: Collected \d+ samples in 20 ms.')

    assert 'Targets connected.' in dut.openocd.write('esp apptrace start file://adc.log 0 9000 5 0 0')
    apptrace_wait_stop(dut.openocd)

    with open(openocd._logfile) as oocd_log:  # pylint: disable=protected-access
        cores = 1 if dut.app.sdkconfig.get('CONFIG_FREERTOS_UNICORE') == 'y' else 2
        params_str = 'App trace params: from {} cores'.format(cores)
        for line in oocd_log:
            if params_str in line:
                break
        else:
            raise RuntimeError(
                '"{}" could not be found in {}'.format(params_str, openocd._logfile)  # pylint: disable=protected-access
            )

    DuplicateStdoutPopen(
        msg_queue,
        [
            os.path.join(idf_path, 'tools', 'esp_app_trace', 'logtrace_proc.py'),
            'adc.log',
            os.path.join(dut.app.elf_file),
        ],
    )

    dut.expect_exact('Parse trace file')
    dut.expect_exact('Parsing completed.')
    dut.expect_exact('====================================================================')
    dut.expect(re.compile(rb'example: Sample:\d+, Value:\d+'))
    dut.expect_exact('====================================================================')
    dut.expect(re.compile(rb'Log records count: \d+'))
