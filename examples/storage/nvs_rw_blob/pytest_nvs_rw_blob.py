# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import logging
import random
import re
import time
from typing import List

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
def test_examples_nvs_rw_blob(dut: Dut) -> None:
    def expect_start_msg(index: int) -> None:
        dut.expect('Restart counter = {}'.format(index), timeout=10)
        dut.expect('Run time:', timeout=10)

    expect_start_msg(0)
    dut.expect('Nothing saved yet!', timeout=5)
    nvs_store: List[str] = []
    for i in range(1, 10):
        time.sleep(random.uniform(0.1, 2))  # in order to randomize the runtimes stored in NVS
        try:
            # Pulling pin low using DTR
            dut.serial.proc.setDTR(True)
            dut.expect('Restarting...', timeout=5)  # the application waits for a second
        finally:
            dut.serial.proc.setDTR(False)

        expect_start_msg(i)

        for store_item in nvs_store:
            dut.expect(store_item.encode(), timeout=10)

        logging.info('Received: {}'.format(', '.join(nvs_store)))
        new_runtime = (dut.expect(re.compile(str.encode('{}: (\\d+)'.format(i))), timeout=10)[0]).decode()
        nvs_store.append(new_runtime)
        logging.info('loop {} has finished with runtime {}'.format(i, new_runtime))
