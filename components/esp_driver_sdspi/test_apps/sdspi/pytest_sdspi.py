# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from pytest_embedded_idf import IdfDut


def test_sdspi(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
