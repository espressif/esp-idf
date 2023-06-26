# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.esp32
@pytest.mark.sdio_master_slave
@pytest.mark.parametrize('count', [2,], indirect=True)
def test_sdio_multi_dev(case_tester) -> None:        # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True, timeout=180)
