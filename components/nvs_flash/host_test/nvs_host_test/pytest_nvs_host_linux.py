# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.linux
@pytest.mark.host_test
@pytest.mark.parametrize('config', [
    'default_set_key', 'legacy_set_key'
], indirect=True)
def test_nvs_host_linux(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=60)
