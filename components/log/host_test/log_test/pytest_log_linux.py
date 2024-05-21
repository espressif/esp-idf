# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.linux
@pytest.mark.host_test
@pytest.mark.parametrize('config', [
    'default',
    'system_timestamp'
    'tag_level_linked_list',
    'tag_level_linked_list_and_array_cache',
    'tag_level_none',
], indirect=True)
def test_log_linux(dut: Dut) -> None:
    dut.expect_exact('All tests passed', timeout=5)
