# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pytest_embedded_idf import IdfDut


def test_isp(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
