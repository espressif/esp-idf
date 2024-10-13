# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# # SPDX-License-Identifier: CC0-1.0
import logging
import os
import re
import signal

import pexpect
import pytest
from pytest_embedded_idf import IdfDut


def run_gdb_test(dut: IdfDut) -> None:
    with open(os.path.join(dut.logdir, 'ocd.txt'), 'w') as ocd_log, \
        pexpect.spawn(f'openocd -f board/esp32c6-builtin.cfg',
                      timeout=60,
                      logfile=ocd_log,
                      encoding='utf-8',
                      codec_errors='ignore') as p:
        try:
            p.expect(re.compile(r'JTAG tap: esp32c6.cpu tap/device found'), timeout=5)
            logging.info('JTAG is enabled.')

        except pexpect.TIMEOUT:
            logging.info('JTAG is disabled')

        finally:
            p.terminate()
            p.kill(signal.SIGKILL)


@pytest.mark.esp32c6
@pytest.mark.jtag_re_enable
def test_jtag_re_enable(dut: IdfDut) -> None:

    dut.expect_exact('esp32c6>', timeout=30)

    logging.info('Initially:')
    run_gdb_test(dut)

    logging.info('After calling enable_jtag:')
    # The following token data is generated using the HMAC key:
    # {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32}
    dut.write('enable_jtag b2a49b1cce1be922bb7e431277413e3e8e6c3e8e6e17625c50ac66a9a857949b')
    dut.expect('JTAG re-enablement workflow performed', timeout=30)
    run_gdb_test(dut)

    logging.info('After calling disable_jtag:')
    dut.write('disable_jtag')
    dut.expect('JTAG disabled temporarily', timeout=30)
    run_gdb_test(dut)
