# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_sdm_dac_example(dut: Dut) -> None:
    dut.expect(r'sdm_dac: Sigma-delta output is attached to GPIO \w+')
    dut.expect(r'sdm_dac: Timer allocated with resolution \w+ Hz')
    dut.expect(r'sdm_dac: Timer callback registered, interval \w+ us')
    dut.expect_exact('sdm_dac: Timer enabled')
    dut.expect_exact('sdm_dac: Output start')
