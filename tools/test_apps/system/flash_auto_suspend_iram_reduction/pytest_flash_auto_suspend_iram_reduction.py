# SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.flash_suspend
@idf_parametrize('config', ['default', 'rtc_clk'], indirect=['config'])
@idf_parametrize(
    'target,markers',
    [
        (
            target,
            (
                pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='flash clock lose in startup'),  # TODO: IDF-15212
                pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runners'),  # TODO: IDF-16185
                pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runners'),  # TODO: IDFCI-11111
                pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='lack of runners'),  # TODO: IDF-15531
            ),
        )
        for target in soc_filtered_targets('SOC_SPI_MEM_SUPPORT_AUTO_SUSPEND == 1')
    ],
    indirect=['target'],
)
def test_flash_auto_suspend_minimize_iram_usage(dut: Dut) -> None:
    dut.expect_exact('Hello World!', timeout=30)
