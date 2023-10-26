# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import os
from typing import List

import pytest
from pytest_embedded import Dut


def get_socket_msgs(i: int) -> List[str]:
    msg = 'Socket message S{}'.format(i)
    return ['uart_select_example: {} bytes were written to socket: {}'.format(len(msg), msg),
            'uart_select_example: {} bytes were received through socket: {}'.format(len(msg), msg)]


def get_uart_msgs(i: int) -> List[str]:
    msg = 'UART message U{}'.format(i)
    return ['uart_select_example: {} bytes were sent to UART1: {}'.format(len(msg), msg),
            'uart_select_example: {} bytes were received through UART1: {}'.format(len(msg), msg)]


@pytest.mark.supported_targets
@pytest.mark.generic
def test_examples_select(dut: Dut) -> None:

    dut.expect_exact('main_task: Calling app_main()', timeout=30)

    exp_list = []
    for i in range(1, 10):
        exp_list += get_socket_msgs(i)
        exp_list += get_uart_msgs(i)

    logging.info('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list)))
    dut.expect(exp_list, timeout=60, expect_all=True)
