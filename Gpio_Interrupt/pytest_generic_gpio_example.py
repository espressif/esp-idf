# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from typing import Callable

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_generic_gpio_example(
    dut: Dut, log_minimum_free_heap_size: Callable[..., None]
) -> None:
    log_minimum_free_heap_size()
    dut.expect(r'cnt: \d+')
