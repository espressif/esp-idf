# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8976
@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize('config', [
    'default',
    'slp_iram_opt',
    'limits',
    'options',
], indirect=True)
def test_esp_pm(dut: Dut) -> None:
    dut.run_all_single_board_cases()
