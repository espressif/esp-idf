# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8975
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_spi_lcd(dut: Dut) -> None:
    dut.run_all_single_board_cases()
