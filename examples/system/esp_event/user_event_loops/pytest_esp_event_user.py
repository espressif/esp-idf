# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging

import pytest
from pytest_embedded import Dut

TASK_ITERATION_LIMIT = 10

TASK_ITERATION_POSTING = 'posting TASK_EVENTS:TASK_ITERATION_EVENT to {}, iteration {} out of ' + str(TASK_ITERATION_LIMIT)
TASK_ITERATION_HANDLING = 'handling TASK_EVENTS:TASK_ITERATION_EVENT from {}, iteration {}'


@pytest.mark.supported_targets
@pytest.mark.generic
def test_esp_event_user_event_loops(dut: Dut) -> None:

    dut.expect_exact('setting up')
    dut.expect_exact('starting event source')
    dut.expect_exact('starting application task')

    for iteration in range(1, TASK_ITERATION_LIMIT + 1):
        loop = None

        if (iteration % 2 == 0):
            loop = 'loop_with_task'
        else:
            loop = 'loop_without_task'

        dut.expect(TASK_ITERATION_POSTING.format(loop, iteration))
        logging.info('Posted iteration {} to {}'.format(iteration, loop))
        dut.expect(TASK_ITERATION_HANDLING.format(loop, iteration))
        logging.info('Handled iteration {} from {}'.format(iteration, loop))

    dut.expect('deleting task event source')
    logging.info('Deleted task event source')
