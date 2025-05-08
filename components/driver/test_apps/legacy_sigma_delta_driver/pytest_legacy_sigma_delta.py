# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'],
    indirect=['target'],
)
def test_legacy_sigma_delta(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sigma_delta')
