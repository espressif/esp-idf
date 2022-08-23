# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import logging
from itertools import zip_longest

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
def test_examples_nvs_rw_value(dut: Dut) -> None:
    for i, counter_state in zip_longest(range(4), ('The value is not initialized yet!',), fillvalue='Done'):
        dut.expect('Opening Non-Volatile Storage \\(NVS\\) handle... Done', timeout=20)
        dut.expect('Reading restart counter from NVS ... {}'.format(counter_state), timeout=20)
        dut.expect('Restart counter = {}'.format(i) if i > 0 else '', timeout=20)
        dut.expect('Updating restart counter in NVS ... Done', timeout=20)
        dut.expect('Committing updates in NVS ... Done', timeout=20)
        dut.expect('Restarting in 10 seconds...', timeout=20)
        logging.info('loop {} has finished'.format(i))
