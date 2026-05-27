# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
def test_psram_stack_example(dut: Dut) -> None:
    dut.expect_exact('Initialising flash dispatcher')

    output = dut.expect_exact('Example complete', timeout=15, return_what_before_match=True).decode(
        'utf-8', errors='ignore'
    )
    for i in range(3):
        assert re.search(rf'worker\[{i}\]: stack @ 0x[0-9a-f]+ +-> PSRAM', output)
        assert re.search(rf'worker\[{i}\]: flash read OK', output)
