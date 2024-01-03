# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging

import pytest
from pytest_embedded import Dut


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8989
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'defaults',
    'always_skip',
], indirect=True)
def test_startup_time_example(dut: Dut) -> None:

    res = dut.expect(r'\((\d+)\) [^:]+: App started!')
    time = int(res[1])

    logging.info(f'[Performance][startup_time]: {time}')
