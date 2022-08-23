# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded.dut import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c2
def test_adc_oneshot(dut: Dut) -> None:
    dut.expect(r'ADC_ONESHOT: ADC1 Channel\[(\d+)]\ Raw Data: (\d+)', timeout=5)
