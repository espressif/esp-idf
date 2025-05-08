# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_sdm_dac_example(dut: Dut) -> None:
    dut.expect(r'sdm_dac: Sigma-delta output is attached to GPIO \w+')
    dut.expect(r'sdm_dac: Timer allocated with resolution \w+ Hz')
    dut.expect(r'sdm_dac: Timer callback registered, interval \w+ us')
    dut.expect_exact('sdm_dac: Timer enabled')
    dut.expect_exact('sdm_dac: Output start')
