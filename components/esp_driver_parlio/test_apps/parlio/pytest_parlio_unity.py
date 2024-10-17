# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c5
@pytest.mark.esp32c6
<<<<<<< HEAD
=======
@pytest.mark.esp32h2
@pytest.mark.esp32p4
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_parlio(dut: Dut) -> None:
    dut.run_all_single_board_cases()
