# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import logging
import os

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_eventfd(dut: Dut) -> None:

    dut.expect_exact('main_task: Calling app_main()')

    exp_list_5seconds = [
        'eventfd_example: Select timeouted for 1 times',
        'eventfd_example: Timer triggered for 2 times',
        'eventfd_example: Progress triggered for 1 times',
    ]

    exp_list_10seconds = [
        'eventfd_example: Select timeouted for 2 times',
        'eventfd_example: Timer triggered for 4 times',
        'eventfd_example: Progress triggered for 2 times',
    ]

    logging.info('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list_5seconds)))
    for exp in exp_list_5seconds:
        dut.expect_exact(exp)

    logging.info('Expecting:{}{}'.format(os.linesep, os.linesep.join(exp_list_10seconds)))
    for exp in exp_list_10seconds:
        dut.expect_exact(exp)
