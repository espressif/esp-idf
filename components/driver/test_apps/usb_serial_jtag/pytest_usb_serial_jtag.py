# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.parametrize('config', ['release',], indirect=True,)
def test_usb_serial_jtag_single_dev(case_tester) -> None:                # type: ignore
    case_tester.run_all_normal_cases(reset=True)
