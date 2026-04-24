# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32c5'], indirect=['target']
)
def test_crypto_drivers(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
