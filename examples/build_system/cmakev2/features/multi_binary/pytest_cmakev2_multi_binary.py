# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from pytest_embedded_idf.app import FlashFile
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cmakev2_multi_binary(dut: IdfDut) -> None:
    # app1 is auto-flashed by the DUT fixture via flasher_args.json
    dut.expect_exact('Hello from component1!')
    dut.expect_exact('Hello from component2!')

    # Re-flash only the app partition with app2
    app_offset = int(dut.app.flash_args['app1']['offset'], 16)
    saved = dut.app.flash_files
    dut.app.flash_files = [FlashFile(app_offset, os.path.join(dut.app.binary_path, 'app2.bin'))]
    dut.serial.flash()
    dut.app.flash_files = saved

    dut.expect_exact('Hello from component1!')
    dut.expect_exact('Hello from component2!')
    dut.expect_exact('Hello from component3!')
