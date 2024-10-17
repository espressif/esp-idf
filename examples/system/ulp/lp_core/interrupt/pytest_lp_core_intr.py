# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


<<<<<<< HEAD
=======
@pytest.mark.esp32c5
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
@pytest.mark.esp32c6
@pytest.mark.esp32p4
@pytest.mark.generic
def test_lp_core_intr(dut: Dut) -> None:
    dut.expect('Triggered 10 interrupts on the LP-Core, LP-Core received 10 interrupts')
