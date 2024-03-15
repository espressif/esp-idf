# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os

import pytest
from pytest_embedded import Dut

PROMPT = 'test_intr_dump>'


@pytest.mark.esp32
@pytest.mark.qemu
@pytest.mark.host_test
def test_esp_intr_dump_nonshared(dut: Dut) -> None:
    dut.expect_exact(PROMPT, timeout=10)

    dut.write('intr_alloc GPIO LEVEL3\n')
    dut.expect_exact('Allocated GPIO LEVEL3')
    dut.expect_exact(PROMPT)

    dut.write('intr_dump\n')
    dut.expect(r'(\d+)\s+3\s+Level\s+Used: GPIO')


@pytest.mark.esp32
@pytest.mark.qemu
@pytest.mark.host_test
def test_esp_intr_dump_shared(dut: Dut) -> None:
    dut.expect_exact(PROMPT, timeout=10)

    dut.write('intr_alloc GPIO SHARED\n')
    dut.expect_exact('Allocated GPIO SHARED')
    dut.expect_exact(PROMPT)

    dut.write('intr_dump\n')
    dut.expect(r'(\d+)\s+1\s+Level\s+Shared: GPIO')
    dut.expect_exact(PROMPT)

    dut.write('intr_alloc UART1 SHARED\n')
    dut.expect_exact('Allocated UART1 SHARED')
    dut.expect_exact(PROMPT)

    dut.write('intr_dump\n')
    dut.expect(r'(\d+)\s+1\s+Level\s+Shared: UART1 GPIO')
    dut.expect_exact('Shared interrupts: 1')
    dut.expect_exact(PROMPT)


# TODO: IDF-9512, Update the expected output of dual core RISC-V chips when the issue is resolved
@pytest.mark.supported_targets
@pytest.mark.generic
def test_esp_intr_dump_expected_output(dut: Dut) -> None:
    dut.expect_exact(PROMPT, timeout=10)
    dut.write('intr_dump\n')
    exp_out_file = os.path.join(os.path.dirname(__file__), 'expected_output', f'{dut.target}.txt')
    for line in open(exp_out_file, 'r').readlines():
        dut.expect_exact(line.strip())
