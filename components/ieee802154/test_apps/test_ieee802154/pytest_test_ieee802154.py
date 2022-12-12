# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32h4
@pytest.mark.ieee802154
@pytest.mark.parametrize(
    'count, config, beta_target', [
        (2, 'release', 'esp32h2beta2'),     # No need to rename beta_target as it is still called h2 in esptool
    ], indirect=True
)
def test_based_txrx(dut: Tuple[IdfDut, IdfDut]) -> None:
    transmit = dut[0]
    receive = dut[1]
    receive.expect('ieee802154>', timeout=60)
    receive.write('rx -r 1')
    receive.expect('RX Start', timeout=60)
    transmit.expect('ieee802154>', timeout=60)
    transmit.write('tx -l 10')
    transmit.expect('Tx Done 10 bytes', timeout=60)
    transmit.expect('00 01 02 03 04 05 06 07', timeout=60)
    transmit.expect('08 09 00 00 00 00 00 00', timeout=60)
    receive.expect('Rx Done 10 bytes', timeout=60)
    receive.expect('00 01 02 03 04 05 06 07', timeout=60)
